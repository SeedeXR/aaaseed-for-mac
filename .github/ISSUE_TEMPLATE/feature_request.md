---
name: Feature request
about: Propose a maintenance enhancement or v2+ direction for AAASeed for Mac.
title: "[FEATURE] <one-line summary>"
labels: ["enhancement", "triage"]
assignees: []
---

<!--
PROJECT-CLOSURE NOTE
====================
Per `memory/project_v1_ship_gate.md`, the v1 feature scope for AAASeed for Mac
is CLOSED. The project ships at 516/516 tests passing with a universal DMG.

This template is intended for ONE of:

  (a) Maintenance proposals — small, scoped enhancements that fit inside the
      existing doctrine (e.g. a new ctest cohort, a docs improvement, a CI
      hardening change). These may be accepted into `main`.

  (b) v2+ planning conversations — larger feature ideas that we will NOT ship
      under v1 but want to capture for a hypothetical successor release.

If you are reporting a bug, please use the bug-report template instead.
-->

## Use case

What problem are you trying to solve? Who benefits? Concrete user stories help:
"As a <designer / engine integrator / contributor>, I want to <X> so that <Y>."

## Proposed solution

Describe the change you would like to see. Include, where applicable:

- API surface (new Lua function names, signatures, expected return shapes).
- File/module touchpoints (e.g. `src/engine/aaa_app/...`, new shim header).
- Test cohort impact (unit / integration / regression / perf).
- Documentation impact (docs/designer, docs/developer, authoring-guide).

## Alternatives considered

What other approaches did you weigh, and why are they less attractive?

## Scope classification

Tick one:

- [ ] **(a) Maintenance** — fits inside v1 doctrine, no new memory/feedback note
      required. (Likely accepted if implementation is clean.)
- [ ] **(b) v2+ planning** — exceeds v1 closure ; captured for later. (Will be
      labelled `v2-candidate` and triaged on a slower cadence.)
- [ ] **(c) I'm not sure** — happy to defer to maintainers on classification.

## Doctrine check (for maintenance proposals only)

If you are proposing a (a) maintenance change, please indicate whether it
requires a new `memory/feedback_*.md` doctrine note (per the project's
"Doctrine limits" rule):

- [ ] No new doctrine required ; existing notes cover this change.
- [ ] New doctrine note required ; I have drafted (or will draft) a
      `memory/feedback_<topic>.md` alongside the PR.

## Additional context

Links, screenshots, prior-art references, or upstream-Windows behaviour notes
(if the feature mirrors something the Windows build already does).
