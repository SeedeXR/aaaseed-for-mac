
#ifdef AAA_TOUCHLIB_RECT2D_H
#error "TOUCHLIB_RECT2D_H included more than once."
#endif
#define AAA_TOUCHLIB_RECT2D_H 1



#ifndef AAA_TOUCHLIB_VECTOR2D_H
#	include "vector2d.h"
#endif
#ifndef AAA_AAA_MATH_H
#	include "math/aaa_math.h"
#endif

using namespace vector_2d;

namespace rect_2d
{

	// The following code was originally written by Nikolaus Gebhardt as part of Irrlicht.
	// See www.irrlicht3d.org

	// The Irrlicht Engine License
	// Copyright � 2002-2005 Nikolaus Gebhardt
	// This software is provided 'as-is', without any express or implied warranty. In no event will the authors be held 
	// liable for any damages arising from the use of this software.
	//
	// Permission is granted to anyone to use this software for any purpose, including commercial applications, and to 
	// alter it and redistribute it freely, subject to the following restrictions:
	//
	// 1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. 
	//    If you use this software in a product, an acknowledgment in the product documentation would be appreciated but 
	//    is not required.
	// 2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
	// 3. This notice may not be removed or altered from any source distribution.

	//!	Rectangle template.
	template <class T>
	class rect
	{
	private:
		vector2d<T> _tr;
		vector2d<T> _bl;
	public:

		//rect()
		//	: upper_left(0,0), lower_right(0,0) {}
		rect( T x=0, T y=0, T x2=0, T y2=0 )
			: _bl(x,y), _tr(x2,y2) {}
//		rect( CONST vector2d<T>& _ur, CONST vector2d<T>& _bl )
//			: _bl(bottom_left), _ur(upper_right) {}

		FINLINE	void	set_bottom_left( CONST T& x, CONST T& y)	{ _bl.set( x, y ); }
		FINLINE	void	set_top_right( CONST T& x, CONST T& y)		{ _tr.set( x, y ); }

		FINLINE	vector2d<T>		get_bottom_left()	{ return _bl; }
		FINLINE	vector2d<T>		get_top_right()		{ return _tr; }

		FINLINE void	add_point(T x, T y)
		{
			if( _tr.is_null() && _bl.is_null() )
			{
				_bl.set( x, y );
				_tr.set( x, y );
				return;
			}

			if( x < _bl.get_x() )	_bl.set_x( x );
			if( y < _bl.get_y() )	_bl.set_y( y );

			if( x > _tr.get_x() )	_tr.set_x( x );
			if( y > _tr.get_y() )	_tr.set_y( y );
		}

//		FINLINE void add_point( CONST vector2d<T>& pt )	//todo solve trouble with the CONST
		FINLINE void	add_point( vector2d<T>& pt )
		{
			add_point( pt.get_x(), pt.get_y() );
		}

		FINLINE void	reset( CONST vector2d<T> &pt )
		{
			_bl = pt;
			_tr = pt;
		}

/*		//! Returns if the rectangle collides with an other rectangle.
		FINLINE	bool is_rect_collided(CONST rect<T>& other) CONST
		{
			return	_lower_right.get_y() > other._upper_left.get_y()
				&&	_upper_left.get_y() < other._lower_right.get_y()
				&&	_lower_right.get_x() > other._upper_left.get_x()
				&&	_upper_left.get_x() < other._lower_right.get_x();
		}
*/
/*		//! Returns if the rectangle collides with an other rectangle.
		FINLINE bool is_box_inside(CONST rect<T>& other) CONST
		{
			return other._upper_left.get_y() >= _upper_left.get_y()
				&& other._upper_left.get_x() >= _upper_left.get_x()
				&& other._lower_right.get_y() <= _lower_right.get_y()
				&& other._lower_right.get_x() <= _lower_right.get_x();
		}
*/
		//! Returns width of rectangle.
		FINLINE T	get_size_x() CONST 	{	return _tr.get_x() - _bl.get_x(); }
		//! Returns height of rectangle.
		FINLINE T	get_size_y() CONST	{	return _tr.get_y() - _bl.get_y(); }
		 //todo CONST
		FINLINE T	get_area()	CONST	{	return get_size_x() * get_size_y();	}

		//! Returns center of rectangle in x.
		FINLINE T	get_center_x() CONST
		{
			return ( _bl.get_x() + _tr.get_x() ) * REAL(.5);
		}
		//! Returns center of rectangle in y.
		FINLINE T	get_center_y() CONST
		{
			return ( _bl.get_y() + _tr.get_y()) * REAL(.5);
		}
		//! Returns the center of the rectangle
		FINLINE vector2d<T>	get_center() CONST
		{
			return vector2d<T>( get_center_x(), get_center_y() );
		}
/*
		//! Clips this rectangle with another one.
		void clip_against(CONST rect<T>& other) 
		{
			if( other._lower_right.get_x() < _lower_right.get_x() )
				_lower_right.set_x( other._lower_right.get_x() );
			if( other._lower_right.get_y() < _lower_right.get_y() )
				_lower_right.set_y( other._lower_right.get_y() );

			if( other._upper_left.get_x() > _upper_left.get_x() )
				_upper_left.set_x( other._upper_left.get_x() );
			if( other._upper_left.get_y() > _upper_left.get_y() )
				_upper_left.set_y( other._upper_left.get_y() );

			// correct possible invalid rect resulting from clipping
			if( _upper_left.get_x() > _lower_right.get_x() )
				_upper_left.set_x( _lower_right.get_x() );
			if( _upper_left.get_y() > _lower_right.get_y() )
				_upper_left.set_y( _lower_right.get_y() );
		}
*/
		FINLINE	void	scale_translate( REAL* scale, REAL* translate )
		{
			_bl.set_x( translate[0] + _bl.get_x()*scale[0] );
			_bl.set_y( translate[1] + _bl.get_y()*scale[1] );
			_tr.set_x( translate[0] + _tr.get_x()*scale[0] );
			_tr.set_y( translate[1] + _tr.get_y()*scale[1] );
		}
		FINLINE REAL	get_proximity( T x, T y ) CONST
		{
			x = get_center_x() - x;
			y = get_center_y() - y;
			x /= get_size_x()*REAL(.5);
			y /= get_size_y()*REAL(.5);
			REAL d = x*x + y*y;
			if ( d>1.)
				return 0.;
			return REAL(1.)-SQRT(d);
		}
		//todoqqq this function does nothing intelligent
		FINLINE void	add_gradient_at( REAL* grad, T x, T y )
		{
			x = x - get_center_x();
			y = y - get_center_y();
			x /= get_size_x()*REAL(.5);
			y /= get_size_y()*REAL(.5);
			REAL d = x*x + y*y;
			if ( d>1.)
				return;
		}
		FINLINE bool	is_point_inside( CONST T x, CONST T y ) CONST
		{
			return	_bl.get_x()	<= x
				&&	_bl.get_y()	<= y
				&&	x <= _tr.get_x()
				&&	y <= _tr.get_y();
		}
		FINLINE bool	is_point_inside( CONST vector2d<T>& pos ) CONST
		{
			return	is_point_inside( pos.get_x(), pos.get_y() );
		}
	};

	//! Typedef for REAL 2d vector.
	typedef rect<REAL> rect2df;
}
