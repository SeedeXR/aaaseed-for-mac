# Building from source

This page covers building AAASeed for Mac from a clean checkout. For the
end-to-end DMG ship pipeline see [Ship script](ship-script.md).

---

## Prerequisites

| Tool                 | Minimum version       | Notes                                              |
| -------------------- | --------------------- | -------------------------------------------------- |
| macOS                | 13 (Ventura)          | Universal-binary build targets 11 (Big Sur)+       |
| Xcode Command Line   | latest available      | `xcode-select --install`                           |
| CMake                | 3.20                  | `brew install cmake`                               |
| Apple Silicon Mac    | M1 or newer           | Intel Macs can build but cross-compile is slower   |
| (optional) Developer ID Application certificate | -- | For code-signed DMGs ; see [Ship script](ship-script.md) |

The build produces a universal binary by default. Both arches are
compiled natively on Apple Silicon -- Apple's clang frontend handles
`-target x86_64-apple-macos11` without external SDK installs.

---

## Clone + configure

```bash
git clone https://github.com/SeedeXR/aaaseed-for-mac
cd aaaseed-for-mac
git submodule update --init --recursive
cmake -B out/macos-arm64-debug -S .
```

CMake will :

- Locate the `MetalKit`, `Metal`, `Foundation`, `AppKit` frameworks.
- Configure the bundled Lua 5.1 (`cmake/aaa_lua.cmake`).
- Configure the bundled `luasocket` (`cmake/aaa_luasocket.cmake`).
- Configure the bundled `stb_image` + `tinyexr` (PNG / EXR loaders).
- Discover the 169 `.metal` shader sources in `src/shaders/msl/`.

If you see a `Metal framework not found` error, your Xcode CLT install
is incomplete -- run `sudo xcode-select --reset` then reinstall.

---

## Build

```bash
cmake --build out/macos-arm64-debug -j 8
```

The targets you usually care about :

| Target          | What it is                                                                  |
| --------------- | --------------------------------------------------------------------------- |
| `aaaseed_app`   | The `.app` bundle's main executable                                         |
| `aaaseed_meu_runner` | Static lib for the MEU runner sub-lib                                  |
| `aaaseed_ui_widgets_mac` | Static lib for the widget system                                  |
| `aaaseed_metal_backend`  | The `GOL::Backend` Metal implementation                          |
| `all_tests`     | Aggregate target that builds every gtest binary                             |

After a clean build the `.app` is at
`out/macos-arm64-debug/src/ui/macos/AAASeed.app`.

---

## Build modes

| Mode             | Flags                                                              | When to use                                 |
| ---------------- | ------------------------------------------------------------------ | ------------------------------------------- |
| `Debug` (default if no `-DCMAKE_BUILD_TYPE`) | `-O0 -g`                              | Day-to-day development + test iteration     |
| `Release`        | `-Os -flto=thin -dead_strip` + post-link `strip -S`                | Ship DMG ; smallest + fastest binary        |
| `RelWithDebInfo` | `-O2 -g`                                                           | Profiling under Instruments                 |

LTO + dead-strip are configured in `src/ui/macos/CMakeLists.txt`
(c142-A). The strip pass runs as a `add_custom_command(TARGET ...
POST_BUILD)`. For Release builds the resulting `aaaseed_app`
executable is ~75% smaller than Debug.

To configure a Release build :

```bash
cmake -B out/macos-arm64-release -S . -DCMAKE_BUILD_TYPE=Release
cmake --build out/macos-arm64-release -j 8
```

---

## Universal binary (production)

For a UNIVERSAL binary (Apple Silicon + Intel x86_64), use the ship
script which handles per-arch builds + `lipo` automatically :

```bash
./scripts/ship-dmg.sh
```

Defaults : `BUILD_TYPE=Release`, `ARCHES="arm64 x86_64"`, output DMG at
`out/AAASeed-0.0.1.dmg`. See [Ship script](ship-script.md) for the
full env-var matrix.

---

## Single-arch dev build (fast iteration)

When you don't need x86_64 and want a Release DMG ASAP :

```bash
ARCHES="arm64" ./scripts/ship-dmg.sh
```

This skips the x86_64 build entirely + skips `lipo` (no merging
needed). On an M1 Pro the full pipeline (build + sign + DMG) runs in
under 90 s.

---

## CMake presets

`CMakePresets.json` ships several common preset names :

```bash
cmake --preset macos-arm64-debug
cmake --preset macos-arm64-release
cmake --preset macos-x86_64-release
```

Each preset wires `CMAKE_OSX_ARCHITECTURES`, `CMAKE_BUILD_TYPE`, and
the per-arch build directory under `out/`.

---

## Common gotchas

??? warning "I just deleted a .metal file but the .app still has it"

    The MTKView host loads shaders from the bundle's `Resources/shaders/`
    at startup. Run `cmake --build out/...` after editing shaders to
    re-copy the catalog into the bundle. (Hot-reload via FSEvents is
    only wired for `.lua` MEU scripts, not `.metal` shaders.)

??? warning "Tests pass locally but fail in CI on a parallel `ctest -j`"

    See [Distnoted dual-center](memory-doctrine.md#distnoted-dual-center).
    Mac `distnoted` drops distributed notifications between parallel
    ctest workers. The fix : post + observe on **both**
    `CFNotificationCenterGetLocalCenter()` and
    `GetDistributedCenter()`. Already applied to all affected tests.

??? warning "`gtest_discover_tests` only honors the FIRST label"

    Per [CTest label first-only](memory-doctrine.md#ctest-label-first-only).
    Put the primary filter key (`perf`, `regression`, etc.) first in
    `PROPERTIES LABELS` ; subsequent labels are accepted by CMake but
    ignored by `ctest -L`.

---

## Cross-references

- [Architecture](architecture.md)
- [Running tests](testing.md)
- [Ship script](ship-script.md)
- [Memory doctrine index](memory-doctrine.md)
