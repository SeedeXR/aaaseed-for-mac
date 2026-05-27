# Code Utils — General Utilities

`code_utils/` is configured as an include directory in the project (headers included without path prefix).

## Memory (`aaa_mem.h/cpp`, `aaa_mem_win.h`, `memcpy.cpp`)

`mem` — static singleton for all allocations.
- Tracking: `counter_malloc`, `counter_free`, `counter_realloc`
- Integrity: `is_memory_ok()`, `is_heap_ok()`
- Aligned allocation: `malloc_aligned()`, `realloc_aligned()`, `free_aligned()`
- SIMD copy dispatch (runtime-selected): `b_cpy_use_movsb`, `b_cpy_use_avx2`, `b_cpy_use_sse41`, `b_memset_use_avx`
- Per-flag user toggle: `b_cpy_use_movsb_asked`, `b_cpy_use_avx2_asked`, `b_cpy_use_sse41_asked`, `b_memset_use_avx_asked` (exposed as `PARAM_DEF_BOOL_ON` in pref.cpp Memory group)
- Parallel copy: `b_cpy_parallel`, `cpy_parallel_thread_nb`, `cpy_parallel_threshold_kb` (default 1024 KB)
- Windows debug: `c_memory_leak_detector_wrapper`
- Verbose mode with size threshold
- **Non-temporal stores** (`_mm_stream_*`) require `_mm_sfence()` before the function returns — missing sfence makes stores visible out of order to other cores. All stream-based memcpy variants in `memcpy.cpp` must follow this rule.
- **`_msize` is UB on `_aligned_malloc` blocks** — use `_aligned_msize(ptr, alignment, 0)` if you need the size, or avoid it (alignment is not stored at free time in the current code).
- **Ownership of SIMD dispatch flags** — `mem` owns the resolved flags (`b_cpy_use_*`, `b_memset_use_avx`). `c_cpu` owns capability detection (`_b_*_has`) and user opt-out (`_b_*_allow`), and exposes resolved capability via `is_use_X()`. `mem::update()` reads `c_cpu::one->is_use_X()` and ANDs with its own `_asked` toggles. **Do NOT write `mem::b_cpy_use_*` or `mem::b_memset_use_avx` from anywhere except `mem::update()`.** A previous bug had `c_cpu::update()` writing `mem::b_memset_use_avx = _b_AVX_use` cross-module; this is fixed.

## Strings (`aaa_str.h/cpp`)

`o_str final` — heap-allocated string container.
- Block-aligned allocation: `(size + 16) & ~0xf` (16-byte blocks)
- Length and line-number tracking
- Instance counters: `get_nb_pt()` (live), `get_nb_created_pt()` (total)
- Static name stack: `push_name()` / `pop_name()` — avoids allocation for short temp names

## File I/O (`file/` subdirectory)

### `c_file final` (`aaa_file.h/cpp`)
Main file access singleton.
- Virtual filesystem: `open_vfile_save()`, `open_vfile_load()`, `close_vfile()`, `push_vfile()`, `pop_vfile()`
- Encoding detection: `TEXT_FILE_ENCODING` enum (UTF8, UTF16, BOM)
- Fast read mode: `b_read_fast`
- Query: `is_exist()`, `is_exist_real_file()`, `is_existing_dir()`, `get_mdate()`

### `c_file_virtual final` (`file_virtual.h/cpp`)
Virtual filesystem layer — transparent abstraction over real files.
`st_vfile` holds a data pointer. Must be active via `push_vfile()` / `pop_vfile()`.

### `c_file_io final` (`file_io.h/cpp`)
Binary and text read/write with optional async support.
- Async: `c_thread_files`, `c_files_async` for threaded file operations

### `fname final` (`fname.h/cpp`)
Filename parsing: extension, directory, base name components.
`is_to_be_ignored()` — filters out system/temp files.

### Directory (`aaa_dir.h`, `dirparser.h`, `dir_pool.h`)
- `c_dir final` — directory listing and recursive scanning
- `c_dir_parser final` — directory traversal with filtering
- `c_dir_pool final` (`c_obj_ui`) — cached directory pool

### Other file utilities
- `file_csv.h/cpp` — CSV parsing
- `file_xls.h/cpp` — Excel file support
- `asc_parser.h/cpp` — ASCII file parsing base (`c_asc_parser`, `c_asc_parser_cstring`)

## Time (`time/` subdirectory)

- `c_delta_t final` — delta time between frames
- `c_speed final` / `c_speed_ui final` — velocity with optional UI binding
- `c_speed_master final` (`c_obj_active_ui`) — global speed manager
- `c_aaa_time` — time utilities
- `c_aaa_date` — date utilities

## Time Buffer (`time_buf.h/cpp`, `time_buf_master.h/cpp`)

- `c_tdata final` — single time-buffered data element
- `c_tbuf final` (`c_obj`) — time-indexed buffer container
- `c_tbuf_master final` (`c_obj_ui`) — global time buffer manager

## Threading (`thread/` subdirectory)

- `c_thread` (`aaa_thread.h/cpp`) — thread wrapper
- `c_priority final` (`scheduler.h`) — task priority queue (`c_obj_ui`)
- `c_scheduler` — task scheduling with priority

## Data Structures

- `c_data_2d<T> final` (`data_2d.h/cpp`) — sparse 2D array template (`c_obj`)
- `c_buffer_blk` / `c_buffer_stream final` (`buffer/buffer_blk.h/cpp`) — block-based stream

## Error Handling (`err.h/cpp`)

Return codes: `AAA_ERR` (`AAA_OK`, `ERR_ANY`, `ERR_FILE_NO`, …). No exceptions.
`SVaPassNext` / `CVaPassNext` — variable-argument pass-through helpers.

**`lock_mess` is `MUTEX_RECURSIVE`** — `err_print` holds it across two consecutive print calls,
and some print paths call back into `ERR_PRINT_STRING` while the lock is held. Do not change it to
`MUTEX` (non-recursive) — that will deadlock.

## Numeric / String Utilities

- `aaa_util.h/cpp` — `CLAMP`, `SWAP`, `MIN`, `MAX` and 100+ templated numeric helpers. **`OVER_ONE_AS_REAL(v) = 1/v`** (with `_AS_DOUBLE` / `_AS_FP32` siblings) is the project's reciprocal helper, returns `BIGGEST<T>` on `v == 0`. Variables holding the precomputed reciprocal use the **`_over` or `_over_one` suffix** (e.g. `comment_scale_x_over`, `_life_over_one`, `norm_over_one`, `_repulse_dist_over_one`). The suffix means "1 over X", **not** "hover" or "overflow".
- `strnum.h/cpp` — string ↔ number conversion
- `fast_atof.h` — fast ASCII-to-float (vendored from Irrlicht/Assimp; not currently included anywhere in Src, kept for reuse)
- `asc_line.h/cpp` — thin wrappers around `fgets` / `fgetws` for line-by-line text reads. `get_next_line(FILE*, dst, len)` returns 1 (non-empty line read), 0 (blank line `\n` only), -1 (EOF). `get_next_line_no_empty(...)` skips blank lines but does NOT trim leading whitespace from the returned line — callers that need trimmed input must skip leading spaces themselves or use `sscanf`-style parsing. `count_str_until_control` (renamed from `count_str_til_control`) counts whitespace-separated tokens up to the next control char.

## Other Utilities

| File | Purpose |
|------|---------|
| `CPU.h/cpp` (`c_cpu final`, `c_obj_ui`) | CPU capability detection (SIMD flags) |
| `cpu_intrinsics.h` | Cross-compiler wrappers for x86 CPUID and XGETBV (`aaa_cpuid`, `aaa_cpuidex`, `aaa_xgetbv`). MSVC branch via `<intrin.h>`, GCC/Clang branch via `<cpuid.h>` + `<immintrin.h>`. Only consumer in Src is `CPU.cpp` |
| `checksum.h/cpp` | `c_checksum` computation |
| `md5.h/cpp` + `md5c.cpp` | MD5 hashing |
| `aaa_sql.h/cpp` | SQL database access |
| `clipboard.h/cpp` | `c_clipboard` clipboard read/write |
| `os_version.h/cpp` | OS version detection (`RtlGetVersion` via `GetProcAddress` on ntdll, with `b_win11/b_win10/b_win10_or_more/b_win8/b_win8_or_more/b_win7/b_win7_or_more/b_vista/b_xp` flags) |
| `dll_version.h/cpp` | Module version info (`c_module_details`) |
| `spy.h/cpp` + `spy_lua.h/cpp` | Debug inspection + Lua binding |
| `watchdog.h/cpp` (`c_watchdog final`) | Watchdog timer (`c_obj`) |
| `id_unique.h/cpp` | Unique ID generation |
| `console.h/cpp` | Console output |

## CPU (`CPU.h/cpp`)

`c_cpu final` (`c_obj_ui`) — singleton (`c_cpu::one`) detecting CPU vendor, brand, cores, cache hierarchy, SIMD feature flags. The detection runs once in the constructor via the inner `c_internal` class, which calls the cross-compiler wrappers `aaa_cpuid` / `aaa_cpuidex` / `aaa_xgetbv` defined in `cpu_intrinsics.h` (MSVC `<intrin.h>` on Windows, `<cpuid.h>` + `<immintrin.h>` on Clang/GCC).

Key invariants (post-construction):
- `c_cpu::one->_internal._cache_line_size > 0` — guaranteed at least `CACHE_LINE_SIZE_DEFAULT = 64`. CPUID-returned 0 is filtered to 64. `c_cpu::get_align_def()` is a hot-path fast accessor that can rely on this without a runtime guard.
- All `_b_*_has` / `_b_*_allow` / `_b_*_use` flags are initialized in the constructor's init list (false) before `init()` runs. `param_init_with()` sets the `_allow` defaults via PARAM_DEF_BOOL_ON; `init()` then runs `_internal.init()` to populate `_has`, then `update()` computes `_use = _has && _allow`.
- AVX/AVX2/AVX512 OS support (XCR0 mask bits 0x6 and 0xe6) is read once via a single `aaa_xgetbv` call inside `init()`. The standalone `detect_os_AVX()` and `detect_os_AVX512()` member functions were removed during the CPU audit because they were called multiple times per init.
- `c_internal::_data` and `_extdata` are `std::vector<std::array<int, 4>>` populated by `aaa_cpuid` / `aaa_cpuidex`. The CPUID output is always treated as **unsigned 32-bit** when shifted: extracting stepping/model/family does `UINT32 const eax = UINT32(_data[1][0]); _nFamily = (eax >> 8) & 0xf;` — never right-shift the signed `int` directly.

## VR / Sensor Support (`aaavr.h/cpp`)

- `c_sensor_6dof` — 6-DOF sensor interface
- `c_aaavr final` (`c_tracker`) — VR tracker wrapper
- `aaavr_vector.h` / `aaavr_matrix.h` — simple `Vector2/3/4`, `Matrix2/3/4` structs for VR math
