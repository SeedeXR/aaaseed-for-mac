# Sample MEU : mouse

Cursor-driven shader uniforms. Mac v1 echo of `MEU_PROTO/KinMove/`
from the Windows AAASeed kernel.

## What it does

Demonstrates the `aaa.mouse_xy()` binding by normalizing the cursor
position to `[0, 1]^2` and feeding it into vec4 slot 0 of a Path A
shader. Move the mouse to sweep the parameter space.

## Path A shader used

- `aaa_curl_noise_real.metal` — Bridson 2007 divergence-free curl noise
  (c137-A real revival).

## How to modify

Edit `mouse.lua` :

- `shader_name` — swap to any other `.metal` stem.
- The mouse coordinates are normalized against the per-frame `w` and `h`
  args passed to `aaa.on_frame()` ; if you'd rather drive in raw pixels,
  remove the normalization.
- Bind the normalized values into a different uniform slot, or into
  `set_uniform_float( slot, value )` for shaders that take scalars.

## Why this is not the Windows MEU verbatim

`KinMove` on Windows is a kinematic-motion transform node that integrates
pointer input into a scene-graph child's position over time, with damping
and bounds. The scene-graph + transform-node subsystem is deferred to v2
on Mac (project_v1_ship_gate.md). The Mac v1 sample echoes the
pointer-drives-parameter intent through the Path A fullscreen-quad path.
