# AAASeed , Modifying MEUs

## 1. Why this guide

You already know how to use AAASeed: open an APP, play with MEUs, build a piece. You're starting to want more , change a value range that doesn't fit, add a missing control, tweak how a MEU behaves.

This guide is for that step. It does not turn you into an engine developer; it shows what a MEU is made of, how to look inside one, and how to make small targeted changes that survive and reload cleanly. Two paths are covered: changing things from the interface, and editing the Lua script.

## 2. What a MEU is made of

A MEU has two faces, and they live together in the same folder.

**The c_layer side (C++).** The visual stack of objects that actually does the rendering: textures, shaders, FBOs, cameras, render passes. You don't write this side, but you see and manipulate it from inside AAASeed: clicking on a c_layer brings up its params, you change values, you save.

**The Lua side.** A small script (`<type>.lua` in the MEU folder) that:
- builds the MEU's user interface (the sliders, buttons, selectors you see),
- initializes things on first open,
- prepares parameters before each frame,
- can take over the rendering when you want custom behavior.

The two are saved together as part of the MEU. When you reload, both come back exactly as they were.

## 3. BUs, values, and targets

A **BU** (Box User) is what you see on screen: a slider, a button, a selector, a window. It is **autonomous**: it holds and lets you edit a value of its own. Whatever the script does with that value (or doesn't do) is a separate question , the BU exists, displays, and accepts edits on its own terms.

By default the value is **internal**: nothing else holds it; reading or writing the BU goes to its own storage.

Optionally a BU can be **bound to an external source via a target**, so the value actually lives elsewhere and the BU stays in sync with it. AAASeed currently provides two target kinds:

- **`target_lua`** , a field in a Lua table (e.g. an attribute on the MEU itself, or on any reachable Lua object).
- **`target_param`** , a **c_param** (a named knob on a `c_obj_ui` like a c_layer or c_module). This is a direct wire between one BU and one c_param. Use it when that direct connection is the right shape; if your interaction with the C++ side is more elaborate, prefer the general-purpose route below.

The model is open , more target kinds may be added later (network endpoints, automation curves, ...).

The general-purpose way of interacting with the C++ side is **not** `target_param` , it is the set of Lua references and functions that let you reach `c_obj_ui` and `c_param` directly: `aaa.obj.get_*`, `param.get_ref(obj, name)`, `param.get(ref)`, `param.set(ref, val)`, etc. With those you can read/write any c_param at will, derive its value from any combination of BU values, write to several c_params at once, or do nothing at all. `target_param` is a convenience for the simple "one BU mirrors one c_param" case; the real plumbing is the references-and-functions API.

A few practical implications follow:

- **An entire APP can be written without ever using `target_param`.** Many do, even when they talk to the C engine: they hold c_obj_ui / c_param refs and drive them from `update()`.
- An APP that doesn't talk to `c_obj_ui` at all needs neither `target_param` nor c_param refs. GaBu sits on top of the C engine but doesn't require it for every interaction.
- The user sees and touches BUs. How those BU values flow into the rest of the system, into c_params or anywhere else, is a script-level decision, made case by case.

## 4. Looking inside a MEU

Pick a MEU you've already used, say a `Mire_1`. Open your APP folder on disk, navigate to `<APP>/AAA_MEU/Mire_1/`. You'll see something like:

```
Mire_1/
├── fx.aaa_layers_all      ← the C++ layer tree of the MEU
├── preset_0.plua          ← auto-saved current values of all BUs
├── preset_1.plua          ← user-saved snapshot 1 (if any)
└── ...                    ← other resources the MEU uses (shaders, images, ...)
```

The `.lua` script for `Mire`-type MEUs is normally in the **prototype folder**, not in the instance folder , because all `Mire_*` instances share the same script. The proto folder is somewhere up the search path: in your APP's `AAA_PROTO/`, in a parent folder's `AAA_PROTO/`, or in the kernel's `AAAKernel/AAA_PROTO/MEU_PROTO_*/Mire/`.

> If a MEU has its `.lua` directly in its instance folder (no separate proto exists), it is a **self-proto**: the instance is its own prototype. Once the self-proto exists, **subsequent instances of the same type adopt it as their proto**, so editing this `.lua` affects every instance of the type that uses this self-proto. We come back to this in section 8.

## 5. Modifying without coding: the c_layer editor

The simplest way to change a MEU's behavior is from the AAASeed interface. No Lua needed.

1. Open the MEU you want to tweak (the MEU UI appears).
2. **Focus a c_layer**: most MEUs have a "Focus" button (or several, e.g. `Focus Video`, `Focus Kinect`) that opens the c_params of one of the underlying c_layers.
3. The c_layer's params window appears with all its knobs , far more than what the MEU UI exposes by default.
4. Adjust what you need. Changes are applied immediately.
5. Save the MEU (Save button on its bar). Your changes go into the MEU's files on disk and survive the next reload.

This is enough for most tweaks: try different blend modes, change a smoothing factor, swap a shader, tune a default. You're editing the c_layer side without touching code.

## 6. First Lua edit

Sometimes the c_layer is fine but you want to change the **interface** itself: extend a slider's range, add a missing control, change a default. That lives in the Lua script.

1. On the MEU's UI bar, click **Lua**. The script opens in your configured editor.
2. Find `define_ui()`. This is where the BUs are added.
3. Make a small change (next section gives examples), save the file in your editor.
4. Within a moment, AAASeed picks up the change automatically. The MEU's UI rebuilds.
5. Test, adjust, save the file again as needed. Save the MEU's state (Save) when happy.

If something looks wrong after an edit, the **Def** button on the MEU UI rebuilds the MEU from scratch (it frees and recreates the MEU, calling `init` and `define_ui` again). Use it when state has gotten weird.

## 7. Recipes

Short, concrete tweaks you'll use often.

### 7.1. Change a slider's range

Find the line in `define_ui` that creates the slider. It will look like:

```lua
self:add_param_obj_name( {ix, iy, 8, 1}, "Factor", ref.bdd, "factor", 0, 64 )
```

The two trailing numbers (`0, 64` here) are the min and max. Change them:

```lua
self:add_param_obj_name( {ix, iy, 8, 1}, "Factor", ref.bdd, "factor", 0, 128 )
```

Save. The slider now ranges to 128.

### 7.2. Add a button on an existing c_param

Suppose the c_layer has a boolean c_param `smooth` you want exposed.

```lua
self:add_param_obj_name( {ix, iy, 1, 1}, "smooth", ref.bdd, "smooth" )
```

`add_param_obj_name` figures out the right BU type (button for booleans, slider for floats, etc.) from the c_param's type. The first arg is the position+size in the 16×16 grid (column, row, width, height).

Pick `ix, iy` so the new BU doesn't overlap an existing one.

### 7.3. Force a behavior in update

If you want every frame to clamp a computed value or apply a condition before the c_layer reads it, write `update()`:

```lua
function meu:update()
    local v = self:get_bu_value("speed")
    if v < 0.1 then
        v = 0.1   -- minimum effective speed
    end
    param.set( self.ref.speed_param, v )
end
```

`get_bu_value(key)` reads the current value of the BU you named. `param.set` pushes the value to the c_param.

### 7.4. Customize the MU icon

The MU is the small visual handle in the parent MUS. You can draw a custom icon for your MEU type:

```lua
function meu:draw_icon()
    MEU.draw_icon( self )                 -- keep the default frame
    self:set_icon_color()
    gol.set_line_width( 2 )
    gol.push_translate_scale_2d( -.35, 0, .3, 1.2 )
        aaa.draw_circle_axe_z( 0, 0, 0, 1.2, 6 )
    gol.pop_matrix()
end
```

Save. Every MU of this MEU type now shows the custom icon in its MUS.

## 8. Proto vs instance: who am I editing?

This is the one concept that traps everyone the first time.

A MEU has a **prototype** (the `.lua` script that defines its type) and one or more **instances** (the actual MEUs in your APP). When you edit Lua, the question is: am I editing the prototype, or am I editing one specific instance?

**Editing the prototype** affects every instance of that type, in this APP and in others that load the same proto. If you change `Mire`'s `define_ui`, every `Mire_1`, `Mire_2`, ... in every APP will get the new UI on next reload. This is what you want when you're improving a MEU type.

**Editing a self-proto** affects every instance of its type that uses this self-proto. A self-proto is a MEU whose folder contains the `.lua` directly (because no separate proto was found for its type at the time it was created). Once it exists, subsequent instances of the type adopt it as their proto, so it behaves like any shared prototype , the changes are not local to a single instance.

How to tell where you are: when you click **Lua** on a MEU's UI, AAASeed opens whichever `.lua` actually drives the MEU. Check the file path:

- `AAAKernel/AAA_PROTO/MEU_PROTO_<bucket>/<Type>/<type>.lua` , kernel-shipped proto, careful: changing it affects the engine's defaults for everyone.
- `<APP>/AAA_PROTO/<Type>/<type>.lua` or in a parent dir's `AAA_PROTO/` , an app-local or shared proto. Changes affect every instance of the type in apps that see this proto.
- `<APP>/AAA_MEU/<Type>_<inst_key>/<type>.lua` , a self-proto instance. Changes are local to this instance.

When in doubt, **make a copy first**: duplicate the proto folder under a different name (or under your APP's `AAA_PROTO/`), then edit the copy. You'll experiment without breaking anything else.

## 9. Pitfalls and tips

A short list of things that catch people the first time.

- **Empty method does not remove a method.** If you delete a `function meu:foo() ... end` from the file and save, the previous `foo` stays alive. The proto's table accumulates definitions across reloads. To actually drop a method: nil it explicitly (`meu.foo = nil`), or hit **Def** to recreate the MEU from scratch.
- **`init` is not re-run on save.** A plain Lua save updates methods, but `init()` ran only once at MEU creation. To re-run it after an edit (because you changed init logic), use **Def**.
- **Renaming a BU breaks presets.** Preset files reference BUs by name. If you rename a BU after saving presets, those presets lose that BU's value on reload. Pick names you can live with from the start.
- **The instance key matters for some types.** `Fbo_F2` uses `F2` as the FBO name; `Ref_<Name>` references the MEU named `<Name>`. Don't pick instance keys arbitrarily for these types.
- **Back up before risky tweaks.** Copy the MEU folder, or commit to git, or save the APP under a new name. Saving the file commits the change to disk; there's no undo for a saved Lua edit.
- **Use presets for A/B testing.** Store the current state in slot 1 (Ctrl+click on the slot), make your changes, store in slot 2, click between them to compare. Slot 0 is always the live current state.

## 10. Where to go next

- **AAASeed user manual** , for the basics of using AAASeed without writing code.
- **`AAASeed_dev_guide.md`** (in this folder) , when you want to create new MEU types from scratch, contribute to the framework, or understand the engine internals.
- **`MEU_and_MU.md`** (in this folder) , reference for naming, labels, finding MEUs by name.
- **`lua_aaaseed_*.md`** (in this folder, auto-generated from `.lua`) , the public Lua API: drawing, params, helpers, classes.
