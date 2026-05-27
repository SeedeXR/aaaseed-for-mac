# Installation

> Audience : designers installing the `AAASeed-0.0.1.dmg` artifact onto
> a Mac. No build tools required. Most steps are drag-and-drop ; the
> only Terminal command is the one-time Gatekeeper rinse.

---

## System requirements

- **macOS 13.0 (Ventura) or newer.** This is pinned by
  `LSMinimumSystemVersion` inside the bundle's `Info.plist`. Older
  macOS versions will refuse to launch with a clear OS-level dialog.
- **Apple Silicon (M1 / M2 / M3 / M4 family) OR Intel x86_64.** The
  binary inside the .app is universal -- a single Mach-O containing
  both arm64 and x86_64 slices. No Rosetta required.
- **About 5 MB free disk** after install. The shipped DMG is roughly
  685 KB compressed.
- **A Metal-capable GPU.** Any Mac that runs macOS 13 qualifies ; this
  is not a high-end requirement.

---

## Install in 4 drag-and-drop steps

### Step 1 -- mount the DMG

Double-click `AAASeed-0.0.1.dmg` (typically in your `~/Downloads`).
Finder mounts it as a volume named `AAASeed-0.0.1` and opens a window
showing two icons : `AAASeed.app` and a shortcut to `/Applications`.

<!-- screenshot: dmg-mounted.png -->

### Step 2 -- drag the app to Applications

Drag `AAASeed.app` from the mounted DMG onto the `Applications`
shortcut. Finder copies the bundle to `/Applications/AAASeed.app`.

<!-- screenshot: dmg-drag-to-applications.png -->

### Step 3 -- eject the DMG

Right-click the `AAASeed-0.0.1` volume in the Finder sidebar and choose
**Eject**. The DMG is no longer needed -- you can delete it from
`~/Downloads` if you like.

### Step 4 -- clear the Gatekeeper quarantine flag (one time only)

This DMG is **unsigned + unnotarized** (see "Signing status" below for
why and how to change that). macOS attaches a quarantine attribute on
download ; you need to remove it once before first launch. Open
**Terminal.app** and paste :

```
xattr -d com.apple.quarantine /Applications/AAASeed.app
```

You only do this once. Subsequent launches just work.

### Step 5 -- launch

Double-click `AAASeed.app` in `/Applications`. You should see :

- A 1280 by 720 window opens.
- A Path A shader effect renders inside (animated by default).
- A HUD overlay in the top-left corner shows the active shader name +
  a running frame counter.

If you instead see a **"AAASeed.app is damaged and cannot be opened"**
dialog, step 4 was skipped -- run the `xattr` command and try again.

If you see a **black window**, see "Troubleshooting" below.

<!-- screenshot: first-launch-curl-noise.png -->

---

## Verify your DMG before installing (optional but recommended)

A standalone verification script ships in the source tree :

```
./scripts/verify-dmg.sh /path/to/AAASeed-0.0.1.dmg
```

It runs `hdiutil verify`, mounts the DMG read-only, checks that the
`.app` and the `Applications` symlink are present, confirms the
executable is a universal Mach-O covering both arm64 and x86_64, lints
the `Info.plist`, confirms the MEU Lua bundle is intact, confirms the
169-shader Path A catalog is intact, then unmounts.

Exit code 0 = pass. Any non-zero exit + a diagnostic line tells you
which check failed.

---

## Verify universal-binary coverage (optional)

Once installed, confirm the binary covers your CPU :

```
lipo -info /Applications/AAASeed.app/Contents/MacOS/aaaseed_app
```

Expected output :

```
Architectures in the fat file: /Applications/AAASeed.app/Contents/MacOS/aaaseed_app are: x86_64 arm64
```

If you see only one architecture, you have a non-universal build --
report the issue and switch to the official DMG.

---

## Signing status (honest)

The shipped DMG is **unsigned** -- no Developer ID signature embedded
in the Mach-O. This is why Gatekeeper marks it quarantined on first
launch and why you have to run the `xattr` command in step 4.

If you build from source and want a signed bundle, set the
`CODESIGN_IDENTITY` environment variable to your Developer ID
Application identity before running `scripts/ship-dmg.sh`. Details and
the full runbook live in `bundle/macos/README.md`. The signing
identity is never hardcoded in the repository -- it is read from the
environment so the same scripts work for any team member.

The DMG is **not notarized** either. Notarization requires an Apple
Developer Program membership + an App Store Connect API key. The ship
script has a notarization step that activates when
`NOTARY_API_KEY_PATH`, `NOTARY_API_KEY_ID`, and
`NOTARY_API_KEY_ISSUER_ID` are all set. Until those are configured,
the DMG ships unstapled.

---

## Troubleshooting

**"AAASeed.app is damaged and cannot be opened"** -- Gatekeeper
quarantine. Run :

```
xattr -d com.apple.quarantine /Applications/AAASeed.app
```

**Black window on launch.** The Path A shader catalog or MEU script
may be missing from the bundle. Re-run `scripts/verify-dmg.sh` on
your DMG ; if that passes but you still see black, file an issue with
the output of `Console.app` filtered to `aaaseed_app`.

**"You can't open the application because it is not supported on this
type of Mac"** -- your macOS version is older than 13.0 (Ventura).
Upgrade macOS or use a Mac that meets the system requirements.

**App launches but no HUD text shows.** The HUD requires the bundled
font at `Contents/Resources/fonts/SourceCodePro-Medium.ttf`. Check
that the font file is present in the bundle ; if not, your DMG is
corrupt -- re-download and re-verify.

**`lipo -info` reports only one architecture.** You have a
non-universal build. Use the official DMG or build with both arm64 +
x86_64 active in your CMake configuration.

---

## Uninstall

Drag `/Applications/AAASeed.app` to the Trash and empty the Trash.
There are no preference files, no LaunchAgents, no caches written
outside the bundle in v1. The app is fully self-contained.

---

## Next steps

- [Getting Started](getting-started.md) -- your first MEU in 5 minutes.
- [Sample MEUs](samples.md) -- 14 starter scripts to crib from.
