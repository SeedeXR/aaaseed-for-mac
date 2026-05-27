# AAASeed for Mac -- Ship Checklist (v0.0.1)

User-facing summary of what is in the DMG you just received, what is
verified, and what is intentionally external. If you are the developer,
read `bundle/macos/README.md` instead -- this file is for the person
who clicks the DMG link in an email.

Build date : 2026-05-27. Continuation tag : c145.

---

## 1. What is in this DMG

- Single artifact : `AAASeed-0.0.1.dmg`, approximately 800 KB.
- One application bundle : `AAASeed.app`.
- One universal binary inside : `aaaseed_app` (Apple Silicon `arm64`
  AND Intel `x86_64` in a single Mach-O).
- 11 real-algorithm shader effects (Path A catalog revivals), each
  rendered as a `.metal` source file shipped in
  `Contents/Resources/shaders/`.
- A MEU Lua runner with one example script
  (`Contents/Resources/meu/hello_world.lua`).
- A bundled monospace font for the on-screen HUD
  (`Contents/Resources/fonts/SourceCodePro-Medium.ttf`).
- A single test-pattern PNG asset.

Honest scope : this is a v1 graphics-runtime preview. It renders shader
effects, runs a Lua script at startup, and shows a small text HUD. It
is not yet a full application with menus, settings, or persistence.

## 2. System requirements

- macOS 13.0 (Ventura) or newer.
  Pinned by `LSMinimumSystemVersion` in the bundle `Info.plist`.
- Either an Apple Silicon Mac (M1 / M2 / M3 / M4 family) OR an
  Intel Mac (x86_64). The binary is universal -- it runs natively on
  both, no Rosetta required.
- Approximately 5 MB of free disk space after installation.

## 3. How to install (drag-and-drop)

1. Double-click `AAASeed-0.0.1.dmg`. The DMG mounts as a Finder volume
   called `AAASeed-0.0.1`.
2. Drag `AAASeed.app` onto the `Applications` shortcut that appears
   alongside it in the mounted volume.
3. Eject the DMG (right-click the volume in the Finder sidebar ->
   Eject).
4. First launch requires clearing the quarantine attribute, because
   the build is unsigned (see section 5 for the why). Open Terminal
   and run :
   ```
   xattr -d com.apple.quarantine /Applications/AAASeed.app
   ```
   You only need to do this once.
5. Double-click `AAASeed.app` in `/Applications` to launch.

## 4. What you should see when it launches

- A 1280 by 720 application window opens.
- The window shows a rendered Path A shader effect (animated).
- Press the Space key to cycle through the 11 available shader effects.
- In one corner of the window a small HUD overlay shows the active
  shader name plus a running frame counter (added in continuation
  c145).

If you see a black window or a "damaged and cannot be opened" dialog,
step 4 of section 3 was skipped -- re-run the `xattr` command.

## 5. Code signing status (honest)

This DMG is **unsigned**. macOS Gatekeeper will mark it quarantined on
first launch, which is the reason for the `xattr -d com.apple.quarantine`
step in section 3.

To produce a signed build locally, set the `CODESIGN_IDENTITY`
environment variable to your Developer ID Application identity and
re-run `./scripts/ship-dmg.sh`. Details and the full runbook live in
`bundle/macos/README.md`. The signing identity is never hardcoded in
the repository -- it is read from the environment.

## 6. Notarization status (honest)

This DMG is **not notarized**. Notarization requires an Apple Developer
Program membership plus an App Store Connect API key. The ship script
(`scripts/ship-dmg.sh`) has a notarization step that activates when the
`NOTARY_API_KEY_PATH`, `NOTARY_API_KEY_ID`, and `NOTARY_API_KEY_ISSUER_ID`
environment variables are all set. Until those are configured, the DMG
ships unstapled.

## 7. Verify your DMG before installing

A standalone verification script is included in the source tree :
```
./scripts/verify-dmg.sh path/to/AAASeed-0.0.1.dmg
```
It runs `hdiutil verify`, mounts the DMG read-only, checks that the
`.app` and the `Applications` symlink are present, confirms the
executable is a Mach-O universal binary covering both `arm64` and
`x86_64`, lints the `Info.plist`, confirms the MEU Lua bundle is
present, confirms the shader catalog is intact, then unmounts.

The script exits 0 on success. Any structural problem produces a
distinct non-zero exit code so you can tell exactly what failed. Pass
`--quiet` to suppress progress output (errors still print).

This is structural verification, not signature verification. If you
need to confirm the DMG came from a specific signer, that is a
separate `codesign --verify` and `spctl --assess` step -- not in
scope for `verify-dmg.sh`.

## 8. Architecture support

The binary is a Mach-O universal binary. You can verify the slice list
yourself once the app is installed :
```
lipo -info /Applications/AAASeed.app/Contents/MacOS/aaaseed_app
```
Expected output : `Architectures in the fat file: ... are: x86_64 arm64`
(order may vary).

The same check is performed automatically by `scripts/verify-dmg.sh`
on the mounted DMG before installation.

## 9. Test counts (transparency)

- 416 automated tests pass at build time (gtest, ctest).
- 11 real-algorithm shader revivals, each with a visual regression
  proof captured during the Path A revival sessions.
- Cross-platform interface (`.deproj` round-trip, header parity, etc.)
  verified clean.
- This v1 ship has been built and verified on macOS 13+ hosts. The
  ULMO/LZMA compression tier won on the reference build ; ULFO and
  UDBZ are available as fallbacks if a future host rejects ULMO.

## 10. What is NOT in this v1

- Code signing -- needs a Developer ID Application certificate.
- Notarization -- needs an App Store Connect API key.
- Full BDD geometry rendering pipeline -- deferred to v2.
- The Windows counterpart -- built in a separate
  Windows-machine session per `docs/windows-backend-howto.md`.
- Sparkle auto-update -- planned for v1.1.
- A custom application icon -- v1 falls back to the generic Cocoa
  application icon ; drop a `bundle/macos/AAASeed.icns` and rebuild
  to wire one in.
- Persistent settings, application menus beyond the default Cocoa
  shell, file open dialogs, or a built-in MEU editor.

## 11. Reporting issues

File an issue at the project repository. Include :
- macOS version (`sw_vers -productVersion`).
- Hardware (`uname -m` and `sysctl -n machdep.cpu.brand_string`).
- The output of `./scripts/verify-dmg.sh` on your downloaded copy.
- A short description of what you expected versus what you saw.

If you cannot run the app at all after step 4 of section 3, attach
the output of :
```
codesign -dv --verbose=4 /Applications/AAASeed.app 2>&1
spctl --assess --type execute --verbose /Applications/AAASeed.app 2>&1
```
These two commands explain most first-launch failures.
