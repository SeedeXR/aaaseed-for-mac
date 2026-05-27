# Project Context — AAASeed Mac Port

**Status snapshot 2026-05-26 (continuation 26)**

| Deliverable | Status |
|---|---|
| D1 CMake + presets | ✅ `cmake --preset macos-arm64-debug` configures and builds ; 99 tests pass |
| D2 Platform-clean subsystems | 🟢 math + code_utils (incl. file/dir/time/os_version/console POSIX) + aaa-threading + infrastructure + nsight-signpost ; aaa_mem partial (continuation 26 wiring) ; err.cpp deferred on heavy deps |
| D3 GOL::Backend abstraction | 🟢 abstract interface shipped + Metal concrete implementation through it ; 5 render integration tests pass |
| D4 GLSL→MSL translator | 🟡 doctrine : pure Mac hand-port (no SPIR-V) ; 1 of 250 macro-free shaders ported and integration-tested |
| D5 Mac UI host | 🟢 NSWindow + MTKView + AAASeedMTKViewDelegate ship the triangle-rendering .app bundle ; keyboard/mouse plumbing pending |
| D6 Spout → Syphon | 🔴 not started ; Phase 5 |
| D7 Test harness | 🟡 unit (99 green) + integration (5 GolMetalRender + 2 ShaderMslPort) ; regression + perf scaffolds land continuation 26 |
| D8 Signed `.app` DMG | 🔴 not started ; Phase 8 |
| D9 Memory docs current | ✅ doctrine block at top of todo.md ; philosophy.md Parts 2.86/2.95/2.97 added ; handover up to continuation 26 |

## Platform target (clarified 2026-05-26, continuation 26)

- **Primary :** Apple Silicon (M-series, M1+). macOS 13+ deployment, arm64-only for v1. Apple Metal GPU directly — no Vulkan / SPIR-V / SPIRV-Cross / MoltenVK / ANGLE / OpenGL-on-Metal layer. See `philosophy.md` Part 2.86.
- **Reciprocal :** Windows MSVC continues to build the same engine source from `vendor/aaaseed-engine/Src/` verbatim. Mac changes live strictly inside `#elif AAA_OS_MAC()` ; Windows code paths are preserved. See `philosophy.md` Part 2.97.
- **Performance profiling :** embedded in every Metal backend change (CPU `os_signpost` + GPU `pushDebugGroup` + `perf`-labelled CTest budget). See `philosophy.md` Part 2.95.

---

## End goal

Deliver a **native macOS build of AAASeed** that:

1. Runs on Apple Silicon (M1 / M2 / M3 / M4) as a first-class `.app`.
2. **Coexists with the Windows build** without forking the engine source: shared C++ codebase with platform-specific backends.
3. Loads the **same runtime content** (`aaaseed-main/AAAKernel/`, `AAAAPPs/`, etc.) that the Windows build loads. No content fork.
4. Renders correctly on a Metal backend, replacing the OpenGL backend that exists on Windows.
5. **Cross-platform reciprocity:** an immersive experience authored on a Mac can be played back on Windows, and vice versa, with documented exceptions for hardware-bound features.
6. Is light-weight, robust, and performant on Apple Silicon (leveraging unified memory, TBDR, Metal command-buffer model).

## Concrete deliverables

| # | Deliverable | Acceptance criterion |
|---|---|---|
| D1 | `aaaseed-for-mac/CMakeLists.txt` + presets | `cmake --preset macos-arm64-metal && cmake --build --preset macos-arm64-metal` produces `AAASeed.app` |
| D2 | All platform-clean subsystems compile under Clang on Apple Silicon | `aaaseed_core_lib` static library target green; unit tests for `Src/math/` and `Src/infrastructure/` pass |
| D3 | `GOL::Backend` abstraction with `WindowsBackend` (existing) and `MetalBackend` (new) | Both backends pass a shared smoke-test suite (textured triangle, FBO round-trip, compute kernel) |
| D4 | 373 GLSL shaders translated to MSL (or runtime-translated) | Each shader compiles to `.metallib`; regression-frame comparison within tolerance |
| D5 | Mac UI host: `NSApplication` + `NSWindow` + `MTKView` + `NSEvent` → `c_event_*` translator | One end-to-end MEU (e.g. MEU_Video) plays on Mac with keyboard / mouse interaction |
| D6 | Spout → Syphon shim | A Syphon server in the Mac build pushes a texture that a Syphon-aware Mac app (e.g. Simple Client) can receive |
| D7 | Test harness: unit, integration, regression on both platforms | CI matrix (`windows-2022`, `macos-14`) green on every PR |
| D8 | Signed and notarized `.app` distributable | DMG install + first-run experience reproducible on a clean Mac |
| D9 | Documentation: `memory/` and `instructions/` kept current; per-subsystem `CLAUDE.md` updated where Mac-specific notes are needed | Any new agent session can pick up the work from these files alone |

## Evaluation criteria (the bar the project will be judged against at the end)

### Correctness
- A representative MEU runs on both platforms producing visually equivalent output (within documented per-shader tolerance).
- All Lua MEU APIs available on Windows are also available on Mac, or are stubbed with a clear `not supported on Mac v1` error and a `todo.md` entry.
- No `c_obj_ui` factory entry exists on one platform that does not exist on the other (unless explicitly opt-in).

### Performance
- 60 fps minimum on M2 / M3 / M4 for a "moderate" reference scene (defined in `tests/perf/reference_scene.lua`).
- ProMotion (120 Hz) support behind a toggle.
- Memory footprint at idle ≤ 250 MB.

### Robustness
- Survives 8-hour soak run without crash, hot-reload accidents, or VRAM leak.
- Hot-reload works on Mac via `FSEvents` with the same semantics as the Windows `c_lua_wrap` behavior.
- Clean shutdown: no GPU validation warnings, no orphaned `MTLBuffer`s, no held `id<MTLCommandBuffer>` references after exit.

### Portability and longevity
- Adding a hypothetical third platform (Linux + Vulkan) should require only a new `GOL::VulkanBackend` and a new `src/ui/linux/` — no surgery to engine subsystems above GOL.
- All Lua / shader / asset content remains a single shared tree.

### Engineering hygiene
- Zero `#ifdef _WIN32` / `#ifdef __APPLE__` spaghetti inside subsystem `.cpp` files except in the platform layer (`Src/platform/`, `src/platform/macos/`) and in the GOL implementations.
- All porting decisions traceable to a `porting.md` entry, a `philosophy.md` rationale, or a `handover_session.md` note.

## Stakeholders

- **Mâa (Mâa Berriet, per `books/AAASeed_Introduction.md` line 29)** — original author of AAASeed, owner of the Windows engine. Approval needed on changes that touch shared code or change semantics.
- **Alex Mkwizu** — owner of this Mac port effort. Email: alex@bsa.ai. Tanzanian software engineer; backgrounds in UX/UI, data viz, digital marketing; primary languages C#, Python, JS, TS. New to deep C++ engine work but driving the port. Fork created on GitLab 2026-05-20.
- **ArtCast4d.eu (European project)** — stabilized AAASeed; potential users of the Mac port.
- **AAA_Foundation (GitLab group)** — host of the open-source AAASeed under MIT license.

## What this project is NOT

- Not a rewrite. We are porting, not re-engineering. The Windows engine's design choices (param-based UI, GaBu Lua layer, `c_obj_ui` hierarchy, render-graph depth of four) are preserved.
- Not a UI redesign. The Mac UI looks the same as the Windows UI (in-Metal GaBu panels), modulo platform conventions (macOS menubar, file dialogs).
- Not a content rewrite. The 115 Lua kernel files, 373 shaders, and MEU prototypes ship unchanged.
- Not a feature expansion. Mac-only new features are out of scope until v2.
- Not a security audit. No new threat model; AAASeed remains a trusted-input artistic tool.

## Constraints

- **Time horizon:** open-ended; first usable Mac demo target Q3 2026 (working assumption — confirm with Alex).
- **Hardware:** Mac dev machine is Apple Silicon. No Intel Mac in test loop.
- **Licensing:** MIT, no GPL contamination. New third-party Mac-only deps must be MIT/BSD/Apache compatible.
- **No proprietary Apple-only frameworks beyond Metal/MetalKit/Cocoa/AVFoundation/CoreVideo/CoreGraphics/IOSurface.**
- **Lua remains 5.1** to match upstream. No upgrade to 5.4 in this port.
- **English-only in all source artifacts.** ASCII-only in `.cpp` / `.h`. (Markdown docs in `memory/` and `instructions/` are exempt.)

## What success looks like, qualitatively

At the end of the project, Mâa can hand a Mac laptop to a VJ at a venue, the VJ double-clicks `AAASeed.app`, loads a project authored on a Windows machine that morning, plugs in a projector, and runs the show. The audience cannot tell which OS the engine is running on. The frame doesn't drop. The Lua hot-reload still works when the VJ tweaks a parameter mid-set.

That is the bar.
