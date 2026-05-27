

// GLSL vertex shader - fixed functionality + banana deformation
// for Aaaseed
// Author : Franz Hildgen


//out vec4 ecPosition;
out vec3 transformedNormal;

out VS_out
{
//	mat4			mv_inverse;
	vec4 			pos_world;
	vec4			color;
	vec4			ecPosition;
	vec3 			nor_world;
	vec3			uvw;
//	vec3			normal;
	vec3 			x_world;
	vec3 			y_world;
	vec3 			z_world;
	vec3 			ray;
	vec3			light_in;
//	float			fall_off;
//	float			z_to_eye;
} vs_out;


void main(void)
{
	vs_out.color		= gl_Color;
	vs_out.pos_world	= gl_Vertex;

	// Do fixed functionality vertex transform
	gl_Position = ftransform();

//	vs_out.mv_inverse = inverse(gl_ModelViewMatrix);

	// Eye-coordinate position of vertex, needed in various calculations
	vec4 ecPosition = gl_ModelViewMatrix * gl_Vertex;
	vs_out.ecPosition = ecPosition;

	vs_out.nor_world = gl_Normal;

	//transformedNormal = fnormal( gl_Normal	);

	vs_out.x_world = fnormal( vec3( 1., 0., 0. ) );
	vs_out.y_world = fnormal( vec3( 0., 1., 0. ) );
	vs_out.z_world = fnormal( vec3( 0., 0., 1. ) );

	vec4 li_pos = gl_LightSource[ 0 ].position;
	vs_out.light_in = normalize( ecPosition.xyz - (gl_ModelViewMatrix * li_pos).xyz );

//	if( aaa_lights!=0u )		//todo really use aaa_lights info
//	{
//		gl_FrontColor = flight( transformedNormal.xyz, ecPosition.xyz );
//	}
//	else
	{
		gl_FrontColor = gl_Color;
	//	gl_BackColor = gl_Color;
	}
//	gl_PointSize = clamp( (ecPosition.z - aaa_vu_float[0] ) * aaa_vu_float[1], aaa_vu_float[2], aaa_vu_float[3] );

	vs_out.uvw = (gl_TextureMatrix[0] * gl_MultiTexCoord0 ).stp;
}

