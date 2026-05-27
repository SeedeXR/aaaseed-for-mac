# Sample MEU : v2_widgets

The canonical demo for the v2 Phase 1 immediate-mode widget surface.
Mac-native counterpart to the Windows GaBu BU widget shelf
(`vendor/aaaseed-runtime/AAAKernel/GaBu/`), rendered natively inside
`AAASeedMTKView` without the Layer subsystem and without GaBu's render
path.

## What it does

Opens a 320x220-pixel panel at the top-left of the window containing :

- A `Cycle shader` button that walks `aaa_noise_real ->
  aaa_bloom_real -> aaa_motion_blur_real` and back.
- An `intensity` slider in [0.0, 2.0] -- bound to scalar uniform slot 0.
- A `frequency` slider in [0.1, 8.0] -- bound to scalar uniform slot 1.
- A `tint` color-well -- bound to vec4 uniform slot 0 (RGBA).

The HUD line below the panel echoes the live state for QA + screenshots.

## Path A shaders used

- `aaa_noise_real.metal` -- Perlin + Simplex + FBM (c135-A revival).
- `aaa_bloom_real.metal` -- Lottes-style bloom (c138-A revival).
- `aaa_motion_blur_real.metal` -- camera-velocity projected (c139-A revival).

## Widget bindings exercised

All five `aaa.ui.*` bindings in v2 Phase 1 :

- `aaa.ui.begin_panel( title, x, y, w, h )` -- container.
- `aaa.ui.button( label )` -- rising-edge click signal.
- `aaa.ui.slider( label, value, min, max )` -- float scrubber.
- `aaa.ui.color_well( label, r, g, b, a )` -- RGBA picker (preset cycler).
- `aaa.ui.end_panel()` -- close the container.

See `docs/AUTHORING_MEUS_ON_MAC.md` section "aaa.ui.* widget API (v2 Phase 1)"
for the full reference.

## How to modify

Edit `widgets_demo.lua` :

- Add or remove shader names in the `shaders` table.
- Change slider ranges (the `min` / `max` args) to fit a different uniform.
- Swap `set_uniform_vec4` slots if your target shader binds tint to a
  different slot.

Reload : `runner.reload()` (API-level supported, key-binding wiring is
v2 polish ; rebuild `aaaseed_app` for now).

## Why this is not a literal Windows port

The Windows GaBu BU widget surface (`add_param_obj_name`,
`add_bu_texture_target_unit`, etc.) depends on the engine Layer subsystem
which the Mac port superseded per `memory/project_layer_supersession.md`
(c144-B). The c147-A Mac widget layer is a fresh implementation that
renders inside `AAASeedMTKView`'s Metal pipeline -- no GaBu, no NSPanel,
no Cocoa controls. Lua API surface matches the spirit of GaBu (declare
the widget, get a value back) without dragging in the Windows render
path.

## What's NOT here (v2 Phase 2)

Full HSV color picker, modal popups, nested panels, keyboard text-input
fields, multi-line text. See `memory/project_v1_ship_gate.md` "Authoring
Surface" section for the v2 phase plan.
