// tests/unit/aaalua_misc_residual_stub.cpp
//
// c111 (2026-05-26) — clears the c_trax + c_file_virtual residual +
// misc buckets from the aaalua trio link cascade :
//   - c_trax::get_plug_in / get_plug_in_first              (2 syms, FINLINE bodies in trax.cpp)
//   - c_file_virtual::push_vfile / pop_vfile               (2 syms ; existing
//                                                            aaalua_file_virtual_stub.cpp omits them)
//   - aaa::file::ask_type_io_load                          (1 sym, seedfile.cpp)
//   - spy::sleep                                           (1 sym, spy.cpp ; Mac spy.h shim does not
//                                                            currently link the impl)
//   - sysutils::get_folder_path_known(o_str*, FOLDER_KNOWN) (1 sym, Win32-only impl)
//   - node_pref global                                     (1 sym, aaaseed.cpp's c_node_ui* singleton)
//
// All defs are weak so future real ports take over automatically
// (see [[feedback-weak-symbol-stubs]]).
//
// Scope : aaaseed_aaalua_trio_link_audit_tests target only.

#include "aaa_type.h"

//	c_trax (forward-declare to avoid pulling trax.h which would
//	transitively pull params + param_data + the full infra cone).
class c_param;
class c_trax;

//	The mangled name `c_trax::get_plug_in(int&)` needs to be emitted from
//	a method body. Since we can't define methods on a forward-declared
//	class, declare a minimal definition that the linker can name-match
//	via demangling. The cleanest approach : include trax.h, but pull
//	only what's needed.
#include "infrastructure/param/trax.h"

//	FINLINE expands to `inline` on Mac (aaa_type.h:308). Inline functions
//	only emit an out-of-line body when REFERENCED from within the TU --
//	since this stub TU has no call sites, the compiler skips emission and
//	the linker can't find the symbol. `__attribute__((used))` forces the
//	compiler to emit the symbol unconditionally, making it visible to
//	the linker. `__attribute__((weak))` keeps coexistence with any future
//	strong def from trax.cpp.
__attribute__((used, weak))
c_param* c_trax::get_plug_in( INT32& /*i*/ )
{
    //	Engine impl walks _plug_in_list ; on Mac stub returns nullptr so
    //	any caller-side null-check short-circuits the plug-in iteration.
    return nullptr;
}

__attribute__((used, weak))
c_param* c_trax::get_plug_in_first()
{
    return nullptr;
}

//	c_file_virtual extension : push_vfile / pop_vfile missing from
//	aaalua_file_virtual_stub.cpp (c66, partial coverage).
//	file_virtual.h has single-include `#error` guard and is already
//	pulled transitively via aaa_file.h above (through aaalua_*.cpp
//	transitive cone in this TU). Guard our include with #ifndef.
#ifndef AAA_FILE_VIRTUAL_H
#   include "file/file_virtual.h"
#endif

__attribute__((weak))
void c_file_virtual::push_vfile()
{
    //	Engine impl saves b_active / file_cur / dirname onto an internal
    //	stack ; Mac stub no-op since b_active is permanently false.
}

__attribute__((weak))
void c_file_virtual::pop_vfile()
{
    //	Mirror.
}

//	aaa::file::ask_type_io_load (seedfile.cpp line 536). Engine impl
//	opens a Win32 file picker ; Mac stub no-op.
#include "infrastructure/seedfile.h"

namespace aaa { namespace file {

__attribute__((weak))
void ask_type_io_load( TYPE_IO /*type_io*/, c_obj_ui* /*obj*/, C_PCHAR_C /*filename_prev*/ )
{
    //	No file dialog on Mac for audit target. A future runtime test
    //	can wire NSOpenPanel here.
}

}}

//	spy::sleep (spy.cpp). Engine impl wraps Win32 Sleep ; Mac stub
//	no-op since the audit target never executes spy paths.
#include "spy.h"

namespace spy {

__attribute__((weak))
void sleep( INT32 /*second_milli*/, C_PCHAR_C /*text*/ )
{
    //	Mac no-op : real impl would use usleep or std::this_thread::sleep_for.
}

}

//	sysutils::get_folder_path_known(o_str*, FOLDER_KNOWN) — Win32 calls
//	SHGetKnownFolderPath ; Mac stub returns false so any caller-side
//	build_editor_path fallback short-circuits to "no editor path".
//
//	Pull the c105/c106 umbrella shim header so the decl exactly matches
//	(FOLDER_KNOWN underlying type defaults differ between forward-decl
//	"enum class : int" and shim's body-decl "enum class { ... }"). The
//	shim is on this target's include path BEFORE the engine path.
#include "system/shared/SystemUtils.h"

__attribute__((weak))
bool sysutils::get_folder_path_known( o_str* /*dst*/, FOLDER_KNOWN /*folder_type*/ )
{
    return false;
}

//	node_pref global (aaaseed.cpp:1021). Declared `extern c_node_ui* node_pref;`
//	in aaaseed.h:56. aaalua_wrap.cpp::do_text references it ; Mac stub
//	nullptr lets any null-guard branch in do_text take the safe path.
class c_node_ui;
__attribute__((weak)) c_node_ui* node_pref = nullptr;
