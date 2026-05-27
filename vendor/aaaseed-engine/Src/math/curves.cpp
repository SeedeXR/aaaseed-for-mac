#include "curves.h"
#include "v.h"
#include "file/file_csv.h"

//todo it is overprotected
//	acces to sevral point should be done with a check fn then a low level access

REAL CONST	c_curve_3d::DEFAULT_TIGHTNESS = 0.25;

//REAL	CoxDeBoor( REAL CONST u, INT32 CONST i, INT32 CONST k, REAL CONST * CONST Knots )
//{
//	if( k == 1 )
//	{
//		if( Knots[ i ] <= u && u <= Knots[ i + 1 ] )
//			return 1.0f;
//		return 0.0f;
//	}
//	REAL	Den1 = Knots[ i + k - 1 ] - Knots[ i ];
//	REAL	Den2 = Knots[ i + k ] - Knots[ i + 1 ];
//	REAL	Eq1 = .0f;
//	REAL	Eq2 = .0f;
//	if( Den1 > 0 )
//		Eq1 = ( ( u - Knots[ i ] ) / Den1 ) * CoxDeBoor( u, i, k - 1, Knots );
//	if( Den2 > 0 )
//		Eq2 = ( Knots[ i + k ] - u ) / Den2 * CoxDeBoor( u, i + 1, k - 1, Knots );
//	return Eq1 + Eq2;
//}


//------------------------------------------------------------	GetOutpoint()
//
//void	c_bdd_curve_3d::get_point_nurbs( REAL t, REAL* OutPoint, REAL CONST* knots )
//{

//// sum the effect of all CV's on the curve at this point to 
//// get the evaluated curve point
//for( UINT32 i = 0; i != _nb_points[ _dataset_id -1 ]; v )
//{
//	// calculate the effect of this point on the curve
//	REAL	Val = CoxDeBoor( t, i, 4, knots );

//	REAL*	pt = _controls_draw->get_point( i, 0 );
//	if( Val > 0.001f )
//	{
//		// sum effect of CV on this part of the curve
//		OutPoint[0] += Val * pt[0];
//		OutPoint[1] += Val * pt[1];
//		OutPoint[2] += Val * pt[2];
//	}
//}
//}

//if( _controls_draw && _controls_draw->get_nb_u() > 0 )
//{
//	bool	b_catmull = false;
//	if( _s_curve_type == c_curve_3d::CURVE_CATMULLROM && _controls_draw->get_nb_u() >= 4 )
//	{
//		b_catmull = true;
//	}
//	if( j == _dataset_id )
//	{
//		set_color_ui_selected( 1.0f );
//	}
//	else
//	{
//		set_color_white( 1.0f );
//	}

//	GOL::begin( GL_LINE_STRIP );
//		if( b_catmull )
//		{
//			REAL	tmp[3];
//			GOL::vertex3v( get_point_draw( 0 ) );
//			// no catmull rom for first and last point
//			for( INT32 i = 0; i < _controls_draw->get_nb_u()-3; ++i )
//			{
//				REAL	t;
//				for( UINT32 k = 0; k <= _curve_lod[ j ]; ++k )
//				{
//					t = (REAL)k / (REAL)_curve_lod[ j ];
//					cvCatmullRom_3r( tmp, get_point_draw( i ), get_point_draw( i+1 ), get_point_draw( i+2 ), get_point_draw( i+3 ), t );
//					GOL::vertex3v( tmp );
//				}
//			}
//			GOL::vertex3v( get_point_draw( _controls->get_nb_u()-1 ) );
//		}
//		else if( _s_curve_type == c_curve_3d::CURVE_CUBIC_B_SPLINE )
//		{
//			for( INT32 start_cv = -3,k = 0; k != _nb_points[ j-1 ] + 3; ++k,++start_cv )
//			{
//				REAL	t;
//				for( INT32 l = 0; l <= _curve_lod[ j ]; ++l )
//			//	for( REAL t = .0f; t <= 1.0f; t += .1f)
//			//	for( INT32 i = 0; i != LOD; ++i )
//				{
//					// use the parametric time value 0 to 1 for this curve segment.
//				//	REAL	t = (REAL)i/LOD;
//					t = (REAL)l / (REAL)_curve_lod[ j ];
//					// the t value inverted
//					REAL	it = 1.0f - t;

//					// calculate blending functions for cubic bspline
//					REAL	t_2 = t * t;
//					REAL	t_3 = t_2 * t;
//					REAL	b0 = it * it * it / 6.0f;
//					REAL	b1 = ( 3 * t_3 - 6 * t_2 + 4 ) / 6.0f;
//					REAL	b2 = ( -3 * t_3 +3 * t_2 + 3 * t + 1 ) / 6.0f;
//					REAL	b3 =  t_3/6.0f;

//					REAL	x, y, z;
//					// calculate the x,y and z of the curve point
//					x =	b0 * get_point_spline( start_cv + 0 )[0] +
//						b1 * get_point_spline( start_cv + 1 )[0] +
//						b2 * get_point_spline( start_cv + 2 )[0] +
//						b3 * get_point_spline( start_cv + 3 )[0] ;

//					y =	b0 * get_point_spline( start_cv + 0 )[1] +
//						b1 * get_point_spline( start_cv + 1 )[1] +
//						b2 * get_point_spline( start_cv + 2 )[1] +
//						b3 * get_point_spline( start_cv + 3 )[1] ;

//					z =	b0 * get_point_spline( start_cv + 0 )[2] +
//						b1 * get_point_spline( start_cv + 1 )[2] +
//						b2 * get_point_spline( start_cv + 2 )[2] +
//						b3 * get_point_spline( start_cv + 3 )[2] ;

//					GOL::vertex3( x,y,z );
//					// specify the point
//				}
//			}

//		}
//		//else if( _s_curve_type == CURVE_NURBS )
//		//{
//		//	INT32	LOD = 20;
//		//	INT32	knots_nb = _nb_points[ _dataset_id -1 ] + 4;
//		//	REAL*	knots = (REAL*)MALLOC( (_nb_points[ _dataset_id -1 ] + 4 )* sizeof(REAL) );
//		//	
//		//	for( UINT32 i = 0; i < _nb_points[ _dataset_id -1 ]; ++i )
//		//	{
//		//		knots[ i ] = 1.0;
//		//	}
//		//	for( UINT32 i = _nb_points[ _dataset_id -1 ]; i < knots_nb; ++i )
//		//	{
//		//		knots[ i ] = (REAL)i + 1.f;
//		//	}
//		//	-
//		//	for( INT32 i=0; i!=LOD; ++i )
//		//	{
//		//		float t  = knots[knots_nb-1] * i / (float)(LOD-1);

//		//		if(i==LOD-1) 
//		//			t-=0.001f;

//		//		float Outpoint[3]={0,0,0};

//		//		get_point_nurbs( t, Outpoint, knots );

//		//		GOL::vertex3v( Outpoint );
//		//	}

//		//}
//		else
//		{
//			UINT32	points_nb = _curve->get_points_lod_nb();
//			REAL*	pt = _curve->get_points_lod();
//			// todo use curve data, and use buffer array to speed up display
//			for( INT32 i = 0; i < points_nb; ++i )
//			{
//				GOL::vertex3v( pt );
//				pt += 3;
//			}
//		}
//	GOL::end();
//}
//}


c_curve_3d::c_curve_3d()
:_b_compute_needed(			true	)
,_b_just_changed(			false	)
,_b_valid_curve(			false	)
,_b_valid_data(				false	)

,_len(						.0f		)

,_controls(					nullptr	)
,_controls_nb(				0		)
,_controls_alloc_nb(		0		)
,_controls_arc_len(			nullptr	)

,_points(					nullptr	)
,_points_nb(				0		)
,_points_alloc_nb(			0		)
,_points_arc_len(			nullptr	)
,_points_arc_nb(			2		)	//hack _points_arc_nb(0) fuck up first curve in eo

,_b_loop(					false	)
,_s_curve_type(				CURVE_LINEAR	)
,_lod(						DEFAULT_LOD		)

{
}

c_curve_3d::~c_curve_3d()
{
	dealloc();
	dealloc_curve();
}

void	c_curve_3d::clear()
{
	_controls_nb = 0;
	_b_valid_data = false;
	_b_valid_curve = false;
	_len = .0f;
	clear_curve();
}

void	c_curve_3d::dealloc()
{
	clear();
	IF_FREE_ALIGNED_AND_NULL( _controls );
	IF_FREE_ALIGNED_AND_NULL( _controls_arc_len );
	_controls_alloc_nb = 0;
}

void	c_curve_3d::update_curve_valid()
{
	switch( _s_curve_type )
	{
	case CURVE_LINEAR:		_b_valid_curve =  _controls_nb >= 2 ;		break;
	default:
	case CURVE_CATMULLROM:	_b_valid_curve =  _controls_nb >= 4 ;		break;
	}

}

void	c_curve_3d::realloc( UINT32 CONST nb, bool CONST b_clear )
{
	if( nb > _controls_alloc_nb )
	{
		_controls				= (REAL*)	REALLOC_ALIGNED( (void*)_controls,				nb		* sizeof(REAL)	* 3	);
		_controls_arc_len		= (DOUBLE*)	REALLOC_ALIGNED( (void*)_controls_arc_len,		(nb+1)	* sizeof(DOUBLE)	);
		if( _controls && _controls_arc_len )
		{
			REAL* pt = _controls + _controls_alloc_nb*3;
			clear_vr( pt, (nb-_controls_alloc_nb)*3 );
			_controls_alloc_nb = nb;
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
		_controls_nb = nb;
		update_curve_valid();
		if( b_clear )
		{
			clear_vr( _controls,	nb*3 );
			clear_vd( _controls_arc_len,	nb + 1 );
			_len = .0f;
		}
	}
	else
		_b_valid_curve = false;
}

void	c_curve_3d::clear_curve()
{
	_points_alloc_nb = 0;
	_points_nb = 0;
	_points_arc_nb = 0;
	_b_valid_data = false;
}
void	c_curve_3d::dealloc_curve()
{
	IF_FREE_ALIGNED_AND_NULL( _points );
	IF_FREE_ALIGNED_AND_NULL( _points_arc_len );
	clear_curve();
}

void	c_curve_3d::alloc_curve( UINT32 CONST nb )
{
	if( nb <= _points_alloc_nb )
	{
		_points_nb = nb;
		_b_valid_data = true;
		return;
	}
	_points				= (REAL*)		REALLOC_ALIGNED( (void*)_points,			nb * sizeof(REAL)*3			);
	_points_arc_len		= (DOUBLE*)		REALLOC_ALIGNED( (void*)_points_arc_len,	( nb + 1 ) * sizeof(DOUBLE)	);
	if( _points && _points_arc_len )
	{
		_points_alloc_nb = nb; 
		_points_nb = nb;
		_b_valid_data = true;
	}
	else
	{
		ERR_PRINT_STRING( "%s() can't allocate points lod", __FUNCTION__ );
		dealloc_curve();
	}
}

void	c_curve_3d::write_in_file( C_PCHAR_C name )
{
	file_csv::write_real_grid_all( name, _controls, _controls_nb * 3, 1 );
}

void	c_curve_3d::read_from_file( C_PCHAR_C name )
{
	UINT32	x;
	UINT32	y;
	file_csv::count_float( name, x,y );
	if( y == 1 )
	{
		realloc( x / 3, true );
		file_csv::read_float_grid( name, _controls, x, 1 );
	}
}

void	c_curve_3d::add_control_point( REAL CONST * CONST src )
{
	// add a control point at the end
	realloc( _controls_nb + 1 );
	REAL* dst = get_control_point( _controls_nb - 1 );
	if( src )
		cpy_v3( dst, src );
	else
	{
		if( _controls_nb == 1 )
			clear_v3( dst );
		else
		{
			REAL	delta[3] = { .1f, .1f, .0f };
			add_v3( delta, get_control_point( _controls_nb - 2 ) );
			cpy_v3( dst, delta );
		}
	}
	ask_update_points();
}

void	c_curve_3d::insert_control_point( UINT32 index, REAL CONST * CONST src )
{
	index = CLAMP( index + 1, (UINT32)0, _controls_nb-1);
	if ( _controls_nb == 0 || index == _controls_nb - 1 )
	{
		add_control_point( src );
		return;
	}
	// insert
	realloc( _controls_nb + 1 );
	//todo MEMMOVE
	for ( UINT32 i = _controls_nb - 1; i > index; --i )
		cpy_v3( get_control_point(i), get_control_point(i-1) );
	REAL* dst =  get_control_point( index );
	if( src )
		cpy_v3( dst, src );
	else
		add_then_scale_v3r( dst, get_control_point(index-1), get_control_point(index+1), .5f );	
	ask_update_points();
}

void	c_curve_3d::delete_control_point( UINT32 CONST index )
{
	if ( index >= _controls_nb )
		return;
#if 1	//this is not really faster and a little dangerous perhaps have to be refined
	INT32 nb = _controls_nb - 1 - index;
	if( nb > 0 )
	{
		UINT8* dst = (UINT8*) &_controls[ index*3 ];
		CONSTEXPR INT32 size_one = sizeof(REAL)*3;
		MEMMOVE( dst, dst + size_one, nb * size_one );
	}
#else
	for ( UINT32 i = index; i < (_controls_nb - 1); ++i )
		cpy_v3( get_control_point(i), get_control_point(i+1) );
#endif
	--_controls_nb;
	update_curve_valid();
	ask_update_points();
}

DOUBLE	c_curve_3d::compute_len_linear()
{
	_len = .0f;
	if( _points_nb > 1 )
	{
		_points_arc_len[ 0 ] = _len;
		REAL*	prev = _points;
		REAL*	next = _points + 3;
		for( UINT32 i = 1; i < _points_nb; ++i )
		{
			_len += dist_v3r( prev, next );
			_points_arc_len[ i ] = _len;
			prev = next;
			next += 3;
		}
	}
	return _len;
}

void	c_curve_3d::update()
{
	if( _controls_nb > 0 )
	{
		if( _b_compute_needed )	//this a special case of compute_check to hack _b_just_changed state
		{
			compute_curve();
			return;
		}
	}
	_b_just_changed = false;
}

void	c_curve_3d::set_type( CURVE3D_TYPE CONST val )
{
	if( val != _s_curve_type )
	{
		_s_curve_type = val;
		update_curve_valid();
		ask_update_points();
	}
}

void	c_curve_3d::set_control_points( REAL CONST * CONST points, UINT32 CONST nb )
{
	realloc( nb );
	if( _controls_nb > 0 )
	{
		MEMCPY( _controls, points, nb * sizeof( REAL )*3, __FUNCTION__ );
		ask_update_points();
	}
}

void	c_curve_3d::set_control_point( UINT32 CONST index, REAL CONST * CONST src )
{
	if( !src )
		return;
	REAL* pt = get_control_point( index );
	if( pt )
	{
		cpy_v3( pt, src );
		ask_update_points();
	}
}

void	c_curve_3d::set_lod( UINT32 CONST val )
{
	if( _lod != val )
	{
		_lod = val;
		ask_update_points();
	}
}

void	c_curve_3d::set_loop( bool CONST b_loop )
{
	if( _b_loop != b_loop )
	{
		_b_loop = b_loop;
		ask_update_points();
	}
}

void	c_curve_3d::compute_curve_catmull()
{
	if( _controls_nb < 4 )
	{
		alloc_curve( _controls_nb );
		if( _points )
		{
			MEMCPY( _points, _controls, _controls_nb * sizeof(REAL)*3, __FUNCTION__ );
			clear_update_point();
		}
		return;
	}
	
	INT32 i;
	INT32 i_end;
	if( _b_loop )
	{
		i = 1;
		i_end = _controls_nb-1;
	}
	else
	{
		i = 1;
		i_end = _controls_nb-2;
	}
	alloc_curve( ( i_end-i ) * _lod + 1 + 2 );	// +2 is here because we add a point before and after to have the 4 point necessary to build at CatmullRom segment	

	if( !_points )
	{
		ERR_PRINT_STRING( "%s(), no curvepoints", __FUNCTION__ );
		return;
	}

	REAL	len = .0;
	REAL*	pt = _points + 3;
	REAL*	pt_prev;
	
	_points_arc_len[1] = .0f;
	_points_arc_nb = 2;

	_controls_arc_len[0] = .0f;
	_controls_arc_len_nb = 1;
	REAL over_lod = OVER_ONE_AS_REAL( _lod );
	cpy_v3( pt, get_control_point( i ) );
	for( ; i < i_end; ++i )
	{
		REAL* a = get_control_point( i - 1	);
		REAL* b = get_control_point( i		);
		REAL* c = get_control_point( i + 1	);
		REAL* d = get_control_point( i + 2	);
		for( UINT32 k = 1; k <= _lod; ++k )
		{
			pt_prev = pt;
			pt += 3;
			catmull_rom_3( pt, a,b,c,d, (REAL)k * over_lod );
			len += dist_v3r( pt, pt_prev );
			_points_arc_len[ _points_arc_nb ] = len;
			++_points_arc_nb;
		}
		_controls_arc_len[ _controls_arc_len_nb ] = len;
		++_controls_arc_len_nb;
	}
	_len = len;

	_points_arc_len[0] = -_points_arc_len[2];	//because arc_len[1] == 0.
	_points_arc_len[_points_arc_nb] = REAL( 2.*_points_arc_len[_points_arc_nb-1] ) - _points_arc_len[_points_arc_nb-2];
	if( _b_loop )
	{
		cpy_v3( _points,					_points+(_points_arc_nb-2)*3	);
		cpy_v3( _points+_points_arc_nb*3,	_points+2*3			);
	}
	else
	{
		sym_v3r( _points,					_points+3,						_points+2*3					);
		sym_v3r( _points+_points_arc_nb*3,	_points+(_points_arc_nb-1)*3,	_points+(_points_arc_nb-2)*3	);
	}
	++_points_arc_nb;

	clear_update_point();
}

void	c_curve_3d::compute_curve()
{
	if( !_b_compute_needed )
		return;
	if( _controls_nb <= 0 )
	{
		_points_nb = 0;
		return;
	}

	switch( _s_curve_type )
	{
	//case CURVE_CUBIC_B_SPLINE:
				//compute_curve_bspline();
				//break;
	case CURVE_CATMULLROM:
		compute_curve_catmull();
		break;
	case CURVE_LINEAR:
	default:
		// no interpolation needed, just copy points for linear curve
		alloc_curve( _b_loop ? (_controls_nb + 1) : _controls_nb );

		if( _points )
			MEMCPY( _points, _controls, _controls_nb * sizeof(REAL)*3, __FUNCTION__ );
		if( _b_loop )
			cpy_v3( _points+_controls_nb*3, get_control_point(0) );

		compute_len_linear();
		clear_update_point();
		break;
	}
}

UINT32	c_curve_3d::get_point_curve_nb()
{
	compute_check();
	return _points_nb;	//todo move linear part to the _points_arc_nb logic
}

REAL*	c_curve_3d::get_point_curve( UINT32 index )
{
//maa these tests slow down but where perhaps more secure
//	if( _control_points_nb > 0 )
//	{
//		compute_check();
//		if( _points )
		{
			if( index < 0 )
				index = 0;
			else if( _points_nb <= index )
				index = _points_nb - 1;
			return _points + index * 3;
		}
//	}
//	return nullptr;
}

REAL*	c_curve_3d::get_point_curve_pt()
{
	if( _controls_nb > 0 )
	{
		compute_check();
		return	_points;
	}
	return nullptr;
}


FINLINE	bool	c_curve_3d::find_arc_index( DOUBLE& s, INT32& i_next )
{
	if( _b_loop )
		s = FWRAP( s );
	else
		CLAMP_REF_01( s );
	s *= _len;
#if	0
	//todonow this not acceptable in terms of architecture to search this way
	for( UINT32 i = 2; i < _points_arc_nb-1; ++i )
	{
		if( _points_arc_len[i] >= s )
		{
			i_next = i;
			REAL prev = _points_arc_len[i-1];
			s = ( s - prev ) / ( _points_arc_len[i] - prev );
			return true;
		}
	}
	i_next = _points_arc_nb-2;
	s = 1;
	return true;
#else
	INT32	min		=	1;
	INT32	max		=	_points_arc_nb - 1;
	INT32	center;

	do
	{	
		center = ( max + min ) >> 1;
		if( s < _points_arc_len[center] )
			max = center;
		else
			min = center;
	}
	while( max-min > 1 );

	i_next = max;
	DOUBLE prev = _points_arc_len[max-1];
	s = ( s - prev ) / ( _points_arc_len[max] - prev );
	return true;
#endif
}

bool	c_curve_3d::find_index( DOUBLE& s, INT32& i_prev, INT32& i_next )
{
	s = CLAMP_01( s ) * _len;
#if	0
	//todonow this not acceptable in terms of architecture to search this way
	UINT32	index_end = _points_nb;

	for( UINT32 i = 1; i < index_end; ++i )
	{
		if( _points_arc_len[i] >= s )	//maa was > s
		{
			i_prev = i-1;
			i_next = i;
			REAL prev = _points_arc_len[i_prev];
			s = ( s - prev ) / ( _points_arc_len[i_next] - prev );
			return true;
		}
	}
	return false;
#else
	INT32	min		=	0;
	INT32	max		=	_points_nb - 1;

	while( max - min > 1 )
	{	
		INT32	center = ( max + min ) >> 1;
		if( s < _points_arc_len[center] )
			max = center;
		else
			min = center;
	}
	i_prev = min;
	i_next = max;
	DOUBLE prev = _points_arc_len[min];
	s = ( s - prev ) / ( _points_arc_len[max] - prev );
	return true;
#endif
}

FINLINE	void	c_curve_3d::get_point_catmull( REAL* dst, DOUBLE s )
{
	INT32	i_next;
	if( find_arc_index( s, i_next ) )
		catmull_rom_v3( dst, get_point_curve( i_next-2 ), (FP32)s );
}

FINLINE	void	c_curve_3d::get_tangent_catmull( REAL* dst, DOUBLE s )
{
	INT32	i_next;
	if( find_arc_index( s, i_next ) )
	{
		catmull_rom_derivative_v3( dst, get_point_curve( i_next-2 ), (REAL)s );
		normalize_v3r( dst );
	}
	else
		cpy_v3( dst, unit_z_v4fp32 );
}

FINLINE	void	c_curve_3d::get_point_tangent_catmull( REAL* dst, REAL* tgn, DOUBLE s )
{
	INT32	i_next;
	if( find_arc_index( s, i_next ) )
	{
		REAL*	pt = get_point_curve( i_next-2 );
		catmull_rom_v3( dst, pt, (FP32)s );
		catmull_rom_derivative_v3( tgn, pt, (REAL)s );
		normalize_v3r( tgn );
	}
	else
		cpy_v3( tgn, unit_z_v4fp32 );
}

FINLINE	void	c_curve_3d::get_point_linear( REAL* dst, DOUBLE s )
{
	INT32	i_prev;
	INT32	i_next;
	if( find_index( s, i_prev, i_next ) )
		interpolate_v3( dst, get_point_curve(i_prev), get_point_curve(i_next), s );
	else
	{
		//GOOD_PRINT_STRING( "Not found, s, real s : %f, %f", s, real_s );
	}
}

FINLINE	void	c_curve_3d::get_tangent_linear( REAL* dst, DOUBLE s )
{
	//	we want to avoid more test for speed reason
	//	no test is dangerous but the test should have be done outside and before
	//	if( is_curve_valid() )
	{
		INT32	i_prev;
		INT32	i_next;
		if( find_index( s, i_prev, i_next ) )
		{
			interpolate_v3( dst, get_point_curve(i_prev), get_point_curve(i_next), s );
			// find previous point
			if( s != .0 )
				sub_v3( dst, get_point_curve(i_prev) );
			else
			{
				if( i_prev > 1 )
					sub_v3( dst, get_point_curve(i_prev-1) );
				else
					sub_v3( dst, get_point_curve(i_next), dst );
			}
			normalize_v3r( dst );
			return;
		}
		else
		{
			//GOOD_PRINT_STRING( "Not found, s, real s : %f, %f", s, real_s );
			clear_v3( dst);
		}
	}
	clear_v3( dst );
}

void	c_curve_3d::get_tangent( REAL* dst, REAL CONST s )
{
	//	we want to avoid more test for speed reason
	//	no test is dangerous but the test should have be done outside and before
	//	if( is_curve_valid() )
	{
		//	now done is valid test because needed to be done before
		//		compute_check();	

		switch( _s_curve_type )
		{
		case CURVE_LINEAR:		get_tangent_linear(  dst, s );	return;
		case CURVE_CATMULLROM:	get_tangent_catmull( dst, s );	return;
		}
	}
	clear_v3( dst );
}

void	c_curve_3d::get_point( REAL* dst, REAL CONST s )
{
	//	we want to avoid more test for speed reason
	//	no test is dangerous but the test should have be done outside and before
	//	if( is_curve_valid() )
	{
		//	now done is valid test because needed to be done before
		//		compute_check();	
		switch( _s_curve_type )
		{
		case CURVE_LINEAR:		get_point_linear(  dst, s );	return;
		case CURVE_CATMULLROM:	get_point_catmull( dst, s );	return;
		}
	}
	clear_v3( dst );
}

void	c_curve_3d::get_point_tangent( REAL* dst, REAL* tgn, REAL CONST s )
{
	//	we want to avoid more test for speed reason
	//	no test is dangerous but the test should have be done outside and before
	//	if( is_curve_valid() )
	{
		//	now done is valid test because needed to be done before
		//		compute_check();	

		switch( _s_curve_type )
		{
		case CURVE_LINEAR:		get_point_linear( dst, s );		
								get_tangent_linear( tgn, s );				return;
		case CURVE_CATMULLROM:	get_point_tangent_catmull( dst, tgn, s );	return;
		}
	}
	clear_v3( dst );
}
