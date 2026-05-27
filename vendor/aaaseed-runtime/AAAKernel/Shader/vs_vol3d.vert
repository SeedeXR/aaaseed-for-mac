


out vec4 ecPosition;
out vec3 transformedNormal;

out VS_out
{
	vec4 			pos;
	vec3 			nor;
	vec3			uvw;
//	vec4			color;
//	vec3			normal;
	vec3 			a;
	vec3 			b;
	vec3 			c;
	vec3 			ray;
	vec3			light;
//	float			fall_off;
//	float			z_to_eye;
} vs_out;

void main(void)
{
	float alpha = gl_Color.a;
	// Do fixed functionality vertex transform
	gl_Position = ftransform();


	// Eye-coordinate position of vertex, needed in various calculations
	vs_out.pos = gl_Vertex;
	ecPosition = gl_ModelViewMatrix * gl_Vertex;

	vs_out.nor = gl_Normal;

	//transformedNormal = fnormal( gl_Normal	);

	vec3 a = fnormal( vec3( 1., 0., 0. ) );
	vec3 b = fnormal( vec3( 0., 1., 0. ) );
	vec3 c = fnormal( vec3( 0., 0., 1. ) );

	vs_out.a = a;
	vs_out.b = b;
	vs_out.c = c;

	vec3 ec = normalize( ecPosition.xyz );
//	vec3 ec = ecPosition.xyz;


	vec3 d;
	d.x = dot( a, ec );
	d.y = dot( b, ec );
	d.z = dot( c, ec );

//	vs_out.ray = normalize( d );
	vs_out.ray = d;

	vec4 l = gl_LightSource[ 0 ].position;
	vs_out.light = normalize( ecPosition.xyz - (gl_ModelViewMatrix * l).xyz );


	gl_FrontColor = gl_Color;
//	gl_BackColor = gl_Color;

//	gl_PointSize = clamp( (ecPosition.z - aaa_vu_float[0] ) * aaa_vu_float[1], aaa_vu_float[2], aaa_vu_float[3] );

	vs_out.uvw = (gl_TextureMatrix[0] * gl_MultiTexCoord0 ).stp;
}

