# Project memory — Layer subsystem (Task #31) formal supersession

**Date codified :** 2026-05-27 (continuation 145).
**Authority :** c142-C definitive gap inventory + c120 strategic pivot.
**Companion memories :** `[[doctrine-limits]]` (`feedback_doctrine_limits.md`), `[[v1-ship-gate]]` (`project_v1_ship_gate.md`), `[[hermetic-mac-sublibs]]` (`feedback_hermetic_mac_sublibs.md`).

---

## TL;DR

Task #31 — "Port the Windows layer subsystem (~7 files / ~14 .cpp / ~6.7 K LOC core)" — is **formally superseded** by the c142-B MEU Runner + Path A catalog + MetalBackend, NOT by a literal port. The Mac-native equivalent of `vendor/aaaseed-engine/Src/infrastructure/layer/` is `src/meu/aaa_meu_runner_mac.{h,mm}` plus the 141-shader Path A catalog at `src/shaders/msl/` plus `src/gol/metal/MetalBackend`. Task #31 will NOT be reopened in its literal form unless a vendor patch authorizes touching `vendor/aaaseed-engine/Src/infrastructure/layer/`.

---

## Why a literal port is the wrong tool

The vendor layer subsystem is a four-deep fixed render graph (`c_layer` -> `c_layers` -> `c_module` -> `c_modules`) wired to (a) the GL fixed-function pipeline, (b) the `c_obj_ui` factory tree, (c) `c_traxs` legacy animation system, (d) the `c_lua_wrap` engine-side Lua binding layer. Mac-cleaning even one core file (`layer.cpp`, 2486 LOC) requires dragging :

- `draw/` (cameras, lights, materials, render state, fog, stencil, texturing) — ~20.5 K LOC of GL-bound code.
- `obj_ui/bdd/` (80+ files / ~15 K LOC of bdd_* drawers, all GL immediate-mode).
- `c_traxs` animation system.
- `c_lua_wrap` engine-side Lua binding mesh.

The c119-A faked-class compile experiment hit the **doctrine-limits wall** (`feedback_doctrine_limits.md`) on exactly this subsystem : the layer adapter fan-out grew shim count faster than it shrank error count after 2 iterations. The doctrine codified at that point : **stop, pivot, do not iterate**.

c120 pivoted to a different end-state : instead of porting the Windows render-graph orchestrator, the Mac side **builds an equivalent orchestrator from Mac-native pieces**. The result has the same observable surface (Lua scripts drive per-frame draws against an abstract backend) without sharing a single line of C++ with the Windows side.

---

## The Mac-native equivalent

| Windows file (`vendor/aaaseed-engine/Src/infrastructure/layer/`) | Mac-native equivalent | Notes |
|---|---|---|
| `layer.cpp` (2486 LOC) — owns one drawer + per-layer attribute stack | `src/meu/aaa_meu_runner_mac.{h,mm}` (c142-B) — one MEU runner per scene, drives MetalBackend directly. | The drawer concept is collapsed : the runner exposes `use_shader()` + `draw_fullscreen_quad()` Lua bindings, the script picks its own drawer per frame. No layered attribute stack ; Metal pipeline state objects (PSOs) cache per-shader. |
| `layer.h` — public layer interface | `src/meu/aaa_meu_runner_mac.h` `aaa::meu::Runner` class. | Same role : per-frame entry point, life-cycle (load_script / unload / reload), input dispatch. |
| `module.cpp` (1289 LOC) — group of layers + lights + Lua + cameras + values + traxs | Each `.lua` script loaded by the runner. | The script is the module. Composition (multi-layer scenes) is the script author's choice via Lua tables ; no fixed-depth C++ render graph. |
| `module.h` | Implicit — the runner's `load_script(path)` API is the module-load entry point. | No header to mirror : a module is "a file the runner can load". |
| `modules.cpp` (974 LOC) — top-of-engine group of modules | `src/ui/macos/AAASeedMTKView.mm` (c143-C wiring) holds the single live `aaa::meu::Runner`. | The four-deep Win render graph collapses to two-deep on Mac : MTKView -> Runner -> Lua. Composition above two-deep is the script's job, not the engine's. |
| `app.cpp` (447 LOC), `app.h` — global `c_app` (singleton `g_app`), render settings | `src/macos/AAASeedAppDelegate.{h,mm}` + `bundle/macos/Info.plist.in` | The app object becomes the Cocoa app delegate ; global render settings live in MetalBackend init params + the Info.plist + the script. |
| `layers.cpp` (1532 LOC), `layers.h` — sequence of up to 26 `c_layer` (a..z) | NOT NEEDED on Mac. | The MEU runner manages one scene at a time. Multiple "layers" on Mac are the script's responsibility (multiple shader passes within one frame ; the script can drive any number sequentially). The fixed-26-letter convention was a Windows-side UI affordance not a rendering requirement. |
| `layer_att.cpp`, `layers_att.cpp` — per-layer attribute current/owner/none mode stacks | NOT NEEDED on Mac. | The Win attribute system is the GL state-machine wrapper. On Metal, state lives in the PSO / render encoder ; no inheritance / push/pop needed. |
| `*_lua.cpp` files (layer_lua, layers_lua, module_lua, modules_lua) — engine-side bindings | `src/meu/aaa_meu_runner_mac.mm` raw `lua_pushcfunction` bindings (c124-A precedent). | Hermetic Mac binding surface (`aaa.use_shader`, `aaa.draw_fullscreen_quad`, `aaa.target`, `aaa.key_down`, `aaa.mouse_xy`, ...). Different naming convention from the engine's `AAALUACALL` macros, by design (hermetic sub-lib doctrine prevents pulling the engine binding macro mesh). |

**Surface equivalence proof :** continuation 143's `hello_world.lua` cycles 11 Path A revivals on Space-press, with FPS HUD + input dispatch + MetalBackend draws — i.e. the user-observable behavior of "load a scene, see it animate, press keys to interact" is achieved end-to-end without a single .cpp from `vendor/aaaseed-engine/Src/infrastructure/layer/`.

---

## When to revisit

Task #31 reopens as a re-port ONLY IF :

1. **Vendor authorization is granted** for editing `vendor/aaaseed-engine/Src/infrastructure/layer/`. Today the engine source is read-only on the Mac side (only the documented upstream patches in `todo.md` "Upstream patches applied" section have been touched, all behavior-preserving on Windows by inspection).

2. **A user-surfaced need** demands literal cross-platform binary parity for an asset authored in the Windows AAASeed `c_layer` UI (e.g. a `.deproj` that depends on a `layer_att` mode that the Mac MEU runner cannot reproduce via Lua).

3. **A Win-side counterpart task (Task #152 WindowsBackend extraction)** lands first, since cross-platform binary parity requires the Win side to ALSO route through the same abstract backend interface as Mac.

Without all three conditions, Task #31 stays superseded. The Mac side ships v1 with the MEU runner equivalent.

---

## What's preserved from the original task

The Stage 1 header-parse tests (`aaaseed_layer_app_compile_tests`, c111 / c112) for `layer.h` + `app.h` are **kept in the build**. They guard the engine header chain from regressions caused by upstream patches : if a future upstream patch breaks the layer header parse on AppleClang, these tests fail, and we know to push the patch back upstream. They're regression guards, not a port.

The Path A 141-shader catalog at `src/shaders/msl/` is the **rendering** equivalent of what the layer subsystem's `c_bdd` drawer used to drive on Windows. Each shader is a self-contained MSL file with explicit uniforms ; the layer subsystem's "drawer + attribute stack" model is replaced by "one shader per effect + uniforms-as-data" model.

---

## Cross-references

- `memory/feedback_doctrine_limits.md` — the doctrine that codified "stop after 2 iterations when shim count grows faster than error count shrinks", which is why c119-A pivoted to c120.
- `memory/project_v1_ship_gate.md` (c142-C) — the gap inventory that listed Task #31 as a candidate for formal closure ; this memo executes that closure.
- `memory/feedback_hermetic_mac_sublibs.md` — the doctrine that the MEU runner + this superseder follow : std::string only, no `o_str`, no `aaa_mem`, no engine link.
- `src/meu/aaa_meu_runner_mac.h` — the live Mac-native runner contract.
- `src/shaders/msl/` — the Path A catalog (141 shaders as of c139).

---

## Status

**Task #31 : SUPERSEDED.** Closed by referencing this memo. Not eligible for reopening without vendor authorization (see "When to revisit" above).
