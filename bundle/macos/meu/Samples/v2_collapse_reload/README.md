# Sample MEU : v2_collapse_reload

The canonical demo for the v2 Phase 4 collapsing panels + hot-reload
button. Mac-native counterpart to vendor GaBu's
`add_bu_param_group_expandable` (per-section expand/collapse) and a
QoL "rebuild this MEU" affordance that completes the in-app authoring
loop introduced in c147 + c148.

## What it does

Opens a 360x280-pixel main panel containing :

- An `Effect` collapsing section with an `intensity` slider in
  `[0.0, 2.0]`. Click the section header to collapse / expand.
- A `Fog` collapsing section with a `density` slider in `[0.0, 1.0]`.
- A `Reload MEU` hot-reload button at the bottom. Clicking it closes
  + re-opens the Lua state, re-runs the script, resets all locals to
  their declared initial values.

The HUD line below echoes
`intensity=X.XX density=X.XX last_reload=N` where `N` is the frame
the most recent reload happened (or 0 if never).

## Widget bindings exercised

- `aaa.ui.begin_panel` / `aaa.ui.slider` / `aaa.ui.end_panel` -- from
  Phase 1.
- `aaa.ui.begin_collapsing_panel( title, x, y, w, h ) -> expanded` --
  new v2 Phase 4. Returns the expanded state ; child widgets are only
  emitted when `expanded == true`.
- `aaa.ui.end_collapsing_panel()` -- new v2 Phase 4. Closes the
  collapsing block ; must pair 1:1 with `begin_collapsing_panel`.
- `aaa.ui.hot_reload_button( label? )` -- new v2 Phase 4. Triggers
  `Runner::reload()` on click. Returns rising-edge `true`.

See `docs/AUTHORING_MEUS_ON_MAC.md` section 3.8
"Collapsing panels + hot reload" for the full binding reference.

## Path A shader used

- `aaa_bloom_real.metal` -- Lottes-style bloom (c138-A revival).
  Float slot 0 = intensity (gain). Float slot 1 = fog density.

## How to modify

Edit `collapse_reload_demo.lua` :

- Add a third collapsing section by repeating the
  `begin_collapsing_panel` / `end_collapsing_panel` pattern with new
  y coordinates.
- Swap the slider with `aaa.ui.hsv_color_picker` (Phase 2) inside a
  collapsing section to combine the two new feature sets.
- Change the hot-reload button label to something MEU-specific
  (e.g. "Re-roll seed").

## Hot-reload semantics

Clicking the hot-reload button calls `Runner::reload()` which :

- Closes the current `lua_State`.
- Re-reads the same script file from disk (so external edits land).
- Re-opens a fresh `lua_State`, re-registers all `aaa.*` bindings,
  re-runs the chunk.
- Resets all Lua locals to their declared initial values
  (`effect_intensity = 1.0`, etc.).

This is the canonical way to "reset all sliders" mid-session AND the
canonical way to pick up an external text-editor change without
restarting the .app.

## v2 Phase 4 boundary

One-level nesting only (collapsing panels can live inside `begin_panel`
but not inside each other). File-watch hot-reload (auto-trigger when
the script changes on disk) is deferred to v3 (see
`memory/project_v1_ship_gate.md` "Authoring Surface" section).
