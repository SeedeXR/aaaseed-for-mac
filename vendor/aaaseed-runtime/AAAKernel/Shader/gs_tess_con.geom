
//	layout( triangles ) in;
//layout( triangles, invocations = 2 ) in;
layout( triangles ) in;
layout( triangle_strip, max_vertices = 8 ) out;


in VS_out
{
	TEX_COOR_VEC	tex_coor[TEX_UNIT_NB];
	vec4			color;
	vec3			normal;
	float			fall_off;
	float			z_to_eye;
} vs_in[3];

out VS_out
{
	TEX_COOR_VEC	tex_coor[TEX_UNIT_NB];
	vec4			color;
	vec3			normal;
	float			fall_off;
	float			z_to_eye;
} gs_out;

vec4			position[6];
TEX_COOR_VEC	tex_coor[TEX_UNIT_NB*6];
vec4			color[6];
vec3			normal[6];
float			fall_off[6];
float			z_to_eye[6];


void emit_vertex( in int i )
{
	gl_Position			= position[i];
	gs_out.tex_coor[0]	= tex_coor[i];
	for( int unit = 1; unit<TEX_UNIT_NB; ++unit )
	{
		if( aaa_tex_dim[unit] > 0 )	//	dim is negative when unit unused, dim 0 don't need mapping
		{
			gs_out.tex_coor[unit] = tex_coor[ unit*6 + i];
		}
	}
	gs_out.color		= color[i];
	gs_out.normal		= normal[i];
	gs_out.fall_off		= fall_off[i];
	gs_out.z_to_eye		= z_to_eye[i];
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

	normal[0]	= vs_in[0].normal;
	normal[2]	= vs_in[1].normal;
	normal[4]	= vs_in[2].normal;

	position[0]	= gl_in[0].gl_Position;
	position[2]	= gl_in[1].gl_Position;
	position[4]	= gl_in[2].gl_Position;
#if	1
	if( dot( normal[0], normal[2] ) < aaa_gu_float[0]
		|| dot( normal[2], normal[4] ) < aaa_gu_float[0]
//		|| dot( normal[4], normal[0] ) < aaa_gu_float[0]
		|| distance( position[0], position[2] ) > aaa_gu_float[1]
		|| distance( position[0], position[4]	 ) > aaa_gu_float[1]
		|| position[0].z  < aaa_gu_float[3]
		)
	{
		//	normal	0 2 4 done already
		normal[1]	= (normal[0] + normal[2]) * .5;
		normal[3]	= (normal[2] + normal[4]) * .5;
		normal[5]	= (normal[4] + normal[0]) * .5;

		//	position 0 2 4 done already
		position[1]	= (position[0] + position[2]) * .5;
		position[3]	= (position[2] + position[4]) * .5;
		position[5]	= (position[4] + position[0]) * .5;

		tex_coor[0]	= vs_in[0].tex_coor[0];
		tex_coor[2]	= vs_in[1].tex_coor[0];
		tex_coor[4]	= vs_in[2].tex_coor[0];
		tex_coor[1]	= mix( tex_coor[0], tex_coor[2], .5 );
		tex_coor[3]	= mix( tex_coor[2], tex_coor[4], .5 );
		tex_coor[5]	= mix( tex_coor[4], tex_coor[0], .5 );

		for( int unit = 1; unit<TEX_UNIT_NB; ++unit )
		{
			if( aaa_tex_dim[unit] > 0 )	//	dim is negative when unit unused, dim 0 don't need mapping
			{
				int offset = unit * 6;
				tex_coor[offset+0]	= vs_in[0].tex_coor[unit];
				tex_coor[offset+2]	= vs_in[1].tex_coor[unit];
				tex_coor[offset+4]	= vs_in[2].tex_coor[unit];
				tex_coor[offset+1]	= mix( tex_coor[offset+0], tex_coor[offset+2], .5 );
				tex_coor[offset+3]	= mix( tex_coor[offset+2], tex_coor[offset+4], .5 );
				tex_coor[offset+5]	= mix( tex_coor[offset+4], tex_coor[offset+0], .5 );
			}
		}
		color[0]	= vs_in[0].color;
		color[2]	= vs_in[1].color;
		color[4]	= vs_in[2].color;
		color[1]	= (color[0] + color[2]) * .5;
		color[3]	= (color[2] + color[4]) * .5;
		color[5]	= (color[4] + color[0]) * .5;

		fall_off[0]	= vs_in[0].fall_off;
		fall_off[2]	= vs_in[1].fall_off;
		fall_off[4]	= vs_in[2].fall_off;
		fall_off[1]	= (fall_off[0] + fall_off[2]) * .5;
		fall_off[3]	= (fall_off[2] + fall_off[4]) * .5;
		fall_off[5]	= (fall_off[4] + fall_off[0]) * .5;

		z_to_eye[0]	= vs_in[0].z_to_eye;
		z_to_eye[2]	= vs_in[1].z_to_eye;
		z_to_eye[4]	= vs_in[2].z_to_eye;
		z_to_eye[1]	= (z_to_eye[0] + z_to_eye[2]) * .5;
		z_to_eye[3]	= (z_to_eye[2] + z_to_eye[4]) * .5;
		z_to_eye[5]	= (z_to_eye[4] + z_to_eye[0]) * .5;
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
	}
	else
	{
		for( int i = 0; i < gl_VerticesIn; ++i )
		{
 			gl_Position			=	position[i<<1];
			for( int unit = 0; unit<TEX_UNIT_NB; ++unit )
			{
				if( aaa_tex_dim[unit] > 0 )	//	dim is negative when unit unused, dim 0 don't need mapping
				{
					gs_out.tex_coor[unit] = vs_in[i].tex_coor[unit];
				}
			}
			//gs_out.tex_coor		=	vs_in[i].tex_coor;
			gs_out.color		=	vs_in[i].color;
			gs_out.normal		=	normal[i<<1];
			gs_out.fall_off		=	vs_in[i].fall_off;
			gs_out.z_to_eye		=	vs_in[i].z_to_eye;
			//gs_out		=	vs_in[i];
			EmitVertex();
		}
		EndPrimitive();
	}
#endif
#if	0
	if( aaa_gu_float[2] > 0. )
	{
		for( int i = 0; i < gl_VerticesIn; ++i )
		{
			vec4 pos = position[i<<1];
			pos.xyz 	+=	normal[i<<1].xyz * aaa_gu_float[2];
			gl_Position		=	pos;
			gs_out.tex_coor	=	vs_in[i].tex_coor;
			gs_out.color	=	vs_in[i].color;
			EmitVertex();
		}
		EndPrimitive();
	}
#endif
}
