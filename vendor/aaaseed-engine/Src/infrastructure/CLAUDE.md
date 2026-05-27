# Infrastructure — Object Model, Factory, Params, Layers, Data

## Object Model (`obj/`)

### Class Hierarchy

```
c_obj        — base: counts live/created instances (obj_nb, obj_nb_created)
 └─ c_obj_ui — full runtime object: factory, params, name, tree, ID, Lua binding
     └─ (all concrete classes: c_fog, c_picking, c_render, c_light, …)
```

`c_obj_ui` is abstract (pure virtual destructor). All concrete objects go through the factory.

**`c_obj_active_ui` is a typedef alias for `c_obj_ui`** — not a separate class.
The original subclass was commented out; the typedef is retained for naming clarity only.

### Object ID System (`obj_ui.h`)

Packed `OBJ_UI_ID` (UINT32):
- Bits 0–19: unique ID (max ~1M objects, `ID_MAX = (1<<20)-1`)
- Bits 20–23: flags (`FLAG_REF_OBJ`, `FLAG_REF_PARAM`)
- Bits 26–31: existence signature (`SIGNATURE_OBJ_EXIST = 0x33`)

`get_from_id(id)` — O(1) lookup via global `id_to_obj` array.
`is_valid_obj_ui(id)` — uses signature bits to safely detect deleted/stale IDs.

### Tree / Branch Structure

`set_root(father)` / `get_root()` — navigate parent.
`get_branch(index)` / `get_branch_nb()` — navigate children.
Search helpers: `find_up_by_class<T>()`, `find_down_by_class_name()`, `find_from_top_by_name_symbo()`.

### Supporting Object Types

- `c_node` (`node.h`) — scene graph node
- `c_command` (`command.h`) — command/action pattern
- `c_state_master` (`state.h`) — state tracking (only compiled if `AAA_STATE_COMPILE()`)
- `c_obj_server<T>` / `c_obj_array_server<T>` (`obj_server.h`) — managed object arrays
- `c_obj_ui_array<T>` (`obj_ui_array.h`) — array of `c_obj_ui` pointers

---

## Factory & Registry System (`factory/`)

### Factory Macros — always used in pairs

| In `.h` | In `.cpp` |
|---------|----------|
| `FACTORY_DECLARE(class, super)` | `FACTORY_CREATE_V1(class, cid, human, ext)` |
| `FACTORY_ABSTRACT_DECLARE(class, super)` | `FACTORY_ABSTRACT_CREATE_V1(class, cid, human)` |

`FACTORY_DECLARE` injects: `SELF`/`SUPER` typedefs, `the_factory()`, `create_obj()`,
constructor/destructor declarations.

`CONSTRUCTOR_CREATE(class)` — defines the constructor signature (takes `c_factory_base*`,
chains to `SUPER( &the_factory() )`).

`EMPTY_DESTRUCTOR(class)` — defines an empty destructor.

**ODR trick**: the `b_trick_##class_name` global bool in `FACTORY_CREATE_MACRO` forces the static
factory to be constructed at startup — sensitive to static initialisation order.

### Registries

Two global `c_registry` instances in `factory.cpp`:
- `registry_by_cid` — keyed by class-id string
- `registry_by_file_ext` — keyed by file extension

`c_registry<T>` — `std::map` with `strcmp` comparator; keys heap-allocated (MALLOC).
`C_PCHAR_C` specialisation also heap-allocates values. `sign_in` / `sign_out` manage entries.

`c_map_server<T_KEY,T>` (`registry_generic.h`) — `unordered_map` with a single-entry inline cache
for O(1) repeated access. `find(key)` → existing or nullptr. `get(key)` → creates `T` if missing.

---

## Param System (`param/`)

Folder-internal details, helpers, conventions, known TODOs : see `Src/infrastructure/param/CLAUDE.md`.

### `c_param`

Each `c_obj_ui` holds an array of `c_param` (via `c_params`). A `c_param` has:
- `_pt` — `void*` pointer to the member variable it controls
- `_data` — shared `c_param_data*` (type, limits, name, symbolic strings); lazily owned
- `_more` — optional `c_param_more*`; lazily allocated; holds connex lists, obj reference, comment, error flag
- `_flags` — packed UINT32: bits 0–9 = param index; bits 20+ = boolean flags

### Param Types (`aaa::param::TYPE` in `param_def.h`)

`TYPE_INT32`, `TYPE_UINT32`, `TYPE_BOOL`, `TYPE_FP32`, `TYPE_DOUBLE`, `TYPE_SYMBOLIC`,
`TYPE_STR`, `TYPE_FILENAME`, `TYPE_GROUP`, `TYPE_GROUP_CLOSED`, …

**Type flags** (ORed into `TYPE` value at bits ≥ 16):
- `M_LOCK` — cannot be changed or saved
- `M_SAVE_NOT` — not saved (belongs to another object)
- `M_SYNO` (bit `FLAG_SHIFT+2`) — for `TYPE_SYMBO`: symbol string array carries synonyms (today exactly two strings per value, the array is twice as long as max-min+1; the `_X2` historical name lives on in the macros' `/2` divisor)
- `M_STATE_INSENSITIVE` (bit `FLAG_SHIFT+4`, only with `AAA_STATE_COMPILE()`) — param not state-tracked

(Earlier versions of these had a bit conflict at `FLAG_SHIFT+2`; resolved in 2024 by moving `M_STATE_INSENSITIVE` to `+4`.)

### Param Binding Pattern

Every concrete class overrides `param_init_pt()`:
```cpp
void c_fog::param_init_pt() {
    INT32 h = 0;
    param_set_pt( h, get_pt_active() );
    param_set_pt( h, _s_mode_ui );
    ...
    err_param_init_pt(h);   // debug check: h must equal PARAM_NB_MAX
}
```
`param_init_with(param_def_array, NB)` is called in the constructor.
`param_set_pt_v<N>(h, ptr)` — binds N consecutive elements via a single C++20 generic template (any compile-time N). Backward-compat aliases `param_set_pt_2` ... `param_set_pt_5` are 1-line forwards.
`param_set_pt_n(h, ptr, runtime_nb)` — runtime-sized array variant.
`param_set_pt_rgbfa(h, ptr)` — special-case binder for 5-element color arrays. The in-memory layout is `[R, G, B, A, F]` (R/G/B at 0..2, alpha at 3, factor at 4) but the param order exposed to UI / save / Lua is `[R, G, B, F, A]`. The helper binds 0,1,2 then 4 then 3 to produce that permutation. Do not replace with a plain `param_set_pt_v<5>` , the order would not match.

### Serialisation

`param_read_from_mem()` / `param_write_to_mem()` on `c_params`.
`cpy_from()` — copy params from another object.

---

## Connection / Trax System (`param/connex.h`, `param/Trax.h`)

### `c_connex`

Links a `c_param` to a `c_trax` animation driver:
- `_b_trax_out` — direction: true = trax is the output side, false = trax is the input side
- `_param` / `_param_obj` — always the non-trax side of the connection
- `_trax` — always the `c_trax*`
- `get_in_param()` / `get_out_param()` — direction-aware getters
- `get_param_in_set()` / `get_param_out_set()` — both return `_param` (not directional)

**Ownership rule**: always destroy via `c_connex::remove(con)`, NOT `delete con`.
The destructor intentionally does NOT call `unplug()` — doing so would mutate a container
being iterated.

### `c_trax`

Full `c_obj_active_ui` animation driver (connects to params via `c_connex` lists in
`c_param_more::_in` / `_out`). Drives parameters with functions: sine, random, MIDI, etc.

Key members: `_fn_type_ui`, `_freq_ui`, `_phase_ui`, `_value_out[3]`, `_min`, `_max`,
`_offset`, `_gain`, `_bias`, `_round`, `_limit_min/max`, MIDI/network output, drawing params,
data channel (`_channel_id`, `_control_index_ui`), `ST_TRAX_EVENT*` array, `_flux_filter`.

`c_traxs` (`traxs.h/cpp`) — collection of trax instances.

---

## Bind System (`bind/`)

`c_bind_abstract` (`bind.h`) — base, inherits `c_obj_ui`.
`c_bind final` — list of file paths with a current index.
**`alloc(nb, menu_sub_nb)` must be called before any `get_str()` access** — container is not empty-safe.

---

## Layer System (`layer/`)

### `c_layer`

A scene rendering unit. Sub-objects are lazy-created via `get_X()` methods:
`c_bdd` (geometry DB), `c_render`, `c_model`, `c_texturing`, `c_shading`, `c_stencil`,
`c_clip`, `c_tex_video`, `c_multiple`, `c_color`, `c_lights_switch`, `c_fog`, `c_def_node`
(deformer), `c_tex_anim`, and up to 2 `c_transfo_trs`.

Texture/geometry binding:
- `_bind_2d_out`, `_bind_3d_out`, `_bind_1d_out` — active index (used during render)
- `_bind_2d_ui`, … — user selection (may differ from active)
- `_bdd` — current geometry DB; `_bdds` map — one DB per factory type

### `c_layers`

Manages up to `LAYER_NB_MAX=26` `c_layer` objects, plus:
- Up to 16 cameras (`c_cameras`)
- Lua script (`c_lua_wrap`)
- Trax connections (`c_traxs`)
- FBO (framebuffer object)
- `c_obj_value` block

`get_cur()` / `get_ui()` — static current/UI instance accessors.
`layer_get_always_from_index(i)` — creates if missing.
`layer_get_from_index(i)` — returns null if out of range.

### `c_app` (`layer/app.h`)

Global `g_app` — top-level application object, root of the entire object tree.
Passed to `set_root()` for orphaned objects. Holds global rendering settings and modules.

### Layer Attribute Stack

`c_layer_att` / `c_layers_att` — push/pop rendering attributes (camera, lights, FBO, etc.)
for nested rendering. Managed as a static stack.

---

## Data System (`data/`)

`c_datacube final` — top-level multi-channel store. Global: `extern c_datacube* g_datacube`.
- Up to `DATAGRID_NB=8` channels, each a `c_datagrid`
- Access: `get_fp32(channel, row, col)`, `get_double(...)`, `get_str(...)`
- Mutation: `set_double()`, `set_str()`, `insert_double()`, `insert_str()`, `push_row()`, `pop_row()`
- Change tracking: `is_changed()` on `c_datagrid`

`c_data_line final` — single row: auto-growing deque of `c_data_elt*`.
`_size` tracks logical size in parallel with the deque — **both must stay in sync**.

`c_data_elt final` — single cell:
- `_b_number` flag
- `DOUBLE _d` — numeric value
- `o_str* _str` — optional string
- `set_str_and_double()` — sets both representations together

---

## Viewport (`viewport.h/cpp`)

`c_viewport final` (`c_obj_ui`) — viewport definition (position, size).

## Cameras Container (`cameras.h/cpp`)

`c_cameras` — collection of camera objects, owned by `c_layers`.

## Compute (`compute_master.h/cpp`, `compute_parallel.h/cpp`)

`c_compute_master final` (`c_obj_ui`) — coordinates parallel compute tasks.
`compute_parallel.h/cpp` — parallel computation workers.
