# Language / Lua Binding Layer

`Src/language/lua/` contains the C wrapper around the Lua C API (`aaalua_*.cpp/h`) plus the auxiliary subfolders `lua_sec` (Lua/SEC integration) and `lua_socket` (luasocket port). Subsystem-level Lua bindings (`*_lua.cpp` files) live in their respective subsystem folders (`Src/draw/camera_lua.cpp`, `Src/image/img_lua.cpp`, etc.) and are described per-subsystem.

This file documents the conventions and helpers shared across all binding files. New `*_lua.cpp` files must follow these rules.

## Files

| File | Responsibility |
|------|----------------|
| `aaalua_glue.h/cpp` | Macros: `AAALUACALL`, `ADD_FN`, `ADD_FN_WITH_HELP`, `LUAAAA_START`. Registration entry points. |
| `aaalua_util.h/cpp` | `c_lua_state` wrapper: stack helpers, type checks, table builders, value getters. |
| `aaalua_wrap.h/cpp` | `c_lua_wrap` -- a `c_obj_ui` that owns a Lua VM, runs scripts, dispatches hooks. |
| `aaalua_master.h/cpp` | `c_lua_master` -- singleton coordinating Lua VMs, error policy, edit-trigger. |
| `aaalua_array.h/cpp` | `arrayfp32` userdata type exposed to Lua for fast typed buffers. |
| `aaalua_debug.h/cpp` | Debug helpers: traceback build, line/script accessors. |
| `aaalua_draw.h/cpp` | Draw-side helpers separate from `gol_lua.cpp`. |
| `aaalua_exp.h/cpp` | Lua expression evaluation utilities. |

## Function declaration: `AAALUACALL`

A Lua-callable C function is declared using `AAALUACALL(name)` inside a namespace. The macro expands to:

```cpp
int name( lua_State* L_param )
```

The body has access to a `c_lua_state& l` reference (created by `LUAAAA_START`) and must return an int (number of values pushed on the Lua stack).

```cpp
namespace aaalua { namespace n_bdd {
    AAALUACALL( set_color )
    {
        LUAAAA_START( L, __FUNCTION__ );
        l.check_arg_nb( 4 );
        c_bdd* CONST bdd = get_bdd_cur( l );
        INT32 CONST  index = l.get_int32( 1 ) - 1;     // Lua 1-based -> C 0-based
        REAL col[3];
        l.get_v3( col, 2 );
        bdd->set_color( index, col );
        return l.return_nothing();
    }
}}
```

## Function registration: `ADD_FN`

Inside a `register_<sub>( lua_State* L )` function, the current Lua table is opened with `l.define_table("X")` and closed with `l.pop(N)`. `ADD_FN(name)` registers the C function under the current table.

```cpp
void register_bdd( lua_State* L )
{
    LUAAAA_START( L, __FUNCTION__ );
    l.define_table( "bdd" );
        ADD_FN( get_cur );
        ADD_FN( set_color );
        ADD_FN( set_color_map );
        ...
    l.pop( 1 );
}
```

Variants:
- `ADD_FN_WITH_HELP( name )` -- registers with an extra help string lookup.
- `ADD_FN_3D( name )` -- expands to `l.add_fn_to_table( #name, name##_3d )`. The C function is `name_3d`; the Lua name is `name`. Used inside the `img3d` table to mirror the 2D API surface.
- `add_fn_to_table( "lua_name", cpp_fn )` -- explicit renaming. Used when the desired Lua name is a C++ keyword (e.g. `delete` registered against `delete_obj`) or when consolidating multiple C functions under one Lua name.
- `//unused ADD_FN( name )` -- a commented-out registration kept on a single line with the `//unused` marker. Means: the `AAALUACALL(name)` is declared and compiles, but is intentionally not exposed to Lua scripts yet. Grep `//unused` to find these.

## Macros that emit `AAALUACALL` implicitly

Some declarations look like `ADD_FN`-flavored helpers but actually expand to a full `AAALUACALL` body plus the C function definition. They appear at file scope (alongside other `AAALUACALL` blocks), then they are still registered with a regular `ADD_FN` in the `register_*` function below.

| Macro | Expands to |
|-------|-----------|
| `AAALUA_CALL_FN( name, cpp_fn )` | A `void`-returning forwarder to `cpp_fn()`. |
| `AAALUA_CALL_FN_INT32( name, cpp_fn )` | Reads one int from the stack, calls `cpp_fn(int)`. |
| `AAALUA_CALL_FN_REAL( name, cpp_fn )` | Reads one real, calls `cpp_fn(real)`. |
| `AAALUA_CALL_FN_BOOL( name, cpp_fn )` | Reads one bool, calls `cpp_fn(bool)`. |
| `AAALUA_CALL_FN_RETURN_INT32( name, cpp_fn )` | Calls `cpp_fn()`, returns the int via `l.return_int32`. |
| `AAALUA_CALL_FN_RETURN_REAL( name, cpp_fn )` | Same for real. |
| `COOR_CALL( lua_name, method )` | (in `camera_lua.cpp`) declares `world_to_cam`, `cam_to_world`, etc. as Lua bindings forwarding to camera methods. |
| `DEFINE_COLOR_FN( color_name, r, g, b )` | (in `gol_lua.cpp`) declares `color_red`, `color_blue`, etc. with hardcoded RGB literals. |
| `FN_UPDATE_UNIFORM( name )` | (in `gol_lua.cpp`) declares the `update_uniform_*` family. |

Static analysis or grepping for `AAALUACALL` will miss the names produced by these macros. To get a full list of Lua-callable names registered in a given file, grep for `ADD_FN(` instead.

## `c_lua_state` helpers (in `aaalua_util.h`)

### Argument count
- `l.check_arg_nb( N )` -- assert the Lua call provided exactly N args.
- `l.get_arg_nb( min, max )` -- assert range; returns the actual count.
- `l.get_arg_nb( a, b, c )` -- one of these three exact counts.
- `l.get_arg_nb_min( min )` / `l.get_arg_nb_min_max( min, max )` -- variants.
- `l.get_arg_nb()` -- raw count, no assertion.
- `DBG_CHECK_ARG_NB( N )` -- debug-only `check_arg_nb`, no overhead in release.

### Single value getters (positional, 1-based)
- `l.get_int32( i )`, `l.get_uint32( i )`, `l.get_real( i )`, `l.get_fp32( i )`, `l.get_double( i )`, `l.get_bool( i )`, `l.get_str( i )`.
- Negative `i` indexes from the top of the stack: `-1` is the last pushed value.
- `l.get_value<T>( i )` / `l.get_value_direct<T>( i )` -- generic typed access.

### Multi-value getters: `get_vN`

`get_v2`, `get_v3`, `get_v4`, ..., `get_v8` (templated, defined in `aaalua_util.h:204+`).

```cpp
REAL pos[3];
l.get_v3( pos, 1 );             // reads stack positions 1, 2, 3 into pos[0..2]
l.get_v4( color, 2 );           // reads stack positions 2, 3, 4, 5 into color[0..3]
```

**Negative-index convention**: `get_vN(dst, index)` always reads stack positions `[index, index+1, ..., index+N-1]`, i.e. `index` is the **start** of the block, never the end. Consequently:
- `l.get_v3( pos, -3 )` reads `[-3, -2, -1]`, the last three stack slots.
- `l.get_v3( pos, 1 )` reads `[1, 2, 3]`, the first three.

The default index args on convenience wrappers reflect this: `get_int32_v3( dst, index = -3 )` defaults to "last 3" by passing `-3`.

There are two `get_vN` template implementations in `aaalua_util.h`. They MUST agree on the negative-index semantic above; a past divergence (where one version did `index -= NB-1` for negative indices, shifting reads by `-(NB-1)` slots) caused vec3/vec4 read corruption silently. Do NOT reintroduce that adjustment.

### Multi-value getters from a Lua table: `get_vN_table`
`l.get_v3_table( pos, index )` -- reads a Lua table at stack position `index` and unpacks its first 3 elements into `pos`.

### Return helpers
- `l.return_nothing()` -- return 0 (no values pushed).
- `l.return_int32( v )`, `l.return_fp32( v )`, `l.return_real( v )`, `l.return_bool( v )`, `l.return_str( v )`, `l.return_obj_ref( obj_ptr )` -- push one and return 1.
- `l.return_real_v2( vec )`, `l.return_real_v3( vec )`, `l.return_real_v4( vec )`, `l.return_fp32_v4( vec )` -- push N values from a buffer.
- `l.return_true()` / `l.return_false()` -- shorthand.

### Object-ref retrieval
- `l.get_aaa_obj( i )` -- generic obj_ui retrieval, errors on type mismatch.
- `l.get_aaa_obj_derived_from( i, "class_name" )` -- requires inheritance.
- `l.get_aaa_obj_of_class<T>( i )` -- typed retrieval (templated).
- `l.get_aaa_param_from_stack( i )` -- pulls a `c_param` ref.

### Errors
- `l.error_message( "fmt %d", arg )` -- prints + ascends back to the Lua VM with a Lua error.
- `l.error_and_escape( "fmt", arg )` -- error + non-local exit via Lua longjmp.

## Index conventions

Lua arrays/tables are 1-based; AAASeed C internals are 0-based. The convention at the binding boundary is:
- Lua callers pass 1-based indices.
- The C binding subtracts 1 immediately after `get_int32`: `INT32 CONST index = l.get_int32(i) - 1;`.
- The C function then uses the 0-based value internally.

Document the 1-based convention in the doc file at `M:\AAA\AAASeed\AAADoc\lua_aaaseed_*.lua` and in any helper comment, so script authors don't get surprised.

## Doc files in the sibling repo

The Lua API reference for end-users lives in `M:\AAA\AAASeed\AAADoc\` (separate git repo):
- `lua_aaaseed_interface.lua` -- main API surface (object model, networking, files, BDD, etc.).
- `lua_aaaseed_draw.lua` -- GOL/draw/image API.
- `lua_aaaseed_helpers.lua` -- standard-library extensions (math, string, table, array, format) defined in `aaa_*.lua` scripts.

Tag convention: a function entry tagged trailing `-- lua` is defined in an `aaa_*.lua` script (pure-Lua, no C binding); absence of tag means it's a C binding. Maintain this when adding entries.

When a new `AAALUACALL`/`ADD_FN` pair is added in a `*_lua.cpp` file, add a corresponding entry in the appropriate doc file. When a binding is renamed or removed, update the doc.

## Pure-Lua scripts that mirror the C surface

Many `aaa_*.lua` scripts in `M:\AAA\AAASeed\AAAKernel\lua\` add helpers ON TOP of the C bindings. Examples:
- `aaa_obj.lua` -- defines `aaa.obj.clone`, `aaa.obj.is_class` (pure Lua, but called from anywhere).
- `aaa_math.lua` -- defines `math.is_nan`, `math.dist_v2r`, etc. (extends standard `math.*`).
- `aaa_table.lua` -- adds `table.copy_deep`, `table.print`, `array.set`, etc.

These do not exist in the C binding files; they are Lua-side functions visible only because the script has been loaded. The `-- lua` tag in the doc files identifies them.

## Common patterns to watch

- **Negative-index API contract**: see "Negative-index convention" above. The two `get_vN` implementations in `aaalua_util.h` must remain consistent.
- **`AAALUACALL` declarations whose `ADD_FN` is `//unused`**: dead-code-like in practice, but the C function is still compiled. Grep `//unused` to find these intentionally-deferred bindings.
- **Macro-emitted bindings invisible to grep**: a static "what does Lua see" audit must include `DEFINE_COLOR_FN`, `FN_UPDATE_UNIFORM`, `COOR_CALL`, `AAALUA_CALL_FN_*`, plus the `ADD_FN_3D` suffix-stripping behavior.
- **Mismatch between C function name and Lua name**: when `add_fn_to_table( "lua_name", cpp_fn )` is used (e.g. `aaa.obj.delete` for `delete_obj`), grep for `delete_obj` in the C side and `aaa.obj.delete` in the Lua side -- they look different.
