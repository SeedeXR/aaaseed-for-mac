# Sample MEU : v3_preset_save_load

The canonical demo for the v3 preset save/load surface (c149-A).
Mac-native counterpart to the Windows `preset_*.plua` cascade ;
serializes the entire `aaa.ui.*` widget-state map to a single
Lua-table file an artist can name, version-control, share with
a teammate, and reload on a future session.

## What it does

Opens a 360x280-pixel main panel containing :

- An `intensity` slider in `[0.0, 2.0]` driving the bloom gain
  uniform.
- A `main_color` HSV picker driving the bloom tint uniform.
- A `Save preset...` button that opens NSSavePanel + writes the
  current widget state to the chosen file.
- A `Load preset...` button that opens NSOpenPanel + restores
  widget state from the chosen file.
- A HUD line echoing the most recent operation status.

## Widget + I/O bindings exercised

- `aaa.ui.begin_panel` / `aaa.ui.slider` / `aaa.ui.button` /
  `aaa.ui.end_panel` -- from Phase 1.
- `aaa.ui.hsv_color_picker` -- from Phase 2.
- `aaa.io.save_file_dialog( title, default_name, ext_list ) ->
  path_or_nil` -- new v3. Wraps the c131-B
  `src/ui/macos/aaa_file_dialog.{h,mm}` NSSavePanel adapter ;
  returns the chosen save path or `nil` on cancel.
- `aaa.io.open_file_dialog( title, ext_list ) -> path_or_nil` --
  new v3. Wraps the c131-B NSOpenPanel adapter.
- `aaa.io.save_preset( path ) -> bool` -- new v3. Serializes the
  current widget-state map to a Lua-table file. Returns true on
  success.
- `aaa.io.load_preset( path ) -> bool` -- new v3. Deserializes a
  Lua-table file into the runner widget-state map. Returns true
  on success.

See `docs/AUTHORING_MEUS_ON_MAC.md` section 4 "aaa.io.* file I/O
API (v3)" for the full binding reference.

## Path A shader used

- `aaa_bloom_real.metal` -- Lottes-style bloom (c138-A revival).
  Float slot 0 = intensity gain. Vec4 slot 0 = tint.

## Preset file format

The serialized format is a single Lua chunk that returns a table :

```lua
return {
    intensity = 1.0,
    main_color = { 1.0, 0.5, 0.2, 1.0 },
    -- ... one entry per registered widget id
}
```

Hand-editable. Version-control-friendly. Forward-compatible :
widgets whose IDs no longer exist in the running MEU are
silently skipped on load.

## How to modify

Edit `preset_demo.lua` :

- Add a second slider for `bloom_radius` and watch it persist.
- Replace the bloom shader with `aaa_noise_real` to demonstrate
  preset round-trip over a different effect family.
- Add an "Export preset to clipboard" button using
  `aaa.io.save_preset` with a temp-file path + `aaa.log` of the
  contents.

## v3 boundary

- Preset files contain WIDGET STATE only -- not the .lua source.
  To share both, share the .lua + the .preset.lua side-by-side.
- `aaa.io.save_preset` writes via a standard NSURL file handle ;
  it does NOT honor App-Sandbox containers (out-of-scope for v1
  ship, which is non-sandboxed).
- Auto-save on quit / restore on launch is v3.1 (would need a
  canonical "last preset" path).
