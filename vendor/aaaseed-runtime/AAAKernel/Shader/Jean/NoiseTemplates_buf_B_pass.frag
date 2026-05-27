


in VS_out
{
    vec4            pos_world;
    vec4            pos_ec;
    vec3            nor_ec;
    float           alpha;
    vec4            color;
    vec4            tex_coor[4];
} vs_out;

//	AAASeed uniform implicit
uniform sampler2D	aaa_tex2d[4];



float           time        =   aaa_fu_float[0];

#define SCALE 7.

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
            pt = .5 + .5 * sin(iTime + 6.28*pt);
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

vec3 samplef(const int x, const int y, in vec2 fragCoord)
{
    vec2 uv = vs_out.tex_coor[0].st;
	uv = (uv + vec2(x, y)) / iChannelResolution[0].xy;
	return texture(iChannel0, uv).xyz;
}

float luminance(vec3 c)
{
	return dot(c, vec3(.2126, .7152, .0722));
}

vec3 filterf(in vec2 fragCoord)
{
	vec3 hc =samplef(-1,-1, fragCoord) *  1. + samplef( 0,-1, fragCoord) *  2.
		 	+samplef( 1,-1, fragCoord) *  1. + samplef(-1, 1, fragCoord) * -1.
		 	+samplef( 0, 1, fragCoord) * -2. + samplef( 1, 1, fragCoord) * -1.;

    vec3 vc =samplef(-1,-1, fragCoord) *  1. + samplef(-1, 0, fragCoord) *  2.
		 	+samplef(-1, 1, fragCoord) *  1. + samplef( 1,-1, fragCoord) * -1.
		 	+samplef( 1, 0, fragCoord) * -2. + samplef( 1, 1, fragCoord) * -1.;

	return samplef(0, 0, fragCoord) * pow(luminance(vc*vc + hc*hc), .6);
}

void main( void )
{
	vec2 uv = vs_out.tex_coor[0].st;
//    vec3 col = voronoi(uv);
	vec3 col = filterf(uv);
    gl_FragColor = vec4( col, 1.0 );
}