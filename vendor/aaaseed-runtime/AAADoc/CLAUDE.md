# AAADoc -- AAASeed Public API Reference

This folder holds the end-user-facing documentation for the AAASeed Lua API. Files come in `.lua` / `.md` pairs:

| File pair | Topic | Type |
|-----------|-------|------|
| `lua_aaaseed_draw.lua` / `.md` | GOL (OpenGL isolation), draw, image, color, font | mixed C + Lua |
| `lua_aaaseed_interface.lua` / `.md` | Object model, params, BDD, networking, files, system | mixed C + Lua |
| `lua_aaaseed_helpers.lua` / `.md` | Pure-Lua extensions to math/string/table/array/format | mixed C + Lua |
| `lua_aaaseed_GABU_OBJ.lua` / `.md` | The `CLASS` registry and the `GABU_OBJ` base class | all-Lua |
| `lua_aaaseed_SHADING.lua` / `.md` | The `SHADING` class wrapping the C shader object | all-Lua |

The `.lua` files are the **source of truth**. The `.md` files are **generated artifacts** -- do not edit them by hand, your edits will be overwritten on next regen.

**Per-class doc convention**: when a class is large enough to warrant its own file (GABU_OBJ, SHADING), give it `lua_aaaseed_<CLASSNAME>.lua` with the class name in uppercase preserved in the filename. The generator (`tools/lua_to_md.pl`) preserves uppercase in the derived title when the basename has any uppercase letter; otherwise it falls back to `ucfirst(lc(...))` so `draw` -> `Draw`.

## Section convention (Option C)

Inside the `.lua` doc files, sections are marked using a markdown-flavored prefix. The number of `#` after `--` directly maps to the heading depth:

```
-- # SECTION                 -- level 1 (top), maps to MD `## 1. SECTION`
-- ## sub-section            -- level 2,        maps to MD `### 1.1. sub-section`
-- ### sub-sub-section       -- level 3,        maps to MD `#### 1.1.1. sub-sub-section`
```

The triplet decoration may be retained for visual readability:

```
--
--	# SECTION
--
```

Free comments (no `#` prefix) are kept as `-- ...` lines and become MD prose paragraphs above the next code block. Hard line breaks (paragraph-internal newlines) and paragraph breaks (separator `--`) are preserved in the generated MD.

## Tag convention

In **mixed C + Lua** files (`draw`, `interface`, `helpers`):
a trailing `-- lua` tag at the end of a function entry means it is defined in an `aaa_*.lua` script (pure-Lua); absence of tag means the function is registered by a C binding in the build repo.

```lua
	aaa.bdd.set_color( index, r, g, b, a )                              -- C binding (no tag)
	aaa.time.init()                                 -- lua	resets the lua time state
```

In **all-Lua** files (`GABU_OBJ`, `SHADING`, future class-specific files), the `-- lua` tag is dropped entirely since it would just be noise on every line. The preamble of those files states explicitly: "All entries in this file are pure Lua (no C bindings)."

## Placeholder convention

When describing a family of methods that share a naming pattern (e.g. `set_vert_float`, `set_geom_float`, `set_frag_float`, `set_comp_float`), use **UPPERCASE placeholders** in the doc:

```lua
	self:set_STAGE_float( id, val )         -- 4 methods (STAGE in vert/geom/frag/comp)
	self:set_STAGE_vec4_AXIS( id, v )       -- 16 methods (AXIS in x/y/z/w)
```

Do NOT use `<stage>` / `<axis>` style: angle-bracketed lowercase tokens get parsed as HTML tags by markdown renderers and disappear in the rendered `.md`. UPPERCASE stays untouched and is visually distinct from real Lua identifiers (which are snake_case lowercase).

A `## NOTATION` sub-section at the top of an all-Lua doc file should explain which placeholders are used and what they substitute to.

## Coverage status

The 5 doc files cover:
- C bindings registered in `AAASeed_EXE/Src/*_lua.cpp`
- Pure-Lua functions in every `AAAKernel/lua/aaa_*.lua` script
- The 8 small/medium top-level helper files: `gol_util`, `vector_2d`, `vector_3d`, `RECT`, `csv_util`, `aaa_jit`, `aaa_jit_dump`, `aaa_power`
- The class wrappers `CLASS` + `GABU_OBJ` (own file) and `SHADING` (own file)

Not yet covered:
- Class wrappers `TRANSFO.lua` (TRANSFO_TRS / TRANSFO_THREE) and `OPENCL.lua` -- each will get its own `lua_aaaseed_<CLASS>.lua` when documented.
- Scripts under `AAAKernel/GaBu/` subfolders (most are MEU/APP project content rather than reusable API).

Use `AAAKernel/Dev/AAAUser/Guest/default.lua_master.lua` to enumerate the full set of Lua files loaded at runtime when extending coverage.

## What NOT to document

- **iOS / `b_ios` branch**: the `aaa.b_ios` reduced-VM branch in `AAAKernel/lua/*` is no longer used. Ignore everything inside `if aaa.b_ios then ...` and inside `else` clauses of `if not aaa.b_ios then ... else ... end` constructs. Only the desktop branch (`b_ios = false`) is the real runtime, so document only that.
- **Deprecated entries**: when a Lua function is marked deprecated in the source (`-- deprecated:` comment block above the family), drop it from the API doc. Existing example: `set_STAGE_bind` and `get_ref_STAGE_bind` in SHADING.

## Generation pipeline

The `tools/` subfolder contains the scripts that turn `.lua` -> `.md`:

| Script | Role |
|--------|------|
| `tools/lua_to_md.pl` | Main generator. Parses Option C markers, emits MD with TOC + scientific numbering + fenced `lua` code blocks. Preserves uppercase in the derived title when the basename has uppercase letters (so `lua_aaaseed_GABU_OBJ.lua` -> title `GABU_OBJ`). |
| `tools/section_promote.pl` | One-shot helper. Promotes legacy markers (triplet + bare `-- NAME` UPPERCASE) to Option C. Idempotent. |
| `tools/regen_all.sh` | Wrapper. Regenerates every `.md` file in one call. Add new doc files to its loop when introducing them. |

### Usage

After editing any `.lua` doc:

```sh
cd /path/to/AAASeed/AAADoc
./tools/regen_all.sh
```

Or one file:

```sh
perl tools/lua_to_md.pl lua_aaaseed_draw.lua lua_aaaseed_draw.md
```

## When to update what

- New C binding registered in the sibling build repo (`AAASeed_EXE/Src/*_lua.cpp`): add an entry in the matching mixed `.lua` doc here (no `-- lua` tag), regenerate the `.md`.
- New pure-Lua helper in `AAAKernel/lua/aaa_*.lua`: add an entry with `-- lua` tag in the mixed doc, regenerate.
- New class declared in `AAAKernel/lua/<CLASS>.lua` and large enough to need its own doc: create `lua_aaaseed_<CLASS>.lua` (uppercase preserved), add it to `tools/regen_all.sh`, no `-- lua` tag inside (all-Lua file), regenerate.
- Renaming or removing a binding/helper: update the doc entry accordingly, regenerate.
- Sub-grouping change: edit/move/add `-- ##` / `-- ###` markers in the `.lua` source, regenerate.

## English-only rule

All `.lua` doc files, generated `.md` files, and this `CLAUDE.md` are written in English. Conversational chat with Mâa stays in French.
