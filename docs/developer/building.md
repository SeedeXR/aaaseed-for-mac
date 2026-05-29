# Building from source

This page covers building **AAASeed Studio** (the Qt 6 + QML
authoring app) and **aaaseed_runtime** (the engine playback helper)
from a clean checkout. For the end-to-end DMG ship pipeline, see
[Ship script](ship-script.md).

---

## Prerequisites

| Tool                                | Minimum             | Notes                                                           |
| ----------------------------------- | ------------------- | --------------------------------------------------------------- |
| macOS                               | 13 (Ventura)        | `CMAKE_OSX_DEPLOYMENT_TARGET=13.0` baked into the ship script   |
| Apple Silicon Mac                   | M1 / M2 / M3 / M4   | arm64-only ; Intel Macs run via Rosetta but aren't a CI target  |
| Xcode Command Line Tools            | latest              | `xcode-select --install`                                        |
| CMake                               | 3.27                | `brew install cmake`                                            |
| Ninja                               | 1.10                | `brew install ninja`                                            |
| **Qt 6**                            | **6.6** (we ship 6.11) | `brew install qt`. Qt 6.6+ for `Shape.CurveRenderer`         |
| Metal toolchain                     | shipped with Xcode  | `xcodebuild -downloadComponent MetalToolchain` if missing       |

Homebrew lands Qt at `/opt/homebrew/opt/qt/`. `cmake/aaa_qt6.cmake`
auto-detects this ; no extra `Qt6_DIR` configuration needed.

## Clone + configure

```bash
git clone https://github.com/SeedeXR/aaaseed-for-mac
cd aaaseed-for-mac
git submodule update --init --recursive

cmake -S . -B out/macos-arm64-debug \
    -G Ninja \
    -DCMAKE_BUILD_TYPE=Debug \
    -DCMAKE_OSX_ARCHITECTURES=arm64 \
    -DCMAKE_OSX_DEPLOYMENT_TARGET=13.0
```

For a Release build, swap `Debug` → `Release` and change the build
directory accordingly (`out/macos-arm64-release`).

## Build

```bash
cmake --build out/macos-arm64-debug --parallel
```

`--parallel` with no argument uses CMake's auto-detected core count
(via `Ninja`). On a 12-core M-series Mac this saturates the machine ;
on a 4-core CI runner it scales down automatically. Don't hard-code
`-jN`.

Common build targets :

| Target                         | What                                    |
| ------------------------------ | --------------------------------------- |
| `aaaseed_app_qt`               | Studio app : `bin/AAASeed-Studio.app`   |
| `aaaseed_runtime`              | Engine playback : `bin/aaaseed_runtime.app` |
| `aaa_qt_studio_model_test`     | Qt::Test : Studio data layer (25 cases) |
| `aaa_qt_panel_models_test`     | Qt::Test : Sound / Camera / Tasks (8)   |
| `aaa_qt_lua_helper_test`       | Qt::Test : lint + asset classifier (14) |
| `aaa_qt_settings_model_test`   | Qt::Test : preferences round-trip (7)   |

## Test

```bash
cd out/macos-arm64-debug
ctest -L qt --output-on-failure
```

The Studio test surface uses `QT_QPA_PLATFORM=offscreen`, set
automatically by the CTest properties.

## Run

```bash
# Studio (authoring shell)
out/macos-arm64-debug/bin/AAASeed-Studio.app/Contents/MacOS/AAASeed-Studio

# Runtime (engine playback) directly with a project file
out/macos-arm64-debug/bin/aaaseed_runtime.app/Contents/MacOS/aaaseed_runtime \
    --project bundle/macos/sample/starter.aaaproj.lua
```

The Studio's `▶ Play` (Cmd+P) spawns the runtime via `QProcess`.

## DMG

```bash
./scripts/ship-qt-dmg.sh
```

Produces `out/AAASeed-Studio-<version>.dmg`. See
[Ship script](ship-script.md) for env vars (CODESIGN_IDENTITY,
NOTARY_API_KEY_*).

## Legacy gtest tree

The pre-Qt test tree (`tests/unit/`, `tests/integration/`,
`tests/regression/`) is gated behind an opt-in :

```bash
cmake -S . -B out/macos-arm64-debug \
    -DAAA_BUILD_LEGACY_TESTS=ON \
    ...
```

It's not required to pass for a Qt-era ship.
