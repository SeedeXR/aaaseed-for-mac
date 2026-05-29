# Architecture

AAASeed for Mac is a Mac-native VJ / generative-art engine shipped as
**two cooperating app bundles** :

- **`AAASeed Studio.app`** — Qt 6 + QML authoring shell. Project I/O,
  node graph, code editor, panels. Talks to the engine via
  `.aaaproj.lua` files and spawns `aaaseed_runtime.app` for playback.
- **`aaaseed_runtime.app`** (nested inside the Studio's
  `Contents/Resources/runtime/`) — engine playback. Owns the
  `MTKView`, the `GOL::Backend`, the MEU runner, the 169 `.metal`
  shader catalog, and the file-watch hot-reload.

This page is the developer's mental map. For history + decisions see
the [memory doctrine index](memory-doctrine.md). For the Studio
internals specifically see [Studio UI](studio.md).

---

## High-level diagram

```mermaid
flowchart LR
    subgraph StudioApp["AAASeed Studio.app  (Qt6 + QML)"]
        QML[QML scene<br/>Main.qml + panels/]
        Models[Q_OBJECT bridges<br/>StudioModel · NodeListModel<br/>SoundDeviceModel · LuaHelper<br/>SettingsModel · ...]
        Core[aaa::ui::studio::Studio<br/>platform-neutral data model]
        QML --> Models --> Core
    end

    Proj[(.aaaproj.lua<br/>project file)]

    subgraph RuntimeApp["aaaseed_runtime.app  (engine playback)"]
        Delegate[AAASeedAppDelegate]
        MTKView[AAASeedMTKView]
        InputView[AAASeedInputView<br/><i>NSTextInputClient</i>]
        Delegate --> MTKView
        MTKView -.subclass.-> InputView
    end

    subgraph Render["Render side"]
        Backend[GOL::Backend<br/>MetalBackend]
        Shaders[(169 .metal<br/>shaders)]
        Backend --> Shaders
    end

    subgraph Authoring["Authoring side"]
        Runner[aaa::meu::Runner]
        Widgets[aaa::ui::widgets::<br/>WidgetSystem]
        FileWatcher[aaa::meu::<br/>FileWatcher]
        Lua[(lua_State)]
        Runner --> Lua
        Runner <--> Widgets
        Runner --> FileWatcher
    end

    Core <-->|save / open| Proj
    Core -.QProcess spawn<br/>--project arg.-> Delegate
    Proj -.--project arg.-> Runner

    MTKView --> Backend
    MTKView --> Runner
    MTKView --> Widgets
    InputView --> Widgets

    Runner -.aaa.* bindings.-> Backend
    Widgets -.batched draws.-> Backend
```

The Studio process **never** opens a Metal device. All GPU work
happens in the runtime process. The two processes communicate via the
`.aaaproj.lua` file (saved by Studio, read by runtime on launch) and
via QProcess stdout/stderr streamed into the Studio's Console panel.

---

## Subsystem boundaries

The Mac port deliberately limits the surface of vendor-engine code it
absorbs. Most user-visible functionality is in **Mac-native sub-libs**
under `src/` that compile without the blocked `aaa_mem` / `c_cpu`
cascade.

| Layer                       | Lives in                                                | Doctrine                                                                       |
| --------------------------- | ------------------------------------------------------- | ------------------------------------------------------------------------------ |
| Studio shell (Qt6 + QML)    | `src/ui/qt/`                                            | QGuiApplication + QQmlApplicationEngine ; no Metal in this process             |
| Studio data model (neutral) | `src/ui/studio/aaa_studio.{h,cpp}`                      | Platform-neutral ; serializes to `.aaaproj.lua` for round-trip                 |
| Runtime app host (Cocoa)    | `src/ui/macos/`                                         | Mac-only ; Objective-C++ ; subclasses `MTKView`                                |
| Metal rendering backend     | `src/aaa/` + `src/macos/`                               | `GOL::Backend` -- present-per-pass discipline                                  |
| Shader catalog              | `src/shaders/msl/` (169 `.metal` files)                 | [Path A revival pattern](memory-doctrine.md#path-a-revival-pattern)            |
| MEU runner (Lua VM + draws) | `src/meu/aaa_meu_runner_mac.{h,mm}`                     | [Hermetic Mac sub-lib](memory-doctrine.md#hermetic-mac-sub-libs)               |
| File watcher (hot reload)   | `src/meu/aaa_file_watcher_mac.{h,mm}`                   | FSEvents ; GCD dispatch                                                        |
| Widget UI system            | `src/ui/widgets/aaa_widgets_mac.{h,mm}`                 | Immediate-mode + retained state ; runtime-only since Qt cutover                |
| Input event bridges         | `src/ui/macos/aaa_event_bridge*.{h,mm}`                 | [Bridge API standardization](memory-doctrine.md#bridge-api-standardization)    |
| Lua bindings                | inline in `aaa_meu_runner_mac.mm`                       | Raw `lua_pushcfunction` (NO `AAALUACALL` macros)                               |
| Vendor engine (limited)     | `aaaseed-windows/` submodule subset                     | Touched only via shadow shims                                                  |

The vendor engine's **layer subsystem** (`Src/infrastructure/layer/`,
~7 files / ~6.7 K LOC) is **NOT** in the Mac build. Per
`project_layer_supersession.md`
and c142-C the layer subsystem is formally **superseded** by :

- **MEU Runner** (per-frame Lua entry point + shader dispatch).
- **Path A catalog** (the 169 `.metal` shaders).
- **MetalBackend** (`GOL::Backend` present-per-pass + multi-pass).

These three together cover the rendering surface the layer subsystem
would have provided, without dragging in the blocked `aaa_mem` /
`c_cpu` dependency cone. The supersession is reopened **only** with
vendor authorization + a user-surfaced asset-parity need + the
matching Win-side Task #152 work-tree (see
[Windows backend doc](windows-backend.md)).

---

## Render-loop sequence

The render loop is a single-threaded pump rooted in MetalKit's
`drawInMTKView:` callback. The view orchestrates the three side
subsystems (Backend, WidgetSystem, Runner) in a strict order :

```mermaid
sequenceDiagram
    participant MTK as AAASeedMTKView
    participant W as WidgetSystem
    participant R as Runner
    participant B as GOL::Backend
    participant Lua as lua_State

    MTK->>W: begin_frame(w, h, mx, my, pressed, released)
    Note over W: hash-keyed retained state<br/>+ pending quads
    MTK->>B: begin_window_render_pass()
    MTK->>R: render_frame(w, h, target)
    R->>Lua: aaa.on_frame(w, h, frame_idx)
    Lua->>R: aaa.use_shader("...")
    Lua->>B: aaa.draw_fullscreen_quad()
    Lua->>R: aaa.draw_hud_text("status")
    R-->>MTK: (returns)
    MTK->>W: end_frame()
    W->>B: encode batched UI quads
    MTK->>B: render HUD text (glyph atlas)
    MTK->>B: end_render_pass() + present()
```

Per [`feedback_metal_present_per_pass.md`](memory-doctrine.md#metal-present-per-pass)
**every** `end_render_pass()` is paired with a matching
`present()`. Skipping a present causes silent corruption + perf-test
command-queue saturation.

---

## Threading model

AAASeed for Mac runs single-threaded for rendering :

- **Main thread** : `drawInMTKView:` -> WidgetSystem + Runner + Backend.
- **GCD background queue** : `aaa::meu::FileWatcher` runs FSEvents
  callbacks on a `dispatch_queue_create("aaa.meu.filewatcher", ...)`.
  When a watched `.lua` changes, the watcher dispatches the reload
  callback to the main queue via `dispatch_async(dispatch_get_main_queue())`,
  so `Runner::reload()` always executes on the render thread.
- **Cocoa main run loop** : NSEvent dispatch (mouse / keyboard /
  drag-drop). Forwarded to per-view bridges per
  [Bridge API standardization](memory-doctrine.md#bridge-api-standardization).
- **`distnoted`** : two `CFNotificationCenter` channels for parallel
  ctest distributed notifications per
  [Distnoted dual-center](memory-doctrine.md#distnoted-dual-center).

A single `MTLCommandQueue` (owned by `GOL::Backend`) services every
frame. There is no separate compute queue ; multi-pass shaders run
sequentially in command buffers.

---

## Lua VM lifecycle

Each `aaa::meu::Runner` owns exactly one `lua_State`. The lifecycle :

1. `Runner` constructor : creates **no** Lua VM.
2. `Runner::load_script(path)` : `luaL_newstate()` + register `aaa.*`
   binding table + `luaL_dofile(path)`. The script's top-level body
   runs once.
3. Each `Runner::render_frame()` : look up `aaa.on_frame` global, push
   `(width, height, frame_idx)`, `lua_pcall`. Errors are logged via
   `aaa.log_error` but do not crash the app -- the next frame still
   calls `aaa.on_frame`.
4. `Runner::reload()` : the **lua_State is NOT torn down**. The script
   file is re-read + re-run as a new chunk. Globals from the previous
   run persist (Lua's normal idempotent re-registration), but any
   table the script rebuilds in its top-level body gets the fresh
   value.
5. `Runner::unload()` or destructor : `lua_close(L)` + clear cached
   shader IDs.

The Lua bindings use raw `lua_pushcfunction` per c124-A and the c124
hermetic doctrine -- **NOT** the engine's `AAALUACALL` macros. This
keeps the runner buildable without `aaalua`'s reflection chain.

---

## Studio process

The Studio (`AAASeed Studio.app`) hosts the authoring UI in Qt 6 +
QML. It owns no Metal device and never opens an `MTKView`. Its
lifecycle :

1. `QGuiApplication` boots, sets `Fusion` QML style + dark `QPalette`.
2. Constructs an `aaa::ui::studio::Studio` (platform-neutral C++ core)
   and a handful of `Q_OBJECT` adapters (`StudioModel`,
   `NodeListModel`, `SoundDeviceModel`, `LuaHelper`, `SettingsModel`,
   …) registered as QML context properties.
3. `QQmlApplicationEngine` loads `qrc:/aaa/ui/qt/qml/Main.qml`.
4. User edits a project ; mutations route through `StudioModel`,
   which captures undo snapshots via
   `Studio::serialize_to_string()` before each one.
5. **Play (Cmd+P)** : auto-saves the project, then spawns
   `aaaseed_runtime.app/Contents/MacOS/aaaseed_runtime --project <path>`
   via `QProcess`. Stdout/stderr stream into the Studio's Console
   panel.
6. Workspace state (per-panel float/collapse/visible + named slots)
   persists via `QSettings` under `workspace/` and `workspaces/<name>/`.

The full Studio internals (panel docking, settings model, code
editor, asset classifier) are documented in [Studio UI](studio.md).

---

## Cross-references

- [Studio UI](studio.md)
- [Building from source](building.md)
- [Running tests](testing.md)
- [Ship script](ship-script.md)
- [Memory doctrine index](memory-doctrine.md)
- [Path A catalog](path-a-catalog.md)
- [MEU runner](meu-runner.md)
- [Widget system](widget-system.md)
- [NSTextInputClient + IME](nstextinputclient.md)
- [Windows backend](windows-backend.md)
