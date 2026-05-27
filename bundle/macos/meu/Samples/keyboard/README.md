# Sample MEU : keyboard

Interactive shader selector. Mac v1 echo of `MEU_PROTO/GridSel/`
from the Windows AAASeed kernel.

## What it does

Demonstrates the `aaa.key_down()` binding by mapping seven number keys
and the left / right arrow keys onto a curated list of Path A shaders.
HUD shows the active slot index and shader name.

## Path A shaders used

1. `ps_Maa_add_scale.metal` — gradient (c121 baseline).
2. `aaa_noise_real.metal` — Perlin / Simplex / FBM (c135-A).
3. `aaa_curl_noise_real.metal` — Bridson 2007 (c137-A).
4. `aaa_gol_real.metal` — Conway + Brian's Brain (c136-A).
5. `aaa_bloom_real.metal` — Lottes bloom (c138-A).
6. `aaa_motion_blur_real.metal` — camera-velocity (c139-A).
7. `aaa_dof_hex_bokeh_real.metal` — McIntosh hex bokeh (c140-A).

## How to modify

Edit `keyboard.lua` :

- Replace any entry in the `shaders` table with another `.metal` stem
  from `Resources/shaders/`.
- The `digit_codes` table maps to Apple hardware keycodes (Carbon style)
  for digits 1-7. Extend with `21 -> 4`, `23 -> 5`, etc. patterns ; full
  table at `src/meu/aaa_meu_runner_mac.mm` `l_key_down`.
- `aaa.key_down( "left" )` accepts the named keys `space`, `return`,
  `escape`, `left`, `right`, `up`, `down`. Pass an integer for any other
  Apple keycode.

## Why this is not the Windows MEU verbatim

GridSel on Windows is a GaBu BU widget — a clickable N-cell on-screen
picker bound to a parameter cell. That widget surface is deferred to v2
(project_v1_ship_gate.md). The Mac v1 sample uses keyboard input to
the same selection-from-N intent.
