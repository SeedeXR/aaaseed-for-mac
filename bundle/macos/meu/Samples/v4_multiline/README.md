# Sample MEU : v4_multiline

The canonical demo for the v4 multi-line `aaa.ui.text_area`
widget primitive (c150-A). Mac-native counterpart to a
multi-line text editor inside the AAASeed authoring surface ;
demonstrates that text with embedded newlines round-trips
through an in-app widget without leaving the .app for a
separate text editor.

## What it does

Opens a 420x360-pixel main panel containing :

- A `notes` text-area widget with 8 visible lines, 48-character
  width, capped at 1024 characters. Pre-populated with 5 lines
  of welcome / instruction text so the multi-line render path
  is visible at launch.
- A `Save notes...` button that opens NSSavePanel with `.txt`
  filtering. The dialog returns the chosen path ; a real
  implementation would write `notes` to that path via the
  standard Lua io library (this sample logs the would-be path
  via `aaa.log` to keep the demo read-only).
- A HUD line echoing the current line count + the last save
  destination.

## Widget + I/O bindings exercised

- `aaa.ui.begin_panel` / `aaa.ui.button` / `aaa.ui.end_panel` --
  from Phase 1.
- `aaa.ui.text_area( label, value, visible_lines, width_chars,
  max_length )` -- new v4. Multi-line counterpart to
  `aaa.ui.text_input`. Returns the new string each frame, same
  return-and-reassign idiom.
- `aaa.io.save_file_dialog( title, default_name, ext_list ) ->
  path_or_nil` -- from v3.

See `docs/AUTHORING_MEUS_ON_MAC.md` section 3.9
"Multi-line text_area + IME (v4)" for the full binding reference.

## Path A shader used

- `aaa_curl_noise_real.metal` -- Bridson 2007 divergence-free
  curl noise (c137-A revival). Moving background helps QA the
  text-area background alpha + cursor blink against motion.

## How text_area differs from text_input

| Aspect           | `text_input` (v2 P3) | `text_area` (v4)         |
|------------------|----------------------|--------------------------|
| Lines            | 1                    | N (caller-specified)     |
| Newline keys     | rejected             | inserted as `\n`         |
| Vertical cursor  | n/a                  | arrow-up / arrow-down    |
| IME composition  | yes (c150-A)         | yes (c150-A)             |
| Default sizing   | 1 line tall          | `visible_lines` * row    |

Both widgets route through the same `NSTextInputClient`
protocol path c150-A wired into `AAASeedInputView` ; with a CJK
input source active, marked-text underline renders in either
widget identically.

## How to modify

Edit `multiline_demo.lua` :

- Add a second `text_area` for an "embedded MSL snippet" so the
  field renders below the notes field ; demonstrates two
  multi-line text widgets in one panel.
- Increase `visible_lines` to 16 + extend panel height to 600 ;
  shows that the widget scales linearly with reserved height.
- Wire the save button to actually write `notes` to disk via
  the Lua io library (`local f = io.open(path, "w"); f:write(
  notes); f:close()`).

## v4 boundary

- The widget renderer reserves `visible_lines * glyph_row_height`
  pixels of vertical space ; text beyond that scrolls inside
  the widget (cursor stays visible).
- Word-wrap at `width_chars` is glyph-cell-based ; proportional
  font wrap is a future polish item, not a v5 feature (no v5
  per the project closure).
- The c150-A renderer extension reuses the existing single-line
  cursor blink + selection-highlight code paths ; no new visual
  primitives.
- Per the v1 ship-gate "PROJECT CLOSURE" section, the Mac port
  is feature-complete at v4 ; further multi-line polish
  (find/replace, undo/redo stacks, syntax-highlighted text
  areas) is maintenance work, not a new version.
