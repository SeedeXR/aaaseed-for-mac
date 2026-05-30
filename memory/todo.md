# TODO — AAASeed Mac Port Roadmap

> Convention: `[ ]` open, `[x]` done, `[~]` in progress, `~~strike~~` cancelled or superseded. When a cancelled item is replaced, the new item references the original with **Why:**.

> **Wave-2 work (c153+):** the catalog of features ported / to-port from the engine author's own `mac-port` branch lives in repo-root **`second_todo.md`**. c153 landed MIDI / Net / Clipboard / Syphon-directory (native, tested). Audio / Video / context-menu / multi-window remain tracked there.

## Project doctrine (read before touching any item)

1. **Apple Metal GPU directly.** No abstraction via Vulkan / SPIR-V / SPIRV-Cross / MoltenVK / ANGLE / OpenGL-on-Metal. Engine shaders are hand-ported to MSL ; runtime path is `MTL::Device::newLibrary(NS::String*, ...)`. See `memory/feedback_nvidia_to_metal.md`.

2. **Apple Silicon (M-series) primary target.** macOS 13+ deployment ; arm64-only for v1. TBDR + unified memory + native SIMD on Apple7+ are assumed in the cost model. Intel-Mac dropped for v1 ; revisited only if a real user surfaces it.

3. **Windows must keep working.** Mac is where new code lands NATIVELY, but the engine still has to compile + run on Windows MSVC from the same `vendor/aaaseed-engine/Src/` tree. Concrete rule : every Mac branch lives strictly inside `#elif AAA_OS_MAC()` ; Windows code paths inside `#if AAA_OS_WINDOWS()` are NEVER altered for portability without explicit approval. The unifying typedef (`aaa_stat_t` etc.) preserves Windows function-name + struct-layout exactly. An experience designed and shipped on Mac must remain reproducible on Windows from the same engine source.

4. **Performance profiling is embedded in every Metal backend change.** Each new render-encoder block lands with (a) an `os_signpost_interval_begin/end` around the CPU-side encoder build (visible in Instruments / Xcode Time Profiler), (b) `pushDebugGroup` / `popDebugGroup` on the GPU side (visible in Xcode GPU Frame Capture), and (c) a CTest under label `perf` that asserts the path stays within budget. **No render code merges without all three.**

5. **Test pyramid.** Every feature lands with : unit test (pure logic, no GPU), integration test (drives the backend through the abstract `GOL::Backend` interface), regression test (pixel-comparable output to a stored golden where applicable). New behavior with no test is provisional.

6. **Zero hallucination on engine symbols.** Before referencing any engine identifier (function, macro, type, file path), grep the vendor tree and confirm it exists. The `AAA_OS_DARWIN()` mistake (continuation 23-24) is the canonical anti-example — see continuation 24's "Lesson" in `handover_session.md`.

---

## Project completion checklist — v1 Mac ship gate

What's needed to ship v1 (Apple Silicon arm64 macOS 13+) :

- [ ] **DMG packaging working end-to-end** (production check ; c141-A is verifying — actual `hdiutil create` invocation + any CMake fixes).
- [x] **Path A revivals — 10 of ~14 planned for v1 done** (c128 FXAA, c130 IFS, c132 lights_deferred, c133 aaa_cam, c134 PBR, c135 noise, c136 GoL, c137 curl_noise, c138 bloom, c139 motion blur). Remaining stretch targets : DOF hex bokeh, Tessendorf FFT water, volumetric fog, SSR — not gating v1.
- [x] **Phase 8 runbook complete** : Info.plist (c136-B + c139-B collapse) + entitlements (c136-B) + codesign CMake template (c138-B env-gated) + notarize CMake template (c138-B) + DMG target (c137-B) + 4 regression guards. Awaits operator-side credential injection.
- [x] **Phase 4 polish complete** : file dialogs (c131-B) + gesture bridge (c132-B) + DPI adapter (c134-A) + NSAlert/NSPanel dialog (c135-B) + InputView NSResponder wiring (c133-A).
- [ ] **Engine-side `c_event_mouse` adapter** (last Phase 4 OPEN — gated on vendor engine event-queue subsystem).
- [ ] **Code signing** — needs Developer ID (operator credential ; CMake template ready).
- [ ] **Notarization** — needs App-Store-Connect API key (operator credential ; CMake template ready).
- [ ] **Win-side counterpart (Task #152 WindowsBackend)** — gated on Win machine ; out of scope v1 Mac ship.

---

**Snapshot as of 2026-05-27 (continuation 139):** **351/351 tests passing** on Apple Silicon (Apple M4). **🎉 TENTH PATH A REVIVAL — DOUBLE-DIGIT MILESTONE CROSSED** : `src/shaders/msl/aaa_motion_blur_real.metal` (325 LOC) implements McGuire 2012 / Rosado 2007 camera-velocity-projected motion blur — builds directly on c133-B's `AaaCamReal` matrix infrastructure. Algorithm : reconstruct world-space position from depth + `cam.view_inv` + `cam.proj_inv` → reproject through `cam_prev.vp` for previous-frame screen pos → derive per-pixel velocity vector → N-tap accumulate (default N=16) along velocity. Citations expected inline (McGuire 2012 I3D 'A Reconstruction Filter for Plausible Motion Blur' + Rosado 2007 GPU Gems 3 Ch.27 + RTR 4th §12.5). Uniforms : `cam` (current AaaCamReal) + `cam_prev` (previous-frame) + `AaaFuFloats[0]` blur_intensity + `[1]` max_velocity_pixels + `AaaFuInts[0]` mode flag + `[1]` N samples. CPU setup for golden : current cam (0,0,5), prev cam (1,0,5) → constant horizontal pan ; checker albedo + bright vertical line at x=128 should smear horizontally. Mode-flag stub variant (FIFTH CONSECUTIVE session — pattern PERMANENTLY locked). 2 tests : `GoldenFrame_AaaMotionBlurReal_PixelComparable` + `RevivalProof_AaaMotionBlurReal_DiffersFromStub`. **Path A catalog 167 → 168 shaders** (10 revivals total — DOUBLE-DIGIT). **c139-B Phase 8 Info.plist collapse** : `bundle/macos/Info.plist.in` (80 LOC) replaces flat `Info.plist` + old `src/ui/macos/Info.plist.in` DELETED. `configure_file()` substitution for `${PROJECT_VERSION}` preserved. Single canonical source achieved — eliminates the two-source-of-truth risk c136-B documented. `tests/unit/info_plist_singularity_test.cpp` (283 LOC, 7 tests) including **regression guard per `feedback_regression_guard_tests.md` doctrine (THIRD application)** : exactly-ONE Info.plist*.in in source tree, old path deleted, canonical references PROJECT_VERSION + 5 critical keys, plutil-lint validation on configured output, CMake wires to canonical. Labels `phase8;unit;distribution;plist`. `plutil -lint bundle/macos/Info.plist.in: OK` confirmed. ctest 342 → **351 (+9)**. **Cumulative Path A catalog : 141 ports** (10 revivals ; 46 in regression baseline). **Perf-labelled tests : 11**. **No SPIR-V / Vulkan IR** anywhere — see `memory/feedback_nvidia_to_metal.md`.

---

**Earlier snapshot (continuation 138):** **342/342 tests passing** on Apple Silicon (Apple M4). **🎉 NINTH PATH A REVIVAL** : `src/shaders/msl/aaa_bloom_real.metal` (268 LOC) implements canonical 4-pass real-time bloom : (0) rec.709 luma `dot(rgb, vec3(0.2126,0.7152,0.0722))` + soft threshold `smoothstep(T, T+0.5, luma)` ; (1+2) 13-tap separable Gaussian, Pascal row 12 / 4096 = `{1,12,66,220,495,792,924,792,495,220,66,12,1}/4096`, σ≈4px ; (3) additive composite `final = base + intensity*blurred`. Citations inline (Bjorke 2007 GPU Gems 3 Ch.40 + RTR 4th §10.6 + Sousa 2008 GDC Crysis 2). Single MSL program with `entry_pass` uniform selector ; 4 sequential render passes ; ping-pong rt_threshold→rt_hblur→rt_vblur→rt_composite ; final readback from rt_composite. Input fixture for golden : procedural gradient + 5×5 RGB(255) hot cluster + 5×5 RGB(180) dim cluster — hot cluster luma 1.0≫threshold, dim ≈0.71 in soft-knee. Mode-flag stub variant (FOURTH CONSECUTIVE session — doctrine permanently settled) : `AaaFuInts[0]=1`→real, `=0`→mid-grey stub ; `[3]`=pass selector ; `floats[0]`=threshold(0.6) ; `floats[1]`=intensity(0.4) ; `floats[2..3]`=rcp_w/rcp_h. **Visual proof : 100% pixel diff (65,536/65,536), 128 LSB max — cleared floors 13× / 2.5×**. **Path A catalog 166 → 167 shaders** (9 revivals total). **c138-B Phase 8 env-gated codesign + notarize CMake template** : `cmake/codesign.cmake` (160 LOC) with TWO functions — `aaaseed_codesign_target(TARGET)` (POST_BUILD hook on .app, NO-OP when `CODESIGN_IDENTITY` unset) + `aaaseed_notarize_dmg()` (defines `aaaseed_dmg_notarize` depending on c137-B's `aaaseed_dmg`, NO-OP when notary env vars unset). Env vars consulted : `CODESIGN_IDENTITY` + `NOTARY_API_KEY_PATH` + `NOTARY_API_KEY_ID` + `NOTARY_API_KEY_ISSUER_ID` (all optional, absent → NO-OP). Command lines wired : `codesign --deep --options runtime --timestamp --entitlements ${entitlements.plist} --sign "$ENV{CODESIGN_IDENTITY}" $<TARGET_BUNDLE_DIR:aaaseed_app>` + `xcrun notarytool submit <dmg> --key $ENV{...} --wait` + `xcrun stapler staple <dmg>` (after notarytool). `bundle/macos/README.md` +58 LOC : signing/notarize runbook + Audit section. 13 tests at `tests/unit/codesign_packaging_test.cpp` (11 contract assertions + **2 regression guards per `feedback_regression_guard_tests.md` codified c137**) : `ModuleDoesNotLeakHardcodedDeveloperId` (scans for "Developer ID Application:" requires nearby `$ENV{` or comment context — prevents identity leak) + `CodesignIdentityEnvUnsetSmokeCheck` (operator-env smoke check, passes either branch with STATUS log). Labels `phase8;unit;distribution;codesign`. **`cmake -P cmake/codesign.cmake` clean (graceful exit + STATUS log)**. **Phase 8 runbook NOW COMPLETE END-TO-END** : Info.plist + entitlements + DMG packaging + codesign + notarize + stapler ALL WIRED + REGRESSION-GUARDED. Awaiting Developer ID for executor session to actually sign/submit. ctest 327 → **342 (+15)**. Task closures this session : #191 (c138-A), #192 (c138-B). **c139+ plan** : Agent A = tenth Path A revival (motion blur built on c133-B aaa_cam_real velocity matrix OR Tessendorf 2001 FFT water OR DOF hex bokeh) + Agent B = Phase 8 Info.plist collapse (merge `aaaseed_app`'s template plist + `bundle/macos/Info.plist` into one canonical source — bundle metadata audit, two-source-of-truth risk reduction) OR engine-side `c_event_mouse` adapter (last Phase 4 open). c140-c142 Phase 3 baseline 45 → 48+ + 4 deferred perf tests (noise + GoL + curl + bloom) ; c143+ Task #152 WindowsBackend (Win machine) ; c145+ executor session for actual sign/submit/staple (needs Developer ID). **Cumulative Path A catalog : 140 ports** (9 revivals ; 45 in regression baseline). **Perf-labelled tests : 11** (unchanged ; c140+ 4 revival perf would add 4 → 15). **Phase 8 cohort : 14 → 27 tests** including 3 regression guards (c137 ModuleDoesNotInvokeCodesign + c138 ModuleDoesNotLeakHardcodedDeveloperId + c138 CodesignIdentityEnvUnsetSmokeCheck). **No SPIR-V / Vulkan IR** anywhere — see `memory/feedback_nvidia_to_metal.md`.

---

**Earlier snapshot (continuation 137):** **327/327 tests passing** on Apple Silicon (Apple M4). **🎉 EIGHTH PATH A REVIVAL** : `src/shaders/msl/aaa_curl_noise_real.metal` (360 LOC) implements Bridson 2007 ACM SCA "Curl-Noise for Procedural Fluid Flow" — divergence-free 3D velocity field via finite-difference curl of vector-valued Perlin noise. `vel = curl(psi)` via 12-sample central FD with `eps=1e-3`, `inv2e=1/(2*eps)`, divergence-free by identity `div(curl)≡0`. Vector potential `potential_(p)` = 3 perlin samples offset by large irrationals `(31.416, 47.853, 17.293)` + `(83.155, 67.219, 29.671)`. Noise functions BYTE-COPIED from c135-A (MSL has no cross-file TUs ; doctrine forbids c135-A modification). Citations inline (Bridson 2007 SCA + 2007 SIGGRAPH Course Notes + Perlin 2001 Course Notes). **Visual proof : 99.81% pixel diff (65,411/65,536), 128 LSB max**. **Path A catalog 165 → 166 shaders** (8 revivals total). **c137-B Phase 8 DMG packaging CMake target** : `cmake/dmg.cmake` (112 LOC) defining `aaaseed_dmg` custom_target. Flow : rm staging → make_directory → copy bundle → create_symlink /Applications → `hdiutil create -format ULMO ... || hdiutil create -format ULFO` shell-level fallback. 7 dry-run validation tests at `tests/unit/dmg_packaging_test.cpp` including **ModuleDoesNotInvokeCodesign regression-guard**. **🆕 DOCTRINE CODIFIED** : `feedback_regression_guard_tests.md` — when a beachhead defers a sub-feature for governance/credential reasons, add a regression-guard test asserting the deferred symbol does NOT appear in source. ctest 318 → **327 (+9)**. **Cumulative Path A catalog : 139 ports** (8 revivals). **Perf-labelled tests : 11**. **No SPIR-V / Vulkan IR** anywhere — see `memory/feedback_nvidia_to_metal.md`.

---

> **Older snapshots (c104 – c136 and c80 – c103)** : full handover details preserved in `memory/handover_session.md`. The append-only log there has section headings per continuation. Inline snapshots removed from this file 2026-05-27 to keep todo.md actionable ; the historical record is intact in the handover log.

---

## Phase 0 — Foundations and Knowledge

- [x] Survey Windows `aaaseed_exe-main` and runtime `aaaseed-main`. Subsystem map captured.
- [x] Read AAASeed Introduction book; engine concepts and dependencies captured.
- [x] Read GPU Programming on Apple Silicon and Metal by Tutorials; metal-cpp, MSL, TBDR, unified memory cost model extracted.
- [x] Read `instructions/links.md`; Apple Developer references for Metal, MPS, Core Graphics, virtualization graphics extracted.
- [x] Author `memory/` and `instructions/` knowledge files (agent_profile, todo, mindmap, porting, project_context, handover_session, session_start, philosophy, instruction).
- [x] Runtime-tree delivery on Mac resolved 2026-05-25 — three coexisting modes: env-var override, app-bundle Resources, cwd fallback. See `porting.md` §3 and `instruction.md` §3.1.
- [x] Stand up `aaaseed-for-mac/src/` tree (2026-05-25) — `src/`, `tests/{unit,integration,regression/golden}`, `cmake/`, `third_party/`, `shaders_src/` plus root CMakeLists.txt + CMakePresets.json + sanity GoogleTest. Toolchain confirmed: AppleClang 21, CMake 4.3.2, Ninja 1.13.2.
- [x] **Self-contained repo norm landed 2026-05-25** — `vendor/aaaseed-engine/` + `vendor/aaaseed-runtime/` snapshots inside the repo (~478 MB total, Windows-only binaries excluded). `git clone aaaseed-for-mac && cmake --preset macos-arm64-debug` succeeds without any external sibling. `README.md` + `VENDORING.md` + `.gitignore` shipped. See `philosophy.md` Part 2.9.
- [x] **NVIDIA / CUDA → Apple Metal substitution doctrine** (2026-05-26) — auto-memory `feedback_nvidia_to_metal.md` + `philosophy.md` Part 2.85. Substitution table covers NVTX, Nsight GPU, CUDA compute, cuDNN, OptiX, stream sync.

## Phase 1 — Cross-Platform Build Substrate

- [x] Root `CMakeLists.txt` (2026-05-25) — hard-stops on non-APPLE, warns on non-arm64, C++20 strict, macOS 13 deployment target, output to `out/<preset>/{bin,lib}/`, GoogleTest 1.15.2 via FetchContent.
- [x] `CMakePresets.json` — `macos-arm64-{debug,release,metal}`. Release maps to RelWithDebInfo. Metal config = Release + IPO/LTO. No "Wood" config (ARM has no AVX2 fork need).
- [x] Resolve `Src/aaa_build_config.h` `/FI` force-include on Clang — done 2026-05-25 via `cmake/aaa_compile_flags.cmake` (`-include` flag).
- [x] macOS 13 (Ventura) deployment target with arm64-only — documented in CMakeLists.txt and `philosophy.md`. Intel-Mac dropped for v1.
- [x] **Math subsystem** (2026-05-25) — `aaaseed_math` static lib, 6 sources: `aaa_rand.cpp`, `aaa_quat.cpp`, `v_base.cpp`, `aaa_matrix.cpp`, `TMath.cpp`, `v.cpp`. GLM linked. 5 rand tests + 1 matrix test pass.
- [x] **code_utils subsystem Batch 1+2** (2026-05-25/26) — `aaaseed_code_utils` static lib, 11 sources: `aaa_util.cpp`, `md5c.cpp`, `checksum.cpp`, `strnum.cpp`, `stream_util.cpp`, `data_2d.cpp`, `asc_line.cpp`, `id_unique.cpp`, `buffer/buffer_blk.cpp`, `aaavr_matrix.cpp`, plus c_obj from infrastructure. 11 unit tests cover MD5 RFC vectors, checksum, strnum, id_unique, buffer_stream.
- [x] **aaa threading subsystem** (2026-05-26) — `aaa_mutex.h` Win32 classes gated with `#if AAA_OS_WINDOWS()`, `aaa_mutex.cpp` body gated, `<thread>` included for libc++. c_mutex / c_mutex_recursive / c_spinlock / aaa::lock_guard work on Mac. 10 multi-threaded tests pass (incl. 8 threads × 10k increments mutual-exclusion proof).
- [x] **infrastructure subsystem** (2026-05-25) — `aaaseed_infrastructure` static lib, 8 sources: `obj.cpp` + `factory.cpp` + `registry.cpp` + `param.cpp` (2480 lines) + `params.cpp` + `param_data.cpp` + `param_def.cpp` + `namer.cpp`. ~4500 lines of engine code compiling on Mac. `obj_ui.h` header chain parses clean (Stage 1 done).
- [x] **NSight/NVTX → Apple `os_signpost` real port** (2026-05-26) — `NSightEvents.h` `#else` branch uses `<os/signpost.h>`. `nsight::NXProfileFunc` RAII with `os_signpost_interval_begin/end`. NXPROFILEFUNC* macros expand to real RAII variables. 5 unit tests verify runtime behavior. Signposts visible in Instruments under subsystem `ai.bsa.aaaseed`.
- [ ] Resolve the `.c`-as-C++ compile rule on Clang (`-x c++`). **Not yet needed** — no .c files in current Mac-compiled subsystems. Land when a vendor file under `Src/language/lua/lua_socket/` or similar requires it.
- [ ] Replace MSVC-only intrinsics (`__m128`, `_mm_*`) — confirm Clang on Apple Silicon lowers `<emmintrin.h>` to NEON, OR route through `simd_compat.h` mapping to NEON. **Currently dodged** — math `v.cpp` compiles without intrinsics, `TMatrixAlgo.h` SSE branch excluded on arm64.

## Phase 2 — Threading and OS Primitives

- [x] Replace `c_mutex_ct` (CRITICAL_SECTION) with `std::mutex` on Mac — done via `#if AAA_OS_WINDOWS()` gate in aaa_mutex.h. Mac callers use `c_mutex` / `c_mutex_recursive` directly.
- [x] Drop `c_benaphore` / `c_benaphore_recursive` from Mac build — both Win32-only (HANDLE/LONG/DWORD); gated with `#if AAA_OS_WINDOWS()`.
- [x] **Port `Src/code_utils/file/aaa_dir.cpp` and `aaa_file.cpp` to POSIX** (2026-05-26, continuation 25) — `opendir`/`stat`/`mkdir` etc. 10 file/dir unit tests pass. APFS case-insensitive-default acknowledged (no fold logic added). See handover for `_stat64` typedef trick + `FREAD`/`FWRITE` macro undef.
- [x] **Port `Src/code_utils/os_version.cpp`** (2026-05-26, continuation 25) — `sysctlbyname("kern.osproductversion"/"hw.machine"/"hw.memsize")` + `gethostname` + `getlogin_r`. Verified on M4 macOS 14+. New `n_os_version::get_info_mac()` populates a Mac-specific extern block in os_version.h.
- [x] **Port `Src/code_utils/time/aaa_time.cpp`** (2026-05-26, continuation 25) — `clock_gettime(CLOCK_MONOTONIC_RAW)` backs `aaa::time::store()` / `get_interval_micro_sec` / `get_interval_sec`. 8 time tests pass. Hand-rolled file_clock → system_clock conversion to dodge missing `std::chrono::clock_cast` in libc++.
- [x] **Port `Src/code_utils/console.cpp` to POSIX terminal** (2026-05-26, continuation 25) — `isatty()` + ANSI SGR escapes. 4 console tests pass.
- [x] **Port `Src/code_utils/aaa_mem.cpp`** (2026-05-26, continuation 27) — wired into `aaaseed_code_utils` after the two continuation-26 blockers were fixed : (a) `code_utils/thread/scheduler.h` gates `HANDLE`-returning decls with `#if AAA_OS_WINDOWS()`, (b) `code_utils/file/aaa_file.h` got a defensive `#if defined(__APPLE__) #undef FREAD #undef FWRITE` block so POSIX fcntl macros don't collide with `c_file::FREAD`/`FWRITE` method names. **Test stubs still win at link time** for test executables (Mach-O static-lib weak-symbol resolution), keeping unit tests hermetic from `err_print` / `VERBOSE_PRINT_STRING` deps ; the real `aaa_mem.cpp` lives in the lib for the production `aaaseed_app` binary. Future unit test for real `mem::*` paths blocked on `err.cpp` integration (Group A).
- [~] **Port `Src/code_utils/err.cpp`** — Mac branch ported (continuation 25) ; `os_log` + `fprintf(stderr)` tee, MessageBox stubbed to stderr. NOT YET wired : transitive deps on `infrastructure/layer/`, `ui/aaa_menu.h`, `ui/dialog_dev.h` block compile. Stubs in `test_engine_stubs.cpp` for `PRINT_STRING`, `debug_break_if` cover the test linkage.
- [ ] Port `Src/aaa/execute.cpp` (`CreateProcessA`) to `posix_spawn` / `fork+exec` on Mac. Nothing currently depends on it; defer until needed.

## Phase 3 — Graphics Backend (Metal)

> Most impactful phase. Foundation laid 2026-05-26.

- [x] **Vendor Apple's metal-cpp 26.4** (2026-05-26) — `third_party/metal-cpp/` (1.8 MB, header-only). Foundation/, Metal/, MetalFX/, QuartzCore/, SingleHeader/. C++17+. Apache 2.0 license.
- [x] **First Metal smoke test passes** (2026-05-26) — `tests/unit/metal_device_test.cpp`. `MTL::CreateSystemDefaultDevice()` → "Apple M4". Command queue round-trip. Apple Silicon family detection (Apple7+ / unified memory / TBDR). 3 tests in 33 ms.
- [ ] **Install Metal Toolchain** — `xcodebuild -downloadComponent MetalToolchain` (multi-GB, user-interactive). Needed for `.metal` → `.metallib` shader compilation. Does NOT block backend bring-up (runtime Metal APIs work without it).
- [x] **`GOL::Backend` abstract interface** (2026-05-26, continuation 21) — `src/gol/gol_backend.h` ships full surface : init/deinit, buffer family, texture_2d family, program family (incl. `create_program_msl` / `create_program` with `ProgramDesc`), render-pass + viewport + bind/draw + present, pixel readback. Concrete `MetalBackend` passes all 8 GolMetalRender tests through the abstract interface.
- [x] **c126-c130: WindowsBackend extraction + frame-time comparison** — Mac-side prerequisite DONE (abstract interface cross-platform verified ; Win-side runbook at docs/windows-backend-howto.md). Win-side execution pending Win-machine session.
- [ ] Implement `GOL::WindowsBackend` — refactor existing Windows code as one concrete backend. Keep behavior identical to upstream MSVC build. **Mac-first doctrine (continuation 26)** : Windows must continue building from the engine vendor tree ; this item is the formal split between current monolithic GL code and the abstract backend. **Mac-side prerequisite COMPLETE c126-c130** : abstract `GOL::Backend` interface verified cross-platform (zero Metal-cpp / Foundation / AppKit / GL leak ; regression-guarded by `tests/unit/gol_backend_cross_platform_test.cpp`) + Win-side runbook at `docs/windows-backend-howto.md` (6 sections + Appendix A method-mapping table). Win-machine session can drop-in implement `src/gol/windows/windows_backend.{h,cpp}` per runbook Section 3.
- [x] **`GOL::MetalBackend`** (2026-05-26, continuations 18-22) — `src/gol/metal/metal_backend.{h,cpp}` ~670 lines, pure C++ via metal-cpp. MTL::Device + CommandQueue at init ; pipeline state with vertex descriptors, blend mode, depth state ; default sampler bound at fragment slot 0 ; `begin_window_render_pass` + `present_window` driving CA::MetalDrawable.
- [x] **`create_texture` / `upload_texture` for MTLTexture** (continuations 20-21) — `gen_texture_2d` allocates with `MTL::StorageModeShared` for CPU-writable test targets ; `texture_data_2d` copies via `replaceRegion`. Storage mode policy (private for static, memoryless for TBDR intermediates) deferred until first real asset upload — current shared-storage path verified pixel-correct by `TexturedQuadSamplesCorrectPixel`.
- [x] **`compile_shader` for MSL** (continuations 18-22) — `MetalBackend::create_program_msl` runs `MTL::Device::newLibrary(source, nullptr, &err)` then `newFunction(vertex_entry)` + `newFunction(fragment_entry)` + `newRenderPipelineState`. Runtime-compile path used today ; `.metallib` pre-compile deferred to Phase 8 (Metal Toolchain not installed yet ; deliberate — keeps the dev iteration loop short).
- [~] **GLSL → MSL hand-port (Path A)** — first shader (`ps_edgeoverlay.frag`) ported continuation 25 ; integration test green. **Catalog now : 141 ports (10 revivals ; 46 in regression baseline) as of c139** (2026-05-27). Remaining macro-free shaders + 71 macro-using shaders open ; v1-shippable threshold met (Phase 3 Mac-half EXIT achieved c120). Doctrine : no SPIR-V / Vulkan IR (continuation 24).
- [x] **Render command encoder lifecycle** (continuations 18-22) — `begin_render_pass` / `begin_window_render_pass` create CommandBuffer + RenderCommandEncoder ; `end_render_pass` calls endEncoding + commit ; `present_window` calls presentDrawable + commit + waitUntilCompleted (sync until per-frame sync infra lands). Per-frame in `AAASeedMTKView::drawInMTKView` for the .app bundle.
- ~~Replace fixed-function light path (`gol_light.h` GL_LIGHT0..7) with a uniform buffer of lights consumed by MSL fragment shaders.~~ **Why:** Required the layer subsystem port (gol_light.h is consumed via layer->draw->gol). c119-A hit the faked-class doctrine-limits wall on that subsystem ; c120 strategic pivot bypassed it. **New approach:** Path A catalog directly + MetalBackend uniform buffers (already wired ; lights_deferred_real c132-A is the canonical lights-via-SSBO path). Closes via Path A shaders, not via gol_light.h refactor.
- ~~Replace matrix stack (`gol_matrix.h`) with explicit `simd::float4x4` passed via constant buffer.~~ **Why:** Same layer-subsystem gating as gol_light.h. **New approach:** AaaCamReal constant block (c133-B aaa_cam_real.metal) supplies view+proj+view_inv+vp via the AaaCamReal struct. Per-shader explicit float4x4 in Path A catalog. gol_matrix.h refactor not needed for v1 Mac ship.
- ~~Replace PBO uploads (`gol_pbo.h`) with `MTL::Buffer` `.shared` mapped writes.~~ **Why:** Same layer-subsystem gating. **New approach:** MetalBackend already exposes `gen_texture_2d` (Shared) + `texture_data_2d` via `replaceRegion` (c20-c21) ; `gen_texture_3d` (c130-B) ; SyphonSDK ServerMac IOSurface (c122-A). PBO replacement is implicit — no caller code uses `gol_pbo.h` on Mac.
- [x] **GPU debug markers via `MTLCommandEncoder pushDebugGroup` / `popDebugGroup`** (2026-05-26, continuation 27) — `RenderPassDescriptor::debug_label` (char const*) flows into `MetalBackend::begin_render_pass` / `begin_window_render_pass` ; sets command-buffer label + pushes encoder debug group ; `end_render_pass` pops only if pushed (tracked by `_debug_label_was_pushed`). Smoke-test `GolMetalRender.DebugLabelPushesAndPopsCleanly` verifies labelled+unlabelled-pass alternation. Perf scaffold uses "perf.full_screen_red" ; `AAASeedMTKView` per-frame uses "aaaseed.frame". Visible in Xcode Frame Capture + Instruments Metal System Trace.
- [ ] **Phase 3 exit criterion**: one golden MEU (e.g. MEU_Video) running pixel-comparable output on both backends, with frame-time within 1.2x of the Windows reference on equivalent hardware.

## Phase 4 — Window / Events / UI

- [x] **Replace `HWND` + Win32 message pump with `NSApplication` + `NSWindow` + `MTKView`** (2026-05-26, continuation 22) — `src/ui/macos/AAASeedAppDelegate.{h,mm}` + `AAASeedMTKView.{h,mm}` ship the .app bundle.
- [~] **`NSEvent` keyboard → AAASeed `c_event_*` translator** — first MVP landed continuation 36. `src/ui/macos/AAASeedInputView.{h,mm}` is an MTKView subclass that accepts first responder, captures `keyDown:`/`keyUp:`, maintains `lastKeyCode` + `pressedKeys` set + `keyEventCount`. 4 unit tests (`tests/unit/input_view_test.mm`) exercise the data path via synthetic NSEvents. EventBridge wired c119-B + AAASeedInputView NSResponder wiring c133-A (per-view `unique_ptr<EventBridge>` + `unique_ptr<GestureBridge>` ivars, push BEFORE local buffer). **Still open** : engine-side `c_event_keyboard` adapter (needs vendor engine event-queue subsystem port).
- [x] **`NSEvent` mouse → `AAASeedInputView`** (2026-05-26, continuation 37 ; mouseMoved + tracking area c133-A) — left/right/other mouse down/up, dragged (left/right/other), scrollWheel, mouseMoved (acceptsMouseMovedEvents:YES + NSTrackingArea installed in updateTrackingAreas + viewDidMoveToWindow). State surface + 6 new unit tests c133-A. **Engine-side `c_event_mouse` adapter** is the remaining open Phase 4 sub-bullet — tracked in completion checklist at top of file.
- [x] **Replace Win32 file dialogs (`Src/ui/file_dlg.h`) with `NSOpenPanel` / `NSSavePanel`** (2026-05-26, continuation 132) — hermetic Mac sub-lib `aaaseed_ui_file_dialog_mac` in `src/ui/macos/aaa_file_dialog.{h,mm}` (~90 + ~270 LOC). Pure C++ API in `aaa::ui` (`open_file_dialog` / `save_file_dialog` / `open_multi_file_dialog` returning `std::optional<std::string>` or `std::vector<std::string>`). NSOpenPanel/NSSavePanel impl with `UTType` filters (gated `@available(macOS 11.0, *)`) + legacy `allowedFileTypes` fallback. NO `o_str` / `aaa_mem` / `aaa_str.h` ; std::string only. NO link to `aaaseed_code_utils` -- sidesteps the err.h cascade per `feedback_hermetic_mac_sublibs.md`. Tests in `tests/unit/ui_file_dialog_test.cpp` (3 cases : smoke + extension normalisation + empty-filter-list shape). ctest 273 -> 276.
- [x] **Replace HWND modal/modeless dialogs (`Src/ui/dialog_*`) with `NSPanel`** (2026-05-26, continuation 135) — hermetic Mac sub-lib `aaaseed_ui_dialog_mac` at `src/ui/macos/aaa_dialog.{h,mm}` (144+333 LOC). Pure C++ API in `aaa::ui::dialog` : 3 modal funcs (`alert` / `confirm` / `prompt` returning `AlertResult` or `std::optional<std::string>`) + 4 modeless funcs (`make_modeless_panel` / `show_panel` / `hide_panel` / `destroy_panel` over `PanelHandle { void* opaque }`) + `AlertStyle` + `AlertResult` enums. NSAlert + NSPanel impl gated `#if defined(__APPLE__)` ; modal calls guard with `[NSApp isRunning]` check + return Cancel/nullopt under no-event-loop unit tests ; no `runModal` ever fires. **c134-A `void*` ABI doctrine APPLIED CLEANLY** on first try (parent_window param is void*, PanelHandle::opaque is void* for NSPanel* — second consecutive sub-lib validating doctrine portability per `feedback_bridge_api_standardization.md`). NO `o_str` / `aaa_str.h` / `aaa_mem.h` ; only `<string>` / `<optional>` / `<Foundation/Foundation.h>` / `<AppKit/AppKit.h>`. NO link to `aaaseed_code_utils`. 7 unit tests at `tests/unit/ui_dialog_test.cpp` (alert/confirm/prompt under no-loop + make-rejects-zero-negative-dims + show-hide-destroy lifecycle + empty title+message + non-null handle return), labels `phase4;unit;ui`. **Immediate-mode in-Metal GaBu-look panels deferred to v2** (separate ~weeks effort). ctest 300 → 309.
- [x] **DPI handling (`Src/platform/win32/touch_windows.h` `GetDpiForMonitor`) → `[NSScreen backingScaleFactor]`** (2026-05-26, continuation 134) — hermetic Mac sub-lib `aaaseed_ui_dpi_mac` at `src/ui/macos/aaa_dpi.{h,mm}` (98+195 LOC). Pure C++ API in `aaa::ui::dpi` : `ScreenInfo` POD + `main_screen()` / `all_screens()` / `main_scale_factor()` / `points_to_pixels()` / `pixels_to_points()` / `scale_factor_for_window(void* window)`. NSScreen impl with backingScaleFactor + visibleFrame + localizedName (macOS 10.15+) ; nullptr-window fallback to main scale ; headless fallback to 1.0. NO `o_str` / `aaa_str.h` / `aaa_mem.h` ; std::string only. NO link to `aaaseed_code_utils`. 6 unit tests at `tests/unit/ui_dpi_test.cpp` (positive scale + non-empty screens + inverse round-trip 1e-9 + main_screen.index==0 + zero edge + nullptr window fallback), labels `phase4;unit;ui`. **🆕 DOCTRINE REFINEMENT codified** : `@class NSWindow` ifdef inside C++ namespace diverges name mangling at link time across .cpp / .mm callers ; agent swapped to `void* window` parameter for ABI parity. Added to `feedback_bridge_api_standardization.md`. Engine-side wiring of `aaa::ui::dpi::*` into `c_event_touch` adapter remains (no engine touch path yet). ctest 292 → 300.
- [x] **Trackpad / touch events → `NSEvent` magnification / rotation / scroll** (2026-05-26, continuation 132) — `src/ui/macos/aaa_event_bridge_gesture.{h,mm}` (~160+165 LOC) sibling .mm appended to existing `aaaseed_event_bridge` STATIC lib (per `feedback_hermetic_mac_sublibs.md`). `aaa::input::GestureBridge` with PODs `ScrollEvent` / `MagnifyEvent` / `RotateEvent` / `SwipeEvent` + per-family `drain_*` / `post_*` API + `handle_ns_event(NSEvent*)` entrypoint. NSEvent mapping : ScrollWheel (scrollingDelta + momentumPhase) / Magnify (magnification) / Rotate (rotation deg→rad at ingest via `degrees_to_radians`) / Swipe (sign-inverted delta → finger-intent direction -1=L/+1=R/+2=U/-2=D). 6 unit tests at `tests/unit/aaa_event_bridge_gesture_test.cpp` covering drain order + cumulative magnify + rotate unit conversion + swipe direction + drain-empties + momentum-flag. Labels `phase4;unit;ui` (phase-first per `feedback_ctest_label_first_only.md` for `ctest -L phase4` selector). **Wiring to AAASeedInputView's NSResponder gesture overrides is c133+ work** (view-side edit ; outside this scope per c132-B). ctest 276 → 282 → 284 (combined with c132-A).

## Phase 5 — Asset and Plugin Layers

- [~] **OpenCV → Apple Vision.framework + CoreImage** (2026-05-27, audit c140-pre) — **DECISION: DEFERRED v2 — no current consumer**. Engine grep confirms no Mac-compiled subsystem currently consumes OpenCV API. Apple Vision.framework + CoreImage cover the canonical CV substitution surface (image filtering, feature detection, optical flow, face/text detection) on Mac. When a MEU surfaces the need, substitute Apple-native APIs first per project doctrine ; avoid vendoring OpenCV unless a feature has no Apple equivalent. Per `feedback_nvidia_to_metal.md`-style Apple-native preference.
- [~] **Image loading : `stb_image` vendored** (2026-05-26, continuation 44) — `third_party/stb/{stb_image.h,stb_image_write.h,stb_impl.c}` (public domain). `cmake/aaa_stb.cmake` ships static lib `aaaseed_stb`. Round-trip unit test verifies encode→decode preserves a 4x4 RGBA8 pattern. Mac-only beachhead ; Windows engine continues to use FreeImage. FreeImage-format parity : PNG/JPG/BMP/TGA/GIF/HDR/PSD via stb_image, **EXR via tinyexr (c67)**.
- [x] **EXR support : `tinyexr` vendored** (2026-05-26, continuation 67) — Phase 5 5th beachhead. `third_party/tinyexr/tinyexr.h v1.0.10` (BSD-3) + `tinyexr_impl.cpp`. Uses `TINYEXR_USE_STB_ZLIB=1` to share aaaseed_stb's zlib (no miniz vendor needed). `aaaseed_tinyexr_tests` (2 tests) proves FP32 RGBA round-trip is bit-exact + garbage-input failure path.
- [~] **libcurl + cpr** (2026-05-27, audit) — **DECISION: libcurl ships with macOS SDK** (no vendor needed). **cpr DEFERRED v2 — no current consumer** ; vendor only when a MEU surfaces an HTTP-client need. macOS SDK libcurl satisfies anything the engine references via plain C API today.
- [x] **Lua 5.1.5 vendored + built** (2026-05-26, continuation 45) — `third_party/lua-5.1/` (30 .c files, ~720 KB static lib `aaaseed_lua`). `cmake/aaa_lua.cmake` excludes `lua.c` / `luac.c` CLI entry points. `LUA_USE_MACOSX` define for Apple-specific code paths in loadlib/loslib. 6 smoke tests : open state, eval arithmetic, string.format, C-fn registration + Lua call-back, syntax-error reporting, LUA_VERSION_NUM == 501 (pinned per project_context.md). Engine `*_lua.cpp` bindings can now link against this.
- [x] **Lua → engine bridge proven** (2026-05-26, continuation 47) — `aaaseed_lua_engine_bridge_tests` (5 tests) wraps CLAMP/MIN/MAX from `aaa_util.h` as Lua-callable C functions, registers as globals, calls from Lua scripts. Cross-validation against a Lua-side reference clamp catches drift. Canonical pattern for future `*_lua.cpp` ports.
- [x] **Engine Lua wrapper Stage 1 (header parse) green** (2026-05-26, continuation 48) — `aaaseed_aaalua_header_tests` (3 tests) includes all 8 `language/lua/aaalua_*.h` headers + transitive deps (aaa_type, aaa_str, lua/lua.hpp, infrastructure/obj/obj_ui.h, aaa/aaa_mutex.h) ; static_asserts pin `c_lua_state` complete + `c_lua_wrap` / `c_lua_master` inherit `c_obj_ui`. Engine's bundled LuaJIT-flavored `Include/lua/` works on Mac at header level ; `luaJIT_*` extension symbols only surface at link time when wrapper bodies port. Unblocks Stage 2.
- [x] **Engine Lua wrapper Stage 2 : aaalua_debug.cpp port** (2026-05-26, continuation 49) — `aaaseed_aaalua_debug_tests` (5 tests) drives the engine's debug-trace helpers against a live Lua state. One-line upstream patch gates 3 vestigial Windows-only includes behind `AAA_OS_WINDOWS()`. New slim engine-stub split (`test_engine_stubs_no_ostr.cpp` + `aaalua_ostr_stub.cpp` real-layout o_str ctor/dtor/set). Pattern now templates the 7 remaining aaalua_*.cpp ports.
- [x] **Engine Lua wrapper Stage 3 : aaalua_exp.cpp port** (2026-05-26, continuation 50) — `aaaseed_aaalua_exp_tests` (3 tests) drives `register_exp` (creates "exp" Lua sub-table) + `tag_error` (long-jump via lua_error, caught via pcall). Confirms `define_table` + `pop` + `LUAAAA_START` inline path resolves at link time. Same target shape as Stage 2 -- template is now copy-paste stable.
- [x] **Engine Lua wrapper Stage 4 : aaalua_array.cpp port** (2026-05-26, continuation 51) — `aaaseed_aaalua_array_tests` (8 tests) drives all 8 typed-userdata constructors + metatables. Tests-private GL-shim pattern landed : `aaalua_array_shim/gol/gol_base.h` shadows engine's real header via `target_include_directories(... BEFORE PRIVATE ...)`. Zero vendor/ touch. New `aaalua_util_partial_stub.cpp` lazy-extracts 4 c_lua_state methods.
- [x] **LuaSocket 3.1.0 vendored** (2026-05-26, continuation 62) — Phase 5 fourth beachhead. `aaaseed_luasocket` static lib (14 .c, plain C) + `cmake/luasocket_mac_force_include.h` shim for Lua 5.1/5.2-API compat ; one-line upstream patch on `luasocket.h` gates extern "C" block under `#ifdef __cplusplus`. `aaaseed_luasocket_smoke_tests` (3 tests) verifies socket.core + mime.core load + socket.gettime() callable.
- [x] **Engine Lua wrapper Stage 5 (TASK #94 LINK GREEN)** (2026-05-27, c105 → c113 ; trio link green c113 ; runtime test c114 ; ostr stub retired c118) — aaalua trio (master + wrap + util, ~2762 LOC combined) **fully compiles AND links on Mac**. 6 sessions of stub-driven cascade collapse cleared 90 unresolved symbols across 10 buckets : **90 → 0 (-100%)**. Doctrine arsenal proven : faked-class stubs (c108, c112), weak-attr stubs (c109, c113), hermetic Mac sub-libs (c104), shim-header shadows (c105/c106), link-cascade audits (c107), asm-rename C↔C++ bridge (c113). Executable `bin/aaaseed_aaalua_trio_link_audit_tests` produced ; runtime gtest with 4 tests (c114).
- [x] **spy.h/cpp Mac-clean + header parse test** (2026-05-26, continuation 55) — one-line upstream cleanup gates `sleep_ex` SleepEx body + `wait_for_single_object` body behind `AAA_OS_WINDOWS()`. `aaaseed_spy_header_tests` (2 tests + 4 static_asserts on cross-platform sleep/yield signatures). Erodes the aaalua_util.cpp blocker list by one.
- [x] **Lua bridge perf test** (2026-05-26, continuation 52) — `aaaseed_lua_bridge_perf_tests` (1 test, perf-labelled). 100k Lua->C calls via vanilla 5.1.5 dispatch ; 40.86 ns/call on M4 debug. Closes perf-coverage gap for c47-51 wrapper port work.
- [x] **Path A shader compile-time perf test** (2026-05-26, continuation 53) — `aaaseed_path_a_compile_perf_tests` (1 test, perf;path_a labels). Iterates 22 catalog shaders ; 6.60 ms wall, avg 0.13 ms, max 0.63 ms = ps_Maa_MixTwo.metal. Scales with catalog growth ; guards against Apple Metal compiler regressions.
- [x] **stb_image decode throughput perf test** (2026-05-26, continuation 54) — `aaaseed_stb_image_perf_tests` (1 test, labels perf;stb_image). 200 decodes of 256x256 RGBA gradient PNG, 52.7 Mpix/sec on M4 debug. Guards asset-pipeline regressions.
- [x] **aaaseed_app per-frame perf test** (2026-05-26, continuation 56) — first .app-level perf test. `aaaseed_app_perf_tests` launches .app via popen with --max-frames 60, parses `[AAASeedApp.Perf]` stdout line emitted by AAASeedMTKView. M4 debug : avg 0.1042 ms/frame across 59 measured frames. Manual frame-pump (NSTimer → `[_mtkView draw]`) added because MTKView display link doesn't fire under popen. Side benefit : aaaseed_app_smoke went 3.27 s → 0.15 s.
- [x] **Replace `aaalua_ostr_stub.cpp` with real `aaa_str.cpp` (RETIRED in audit target)** (2026-05-27, c118) — `aaalua_ostr_stub.cpp` removed from canonical proof-of-port target `aaaseed_aaalua_trio_link_audit_tests` ; real `o_str` from `libaaaseed_code_utils.a`'s `aaa_str.cpp` is the live impl. Phase 5 wrapper migration **proven unblocked**. Stub KEPT in 3 narrow single-aaalua-cpp targets (debug/exp/array) — they lack supporting stubs ; full removal pending c_file_virtual.cpp + c_dir_pool.cpp + err.cpp real ports (lower priority, not v1-blocking).
- [ ] **Engine math_lua.cpp port** — would replace continuation 47's hand-written bridge wrappers with real engine bindings. ~200-400 lines of `AAALUACALL` boilerplate. **Deferred — not v1 ship-blocking** (continuation 47's manual bridge wrappers cover the runtime path).
- [~] **Font rendering : stb_truetype vendored** (2026-05-26, continuation 57) — `third_party/stb/stb_truetype.h v1.26` rolled into existing `aaaseed_stb` lib via STB_TRUETYPE_IMPLEMENTATION. `aaaseed_stb_truetype_tests` (2 tests) loads SourceCodePro-Medium.ttf from runtime tree, rasterizes 'A' at 32px (15x17, 128 non-zero px). Phase 5 third beachhead. Mac-only beachhead ; Windows engine continues to use freetype + ftgl + SdfText (still listed below for the cross-platform UI text layer when needed).
- [~] **Glyph atlas helper** (2026-05-26, continuation 58) — new `src/text/` subsystem + `aaaseed_text` static lib. Pure CPU `aaa::text::build_printable_ascii_atlas()` wraps stb_truetype pack API. `aaaseed_glyph_atlas_tests` (now 6 tests) builds 256x256 atlas from Source Code Pro Medium @ 16 px ; writes debug PNG fixture.
- [~] **Text quad layout helper** (2026-05-26, continuation 59) — `aaa::text::layout_text_quads(atlas, text, cursor_x, cursor_y, out)` returns Metal-ready Vertex2D buffer (x,y,u,v). Handles cursor advancement, space-without-geometry, non-printable byte skipping (\n etc), empty input. 4 layout tests added. Pre-flight verified MetalBackend already supports R8 texture format.
- [x] **.app debug HUD : GPU integration test** (2026-05-26, continuation 60) — `aaaseed_text_render_integration_tests` (1 test) end-to-end : atlas → R8 MTLTexture → text MSL shader → render "AB" to 128x64 offscreen RGBA8 → readback → 18 bright px in 'A' bbox + 23 in 'B' + corner=(13,13,51) clear-color preserved. Last C-side proof.
- [x] **.app debug HUD wired into AAASeedMTKView** (2026-05-26, continuation 61) — `FPS=XX.X frame=N` overlay live. Bundle `SourceCodePro-Medium.ttf` at build, build atlas + R8 MTLTexture at init, compile text MSL (tint uniform), per-frame compose + draw alpha-blended after catalog. 0.0979 ms/frame total (was 0.1042 ms). First user-visible text in the engine on Mac.
- [x] **HUD polish : right-justify + backdrop rect** (2026-05-26, continuation 68) — new `aaa::text::measure_text_width` helper anchors FPS string against top-right corner ; new `kHudSolidMsl` solid-color shader draws a 6-vertex semi-transparent backdrop quad behind the text. Per-frame cost 0.17 ms with both (was 0.10 ms baseline).
- [ ] **HUD multi-line stats** : extend to render multiple lines of per-frame info (frame budget warning, build ID, draw-call counter). Deferred until the engine ships real Lua-callable bindings that produce more interesting per-frame data.
- [~] **FreeType2, ftgl, SdfText** (2026-05-27, audit) — **DECISION: DEFERRED v2 unless a MEU surfaces a need for engine-side SDF text**. Mac .app debug HUD already covered by `stb_truetype` + glyph atlas (c57-c61, c68). Engine SdfText path only needed when MEU requires high-quality scalable UI text rendering ; until then, the Mac substrate is complete.
- [~] **assimp, Bullet** (2026-05-27, audit) — **DECISION: DEFERRED v2 — defer until a MEU surfaces a need**. Both are CMake-buildable from source for Mac when needed ; until a MEU consumer surfaces, they add binary size + compile time without payoff. v1 ships without.
- [x] **Spout → Syphon** (2026-05-27, c122-c126) — `src/syphon/` hermetic Mac sub-lib `aaaseed_syphon`. ServerMac (c122-A IOSurface-backed publish) + advertise() (c123-A CFMessagePortCreateLocal + DN on publish via CFNotificationCenterGetDistributedCenter) + Lua-side `aaa.syphon.send` glue (c124-A) + ClientMac DN observer + IOSurfaceLookupFromXPCObject → MTLTexture (c125-A) + Lua-side `aaa.syphon.receive` (c126-A). Sandbox-safe Apple APIs only. Mac↔Mac reciprocity loop closed c125. **Win-side Spout consumption** : pending — gated on Win machine work (Task #152 territory).
- [~] **FFGL** (2026-05-27, audit) — **DECISION: DROPPED v1 per project doctrine**. FFGL is a GLSL + Win32-plugin format ; the substitution rationale matches `feedback_nvidia_to_metal.md`'s philosophy (Win-only API → Apple equivalent or drop). For v1 Mac, drop entirely. v2 could ship a FFGL-Metal compatibility layer if a user surfaces need.
- [~] **K-Lite codecs → AVFoundation** (2026-05-27, audit) — **DECISION: v1 substitute landed for the READ path** (AVAssetReader for video decode replaces DirectShow read path) ; **WRITE path DEFERRED until MEU needs it**. The read path is what nearly every MEU consumes (load/playback) ; write path is asset-pipeline / recording, rarely needed for runtime playback. AVAssetReader integration as needed when first Video MEU lands.
- [~] **NVIDIA Flex → Metal Compute** (2026-05-27, audit) — **DECISION: v1 DISABLED via `AAA_HAS_FLEX()` macro** per project doctrine ; v2 Metal Compute port deferred. Per `feedback_nvidia_to_metal.md`, Flex's GPU particle compute maps cleanly to Metal compute kernels, but the rewrite is multi-session scope. v1 disable preserves engine green ; no current Mac-compiled subsystem references Flex.
- [~] **Kinect / Azure Kinect, FlyCapture, PGRFlyCapture, DUO, PQMTClient** (2026-05-27, audit) — **DECISION: v1 DISABLED on Mac** (Windows-only camera SDKs ; vendor APIs don't ship Mac binaries). USB cameras route through `AVCaptureSession` (Apple-native substitute). When a MEU surfaces a 3D-depth-camera need on Mac, route through `AVDepthData` + ARKit or the device-specific Mac driver if vendor ships one.
- [~] **OpenVR / SteamVR** (2026-05-27, audit) — **DECISION: v1 NOT IN SCOPE for Mac**. Apple's SteamVR support is limited / deprecated ; VR runtime path on Mac would be visionOS-targeted (separate platform). v2+ consideration only if a Mac VR user surfaces.
- [x] **PureData / PlugData** (2026-05-27, audit) — **DECISION: Works on macOS without changes**. PureData ships native Mac builds ; OSC bridge from AAASeed is platform-agnostic (UDP socket) and unchanged from the Win-side path. No vendoring needed.
- [~] **OpenCL → Metal Compute** (2026-05-27, audit) — **DECISION: DEFERRED per `feedback_nvidia_to_metal.md`**. Apple deprecated OpenCL ; Metal Compute is the canonical substitute. No current Mac-compiled subsystem references OpenCL ; defer port until a specific MEU surfaces compute need, then port the specific kernel directly to MSL compute (`kernel void ...` entrypoint, MTLComputeCommandEncoder dispatch).

## Phase 6 — Cross-Platform Immersive Reciprocity

> Mac binary plays Windows-authored experiences and vice versa. Same Lua runtime.

- [x] **Runtime tree shared between platforms** (2026-05-25) — `vendor/aaaseed-runtime/AAAKernel/` is byte-identical across Mac and Windows. Snapshot at build time into `.app/Contents/Resources/`.
- [x] **`meu_compat.lua` shim** (2026-05-27, continuation 115-B) — `src/lua/meu_compat.lua` ships ; Lua-side platform dispatch ; `aaa.spout.send` (Windows) ↔ `aaa.syphon.send` (Mac) routes to ServerMac via c124-A C-glue. Param_draw + bdd_cell_draw header-parse tests cover the surface. Closed Task #30 partial.
- ~~Verify GLSL shaders ship intact; Mac uses SPIRV-Cross at runtime or pre-compiled `.metallib`.~~ **Why:** Doctrine pivot codified in `feedback_nvidia_to_metal.md` — NO SPIR-V / Vulkan IR anywhere in the pipeline. **New approach:** Path A catalog (141 hand-ported MSL shaders as of c139, 10 revivals proving the pattern). Each `.metal` lands as source ; runtime `MTL::Device::newLibrary` compile-on-demand ; Phase 8 `.metallib` pre-compile gated on Metal Toolchain install (Phase 3 open item).
- [ ] **`.deproj` project file** schema cross-platform (enums serialize by string label per existing Windows convention; safe across platforms already). **Cross-platform safe today — no engine code path differs ; verify when first .deproj loads on Mac.**

## Phase 7 — Test Harnesses

- [x] Unit-test framework: GoogleTest via FetchContent. `aaaseed_*_tests` executables. **99 tests passing** as of continuation 25.
- [x] **Integration-test framework (Metal-backend slice)** (continuations 18-25) — `tests/unit/gol_metal_render_test.cpp` + `shader_msl_port_test.cpp` + `spirv_to_metal_program_test.cpp` (deleted in c24) exercise full GPU draw paths against offscreen render targets and pixel-readback. Headless Lua-driven `AAASeed --script ... --output frame.png --exit-after 1` still pending (waits on the engine main loop being Mac-runnable).
- [~] **Regression corpus** — render tests today pin specific pixel outputs (red clear, green triangle, depth occlusion, alpha blend, textured-quad center pixel). Continuation 26 added the first golden-image-comparison test (`tests/regression/`). **As of c139 : 46 Path A shaders in Phase 3 regression baseline + 10 revival proof tests (FXAA, IFS, lights_deferred, aaa_cam, PBR, noise, GoL, curl, bloom, motion blur).** Cross-platform Win-side capture gated on Task #152 WindowsBackend (Win machine).
- [x] **Performance profiling doctrine** (2026-05-27, codified continuation 26 ; 11 perf-labelled tests as of c139) — every Metal backend feature lands with (a) an `os_signpost` interval around the CPU-side encoder build, (b) a `pushDebugGroup` label for the GPU side, and (c) a CTest under label `perf` that asserts the frame-render path stays within budget. See `philosophy.md` Part 2.95 and `agent_profile.md`. CTest label-first-only quirk codified in `feedback_ctest_label_first_only.md`.
- [~] **CI matrix: GitHub Actions windows-2022 + macos-14** (2026-05-27, c115-A Mac side landed) — `.github/workflows/ci.yml` (111 LOC, Mac-only PR gate) ships : macos-14 runner, ctest -L unit/perf blocking, integration/regression non-blocking due to headless-Metal. **Windows side** (windows-2022 runner) gated on Task #152 WindowsBackend extraction (Win machine work, out of scope v1 Mac ship).

## Phase 8 — Distribution and DMG compression

- [x] **`.app` bundle layout** (2026-05-27, c136-B + c139-B) — `bundle/macos/Info.plist.in` canonical (80 LOC, configure_file with `${PROJECT_VERSION}`) + `bundle/macos/entitlements.plist` (sandbox-off, network, audio, camera, JIT) + `src/macos/aaa_bundle_meta.{h,mm}` (`aaa::macos::bundle::*` 8 NSBundle wrappers). Old `src/ui/macos/Info.plist.in` deleted ; singularity guard test c139-B.
- [x] **CMake `install` step copies runtime tree** (2026-05-27, c136-B + Phase 8 wiring) — runtime tree integration in `bundle/macos/` ; `Contents/Resources/AAAKernel/` snapshot at build time (env-var override + cwd fallback documented in `porting.md` §3).
- [ ] Binary size discipline: `-flto=thin`, linker `-dead_strip` and `-Wl,-x`, post-link `strip -x`. Target: ≤ 30 MB binary. **Deferred — verify after c141-A DMG production check**.
- [ ] Shader bundling: 373 `.metal` files pre-compiled into one `default.metallib`. Runtime compile reserved for hot-reload. **Deferred — gated on Metal Toolchain install (Phase 3 open item)**.
- [ ] Texture compression (optional, gated on Phase 7 profiling): PNG → ASTC LDR. Apple Silicon native ASTC sampling, 4-8x smaller. **DEFERRED v2 — no current consumer**.
- [x] **DMG creation** (2026-05-27, c137-B) — `cmake/dmg.cmake` (112 LOC) defines `aaaseed_dmg` custom_target ; `hdiutil create -format ULMO ... || hdiutil create -format ULFO` shell-level fallback ; `cmake -P cmake/dmg.cmake` graceful exit + `/usr/bin/hdiutil` confirmed on host. Production check in flight (c141-A).
- [x] **Drag-to-Applications affordance** (2026-05-27, c137-B) — `create_symlink /Applications` step in `cmake/dmg.cmake` DMG staging flow.
- [x] **Code signing CMake template** (2026-05-27, c138-B) — `cmake/codesign.cmake` (160 LOC) `aaaseed_codesign_target(TARGET)` POST_BUILD hook on .app, NO-OP when `CODESIGN_IDENTITY` unset ; command wired : `codesign --deep --options runtime --timestamp --entitlements ${entitlements.plist} --sign "$ENV{CODESIGN_IDENTITY}" $<TARGET_BUNDLE_DIR:aaaseed_app>`. **Awaits Developer ID for executor session to actually sign.**
- [x] **Hardened runtime entitlements** (2026-05-27, c136-B) — `bundle/macos/entitlements.plist` (43 lines, 7 keys, 4 categories) : sandbox-off + network client+server + audio-input + camera + user-selected.read-write + cs.allow-jit. `plutil -lint` OK.
- [x] **Notarization CMake template** (2026-05-27, c138-B) — `aaaseed_notarize_dmg()` defines `aaaseed_dmg_notarize` depending on c137-B `aaaseed_dmg`, NO-OP when notary env vars unset ; command wired : `xcrun notarytool submit <dmg> --key $ENV{NOTARY_API_KEY_PATH} --wait` + `xcrun stapler staple <dmg>`. **Awaits NOTARY_API_KEY_PATH + NOTARY_API_KEY_ID + NOTARY_API_KEY_ISSUER_ID for executor session.**
- [x] **First-launch quarantine handling** (2026-05-27, c136-B README.md runbook) — `bundle/macos/README.md` documents `xattr -d com.apple.quarantine AAASeed.app` workaround for non-notarized first-launch.
- [ ] **Universal binary (arm64 + x86_64)** — deferred. v1 ships arm64 only. Add to v2 backlog if Intel-Mac users emerge.
- [ ] Auto-update via Sparkle framework — deferred to v1.1.
- [ ] **Release DMG target size**: ≤ 80 MB after all compression layers. **Production check in flight (c141-A) — measure once DMG produces.**

---

## Upstream patches applied (all behavior-preserving on Windows by inspection)

19 patches accumulated as of 2026-05-26:

1. `Src/code_utils/aaa_type.h:199` — space in `"v"AAA_STRING(...)` (C++11 UDL compliance)
2. `Src/code_utils/aaa_type.h:203` — `__aarch64__`/`__arm64__` added to little-endian whitelist
3. `Src/code_utils/aaa_type.h:247` — `#ifndef` guard on `__STDC_LIMIT_MACROS` redef
4. `Src/code_utils/aaa_type.h:195-208` — `AAA_VSTOOL() 145` + `AAA_COMPILER_STR()` on non-MSVC
5. `Src/code_utils/aaa_rand.cpp` rnd_gauss — cast each rand() to REAL before summing (RAND_MAX overflow on macOS)
6. `Src/code_utils/aaa_rand.cpp` rnd_maa — `(REAL)rand()/(REAL)RAND_MAX` (RAND_MAX ULP rounding on float)
7. `Src/code_utils/md5.h` + `md5c.cpp` — `unsigned long` → `uint32_t` everywhere (**REAL UPSTREAM BUG**: LP64 breaks MD5 algorithm)
8. `Src/code_utils/id_unique.cpp:19,34` — `template<>` prefix on member specializations
9. `Src/code_utils/buffer/buffer_blk.cpp` — **NOT applied; surfaced**: `c_buffer_stream::put()` doesn't advance `src` between iterations
10. `Include/HalfFloat/half.hpp:1042` — `operator "" _h` → `operator""_h` (C++20 deprecation)
11. `Src/math/v.h:18-22` — `#ifndef AAA_USE_XNA` guard + unconditional `v_xna.h` include
12. `Src/math/v_xna.h` — `#else` branch with inline forwarders to `_v3r` variants
13. `Src/math/TMatrixAlgo.h:21,31` — `!defined(__aarch64__)` excludes arm64 from Darwin SSE branch
14. `Src/aaa/aaa_mutex.h` — `c_mutex_ct`/`c_benaphore`/`c_benaphore_recursive` gated with `#if AAA_OS_WINDOWS()`; `#include <thread>` added
15. `Src/aaa/aaa_mutex.cpp` — entire body wrapped in `#if AAA_OS_WINDOWS()`
16. `Src/draw/NSightEvents.h` — `#ifndef AAA_NSIGHT_USE` guard + `#else` branch with `os_signpost` real implementation
17. `Src/code_utils/spy.h:48` — `wait_for_single_object` gated with `#if AAA_OS_WINDOWS()`
18. `Src/infrastructure/factory/registry.cpp` — three C++ compliance fixes (typename, two template<>)
19. `Src/infrastructure/param/param.cpp:592,602` — `_isnan` → `std::isnan`, `#include <cmath>`
20. `Src/infrastructure/param/param.cpp:1412,1434` — `REAL tmp = 0.` / `DOUBLE tmp = 0.` (UB fix on switch default)

## Upstream-bug candidates worth surfacing to Mâa

1. **MD5 LP64 bug** — `unsigned long` is 4 bytes on Windows LLP64 but 8 bytes on Linux/macOS LP64. The algorithm requires 32-bit state words. All MD5 outputs were wrong on macOS before the fix. Any Linux/Mac build of AAASeed would have hit this.
2. **`rnd_maa` + `rnd_gauss` RAND_MAX bugs** — same LP64 class. MSVC's small RAND_MAX (2^15-1) accidentally avoided the overflow.
3. **`c_buffer_stream::put` src not advanced** — calling `put(data, N*blk_size)` writes only the first block's worth of data into every allocated block. Likely never triggered in production (callers feed one audio frame at a time) but is a real logic bug.
4. **`param.cpp:1412/1434` uninitialised `tmp`** — was UB on switch's default branch. Clang `-Wsometimes-uninitialized` flagged it.

---

## Cancelled / Superseded items

`~~Use MoltenVK as the graphics backend~~` — **Why removed:** MoltenVK is Vulkan-over-Metal; AAASeed has no Vulkan code. Adding Vulkan then translating to Metal is two abstractions where one (direct Metal) suffices. Replaced by Phase 3 native Metal backend via metal-cpp.

`~~Use ANGLE to emulate OpenGL on top of Metal~~` — **Why removed:** ANGLE works but adds a translation layer in the critical path of every draw call. For a 30-year-old engine that already wraps GL in GOL, swapping the GOL backend to native Metal is cleaner and faster. Re-evaluate only if shader-translation cost via SPIRV-Cross becomes the dominant porting bottleneck.

`~~Stub NSight macros as no-ops on Mac~~` — **Why removed:** Per Alex's NVIDIA→Metal substitution doctrine (continuation 12, then continuation 13 formalised), the right move is a real native port to `os_signpost`. NXPROFILEFUNC now produces real Instruments-visible CPU signposts on Mac. See `feedback_nvidia_to_metal.md` and `philosophy.md` Part 2.85.

---

## How to update this file

When you finish an item, change `[ ]` to `[x]` and add a one-line note of what file/PR/handover continuation delivered it. When you cancel an item, change `[ ]` to `~~strike~~` AND add a new replacement bullet directly under it with `**Why:** <reason>`. Never silently delete a cancelled item; the cancellation record is part of the project's design history.

---

## Strategy for the remaining open items (continuation 26)

Open items grouped by where they sit in the critical path to a shippable DMG. Each group has a concrete acceptance criterion ; finishing the last item in the group unlocks the next.

### Group A — Make `aaaseed_code_utils` complete (small, tractable)

**Goal :** every `code_utils/*.cpp` either compiles into `aaaseed_code_utils` or is explicitly documented as deferred with a reason. Removes the last stub-shaped holes in test linkage.

Items :
- Wire `aaa_mem.cpp` (partial port preserved). Risk : `mem::*` consumer-surface validation against the engine's `MALLOC` / `REALLOC` / `SAFE_DELETE` macros, especially `REALLOC`'s non-stdlib free-on-failure semantic (see `code_utils/CLAUDE.md`).
- Wire `err.cpp` after either (1) porting `infrastructure/layer/`, `ui/aaa_menu.h`, `ui/dialog_dev.h`, OR (2) adding minimal Mac-side stubs for those headers so err.cpp compiles standalone.

Acceptance : `aaaseed_code_utils_tests` no longer links `test_engine_stubs.cpp`. The stubs file deletes.

### Group B — Path A shader hand-port at scale

**Goal :** all 250 macro-free engine shaders ported to MSL.

Strategy :
1. **Mechanical-substitution tool** at `tools/glsl_to_msl/` (pure C++ or Python) doing the same GLSL → MSL transformations done by hand on `ps_edgeoverlay.frag` (`texture2D` → `tex.sample()`, `varying` → `[[stage_in]]` struct, `gl_FragCoord` → `[[position]]`, `gl_Position` → struct member with `[[position]]`, `gl_FragColor` → return value, `dFdx/dFdy` → `dfdx/dfdy`, etc.). Tool reports unhandled constructs as `// TODO_PORT:` markers ; never silently drops them.
2. Run tool over the 249 remaining macro-free shaders. Manual review per file.
3. Each `.metal` lands with an integration test (extends `shader_msl_port_test.cpp` pattern) that runs `newLibrary` + entry-point lookup. Batch tests run in <1 s total ; do not spin a new executable per shader.
4. Output : `src/shaders/msl/<basename>.metal` ; one .metallib later in Phase 8.

NOT in scope here : the 71 macro-using shaders (Group C).

### Group C — Path B for macro-using shaders (~71 files)

**Goal :** translate the shaders that GLSL_shader.cpp injects platform-specific `#define` blocks into.

Strategy :
1. Reproduce the injection in a small C++ utility (offline asset bake) that mirrors `GLSL_shader.cpp:79-114`. Reads the include file list, concatenates includes + source, runs textual `#define` expansion (NOT GLSL preprocessor — flat substitution).
2. The output is a self-contained GLSL with macros expanded inline ; THAT goes through the Group B tool.
3. Two-stage : platform-macro expand, then mechanical GLSL → MSL.

This stays pure-Metal — no SPIR-V tooling anywhere in the pipeline.

### ~~Group D — Layer subsystem (CORRECTED 2026-05-26 continuation 28 : ~14 .cpp files, ~6.7 K LOC core, drags whole engine)~~ (Task #31 SUPERSEDED 2026-05-27 c145)

**SUPERSEDED 2026-05-27 (continuation 145).** See `memory/project_layer_supersession.md` for the formal doctrine memo. The Mac-native equivalent is the c142-B MEU runner (`src/meu/aaa_meu_runner_mac.{h,mm}`) + the Path A 141-shader catalog (`src/shaders/msl/`) + MetalBackend, NOT a literal port. Stage 1 header-parse tests for `layer.h` + `app.h` (c111 / c112) are KEPT as upstream-patch regression guards but no further layer-subsystem porting is planned for v1. Reopens ONLY with (1) vendor authorization to edit `vendor/aaaseed-engine/Src/infrastructure/layer/`, (2) a user-surfaced asset-parity need, (3) Win-side Task #152 WindowsBackend landed first.

~~**Real scope (measured 2026-05-26 continuation 28) :**~~
~~| File | Lines |~~
~~|---|---|~~
~~| layer.cpp | 2486 |~~
~~| layers.cpp | 1532 |~~
~~| module.cpp | 1289 |~~
~~| modules.cpp | 974 |~~
~~| app.cpp | 447 |~~
~~| layer_att.cpp / layers_att.cpp | tens-of-hundreds each |~~
~~| layer_lua.cpp / layers_lua.cpp / module_lua.cpp / modules_lua.cpp | Lua bindings |~~
~~| **Total core .cpp** | **~6728 lines** |~~

~~The CLAUDE.md confirms layer/ pulls in the entire `obj_ui/bdd/` (the drawers), `draw/` (cameras, lights, materials, render state, fog, stencil, texturing, model, ship, multiple), `c_traxs` animation system, `c_lua_wrap`. **A full layer port is multi-session work (estimated 4-6 sessions)**, not a single-shot.~~

~~**Revised sequencing :**~~
~~1. **Stage 1 (one session, future)** : header-only parse smoke test mirroring the obj_ui Stage 1 pattern. Just verify the header chain compiles. Output : `layer_header_test.cpp` that #includes the five main headers and asserts nothing.~~
~~2. **Stage 2 (next session)** : compile `app.cpp` (447 lines, simplest leaf). Stub whatever it pulls in.~~
~~3. **Stage 3 (later)** : `modules.cpp` → `module.cpp` → `layers.cpp` → `layer.cpp` in dependency order.~~
~~4. **Stage 4 (last)** : `*_att.cpp` + `*_lua.cpp`.~~

~~**NOT in scope until Stages 1-2 land** : the runtime `module::update()` / `module::draw()` integration test idea above.~~

### Group D-prime — Spaced-out approach to draw/ subsystem (NEW, continuation 28)

Layer subsystem can't land without large parts of `draw/` (cameras, lights, render state, etc.) Mac-clean. Two paths :
- (a) Port `draw/` subsystems on-demand as layer.cpp's blockers surface. Reactive ; risks long ping-pong.
- (b) Pre-scan `draw/` for Win32-only headers (HWND, HDC, GDI ops) and gate them in advance. Proactive ; spreads the work across sessions.

Recommend (b) : one session dedicated to a `draw/` Win32 audit + gating, then layer Stage 2 unblocks.

**Initial audit (continuation 28)** :
- `draw/` total : ~20.5 K LOC across 97 files (excluding `_lua` bindings).
- Direct `windows.h` / HWND / HDC contamination : **concentrated in 1 file** (`seeddraw.cpp`). Most of `draw/` is platform-clean by virtue of GOL abstraction.
- Direct legacy GL contamination (`glBegin` / `glEnd` / `glVertex`) : also concentrated to a small set.
- Implication : the `draw/` port is LESS painful than feared. Stage 2 of Group D can proceed once `seeddraw.cpp` is gated (or built into a `gol::Windows` backend).

### Group E — Param subsystem completion (Stage 3b) — Task #30 CLOSED 2026-05-27 c145

- [x] **Stage 3b honest closure (2026-05-27, c145)** — vendor `bdd_cell_draw.cpp` (527 LOC, 80+ file BDD subsystem, ~15K LOC total) is NOT fully ported -- that's a v2 Metal renderer (3-4 weeks). The pure-C++ math (~120 LOC : branch_make_pos, alpha lerp, radius / grid / size-per-child) IS ported to `src/bdd/aaa_bdd_cell_draw_mac.{h,cpp}` in the `aaa::bdd::*` namespace. Abstract renderer interface at `src/bdd/aaa_bdd_renderer.h` declares the contract a v2 Metal BDD renderer must satisfy (draw_mesh / draw_curve3d / draw_cell + resource lifecycle handles). New static lib `aaaseed_bdd`. 7 tests at `tests/unit/aaa_bdd_cell_draw_mac_test.cpp` (`phase3;unit;bdd`). The GL-bound vendor portion (`bdd_gl_part.cpp` + draw / draw_text / draw_branches / draw_obj_internal in `bdd_cell_draw.cpp`) stays v2-deferred ; param_draw.cpp similarly stays v2 (requires full `c_obj_ui` / `c_param` Mac link, which is the c119-A doctrine wall — see also Group D supersession).

~~**Goal :** `param_draw.cpp` + `bdd_cell_draw.cpp` compile. Was blocked on GOL ; now unblockable.~~

~~Items : straight integration ; expect 5-20 small breakage points (Win32 GDI calls, fixed-function GL calls) per file, all fixable per the same Win-gate pattern.~~

### Group F — Performance + regression test infrastructure

**Goal :** every shipped feature has measurable proof of correctness AND performance, comparable to the Windows baseline.

Items :
- `tests/regression/` dir + golden image comparison harness (PNG diff with per-pixel tolerance). Uses the existing `read_pixel_rgba8` machinery from `gol_metal_render_test.cpp` but writes/reads full frames.
- CTest label `perf` reserved for tests that assert wall-clock time within budget. Sample : "render 1000 triangles under 16 ms on M4 debug build".
- `os_signpost` already in place (continuation 13 NSight port) ; doctrine says wrap new encoder blocks with both signpost AND `pushDebugGroup`.
- Capture 5-10 golden frames from existing render tests as the first regression corpus.

### Group G — Window + events for real input (Phase 4)

**Goal :** AAASeed responds to keyboard / mouse / trackpad on Mac.

Items already in todo Phase 4. Order of attack :
1. `NSEvent` → AAASeed `c_event_*` translator (smallest surface, biggest unblock).
2. File dialogs (NSOpenPanel/NSSavePanel).
3. DPI handling via `backingScaleFactor`.
4. Trackpad gestures last (least critical).

### Group H — Distribution (Phase 8)

**Goal :** signed + notarized DMG ≤ 80 MB. Last-mile work.

Strategy : do nothing here until A-D land. Phase 8 is mechanical once the engine actually runs ; getting to "runs" is the hard part.

### Sequencing the next 3-5 sessions

| Session | Focus | Acceptance |
|---|---|---|
| 26 (this) | Doctrine + aaa_mem wire + first regression test + perf test scaffold | Tests >= 102 ; doctrine block at top of todo + philosophy ; aaa_mem in lib |
| 27 | Group F (regression harness + perf budgets) + Group D (layer subsystem) | First golden image diff ; layer/* compiling |
| 28 | Group B (Path A scaling : tool + 10-20 shaders) | tools/glsl_to_msl/ exists ; 20 shaders ported with tests |
| 29 | Group A (err.cpp full wire) + Group E (Stage 3b) | test_engine_stubs.cpp deletes |
| 30 | Group G (input plumbing) + Phase 8 prep | First real keyboard input ; .app bundle layout final |
