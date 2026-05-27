//#version 330
//#extension GL_EXT_geometry_shader4 : enable
////#extension GL_NV__buffer_load : enable

//CIRL GPU Geometry Program: Derek Anderson and Robert Luke
// very simple geometry shader

/*
GEOMETRY SHADER DOCUMENTATION

Geometry language built-in outputs:
	out vec4 gl_FrontColor;
	out vec4 gl_BackColor;
	out vec4 gl_FrontSecondaryColor;
	out vec4 gl_BackSecondaryColor;
	out vec4 gl_TexCoord[]; 	// at most gl_MaxTextureCoords
	out float gl_FogFragCoord;

Geometry language input varying variables:
	in vec4 gl_FrontColorIn[gl_VerticesIn];
	in vec4 gl_BackColorIn[gl_VerticesIn];
	in vec4 gl_FrontSecondaryColorIn[gl_VerticesIn];
	in vec4 gl_BackSecondaryColorIn[gl_VerticesIn];
	in vec4 gl_TexCoordIn[gl_VerticesIn][]; 	// at most will be// gl_MaxTextureCoords
	in float gl_FogFragCoordIn[gl_VerticesIn];
	in vec4 gl_PositionIn[gl_VerticesIn];
	in float gl_PointSizeIn[gl_VerticesIn];
	in vec4 gl_ClipVertexIn[gl_VerticesIn];

Geometry Shader Function

	This section contains functions that are geometry language specific.

	Syntax:
	 void EmitVertex();
	 void EndPrimitive();

	Description:

	 The function EmitVertex() specifies that a vertex is completed. A vertex is added to the current output primitive using the current values of the varying output variables and the current values of the special built-in output variables gl_PointSize, gl_ClipVertex, gl_Layer, gl_Position and gl_PrimitiveID. The values of any unwritten output variables are undefined. The values of all varying output variables and the special built-in output variables are undefined after a call to EmitVertex(). If a geometry shader, in one invocation, emits more vertices than the valueGEOMETRY_VERTICES_OUT_EXT, these emits may have no effect.

	The function EndPrimitive() specifies that the current output primitive is completed and a new output primitive (of the same type) should be started. This function does not emit a vertex. The effect of EndPrimitive() is roughly equivalent to calling End followed by a new Begin, where the primitive mode is taken from the program object parameter GEOMETRY_OUTPUT_TYPE_EXT. If the output primitive type is POINTS, calling EndPrimitive() is optional.
*/


/*
void main(void)
{

	//increment variable
	int i;

	//METHOD 1:
	// this example has two parts
	//	step a) draw the primitive pushed down the pipeline
	//		 there are gl_Vertices # of vertices
	//		 put the vertex value into gl_Position
	//		 use EmitVertex => 'create' a new vertex
	// 		use EndPrimitive to signal that you are done creating a primitive!
	//	step b) create a new piece of geometry (I.E. WHY WE ARE USING A GEOMETRY SHADER!)
	//		I just do the same loop, but swizzle the x and y values
	//	result => the line we want to draw, and the same line, but along the other axis

	//Pass-thru!
	for( i=0; i< gl_VerticesIn; ++i )
	{
		gl_Position = gl_PositionIn[i];
		EmitVertex();
	}
	EndPrimitive();


	//New piece of geometry!  We just swizzle the x and y terms
	for(i=0; i< gl_VerticesIn; i++){
		gl_Position = gl_PositionIn[i];
		gl_Position.xy = gl_Position.yx;
		EmitVertex();
	}
	EndPrimitive();
}
*/

layout( triangles ) in;
layout( triangle_strip, max_vertices=48 ) out;


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


/*
layout (std140) uniform Matrices {
	mat4	gl_ModelViewProjectionMatrix;
    mat4	projModelViewMatrix;
    mat3	normalMatrix;
};
*/

void emit_triangle( in vec4 a, in vec4 b, in vec4 c )
{
	gl_Position = a;
	EmitVertex();
	gl_Position = b;
	EmitVertex();
	gl_Position = c;
	EmitVertex();

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
	vec4	A	=	gl_in[0].gl_Position;
	vec4	B	=	gl_in[1].gl_Position;
	vec4	C	=	gl_in[2].gl_Position;
//	test on normalm extention
//	A.xyz	+=	vs_in[0].normal * aaa_gu_float[0];
//	B.xyz	+=	vs_in[1].normal * aaa_gu_float[0];
//	C.xyz	+=	vs_in[2].normal * aaa_gu_float[0];
	vec4 	a	= ( A + B ) * .5;
	vec4 	b	= ( B + C ) * .5;
	vec4 	c	= ( C + A ) * .5;

	emit_triangle_4( A, a, c );
	emit_triangle_4( a, B, b );
	emit_triangle_4( a, b, c );
	emit_triangle_4( c, b, C );


//		gs_out.tex_coor	= vs_in[i].tex_coor;
//		gs_out.color	= vs_in[i].color;
//		gs_out.normal	= vs_in[i].normal;

}
