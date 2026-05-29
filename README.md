# AAASeed for Mac

Native macOS port of [AAASeed](https://aaaseed.org/) — Mâa Berriet's C++/Lua/OpenGL real-time visual engine for live performance, generative art, and interactive installations.

This repository is the macOS development tree. It is **self-contained**: clone it, install the build prerequisites, and `cmake --build` produces an Apple Silicon binary. No external sibling repositories required.

## Status

**Shippable.** Two-binary split :

- **`AAASeed Studio.app`** — Qt6 + QML authoring shell. **Home screen** with project tiles (last-modified, click-to-open, ✕ delete with confirm). **Native macOS menubar** : File · Edit (with Cmd+Z / Cmd+Shift+Z **Undo / Redo**) · Run · View (per-panel visibility toggles + Reset Workspace) · Window (Save / Load / Delete named workspaces). **Preferences** dialog (Cmd+,) — theme · editor font / tab / word-wrap / line-numbers / lint-debounce · auto-save · default project folder. **Detachable panels** (Photoshop-style) — every panel has a ⠿ grip (drag detaches), ⤢ / ⬡ snap toggle, — collapse, × close ; floating windows snap back when dragged near their dock slot. **Code Editor** with Lua syntax highlighting (keywords / strings / numbers / function calls / comments) and live lint (Run is gated until lint is green). **Node Graph** with pin-drag wiring, click-midpoint-to-delete wires, right-click context menus, generous drop tolerance. **Assets** panel with type-classified icons (image / video / mesh / audio / shader / script / project).
- **`aaaseed_runtime.app`** — engine playback. MTKView + MEU runner + 169 MSL shaders + file-watch hot-reload. Spawned by the Studio's `▶ Play` (Cmd+P) with `--project <path>` ; bundled inside `AAASeed Studio.app/Contents/Resources/runtime/`.

Build : `./scripts/ship-qt-dmg.sh` produces `out/AAASeed-Studio-<version>.dmg` (~54 MB, ULMO/LZMA compressed, ad-hoc signed ; export `CODESIGN_IDENTITY` + the three `NOTARY_API_KEY_*` env vars for a Developer-ID + notarised release). Verify with `./scripts/verify-qt-dmg.sh out/AAASeed-Studio-*.dmg`.

Test suite : **54 / 54** Qt::Test cases across 4 binaries — `aaa_qt_studio_model_test` (25 cases : project lifecycle, node graph, links + duplicate-reject, uniforms round-trip, workspace save/load, **undo/redo**, recents, project delete), `aaa_qt_panel_models_test` (8 : Sound / Camera / BinaryTask), `aaa_qt_lua_helper_test` (14 : Lua lint + asset classifier), `aaa_qt_settings_model_test` (7 : settings round-trip + clamping). Legacy gtest tree gated behind `-DAAA_BUILD_LEGACY_TESTS=ON`.

UI is **Qt6 + QML only** since c152-C. Dear ImGui retired (3000+ lines removed) ; the platform-neutral `aaa::ui::studio::Studio` data model is shared between the Qt UI and the engine runtime via `.aaaproj.lua` files.

## Requirements

- **Hardware:** Apple Silicon (M1 / M2 / M3 / M4).
- **macOS:** 13.0 (Ventura) or newer.
- **Toolchain:**
  - Xcode 15 or newer (with Command Line Tools : `xcode-select --install`).
  - CMake 3.27+ (`brew install cmake`).
  - Ninja (`brew install ninja`).
  - **Qt 6** for the Studio (`brew install qt`). Qt 6.6+ for `Shape.CurveRenderer` ; we ship against 6.11.
  - Metal compiler (`xcodebuild -downloadComponent MetalToolchain`) — needed for the runtime's MSL shaders.

## Build

```bash
cmake --preset macos-arm64-debug
cmake --build --preset macos-arm64-debug
ctest --preset macos-arm64-debug
```

Three configure presets are available:

| Preset | Map | Use for |
|---|---|---|
| `macos-arm64-debug` | `-O0 -g` | Day-to-day iteration |
| `macos-arm64-release` | `-O2 -g` (`RelWithDebInfo`) | Bug repros at speed |
| `macos-arm64-metal` | `-O3 -flto=thin` | Shipping binary |

Output lives under `out/<preset>/bin/` and `out/<preset>/lib/`.

## Repository layout

```
aaaseed-for-mac/
├── CMakeLists.txt              Root build script
├── CMakePresets.json           macos-arm64-* presets
├── cmake/                      Build helpers ; aaa_qt6.cmake locates Qt
├── src/                        Mac-native engine code
│   ├── gol/metal/              GOL OpenGL-isolation backend, Metal target
│   ├── meu/                    MEU Lua runner (Mac)
│   ├── ui/macos/               NSApplication / NSWindow / MTKView host (runtime)
│   ├── ui/widgets/             widget chrome (used by runtime)
│   ├── ui/studio/              platform-neutral Studio data model (Q_OBJECT-free)
│   ├── ui/qt/                  Qt6 + QML Studio shell (the authoring UI)
│   │   ├── aaa_qt_main.cpp       QGuiApplication + QQmlApplicationEngine
│   │   ├── aaa_studio_model.*    Q_OBJECT bridge over aaa_studio.h
│   │   ├── aaa_panel_models.*    Sound / Camera / BinaryTask adapters
│   │   ├── aaa_lua_helper.*      Lua highlight + lint + asset classifier
│   │   ├── aaa_settings_model.*  app prefs (theme, editor, auto-save)
│   │   └── qml/                  Main.qml, HomeScreen, SettingsDialog, panels/
│   └── shaders/msl/            169 Path A .metal shaders
├── tests/qt/                   Qt::Test coverage (54 cases ; 4 binaries)
├── tests/unit/, integration/,  Legacy gtest tree (-DAAA_BUILD_LEGACY_TESTS=ON)
└── tests/regression/
├── bundle/macos/               Info.plist.in, entitlements.plist, AAASeed.icns
├── vendor/
│   ├── aaaseed-engine/         Snapshot of Mâa's Windows C++ engine source
│   └── aaaseed-runtime/        Snapshot of the AAASeed Lua runtime content
├── books/                      Reference reading (gitignored, per-contributor)
├── instructions/               Human-readable porting instructions
├── memory/                     Agent-readable project state (gitignored, per-contributor)
└── VENDORING.md                How vendor/ relates to upstream and how to sync
```

## How to inspect or customise

- **Engine C++ source:** `vendor/aaaseed-engine/Src/` — read-only reference. Edits to upstream files are governed by the pre-approved patch policy described in `VENDORING.md` and `memory/feedback_upstream_patches.md`. Most edits should land in `src/` (Mac-native code) instead.
- **Mac-native code:** `src/` — anything platform-specific. Add freely.
- **Lua scripts and shaders:** `vendor/aaaseed-runtime/AAAKernel/` — same content tree the Windows binary uses. Edit freely; AAASeed's hot-reload picks up changes at runtime.
- **Build system:** `CMakeLists.txt`, `cmake/`. Pure CMake, no Xcode-only steps.

## Tests

```
ctest --preset macos-arm64-debug
```

Categories (selectable via `-L`):

- `sanity` — toolchain-only checks (no engine deps).
- `platform` — verifies AAASeed's platform-detection macros produce expected Apple Silicon values.
- `math` — exercises the vendored `Src/math/` subsystem.

Every porting step must land with passing **unit + integration + regression** tests before being marked complete. See `memory/agent_profile.md` for the testing discipline.

## Project documentation

If you are an LLM agent or a new contributor, **read `memory/session_start.md` first**. It walks through the project's design philosophy, the porting approach, and the open threads.

The `memory/` folder is the project's living brain:
- `project_context.md` — goal, deliverables, evaluation criteria.
- `philosophy.md` — design principles (Apple Silicon UMA, TBDR, Metal-cpp, no-Swift, single-source runtime, etc.).
- `mindmap.md` — architecture map (always current).
- `porting.md` — per-subsystem porting recipes.
- `todo.md` — roadmap with strikethrough convention.
- `handover_session.md` — append-only session log with timestamps.

`instructions/instruction.md` is the end-to-end execution runbook.

## License

MIT — see [`LICENSE`](LICENSE) for the full text. This matches upstream AAASeed (`vendor/aaaseed-engine/LICENSE` and `vendor/aaaseed-runtime/LICENSE`).

## Contributing

Bug reports, fixes, sample MEUs and shader revivals are welcome. See [`CONTRIBUTING.md`](CONTRIBUTING.md) for the dev-environment setup, code style, PR flow, and testing discipline. By contributing, you agree your work is released under the project's MIT License.

Community expectations are documented in [`CODE_OF_CONDUCT.md`](CODE_OF_CONDUCT.md). Security vulnerabilities should be reported privately per [`SECURITY.md`](SECURITY.md).

## Credits

- **AAASeed** — Mâa Berriet (engine, 1996–present), Franz Hildgen (significant contributions), the ArtCast4d.eu European project.
- **Mac port** — Alex Mkwizu (`a.mkwizu@seedexr.com`), with AI-assisted research and porting.

## Reporting issues

For Mac-specific issues, open an issue in this repository (templates : bug, feature, question). For engine-level issues that affect both Windows and Mac, please surface them upstream where Mâa maintains the canonical engine. For security issues, see [`SECURITY.md`](SECURITY.md).
