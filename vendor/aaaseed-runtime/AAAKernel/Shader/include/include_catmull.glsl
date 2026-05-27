
#define CATMULL 1	//	if 0 we do linear on the 2 central vect ( b and c )
#define CATMULL_CONTROL_NB 6	//todo pass it to the shader as variable in some version

vec3	catmull_rom( const in vec3 a, const in vec3 b, const in vec3 c, const in vec3 d, const in float t )
{
#if	CATMULL
	float	t2 = t * t;
	float	t3 = t * t2;
	float	f1, f2, f3 ,f4;
	f1 =    - t + 2. * t2	-      t3;
	f2 = 2.     - 5. * t2	+ 3. * t3;
	f3 =      t + 4. * t2	- 3. * t3;
	f4 =        -      t2	+      t3;
	return	(f1*a + f2*b + f3*c + f4*d) * .5;
#else
	return	(1.-t)*b + t*c;
#endif
}
vec3	catmull_rom_derivative( const in vec3 a, const in vec3 b, const in vec3 c, const in vec3 d, const in float t )
{
#if	CATMULL
	float	t2 = t * t;
	float	f1, f2, f3 ,f4;
	f1 = - 1. +  4. * t - 3. * t2;
	f2 =      - 10. * t + 9. * t2;
	f3 =   1. +  8. * t - 9. * t2;
	f4 =      -  2. * t + 3. * t2;
	return (f1*a + f2*b + f3*c + f4*d) * .5;
#else
	return c - b;
#endif
}

//	but we tried to optimzed this way
const vec4	catmull_rom_f1f = vec4(		 0.,	-1.,	 2.,	-1.	) * .5;
const vec4	catmull_rom_f2f = vec4(		 2.,	 0.,	-5.,	 3.	) * .5;
const vec4	catmull_rom_f3f = vec4(		 0.,	 1.,	 4.,	-3.	) * .5;
const vec4	catmull_rom_f4f = vec4(		 0.,	 0.,	-1.,	 1.	) * .5;

const vec3	catmull_rom_f1d = vec3( 	-1.,	  4.,	-3.	) * .5;
const vec3	catmull_rom_f2d = vec3( 	 0.,	-10.,	 9.	) * .5;
const vec3	catmull_rom_f3d = vec3( 	 1.,	  8.,	-9.	) * .5;
const vec3	catmull_rom_f4d = vec3( 	 0.,	 -2.,	 3.	) * .5;



vec3	catmull_rom_both( out vec3 tgn, const in int i, const in float t )
{
#if	CATMULL
	const float	t2 = t * t;
	vec4	vt = vec4( 1, t, t2, t*t2 );
	vec3	pos;
	vec3	v;
	v = aaa_vu_vec4[i].xyz;
	pos =	dot( vt,		catmull_rom_f1f )	* v;
	tgn =	dot( vt.xyz,	catmull_rom_f1d ) 	* v;
	v = aaa_vu_vec4[i+1].xyz;
	pos +=	dot( vt,		catmull_rom_f2f )	* v;
	tgn +=	dot( vt.xyz,	catmull_rom_f2d )	* v;
	v = aaa_vu_vec4[i+2].xyz;
	pos +=	dot( vt,		catmull_rom_f3f )	* v;
	tgn +=	dot( vt.xyz,	catmull_rom_f3d )	* v;
	v = aaa_vu_vec4[i+3].xyz;
	pos +=	dot( vt,		catmull_rom_f4f )	* v;
	tgn +=	dot( vt.xyz,	catmull_rom_f4d )	* v;
	return pos;
#else
	tgn = aaa_vu_vec4[i+2].xyz - aaa_vu_vec4[i+1].xyz;
	return	(1.-t)*aaa_vu_vec4[i+1].xyz + t*aaa_vu_vec4[i+2].xyz;
#endif
}

// fish are aligned on x axis head toward x<0 tail x>0
//this is the case for SoOuest, Toulon and now Monaco
#define LEN_SEG				aaa_vu_float[0]
#define LEN_SEG_OVER		aaa_vu_float[1]
#define S_INTER				aaa_vu_float[2]
#define SIZE_FACTOR			aaa_vu_float[3]

#if 0
	//	deformation, maa's cooking
	float x = pos.x;

	vec3 a;
	vec3 o;
	float inter;
	//	DEFORMATION sur 6 pts
	int	index;
	if( x < 0 )
	{
		o = cvCatmullRom_both(	a, 0, 1. - S_INTER * LEN_SEG_OVER );
		a = normalize( a );
		o += a * x ;
	}
	else
	{
		x += LEN_SEG - S_INTER;
		if( x < dx )		index = 0;
		else if( x < 2*dx )	index = 1;
		else				index = 2;	// if (x < 1.5)

		inter = x * LEN_SEG_OVER - index;
		//	if( index >= 0 )
		o = cvCatmullRom_both(	a, index, inter );
		a = normalize( a );
	}
#endif


void	build_catmull_space_y( inout vec3 pos, out vec3 o, out vec3 vx, out vec3 vy, out vec3 vz )
{
	//	DEFORMATION sur 6 pts
	if( pos.x < 0 )
	{
		o = catmull_rom_both( vx, 0, 0 );
		vx = normalize( vx );
		o += vx * pos.x;
	}
	else
	{
		float x = pos.x * LEN_SEG_OVER;
#if 0	//to pass an int to control this
		//interpolate the change from straight to curve to avoid artifact
		if ( x < .5 )
		{
			vec3 ob,vxb;
			o = catmull_rom_both( vx, 0, 0 );
			vx = normalize( vx );
			o += vx * x;

			ob = catmull_rom_both( vxb, 0, x );
			vxb = normalize( vxb );

			x = sqrt(x*2.);
			o = mix( o, ob, x );
			vx = mix( vx, vxb, x );
			//vx = normalize( vx );
		}
		else
#endif
		{
			int index = min( CATMULL_CONTROL_NB-4, int( floor(x) ) );
			o = catmull_rom_both( vx, index, x - index );
			vx = normalize( vx );
			//todo add rigid after the end
		}
	}

	//	BUILD COOR SYSTEM VECTOR
	vz = normalize( cross( vx, vec3(0.,1.,0.) ) );
	vy = cross( vz, vx );
}

void	deform_catmull_rom( inout vec3 pos, inout vec3 nor )
{
	vec3 o, vx, vy, vz;
	build_catmull_space_y( pos, o, vx,vy,vz );

	//	mat3 basis = mat3( vx, vy, vz );
	//	COMPUTE DEFORMED POINT
		// Transform the normal vector and positions:
		//	vNormal = NormalMatrix * (basis * Normal);
		//	pos.xyz += a * pos.x ;
	pos =	o						+ vy * pos.y	+ vz * pos.z;
	//	COMPUTE DEFORMED NORMAL
	nor = 			vx * nor.x		+ vy * nor.y	+ vz * nor.z;
}

void	deform_catmull_rom_monaco( inout vec3 pos, inout vec3 nor, inout vec3 tgn, inout vec3 binor  )
{
	vec3 o, vx, vy, vz;
	build_catmull_space_y( pos, o, vx,vy,vz );

	//	mat3 basis = mat3( vx, vy, vz );
	//	COMPUTE DEFORMED POINT
		// Transform the normal vector and positions:
		//	vNormal = NormalMatrix * (basis * Normal);
		//	pos.xyz += a * pos.x ;
	pos =	o						+ vy * pos.y	+ vz * pos.z;
	//	COMPUTE DEFORMED NORMAL
	nor = 			vx * nor.x		+ vy * nor.y	+ vz * nor.z;
	tgn = 			vx * tgn.x		+ vy * tgn.y	+ vz * tgn.z;
	binor = 		vx * binor.x	+ vy * binor.y	+ vz * binor.z;
}
//begin dummy lines to have nice offset (400) in error message













//end should be x * 100 lines
