// tests/native/meu_runner_perlin_test.mm
//
// c155 : INTEGRATION coverage -- the real aaa::meu::Runner loads the
// perlin_noise sample, compiles the aaa_noise_real shader from the Path A
// catalog, and renders frames into an offscreen Metal target (same
// begin_render_pass / set_viewport / render_frame / present sequence the
// MTKView host drives). Also exercises the c155 has_on_frame() diagnostic
// with a pure-library-module script -- the exact "my perlin.lua didn't
// work" failure mode this sample exists to teach.

#import <gtest/gtest.h>

#include "src/meu/aaa_meu_runner_mac.h"
#include "src/gol/gol_backend.h"
#include "src/gol/metal/metal_backend.h"

#include <cstdio>
#include <filesystem>
#include <fstream>
#include <string>

namespace
{

GOL::MetalBackend* make_backend()
{
    auto* backend = new GOL::MetalBackend();
    if( !backend->init() )
    {
        delete backend;
        return nullptr;
    }
    return backend;
}

//	A pure library module : valid Lua, returns a table, defines NO
//	aaa.on_frame -- loads fine, renders nothing. (The user's original
//	perlin.lua had exactly this shape.)
constexpr char const* kModuleOnlyScript =
    "local m = {}\n"
    "function m.noise(x) return 0 end\n"
    "return m\n";

std::filesystem::path write_temp_lua( char const* stem, char const* body )
{
    auto tmp = std::filesystem::temp_directory_path()
             / ( std::string( "aaa_perlin_itest_" ) + stem + ".lua" );
    std::ofstream out( tmp );
    out << body;
    out.close();
    return tmp;
}

}   //	anonymous namespace

TEST( MeuRunnerPerlin, SampleLoadsAndDefinesOnFrame )
{
    auto* backend = make_backend();
    if( backend == nullptr )
        GTEST_SKIP() << "no Metal device available";

    {
        aaa::meu::Runner runner( backend );
        ASSERT_TRUE( runner.load_script( AAA_PERLIN_SAMPLE_LUA ) );
        EXPECT_TRUE( runner.has_on_frame() );
    }
    delete backend;
}

TEST( MeuRunnerPerlin, ModuleOnlyScriptLoadsButHasNoOnFrame )
{
    auto* backend = make_backend();
    if( backend == nullptr )
        GTEST_SKIP() << "no Metal device available";

    auto const tmp = write_temp_lua( "module_only", kModuleOnlyScript );
    {
        aaa::meu::Runner runner( backend );
        //	The c155 diagnosis : a library module LOADS successfully...
        EXPECT_TRUE( runner.load_script( tmp.string() ) );
        //	...but the runner can tell it will never render.
        EXPECT_FALSE( runner.has_on_frame() );
    }
    std::filesystem::remove( tmp );
    delete backend;
}

TEST( MeuRunnerPerlin, HasOnFrameFalseWhenNothingLoaded )
{
    auto* backend = make_backend();
    if( backend == nullptr )
        GTEST_SKIP() << "no Metal device available";
    {
        aaa::meu::Runner runner( backend );
        EXPECT_FALSE( runner.has_on_frame() );
    }
    delete backend;
}

TEST( MeuRunnerPerlin, RendersFramesOffscreen )
{
    auto* backend = make_backend();
    if( backend == nullptr )
        GTEST_SKIP() << "no Metal device available";

    {
        aaa::meu::Runner runner( backend );
        ASSERT_TRUE( runner.load_script( AAA_PERLIN_SAMPLE_LUA ) );

        GOL::TextureId const target =
            backend->gen_texture_2d( 64, 64, GOL::TextureFormat::RGBA8 );
        ASSERT_NE( target, GOL::kInvalidTextureId );

        //	Same per-frame sequence the MTKView host drives, against an
        //	offscreen color target (present() commits + waits).
        for( int f = 0; f < 3; ++f )
        {
            GOL::RenderPassDescriptor rpd;
            rpd.color_attachment = target;
            rpd.load_action      = GOL::LoadAction::Clear;
            ASSERT_TRUE( backend->begin_render_pass( rpd ) );
            backend->set_viewport( 0.f, 0.f, 64.f, 64.f );
            runner.render_frame( 64, 64, target );
            backend->present();
        }

        //	The script selected the GPU Perlin revival, advanced 3 frames,
        //	and queued a HUD line containing a live CPU-noise sample.
        EXPECT_EQ( runner.current_shader_name(), std::string( "aaa_noise_real" ) );
        EXPECT_EQ( runner.frame_index(), 3 );
        EXPECT_FALSE( runner.get_pending_hud_text().empty() );

        backend->delete_texture( target );
    }
    delete backend;
}
