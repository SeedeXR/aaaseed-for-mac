
in	vec3	attrib[];

out VS_out
{
	vec2	uv;
	vec4	color;
	vec3	normal;
} vs_out;



void get_point_normal_morph( out vec4 vertex, out vec3 normal, in float f1 )
{
	vertex = gl_Vertex;
	normal = gl_Normal;

}

// --------------------------------------------------------------------------
// MAIN FUNCTION

void main()
{
	vec4	vertex_ori = gl_Vertex;
	vec3	normal_ori = gl_Normal;

	vec4	vertex = vertex_ori;
	vec3	normal = normal_ori;

	float f = aaa_vu_float[0];
	if( f != 0. )
	{
		vertex.xyz	+=	attrib[0] * f;
		normal		+=	attrib[1] * f;
	}

	vertex.xyz *= aaa_vu_float[3];

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

//	COMPUTE FALL OFF
	//	this is needed also for light computation
	vec3 ecPosition3 = (vec3 (eye_coor_position)) / eye_coor_position.w;

//	COMPUTE LIGHT
	vec4	color;
	if( aaa_lights != 0u )
		color = flight_nb( normal, ecPosition3, 3 );
	else
		color = gl_Color;

	//gl_FrontColor = color;
	vs_out.color = color;

	vs_out.uv = (gl_TextureMatrix[0] * gl_MultiTexCoord0).xy;
}