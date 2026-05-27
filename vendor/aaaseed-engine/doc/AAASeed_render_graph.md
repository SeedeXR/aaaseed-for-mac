# AAASeed Render Graph

A walkthrough of the structural backbone of an AAASeed scene: the four nesting levels of the render graph (`c_layer` → `c_layers` → `c_module` → `c_modules`), what each level brings, and how the application-side scripting layer builds on top of them.

This document is a high-level guide. For implementation details (private fields, file layout, attribute push/pop stacks, code conventions), see `Src/infrastructure/layer/CLAUDE.md`.

---

## 1. The `c_obj_ui` foundation

Every object in the AAASeed engine inherits from `c_obj_ui`. From a user's perspective, a `c_obj_ui` exposes:

- **Params** — its public interface, the visible tip of the iceberg. Params are typed cells (int, float, bool, color, file path, symbolic enum, etc.) used both as control inputs (set from the UI, from a Lua script, or from an animation driver) and as output information (read back to know the object's current state).
- Two key behavioural methods, called by the render loop:
  - **`update()`** — refresh internal state and prepare for the next draw. Called once per frame.
  - **`draw()`** — execute as fast as possible. May be called several times in a row on the same object (for example when the same object is replayed across multiple viewports or multi-screen tiles).

`draw()` is almost a misnomer: a `c_obj_ui` may perform actions other than rendering (for example a sensor tracker, or a network sender). `execute()` would describe the role more accurately, but `draw()` is the historical name and AAASeed keeps it.

Everything that follows is built on this `params + update + draw` triplet.

---

## 2. The four nesting levels

AAASeed is a **static-depth render graph**: the depth of the structural tree is fixed at four, independent of how complex the scene becomes. The four levels, from leaf to root, are:

```
c_modules                      (root container, plus singleton main reference)
 └─ c_module                   (groups layers, owns lights / materials / multi-screen)
     └─ c_layers               (a sequence of up to 26 layers, owns cameras / lua / values / traxs)
         └─ c_layer            (one BDD plus its full attribute system)
             └─ c_bdd          (the actual drawer)
```

Each level's job is to add one specific kind of behaviour to the level below it.

### 2.1 `c_layer` — the leaf

A `c_layer` is the smallest meaningful rendering unit. It encapsulates exactly one **`c_bdd`** (the object that actually draws something on screen, see the `c_bdd` family in `Src/obj_ui/bdd/`) and carries a full **attribute system** that decorates the `c_bdd` before it draws.

The attribute system is a fixed set of slots (color, render state, fog, model, deformer, mapping, texturing, shading, stencil, clip, video texture, animated texture, multiple, lights switch, transforms, ...). Each slot is in one of three modes:

- **current** — use whatever is currently active in the engine.
- **owner** — install this layer's own version of the attribute (overrides the current).
- **none** — the attribute is disabled for this draw.

Some slots are required, others are optional. The `c_layer` updates the requested attributes before its `c_bdd` draws, then restores the previous attributes when the draw is done. The push/pop machinery for that lives in `c_layer_att`.

### 2.2 `c_layers` — a sequence of layers

A `c_layers` holds **up to 26 `c_layer`**, addressed by a single letter from `a` to `z`. The 26-letter convention is enough for most cases given how rich the `c_bdd` family is (a single `c_layers` can express quite complex behaviour without nesting deeper).

On top of the layer sequence, a `c_layers` adds four kinds of state:

1. **A Lua wrapper** (`c_lua_wrap`). The wrapper points to a Lua script (either a `.lua` file path or an inline text block) and exposes a handful of options that control how the script is used at runtime:
   - **doit** — run the script every frame, or stop running it.
   - **doit_trig** — trigger a single one-shot execution.
   - **file_check** / **file_check_always** — watch the script file for changes (one-shot or continuous), so editing the `.lua` outside the engine reloads automatically.
   - **file_reload_trig** — force a manual reload.
   - **run_only_when_compiled** — only execute if the last compilation succeeded.
   - **skip_rest** — flag set from the script to short-circuit the rest of the parent's execution this frame.
   - **edit_trig** — open the script for edit.

   The script is executed first, before any layer draws. This is what makes the layers behaviour dynamic: the script can read, modify or animate any param of any sub-layer, decide which layers to skip this frame, etc.
2. **Cameras** (`c_cameras`). Up to 16 cameras owned by the `c_layers`. The active camera is selectable per layer.
3. **A values block** (`c_obj_value`). Generic placeholders, named typed slots, used by the Lua script and by traxs as scratch storage / shared state.
4. **A traxs collection** (`c_traxs`). Traxs are AAASeed's older animation-driver mechanism. A trax can be plugged in or out on any param to drive its value over time (a sine wave, a MIDI input, a random walk, an envelope, etc.). Lua scripting now does most of what traxs used to do, but traxs remain in the engine because they are still the right tool for many cases (legacy projects, MIDI-driven shows, time-line replay) and because their plug-in / plug-out model is cleaner for parameter automation.

In practice a single `c_layers` is often enough to encapsulate a whole effect: a Lua script orchestrating a few `c_bdd` instances through their attributes, with cameras and traxs providing the time-varying inputs.

### 2.3 `c_module` — group of layers, plus lights and materials

A `c_module` groups several `c_layers`. It adds its own lua / values / traxs / cameras (same shape as `c_layers`, scoped one level higher), and on top:

- **Lights** (`c_lights`) — up to 8 lights, fixed-function or modern UBO struct.
- **Materials** (`c_materials`) — optional, up to 128 materials with separate front / back indices.
- **Multi-screen handling** (`c_multi_screen`) — for multi-projector / multi-display setups.

The intuition: a `c_module` is what you reach for when a single `c_layers` is not enough, typically when the effect needs its own lighting setup or material pool.

### 2.4 `c_modules` — top of the graph

A `c_modules` groups modules. It adds yet another level of lua / values / traxs at the program-wide top. There is normally a single live `c_modules` in the engine, referenced through `c_modules::main` (the top-level singleton).

Above the four, **`c_app`** is the absolute top of the engine, exposed as the global `g_app`. It owns the modules and the global rendering settings.

---

## 3. Why a static-depth graph

AAASeed deliberately fixes the depth at four. This is a constraint, but it is what makes the engine fast and predictable:

- The render loop walks a known structure each frame, no recursive descent.
- Each level has a clear, distinct role (BDD + attributes / lua + cameras / lights + materials / top-level orchestration).
- Performance is bounded: the worst case is `c_modules` × N `c_module` × M `c_layers` × 26 `c_layer`, all flat arrays.
- Tooling (UI panels, save / load formats, Lua API, MIDI mappings) can target each level individually because they are stable.

Going deeper, when an effect genuinely needs more nesting, is the job of the application-side scripting layer (next section).

---

## 4. The application side: MEUs and the Lua tree

Above the engine, the application layer (Lua scripts in the sibling repo `AAASeed`, plus the GaBuZoMeu UI framework) builds **MEUs**: **M**odule **E**ditable **U**nit. A MEU is the user-facing unit of organisation, what an artist composes a show out of.

A MEU is built on top of either:

- **a `c_layers`** — if the effect fits in a single layers (a Lua-wrapped sequence of BDDs is enough), or
- **a `c_module`** — if the effect needs its own lights or materials.

Once an effect is wrapped as a MEU, the underlying `c_layers` or `c_module` is no longer a direct branch of the global engine graph. Instead, the scripting layer manages it as part of a higher-level arborescence (a tree of MEUs, with sub-MEUs, named entries, save / load, presets, time-line orchestration, etc.). The engine still draws each `c_layers` / `c_module` as a flat unit, but how MEUs are organised, named, persisted and combined is a Lua-side concern.

This means the **structural depth is split** between the C++ engine (4 fixed levels for performance) and the Lua application layer (arbitrary depth, pure script, infinitely composable). The engine handles the hot path; the script handles the architecture of a show.

---

## 5. Vocabulary cheat-sheet

| Term | What it is |
|---|---|
| `c_obj_ui` | The base class for every named, param-bearing object in the engine. |
| **Param** | A typed control / output cell on a `c_obj_ui`. |
| `c_bdd` | The leaf class family, the actual drawer (geometry, image, particles, sensor, file, ...). The `c_bdd` vocabulary is rich, see `Src/obj_ui/bdd/`. |
| **Attribute** | A piece of state surrounding a `c_bdd` draw (color, render mode, lights switch, fog, ...), managed by `c_layer`. |
| **Layer** | One `c_bdd` plus its attributes. The smallest meaningful render unit. |
| **Layers** | A sequence of up to 26 layers (`a..z`), with its own Lua wrapper, cameras, values and traxs. |
| **Module** | A group of layers, plus lights, materials, multi-screen. Has its own Lua wrapper / cameras / values / traxs. |
| **Modules** | A group of modules at the top of the engine, with its own Lua wrapper / values / traxs. Singleton-like (`main`). |
| **Trax** | An animation driver pluggable on a param. Sine, random, MIDI input, envelope, etc. Older mechanism, complementary to Lua. |
| **Lua wrapper** | A `c_lua_wrap` attached at a level of the graph. Points to a Lua script and exposes options that control its use at runtime (run / stop, one-shot trigger, file watch and auto-reload, manual reload, run-only-when-compiled, skip-rest, edit). |
| **Lua script** | The actual `.lua` file (or inline text block) that the wrapper points to. Pure script, edited and reloaded independently of the engine. |
| **Values** | A `c_obj_value` block, generic typed placeholders, used as scratch / shared state by Lua and traxs. |
| **MEU** | Module Editable Unit, the application-side user-facing unit. Built on a layers or a module. |

---

## See also

- `Src/infrastructure/layer/CLAUDE.md` — the implementation-side guide for the four classes and their attribute stacks.
- `Src/obj_ui/bdd/` — the BDD vocabulary (geometry, images, particles, hardware sensors, ...).
- `Src/infrastructure/param/` — the param system that underpins every `c_obj_ui`.
- The sibling `AAASeed` repo — Lua kernel (`AAAKernel/lua/`), public API doc (`AAADoc/lua_aaaseed_*.lua`), and the GaBuZoMeu UI framework that drives MEUs.
- `doc/AAASeed Base concept.odt` and `doc/AAASeed programming guideline.odt` — older companion documents covering complementary aspects of the engine.
