# Lua API : `aaa.ui.*` (widgets)

> Reference for the 14 immediate-mode widget bindings under the
> `aaa.ui.*` namespace, landed across v2 Phases 1-4 (c147 / c148) and
> v3 / v4 (c149 / c150). Source : `src/meu/aaa_meu_runner_mac.{h,mm}`
> + `src/ui/widgets/aaa_widgets_mac.{h,mm}`.

These widgets render inside `AAASeedMTKView`'s own Metal pipeline.
No Cocoa controls, no NSPanel, no GaBu render path. They are the
spiritual successor to the Windows GaBu BU widget shelf, reimagined
as an immediate-mode API.

---

## Two universal conventions

### begin / end pair convention

Every `begin_*` call MUST pair with exactly one matching `end_*` call
in the same frame. The pairs are :

- `begin_panel` <-> `end_panel`
- `begin_modal` <-> `end_modal` (only when modal is `open`)
- `begin_collapsing_panel` <-> `end_collapsing_panel` (only when
  section is `expanded`)

Skipping the `end_*` when the conditional says you should NOT call it
is REQUIRED. The idiomatic pattern :

```lua
if aaa.ui.begin_collapsing_panel( "Fog", x, y, w, h ) then
    -- ... widgets in the expanded section ...
    aaa.ui.end_collapsing_panel()
end
```

### Retained state by label

Widget state (slider position, color, collapse expanded-flag, etc.)
is keyed by the **label string** you pass. Two widgets with the same
label in the same panel share state. The widget owns the live value ;
your script's job is to hand in the current value and assign the
returned value back to your Lua local :

```lua
intensity = aaa.ui.slider( "intensity", intensity, 0.0, 2.0 )
```

This is the **return-and-reassign idiom** -- it works for `slider`,
`color_well`, `hsv_color_picker`, `text_input`, and `text_area`.

---

## `aaa.ui.begin_panel( title, x, y, w, h ) -> handle`

Open a UI container at screen-space rect `(x, y, w, h)` and start
accepting widget calls into it.

- `title` (string) -- displayed at the top of the panel.
- `x, y` (integers) -- top-left pixel coordinates in window space.
  `y` is measured from the top edge.
- `w, h` (integers) -- pixel size of the panel.
- Returns an integer handle (opaque ; reserved for future per-panel
  state queries -- you can ignore it in v2).
- **Side effect** : pushes a panel onto the runner's widget stack.
  MUST pair with exactly one `aaa.ui.end_panel()` per frame.

```lua
aaa.ui.begin_panel( "Controls", 16, 16, 320, 220 )
-- ... widget calls go here ...
aaa.ui.end_panel()
```

See also : [`end_panel`](#aaauiend_panel---nil),
[`begin_collapsing_panel`](#aaauibegin_collapsing_panel-title-x-y-w-h---expanded).

---

## `aaa.ui.end_panel() -> nil`

Close the current panel.

- Returns nothing.
- **Side effect** : pops the panel off the widget stack and submits
  accumulated draw commands to the host renderer. Widgets composite
  OVER the next `aaa.draw_fullscreen_quad()` call this frame.
- MUST pair 1:1 with `aaa.ui.begin_panel`.

See also : [`begin_panel`](#aaauibegin_panel-title-x-y-w-h---handle).

---

## `aaa.ui.slider( label, value, min, max ) -> new_value`

Add a horizontal scrubber to the current panel.

- `label` (string) -- displayed left of the bar.
- `value` (number) -- current value.
- `min, max` (numbers) -- bounds. `min < max` is required.
- Returns the live value : equal to `value` on idle frames, clamped
  to `[min, max]` when the user is dragging.

```lua
local intensity = 0.5

function aaa.on_frame( w, h, frame )
    aaa.ui.begin_panel( "Controls", 16, 16, 320, 80 )
    intensity = aaa.ui.slider( "intensity", intensity, 0.0, 2.0 )
    aaa.ui.end_panel()
    -- ... drive shader uniform from intensity ...
end
```

See also : [`button`](#aaauibutton-label---clicked),
[`hsv_color_picker`](#aaauihsv_color_picker-label-r-g-b-a---r-g-b-a).

---

## `aaa.ui.button( label ) -> clicked`

Add a button to the current panel.

- `label` (string) -- displayed inside the button.
- Returns `true` only on the **rising-edge frame** the user clicked
  (the widget layer debounces internally so two consecutive frames
  never both return true from one click).
- **Side effect** : laid out vertically below the previous widget.

```lua
if aaa.ui.button( "Reset" ) then
    intensity = 0.5
    tint = { r = 1, g = 1, b = 1, a = 1 }
end
```

See also : [`hot_reload_button`](#aaauihot_reload_button-label----clicked).

---

## `aaa.ui.color_well( label, r, g, b, a ) -> r, g, b, a`

Preset-cycling color swatch (v2 Phase 1 -- superseded by
`hsv_color_picker` in Phase 2, but kept for backwards compatibility
and for cases where a curated palette is what you want).

- `label` (string) -- displayed next to the swatch.
- `r, g, b, a` (numbers in `[0, 1]`) -- current color.
- Returns FOUR numbers : `r, g, b, a`. Unchanged on idle frames ;
  advanced to the next preset (~8 curated hues) on click frames.

```lua
local r, g, b, a = 1.0, 0.5, 0.2, 1.0
-- in on_frame :
r, g, b, a = aaa.ui.color_well( "tint", r, g, b, a )
```

See also : [`hsv_color_picker`](#aaauihsv_color_picker-label-r-g-b-a---r-g-b-a)
for the drag-to-hue picker.

---

## `aaa.ui.hsv_color_picker( label, r, g, b, a ) -> r, g, b, a`

v2 Phase 2 (c148) -- drag-to-hue HSV color picker built on the
Smith 1978 HSV<->RGB algorithm.

- `label` (string) -- displayed next to the picker.
- `r, g, b, a` (numbers in `[0, 1]`) -- current color.
- Returns FOUR numbers : `r, g, b, a`. Unchanged on idle frames ;
  updated to the freshly picked color when the user drags inside the
  hue triangle, value bar, or alpha slider.
- Renders as : hue triangle (chroma) + value bar (brightness) +
  alpha slider, ~140 pixels tall. Size your panel >= 280 px tall.

```lua
local tint = { r = 1.0, g = 0.5, b = 0.2, a = 1.0 }

function aaa.on_frame( w, h, frame )
    aaa.ui.begin_panel( "Color", 16, 16, 360, 280 )
    tint.r, tint.g, tint.b, tint.a = aaa.ui.hsv_color_picker(
        "tint", tint.r, tint.g, tint.b, tint.a )
    aaa.ui.end_panel()

    aaa.use_shader( "aaa_noise_real" )
    aaa.set_uniform_int( 0, 1 )
    aaa.set_uniform_vec4( 0, tint.r, tint.g, tint.b, tint.a )
    aaa.draw_fullscreen_quad()
end
```

See also : [Sample MEU : v2_color_picker](../samples.md#v2_color_picker).

---

## `aaa.ui.show_modal( id ) -> nil`

Ask the runner to display the modal identified by `id` on subsequent
frames.

- `id` (string) -- an MEU-author-chosen identifier ; must match the
  `id` you later pass to `begin_modal`.
- Returns nothing. Idempotent : calling twice in one frame is
  harmless.
- **Side effect** : sets a runner-side flag. The flag clears
  automatically when `begin_modal` returns `ok_clicked == true` or
  `cancel_clicked == true`.

See [`begin_modal`](#aaauibegin_modal-id-w-h---open-ok_clicked-cancel_clicked) below for
the full pattern.

---

## `aaa.ui.begin_modal( id, w, h ) -> open, ok_clicked, cancel_clicked`

Begin a modal block. Renders OVER the main scene with a darkened
backdrop ; input outside the modal is consumed.

- `id` (string) -- must match the id passed to `show_modal`.
- `w, h` (integers) -- modal size in pixels. The widget layer centers
  the modal on the window automatically.
- Returns THREE booleans : `open`, `ok_clicked`, `cancel_clicked`.
  - `open` -- true while the modal is displaying.
  - `ok_clicked` / `cancel_clicked` -- rising-edge on the frame the
    user accepts / dismisses.
- Pair with exactly one `aaa.ui.end_modal()` per frame the modal is
  open. Skipping `end_modal` when `open == false` is REQUIRED.

```lua
local rename_buffer = "default"
local preset_name = "default"

function aaa.on_frame( w, h, frame )
    aaa.ui.begin_panel( "Main", 16, 16, 340, 140 )
    if aaa.ui.button( "Rename preset..." ) then
        rename_buffer = preset_name
        aaa.ui.show_modal( "rename" )
    end
    aaa.ui.end_panel()

    local open, ok, cancel = aaa.ui.begin_modal( "rename", 280, 140 )
    if open then
        rename_buffer = aaa.ui.text_input(
            "name", rename_buffer, 32 )
        if ok then preset_name = rename_buffer end
        aaa.ui.end_modal()
    end
end
```

See also : [`show_modal`](#aaauishow_modal-id---nil),
[`end_modal`](#aaauiend_modal---nil),
[Sample MEU : v2_modal_textinput](../samples.md#v2_modal_textinput).

---

## `aaa.ui.end_modal() -> nil`

Close the current modal block.

- Returns nothing.
- MUST pair 1:1 with `begin_modal` calls that returned `open == true`.

See also : [`begin_modal`](#aaauibegin_modal-id-w-h---open-ok_clicked-cancel_clicked).

---

## `aaa.ui.text_input( label, value, max_length? ) -> new_value`

Single-line keyboard text input. CJK / IME composition is supported
through the `NSTextInputClient` path in `AAASeedInputView` -- with a
CJK input source active in System Settings, typing into this widget
produces marked-text underline rendering during composition + commits
on Enter.

- `label` (string) -- displayed left of the input field.
- `value` (string) -- current contents.
- `max_length` (integer, optional) -- maximum character count.
  Defaults to 64 ; capped at 256.
- Returns the new string after this frame's keystrokes.

```lua
local name = "untitled"

-- in on_frame :
name = aaa.ui.text_input( "name", name, 32 )
```

See also : [`text_area`](#aaauitext_area-label-value-visible_lines-width_chars-max_length---new_value)
for multi-line, [Lua API : IME](ime.md) for the synthetic injection
path used in tests.

---

## `aaa.ui.text_area( label, value, visible_lines, width_chars, max_length ) -> new_value`

v4 (c150) -- multi-line text input. Newlines in `value` are preserved
+ rendered as line breaks. Text beyond `visible_lines` rows scrolls
inside the widget.

- `label` (string) -- displayed left of the input field.
- `value` (string) -- current contents. `\n` in the value renders as
  a line break.
- `visible_lines` (integer) -- vertical glyph rows reserved.
- `width_chars` (integer) -- glyph-cell width. Glyphs beyond
  `width_chars` wrap.
- `max_length` (integer) -- maximum character count. Hard cap 8192.
- Returns the new string after this frame's keystrokes.

```lua
local notes = "line one\nline two\nline three"

-- in on_frame :
notes = aaa.ui.text_area( "notes", notes, 8, 48, 1024 )
```

See also : [Sample MEU : v4_multiline](../samples.md#v4_multiline).

---

## `aaa.ui.hot_reload_button( label? ) -> clicked`

A button that triggers `Runner::reload()` on click. Returns
rising-edge `true` on the click frame.

- `label` (string, optional) -- defaults to `"Reload MEU"`.
- **Side effect** : on click, the runner closes the current
  `lua_State`, re-reads the script from disk, re-opens a fresh
  `lua_State`, re-registers all bindings, and re-runs the chunk.
  Lua locals reset to their declared initial values.

```lua
if aaa.ui.hot_reload_button( "Reload MEU" ) then
    aaa.log( "reload requested" )
end
```

The click registers BEFORE the reload happens ; the new `lua_State`
starts with locals at initial values, so any flag set here will be
lost on the next frame. Use a preset
([`aaa.io.save_preset`](io.md#aaaiosave_preset-path---bool)) to
persist values across reloads.

See also : [`begin_collapsing_panel`](#aaauibegin_collapsing_panel-title-x-y-w-h---expanded),
[Sample MEU : v2_collapse_reload](../samples.md#v2_collapse_reload).

---

## `aaa.ui.begin_collapsing_panel( title, x, y, w, h ) -> expanded`

v2 Phase 4 (c148) + v3 deep-nesting (c149-A) -- expand / collapse a
section header. Contents are drawn ONLY when `expanded == true`.

- `title` (string) -- displayed in the section header.
- `x, y, w, h` (integers) -- section rect in window pixels (NOT
  relative to the enclosing `begin_panel`).
- Returns `expanded` (boolean). Sections start expanded ; click the
  header to toggle.
- v3 allows arbitrary nesting depth. Depth-2 children typically use
  `x = parent_x + 16` to indent visually -- there is no auto-layout
  in v1.
- Pair with exactly one `aaa.ui.end_collapsing_panel()` per frame the
  panel is expanded.

```lua
aaa.ui.begin_panel( "Main", 16, 16, 380, 360 )

if aaa.ui.begin_collapsing_panel( "Effects", 16, 48, 380, 200 ) then
    if aaa.ui.begin_collapsing_panel( "Color", 32, 80, 360, 60 ) then
        intensity = aaa.ui.slider( "intensity", intensity, 0, 2 )
        aaa.ui.end_collapsing_panel()
    end
    if aaa.ui.begin_collapsing_panel( "Bloom", 32, 144, 360, 80 ) then
        radius = aaa.ui.slider( "radius", radius, 1, 16 )
        aaa.ui.end_collapsing_panel()
    end
    aaa.ui.end_collapsing_panel()
end

aaa.ui.end_panel()
```

See also : [Sample MEU : v3_nested_panels](../samples.md#v3_nested_panels).

---

## `aaa.ui.end_collapsing_panel() -> nil`

Close the current collapsing block.

- Returns nothing.
- MUST pair 1:1 with each `begin_collapsing_panel` that returned
  `expanded == true`.

See also : [`begin_collapsing_panel`](#aaauibegin_collapsing_panel-title-x-y-w-h---expanded).

---

## What widgets do NOT do in v1

These surfaces were considered and deferred ; do not author MEUs that
depend on them :

- **Sandbox-aware file dialogs** -- v1 is non-sandboxed ; dialogs
  return absolute paths.
- **Tab / Shift-Tab keyboard focus traversal between widgets** -- the
  responder-chain hookup is incomplete in v1.
- **Find / replace + multi-step undo inside `text_area`** -- a single
  linear edit history, no Cmd-Z stack.
- **Bidi / RTL text layout** in `text_input` / `text_area` -- glyphs
  render left-to-right ; full bidi is a polish item.
- **Tooltips on hover** -- not implemented.
- **Drag-handles to resize panels at runtime** -- panel rects are
  fixed in your `begin_panel` call.

These are not blockers for the v1 authoring surface ; design around
them.

---

## Composing widgets with shader uniforms

The canonical pattern (see [Sample MEU : v2_widgets](../samples.md#v2_widgets)) :

```lua
local intensity = 0.5
local rgba = { r = 1.0, g = 0.5, b = 0.2, a = 1.0 }

function aaa.on_frame( w, h, frame )
    aaa.ui.begin_panel( "Controls", 16, 16, 320, 220 )
    if aaa.ui.button( "Reset" ) then intensity = 0.5 end
    intensity = aaa.ui.slider( "intensity", intensity, 0.0, 2.0 )
    rgba.r, rgba.g, rgba.b, rgba.a = aaa.ui.hsv_color_picker(
        "tint", rgba.r, rgba.g, rgba.b, rgba.a )
    aaa.ui.end_panel()

    aaa.use_shader( "aaa_bloom_real" )
    aaa.set_uniform_int( 0, 1 )
    aaa.set_uniform_float( 0, intensity )
    aaa.set_uniform_vec4( 0, rgba.r, rgba.g, rgba.b, rgba.a )
    aaa.draw_fullscreen_quad()
end
```

The widget return value plugs DIRECTLY into a `set_uniform_*`
argument. No marshalling, no copy, no engine layer cell.
