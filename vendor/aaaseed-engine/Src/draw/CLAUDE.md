# Draw — High-Level Scene Objects

All drawing objects inherit from `c_obj_ui` through the factory system and are built on GOL.

## Static cur/ui/def Pattern

Most rendering/scene-state classes in this folder maintain three static instance pointers:
- `cur` — currently active (used during rendering)
- `ui` — user-selected (may differ from active during preview)
- `def` — default/fallback (always installed at startup)

Accessors: `get_cur()` / `set_cur()`, `get_ui()` / `set_ui()`, `get_def()`.

When `def` is installed, `get_cur()` never returns null — do not add null checks after `get_cur()` calls on those classes.

**Scope of the pattern (verified 2026-05-08):**
- **Have `def`:** `c_color`, `c_lights`, `c_map`, `c_materials` (mat.h), `c_model`, `c_multiple`, `c_render`, `c_ship`, `c_stencil`, `c_texturing`, `c_tex_anim` (here in draw/), plus `c_tex_video` (media/), `c_bdd_mocap` (obj_ui/bdd/), `c_multi_screen` (obj_ui/), `c_shading` (shaders/). Some expose `get_def()`/`set_def()` accessors, others touch the `def` field directly , same accessor-vs-field inconsistency as `cur`.
- **No `def` (only `cur` / `ui` pair):** `c_layer`, `c_layers`, `c_module`, `c_modules` in `Src/infrastructure/layer/` , `get_cur()` on these CAN return null. `c_modules` adds a `main` static for the singleton-like top-level instance, not a fallback.

For the layer-subsystem variant, see `Src/infrastructure/layer/CLAUDE.md`.

## seeddraw (`seeddraw.h`)

Static-method-only controller for the render loop.
- Frame counter: `frame_index`, `inc_frame()`, `get_frame()`
- Swap buffer: `b_swap_buffer_ui`, `set_swap_buffer()`
- Clean render: `b_clean_render`, `b_clean_focus`, `flip_clean_render()`
- Timing: `get_timing_fps_average()`, `get_timing_fps_swap_average()`
- Render stages: `render_before()`, `render_central()`, `render_after()`
- `c_draw_ui_guard` — RAII guard for shading context

## Cameras (`seedcam.h`, `camera_lua.h`)

- `c_master_camera` — single master camera (factory object)
- `c_seedcam` — per-camera object; static `cur`, `ui`, `master` instances
- **14 commands** (enum `COMMAND`): ROT_*, ROLL_*, TRA_*, RESET
- `st_ubo_cam` — GPU UBO struct: view, projection, inverses, position, near/far, CoC factors
- Perspective or ortho: `_b_perspective_ui`, `_focal_ui`, `_ortho_size_ui`
- Sub-viewport support: `_b_sub_viewport_ui` with both pixel and normalised variants
- Euler rotation order: `gs_euler_order_ui` (6 orders: XYZ, YZX, ZXY, XZY, YXZ, ZYX)
- `_b_focal_link_to_target_ui` — links focal distance to target distance
- Serialization: `camera_format.h/cpp`

## Lights (`light.h`, `lights.h`, `lights_lua.h`)

- `c_light` — single light, type `LIGHT_TYPE` enum: `SUN`, `BULB`, `SPOT`
  - Legacy fixed-function arrays AND modern `st_light_modern` UBO struct coexist
  - `st_point_light`, `st_directional_light`, `st_spot_light` — GPU-side structs
- `c_lights` — container; static `cur`, `ui`, `def`
- Fixed-function limit: **8 lights max** (`LIGHT_NB = 8`)

## Materials (`mat.h`, `mat_lua.h`)

- `c_material` — one material; 5-element color arrays for ambient, diffuse, specular, emission
  `[R,G,B,F,A]` where index 4 is intensity scale
- `st_material` — GPU UBO struct: emissive, roughness, diffuse, AO, specular, reflection, ambient
- `c_materials` — container; static `cur`, `ui`, `def`; max **128 materials** (`MATERIAL_MAX_NB`)
- Separate front/back material indices: `_index_front`, `_index_back`
- `get_front()` / `get_back()` / `get_material_front()` / `get_material_back()`
- Backing store: `c_obj_ui_array_pt<c_material>` + `c_bind*`

## Textures (`texture.h`)

- `c_texture_unit` — one texture unit; bindings: `_bind_1d_ui`, `_bind_2d_ui`, `_bind_3d_ui`, `_bank_2d_ui`
  Active index: `_bind_2d_out` (vs `_ui` = user selection)
- `c_texturing` — container of **3 texture units** (`TEXTURING_UNIT_NB = 3`); static `cur`, `ui`, `def`
- `disable()` — disables units from index 1 upward and sets cur to null

## Render State (`render.h`)

- Face culling: `_b_clockwise_ui`, `_s_cull_ui`, front/back polygon modes
- Lines: `_b_line_smooth_ui`, `_line_size`
- Points: `_b_point_smooth_ui`, `_point_size`, `_point_sprite_tex_ui`, `_point_sprite_origin`
- Lighting: `_b_light_ui`, `_b_gouraud_ui`, `_b_draw_using_normal_ui`
- Depth: `_b_depth_test_ui`, `_s_depth_test_ui`, `_b_depth_write_ui`
- Draw primitive: `_s_draw_primitive_ui`
- **Normal visualisation overlay**: `_b_top_normal_ui`, `_normal_len_point/face`, `_normal_color_ui[5]`
- **Wireframe/point overlay**: `_b_top_line_ui`, `_b_top_point_ui`, `_top_color_ui[5]`, offset params
- Random coloring: `_random_on_color`, `_random_on_color_store`
- Summary: `build_sum_up()`

## Fog (`fog.h`)

`c_fog` — `c_obj_active_ui` (alias for `c_obj_ui`). Static `cur`, `ui`.
- Mode, density, start/end, hint (DONT_CARE/FASTEST/NICEST), color `[5]`
- `enable()` / `disable()` / `update()`

## Geometry (`geo/`, `mesh.h`, `primitive.h`)

### `geo/points.h`
- `c_points_1d` — 1D array of REAL points
- `c_points_3d` — 3D points with deformed copy and draw copy; normal array
- `c_points_uv` — extends `c_points_3d` with UV coordinates
- `c_grid_point3d` — 2D grid of 3D points with file I/O and UV mapping

### Other geo/ files
- `sphere.h/cpp` — sphere tessellation
- `line_3d.h/cpp` — 3D line segments
- `polyline.h/cpp` — connected line segments
- `curve_flat.h/cpp` — 2D/flat curves

### `mesh.h` (`c_bdd_mesh`)
2D mesh/grid with perturbation (`c_fn1d_fm_add`) and regularisation (`c_fn1d_phase_pertub`).
Resolution: `_nb_x`, `_nb_y`. Normal generation: `_b_to_make_normal`.

### `primitive.h` (GOL namespace)
Template vertex and attribute arrays: `c_attrib<DIM>` (typedef'd as `c_attrib2/3/4`),
`c_prim_base<DIM>`. Use `REALLOC_ALIGNED_SIGNATURE` for aligned allocation.

## Shape Drawing (`shape.h`, `box.h`, `shape.cpp`, `box.cpp`)

`shape.h` / `shape.cpp` — non-box primitives: circles, ellipses, cones, cylinder, teapot, grid, `faces_draw`, `draw_light_test`, and the inline tri/mul/plus/mark line helpers.
- `shape::init()` allocates `c_prim2::base` and `c_prim3::base` vertex buffers, then calls `box::init()`
- `shape.h` includes `primitive.h`, which transitively provides `gol/gol_draw.h` **and** `gol/gol_color.h`

`box.h` / `box.cpp` — box and cube drawing: `draw_box_min_max*`, `draw_box_at_tgn`, `draw_cube`, `draw_cube_cano`.
- `box.h` includes only `infrastructure/obj/obj.h` — it does **not** pull in `gol_color.h` or `gol_draw.h`
- **Include chain warning**: any file that switches from `#include "draw/shape.h"` to `#include "draw/box.h"` loses transitive access to `gol_color.h` and `gol_draw.h`. Add those explicitly if the file uses GOL color constants or draw calls directly.
- `namespace box` in `box.cpp` owns: `guf_box_base`, `guf_box`, `points_cube_xyz_cano` (canonical size-1 cube, computed once in `init()`), and `compute_cube_align_points(vec, size)`. Managed by `box::init()` / `box::deinit()`.
- `draw_cube` caches the last size with a `-42.42` dirty sentinel and calls `compute_cube_align_points` only on change. `draw_cube_cano` skips that and draws directly from `box::points_cube_xyz_cano`.

### Two box drawing templates (file scope in `box.cpp`)

`draw_box_triangles_old_school<B_NOR, B_UV>` — used by `draw_box_at_tgn`. Draws a box in an arbitrary tangent frame; normals are computed from the `u`, `v`, `nor` direction vectors passed in. Always uses GL_TRIANGLES.

`draw_box_xyz_old_school<B_NOR, B_UV>` — used by `draw_box_from_points`. Draws an axis-aligned world-space box; normals are the fixed `unit_x_v4fp32` / `unit_y_v4fp32` / etc. constants. The `<false, false>` specialisation uses GL_TRIANGLE_STRIP (cheaper, no normals or UVs); all other combinations use GL_TRIANGLES.

### `if CONSTEXPR` one-liner idiom in box templates

```cpp
if CONSTEXPR (B_UV) GOL::texcoord2v( uv[0] );	GOL::vertex3v( vec[0] );
```
These are **two separate statements** on one line. The `if CONSTEXPR` guards only the `texcoord2v` call; `vertex3v` **always executes** regardless of `B_UV`. Do not read this as a conditional vertex.

## GUF — CPU Geometry Buffers (`guf.h`, `guf.cpp`)

`c_guf` wraps CPU-side point/normal/UV arrays used with legacy client-state GL (`glVertexPointer`, etc.).
- `draw_before(bool, bool)` / `draw_after()` — `draw_after()` delegates entirely to `draw_after_static()`, which reads only the **static** class members `b_normal_set` and `b_uv_set`. Calling it on any `c_guf` instance produces the same result — the instance is irrelevant.
- `_b_owner` flag: when true the object owns its arrays (alloc/free); when false it holds external pointers.
- `c_guf_index` extends `c_guf` with an index buffer; `alloc_for_strip` / `alloc_for_triangles` build the index.

## Other Draw Files

- `picking.h/cpp` + `picked.h/cpp` — object picking / selection
- `stencil.h/cpp` — stencil buffer operations
- `clip.h/cpp` — clipping planes
- `stereo.h/cpp` — stereoscopic 3D rendering
- `feedback.h/cpp` — **disabled**. Kept for legacy reference: this was the INTERGRAPH Demo feedback-buffer effect. The class body in `.h` is wrapped in `/* */`; the implementation in `.cpp` is wrapped in `#if 0`. `c_bdd_feedback` (`Src/obj_ui/bdd/bdd_image/bdd_feedback.h`) should carry most of its functionality. Do not attempt to re-enable without a full rewrite — it uses removed OpenGL APIs (immediate mode, `glOrtho`, `GL_QUADS`, `GL_ALL_ATTRIB_BITS`, etc.).
- `tex.h/cpp`, `tex_info.h/cpp`, `tex_anim.h/cpp`, `map.h/cpp` — texture loading, metadata, animation, mapping
  - `c_map::get_cur()` always returns a valid pointer — a default object is always present. No null check needed after `get_cur()`.

### `tex_anim.h/cpp` — force_start / force_stop pattern

`c_tex_anim` has two param pairs bound to the same `_start` / `_stop` member variables:
- `start` / `stop` — no update callback; sets the value silently
- `force_start` / `force_stop` — trigger `texture_anim_set_start` / `texture_anim_set_stop` callbacks

The dual binding is **intentional**: it allows setting the value either silently or with a triggered side effect. Do not "fix" this apparent duplication.
- `model.h/cpp` — 3D model loading and management
- `multiple.h/cpp` — multi-instance rendering
- `dust.h/cpp` — particle/dust effect
- `color.h/cpp` — color utilities
- `axe.h/cpp` — axis visualisation
- `Bspfile.h/BspFileStructs.h` — BSP file loading (legacy)
- `NSight.cpp/NSightEvents.h` — NVIDIA NSight debug integration
- `screen_def.h/cpp` — screen deformation (currently disabled with `#if AAA_SCREEN_DEFORMATION() 0`)

## Lua Bindings

`camera_lua`, `lights_lua`, `mat_lua`, `gol_lua` — expose subsystems to Lua.
