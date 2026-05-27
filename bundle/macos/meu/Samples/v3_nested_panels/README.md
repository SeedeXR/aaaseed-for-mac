# Sample MEU : v3_nested_panels

The canonical demo for v3 deep-nested collapsing panels (c149-A).
Closes the "one-level nesting only" Phase 4 limitation from v2
by demonstrating two depth-2 sub-panels inside a single depth-1
parent, plus a sibling depth-1 panel.

## What it does

Opens a 380x360-pixel main panel containing :

- A depth-1 `Effects` collapsing panel that contains two
  depth-2 sub-panels :
  - `Color` (with an `intensity` slider in `[0.0, 2.0]`).
  - `Bloom` (with `radius` in `[1.0, 16.0]` + `threshold` in
    `[0.0, 2.0]` sliders).
- A sibling depth-1 `Atmosphere` collapsing panel with a
  `fog density` slider in `[0.0, 1.0]`.

The HUD line below echoes
`i=X.XX r=X.X t=X.XX fog=X.XX` so a screenshot captures the
full nested-state snapshot in one frame.

## Widget bindings exercised

- `aaa.ui.begin_panel` / `aaa.ui.slider` / `aaa.ui.end_panel`
  -- from Phase 1.
- `aaa.ui.begin_collapsing_panel( title, x, y, w, h ) ->
  expanded` -- from Phase 4, extended in v3 to allow nesting
  inside another `begin_collapsing_panel`.
- `aaa.ui.end_collapsing_panel()` -- pairs 1:1 with each
  `begin_collapsing_panel` that returned `expanded == true`.

See `docs/AUTHORING_MEUS_ON_MAC.md` section 3.8 "Collapsing
panels + hot reload" (v3 nesting subsection) for the depth-
rules + idiomatic-collapse-pattern reference.

## Path A shader used

- `aaa_bloom_real.metal` -- Lottes-style bloom (c138-A
  revival). Float slot 0 = threshold (luminance gate). Float
  slot 1 = intensity (gain).

## How to modify

Edit `nested_panels_demo.lua` :

- Add a depth-3 sub-panel inside `Color` for chroma channels
  (R/G/B individually). The widget renderer auto-indents
  arbitrarily deep ; the practical limit is screen real estate.
- Convert one of the depth-1 panels to a non-collapsing
  `begin_panel` to demonstrate mixed-mode layout.
- Add an `aaa.ui.hsv_color_picker` (Phase 2) inside the
  `Color` sub-panel to combine v2 Phase 2 + v3 nesting.

## v3 boundary

- Nesting is unlimited in depth at the widget-renderer level,
  but the MEU author is responsible for laying out the rects
  (no auto-stacking yet).
- Collapse state is per-panel-per-session ; it does not
  persist across `Runner::reload()`. Use a preset
  (`v3_preset_save_load`) to persist parameter values.
- Drag-to-reorder sub-panels is v3.1 (would require a
  per-panel registry the widget layer does not yet maintain).
