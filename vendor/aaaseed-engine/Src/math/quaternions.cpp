#include "quaternions.h"

c_quaternion::c_quaternion()
{
	m_x = m_y = m_z = 0.0f;
	m_w = 1.0f;
}

void	c_quaternion::create_from_axis_angle( REAL x, REAL y, REAL z, REAL round )
{
	REAL	angle = round * PI_TIME_2 / 2.0f;
	REAL	result = SIN_RAD( angle );
	m_w = COS_RAD( angle );
	m_x = x * result;
	m_y = y * result;
	m_z = z * result;
}

void	c_quaternion::create_matrix( FP32 * matrix )
{
	matrix[ 0] = 1.0f - 2.0f * ( m_y * m_y + m_z * m_z );
	matrix[ 1] = 2.0f * (m_x * m_y + m_z * m_w);
	matrix[ 2] = 2.0f * (m_x * m_z - m_y * m_w);
	matrix[ 3] = 0.0f;

	matrix[ 4] = 2.0f * ( m_x * m_y - m_z * m_w );
	matrix[ 5] = 1.0f - 2.0f * ( m_x * m_x + m_z * m_z ); 
	matrix[ 6] = 2.0f * (m_z * m_y + m_x * m_w );
	matrix[ 7] = 0.0f;

	matrix[ 8] = 2.0f * ( m_x * m_z + m_y * m_w );
	matrix[ 9] = 2.0f * ( m_y * m_z - m_x * m_w );
	matrix[10] = 1.0f - 2.0f * ( m_x * m_x + m_y * m_y );
	matrix[11] = 0.0f;  

	matrix[12] = 0;
	matrix[13] = 0;
	matrix[14] = 0;
	matrix[15] = 1.0f;
}
//c_quaternion&	operator *=( const c_quaternion q );
//c_quaternion	operator * ( const c_quaternion q ) const;

c_quaternion	c_quaternion::operator *( const c_quaternion q ) const
{
	c_quaternion	r;
	r.m_w = m_w*q.m_w - m_x*q.m_x - m_y*q.m_y - m_z*q.m_z;
	r.m_x = m_w*q.m_x + m_x*q.m_w + m_y*q.m_z - m_z*q.m_y;
	r.m_y = m_w*q.m_y + m_y*q.m_w + m_z*q.m_x - m_x*q.m_z;
	r.m_z = m_w*q.m_z + m_z*q.m_w + m_x*q.m_y - m_y*q.m_x;
	return r;
}

c_quaternion&	c_quaternion::operator *=( const c_quaternion q )
{
	m_w = m_w*q.m_w - m_x*q.m_x - m_y*q.m_y - m_z*q.m_z;
	m_x = m_w*q.m_x + m_x*q.m_w + m_y*q.m_z - m_z*q.m_y;
	m_y = m_w*q.m_y + m_y*q.m_w + m_z*q.m_x - m_x*q.m_z;
	m_z = m_w*q.m_z + m_z*q.m_w + m_x*q.m_y - m_y*q.m_x;
	return *this;
}

void	c_quaternions::set_axis_angle( REAL x, REAL y, REAL z, REAL round )
{
	REAL	angle = round * PI_TIME_2 / 2.0f;
	REAL	result = SIN_RAD( angle );
	_w = COS_RAD( angle );
	_x = x * result;
	_y = y * result;
	_z = z * result;
}
REAL	c_quaternions::dot( c_quaternions gq )
{
	return ( _w*gq._w + _x*gq._x + _y*gq._y + _z*gq._z );
}
c_quaternions	c_quaternions::get_cross( c_quaternions gq )
{
	c_quaternions	cross;
	cross._w =  _w * gq._w - (_x * gq._x + _y * gq._y + _z*gq._z );
	cross._x = (gq._x * _w) + (_x * gq._w) + _y*gq._z - _z*gq._y;
	cross._y = (gq._y * _w) + (_y * gq._w) + _z*gq._x - _x*gq._z;
	cross._z = (gq._z * _w) + (_z * gq._w) + _x*gq._y - _y*gq._x;
	return	cross;
}
c_quaternions	c_quaternions::get_conjugate()
{
	c_quaternions temp(*this);
	temp.conjugate();
	return temp;
}
c_quaternions&	c_quaternions::conjugate()
{
	_x = -_x;
	_y = -_y;
	_z = -_z;
	return *this;
}
c_quaternions	c_quaternions::get_inverse()
{
	c_quaternions temp(*this);
	temp.inverse();
	return temp;
}
c_quaternions&	c_quaternions::inverse()
{
	conjugate();
	*this /= norm();
	return *this;
}
REAL	c_quaternions::norm()
{
	return sqrt( _w*_w + _x*_x + _y*_y +_z*_z );
}
c_quaternions	c_quaternions::unit()
{
	return this->operator / ( norm() );
}
c_quaternions&	c_quaternions::normalize()
{
	return this->operator /=( norm() );
}
void	c_quaternions::rotate_vector( REAL* src, REAL* dst )
{
	c_quaternions temp( src[ 0 ], src[ 1 ], src[ 2 ]);
	c_quaternions	tmp2 = get_cross(temp);
	tmp2 *= get_conjugate();
	dst[ 0 ] = tmp2._x;
	dst[ 1 ] = tmp2._y;
	dst[ 2 ] = tmp2._z;
}
void	c_quaternions::matrix( REAL * matrix )
{
	REAL xx=_x*_x;  REAL xy=_x*_y;  REAL xz=_x*_z;  REAL xw=_x*_w;
	REAL yy=_y*_y;  REAL yz=_y*_z;  REAL yw=_y*_w;
	REAL zz=_z*_z;  REAL zw=_z*_w;
	matrix[ 0] = 1 - 2*(zz + yy);
	matrix[ 1] = 2*(xy + zw);
	matrix[ 2] = 2*(xz - yw);
	matrix[ 3] = 0;

	matrix[ 4] = 2*(xy - zw);
	matrix[ 5] = 1 - 2*(zz + xx);
	matrix[ 6] = 2*(yz + xw);
	matrix[ 7] = 0;

	matrix[ 8] = 2*(yw + xz);
	matrix[ 9] = 2*(yz - xw);
	matrix[10] = 1 - 2*(yy + xx);
	matrix[11] = 0;

	matrix[12] = 0;
	matrix[13] = 0;
	matrix[14] = 0;
	matrix[15] = 1;
}

