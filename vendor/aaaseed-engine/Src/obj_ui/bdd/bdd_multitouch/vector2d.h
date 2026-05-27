
#ifdef AAA_TOUCHLIB_VECTOR2D_H
#error "TOUCHLIB_VECTOR2D_H included more than once."
#endif
#define AAA_TOUCHLIB_VECTOR2D_H 1



#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_AAA_MATH_H
#	include "math/aaa_math.h"
#endif

namespace vector_2d 
{
//	REAL CONST	PI = 3.14159f;
	REAL CONST	GRAD_PI = 180.0f / 3.14159f;
	REAL CONST	GRAD_PI2 = 3.14159f / 180.0f;
	REAL CONST	ROUNDING_ERROR = 0.0001f;

	template <class T>
	class vector2d
	{
		T	_x;
		T	_y;
	public:

		vector2d( T x=T(0), T y=T(0) ) : _x(x), _y(y) {}
		vector2d( CONST vector2d<T>& other ) :_x(other._x), _y(other._y) {}

		// operators
		FINLINE vector2d<T>		operator	-	() CONST							{ return vector2d<T>( -_x, -_y ); }

		FINLINE vector2d<T>&	operator	=	(CONST vector2d<T>& other)			{ _x = other._x; _y = other._y; return *this; }

		FINLINE vector2d<T>		operator	+	(CONST vector2d<T>& other) CONST	{ return vector2d<T>(_x + other._x, _y + other._y); }
		FINLINE vector2d<T>&	operator	+=  (CONST vector2d<T>& other)			{ _x += other._x; _y += other._y; return *this; }

		FINLINE vector2d<T>		operator	-	(CONST vector2d<T>& other) CONST	{ return vector2d<T>(_x - other._x, _y - other._y); }
		FINLINE vector2d<T>&	operator	-=	(CONST vector2d<T>& other)			{ _x -= other._x; _y -= other._y; return *this; }

		FINLINE vector2d<T>		operator	*	(CONST vector2d<T>& other) CONST	{ return vector2d<T>(_x * other._x, _y * other._y); }
		FINLINE vector2d<T>&	operator	*=	(CONST vector2d<T>& other)			{ _x *= other._x; _y *= other._y; return *this; }
		FINLINE vector2d<T>		operator	*	(CONST T v) CONST					{ return vector2d<T>( _x * v, _y * v ); }
		FINLINE vector2d<T>&	operator	*=	(CONST T v)							{ _x *= v; _y *= v; return *this; }

		FINLINE vector2d<T>		operator	/	(CONST vector2d<T>& other) CONST	{ return vector2d<T>( _x / other._x, _y / other._y ); }
		FINLINE vector2d<T>&	operator	/=	(CONST vector2d<T>& other)			{ _x /= other._x; _y /= other._y; return *this; }
		FINLINE vector2d<T>		operator	/	(CONST T v) CONST					{ return vector2d<T>( _x / v, _y / v ); }
		FINLINE vector2d<T>&	operator	/=	(CONST T v)							{ _x /= v; _y /= v; return *this; }

		FINLINE	bool			operator	== ( CONST vector2d<T>& other ) CONST	{ return other._x == _x && other._y ==_y; }
		FINLINE	bool			operator	!= ( CONST vector2d<T>& other ) CONST	{ return other._x != _x || other._y != _y; }
		
		FINLINE	bool	is_null()						{ return _x == T(0) && _y == T(0); }

		//	ancessors
		FINLINE	T		get_x() CONST					{ return _x; }
		FINLINE	T		get_y() CONST					{ return _y; }
		FINLINE	T		get_x_pt()						{ return &_x; }
		FINLINE	T		get_y_pt()						{ return &_y; }

		FINLINE	void	set( CONST T& x, CONST T& y )	{ _x = x; _y = y; }
		FINLINE	void	set( CONST vector2d<T>& p )		{ _x = p._x; _y = p._y; }
		FINLINE	void	set_x( CONST T& x )				{ _x = x; }
		FINLINE	void	set_y( CONST T& y )				{ _y = y; }

		FINLINE REAL	get_length_squared() CONST		{ return _x * _x + _y * _y ; }
		FINLINE REAL	get_length() CONST				{ return REAL( sqrt( get_length_squared() ) ); }

		FINLINE T		dot_product(CONST vector2d<T>& other) CONST
		{
			return _x * other._x + _y * other._y;
		}
		FINLINE T		cross_product(CONST vector2d<T>& other) CONST
		{
			return _x * other._y - _y * other._x;
		}
		FINLINE REAL	get_distance_from_squared(CONST vector2d<T>& other) CONST
		{
			REAL vx = _x - other._x; 
			REAL vy = _y - other._y;
			return ( vx * vx + vy * vy );
		}
		FINLINE	REAL	get_distance_from(CONST vector2d<T>& other) CONST
		{
			return sqrt( get_distance_from_squared(other) );
		}

		//! rotates the point around a center by an amount of degrees.
		FINLINE	void	rotate_by_deg( REAL degrees, CONST vector2d<T>& center)
		{
			rotate( degrees * GRAD_PI );
		}

		//! rotates the point around a center by an amount of radians.
		FINLINE	void	rotate( REAL radians, CONST vector2d<T>& center)
		{
			T cs = (T)COS_RAD(radians);
			T sn = (T)SIN_RAD(radians);

			_x -= center._x;
			_y -= center._y;
			set( _x * cs - _y * sn, _x * sn + _y * cs );

			_x += center._x;
			_y += center._y;
		}

		//! rotates the point by an amount of radians.
		FINLINE	void	rotate( REAL radians )
		{
			T cs = (T)COS_RAD(radians);
			T sn = (T)SIN_RAD(radians);
			set( _x * cs - _y * sn, _x * sn + _y * cs );
		}

		//! normalizes the vector.
		FINLINE vector2d<T>& normalize()
		{
			T l = (T)get_length();
			if( l == 0 )
				return *this;

			l = (T)1.0 / l;
			_x *= l;
			_y *= l;
			return *this;
		}

		//! Calculates the angle of this vector in grad in the trigonometric sense.
		//! This method has been suggested by Pr3t3nd3r.
		//! \return Returns a value between 0 and 360.
		FINLINE REAL get_angle_deg() CONST
		{
			return get_angle() * GRAD_PI;
		}

		FINLINE REAL get_angle() CONST
		{
			return atan2( _y, _x );
		}

		////! Calculates the angle of this vector in degrees in the counter trigonometric sense.
		////! \return Returns a value between 0 and 360.
		//FINLINE REAL get_angle_deg() CONST
		//{
		//	return get_angle() * GRAD_PI;
		//}

		////! Calculates the angle of this vector in radians in the counter trigonometric sense.
		////! \return Returns a value between 0 and 360.
		//FINLINE REAL get_angle() CONST
		//{
		//	if (_y == 0.0 )
		//		return _x < 0.0 ? PI : 0.0; 
		//	else if (_x == 0.0 )
		//		return _y < 0.0 ? PI_BY_2 : PI_BY_2 * 3.;

		//	REAL tmp = _y / sqrt( _x * _x + _y * _y );
		//	tmp = atan( sqrt( 1 - tmp * tmp ), tmp );

		//	if(_x > 0.0 && _y > 0.0 )
		//		return tmp + PI_BY_2 * 3.;
		//	else
		//		if(_x > 0.0 && _y < 0.0 )
		//			return tmp + PI_BY_2;
		//		else
		//			if (_x < 0.0 && _y < 0.0 )
		//				return PI_BY_2 - tmp;
		//			else
		//				if (_x < 0.0 && _y > 0.0 )
		//					return PI_BY_2 * 3. - tmp;

		//	return tmp;
		//}

		//! Calculates the angle between this vector and another one in degrees.
		//! \return Returns a value between 0 and 90.
		FINLINE REAL get_angle_with_deg( CONST vector2d<T>& other ) CONST
		{
			return get_angle_with( other ) * GRAD_PI;
		}

		//! Calculates the angle between this vector and another one in radians.
		//! \return Returns a value between 0 and 90.
		FINLINE REAL get_angle_with( CONST vector2d<T>& other ) CONST
		{
			REAL tmp = _x * other._x + _y * other._y;

			if( tmp == 0.0 )
				return PI;

			tmp = tmp / sqrt( ( _x * _x + _y * _y ) * ( other._x * other._x + other._y * other._y ) );
			if( tmp < 0.0 )
				tmp = -tmp;

			return atan2( sqrt( 1 - tmp * tmp ), tmp );
		}

		//! returns interpolated vector
		//! \param other: other vector to interpolate between
		//! \param d: value between 0.0f and 1.0f.
		FINLINE vector2d<T> get_interpolated( CONST vector2d<T>& other, REAL d ) CONST
		{
			REAL inv = 1.0f - d;
			return vector2d<T>( other._x * inv + _x * d, other._y * inv + _y * d );
		}

		//! Returns if this vector interpreted as a point is on a line between two other points.
		/** It is assumed that the point is on the line. */
		FINLINE bool is_between_points(CONST vector2d<T>& begin, CONST vector2d<T>& end) CONST
		{
			REAL f = (REAL)(end - begin).get_length_squared();
			return (REAL)get_distance_from_square(begin) < f && 
				(REAL)get_distance_from_square(end) < f;
		}

		FINLINE	static bool is_on_same_side( vector2d<T> p1, vector2d<T> p2, vector2d<T> a, vector2d<T> b )
		{
			vector2d<T> ba = b - a;

			REAL cp1 = ba.cross_product( p1 - a );
			REAL cp2 = ba.cross_product( p2 - a );

			return ( cp1 * cp2 >= 0.0f );
		}
	};

	//! Typedef for FP32 2d vector.
	typedef vector2d<FP32> vector2df;
	//! Typedef for integer 2d vector.
	typedef vector2d<INT32> vector2di;

} // end namespace

