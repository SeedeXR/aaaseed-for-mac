--  bundle/macos/meu/Samples/particle_portal/particle_portal.lua  (c157)
--
--  40,960 small white particles forming a circular portal / energy-field
--  flow on a black background -- with live WEIGHT / PUSH / PULL /
--  ACCELERATION sliders and a mouse-driven POINT OF ORIGIN.
--
--  Architecture (the CPU/GPU split every fast MEU uses) :
--    GPU : `aaa_particle_portal` (c157 catalog shader) evaluates all
--          40,960 particles procedurally per frame -- each particle's
--          position is a closed-form function of (id, t), found per
--          pixel through a polar cell hash, so vanilla Lua never has
--          to step 40k bodies.
--    CPU : THIS script owns the forces. Sliders set weight/push/pull/
--          accel, the mouse moves the origin, and an embedded Perlin
--          module pulses the push force so the field breathes
--          organically instead of metronomically.
--
--  Uniform contract (see the shader header for the full ABI) :
--    int[0]   = 1 (real mode)
--    float[0] = time
--    vec4[0]  = ( pull, push, accel, weight )
--    vec4[1]  = ( origin_x, origin_y, portal_radius, particle_size )
--    vec4[2]  = ( brightness, aspect, spin_dir, 0 )

--  ------------------------------------------------------------------
--  Compact Perlin (Ken Perlin's Improved Noise, Lua 5.1-safe) -- used
--  at CONTROL RATE only : one call per frame to pulse the push force.
--  ------------------------------------------------------------------
perlin = {}
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
local p = {}
for i = 0, 255 do p[i] = permutation[i + 1] ; p[i + 256] = permutation[i + 1] end
local function fade(t) return t * t * t * (t * (t * 6 - 15) + 10) end
local function lerp(t, a, b) return a + t * (b - a) end
local function grad(hash, x, y, z)
  local h = hash % 16
  local u = (h < 8) and x or y
  local v
  if h < 4 then v = y elseif h == 12 or h == 14 then v = x else v = z end
  local r = ((h % 2) == 0) and u or -u
  return r + (((h % 4) < 2) and v or -v)
end
function perlin.noise(x, y, z)
  y = y or 0 ; z = z or 0
  local X, Y, Z = math.floor(x) % 256, math.floor(y) % 256, math.floor(z) % 256
  x, y, z = x - math.floor(x), y - math.floor(y), z - math.floor(z)
  local u, v, w = fade(x), fade(y), fade(z)
  local A  = p[X] + Y ;  local AA = p[A] + Z ;  local AB = p[A + 1] + Z
  local B  = p[X + 1] + Y ; local BA = p[B] + Z ; local BB = p[B + 1] + Z
  return lerp(w,
    lerp(v, lerp(u, grad(p[AA], x, y, z),     grad(p[BA], x - 1, y, z)),
            lerp(u, grad(p[AB], x, y - 1, z), grad(p[BB], x - 1, y - 1, z))),
    lerp(v, lerp(u, grad(p[AA + 1], x, y, z - 1),     grad(p[BA + 1], x - 1, y, z - 1)),
            lerp(u, grad(p[AB + 1], x, y - 1, z - 1), grad(p[BB + 1], x - 1, y - 1, z - 1))))
end

--  ------------------------------------------------------------------
--  Live state (persisted across frames ; reset on reload).
--  ------------------------------------------------------------------
local weight = 0.50    -- particle mass : size/brightness up, spin down
local push   = 0.35    -- radial breathing amplitude
local pull   = 0.25    -- gather toward the portal ring
local accel  = 0.80    -- spin-up multiplier (0..3)
local radius = 0.28    -- portal ring radius (uv units)
local size   = 0.0030  -- particle size (uv units)

function aaa.on_frame(w, h, frame)
  local t = aaa.time()

  --  Force panel : the four requested controls + portal shape.
  aaa.ui.begin_panel("Portal forces", 16, 16, 330, 230)
  weight = aaa.ui.slider("weight",       weight, 0.0, 1.0)
  push   = aaa.ui.slider("push",         push,   0.0, 1.0)
  pull   = aaa.ui.slider("pull",         pull,   0.0, 0.9)
  accel  = aaa.ui.slider("acceleration", accel,  0.0, 3.0)
  radius = aaa.ui.slider("radius",       radius, 0.10, 0.45)
  size   = aaa.ui.slider("size",         size,   0.001, 0.008)
  aaa.ui.end_panel()

  --  Point of origin follows the mouse ; window centre until the
  --  pointer has entered the window.
  local mx, my = aaa.mouse_xy()
  local ox, oy = 0.5, 0.5
  if mx > 0 and my > 0 and w > 0 and h > 0 then
    ox, oy = mx / w, my / h
  end

  --  CPU Perlin pulses the push force (organic energy breathing) and
  --  slowly wanders the spin direction bias.
  local pulse    = 0.65 + 0.35 * perlin.noise(t * 0.35, 1.7, 9.2)
  local push_eff = push * pulse
  local dir      = (perlin.noise(t * 0.05, 4.4, 2.2) >= 0) and 1.0 or -1.0

  aaa.use_shader("aaa_particle_portal")
  aaa.set_uniform_int(0, 1)
  aaa.set_uniform_float(0, t)
  aaa.set_uniform_vec4(0, pull, push_eff, accel, weight)
  aaa.set_uniform_vec4(1, ox, oy, radius, size)
  aaa.set_uniform_vec4(2, 1.25, w / h, dir, 0.0)

  aaa.draw_hud_text(string.format(
    "portal | 40960 particles | w=%.2f push=%.2f pull=%.2f acc=%.2f | pulse=%.2f",
    weight, push_eff, pull, accel, pulse))

  aaa.draw_fullscreen_quad()
end

aaa.log("particle_portal.lua : loaded (40,960 GPU particles, CPU perlin forces)")
