
// Main header
#include "math/TMath.h"


/////////////////////////////////////////////////////////////////////////////////////////////////
// solveCubic
template<typename T>
int solveCubic( T a, T b, T c, T d, T result[3] )
{
	if( a == 0 )
		return solveQuadratic( b, c, d, result );

	T f = ((3 * c / a) - ((b * b) / (a * a))) / 3;
	T g = ((2 * b * b * b) / (a * a * a) - (9 * b * c) / (a * a) + (27 * d) / (a)) / 27;
	T h = g * g / 4 + f * f * f / 27;

	if( f == 0 && g == 0 && h == 0 ) {
		result[0] = -tmath<T>::cbrt( d / a );
		return 1;
	}
	else if( h > 0 ) {
		// 1 root
		T r = -( g / 2 ) + tmath<T>::sqrt( h );
		T s = tmath<T>::cbrt( r );
		T t = -(g / 2) - tmath<T>::sqrt( h );
		T u = tmath<T>::cbrt( t );

		result[0] = (s + u) - (b / (3 * a));
		return 1;
	}
	else { // 3 roots
		T i = tmath<T>::sqrt( (g * g / 4) - h );
		T j = tmath<T>::cbrt( i );
		T k = tmath<T>::acos( -(g / (2 * i)) );
		T l = -j;
		T m = tmath<T>::cos( k / 3 );
		T n = tmath<T>::sqrt(3) * tmath<T>::sin( k / 3 );
		T p = -b / (3 * a);
		result[0] = 2 * j * tmath<T>::cos(k / 3) - (b / (3 * a));
		result[1] = l * (m + n) + p;
		result[2] = l * (m - n) + p;
		return 3;
	}
}
template int solveCubic( float a, float b, float c, float d, float result[3] );
template int solveCubic( double a, double b, double c, double d, double result[3] );
