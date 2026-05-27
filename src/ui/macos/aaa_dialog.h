// src/ui/macos/aaa_dialog.h
//
// c136 / Phase 4 (todo.md L301) : NSAlert + NSPanel replacement for the
// Win32 modal/modeless dialogs in `vendor/aaaseed-engine/Src/ui/dialog_*.h`.
// Those vendor entry points are Win32-only (DialogBoxParam / MessageBox /
// CreateDialog) and unreachable from Mac. This header exposes a Mac-first,
// pure-C++ API (no Objective-C in the header) so C++-only TUs may include
// it freely ; the impl lives in `aaa_dialog.mm`.
//
// API surface, mirroring the most common upstream dialog patterns :
//   - alert()        : single-button informational/warning/critical popup
//                      (NSAlert with one OK button).
//   - confirm()      : two-button OK/Cancel popup (NSAlert with two buttons).
//   - prompt()       : single-line text input popup (NSAlert + NSTextField
//                      accessoryView).
//   - make/show/hide/destroy_panel : opaque NSPanel-backed modeless panel
//                      handle for custom HUDs and tool-palette use cases.
//
// Hermetic Mac sub-lib doctrine (c104 / feedback_hermetic_mac_sublibs.md) :
//   - Pure CPU. No Metal touch.
//   - std::string / std::optional only. No `o_str`, no `aaa_mem.h`,
//     no `aaa_str.h`, no `aaa_type.h`.
//   - No vendor edits. No link to `aaaseed_code_utils`.
//   - Foundation + AppKit only (impl in aaa_dialog.mm).
//
// Bridge-API doctrine (c134-A refinement / feedback_bridge_api_standardization.md) :
//   parent_window is typed `void*` rather than `NSWindow*`. The `@class
//   NSWindow` ifdef pattern is illegal inside a C++ namespace and diverges
//   name mangling at link time across .cpp / .mm consumers. `void*` is
//   ABI-identical from both sides ; Objective-C++ callers may pass an
//   NSWindow* via implicit pointer conversion ; the .mm impl casts back
//   with `(__bridge NSWindow*)`.
//
// Wiring lives in `src/ui/macos/CMakeLists.txt` as a new static lib
// `aaaseed_ui_dialog_mac`. Tests in `tests/unit/ui_dialog_test.cpp`.

#pragma once

#include <optional>
#include <string>

namespace aaa
{
namespace ui
{
namespace dialog
{

//	Visual severity for alert() / confirm() / prompt(). Maps to
//	NSAlertStyleInformational / Warning / Critical at the .mm layer.
//	Informational is the safe default.
enum class AlertStyle
{
    Informational,
    Warning,
    Critical,
};

//	Coarse user-action outcome for the modal entry points. NSAlert's
//	first button (rightmost on macOS) returns OK ; second button returns
//	Cancel ; any further button returns Other. Headless / no-event-loop
//	contexts (unit-test process) return Cancel.
enum class AlertResult
{
    OK,
    Cancel,
    Other,
};

//	Single-button informational/warning/critical alert. Returns OK on
//	dismissal. Under a unit-test process (no NSApplication / no event
//	loop), returns Cancel without blocking.
//
//	`title`         -- alert window title ; empty = default.
//	`message`       -- informative body text.
//	`style`         -- visual severity.
//	`parent_window` -- optional NSWindow* (passed as void* per c134-A
//	                   bridge doctrine) ; nullptr = app-modal.
AlertResult alert(
        std::string const&      title,
        std::string const&      message,
        AlertStyle              style = AlertStyle::Informational,
        void*                   parent_window = nullptr );

//	Two-button confirm dialog. Returns OK or Cancel depending on which
//	button the user clicked. Custom button labels supported. Under a
//	unit-test process (no NSApplication / no event loop), returns
//	Cancel without blocking.
AlertResult confirm(
        std::string const&      title,
        std::string const&      message,
        std::string const&      ok_label = "OK",
        std::string const&      cancel_label = "Cancel",
        void*                   parent_window = nullptr );

//	Single-line text input prompt. Returns the typed string on OK,
//	nullopt on Cancel. Under a unit-test process (no NSApplication /
//	no event loop), returns nullopt without blocking.
std::optional< std::string > prompt(
        std::string const&      title,
        std::string const&      message,
        std::string const&      default_value = "",
        std::string const&      ok_label = "OK",
        std::string const&      cancel_label = "Cancel",
        void*                   parent_window = nullptr );

//	Opaque handle to a modeless NSPanel. `opaque` is an NSPanel* the
//	.mm side retained on creation. Pass-by-value POD ; nullptr opaque
//	denotes a degenerate / failed-creation handle (all ops on it are
//	no-ops).
struct PanelHandle
{
    void*   opaque;     //	NSPanel* retained by make_modeless_panel
};

//	Create a modeless NSPanel sized in AppKit points. The panel is
//	created but NOT shown -- call show_panel() to orderFront it. The
//	title appears in the panel's title bar.
//
//	`width_points` / `height_points` <= 0 returns { nullptr } so the
//	caller can detect the degenerate case without crashing.
PanelHandle make_modeless_panel(
        std::string const&      title,
        int                     width_points,
        int                     height_points );

//	orderFront the panel (visible + on-screen). Under a unit-test
//	process the orderFront call still works -- AppKit allows creating
//	+ ordering panels without a running NSApp -- but no event loop
//	will drain. No-op when `h.opaque == nullptr`.
void show_panel( PanelHandle h );

//	orderOut the panel (hidden + off-screen). The handle remains
//	valid ; show_panel() can re-show. No-op when `h.opaque == nullptr`.
void hide_panel( PanelHandle h );

//	Release the panel's retain count taken at make_modeless_panel
//	time. The handle becomes invalid -- callers must drop their copy.
//	Safe to call on a degenerate { nullptr } handle.
void destroy_panel( PanelHandle h );

}   //	namespace dialog
}   //	namespace ui
}   //	namespace aaa
