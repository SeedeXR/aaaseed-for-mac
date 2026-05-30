# Sanity test — pure scaffold, no engine deps. Always builds.
add_executable(aaaseed_sanity_tests
    sanity_test.cpp
)
target_link_libraries(aaaseed_sanity_tests PRIVATE GTest::gtest GTest::gtest_main)
target_compile_features(aaaseed_sanity_tests PRIVATE cxx_std_20)

# Platform header smoke test — exercises the cross-tree force-include and
# include-path set against the AAASeed Windows source headers (no .cpp yet).
add_executable(aaaseed_platform_tests
    aaa_platform_test.cpp
)
target_link_libraries(aaaseed_platform_tests PRIVATE GTest::gtest GTest::gtest_main)
target_compile_features(aaaseed_platform_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_platform_tests)

# Math unit tests — links against aaaseed_math (first engine subsystem).
add_executable(aaaseed_math_tests
    math_rand_test.cpp
    math_matrix_test.cpp
)
target_link_libraries(aaaseed_math_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_math
)
target_compile_features(aaaseed_math_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_math_tests)

# Code-utils unit tests — md5, checksum, strnum, id_unique.
# Links a minimal stubs file providing mem::memcpy / memclear / ERR_PRINT_STRING
# until aaa_mem.cpp and err.cpp are properly ported. See test_engine_stubs.cpp.
add_executable(aaaseed_code_utils_tests
    code_utils_test.cpp
    code_utils_buffer_test.cpp
    "${AAASEED_ENGINE_SRC}/infrastructure/obj/obj.cpp"
    test_engine_stubs.cpp
)
target_link_libraries(aaaseed_code_utils_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_code_utils
)
target_compile_features(aaaseed_code_utils_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_code_utils_tests)

# AAA threading tests — std::-backed mutex / recursive / spinlock + lock guards.
# c_mutex_ct / c_benaphore are Win32-only and gated out of the Mac build.
# c_mutex / c_mutex_recursive extend c_obj (infrastructure/obj/obj.h) — we
# pull obj.cpp + test_engine_stubs.cpp into the test executable directly until
# the infrastructure subsystem lands.
add_executable(aaaseed_aaa_tests
    aaa_threading_test.cpp
    "${AAASEED_ENGINE_SRC}/infrastructure/obj/obj.cpp"
    test_engine_stubs.cpp
)
target_link_libraries(aaaseed_aaa_tests PRIVATE
    GTest::gtest GTest::gtest_main
)
target_compile_features(aaaseed_aaa_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_aaa_tests)

include(GoogleTest)
gtest_discover_tests(aaaseed_sanity_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;sanity"
)
gtest_discover_tests(aaaseed_platform_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;platform"
)
gtest_discover_tests(aaaseed_math_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;math"
)
gtest_discover_tests(aaaseed_code_utils_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;code_utils"
)
gtest_discover_tests(aaaseed_aaa_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;aaa"
)

# aaa::execute_* POSIX port — c104 (2026-05-26). Mac-side parallel
# implementation at src/aaa/execute_posix.cpp owns the symbols ; vendor
# .cpp (Win32-only) is NOT in any Mac target. Smallest forward step in
# the aaalua_master.cpp dep cascade (Task #94).
add_executable(aaaseed_aaa_execute_tests
    aaa_execute_test.cpp
    "${AAASEED_ENGINE_SRC}/infrastructure/obj/obj.cpp"
    test_engine_stubs.cpp
)
#	test_engine_stubs.cpp provides DBG_PRINT_STRING + the o_str stub
#	pair ; aaaseed_aaa carries the POSIX execute_* impl. No link to
#	aaaseed_code_utils on purpose -- execute_posix.cpp uses std::string
#	to avoid pulling the mem/c_cpu cascade that hasn't landed (#96/#98).
target_link_libraries(aaaseed_aaa_execute_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_aaa
)
target_compile_features(aaaseed_aaa_execute_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_aaa_execute_tests)
gtest_discover_tests(aaaseed_aaa_execute_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;aaa_execute"
)

# compute_parallel Mac port -- c116-B (2026-05-26). Sub-task of the
# c115-C audit. Vendor's vendor/aaaseed-engine/Src/infrastructure/
# compute_parallel.h is Win-only (TBB / PPL). The Mac shim at
# src/infrastructure/compute_parallel_mac.{h,cpp} provides
# aaa::parallel::get_thread_nb_def + parallel_for via Grand Central
# Dispatch (dispatch_apply). Hermetic : the sub-lib only uses
# std::thread + std::atomic + libdispatch, no o_str / aaa_mem
# dependency, so it stays clear of c116-A's memcpy/mem cascade.
add_executable(aaaseed_compute_parallel_mac_tests
    compute_parallel_mac_test.cpp
)
target_link_libraries(aaaseed_compute_parallel_mac_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_infrastructure_mac
)
target_compile_features(aaaseed_compute_parallel_mac_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_compute_parallel_mac_tests)
gtest_discover_tests(aaaseed_compute_parallel_mac_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;compute_parallel"
)

# Layer subsystem header parse smoke test — Group D Stage 1 (continuation 30).
# Header-only ; verifies the chain app.h / layer.h / layers.h / module.h /
# modules.h + cameras.h + aaa_global.h compiles under AppleClang force-
# include flags. Identifies blockers before any layer-related .cpp is
# touched. Stage 2 (compile app.cpp) follows once this is green.
add_executable(aaaseed_layer_header_tests
    layer_header_test.cpp
)
target_link_libraries(aaaseed_layer_header_tests PRIVATE
    GTest::gtest GTest::gtest_main
)
target_compile_features(aaaseed_layer_header_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_layer_header_tests)
gtest_discover_tests(aaaseed_layer_header_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;layer_header"
)

# aaalua_array.cpp Stage 4 (continuation 51) — third real engine Lua
# wrapper .cpp on Mac. 319 lines, exposes `array.new_*` typed-userdata
# constructors + __index / __newindex / __len / __gc / set / fill /
# clear / resize metamethods.
#
# The only non-Mac-clean dep is `gol/gol_base.h` -- aaalua_array.cpp
# uses 8 GLenum values as opaque integer type tags (no GL function
# calls verified by grep). Tests-private shim at
# `aaalua_array_shim/gol/gol_base.h` provides GLenum + the 8 constants
# and is added to the target's include path BEFORE the engine path so
# the shim wins for this build only. Zero vendor/ touch.
#
# Out-of-line aaalua_util.cpp methods used : `c_lua_state::add_fn_to_
# table` + `aaalua::new_metatable` -- both verbatim-extracted into
# `aaalua_util_partial_stub.cpp` for now.
add_executable(aaaseed_aaalua_array_tests
    aaalua_array_test.cpp
    "${AAASEED_ENGINE_SRC}/language/lua/aaalua_array.cpp"
    "${AAASEED_ENGINE_SRC}/language/lua/aaalua_exp.cpp"   # for tag_error free fn
    "${AAASEED_ENGINE_SRC}/infrastructure/obj/obj.cpp"
    aaalua_util_partial_stub.cpp
    # c118 : aaalua_ostr_stub.cpp KEPT here -- removal cascade pulls aaa_str.cpp
    # from libaaaseed_code_utils.a which references c_file_virtual / c_dir_pool /
    # BOX_TITLE_ERR_VA that this target doesn't stub (only the audit target does).
    # The stub stays for these single-aaalua-cpp targets ; weak attrs handle ABI.
    aaalua_ostr_stub.cpp
    test_engine_stubs_no_ostr.cpp
)
target_include_directories(aaaseed_aaalua_array_tests BEFORE PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/aaalua_array_shim"   # shadows engine's gol/gol_base.h
)
target_link_libraries(aaaseed_aaalua_array_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_lua
    aaaseed_code_utils
)
target_compile_features(aaaseed_aaalua_array_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_aaalua_array_tests)
gtest_discover_tests(aaaseed_aaalua_array_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;aaalua_array"
)

# aaalua_exp.cpp Stage 3 (continuation 50) — second real engine Lua
# wrapper .cpp on Mac. 160 lines but ~140 are inside `#if 0` /
# `#if !AAA_LIB_LUA_STATIC()` ; effective compiled body is ~15 lines :
# tag_error free function + aaalua::n_exp::register_exp namespace
# member. Test target reuses the stub split landed in continuation 49.
# AAA_LIB_LUA_STATIC is `1` (see aaalua_util.h:15) so the Lua-lib
# polyfills inside the #if block are excluded -- we rely on the
# vendored Lua 5.1.5 to provide luaL_typerror / luaL_argerror /
# luaL_findtable / luaL_error / luaL_checkinteger / luaL_where.
add_executable(aaaseed_aaalua_exp_tests
    aaalua_exp_test.cpp
    "${AAASEED_ENGINE_SRC}/language/lua/aaalua_exp.cpp"
    "${AAASEED_ENGINE_SRC}/infrastructure/obj/obj.cpp"
    aaalua_ostr_stub.cpp   # c118 : kept (no c_file_virtual / dir_pool stubs in this target)
    test_engine_stubs_no_ostr.cpp
)
target_link_libraries(aaaseed_aaalua_exp_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_lua
    aaaseed_code_utils
)
target_compile_features(aaaseed_aaalua_exp_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_aaalua_exp_tests)
gtest_discover_tests(aaaseed_aaalua_exp_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;aaalua_exp"
)

# aaalua_master.cpp compile-only beachhead (Task #94).
# Pulls vendor/aaaseed-engine/Src/language/lua/aaalua_master.cpp into a
# test TU and asks AppleClang to compile it. Goal : surface the exact
# compile errors so we can size the upstream-dep cascade before
# investing in a full port. NOT a link target -- duplicate symbols and
# unresolved infra refs are expected at link time.
# OBJECT library : compiles `aaalua_master.cpp` (vendor TU) without linking.
# Link is intentionally NOT attempted -- the TU has ~20 unresolved symbols
# from the full obj_ui / param / factory / c_lua_wrap surface that requires
# the multi-session port (Task #94). The compile alone is the deliverable.
add_library(aaaseed_aaalua_master_compile_tests OBJECT
    "${AAASEED_ENGINE_SRC}/language/lua/aaalua_master.cpp"
    aaalua_master_compile_test.cpp
)
target_include_directories(aaaseed_aaalua_master_compile_tests BEFORE PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/aaalua_master_shim"   # shadows engine's lib_use.h + SystemUtils.h
)
target_link_libraries(aaaseed_aaalua_master_compile_tests PRIVATE
    GTest::gtest        # for #include <gtest/gtest.h> in the test TU
)
target_compile_features(aaaseed_aaalua_master_compile_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_aaalua_master_compile_tests)
# No gtest_discover_tests -- compile-only target, no ctest entry.

# aaalua_wrap.cpp compile-only beachhead (Task #137 / c106).
# Mirrors the c105 OBJECT-lib approach used for aaalua_master.cpp.
# Pulls vendor/aaaseed-engine/Src/language/lua/aaalua_wrap.cpp (1404 LOC,
# largest of the aaalua trio) into a TU so AppleClang chews through the
# include chain (aaalua_wrap.h, aaalua_util.h, aaalua_debug.h, aaalua_glue.h,
# infrastructure/factory.h, etc.). NOT a link target -- the_factory /
# push_caller / pop_caller / get_caller / FACTORY_DECLARE bodies will be
# unresolved at link time (already documented in the master.cpp set).
# Re-uses aaalua_master_shim/ : same lib_use.h + SystemUtils.h shadowing
# unblocks wrap.cpp too (verified iteratively).
add_library(aaaseed_aaalua_wrap_compile_tests OBJECT
    "${AAASEED_ENGINE_SRC}/language/lua/aaalua_wrap.cpp"
    aaalua_wrap_compile_test.cpp
)
target_include_directories(aaaseed_aaalua_wrap_compile_tests BEFORE PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/aaalua_master_shim"   # reuse master's lib_use.h + SystemUtils.h shims
)
target_link_libraries(aaaseed_aaalua_wrap_compile_tests PRIVATE
    GTest::gtest        # for #include <gtest/gtest.h> in the test TU
)
target_compile_features(aaaseed_aaalua_wrap_compile_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_aaalua_wrap_compile_tests)
# No gtest_discover_tests -- compile-only target, no ctest entry.

# aaalua_util.cpp compile-only beachhead (Task #94 trio third member).
# Mirrors the c105/c106 OBJECT-lib approach used for aaalua_master.cpp
# and aaalua_wrap.cpp. Pulls vendor/aaaseed-engine/Src/language/lua/
# aaalua_util.cpp (884 LOC) into a TU so AppleClang chews through the
# include chain. NOT a link target -- pop_str_and_message /
# pop_str_and_print_error / error_message references the_factory +
# push_caller + pop_caller + get_caller + the un-ported o_str method
# tail (add(char const*), push_name, pop_name) which are unresolved at
# link time (same trio cascade as master/wrap). Re-uses the umbrella
# aaalua_master_shim/ : same lib_use.h + SystemUtils.h shadowing
# unblocks util.cpp too.
add_library(aaaseed_aaalua_util_compile_tests OBJECT
    "${AAASEED_ENGINE_SRC}/language/lua/aaalua_util.cpp"
    aaalua_util_compile_test.cpp
)
target_include_directories(aaaseed_aaalua_util_compile_tests BEFORE PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/aaalua_master_shim"   # reuse master/wrap's lib_use.h + SystemUtils.h shims
)
target_link_libraries(aaaseed_aaalua_util_compile_tests PRIVATE
    GTest::gtest        # for #include <gtest/gtest.h> in the test TU
)
target_compile_features(aaaseed_aaalua_util_compile_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_aaalua_util_compile_tests)
# No gtest_discover_tests -- compile-only target, no ctest entry.

# Task #146 Layer Stage 2 (c119, 2026-05-26) -- app.cpp compile-only beachhead.
# Mirrors c105/c106/c107 OBJECT-lib pattern used for the aaalua trio. Pulls
# vendor/aaaseed-engine/Src/infrastructure/layer/app.cpp (447 LOC, smallest
# leaf of the 4-level render-graph c_layer < c_layers < c_module < c_modules)
# into a TU so AppleClang chews through the include chain (factory + param +
# obj_ui + draw/* + image/* + media/video + obj_ui/bdd/* + shaders + time/aaa_*
# + truetype + gol/gol_matrix + spy + infrastructure/aaa_env + seed_stop).
# NOT a link target -- g_bind_img_2d, c_materials::get_def, c_lights::get_def,
# bind_text::bind, etc are unresolved at link time (expected -- full port deferred).
# Re-uses aaalua_master_shim/ umbrella for lib_use.h + SystemUtils.h shadowing.
# c121 polish : EXCLUDE_FROM_ALL because c119-A's compile-only attempt hit
# the faked-class doctrine WALL (fan-out adapter layer) — target is RED.
# Mirrors the c107 audit-target pattern : keep it discoverable for
# focused `cmake --build . --target aaaseed_layer_app_compile_tests`
# runs but DON'T pollute the default `cmake --build .` / CI cycle.
add_library(aaaseed_layer_app_compile_tests OBJECT EXCLUDE_FROM_ALL
    "${AAASEED_ENGINE_SRC}/infrastructure/layer/app.cpp"
    aaalua_layer_app_compile_test.cpp
)
target_include_directories(aaaseed_layer_app_compile_tests BEFORE PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/aaalua_layer_app_shim" # layer-specific : aaa_thread.h + aaa_gl.h shadows
    "${CMAKE_CURRENT_SOURCE_DIR}/aaalua_master_shim"    # reuse umbrella : lib_use.h + SystemUtils.h
)
target_link_libraries(aaaseed_layer_app_compile_tests PRIVATE
    GTest::gtest        # for #include <gtest/gtest.h> in the test TU
)
target_compile_features(aaaseed_layer_app_compile_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_layer_app_compile_tests)
# No gtest_discover_tests -- compile-only target, no ctest entry.

# aaalua_util.cpp Stage 5 attempted continuation 63 -- DEFERRED.
# Discovery : aaalua_util.cpp's error-handling paths (pop_str_and_message,
# pop_str_and_print_error, error_message) pull on ~10 out-of-line symbols
# from aaalua_wrap.cpp (the_factory, push_caller, pop_caller, get_caller,
# is_instance via FACTORY_DECLARE generates inline body that still
# references the_factory) + aaalua_master.cpp (trig_edit_file) +
# the un-ported o_str methods (add(char const*), push_name, pop_name).
# The lazy-extract stub pattern hits a complexity ceiling here -- the
# right next step is porting aaalua_util + aaalua_wrap + aaalua_master
# AS A GROUP since their static-method dependencies are mutually
# recursive at link time. Tracked in todo.md as a multi-session item.

# c107 trio link-cascade audit target (Task #138).
# This is an EXECUTABLE (not OBJECT) that compiles + LINKS all 6
# aaalua_*.cpp engine TUs together so the linker enumerates every
# unresolved symbol. The build is EXPECTED to fail at link ; the value
# is the ld output, which c108+ uses to plan the subsystem ports
# (obj_ui / param / factory / c_lua_wrap singleton / time_buf / sysutils).
#
# Sources : 6 aaalua_*.cpp + obj.cpp + existing stubs.
# Include dir : reuses the aaalua_master_shim/ umbrella so master/wrap/
# util compile (same shim that c105/c106 use for their OBJECT libs).
# Link libs : aaaseed_lua + aaaseed_code_utils (mirror the c79 debug
# pattern).
#
# c114 (2026-05-26) : PROMOTED from EXCLUDE_FROM_ALL + SUCCEED sentinel
# to a real runtime gtest target. c113 closed LINK GREEN (90 → 0
# unresolved). The audit target now drives aaalua_debug through a live
# lua_State to prove the trio's link is functionally usable, not just
# present in the symbol table. EXCLUDE_FROM_ALL removed ; gtest_discover_tests
# wired below.
add_executable(aaaseed_aaalua_trio_link_audit_tests
    aaalua_trio_link_audit_test.cpp
    "${AAASEED_ENGINE_SRC}/language/lua/aaalua_debug.cpp"
    "${AAASEED_ENGINE_SRC}/language/lua/aaalua_exp.cpp"
    "${AAASEED_ENGINE_SRC}/language/lua/aaalua_array.cpp"
    "${AAASEED_ENGINE_SRC}/language/lua/aaalua_master.cpp"
    "${AAASEED_ENGINE_SRC}/language/lua/aaalua_wrap.cpp"
    "${AAASEED_ENGINE_SRC}/language/lua/aaalua_util.cpp"
    "${AAASEED_ENGINE_SRC}/infrastructure/obj/obj.cpp"
    # c118 : aaalua_ostr_stub.cpp REMOVED -- real aaa_str.cpp in aaaseed_code_utils
    # now self-sufficient after c116 memcpy.cpp port. Weak attrs from c113 already
    # silently deferred to strong defs ; this is explicit cleanup.
    aaalua_obj_ui_stub.cpp                      # c108 : c_obj_ui base bucket (clears 38 syms)
    aaalua_cpu_stub.cpp                         # c108b : c_cpu::one (1 sym) -- pre-written stub re-used
    aaalua_file_virtual_stub.cpp                # c108b : c_file_virtual:: bucket (7 syms)
    aaalua_dir_pool_stub.cpp                    # c108b : c_dir_pool:: bucket (4 syms)
    # c117 : aaalua_mem_dispatch_stub.cpp RETIRED -- c116-A's real
    # src/code_utils/memcpy_mac.cpp ships strong defs for all 14 mem::
    # dispatch flags + memmove/memcpy/memcpy_monothread/memcpy_parallel.
    # The c109 weak stubs already auto-superseded, but explicit removal
    # is the cleanup. Stub file kept on disk for git-blame history but no
    # longer in any build target.
    aaalua_glue_box_stub.cpp                    # c110  : aaalua::register/unregister/b_call_* + BOX_*/SET_COLOR_* (9 syms)
    aaalua_misc_residual_stub.cpp               # c111  : c_trax + c_file_virtual extend + ask_type_io_load + spy::sleep + sysutils + _node_pref (8 syms)
    aaalua_factory_param_stub.cpp               # c112  : c_factory_base + c_param_def + c_param + c_params (10 syms)
    test_engine_stubs_no_ostr.cpp
)
target_include_directories(aaaseed_aaalua_trio_link_audit_tests BEFORE PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/aaalua_master_shim"   # umbrella : lib_use.h + SystemUtils.h shims
    "${CMAKE_CURRENT_SOURCE_DIR}/aaalua_array_shim"    # gol/gol_base.h shim for array.cpp
)
target_link_libraries(aaaseed_aaalua_trio_link_audit_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_lua
    aaaseed_code_utils
    aaaseed_aaa                                  # c108b : aaa::execute_* (POSIX port from c104)
    aaaseed_luasocket                            # c108b : luaopen_socket_core + luaopen_mime_core (2 syms)
)
#	c111 : luasocket -force_load attempt REVERTED (c112) -- promoting
#	aaaseed_luasocket to whole-archive triggers transitive pulls of
#	aaa_str.cpp/aaa_mem.cpp from libaaaseed_code_utils.a, which collide
#	with stubs in aaalua_ostr_stub.cpp / test_engine_stubs_no_ostr.cpp.
#	The 2 luasocket unresolveds remain ; clean fix is the upstream vendor
#	patch adding `extern "C"` to mime.h:21 + luasocket.h:52.
target_compile_features(aaaseed_aaalua_trio_link_audit_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_aaalua_trio_link_audit_tests)
gtest_discover_tests(aaaseed_aaalua_trio_link_audit_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;aaalua_trio_link"
)

# aaalua_debug.cpp Stage 2 (continuation 49) — first real engine Lua
# wrapper .cpp on Mac (91 lines, smallest aaalua_*.cpp). Compiles the
# engine source directly + links our vendored Lua 5.1.5. Verifies the
# debug-trace helpers actually run against a live lua_State, captured
# frames have non-zero level, build_trace produces output.
# Upstream patch (continuation 49) : aaalua_debug.cpp's 3 vestigial
# Windows-only includes (system/shared/SystemUtils.h, shlobj.h,
# ShFolder.h) are now gated behind `#if AAA_OS_WINDOWS()` -- behaviour-
# preserving on Windows (the includes weren't used cross-platform).
add_executable(aaaseed_aaalua_debug_tests
    aaalua_debug_test.cpp
    "${AAASEED_ENGINE_SRC}/language/lua/aaalua_debug.cpp"
    "${AAASEED_ENGINE_SRC}/infrastructure/obj/obj.cpp"
    aaalua_ostr_stub.cpp   # c118 : kept (no c_file_virtual / dir_pool stubs in this target)
    test_engine_stubs_no_ostr.cpp
)
target_link_libraries(aaaseed_aaalua_debug_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_lua
    aaaseed_code_utils
)
target_compile_features(aaaseed_aaalua_debug_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_aaalua_debug_tests)
gtest_discover_tests(aaaseed_aaalua_debug_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;aaalua_debug"
)

# Real-o_str validation test attempted c66 -- DEFERRED.
# Discovery : retiring aaalua_ostr_stub.cpp + linking a target against
# the real aaa_str.cpp pulls in aaa_mem.cpp body which contains mem::
# update() referencing dispatch flags defined ONLY in memcpy.cpp, which
# in turn pulls infrastructure/compute_parallel.h -- the next cascade
# layer. Stubbing the 12 flag symbols in test_engine_stubs collides
# with libaaaseed_code_utils.a's own mem::malloc_aligned etc. Memcpy
# duplicate symbols are a multi-session port problem.
# Stub files (aaalua_cpu_stub.cpp / aaalua_file_virtual_stub.cpp /
# aaalua_dir_pool_stub.cpp) are written and ready for whenever the
# memcpy.cpp port lands. Tracked in todo.md.

# spy.h header parse smoke test (continuation 55).
# Locks in Mac-cleanness of code_utils/spy.h after the one-line upstream
# cleanup that gated SleepEx + WaitForSingleObject bodies behind
# AAA_OS_WINDOWS(). Erodes the aaalua_util.cpp port blocker list by one.
add_executable(aaaseed_spy_header_tests
    spy_header_test.cpp
)
target_link_libraries(aaaseed_spy_header_tests PRIVATE
    GTest::gtest GTest::gtest_main
)
target_compile_features(aaaseed_spy_header_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_spy_header_tests)
gtest_discover_tests(aaaseed_spy_header_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;spy_header"
)

# .app per-frame perf test (continuation 56). Launches aaaseed_app
# with --max-frames 60, captures stdout, parses the [AAASeedApp.Perf]
# line emitted by AAASeedMTKView, asserts avg/max per-frame budgets.
# First .app-level perf test -- exercises the window-driven Metal
# drawable path (different from the headless gol_metal_perf).
add_executable(aaaseed_app_perf_tests
    aaaseed_app_perf_test.cpp
)
target_link_libraries(aaaseed_app_perf_tests PRIVATE
    GTest::gtest GTest::gtest_main
)
target_compile_features(aaaseed_app_perf_tests PRIVATE cxx_std_20)
target_compile_definitions(aaaseed_app_perf_tests PRIVATE
    "AAA_APP_BINARY_PATH=\"$<TARGET_FILE:aaaseed_app>\""
)
add_dependencies(aaaseed_app_perf_tests aaaseed_app)
gtest_discover_tests(aaaseed_app_perf_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "perf;app"
)

# aaalua_*.h header parse smoke test — Stage 1 of the engine Lua wrapper
# port (continuation 48). Mirrors layer_header_test : just compile the
# eight aaalua_*.h headers + their transitive engine deps (aaa_type,
# aaa_str, lua/lua.hpp, infrastructure/obj/obj_ui.h, aaa/aaa_mutex.h).
# Engine bundles its own LuaJIT-flavored Lua headers at
# vendor/aaaseed-engine/Include/lua/ ; the Lua C API is compatible with
# our vanilla Lua 5.1.5 link target. If this builds, Stage 2 (port the
# smallest .cpp -- aaalua_debug or aaalua_array) can begin.
add_executable(aaaseed_aaalua_header_tests
    aaalua_header_test.cpp
)
target_link_libraries(aaaseed_aaalua_header_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_lua    # inline c_lua_state helpers in aaalua_util.h call lua_rawgeti / lua_settop / lua_tonumber
)
target_compile_features(aaaseed_aaalua_header_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_aaalua_header_tests)
gtest_discover_tests(aaaseed_aaalua_header_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;aaalua_header"
)

# Infrastructure obj_ui header parse smoke test — Stage 1 of the obj_ui port.
# Just compiles the include chain ; does NOT link against obj_ui.cpp or
# factory/param .cpp files yet. If this builds, the header graph is healthy
# and we can move to Stage 2 (compile the bottom of the .cpp stack).
add_executable(aaaseed_obj_ui_header_tests
    obj_ui_header_test.cpp
)
target_link_libraries(aaaseed_obj_ui_header_tests PRIVATE
    GTest::gtest GTest::gtest_main
)
target_compile_features(aaaseed_obj_ui_header_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_obj_ui_header_tests)
gtest_discover_tests(aaaseed_obj_ui_header_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;obj_ui_header"
)

# NSight → os_signpost shim. Header-only ; exercises the macro at runtime
# so the signposts show up in Instruments when profiling.
add_executable(aaaseed_nsight_tests
    nsight_signpost_test.cpp
)
target_link_libraries(aaaseed_nsight_tests PRIVATE
    GTest::gtest GTest::gtest_main
)
target_compile_features(aaaseed_nsight_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_nsight_tests)
gtest_discover_tests(aaaseed_nsight_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;nsight"
)

# Metal smoke test — proves metal-cpp + Metal framework work from pure C++.
# Foundation of the future GOL::MetalBackend. Links Metal, Foundation,
# QuartzCore frameworks. The test TU defines NS/MTL/CA_PRIVATE_IMPLEMENTATION
# so metal-cpp's selector/class symbols get emitted exactly once.
find_library(METAL_FRAMEWORK Metal REQUIRED)
find_library(FOUNDATION_FRAMEWORK Foundation REQUIRED)
find_library(QUARTZCORE_FRAMEWORK QuartzCore REQUIRED)

add_executable(aaaseed_metal_tests
    metal_device_test.cpp
)
target_link_libraries(aaaseed_metal_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_gol_metal     # brings in the PRIVATE_IMPLEMENTATION symbols
)
target_compile_features(aaaseed_metal_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_metal_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;metal"
)

# GOL Metal backend tests — exercises GOL::MetalBackend through the abstract
# GOL::Backend interface. Polymorphic dispatch, lifecycle, buffer / texture
# creation. Headless (no MTKView yet).
add_executable(aaaseed_gol_metal_tests
    gol_metal_backend_test.cpp
    gol_metal_render_test.cpp
)
target_link_libraries(aaaseed_gol_metal_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_gol_metal
)
target_compile_features(aaaseed_gol_metal_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_gol_metal_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;gol_metal"
)

# MEU runner shim tests (c142-B, finished post-API-overload). Exercises
# the Mac-native MEU runner that loads a Lua script + drives the Path A
# catalog directly through MetalBackend, sidestepping the c119-A layer-
# subsystem doctrine wall per project_v1_ship_gate.md.
add_executable(aaaseed_meu_runner_tests
    meu_runner_test.cpp
)
target_link_libraries(aaaseed_meu_runner_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_meu_runner
    aaaseed_gol_metal
)
target_compile_features(aaaseed_meu_runner_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_meu_runner_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;meu;runner"
)

# c146 : MEU HUD overlay unit tests. Exercises the Lua
# aaa.draw_hud_text binding + Runner::get_pending_hud_text host
# accessor. Sibling to meu_runner_test.cpp ; same MetalBackend +
# temp-Lua-file pattern. Closes the c143-C "which shader is active"
# visual gap by validating the queue-and-readback contract.
add_executable(aaaseed_meu_hud_tests
    meu_hud_test.cpp
)
target_link_libraries(aaaseed_meu_hud_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_meu_runner
    aaaseed_gol_metal
)
target_compile_features(aaaseed_meu_hud_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_meu_hud_tests
    DISCOVERY_TIMEOUT 30
    #  feedback_ctest_label_first_only.md : `unit` first, `hud` last
    #  so `ctest -L hud` future-proofs sub-selecting the cohort.
    PROPERTIES LABELS "unit;meu;hud"
)

# 3D texture smoke tests — proves MetalBackend can author + sample a
# `texture3d<float>` end-to-end. Unblocks vol3d_Plane / vol3d_Voxel
# shader baselining (c131+).
add_executable(aaaseed_gol_metal_texture_3d_tests
    gol_metal_texture_3d_test.cpp
)
target_link_libraries(aaaseed_gol_metal_texture_3d_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_gol_metal
)
target_compile_features(aaaseed_gol_metal_texture_3d_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_gol_metal_texture_3d_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;gol_metal"
)

# Shader MSL port test (Path A : direct GLSL -> MSL hand-port).
# Verifies the hand-translated `ps_edgeoverlay` MSL compiles via
# `MTL::Device::newLibrary` AND via `MetalBackend::create_program_msl`.
# Mirrors the gol_metal_tests linkage pattern.
add_executable(aaaseed_shader_msl_port_tests
    shader_msl_port_test.cpp
)
target_link_libraries(aaaseed_shader_msl_port_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_gol_metal
)
target_compile_features(aaaseed_shader_msl_port_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_shader_msl_port_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "integration;shader_msl_port"
)

# Lua <-> engine bridge test (continuation 47).
# First end-to-end proof that vendored Lua can drive engine code on
# Mac : wraps CLAMP / MIN / MAX (from aaa_util.h) as Lua-callable C
# functions, calls them from Lua scripts, verifies semantics. Documents
# the pattern future `*_lua.cpp` ports follow.
add_executable(aaaseed_lua_engine_bridge_tests
    lua_engine_bridge_test.cpp
)
target_link_libraries(aaaseed_lua_engine_bridge_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_lua
    aaaseed_code_utils
)
target_compile_features(aaaseed_lua_engine_bridge_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_lua_engine_bridge_tests)
gtest_discover_tests(aaaseed_lua_engine_bridge_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;lua_engine"
)

# LuaSocket smoke test (continuation 62 -- Phase 5 fourth beachhead).
# Last `aaalua_util.cpp` blocker resolution. Opens vendored LuaSocket
# 3.1.0 via luaopen_socket_core + luaopen_mime_core, verifies the
# returned module tables expose the canonical C-side functions
# (socket.tcp/udp/gettime/select, mime.b64/unb64/qp/...).
add_executable(aaaseed_luasocket_smoke_tests
    luasocket_smoke_test.cpp
)
target_link_libraries(aaaseed_luasocket_smoke_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_luasocket
    aaaseed_lua
)
target_compile_features(aaaseed_luasocket_smoke_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_luasocket_smoke_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;luasocket"
)

# Lua 5.1 smoke test (continuation 45 -- Phase 5 second beachhead).
# Verifies the vendored Lua static lib produces a working runtime on
# Apple Silicon : open state, eval arithmetic, string.format, C fn
# registration + call, error reporting, version check.
add_executable(aaaseed_lua_smoke_tests
    lua_smoke_test.cpp
)
target_link_libraries(aaaseed_lua_smoke_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_lua
)
target_compile_features(aaaseed_lua_smoke_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_lua_smoke_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;lua"
)

# meu_compat.lua shim smoke test (Phase 6 partial unblock).
# Opens a Lua state, injects a stub `aaa.platform_name()`, loads the
# pure-Lua shim at src/lua/meu_compat.lua, calls `detect()`, and
# verifies the spout <-> syphon bridge installs on the simulated Mac
# path. Mirrors the lua_smoke_test target shape : only links
# aaaseed_lua, no engine .cpp dependencies.
add_executable(aaaseed_meu_compat_tests
    meu_compat_test.cpp
)
target_link_libraries(aaaseed_meu_compat_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_lua
)
target_compile_features(aaaseed_meu_compat_tests PRIVATE cxx_std_20)
target_compile_definitions(aaaseed_meu_compat_tests PRIVATE
    "AAA_MEU_COMPAT_LUA=\"${CMAKE_SOURCE_DIR}/src/lua/meu_compat.lua\""
)
gtest_discover_tests(aaaseed_meu_compat_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;meu_compat"
)

# param_draw.h header parse smoke test (Task #30, c113 Agent A audit).
# Stage 1 of the param_draw port -- header-only ; defers the .cpp body
# to post-GOL state-stack. Mirrors aaalua_header_test shape : no link
# to param_draw.cpp, just verifies the chain compiles + static_asserts
# the free function `param_draw(c_obj_ui*)` and the `aaa::param::flat`
# namespace are exposed with the expected signatures.
add_executable(aaaseed_param_draw_header_tests
    param_draw_header_test.cpp
)
target_link_libraries(aaaseed_param_draw_header_tests PRIVATE
    GTest::gtest GTest::gtest_main
)
target_compile_features(aaaseed_param_draw_header_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_param_draw_header_tests)
gtest_discover_tests(aaaseed_param_draw_header_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;param_draw_header"
)

# bdd_cell_draw.h header parse smoke test (Task #30, c113 Agent A audit).
# Stage 1 of the bdd_cell_draw port -- header-only ; defers the .cpp
# body to post-GOL state-stack. Mirrors aaalua_header_test shape : no
# link to bdd_cell_draw.cpp, just verifies the chain compiles + static_
# asserts c_bdd_cell_draw derives from c_bdd and stays `final`.
add_executable(aaaseed_bdd_cell_draw_header_tests
    bdd_cell_draw_header_test.cpp
)
target_link_libraries(aaaseed_bdd_cell_draw_header_tests PRIVATE
    GTest::gtest GTest::gtest_main
)
target_compile_features(aaaseed_bdd_cell_draw_header_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_bdd_cell_draw_header_tests)
gtest_discover_tests(aaaseed_bdd_cell_draw_header_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;bdd_cell_draw_header"
)

# tinyexr round-trip test (continuation 67 -- Phase 5 5th beachhead).
# Closes the EXR asset-format gap. Encode a 4x4 RGBA FP32 pattern via
# SaveEXRToMemory, decode via LoadEXRFromMemory, verify exact float
# round-trip (lossless when save_as_fp16=0).
add_executable(aaaseed_tinyexr_tests
    tinyexr_test.cpp
)
target_link_libraries(aaaseed_tinyexr_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_tinyexr
)
target_compile_features(aaaseed_tinyexr_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_tinyexr_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;tinyexr"
)

# stb_image round-trip test (continuation 44 -- Phase 5 beachhead).
# Encode a known 4x4 RGBA pattern to PNG bytes via stbi_write_png_to_func,
# decode via stbi_load_from_memory, verify exact pixel match.
add_executable(aaaseed_stb_image_tests
    stb_image_test.cpp
)
target_link_libraries(aaaseed_stb_image_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_stb
)
target_compile_features(aaaseed_stb_image_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_stb_image_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;stb_image"
)

# stb_truetype font beachhead test (continuation 57 -- Phase 5 third
# beachhead, alongside stb_image c44 + Lua 5.1 c45). Loads
# SourceCodePro-Medium.ttf from vendor/aaaseed-runtime, rasterizes 'A'
# at 32 px, verifies non-zero glyph pixels + reasonable font metrics.
# Reuses the existing aaaseed_stb static lib (now also compiles
# STB_TRUETYPE_IMPLEMENTATION via stb_impl.c).
add_executable(aaaseed_stb_truetype_tests
    stb_truetype_test.cpp
)
target_link_libraries(aaaseed_stb_truetype_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_stb
)
target_compile_features(aaaseed_stb_truetype_tests PRIVATE cxx_std_20)
target_compile_definitions(aaaseed_stb_truetype_tests PRIVATE
    "AAA_STB_TT_FONT_PATH=\"${CMAKE_SOURCE_DIR}/vendor/aaaseed-runtime/AAAKernel/Fonts/Google/Source_Code_Pro/static/SourceCodePro-Medium.ttf\""
)
gtest_discover_tests(aaaseed_stb_truetype_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;stb_truetype"
)

# Glyph atlas helper test (continuation 58 -- first building block for
# the .app debug HUD). Builds a 256x256 alpha-8 atlas of printable
# ASCII from Source Code Pro at 16 px ; saves a PNG fixture for visual
# inspection. Links aaaseed_text (the new subsystem) + aaaseed_stb for
# stbi_write_png.
add_executable(aaaseed_glyph_atlas_tests
    glyph_atlas_test.cpp
)
target_link_libraries(aaaseed_glyph_atlas_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_text
)
target_compile_features(aaaseed_glyph_atlas_tests PRIVATE cxx_std_20)
target_compile_definitions(aaaseed_glyph_atlas_tests PRIVATE
    "AAA_STB_TT_FONT_PATH=\"${CMAKE_SOURCE_DIR}/vendor/aaaseed-runtime/AAAKernel/Fonts/Google/Source_Code_Pro/static/SourceCodePro-Medium.ttf\""
    "AAA_GLYPH_ATLAS_OUT_DIR=\"${CMAKE_BINARY_DIR}/bin\""
)
gtest_discover_tests(aaaseed_glyph_atlas_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;glyph_atlas"
)

# Text rendering end-to-end integration test (continuation 60). Builds
# atlas + uploads to R8 MTLTexture + compiles text MSL + draws "AB" to
# 128x64 offscreen RGBA8 target + readback. Final C-side proof of the
# text pipeline before the AAASeedMTKView debug HUD wiring.
add_executable(aaaseed_text_render_integration_tests
    text_render_integration_test.cpp
)
target_link_libraries(aaaseed_text_render_integration_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_text
    aaaseed_gol_metal
)
target_compile_features(aaaseed_text_render_integration_tests PRIVATE cxx_std_20)
target_compile_definitions(aaaseed_text_render_integration_tests PRIVATE
    "AAA_STB_TT_FONT_PATH=\"${CMAKE_SOURCE_DIR}/vendor/aaaseed-runtime/AAAKernel/Fonts/Google/Source_Code_Pro/static/SourceCodePro-Medium.ttf\""
)
gtest_discover_tests(aaaseed_text_render_integration_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "integration;text_render"
)

# Path A runtime end-to-end test (continuation 38).
# Loads a single .metal from src/shaders/msl/, sets up an offscreen
# render target + procedural source texture + uniform buffer, renders
# through the catalog shader, reads back pixels, asserts the engine
# semantics actually fired. Proves the catalog isn't dead weight at
# the GPU level.
add_executable(aaaseed_path_a_runtime_tests
    path_a_runtime_test.cpp
)
target_link_libraries(aaaseed_path_a_runtime_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_gol_metal
)
target_compile_features(aaaseed_path_a_runtime_tests PRIVATE cxx_std_20)
target_compile_definitions(aaaseed_path_a_runtime_tests PRIVATE
    "AAA_SHADERS_MSL_DIR=\"${CMAKE_SOURCE_DIR}/src/shaders/msl\""
)
gtest_discover_tests(aaaseed_path_a_runtime_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "integration;path_a_runtime"
)

# Path A catalog : iterates every .metal in src/shaders/msl/ and feeds
# each one to MetalBackend. Auto-discovers ports landed by the
# glsl_to_msl tool. Floor of 8 shaders ; bump as catalog grows.
# Acts as the Path A regression corpus.
add_executable(aaaseed_path_a_catalog_tests
    path_a_catalog_test.cpp
)
target_link_libraries(aaaseed_path_a_catalog_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_gol_metal
)
target_compile_features(aaaseed_path_a_catalog_tests PRIVATE cxx_std_20)
target_compile_definitions(aaaseed_path_a_catalog_tests PRIVATE
    "AAA_SHADERS_MSL_DIR=\"${CMAKE_SOURCE_DIR}/src/shaders/msl\""
)
gtest_discover_tests(aaaseed_path_a_catalog_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "regression;path_a"
)

# Phase 4 input plumbing test (continuation 36) — Objective-C++ TU
# that synthesizes NSEvent keyDown / keyUp and verifies the
# AAASeedInputView state surface. Compiles the input-view .mm directly
# rather than linking the full .app bundle to keep the unit test
# hermetic from NSApplication / window machinery.
find_library(COCOA_FRAMEWORK_INPUT Cocoa REQUIRED)
find_library(METALKIT_FRAMEWORK_INPUT MetalKit REQUIRED)
#  c133 view-side wiring : AAASeedInputView now owns EventBridge +
#  GestureBridge instances (per-view, std::unique_ptr) and forwards
#  every NSResponder method into them. The test TU therefore pulls in
#  both bridge .mm files alongside the view .mm so accessor tests link.
add_executable(aaaseed_input_view_tests
    input_view_test.mm
    "${CMAKE_SOURCE_DIR}/src/ui/macos/AAASeedInputView.mm"
    "${CMAKE_SOURCE_DIR}/src/ui/macos/aaa_event_bridge.mm"
    "${CMAKE_SOURCE_DIR}/src/ui/macos/aaa_event_bridge_gesture.mm"
)
set_source_files_properties(input_view_test.mm
    "${CMAKE_SOURCE_DIR}/src/ui/macos/AAASeedInputView.mm"
    "${CMAKE_SOURCE_DIR}/src/ui/macos/aaa_event_bridge.mm"
    "${CMAKE_SOURCE_DIR}/src/ui/macos/aaa_event_bridge_gesture.mm"
    PROPERTIES
    LANGUAGE OBJCXX
    COMPILE_FLAGS "-fno-objc-arc"
)
target_link_libraries(aaaseed_input_view_tests PRIVATE
    GTest::gtest GTest::gtest_main
    #  c144 : AAASeedInputView now references aaa::meu::Runner::on_*_event
    #  for the MEU forwarding path. Linking the runner sub-lib here keeps
    #  the existing c133 unit test green without pulling in the full
    #  MTKView delegate (the InputView resolves the Runner pointer at
    #  runtime via NSInvocation on -meuRunner, not at link time).
    aaaseed_meu_runner
    "${COCOA_FRAMEWORK_INPUT}"
    "${METALKIT_FRAMEWORK_INPUT}"
)
target_include_directories(aaaseed_input_view_tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_features(aaaseed_input_view_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_input_view_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;input"
)

# stb_image decode-throughput performance test (continuation 54).
# Closes the perf gap on the asset pipeline live in the .app
# (continuation 46). Encodes a 256x256 RGBA gradient to PNG in memory
# once, then times 200 decodes via stbi_load_from_memory with
# os_signpost + chrono + budget assertion. Reports Mpix/sec headline.
add_executable(aaaseed_stb_image_perf_tests
    stb_image_perf_test.cpp
)
target_link_libraries(aaaseed_stb_image_perf_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_stb
)
target_compile_features(aaaseed_stb_image_perf_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_stb_image_perf_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "perf;stb_image"
)

# Glyph atlas build performance test (continuation 74).
# Closes the perf-coverage gap on the c58 atlas builder helper.
# Measures 100 iterations of building a 256x256 alpha-8 atlas from
# Source Code Pro Medium @ 16 px. The .app pays this once at boot ;
# perf test catches future stb_truetype pack-API regressions.
add_executable(aaaseed_glyph_atlas_perf_tests
    glyph_atlas_perf_test.cpp
)
target_link_libraries(aaaseed_glyph_atlas_perf_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_text
)
target_compile_features(aaaseed_glyph_atlas_perf_tests PRIVATE cxx_std_20)
target_compile_definitions(aaaseed_glyph_atlas_perf_tests PRIVATE
    "AAA_STB_TT_FONT_PATH=\"${CMAKE_SOURCE_DIR}/vendor/aaaseed-runtime/AAAKernel/Fonts/Google/Source_Code_Pro/static/SourceCodePro-Medium.ttf\""
)
gtest_discover_tests(aaaseed_glyph_atlas_perf_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "perf;glyph_atlas"
)

# tinyexr decode-throughput performance test (continuation 73).
# Closes the perf gap on the Phase 5 5th beachhead (tinyexr, c67).
# Encodes a 128x128 RGBA FP32 gradient to EXR in memory once, then
# times 100 decodes via LoadEXRFromMemory with os_signpost + chrono +
# budget assertion. Reports Mpix/sec headline for asset-pipeline
# planning alongside the c54 stb_image_perf 52.7 Mpix/sec baseline.
add_executable(aaaseed_tinyexr_perf_tests
    tinyexr_perf_test.cpp
)
target_link_libraries(aaaseed_tinyexr_perf_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_tinyexr
)
target_compile_features(aaaseed_tinyexr_perf_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_tinyexr_perf_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "perf;tinyexr"
)

# Path A shader compile-time performance test (continuation 53).
# Iterates every .metal file in src/shaders/msl/ and measures the
# MetalBackend::create_program_msl wall-clock + os_signpost interval
# per shader. Per-shader budget 250 ms ; sweep budget 2 s. Guards
# against Apple Metal compiler regressions and against accidentally
# landing a pathologically-large port. CTest label "perf;path_a".
add_executable(aaaseed_path_a_compile_perf_tests
    path_a_compile_perf_test.cpp
)
target_link_libraries(aaaseed_path_a_compile_perf_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_gol_metal
)
target_compile_features(aaaseed_path_a_compile_perf_tests PRIVATE cxx_std_20)
target_compile_definitions(aaaseed_path_a_compile_perf_tests PRIVATE
    "AAA_SHADERS_MSL_DIR=\"${CMAKE_SOURCE_DIR}/src/shaders/msl\""
)
gtest_discover_tests(aaaseed_path_a_compile_perf_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "perf;path_a"
)

# Lua bridge performance test (continuation 52).
# Measures the steady-state Lua->C->Lua roundtrip cost of an AAALUACALL-
# style binding. Fills the perf-coverage gap for the wrapper port work
# of continuations 47-51 (5 Lua wrapper test suites landed but ZERO
# perf coverage of the shared hot path). Three-artifact perf doctrine :
# os_signpost interval + std::chrono measurement + budget assertion.
add_executable(aaaseed_lua_bridge_perf_tests
    lua_bridge_perf_test.cpp
)
target_link_libraries(aaaseed_lua_bridge_perf_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_lua
)
target_compile_features(aaaseed_lua_bridge_perf_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_lua_bridge_perf_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "perf;lua_bridge"
)

# GOL Metal performance scaffold (continuation 26).
# Demonstrates the perf-doctrine three-artifact pattern (philosophy.md
# Part 2.95) : os_signpost interval + std::chrono measurement + budget
# assertion. Labelled `perf` so CI can run perf tests selectively
# (`ctest -L perf`).
add_executable(aaaseed_gol_metal_perf_tests
    gol_metal_perf_test.cpp
)
target_link_libraries(aaaseed_gol_metal_perf_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_gol_metal
)
target_compile_features(aaaseed_gol_metal_perf_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_gol_metal_perf_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "perf;gol_metal"
)

# Phase 3 EXIT per-frame perf baseline (continuation 121-B).
# Drives the Path A `ps_Maa_add_scale.metal` shader through MetalBackend
# at 1280x720 BGRA8 offscreen for N=100 iterations ; captures mean /
# median / p95 frame-time and asserts mean is within the Apple-M4
# envelope [10us, 5000us]. Establishes the Mac side of the Phase 3 EXIT
# "frame-time within 1.2x Windows reference" cross-platform contract --
# Windows reference number lands in c126+.
add_executable(aaaseed_phase3_per_frame_perf_tests
    phase3_per_frame_perf_test.cpp
)
target_link_libraries(aaaseed_phase3_per_frame_perf_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_gol_metal
)
target_compile_features(aaaseed_phase3_per_frame_perf_tests PRIVATE cxx_std_20)
target_compile_definitions(aaaseed_phase3_per_frame_perf_tests PRIVATE
    "AAA_SHADERS_MSL_DIR=\"${CMAKE_SOURCE_DIR}/src/shaders/msl\""
)
#	NOTE on label ordering : CMake's `gtest_discover_tests` PROPERTIES
#	LABELS forwarding strips the `;` separator on this toolchain, so only
#	the FIRST label survives to `set_tests_properties` (verified : the
#	c117 memcpy_parallel test labelled "unit;perf;memcpy_parallel" shows
#	up only under `ctest -L unit`, not `ctest -L perf`). Putting `perf`
#	FIRST is what makes `ctest -L perf` pick this test up -- matches the
#	convention used by gol_metal_perf / stb_image_perf / etc. The spec
#	intent ("unit;perf;phase3") is preserved here as a comment ; the
#	primary axis is "perf" for the per-frame budget surface.
gtest_discover_tests(aaaseed_phase3_per_frame_perf_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "perf;unit;phase3"
)

# Phase 3 Path A revival perf budgets (c135-A noise, c138-A bloom,
# c139-A motion blur). Three perf-labelled tests each driving its real
# revived shader at 1280x720 BGRA8 (the c121-B benchmark size = .app
# present resolution). Closes the per-revival perf-budget gap the
# project doctrine requires for every Metal backend change.
add_executable(aaaseed_revival_perf_tests
    revival_perf_test.cpp
)
target_link_libraries(aaaseed_revival_perf_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_gol_metal
)
target_compile_features(aaaseed_revival_perf_tests PRIVATE cxx_std_20)
target_compile_definitions(aaaseed_revival_perf_tests PRIVATE
    "AAA_SHADERS_MSL_DIR=\"${CMAKE_SOURCE_DIR}/src/shaders/msl\""
)
#	Label order : `perf` MUST be first ; see [[ctest-label-first-only]].
#	gtest_discover_tests propagates only the first LABELS entry on this
#	CMake/AppleClang toolchain, so `ctest -L perf` requires the primary
#	axis to head the list. Other labels follow as metadata.
gtest_discover_tests(aaaseed_revival_perf_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "perf;unit;revival;phase3"
)

# Mac memcpy.cpp port test (c115, Task #98 / Phase 5).
# Verifies the hermetic src/code_utils/memcpy_mac.cpp forwarders to
# libc work correctly + the 14 dispatch data symbols resolve at link
# time. Supersedes the c109 weak stubs in aaalua_mem_dispatch_stub.cpp
# (those stubs were `__attribute__((weak))` exactly for this transition ;
# they remain in the trio-audit target's source list as a safety net
# but the strong defs in aaaseed_code_utils win at link time).
add_executable(aaaseed_code_utils_memcpy_tests
    code_utils_memcpy_mac_test.cpp
    "${AAASEED_ENGINE_SRC}/infrastructure/obj/obj.cpp"
    test_engine_stubs.cpp
)
target_link_libraries(aaaseed_code_utils_memcpy_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_code_utils
)
target_compile_features(aaaseed_code_utils_memcpy_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_code_utils_memcpy_tests)
gtest_discover_tests(aaaseed_code_utils_memcpy_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;code_utils_memcpy"
)

# c117 : perf-labelled benchmark for mem::memcpy_parallel (GCD dispatch_apply)
# vs single-thread memcpy on a 16MB buffer. Labels include "perf" so this is
# counted under the `ctest -L perf` filter (8th perf-labelled test as of c117).
add_executable(aaaseed_code_utils_memcpy_parallel_perf_tests
    code_utils_memcpy_parallel_perf_test.cpp
    "${AAASEED_ENGINE_SRC}/infrastructure/obj/obj.cpp"
    test_engine_stubs.cpp
)
target_link_libraries(aaaseed_code_utils_memcpy_parallel_perf_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_code_utils
)
target_compile_features(aaaseed_code_utils_memcpy_parallel_perf_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_code_utils_memcpy_parallel_perf_tests)
gtest_discover_tests(aaaseed_code_utils_memcpy_parallel_perf_tests
    DISCOVERY_TIMEOUT 30
    # c121-B finding : gtest_discover_tests honors FIRST label only on this
    # toolchain (CMake semicolon-stripping quirk). Put "perf" first so
    # `ctest -L perf` includes this test ; "unit" + "memcpy_parallel" remain
    # in the metadata but are not used as filter keys.
    PROPERTIES LABELS "perf;unit;memcpy_parallel"
)

# POSIX file/dir test : pins down the syscall contract our Mac branches
# of aaa_dir.cpp + aaa_file.cpp depend on. Hermetic, no engine init.
add_executable(aaaseed_code_utils_file_tests
    code_utils_file_test.cpp
)
target_link_libraries(aaaseed_code_utils_file_tests PRIVATE
    GTest::gtest GTest::gtest_main
)
target_compile_features(aaaseed_code_utils_file_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_code_utils_file_tests)
gtest_discover_tests(aaaseed_code_utils_file_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;code_utils_file"
)

# POSIX time / os_version test : POSIX-API sanity (clock_gettime,
# sysctlbyname, gethostname) + engine wrappers (aaa::time::store and
# n_os_version::get_info). Links the real ported .cpp files via
# aaaseed_code_utils + the obj engine stub for the few unresolved syms.
add_executable(aaaseed_code_utils_time_tests
    code_utils_time_test.cpp
    "${AAASEED_ENGINE_SRC}/infrastructure/obj/obj.cpp"
    test_engine_stubs.cpp
)
target_link_libraries(aaaseed_code_utils_time_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_code_utils
)
target_compile_features(aaaseed_code_utils_time_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_code_utils_time_tests)
gtest_discover_tests(aaaseed_code_utils_time_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;code_utils_time"
)

# POSIX console / log test : drives console.cpp's real Mac branches
# (ANSI escapes via isatty + fputs) ; the err.cpp output paths are
# stubbed through test_engine_stubs.cpp until err.cpp's transitive
# infra deps (layer/, ui/dialog_dev) are ported.
add_executable(aaaseed_code_utils_log_tests
    code_utils_log_test.cpp
    "${AAASEED_ENGINE_SRC}/infrastructure/obj/obj.cpp"
    test_engine_stubs.cpp
)
target_link_libraries(aaaseed_code_utils_log_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_code_utils
)
target_compile_features(aaaseed_code_utils_log_tests PRIVATE cxx_std_20)
aaa_apply_engine_compile_flags(aaaseed_code_utils_log_tests)
gtest_discover_tests(aaaseed_code_utils_log_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;code_utils_log"
)

# c119-B / Task #150 : aaa_event_bridge unit tests. Drains synthetic
# NSEvents from an AAASeedInputView into the EngineEvent FIFO and asserts
# Kind + payload. Hermetic : links the new aaaseed_event_bridge static lib
# only (which itself links Cocoa + MetalKit). No engine link, no o_str,
# no aaa_mem. Compiles the test TU as ObjC++.
find_library(COCOA_FRAMEWORK_BRIDGE Cocoa REQUIRED)
find_library(METALKIT_FRAMEWORK_BRIDGE MetalKit REQUIRED)
#  c133 : AAASeedInputView.mm was removed from the aaaseed_event_bridge
#  static lib to break the duplicate-ObjC-class warning at app-link.
#  Pull it in here directly so the EventBridge tests still have the
#  view symbol to synthesize events against.
add_executable(aaaseed_event_bridge_tests
    aaa_event_bridge_test.mm
    "${CMAKE_SOURCE_DIR}/src/ui/macos/AAASeedInputView.mm"
)
set_source_files_properties(aaa_event_bridge_test.mm
    "${CMAKE_SOURCE_DIR}/src/ui/macos/AAASeedInputView.mm"
    PROPERTIES
    LANGUAGE OBJCXX
    COMPILE_FLAGS "-fno-objc-arc"
)
target_link_libraries(aaaseed_event_bridge_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_event_bridge
    #  c144 : AAASeedInputView (compiled in here directly) references the
    #  MEU runner's on_*_event symbols. Static-link the runner lib so the
    #  view's c133-A bridge ingest path still has the symbols available.
    aaaseed_meu_runner
    "${COCOA_FRAMEWORK_BRIDGE}"
    "${METALKIT_FRAMEWORK_BRIDGE}"
)
target_include_directories(aaaseed_event_bridge_tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_features(aaaseed_event_bridge_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_event_bridge_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;event_bridge"
)

# c133 / Phase 4 (todo L291) : aaa_event_bridge_gesture unit tests.
# Trackpad / gesture adapter coverage -- Scroll / Magnify / Rotate /
# Swipe queue shape via the GestureBridge `post_*` API. Plain C++ TU
# (no NSEvent synth -- AppKit throws for gesture types ; same caveat
# as the c119-B scroll path documents). Links only the hermetic
# aaaseed_event_bridge static lib (Cocoa transitively).
add_executable(aaaseed_event_bridge_gesture_tests
    aaa_event_bridge_gesture_test.cpp
)
target_link_libraries(aaaseed_event_bridge_gesture_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_event_bridge
)
target_include_directories(aaaseed_event_bridge_gesture_tests PRIVATE
    "${CMAKE_SOURCE_DIR}"
)
target_compile_features(aaaseed_event_bridge_gesture_tests PRIVATE cxx_std_20)
#  Label ordering follows feedback_ctest_label_first_only.md : on this
#  toolchain gtest_discover_tests honors only the FIRST label. `phase4`
#  is the unique discriminator (`unit` already matches 200+ other tests,
#  `ui` is also broadly shared) -- putting it first lets `ctest -L phase4`
#  cleanly select the Phase 4 cohort.
gtest_discover_tests(aaaseed_event_bridge_gesture_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "phase4;unit;ui"
)

# c144 / Phase 4 v1 ship-gate (todo.md L29) : engine-side `c_event_mouse`
# + `c_event_keyboard` adapter coverage. Plain C++ TU (no NSEvent synth ;
# tests push EngineEvents directly into EventBridge and verify the
# adapter's POD output). Links only the hermetic aaaseed_event_bridge
# static lib (Cocoa transitively). NO link to aaaseed_code_utils --
# preserves the c104 hermetic doctrine.
add_executable(aaaseed_event_adapter_mac_tests
    aaa_event_adapter_mac_test.cpp
)
target_link_libraries(aaaseed_event_adapter_mac_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_event_bridge
)
target_include_directories(aaaseed_event_adapter_mac_tests PRIVATE
    "${CMAKE_SOURCE_DIR}"
)
target_compile_features(aaaseed_event_adapter_mac_tests PRIVATE cxx_std_20)
#  Label ordering follows feedback_ctest_label_first_only.md : on this
#  toolchain gtest_discover_tests honors only the FIRST label. `phase4`
#  is the unique discriminator. Spec calls for `phase4;unit;ui;
#  event-adapter` (4 labels, first wins).
gtest_discover_tests(aaaseed_event_adapter_mac_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "phase4;unit;ui;event-adapter"
)

# c132 / Phase 4 unblock : aaaseed_ui_file_dialog tests. Exercises the
# pure-CPU normalisation + struct-usability surface of the hermetic
# aaaseed_ui_file_dialog_mac sub-lib (NSOpenPanel/NSSavePanel impl
# behind aaa::ui::*_file_dialog). The runModal path needs a live
# NSApplication and is NOT exercised here ; smoke test only takes
# function addresses to force link resolution. Hermetic : links only
# the new sub-lib (which transitively brings AppKit + Foundation +
# UniformTypeIdentifiers). NO link to aaaseed_code_utils -- preserves
# the c104 hermetic doctrine.
add_executable(aaaseed_ui_file_dialog_tests
    ui_file_dialog_test.cpp
)
target_link_libraries(aaaseed_ui_file_dialog_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_ui_file_dialog_mac
)
target_include_directories(aaaseed_ui_file_dialog_tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_features(aaaseed_ui_file_dialog_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_ui_file_dialog_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;ui;phase4"
)

# c135 / Phase 4 (todo L298) : aaaseed_ui_dpi tests. Exercises the
# pure-C++ surface of the hermetic aaaseed_ui_dpi_mac sub-lib
# (NSScreen-backed DPI adapter behind aaa::ui::dpi). Real NSScreen
# is queried under ctest -- the macOS test host has a live
# WindowServer -- so main_scale_factor / all_screens / main_screen
# return real values. Hermetic : links only the new sub-lib (which
# transitively brings AppKit + Foundation). NO link to
# aaaseed_code_utils -- preserves c104 hermetic doctrine. Pure C++
# test TU (no .mm) verifies the no-ObjC-in-header contract of
# aaa_dpi.h at compile time.
add_executable(aaaseed_ui_dpi_tests
    ui_dpi_test.cpp
)
target_link_libraries(aaaseed_ui_dpi_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_ui_dpi_mac
)
target_include_directories(aaaseed_ui_dpi_tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_features(aaaseed_ui_dpi_tests PRIVATE cxx_std_20)
#  Label ordering follows feedback_ctest_label_first_only.md : on this
#  toolchain gtest_discover_tests honors only the FIRST label. `phase4`
#  is the unique discriminator (per c132-B convention) so
#  `ctest -L phase4` cleanly selects the Phase 4 cohort.
gtest_discover_tests(aaaseed_ui_dpi_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "phase4;unit;ui"
)

# c136 / Phase 4 (todo L301) : aaaseed_ui_dialog tests. Exercises the
# pure-C++ surface of the hermetic aaaseed_ui_dialog_mac sub-lib
# (NSAlert + NSPanel impl behind aaa::ui::dialog::*). Under ctest there
# is no live NSApplication event loop, so the modal entry points
# (alert / confirm / prompt) return their Cancel / nullopt sentinels
# via the `has_live_event_loop` headless guard ; the panel lifecycle
# (make / show / hide / destroy) runs against real AppKit. Hermetic :
# links only the new sub-lib (which transitively brings AppKit +
# Foundation). NO link to aaaseed_code_utils -- preserves c104
# hermetic doctrine. Pure C++ test TU (no .mm) verifies the
# no-ObjC-in-header contract of aaa_dialog.h at compile time. Label
# ordering follows feedback_ctest_label_first_only.md : `phase4` is the
# unique discriminator (per c132-B / c135-A convention) so
# `ctest -L phase4` cleanly selects the Phase 4 cohort.
add_executable(aaaseed_ui_dialog_tests
    ui_dialog_test.cpp
)
target_link_libraries(aaaseed_ui_dialog_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_ui_dialog_mac
)
target_include_directories(aaaseed_ui_dialog_tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_features(aaaseed_ui_dialog_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_ui_dialog_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "phase4;unit;ui"
)

# c122 / Phase 6 reciprocity beachhead : aaaseed_syphon smoke. Verifies
# the aaa::syphon::ServerMac API signature compiles + ctor/publish_*()
# /dtor do not crash on an IOSurface-backed MTLTexture. No Mach-port
# advertisement is exercised (c123+ ; needs entitlements outside the
# unit-test sandbox). Links the new aaaseed_syphon hermetic sub-lib
# only ; no engine link, no o_str, no aaa_mem.
find_library(METAL_FRAMEWORK_SYPHON_TEST     Metal     REQUIRED)
find_library(FOUNDATION_FRAMEWORK_SYPHON_TEST Foundation REQUIRED)
find_library(IOSURFACE_FRAMEWORK_SYPHON_TEST IOSurface REQUIRED)
add_executable(aaaseed_syphon_mac_smoke_tests
    syphon_mac_smoke_test.mm
)
set_source_files_properties(syphon_mac_smoke_test.mm
    PROPERTIES
    LANGUAGE OBJCXX
    COMPILE_FLAGS "-fno-objc-arc"
)
target_link_libraries(aaaseed_syphon_mac_smoke_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_syphon
    "${METAL_FRAMEWORK_SYPHON_TEST}"
    "${FOUNDATION_FRAMEWORK_SYPHON_TEST}"
    "${IOSURFACE_FRAMEWORK_SYPHON_TEST}"
)
target_include_directories(aaaseed_syphon_mac_smoke_tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_features(aaaseed_syphon_mac_smoke_tests PRIVATE cxx_std_20)
#  c124-C : RESOURCE_LOCK so the DN-using PublishPostsDistributedNotification
#  test doesn't race the new client-side DN tests added below. The system
#  distnoted daemon drops deliveries under tight parallel post-load ;
#  tagging the whole target with the lock is safe (the 7 non-DN smoke
#  tests are sub-ms and serializing them costs nothing).
gtest_discover_tests(aaaseed_syphon_mac_smoke_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES
        LABELS "unit;syphon"
        RESOURCE_LOCK "aaaseed_syphon_dn_bus"
)

# c124 : Phase 6 reciprocity stack -- Lua-side C-binding into ServerMac.
# Verifies aaa::syphon::register_lua_bindings installs `aaa.syphon.send`
# as a Lua function and that calling it from a Lua script lazily creates
# the ServerMac in the process-local registry. The test does NOT exercise
# publish_metal_texture (already covered by syphon_mac_smoke ; would need
# a real MTLDevice + IOSurface).
add_executable(aaaseed_syphon_lua_mac_tests
    syphon_lua_mac_test.cpp
)
#  c124-D : ReceiveAfterServerPublish builds an IOSurface via plain
#  CoreFoundation primitives (no ObjC literals -- keeps this TU a
#  .cpp). Link CoreFoundation + IOSurface explicitly ; aaaseed_syphon
#  already brings them in transitively but being explicit avoids any
#  static-archive ordering surprises.
find_library(COREFOUNDATION_FRAMEWORK_SYPHON_LUA CoreFoundation REQUIRED)
find_library(IOSURFACE_FRAMEWORK_SYPHON_LUA      IOSurface      REQUIRED)
target_link_libraries(aaaseed_syphon_lua_mac_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_syphon
    aaaseed_lua
    "${COREFOUNDATION_FRAMEWORK_SYPHON_LUA}"
    "${IOSURFACE_FRAMEWORK_SYPHON_LUA}"
)
target_include_directories(aaaseed_syphon_lua_mac_tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_features(aaaseed_syphon_lua_mac_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_syphon_lua_mac_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES
        LABELS "unit;syphon_lua"
        #  c124-D : ReceiveAfterServerPublish posts on the same
        #  "aaa.syphon.frame" DN channel as c124-C client tests +
        #  c122/c123-A smoke. Share the RESOURCE_LOCK so ctest -j
        #  serializes all DN-using tests as a single resource group.
        RESOURCE_LOCK "aaaseed_syphon_dn_bus"
)

# c124-C : Phase 6 reciprocity LAST HOP -- ClientMac DN-discovery tests.
# Pairs with c122/c123-A syphon_mac_smoke ServerMac coverage. Critical
# integration test : ServerToClientSameProcess creates a ServerMac +
# ClientMac in the same process, publishes an IOSurface, polls the
# runloop, verifies the client recovers the surface via
# IOSurfaceLookup(surface_id) round-trip. Links the same hermetic
# aaaseed_syphon sub-lib that owns ServerMac + ClientMac ; no engine
# link.
find_library(METAL_FRAMEWORK_SYPHON_CLIENT_TEST     Metal      REQUIRED)
find_library(FOUNDATION_FRAMEWORK_SYPHON_CLIENT_TEST Foundation REQUIRED)
find_library(IOSURFACE_FRAMEWORK_SYPHON_CLIENT_TEST IOSurface  REQUIRED)
add_executable(aaaseed_syphon_client_mac_tests
    syphon_client_mac_test.mm
)
set_source_files_properties(syphon_client_mac_test.mm
    PROPERTIES
    LANGUAGE OBJCXX
    COMPILE_FLAGS "-fno-objc-arc"
)
target_link_libraries(aaaseed_syphon_client_mac_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_syphon
    "${METAL_FRAMEWORK_SYPHON_CLIENT_TEST}"
    "${FOUNDATION_FRAMEWORK_SYPHON_CLIENT_TEST}"
    "${IOSURFACE_FRAMEWORK_SYPHON_CLIENT_TEST}"
)
target_include_directories(aaaseed_syphon_client_mac_tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_features(aaaseed_syphon_client_mac_tests PRIVATE cxx_std_20)
#  c124-C : SyphonClientMac tests + the existing SyphonMacSmoke
#  PublishPostsDistributedNotification share the system-wide DN
#  daemon (distnoted). Empirically the daemon drops deliveries when
#  multiple processes post on the SAME notification name in tight
#  parallel ; ctest -j 4 with our client tests + the smoke
#  DN-publish test routinely failed. Tag both target groups with a
#  RESOURCE_LOCK so ctest serializes them while still parallelizing
#  the rest of the suite.
gtest_discover_tests(aaaseed_syphon_client_mac_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES
        LABELS "unit;syphon_client"
        RESOURCE_LOCK "aaaseed_syphon_dn_bus"
)
#  Note : the RESOURCE_LOCK above pairs with the same lock name
#  applied to aaaseed_syphon_mac_smoke_tests' PROPERTIES block
#  (see the c122/c123-A target above). ctest serializes the
#  syphon DN-using tests as a single resource group, letting the
#  rest of the suite parallelize normally.

# c127-A : Phase 6 reciprocity Mac IPC perf baseline -- Syphon publish ->
# DN -> consume round-trip. ServerMac + ClientMac live in the same
# process ; delivery flows through CFNotificationCenterGetLocalCenter()
# (c124-C extension) which is in-process + synchronous so the measured
# round-trip is the pure in-process IPC cost. Reuses 1280x720 BGRA8 to
# combine cleanly with c121-B's per-frame perf -- together they size
# the "render frame + push to peer" budget end-to-end. RESOURCE_LOCK
# shared with c122 / c124-C / c124-D so ctest -j serializes all
# DN-using tests as a single resource group.
find_library(METAL_FRAMEWORK_SYPHON_PERF_TEST     Metal      REQUIRED)
find_library(FOUNDATION_FRAMEWORK_SYPHON_PERF_TEST Foundation REQUIRED)
find_library(IOSURFACE_FRAMEWORK_SYPHON_PERF_TEST IOSurface  REQUIRED)
add_executable(aaaseed_syphon_round_trip_perf_tests
    syphon_round_trip_perf_test.mm
)
set_source_files_properties(syphon_round_trip_perf_test.mm
    PROPERTIES
    LANGUAGE OBJCXX
    COMPILE_FLAGS "-fno-objc-arc"
)
target_link_libraries(aaaseed_syphon_round_trip_perf_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_syphon
    "${METAL_FRAMEWORK_SYPHON_PERF_TEST}"
    "${FOUNDATION_FRAMEWORK_SYPHON_PERF_TEST}"
    "${IOSURFACE_FRAMEWORK_SYPHON_PERF_TEST}"
)
target_include_directories(aaaseed_syphon_round_trip_perf_tests PRIVATE "${CMAKE_SOURCE_DIR}")
target_compile_features(aaaseed_syphon_round_trip_perf_tests PRIVATE cxx_std_20)
#  c121-B doctrine : gtest_discover_tests honors FIRST label only on this
#  toolchain (CMake semicolon-stripping quirk). "perf" MUST be first so
#  `ctest -L perf` includes this test ; "unit" + "syphon_perf" remain in
#  the metadata for catalogue discovery. RESOURCE_LOCK shared with the
#  other DN-using syphon tests serializes all of them as one group.
gtest_discover_tests(aaaseed_syphon_round_trip_perf_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES
        LABELS "perf;unit;syphon_perf"
        RESOURCE_LOCK "aaaseed_syphon_dn_bus"
)

# Phase 8 distribution beachhead (c136-B, 2026-05-27, todo.md L365-L383).
# `aaa::macos::bundle` reads Info.plist via NSBundle mainBundle.
# Tests assert "non-empty AND not crashing" semantics : gtest binaries
# live outside AAASeed.app so the canonical plist keys are typically
# absent ; the API contract is defensive (never crash, sensible
# defaults), not "always returns com.seedexr.aaaseed". c121-B doctrine :
# `phase8` is FIRST so `ctest -L phase8` includes this test.
add_executable(aaaseed_macos_bundle_meta_tests
    macos_bundle_meta_test.cpp
)
target_link_libraries(aaaseed_macos_bundle_meta_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_bundle_meta
)
target_compile_features(aaaseed_macos_bundle_meta_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_macos_bundle_meta_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "phase8;unit;distribution"
)

# Phase 8 DMG packaging dry-run tests (c137-B, 2026-05-27, todo.md L379-L380).
# Validates cmake/dmg.cmake without invoking hdiutil ; locks the
# hdiutil-create + Applications-symlink + version-stamped DMG contract.
# Pure C++ -- no engine deps. `phase8` first per c121-B doctrine
# (CTest LABELS first-only quirk).
add_executable(aaaseed_dmg_packaging_tests
    dmg_packaging_test.cpp
)
target_link_libraries(aaaseed_dmg_packaging_tests PRIVATE
    GTest::gtest GTest::gtest_main
)
target_compile_features(aaaseed_dmg_packaging_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_dmg_packaging_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "phase8;unit;distribution;dmg"
)

# Phase 8 codesign + notarize dry-run tests (c138-B, 2026-05-27, todo.md L385-L387).
# Validates cmake/codesign.cmake env-gating contract without invoking
# codesign / notarytool / stapler. Locks command-line shape + env-var
# references. Tests 12 + 13 are regression guards per c137 doctrine
# (memory/feedback_regression_guard_tests.md) : test 12 asserts no
# hardcoded Developer ID leaks ; test 13 asserts CODESIGN_IDENTITY is
# unset at routine test-run time (passes either way + logs STATUS
# if set). Pure C++ -- no engine deps. `phase8` first per c121-B doctrine.
add_executable(aaaseed_codesign_packaging_tests
    codesign_packaging_test.cpp
)
target_link_libraries(aaaseed_codesign_packaging_tests PRIVATE
    GTest::gtest GTest::gtest_main
)
target_compile_features(aaaseed_codesign_packaging_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_codesign_packaging_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "phase8;unit;distribution;codesign"
)

# Phase 8 ship-script regression-guards (c143-B, 2026-05-27).
# Validates `scripts/ship-dmg.sh` (universal-binary ship pipeline)
# without executing it ; reads the script as text and asserts strict
# mode, lipo invocation, three compression-tier references (ULMO /
# ULFO / UDBZ), env-gated CODESIGN_IDENTITY + notarytool, no hardcoded
# Developer ID, hdiutil verify, lipo -info verification, and the
# universal-binary default ARCHES list ("arm64 x86_64"). Pure C++ ;
# no engine deps. `phase8` first per c121-B doctrine (CTest LABELS
# first-only quirk).
add_executable(aaaseed_ship_script_tests
    ship_script_test.cpp
)
target_link_libraries(aaaseed_ship_script_tests PRIVATE
    GTest::gtest GTest::gtest_main
)
target_compile_features(aaaseed_ship_script_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_ship_script_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "phase8;unit;distribution;ship-script"
)

# Phase 8 verify-dmg.sh regression-guards (c145, 2026-05-27).
# Validates `scripts/verify-dmg.sh` (post-receipt structural verifier
# shipped alongside SHIP_CHECKLIST.md) without executing it. Reads the
# script as text and asserts : exists + executable, strict mode,
# references to all critical structural artifacts (AAASeed.app,
# Applications, hello_world.lua, lipo -info, hdiutil verify,
# plutil -lint), AND a regression guard that no `codesign` reference
# leaks in (signature verification is a separate concern -- see
# memory/feedback_regression_guard_tests.md). Pure C++ ; no engine
# deps. `phase8` first per c121-B doctrine (CTest LABELS first-only
# quirk).
add_executable(aaaseed_verify_dmg_script_tests
    verify_dmg_script_test.cpp
)
target_link_libraries(aaaseed_verify_dmg_script_tests PRIVATE
    GTest::gtest GTest::gtest_main
)
target_compile_features(aaaseed_verify_dmg_script_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_verify_dmg_script_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "phase8;unit;distribution;verify-dmg"
)

# Phase 8 Release-mode-default regression-guard (c144-A, 2026-05-27).
# Asserts scripts/ship-dmg.sh honors BUILD_TYPE env (default Release),
# propagates it as -DCMAKE_BUILD_TYPE=, doesn't hardcode the literal
# Release flag, and keeps the LTO + dead-strip + -Os triplet wired in
# src/ui/macos/CMakeLists.txt. Pure C++ ; no engine deps. `phase8` first
# per c121-B doctrine (CTest LABELS first-only quirk).
add_executable(aaaseed_release_mode_default_tests
    release_mode_default_test.cpp
)
target_link_libraries(aaaseed_release_mode_default_tests PRIVATE
    GTest::gtest GTest::gtest_main
)
target_compile_features(aaaseed_release_mode_default_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_release_mode_default_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "phase8;unit;distribution;release-mode"
)

# Phase 8 Info.plist singularity regression-guard (2026-05-27).
# Collapses c136-B's two-plist arrangement (src/ui/macos/Info.plist.in
# + bundle/macos/Info.plist) into ONE canonical template at
# bundle/macos/Info.plist.in, configure_file()'d into the build dir
# and consumed via MACOSX_BUNDLE_INFO_PLIST. The regression-guard
# tests enforce the singularity stays singular (per
# memory/feedback_regression_guard_tests.md). Pure C++ ; no engine
# deps. `phase8` first per c121-B doctrine (CTest LABELS first-only).
add_executable(aaaseed_info_plist_singularity_tests
    info_plist_singularity_test.cpp
)
target_link_libraries(aaaseed_info_plist_singularity_tests PRIVATE
    GTest::gtest GTest::gtest_main
)
target_compile_features(aaaseed_info_plist_singularity_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_info_plist_singularity_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "phase8;unit;distribution;plist"
)

# Phase 8 `.deproj` cross-platform safety round-trip test
# (c142-C definitive gap inventory, 2026-05-27, project_v1_ship_gate.md
# Tier 1). Pins the FORMAT-level cross-platform safety contract :
# `.deproj` is XML 1.0 / UTF-8 / 7-bit-ASCII / no Win32 backslashes ;
# enum params persist by string label (CODE_STYLE.md +
# image/pixel_format.cpp:243). Pure C++ test TU -- no engine link, no
# o_str / aaa_str.h / aaa_mem.h pull-in (hermetic Mac sub-test per
# memory/feedback_hermetic_mac_sublibs.md). Header compile-clean for
# `infrastructure/seedfile.h` is regression-guarded transitively by the
# `aaaseed_aaalua_master_compile_tests` OBJECT-lib target -- we declare
# the dependency below so a future seedfile.h breakage cascades into a
# build failure rather than a silent skip. `phase8` first per c121-B
# doctrine (CTest LABELS first-only quirk).
add_executable(aaaseed_deproj_round_trip_tests
    deproj_round_trip_test.cpp
)
target_link_libraries(aaaseed_deproj_round_trip_tests PRIVATE
    GTest::gtest GTest::gtest_main
)
target_compile_features(aaaseed_deproj_round_trip_tests PRIVATE cxx_std_20)
target_compile_definitions(aaaseed_deproj_round_trip_tests PRIVATE
    "AAASEED_SOURCE_ROOT=\"${CMAKE_SOURCE_DIR}\""
)
#  Cross-target dependency : if seedfile.h ever stops parsing on
#  AppleClang, the master_compile OBJECT-lib build fails, which fails
#  this target via the explicit add_dependencies() below. This is the
#  build-system encoding of Test 1.
add_dependencies(aaaseed_deproj_round_trip_tests
    aaaseed_aaalua_master_compile_tests
)
gtest_discover_tests(aaaseed_deproj_round_trip_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "phase8;unit;regression;deproj"
)

# c145 (Task #30 honest closure, 2026-05-27) : portable BDD math
# helpers + abstract Renderer interface from src/bdd/. Pure C++ test
# TU (no engine link, no Metal, no Objective-C). Links the hermetic
# Mac sub-lib `aaaseed_bdd`. The vendor's `bdd_cell_draw.cpp` (527
# LOC) had ~120 LOC of pure-C++ math (branch_make_pos, alpha lerp,
# radius / grid / size-per-child) that ports cleanly ; the GL-bound
# remainder is the v2 Metal renderer (see src/bdd/aaa_bdd_renderer.h
# for the contract). `phase3` first per CTest LABELS first-only
# quirk (feedback_ctest_label_first_only.md).
add_executable(aaaseed_bdd_cell_draw_mac_tests
    aaa_bdd_cell_draw_mac_test.cpp
)
target_link_libraries(aaaseed_bdd_cell_draw_mac_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_bdd
)
target_include_directories(aaaseed_bdd_cell_draw_mac_tests PRIVATE
    "${CMAKE_SOURCE_DIR}"
)
target_compile_features(aaaseed_bdd_cell_draw_mac_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_bdd_cell_draw_mac_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "phase3;unit;bdd"
)

# Phase 3 GOL::Backend cross-platform regression guard (c126-c130 /
# Task #152, 2026-05-27). Mac-side prerequisite for the Win-machine
# WindowsBackend extraction : the abstract `GOL::Backend` interface in
# src/gol/gol_backend.h must stay free of Metal-cpp / Foundation /
# AppKit / GL leaks so the Win-side concrete impl can drop in. Per
# memory/feedback_regression_guard_tests.md (c137 codification) : the
# test scans the header text for forbidden symbols and surfaces the
# rationale in failure messages. Pure C++ ; no engine deps. `phase3`
# first per c121-B doctrine (CTest LABELS first-only quirk).
add_executable(aaaseed_gol_backend_cross_platform_tests
    gol_backend_cross_platform_test.cpp
)
target_link_libraries(aaaseed_gol_backend_cross_platform_tests PRIVATE
    GTest::gtest GTest::gtest_main
)
target_compile_features(aaaseed_gol_backend_cross_platform_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_gol_backend_cross_platform_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "phase3;unit;cross-platform;regression-guard"
)

# c145-D v1 MEU sample library + authoring guide regression-guard
# (2026-05-27). Asserts the 5 starter MEUs at
# bundle/macos/meu/Samples/ are well-formed (Lua syntax valid, no
# Windows-only constructs, aaa.* binding references present) + the
# authoring guide at docs/AUTHORING_MEUS_ON_MAC.md names every
# canonical aaa.* binding the runner installs. Pure C++ test TU ;
# hermetic Mac sub-test per memory/feedback_hermetic_mac_sublibs.md
# (no engine link, no Metal, no Objective-C, no o_str / aaa_mem).
# The `unit` label is FIRST per c121-B doctrine
# (memory/feedback_ctest_label_first_only.md ; the test filter key
# is `unit` for this batch, then `meu` + `authoring` for refinement).
add_executable(aaaseed_meu_samples_tests
    meu_samples_test.cpp
)
target_link_libraries(aaaseed_meu_samples_tests PRIVATE
    GTest::gtest GTest::gtest_main
)
target_compile_features(aaaseed_meu_samples_tests PRIVATE cxx_std_20)
target_compile_definitions(aaaseed_meu_samples_tests PRIVATE
    "AAASEED_SOURCE_ROOT=\"${CMAKE_SOURCE_DIR}\""
)
gtest_discover_tests(aaaseed_meu_samples_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;meu;authoring"
)

# c147 v2 Phase 1 : regression-guard for the canonical widgets demo MEU
# (bundle/macos/meu/Samples/v2_widgets/) + the new "aaa.ui.* widget API"
# section in docs/AUTHORING_MEUS_ON_MAC.md . Same hermetic pattern as
# aaaseed_meu_samples_tests above (pure C++, no engine link, no Metal,
# no Objective-C ; source root via compile-time AAASEED_SOURCE_ROOT).
# Labels per c121-B "first label only" doctrine
# (memory/feedback_ctest_label_first_only.md) : `unit` first, then
# meu;authoring;v2 for refinement.
add_executable(aaaseed_v2_widgets_sample_tests
    v2_widgets_sample_test.cpp
)
target_link_libraries(aaaseed_v2_widgets_sample_tests PRIVATE
    GTest::gtest GTest::gtest_main
)
target_compile_features(aaaseed_v2_widgets_sample_tests PRIVATE cxx_std_20)
target_compile_definitions(aaaseed_v2_widgets_sample_tests PRIVATE
    "AAASEED_SOURCE_ROOT=\"${CMAKE_SOURCE_DIR}\""
)
gtest_discover_tests(aaaseed_v2_widgets_sample_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;meu;authoring;v2"
)

# c148 v2 Phases 2-4 : regression-guard for the THREE new canonical
# sample MEUs (bundle/macos/meu/Samples/v2_color_picker/,
# v2_modal_textinput/, v2_collapse_reload/) + the new section-3
# subsections in docs/AUTHORING_MEUS_ON_MAC.md (3.6 hsv_color_picker,
# 3.7 modals + text input, 3.8 collapsing panels + hot reload). Same
# hermetic pattern as aaaseed_meu_samples_tests + aaaseed_v2_widgets_
# sample_tests (pure C++, no engine link, no Metal, no Objective-C ;
# source root via compile-time AAASEED_SOURCE_ROOT). Labels per c121-B
# "first label only" doctrine (memory/feedback_ctest_label_first_only.md) :
# `unit` first, then meu;authoring;v2 for refinement.
add_executable(aaaseed_v2_phase234_samples_tests
    v2_phase234_samples_test.cpp
)
target_link_libraries(aaaseed_v2_phase234_samples_tests PRIVATE
    GTest::gtest GTest::gtest_main
)
target_compile_features(aaaseed_v2_phase234_samples_tests PRIVATE cxx_std_20)
target_compile_definitions(aaaseed_v2_phase234_samples_tests PRIVATE
    "AAASEED_SOURCE_ROOT=\"${CMAKE_SOURCE_DIR}\""
)
gtest_discover_tests(aaaseed_v2_phase234_samples_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;meu;authoring;v2"
)

# c149-B v3 : regression-guard for the THREE new v3 sample MEUs
# (bundle/macos/meu/Samples/v3_drag_drop/, v3_preset_save_load/,
# v3_nested_panels/) + the new section 4 in docs/AUTHORING_MEUS_ON_MAC.md
# (aaa.io.* file I/O API) + the "v3 -- LANDED (c149)" subsection in
# memory/project_v1_ship_gate.md . Same hermetic pattern as
# aaaseed_v2_phase234_samples_tests (pure C++, no engine link, no Metal,
# no Objective-C ; source root via compile-time AAASEED_SOURCE_ROOT).
# Labels per c121-B "first label only" doctrine
# (memory/feedback_ctest_label_first_only.md) : `unit` first, then
# meu;authoring;v3 for refinement.
add_executable(aaaseed_v3_samples_tests
    v3_samples_test.cpp
)
target_link_libraries(aaaseed_v3_samples_tests PRIVATE
    GTest::gtest GTest::gtest_main
)
target_compile_features(aaaseed_v3_samples_tests PRIVATE cxx_std_20)
target_compile_definitions(aaaseed_v3_samples_tests PRIVATE
    "AAASEED_SOURCE_ROOT=\"${CMAKE_SOURCE_DIR}\""
)
gtest_discover_tests(aaaseed_v3_samples_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;meu;authoring;v3"
)

# c147-A : unit tests for the Mac-native immediate-mode widget system
# (src/ui/widgets/aaa_widgets_mac.{h,mm}). 10 tests cover ctor/dtor,
# begin/end-frame balance, panel chrome, slider clamp + drag, button
# click+release, color-well preset cycle, widget-count tracking, and
# empty-label robustness. The header is pure C++ ; the test TU stays
# .cpp (no ObjC types in header). MetalBackend is the canonical
# aaaseed_gol_metal link (no ObjC framework deps beyond Foundation
# which the widget sub-lib already PUBLIC-links).
add_executable(aaaseed_widgets_mac_tests
    aaa_widgets_mac_test.cpp
)
target_link_libraries(aaaseed_widgets_mac_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_ui_widgets_mac
    aaaseed_gol_metal
)
target_compile_features(aaaseed_widgets_mac_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_widgets_mac_tests
    DISCOVERY_TIMEOUT 30
    # feedback_ctest_label_first_only.md : first label is the selector
    # key. `unit` primary ; `ui` + `widgets` for cohort refinement.
    PROPERTIES LABELS "unit;ui;widgets"
)

# c148-A v2 Phases 2-4 unit tests : 14 cases covering HSV color picker,
# modal dialog, single-line text input, hot-reload button, collapsing
# panel. Same hermetic .cpp pattern as aaa_widgets_mac_test.cpp.
add_executable(aaaseed_widgets_v2_phase234_tests
    aaa_widgets_v2_phase234_test.cpp
)
target_link_libraries(aaaseed_widgets_v2_phase234_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_ui_widgets_mac
    aaaseed_gol_metal
)
target_compile_features(aaaseed_widgets_v2_phase234_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_widgets_v2_phase234_tests
    DISCOVERY_TIMEOUT 30
    # `unit` primary per feedback_ctest_label_first_only.md ; `v2`
    # cohort key for the Phases 2-4 widget batch.
    PROPERTIES LABELS "unit;ui;widgets;v2"
)

# c149-A v3 unit tests : drag-drop, file dialog guard, preset save/load,
# nested collapsing panels. Plain .cpp (header is pure C++) ; links the
# widget + runner + file-dialog sub-libs. The file-watcher tests live
# in the sibling aaaseed_file_watcher_tests target below.
add_executable(aaaseed_widgets_v3_tests
    aaa_widgets_v3_test.cpp
)
target_link_libraries(aaaseed_widgets_v3_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_ui_widgets_mac
    aaaseed_gol_metal
    aaaseed_meu_runner
    aaaseed_ui_file_dialog_mac
    aaaseed_meu_file_watcher
)
target_compile_features(aaaseed_widgets_v3_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_widgets_v3_tests
    DISCOVERY_TIMEOUT 30
    # feedback_ctest_label_first_only.md : `unit` primary ; `v3` cohort
    # key for the new authoring features.
    PROPERTIES LABELS "unit;ui;widgets;v3"
)

# c149-A v3 Feature 3 : FSEvents hot-reload watcher unit tests. Separate
# binary so the FileWatcher sub-lib can be regression-tested without
# pulling the full runner + widget stack.
add_executable(aaaseed_file_watcher_tests
    aaa_file_watcher_test.cpp
)
target_link_libraries(aaaseed_file_watcher_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_meu_file_watcher
)
target_compile_features(aaaseed_file_watcher_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_file_watcher_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;meu;file-watch"
)

# c150 v4 : WidgetSystem text_area + marked-text (IME composition) unit
# tests. c150-A agent stalled before writing these ; finished by the
# harness. Pattern matches aaaseed_widgets_v3_tests above.
add_executable(aaaseed_widgets_v4_tests
    aaa_widgets_v4_test.cpp
)
target_link_libraries(aaaseed_widgets_v4_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_ui_widgets_mac
    aaaseed_gol_metal
)
target_compile_features(aaaseed_widgets_v4_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_widgets_v4_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;ui;widgets;v4"
)

# c150 v4 : sample-MEU regression guards for Samples/v4_ime_text/ +
# Samples/v4_multiline/ + AUTHORING + ship-gate closure wording.
# Hermetic pattern matches aaaseed_meu_samples_tests above.
add_executable(aaaseed_v4_samples_tests
    v4_samples_test.cpp
)
target_link_libraries(aaaseed_v4_samples_tests PRIVATE
    GTest::gtest GTest::gtest_main
)
target_compile_features(aaaseed_v4_samples_tests PRIVATE cxx_std_20)
target_compile_definitions(aaaseed_v4_samples_tests PRIVATE
    "AAASEED_SOURCE_ROOT=\"${CMAKE_SOURCE_DIR}\""
)
gtest_discover_tests(aaaseed_v4_samples_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;meu;authoring;v4"
)

# c148 : ImGui Studio authoring surface unit tests.
# Pure C++ — no GPU, no NSApplication. Studio is exercised with null
# backend + runner (null-GPU degraded path must not crash).
add_executable(aaaseed_studio_tests
    aaa_studio_test.cpp
)
target_link_libraries(aaaseed_studio_tests PRIVATE
    GTest::gtest GTest::gtest_main
    aaaseed_ui_studio_mac
    # c151-B : Lua-binding tests open a local lua_State and call
    # Studio::install_lua_bindings on it. aaaseed_lua provides the
    # vendored 5.1 interpreter ; pulled in via aaaseed_ui_studio_mac
    # transitively but stated explicitly for self-documentation.
    aaaseed_lua
)
target_compile_features(aaaseed_studio_tests PRIVATE cxx_std_20)
gtest_discover_tests(aaaseed_studio_tests
    DISCOVERY_TIMEOUT 30
    PROPERTIES LABELS "unit;studio"
)
