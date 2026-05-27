
#ifdef AAA_RECTANGLE_2D_H
#error "RECTANGLE_2D_H included more than once."
#endif
#define AAA_RECTANGLE_2D_H 1



#ifndef AAA_TOUCHLIB_VECTOR2D_H
#	include "bdd_multitouch/vector2d.h"
#endif

using namespace vector_2d;

//!	Rectangle template.
template <class T>
class rectangle_2d
{
private:
	vector2d<T> _center;
	T	_size_x;
	T	_size_y;
	T	_angle;
	T	_scale;
public:

	rectangle_2d( T x = 0, T y = 0, T w = 1, T h = 1, T a = 0, T s = 1 )
		: _center( x , y), _size_x(w), _size_y(w), _angle(a), _scale(s) {}
	rectangle_2d( CONST vector2d<T>& center, CONST T size_x, CONST T size_y, CONST T angle, CONST T scale )
		: _center(center), _size_x(size_x), _size_y(size_y), _angle(angle), _scale(scale) {}


	
	FINLINE	void	set_center( CONST vector2d<T>v )		{ _center = v; }
	FINLINE	void	set_center( CONST T& x, CONST T& y )	{ _center.set( x, y ); }
	FINLINE	void	set_size_x( CONST T size_x )			{ _size_x = size_x; }
	FINLINE	void	set_size_y( CONST T size_y )			{ _size_y = size_y; }
	FINLINE	void	set_angle( CONST T angle )				{ _angle = angle; }
	FINLINE	void	set_dimension( CONST T sx, CONST T sy )	{ _size_x = sx; _size_y = sy; }
	FINLINE	void	set_scale( CONST T s )					{ _scale = s; }
	FINLINE vector2d<T>	get_center()						{ return _center; }
	FINLINE	T		get_size_x()							{ return _size_x; }
	FINLINE	T		get_size_y()							{ return _size_y; }
	FINLINE	T		get_angle()								{ return _angle; }
	FINLINE	T		get_scale()								{ return _scale; }
	FINLINE	T		get_x()									{ return _center.get_x(); }
	FINLINE	T		get_y()									{ return _center.get_y(); }

	//FINLINE void	set_scale_diff( CONST T scale )				{ _scale *= scale; }
	//FINLINE	void	set_center_diff( CONST T& x, CONST T& y )	{ _center.set( _center.get_x() + x, _center.get_y() + y ); }
	//FINLINE	void	set_angle_diff( CONST T angle )					{ _angle += angle; }


	FINLINE bool is_point_inside( CONST vector2d<T>& pos ) CONST
	{
		vector2d<T>	c;
		c = pos - _center;
		c.rotate_by_rad( -_angle );
		T	dx = _size_x * (T)0.5 * _scale;
		T	dy = _size_y * (T)0.5 * _scale;
		return c.get_x() < dx && c.get_x() > -dx && c.get_y() < dy && c.get_y() > -dy;
	}
	FINLINE bool is_point_inside( CONST T x, CONST T y ) CONST
	{
		return is_point_inside( vector2d<T>( x, y ) )
	}
/*
	FINLINE bool is_point_inside( CONST T x, CONST T y ) CONST
	{
		vector2d<T>	c;
		c = vector2d<T>( x, y ) - _center;
		c.rotate( -_angle );
		T	dx = _size_x * (T)0.5 * _scale;
		T	dy = _size_y * (T)0.5 * _scale;
		return c.get_x() < dx && c.get_x() > -dx && c.get_y() < dy && c.get_y() > -dy;
	}
*/
	
};

//! Typedef for REAL 2d vector.
typedef rectangle_2d<REAL> rect_2d_f;

#endif	//__RECTANGLE_2D__
