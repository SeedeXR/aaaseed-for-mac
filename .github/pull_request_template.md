<!--
Thanks for the PR. AAASeed for Mac is the universal-binary port of the upstream
Windows engine, currently shipping v1 at 516/516 tests passing.

This template captures the doctrine + verification expectations from
`memory/project_v1_ship_gate.md`. Please fill out every section ; leave a brief
"N/A" where a section genuinely does not apply.
-->

## Summary

<!-- 1-3 sentences. What does this PR do, at a high level? -->

## Motivation / linked issues

<!--
Why is this change being made? Link the issue (Fixes #123 / Closes #456 /
Refs #789). If there is no issue, briefly explain the trigger.
-->

- Fixes #
- Refs #

## Changes

<!-- Bullet list of files touched, grouped by subsystem. Highlight any new files. -->

- `src/...`
- `tests/unit/...`
- `cmake/...`
- `docs/...` (if any ; coordinate with c151-A / c151-B owners)
- `memory/feedback_*.md` (if a new doctrine note was authored)

## Test plan

<!--
Which ctest cohorts did you run, and what was the delta vs `main`?
Cohorts in this project: unit, integration, regression, perf.
Expected baseline at the time of PR creation: 516/516 unit tests pass.
-->

- [ ] `ctest -L unit --output-on-failure` — local result: `___/___` pass
- [ ] `ctest -L perf --output-on-failure` — local result: `___/___` pass
- [ ] `ctest -L integration --output-on-failure` — local result: `___/___` pass (non-blocking, may need windowed Metal context)
- [ ] `ctest -L regression --output-on-failure` — local result: `___/___` pass (non-blocking, headless-Metal caveat)

Expected delta vs `main`:

<!--
e.g. "+3 unit tests added for foo_bar ; no regressions in other cohorts."
"net-zero test count ; behaviour-preserving refactor only."
-->

## Doctrine check

<!--
Per `memory/project_v1_ship_gate.md`, ANY change that introduces a new pattern
the project will repeat MUST also land a `memory/feedback_<topic>.md` note
documenting it. Tick the relevant box:
-->

- [ ] No new doctrine required ; this change fits inside an existing
      `memory/feedback_*.md` note. (List which: __________)
- [ ] New doctrine note added in this PR: `memory/feedback_<topic>.md`
- [ ] New doctrine note required but deferred to follow-up PR (justify below)

## Honest-gaps disclosure

<!--
The project keeps an explicit list of "interactive-verification gaps" — tests
or code paths that cannot be exercised in headless CI (e.g. windowed Metal
surfaces, on-screen NSWindow flows, NSWorkspace dialog flows). If your change
preserves an existing gap, or introduces a new one, list it here. Be honest;
silent gaps are the failure mode this section exists to prevent.
-->

- [ ] No interactive-verification gaps preserved or introduced.
- [ ] Existing gap preserved (which: __________). Rationale: __________
- [ ] New gap introduced (which: __________). Mitigation / follow-up plan: __________

## Checklist

- [ ] `ctest -L unit` is green locally.
- [ ] `scripts/verify-dmg.sh` passes against the locally built DMG (if this PR
      touches the build, packaging, or runtime).
- [ ] `scripts/ship-dmg.sh` succeeds end-to-end (if this PR touches packaging).
- [ ] Documentation updated where user-facing behaviour changed
      (`docs/designer/*` for designer-facing changes, `docs/developer/*` for
      contributor-facing changes — coordinate with c151-A / c151-B owners,
      do not edit those trees in this PR if scope overlaps).
- [ ] Authoring-guide / Lua API reference updated if any Lua function
      signature, return shape, or semantics changed.
- [ ] No NVIDIA / CUDA code reintroduced ; any upstream Win32 file with such
      references substitutes the Apple framework equivalent
      (per `feedback_nvidia_to_metal.md`).
- [ ] All new Win32-only ports use the hermetic-Mac-sub-lib pattern
      (per `feedback_hermetic_mac_sublibs.md`).
- [ ] Commit messages are descriptive and reference the issue / doctrine note.

## Reviewer notes

<!-- Anything the reviewer should look at first, or known follow-ups. -->
