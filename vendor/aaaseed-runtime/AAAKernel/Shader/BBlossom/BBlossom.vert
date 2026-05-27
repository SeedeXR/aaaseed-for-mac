
//END include
#pragma unroll
#pragma optionNV(unroll all)

out VS_out
{
	vec4			pos_world;
	vec4			pos_ec;
	vec3			nor_ec;
	float			alpha;
	vec4			color;
	vec3			specular;
	vec4			tex_coor[2];
} vs_out;

AAA_DEFINE_FN_POINT_NORMAL()

void flight_bb( out vec4 color, out vec3 specular, const in vec3 normal, const in vec3 ecPosition3  )
{
	vec3 eye = vec3 (0.0, 0.0, 1.0);
	
	st_ads glo;
	// Clear the light intensity accumulators
	glo.ambient  = vec4(0.0);
	glo.diffuse  = vec4(0.0);
	glo.specular = vec4(0.0);

	//	parse 4 bits by 4 bits ( 8 lights ) 
	uint lights = aaa_lights;
	for( int i_flight=0; i_flight<LIGHT_NB; ++i_flight )
	{
		int one = int(lights) & 0xf;
		if( one!=0 )
		{
			if( one == 1 )
				directionalLight( glo, i_flight, normal );
			else if( one == 2 )
				pointLight( glo, i_flight, normal, eye, ecPosition3 );
			else
				spotLight( glo, i_flight, normal, eye, ecPosition3 );
		}
		lights >>= 4u;
		if( lights==0u )	//	avoid rest of loop
			break;
	}
	//	in fixed path l'alpha est la
	color 			= gl_FrontLightModelProduct.sceneColor;
	color.rgb		+= glo.ambient.rgb	* gl_FrontMaterial.ambient.rgb;
	color.rgb		+= glo.diffuse.rgb	* gl_FrontMaterial.diffuse.rgb;
	specular.rgb	=  glo.specular.rgb	* gl_FrontMaterial.specular.rgb;

	color = clamp( color, 0.0, 1.0 );
}

void main(void)
{
	vec4	pos;
	vec3	nor;
	AAA_get_point_normal( pos, nor );
	vs_out.pos_world = pos;

	vs_out.alpha = gl_Color.a;

	// Eye-coordinate position of vertex, needed in various calculations
	pos = gl_ModelViewMatrix * pos;
	vs_out.pos_ec = pos;

	// Do fixed functionality vertex transform
	gl_Position = gl_ProjectionMatrix * pos ;


	nor = fnormal( nor );
	vs_out.nor_ec = nor;

	if( aaa_lights != 0u )		//todo really use aaa_lights info
	{
	//	vs_out.color = gl_Color;
		flight_bb( vs_out.color, vs_out.specular, nor.xyz, pos.xyz / pos.w );
	}
	else
	{
		vs_out.color = gl_Color;
		vs_out.specular = gl_Color.rgb;
	//	gl_BackColor = gl_Color;
	}
	//color.a = 1.0;	//counter bug ?
	vs_out.tex_coor[0] = ftexgen_0_vec4( gl_MultiTexCoord0, vs_out.pos_world, nor, pos );
	vs_out.tex_coor[1] = ftexgen_0_vec4( gl_MultiTexCoord0, vs_out.pos_world, nor, pos, 3 );

	//offset for malacite
	vs_out.tex_coor[0].st = vs_out.tex_coor[0].st + aaa_vu_vec4[0].st;

	//for compatibility with existing shader
	gl_TexCoord[0].st = vs_out.tex_coor[0].st;
	gl_TexCoord[1].st = vs_out.tex_coor[1].st;
	//gl_PointSize += clamp( -(pos.z - aaa_vu_float[0] ) * aaa_vu_float[1], aaa_vu_float[2], aaa_vu_float[3] );
	//gl_PointSize = 8.;
}

