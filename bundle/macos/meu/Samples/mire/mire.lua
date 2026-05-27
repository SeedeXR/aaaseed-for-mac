--  bundle/macos/meu/Samples/mire/mire.lua
--
--  Sample MEU : "mire" -- test-pattern cycler.
--
--  Spirit of vendor/aaaseed-runtime/AAAKernel/AAA_PROTO/MEU_PROTO/Mire/
--  PROTO_Mire.lua (Windows). The Windows version exposes Offset / Factor /
--  Clamp / Bias / Gain widgets via GaBu BU adapters bound to a
--  `bdd_img_gradient` engine cell. That widget surface is v2 work on Mac
--  (project_v1_ship_gate.md "Authoring Surface" section), so on Mac v1 we
--  echo the same intent through the runner's Lua API : cycle three
--  test-pattern shaders that ship in the Path A catalog so an artist can
--  validate their display + colorspace before authoring real content.
--
--  Controls : press SPACE to advance to the next pattern.
--  HUD shows the active pattern name + frame index.

local patterns = {
    "ps_Maa_add_scale",       -- gradient (baseline)
    "ps_Maa_alpha",           -- alpha-blend ramp
    "aaa_noise_real",         -- procedural noise field (c135-A)
}

local idx = 1
local last_space = false

function aaa.on_frame( w, h, frame )
    local space = aaa.key_down( "space" )
    if space and not last_space then
        idx = ( idx % #patterns ) + 1
        aaa.log( "mire : pattern -> " .. patterns[ idx ] )
    end
    last_space = space

    aaa.use_shader( patterns[ idx ] )

    --  Reasonable defaults that all three patterns ignore-or-use safely :
    --   mode=1 selects the real (non-stub) variant on shaders that
    --   carry the c128-c140 mode-flag pattern.
    aaa.set_uniform_int( 0, 1 )
    aaa.set_uniform_float( 0, 0.0 )                       -- no time anim
    aaa.set_uniform_vec4( 0, 1.0, 1.0, 1.0, 1.0 )         -- gain
    aaa.set_uniform_vec4( 1, 0.0, 0.0, 0.0, 0.0 )         -- offset

    aaa.draw_hud_text( "mire | " .. patterns[ idx ] .. " | frame " .. frame )
    aaa.draw_fullscreen_quad()
end

aaa.log( "mire.lua : loaded, " .. #patterns .. " test patterns cued" )
