// src/video/metal_texture_bridge.mm
//
// second_todo.md S2 (c153) : zero-copy CVPixelBuffer -> MTLTexture.
// See metal_texture_bridge.h for the unified-memory rationale and the
// full lifetime / ownership model. Pure ObjC++, manual reference
// counting (-fno-objc-arc).

#import "src/video/metal_texture_bridge.h"

#import <CoreVideo/CoreVideo.h>
#import <Metal/Metal.h>

namespace aaa
{
namespace video
{

MetalTextureBridge::MetalTextureBridge( id<MTLDevice> device )
{
    if( device == nil )
        return;

    //	Create the cache. allocator/cacheAttributes/textureAttributes are
    //	all optional (NULL) -- defaults are fine for the BGRA8 zero-copy
    //	case. cacheOut is CV_RETURNS_RETAINED : we own the +1 and release
    //	it in the dtor.
    CVMetalTextureCacheRef cache = nullptr;
    CVReturn const rc = CVMetalTextureCacheCreate(
        kCFAllocatorDefault,
        nullptr,
        device,
        nullptr,
        &cache );
    if( rc == kCVReturnSuccess )
        cache_ = cache;
    //	On failure CoreVideo leaves cache untouched (nullptr) ; nothing to
    //	release. is_valid() stays false.
}

MetalTextureBridge::~MetalTextureBridge()
{
    release_current_texture();
    if( cache_ != nullptr )
    {
        //	The cache is a CF object we created (+1). CFRelease it.
        CFRelease( cache_ );
        cache_ = nullptr;
    }
}

void MetalTextureBridge::release_current_texture()
{
    if( current_texture_ != nullptr )
    {
        //	current_texture_ holds a +1-retained CVMetalTextureRef
        //	(== CVImageBufferRef, a CF type). Release our reference ; the
        //	underlying MTLTexture we vended earlier is no longer kept
        //	alive by us once this drops.
        CFRelease( (CVMetalTextureRef) current_texture_ );
        current_texture_ = nullptr;
    }
}

id<MTLTexture> MetalTextureBridge::texture_from_pixel_buffer(
    CVPixelBufferRef pixel_buffer )
{
    if( cache_ == nullptr || pixel_buffer == nullptr )
        return nil;

    //	Release the previous frame's transient CVMetalTextureRef. This is
    //	the documented lifetime boundary : the texture vended by the
    //	prior call becomes invalid here.
    release_current_texture();

    size_t const width  = CVPixelBufferGetWidth( pixel_buffer );
    size_t const height = CVPixelBufferGetHeight( pixel_buffer );
    if( width == 0 || height == 0 )
        return nil;

    //	Create a CVMetalTextureRef bound to the pixel buffer's IOSurface.
    //	planeIndex 0 : BGRA is single-plane. The returned cv_tex is
    //	CV_RETURNS_RETAINED -- we own the +1 and hold it in
    //	current_texture_ until the next call / flush() / dtor. Holding it
    //	is REQUIRED : per the SDK header, releasing the CVMetalTexture
    //	signals CoreVideo it may recycle the underlying image, which would
    //	invalidate the MTLTexture we hand back.
    CVMetalTextureRef cv_tex = nullptr;
    CVReturn const rc = CVMetalTextureCacheCreateTextureFromImage(
        kCFAllocatorDefault,
        cache_,
        (CVImageBufferRef) pixel_buffer,
        nullptr,
        MTLPixelFormatBGRA8Unorm,
        width,
        height,
        0,
        &cv_tex );
    if( rc != kCVReturnSuccess || cv_tex == nullptr )
    {
        if( cv_tex != nullptr )
            CFRelease( cv_tex );
        return nil;
    }

    //	CVMetalTextureGetTexture returns an autoreleased / cache-owned
    //	id<MTLTexture> view over the IOSurface. We do NOT own a +1 on it ;
    //	its validity is tied to keeping cv_tex retained, which we do via
    //	current_texture_.
    id<MTLTexture> texture = CVMetalTextureGetTexture( cv_tex );
    if( texture == nil )
    {
        CFRelease( cv_tex );
        return nil;
    }

    current_texture_ = (void *) cv_tex;   //	transfer ownership of the +1
    return texture;
}

void MetalTextureBridge::flush()
{
    release_current_texture();
    if( cache_ != nullptr )
        CVMetalTextureCacheFlush( cache_, 0 );
}

}   //	namespace video
}   //	namespace aaa
