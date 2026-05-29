// AAASeedMTKView.mm — MTKViewDelegate driving GOL::MetalBackend per frame.
//
// Bridge pattern : MTKView gives us a `id<CAMetalDrawable>` (Objective-C).
// metal-cpp's `CA::MetalDrawable*` is layout-compatible with that id<> so
// `__bridge` cast is enough — no retain/release across the boundary. The
// drawable's lifetime is owned by Cocoa for the duration of the frame.

#import "AAASeedMTKView.h"

#include <Metal/Metal.hpp>
#include <QuartzCore/QuartzCore.hpp>

#include <chrono>
#include <cstdint>
#include <cstdio>
#include <memory>
#include <string>
#include <vector>

#include "src/gol/gol_backend.h"
#include "src/text/glyph_atlas.h"

//	c144 : MEU runner full type for ivar storage + render_frame dispatch.
#include "src/meu/aaa_meu_runner_mac.h"
//	c147-A : widget system full type. The view owns a unique_ptr<>
//	ivar + drives begin_frame / end_frame around the MEU runner.
#include "src/ui/widgets/aaa_widgets_mac.h"
//	c147-A : need AAASeedInputView's lastMouseLocation accessor to feed
//	the widget system. The umbrella module already imports the input
//	view header transitively via the app target ; we include it here
//	explicitly so the .mm can call accessors on the delegate's view
//	(via the MTKView's `view` argument).
#import "AAASeedInputView.h"

//	c148 : ImGui Studio authoring surface. Owned alongside the widget
//	system ; driven once per frame (new_frame before the pass,
//	render inside the pass after the widget system end_frame).
// c152-D : Studio include retired with the ImGui port.

#include "stb_image.h"

#include <cmath>
#include <cstring>
#include <fstream>

namespace
{
    //	Inline MSL shader : full-screen triangle on a blue clear background.
    //	Three-vertex triangle covers all of NDC ; fragment shader returns
    //	white with the position-derived gradient so the centre is bright.
    //	Reuses the [[vertex_id]] trick so we don't need a vertex buffer.
    constexpr char const* kTriangleMsl = R"MSL(
#include <metal_stdlib>
using namespace metal;

struct VertexOut
{
    float4 position [[position]];
    float2 uv;
};

vertex VertexOut vs_main( uint vid [[vertex_id]] )
{
    //	Small centred equilateral-ish triangle so we can see both the
    //	clear color and the triangle on the same frame.
    float2 verts[3];
    verts[0] = float2( -0.7, -0.5 );
    verts[1] = float2(  0.7, -0.5 );
    verts[2] = float2(  0.0,  0.7 );
    VertexOut out;
    out.position = float4( verts[ vid ], 0.0, 1.0 );
    out.uv       = ( verts[ vid ] + float2( 1.0, 1.0 ) ) * 0.5;
    return out;
}

fragment float4 fs_main( VertexOut in [[stage_in]] )
{
    //	Per-vertex gradient : red ↔ green based on uv ; lets us see
    //	the rasterizer's barycentric interpolation in the window.
    return float4( in.uv.x, in.uv.y, 0.4, 1.0 );
}
)MSL";

    //	Continuation 61 : minimal text MSL used by the FPS HUD. Same as
    //	tests/unit/text_render_integration_test.cpp ; pixel-space xy ->
    //	NDC with y-axis flip, R8 atlas sample as alpha coverage. Output
    //	a configurable RGB color (uniform at fragment buffer slot 0) so
    //	the HUD can choose its tint without recompiling.
    constexpr char const* kHudTextMsl = R"MSL(
#include <metal_stdlib>
using namespace metal;

struct VIn {
    float2 pos [[attribute(0)]];
    float2 uv  [[attribute(1)]];
};

struct VOut {
    float4 position [[position]];
    float2 uv;
};

struct ScreenSize { float2 size; float2 pad; };
struct HudColor   { float4 rgba; };

vertex VOut vs_main(
    VIn in                       [[stage_in]],
    constant ScreenSize& screen  [[buffer(1)]] )
{
    float2 ndc;
    ndc.x =  (in.pos.x / screen.size.x) * 2.0 - 1.0;
    ndc.y = -((in.pos.y / screen.size.y) * 2.0 - 1.0);
    VOut o;
    o.position = float4( ndc, 0.0, 1.0 );
    o.uv       = in.uv;
    return o;
}

fragment float4 fs_main(
    VOut             in            [[stage_in]],
    texture2d<float> atlas         [[texture(0)]],
    sampler          atlas_sampler [[sampler(0)]],
    constant HudColor& tint        [[buffer(0)]] )
{
    float cov = atlas.sample( atlas_sampler, in.uv ).r;
    return float4( tint.rgba.rgb, tint.rgba.a * cov );
}
)MSL";

    struct HudScreenSize { float w, h, pad0, pad1; };
    static_assert( sizeof( HudScreenSize ) == 16, "HudScreenSize alignment" );
    struct HudTint { float r, g, b, a; };
    static_assert( sizeof( HudTint ) == 16, "HudTint alignment" );

    //	Cap on vertices in the per-frame HUD buffer. 192 chars * 6 verts =
    //	1152, comfortable for "FPS=XX.X frame=NNNNN" + future expansion.
    constexpr std::size_t kHudMaxVertices = 1152;

    //	c68 -- solid-color quad MSL for the HUD backdrop rectangle.
    //	No atlas sample ; the fragment shader is just the tint uniform.
    //	Same pixel-space->NDC vertex stage as kHudTextMsl ; vertex layout
    //	only uses .pos (uv is unused but the vertex descriptor stays
    //	2-attribute compatible with the text pipeline so a single layout
    //	can drive both pipelines).
    constexpr char const* kHudSolidMsl = R"MSL(
#include <metal_stdlib>
using namespace metal;

struct VIn {
    float2 pos [[attribute(0)]];
    float2 uv  [[attribute(1)]];   // unused but kept for VBO compat
};

struct VOut {
    float4 position [[position]];
};

struct ScreenSize { float2 size; float2 pad; };
struct HudColor   { float4 rgba; };

vertex VOut vs_main(
    VIn in                       [[stage_in]],
    constant ScreenSize& screen  [[buffer(1)]] )
{
    float2 ndc;
    ndc.x =  (in.pos.x / screen.size.x) * 2.0 - 1.0;
    ndc.y = -((in.pos.y / screen.size.y) * 2.0 - 1.0);
    VOut o;
    o.position = float4( ndc, 0.0, 1.0 );
    return o;
}

fragment float4 fs_main(
    VOut in                  [[stage_in]],
    constant HudColor& tint  [[buffer(0)]] )
{
    return tint.rgba;
}
)MSL";
}

//	Engine convention : per-frame fu_vec4 uniforms uploaded as a 16-element
//	float4 array. Mirrors `AaaFuVec4s` from tools/glsl_to_msl/glsl_to_msl.py.
struct AAASeedAaaFuVec4s
{
    float values[ 16 ][ 4 ];
};

@implementation AAASeedMTKViewDelegate
{
    GOL::MetalBackend* _backend;        // not owned
    NSInteger          _maxFrames;
    NSInteger          _frameCount;
    BOOL               _shouldTerminate;
    GOL::ProgramId     _triangleProgram;
    //	Continuation 39 : Path A demo state. First catalog shader actually
    //	rendered in the .app bundle's main loop. Loaded from
    //	`.app/Contents/Resources/shaders/ps_Maa_add_scale.metal`. When
    //	the load succeeds we render through this program ; when it fails
    //	(rare -- bundle ID drift or build-time copy missed) the original
    //	inline triangle is the fallback.
    GOL::ProgramId     _catalogProgram;
    GOL::TextureId     _catalogSrcTex;
    GOL::BufferId      _catalogUniformBuf;
    //	Per-frame perf counters (continuation 56). Aggregated across
    //	measured frames, emitted as a single stdout line when the app
    //	terminates via --max-frames. First frame is excluded from the
    //	measurement to avoid warmup skew (Metal pipeline JIT + first
    //	drawable allocation).
    std::uint64_t      _perfTotalNs;
    std::uint64_t      _perfMaxNs;
    std::uint64_t      _perfMinNs;
    NSInteger          _perfMeasuredFrames;

    //	Continuation 61 : HUD text path. Atlas built once at init from
    //	bundled SourceCodePro-Medium.ttf. R8 MTLTexture + text MSL +
    //	per-frame transient vertex buffer holding layout_text_quads()
    //	output for the FPS+frame counter string.
    GOL::ProgramId     _hudProgram;
    GOL::TextureId     _hudAtlasTex;
    GOL::BufferId      _hudVertexBuf;
    GOL::BufferId      _hudScreenBuf;
    GOL::BufferId      _hudTintBuf;
    aaa::text::GlyphAtlas _hudAtlas;
    //	c68 -- backdrop rectangle. Solid-color shader + its own vertex
    //	buffer (6-vertex quad) + dark-tint uniform. Drawn BEFORE the
    //	text so alpha-blend layers white text over semi-transparent
    //	dark for legibility against the catalog shader noise.
    GOL::ProgramId     _hudBackdropProgram;
    GOL::BufferId      _hudBackdropVertexBuf;
    GOL::BufferId      _hudBackdropTintBuf;
    //	Rolling FPS average across the last N frames so the HUD doesn't
    //	flicker integer-by-integer. Window of 30 frames at 60 Hz = 0.5 s.
    std::uint64_t      _hudRecentFrameNs[ 30 ];
    int                _hudRecentFrameWriteIx;
    int                _hudRecentFrameCount;

    //	c144 : MEU runner. Lua-driven Path A catalog cycler. Constructed
    //	in initWithBackend: against the backend pointer ; load_script
    //	resolves `hello_world.lua` via NSBundle (Resources/meu/) with a
    //	compile-time source-tree fallback. drawInMTKView: calls
    //	render_frame between the catalog draw and the HUD overlay.
    //	`_meuRenderActive` gates draw-time dispatch -- false when the
    //	runner failed to load or the script absent, in which case the
    //	c39 catalog path stays the visible content.
    std::unique_ptr< aaa::meu::Runner > _meuRunner;
    BOOL                                _meuRenderActive;

    //	c146 : Lua-driven HUD overlay. After render_frame the runner
    //	exposes any string queued via aaa.draw_hud_text(); the view
    //	lays out glyph quads into _meuHudVertexBuf and draws them with
    //	the existing _hudProgram (same atlas + text-MSL pipeline). Drawn
    //	AFTER the c61 FPS HUD so the script's text sits on top. The
    //	overlay is a logical "second pass" in the spec sense but lives
    //	inside the same window render pass to avoid double-presenting
    //	the drawable (Metal's begin_window_render_pass / present_window
    //	pairing is one-per-frame). Alpha-blend over the runner output
    //	preserves the c143-C visual content underneath.
    GOL::BufferId _meuHudVertexBuf;

    //	c147-A : immediate-mode widget UI. Owned ivar ; lifetime tied to
    //	the delegate. Begun BEFORE _meuRunner->render_frame so Lua
    //	bindings can dispatch into it ; ended AFTER the MEU HUD overlay
    //	so widget chrome sits beneath HUD text (HUD text wins layering
    //	on overlap, which is fine per spec).
    std::unique_ptr< aaa::ui::widgets::WidgetSystem > _widgetSystem;
    //	Mouse edge flags consumed by the widget system. AAASeedInputView's
    //	mouseDown: / mouseUp: call queueMousePressed / queueMouseReleased
    //	on the delegate ; drawInMTKView: consumes them via drain* helpers.
    BOOL _mousePressedEdge;
    BOOL _mouseReleasedEdge;
    // c152-D : ImGui Studio ivars retired with the .mm Studio surface.
}

//	Read a .metal file out of the .app bundle's Resources/shaders/
//	directory. Returns @"" if missing -- caller falls back to inline.
+ (NSString*)loadCatalogShader:(NSString*)name
{
    NSBundle* bundle = [NSBundle mainBundle];
    NSString* path   = [bundle pathForResource:name ofType:@"metal" inDirectory:@"shaders"];
    if( !path )
    {
        NSLog( @"AAASeed: catalog shader '%@' not found in bundle Resources/shaders/", name );
        return @"";
    }
    NSError* err = nil;
    NSString* src = [NSString stringWithContentsOfFile:path
                                              encoding:NSUTF8StringEncoding
                                                 error:&err];
    if( !src )
    {
        NSLog( @"AAASeed: failed to read '%@' : %@", path, err );
        return @"";
    }
    return src;
}

- (instancetype)initWithBackend:(GOL::MetalBackend*)backend
                      maxFrames:(NSInteger)maxFrames
{
    self = [super init];
    if( self )
    {
        _backend           = backend;
        _maxFrames         = maxFrames;
        _frameCount        = 0;
        _shouldTerminate   = NO;
        _triangleProgram   = GOL::kInvalidProgramId;
        _catalogProgram    = GOL::kInvalidProgramId;
        _catalogSrcTex     = GOL::kInvalidTextureId;
        _catalogUniformBuf = GOL::kInvalidBufferId;
        _perfTotalNs       = 0;
        _perfMaxNs         = 0;
        _perfMinNs         = ~std::uint64_t( 0 );
        _perfMeasuredFrames = 0;

        _hudProgram        = GOL::kInvalidProgramId;
        _hudAtlasTex       = GOL::kInvalidTextureId;
        _hudVertexBuf      = GOL::kInvalidBufferId;
        _hudScreenBuf      = GOL::kInvalidBufferId;
        _hudTintBuf        = GOL::kInvalidBufferId;
        _hudBackdropProgram   = GOL::kInvalidProgramId;
        _hudBackdropVertexBuf = GOL::kInvalidBufferId;
        _hudBackdropTintBuf   = GOL::kInvalidBufferId;
        _meuHudVertexBuf      = GOL::kInvalidBufferId;
        std::memset( _hudRecentFrameNs, 0, sizeof( _hudRecentFrameNs ) );
        _hudRecentFrameWriteIx = 0;
        _hudRecentFrameCount   = 0;
        _meuRenderActive       = NO;
        _mousePressedEdge      = NO;
        _mouseReleasedEdge     = NO;
        // c152-D : studio init flags retired.

        if( !_backend )
            return self;

        //	Compile the inline triangle shader as the fallback. If
        //	catalog setup below succeeds, we never call it ; but if
        //	the bundle is missing the .metal asset (rare), the user
        //	still sees a window with the triangle gradient.
        _triangleProgram = _backend->create_program_msl(
            kTriangleMsl, "vs_main", "fs_main",
            GOL::TextureFormat::BGRA8 );
        if( _triangleProgram == GOL::kInvalidProgramId )
        {
            NSLog( @"AAASeed: triangle shader compile failed : %s",
                   _backend->get_last_error().c_str() );
        }

        //	---- Path A demo path : load from bundle, compile, set up the
        //	full draw resources. The continuation-39 segfault was a
        //	double-release in `MetalBackend::gen_texture_2d` ; fixed in
        //	continuation 40 so the texture/uniform allocation inside
        //	AppKit's run loop is now safe.
        NSString* msl = [AAASeedMTKViewDelegate
                            loadCatalogShader:@"ps_Maa_add_scale"];
        if( [msl length] > 0 )
        {
            _catalogProgram = _backend->create_program_msl(
                [msl UTF8String], "vs_main", "fs_main",
                GOL::TextureFormat::BGRA8 );
            if( _catalogProgram == GOL::kInvalidProgramId )
            {
                NSLog( @"AAASeed: catalog shader compile failed : %s",
                       _backend->get_last_error().c_str() );
            }
        }

        if( _catalogProgram != GOL::kInvalidProgramId )
        {
            //	Continuation 46 : load `test_pattern.png` from
            //	bundle Resources/ via stb_image. End-to-end Phase 5
            //	asset path : build-time generator (aaa_make_test_png)
            //	-> PNG file in bundle -> NSBundle lookup -> stbi_load
            //	-> MTLTexture upload -> render through catalog shader.
            //
            //	Fallback : if the bundle lookup or decode fails, build
            //	the procedural pattern in memory (same shape as the
            //	bundle pattern). Keeps the .app visible even on a
            //	stripped/repackaged bundle.
            NSBundle* bundle = [NSBundle mainBundle];
            NSString* png_path = [bundle pathForResource:@"test_pattern"
                                                   ofType:@"png"];
            int img_w = 0, img_h = 0, img_c = 0;
            std::uint8_t* png_pixels = nullptr;
            if( png_path )
            {
                png_pixels = stbi_load( [png_path UTF8String],
                                        &img_w, &img_h, &img_c,
                                        /*desired_channels*/ 4 );
            }

            std::uint32_t W = 64;
            std::uint32_t H = 64;
            std::vector< std::uint8_t > fallback_pixels;
            std::uint8_t const* tex_bytes = nullptr;

            if( png_pixels && img_w > 0 && img_h > 0 )
            {
                W = (std::uint32_t) img_w;
                H = (std::uint32_t) img_h;
                tex_bytes = png_pixels;
                NSLog( @"AAASeed: loaded test_pattern.png %dx%d from bundle.",
                       img_w, img_h );
            }
            else
            {
                NSLog( @"AAASeed: test_pattern.png missing or invalid ; using "
                       @"procedural fallback. stbi reason=%s",
                       stbi_failure_reason() ? stbi_failure_reason() : "(none)" );
                fallback_pixels.resize( W * H * 4, 0 );
                for( std::uint32_t y = 0; y < H; ++y )
                    for( std::uint32_t x = 0; x < W; ++x )
                    {
                        std::size_t i = ( y * W + x ) * 4;
                        fallback_pixels[ i + 0 ] = std::uint8_t( ( x * 255 ) / ( W - 1 ) );
                        fallback_pixels[ i + 1 ] = std::uint8_t( ( y * 255 ) / ( H - 1 ) );
                        fallback_pixels[ i + 2 ] = 128;
                        fallback_pixels[ i + 3 ] = 255;
                    }
                tex_bytes = fallback_pixels.data();
            }

            _catalogSrcTex = _backend->gen_texture_2d( W, H, GOL::TextureFormat::RGBA8 );
            if( _catalogSrcTex != GOL::kInvalidTextureId && tex_bytes )
            {
                _backend->texture_data_2d( _catalogSrcTex, tex_bytes, W * 4 );
            }

            if( png_pixels )
                stbi_image_free( png_pixels );

            AAASeedAaaFuVec4s u{};
            for( int i = 0; i < 16; ++i )
                for( int j = 0; j < 4; ++j )
                    u.values[ i ][ j ] = 0.0f;
            //	offset = small blue tint ; scale = identity.
            u.values[ 0 ][ 2 ] = 0.3f;
            u.values[ 1 ][ 0 ] = 1.0f;
            u.values[ 1 ][ 1 ] = 1.0f;
            u.values[ 1 ][ 2 ] = 1.0f;
            u.values[ 1 ][ 3 ] = 1.0f;

            _catalogUniformBuf = _backend->gen_buffer();
            if( _catalogUniformBuf != GOL::kInvalidBufferId )
            {
                _backend->buffer_data( _catalogUniformBuf, sizeof( u ),
                                       &u, GOL::BufferUsage::Static );
            }

            NSLog( @"AAASeed: catalog shader 'ps_Maa_add_scale' wired ; bundle path live." );
        }

        //	---- Continuation 61 : HUD text setup --------------------------
        //	Bundle path : Resources/fonts/SourceCodePro-Medium.ttf. Read
        //	the file once, build a 256x256 alpha-8 atlas at 16 px, upload
        //	to R8 MTLTexture. Per-frame draw assembles the FPS string,
        //	rebuilds the transient vertex buffer, draws with alpha-blend
        //	after the catalog pass.
        NSString* font_path = [[NSBundle mainBundle]
            pathForResource:@"SourceCodePro-Medium" ofType:@"ttf" inDirectory:@"fonts"];
        if( font_path )
        {
            std::ifstream font_in( [font_path UTF8String], std::ios::binary | std::ios::ate );
            if( font_in )
            {
                std::streamsize const sz = font_in.tellg();
                font_in.seekg( 0, std::ios::beg );
                std::vector< std::uint8_t > font_bytes( (std::size_t) sz );
                if( font_in.read( reinterpret_cast< char* >( font_bytes.data() ), sz )
                    && aaa::text::build_printable_ascii_atlas(
                        font_bytes.data(), font_bytes.size(),
                        16.0f, 256, 256, _hudAtlas ) )
                {
                    _hudAtlasTex = _backend->gen_texture_2d(
                        std::uint32_t( _hudAtlas.width ),
                        std::uint32_t( _hudAtlas.height ),
                        GOL::TextureFormat::R8 );
                    if( _hudAtlasTex != GOL::kInvalidTextureId )
                    {
                        _backend->texture_data_2d( _hudAtlasTex,
                                                   _hudAtlas.alpha8.data(),
                                                   (std::size_t) _hudAtlas.width );
                    }

                    GOL::VertexAttribute attrs[ 2 ];
                    attrs[ 0 ].shader_location = 0;
                    attrs[ 0 ].buffer_slot     = 0;
                    attrs[ 0 ].offset          = offsetof( aaa::text::Vertex2D, x );
                    attrs[ 0 ].format          = GOL::VertexFormat::Float2;
                    attrs[ 1 ].shader_location = 1;
                    attrs[ 1 ].buffer_slot     = 0;
                    attrs[ 1 ].offset          = offsetof( aaa::text::Vertex2D, u );
                    attrs[ 1 ].format          = GOL::VertexFormat::Float2;
                    GOL::VertexBufferLayout layouts[ 1 ];
                    layouts[ 0 ].slot   = 0;
                    layouts[ 0 ].stride = sizeof( aaa::text::Vertex2D );

                    GOL::ProgramDesc pd;
                    pd.msl_source       = kHudTextMsl;
                    pd.vertex_entry     = "vs_main";
                    pd.fragment_entry   = "fs_main";
                    pd.target_color_fmt = GOL::TextureFormat::BGRA8;
                    pd.attributes       = attrs;
                    pd.attribute_nb     = 2;
                    pd.layouts          = layouts;
                    pd.layout_nb        = 1;
                    pd.blend_mode       = GOL::BlendMode::AlphaBlend;
                    _hudProgram = _backend->create_program( pd );
                    if( _hudProgram == GOL::kInvalidProgramId )
                    {
                        NSLog( @"AAASeed: HUD text shader compile failed : %s",
                               _backend->get_last_error().c_str() );
                    }

                    _hudVertexBuf = _backend->gen_buffer();
                    if( _hudVertexBuf != GOL::kInvalidBufferId )
                    {
                        std::vector< aaa::text::Vertex2D > zeros( kHudMaxVertices, aaa::text::Vertex2D{ 0.0f, 0.0f, 0.0f, 0.0f } );
                        _backend->buffer_data( _hudVertexBuf,
                                               zeros.size() * sizeof( aaa::text::Vertex2D ),
                                               zeros.data(),
                                               GOL::BufferUsage::Dynamic );
                    }
                    //	c146 : MEU HUD vertex buffer -- separate from the c61
                    //	FPS HUD buffer so both can be uploaded + drawn within
                    //	the same window render pass without ping-ponging the
                    //	same buffer in a single encoder.
                    _meuHudVertexBuf = _backend->gen_buffer();
                    if( _meuHudVertexBuf != GOL::kInvalidBufferId )
                    {
                        std::vector< aaa::text::Vertex2D > zeros( kHudMaxVertices, aaa::text::Vertex2D{ 0.0f, 0.0f, 0.0f, 0.0f } );
                        _backend->buffer_data( _meuHudVertexBuf,
                                               zeros.size() * sizeof( aaa::text::Vertex2D ),
                                               zeros.data(),
                                               GOL::BufferUsage::Dynamic );
                    }
                    _hudScreenBuf = _backend->gen_buffer();
                    _hudTintBuf   = _backend->gen_buffer();
                    HudTint tint{ 1.0f, 1.0f, 1.0f, 1.0f };
                    if( _hudTintBuf != GOL::kInvalidBufferId )
                    {
                        _backend->buffer_data( _hudTintBuf, sizeof( tint ), &tint,
                                               GOL::BufferUsage::Static );
                    }

                    //	c68 -- backdrop solid-color pipeline. Same vertex
                    //	descriptor as the text path (Vertex2D xy+uv) so
                    //	the same VBO slot layout drives both. UV unused
                    //	by the backdrop fragment shader.
                    GOL::ProgramDesc pd_bd = pd;
                    pd_bd.msl_source = kHudSolidMsl;
                    _hudBackdropProgram = _backend->create_program( pd_bd );
                    if( _hudBackdropProgram == GOL::kInvalidProgramId )
                    {
                        NSLog( @"AAASeed: HUD backdrop shader compile failed : %s",
                               _backend->get_last_error().c_str() );
                    }
                    _hudBackdropVertexBuf = _backend->gen_buffer();
                    if( _hudBackdropVertexBuf != GOL::kInvalidBufferId )
                    {
                        //	6 vertices for the backdrop quad ; uploaded per frame.
                        aaa::text::Vertex2D zeros[ 6 ] = {};
                        _backend->buffer_data( _hudBackdropVertexBuf,
                                               sizeof( zeros ), zeros,
                                               GOL::BufferUsage::Dynamic );
                    }
                    _hudBackdropTintBuf = _backend->gen_buffer();
                    HudTint backdrop_tint{ 0.0f, 0.0f, 0.0f, 0.55f };  //  semi-transparent black
                    if( _hudBackdropTintBuf != GOL::kInvalidBufferId )
                    {
                        _backend->buffer_data( _hudBackdropTintBuf,
                                               sizeof( backdrop_tint ), &backdrop_tint,
                                               GOL::BufferUsage::Static );
                    }

                    NSLog( @"AAASeed: HUD text pipeline live (atlas %dx%d, %zu glyphs).",
                           _hudAtlas.width, _hudAtlas.height,
                           (size_t) aaa::text::kNumAsciiChars );
                }
                else
                {
                    NSLog( @"AAASeed: HUD atlas build failed." );
                }
            }
        }
        else
        {
            NSLog( @"AAASeed: HUD font missing from bundle ; HUD disabled." );
        }

        //	---- c147-A : widget system construction -----------------------
        //	Built immediately so the MEU runner constructed below picks
        //	up the widget pointer via set_widget_system. Widget render
        //	is additive to the existing window pass per c145-A's
        //	"render INSIDE the active encoder" architectural deviation.
        _widgetSystem = std::make_unique< aaa::ui::widgets::WidgetSystem >( _backend );

        //	---- c144 : MEU runner construction + script load --------------
        //	Resolve `hello_world.lua` from the .app bundle's Resources/meu/
        //	directory first (the canonical path inside a shipped binary).
        //	When running under ctest / a non-bundled binary, fall back to
        //	the compile-time source-tree path so the integration test can
        //	drive the same code without re-staging the .app.
        _meuRunner = std::make_unique< aaa::meu::Runner >( _backend );
        //	c147-A : install widget system into the runner BEFORE
        //	load_script so a script that calls aaa.ui.* during its
        //	top-level run sees a live pointer.
        _meuRunner->set_widget_system( _widgetSystem.get() );
        //	c148-A : wire the hot-reload button. The callback captures
        //	a raw Runner* by value ; lifetime is safe because both
        //	objects are owned by this delegate (the callback is cleared
        //	in `unload` / on delegate dealloc before either resets).
        aaa::meu::Runner* runner_ptr = _meuRunner.get();
        _widgetSystem->set_reload_callback(
            [runner_ptr]{ runner_ptr->reload(); } );

        std::string lua_path;
        NSString*   bundlePath = [[NSBundle mainBundle]
            pathForResource:@"hello_world"
                     ofType:@"lua"
                inDirectory:@"meu"];
        if( bundlePath )
        {
            lua_path = std::string( [bundlePath UTF8String] );
        }
        else
        {
#ifdef AAA_MEU_HELLO_WORLD_LUA_PATH
            lua_path = std::string( AAA_MEU_HELLO_WORLD_LUA_PATH );
#endif
        }

        if( !lua_path.empty() )
        {
            if( _meuRunner->load_script( lua_path ) )
            {
                _meuRenderActive = YES;
                NSLog( @"AAASeed: MEU runner loaded '%s'", lua_path.c_str() );
                //	c149-A v3 Feature 3 : enable file-watch hot reload.
                //	Honors AAA_DISABLE_FILE_WATCH env var so regression
                //	tests can opt out. Best-effort -- a failure here just
                //	disables auto-reload, the click-only hot_reload_button
                //	still works.
                if( _meuRunner->enable_file_watch() )
                {
                    NSLog( @"AAASeed: file-watch hot reload enabled on '%s'",
                           lua_path.c_str() );
                }
            }
            else
            {
                NSLog( @"AAASeed: MEU runner load_script('%s') FAILED ; "
                       @"falling back to c39 catalog path.",
                       lua_path.c_str() );
            }
        }
        else
        {
            NSLog( @"AAASeed: hello_world.lua not found (bundle + fallback) ; "
                   @"MEU runner disabled." );
        }
    }
    return self;
}

- (BOOL)shouldTerminate { return _shouldTerminate; }

//	c144 : MEU runner accessor for AAASeedInputView event forwarding.
- (aaa::meu::Runner*)meuRunner
{
    return _meuRunner.get();
}

//	c147-A : widget system accessor + edge-queue helpers.
- (aaa::ui::widgets::WidgetSystem*)widgetSystem
{
    return _widgetSystem.get();
}

// c152-D : Studio accessor retired with the .mm Studio.

- (void)queueMousePressed
{
    _mousePressedEdge = YES;
}

- (void)queueMouseReleased
{
    _mouseReleasedEdge = YES;
}

- (void)mtkView:(nonnull MTKView*)view drawableSizeWillChange:(CGSize)size
{
    (void) view; (void) size;
    //	Nothing to do for the MVP : we render to the drawable's size
    //	directly via the viewport ; no FBO resize bookkeeping.
}

- (void)drawInMTKView:(nonnull MTKView*)view
{
    if( !_backend ) return;

    id< CAMetalDrawable > drawable = view.currentDrawable;
    if( !drawable ) return;

    CA::MetalDrawable* drawable_cpp = (__bridge CA::MetalDrawable*) drawable;

    //	Per-frame timing (continuation 56). Measures CPU-side encoder build +
    //	present_window wait. Sampled from frame 1 onwards ; frame 0 is excluded
    //	from the perf aggregate to avoid warmup skew.
    auto const t_frame_start = std::chrono::steady_clock::now();

    //	---- c148 : Studio lazy init + new_frame ---------------------------
    // c152-D : studio lazy-init / new_frame / NSEvent monitor / Cmd+R
    // wiring all retired with the ImGui Studio. The runtime binary
    // (aaaseed_runtime) is a pure engine renderer ; project authoring
    // lives in the Qt Studio. If a project path was provided on the
    // command line, the MEU runner loaded it in the app delegate ;
    // we just render whatever the runner emits.

    GOL::RenderPassDescriptor rpd;
    rpd.load_action      = GOL::LoadAction::Clear;
    //	Red clear so success is obvious to the user opening the window.
    //	Once real engine rendering lands, this clear color comes from the
    //	scene's c_render state.
    rpd.clear_color[ 0 ] = 1.0f;
    rpd.clear_color[ 1 ] = 0.0f;
    rpd.clear_color[ 2 ] = 0.0f;
    rpd.clear_color[ 3 ] = 1.0f;
    //	Per-frame GPU debug marker — Xcode Frame Capture sees one band per
    //	frame labelled "aaaseed.frame". Pairs with the existing CPU
    //	signposts in NSightEvents.h per the perf-profiling doctrine
    //	(philosophy.md Part 2.95).
    rpd.debug_label      = "aaaseed.frame";

    if( !_backend->begin_window_render_pass( drawable_cpp, rpd ) )
        return;

    CGSize const sz = view.drawableSize;
    _backend->set_viewport( 0.f, 0.f, (float) sz.width, (float) sz.height );

    //	---- c147-A : widget system begin_frame ----------------------------
    //	Pull mouse state from the view's lastMouseLocation (when the view
    //	is an AAASeedInputView -- the only thing the .app ever instantiates).
    //	Probe via respondsToSelector so this code path doesn't statically
    //	depend on the input view class (matches the c144 dispatch
    //	pattern in AAASeedInputView.mm's aaa_meu_runner lookup).
    //
    //	Coordinate space : NSEvent locationInWindow is in POINTS (NSView
    //	bottom-left), while view.drawableSize is in PIXELS (retina-aware,
    //	2x on Mac retina displays). To keep widget hit-testing consistent
    //	with mouse events, the widget system runs entirely in POINT space :
    //	we pass view.bounds.size (points) for the begin_frame dimensions
    //	+ mouse coords in points + a flip-y from bottom-left to top-left.
    //	The widget shader's pixel-space-to-NDC math uses the same screen
    //	dimensions we pass here, so the GPU rasterizes against the actual
    //	drawable pixel grid (NDC is resolution-independent ; the viewport
    //	set above to drawableSize handles the final fragment scaling).
    if( _widgetSystem )
    {
        NSSize const view_pts = view.bounds.size;
        double mouse_x = 0.0, mouse_y = 0.0;
        if( [view respondsToSelector:@selector( lastMouseLocation )] )
        {
            NSPoint const p =
                [(AAASeedInputView*) view lastMouseLocation];
            mouse_x = (double) p.x;
            mouse_y = (double) ( view_pts.height - p.y );
        }
        BOOL const pressed_now  = _mousePressedEdge;
        BOOL const released_now = _mouseReleasedEdge;
        _mousePressedEdge  = NO;
        _mouseReleasedEdge = NO;
        _widgetSystem->begin_frame( (std::uint32_t) view_pts.width,
                                    (std::uint32_t) view_pts.height,
                                    mouse_x, mouse_y,
                                    pressed_now == YES,
                                    released_now == YES );
    }

    //	c144 : MEU runner takes priority over the c39 catalog path
    //	whenever a script is loaded. The runner emits draws against the
    //	currently-active render pass (per its header contract -- it
    //	does NOT begin/end the pass and does NOT call present_window ;
    //	this view does that below). When the runner isn't active we
    //	fall back to the c39 catalog draw so the .app still shows
    //	pixels even if the .lua is missing from the bundle.
    if( _meuRenderActive && _meuRunner )
    {
        _meuRunner->render_frame(
            (std::uint32_t) sz.width,
            (std::uint32_t) sz.height,
            GOL::kInvalidTextureId );
    }
    else if( _catalogProgram    != GOL::kInvalidProgramId
        && _catalogSrcTex     != GOL::kInvalidTextureId
        && _catalogUniformBuf != GOL::kInvalidBufferId )
    {
        //	Catalog-shader path : full-screen draw through the
        //	bundle-loaded ps_Maa_add_scale shader. The MSL declares 4
        //	textures (engine `aaa_tex2d[4]`) ; we bind the same source
        //	texture into all 4 slots so Metal validation is satisfied
        //	even though only slot 0 is sampled.
        _backend->bind_program( _catalogProgram );
        _backend->bind_fragment_texture( _catalogSrcTex, 0 );
        _backend->bind_fragment_texture( _catalogSrcTex, 1 );
        _backend->bind_fragment_texture( _catalogSrcTex, 2 );
        _backend->bind_fragment_texture( _catalogSrcTex, 3 );
        _backend->bind_fragment_buffer(  _catalogUniformBuf, 0, 0 );
        _backend->draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    }
    else if( _triangleProgram != GOL::kInvalidProgramId )
    {
        //	Fallback : inline triangle when the catalog asset path
        //	couldn't be wired (bundle missing, compile failure).
        _backend->bind_program( _triangleProgram );
        _backend->draw_arrays( GOL::PrimitiveType::Triangles, 0, 3 );
    }

    //	---- c147-A : widget system end_frame ------------------------------
    //	Emit all queued widget quads as one draw_arrays against the active
    //	encoder. Placed AFTER the MEU runner / catalog path so widget
    //	chrome paints on top of the script's output ; placed BEFORE the
    //	HUD overlay below so HUD text wins z-order on overlap (per spec).
    if( _widgetSystem )
    {
        _widgetSystem->end_frame();
    }

    // c152-D : ImGui Studio render block retired.

    //	---- Continuation 61 : HUD overlay ---------------------------------
    if( _hudProgram != GOL::kInvalidProgramId
        && _hudAtlasTex != GOL::kInvalidTextureId
        && _hudVertexBuf != GOL::kInvalidBufferId
        && _hudScreenBuf != GOL::kInvalidBufferId
        && _hudTintBuf   != GOL::kInvalidBufferId )
    {
        //	Compute rolling-average FPS from the last N measured frames.
        double avg_ms = 0.0;
        if( _hudRecentFrameCount > 0 )
        {
            std::uint64_t sum = 0;
            for( int i = 0; i < _hudRecentFrameCount; ++i )
                sum += _hudRecentFrameNs[ i ];
            avg_ms = double( sum ) / 1.0e6 / double( _hudRecentFrameCount );
        }
        double const fps = ( avg_ms > 0.0001 ) ? ( 1000.0 / avg_ms ) : 0.0;

        char hud_str[ 96 ];
        std::snprintf( hud_str, sizeof( hud_str ),
                       "FPS=%4.1f frame=%ld",
                       fps, (long) _frameCount );

        //	c68 -- right-justify the HUD anchored to the top-right
        //	corner. Measure text width via the atlas's xadvance sum.
        float const text_w   = aaa::text::measure_text_width( _hudAtlas, hud_str );
        float const margin_x = 12.0f;
        float const margin_y = 8.0f;
        //	Baseline is `ascent` pixels below the top of the line.
        float const cursor_x = (float) sz.width - text_w - margin_x;
        float const cursor_y = margin_y + _hudAtlas.ascent;

        std::vector< aaa::text::Vertex2D > verts;
        verts.reserve( 128 * 6 );
        aaa::text::layout_text_quads( _hudAtlas, hud_str,
                                       cursor_x, cursor_y, verts );

        if( !verts.empty() && verts.size() <= kHudMaxVertices )
        {
            //	c68 -- backdrop rect first, then text on top.
            //	Backdrop : a single 6-vertex quad slightly larger than
            //	the text bbox for legibility. Bbox = [cursor_x ..
            //	cursor_x + text_w] horizontally, [margin_y ..
            //	margin_y + line_advance] vertically.
            float const pad      = 4.0f;
            float const bd_x0    = cursor_x - pad;
            float const bd_y0    = margin_y - pad * 0.5f;
            float const bd_x1    = cursor_x + text_w + pad;
            float const bd_y1    = margin_y + _hudAtlas.line_advance + pad * 0.5f;

            //	Common screen-size uniform for both pipelines.
            HudScreenSize screen{ (float) sz.width, (float) sz.height, 0.0f, 0.0f };
            _backend->buffer_data( _hudScreenBuf,
                                   sizeof( screen ), &screen,
                                   GOL::BufferUsage::Static );

            if( _hudBackdropProgram != GOL::kInvalidProgramId
                && _hudBackdropVertexBuf != GOL::kInvalidBufferId
                && _hudBackdropTintBuf   != GOL::kInvalidBufferId )
            {
                aaa::text::Vertex2D bd_verts[ 6 ] = {
                    { bd_x0, bd_y0, 0.0f, 0.0f },
                    { bd_x0, bd_y1, 0.0f, 0.0f },
                    { bd_x1, bd_y0, 0.0f, 0.0f },
                    { bd_x1, bd_y0, 0.0f, 0.0f },
                    { bd_x0, bd_y1, 0.0f, 0.0f },
                    { bd_x1, bd_y1, 0.0f, 0.0f },
                };
                _backend->buffer_data( _hudBackdropVertexBuf,
                                       sizeof( bd_verts ), bd_verts,
                                       GOL::BufferUsage::Dynamic );

                _backend->bind_program( _hudBackdropProgram );
                _backend->bind_vertex_buffer( _hudBackdropVertexBuf, 0, 0 );
                _backend->bind_vertex_buffer( _hudScreenBuf, 1, 0 );
                _backend->bind_fragment_buffer( _hudBackdropTintBuf, 0, 0 );
                _backend->draw_arrays( GOL::PrimitiveType::Triangles, 0, 6 );
            }

            //	Text on top of the backdrop.
            _backend->buffer_data( _hudVertexBuf,
                                   verts.size() * sizeof( aaa::text::Vertex2D ),
                                   verts.data(),
                                   GOL::BufferUsage::Dynamic );

            _backend->bind_program( _hudProgram );
            _backend->bind_fragment_texture( _hudAtlasTex, 0 );
            _backend->bind_vertex_buffer( _hudVertexBuf, 0, 0 );
            _backend->bind_vertex_buffer( _hudScreenBuf, 1, 0 );
            _backend->bind_fragment_buffer( _hudTintBuf, 0, 0 );
            _backend->draw_arrays( GOL::PrimitiveType::Triangles, 0,
                                   std::uint32_t( verts.size() ) );
        }
    }

    //	---- c146 : MEU-script HUD overlay --------------------------------
    //	After the runner's render_frame returns, read back any text the
    //	Lua script queued via aaa.draw_hud_text() and emit a second set
    //	of glyph quads on top of the c61 FPS HUD. Uses the same atlas +
    //	text MSL ; only the cursor origin + vertex buffer differ. Logical
    //	"second pass" per the c146 spec ; physically shares the active
    //	window render pass to stay compatible with begin_window_render_pass
    //	/ present_window's one-per-frame contract.
    if( _meuRenderActive && _meuRunner
        && _hudProgram      != GOL::kInvalidProgramId
        && _hudAtlasTex     != GOL::kInvalidTextureId
        && _meuHudVertexBuf != GOL::kInvalidBufferId
        && _hudScreenBuf    != GOL::kInvalidBufferId
        && _hudTintBuf      != GOL::kInvalidBufferId )
    {
        std::string const meu_text = _meuRunner->get_pending_hud_text();
        if( !meu_text.empty() )
        {
            //	Anchor to the top-left, beneath an 8px margin. ascent is
            //	already in pixels at the build-time font size.
            float const margin_x = 12.0f;
            float const margin_y = 8.0f;
            float const cursor_x = margin_x;
            float const cursor_y = margin_y + _hudAtlas.ascent;

            std::vector< aaa::text::Vertex2D > verts;
            verts.reserve( 256 * 6 );
            aaa::text::layout_text_quads( _hudAtlas, meu_text.c_str(),
                                           cursor_x, cursor_y, verts );

            if( !verts.empty() && verts.size() <= kHudMaxVertices )
            {
                //	_hudScreenBuf was already uploaded above with the
                //	current drawable size by the FPS HUD branch when it
                //	ran. If the FPS HUD branch was inactive, populate
                //	the screen-size uniform now.
                HudScreenSize screen{ (float) sz.width, (float) sz.height,
                                      0.0f, 0.0f };
                _backend->buffer_data( _hudScreenBuf,
                                       sizeof( screen ), &screen,
                                       GOL::BufferUsage::Static );

                _backend->buffer_data( _meuHudVertexBuf,
                                       verts.size() * sizeof( aaa::text::Vertex2D ),
                                       verts.data(),
                                       GOL::BufferUsage::Dynamic );

                _backend->bind_program( _hudProgram );
                _backend->bind_fragment_texture( _hudAtlasTex, 0 );
                _backend->bind_vertex_buffer( _meuHudVertexBuf, 0, 0 );
                _backend->bind_vertex_buffer( _hudScreenBuf, 1, 0 );
                _backend->bind_fragment_buffer( _hudTintBuf, 0, 0 );
                _backend->draw_arrays( GOL::PrimitiveType::Triangles, 0,
                                       std::uint32_t( verts.size() ) );
            }
        }
    }

    //	present_window auto-ends the encoder if still open ; we keep the
    //	HUD draws inside the same render pass as the catalog draw so the
    //	overlay blends against fresh pixels (no intermediate copy needed).
    _backend->present_window( drawable_cpp );

    auto const t_frame_end = std::chrono::steady_clock::now();
    std::uint64_t const frame_ns = static_cast< std::uint64_t >(
        std::chrono::duration_cast< std::chrono::nanoseconds >(
            t_frame_end - t_frame_start ).count() );

    //	Skip frame 0 -- it pays the one-shot Metal pipeline-state + first-
    //	drawable allocation cost which would dwarf the steady-state average.
    if( _frameCount > 0 )
    {
        _perfTotalNs += frame_ns;
        if( frame_ns > _perfMaxNs ) _perfMaxNs = frame_ns;
        if( frame_ns < _perfMinNs ) _perfMinNs = frame_ns;
        ++_perfMeasuredFrames;

        //	Rolling-window FPS source for the HUD (continuation 61).
        _hudRecentFrameNs[ _hudRecentFrameWriteIx ] = frame_ns;
        _hudRecentFrameWriteIx = ( _hudRecentFrameWriteIx + 1 )
            % int( sizeof( _hudRecentFrameNs ) / sizeof( _hudRecentFrameNs[ 0 ] ) );
        int const cap = int( sizeof( _hudRecentFrameNs ) / sizeof( _hudRecentFrameNs[ 0 ] ) );
        if( _hudRecentFrameCount < cap ) ++_hudRecentFrameCount;
    }

    ++_frameCount;
    if( _maxFrames > 0 && _frameCount >= _maxFrames )
    {
        _shouldTerminate = YES;
        //	Emit a single line consumable by the aaaseed_app_frame_perf
        //	CTest. Format is stable -- update both ends in lockstep.
        if( _perfMeasuredFrames > 0 )
        {
            double const avg_ms = double( _perfTotalNs ) / 1.0e6 / double( _perfMeasuredFrames );
            double const max_ms = double( _perfMaxNs ) / 1.0e6;
            double const min_ms = double( _perfMinNs ) / 1.0e6;
            std::fprintf( stdout,
                "[AAASeedApp.Perf] frames=%ld measured=%ld avg_ms=%.4f max_ms=%.4f min_ms=%.4f\n",
                (long) _frameCount, (long) _perfMeasuredFrames,
                avg_ms, max_ms, min_ms );
            std::fflush( stdout );
        }
    }
}

- (void)dealloc
{
    //	c144 : tear the MEU runner down FIRST so its cached programs +
    //	buffers + textures get released while the backend is still alive.
    //	`_meuRunner.reset()` triggers RunnerImpl::~RunnerImpl which calls
    //	unload() -- closes lua_State, deletes programs / buffers / texture
    //	via the backend pointer it still holds.
    //	c147-A : clear the runner's widget pointer FIRST so any final
    //	on_frame from a destructor path doesn't dispatch into a half-
    //	dead widget system. Then drop the runner ; then drop the widget
    //	system (which deletes its program + buffer via the backend that
    //	is still alive at this point in dealloc).
    if( _meuRunner ) _meuRunner->set_widget_system( nullptr );
    //	c148-A : clear the reload callback BEFORE resetting the runner so
    //	the captured raw Runner* never lingers after Runner destruction.
    if( _widgetSystem ) _widgetSystem->set_reload_callback( std::function< void() >{} );
    // c152-D : studio event monitor + studio teardown retired.
    _meuRunner.reset();
    _widgetSystem.reset();

    if( _backend )
    {
        if( _triangleProgram   != GOL::kInvalidProgramId )
            _backend->delete_program( _triangleProgram );
        if( _catalogProgram    != GOL::kInvalidProgramId )
            _backend->delete_program( _catalogProgram );
        if( _catalogSrcTex     != GOL::kInvalidTextureId )
            _backend->delete_texture( _catalogSrcTex );
        if( _catalogUniformBuf != GOL::kInvalidBufferId )
            _backend->delete_buffer(  _catalogUniformBuf );
        if( _hudProgram        != GOL::kInvalidProgramId )
            _backend->delete_program( _hudProgram );
        if( _hudAtlasTex       != GOL::kInvalidTextureId )
            _backend->delete_texture( _hudAtlasTex );
        if( _hudVertexBuf      != GOL::kInvalidBufferId )
            _backend->delete_buffer(  _hudVertexBuf );
        if( _hudScreenBuf      != GOL::kInvalidBufferId )
            _backend->delete_buffer(  _hudScreenBuf );
        if( _hudTintBuf        != GOL::kInvalidBufferId )
            _backend->delete_buffer(  _hudTintBuf );
        if( _hudBackdropProgram   != GOL::kInvalidProgramId )
            _backend->delete_program( _hudBackdropProgram );
        if( _hudBackdropVertexBuf != GOL::kInvalidBufferId )
            _backend->delete_buffer(  _hudBackdropVertexBuf );
        if( _hudBackdropTintBuf   != GOL::kInvalidBufferId )
            _backend->delete_buffer(  _hudBackdropTintBuf );
        if( _meuHudVertexBuf      != GOL::kInvalidBufferId )
            _backend->delete_buffer(  _meuHudVertexBuf );
    }
    [super dealloc];
}

@end
