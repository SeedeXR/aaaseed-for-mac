# Sample MEU : v2_modal_textinput

The canonical demo for the v2 Phase 3 modal-dialog + text-input combo.
Mac-native counterpart to the "rename preset" workflow common across
parameter-authoring UIs : user clicks a button, a centered popup
appears over a darkened scene, the user types a new name, clicks OK
or Cancel, the popup closes.

## What it does

Opens a 340x200-pixel main panel showing the current `preset` name +
a `Rename preset...` button. When clicked, a 280x140-pixel modal pops
up centered on the window with :

- A single-line `name` text-input pre-filled with the current preset
  name.
- An OK button (commits the typed name -> updates the main panel).
- A Cancel button (discards the change).

The background scene uses `ps_Maa_add_scale` for a simple gradient
that the modal backdrop darkens, so the modal is visually distinct.

## Widget bindings exercised

- `aaa.ui.begin_panel` / `aaa.ui.button` / `aaa.ui.end_panel` -- from
  Phase 1.
- `aaa.ui.show_modal( id )` -- new v2 Phase 3. Asks the runner to
  display the modal with the given id on subsequent frames until the
  user OKs or Cancels it.
- `aaa.ui.begin_modal( id, w, h ) -> open, ok, cancel` -- new v2
  Phase 3. Three booleans drive the modal lifecycle.
- `aaa.ui.text_input( label, value, max_length )` -- new v2 Phase 3.
  Single-line ASCII keyboard input.
- `aaa.ui.end_modal()` -- new v2 Phase 3. Closes the modal block ;
  must pair 1:1 with each `begin_modal` that returned `open == true`.

See `docs/AUTHORING_MEUS_ON_MAC.md` section 3.7 "Modals + text input"
for the full binding reference.

## Path A shader used

- `ps_Maa_add_scale` -- additive-scale shader used as a backdrop.
  Any Path A shader can substitute ; the demo picks this one because
  its gradient makes the modal's darkened backdrop visible.

## How to modify

Edit `modal_textinput_demo.lua` :

- Change `max_length` (default 64) to widen / narrow the input.
- Add a second `aaa.ui.text_input` between `begin_modal` and
  `end_modal` to build a multi-field form (each input persists in
  its own Lua local).
- Replace `preset_name` commit logic with whatever your shader needs
  (e.g. push the typed name to `aaa.log` only, no persistence).

## v2 Phase 3 boundary

This demo deliberately uses ASCII-only single-line input. Multi-line
text, full keyboard focus traversal, IME (Japanese / Chinese / Korean
input), copy / paste via the NSResponder chain, and drag-drop file
targets are all deferred to v3 (see
`memory/project_v1_ship_gate.md` "Authoring Surface" section).
