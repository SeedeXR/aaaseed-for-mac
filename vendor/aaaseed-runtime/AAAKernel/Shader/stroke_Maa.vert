

out vec4 ecPosition;
out vec3 transformedNormal;
out float alpha;

layout(location = 1) in vec3 att;

void main(void)
{
#if 0

	alpha = gl_Color.a;
	// Eye-coordinate position of vertex, needed in various calculations
	ecPosition = gl_ModelViewMatrix * gl_Vertex;

	// Do fixed functionality vertex transform
	gl_Position = ftransform();
	transformedNormal = fnormal( gl_Normal );
	if( aaa_lights!=uint(0) )
	{
		gl_FrontColor = flight( transformedNormal, ecPosition.xyz );
	}
	else
	{
		gl_FrontColor = gl_Color;
	//	gl_BackColor = gl_Color;
	}
	//gl_FrontColor.a = 1.0;	//counter bug ?
	gl_FrontColor = gl_Color;
	gl_FrontColor.a = 1.0;
	gl_BackColor = gl_FrontColor;

	//float v = clamp( (ecPosition.z + aaa_vu_float[0]) * aaa_vu_float[1], 0.0, 1.0 );
	//gl_PointSize = aaa_vu_float[4] + v * ( aaa_vu_float[5] - aaa_vu_float[4] );
#else

	// Eye-coordinate position of vertex, needed in various calculations
	vec4 pos = gl_Vertex;
	pos.z = pow( att.x, aaa_vu_float[2] ) *  aaa_vu_float[3];
	ecPosition = gl_ModelViewMatrix * pos;

	// Do fixed functionality vertex transform
	//gl_Position = ftransform();
	gl_Position = gl_ProjectionMatrix * ecPosition;

	//transformedNormal = normalize( gl_NormalMatrix * normal );

	gl_FrontColor = gl_Color;
//	gl_BackColor = gl_Color;

	gl_FrontColor.a = 1.0;	//counter bug ?
	gl_TexCoord[0] = gl_TextureMatrix[0] * gl_MultiTexCoord0;

#endif
	gl_PointSize = aaa_vu_float[0] + pow( att.x, aaa_vu_float[2] ) * aaa_vu_float[1];
//	gl_LineSize = pow( att[0], aaa_vu_float[1] ) * aaa_vu_float[0];
}

