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



void derviche_06b_v1( in float ph, out float x, out float y, out float z )
{
	float v0 = -15. + 30. * sin( 0.1*time );
		z	= cos(3*v0+log(0.04+cos(4*ph)*cos(4*ph)));
		x	= cos(7*ph)*cos(8*ph)*cos(8*ph);
		y	= sin(7*ph)*cos(8*ph)*cos(8*ph);

	//float f = 1;
	//x = f * x/z;
	//y = f * y/z;
	//z = 0;
}

void derviche_06b_v2( in float ph, out float x, out float y, out float z )
{
	float v0 = -15. + 30. * sin( 0.1*time );
		z	= sin(16*x)*cos(5*v0+log(0.15+cos(4*ph)*cos(4*ph)));
		x	= cos(7*ph)*cos(8*ph)*cos(8*ph);
		y	= sin(7*ph)*cos(8*ph)*cos(8*ph);
		//z	= 1.5+log(0.05+cos(v0+4*ph)*cos(v0+4*ph));
		//x	= cos(7*ph)*cos(8*ph)*cos(8*ph);
		//y	= sin(7*ph)*cos(8*ph)*cos(8*ph);
	//float f = 1;
	//x = f * x/z;
	//y = f * y/z;
	//z = 0;
}

void derviche_06b_v3( in float ph, out float x, out float y, out float z )
{
	float v0 = -15. + 30. * sin( 0.1*time );
		z	= cos(3*v0+log(0.04+cos(4*ph)*cos(4*ph)));
		x	= cos(7*ph)*cos(8*ph)*cos(8*ph)*z;
		y	= sin(7*ph)*cos(8*ph)*cos(8*ph)*z;
		//z	= cos( v0+log(0.17+cos(0.5*ph)*cos(0.5*ph)) )/cos(8*ph);
		//x	= 3*cos(7*ph)*cos(8*ph)*cos(8*ph)*z;
		//y	= 3*sin(7*ph)*cos(8*ph)*cos(8*ph)*z;
		//float f = 1;
	//x = f * x/z;
	//y = f * y/z;
	//z = 0;
}

void derviche_06b_v4( in float ph, out float x, out float y, out float z )
{
	float v0 = -15. + 30. * sin( 0.1*time );
		z	= 0.3*(1.5+log(0.05+cos(v0+4*ph)*cos(v0+4*ph)));
		x	= 0.3*cos(7*ph)*cos(8*ph)*cos(8*ph);
		y	= 0.3*sin(7*ph)*cos(8*ph)*cos(8*ph);
	//float f = 1;
	//x = f * x/z;
	//y = f * y/z;
	//z = 0;
}

void derviche_06b_v5( in float ph, out float x, out float y, out float z )
{
	float v0 = -15. + 30. * sin( 0.1*time );
		z	= cos(v0+log(0.05+cos(8*ph)*cos(8*ph)));
		x	= cos(7*ph)*cos(8*ph)*cos(8*ph);
		y	= sin(7*ph)*cos(8*ph)*cos(8*ph);
		// Variation intéressante de V5
		//z	= cos(v0+log(0.05+cos(v0+8*ph)*cos(v0+8*ph)));
		//x	= cos(7*ph)*cos(8*ph)*cos(8*ph);
		//y	= sin(7*ph)*cos(8*ph)*cos(8*ph);//float f = 1;
		// Autre variation intéressante de V5
		//z	= cos(v0+log(0.05+cos(8*ph)*cos(8*ph)));
		//x	= cos(70*ph)*cos(8*ph)*cos(8*ph);
		//y	= sin(70*ph)*cos(8*ph)*cos(8*ph);
	//x = f * x/z;
	//y = f * y/z;
	//z = 0;
}

void derviche_06b_v6( in float ph, out float x, out float y, out float z )
{
	float v0 = -15. + 30. * sin( 0.1*time );
		z	= cos(16*x)*cos(5*v0+log(0.3+cos(4*ph)*cos(4*ph)));
		x	= cos(700*ph)*cos(8*ph)*cos(8*ph)*z;
		y	= sin(700*ph)*cos(8*ph)*cos(8*ph)*z;

		//z	= cos(16*x)*cos(v0+log(0.3+cos(4*ph)*cos(4*ph)));
		//x	= cos(7000*ph)*cos(8*ph)*cos(8*ph)*z;
		//y	= sin(700*ph)*cos(8*ph)*cos(8*ph)*z;

		//z	= cos(16*x)*cos(v0+log(0.3+cos(4*ph)*cos(4*ph)));
		//x	= cos(70*ph)*cos(8*ph)*cos(8*ph)*z;
		//y	= sin(70*ph)*cos(8*ph)*cos(8*ph)*z;

		//x = f * x/z;
		//y = f * y/z;
		//z = 0;
}

void derviche_06b_v7( in float ph, out float x, out float y, out float z )
{
	float v0 = -15. + 30. * sin( 0.1*time );

		z	= cos(v0+log(0.3+cos(4*ph)*cos(4*ph)))/cos(8*x);
		x	= cos(7*ph)*cos(8*ph)*cos(8*ph)*z;
		y	= sin(7*ph)*cos(8*ph)*cos(8*ph)*z;

		//PUR DERVICHE
		//z	= cos(v0+log(0.3+cos(4*ph)*cos(4*ph)))/cos(8*x);
		//x	= cos(7*ph)*cos(8*ph)*cos(8*ph)*z;
		//y	= sin(7*ph)*cos(8*ph)*cos(8*ph)*z;

		//x = f * x/z;
		//y = f * y/z;
		//z = 0;
}

void derviche_06b_v8( in float ph, out float x, out float y, out float z )
{
	float v0 = -15. + 30. * sin( 0.1*time );
		z	= sin(v0+exp(0.17+cos(2*ph)*cos(2*ph)))/cos(8*ph);
		x	= cos(8*ph)*cos(8*ph);
		y	= cos(8*ph)*cos(8*ph)*cos(8*ph)*sin(8*ph);

		//Variation de v8
		//z	= sin(v0+exp(0.17+cos(2*ph)*cos(2*ph)))/cos(8*ph);
		//x	= cos(7*ph)*cos(8*ph)*cos(8*ph)*z;
		//y	= sin(7*ph)*cos(8*ph)*cos(8*ph)*z;

		//x = f * x/z;
		//y = f * y/z;
		//z = 0;
}

void derviche_06b_v9( in float ph, out float x, out float y, out float z )
{
	float v0 = -15. + 30. * sin( 0.1*time );
		z	= sin(v0+exp(0.17+cos(4*ph)*cos(4*ph)))/cos(4*ph);
		x	= cos(7*ph)*cos(8*ph)*cos(8*ph)*z;
		y	= sin(7*ph)*cos(8*ph)*cos(8*ph)*z;

		//x = f * x/z;
		//y = f * y/z;
		//z = 0;
}


void derviche_06b_v10( in float ph, out float x, out float y, out float z )
{
	float v0 = -15. + 30. * sin( 0.1*time );
		z	= sin(v0+exp(0.17+cos(8*ph)*cos(8*ph)))/cos(8*ph);
		x	= cos(7*ph)*cos(8*ph)*cos(8*ph)/z;
		y	= sin(7*ph)*cos(8*ph)*cos(8*ph)/z;

		//x = f * x/z;
		//y = f * y/z;
		//z = 0;
}

void derviche_06b_v11( in float ph, out float x, out float y, out float z )
{
	float v0 = -15. + 30. * sin( 0.1*time );
		z	= tan(4*ph)*exp(0.17+cos(8*ph+v0));
		x	= cos(7*ph)*cos(8*ph)*sin(v0+exp(0.17+cos(4*ph)*cos(4*ph)));
		y	= sin(7*ph)*cos(8*ph)*sin(v0+exp(0.17+cos(4*ph)*cos(4*ph)));

		//intéressante variation de v11
		//z	= 2*tan(4*ph)*exp(0.17+cos(8*ph+v0));
		//x	= 2*cos(7*ph)*cos(8*ph)*cos(8*ph)*z;
		//y	= 2*sin(7*ph)*cos(8*ph)*cos(8*ph)*z;

		//x = f * x/z;
		//y = f * y/z;
		//z = 0;
}

void derviche_06b_v12( in float ph, out float x, out float y, out float z )
{
	float v0 = -15. + 30. * sin( 0.1*time );
		z	= tan(v0+log(0.17+cos(4*ph)*cos(4*ph)))/cos(4*ph);
		x	= tan(4*ph)*cos(7*ph)*cos(8*ph)*cos(8*ph)*z;
		y	= tan(4*ph)*sin(7*ph)*cos(8*ph)*cos(8*ph)*z;

		//x = f * x/z;
		//y = f * y/z;
		//z = 0;
}

void derviche_06b_v13( in float ph, out float x, out float y, out float z )
{
	float v0 = -15. + 30. * sin( 0.1*time );
		z	= sin(v0+exp(0.17+cos(4*ph)*cos(4*ph)))/cos(8*ph);
		x	= cos(7/ph)/z;
		y	= sin(7/ph)/z;

		//x = f * x/z;
		//y = f * y/z;
		//z = 0;
}

void derviche_06b_v14( in float ph, out float x, out float y, out float z )
{
	float v0 = -15. + 30. * sin( 0.1*time );
		z	= sin(v0+exp(0.17+cos(4*ph)*cos(4*ph)))/cos(8*ph);
		x	= cos(7/ph)/z;
		y	= sin(7/ph)/z;

		//x = f * x/z;
		//y = f * y/z;
		//z = 0;
}

void derviche_06b_v15( in float ph, out float x, out float y, out float z )
{
	float v0 = -15. + 30. * sin( 0.1*time );
		z	= sin(v0+exp(0.17+cos(ph)*cos(ph)))/cos(ph*ph*ph*ph*ph*ph*ph);
		x	= cos(7/ph/ph)/z;
		y	= sin(7/ph/ph)/z;

		//x = f * x/z;
		//y = f * y/z;
		//z = 0;
}

void derviche_06b_v16( in float ph, out float x, out float y, out float z )
{
	float v0 = -15. + 30. * sin( 0.1*time );

		z	= cos(v0+log(0.3+cos(4*ph)*cos(4*ph)))/cos(8*x);
		x	= cos(7*ph)*cos(8*ph)*cos(8*ph)*z;
		y	= sin(7*ph)*cos(8*ph)*cos(8*ph)*z;
}

void main(void)
{
	vec4	pos;
	vec4	color;

	float ph	= (gl_VertexID * PH_VERTEX_FACTOR + gl_InstanceID ) * PH_INSTANCE_FACTOR;
	ph = min( max( ph, PH_MIN ), PH_MAX ) * PI2;
	float x,y,z;

////////////////////////////////////////////////////////////////////////////////////////////////
//	Begin HUGO

	if( aaa_vu_int[0] <= 8 )
		if( aaa_vu_int[0] <= 4 )
			if( aaa_vu_int[0] <= 2 )	// 1-2
				if( aaa_vu_int[0] == 1 )	derviche_06b_v1( ph, x, y, z );
				else						derviche_06b_v2( ph, x, y, z );
			else						// 3-4
				if( aaa_vu_int[0] == 3 )	derviche_06b_v3( ph, x, y, z );
				else						derviche_06b_v4( ph, x, y, z );
		else
			if( aaa_vu_int[0] <= 6 )	// 5-6
				if( aaa_vu_int[0] == 5 )	derviche_06b_v5( ph, x, y, z );
				else						derviche_06b_v6( ph, x, y, z );
			else						// 7-8
				if( aaa_vu_int[0] == 7 )	derviche_06b_v7( ph, x, y, z );
				else						derviche_06b_v8( ph, x, y, z );
	else
		if( aaa_vu_int[0] <= 12 )
			if( aaa_vu_int[0] <= 10 )	// 9-10
				if( aaa_vu_int[0] == 9 )	derviche_06b_v9( ph, x, y, z );
				else						derviche_06b_v10( ph, x, y, z );
			else						// 11-12
				if( aaa_vu_int[0] == 11 )	derviche_06b_v11( ph, x, y, z );
				else						derviche_06b_v12( ph, x, y, z );
		else
			if( aaa_vu_int[0] <= 14 )	// 13-14
				if( aaa_vu_int[0] == 13 )	derviche_06b_v13( ph, x, y, z );
				else						derviche_06b_v14( ph, x, y, z );
			else						// 15-16
				if( aaa_vu_int[0] == 15 )	derviche_06b_v15( ph, x, y, z );
				else						derviche_06b_v16( ph, x, y, z );

	color = mix( color1, color2, 30*v3*(x*x + y*y ) );

//	End derviche
	pos = vec4( y, z, x, 1. );
//	begin tranformation

//z = z+5*sin(time*10.);
//y = y+5*sin(time*100);
	//pos.y = 2+pos.y + 4*sin(time*50);
	//pos.z = 0+pos.z +1*sin(time*5);
	//pos.x = -15+pos.x + 9*sin(time*50);
	//rotate( pos.xy, time*10. );
	//rotate( pos.yz, time*100. );
	//rotate( pos.xz, time*100. );
//smoothstep( v3, v4, x*x + y*y );

//	End HUGO
////////////////////////////////////////////////////////////////////////////////////////////////
	float f = aaa_vu_vec4[7].w;
	if( f!=0. )
	{
		pos.xy = pos.xy * f / (pos.z+aaa_vu_vec4[7].z) ;
		pos.z = 0.;
	}
	vs_out.pos_world = pos;
	// Eye-coordinate position of vertex, needed in various calculations
	pos = gl_ModelViewMatrix * pos;
	vs_out.pos_ec = pos;

	vs_out.color = clamp_01( color * gl_Color );
	// Do fixed functionality vertex transform
	gl_Position = gl_ProjectionMatrix * pos ;
}