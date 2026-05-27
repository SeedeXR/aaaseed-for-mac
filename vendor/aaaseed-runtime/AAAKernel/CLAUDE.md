# AAAKernel

C++ + Lua kernel of the **AAASeed** realtime visual engine (Windows, developed since 1996 by Mâa). The name `GaBuZoMeu` is a nod to the **Shadoks** (1968 French animation series); `Ga`, `Bu`, `Zo`, `Meu` are the four syllables of the Shadok language. `Zo` is currently unused but will be back.

## Three conceptual layers

The Lua side of AAASeed runs as three stacked layers:

1. **AAASeed VM** , `aaa.*` and `gol.*` namespaces, Lua extensions on `string`/`table`/`math`. Pure functions, no class system. Documented in `AAADoc/lua_aaaseed_draw.md`, `lua_aaaseed_interface.md`, `lua_aaaseed_helpers.md`.
2. **GaBu** , starts the moment Lua classes are used (`GABU_OBJ` is the first GaBu file loaded). Adds the class system, the UI tree (`BU`/`BUS`/`BUSS`/`BUS_CTX`), the global dispatch (`GA`, `BLOB/BLOBS`, native multitouch), the application base (`APP`), and standalone utility classes (`FBO`, `IMG`, `PHASOR`, `STACK`, `QUEUE`, `TEXT`, `VIDEO`, `CHANGER`, `SHADING`, `TRANSFO`, ...).
3. **GaBuZoMeu** , adds the `MEU` (Module Editable Unit) meta-mechanism on top of GaBu: `MEU`/`MU`/`MUS`/`MEU_DIR`/`GP`/`APP_GP`. The frontier between GaBu and GaBuZoMeu is **MEU** (not APP, which lives in GaBu). `GP` (Garden Party) is the rendering loop and root MUS holder, owned by `APP_GP`.

## Loading mechanisms

- **VM** : `default.lua_master.lua` defines the sequential load order of `.lua` files in `AAAKernel/lua/`. The master ships in `AAAKernel/Dev/AAAUser/Guest/`, but at install time the installer copies the whole `AAAUser/` folder **at the same level as the folder containing the AAASeed executable**. AAASeed looks for that user-level `AAAUser/` first; the `AAAKernel/Dev/AAAUser/Guest/` copy is init/fallback only.
- **GaBu** : `AAAKernel/GaBu/GaBu_Util/default.layers_bind` and `AAAKernel/GaBu/Gabu_BU/default.layers_bind` are serialized C++ layer trees. Each `c_lua_wrap` layer in those trees owns one `.lua` and compiles/runs it according to its own params.
- **Hot reload** : every `.lua` is watched by its `c_lua_wrap`. On file change, the script is re-compiled and re-executed. With prototype-based inheritance (see MEU section), live instances pick up the new methods immediately.

### `c_lua_wrap` key params

| Param | Meaning |
|---|---|
| `filename` | path to the `.lua` file |
| `doit` | triggers a one-shot compile + run when ON |
| `doit_trig` | momentary version of `doit` (clears after firing) |
| `run_only_when_compiled` | run the script **only at the moment of (re)compilation**, not on every frame thereafter (typical for class-definition scripts) |
| `skip_rest` | skip the remaining layers of the current tree if this script just ran (gates dependent layers behind a successful Lua run) |
| `text` | inline script text (when not using a separate file) |
| `edit_trig` | triggers an open-in-editor action |

## Directory layout

```
AAAKernel/
├── lua/            AAASeed VM (aaa.*, gol.*, Lua extensions). GABU_OBJ.lua physically lives here
│                   but conceptually inaugurates GaBu (it brings the CLASS system).
├── GaBu/           GaBu UI framework + GaBuZoMeu (MEU layer). See three-layer split above.
│   ├── GaBu_Util/  Standalone utility classes (FBO, IMG, PHASOR, STACK, QUEUE, TIMER, ...)
│   └── GaBu_BU/    UI widget hierarchy + MEU system (BU, BUI, BUS, MEU, APP, GA, MU, MUS, ...)
├── AAA_PROTO/      MEU prototypes shipped with the kernel.
│   └── MEU_PROTO*/<Type>/   Subdivision (MEU_PROTO, MEU_PROTO_2, MEU_PROTO_MAA, ...)
│                            is historical / by author or maintainer, NOT a functional category.
├── Shader/         Built-in GLSL shader files
├── Texture/        Built-in textures
├── Fonts/          Font resources
├── Module/         Module-level config/resources
└── OpenCl/         OpenCL kernel files
```

## Architecture

- **C++ kernel** exposes objects (`c_obj_ui`) with named parameters to Lua via `param.*` API.
- **Lua glue** (`lua/`) wraps the C++ API into `aaa.*` (high-level) and `gol.*` (OpenGL) namespaces.
- **GaBu** (`GaBu/`) is the Lua UI + class framework on top of the glue.
  - `GA` = Global Action (high-level dispatch: keyboard, BLOB/touch, update, draw)
  - `BU` = Box User (interactive UI element)
  - `Zo` = unused (Shadoks reference, kept for the name)
  - `MEU` = Module Editable Unit (atomic functional block, GaBuZoMeu layer)
- **`GP` = Garden Party** (live coding / VJ culture). `APP_GP` is the APP variant that hosts MEUs.

## MEU prototypes (`AAA_PROTO/`)

Each MEU type lives in its own folder containing:
- `fx.aaa_layers_all` , C++ object tree (serialized, uses `//AAA::filename` directives)
- `<type_snake>.lua` , Lua script defining the standard methods (see below)
- Optional: `sha.vert`, `sha.frag`, `sha.geom`, `preset_0.plua`, `*.comp`

**Standard methods** (the MEU lifecycle entry points , they are *methods*, not hooks):

| Method | Role | Required |
|---|---|---|
| `define_meu_infos()` | returns `{author, date, tags, help, name_long}` | always (cleanliness) |
| `define_ui()` | builds the BUS via `self:add_*` (see `MEU_add.lua`). BU names drive load/save and external access. Called *before* MEU loading. | always |
| `init()` | first call at creation. Init variables, structures, C++ obj/param refs, costly ops. Called again on every `redefine_ui` (UI "Def" button). MEU init order in a MUS is **non deterministic**, do not assume other MEUs are present. | optional |
| `update()` | prepare for draw: set C++ params, prepare data. Anything you don't want to repeat inside `draw`. UI "**U**" button toggles its execution. | optional |
| `draw()` | actual rendering. If absent, the standard draw runs the C++ layers. If present, can draw via Lua, draw a single layer with custom attrs, loop with state changes, etc. UI "**D**" button toggles its execution. | optional |
| `update_ui()` | called only when MEU UI is visible. Refresh BU text, indicators, custom pre-draw ops. | optional |
| `draw_icon()` | custom icon drawing for the MU. | optional |
| `get_preset_nb()` | number of PRESET slots | optional |

**Hot reload nuance** : an empty method does NOT remove a previously defined one , the proto's method table accumulates across reloads. To actually remove a method, nil it from the proto or recreate the proto.

**Three implementation styles for a proto's `.lua`, from simple to more integrated** :
1. **`function meu:foo()`** , methods set on the global `meu` (= the proto). Simplest, most common. Use when the type needs methods only, no extra class state.
2. **`CLASS.DECLARE("MEU_<TYPE>", MEU)`** then methods on the subclass , when the type warrants its own class with state/behavior beyond methods (ex: Mire). `<TYPE>` here is in uppercase.
3. **Full class in `Gabu_BU/MEU_<TYPE>/`** , the proto's `.lua` is empty (just a comment), the class is fully defined in its own folder under `Gabu_BU/`. Used for MEU types tightly integrated with the kernel (Dir, Ref, TRAX, Video, GridSel, MuBegin, MuEnd).

**Prototype search strategy** when instantiating a MEU type:
1. **App / parent dirs** , `MUS:__get_dir_proto(b_search_up)` looks at `<MUS dir>/AAA_PROTO/<type>/`, then walks up parent directories, appending each `AAA_PROTO/` found. Walk stops at `dir_kernel` (the `AAAKernel/` root) OR filesystem root (`dir == ""`) OR `dir_start`. Format is **flat** at this level (`AAA_PROTO/<type>/`).
2. **Kernel** , `MEU_CTX:__get_kernel_proto_dirs` pre-loads at startup all `AAAKernel/AAA_PROTO/MEU_PROTO*/<type>/`. Note the **extra grouping level** (`MEU_PROTO*/`) only present at kernel.
3. **MEU_DIR opening** , each time a `MEU_DIR` instance is opened, its own folder is also probed for an `AAA_PROTO/` (so a MEU_DIR can carry private prototypes for its children).
4. **Self-proto fallback** , if no proto found anywhere, the instance is its own proto: it carries both roles, its `.lua` lives directly in the instance folder. This lets you create a new MEU type ad hoc inside an APP without first promoting a prototype.

The MEU on-disk tree mirrors the live tree exactly: each MEU instance is a folder; a `MEU_DIR` instance contains a sub-`AAA_MEU/` folder holding its children.

**Instance naming** is `<type>_<inst_key>`, split on the **first** underscore (so `Ref_Fbo_F1` → type `Ref`, instance `Fbo_F1`). Must be unique within its containing MUS. Allowed chars: alphanumerics + `_` `.` `-`. Case-insensitive match. For some types the instance key is functional (e.g. `Fbo_F2` uses `F2` as the FBO name; `Ref_<Name>` references the MEU named `<Name>`). Helpers (on `MEU_CTX`): `MEU_CTX.cur:split_meu_type_inst`, `MEU_CTX.cur:build_inst_name`, `MEU_CTX.cur:is_name_valid`. See [`AAADoc/MEU_and_MU.md`](../AAADoc/MEU_and_MU.md) for the end-user-oriented reference (naming, labels, find by name, cached lookups).

**Execution order** of MEUs in a MUS is **derived from the spatial layout of MUs**: bottom→top then left→right within their MUS, with `BU_RECT` groups taking priority by id. This layout is serialized via the BU position save mechanism (distinct from PRESET, which serializes BU values).

## Key entry points

| File | Role |
|---|---|
| `lua/GABU_OBJ.lua` | Base class for all GaBu Lua objects. `CLASS.DECLARE()`, up/down tree, print/debug helpers. |
| `lua/aaa_util.lua` | Global helpers: `switch()`, `pack()`, references init. |
| `GaBu/Gabu_BU/MEU/MEU.lua` | MEU class core. `MEU:create()` and `__init_new()` decide between **isolated proto / self-proto / pure instance** (the third uses `setmetatable(self, {__index=proto})` for prototype-based method inheritance). |
| `GaBu/Gabu_BU/MUS/MUS.lua` | `MUS:create_mu(meu_type, inst_key, label)` is the user-level entry point that creates a MU and its MEU. Trace this to understand instantiation. |
| `GaBu/Gabu_BU/MEU_add/MEU_add.lua` | All `add_*` UI builder methods used inside `define_ui`. |
| `GaBu/Gabu_BU/APP/APP.lua` | Base class for AAASeed applications (GaBu layer). |
| `GaBu/Gabu_BU/APP_GP/APP_GP.lua` | APP variant hosting MEUs (GaBuZoMeu layer). Holds a `GP` object with the root MUS, a `BU_CREATE_MEU` for runtime MEU instantiation, plus `BU_DOC`, `BU_BANK`, `BU_WIZ` widgets, and `BU_MONITOR`. |
| `GaBu/Gabu_BU/GA/GA.lua` | High-level dispatch: keyboard, BLOB/multitouch, update/draw orchestration. |
