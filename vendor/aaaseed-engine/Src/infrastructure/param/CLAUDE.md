# Param Subsystem — Class Collaboration, Layout, Helpers

This folder is the runtime backbone of AAASeed's "remote control": every named parameter exposed by an object lives here. See `Src/infrastructure/CLAUDE.md` for the high-level picture; this file covers folder-internal details.

## File / class map

| File | Class | Role |
|---|---|---|
| `param.h/cpp` | `c_param` | Per-instance runtime param. One per slot of an obj's param array. |
| `params.h/cpp` | `c_params` | Owns the contiguous `c_param[N+1]` array of one obj (slot 0 is the header). |
| `param_data.h/cpp` | `c_param_data` | Per-instance data: name, type, limits, fn_update, symbolic strings. Lazily owned by the c_param when a setter mutates it. |
| `param_declare.h/cpp` | `c_param_def` | Per-class data definition. The factory holds a static array of these and hands them to `c_params::init_for_obj`. |
| `param_def.h/cpp` | (no class) | The `aaa::param::TYPE` enum, `M_*` modifier flags, `type_info[]` table, `param_action_str[]`. |
| `param_more.h/cpp` | `c_param_more` | Optional, lazy-allocated extra: comment, error flag, attached-param info, connex lists, obj_ui id. |
| `param_focus.h/cpp` | `c_fifo` + `focus_param` namespace | UI focus state and recent-list management. |
| `param_draw.h/cpp` | `aaa::param::flat` (namespace) | Flatland rendering of params, dive-walking the param graph. Named `flat` because the global `class draw` (`seeddraw.h`) clashes with `aaa::param::draw` once `param_declare.h`'s `using namespace aaa::param;` is in scope. |
| `param_lua.h/cpp` | `aaalua::n_param` | Lua bindings for the param API. |
| `connex.h/cpp` | `c_connex` | Animation link between a param and a trax. |
| `trax.h/cpp` | `c_trax` | The animation driver itself (sine, random, MIDI, etc.). |
| `traxs.h/cpp` | `c_traxs` | Collection of trax instances per layer / module / app. |
| `trax_name.h/cpp` | (constants) | Name strings for trax types. |

## Memory layout

### `c_param` (compact, fits in 32 bytes)

```
_pt    : void*               // pointer to the controlled member
_data  : c_param_data*       // shared by default, owned after a mutating setter
_more  : c_param_more*       // null until comment/connex/attach is needed
_flags : UINT32              // packed
```

`_flags` packing:
- bits 0..9 : 10-bit param index. `M_ID = 0x3ff` (1023) is the "out of range" sentinel; `get_id()` walks predecessors when it sees the sentinel.
- bits 20+  : `PARAM_FLAGS` enum (DATA_OWNER, REFERENCED, LOCK, LAST, UNUSED, EXPAND_OUT/IN/LIST, DBG_DISPLAY, UPDATE_TRIG, STATE_SENSITIVE).

### `c_param_more::_flags_and_nb`

- bits 0..23 : count of attached params (`NB_MASK = 0x00ffffff`)
- bit 24      : error flag (`ERROR_MASK = 0x01000000`)
- bits 25..31 : reserved

### `c_param_data`

`_type_and_flag` (UINT32) = type in low 8 bits + modifier flags in high bits. See type system below.

### `c_param_data::_symbolic_str` is two-headed

It is `C_PCHAR_C*` for `TYPE_SYMBOLIC*`, but reinterpreted as `c_factory_group*` for `TYPE_CLASS_BRANCH` (via `get_factory_group()`). Both interpretations are valid depending on the param's type. There is no `set_factory_group()` ; assignment goes through `set_symbolic_str(C_PCHAR_C*)` and the read side casts.

## Type system (`param_def.h`)

`aaa::param::TYPE` enum with 24 values, in low 8 bits via `TYPE_MASK = 0xff`. See `Src/infrastructure/CLAUDE.md` for the catalog.

Modifier flags (bits ≥ `FLAG_SHIFT = 16`) :
- `M_LOCK` (`+0`) : cannot be changed or saved.
- `M_SAVE_NOT` (`+1`) : not serialized.
- `M_SYNO` (`+2`) : `TYPE_SYMBOLIC` symbol array carries synonyms. **For now (2026 May), "syno" means exactly two strings per value**, so the array is twice as long as max-min+1, hence the `/2` divisor in `PARAM_DEF_SYMBO_SYNO_PSTR`. The vocabulary is "syno" rather than "x2" to leave room for an N-strings-per-value generalisation if the need arises.
- `M_STATE_INSENSITIVE` (`+4`, gated on `AAA_STATE_COMPILE()`) : not snapshot-tracked.

The historical conflict between `M_SYNO` and `M_STATE_INSENSITIVE` at the same bit was resolved.

## Param binding pattern

Every concrete `c_obj_ui` subclass overrides `param_init_pt()` :

```cpp
void c_my_class::param_init_pt() {
    INT32 h = 0;
    param_set_pt(   h, _scalar_member );          //  1 slot
    param_set_pt_3( h, _xyz_array );              //  3 slots
    param_set_pt_5( h, _color_rgbfa );            //  5 slots (R,G,B,F,A)
    param_set_pt_v<7>( h, _array_of_7 );          //  generic, any compile-time N
    param_set_pt_n( h, _runtime_array, runtime_nb );
    err_param_init_pt(h);                         //  asserts h == PARAM_NB_MAX
}
```

**Positional API** : each call binds the Nth entry of `param[PARAM_NB_MAX]`. Adding, removing, or reordering a param requires three coordinated edits (the `param_def` array in the factory, the `param_init_pt`, and any consumer that referenced positions).

## Compile-time-N helper templates

`c_params` exposes three fold-unrolled families :

```cpp
template < INT32 N, class T > void set_pt_v(       INT32& h, T* pt );
template < INT32 N >          void set_pt_null_v(  INT32& h );
template < INT32 N >          void set_unused_v(   INT32 h, bool b );
```

Each unrolls into N explicit `c_param::set_pt(pt+I)` (or equivalent) calls via a C++20 generic lambda + `std::index_sequence` fold expression. The unrolling happens at template instantiation, **not** at the optimiser pass, so machine code is identical to a hand-rolled v3/v4/... at every optimisation level (Debug included).

Backward-compat aliases `set_pt_v2..v5`, `set_pt_null_v2..v5`, `set_unused_2..6` are 1-line forwards to the generic version, kept so call sites stay readable.

## Same-pt comparison

`c_param::is_same_pt(p1, p2)` (static, in `param.h`) returns true if both params reference the same `_pt`. Null-safe (returns false if either side is null). Used by `c_fifo::_store` to match the current focus state against stored entries.

## Lifecycle and ownership

- **`c_param`** : embedded contiguously in `c_params::_param_array` (one `new c_param[N+1]`, header at index 0). No per-param allocation.
- **`c_param_more`** : heap-allocated lazily via `get_more_always()` when a comment, connex, or obj reference is attached. Destroyed by `~c_param`.
- **`c_param_data`** : by default shared with the factory's static table. Promoted to per-param ownership through `set_data_owner()` when a mutating setter (`set_min`, `set_max`, `set_def`, `set_ina`, `set_type`, `set_save`, `set_limits`, `set_name`) is called. Ownership is tracked by the `M_DATA_OWNER` flag bit and the data is freed in `~c_param`.

`c_params` deals with the bookkeeping ; subclasses should not free `c_param_data` directly.

## Serialisation

### Text (file format)

`c_param::list_save_to_file` and `c_param::list_load_from_file` (declared in `param.h`, defined in `param.cpp`).

The save path uses a 1 MB scratch buffer (`BUF_SIZE = 1024 * 1024`) with a 32 KB safety margin (`SAVE_MARGIN_BYTES = 1024 * 32`). The margin check is performed **before** each `param->save()` so an oversized single-param write aborts cleanly without smashing the next stack frame. A post-write check (active in all builds) fires `debug_break` if a single save ever exceeds the margin, indicating either the buffer or the margin needs to grow.

The load path uses one `c_file_io* reader` per nesting level (`reader_push` / `reader_pop`), and a single global `buf_load[32 KB]` for token scanning.

### Binary in-memory

`c_param::write_to_mem` / `c_param::read_from_mem`, called via `c_params::param_write_to_mem` / `param_read_from_mem` for whole-array snapshots.

Asymmetric clamping by design: `write_to_mem` trusts that the value is already in `[min, max]` and stores it verbatim ; `read_from_mem` clamps as a security against corrupted or out-of-range buffer data. Strings (`TYPE_INTERNAL_STRING`) are not handled on either side yet.

## Connex / Trax (`connex.h/cpp`, `trax.h/cpp`, `traxs.h/cpp`)

See `Src/infrastructure/CLAUDE.md` for the c_connex / c_trax / c_traxs ownership rules. Folder-local highlights :

- `c_connex` destruction must always go through `c_connex::remove(con)`, never `delete`.
- The `c_connex` destructor intentionally does NOT call `unplug()` (would mutate a container being iterated).
- `c_trax` is a `c_obj_active_ui` (i.e. participates in the obj tree, not just data).

### Connex list ownership : `c_param_more::_in` / `_out`

A param's `c_connex::LIST*` is **lazy-allocated** by `c_param_more::add_con` the first time `add_in` / `add_out` is called, and **owned** by that same `c_param_more` : `~c_param_more` walks the list (`c_connex::remove` per item) and `SAFE_DELETE`s `_in` / `_out`.

**trax_header borrow pattern** : `c_traxs::prepare_for_ui` ([traxs.cpp:101-105](traxs.cpp)) makes each c_traxs UI proxy slot mirror the corresponding `c_trax`'s param_header (the trax's "self" param). Name, type, value pointer AND the `_in` / `_out` LIST pointers are copied into the slot. The slot does **not** own the lists ; the trax_header remains the sole owner. The mirror is set via the dedicated borrow API on `c_param` :

- `borrow_in(LIST*)` / `borrow_out(LIST*)` : install an externally-owned list pointer. Asserts non-null in debug.
- `unborrow_in()` / `unborrow_out()` : clear the borrowed pointer. **MUST be called before the borrowing _more is destroyed**, otherwise `~c_param_more` will `SAFE_DELETE` a list it does not own. `c_traxs::disconnect_all` ([traxs.cpp:332-347](traxs.cpp)) is the un-mirror step that nulls the borrows.

The borrow contract is currently implicit (no per-slot "borrowed" flag) and relies on the matched borrow / unborrow pair in `c_traxs`. **Pending cleanup, deferred to the trax / traxs review** : either make the borrow explicit (a flag bit on `_more` so the destructor can `debug_break` instead of double-freeing), or kill the borrow scheme entirely by having the c_traxs UI proxy slot delegate to the trax_header on the fly (single source of truth, no shared pointer).

A further memory-shaving option also belongs to the trax review : move `_in` / `_out` out of `c_param_more` for trax_header-flavored params (where the LIST genuinely lives "on the c_trax"), shrinking `c_param_more` for that case and dissolving the proxy / borrow scheme. Ordinary params (a `c_layer`'s `color_alpha`, a `c_fog`'s `_density`) cannot use that scheme : their connex list belongs to the param itself, not to any single c_trax.

## Conventions specific to this folder

- **`is_changeable()` = `is_pt() && !is_lock()`** is the standard guard before any direct setter (`set_int32_direct`, `set_double_direct`, ...). Use it.
- **No `is_X_not()` family** : `is_lock_not`, `is_empty_not`, `is_state_sensitive_not`, `is_referenced_not` were removed in 2026-05-06. Always write `!is_X()`.
- **`fn_update`** : optional callback fired on change unless `c_param::get_obj_loading()` is non-null. Several setters guard on this. The mechanism is not fully formalised yet ; expect future refactor before adding new fn_update consumers.
- **Negative-count assertions in destructors** : use `"%s() destructor called with X count already at zero"` (the dec-then-error pattern fires when count is already zero, not "we reach a negative count"). Sweep applied 2026-05-06.
- **Temp `o_str` use `o_str::push_name()` / `pop_name()`** : see root `CLAUDE.md` "No dynamic allocation" rule. Applied in `cpy_value_from_param_type_same`.

## Known TODOs / known broken

- **`AAA_STATE_COMPILE_PRIVATE` snapshot system is broken**. Enabling the flag would surface ~6 compile errors in `params.cpp:364-590` (`get_pt`, `get_pt_changeable`, `get_param_pt`, `set_int32`, `set_uint32`, missing `typename` on dependent map iterators). Decide the snapshot API before touching.
- **`c_param_data` and `c_param_def` overlap** : both hold the same logical data, with two slightly different APIs and duplicate ref-count machinery. Both files have `//todo merge` markers. Wait until the snapshot story is settled.
- **String snapshot in `read_from_mem` / `write_to_mem`** : `TYPE_INTERNAL_STRING` is silently skipped. Marked with `//hack we should do something here`.
- **`fn_update` semantics need to be locked down** ; once they are, revisit `cpy_value_from_param_type_same` to short-circuit the o_str copy via `set_value_str_direct( src->get_const_o_str() )`.
- **Several `#if 0` experimental blocks in `param_def.cpp`** (X-macro experiments, abandoned designs). To be removed in a cleanup pass.

## See also

- Root `CLAUDE.md` : project-wide rules (English-only, ASCII-only, if-body-on-own-line, no dyn alloc, push_name pattern).
- `Src/infrastructure/CLAUDE.md` : the param subsystem in the wider infrastructure (factory, registries, layer, data, viewport).
- `Src/code_utils/CLAUDE.md` : `o_str` and the static name stack.
