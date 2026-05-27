// tests/unit/aaa_event_adapter_mac_test.cpp
//
// Phase 4 / v1 ship-gate (todo.md L29) : engine-side `c_event_mouse` +
// `c_event_keyboard` adapter coverage. Plain-C++ TU (no NSEvent synth ;
// drain feeds from EventBridge::push so we never construct an
// AAASeedInputView and never touch AppKit at test runtime).
//
// Hermetic test : links only the aaaseed_event_bridge static lib (which
// transitively brings Cocoa / MetalKit ; we don't call into them).
// NO link to aaaseed_code_utils -- preserves the c104 hermetic doctrine.

#include <gtest/gtest.h>

#include <cstdint>
#include <type_traits>
#include <vector>

#include "src/ui/macos/aaa_event_adapter_mac.h"
#include "src/ui/macos/aaa_event_bridge.h"

namespace
{

using aaa::input::EngineEvent;
using aaa::input::EventBridge;
using aaa::input::KeyboardEventOut;
using aaa::input::MouseEventOut;

}   //	anonymous namespace

//	Test 1 : pure conversion produces correct type/button/x/y from
//	synthetic input. No bridge involvement.
TEST( EventAdapterMac, MouseEventToEngineProducesCorrectFields )
{
    MouseEventOut const out = aaa::input::mouse_event_to_engine(
            /* x */ 17.5,
            /* y */ 42.25,
            /* button */ 1,                 //	right
            /* type_code */ 1,              //	down
            /* modifiers */ 0,
            /* timestamp_ns */ 12345u );

    EXPECT_EQ( out.type,         1 );
    EXPECT_EQ( out.button,       1 );
    EXPECT_DOUBLE_EQ( out.x,     17.5 );
    EXPECT_DOUBLE_EQ( out.y,     42.25 );
    EXPECT_EQ( out.timestamp_ns, 12345u );
    EXPECT_EQ( out.modifiers,    0 );
    EXPECT_DOUBLE_EQ( out.scroll_dx, 0.0 );
    EXPECT_DOUBLE_EQ( out.scroll_dy, 0.0 );
}

//	Test 2 : keyboard_event_to_engine distinguishes down vs up.
TEST( EventAdapterMac, KeyboardEventDownVsUp )
{
    KeyboardEventOut const down = aaa::input::keyboard_event_to_engine(
            /* key_code */ 65,              //	'a'
            /* down */ true,
            /* modifiers */ 0,
            /* timestamp_ns */ 7u );
    KeyboardEventOut const up   = aaa::input::keyboard_event_to_engine(
            /* key_code */ 65,
            /* down */ false,
            /* modifiers */ 0,
            /* timestamp_ns */ 8u );

    EXPECT_EQ( down.type,     0 );          //	key_down
    EXPECT_EQ( up.type,       1 );          //	key_up
    EXPECT_EQ( down.key_code, 65 );
    EXPECT_EQ( up.key_code,   65 );
    EXPECT_EQ( down.timestamp_ns, 7u );
    EXPECT_EQ( up.timestamp_ns,   8u );
}

//	Test 3 : modifier bitfield encoding round-trips.
TEST( EventAdapterMac, ModifierBitfieldEncoding )
{
    using namespace aaa::input;

    //	Distinct bits, no overlap.
    EXPECT_NE( kModShift, kModCtrl );
    EXPECT_NE( kModShift, kModAlt );
    EXPECT_NE( kModShift, kModCmd );
    EXPECT_NE( kModCtrl,  kModAlt );
    EXPECT_NE( kModCtrl,  kModCmd );
    EXPECT_NE( kModAlt,   kModCmd );

    int const both = kModShift | kModCmd;
    EXPECT_NE( both & kModShift, 0 );
    EXPECT_NE( both & kModCmd,   0 );
    EXPECT_EQ( both & kModCtrl,  0 );
    EXPECT_EQ( both & kModAlt,   0 );

    //	Round-trip through mouse_event_to_engine.
    MouseEventOut const m = mouse_event_to_engine( 0.0, 0.0, 0, 0, both, 0u );
    EXPECT_EQ( m.modifiers, both );
    EXPECT_NE( m.modifiers & kModShift, 0 );
    EXPECT_NE( m.modifiers & kModCmd,   0 );

    //	Round-trip through keyboard_event_to_engine.
    KeyboardEventOut const k = keyboard_event_to_engine( 0, true, both, 0u );
    EXPECT_EQ( k.modifiers, both );
}

//	Test 4 : timestamp_ns is preserved (not truncated, not zero).
TEST( EventAdapterMac, TimestampPreserved )
{
    //	Non-trivial timestamp that exercises the full 64-bit width.
    std::uint64_t const ts = 0x0123456789ABCDEFull;

    MouseEventOut const m = aaa::input::mouse_event_to_engine(
            0.0, 0.0, 0, 0, 0, ts );
    EXPECT_EQ( m.timestamp_ns, ts );
    EXPECT_NE( m.timestamp_ns, 0u );

    KeyboardEventOut const k = aaa::input::keyboard_event_to_engine(
            0, true, 0, ts );
    EXPECT_EQ( k.timestamp_ns, ts );
    EXPECT_NE( k.timestamp_ns, 0u );
}

//	Test 5 : drain_to_engine_mouse on an empty bridge returns empty.
TEST( EventAdapterMac, DrainEmptyBridgeReturnsEmpty )
{
    EventBridge bridge;
    std::vector< MouseEventOut > const mouse = aaa::input::drain_to_engine_mouse( bridge );
    EXPECT_TRUE( mouse.empty() );

    std::vector< KeyboardEventOut > const kbd = aaa::input::drain_to_engine_keyboard( bridge );
    EXPECT_TRUE( kbd.empty() );
}

//	Test 6 : drain_to_engine_mouse returns multiple events when the
//	bridge has multiple pushed.
TEST( EventAdapterMac, DrainMultipleMouseEvents )
{
    EventBridge bridge;

    EngineEvent move;
    move.kind = EngineEvent::Kind::MOUSE_MOVE;
    move.x    = 10.0;
    move.y    = 20.0;
    bridge.push( move );

    EngineEvent down;
    down.kind   = EngineEvent::Kind::MOUSE_DOWN;
    down.button = 0;            //	left
    down.x      = 11.0;
    down.y      = 21.0;
    bridge.push( down );

    EngineEvent up;
    up.kind   = EngineEvent::Kind::MOUSE_UP;
    up.button = 0;
    up.x      = 12.0;
    up.y      = 22.0;
    bridge.push( up );

    std::vector< MouseEventOut > const out = aaa::input::drain_to_engine_mouse( bridge );
    ASSERT_EQ( out.size(), 3u );

    EXPECT_EQ( out[ 0 ].type, 0 );  //	move
    EXPECT_DOUBLE_EQ( out[ 0 ].x, 10.0 );
    EXPECT_DOUBLE_EQ( out[ 0 ].y, 20.0 );

    EXPECT_EQ( out[ 1 ].type, 1 );  //	down
    EXPECT_EQ( out[ 1 ].button, 0 );
    EXPECT_DOUBLE_EQ( out[ 1 ].x, 11.0 );

    EXPECT_EQ( out[ 2 ].type, 2 );  //	up
    EXPECT_EQ( out[ 2 ].button, 0 );
    EXPECT_DOUBLE_EQ( out[ 2 ].x, 12.0 );

    //	Bridge is fully drained.
    EXPECT_EQ( bridge.pending(), 0u );
}

//	Test 7 : scroll event encoding -- type=4 + dx/dy preserved.
TEST( EventAdapterMac, ScrollEventEncoding )
{
    EventBridge bridge;

    EngineEvent scroll;
    scroll.kind = EngineEvent::Kind::SCROLL;
    scroll.dx   = -2.5;
    scroll.dy   =  4.0;
    bridge.push( scroll );

    std::vector< MouseEventOut > const out = aaa::input::drain_to_engine_mouse( bridge );
    ASSERT_EQ( out.size(), 1u );
    EXPECT_EQ( out[ 0 ].type,           4 );
    EXPECT_DOUBLE_EQ( out[ 0 ].scroll_dx, -2.5 );
    EXPECT_DOUBLE_EQ( out[ 0 ].scroll_dy,  4.0 );
}

//	Test 8 : engine-shape POD is trivially-copyable + trivially-
//	destructible. Verified via static_assert (compile-time) AND a
//	runtime EXPECT_TRUE so the test reports a clear failure if the
//	POD shape ever drifts.
TEST( EventAdapterMac, EngineShapeIsTriviallyCopyableAndDestructible )
{
    static_assert( std::is_trivially_copyable_v   < MouseEventOut    >,
                   "MouseEventOut must be trivially copyable" );
    static_assert( std::is_trivially_destructible_v < MouseEventOut  >,
                   "MouseEventOut must be trivially destructible" );
    static_assert( std::is_trivially_copyable_v   < KeyboardEventOut >,
                   "KeyboardEventOut must be trivially copyable" );
    static_assert( std::is_trivially_destructible_v < KeyboardEventOut >,
                   "KeyboardEventOut must be trivially destructible" );

    EXPECT_TRUE( ( std::is_trivially_copyable_v     < MouseEventOut    > ) );
    EXPECT_TRUE( ( std::is_trivially_destructible_v < MouseEventOut    > ) );
    EXPECT_TRUE( ( std::is_trivially_copyable_v     < KeyboardEventOut > ) );
    EXPECT_TRUE( ( std::is_trivially_destructible_v < KeyboardEventOut > ) );
}
