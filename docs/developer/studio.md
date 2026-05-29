# Studio UI

The Studio is the authoring app that ships as **AAASeed Studio.app**.
It is implemented on **Qt 6 + QML** (since c152-C) and embeds the
existing engine playback runtime as a **separate `aaaseed_runtime.app`**
nested inside its `Contents/Resources/`.

> Earlier sessions used Dear ImGui inside the same Metal pass. That
> stack is retired ; the historical roadmap lives in the
> `qt6-migration` project memory.

## Architecture

```
AAASeed-Studio.app
├── Contents/MacOS/AAASeed-Studio         (Qt6 + QML authoring shell)
├── Contents/Frameworks/                  (QtCore, QtGui, QtQml, QtQuick, QtMultimedia, ...)
├── Contents/PlugIns/                     (platforms/cocoa, qmltooling, ...)
└── Contents/Resources/
    ├── AAASeed.icns                      (hand logo)
    ├── sample/starter.aaaproj.lua        (bundled starter project)
    └── runtime/aaaseed_runtime.app       (engine playback, spawned via QProcess)
```

The Studio process **never** opens Metal. The runtime process owns
the MTKView + MEU runner ; the Studio launches it on Cmd+P with
`--project <path>`. State is shared via the `.aaaproj.lua` file
format (the platform-neutral `aaa::ui::studio::Studio` core).

## C++ surface (`src/ui/qt/`)

| File                     | Q_OBJECT class       | What it exposes to QML                              |
| ------------------------ | -------------------- | --------------------------------------------------- |
| `aaa_studio_model.*`     | `StudioModel`        | Project lifecycle, node graph mutators, undo/redo, recents, workspace save/load |
| `aaa_studio_model.*`     | `NodeListModel`      | `QAbstractListModel` over the node collection      |
| `aaa_studio_model.*`     | `ConsoleListModel`   | `QAbstractListModel` over the log buffer           |
| `aaa_studio_model.*`     | `AssetListModel`     | `QAbstractListModel` over project asset paths      |
| `aaa_panel_models.*`     | `SoundDeviceModel`   | QMediaDevices audio devices + live input level meter |
| `aaa_panel_models.*`     | `CameraController`   | QMediaCaptureSession + QCamera preview binding     |
| `aaa_panel_models.*`     | `BinaryTaskModel`    | QProcess-backed external tasks                     |
| `aaa_lua_helper.*`       | `LuaHelper`          | Lua syntax highlighter attach + linter + asset classifier |
| `aaa_settings_model.*`   | `SettingsModel`      | Theme, editor font/tab/wrap, lint debounce, auto-save |
| `aaa_engine_viewport.*`  | `EngineViewport`     | Foreign NSView host for the in-window engine preview |

All are registered as QML context properties in `aaa_qt_main.cpp`
(`studio`, `sound`, `camera`, `tasks`, `viewport`, `luaHelper`,
`settings`).

## QML tree (`src/ui/qt/qml/`)

```
Main.qml                  ApplicationWindow + menubar + SplitView body
├── HomeScreen.qml        Project gallery overlay (visible when no project open)
├── SettingsDialog.qml    Preferences modal (Cmd+,)
└── panels/
    ├── PanelHost.qml         The docking primitive (see below)
    ├── NodeGraphPanel.qml    Pan/zoom canvas + bezier wires + pin-drag
    ├── EnginePreviewPanel.qml Foreign-NSView embed of aaaseed_runtime
    ├── CodeEditorPanel.qml   Lua-highlighted TextArea + live lint banner
    ├── InspectorPanel.qml    Selected-node fields + uniforms editor
    ├── AssetsPanel.qml       Asset list with type-classified icons
    ├── ShaderCatalogPanel.qml List of bundled MSL shaders
    ├── CameraPanel.qml       Video preview + device picker
    ├── SoundPanel.qml        Audio devices + 440 Hz test + input meter
    ├── BinaryManagerPanel.qml QProcess task list
    └── ConsolePanel.qml      4-level log
```

## Panel docking (PanelHost)

Every panel is wrapped in **`PanelHost`** which gives it a uniform
Photoshop-style chrome :

```
┌────────────────────────────────────────────┐
│ ⠿  Panel Title              ⤢   —   ×      │   ← header bar
├────────────────────────────────────────────┤
│              Panel content                 │
└────────────────────────────────────────────┘
```

| Element              | Action                                                  |
| -------------------- | ------------------------------------------------------- |
| **⠿ grip**           | Left-drag → detach to floating window. Right-click → context menu. |
| **⤢ / ⬡ toggle**    | Detach (when docked) / Snap-back (when floating).       |
| **—  collapse**     | Collapse to header-only / expand. Double-click header also toggles. |
| **× close**         | Emits `closeRequested()` signal — parent flips the matching `showXxx` flag in `Main.qml`. View menu can restore. |

Floating windows are real `Window {}` items. Closing them docks back
(Photoshop semantics). Dragging the window's title bar near the
inline slot's screen position auto-docks (snap threshold ≈ 80 px).

### Workspace persistence

Each PanelHost stores its state under
`workspace/<panelId>/{floating,collapsed,floatX,floatY,floatW,floatH}`
via `QSettings`. Window menu provides :

- **Save Workspace As…** — copies all `workspace/*` keys to
  `workspaces/<name>/*`.
- **Load Workspace ▸ <name>** — restores from there.
- **Delete Workspace ▸ <name>** — removes the slot.
- **Reset Layout to Defaults** — emits `workspaceResetRequested()` on
  `StudioModel` ; every PanelHost + Main.qml listen and reset their
  in-memory state (clearing QSettings alone wouldn't re-bind the live
  aliases).

## Undo / Redo

`StudioModel` keeps two bounded stacks (64 entries each) of serialized
`.aaaproj.lua` blobs. Every mutating method on `NodeListModel` calls
`pushUndoSnapshot()` first, which captures the previous state via
`Studio::serialize_to_string()`. **Cmd+Z** / **Cmd+Shift+Z** invoke
`undo()` / `redo()` which restore via `Studio::load_from_string()`.

The code editor's `TextArea` has its own per-buffer undo built in to
Qt — Cmd+Z routes to it when the editor has focus.

## Code editor

- **Syntax highlighting** : `LuaSyntaxHighlighter` (C++ QSyntaxHighlighter)
  paints keywords (violet bold), stdlib (teal), numbers (amber),
  function calls (yellow), `local` defs (red), strings (green),
  comments (grey italic).
- **Linting** : `LuaHelper::lint()` calls `luaL_loadstring` ; runs on
  a debounced timer (interval driven by `settings.lintDebounceMs`).
  A red/green dot in the editor header indicates status ; the Run
  button is disabled while lint is red.

## Asset classifier

`LuaHelper::classifyAsset(path)` returns one of
`image | video | mesh | audio | shader | script | project | other`
based on extension (case-insensitive). The Assets panel displays a
glyph icon per category ; future node-drop logic can route by type
(image → texture binding, .lua → script binding, etc.).
