# AAASeed for Mac — Ship Checklist

User-facing summary of what is in the DMG you just received, what is
verified, and what is intentionally external. If you are the developer,
read `docs/developer/ship-script.md` instead — this file is for the
person who clicks the DMG link in an email.

Build pipeline : `scripts/ship-qt-dmg.sh` (Qt 6 + QML Studio +
nested engine runtime, c152-O).

---

## 1. What is in this DMG

- Single artifact : `AAASeed-Studio-<version>.dmg` (~52 MB
  ULMO/LZMA-compressed).
- One application bundle : **`AAASeed Studio.app`** (the Qt 6 + QML
  authoring shell).
- Nested inside it : **`Contents/Resources/runtime/aaaseed_runtime.app`**
  — engine playback helper that the Studio spawns via QProcess on
  Play (Cmd+P).
- Bundled Qt frameworks (QtCore, QtGui, QtQml, QtQuick,
  QtMultimedia, ...) via `macdeployqt`. Bundled platform plugin
  (`platforms/libqcocoa.dylib`) and QML modules.
- 169 real-algorithm `.metal` shader effects shipped with the
  runtime in `runtime.app/Contents/Resources/shaders/`.
- Starter project (`Contents/Resources/sample/starter.aaaproj.lua`)
  with 3 wired nodes — loadable from the Studio's Home screen.
- App icon (`AAASeed.icns`, hand-logo).

## 2. System requirements

- macOS 13.0 (Ventura) or newer.
- **Apple Silicon** Mac (M1 / M2 / M3 / M4 family) recommended. Intel
  Macs can run under Rosetta but are not a tested CI target —
  Homebrew Qt 6 is arm64-only.
- ~150 MB of free disk space after installation (most of it Qt
  frameworks).

## 3. How to install (drag-and-drop)

1. Double-click `AAASeed-Studio-<version>.dmg`. The DMG mounts as a
   Finder volume.
2. Drag **`AAASeed Studio.app`** onto the `Applications` shortcut
   inside the mounted volume.
3. Eject the DMG.
4. First launch requires clearing the quarantine attribute, because
   the build is unsigned (see section 5). In Terminal :
   ```
   xattr -d com.apple.quarantine "/Applications/AAASeed Studio.app"
   ```
5. Double-click `AAASeed Studio` in `/Applications` to launch.

## 4. What you should see when it launches

- The Studio's **Home screen** opens with project tiles for any
  previously-opened projects (empty list on first launch).
- Big buttons : **✦ New Project** · **Open Project…** · **Open Sample**.
- Once a project is loaded the layout reveals : side tabs (Inspector
  / Assets / Shader Catalog) · Node Graph + Engine Preview · Code
  Editor · right tabs (Camera / Sound / Tasks) · footer Console.
- Press **Cmd+P** (or click ▶ Play) to spawn `aaaseed_runtime` for
  engine playback against the current project.

If you see a "damaged and cannot be opened" dialog, step 4 of
section 3 was skipped — re-run the `xattr` command.

## 5. Code signing status (honest)

This DMG is **unsigned**. macOS Gatekeeper will mark it quarantined
on first launch (hence the `xattr` step in section 3).

To produce a signed build locally, set `CODESIGN_IDENTITY` to your
Developer ID Application identity and re-run `./scripts/ship-qt-dmg.sh`.
The signing identity is never hardcoded — it is read from the
environment.

## 6. Notarization status (honest)

This DMG is **not notarized**. Notarization requires an Apple
Developer Program membership plus an App Store Connect API key. The
ship script has a notarization step that activates when
`NOTARY_API_KEY_PATH`, `NOTARY_API_KEY_ID`, and
`NOTARY_API_KEY_ISSUER_ID` are all set.

## 7. Verify your DMG before installing

```
./scripts/verify-qt-dmg.sh path/to/AAASeed-Studio-<version>.dmg
```

It runs `hdiutil verify`, mounts the DMG read-only, checks :

- `AAASeed Studio.app` exists
- Studio binary is an arm64 Mach-O
- Required Qt frameworks bundled (QtCore, QtGui, QtQml, QtQuick,
  QtMultimedia)
- `qt.conf` landed in `Resources/`
- Nested `aaaseed_runtime.app` is present + executable
- Bundled starter project and icon are present
- Clean detach

Exit code is non-zero on any failure with distinct codes per step.
Pass `--quiet` to suppress progress output.

This is structural verification, not signature verification. If you
need to confirm the DMG came from a specific signer, that is a
separate `codesign --verify` and `spctl --assess` step — not in scope
for `verify-qt-dmg.sh`.

## 8. Architecture support

```
lipo -archs "/Applications/AAASeed Studio.app/Contents/MacOS/AAASeed-Studio"
```

Expected output : `arm64`.

The same check is performed by `verify-qt-dmg.sh` on the mounted DMG.

## 9. Test status (transparency)

The Qt 6 Studio data layer is covered by **54 / 54** Qt::Test cases
across 4 binaries :

- `aaa_qt_studio_model_test` — 25 cases (project lifecycle, node
  graph, links, uniforms, workspace, undo/redo)
- `aaa_qt_panel_models_test` — 8 (sound / camera / tasks)
- `aaa_qt_lua_helper_test` — 14 (Lua lint + asset classifier)
- `aaa_qt_settings_model_test` — 7 (preferences round-trip)

CI runs these on every PR (`macos-14` runner, headless `offscreen`
Qt platform). A legacy gtest tree is gated behind
`-DAAA_BUILD_LEGACY_TESTS=ON` for runtime-side regressions ; it is
not required to pass for a ship.

## 10. What is NOT in this release

- **Code signing** — needs a Developer ID Application certificate.
- **Notarization** — needs an App Store Connect API key.
- **Universal binary** — arm64-only. Intel Macs via Rosetta.
- **Light theme** — Settings dialog lists it but only dark is wired.
- **Full ADS-style drag-and-drop docking** with ghost previews + tab
  extraction. PanelHost provides detach/dock + named workspaces ;
  the full drop-zone preview system is a future polish round.

## 11. Reporting issues

File an issue at the project repository. Include :

- macOS version (`sw_vers -productVersion`).
- Hardware (`uname -m` and `sysctl -n machdep.cpu.brand_string`).
- The output of `./scripts/verify-qt-dmg.sh` on your downloaded copy.
- A short description of what you expected versus what you saw.

If you cannot run the app at all after step 4 of section 3, attach :
```
codesign -dv --verbose=4 "/Applications/AAASeed Studio.app" 2>&1
spctl --assess --type execute --verbose "/Applications/AAASeed Studio.app" 2>&1
```
