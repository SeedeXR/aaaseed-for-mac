
in VS_out
{
    vec4            pos_world;
    vec4            pos_ec;
    vec3            nor_ec;
    float           alpha;
    vec4            color;
    vec4            tex_coor[2];
} vs_out;

//	AAASeed uniform implicit
uniform sampler2D	aaa_tex2d[4];



float           time        =   aaa_fu_float[0];

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
            pt = .5 + .5 * sin(time + 6.28*pt);
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

float fbm(vec2 uv)
{
    float ret;
    float tmp = (1.*voronoi(uv).y);
    ret = noise(uv);
    uv *= 2.;
    ret += .5 * noise(uv);
    uv *= 2.;
    ret += .125 * noise(uv);
    uv *= 2.;
    ret += .0625 * noise(uv);
    /*
    ret = noise(uv);
    uv *= 2.;
    ret += .5 * noise(uv);
    uv *= 2.;
    ret += .125 * noise(uv);
    uv *= 2.;
    ret += .0625 * noise(uv);*/
    return ret;
}

void main( void )
{
	vec2 uv = vs_out.tex_coor[0].st;
	//vec3 col = vec3( noise( sin( ( uv.x - uv.y ) * 20. ) ) );
    //vec3 col = vec3( sin( length( fbm( uv * 2. + iTime * .1 ) * .5 *iTime ) ) );
    float val = fbm( vec2( ( uv.x-.5 ) * 6., ( uv.y-.5 ) * 4. ) ); //+ iTime * .1 );
//    * (2.*voronoi(uv).y)
    //float lines = sin( ( uv.x + uv.y ) * iTime * iTime ); // if not using noise
    float lines = abs( sin( val * 1. ) );
    //float lines = abs( sin( length( uv - .5 ) * 30. ) + val );

    float lines2 = abs( cos( val * 1. ) );

    float lines3 = abs( sin( val * 1. ) );

    vec3 col = vec3( lines, lines2, lines3 );

    gl_FragColor = vec4(col,1.0);
}