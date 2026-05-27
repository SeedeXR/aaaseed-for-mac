# Sample MEU : v2_color_picker

The canonical demo for the v2 Phase 2 HSV color picker. Mac-native
counterpart to a "real" color-picker widget : Phase 1 only cycled
through a preset palette via `aaa.ui.color_well` ; Phase 2 introduces
`aaa.ui.hsv_color_picker` -- a drag-to-hue immediate-mode primitive
built on the Smith 1978 HSV<->RGB algorithm. Rendered natively inside
`AAASeedMTKView` -- no NSColorWell, no Cocoa controls.

## What it does

Opens a 360x280-pixel panel at the top-left of the window containing :

- A `tint` HSV color picker with a hue triangle, value bar, and alpha
  slider. Drag inside any region to change the corresponding component.

The HUD line below the panel echoes the live picked RGBA as
`HSV picker RGB=(r,g,b,a)` for QA + screenshots. The picked RGBA
pushes directly into `aaa_noise_real`'s vec4 slot 0 (tint uniform) so
the on-screen pixels reflect the picker output every frame.

## Path A shader used

- `aaa_noise_real.metal` -- Perlin + Simplex + FBM (c135-A revival).
  Slot 0 vec4 = tint color.

## Widget bindings exercised

- `aaa.ui.begin_panel( title, x, y, w, h )` -- container.
- `aaa.ui.hsv_color_picker( label, r, g, b, a )` -- new v2 Phase 2
  primitive. Returns r, g, b, a after user interaction.
- `aaa.ui.end_panel()` -- close the container.

See `docs/AUTHORING_MEUS_ON_MAC.md` section 3.6
"`aaa.ui.hsv_color_picker`" for the full binding reference.

## How to modify

Edit `color_picker_demo.lua` :

- Change the initial `rgba` table for a different starting hue.
- Swap `aaa_noise_real` for any other Path A shader that accepts a
  vec4 tint in slot 0 (e.g. `aaa_bloom_real`).
- Stack the picker with a Phase 1 slider to add a non-color uniform
  to the same panel.

## Why HSV (not RGB sliders)

HSV separates chroma (hue + saturation) from value (brightness), which
matches how artists think about color. RGB sliders force the artist to
coordinate three numbers to change "brightness" alone -- HSV's value
bar does that with one drag. See the Smith 1978 paper cited inline in
`src/ui/widgets/aaa_widgets_mac.mm`.

## v2 Phase 2 boundary

This demo stops at single-color picking. Multi-color gradients,
palette save/load, and color-blind preview filters are deferred to v3
(see `memory/project_v1_ship_gate.md` "Authoring Surface" section).
