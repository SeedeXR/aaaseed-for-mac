# Contributing to AAASeed for Mac

Thanks for your interest. This project is the macOS-native port of
[AAASeed](https://aaaseed.org/). Contributions of every shape are welcome :
bug reports, build-fixes, ports, shader revivals, sample MEUs, doc edits.

Before you start, please read the [Code of Conduct](CODE_OF_CONDUCT.md) and
[Security policy](SECURITY.md).

---

## Quick links

| What | Where |
|---|---|
| Report a bug | [New issue → Bug report](https://github.com/SeedeXR/aaaseed-for-mac/issues/new?template=bug_report.md) |
| Request a feature | [New issue → Feature request](https://github.com/SeedeXR/aaaseed-for-mac/issues/new?template=feature_request.md) |
| Ask a question | [New issue → Question](https://github.com/SeedeXR/aaaseed-for-mac/issues/new?template=question.md) |
| Security disclosure | `a.mkwizu@seedexr.com` (see [SECURITY.md](SECURITY.md)) |
| Build instructions | [README → Build](README.md#build) |
| Architecture overview | [`docs/developer/architecture.md`](docs/developer/architecture.md) |
| Shipping checklist | [`SHIP_CHECKLIST.md`](SHIP_CHECKLIST.md) |

---

## Development environment

Prerequisites :

- **Hardware** : Apple Silicon (M1 / M2 / M3 / M4). The release is
  arm64-only ; Intel Macs may run under Rosetta but aren't a CI target
  (Homebrew Qt 6 is arm64-only).
- **macOS** : 13.0 (Ventura) or newer.
- **Xcode 15+** (Command Line Tools : `xcode-select --install`).
- **CMake 3.27+** (`brew install cmake`).
- **Ninja** (`brew install ninja`).
- **Qt 6** (`brew install qt`). 6.6+ ; we ship against 6.11.
- **Metal compiler** : `xcodebuild -downloadComponent MetalToolchain`.

Clone, configure, build :

```bash
git clone https://github.com/SeedeXR/aaaseed-for-mac.git
cd aaaseed-for-mac
cmake --preset macos-arm64-debug
cmake --build --preset macos-arm64-debug
ctest --preset macos-arm64-debug
```

Three configure presets are available : `macos-arm64-debug`,
`macos-arm64-release`, `macos-arm64-metal`. See
[README → Build](README.md#build) for the full table.

---

## Pull-request flow

1. **Fork** the repository and create a topic branch off `main` :
   `git checkout -b fix/short-description`.
2. **Build clean** locally :
   ```bash
   cmake --preset macos-arm64-debug
   cmake --build --preset macos-arm64-debug --target all
   ```
3. **All tests must pass** :
   ```bash
   ctest --preset macos-arm64-debug --output-on-failure
   ```
   The CI gate runs the `unit` and `perf` labels as blocking.
4. **Open a PR** against `main`. Fill out
   [`.github/pull_request_template.md`](.github/pull_request_template.md) ; in
   particular, describe the test plan and link to any related issue.
5. **CI must be green** on `macos-14` (Apple Silicon) before review. If a
   regression test or visual baseline fails, fix the cause rather than
   updating the baseline blindly.

---

## Code style

The Mac port follows the engine's existing conventions where they apply :

- **English-only** in all source files, doc comments, and `CLAUDE.md`
  briefs. Conversational chat with the original author can stay in
  French, but committed artefacts are English.
- **ASCII-only** inside `.cpp` / `.h` string literals and comments.
  Markdown is exempt. Replace em dashes with `--`, curly quotes with
  `'` / `"`, arrows with `->`, ellipses with `...`.
- **Type macros** over raw primitives in engine-adjacent code :
  `REAL`, `FP32`, `INT32`, `UINT32`, `INT64`, `FINLINE`. The Mac-native
  layer (`src/`) is allowed to use `std::` types directly.
- **Include guards** : `AAA_FILENAME_H` followed by the matching `#error`
  on double-include. See the engine `CLAUDE.md` for the exact pattern.
- **No dynamic allocation in real-time paths.** Prefer the
  `o_str::push_name()` / `pop_name()` pattern for temporary strings, the
  `SAFE_DELETE*` / `FREE_AND_NULL` helpers for owned heap pointers.
- **Doctrine over invention.** Where a `feedback_*.md` or
  `project_*.md` brief exists for the area you are touching, read it
  first. Land your change in a way that respects the recorded
  invariants.

The full engine style guide lives in `vendor/aaaseed-engine/CODE_STYLE.md`
and in the per-subsystem `CLAUDE.md` files alongside the C++ sources.

---

## Commit messages

- One topic per commit. A bug fix and a refactor are two commits.
- First line : 60 characters or fewer, present tense imperative.
- Body : wrap at 72, explain the WHY, link the issue (`Refs #N`,
  `Closes #N`). The diff already shows the WHAT.
- If the change came from an AI-assisted session, you may add a
  `Co-Authored-By:` trailer ; not required.

---

## Testing discipline

Every porting step must land with passing **unit + integration +
regression** suites before being merged. The labels are :

- `sanity` : toolchain-only smoke tests.
- `platform` : Apple Silicon platform-detection.
- `math` : vendored math subsystem.
- `unit` : per-module GTest suites (blocking).
- `perf` : timing budgets (blocking).
- `integration` : end-to-end MEU runs (best-effort on headless CI).
- `regression` : golden frame-buffer comparisons (best-effort on headless CI).

Headless `macos-14` runners cannot create on-screen Metal surfaces, so
any test needing `CAMetalLayer` is currently allowed to fail in CI but
must pass on a developer machine before the PR is merged.

---

## Reporting bugs

Open an issue using the
[Bug report template](https://github.com/SeedeXR/aaaseed-for-mac/issues/new?template=bug_report.md).
Include :

- macOS version : `sw_vers -productVersion`.
- Hardware : `uname -m` and `sysctl -n machdep.cpu.brand_string`.
- Repro steps, expected vs. actual.
- If the bug is in a downloaded DMG, attach the output of
  `./scripts/verify-qt-dmg.sh path/to/AAASeed-*.dmg`.

---

## Licensing of contributions

By submitting a pull request, you agree that your contribution is
licensed under the project's MIT License (see [LICENSE](LICENSE)) and
that you have the right to submit the work. No copyright assignment is
required ; you keep ownership of your commits, they are simply licensed
to the project and its downstream users under the same MIT terms as the
rest of the source.

---

## Maintainer

Alex Mkwizu (`a.mkwizu@seedexr.com`). Tag in issues with `@a-mkwizu`
for review requests.
