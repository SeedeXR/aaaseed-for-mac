// src/ui/macos/aaa_event_adapter_mac.mm
//
// Phase 4 / v1 ship-gate adapter implementation. See header for the
// design narrative. This TU is .mm (not .cpp) so a future revision
// can grow an NSEvent direct-path entry point without renaming /
// re-wiring CMake -- ObjC++ flexibility is cheaper to preserve than
// to add back later. Today's body is pure C++ and would compile as
// .cpp too.
//
// Hermetic doctrine reminder : no o_str, no aaa_mem, no engine link.
// std::vector + std::uint64_t only.

#import "aaa_event_adapter_mac.h"

namespace aaa
{
namespace input
{

MouseEventOut mouse_event_to_engine( double x, double y, int button,
                                     int type_code, int modifiers,
                                     std::uint64_t timestamp_ns )
{
    MouseEventOut out;
    out.type         = type_code;
    out.button       = button;
    out.x            = x;
    out.y            = y;
    out.scroll_dx    = 0.0;
    out.scroll_dy    = 0.0;
    out.timestamp_ns = timestamp_ns;
    out.modifiers    = modifiers;
    return out;
}

KeyboardEventOut keyboard_event_to_engine( int key_code, bool down,
                                           int modifiers,
                                           std::uint64_t timestamp_ns )
{
    KeyboardEventOut out;
    out.type         = down ? 0 : 1;
    out.key_code     = key_code;
    out.modifiers    = modifiers;
    out.timestamp_ns = timestamp_ns;
    return out;
}

//	Internal helper : translate a single EngineEvent into a
//	MouseEventOut. Caller is responsible for filtering by Kind --
//	this helper assumes the EngineEvent is mouse-class.
static MouseEventOut engine_event_to_mouse_out( EngineEvent const& ev )
{
    MouseEventOut out;
    out.timestamp_ns = 0;       //	c119-B EngineEvent has no timestamp field yet
    out.modifiers    = 0;       //	c119-B EngineEvent has no modifier field yet

    switch( ev.kind )
    {
        case EngineEvent::Kind::MOUSE_MOVE :
            out.type   = 0;
            out.button = 0;
            out.x      = ev.x;
            out.y      = ev.y;
            break;
        case EngineEvent::Kind::MOUSE_DOWN :
            out.type   = 1;
            out.button = ev.button;
            out.x      = ev.x;
            out.y      = ev.y;
            break;
        case EngineEvent::Kind::MOUSE_UP :
            out.type   = 2;
            out.button = ev.button;
            out.x      = ev.x;
            out.y      = ev.y;
            break;
        case EngineEvent::Kind::SCROLL :
            out.type      = 4;
            out.button    = 0;
            out.scroll_dx = ev.dx;
            out.scroll_dy = ev.dy;
            break;
        default :
            //	Not a mouse-class EngineEvent ; caller should have
            //	filtered. Return a benign move-at-origin so the
            //	caller still gets a defined POD shape.
            out.type = 0;
            break;
    }
    return out;
}

//	Internal helper : translate a single EngineEvent into a
//	KeyboardEventOut. Caller is responsible for filtering by Kind.
static KeyboardEventOut engine_event_to_keyboard_out( EngineEvent const& ev )
{
    KeyboardEventOut out;
    out.timestamp_ns = 0;
    out.modifiers    = 0;
    out.key_code     = ev.key_code;
    switch( ev.kind )
    {
        case EngineEvent::Kind::KEY_DOWN : out.type = 0; break;
        case EngineEvent::Kind::KEY_UP   : out.type = 1; break;
        default :
            //	Not a keyboard-class EngineEvent ; default to key_down.
            out.type = 0;
            break;
    }
    return out;
}

std::vector< MouseEventOut > drain_to_engine_mouse( EventBridge& bridge )
{
    std::vector< MouseEventOut > out;
    EngineEvent ev;
    while( bridge.next_event( ev ) )
    {
        switch( ev.kind )
        {
            case EngineEvent::Kind::MOUSE_MOVE :
            case EngineEvent::Kind::MOUSE_DOWN :
            case EngineEvent::Kind::MOUSE_UP   :
            case EngineEvent::Kind::SCROLL     :
                out.push_back( engine_event_to_mouse_out( ev ) );
                break;
            default :
                //	keyboard-class : silently dropped per header
                //	contract (consumer is expected to also call
                //	drain_to_engine_keyboard back-to-back).
                break;
        }
    }
    return out;
}

std::vector< KeyboardEventOut > drain_to_engine_keyboard( EventBridge& bridge )
{
    std::vector< KeyboardEventOut > out;
    EngineEvent ev;
    while( bridge.next_event( ev ) )
    {
        switch( ev.kind )
        {
            case EngineEvent::Kind::KEY_DOWN :
            case EngineEvent::Kind::KEY_UP   :
                out.push_back( engine_event_to_keyboard_out( ev ) );
                break;
            default :
                //	mouse-class : silently dropped.
                break;
        }
    }
    return out;
}

}   //	namespace input
}   //	namespace aaa
