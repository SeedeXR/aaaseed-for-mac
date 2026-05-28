# Studio UI — Todo / Milestone Tracker

## v1 — Authoring Surface (current)

### Completed
- [x] `cmake/aaa_imgui.cmake` — FetchContent Dear ImGui v1.91.6; two static libs
      (`aaaseed_imgui` + `aaaseed_imgui_metal_backend`)
- [x] `src/ui/studio/aaa_studio.h` — full public pure-C++ API
- [x] `src/ui/studio/aaa_studio_metal.mm` — ObjC++ implementation; all 10 panels
- [x] `src/ui/studio/CMakeLists.txt` — `aaaseed_ui_studio_mac` static lib
- [x] Root `CMakeLists.txt` — `include(aaa_imgui)` + `add_subdirectory(src/ui/studio)`
- [x] `AAASeedMTKView` wiring — lazy init, `new_frame`, `render`, `studio` accessor
- [x] `GOL::MetalBackend::get_active_encoder()` — c134-A `void*` bridge accessor
- [x] Unit tests — `tests/unit/aaa_studio_test.cpp` (30+ tests, no GPU)
- [x] Integration tests — `tests/integration/studio_in_app_test.mm` (4 in-app tests)
- [x] Regression tests — `tests/regression/studio_data_model_regression_test.cpp`
      (8 invariant guards)

### c151-A fixes (this session)
- [x] `new_frame()` now wires `io.DisplaySize` + `io.DeltaTime` (no longer
      discards the caller's args ; falls back to `steady_clock` if delta=0)
- [x] `new_frame()` passes the real `NSView*` to `ImGui_ImplOSX_NewFrame`
      (was `nullptr`) and a fresh `MTLRenderPassDescriptor` to
      `ImGui_ImplMetal_NewFrame` (was `nil`)
- [x] `init()` cleans up the ImGui context on backend-init failure
      (no more leaked context on retry)
- [x] `init()` returns false gracefully on null args (no abort assert)
- [x] `render(encoder, cmd_buffer)` now takes both ; passing nil for
      either falls back to `ImGui::EndFrame()` (test-scaffolding path)
- [x] `set_font_scale()` / `apply_gabuzoumeu_theme()` defensive against
      no-context (live-context guard)
- [x] NSEvent local monitor installed in `AAASeedMTKView` so ImGui
      windows are actually interactive (the previous build drew them
      but consumed zero events)
- [x] Cmd+R in Code Editor now writes the buffer to a temp .lua and
      reloads via `_meuRunner->load_script(temp_path)` ; status logged
      to the Studio Console
- [x] Shader Catalog now enumerates `src/shaders/msl/*.metal` at
      runtime via the `AAA_SHADERS_MSL_DIR` compile define (169 shaders
      visible ; was a hardcoded 14-entry list)
- [x] Sound panel now enumerates real Core Audio devices via
      `AudioObjectGetPropertyData` (was hardcoded fake entries)
- [x] `Studio::enumerate_sound_devices()` public API refreshes from
      Core Audio on each call
- [x] imgui backend `.mm` files now compile with `-fobjc-arc` (upstream
      assumption ; `-fno-objc-arc` broke `MetalContext.device` retain
      semantics and caused `newLibraryWithSource` → nil)
- [x] `GameController.framework` linked into `aaaseed_imgui_metal_backend`
      (imgui_impl_osx needs `GCController` for gamepad input)
- [x] `GOL::MetalBackend::get_active_command_buffer()` bridge accessor
      added ; the MTKView caller now passes both encoder + cb to
      `Studio::render`
- [x] 5 in-app integration tests relinked against `aaaseed_ui_studio_mac`
      (they compile `AAASeedMTKView.mm` directly and now need the symbols)
- [x] V3Samples ship-gate test : skip-if-missing pattern matching V4
- [x] Metal feature checks gated on real Apple Silicon ; skip on
      virtualized / non-Apple-family Metal devices
- [x] Syphon `*_test.mm` files : skip when `MTLCreateSystemDefaultDevice`
      returns nil

### c151-B closures (this session, second pass)
- [x] App icon placeholder — `bundle/macos/AAASeed.icns` generated
      via Swift/NSImage (dark-teal + AAA wordmark), 10 sizes in
      `bundle/macos/AAASeed.iconset/`, wired through `CFBundleIconFile`
      in `Info.plist.in` and a POST_BUILD copy in `src/ui/macos/CMakeLists.txt`
- [x] Lua `aaa.studio.*` bindings (live, registered automatically when
      a Runner is wired) :
        - `aaa.studio.log(level, text)` — `level` may be int or
          `"info"`/`"warn"`/`"err"`/`"lua"`
        - `aaa.studio.add_node(label[, x, y])` → returns node id
        - `aaa.studio.set_camera(px, py, pz, lx, ly, lz[, ux, uy, uz, fov])`
        - `aaa.studio.set_font_scale(s)`
      Hook surface : `Runner::set_lua_extension_hook(fn(void*))` ;
      fires inside `load_script` after the runner's own `aaa.*`
      bindings. 8 unit tests cover the bindings without Metal or
      NSApplication
- [x] First-frame drawable race — fixed. `init()` now takes the
      MTKView's static `colorPixelFormat` ; `new_frame()` synthesises
      a 1x1 probe texture stamped with that format whenever
      `currentDrawable` is nil. No more "failed to create Metal
      library: (null)" startup log
- [x] NSTask binary manager — real spawn / SIGTERM / stdout+stderr
      pipe drain into Studio Console. Attach form (label + path +
      args), per-task Start / Stop / Remove buttons, exit-code
      display
- [x] AVFoundation camera capture — `AVCaptureDeviceDiscoverySession`
      enumeration, `AVCaptureSession` with `AVCaptureVideoDataOutput`,
      CVPixelBuffer → MTLTexture via `CVMetalTextureCache` (UMA zero-
      copy on Apple Silicon), preview rendered with `ImGui::Image`.
      Lifetime tied to the Studio ; `stopCapture` on dtor
- [x] Cross-platform data-model split — `src/ui/studio/aaa_studio.cpp`
      now hosts the pure-C++ data methods (ctor, node graph mutations,
      editor, console, camera, perf, set_font_scale) ; `aaa_studio_metal.mm`
      keeps only Mac-specific code (ImGui Metal/OSX init, NSEvent
      monitor, Core Audio, AVFoundation, NSTask, Lua bindings install,
      Cocoa menu). Shared private header `aaa_studio_impl.h` holds
      `StudioImpl` + `PerfRing` + node-layout constants. Both .cpp
      and .mm compile into the same `aaaseed_ui_studio_mac` static lib

### Truly still open (no longer in scope this session)
- [ ] File-dialog `...` button in the Code Editor panel is a no-op
      (Studio panel has the button but no NSOpenPanel wiring)
- [ ] Real Windows / Linux platform siblings — `aaa_studio.cpp` is
      now ready ; need `aaa_studio_dx12.cpp` + `aaa_studio_vulkan.cpp`
      with the matching ImGui platform backends
- [ ] Bezier wires + imnodes integration for richer routing
- [ ] Double-click node → open code editor pre-loaded with bound script
