//
//	DEFORM
//

#define CATMULL 1	//	if 0 we do linear on the 2 central vect ( b and c )

vec3	cvCatmullRom( const in vec3 a, const in vec3 b, const in vec3 c, const in vec3 d, const in float t )
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
vec3	cvCatmullRom_derivative( const in vec3 a, const in vec3 b, const in vec3 c, const in vec3 d, const in float t )
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
const vec3	f1d = vec3( 	-1.,	4.,		-3.	) * .5;
const vec3	f2d = vec3( 	 0.,	-10.,	9.	) * .5;
const vec3	f3d = vec3( 	 1.,	8.,		-9.	) * .5;
const vec3	f4d = vec3( 	 0.,	-2.,	3.	) * .5;

const vec4	f1f = vec4(		0.,		-1.,	2.,		-1.	) * .5;
const vec4	f2f = vec4(		2.,		0.,		-5.,	3.	) * .5;
const vec4	f3f = vec4(		0.,		1.,		4.,		-3.	) * .5;
const vec4	f4f = vec4(		0.,		0.,		-1.,	1.	) * .5;

vec3	cvCatmullRom_both( out vec3 tgn, const in int i, const in float t )
{
#if	CATMULL
	const float	t2 = t * t;
	vec4	vt = vec4( 1, t, t2, t*t2 );
	vec3	pos;
	vec3	v;
	v = aaa_vu_vec4[i].xyz;
	tgn =	dot( vt.xyz,	f1d ) 	* v;
	pos =	dot( vt,		f1f )	* v;
	v = aaa_vu_vec4[i+1].xyz;
	tgn +=	dot( vt.xyz,	f2d )	* v;
	pos +=	dot( vt,		f2f )	* v;
	v = aaa_vu_vec4[i+2].xyz;
	tgn +=	dot( vt.xyz,	f3d )	* v;
	pos +=	dot( vt,		f3f )	* v;
	v = aaa_vu_vec4[i+3].xyz;
	tgn +=	dot( vt.xyz,	f4d )	* v;
	pos +=	dot( vt,		f4f )	* v;
	return pos;
#else
	tgn = aaa_vu_vec4[i+2].xyz - aaa_vu_vec4[i+1].xyz;
	return	(1.-t)*aaa_vu_vec4[i+1].xyz + t*aaa_vu_vec4[i+2].xyz;
#endif
}
