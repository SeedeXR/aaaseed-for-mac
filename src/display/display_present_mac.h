// src/display/display_present_mac.h
//
// second_todo.md S8 (c153) : the per-display PRESENT primitive for the
// fullscreen-span feature. MultiDisplay (display_mac.h) creates one
// CAMetalLayer per aux screen ; this class draws the correct slice of a
// shared, already-rendered source texture into each aux layer's drawable.
//
// `SubRectPresenter` renders a normalized sub-rect (from
// display_layout::normalized_subrect) of a source MTLTexture into a
// destination texture (an aux drawable's texture) via a fullscreen textured
// quad with bilinear/nearest sampling -- so differing aux-display resolutions
// scale correctly. This is the reusable core the engine present loop calls
// once per aux window each frame.
//
// Why a class here (not in the MTKView host) : keeping the present primitive
// hermetic + standalone makes it unit-testable headless (render into an
// offscreen texture, read the bytes back) without a window, an MTKView, or
// the gol MetalBackend. The thin host glue -- render the scene to a texture,
// then for each aux layer grab nextDrawable and call present() -- lives in
// src/ui/macos and is the remaining tracked integration step (see
// second_todo.md S8).
//
// Hermetic Mac sub-lib doctrine (feedback_hermetic_mac_sublibs.md) :
//   - Pure ObjC++ (.mm). std:: + Metal + Foundation only. No engine link, no
//     gol/MetalBackend dependency, no o_str, no aaa_mem. The MSL is compiled
//     from an inline source string via newLibraryWithSource (no .metallib
//     asset, no build-time metal step). Manual reference counting.
//   - C++-clean except the id<MTL*> entry points, guarded with __OBJC__ like
//     metal_texture_bridge.h.

#pragma once

#include "src/display/display_layout.h"

#ifdef __OBJC__
@protocol MTLDevice;
@protocol MTLTexture;
@protocol MTLCommandQueue;
#else
typedef void * MTLDeviceHandle;
typedef void * MTLTextureHandle;
typedef void * MTLCommandQueueHandle;
#endif

namespace aaa
{
namespace display
{

class SubRectPresenter
{
public:
#ifdef __OBJC__
    //	Build the blit pipeline + sampler for `device`. is_valid() is false if
    //	the device is nil or the inline MSL fails to compile.
    explicit SubRectPresenter( id<MTLDevice> device );
#else
    explicit SubRectPresenter( MTLDeviceHandle device );
#endif
    ~SubRectPresenter();

    SubRectPresenter( SubRectPresenter const & )             = delete;
    SubRectPresenter & operator=( SubRectPresenter const & ) = delete;

    bool is_valid() const;

#ifdef __OBJC__
    //	Draw the `subrect` (normalized 0..1) of `source` into `dest` via a
    //	fullscreen textured quad on `queue`. Commits and waits for completion
    //	(so a test can read `dest` back immediately ; the host can drop the
    //	wait and present the drawable instead). Returns false on nil args or
    //	if the pipeline is invalid. The pipeline is (re)built to match
    //	`dest.pixelFormat` on first use / format change.
    bool present( id<MTLCommandQueue> queue,
                  id<MTLTexture>      source,
                  id<MTLTexture>      dest,
                  Rect const &        subrect );

    //	Live variant for the host present loop : render `source`'s `subrect`
    //	into `drawable`'s texture and schedule the drawable for presentation
    //	on the same command buffer, then commit WITHOUT waiting (OS frame
    //	pacing, like MetalBackend::present_window). `drawable` is an opaque
    //	id<CAMetalDrawable> passed as void* to keep QuartzCore out of this
    //	header. Returns false on nil args / invalid pipeline.
    bool present_to_drawable( id<MTLCommandQueue> queue,
                              id<MTLTexture>      source,
                              void *              ca_metal_drawable,
                              Rect const &        subrect );
#else
    bool present( MTLCommandQueueHandle queue,
                  MTLTextureHandle      source,
                  MTLTextureHandle      dest,
                  Rect const &          subrect );
#endif

private:
    struct Impl;
    Impl * impl_;
};

}   //	namespace display
}   //	namespace aaa
