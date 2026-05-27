// tests/unit/aaalua_master_shim/system/shared/SystemUtils.h
//
// Tests-private SHIM for vendor/aaaseed-engine/Src/system/shared/SystemUtils.h,
// used ONLY by the aaalua_master.cpp compile-only test target. Added to
// the target's include path BEFORE the engine include path so this file
// wins for the test build only ; the engine's real SystemUtils.h still
// resolves everywhere else.
//
// Why a shim : the real header has do_file_dialog_load / _save / _folder
// declarations at lines 181/184/187 referencing `LPWSTR` and
// `COMDLG_FILTERSPEC` UNCONDITIONALLY (the matching guards at lines
// 162-169 cover only the helper, not the dialog fns). On Windows the
// `#include <Shtypes.h>` earlier in the file pulls those typedefs in ;
// on Mac there is no analog, so the unconditional decls hit "unknown
// type name LPWSTR/COMDLG_FILTERSPEC".
//
// aaalua_master.cpp itself uses `sysutils::FOLDER_KNOWN` enum (3 values)
// + `sysutils::get_folder_path_known(o_str*, FOLDER_KNOWN)`. Both are
// Windows-only at the implementation level. We declare them here so
// the TU compiles ; the bodies are anonymous-namespace static functions
// in aaalua_master.cpp's `build_editor_path` which is unreferenced
// outside that one method (`trig_edit_file`). Linker would surface
// `get_folder_path_known` as an unresolved symbol -- compile-only test,
// so it doesn't matter.

#ifdef AAA_SYSTEMUTILS_H
#error "SYSTEMUTILS_H included more than once."
#endif
#define AAA_SYSTEMUTILS_H 1

#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_AAA_STR_H
#	include "code_utils/aaa_str.h"
#endif

namespace sysutils
{
	enum class FOLDER_KNOWN
	{
		PROGRAM_FILES_x64,
		PROGRAM_FILES_x86,
		APP_DATA_LOCAL,
	};

	//	Declared but never defined : aaalua_master.cpp's build_editor_path
	//	is the only caller, and that path is not exercised at runtime in
	//	the compile-only test. Link would surface an unresolved symbol.
	bool	get_folder_path_known( o_str * p_path, FOLDER_KNOWN folder );
}
