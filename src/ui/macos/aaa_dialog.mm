// src/ui/macos/aaa_dialog.mm
//
// c136 / Phase 4 (todo.md L301) : NSAlert + NSPanel impl behind the
// `aaa::ui::dialog::*` API declared in `aaa_dialog.h`.
//
// Hermetic Mac sub-lib (c104 / feedback_hermetic_mac_sublibs.md) :
// Foundation + AppKit only. No `o_str`, no `aaa_mem`, no vendor link.
// ASCII-only source. Manual reference counting (no ARC) -- the
// `aaaseed_ui_dialog_mac` target sets `-fno-objc-arc` to match the
// rest of `src/ui/macos/`.
//
// Bridge doctrine (c134-A / feedback_bridge_api_standardization.md) :
// parent_window is `void*` in the header ; we cast back to NSWindow*
// via `(__bridge NSWindow*)` at the .mm boundary. Inside `namespace
// aaa::ui::dialog` the `@class NSWindow` ifdef would mangling-diverge
// across .cpp/.mm consumers ; `void*` is ABI-identical.
//
// Headless / unit-test guard : NSAlert's runModal blocks indefinitely
// when there is no running NSApplication event loop. We probe
// `[NSApp isRunning]` (NSApp == nil on processes that never called
// NSApplicationLoad / -sharedApplication) before invoking runModal ;
// if no live loop, return Cancel / nullopt without blocking. This
// preserves the c131-B no-runModal-under-ctest pattern from
// aaa_file_dialog (which used the heavier-handed "panel object exists
// but we don't call runModal" approach -- we are more explicit here
// because NSAlert auto-creates `NSApp` as a side effect of init, so
// the isRunning probe is the cleanest gate).

#include "src/ui/macos/aaa_dialog.h"

#if defined(__APPLE__)

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

namespace aaa
{
namespace ui
{
namespace dialog
{

namespace
{

//	Map our public severity enum to NSAlertStyle. Default branch
//	returns Informational for forward-compatibility (any new enum
//	value added to AlertStyle silently degrades to Informational
//	rather than triggering UB on switch fall-through).
NSAlertStyle ns_style_for( AlertStyle s )
{
    switch( s )
    {
        case AlertStyle::Warning:       return NSAlertStyleWarning;
        case AlertStyle::Critical:      return NSAlertStyleCritical;
        case AlertStyle::Informational: return NSAlertStyleInformational;
    }
    return NSAlertStyleInformational;
}

//	Convert std::string -> NSString safely. Returns @"" for empty
//	input (NSAlert tolerates @"" for messageText / informativeText).
//	NSString returns nil on a malformed UTF-8 input -- we fall back
//	to @"" in that case so callers never see a nil substitution.
NSString* ns_from_std( std::string const& s )
{
    if( s.empty() )
        return @"";
    NSString* n = [NSString stringWithUTF8String:s.c_str()];
    if( n == nil )
        return @"";
    return n;
}

//	True when NSApplication has been activated AND its event loop is
//	running. Both halves matter : a process that has called
//	[NSApplication sharedApplication] but never -run has a non-nil
//	NSApp but NSApp.isRunning == NO. NSAlert.runModal in that state
//	will spin its own modal event loop and block forever under ctest.
//
//	Some test processes also synthesize NSEvents without a loop ; we
//	stay defensive and return false unless NSApp is BOTH non-nil and
//	-isRunning. Production code paths (the .app bundle) always have
//	a live NSApp + loop, so the safety guard never fires there.
bool has_live_event_loop()
{
    if( NSApp == nil )
        return false;
    return [NSApp isRunning] == YES;
}

//	Build a configured NSAlert from common parameters. Caller retains
//	the returned alert and is responsible for releasing it. Returns
//	nil only on allocation failure (effectively impossible in normal
//	conditions ; we still null-check to avoid a downstream crash).
NSAlert* make_alert(
        std::string const&      title,
        std::string const&      message,
        AlertStyle              style )
{
    NSAlert* a = [[NSAlert alloc] init];
    if( a == nil )
        return nil;
    [a setMessageText:ns_from_std( title )];
    [a setInformativeText:ns_from_std( message )];
    [a setAlertStyle:ns_style_for( style )];
    return a;
}

//	Map NSAlert's modal response back to our coarse AlertResult.
//	NSAlertFirstButtonReturn is the rightmost button (default action)
//	which we treat as OK. NSAlertSecondButtonReturn is the left/Cancel
//	slot. Anything else -> Other.
AlertResult result_from_response( NSModalResponse rc )
{
    if( rc == NSAlertFirstButtonReturn )
        return AlertResult::OK;
    if( rc == NSAlertSecondButtonReturn )
        return AlertResult::Cancel;
    return AlertResult::Other;
}

}   //	anonymous namespace

//	------------------------------------------------------------------
//	Modal public API
//	------------------------------------------------------------------

AlertResult alert(
        std::string const&      title,
        std::string const&      message,
        AlertStyle              style,
        void*                   parent_window )
{
    (void)parent_window;    //	Reserved for sheet-mode in a future
                            //	revision ; currently always app-modal.
    @autoreleasepool
    {
        if( !has_live_event_loop() )
            return AlertResult::Cancel;

        NSAlert* a = make_alert( title, message, style );
        if( a == nil )
            return AlertResult::Cancel;
        //	One OK button. NSAlert defaults to a single OK button if
        //	none are added, but we add it explicitly for symmetry
        //	with confirm() below.
        [a addButtonWithTitle:@"OK"];

        NSModalResponse rc = [a runModal];
        [a release];
        return result_from_response( rc );
    }
}

AlertResult confirm(
        std::string const&      title,
        std::string const&      message,
        std::string const&      ok_label,
        std::string const&      cancel_label,
        void*                   parent_window )
{
    (void)parent_window;
    @autoreleasepool
    {
        if( !has_live_event_loop() )
            return AlertResult::Cancel;

        NSAlert* a = make_alert( title, message, AlertStyle::Informational );
        if( a == nil )
            return AlertResult::Cancel;
        //	First-added button is the default / rightmost / OK slot.
        [a addButtonWithTitle:ns_from_std( ok_label )];
        [a addButtonWithTitle:ns_from_std( cancel_label )];

        NSModalResponse rc = [a runModal];
        [a release];
        return result_from_response( rc );
    }
}

std::optional< std::string > prompt(
        std::string const&      title,
        std::string const&      message,
        std::string const&      default_value,
        std::string const&      ok_label,
        std::string const&      cancel_label,
        void*                   parent_window )
{
    (void)parent_window;
    @autoreleasepool
    {
        if( !has_live_event_loop() )
            return std::nullopt;

        NSAlert* a = make_alert( title, message, AlertStyle::Informational );
        if( a == nil )
            return std::nullopt;
        [a addButtonWithTitle:ns_from_std( ok_label )];
        [a addButtonWithTitle:ns_from_std( cancel_label )];

        //	Single-line NSTextField as accessory view. 24pt height is
        //	the AppKit standard single-line input height ; 240pt width
        //	matches NSAlert's natural body width.
        NSRect frame = NSMakeRect( 0.0, 0.0, 240.0, 24.0 );
        NSTextField* field = [[NSTextField alloc] initWithFrame:frame];
        if( field == nil )
        {
            [a release];
            return std::nullopt;
        }
        [field setStringValue:ns_from_std( default_value )];
        [a setAccessoryView:field];

        NSModalResponse rc = [a runModal];
        std::optional< std::string > out;
        if( rc == NSAlertFirstButtonReturn )
        {
            NSString* v = [field stringValue];
            if( v != nil )
            {
                char const* c = [v UTF8String];
                if( c != nullptr )
                    out = std::string { c };
                else
                    out = std::string {};
            }
            else
            {
                out = std::string {};
            }
        }
        [field release];
        [a release];
        return out;
    }
}

//	------------------------------------------------------------------
//	Modeless NSPanel public API
//	------------------------------------------------------------------

PanelHandle make_modeless_panel(
        std::string const&      title,
        int                     width_points,
        int                     height_points )
{
    //	Reject degenerate sizes. AppKit accepts a NSZeroRect panel
    //	but a zero-sized window has no client area and is a UI bug
    //	from the caller's perspective. Signal it via { nullptr }.
    if( width_points <= 0 || height_points <= 0 )
        return PanelHandle{ nullptr };

    NSRect frame = NSMakeRect(
            0.0,
            0.0,
            static_cast< CGFloat >( width_points ),
            static_cast< CGFloat >( height_points ) );

    //	NSWindowStyleMaskUtilityWindow gives the small-titlebar
    //	"palette" look standard for HUDs and tool panels. Titled +
    //	Closable so the user can dismiss without touching the .app
    //	menu. Resizable so window contents can grow.
    NSUInteger style_mask =
            NSWindowStyleMaskTitled
          | NSWindowStyleMaskClosable
          | NSWindowStyleMaskResizable
          | NSWindowStyleMaskUtilityWindow;

    NSPanel* panel = [[NSPanel alloc]
            initWithContentRect:frame
                      styleMask:style_mask
                        backing:NSBackingStoreBuffered
                          defer:YES];
    if( panel == nil )
        return PanelHandle{ nullptr };

    //	Map UTF-8 title. Empty title -> empty NSString (panel shows
    //	a bare titlebar -- valid AppKit state).
    NSString* t = nil;
    if( !title.empty() )
        t = [NSString stringWithUTF8String:title.c_str()];
    if( t == nil )
        t = @"";
    [panel setTitle:t];

    //	Modeless panel + auto-release-on-close OFF : we own the
    //	retain explicitly so destroy_panel can release deterministic-
    //	ally. Without this, AppKit would over-release the panel when
    //	the user clicks the close button + we'd double-free in dtor.
    [panel setReleasedWhenClosed:NO];

    return PanelHandle{ static_cast< void* >( panel ) };
}

void show_panel( PanelHandle h )
{
    if( h.opaque == nullptr )
        return;
    NSPanel* panel = static_cast< NSPanel* >( h.opaque );
    //	orderFrontRegardless lets the panel appear even when this
    //	process is not the active app (HUD use case). For a normal
    //	in-app panel `makeKeyAndOrderFront:` would be more typical
    //	but requires a key window context that unit tests do not
    //	have. orderFrontRegardless is the safe headless-friendly
    //	choice.
    [panel orderFrontRegardless];
}

void hide_panel( PanelHandle h )
{
    if( h.opaque == nullptr )
        return;
    NSPanel* panel = static_cast< NSPanel* >( h.opaque );
    [panel orderOut:nil];
}

void destroy_panel( PanelHandle h )
{
    if( h.opaque == nullptr )
        return;
    NSPanel* panel = static_cast< NSPanel* >( h.opaque );
    //	Ensure the panel is off-screen before release. orderOut is
    //	idempotent so calling on an already-hidden panel is fine.
    [panel orderOut:nil];
    [panel release];
}

}   //	namespace dialog
}   //	namespace ui
}   //	namespace aaa

#endif  //	defined(__APPLE__)
