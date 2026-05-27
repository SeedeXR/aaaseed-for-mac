# AAASeed , Developer Guide

## 1. Who this guide is for

This is the developer's companion to AAASeed: how the runtime is laid out, how to extend it, where to read first when something behaves unexpectedly. It assumes you can read Lua, understand a class system with inheritance, and know what real-time rendering means at a high level.

For the high-level overview (audience: someone discovering AAASeed), see `AAASeed_overview.md`.
For the live API of the AAASeed Lua VM, see `AAADoc/lua_aaaseed_*.md` (auto-generated from `lua_aaaseed_*.lua`).
For agent-oriented catalogs of files and classes, see the `CLAUDE.md` files at `AAAKernel/`, `AAAKernel/GaBu/`, and `AAAKernel/GaBu/Gabu_BU/`.

## 2. Repository map

AAASeed is split across two repos that cooperate:

| Repo | Role |
|---|---|
| **`AAASeed_EXE`** (build, sibling) | C++ source, MSBuild + CMake projects, produces the executable. Hosts `Src/*_lua.cpp` files that register C functions as Lua bindings. |
| **`AAASeed`** (runtime, this one) | Kernel Lua scripts, GaBu UI framework, user APPs and MEUs, and the public API documentation. The EXE loads its content from here at startup. |

Within the runtime repo, who edits what:

| Path | Edited when ... |
|---|---|
| `AAAKernel/lua/` | Adding/changing the AAASeed VM (`aaa.*`, `gol.*`, Lua extensions) or `GABU_OBJ` core. |
| `AAAKernel/GaBu/GaBu_Util/` | Adding/changing standalone utility classes (FBO, IMG, PHASOR, STACK, ...). |
| `AAAKernel/GaBu/Gabu_BU/` | Adding/changing the UI framework or MEU system internals (BU, BUS, MEU, APP, GA, ...). |
| `AAAKernel/AAA_PROTO/MEU_PROTO*/<Type>/` | Adding/changing kernel-level MEU prototypes (shipped with the engine). |
| `AAAAPPs/<Group>/<APP>/` | Building an end-user application: APP code, MEU instances, app-local prototypes. |
| `APPs_Maa/` | Mâa's personal APPs / MEUs. |
| `AAADoc/lua_aaaseed_*.lua` | Editing the public API doc. After edits run `AAADoc/tools/regen_all.sh` to refresh the `.md` views. |
| `AAADoc/CLAUDE.md` and other `CLAUDE.md` files | Updating the agent-facing catalog when the architecture or conventions change. |

The two repos are independent (no submodules); their actual filesystem locations are environment-specific. When in doubt, ask the project owner.

## 3. The three Lua layers

The Lua side of AAASeed runs as three stacked layers. The boundary is conceptual: it is set by what the code uses, not by a marker in any file.

1. **AAASeed VM** , `aaa.*` and `gol.*` namespaces, Lua extensions to `string` / `table` / `math`. Pure functions, no class system. This is what `AAADoc/lua_aaaseed_draw.md`, `lua_aaaseed_interface.md`, and `lua_aaaseed_helpers.md` describe.

2. **GaBu** , starts the moment Lua classes are used. `GABU_OBJ.lua` is the first file that crosses the boundary (it brings `CLASS.DECLARE`). GaBu adds: the class system, the UI tree (`BU`/`BUI`/`BALUE`/`BUS`/`BUSS`/`BUS_CTX`), the global dispatch (`GA`, `BLOB`/`BLOBS`, native multitouch), the application base (`APP`), and standalone utility classes (`FBO`, `IMG`, `PHASOR`, ...). With just GaBu you can build a fully-fledged app.

3. **GaBuZoMeu** , adds the `MEU` (Module Editable Unit) meta-mechanism. Brings `MEU`, `MU`, `MUS`, `MEU_DIR`, `GP`, `APP_GP`. The frontier between GaBu and GaBuZoMeu is **MEU**, not APP (APP lives in GaBu). `APP_GP` is the GaBuZoMeu-aware APP that hosts MEUs; it carries a `GP` (Garden Party , the rendering loop and root MUS holder) plus a `BU_CREATE_MEU` window for creating MEU instances at runtime.

Practical consequence: the same codebase serves three audiences. A user only sees MEUs and their UIs. A MEU author writes Lua against `meu:` methods. A GaBu maintainer touches the framework itself and rarely the MEUs. Knowing which layer you are working in tells you which CLAUDE.md to start from.

## 4. Loading

Two distinct load mechanisms, one per layer.

### 4.1. VM loader

The VM master is `default.lua_master.lua`. It ships in `AAAKernel/Dev/AAAUser/Guest/`, but at install time the installer copies the whole `AAAUser` folder **at the same level as the folder containing the AAASeed executable** (so it sits next to the runtime tree, not inside it). At launch, AAASeed looks for that user-level `AAAUser` first; the `AAAKernel/Dev/AAAUser/Guest/` copy is only used as init/fallback when the user-level one is missing.

The master lists the `.lua` files of `AAAKernel/lua/` in their load order, then runs them sequentially to produce the AAASeed Lua VM. Adding a new VM-level file means editing this master.

### 4.2. GaBu loader

`AAAKernel/GaBu/GaBu_Util/default.layers_bind` and `AAAKernel/GaBu/Gabu_BU/default.layers_bind` are serialized C++ layer trees. Each entry is a folder; inside each folder, an `fx.aaa_layers_all` describes the layers, including a `c_lua_wrap` that owns the folder's `.lua`. `GaBu_Util` is loaded before `Gabu_BU` because that is the order the engine walks them.

Compilation and execution are handled by `c_lua_wrap`, not by the layer walk itself, see section 4.3.

The conceptual chain inside `Gabu_BU`:

- **GaBu** , `GABU_OBJ → BU → BUI → widgets (SLIDER/BUTTON/SELECTOR/...) → BUS → BUSS → BUS_CTX → GA → APP`
- **GaBuZoMeu** , `MEU → MU → MUS → GP → APP_GP`

The actual order is what `default.layers_bind` says; the chain above mirrors the dependency graph.

**Adding a new GaBu class**: easiest is to **duplicate an existing sibling folder**, rename the folder and the inner `.lua`, edit the `filename` field of the `c_lua_wrap` block in `fx.aaa_layers_all` to match, then add the folder to the `default.layers_bind` list (typically from the AAASeed UI, not by hand-editing the binary).

### 4.3. c_lua_wrap

`c_lua_wrap` is the C++ object that owns one `.lua` file and decides when to compile and run it. It is the unit of hot reload: every Lua script in the system, kernel or app, is loaded through one of these.

Observed params (in `fx.lua_wrap` blocks across the codebase):

| Param | Meaning |
|---|---|
| `filename` | Path to the `.lua` file the wrap owns. |
| `doit` | Triggers a one-shot compile + run when set to `ON`. |
| `doit_trig` | Same as `doit` but as a momentary trigger (cleared after firing). |
| `run_only_when_compiled` | Run the script **only at the moment of (re)compilation**, not on every subsequent frame. Typical for class-definition scripts that only need to register methods once after each compile. |
| `skip_rest` | Skip the remaining layers of the current tree if this script was just run (used to gate dependent layers behind a successful Lua run). |
| `text` | Raw script text (used when the script is held inline rather than in a separate file). |
| `edit_trig` | Triggers an open-in-editor action. |

What triggers an actual run, in practice:

- File modification detected by the wrap → re-compile, then run if the run policy says so.
- A `doit` / `doit_trig` request from Lua or UI → one-shot run.
- `run_only_when_compiled` pins execution to compile events only (no per-frame re-run); the wrap can also be globally suspended (live update OFF) for performance.

Mental model: "what needs to be updated is updated", and you can suspend live update for performance. The exact rules live in the C++ source of `c_lua_wrap` (in the build repo `AAASeed_EXE`); see there if you need precise semantics.

## 5. Per-MEU render cycle

The frame is **per-MEU**, not "all updates then all draws". The order is set by the spatial layout of MUs in their MUS (bottom→top, left→right, with `BU_RECT` groups taking priority by id). For each MU in this order, the engine calls `MEU:render(alpha)`.

By default, `MEU:render()` does `MEU:update()` then `MEU:draw()` on that single MEU. But a script can override `render()` and do something completely different , call `update()` multiple times, swap params between several `draw()` calls, run only `draw()`, run nothing, etc. So at the frame scale, MEUs interleave their own update→draw work, in the order their MUs lay out.

`MEU:update()` is where you do the per-frame state preparation (set C++ params, prepare data); the **U** button on the MEU UI toggles its execution. `MEU:draw()` does the actual rendering work; the **D** button toggles its execution. When `draw()` is absent, the standard draw runs the MEU's C++ layers tree.

`MEU:init()` is its own concern, see hot reload section.

In parallel, `GA` orchestrates the higher-level UI tree (`BUS_CTX.cur` → `BUSS` → `BUS` → `BU`) for input dispatch (touch BLOBs delivered to BUs via `BU:add_contact()`) and visibility. That walk is conceptually parallel to the MEU render loop.

## 6. Hot reload

Every Lua script in the system, GaBu classes, MEU protos, MEU instances, app scripts, is loaded through a `c_lua_wrap` (see section 4.3). When the wrap detects a file change (and live update is enabled), the `.lua` is recompiled and the wrap's run policy decides when to run it.

Effects on live state:

- **Class methods** , redefined methods land on the class table. Live instances see the new methods at the next call because Lua's `__index` lookup walks to the class table every time.
- **Instance state** , preserved across reload. Fields you stored on `self` keep their values.

(MEUs use the same machinery with one twist , an instance delegates to a separate "proto" object via `__index`. The MEU-specific reload behavior, including when `init()` is called and how `redefine_ui` rebuilds from scratch, is covered in the MEU sections below once the MEU model is in place.)

Caveats:

- **Empty method does NOT remove a method.** If you delete a function from a `.lua` and reload, the previous definition stays in the class/proto table. To drop it: `MyClass.foo = nil` (or `proto.foo = nil` for a MEU proto), or recreate the class/proto. This is the single most common reload trap.
- **Globals leak** , removing a global assignment from a file does not unset the global. Prefer locals.

## 7. The MEU model: MEU, MU, MUS

Three classes carry the GaBuZoMeu layer:

- **MEU** , the functional/rendering unit (the "block" that does something: a shader pass, a video, a clear, a camera control, a bank of presets, etc.). Encapsulates state and behavior. Defined by a **type** (e.g. `Mire`, `Fbo`, `Cam`) shared across instances.
- **MU** (Module Unit) , a UI handle derived from `BU` that points to a MEU. The MU is the manipulable visual object in the scene; the MEU is the underlying logic. One-to-one between a MU and its MEU. You move/resize/group MUs, you don't touch the MEU directly to do that.
- **MUS** , the container of MUs, analogous to BUS for BUs. A MUS holds a set of MUs in a 2D space; their **spatial layout** in that space defines the **render order** of the corresponding MEUs (bottom→top, left→right, with `BU_RECT` groups taking priority by id, see section 5).

A `MEU_DIR` is a special MEU type whose role is to hold a sub-MUS, so MEU trees nest recursively.

### 7.1. Naming a MEU

A MEU's name is `<type>_<inst_key>`, with the **first underscore** as separator (everything before it is the type, everything after is the instance key).

Examples:

| Name | Type | Instance key |
|---|---|---|
| `Displace_1` | `Displace` | `1` |
| `Mire_16` | `Mire` | `16` |
| `Fbo_F2` | `Fbo` | `F2` |
| `Ref_Fbo_F1` | `Ref` | `Fbo_F1` |

Allowed characters: alphanumerics `a-z A-Z 0-9` plus underscore `_`, dot `.`, dash `-`. Names are matched **case-insensitively** internally (uppercase is preserved only for display readability), so `Displace_1` and `displace_1` refer to the same MEU.

Uniqueness: a name must be unique among siblings (same parent MUS / same `AAA_MEU/` folder). Different folders can hold MEUs with identical names without conflict.

Helpers (on `MEU_CTX`, in `MEU_CTX/MEU_CTX.lua`):

```lua
local meu_type, inst_key = MEU_CTX.cur:split_meu_type_inst(name)   -- split on first underscore
local full_name          = MEU_CTX.cur:build_inst_name(meu_type, inst_key)
local b                  = MEU_CTX.cur:is_name_valid(name)         -- check character constraints
```

On a MEU instance:

```lua
local t   = m:get_meu_type()                  -- "Mire"
local k   = m:get_inst_key()                  -- "16"
local t,k = m:get_meu_type_inst_key()         -- both
local b   = m:is_meu_type("Mire")             -- type match
```

### 7.2. Labels (display-only)

A MEU/MU can carry a **label** distinct from its name. Names identify and look up; labels are pure display strings (used for shorter/clearer titles in the UI). Labels can be edited via the StarMenu rename command. They are stored in the MEU's `aaa.lua` file.

```lua
local s = obj:get_label()
obj:set_label("Short Title")
```

For more on naming, finding MEUs by name (`get_meu_by_name`, cached lookup), and label semantics, see [`AAADoc/MEU_and_MU.md`](MEU_and_MU.md).

## 8. Instantiating a MEU in an APP

When you compose an APP_GP, you put MEUs (and their MUs) inside its root MUS. The runtime entry point is `MUS:create_mu(meu_type, inst_key, label)` (see `AAAKernel/GaBu/Gabu_BU/MUS/MUS.lua`). End users do not call this by hand , they use the `BU_CREATE_MEU` window (held by `APP_GP`) to pick a type from the list of available prototypes and name the instance. The window calls `create_mu` for them.

What happens on the disk:

```
APP_X/AAA_MEU/
└── <type>_<inst_key>/        ← new folder created for the instance
    ├── preset_0.plua          ← auto-saved current state of the MEU's BU values
    └── (other instance-specific files as needed)
```

A `MEU_DIR` instance creates a sub-`AAA_MEU/` folder inside its own folder, holding its child MEUs recursively. The on-disk tree mirrors the live MUS/MEU tree exactly.

**What gets persisted, and how**:

| Concern | Channel | Stored in |
|---|---|---|
| Current values of all BU/BUI in the MEU | PRESET (slot 0 = automatic snapshot) | `preset_0.plua` |
| User-saved value snapshots | PRESET (slots 1, 2, ...) | `preset_<n>.plua` |
| Position of MUs in their MUS, BU_RECT ids | BU position save | Per-MEU layout files (BU pos save mechanism) |
| Whatever the MEU writes itself (custom files) | App-level | Wherever you choose |

The order in which MEUs render is **derived from MU layout** in their MUS (see section 7). There is no separate "execution order" file; moving a MU reorders execution.

## 9. Creating a new MEU type (a prototype)

A prototype defines a MEU **type**. Once a proto exists for type `Foo`, you can instantiate `Foo_1`, `Foo_2`, etc. inside any APP that can see the proto.

### 9.1. Lua prototype inheritance, in plain words

This is how MEU instances reuse code from their proto, and is worth understanding once before reading the rest.

In Lua, an object is a table. When you write `obj:foo()`, Lua looks for `foo` first on `obj` itself; if not found, it follows a metatable hint called `__index`, which can point to another table where the lookup continues. AAASeed uses this to share methods:

- **At proto load time**, the engine sets the global variable `meu` to the proto object. So `function meu:define_ui()` *sets* `define_ui` on the proto's table.
- **At instance creation**, the engine sets the instance's metatable so its `__index` points to the proto. Then calling `instance:define_ui()` does not find `define_ui` on the instance itself, follows `__index` to the proto, finds it there, and runs it with `self` = the instance.

Three consequences:

1. **All instances of a type share the proto's methods.** You write the method once on the proto, every instance sees it.
2. **Hot reload is transparent.** Edit `<type>.lua` and save: the proto's method table is updated; live instances see the new methods on the very next call (no instance update needed).
3. **Per-instance overrides are possible.** If you set `self.foo = function ... end` directly on an instance, that wins over the proto's `foo` for that instance only.

Whenever this guide says "the proto's methods are inherited by instances", it means exactly the above.

### 9.2. Where to put the proto

The engine searches for prototypes in this order (covered fully in `AAAKernel/CLAUDE.md`):

1. **App-local** , `<APP_dir>/AAA_PROTO/<type>/`
2. **Walk-up** , each parent directory of the app, looking for `AAA_PROTO/<type>/`. The walk stops at the **kernel root** (the `AAAKernel/` folder where the engine itself lives) , above that the kernel-level search takes over.
3. **Kernel** , `AAAKernel/AAA_PROTO/MEU_PROTO*/<type>/` (pre-loaded at startup; the `MEU_PROTO*` second-level grouping is historical / by author, not functional).
4. **Self-proto fallback** , if nothing is found, the instance is its own proto: its folder carries both the prototype `.lua` and the instance state. This lets you draft a one-off type inside an APP without first promoting a real proto.

Pick the level that matches the proto's audience: kernel for engine-shipped, walk-up for shared between sibling apps, app-local for app-private, self-proto for ad-hoc.

### 9.3. Files in a proto folder

Required:

- `<type_snake>.lua` , the script that defines the MEU's behavior (methods on `meu`, or a class).
- `fx.aaa_layers_all` , the C++ object tree of the MEU (the `c_lua_wrap` for the `.lua`, plus any `layers`/`module`/etc. the MEU draws). Normally edited from the AAASeed UI; you can hand-edit it if you know the format.

Optional, depending on what the MEU needs:

- `preset_0.plua`, `preset_<n>.plua` , default and user presets.
- Any resource files the MEU reads from its own folder. Common cases include GLSL shaders (`sha.vert`, `sha.frag`, `sha.geom`, `*.comp`), OpenCL kernels, CSV data, images, audio, custom configs. There is no fixed list; whatever your code needs.

### 9.4. Standard methods of a MEU

The engine sets the global `meu` to point to the running MEU before calling any of these methods. So `self` inside a method is the live instance, and other code in the script can refer to `meu` and reach the same object. At proto-load time, the same global `meu` points to the proto being built; that is why writing `function meu:foo()` at the top level of a proto's `.lua` registers `foo` on the proto's method table , exactly the mechanism described in 9.1.

| Method | Role | Required |
|---|---|---|
| `meu:define_meu_infos()` | returns `{author, date, tags, help, name_long}` | always (cleanliness) |
| `meu:define_ui()` | builds the BUS via `self:add_*` (see section 10). **BU names matter**: they drive load/save and external access. Called *before* MEU loading. | always |
| `meu:init()` | first call at creation. Init variables, structures, C++ obj/param refs, costly ops. Called again on `redefine_ui` (after the MEU is freed and recreated). MEU init order in a MUS is **non deterministic** , do not assume sibling MEUs are initialized yet. | optional |
| `meu:update()` | prepare for draw: set C++ params, prepare data. Anything you don't want repeated inside `draw()`. The **U** button on the UI toggles its execution. | optional |
| `meu:draw()` | actual rendering. If absent, the standard draw runs the C++ layers tree. If present, draw via Lua, drive a single layer with custom attrs, loop with state changes, etc. The **D** button toggles its execution. | optional |
| `meu:update_ui()` | called only when the MEU UI is visible. Refresh BU text, indicators, custom pre-draw work. | optional |
| `meu:draw_icon()` | custom drawing of the MU icon. | optional |
| `meu:get_preset_nb()` | number of PRESET slots. | optional |

### 9.5. The render method

The frame-level entry point on a MEU is `meu:render(alpha)`. By default, it does `update()` then `draw()` (see section 5). Override `render()` if you need to break that pattern , skip `update()`, run `draw()` multiple times, swap params between draws, etc.

### 9.6. Three implementation styles, from simple to more integrated

How you organize the proto's `.lua` depends on how much custom behavior the type needs.

1. **Methods on `meu`** (simplest, most common): just `function meu:define_ui() ... end`, etc. The script body sets methods on the global `meu`, which at proto load time is the proto itself. All instances inherit these methods (see 9.1). Use this when the type only needs methods and no extra class-level state.

2. **Subclass via `CLASS.DECLARE`** in the proto's `.lua`: `if CLASS.DECLARE("MEU_<TYPE>", MEU) then ... end` at the top, then `function MEU_<TYPE>:foo()`, etc. Use this when the type warrants its own class (with class-level state, inheritance from a richer base, etc.). `<TYPE>` is the type name in **uppercase**.

3. **Full class in `Gabu_BU/MEU_<TYPE>/`**: the proto's `.lua` is empty (just a comment), and the class lives in its own folder under `Gabu_BU/`, hot-reloaded like any other GaBu class. Used for types tightly integrated with the kernel: `Dir`, `Ref`, `TRAX`, `Video`, `GridSel`, `MuBegin`, `MuEnd`. The instantiation path detects `MEU_<TYPE_UPPER>` and uses it automatically.

### 9.7. Hot reload of a MEU proto

Editing `<type>.lua` and saving triggers the proto's `c_lua_wrap` to recompile and re-run the script. Methods redefined on the proto are immediately visible to all live instances (see 9.1). Instance state on `self` is preserved.

`init()` is **not** re-called on plain hot reload of the proto; live instances keep their state. To force a fresh init, use the **Def** button on the MEU UI: it calls `redefine_ui`, which **frees the MEU first**, then creates it again, so `init()` then `define_ui()` rebuild from scratch.

The empty-method-does-not-remove-a-method trap (section 6) applies here: removing a method from the file leaves the previous one in the proto's table.

## 10. Defining the UI of a MEU

`MEU_add.lua` (in `AAAKernel/GaBu/Gabu_BU/MEU_add/`) is the entry point: it defines the family of `self:add_*` methods you call inside `define_ui()`.

### 10.1. The grid

All `add_*` methods take an `irect = {ix, iy [, nx [, ny]]}`:

- `ix`, `iy` are 1-based grid coordinates: `ix` = column from the left (1 = leftmost), `iy` = row from the top (1 = topmost; `iy` increases downward).
- `nx`, `ny` = width/height in cells.
- The grid is **16 × 16** cells; one cell is 1/16 of BUS space `[-0.5, +0.5]`.
- For multi-column or multi-row layouts, track `iy` (and `ix`) yourself between calls.

### 10.2. Adders by concern

The full menu of `add_*` lives in `MEU_add.lua` and friends. Group by concern:

| Concern | File | Examples |
|---|---|---|
| Generic | `MEU_add/MEU_add.lua` | `add_button`, `add_slider`, `add_param_obj_name`, `add_trig`, `add_text_info`, `add_rgb`, `add_selector`, `add_rgbfa`, `add_monitor` |
| Camera | `MEU_camera/MEU_camera.lua` | `add_camera` |
| Transfo | `MEU_add_transfo/MEU_add_transfo.lua` | `add_transfo`, `add_transfo_tab` (wraps `TRANSFO_TRS`) |
| Texture | `MEU_add_texture/MEU_add_texture.lua` | `add_bu_texture`, `add_bu_texture_target_unit`, `add_bu_texture_target_unit_nb` |
| Shader | `MEU_shader/MEU_shader.lua` | `add_shading`, `add_shading_ui`, `add_shading_sliders` |
| Pixel | `MEU_pixel/MEU_pixel.lua` | `add_pixel_size`, `add_pixel_format` |
| Time | `MEU_time/MEU_time.lua` | `define_time(rect, max, b_save)` , builds the time controls (play, restart, speed, looping, max). Companion methods: `update_time`, `restart_time`, `time_play`, `set_time_max`, `set_time_speed`, `set_time_looping`, `is_time_looping`, `update_time_ui`. |

### 10.3. Why BU names matter

Every BU/BUI you add inside `define_ui()` becomes a named child of the MEU's BUS. Those names are the **keys** for:

- **Save / load** of values (PRESET) , the preset file references each BUI by its name.
- **External access** , code that wants to reach into the MEU (siblings, scripts, OSC, MIDI bindings) finds a BUI by `<bus>:find_bu_by_name_lowercase(...)` style lookups.
- **MIDI / network bindings** , your config refers to BUIs by name.

Consequences:
- Pick names you can live with. Renaming a BU later breaks existing presets that referenced the old name.
- Names must be unique within their parent BUS (two BUIs in the same MEU UI cannot share a name).
- Convention: short, English, snake_case for compound names, no whitespace.

The same caution applies to the **instance key** of a MEU itself: for some types it is not just an identifier but is **used functionally** by the MEU. Examples:
- `Fbo_F2` , the instance key `F2` is used as the FBO name (so naming siblings `Fbo_F1`, `Fbo_F2`, ... binds them to FBOs `F1`, `F2`, ...).
- `Ref_Displace_1` , a `Ref`-type MEU references another MEU by name. The instance key (`Displace_1`) **is** the name of the referenced MEU.

Pick instance keys with that in mind for these types; for plain types (e.g. `Mire`) the key is purely an identifier.

## 11. Binding a value: BUI, BALUE, target

A `BUI` (Box User with values) is **autonomous**: it holds and lets you edit one or more values of its own. Each value is wrapped in a `BALUE` object (one BALUE per value dimension), so a SLIDER has one BALUE, a SLIDER_XY has two, a SLIDER_MULTI has many. A BUI holds its BALUEs in a Lua table (`self.__balues`, accessible via `BUI:__get_balues()`); the size of that table is the BUI's value-arity.

A BALUE carries:
- min / max / default / "ina" (inactive value)
- a canonical form in `[0, 1]` (used by MIDI mapping, automation, ...)
- type info: integer, bool, or float (default)
- a value source: either **internal** (the BALUE itself stores it , the default) or a **target** binding that links it to an external source (see 11.3).

### 11.1. Reading and writing values

Two accessor pairs, depending on whether you address one BALUE by id or the whole collection at once:

```lua
-- single BALUE (id is 1-based; default 1 for single-value BUIs)
local v = bui:get_value(id)
bui:set_value(value, id)

-- whole collection
local vs = bui:get_values()    -- single value if arity is 1, else a table
bui:set_values(value_or_table) -- table replaces all; single value targets BALUE 1
```

Companion accessors: `get_min(id)`, `get_max(id)`, `set_min_max(min, max, id)`, `get_def(id)`, `set_def(v, id)`, `get_ina(id)`, `set_value_type_integer(b, id)`, `set_value_type_bool(b, id)`, `flip_value(id)`, `get_value_as_bool(id)`, plus the canonical-form variants `get_value_cano(id)` / `set_value_cano(val, id)` (operating in `[0, 1]`).

Use `set_values_ui` (and the matching `_ui` variants) when the change should also feed the undo/redo history , the plain `set_value*` paths bypass undo.

### 11.2. From a MEU: addressing a BU by key

Inside a MEU's methods, you typically don't keep direct references to every BU you added in `define_ui()`. Instead, address them by their **key** (the name you gave to the BU at creation time). The MEU exposes shortcut helpers:

```lua
local bu = self:get_bu_by_key("speed")        -- retrieve the BU object

self:set_bu_value(  "speed", 0.5)             -- set BALUE 1's value
self:set_bu_value(  "speed", 0.5, id)         -- set BALUE id's value
self:set_bu_values( "color", {r, g, b, a})    -- set the whole collection at once

local v = self:get_bu_value("speed")          -- get BALUE 1's value
local v = self:get_bu_value("speed", id)      -- get BALUE id's value

local b = self:get_bu_value_as_bool("active") -- bool view
```

Lookup is **case-insensitive**: `get_bu_by_key("Speed")` and `get_bu_by_key("speed")` resolve to the same BU.

These helpers replace dozens of `bus:find_bu_by_name_lowercase(...)` boilerplate and are the idiomatic way to read/write your own UI from inside `update`, `update_ui`, callbacks, etc. Source: `AAAKernel/GaBu/Gabu_BU/MEU_bu/MEU_bu.lua`.

Subclass-specific accessors exist on top of these (selectors, color BUs, texture BUs, ...) , too many to list here; look at the class file when you need one.

> **Known gap**: there is no `MEU:get_bu_values(key)` symmetric to `set_bu_values`. For now you have to write `self:get_bu_by_key(key):get_values()`. To be addressed.

### 11.3. Targets , optional binding to an external source

A BALUE is autonomous by default: it stores its own value, no binding involved. Optionally, you can bind it to a **target** so the value actually lives elsewhere. AAASeed currently provides two target kinds:

```lua
-- target_lua: bind to a field in a Lua table
bui:set_target_lua(tab, key, val, id)

-- target_param: bind to a c_param (a direct one-to-one wire to a c_obj_ui's knob)
bui:set_target_param(param_ref, id)
```

`id` selects which BALUE inside the BUI (defaults to 1 for single-value BUIs). The model is open: more target kinds (network endpoints, automation curves, ...) can be added without changing the BU/BUI surface.

When a target is set, the BALUE and the target stay in sync. Reads pull from the target; writes push to the target. Multiple BALUEs can share one target, in which case editing one updates them all. Changes that come from elsewhere (a direct write to the c_param, MIDI, OSC, an animation curve) flow back into the BUI display because the BUI re-reads the target.

**`target_param` is one specific binding shape, not the way to talk to the C++ side.** The general-purpose route to interact with `c_obj_ui` and `c_param` is the Lua references-and-functions API: `aaa.obj.get_*`, `param.get_ref(obj, name)`, `param.get(ref)`, `param.set(ref, val)`, and friends. With that route the script keeps full freedom, reads/writes any c_param at any time from `update()`, derives values from arbitrary combinations of BUIs, etc. Use `target_param` when the direct one-to-one wire between a BUI and one c_param happens to be the right shape; otherwise hold the c_param ref yourself and drive it from `update()`. An entire APP can run without ever calling `set_target_param`, and many do even when they talk to the C engine. An APP that doesn't talk to `c_obj_ui` at all needs neither.

Unbinding (returning to the internal state) is supported through the same setters with appropriate arguments , see `BALUE.lua` for the exact API.

### 11.4. Reacting to changes

Callbacks for value changes:

```lua
bui:set_function_on_value_change(fn)            -- standalone function
bui:set_method_on_value_change(obj, method)     -- method call
```

Both fire when the BUI's effective value changes (whether the change came from user interaction, a target write, MIDI, etc.).

### 11.5. Reading the source

`AAAKernel/GaBu/Gabu_BU/BALUE/BALUE.lua` is the authoritative read for the target/sharing semantics, type handling, and edge cases.

## 12. Event dispatch: keyboard, touch, UIF

GA owns the high-level dispatch and feeds the UI tree.

### 12.1. Keyboard

Keys flow through `GA:do_key(key)` (ASCII keys) and `GA:do_key_special(key)` (function keys, arrows, etc.). Each level (`GA_HELP`, the focused BU, the running APP, finally GA itself) gets a chance to consume the key:

```
GA:do_key_special(key)
  → GA_HELP:do_key_special   (handles F1)
  → focused_bu:do_key_special (if any)
  → app:do_key_special       (if any)
  → GA's own handlers        (F2 toggles UI, F3, ...)
```

A handler returns `true` to consume the key.

### 12.2. Touch (BLOB)

AAASeed is natively multitouch. Each contact is a `BLOB`; the set of live contacts in a frame is `BLOBS`. Dispatch flows top-down:

```
GA → BUS_CTX.cur → BUSS → BUS → BU
```

The capture point on a BU is `BU:add_contact(blob)`. From there, the BU tracks the contact in its `update()` (move, untouch, ...). Click semantics are derived: `do_click_down`, `do_click_up`, `do_click_double`, `do_click_triple`, `do_click_long`. Wire callbacks with:

```lua
bu:set_function_on_click(fn)
bu:set_method_on_click(obj, method)
-- and similarly for click_double / click_triple / click_long
```

A BU can act as a drop target:

```lua
bu:set_drop_receiver(receiver_object)
-- receiver:drop_on(target, x, y)  is then called when a drop happens
```

### 12.3. UIF (Star Menu)

`BU_uif` is a context menu attached to every BU. It opens with a **left-click or quick-drag** on a BU whose plain click already has another meaning (BUTTON, BU_MONITOR, ...). The menu's content varies by BU type and exposes per-BU operations (rename, edit, copy, ...).

UIF is a major area of upcoming work; the detailed contract for adding entries to a BU's UIF is being defined.

## 13. Persistence: three channels

Three independent channels persist different aspects of an app:

| Channel | What it stores | Where |
|---|---|---|
| **PRESET** | BU/BUI **values** of one MEU, in slots | `<MEU folder>/preset_0.plua` (auto current state) and `preset_<n>.plua` (user slots) |
| **BU position save** | Positions of MUs in their MUS, BU_RECT ids , i.e. the **render order** and visual layout | Per-MEU layout files attached to the MEU folder |
| **App / env** | Application-level preferences, window positions, recent files, ... | `AAAUser/` folder (the user-level copy), `default.pref_start`, etc. |

### 13.1. PRESET

A MEU that uses the PRESET selector exposes numbered slots. Slot **0** is special: it is the **automatic snapshot** of the current state, saved continuously. Slots 1, 2, ... are user-controlled:

- click on slot N → recall that snapshot
- Ctrl+click on slot N → store current state into slot N
- Ctrl+Alt+click on slot N → erase slot N
- Ctrl+Alt+Shift+click on a slot → save **all** presets at once

The number of slots a MEU exposes is controlled by `meu:get_preset_nb()` (override on the proto).

Files are `.plua` (Lua-table-as-text). Renaming a BU or BUI between save and reload breaks the lookup for that entry , another reason BU names should be picked carefully (section 10.3).

### 13.2. BU position save

The visual layout of MUs in a MUS is the spatial encoding of execution order (section 7). It is persisted via the BU position save mechanism, distinct from PRESET. Files are written next to the MEU. Moving a MU and saving the app commits the new order.

### 13.3. App / env

Application-level state lives in the user-side `AAAUser/` folder (the one the installer copies at the same level as the AAASeed exe folder, see section 4.1). The `Dev/AAAUser/` copy in the kernel is the init/fallback. Hooks `aaa.hook_env_load_*` and `aaa.hook_env_save_*` run on every env load/save; use them sparingly and only for genuinely application-wide concerns.

## 14. Conventions

A short list of conventions to follow when contributing code or content.

### 14.1. Language

All source files (Lua, C++ in the build repo), all documentation files (`.lua` API docs, `.md`, every `CLAUDE.md`) are written in **English**. Conversational chat with the project owner can stay in French. The English-only rule keeps the project portable for any future collaborator or tool.

### 14.2. Naming

- **MEU instance names**: `<type>_<inst_key>`, split on the first underscore (see section 7.1). Allowed chars: alphanumerics, `_`, `.`, `-`. Match is case-insensitive.
- **BU names** in `define_ui`: short, English, snake_case for compounds, no whitespace. They are keys for save/load and external access (section 10.3).
- **Lua identifiers**: snake_case for variables and methods. Class names: UPPERCASE for GaBu classes (`MEU`, `BUS`, `SELECTOR`, ...), `MEU_<TYPE>` for MEU subclasses with `<TYPE>` upper.
- **Folder names** mirror the class name (`MEU_DIR/MEU_DIR.lua`, `BU_uif/BU_uif.lua`, ...).

### 14.3. Where to put a new prototype

- App-private (only for one app): `<APP>/AAA_PROTO/<type>/`
- Shared between sibling apps: parent dir's `AAA_PROTO/<type>/`
- Engine-shipped: `AAAKernel/AAA_PROTO/MEU_PROTO_<bucket>/<type>/` (bucket choice is conventional, often by author)
- Ad-hoc, no proto needed: just an instance folder, the engine's self-proto fallback (section 9.2) takes care of it

### 14.4. File encoding

UTF-8 throughout. Source files must round-trip cleanly through Lua compilation; avoid BOM unless a tool you depend on requires it.

## 15. Common traps

A growing list of things that have bitten people, kept here to save you the same evening.

- **Empty method does NOT remove a method.** Deleting a `function meu:foo() end` line and reloading leaves the previous `foo` definition on the proto. To actually drop it: `proto.foo = nil`, or recreate the proto. (Section 6.)
- **`init()` is not re-run on plain hot reload.** Live instances keep their old `init` results. Use the **Def** button (`redefine_ui`) to free + recreate the MEU and re-run `init()` then `define_ui()`. (Section 9.7.)
- **MEU init order in a MUS is non-deterministic.** Do not assume sibling MEUs are initialized when your `init()` runs. If you need cross-MEU references, resolve them lazily in `update()` or via `get_meu_by_name_cached`.
- **Renaming a BU breaks existing presets.** PRESET files reference BUs by name; renaming after a save invalidates the lookup for that entry. Pick names you can live with from the start.
- **The instance key is functional for some types.** `Fbo_F2` uses `F2` as the FBO name; `Ref_Foo_1` references the MEU named `Foo_1`. Don't pick instance keys arbitrarily for these types. (Section 10.3.)
- **`bus_cur` is a global**: forgetting to `bus:push()` / `bus:pop()` around a UI build leaks the wrong `bus_cur` to surrounding code. Use the bracket helpers (`init_begin` / `init_end`, `push_for_change` / `pop_for_change`) to keep this clean.
- **Differentiate MU from MEU.** A MU is the visual handle (a BU subclass), the MEU is the functional unit. You move/group MUs; you read/write state on MEUs. `mu:get_meu()` and `meu:get_mu()` cross between them.
- **`set_value*` bypasses undo; `set_value*_ui` records it.** Pick the right one depending on whether the change should be in the user's undo stack. (Section 11.1.)
- **VS Code search picks up `.claude/`** unless excluded. Add `**/.claude` to `search.exclude` in your workspace settings.

## 16. Further reading

- **High-level overview** of AAASeed (audience: anyone, not just devs): `AAADoc/AAASeed_overview.md`.
- **API reference** for the AAASeed Lua VM (`aaa.*`, `gol.*`, helpers, GABU_OBJ, SHADING, ...): `AAADoc/lua_aaaseed_*.md` , generated from `lua_aaaseed_*.lua`. To refresh, run `bash AAADoc/tools/regen_all.sh`.
- **MEU/MU naming, labels, find-by-name, cached lookups**: `AAADoc/MEU_and_MU.md` (end-user-oriented).
- **Agent-oriented catalogs** (file paths, class roles, conventions distilled): `AAAKernel/CLAUDE.md`, `AAAKernel/GaBu/CLAUDE.md`, `AAAKernel/GaBu/Gabu_BU/CLAUDE.md`, `AAADoc/CLAUDE.md`.
- **Code to read first** when extending the engine:
  - `AAAKernel/GaBu/Gabu_BU/MUS/MUS.lua` , `MUS:create_mu()` (instantiation flow).
  - `AAAKernel/GaBu/Gabu_BU/MEU/MEU.lua` , `MEU:create()`, `MEU:__init_new()` (proto vs instance vs self-proto).
  - `AAAKernel/GaBu/Gabu_BU/MEU_add/MEU_add.lua` , the full menu of UI builders.
  - `AAAKernel/GaBu/Gabu_BU/BALUE/BALUE.lua` , target / value semantics.
  - `AAAKernel/GaBu/Gabu_BU/MEU_bu/MEU_bu.lua` , MEU helpers for accessing BUs.
- **Build repo** (C++ side, sibling repo `AAASeed_EXE`): for `c_lua_wrap`, the `param.*` bridge, the `c_obj_ui` model, and how Lua bindings are registered (`Src/*_lua.cpp`).
