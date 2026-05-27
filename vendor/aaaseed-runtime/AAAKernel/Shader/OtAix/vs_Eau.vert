

#define TEX_COOR_DIM 2
#define TEX_COOR_VEC vec2
out VS_out
{
	TEX_COOR_VEC	tex_impli;
	vec4			color;
	vec3			normal;
} vs_out;

AAA_DEFINE_FN_POINT_NORMAL()

// --------------------------------------------------------------------------
// MAIN FUNCTION

void main()
{
	vec4	vertex;
	vec3	normal;

	AAA_get_point_normal( vertex, normal );

//	TRANSFORM	position
	//	COMPUTE COOR IN DIFFERENT REFERENCE
	// Eye-coordinate position of vertex, needed in various calculations
	vec4	eye_coor_position = gl_ModelViewMatrix * vertex;

	// Do fixed functionality vertex transform
	//	ok but outdated in 4.2
	//		gl_Position = ftransform();
	//	ok but we already have ecPosition
	//		gl_Position = gl_ModelViewProjectionMatrix * vertex;
	gl_Position = gl_ProjectionMatrix * eye_coor_position;
	//	gl_Position = projection_matrix * modelview_matrix * vec4(vertex, 1.0);
	//	gl_Position = ModelviewProjection * vec4(position, 1);
//	TRANSFORM	normal
	normal = fnormal( normal );
	vs_out.normal = normal;
	
	vs_out.tex_impli = (gl_TextureMatrix[0] * gl_MultiTexCoord0 ).st;

	//	COMPUTE LIGHT
	vec4	color;
	color = gl_Color;
	gl_FrontColor = color;
	vs_out.color = color;

}