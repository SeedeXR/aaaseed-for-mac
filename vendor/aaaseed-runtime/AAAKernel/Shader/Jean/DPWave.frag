
in ST_AAA_BV BV;

//	AAASeed uniform implicit
uniform sampler2D	aaa_tex2d[4];



int             noise_fn    =   aaa_fu_int[0];
float           time        =   aaa_fu_float[0];
float           scale       =   aaa_fu_float[1];
vec4            col1        =   aaa_fu_vec4[0];
vec4            col2        =   aaa_fu_vec4[1];


#define SCALE 8.

vec2 random2( vec2 p ) {
    return fract(sin(vec2(dot(p,vec2(127.1,311.7)),dot(p,vec2(269.5,183.3))))*43758.5453);
}

vec3	voronoi(vec2 uv)
{
    // Scaling
    uv *= SCALE;
    // Space tiling
    vec2 i_uv = floor(uv);
    uv = fract(uv);

    float mdist = 1.;
    vec2 mpt;
    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            vec2 neighbor = vec2(float(x), float(y));
            vec2 pt = random2(i_uv + neighbor);
            pt = .5 + .5 * sin(aaa_fu_float[3] + 6.28*pt);
            vec2 diff = neighbor + pt - uv;
            float dist = length(diff);
            if (dist < mdist)
            {
	            mdist = dist;
                mpt = pt;
            }
        }
    }
    return vec3(mdist, mpt);
}

vec2 hash( vec2 p ) // replace this by something better
{
	p = vec2( dot(p,vec2(127.1,311.7)),
			  dot(p,vec2(269.5,183.3)) );

	return -1.0 + 2.0*fract(sin(p)*43758.5453123);
}

float noise( in vec2 p )
{
    const float K1 = 0.366025404; // (sqrt(3)-1)/2;
    const float K2 = 0.211324865; // (3-sqrt(3))/6;

	vec2 i = floor( p + (p.x+p.y)*K1 );

    vec2 a = p - i + (i.x+i.y)*K2;
    vec2 o = step(a.yx,a.xy);
    vec2 b = a - o + K2;
	vec2 c = a - 1.0 + 2.0*K2;

    vec3 h = max( 0.5-vec3(dot(a,a), dot(b,b), dot(c,c) ), 0.0 );

	vec3 n = h*h*h*h*vec3( dot(a,hash(i+0.0)), dot(b,hash(i+o)), dot(c,hash(i+1.0)));

    return dot( n, vec3(70.0) );

}
float random (vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))
                 * 43758.5453123);
}
float noise2(vec2 st) {
    vec2 i = floor(st);
    vec2 f = fract(st);

    // Four corners in 2D of a tile
    float a = random(i);
    float b = random(i + vec2(1.0, 0.0));
    float c = random(i + vec2(0.0, 1.0));
    float d = random(i + vec2(1.0, 1.0));

    // Smooth Interpolation

    // Cubic Hermine Curve.  Same as SmoothStep()
    vec2 u = f*f*(3.0-2.0*f);
    // u = smoothstep(0.,1.,f);

    // Mix 4 coorners porcentages
    return mix(a, b, u.x) +
            (c - a)* u.y * (1.0 - u.x) +
            (d - b) * u.x * u.y;
}

// Part from https://www.shadertoy.com/view/MdXyzX user :  afl_ext
// its from here https://github.com/achlubek/venginenative/blob/master/shaders/include/WaterHeight.glsl
float wave(vec2 uv, vec2 emitter, float speed, float phase){
	float dst = distance(uv, emitter);
	return pow((0.5 + 0.5 * sin(dst * phase - time * speed)), 5.0);
}
#define GOLDEN_ANGLE_RADIAN 2.39996
float getwavesHI(vec2 uv){
	float w = 0.0;
	float sw = 0.0;
	float iter = 0.0;
	float ww = 1.0;
    uv += time * 0.5;
	// it seems its absolutely fastest way for water height function that looks real
	for(int i=0;i<24;i++){
		w += ww * wave(uv * 0.06 , vec2(sin(iter), cos(iter)) * 10.0, 2.0 + iter * 0.08, 2.0 + iter * 3.0);
		sw += ww;
		ww = mix(ww, 0.0115, 0.4);
		iter += GOLDEN_ANGLE_RADIAN;
	}

	return w / sw;
}

float fbm(vec2 uv)
{
    float ret;
    //float tmp = (1.*voronoi(uv).y);
    if (noise_fn == 0)
    {
        ret =           noise(uv);
        uv *= 2.;
        ret += .5 *     noise(uv);
        uv *= 2.;
        ret += .125 *   noise(uv);
        uv *= 2.;
        ret += .0625 *  noise(uv);
    }
    else if (noise_fn == 1)
    {
        ret =           noise2(uv);
        uv *= 2.;
        ret += .5 *     noise2(uv);
        uv *= 2.;
        ret += .125 *   noise2(uv);
        uv *= 2.;
        ret += .0625 *  noise2(uv);
    }
    else
    {
        ret =           getwavesHI(uv);
        uv *= 2.;
       /* ret += .5 *     getwavesHI(uv);
        uv *= 2.;
        ret += .125 *   getwavesHI(uv);
        uv *= 2.;
        ret += .0625 *  getwavesHI(uv);*/
    }
    return ret;
}

void main( void )
{
    vec4 texture;

	vec2 uv = BV.tex_coor[0].st;
	uv = (uv - .5) * 2.;

    //float val = voronoi(fbm( uv + aaa_fu_float[3] ) * uv ).x;
    float val = fbm( uv * scale + time );

    gl_FragColor = col1 * val + col2 * ( 1 - val );
}