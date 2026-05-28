# Studio UI — Architecture

## Layer Diagram

```
┌─────────────────────────────────────────────────────────────┐
│  NSApplication / MTKView (src/ui/macos/)                     │
│  AAASeedMTKViewDelegate — owns Studio via unique_ptr         │
│  drawInMTKView: drives  new_frame() → render()               │
└────────────────────────┬────────────────────────────────────┘
                         │ c148 lazy init on first draw tick
                         ▼
┌─────────────────────────────────────────────────────────────┐
│  aaa::ui::studio::Studio  (src/ui/studio/)                   │
│  Pure-C++ public API (aaa_studio.h)                          │
│  ObjC++ impl in aaa_studio_metal.mm                          │
│                                                              │
│  Panels:                                                     │
│    Node Graph  │ Code Editor  │ MEU Inspector                │
│    Shader Catalog │ Camera    │ Sound (placeholder)          │
│    Binary Mgr  │ Console      │ Perf  │ Preferences           │
└────┬───────────────────────┬──────────────────────────────┘
     │                       │
     ▼                       ▼
┌──────────────┐    ┌────────────────────────────────────────┐
│ Dear ImGui   │    │  GOL::MetalBackend                     │
│ v1.91.6      │    │  get_active_encoder() → void*          │
│ (vendored)   │    │  get_device() → MTL::Device*           │
│              │    │  begin_window_render_pass()             │
│  imgui_impl  │    └────────────────┬───────────────────────┘
│  _metal.mm   │                     │
│  _osx.mm     │                     ▼
└──────────────┘    ┌────────────────────────────────────────┐
                    │  Metal GPU (Apple Silicon)              │
                    │  MTLRenderCommandEncoder                │
                    └────────────────────────────────────────┘
```

## Hermetic Sub-lib Boundaries

`aaaseed_ui_studio_mac` follows the hermetic Mac sub-lib doctrine:

| Rule | Detail |
|------|--------|
| Public header is pure C++ | `aaa_studio.h` — no ObjC, no metal-cpp types |
| ObjC++ isolated | Only `aaa_studio_metal.mm` is `.mm` |
| Mixed ARC | `aaa_studio_metal.mm` : MRC (`-fno-objc-arc`, matches metal-cpp). Upstream imgui_impl_metal.mm + imgui_impl_osx.mm : ARC (`-fobjc-arc`, required by their internal retain/release). The boundary is the `__bridge` cast at `ImGui_ImplMetal_Init(device)` |
| No shared-state includes | No `o_str`, `aaa_mem`, `aaa_str`, `aaaseed_code_utils` |
| STL only | `std::string`, `std::vector`, `std::unordered_map`, `<cstdint>`, `<filesystem>` |
| c134-A bridge API | `void*` for all ObjC/Metal types in public headers |

## Static Library Dependency Graph

```
aaaseed_app
  └── aaaseed_ui_studio_mac       ← c148 / c151-A / c151-B
        # Sources :
        #   aaa_studio.cpp        pure-C++ data model (cross-platform)
        #   aaa_studio_metal.mm   Mac glue (ImGui Metal, NSEvent, Cocoa,
        #                         CoreAudio, AVFoundation, NSTask, Lua)
        # Both compile into the same static lib ; share aaa_studio_impl.h
        ├── aaaseed_imgui_metal_backend
        │     ├── aaaseed_imgui (core)
        │     ├── Metal.framework
        │     ├── MetalKit.framework
        │     ├── QuartzCore.framework
        │     ├── AppKit.framework
        │     └── GameController.framework    ← c151-A : imgui_impl_osx
        │                                       references GCController
        ├── aaaseed_gol_metal
        ├── aaaseed_meu_runner
        ├── Cocoa.framework
        ├── Metal.framework
        ├── MetalKit.framework
        ├── CoreAudio.framework               ← c151-A : Core Audio
        │                                       device enumeration
        ├── AVFoundation.framework            ← c151-B : camera capture
        ├── CoreVideo.framework               ← c151-B : CVPixelBuffer
        ├── CoreMedia.framework               ← c151-B : CMSampleBuffer
        └── IOSurface.framework               ← c151-B : zero-copy
                                                CVPixelBuffer → MTLTexture
```

## Test Coverage

| Test suite | File | Labels | GPU needed |
|------------|------|--------|-----------|
| Unit | `tests/unit/aaa_studio_test.cpp` | `unit;studio` | No |
| Integration | `tests/integration/studio_in_app_test.mm` | `integration;studio` | Yes (real Metal) |
| Regression | `tests/regression/studio_data_model_regression_test.cpp` | `regression;studio` | No |

Per the project's CTest doctrine ([feedback_ctest_label_first_only]),
`gtest_discover_tests PROPERTIES LABELS` honours only the FIRST label on
this toolchain. The PRIMARY filter key (`unit` / `integration` /
`regression`) must come first ; the secondary `studio` tag is
informational, not selectable via `ctest -L studio` on the current
runner. To run only the studio tests, use `--gtest_filter='Studio*'` on
the binary directly.
