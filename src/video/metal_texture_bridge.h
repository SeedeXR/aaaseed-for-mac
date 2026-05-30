// src/video/metal_texture_bridge.h
//
// second_todo.md S2 (c153) : Video -- AVFoundation -> Metal ZERO-COPY.
//
// THE headline feature of the native macOS VIDEO sub-library : a
// zero-copy bridge from a decoded / captured CVPixelBuffer to an
// id<MTLTexture>. On Apple-Silicon the CPU and GPU share one physical
// pool of unified memory, so an IOSurface-backed CVPixelBuffer and the
// MTLTexture wrapping it point at the SAME bytes. There is NO CPU
// readback, NO staging buffer, NO blit : CVMetalTextureCache hands the
// GPU a texture view directly over the pixel-buffer's IOSurface. This
// is the cheapest possible decode/capture -> sampler path and the whole
// reason this sub-lib exists.
//
// Rationale (unified memory) :
//   decode/capture --> CVPixelBuffer (IOSurface-backed, Metal compatible)
//                  --> CVMetalTextureCacheCreateTextureFromImage
//                  --> CVMetalTextureRef (transient binding object)
//                  --> CVMetalTextureGetTexture -> id<MTLTexture>
//   The MTLTexture aliases the IOSurface pages ; on unified memory those
//   pages never leave DRAM and are visible to both the AVFoundation
//   producer and the Metal shader consumer with zero copies.
//
// Hermetic Mac sub-lib doctrine (feedback_hermetic_mac_sublibs.md) :
//   - Pure ObjC++ (.mm). std:: + CoreVideo + Metal + Foundation only.
//   - No engine link, no o_str, no aaa_mem, no vendor includes.
//   - Manual reference counting (-fno-objc-arc) : CFRelease what we
//     create, retain/release ObjC objects we own.
//   - ASCII-only, English-only comments. House style mirrors
//     src/syphon/syphon_mac.h ; the CVMetalTextureCache idiom is the
//     analogue of the IOSurface->MTLTexture bridge in
//     src/syphon/syphon_client_mac.mm consume_metal_texture().
//
// Verified SDK signatures (MacOSX.sdk CoreVideo Headers) :
//   CVMetalTextureCacheCreate(CFAllocatorRef, CFDictionaryRef,
//       id<MTLDevice>, CFDictionaryRef, CVMetalTextureCacheRef*) -> CVReturn
//       (CVMetalTextureCache.h:64)
//   CVMetalTextureCacheCreateTextureFromImage(CFAllocatorRef,
//       CVMetalTextureCacheRef, CVImageBufferRef, CFDictionaryRef,
//       MTLPixelFormat, size_t width, size_t height, size_t planeIndex,
//       CVMetalTextureRef*) -> CVReturn  (CVMetalTextureCache.h:110)
//   CVMetalTextureGetTexture(CVMetalTextureRef) -> id<MTLTexture>
//       (CVMetalTexture.h:53)
//   CVMetalTextureCacheFlush(CVMetalTextureCacheRef, CVOptionFlags) -> void
//       (CVMetalTextureCache.h:128)

#pragma once

#include <cstddef>

//	CoreVideo opaque CF types. Forward-declared so a plain C++ TU can
//	hold the handle without pulling in <CoreVideo/CoreVideo.h>. The .mm
//	impl includes the real headers.
typedef struct __CVBuffer *           CVPixelBufferRef;
typedef struct __CVMetalTextureCache * CVMetalTextureCacheRef;

#ifdef __OBJC__
@protocol MTLDevice;
@protocol MTLTexture;
#else
//	C++-only TUs see id<MTLDevice> / id<MTLTexture> as opaque pointer
//	handles. The .mm impl casts back to the protocol types at the
//	ObjC boundary.
typedef void * MTLDeviceHandle;
typedef void * MTLTextureHandle;
#endif

namespace aaa
{
namespace video
{

//	MetalTextureBridge -- one CVMetalTextureCache bound to one MTLDevice.
//	Wraps BGRA8 CVPixelBuffers as MTLTextures with no CPU copy.
//
//	Threading : single producer/consumer model. Construct, call
//	texture_from_pixel_buffer(), and flush() on the same thread (the
//	render thread). Not internally synchronized.
//
//	Lifetime / ownership model (see texture_from_pixel_buffer doc) :
//	  - The bridge OWNS the CVMetalTextureCacheRef ; the dtor releases it.
//	  - The bridge does NOT retain the caller's CVPixelBuffer ; the caller
//	    keeps it alive for as long as the returned texture is in use.
//	  - The transient CVMetalTextureRef produced per call is held alive
//	    internally until the NEXT texture_from_pixel_buffer() or flush().
class MetalTextureBridge
{
public:
#ifdef __OBJC__
    //	Create a texture cache for `device`. On failure (nil device or
    //	CVMetalTextureCacheCreate != kCVReturnSuccess) is_valid() returns
    //	false and texture_from_pixel_buffer() always returns nil.
    explicit MetalTextureBridge( id<MTLDevice> device );
#else
    explicit MetalTextureBridge( MTLDeviceHandle device );
#endif

    ~MetalTextureBridge();

    MetalTextureBridge( MetalTextureBridge const & )             = delete;
    MetalTextureBridge & operator=( MetalTextureBridge const & ) = delete;

    //	True once the cache was created successfully.
    bool is_valid() const { return cache_ != nullptr; }

#ifdef __OBJC__
    //	Wrap a BGRA8 (kCVPixelFormatType_32BGRA) CVPixelBuffer as an
    //	id<MTLTexture> of format MTLPixelFormatBGRA8Unorm with ZERO CPU
    //	copy. The pixel buffer MUST be IOSurface-backed / Metal-compatible
    //	(buffers from AVPlayerItemVideoOutput / AVCaptureVideoDataOutput
    //	and from CVPixelBufferCreate with kCVPixelBufferMetalCompatibilityKey
    //	are). Returns nil if the bridge is invalid, the input is NULL, or
    //	CVMetalTextureCacheCreateTextureFromImage fails.
    //
    //	Lifetime of the returned texture : it aliases the pixel-buffer's
    //	IOSurface bytes. It stays valid until the NEXT call to
    //	texture_from_pixel_buffer() or flush() on this bridge (those
    //	release the internally-held transient CVMetalTextureRef), and only
    //	while the caller keeps the source CVPixelBuffer alive. To use a
    //	texture across multiple frames, retain it yourself.
    id<MTLTexture> texture_from_pixel_buffer( CVPixelBufferRef pixel_buffer );
#else
    MTLTextureHandle texture_from_pixel_buffer( CVPixelBufferRef pixel_buffer );
#endif

    //	Release the transient CVMetalTextureRef from the last call and let
    //	the cache recycle internal resources (CVMetalTextureCacheFlush).
    //	Call once per frame after the GPU has consumed the texture.
    void flush();

private:
    void release_current_texture();

    CVMetalTextureCacheRef cache_           { nullptr };
    //	Transient per-frame CVMetalTextureRef (a CVImageBufferRef typedef).
    //	Held as void* so the header stays C++-clean ; the .mm casts it back
    //	to CVMetalTextureRef. Released on the next call / flush() / dtor.
    void *                 current_texture_ { nullptr };
};

}   //	namespace video
}   //	namespace aaa
