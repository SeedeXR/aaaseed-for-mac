# AAASeed -- Runtime Content Repo

This repository (`AAASeed`) contains the **runtime content** of the AAASeed engine: the Lua/AAA virtual machine kernel, the user APPs and MEUs, and the public API documentation. It is the companion of the **build repo** `AAASeed_EXE` which produces the C++ executable.

## What is AAASeed

AAASeed is a real-time C++/Lua/OpenGL visual engine developed by Mâa since 1996, running on Windows (Linux and Macintosh versions will come at some point). The C++ engine is an old school rendering graph exposing scene objects (cameras, lights, materials, geometry databases, textures, etc.) as attributes in a layer/layers/module/modules/app structure. Each object (c_obj_ui) has named parameters (c_param) that are like a remote control for it. Through params, objects are changed in the UI, loaded/saved, and can be driven from Lua scripts. A user-facing UI layer (**GaBu**, the Lua framework built on top of the C/Lua bridge) is fully written in Lua. **GaBuZoMeu = GaBu + MEU/MU/MUS** (the meta-mechanism for composing MEUs on top of GaBu). The runtime loads from this repo at startup; the executable is built from the sibling.

Two repos cooperate:

| Repo | Purpose |
|------|---------|
| **AAASeed_EXE** (build, sibling) | C++ source, MSBuild + CMake projects, produces the EXE. Contains `Src/*_lua.cpp` files that register C functions as Lua bindings. Has per-subsystem `CLAUDE.md` files under `Src/`. |
| **AAASeed** (this repo, runtime) | Kernel Lua scripts, GaBu UI framework, user APPs, MEUs (Module Editable Unit), and end-user API documentation. The EXE loads from here at startup. Has per-folder `CLAUDE.md` files (AAADoc, AAAKernel, GaBu, etc.). |

The two repos are independent (not git submodules); cross-references via `CLAUDE.md` files are the only formal link. Their actual filesystem locations are environment-specific -- ask the project owner when cloning fresh.

When debugging a runtime behavior, you typically need both: the C side defines what's registered, the Lua side defines how it's used. The doc files in `AAADoc/` describe the merged surface from a script author's point of view.

## Directory layout

```
AAA/AAASeed/
|
|-- AAADoc/                 End-user API documentation (lua_aaaseed_*.lua + .md)
|                           Three mixed C+Lua files (draw, interface, helpers) plus
|                           one file per large class wrapper (GABU_OBJ, SHADING).
|                           See AAADoc/CLAUDE.md for the .lua -> .md pipeline.
|
|-- AAAKernel/              Kernel content loaded by the engine at boot
|   |-- lua/                Core Lua glue: aaa_*.lua scripts that extend the C bindings
|   |                       (audio, draw, file, img, keyboard, layer, math, mouse,
|   |                        net, obj, param, screen, stereo, string, table, time,
|   |                        util, viewport, ...)
|   |                       See AAAKernel/lua/CLAUDE.md for layer details.
|   |-- GaBu/               GaBu Lua UI framework + GaBuZoMeu (MEU/MU/MUS layer)
|   |                       See AAAKernel/GaBu/CLAUDE.md.
|   |-- AAA_PROTO/          Built-in MEU prototypes
|   |-- Shader/             Built-in GLSL shaders
|   |-- Texture/            Built-in textures
|   |-- Fonts/              Font resources
|   |-- Module/             Module-level config + resources
|   |-- OpenCl/             OpenCL kernel files
|   |-- Dev/AAAUser/Guest/  Default user pref + lua_master config (defines which
|   |                       Lua files load at boot -- see default.lua_master.lua)
|   `-- (see AAAKernel/CLAUDE.md for kernel architecture)
|
|-- AAAAPPs/                User-level AAA applications (CUR/, ...)
|-- APPs_Maa/               Mâa's personal MEUs / projects
|
|-- AAASeed_Metal.exe       Built executable, copied here for end-user runs
|-- *.deproj / *.deuser     Decoda Lua editor project files
`-- ...                     Various tutorial / training / archive folders
```

## English-only rule

All source files (Lua, C side -- in the build repo --), all documentation files (.lua API docs, .md), and all CLAUDE.md files in both repos are written in English. Conversational chat with Mâa can stay in French. This rule keeps the project portable for any future collaborator or tool.

## Working in this repo vs the build repo

- **Edit `*_lua.cpp` (binding, in AAASeed_EXE)** when the change requires new C code, new fields, new SIMD paths, etc. After editing, rebuild the EXE there and copy it to the runtime repo (`AAASeed_Metal.exe` at the root of this repo).
- **Edit `aaa_*.lua` or `GaBu/*.lua` (in this repo)** when the change is pure-Lua: a new helper, a new MEU, a UI tweak, a script-level fix. No rebuild needed -- the EXE picks up the new Lua at next launch.
- **Edit `AAADoc/lua_aaaseed_*.lua` (in this repo)** when adding/renaming/removing an API entry. Then regenerate the `.md` view with `AAADoc/tools/regen_all.sh`. See `AAADoc/CLAUDE.md` for the section convention and pipeline.
- **For deeper context**: `AAADoc/AAASeed_dev_guide.md` (developer walkthrough of the runtime), `AAADoc/AAASeed_modifying_meus.md` (advanced-user guide on the Lua/c_layer hybrid model), and the per-folder `CLAUDE.md` files (agent-oriented catalogs).

## Lua binding conventions (cross-repo reference)

The C++ side (`*_lua.cpp` in AAASeed_EXE) registers functions into Lua tables. The patterns below are useful context when reading or extending the Lua scripts in this repo:

- `AAALUACALL(name) { ... }` declares a Lua-callable function on the C side.
- `ADD_FN(name)` inside a `register_*( lua_State* L )` block registers it under the current Lua table (opened with `l.define_table("X")` and closed with `l.pop(N)`).
- `add_fn_to_table("lua_name", cpp_fn)` registers under a different Lua name (used for C++ keywords, e.g. `delete` -> `aaa.obj.delete`).
- Macros that expand to `AAALUACALL` implicitly: `AAALUA_CALL_FN`, `AAALUA_CALL_FN_INT32`, `AAALUA_CALL_FN_REAL`, `AAALUA_CALL_FN_BOOL`, `AAALUA_CALL_FN_RETURN_INT32`, `AAALUA_CALL_FN_RETURN_REAL`, `COOR_CALL`, `DEFINE_COLOR_FN`, `FN_UPDATE_UNIFORM`, `ADD_FN_3D` (registers `name_3d` under Lua name `name`).

**Doc tag convention** in `AAADoc/lua_aaaseed_*.lua`: a trailing `-- lua` tag means the function is defined in `aaa_*.lua` scripts (pure-Lua); absence of the tag means it is a C binding from `*_lua.cpp`.

**`default.lua_master.lua`** (`AAAKernel/Dev/AAAUser/Guest/`) is the definitive list of Lua files loaded at runtime; useful when auditing the live API surface.

**Hot reload**: every Lua file in the runtime is loaded by a `c_lua_wrap` C++ object that watches it for changes and re-runs it according to its own params (`doit`, `run_only_when_compiled`, `skip_rest`, ...). See `AAAKernel/CLAUDE.md` for params and behavior.

**`PAIRS` / `IPAIRS`** (defined in `AAAKernel/lua/aaa_table.lua`): drop-in replacements for `pairs` / `ipairs` that accept `nil` as the table (return an empty iterator instead of erroring). Use them when iterating over a possibly-nil table without an explicit `if tab then` guard.

**Coordinate / size naming convention**: across the codebase, positions use `x`, `y`, `z` (not `cx`, `cy`, `cz`) and sizes use `sx`, `sy`, `sz` (not `width`, `height`, `w`, `h`). Many APIs return them grouped, e.g. `BU:get_xy_sxy()` returns `x, y, sx, sy`. Keep new variables and parameters in this style (e.g. `panel_x`, `panel_sy`, not `panel_cx`, `panel_h`).

**`gol.color_<name>([alpha])`**: many named-color helpers exist on the `gol` table (e.g. `gol.color_white`, `gol.color_black`, `gol.color_magenta`, `gol.color_cyan`, `gol.color_green`, `gol.color_red`, ...) and accept an optional alpha argument. Prefer them over `gol.color(r,g,b,a)` literals when a named color fits.

**`BU.__draw_text_line_width`**: the reference line width used across the GaBu UI/UIF rendering. Code that draws lines, frames or rect outlines on top of buttons/menus uses this constant with multipliers (`* .5`, `* .75`, `* 1`, `* 1.5`, `* 2`, `* 3`) instead of hardcoded pixel widths. Use the same pattern for consistent stroke weights across the UI.

**OO conventions**: classes are declared with `CLASS.DECLARE("Name", parent_class, defaults_table)`. Methods are added as `function CLASS:method(...) end`. Super calls use `oo.getsuper(CLASS).method(self, ...)`. Runtime type checks: `obj:is_class(C)` for exact class match, `obj:is_derived_from_class(C)` to also accept subclasses. This pattern is used everywhere (BU, BUI, MU, MEU, ...).

**Idempotent fill pattern**: helpers that populate a shared table (e.g. a `zones` dictionary for UIF) typically use `t.x = t.x or {...}` so a caller can pre-fill a variant first and the helper will leave it alone. Follow this pattern when writing new helpers that take an in/out table, so they compose with each other.

**`if false then ... end` is a preserved-draft idiom**: dead `if false` blocks (and the dead `else` branch of `if true then ... else ... end` toggles) are intentional drafts kept for reference or future revival. Do not delete them and do not propose simplifying the surrounding `if`. LSP warnings inside such blocks (undefined globals, missing fields, dead variables, etc.) are accepted noise.

**LSP setup**: the repo ships with `.luarc.json` at the root and meta stubs in `meta/aaa.lua` for `lua-language-server` (LuaLS). The meta stubs declare AAASeed extensions on top of the standard `math` / `table` / `string` libraries plus the C-injected `param` namespace, so LSP stops flagging them as `undefined-field`. To run the checker on a subfolder, pass the config explicitly:

```
lua-language-server --check <subfolder> --configpath=<repo>/.luarc.json --checklevel=Warning
```

without `--configpath`, the workspace defaults to the subfolder and the repo `.luarc.json` is not picked up.
