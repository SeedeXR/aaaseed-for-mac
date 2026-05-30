// src/display/display_present_mac.mm
//
// second_todo.md S8 (c153) : sub-rect present primitive. See
// display_present_mac.h for scope + doctrine. Pure ObjC++, manual reference
// counting (-fno-objc-arc).

#import "src/display/display_present_mac.h"

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

#include <cstdint>

namespace aaa
{
namespace display
{

namespace
{

//	Inline MSL : fullscreen triangle sampling a normalized sub-rect of the
//	source. uv.x maps screen-left->subrect-left (no flip) ; uv.y is flipped so
//	an identity rect (0,0,1,1) presents the source upright (texture-top ->
//	screen-top).
char const * const kSubRectMSL =
    "#include <metal_stdlib>\n"
    "using namespace metal;\n"
    "struct VOut { float4 pos [[position]]; float2 uv; };\n"
    "vertex VOut sub_v(uint vid [[vertex_id]], constant float4& rect [[buffer(0)]]) {\n"
    "  float2 quad = float2((vid << 1) & 2, vid & 2);\n"   //	(0,0),(2,0),(0,2)
    "  float2 pos  = quad * 2.0 - 1.0;\n"                  //	(-1,-1),(3,-1),(-1,3)
    "  float2 t    = quad;\n"                              //	0..1 across the visible screen
    "  VOut o;\n"
    "  o.pos = float4(pos, 0.0, 1.0);\n"
    "  o.uv  = float2(rect.x + t.x * rect.z, rect.y + (1.0 - t.y) * rect.w);\n"
    "  return o;\n"
    "}\n"
    "fragment float4 sub_f(VOut in [[stage_in]], texture2d<float> src [[texture(0)]],\n"
    "                      sampler smp [[sampler(0)]]) {\n"
    "  return src.sample(smp, in.uv);\n"
    "}\n";

}   //	anonymous namespace

struct SubRectPresenter::Impl
{
    id<MTLDevice>              device   = nil;
    id<MTLFunction>           vfn       = nil;
    id<MTLFunction>           ffn       = nil;
    id<MTLSamplerState>       sampler   = nil;
    id<MTLRenderPipelineState> pipeline = nil;
    MTLPixelFormat            pipeline_format = MTLPixelFormatInvalid;
    bool                      valid    = false;

    void teardown()
    {
        [pipeline release]; pipeline = nil;
        [sampler release];  sampler  = nil;
        [ffn release];      ffn      = nil;
        [vfn release];      vfn      = nil;
        [device release];   device   = nil;
        valid = false;
    }

    //	Defined out-of-line below (records the sub-rect quad pass on `cb`).
    bool encode( id<MTLCommandBuffer> cb, id<MTLTexture> source,
                 id<MTLTexture> dest, Rect const & subrect );

    bool ensure_pipeline( MTLPixelFormat fmt )
    {
        if( pipeline != nil && pipeline_format == fmt )
            return true;
        [pipeline release]; pipeline = nil;

        MTLRenderPipelineDescriptor * d =
            [[[MTLRenderPipelineDescriptor alloc] init] autorelease];
        d.vertexFunction   = vfn;
        d.fragmentFunction = ffn;
        d.colorAttachments[ 0 ].pixelFormat = fmt;

        NSError * err = nil;
        pipeline = [device newRenderPipelineStateWithDescriptor:d error:&err];
        if( pipeline == nil )
            return false;
        pipeline_format = fmt;
        return true;
    }
};

SubRectPresenter::SubRectPresenter( id<MTLDevice> device )
    : impl_( new Impl() )
{
    if( device == nil )
        return;

    @autoreleasepool
    {
        impl_->device = [device retain];

        NSError *      err = nil;
        NSString *     src = [NSString stringWithUTF8String:kSubRectMSL];
        id<MTLLibrary> lib = [device newLibraryWithSource:src options:nil error:&err];
        if( lib == nil )
            return;

        impl_->vfn = [[lib newFunctionWithName:@"sub_v"] retain];
        impl_->ffn = [[lib newFunctionWithName:@"sub_f"] retain];
        if( impl_->vfn == nil || impl_->ffn == nil )
            return;

        MTLSamplerDescriptor * sd = [[[MTLSamplerDescriptor alloc] init] autorelease];
        sd.minFilter = MTLSamplerMinMagFilterNearest;
        sd.magFilter = MTLSamplerMinMagFilterNearest;
        sd.sAddressMode = MTLSamplerAddressModeClampToEdge;
        sd.tAddressMode = MTLSamplerAddressModeClampToEdge;
        impl_->sampler = [[device newSamplerStateWithDescriptor:sd] retain];
        if( impl_->sampler == nil )
            return;

        impl_->valid = true;
    }
}

SubRectPresenter::~SubRectPresenter()
{
    impl_->teardown();
    delete impl_;
}

bool SubRectPresenter::is_valid() const { return impl_->valid; }

//	Shared encode : record the sub-rect quad pass that draws `source` into
//	`dest` on `cb`. Caller owns commit / present / wait. Returns false if the
//	pipeline can't be built for dest's format.
bool SubRectPresenter::Impl::encode( id<MTLCommandBuffer> cb,
                                     id<MTLTexture>       source,
                                     id<MTLTexture>       dest,
                                     Rect const &         subrect )
{
    if( !ensure_pipeline( dest.pixelFormat ) )
        return false;

    MTLRenderPassDescriptor * rp = [MTLRenderPassDescriptor renderPassDescriptor];
    rp.colorAttachments[ 0 ].texture     = dest;
    rp.colorAttachments[ 0 ].loadAction  = MTLLoadActionClear;
    rp.colorAttachments[ 0 ].storeAction = MTLStoreActionStore;
    rp.colorAttachments[ 0 ].clearColor  = MTLClearColorMake( 0, 0, 0, 1 );

    id<MTLRenderCommandEncoder> enc =
        [cb renderCommandEncoderWithDescriptor:rp];
    float rect4[ 4 ] = { (float) subrect.x, (float) subrect.y,
                         (float) subrect.w, (float) subrect.h };
    [enc setRenderPipelineState:pipeline];
    [enc setVertexBytes:rect4 length:sizeof( rect4 ) atIndex:0];
    [enc setFragmentTexture:source atIndex:0];
    [enc setFragmentSamplerState:sampler atIndex:0];
    [enc drawPrimitives:MTLPrimitiveTypeTriangle vertexStart:0 vertexCount:3];
    [enc endEncoding];
    return true;
}

bool SubRectPresenter::present( id<MTLCommandQueue> queue,
                                id<MTLTexture>      source,
                                id<MTLTexture>      dest,
                                Rect const &        subrect )
{
    if( !impl_->valid || queue == nil || source == nil || dest == nil )
        return false;

    @autoreleasepool
    {
        id<MTLCommandBuffer> cb = [queue commandBuffer];
        if( !impl_->encode( cb, source, dest, subrect ) )
            return false;
        [cb commit];
        [cb waitUntilCompleted];
        return true;
    }
}

bool SubRectPresenter::present_to_drawable( id<MTLCommandQueue> queue,
                                            id<MTLTexture>      source,
                                            void *              ca_metal_drawable,
                                            Rect const &        subrect )
{
    if( !impl_->valid || queue == nil || source == nil || ca_metal_drawable == nullptr )
        return false;

    @autoreleasepool
    {
        id<CAMetalDrawable> drawable = (__bridge id<CAMetalDrawable>) ca_metal_drawable;
        id<MTLTexture>      dest     = drawable.texture;
        if( dest == nil )
            return false;

        id<MTLCommandBuffer> cb = [queue commandBuffer];
        if( !impl_->encode( cb, source, dest, subrect ) )
            return false;
        [cb presentDrawable:drawable];
        [cb commit];   //	no wait : OS frame pacing, like present_window
        return true;
    }
}

}   //	namespace display
}   //	namespace aaa
