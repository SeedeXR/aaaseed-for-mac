# GaBu_BU , UI widget hierarchy + MEU system

The complete AAASeed interactive UI framework and MEU (Module Editable Unit) system.

## Class hierarchy

```
GABU_OBJ  (lua/GABU_OBJ.lua , base for everything)
└── BU    (BU/ , positioned, touchable UI element with color/text/transform)
    ├── BUI   (BUI/ , BU with one or more values via BALUE objects)
    │   ├── SLIDER       (SLIDER/ , float/int slider, range, dial mode)
    │   │   ├── SLIDER_TWO   (SLIDER_TWO/ , dual min/max slider)
    │   │   ├── SLIDER_XY    (SLIDER_XY/ , 2D XY slider)
    │   │   └── SLIDER_MULTI (SLIDER_MULTI/ , multi-value slider)
    │   ├── BUTTON       (BUTTON/ , bool toggle or value button)
    │   ├── SELECTOR     (SELECTOR/ , discrete item selector)
    │   │   └── PRESET   (PRESET/ , preset slot selector)
    │   └── BU_TEXT      (BU_TEXT/ , editable text input)
    ├── MU    (MU/ , Module Unit: handle pointing to a MEU; GaBuZoMeu layer)
    ├── BU_RECT     (BU_RECT/ , rectangle in a MUS, drawn under MUs)
    ├── BU_MEU      (BU_MEU/ , MEU inspector)
    ├── BU_WINDOW   (BU_WINDOW/ , floating window)
    │   └── BU_WINDOW_LIST  (BU_WINDOW_LIST/ , scrollable list window)
    │       └── BU_CREATE_MEU (BU_CREATE_MEU/ , runtime MEU instantiation window)
    ├── BU_DOC      (BU_DOC/ , doc display)
    ├── BU_BANK     (BU_BANK/ , bank/library browser)
    ├── BU_MONITOR  (BU_MONITOR/ , monitor / dashboard widget)
    └── BU_WIZ      (BU_WIZ/ , collection of widget BUs: BU_ALIVE, BU_EYE, BU_PB, ...)

BUS    (BUS/ , container for BU elements, coordinate system [-.5, .5])
└── BUSS    (BUSS/ , BUS group; filters its children by is_active() for update/draw)

BUS_CTX (BUS_CTX/ , container of BUSS; manages a "current" group via BUS_CTX.cur)

BLOB / BLOBS  (touch contacts and their collection; native multitouch dispatched by GA)

MUS    (MUS/ , container of MU; what BUS is to BU. A MEU_DIR instance owns a sub-MUS.)

MEU    (MEU/ + MEU_*/ , Module Editable Unit, atomic functional/rendering block. GaBuZoMeu.)

APP    (APP/ , base class for AAASeed applications. GaBu layer.)
└── APP_GP  (APP_GP/ , APP variant hosting MEUs. GaBuZoMeu layer.)

GA     (GA/ , Global Action: dispatch + update/draw orchestration)
GP     (GP/ , Garden Party: rendering loop, render index, root MUS holder)
```

## BU / BUI

**BU** (`BU/bus.lua`, `BU_pos/`, `BU_draw/`, `BU_uif/`, ...):
- Position/size/angle in BUS space `[-0.5, +0.5]`
- Touch/mouse contacts: `add_contact`, `remove_contact`, `update`, `do_click_up/down`
- Callbacks: `set_function_on_click(fn)`, `set_method_on_click(obj, method)`, similarly for `click_double`, `click_triple`, `click_long`
- Visibility: `set_visible(b)`, `set_active(b)`, `set_mobile(b)`
- Color: `set_color_back(name_or_rgba)`, named colors: `"bui"`, `"slider"`, `"x"`, `"y"`, `"z"`, `"restart"`, `"save"`, `"lua"`, `"focus"`, etc.
- Text: `set_text(str)`, `push_text(str)` / `pop_text()`, `set_text_visible(b)`
- Drop system: `set_drop_receiver(receiver)`, `drop_on(target, x, y)`
- Tree navigation: `get_bus_up()`, `get_bu_up()`, `get_meu_up()`

**BUI** is autonomous: it holds and edits one or more values of its own through **BALUE** objects (one per value dimension), held in `self.__balues` (a Lua table; `BUI:__get_balues()` accessor). Each BALUE's value source is either internal (the default, BALUE stores it) or bound to an external **target**.
- Single BALUE by id: `set_value(v, id)`, `get_value(id)`, `set_min_max(min, max, id)`, `set_value_type_integer(b, id)`, `set_value_type_bool(b, id)`, `flip_value(id)`, `get_value_as_bool(id)`. Canonical-form variants in `[0,1]`: `get_value_cano(id)` / `set_value_cano(v, id)`.
- Whole collection: `set_values(value_or_table)`, `get_values()` (returns single value if arity is 1, else a table).
- **Undo**: plain `set_value*` paths bypass the undo stack; the `_ui` variants (`set_value_ui`, `set_values_ui`, `add_value_ui`, `flip_value_ui`) record the change for undo. Use `_ui` for changes that come from user interaction or that should be undoable.
- **Target (optional binding to an external source)**: `set_target_lua(table, key, val, id)` binds to a Lua-table field; `set_target_param(param_ref, id)` binds to a c_param (a direct one-to-one wire between one BUI and one c_param). More target kinds may be added (model is open). When set, BALUE and target stay in sync; multiple BALUEs can share one target. Targets are not required. **Note**: the general-purpose way to interact with the C++ side is the Lua references-and-functions API (`aaa.obj.get_*`, `param.get_ref`, `param.get`, `param.set`, ...) , `target_param` is just a convenience for the direct-wire case. See `BALUE.lua`.
- Change callback: `set_function_on_value_change(fn)`, `set_method_on_value_change(obj, method)`
- Presets: `set_preset_use(b)`
- MIDI: `set_midi(ch, ctl)`
- Menu (dropdown): `set_menu(tab_or_selector [, name, val])` → returns `BU_MENU`

## BUS

Container holding BU elements in a local coordinate space `[-0.5, +0.5]`.
- `BUS:create(name)` , create a BUS
- `bus:init_begin()` / `bus:init_end()` , bracket BU creation
- `bus:create_add_bu(class, name, rect, ...)` , add a typed BU
- `bus:add_slider(rect, name, ...)` , convenience adder methods (and many others, see `BUS_add/`)
- `bus_cur` global , current active BUS; automatically set by `bus:push()` / `bus:pop()`
- `bus:find_bu_by_name_lowercase(bus_name, bu_name)` , navigate the tree
- BU groups: `bus:begin_bu_group(name)` / `bus:end_bu_group()`

`BUSS` extends BUS by filtering its children by `is_active()` for update/draw (via `update_bus_active` and `apply_bus_active`).

`BUS_CTX` aggregates several BUSS and exposes a "current" via `BUS_CTX.cur`. Walked top-down by GA's update/draw orchestration.

## MEU system , files by concern

| Directory | File(s) | Concern |
|---|---|---|
| `MEU/` | `MEU.lua` | Core class. `MEU:create(obj, type, key, label, dir)` and `__init_new()` decide between **isolated proto / self-proto / pure instance** (the third uses `setmetatable(self, {__index=proto})` for prototype-based method inheritance). `free()`, `save()`, `get_dir_absolute()`, `add_script()`, `set_focus()`. `self.ref.__obj_main`, `self.ref.__layers_main`. |
| `MEU_ui/` | `MEU_ui.lua` | Tab system (`set_tab_key`, `__do_tab_value_changed`), `register_update_ui(method)`, `unregister_update_ui()`, `__define_bus()`, fixed bar (Save/Load/Lua/Close/FBO buttons). |
| `MEU_add/` | `MEU_add.lua` | **All `add_*` UI builder methods** (used inside `define_ui`). Coordinate conversion `{ix,iy,nx,ny}` → BUS rect via `make_rect_from_irect()`. |
| `MEU_add_texture/` | `MEU_add_texture.lua` | `add_bu_texture_target_unit`, `add_bu_texture_target_unit_nb`, `add_bu_texture`, `get_texture_bind_2d(id)`, `set_texture_bind_2d(id,bind)`, `bind_texture_to_unit(id,unit)`. |
| `MEU_add_transfo/` | `MEU_add_transfo.lua` | `add_transfo(rect,nb)`, `add_transfo_tab(rect,nb)`, `__update_transfo(transfo)`. Wraps `TRANSFO_TRS`. |
| `MEU_shader/` | `MEU_shader.lua` | `add_shading(layer_id,name)`, `get_shading(id)`, `set_shading_cur(id)`, `add_shading_ui`, `add_shading_ui_multiple`, `add_shading_sliders`, `add_shading_sliders_tab`, `add_shading_sliders_type`. |
| `MEU_camera/` | `MEU_camera.lua` | `add_camera(rect,nb,b_force_no)`, `get_camera(index)`, `get_camera_used()`, `get_camera_sel()`. |
| `MEU_fbo/` | `MEU_fbo.lua` | `get_fbo()`, `get_meu_fbo()`, `set_meu_fbo(inst_key)`, `do_fbo(fn)`, `do_fbo_flipflop(b_verbose)`, `__build_ref_fbo()`. |
| `MEU_render/` | `MEU_render.lua` | `render(alpha)` , engine entry point. `draw_layers_begin()`, `draw_layer(n)`, `draw_layer_two(id1,id2)`, `draw_layers_end()`, `draw_layers()`, `get_alpha()`, `set_alpha(v)`, `is_render()`, `is_rendered_in_this_frame()`. |
| `MEU_time/` | `MEU_time.lua` | `define_time(rect,max,b_save)` → iy, `update_time()`, `restart_time()`, `time_play(b)`, `set_time_max(v)`, `set_time_speed(v)`, `set_time_looping(b)`, `is_time_looping()`, `update_time_ui()`. |
| `MEU_pixel/` | `MEU_pixel.lua` | `set_pixel_size(sx,sy)`, `get_pixel_size()`, `get_pixel_channel_type()`, `set_pixel_channel_type(ch_nb,ch_type)`, `add_pixel_size(rect,obj)`, `add_pixel_format(rect,obj)`. |
| `MEU_draw/` | `MEU_draw.lua` | Icon drawing: `draw_icon()`, `draw_icon_texture(bind,size)`, `draw_icon_text(text,fx)`, `draw_icon_camera()`, `draw_icon_phase(phase)`, `set_icon_color()`. |
| `MEU_REF/` | , | `self.ref` table population: `__obj_main`, `__layers_main`, `__module_main`, `rendering`, `mapping`, `trs`, `layer[n].obj`, `fbo`, `shading_stuff`, `color`. |
| `MEU_CTX/` | , | `MEU_CTX.cur` , current MEU context (the loaded MEU set, proto registry by type). |
| `MEU_DIR/` | , | Directory MEU type. `MEU.split_meu_type_inst(name)`, `MEU.build_inst_name(type,inst)`. A MEU_DIR instance owns a sub-MUS. |
| `MEU_bu/` | `MEU_bu.lua` | MEU helpers to address a BU by **key** (case-insensitive name lookup): `get_bu_by_key(key)`, `set_bu_value(key, val, id)`, `get_bu_value(key, id)`, `set_bu_values(key, values)`, `get_bu_value_as_bool(key, id)`, `get_bu_item_data(key, id)`. Idiomatic alternative to `bus:find_bu_by_name_lowercase(...)`. **Known gap**: no `MEU:get_bu_values(key)` symmetric to `set_bu_values`; use `meu:get_bu_by_key(key):get_values()` for now. |
| `MEU_about/` | , | MEU info/help display. |
| `MEU_exploitation/` | , | MEU preset/load/save management. |

## MEU coordinate system (UI grid)

All `add_*` methods take `irect = {ix, iy [, nx [, ny]]}`:
- `ix` = column, **1-based from left** (1 = leftmost)
- `iy` = row, **1-based from top** (1 = topmost, increases downward)
- `nx`, `ny` = width/height in grid cells (1 cell = 1/16 of BUS space)
- Grid is 16 × 16 cells; BUS space is `[-0.5, +0.5]` in both axes
- `iy` is auto-advanced after each `add_*` call by `ny`; track manually for multi-column layouts

## MEU standard methods

These are *methods* on the meu object (the proto, inherited by instances via `__index`). Not to be confused with `aaa.hook_*` / `EVENT.hook` (see Terminology below).

The engine sets `_G.meu = self` before calling any of these.

| Method | Role | Required |
|---|---|---|
| `define_meu_infos()` | returns `{author, date, tags, help, name_long}` | always (cleanliness) |
| `define_ui()` | builds the BUS via `self:add_*` (see `MEU_add.lua`). **BU names drive load/save and external access.** Called *before* MEU loading. | always |
| `init()` | first call at creation. Init variables, structures, C++ obj/param refs, costly ops. Called again on every `redefine_ui` (UI "Def" button). MEU init order in a MUS is **non deterministic**, do not assume other MEUs are present. | optional |
| `update()` | prepare for draw: set C++ params, prepare data. Anything you don't want to repeat inside `draw`. UI "**U**" button toggles its execution. | optional |
| `draw()` | actual rendering. If absent, the standard draw runs the C++ layers. If present, can draw via Lua, draw a single layer with custom attrs, loop with state changes, etc. UI "**D**" button toggles its execution. | optional |
| `update_ui()` | called only when MEU UI is visible. Refresh BU text, indicators, custom pre-draw ops. | optional |
| `draw_icon()` | custom icon drawing for the MU. | optional |
| `get_preset_nb()` | number of PRESET slots. | optional |

**Frame-level override**: `meu:render(alpha)` is the engine's per-MEU entry point each frame. By default it does `update()` then `draw()`; override it to break the pattern (skip `update`, call `draw` multiple times, swap params between draws, etc.). See `MEU_render/MEU_render.lua`.

**Hot reload nuance** : an empty method does NOT remove a previously defined one , the proto's method table accumulates across reloads. To actually remove a method, nil it from the proto table or recreate the proto. `init()` is NOT re-run on plain hot reload; use the **Def** button to free + recreate the MEU.

For the three implementation styles of a proto's `.lua` (`function meu:foo()`, `CLASS.DECLARE("MEU_<TYPE>", MEU)`, full class in `Gabu_BU/MEU_<TYPE>/`) and the proto search strategy (app dir + walk-up + kernel + MEU_DIR + self-proto fallback), see `AAAKernel/CLAUDE.md`.

## Persistence: three channels

Three independent serialization channels, each with its own files:

| Channel | What it stores | Where |
|---|---|---|
| **PRESET** | BU/BUI **values** of one MEU, in slots. Slot 0 is the auto-saved current state; slots 1+ are user snapshots (Ctrl+click=store, click=recall, Ctrl+Alt+click=erase, Ctrl+Alt+Shift+click=save all). | `<MEU folder>/preset_<n>.plua` |
| **BU position save** | Positions of MUs in their MUS, BU_RECT ids , i.e. the **render order** and visual layout. | Per-MEU layout files attached to the MEU folder. |
| **App / env** | Application-level prefs, window positions, recent files. Hooks: `aaa.hook_env_load_*`, `aaa.hook_env_save_*`. | User-side `AAAUser/` folder (installed at the same level as the AAASeed exe folder). |

## APP and APP_GP

**`APP`** (`APP/APP.lua`) is the GaBu-layer base class for all AAASeed applications.
- Declared with `APP.DECLARE("MyApp", APP, {...})` (registers with `APP_FACTORY`)
- Created with `APP.CREATE_INST(MyApp, "name")` , only one app can be open at a time
- `app` global is set after creation
- Key methods: `init_app()` (override), `get_dir_absolute()`, `create_obj_by_cid(cid)`, `get_meu_by_name(name)`, `get_meu_by_name_cached(name)`, `register_for_midi_update(bui)`, `unregister_for_target_update(bui)`

**`APP_GP`** (`APP_GP/APP_GP.lua`) is the **GaBuZoMeu-layer** APP variant that hosts MEUs. It carries among other things:
- `GP` (Garden Party) , holds the **root MUS** of the app and the rendering loop
- `BU_CREATE_MEU` , runtime MEU instantiation window
- `BU_DOC` , doc display
- `BU_BANK` , bank / library browser
- `BU_WIZ` widgets (`BU_ALIVE`, `BU_EYE`, `BU_PB`, `BU_MEMORY`, `BU_SHOW`, `BU_FPS`, `BU_MAAEB`, `BU_MESS`, ...)
- `BU_MONITOR` , dashboard widget(s)

## Other notable classes

| Directory | Class | Description |
|---|---|---|
| `BALUE/` | `BALUE` | Holds one scalar value for a BUI. Min/max/def/ina, canonical form `[0,1]`, targets (param or Lua table). When a target is set it is the source of truth; multiple BALUEs can share a target. |
| `GA/` | `GA` | Global Action , high-level dispatch (keyboard, BLOB/multitouch, update/draw orchestration), undo/redo, time settings. |
| `GP/` | `GP` | **Garden Party** (live coding / VJ origin) , rendering loop, render index, spy profiling, holds the root MUS. |
| `MU/` | `MU` | Module Unit , derives from BU, points to its MEU. Visual handle in the scene. |
| `MUS/` | `MUS` | Collection of MUs forming a rendering scene. What BUS is to BU. |
| `BUS_add/` | , | `bus:add_slider`, `bus:add_button`, `bus:add_selector`, `bus:add_trig` etc. , convenience adders on BUS. |
| `LUA_WRAP/` | `LUA_WRAP` | Lua handle on top of `c_lua_wrap` (the C++ object that watches a `.lua` file and triggers hot reload). |
| `UNDO_REDO/` | `UNDO_REDO` | Undo/redo stack. Captures BUI value changes and BU position changes (exact scope to refine). |
| `EVENT/` | `EVENT` | Event dispatch system. `EVENT.hook(e_type, e_sub_type, a,b,c,d,e)` is called from C++; activation flag in `lua_master`. |
| `SEQS/` | `SEQS` | Collection of `SEQ` (sequences). |
| `BU_WINDOW/` | `BU_WINDOW` | BU acting as a floating window (title bar, minimize, etc.). |
| `BU_WINDOW_LIST/` | `BU_WINDOW_LIST` | Scrollable list window (parent of `BU_CREATE_MEU`). |
| `BU_MENU/` | `BU_MENU` | Popup menu attached to a BUI. |
| `BU_TEXT/` | `BU_TEXT` | Editable text field BU. |
| `BU_TEXTURE/` | `BU_TEXTURE` | Texture-displaying BU. |
| `BU_COLOR/` | `BU_COLOR` | Color-displaying BU. |
| `BU_OBJ/` | `BU_OBJ` | BU linked to a C++ object. |
| `BU_RECT/` | `BU_RECT` | Rectangle in a MUS, drawn **under** MUs. Its alpha multiplies the alpha of contained MU/MEU. Lets you move a group of MUs together via glue/unglue. |
| `BU_uif/` | `BU_uif` | UI Fast / star menu attached to each BU. Content varies by BU type. **Major area of upcoming work**, detailed doc deferred. |
| `BU_MEU/` | `BU_MEU` | MEU inspector. When activated, surfaces the MEU's BUS (generic Save/Load/Lua/Close/FBO bar + MEU-specific UI). |
| `BU_CREATE_MEU/` | `BU_CREATE_MEU` | Runtime MEU instantiation window. Descends from `BU_WINDOW_LIST/BU_WINDOW`. Held by `APP_GP`. |
| `BU_DOC/` | `BU_DOC` | Doc display window. |
| `BU_BANK/` | `BU_BANK` | Bank / library browser. |
| `BU_MONITOR/` | `BU_MONITOR` | Dashboard / monitor widget. |
| `BU_WIZ/` | `BU_*` family | Wizardly widgets (`BU_ALIVE`, `BU_EYE`, `BU_PB`, `BU_MEMORY`, `BU_SHOW`, `BU_FPS`, `BU_MAAEB`, `BU_MESS`, ...). |
| `DOMINO/` | `DOMINO` | Domino-style layout management. **Legacy from an old Arte app; slated to move to `GaBu_Util` or be removed.** |
| `PRESET/` | `PRESET` | Preset slot selector (SELECTOR subclass). Click=recall, Ctrl+click=store, Ctrl+Alt+click=erase, Ctrl+Alt+Shift+click=save all presets. |
| `SELECTOR/` | `SELECTOR` | Discrete item selector. `set_nb(n)`, `set_item_text(i,...)`, `set_item_data(i,...)`, `get_item_data()`. |
| `SLIDER_TWO/` | `SLIDER_TWO` | Dual-value slider (range). |
| `SLIDER_XY/` | `SLIDER_XY` | 2D XY pad slider. |

## Terminology: hooks vs methods

Two distinct concepts often conflated:

- **Method** , a Lua method on a class or object (looked up via `__index` in inheritance chains). MEU lifecycle entry points (`define_ui`, `init`, `update`, `draw`, ...) are **methods** , the engine calls them on the current `meu` object. Hot reload updates them on the proto's class table.
- **Hook** , a global Lua function with a conventional name that the C++ engine calls at specific lifecycle moments. Defined by the user (typically in `lua_master`) or kernel. Examples: `aaa.hook_env_load_before(filename)`, `aaa.hook_env_save_after(filename)`, `aaa.hook_main_loop_begin()`, `aaa.hook_quit(...)`, `aaa.flatland.hook_draw_info/focus(sx,sy)`, `aaa.menu.hook(id)`, `aaa.midi.hook_set_control(ch,ctl,val)`, `aaa.net.hook_receive_text8(str)`, `EVENT.hook(...)`.
