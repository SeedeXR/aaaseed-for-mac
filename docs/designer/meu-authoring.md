# Authoring MEUs

> Task-oriented walkthrough of the MEU authoring workflow. Once you
> finish this tutorial you can write a MEU from scratch that selects a
> shader, drives uniforms from a live UI panel, hot-reloads on save,
> and persists state to disk.
>
> For the strict reference of every binding see [Lua API : core](lua-api/core.md),
> [Lua API : UI](lua-api/ui.md), [Lua API : IO](lua-api/io.md), and
> [Lua API : IME](lua-api/ime.md). For the original developer-facing
> document see `docs/AUTHORING_MEUS_ON_MAC.md` in the source tree.

---

## What is a MEU on Mac v1 ?

A MEU (Module Editable Unit) on Mac v1 is **just a single Lua script**.
The runner (`aaa::meu::Runner` in `src/meu/aaa_meu_runner_mac.{h,mm}`)
loads it at app launch and calls its `aaa.on_frame(w, h, frame)`
function once per Metal frame.

Locations :

- The default entry script ships at
  `Contents/Resources/meu/hello_world.lua` inside the .app bundle.
- 14 ready-to-crib samples ship at
  `Contents/Resources/meu/Samples/<name>/<name>.lua`. See
  [Sample MEUs](samples.md) for the guided tour.

There is no folder ceremony : no `fx.aaa_layers_all`, no
`preset_*.plua` cascade, no GaBu BU widget tree. Just one Lua file.

---

## Anatomy of a MEU script

Every MEU follows the same skeleton :

```lua
-- 1. Per-MEU Lua locals -- your "fields" / persisted state.
local intensity = 0.5
local tint = { r = 1.0, g = 0.5, b = 0.2, a = 1.0 }

-- 2. (optional) helper functions you call from on_frame.
local function clamp01( x ) return math.max( 0.0, math.min( 1.0, x ) ) end

-- 3. The mandatory entry point.
function aaa.on_frame( w, h, frame )
    -- 3a. Build the UI for this frame.
    aaa.ui.begin_panel( "Controls", 16, 16, 320, 160 )
    intensity = aaa.ui.slider( "intensity", intensity, 0.0, 2.0 )
    tint.r, tint.g, tint.b, tint.a = aaa.ui.hsv_color_picker(
        "tint", tint.r, tint.g, tint.b, tint.a )
    aaa.ui.end_panel()

    -- 3b. Render the scene.
    aaa.use_shader( "aaa_bloom_real" )
    aaa.set_uniform_int( 0, 1 )                       -- real algo
    aaa.set_uniform_float( 0, intensity )
    aaa.set_uniform_vec4( 0, tint.r, tint.g, tint.b, tint.a )
    aaa.draw_fullscreen_quad()

    -- 3c. (optional) HUD overlay.
    aaa.draw_hud_text(
        string.format( "i=%.2f frame=%d", intensity, frame ) )
end
```

Three sections to internalise :

- **Locals** persist across frames. The Lua state lives as long as the
  script is loaded ; locals declared at chunk-scope keep their values
  between `on_frame` calls.
- **`on_frame(w, h, frame)`** is mandatory. `w` and `h` are the window
  dimensions in pixels ; `frame` is the integer frame index (resets to
  0 on `Runner::reload()`).
- The **return-and-reassign idiom** is universal for `aaa.ui.*`
  widgets : `intensity = aaa.ui.slider("intensity", intensity, ...)`.
  The widget owns the live value, returns it each frame ; you assign
  back so the persisted Lua local catches up.

---

## Step-by-step : your first slider-driven shader

We will build a MEU that drives the `aaa_bloom_real` shader from a
single slider.

### 1. Create or open the script

Open `Contents/Resources/meu/hello_world.lua` in your editor. (Right-
click the .app -> Show Package Contents -> drill down to
`Contents/Resources/meu/`.)

### 2. Set up a persisted local

```lua
local intensity = 1.0
```

This is your slider's backing value. It lives at chunk scope so it
survives between `on_frame` calls.

### 3. Open a UI panel

```lua
function aaa.on_frame( w, h, frame )
    aaa.ui.begin_panel( "Bloom controls", 16, 16, 320, 100 )
```

`begin_panel(title, x, y, w, h)` opens a window-pixel-coordinate panel
at the top-left of the window. Origin is top-left, y grows downward
(matching `aaa.mouse_xy()`).

### 4. Add the slider

```lua
    intensity = aaa.ui.slider( "intensity", intensity, 0.0, 2.0 )
```

`slider(label, value, min, max) -> new_value`. The return value
replaces the local. On idle frames it equals the passed-in value ; on
drag frames it is the clamped new value.

### 5. Close the panel

```lua
    aaa.ui.end_panel()
```

Every `begin_panel` MUST pair with exactly one `end_panel`.

### 6. Drive the shader uniforms + draw

```lua
    aaa.use_shader( "aaa_bloom_real" )
    aaa.set_uniform_int( 0, 1 )              -- slot 0 = 1 -> real algo
    aaa.set_uniform_float( 0, intensity )    -- gain
    aaa.draw_fullscreen_quad()
end
```

`aaa.set_uniform_int(0, 1)` is the c128-c140 mode-flag pattern. Path A
revival shaders branch on slot 0 : `1` selects the real algorithm,
`0` selects the legacy stub. Always pass `1` from a MEU unless you are
specifically testing the stub path.

### 7. Save the file

The runner's FSEvents-driven file watcher picks up the change. The
script reloads in-place ; Lua locals reset to their declared initial
values (here, `intensity = 1.0` again). Drag the slider to confirm
the bloom intensity follows.

---

## Hot reload

Two ways to reload :

- **Automatic** : edit + save the .lua file. The file watcher
  (`src/meu/aaa_file_watcher_mac.{h,mm}`) sees the FSEvents mtime
  change and calls `Runner::reload()` on the main thread.
- **Manual** : add an `aaa.ui.hot_reload_button("Reload MEU")` widget
  to your panel. Clicking it triggers the same reload path.

On reload :

1. The current `lua_State` is closed.
2. The same script file is re-read from disk.
3. A fresh `lua_State` opens.
4. All 33 `aaa.*` / `aaa.ui.*` / `aaa.io.*` / `aaa.ime.*` bindings
   re-register.
5. The chunk re-runs.
6. Lua locals are back at their declared initial values.

To persist values across reloads, save them to a preset (next section).

---

## Save and load presets

Two file-dialog bindings + two preset bindings live under `aaa.io.*`.
The pattern :

```lua
local intensity = 1.0
local tint = { r = 1, g = 1, b = 1, a = 1 }
local status = ""

function aaa.on_frame( w, h, frame )
    aaa.ui.begin_panel( "Controls", 16, 16, 360, 200 )
    intensity = aaa.ui.slider( "intensity", intensity, 0.0, 2.0 )
    tint.r, tint.g, tint.b, tint.a = aaa.ui.hsv_color_picker(
        "tint", tint.r, tint.g, tint.b, tint.a )

    if aaa.ui.button( "Save preset..." ) then
        local path = aaa.io.save_file_dialog(
            "Save preset", "my_preset.lua", { "lua" } )
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

The on-disk preset is a single Lua table you can hand-edit + check
into Git :

```lua
-- AAASeed preset (generated by aaa.io.save_preset)
return {
    slider_state = { intensity = 1.0 },
    color_well_state = {
        tint = { 1.0, 0.5, 0.2, 1.0 },
    },
    text_input_state = {},
}
```

See [Lua API : IO](lua-api/io.md) for the full preset format spec.

---

## Cycle through shaders with the keyboard

The `aaa.key_down(key)` binding takes a named key string or an integer
Carbon keycode :

```lua
local shaders = {
    "aaa_noise_real",
    "aaa_curl_noise_real",
    "aaa_gol_real",
    "aaa_bloom_real",
    "aaa_motion_blur_real",
    "aaa_dof_hex_bokeh_real",
    "fxaa_lottes",
}
local idx = 1
local last_advance_frame = -10

function aaa.on_frame( w, h, frame )
    -- Edge-detect Space : advance once per press, not once per frame.
    if aaa.key_down( "space" ) and frame - last_advance_frame > 5 then
        idx = ( idx % #shaders ) + 1
        last_advance_frame = frame
    end

    aaa.use_shader( shaders[ idx ] )
    aaa.set_uniform_int( 0, 1 )
    aaa.set_uniform_float( 0, aaa.time() )
    aaa.draw_fullscreen_quad()
    aaa.draw_hud_text( shaders[ idx ] )
end
```

Named keys recognized by `aaa.key_down` : `"space"`, `"return"`,
`"escape"`, `"left"`, `"right"`, `"down"`, `"up"`. Other keys take an
integer Carbon keycode (`1` = 18, `2` = 19, etc.).

---

## What is NOT supported in v1

These surfaces are explicitly deferred ; do not author MEUs that
depend on them :

- **Windows-style C++ Win32 param dialogs** (`Src/ui/seed_ui.cpp` on
  Windows). Replaced by the `aaa.ui.*` immediate-mode widget surface.
- **GaBu BU widget tree** (`AAAKernel/GaBu/` on Windows). No render
  path on Mac.
- **`fx.aaa_layers_all` engine layer serialization.** The Layer
  subsystem was superseded ; Mac v1 ships Path A shaders only, no
  layer cells, no `bdd_*` engine objects accessible from Lua.
- **Multi-file MEU folders with preset cascades.** The runner takes a
  single `.lua` file. Use `aaa.io.save_preset` + `aaa.io.load_preset`
  for parameter persistence instead.
- **In-app visual node editor.** Never existed on Windows either ;
  not in scope for Mac v1.

Honest gaps in the v1 ship surface :

- **Real CJK keyboard input** is end-to-end plumbed through
  `NSTextInputClient`, but final-mile verification with a real
  Pinyin / Hiragana / Hangul keyboard requires a human at a Mac with
  the system input source enabled -- autonomous tests use the
  `aaa.ime.set_marked_text` synthetic-injection path.
- **App-Sandbox containers** : v1 is non-sandboxed, so all paths are
  accessible to the running process. App-Sandbox is out-of-scope.
- **Auto-save on quit / restore on launch** : you save and load
  presets manually.
- **Find / replace + undo / redo** inside `aaa.ui.text_area` : a single
  linear edit history, no Cmd-Z stack.

---

## Authoring loop summary

1. Edit `Contents/Resources/meu/hello_world.lua` (or any
   `Samples/<name>/<name>.lua` if you point the runner at it).
2. Save -- the file watcher reloads automatically.
3. Confirm visually in the running app window.
4. When the parameter set is what you want, click "Save preset..." to
   persist values to a `.lua` table file.
5. Reload that preset from any session via "Load preset...".

That is the entire workflow. No build step, no rebuild of the .app,
no engine restart.

---

## Next steps

- [Running scripts](running-scripts.md) -- Cmd+R runs the editor buffer
  in the engine, Play runs the project, drag-drop, hot reload, and the
  load-but-render-nothing trap.
- [Lua API : core](lua-api/core.md) -- every `aaa.*` binding with
  examples.
- [Lua API : UI](lua-api/ui.md) -- every `aaa.ui.*` widget.
- [Sample MEUs](samples.md) -- 16 starter scripts to crib from
  (`perlin_noise` shows how to wrap a library module so it renders).
- [Shader catalog](shader-catalog.md) -- pick your visual building
  block.
