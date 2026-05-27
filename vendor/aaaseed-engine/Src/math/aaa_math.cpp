#include "err.h"
#include "math/aaa_math.h"
#include "aaa_util.h"
#include <limits>


UINT8	c_math::bits_nb		[ BITS_NB_TABLE_SIZE	];
REAL	c_math::sin_table	[ SIN_TABLE_SIZE		];
REAL	c_math::tan_table	[ TAN_TABLE_SIZE		];
REAL	c_math::over1_table	[ OVER1_TABLE_SIZE		];

static c_math math;	// force call to init() through constructor

//////
////	TEST
//
AAA_ERR	c_math::test()
{
	REAL	df;
	INT32	d;

	GOOD_PRINT_STRING( "Test of Math module" );
	for( REAL f = -2.5; f < 2.; f += .5 )
	{
		d = (INT32) f;
		GOOD_PRINT_STRING( "f is %f, (INT32)f is %d", (DOUBLE)f, d );
	}
	for( REAL f = -2.5; f < 2.; f += .5 )
	{
		df = F_FLOOR( f );
		GOOD_PRINT_STRING( "f is %f, F_FLOOR(f) is %f", (DOUBLE)f, df );
	}
	for( REAL f = -2.5; f < 2.; f += .5 )
	{
		d = I_FLOOR( f );
		GOOD_PRINT_STRING( "f is %f, I_FLOOR(f) is %d", (DOUBLE)f, d );
	}
	for( REAL f =- 2.5; f < 2.; f += .25 )
	{
		df = FMOD( f );
		GOOD_PRINT_STRING( "f is %f, FMOD(f,1) %f", (DOUBLE)f, df );
	}
	for( INT32 i =- 4; i < 5; ++i )
	{
		GOOD_PRINT_STRING( "i is %d, i is %d, IMOD(i,3) is %d", i, i % 3, IMOD( i, 3 ) );
	}
	GOOD_PRINT_STRING( "nb\tc modulo %%\tIMOD" );
	for( INT32 i =- 8; i < 13; ++i )
	{
		GOOD_PRINT_STRING( "%3d\t\t%3d\t%d", i, i % 3, IMOD( i, 3 ) );
	}
	return	AAA_OK;
}


//CONST	REAL	FLOAT_MAX = FLT_MAX;		//FLT_MAX	3.402823466e+38F
//CONST	REAL	FLOAT_MAX = std::numeric_limits<REAL>::max();		//FLT_MAX	3.402823466e+38F

namespace
{
	FINLINE	DOUBLE	SIN( DOUBLE a )
	{
	//#if AAA_REAL_IS_DOUBLE()
		return sin( a );
	//#else
	//	return sinf( a );
	//#endif
	}

	FINLINE	DOUBLE	TAN( DOUBLE a )
	{
	//#if AAA_REAL_IS_DOUBLE()
		return tan( a );
	//#else
	//	return tanf( a );
	//#endif
	}
}


//////
////	INITIALIZATION
//
c_math::c_math()
{
	REAL*	pf;

	//	initialize sin_table
	pf = sin_table - 1;
	for( INT32 i = 0; i < SIN_TABLE_SIZE; ++i )
	{
		*++pf = REAL( SIN( (i * PI_TIME_2) / SIN_TABLE_SIZE  ) );
	}

	//	initialize tan_table
	pf = tan_table - 1;
	for( INT32 i = 0; i < TAN_TABLE_SIZE; ++i )
	{
		*++pf = REAL( TAN( (i * PI_TIME_2) / TAN_TABLE_SIZE ) );
	}

	//	initialize over1
	pf = over1_table;
	over1_table[0] = REAL_BIG_VALUE;
	for( INT32 i = 1; i < OVER1_TABLE_SIZE; ++i )
	{
		*++pf = REAL(OVER1_TABLE_SIZE)/i;
	}

	//	initialize bits_nb
	//	count the bits but not the 3 lowest ones: each loop write 8 values
	UINT8* p = bits_nb - 1;
	for( INT32 i = 0; i < BITS_NB_TABLE_SIZE/8; ++i )
	{
		INT32 count = 0;
		for( INT32 j = 0; j < (8+5); ++j )
		{
			if( i & ( 1 << j ) )
				++count;
		}
		*++p = count;	// 0
		*++p = count+1;	// 1
		*++p = count+1;	// 2
		*++p = count+2;	// 3

		*++p = count+1; // 4
		*++p = count+2; // 5
		*++p = count+2; // 6
		*++p = count+3; // 7
	}
}



void compute_gauss_coeff( DOUBLE* kernel, INT32 W, DOUBLE sigma )
{
	DOUBLE	mean	= W/2;
	DOUBLE	sum		= 0.0; // For accumulating the kernel values
	DOUBLE*	pt		= kernel;
	for( INT32 y = 0; y < W; ++y )
		for( INT32 x = 0; x < W; ++x ) 
		{
			*pt = exp( -0.5 * (pow((x-mean)/sigma, 2.0) + pow((y-mean)/sigma,2.0)) ) / (2 * M_PI * sigma * sigma);
			// Accumulate the kernel values
			sum += *pt;
			++pt;
		}

// Normalize the kernel
	pt = kernel;
	for( INT32 i = W*W; i > 0; --i ) 
		*pt++ /= sum;
}

namespace {
	//rotate/flip a quadrant appropriately
	FINLINE void hilbert_rot( INT32 n, INT32& x, INT32& y, INT32 CONST rx, INT32 CONST ry )
	{
		if( ry == 0 )
		{
			if( rx == 1 )
			{
				--n;
				INT32 tmp = n - x;
				x = n - y;
				y = tmp;
			}
			else
			{
				SWAP( x,y );
			}
		}
	}
}

//todo optimize
INT32 c_math::convert_hilbert_xy_to_d( INT32 n, INT32 x, INT32 y ) NOEXCEPT 
{
	INT32 d=0;
	n = n >> 1;
	for( ; n>0; n >>= 1 )
	{
		INT32 rx = (x & n) > 0;
		INT32 ry = (y & n) > 0;
		d |= n * n * ((3 * rx) ^ ry);
		hilbert_rot( n, x,y, rx,ry );
	}
	return d;
}

void c_math::convert_hilbert_d_to_xy( INT32 n, INT32 d, INT32& x, INT32& y ) NOEXCEPT
{
	INT32 t = d;
	x = y = 0;
	for( INT32 s=1; s<n; s*=2 )
	{
		INT32 rx = (t >> 1) & 1;
		INT32 ry = (t ^ rx) & 1;
		hilbert_rot( s, x,y, rx,ry );
		x += s * rx;
		y += s * ry;
		t >>= 2;
	}
}

/*
function last2bits(x) { return (x & 3); }

function hindex2xy(hindex, N)
{
    // 1. compute position of node in N=2 curve
    var positions = [
     [0, 0],	//	0
     [0, 1],	//	1
     [1, 1],	//	2
     [1, 0]		//	3
    ];

    var tmp = positions[last2bits(hindex)];
    hindex = (hindex >> 2);

    // 2. iteratively compute coords
    var x = tmp[0];
    var y = tmp[1];
    
    for( var n = 4; n <= N; n *= 2 )
	{
        var n2 = n / 2;
        switch( last2bits(hindex) )
		{
        case 0: //	case A: left-bottom
            tmp = x; x = y; y = tmp;
            break;
        case 1: // case B: left-upper
            x = x;
            y = y + n2;
            break;
        case 2: // case C: right-upper
            x = x + n2;
            y = y + n2;
            break;
        case 3: // case D: right-bottom
            tmp = y;
            y = (n2-1) - x;
            x = (n2-1) - tmp;
            x = x + n2;
            break;;;
        }
        hindex = (hindex >> 2);
    }
    return [x, y];
}
*/

/*
// It takes as input the coordinates of the point (x,y) to be inverted,
//		and the corners of an enclosing right isosceles triangle (ax, ay), (bx, by), and (cx, cy).
//		(Note that the unit square is the union of two such triangles.)
//		The remaining parameters specify the level of accuracy to which the inverse should be computed.
//more on https://en.m.wikipedia.org/wiki/Sierpi%C5%84ski_curve

    static long sierp_pt2code( double ax, double ay, double bx, double by, double cx, double cy,
        int currentLevel, int maxLevel, long code, double x, double y ) 
    {
        if (currentLevel <= maxLevel) {
            currentLevel++;
            if ((sqr(x-ax) + sqr(y-ay)) < (sqr(x-cx) + sqr(y-cy))) {
                code = sierp_pt2code( ax, ay, (ax+cx)/2.0, (ay+cy)/2.0, bx, by,
                    currentLevel, maxLevel, 2 * code + 0, x, y );
            }
            else {
                code = sierp_pt2code( bx, by, (ax+cx)/2.0, (ay+cy)/2.0, cx, cy,
                    currentLevel, maxLevel, 2 * code + 1, x, y );
            }
        }
        return code;    
    }
*/