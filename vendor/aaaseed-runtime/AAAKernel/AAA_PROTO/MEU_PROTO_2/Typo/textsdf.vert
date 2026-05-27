
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

AAA_DEFINE_FN_POINT_NORMAL()
// void	AAA_get_point_normal( out vec4 vertex, out vec3 normal )
// {
// 	vertex = gl_Vertex;
// //	normal = gl_Normal;
// //	vertex = vec4( in_position_world, 1. );
// //	vertex.y += 1;
// 	normal = vec3( 0., 0., 1. );
// }


void main(void)
{
	vec4	pos;
	vec3	nor;

	AAA_get_point_normal( pos, nor );
	BV.pos_world = pos;

	BV.alpha = gl_Color.a;

	// Eye-coordinate position of vertex, needed in various calculations
	pos = gl_ModelViewMatrix * pos;
	BV.pos_ec = pos;

	// Do fixed functionality vertex transform
	gl_Position = gl_ProjectionMatrix * pos ;
	//aaa_cam.view_projection * vec4( position, 1.0 );

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
	BV.tex_coor[0] = ftexgen_0_vec4( vec4(in_texcoord,0,1), BV.pos_world, nor, pos );

	//for compatibility with existing shader
	gl_TexCoord[0].st = BV.tex_coor[0].st;

	//gl_PointSize += clamp( -(pos.z - aaa_vu_float[0] ) * aaa_vu_float[1], aaa_vu_float[2], aaa_vu_float[3] );
	//gl_PointSize = 8.;
}

