# Path A catalog

**Path A** is the term we use for the catalog of `.metal` shaders that
ship with AAASeed for Mac. The catalog has **169 files** at
`src/shaders/msl/`, each compiled at build time into the bundle's
`Resources/shaders/` directory.

A Path A shader is **selectable by name** from a Lua MEU via
`aaa.use_shader("name")` -- e.g. `aaa.use_shader("aaa_bloom_real")`.
The MEU runner caches the compiled `MTLRenderPipelineState` so repeat
selections are zero-cost.

---

## Stub vs revival

Most Path A entries started as **stubs** -- minimal fragment shaders
that emit a recognizable placeholder (gradient, animated noise,
solid color). The stub gives the runner a complete catalog to render
**every** MEU script that names a shader, even before the real
algorithm is ported.

Per [Path A revival pattern](memory-doctrine.md#path-a-revival-pattern),
when we port a stub to its **real algorithm** we :

1. Create a NEW file `<name>_<algo>.metal` (or `<name>_real.metal`).
2. **Preserve** the original stub `.metal` file unchanged.
3. Both coexist in the catalog.
4. The regression baseline for the stub is intact.
5. Add a new regression test for the revival's pixel output.
6. Drop an inline algorithm **citation** at the top of the new file.

This lets the catalog grow from "complete catalog of placeholders" to
"complete catalog of real algorithms" one file at a time, without ever
breaking a script that hardcoded a stub's name.

---

## Shipped revivals (11 as of v4)

| # | Stub                  | Revival                       | Algorithm                                                | Pass count |
| - | --------------------- | ----------------------------- | -------------------------------------------------------- | ---------- |
| 1 | `fxaa.metal`          | `fxaa_lottes.metal`           | FXAA (Timothy Lottes) 3.11 NVIDIA whitepaper             | 1          |
| 2 | (multi)               | (IFS DE library)              | Iterated Function System distance estimators             | 1          |
| 3 | `lights_deferred_v0.metal` | `lights_deferred_real.metal` | G-buffer deferred lighting (gbuffer + lighting)     | 2          |
| 4 | (cam stub)            | `aaa_cam_real.metal`          | Perspective camera matrix + depth-aware ray dir          | 1          |
| 5 | `aaa_material_pbr.metal` | (Cook-Torrance integration) | Cook-Torrance microfacet BRDF (Walter et al. 2007)       | 1          |
| 6 | (noise stub)          | `aaa_noise_real.metal`        | Perlin (1985) + Simplex (Perlin 2001) hybrid             | 1          |
| 7 | (gol stub)            | `aaa_gol_real.metal`          | Conway's Game of Life ping-pong                          | 2          |
| 8 | (curl noise stub)     | `aaa_curl_noise_real.metal`   | Bridson 2007 curl noise for fluid-like advection         | 1          |
| 9 | `bloom.metal`         | `aaa_bloom_real.metal`        | Gaussian-blur threshold pyramid + additive composite     | 3          |
|10 | (motion blur stub)    | `aaa_motion_blur_real.metal`  | Velocity-buffer sampled trail                            | 1          |
|11 | `dof.metal`           | `aaa_dof_hex_bokeh_real.metal`| Hexagonal-bokeh depth-of-field via three skewed passes   | 3          |

The full per-revival citation block sits at the top of each
`*_real.metal` file. Example from `aaa_bloom_real.metal` :

```c
// aaa_bloom_real.metal
//
// Real Gaussian bloom. Pipeline :
//   pass 1 : threshold (brightness > 1.0 -> emit, else discard) into
//            half-res target.
//   pass 2 : separable Gaussian blur, 9-tap kernel.
//   pass 3 : additive composite onto the scene color.
//
// Reference : Akenine-Moller, Haines, Hoffman, "Real-Time Rendering"
//   4th ed, ch.12.4.1 (HDR bloom).
//
// Companion : src/shaders/msl/bloom.metal (the original stub) is
// PRESERVED unchanged per the Path A revival pattern.
```

---

## Mode-flag stub variant

Some stubs have a **mode-flag** variant : a single `.metal` file with
a uniform `uint mode` whose value selects between visual variants. The
stub honors the mode flag with a switch over branches that all hit the
same placeholder pattern, so the revival can later replace each branch
independently with the real algorithm.

Example shape :

```c
// src/shaders/msl/some_effect.metal (stub)
fragment float4 frag( ... ) {
    switch( uniforms.mode ) {
        case 0: return placeholder_mode_0( uv );
        case 1: return placeholder_mode_1( uv );
        case 2: return placeholder_mode_2( uv );
    }
    return float4( 1, 0, 1, 1 );  // hot pink unknown-mode marker
}
```

The revival pattern can then create
`some_effect_mode0_<algo>.metal`, `some_effect_mode1_<algo>.metal`,
etc., one at a time, without disturbing the catalog or the existing
mode-flag dispatch.

---

## Citation discipline

Every revival inlines :

- **Algorithm name + canonical citation** (author, year, paper / book).
- **Pass count** (so it's obvious why a multi-pass file references
  several render targets).
- **Companion line** noting the original stub is preserved.

This lets a future developer reading the shader know exactly which
paper to grep when the visual diverges from the reference image. It
also makes audit trails for content rights painless -- no shipped
shader uses an unattributed algorithm.

---

## Discovery from Lua

The runner exposes catalog discovery via `Runner::list_shaders()`,
which is bound into Lua as `aaa.list_shaders()` :

```lua
-- print every shader the catalog ships
for _, name in ipairs( aaa.list_shaders() ) do
  print( name )
end
```

The list is sorted alphabetically + stable across calls within a
single `Runner` lifetime. Each entry is the bare stem (no extension,
no directory).

---

## Adding a new shader

1. Drop `src/shaders/msl/<your_shader>.metal`.
2. `cmake --build out/...` -- the build system picks up the new file
   automatically via glob (refresh CMake cache if needed).
3. The shader is now selectable as `aaa.use_shader("your_shader")` in
   any MEU.

For a revival (rather than a stub) add `_<algo>` to the new file's
stem AND inline the citation block at top.

---

## Cross-references

- [Architecture](architecture.md)
- [Memory doctrine index](memory-doctrine.md)
- [MEU runner](meu-runner.md)
- [`feedback_path_a_revival_pattern.md`](../../memory/feedback_path_a_revival_pattern.md)
- [Authoring MEUs (legacy guide)](../AUTHORING_MEUS_ON_MAC.md)
