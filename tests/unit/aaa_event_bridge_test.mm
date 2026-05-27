// tests/unit/aaa_event_bridge_test.mm
//
// c119-B / Task #150 : EventBridge drain → consume coverage. Synthesizes
// NSEvent keyDown / mouseDown / mouseMoved into AAASeedInputView, drains
// via aaa::input::EventBridge, asserts the EngineEvent FIFO yields the
// expected Kind + payload.
//
// Mirrors input_view_test.mm's NSEvent synthesis pattern. The scroll path
// is exercised in EventBridge by directly setting `view.lastScrollDelta`
// (same workaround input_view_test.mm documents : `+mouseEventWithType:
// NSEventTypeScrollWheel` throws from AppKit).

#import <Cocoa/Cocoa.h>
#import <gtest/gtest.h>

#import "src/ui/macos/AAASeedInputView.h"
#import "src/ui/macos/aaa_event_bridge.h"

namespace
{
    NSEvent* make_key_event( NSEventType type, unsigned short code )
    {
        NSString* chars = @"";
        return [NSEvent keyEventWithType:type
                                location:NSZeroPoint
                           modifierFlags:0
                               timestamp:0
                            windowNumber:0
                                 context:nil
                              characters:chars
             charactersIgnoringModifiers:chars
                               isARepeat:NO
                                 keyCode:code];
    }

    NSEvent* make_mouse_event( NSEventType type, NSPoint loc )
    {
        return [NSEvent mouseEventWithType:type
                                  location:loc
                             modifierFlags:0
                                 timestamp:0
                              windowNumber:0
                                   context:nil
                               eventNumber:0
                                clickCount:1
                                  pressure:1.0];
    }
}

TEST( EventBridge, EmptyDrainYieldsNoEvents )
{
    @autoreleasepool
    {
        AAASeedInputView* view = [[AAASeedInputView alloc]
                                    initWithFrame:NSMakeRect( 0, 0, 64, 64 )
                                           device:nil];
        ASSERT_NE( view, nil );

        aaa::input::EventBridge bridge;
        EXPECT_EQ( bridge.drain_from_view( view ), 0u );
        EXPECT_EQ( bridge.pending(), 0u );

        aaa::input::EngineEvent ev;
        EXPECT_FALSE( bridge.next_event( ev ) );

        [view release];
    }
}

TEST( EventBridge, KeyDownDrainsToKEY_DOWN )
{
    @autoreleasepool
    {
        AAASeedInputView* view = [[AAASeedInputView alloc]
                                    initWithFrame:NSMakeRect( 0, 0, 64, 64 )
                                           device:nil];
        ASSERT_NE( view, nil );

        [view keyDown:make_key_event( NSEventTypeKeyDown, 42 /* magic */ )];

        aaa::input::EventBridge bridge;
        std::size_t const n = bridge.drain_from_view( view );
        EXPECT_GE( n, 1u );

        //	First event in queue should be KEY_DOWN with code 42.
        aaa::input::EngineEvent ev;
        ASSERT_TRUE( bridge.next_event( ev ) );
        EXPECT_EQ( ev.kind,     aaa::input::EngineEvent::Kind::KEY_DOWN );
        EXPECT_EQ( ev.key_code, 42 );

        [view release];
    }
}

TEST( EventBridge, MouseDownDrainsToMOUSE_DOWN )
{
    @autoreleasepool
    {
        AAASeedInputView* view = [[AAASeedInputView alloc]
                                    initWithFrame:NSMakeRect( 0, 0, 64, 64 )
                                           device:nil];
        ASSERT_NE( view, nil );

        NSPoint const at = NSMakePoint( 17.0, 23.0 );
        [view mouseDown:make_mouse_event( NSEventTypeLeftMouseDown, at )];

        aaa::input::EventBridge bridge;
        EXPECT_GE( bridge.drain_from_view( view ), 1u );

        //	Walk the queue, look for the MOUSE_DOWN. There will be a
        //	companion MOUSE_MOVE (lastMouseLocation is non-zero) ; both
        //	are valid drain output.
        bool saw_mouse_down = false;
        aaa::input::EngineEvent ev;
        while( bridge.next_event( ev ) )
        {
            if( ev.kind == aaa::input::EngineEvent::Kind::MOUSE_DOWN )
            {
                EXPECT_EQ( ev.button, 0 );        //	left = 0
                EXPECT_DOUBLE_EQ( ev.x, 17.0 );
                EXPECT_DOUBLE_EQ( ev.y, 23.0 );
                saw_mouse_down = true;
            }
        }
        EXPECT_TRUE( saw_mouse_down );

        [view release];
    }
}

TEST( EventBridge, MouseMovedDrainsToMOUSE_MOVE )
{
    @autoreleasepool
    {
        AAASeedInputView* view = [[AAASeedInputView alloc]
                                    initWithFrame:NSMakeRect( 0, 0, 64, 64 )
                                           device:nil];
        ASSERT_NE( view, nil );

        //	Use mouseDragged: instead of mouseMoved: — the view's
        //	mouseMoved: handler isn't implemented (NSWindow setup beyond
        //	MVP, per AAASeedInputView.mm:110-112), but mouseDragged:
        //	updates the same lastMouseLocation surface that our drain
        //	maps to MOUSE_MOVE. After draining, mouseButtonsPressed is
        //	empty (no companion mouseDown:) so the bridge emits exactly
        //	one MOUSE_MOVE.
        NSPoint const at = NSMakePoint( 99.5, 50.25 );
        [view mouseDragged:make_mouse_event( NSEventTypeLeftMouseDragged, at )];

        aaa::input::EventBridge bridge;
        EXPECT_EQ( bridge.drain_from_view( view ), 1u );

        aaa::input::EngineEvent ev;
        ASSERT_TRUE( bridge.next_event( ev ) );
        EXPECT_EQ( ev.kind, aaa::input::EngineEvent::Kind::MOUSE_MOVE );
        EXPECT_DOUBLE_EQ( ev.x, 99.5 );
        EXPECT_DOUBLE_EQ( ev.y, 50.25 );

        EXPECT_FALSE( bridge.next_event( ev ) );

        [view release];
    }
}
