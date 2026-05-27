# Shader Catalog (Path A)

> Reference for the 169 Path A shaders shipped at
> `Contents/Resources/shaders/<stem>.metal` inside the .app bundle.
> Source mirror : `src/shaders/msl/`. Bundled into the .app by
> `src/ui/macos/CMakeLists.txt`.
>
> Pick a shader stem and pass it to `aaa.use_shader("<stem>")` from
> any MEU. See [Lua API : core](lua-api/core.md#aaause_shader-name---nil)
> for the binding signature.

---

## Naming convention

Shaders fall into three rough buckets, distinguishable from the stem :

| Prefix             | Meaning                                                   | Example                  |
|--------------------|-----------------------------------------------------------|--------------------------|
| `aaa_<name>_real`  | High-quality real-algorithm revival (the c128-c140 cohort)| `aaa_curl_noise_real`    |
| `aaa_<name>`       | Engine-side AAASeed effect (camera, material, etc.)       | `aaa_cam_real`           |
| `ps_<name>`        | Pixel-shader-style legacy port from the Windows kernel    | `ps_Maa_edge_sobel`      |
| `ps_Maa_*`         | Maa (vendor) effects ported from the Windows pixel shelf  | `ps_Maa_alpha`           |
| (other)            | Decorative / experimental / baseline test patterns        | `caustics_1`, `mirror`   |

Internal MSL convention :

- All shaders use a canonical `set_uniform_float` table (slot 0..N) and
  a canonical `set_uniform_vec4` table for parameter pushdown -- this
  is what makes `aaa.set_uniform_float(slot, value)` and
  `aaa.set_uniform_vec4(slot, ...)` work uniformly.
- All Path A revival shaders reserve **integer slot 0** as the
  algorithm-mode flag : `1` = real algorithm, `0` = legacy stub. MEUs
  always pass `aaa.set_uniform_int(0, 1)`.

---

## The 11 real-algorithm revivals

These are the high-quality shaders revived from stubs across sessions
c128 through c140. Each cites the original algorithm + paper.

| Shader stem                | Algorithm                                                       | Citation / source                                | Session |
|----------------------------|-----------------------------------------------------------------|--------------------------------------------------|---------|
| `fxaa_lottes`              | Lottes FXAA 3.11 (post-process antialiasing)                    | Timothy Lottes, NVIDIA whitepaper FXAA 3.11      | c128-A  |
| `ifs_de_library`           | Real IFS distance-estimator library                             | Hart 1996 sphere-tracing + IFS DE foundations    | c130-A  |
| `aaa_cam_real`             | Real camera projection (perspective matrix)                     | Standard pinhole-camera projection               | c134-B  |
| `aaa_material_pbr`         | Cook-Torrance PBR (microfacet BRDF)                             | Cook & Torrance 1982 + Disney 2012 modifications | c134-B  |
| `aaa_noise_real`           | Perlin + Simplex + Fractional Brownian Motion (FBM)             | Perlin 1985 / Perlin 2001 / Mandelbrot 1968 FBM  | c135-A  |
| `aaa_gol_real`             | Conway's Game of Life + Brian's Brain cellular automaton        | Gardner 1970 / Silverman 1989                    | c136-A  |
| `aaa_curl_noise_real`      | Bridson 2007 divergence-free curl noise                         | Bridson SIGGRAPH 2007                            | c137-A  |
| `aaa_bloom_real`           | Lottes-style bloom (run-then-readback gaussian pyramid)         | Timothy Lottes, NVIDIA bloom 2009                | c138-A  |
| `aaa_motion_blur_real`     | Camera-velocity projected motion blur                           | McGuire et al. 2012                              | c139-A  |
| `aaa_dof_hex_bokeh_real`   | McIntosh hex-bokeh depth-of-field                               | McIntosh et al. 2012                             | c140-A  |

Visual-proof signal :

Each revival has a regression baseline in the Phase 3 catalog
(`tests/regression/baselines/<stem>.png`) -- a reference render
generated from a deterministic seed + frame timestamp. The unit-test
target diffs the live render against the baseline and asserts the
maximum-LSB-difference + percent-pixel-differ stay below the per-shader
tolerance (typically `<= 2 LSB max` and `<= 5% pixels differ`).

If you author a MEU that uses one of these eleven shaders, you get
production-quality output. The remaining 158+ shaders in the catalog
are either baseline test patterns, decorative effects ported verbatim
from the Windows pixel shelf, or experimental scratch -- all usable,
but not algorithmically verified at the revival cohort's bar.

---

## Phase 3 baseline catalog

The Phase 3 (c121) baseline included 45 shaders selected as the
minimum-viable visual catalog -- these have the longest test-fixture
history and the most regression coverage. Notable members :

| Stem                  | Use case                                            |
|-----------------------|-----------------------------------------------------|
| `ps_Maa_add_scale`    | Linear gradient -- the canonical test pattern       |
| `ps_Maa_alpha`        | Alpha-blend ramp                                    |
| `ps_Maa_bug_trame`    | Interlace-bug emulation (CRT-style trame)           |
| `ps_Maa_edge_sobel`   | Sobel edge detection                                |
| `ps_Maa_edge_laplace` | Laplace edge detection                              |
| `ps_Maa_fbm`          | Fractional Brownian Motion (cheap variant)          |
| `ps_Maa_gaussH`       | Horizontal gaussian blur                            |
| `ps_Maa_gaussV`       | Vertical gaussian blur (pair with H for 2D)         |
| `ps_Maa_noise`        | Cheap value noise                                   |
| `ps_Maa_noise_cheap`  | Even cheaper noise variant                          |
| `ps_Maa_threshold`    | Hard threshold (clip / binarize)                    |
| `ps_Maa_key_chroma_01`| Chroma keying                                       |
| `ps_Maa_luma_and_scale`| Luma-channel extract + scale                       |
| `ps_Maa_drop`         | Single-drop ripple                                  |
| `ps_Maa_edge`         | Generic edge                                        |
| `ps_Maa_4tex`         | Four-texture mix                                    |

These are best for : test-pattern cycling, baseline effects on top of
a real-algorithm pass, or as scratchpad while learning. They are
**stable** ; revival to "real" quality is not planned for v1.

---

## Decorative / experimental cohort

The remaining shaders are organized loosely :

- **Color grade** : `ps_to_grey`, `ps_to_grey_cc`, `ps_sepia`,
  `ps_negative`, `ps_black_white`, `posterize`, `negative`, `tonemap`,
  `exposure_kelvins`, `gbuffer*`, `brightness`, `hard_bw`, `hard_red`,
  `hard_red2`, `hard_green`, `soft_bw`, `ps_gamma`, `ps_colorGradient`.
- **Blur + bloom** : `blur`, `blur_add_box`, `blur_add_disc`,
  `dof`, `depth_of_field_disc`, `depth_coc`, `bloom`, `darkglow`,
  `godrays`.
- **Distortion / displacement** : `mirror`, `mosaic`, `ink`, `inkcolor`,
  `slider`, `swap_greenblue`, `templateJean`, `ps_slide`.
- **Generative** : `caustics_1`, `fire`, `blood`, `drip`, `fish_tank_base`,
  `fog`, `fog_monaco`, `force_field`, `thinks`, `pool1`,
  `reaction_diffusion`, `aaatree_a`, `star`, `Spiral`, `Toon`,
  `Buildings`, `BBlossom`, `HypnoLord`, `Voroday`, `DPWave`,
  `Maa_WAVY`, `perlinpinpin`, `dotgrid`, `dotgridcolor`.
- **Composition + meta** : `MEU_Out`, `MEU_Pip`, `GaBu_Monitor`,
  `light_sprites`, `lights_deferred_v0`, `lights_deferred_v1`, `obj3d`,
  `gbuffer`, `gbuffer_generic`, `gbuffer_grid`, `gbuffer_matte`,
  `vol3d_Plane`, `vol3d_Voxel`, `stroke_Maa`, `draw_sprite`.
- **Text + sprites** : `ps_fontsdf`, `ps_foot`, `transparent_blit`.
- **Image / IFS variants** : `ifs`, `AndrejMetaImage`, `PeteMetaImage`,
  `ps_earth`, `ps_neand`, `ps_lattice`, `ps_perlinpinpin`,
  `ps_plancton_1`, `ps_plancton_Monaco`, `ps_point_Maa`,
  `ps_qwartz_compo`, `ps_gp_galaxy`, `ps_gp_grid`, `ps_gp_textflow`,
  `ps_halftone`, `ps_halftone_dot`, `ps_lumakey`,
  `ps_matrix42_block`, `ps_mrt_test`, `ps_scanly`, `ps_scanly_2`,
  `ps_toon2`, `ps_xray`, `ps_chromakey_norm`, `ps_colorizedsobel`,
  `ps_debug`, `ps_edgeoverlay`, `ps_fixed_color`, `ps_fixed_main`,
  `ps_angle_mult`, `ps_Eau`, `ps_Maarko_v0_1`, `ps_Maarko_v0_2`,
  `ps_Toy_CloudIq`, `ps_Toy_fog`, `ps_Maa_suns`, `ps_Maa_diff`,
  `ps_Maa_by_n`, `ps_Maa_move_pixel`, `ps_Maa_normal`,
  `ps_Maa_rgb_2_yuv`, `ps_Maa_edge_11114`, `ps_Maa_edge_before`,
  `ps_Maa_key_color_01`, `ps_Maa_Blur_01`, `ps_Maa_Blur_02`,
  `fs_Maa_exp`, `multipler`, `emboss`, `sobel`, `texture_pure`,
  `color_pure`, `post`, `fxaa`, `test`.

The bundle ships ALL of these regardless of how often they are used.
At ~169 `.metal` source files totalling well under 1 MB compiled they
do not meaningfully impact DMG size.

---

## How to discover shader names from the .app

To list every shader stem available in your installed bundle :

```
ls /Applications/AAASeed.app/Contents/Resources/shaders/ | sed 's/\.metal$//'
```

Pick any stem from that list, pass to `aaa.use_shader(...)`, render.

---

## How to add a new shader

Authoring new `.metal` shaders is a **developer-side task** (it
requires recompiling the .app + updating the CMake bundle copy +
adding a regression baseline). See the developer documentation
(`docs/developer/...`) for the .metal authoring side of the loop.

Designer-side workflow for "I need a shader I do not have" :

1. Check the catalog above + the bundle directory listing -- there
   are 169 shipped shaders ; the one you want may exist under a
   different name.
2. Combine multiple shipped shaders by alternating frames (see
   [Sample MEU : composer](samples.md#composer)) or by switching mid-
   sequence based on `aaa.time()` / `aaa.key_down`.
3. If you still need a new shader, file a feature request with the
   target visual effect, citation if known, and an example use case
   -- the developer side will assess revival cost vs. existing
   coverage.

---

## See also

- [Lua API : core](lua-api/core.md#aaause_shader-name---nil) for the
  binding signature.
- [Sample MEU : keyboard](samples.md#keyboard) for a 7-shader selector
  driven by digit keys.
- [Sample MEU : mire](samples.md#mire) for the test-pattern cycler.
- [MEU authoring](meu-authoring.md#step-by-step--your-first-slider-driven-shader)
  for the canonical "drive a shader from a slider" pattern.
