
#include	"aqua.glsl"

in VS_out
{
	TEX_COOR_VEC	tex_coor[TEX_UNIT_NB];
	vec4			color;
	vec3			normal;
	float			fall_off;
	float			z_to_eye;
} fs_in;

//	output
out vec4	fragColor;

//	AAASeed uniform implicit
uniform sampler2D	aaa_tex2d[4];


vec4 get_color( in int unit )
{
	vec4	tex;
	int dim = aaa_tex_dim[unit];
	if( dim == 2 )
	{
		tex = texture( aaa_tex2d[unit], fs_in.tex_coor[unit].st );	//gl_TexCoord[unit].st );
//		tex = vec4( 1, 1, 0, 1 );
	}
	else if( dim == 0 )
		tex = vec4( 1, 1, 1, 1 );
	else if( dim == 1 )
		tex = vec4( 1, 0, 0, 1 );
	else
		tex = vec4( 0, 0, 1, 1 );

	return tex;
}


void main (void)
{
//	discard;
	vec4 tex;

#if 1
	//tex = vec4( 1, 1, 0, 1);
	tex = get_color( 0 );
	#if	(TEX_UNIT_NB > 1 )
		if( aaa_tex_dim[1] >= 1 )	// because texture 1d is not here for now should >= 1 otherwise
			tex *= get_color( 1 );
	#endif
	#if	(TEX_UNIT_NB > 2 )
		if( aaa_tex_dim[2] >= 1 )	// because texture 1d is not here for now should >= 1 otherwise
			tex *= get_color( 2 );
	#endif
	#if	(TEX_UNIT_NB > 3 )
		if( aaa_tex_dim[3] >= 1 )	// because texture 1d is not here for now should >= 1 otherwise
			tex *= get_color( 3 );
	#endif

	#if 1
		tex = blend_fs( texture );
	#endif
	//	FALL OFF
	#if 0
		float f = get_fall_off( fs_in.fall_off );
		//	f = 1. - f;
		tex.a = f;
		tex.xyz *= vec3( f, f, f );
	#endif
#else
	//gl_FrontColor = blend( texture );
	tex = texture( aaa_tex2d[1], fs_in.tex_coor[1].st );
	//texture *= fs_in.color;
#endif
#if 0
	tex = compute_fog4( tex, fs_in.z_to_eye );
#endif
	fragColor = tex;
}
