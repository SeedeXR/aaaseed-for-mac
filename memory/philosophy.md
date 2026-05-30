# Philosophy — How AAASeed is built on Windows, and how it should be built on Mac

> The deeper "why" behind the port. Not a how-to; a worldview. Read this when a design decision is contested.

---

## Part 1 — Understanding the Windows engine's building style

### 1.1 Param-driven design as the central nervous system

Every engine object inherits `c_obj_ui` and exposes a set of typed parameters (`c_param`). Parameters are not just "config knobs" — they are the **public interface** of the object. The UI reads/writes them. Lua reads/writes them. Project files (`.deproj`) serialize them. The animation system (`c_trax`) drives them over time.

This is a single, uniform "remote control" surface for everything. It means:
- No object needs a custom Lua binding for "exposing its state"; the param mechanism does it.
- Serialization is generic: dump the params, you have the project file.
- Animation is generic: any param can be a trax target.
- The UI (Flatland) draws params; the same UI works for every object.

**Why this matters for the port:** the param mechanism is the **stable contract** between C++ and Lua. Whatever changes on the rendering backend, the param surface must not. If a Mac-only consideration appears (e.g. Metal does not have a fixed-function light count of 8), it gets absorbed inside the implementation; the param shape stays identical.

### 1.2 GOL — OpenGL Isolation Layer

`Src/gol/` is a namespace (`GOL::`) that wraps every OpenGL call. No code outside GOL says `glDrawArrays` or `glBindTexture`. This is **uncommon** for a 30-year-old engine, and it is the single most important piece of forethought for this port. It exists explicitly because Mâa anticipated wanting to swap GL out one day.

**Why this matters for the port:** the GOL surface is already a backend abstraction in spirit. We turn it into a backend abstraction in fact by:
- Promoting `GOL::` free functions to virtual methods of a `GOL::Backend` interface.
- Keeping the existing Windows implementation as `GOL::WindowsBackend`.
- Adding `GOL::MetalBackend` in `src/gol/metal/`.

We do **not** rewrite the engine. We make GOL polymorphic.

### 1.3 Lua hot-reload as a creative tool, not a debugging tool

Every Lua file is wrapped by a `c_lua_wrap` C++ object that watches it for changes. When the file changes on disk, it re-runs. This is intentional. AAASeed is a tool for **live performance**: a VJ tweaks a Lua MEU mid-show, the file saves, the engine picks up the change at the next frame, the visual output updates. **Hot-reload is not a developer-convenience feature; it is a user-facing feature.**

**Why this matters for the port:** macOS file watching uses `FSEvents`, not `ReadDirectoryChangesW`. Wrap the OS-specific watcher behind the existing `c_lua_wrap` interface. **Latency matters**: a VJ wants < 100 ms between save and reload. `FSEvents` with `kFSEventStreamCreateFlagNoDefer` meets this.

### 1.4 Render graph of fixed depth four

`c_layer` → `c_layers` → `c_module` → `c_modules`. Above `c_modules` is the Lua side, which can compose arbitrarily deep. Below `c_layer` is the `c_bdd` leaf drawer.

This is a **performance-driven design choice**: a fixed-depth graph is cheap to walk per frame. The Lua side absorbs the "arbitrary composition" cost because Lua composition happens at setup time, not per-frame.

**Why this matters for the port:** do not "modernize" this into an arbitrary scene graph. The fixed depth is load-bearing. The same shape ports to Mac unchanged.

### 1.5 ASCII-only source, English-only docs

`Src/aaaseed_exe-main/CODE_STYLE.md` mandates 7-bit ASCII inside `.cpp` / `.h`. Reason: the Windows debug output sink writes bytes directly to a cp-1252 console; UTF-8 em-dashes render as `ÔÇö` garbage. English-only docs ensure portability for any future collaborator.

**Why this matters for the port:** macOS Terminal is UTF-8 by default, but the Mac engine's debug sink should still write 7-bit ASCII because the same `.cpp` source must compile clean on both platforms. The constraint is shared.

### 1.6 The Maa optimization stance: speed over size, debuggability where it matters

The Windows configs encode an explicit priority:
- **Metal / Wood:** maximum optimization, LTCG, no debug info — production binary for end users.
- **Release:** same optimization as Metal but no LTCG, with PDB — "Debug but faster" for reproducing bugs that only manifest at speed.
- **Debug:** `/Od` + full debug info + Edit & Continue — iteration loop.

`/Gy-` (off) is explicit: do not let the linker reorder functions, because i-cache locality between functions in the same `.cpp` matters more than the ~10% size saving.

**Why this matters for the port:** mirror this priority on Mac. Three Mac configs: `Debug` (`-O0 -g`), `Release` (`-O3 -g`), `Metal` (`-O3 -flto=thin`, no debug info). No "Wood" — Apple Silicon does not have a SIMD-fork-in-the-binary need (NEON is everywhere on ARM).

### 1.7 Static allocation everywhere, name-stack pattern for temporaries

AAASeed avoids `malloc` on the hot path. Temporary strings use `o_str::push_name()` / `pop_name()` against a pre-allocated LIFO pool. Realloc wrappers free the original on failure to keep call sites simple. `SAFE_DELETE_*` macros null the pointer after delete.

**Why this matters for the port:** unified memory on Apple Silicon makes some Windows-era allocation tricks less critical (no PCIe transfer to plan around). But the **no-allocation hot path** discipline is universal. Keep it. Do not introduce `std::vector` resizing in a fragment-shader-update path; the engine assumes preallocated pools.

---

## Provenance and verification status (added 2026-05-25)

The book content in Part 2 below is grounded in direct reading of:

- `books/AAASeed_Introduction.md` — engine concept confirmation, e.g. line 29 attribution ("created by Mâa Berriet with significant help from Franz Hildgen"), line 78 ("the core of AAASeed is an old school very robust C++ render graph"), line 82–85 (Lua access to OpenGL/GLSL/OpenCL/OpenCV/Flex/dlib/Bullet).
- `books/Metal by Tutorials (Fourth Edition).md` — direct grep-and-read for load-bearing facts:
  - Line 13119 confirms TBDR on Apple Silicon since A7.
  - Line 13228 confirms `storageMode: .memoryless` as the API form.
  - Line 2226 confirms `MTLDevice` as "the software reference to the GPU hardware device."
  - Line 13138 confirms memoryless textures stay on fast GPU memory.
- `books/GPU_Programming_on_Apple_Silicon_Using_CPP.md` — TOC confirms chapter coverage of unified memory (§2.3), M1/M2/M3/M4 GPU internals (§2.1), Metal/C++ integration (§5.3), first GPU program (§6).
- `developer.apple.com/metal/cpp/` — fetched and quoted verbatim above.

**Not verified directly:** the following pages from `instructions/links.md` are JS-rendered SPAs that WebFetch could not extract content from:
- `developer.apple.com/documentation/Metal` (got title only)
- `developer.apple.com/documentation/metalperformanceshaders` (got title only)
- `developer.apple.com/documentation/xcode/optimizing-gpu-performance` (got title only)
- `developer.apple.com/documentation/coregraphics` (not yet fetched)
- `developer.apple.com/documentation/virtualization/graphics` (not yet fetched)

Claims about these areas in the sections below are book-derived. When implementing the corresponding Metal calls, **open the Apple docs in a browser to verify the exact signatures** before committing code — do not rely on book wording as the final API.

---

## Part 2 — Understanding the Apple Silicon mental model

### 2.1 Unified Memory Architecture (UMA)

On Apple Silicon, **CPU and GPU share one physical memory pool**. The PCIe-VRAM split that a Windows discrete-GPU developer plans around does not exist.

Implications:
- **Buffer transfers are free in bandwidth** (no copy), but **not free in cache coherency**. CPU and GPU caches must be flushed/invalidated at sync points.
- `MTLStorageMode.shared` lives in the unified pool and is CPU+GPU visible. Use for streamed data (per-frame uniforms, dynamic geometry from CPU side).
- `MTLStorageMode.private` lives in the same physical pool but is marked GPU-only — the GPU can use a tighter cache policy. Use for render targets, static textures.
- `MTLStorageMode.memoryless` lives in **tile memory** only, never spilled to system RAM. Use for intermediate render targets in deferred rendering. Cannot be sampled from outside the pass.
- `MTLStorageMode.managed` is **not supported on Apple Silicon** (it exists for Intel Macs with discrete GPUs). Do not use it.

**Mental shift from Windows GL:** don't think "upload buffer to GPU"; think "share buffer with GPU, then synchronize cache." This collapses a lot of the boilerplate around staging buffers.

### 2.2 Tile-Based Deferred Rendering (TBDR)

Apple Silicon GPUs use TBDR. The screen is divided into ~16x16-pixel tiles; for each tile, the GPU:
1. Collects all primitives that touch that tile.
2. Sorts visibility (hidden-surface removal).
3. Runs the fragment shader only on visible pixels.
4. Writes to tile memory (fast, on-chip).
5. Resolves to system memory at end-of-pass.

Implications:
- **Forward rendering is cheap on TBDR** (hidden-surface removal pre-shader).
- **Deferred rendering can use `.memoryless` G-buffers** that never touch system memory — huge bandwidth savings.
- **Ping-pong passes are expensive** (each resolve to system memory wastes the tile-memory benefit). Avoid.
- **`storeAction = .dontCare` on intermediate attachments** is a real optimization, not a hint.

**Mental shift from Windows GL:** the engine today is forward and single-pass. That maps to TBDR with zero porting overhead. Do not "modernize" to deferred just because deferred is fashionable — we get the TBDR benefit either way.

### 2.3 The Metal command model

Metal exposes the GPU as an explicit command queue. Each frame:

```
MTLCommandBuffer cb = commandQueue.commandBuffer()
MTLRenderCommandEncoder enc = cb.makeRenderCommandEncoder(passDescriptor)
  enc.setRenderPipelineState(pso)
  enc.setVertexBuffer(buffer, offset, index)
  enc.setFragmentTexture(texture, index)
  enc.drawPrimitives(...)
enc.endEncoding()
cb.presentDrawable(view.currentDrawable)
cb.commit()
```

There are **no global GL-style state machine** calls. State lives in pipeline state objects (`MTLRenderPipelineState`) and is bound to the encoder explicitly. PSOs are immutable, pre-compiled, and cheap to bind.

**Implication for the port:** the GOL Windows backend uses OpenGL's state-machine model (glBindTexture, glEnable, etc.). The Metal backend translates these into PSO selection and encoder method calls. The translation is **stateful within the backend**: track current blend mode / depth state / shader, batch into a PSO at the next draw call. Cache PSOs (don't recompile per draw).

### 2.4 Metal Shading Language (MSL)

MSL is C++11-based, not GLSL. The translation patterns from GLSL:

| GLSL | MSL |
|---|---|
| `uniform mat4 viewProj;` | `constant float4x4 &viewProj [[buffer(N)]]` |
| `in vec3 position;` | `float3 position [[attribute(0)]]` inside a struct with `[[stage_in]]` |
| `out vec4 fragColor;` | `return float4(...)` from `fragment` function |
| `texture2D(tex, uv)` | `tex.sample(samp, uv)` |
| `gl_Position` | return value of `vertex` function with `[[position]]` |
| `gl_FragCoord` | `float4 position [[position]]` parameter |

Compiler: `xcrun metal` (one-step compile to `.air`), `xcrun metallib` (link `.air` files into `.metallib`). Best practice: pre-compile all shaders at build time and bundle the `.metallib` in `AAASeed.app/Contents/Resources/`.

**The 373-shader question:** SPIRV-Cross batch-translation is the fastest path to coverage. Hand-fix the dozen shaders that translate badly (geometry shaders, advanced control flow, mismatched type widths). This gets us to v1; long-term consider hand-writing MSL for hot shaders to squeeze more out of TBDR.

### 2.5 Frame pacing and display sync

`MTKView` provides a delegate `mtkView:drawInMTKView:` called once per refresh. The drawable acquired via `currentDrawable` is double- or triple-buffered. The Metal command buffer's `presentDrawable:` schedules the swap at vsync.

For ProMotion (variable 24–120 Hz) displays, the system picks the rate. Override via `view.preferredFramesPerSecond = 60` to force 60 Hz if the visual content does not benefit from 120.

**Implication:** the Windows engine's per-frame structure (a single main loop with `PeekMessage` + `update` + `draw` + `SwapBuffers`) inverts on Mac. The `MTKView` delegate **is called by Cocoa** at the right moment; you do not pump events yourself. The engine's `update` / `draw` functions run inside the delegate callback.

### 2.6 Threading on macOS — Grand Central Dispatch is optional but ubiquitous

`std::thread` / `std::mutex` / `std::condition_variable` work fine on macOS via libc++. GCD (`dispatch_queue_t`) is the platform-native concurrency mechanism and is friendlier with Cocoa main-thread requirements. For AAASeed, **stay with `std::`** to keep the threading layer cross-platform.

One caveat: Cocoa UI calls **must happen on the main thread**. The engine's main loop runs on the main thread already, so this is satisfied.

### 2.7 Metal-cpp — the official C++ binding (verified 2026-05-25)

Apple distributes Metal-cpp at <https://developer.apple.com/metal/cpp/>. Verified directly. Key facts from the official distribution page:

- **"A low-overhead C++ interface for Metal that helps you add Metal functionality to graphics apps, games, and game engines that are written in C++."**
- **"Drop in C++ alternative interface to the Metal Objective-C headers."**
- **"Direct mapping of all Metal Objective-C classes, constants, and enums to C++ in the MTL C++ namespace."**
- **"No measurable overhead compared to calling Metal Objective-C headers, due to inlining of C++ function calls."**
- Header-only. Memory management follows Cocoa/Cocoa Touch rules — see the included Read Me for ARC considerations with C++ objects.

**Requirements:**
- C++17 minimum (required for `constexpr` in `NS::Object`).
- Xcode 9.3 or later.

**Headers provided** (this is the precise surface — important):
- `<Foundation/Foundation.hpp>`
- `<Metal/Metal.hpp>`
- `<QuartzCore/QuartzCore.hpp>`

**Headers NOT provided by metal-cpp:**
- `<MetalKit/MetalKit.hpp>` — there is no MTKView equivalent in metal-cpp. MTKView remains an Objective-C class.
- `<AppKit/AppKit.hpp>` — Cocoa (NSApplication, NSWindow, NSEvent) has no metal-cpp binding.
- AVFoundation, AVCaptureSession, IOSurface — likewise Objective-C only.

**Platform availability:** identical across iOS, iPadOS, macOS, tvOS. Versioned downloads include `metal-cpp_26.zip` (current at time of writing, matches macOS 26 / iOS 19 release line) down through `metal-cpp_macOS12_iOS15.zip` for older targets.

**Backward-compatibility helper:** all `bool MTL::Device::supports...()` selectors check existence and return `false` if absent. So a build against an older deployment target can still query newer caps safely.

**Implication for the layering:**

```
.cpp files: engine + Metal-cpp calls (MTL::Device*, MTL::CommandQueue*, etc.)
             — most of the rendering backend stays in pure C++
.mm files: application shell only —
           NSApplication, NSApplicationMain, NSWindow,
           MTKView (or hand-rolled CAMetalLayer + NSView host),
           NSEvent dispatch, AVCaptureSession, IOSurface bridges
```

This is the choice for the port. The Mac graphics backend is **mostly pure C++ via metal-cpp**, with `.mm` confined to the application shell. Far better than I initially scoped (the first cut assumed all Metal calls needed `.mm`).

The CAMetalLayer object itself is reachable through QuartzCore.hpp in metal-cpp, but it's typically attached to an NSView in `.mm` code; the engine-side handle then crosses the boundary as a `CA::MetalLayer*`.

### 2.8 Xcode debugging and profiling

- **GPU Frame Capture** (Debug → Capture GPU Frame): inspects each command buffer, encoder, draw call, texture binding, shader. Indispensable.
- **Metal Validation Layer** (env: `MTL_DEBUG_LAYER=1`): catches use-after-free, format mismatches, hazards. Default ON in Debug, OFF in Metal/Release configs.
- **Instruments → Metal System Trace**: GPU utilization, command-buffer latency, memory bandwidth.
- **Instruments → Allocations**: track Metal buffer/texture allocations to catch leaks.

**Implication for the port:** wire these into the developer workflow from day one. A Mac dev session opens the project in Xcode (or VS Code + CMake), F5-launches with the Metal validation layer on, and uses GPU Frame Capture on every regression.

---

## Part 2.85 — NVIDIA / CUDA → Apple Metal substitution (added 2026-05-26)

Where AAASeed upstream uses NVIDIA or CUDA technology, the Mac port substitutes the Apple Metal / Apple frameworks equivalent rather than stubbing or no-opping. This is project-wide doctrine, not a per-feature decision.

| Upstream NVIDIA tech | Mac substitute |
|---|---|
| NVTX (Nsight CPU markers) | `os_signpost` (`<os/signpost.h>`) — visible in Instruments / Time Profiler. Done in `vendor/aaaseed-engine/Src/draw/NSightEvents.h` `#else` branch. |
| Nsight Graphics (GPU markers) | `[MTLCommandEncoder pushDebugGroup:]` — visible in Xcode GPU Frame Capture. Lands with the Metal backend. |
| CUDA compute kernels | Metal Compute (`MTLComputePipelineState` + `MTLComputeCommandEncoder`). Same threadgroup dispatch model, MSL not CUDA C++. |
| NVIDIA Flex (CUDA fluid / cloth) | Re-implement as Metal compute kernels. Big rewrite ; deferred to v2 unless a specific MEU demands it. |
| NVIDIA cuDNN | MPS (Metal Performance Shaders) and MPSGraph. |
| NVIDIA OptiX (ray tracing) | Metal Ray Tracing (`MTLAccelerationStructure` + `MTLIntersectionFunctionTable`). Apple Silicon M3+ has hardware acceleration. |
| CUDA stream / event sync | `MTLEvent` / `MTLSharedEvent` / `MTLFence`. |

**Why:** Mac users get real GPU-accelerated, profiler-visible behavior — not silently dead code. The whole point of porting to Apple Silicon is to use what Apple Silicon does best ; defaulting to stubs throws that away. Stubs are a last resort only when no Apple equivalent exists.

The companion auto-memory entry `[[feedback-nvidia-to-metal]]` carries the operational checklist : check for Apple equivalent first, port to it, log the substitution in the next handover entry.

---

## Part 2.86 — Pure Apple Metal (clarified 2026-05-26, continuation 24-26)

**Rule :** no Vulkan / SPIR-V / SPIRV-Cross / MoltenVK / ANGLE / OpenGL-on-Metal in the Mac pipeline. Engine GLSL shaders are hand-ported (or transformed by an in-repo mechanical-substitution tool that does NOT route through any IR). Runtime path : MSL source string → `MTL::Device::newLibrary(NS::String*, MTL::CompileOptions*, NS::Error**)` → `MTL::Function`.

**Why :** every translation layer in the critical path costs something. SPIRV-Cross adds a build dependency (glslang), a runtime translation cost, and an ABI surface that drifts as the Vulkan SDK iterates. Apple's compiler is the only one in the pipeline we ship with — and the only one that knows Apple Silicon's TBDR cost model end-to-end. The MoltenVK / ANGLE detours have the same flaw at a coarser grain.

**What this gives up :** automatic cross-platform shader reuse. We accept that — engine shaders go through a Mac-specific port pass. The shader corpus is bounded (~321 files) ; one-off porting is finite work.

---

## Part 2.95 — Performance profiling is embedded, not retrofitted (added 2026-05-26, continuation 26)

Every Metal backend change lands with three artifacts :

1. **CPU side** — `os_signpost_interval_begin/end` from `<os/signpost.h>` around each significant encoder build. Already integrated as the Mac branch of NSightEvents.h (which is `NXPROFILEFUNC` upstream). Subsystem identifier : `com.seedexr.aaaseed`. Visible in Instruments → Time Profiler / Points of Interest.

2. **GPU side** — `MTL::RenderCommandEncoder::pushDebugGroup(NS::String*)` / `popDebugGroup()` around each render block, and `MTL::CommandBuffer::setLabel(NS::String*)` per frame. Visible in Xcode → Debug → GPU Frame Capture. This is the Apple equivalent of NVIDIA Nsight Graphics ranges per `feedback_nvidia_to_metal.md`.

3. **Budget assertion** — CTest under label `perf` that times the path. Default budget : **16 ms wall-clock per frame in debug build on Apple M-series**. Tighter per-subsystem where appropriate (math fast-paths, single buffer uploads, etc.). A `perf` test failure blocks the porting step from being marked done in `todo.md` — same gate as a regression failure.

**Why embedded, not retrofitted :** AAASeed is real-time. A 60 Hz frame budget is 16.6 ms ; a 120 Hz Pro Display budget is 8.3 ms. If we don't measure as we build, we will land code that compiles, looks right, and silently drops frames. Profiling-after-the-fact has a much higher fix cost — the offending allocation / barrier / state-change is buried under months of subsequent work.

**Composition with NVIDIA→Metal doctrine (2.85) :** NVTX → `os_signpost` and Nsight GPU → `pushDebugGroup` are the same instrumentation surface ; they were already required by 2.85 for parity. 2.95 adds the budget assertion test so the instrumentation isn't just observable, it's enforced.

---

## Part 2.97 — Mac-primary, Windows-reciprocal (added 2026-05-26, continuation 26)

New code lands NATIVELY on Mac (Metal + Apple Silicon + Cocoa). The Windows MSVC build of the same engine source must keep working — no Mac-port change is allowed to silently break it.

**Operational rules :**
- Every Mac branch lives inside `#elif AAA_OS_MAC()` and never alters a `#if AAA_OS_WINDOWS()` body.
- Where a function genuinely needs to vary by platform (e.g. `local_stat` in `aaa_file.cpp`), the platform-neutral signature comes from a typedef or inline wrapper — NEVER a `#define name impl_name` macro alias (the `_stat64` lesson from continuation 25). Function-style macros for type aliases break C++ name-resolution at use sites.
- Includes that pull in Windows-only types (`<windows.h>`, `system/shared/SystemUtils.h`, etc.) get gated with `#if AAA_OS_WINDOWS()`.
- Pre-existing engine code that doesn't compile under AppleClang's stricter parsing gets fixed with the smallest behavior-preserving change (e.g. `unsigned char(0xFF)` → `static_cast<unsigned char>(0xFF)`), recorded as an upstream patch in the handover log.

**Why both :** AAASeed has 30 years of Windows + OpenGL DNA. Walking away from that means walking away from Mâa's real-time intuitions, the existing MEU library, and the artist community already using the engine. Mac is the new platform we make first-class, not a replacement for what already works.

**What "Mac-primary" actually means :**
- New features get designed against the Apple Silicon cost model first (TBDR-friendly, unified-memory-friendly, Metal-debug-marker-aware).
- Where a Mac native API exists for what we want, we use it directly (`Syphon` over Spout, `AVFoundation` over DirectShow, `os_signpost` over NVTX, `MTLEvent` over CUDA streams).
- Windows continues to build from the SAME engine vendor tree — Windows code paths are preserved verbatim. No "macOS rewrite of the engine ; Windows ports later". The vendor tree is bi-platform from day one.

The implication for the .app bundle : a `.deproj` saved on Mac must load on Windows, and a runtime asset bundle authored on Windows must play on Mac (modulo platform-specific MEUs gated through `meu_compat.lua`). Phase 6 in `todo.md` formalises this reciprocity.

---

## Part 2.9 — Self-contained repository (added 2026-05-25)

A new contributor cloning `aaaseed-for-mac` and running `cmake --preset macos-arm64-debug` should succeed without:
- Recursing git submodules.
- Cloning a sibling repository.
- Setting environment variables.
- Reading a setup checklist.

This is non-negotiable. The repo embeds the upstream Windows engine source under `vendor/aaaseed-engine/` and the Lua runtime under `vendor/aaaseed-runtime/`. CMake reads from these vendored snapshots by default; an `AAASEED_ENGINE_ROOT` cache override exists for the rare case of testing against an alternative tree.

**Why this is the right call:**
- AAASeed is open-source artistic infrastructure. The audience includes VJs, artists, students, researchers — not just C++ engine developers. Frictionless `git clone && build` is the only acceptable entry point.
- Submodules are notoriously fragile in this audience (forgotten `--recurse-submodules` arguments are the #1 build failure for novice users).
- Vendor snapshots make every clone byte-equivalent across machines. Disk size impact (~500 MB) is small for a media engine.

**What this requires of agents working on the port:**
- Treat `vendor/` as the engine source. Edit upstream files there, not in any sibling tree.
- Log every upstream patch in `memory/handover_session.md` with the full diff. The audit trail is how we sync back to Mâa eventually.
- Test the clone-and-build path before declaring a session done: `rm -rf out/ && cmake --preset macos-arm64-debug && cmake --build --preset macos-arm64-debug && ctest --preset macos-arm64-debug` must be green.

The `VENDORING.md` file at the repo root documents the relationship and the sync-from-upstream procedure.

---

## Part 3 — Principles for the port

### 3.1 Preserve the engine

We are porting, not rewriting. Mâa's choices (param-driven, GOL-isolated, render-graph of four, Lua-hot-reload, static-allocation pools) are not up for re-litigation. We change what we must to land on macOS, nothing more.

### 3.2 Abstract by interface, not by `#ifdef`

The only acceptable preprocessor split is at the platform boundary:
- `Src/platform/win32/` ↔ `src/platform/macos/`
- `Src/ui/` (Win32) ↔ `src/ui/macos/`
- `GOL::WindowsBackend` ↔ `GOL::MetalBackend`

Inside `Src/draw/`, `Src/math/`, `Src/infrastructure/`, `Src/image/`, `Src/language/lua/`, etc., there should be **zero** `#ifdef _WIN32` lines added by the port.

### 3.3 Leverage what Apple Silicon does best

- **Unified memory:** drop staging buffers and PBO upload-via-staging tricks. Map a `.shared` buffer, write, sync. Done.
- **TBDR:** keep the forward renderer. If a future MEU needs many lights, switch to deferred with `.memoryless` G-buffer — but only for that pass.
- **Metal-cpp:** keep the engine in C++. `.mm` only for the application shell.
- **Pre-compiled shaders:** `.metallib` bundled in the app. Runtime compile only for hot-reload of edited shaders.

### 3.4 Prefer pre-compiled over runtime

- Shaders: pre-compile at build time, runtime-compile only on hot-reload.
- Pipeline state objects: pre-create at scene load, not per-frame.
- Texture descriptors: build once, reuse.

### 3.5 No Swift

Adding Swift means a third language in the engine (after C++ and Objective-C++). Swift's ABI compatibility story is well-known but adds binary-size and toolchain cost. The Mac port stays in C++ and Objective-C++. If a Swift-only API becomes essential (e.g. SwiftUI for a settings panel), wrap it in a thin `.mm` shim.

### 3.6 No `#ifdef` storms — match the receiving codebase's style

AAASeed's existing style uses macros for type genericity (`REAL`, `INT32`, `FP32`), include guards (`AAA_FILENAME_H`), and forced includes. It does **not** use `#ifdef PLATFORM_X` inside .cpp files. Match this style on Mac. Platform divergence lives in folders, not in preprocessor branches.

### 3.7 The test pyramid is mandatory

No PR lands without:
- Unit tests for every new function in the platform layer or backend.
- Integration tests for any cross-subsystem flow you change.
- Regression frame captures for any rendering change.

A green Mac CI with red Windows CI is a failure. Both must be green.

### 3.8 Document why, not just what

When you make a non-obvious choice (use `.private` instead of `.shared` for a specific buffer; pre-compile shader X but runtime-compile shader Y), record the rationale in `porting.md` or `philosophy.md`. Future agents will read these. Save them from re-deriving your reasoning.

---

## Closing

AAASeed survives because it has a stable internal interface (params + GOL) and a flexible authoring layer (Lua + GaBu). The port respects both. The Mac binary differs from the Windows binary in **two places**: the platform shell (UI / events / windowing) and the graphics backend. Everything else is shared. That is the philosophy.

If you ever feel the urge to "improve" something outside those two places, stop and ask Alex.
