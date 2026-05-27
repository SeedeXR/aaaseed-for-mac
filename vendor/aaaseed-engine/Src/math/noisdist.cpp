#include "math/noisdist.h"
#include "err.h"
#include "math/noisturb.h"
#include "math/rand.h"
#include "file/aaa_file.h"
#include <stdio.h>
#include "aaa_util.h"

INT32 CONST	NOISE_SAMPLE_NB		=	0x400000;
REAL CONST	NOISE_VOLUME_SIZE	=	100.;
INT32 CONST	PROBA_SAMPLE_NB		=	0x100000;
INT32 CONST	CURVE_SAMPLE_NB		=	0x10000;
INT32 CONST	OUT_SAMPLE_NB		=	0x1000;
INT32 CONST	NUMBER_PER_LINE		=	8;


c_fn1d_periodic_turb_dist::c_fn1d_periodic_turb_dist()
{
	_index_col_out = 0;

	set_sample_nb( PROBA_SAMPLE_NB);
	set_min_max( 0, 1.);
	set_frequency( 1.);
}


void	c_fn1d_periodic_turb_dist::fill()
{
	samples_alloc();

	INT32	nb = get_sample_nb();
	REAL*	p_f = _samples - 1;
	//	>= because there is sample_nb+1  
	for( INT32 i = nb; i >= 0; --i )
	{
		*++p_f = 0.;
	}

	c_rand_lin	rnd;
	REAL pt[3];
	p_f = _samples;
	for( INT32 i = NOISE_SAMPLE_NB; i > 0; --i )
	{
		pt[0] = rnd.get_fp32() * NOISE_VOLUME_SIZE;
		pt[1] = rnd.get_fp32() * NOISE_VOLUME_SIZE;
		pt[2] = rnd.get_fp32() * NOISE_VOLUME_SIZE;
		//f = turbulence(pt, 1.0, 1);
		REAL	f = noise3(pt) * REAL(.5) + REAL(.5);
		f *= nb;
		INT32	itmp = I_FLOOR(f);
#if	0
		if ( itmp >= nb)
			itmp = nb-1;
		else if ( itmp < 0)
			itmp = 0;
#else	
		if ( 0<=itmp && itmp<nb)
#endif
		*( p_f + itmp ) += 1.;
	}
}

void	c_fn1d_periodic_turb_dist::save_value( REAL value)
{
	if ( --_index_col_out <= 0 )
	{
		_index_col_out = NUMBER_PER_LINE;
		fprintf( _file, "%f,\n", (DOUBLE)value );
	}
	else
	{
		fprintf( _file, "%f,\t", (DOUBLE)value );
	}	
}

void	c_fn1d_periodic_turb_dist::save_cpp_to_file( o_str CONST & filename )
{
	if( !filename.is_empty() )
	{
		//	open _file
		_file = c_file::FOPEN( filename, "wt");
		if( !_file )
		{
			ERR_PRINT_STRING( "Can't open _file : %s", filename.get() );
			//return  nullptr;
		}
		else
		{
			DOUBLE	tmp;

			fprintf( _file, "#define\tNOISE_TCURVE_SAMPLE_NB\t%d\n", OUT_SAMPLE_NB );
			fprintf( _file, "REAL\tcurve_sample[NOISE_TCURVE_SAMPLE_NB]=\n");
			_index_col_out = NUMBER_PER_LINE;

			fprintf( _file, "{\n" );

			integrate_start();

			for( UINT32 i = 0; i < OUT_SAMPLE_NB; ++i )
			{
				_x = DOUBLE( i ) / DOUBLE( OUT_SAMPLE_NB );
				while( _cur < _x )
					integrate_next();
				if( _cur == 1. )
				{
					save_value( 1.0 );
				}
				else
				{
					tmp = ( _x - _x0 );
					tmp += _y0;
					save_value( REAL(tmp) );
				}
			}
			fprintf( _file, "};\n\n" );
			c_file::FCLOSE( _file );
		}
	}
}

void	c_fn1d_periodic_turb_dist::integrate_start()
{
	_index = 0;
	_next = 0;
	_xp = 0;
	_cur = 0.;
	_x = 0.;
	_x0 = 0.;
	_y0 = 0.;
	_target = 0;
	_curve_surface_step =	DOUBLE(NOISE_SAMPLE_NB)
							/ DOUBLE(CURVE_SAMPLE_NB);
	_p = _samples;
}

void	c_fn1d_periodic_turb_dist::integrate_next()
{
	//	keep start of segment
	_x0 = _cur;
	_y0 = (DOUBLE)(_index )/(DOUBLE)CURVE_SAMPLE_NB;

	//	define aim
	_target = INT32( _curve_surface_step * (DOUBLE)(_index+1) );
	PRINT_STRING("_target : %d :", _target );

	//	move
	while( _next < _target )
	{
		_prev = _next;
		if(	_xp < PROBA_SAMPLE_NB )
		{
			_next += (INT32)*_p++;
			++_xp;
		}
		else
		{
			_next = _target;
			_cur = 1.;
			PRINT_STRING(",%f\n", (DOUBLE)_cur );
			return;
		}
	}
	//
	_cur = (DOUBLE)(_target-_prev )/(DOUBLE)(_next-_prev );
	PRINT_STRING(",%f", (DOUBLE)_cur );
	_cur += (DOUBLE)(_xp-1);
	PRINT_STRING(",%f", (DOUBLE)_cur );
	_cur /= (DOUBLE)PROBA_SAMPLE_NB;
	PRINT_STRING(",%f\n", (DOUBLE)_cur );
	++_index;
}


c_fn1d_periodic_turb_dist	g_turb_dist;

