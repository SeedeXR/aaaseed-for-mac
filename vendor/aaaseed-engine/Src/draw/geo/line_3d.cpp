#include "line_3d.h"
#include "math/v.h"


c_line_3d::c_line_3d()
:_len(						.0f		)
,_controls(					nullptr	)
,_controls_nb(				0		)
,_controls_alloc_nb(		0		)
,_controls_dist_next(		nullptr	)
,_controls_len(				nullptr	)
//_b_compute_needed(		true	)
//,_b_just_changed(			false	)
//_b_valid_curve(			false	)
//_b_valid_data(			false	)
//,_b_loop_ui(				false	)
{
}

c_line_3d::~c_line_3d()
{
	dealloc();
//	dealloc_curve();
}

void	c_line_3d::clear()
{
	_controls_nb = 0;
//	_b_valid_data = false;
//	_b_valid_curve = false;
//	clear_curve();
}

void	c_line_3d::dealloc()
{
	clear();
	IF_FREE_ALIGNED_AND_NULL( _controls				);
	IF_FREE_ALIGNED_AND_NULL( _controls_dist_next	);
	IF_FREE_ALIGNED_AND_NULL( _controls_len			);
}

//void	c_line_3d::update_curve_valid()
//{
//	_b_valid_curve =  _controls_nb >= 2;
//}

void	c_line_3d::realloc( UINT32 CONST nb, bool CONST b_clear )
{
	if( nb > _controls_alloc_nb )
	{
		UINT32 nb_alloc = (nb + 31) & 0xffffffe0;
		_controls			= (REAL*)	REALLOC_ALIGNED( (void*)_controls,				nb_alloc		* sizeof(REAL)*3	);
		_controls_dist_next	= (REAL*)	REALLOC_ALIGNED( (void*)_controls_dist_next,	nb_alloc		* sizeof(REAL)		);
		_controls_len		= (DOUBLE*)	REALLOC_ALIGNED( (void*)_controls_len,			nb_alloc		* sizeof(DOUBLE)	);
		if( _controls && _controls_dist_next && _controls_len )
		{
			REAL* pt = _controls + _controls_alloc_nb * 3;
			for( UINT32 i=_controls_alloc_nb; i < nb_alloc; ++i )
			{ 
				clear_v3( pt );
				pt += 3;
			}
			_controls_alloc_nb = nb_alloc;
			_len = .0f;
		}
		else
		{
			dealloc();
			ERR_PRINT_STRING( "%s() can't allocate control points", __FUNCTION__ );
		}
	}

	if( _controls )
	{
//		_controls_nb = nb;
//		update_curve_valid();
		if( b_clear )
		{
			clear_vr( _controls,			nb*3	);
			clear_vr( _controls_dist_next,	nb		);
			clear_vd( _controls_len,		nb		);
			_len = .0f;
		}
	}
	//else
	//	_b_valid_curve = false;
}


REAL*		c_line_3d::get_control_point( INT32 index )
{
	if( _controls_nb <= 0 )
	{
		debug_break( "%s() no control points", __FUNCTION__ );
		return nullptr;
	}
	//if( _b_loop )
	//	index = WRAP_ID( index, (INT32)(_controls_nb - 1) );
	//else
	if( index < 0 || (INT32)_controls_nb <= index )
	{
		debug_break( "%s() control points index %d don't exist", __FUNCTION__, index );
		return nullptr;
	}
	return _controls + index*3;
}

void		c_line_3d::set_control_point( INT32 index, REAL CONST * CONST vec )
{
	REAL* pt = get_control_point( index );
	if( pt )
		cpy_v3( pt, vec );
}

void	c_line_3d::push_control_point( REAL CONST * CONST src )
{
	// add a control point at the end
	realloc( _controls_nb + 1 );
	REAL* dst = &_controls[ _controls_nb*3 ];
	cpy_v3( dst, src ? src : zero_v4fp32 );
	_controls_dist_next[_controls_nb] = 0.;
	++_controls_nb;
	if( _controls_nb >= 2 )
	{
		REAL d = dist_v3r(dst, dst - 3);
		_controls_dist_next[_controls_nb-2] = d;
		if( d > 1. )
			pop_control_point( _controls_nb - 1 );
		else
			compute_len();
	}
	else
		compute_len();
}

//void	c_line_3d::insert_control_point( UINT32 index, REAL CONST * CONST src )
//{
//	index = CLAMP( index + 1, (UINT32)0, _controls_nb-1);
//	if( _controls_nb == 0 || index == _controls_nb - 1 )
//	{
//		add_control_point( src );
//		return;
//	}
//	// insert
//	realloc( _controls_nb + 1 );
//	//todo MEMMOVE
//	for( UINT32 i = _controls_nb - 1; i > index; --i )
//		cpy_v3( get_control_point(i), get_control_point(i-1) );
//	REAL* dst =  get_control_point( index );
//	if( src )
//		cpy_v3( dst, src );
//	else
//		add_then_scale_v3r( dst, get_control_point(index-1), get_control_point(index+1), .5f );	
//	ask_update_points();
//}

//todo add pop with an arg of nb to pop
void	c_line_3d::pop_control_point( INT32 nb_to_remove )
{
	INT32 nb = _controls_nb;
	nb_to_remove = MIN( nb_to_remove, nb );
	if( nb_to_remove > 0 )
	{
		INT32 nb_to_move = nb - nb_to_remove;
		UINT8* dst = (UINT8*) &_controls[0];
		MEMMOVE( dst, dst + sizeof(REAL) * 3 * nb_to_remove,	nb_to_move * sizeof(REAL) * 3 );
		dst = (UINT8*) &_controls_dist_next[0];
		MEMMOVE( dst, dst + sizeof(REAL) * nb_to_remove,		nb_to_move * sizeof(REAL) );
		_controls_nb = nb - nb_to_remove;
	}
	compute_len();
//	update_curve_valid();
//	ask_update_points();
}


//void	c_line_3d::delete_control_point( UINT32 CONST index )
//{
//	if( index >= _controls_nb )
//		return;
//#if 1	//this is not really faster and a little dangerous perhaps have to be refined
//	INT32 nb = _controls_nb - 1 - index;
//	if( nb > 0 )
//	{
//		UINT8* dst = (UINT8*) &_controls[ index ];
//		MEMMOVE( dst, dst + sizeof(REAL)*3, nb * sizeof( REAL)*3 );
//	}
//#else
//	for ( UINT32 i = index; i < (_controls_nb - 1); ++i )
//		cpy_v3( get_control_point(i), get_control_point(i+1) );
//#endif
//	--_controls_nb;
//	update_curve_valid();
//	ask_update_points();
//}

//DOUBLE	c_line_3d::compute_len()
//{
//	_len = .0f;
//	if( _controls_nb > 1 )
//	{
//		_controls_len[ 0 ] = 0.;
//		REAL*	prev = _controls;
//		REAL*	next = _controls + 3;
//		for( UINT32 i = 1; i < _controls_nb; ++i )
//		{
//			REAL dist = dist_v3r( prev, next );
//			_controls_dist_next[ i-1 ] = dist;
//			_len += dist;
//			_controls_len[ i ] = _len;
//			prev = next;
//			next += 3;
//		}
//	}
//	return _len;
//}

DOUBLE	c_line_3d::compute_len()
{
	_len = .0f;
	if( _controls_nb > 1 )
	{
		_controls_len[ 0 ] = 0.;
		for( UINT32 i = 1; i < _controls_nb; ++i )
		{
			_len += _controls_dist_next[ i-1 ];
			_controls_len[ i ] = _len;
		}
	}
	return _len;
}

//void	c_line_3d::update()
//{
//	if( _controls_nb > 0 )
//	{
//		if( _b_compute_needed )	//this a special case of compute_check to hack _b_just_changed state
//		{
//			compute_curve();
//			return;
//		}
//	}
//	_b_just_changed = false;
//}

//void	c_line_3d::set_control_points( REAL CONST * CONST points, UINT32 CONST nb )
//{
//	realloc( nb );
//	if( _controls_nb > 0 )
//	{
//		MEMCPY( _controls, points, nb * sizeof(REAL)*3, __FUNCTION__ );
//		ask_update_points();
//	}
//}
//
//void	c_line_3d::set_control_point( UINT32 CONST index, REAL CONST * CONST src )
//{
//	if( !src )
//		return;
//	REAL*	pt = get_control_point( index );
//	if( pt )
//	{
//		cpy_v3( pt, src );
//		ask_update_points();
//	}
//}

//void	c_line_3d::set_loop( bool CONST b_loop )
//{
//	if( _b_loop != b_loop )
//	{
//		_b_loop = b_loop;
////		ask_update_points();
//	}
//}

//void	c_line_3d::compute_curve()
//{
//	if( !_b_compute_needed )
//		return;
//	//if( _controls_nb <= 0 )
//	//{
//	//	_points_nb = 0;
//	//	return;
//	//}
//
//	compute_len();
//	clear_update_point();
//}

//UINT32	c_line_3d::get_point_curve_nb()
//{
////	compute_check();
//	return _controls_nb;	//todo move linear part to the _points_arc_nb logic
//}
//
//REAL*	c_line_3d::get_point_curve( UINT32 index )
//{
////maa these tests slow down but where perhaps more secure
////	if( _control_points_nb > 0 )
////	{
////		compute_check();
////		if( _points )
//			{
//				if( index < 0 )
//					index = 0;
//				else if( _controls_nb <= index )
//					index = _controls_nb - 1;
//				return &_controls[ index*3 ];
//			}
////	}
////	return nullptr;
//}
//
//REAL*	c_line_3d::get_point_curve_pt()
//{
//	if( _controls_nb > 0 )
//	{
////		compute_check();
//		return _controls;
//	}
//	return nullptr;
//}


bool	c_line_3d::find_index( DOUBLE& s, INT32& i_prev, INT32& i_next )
{
//	if( _b_loop )	s = FWRAP( s, 1.f );
//	else
	if( _controls_nb >= 2 )
	{
		CLAMP_REF_01( s );
		s *= _len;
		//good old dicothomy
		INT32	min		=	0;
		INT32	max		=	_controls_nb - 1;
		while( max-min > 1 )
		{	
			INT32	center = ( max + min ) >> 1;
			if( s < _controls_len[center] )
				max = center;
			else
				min = center;
		}
		i_prev = min;
		i_next = max;
		// now we compute the interpolation factor between the two pointd
		DOUBLE prev = _controls_len[min];
		s = ( s - prev ) / ( _controls_len[max] - prev );
		return true;
	}
	if( _controls_nb == 1 )
	{
		i_prev = 0;
		i_next = 0;
		s = 0;
		return true;
	}
	return false;
}


void	c_line_3d::get_tangent( REAL* dst, DOUBLE s )
{
	INT32	i_prev;
	INT32	i_next;
	if( find_index( s, i_prev, i_next ) )
	{
		sub_v3( dst, _controls+i_next*3, _controls+i_prev*3 );
		normalize_v3r( dst );
	}
	else
		clear_v3( dst );
}

void	c_line_3d::get_point( REAL* dst, DOUBLE s )
{
	INT32	i_prev;
	INT32	i_next;
	if( find_index( s, i_prev, i_next ) )
		interpolate_v3( dst, _controls+i_prev*3, _controls+i_next*3, s );
	else
		clear_v3( dst );
}

void	c_line_3d::get_point_tangent( REAL* dst, REAL* tgn, DOUBLE s )
{
//todo do directly both
		get_point( dst, s );		
		get_tangent( tgn, s );
}
