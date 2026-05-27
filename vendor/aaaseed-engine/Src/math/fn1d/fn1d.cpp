#include "aaa_def.h"
#include "math/fn1d/fn1d.h"
#include "err.h"
#include "gol/gol_draw.h"
#include "gol/gol_color.h"
#include "gol/gol_light.h"
#include "gol/gol_tex.h"
#include "gol/gol_matrix.h"


//////
////	FNX
//
//todo make it clean
void c_fn1d::compute()
{
	ERR_PRINT_STRING( "c_fn1d::compute() should not be called");
}


REAL	c_fn1d::get( REAL f_in)
{
	ERR_PRINT_STRING( "c_fn1d::get() should not be called");
	return 0.;
}

void c_fn1d::update()
{
	if( _b_to_update )
		compute();
}

REAL	c_fn1d::get_from_2_cont( REAL a, REAL b, REAL fb)
{
	b *= fb;
	return get( a) + get( a + b+b);
}

//todo	integrate per_z and per_x ?
//todo	speed it up by unroll and thinking
REAL	c_fn1d::get_from_2( REAL a, REAL b, REAL	fb)
{
	REAL	flo;
	REAL	left;
	REAL	prev;
	REAL	next;

	flo = F_FLOOR( b);
	left = b - flo;
	flo *= fb;
	prev = get( a+flo) + get( a+flo+flo);
	flo += fb;
	next = get( a+flo) + get( a+flo+flo);

	return (prev*(REAL(1)-left) + next*left);
}

//////
////	FNX PERIODIC DONE WITH AN ARRAY OF SAMPLES
//
void	c_fn1d_periodic::set_min_max( REAL min_in, REAL max_in )
{
	if ( ( min_in != _min ) || ( max_in != _max ) )
	{
		_min = min_in;
		_max = max_in;
		_b_to_update = true;
	}
}

#include "draw/render.h"
//todoq give interface to visualization
//		to change the _visualize_step
//		and even to navigate in it
void	c_fn1d_periodic::visualize_in_rect(REAL x, REAL y, REAL w, REAL h)
{
	REAL t = OVER_ONE_AS_REAL( _frequency );
	REAL dt = t / _visualize_step;
	REAL dx = w / _visualize_step;
	x = 0.;
	t = 0.;

	GOL::begin(GL_LINE_STRIP);
	for( INT32 i = _visualize_step; i > 0; --i )
	{
		GOL::vertex2( x, y + ( get( t) - _min ) / ( _max - _min ) * h );
		t += dt;
		x += dx;
	}
	GOL::end();
}


#define	VISUALIZE_SIZE_REF	100
void c_fn1d_periodic::visualize()
{	//todo these disables are dangerous
	GOL::set_texture_dim( 0 );
	GOL::disable_lighting();
	GOL::matrix::push();
		GOL::matrix::set_ortho_with_secu_margin( VISUALIZE_SIZE_REF, VISUALIZE_SIZE_REF);
		GOL::color_red();
		visualize_in_rect(0, 0, VISUALIZE_SIZE_REF, VISUALIZE_SIZE_REF);
	GOL::matrix::pop();

}


//////
////	FNX PERIODIC DONE WITH AN ARRAY OF SAMPLES
//
void c_fn1d_periodic_table::samples_alloc()
{
	INT32	nb = get_sample_nb();
	if ( nb == 0 )
		debug_break( "Bug in c_fn1d_periodic_table::samples_alloc()");
	_samples = (REAL *) REALLOC_ALIGNED( _samples, sizeof(REAL) * (nb + 1) );
}

c_fn1d_periodic_table::~c_fn1d_periodic_table()
{
	FREE_ALIGNED( _samples );
}

REAL c_fn1d_periodic_table::get( REAL f_in )
{
	if( _samples )
	{
		f_in *= _frequency;
		//	get the decimal part
		f_in -= F_FLOOR(f_in);
		// and use it in the array
		f_in *= _f_sample_nb;

		INT32	i_floor = (INT32) f_in;	// we know f_in > 0
		REAL*	p_f = _samples + i_floor;
		f_in	-= i_floor;

		return *p_f * (REAL(1) - f_in) + *(p_f+1) * f_in;
	}
	else
		return 0;
}

void	c_fn1d_periodic_table::find_min_max()
{
	REAL*	p_f;
	REAL	f_min;
	REAL	f_max;
	REAL	f;
	//	lets find the min and max
	f_min = f_max = *_samples;

	p_f = _samples;
	for( INT32 i = get_sample_nb(); i > 0; --i )
	{
		f = *p_f;
		if ( f > f_max )
		{
			f_max = f;
			DBG_PRINT_STRING( "max = %f", (DOUBLE)f_max );
		}
		else if ( f < f_min )
		{
			f_min = f;
			DBG_PRINT_STRING( "min = %f", (DOUBLE)f_min );
		}
		++p_f;
	}

	_min = f_min;
	_max = f_max;
}

void	c_fn1d_periodic_table::normalize()
{
	REAL	*p_f;
	REAL	factor = OVER_ONE_AS_REAL( _max - _min );
	p_f = _samples;
	for( INT32 i = get_sample_nb(); i > 0; --i )
	{
		*p_f = (*p_f - _min ) * factor;
		if ( *p_f > .8 )
			PRINT_STRING( "%f ", (DOUBLE)*p_f );
		++p_f;
	}
	_min = 0.;
	_max = 1.;
}

/*
//	is the original code
REAL	c_pertub::get(register	REAL f_in)
{
register	REAL	*p_f;
register	INT32	i_floor;

	f_in *= f_value_max;
	f_in = FMOD(f_in,f_value_max);

	i_floor = I_FLOOR(f_in);
	p_f = values + i_floor;
	f_in -= i_floor;

	return *p_f*( 1. - f_in) + *(p_f+1)*f_in;
}
*/





