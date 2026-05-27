# Vendoring — `vendor/`

This repository is **self-contained**: cloning it gives you everything needed to compile and run AAASeed on macOS. The `vendor/` directory is the heart of that promise.

## What's in `vendor/`

| Path | Origin | Purpose |
|---|---|---|
| `vendor/aaaseed-engine/` | Snapshot of Mâa's `aaaseed_exe-main` Windows repo | C++ engine source (`Src/`), 3rd-party headers (`Include/`), CMake reference (`cmake/`, `CMakeLists.txt`), build docs (`doc/`, `Build_AAASeed.md`, `CODE_STYLE.md`). |
| `vendor/aaaseed-runtime/` | Snapshot of Mâa's `aaaseed-main` runtime repo | Lua kernel (`AAAKernel/`), GaBu UI framework, MEU prototypes (`AAA_PROTO/`), built-in shaders (`AAAKernel/Shader/`), example apps (`AAAAPPs/`), API docs (`AAADoc/`). |

These are full snapshots minus Windows-only artifacts (`lib_x64/`, `Lib_x64/`, `dll_x64/`, `*.exe`, `*.dll`, `*.pdb`) which are excluded both during initial vendoring and via `.gitignore`. The Mac port doesn't consume them.

## Why vendor and not submodule?

A new contributor running `git clone` and then `cmake --preset macos-arm64-debug` should succeed without:
- Recursing submodules.
- Setting environment variables.
- Cloning a separate sibling tree.

Vendoring as a snapshot delivers that. The price is that `vendor/` doesn't track upstream commits automatically — see "Syncing from upstream" below.

## What is allowed inside `vendor/`

Three classes of edit are accepted in this repo:

1. **One-line behavior-preserving cleanups** — whitespace/syntax fixes that Clang requires and that MSVC is unaffected by (e.g., adding a space between a string literal and an adjacent identifier, adding `__aarch64__` to a platform check, replacing `operator "" _h` with `operator""_h`). Pre-approved per the policy in `memory/feedback_upstream_patches.md`. **Each such edit must be recorded in the next `memory/handover_session.md` entry with the full diff.**

2. **Behavior-preserving Mac-side branches** — adding an `#else` or `#elif __APPLE__` arm to an existing conditional that compiles to a no-op on Windows. Same pre-approval, same diff-logging requirement. Example: the `v_xna.h` `#else` branch with inline forwarders to `_v3r`.

3. **Real bug fixes that the Mac port surfaces** — when a Mac-side test reveals a portability bug in the upstream code (e.g., the `RAND_MAX` integer-overflow fix in `aaa_rand.cpp`), the patch goes in `vendor/` AND is flagged for upstream contribution. Mark it in `handover_session.md` with `[upstream-contribution-candidate]`.

What is **not** allowed in `vendor/` without explicit approval:

- Renaming functions or symbols.
- Removing existing code branches.
- Changing default macro values.
- Modifying `vendor/aaaseed-engine/Src/err.h` or `aaa_type.h` content (header-blast triggers full rebuild — pre-approved exception: whitespace-only fixes).

## Syncing from upstream

When Mâa publishes new commits on the Windows tree, here is the procedure:

1. Update the working tree elsewhere on the dev machine (e.g. `~/aaaseed-windows/aaaseed_exe-main/` and `.../aaaseed-main/`). Pull the latest.
2. Diff against `vendor/`:
   ```
   diff -r ~/aaaseed-windows/aaaseed_exe-main vendor/aaaseed-engine | head -100
   ```
3. Identify the upstream-only changes (not our patches).
4. Re-vendor with rsync, but **do not blow away our patches**:
   ```
   rsync -a --exclude='lib_x64/' --exclude='Lib_x64/' --exclude='dll_x64/' \
         --exclude='*.exe' --exclude='*.pdb' --exclude='*.dll' \
         --exclude='.git/' --exclude='.DS_Store' \
         --update \
         ~/aaaseed-windows/aaaseed_exe-main/ vendor/aaaseed-engine/
   ```
   The `--update` flag only overwrites files where the source is strictly newer.
5. Re-apply our patches if any got overwritten. The full patch list lives in `memory/handover_session.md` chronologically (search for `upstream patched`).
6. Rebuild and run the full test suite. Any test that goes red on the new upstream is a regression — bisect, fix, document.
7. Commit the synced vendor with a message like `vendor: sync aaaseed-engine to upstream <sha-prefix> (2026-MM-DD)`.

For now this is a manual procedure. A `tools/sync_vendor.sh` script will land when the cadence demands it.

## What if I want to test against a different vendor copy?

Override the path at configure time:
```
cmake --preset macos-arm64-debug \
      -DAAASEED_ENGINE_ROOT=/path/to/alternate/aaaseed_exe-main
```

The default is `${CMAKE_SOURCE_DIR}/vendor/aaaseed-engine`. The override is useful when:
- You want to test a Mac patch against a fresh upstream pull without re-vendoring.
- You are sharing a tree with a Windows developer working in the upstream tree.

## Why are the runtime and engine kept separate?

Mâa's upstream is structured as two repos (`aaaseed_exe-main` and `aaaseed-main`) precisely because the C++ build artifact loads runtime content at startup. The split survives in `vendor/` for the same reason: the Mac binary (built from `aaaseed-engine/Src/`) loads scripts/shaders/textures from `aaaseed-runtime/AAAKernel/` at runtime.

When the Mac `.app` is packaged, the build's `install` step copies `vendor/aaaseed-runtime/AAAKernel/` into `AAASeed.app/Contents/Resources/AAAKernel/`. See `instructions/instruction.md` §4.1a for the DMG-packaging plan.

## Repo size impact

Roughly:
- `vendor/aaaseed-engine/` — 200 MB (source + headers).
- `vendor/aaaseed-runtime/` — 278 MB (Lua, shaders, fonts, textures, docs, example apps).
- Total vendored — ~480 MB.

For a media engine this is small. For an open-source clone-and-go experience this is the right trade-off.
