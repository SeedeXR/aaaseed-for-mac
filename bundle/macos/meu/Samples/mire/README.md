# Sample MEU : mire

Test-pattern cycler. Mac v1 echo of `MEU_PROTO/Mire/PROTO_Mire.lua` from the
Windows AAASeed kernel.

## What it does

Cycles through three Path A shaders that serve as display / colorspace
test patterns. SPACE advances. HUD shows the active pattern name.

## Path A shaders used

- `ps_Maa_add_scale.metal` — gradient (c121 baseline).
- `ps_Maa_alpha.metal` — alpha-blend ramp (c121 baseline).
- `aaa_noise_real.metal` — Perlin + Simplex + FBM (c135-A real revival).

## How to modify

Edit `mire.lua` :

- Add or remove pattern names in the `patterns` table.
- Names must match a `.metal` stem in `Resources/shaders/` of the .app
  bundle.
- Change `set_uniform_vec4( 0, ... )` to bias the gain ; `( 1, ... )` to
  shift the offset.

Reload : if the runner exposes `runner.reload()` in your build, no app
restart is needed ; otherwise rebuild `aaaseed_app` to pick up edits.

## Why this is not the Windows MEU verbatim

The Windows `PROTO_Mire.lua` constructs GaBu BU widgets (`add_param_obj_name`,
`add_button`, `add_bu_texture_target_unit`) bound to a `bdd_img_gradient`
engine cell. Those depend on the Layer subsystem + GaBu UI render path,
both deferred to v2 on Mac (project_v1_ship_gate.md). The Mac v1 sample
echoes the intent — pick a test pattern — without that surface.
