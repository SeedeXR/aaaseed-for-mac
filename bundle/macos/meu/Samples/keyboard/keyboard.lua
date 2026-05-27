--  bundle/macos/meu/Samples/keyboard/keyboard.lua
--
--  Sample MEU : "keyboard" -- interactive shader selector.
--
--  Spirit of vendor/aaaseed-runtime/AAAKernel/AAA_PROTO/MEU_PROTO/GridSel/
--  (Windows grid-select picker). The Windows GridSel exposes an N-cell
--  picker widget bound to a parameter ; the artist clicks a cell to
--  select an index. On Mac v1 we echo the same selection-from-N idea via
--  arrow keys + number keys, since GaBu BU widgets are deferred to v2
--  (project_v1_ship_gate.md "Authoring Surface" section).
--
--  Controls :
--   - Number keys 1-7 jump directly to a shader slot.
--   - Left / Right arrow cycle through the catalog.
--   - HUD shows the active slot index + shader name.

local shaders = {
    "ps_Maa_add_scale",       -- 1 : gradient
    "aaa_noise_real",         -- 2 : Perlin/Simplex/FBM
    "aaa_curl_noise_real",    -- 3 : curl noise (c137-A)
    "aaa_gol_real",           -- 4 : Conway / Brian's Brain (c136-A)
    "aaa_bloom_real",         -- 5 : Lottes bloom (c138-A)
    "aaa_motion_blur_real",   -- 6 : camera-velocity blur (c139-A)
    "aaa_dof_hex_bokeh_real", -- 7 : McIntosh hex bokeh (c140-A)
}

--  Apple hardware key codes for digits 1..7 (Carbon-style).
local digit_codes = { 18, 19, 20, 21, 23, 22, 26 }

local idx = 1
local last_left  = false
local last_right = false
local last_digit = { false, false, false, false, false, false, false }

function aaa.on_frame( w, h, frame )
    --  Direct selection by digit keys (rising edge each).
    for i = 1, #digit_codes do
        local d = aaa.key_down( digit_codes[ i ] )
        if d and not last_digit[ i ] then
            idx = i
            aaa.log( "keyboard : direct -> " .. shaders[ idx ] )
        end
        last_digit[ i ] = d
    end

    --  Arrow-cycle (rising edge each).
    local L = aaa.key_down( "left"  )
    local R = aaa.key_down( "right" )
    if L and not last_left  then
        idx = ( ( idx - 2 ) % #shaders ) + 1
        aaa.log( "keyboard : prev -> " .. shaders[ idx ] )
    end
    if R and not last_right then
        idx = ( idx % #shaders ) + 1
        aaa.log( "keyboard : next -> " .. shaders[ idx ] )
    end
    last_left  = L
    last_right = R

    aaa.use_shader( shaders[ idx ] )
    aaa.set_uniform_int( 0, 1 )

    local t = aaa.time()
    aaa.set_uniform_float( 0, t - math.floor( t ) )
    aaa.set_uniform_vec4( 0, 1.0, 1.0, 1.0, 1.0 )
    aaa.set_uniform_vec4( 1, 0.0, 0.0, 0.0, 0.0 )

    aaa.draw_hud_text( "keyboard | [" .. idx .. "/" .. #shaders .. "] "
                       .. shaders[ idx ] )
    aaa.draw_fullscreen_quad()
end

aaa.log( "keyboard.lua : loaded, " .. #shaders .. " shaders (1-7 / arrows)" )
