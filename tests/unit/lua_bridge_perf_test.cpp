// tests/unit/lua_bridge_perf_test.cpp
//
// Performance-profiling test for the Lua -> C -> engine bridge pattern
// landed in continuations 47-51. Mirrors the three-artifact perf doctrine
// (philosophy.md Part 2.95) :
//   1. CPU-side `os_signpost` interval around the measured loop (visible
//      in Instruments / Time Profiler under subsystem com.seedexr.aaaseed,
//      category "performance").
//   2. Wall-clock measurement with std::chrono::steady_clock.
//   3. Budget assertion : average per-call latency for the bridge must
//      stay within a generous bound for the debug build on Apple M-series.
//
// What this measures : the steady-state cost of `lua_pushcfunction +
// lua_setglobal` registration is one-shot ; the loop body times
// repeated `script.fn()` calls from Lua INTO a registered C function
// that does trivial work (returns a constant). This is the "fixed
// overhead" of every AAALUACALL the engine wrapper layer dispatches.
// Future binding ports (continuations 47-51 and beyond) inherit this
// cost ; the test acts as a guard-rail against regressions in the
// vendored Lua 5.1.5 lib or the AAALUACALL macro expansion.
//
// Why this matters for the embed-perf-everywhere doctrine : we now
// have 5 Lua wrapper test suites (lua_engine_bridge, lua_smoke,
// aaalua_header / debug / exp / array) but ZERO perf coverage for the
// hot path they all share -- a registered C function invoked from Lua.
// This test plugs the gap.
//
// CTest label : `perf`.

#include <gtest/gtest.h>

#include <chrono>
#include <cstdio>

#include <os/log.h>
#include <os/signpost.h>

extern "C" {
    #include "lua.h"
    #include "lualib.h"
    #include "lauxlib.h"
}

namespace
{
    //	Minimal C function : reads one Lua number from the stack, pushes
    //	a constant back. Models the cheapest meaningful binding -- any
    //	real engine binding (CLAMP, MIN, MAX, math.noise1, etc.) does at
    //	least this much work plus the actual computation.
    int l_passthrough( lua_State* L )
    {
        double const a = lua_tonumber( L, 1 );
        lua_pushnumber( L, a + 1.0 );
        return 1;
    }

    //	Subsystem-shared with NSightEvents.h Mac branch + gol_metal_perf
    //	so Instruments groups all aaaseed signposts under one category.
    os_log_t perf_log()
    {
        static os_log_t log = os_log_create( "com.seedexr.aaaseed", "performance" );
        return log;
    }
}

TEST( LuaBridgePerf, RegisteredFunctionCallStaysUnderBudget )
{
    //	Budget : 1.0 us per call (1000 ns) for the debug build on Apple
    //	M-series. A vanilla Lua 5.1.5 C call should land well under this
    //	(typical : 50-200 ns) ; the loose bound catches real regressions
    //	without flapping on debug-build measurement noise.
    constexpr int    kIterations       = 100000;
    constexpr double kBudgetNsPerCall  = 1000.0;
    constexpr int    kWarmupCalls      = 1000;

    lua_State* L = luaL_newstate();
    ASSERT_NE( L, nullptr );
    luaL_openlibs( L );

    lua_pushcfunction( L, l_passthrough );
    lua_setglobal( L, "passthrough" );

    //	Build a Lua chunk that runs the hot loop entirely inside the
    //	VM -- this measures the round-trip cost (Lua dispatch + C entry
    //	+ stack pop/push + return), not the cost of repeated luaL_dostring
    //	parsing.
    char script[ 256 ];
    std::snprintf( script, sizeof( script ),
        "local pt = passthrough\n"
        "local s = 0\n"
        "for i = 1, %d do s = pt( s ) end\n"
        "return s\n",
        kIterations );

    //	Warmup : compile + first few calls pay JIT/dispatch table init
    //	(though vanilla Lua 5.1 has no JIT, the bytecode interpreter
    //	still benefits from icache warmup). Run a smaller warmup script
    //	and discard its time.
    char warmup_script[ 256 ];
    std::snprintf( warmup_script, sizeof( warmup_script ),
        "local pt = passthrough\n"
        "for i = 1, %d do pt( 0 ) end\n",
        kWarmupCalls );
    ASSERT_EQ( luaL_dostring( L, warmup_script ), 0 )
        << lua_tostring( L, -1 );

    //	Measured : single os_signpost interval covering the whole loop.
    os_signpost_id_t spid = os_signpost_id_generate( perf_log() );
    os_signpost_interval_begin( perf_log(), spid, "lua_bridge.loop",
                                "iterations=%d", kIterations );

    auto const t0 = std::chrono::steady_clock::now();

    int const ret = luaL_dostring( L, script );

    auto const t1 = std::chrono::steady_clock::now();
    os_signpost_interval_end( perf_log(), spid, "lua_bridge.loop",
                              "iterations=%d", kIterations );

    ASSERT_EQ( ret, 0 ) << lua_tostring( L, -1 );

    //	Sanity-check : the script should have summed N calls of `+1`
    //	starting from 0, so result == iterations.
    ASSERT_TRUE( lua_isnumber( L, -1 ) );
    EXPECT_DOUBLE_EQ( lua_tonumber( L, -1 ), double( kIterations ) );
    lua_pop( L, 1 );

    auto const total_ns = std::chrono::duration< double, std::nano >( t1 - t0 ).count();
    double const avg_ns = total_ns / kIterations;

    std::printf( "[LuaBridgePerf] %d Lua->C calls in %.3f ms "
                 "(avg %.2f ns/call ; budget %.1f ns)\n",
                 kIterations, total_ns / 1e6, avg_ns, kBudgetNsPerCall );

    EXPECT_LT( avg_ns, kBudgetNsPerCall )
        << "Average Lua->C bridge call " << avg_ns
        << " ns exceeds budget " << kBudgetNsPerCall << " ns.";

    lua_close( L );
}
