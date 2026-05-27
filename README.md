# AAASeed for Mac

Native macOS port of [AAASeed](https://aaaseed.org/) — Mâa Berriet's C++/Lua/OpenGL real-time visual engine for live performance, generative art, and interactive installations.

This repository is the macOS development tree. It is **self-contained**: clone it, install the build prerequisites, and `cmake --build` produces an Apple Silicon binary. No external sibling repositories required.

## Status

Work in progress. The math subsystem compiles and tests pass on Apple Silicon. Graphics backend (Metal) and UI host (Cocoa) are upcoming. See `memory/todo.md` for the live roadmap.

## Requirements

- **Hardware:** Apple Silicon (M1 / M2 / M3 / M4).
- **macOS:** 13.0 (Ventura) or newer.
- **Toolchain:**
  - Xcode 15 or newer (with Command Line Tools installed: `xcode-select --install`).
  - CMake 3.27+ (`brew install cmake`).
  - Ninja (`brew install ninja`).
  - Metal compiler (`xcodebuild -downloadComponent MetalToolchain`) — needed once Phase 3 lands.

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
├── cmake/                      Build helpers
├── src/                        Mac-native engine code (gol/metal/, ui/macos/, etc.)
├── tests/
│   ├── unit/                   GoogleTest unit tests
│   ├── integration/            (Coming soon) end-to-end MEU runs
│   └── regression/golden/      (Coming soon) frame-buffer baselines
├── vendor/
│   ├── aaaseed-engine/         Snapshot of Mâa's Windows C++ engine source
│   └── aaaseed-runtime/        Snapshot of the AAASeed Lua runtime content
├── books/                      Reference reading (Metal, GPU on Apple Silicon, AAASeed intro)
├── instructions/               Human-readable porting instructions
├── memory/                     Agent-readable project state (philosophy, todo, mindmap, sessions)
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

MIT, matching upstream AAASeed. See `vendor/aaaseed-engine/LICENSE` and `vendor/aaaseed-runtime/LICENSE`.

## Credits

- **AAASeed** — Mâa Berriet (engine, 1996–present), Franz Hildgen (significant contributions), the ArtCast4d.eu European project.
- **Mac port** — Alex Mkwizu (`a.mkwizu@seedexr.com`), with AI-assisted research and porting.

## Reporting issues

For Mac-specific issues, open an issue in this repository. For engine-level issues that affect both Windows and Mac, please surface them upstream where Mâa maintains the canonical engine.
