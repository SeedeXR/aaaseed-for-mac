# Mindmap — AAASeed Mac Port

> Living document. Update whenever a subsystem boundary shifts, a backend choice changes, or a new dependency lands.

**Snapshot 2026-05-26 01:55 EAT** — 52/52 tests green on Apple M4. Math + code_utils + aaa-threading + infrastructure (param/factory/namer) + nsight-signpost Mac-compiled. metal-cpp vendored. MTL::Device + MTL::CommandQueue verified working from pure C++.

---

## Top-level

```
aaaseed-for-mac/                     # this is the entire project — self-contained
├── README.md                        # clone-and-build entry point
├── VENDORING.md                     # vendor/ ↔ upstream contract
├── .gitignore                       # build outputs, IDE state, Windows binaries
├── CMakeLists.txt                   # root build script (arm64, macOS 13+, C++20)
├── CMakePresets.json                # macos-arm64-{debug,release,metal}
├── cmake/
│   └── aaa_compile_flags.cmake      # force-include, include paths, XNA off
├── src/                             # Mac-native engine code (currently CMake only; impls in vendor/)
│   ├── code_utils/CMakeLists.txt    # aaaseed_code_utils static lib (11 sources)
│   ├── infrastructure/CMakeLists.txt # aaaseed_infrastructure static lib (8 sources)
│   └── math/CMakeLists.txt          # aaaseed_math static lib (6 sources)
├── tests/
│   ├── unit/                        # 52 GoogleTest tests across 7 executables
│   │   ├── sanity_test.cpp                  # 4 toolchain-only
│   │   ├── aaa_platform_test.cpp            # 8 platform-detection
│   │   ├── math_rand_test.cpp               # 5 RNG (with RAND_MAX UB fix)
│   │   ├── math_matrix_test.cpp             # 1 GLM TRS identity
│   │   ├── code_utils_test.cpp              # 11 MD5/checksum/strnum/id_unique
│   │   ├── code_utils_buffer_test.cpp       # 5 c_buffer_stream
│   │   ├── aaa_threading_test.cpp           # 10 mutex/spinlock/lock_guard (incl. multi-thread)
│   │   ├── nsight_signpost_test.cpp         # 5 NXPROFILEFUNC → os_signpost
│   │   ├── obj_ui_header_test.cpp           # 1 obj_ui.h header chain parse
│   │   ├── metal_device_test.cpp            # 3 MTL::Device, CommandQueue, Apple Silicon detection
│   │   └── test_engine_stubs.cpp            # stubs for mem::*, err, DBG_PRINT, debug_break
│   ├── integration/                 # (Coming soon) end-to-end MEU harness
│   └── regression/golden/           # (Coming soon) frame-buffer baselines
├── vendor/                          # snapshot of upstream Windows trees
│   ├── aaaseed-engine/              # C++ engine source (formerly aaaseed_exe-main)
│   │   ├── Src/                     # 766 .cpp / 1283 .h enumerated; we cherry-pick
│   │   ├── Include/                 # 3rd-party headers: glm, OpenCV, FreeImage, etc.
│   │   ├── LibSource/               # 3rd-party source
│   │   ├── cmake/                   # Windows CMake reference
│   │   ├── doc/                     # engine docs
│   │   └── ...
│   └── aaaseed-runtime/             # Lua runtime content (formerly aaaseed-main)
│       ├── AAAKernel/               # lua/, GaBu/, Shader/, Texture/, Module/
│       ├── AAAAPPs/                 # example MEU apps
│       └── AAADoc/                  # API docs (.lua + .md)
├── books/                           # reference reading (Metal, GPU on Apple Silicon, AAASeed Intro)
├── instructions/                    # human-readable instructions (links.md, instruction.md)
├── memory/                          # agent state: this file, todo, philosophy, sessions
├── third_party/                     # vendored library build artifacts
│   └── metal-cpp/                   # Apple official metal-cpp 26.4 (1.8 MB header-only, 2026-05-26)
├── shaders_src/                     # (Phase 3) .metal source → default.metallib
└── out/                             # (gitignored) build directory
```

Optional sibling directory (preserved for diff-against-upstream reference, but the
build no longer depends on it):
```
../aaaseed-windows/                  # historical reference; was the source before vendoring
```

---

## Engine architecture (canonical, both platforms)

```
c_app   (singleton g_app — global render settings + modules ownership)
  └── c_modules   (top of engine, group of c_module)
       └── c_module   (group of c_layers; lights, optional materials, multi-screen)
            └── c_layers   (sequence of up to 26 c_layer, named a..z; cameras, values, traxs)
                 └── c_layer   (one c_bdd drawer + full attribute system)
                      └── c_bdd   (the actual drawer; in Src/obj_ui/bdd/)

Above c_modules, structural depth is in the Lua side (GaBuZoMeu MEUs).
Below c_layer, the c_bdd is the leaf draw primitive.
```

**Every engine object inherits `c_obj_ui`** and exposes:
- `params` — typed control / output cells (the public interface).
- `update()` — refresh internal state for the next frame.
- `draw()` — execute as fast as possible, may run multiple times per frame.

---

## Render path (Windows current → Mac target)

```
Windows (current)
  Lua MEU calls aaa.draw.* / gol_*
       │
       ▼
  Src/gol/*        ← OpenGL isolation layer (GOL namespace)
       │
       ▼
  Src/gl/*         ← OOP modern GL resource layer (texture, FBO, VAO, UBO, SSBO)
       │           uses GOL internally
       ▼
  glew (glew32 / glew32s)
       │
       ▼
  WGL context + OpenGL driver (NVIDIA / AMD / Intel on Windows)


Mac (target — Phase 3)
  Lua MEU calls aaa.draw.* / gol_*           ← unchanged
       │
       ▼
  Src/gol/*  with GOL::Backend interface     ← abstract surface (Phase 3 step 1)
       │
       ├──► GOL::WindowsBackend (existing WGL/glew code, gated to _WIN32)
       │
       └──► GOL::MetalBackend  (new, in src/gol/metal/, .mm files)
                 │
                 ▼
            MTLDevice → MTLCommandQueue → MTLCommandBuffer → MTLRenderCommandEncoder
                 │
                 ▼
            CAMetalLayer ← hosted by MTKView ← NSWindow ← NSApplication
```

Two backends, one interface. Lua and the upper engine see only the GOL namespace and do not change.

---

## Subsystem map and Mac-compile status

Legend: ✅ compiling green on Mac · 🟡 partially compiled · 🔴 not yet started · 🟣 Win32-only (full rewrite required) · 🆕 new Mac-only · ⛔ deferred / out-of-scope v1

| Subsystem | Path in vendor | Status | Notes |
|---|---|---|---|
| **math** | Src/math/ | ✅ 6 sources | aaa_rand, aaa_quat, v_base, aaa_matrix, TMath, v.cpp (with XNA shim). billboard.cpp blocked (GL/glu.h). 7 math/* .cpp still need infra. |
| **code_utils** | Src/code_utils/ | 🟡 11 sources | aaa_util, md5c, checksum, strnum, stream_util, data_2d, asc_line, id_unique, buffer/buffer_blk, aaavr_matrix. aaa_mem.cpp / err.cpp stubbed in tests. file/, time/, os_version, console pending POSIX port. |
| **aaa (threading)** | Src/aaa/ | ✅ headers gated | c_mutex / c_mutex_recursive / c_spinlock / aaa::lock_guard work on Mac. c_mutex_ct + c_benaphore[_recursive] gated to Windows only. aaa_mutex.cpp body wrapped in `#if AAA_OS_WINDOWS()`. |
| **infrastructure** (obj + factory + param + namer) | Src/infrastructure/ | 🟡 8 sources | obj.cpp, factory.cpp, registry.cpp, param.cpp (2480 lines), params.cpp, param_data.cpp, param_def.cpp, namer.cpp. ~4500 lines compiling. obj_ui.cpp + param_focus/param_draw + connex/trax/traxs still need it. |
| **infrastructure/obj/obj_ui.cpp** | Src/infrastructure/obj/ | 🔴 Stage 1 done | Header chain parses. Stage 2 (factory + param + namer) done. **Stage 3 blocked on GOL Metal backend** for param_draw + bdd_cell_draw. |
| **infrastructure/layer/** | Src/infrastructure/layer/ | 🔴 blocked on GOL | 7 .cpp files / ~7000 LOC. layer.cpp has 15 draw/* includes. 8-12 days after GOL exists. |
| **nsight-signpost** | Src/draw/NSightEvents.h | ✅ ported to os_signpost | NVTX → Apple os_signpost real impl. NXPROFILEFUNC* macros work. Signposts visible in Instruments. |
| **image** | Src/image/ | 🔴 not started | Expected portable except GPU upload (routes through GOL backend). |
| **draw** | Src/draw/ | 🔴 blocked on GOL | Heavy GOL coupling. |
| **gol** | Src/gol/ | 🔴 designing | metal-cpp foundation ready (MTL::Device verified on Apple M4). Backend interface design in progress. |
| **gl** | Src/gl/ | 🔴 blocked on GOL | OOP modern-GL layer; depends on GOL backend. |
| **language/lua** | Src/language/lua/ | 🔴 not started | Vendor liblua51.a; bindings should port cleanly. |
| **ui** | Src/ui/ | 🟣 replace | Full rewrite in src/ui/macos/ — NSApplication + NSWindow + MTKView + NSEvent. |
| **platform/win32** | Src/platform/win32/ | 🟣 replace | Mac counterpart at src/platform/macos/. |
| **flex** | Src/flex/ | ⛔ v1 disabled | CUDA-only. Per NVIDIA→Metal doctrine, v2 ports to Metal compute. |
| **mocap/tracking** | Src/mocap/, Src/tracking/ | ⛔ v1 disabled | Kinect/FlyCapture Windows-only. AVCaptureSession for USB cams. |
| **FFGL** | Src/FFGL/ | ⛔ v1 out-of-scope | Win32 plugin format. |
| **SpoutSDK** | Src/SpoutSDK/ | 🟣 → Syphon | src/SyphonSDK/ (IOSurface-backed). |
| **OpenCL** | Src/OpenCL/ | ⛔ → Metal Compute | Per NVIDIA→Metal doctrine. Defer until a specific MEU needs it. |
| **TrueType / ftgl / SdfText** | Src/TrueType/, Src/ftgl/, Src/SdfText/ | 🔴 not started | Expected portable (cross-platform 3rd-party). |
| **blosc** | Src/blosc/ | 🔴 not started | Cross-platform; CMake build from source. |
| **boids, fbx, shaders, MSA, Gypsy** | various | 🔴 not started | Expected portable. |
| **metal-cpp vendored** | third_party/metal-cpp/ | ✅ header-only | Apple 26.4. Foundation, Metal, MetalFX, QuartzCore. |
| **gol/metal** | src/gol/metal/ | 🆕 not yet | Metal backend. Phase 3 of GOL port. metal-cpp foundation ready. |
| **ui/macos** | src/ui/macos/ | 🆕 not yet | NSApplication + NSWindow + MTKView + NSEvent (`.mm`). |
| **platform/macos** | src/platform/macos/ | 🆕 not yet | DPI, power, dynamic-load helpers. |
| **SyphonSDK** | src/SyphonSDK/ | 🆕 not yet | IOSurface texture sharing. |

---

## Runtime content (aaaseed-main/AAAKernel/)

```
AAAKernel/
├── lua/              ← 115 .lua files; aaa_*.lua glue scripts
├── GaBu/             ← Lua UI framework + GaBuZoMeu (MEU/MU/MUS layer)
├── AAA_PROTO/        ← built-in MEU prototypes
├── Shader/           ← 373 GLSL files (.vert / .frag / .geom / .comp). Cross-compile to MSL on Mac.
├── Texture/          ← built-in textures
├── Fonts/            ← font resources
├── Module/           ← module-level config + resources
├── OpenCl/           ← OpenCL kernel files (Mac: map to Metal compute)
└── Dev/AAAUser/Guest/default.lua_master.lua    ← definitive load list at boot
```

**Same content tree serves both platforms.** No fork. Mac binary reads from the same `aaaseed-main` directory the Windows binary uses; only the path-resolution differs.

---

## Build flow (Mac target)

```
[ src/CMakeLists.txt ]
        │
        ├── platform detection: APPLE → arm64 → Apple Silicon
        │
        ├── per-subsystem sources.cmake  (mirror Windows pattern)
        │
        ├── ForceInclude aaa_build_config.h via -include
        │
        ├── compile .c as C++ via set_source_files_properties
        │
        ├── compile .metal files via custom build rule
        │       .metal  →  xcrun metal  →  .air  →  xcrun metallib  →  default.metallib
        │
        ├── link frameworks: Metal, MetalKit, QuartzCore, Cocoa, Foundation, AVFoundation
        │
        ├── link static libs: liblua51.a, libopencv, libfreetype, libfreeimage, libcurl, libcpr, libassimp, libBullet
        │
        └── output: AAASeed.app/Contents/MacOS/AAASeed
                    + AAASeed.app/Contents/Resources/default.metallib
                    + AAASeed.app/Contents/Resources/AAAKernel/  (symlink or copied)
```

---

## Frame loop (Mac target — final shape)

```
MTKView delegate -drawInMTKView: (called once per display refresh)
  │
  ├── frame_begin()    ← reset per-frame state, advance time
  │
  ├── for each c_module in g_app.modules:
  │     module->update()    ← per-frame logic, Lua MEU update() calls
  │
  ├── commandBuffer = [commandQueue commandBuffer]
  ├── renderPassDesc = view.currentRenderPassDescriptor
  ├── encoder = [commandBuffer renderCommandEncoderWithDescriptor:renderPassDesc]
  │
  ├── for each c_module in g_app.modules:
  │     module->draw(encoder)    ← per-frame rendering, Lua MEU draw() calls
  │       → GOL::MetalBackend translates each gol call into MTL* calls on encoder
  │
  ├── [encoder endEncoding]
  ├── [commandBuffer presentDrawable:view.currentDrawable]
  ├── [commandBuffer commit]
  │
  └── frame_end()    ← stats, profile, hot-reload checks
```

---

## Dependency map (Mac target)

```
Open-source / cross-platform (vendored or via package manager):
  Lua 5.1, FreeType2, FreeImage, OpenCV, libcurl, cpr, assimp, Bullet,
  blosc, GoogleTest (tests), GLM (math headers)

Apple frameworks (system):
  Metal, MetalKit, QuartzCore, Cocoa, Foundation, AVFoundation,
  CoreVideo, CoreGraphics, IOSurface (for Syphon), AudioToolbox (if needed)

External Mac apps / SDKs (third-party):
  Syphon (for inter-app GPU texture sharing) — replaces Spout
  PlugData (audio host; runs as separate process, talks OSC) — same as Windows

Removed for Mac v1 (Windows-only, no equivalent):
  GLEW (replaced by direct Metal), Spout (→ Syphon), Azure Kinect (k4a),
  FlyCapture, NVIDIA Flex, FFGL, OpenVR (deferred), Direct* anything,
  K-Lite codecs (→ AVFoundation)
```

---

## Open architectural questions

1. **Shader compilation strategy.** Build-time pre-compile of all 373 shaders to `.metallib`, or runtime SPIRV-Cross translation, or hand-port? See `philosophy.md` and `porting.md` for the active argument. **Awaits Metal Toolchain install** (`xcodebuild -downloadComponent MetalToolchain`).
2. **Forward vs deferred renderer.** AAASeed today is forward (single pass). On TBDR, forward stays cheap. We do not flip to deferred just because TBDR makes it cheap; we keep forward and only revisit if a specific MEU needs many lights.
3. **Lua-versus-Swift on the host side.** No Swift in this project. Host-side code is C++17/20 and Objective-C++. Swift adds a third language; rejected for portability and binary-size reasons.
4. **Universal binary (arm64 + x86_64).** Deferred. v1 ships arm64 only.
5. **GOL::Backend interface granularity.** ~80-100 virtual methods per the scope agent. MVP starts with 10 (init/swap/clear/viewport/buffer-create/buffer-bind/texture-create/draw-elements/use-program/uniform-set) and grows as engine needs more. Open question: do we collapse legacy fixed-function calls (matrix stack, fixed lights) into the interface or substitute at the engine layer with shader uniforms?

## Upstream patch accumulation

20 patches applied to `vendor/aaaseed-engine/` (all behavior-preserving on Windows by inspection). Full audit-trail in `memory/handover_session.md`. Key real-bug fixes worth surfacing upstream to Mâa:
1. MD5 LP64 (`unsigned long` → `uint32_t`).
2. rnd_maa / rnd_gauss RAND_MAX overflow.
3. c_buffer_stream::put src-not-advanced.
4. param.cpp:1412/1434 uninitialised tmp UB.
