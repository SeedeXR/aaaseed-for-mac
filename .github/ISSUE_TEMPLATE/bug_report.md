---
name: Bug report
about: Report a defect in AAASeed for Mac (DMG build, runtime crash, test failure, regression).
title: "[BUG] <one-line summary>"
labels: ["bug", "triage"]
assignees: []
---

<!--
Thanks for filing a bug. AAASeed for Mac is the universal-binary port of the
upstream Windows engine. Before submitting, please confirm:

  1. You have the latest DMG from the Releases page (or you built from `main`).
  2. You ran `scripts/verify-dmg.sh` against the DMG (if applicable) and it
     reported "OK".
  3. You have checked SHIP_CHECKLIST.md and the docs site for known limitations
     (e.g. headless-Metal limitations in CI, unsigned/un-notarized binary).
-->

## Description

A clear and concise description of the bug. What did you expect, and what
actually happened?

## Steps to reproduce

1. Mount `AAASeed-X.Y.Z.dmg`...
2. Drag `AAASeed.app` to `/Applications`...
3. Run `xattr -d com.apple.quarantine /Applications/AAASeed.app`...
4. Launch and ...
5. See error.

If the bug surfaces only via `ctest`, list the exact command:

```
cd out/macos-arm64-debug
ctest -L unit --output-on-failure -R <test_name>
```

## Expected behaviour

What you expected to happen.

## Actual behaviour

What actually happened. Include screenshots or screen recordings if the bug is
visual.

## Environment

Please complete the following information:

- **macOS version**: (e.g., 14.5 Sonoma, 15.0 Sequoia)
- **Architecture**: (arm64 Apple Silicon / x86_64 Intel / Rosetta 2)
- **AAASeed DMG version**: (e.g., 0.0.1; from `Get Info` on AAASeed.app, or `defaults read /Applications/AAASeed.app/Contents/Info CFBundleShortVersionString`)
- **Source commit** (if built locally): `git rev-parse HEAD` output
- **Build type**: (Release from DMG / local Debug build / local Release build)
- **GPU**: (e.g., Apple M1, M2 Pro, Intel UHD 630, AMD Radeon Pro 5500M)

## Logs

Please attach or paste relevant logs. Common sources:

- **NSLog / Console.app** — filter by process name `aaaseed_app`. Copy any lines
  in the 30 seconds surrounding the failure.
- **ctest output** — `ctest -L <label> --output-on-failure` for failing tests.
- **Crash report** — `~/Library/Logs/DiagnosticReports/aaaseed_app-*.ips` (most
  recent file).
- **Metal validation** — if you ran with `METAL_DEVICE_WRAPPER_TYPE=1` or
  `MTL_DEBUG_LAYER=1`, paste the validator messages.

```
<paste logs here ; trim to the relevant ~50 lines>
```

## Doctrine touchpoints (optional, for contributors)

If this bug touches one of the documented port doctrines, link it:

- [ ] Hermetic Mac sub-libs (Win32-only files)
- [ ] Shim header shadow (vendor header leak)
- [ ] Weak-symbol stubs
- [ ] Path A revival pattern
- [ ] Distnoted dual-center
- [ ] Other / unsure

## Additional context

Any other context, links to related issues, or hypotheses about root cause.
