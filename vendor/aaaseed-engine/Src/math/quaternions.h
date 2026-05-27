
#ifdef AAA_QUATERNIONS_H
#error "QUATERNIONS_H included more than once."
#endif
#define AAA_QUATERNIONS_H 1


#ifndef AAA_AAA_MATH_H
#	include "aaa_math.h"
#endif
#ifndef	AAA_MATRIX_H
#	include "matrix.h"
#endif

class	c_quaternion  
{
public:
	c_quaternion&	operator *=( const c_quaternion q );
	c_quaternion	operator * ( const c_quaternion q ) const;
	void			create_matrix( REAL * matrix );
	void			create_from_axis_angle( REAL x, REAL y, REAL z, REAL round );
	void			create_from_axis_x( REAL round )		{ create_from_axis_angle( 1.0f, .0f, .0f, round ); }
	void			create_from_axis_y( REAL round )		{ create_from_axis_angle( .0f, 1.0f, .0f, round ); }
	void			create_from_axis_z( REAL round )		{ create_from_axis_angle( .0f, .0f, 1.0f, round ); }
	c_quaternion();
	virtual ~c_quaternion()		{}

private:
	REAL	m_w;
	REAL	m_z;
	REAL	m_y;
	REAL	m_x;
};


class	c_quaternions
{
private:
	REAL	_w;
	REAL	_x;
	REAL	_y;
	REAL	_z;
public:
	c_quaternions():_w(1),_x(0),_y(0),_z(0)		{}
	c_quaternions( const REAL* gq ):_w(gq[0]),_x(gq[1]),_y(gq[2]),_z(gq[3])		{}
	c_quaternions( const c_quaternions& gq ):_w(gq._w),_x(gq._x),_y(gq._y),_z(gq._z)	{}
	c_quaternions( REAL w, REAL x, REAL y, REAL z )	{ set( w, x, y, z ); }
	c_quaternions( REAL x, REAL y, REAL z )			{ set( 1.0f, x, y, z); }

	~c_quaternions()	{}

	FINLINE	REAL	get_w()					{ return _w; }
	FINLINE	REAL	get_x()					{ return _x; }
	FINLINE	REAL	get_y()					{ return _y; }
	FINLINE	REAL	get_z()					{ return _z; }
	FINLINE	void	get_xyz( REAL* dst )	{ dst[0]=_x;dst[1]=_y;dst[2]=_z; }

	FINLINE	void	set( REAL w, REAL x, REAL y, REAL z)	{ _w = w; _x = x; _y = y; _z = z; }
	FINLINE	void	set( const REAL* gq )					{ set(gq[0],gq[1],gq[2],gq[3]); }
			void	set_axis_angle( REAL x, REAL y, REAL z, REAL round );
			void	set_axis_x( REAL round )		{ set_axis_angle( 1.0f, .0f, .0f, round ); }
			void	set_axis_y( REAL round )		{ set_axis_angle( .0f, 1.0f, .0f, round ); }
			void	set_axis_z( REAL round )		{ set_axis_angle( .0f, .0f, 1.0f, round ); }

	c_quaternions	operator	+ (c_quaternions gq)	{ c_quaternions temp(*this); temp += gq; return temp; }
	c_quaternions	operator	- (c_quaternions gq)	{ c_quaternions temp(*this); temp -= gq; return temp; }
	c_quaternions&	operator	+= (c_quaternions gq)	{ _w += gq._w; _x += gq._x; _y += gq._y; _z += gq._z; return *this; }
	c_quaternions&	operator	-= (c_quaternions gq)	{ _w -= gq._w; _x -= gq._x; _y -= gq._y; _z -= gq._z; return *this; }
	c_quaternions	operator	* (c_quaternions gq)	{ c_quaternions temp(*this); temp *=  gq; return temp; }
	//TODO f= .0f
	c_quaternions	operator	/ (REAL f)				{ c_quaternions temp(*this); temp /= f; return temp; }
	c_quaternions&	operator	*= (c_quaternions gq)
	{
		REAL w = _w * gq._w - (_x*gq._x + _y*gq._y + _z*gq._z);
		REAL x = (gq._x * _w) + (_x * gq._w) + _y*gq._z - _z*gq._y;
		REAL y = (gq._y * _w) + (_y * gq._w) + _z*gq._x - _x*gq._z;
		REAL z = (gq._z * _w) + (_z * gq._w) + _x*gq._y - _y*gq._x;
		set( w, x,y,z );
		//set( _w * gq._w - (_x*gq._x + _y*gq._y + _z*gq._z) , (gq.v * _w) + (v * gq._w) + v.getCross(gq.v) );
		return *this;
	}

	c_quaternions&	operator	/= (REAL f)				{ set( _w / f , _x / f, _y / f, _z / f ); return *this; }
	REAL						dot( c_quaternions gq );
	c_quaternions				get_cross (c_quaternions gq );
	c_quaternions				get_conjugate();
	c_quaternions&				conjugate();
	c_quaternions				get_inverse();
	c_quaternions&				inverse();
	REAL						selection()				{ return _w; }
	REAL						norm();
	c_quaternions				unit();
	FINLINE	c_quaternions&		normalise()				{ return normalize(); }
	c_quaternions&				normalize();
	void						rotate_vector( REAL* src, REAL* dst );
	void						matrix( REAL * matrix );
};

