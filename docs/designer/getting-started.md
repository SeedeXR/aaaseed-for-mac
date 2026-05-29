# Getting Started with AAASeed for Mac

> Audience : visual artists, VJs, and technical designers who want to
> build immersive Metal-rendered experiences scripted in Lua. No C++
> required.

---

## What is AAASeed ?

AAASeed is a Mac-native port of a Windows VJ engine. Think of it as a
small, fast Metal renderer that you script in Lua. Each "patch" is
called a **MEU** (Module Editable Unit) -- a single `.lua` file that
selects a shader, pushes uniforms every frame, draws a fullscreen quad,
and (optionally) renders an in-app immediate-mode UI panel for live
parameter tweaking.

Coming from TouchDesigner, vvvv, Resolume, or Notch ? Here is the rough
mental map :

| You know                          | In AAASeed Mac v1 this is                                 |
|-----------------------------------|-----------------------------------------------------------|
| TOPs / GLSL Texture nodes         | A `.metal` Path A shader picked by `aaa.use_shader(...)`  |
| Parameter inputs / fields         | Lua locals + `aaa.set_uniform_*` calls                    |
| UI panel / control surface        | `aaa.ui.*` immediate-mode widgets (panel + slider + ...)  |
| The render output                 | `aaa.draw_fullscreen_quad()` once per frame               |
| File I/O / preset save            | `aaa.io.save_preset` + `aaa.io.load_preset`               |

There is NO visual node editor on Mac v1 (none on Windows either --
artists wrote Lua + edited GaBu BU widget trees by hand there too).
You write small Lua scripts ; the runner hot-reloads them on save.

What you get out of the box :

- 169 Path A shaders in `Contents/Resources/shaders/`, including 11
  real-algorithm revivals (FXAA Lottes, Cook-Torrance PBR, Bridson
  curl noise, Lottes bloom, hex-bokeh DoF, and more).
- 33 Lua bindings : 12 core (`aaa.*`) + 14 widgets (`aaa.ui.*`) +
  5 file I/O (`aaa.io.*`) + 2 IME (`aaa.ime.*`).
- 14 sample MEUs covering every binding, with per-sample READMEs.
- Apple Silicon `arm64` build (Intel via Rosetta).

---

## Your first MEU in 5 minutes

You will need : an installed **AAASeed Studio.app** (see
[Installation](installation.md)) and any text editor (VS Code, Sublime,
TextEdit, vim, whatever).

### Step 1 -- locate the entry script

Inside the application bundle the default MEU lives at :

```
"/Applications/AAASeed Studio.app"/Contents/Resources/meu/hello_world.lua
```

This is the file the runner loads at launch. Open it in your editor
(right-click the .app -> Show Package Contents -> drill down to
`Contents/Resources/meu/`).

### Step 2 -- replace it with the smallest possible MEU

Overwrite `hello_world.lua` with :

```lua
-- Minimal AAASeed MEU.

function aaa.on_frame( w, h, frame )
    aaa.use_shader( "aaa_curl_noise_real" )
    aaa.set_uniform_int( 0, 1 )                 -- 1 = real algorithm
    aaa.set_uniform_float( 0, aaa.time() )      -- drive time uniform
    aaa.set_uniform_vec4( 0, 1.0, 0.5, 0.2, 1.0 )
    aaa.draw_fullscreen_quad()
    aaa.draw_hud_text( "frame " .. frame )
end
```

That is the entire program. The runner :

1. Calls `aaa.on_frame(w, h, frame)` once per Metal frame.
2. You select a shader, push uniforms, draw the fullscreen triangle.
3. The HUD renderer picks up `aaa.draw_hud_text` after your call
   returns and overlays it via the bundled monospace atlas.

### Step 3 -- launch the app

Double-click **AAASeed Studio.app**. The Home screen opens ; click
**Open Sample** (or use one of your recent projects). Then press
**Cmd+P** (▶ Play) — the Studio spawns the engine runtime which
opens a fullscreen window filled with the orange-tinted Bridson
curl-noise field, with a `frame N` HUD counter in the top-left
corner.

### Step 4 -- iterate

Edit `hello_world.lua`, save, and the FSEvents-driven file watcher in
the runner picks up the change. Try :

- Change the shader stem to `"aaa_bloom_real"`,
  `"aaa_motion_blur_real"`, or `"aaa_dof_hex_bokeh_real"` -- see the
  [Shader catalog](shader-catalog.md) for all 169 options.
- Change the RGB tuple in `set_uniform_vec4` from `1.0, 0.5, 0.2` to
  `0.2, 0.5, 1.0` for a cool blue.
- Replace `aaa.time()` with `math.sin( aaa.time() ) * 0.5 + 0.5` for
  an oscillating value.

<!-- screenshot: hello-world-curl-noise.png -->

### Step 5 -- add your first widget

Replace the body with :

```lua
local intensity = 0.5

function aaa.on_frame( w, h, frame )
    aaa.ui.begin_panel( "Controls", 16, 16, 320, 120 )
    intensity = aaa.ui.slider( "intensity", intensity, 0.0, 2.0 )
    aaa.ui.end_panel()

    aaa.use_shader( "aaa_bloom_real" )
    aaa.set_uniform_int( 0, 1 )
    aaa.set_uniform_float( 0, intensity )
    aaa.draw_fullscreen_quad()
end
```

Save. You now have a draggable slider driving the bloom gain uniform
every frame. The return value of `aaa.ui.slider` IS the live value --
no marshalling, no two-way binding ceremony. Assign back to your local
and the slider position persists across frames.

You are now authoring AAASeed MEUs.

---

## Mac vs Windows AAASeed

If you have used the Windows build before, here is what is different on
Mac v1 :

| Surface                                  | Windows                                            | Mac v1                                              |
|------------------------------------------|----------------------------------------------------|-----------------------------------------------------|
| MEU on disk                              | Folder with `.lua` + `fx.aaa_layers_all` + presets | Single `.lua` file                                  |
| Parameter UI                             | C++ Win32 NSPanel-style param dialogs              | In-app immediate-mode widgets (`aaa.ui.*`)          |
| Engine layers (`fx.aaa_layers_all`)      | Layer subsystem                                    | Superseded -- Path A shaders only (no layer cells)  |
| Visual node editor                       | None (never existed)                               | None                                                |
| Lua authoring                            | Supported                                          | Supported                                           |
| Sample MEU library                       | `AAAKernel/AAA_PROTO/MEU_PROTO*/`                  | `Contents/Resources/meu/Samples/` (14 starters)     |
| Hot reload                               | Manual                                             | FSEvents file watcher -- save and it reloads        |
| Preset format                            | `preset_*.plua`                                    | Single-table Lua `return { ... }` file              |
| Graphics backend                         | DirectX 11                                         | Metal                                               |

The big simplification : Mac v1 dropped the Windows Layer subsystem and
the GaBu BU widget render path. You author Lua scripts that talk
directly to Metal through the 33 Lua bindings. Less ceremony, fewer
moving parts, all the same shaders.

---

## Where to go next

- [Installation](installation.md) -- DMG verification, Gatekeeper
  rinse, architecture check.
- [MEU authoring](meu-authoring.md) -- the canonical tutorial. Anatomy
  of a MEU, hot-reload, presets, what is not supported in v1.
- [Lua API : core](lua-api/core.md) -- `aaa.use_shader`,
  `aaa.set_uniform_*`, `aaa.draw_*`, time / input / log bindings.
- [Lua API : UI widgets](lua-api/ui.md) -- panel, slider, button,
  color picker, modals, text input, collapsing panels, hot-reload
  button.
- [Lua API : file I/O](lua-api/io.md) -- file dialogs, drag-drop,
  preset save / load + preset format spec.
- [Lua API : IME](lua-api/ime.md) -- CJK marked-text composition path +
  synthetic injection bindings.
- [Sample MEUs](samples.md) -- a guided tour of all 14 starter MEUs.
- [Shader catalog](shader-catalog.md) -- the 169 Path A shaders, with
  the 11 real-algorithm revivals called out plus citations.

Found a gap or a typo in these docs ? Open an issue on the repository
and tag it `docs/designer`.
