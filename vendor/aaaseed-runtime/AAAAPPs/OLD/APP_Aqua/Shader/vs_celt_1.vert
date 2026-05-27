
#include	"aqua_deform.glsl"

out VS_out
{
	TEX_COOR_VEC	tex_coor[TEX_UNIT_NB];
	vec4			color;
	vec3			normal;
	float			fall_off;
	float			z_to_eye;
} vs_out;

#include	"morph_target.glsl"

// --------------------------------------------------------------------------
// MAIN FUNCTION

void main()
{
	vec4	vertex;
	vec3	normal;
	get_point_normal_morph_target_4( vertex, normal, aaa_vu_vec4[0].w, aaa_vu_vec4[1].w, aaa_vu_vec4[2].w, aaa_vu_vec4[3].w );

	vec3	transformedNormal;
	vec4	ecPosition;

//	DEFORM
	if( aaa_vu_int[0] >= 0. )
		deform_catmull( vertex.xyz, normal );

//	TRANSFORM	position
	//	COMPUTE COOR IN DIFFERENT REFERENCE
	// Eye-coordinate position of vertex, needed in various calculations
	ecPosition = gl_ModelViewMatrix * vertex;

	// Do fixed functionality vertex transform
	//	ok but outdated in 4.2
	//		gl_Position = ftransform();
	//	ok but we already have ecPosition
	//		gl_Position = gl_ModelViewProjectionMatrix * vertex;
	gl_Position = gl_ProjectionMatrix * ecPosition;
	//	gl_Position = projection_matrix * modelview_matrix * vec4(vertex, 1.0);
	//	gl_Position = ModelviewProjection * vec4(position, 1);

	normal = fnormal( normal );

	transformedNormal = normal;
	vs_out.normal = normal;

//	COMPUTE FALL OFF
	//	this is needed also for light computation
	vec3 ecPosition3 = vec3(ecPosition) / ecPosition.w;
	// Compute vector from surface to eye
	#if 0
		vec3 VP = vec3 (0.0, 0.0, 1.0) - ecPosition3;	// direction from surface to eye
		VP = normalize( VP );
		vs_out.fall_off = clamp( dot( VP, normal ), 0.0, 1.0 );
	#else
		vs_out.fall_off = clamp( -dot( normalize( ecPosition3 ), normal ), 0.0, 1.0 );
	#endif

//	COMPUTE LIGHT
	vec4	color;
	if( aaa_lights!=0u )
		color = flight( transformedNormal, ecPosition3 );
	else
		color = gl_Color;
	gl_FrontColor = color;
	vs_out.color = color;

#if 0
//	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;
#else
	ftexgen_for_texcoor( transformedNormal, ecPosition );
#endif
	//	for fog and other
	vs_out.z_to_eye = abs(ecPosition.z);
}