// GLSL fragment shader
// Halftone

// Uniform variables for texturing
uniform sampler2D	aaa_tex2d[4];


float	NOISE_PATCHES = 4.0;
vec2	DOTS_PER_BIT = {aaa_fu_float[0], aaa_fu_float[0]}; //8.0;
vec3	lumconst = { aaa_fu_vec4[0].rgb }; // { 0.2, 0.7, 0.1 };
float	use_color = aaa_fu_float[1];
float	black_transparent = aaa_fu_float[2];

vec4 make_tones_r( vec3 pos, in vec4 tex_color )
{
	vec3	pos_mod;
	pos_mod.x = mod( pos.x, 1 );
	pos_mod.y = mod( pos.y, 1 );
	pos_mod.z = mod( pos.z, 4 );
	vec2	delta = pos_mod.xy - vec2( 0.5, 0.5 );
	float	d = dot( delta, delta );
	float	rSquared = ( pos_mod.z * pos_mod.z ) / 2.0;
//	float	n2 = ( d < rSquared ) ? 1.0 : 0.0;
//	return vec4( n2, n2, n2, 1.0);
	float	alpha;
	if( black_transparent >= 1.0 )
		alpha = 0.0;
	else
		alpha = 1.0;
	if( d < rSquared )
		if( use_color >= 1.0 )
			return vec4( gl_Color.xyz, tex_color.a ) ;
		else
			return vec4( tex_color.xyz, tex_color.a ) ;
	else
		return vec4( 0.0 , 0.0 , 0.0 , alpha);
}

vec4 halftone_r_PS()
{
	vec4  scnC  = texture2D( aaa_tex2d[0], gl_TexCoord[0].st );
	float lum   = dot( lumconst, scnC.xyz );
	vec2  lx0   = vec2( DOTS_PER_BIT * gl_TexCoord[0].st );
	vec3  lx    = vec3( lx0.x, lx0.y, lum);
	vec4  dotC  = make_tones_r( lx, scnC );

	return dotC;
//		return vec4( dotC.xyz, scnC.a);

}


void main()
{
	gl_FragColor = blend( halftone_r_PS() );//dot_color;
}
