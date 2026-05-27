//#version 330 compatibility
//#extension GL_EXT_gpu_shader4 : enable

out VS_out
{
	vec4 pos_world;
	vec4 color;
	vec3 normal;
} vs_out;

void main(void)
{
	vs_out.pos_world = gl_Vertex;
	// Eye-coordinate position of vertex, needed in various calculations
 	//vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;



	// Do fixed functionality vertex transform
	//gl_Position = gl_ProjectionMatrix * ecPosition;	//	= ftransform( );
	//gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;
	//transformedNormal = fnormal( gl_Normal	);
//	if( aaa_lights!=0u )		//todo really use aaa_lights info
	{
	//	gl_FrontColor = gl_Color;
	//	gl_FrontColor = flight( transformedNormal.xyz, ecPosition.xyz );
//		color_vs = flight( transformedNormal.xyz, ecPosition.xyz );
	}
//	else
	{
	//	gl_FrontColor = gl_Color;
		vs_out.color = gl_Color;
		vs_out.normal = gl_Normal;
	//	gl_BackColor = gl_Color;
	}
	//gl_FrontColor.a = 1.0;	//counter bug ?
	//mvp = gl_ModelViewProjectionMatrix;
	//gl_PointSize = clamp( (ecPosition.z - aaa_vu_float[0] ) * aaa_vu_float[1], aaa_vu_float[2], aaa_vu_float[3] );
}
