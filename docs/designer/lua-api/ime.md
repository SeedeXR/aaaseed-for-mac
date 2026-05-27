# Lua API : `aaa.ime.*` (IME composition)

> Reference for the 2 synthetic-injection bindings under the
> `aaa.ime.*` namespace, landed in v4 (c150-A). Source :
> `src/meu/aaa_meu_runner_mac.{h,mm}` + `src/ui/widgets/aaa_widgets_mac.{h,mm}`
> + `src/ui/macos/AAASeedInputView.{h,mm}` (the `NSTextInputClient`
> protocol implementation).

These bindings exist to **drive the same code path the production
IME uses**, from Lua, so integration tests can verify marked-text
rendering + commit transitions without requiring a CJK input source
on the test host. End-user MEUs rarely need to call them directly --
typing into `aaa.ui.text_input` or `aaa.ui.text_area` with a CJK
input source active does the same thing automatically.

---

## How real CJK input routes through AAASeed

When a CJK input source (Pinyin / Hiragana / Hangul / etc.) is
active in macOS System Settings and the user types into a focused
`aaa.ui.text_input` or `aaa.ui.text_area` :

1. Each keystroke generates a `marked text` event with an
   underline-rendered pending composition (the squiggly line under
   typed romaji on a Japanese IME, the pinyin display on a Chinese
   IME, the hangul jamos on a Korean 2-Set IME).
2. The IME's commit gesture (Enter on most IMEs ; Space on some
   Chinese IMEs to confirm the top candidate) commits the
   composition : marked text clears, the committed string inserts
   at the cursor.
3. Escape clears pending marked text without committing.

The c150-A `NSTextInputClient` protocol implementation in
`AAASeedInputView` covers all 10+ required methods :
`setMarkedText:selectedRange:replacementRange:`, `unmarkText`,
`hasMarkedText`, `markedRange`, `selectedRange`,
`validAttributesForMarkedText`,
`attributedSubstringForProposedRange:actualRange:`,
`firstRectForCharacterRange:actualRange:`,
`characterIndexForPoint:`, plus the `insertText:replacementRange:`
final-commit hook.

The `aaa.ime.*` bindings let a Lua script (typically a test harness)
inject the same `setMarkedText` + `insertText` events that the real
IME would, so integration tests can round-trip a fake "ko" -> "ko" ->
commit sequence and assert the resulting widget value -- no real
keyboard required.

---

## `aaa.ime.set_marked_text( text, sel_start, sel_len ) -> nil`

Synthetic test injection that drives the same code path the
production IME uses for marked-text composition.

- `text` (string) -- the pending composition string to display with
  the marked-text underline.
- `sel_start` (integer) -- selection start within `text`, 0-based.
- `sel_len` (integer) -- selection length within `text`.
- The pair `(sel_start, sel_len)` matches the `NSTextInputClient`
  `selectedRange` semantics ; pass `(0, length)` to select the whole
  pending composition.
- Returns nothing.
- **Side effect** : sets the focused text widget's marked-text buffer
  + selection. The widget renderer paints the marked text with an
  underline on the next frame.

```lua
-- From a test harness or a synthetic-demo MEU.
-- Step 1 : pretend the user typed "k".
aaa.ime.set_marked_text( "k", 0, 1 )
-- ... wait some frames ...
-- Step 2 : pretend the user typed "o", making "ko" pending.
aaa.ime.set_marked_text( "ko", 0, 2 )
-- ... wait some frames ...
-- Step 3 : commit (see commit_marked_text below).
aaa.ime.commit_marked_text()
```

In the production path -- with a CJK keyboard active -- this happens
automatically as the user types ; you do not need to call this from
a normal MEU.

See also : [`commit_marked_text`](#aaaimecommit_marked_text-nil),
[Sample MEU : v4_ime_text](../samples.md#v4_ime_text).

---

## `aaa.ime.commit_marked_text() -> nil`

Synthetic test injection that commits the focused widget's pending
marked text. Equivalent to the IME calling `insertText:` with the
finalised composition after the user accepts.

- Returns nothing.
- **Side effect** : the marked-text buffer empties ; the committed
  string inserts at the cursor of the focused text widget ; the
  widget's return-value string updates so the MEU's Lua local
  catches up through the return-and-reassign idiom.

```lua
-- Synthetic round-trip : type "ko", commit.
aaa.ime.set_marked_text( "ko", 0, 2 )
-- ... after the widget has rendered the marked text for a few frames ...
aaa.ime.commit_marked_text()
-- The focused widget's value string now contains "ko" at the cursor.
```

The production CJK keyboard equivalent : on a Japanese IME, type
romaji `ko`, the IME shows the candidate hiragana `こ` with the
underline ; press Enter or click the candidate to commit ; the
widget value updates to `こ`.

See also : [`set_marked_text`](#aaaimeset_marked_text-text-sel_start-sel_len-nil).

---

## How to type CJK characters in AAASeed (artist setup)

1. Open **System Settings**.
2. Go to **Keyboard -> Input Sources -> Edit -> +**.
3. Choose a language and an input method :
   - **Chinese** : Pinyin Simplified, Pinyin Traditional, Cangjie,
     Wubi Hua, Stroke Simplified, Zhuyin, etc.
   - **Japanese** : Romaji, Kana, or split-Romaji+Kana.
   - **Korean** : 2-Set Korean (default), 3-Set Korean (390 / Final).
4. Add the input source ; the menu-bar Input Source switcher (or
   `Ctrl-Space` by default) toggles between US English and your
   CJK IME.
5. Launch AAASeed, switch to your CJK IME, focus an `aaa.ui.text_input`
   or `aaa.ui.text_area` widget, and type normally. Marked-text
   underline appears under pending composition characters ; Enter
   (or the IME-specific commit gesture) commits.

<!-- screenshot: ime-pinyin-marked-text.png -->

The `Samples/v4_ime_text/ime_text_demo.lua` sample includes a
"Run synthetic IME demo" button that drives `set_marked_text` +
`commit_marked_text` programmatically, so you can watch the
marked-text underline appear + clear without setting up a CJK input
source.

---

## Honest gap : interactive CJK keyboard verification

The `NSTextInputClient` protocol plumbing + `WidgetSystem` state
machine are unit + integration tested via the synthetic
`aaa.ime.set_marked_text` calls
(`tests/unit/aaa_widgets_v4_test.cpp` `WidgetsV4Cjk.*` round-trips
for Japanese `こ` / Chinese `你` / Korean `안`). The full
on-device verification path -- actual keystrokes routing through
Pinyin / Hiragana / Hangul keyboards through the macOS input-method
server -- **requires a human at a Mac with the input source enabled**.
Autonomous tests do not reach the OS input-method server.

What this means practically :

- The marked-text / commit code path IS exercised by tests on every
  CI run, via the synthetic injection bindings on this page.
- If you find a bug typing CJK into an AAASeed widget on a real
  Mac, please file an issue with reproducer details (input source,
  the exact keystrokes, the expected vs. actual widget value
  string). It is highly likely the bug is in the OS-binding layer,
  not in the widget state machine.

This is the one v1 honest gap in the IME surface ; the rest of the
plumbing is end-to-end verified.

---

## What `aaa.ime.*` does NOT do

- **Inject keystrokes for non-text widgets.** `set_marked_text`
  targets the focused text-editing widget only. There is no `aaa.ime`
  binding to "press space" on a button, etc. (use the widget's
  natural Lua-side trigger : `if aaa.ui.button("X") then ... end`.)
- **Bundle its own IME engine.** The .app relies on the system IME
  chain ; switching IMEs is a System Settings operation. Bundling
  IMEs is out-of-scope.
- **Support bidi layout** (Arabic / Hebrew). The renderer paints
  glyphs left-to-right ; bidi reordering is polish, not a v5
  feature.
- **Multi-step undo / redo of committed compositions.** A committed
  composition is part of the widget's linear edit history ; no
  Cmd-Z stack.

---

## See also

- [Lua API : UI](ui.md#aaauitext_input-label-value-max_length-new_value)
  for `aaa.ui.text_input` (single-line, CJK-capable).
- [Lua API : UI](ui.md#aaauitext_area-label-value-visible_lines-width_chars-max_length-new_value)
  for `aaa.ui.text_area` (multi-line, CJK-capable).
- [Sample MEU : v4_ime_text](../samples.md#v4_ime_text).
- [Sample MEU : v4_multiline](../samples.md#v4_multiline).
