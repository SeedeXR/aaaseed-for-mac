--  bundle/macos/meu/Samples/mouse/mouse.lua
--
--  Sample MEU : "mouse" -- mouse-driven shader uniforms.
--
--  Spirit of vendor/aaaseed-runtime/AAAKernel/AAA_PROTO/MEU_PROTO/KinMove/
--  (Windows kinetic-mover driven by 2D pointer). On Mac v1 we echo the
--  same pointer-driven intent : the mouse XY position normalized to
--  [0,1]^2 drives two shader-uniform slots, so a hover sweeps through
--  the parameter space without any clicks.
--
--  Demonstrates : `aaa.mouse_xy()` returning (x, y) in window pixels ;
--  normalization + clamping ; binding to vec4 uniform slots.
--
--  Note : the runner does NOT receive a width / height on the input
--  path, so we use the `w` and `h` arguments of `aaa.on_frame()` as the
--  normalization basis. If the window is resized, the next frame's
--  arguments reflect the new size.

local shader_name = "aaa_curl_noise_real"

local function clamp01( v )
    if v < 0.0 then return 0.0 end
    if v > 1.0 then return 1.0 end
    return v
end

function aaa.on_frame( w, h, frame )
    local mx, my = aaa.mouse_xy()
    local nx = clamp01( mx / math.max( 1, w ) )
    local ny = clamp01( my / math.max( 1, h ) )

    aaa.use_shader( shader_name )
    aaa.set_uniform_int( 0, 1 )

    --  Float slot 0 : time (kept so the field still evolves under a
    --  stationary cursor).
    local t = aaa.time()
    aaa.set_uniform_float( 0, t - math.floor( t ) )

    --  Vec4 slot 0 : nx / ny in xy, time-phase in z, 1.0 in w.
    aaa.set_uniform_vec4( 0, nx, ny, t - math.floor( t ), 1.0 )

    --  Vec4 slot 1 : zero offset (kept for shader compatibility).
    aaa.set_uniform_vec4( 1, 0.0, 0.0, 0.0, 0.0 )

    aaa.draw_hud_text( "mouse | x=" .. string.format( "%.2f", nx )
                       .. " y=" .. string.format( "%.2f", ny )
                       .. " | " .. shader_name )
    aaa.draw_fullscreen_quad()
end

aaa.log( "mouse.lua : loaded, cursor drives uniforms on " .. shader_name )
