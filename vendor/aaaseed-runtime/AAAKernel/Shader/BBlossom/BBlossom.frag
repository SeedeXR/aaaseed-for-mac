
in VS_out
{
	vec4			pos_world;
	vec4			pos_ec;
	vec3			nor_ec;
	float			alpha;
	vec4			color;
	vec3			specular;
	vec4			tex_coor[2];
} fs_in;


//	output
//out vec4	gl_FragColor;

//	AAASeed uniform implicit
uniform sampler2D	aaa_tex2d[4];



vec4 get_color( in int unit )
{
	vec4	texture;
	int dim = aaa_tex_dim[unit];
	if( dim == 2 )
	{
		texture = texture2D( aaa_tex2d[unit], fs_in.tex_coor[unit].st );	//gl_TexCoord[unit].st );
//		texture = vec4( 1, 1, 0, 1 );
	}
	else if( dim == 0 )
		texture = vec4( 1, 1, 1, 1 );
	else if( dim == 1 )
		texture = vec4( 1, 0, 0, 1 );
	else
		texture = vec4( 0, 0, 1, 1 );
#if	0
	//	noise 3d
	vec3 pos;
	pos.xy = fs_in.tex_coor.st;
	pos.x *= aaa_fu_float[0];
	pos.y *= aaa_fu_float[1];
	pos.z = aaa_fu_float[2];
	float f = cnoise( pos ) * aaa_fu_float[3] + aaa_fu_float[4];
	texture.xyz += f;
#endif
	return texture;
}

void main (void)
{
//	discard;
	vec4 texture;

#if 0
	//texture = vec4( 1, 1, 0, 1);
	texture = get_color( 0 )
	texture.a = 1;
	#if	(TEX_UNIT_NB > 1 )
//		if( aaa_tex_dim[1] >= 1 )	// because texture 1d is not here for now should >= 1 otherwise
//			texture *= get_color( 1 );
	#endif
	#if	(TEX_UNIT_NB > 2 )
//		if( aaa_tex_dim[2] >= 1 )	// because texture 1d is not here for now should >= 1 otherwise
//			texture *= get_color( 2 );
	#endif

	#if 0
		texture = blend_fs( texture );
	#endif
	//	FALL OFF
	#if 0
		float f = get_fall_off( fs_in.fall_off );
		//	f = 1. - f;
		texture.a = f;
		texture.xyz *= vec3( f, f, f );
	#endif
#else
	texture = get_color( 0 ) * fs_in.color;
//	gl_FragColor = blend_fs( texture );
//	texture = texture2D( aaa_tex2d[0], fs_in.tex_coor[0].st );
//	texture = texture2D( aaa_tex2d[0], fs_in.tex_coor[0].st );
	texture.rgb += texture2D( aaa_tex2d[1], fs_in.tex_coor[1].st ).rgb * fs_in.specular * aaa_fu_vec4[0].rgb * aaa_fu_vec4[0].a;
//	texture *= fs_in.color;
#endif
#if 0
	texture = compute_fog4( texture, fs_in.z_to_eye );
#endif
	gl_FragColor = texture;
//	gl_FragColor.a = 1;
//	gl_FragColor = vec4(0,1,0,.5);

}
