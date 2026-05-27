// GLSL fragment shader
// Halftone

// Uniform variables for texturing
uniform sampler2D tex0;

const float	NOISE_PATCHES = 4.0;
const vec2	DOTS_PER_BIT = 8.0;
const vec2	IMG_DIVS = 8.0;
const vec3	lumconst = { 0.2, 0.7, 0.1 };


vec4 make_tones_n( vec3 pos )
{
	float n = 0.5 * ( 1.0 + noise( pos * 20.0 ) );
	float s = pos.z;
	if( n < s )
		return vec4( gl_Color.xyz, 1.0) ;
	else
		return vec4( 0.0 , 0.0 , 0.0 , 1.0);
}

vec4 halftone_n_PS()
{
	vec4	scnC = texture2D( tex0, gl_TexCoord[0].st );
	float	lum   = dot( lumconst, scnC.xyz );
	vec2	lx0   = vec2( NOISE_PATCHES * IMG_DIVS * gl_TexCoord[0].st );
	vec3	lx    = vec3(lx0.x, lx0.y, lum);
	vec4	dotC  = make_tones_n(lx);

	return vec4( dotC.xyz, scnC.a );
}


void main()
{
    gl_FragColor = blend( halftone_n_PS() );
}
