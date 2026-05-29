-- AAASeed starter project ; c152-I.
-- A minimal three-node chain : Source -> Effect -> Output, with the
-- middle node bound to a real shader so Play immediately renders
-- something.
return {
  version = 1,
  name    = "Starter",
  nodes   = {
    { id = 1, label = "Source",
      x = 80, y = 140,
      shader = "", script = "", enabled = true },
    { id = 2, label = "Bloom Effect",
      x = 320, y = 140,
      shader = "aaa_bloom_real", script = "", enabled = true },
    { id = 3, label = "Output",
      x = 560, y = 140,
      shader = "", script = "", enabled = true },
  },
  links   = {
    { id = 1, from = 1, to = 2 },
    { id = 2, from = 2, to = 3 },
  },
  editor_text = [==[
-- Starter project Lua. The runtime calls aaa.on_frame() each frame.
-- Below : cycle through a few shaders in time with the seconds clock.
-- Open the Engine Preview panel + press Start to see it live.

local catalog = {
  "aaa_bloom_real",
  "aaa_curl_noise_real",
  "aaa_fxaa_real",
  "aaa_pbr_real",
}

function aaa.on_frame()
  local i = ( math.floor( aaa.time() * 0.5 ) % #catalog ) + 1
  aaa.use_shader( catalog[ i ] )
  aaa.draw_fullscreen_quad()
end
]==],
  camera = { pos = {0,0,5}, look = {0,0,0}, up = {0,1,0},
             fov = 60, near = 0.01, far = 1000 },
  assets = { },
}
