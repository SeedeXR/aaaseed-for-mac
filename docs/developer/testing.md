# Running tests

The active test surface is **54 Qt::Test cases** across 4 binaries
covering the Qt 6 + QML Studio data layer. A larger legacy gtest tree
(~500 tests for the pre-Qt ImGui era) is gated behind
`-DAAA_BUILD_LEGACY_TESTS=ON` ; it is **not** required to pass for a
ship.

---

## Active Qt::Test suite (c152-O)

| Binary                              | Cases | Covers                                                            |
| ----------------------------------- | ----- | ----------------------------------------------------------------- |
| `aaa_qt_studio_model_test`          | 25    | Project lifecycle (new / open / save round-trip), node graph (add / remove / select / position / shader / script / uniforms with disk round-trip), link CRUD (+ duplicate-reject), `linksChanged` signal, recents + project delete, project metadata (basename / mtime / exists), workspace save/list/load/delete, workspace reset signal, serialize round-trip, **undo / redo** (basic add+remove, link reversible, redo invalidation on diverging mutation, `canUndo` / `canRedo` accessors). |
| `aaa_qt_panel_models_test`          | 8     | Sound device enumeration, kind-flag sanity, camera enumeration + `setActive` safety, BinaryTask add / remove / `/bin/echo` round-trip. |
| `aaa_qt_lua_helper_test`            | 14    | Lua lint accept / reject / empty ; asset classifier for image / video / mesh / audio / shader / script / project / unknown ; case-insensitive paths ; glyph-icon coverage. |
| `aaa_qt_settings_model_test`        |  7    | Defaults sane, persistence across instances via QSettings, value clamping (font 9-28, tab 1-8, lint 50-2000, autoSave 5-3600), reset-to-defaults, change-signal fires-once. |
| **Total**                           | **54**|                                                                   |

## Running the active suite

```bash
cd out/macos-arm64-debug   # or -release
for t in aaa_qt_studio_model_test \
         aaa_qt_panel_models_test \
         aaa_qt_lua_helper_test \
         aaa_qt_settings_model_test; do
    QT_QPA_PLATFORM=offscreen \
    QT_PLUGIN_PATH=/opt/homebrew/share/qt/plugins \
        ./bin/$t
done
```

Or via CTest :

```bash
ctest -L qt --output-on-failure
```

Each binary uses `QTEST_GUILESS_MAIN` so a real display isn't
required ; `QT_QPA_PLATFORM=offscreen` covers the cases that DO
need a QGuiApplication.

## Machine-aware parallelism

CTest's `-j` flag defaults to `nproc`. On a fresh checkout :

```bash
ctest --parallel        # uses sysctl-reported core count
```

The CI workflow (`ci.yml`) follows the same pattern — no hard-coded
parallelism — so the same step runs efficiently on a 4-core
GitHub-hosted runner and a 12-core dev Mac.

## Legacy gtest tree (`-DAAA_BUILD_LEGACY_TESTS=ON`)

The pre-Qt tree under `tests/unit/`, `tests/integration/`,
`tests/regression/` contains the c113-era 516-test pyramid built for
the ImGui Studio. Most cases still pass against the runtime side
(`aaaseed_runtime.app`) ; the Studio-side cases were superseded by
the Qt::Test surface above. Build them only when investigating an
engine regression in the runtime ; they aren't required for a ship.

```bash
cmake -B out/macos-arm64-debug -S . \
      -DAAA_BUILD_LEGACY_TESTS=ON
cmake --build out/macos-arm64-debug --parallel
ctest --test-dir out/macos-arm64-debug -L "unit|perf"
```

## Test labels

Active suite uses :

| Label             | What                                                  |
| ----------------- | ----------------------------------------------------- |
| `qt`              | All Qt::Test binaries above.                          |
| `unit`            | All four Qt::Test binaries (sub-second each).         |
| `metal-windowed`  | Reserved for tests that need a CAMetalLayer drawable. Empty on the active surface ; CI's `metal-windowed` step is a future-proofing hook. |

## CI gate

`.github/workflows/ci.yml` runs the four Qt::Test binaries on every
PR against `macos-14`. The job is :

- **Machine-aware** — reads `sysctl -n hw.ncpu` and `hw.memsize` into
  job notes ; the build uses `cmake --build --parallel` (no fixed
  `-j`) so it scales to the runner.
- **Headless-aware** — sets `QT_QPA_PLATFORM=offscreen` so Qt boots
  without an on-screen surface. Tests that would require a real GPU
  drawable are skipped via the `metal-windowed` label (currently
  empty ; opt-in via `GH_RUNNER_HAS_GPU=yes`).
- **Failure-friendly** — uploads `Testing/` directory as an artifact
  on failure.
