# GOL::WindowsBackend — Win-side plug-in runbook

Status : Mac-side prerequisite COMPLETE (c126-c130 / Task #152, 2026-05-27).
Win-side execution PENDING (separate Win-machine session ; see Section 6).

This document is the actionable handover from the Mac port back to a
Win-machine session that will refactor the existing Win32 + OpenGL
fixed-function code into a `GOL::WindowsBackend` concrete class. The
abstract `GOL::Backend` interface (Mac-side, c20-c22) is verified
cross-platform : every method has a sensible Win-side mapping (Section 2)
and the header carries zero Metal-cpp / Foundation / AppKit leak (proven
by `tests/unit/gol_backend_cross_platform_test.cpp`).

Phase 3 EXIT criterion (Section 4) closes once a single Path A golden
shader (e.g. `aaa_bloom_real`) renders pixel-comparable output on both
backends + frame time within 1.2x of the Windows reference (Section 5).

---

## Section 1 — Prerequisites

Win machine baseline :

- Windows 10 1909+ or Windows 11 22H2+.
- Visual Studio 2022 17.10+ (MSVC v143 toolset) OR Build Tools 2022.
- CMake 3.27+ and Ninja 1.11+ (the existing project uses Ninja-MSVC).
- Git 2.40+ with `core.autocrlf=input` (matches the Mac side).
- Existing `aaaseed-windows` clone with the full vendor tree at
  `vendor/aaaseed-engine/Src/`.
- The Mac repo's `src/gol/gol_backend.h` available at the same logical
  path. Recommended : clone `aaaseed-for-mac` next to the windows
  clone, then symlink or include-path `${MAC_REPO}/src/gol/` from the
  Win build. The header is pure C++20, no platform dependencies.

What you do NOT need :

- metal-cpp (Metal-only, not consumed by the Win build).
- Apple frameworks (Foundation, AppKit, Metal, MetalKit).
- The Mac CMake presets (the Win build has its own).

---

## Section 2 — Discover GL call sites

Enumerate the existing Win-side fixed-function OpenGL surface. From the
Win repo root :

```
grep -rn "glBegin\|glEnd\|glVertex\|glColor3\|glColor4" vendor/aaaseed-engine/Src/gol/
grep -rn "glEnable\|glDisable\|glBindTexture\|glTexImage2D" vendor/aaaseed-engine/Src/gol/
grep -rn "glLight\|glMaterial\|GL_LIGHT0\|GL_LIGHT1" vendor/aaaseed-engine/Src/gol/
grep -rn "glMatrixMode\|glLoadIdentity\|glPushMatrix\|glPopMatrix" vendor/aaaseed-engine/Src/gol/
grep -rn "glPushAttrib\|glPopAttrib" vendor/aaaseed-engine/Src/gol/
grep -rn "glGenBuffers\|glBindBuffer\|glBufferData" vendor/aaaseed-engine/Src/gol/
grep -rn "glCreateShader\|glCompileShader\|glLinkProgram" vendor/aaaseed-engine/Src/gol/
grep -rn "glDrawArrays\|glDrawElements\|glDrawElementsBaseVertex" vendor/aaaseed-engine/Src/gol/
grep -rn "glReadPixels\|glReadBuffer" vendor/aaaseed-engine/Src/gol/
```

Per the c142-C inventory, the surface lives across these vendor files :

| Vendor file        | What it owns                                            | Backend method that absorbs it                      |
| ------------------ | ------------------------------------------------------- | --------------------------------------------------- |
| `gol_base.cpp`     | wgl context creation, swap buffers, enable/disable      | `init()` + `deinit()` + `present()`                 |
| `gol_attrib.cpp`   | `glPushAttrib` / `glPopAttrib` (state stack)            | mapped into `bind_program` / `set_viewport` state   |
| `gol_light.cpp`    | `glLight*` + `GL_LIGHT0..7` (fixed-function lights)     | uniform-buffer struct, bound via `bind_fragment_buffer` |
| `gol_matrix.cpp`   | `glMatrixMode` / `glLoadIdentity` / push/pop            | explicit `float4x4` in vertex constants buffer      |
| `gol_pbo.cpp`      | pixel-buffer-object uploads                             | `buffer_data()` with `BufferUsage::Stream`          |
| `gol_buffer.cpp`   | VBO gen/bind/data                                       | `gen_buffer()` + `buffer_data()`                    |
| `gol_tex.cpp`      | `glGenTextures` / `glTexImage2D` / 3D variants          | `gen_texture_2d` / `gen_texture_3d` / `texture_data_2d` |
| `gol_shader.cpp`   | GLSL compile + link                                     | `create_program_msl` (Win backend compiles GLSL not MSL ; same entry-point semantics) |
| `gol_draw.cpp`     | `glDrawArrays` / `glDrawElements`                       | `draw_arrays()` + `draw_elements()`                 |
| `gol_color.cpp`    | `glClear` + clear-colour state                          | `begin_render_pass()` with `LoadAction::Clear` + `clear_color[4]` |
| `gol_list.cpp`     | Display lists (deprecated GL)                           | NO-OP on `WindowsBackend` ; deprecated, not in abstract interface |

The `create_program_msl` name is historical : on the Win side it should
accept GLSL source text (entry points = "main" for both stages by
convention) and the abstract interface treats the source string as
opaque platform-native shader source. A later cleanup may rename to
`create_program_source` ; deferred to keep the c20-c22 contract stable.

---

## Section 3 — Refactor strategy

### Step 3.1 — Create the concrete class

New files :

```
src/gol/windows/windows_backend.h    (mirrors src/gol/metal/metal_backend.h shape)
src/gol/windows/windows_backend.cpp  (mirrors src/gol/metal/metal_backend.cpp impl)
```

Class shape :

```cpp
// src/gol/windows/windows_backend.h
#pragma once
#include "src/gol/gol_backend.h"

namespace GOL
{
    class WindowsBackend final : public Backend
    {
    public:
        WindowsBackend();
        ~WindowsBackend() override;

        bool init() override;
        void deinit() override;
        DeviceInfo get_device_info() const override;
        // ... mirror every pure-virtual in src/gol/metal/metal_backend.h
        char const* backend_name() const override { return "WindowsBackend"; }

    private:
        // wgl + GL state opaque handles ; nothing leaks into the .h.
        struct Impl;
        std::unique_ptr<Impl> _impl;
    };
}
```

### Step 3.2 — Wire the factory

`src/gol/gol_backend_factory.cpp` (currently Mac-only ; sees only
`MetalBackend::create_default_backend()`) needs a `#if AAA_OS_WINDOWS()`
branch :

```cpp
GOL::Backend* GOL::create_default_backend()
{
#if AAA_OS_MAC()
    return new GOL::MetalBackend();
#elif AAA_OS_WINDOWS()
    return new GOL::WindowsBackend();
#else
    #error "no default backend for this platform"
#endif
}
```

### Step 3.3 — Implement each virtual as a thin GL wrapper

Each pure-virtual in `gol_backend.h` maps to 1-3 GL calls. Reference
the table in Section 2 ; for fan-out cases (e.g. `bind_program` ->
`glUseProgram` + viewport restoration + blend-state re-application)
fold the state into `WindowsBackend::Impl` so the calls are idempotent.

Storage modes : on Win the buffer-usage hint maps to GL_STATIC_DRAW /
GL_DYNAMIC_DRAW / GL_STREAM_DRAW respectively (1:1 with the
`BufferUsage` enum in the abstract header).

Texture formats : the abstract `TextureFormat::RGBA8` -> GL_RGBA8 ;
`BGRA8` -> GL_BGRA / GL_RGBA8 (driver swizzle) ; `Depth32F` ->
GL_DEPTH_COMPONENT32F. The Mac side documents `BGRA8` because it's
the MTKView default ; the Win side should accept it as a valid format
even though the wgl swap-chain is RGBA.

### Step 3.4 — Gate the existing direct-GL call sites

The legacy code in `vendor/aaaseed-engine/Src/gol/gol_*.cpp` keeps
working AS-IS for the upstream MSVC build. On the abstract path,
callers route through `GOL::create_default_backend()` instead of
directly into `gol::draw_triangle()`-style fixed-function entries.

DO NOT modify `vendor/aaaseed-engine/Src/gol/*` to gate calls (the
"no touching vendor" rule from the Mac side applies in reverse). The
new `WindowsBackend` is the ADAPTER ; the legacy fixed-function code
stays intact and continues to compile on the upstream MSVC target.

---

## Section 4 — Phase 3 EXIT criterion

Per `memory/todo.md` Phase 3 final bullet :

> Phase 3 exit criterion : one golden MEU (e.g. MEU_Video) running
> pixel-comparable output on both backends, with frame-time within
> 1.2x of the Windows reference on equivalent hardware.

Concrete test plan :

1. Pick one Path A revival shader from the Mac catalog. Recommended :
   `src/shaders/msl/aaa_bloom_real.metal` (deterministic, 4-pass, no
   external textures except a procedural fixture).
2. Hand-port the MSL to GLSL with same uniform layout. Land at
   `vendor/aaaseed-engine/Src/shaders_glsl/aaa_bloom_real.frag` ON
   THE WIN REPO ONLY.
3. Mac side renders to a 256x256 RGBA8 texture, reads back, writes
   to `tests/regression/golden/aaa_bloom_real.png` (already exists).
4. Win side runs the same shader through `WindowsBackend`, reads
   back, compares against the golden.
5. Pass criterion : 100% pixel-match within 16 LSB per channel
   (matches the c121-A regression doctrine baseline).

A 16-LSB tolerance is the doctrine ceiling ; tighter tolerances are
acceptable shader-by-shader as the catalog grows.

---

## Section 5 — Frame-time comparison

Reference frame budget : Apple M4 unified-memory, 1280x720 BGRA8,
single Path A shader, single full-screen quad. Mac side already
collects this via `tests/unit/gol_metal_perf_test.cpp` (label `perf`).

Win-side runbook :

1. Implement a `windows_backend_perf_test.cpp` mirroring the Mac
   target's per-frame timing harness.
2. Measure mean wall-clock frame time over 1000 frames at the same
   resolution + same shader.
3. Assert `mean_win_ms <= 1.2 * mean_mac_ms` (or the reverse for the
   "Windows is reference" framing per Phase 3 EXIT wording).
4. Equivalent hardware : Apple M4 vs Intel Core Ultra 7 + RTX 4070.
   The 1.2x cap is generous for the unified-memory advantage on Mac.

If the assertion fails, the failure does NOT close Phase 3 — it opens
a perf-investigation thread. The 1.2x cap is the SUCCESS criterion,
not a hard build gate.

---

## Section 6 — Hard constraints

These are non-negotiable :

- **Mac must continue building from the same source tree.** No Mac
  branches inside Win-specific code. No `#if defined(_WIN32)` inside
  Mac source files. The split lives at the directory boundary :
  `src/gol/metal/` vs `src/gol/windows/`.
- **Windows must continue building from the existing engine vendor
  tree.** The legacy fixed-function paths in
  `vendor/aaaseed-engine/Src/gol/*` stay intact. `WindowsBackend` is
  an ADAPTER, not a replacement.
- **The abstract `gol_backend.h` header is frozen for Mac-only types.**
  No `MTL::*`, no `NS::*`, no `CA::*`, no `<Metal/Metal.h>`, no
  `<Foundation/Foundation.h>` includes. The regression-guard test at
  `tests/unit/gol_backend_cross_platform_test.cpp` enforces this on
  every Mac-side build. If a Win-side need surfaces that doesn't
  map cleanly, ADD a new opaque-handle abstraction to the interface
  (do NOT leak GL types either).
- **No SPIR-V / Vulkan IR anywhere.** The Win backend compiles GLSL
  natively ; the Mac backend compiles MSL natively. Per
  `memory/feedback_nvidia_to_metal.md`.
- **No `o_str` / `aaa_str.h` / `aaa_mem.h` in the new
  `src/gol/windows/`** subtree. Pure `std::string` + `std::uint32_t`
  per `memory/feedback_hermetic_mac_sublibs.md` (the hermetic
  doctrine applies to the cross-platform boundary too).

When the Win-side session lands, append a completion line to
`memory/handover_session.md` and flip Task #152's `[ ]` to `[x]` in
`memory/todo.md` Phase 3 section (line ~98). The Mac-side
prerequisite is COMPLETE as of c126-c130, 2026-05-27.

---

## Appendix A — Cross-platform interface audit summary

The full audit lives in the regression-guard test, but for reference :

| Abstract method                | Win-side mapping                            | Mac-side mapping                           |
| ------------------------------ | ------------------------------------------- | ------------------------------------------ |
| `init()`                       | wglCreateContext + wglMakeCurrent           | MTL::CreateSystemDefaultDevice             |
| `deinit()`                     | wglDeleteContext                            | release MTL handles                        |
| `get_device_info()`            | `glGetString(GL_RENDERER)`                  | `MTL::Device::name()` + supportsFamily()   |
| `gen_buffer()`                 | `glGenBuffers(1, &id)`                      | `device->newBuffer(...)` (lazy alloc)      |
| `buffer_data(...)`             | `glBufferData(target, size, data, usage)`   | `buffer->contents()` memcpy                |
| `gen_texture_2d(...)`          | `glGenTextures` + `glTexImage2D`            | `device->newTexture(desc)`                 |
| `texture_data_2d(...)`         | `glTexSubImage2D`                           | `texture->replaceRegion(...)`              |
| `gen_texture_3d(...)`          | `glTexImage3D`                              | `device->newTexture(3d desc)`              |
| `create_program_msl(...)`      | `glCreateShader` + `glCompileShader` + link | `device->newLibrary(MSL)` + newFunction    |
| `create_program(desc)`         | same + glVertexAttribPointer                | RenderPipelineDescriptor                   |
| `begin_render_pass(desc)`      | `glBindFramebuffer` + `glClear*`            | `MTL::RenderPassDescriptor` + encoder      |
| `end_render_pass()`            | `glFlush` (FBO unbind)                      | `encoder->endEncoding()` + commit          |
| `set_viewport(...)`            | `glViewport(x, y, w, h)`                    | `encoder->setViewport(...)`                |
| `bind_program(id)`             | `glUseProgram(id)`                          | `encoder->setRenderPipelineState(...)`     |
| `bind_vertex_buffer(...)`      | `glBindBuffer(GL_ARRAY_BUFFER, ...)`        | `encoder->setVertexBuffer(...)`            |
| `bind_fragment_buffer(...)`    | `glBindBuffer(GL_UNIFORM_BUFFER, ...)`      | `encoder->setFragmentBuffer(...)`          |
| `bind_fragment_texture(...)`   | `glActiveTexture` + `glBindTexture`         | `encoder->setFragmentTexture(...)`         |
| `bind_fragment_texture_3d(...)`| `glActiveTexture` + `glBindTexture(3D)`     | same Mac path (Metal disambiguates by type)|
| `draw_arrays(...)`             | `glDrawArrays(prim, first, count)`          | `encoder->drawPrimitives(...)`             |
| `draw_elements(...)`           | `glDrawElements(...)`                       | `encoder->drawIndexedPrimitives(...)`      |
| `read_texture_pixels(...)`     | `glReadPixels` on bound FBO                 | `texture->getBytes(...)`                   |
| `present()`                    | `SwapBuffers(hdc)`                          | `commandBuffer->presentDrawable(...)`      |
| `get_last_error()`             | shader info-log capture in `_last_error`    | newLibrary/newFunction NS::Error -> string |
| `backend_name()`               | returns "WindowsBackend"                    | returns "MetalBackend"                     |

No Mac-only leak found. Every method has a clean Win-side mapping.
The abstract interface is genuinely cross-platform.
