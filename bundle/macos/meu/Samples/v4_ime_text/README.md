# Sample MEU : v4_ime_text

The canonical demo for the v4 IME composition + commit surface
(c150-A). Mac-native counterpart to the Windows IME path ;
demonstrates that an `aaa.ui.text_input` field can accept full
CJK (Chinese / Japanese / Korean) composition through the macOS
Text Input System, plus exposes the synthetic
`aaa.ime.set_marked_text` / `aaa.ime.commit_marked_text` Lua
bindings for autonomous test injection.

## What it does

Opens a 380x240-pixel main panel containing :

- A `your name (CJK ok)` text input field accepting up to 64
  characters. With a CJK input source active the field shows
  underline-rendered marked text during composition + commits on
  Enter.
- A `Run synthetic IME demo` button. On click the next 90 frames
  inject a 3-step pretend composition ("k" -> "ko" -> commit)
  into the same field, so a screenshot can capture the marked-
  text underline + commit transition without a real IME keyboard.
- A HUD line echoing the committed `name` + the current
  synthetic-demo status.

## How to enable CJK input on macOS

1. Open System Settings.
2. Keyboard -> Input Sources -> Edit -> +.
3. Pick a language (Chinese, Japanese, Korean) and an input
   method (Pinyin Simplified, Romaji, 2-Set Korean, etc.).
4. Add it -- the menu-bar Input Source switcher (or Ctrl-Space
   by default) now toggles between US English and your CJK IME.
5. Switch to the CJK IME, focus the AAASeed text input, and type
   normally. The marked-text underline appears under pending
   composition characters ; Enter commits them.

## Widget + IME bindings exercised

- `aaa.ui.begin_panel` / `aaa.ui.button` / `aaa.ui.end_panel` --
  from Phase 1.
- `aaa.ui.text_input` -- from v2 Phase 3 ; now also routes IME
  composition events from the `NSTextInputClient` protocol path
  that c150-A wired into `AAASeedInputView`.
- `aaa.ime.set_marked_text( text, sel_start, sel_len )` -- new
  v4 synthetic injection. Same effect as the IME calling
  `setMarkedText:selectedRange:replacementRange:` on the input
  view.
- `aaa.ime.commit_marked_text()` -- new v4 synthetic injection.
  Same effect as the IME calling `insertText:` with the
  finalised composition.

See `docs/AUTHORING_MEUS_ON_MAC.md` section 3.9
"Multi-line text_area + IME (v4)" for the full binding reference.

## Path A shader used

- `aaa_noise_real.metal` -- Perlin + Simplex + FBM (c135-A).
  Provides a moving background so the marked-text underline
  reads visibly over a non-flat fill.

## Why aaa.ime.* exists (test-only synthetic API)

The production CJK path runs through Cocoa's Text Input System,
which requires a real CJK input source active in System
Settings. Autonomous CI cannot guarantee that prerequisite, so
c150-A added a Lua-callable synthetic API that injects the same
events directly into the runner's input view. Integration tests
use the synthetic path ; artists at a Mac with a CJK IME use
the production path. Both arrive at the same widget-renderer
marked-text + commit code.

## How to modify

Edit `ime_text_demo.lua` :

- Add a second `text_input` to demonstrate Tab traversal between
  fields once the v4 focus chain is fully wired.
- Swap `aaa_noise_real` for `aaa_curl_noise_real` to test the
  marked-text legibility over higher-contrast motion.
- Extend the synthetic demo to commit different strings each
  cycle (`"ko"` -> `"kon"` -> `"kongo"`) to exercise the
  composition-grows-then-commits pattern.

## v4 boundary

- The synthetic API drives composition into the FOCUSED text
  input ; if no input is focused, the calls are silent no-ops.
- Real CJK keyboard input verification requires a Mac with a
  CJK input source active -- a known interactive gap (same
  shape as drag-drop / interactive Space-press from c143-C /
  c145-A / c149-A).
- The `NSTextInputClient` protocol implementation in
  AAASeedInputView is complete (10+ methods) ; the candidate-
  window positioning callback (`firstRectForCharacterRange:`)
  returns the focused widget's screen-space rect so the system
  IME UI appears below the input field.
- Per the v1 ship-gate "PROJECT CLOSURE" section, the Mac port
  is feature-complete at v4 ; further IME polish (e.g. emoji
  panel triggers, bidi text) is maintenance work, not a new
  version.
