// tests/native/particle_portal_script_test.cpp
//
// c157 : PURE-LUA unit coverage for the particle_portal sample MEU. A bare
// lua_State with recording stubs (core aaa.* + aaa.ui.* sliders + mouse_xy)
// loads the sample and asserts the MEU contract : selects the
// aaa_particle_portal shader, pushes the full force-uniform set
// (int mode + time float + 3 vec4 slots), builds the slider panel, queues
// HUD text, draws exactly once per frame -- and the embedded control-rate
// Perlin module is alive and deterministic.

#include <gtest/gtest.h>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <string>

namespace
{

char const* kStubPrelude = R"LUA(
calls = { use_shader = {}, draws = 0, hud = "",
          ints = {}, floats = {}, vec4s = {}, sliders = {} }
aaa = {
  log                  = function(_) end,
  time                 = function() return 2.5 end,
  mouse_xy             = function() return 400, 300 end,
  use_shader           = function(n) calls.use_shader[#calls.use_shader+1] = n end,
  set_uniform_int      = function(i, v) calls.ints[i] = v end,
  set_uniform_float    = function(i, v) calls.floats[i] = v end,
  set_uniform_vec4     = function(i, x, y, z, w)
                           calls.vec4s[i] = { x, y, z, w }
                         end,
  draw_hud_text        = function(s) calls.hud = s end,
  draw_fullscreen_quad = function() calls.draws = calls.draws + 1 end,
  ui = {
    begin_panel = function(title) calls.panel = title end,
    end_panel   = function() end,
    slider      = function(label, value, lo, hi)
                    calls.sliders[#calls.sliders+1] = label
                    return value          -- idle frame : passthrough
                  end,
  },
}
)LUA";

class PortalScript : public ::testing::Test
{
protected:
    lua_State* L = nullptr;

    void SetUp() override
    {
        L = luaL_newstate();
        ASSERT_NE( L, nullptr );
        luaL_openlibs( L );
        ASSERT_EQ( luaL_dostring( L, kStubPrelude ), 0 )
            << lua_tostring( L, -1 );
        ASSERT_EQ( luaL_dofile( L, AAA_PORTAL_SAMPLE_LUA ), 0 )
            << ( lua_isstring( L, -1 ) ? lua_tostring( L, -1 ) : "(no msg)" );
        ASSERT_EQ( luaL_dostring( L, "aaa.on_frame(800, 600, 0)" ), 0 )
            << lua_tostring( L, -1 );
    }

    void TearDown() override { if( L ) lua_close( L ); }

    double num( std::string const& e )
    {
        EXPECT_EQ( luaL_dostring( L, ( "return " + e ).c_str() ), 0 )
            << lua_tostring( L, -1 );
        double const v = lua_tonumber( L, -1 );
        lua_pop( L, 1 );
        return v;
    }

    std::string str( std::string const& e )
    {
        EXPECT_EQ( luaL_dostring( L, ( "return tostring(" + e + ")" ).c_str() ), 0 )
            << lua_tostring( L, -1 );
        char const* s = lua_tostring( L, -1 );
        std::string const out = s ? s : "";
        lua_pop( L, 1 );
        return out;
    }
};

}   //	anonymous namespace

TEST_F( PortalScript, SelectsPortalShaderAndDrawsOnce )
{
    EXPECT_EQ( str( "calls.use_shader[1]" ), std::string( "aaa_particle_portal" ) );
    EXPECT_DOUBLE_EQ( num( "calls.draws" ), 1.0 );
    EXPECT_NE( str( "calls.hud" ), std::string( "" ) );
}

TEST_F( PortalScript, PushesFullForceUniformSet )
{
    EXPECT_DOUBLE_EQ( num( "calls.ints[0]" ),   1.0 );    //	mode = real
    EXPECT_DOUBLE_EQ( num( "calls.floats[0]" ), 2.5 );    //	time
    //	vec4[0] = ( pull, push, accel, weight ) -- all present + in range.
    EXPECT_GE( num( "calls.vec4s[0][1]" ), 0.0 );
    EXPECT_LE( num( "calls.vec4s[0][1]" ), 0.9 );          //	pull
    EXPECT_GT( num( "calls.vec4s[0][3]" ), 0.0 );          //	accel
    //	vec4[1] = ( origin from mouse 400/800, 300/600 -> 0.5, 0.5 ... )
    EXPECT_DOUBLE_EQ( num( "calls.vec4s[1][1]" ), 0.5 );
    EXPECT_DOUBLE_EQ( num( "calls.vec4s[1][2]" ), 0.5 );
    EXPECT_GT( num( "calls.vec4s[1][3]" ), 0.0 );          //	radius
    EXPECT_GT( num( "calls.vec4s[1][4]" ), 0.0 );          //	size
    //	vec4[2] = ( brightness, aspect=800/600, dir, 0 )
    EXPECT_GT( num( "calls.vec4s[2][1]" ), 0.0 );
    EXPECT_NEAR( num( "calls.vec4s[2][2]" ), 800.0 / 600.0, 1e-9 );
}

TEST_F( PortalScript, BuildsTheForceSliderPanel )
{
    EXPECT_EQ( str( "calls.panel" ), std::string( "Portal forces" ) );
    //	weight / push / pull / acceleration / radius / size.
    EXPECT_DOUBLE_EQ( num( "#calls.sliders" ), 6.0 );
    EXPECT_EQ( str( "calls.sliders[1]" ), std::string( "weight" ) );
    EXPECT_EQ( str( "calls.sliders[4]" ), std::string( "acceleration" ) );
}

TEST_F( PortalScript, EmbeddedPerlinIsAliveAndDeterministic )
{
    double const a = num( "perlin.noise(0.5, 0.25, 0.75)" );
    double const b = num( "perlin.noise(0.5, 0.25, 0.75)" );
    EXPECT_DOUBLE_EQ( a, b );
    EXPECT_NE( a, 0.0 );
    EXPECT_DOUBLE_EQ( num( "perlin.noise(3, 7, 2)" ), 0.0 );   //	lattice zero
}

TEST_F( PortalScript, TenFramesTenDraws )
{
    ASSERT_EQ( luaL_dostring( L,
        "for f = 1, 9 do aaa.on_frame(800, 600, f) end" ), 0 )
        << lua_tostring( L, -1 );
    EXPECT_DOUBLE_EQ( num( "calls.draws" ), 10.0 );
}
