// src/clipboard/clipboard_mac.h
//
// second_todo.md S6 (continuation 153) : native macOS clipboard copy/paste.
//
// Background : the wave-1 Mac port only handled drag-drop file URLs
// (src/ui/macos/AAASeedInputView.mm). The upstream engine author's mac-port
// branch added a UTF-8 string clipboard bridge (Src/infrastructure/
// clipboard_mac.mm, 37 LOC) exposing the C-ABI `mac_clipboard_copy` /
// `mac_clipboard_paste`. This sub-lib brings that capability across natively
// and adds a C++-friendly std::string surface on top.
//
// The C-ABI signatures below are byte-compatible with the upstream branch so
// the shared engine call site (e.g. a GaBu text-field paste) links the same
// symbol on both platforms -- cross-platform reciprocity, no #ifdef at the
// call site.
//
// Hermetic Mac sub-lib doctrine (feedback_hermetic_mac_sublibs.md) :
//   - Pure ObjC++. std:: + Cocoa (AppKit NSPasteboard) only. No engine link,
//     no o_str, no aaa_mem cascade. No vendor edits.
//   - Compiled with -fno-objc-arc (matches the other Mac sub-libs).
//
// Tests : tests/native/clipboard_mac_test.mm (round-trip a UTF-8 string,
// incl. multibyte ; truncation guard ; empty-string handling).

#pragma once

#include <string>

//	-----------------------------------------------------------------------
//	C-ABI : byte-compatible with the upstream mac-port branch. The shared
//	engine source references these symbols directly. `len` is the byte
//	length of `str` (not including any terminator). Returns true on success.
//	-----------------------------------------------------------------------
extern "C" bool mac_clipboard_copy( char const * str, int len );

//	Copy the general-pasteboard string into `buf` (UTF-8, NUL-terminated,
//	truncated to fit `buf_size`). `*out_len` receives the written byte count
//	(excluding the terminator). Returns false if the pasteboard holds no
//	string or on bad arguments.
extern "C" bool mac_clipboard_paste( char * buf, int buf_size, int * out_len );

namespace aaa
{
namespace clipboard
{

//	Copy a UTF-8 std::string to the general pasteboard. Returns true on
//	success. An empty string clears the pasteboard string and returns true.
bool copy( std::string const & text );

//	Return the general-pasteboard string as UTF-8. Returns "" if the
//	pasteboard holds no string type.
std::string paste();

}   //	namespace clipboard
}   //	namespace aaa
