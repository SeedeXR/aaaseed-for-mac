//#version 330 compatibility
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


layout( points ) in;
layout( triangle_strip, max_vertices=16 ) out;


//in	vec4	tex_coor_vs[3];
//in	vec3	normal_vs[3];
//out	vec4	tex_coor_gs;

in VS_out
{
	vec4 pos_world;
	vec4 color;
	vec3 normal;
} vs_in[];

out GS_out
{
	vec2 uv;
	vec4 color;
//	vec3 normal;
} gs_out;

/*
layout (std140) uniform Matrices {
	mat4	gl_ModelViewProjectionMatrix;
    mat4	projModelViewMatrix;
    mat3	normalMatrix;
};
*/

void emit_point_2_left( in vec4 a, in vec4 b )
{
	gl_Position = a;	gs_out.uv = vec2( 0, 1 );	EmitVertex();
	gl_Position = b;	gs_out.uv = vec2( 0, 0 );	EmitVertex();
}
void emit_point_2_right( in vec4 a, in vec4 b )
{
	gl_Position = a;	gs_out.uv = vec2( 1, 1 );	EmitVertex();
	gl_Position = b;	gs_out.uv = vec2( 1, 0 );	EmitVertex();
}
void emit_point_2( in vec4 a, in vec4 b )
{
	gl_Position = a;	EmitVertex();
	gl_Position = b;	EmitVertex();
}

void emit_triangle( in vec4 a, in vec4 b, in vec4 c )
{
	gl_Position = a;	EmitVertex();
	gl_Position = b;	EmitVertex();
	gl_Position = c;	EmitVertex();

}

vec3 hsv_to_rgb( float h, float s, float v )
{
	float c = v * s;
	h = mod((h * 6.0), 6.0);
	float x = c * (1.0 - abs(mod(h, 2.0) - 1.0));
	vec3 color;

	if		( h < 1.0 )		color = vec3(c, x, 0.0);
	else if ( h < 2.0 )		color = vec3(x, c, 0.0);
	else if ( h < 3.0 ) 	color = vec3(0.0, c, x);
	else if ( h < 4.0 ) 	color = vec3(0.0, x, c);
	else if ( h < 5.0 ) 	color = vec3(x, 0.0, c);
	else 					color = vec3(c, 0.0, x);

	color += v - c;

	return color;
}

float dist_vol( vec3 cen, vec3 pos )
{
//	vec3 a = cen-pos;
//	float f = 1 - dot(a, a);
//	float f = clamp( 1 - dot(a, a), 0, 1 );
	float f = clamp( 1 - distance(cen.xyz, pos.xyz), 0, 1 );
	//f = f*f;
	return f + .2;
}

void main()
{
	vec4 vs_col = vec4( vs_in[0].normal.xyz, 1 );
	float size = vs_in[0].color.w; // * (.2 + color.r);
	//size *= (2-abs( vs_col.x )) * (2-abs( vs_col.y )) *.25 ;
	//size *= pow( abs(vs_col.z), .99 );
	size *= abs(vs_col.z);	//smaller when away
	if( size <= 0.)	return;

	vec3 	wp = vs_in[0].pos_world.xyz;
	mat4	mvp =  gl_ModelViewProjectionMatrix;
	vec4	a =	mvp * vec4( wp, 1 );
	if( a.z < 0.001 )	return;	//close cliping

	// color is white or shaded
	vec4 color = mix( vec4(1), vec4(size,size,size,1), aaa_gu_vec4[0].x );
	// color is rainbow
	if( aaa_gu_vec4[0].y > 0. )
	{	// ARC
/*
#define Z_LIGHT -0.5
#define Y_LIGHT 0.
		float fr = dist_vol( vec3(	0,		Y_LIGHT,	Z_LIGHT		), wp );
		float fg = dist_vol( vec3(	.5,		Y_LIGHT,	Z_LIGHT+.5	), wp );
		float fb = dist_vol( vec3(	-.5,	Y_LIGHT,	Z_LIGHT+.5	), wp );
		color.xyz = mix( color.xyz, vec3( fr, fg, fb ), aaa_gu_vec4[0].y );
*/
		color.xyz *= mix( vec3(1,1,1), mix( vs_in[0].color.xyz, vs_in[0].normal.xyz, aaa_gu_vec4[0].z), aaa_gu_vec4[0].y );
		//

		//hsv_to_rgb( wp.x, 1., 1. );
	}
	// sound appear in orange
	if( aaa_gu_vec4[0].w > 0. )
	{
		float d = distance( vec2( 0, 0 ), a.xy * vec2( 1, .5 ) ) * 1.8 + .25;
		//float dz = -distance( vec3( 0, 0, .5 ), wp );
		//d = mix( pow(d,.3), dz, aaa_gu_vec4[0].w );

		d = pow(d,.3);
		d = sin( d * 60 + aaa_gu_float[2] * 360. ) * .5 + .5;
		d = clamp( d*3-1.7, 0,  1 );
		color.xyz = mix( color.xyz, mix( color.xyz, vec3(1,.5,0), d ), aaa_gu_vec4[0].w );
	}

	// vec3 face_normal = normalize (
	// 				cross( gl_in[2].gl_Position.xyz - gl_in[0].gl_Position.xyz,
	// 						gl_in[1].gl_Position.xyz - gl_in[0].gl_Position.xyz)
	// 						);

	//c	=	a;

	size *= min( aaa_gu_float[0] *.01, .25 );

	float r = 1 - aaa_gu_float[1];

	vec4 fix_col = color;

	//direction
	vec3 vx = mvp[0].xyz * size;
	vec3 vy = mvp[1].xyz * size;
	// corner
	a.xyz += ( -vx + vy ) * .5;

	if( aaa_gu_int[0] <= 2 )
	{	// face z
		vec4 b = a;
		b.xyz -= vy;
		gs_out.color = mix( color, fix_col, r );
		emit_point_2_left( a, b );

		a.xyz += vx;
		b.xyz += vx;
		emit_point_2_right( a, b );
		EndPrimitive();

		if( aaa_gu_int[0] >=1 )
		{	// face x
			vec3 vz = mvp[2].xyz * size;

			a.xyz +=  ( vz -vx ) * .5;

			b.xyz = a.xyz - vy;
			emit_point_2_left( a, b );

			a.xyz -= vz;
			b.xyz -= vz;
			emit_point_2_right( a, b );
			EndPrimitive();

			if( aaa_gu_int[0] == 2 )
			{	// face y
				a.xyz += (vx - vy) * .5;

				b.xyz = a.xyz - vx;
				emit_point_2_left( a, b );

				a.xyz += vz;
				b.xyz += vz;
				emit_point_2_right( a, b );
				EndPrimitive();
			}
		}
	}
	else
	{	// draw_cube
		vec3 vz = mvp[2].xyz * size;

		vec4 b;
		b.xyz = a.xyz - vy;
		b.w = a.w;
		gs_out.color = color * vec4( 1, r, r, 1. );
		emit_point_2_left( a, b );

		a.xyz += vz;
		b.xyz += vz;
		emit_point_2_right( a, b );
		EndPrimitive();

		gs_out.color = color * vec4( r, 1, r, 1. );
		emit_point_2_left( a, b );
		a.xyz += vx;
		b.xyz += vx;
		emit_point_2_right( a, b );
		EndPrimitive();

		gs_out.color = color * vec4( r, r, 1, 1. );
		emit_point_2_left( a, b );
		a.xyz -= vz;
		b.xyz -= vz;
		emit_point_2_right( a, b );
		EndPrimitive();
	}
}
