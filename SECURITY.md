# Security Policy

## Supported versions

AAASeed for Mac is pre-1.0. Only the latest released version is
supported with security fixes. Older tagged releases are kept on the
GitHub Releases page for reference but will not receive backported
patches.

| Version          | Supported |
|------------------|-----------|
| `main` (HEAD)    | yes       |
| latest tagged    | yes       |
| older tagged     | no        |

## Reporting a vulnerability

Please **do not** open a public GitHub issue for security
vulnerabilities. Instead, report them privately so a fix can be
prepared and released before details become public.

- **Preferred** : email **`a.mkwizu@seedexr.com`** with the subject
  prefix `[AAASeed-Mac security]`.
- **Alternative** : use GitHub's
  [private vulnerability reporting](https://github.com/SeedeXR/aaaseed-for-mac/security/advisories/new)
  on the repository's Security tab.

Please include, where applicable :

- A short description of the issue.
- The affected version, commit, or release tag.
- Reproduction steps or a minimal proof-of-concept.
- Your assessment of impact (data exposure, code execution, denial of
  service, etc.).
- Whether you would like to be credited in the release notes.

## What to expect

- **Acknowledgement** : within 5 business days of your report.
- **Triage update** : within 14 days, with a severity assessment and
  a target window for the fix.
- **Fix and disclosure** : depending on severity, typically within
  30 to 90 days. Coordinated disclosure is preferred ; a CVE will be
  requested for issues that warrant one.

## Scope

In scope :

- The Mac-native code under `src/` (Cocoa host, Metal backend,
  IPC bridges, MEU runner integration).
- The build and ship scripts under `scripts/` (DMG packaging,
  verification, code-sign and notarisation hooks).
- The release workflow under `.github/workflows/release.yml`.
- Vendored code under `vendor/` only to the extent that the Mac port
  exposes the issue. Upstream-only vulnerabilities should be
  reported to the upstream AAASeed project.

Out of scope :

- Issues that require physical or root access to the user's machine
  to exploit.
- Reports against unsupported macOS versions (anything older than
  the `LSMinimumSystemVersion` declared in `bundle/macos/Info.plist.in`).
- Findings on third-party services not operated by the project
  (GitHub, Homebrew, Apple developer infrastructure).

## Hardening notes

- Releases ship **unsigned and un-notarised** by default. Users are
  expected to clear the quarantine attribute on first launch ; see
  [`SHIP_CHECKLIST.md`](SHIP_CHECKLIST.md) sections 5 and 6.
- The release workflow signs only when `CODESIGN_IDENTITY` and the
  `NOTARY_API_KEY_*` env vars are provided ; secrets must be supplied
  via GitHub Actions secrets, never committed.
- The `.gitignore` blocks common credential file extensions
  (`*.p8`, `*.p12`, `*.cer`, `*.mobileprovision`,
  `NotaryAPIKey.json`). If you discover a leaked credential in the
  repository or its history, treat it as a security issue and report
  it through the channels above.
