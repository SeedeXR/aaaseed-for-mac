# Installation

> Audience : designers installing the `AAASeed-Studio-<version>.dmg`
> artifact onto a Mac. No build tools required. Most steps are
> drag-and-drop ; the only Terminal command is the one-time
> Gatekeeper rinse.

---

## System requirements

- **macOS 13.0 (Ventura) or newer.** Pinned by `LSMinimumSystemVersion`
  in the bundle's `Info.plist`. Older macOS will refuse to launch with
  a clear OS-level dialog.
- **Apple Silicon (M1 / M2 / M3 / M4 family).** Homebrew Qt 6 is
  arm64-only ; Intel Macs can run under Rosetta but are not a tested
  CI target.
- **About 150 MB free disk** after install (Qt frameworks make up most
  of it). The DMG itself is ~52 MB.
- **A Metal-capable GPU.** Any Mac that runs macOS 13 qualifies.

---

## Install in 4 drag-and-drop steps

### Step 1 — mount the DMG

Double-click `AAASeed-Studio-<version>.dmg` (typically in
`~/Downloads`). Finder mounts it and opens a window showing two
icons : **AAASeed Studio.app** and a shortcut to `/Applications`.

### Step 2 — drag the app to Applications

Drag **AAASeed Studio.app** from the mounted DMG onto the
`Applications` shortcut. Finder copies the bundle to
`/Applications/AAASeed Studio.app`.

### Step 3 — eject the DMG

Right-click the mounted volume in the Finder sidebar and choose
**Eject**. The DMG is no longer needed.

### Step 4 — clear the Gatekeeper quarantine flag (one time only)

The DMG is **unsigned + unnotarized** (see "Signing status" below).
macOS attaches a quarantine attribute on download ; you need to remove
it once before first launch. Open **Terminal.app** and paste :

```
xattr -d com.apple.quarantine "/Applications/AAASeed Studio.app"
```

You only do this once. Subsequent launches just work.

### Step 5 — launch

Double-click **AAASeed Studio** in `/Applications`. You should see :

- The Studio's **Home screen** with project tiles (empty list on
  first launch).
- Big primary actions : **✦ New Project** · **Open Project…** ·
  **Open Sample**.
- Native macOS menubar : **File · Edit · Run · View · Window**.

If you instead see a **"is damaged and cannot be opened"** dialog,
step 4 was skipped — run the `xattr` command and try again.

---

## Verify your DMG before installing (optional but recommended)

```
./scripts/verify-qt-dmg.sh /path/to/AAASeed-Studio-<version>.dmg
```

It runs `hdiutil verify`, mounts the DMG read-only, then confirms :

- `AAASeed Studio.app` is present
- Studio binary is an arm64 Mach-O
- All required Qt frameworks bundled (QtCore, QtGui, QtQml, QtQuick,
  QtMultimedia)
- macdeployqt's `qt.conf` landed in `Resources/`
- Nested `aaaseed_runtime.app` is bundled (engine playback helper)
- Starter project + icon are present
- Clean detach

Exit code 0 = pass. Distinct non-zero codes per check.

---

## Verify architecture (optional)

```
lipo -archs "/Applications/AAASeed Studio.app/Contents/MacOS/AAASeed-Studio"
```

Expected output : `arm64`.

---

## Signing status (honest)

The shipped DMG is **unsigned** — no Developer ID signature embedded
in the Mach-O. This is why Gatekeeper marks it quarantined on first
launch and why you have to run the `xattr` command in step 4.

If you build from source and want a signed bundle, set
`CODESIGN_IDENTITY` to your Developer ID Application identity before
running `scripts/ship-qt-dmg.sh`. The signing identity is read from
the environment ; never hardcoded.

The DMG is **not notarized** either. Notarization requires an Apple
Developer Program membership + an App Store Connect API key. The
ship script has a notarization step that activates when
`NOTARY_API_KEY_PATH`, `NOTARY_API_KEY_ID`, and
`NOTARY_API_KEY_ISSUER_ID` are all set.

---

## Troubleshooting

**"AAASeed Studio is damaged and cannot be opened"** — Gatekeeper
quarantine. Run :

```
xattr -d com.apple.quarantine "/Applications/AAASeed Studio.app"
```

**Home screen never appears.** The QML scene failed to load. Open
`Console.app`, filter for `AAASeed-Studio`, and file an issue with
the first QML error line.

**Play (Cmd+P) doesn't open the runtime.** The Studio expects
`aaaseed_runtime.app` nested in its Resources. Re-run
`verify-qt-dmg.sh` against your DMG ; if the verifier confirms the
runtime is bundled but Play still fails, file an issue with the
Console panel output.

**"You can't open the application because it is not supported on this
type of Mac"** — your macOS version is older than 13.0 (Ventura), or
you're on Intel without Rosetta installed.

---

## Uninstall

Drag `/Applications/AAASeed Studio.app` to the Trash. App settings
(workspace layout, recent projects, prefs) live under
`~/Library/Preferences/com.seedexr.aaaseed.plist` — delete that too if
you want a clean slate.

---

## Next steps

- [Getting Started](getting-started.md) — your first project in 5 minutes.
- [Sample MEUs](samples.md) — starter scripts to crib from.
