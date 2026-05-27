
// Author : Mâa

#pragma unroll
#pragma optionNV(unroll all)

// define conditionally the aaa_fn()
AAA_DEFINE_FN_BASE_DEPRECIATED() // need this one for now because of transformation
//AAA_DEFINE_FN_BASE()

//this what we pass to the next stage (defined in include_b.glsl)
out ST_AAA_BV BV;

// give a symbolic names to the generic AAASeed uniforms (defined in include_b.glsl)
CONST int axe				= aaa_vu_int[0];

CONST float grad_dist		= aaa_vu_float[0];
CONST float range_min		= aaa_vu_float[1];
CONST float range_max		= aaa_vu_float[2];
CONST float gamma			= aaa_vu_float[3];
CONST float cut				= aaa_vu_float[4];
CONST float th				= aaa_vu_float[5];
CONST float disp_factor		= aaa_vu_float[6];
CONST float push_factor		= aaa_vu_vec4[1].x;
CONST float normal_factor	= aaa_vu_vec4[1].y;

CONST vec3 duv = aaa_vu_vec4[0].xyz * grad_dist;	// z is zero

// declare samplers
layout(binding = 0) uniform sampler2D TEX_MAP;
layout(binding = 1) uniform sampler2D TEX_DISPLACE;



vec3 build_normal( in vec2 uv )
{
	vec3	nor;
	float	a,b;

	//	GRAD X
	a		=	texture( TEX_DISPLACE, uv + duv.xz ).r;
	//a		=	clamp( (a - o) * of, 0., 1. );
	//a		=	smoothstep( range_min, range_max, a );
	b		=	texture( TEX_DISPLACE, uv - duv.xz ).r;
	//b		=	clamp( (b - o) * of, 0., 1. );
	//b		=	smoothstep( range_min, range_max, b );
	vec3	nora = vec3(  duv.x, 0, a-b );

	//	GRAD Y
	a		=	texture( TEX_DISPLACE, uv + duv.zy ).r;
	//a		=	clamp( (a - o) * of, 0., 1. );
	//a		=	smoothstep( range_min, range_max, a );
	b		=	texture( TEX_DISPLACE, uv - duv.zy ).r;
	//b		=	clamp( (b - o) * of, 0., 1. );
	//b		=	smoothstep( range_min, range_max, b );
	vec3	norb = vec3( 0, duv.y, a-b );

	nor = cross( nora, norb );
	nor = normalize( nor );

	return nor;
}

void main(void)
{
	vec2	uv = gl_MultiTexCoord0.st;
	vec4	pos;
	vec3	nor;

	AAA_get_point_normal( pos, nor );

	BV.color = gl_Color;

	//pos.z += sin( pos.x * 100. ) * .2;
	nor = build_normal( uv );
	if( axe == 1 )
		nor = nor.yzx;
	else if( axe == 0 )	
		nor = nor.zyx * vec3( -1, 1, 1 );	//todo this case is strange becaise grid is reversed, grid should be changed
	BV.nor_world = vec4( nor, 1 );

	float disp = texture( TEX_DISPLACE, uv ).r;
	disp = linearstep( range_min, range_max, disp );
	disp = pow( disp, gamma );
	if( disp < cut )
		BV.color.a = 0.;
	if( disp < th )
		disp_factor = 0.;
	pos[axe] += disp * disp_factor;
	pos.xyz += nor * push_factor ;

	//nor *= 1.;
	BV.pos_world = pos;

	//BV.alpha *= gl_Color.a;

	// Eye-coordinate position of vertex, needed in various calculations
	pos = AAA_transform_model_to_view( pos );
	BV.pos_ec = pos;

	// Do fixed functionality vertex transform
	gl_Position = AAA_transform_view_to_projection( pos );

	//here we go to view
	nor = AAA_transform_model_to_view_normal( nor );

	//nor = normalize( nor );
	BV.nor_ec = nor;

	//then we go back from view
	// we do this double computation because we have no separate model matrix yet (October 2020)
	//nor = (aaa_cam.view_inverse * vec4( nor.xyz, 0.)).xyz;
	//nor = normalize( nor );
	//BV.nor_world = vec4( nor, 1 );

#if !AAA_IS_AMD	// flight() need to be work again on AMD
	if( aaa_lights != 0u )		//todo really use aaa_lights info
		BV.color *= flight( nor.xyz * normal_factor, pos.xyz / pos.w );
#endif	//#if !AAA_IS_AMD

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
