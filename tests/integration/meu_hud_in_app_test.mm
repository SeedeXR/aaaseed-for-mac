//  tests/integration/meu_hud_in_app_test.mm
//
//  c146 : Integration test for the MEU HUD overlay wired into
//  AAASeedMTKView. Verifies :
//    1. AAASeedMTKView's drawInMTKView: drives MEU render_frame which
//       runs hello_world.lua's aaa.draw_hud_text call.
//    2. After the draw, Runner::get_pending_hud_text() returns the
//       Lua-side text -- proves the binding fired AND the host can
//       read it back for rendering.
//    3. The text content matches hello_world.lua's expected format
//       "shader: <name> | frame: <n>" -- proves the c143-C "which
//       shader is active" visual gap is now answered on-screen.
//    4. Glyph-quad count via aaa::text::layout_text_quads on the
//       captured HUD text is NON-ZERO -- proves the host renderer
//       has real geometry to draw (not just a queued string with no
//       visible output).
//
//  Hosting strategy : same as c144 meu_runner_in_app_test.mm -- drive
//  AAASeedMTKView + AAASeedInputView directly via the headless MTKView
//  path. No NSApplication required.
//
//  Doctrine respected :
//    - .mm TU (AAASeedMTKView is ObjC++).
//    - No `o_str`, no `aaa_mem`, no `aaa_str.h`.
//    - Per feedback_ctest_label_first_only.md : LABELS order matters ;
//      `integration` is the primary key and goes first.

#import <Cocoa/Cocoa.h>
#import <MetalKit/MetalKit.h>

#import "src/ui/macos/AAASeedMTKView.h"
#import "src/ui/macos/AAASeedInputView.h"

#include "src/gol/metal/metal_backend.h"
#include "src/meu/aaa_meu_runner_mac.h"
#include "src/text/glyph_atlas.h"

#include <gtest/gtest.h>

#include <fstream>
#include <memory>
#include <string>
#include <vector>

namespace
{

//  Shared MetalBackend across the cohort -- same pattern as c144.
struct SharedBackend
{
    GOL::MetalBackend* get()
    {
        if( !inited ) inited = backend.init();
        return inited ? &backend : nullptr;
    }
    GOL::MetalBackend backend;
    bool              inited{ false };
};

SharedBackend& shared_backend()
{
    static SharedBackend s;
    return s;
}

AAASeedInputView* make_view_with_delegate( GOL::MetalBackend* backend,
                                           AAASeedMTKViewDelegate** out_delegate )
{
    id< MTLDevice > device = (__bridge id< MTLDevice >) backend->get_device();
    NSRect const frame = NSMakeRect( 0, 0, 256, 192 );
    AAASeedInputView* view = [[AAASeedInputView alloc] initWithFrame:frame
                                                              device:device];
    view.colorPixelFormat = MTLPixelFormatRGBA8Unorm;
    view.clearColor       = MTLClearColorMake( 0.0, 0.0, 0.0, 1.0 );
    view.preferredFramesPerSecond = 60;
    [view setPaused:YES];
    [view setEnableSetNeedsDisplay:NO];

    AAASeedMTKViewDelegate* delegate =
        [[AAASeedMTKViewDelegate alloc] initWithBackend:backend maxFrames:0];
    view.delegate = delegate;
    *out_delegate = delegate;
    return view;
}

}   //  anonymous

//  -----------------------------------------------------------------------
//  Test 1 : 5 frames driven through the .app view ; verify the runner
//  exposes a non-empty pending HUD text matching hello_world.lua's
//  format on every frame.
TEST( MeuHudInApp, HelloWorldQueuesHudText )
{
    GOL::MetalBackend* backend = shared_backend().get();
    ASSERT_NE( backend, nullptr ) << "MetalBackend init failed";

    @autoreleasepool
    {
        AAASeedMTKViewDelegate* delegate = nil;
        AAASeedInputView* view = make_view_with_delegate( backend, &delegate );

        aaa::meu::Runner* runner = [delegate meuRunner];
        ASSERT_NE( runner, nullptr );

        //  Drive 5 frames through the view. Each [view draw] fires
        //  drawInMTKView: which calls _meuRunner->render_frame ; the
        //  runner runs hello_world.lua's on_frame which calls
        //  aaa.draw_hud_text with "shader: <name> | frame: <n>".
        for( int i = 0; i < 5; ++i )
        {
            [view draw];

            std::string const hud = runner->get_pending_hud_text();
            EXPECT_FALSE( hud.empty() )
                << "frame " << i << " : HUD text not queued ; "
                << "aaa.draw_hud_text binding misfired.";
            EXPECT_NE( hud.find( "shader:" ), std::string::npos )
                << "frame " << i << " : HUD text doesn't contain "
                << "'shader:' prefix from hello_world.lua. Got: " << hud;
            EXPECT_NE( hud.find( "| frame:" ), std::string::npos )
                << "frame " << i << " : HUD text doesn't contain "
                << "'| frame:' separator from hello_world.lua. Got: " << hud;
        }

        view.delegate = nil;
        [delegate release];
        [view release];
    }
}

//  -----------------------------------------------------------------------
//  Test 2 : after the .app view has rendered, the captured HUD text
//  produces a NON-ZERO quad count when laid out against a fresh glyph
//  atlas. Proves the host renderer has real geometry to draw -- not
//  just a queued string with empty output.
//
//  Builds a small atlas from the bundled SourceCodePro-Medium.ttf
//  (same font the host uses) so the test exercises the same code
//  path the host's HUD render pass walks.
TEST( MeuHudInApp, HudTextProducesNonZeroQuadCount )
{
    GOL::MetalBackend* backend = shared_backend().get();
    ASSERT_NE( backend, nullptr );

    //  Locate the same TTF the .app bundle ships. CMake passes the
    //  source-tree path via AAA_MEU_HUD_FONT_PATH for ctest contexts
    //  where NSBundle has no Resources/fonts/.
    std::string font_path;
#ifdef AAA_MEU_HUD_FONT_PATH
    font_path = AAA_MEU_HUD_FONT_PATH;
#endif
    @autoreleasepool
    {
        NSString* bundled = [[NSBundle mainBundle]
            pathForResource:@"SourceCodePro-Medium"
                     ofType:@"ttf"
                inDirectory:@"fonts"];
        if( bundled ) font_path = std::string( [bundled UTF8String] );
    }
    ASSERT_FALSE( font_path.empty() )
        << "SourceCodePro-Medium.ttf not findable for this test ;"
           " check AAA_MEU_HUD_FONT_PATH define.";

    std::ifstream font_in( font_path, std::ios::binary | std::ios::ate );
    ASSERT_TRUE( font_in.is_open() ) << "open failed : " << font_path;
    std::streamsize const sz = font_in.tellg();
    font_in.seekg( 0, std::ios::beg );
    std::vector< std::uint8_t > font_bytes( (std::size_t) sz );
    ASSERT_TRUE( font_in.read( reinterpret_cast< char* >( font_bytes.data() ), sz ) );

    aaa::text::GlyphAtlas atlas;
    ASSERT_TRUE( aaa::text::build_printable_ascii_atlas(
        font_bytes.data(), font_bytes.size(),
        16.0f, 256, 256, atlas ) );

    @autoreleasepool
    {
        AAASeedMTKViewDelegate* delegate = nil;
        AAASeedInputView* view = make_view_with_delegate( backend, &delegate );

        aaa::meu::Runner* runner = [delegate meuRunner];
        ASSERT_NE( runner, nullptr );

        //  Drive 5 frames so hello_world.lua's on_frame has run and
        //  queued its HUD text. Use the LAST frame's text for the
        //  quad-count check.
        for( int i = 0; i < 5; ++i ) [view draw];

        std::string const hud = runner->get_pending_hud_text();
        ASSERT_FALSE( hud.empty() );

        //  Lay out the captured HUD string into the atlas's quad
        //  geometry. Each printable ASCII char produces 6 vertices ;
        //  hello_world.lua's "shader: <name> | frame: <n>" is well
        //  above 0 printable chars, so quad count must be > 0.
        std::vector< aaa::text::Vertex2D > verts;
        int const quads = aaa::text::layout_text_quads( atlas, hud.c_str(),
                                                         12.0f, 32.0f, verts );
        EXPECT_GT( quads, 0 )
            << "layout_text_quads emitted 0 quads for '" << hud << "' ; "
            << "host renderer would have no geometry to draw.";
        EXPECT_EQ( verts.size(), (std::size_t)( quads * 6 ) )
            << "vertex count mismatches quad count * 6.";

        view.delegate = nil;
        [delegate release];
        [view release];
    }
}
