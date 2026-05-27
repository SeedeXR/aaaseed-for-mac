# Porting — Windows AAASeed → Mac AAASeed

> The single most authoritative document about **what the Windows engine actually does** and **how each piece will be brought across to macOS**. Read this end-to-end before touching code. Update as facts on the ground change.

---

## 1. Repos and how they fit together

The Mac port is **a single self-contained repository**. The upstream Windows engine and runtime live as snapshots inside `vendor/`:

| Path inside `aaaseed-for-mac/` | Origin | Purpose |
|---|---|---|
| `vendor/aaaseed-engine/` | Snapshot of Mâa's `aaaseed_exe-main` (Windows C++ engine) | Engine source `Src/`, 3rd-party headers `Include/`, build reference `cmake/`, `CMakeLists.txt`. CMake reads from here. |
| `vendor/aaaseed-runtime/` | Snapshot of Mâa's `aaaseed-main` (runtime content) | Lua kernel `AAAKernel/`, MEU prototypes, GaBu, GLSL shaders, fonts, textures, example apps. Loaded by the Mac binary at runtime; also packaged into the `.app` bundle's `Resources/`. |

Both snapshots are byte-identical to the upstream trees minus Windows-only binary artifacts (`lib_x64/`, `Lib_x64/`, `dll_x64/`, `*.exe`, `*.pdb`, `*.dll` — these are useless on Mac and would bloat the clone size). See `VENDORING.md` at the repo root for the snapshot rationale, allowed-edit policy, and sync-from-upstream procedure.

`../aaaseed-windows/` outside the repo is kept on the dev machine as a historical reference for diff-against-upstream debugging, but the build does not depend on it.

### Runtime-content model: single source, snapshot at build time

The runtime tree (`aaaseed-main/AAAKernel/`) is **platform-neutral and single-source**. There is no fork between Windows and Mac. Lua 5.1 scripts, GLSL text, fonts, textures, MEU prototypes all load identically on both platforms.

**How that single source reaches the running engine** has one model with two access paths:

| Access path | When | Purpose |
|---|---|---|
| **`AAASeed_DirStart` env var** | Set: dev iteration, mounting the live shared tree | Edit-and-hot-reload across platforms. Same convention as Windows. |
| **`[NSBundle mainBundle].resourcePath/AAAKernel/`** | Default: shipped `.app` on end-user machine | Self-contained install via DMG. No external runtime needed. |
| **`./AAAKernel/`** (cwd) | Last-resort headless / CLI fallback | Useful for `--headless --script foo.lua` test runs. |

The engine resolves in that order at boot.

**Bundling is a snapshot, not a fork.** At build time, CMake's `install` step copies the current state of `aaaseed-main/AAAKernel/` (and optionally `AAAAPPs/`, `AAADoc/`) into `AAASeed.app/Contents/Resources/`. The next build re-snapshots from the same upstream, so changes in the runtime tree propagate to the next DMG release automatically. The upstream remains the only place anyone edits the content.

**Implication:** native Mac running does not require Mac-side access to the Windows runtime repo for end-users — the bundled snapshot is self-sufficient. Developers benefit from the env-var path to iterate live against the shared tree without rebuilding.

---

## 2. Directory layout (engine — `aaaseed_exe-main/Src/`)

Subsystems and their role:

| Folder | Files (approx) | Role | Windows-specificity |
|---|---|---|---|
| `aaa/` | 7 | Threading primitives (mutex, lock-guard, spinlock, benaphore), `c_rect`, `execute` (process spawn). | Heavy: `CRITICAL_SECTION`, `CreateProcessA`. Already partly portable via `std::` fallback. |
| `aaa_check_cpu.{h,cpp}` | 2 | CPUID-based AVX2 startup check; CRT C-initializer (`.CRT$XIB`). | Heavy: relies on x86 CPUID. Mac arm64 path: skip entire check (no AVX2 concept on ARM). |
| `aaa_os.h` | 1 | Win32 SDK floor + `<windows.h>` include logic. | Heavy: replace on Mac with `#include <TargetConditionals.h>` + `<Cocoa/Cocoa.h>` (in `.mm` files only). |
| `aaaseed.{h,cpp}` | 2 | Application entry; ties `c_app` to platform window. | Heavy: `WinMain`. Mac: `NSApplicationMain` + main runloop hosted by `MTKView` delegate. |
| `action.{h,cpp}` | 2 | High-level UI actions. | Clean. |
| `blosc/` | many | Compressed binary blob store. Includes `blosc/win32/pthread.h` thin wrapper. | Mac: drop the Win32 pthread wrapper, use system pthread / std::thread. |
| `boids/` | 11 | Boid flocking sim, Lua-exposed. | Clean. |
| `code_utils/` | 57 | Memory (`mem::*`), strings (`o_str`), file/dir/path, time, scheduler, os_version. | Light Win32: file paths, `os_version` (`RtlGetVersion`). |
| `core/` | — | (verify) | — |
| `draw/` | 94 | Scene objects: cameras, lights, materials, textures, geometry DB, picking, fog, gradients. All inherit `c_obj_ui`. | Clean — rendering routed through GOL. |
| `evt/` | — | Event types and dispatch. | Verify; expected platform-neutral. |
| `executable/` | — | Top-level main / EXE entry. | Heavy: `WinMain`. Mac: new entry point. |
| `FBO/`, `FFGL/`, `Gypsy/`, `MSA/`, `OpenCL/`, `OpenFramework/`, `SdfText/`, `SpoutSDK/`, `StackWalker/`, `TrueType/` | various | Vendored third-party (or Mâa-vendored). | Mixed: FFGL & SpoutSDK & StackWalker = Windows-only; others portable. |
| `fbx/` | 19 | Autodesk FBX SDK glue. | Portable (FBX SDK is Win/Mac/Linux). |
| `flex/` | 29 | NVIDIA Flex (fluid/cloth) + CUDA bridge. | Heavy: CUDA Windows/Linux only. Mac v1: disabled. |
| `ftgl/` | 52 | FTGL font rendering on top of FreeType. | Portable. |
| `gl/` | 22 | OOP modern-GL resource layer (texture, FBO, VAO, UBO, SSBO). | Light — uses GOL internally. |
| `gol/` | 30 | **OpenGL isolation layer.** All raw GL calls funnel through `GOL::` namespace. | Heavy: `<wglew.h>`, WGL context, vsync via `wglSwapIntervalEXT`. This is the prime target for backend abstraction. |
| `image/` | 49 | Image I/O, pixel format conversion, GPU upload. | Light — uses GOL for upload. |
| `infrastructure/` | 34 | `c_obj` → `c_obj_ui`, factory/registry, params, layer/layers/module/modules, trax animation, data storage. | Clean. |
| `language/lua/` | 19 | Lua 5.1 binding layer: state wrapper, registration macros, helpers. | Clean. Per-subsystem `*_lua.cpp` files (e.g. `camera_lua.cpp`) register C++ into Lua tables. |
| `lib_wrappers/` | — | Vendored library wrappers. | Verify per-library. |
| `license/` | — | License banner + `aaa_sdk_version.generated.h`. | Light: header generation hooks into both MSBuild + CMake. Mac CMake adds its own configure_file step. |
| `math/` | 50 | SIMD math: `v.h`, `v_base.h`, `TVector.h`, `TMatrix*.h`, `TRay.h`, etc. Two parallel libraries (raw-ptr ops and templated). | Clean. SSE intrinsics: confirm Clang lowering to NEON, or branch to NEON manually for hottest paths. |
| `media/` | — | Media pipeline (video, audio). | Verify. Likely heavy if it touches DirectShow. Mac: AVFoundation. |
| `mocap/` | 4+ | Kinect, optical mocap, body tracking. | Heavy. Mac v1: disabled. |
| `net_requests/` | — | HTTP client wrapper over libcurl/cpr. | Clean (curl/cpr portable). |
| `obj_ui/` | many | Includes `bdd/` (drawer leaves), per-object UI behavior. | Clean. |
| `platform/` | 2 + `win32/` | Platform stubs (power_master.h); `win32/` holds Windows-only helpers (touch, DLL load, dialog util). | Mac: add `platform/macos/`. |
| `shaders/` | 19 | C++ shader-helper code (not GLSL files themselves; those live in the runtime repo). | Clean. |
| `system/` | — | System info, capabilities. | Light: probably reads Win32. |
| `thread/` | — | Higher-level thread utilities. | Verify. |
| `tinyxml/` | — | XML parser. | Portable. |
| `tracking/` | 3 | Object tracking. | Heavy: FlyCapture / OpenCV. v1 Mac: disabled. |
| `ui/` | 59 | Win32 UI: events, dialogs, menus, font, prefs, flatland overlay, file dialog, keyboard, mouse. | **Totally Win32.** Mac: full replacement in `src/ui/macos/`. |
| `unused/` | — | Dead code / preserved drafts. | Skip on Mac. |
| `wrapper/` | — | Thin wrappers. | Verify. |

---

## 3. Render graph and its module-to-module flow

Detailed in `mindmap.md` — repeated here in execution terms.

**Boot:**
1. `WinMain` → init COM → init GL context via WGL → load `<glew>` extensions → set vsync.
2. Construct `g_app` (singleton of `c_app`). Allocate `c_modules::main`.
3. Read `AAAKernel/Dev/AAAUser/Guest/default.lua_master.lua` to discover which `aaa_*.lua` files to load.
4. Each Lua file is wrapped in a `c_lua_wrap` C++ object that watches it for changes (hot reload).
5. Run kernel Lua, which registers GaBu, MEUs, default scene.
6. Enter main loop.

**Per-frame:**
1. Pump Win32 messages (`PeekMessage` / `DispatchMessage`). Keyboard / mouse state cached.
2. `g_app.update()`. Walks `c_modules` → `c_module` → `c_layers` → `c_layer`. Each calls `update()` on its `c_obj_ui` children and on the wrapped Lua MEUs (`update()` Lua function fires).
3. Begin frame: clear, set viewport, set initial GL state.
4. `g_app.draw()`. Same tree walk, calls `draw()` which emits GL through `GOL::*`.
5. `SwapBuffers(hdc)`. Vsync handled by WGL.
6. Hot reload check: any `.lua` file changed since last frame? Reload it.

**Inter-module communication:**
- Through **params** (`c_param`). Every object has a typed parameter set. Params can be read / written by other objects, by Lua, by the UI. This is the "remote control" mechanism.
- Through **values block** (`c_values`) at the `c_layers` / `c_module` / `c_modules` levels — a key/value scratchpad for cross-cutting state.
- Through **trax** (`c_trax`) — older animation-driver mechanism that schedules param-set updates over time.

**Hardware touchpoints on Windows:**
- GPU via OpenGL (NVIDIA / AMD / Intel driver).
- CPU SIMD via AVX2 (Metal/Release/Debug configs) or SSE2 (Wood config).
- Display via Win32 multi-monitor APIs (`EnumDisplayMonitors`, `GetMonitorInfo`).
- HID via `GetAsyncKeyState`, `GetCursorPos`, Win32 raw input for touch.
- Cameras via DirectShow / Kinect SDK / FlyCapture / Azure Kinect SDK.
- Audio via PureData / PlugData over OSC (separate process; bridge is OSC packets, not in-process audio).
- Inter-app GPU sharing via Spout (DirectX texture handles over IPC).

**Hardware touchpoints on Mac (target):**
- GPU via Metal (`MTLDevice` from `MTLCreateSystemDefaultDevice()`).
- CPU SIMD via NEON (ARM); Apple's `<simd/simd.h>` for high-level vector types.
- Display via `NSScreen` enumeration, `CGDisplay*` APIs for absolute pixel layout.
- HID via `NSEvent` event stream (keyboard, mouse, trackpad).
- Cameras via `AVCaptureSession` / `AVCaptureDevice`.
- Audio via PlugData (same; OSC is platform-neutral).
- Inter-app GPU sharing via **Syphon** (IOSurface-backed textures).

---

## 4. Build system

**Windows current state (verified):**
- `AAASeed_by_Maa.sln` + `.vcxproj` — historical, still authoritative.
- `CMakeLists.txt` + `CMakePresets.json` — parallel CMake build, produces the same EXE.
- 4 configs: `Debug`, `Release`, `Metal`, `Wood`. Two toolsets: `v143` (VS2022), `v145` (VS2026).
- Force-include `Src/aaa_build_config.h` everywhere via `/FI`.
- Per-subsystem `Src/<subsys>/sources.cmake` files enumerate sources.

**Mac target state (Phase 1):**
- New `aaaseed-for-mac/CMakeLists.txt` that **detects `APPLE`** and includes Mac-specific cmake snippets.
- Reuse the per-subsystem `sources.cmake` enumerations where they list platform-agnostic files; introduce `sources_mac.cmake` for Mac-only files (in `src/platform/macos/`, `src/ui/macos/`, `src/gol/metal/`, `src/SyphonSDK/`).
- Custom build rule for `.metal` → `default.metallib` using `xcrun metal` and `xcrun metallib`.
- Output: `AAASeed.app` bundle.
- Compiler: Apple Clang. C++ standard: `c++20` (matches `/std:c++latest` on MSVC closely enough).

**Compiler-flag translation (MSVC → Clang):**

| MSVC flag | Clang equivalent |
|---|---|
| `/Od` | `-O0` |
| `/O2 /Ob3` | `-O3` (Clang's `-O3` enables aggressive inlining; no exact `/Ob3` knob) |
| `/Oi` | `-fbuiltin` (default in `-O2+`) |
| `/Ot /Oy` | `-O3 -fomit-frame-pointer` |
| `/GL` + `/LTCG` | `-flto=thin` |
| `/Gy-` | `-fno-function-sections` |
| `/fp:fast` | `-ffast-math` (be aware of NaN/Inf assumption changes) |
| `/Zp16` | `__attribute__((aligned(16)))` per-struct OR `-fpack-struct=16` (avoid global; prefer per-struct) |
| `/GS-` | `-fno-stack-protector` |
| `/Qpar` | (Clang vectorizes by default at -O3; no exact equivalent flag) |
| `/MP` | (n/a; CMake passes `-j` to ninja) |
| `/arch:AVX2` | (n/a on arm64; Apple Silicon has NEON, not AVX2) |
| `/FI <file>` | `-include <file>` |

---

## 5. Per-subsystem porting recipe

For each subsystem flagged 🔴 or 🟣 in `mindmap.md`, the recipe:

### 5.1 `Src/gol/` (Heavy → Mac via interface)

**Step 1.** Read every `.h` in `Src/gol/`. List the public functions on the `GOL` namespace.

**Step 2.** Group them into capabilities:
- Context/OS (init, vsync, swap, present, viewport)
- Buffers (create, upload, bind, destroy)
- Textures (create, upload, bind, generate-mipmaps, sample-state)
- Shaders (compile, link, bind, set-uniform)
- State (depth, blend, cull, scissor, stencil)
- Draw (draw-arrays, draw-elements, draw-instanced, draw-indirect)
- Misc (PBO, FBO, query, fence)

**Step 3.** Convert the namespace into a virtual interface `class GOL::Backend { virtual ... }`. Move existing function bodies into `GOL::WindowsBackend : public Backend`. Inline-compile-time path on Windows stays identical (no runtime virtual call cost: pick the backend at startup, store a global pointer).

**Step 4.** Implement `GOL::MetalBackend : public Backend` in `src/gol/metal/`. Each method translates the operation into Metal. Where Metal's model differs (e.g. no fixed-function lights, no immediate-mode matrix stack), translate to constant-buffer-backed equivalents.

**Step 5.** Smoke-test: a tiny program that draws a textured triangle via the GOL interface, runs on both backends, produces the same image (within tolerance).

### 5.2 `Src/ui/` (Total → full replacement)

The Win32 UI cannot be ported file-by-file; the model differs. Strategy:

1. Identify the **AAASeed-internal event API** (likely `c_event_*` in `Src/ui/event/`). That is the boundary.
2. Implement a Mac event source in `src/ui/macos/` that translates `NSEvent` (keyboard, mouse, scroll, trackpad gestures, magnification, rotation) into `c_event_*` and pushes them into the same dispatcher the Windows code uses.
3. Dialogs: re-author each dialog as a `NSPanel` subclass OR as an in-Metal immediate-mode panel inside the GaBu layer. Prefer the in-Metal route to keep look-and-feel identical between platforms.
4. File dialogs: `NSOpenPanel` / `NSSavePanel`.
5. Menus: macOS has a single menubar attached to `NSApp`. AAASeed uses pop-up menus tied to mouse buttons (`Src/ui/aaa_menu.h`); these are not in the macOS menubar, they are in-app overlays — render them in-Metal too. The macOS menubar remains a minimal app menu (Quit, Hide, etc.).
6. Fonts: AAASeed has its own bitmap-font path and FTGL/SDF text. Keep both; they only need the GPU upload to route through MetalBackend.

### 5.3 `Src/aaa/` threading

- `c_mutex` (`std::mutex`): unchanged.
- `c_mutex_recursive` (`std::recursive_mutex`): unchanged.
- `c_mutex_ct` (`CRITICAL_SECTION`): on Mac, fall through to `std::mutex`. The `c_mutex_ct` type continues to exist; its implementation is `std::mutex` on `__APPLE__`.
- `c_spinlock` (`std::atomic_flag` + yield): unchanged.
- `c_benaphore`: drop on Mac; replace call sites with `c_mutex`.
- `execute.cpp` `CreateProcessA`: replace with `posix_spawn` on Mac.

### 5.4 `Src/code_utils/` file & path

- `aaa_dir.cpp` / `aaa_file.cpp`: replace Win32 `FindFirstFile` / `CreateFile` with POSIX `opendir` / `open` / `stat`. Normalize path separator to `/`.
- `os_version.cpp`: on Mac, read `kern.osrelease` or use `[NSProcessInfo processInfo].operatingSystemVersion`.

### 5.5 `Src/flex/`, `Src/mocap/`, `Src/tracking/`, `Src/FFGL/`, `Src/SpoutSDK/`

- **Flex**: behind `AAA_HAS_FLEX()` macro, defined `1` on Windows, `0` on Mac. Disable all Flex bindings on Mac.
- **Spout**: replace with Syphon. New folder `src/SyphonSDK/` mirroring SpoutSDK's interface. Syphon API: `SyphonServer` and `SyphonClient` from Syphon framework; IOSurface-backed Metal textures.
- **FFGL**: defer. Out of scope v1.
- **Mocap / tracking**: behind `AAA_HAS_KINECT()` / `AAA_HAS_FLYCAPTURE()`. Disabled on Mac v1.

### 5.6 `Src/media/` (video / audio)

- Video decode (currently DirectShow via K-Lite): replace with AVFoundation `AVAssetReader`. New file `src/media/macos/video_avf.mm`.
- Audio: PlugData runs as a separate process and talks OSC. The bridge is just OSC packets over UDP; no platform-specific code.

---

## 6. Test harness mapping

| Test tier | Windows runner | Mac runner | Shared assets |
|---|---|---|---|
| Unit | GoogleTest, MSVC | GoogleTest, Clang | Per-subsystem TUs |
| Integration | headless launcher `--script tests/integration/<name>.lua` | same | `aaaseed-main/AAAKernel/test/` |
| Regression | frame-buffer hash compare to `tests/regression/golden/` | same | Same golden corpus |

---

## 7. Risk register

| Risk | Likelihood | Impact | Mitigation |
|---|---|---|---|
| OpenGL → Metal shader translation introduces visual drift in 373 GLSL files | High | High | Start with SPIRV-Cross batch; hand-fix the 10–20 outliers. Capture per-shader regression frames before and after. |
| `c_obj_ui` factory + RTTI assumptions tie to MSVC-specific layout | Low | High | Verify with a small spike: instantiate `c_obj_ui` subclass under Clang, dump vtable, compare. |
| AVX2 code paths fail to lower cleanly to NEON via Clang | Medium | Medium | Audit `Src/math/v.h` for explicit `__m128`; provide NEON-via-`<arm_neon.h>` fallbacks in a `simd_compat.h`. |
| Hot reload semantics differ (file-watching API) | Medium | Low | macOS has `FSEvents`; wrap behind the existing `c_lua_wrap` change-detection interface. |
| Lua 5.1 + extensions assume Windows path separators or `\r\n` | Low | Low | Audit `Src/language/lua/lua_socket/` and friends; mostly platform-neutral. |
| Syphon vs Spout: protocol mismatch means MEUs sharing GPU frames across platforms fail | High | Medium | v1: document the asymmetry. Long-term: build a NDI bridge (cross-platform) for cross-OS GPU sharing. |
| Frame pacing on ProMotion displays (120 Hz variable) | Medium | Low | Default to `view.preferredFramesPerSecond = 60`; expose toggle. |

---

## 8. Provenance

This document is grounded in:
- `aaaseed-windows/aaaseed_exe-main/CLAUDE.md` (build repo)
- `aaaseed-windows/aaaseed-main/CLAUDE.md` (runtime repo)
- `aaaseed-for-mac/books/AAASeed_Introduction.md`
- `aaaseed-for-mac/books/GPU_Programming_on_Apple_Silicon_Using_CPP.md`
- `aaaseed-for-mac/books/Metal by Tutorials (Fourth Edition).md`
- `aaaseed-for-mac/instructions/links.md`
- Direct file inspection of `Src/` subtree (see Explore agent reports archived in `handover_session.md`).

Update this file whenever the underlying source moves, a subsystem boundary changes, or a backend decision flips.
