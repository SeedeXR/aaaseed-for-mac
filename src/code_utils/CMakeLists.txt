# src/code_utils/CMakeLists.txt
#
# Stage 1 of the code_utils port : compile the 10 files the Explore-agent
# survey flagged as platform-clean (no Win32, no x86 intrinsics, no
# cross-subsystem deps on infrastructure/, gol/, math/v.h, language/lua/).
#
# Subsequent batches will land via separate add_subdirectory passes :
#   Batch 2 — cpu_intrinsics-driven (CPU.cpp ; needs the dual-branch header).
#   Batch 3 — cross-subsystem (err.cpp, memcpy.cpp, time_buf.cpp, aaavr.cpp ;
#             need infrastructure/ or language/lua/).
#   Batch 4 — POSIX rewrites (console.cpp, os_version.cpp, aaa_str.cpp wide-char
#             paths, time/aaa_time.cpp QueryPerformanceCounter,
#             file/aaa_dir.cpp + file/aaa_file.cpp + file/file_io.cpp).
#
# Static lib target : aaaseed_code_utils.

add_library(aaaseed_code_utils STATIC
    "${AAASEED_ENGINE_SRC}/code_utils/aaa_util.cpp"
    "${AAASEED_ENGINE_SRC}/code_utils/aaa_str.cpp"
    "${AAASEED_ENGINE_SRC}/code_utils/md5c.cpp"
    "${AAASEED_ENGINE_SRC}/code_utils/checksum.cpp"
    "${AAASEED_ENGINE_SRC}/code_utils/strnum.cpp"
    "${AAASEED_ENGINE_SRC}/code_utils/stream_util.cpp"
    "${AAASEED_ENGINE_SRC}/code_utils/data_2d.cpp"
    "${AAASEED_ENGINE_SRC}/code_utils/asc_line.cpp"
    "${AAASEED_ENGINE_SRC}/code_utils/id_unique.cpp"
    "${AAASEED_ENGINE_SRC}/code_utils/buffer/buffer_blk.cpp"
    "${AAASEED_ENGINE_SRC}/code_utils/aaavr_matrix.cpp"
    # Batch 4 — POSIX rewrites landed 2026-05-26 (continuation 24).
    "${AAASEED_ENGINE_SRC}/code_utils/file/aaa_dir.cpp"
    "${AAASEED_ENGINE_SRC}/code_utils/file/aaa_file.cpp"
    "${AAASEED_ENGINE_SRC}/code_utils/file/fname.cpp"        # c64 -- pulled in by aaa_str.cpp's set_fname_pure
    "${AAASEED_ENGINE_SRC}/code_utils/str_utils.cpp"         # c66 -- pulled in by aaa_str.cpp's set_dir_name + fname.cpp::get_fname
    #  spy.cpp not in the lib : pulls tbuf::init/deinit/update from
    #  time_buf_master.cpp which isn't ported yet. spy::sleep is
    #  stubbed in test_engine_stubs_no_ostr.cpp instead (c65).
    "${AAASEED_ENGINE_SRC}/code_utils/os_version.cpp"
    "${AAASEED_ENGINE_SRC}/code_utils/time/aaa_time.cpp"
    "${AAASEED_ENGINE_SRC}/code_utils/console.cpp"
    # aaa_mem.cpp landed continuation 27 — both blockers from continuation
    # 26 fixed : (a) scheduler.h gates HANDLE-returning decls with
    # `#if AAA_OS_WINDOWS()`, (b) aaa_file.h has a defensive
    # `#undef FREAD / FWRITE` inside `#if AAA_OS_MAC()` so the fcntl.h
    # POSIX flag-bit macros never collide with c_file::FREAD / FWRITE
    # method names.
    "${AAASEED_ENGINE_SRC}/code_utils/aaa_mem.cpp"
    # c115 (2026-05-26) — Mac port of memcpy.cpp via a hermetic sub-lib.
    # Forwards mem::memmove / mem::memcpy / mem::memcpy_monothread to
    # libc (Apple's _platform_memmove is NEON-tuned ; we won't beat it),
    # provides real out-of-line defs for the 14 dispatch data symbols
    # the engine references (b_cpy_use_*, cpy_use_cache_*_size,
    # b_cpy_parallel, cpy_parallel_thread_nb, cpy_parallel_threshold_kb,
    # b_memset_use_avx*). Supersedes the c109 weak stubs in
    # tests/unit/aaalua_mem_dispatch_stub.cpp at link time. memcpy_parallel
    # is a forwarder to memcpy_monothread until c117 (compute_parallel
    # port) lands.
    "${CMAKE_SOURCE_DIR}/src/code_utils/memcpy_mac.cpp"
    # err.cpp deferred : depends on infrastructure/layer/, ui/aaa_menu.h,
    # ui/dialog_dev.h which haven't been Mac-ported yet. The Mac port of
    # err.cpp itself (POSIX log_with_type / fprintf path) is done, but the
    # transitive header chain needs more subsystems to land first.
    # ────────────────────────────────────────────────────────────────────
    # Deferred to a later batch :
    #
    # bdd_util.cpp        — transitively includes draw/aaa_gl.h → <GL/glu.h>.
    #                        Lands once GOL backend is abstracted.
    # ────────────────────────────────────────────────────────────────────
)

aaa_apply_engine_compile_flags(aaaseed_code_utils)

target_include_directories(aaaseed_code_utils PUBLIC
    "${AAASEED_ENGINE_SRC}"
    "${AAASEED_ENGINE_SRC}/code_utils"
    "${AAASEED_ENGINE_SRC}/system"
    # c117 : memcpy_mac.cpp includes "infrastructure/compute_parallel_mac.h"
    # for the parallel_for symbol used in mem::memcpy_parallel.
    "${CMAKE_SOURCE_DIR}/src"
)

# c117 : mem::memcpy_parallel calls aaa::parallel::parallel_for from
# aaaseed_infrastructure_mac (GCD dispatch_apply impl). Link as PUBLIC
# so anyone linking aaaseed_code_utils gets the parallel symbol too.
target_link_libraries(aaaseed_code_utils PUBLIC
    aaaseed_infrastructure_mac
)

set_target_properties(aaaseed_code_utils PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
