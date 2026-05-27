# Instruction — End-to-End Porting Guide

> The executable how-to for the AAASeed Windows-to-Mac port. Reads as a runbook: modules, directory structure, process flows, compression and asset handling, and the approach to porting. Companion to `memory/philosophy.md` (why) and `memory/porting.md` (per-subsystem detail). Read those first; this one tells you the order of operations.

---

## 0. Pre-flight

Before running any command in this document:

1. Read `memory/session_start.md` end-to-end.
2. Confirm the three mindsets are loaded: **multi-agent**, **token-efficiency**, **zero-hallucination**.
3. Confirm the repo is self-contained:
   ```
   aaaseed-for-mac/                  # this is the entire project
   ├── CMakeLists.txt
   ├── CMakePresets.json
   ├── VENDORING.md
   ├── src/                          # Mac-native code
   ├── tests/
   ├── vendor/
   │   ├── aaaseed-engine/           # snapshot of upstream C++ engine
   │   └── aaaseed-runtime/          # snapshot of upstream Lua/asset runtime
   ├── books/
   ├── instructions/                 # this file lives here
   └── memory/
   ```
4. `xcrun --show-sdk-path` returns a valid macOS SDK path (Xcode Command Line Tools installed).
5. `xcrun metal --version` returns a Metal compiler version (Apple Silicon with Metal toolchain installed).
6. `cmake --version` ≥ 3.27 (for `CMakePresets.json` v6 schema).
7. `clang --version` ≥ 15 (Apple Clang ships with Xcode).

---

## 1. Module inventory (what is being ported)

Eleven module groups, in the order they should land on macOS. Each group lists its source folder under `aaaseed-windows/aaaseed_exe-main/Src/`, the target folder under `aaaseed-for-mac/src/`, and the porting verb.

| # | Source | Target | Verb | Notes |
|---|---|---|---|---|
| 1 | `Src/math/`, `Src/infrastructure/`, `Src/code_utils/` (clean subset) | `src/math/`, `src/infrastructure/`, `src/code_utils/` | **Compile-as-is** under Clang | First green build proves the toolchain works |
| 2 | `Src/code_utils/` Win32-touching files (`aaa_dir.cpp`, `aaa_file.cpp`, `os_version.cpp`) | same path, POSIX impl | **Re-implement** the Win32 calls | Path conventions, `RtlGetVersion` → `sysctl` |
| 3 | `Src/aaa/` | `src/aaa/` | **Conditional compile** | `c_mutex_ct` → `std::mutex` on Mac; drop `c_benaphore` |
| 4 | `Src/image/` | `src/image/` | **Compile-as-is** | GPU upload calls go through GOL — no change here |
| 5 | `Src/language/lua/` | `src/language/lua/` | **Compile-as-is** | Vendor `liblua51.a` for arm64 |
| 6 | `Src/draw/` | `src/draw/` | **Compile-as-is** | Pure C++ scene-graph code |
| 7 | `Src/gol/` | `src/gol/` + `src/gol/metal/` (new) | **Abstract + new backend** | Phase 3 of the port |
| 8 | `Src/ui/` Win32 layer | `src/ui/macos/` (new) | **Replace** | `NSWindow` + `MTKView` + `NSEvent` translation |
| 9 | `Src/platform/win32/` | `src/platform/macos/` (new) | **Replace** | DPI, dynamic library loading, touch |
| 10 | `Src/SpoutSDK/` | `src/SyphonSDK/` (new) | **Replace** | Spout → Syphon |
| 11 | `Src/flex/`, `Src/FFGL/`, `Src/mocap/`, `Src/tracking/` | `src/<same>/` | **Gate off via `AAA_HAS_*`** | Mac v1: disabled |

Subsystems intentionally not in this list (`Src/blosc/`, `Src/boids/`, `Src/fbx/`, `Src/ftgl/`, `Src/TrueType/`, `Src/SdfText/`, `Src/MSA/`, `Src/OpenFramework/`, `Src/gl/`, `Src/shaders/`, `Src/StackWalker/`, `Src/OpenCL/`, `Src/Gypsy/`, `Src/net_requests/`, `Src/system/`, `Src/thread/`, `Src/evt/`, `Src/action/`, `Src/tinyxml/`, etc.) are either platform-clean (just compile) or get pulled in transitively. They are not separate workstreams.

---

## 2. Directory structure (target)

The Mac engine source mirrors the Windows engine source directory layout, with Mac-specific additions in clearly-named sibling folders.

```
aaaseed-for-mac/
├── CMakeLists.txt                       # root build script (Phase 1)
├── CMakePresets.json                    # macos-arm64-debug | -release | -metal
├── books/                               # reference reading (existing)
├── instructions/                        # this folder (existing)
│   ├── links.md                         # external references (existing)
│   └── instruction.md                   # this file
├── memory/                              # agent state (existing)
│   ├── agent_profile.md
│   ├── handover_session.md
│   ├── mindmap.md
│   ├── philosophy.md
│   ├── porting.md
│   ├── project_context.md
│   ├── session_start.md
│   └── todo.md
├── src/
│   ├── math/                            # mirror of Src/math/
│   ├── infrastructure/                  # mirror of Src/infrastructure/
│   ├── code_utils/                      # mirror of Src/code_utils/ (POSIX paths)
│   ├── image/                           # mirror of Src/image/
│   ├── draw/                            # mirror of Src/draw/
│   ├── language/lua/                    # mirror of Src/language/lua/
│   ├── aaa/                             # mirror of Src/aaa/ (std::mutex paths)
│   ├── gol/                             # mirror of Src/gol/ — interface + Windows backend (referenced)
│   │   └── metal/                       # NEW — Metal backend (.mm files)
│   ├── ui/                              # mirror of Src/ui/ — shared event abstraction only
│   │   └── macos/                       # NEW — Cocoa host (.mm files)
│   ├── platform/                        # mirror of Src/platform/
│   │   └── macos/                       # NEW
│   ├── SyphonSDK/                       # NEW — replaces SpoutSDK on Mac
│   ├── shaders/                         # shader-helper C++ (mirror of Src/shaders/)
│   └── ...                              # everything else mirrored
├── shaders/                             # NEW — compiled .metallib output goes here
├── tests/
│   ├── unit/
│   ├── integration/
│   └── regression/
│       └── golden/                      # captured frames from Windows reference build
├── cmake/                               # cmake helpers (mirrors Windows aaaseed_exe-main/cmake/)
│   ├── defines.cmake
│   ├── compiler_flags.cmake
│   ├── include_dirs.cmake
│   ├── link_options.cmake
│   ├── frameworks_macos.cmake           # NEW — finds Metal, MetalKit, Cocoa, etc.
│   └── metallib_compile.cmake           # NEW — .metal → .metallib rule
├── third_party/                         # vendored libraries built for arm64
│   ├── lua-5.1.5/
│   ├── opencv/
│   ├── freetype/
│   ├── freeimage/
│   ├── libcurl/
│   ├── cpr/
│   ├── assimp/
│   ├── bullet/
│   ├── blosc/
│   ├── gtest/
│   ├── metal-cpp/                       # Apple's official C++ binding (header-only)
│   └── syphon-framework/                # macOS-only
└── README.md
```

**Symlink convention.** The Windows source at `aaaseed-windows/aaaseed_exe-main/Src/<subsys>/` is the authority. The Mac `src/<subsys>/` does NOT copy those files; it includes them via the CMake `target_sources` enumeration (paths resolved relative to the Windows tree). New Mac-only files live physically in `aaaseed-for-mac/src/<subsys>/macos/` or `src/<subsys>/metal/`. This guarantees Windows changes flow into the Mac build without manual sync.

(If you must copy files instead of cross-tree referencing — e.g. CMake limitations or a path-length issue — document the exception in `porting.md`.)

---

## 3. Process flows

### 3.1 Boot

```
NSApplicationMain(argc, argv)
  ↓
[NSApplication run]
  ↓
AppDelegate -applicationDidFinishLaunching:
  ↓
  create NSWindow
  create MTKView (with shared MTLDevice)
  set MTKView delegate = AAASeedRenderer
  ↓
AAASeedRenderer -init
  ↓
  initialize C++ engine:
    GOL::Backend::set( new GOL::MetalBackend(device) )
    g_app = new c_app
    g_app.boot_from( resolve_runtime_dir() )
      ↑ resolve order:
          1. $AAASeed_DirStart env var (dev iteration; shared with Windows tree if available)
          2. [NSBundle mainBundle].resourcePath + "/AAAKernel/"  (shipped .app)
          3. cwd + "/AAAKernel/"                                 (headless / CLI fallback)
  ↓
  load Lua kernel:
    parse default.lua_master.lua
    for each listed Lua file:
      c_lua_wrap.load_and_register(path)
  ↓
  ready for first frame
```

**Windows equivalent for reference:**
```
WinMain → init_com → create_hwnd → init_wgl → wglMakeCurrent → glew_init
  → g_app.boot_from(env.AAASeed_DirStart) → message_loop
```

### 3.2 Per-frame

```
MTKView delegate -drawInMTKView:
  ↓
  AAASeedRenderer.draw()
    ↓
    g_app.update()                    ← walks c_modules tree, calls Lua MEU update() callbacks
    ↓
    cb = [commandQueue commandBuffer]
    rpd = view.currentRenderPassDescriptor
    enc = [cb renderCommandEncoderWithDescriptor:rpd]
    ↓
    GOL::Backend::set_current_encoder(enc)
    ↓
    g_app.draw()                       ← walks c_modules tree, calls Lua MEU draw() callbacks
                                         which invoke GOL::* which translate to enc method calls
    ↓
    [enc endEncoding]
    [cb presentDrawable:view.currentDrawable]
    [cb commit]
    ↓
    hot_reload_check()                 ← FSEvents stream; c_lua_wrap.reload_if_changed
```

### 3.3 Hot reload (Lua and MSL)

```
FSEvents stream callback (kFSEventStreamCreateFlagFileEvents)
  ↓
  enqueue ChangedFile(path)
  ↓
end of next frame:
  for each ChangedFile:
    if path ends in .lua:
      c_lua_wrap.find_by_path(path).reload()
    if path ends in .metal:
      MTL_library = [device newLibraryWithSource:fileContents options:nil error:&err]
      rebind affected MTLRenderPipelineState objects
```

Latency budget: < 100 ms from save to visible change. Verified per release in the regression suite.

### 3.4 Shutdown

```
[NSApplication terminate:]
  ↓
AppDelegate -applicationWillTerminate:
  ↓
  AAASeedRenderer.shutdown()
    ↓
    g_app.shutdown()                  ← walks c_modules tree in reverse, calls destructors
    ↓
    release all MTLBuffer, MTLTexture, MTLLibrary, MTLRenderPipelineState
    ↓
    release MTLCommandQueue, MTLDevice
    ↓
    confirm no outstanding MTLCommandBuffer (use [cb waitUntilCompleted] for any in-flight)
    ↓
    exit(0)
```

Clean shutdown is regression-tested via Metal validation layer (must produce zero warnings).

---

## 4. Compression and asset handling

### 4.1 Dependency archive format (Windows)

Windows dependency bundles ship as `.7z`. Mac equivalent:

- **Vendored third-party libraries** built from source via CMake or via `vcpkg` (if cross-platform).
- **Pre-built binary artifacts** distributed as `.tar.xz` (matches Unix convention; xz compression is comparable to 7z).
- Use `tar -cJf` / `tar -xJf` for archive ops. Avoid `.7z` on Mac unless `p7zip` is a dev-machine requirement.

### 4.1a Final-product DMG (end-user distribution)

The shipped artifact is `AAASeed-<version>.dmg`. Compression strategy (layered, in this order):

1. **Binary minimization** — `-flto=thin` link-time optimization (matches Windows `/GL`+`/LTCG`), `-dead_strip`, post-link `strip -x`.
2. **Shader bundle** — all `.metal` sources compiled to one `default.metallib`. Denser than 373 GLSL text files and faster cold-start.
3. **Texture compression** — built-in textures encoded to ASTC LDR. Native sampling on Apple Silicon (no decode cost), 4–8x smaller per texture.
4. **DMG container compression** — `hdiutil create -format ULMO` (LZMA). Smallest container; fallback to `-format ULFO` (LZFSE) if notarization friction.
5. **Code signing and notarization** — `codesign --deep --options runtime --timestamp`, then `xcrun notarytool submit ... --wait`, then `xcrun stapler staple`.

Reference DMG-build script (lives in `tools/build_dmg.sh`):

```bash
#!/usr/bin/env bash
set -euo pipefail
VERSION="$1"
STAGING=dmg_staging
rm -rf "$STAGING" && mkdir -p "$STAGING"
cp -R AAASeed.app "$STAGING/"
ln -s /Applications "$STAGING/Applications"
hdiutil create -format ULMO -srcfolder "$STAGING" -volname "AAASeed" "AAASeed-$VERSION.dmg"
codesign --deep --options runtime --timestamp \
  --sign "Developer ID Application: <name>" "AAASeed-$VERSION.dmg"
xcrun notarytool submit "AAASeed-$VERSION.dmg" --keychain-profile aaaseed-notary --wait
xcrun stapler staple "AAASeed-$VERSION.dmg"
```

Target sizes:
- AAASeed binary: ≤ 30 MB (Windows Metal config is ~24 MB; arm64 typically slightly larger).
- `default.metallib`: bounded by shader count and complexity; expect ~2–5 MB.
- `Contents/Resources/AAAKernel/` snapshot: bounded by Mâa's upstream content; currently a few MB.
- Final DMG: ≤ 80 MB target. Renegotiate if MEU asset libraries grow.

### 4.2 Engine-internal compression (`Src/blosc/`)

AAASeed uses **blosc** (multi-codec compressed binary store) internally for some runtime data. blosc is cross-platform, built from source on Mac. The `Src/blosc/win32/pthread.h` Windows-only thin wrapper is dropped on Mac; Mac uses system pthread.

Build steps:
```
cd third_party/blosc
cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DCMAKE_OSX_ARCHITECTURES=arm64
cmake --build build
```

### 4.3 Texture compression

- **Source textures:** PNG / JPG / TGA / EXR. Loaded via FreeImage.
- **GPU-compressed textures (PVRTC, ETC, ASTC):** AAASeed today does not pre-compress; textures upload raw. On Mac, optional optimization: convert hot textures to ASTC (Apple Silicon supports ASTC LDR/HDR natively). Defer until profiling shows texture-memory bandwidth as a bottleneck.

### 4.4 Shader compression

- **GLSL source:** plain text, ASCII, lives in `aaaseed-main/AAAKernel/Shader/`. Not compressed.
- **MSL source:** translated from GLSL (via SPIRV-Cross) or hand-written. Lives in `aaaseed-for-mac/shaders_src/*.metal` (new convention).
- **`.metallib`:** Apple's compiled binary shader library. Already compact. Bundle one `default.metallib` in `AAASeed.app/Contents/Resources/`. Reload-on-edit during development uses runtime compilation (`newLibraryWithSource:`), then writes a fresh `.metallib` for the next launch.

### 4.5 Project files (`.deproj`)

- Plain-text-with-XML-fragments format on Windows.
- Cross-platform-safe: enum params serialize by **string label**, not integer; safe across machine word size and enum reorderings.
- The Mac engine reads and writes the same format. No fork. **Test:** a `.deproj` saved on Windows opens on Mac and renders the same scene.

### 4.6 Video assets

- Source files: MP4 / MOV / AVI containers.
- **Windows decode:** DirectShow via K-Lite Codec Pack.
- **Mac decode:** `AVAssetReader` (AVFoundation). Output to a `CVPixelBuffer`, wrapped as an `IOSurface`, then bound as `MTLTexture` via `[device newTextureWithDescriptor:iosurface:plane:]` for zero-copy upload.

---

## 5. Approach to the port (the order of work)

Twelve steps, in order. Do not parallelize across steps unless they are explicitly marked parallelizable.

### Step 1 — Stand up the build skeleton (1 day)

- Create `aaaseed-for-mac/CMakeLists.txt` + `CMakePresets.json` with three macos-arm64 configs.
- Add `cmake/frameworks_macos.cmake` that does `find_library(METAL_FRAMEWORK Metal)` and friends.
- Empty target `AAASeed_mac` that links nothing yet. `cmake --build` succeeds with an empty `.app` skeleton.

**Tests:** `cmake --preset macos-arm64-debug && cmake --build --preset macos-arm64-debug` exits 0.

### Step 2 — Build the clean subsystems (2 days)

- Add `target_sources(AAASeed_mac PRIVATE ...)` calls for `Src/math/`, `Src/infrastructure/`, `Src/code_utils/` (clean files only), `Src/blosc/`, `Src/boids/`, `Src/fbx/`, `Src/image/`, `Src/draw/`, `Src/language/lua/`, `Src/aaa/`.
- Resolve MSVC-only compile errors one at a time. Use the flag-translation table in `porting.md` §4.
- Vendor `liblua51.a` for arm64.

**Tests:** static library `aaaseed_core` builds clean; `gtest`-based unit tests for `Src/math/` pass on arm64.

### Step 3 — Port the OS-specific code_utils + threading (1 day)

- POSIX impl of `aaa_dir.cpp` and `aaa_file.cpp` directory walk.
- `os_version.cpp`: `[NSProcessInfo processInfo].operatingSystemVersion`.
- `c_mutex_ct` → `std::mutex` typedef under `__APPLE__`.
- Drop `c_benaphore` from Mac build (gated).

**Tests:** file/dir round-trips, `os_version` returns a sensible string on macOS 13/14/15.

### Step 4 — Define `GOL::Backend` interface (2 days)

- Audit every `GOL::` function in `Src/gol/*.h`. List in `mindmap.md` if new ones appear.
- Promote each free function to a virtual method on `class GOL::Backend`.
- Move existing Windows implementations into `GOL::WindowsBackend : public Backend` (no behavioral change — Windows build must remain green).
- Add a global `GOL::Backend* g_gol_backend` set once at startup.

**Tests:** Windows CI still green. Mac CI gets one new test that creates a `GOL::NullBackend` and validates the interface dispatches correctly.

### Step 5 — Implement `GOL::MetalBackend` skeleton (3 days)

- New folder `aaaseed-for-mac/src/gol/metal/`.
- `metal_backend.h` declares `class MetalBackend : public Backend`.
- `metal_backend.mm` (Objective-C++ for now; switches to pure C++ with metal-cpp in Step 6) implements: device init, command-queue setup, command-buffer-per-frame, encoder lifecycle.
- Stub methods that aren't yet implemented log "not yet implemented" and return.

**Tests:** smoke test launches an empty Metal frame loop. No drawing yet; `presentDrawable` clears to red and survives 600 frames without crash.

### Step 6 — Switch Metal calls to Metal-cpp (1 day)

- Vendor Apple's metal-cpp at `third_party/metal-cpp/`.
- Replace `id<MTLDevice>` etc. with `MTL::Device*` etc. in `metal_backend.{cpp,h}`.
- Convert `.mm` → `.cpp` where Metal-cpp covers the surface. Keep `.mm` only for `MTKView` / `NSWindow` / `NSEvent` (Step 7).

**Tests:** Step 5's smoke test still passes.

### Step 7 — Mac UI host (3 days)

- `src/ui/macos/AppDelegate.mm`, `src/ui/macos/AAASeedRenderer.mm`, `src/ui/macos/MTKView host`.
- Wire MTKView delegate to call into a C++ `AAASeedRenderer::draw_frame()` member.
- Translate `NSEvent` → `c_event_*` in `src/ui/macos/event_translator.mm`.
- Wire keyboard + mouse + scroll. Defer trackpad / gestures to a follow-up step.

**Tests:** integration test launches the app, sends synthetic NSEvent via Quartz Event Services, asserts the C++ event handler receives the translated event.

### Step 8 — Shader translation (5 days, parallelizable across shaders)

- Batch-translate `aaaseed-main/AAAKernel/Shader/*.{vert,frag,geom,comp}` via SPIRV-Cross.
- Compile each `.metal` to `.air`, link to `default.metallib`.
- Manifest file lists each shader's status: `translated_ok`, `translated_with_warnings`, `hand_port_required`.
- Hand-port the top 20 hottest shaders for TBDR-aware optimizations (load action, store action, memoryless attachments where applicable).

**Tests:** for each shader, render a known input geometry on both Windows and Mac, capture frame, hash compare. Tolerance per shader documented in `tests/regression/shader_tolerance.csv`.

### Step 9 — Buffer / texture / FBO Metal paths (4 days)

- Implement `MetalBackend::create_texture`, `upload_texture` (with `.shared`, `.private`, `.memoryless` selection logic).
- Implement `create_buffer`, `update_buffer`. `.shared` for streamed, `.private` for static.
- Implement FBO equivalent via `MTLRenderPassDescriptor` (color, depth, stencil attachments).
- PBO replacement via `.shared` `MTLBuffer` direct map.

**Tests:** known-input → upload → render-to-fbo → read-back → hash compare matches Windows.

### Step 10 — Hot reload via FSEvents (1 day)

- New file `src/code_utils/fs_events_watcher.mm`.
- Wrap `FSEventStreamCreate` / `Start` / `Stop` behind `c_lua_wrap`'s existing file-watch interface.
- Verify <100 ms reload latency in a manual test.

**Tests:** integration test edits a tracked `.lua` file, confirms reload fires within 200 ms.

### Step 11 — Sample MEU end-to-end (3 days)

- Pick one MEU (e.g. MEU_Video — least graphics complexity, exercises the asset pipeline).
- Run it on the Mac binary. Compare frame output to the Windows reference.
- Iterate until matches within tolerance.

**Acceptance:** MEU_Video plays a sample video file on Mac, with the same overlay UI, at the same frame rate. Phase 3 exit criterion in `todo.md` is met.

### Step 12 — Spout → Syphon (2 days)

- Vendor Syphon-framework.
- New folder `src/SyphonSDK/`.
- Drop-in replacement at the application call sites.
- Test: Mac AAASeed publishes a Syphon frame, Mac "Simple Syphon Client" app receives it.

---

## 6. Conventions reminders

- **ASCII-only in `.cpp` and `.h`.** No em-dashes, no curly quotes. (`memory/philosophy.md` § 1.5.)
- **English-only in source artifacts.** Conversational chat may be French.
- **Coordinate names:** `x, y, z` for positions; `sx, sy, sz` for sizes. Never `width, height, cx, cy`.
- **`if` body on its own line.** Never `if(cond) stmt;`.
- **No `#ifdef _WIN32` / `#ifdef __APPLE__`** inside engine `.cpp` files. Platform divergence lives in folders.
- **`SAFE_DELETE_*` macros** instead of raw `delete` for owned heap pointers.
- **`o_str::push_name()` / `pop_name()`** for temporary `o_str` strings.
- **Include guards:** `AAA_FILENAME_H` pattern. `Src/flex/` uses the `AAA_FLEX_*` exception.
- **Class declaration:** `CLASS.DECLARE("Name", parent_class, defaults_table)` for Lua-exposed classes.
- **`if false then ... end`** blocks in Lua are preserved drafts; do not delete.

---

## 7. When you finish a step

1. Mark the step in `memory/todo.md`.
2. Append a new entry at the top of `memory/handover_session.md` describing what changed.
3. Update `memory/mindmap.md` if a subsystem boundary shifted.
4. Update `memory/porting.md` if a recipe needed to be refined.
5. Commit. Push. Watch CI on both platforms.
6. Notify Alex.

---

## 8. Emergency stops

Stop and ask Alex (do not proceed) if:

- You are about to modify a file in `aaaseed-windows/`. That is the upstream; the Mac port should rarely touch it. If a Windows-side change is needed, it must be discussed first.
- You are about to modify `err.h` or `aaa_type.h`. Both trigger full rebuilds.
- You are about to delete a `c_obj_ui` factory entry or rename an existing Lua API.
- A test you thought would pass is failing in an unexplained way for more than 30 minutes of investigation.
- A Windows CI build goes red as a side-effect of a Mac-targeted change.

The cost of pausing to confirm is low. The cost of an unwanted change to the upstream engine is high.
