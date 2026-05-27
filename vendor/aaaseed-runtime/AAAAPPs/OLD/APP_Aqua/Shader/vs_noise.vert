

out VS_out
{
	float			grey;
	float			alpha;
} vs_out;


// --------------------------------------------------------------------------
// MAIN FUNCTION

void main()
{
	vec4	vertex = gl_Vertex;

	float v = snoise( (vertex.xyz + aaa_vu_vec4[1].xyz) * aaa_vu_vec4[0].xyz );
	v = (v + 1.) * .5;
	vs_out.grey = v;

	v = snoise( (vertex.xyz + aaa_vu_vec4[3].xyz) * aaa_vu_vec4[2].xyz );
	vs_out.alpha = aaa_vu_float[0] + v * aaa_vu_float[1];

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

//	vec4	color = gl_Color;

//	gl_FrontColor = vs_out.color;
}