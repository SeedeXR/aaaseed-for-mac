# AAAKernel/GaBu , GaBu + GaBuZoMeu Lua framework

The Lua-based UI and application framework that runs on top of the AAASeed C++ kernel.

- **GaBu** = the UI framework + class system: dispatch (`GA`, `BLOB/BLOBS`), UI tree (`BU`/`BUS`/`BUSS`/`BUS_CTX`), application base (`APP`), plus standalone utility classes (`FBO`, `IMG`, `PHASOR`, ...).
- **GaBuZoMeu** = adds the `MEU` (Module Editable Unit) meta-mechanism on top of GaBu: `MEU`/`MU`/`MUS`/`MEU_DIR`/`APP_GP`. **Frontier between the two = MEU** (not APP, which lives in GaBu).

## Acronyms

| Name | Meaning | Notes |
|---|---|---|
| `GA` | Global Action | High-level dispatch: keyboard, BLOB/touch, update/draw orchestration. Singleton (`GA|SINGLETON`). |
| `BU` | Box User | Interactive UI element with position, color, touch/mouse handling. Base for SLIDER, BUTTON, SELECTOR, ... |
| `BUI` | Box User with values | BU subclass that holds one or more values, each in a `BALUE` object. |
| `BUS` | container of BU | Coordinate space `[-0.5, +0.5]`. A BU may itself contain a BUS, recursively. |
| `BUSS` | container of BUS | Filters its children by `is_active()` for update/draw. |
| `BUS_CTX` | container of BUSS | Manages a "current" group; `BUS_CTX.cur`. |
| `BLOB` / `BLOBS` | touch contacts / collection | Native multitouch dispatched top-down by `GA`. |
| `BALUE` | one scalar value of a BUI | Autonomous by default (stores its own value). Holds min/max/def, canonical `[0,1]` form. Optional **target** binds to an external source: `target_lua` (a Lua-table field) or `target_param` (a direct one-to-one wire to a c_param). More target kinds may be added. Targets are not required. Note: `target_param` is just one shape of C++ interaction; the general-purpose route is the Lua refs-and-functions API (`aaa.obj.*`, `param.get_ref`, `param.get`, `param.set`, ...). |
| `UIF` | UI Fast / star menu | `BU_uif/`, attached to each BU; content varies by BU type. Major upcoming work area. |
| `Zo` | unused | Shadoks reference (1968 series), will be back. |
| `MEU` | Module Editable Unit | GaBuZoMeu layer , atomic functional/rendering block. |
| `MU` | Module Unit | UI handle representing a MEU; derives from BU and points to its MEU. |
| `MUS` | container of MU | What BUS is to BU, MUS is to MU. A `MEU_DIR` instance owns a sub-MUS. |
| `GP` | **Garden Party** | Live coding / VJ culture origin. The `GP` object owns the **root MUS** and the rendering loop (render index, spy profiling). `APP_GP` is the APP variant that carries a `GP` and the runtime tools (`BU_CREATE_MEU`, `BU_DOC`, `BU_BANK`, `BU_WIZ`, `BU_MONITOR`). |

## Subdirectories

```
GaBu/
├── GaBu_Util/   Standalone utility classes , no UI dependencies
│                (FBO, IMG, PHASOR, TIMER, STACK, QUEUE, TEXT, VIDEO, CHANGER, ...)
│                Loading driven by GaBu_Util/default.layers_bind.
└── GaBu_BU/     UI widget hierarchy + MEU system
                 (BU, BUI, BUS, BUSS, BUS_CTX, SLIDER, BUTTON, SELECTOR, MEU, MU, MUS,
                  APP, APP_GP, GA, BLOB, ...)
                 Loading driven by Gabu_BU/default.layers_bind.
```

Each class lives in its own subfolder `<ClassName>/<class_name>.lua`. The `.lua` is loaded by a C++ `c_lua_wrap` that watches the file for hot reload.

## Loading order

Both `default.layers_bind` files are serialized C++ layer trees that pilot the load order. `GaBu_Util` loads before `GaBu_BU`. Within `GaBu_BU`, the conceptual flow is:

```
GABU_OBJ → BU → BUI → widgets (SLIDER/BUTTON/SELECTOR/...) → BUS → BUSS → BUS_CTX → GA → APP → MEU → MU → MUS → APP_GP
```

(Strict order is what the `layers_bind` says; the above mirrors the dependency graph.)

## Global variables of note

| Variable | Type | Description |
|---|---|---|
| `bus_cur` | BUS | Currently active BUS container during UI building and event dispatch. |
| `app` | APP | Running application instance (nil if no app loaded). |
| `ga` | GA | Global Action manager (singleton). |
| `meu` | MEU | Set by the engine to the current MEU before calling its Lua **methods** (`define_ui`, `init`, `update`, `draw`, ...). |
| `MEU_CTX.cur` | MEU_CTX | Current MEU context (active proto registry, etc.). |
| `CLASS` | table | Class registry , `CLASS.DECLARE()`, `CLASS.is_gabu_obj()`. |
| `GABU_OBJ` | class | Root base class for all GaBu objects. |

## Hot reload

Every class `.lua` is loaded by a `c_lua_wrap` that watches the file. On change, the script is re-compiled and re-executed. New method definitions update the class's method table, and live instances pick up the new methods immediately via Lua's `__index` metatable mechanism (used both for class inheritance and , in MEU's case , for instance-to-proto delegation).

**Caveat**: an empty method does NOT remove a previously defined one. The class's method table accumulates across reloads; to actually drop a method, nil it from the class table or recreate the class. See `AAAKernel/CLAUDE.md` MEU section for details.

## UIF (star menu) mechanics

The UIF menu of a BU is opened by `BU:begin_uif()` which calls `self:get_uif_zones()` to build a `zones` table (`{name -> def}`). Each def positions one zone relative to the radial center (the position where UIF opens):

- `angle`, `dist`, `rect={x, y, sx, sy}`: position in `UIF_DY` units. The effective dist factor is `(1 + def.dist) * UIF_DY`.
- `dangle`: makes the zone angular (a pie slice arc). Without `dangle`, the zone is rect-based and uses its `lrbt` for hit-test.
- `type`: defaults to a regular button. `"frame"` draws a rect outline (with optional `def.color`, `b_above` to render in the post-pass on top of buttons). `"panel"` draws a filled backdrop with magenta frame and a title strip (drawn above the rect) and is **never** returned as a hovered zone in `__find_zone_sel`.
- `bind_flat`: a texture image to render inside the rect (BU_TEXTURE pattern).
- `sub_def`: when the mouse hovers this zone, `define_uif_sub(sub_def)` is called to install a sub menu. Use `b_transient = true` for "auto-close on exit" subs (panel-style) or omit for persistent subs (BU_TEXTURE grid). `xy_sxy` defines the keep-alive bbox so the sub stays open in the gaps between sub buttons.

The active sub lives in `uif.sub`. While in sub, `uif.b_in_main = false` and `uif.b_sub_sel = true`; `uif.sub_parent_name` keeps the parent main zone name. `get_uif_name_sel(uif)` returns the parent name for command routing while `is_uif_name_sel(uif, name, b_sub)` keeps highlight scope correct (a main zone never highlights when the hover is in sub, and vice versa).

`__draw_uif_zones` runs three passes: a pre-pass for `type="panel"` backdrops, a main pass that draws non-panel/non-`b_above` zones sorted by area descending (smaller buttons end up on top of larger ones), and a post-pass for `type="frame"` with `b_above`. The drawing helpers `__set_color_uif_back()` (matches the radial back color) and `BU.__draw_text_line_width` (line width reference) keep the chrome consistent.

`BU:add_uif_zones_slide(zones, b_bool, dist_min_max, dist_slide)` is the single source of truth for `Slide / Min / Max / True / False` zone positions, called by both `BUI:add_uif_zones_base` and `MU:add_uif_zones_mu`. `BU:add_uif_zones_base(zones, dist, x, y)` adds the `Name / Values / Dump / Doc` row.
