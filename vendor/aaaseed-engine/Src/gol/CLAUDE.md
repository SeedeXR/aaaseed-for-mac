# GOL — OpenGL Isolation Layer (`Src/gol/`)

All OpenGL calls go through the `GOL` namespace. Direct `gl*` calls outside GOL are not the pattern.

## Files

| File | Responsibility |
|---|---|
| `gol.h/.cpp` | Core namespace entry point, `update()`, capability flag init |
| `gol_base.h/.cpp` | Low-level buffer binding helpers (`bind_buffer_base`, `unbind_buffer_base`) |
| `gol_color.h/.cpp` | 140+ named colors (`enum class aaa_color`), 5-element `[R,G,B,F,A]` arrays |
| `gol_tex.h/.cpp` | Texture upload, NPOT support, `INTERNAL_TYPE` / `INTERNAL_TYPE_FORCE` enums |
| `gol_draw.h/.cpp` | VAO/VBO draw calls, `VAO_TYPE` / `IBO_TYPE` enums, CONSTEXPR lookup tables |
| `gol_buffer.h/.cpp` | Generic buffer object operations |
| `gol_shader.h/.cpp` | Shader compilation, linking, binding |
| `gol_matrix.h/.cpp` | Local GLM matrix stack mirroring GL fixed-function (`MODEL_VIEW`, `PROJECTION`, `TEXTURE`) |
| `gol_light.h/.cpp` | Fixed-function lighting, 8 lights max (`GL_LIGHT0`–`GL_LIGHT7`) |
| `gol_attrib.h/.cpp` | Push/pop attrib stack (`c_poper`, `c_attrib_stack` — 32 levels max) |
| `gol_pbo.h/.cpp` | Pixel buffer objects |
| `gol_os.h/.cpp` | OS/WGL context operations |
| `gol_list.h/.cpp` | Display lists |
| `gol_debug.h/.cpp` | GL error checking and debug reporting |
| `gol_lua.h/.cpp` | Lua bindings exposing GOL state to scripts |

## State Caching

GOL tracks GL state to avoid redundant driver calls.
- `is_state_cache_no()` — when true, bypass cache and always call GL (debug mode)
- Cached booleans follow naming: `b_<state>` (e.g. `b_blend`, `b_depth_test`)

**Disable vs enable rule** — disable functions must NOT check `allow_ui` flags:
- `disable_X()` always goes through unconditionally
- `enable_X()` respects `b_X_allow_ui`

**Capability flags** follow the three-part pattern:
```cpp
bool b_X_can;        // hardware/driver supports it
bool b_X_allow_ui;   // user/config allows it
bool b_X_use;        // actually active (can && allow_ui)
```
`update()` recomputes all `_use` flags from `_can` and `_allow_ui`.

## Dirty Sentinels

- `-42` — standard uninitialized/dirty marker (e.g. `fbo_id_cur = -42`)
- `color_cur = {-1,-1,-1,-1}` — intentional dirty sentinel for color state
- `reset_X()` functions use the dirty-sentinel trick to force re-upload on next use

## Push/Pop Attrib Stack (`gol_attrib.h`)

`GOL::push_att()` / `GOL::pop_att()` with `c_poper`.

- `c_poper`: stores a stack of function pointers + typed values; `pop()` executes them in reverse
- `c_attrib_stack`: fixed size — **32 levels maximum**
- Comments saying `//potential state sync problem` are known limitations of the snapshot-at-push approach

## Matrix Stack (`gol_matrix.h`)

`GOL::matrix` namespace mirrors the GL fixed-function matrix stack with a local GLM cache.
- Modes: `MODEL_VIEW`, `PROJECTION`, `TEXTURE`
- `GOL_MATRIX_CACHE()` compile flag controls dual caching — both code paths must stay consistent

## Drawing / VAO / VBO (`gol_draw.h`)

- `VAO_TYPE` enum: `FP32`, `VEC2`, `VEC3`, `VEC4`, `UINT4`, `UNDEFINED`
- `IBO_TYPE` enum: `IBO_UINT8`, `IBO_UINT16`, `IBO_UINT32`, `UNDEFINED`
- State: `b_vao_can`, `b_vao_allow`, `b_vao_do`, `vao_cur`, `b_vbo_allow`
- CONSTEXPR lookup tables for VAO component counts and byte sizes

## Textures (`gol_tex.h`)

- `INTERNAL_TYPE` enum: `UINT_8`, `UINT_16`, `FLOAT_16`, `FLOAT_32`, `DEPTH_*`, `DEPTH_STENCIL_*`
- `INTERNAL_TYPE_FORCE` enum: controls override of the default upload format
- NPOT support: `b_tex_npot_can`, `b_tex_npot_allow_ui`; `adjust_tex_size_to_valid()` for clamping

## Lighting (`gol_light.h`)

Fixed-function pipeline only. `LIGHT_NB = 8` maximum lights (`GL_LIGHT0`–`GL_LIGHT7`).
- State: `b_light_on[8]`, `b_lighting_allow_ui`, `b_lighting`
- `reset_light()` initialises all lights off with `-42` sentinels

## Direct State Access (DSA)

Capability flags: `b_direct_state_access_can`, `b_direct_state_access_asked_ui`, `b_direct_state_access_use`.
Named-buffer variants exist alongside the classic bind-then-operate API.

## Color Enum (`gol_color.h`)

`enum class aaa_color` defines 140+ named colors (WHITE, RED, GREEN, BLUE, DARK_BLUE, LIGHT_CORAL, …).
5-element color arrays `[R,G,B,F,A]`: F (index 4) is an intensity scale factor, not a second alpha.

## Lua Binding

`gol_lua.h/cpp` exposes GOL state to Lua scripts.
