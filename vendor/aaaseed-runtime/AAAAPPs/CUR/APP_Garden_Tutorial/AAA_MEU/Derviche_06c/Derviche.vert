//#version 330 compatibility
////#extension GL_ARB_shading_language_include : enable
//#extension GL_EXT_gpu_shader4 : enable


uniform float	iGlobalTime;

out VS_out
{
	vec4	pos_world;
	vec4	pos_ec;
	vec4	color;
} vs_out;

#define time aaa_vu_float[0]
#define v1 aaa_vu_float[1]
#define v2 aaa_vu_float[2]
#define v3 aaa_vu_float[3]
#define v4 aaa_vu_float[4]
#define v5 aaa_vu_float[5]
#define v6 aaa_vu_float[6]
#define v7 aaa_vu_float[7]

#define PH_VERTEX_FACTOR	aaa_vu_vec4[0].x
#define PH_INSTANCE_FACTOR	aaa_vu_vec4[0].y
#define PH_MIN				aaa_vu_vec4[0].z
#define PH_MAX				aaa_vu_vec4[0].w

#define color1 aaa_vu_vec4[1]
#define color2 aaa_vu_vec4[2]
#define color3 aaa_vu_vec4[3]
#define color4 aaa_vu_vec4[4]

void main(void)
{
	vec4	pos;
	vec4	color;

	float ph	= (gl_VertexID * PH_VERTEX_FACTOR + gl_InstanceID ) * PH_INSTANCE_FACTOR;
	ph = min( max( ph, PH_MIN ), PH_MAX ) * PI2;
	float x,y,z;

////////////////////////////////////////////////////////////////////////////////////////////////
//	Begin HUGO

	float v0 = -15. + 30. *sin( time );
	if( aaa_vu_int[0] == 1 )
	{
		z	= sin(0.08*v0+exp(0.17+cos(8*ph)*cos(80*ph)))/cos(80*ph);
		x	= 0.5*cos(7/ph)*cos(8000*ph)/cos(z);
		y	= 0.5*sin(7/ph)*cos(800*ph)/tan(z);
	}
	else if( aaa_vu_int[0] == 2 )
	{
		z	= sin(v0+exp(0.17+cos(ph)*cos(ph)))/cos(8*ph*ph*ph*ph*ph*ph*ph);
		x	= cos(7/ph/ph)/z;
		y	= sin(7/ph/ph)/z;
	}
	else if( aaa_vu_int[0] == 3 )
	{
		z	= tan(4*ph)*exp(0.17+cos(8*ph+v0));
		x	= cos(7*ph)*cos(8*ph)*sin(v0+exp(0.17+cos(4*ph)*cos(4*ph)));
		y	= sin(7*ph)*cos(8*ph)*sin(v0+exp(0.17+cos(4*ph)*cos(4*ph)));
	}
	else
	{

		z	= sin(v0+exp(0.17+cos(8.0*ph)*cos(8.0*ph)))/tan(160*ph);
		x	= cos(7*ph)*cos(8*ph)*cos(8*ph)/cos(v0+exp(0.17+cos(800*ph)*cos(800*ph)))/cos(80*ph);
		y	= sin(7*ph)*cos(8*ph)*cos(8*ph)/cos(v0+exp(0.17+cos(800*ph)*cos(800*ph)))/cos(80*ph);
	}
	color = mix( color1, color2, 30*v3*(x*x + y*y ) );

// smoothstep( v3, v4, x*x + y*y );

//	End HUGO
////////////////////////////////////////////////////////////////////////////////////////////////
	pos = vec4( y, z, x, 1. );

	pos.w = 1.;
	vs_out.pos_world = pos;
	// Eye-coordinate position of vertex, needed in various calculations
	pos = gl_ModelViewMatrix * pos;
	vs_out.pos_ec = pos;

	vs_out.color = clamp_01( color * gl_Color );
	// Do fixed functionality vertex transform
	gl_Position = gl_ProjectionMatrix * pos ;
}