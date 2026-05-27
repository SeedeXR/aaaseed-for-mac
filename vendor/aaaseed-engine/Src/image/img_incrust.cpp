#include "img_incrust.h"
#include "math/gainbias.h"
#include "aaa_util.h"

//UINT8	_lut[ LUT_VALUE_NB ];


void	c_incrust_process::update()
{
	static	CONSTEXPR DOUBLE LUT_FACTOR			= LUT_VALUE_NB - 1;
	static	CONSTEXPR DOUBLE LUT_FACTOR_OVER	= 1. / LUT_FACTOR;

	REAL	min	= _min_ui;
	REAL	max	= _max_ui;
	REAL 	gain_factor	= gain_slick_factor( _gain_ui );
	REAL 	bias_factor = bias_slick_factor( _bias_ui );
	if( _min != min || _max != max || _gain_factor != gain_factor || _bias_factor != bias_factor || _b_inverse != _b_inverse_ui )
	{
		_min			= min;
		_max			= max;
		_gain_factor	= gain_factor;
		_bias_factor	= bias_factor;
		_b_inverse		= _b_inverse_ui;

		REAL f = OVER_ONE_AS_REAL( max - min );
		if( min <= max )
		{
			for( INT32 j = 0; j < LUT_VALUE_NB; ++j )
			{
				REAL r = REAL( j * LUT_FACTOR_OVER );
				if( r <= min )
					r = 0.;
				else if( max <= r )
					r = 1.;
				else 
				{
					r = CLAMP_01( ( r - min ) * f );
					if( gain_factor != 0. )
						gain_slick_with_factor( r, gain_factor );
					if( bias_factor != 0. )
						bias_slick_with_factor( r, bias_factor );
				}
				if( _b_inverse )
					r = REAL( 1.-r );
				_lut[j] = UINT8( r * LUT_FACTOR );
			}
		}
		else
		{
			for( INT32 j = 0; j < LUT_VALUE_NB; ++j )
			{
				REAL r = REAL( j * LUT_FACTOR_OVER );
				if( r <= max )
					r = 1.;
				else if( min <= r )
					r = 0.;
				else
				{
					r = CLAMP_01( ( r - min ) * f );
					if( gain_factor != 0. )
						gain_slick_with_factor( r, gain_factor );
					if( bias_factor != 0. )
						bias_slick_with_factor( r, bias_factor );
				}
				if( _b_inverse )
					r = REAL( 1.-r );
				_lut[j] = UINT8( r * LUT_FACTOR );
			}
		}
	}
}
