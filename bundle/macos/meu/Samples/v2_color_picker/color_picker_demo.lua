--  bundle/macos/meu/Samples/v2_color_picker/color_picker_demo.lua
--
--  Sample MEU : "v2_color_picker" -- v2 Phase 2 canonical demo.
--
--  Demonstrates the new `aaa.ui.hsv_color_picker` binding that c148-A
--  landed (Mac-native HSV picker rendered inside AAASeedMTKView -- no
--  NSColorWell, no Cocoa controls). v2 Phase 1 only cycled through a
--  preset palette via `aaa.ui.color_well`. Phase 2 is the real
--  drag-to-hue picker built on the Smith 1978 HSV<->RGB algorithm
--  (see the agent's inline citation in src/ui/widgets/aaa_widgets_mac.mm).
--
--  The picked RGBA value drives a Path A shader's primary uniform
--  END-TO-END every frame :
--
--    1. `aaa.ui.begin_panel` -- open a UI container as in Phase 1.
--    2. `aaa.ui.hsv_color_picker( label, r, g, b, a )` -- returns
--       r, g, b, a after the user drags the picker triangle / value
--       bar / alpha slider this frame. Same return-and-reassign idiom
--       as `slider` / `color_well`.
--    3. The picked RGBA pushes through `aaa.set_uniform_vec4` slot 0
--       to tint the noise shader.
--    4. `aaa.draw_hud_text` echoes the picked color in human-readable
--       form so a QA pass can verify the picker output by reading the
--       overlay text.
--
--  Spirit of vendor/aaaseed-runtime/AAAKernel/GaBu/'s ColorPicker
--  widget, reimagined as a Mac-native immediate-mode primitive that
--  returns the live picked color every frame. See
--  docs/AUTHORING_MEUS_ON_MAC.md section 3.6 "aaa.ui.hsv_color_picker"
--  for the binding reference.

--  Starting hue : warm orange. Persisted across frames -- the picker
--  returns r,g,b,a each frame, we reassign back to the same table so
--  the widget's drag-state is sticky.
local rgba = { r = 1.0, g = 0.5, b = 0.2, a = 1.0 }

function aaa.on_frame( w, h, frame )
    --  Panel is 360x280 -- larger than the Phase 1 demo because the
    --  HSV picker needs vertical room for the value bar + alpha slider
    --  under the hue triangle.
    aaa.ui.begin_panel( "Color Picker Demo", 16, 16, 360, 280 )

    --  The new Phase 2 binding. Signature : ( label, r, g, b, a ).
    --  Returns r, g, b, a -- unchanged on frames the user isn't
    --  interacting ; updated to the freshly picked color on drag
    --  frames. Drag inside the hue triangle to change the chroma ;
    --  drag the value bar to change brightness ; drag the alpha
    --  slider on the right to change opacity.
    rgba.r, rgba.g, rgba.b, rgba.a = aaa.ui.hsv_color_picker(
        "tint", rgba.r, rgba.g, rgba.b, rgba.a )

    aaa.ui.end_panel()

    --  HUD echo so a screenshot / Console.app log captures the picked
    --  color even without watching the panel.
    aaa.draw_hud_text( string.format(
        "HSV picker RGB=(%.2f,%.2f,%.2f,%.2f)",
        rgba.r, rgba.g, rgba.b, rgba.a ) )

    --  Drive the noise shader's tint uniform with the picked color.
    --  This is the END-TO-END loop : HSV picker -> Lua table -> vec4
    --  uniform -> Metal pipeline -> on-screen pixels.
    aaa.use_shader( "aaa_noise_real" )
    aaa.set_uniform_int( 0, 1 )                       -- real algo flag.
    aaa.set_uniform_vec4( 0, rgba.r, rgba.g, rgba.b, rgba.a )
    aaa.draw_fullscreen_quad()
end

aaa.log( "color_picker_demo.lua : v2 Phase 2 HSV picker demo loaded" )
