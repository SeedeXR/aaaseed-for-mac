
#ifdef AAA_POLYLINE_H
#error "POLYLINE_H included more than once."
#endif
#define AAA_POLYLINE_H 1


#ifndef AAA_AAA_MEM_H
#	include "aaa_mem.h"
#endif
#ifndef _VECTOR_
#	include <vector>
#endif
#ifndef	AAA_LAYER_H
#	include "infrastructure/layer/layer.h"
#endif
#ifndef AAA_BDD_UTIL_H
#	include "bdd_util.h"
#endif
#ifndef AAA_V_H
#	include "math/v.h"
#endif

//perhaps make in more opengl type (no struct) to ease rest of code
typedef	struct	st_point_2d
{
	REAL	x;
	REAL	y;
}	st_point_2d;

typedef	struct	st_point_3d
{
	REAL	x;
	REAL	y;
	REAL	z;
}	st_point_3d;

template<class T>
class	c_polyline
{
private:
	std::vector<T>	_points;
	bool			_b_closed;
	REAL			_strip_size_half;
	REAL*			_triangle_strip;
	REAL*			_uv;
	//REAL			_pos_max_x;
	//REAL			_pos_max_y;

private:
			bool			alloc_uv();
			bool			alloc_strip();
			void			build_strip( bool CONST b_constant_width );

public:
	FINLINE	c_polyline();
			~c_polyline();

			//	copy constructor
			c_polyline( CONST c_polyline& obj )
			{
				debug_break( "c_polyline copy constructor unimplemented" );
				//set( obj.data, obj.len, obj.line_nb );
			}
			//	copy assignment operator
			c_polyline& operator=( CONST c_polyline& src )
			{
				if( this == &src )
					return *this;
				//debug_break( "c_param copy assignment operator unimplemented" );
				//set( obj.data, obj.len, obj.line_nb );
				//avoid duplication and then double free of buffer
				_points = src._points;
				_b_closed = src._b_closed;
				_strip_size_half = src._strip_size_half;
				_triangle_strip = nullptr;
				_uv = nullptr;
				return *this;
			}


			FINLINE	void		clear();

			void		scale_translate( REAL CONST * CONST scale, REAL CONST * CONST translate );

	FINLINE	void		add_point(			REAL CONST x, REAL CONST y );
	FINLINE	INT32		get_point_nb()		CONST							{ return (INT32)_points.size(); }
	FINLINE	T CONST		get_point(			INT32 CONST index ) CONST		{ return _points[index]; }
	FINLINE	T CONST	&	get_point_const(	INT32 CONST index ) CONST		{ return _points[index]; }
	FINLINE	void		pop_point()											{ return _points.pop_back(); }

	FINLINE	bool		is_closed()	CONST									{ return _b_closed; }
	FINLINE	void		set_closed(			bool CONST b )					{ _b_closed = b; }
			
	FINLINE	void		set_width(			REAL CONST in )					{ _strip_size_half = REAL( in*.5 ); }

			void		draw_line() CONST;
			void		draw_quad(			bool CONST b_constant_width = false ); 

			void		find_max_y(			REAL& x, REAL& y );
			void		find_max_y_average( REAL& x, REAL& y, REAL d );
};

template<class T>
FINLINE	c_polyline<T>::c_polyline() :
	_b_closed(false),
	_triangle_strip(nullptr),
	_uv(nullptr),
	_strip_size_half( REAL(.001) )
{
}

template<class T>
c_polyline<T>::~c_polyline()
{
	IF_FREE_AND_NULL( _triangle_strip );
	IF_FREE_AND_NULL( _uv );
	clear();
}

template<class T>
FINLINE	void	c_polyline<T>::clear()
{
	_points.clear();
}

template<class T>
FINLINE	void	c_polyline<T>::add_point( REAL CONST x, REAL CONST y )
{
	st_point_2d	pt={x,y};
	_points.push_back( pt );
}

template<class T>
FINLINE	bool	c_polyline<T>::alloc_uv()
{
	_uv = (REAL*) REALLOC( _uv, (sizeof(REAL)*2) * 2 * (get_point_nb()+1) );	// quad strip have 2 pt with 2 coors by original point
																				//	one more for closed ones
	return _uv != nullptr;
}


template<class T>
FINLINE	void	c_polyline<T>::build_strip( bool CONST b_constant_width )
{
	if( !alloc_strip() )
		return;

	INT32 nb = get_point_nb();
	INT32 nb_minus_1 = nb-1;
//	if( nb_minus_1 < 1 )
//		return;

	for( INT32 i=1; i<nb_minus_1; ++i )
		compute_point( _triangle_strip+4*i,				&_points[i-1],			&_points[i],		  &_points[i+1],		_strip_size_half, b_constant_width );
	if( _b_closed )
	{
		compute_point( _triangle_strip,					&_points[nb_minus_1],   &_points[0],          &_points[1],			_strip_size_half, b_constant_width );
		//todo copy should be enough
		compute_point( _triangle_strip+(4*nb),			&_points[nb_minus_1],   &_points[0],          &_points[1],			_strip_size_half, b_constant_width );
		compute_point( _triangle_strip+(4*nb_minus_1),	&_points[nb_minus_1-1], &_points[nb_minus_1], &_points[0],			_strip_size_half, b_constant_width );
	}																																		  
	else																																	  
	{																																		  
		compute_point( _triangle_strip,			       &_points[0],			   &_points[0],			 &_points[1],			_strip_size_half, b_constant_width );
		compute_point( _triangle_strip+(4*nb_minus_1), &_points[nb_minus_1-1], &_points[nb_minus_1], &_points[nb_minus_1],	_strip_size_half, b_constant_width );
	}

	if( c_layer::get_cur()->is_need_uv() )
	{
		bdd_util_build_uv( _uv, nb, 2, _b_closed, false );
		if( _b_closed )
			cpy_v2( _uv + 2*nb, _uv );
	}
}

template<class T>
FINLINE	void	c_polyline<T>::find_max_y( REAL& x, REAL& y )
{
	x=0.5;
	y=0;

	INT32	i = get_point_nb()-1;

	for( ; i>=0; --i )
	{
		if( y < _points[i].y )
		{
			y = _points[i].y;
			x = _points[i].x;
		}
	}
}

template<class T>
FINLINE	void	c_polyline<T>::find_max_y_average( REAL& x, REAL& y, REAL d )
{
	REAL	y_max = 0;
	INT32	nb = get_point_nb()-1;
	for( INT32 i=nb; i>=0; --i )
	{
		if( y_max < _points[i].y )
			y_max = _points[i].y;
	}

	x = 0;
	y = 0;
	INT32 nb_used = 0;
	y_max -= MAX0( d );
	for( INT32 i=nb; i>=0; --i )
	{
		if( y_max <= _points[i].y )
		{
			++nb_used;
			y += _points[i].y;
			x += _points[i].x;
		}
	}

	REAL over = REAL( 1. / nb_used );

	x *= over;
	y *= over;
}

typedef	c_polyline<st_point_3d>	c_polyline_3d;
typedef	c_polyline<st_point_2d>	c_polyline_2d;

