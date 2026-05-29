# Ship script

`scripts/ship-qt-dmg.sh` is the **single end-to-end pipeline** that
produces a Qt 6 + QML AAASeed Studio DMG, optionally code-signed and
notarized. Companion `scripts/verify-qt-dmg.sh` runs the post-receipt
structural check.

> The legacy `ship-dmg.sh` (Dear ImGui era, universal binary) was
> retired in c152-P along with the ImGui binary itself.

---

## Pipeline overview

```
1. cmake configure (Release, arm64, Ninja, MACOSX_DEPLOYMENT_TARGET=13.0)
2. cmake --build  aaaseed_app_qt aaaseed_runtime
3. Copy aaaseed_runtime.app  →  Studio.app/Contents/Resources/runtime/
4. macdeployqt  Studio.app  -qmldir=src/ui/qt/qml   (bundles QtCore/Gui/Qml/Quick/Multimedia + platforms/cocoa)
5. (optional) codesign --sign $CODESIGN_IDENTITY  (skipped if env unset)
6. hdiutil create -format ULMO  →  out/AAASeed-Studio-<version>.dmg
7. hdiutil verify
8. (optional) notarytool submit + staple   (skipped if NOTARY_API_KEY_* unset)
```

`AAA_MACDEPLOYQT` env var overrides the macdeployqt path
(auto-detected from `/opt/homebrew/opt/qt/bin/macdeployqt`).

## Architectures

**arm64 only.** Homebrew's `qt` formula ships arm64 frameworks ; an
Intel-compatible universal binary would require dual Qt builds. Intel
Macs run under Rosetta, which works in practice but is unsupported.

## Outputs

```
out/macos-arm64-release/bin/AAASeed-Studio.app   (built + macdeployqt'd)
out/macos-arm64-release/bin/aaaseed_runtime.app  (built + nested into Studio)
out/AAASeed-Studio-<version>.dmg                 (~54 MB ULMO/LZMA)
```

## Verifier

```
./scripts/verify-qt-dmg.sh out/AAASeed-Studio-*.dmg
```

Confirms (structural — NOT signature) :

1. `hdiutil verify` (checksum + filesystem integrity)
2. Mount read-only
3. `AAASeed Studio.app` exists
4. Studio executable is arm64 Mach-O
5. Qt frameworks bundled : `QtCore`, `QtGui`, `QtQml`, `QtQuick`, `QtMultimedia`
6. `qt.conf` landed in `Resources/`
7. `aaaseed_runtime.app` nested + starter project + `AAASeed.icns` present
8. Clean detach

Exit code is non-zero on any failure ; distinct codes per step (see
the script header).

## Signing + Notarization

Set these env vars before running `ship-qt-dmg.sh` :

| Var                            | Value                                              |
| ------------------------------ | -------------------------------------------------- |
| `CODESIGN_IDENTITY`            | "Developer ID Application: …"                      |
| `NOTARY_API_KEY_PATH`          | Path to the App Store Connect API .p8 key file     |
| `NOTARY_API_KEY_ID`            | Key ID                                             |
| `NOTARY_API_KEY_ISSUER_ID`     | Issuer UUID                                        |

Without these, the DMG is **ad-hoc signed** and Gatekeeper will
prompt the user to clear the quarantine bit
(`xattr -d com.apple.quarantine "/Applications/AAASeed Studio.app"`).

## CI integration

`.github/workflows/release.yml` runs the pipeline on `macos-14`
(Apple Silicon) when a `v*` tag is pushed. The verifier is then run
against the produced DMG before publishing the GitHub Release. See
the workflow file for the per-step environment.
