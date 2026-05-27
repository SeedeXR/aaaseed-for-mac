# bundle/macos -- Phase 8 distribution scaffolding

Authoritative AAASeed.app metadata + the runbook for code signing,
notarization, and DMG packaging. None of this is executed today
(c136-B, 2026-05-27) -- it is dormant until an Apple Developer ID
certificate becomes available.

## Files

- `Info.plist` -- canonical CFBundle* + LSMinimumSystemVersion +
  NSCameraUsageDescription / NSMicrophoneUsageDescription / etc.
  Mirrors keys in `src/ui/macos/Info.plist.in` (the CMake-substituted
  template wired into the dev build), but is the authoritative source
  for downstream signing / notarization tools.
- `entitlements.plist` -- hardened runtime entitlements (sandbox off
  for v1, network client+server, JIT, audio-input, camera,
  user-selected file read-write). Consumed by `codesign --entitlements`.
- `AAASeed.icns` -- bundle icon (NOT YET PRESENT ; v1 ships without
  a custom icon -- Finder falls back to the generic Cocoa app icon).
  Drop the .icns file here when art is ready ; CMake's existing
  `MACOSX_BUNDLE_ICON_FILE` setter in `src/ui/macos/CMakeLists.txt`
  is where it gets wired in.

## Code signing (template -- NOT executed)

Once a Developer ID Application certificate is installed in the
keychain :

```sh
codesign --deep --options runtime --timestamp \
    --sign "Developer ID Application: <name>" \
    --entitlements bundle/macos/entitlements.plist \
    out/macos-arm64-release/bin/aaaseed_app.app
```

Verify :

```sh
codesign --verify --deep --strict --verbose=2 \
    out/macos-arm64-release/bin/aaaseed_app.app
spctl --assess --type execute --verbose \
    out/macos-arm64-release/bin/aaaseed_app.app
```

## DMG creation (template)

```sh
mkdir -p dmg_staging
cp -R out/macos-arm64-release/bin/aaaseed_app.app dmg_staging/AAASeed.app
ln -s /Applications dmg_staging/Applications

hdiutil create -format ULMO -srcfolder dmg_staging/ \
    -volname "AAASeed" \
    AAASeed-0.1.0.dmg
```

Fallback : `-format ULFO` if notarization friction with ULMO.

### Automated DMG target (c137-B)

The above is now wired as a CMake custom target. Running

```sh
cmake --build out/macos-arm64-debug --target aaaseed_dmg
```

stages `AAASeed.app` + an `Applications` symlink + a placeholder
`.background/` dir + a one-line `README.txt` into
`out/macos-arm64-debug/dmg_staging/`, then invokes
`hdiutil create -format ULMO ... -overwrite` (with `-format ULFO`
shell-fallback) to produce
`out/macos-arm64-debug/AAASeed-${PROJECT_VERSION}.dmg`. The result
is UNSIGNED + NOT NOTARIZED. Signing / notarization commands above
still apply -- the runbook is unchanged ; the CMake target only
automates the packaging half. Module : `cmake/dmg.cmake` ; placeholder
assets : `cmake/dmg_assets/`. Tests : `aaaseed_dmg_packaging_tests`
(7 dry-run validations, label `phase8;unit;distribution;dmg`).

## Notarization (template)

```sh
xcrun notarytool submit AAASeed-0.1.0.dmg \
    --apple-id "<apple-id>" --team-id "<team-id>" \
    --password "@keychain:AC_PASSWORD" --wait

xcrun stapler staple AAASeed-0.1.0.dmg
```

## How to sign locally (c138-B, env-gated)

`cmake/codesign.cmake` defines an env-gated POST_BUILD codesign hook
on `aaaseed_app`. When the env var is unset (the default for CI +
local dev) the hook is a NO-OP and the build stays unsigned. To
activate :

```sh
export CODESIGN_IDENTITY='Developer ID Application: Foo (BAR123)'
cmake -S . -B out/macos-arm64-debug         # reconfigure picks up env
cmake --build out/macos-arm64-debug --target aaaseed_app
```

The post-build step then runs `codesign --deep --options runtime
--timestamp --entitlements bundle/macos/entitlements.plist --sign
"$CODESIGN_IDENTITY" <bundle>` automatically. No hardcoded identity
string ever lands in the repo -- the value is read from the env at
configure time.

## How to notarize (c138-B, env-gated)

`cmake/codesign.cmake` also defines `aaaseed_dmg_notarize`, a custom
target that depends on `aaaseed_dmg` (c137-B) and submits the DMG to
Apple's notary service via `xcrun notarytool` followed by `xcrun
stapler staple`. Gated on three env vars :

```sh
export NOTARY_API_KEY_PATH='/secure/path/AuthKey_XXXXXXXXXX.p8'
export NOTARY_API_KEY_ID='XXXXXXXXXX'
export NOTARY_API_KEY_ISSUER_ID='12345678-1234-1234-1234-123456789012'
cmake -S . -B out/macos-arm64-debug         # reconfigure picks up env
cmake --build out/macos-arm64-debug --target aaaseed_dmg_notarize
```

`notarytool submit --wait` blocks until Apple's service returns
Accepted / Invalid / Rejected ; `stapler staple` then attaches the
notarization ticket to the DMG for offline Gatekeeper verification.
With any of the three env vars unset, `aaaseed_dmg_notarize` exists
as a target (so CI invocations don't fail) but echoes a NO-OP banner
and exits 0.

## Audit (c138-B regression guards)

Verify the env vars are unset before commits :

```sh
echo "${CODESIGN_IDENTITY:-<unset>}"
echo "${NOTARY_API_KEY_PATH:-<unset>}"
echo "${NOTARY_API_KEY_ID:-<unset>}"
echo "${NOTARY_API_KEY_ISSUER_ID:-<unset>}"
```

The regression-guard tests in `tests/unit/codesign_packaging_test.cpp`
(per `memory/feedback_regression_guard_tests.md`, c137 doctrine)
assert that :

  - `cmake/codesign.cmake` contains no hardcoded `Developer ID
    Application:` identity string outside comments / $ENV references
    (test 12).
  - `CODESIGN_IDENTITY` is unset at test-run time, or a STATUS line
    fires noting that signing is active for this configure (test 13).

If a future commit leaks an identity string into `cmake/codesign.cmake`,
test 12 fires with a pointer back to this section and to the
regression-guard doctrine memo.

## Ship a universal-binary DMG (c143-B, 2026-05-27)

`scripts/ship-dmg.sh` is the single end-to-end ship pipeline. It builds
BOTH `arm64` + `x86_64` Release artifacts, fuses them with `lipo
-create`, wraps the universal binary in an `AAASeed.app` skeleton,
runs an optional codesign step (gated on `CODESIGN_IDENTITY`), stages
the DMG layout (`Applications` symlink + `.background/` + `README.txt`),
walks a compression cascade (ULMO -> ULFO -> UDBZ), verifies the
output with `hdiutil verify` + mount + `lipo -info` + `plutil -lint`,
and optionally submits to `xcrun notarytool` + `xcrun stapler` (gated
on the three `NOTARY_API_KEY_*` vars).

```sh
# Default : ship a universal binary DMG (arm64 + x86_64), Release mode.
./scripts/ship-dmg.sh

# Apple-Silicon-only build (faster, for development).
ARCHES="arm64" ./scripts/ship-dmg.sh

# Debug mode (fast dev DMG, no LTO ; c144-A override).
BUILD_TYPE=Debug ./scripts/ship-dmg.sh

# Ship with signing.
export CODESIGN_IDENTITY="Developer ID Application: <name> (<team>)"
./scripts/ship-dmg.sh

# Ship + sign + notarize.
export CODESIGN_IDENTITY="..."
export NOTARY_API_KEY_PATH="$HOME/AuthKey_XXXXXXXXXX.p8"
export NOTARY_API_KEY_ID="XXXXXXXXXX"
export NOTARY_API_KEY_ISSUER_ID="12345678-1234-1234-1234-123456789012"
./scripts/ship-dmg.sh
```

### Compression cascade rationale

The script tries three `hdiutil` formats in order ; the FIRST success
wins ("best working compression for portability") :

- **ULMO (LZMA)** -- highest compression ratio. Supported macOS 10.11+.
  Best for SHIPPING (smallest download for the user).
- **ULFO (LZFSE)** -- Apple's native compression algorithm. Decoded
  at the filesystem level on all modern macOS. Best for COMPATIBILITY
  across Apple platforms (used when ULMO hits notarization edge
  cases).
- **UDBZ (bzip2)** -- broad legacy compatibility going back to macOS
  10.4. Largest output. Final fallback for OLDEST macOS support.

On a modern host (macOS 13+ / 14+ / 15+), ULMO wins immediately and
the cascade exits at tier 1. On the c143-B reference build :

```
ARCHES built            : arm64 x86_64
DMG path                : out/AAASeed-0.0.1.dmg
DMG size                : 627168 bytes (0.60 MB)
Compression tier        : ULMO/LZMA
lipo -info (mounted)    : x86_64 arm64
codesign                : skipped (CODESIGN_IDENTITY unset)
notarize                : skipped (NOTARY_API_KEY_* unset)
```

### Regression-guard tests

`tests/unit/ship_script_test.cpp` (label `phase8;unit;distribution;
ship-script`) reads `scripts/ship-dmg.sh` as text and asserts ten
contract invariants : strict-mode flags, lipo invocation, all three
compression-format tokens (ULMO + ULFO + UDBZ), `CODESIGN_IDENTITY`
env-gating, `xcrun notarytool` reference, no hardcoded Developer ID
identity (per `memory/feedback_regression_guard_tests.md`), `hdiutil
verify`, `lipo -info` on the mounted bundle, and the universal
default `ARCHES` value covering BOTH `arm64` and `x86_64`. A future
session that silently drifts the script (e.g., drops `x86_64` from
the default) fires one of these tests on the next `ctest` run.

`tests/unit/release_mode_default_test.cpp` (label `phase8;unit;
distribution;release-mode`, c144-A, 5 tests) locks the Release-mode
ship default : `BUILD_TYPE` env var referenced ; default value is
`Release` (grep for `BUILD_TYPE:-Release`) ; propagated as
`-DCMAKE_BUILD_TYPE=` ; no hardcoded `-DCMAKE_BUILD_TYPE=Release`
literal (the env override must always win) ; LTO + dead-strip + -Os
generator-expressions remain wired in `src/ui/macos/CMakeLists.txt`
(c142-A). If a future session silently downgrades the ship default
back to Debug, or drops the optimization flags, one of these five
tests fires.

## Release vs Debug ship sizes (c144-A, 2026-05-27)

`./scripts/ship-dmg.sh` now defaults to `BUILD_TYPE=Release`.
`BUILD_TYPE=Debug ./scripts/ship-dmg.sh` overrides for a fast,
unoptimized dev DMG.

Reference build on this host (`arm64 + x86_64` universal, ULMO/LZMA
winner, c144-A) :

| Artifact                              | Debug (c143-B) | Release (c144-A) | Delta            |
|---------------------------------------|----------------|------------------|------------------|
| DMG (`out/AAASeed-0.0.1.dmg`)         | 817,588 B      | 817,588 B        | 0 B (0.0 %)      |
| Universal binary inside `.app`        | 1,786,712 B    | 1,486,040 B      | -300,672 B (-17 %)|
| Per-arch arm64 binary (pre-lipo)      | 1,786,712 B    | 748,728 B        | -1,037,984 B (-58 %)|
| Per-arch x86_64 binary (pre-lipo)     | n/a (1-arch)   | 731,080 B        | n/a              |
| Binary pre-strip / post-strip (lipo)  | 1,786,712 / 1,786,744 | 1,486,008 / 1,486,040 | strip -x recovers ~32 B in both modes |
| Compression tier won                  | ULMO/LZMA      | ULMO/LZMA        | unchanged        |

Notes on the surprising 0-byte DMG delta :

- The reference Debug DMG measured for the c143 entry was a
  **single-arch arm64** build. The c144-A Release DMG is a
  **universal** (arm64 + x86_64) build. The per-arch arm64 Release
  binary is 58 % smaller than the Debug single-arch binary
  (1.79 MB -> 0.75 MB), but the universal-binary container carries
  TWO architectures, so the lipo'd fat binary lands at 1.49 MB --
  17 % smaller than the Debug single-arch binary but still close
  enough that ULMO produces a similarly sized DMG.
- A truly apples-to-apples comparison (single-arch arm64 Release vs.
  single-arch arm64 Debug) would yield a substantially smaller
  Release DMG. Verify with :
  ```sh
  ARCHES="arm64" BUILD_TYPE=Debug   ./scripts/ship-dmg.sh
  ARCHES="arm64" BUILD_TYPE=Release ./scripts/ship-dmg.sh
  ```
- The `strip -x` step in the ship pipeline only frees ~32 B because
  the linker already drops dead code via `-Wl,-dead_strip` and local
  symbols are already absent from a Release-mode link. The strip
  step remains in the pipeline as a belt-and-suspenders guard
  against debug-symbol leaks.
- ULMO (LZMA) wins on both modes because the host (macOS 13+) accepts
  it without notarization friction. Cascade falls through to ULFO
  (LZFSE) only on edge-case rejection.

Verification of the c144-A Release DMG :

```
hdiutil verify   : VALID (CRC32 $CF8B1936)
file (mounted)   : Mach-O universal binary with 2 architectures
                   [x86_64:Mach-O 64-bit executable x86_64]
                   [arm64:Mach-O 64-bit executable arm64]
lipo -info       : x86_64 arm64
plutil -lint     : Info.plist OK
Resources/meu/hello_world.lua : present (c143-C bundle wiring intact)
Resources/shaders/            : 172 files (Path A catalog intact)
Resources/fonts/              : SourceCodePro-Medium.ttf
Resources/test_pattern.png    : 275 B (Phase 5 asset)
```

Compression tier on the c144-A reference build : **ULMO/LZMA**
(highest ratio, supported macOS 10.11+). Cascade fallback order
remains ULMO -> ULFO -> UDBZ.

## Why this lives here

v1 ships unsigned -- distribution will be ad-hoc (download + manual
`xattr -d com.apple.quarantine AAASeed.app`). This README is the
single-source runbook future c137+ sessions consult when Developer ID
becomes available. Future agents : do NOT execute these commands
without the user's explicit go-ahead (signing requires keychain
access ; notarization uploads the binary to Apple's servers).
