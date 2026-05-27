--  bundle/macos/meu/Samples/composer/composer.lua
--
--  Sample MEU : "composer" -- multi-shader composition.
--
--  Spirit of vendor/aaaseed-runtime/AAAKernel/AAA_PROTO/MEU_PROTO/Out/
--  (Windows output-composition node) + the c138-A bloom run-then-readback
--  pattern. On Mac v1 the MEU runner exposes a single
--  `aaa.draw_fullscreen_quad()` per frame, so true ping-pong FBO
--  composition is v2 work ; here we demonstrate the alternating-frames
--  technique : on even frames issue the noise pass, on odd frames issue
--  the bloom pass, so the user can SEE both contributions to the
--  composition over time.
--
--  Demonstrates : conditional shader selection per frame ; how to plan
--  toward the eventual FBO ping-pong by using frame_index as a phase
--  selector. Press SPACE to toggle between "alternating" and "fixed"
--  modes.
--
--  This is NOT a true compositor -- a true compositor would render the
--  first pass to an offscreen texture, bind it via aaa.set_bind_texture,
--  then run a second pass. That requires the FBO + texture-target plumbing
--  c142-B deferred ; see project_v1_ship_gate.md.

local pass_a = "aaa_curl_noise_real"
local pass_b = "aaa_bloom_real"

local alternating = true
local last_space  = false

function aaa.on_frame( w, h, frame )
    local space = aaa.key_down( "space" )
    if space and not last_space then
        alternating = not alternating
        aaa.log( "composer : alternating=" .. tostring( alternating ) )
    end
    last_space = space

    local active
    if alternating then
        --  Even frames -> pass A ; odd frames -> pass B. The eye
        --  integrates the two at ~30 Hz alternation.
        if ( frame % 2 ) == 0 then active = pass_a else active = pass_b end
    else
        active = pass_a
    end

    aaa.use_shader( active )
    aaa.set_uniform_int( 0, 1 )

    local t = aaa.time()
    aaa.set_uniform_float( 0, t - math.floor( t ) )
    aaa.set_uniform_vec4( 0, 1.0, 1.0, 1.0, 1.0 )
    aaa.set_uniform_vec4( 1, 0.0, 0.0, 0.0, 0.0 )

    aaa.draw_hud_text( "composer | " .. ( alternating and "ALT" or "FIX" )
                       .. " | " .. active .. " | f=" .. frame )
    aaa.draw_fullscreen_quad()
end

aaa.log( "composer.lua : loaded, alternating "
         .. pass_a .. " <-> " .. pass_b )
