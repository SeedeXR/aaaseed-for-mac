--  bundle/macos/meu/Samples/animator/animator.lua
--
--  Sample MEU : "animator" -- time-driven animation.
--
--  Spirit of vendor/aaaseed-runtime/AAAKernel/AAA_PROTO/MEU_PROTO/KinMoveAuto/
--  (Windows automatic-motion kinetic mover). The Windows version drives
--  a transform over engine time ; on Mac v1 we echo the same idea by
--  feeding `aaa.time()` into a Path A shader's animation uniform.
--
--  Demonstrates : how to bind wall-clock time (seconds since runner
--  construction) into a shader uniform slot to produce frame-to-frame
--  visual motion without any user input.

--  Pick a shader with an obvious time-driven look. The c137-A curl-noise
--  field animates with slot-0-float ; the c136-A Game-of-Life sim
--  evolves on each frame independent of the uniform but pairs well.
local shader_name = "aaa_curl_noise_real"

function aaa.on_frame( w, h, frame )
    aaa.use_shader( shader_name )

    --  mode = 1 -> real algorithm (Bridson 2007), not the stub variant.
    aaa.set_uniform_int( 0, 1 )

    --  Time-driven uniform : seconds since launch, wrapped to [0,1) so
    --  numerically-sensitive shaders stay bounded.
    local t = aaa.time()
    local phase = t - math.floor( t )
    aaa.set_uniform_float( 0, phase )

    --  Gain (vec4 slot 0) modulates with a slow sin so the field
    --  pulses ; offset stays zero.
    local pulse = 0.5 + 0.5 * math.sin( t * 0.5 )
    aaa.set_uniform_vec4( 0, pulse, pulse, pulse, 1.0 )
    aaa.set_uniform_vec4( 1, 0.0, 0.0, 0.0, 0.0 )

    aaa.draw_hud_text( "animator | t=" .. string.format( "%.2f", t )
                       .. " | shader " .. shader_name )
    aaa.draw_fullscreen_quad()
end

aaa.log( "animator.lua : loaded, time-driven " .. shader_name )
