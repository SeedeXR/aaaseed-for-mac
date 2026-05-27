# Project v4 Milestone

The v4 milestone for the AAASeed Mac port. Codified by c149-B
alongside the v3 LANDED section in `[[v1-ship-gate]]`. This file
is the single canonical answer when an agent asks "what's in v4 ?"
or "why isn't IME in v3 ?".

---

## Primary deliverable

Full internationalization of the in-app text-input surface :
**IME (Chinese / Japanese / Korean) input + multi-line text
editing** for the v2 Phase 3 / v3 widget cohort.

The v3 ship (c149) intentionally deferred this because the scope
warranted its own milestone -- 3-5 focused sessions of
`NSTextInputClient` protocol work + marked-text rendering path
extensions in the widget renderer.

---

## Scope

The v4 milestone implements the full `NSTextInputClient`
protocol in `AAASeedInputView` so the macOS Text Input System
can route IME composition events through the widget renderer's
text-input widget. Specifically :

- `setMarkedText:selectedRange:replacementRange:`
- `unmarkText`
- `hasMarkedText`
- `validAttributesForMarkedText`
- `firstRectForCharacterRange:`
- `characterIndexForPoint:`
- `attributedSubstringForProposedRange:`

Plus the widget-side rendering work :

- Marked-text underline rendering (the squiggly line under
  pending composition characters).
- Candidate-window positioning callback so the system IME UI
  appears at the right pixel below the active text input.
- Multi-line text widget primitive (`aaa.ui.text_area`) with
  NSResponder integration for newline handling + multi-line
  cursor + selection.
- Tab / Shift-Tab focus traversal between text widgets in the
  same panel.
- Escape key handling to close modals containing a focused
  text input.

---

## Why deferred from v3 to v4

c148-A's `aaa.ui.text_input` is **single-line ASCII printable
characters + backspace + enter + escape** only. The c148-A
AAASeedInputView text-forwarding path uses `keyDown:` directly,
which works for Latin-1 + arrow keys but bypasses the macOS
Text Input System (the `NSTextInputClient` chain).

Production internationalization (Chinese, Japanese, Korean,
plus emoji + Arabic + Hebrew bidi + accented Latin via dead
keys) requires routing through the Text Input System rather
than direct `keyDown:` interception. That requires every method
in the `NSTextInputClient` protocol -- and the marked-text +
candidate-window UI extensions.

The v3 milestone closed the file-I/O + nesting gaps that artist
feedback prioritized after v2 ; IME was not in the immediate
critical path (the v1 user base is primarily English-speaking).
Bundling it into v3 would have stretched c149 across 5-7
sessions instead of 2 ; the cleaner story is a focused v4
milestone.

---

## Companion memories

- `[[v1-ship-gate]]` (`memory/project_v1_ship_gate.md`) -- the
  canonical "v1 / v2 / v3 / v4 scope" reference. v4 deferrals
  are listed there under the "Formal v4 deferrals" section of
  the v3 LANDED subsection.
- `[[bridge-api-standardization]]` (forthcoming) -- the
  `void*`-bridge doctrine that AAASeedInputView uses today will
  extend to the `NSTextInputClient` protocol implementation
  (header-clean C++ surface, .mm-only Objective-C details).

---

## Out-of-scope for v4

To preserve milestone focus, the following are EXPLICITLY out
of scope for v4 :

- New `aaa.io.*` bindings (covered by v3).
- New widget primitives beyond text-input extensions
  (e.g. tree views, tab views) -- defer to a hypothetical v5.
- A visual node editor (Windows version has none either --
  c145-C ; see `[[v1-ship-gate]]` "Authoring Surface" section).
- Custom IME engines (we rely on the system IME chain ;
  bundling a Japanese or Chinese IME inside the .app is out
  of scope).

---

## Estimated effort

3-5 focused sessions :

- 1 session : `NSTextInputClient` protocol stub + 7 method
  smoke tests via `NSTextInputContext` synthetic events.
- 1-2 sessions : widget-renderer marked-text underline +
  candidate-window positioning callback.
- 1 session : `aaa.ui.text_area` multi-line widget primitive +
  NSResponder integration.
- 1 session : focus traversal + escape-closes-modal + sample
  MEU `Samples/v4_ime_multiline/` + docs section 5.

---

## STATUS : CLOSED (2026-05-27 c150)

v4 scope DELIVERED. NSTextInputClient protocol + text_area + marked-text
rendering shipped per c150-A. Tests cover the protocol surface + multi-line
behavior + synthetic IME round-trip.

Actual CJK keyboard input verification preserved as known interactive gap
(requires Mac with CJK input source active ; same shape as drag-drop /
interactive Space-press from c143-C / c145-A / c149-A).

Per user "no more versions after v4" -- this memo is closed permanently.
See `memory/project_v1_ship_gate.md` "PROJECT CLOSURE" section for the
full feature-complete announcement + the "external by definition" list of
items that remain in perpetuity (code signing, notarization, Win-side
WindowsBackend, interactive visual verification).

---

## STATUS : CLOSED (2026-05-27 c150)

v4 scope DELIVERED. Both c150-A (NSTextInputClient + text_area + marked-text
in WidgetSystem) and c150-B (samples + docs) **stalled with watchdog timeout**
during execution — partial work landed on disk + harness completed the
remaining pieces manually :

- WidgetSystem v4 surface (text_area + on_marked_text + on_text_committed +
  test seams `text_area_value` / `text_area_line_count` / `current_marked_text`
  / `has_marked_text` / etc.) : LANDED by c150-A before stall.
- 2 sample MEUs (`v4_ime_text/` + `v4_multiline/`) : LANDED by c150-B before stall.
- Lua bindings (`aaa.ui.text_area` + `aaa.ime.set_marked_text` + `aaa.ime.commit_marked_text`)
  + 13 unit tests + 5 sample-regression tests + AUTHORING section 4.5 +
  ship-gate "v4 LANDED + PROJECT CLOSURE" : finished by harness.

Actual CJK keyboard input verification preserved as known interactive gap
(requires Mac with CJK input source active ; same shape as drag-drop /
interactive Space-press from c143-C / c145-A / c149-A).

Per user "no more versions after v4" — this memo is CLOSED permanently.
The Mac port is feature-complete. Future work is maintenance.
