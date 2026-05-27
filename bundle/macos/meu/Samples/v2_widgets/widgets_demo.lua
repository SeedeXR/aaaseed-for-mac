--  bundle/macos/meu/Samples/v2_widgets/widgets_demo.lua
--
--  Sample MEU : "v2_widgets" -- v2 Phase 1 canonical demo.
--
--  Demonstrates the new `aaa.ui.*` immediate-mode widget surface that
--  c147-A landed (Mac-native immediate-mode widgets rendered inside
--  AAASeedMTKView -- no GaBu, no NSPanel). The widgets here drive Path A
--  shader uniforms END-TO-END :
--
--    1. `aaa.ui.begin_panel` positions a UI container in screen space.
--    2. `aaa.ui.button` returns true on the frame it was clicked
--       (rising-edge ; auto-resets next frame).
--    3. `aaa.ui.slider` returns a float in [min, max] driven by the
--       cursor x-position when the user drags the widget.
--    4. `aaa.ui.color_well` returns rgba components ; cycles through a
--       preset palette on click (v2 Phase 1 stops short of a full HSV
--       picker -- see "What's NOT in v2 Phase 1" in the authoring guide).
--    5. `aaa.ui.end_panel` closes the panel ; the rendered widgets
--       composite OVER the next `aaa.draw_fullscreen_quad()` call.
--
--  The values returned by slider / button / color_well plug DIRECTLY
--  into `aaa.set_uniform_*` calls -- no marshalling, no copy. This is
--  the canonical authoring loop for v2 Phase 1.
--
--  Cycles : noise -> bloom -> motion-blur via the "Cycle shader"
--  button at the top of the panel. The slider and color-well affect
--  whichever shader is currently active. Each Path A shader carries
--  the c128-c140 mode-flag pattern : slot 0 int = 1 selects the real
--  algorithm.
--
--  Spirit of the Windows GaBu BU widget surface
--  (vendor/aaaseed-runtime/AAAKernel/GaBu/), echoed natively on Mac
--  without the engine layer + GaBu render path. See
--  docs/AUTHORING_MEUS_ON_MAC.md "aaa.ui.* widget API (v2 Phase 1)".

local shaders = {
    "aaa_noise_real",         -- c135-A : Perlin + Simplex + FBM.
    "aaa_bloom_real",         -- c138-A : Lottes-style bloom.
    "aaa_motion_blur_real",   -- c139-A : camera-velocity projected blur.
}
local shader_idx = 1

--  Slider-driven values persisted across frames. The widget returns
--  the NEW value each frame ; we assign it back to the same local so
--  the slider position is sticky.
local intensity = 0.5
local frequency = 1.0

--  Color-well state. The widget returns r, g, b, a -- we destructure
--  into a fresh table each frame so the panel sees the persisted hue.
local rgba = { r = 1.0, g = 0.5, b = 0.2, a = 1.0 }

function aaa.on_frame( w, h, frame )
    --  Place the panel top-left, 320x220 pixels. Coordinates are in
    --  window pixels ; the runner converts to NDC inside the widget
    --  layer (per c147-A spec).
    aaa.ui.begin_panel( "v2 Widgets Demo", 16, 16, 320, 220 )

    --  Button : true on the frame the user clicks. Rising-edge ; the
    --  widget layer handles debouncing.
    if aaa.ui.button( "Cycle shader" ) then
        shader_idx = ( shader_idx % #shaders ) + 1
        aaa.log( "widgets_demo : shader -> " .. shaders[ shader_idx ] )
    end

    --  Sliders : signature ( label, current_value, min, max ).
    --  Return value = new current_value after user input this frame.
    intensity = aaa.ui.slider( "intensity", intensity, 0.0, 2.0 )
    frequency = aaa.ui.slider( "frequency", frequency, 0.1, 8.0 )

    --  Color-well : signature ( label, r, g, b, a ). Returns r, g, b, a.
    --  In v2 Phase 1 this cycles through a preset palette ; a full HSV
    --  picker is v2 Phase 2 (see authoring guide).
    rgba.r, rgba.g, rgba.b, rgba.a = aaa.ui.color_well(
        "tint", rgba.r, rgba.g, rgba.b, rgba.a )

    aaa.ui.end_panel()

    --  HUD line so the user sees current state in text even without
    --  watching the panel (e.g. when screenshotting for QA).
    aaa.draw_hud_text( string.format(
        "v2 demo | %s | intensity=%.2f | freq=%.2f",
        shaders[ shader_idx ], intensity, frequency ) )

    --  Drive Path A shader uniforms with the widget-controlled values.
    --  This is the END-TO-END loop : widget -> Lua local -> uniform ->
    --  Metal pipeline -> on-screen pixels.
    aaa.use_shader( shaders[ shader_idx ] )
    aaa.set_uniform_int( 0, 1 )                     -- real-algorithm mode.
    aaa.set_uniform_float( 0, intensity )           -- slot 0 = gain.
    aaa.set_uniform_float( 1, frequency )           -- slot 1 = frequency.
    aaa.set_uniform_vec4( 0, rgba.r, rgba.g, rgba.b, rgba.a )

    aaa.draw_fullscreen_quad()
end

aaa.log( "widgets_demo.lua : v2 Phase 1 demo loaded with "
         .. #shaders .. " shaders" )
