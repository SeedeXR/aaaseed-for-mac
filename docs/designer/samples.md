# Sample MEUs

> Walkthrough of all 14 sample MEUs shipped at
> `Contents/Resources/meu/Samples/<name>/<name>.lua` inside the .app
> bundle. Each entry covers what the sample does, which bindings it
> exercises, and a "try this" modification.
>
> Order : v1 starters (5) -> v2 widgets (4) -> v3 advanced (3) ->
> v4 IME + multiline (2).

To point the runner at a sample instead of the default `hello_world.lua`,
either :

- Copy the sample's `.lua` over `Contents/Resources/meu/hello_world.lua`,
  OR
- Modify `src/ui/macos/AAASeedMTKView.mm` to load the sample directly
  (developer path), OR
- Use the `v3_drag_drop` sample's "Browse for MEU script..." button to
  load any `.lua` from disk at runtime.

---

## v1 starters

### mire

**Path** : `Resources/meu/Samples/mire/mire.lua`

One-line description : test-pattern cycler -- press Space to advance
through three test patterns.

Bindings used : `aaa.use_shader`, `aaa.set_uniform_vec4`,
`aaa.set_uniform_int`, `aaa.draw_fullscreen_quad`, `aaa.draw_hud_text`,
`aaa.key_down`, `aaa.frame_index`.

Shaders : `ps_Maa_add_scale`, `ps_Maa_alpha`, `aaa_noise_real`.

Try this : add a fourth pattern by appending a shader stem (e.g.
`"aaa_gol_real"`) to the `patterns` table. The Space cycler picks it
up automatically.

---

### animator

**Path** : `Resources/meu/Samples/animator/animator.lua`

One-line description : time-driven curl-noise animation, no user
input -- the visual evolves on its own from `aaa.time()`.

Bindings used : `aaa.use_shader`, `aaa.set_uniform_float`,
`aaa.set_uniform_vec4`, `aaa.set_uniform_int`, `aaa.draw_fullscreen_quad`,
`aaa.draw_hud_text`, `aaa.time`.

Shader : `aaa_curl_noise_real`.

Try this : replace the `phase = t - math.floor(t)` line with
`phase = t * 0.25` for a continuous unbounded sweep, or
`phase = math.sin(t * 0.5)` for a slow oscillation.

---

### keyboard

**Path** : `Resources/meu/Samples/keyboard/keyboard.lua`

One-line description : seven-shader selector via digit keys `1`-`7` +
left / right arrows ; HUD shows the active slot index and shader name.

Bindings used : `aaa.use_shader`, `aaa.set_uniform_float`,
`aaa.set_uniform_int`, `aaa.draw_fullscreen_quad`, `aaa.draw_hud_text`,
`aaa.key_down`, `aaa.time`, `aaa.frame_index`.

Shaders : `ps_Maa_add_scale`, `aaa_noise_real`, `aaa_curl_noise_real`,
`aaa_gol_real`, `aaa_bloom_real`, `aaa_motion_blur_real`,
`aaa_dof_hex_bokeh_real`.

Try this : add an eighth shader at index `8` (keycode `25`) -- copy
the `keys = { ... }` and `shaders = { ... }` tables and add one more
slot to each. Carbon keycodes for digits : 1 = 18, 2 = 19, 3 = 20, ...

---

### mouse

**Path** : `Resources/meu/Samples/mouse/mouse.lua`

One-line description : cursor XY drives shader uniforms ; move the
mouse to sweep the parameter space.

Bindings used : `aaa.use_shader`, `aaa.set_uniform_float`,
`aaa.set_uniform_vec4`, `aaa.set_uniform_int`,
`aaa.draw_fullscreen_quad`, `aaa.draw_hud_text`, `aaa.mouse_xy`,
`aaa.time`.

Shader : `aaa_curl_noise_real`.

Try this : swap to `aaa_bloom_real` and bind mouse-x to the bloom
intensity slot (float slot 0) instead of the vec4 -- you get a
cursor-driven bloom-gain demo with no widget needed.

---

### composer

**Path** : `Resources/meu/Samples/composer/composer.lua`

One-line description : alternating-frames multi-shader composition --
the eye integrates curl-noise + bloom at refresh rate. Space toggles
between "alternating" and "fixed" (pass A only).

Bindings used : `aaa.use_shader`, `aaa.set_uniform_float`,
`aaa.set_uniform_int`, `aaa.set_uniform_vec4`,
`aaa.draw_fullscreen_quad`, `aaa.draw_hud_text`, `aaa.key_down`,
`aaa.time`, `aaa.frame_index`.

Shaders : `aaa_curl_noise_real`, `aaa_bloom_real`.

Try this : change `(frame % 2)` to `(frame % 4)` for a slower
alternation -- the eye stops integrating the two passes and you see
them as discrete frames. Or drive selection by
`aaa.time()` instead of the frame index for time-domain alternation.

---

## v2 widgets

### v2_widgets

**Path** : `Resources/meu/Samples/v2_widgets/widgets_demo.lua`

One-line description : the v2 Phase 1 canonical demo. Button + slider
+ color-well driving Path A shader uniforms end-to-end inside an
in-app panel. **Start here when learning the `aaa.ui.*` widget surface.**

Bindings used : `aaa.ui.begin_panel`, `aaa.ui.button`, `aaa.ui.slider`,
`aaa.ui.color_well`, `aaa.ui.end_panel`, plus the core uniform-push
surface.

Shaders : `aaa_noise_real`, `aaa_bloom_real`, `aaa_motion_blur_real`.

Try this : replace `aaa.ui.color_well` (preset cycling) with
`aaa.ui.hsv_color_picker` (drag-to-hue). You will need to expand the
panel height from 220 to 320 to fit the hue triangle.

---

### v2_color_picker

**Path** : `Resources/meu/Samples/v2_color_picker/color_picker_demo.lua`

One-line description : the v2 Phase 2 HSV color picker (drag-to-hue
triangle + value bar + alpha slider) driving the `aaa_noise_real`
tint uniform.

Bindings used : `aaa.ui.begin_panel`, `aaa.ui.hsv_color_picker`,
`aaa.ui.end_panel`, plus the core uniform-push surface.

Shader : `aaa_noise_real`.

Try this : add a second `hsv_color_picker` underneath the first
(give it a different label like `"tint2"`) and drive a second vec4
slot of the shader. Stack two color pickers with two tints.

---

### v2_modal_textinput

**Path** : `Resources/meu/Samples/v2_modal_textinput/modal_textinput_demo.lua`

One-line description : the v2 Phase 3 modal-popup + single-line
text-input combo. "Rename preset..." button -> centered modal with a
text field + OK / Cancel.

Bindings used : `aaa.ui.begin_panel`, `aaa.ui.button`,
`aaa.ui.show_modal`, `aaa.ui.begin_modal`, `aaa.ui.text_input`,
`aaa.ui.end_modal`, `aaa.ui.end_panel`.

Shader : `ps_Maa_add_scale`.

Try this : add a second `aaa.ui.text_input` to the modal (e.g. a
`description` field) -- you can have multiple text fields in one
modal, each keyed by its label.

---

### v2_collapse_reload

**Path** : `Resources/meu/Samples/v2_collapse_reload/collapse_reload_demo.lua`

One-line description : the v2 Phase 4 canonical demo. Two collapsing
sections (`Effect` + `Fog`) each containing a slider, plus a
`Reload MEU` hot-reload button driving `aaa_bloom_real`.

Bindings used : `aaa.ui.begin_panel`, `aaa.ui.begin_collapsing_panel`,
`aaa.ui.slider`, `aaa.ui.end_collapsing_panel`,
`aaa.ui.hot_reload_button`, `aaa.ui.end_panel`.

Shader : `aaa_bloom_real`.

Try this : add a third collapsing section (e.g. `Tint`) with an
`aaa.ui.hsv_color_picker` inside, driving the bloom tint vec4 slot.
You will need to expand the main panel height to accommodate.

---

## v3 advanced

### v3_drag_drop

**Path** : `Resources/meu/Samples/v3_drag_drop/drag_drop_demo.lua`

One-line description : v3 drag-drop + open-file-dialog surface.
Production drag-drop is wired in `AAASeedMTKView` ; the in-MEU
button is a manual fallback that demonstrates `aaa.io.open_file_dialog`.

Bindings used : `aaa.ui.begin_panel`, `aaa.ui.button`, `aaa.ui.end_panel`,
`aaa.io.open_file_dialog` (and the production drag-drop responder).

Shader : `aaa_noise_real`.

Try this : extend the `ext_list` from `{ "lua" }` to
`{ "lua", "plua" }` so the dialog also lists Windows-format preset
files (the runner will still treat them as Lua source). Or change
the title string to "Pick a texture..." and switch to
`{ "png", "jpg", "exr" }` to scope the dialog to image files.

---

### v3_preset_save_load

**Path** : `Resources/meu/Samples/v3_preset_save_load/preset_demo.lua`

One-line description : v3 preset save / load round-trip. Slider +
HSV color picker driving `aaa_bloom_real` ; `Save preset...` +
`Load preset...` buttons round-trip widget state through a single
Lua-table file.

Bindings used : `aaa.ui.begin_panel`, `aaa.ui.slider`,
`aaa.ui.hsv_color_picker`, `aaa.ui.button`, `aaa.ui.end_panel`,
`aaa.io.save_file_dialog`, `aaa.io.open_file_dialog`,
`aaa.io.save_preset`, `aaa.io.load_preset`.

Shader : `aaa_bloom_real`.

Try this : save a preset under one name, drag the slider to a new
value, save under a SECOND name, then load the first to confirm the
slider snaps back. The preset file is hand-editable -- open it in
your text editor and change a value, then reload to confirm the
change takes effect.

---

### v3_nested_panels

**Path** : `Resources/meu/Samples/v3_nested_panels/nested_panels_demo.lua`

One-line description : v3 deep-nested collapsing panels. A depth-1
`Effects` parent contains two depth-2 children (`Color` + `Bloom`),
plus a sibling depth-1 `Atmosphere` panel.

Bindings used : `aaa.ui.begin_panel`, `aaa.ui.begin_collapsing_panel`,
`aaa.ui.slider`, `aaa.ui.end_collapsing_panel`, `aaa.ui.end_panel`.

Shader : `aaa_bloom_real`.

Try this : add a third depth-2 child under `Effects` (e.g. a `Glow`
section) with its own slider. Remember to lay out the sub-panel
rects manually -- there is no auto-stacking, so increment `y` by
your section heights.

---

## v4 IME + multi-line

### v4_ime_text

**Path** : `Resources/meu/Samples/v4_ime_text/ime_text_demo.lua`

One-line description : v4 IME composition + commit canonical demo.
Single-line `aaa.ui.text_input` accepts CJK composition through the
production `NSTextInputClient` path ; a `Run synthetic IME demo`
button drives the `aaa.ime.set_marked_text` + `aaa.ime.commit_marked_text`
synthetic round-trip without needing a real IME keyboard.

Bindings used : `aaa.ui.begin_panel`, `aaa.ui.text_input`,
`aaa.ui.button`, `aaa.ui.end_panel`, `aaa.ime.set_marked_text`,
`aaa.ime.commit_marked_text`, `aaa.frame_index`.

Shader : `aaa_noise_real`.

Try this : with a Japanese Romaji IME active in System Settings,
focus the `your name` field and type `konnichiwa`. Watch the
underline appear under pending romaji ; press Enter to commit and
see the field populate with the candidate hiragana.

---

### v4_multiline

**Path** : `Resources/meu/Samples/v4_multiline/multiline_demo.lua`

One-line description : v4 multi-line `aaa.ui.text_area` canonical
demo. 8 visible lines, 48-character width, capped at 1024 chars,
pre-populated with welcome text so the multi-line render path is
visible at launch. `Save notes...` button opens NSSavePanel.

Bindings used : `aaa.ui.begin_panel`, `aaa.ui.text_area`,
`aaa.ui.button`, `aaa.ui.end_panel`, `aaa.io.save_file_dialog`,
`aaa.log`.

Shader : `aaa_curl_noise_real`.

Try this : with a CJK input source active, focus the text area and
type a paragraph in your language of choice. Newlines + scroll +
marked-text underline all work in the same widget.

---

## Coverage summary

Across the 14 samples :

- **All 12 `aaa.*` core bindings** exercised at least once.
- **All 14 `aaa.ui.*` widget bindings** exercised by at least one
  v2 / v3 / v4 sample.
- **All 5 `aaa.io.*` file-IO bindings** exercised by v3 samples.
- **Both `aaa.ime.*` synthetic bindings** exercised by the v4
  `ime_text_demo`.

If you can read all 14 samples + the per-namespace API references
([core](lua-api/core.md), [ui](lua-api/ui.md), [io](lua-api/io.md),
[ime](lua-api/ime.md)), you have the full Mac v1 authoring surface
in your head.
