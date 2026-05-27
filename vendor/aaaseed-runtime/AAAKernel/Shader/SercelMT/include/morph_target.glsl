#ifndef	ATTRIB_NB
#define ATTRIB_NB		8
#endif

in	vec3	attrib[ATTRIB_NB];

#if	ATTRIB_NB >= 8
void get_point_normal_morph_target_sub_4( out vec4 vertex, out vec3 normal, in float f1, in float f2, in float f3, in float f4 )
{
	vertex = gl_Vertex;
	normal = gl_Normal;
	if( f1 != 0. )
	{
		vertex.xyz	+=	( attrib[0] - gl_Vertex.xyz ) * f1;
		normal		+=	( attrib[1] - gl_Normal ) * f1;
	}
	if( f2 != 0. )
	{
		vertex.xyz	+=	( attrib[2] - gl_Vertex.xyz ) * f2;
		normal		+=	( attrib[3] - gl_Normal ) * f2;
	}
	if( f3 != 0. )
	{
		vertex.xyz	+=	( attrib[4] - gl_Vertex.xyz ) * f3;
		normal		+=	( attrib[5] - gl_Normal ) * f3;
	}
	if( f4 != 0. )
	{
		vertex.xyz	+=	( attrib[6] - gl_Vertex.xyz ) * f4;
		normal		+=	( attrib[7] - gl_Normal ) * f4;
	}
}
#endif

#if	ATTRIB_NB >= 6
void get_point_normal_morph_target_sub_3( out vec4 vertex, out vec3 normal, in float f1, in float f2, in float f3 )
{
	vertex = gl_Vertex;
	normal = gl_Normal;
	if( f1 != 0. )
	{
		vertex.xyz	+=	( attrib[0] - gl_Vertex.xyz ) * f1;
		normal		+=	( attrib[1] - gl_Normal ) * f1;
	}
	if( f2 != 0. )
	{
		vertex.xyz	+=	( attrib[2] - gl_Vertex.xyz ) * f2;
		normal		+=	( attrib[3] - gl_Normal ) * f2;
	}
	if( f3 != 0. )
	{
		vertex.xyz	+=	( attrib[4] - gl_Vertex.xyz ) * f3;
		normal		+=	( attrib[5] - gl_Normal ) * f3;
	}
}
#endif

#if	ATTRIB_NB >= 4
void get_point_normal_morph_target_sub_2( out vec4 vertex, out vec3 normal, in float f1, in float f2 )
{
	vertex = gl_Vertex;
	normal = gl_Normal;
	if( f1 != 0. )
	{
		vertex.xyz	+=	( attrib[0] - gl_Vertex.xyz ) * f1;
		normal		+=	( attrib[1] - gl_Normal ) * f1;
	}
	if( f2 != 0. )
	{
		vertex.xyz	+=	( attrib[2] - gl_Vertex.xyz ) * f2;
		normal		+=	( attrib[3] - gl_Normal ) * f2;
	}
}
#endif

void get_point_normal_morph_target_sub_1( out vec4 vertex, out vec3 normal, in float f1 )
{
	vertex = gl_Vertex;
	normal = gl_Normal;
	if( f1 != 0. )
	{
		vertex.xyz	+=	( attrib[0] - gl_Vertex.xyz ) * f1;
		normal		+=	( attrib[1] - gl_Normal ) * f1;
	}
}

//
//	No soustraction here because it is preprocessed un AAASeed bdd_tri
//
#if	ATTRIB_NB >= 8
void get_point_normal_morph_target_4( out vec4 vertex, out vec3 normal, in float f1, in float f2, in float f3, in float f4 )
{
	vertex = gl_Vertex;
	normal = gl_Normal;
	if( f1 != 0. )
	{
		vertex.xyz	+=	attrib[0] * f1;
		normal		+=	attrib[1] * f1;
	}
	if( f2 != 0. )
	{
		vertex.xyz	+=	attrib[2] * f2;
		normal		+=	attrib[3] * f2;
	}
	if( f3 != 0. )
	{
		vertex.xyz	+=	attrib[4] * f3;
		normal		+=	attrib[5] * f3;
	}
	if( f4 != 0. )
	{
		vertex.xyz	+=	attrib[6] * f4;
		normal		+=	attrib[7] * f4;
	}
}
#endif

#if	ATTRIB_NB >= 6
void get_point_normal_morph_target_3( out vec4 vertex, out vec3 normal, in float f1, in float f2, in float f3 )
{
	vertex = gl_Vertex;
	normal = gl_Normal;
	if( f1 != 0. )
	{
		vertex.xyz	+=	attrib[0] * f1;
		normal		+=	attrib[1] * f1;
	}
	if( f2 != 0. )
	{
		vertex.xyz	+=	attrib[2] * f2;
		normal		+=	attrib[3] * f2;
	}
	if( f3 != 0. )
	{
		vertex.xyz	+=	attrib[4] * f3;
		normal		+=	attrib[5] * f3;
	}
}
#endif

#if	ATTRIB_NB >= 4
void get_point_normal_morph_target_2( out vec4 vertex, out vec3 normal, in float f1, in float f2 )
{
	vertex = gl_Vertex;
	normal = gl_Normal;
	if( f1 != 0. )
	{
		vertex.xyz	+=	attrib[0] * f1;
		normal		+=	attrib[1] * f1;
	}
	if( f2 != 0. )
	{
		vertex.xyz	+=	attrib[2] * f2;
		normal		+=	attrib[3] * f2;
	}
}
#endif

void get_point_normal_morph_target_1( out vec4 vertex, out vec3 normal, in float f1 )
{
	vertex = gl_Vertex;
	normal = gl_Normal;
	if( f1 != 0. )
	{
		vertex.xyz	+=	attrib[0] * f1;
		normal		+=	attrib[1] * f1;
	}
}
