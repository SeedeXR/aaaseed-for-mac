#include "math/noisturb.h"
#include "math/v.h"
#include "math/rand.h"
#include "noiscurv.cpp"	//	Yes it's a cpp
#include "err.h"


//todo add extern in include and add fns like fbm
// 
//todo	should I do a version with DOUBLE

CONSTEXPR REAL	NOISE_FACTOR = REAL(1);

namespace
{
//	noise functions over 1, 2, and 3 dimensions
	UINT32	CONST	P_NB	= 0x300;
	INT32	CONST	BM		= (P_NB-1);

	UINT32	CONST	N		= 0x1000;	//	??? 0x1000 Introduce an error in the calculation

//#define NP 12   /* 2^N */
//#define NM 0xfff

	INT32	p[  P_NB + P_NB + 2 ];
	REAL	g3[ P_NB + P_NB + 2 ][3];
	REAL	g2[ P_NB + P_NB + 2 ][2];
	REAL	g1[ P_NB + P_NB + 2 ];
}

FINLINE	CONSTEXPR REAL s_curve( REAL CONST t )
{
	return t * t * (3 - 2*t);
}

#define	SETUP( v, b0,b1, r0,r1 )\
	t = v + REAL(N);\
	b0 = (I_FLOOR(t)) & BM;\
	b1 = (b0+1) & BM;\
	r0 = t - (I_FLOOR(t));\
	r1 = r0 - REAL(1);

namespace
{
	bool	b_turbulence_to_init = true;
	bool	b_wavelet_turbulence_to_init = true;
}

REAL	noise1( REAL CONST arg )
{
	INT32	bx0,bx1;
	REAL	rx0,rx1, t, u,v;

	SETUP( arg, bx0,bx1, rx0,rx1 );

	REAL CONST sx = s_curve(rx0);

	u = rx0 * g1[ p[ bx0 ] ];
	v = rx1 * g1[ p[ bx1 ] ];

	return interpolate( u, v, sx );
}

REAL	noise2( REAL CONST * CONST vec )	//expecting 2 REALs
{
	INT32	bx0, bx1, by0, by1, b00, b10, b01, b11;
	REAL	rx0, rx1, ry0, ry1, *q, a, b, t, u, v;
	INT32	i, j;

	SETUP( vec[0], bx0,bx1, rx0,rx1 );
	SETUP( vec[1], by0,by1, ry0,ry1 );

	i = p[ bx0 ];
	j = p[ bx1 ];

	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];

	REAL CONST sx = s_curve(rx0);
	REAL CONST sy = s_curve(ry0);

#define	at2(rx,ry) ( rx * q[0] + ry * q[1] )

	q = g2[ b00 ] ; u = at2(rx0,ry0);
	q = g2[ b10 ] ; v = at2(rx1,ry0);
	a = interpolate( u, v, sx );

	q = g2[ b01 ] ; u = at2(rx0,ry1);
	q = g2[ b11 ] ; v = at2(rx1,ry1);
	b = interpolate( u, v, sx );

	return interpolate( a, b, sy );
}

FINLINE	void	mysetup( REAL CONST f, INT32 &b, REAL &r )
{
	b = I_FLOOR(f);
	r = f - b;
	b &= BM;
//	b1 = (b0+1) & BM;
}

FINLINE	REAL	myat3( REAL CONST * CONST q, REAL CONST rx, REAL CONST ry, REAL CONST rz )
{
	return rx * q[0] + ry * q[1] + rz * q[2];
}

FINLINE	REAL	noise3( REAL CONST * CONST vec )	//expecting 3 REALs
{
	INT32	bx, by, bz;
	REAL	rx0, ry0;
	REAL	rz;

	mysetup( vec[0], bx, rx0 );
	mysetup( vec[1], by, ry0 );

	REAL CONST rx1 = rx0 - REAL(1);
	bz = p[ bx ] + by;	//	bz is used as tmp until setup on z
	INT32 CONST b00 = p[ bz ];
	INT32 CONST b01 = p[ bz + 1 ];

	REAL CONST ry1 = ry0 - REAL(1);
	bz = p[ bx + 1 ] + by;
	INT32 CONST b10 = p[ bz ];
	INT32 CONST b11 = p[ bz + 1 ];

	REAL CONST sx = s_curve( rx0 );
	REAL CONST sy = s_curve( ry0 );

	mysetup( vec[2], bz, rz );
			//	if (rz>1.)
			//		DBG_PRINT_STRING( "rz too big %f", (DOUBLE)rz);
	REAL CONST sz = s_curve(rz);

	REAL a,b;
	a = interpolate( myat3( g3[ b00 + bz ], rx0, ry0, rz ), myat3( g3[ b10 + bz ], rx1, ry0, rz ), sx );
	b = interpolate( myat3( g3[ b01 + bz ], rx0, ry1, rz ), myat3( g3[ b11 + bz ], rx1, ry1, rz ), sx );
	REAL CONST c = interpolate( a, b, sy );

	bz = bz + 1;
	rz = rz - 1;	
	a = interpolate( myat3( g3[ b00 + bz ], rx0, ry0, rz ), myat3( g3[ b10 + bz ], rx1, ry0, rz ), sx );
	b = interpolate( myat3( g3[ b01 + bz ], rx0, ry1, rz ), myat3( g3[ b11 + bz ], rx1, ry1, rz ), sx );
	REAL CONST d = interpolate( a, b, sy );

	REAL result = interpolate( c, d, sz );
#if	0
	static REAL	tmp_max = 0;
	static REAL	tmp_min = 0;
	if ( result > tmp_max+.000001 )
		{
		tmp_max = result;
		DBG_PRINT_STRING( "max for noise3 is  %f", tmp_max );
/*		if (tmp_max>0.5)
			{
			( "tmp_max too big %f\n", tmp_max);
			printf( "a b c d : %f %f %f %f\n",	a,	b,	c,	d );
			printf( "sx sy sz %f %f %f\n",		sx,	sy, sz );
			}
*/
		}
	else if ( result < tmp_min-.000001 )
		{
		tmp_min = result;
		DBG_PRINT_STRING( "min for noise3 is  %f", tmp_min );
/*		if (tmp_max>0.5)
			{
			( "tmp_max too big %f\n", tmp_max);
			printf( "a b c d : %f %f %f %f\n",	a,	b,	c,	d );
			printf( "sx sy sz %f %f %f\n",		sx,	sy, sz );
			}
*/
		}
#endif

#ifdef	NOISE_TCURVE_SAMPLE_NB
	result = (result + REAL(1)) * REAL(.5) * REAL(NOISE_TCURVE_SAMPLE_NB);
	bz = I_FLOOR(result);
	if( bz >= NOISE_TCURVE_SAMPLE_NB-1 )
		bz = NOISE_TCURVE_SAMPLE_NB-2;
	else if( bz< 0 )
		bz  = 0;
	result = interpolate( curve_sample[bz], curve_sample[bz+1], result-REAL(bz) );
	result = result * REAL(2) - REAL(1);
#endif
/*
	if ( result > 1.)
		result = 1.;
	else if ( result < -1.)
		result = -1.;
*/
	return result;
}

REAL	noise3_by_perlin( REAL CONST * CONST vec )
{
	INT32	bx0, bx1, by0, by1, bz0, bz1, b00, b10, b01, b11;
	REAL	rx0, rx1, ry0, ry1, rz0, rz1, *q, sy, sz, a, b, c, d, t, u, v;
	INT32	i, j;

	SETUP( vec[0], bx0,bx1, rx0,rx1 );
	SETUP( vec[1], by0,by1, ry0,ry1 );
	SETUP( vec[2], bz0,bz1, rz0,rz1 );

	i = p[ bx0 ];
	j = p[ bx1 ];

	b00 = p[ i + by0 ];
	b10 = p[ j + by0 ];
	b01 = p[ i + by1 ];
	b11 = p[ j + by1 ];

	t  = s_curve(rx0);
	sy = s_curve(ry0);
	sz = s_curve(rz0);

#define at3(rx,ry,rz) ( rx * q[0] + ry * q[1] + rz * q[2] )

	q = g3[ b00 + bz0 ] ; u = at3(rx0,ry0,rz0);
	q = g3[ b10 + bz0 ] ; v = at3(rx1,ry0,rz0);
	a = interpolate( u, v, t );

	q = g3[ b01 + bz0 ] ; u = at3(rx0,ry1,rz0);
	q = g3[ b11 + bz0 ] ; v = at3(rx1,ry1,rz0);
	b = interpolate( u, v, t );

	c = interpolate( a, b, sy );

	q = g3[ b00 + bz1 ] ; u = at3(rx0,ry0,rz1);
	q = g3[ b10 + bz1 ] ; v = at3(rx1,ry0,rz1);
	a = interpolate( u, v, t );

	q = g3[ b01 + bz1 ] ; u = at3(rx0,ry1,rz1);
	q = g3[ b11 + bz1 ] ; v = at3(rx1,ry1,rz1);
	b = interpolate( u, v, t );

	d = interpolate( a, b, sy );

	return interpolate( c, d, sz );
}

namespace
{
	CONSTEXPR	INT32	TURBULENCE_SCALING_FACTOR_NB = 64;
	REAL	turbulence_scaling_factor[TURBULENCE_SCALING_FACTOR_NB];


	REAL get_turb_factor( INT32 CONST harm_nb )
	{
		if( harm_nb > TURBULENCE_SCALING_FACTOR_NB )
		{
			ERR_PRINT_STRING( "%s() scaling factor handle only up to %d, asking %d here ", __FUNCTION__, TURBULENCE_SCALING_FACTOR_NB, harm_nb );
			return turbulence_scaling_factor[TURBULENCE_SCALING_FACTOR_NB-1];
		}
		return turbulence_scaling_factor[harm_nb-1];
	}
}

static	void	turbulence_init_factor()
{
	REAL	t = 1.;
	REAL	total = 0.;
	REAL*	f = turbulence_scaling_factor;
	for( INT32 i = TURBULENCE_SCALING_FACTOR_NB; i>0; i-- )
	{
		total += t;
		*f++ = REAL(1) / total;
		t *= .5;
	}
}

#if 0 //unused but as reference
#define BUILD_FN_HARM( fn_name, CALL_VEC )\
REAL	fn_name( REAL CONST * CONST pos_in, REAL CONST freq, INT32 harm_nb ) \
{\
	REAL	t, vec[3];\
	\
	scale_v3( vec, pos_in, freq );\
	if( harm_nb == 1 )\
		t = CALL_VEC;\
	else if( harm_nb == 2 )\
	{\
		t = CALL_VEC ;\
		scale_v3( vec, 2. );\
		t += CALL_VEC * REAL(.5);\
		t *= turbulence_scaling_factor[1];\
	}\
	else\
	{\
		REAL factor = get_turb_factor(harm_nb);\
		t = CALL_VEC * factor ;\
		for( ; harm_nb > 1; --harm_nb )\
		{\
			scale_v3( vec, 2. );\
			factor *= .5;\
			t += CALL_VEC * factor ;\
		}\
	}\
	return (t + REAL(1)) * REAL(.5);\
}
BUILD_FN_HARM( fractalsum, noise3(vec) )
#endif

#define AAA_NOISE_USE_TEMPLATE() 1

#if AAA_NOISE_USE_TEMPLATE()
template< bool B_TURB, typename FN_NAME >
FINLINE REAL	FN_HARM( FN_NAME fn, REAL CONST * CONST pos_in, INT32 harm_nb )
{
	if( harm_nb == 0 )
		return 0.;

	REAL t;
	t = fn(pos_in);
	if constexpr (B_TURB)
		t = ABS(t);	
	if( harm_nb == 1 )
	{
	}
	else if( harm_nb == 2 )
	{
		FP32 vec[3];
		scale_v3( vec, pos_in, 2. );
		if constexpr (B_TURB)
			t += ABS(fn(vec)) * REAL(.5);
		else
			t += fn(vec) * REAL(.5);
		t *= turbulence_scaling_factor[1];
	}
	else
	{
		REAL factor = get_turb_factor(harm_nb);
		t *= factor;
		FP32 vec[3];
		scale_v3( vec, pos_in, 2. );
		if constexpr (B_TURB)
			t += ABS(fn(vec)) * factor;
		else
			t += fn(vec) * factor;
		factor *= .5;
		for( ; harm_nb > 2; --harm_nb )
		{
			scale_v3( vec, 2. );
			factor *= .5;
			if constexpr (B_TURB)
				t += ABS(fn(vec)) * factor;
			else
				t += fn(vec) * factor;
		}
	}
	if constexpr (B_TURB)
		return t;
	else
		return (t + REAL(1)) * REAL(.5);
}

REAL	turbulence( REAL CONST * CONST pos_in, INT32 CONST harm_nb )
{
	return FN_HARM< true >( noise3, pos_in, harm_nb );
}
REAL	fractalsum( REAL CONST * CONST pos_in, INT32 CONST harm_nb )
{
	return FN_HARM< false >( noise3, pos_in, harm_nb );
}
#else
REAL	turbulence( REAL CONST * CONST pos_in, REAL CONST freq, INT32 harm_nb )
{
	REAL	t, vec[3];

	scale_v3( vec, pos_in, freq );
	if( harm_nb == 1 )
	{
		return ABS( noise3(vec) );
	}
	else if( harm_nb == 2 )
	{
		t = ABS( noise3(vec) );
		scale_v3( vec, 2. );
		t += ABS( noise3(vec) ) * REAL(.5);
		return t * turbulence_scaling_factor[1];
	}
	else
	{
		REAL factor = get_turb_factor(harm_nb);
		t = ABS( noise3(vec) ) * factor;
		for ( ; harm_nb > 1; --harm_nb )
		{
			scale_v3( vec, 2. );
			factor *= .5;
			t += ABS( noise3(vec) ) * factor;
		}
		return t;
	}
}

REAL	fractalsum( REAL CONST * CONST pos_in, REAL CONST freq, INT32 harm_nb )
{
	REAL	t, vec[3];

	scale_v3( vec, pos_in, freq );
	if( harm_nb == 1 )
	{
		t = noise3(vec);
	}
	else if( harm_nb == 2 )
	{
		t = noise3(vec) ;
		scale_v3( vec, 2. );
		t += noise3(vec) * REAL(.5);
		t *= turbulence_scaling_factor[1];
	}
	else
	{
		REAL factor = get_turb_factor(harm_nb);
		t = noise3(vec) * factor ;
		for( ; harm_nb > 1; --harm_nb )
		{
			scale_v3( vec, 2. );
			factor *= .5;
			t += noise3(vec) * factor ;
		}
	}
	return (t + REAL(1)) * REAL(.5);
}
#endif

REAL	turbulence_by_perlin( REAL CONST * CONST pos_in, REAL freq )
{
	REAL	t, vec[3];
	for( t = 0.; freq >= 1.; freq /= 2 )
	{
		scale_v3( vec, pos_in, freq );
		t += ABS(noise3_by_perlin(vec)) / freq;
	}
	return t ;
}


/*
 * Procedural fBm evaluated at "point"; returns value stored in "value".
 *
 * Copyright 1994 F. Kenton Musgrave 
 * 
 * Parameters:
 *    ``H''  is the fractal increment parameter
 *    ``lacunarity''  is the gap between successive frequencies
 *    ``octaves''  is the number of frequencies in the fBm
 */

// RB:
// Modified to be evaluated with a scalar.
// FPO:
// Modified to use floats only

#define	MAX_OCTAVES 16
REAL	fBm1( REAL point, REAL H, REAL lacunarity, REAL octaves )
{
static	REAL	exponent_array[ MAX_OCTAVES + 1 ];
static	REAL	lastH;
		REAL	val, frequency, remainder;
		INT32	i;
static	bool	b_first = true;

//precompute and store spectral weights
	if( b_first || H!= lastH )
	{
		lastH = H;
		frequency = 1.0;
		for( i = 0; i <= octaves; ++i )
		{
			// compute weight for each frequency
			exponent_array[i] = (REAL)pow( frequency, -H );
			frequency *= lacunarity;
		}
		b_first = false;
	}

	val = 0.0;	//	initialize vars to proper values
	frequency = 1.0;

	//	inner loop of spectral construction
	for( i = 0; i < octaves; ++i )
	{
		val += noise1( point ) * exponent_array[i];
		point *= lacunarity;
	}

	remainder = octaves - (INT32)octaves;
	if ( remainder )	//	add in ``octaves''  remainder
	//``i''  and spatial freq. are preset in loop above 
		val += remainder * noise1( point ) * exponent_array[i];

	return( val );
}

// JAVA REFERENCE IMPLEMENTATION OF IMPROVED NOISE - COPYRIGHT 2002 KEN PERLIN.

static	INT32	pik[512] = {
	151,160,137,91,		90,15,131,13,		201,95,96,53,		194,233,7,225,
	140,36,103,30,		69,142,8,99,		37,240,21,10,		23,190,6,148,
	247,120,234,75,		0,26,197,62,		94,252,219,203,		117,35,11,32,
	57,177,33,88,		237,149,56,87,		174,20,125,136,		171,168,68,175,

	74,165,71,134,		139,48,27,166,		77,146,158,231,		83,111,229,122,
	60,211,133,230,		220,105,92,41,		55,46,245,40,		244,102,143,54,
	65,25,63,161,		1,216,80,73,		209,76,132,187,		208,89,18,169,
	200,196,135,130,	116,188,159,86,		164,100,109,198,	173,186,3,64,

	52,217,226,250,		124,123,5,202,		38,147,118,126,		255,82,85,212,
	207,206,59,227,		47,16,58,17,		182,189,28,42,		223,183,170,213,
	119,248,152,2,		44,154,163,70,		221,153,101,155,	167,43,172,9,
	129,22,39,253,		19,98,108,110,		79,113,224,232,		178,185,112,104,

	218,246,97,228,		251,34,242,193,		238,210,144,12,		191,179,162,241,
	81,51,145,235,		249,14,239,107,		49,192,214,31,		181,199,106,157,
	184,84,204,176,		115,121,50,45,		127,4,150,254,		138,236,205,93,
	222,114,67,29,		24,72,243,141,		128,195,78,66,		215,61,156,180
};

void	noise_improved_init()
{
	for( INT32 i=0; i < 256 ; ++i)
		pik[256+i] = pik[i];
}

static	FINLINE	REAL	fade( REAL CONST t )
{
	return t * t * t * (t * (t * 6 - 15) + 10);
}

static FINLINE	REAL	lerp( REAL CONST t, REAL CONST a, REAL CONST b )
{
	return a + t * (b - a);
}
// most recent
static	FINLINE	REAL	grad( INT32 CONST hash, REAL CONST x, REAL CONST y, REAL CONST z )
{
	INT32	h = hash & 15;						// CONVERT LO 4 BITS OF HASH CODE
	REAL	u = (h<8) ? x : y;					// INTO 12 GRADIENT DIRECTIONS.
	REAL	v = (h<4) ? y : ( (h==12||h==14) ? x : z) ;
	return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

REAL	noise_improved( REAL x, REAL y, REAL z )
{
	INT32	ix = I_FLOOR(x);
	INT32	iy = I_FLOOR(y);
	INT32	iz = I_FLOOR(z);

	x -= ix;		// FIND RELATIVE X,Y,Z
	y -= iy;		// OF POINT IN CUBE.
	z -= iz;

	ix &= 255;	// FIND UNIT CUBE THAT
	iy &= 255;	// CONTAINS POINT.
	iz &= 255;

	REAL	u = fade(x);	// COMPUTE FADE CURVES
	REAL	v = fade(y);	// FOR EACH OF X,Y,Z.
	REAL	w = fade(z);

	INT32	A = pik[ix  ]+iy;	// HASH COORDINATES OF
	INT32	AA = pik[A]+iz;
	INT32	AB = pik[A+1]+iz; 
	INT32	B = pik[ix+1]+iy;	// THE 8 CUBE CORNERS
	INT32	BA = pik[B]+iz;
	INT32	BB = pik[B+1]+iz;

		return lerp(w, lerp(v,  lerp(u, grad(pik[AA  ], x  , y  , z   ),  // AND ADD
										grad(pik[BA  ], x-1, y  , z   )), // BLENDED
								lerp(u, grad(pik[AB  ], x  , y-1, z   ),  // RESULTS
										grad(pik[BB  ], x-1, y-1, z   ))),// FROM  8
								lerp(v, lerp(u, grad(pik[AA+1], x  , y  , z-1 ),  // CORNERS
										grad(pik[BA+1], x-1, y  , z-1 )), // OF CUBE
								lerp(u, grad(pik[AB+1], x  , y-1, z-1 ),
										grad(pik[BB+1], x-1, y-1, z-1 ))));
}

#if AAA_NOISE_USE_TEMPLATE()
	REAL	turbulence_improved( REAL CONST * CONST pos_in, INT32 harm_nb )
	{
		return FN_HARM< true >( noise_improved3, pos_in, harm_nb );
	}
	REAL	fractalsum_improved( REAL CONST * CONST pos_in, INT32 harm_nb )
	{
		return FN_HARM< false >( noise_improved3, pos_in, harm_nb );
	}
#else

REAL	turbulence_improved( REAL CONST * CONST pos_in, REAL CONST freq, INT32 harm_nb )
{
	REAL	vec[3];
	scale_v3( vec, pos_in, freq );

	if( harm_nb == 1 )
	{
		return ABS( noise_improved3(vec) );
	}
	else if( harm_nb == 2 )
	{
		REAL t = ABS( noise_improved3(vec) );
		scale_v3( vec, 2. );
		t += ABS( noise_improved3(vec) ) * REAL(.5);
		return t * turbulence_scaling_factor[1];
	}
	else
	{
		REAL factor = turbulence_scaling_factor[harm_nb-1];
		REAL t = ABS( noise_improved3(vec) ) * factor;
		for ( ; harm_nb > 1; --harm_nb )
		{
			scale_v3( vec, 2. );
			factor *= .5;
			t += ABS( noise_improved3(vec) ) * factor;
		}
		return t;
	}
}

REAL	fractalsum_improved( REAL CONST * CONST pos_in, REAL CONST freq, INT32 harm_nb )
{
	REAL	vec[3];
	scale_v3( vec, pos_in, freq );

	if ( harm_nb == 1 )
	{
		return (noise_improved3(vec) + REAL(1)) * REAL(.5);
	}
	else if ( harm_nb == 2 )
	{
		REAL t = noise_improved3(vec) ;
		scale_v3( vec, 2. );
		t += noise_improved3(vec) * REAL(.5);
		t *= turbulence_scaling_factor[1];
		return (t + REAL(1)) * REAL(.5);
	}
	else
	{
		REAL factor = turbulence_scaling_factor[harm_nb-1];
		REAL t = noise_improved3(vec) * factor ;
		for ( ; harm_nb > 1; --harm_nb )
		{
			scale_v3( vec, 2. );
			factor *= .5;
			t += noise_improved3(vec) * factor ;
		}
		return (t + REAL(1)) * REAL(.5);
	}
}
#endif

//todoq we should rewrite the random to make it as good as rand on NTi
//#define	loc_get_rand()	((((INT32)a_rand.get_uint32()>>15) & RAND_MAX))
#define	loc_get_rand()	(rand())

void	turbulence_init()
{
	if( b_turbulence_to_init )
	{
		REAL	s;
		REAL	tmp;
		c_rand_lin	a_rand;
		srand( 1 );
		for( UINT32 i = 0 ; i < P_NB ; ++i )
		{
			p[i] = i;

			g1[i] = (REAL)((loc_get_rand() % (P_NB + P_NB)) - P_NB) / P_NB;

			for( UINT32 j = 0 ; j < 2 ; ++j )
				g2[i][j] = (REAL)((loc_get_rand() % (P_NB + P_NB)) - P_NB) / P_NB;
			normalize_v2r( g2[i] );

			do
			{
				tmp = (REAL) ((loc_get_rand() % (P_NB + P_NB)) - P_NB) / P_NB;
				g3[i][0] = tmp;
				s = tmp * tmp;
				tmp = (REAL) ((loc_get_rand() % (P_NB + P_NB)) - P_NB) / P_NB;
				g3[i][1] = tmp;
				s += tmp * tmp;
				tmp = (REAL) ((loc_get_rand() % (P_NB + P_NB)) - P_NB) / P_NB;
				g3[i][2] = tmp;
				s += tmp * tmp;
			}
			while( s > 1. );

			s = NOISE_FACTOR/SQRT(s);

			g3[i][0] *= s;
			g3[i][1] *= s;
			g3[i][2] *= s;
		}

		INT32	l;
		INT32	nb = P_NB;
		INT32	k;
		while( --nb )
		{
			k = p[nb];
			p[nb] = p[l = loc_get_rand() % P_NB];
			p[l] = k;
		}

		for( UINT32 i = 0 ; i < P_NB + 2 ; ++i )
		{
			p[P_NB + i] = p[i];
			g1[P_NB + i] = g1[i];
			for( INT32 j = 0 ; j < 2 ; ++j )
				g2[P_NB + i][j] = g2[i][j];
			for( INT32 j = 0 ; j < 3 ; ++j )
				g3[P_NB + i][j] = g3[i][j];
		}

		turbulence_init_factor();

		b_turbulence_to_init = false;
	}
}

// wavelet noise
static	REAL*	noiseTileData;
static	INT32	noiseTileSize;
//todoqqq check this buggy bis the right one
FINLINE	INT32	Mod( INT32 CONST x, INT32 CONST n )
{
	INT32	m = x%n;
	return (m<0) ? m+n : m;
}

#define ARAD	16
void	downsample( REAL* from, REAL* to, INT32 n, INT32 stride )
{
#ifdef _MSC_VER
#	pragma warning( push )  
#	pragma warning (4 : 4305)	// disable truncation warnings
#	pragma warning (4 : 4838)	// disable conversion requires a narrowing conversion warnings
#endif	//_MSC_VER
	REAL	*a, aCoeffs[ 2 * ARAD ] = {
		 0.000334, -0.001528,  0.000410,  0.003545, -0.000938, -0.008233,  0.002172,  0.019120,
		-0.005040, -0.044412,  0.011655,  0.103311, -0.025936, -0.243780,  0.033979,  0.655340,
		 0.655340,  0.033979, -0.243780, -0.025936,  0.103311,  0.011655, -0.044412, -0.005040,
		 0.019120,  0.002172, -0.008233, -0.000938,  0.003546,  0.000410, -0.001528,  0.000334};
#ifdef _MSC_VER
#	pragma warning( pop )  
#endif	//_MSC_VER

	a = &aCoeffs[ARAD];
	for( INT32 i = 0; i < n/2; ++i )
	{
		to[ i * stride ] = 0.0f;
		for( INT32 k = 2*i-ARAD; k <= 2*i+ARAD; ++k )
			to[ i*stride ] += a[k-2*i] * from[Mod(k,n)*stride];
	}
}

void	upsample( REAL* from, REAL* to, INT32 n, INT32 stride )
{
	REAL*	pt;
	REAL	pCoeffs[ 4 ] = { 0.25, 0.75, 0.75, 0.25 };

	pt = &pCoeffs[ 2 ];
	for( UINT32 i = 0; i < (UINT32)n/2; ++i )
	{
#if !AAA_WIN64()
		to[ i * stride ] = 0.0f;
		for( UINT32 k = i/2; k <= i/2+1; ++k )
			to[ i * stride ] += pt[ i - 2 * k ] * from[ Mod( k, n/2 ) * stride ];
#endif
	}
}

REAL	gaussianNoise()
{
#define NoiseMask  0x7fff
//#define SigmaGaussian  0.20
//#define SigmaMultiplicativeGaussian  0.5
//#define TauGaussian  1.0

DOUBLE	alpha, sigma, val;
//DOUBLE	beta;

	//alpha=(double) (rand() & NoiseMask)/NoiseMask;
	//if (alpha == 0.0)
	//	alpha=1.0;
//	double tau;
	//beta = (double) ( rand() & NoiseMask ) / NoiseMask;
	//sigma = sqrt( -1.0 * log( alpha ) ) * cos( PI_TIME_2 * beta);
	//tau = sqrt( -1.0 * log( alpha ) ) * sin( PI_TIME_2 * beta);
	//value = SigmaGaussian * sigma + TauGaussian * tau;
	//if(value < -1.0)
	//	return -1.0;
	//if( value > 1.0)
	//	return 1.0;
	//return( value ); //+ 0.5 );

// Central Limit Theorem Method
	sigma = 0.0;
	for( UINT32 i = 1; i < 30; ++i )  // to N
	{
		alpha = (double) (rand() & NoiseMask)/NoiseMask;	//U = uniform()
		sigma = sigma + alpha; //X = X + U
	}
	
	/* for uniform randoms in [0,1], mu = 0.5 and var = 1/12 */
	/* adjust X so mu = 0 and var = 1 */
	
	val = sigma - 30/2; //   X = X - N/2			/* set mean to 0 */
	val = val * sqrt( 12.0 / 30.0 ); //X = X * sqrt(12 / N)		/* adjust variance to 1 */
	val = val / 30.0;	// value is in interval [-N,N]
	return REAL(val);

	//// Box-Mueller Method
	//do
	//	{
	//	alpha = (double) (rand() & NoiseMask)/NoiseMask; //uniform()            /* U1=[0,1] */
	//	beta = (double) (rand() & NoiseMask)/NoiseMask;  //uniform()            /* U2=[0,1] */
	//	sigma = 2.0 * alpha - 1.0;            /* V1=[-1,1] */
	//	tau = 2.0 * beta - 1.0;           /* V2=[-1,1] */
	//	value = sigma * sigma + tau * tau;
	//	}
	//while (value >=1);
	//
 //  REAL X = sqrt(-2.0 * log(value) / value) * sigma;
 //  REAL Y=sqrt(-2.0 * log(value) / value) * tau;
	//return X;

	//REAL x1, x2, w, y1, y2;

	//do
	//	{
	//	x1 = 2.0 * ((double) (rand() & NoiseMask)/NoiseMask) - 1.0;
	//	x2 = 2.0 * ((double) (rand() & NoiseMask)/NoiseMask) - 1.0;
	//	w = x1 * x1 + x2 * x2;
	//	}
	//while ( w >= 1.0 );

	//w = sqrt( (-2.0 * log( w ) ) / w );
	//y1 = x1 * w;
	//y2 = x2 * w;

	//return y1;
}

// WAVELET NOISE
//
void	wavelet_noise_deinit()
{
	IF_FREE_ALIGNED_AND_NULL( noiseTileData );
}
void	wavelet_noise_init( INT32 n, INT32 olap )	// olap ??? it is unused
{
	if( b_wavelet_turbulence_to_init )
	{
		UINT32	n_2 = n * n;
		UINT32	n_3 = n_2 * n;
	
		INT32	j;
		INT32	sz = n_3 * sizeof(REAL);
		REAL*	temp1 = (REAL *)MALLOC_ALIGNED( sz, 0 );
		REAL*	temp2 = (REAL *)MALLOC_ALIGNED( sz, 0 );
		REAL*	noise = (REAL *)MALLOC_ALIGNED( sz, 0 );

		if( n % 2 )
			++n;

		clear_vr( temp1, n_3 );
		clear_vr( temp2, n_3 );

		// step 1, fill the tile with random number in the range -1 to 1
		for( UINT32 i = 0; i < n_3; ++i )
			noise[i] = gaussianNoise();

		// step 2 and 3, downsample and upsample the tile
		for( INT32 iy = 0; iy < n; ++iy )
			for( INT32 iz = 0; iz < n; ++iz )
			{
				j = iy * n + iz * n_2;
				downsample( &noise[j], &temp1[j], n, 1 );
				upsample( &temp1[j], &temp2[j], n, 1 );
			}
		for( INT32 ix = 0; ix < n; ++ix )
			for( INT32 iz = 0; iz < n; ++iz )
			{
				j = ix + iz * n_2;
				downsample( &temp2[j], &temp1[j], n, n );
				upsample( &temp1[j], &temp2[j], n, n );
			}
		for( INT32 ix = 0; ix < n; ++ix )
			for( INT32 iy = 0; iy < n; ++iy )
			{
				j = ix + iy * n;
				downsample( &temp2[j], &temp1[j], n, n_2 );
				upsample( &temp1[j], &temp2[j], n, n_2 );
			}

		// step 4
		for( UINT32 i = 0; i < n_3; ++i )
			noise[i] -= temp2[i];

		INT32	offset = n / 2;
		if( offset % 2 )
			++offset;

		for( INT32 i = 0, ix = 0; ix < n; ++ix )
			for( INT32 iy = 0; iy < n; ++iy )
				for( INT32 iz = 0; iz < n; ++iz )
					temp1[i++] = noise[ Mod( ix + offset, n) + Mod( iy + offset, n ) * n + Mod( iz + offset, n ) * n_2 ];
		for( UINT32 i = 0; i < n_3; ++i )
		{
			noise[i] += temp1[i];
	//		DBG_PRINT_STRING("noiseTileData %d = %f", i, noise[i] );
		}
		noiseTileData = noise;
		noiseTileSize = n;
		FREE_ALIGNED( temp1 );
		FREE_ALIGNED( temp2 );

		b_wavelet_turbulence_to_init = false;
	}
}


REAL	wavelet_noise( REAL CONST pt[3] )
{
	INT32	f[3], c[3], mid[3], n = noiseTileSize;
	REAL	w[3][3], t, result = 0;
	INT32	n_2 = n * n;

	/* Evaluate quadratic B-spline basis functions */
	for( UINT32 i = 0; i < 3; ++i )
	{
		mid[i] = INT32( CEIL( pt[i] - 0.5 ) );
		t = REAL( mid[i] - ( pt[i] - 0.5 ) );
		w[i][0] = t * t / 2;
		w[i][2] = ( 1 - t ) / 2;
		w[i][1] = 1 - w[i][0] - w[i][2];
	}
	/* Evaluate noise by weighting noise coefficients by basis function value */
	for( f[2] = -1; f[2] <= 1; ++f[2] )
		for( f[1] = -1; f[1] <= 1; ++f[1] )
//			for( f[1] = -1; f[1] <= 1; ++f[1] )
			for( f[0] = -1; f[0] <= 1; ++f[0] )
			{
				REAL	weight = 1.f;
				for( UINT32 i = 0; i < 3; ++i )
				{
					c[i] = Mod( mid[i] + f[i], n );
					weight *= w[i][ f[i]+1 ];
				}
				result += weight * noiseTileData[ c[2] * n_2 + c[1] * n + c[0] ];
			}
	return result;
}

#if AAA_NOISE_USE_TEMPLATE()
	REAL	turbulence_wavelet( REAL CONST * CONST pos_in, INT32 harm_nb )
	{
		return FN_HARM< true >( wavelet_noise, pos_in, harm_nb );
	}
	REAL	fractalsum_wavelet( REAL CONST * CONST pos_in, INT32 harm_nb )
	{
		return FN_HARM< false >( wavelet_noise, pos_in, harm_nb );
	}
#else
REAL	turbulence_wavelet( REAL CONST * CONST pos_in, REAL CONST freq, INT32 harm_nb )
{
	REAL	vec[3];
	scale_v3( vec, pos_in, freq);

	if ( harm_nb == 1 )
	{
		return ABS( wavelet_noise(vec) );
	}
	else if ( harm_nb == 2 )
	{
		REAL t = ABS( wavelet_noise(vec) );
		scale_v3( vec, 2. );
		t += ABS( wavelet_noise(vec) ) * REAL(.5);
		return t * turbulence_scaling_factor[1];
	}
	else
	{
		REAL factor = turbulence_scaling_factor[harm_nb-1];
		REAL t = ABS( wavelet_noise(vec) ) * factor;
		for( ; harm_nb > 1; --harm_nb )
		{
			scale_v3( vec, 2. );
			factor *= .5;
			t += ABS( wavelet_noise(vec) ) * factor;
		}
		return t;
	}
}

REAL	fractalsum_wavelet( REAL CONST * CONST pos_in, REAL CONST freq, INT32 harm_nb )
{
	REAL	vec[3];
	scale_v3( vec, pos_in, freq);

	if ( harm_nb == 1 )
	{
		return (wavelet_noise(vec) + REAL(1)) * REAL(.5);
	}
	else if ( harm_nb == 2 )
	{
		REAL t = wavelet_noise(vec) ;
		scale_v3( vec, 2. );
		t += wavelet_noise(vec) * REAL(.5);
		t *= turbulence_scaling_factor[1];
		return (t + REAL(1)) * REAL(.5);
	}
	else
	{
		REAL factor = turbulence_scaling_factor[harm_nb-1];
		REAL t = wavelet_noise(vec) * factor ;
		for( ; harm_nb > 1; --harm_nb )
		{
			scale_v3( vec, 2. );
			factor *= .5;
			t += wavelet_noise(vec) * factor ;
		}
		return (t + REAL(1)) * REAL(.5);
	}
}
#endif

// NOISE GLOBAL
//
namespace aaa {
namespace noise {
	void	init()
	{
		turbulence_init();
		noise_improved_init();
		wavelet_noise_init( 32, 0 );
	}

	void	deinit()
	{
		wavelet_noise_deinit();
	}
}	//namespace noise
}	//namespace aaa

