# Lua API : `aaa.*` (core)

> Reference for the 12 core bindings under the `aaa.*` namespace.
> Every entry has signature + arguments + side effects + a short code
> example. Source : `src/meu/aaa_meu_runner_mac.{h,mm}` `install_aaa_bindings()`.

The `aaa` table is created in the global Lua environment before your
script runs. Bindings are installed at chunk-load time -- they are
available from the very top of your file.

---

## `aaa.use_shader( name ) -> nil`

Select the Path A shader to draw with on the next
`aaa.draw_fullscreen_quad()` call.

- `name` (string) -- the bare stem of a `.metal` file in
  `Contents/Resources/shaders/` (e.g. `"aaa_curl_noise_real"`,
  `"fxaa_lottes"`, `"ps_Maa_edge_sobel"`). No path, no extension.
- Returns nothing.
- Bad names are logged via `NSLog` and the previously bound shader
  remains active.

```lua
aaa.use_shader( "aaa_bloom_real" )
```

See also : [Shader catalog](../shader-catalog.md),
[`aaa.draw_fullscreen_quad`](#aaadraw_fullscreen_quad-nil).

---

## `aaa.set_uniform_float( slot, value ) -> nil`

Push a scalar float uniform into the active shader's float slot.

- `slot` (integer, 0-based) -- the float slot index. Shaders bind
  these through the canonical `set_uniform_float` table laid out in
  the Path A MSL convention.
- `value` (number) -- any double-precision float (clamped to fp32 on
  push).

```lua
aaa.set_uniform_float( 0, aaa.time() )      -- slot 0 = time
aaa.set_uniform_float( 1, intensity )       -- slot 1 = intensity
```

See also : [`set_uniform_vec4`](#aaaset_uniform_vec4-slot-x-y-z-w-nil),
[`set_uniform_int`](#aaaset_uniform_int-slot-value-nil).

---

## `aaa.set_uniform_vec4( slot, x, y, z, w ) -> nil`

Push a 4-component float uniform.

- `slot` (integer, 0-based) -- the vec4 slot index.
- `x, y, z, w` (numbers) -- the four components. No clamping ; pass
  exactly what your shader expects.

```lua
-- RGBA tint into slot 0.
aaa.set_uniform_vec4( 0, 1.0, 0.5, 0.2, 1.0 )

-- Mouse-driven XY in slots .xy ; pulse in .z ; alpha in .w.
local mx, my = aaa.mouse_xy()
aaa.set_uniform_vec4( 1, mx / w, my / h, math.sin( aaa.time() ), 1.0 )
```

See also : [`mouse_xy`](#aaamouse_xy-x-y).

---

## `aaa.set_uniform_int( slot, value ) -> nil`

Push an integer uniform. Convention : Path A revival shaders reserve
slot 0 as the **mode flag** -- pass `1` to select the real algorithm,
`0` to select the legacy stub. Author MEUs always pass `1`.

- `slot` (integer, 0-based) -- the int slot index.
- `value` (integer) -- the value.

```lua
aaa.set_uniform_int( 0, 1 )    -- real algorithm
```

See also : [Shader catalog : real-algorithm revivals](../shader-catalog.md#the-11-real-algorithm-revivals).

---

## `aaa.set_bind_texture( slot, texture_name ) -> nil`

Mark a fragment-shader sampler slot as active and reserve a
runner-owned texture for it.

- `slot` (integer, 0..7) -- the sampler slot index.
- `texture_name` (string) -- an asset-registry key the runner will
  resolve to a texture in a future session ; today the runner binds a
  placeholder texture so the shader gets a valid sampler. Pass any
  identifier string -- the value is reserved-not-required in v1.

```lua
aaa.set_bind_texture( 0, "main_input" )
aaa.set_bind_texture( 1, "noise_lookup" )
```

The v1 sample surface does not allocate textures from Lua ; this
binding is exposed for forward compatibility with the v2 FBO
authoring surface.

See also : [What is NOT supported in v1](../meu-authoring.md#what-is-not-supported-in-v1).

---

## `aaa.draw_fullscreen_quad() -> nil`

Encode a 3-vertex fullscreen triangle through the active Metal encoder
using the currently bound shader + uniforms.

- Call exactly **once per frame**. The runner does not begin / end its
  own render pass ; it draws into the host MTKView pass.

```lua
aaa.use_shader( "aaa_noise_real" )
aaa.set_uniform_int( 0, 1 )
aaa.set_uniform_float( 0, aaa.time() )
aaa.draw_fullscreen_quad()
```

See also : [`use_shader`](#aaause_shader-name-nil).

---

## `aaa.draw_hud_text( text ) -> nil`

Queue a single line of HUD overlay text for this frame.

- `text` (string) -- the text to display.
- One queued string per frame ; calling twice replaces the previous.
- Empty string = no HUD this frame.
- The host `AAASeedMTKView` reads this after your `on_frame` returns
  and emits the text through the bundled c61 glyph atlas + text-MSL
  pipeline.

```lua
aaa.draw_hud_text( "frame " .. frame )
aaa.draw_hud_text(
    string.format( "i=%.2f shader=%s", intensity, current_shader ) )
```

See also : [`frame_index`](#aaaframe_index-int).

---

## `aaa.log( text ) -> nil`

Write a line to the macOS unified log via `NSLog`.

- `text` (string) -- the text to log.
- Visible in **Console.app** filtered by the `aaaseed_app` process.
  Each line is prefixed `aaa.log: ...`.
- Use for debugging from the Lua side. Cheap ; safe to call every
  frame, but per-frame logs flood the console quickly.

```lua
aaa.log( "shader changed to " .. shaders[ idx ] )
aaa.log( string.format( "mx=%d my=%d frame=%d", mx, my, frame ) )
```

See also : [`draw_hud_text`](#aaadraw_hud_text-text-nil) for in-app
overlay (preferred over `log` for live tuning).

---

## `aaa.frame_index() -> int`

Return the integer frame index of the current frame. Resets to 0 on
`Runner::reload()`.

```lua
local f = aaa.frame_index()
if f == 0 then aaa.log( "MEU reloaded, frame 0" ) end
```

In practice the same value is passed as the third argument to
`aaa.on_frame(w, h, frame)`, so you rarely need this function. It
exists for utility scripts that need the frame index outside of
`on_frame` scope.

See also : [`time`](#aaatime-seconds).

---

## `aaa.time() -> seconds`

Return seconds (Lua number, double-precision) since the runner was
constructed at process launch. Wall-clock, monotonic.

- Does NOT reset on `Runner::reload()` -- only on app relaunch.

```lua
aaa.set_uniform_float( 0, aaa.time() )

-- Oscillating value in [0, 1].
local t = math.sin( aaa.time() ) * 0.5 + 0.5
aaa.set_uniform_float( 1, t )
```

See also : [`frame_index`](#aaaframe_index-int).

---

## `aaa.key_down( key ) -> bool`

Query whether a key is held this frame.

- `key` can be a **string** (named key) or an **integer** Carbon
  keycode.
- Named keys : `"space"` (49), `"return"` (36), `"escape"` (53),
  `"left"` (123), `"right"` (124), `"down"` (125), `"up"` (126).
- For any other key, pass the integer keycode directly (e.g. digit
  `1` is keycode `18`, digit `2` is `19`, `q` is `12`, `w` is `13`).
- Returns `true` (held this frame) or `false` (not held).
- `key_down` is **level-triggered**, not edge-triggered. If you want
  "once per press" semantics, debounce on the frame index :

```lua
local last_press = -10

function aaa.on_frame( w, h, frame )
    if aaa.key_down( "space" ) and frame - last_press > 5 then
        last_press = frame
        -- ... one-time action here ...
    end
end
```

See also : [Sample MEU : keyboard](../samples.md#keyboard).

---

## `aaa.mouse_xy() -> x, y`

Return the cursor position as two values (x, y) in **window pixels**.

- Origin is top-left ; y grows downward.
- The runner does not normalize ; divide by the `w` / `h` arguments
  of `aaa.on_frame()` to get `[0, 1]^2`.
- Returns the last known cursor position when the cursor is outside
  the window.

```lua
function aaa.on_frame( w, h, frame )
    local mx, my = aaa.mouse_xy()
    local u = mx / w
    local v = my / h
    aaa.use_shader( "aaa_curl_noise_real" )
    aaa.set_uniform_int( 0, 1 )
    aaa.set_uniform_vec4( 0, u, v, 0.0, 1.0 )
    aaa.draw_fullscreen_quad()
end
```

See also : [Sample MEU : mouse](../samples.md#mouse).

---

## Conventions recap

- **Slot 0 of `set_uniform_int`** is the mode flag for Path A revival
  shaders : `1` = real algorithm, `0` = legacy stub. Always pass `1`.
- **`draw_fullscreen_quad`** is called exactly once per `on_frame`.
  More than once = undefined behaviour ; zero times = no scene this
  frame.
- **HUD text** is queued, not drawn immediately. The host reads after
  `on_frame` returns.
- **`time`** is process-relative monotonic seconds ; does not reset
  on reload.
- **`frame_index`** is reload-relative ; resets to 0 on
  `Runner::reload()`.
