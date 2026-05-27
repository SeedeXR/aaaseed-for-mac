# AAASeed engine substrate

The C++ engine layer that GaBuZoMeu, MEUs and Lua scripts build on top of. This document explains, from a script author's point of view, what the engine guarantees, what the four fixed nesting levels mean, and where script work plugs in.

It is the script-side companion of the engine-side architecture doc `AAASeed_render_graph.md` in the sibling build repo `AAASeed_EXE/doc/`. For the high-level user-facing introduction see `AAASeed_system_overview.md`. For the GaBuZoMeu glossary (GA, BU, MEU, MU, APP, ...) see `MEU_and_MU.md`.

---

## 1. The `c_obj_ui` foundation

Everything in the engine is a `c_obj_ui`. From Lua / GaBu, an object exposes:

- **Params** — typed control / output cells, the object's public interface. A param can be an int, float, bool, color, file path, symbolic enum, etc. Reachable from Lua through `aaa.param.*`. The same params are what the UI panels show, what save / load files preserve, and what traxs can plug in or out on.
- Two engine-side methods called per frame:
  - **`update()`** — refresh internal state, prepare for the next draw.
  - **`draw()`** — execute as fast as possible. May be called several times per frame (for multi-viewport, multi-screen, multi-pass). `draw()` is almost a misnomer: a `c_obj_ui` may do something other than rendering (a sensor tracker, a network sender, ...). `execute()` would be more accurate but `draw()` is the historical name.

Anything you reach from a script — a layer, a camera, a light, a BDD, a value — is a `c_obj_ui`. Once you have it, you read its params, you set its params, and the engine handles the rest.

---

## 2. The four fixed engine levels

The render graph has a **fixed depth of four**. This is by design:
- Performance: the render loop walks a known structure each frame, no recursive descent.
- Predictability: each level has a clear, distinct role.
- Tooling: UI panels, save / load, MIDI mappings target each level individually because they are stable.

```
c_modules                       (top-of-engine, with main singleton-like reference)
 └─ c_module                    (group of layers + lights + materials + multi-screen)
     └─ c_layers                (sequence of up to 26 layers + lua + cameras + values + traxs)
         └─ c_layer             (one BDD + a full attribute system)
             └─ c_bdd           (the actual drawer)
```

Above the four sits **`c_app`** (the absolute top, exposed to scripts as `g_app`).

What each level adds:

### `c_layer`

The smallest meaningful render unit. Encapsulates one **`c_bdd`** (the actual drawer: a geometry, an image, a particle system, a sensor input, a file source, ...). The full vocabulary of `c_bdd` subclasses is what gives a single layer its expressive range.

Around the `c_bdd`, the layer carries an **attribute system**: a fixed set of slots for color, render state, fog, model, deformer, mapping, texturing, shading, stencil, clip, video texture, animated texture, multiple, lights switch, transforms, ... Each slot is in one of three modes:
- **current** — use whatever is currently active in the engine.
- **owner** — install this layer's own version of the attribute, overriding the current.
- **none** — the attribute is disabled for this draw.

The layer updates the requested attributes before its `c_bdd` draws, then restores them afterward.

### `c_layers`

A sequence of up to **26 layers**, addressed by a single letter from `a` to `z`. Often enough to encapsulate a whole effect, given how rich the `c_bdd` vocabulary is.

A `c_layers` adds four kinds of state on top of the layer sequence:

1. **A Lua wrapper** (`c_lua_wrap`). Points to a Lua script (a `.lua` file or an inline text block) and exposes options on how to use it (run / stop, one-shot trigger, file watch and auto-reload, manual reload, run-only-when-compiled, skip-rest, edit). The script is executed first, before any layer draws. This is what makes the layers behaviour dynamic.
2. **Cameras** (`c_cameras`). Up to 16 cameras, the active camera is selectable per layer.
3. **A values block** (`c_obj_value`). Generic typed placeholders, named slots used by Lua and by traxs as scratch / shared state.
4. **A traxs collection** (`c_traxs`). The older animation-driver mechanism: a trax can be plugged in or out on any param to drive its value over time (sine, random, MIDI, envelope, ...). Lua scripting now does most of what traxs used to do, but traxs remain because they are still the right tool for time-line replay, MIDI shows and clean parameter automation.

### `c_module`

A group of `c_layers`, with its own lua / cameras / values / traxs (same shape as `c_layers`, scoped one level higher), AND on top:
- **Lights** (`c_lights`).
- **Materials** (`c_materials`), optional.
- **Multi-screen handling** (`c_multi_screen`).

Reach for a `c_module` when a single `c_layers` is not enough — typically when the effect needs its own lighting setup or material pool.

### `c_modules`

Top of the engine. Groups modules. Adds yet another level of lua / values / traxs at the program-wide top. There is normally a single live `c_modules`, referenced through `c_modules::main`.

---

## 3. Where GaBuZoMeu plugs in

GaBuZoMeu's user-facing units (the **MEUs**, *Module Editable Units*) are not engine objects in their own right. A MEU is built on top of either:

- **a `c_layers`** — the common case, when the effect fits in one layers (a Lua-wrapped sequence of BDDs is enough).
- **a `c_module`** — when the effect needs its own lights or materials.

Once an effect is wrapped as a MEU, the underlying `c_layers` / `c_module` is **no longer a direct branch of the engine graph**. GaBuZoMeu manages it as part of a higher-level arborescence: a tree of MEUs, with sub-MEUs, named entries, save / load, presets, time-line orchestration, BU / MU views, etc.

The engine still draws each `c_layers` / `c_module` as a flat unit in its 4-deep graph. **How** MEUs are organised, named, persisted and combined is a Lua-side concern.

In one sentence:
- The **engine** is **static-depth for performance**.
- **GaBuZoMeu** is **arbitrary-depth for composition**.

This split is the central design choice that lets AAASeed be both real-time-fast at the engine level and infinitely composable at the user level.

---

## 4. Vocabulary cheat-sheet (script-side angle)

| Term | Lua-side perspective |
|---|---|
| `c_obj_ui` | Anything reachable through `aaa.obj.*`, `aaa.layer.*`, `aaa.module.*`, etc. The base of every named, param-bearing object. |
| **Param** | A typed cell on an object, accessible via `aaa.param.*`. The control surface from a script. |
| `c_bdd` | The actual drawer family, what a layer ends up rendering. Reach via `aaa.bdd.*` or via the layer's `get_bdd()`. |
| **Layer (`c_layer`)** | One `c_bdd` plus its attributes. Smallest render unit. |
| **Layers (`c_layers`)** | Up to 26 layers, plus lua / cameras / values / traxs. The most common MEU substrate. |
| **Module (`c_module`)** | A group of layers, plus lights / materials / multi-screen. The substrate for MEUs that need their own lighting or material pool. |
| **Modules (`c_modules`)** | Top-of-engine. Singleton-like (`main`). |
| **Trax** | An animation driver pluggable on a param. Older mechanism, complementary to Lua. |
| **Lua wrapper** | A `c_lua_wrap` attached at a level of the engine graph. Points to your script and controls when / how it runs. |
| **Lua script** | The `.lua` file (or inline text) the wrapper points to. |
| **Values** | A `c_obj_value` block, generic typed placeholders, scratch / shared state for Lua and traxs. |
| **MEU** | The user-facing unit on top of a `c_layers` or `c_module`. |
| **MU / BU / GA** | GaBuZoMeu-side concepts, see `MEU_and_MU.md`. |

---

## 5. To enrich

This document is a starting point. Future sections worth adding when the topics come up:

- **Params from the script side**: full tour of the `aaa.param.*` API surface, type semantics, animation hooks.
- **The attribute system seen from a MEU**: which attributes a MEU typically owns, which it leaves on current.
- **Trax model in detail**: the lifecycle of a plugged trax, how it interacts with Lua-driven param sets.
- **`c_bdd` family tour**: the main BDD subclasses and what each adds.
- **Performance budget**: what to expect at each engine level (per-frame cost of a layer, of a layers's lua, of a module).
- **GaBuZoMeu integration patterns**: idiomatic ways to expose an engine object as a BU, to bind a param to a slider, to drive a layer from a tracker.

Add sections here as the needs arise.

---

## See also

- `AAASeed_EXE/doc/AAASeed_render_graph.md` — the engine architecture from the C++ side. Same content, different audience and angle.
- `AAASeed_system_overview.md` — high-level user-facing introduction to AAASeed.
- `MEU_and_MU.md` — glossary of GaBuZoMeu concepts (GA, BU, BUS, MEU, MU, MUS, APP).
- `AAASeed_design_system.md` and `AAASeed_technical_documentation.md` — companion architecture docs in this folder.
- `lua_aaaseed_*.lua` and the generated `.md` views in this folder — the public Lua API reference.
