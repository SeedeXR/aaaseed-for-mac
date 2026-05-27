
// for AAASeed
// Author : Franz Hildgen + Mâa

//END include
#pragma unroll
#pragma optionNV(unroll all)

//pipeline inputs
layout(location = 0) in vec3 in_position_world;
//layout(location = 1) in vec3 in_normal_world;
layout(location = 1) in vec2 in_texcoord;

//todo try to pass less information
out ST_AAA_BV BV;

DEFINE_POINT_NORMAL_FN()
// void	get_point_normal( out vec4 vertex, out vec3 normal )
// {
// 	vertex = gl_Vertex;
// //	normal = gl_Normal;
// //	vertex = vec4( in_position_world, 1. );
// //	vertex.y += 1;
// 	normal = vec3( 0., 0., 1. );
// }



void ftexgen( out vec4 texcoor, in vec3 normal, in vec4 ecPosition )
{
	// Compute texture coordinate
	int tex_gen = aaa_tex_gen[0];
	if( tex_gen==0 )
	{
//		texcoor = gl_TextureMatrix[0] * gl_MultiTexCoord0;
		texcoor = gl_TextureMatrix[0] * vec4( in_texcoord, 0, 1 );
	}
	else if( tex_gen == 1 )
	{
		texcoor.s = dot( gl_Vertex, gl_ObjectPlaneS[0] );
		texcoor.t = dot( gl_Vertex, gl_ObjectPlaneT[0] );
		texcoor.p = dot( gl_Vertex, gl_ObjectPlaneR[0] );
		texcoor.q = dot( gl_Vertex, gl_ObjectPlaneQ[0] );
	}
	else if( tex_gen == 2 )
	{
		texcoor.s = dot( ecPosition, gl_EyePlaneS[0] );
		texcoor.t = dot( ecPosition, gl_EyePlaneT[0] );
		texcoor.p = dot( ecPosition, gl_EyePlaneR[0] );
		texcoor.q = dot( ecPosition, gl_EyePlaneQ[0] );
	}
	else if( tex_gen == 3 )
	{
		vec3 ecPosition3;
		ecPosition3 = (vec3(ecPosition)) / ecPosition.w;
		vec2 uv = SphereMap( ecPosition3, normal );
		texcoor = vec4( uv, 0.0, 1.0 );
	}
	else if( tex_gen == 4 )
	{
		vec3 ecPosition3;
		ecPosition3 = (vec3(ecPosition)) / ecPosition.w;
		vec3 reflection;
		reflection = ReflectionMap( ecPosition3, normal );
		texcoor = vec4( reflection, 1.0 );
	}
	else if( tex_gen == 5 )
		texcoor = vec4( normal, 1.0 );
}

void main(void)
{
	vec4	pos;
	vec3	nor;

	get_point_normal( pos, nor );
	BV.pos_world = pos;

	BV.alpha = gl_Color.a;

	// Eye-coordinate position of vertex, needed in various calculations
	pos = gl_ModelViewMatrix * pos;
	BV.pos_ec = pos;

	// Do fixed functionality vertex transform
	gl_Position = gl_ProjectionMatrix * pos ;
	//aaa_cam.view_projection * vec4( position, 1. );

	//here we go to view
	nor = (gl_ModelViewMatrix * vec4( nor.xyz, 0.0 )).xyz;
	nor = normalize( nor );
	BV.nor_ec = nor;
	//then we go back from view
	// we do this double computation because we have no separate model matrix yet (October 2020)
	nor = (aaa_cam.view_inverse * vec4( nor.xyz, 0.0 )).xyz;
	nor = normalize( nor );
	BV.nor_world = vec4( nor, 1 );


	if( aaa_lights != 0u )		//todo really use aaa_lights info
	{
	//	BV.color = gl_Color;
		BV.color = flight( nor.xyz, pos.xyz / pos.w );
	}
	else
	{
		BV.color = gl_Color;
	//	gl_BackColor = gl_Color;
	}
	//color.a = 1.0;	//counter bug ?
	ftexgen( BV.tex_coor[0], nor, pos );

	//for compatibility with existing shader
	gl_TexCoord[0].st = BV.tex_coor[0].st;

	//gl_PointSize += clamp( -(pos.z - aaa_vu_float[0] ) * aaa_vu_float[1], aaa_vu_float[2], aaa_vu_float[3] );
	//gl_PointSize = 8.;
}

