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

#define time  aaa_vu_float[0]
#define v1 aaa_vu_float[1]
#define v2 aaa_vu_float[2]
#define v3 aaa_vu_float[3]
#define v4 aaa_vu_float[4]
#define v5 aaa_vu_float[5]
#define v6 aaa_vu_float[6]
#define v7 aaa_vu_float[7]


#define PH_VERTEX_FACTOR	( 1. / 1024 )
#define PH_INSTANCE_FACTOR	( 1. / 1024 )

void main(void)
{
	vec4	pos;
	pos.w = 1.;
	//pos.x += gl_InstanceID / 1024.;

	float ph	= (gl_VertexID * PH_VERTEX_FACTOR + gl_InstanceID ) * PH_INSTANCE_FACTOR * PI2;

	float v0 = -15. + 30. * sin( time );
//	float tmp	= cos( 4.0*ph );
//	float y		= cos( 160.0*V1*ph ) * cos( V0+log(V2+tmp*tmp) );
//	pos.y		= y;
//	tmp			= cos(8.0*ph);
//	pos.x		= cos(7*ph)*tmp*tmp*y;
//	pos.z		= sin(7*ph)*tmp*tmp*y;
	if( aaa_vu_int[0] < 0 )
	{
		float phn = ph;
		pos.x		= cos( phn * 100. ) * ph;
		pos.y		= cos( ph * 1000. ) * cos( ph * 30.);
		pos.z		= sin( phn * 100. ) * ph;

		vs_out.color = mix( aaa_vu_vec4[0], aaa_vu_vec4[1], (pos.x*pos.x + pos.z*pos.z) * v3 ) * gl_Color;
	}
	else
	{
		//	local tmp = cos(4.0*ph)
		//	y	= cos(160.0*v1*ph)*cos( v0+log(v2+tmp*tmp) )
		//	tmp = cos(8.0*ph)
		//	x	= cos(7*ph)*tmp*tmp*y
		//	z	= sin(7*ph)*tmp*tmp*y
		float tmp = cos(4.0*ph);
		pos.y	= cos(160.0*v1*ph) * cos( v0+log(v2+tmp*tmp) );
		tmp = cos(32.0*ph);
		tmp = tmp*tmp*pos.y;
		ph *= 7.;
		pos.x	= cos(ph)*tmp;
		pos.z	= sin(ph)*tmp;

		vs_out.color = mix( aaa_vu_vec4[0], aaa_vu_vec4[1], (pos.x*pos.x + pos.z*pos.z) * v3 );
	}
	vs_out.pos_world = pos;

	// Eye-coordinate position of vertex, needed in various calculations
	pos = gl_ModelViewMatrix * pos;
	vs_out.pos_ec = pos;

	// Do fixed functionality vertex transform
	gl_Position = gl_ProjectionMatrix * pos ;


}
