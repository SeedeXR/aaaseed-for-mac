// tests/native/perlin_meu_script_test.cpp
//
// c155 : PURE-LUA unit coverage for the perlin_noise sample MEU
// (bundle/macos/meu/Samples/perlin_noise/perlin_noise.lua). No Metal, no
// runner : a bare lua_State with a recording stub `aaa` table loads the
// sample and the test asserts (a) the embedded Perlin module's math
// (determinism, lattice zeros, range bound, reseed reproducibility) and
// (b) the MEU contract -- on_frame selects aaa_noise_real, sets uniforms,
// queues HUD text and emits exactly one fullscreen-quad draw per frame.

#include <gtest/gtest.h>

extern "C" {
#include "lua.h"
#include "lualib.h"
#include "lauxlib.h"
}

#include <cmath>
#include <string>

namespace
{

//	Recording stubs : every aaa.* the sample touches, implemented in Lua
//	so the test TU stays free of C-binding plumbing. `calls` accumulates
//	what on_frame did.
char const* kStubPrelude = R"LUA(
calls = { use_shader = {}, draws = 0, hud = "", uniforms = 0 }
aaa = {
  log                  = function(_) end,
  time                 = function() return 1.25 end,
  use_shader           = function(n) calls.use_shader[#calls.use_shader+1] = n end,
  set_uniform_int      = function() calls.uniforms = calls.uniforms + 1 end,
  set_uniform_float    = function() calls.uniforms = calls.uniforms + 1 end,
  set_uniform_vec4     = function() calls.uniforms = calls.uniforms + 1 end,
  draw_hud_text        = function(s) calls.hud = s end,
  draw_fullscreen_quad = function() calls.draws = calls.draws + 1 end,
}
)LUA";

class PerlinMeuScript : public ::testing::Test
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
        ASSERT_EQ( luaL_dofile( L, AAA_PERLIN_SAMPLE_LUA ), 0 )
            << ( lua_isstring( L, -1 ) ? lua_tostring( L, -1 ) : "(no msg)" );
    }

    void TearDown() override
    {
        if( L ) lua_close( L );
    }

    //	Evaluate a Lua expression returning one number.
    double eval_num( std::string const& expr )
    {
        std::string const chunk = "return " + expr;
        EXPECT_EQ( luaL_dostring( L, chunk.c_str() ), 0 )
            << lua_tostring( L, -1 );
        double const v = lua_tonumber( L, -1 );
        lua_pop( L, 1 );
        return v;
    }

    //	Evaluate a Lua expression returning one string.
    std::string eval_str( std::string const& expr )
    {
        std::string const chunk = "return tostring(" + expr + ")";
        EXPECT_EQ( luaL_dostring( L, chunk.c_str() ), 0 )
            << lua_tostring( L, -1 );
        char const* s = lua_tostring( L, -1 );
        std::string const out = s ? s : "";
        lua_pop( L, 1 );
        return out;
    }
};

}   //	anonymous namespace

//	--- Perlin math -----------------------------------------------------------

TEST_F( PerlinMeuScript, NoiseIsZeroOnIntegerLattice )
{
    //	Improved Noise is exactly 0 at every integer lattice point
    //	(u=v=w=0 collapses the lerp tree onto grad(hash, 0,0,0) == 0).
    EXPECT_DOUBLE_EQ( eval_num( "perlin.noise(0, 0, 0)" ),   0.0 );
    EXPECT_DOUBLE_EQ( eval_num( "perlin.noise(3, 7, 2)" ),   0.0 );
    EXPECT_DOUBLE_EQ( eval_num( "perlin.noise(-5, 11, 0)" ), 0.0 );
}

TEST_F( PerlinMeuScript, NoiseIsDeterministic )
{
    double const a = eval_num( "perlin.noise(0.5, 0.25, 0.75)" );
    double const b = eval_num( "perlin.noise(0.5, 0.25, 0.75)" );
    EXPECT_DOUBLE_EQ( a, b );
    //	And non-trivial (not the lattice-zero degenerate case).
    EXPECT_NE( a, 0.0 );
}

TEST_F( PerlinMeuScript, ReseedIsReproducible )
{
    double const a = eval_num(
        "(function() perlin.seed(42) return perlin.noise(0.3, 0.6, 0.9) end)()" );
    double const b = eval_num(
        "(function() perlin.seed(42) return perlin.noise(0.3, 0.6, 0.9) end)()" );
    EXPECT_DOUBLE_EQ( a, b );
}

TEST_F( PerlinMeuScript, OctaveStaysInUnitRange )
{
    //	3D Perlin's theoretical bound is sqrt(3)/2 ~ 0.866 ; the fBm
    //	normalization keeps the octave sum within the same bound. Sweep
    //	a diagonal and assert |v| <= 1 everywhere.
    double const worst = eval_num( R"LUA(
        (function()
           local m = 0
           for i = 0, 400 do
             local v = perlin.octave(i * 0.073, i * 0.041, i * 0.019, 4, 0.5)
             if math.abs(v) > m then m = math.abs(v) end
           end
           return m
         end)() )LUA" );
    EXPECT_LE( worst, 1.0 );
    EXPECT_GT( worst, 0.0 );   //	the sweep is not silently all-zero
}

//	--- MEU contract ----------------------------------------------------------

TEST_F( PerlinMeuScript, DefinesOnFrame )
{
    EXPECT_EQ( eval_str( "type(aaa.on_frame)" ), std::string( "function" ) );
}

TEST_F( PerlinMeuScript, OnFrameSelectsNoiseShaderAndDraws )
{
    ASSERT_EQ( luaL_dostring( L, "aaa.on_frame(64, 64, 0)" ), 0 )
        << lua_tostring( L, -1 );
    EXPECT_EQ( eval_str( "calls.use_shader[1]" ), std::string( "aaa_noise_real" ) );
    EXPECT_DOUBLE_EQ( eval_num( "calls.draws" ), 1.0 );
    EXPECT_GE( eval_num( "calls.uniforms" ), 4.0 );   //	int + float + 2x vec4
    EXPECT_NE( eval_str( "calls.hud" ), std::string( "" ) );
}

TEST_F( PerlinMeuScript, EveryFrameDrawsExactlyOnce )
{
    ASSERT_EQ( luaL_dostring( L,
        "for f = 0, 9 do aaa.on_frame(64, 64, f) end" ), 0 )
        << lua_tostring( L, -1 );
    EXPECT_DOUBLE_EQ( eval_num( "calls.draws" ), 10.0 );
}
