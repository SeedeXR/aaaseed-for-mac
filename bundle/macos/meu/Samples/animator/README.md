# Sample MEU : animator

Time-driven animation. Mac v1 echo of `MEU_PROTO/KinMoveAuto/`
from the Windows AAASeed kernel.

## What it does

Renders a single Path A shader continuously, with time (seconds since
runner construction) and a slow sinusoidal pulse feeding into the
shader's uniform slots. No user input ; the visual evolves on its own.

## Path A shader used

- `aaa_curl_noise_real.metal` — Bridson 2007 divergence-free curl noise
  (c137-A real revival).

## How to modify

Edit `animator.lua` :

- `shader_name` — swap to any other `.metal` stem in `Resources/shaders/`.
- `phase = t - math.floor( t )` — replace with `t * speed` for an
  unbounded sweep, or with `math.sin( t * freq )` for an oscillation.
- `pulse` formula — change to drive a different uniform slot or to
  shape the per-channel gain differently.

## Why this is not the Windows MEU verbatim

`KinMoveAuto` on Windows is a transform-graph cell that injects time
into a kinematic-motion node connected to scene-graph children. The
scene-graph + Layer subsystem is deferred to v2 on Mac
(project_v1_ship_gate.md). The Mac v1 sample echoes the time-driven
intent through the Path A fullscreen-quad path.
