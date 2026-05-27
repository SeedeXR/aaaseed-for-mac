--  bundle/macos/meu/hello_world.lua
--
--  First runnable Mac-native MEU. Cycles through a curated subset of
--  the Path A 172-shader catalog, switching to the next shader when the
--  user presses Space. Demonstrates that the .app actually DOES
--  something when launched, sidestepping the c119-A layer-subsystem
--  doctrine wall (per project_v1_ship_gate.md).
--
--  Hermetic : depends only on the `aaa.*` C bindings installed by
--  src/meu/aaa_meu_runner_mac.mm . No engine layers / Lua master /
--  module loader required.
--
--  Further reading (c145-D, v1 authoring surface ; c147 v2 Phase 1) :
--   - bundle/macos/meu/Samples/                  -- starter MEUs :
--       mire, animator, keyboard, mouse, composer (c145-D v1) +
--       v2_widgets (c147 v2 Phase 1, CANONICAL DEMO for the new
--       aaa.ui.* immediate-mode widget surface). Each demonstrates
--       one canonical pattern (test-pattern cycling, time-driven
--       anim, interactive selection, pointer-driven uniforms,
--       multi-shader composition, and widget-driven uniforms).
--       Inside the .app they ship at
--       Resources/meu/Samples/<name>/<name>.lua + README.md .
--   - docs/AUTHORING_MEUS_ON_MAC.md              -- full aaa.* API
--       reference (section 2), aaa.ui.* widget API (section 3,
--       v2 Phase 1), Path A catalog (section 4), how-to-author
--       (section 5), hot-reload status (section 6), what's not in
--       v1 (section 7), v2 plan (section 8). Ships at Resources/
--       in the .app.
--   - memory/project_v1_ship_gate.md             -- "Authoring Surface"
--       doctrine : what v1 ships, what defers to v2 ; v2 Phase 1
--       LANDED subsection summarizes the c147 widget surface.

--  Subset chosen for visual variety + revival proof.
local shaders = {
    "ps_Maa_add_scale",       -- c121 baseline (gradient)
    "ps_Maa_alpha",           -- c121 baseline (alpha-blend)
    "ifs_de_library",         -- c130-A : real fractal DE library
    "aaa_curl_noise_real",    -- c137-A : Bridson 2007 divergence-free
    "aaa_bloom_real",         -- c138-A : Lottes-style bloom
    "fxaa_lottes",            -- c128-A : real FXAA 3.11
    "aaa_material_pbr",       -- c134-B : Cook-Torrance PBR
    "aaa_motion_blur_real",   -- c139-A : camera-velocity projected
    "aaa_noise_real",         -- c135-A : Perlin + Simplex + FBM
    "aaa_gol_real",           -- c136-A : Conway + Brian's Brain
    "aaa_dof_hex_bokeh_real", -- c140-A : McIntosh hex bokeh
}

local idx = 1
local last_space = false

function aaa.on_frame( w, h, frame )
    --  Cycle to the next shader when Space is pressed (rising edge).
    local space = aaa.key_down( "space" )
    if space and not last_space then
        idx = ( idx % #shaders ) + 1
        aaa.log( "hello_world.lua : switched to " .. shaders[ idx ] )
    end
    last_space = space

    --  Select shader + populate the most common uniforms. Each shader
    --  ignores slots it doesn't bind ; no harm in setting them all.
    aaa.use_shader( shaders[ idx ] )

    --  Mode flag = 1 (real) for the c135 / c136 / c137 / c138 / c139 /
    --  c140 revivals that use the mode-flag stub variant.
    aaa.set_uniform_int( 0, 1 )

    --  Time-driven slot 0 for any shader that animates.
    local t = aaa.time()
    aaa.set_uniform_float( 0, t - math.floor( t ) )

    --  Default "scale = 1" via slot 0 of the vec4 array (many shaders
    --  use vec4[0].x as a gain). Slot 1 = identity offset.
    aaa.set_uniform_vec4( 0, 1.0, 1.0, 1.0, 1.0 )
    aaa.set_uniform_vec4( 1, 0.0, 0.0, 0.0, 0.0 )

    --  c146 : queue a HUD overlay line so the user can SEE which shader
    --  is currently active and how many frames have elapsed since launch.
    --  Closes the "interactive Space-press visual confirmation : UNVERIFIED"
    --  gap from c143-C.
    aaa.draw_hud_text( "shader: " .. shaders[ idx ] .. " | frame: " .. frame )

    aaa.draw_fullscreen_quad()
end

aaa.log( "hello_world.lua : loaded, " .. #shaders .. " shaders cued" )
