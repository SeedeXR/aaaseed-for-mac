// src/ui/macos/aaa_file_dialog.h
//
// Phase 4 unblock : NSOpenPanel / NSSavePanel replacement for the
// Win32 file dialogs in `vendor/aaaseed-engine/Src/ui/file_dlg.h`
// (`aaa::file::get_filename_open/save/get_folder`). Those vendor
// entry points are Win32-only (GetOpenFileName / GetSaveFileName /
// SHBrowseForFolder / IFileDialog) and unreachable from Mac.
//
// This header exposes a Mac-first, pure-C++ API that callers can
// adopt without touching the vendor signature. A future cross-platform
// shim layer can route `aaa::file::get_filename_*` to these
// `aaa::ui::*_file_dialog` functions on `__APPLE__`. The header is
// pure standard C++ (no Objective-C types) so C++-only TUs may
// include it freely ; the impl lives in `aaa_file_dialog.mm`.
//
// Hermetic Mac sub-lib doctrine (c104 / feedback_hermetic_mac_sublibs.md) :
//   - Pure CPU. No Metal touch.
//   - std::string / std::vector / std::optional only. No `o_str`,
//     no `aaa_mem.h`, no `aaa_str.h`, no `aaa_type.h`.
//   - No vendor edits. No link to `aaaseed_code_utils`.
//   - Foundation + AppKit only (UniformTypeIdentifiers gated behind
//     `@available(macOS 11.0, *)` at the .mm level).
//
// Wiring lives in `src/ui/macos/CMakeLists.txt` as a new static lib
// `aaaseed_ui_file_dialog_mac`. Test target `aaaseed_ui_file_dialog_tests`
// lives in `tests/unit/ui_file_dialog_test.cpp`.

#pragma once

#include <optional>
#include <string>
#include <vector>

namespace aaa
{
namespace ui
{

//	One file-type filter entry. `label` is a human-readable description
//	("Lua scripts", "PNG images", ...). `extensions` is the bare extension
//	list ("lua", "png") -- the implementation normalises leading dots so
//	callers may pass either ".lua" or "lua" interchangeably.
struct FileDialogFilter
{
    std::string                 label;
    std::vector< std::string >  extensions;
};

//	Normalise one extension string to the bare lower-case form expected
//	by NSOpenPanel.allowedFileTypes / UTType lookups. Strips a leading
//	dot if present and lower-cases the result. Exposed for tests + for
//	future callers that want to canonicalise before stuffing into a
//	FileDialogFilter. Pure CPU, no Foundation dependency.
std::string normalise_extension( std::string const& ext );

//	Show a "choose one existing file" dialog modally. Returns the chosen
//	POSIX path on success, std::nullopt on cancel / no-selection / when
//	the call is made in a context where the panel cannot run (e.g. unit
//	test process with no event loop -- the panel's runModal returns
//	NSModalResponseCancel).
//
//	`title`     -- window title / message ; empty string = default.
//	`filters`   -- allowed file kinds. Empty list = "all files".
//	`start_dir` -- initial directory ; empty = system default.
std::optional< std::string > open_file_dialog(
        std::string const&                          title,
        std::vector< FileDialogFilter > const&      filters,
        std::string const&                          start_dir = "" );

//	Show a "save as" dialog modally. Returns the chosen POSIX path on
//	success, std::nullopt on cancel. `default_filename` populates the
//	"Save As:" text field. Empty `filters` -> "all files".
std::optional< std::string > save_file_dialog(
        std::string const&                          title,
        std::string const&                          default_filename,
        std::vector< FileDialogFilter > const&      filters );

//	Show a "choose one or more files" dialog modally. Returns the chosen
//	POSIX paths in selection order ; empty vector on cancel / no
//	selection. Other semantics identical to open_file_dialog.
std::vector< std::string > open_multi_file_dialog(
        std::string const&                          title,
        std::vector< FileDialogFilter > const&      filters );

}   //	namespace ui
}   //	namespace aaa
