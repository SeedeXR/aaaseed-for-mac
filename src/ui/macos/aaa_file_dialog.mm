// src/ui/macos/aaa_file_dialog.mm
//
// Phase 4 unblock : NSOpenPanel / NSSavePanel impl behind the
// `aaa::ui::*_file_dialog` API declared in `aaa_file_dialog.h`.
//
// Hermetic Mac sub-lib (c104 / feedback_hermetic_mac_sublibs.md) :
// Foundation + AppKit only ; UniformTypeIdentifiers used when
// available (macOS 11+) with a graceful legacy fallback to the
// deprecated `allowedFileTypes` API for older targets. No `o_str`,
// no `aaa_mem`, no vendor link. ASCII-only source.
//
// Gating : the whole TU is `#if defined(__APPLE__)` so a future
// cross-platform .mm/.cpp split can drop a stub or call into a
// shared header without breaking Windows builds.

#include "src/ui/macos/aaa_file_dialog.h"

#if defined(__APPLE__)

#import <Foundation/Foundation.h>
#import <AppKit/AppKit.h>

#include <algorithm>
#include <cctype>

//	UTType is iOS 14 / macOS 11+. The header itself is safe to import
//	on older toolchains but `UTType` symbols are only usable behind
//	`@available`. Test the deployment target at compile time too --
//	the project pins macOS 13 (Ventura) so the header is always
//	available, but we keep the guard for future portability.
#if __has_include(<UniformTypeIdentifiers/UniformTypeIdentifiers.h>)
#   import <UniformTypeIdentifiers/UniformTypeIdentifiers.h>
#   define AAA_HAS_UTTYPE 1
#else
#   define AAA_HAS_UTTYPE 0
#endif

namespace aaa
{
namespace ui
{

//	------------------------------------------------------------------
//	Pure-CPU helpers
//	------------------------------------------------------------------

std::string normalise_extension( std::string const& ext )
{
    std::string out = ext;
    //	Strip a single leading dot if present (".lua" -> "lua").
    if( !out.empty() && out.front() == '.' )
        out.erase( out.begin() );
    //	Lower-case. UTType lookups are case-insensitive but the
    //	NSOpenPanel.allowedFileTypes legacy API matches by exact
    //	string ; normalising to lower-case is the safe choice and
    //	matches what NSURL.pathExtension returns on a "foo.LUA"
    //	input on case-insensitive HFS+/APFS (lower-cased).
    std::transform( out.begin(), out.end(), out.begin(),
            []( unsigned char c ){ return static_cast< char >( std::tolower( c ) ); } );
    return out;
}

namespace
{

//	Build an NSArray<NSString*> of normalised extensions from the
//	caller's filter list, for the legacy allowedFileTypes path.
//	Empty filter list -> nil (NSOpenPanel interprets nil as "all
//	files allowed").
NSArray< NSString* >* build_extension_array( std::vector< FileDialogFilter > const& filters )
{
    if( filters.empty() )
        return nil;
    NSMutableArray< NSString* >* out = [NSMutableArray array];
    for( auto const& f : filters )
    {
        for( auto const& e : f.extensions )
        {
            std::string norm = normalise_extension( e );
            if( norm.empty() )
                continue;
            NSString* s = [NSString stringWithUTF8String:norm.c_str()];
            if( s != nil )
                [out addObject:s];
        }
    }
    if( [out count] == 0 )
        return nil;
    return out;
}

#if AAA_HAS_UTTYPE
//	Build an NSArray<UTType*> from the caller's filter list. Each
//	extension is resolved via [UTType typeWithFilenameExtension:].
//	Unknown extensions are skipped silently (UTType returns nil for
//	a totally novel extension ; the legacy path covers that case).
//	Available on macOS 11+ ; caller must @available-gate before use.
API_AVAILABLE(macos(11.0))
NSArray< UTType* >* build_uttype_array( std::vector< FileDialogFilter > const& filters )
{
    if( filters.empty() )
        return nil;
    NSMutableArray< UTType* >* out = [NSMutableArray array];
    for( auto const& f : filters )
    {
        for( auto const& e : f.extensions )
        {
            std::string norm = normalise_extension( e );
            if( norm.empty() )
                continue;
            NSString* s = [NSString stringWithUTF8String:norm.c_str()];
            if( s == nil )
                continue;
            UTType* t = [UTType typeWithFilenameExtension:s];
            if( t != nil )
                [out addObject:t];
        }
    }
    if( [out count] == 0 )
        return nil;
    return out;
}
#endif  //	AAA_HAS_UTTYPE

//	Apply filters + title + start dir to a freshly created panel.
//	Common helper for open / save / multi panels.
void configure_panel(
        NSSavePanel*                                panel,
        std::string const&                          title,
        std::vector< FileDialogFilter > const&      filters,
        std::string const&                          start_dir )
{
    if( !title.empty() )
    {
        NSString* s = [NSString stringWithUTF8String:title.c_str()];
        if( s != nil )
            [panel setMessage:s];
    }

    if( !start_dir.empty() )
    {
        NSString* s = [NSString stringWithUTF8String:start_dir.c_str()];
        if( s != nil )
        {
            NSURL* u = [NSURL fileURLWithPath:s isDirectory:YES];
            if( u != nil )
                [panel setDirectoryURL:u];
        }
    }

#if AAA_HAS_UTTYPE
    if( @available( macOS 11.0, * ) )
    {
        NSArray< UTType* >* types = build_uttype_array( filters );
        if( types != nil )
            [panel setAllowedContentTypes:types];
        return;
    }
#endif
    //	Legacy fallback. -allowedFileTypes is deprecated on macOS 11+
    //	but still works ; we only land here when UTType is genuinely
    //	unavailable at runtime (won't happen on macOS 13+ deploy
    //	target, kept for forward-portability).
    NSArray< NSString* >* exts = build_extension_array( filters );
    if( exts != nil )
    {
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdeprecated-declarations"
        [panel setAllowedFileTypes:exts];
#pragma clang diagnostic pop
    }
}

//	Convert an NSURL chosen by the panel into a UTF-8 std::string
//	POSIX path. Returns empty string if url is nil or has no path.
std::string url_to_posix_string( NSURL* url )
{
    if( url == nil )
        return std::string {};
    NSString* p = [url path];
    if( p == nil )
        return std::string {};
    char const* c = [p UTF8String];
    if( c == nullptr )
        return std::string {};
    return std::string { c };
}

}   //	anonymous namespace

//	------------------------------------------------------------------
//	Public API
//	------------------------------------------------------------------

//	c149-A follow-up : `runModal` blocks forever waiting for user
//	interaction when no NSApp event loop is running (unit-test context).
//	Before c149-A added the v3 open_file_dialog test, this surface was
//	only invoked from the running .app where NSApp.isRunning is YES.
//	Match c135-B's NSPanel adapter pattern : guard the runModal calls
//	with isRunning so unit tests return nullopt instead of hanging.
//	Honors the spec note "Under unit tests (no NSApp event loop),
//	returns nullopt per c131-B's guard" — the guard now actually exists.
static bool aaa_no_event_loop_running()
{
    return [NSApp isRunning] == NO;
}

std::optional< std::string > open_file_dialog(
        std::string const&                          title,
        std::vector< FileDialogFilter > const&      filters,
        std::string const&                          start_dir )
{
    @autoreleasepool
    {
        if( aaa_no_event_loop_running() )
            return std::nullopt;

        NSOpenPanel* panel = [NSOpenPanel openPanel];
        if( panel == nil )
            return std::nullopt;

        [panel setCanChooseFiles:YES];
        [panel setCanChooseDirectories:NO];
        [panel setAllowsMultipleSelection:NO];
        [panel setResolvesAliases:YES];
        configure_panel( panel, title, filters, start_dir );

        NSModalResponse rc = [panel runModal];
        if( rc != NSModalResponseOK )
            return std::nullopt;

        NSURL* url = [panel URL];
        std::string p = url_to_posix_string( url );
        if( p.empty() )
            return std::nullopt;
        return p;
    }
}

std::optional< std::string > save_file_dialog(
        std::string const&                          title,
        std::string const&                          default_filename,
        std::vector< FileDialogFilter > const&      filters )
{
    @autoreleasepool
    {
        if( aaa_no_event_loop_running() )
            return std::nullopt;

        NSSavePanel* panel = [NSSavePanel savePanel];
        if( panel == nil )
            return std::nullopt;

        configure_panel( panel, title, filters, /*start_dir*/ std::string {} );

        if( !default_filename.empty() )
        {
            NSString* s = [NSString stringWithUTF8String:default_filename.c_str()];
            if( s != nil )
                [panel setNameFieldStringValue:s];
        }

        NSModalResponse rc = [panel runModal];
        if( rc != NSModalResponseOK )
            return std::nullopt;

        NSURL* url = [panel URL];
        std::string p = url_to_posix_string( url );
        if( p.empty() )
            return std::nullopt;
        return p;
    }
}

std::vector< std::string > open_multi_file_dialog(
        std::string const&                          title,
        std::vector< FileDialogFilter > const&      filters )
{
    @autoreleasepool
    {
        if( aaa_no_event_loop_running() )
            return std::vector< std::string > {};

        NSOpenPanel* panel = [NSOpenPanel openPanel];
        if( panel == nil )
            return std::vector< std::string > {};

        [panel setCanChooseFiles:YES];
        [panel setCanChooseDirectories:NO];
        [panel setAllowsMultipleSelection:YES];
        [panel setResolvesAliases:YES];
        configure_panel( panel, title, filters, /*start_dir*/ std::string {} );

        NSModalResponse rc = [panel runModal];
        if( rc != NSModalResponseOK )
            return std::vector< std::string > {};

        NSArray< NSURL* >* urls = [panel URLs];
        std::vector< std::string > out;
        if( urls != nil )
        {
            out.reserve( [urls count] );
            for( NSURL* u in urls )
            {
                std::string p = url_to_posix_string( u );
                if( !p.empty() )
                    out.push_back( std::move( p ) );
            }
        }
        return out;
    }
}

}   //	namespace ui
}   //	namespace aaa

#endif  //	defined(__APPLE__)
