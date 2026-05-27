# AAAKernel/lua — Core Lua glue layer

This directory contains the lowest-level Lua files that wrap the C++ kernel. They are loaded once at startup and define the `aaa.*`, `gol.*`, `param.*` global namespaces, plus a handful of core classes.

## File catalog

### Base class system
| File | Description |
|---|---|
| `GABU_OBJ.lua` | Root base class for all GaBu Lua objects. Defines `CLASS` registry, `CLASS.DECLARE()`, `GABU_OBJ` with print/debug/name/up-down tree/fn-table/param helpers. Every GaBu class is declared with `CLASS.DECLARE("Name", SuperClass, { ...fields })`. |

### `aaa.*` namespace — high-level AAASeed API
| File | Key exports |
|---|---|
| `aaa_util.lua` | `switch()`, `pack()`, `aaa.ref` (global C++ object refs), `aaa.activate/deactivate()` |
| `aaa_obj.lua` | `aaa.obj.*` — C++ object CRUD: `get`, `get_down`, `delete`, `update`, `draw`, `update_then_draw`, `set_focus_ui`, `get_from_top_by_class`, `is_ref_no_error` |
| `aaa_param.lua` | Wraps `param.*` — `get_ref`, `get`, `set`, `set_save`, `set_real_4`, `is_type_real`, `do_action_open` |
| `aaa_layer.lua` | `aaa.layer.*` — single layer ops |
| `aaa_layers.lua` | currently a stub (4-line placeholder; reserved for future Lua-side layers helpers). The `aaa.layers.*` C bindings (`draw_begin`, `draw_end`, `draw_layer_all`) are registered from the build repo. |
| `aaa_img.lua` | `aaa.img.*` — `read`, `exist`, `get_size`, `save`, `make_bank_bind_2d` |
| `aaa_camera.lua` | currently a stub (4-line placeholder; reserved for future Lua-side camera helpers) |
| `aaa_screen.lua` | `aaa.screen.*` — screen dimensions |
| `aaa_viewport.lua` | `aaa.viewport.*` |
| `aaa_keyboard.lua` | `aaa.keyboard.*` — `is_ctrl`, `is_shift`, `is_alt`, `is_alt_only` |
| `aaa_mouse.lua` | `aaa.mouse.*` |
| `aaa_net.lua` | `aaa.net.*` — OSC / network |
| `aaa_file.lua` | `aaa.file.*` — `get_dir_name`, path utilities |
| `aaa_format.lua` | `aaa.format.*` — string formatting helpers |
| `aaa_time.lua` | `aaa.time.*` — `t`, `dt`, `t_real`, `dt_real`, `get_time_now()` |
| `aaa_math.lua` | `aaa.math.*` extensions; also extends Lua `math.*`: `math.get_angle`, `math.get_length`, `math.rotate_ab_turn`, `math.pow`, `math.pi2`, `clamp_01`, `wrap`, `outside`, `inside` |
| `aaa_string.lua` | `aaa.string.*` and `string.*` extensions |
| `aaa_table.lua` | `table.*` extensions: `table.find_key_by_val`, `table.build_array_with_unique_value`, `table.copy_simple`, `array.*`, `pairs_sorted`, `IPAIRS`, `PAIRS` |
| `aaa_draw.lua` | Lua-side draw helpers: `aaa.draw_rect`, `aaa.draw_rect_uv`, `aaa.draw_rect_line`, `aaa.draw_rect_size`, `aaa.draw_str`, `aaa.draw_str_xy`, `aaa.draw_box`, `aaa.draw_sphere`, `aaa.draw_circle_axe_z`, `aaa.draw_disk_axe_z`, `aaa.draw_line`, `aaa.draw_rect_uv_at_z`, `aaa.bind_draw_rect` |
| `aaa_power.lua` | Power/preset management helpers |
| `aaa_audio.lua` | `aaa.audio.*` |
| `aaa_stereo.lua` | Stereo rendering helpers |
| `aaa_info.lua` | `aaa.info.*` — debug info display |
| `aaa_debug.lua` | `aaa.debug.*` — `print_traceback`, `show`, `show_warning`, `show_error`, `get_fn_info`, `get_fn_name` |
| `aaa_jit.lua` | LuaJIT control |

### `gol.*` namespace — OpenGL abstraction
| File | Key exports |
|---|---|
| `gol_util.lua` | All `gol.*` functions: colors (`gol.color`, `gol.color_white/red/…`), transforms (`gol.push_matrix`, `gol.pop_matrix`, `gol.translate`, `gol.rotate_x/y/z`, `gol.scale`), blend (`gol.set_blend_add`, `gol.enable_blend`), texture (`gol.bind_texture`, `gol.set_tex_unit_2d_bind`, `gol.set_quad_uv`), drawing (`gol.begin_triangles`, `gol.vertex`, `gol.texcoor2`, `gol.do_end`, `gol.set_default`), combined helpers (`gol.push_translate_scale_2d`, `gol.push_translate_rotate_z_scale_2d`) |

### Core classes (non-UI)
| File | Class | Description |
|---|---|---|
| `TRANSFO.lua` | `TRANSFO_TRS`, `TRANSFO_THREE` | Wrap C++ `transfo_trs` object. Methods: `set_translate(x,y,z)`, `set_rotate(x,y,z)`, `set_scale(x,y,z)`, `set_active(b)`. Angles in **turns** (0.25 = 90°). |
| `SHADING.lua` | `SHADING` | Wrap C++ shading object. |
| `RECT.lua` | `RECT` | Rectangle math helper. |
| `UPDATER.lua` | `UPDATER` | Callback registration for per-frame updates. |
| `OPENCL.lua` | `OPENCL` | OpenCL kernel wrapper. |
| `vector_2d.lua` | `V2` | 2D vector math. `V2.dist_from_xyxy(x1,y1,x2,y2)`, etc. |
| `vector_3d.lua` | `V3` | 3D vector math. |

## Key conventions

- **Angles are in turns** (not degrees, not radians): `0.25 = 90 deg`, `0.5 = 180 deg`, `1.0 = 360 deg`. `math.pi2 = 2 * pi`.
- **Colors** are `r,g,b,a` in `[0,1]`.
- **`param.*`** is the low-level C++ parameter bridge. Always prefer `param.get_ref()` for frequently-accessed params (caches the pointer).
- **`aaa.show_file_begin("name")` / `aaa.show_file_end("name")`** bracket each file for load tracking.
- **`CLASS.DECLARE("ClassName", SuperClass, {fields})`** registers a new class globally. Omit `SuperClass` to default to `GABU_OBJ`. Returns the class or `nil` if already declared (use `if CLASS.DECLARE(...) then ... end`).
- **iOS / `b_ios` branch is dead**: the `aaa.b_ios` reduced-VM branch that targeted Apple iOS in the 201x era is no longer used. When editing files in this folder, ignore everything inside `if aaa.b_ios then ...` (and the `else` clause of `if not aaa.b_ios then ... else ... end`). The desktop branch is the only live runtime.
- **Deprecated entries are flagged in source** with a `-- deprecated:` comment block above the function family. Do not delete them silently; leave a deprecation marker. They are intentionally absent from the public API doc in `AAADoc/lua_aaaseed_*.lua`.
