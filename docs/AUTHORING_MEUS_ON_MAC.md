# Authoring MEUs on Mac (v1)

This document is the canonical reference for authoring Module Editable
Units (MEUs) for AAASeed on macOS. It covers the v1 authoring surface
honestly — what works today, what doesn't, and where to look next.

> Audience : artists + technical artists writing or modifying MEUs for
> the Mac DMG build. Familiarity with Lua is assumed ; familiarity with
> the Windows AAASeed authoring flow is helpful but not required.

---

## 1. What is an MEU on Mac v1

An MEU on Windows is a directory under `AAAKernel/AAA_PROTO/MEU_PROTO*/`
containing :

- `<name>.lua` — the MEU class definition + UI declaration.
- `fx.aaa_layers_all` — engine layer serialization (the visual graph).
- `preset_*.plua` — presets.

On Mac v1 an MEU is **just a Lua script** loaded by
`aaa::meu::Runner` (`src/meu/aaa_meu_runner_mac.{h,mm}`). That script
defines an `aaa.on_frame( w, h, frame )` callback that the runner
invokes once per Metal frame. The script uses the `aaa.*` Lua bindings
to select a Path A shader, push uniforms, queue HUD text, and draw a
fullscreen quad.

**Mac v1 does not load Windows-style MEU folders.** The `fx.aaa_layers_all`
file requires the engine's Layer subsystem, which the Mac port
superseded per `memory/project_layer_supersession.md` (c144-B). Five
sample MEUs at `bundle/macos/meu/Samples/` echo the spirit of canonical
Windows prototypes without dragging in the Layer surface.

---

## 2. The `aaa.*` Lua API

Every binding below is installed by `src/meu/aaa_meu_runner_mac.mm`'s
`install_aaa_bindings()`. They are the entire authoring surface ; do
not assume any other binding exists.

### `aaa.use_shader( name )`

Select the Path A shader to draw with on the next
`aaa.draw_fullscreen_quad()` call.

- `name` (string) — the bare stem of a `.metal` file in
  `Resources/shaders/` (e.g. `"aaa_curl_noise_real"`). No path, no
  extension.
- Returns nothing. Bad names are logged and the previous shader stays
  bound.
- Example : `aaa.use_shader( "aaa_bloom_real" )`.

### `aaa.set_uniform_float( slot, value )`

Push a scalar uniform.

- `slot` (integer, 0-based) — the float slot index ; shaders bind these
  through the canonical `set_uniform_float` table.
- `value` (number) — any double-precision float.
- Example : `aaa.set_uniform_float( 0, aaa.time() )`.

### `aaa.set_uniform_vec4( slot, x, y, z, w )`

Push a 4-component uniform.

- `slot` (integer, 0-based) — the vec4 slot index.
- `x, y, z, w` (numbers) — components.
- Example : `aaa.set_uniform_vec4( 0, 1.0, 1.0, 1.0, 1.0 )`.

### `aaa.set_uniform_int( slot, value )`

Push an integer uniform. The c128-c140 mode-flag pattern reserves slot
0 ; `1` selects the real algorithm, `0` selects the stub variant.

- Example : `aaa.set_uniform_int( 0, 1 )`.

### `aaa.set_bind_texture( slot, texture_name )`

Mark a fragment-shader sampler slot as active and reserve a
runner-owned texture for it.

- `slot` (integer, 0..7) — the sampler slot index.
- `texture_name` (string) — an asset-registry key the runner will
  resolve to a texture in a future session ; today the runner binds a
  placeholder texture so the shader gets a valid sampler. Pass any
  identifier string ; the value is reserved-not-required in v1.
- Returns nothing. The v1 sample surface does not allocate textures
  from Lua ; this binding is exposed for forward compatibility with v2
  FBO authoring.

### `aaa.draw_fullscreen_quad()`

Encode a 3-vertex fullscreen triangle through the active Metal encoder
using the currently-bound shader + uniforms.

- Call exactly once per frame ; the runner does not begin / end its own
  render pass.

### `aaa.draw_hud_text( text )`

Queue a single line of HUD overlay text for this frame.

- `text` (string) — the text to display. The host
  (`AAASeedMTKView`) reads this after `render_frame` returns and emits
  the text through the c61 glyph atlas + text-MSL pipeline.
- One queued string per frame ; calling twice replaces the previous.
- Empty string = no HUD this frame.
- Example : `aaa.draw_hud_text( "frame " .. frame )`.

### `aaa.log( text )`

Write a line to the macOS unified log via `NSLog`.

- Use for debugging from the Lua side ; visible in Console.app filtered
  by the `aaaseed_app` process.
- Example : `aaa.log( "shader changed to " .. shaders[ idx ] )`.

### `aaa.frame_index()`

Return the integer frame index. Resets to 0 on `runner.reload()`.

### `aaa.time()`

Return seconds (Lua number, double-precision) since the runner was
constructed. Wall-clock, monotonic, started at process launch.

### `aaa.key_down( key )`

Query whether a key is held this frame.

- `key` can be a string (named key) or an integer Apple hardware
  keycode (Carbon-style).
- Named keys : `"space"` (49), `"return"` (36), `"escape"` (53),
  `"left"` (123), `"right"` (124), `"down"` (125), `"up"` (126).
- For any other key, pass the integer keycode directly (e.g. digit `1`
  is keycode `18`).
- Returns `true` / `false`.
- Example : `if aaa.key_down( "space" ) then ... end`.

### `aaa.mouse_xy()`

Return the cursor position as two values (x, y) in window pixels.

- The runner does not normalize ; divide by the `w` / `h` arguments of
  `aaa.on_frame()` to get `[0, 1]^2`.
- Example : `local mx, my = aaa.mouse_xy()`.

---

## 3. `aaa.ui.*` widget API (v2 Phase 1)

The `aaa.ui.*` namespace exposes a Mac-native immediate-mode widget
surface introduced in v2 Phase 1 (c147). Widgets render inside
`AAASeedMTKView`'s Metal pipeline -- no Cocoa controls, no NSPanel, no
GaBu render path. The API is the spiritual successor to the Windows
GaBu BU widget shelf (`AAAKernel/GaBu/`), reimagined as an immediate-mode
API that returns the live user-modified value each frame.

Bindings are installed by `src/meu/aaa_meu_runner_mac.mm` alongside the
`aaa.*` core surface. They are call-once-per-frame and must appear
between `aaa.ui.begin_panel` and `aaa.ui.end_panel`.

### `aaa.ui.begin_panel( title, x, y, w, h )`

Open a UI container at screen-space rect `(x, y, w, h)` and start
accepting widget calls into it.

- `title` (string) -- displayed at the top of the panel.
- `x, y` (integers) -- top-left pixel coordinates in window space.
- `y` is measured from the top edge (consistent with `aaa.mouse_xy()`).
- `w, h` (integers) -- pixel size of the panel.
- Returns an integer handle (opaque ; reserved for future per-panel
  state queries -- in v2 Phase 1 you can ignore the return).
- Side effect : pushes a panel onto the runner's widget stack. Must be
  paired with exactly one `aaa.ui.end_panel()`.
- Example : `aaa.ui.begin_panel( "Controls", 16, 16, 320, 220 )`.

### `aaa.ui.button( label )`

Add a button to the current panel.

- `label` (string) -- displayed inside the button.
- Returns `true` only on the frame the user clicked the button
  (rising-edge ; the widget layer debounces internally so two
  consecutive frames never both return true from one click).
- Side effect : laid out vertically below the previous widget in the
  same panel.
- Example : `if aaa.ui.button( "Reset" ) then state = 0 end`.

### `aaa.ui.slider( label, value, min, max )`

Add a horizontal scrubber to the current panel.

- `label` (string) -- displayed left of the bar.
- `value` (number) -- the current value to display.
- `min, max` (numbers) -- bounds. `min < max` required.
- Returns a number : `value` unchanged when the user is not dragging,
  or the new value clamped to `[min, max]` when they are.
- Side effect : laid out vertically below the previous widget. The
  bar position reflects `(value - min) / (max - min)`.
- Idiom : assign the return value back to the same Lua local so the
  slider position persists across frames :
  `intensity = aaa.ui.slider( "intensity", intensity, 0.0, 2.0 )`.

### `aaa.ui.color_well( label, r, g, b, a )`

Add a color preview swatch + cycle-through-preset picker.

- `label` (string) -- displayed next to the swatch.
- `r, g, b, a` (numbers in `[0, 1]`) -- current color.
- Returns FOUR numbers : `r, g, b, a` -- unchanged on frames the user
  did not click ; advanced to the next preset on click frames.
- v2 Phase 1 implements preset-cycling only (a curated palette of
  ~8 hues). Full HSV / RGB picker is v2 Phase 2 (see "What widgets do
  NOT do" below).
- Idiom : `r, g, b, a = aaa.ui.color_well( "tint", r, g, b, a )`.

### `aaa.ui.end_panel()`

Close the current panel.

- Returns nothing.
- Side effect : pops the panel off the widget stack and submits the
  accumulated draw commands to the host renderer. The widgets will
  composite OVER the next `aaa.draw_fullscreen_quad()` call this frame.
- Must pair 1:1 with `aaa.ui.begin_panel`.

### How widgets compose with shader uniforms

The canonical pattern (also at `Samples/v2_widgets/widgets_demo.lua`) :

```lua
local intensity = 0.5    -- persisted across frames
local rgba = { r = 1.0, g = 0.5, b = 0.2, a = 1.0 }

function aaa.on_frame( w, h, frame )
    aaa.ui.begin_panel( "Controls", 16, 16, 320, 180 )
    if aaa.ui.button( "Reset" ) then intensity = 0.5 end
    intensity = aaa.ui.slider( "intensity", intensity, 0.0, 2.0 )
    rgba.r, rgba.g, rgba.b, rgba.a = aaa.ui.color_well(
        "tint", rgba.r, rgba.g, rgba.b, rgba.a )
    aaa.ui.end_panel()

    aaa.use_shader( "aaa_bloom_real" )
    aaa.set_uniform_int( 0, 1 )
    aaa.set_uniform_float( 0, intensity )
    aaa.set_uniform_vec4( 0, rgba.r, rgba.g, rgba.b, rgba.a )
    aaa.draw_fullscreen_quad()
end
```

The widget return value plugs DIRECTLY into a `set_uniform_*` argument.
No marshalling, no copy, no engine layer cell.

### 3.6 `aaa.ui.hsv_color_picker( label, r, g, b, a )`

v2 Phase 2 (c148) -- the real drag-to-hue picker that supersedes the
Phase 1 preset-cycling `color_well`. Built on the Smith 1978 HSV<->RGB
algorithm (citation inline in `src/ui/widgets/aaa_widgets_mac.mm`).

- `label` (string) -- displayed next to the picker.
- `r, g, b, a` (numbers in `[0, 1]`) -- current color.
- Returns FOUR numbers : `r, g, b, a`. Unchanged on idle frames ;
  updated to the freshly picked color when the user drags the hue
  triangle, value bar, or alpha slider.
- Renders as a hue triangle (chroma) + value bar (brightness) + alpha
  slider in a ~140-pixel-tall block. Sized so `begin_panel` heights
  of 280+ accommodate the full picker plus title bar.
- Idiom : `r, g, b, a = aaa.ui.hsv_color_picker( "tint", r, g, b, a )`.
- See `Samples/v2_color_picker/color_picker_demo.lua` for the
  canonical end-to-end demo.

### 3.7 Modals + text input (`begin_modal` / `end_modal` / `show_modal` / `text_input`)

v2 Phase 3 (c148) -- modal-popup dialog surface + single-line keyboard
text input. Together these enable workflows like "rename preset",
"confirm reset", "enter MIDI channel number" without leaving the
.app for a separate text editor.

#### `aaa.ui.show_modal( id )`

Ask the runner to display the modal identified by `id` on subsequent
frames. `id` (string) is an MEU-author-chosen identifier ; the matching
`begin_modal( id, ... )` call will return `open == true` until the
user OKs or Cancels.

- Returns nothing. Idempotent : calling twice in one frame is harmless.
- Side effect : sets a runner-side flag. The flag clears automatically
  when `begin_modal` returns `ok_clicked == true` or
  `cancel_clicked == true`.

#### `aaa.ui.begin_modal( id, w, h ) -> open, ok_clicked, cancel_clicked`

Begin a modal block. Renders OVER the main scene with a darkened
backdrop ; input outside the modal is consumed so widgets in the main
panel can't be clicked while the modal is open.

- `id` (string) -- must match the id passed to `show_modal`.
- `w, h` (integers) -- modal size in pixels. The widget layer centers
  the modal on the window automatically.
- Returns THREE booleans : `open`, `ok_clicked`, `cancel_clicked`.
  `open` is true while the modal is displaying. `ok_clicked` /
  `cancel_clicked` are rising-edge on the frame the user accepts /
  dismisses.
- Pair with exactly one `aaa.ui.end_modal()` per frame the modal is
  open. Skipping `end_modal` when `open == false` is REQUIRED (and
  matches the idiomatic `if open then ... aaa.ui.end_modal() end`).
- Example : `Samples/v2_modal_textinput/modal_textinput_demo.lua`.

#### `aaa.ui.end_modal()`

Close the current modal block.

- Returns nothing.
- Must pair 1:1 with `begin_modal` calls that returned `open == true`.

#### `aaa.ui.text_input( label, value, max_length? )`

Single-line ASCII keyboard input. Accepts characters from
`AAASeedInputView` (c148-A added the responder-chain text-forwarding
path) and returns the updated string each frame.

- `label` (string) -- displayed left of the input field.
- `value` (string) -- current contents of the input.
- `max_length` (integer, optional) -- maximum character count.
  Defaults to 64 ; capped at 256.
- Returns the new string after this frame's keystrokes.
- ASCII-only in Phase 3 ; IME (Japanese / Chinese / Korean) and full
  responder-chain keyboard traversal are v3 work.
- Idiom : `name = aaa.ui.text_input( "name", name, 32 )`.
- See `Samples/v2_modal_textinput/modal_textinput_demo.lua`.

### 3.8 Collapsing panels + hot reload (`begin_collapsing_panel` / `end_collapsing_panel` / `hot_reload_button`)

v2 Phase 4 (c148) -- per-section expand/collapse + a one-button
"rebuild this MEU" affordance that completes the in-app authoring
loop introduced in c147.

#### `aaa.ui.begin_collapsing_panel( title, x, y, w, h ) -> expanded`

Open a collapsing section. The section header is always drawn ; the
contents are drawn ONLY when `expanded == true`.

- `title` (string) -- displayed in the section header.
- `x, y, w, h` (integers) -- section rect in window pixels (NOT
  relative to the enclosing `begin_panel`). One-level nesting only :
  a collapsing panel can live inside `begin_panel` but not inside
  another `begin_collapsing_panel`.
- Returns `expanded` (boolean). On Phase 4 ship, sections start
  expanded ; click the header to toggle.
- Pair with exactly one `aaa.ui.end_collapsing_panel()` per
  `begin_collapsing_panel`.

#### `aaa.ui.end_collapsing_panel()`

Close the current collapsing block.

- Returns nothing.
- Must pair 1:1 with `begin_collapsing_panel`.
- Idiom :
  `if aaa.ui.begin_collapsing_panel("Fog", x, y, w, h) then ... aaa.ui.end_collapsing_panel() end`.

#### `aaa.ui.hot_reload_button( label? )`

A button widget that triggers `Runner::reload()` on click. Returns
rising-edge `true` on the click frame.

- `label` (string, optional) -- defaults to "Reload MEU".
- Side effect : on click, the runner closes the current `lua_State`,
  re-reads the script from disk, re-opens a fresh `lua_State`,
  re-registers all `aaa.*` + `aaa.ui.*` bindings, and re-runs the
  chunk. Lua locals reset to their declared initial values.
- The MEU can use the return value to log / track reload events for
  QA (the click registers BEFORE the reload happens ; the new
  lua_State starts with locals at initial values, so any flag set
  here will be lost on the next frame).
- Idiom : `if aaa.ui.hot_reload_button( "Reload MEU" ) then aaa.log("reloaded") end`.
- See `Samples/v2_collapse_reload/collapse_reload_demo.lua`.

#### Deep nesting (v3, c149)

v2 Phase 4 limited `begin_collapsing_panel` to one level inside
`begin_panel`. v3 (c149-A) removed that restriction : a
`begin_collapsing_panel` can now live inside another
`begin_collapsing_panel` to arbitrary depth. The widget renderer
auto-indents the header chrome (chevron + click region) per depth
so the visual hierarchy reads correctly.

Rules :

- Each `begin_collapsing_panel` MUST be paired with exactly one
  `aaa.ui.end_collapsing_panel()` on frames the panel was expanded.
- Skipping `end_collapsing_panel` when `expanded == false` is
  REQUIRED (the idiomatic `if ... then ... end_collapsing_panel() end`
  pattern handles this automatically).
- The MEU author is responsible for laying out the sub-panel
  rects (no auto-stacking yet) ; depth-2 children typically use
  x = parent_x + 16 to indent visually.
- Collapse state is per-panel-per-session ; it does not persist
  across `Runner::reload()`. Use a v3 preset (section 4) to
  persist parameter values across reloads.

See `Samples/v3_nested_panels/nested_panels_demo.lua` for a
depth-2 example with two sibling sub-panels inside a parent
panel, plus a sibling depth-1 panel.

### 3.9 Multi-line text_area + IME (v4)

v4 (c150-A) lands two extensions to the v2 Phase 3 text-input
surface : a multi-line `aaa.ui.text_area` widget primitive, and
the full `NSTextInputClient` protocol on `AAASeedInputView` so
the macOS Text Input System can route IME composition events
(Chinese, Japanese, Korean, plus emoji + Arabic + accented
Latin via dead keys) through any focused text widget.

Together these close the formal v4 deferrals from section 4's
preamble : multi-line input + CJK composition are now in-app
primitives, no longer "leave the app for a separate text
editor" work.

#### `aaa.ui.text_area( label, value, visible_lines, width_chars, max_length )`

Multi-line text input widget. Sibling to `aaa.ui.text_input` ;
accepts newlines + arbitrary length text up to `max_length`.

- `label` (string) -- displayed left of the input field.
- `value` (string) -- current contents of the input. Newlines
  (`\n`) in the value are preserved + rendered as line breaks.
- `visible_lines` (integer) -- the number of glyph rows of
  vertical space the renderer reserves for the widget. Text
  beyond `visible_lines` rows scrolls inside the widget ;
  cursor stays visible.
- `width_chars` (integer) -- the glyph-cell width the renderer
  reserves. Glyphs beyond column `width_chars` wrap to the next
  line. Glyph-cell based ; proportional-font wrap is future
  polish, not a v5 feature.
- `max_length` (integer) -- maximum character count. Hard cap
  at 8192 ; values above are clamped.
- Returns the new string after this frame's keystrokes.
  Same return-and-reassign idiom as `text_input` + slider.
- Idiom :
  `notes = aaa.ui.text_area( "notes", notes, 8, 48, 1024 )`.
- See `Samples/v4_multiline/multiline_demo.lua`.

#### IME composition (production path)

With a CJK input source active in System Settings, typing into
ANY focused `aaa.ui.text_input` or `aaa.ui.text_area` routes
through the macOS Text Input System :

1. Each keystroke generates a `marked text` event with an
   underline-rendered pending composition (the squiggly line
   under typed romaji on a Japanese IME, the pinyin display on
   a Chinese IME, the hangul jamos on a Korean 2-Set IME).
2. The IME's commit gesture (Enter on most IMEs ; Space on
   some Chinese IMEs to confirm the top candidate) commits the
   composition : marked text clears, the committed string
   inserts at the cursor.
3. Escape clears pending marked text without committing.

The c150-A `NSTextInputClient` protocol implementation in
AAASeedInputView covers all 10+ required methods :
`setMarkedText:selectedRange:replacementRange:`, `unmarkText`,
`hasMarkedText`, `markedRange`, `selectedRange`,
`validAttributesForMarkedText`, `attributedSubstringFor-
ProposedRange:actualRange:`, `firstRectForCharacterRange:-
actualRange:`, `characterIndexForPoint:`, plus the
`insertText:replacementRange:` final-commit hook.

#### `aaa.ime.set_marked_text( text, sel_start, sel_len )`

Synthetic test injection that drives the same code path the
production IME uses. Lets integration tests verify marked-text
rendering + commit transitions without requiring a CJK input
source on the test host.

- `text` (string) -- the pending composition string to display
  with the marked-text underline.
- `sel_start` (integer) -- selection start within `text`,
  0-based.
- `sel_len` (integer) -- selection length within `text`. The
  pair `(sel_start, sel_len)` matches the
  `NSTextInputClient` `selectedRange` semantics ; pass
  `(0, length)` to select the whole pending composition.
- Returns nothing.
- Side effect : sets the focused text widget's marked-text
  buffer + selection. The widget renderer paints the marked
  text with an underline on the next frame.
- Idiom : `aaa.ime.set_marked_text( "ko", 0, 2 )` (from a
  test harness ; on a CJK keyboard this happens automatically).
- See `Samples/v4_ime_text/ime_text_demo.lua` for an end-to-end
  synthetic IME round-trip driven from Lua.

#### `aaa.ime.commit_marked_text()`

Synthetic test injection that commits the focused widget's
pending marked text. Equivalent to the IME calling `insertText:`
with the finalised composition after the user accepts.

- Returns nothing.
- Side effect : the marked-text buffer empties ; the committed
  string inserts at the cursor of the focused text widget ;
  the widget's return-value string updates so the MEU's Lua
  local catches up via the return-and-reassign idiom.
- Idiom : `aaa.ime.commit_marked_text()` (typically called by
  a test after `set_marked_text` ; on a CJK keyboard this
  happens automatically on Enter).

#### How to type CJK characters (artist setup)

1. Open System Settings.
2. Keyboard -> Input Sources -> Edit -> +.
3. Choose a language and an input method :
   - Chinese : Pinyin Simplified, Pinyin Traditional, Cangjie,
     Wubi Hua, Stroke Simplified, Zhuyin, etc.
   - Japanese : Romaji, Kana, or split-Romaji+Kana.
   - Korean : 2-Set Korean (default), 3-Set Korean (390 / Final).
4. Add the input source ; the menu-bar Input Source switcher
   (or Ctrl-Space by default) toggles between US English and
   your CJK IME.
5. Switch to the CJK IME, focus an AAASeed text widget, and
   type normally. Marked-text underline appears under pending
   composition characters ; Enter (or the IME-specific commit
   gesture) commits.

#### What v4 text + IME does NOT do

- **Bundled IME engines** : the .app does not include its own
  IME ; it relies on the system IME chain. Per the v4 milestone
  memo's out-of-scope list.
- **Bidi text layout** (Arabic / Hebrew) : the renderer paints
  glyphs left-to-right ; full bidi reordering is a polish
  item, not a v5 feature.
- **Find / replace + undo / redo stacks** : the text_area
  widget tracks a single linear edit history (no Cmd-Z stack).
  Multi-step undo is maintenance work, not a new version.

### What widgets do NOT do in v2 Phases 1-4

The following are deferred to v3 (re-prioritize after v2 user
feedback ; see `memory/project_v1_ship_gate.md` "Authoring Surface"
section) :

- **Drag-drop file targets** -- no `aaa.ui.drop_target` ; quarantine
  + sandbox concerns push this to v3.
- **Multi-line text widgets** -- `aaa.ui.text_input` is single-line ;
  multi-line + scroll is v4 work (now LANDED in c150-A,
  see section 3.9 `aaa.ui.text_area`).
- **Full keyboard focus traversal** -- Tab between text inputs +
  Escape to close modal are v3 (NSResponder chain integration).
- **Lua-side `NSOpenPanel` file picker** -- c131-B already has the
  C++ dialog adapter ; wiring to a Lua `aaa.ui.open_file_dialog`
  binding is v3.
- **Save / load preset state** -- the Phase 3 text input lets the
  user TYPE a preset name ; persisting state to disk is v3.
- **Deep panel nesting** -- one level of `begin_collapsing_panel`
  inside `begin_panel` ; deeper is v3 (now LANDED in c149-A,
  see section 3.8 "Deep nesting").

---

## 4. `aaa.io.*` file I/O API (v3)

The `aaa.io.*` namespace exposes the Mac-native file-dialog +
preset persistence + drag-drop synthetic surface introduced in
v3 (c149-A). Bindings are installed by
`src/meu/aaa_meu_runner_mac.mm`'s `install_aaa_bindings()`
alongside the `aaa.*` core surface + the `aaa.ui.*` widget
surface from v2. All five are call-from-anywhere (no
panel-stack requirement) ; the dialog bindings spin a blocking
Cocoa sheet so they should not be called from a tight
per-frame loop body.

This API completes the file-I/O half of the in-app authoring
loop that v2 began :

- v1 (c142-B) : MEU runner + Lua chunk + Path A shader catalog.
- v2 (c147 / c148) : in-app widgets to drive uniforms.
- v3 (c149) : load other MEUs via drag-drop or dialog ;
  save / load widget state as portable `.lua` presets.

### `aaa.io.drop_file( path )`

Synthetic test injection that simulates an NSWindow drag-drop
event. The PRODUCTION drag-drop path is the AAASeedMTKView
`registerForDraggedTypes:` + `performDragOperation:` responder
chain c149-A wired into the host view : the user drags a `.lua`
from Finder onto the window, the runner's drag-drop hook calls
`Runner::load_script` and re-runs the chunk.

- `path` (string) -- absolute path to a `.lua` file. Non-`.lua`
  extensions are rejected at the responder-chain level for the
  production path ; the synthetic injection follows the same
  filter for parity.
- Returns nothing.
- Use case : integration tests that need a deterministic
  "user dragged this file" event without an NSEvent loop.
- Idiom : `aaa.io.drop_file( "/tmp/test_meu.lua" )` (from a
  test harness, NOT from an `on_frame` body in production --
  it would reload the MEU mid-frame and discard the caller's
  stack).
- See `Samples/v3_drag_drop/drag_drop_demo.lua`.

### `aaa.io.open_file_dialog( title, ext_list ) -> path_or_nil`

Lua-callable wrapper around the c131-B NSOpenPanel adapter
(`src/ui/macos/aaa_file_dialog.{h,mm}`). Opens a blocking sheet
dialog ; returns the chosen path string or nil if the user
cancelled.

- `title` (string) -- dialog title bar text.
- `ext_list` (Lua array of strings) -- allowed extensions
  WITHOUT leading dots. E.g. `{ "lua" }` or `{ "lua", "plua" }`
  or `{ "png", "jpg", "exr" }`.
- Returns either an absolute path string (on accept) or `nil`
  (on cancel).
- Idiom :
  ```lua
  local path = aaa.io.open_file_dialog(
      "Choose a .lua MEU script", { "lua" } )
  if path then aaa.log( "selected " .. path ) end
  ```
- See `Samples/v3_drag_drop/drag_drop_demo.lua`.

### `aaa.io.save_file_dialog( title, default_name, ext_list ) -> path_or_nil`

Lua-callable wrapper around the c131-B NSSavePanel adapter.
Same return contract as `open_file_dialog` but for SAVING : the
user picks a destination, Cocoa prompts on overwrite, the call
returns the chosen path or nil on cancel.

- `title` (string) -- dialog title bar text.
- `default_name` (string) -- pre-filled into the Cocoa text
  field. The user can change it before clicking Save.
- `ext_list` (Lua array of strings) -- allowed extensions
  WITHOUT leading dots. Cocoa appends the first extension if
  the user typed a name without one.
- Returns either an absolute path string (on accept) or `nil`
  (on cancel).
- Idiom :
  ```lua
  local path = aaa.io.save_file_dialog(
      "Save preset", "my_preset.lua", { "lua" } )
  ```
- See `Samples/v3_preset_save_load/preset_demo.lua`.

### `aaa.io.save_preset( path ) -> bool`

Serializes the current widget-state map (all `aaa.ui.*` widget
values the runner has captured this frame) to a Lua-table file
at `path`. Returns `true` on success ; `false` on I/O error
(unwritable path, full disk, etc.).

- `path` (string) -- absolute path to write to. If the file
  exists it is overwritten ; if its directory does not exist
  the call returns `false`.
- The on-disk format is a single `return { ... }` chunk so the
  file is hand-editable + version-control-friendly :
  ```lua
  return {
      intensity = 1.0,
      main_color = { 1.0, 0.5, 0.2, 1.0 },
      -- one entry per registered widget id
  }
  ```
- Returns `bool`.
- Idiom :
  ```lua
  if path and aaa.io.save_preset( path ) then
      status_msg = "Saved : " .. path
  end
  ```
- See `Samples/v3_preset_save_load/preset_demo.lua`.

### `aaa.io.load_preset( path ) -> bool`

Reverse of `save_preset` : reads the Lua-table file at `path`,
restores widget state to the saved values. Returns `true` on
success ; `false` on I/O / parse error.

- `path` (string) -- absolute path to read from.
- Widgets whose IDs no longer exist in the running MEU are
  silently skipped (forward-compatible -- a preset saved by an
  older MEU loads cleanly into a newer one that dropped some
  widgets).
- After a successful load, the next `aaa.ui.slider` /
  `aaa.ui.hsv_color_picker` / etc. calls return the restored
  values, so the Lua locals catch up automatically through the
  return-and-reassign idiom.
- Returns `bool`.
- Idiom :
  ```lua
  if path and aaa.io.load_preset( path ) then
      status_msg = "Loaded : " .. path
  end
  ```
- See `Samples/v3_preset_save_load/preset_demo.lua`.

### What `aaa.io.*` does NOT do in v3

- **Sandbox-aware file access** : the v1 ship is non-sandboxed,
  so all paths are accessible to the running process.
  App-Sandbox containers are out-of-scope for v1.
- **Directory watching for hot reload** : c149-A's
  `aaa_file_watcher_mac` watches the loaded script file's
  mtime ; watching an entire `Samples/` tree (auto-reload on
  any change) is v3.1.
- **Auto-save on quit / restore on launch** : would need a
  canonical "last preset" path ; v3.1.
- **IME-aware text input in the dialog title** : NSOpenPanel /
  NSSavePanel use Cocoa's own IME path, so this is free at the
  dialog level, but the in-MEU `aaa.ui.text_input` widget
  remains ASCII-only (v4, see
  `memory/project_v4_milestone.md`).

---

## 4.5. Multi-line text + IME (v4 — final milestone)

v4 adds multi-line text entry and IME (Chinese / Japanese / Korean
composed-input) support. The widget system gains a `text_area` primitive
for longer text + a marked-text composition path for non-Latin scripts.
This is the FINAL feature milestone — per user mandate, no v5+ versions.

### `aaa.ui.text_area( label, value, visible_lines, width_chars, max_length ) -> new_value:string`

Multi-line editable text. Word-wraps within `width_chars`. Enter inserts
a newline. Scroll appears when `lines > visible_lines`.

```lua
local notes = "line one\nline two\nline three"

function aaa.on_frame( w, h, frame )
    aaa.ui.begin_panel( "notes", 16, 16, 420, 320 )
    notes = aaa.ui.text_area( "notes", notes, 8, 48, 1024 )
    aaa.ui.end_panel()
    aaa.use_shader( "aaa_curl_noise_real" )
    aaa.set_uniform_int( 0, 1 )
    aaa.draw_fullscreen_quad()
end
```

### IME composition (CJK input)

On a Mac with a CJK input source active (System Settings → Keyboard →
Input Sources → Add → Pinyin / Hiragana / Hangul), typing into a focused
`aaa.ui.text_input` or `aaa.ui.text_area` produces **marked text** — the
composing characters displayed with an underline until you press Enter
(commits) or Escape (clears). Marked text routes via the NSTextInputClient
protocol implemented in `AAASeedInputView` → `WidgetSystem::on_marked_text`
→ rendered with an underline in the same frame.

### `aaa.ime.set_marked_text( composing, sel_start, sel_len )`

Synthetic-injection path. Tests + scripts use this to drive composition
state without an actual CJK keyboard.

### `aaa.ime.commit_marked_text()`

Commits the current marked text into the focused widget (analogous to
the user pressing Enter on a CJK composition).

### Sample MEUs

- `Samples/v4_multiline/multiline_demo.lua` — text_area driving curl-noise
  shader. Shows newlines, scroll, save-as.
- `Samples/v4_ime_text/ime_text_demo.lua` — single-line text_input with
  IME composition + the synthetic `aaa.ime.*` demo path.

### Honest gap : interactive CJK keyboard input

The NSTextInputClient protocol plumbing + WidgetSystem state machine are
unit + integration tested via synthetic `aaa.ime.set_marked_text` calls
(see `tests/unit/aaa_widgets_v4_test.cpp` `WidgetsV4Cjk.*` round-trips for
Japanese こ / Chinese 你 / Korean 안). Final-mile verification of actual
keystrokes through Pinyin / Hiragana / Hangul keyboards requires a human
at a Mac with the input source enabled — autonomous tests don't reach
the OS input-method server.

---

## 5. Path A shader catalog

The visual building blocks live as `.metal` files at
`src/shaders/msl/` (source) and ship into the .app at
`Resources/shaders/` (bundled by `src/ui/macos/CMakeLists.txt`). 170+
shaders total, of which 11 are the **high-quality real-algorithm
cohort** revived from stubs in sessions c128 through c140 :

| Shader stem                     | Algorithm                                | Session |
|---------------------------------|------------------------------------------|---------|
| `fxaa_lottes`                   | Lottes FXAA 3.11                          | c128-A  |
| `ifs_de_library`                | Real IFS distance-estimator library       | c130-A  |
| `aaa_material_pbr`              | Cook-Torrance PBR                         | c134-B  |
| `aaa_noise_real`                | Perlin + Simplex + FBM                    | c135-A  |
| `aaa_gol_real`                  | Conway + Brian's Brain                    | c136-A  |
| `aaa_curl_noise_real`           | Bridson 2007 divergence-free curl noise   | c137-A  |
| `aaa_bloom_real`                | Lottes-style bloom (run-then-readback)    | c138-A  |
| `aaa_motion_blur_real`          | Camera-velocity projected blur            | c139-A  |
| `aaa_dof_hex_bokeh_real`        | McIntosh hex bokeh DoF                    | c140-A  |
| `aaa_cam_real`                  | Real camera projection (matrix)           | c134-B  |

The remaining shaders are baseline / decorative / experimental. All are
usable via `aaa.use_shader( "<stem>" )` from any MEU.

---

## 6. How to author a new MEU

1. Copy any of `bundle/macos/meu/Samples/<name>/` to a new directory.
2. Rename the `.lua` to match your MEU name.
3. Edit the `aaa.on_frame()` body to select your shader + uniforms.
4. Validate syntax : `luac -p path/to/your.lua`.
5. To load your MEU at app launch :
   - Replace the contents of `bundle/macos/meu/hello_world.lua`, OR
   - Modify `src/ui/macos/AAASeedMTKView.mm` to point at your file,
     OR (preferred for development) call `runner.reload()` after
     swapping the file on disk if your host wiring exposes that path.
6. Rebuild `aaaseed_app` (or trigger reload — see section 7).
7. Run the .app and verify your changes.

---

## 7. Hot-reload status

`aaa::meu::Runner::reload()` (see `src/meu/aaa_meu_runner_mac.h`) :

- Closes the current `lua_State`.
- Re-reads the same script file from disk (so external edits land).
- Re-opens a fresh `lua_State`, re-registers all `aaa.*` bindings, and
  re-runs the chunk.
- Returns `true` on success ; `false` if no script was loaded or the
  re-load errored.

**Live in v1** : YES, the runner supports reload at the API level.

**Not wired in v1** : there is no keyboard shortcut or file-watch path
in `AAASeedMTKView` that calls `reload()` automatically. To exercise it,
either add a key handler in `AAASeedMTKView.mm` that calls
`_runner->reload()`, or rebuild the app after editing.

This is a known v2 polish gap.

---

## 8. What's NOT supported in v1

- **Windows-style GaBu BU widgets in-app** — sliders, buttons, color
  wells, grid pickers, texture target picklists. The Windows authoring
  flow renders these inside the engine window via `AAAKernel/GaBu/`.
  Mac v1 does not have a GaBu UI render path.
- **`fx.aaa_layers_all` engine layer serialization** — the Layer
  subsystem was superseded per c144-B (`memory/project_layer_supersession.md`).
  Loading a Windows-format MEU folder will not work.
- **MEU folder hierarchy + presets** — the runner takes a single `.lua`
  file. Multi-file MEUs with preset cascades are v2 work.
- **C++ Win32 modal parameter dialogs** (`Src/ui/seed_ui.cpp`) — Mac
  port deferred ; see v2 plan below.
- **In-app visual node editor** — does not exist on Windows either,
  per c145-C confirmation. v2 would be a net-new capability.

---

## 9. v2 plan : visual parameter UI

Two paths under consideration (the v1 ship-gate doctrine at
`memory/project_v1_ship_gate.md` "Authoring Surface" section captures
this) :

**Option A — Mac-native NSPanel parameter widgets.** Port the Windows
`c_param` surface to Cocoa NSPanel / NSSlider / NSColorWell using the
c134-A `void*` bridge doctrine (no Objective-C types in C++ headers).
This matches the Windows UX one-for-one.

**Option B — In-app Metal node editor.** Build a node-graph authoring
surface inside the .app (Metal-rendered nodes + connections, mouse +
keyboard authoring). Would be a NEW capability not present on Windows.

The choice is open ; the v1 ship can ship without either.

---

## 10. Existing sample MEUs

All sample MEUs live under `bundle/macos/meu/Samples/` and ship into
the .app at `Resources/meu/Samples/` :

- `Samples/mire/mire.lua` — test-pattern cycler. SPACE to advance.
- `Samples/animator/animator.lua` — time-driven curl-noise animation.
- `Samples/keyboard/keyboard.lua` — 7-shader selector via digit keys +
  arrows.
- `Samples/mouse/mouse.lua` — cursor XY drives shader uniforms.
- `Samples/composer/composer.lua` — alternating-frames multi-shader
  composition (curl-noise + bloom).
- `Samples/v2_widgets/widgets_demo.lua` — **v2 Phase 1 canonical demo
  (c147).** Uses `aaa.ui.*` widgets (button / slider / color-well) to
  drive Path A shader uniforms end-to-end. Start here when learning
  the v2 authoring surface.
- `Samples/v2_color_picker/color_picker_demo.lua` — **v2 Phase 2
  canonical demo (c148).** Uses `aaa.ui.hsv_color_picker` to drive a
  Path A shader tint uniform via the Smith 1978 HSV<->RGB algorithm.
- `Samples/v2_modal_textinput/modal_textinput_demo.lua` — **v2 Phase 3
  canonical demo (c148).** Combines `aaa.ui.show_modal` +
  `aaa.ui.begin_modal` + `aaa.ui.text_input` + `aaa.ui.end_modal` for
  a "rename preset" workflow.
- `Samples/v2_collapse_reload/collapse_reload_demo.lua` — **v2 Phase 4
  canonical demo (c148).** Two collapsing sections + a hot-reload
  button driving `aaa_bloom_real` end-to-end.
- `Samples/v3_drag_drop/drag_drop_demo.lua` — **v3 canonical demo
  (c149).** Drag-drop file loading + `aaa.io.open_file_dialog`
  browse over `aaa_noise_real`.
- `Samples/v3_preset_save_load/preset_demo.lua` — **v3 canonical
  demo (c149).** `aaa.io.save_preset` + `aaa.io.load_preset`
  round-trip via NSSavePanel / NSOpenPanel driving
  `aaa_bloom_real`.
- `Samples/v3_nested_panels/nested_panels_demo.lua` — **v3
  canonical demo (c149).** Depth-2 nested collapsing panels +
  sibling depth-1 panel driving `aaa_bloom_real`.
- `Samples/v4_ime_text/ime_text_demo.lua` — **v4 canonical demo
  (c150).** IME composition + commit over `aaa.ui.text_input`
  via the `NSTextInputClient` protocol path, plus
  `aaa.ime.set_marked_text` / `aaa.ime.commit_marked_text`
  synthetic round-trip driving `aaa_noise_real`.
- `Samples/v4_multiline/multiline_demo.lua` — **v4 canonical
  demo (c150).** Multi-line `aaa.ui.text_area` widget +
  v3 `aaa.io.save_file_dialog` save flow driving
  `aaa_curl_noise_real`.

Each sample has its own `README.md` explaining what it does, the Path A
shaders it uses, and how to modify it. Together they exercise every
binding documented in sections 2 + 3 + 4.
