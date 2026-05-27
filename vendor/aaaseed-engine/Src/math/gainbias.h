
#ifdef AAA_GAINBIAS_H
#error "GAINBIAS_H included more than once."
#endif
#define AAA_GAINBIAS_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_AAA_MATH_H
#	include "math/aaa_math.h"
#endif

//////
////	Bias Gain
//
//	Christophe Slick method see graphics Gem IV
template <typename T>				CONSTEXPR	FINLINE	T		bias_slick_factor_slow( T b )			{	return T(1)/b - T(2);						}
template <typename T>				CONSTEXPR	FINLINE	T		bias_slick_factor_fast( T b )			{	return OVER1(b) - T(2);						}
#define	bias_slick_factor										bias_slick_factor_slow

template <typename T>				CONSTEXPR	FINLINE	T		bias_slick_from_factor_slow( T b )		{	return T(1) / (b+T(2));						}
template <typename T>				CONSTEXPR	FINLINE	T		bias_slick_from_factor_fast( T b )		{	return OVER1(b+T(2));						}
#define	bias_slick_from_factor									bias_slick_from_factor_slow

template <typename T>				CONSTEXPR	FINLINE	T		gain_slick_factor( T g )				{	return bias_slick_factor( T(1)- g );		}
template <typename T>				CONSTEXPR	FINLINE	T		gain_slick_from_factor( T g )			{	return T(1) - bias_slick_from_factor(g);	}

template <typename T, typename S>	CONSTEXPR	FINLINE	void	bias_slick_with_factor( T& f, S b )		{	f /= T( b * (T(1)-f) + T(1) );				}
template <typename T, typename S>	CONSTEXPR	FINLINE	void	bias_slick( T& f, S b )
{
/*	if( b == 1. )
	{
		if( f < .5 )
			f = 0.;
		else
			f =  1.;
	}
	else
*/
	bias_slick_with_factor( f,  bias_slick_factor(b) );
}
template <typename T, typename S>
CONSTEXPR	FINLINE	void bias_slick_3d( T* f, S b )
{
/*	if( b == 1. )
	{
		if( f < .5 )
			f = 0.;
		else
			f =  1.;
	}
	else
*/
	{
		T	tmp = bias_slick_factor(b);
		bias_slick_with_factor( f[0], tmp );
		bias_slick_with_factor( f[1], tmp );
		bias_slick_with_factor( f[2], tmp );
	}
}

template <typename T, typename S>
CONSTEXPR	FINLINE	void bias_slick_if_needed( T& f, S b )
{
	if ( b != 0.5 )
		bias_slick( f, b );
}
template <typename T, typename S>
CONSTEXPR	FINLINE	void bias_slick_if_needed_3d( T* f, S b )
{
	if ( b != 0.5 )
		bias_slick_3d( f, b );
}

template <typename T, typename S>
CONSTEXPR	FINLINE	void gain_slick_with_factor( T& f, S g )
{
	T	fb = T(1) - T(2) * f;
	if ( fb > 0. )
		f /= T(g) * fb + T(1);
	else 
	{
		fb *= T(g);
		f =	(fb - f) / (fb - T(1));
	}
}
template <typename T, typename S>
CONSTEXPR	FINLINE	void gain_slick( T& f, S g )
{
	if( g == 1. )
	{
		if( f < .5 )
			f = 0.;
		else
			f =  1.;
	}
	else
		gain_slick_with_factor( f,  gain_slick_factor(g) );
}
template <typename T, typename S>
CONSTEXPR	FINLINE	void gain_slick_3d( T* f, S g )
{
	if( g == 1. )
	{
		if( f[0] < .5 )		f[0] = 0.;
		else				f[0] = 1.;
		if( f[1] < .5 )		f[1] = 0.;
		else				f[1] = 1.;
		if( f[2] < .5 )		f[2] = 0.;
		else				f[2] = 1.;
	}
	else
	{
		T	tmp = gain_slick_factor(g);
		gain_slick_with_factor( f[0], tmp );
		gain_slick_with_factor( f[1], tmp );
		gain_slick_with_factor( f[2], tmp );
	}
}

template <typename T, typename S>
CONSTEXPR	FINLINE	void gain_slick_if_needed( T& f, S g )
{
	if ( g != 0.5 )
		gain_slick( f, g );
}
template <typename T, typename S>
CONSTEXPR	FINLINE	void gain_slick_if_needed_3d( T* f, S g )
{
	if ( g != 0.5 )
		gain_slick_3d( f, g );
}

//	Perlin original method, cost a lot more than slick
template <typename T>				CONSTEXPR	FINLINE	T		bias_perlin_factor_slow( T b )			{	return LOG(b)/LOG(.5);									}
template <typename T>				CONSTEXPR	FINLINE	T		bias_perlin_factor_fast( T b )			{	return T(-LOG(b)*1.442695041);							}
#define	bias_perlin_factor							bias_perlin_factor_fast
template <typename T>				CONSTEXPR	FINLINE	T		gain_perlin_factor( T g )				{	return bias_perlin_factor( T(1)-g );					}

template <typename T, typename S>	CONSTEXPR	FINLINE	void	bias_perlin_with_factor( T& f, S b )	{	f = (T) pow( double(f), double(b) );					}
template <typename T, typename S>	CONSTEXPR	FINLINE	void	bias_perlin( T& f, S b )				{	bias_perlin_with_factor( f,  bias_perlin_factor(b) );	}

template <typename T, typename S>	CONSTEXPR	FINLINE	void	gain_perlin_with_factor( T& f, S g )
{
	if ( f < 0.5 )	f = T( POW( T(2)*f, g) * T(.5) );
	else			f = T( (T(2) - POW( T(2)-T(2)*f, g)) * T(.5) );
}
template <typename T, typename S>	CONSTEXPR	FINLINE	void	gain_perlin( T& f, S g )				{	gain_perlin_with_factor( f,  gain_perlin_factor(g) );	}

//todo should use these or similar everywhere
template <typename T>				CONSTEXPR	FINLINE	REAL	gain_factor(		T g )				{	return gain_slick_factor( g );							}
template <typename T>				CONSTEXPR	FINLINE	REAL	bias_factor(		T b )				{	return bias_slick_factor( b );							}
template <typename T, typename S>	CONSTEXPR	FINLINE	void	gain_with_factor(	T& f, S g )			{	gain_slick_with_factor( f, g );							}
template <typename T, typename S>	CONSTEXPR	FINLINE	void	bias_with_factor(	T& f, S b )			{	bias_slick_with_factor( f, b );							}
template <typename T, typename S>	CONSTEXPR	FINLINE	void	gain(				T& f, S g )			{	gain_slick( f, g );										}
template <typename T, typename S>	CONSTEXPR	FINLINE	void	bias(				T& f, S b )			{	bias_slick( f, b );										}
template <typename T, typename S>	CONSTEXPR	FINLINE	void	gain_bias(			T& f, S g, S b )	{
																											gain( f, g );
																											bias( f, b );
																										}

static	CONST	REAL	GAIN_MIN	=	REAL(0.00001);
static	CONST	REAL	GAIN_MAX	=	REAL(0.99999);
static	CONST	REAL	BIAS_MIN	=	REAL(0.00001);
static	CONST	REAL	BIAS_MAX	=	REAL(0.99999);

template <typename T>	CONSTEXPR	FINLINE	T	CLAMP_GAIN_BIAS( T gb )
{
	if( gb < GAIN_MIN )		return GAIN_MIN;
	if( gb > GAIN_MAX )		return GAIN_MAX;
	return gb;
}

template <typename T, typename S>	CONSTEXPR	FINLINE	void	gain_clamped(		T& f, S g )			{	gain_slick( f, CLAMP_GAIN_BIAS(g) );					}
template <typename T, typename S>	CONSTEXPR	FINLINE	void	bias_clamped(		T& f, S b )			{	bias_slick( f, CLAMP_GAIN_BIAS(b) );					}
template <typename T, typename S>	CONSTEXPR	FINLINE	void	gain_bias_clamped(	T& f, S g, S b )	{
																											gain_clamped( f, g );
																											bias_clamped( f, b );
																										}

