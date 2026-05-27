# gl — Modern OpenGL Resource Layer (`Src/gl/`)

OOP-style GPU resource management. All types live in the `gl` namespace and inherit from `gl::item`.
This layer coexists with `gol/` — it uses `GOL::` helpers internally for low-level state, but exposes a cleaner object interface.

## Files

| File | Responsibility |
|---|---|
| `Engine.h/.cpp` | Singleton `gl::engine` — GL context management, double-buffered queues, mutex |
| `Item.h/.cpp` | Abstract base `gl::item` — `init_ogl()` / `update_ogl()` / `release_ogl()` interface |
| `OGLExternal.h` | Platform-dispatch header (GLEW/WGL on Win32, Linux, macOS) |
| `shader.h/.cpp` | `gl::shader` — GLSL program object, pipeline enum, dispatch |
| `tex.h/.cpp` | `gl::tex` — 2D texture with rich format/filter/wrap enums |
| `fbo.h/.cpp` | `gl::fbo` — framebuffer with multi-slot color + optional depth |
| `vao.h/.cpp` | `gl::c_vao` + `gl::c_vao_slot` — vertex array / vertex buffer objects |
| `ibo.h/.cpp` | `gl::ibo` — index buffer with built-in draw calls |
| `ubo.h/.cpp` | `gl::ubo` — uniform buffer object |
| `ssbo.h/.cpp` | `gl::ssbo` — shader storage buffer object (read + write) |
| `utils.h` | Binding-index enums and VAO slot index enums |
| `alignment.h` | GLSL std140 alignment macros and `gl::aligned_size_of()` |

**Not included in the AAASeed project build**: `tex.h/.cpp`, `fbo.h/.cpp`, `alignment.h`, `shader.h/.cpp` — present in the repo but excluded from `AAASeed_by_Maa.vcxproj`.

---

## `gl::engine` — Queue System

Singleton (`gl::engine::get_instance()`). Manages GPU work across threads via four double-buffered queues:
- **init** — upload new resources to GPU
- **update** — re-upload modified resources
- **release** — free GPU-side data
- **delete** — free CPU object after GPU release

All queue pushes are mutex-protected. `swap_queues()` atomically flips stock ↔ process buffers.
`process_queues()` drains the process queues on the GL thread by calling `init_ogl()` / `update_ogl()` / `release_ogl()`.

Context activation: `context_make_current()` / `context_done_current()` guard a `system_node*`.

---

## `gl::item` — Resource Base Class

All GPU resources inherit from `gl::item` (which inherits `c_obj`).

```cpp
virtual void init_ogl()    = 0;   // allocate GPU resource
virtual void update_ogl()  = 0;   // re-upload data
virtual void release_ogl() = 0;   // free GPU resource
```

`_gol_id` (UINT32) stores the raw OpenGL object id.

**Immediate vs queued**: every request method has a `bool b_immediate` variant:
- `true` → calls the GL function directly on the calling thread
- `false` → pushes to `gl::engine` queue for GL-thread execution

---

## Lifecycle Pattern — `make` / `reclaim`

All resources use a static factory pair instead of `new`/`delete`:

```cpp
gl::tex * t = gl::tex::make( props, /*b_immediate=*/true );
// ...
gl::tex::reclaim( t, /*b_immediate=*/true );
```

`reclaim()` calls `release_ogl()` then deletes the object (immediate) or queues it (deferred).
`item::release_and_null( ptr )` is a convenience helper that releases + nulls the pointer.

---

## `gl::shader`

Pipelines (`gl::shader_pipeline_e`): `compute`, `pixel`, `geometry`, `tessellation`.

`shader_props` holds: pipeline type + up to 5 GLSL source strings (`_prgms[5]`).

Key methods: `bind()`, `unbind()`, `dispatch(num_x, num_y, num_z)` (compute only).
`update(prgm1, ..., b_immediate)` — recompiles with new source strings.

---

## `gl::tex`

**Enums** (all in `gl` namespace):

| Enum | Values (selected) |
|---|---|
| `e_texture_format` | `gray_scale`, `RGB/RGBA/BGR/BGRA` + `16F/32F` variants, `depth*`, `depth24_stencil8` |
| `e_texture_data_type` | `unsigned_byte`, `real` |
| `e_texture_filter` | `linear`, `nearest` |
| `e_texture_wrap` | `border`, `edge`, `repeat`, `mirror`, `mirroredge` |

`tex_props`: data pointer, width, height, format, data_type, filter, wrap, `_generate_mipmaps`.

**Compute bind variants**: `bind_compute_read()`, `bind_compute_write()`, `bind_compute_read_write()` (and matching unbind), all take a `bind_index`.

---

## `gl::fbo`

`fbo_props` = list of `fbo_slot`s (color attachments) + optional depth.
Each `fbo_slot` has its own format, data_type, filter, wrap, clear_color.

Key methods:
- `bind()` / `bind_and_clear()` / `unbind()`
- `bind_slot_read(slot_index, bind_index)` — bind a color attachment as texture input
- `bind_depth_read(bind_index)` — bind depth attachment as texture input
- `update_resolution(x, y, b_immediate)` — resize all attachments

---

## `gl::c_vao` + `gl::c_vao_slot`

VAO uses `GOL::VAO_TYPE` for slot types (defined in `gol/gol_draw.h`).

`c_vao_props` holds a `std::vector<c_vao_slot_props>`. Note: `C_NO_CPY_MOVE` is intentionally
**not** applied to props classes — copy is needed for `emplace_back()` (e.g. in FBX mesh creation).

`c_vao_slot` tracks `_nb_allocated` separately from `_nb` — buffers are only reallocated when the new
size exceeds the previous allocation (reuse optimization).

`c_vao::set_slot_data(idx, data, b_immediate)` — update a single slot's data.

---

## `gl::ibo`

Uses `GOL::IBO_TYPE` (`IBO_UINT8`, `IBO_UINT16`, `IBO_UINT32`). Default is `IBO_UINT32`.

Draw calls live directly on `ibo` (no separate draw object needed):
```cpp
ibo->draw_triangles();
ibo->draw_triangles( instance_nb );   // instanced
ibo->draw_lines();
ibo->draw_triangle_strip();
ibo->draw_points();
```

Same `_nb_allocated` reuse optimization as `c_vao_slot`.

---

## `gl::ubo` — Uniform Buffer

Binding via `GOL::bind_buffer_base( GL_UNIFORM_BUFFER, binding_index, _gol_id )`.

```cpp
ubo->bind( binding_index );
ubo->bind_and_write( binding_index, data );            // full buffer
ubo->bind_and_write( binding_index, data, size );      // partial
ubo->write( data, size, dst_offset, b_immediate );
ubo->write_all( data, b_immediate );
ubo->map_write() / ubo->unmap();                       // persistent map
```

---

## `gl::ssbo` — Shader Storage Buffer

Same bind pattern as UBO but `GL_SHADER_STORAGE_BUFFER`. Adds `read()` (GPU→CPU readback).

```cpp
ssbo->write( data, size, dst_offset, b_immediate );
ssbo->read(  output, size, dst_offset );
ssbo->map_write() / ssbo->unmap();
```

---

## Binding Index Conventions (`utils.h`)

Fixed binding slots — shaders must match:

**UBO** (`e_ubo_binding_index`):
- 10 = `light_count`
- 11 = `model`
- 12 = `camera`
- 13 = `material`
- 14 = `scene_matrix`

**SSBO** (`e_ssbo_binding_index`):
- 13 = `directional_light`
- 14 = `point_light`
- 15 = `spot_light`

**VAO mesh slots** (`e_vao_mesh_slot_index`):
- 0=position, 1=normal, 2=texcoord, 3=tangent, 4=binormal, 5=bone_index, 6=bone_weight

---

## GLSL Alignment (`alignment.h`)

std140 layout rules encoded as macros:
- `AAA_GLSL_SIZEOF_VEC3` = `sizeof(float) * 4` (padded to 16 bytes, not 12)
- `AAA_GLSL_SIZEOF_MAT3` = `sizeof(float) * 16`
- `gl::aligned_size_of(size)` — rounds up to 4-byte (or 2-byte) GLSL alignment

Always use these when sizing UBO/SSBO structs mapped to GLSL uniforms.
