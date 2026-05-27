//#version 330 compatibility
////#extension GL_ARB_shading_language_include : enable
//#extension GL_EXT_gpu_shader4 : enable

// GLSL vertex shader - fixed functionality + banana deformation
// for AAASeed
// Author : Franz Hildgen + Mâa

#pragma unroll
#pragma optionNV(unroll all)

// define conditionally the aaa_fn()
AAA_DEFINE_FN_BASE_DEPRECIATED() // need this one for now because of transformation
//AAA_DEFINE_FN_BASE()

out ST_AAA_BV BV;
uniform sampler2D	aaa_tex2d[4];
CONST float	strenght	= aaa_fu_float[0];
CONST float	def_gamma	= aaa_fu_float[1];


void main(void)
{
	vec4	pos;
	AAA_get_point( pos );

	// transform to polar coor
	float angle = atan( pos.x, pos.z );
	float rad = sqrt( dot( pos.xz, pos.xz ) );

	// generate uv using 
	//	angle + offset made from x translation
	//	and height with offset and factor manually set to use the central part of video, avoiding black bottom/top
	// and get texture from it
	float offset = gl_ModelViewMatrix[3].x;
	vec3 pix = texture( aaa_tex2d[1], vec2( angle/3.14159/2 + .25 + offset, .17 + pos.y*.25 ) ).rgb;

	// get gray
	float def = compute_gray(pix);	// * (1 - clamp_01(pos.y*.25)) * 4.;

	// and use it to change radius
	rad *= 1. + pow( def, def_gamma ) * strenght * 10.; 
	pos.x = cos(angle) * rad; 
	pos.z = sin(angle) * rad;

	// use also video color
	BV.color = vec4( pix, 1 );
//	BV.color = vec4( 0, pos.y*.25, 0, 1);
//	BV.color = vec4(1.);

	// Eye-coordinate position of vertex, needed in various calculations
	pos = AAA_transform_model_to_view( pos );

	// Do fixed functionality vertex transform
	gl_Position = AAA_transform_view_to_projection( pos );

	BV.color *= gl_Color;

}

