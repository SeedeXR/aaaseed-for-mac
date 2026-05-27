//#version 330 compatibility
////#extension GL_ARB_shading_language_include : enable
//#extension GL_EXT_gpu_shader4 : enable

// GLSL vertex shader - fixed functionality + banana deformation
// for AAASeed
// Author : Franz Hildgen + Mâa

#pragma unroll
#pragma optionNV(unroll all)

out ST_AAA_BV BV;

// define conditionally the aaa_fn()
AAA_DEFINE_FN_BASE_DEPRECIATED() // need this one for now because of transformation
//AAA_DEFINE_FN_BASE()

void main(void)
{
	vec4	pos;
	vec3	nor;
	AAA_get_point_normal( pos, nor );
	BV.pos_world = pos;

	BV.alpha = gl_Color.a;

	// Eye-coordinate position of vertex, needed in various calculations
	pos = AAA_transform_model_to_view( pos );
	BV.pos_ec = pos;

	// Do fixed functionality vertex transform
	gl_Position = AAA_transform_view_to_projection( pos );

	//here we go to view
	nor = AAA_transform_model_to_view_normal( nor );
	nor = normalize( nor );
	BV.nor_ec = nor;
	//then we go back from view
	// we do this double computation because we have no separate model matrix yet (October 2020)
	nor = (aaa_cam.view_inverse * vec4( nor.xyz, 0.0 )).xyz;
	nor = normalize( nor );
	BV.nor_world = vec4( nor, 1 );

#if !AAA_IS_AMD
	if( aaa_lights != 0u )		//todo really use aaa_lights info
	{
	//	BV.color = gl_Color;
	//	BV.color = vec4(.1);
		BV.color = flight( nor.xyz, pos.xyz / pos.w );
	//	BV.color = vec4( nor.xyz, 1. );
	//	BV.color = vec4( pos.xyz / pos.w, 1. );
	}
	else
#endif	//#if !AAA_IS_AMD
	{
		BV.color = gl_Color;
	//	gl_BackColor = gl_Color;
	//	BV.color = vec4(.1);
	}

	//color.a = 1.0;	//counter bug ?
#if (AAA_FIX_TEX_COOR_DIM == 4)
		BV.tex_coor[0] = ftexgen_0_vec4( gl_MultiTexCoord0, BV.pos_world, nor, pos );
#else
		BV.tex_coor[0] = ftexgen_0_vec2( gl_MultiTexCoord0, BV.pos_world, nor, pos );
#endif
//	BV.tex_coor[0] = vec4(  aaa_tex_gen[0] );
//	vs_BVout.tex_coor[0] = gl_MultiTexCoord0;

	//for compatibility with existing shader
	gl_TexCoord[0].st = BV.tex_coor[0].st;

	//gl_PointSize += clamp( -(pos.z - aaa_vu_float[0] ) * aaa_vu_float[1], aaa_vu_float[2], aaa_vu_float[3] );
	//gl_PointSize = 8.;
}

