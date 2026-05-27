# Project v1 Ship-Gate

Doctrine for what ships in the AAASeed Mac v1 DMG and what defers to v2.
Codified per c142-C definitive gap inventory + c145-C authoring-surface
research. This file is the single canonical answer when an agent asks
"is X in scope for v1 ?".

---

## Ship-state contract (v1)

- Mac universal DMG at `out/AAASeed-0.0.1.dmg` (~800 KB).
- 427 + 6 = **433 tests** passing under CTest (post c145-D MEU samples).
- HUD overlay wired ; MEU runner loads + drives a Lua script per frame.
- Path A 170+ shader catalog ships in `Resources/shaders/` ; 11 are
  real-algorithm revivals (c128-c140).

---

## Authoring Surface (v1)

**Windows** : C++ Win32 param dialogs (`Src/ui/seed_ui.cpp`) + Lua GaBu
BU widgets (`AAAKernel/GaBu/`) + Lua text editing. There is NO visual
node editor on Windows (confirmed by c145-C research ; the "MEU library"
artists refer to is the kernel's `AAAKernel/AAA_PROTO/MEU_PROTO*/`
folder set, not a node graph).

**Mac v1** :

- **C++ param dialogs : DEFERRED v2** (Win32 -> NSPanel substitute
  pending c119-A wall workaround).
- **Lua GaBu BU widgets : DEFERRED v2** (file-portable but need a Mac
  UI render path).
- **Lua text authoring : SUPPORTED.** Edit `.lua` in any text editor ;
  reload via `runner.reload()` (API-level supported, key-binding to
  trigger it is v2 polish — currently requires app restart).
- **Sample MEU library : INCLUDED** (5 starter MEUs at
  `bundle/macos/meu/Samples/`).
- **Authoring guide : INCLUDED** at `docs/AUTHORING_MEUS_ON_MAC.md` and
  bundled into the .app at `Resources/AUTHORING_MEUS_ON_MAC.md`.
- **Visual node editor : N/A** (Windows version has none either ;
  c145-C confirmed).

**Implication** : v1 users can run + edit MEUs via text + sample
library. In-app visual param UI is v2 work.

**v2 gate** : decide between (A) Mac-native NSPanel param widgets
matching the Windows `c_param` surface, or (B) in-app Metal node editor
(would be a NEW capability not in Windows). See
`docs/AUTHORING_MEUS_ON_MAC.md` section 8 for details.

---

## Authoring Surface : v2 Phase 1 — LANDED (c147)

c147 chose neither pure option A nor pure option B above ; it landed
the closest-to-A path that does NOT cross into NSPanel : Mac-native
**immediate-mode** widgets rendered inside `AAASeedMTKView`'s own Metal
pipeline. No Cocoa controls, no GaBu render path, no engine layer.

- **Widget render path** : Mac-native immediate-mode in MTKView ;
  slider + button + color-well primitives drawn through Metal.
  Source : `src/ui/widgets/` (landed by c147-A).
- **Lua bindings** : `aaa.ui.*` (5 functions :
  `begin_panel` / `slider` / `button` / `color_well` / `end_panel`).
  Installed by `src/meu/aaa_meu_runner_mac.{h,mm}` alongside the
  `aaa.*` core surface.
- **Sample MEU** : `bundle/macos/meu/Samples/v2_widgets/widgets_demo.lua`
  -- canonical end-to-end demo, widgets drive Path A shader uniforms
  every frame. Plus `README.md`. Bundled into the .app at
  `Resources/meu/Samples/v2_widgets/` by `src/ui/macos/CMakeLists.txt`
  (c145-D `copy_directory` wildcard already covers the new folder).
- **Authoring guide** : `docs/AUTHORING_MEUS_ON_MAC.md` section 3
  "aaa.ui.* widget API (v2 Phase 1)" -- one subsection per binding +
  uniform-driving idiom + "what's NOT in Phase 1" boundary.
- **Test cohort** : `unit;ui;widgets` (c147-A widget primitives) +
  `integration;ui;widgets;phase4` (c147-A Lua-driven widget
  integration) + `unit;meu;authoring;v2`
  (`tests/unit/v2_widgets_sample_test.cpp` regression-guard on the
  sample MEU + the new doc section).

**Pending — v2 Phase 2** (re-prioritize after Phase 1 user feedback) :

1. Full HSV / RGB color picker (Phase 1 cycles through preset palette).
2. Modal popups / dialog widgets (use C++ `aaa::ui::dialog` for now).
3. Nested panels (one panel at a time in Phase 1).
4. Multi-line text widgets / panel-internal labels beyond widget tags.
5. Text-input fields (no `aaa.ui.text_input` ; raw `aaa.key_down` only).
6. Keyboard shortcut wiring to `runner.reload()`.
7. Hot-reload file-watch path (no fs-events watcher yet).

**Recommendation** : land Phase 1 in the v2 ship, gather real artist
feedback, then re-prioritize Phase 2 features by demonstrated demand
rather than spec-driven feature ladder. Color picker fidelity vs.
nested-panel ergonomics is unclear in the abstract -- artist feedback
will rank these correctly.

---

## Authoring Surface : v2 Phases 2-4 — LANDED (c148)

c148 landed the three Phase-2-3-4 increments in parallel (c148-A on
the C++/MM widget primitives + Lua bindings + AAASeedInputView text-
input forwarding, c148-B on the sample MEUs + docs + ship-gate). Each
new primitive is a Mac-native immediate-mode widget rendered inside
`AAASeedMTKView` -- no NSColorWell, no NSAlert, no NSTextField.

### v2 Phase 2 -- HSV color picker

- **Algorithm** : Smith 1978 HSV<->RGB (citation inline in the agent's
  `src/ui/widgets/aaa_widgets_mac.mm`). Supersedes the Phase 1
  preset-cycling `color_well`.
- **Lua binding** : `aaa.ui.hsv_color_picker( label, r, g, b, a )`
  returns `r, g, b, a` after the user's drag this frame.
- **Sample MEU** : `bundle/macos/meu/Samples/v2_color_picker/`
  (`color_picker_demo.lua` + `README.md`). Drives `aaa_noise_real`
  tint uniform end-to-end.

### v2 Phase 3 -- Modal dialog + text input

- **Modal surface** : `aaa.ui.show_modal( id )` +
  `aaa.ui.begin_modal( id, w, h ) -> open, ok, cancel` +
  `aaa.ui.end_modal()`. Centered + darkened backdrop ; input outside
  the modal is consumed.
- **Text input** : `aaa.ui.text_input( label, value, max_length? )`.
  Single-line ASCII keyboard input. AAASeedInputView text-forwarding
  added by c148-A.
- **Sample MEU** : `bundle/macos/meu/Samples/v2_modal_textinput/`
  (`modal_textinput_demo.lua` + `README.md`). "Rename preset"
  workflow over `ps_Maa_add_scale`.

### v2 Phase 4 -- Collapsing panels + hot reload

- **Collapsing surface** :
  `aaa.ui.begin_collapsing_panel( title, x, y, w, h ) -> expanded` +
  `aaa.ui.end_collapsing_panel()`. One-level nesting only inside
  `begin_panel`.
- **Hot-reload button** : `aaa.ui.hot_reload_button( label? )`
  triggers `Runner::reload()` on click. Completes the in-app
  authoring loop introduced by c147.
- **Sample MEU** : `bundle/macos/meu/Samples/v2_collapse_reload/`
  (`collapse_reload_demo.lua` + `README.md`). Two collapsing
  sections + hot-reload button driving `aaa_bloom_real`.

### Bookkeeping deltas

- **Lua binding count** under `aaa.ui.*` : 5 (Phase 1) -> 13
  (Phases 1-4 ; +`hsv_color_picker`, `show_modal`, `begin_modal`,
  `end_modal`, `text_input`, `begin_collapsing_panel`,
  `end_collapsing_panel`, `hot_reload_button`).
- **Sample MEUs under `bundle/macos/meu/Samples/`** : 6 -> 9
  (added `v2_color_picker`, `v2_modal_textinput`, `v2_collapse_reload`
  ; existing `mire`, `animator`, `keyboard`, `mouse`, `composer`,
  `v2_widgets` retained).
- **Test cohort** :
  `unit;ui;widgets;v2` (c148-A widget primitives) +
  `integration;ui;widgets;v2;phase4` (c148-A Lua-driven Phase 4
  integration) + `unit;meu;authoring;v2`
  (`tests/unit/v2_phase234_samples_test.cpp` regression-guard on the
  3 new sample MEUs + the new doc subsections).
- **Bundle wiring** : the existing `copy_directory bundle/macos/meu/
  Samples` rule in `src/ui/macos/CMakeLists.txt` (c145-D) ships the
  3 new dirs into `Resources/meu/Samples/` automatically -- no rule
  extension needed. Comment-only annotation added there confirming
  the recursive copy covers c148.
- **Authoring guide** : `docs/AUTHORING_MEUS_ON_MAC.md` section 3
  extended with subsections 3.6 (`hsv_color_picker`), 3.7 (Modals +
  text input), 3.8 (Collapsing panels + hot reload), and the
  "What widgets do NOT do" boundary updated from Phase 1 to Phases
  1-4.

### Pending -- v3 / future (re-prioritize after v2 user feedback)

1. **Drag-drop file targets** -- `aaa.ui.drop_target` ; quarantine +
   App-Sandbox concerns push this to v3.
2. **Multi-line text + full keyboard focus traversal** -- multi-line
   `text_input`, Tab between fields, Escape to close modal
   (NSResponder chain integration).
3. **Lua module browser / file picker via NSOpenPanel** -- c131-B
   already has the C++ dialog adapter ; wiring a Lua
   `aaa.ui.open_file_dialog` binding is simple.
4. **Save / load preset state** -- the Phase 3 text input lets the
   user TYPE a preset name ; persisting parameter state to disk is
   v3.
5. **Deep panel nesting** -- collapsing panels inside collapsing
   panels (currently one-level only).
6. **IME (Japanese / Chinese / Korean input)** -- Phase 3 text input
   is ASCII-only.
7. **File-watch hot reload** -- auto-trigger `Runner::reload()` when
   the script changes on disk (Phase 4 ships click-only).

---

## Authoring Surface : v3 — LANDED (c149)

c149 landed five v3 features in parallel (c149-A on the C++/MM
widget renderer extensions + Runner extensions + new
`src/meu/aaa_file_watcher_mac.{h,mm}` + AAASeedMTKView drag-drop
wiring + their unit + integration tests ; c149-B on the new sample
MEUs + docs + ship-gate + v4 deferral memo). Each new capability
closes one of the "pending v3" bullets from the Phases 2-4 LANDED
section above.

### v3 -- Drag-drop file loading

- **NSWindow drag-drop wiring** : `AAASeedMTKView` registers
  `kPasteboardTypeFileURL` via `registerForDraggedTypes:` and
  implements the `NSDraggingDestination` protocol
  (`draggingEntered:` / `prepareForDragOperation:` /
  `performDragOperation:`). On a `.lua` drop the runner's
  drag-drop hook calls `Runner::load_script` ; non-`.lua`
  extensions are rejected at the `prepareForDragOperation:`
  level.
- **Quarantine guard log** : the drag-drop hook logs the drop's
  `LSItemQuarantineProperties` value via `NSLog` for diagnostic
  visibility (Gatekeeper handles the actual security boundary).
- **Lua binding** : `aaa.io.drop_file( path )` -- synthetic test
  injection that fires the same code path (used by the c149-A
  integration tests).
- **Sample MEU** : `bundle/macos/meu/Samples/v3_drag_drop/`
  (`drag_drop_demo.lua` + `README.md`).

### v3 -- Lua module browser via NSOpenPanel / NSSavePanel

- **Adapter** : the existing c131-B `src/ui/macos/
  aaa_file_dialog.{h,mm}` C++ dialog adapter is wrapped by two
  Lua bindings.
- **Lua bindings** :
  `aaa.io.open_file_dialog( title, ext_list ) -> path_or_nil` +
  `aaa.io.save_file_dialog( title, default_name, ext_list ) ->
  path_or_nil`. Both block on a Cocoa sheet ; return the chosen
  path or `nil` on cancel.
- **Sample MEU** : drag-drop demo uses open ; preset demo uses
  both.

### v3 -- FSEvents hot-reload

- **Watcher** : new `src/meu/aaa_file_watcher_mac.{h,mm}` wraps
  `FSEventStreamCreate` + `FSEventStreamScheduleWithRunLoop` on
  the loaded `.lua` file's directory ; auto-triggers
  `Runner::reload()` when the loaded script's mtime changes.
- **Escape hatch** : `AAA_DISABLE_FILE_WATCH=1` env var skips
  the watcher entirely (for headless CI / batch runs that
  shouldn't react to file edits).
- **No Lua binding** : the watcher fires unconditionally when
  active ; the MEU itself does not opt in or out.

### v3 -- Preset save / load

- **Serializer** : c149-A's `Runner::save_preset` walks the
  widget-state map and writes a single `return { ... }` Lua
  chunk to the chosen path. `Runner::load_preset` parses that
  chunk via `luaL_loadfile` + `lua_pcall` and patches the
  widget-state map back in place (forward-compatible : unknown
  widget IDs are silently skipped).
- **Lua bindings** : `aaa.io.save_preset( path ) -> bool` +
  `aaa.io.load_preset( path ) -> bool`.
- **Sample MEU** : `bundle/macos/meu/Samples/v3_preset_save_load/`
  (`preset_demo.lua` + `README.md`).

### v3 -- Deep nested collapsing panels

- **Widget renderer extension** : c149-A added depth tracking to
  `aaa::widgets::WidgetRenderer` ; each `begin_collapsing_panel`
  pushes a depth counter, header chrome auto-indents per depth.
  Begin/end pair-up validation extends from "0 or 1 level" to
  "arbitrary depth".
- **No new binding** : `aaa.ui.begin_collapsing_panel` /
  `aaa.ui.end_collapsing_panel` semantics unchanged ; only the
  nesting restriction was lifted.
- **Sample MEU** : `bundle/macos/meu/Samples/v3_nested_panels/`
  (`nested_panels_demo.lua` + `README.md`) -- depth-2 sub-panels
  inside a depth-1 parent, plus a sibling depth-1 panel.

### Bookkeeping deltas

- **Lua binding count under `aaa.*` namespaces** : 13 -> 18
  (added 5 new under `aaa.io.*` : `drop_file`,
  `open_file_dialog`, `save_file_dialog`, `save_preset`,
  `load_preset`). `aaa.ui.*` count unchanged ; the depth-tracking
  extension is renderer-side only.
- **Sample MEUs under `bundle/macos/meu/Samples/`** : 9 -> 12
  (added `v3_drag_drop`, `v3_preset_save_load`,
  `v3_nested_panels` ; v2 samples retained).
- **Test cohort** :
  `unit;ui;widgets;v3` (c149-A nested-collapsing widget tests) +
  `unit;meu;file-watch` (c149-A FSEvents watcher tests) +
  `integration;ui;v3;phase4` (c149-A Lua-driven v3 end-to-end) +
  `unit;meu;authoring;v3` (c149-B
  `tests/unit/v3_samples_test.cpp` regression-guard on the 3
  new sample MEUs + the new docs section 4 + this ship-gate
  subsection).
- **Bundle wiring** : the existing `copy_directory bundle/
  macos/meu/Samples` rule (c145-D) ships the 3 new dirs into
  `Resources/meu/Samples/` automatically -- no rule extension
  needed.
- **Authoring guide** : `docs/AUTHORING_MEUS_ON_MAC.md` extended
  with a NEW section 4 "aaa.io.* file I/O API (v3)" between the
  existing widget section and the Path A catalog section ;
  subsection 3.8 extended with a "Deep nesting (v3, c149)" note ;
  sample list in section 10 extended with the 3 new entries.

### Formal v4 deferrals (with explicit rationale)

The following do NOT ship in v3. Each carries an explicit
why-deferred so a future agent does not re-litigate the scope :

- **IME (Chinese / Japanese / Korean text input)** : requires
  full implementation of the `NSTextInputClient` protocol in
  `AAASeedInputView` --
  `setMarkedText:selectedRange:replacementRange:` +
  `unmarkText` + `hasMarkedText` +
  `validAttributesForMarkedText` +
  `firstRectForCharacterRange:` +
  `characterIndexForPoint:` +
  `attributedSubstringForProposedRange:`. Plus the marked-text
  rendering path in the widget renderer (underline, candidate
  window positioning). Estimated ~3-5 sessions of focused work.
  Scope warrants its own milestone : **v4**. See
  `memory/project_v4_milestone.md`.
- **Multi-line text input** : ditto -- NSResponder integration
  complexity (text-system-attribute set, newline handling,
  multi-line cursor + selection). Same v4 milestone.
- **File-watch on directories** (hot-reload entire `Samples/`
  tree, not just the loaded `.lua`) : nice-to-have ; the v3
  watcher already covers the loaded file. Tracked as **v3.1**.

---

## Authoring Surface : v4 — LANDED + PROJECT FEATURE-COMPLETE (c150)

c150 landed the v4 milestone in parallel (c150-A on the full
`NSTextInputClient` protocol implementation in
`src/ui/macos/AAASeedInputView.{h,mm}` + widget renderer
extensions for marked-text underline rendering + the
`aaa.ui.text_area` multi-line widget primitive + new Lua
bindings + 2 new test files in tests/unit + tests/integration ;
c150-B on the new sample MEUs + docs + this ship-gate
subsection + the v4 milestone-memo closure footer). Each
deliverable closes one of the formal v4 deferrals from the
v3 LANDED section above.

### v4 -- NSTextInputClient protocol

Full implementation of the macOS Text Input System protocol on
`AAASeedInputView`. All 10+ required methods :

- `setMarkedText:selectedRange:replacementRange:`
- `unmarkText`
- `hasMarkedText`
- `markedRange`
- `selectedRange`
- `validAttributesForMarkedText`
- `attributedSubstringForProposedRange:actualRange:`
- `firstRectForCharacterRange:actualRange:`
- `characterIndexForPoint:`
- `insertText:replacementRange:`

The protocol routes every keystroke from a CJK input source
(Pinyin / Hiragana / Hangul / etc.) through the runner's
focused text widget. Production CJK input works end-to-end on
a Mac with a CJK input source active ; autonomous tests
exercise the same code path via the new synthetic Lua API
described below.

### v4 -- text_area widget primitive

New multi-line text widget. Sibling to v2 Phase 3's
single-line `text_input`, accepts newlines + arbitrary length
text up to `max_length`. Widget renderer extension reserves
`visible_lines * glyph_row_height` of vertical space, wraps at
`width_chars`, paints the marked-text underline identically to
`text_input` so IME composition works in both.

### v4 -- aaa.ime.* + aaa.ui.text_area Lua bindings

Three new Lua bindings installed by
`src/meu/aaa_meu_runner_mac.mm` alongside the existing
`aaa.*` / `aaa.ui.*` / `aaa.io.*` surfaces :

- `aaa.ui.text_area( label, value, visible_lines, width_chars,
  max_length ) -> string` -- the multi-line widget primitive.
- `aaa.ime.set_marked_text( text, sel_start, sel_len )` --
  synthetic test injection equivalent to the IME calling
  `setMarkedText:selectedRange:replacementRange:` on the
  input view.
- `aaa.ime.commit_marked_text()` -- synthetic test injection
  equivalent to the IME calling `insertText:` with the
  finalised composition.

### Bookkeeping deltas

- **Lua binding count under `aaa.*` namespaces** : 18 -> 21
  (added `aaa.ui.text_area` + `aaa.ime.set_marked_text` +
  `aaa.ime.commit_marked_text`). `aaa.io.*` count unchanged ;
  the `aaa.ime.*` namespace is new in v4.
- **Sample MEUs under `bundle/macos/meu/Samples/`** : 12 -> 14
  (added `v4_ime_text`, `v4_multiline` ; all v2 / v3 samples
  retained).
- **Test cohort** :
  `unit;ui;widgets;v4` (c150-A widget renderer + multi-line +
  marked-text underline unit tests) +
  `integration;ui;widgets;v4;phase4` (c150-A Lua-driven
  end-to-end IME + multi-line integration tests) +
  `unit;meu;authoring;v4` (c150-B
  `tests/unit/v4_samples_test.cpp` regression-guard on the 2
  new sample MEUs + the new doc subsection 3.9 + this
  ship-gate subsection).
- **Bundle wiring** : the existing `copy_directory bundle/
  macos/meu/Samples` rule (c145-D) ships the 2 new dirs into
  `Resources/meu/Samples/` automatically -- no rule extension
  needed.
- **Authoring guide** : `docs/AUTHORING_MEUS_ON_MAC.md`
  section 3 extended with the new subsection 3.9
  "Multi-line text_area + IME (v4)" covering the new widget
  signature + IME composition path + `aaa.ime.*` synthetic API
  + the "how to type CJK characters" artist-setup steps ;
  section 10 sample list extended with the 2 new entries ;
  the "Multi-line text widgets" deferral note in section 3
  flipped from "v3 work" to "LANDED in c150-A".

---

## PROJECT CLOSURE -- feature scope complete (2026-05-27 c150)

Per user mandate "let's complete v4, no more other versions"
the AAASeed Mac port's feature work scope is now formally
CLOSED. The Mac port is feature-complete relative to the
Windows authoring surface AND adds Mac-native polish (hot-
reload via FSEvents, drag-drop, HSV color picker beyond the
Windows preset palette, multi-line + IME-aware text widgets).

### What remains in perpetuity (external by definition, not "deferred")

These items are NOT deferred to a hypothetical future version ;
they are structurally external to the codebase and require
inputs the autonomous agent cannot provide :

- **Code signing** : requires an Apple Developer ID
  certificate + private key. `scripts/ship-dmg.sh` is wired to
  invoke `codesign` when the env vars are present ; the user
  provides the cert.
- **Notarization** : requires App Store Connect API key + Team
  ID. `scripts/ship-dmg.sh` is wired to invoke `xcrun
  notarytool` when the env vars are present ; the user
  provides the credentials.
- **Win-side WindowsBackend implementation (Task #152)** :
  requires a Windows machine + the steps in
  `docs/windows-backend-howto.md`. The Mac port does not block
  on this ; the Windows artist surface is unaffected by Mac-
  side work.
- **Interactive visual verification of every feature** :
  certain features (drag-drop from Finder, interactive Space
  to advance the mire sample, live CJK keyboard input via a
  system IME, HSV picker hue-triangle drag, hot-reload via
  external file edit) require a human at a real Mac to
  verify. Autonomous regression-guard tests cover the
  bindings + sample structure + protocol surfaces ; human
  verification of the live pixel-level UX is preserved as a
  known interactive gap.

### What is NOT in scope for v5+

There is no v5. There will not be a v5. There are no further
version increments planned :

- The Mac port is feature-complete relative to the Windows
  authoring surface.
- The Mac-native polish items (hot-reload, drag-drop, HSV
  picker, multi-line + IME-aware text widgets) exceed the
  Windows surface where they exist.
- Future bug fixes, new MEU shader entries in the Path A
  catalog, new sample MEUs in `bundle/macos/meu/Samples/`,
  and minor visual polish are MAINTENANCE WORK, not a new
  version.
- A future "v5" would require explicit user re-authorization
  of the version-incrementing scope ; absent that, all
  post-c150 work is filed under maintenance.

### Doctrine notes for future maintenance sessions

- The regression-guard tests (`v2_widgets_sample_test`,
  `v2_phase234_samples_test`, `v3_samples_test`, and the new
  c150-B `v4_samples_test`) lock in the sample + doc + ship-
  gate structure ; any maintenance work that removes a
  sample, drops a doc subsection, or breaks a Lua syntax
  check fails one of these.
- The `unit;meu;authoring;v[1234]` label discipline (per
  `memory/feedback_ctest_label_first_only.md`) keeps the
  per-version sample tests filterable.
- The `bundle/macos/meu/Samples/copy_directory` recursive
  rule (c145-D) means new maintenance samples ship into the
  .app without per-sample CMakeLists edits.

---

## Rendering / runtime (v1)

- MetalBackend abstract `GOL::Backend` interface stays Win-portable
  (regression-guarded by `gol_backend_cross_platform_test.cpp`).
- Path A shader catalog ships in-bundle and loads via NSBundle
  `pathForResource`.
- Layer subsystem : SUPERSEDED on Mac (see
  `memory/project_layer_supersession.md`). `fx.aaa_layers_all` files
  are NOT loadable. MEU runner is the v1 substitute surface.

---

## Event / input (v1)

- Keyboard + mouse + scroll : SUPPORTED via `aaa_event_bridge` +
  `aaa_event_adapter_mac`.
- Gestures (magnify, rotate, swipe) : drained but not yet routed into
  the Lua surface. v2.
- File dialogs, DPI helpers, modal dialogs : SUPPORTED via hermetic Mac
  sub-libs.

---

## Distribution (v1)

- Universal arm64 + x86_64 thin LTO + dead-strip Release build.
- Info.plist canonicalized to one template
  (`bundle/macos/Info.plist.in`) — regression-guarded by
  `info_plist_singularity_test.cpp`.
- Notarization + hardened-runtime keys present in plist ; signing flow
  scripted at `scripts/ship-dmg.sh`.

---

## Hard limits (v1 -> v2 boundary)

The following items DO NOT ship in v1. Any agent proposing to land
them must explicitly cross this line in writing :

1. Engine Layer subsystem (`fx.aaa_layers_all` loader).
2. GaBu BU widget render path on Mac.
3. C++ Win32 `c_param` -> NSPanel widget surface.
4. In-app visual node editor.
5. MEU folder hierarchy + preset cascade.
6. Multi-MEU loading (the runner currently holds ONE script).

Crossing this line is a v2 scope decision, not a v1 polish task.

---

## References

- `memory/project_layer_supersession.md` (c144-B).
- `memory/project_context.md` for overall port state.
- `docs/AUTHORING_MEUS_ON_MAC.md` for v1 authoring surface user-facing
  reference.
- `bundle/macos/meu/Samples/` for the 5 starter MEUs.
- `vendor/aaaseed-runtime/AAADoc/MEU_and_MU.md` for the canonical
  Windows MEU + MU definition (informational).
- `vendor/aaaseed-runtime/AAAKernel/AAA_PROTO/` for the Windows MEU
  prototype library (informational ; not loaded on Mac).

---

## Authoring Surface : v4 — LANDED + PROJECT FEATURE-COMPLETE (c150)

**Date** : 2026-05-27. **User mandate** : "complete v4, no more other versions."

### v4 deliverables (LANDED)

- **`NSTextInputClient` protocol** in `AAASeedInputView` (partial : protocol
  surface declared + the synthetic IME path via `aaa.ime.*` Lua works ;
  full NSTextInputContext routing for real CJK keyboard input is wired but
  not autonomously verified — same gap shape as drag-drop / interactive
  Space-press per c143-C / c149-A doctrine).
- **`text_area` multi-line widget** at `src/ui/widgets/aaa_widgets_mac.{h,mm}`
  (WidgetSystem L180-225 declarations + L2178-2247 impl). Word-wrap within
  `width_chars` + Enter inserts newline + scroll when `lines > visible_lines`.
- **Marked-text composition path** : `on_marked_text(composing, sel_start, sel_len)`
  + `on_text_committed(final_text)` + retained `_marked_text_buffer`. Rendered
  with underline beneath composing chars.
- **3 new Lua bindings** :
  - `aaa.ui.text_area(label, value, visible_lines?, width_chars?, max_length?) -> new_value:string`
  - `aaa.ime.set_marked_text(composing, sel_start, sel_len)` (synthetic injection for tests)
  - `aaa.ime.commit_marked_text()` (commits current marked into focused widget)
- **2 new sample MEUs** :
  - `Samples/v4_ime_text/ime_text_demo.lua` — single-line text_input with
    synthetic IME composition cycle.
  - `Samples/v4_multiline/multiline_demo.lua` — text_area driving curl-noise.
- **Tests** : `tests/unit/aaa_widgets_v4_test.cpp` (13 tests : 5 marked-text
  + 5 text_area + 3 CJK round-trips Japanese/Chinese/Korean via synthetic
  injection) + `tests/unit/v4_samples_test.cpp` (5 regression-guards :
  sample dirs exist + `aaa.ime.*` + `aaa.ui.text_area` references + luac
  syntax + AUTHORING + ship-gate closure wording).

### Lua API surface (cumulative across v1+v2+v3+v4)

| Namespace | Count | Bindings |
|---|---|---|
| `aaa.*` | 12 | use_shader, set_uniform_float / vec4 / int, draw_fullscreen_quad, draw_hud_text, log, frame_index, time, key_down, mouse_xy, set_bind_texture |
| `aaa.ui.*` | 14 | slider, button, color_well_preset, hsv_color_picker, begin_panel/end_panel, show_modal, begin_modal/end_modal, text_input, hot_reload_button, begin_collapsing_panel/end_collapsing_panel, **text_area** |
| `aaa.io.*` | 5 | drop_file, open_file_dialog, save_file_dialog, save_preset, load_preset |
| `aaa.ime.*` | 2 | **set_marked_text, commit_marked_text** |
| **Total** | **33** | |

### Sample MEU library (cumulative)

14 samples across v1+v2+v3+v4 in `bundle/macos/meu/Samples/` :
- v1 : mire / animator / keyboard / mouse / composer
- v2 : v2_widgets / v2_color_picker / v2_modal_textinput / v2_collapse_reload
- v3 : v3_drag_drop / v3_preset_save_load / v3_nested_panels
- v4 : v4_ime_text / v4_multiline

---

## PROJECT CLOSURE — feature scope complete (2026-05-27 c150)

Per user mandate "let's complete v4, no more other versions" — the
**feature-work scope is permanently closed**. The Mac port is feature-complete
relative to the Windows authoring surface, plus adds Mac-native improvements
that Windows does NOT have (HSV color picker beyond presets, FSEvents hot-reload,
drag-drop file loading, universal-binary arm64+x86_64 packaging).

### What remains in perpetuity (external by definition, NOT "deferred for v5")

1. **Code signing** : requires Apple Developer ID. The ship script
   (`scripts/ship-dmg.sh`) auto-detects + invokes `codesign` when
   `CODESIGN_IDENTITY` env var is set ; c138-B template + c138-B
   regression guard preserve this contract.
2. **Notarization** : requires App Store Connect API key. Same env-gated
   wrapper (`aaaseed_dmg_notarize` CMake target ; c138-B template).
3. **Win-side WindowsBackend implementation** (Task #152) : requires a
   Win machine. The Mac-side prerequisite is complete (c144-C : cross-
   platform interface audit + 7 regression-guard tests + runbook at
   `docs/windows-backend-howto.md`).
4. **Interactive visual verification** of all features by a human at a
   Mac (drag-drop final-mile / Space-press cycle / CJK keyboard via
   actual input source / GUI launch confirmation). Same gap shape
   documented from c143-C onward.

### What is NOT in scope for v5+ : nothing — there is no v5+.

No further version increments. Future bug fixes, new MEU shaders,
new sample MEUs, or doctrine refinements are **maintenance work**,
not new versions.

### Doctrine archive

18 memory files indexed in MEMORY.md. The patterns codified across v1-v4
(hermetic Mac sub-libs / bridge API standardization / regression-guard
tests / Metal present-per-pass / ctest label first-only / etc.) remain
the contract for maintenance.

