
#define TEX_COOR_DIM 2
#define TEX_COOR_VEC vec2
out VS_out
{
	TEX_COOR_VEC	tex_impli;
	vec4			color;
	vec4 			vertex;
	vec3			normal;
	vec4			tex_coord;
	float			fall_off;
} vs_out;

AAA_DEFINE_FN_POINT_NORMAL()

// --------------------------------------------------------------------------
// MAIN FUNCTION

void main()
{
	vec4	vertex;
	vec3	normal;

	AAA_get_point_normal( vertex, normal );

//	TRANSFORM	normal
	normal = fnormal( normal );
	vs_out.normal = normal;
	vs_out.vertex = vertex;

//	TRANSFORM	position
	//	COMPUTE COOR IN DIFFERENT REFERENCE
	// Eye-coordinate position of vertex, needed in various calculations
	vec4	eye_coor_position = gl_ModelViewMatrix * vertex;

//	vs_out.eye_coor_position = eye_coor_position;
	// Do fixed functionality vertex transform
	//	ok but outdated in 4.2
	//		gl_Position = ftransform();
	//	ok but we already have ecPosition
	//		gl_Position = gl_ModelViewProjectionMatrix * vertex;
	gl_Position = gl_ProjectionMatrix * eye_coor_position;
	//	gl_Position = projection_matrix * modelview_matrix * vec4(vertex, 1.0);
	//	gl_Position = ModelviewProjection * vec4(position, 1);

	vs_out.tex_impli = (gl_TextureMatrix[0] * gl_MultiTexCoord0 ).st;

	vs_out.tex_coord = gl_MultiTexCoord0;

//	vec3 lightDir = normalize(vec3(gl_LightSource[0].position));
//	vs_out.intensity = dot(lightDir,gl_Normal);


//	COMPUTE FALL OFF
	//	this is needed also for light computation
	vec3 ecPosition3 = (vec3 (eye_coor_position)) / eye_coor_position.w;
	// Compute vector from surface to eye
	#if 0
		vec3 VP = vec3 (0.0, 0.0, 1.0) - ecPosition3;	// direction from surface to eye
		VP = normalize( VP );
		vs_out.fall_off = clamp( dot( VP, normal ), 0.0, 1.0 );
	#else
		vs_out.fall_off = max( 	dot( normalize( ecPosition3 ), normal ), 0.0 );
	#endif



	//	COMPUTE LIGHT
	vec4	color;
	color = gl_Color;
	gl_FrontColor = color;
	vs_out.color = color;

}
