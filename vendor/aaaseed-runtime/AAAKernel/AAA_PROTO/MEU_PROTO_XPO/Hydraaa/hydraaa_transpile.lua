--	HYDRAAA_TRANSPILE
--	Hydra (https://hydra.ojack.xyz) source string -> single GLSL fragment shader
--	Inspired by HeadlessHydra (https://codeberg.org/gugray/HeadlessHydra)
--
--	Public API:
--	  HYDRAAA_TRANSPILE.transpile( source_string ) -> { ok=bool, glsl=string, err=string }
--
--	Coordinate conventions (HeadlessHydra style):
--	  st00 = parent chain coord, c00 = parent chain color
--	  st01..stNN = nested sub-chain coords (modulators, blends)
--	  c01..cNN  = nested sub-chain colors
--
--	The shader output is wired to AAASeed conventions:
--	  in_texcoord (vec2) -> initial st
--	  out_result  (vec4) -> final color
--	  aaa_fu_float[0]    -> time
--	  aaa_fu_float[1..2] -> mouse.x, mouse.y
--	  aaa_fu_float[3..4] -> resolution.x, resolution.y
--	  g_input_texture_0  -> s0 (and o0 fallback)

--	AAASeed runs strict.lua, so we must declare any global before assigning.
if aaa and aaa.lua and aaa.lua.global then
	aaa.lua.global.declare_table("HYDRAAA_TRANSPILE")
end
HYDRAAA_TRANSPILE = HYDRAAA_TRANSPILE or {}

----------------------------------------------------------------
--	Operator inventory
----------------------------------------------------------------

local OP_KIND_SRC		= "src"				--	leaf, no input chain, returns vec4
local OP_KIND_COLOR		= "color"			--	consumes c, returns vec4
local OP_KIND_COORD		= "coord"			--	consumes st, returns vec2
local OP_KIND_COMBINE	= "combine"			--	first arg is sub-chain, returns vec4
local OP_KIND_MOD		= "combineCoord"	--	first arg is sub-chain, returns vec2

local function arg(name, default)
	return { name = name, default = default }
end

--	OPS[name] = { kind, args = {arg, ...} }
local OPS = {
--	source generators
	noise			= { kind=OP_KIND_SRC,	args={ arg("scale",10),		arg("offset",0.1) } },
	voronoi			= { kind=OP_KIND_SRC,	args={ arg("scale",5),		arg("speed",0.3),	arg("blending",0.3) } },
	osc				= { kind=OP_KIND_SRC,	args={ arg("frequency",60),	arg("sync",0.1),	arg("offset",0.0) } },
	shape			= { kind=OP_KIND_SRC,	args={ arg("sides",3),		arg("radius",0.3),	arg("smoothing",0.01) } },
	gradient		= { kind=OP_KIND_SRC,	args={ arg("speed",0) } },
	solid			= { kind=OP_KIND_SRC,	args={ arg("r",0),	arg("g",0),	arg("b",0),	arg("a",1) } },
	src				= { kind=OP_KIND_SRC,	args={ arg("tex","s0") } },	--	tex is an identifier, special-cased
	prev			= { kind=OP_KIND_SRC,	args={} },

--	color
	brightness		= { kind=OP_KIND_COLOR,		args={ arg("amount",0.4) } },
	contrast		= { kind=OP_KIND_COLOR,		args={ arg("amount",1.6) } },
	saturate		= { kind=OP_KIND_COLOR,		args={ arg("amount",2) } },
	hue				= { kind=OP_KIND_COLOR,		args={ arg("hue",0.4) } },
	colorama		= { kind=OP_KIND_COLOR,		args={ arg("amount",0.005) } },
	posterize		= { kind=OP_KIND_COLOR,		args={ arg("bins",3),		arg("gamma",0.6) } },
	shift			= { kind=OP_KIND_COLOR,		args={ arg("r",0.5),	arg("g",0),	arg("b",0),	arg("a",0) } },
	r				= { kind=OP_KIND_COLOR,		args={ arg("scale",1),	arg("offset",0) } },
	g				= { kind=OP_KIND_COLOR,		args={ arg("scale",1),	arg("offset",0) } },
	b				= { kind=OP_KIND_COLOR,		args={ arg("scale",1),	arg("offset",0) } },
	a				= { kind=OP_KIND_COLOR,		args={ arg("scale",1),	arg("offset",0) } },
	invert			= { kind=OP_KIND_COLOR,		args={ arg("amount",1) } },
	luma			= { kind=OP_KIND_COLOR,		args={ arg("threshold",0.5),	arg("tolerance",0.1) } },
	thresh			= { kind=OP_KIND_COLOR,		args={ arg("threshold",0.5),	arg("tolerance",0.04) } },
	color			= { kind=OP_KIND_COLOR,		args={ arg("r",1),	arg("g",1),	arg("b",1),	arg("a",1) } },

--	geometry / coord
	rotate			= { kind=OP_KIND_COORD,		args={ arg("angle",10),			arg("speed",0) } },
	scale			= { kind=OP_KIND_COORD,		args={ arg("amount",1.5),		arg("xMult",1),	arg("yMult",1),	arg("offsetX",0.5),	arg("offsetY",0.5) } },
	pixelate		= { kind=OP_KIND_COORD,		args={ arg("pixelX",20),		arg("pixelY",20) } },
	repeat_			= { kind=OP_KIND_COORD,		args={ arg("repeatX",3),		arg("repeatY",3),	arg("offsetX",0),	arg("offsetY",0) } },
	repeatX			= { kind=OP_KIND_COORD,		args={ arg("reps",3),			arg("offset",0) } },
	repeatY			= { kind=OP_KIND_COORD,		args={ arg("reps",3),			arg("offset",0) } },
	kaleid			= { kind=OP_KIND_COORD,		args={ arg("nSides",4) } },
	scroll			= { kind=OP_KIND_COORD,		args={ arg("scrollX",0.5),		arg("scrollY",0.5),	arg("speedX",0),	arg("speedY",0) } },
	scrollX			= { kind=OP_KIND_COORD,		args={ arg("scrollX",0.5),		arg("speed",0) } },
	scrollY			= { kind=OP_KIND_COORD,		args={ arg("scrollY",0.5),		arg("speed",0) } },

--	blend / combine
	add				= { kind=OP_KIND_COMBINE,	args={ arg("amount",1) } },
	sub				= { kind=OP_KIND_COMBINE,	args={ arg("amount",1) } },
	mult			= { kind=OP_KIND_COMBINE,	args={ arg("amount",1) } },
	blend			= { kind=OP_KIND_COMBINE,	args={ arg("amount",0.5) } },
	diff			= { kind=OP_KIND_COMBINE,	args={} },
	layer			= { kind=OP_KIND_COMBINE,	args={} },
	mask			= { kind=OP_KIND_COMBINE,	args={} },

--	modulators
	modulate		= { kind=OP_KIND_MOD,		args={ arg("amount",0.1) } },
	modulateScale	= { kind=OP_KIND_MOD,		args={ arg("multiple",1),	arg("offset",1) } },
	modulateRotate	= { kind=OP_KIND_MOD,		args={ arg("multiple",1),	arg("offset",0) } },
	modulateRepeat	= { kind=OP_KIND_MOD,		args={ arg("repeatX",3),	arg("repeatY",3),	arg("offsetX",0.5),	arg("offsetY",0.5) } },
	modulateRepeatX	= { kind=OP_KIND_MOD,		args={ arg("reps",3),		arg("offset",0.5) } },
	modulateRepeatY	= { kind=OP_KIND_MOD,		args={ arg("reps",3),		arg("offset",0.5) } },
	modulateKaleid	= { kind=OP_KIND_MOD,		args={ arg("nSides",4) } },
	modulateScrollX	= { kind=OP_KIND_MOD,		args={ arg("scrollX",0.5),	arg("speed",0) } },
	modulateScrollY	= { kind=OP_KIND_MOD,		args={ arg("scrollY",0.5),	arg("speed",0) } },
	modulatePixelate= { kind=OP_KIND_MOD,		args={ arg("multiple",10),	arg("offset",3) } },
	modulateHue		= { kind=OP_KIND_MOD,		args={ arg("amount",1) } },
}

--	JS-name <-> internal-name aliasing for "repeat" (Lua keyword)
local OP_NAME_FROM_JS = { ["repeat"] = "repeat_" }
local OP_NAME_TO_GLSL = { repeat_ = "repeatOp" }	--	GLSL func emitted as "repeatOp"

local function js_to_op(name)
	return OP_NAME_FROM_JS[name] or name
end
local function op_to_glsl_name(name)
	return OP_NAME_TO_GLSL[name] or name
end

----------------------------------------------------------------
--	GLSL helper functions and per-operator function bodies
----------------------------------------------------------------

local GLSL_HELPERS = [[
//	Simplex 3D Noise by Ian McEwan, Ashima Arts
vec4 _permute(vec4 x){ return mod(((x*34.0)+1.0)*x, 289.0); }
vec4 _taylorInvSqrt(vec4 r){ return 1.79284291400159 - 0.85373472095314 * r; }
float _noise(vec3 v){
    const vec2 C = vec2(1.0/6.0, 1.0/3.0);
    const vec4 D = vec4(0.0, 0.5, 1.0, 2.0);
    vec3 i  = floor(v + dot(v, C.yyy));
    vec3 x0 =   v - i + dot(i, C.xxx);
    vec3 g = step(x0.yzx, x0.xyz);
    vec3 l = 1.0 - g;
    vec3 i1 = min(g.xyz, l.zxy);
    vec3 i2 = max(g.xyz, l.zxy);
    vec3 x1 = x0 - i1 + 1.0 * C.xxx;
    vec3 x2 = x0 - i2 + 2.0 * C.xxx;
    vec3 x3 = x0 - 1. + 3.0 * C.xxx;
    i = mod(i, 289.0);
    vec4 p = _permute(_permute(_permute(
        i.z + vec4(0.0, i1.z, i2.z, 1.0))
      + i.y + vec4(0.0, i1.y, i2.y, 1.0))
      + i.x + vec4(0.0, i1.x, i2.x, 1.0));
    float n_ = 1.0/7.0;
    vec3 ns = n_ * D.wyz - D.xzx;
    vec4 j = p - 49.0 * floor(p * ns.z * ns.z);
    vec4 x_ = floor(j * ns.z);
    vec4 y_ = floor(j - 7.0 * x_);
    vec4 x = x_ * ns.x + ns.yyyy;
    vec4 y = y_ * ns.x + ns.yyyy;
    vec4 h = 1.0 - abs(x) - abs(y);
    vec4 b0 = vec4(x.xy, y.xy);
    vec4 b1 = vec4(x.zw, y.zw);
    vec4 s0 = floor(b0)*2.0 + 1.0;
    vec4 s1 = floor(b1)*2.0 + 1.0;
    vec4 sh = -step(h, vec4(0.0));
    vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy;
    vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww;
    vec3 p0 = vec3(a0.xy, h.x);
    vec3 p1 = vec3(a0.zw, h.y);
    vec3 p2 = vec3(a1.xy, h.z);
    vec3 p3 = vec3(a1.zw, h.w);
    vec4 norm = _taylorInvSqrt(vec4(dot(p0,p0),dot(p1,p1),dot(p2,p2),dot(p3,p3)));
    p0 *= norm.x; p1 *= norm.y; p2 *= norm.z; p3 *= norm.w;
    vec4 m = max(0.6 - vec4(dot(x0,x0),dot(x1,x1),dot(x2,x2),dot(x3,x3)), 0.0);
    m = m * m;
    return 42.0 * dot(m*m, vec4(dot(p0,x0),dot(p1,x1),dot(p2,x2),dot(p3,x3)));
}
float _luminance(vec3 rgb){
    const vec3 W = vec3(0.2125, 0.7154, 0.0721);
    return dot(rgb, W);
}
vec3 _rgbToHsv(vec3 c){
    vec4 K = vec4(0.0, -1.0/3.0, 2.0/3.0, -1.0);
    vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
    vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
    float d = q.x - min(q.w, q.y);
    float e = 1.0e-10;
    return vec3(abs(q.z + (q.w - q.y) / (6.0*d + e)), d / (q.x + e), q.x);
}
vec3 _hsvToRgb(vec3 c){
    vec4 K = vec4(1.0, 2.0/3.0, 1.0/3.0, 3.0);
    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}
]]

--	GLSL function bodies, keyed by internal op name
local GLSL_OP_BODIES = {
--	source
	noise = [[
vec4 noise(vec2 _st, float scale, float offset) {
    return vec4(vec3(_noise(vec3(_st*scale, offset*time))), 1.0);
}]],
	voronoi = [[
vec4 voronoi(vec2 _st, float scale, float speed, float blending) {
    vec3 color = vec3(.0);
    _st *= scale;
    vec2 i_st = floor(_st);
    vec2 f_st = fract(_st);
    float m_dist = 10.;
    vec2 m_point;
    for (int j=-1; j<=1; j++) {
        for (int i=-1; i<=1; i++) {
            vec2 neighbor = vec2(float(i), float(j));
            vec2 p = i_st + neighbor;
            vec2 point = fract(sin(vec2(dot(p, vec2(127.1, 311.7)), dot(p, vec2(269.5, 183.3))))*43758.5453);
            point = 0.5 + 0.5*sin(time*speed + 6.2831*point);
            vec2 diff = neighbor + point - f_st;
            float dist = length(diff);
            if (dist < m_dist) {
                m_dist = dist;
                m_point = point;
            }
        }
    }
    color += dot(m_point, vec2(.3, .6));
    color *= 1.0 - blending*m_dist;
    return vec4(color, 1.0);
}]],
	osc = [[
vec4 osc(vec2 _st, float frequency, float sync, float offset) {
    vec2 st = _st;
    float r = sin((st.x-offset/frequency+time*sync)*frequency)*0.5  + 0.5;
    float g = sin((st.x+time*sync)*frequency)*0.5 + 0.5;
    float b = sin((st.x+offset/frequency+time*sync+1.570796)*frequency)*0.5  + 0.5;
    return vec4(r, g, b, 1.0);
}]],
	shape = [[
vec4 shape(vec2 _st, float sides, float radius, float smoothing) {
    vec2 st = _st * 2. - 1.;
    float a = atan(st.x, st.y)+3.1416;
    float r = (2.*3.1416)/sides;
    float d = cos(floor(.5+a/r)*r-a)*length(st);
    return vec4(vec3(1.0-smoothstep(radius, radius + smoothing + 0.0000001, d)), 1.0);
}]],
	gradient = [[
vec4 gradient(vec2 _st, float speed) {
    return vec4(_st, sin(time*speed), 1.0);
}]],
	solid = [[
vec4 solid(vec2 _st, float r, float g, float b, float a) {
    return vec4(r, g, b, a);
}]],
	src = [[
vec4 src(vec2 _st, sampler2D tex) {
    return texture(tex, fract(_st));
}]],
	prev = [[
vec4 prev(vec2 _st) {
    return texture(g_input_texture_0, fract(_st));
}]],

--	color
	brightness = [[
vec4 brightness(vec4 _c0, float amount) {
    return vec4(_c0.rgb + vec3(amount), _c0.a);
}]],
	contrast = [[
vec4 contrast(vec4 _c0, float amount) {
    vec4 c = (_c0-vec4(0.5))*vec4(amount) + vec4(0.5);
    return vec4(c.rgb, _c0.a);
}]],
	saturate = [[
vec4 saturate(vec4 _c0, float amount) {
    const vec3 W = vec3(0.2125, 0.7154, 0.0721);
    vec3 intensity = vec3(dot(_c0.rgb, W));
    return vec4(mix(intensity, _c0.rgb, amount), _c0.a);
}]],
	hue = [[
vec4 hue(vec4 _c0, float hue) {
    vec3 c = _rgbToHsv(_c0.rgb);
    c.r += hue;
    return vec4(_hsvToRgb(c), _c0.a);
}]],
	colorama = [[
vec4 colorama(vec4 _c0, float amount) {
    vec3 c = _rgbToHsv(_c0.rgb);
    c += vec3(amount);
    c = _hsvToRgb(c);
    c = fract(c);
    return vec4(c, _c0.a);
}]],
	posterize = [[
vec4 posterize(vec4 _c0, float bins, float gamma) {
    vec4 c2 = pow(_c0, vec4(gamma));
    c2 *= vec4(bins);
    c2 = floor(c2);
    c2 /= vec4(bins);
    c2 = pow(c2, vec4(1.0/gamma));
    return vec4(c2.xyz, _c0.a);
}]],
	shift = [[
vec4 shift(vec4 _c0, float r, float g, float b, float a) {
    vec4 c2 = vec4(_c0);
    c2.r = fract(c2.r + r);
    c2.g = fract(c2.g + g);
    c2.b = fract(c2.b + b);
    c2.a = fract(c2.a + a);
    return vec4(c2.rgba);
}]],
	r = [[
vec4 r(vec4 _c0, float scale, float offset) {
    return vec4(_c0.r * scale + offset);
}]],
	g = [[
vec4 g(vec4 _c0, float scale, float offset) {
    return vec4(_c0.g * scale + offset);
}]],
	b = [[
vec4 b(vec4 _c0, float scale, float offset) {
    return vec4(_c0.b * scale + offset);
}]],
	a = [[
vec4 a(vec4 _c0, float scale, float offset) {
    return vec4(_c0.a * scale + offset);
}]],
	invert = [[
vec4 invert(vec4 _c0, float amount) {
    return vec4((1.0-_c0.rgb)*amount + _c0.rgb*(1.0-amount), _c0.a);
}]],
	luma = [[
vec4 luma(vec4 _c0, float threshold, float tolerance) {
    float a = smoothstep(threshold-(tolerance+0.0000001), threshold+(tolerance+0.0000001), _luminance(_c0.rgb));
    return vec4(_c0.rgb*a, a);
}]],
	thresh = [[
vec4 thresh(vec4 _c0, float threshold, float tolerance) {
    vec3 rgb = vec3(smoothstep(
        threshold-(tolerance+0.0000001),
        threshold+(tolerance+0.0000001),
        _luminance(_c0.rgb)));
    return vec4(rgb, _c0.a);
}]],
	color = [[
vec4 color(vec4 _c0, float r, float g, float b, float a) {
    vec4 c = vec4(r, g, b, a);
    vec4 pos = step(0.0, c);
    return vec4(mix((1.0-_c0)*abs(c), c*_c0, pos));
}]],

--	coord
	rotate = [[
vec2 rotate(vec2 _st, float angle, float speed) {
    vec2 xy = _st - vec2(0.5);
    float ang = angle + speed*time;
    xy = mat2(cos(ang), -sin(ang), sin(ang), cos(ang)) * xy;
    xy += 0.5;
    return xy;
}]],
	scale = [[
vec2 scale(vec2 _st, float amount, float xMult, float yMult, float offsetX, float offsetY) {
    vec2 xy = _st - vec2(offsetX, offsetY);
    xy *= (1.0 / vec2(amount*xMult, amount*yMult));
    xy += vec2(offsetX, offsetY);
    return xy;
}]],
	pixelate = [[
vec2 pixelate(vec2 _st, float pixelX, float pixelY) {
    vec2 xy = vec2(pixelX, pixelY);
    return (floor(_st * xy) + 0.5)/xy;
}]],
	repeat_ = [[
vec2 repeatOp(vec2 _st, float repeatX, float repeatY, float offsetX, float offsetY) {
    vec2 st = _st * vec2(repeatX, repeatY);
    st.x += step(1., mod(st.y, 2.0)) * offsetX;
    st.y += step(1., mod(st.x, 2.0)) * offsetY;
    return fract(st);
}]],
	repeatX = [[
vec2 repeatX(vec2 _st, float reps, float offset) {
    vec2 st = _st * vec2(reps, 1.0);
    st.y += step(1., mod(st.x, 2.0)) * offset;
    return fract(st);
}]],
	repeatY = [[
vec2 repeatY(vec2 _st, float reps, float offset) {
    vec2 st = _st * vec2(1.0, reps);
    st.x += step(1., mod(st.y, 2.0)) * offset;
    return fract(st);
}]],
	kaleid = [[
vec2 kaleid(vec2 _st, float nSides) {
    vec2 st = _st;
    st -= 0.5;
    float r = length(st);
    float a = atan(st.y, st.x);
    float pi = 2.*3.1416;
    a = mod(a, pi/nSides);
    a = abs(a - pi/nSides/2.);
    return r * vec2(cos(a), sin(a));
}]],
	scroll = [[
vec2 scroll(vec2 _st, float scrollX, float scrollY, float speedX, float speedY) {
    _st.x += scrollX + time*speedX;
    _st.y += scrollY + time*speedY;
    return fract(_st);
}]],
	scrollX = [[
vec2 scrollX(vec2 _st, float scrollX, float speed) {
    _st.x += scrollX + time*speed;
    return fract(_st);
}]],
	scrollY = [[
vec2 scrollY(vec2 _st, float scrollY, float speed) {
    _st.y += scrollY + time*speed;
    return fract(_st);
}]],

--	combine (blends)
	add = [[
vec4 add(vec4 _c0, vec4 _c1, float amount) {
    return (_c0 + _c1)*amount + _c0*(1.0-amount);
}]],
	sub = [[
vec4 sub(vec4 _c0, vec4 _c1, float amount) {
    return (_c0 - _c1)*amount + _c0*(1.0-amount);
}]],
	mult = [[
vec4 mult(vec4 _c0, vec4 _c1, float amount) {
    return _c0*(1.0-amount) + (_c0*_c1)*amount;
}]],
	blend = [[
vec4 blend(vec4 _c0, vec4 _c1, float amount) {
    return _c0*(1.0-amount) + _c1*amount;
}]],
	diff = [[
vec4 diff(vec4 _c0, vec4 _c1) {
    return vec4(abs(_c0.rgb - _c1.rgb), max(_c0.a, _c1.a));
}]],
	layer = [[
vec4 layer(vec4 _c0, vec4 _c1) {
    return vec4(mix(_c0.rgb, _c1.rgb, _c1.a), clamp(_c0.a + _c1.a, 0.0, 1.0));
}]],
	mask = [[
vec4 mask(vec4 _c0, vec4 _c1) {
    float a = _luminance(_c1.rgb);
    return vec4(_c0.rgb * a, a * _c0.a);
}]],

--	combineCoord (modulators)
	modulate = [[
vec2 modulate(vec2 _st, vec4 _c0, float amount) {
    return _st + _c0.xy * amount;
}]],
	modulateScale = [[
vec2 modulateScale(vec2 _st, vec4 _c0, float multiple, float offset) {
    vec2 xy = _st - vec2(0.5);
    xy *= (1.0 / vec2(offset + multiple*_c0.r, offset + multiple*_c0.g));
    xy += vec2(0.5);
    return xy;
}]],
	modulateRotate = [[
vec2 modulateRotate(vec2 _st, vec4 _c0, float multiple, float offset) {
    vec2 xy = _st - vec2(0.5);
    float angle = offset + _c0.x * multiple;
    xy = mat2(cos(angle), -sin(angle), sin(angle), cos(angle)) * xy;
    xy += 0.5;
    return xy;
}]],
	modulateRepeat = [[
vec2 modulateRepeat(vec2 _st, vec4 _c0, float repeatX, float repeatY, float offsetX, float offsetY) {
    vec2 st = _st * vec2(repeatX, repeatY);
    st.x += step(1., mod(st.y, 2.0)) + _c0.r * offsetX;
    st.y += step(1., mod(st.x, 2.0)) + _c0.g * offsetY;
    return fract(st);
}]],
	modulateRepeatX = [[
vec2 modulateRepeatX(vec2 _st, vec4 _c0, float reps, float offset) {
    vec2 st = _st * vec2(reps, 1.0);
    st.y += step(1., mod(st.x, 2.0)) + _c0.r * offset;
    return fract(st);
}]],
	modulateRepeatY = [[
vec2 modulateRepeatY(vec2 _st, vec4 _c0, float reps, float offset) {
    vec2 st = _st * vec2(1.0, reps);
    st.x += step(1., mod(st.y, 2.0)) + _c0.r * offset;
    return fract(st);
}]],
	modulateKaleid = [[
vec2 modulateKaleid(vec2 _st, vec4 _c0, float nSides) {
    vec2 st = _st - 0.5;
    float r = length(st);
    float a = atan(st.y, st.x);
    float pi = 2.*3.1416;
    a = mod(a, pi/nSides);
    a = abs(a - pi/nSides/2.);
    return (_c0.r + r) * vec2(cos(a), sin(a));
}]],
	modulateScrollX = [[
vec2 modulateScrollX(vec2 _st, vec4 _c0, float scrollX, float speed) {
    _st.x += _c0.r*scrollX + time*speed;
    return fract(_st);
}]],
	modulateScrollY = [[
vec2 modulateScrollY(vec2 _st, vec4 _c0, float scrollY, float speed) {
    _st.y += _c0.r*scrollY + time*speed;
    return fract(_st);
}]],
	modulatePixelate = [[
vec2 modulatePixelate(vec2 _st, vec4 _c0, float multiple, float offset) {
    vec2 xy = vec2(offset + _c0.x*multiple, offset + _c0.y*multiple);
    return (floor(_st * xy) + 0.5)/xy;
}]],
	modulateHue = [[
vec2 modulateHue(vec2 _st, vec4 _c0, float amount) {
    return _st + (vec2(_c0.g - _c0.r, _c0.b - _c0.g) * amount * 1.0/resolution);
}]],
}

----------------------------------------------------------------
--	Tokenizer
----------------------------------------------------------------
--	Recognises identifiers, numbers (incl. signed, dot, exp), strings
--	(single/double quotes), and these single-char punctuation: ( ) , . = ;
--	Strips //... and /* ... */ comments.

local function strip_comments(src)
	src = src:gsub("//[^\n]*",			"")		--	line comments
	src = src:gsub("/%*.-%*/",			"")		--	block comments
	return src
end

local function tokenize(src)
	src = strip_comments(src)
	local tokens = {}
	local i = 1
	local n = #src
	while i <= n do
		local c = src:sub(i,i)
		if c:match("[ \t\r\n]") then
			i = i + 1
		elseif c:match("[%a_]") then
			local j = i + 1
			while j <= n and src:sub(j,j):match("[%w_]") do
				j = j + 1
			end
			tokens[#tokens+1] = { kind="ident", v = src:sub(i, j-1) }
			i = j
		elseif c:match("[%d]") or (c == "." and (i+1) <= n and src:sub(i+1,i+1):match("[%d]")) then
			local j = i
			while j <= n and src:sub(j,j):match("[%d%.]") do
				j = j + 1
			end
			--	optional exponent
			if j <= n and (src:sub(j,j) == "e" or src:sub(j,j) == "E") then
				j = j + 1
				if j <= n and (src:sub(j,j) == "+" or src:sub(j,j) == "-") then
					j = j + 1
				end
				while j <= n and src:sub(j,j):match("[%d]") do
					j = j + 1
				end
			end
			tokens[#tokens+1] = { kind="num", v = tonumber(src:sub(i, j-1)) or 0 }
			i = j
		elseif c == "-" or c == "+" then
			--	leading sign on a literal: only when previous token is an op or nothing
			local prev = tokens[#tokens]
			local b_unary =
				(prev == nil) or
				(prev.kind == "punc" and (prev.v == "(" or prev.v == "," or prev.v == "=" or prev.v == "=>" or prev.v == "[" or prev.v == "+" or prev.v == "-" or prev.v == "*" or prev.v == "/"))
			if b_unary and (i+1) <= n and (src:sub(i+1,i+1):match("[%d%.]")) then
				local j = i + 1
				while j <= n and src:sub(j,j):match("[%d%.]") do
					j = j + 1
				end
				if j <= n and (src:sub(j,j) == "e" or src:sub(j,j) == "E") then
					j = j + 1
					if j <= n and (src:sub(j,j) == "+" or src:sub(j,j) == "-") then
						j = j + 1
					end
					while j <= n and src:sub(j,j):match("[%d]") do
						j = j + 1
					end
				end
				tokens[#tokens+1] = { kind="num", v = tonumber(src:sub(i, j-1)) or 0 }
				i = j
			else
				tokens[#tokens+1] = { kind="punc", v = c }
				i = i + 1
			end
		elseif c == "*" or c == "/" then
			tokens[#tokens+1] = { kind="punc", v = c }
			i = i + 1
		elseif c == "=" and (i+1) <= n and src:sub(i+1,i+1) == ">" then
			--	arrow function : "() => <expr>". We tokenize "=>" as a single
			--	punc so the parser can detect and skip it (treat the body
			--	expression as the actual arg value).
			tokens[#tokens+1] = { kind="punc", v = "=>" }
			i = i + 2
		elseif c == "(" or c == ")" or c == "," or c == "." or c == "=" or c == ";" or c == "[" or c == "]" then
			tokens[#tokens+1] = { kind="punc", v = c }
			i = i + 1
		elseif c == "'" or c == '"' then
			local q = c
			local j = i + 1
			while j <= n and src:sub(j,j) ~= q do
				j = j + 1
			end
			tokens[#tokens+1] = { kind="str", v = src:sub(i+1, j-1) }
			i = j + 1
		else
			--	skip unrecognised char
			i = i + 1
		end
	end
	return tokens
end

----------------------------------------------------------------
--	Parser
----------------------------------------------------------------
--	Grammar (informal):
--		stmt    ::= ('o' digit '=')? expr ';'?
--		expr    ::= chain
--		chain   ::= call ( '.' call )*
--		call    ::= IDENT '(' [arglist] ')' | IDENT			(plain identifier, e.g. s0, o0)
--		arglist ::= arg (',' arg)*
--		arg     ::= number | IDENT | chain
--	A chain whose head is a non-call IDENT (e.g. just "o0" or "s0") only valid
--	as the argument of src(...) or as a head of a chain that has only ops.
--	We treat IDENT-as-source identifiers (o0, o1, s0, s1, prev) as args/leaves.

local function parser_new(tokens)
	return { tokens=tokens, pos=1 }
end
local function peek(p, k)		return p.tokens[p.pos + (k or 0)]		end
local function advance(p)		local t = p.tokens[p.pos]; p.pos = p.pos + 1; return t end

local function expect_punc(p, c)
	local t = peek(p)
	if not t or t.kind ~= "punc" or t.v ~= c then
		error("expected '"..c.."' at token "..p.pos.." (got "..(t and (t.kind..":"..tostring(t.v)) or "EOF")..")")
	end
	return advance(p)
end

local parse_chain	--	forward
local parse_arg		--	forward (defined just below; used in parse_arg_primary)
local function parse_arg_primary(p)
	local t = peek(p)
	if not t then error("unexpected EOF in arg list") end

	--	arrow function : "() => <expr>" or "( ) => <expr>".
	--	Hydra uses these to pass dynamic values (e.g. "() => a.fft[0]").
	--	We treat the body as the actual arg : skip the "() =>" prefix.
	if t.kind == "punc" and t.v == "(" then
		--	might be a chain that starts with "(" ? Hydra grammar does not
		--	have that. So we assume it is an arrow function.
		local t1 = peek(p, 1)
		if t1 and t1.kind == "punc" and t1.v == ")" then
			local t2 = peek(p, 2)
			if t2 and t2.kind == "punc" and t2.v == "=>" then
				advance(p); advance(p); advance(p)	-- skip "(" ")" "=>"
				return parse_arg(p)
			end
		end
		--	"(o0)" parenthesised single value : "src((o0), ...)" pattern.
		--	just consume the "(", parse, expect ")".
		advance(p)
		local inner = parse_arg(p)
		if peek(p) and peek(p).kind == "punc" and peek(p).v == ")" then advance(p) end
		return inner
	end

	--	array literal "[a, b, c]" optionally followed by ".smooth(s).fast(f)".
	--	Hydra evaluates arrays as time-varying values cycling through entries.
	if t.kind == "punc" and t.v == "[" then
		advance(p)
		local values = {}
		if peek(p) and not (peek(p).kind == "punc" and peek(p).v == "]") then
			values[#values+1] = parse_arg(p)
			while peek(p) and peek(p).kind == "punc" and peek(p).v == "," do
				advance(p)
				values[#values+1] = parse_arg(p)
			end
		end
		if peek(p) and peek(p).kind == "punc" and peek(p).v == "]" then advance(p) end
		local smooth_arg, fast_arg
		while peek(p) and peek(p).kind == "punc" and peek(p).v == "." do
			advance(p)
			local m = peek(p)
			if not (m and m.kind == "ident") then break end
			advance(p)
			local m_args = {}
			if peek(p) and peek(p).kind == "punc" and peek(p).v == "(" then
				advance(p)
				if peek(p) and not (peek(p).kind == "punc" and peek(p).v == ")") then
					m_args[#m_args+1] = parse_arg(p)
					while peek(p) and peek(p).kind == "punc" and peek(p).v == "," do
						advance(p)
						m_args[#m_args+1] = parse_arg(p)
					end
				end
				if peek(p) and peek(p).kind == "punc" and peek(p).v == ")" then advance(p) end
			end
			if m.v == "smooth" then smooth_arg = m_args[1]
			elseif m.v == "fast" then fast_arg = m_args[1]
			end
			--	other modifiers (.offset, .ease, ...) : ignored for v1
		end
		return { kind = "array", values = values, smooth = smooth_arg, fast = fast_arg }
	end

	if t.kind == "num" then
		advance(p); return { kind="num", v = t.v }
	elseif t.kind == "str" then
		advance(p); return { kind="str", v = t.v }
	elseif t.kind == "ident" then
		--	special : "a.fft[N]" pseudo-identifier maps to an audio uniform.
		--	tokens : ident("a") punc(".") ident("fft") punc("[") num(N) punc("]")
		if t.v == "a" then
			local t1 = peek(p, 1); local t2 = peek(p, 2)
			local t3 = peek(p, 3); local t4 = peek(p, 4)
			local t5 = peek(p, 5)
			if t1 and t1.kind == "punc" and t1.v == "."
			   and t2 and t2.kind == "ident" and t2.v == "fft"
			   and t3 and t3.kind == "punc" and t3.v == "["
			   and t4 and t4.kind == "num"
			   and t5 and t5.kind == "punc" and t5.v == "]" then
				advance(p); advance(p); advance(p); advance(p); advance(p); advance(p)
				local idx = math.floor(tonumber(t4.v) or 0)
				if idx < 0 then idx = 0 end
				if idx > 3 then idx = 3 end
				return { kind="audio_fft", index = idx }
			end
		end
		--	either a plain identifier (s0, o0, etc.) or the head of a chain (osc(...)...)
		--	if next is '(' we parse a chain
		local nxt = p.tokens[p.pos + 1]
		if nxt and nxt.kind == "punc" and nxt.v == "(" then
			return parse_chain(p)
		else
			advance(p); return { kind="ident", v = t.v }
		end
	else
		error("unexpected token kind '"..t.kind.."' in arg")
	end
end

--	parse_arg : standard precedence : "*" and "/" bind tighter than "+" and
--	"-". Left-to-right within the same precedence level. Handles common
--	Hydra patterns like "20 + a.fft[0] * 60" -> "20 + (fft * 60)".
local function parse_arg_term(p)
	local lhs = parse_arg_primary(p)
	while true do
		local t = peek(p)
		if t and t.kind == "punc" and (t.v == "*" or t.v == "/") then
			advance(p)
			local rhs = parse_arg_primary(p)
			lhs = { kind="binop", op = t.v, lhs = lhs, rhs = rhs }
		else
			break
		end
	end
	return lhs
end
parse_arg = function(p)
	local lhs = parse_arg_term(p)
	while true do
		local t = peek(p)
		if t and t.kind == "punc" and (t.v == "+" or t.v == "-") then
			advance(p)
			local rhs = parse_arg_term(p)
			lhs = { kind="binop", op = t.v, lhs = lhs, rhs = rhs }
		else
			break
		end
	end
	return lhs
end

local function parse_call(p)
	local id = advance(p)
	if not id or id.kind ~= "ident" then
		error("expected ident at start of call")
	end
	local args = {}
	if peek(p) and peek(p).kind == "punc" and peek(p).v == "(" then
		expect_punc(p, "(")
		if peek(p) and not (peek(p).kind == "punc" and peek(p).v == ")") then
			args[#args+1] = parse_arg(p)
			while peek(p) and peek(p).kind == "punc" and peek(p).v == "," do
				advance(p)
				args[#args+1] = parse_arg(p)
			end
		end
		expect_punc(p, ")")
	end
	return { kind="call", name = id.v, args = args }
end

parse_chain = function(p)
	local head = parse_call(p)
	local ops = {}
	while peek(p) and peek(p).kind == "punc" and peek(p).v == "." do
		advance(p)
		ops[#ops+1] = parse_call(p)
	end
	return { kind="chain", head = head, ops = ops }
end

--	Skip tokens up to and including the next ";" or until a token that
--	clearly starts a new top-level statement (a known source generator
--	or "o0..o3 ="). Used as recovery after a top-level item we ignore
--	(e.g. "speed = 1", "a.setSmooth(0.96)").
local function skip_to_next_stmt(p)
	while peek(p) do
		local t  = peek(p, 0)
		local nx = peek(p, 1)
		if t.kind == "punc" and t.v == ";" then
			advance(p); return
		end
		if t.kind == "ident" then
			--	"o0 =" / "o1 =" etc. starts a new output assign
			if (t.v == "o0" or t.v == "o1" or t.v == "o2" or t.v == "o3")
			   and nx and nx.kind == "punc" and nx.v == "=" then
				return
			end
			--	"name = () => ..." starts a new function def
			local nx2 = peek(p, 2)
			if nx and nx.kind == "punc" and nx.v == "="
			   and nx2 and nx2.kind == "punc" and nx2.v == "(" then
				return
			end
			--	known source starting a chain
			if OPS[js_to_op(t.v)] and OPS[js_to_op(t.v)].kind == OP_KIND_SRC
			   and nx and nx.kind == "punc" and nx.v == "(" then
				return
			end
		end
		advance(p)
	end
end

local function parse_stmt(p)
	local t0 = peek(p, 0)
	local t1 = peek(p, 1)
	local t2 = peek(p, 2)
	if not t0 then return nil end

	--	"a.something(...)" : top-level method call on the audio global,
	--	or any "ident.method(...)" not starting a known chain. Ignore.
	if t0.kind == "ident" and t0.v == "a"
	   and t1 and t1.kind == "punc" and t1.v == "." then
		skip_to_next_stmt(p)
		return { kind = "skip" }
	end

	--	"name = ..." : function def, output assign, or simple assign.
	if t0.kind == "ident"
	   and t1 and t1.kind == "punc" and t1.v == "=" then
		local name = t0.v
		--	peek further to disambiguate
		local t3 = peek(p, 3)
		local t4 = peek(p, 4)

		--	"name = () => chain" : function definition (no token == arrow)
		if t2 and t2.kind == "punc" and t2.v == "("
		   and t3 and t3.kind == "punc" and t3.v == ")"
		   and t4 and t4.kind == "punc" and t4.v == "=>" then
			advance(p); advance(p); advance(p); advance(p); advance(p)	--	"name" "=" "(" ")" "=>"
			local chain = parse_chain(p)
			if peek(p) and peek(p).kind == "punc" and peek(p).v == ";" then advance(p) end
			return { kind = "fn_def", name = name, chain = chain }
		end

		--	"o0 = chain", "o1 = chain", etc. : real output target assign
		if name == "o0" or name == "o1" or name == "o2" or name == "o3" then
			advance(p); advance(p)	--	"name" "="
			local chain = parse_chain(p)
			if peek(p) and peek(p).kind == "punc" and peek(p).v == ";" then advance(p) end
			return { target = name, chain = chain }
		end

		--	otherwise treat as simple top-level assign : "speed = 1",
		--	"name = something" -> ignore the value, skip to next stmt.
		advance(p); advance(p)	--	"name" "="
		skip_to_next_stmt(p)
		return { kind = "assign_simple", name = name }
	end

	--	default : a bare chain (implicit "o0 =")
	local chain = parse_chain(p)
	if peek(p) and peek(p).kind == "punc" and peek(p).v == ";" then advance(p) end
	--	if the chain ends with ".out(oN)", capture oN as the chain's target
	--	so the program-level picker can route the right chain to the screen.
	--	".out()" or no .out() at all stays target=nil (implicit o0).
	local target = nil
	if chain and chain.ops and #chain.ops > 0 then
		local last = chain.ops[#chain.ops]
		if last and last.name == "out" and last.args and last.args[1] then
			local a = last.args[1]
			if a.kind == "ident" and (a.v == "o0" or a.v == "o1" or a.v == "o2" or a.v == "o3") then
				target = a.v
			end
		end
	end
	return { chain = chain, target = target }
end

local function parse_program(src)
	local tokens = tokenize(src)
	local p = parser_new(tokens)
	local stmts = {}
	while peek(p) do
		local ok, s = pcall(parse_stmt, p)
		if ok and s then
			stmts[#stmts+1] = s
		else
			--	parse error : skip this token and try again next position so
			--	a single bad statement does not abort the whole program.
			advance(p)
		end
	end
	return stmts
end

----------------------------------------------------------------
--	Code generation
----------------------------------------------------------------

local function fmt_num(v)
	if type(v) ~= "number" then return tostring(v) end
	if v == math.floor(v) then
		return string.format("%d.0", v)
	end
	return string.format("%g", v)
end

local arg_to_glsl	--	forward (recursive on binop / array)
--	Build the GLSL expression for a Hydra-style array
--	"[v0, v1, ..., vN].smooth(s).fast(f)" : at runtime, the result picks
--	a value based on time*fast, smoothly interpolating between consecutive
--	entries (smooth=0 -> step transition, smooth=1 -> linear interp).
local function array_glsl(node, ctx)
	local vs = node.values
	local n = #vs
	if n == 0 then return "0.0" end
	if n == 1 then return arg_to_glsl(vs[1], ctx) end

	local fast   = node.fast   and arg_to_glsl(node.fast, ctx)   or "1.0"
	local smooth = node.smooth and arg_to_glsl(node.smooth, ctx) or "1.0"

	--	emit a chain of step+mix to pick value at index i (0..n-1) from
	--	a constant float index. We then do this twice (for v0 = pick(i)
	--	and v1 = pick(i+1 mod n)) and mix between them.
	local arr_str = {}
	for i, v in ipairs(vs) do arr_str[i] = arg_to_glsl(v, ctx) end

	local function pick(idx_expr)
		--	emit nested "(idx_expr < THRESHOLD) ? V[k] : (...)" using
		--	step()-based ternary equivalents. Simpler: nested mix(step).
		--	mix(a, b, step(t, idx_expr)) = idx_expr >= t ? b : a.
		local expr = arr_str[n]
		for k = n-1, 1, -1 do
			expr = string.format("mix(%s, %s, step(%g, %s))",
				arr_str[k], expr, k - 0.5, idx_expr)
		end
		return expr
	end

	--	precompute idx and idx_next as floats to keep the picks simple.
	--	tt = time * fast ; idx = mod(floor(tt), n) ; nxt = mod(idx+1, n).
	--	frac = mix(step(0.5, fract(tt)), fract(tt), smooth).
	local tt   = string.format("(time*(%s))", fast)
	local idx  = string.format("mod(floor(%s), %g)", tt, n)
	local idx_next = string.format("mod(%s+1.0, %g)", idx, n)
	local frac = string.format("mix(step(0.5, fract(%s)), fract(%s), (%s))", tt, tt, smooth)
	local v0 = pick(idx)
	local v1 = pick(idx_next)
	return string.format("mix(%s, %s, %s)", v0, v1, frac)
end

arg_to_glsl = function(arg, ctx)
	if arg.kind == "array" then
		return array_glsl(arg, ctx)
	end
	if arg.kind == "binop" then
		return string.format("(%s %s %s)",
			arg_to_glsl(arg.lhs, ctx), arg.op, arg_to_glsl(arg.rhs, ctx))
	end
	if arg.kind == "num" then
		return fmt_num(arg.v)
	end
	if arg.kind == "audio_fft" then
		--	a.fft[N] : N=0..3 maps to aaa_fu_float[5+N].
		--	The hydraaa MEU update() pushes 4 user-controlled FFT bin
		--	values (or external MIDI/OSC) to those slots each frame.
		return string.format("aaa_fu_float[%d]", 5 + arg.index)
	end
	if arg.kind == "ident" then
		--	special identifiers map to AAASeed inputs.
		--	s0..s3 (external sources) read from the MEU's 4 Tex_ units.
		--	o0..o3 (output buffers / feedback) read from g_fbo_out which
		--	is bound to the chosen FBO MEU's output texture (= previous
		--	frame's render). The user enables feedback by typing the FBO
		--	MEU instance name in the "FBO target" UI field.
		local id = arg.v
		if id == "s0" then return "g_input_texture_0" end
		if id == "s1" then return "g_input_texture_1" end
		if id == "s2" then return "g_input_texture_2" end
		if id == "s3" then return "g_input_texture_3" end
		if id == "o0" or id == "o1" or id == "o2" or id == "o3" then return "g_fbo_out" end
		--	math constants
		if id == "PI" or id == "Math_PI" then return "3.1415926535" end
		--	user identifier kept as-is (will likely be a uniform the user defined)
		return id
	end
	if arg.kind == "str" then
		return "/* str:"..arg.v.."*/0.0"
	end
	error("bad arg kind: "..tostring(arg.kind))
end		--	end of arg_to_glsl

--	Pad a call's args with defaults for any missing positional arg.
--	Returns a list of GLSL strings.
local function pad_args(call, op, idx, ctx)
	local out = {}
	local user_args = call.args or {}
	for i, def in ipairs(op.args) do
		local a = user_args[i]
		if a then
			out[i] = arg_to_glsl(a, ctx)
		else
			--	default
			out[i] = fmt_num(def.default)
		end
	end
	return out
end

--	Produce the GLSL fragment for one chain.
--	idx is the chain depth (0 for outermost). Returns:
--	  glsl_text	(the inline statements ending with `vec4 c<idx> = ...;`)
--	  c_var		("c00", "c01", ...)
--	  st_var	("st00", "st01", ...)
--	  used_ops  table set keyed by internal op name used by this chain (and nested)
--	idx_counter is a single-element table used as a mutable counter
--	across the entire program, so each sub-chain gets a unique index
--	(idx00 for the outermost, then idx01, idx02, ... in encounter order).
--	Earlier we passed idx+1 to all sub-chains which collided when a
--	chain had two parallel sub-chains (two modulates / two blends).
local function gen_chain(chain, idx, parent_st, parent_c, used_ops, idx_counter)
	local sidx = string.format("%02d", idx)
	local c_var  = "c"..sidx
	local st_var = "st"..sidx

	local geos = {}		--	statements that mutate st BEFORE source samples (emitted reversed)
	local source_stmt
	local colors = {}	--	statements that mutate c AFTER source samples (emitted in order)

	--	resolve head: must be a source generator (or a bare identifier like o0/s0)
	local head = chain.head
	local head_name_internal
	local head_call

	if head.kind == "chain" then
		--	head shouldn't be a chain (parse_chain wraps the head call already)
		error("internal: chain head is not a call")
	end
	if head.kind == "call" then
		head_call = head
		head_name_internal = js_to_op(head.name)
	else
		error("expected call at chain head")
	end

	local head_op = OPS[head_name_internal]
	if not head_op then
		error("unknown source generator: '"..head.name.."'")
	end
	used_ops[head_name_internal] = true

	--	build source statement
	local head_args = pad_args(head_call, head_op, idx, nil)
	local args_csv = table.concat(head_args, ", ")
	if head_op == OPS.src then
		--	src(o0|s0): the texture id is in args[1]; use sampler directly
		source_stmt = string.format("vec4 %s = src(%s, %s);", c_var, st_var, args_csv)
	elseif head_op == OPS.prev then
		source_stmt = string.format("vec4 %s = prev(%s);", c_var, st_var)
	elseif head_op == OPS.solid then
		source_stmt = string.format("vec4 %s = solid(%s, %s);", c_var, st_var, args_csv)
	else
		--	osc, noise, voronoi, shape, gradient
		source_stmt = string.format("vec4 %s = %s(%s, %s);", c_var, head_name_internal, st_var, args_csv)
	end

	--	process op chain (post-head)
	for _, op_call in ipairs(chain.ops) do
		local name_internal = js_to_op(op_call.name)
		if name_internal == "out" then
			--	terminator, ignored at chain level (handled at program level)
		else
			local op = OPS[name_internal]
			if not op then
				error("unknown op: '"..op_call.name.."'")
			end
			used_ops[name_internal] = true

			if op.kind == OP_KIND_COLOR then
				local args = pad_args(op_call, op, idx, nil)
				local args_csv = table.concat(args, ", ")
				if #args > 0 then
					colors[#colors+1] = string.format("%s = %s(%s, %s);", c_var, name_internal, c_var, args_csv)
				else
					colors[#colors+1] = string.format("%s = %s(%s);", c_var, name_internal, c_var)
				end

			elseif op.kind == OP_KIND_COORD then
				local args = pad_args(op_call, op, idx, nil)
				local args_csv = table.concat(args, ", ")
				local glsl_name = op_to_glsl_name(name_internal)
				if #args > 0 then
					geos[#geos+1] = string.format("%s = %s(%s, %s);", st_var, glsl_name, st_var, args_csv)
				else
					geos[#geos+1] = string.format("%s = %s(%s);", st_var, glsl_name, st_var)
				end

			elseif op.kind == OP_KIND_COMBINE then
				--	first arg is a sub-chain; remaining args are scalars.
				--	Hydra also lets you pass a buffer/source ident directly
				--	(`.blend(o0)`, `.diff(s1)`, etc.) which is shorthand for
				--	`.blend(src(o0))`. Wrap idents into a synthetic chain.
				local sub_arg = op_call.args[1]
				if sub_arg and sub_arg.kind == "ident"
				   and ( sub_arg.v == "o0" or sub_arg.v == "o1" or sub_arg.v == "o2" or sub_arg.v == "o3"
				      or sub_arg.v == "s0" or sub_arg.v == "s1" or sub_arg.v == "s2" or sub_arg.v == "s3" ) then
					sub_arg = {
						kind = "chain",
						head = { kind = "call", name = "src", args = { { kind = "ident", v = sub_arg.v } } },
						ops  = {},
					}
				end
				if not sub_arg or sub_arg.kind ~= "chain" then
					error("combine op '"..op_call.name.."' expects a chain as first argument")
				end
				--	scalar args (skip the first)
				local rest_call = { args = {} }
				for k = 2, #op_call.args do rest_call.args[k-1] = op_call.args[k] end
				local args = pad_args(rest_call, op, idx, nil)
				local args_csv = table.concat(args, ", ")
				--	render the sub-chain at depth idx+1
				idx_counter[1] = idx_counter[1] + 1
local sub_text, sub_c, sub_st = gen_chain(sub_arg, idx_counter[1], st_var, c_var, used_ops, idx_counter)
				--	the sub-chain reads from same st as parent (per hydra semantics)
				--	combine block as ONE atomic geos entry so reversal does not split it
				local block = table.concat({
					string.format("vec2 %s = %s;", sub_st, st_var),
					sub_text,
				}, "\n")
				geos[#geos+1] = block
				if #args > 0 then
					colors[#colors+1] = string.format("%s = %s(%s, %s, %s);", c_var, name_internal, c_var, sub_c, args_csv)
				else
					colors[#colors+1] = string.format("%s = %s(%s, %s);", c_var, name_internal, c_var, sub_c)
				end

			elseif op.kind == OP_KIND_MOD then
				--	first arg is sub-chain, remaining scalar.
				--	Same shorthand as combine ops : `.modulate(o0)` is
				--	equivalent to `.modulate(src(o0))`. Wrap idents.
				local sub_arg = op_call.args[1]
				if sub_arg and sub_arg.kind == "ident"
				   and ( sub_arg.v == "o0" or sub_arg.v == "o1" or sub_arg.v == "o2" or sub_arg.v == "o3"
				      or sub_arg.v == "s0" or sub_arg.v == "s1" or sub_arg.v == "s2" or sub_arg.v == "s3" ) then
					sub_arg = {
						kind = "chain",
						head = { kind = "call", name = "src", args = { { kind = "ident", v = sub_arg.v } } },
						ops  = {},
					}
				end
				if not sub_arg or sub_arg.kind ~= "chain" then
					error("modulator '"..op_call.name.."' expects a chain as first argument")
				end
				local rest_call = { args = {} }
				for k = 2, #op_call.args do rest_call.args[k-1] = op_call.args[k] end
				local args = pad_args(rest_call, op, idx, nil)
				local args_csv = table.concat(args, ", ")
				idx_counter[1] = idx_counter[1] + 1
local sub_text, sub_c, sub_st = gen_chain(sub_arg, idx_counter[1], st_var, c_var, used_ops, idx_counter)
				--	full modulator block as ONE atomic geos entry: declare sub-st,
				--	render sub-chain (defines sub_c), then mutate parent st via the
				--	modulator function. Atomic so the outer geos reversal does not
				--	split this 3-part sequence.
				local lines_in_block = {
					string.format("vec2 %s = %s;", sub_st, st_var),
					sub_text,
				}
				if #args > 0 then
					lines_in_block[#lines_in_block+1] =
						string.format("%s = %s(%s, %s, %s);", st_var, name_internal, st_var, sub_c, args_csv)
				else
					lines_in_block[#lines_in_block+1] =
						string.format("%s = %s(%s, %s);", st_var, name_internal, st_var, sub_c)
				end
				geos[#geos+1] = table.concat(lines_in_block, "\n")
			end
		end
	end

	--	assemble: geos in REVERSE order (so the LAST chained coord op runs FIRST on st),
	--	then source, then colors in order
	local lines = {}
	for k = #geos, 1, -1 do lines[#lines+1] = geos[k] end
	lines[#lines+1] = source_stmt
	for k = 1, #colors do lines[#lines+1] = colors[k] end
	return table.concat(lines, "\n"), c_var, st_var
end

----------------------------------------------------------------
--	Public entry point
----------------------------------------------------------------

--	The fragment shader receives the VS interface block produced by sha.vert.
--	We use this block (rather than "layout(location = N) in ...") to keep
--	parity with the only MEU vert pattern that compiles on Apple GL Mac
--	(xfb_buffer validation rejects "layout(location) out" otherwise).
--	Four sampler bindings 0..3 expose the four MEU Tex_1..Tex_4 slots
--	to the Hydra "src(sN)" / "src(oN)" syntax (mapped per-N in arg_to_glsl).
local AAASEED_FRAG_PREAMBLE = [[
//	HYDRAAA generated fragment shader (do not edit, regenerated on selection change)

#define time         aaa_fu_float[0]
#define mouse        vec2(aaa_fu_float[1], aaa_fu_float[2])
#define resolution   vec2(max(aaa_fu_float[3], 1.0), max(aaa_fu_float[4], 1.0))

layout(binding = 0) uniform sampler2D g_input_texture_0;
layout(binding = 1) uniform sampler2D g_input_texture_1;
layout(binding = 2) uniform sampler2D g_input_texture_2;
layout(binding = 3) uniform sampler2D g_input_texture_3;
layout(binding = 4) uniform sampler2D g_fbo_out;

in VS_out
{
	vec4	color;
	vec2	tex_coor;
} vs_in;

out vec4 out_result;
]]

local AAASEED_FRAG_MAIN_PROLOGUE = [[
void main(void) {
    vec2 st00 = vs_in.tex_coor;
]]

local AAASEED_FRAG_MAIN_EPILOGUE = [[
    out_result = c00;
}
]]

--	Walk a chain AST and substitute any call to a defined function name
--	(stored in fn_defs as { name -> chain }) with the function's chain.
--	Used as a head of a chain : "s = () => shape()..." then "s().rotate()"
--	expands to "shape()...rotate()". And used as an arg : "s()" inlined.
local function substitute_fns(node, fn_defs, depth)
	depth = depth or 0
	if depth > 8 then return node end		--	prevent infinite recursion
	if type(node) ~= "table" then return node end

	if node.kind == "chain" then
		--	if the head is a call to a defined function, splice in the
		--	function's own chain (head + ops) and append the current ops.
		local h = node.head
		if h and h.kind == "call" and fn_defs[h.name] then
			local fn_chain = fn_defs[h.name]
			fn_chain = substitute_fns(fn_chain, fn_defs, depth+1)
			local ops_combined = {}
			for _, op in ipairs(fn_chain.ops) do
				ops_combined[#ops_combined+1] = substitute_fns(op, fn_defs, depth+1)
			end
			for _, op in ipairs(node.ops) do
				ops_combined[#ops_combined+1] = substitute_fns(op, fn_defs, depth+1)
			end
			return { kind = "chain", head = fn_chain.head, ops = ops_combined }
		end
		--	otherwise recurse into ops args
		local new_ops = {}
		for _, op in ipairs(node.ops) do
			new_ops[#new_ops+1] = substitute_fns(op, fn_defs, depth+1)
		end
		return { kind = "chain", head = substitute_fns(node.head, fn_defs, depth+1), ops = new_ops }
	end
	if node.kind == "call" then
		local new_args = {}
		for _, a in ipairs(node.args or {}) do
			new_args[#new_args+1] = substitute_fns(a, fn_defs, depth+1)
		end
		return { kind = "call", name = node.name, args = new_args }
	end
	if node.kind == "binop" then
		return { kind = "binop", op = node.op,
			lhs = substitute_fns(node.lhs, fn_defs, depth+1),
			rhs = substitute_fns(node.rhs, fn_defs, depth+1) }
	end
	return node
end

function HYDRAAA_TRANSPILE.transpile(source)
	local ok, result = pcall(function()
		local stmts = parse_program(source or "")
		--	collect function defs
		local fn_defs = {}
		for _, s in ipairs(stmts) do
			if s.kind == "fn_def" then
				fn_defs[s.name] = s.chain
			end
		end
		--	use only the LAST stmt assigned to o0, or the last bare chain
		local target_stmt = nil
		for _, s in ipairs(stmts) do
			if (s.target == nil and s.chain) or s.target == "o0" then
				target_stmt = s
			end
		end
		if not target_stmt then
			error("no chain found targeting o0 (or implicit)")
		end

		local target_chain = substitute_fns(target_stmt.chain, fn_defs)
		local used = {}
		local body, c_var, _ = gen_chain(target_chain, 0, "st00", "c00", used, {0})

		--	collect helper bodies in deterministic order
		--	always emit helpers (small enough)
		local body_decls = { GLSL_HELPERS }
		--	emit op bodies for used ops only (avoids GLSL bloat / unused warnings)
		local op_keys = {}
		for k,_ in pairs(used) do op_keys[#op_keys+1] = k end
		table.sort(op_keys)
		for _, k in ipairs(op_keys) do
			local b = GLSL_OP_BODIES[k]
			if b then body_decls[#body_decls+1] = b end
		end

		local lines = {
			AAASEED_FRAG_PREAMBLE,
			table.concat(body_decls, "\n\n"),
			"",
			AAASEED_FRAG_MAIN_PROLOGUE,
			body,
			--	if the user did not explicitly target o0 we still expect c00 as final
			AAASEED_FRAG_MAIN_EPILOGUE,
		}
		return table.concat(lines, "\n")
	end)

	if ok then
		return { ok=true, glsl=result, err=nil }
	else
		return { ok=false, glsl=nil, err=tostring(result) }
	end
end

return HYDRAAA_TRANSPILE
