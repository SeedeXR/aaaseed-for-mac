

//gbuffer outputs
layout(location = 0, index = 0) out vec4 out_diffuse_rough;
layout(location = 1, index = 0) out vec4 out_normal_occlusion;
layout(location = 2, index = 0) out vec4 out_specular_reflection;
layout(location = 3, index = 0) out vec4 out_emissive;

in ST_AAA_BV BV;

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
		texture = texture2D( aaa_tex2d[unit], BV.tex_coor[unit].st );	//gl_TexCoord[unit].st );
//		texture = vec4( 1, 1, 0, 1 );
	}
	else if( dim == 0 )		texture = vec4( 1, 1, 1, 1 );
	else if( dim == 1 )		texture = vec4( 1, 0, 0, 1 );
	else					texture = vec4( 0, 0, 1, 1 );

#if	0
	//	noise 3d
	vec3 pos;
	pos.xy = BV.tex_coor.st;
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
	texture = get_color( 0 );
	#if	(TEX_UNIT_NB > 1 )
		if( aaa_tex_dim[1] >= 1 )	// because texture 1d is not here for now should >= 1 otherwise
			texture *= get_color( 1 );
	#endif
	#if	(TEX_UNIT_NB > 2 )
		if( aaa_tex_dim[2] >= 1 )	// because texture 1d is not here for now should >= 1 otherwise
			texture *= get_color( 2 );
	#endif
	#if	(TEX_UNIT_NB > 3 )
		if( aaa_tex_dim[3] >= 1 )	// because texture 1d is not here for now should >= 1 otherwise
			texture *= get_color( 3 );
	#endif

	#if 1
		texture = blend_fs( texture );
	#endif
	//	FALL OFF
	#if 0
		float f = get_fall_off( BV.fall_off );
		//	f = 1. - f;
		texture.a = f;
		texture.xyz *= vec3( f, f, f );
	#endif
#else
	//gl_FrontColor = blend_fs( texture );
	texture = get_color(0);
	//texture = texture2D( aaa_tex2d[0], BV.tex_coor[0].st );
	texture *= BV.color;
#endif
#if 0
	texture = compute_fog4( texture, BV.z_to_eye );
#endif

//	if( texture.a < .5 )	// perhaps we should use the alpha_text value in C++ mapping
//		discard;

	out_diffuse_rough 		= vec4( texture.rgb, 1 );
//	out_diffuse_rough 		= vec4( 1 );
//	out_diffuse_rough 		= texture( diffuse_texture, in_texcoord);
	out_normal_occlusion 	= vec4( BV.nor_world.xyz, .5 );
//	out_normal_occlusion 	= vec4( in_normal_world, 1.0	);
//	out_normal_occlusion 	= vec4( 0,1,0, 1.0				);
    out_specular_reflection = vec4( 0,0,0,0	);
	out_emissive 			= vec4( 0,0,0,1 );

//	gl_FragColor = texture;
//	gl_FragColor.a = 1;
//	gl_FragColor = vec4(0,1,0,.5);
}
