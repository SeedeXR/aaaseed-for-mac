# Sample MEU : composer

Multi-shader composition demo. Mac v1 echo of `MEU_PROTO/Out/`
from the Windows AAASeed kernel + the c138-A bloom run-then-readback
pattern.

## What it does

Alternates between two Path A shaders on consecutive frames. The eye
integrates them at refresh rate, so the user sees the contribution of
both passes in one composition. Press SPACE to toggle between
"alternating" (two-shader interleave) and "fixed" (pass A only).

## Path A shaders used

- `aaa_curl_noise_real.metal` — Bridson 2007 curl noise (c137-A).
- `aaa_bloom_real.metal` — Lottes-style bloom (c138-A).

## How to modify

Edit `composer.lua` :

- Replace `pass_a` / `pass_b` with any other shader stems.
- Change the alternation period : replace `( frame % 2 )` with
  `( frame % N )` for slower alternation, or pick by `aaa.time()` for a
  time-based switch.
- Add a third pass and switch on `( frame % 3 )`.

## Why this is not a true compositor

A real compositor would :

1. Bind FBO #1 ; render pass A to it.
2. Bind the window pass ; bind FBO #1's color attachment via
   `aaa.set_bind_texture( 0, fbo1_tex )` ; render pass B sampling FBO #1.

The runner exposes `aaa.set_bind_texture` but the FBO + texture-target
allocation surface is v2 (project_v1_ship_gate.md "Authoring Surface").
The c138-A `aaa_bloom_real.metal` shader uses the run-then-readback
pattern internally, which is why it pairs well with the alternation
demo.

## Why this is not the Windows MEU verbatim

The Windows `Out` MEU drives a true FBO compositor across N input
layers, with engine-allocated render targets and the c142-B-deferred
Layer subsystem. Mac v1 echoes the multi-pass intent through the
single-pass alternation technique.
