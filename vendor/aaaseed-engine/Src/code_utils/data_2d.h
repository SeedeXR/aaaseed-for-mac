
#ifdef AAA_DATA_2D_H
#error "DATA_2D_H included more than once."
#endif
#define AAA_DATA_2D_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef AAA_RAND_H
#	include "math/rand.h"
#endif
#ifndef AAA_AAA_UTIL_H
#	include "aaa_util.h"
#endif
#ifndef AAA_ERR_H
#	include "err.h"
#endif
#ifndef _NEW_
#	include <new>	//	std::bad_alloc
#endif

template <class T>
class	c_data_2d final : public c_obj
{
private:
	INT32	_nb_x;
	INT32	_nb_y;
	T*		_data;

public:
	c_data_2d<T>( INT32 nb_x_in=0, INT32 nb_y_in=0 );
	virtual ~c_data_2d<T>();

//	void	init();
	T*		alloc( INT32 nb_x_in, INT32 nb_y_in );
	void	dealloc();

	void	clear_with( T value);
	void	fill_random( T value, INT32 nb, INT32 seed );

//	AAA_ERR	read();
//	AAA_ERR	read_data(CHAR* dst_in);
//	AAA_ERR	save_data();

	FINLINE	T*	get_data_pt( INT32 x, INT32 y )
	{
		//	Wrap logic below handles only ONE step out of range per axis
		//	(single add or single subtract). Valid inputs are:
		//		x in [-_nb_x, 2*_nb_x - 1]
		//		y in [-_nb_y, 2*_nb_y - 1]
		//	Anything further wraps incorrectly and the final pointer lands
		//	outside the buffer (UB). Current callers (cellular automata
		//	neighbor stepping) only move by +/-1 per access, so this narrow
		//	range is adequate -- a full modulo would be ~3x the cost on the
		//	hot path. The debug-only check below catches misuse that breaks
		//	the contract; if a new caller needs broader inputs, either pre-
		//	modulo the arguments or swap the wrap code for a real modulo.
	#if AAA_DEBUG()
		if( x < -_nb_x || x >= 2*_nb_x || y < -_nb_y || y >= 2*_nb_y )
			debug_break( "%s(%d,%d) outside one-step wrap range for %dx%d grid",
					__FUNCTION__, x,y, _nb_x,_nb_y );
	#endif
		if( y<0 )
			y += _nb_y;
		else if( y >= _nb_y )
			y -= _nb_y;
		if( x<0 )
			x += _nb_x;
		else if( x >= _nb_x )
			x -= _nb_x;
		return _data + (y*_nb_x+x );
	}

	FINLINE	void	set( INT32 x, INT32 y, T r )
	{
		*get_data_pt( x,y ) = r;
	}
};

template <class T>
c_data_2d<T>::c_data_2d( INT32 nb_x_in, INT32 nb_y_in )
{
	_nb_x = 0;
	_nb_y = 0;
	_data = nullptr;
	alloc( nb_x_in, nb_y_in );
}

template <class T>
c_data_2d<T>::~c_data_2d()
{
	dealloc();
}

template <class T>
void c_data_2d<T>::dealloc()
{
	SAFE_DELETE_ARRAY( _data );
}


//	void	init();
template <class T>
T*	c_data_2d<T>::alloc( INT32 nb_x_in, INT32 nb_y_in )
{
	//	Fast path: requested dims already match stored state -- skip the
	//	clamp / overflow checks entirely. Values stored in _nb_x/_nb_y are
	//	always post-clamp so this guard is safe.
	if( _nb_x != nb_x_in || _nb_y != nb_y_in )
	{
		//	Clamp each dim to a minimum of 1. A zero or negative dim means a
		//	degenerate grid on which set()/get_data_pt() would wrap-divide or
		//	return stale data. Negative + size_t cast at new[] would also wrap
		//	to a huge positive value and blow past any sane allocation limit.
		if( nb_x_in < 1 || nb_y_in < 1 )
		{
			ERR_PRINT_STRING( "%s() received non-positive dimension(s) %d x %d, clamping to 1",
					__FUNCTION__, nb_x_in, nb_y_in );
			if( nb_x_in < 1 )
				nb_x_in = 1;
			if( nb_y_in < 1 )
				nb_y_in = 1;
		}

		//	Detect INT32 overflow on the element count and clamp nb_y so the
		//	product fits. Done in INT64 to sidestep the signed-overflow UB
		//	that the raw INT32 multiply at new[] would hit. nb_x_in is >= 1
		//	post-clamp so the division is always safe.
		if( INT64(nb_x_in) * INT64(nb_y_in) > INT64(aaa::BIGGEST<INT32>) )
		{
			INT32 CONST nb_y_max = aaa::BIGGEST<INT32> / nb_x_in;
			ERR_PRINT_STRING( "%s() %d x %d overflows INT32 element count, clamping nb_y to %d",
					__FUNCTION__, nb_x_in, nb_y_in, nb_y_max );
			nb_y_in = nb_y_max;
		}

		//	Re-check after clamping: caller may have passed bogus values that
		//	collapse to the stored state, in which case we must not realloc
		//	(would lose the existing contents).
		if( _nb_x != nb_x_in || _nb_y != nb_y_in )
		{
			T* new_data = nullptr;
			try
			{
				new_data = new T[size_t(nb_x_in) * size_t(nb_y_in)];
			}
			catch( std::bad_alloc const & )
			{
				//	On allocation failure we preserve the old _data/_nb_x/_nb_y
				//	(strong exception guarantee) and return the existing pointer
				//	so the caller can keep working with the previous contents.
				ERR_PRINT_STRING( "%s() failed to allocate %d x %d elements of size %zu",
						__FUNCTION__, nb_x_in, nb_y_in, sizeof(T) );
				return _data;
			}
			dealloc();
			_data = new_data;
			_nb_x = nb_x_in;
			_nb_y = nb_y_in;
			clear_with( T() );
		}
	}
	return _data;
}

template <class T>
void	c_data_2d<T>::clear_with( T value )
{
	INT32	nb;
	T*		pt;
	nb = _nb_x*_nb_y+1;
	pt = _data-1;
	while( --nb>0)
		*++pt = value;
}

template <class T>
void	c_data_2d<T>::fill_random( T value, INT32 nb, INT32 seed )
{
	INT32 CONST total = _nb_x * _nb_y;
	if( total <= 0 )
		return;	//	empty grid -- nothing to fill, and guards IMOD against div-by-zero.
	c_rand_lin	rnd;
	rnd.set_seed( seed );
	++nb;
	while( --nb>0 )
	{
		//	Decompose linear index t (row-major, _nb_x columns):
		//	  x = t % _nb_x   (column)
		//	  y = t / _nb_x   (row)
		//	set( x, y, value ) -- previous code had x/y swapped which biased
		//	the distribution on non-square grids.
		INT32 CONST t = IMOD( rnd.get_uint32(), total );
		set	(	IMOD( t, _nb_x ),
				t / _nb_x,
				value
			);
	}
}

