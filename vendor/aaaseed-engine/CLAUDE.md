# AAASeed_EXE — Build Repo for the AAASeed C++ Engine

## What is AAASeed

AAASeed is a real-time C++/Lua/OpenGL visual engine developed by Mâa since 1996, running on Windows (2026 May: Linux and Macintosh versions will come at some point). The C++ engine is an old school rendering graph exposing scene objects (cameras, lights, materials, geometry databases, textures, etc.) as attributes in a layer/layers/module/modules/app structure. Each object (c_obj_ui) has named parameters (c_param) that are like a remote control for it. Through params, objects are changed in the UI, loaded/saved, and can be driven from Lua scripts. A user-facing UI layer (GaBuZoMeu, "GaBu") is fully written in Lua on top of the C/Lua bridge.

Two repos cooperate:

| Repo | Purpose |
|------|---------|
| **AAASeed_EXE** (this repo, build) | C++ source, MSBuild project, produces the EXE. Contains `Src/*_lua.cpp` files that register C functions as Lua bindings. |
| **AAASeed** (sibling, runtime) | Kernel Lua scripts (`AAAKernel/lua/`), GaBu UI framework, user APPs, MEUs, and end-user API documentation (`AAADoc/lua_aaaseed_*.lua` + `.md`). The EXE loads from here at boot. The sibling has its own `CLAUDE.md` at the root and per important folder. |

The two repos are independent (not git submodules); cross-references via `CLAUDE.md` files are the only formal link. Their actual filesystem locations are environment-specific -- ask the project owner when cloning fresh.

When debugging a runtime behavior, you typically need both: the C side defines what's registered, the Lua side defines how it's used. The doc files in the sibling's `AAADoc/` describe the merged surface from a script author's point of view.

## Engine architecture in one paragraph

Every engine object inherits `c_obj_ui` and exposes : **params** (typed control / output cells, the public interface), `update()` (refresh internal state for the next frame), `draw()` (execute as fast as possible, may be called multiple times per frame ; `draw()` is almost a misnomer because a `c_obj_ui` may do something other than rendering, `execute()` would be more accurate). The render graph then has a **fixed depth of four** :

- **`c_layer`** : one `c_bdd` (the actual drawer, see `Src/obj_ui/bdd/`) plus a full attribute system (color, render state, fog, mapping, shading, lights switch, transforms, ..., each in current / owner / none mode).
- **`c_layers`** : a sequence of up to 26 `c_layer` (`a..z`), plus a Lua wrapper, cameras, a values block, and a traxs collection (the older animation-driver mechanism).
- **`c_module`** : a group of `c_layers`, adding lights, optional materials, multi-screen, plus its own lua / cameras / values / traxs.
- **`c_modules`** : top-of-engine, a group of `c_module` with its own lua / values / traxs. Singleton-like via `c_modules::main`.

Above the four, `c_app` (singleton `g_app`) holds the modules and the global rendering settings. Above `c_modules`, the structural depth is **handled by the application-side scripting layer** (the sibling repo's GaBuZoMeu MEUs) : the engine is static-depth for performance, the Lua side is arbitrary-depth for composition. Full version : `doc/AAASeed_render_graph.md`.

## Project Overview
AAASeed is a C++/Lua real time OpenGL application.
`Src` contains all C++ source code, split into subsystems — each has its own `CLAUDE.md`:
- `Src/gol/` — OpenGL isolation layer (GOL namespace) → `Src/gol/CLAUDE.md`
- `Src/math/` — math utilities → `Src/math/CLAUDE.md`
- `Src/draw/` — cameras, lights, materials, geometry, textures, picking, fog, etc. → `Src/draw/CLAUDE.md`
- `Src/image/` — image loading, conversion, pixel formats, GPU upload → `Src/image/CLAUDE.md`
- `Src/infrastructure/` — object model, factory, param, params, layer, layers, modules, trax, data → `Src/infrastructure/CLAUDE.md`
- `Src/code_utils/` — general utilities (memory, strings, files, threading, time) → `Src/code_utils/CLAUDE.md`
- `Src/aaa/` — threading primitives (mutex, lock guard, spinlock) → `Src/aaa/CLAUDE.md`
- `Src/ui/` — Windows UI layer: events, dialogs, menus, font, preferences → `Src/ui/CLAUDE.md`
- `Src/language/lua/` — Lua binding layer (state wrapper, helpers, registration macros) → `Src/language/lua/CLAUDE.md`

Platform: Windows / MSVC / GLEW / WGL.

**English-only rule** — every source file (C/C++, Lua), doc file (`*_lua.lua` API docs in the sibling repo's `AAADoc/`), and `CLAUDE.md` is written in English. The conversational chat with the project owner can be in French; the artifacts cannot. Reason: portability for tooling and any future collaborator.

**Sibling repo for runtime content** — the runtime content (Lua scripts, kernel docs, MEU APPs, generated docs) lives in a separate git repo `AAASeed` (sibling). See its root `CLAUDE.md` for layout.

Notable subfolders in the sibling:
- `AAAKernel/lua/aaa_*.lua` — pure-Lua API extensions that mirror and complete the `*_lua.cpp` C bindings. See sibling's `AAAKernel/lua/CLAUDE.md`.
- `AAADoc/lua_aaaseed_*.lua` — public API reference doc (source of truth, .md is generated). When you add or rename a C binding here in `*_lua.cpp`, update the corresponding entry in `AAADoc/lua_aaaseed_*.lua` over there. See sibling's `AAADoc/CLAUDE.md` for the section convention (Option C: `-- # ` / `-- ## ` / `-- ### `) and the `tools/regen_all.sh` pipeline that rebuilds the `.md` view.
- `AAAKernel/Dev/AAAUser/Guest/default.lua_master.lua` — definitive list of Lua files loaded at runtime; useful when auditing the live API surface.

## Build & Platform
- Windows 11, MSVC compiler.
- **Two coexisting build systems** during the CMake migration:
  - `AAASeed_by_Maa.sln` + `AAASeed_by_Maa.vcxproj` — historical MSBuild project. Still authoritative for now.
  - `CMakeLists.txt` (root) + `cmake/*.cmake` + `Src/<subsys>/sources.cmake` + `CMakePresets.json` — CMake build, builds the same `AAASeed_<Config>.exe` on Windows. Cross-platform-ready (Mac/Linux additions deferred to Louis). See the dedicated section below.
- OpenGL via GLEW (static: `glew32s`, dynamic: `glew32`).
- Links: `opengl32`, `glu32`.
- **Windows API floor : not enforced in the actual day-to-day v145 configs**. `Src/aaa_os.h` documents the floor as Windows 7 (`_WIN32_WINNT=0x0601`) and that's set in the legacy unsuffixed vcxproj configs (`Debug|x64`, `Release|x64`) but those are not what Maa actually builds. The four v145 configs (`Debug_v145|x64`, `Metal_v145|x64`, `Release_v145|x64`, `Wood_v145|x64`) do NOT define `_WIN32_WINNT` — they let the Windows SDK default apply (currently Windows 10+). The CMake build mirrors the v145 configs and likewise does not enforce a floor. Code in `touch_windows.h` (`ORIENTATION_PREFERENCE`) and `display_info.cpp` (`GetDpiForMonitor`) calls Win8.1+ APIs directly without dynamic loading and would fail to compile under a Win7 floor — that's a pre-existing inconsistency between the documented `aaa_os.h` policy and the actual code, not a regression introduced here. If a Win7-compatible build is ever needed, those direct calls must be guarded by `GetProcAddress`-based dynamic loading (pattern from `Src/code_utils/os_version.cpp` for `RtlGetVersion`).
- **SDK version display** (`sdk_version` in the pref panel) comes from `Src/license/aaa_sdk_version.generated.h`. The vcxproj produces it via the `GenerateAaaSdkVersionHeader` MSBuild Target at the end of the file (uses `$(TargetPlatformVersion)`, NOT `$(WindowsTargetPlatformVersion)` which stays as the raw request `"10.0"`). The CMake build produces it via `configure_file` from `Src/license/aaa_sdk_version.generated.h.in` (uses `${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}`). Both paths target the same generated header (gitignored). Dead ends (`<ntverp.h>`, per-config `\"` escapes) documented in memory.

### Configurations

Four configs shared between vcxproj and CMake. Toolset (`v143` or `v145`) is orthogonal : the vcxproj has 4 configs * 2 toolsets * 2 platforms (Win32 / x64) = 16 entries ; the CMake build collapses these to 4 named configs and exposes the toolset via the `AAA_VSTOOL` cache var.

| Config | For | Optim | Debug info | Edit & Continue | Instruction set |
|---|---|---|---|---|---|
| `Debug` | dev iteration | `/Od` (none) | `/ZI` (full + E&C) | yes | AVX2 |
| `Release` | dev "fast debug" | `/O2 /Ob3` (no LTCG) | `/Zi` + linker `/DEBUG` (PDB) | no | AVX2 |
| `Metal` | end-user release | `/O2 /Ob3 /GL` + LTCG | none | no | AVX2 |
| `Wood` | end-user release on non-AVX2 hardware | `/O2 /Ob3 /GL` + LTCG | none | no | SSE2 (x64 default) |

- **Debug** : the iteration workflow. No optimisation, full debug info, Edit & Continue active (modify code while the app runs, the compiler patches in place). Slow at runtime, instant feedback loop.
- **Release** : "Debug but faster, at the price of Edit & Continue". Same `/O2 /Ob3` baseline as Metal, but no `/GL` / `/LTCG` so the link finishes in seconds rather than minutes. Ships a PDB (`/Zi` compile + `/DEBUG` link) so VS can F5-launch and step through with readable stacks at full speed. Used when a bug only reproduces above Debug speed but the LTCG link cost of Metal is unacceptable for iteration.
- **Metal** : production binary for end users. Max optim + AVX2 + LTCG. No debug info for the smallest binary. The default user-facing build on modern machines.
- **Wood** : production binary for end users on hardware without AVX2. Identical to Metal except the instruction set : no `/arch:AVX2`, so MSVC defaults to SSE2 on x64. Compatibility build for older CPUs / VMs.

**AVX2 startup check** : Debug / Metal / Release run a CPUID-based feature check as a CRT C-initializer (`.CRT$XIB`) BEFORE every C++ global ctor ; on a non-AVX2 CPU the user gets a MessageBox naming the build config (via `AAA_BUILD_CONFIG_NAME`) and pointing at AAASeed_Wood.exe, then `ExitProcess(1)`. Wood skips the check (`AAA_BUILD_REQUIRES_AVX2()` evaluates to 0). Implementation in `Src/aaa_check_cpu.{h,cpp}` ; the .cpp is force-compiled with `/arch:SSE2` via `set_source_files_properties` in `Src/sources_root.cmake` so the check itself can never emit the very instructions it is meant to detect.

### Maa's optimisation choices for Metal / Release / Wood

The Metal/Release/Wood compile flags reflect a deliberate **performance over binary size** stance, suited to a real-time OpenGL engine :

- **`/Ob3`** (most aggressive inline policy) : authorises inlining of moderately large functions. Multiplies the optimiser's chances to constant-fold and dead-code-eliminate after inlining.
- **`/Oi`** (intrinsic functions) : `memcpy`, `memset`, `strlen`, ... are emitted as inlined SSE intrinsics, no library call.
- **`/Ot` `/Oy`** (favour speed, omit frame pointer) : speed over size ; one extra free register on x64 because no slot is reserved for `rbp`.
- **`/GL` + `/LTCG`** (whole-program optimisation, **Metal / Wood only**) : the compiler emits `.obj` in intermediate language, the linker does the final code-gen with a view of the whole program (cross-TU inline, devirtualisation, ...). Cost : link in minutes instead of seconds. Gain : typically +10 to +20 % runtime perf. Release deliberately drops `/GL` / `/LTCG` so the iteration cycle stays fast ; the rest of the optim flags are identical.
- **`/Gy-`** (function-level linking OFF, **explicit**) : preserves i-cache locality. Without `/Gy`, functions from the same `.cpp` stay grouped contiguously in the binary ; calling `f1()` then `f2()` from the same file lands on adjacent cache lines. With `/Gy` ON, the linker can reorder and `/OPT:ICF` can fold two binary-identical functions to a single address, which mixes branch-predictor histories between distinct call sites. The ~10 % size gain from `/Gy` is not worth the potential hot-loop penalty.
- **`/OPT:ICF`** (Identical COMDAT Folding) : enabled but with `/Gy-` it only folds template instantiations and inlines (always in COMDAT by ODR rule). Marginal gain on this codebase, but kept for symmetry between vcxproj and CMake builds.
- **`/fp:fast`** (fast floating-point model) : the compiler may reorder / regroup FP operations, assume no NaN / no Inf. Can break explicit NaN tests but is the default everywhere in render / physics for direct gain.
- **`/Zp16`** (struct member alignment 16) : aligns struct members on 16 bytes, mandatory for the SIMD types (`__m128`, `__m256`) used heavily in the AAASeed math layer.
- **`/GS-`** (no buffer security check) : no stack canary to detect buffer overruns. AAASeed trusts its own code (no untrusted input on the runtime path) and saves a few cycles per function.
- **`/Qpar`** (parallel codegen) : the compiler attempts auto-vectorisation of loops. Variable effect, sometimes positive on math hot loops.
- **`/MP` `/Zm300`** : multi-process compile (one `cl.exe` per logical core), with the heap allocation factor set to 300% of default. Applied to all four configs (including Debug) so iteration builds are also parallel. `/Zm300` is enough headroom for the heaviest template TUs (dlib, GLM) without the cost of `/Zm1000`.

Cumulative effect : Metal binary lands around 24 MB, runs at full speed. Debug binary is ~46 MB and the dev iteration cycle stays under a second thanks to E&C.

**MSVC C1001 history** : an earlier flag set (incomplete, not exactly matching the vcxproj) triggered an internal compiler error on `Src/obj_ui/bdd/bdd_edit/bdd_curve3d.cpp:2018` during the LTCG code-gen pass on the v145 toolset. The full set above (`/Ob3 /Zp16 /fp:fast /GS- /Qpar /MP`) avoids the ICE path. If you ever simplify the flag set, rebuild Metal end-to-end first to confirm the ICE is gone (worth re-checking on every MSVC upgrade). Release config sidesteps the ICE entirely by dropping `/GL` (the ICE only fires during the LTCG code-gen pass).

### CMake build (Windows-only at the moment, Mac/Linux will follow)

Single executable target `AAASeed`, mirrors the four configs of the vcxproj : `Debug`, `Metal`, `Release`, `Wood`. The toolset is selected via the `AAA_VSTOOL` cache var (default `145` for VS 2026 Insiders, alternative `143` for VS 2022).

Quick start :
```bash
# Default (VS 2026 / v145), Metal config :
cmake --preset vs2026-v145
cmake --build --preset metal-v145

# Visual Studio 2022 / v143 fallback :
cmake --preset vs2022-v143
cmake --build --preset metal-v143
```
The full preset list lives in `CMakePresets.json`.

Launch under Visual Studio :
1. Set the env var `AAASeed_DirStart` to the runtime folder path (the dir AAASeed treats as its working directory). It must exist BEFORE the CMake configure step, otherwise the `<LocalDebuggerWorkingDirectory>` ends up empty in the generated `.vcxproj` and F5 falls back to the build dir.
2. From a terminal : `cmake --preset vs2026-v145` (or `vs2022-v143`). This generates the solution file under `out/cmake-build-<toolset>/`. **Note** : VS 2026 / v145 emits `AAASeed.slnx` (the new XML-based solution format) ; VS 2022 / v143 emits a classic `AAASeed.sln`. Both behave the same in VS.
3. Open that solution file in VS, set `AAASeed` as startup project, hit F5. The four configs (Debug / Metal / Release / Wood) are selectable from the dropdown ; Debug supports Edit & Continue (`/ZI`), Release also has a PDB and is debuggable at full speed.
4. Each new toolset gets its own build dir : v145 lands at `out/cmake-build-v145/`, v143 at `out/cmake-build-v143/`.

Open Folder mode (Open Folder on the repo root, with `enableCMakePresetsIntegration: true` in `CMakeWorkspaceSettings.json`) also works for editing + single-config builds, but does NOT expose the Batch Build dialog -- use the solution-file flow above when you need to build all four configs in one go.

Files :
- `CMakeLists.txt` (root) — project, configs, target declaration, source enumeration via `include(...sources.cmake)`.
- `cmake/defines.cmake` — `<PreprocessorDefinitions>` reproduction (platform Windows, third-party library config, per-config NDEBUG/DEBUG, ...).
- `cmake/compiler_flags.cmake` — per-config `/O2 /Zi /MD /arch:*` and the C++ standard via `/std:c++latest`.
- `cmake/include_dirs.cmake` — `<IncludePath>` reproduction (`Include/`, `Src/`, `Src/code_utils/`, OpenCV, KinectSDK, freetype2, etc.).
- `cmake/link_options.cmake` — link search paths (`${CMAKE_SOURCE_DIR}` so `./lib_x64/...` from `AAA_LIB_USE_MESSAGE` resolves) and `/NODEFAULTLIB:libc...` + `/LARGEADDRESSAWARE`.
- `Src/<subsys>/sources.cmake` — per-subsystem `target_sources(AAASeed PRIVATE ...)`. 37 files, 766 .cpp / 1283 .h enumerated. Generated initially by `tools/extract_sources_from_vcxproj.ps1` and now authoritative.
- `Src/license/aaa_sdk_version.generated.h.in` — `configure_file` template for the SDK version header.

Conventions worth knowing :
- `aaa_build_config.h` is force-included on every TU via `/FI` (or `-include` on Clang/GCC). It defines `AAASEED()`, `AAA_NEW_DESIGN()`, `AAA_TRACKER()`, `MSA_HOST_AAASEED`. The vcxproj does the same via `<ForcedIncludeFiles>`.
- `.c` files compile **as C++**, replicating the vcxproj's `<CompileAs>CompileAsCpp</CompileAs>`. Required because some vendor headers (`Src/language/lua/lua_socket/luasocket.h`, `mime.h`, `auxiliar.h`) embed bare `extern "C" {` blocks without `#ifdef __cplusplus` guards. The CMakeLists.txt has a small loop at the end that tags every `.c` source with `LANGUAGE CXX`.
- `Include/dlib/test_for_odr_violations.cpp` is the only non-`Src/` source attached to the target. Defines the `USER_ERROR__inconsistent_build_configuration__see_dlib_faq_*` symbols dlib's headers reference.
- `WIN32_LEAN_AND_MEAN` is **not** defined globally. `aaa_os.h`'s local logic defines it AND includes `<windows.h>` only when the macro isn't already in scope ; defining it globally would skip the include and leave `HANDLE`/`THREAD_PRIORITY_*`/etc. undefined.
- `/NODEFAULTLIB:msvcrt.lib` from the vcxproj is **deliberately not** carried over. On VS2015+ it would also block the transitive `ucrt.lib` + `vcruntime.lib` referenced by `/MD`, leaving CRT symbols (`atan2f`, `_Init_thread_*`, `__CxxFrameHandler*`, ...) unresolved.
- `/GL` + `/LTCG` are **on** for Metal/Release/Wood, matching the vcxproj. An earlier exposure on the v145 toolset triggered a C1001 ICE on `Src/obj_ui/bdd/bdd_edit/bdd_curve3d.cpp:2018`, but the full vcxproj-faithful flag set (`/Ob3 /Zp16 /fp:fast /GS- /Qpar /MP /Zm1000`) avoids the ICE path. See the "Configurations" subsection for the full rationale.
- The plan document for the CMake migration lives at `~/.claude/plans/cmake-migration.md`.

## Code Conventions

`CODE_STYLE.md` describes the recommended code style for AAASeed.

**Type macros** — always use project types, never raw C++ primitives directly:
- `REAL` — floating point scalar, switches between `float` and `double` via `AAA_REAL_IS_DOUBLE()`
- `FP32`, `DOUBLE` — explicit float / double when precision must be fixed
- `INT32`, `UINT32`, `INT16`, `UINT16`, `INT64` — sized integers
- `FINLINE`, `CONSTEXPR`, `NOEXCEPT` — compiler hint macros
- `C_PCHAR_C` — `const char* const`

**Lua binding layer** — `*_lua.cpp/h` files (e.g. `camera_lua`, `math_lua`, `img_lua`, `mat_lua`)
expose C++ objects to Lua scripts. They follow a consistent registration pattern and are
present in most subsystems. Full conventions in `Src/language/lua/CLAUDE.md`. Quick reference:
- `AAALUACALL(name) { ... }` declares a Lua-callable function in the current namespace.
- `ADD_FN(name)` inside a `register_*( lua_State* L )` block registers it under the current Lua table (opened with `l.define_table("X")` and closed with `l.pop(N)`).
- `add_fn_to_table("lua_name", cpp_fn)` registers under a different Lua name (used for C++ keywords, e.g. `delete` -> `aaa.obj.delete`).
- `//unused ADD_FN(...)` marks a binding declared but intentionally not exposed yet.
- Macros that expand to `AAALUACALL` implicitly: `AAALUA_CALL_FN`, `AAALUA_CALL_FN_INT32`, `AAALUA_CALL_FN_REAL`, `AAALUA_CALL_FN_BOOL`, `AAALUA_CALL_FN_RETURN_INT32`, `AAALUA_CALL_FN_RETURN_REAL`, `COOR_CALL`, `DEFINE_COLOR_FN`, `FN_UPDATE_UNIFORM`, `ADD_FN_3D` (registers `name_3d` under Lua name `name`).
- Doc convention (in the sibling's `AAADoc/lua_aaaseed_*.lua`): trailing `-- lua` tag means the function is defined in `aaa_*.lua` scripts (pure-Lua); absence of tag means it's a C binding.

**Include guards** — every header uses the pattern:
```cpp
#ifdef AAA_FILENAME_H
#error "FILENAME_H included more than once."
#endif
#define AAA_FILENAME_H 1
```
The guard macro is always `AAA_` + filename uppercased + `_H`; the error message is just the filename uppercased + `_H` (no extra `AAA_` prefix). Examples: `rect.h` → guard `AAA_RECT_H`, message `"RECT_H included more than once."`; `aaa_lock.h` → guard `AAA_AAA_LOCK_H`, message `"AAA_LOCK_H included more than once."`.

**Include guard exception — `Src/flex/`**: files in the flex folder intentionally use a folder-prefixed convention:
- Guard: `AAA_FLEX_` + filename uppercased + `_H` (e.g. `Spring_Host.h` → `AAA_FLEX_SPRING_HOST_H`)
- Message: `"FLEX/` + filename uppercased + `_H included more than once."` (e.g. `"FLEX/SPRING_HOST_H included more than once."`)

Do NOT normalise flex headers to the standard pattern — the `FLEX/` prefix is intentional.

**Ask before editing `err.h` or `aaa_type.h`** — these headers are included project-wide, so any change triggers a full recompile. If a fix or pattern naturally leads to modifying either file, stop and ask first rather than editing them unilaterally — the owner may prefer to batch or defer the change to avoid the rebuild cost.

**`if` body always on its own line** — never put the controlled statement on the same line as the condition. The body must be on the line below so a breakpoint can be set on the body alone and the debugger can step over / hit it independently of the condition evaluation.
```cpp
// YES
if( nb_x_in < 0 )
    nb_x_in = 0;

// NO
if( nb_x_in < 0 ) nb_x_in = 0;
```
Applies to `else`, `for`, and `while` single-statement bodies the same way. Braces are optional for a single-statement body; the line-break rule is what matters.

**`//todo<suffix>` markers** , the trailing suffix is metadata, not drift. `//todo` means "there is something to do here". The suffix (`q`, `qq`, `qqq`, `aqqq`, `now`, `nownow`, `fred`, `franz`, `par`, ...) precises **where / when / by whom** the marker was placed. Do not normalise the suffixes to a single form, they carry information. Only the bare `//todo` is the canonical short form when no extra context is needed.

**No `memcpy` in per-pixel hot paths in Debug** — `/Oi` (intrinsic `memcpy` expansion) only fires under `/O2`, so in Release / Metal a `memcpy( p, q, N )` with small constant N is inlined to a few native moves and costs the same as inline stores. In `Debug /Od` it becomes a real CRT call (no inlining), which kills realtime when called per-pixel. For tiny fixed-size copies in inner loops, expand stores inline or use the `cpy_v2 / cpy_v3 / cpy_v4 / cpy_v16` family in `Src/math/v_base.h` (memcpy fast path in Release, inline stores in Debug, same-type detection via `if constexpr( std::same_as<T, S> )`). For 2-byte type-punning of a wrapper class (eg, `FP16 = half_float::half` around a `uint16_t`), prefer `reinterpret_cast<UINT16 CONST &>(...)` over `memcpy(&bits, &v, 2)`.

**No dynamic allocation, use `o_str::push_name()` / `pop_name()` for temp strings** — AAASeed is a real-time engine and avoids dynamic allocation as much as possible. `o_str` is heap-allocated internally, so a local declaration like `o_str o_tmp;` followed by a write triggers one `malloc` + one `free` per call. Whenever you need a temporary `o_str` (a filename, an intermediate string, a name to copy into) use the static name stack pattern instead:
```cpp
o_str& tmp = o_str::push_name();
    tmp.set( ... );
    use( tmp );
o_str::pop_name();
```
The name stack is pre-allocated and recycled LIFO across calls, so the steady-state cost is zero allocation. Push must be paired with pop on every exit path. Many call sites already follow this pattern (search `o_str::push_name`). The same rule applies to other heap-allocating containers when an equivalent static-pool helper exists.

**Group tightly-related args without inner space** — when a function or macro takes several arguments and some of them form a conceptual unit (coordinates, sizes, colors), write the unit with no space between its components, and put a space between units. It reads as "one thing" visually, which matches the semantics.
```cpp
// YES -- (x,y) is one coord, (_nb_x,_nb_y) is one size
debug_break( "%s(%d,%d) out of range for %dx%d grid",
        __FUNCTION__, x,y, _nb_x,_nb_y );

set_pixel( x,y, r,g,b,a );          // coord, color
draw_line( x0,y0, x1,y1 );          // start, end
translate( x,y,z );                 // single 3D vector

// NO -- reads as 6/7 loose args
set_pixel( x, y, r, g, b, a );
```
This is a readability hint, not a hard rule: apply it where it helps (x,y / x,y,z / r,g,b / w,h / start-end pairs), skip it where the args are genuinely independent. Consistency within a single call matters more than consistency across the codebase.

## Architecture Overview

### GOL — OpenGL Isolation Layer (`Src/gol/`)
All OpenGL calls go through the `GOL` namespace. Direct `gl*` calls outside GOL are not the pattern.
See `Src/gol/CLAUDE.md` for full GOL patterns.

### Math (`Src/math/`)
Two parallel math libraries: raw-pointer ops (`v_base.h`, `v.h`) and a separate template library
(`T*.h` files — `TVector`, `TMatrix*`, `TRay`, etc. — different style, do not confuse them).
See `Src/math/CLAUDE.md`.

### Draw (`Src/draw/`)
High-level scene objects (cameras, lights, materials, textures, render state, fog, geometry).
All inherit from `c_obj_ui` through the factory system. See `Src/draw/CLAUDE.md`.

### Image (`Src/image/`)
Image loading, pixel-format conversion, GPU upload, color space handling.
See `Src/image/CLAUDE.md`.

### Infrastructure (`Src/infrastructure/`)
Core object model (`c_obj` → `c_obj_ui`), factory/registry, param binding, Trax animation,
layer rendering system, and data storage. See `Src/infrastructure/CLAUDE.md`.

### Threading (`Src/aaa/`)
All mutex types in the `aaa` namespace. RAII guards mirror `std::` interface.
See `Src/aaa/CLAUDE.md`.

### Utilities (`Src/code_utils/`)
Memory (`mem` singleton), strings (`o_str`), file I/O (virtual FS), time, scheduler.
See `Src/code_utils/CLAUDE.md`.

### UI Layer (`Src/ui/`)
Event dispatch, modal dialogs, menus, fonts, 2D overlay (flatland), preferences.
See `Src/ui/CLAUDE.md`.

---

## Known Patterns to Watch (Project-Wide)

**Dirty sentinel**: `-42` is the standard uninitialized/dirty marker used project-wide
(e.g. `fbo_id_cur = -42`). `color_cur = {-1,-1,-1,-1}` is a separate dirty sentinel
specific to GOL color state.

**Pre-decrement write idiom** — this pattern is correct and writes exactly N elements:
```cpp
INT32 nb = N + 1;
T*    pt = _data - 1;
while( --nb )
    *++pt = value;
```
Do NOT flag as an off-by-one error. The `data-1` / `++pt` combination is intentional.

**5-element color arrays**: `_color[5]` where `[0..2]` = RGB, `[3]` = alpha, `[4]` = grey/
intensity scale factor. `src[4]` used as a scale in functions like `scale_v3_cpy_v4` is
intentional — `src` is a 5-element array, not a 4-element one.

**`c_obj_active_ui`** is a `typedef` alias for `c_obj_ui` (not a separate class).
The original subclass was commented out; the typedef remains for naming clarity.

**Static cur/ui/def pattern**: Many scene objects maintain static instance pointers
(`cur` = currently active, `ui` = user-selected, `def` = default fallback).

**Debug guards** — use `#if AAA_DEBUG()` + `debug_break()` (declared in `err.h`) for runtime assertions. Use `aaa::LOWEST<T>` / `aaa::BIGGEST<T>` (from `aaa_type.h`, wraps `std::numeric_limits`) for type range constants. Never use `__debugbreak()` directly or hardcode numeric limits.
```cpp
#if AAA_DEBUG()
    if( f < T(aaa::LOWEST<INT32>) || f > T(aaa::BIGGEST<INT32>) )
        debug_break( "f out of INT32 range -- cast to INT32 is UB" );
#endif
```

**`SAFE_*` allocation / deallocation helpers** , declared in [Src/code_utils/aaa_mem.h](Src/code_utils/aaa_mem.h). Always prefer them to raw `new` / `delete` / `delete[]` for owned heap pointers. They are null-safe and reset the pointer to `nullptr` after free, so a double-free or use-after-free path turns into a no-op rather than UB.
- `SAFE_DELETE(p)` , delete + null. Use for single-object `new T`.
- `SAFE_NEW_ARRAY(p, size)` , `new(nothrow) T[size]` only if `p` is currently null. Allocation-failure-tolerant (caller observes `p == nullptr`).
- `SAFE_DELETE_ARRAY(p)` , `delete[]` + null. Use for `new T[size]` blocks.
- Companion C-style helpers : `FREE_AND_NULL(p)`, `FREE_ALIGNED_AND_NULL(p)`, `IF_FREE_AND_NULL(p)`, `IF_FREE_ALIGNED_AND_NULL(p)` for `MALLOC` / `REALLOC_ALIGNED` blocks (use `mem::malloc` family, not `std::malloc`).

The pattern goes hand in hand with the next bullet : `REALLOC` returns null on failure (after freeing the original), so an allocation-failure path leaves you with a clean `nullptr` and `SAFE_DELETE_*` then does nothing.

**`REALLOC` / `REALLOC_ALIGNED` free the original block on failure** (non-standard vs. stdlib `realloc`). Every call site in Src uses the `x = REALLOC(x, size)` pattern, which on stdlib semantics would leak the original block on allocation failure. `mem::realloc` / `mem::realloc_aligned` in `Src/code_utils/aaa_mem.cpp` therefore deliberately free the original on failure, so every caller safely ends up with `x == nullptr`. Do NOT flag `x = REALLOC(x, size)` as a leak, and do NOT change the wrappers back to stdlib semantics.

**`#if 1 / #else` alternative branches are intentional** — the project keeps conditionally-compiled alternative implementations (e.g. `stdcall_thread_func` vs `cdecl_thread_func` in `Src/code_utils/thread/aaa_thread.cpp`) as compile-time toggles for future experimentation or platform variants. Do NOT delete the `#else` branch just because it's currently unreachable. When fixing a bug in the active branch, mirror the fix into the inactive branch so both stay consistent.

**Regex `\b` boundary does NOT match at `_`** — when doing a project-wide substring sweep with regex (rename campaigns, typo fixes, etc.), remember that `\b` is the transition between a word-char and a non-word-char, and `_` IS a word-char. So `\bSTEM\b` does NOT match `STEM_SUFFIX`. For a stem that may be followed by `_FOO` (typical in C/C++ : `XXX_H` include guards, `XXX_LUA` lua bindings, `XXX_WIN` platform suffixes, `XXX_H_V`), either drop the `\b` entirely (plain substring) or use a lookahead like `STEM(?=[_\W]|$)`. Verified empirically during the 2026-05 typo + rename sweeps : `\balignement\b` silently missed `ALIGNEMENT_H` and required a second pass.

**Source files are ASCII-only** — inside `.cpp` / `.h` files, every character in string literals and comments must be plain 7-bit ASCII. The debug output sink (`DBG_PRINT_STRING`, `ERR_PRINT_STRING`, `debug_break` messages, console, log) writes bytes straight to a cp-1252 Windows console; any UTF-8-encoded non-ASCII glyph (em dash `—`, en dash `–`, curly quotes `’‘“”`, ellipsis `…`, arrows `→`, bullet `•`, accented letters) renders as multi-byte garbage like `ÔÇö`. Use ASCII equivalents: `--` for em dash, `'` for apostrophes, `"` for quotes, `...` for ellipsis, `->` for arrows. Autocorrect and paste-from-Word sneak these in silently. Full rationale in `CODE_STYLE.md`. Markdown docs (`CLAUDE.md`, `*.md`) are exempt since they're never piped to runtime output.

**Enum params serialize by string label, not numeric value** — params bound to a C++ `enum class` (typical case: a UI choice list driven by a `*_str[]` table such as `force_pixel_format_str[]` for `PIXEL_FORMAT_FORCE`) are persisted in `.deproj` and similar project files as the **string label**, not the underlying INT32. So adding new enum values in the middle of the enum, or reordering them, is **safe**: old project files still find their label and resolve to the correct entry. What is NOT safe is **renaming an existing label** (the string in the `*_str[]` table), since that orphans every project that referenced it. When extending an enum, append new values wherever it makes semantic sense, add the matching label string, but treat existing labels as immutable.
