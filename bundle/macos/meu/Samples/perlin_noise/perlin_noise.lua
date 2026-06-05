--  bundle/macos/meu/Samples/perlin_noise/perlin_noise.lua  (c155)
--
--  Perlin noise visualization -- the canonical "make my library module
--  actually render" sample.
--
--  WHY THIS SAMPLE EXISTS : a pure-Lua Perlin module (one that computes
--  noise and ends with `return perlin`) loads fine in the runner but
--  renders NOTHING, because a MEU must define `aaa.on_frame(w, h, frame)`
--  and emit at least one draw (`aaa.draw_fullscreen_quad()`). This sample
--  embeds exactly such a module (Ken Perlin's Improved Noise, Lua 5.1+,
--  no bitwise ops) and wires it to the screen two ways at once :
--
--    1. GPU : `aaa_noise_real` (c135-A Path A revival -- real Perlin /
--       Simplex / FBM in MSL) renders the per-pixel field at full
--       resolution. CPU Lua could never fill 60 fps of pixels ; the GPU
--       shader is the per-pixel half of the visualization.
--    2. CPU : the embedded `perlin` module runs EVERY frame and drives
--       the shader's uniforms (animated offset + gain), plus a live
--       noise sample printed on the HUD -- visible proof the Lua module
--       is executing and shaping the output.
--
--  `perlin` is intentionally a GLOBAL (not `local`) so the unit test
--  (tests/native/perlin_meu_script_test.cpp) can poke it directly.
--  Deterministic : seeded with a constant, never os.time().

perlin = {}

--  Ken Perlin's reference permutation (0..255).
local permutation = {
  151,160,137,91,90,15,131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,
  8,99,37,240,21,10,23,190,6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,
  35,11,32,57,177,33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,
  134,139,48,27,166,77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,
  55,46,245,40,244,102,143,54,65,25,63,161,1,216,80,73,209,76,132,187,208,89,18,
  169,200,196,135,130,116,188,159,86,164,100,109,198,173,186,3,64,52,217,226,250,
  124,123,5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,
  28,42,223,183,170,213,119,248,152,2,44,154,163,70,221,153,101,155,167,43,172,9,
  129,22,39,253,19,98,108,110,79,113,224,232,178,185,112,104,218,246,97,228,251,
  34,242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,49,192,
  214,31,181,199,106,157,184,84,204,176,115,121,50,45,127,4,150,254,138,236,205,
  93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
}

--  p is a 512-entry (0..511) table so indices never need hand-wrapping.
local p = {}
local function loadPerm(src)
  for i = 0, 255 do
    p[i] = src[i + 1]
    p[i + 256] = src[i + 1]
  end
end
loadPerm(permutation)

--  Reseed with a shuffled permutation (Fisher-Yates over math.random).
--  Pass a constant for reproducible noise ; this sample seeds 1337 below.
function perlin.seed(s)
  math.randomseed(s or 0)
  local perm = {}
  for i = 0, 255 do perm[i] = i end
  for i = 255, 1, -1 do
    local j = math.random(0, i)
    perm[i], perm[j] = perm[j], perm[i]
  end
  for i = 0, 255 do
    p[i] = perm[i]
    p[i + 256] = perm[i]
  end
end

local function fade(t)
  return t * t * t * (t * (t * 6 - 15) + 10)
end

local function lerp(t, a, b)
  return a + t * (b - a)
end

local function grad(hash, x, y, z)
  local h = hash % 16
  local u = (h < 8) and x or y
  local v
  if h < 4 then v = y
  elseif h == 12 or h == 14 then v = x
  else v = z end
  local r = ((h % 2) == 0) and u or -u
  r = r + (((h % 4) < 2) and v or -v)
  return r
end

--  3D Perlin noise, ~[-1, 1]. y / z optional (default 0).
function perlin.noise(x, y, z)
  y = y or 0
  z = z or 0

  local X = math.floor(x) % 256
  local Y = math.floor(y) % 256
  local Z = math.floor(z) % 256

  x = x - math.floor(x)
  y = y - math.floor(y)
  z = z - math.floor(z)

  local u = fade(x)
  local v = fade(y)
  local w = fade(z)

  local A  = p[X] + Y
  local AA = p[A] + Z
  local AB = p[A + 1] + Z
  local B  = p[X + 1] + Y
  local BA = p[B] + Z
  local BB = p[B + 1] + Z

  return lerp(w,
    lerp(v,
      lerp(u, grad(p[AA],     x,     y,     z),
              grad(p[BA],     x - 1, y,     z)),
      lerp(u, grad(p[AB],     x,     y - 1, z),
              grad(p[BB],     x - 1, y - 1, z))),
    lerp(v,
      lerp(u, grad(p[AA + 1], x,     y,     z - 1),
              grad(p[BA + 1], x - 1, y,     z - 1)),
      lerp(u, grad(p[AB + 1], x,     y - 1, z - 1),
              grad(p[BB + 1], x - 1, y - 1, z - 1))))
end

--  Fractal / fBm : layered octaves, normalized back to ~[-1, 1].
function perlin.octave(x, y, z, octaves, persistence)
  octaves = octaves or 4
  persistence = persistence or 0.5
  local total, frequency, amplitude, maxValue = 0, 1, 1, 0
  for _ = 1, octaves do
    total = total + perlin.noise(x * frequency, (y or 0) * frequency, (z or 0) * frequency) * amplitude
    maxValue = maxValue + amplitude
    amplitude = amplitude * persistence
    frequency = frequency * 2
  end
  return total / maxValue
end

--  ------------------------------------------------------------------
--  The MEU half : without this block the module above loads and then
--  NOTHING renders. This is the minimal runnable pattern.
--  ------------------------------------------------------------------

perlin.seed(1337)   --  deterministic across runs / platforms

function aaa.on_frame(w, h, frame)
  local t = aaa.time()

  --  CPU Perlin drives the animation : two slow fBm walks modulate the
  --  shader's offset + gain so the user's Lua code visibly shapes the
  --  image every frame.
  local drift_x = perlin.octave(t * 0.10, 0.0, 0.0, 4, 0.5)
  local drift_y = perlin.octave(0.0, t * 0.10, 0.0, 4, 0.5)
  local gain    = 0.75 + 0.25 * perlin.noise(t * 0.25, 3.7, 1.3)

  --  GPU Perlin renders the per-pixel field (c135-A revival).
  aaa.use_shader("aaa_noise_real")
  aaa.set_uniform_int(0, 1)                       --  mode flag = real
  aaa.set_uniform_float(0, t - math.floor(t))     --  anim phase
  aaa.set_uniform_vec4(0, gain, gain, gain, 1.0)  --  gain
  aaa.set_uniform_vec4(1, drift_x, drift_y, 0.0, 0.0)  --  fBm-driven offset

  aaa.draw_hud_text(string.format(
    "perlin_noise.lua | cpu fBm=%.3f gain=%.2f | frame %d",
    drift_x, gain, frame))

  aaa.draw_fullscreen_quad()
end

aaa.log("perlin_noise.lua : loaded (cpu module + gpu aaa_noise_real)")
