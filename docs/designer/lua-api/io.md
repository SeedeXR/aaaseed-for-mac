# Lua API : `aaa.io.*` (file I/O)

> Reference for the 5 file-dialog + preset bindings under the
> `aaa.io.*` namespace, landed in v3 (c149-A). Source :
> `src/meu/aaa_meu_runner_mac.{h,mm}` + `src/ui/macos/aaa_file_dialog.{h,mm}`.

These bindings complete the file-I/O half of the in-app authoring
loop : load other MEUs via drag-drop or NSOpenPanel, save / load
widget state as portable `.lua` presets.

All five are call-from-anywhere (no panel-stack requirement). The
dialog bindings spin a **blocking** Cocoa sheet, so do NOT call them
from a tight per-frame loop body -- gate them behind a button click.

---

## `aaa.io.drop_file( path ) -> nil`

Synthetic test injection that simulates an NSWindow drag-drop event.
The **production** drag-drop path is the `AAASeedMTKView`
`registerForDraggedTypes:` + `performDragOperation:` responder chain
that c149-A wired into the host view : drag a `.lua` from Finder onto
the window, the runner's drag-drop hook calls `Runner::load_script`
and re-runs the chunk.

- `path` (string) -- absolute path to a `.lua` file. Non-`.lua`
  extensions are rejected at the responder-chain level for the
  production path ; the synthetic injection follows the same filter.
- Returns nothing.
- **Use case** : integration tests that need a deterministic "user
  dragged this file" event without an NSEvent loop. NOT for use from
  a production `on_frame` body -- it would reload the MEU mid-frame
  and discard the caller's stack.

```lua
-- From a test harness only.
aaa.io.drop_file( "/tmp/test_meu.lua" )
```

See also : [Sample MEU : v3_drag_drop](../samples.md#v3_drag_drop).

---

## `aaa.io.open_file_dialog( title, ext_list ) -> path_or_nil`

Lua-callable wrapper around the NSOpenPanel adapter. Opens a blocking
sheet dialog ; returns the chosen path string or `nil` if the user
cancelled.

- `title` (string) -- dialog title bar text.
- `ext_list` (Lua array of strings) -- allowed extensions WITHOUT
  leading dots. E.g. `{ "lua" }` or `{ "lua", "plua" }` or
  `{ "png", "jpg", "exr" }`.
- Returns either an absolute path string (on accept) or `nil` (on
  cancel).

```lua
if aaa.ui.button( "Open MEU..." ) then
    local path = aaa.io.open_file_dialog(
        "Choose a .lua MEU script", { "lua" } )
    if path then
        aaa.log( "selected " .. path )
    end
end
```

Common extension lists :

| Extension | Use case                          |
|-----------|-----------------------------------|
| `lua`     | MEU script files + preset files   |
| `plua`    | Windows-format preset (read-only) |
| `png`     | Texture asset import (v2)         |
| `txt`     | Plain-text notes export           |

See also : [`save_file_dialog`](#aaaiosave_file_dialog-title-default_name-ext_list-path_or_nil),
[Sample MEU : v3_drag_drop](../samples.md#v3_drag_drop).

---

## `aaa.io.save_file_dialog( title, default_name, ext_list ) -> path_or_nil`

Lua-callable wrapper around the NSSavePanel adapter. Same return
contract as `open_file_dialog` but for saving : the user picks a
destination, Cocoa prompts on overwrite, the call returns the chosen
path or `nil` on cancel.

- `title` (string) -- dialog title bar text.
- `default_name` (string) -- pre-filled into the Cocoa text field.
  The user can change it before clicking Save.
- `ext_list` (Lua array of strings) -- allowed extensions WITHOUT
  leading dots. Cocoa appends the first extension if the user typed
  a name without one.
- Returns either an absolute path string (on accept) or `nil` (on
  cancel).

```lua
if aaa.ui.button( "Save preset..." ) then
    local path = aaa.io.save_file_dialog(
        "Save preset", "my_preset.lua", { "lua" } )
    if path and aaa.io.save_preset( path ) then
        aaa.log( "saved to " .. path )
    end
end
```

See also : [`save_preset`](#aaaiosave_preset-path-bool),
[Sample MEU : v3_preset_save_load](../samples.md#v3_preset_save_load).

---

## `aaa.io.save_preset( path ) -> bool`

Serializes the current widget-state map (all `aaa.ui.*` widget values
the runner has captured this frame) to a Lua-table file at `path`.

- `path` (string) -- absolute path to write to. If the file exists it
  is overwritten ; if its directory does not exist the call returns
  `false`.
- Returns `true` on success, `false` on I/O error (unwritable path,
  full disk, etc.).

```lua
local status = ""

if aaa.ui.button( "Save..." ) then
    local path = aaa.io.save_file_dialog(
        "Save preset", "preset.lua", { "lua" } )
    if path and aaa.io.save_preset( path ) then
        status = "Saved : " .. path
    else
        status = "Save failed."
    end
end
```

See [Preset file format](#preset-file-format) below for the on-disk
schema.

---

## `aaa.io.load_preset( path ) -> bool`

Reverse of `save_preset` : reads the Lua-table file at `path`,
restores widget state to the saved values.

- `path` (string) -- absolute path to read from.
- Widgets whose IDs no longer exist in the running MEU are silently
  skipped (forward-compatible -- a preset saved by an older MEU
  loads cleanly into a newer one that dropped some widgets).
- After a successful load, the next `aaa.ui.slider` /
  `aaa.ui.hsv_color_picker` / etc. calls return the restored values,
  so the Lua locals catch up automatically through the
  return-and-reassign idiom.
- Returns `true` on success, `false` on I/O / parse error.

```lua
if aaa.ui.button( "Load..." ) then
    local path = aaa.io.open_file_dialog(
        "Load preset", { "lua" } )
    if path and aaa.io.load_preset( path ) then
        aaa.log( "loaded " .. path )
    end
end
```

See also : [`save_preset`](#aaaiosave_preset-path-bool),
[Sample MEU : v3_preset_save_load](../samples.md#v3_preset_save_load).

---

## Preset file format

`aaa.io.save_preset` writes a single Lua `return { ... }` chunk, with
one sub-table per widget-state category. The file is hand-editable
and version-control-friendly (it diffs cleanly in Git).

```lua
-- AAASeed preset (generated by aaa.io.save_preset)
return {
    slider_state = {
        intensity = 1.0,
        radius    = 8.0,
        density   = 0.5,
    },
    color_well_state = {
        tint       = { 1.0, 0.5, 0.2, 1.0 },
        main_color = { 0.2, 0.8, 0.4, 1.0 },
    },
    text_input_state = {
        name  = "my_preset",
        notes = "line one\nline two",
    },
}
```

Schema details :

| Key                  | Value type                                      | Source widget(s)                          |
|----------------------|-------------------------------------------------|-------------------------------------------|
| `slider_state`       | table mapping `label -> number`                 | `aaa.ui.slider`                           |
| `color_well_state`   | table mapping `label -> { r, g, b, a }` (array) | `aaa.ui.color_well`, `aaa.ui.hsv_color_picker` |
| `text_input_state`   | table mapping `label -> string`                 | `aaa.ui.text_input`, `aaa.ui.text_area`   |

Notes :

- Color values are stored as a **Lua array** `{ r, g, b, a }`, not a
  record `{ r=..., g=..., b=..., a=... }`. Components are in `[0, 1]`.
- Widget collapse state (`begin_collapsing_panel` expanded flag) is
  per-session, NOT persisted. Sliders / colors / text inputs are
  the source of truth.
- The Lua file is **trusted** -- the loader uses `loadfile` then
  calls the chunk. Do not load presets from untrusted sources in v1
  ; sandboxing the preset loader is v2 work.

---

## What `aaa.io.*` does NOT do in v1

- **Sandbox-aware file access** -- v1 is non-sandboxed, so all paths
  are accessible to the running process. App-Sandbox containers are
  out-of-scope for v1.
- **Directory watching for hot reload** -- the file watcher
  (`aaa_file_watcher_mac`) watches the loaded script's mtime only ;
  watching an entire `Samples/` tree for auto-reload-on-any-change is
  v3.1 polish.
- **Auto-save on quit / restore on launch** -- would need a canonical
  "last preset" path ; v3.1 polish.
- **Preset thumbnails** -- the file is a plain Lua table ; no
  thumbnail image bundled. Add a `screenshot = "..."` field
  manually if your workflow needs it -- the loader will silently
  skip unknown fields.
- **Schema migration on load** -- if you rename a widget label
  between sessions, the old preset's entry under the old label is
  silently dropped. Plan label naming ahead.

---

## Composing dialogs + presets : end-to-end recipe

A complete save / load round-trip MEU :

```lua
local intensity = 1.0
local tint = { r = 1, g = 1, b = 1, a = 1 }
local status = "(no preset loaded)"

function aaa.on_frame( w, h, frame )
    aaa.ui.begin_panel( "Bloom", 16, 16, 360, 280 )
    intensity = aaa.ui.slider( "intensity", intensity, 0.0, 2.0 )
    tint.r, tint.g, tint.b, tint.a = aaa.ui.hsv_color_picker(
        "tint", tint.r, tint.g, tint.b, tint.a )

    if aaa.ui.button( "Save preset..." ) then
        local path = aaa.io.save_file_dialog(
            "Save preset", "bloom.lua", { "lua" } )
        if path and aaa.io.save_preset( path ) then
            status = "Saved : " .. path
        end
    end

    if aaa.ui.button( "Load preset..." ) then
        local path = aaa.io.open_file_dialog(
            "Load preset", { "lua" } )
        if path and aaa.io.load_preset( path ) then
            status = "Loaded : " .. path
        end
    end
    aaa.ui.end_panel()

    aaa.use_shader( "aaa_bloom_real" )
    aaa.set_uniform_int( 0, 1 )
    aaa.set_uniform_float( 0, intensity )
    aaa.set_uniform_vec4( 0, tint.r, tint.g, tint.b, tint.a )
    aaa.draw_fullscreen_quad()
    aaa.draw_hud_text( status )
end
```

See also : [Sample MEU : v3_preset_save_load](../samples.md#v3_preset_save_load).
