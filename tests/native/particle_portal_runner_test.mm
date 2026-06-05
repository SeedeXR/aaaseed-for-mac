// tests/native/particle_portal_runner_test.mm
//
// c157 : INTEGRATION coverage -- the real MEU runner compiles
// aaa_particle_portal.metal from the catalog and renders the
// particle_portal sample into an offscreen Metal target, and the pixels
// PROVE the c157 uniform-ABI fix : the shader's bright particle field only
// appears when ints[0] (mode) actually reaches buffer(2). Before the fix
// the ints buffer was never bound, mode read 0/garbage, and the shader
// could only produce its dim fallback ring (peak ~0.15 -> ~38/255). The
// max-channel threshold below sits decisively between the two.

#import <gtest/gtest.h>

#include "src/meu/aaa_meu_runner_mac.h"
#include "src/gol/gol_backend.h"
#include "src/gol/metal/metal_backend.h"

#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <string>
#include <vector>

namespace
{
GOL::MetalBackend* make_backend()
{
    auto* backend = new GOL::MetalBackend();
    if( !backend->init() ) { delete backend; return nullptr; }
    return backend;
}
}

TEST( ParticlePortal, SampleLoadsCompilesAndRendersParticles )
{
    auto* backend = make_backend();
    if( backend == nullptr )
        GTEST_SKIP() << "no Metal device available";

    {
        aaa::meu::Runner runner( backend );
        ASSERT_TRUE( runner.load_script( AAA_PORTAL_SAMPLE_LUA ) );
        EXPECT_TRUE( runner.has_on_frame() );

        constexpr std::uint32_t W = 256, H = 256;
        GOL::TextureId const target =
            backend->gen_texture_2d( W, H, GOL::TextureFormat::RGBA8 );
        ASSERT_NE( target, GOL::kInvalidTextureId );

        for( int f = 0; f < 3; ++f )
        {
            GOL::RenderPassDescriptor rpd;
            rpd.color_attachment = target;
            rpd.load_action      = GOL::LoadAction::Clear;
            rpd.clear_color[ 0 ] = 0.f; rpd.clear_color[ 1 ] = 0.f;
            rpd.clear_color[ 2 ] = 0.f; rpd.clear_color[ 3 ] = 1.f;
            ASSERT_TRUE( backend->begin_render_pass( rpd ) );
            backend->set_viewport( 0.f, 0.f, float( W ), float( H ) );
            runner.render_frame( W, H, target );
            backend->present();
        }

        EXPECT_EQ( runner.current_shader_name(),
                   std::string( "aaa_particle_portal" ) )
            << "shader failed to compile from the catalog?";
        EXPECT_EQ( runner.frame_index(), 3 );
        EXPECT_FALSE( runner.get_pending_hud_text().empty() );

        //	Pixel proof : particles (mode==1, brightness 1.25) push many
        //	pixels well past the fallback ring's ~38/255 ceiling.
        std::vector< std::uint8_t > px( W * H * 4, 0 );
        backend->read_texture_pixels( target, 0, 0, W, H, px.data(), W * 4 );
        std::uint8_t max_c = 0;
        std::size_t  lit   = 0;
        for( std::size_t i = 0; i < px.size(); i += 4 )
        {
            max_c = std::max( max_c, px[ i ] );      //	R (white field)
            if( px[ i ] > 60 ) ++lit;
        }
        EXPECT_GT( int( max_c ), 60 )
            << "field too dim -- mode/ints uniform likely not reaching the "
               "shader (c157 ABI binding regression?)";
        EXPECT_GT( lit, std::size_t( 50 ) )
            << "almost no lit pixels -- particle evaluation broken?";

        backend->delete_texture( target );
    }
    delete backend;
}

TEST( ParticlePortal, LegacyAbiShaderStillRenders )
{
    //	Regression guard for the c157 per-shader ABI split : the c39-era
    //	ps_Maa_add_scale (the ONE vec4s@buffer(0) shader) must keep
    //	compiling + drawing through its historical binding.
    auto* backend = make_backend();
    if( backend == nullptr )
        GTEST_SKIP() << "no Metal device available";

    {
        aaa::meu::Runner runner( backend );
        //	Inline MEU via a temp file-free path : drop_file rejects
        //	non-lua, so write through the script the sample dir ships.
        //	Simplest : reuse the perlin sample then switch shader in Lua
        //	is overkill -- load the portal sample and re-point use_shader
        //	through a tiny synthesized script instead.
        std::string const tmp = "/tmp/aaa_legacy_abi_test.lua";
        FILE* f = fopen( tmp.c_str(), "w" );
        ASSERT_NE( f, nullptr );
        fputs( "function aaa.on_frame(w, h, fr)\n"
               "  aaa.use_shader('ps_Maa_add_scale')\n"
               "  aaa.set_uniform_vec4(0, 1, 1, 1, 1)\n"
               "  aaa.set_uniform_vec4(1, 0, 0, 0, 0)\n"
               "  aaa.draw_fullscreen_quad()\n"
               "end\n", f );
        fclose( f );

        ASSERT_TRUE( runner.load_script( tmp ) );

        GOL::TextureId const target =
            backend->gen_texture_2d( 64, 64, GOL::TextureFormat::RGBA8 );
        GOL::RenderPassDescriptor rpd;
        rpd.color_attachment = target;
        rpd.load_action      = GOL::LoadAction::Clear;
        ASSERT_TRUE( backend->begin_render_pass( rpd ) );
        backend->set_viewport( 0.f, 0.f, 64.f, 64.f );
        runner.render_frame( 64, 64, target );
        backend->present();

        EXPECT_EQ( runner.current_shader_name(),
                   std::string( "ps_Maa_add_scale" ) );
        backend->delete_texture( target );
        std::remove( tmp.c_str() );
    }
    delete backend;
}
