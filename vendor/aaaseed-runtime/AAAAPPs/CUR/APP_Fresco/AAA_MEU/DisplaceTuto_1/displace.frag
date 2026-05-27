

//gbuffer outputs
layout(location = 0, index = 0) out vec4 out_frag;
//layout(location = 0, index = 0) out vec4 out_diffuse_rough;
// layout(location = 1, index = 0) out vec4 out_normal_occlusion;
// layout(location = 2, index = 0) out vec4 out_specular_reflection;
// layout(location = 3, index = 0) out vec4 out_emissive;

//layout(binding = 0) uniform sampler2D TEX_MAP;
//layout(binding = 1) uniform sampler2D TEX_DISPLACE;

CONST int out_mode			= aaa_fu_int[0];

in ST_AAA_BV BV;

//	output
//out vec4	gl_FragColor;

//	AAASeed uniform implicit
uniform sampler2D	aaa_tex2d[4];

vec4 get_color( in int unit )
{
	vec4 texture;
	int dim = aaa_tex_dim[unit];
	if( dim == 2 )
	{
		texture = texture2D( aaa_tex2d[unit], BV.tex_coor[0].st );	//gl_TexCoord[unit].st );
//		texture = vec4( 1, 1, 0, 1 );
	}
	else if( dim == 0 )		texture = vec4( 1, 1, 1, 1 );
	else if( dim == 1 )		texture = vec4( 1, 0, 0, 1 );
	else					texture = vec4( 0, 0, 1, 1 );

	return texture;
}

void main (void)
{
	vec4 texture;
	if( out_mode>0 )
	{
		switch(out_mode)
		{
		case 1:	texture = vec4( BV.nor_world.rgb, 1 );				break;
		case 2:	texture = vec4( BV.nor_world.rgb * .5 + .5, 1 );	break;
		case 3: texture = vec4( BV.nor_ec.rgb, 1 );					break;
		case 4: texture = vec4( BV.nor_ec.rgb * .5 + .5, 1 );		break;
		}
	}
	else
	{
		if( BV.color.a < .001 )
			discard;
		//gl_FrontColor = blend_fs( texture );
		texture = get_color(0);
		if( texture.a < .001 )	// perhaps we should use the alpha_text value in C++ mapping
			discard;
	}

	//texture *= BV.color;


	
	out_frag = vec4( texture.rgb, 1 ) * BV.color;
//	out_diffuse_rough 		= vec4( BV.nor_world.rgb, 1 );
//	out_diffuse_rough 		= vec4( BV.nor_world.rgb * .5 + .5, 1 );
//	out_diffuse_rough 		= BV.color;
//	out_diffuse_rough 		= vec4( 1 );
//	out_diffuse_rough 		= texture( diffuse_texture, in_texcoord);

//	out_normal_occlusion 	= vec4( BV.nor_world.xyz, .5 );
//	out_normal_occlusion 	= vec4( in_normal_world, 1.0	);
//	out_normal_occlusion 	= vec4( 0,1,0, 1.0				);

//  out_specular_reflection = vec4( 0,0,0,0	);
//	out_emissive 			= vec4( 0,0,0,1 );

//	gl_FragColor = texture;
//	gl_FragColor.a = 1;
//	gl_FragColor = vec4(0,1,0,.5);
}
