# Sample MEU : v3_drag_drop

The canonical demo for the v3 drag-drop + open-file-dialog surface
(c149-A). Mac-native counterpart to the Windows "drop a .lua onto
the runtime window" workflow ; completes the file-I/O half of the
in-app authoring loop that c147 + c148 began.

## What it does

Opens a 360x200-pixel main panel containing :

- A HUD line echoing the most recently loaded path (or
  `(none yet)`).
- A `Browse for MEU script...` button that opens a Cocoa
  NSOpenPanel filtered to `.lua` files only.

In production the host `AAASeedMTKView` also registers as an
`NSDraggingDestination`, so the user can drag any `.lua` from
Finder onto the window and the runner loads it directly --
the button here is the manual fallback for previewing the dialog
flow.

## Widget + I/O bindings exercised

- `aaa.ui.begin_panel` / `aaa.ui.button` / `aaa.ui.end_panel` --
  from Phase 1.
- `aaa.io.open_file_dialog( title, ext_list ) -> path_or_nil` --
  new v3. Wraps the c131-B `src/ui/macos/aaa_file_dialog.{h,mm}`
  adapter ; returns the chosen path string or `nil` if the user
  cancelled. `ext_list` is a Lua array of extension strings
  without leading dots (e.g. `{ "lua" }`).
- `aaa.io.drop_file( path )` -- new v3 ; synthetic test injection
  that fires the same code path drag-drop onto the window
  triggers. Not exercised in this demo's `on_frame` body (it
  would re-load the MEU mid-frame and immediately discard the
  caller's stack), but documented + grep-discoverable for the
  c149-A integration tests.

See `docs/AUTHORING_MEUS_ON_MAC.md` section 4 "aaa.io.* file I/O
API (v3)" for the full binding reference.

## Path A shader used

- `aaa_noise_real.metal` -- Perlin + Simplex + FBM (c135-A
  revival). Gives the drag-drop flow a visual context so the
  drag-target window is clearly alive.

## How to modify

Edit `drag_drop_demo.lua` :

- Add a second extension to the filter : `{ "lua", "plua" }`
  to also accept Windows preset files.
- Replace the noise shader with `aaa_bloom_real` to make the
  drag-target look like a finished scene.
- Add an `aaa.io.save_file_dialog` call alongside the open one
  to demonstrate the round-trip (see `v3_preset_save_load` for
  a focused save/load demo).

## v3 boundary

- Drag-drop is `.lua` files only ; other extensions are
  rejected at the NSDraggingDestination level (c149-A's
  filter).
- The Lua sandbox cannot trigger a script reload directly --
  drag-drop onto the WINDOW is the canonical reload path.
  `aaa.io.drop_file` is synthetic / test-only.
- File quarantine attributes are logged but not enforced
  (Gatekeeper handles the security boundary).
