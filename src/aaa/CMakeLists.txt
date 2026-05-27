# src/aaa/CMakeLists.txt
#
# Mac-side aaa/ static lib. Currently exposes the POSIX port of
# vendor/aaaseed-engine/Src/aaa/execute.cpp (Win32-only) so callers
# that link `aaa::execute_shell` / `aaa::execute_process` resolve on
# Mac. The vendor .cpp is NOT in any Mac target -- this file owns the
# symbols on Mac.
#
# Created c104 (2026-05-26) as the smallest concrete forward step in
# the aaalua_master.cpp dep cascade (Task #94) : aaalua_master.cpp's
# trig_edit_file() and aaalua_glue.cpp's execute_shell/process bindings
# both call aaa::execute_* ; the symbol-resolution side is now Mac-ready.

add_library(aaaseed_aaa STATIC
    execute_posix.cpp
)

aaa_apply_engine_compile_flags(aaaseed_aaa)

target_include_directories(aaaseed_aaa PUBLIC
    "${AAASEED_ENGINE_SRC}"
    "${AAASEED_ENGINE_SRC}/code_utils"
)
#	No link to aaaseed_code_utils on purpose : the POSIX execute impl
#	uses std::string + posix_spawn / system to stay hermetic (zero
#	o_str usage). Pulling code_utils in here would force every
#	dependent test to also pull the aaa_mem / c_cpu / spy::sleep
#	cascade that has not yet landed on Mac (Task #96 / #98).

set_target_properties(aaaseed_aaa PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
