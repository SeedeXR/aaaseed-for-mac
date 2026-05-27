# Layer Subsystem , Layers, Modules, App, Attribute Stacks

This folder is the structural backbone of an AAASeed scene. See `Src/infrastructure/CLAUDE.md` for the high-level picture (object model, factory, params, layers, data) ; this file covers folder-internal details.

## Architectural rationale (the "why")

AAASeed is a static-depth render graph. The 4 layers of nesting in this folder (`c_layer` < `c_layers` < `c_module` < `c_modules`) are **not arbitrary** , each level adds a specific kind of behaviour, and the depth is fixed at 4 by design.

**`c_obj_ui` baseline.** Every object in the graph inherits `c_obj_ui` and exposes :
- A set of **params** (control inputs and output info), the public interface , the visible tip of the iceberg.
- `update()` , update internal state and prepare for draw.
- `draw()` , execute as fast as possible. Can be called several times in a row on the same object. Almost a misnomer : a `c_obj_ui` may do something other than rendering, so `draw()` is closer to `execute()` in spirit.

**The 4 nesting levels, from leaf to root :**

1. **`c_layer`** , the leaf. Encapsulates one `c_bdd` (the actual "drawer") plus a full attribute system. Each attribute slot is one of `current`, `owner`, `none` (optional or required, depending on the attribute). Attributes are updated before the BDD draws.

2. **`c_layers`** , a sequence of up to 26 layers (`a..z`), addressed by letter. The container adds :
   - A **Lua script wrapper** (`c_lua_wrap`) executed first to make the layers behaviour dynamic.
   - A set of **cameras** (`c_cameras`).
   - A **values** block (`c_obj_value`) for generic placeholders.
   - A **traxs** collection (`c_traxs`) , the older animation-driver mechanism, traxs can be plugged in or out on params. Lua scripting now does most of what traxs used to do, but traxs remain for legacy projects and when their model is the right fit.

   A `c_layers` is often enough to encapsulate complex behaviour, given how rich the BDD vocabulary is.

3. **`c_module`** , groups layers (each `c_module` owns N `c_layers`). Adds its own lua / values / traxs / cameras AND on top **lights** (`c_lights`) and optionally **materials** (`c_materials`), plus multi-screen handling.

4. **`c_modules`** , groups modules. Adds its own lua / values / traxs at the top level, used as the program-wide root reference (singleton-like via `c_modules::main`).

Above the four, **`c_app`** is the absolute top, exposed as `g_app`, holds the modules.

**The application side (Lua, sibling AAASeed repo).** The user-facing application layer builds **MEUs** (Module Editable Unit) on top of either a `c_layers` (a Lua-wrapped layers, possibly with sub-layers) or a `c_module` (when the extra lights / materials are needed). In that case the `c_layers` or `c_module` is no longer a direct branch of the global graph , the scripting layer manages it as part of a higher-level arborescence. See the sibling repo's `AAAKernel/lua/` and `AAADoc/lua_aaaseed_*.lua` for the script-side conventions.

## Class hierarchy and runtime containment

**Inheritance** (all derive from `c_obj_ui`, the four main classes are siblings via the `c_obj_active_ui` typedef alias) :
```
c_obj_ui                            (see Src/infrastructure/obj/)
 +- c_app                           global top-level (singleton g_app)
 +- c_obj_active_ui ( = c_obj_ui )
     +- c_modules
     +- c_module
     +- c_layers
     +- c_layer
```

**Runtime containment** (the static-depth render graph) :
```
g_app (c_app)
 +- c_modules                       owns N c_module*
     +- c_module                    owns N c_layers*
         +- c_layers                owns up to LAYER_NB_MAX = 26 c_layer*
             +- c_layer             owns one c_bdd + ~17 attribute sub-objects
```

API per containment level :
- `c_modules` , `module_*` family (`module_get_from_index`, `module_new`, `module_ui_set`, ...)
- `c_module` , `layers_*` family
- `c_layers` , `layer_*` family

`c_layer_att` and `c_layers_att` are the attribute push / pop stacks used for nested rendering (when a `c_multiple` re-enters the graph).

## File map

| File | Role |
|---|---|
| `app.h/cpp` | `c_app` global top-level, exposes `g_app`. |
| `layer.h/cpp` | `c_layer`, scene rendering unit. The big one (~67 KB). |
| `layer_att.h/cpp` | `c_layer_att`, push/pop stack for nested layer attributes (bdd, color, render, fog, ...). |
| `layers.h/cpp` | `c_layers`, layer container with cameras, traxs, FBO, lua. |
| `layers_att.h/cpp` | `c_layers_att`, push/pop stack for nested layers attributes (layers, lights, camera). |
| `module.h/cpp` | `c_module`, container of layers + per-module state (lights, materials, multi-screen). |
| `modules.h/cpp` | `c_modules`, container of modules. |
| `*_lua.h/cpp` | Lua binding shells (4 files, one per main class). |

## Static-instance pattern in this folder : `cur` + `ui` only (no `def`)

Unlike rendering/scene-state classes in `Src/draw/` (`c_render`, `c_lights`, `c_map`, `c_materials`, `c_multiple`, `c_color`, `c_stencil`, `c_texturing`, `c_tex_anim`, `c_model`, `c_ship`) and a few similar elsewhere (`c_tex_video`, `c_bdd_mocap`, `c_multi_screen`, `c_shading`), which maintain a **`cur` / `ui` / `def`** triple where `def` is an always-installed fallback so `get_cur()` never returns null, the four layer-subsystem classes maintain only :

- `cur` , currently active (used during rendering)
- `ui` , user-selected (may differ from `cur` during preview)

`def` is **not** present here. Consequences :

- `c_layer::get_cur()` / `c_layers::get_cur()` / `c_module::get_cur()` / `c_modules::get_cur()` **can return null**, callers must check.
- No "default fallback object" is auto-installed at startup ; if `cur` was never set or has been cleared, you get null.

## `c_modules::main`

[modules.h:30-33](modules.h) , `c_modules` adds a third static :

```cpp
static c_modules* main;
```

`main` is **not** a `def` fallback. It is the singleton-like top-level instance, set explicitly via `set_main()`, used as the program-wide root reference for the modules tree. Don't rename it `def`, the role is different.

## Layer sub-object ownership (`c_layer`)

`c_layer` owns ~17 sub-object pointers (lazy-created on first `get_*()`, freed in destructor) :
`_bdd`, `_render`, `_multiple` (+ `_multiple_for_bdd`, `_multiple_prev`), `_texturing`, `_shading` (+ `_shading_last`), `_map`, `_stencil`, `_tex_video`, `_model` (+ `_model_for_bdd`), `_tex_anim` (+ `_tex_anim_last`), `_color` (+ `_color_for_bdd`), `_lights_switch`, `_fog`, `_deformer_ui`, `_clip`, `_transfo_trs1`, `_transfo_three`, `_transfo_trs2`, `_mocap_direct`. Plus a `bdd_map` keyed by factory (one BDD per factory type).

Pattern : `get_X()` returns the existing sub-object or creates it on demand. The four `set_*` declarations in the public section ([layer.h:295-298](layer.h)) are commented out , the layer is intended to OWN its sub-objects, external assignment is not supported.

## Attribute stacks : `c_layer_att`, `c_layers_att`

Both follow the same shape after the 2026-05-08 cleanup :

- Single contiguous heap allocation in `c_init()` via `SAFE_NEW_ARRAY(stack, STACK_SIZE)`, freed in `c_deinit()` via `SAFE_DELETE_ARRAY`. Cache-friendly, one allocation instead of N.
- `STACK_SIZE` lives in the .cpp anonymous namespace (implementation detail, not part of the API).
- `stack_index = -1` means empty, valid slots are `stack[0..STACK_SIZE-1]` (no slot wasted).
- `push()` always increments `stack_index` ; the save into `stack[stack_index]` is gated on `< STACK_SIZE`. Overflow logs an error and keeps the counter in sync with the call.
- `pop()` always decrements `stack_index` ; the load is gated on `< STACK_SIZE`. Empty-stack pop fires a `debug_break` in debug builds. The push/pop pair stays balanced through the overflow region.

`c_layer_att::STACK_SIZE = 12`, `c_layers_att::STACK_SIZE = 8`. The two scopes are independent : `c_layer_att` tracks nested multiple-with-layer within one layer's draw, `c_layers_att` tracks nested multiple-with-layers within one layers's draw.

`c_layers_att::get_cur()` / `set_cur()` member methods wrap calls to `c_layer_att::push()` / `pop()` : pushing on the c_layers_att stack also pushes on the c_layer_att stack, so the two are co-managed.

## Bare `c_xxx::cur` versus `c_xxx::set_cur()` accessors

[layer_att.cpp:get_cur/set_cur](layer_att.cpp) reaches into ~20 static instances of various classes. Some go through `set_cur()` / `get_cur()` (`c_bdd`, `c_color`, `c_def_node`, `c_layer`, `c_map`, `c_texturing`, `c_shading`, `c_render`), others touch the `cur` field directly (`c_fog`, `c_lights_switch`, `c_stencil`, `c_clip`, `c_tex_video`, `c_model`, `c_tex_anim`, `c_transfo_three`, `c_transfo_trs::transfo1_cur` / `transfo2_cur`, `c_bdd_mocap`, `c_bdd_multiple`, `c_bdd_curve_edit`).

The mix reflects historical evolution : older classes still expose the field, newer ones grew accessor APIs. Promotion to accessors everywhere is **agreed but deferred** (requires preparatory work outside the layer folder, other priorities first).

## Known TODOs / known broken

- `FINLINE_PB` macro at [module.h:200](module.h) on `c_traxs* get_traxs()` (and ~14 other call sites across the code base). The macro is a **no-op** (empty `#define`) at [Src/code_utils/aaa_type.h:295](../../code_utils/aaa_type.h). The `_PB` suffix means **inline-Problem** : Mâa would want the function to be `FINLINE` but making it so triggers a compile or link error he has not yet resolved. The marker tags the function as a candidate to revisit when the underlying issue is understood. Until then, `FINLINE_PB`-marked functions are not inlined.
- `do_hack_new_module()` at [module.h:108](module.h) keeps the "hack" prefix as honest signalling : it bypasses the normal "loaded all" detection by setting the flag directly, called once when a freshly created module needs to be savable without going through a load. Kept as is.
- ~250 lines of dead text (commented blocks, `#if 0`, abandoned alternative implementations) catalogued in `layer_review_2026-05-07.md` section 5. Same policy as for param : leave alone unless a block is confirmed obsolete.

## Conventions specific to this folder

- The 4 main classes (`c_layer`, `c_layers`, `c_module`, `c_modules`) all inherit `c_obj_active_ui` and use the factory.
- `param_init_pt` in each class binds the params to its members ; the param defs live in a `n_X` namespace inside the cpp.
- `prepare_for_ui()` is the standard hook for refreshing UI-derived state before a frame.
- `param_attach_obj_no_inc()` is used to attach related objects (sub-objects of the layer/module) to params for navigation, without inc-ing the param index.

## See also

- Root `CLAUDE.md` : project-wide rules (English-only, ASCII-only, if-body-on-own-line, no dyn alloc, push_name pattern, SAFE_* helpers).
- `Src/infrastructure/CLAUDE.md` : the infrastructure subsystem in the wider picture (factory, registries, param, data, viewport).
- `Src/draw/CLAUDE.md` : the rendering classes that DO follow the `cur / ui / def` triple.
- `layer_review_2026-05-07.md` (project root, untracked) : the running review with deferred items and per-fix annotations.
