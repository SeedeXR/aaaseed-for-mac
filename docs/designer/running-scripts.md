# Running scripts

How a `.lua` file goes from text to pixels in AAASeed for Mac — and why a
script can "load fine" yet render nothing.

---

## The one rule that trips everyone

A script only puts pixels on screen if it does **both** of these:

```lua
function aaa.on_frame(w, h, frame)   -- 1. the per-frame entry point
    aaa.use_shader("ps_Maa_add_scale")
    aaa.draw_fullscreen_quad()       -- 2. at least one draw call
end
```

A **library module** — a file that computes something and ends with
`return mytable` (a Perlin noise module, a math helper, …) — is valid Lua
and loads without error, but it never defines `aaa.on_frame` and never
draws, so **nothing renders**. This is the single most common
"my script didn't work."

When it happens, AAASeed now tells you:

- the **engine runtime** logs
  `WARNING -- '<file>' loaded but defines no aaa.on_frame(w, h, frame).
  Nothing will render.` (visible in Terminal / Console.app), and
- the **Studio editor** prints a hint to the Console panel after a
  syntax check: *"this script defines no aaa.on_frame — it will load but
  render nothing."*

To turn a module into a visualization, keep the module and add the MEU
block underneath — `Samples/perlin_noise/perlin_noise.lua` is the
canonical worked example (a full CPU Perlin module driving the GPU
`aaa_noise_real` shader).

---

## The four ways to run a script

### 1. Run Script from the editor (Cmd+R) — runs the buffer

The Code Editor's **Run → Run Script (Cmd+R)** executes what's in the
editor, in three steps:

1. **Syntax check** — parse errors stop here with a line number; a
   missing `aaa.on_frame` prints the render-nothing hint.
2. The buffer is written to a temp `.lua` and routed by the **Display
   menu**:
   - **Intuitive (default)** — the script runs **inside the Engine
     Preview panel**. If the preview isn't started yet, the script is
     queued and a Console line tells you to press **Start** in the
     panel; it loads the moment the preview attaches.
   - **Native** — the engine runtime is **spawned with `--script`** as
     a standalone macOS window.
3. **Subsequent Cmd+R presses hot-reload in place** (both routes watch
   the temp file) — edit, Cmd+R, see the change; no second window.

**Run Graph (Cmd+Shift+R)** does the same for the **node graph**: it
generates a MEU that time-slices through every shader-bearing node
(3 s each, numeric-keyed node uniforms applied as float slots) and runs
it through the identical route. Nodes without a shader are skipped; an
empty graph logs a Console warning.

If the Console says `aaaseed_runtime not found`, you're in a dev tree
without the runtime built (`cmake --build … --target aaaseed_runtime`)
or a broken install.

**Getting a file INTO the editor:** drag any `.lua` from Finder and
drop it on the Code Editor panel — it loads into the buffer (the panel
shows a teal ring while you hover) and the Console confirms. Then Cmd+R
runs it. Non-`.lua` drops are rejected with a Console warning.

### 2. ▶ Play Project (Cmd+P) — runs the saved project

**Run → ▶ Play Project** launches the runtime pointed at your saved
`.aaaproj.lua`. Use this for project playback; use Cmd+R for script
iteration.

The **Display** menu chooses where engine output appears: the in-Studio
*Intuitive Preview* or the *Native macOS Window* (which also carries
multi-display span).

### 3. Drag-and-drop onto the runtime window

With the runtime running, drag any `.lua` file from Finder onto its
window. The runner loads it immediately (only `.lua` paths are
accepted). This is the fastest way to try a sample:

```
AAASeed Studio.app/Contents/Resources/runtime/aaaseed_runtime.app/
    Contents/Resources/meu/Samples/perlin_noise/perlin_noise.lua
```

### 4. Hot reload (edit the loaded file)

Once a script is loaded, the runner watches it on disk (FSEvents).
Save the file in any editor and the runner re-executes it in place —
no restart. Disable with the `AAA_DISABLE_FILE_WATCH` env var.

---

## The smallest runnable script

Paste this into a `.lua` file and drag it onto the runtime — animated
noise in 8 lines:

```lua
function aaa.on_frame(w, h, frame)
    aaa.use_shader("aaa_noise_real")        -- any name from the catalog
    aaa.set_uniform_int(0, 1)               -- mode flag = real
    local t = aaa.time()
    aaa.set_uniform_float(0, t - math.floor(t))
    aaa.set_uniform_vec4(0, 1, 1, 1, 1)     -- gain
    aaa.set_uniform_vec4(1, 0, 0, 0, 0)     -- offset
    aaa.draw_fullscreen_quad()
end
```

CPU Lua is for **control-rate** work (uniforms, animation, input);
the shader catalog is the **pixel-rate** half. Don't compute per-pixel
values in Lua — 1920×1080×60 fps is ~124M calls/sec, far beyond the VM.

---

## Troubleshooting

| Symptom | Cause | Fix |
|---|---|---|
| Cmd+R says "Syntax OK" but no window opens | Runtime not found (dev tree) or spawn failed | Check the Console for `aaaseed_runtime not found` ; build the runtime target or reinstall the .app |
| Cmd+R opens a window but later edits don't show | You edited a *different* file than the buffer you ran | Press **Cmd+R** again -- it rewrites the temp script and the running window hot-reloads |
| Script loads, screen stays on the default shader / red clear | No `aaa.on_frame` defined (library module) | Add the MEU block — see `Samples/perlin_noise/` ; check the warning in the Console / system log |
| `on_frame` runs but screen is black | No draw call, or shader name typo | Call `aaa.draw_fullscreen_quad()` ; verify the name against `aaa.shaders()` / the [shader catalog](shader-catalog.md) |
| `attempt to call a nil value` on load | Calling an `aaa.*` binding that doesn't exist in v1 | See "What is NOT supported in v1" in [MEU Authoring](meu-authoring.md) |
| Edits to the file don't take effect | File watch disabled or editing a different copy | Check `AAA_DISABLE_FILE_WATCH` ; confirm you're editing the loaded path (shown in the load log) |
| Works in editor lint, errors in runtime | Lint can't see runtime-only globals | Lint ignores `aaa.*` ; trust the runtime error message, it includes the line number |

---

## Where to go next

- [MEU Authoring](meu-authoring.md) — full script anatomy, widgets, presets, hot reload.
- [Sample MEUs](samples.md) — 15 worked examples; `perlin_noise` is the
  "make my module render" demo.
- [Lua API (aaa.*)](lua-api/core.md) — every binding the runner installs.
- [Shader Catalog](shader-catalog.md) — the names `aaa.use_shader()` accepts.
