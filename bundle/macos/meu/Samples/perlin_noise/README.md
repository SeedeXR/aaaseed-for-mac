# perlin_noise — make a Lua library module actually render

**The trap this sample teaches:** a pure-Lua module (a file that computes
something and ends with `return mytable`) **loads fine and renders
nothing**. A MEU must do two things to put pixels on screen:

1. define `function aaa.on_frame(w, h, frame) ... end`
2. emit at least one draw — typically `aaa.draw_fullscreen_quad()`

If you paste a library file (like a `perlin.lua` noise module) into the
editor and Run it, the editor's **Run** only *syntax-checks*; press
**▶ Play (Cmd+P)** to execute in the engine runtime — and even then, with
no `aaa.on_frame`, the runner now logs:

```
WARNING -- '<file>' loaded but defines no aaa.on_frame(w, h, frame).
Nothing will render.
```

## What this sample does

- Embeds a complete **CPU Perlin module** (Ken Perlin's Improved Noise,
  Lua 5.1-safe, deterministic seed) — the exact kind of file users paste.
- Wraps it in the minimal MEU pattern: every frame the CPU `perlin.octave`
  / `perlin.noise` drive the **offset + gain uniforms**, and the GPU
  shader **`aaa_noise_real`** (c135-A Path A revival — real Perlin /
  Simplex / FBM in MSL) renders the per-pixel field. The HUD prints a
  live CPU noise sample so you can SEE the Lua module executing.

Why not compute every pixel in Lua? 1920×1080×60 fps = 124M noise calls
per second — far beyond a scripting VM. The idiomatic split: **CPU Lua for
control-rate values (uniforms, animation), GPU shader for pixel-rate
work.**

## Run it

- Drag `perlin_noise.lua` onto the running AAASeed window, or
- copy it into your project and press ▶ Play in the Studio.

## Tests

- `tests/native/perlin_meu_script_test.cpp` — pure-Lua unit: determinism,
  lattice zeros, range bound, and that `on_frame` selects the shader +
  draws.
- `tests/native/meu_runner_perlin_test.mm` — integration: the real MEU
  runner loads this file, renders frames into an offscreen Metal target,
  and the no-`aaa.on_frame` warning path is exercised with a module-only
  script.
