#include	"catmull_rom.glsl"

void	deform_catmull( inout vec3 pos, inout vec3 nor )
{
	//	deformation, maa's cooking
	float x = pos.x;

	vec3 a;
	vec3 o;
	float inter;
	//	DEFORMATION sur 6 pts
#if 0
	if( x < -1. )
	{
		inter = x + 2.;
		o = mix( aaa_vu_vec4[0].xyz, aaa_vu_vec4[1].xyz, inter );
		a = aaa_vu_vec4[1].xyz - aaa_vu_vec4[0].xyz;
	}
	else if( x < 0. )
	{
		inter = x + 1.;
		o = mix( aaa_vu_vec4[1].xyz, aaa_vu_vec4[2].xyz, inter );
		a = aaa_vu_vec4[2].xyz - aaa_vu_vec4[1].xyz;
	}
	else if( x < 1. )
	{
		inter = x;
		o = mix( aaa_vu_vec4[2].xyz, aaa_vu_vec4[3].xyz, inter );
		a = aaa_vu_vec4[3].xyz - aaa_vu_vec4[2].xyz;
	}
	else if( x < 2. )
	{
		inter = x - 1.;
		o = mix( aaa_vu_vec4[3].xyz, aaa_vu_vec4[4].xyz, inter );
		a = aaa_vu_vec4[4].xyz - aaa_vu_vec4[3].xyz;
	}
	else if( x < 3. )
	{
		inter = x - 2.;
		o = aaa_vu_vec4[4].xyz *(1-inter) + aaa_vu_vec4[5].xyz * inter;
		a = aaa_vu_vec4[5].xyz - aaa_vu_vec4[4].xyz;
	}
	a = normalize( a );
#else
	int	index;
	float dx = aaa_vu_float[0];
	if( x < 0 )
	{
		o = cvCatmullRom_both(	a, 0, (dx - aaa_vu_float[2]) * aaa_vu_float[1] );
		a = normalize( a );
		o += a * x ;
	}
	else
	{
		x += dx - aaa_vu_float[2];
		if( x < dx )		index = 0;
		else if( x < 2*dx )	index = 1;
		else				index = 2;	// if (x < 1.5)

		inter = (x - index*dx) * aaa_vu_float[1];
		//	if( index >= 0 )
		o = cvCatmullRom_both(	a, index, inter );
		a = normalize( a );
	}
#endif
	//	BUILD COOR SYSTEM VECTOR a b c
	vec3 b = normalize( cross( a, vec3(0.,1.,0.) ) );
	vec3 c = cross( b, a );
	//	mat3 basis = mat3( a, b, c );

	//	COMPUTE DEFORMED POINT
		// Transform the normal vector and positions:
		//	vNormal = NormalMatrix * (basis * Normal);
		//	pos.xyz += a * pos.x ;
	pos = o + b * pos.z + c * pos.y ;
	//	COMPUTE DEFORMED NORMAL
	nor = a * nor.x + b * nor.z + c * nor.y ;
}

