//#version 330
//#extension GL_EXT_geometry_shader4 : enable
////#extension GL_NV_shader_buffer_load : enable

//CIRL GPU Geometry Program: Derek Anderson and Robert Luke
// very simple geometry shader


layout( triangles ) in;
layout( triangle_strip, max_vertices=8 ) out;


//in	vec4	tex_coor_vs[3];
//in	vec4	color_vs[3];
//in	vec3	normal_vs[3];
//out	vec4	tex_coor_gs;
//out	vec4	color_gs;

in VS_out
{
	vec4 tex_coor;
	vec4 color;
	vec3 normal;
} vs_in[3];

out VS_out
{
	vec4 tex_coor;
	vec4 color;
	vec3 normal;
} gs_out;

vec4 position[6];
vec4 tex_coor[6];
vec4 color[6];
vec3 normal[6];

/*
layout (std140) uniform Matrices {
	mat4	gl_ModelViewProjectionMatrix;
    mat4	projModelViewMatrix;
    mat3	normalMatrix;
};
*/


void emit_vertex( in int i )
{
	gl_Position		= position[i];
	gs_out.tex_coor	= tex_coor[i];
	gs_out.color	= color[i];
//	gs_out.normal	= normal[i];
	EmitVertex();
}
void emit_triangle( in int a, in int b, in int c )
{
	emit_vertex( a );
	emit_vertex( b );
	emit_vertex( c );

	EndPrimitive();
}

void emit_triangle_4( in vec4 A, in vec4 B, in vec4 C )
{
	vec4 	a	= ( A + B ) * .5;
	vec4 	b	= ( B + C ) * .5;
	vec4 	c	= ( C + A ) * .5;

	gl_Position = A;	EmitVertex();
	gl_Position = c;	EmitVertex();
	gl_Position = a;	EmitVertex();
	gl_Position = b;	EmitVertex();
	gl_Position = B;	EmitVertex();
	EndPrimitive();

	gl_Position = c;	EmitVertex();
	gl_Position = C;	EmitVertex();
	gl_Position = b;	EmitVertex();
	EndPrimitive();


//	emit_triangle( A, a, c );
//	emit_triangle( a, B, b );
//	emit_triangle( a, b, c );
//	emit_triangle( c, b, C );

//		gs_out.tex_coor	= vs_in[i].tex_coor;
//		gs_out.color	= vs_in[i].color;
//		gs_out.normal	= vs_in[i].normal;
}

void main()
{
/*
	vec3 face_normal = normalize (
					cross( gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz,
							gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz)
							);

*/
/*
	vec4	A	=	gl_in[0].gl_Position;
	vec4	B	=	gl_in[1].gl_Position;
	vec4	C	=	gl_in[2].gl_Position;
	vec4 	a	= ( A + B ) * .5;
	vec4 	b	= ( B + C ) * .5;
	vec4 	c	= ( C + A ) * .5;
*/

	position[0]	= gl_in[0].gl_Position;
	position[2]	= gl_in[1].gl_Position;
	position[4]	= gl_in[2].gl_Position;
	position[1]	= (position[0] + position[2]) * .5;
	position[3]	= (position[2] + position[4]) * .5;
	position[5]	= (position[4] + position[0]) * .5;

	tex_coor[0]	= vs_in[0].tex_coor;
	tex_coor[2]	= vs_in[1].tex_coor;
	tex_coor[4]	= vs_in[2].tex_coor;
	tex_coor[1]	= mix( tex_coor[0], tex_coor[2], .5 );
	tex_coor[3]	= mix( tex_coor[2], tex_coor[4], .5 );
	tex_coor[5]	= mix( tex_coor[4], tex_coor[0], .5 );

	color[0]	= vs_in[0].color;
	color[2]	= vs_in[1].color;
	color[4]	= vs_in[2].color;
	color[1]	= (color[0] + color[2]) * .5;
	color[3]	= (color[2] + color[4]) * .5;
	color[5]	= (color[4] + color[0]) * .5;

/*	normal[0]	= vs_in[0].normal;
	normal[2]	= vs_in[1].normal;
	normal[4]	= vs_in[2].normal;
	normal[1]	= (normal[0] + normal[2]) * .5;
	normal[3]	= (normal[2] + normal[4]) * .5;
	normal[5]	= (normal[4] + normal[0]) * .5;
*/
	// emit_triangle( 0, 1, 5 );
	// emit_triangle( 1, 2, 3 );
	// emit_triangle( 1, 3, 5 );
	// emit_triangle( 5, 3, 4 );

	emit_vertex(0);
	emit_vertex(1);
	emit_vertex(5);
	emit_vertex(3);
	emit_vertex(4);
	EndPrimitive();

	emit_vertex(1);
	emit_vertex(2);
	emit_vertex(3);
	EndPrimitive();

//		gs_out.tex_coor	= vs_in[i].tex_coor;
//		gs_out.color	= vs_in[i].color;
//		gs_out.normal	= vs_in[i].normal;

}
