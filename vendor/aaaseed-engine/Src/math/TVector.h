
#ifdef AAA_TVECTOR_H
#error "TVECTOR_H included more than once."
#endif
#define AAA_TVECTOR_H 1


#ifndef _CMATH_
#	include <cmath>
#endif
#ifndef _CSTRING_
#	include <cstring>
#endif
#ifndef _IOSTREAM_
#	include <iostream>
#endif
#include <cassert>
#ifndef _LIMITS_
#	include <limits>
#endif

#ifndef AAA_TMATH_H
#	include "math/TMath.h"
#endif


template<typename T>
struct VECTRAIT {
	typedef float DIST;
};

template<>
struct VECTRAIT<double> {
	typedef double DIST;
};

template<>
struct VECTRAIT<int32_t> {
	typedef float DIST;
};

template<typename T, typename Y>
struct VEC3CONV {
	static T	getX( const Y &v ) { return static_cast<T>( v.x ); }
	static T	getY( const Y &v ) { return static_cast<T>( v.y ); }
	static T	getZ( const Y &v ) { return static_cast<T>( v.z ); }
};

template<typename T> class TVec3;

template<typename T>
class TVec2
{
 public:
	T x,y;

	typedef T							TYPE;
	typedef T							value_type;
	typedef typename VECTRAIT<T>::DIST	DIST;
	static const int DIM = 2;

	TVec2() :x(0), y(0) {}
	TVec2( T nx, T ny ) : x( nx ), y( ny ) {}
	TVec2( const TVec2<T>& src ) : x( src.x ), y( src.y ) {}
	explicit TVec2( const T *d ) : x( d[0] ), y( d[1] ) {}

	template<typename FromT>
	TVec2( const TVec2<FromT>& src )
		: x( static_cast<T>( src.x ) ),y( static_cast<T>( src.y ) )
	{}

	void set( T ax, T ay )
	{
		x = ax; y = ay;
	}

	void set( const TVec2<T> &rhs )
	{
		x = rhs.x; y = rhs.y;
	}

	// Operators
	template<typename FromT>
	TVec2<T>& operator=( const TVec2<FromT>& rhs )
	{
		x = static_cast<T>( rhs.x );
		y = static_cast<T>( rhs.y );
		return * this;
	}

	TVec2<T>& operator=( const TVec2<T>& rhs )
	{
		x = rhs.x;
		y = rhs.y;
		return * this;
	}

	T& operator[]( int n )
	{
		assert( n >= 0 && n <= 1 );
		return (&x)[n];
	}

	const T& operator[]( int n ) const
	{
		assert( n >= 0 && n <= 1 );
		return (&x)[n];
	}

	T*	ptr() const { return &(const_cast<TVec2*>( this )->x); }

	const TVec2<T>	operator+( const TVec2<T>& rhs ) const { return TVec2<T>( x + rhs.x, y + rhs.y ); }
	const TVec2<T>	operator-( const TVec2<T>& rhs ) const { return TVec2<T>( x - rhs.x, y - rhs.y ); }
	const TVec2<T>	operator*( const TVec2<T>& rhs ) const { return TVec2<T>( x * rhs.x, y * rhs.y ); }
	const TVec2<T>	operator/( const TVec2<T>& rhs ) const { return TVec2<T>( x / rhs.x, y / rhs.y ); }
	TVec2<T>&	operator+=( const TVec2<T>& rhs ) { x += rhs.x; y += rhs.y; return *this; }
	TVec2<T>&	operator-=( const TVec2<T>& rhs ) { x -= rhs.x; y -= rhs.y; return *this; }
	TVec2<T>&	operator*=( const TVec2<T>& rhs )	{ x *= rhs.x; y *= rhs.y; return *this; }
	TVec2<T>&	operator/=( const TVec2<T>& rhs ) { x /= rhs.x; y /= rhs.y; return *this; }
	const TVec2<T>	operator/( T rhs ) const { return TVec2<T>( x / rhs, y / rhs ); }
	TVec2<T>&	operator+=( T rhs )	{ x += rhs;	y += rhs; return *this; }
	TVec2<T>&	operator-=( T rhs ) { x -= rhs; y -= rhs; return *this; }
	TVec2<T>&	operator*=( T rhs ) { x *= rhs; y *= rhs; return *this; }
	TVec2<T>&	operator/=( T rhs ) { x /= rhs; y /= rhs; return *this; }

	TVec2<T>		operator-() const { return TVec2<T>( -x, -y ); } // unary negation

	bool operator==( const TVec2<T> &rhs ) const
	{
		return ( x == rhs.x ) && ( y == rhs.y );
	}

	bool operator!=( const TVec2<T> &rhs ) const
	{
		return ! ( *this == rhs );
	}

	T dot( const TVec2<T> &rhs ) const
	{
		return x * rhs.x + y * rhs.y;
	}

	//! Returns the z component of the cross if the two operands were TVec3's on the XY plane, the equivalent of TVec3(*this).cross( TVec3(rhs) ).z
	T cross( const TVec2<T> &rhs ) const
	{
		return x * rhs.y - y * rhs.x;
	}

	DIST distance( const TVec2<T> &rhs ) const
	{
		return ( *this - rhs ).length();
	}

	T distanceSquared( const TVec2<T> &rhs ) const
	{
		return ( *this - rhs ).lengthSquared();
	}

	DIST length() const
	{
		return tmath<DIST>::sqrt( x*x + y*y );
	}

	void normalize()
	{
		DIST invS = 1 / length();
		x *= invS;
		y *= invS;
	}

	TVec2<T> normalized() const
	{
		DIST invS = 1 / length();
		return TVec2<T>( x * invS, y * invS );
	}

	// tests for zero-length
	void safeNormalize()
	{
		T s = lengthSquared();
		if( s > 0 ) {
			DIST invL = 1 / tmath<DIST>::sqrt( s );
			x *= invL;
			y *= invL;
		}
	}

	TVec2<T> safeNormalized() const
	{
		T s = lengthSquared();
		if( s > 0 ) {
			DIST invL = 1 / tmath<DIST>::sqrt( s );
			return TVec2<T>( x * invL, y * invL );
		}
		else
			return TVec2<T>::zero();
	}

	void rotate( DIST radians )
	{
		T cosa = tmath<T>::cos( radians );
		T sina = tmath<T>::sin( radians );
		T rx = x * cosa - y * sina;
		y = x * sina + y * cosa;
		x = rx;
	}

	T lengthSquared() const
	{
		return x * x + y * y;
	}

	//! Limits the length of a TVec2 to \a maxLength, scaling it proportionally if necessary.
	void limit( DIST maxLength )
	{
		T lengthSquared = x * x + y * y;

		if( ( lengthSquared > maxLength * maxLength ) && ( lengthSquared > 0 ) ) {
			DIST ratio = maxLength / tmath<DIST>::sqrt( lengthSquared );
			x *= ratio;
			y *= ratio;
		}
	}

	//! Returns a copy of the TVec2 with its length limited to \a maxLength, scaling it proportionally if necessary.
	TVec2<T> limited( T maxLength ) const
	{
		T lengthSquared = x * x + y * y;

		if( ( lengthSquared > maxLength * maxLength ) && ( lengthSquared > 0 ) ) {
			DIST ratio = maxLength / tmath<DIST>::sqrt( lengthSquared );
			return TVec2<T>( x * ratio, y * ratio );
		}
		else
			return *this;
	}

	void invert()
	{
		x = -x;
		y = -y;
	}

	TVec2<T> inverse() const
	{
		return TVec2<T>( -x, -y );
	}

	TVec2<T> lerp( T fact, const TVec2<T>& r ) const
	{
		return (*this) + ( r - (*this) ) * fact;
	}

	void lerpEq( T fact, const TVec2<T> &rhs )
	{
		x = x + ( rhs.x - x ) * fact; y = y + ( rhs.y - y ) * fact;
	}

	// GLSL inspired swizzling functions.
	TVec2<T> xx() const { return TVec2<T>(x, x); }
	TVec2<T> xy() const { return TVec2<T>(x, y); }
	TVec2<T> yx() const { return TVec2<T>(y, x); }
	TVec2<T> yy() const { return TVec2<T>(y, y); }

	TVec3<T> xxx() const { return TVec3<T>(x, x, x); }
	TVec3<T> xxy() const { return TVec3<T>(x, x, y); }
	TVec3<T> xyx() const { return TVec3<T>(x, y, x); }
	TVec3<T> xyy() const { return TVec3<T>(x, y, y); }
	TVec3<T> yxx() const { return TVec3<T>(y, x, x); }
	TVec3<T> yxy() const { return TVec3<T>(y, x, y); }
	TVec3<T> yyx() const { return TVec3<T>(y, y, x); }
	TVec3<T> yyy() const { return TVec3<T>(y, y, y); }

	static TVec2<T> max()
	{
		return TVec2<T>( std::numeric_limits<T>::max(), std::numeric_limits<T>::max() );
	}

	static TVec2<T> zero()
	{
		return TVec2<T>( 0, 0 );
	}

	static TVec2<T> one()
	{
		return TVec2<T>( 1, 1 );
	}

	friend std::ostream& operator<<( std::ostream& lhs, const TVec2<T>& rhs )
	{
		lhs << "[" << rhs.x << "," << rhs.y << "]";
		return lhs;
	}

	static TVec2<T> xAxis() { return TVec2<T>( 1, 0 ); }
	static TVec2<T> yAxis() { return TVec2<T>( 0, 1 ); }

	static TVec2<T> NaN()   { return TVec2<T>( tmath<T>::NaN(), tmath<T>::NaN() ); }
};

template<typename T>
class TVec3
{
public:
	T x,y,z;

	typedef T								TYPE;
	typedef T								value_type;
	static const int DIM = 3;

	TVec3() :x(0), y(0), z(0) {}
	TVec3( T nx, T ny, T nz )
		: x( nx ), y( ny ), z( nz )
	{}
	TVec3( const TVec3<T> &src )
		: x( src.x ), y( src.y ), z( src.z )
	{}
	TVec3( const TVec2<T> &v2, T aZ )
		: x( v2.x ), y( v2.y ), z( aZ )
	{}
	explicit TVec3( const TVec2<T> &v2 )
		: x( v2.x ), y( v2.y ), z( 0 )
	{}
	explicit TVec3( const T *d ) : x( d[0] ), y( d[1] ), z( d[2] ) {}
	template<typename FromT>
	TVec3( const TVec3<FromT> &src )
		: x( static_cast<T>( src.x ) ), y( static_cast<T>( src.y ) ), z( static_cast<T>( src.z ) )
	{}
	template<typename Y>
	explicit TVec3( const Y &v )
		: x( VEC3CONV<TVec3<typename T::TYPE>,Y>::getX( v ) ), y( VEC3CONV<typename T::TYPE,Y>::getY( v ) ), z( VEC3CONV<typename T::TYPE,Y>::getZ( v ) )
	{
	}

	void set( T ax, T ay, T az )
	{
		x = ax; y = ay; z = az;
	}

	void set( const TVec3<T> &rhs )
	{
		x = rhs.x; y = rhs.y; z = rhs.z;
	}

	TVec3<T>& operator=( const TVec3<T> &rhs )
	{
		x = rhs.x;
		y = rhs.y;
		z = rhs.z;
		return * this;
	}

	template<typename FromT>
	TVec3<T>& operator=( const TVec3<FromT> &rhs )
	{
		x = static_cast<T>( rhs.x );
		y = static_cast<T>( rhs.y );
		z = static_cast<T>( rhs.z );
		return * this;
	}

	T& operator[]( int n )
	{
		assert( n >= 0 && n <= 2 );
		return (&x)[n];
	}

	const T& operator[]( int n ) const
	{
		assert( n >= 0 && n <= 2 );
		return (&x)[n];
	}

	T*	ptr() const { return &(const_cast<TVec3*>( this )->x); }

	const TVec3<T>	operator+( const TVec3<T>& rhs ) const { return TVec3<T>( x + rhs.x, y + rhs.y, z + rhs.z ); }
	const TVec3<T>	operator-( const TVec3<T>& rhs ) const { return TVec3<T>( x - rhs.x, y - rhs.y, z - rhs.z ); }
	const TVec3<T>	operator*( const TVec3<T>& rhs ) const { return TVec3<T>( x * rhs.x, y * rhs.y, z * rhs.z ); }
	const TVec3<T>	operator/( const TVec3<T>& rhs ) const { return TVec3<T>( x / rhs.x, y / rhs.y, z / rhs.z ); }
	TVec3<T>&	operator+=( const TVec3<T>& rhs ) { x += rhs.x; y += rhs.y; z += rhs.z; return *this; }
	TVec3<T>&	operator-=( const TVec3<T>& rhs ) { x -= rhs.x; y -= rhs.y; z -= rhs.z; return *this; }
	TVec3<T>&	operator*=( const TVec3<T>& rhs ) { x *= rhs.x; y *= rhs.y; z *= rhs.z; return *this; }
	TVec3<T>&	operator/=( const TVec3<T>& rhs ) { x /= rhs.x; y /= rhs.y; z /= rhs.z; return *this; }
	const TVec3<T>	operator/( T rhs ) const { T invRhs = static_cast<T>( 1.0 ) / rhs; return TVec3<T>( x * invRhs, y * invRhs, z * invRhs ); }
	TVec3<T>&	operator+=( T rhs ) { x += rhs; y += rhs; z += rhs; return *this; }
	TVec3<T>&	operator-=( T rhs ) { x -= rhs; y -= rhs; z -= rhs; return *this; }
	TVec3<T>&	operator*=( T rhs ) { x *= rhs; y *= rhs; z *= rhs; return *this; }
	TVec3<T>&	operator/=( T rhs ) { x /= rhs; y /= rhs; z /= rhs; return *this; }

	TVec3<T>		operator-() const { return TVec3<T>( -x, -y, -z ); } // unary negation

	bool operator==( const TVec3<T>& rhs ) const
	{
		return ( x == rhs.x ) && ( y == rhs.y ) && ( z == rhs.z );
	}

	bool operator!=( const TVec3<T>& rhs ) const
	{
		return !( *this == rhs );
	}

	T dot( const TVec3<T> &rhs ) const
	{
		return x*rhs.x + y*rhs.y + z*rhs.z;
	}

	TVec3<T> cross( const TVec3<T> &rhs ) const
	{
		return TVec3<T>( y * rhs.z - rhs.y * z, z * rhs.x - rhs.z * x, x * rhs.y - rhs.x * y );
	}

	T distance( const TVec3<T> &rhs ) const
	{
		return ( *this - rhs ).length();
	}

	T distanceSquared( const TVec3<T> &rhs ) const
	{
		return ( *this - rhs ).lengthSquared();
	}

	T length() const
	{
		return tmath<T>::sqrt( x*x + y*y + z*z );
	}

	T lengthSquared() const
	{
		return x*x + y*y + z*z;
	}

	//! Limits the length of a TVec3 to \a maxLength, scaling it proportionally if necessary.
	void limit( T maxLength )
	{
		T lengthSquared = x * x + y * y + z * z;

		if( ( lengthSquared > maxLength * maxLength ) && ( lengthSquared > 0 ) ) {
			T ratio = maxLength / tmath<T>::sqrt( lengthSquared );
			x *= ratio;
			y *= ratio;
			z *= ratio;
		}
	}

	//! Returns a copy of the TVec3 with its length limited to \a maxLength, scaling it proportionally if necessary.
	TVec3<T> limited( T maxLength ) const
	{
		T lengthSquared = x * x + y * y + z * z;

		if( ( lengthSquared > maxLength * maxLength ) && ( lengthSquared > 0 ) ) {
			T ratio = maxLength / tmath<T>::sqrt( lengthSquared );
			return TVec3<T>( x * ratio, y * ratio, z * ratio );
		}
		else
			return *this;
	}

	void invert()
	{
		x = -x; y = -y; z = -z;
	}

	TVec3<T> inverse() const
	{
		return TVec3<T>( -x, -y, -z );
	}

	void normalize()
	{
		T invS = ((T)1) / length();
		x *= invS;
		y *= invS;
		z *= invS;
	}

	TVec3<T> normalized() const
	{
		T invS = ((T)1) / length();
		return TVec3<T>( x * invS, y * invS, z * invS );
	}

	// tests for zero-length
	void safeNormalize()
	{
		T s = lengthSquared();
		if( s > 0 ) {
			T invS = ((T)1) / tmath<T>::sqrt( s );
			x *= invS;
			y *= invS;
			z *= invS;
		}
	}

	TVec3<T> safeNormalized() const
	{
		T s = lengthSquared();
		if( s > 0 ) {
			float invS = ((T)1) / tmath<T>::sqrt( s );
			return TVec3<T>( x * invS, y * invS, z * invS );
		}
		else
			return *this;
	}

	//! Returns a vector which is orthogonal to \a this
	TVec3<T> getOrthogonal() const
	{
		if( tmath<T>::abs( y ) < (T)0.99 ) // abs(dot(u, Y)), somewhat arbitrary epsilon
			return TVec3<T>( -z, 0, x ); // cross( this, Y )
		else
			return TVec3<T>( 0, z, -y ); // cross( this, X )
	}

	void rotateX( T angle )
	{
		T sina = tmath<T>::sin(angle);
		T cosa = tmath<T>::cos(angle);
		T ry = y * cosa - z * sina;
		T rz = y * sina + z * cosa;
		y = ry;
		z = rz;
	}

	void rotateY( T angle )
	{
		T sina = tmath<T>::sin(angle);
		T cosa = tmath<T>::cos(angle);
		T rx = x * cosa - z * sina;
		T rz = x * sina + z * cosa;
		x = rx;
		z = rz;
	}

	void rotateZ( T angle )
	{
		T sina = tmath<T>::sin(angle);
		T cosa = tmath<T>::cos(angle);
		T rx = x * cosa - y * sina;
		T ry = x * sina + y * cosa;
		x = rx;
		y = ry;
	}

	void rotate( TVec3<T> axis, T angle )
	{
		T cosa = tmath<T>::cos(angle);
		T sina = tmath<T>::sin(angle);

		T rx = (cosa + (1 - cosa) * axis.x * axis.x) * x;
		rx += ((1 - cosa) * axis.x * axis.y - axis.z * sina) * y;
		rx += ((1 - cosa) * axis.x * axis.z + axis.y * sina) * z;

		T ry = ((1 - cosa) * axis.x * axis.y + axis.z * sina) * x;
		ry += (cosa + (1 - cosa) * axis.y * axis.y) * y;
		ry += ((1 - cosa) * axis.y * axis.z - axis.x * sina) * z;

		T rz = ((1 - cosa) * axis.x * axis.z - axis.y * sina) * x;
		rz += ((1 - cosa) * axis.y * axis.z + axis.x * sina) * y;
		rz += (cosa + (1 - cosa) * axis.z * axis.z) * z;

		x = rx;
		y = ry;
		z = rz;
	}

	TVec3<T> lerp( T fact, const TVec3<T> &rhs ) const
	{
		return (*this) + (rhs - (*this)) * fact;
	}

	void lerpEq( T fact, const TVec3<T> &rhs )
	{
		x = x + ( rhs.x - x ) * fact; y = y + ( rhs.y - y ) * fact; z = z + ( rhs.z - z ) * fact;
	}

	static TVec3<T> max( void )
	{
		return TVec3<T>( std::numeric_limits<T>::max(), std::numeric_limits<T>::max(), std::numeric_limits<T>::max() );
	}

	static TVec3<T> zero( void )
	{
		return TVec3<T>( static_cast<T>( 0 ), static_cast<T>( 0 ), static_cast<T>( 0 ) );
	}

	static TVec3<T> one( void )
	{
		return TVec3<T>( static_cast<T>( 1 ), static_cast<T>( 1 ), static_cast<T>( 1 ) );
	}

	static TVec3<T> view_direction( void )
	{
		return TVec3<T>( static_cast<T>( 0 ), static_cast<T>( 0 ), static_cast<T>( -1 ) );
	}

	TVec3<T> slerp( T fact, const TVec3<T> &r ) const
	{
		T cosAlpha, alpha, sinAlpha;
		T t1, t2;
		TVec3<T> result;

		// get cosine of angle between vectors (-1 -> 1)
		cosAlpha = dot( r );

		// get angle (0 -> pi)
		alpha = tmath<T>::acos( cosAlpha );

		// get sine of angle between vectors (0 -> 1)
		sinAlpha = tmath<T>::sin( alpha );

		// this breaks down when sinAlpha = 0, i.e. alpha = 0 or pi
		t1 = tmath<T>::sin( ((T)1 - fact) * alpha) / sinAlpha;
		t2 = tmath<T>::sin( fact * alpha ) / sinAlpha;

		// interpolate src vectors
		return *this * t1 + r * t2;
	}

	// derived from but not equivalent to Quaternion::squad
	TVec3<T> squad( T t, const TVec3<T> &tangentA, const TVec3<T> &tangentB, const TVec3<T> &end ) const
	{
		TVec3<T> r1 = slerp( t, end );
		TVec3<T> r2 = tangentA.slerp( t, tangentB );
		return r1.slerp( 2 * t * (1-t), r2 );
	}

	// GLSL inspired swizzling functions.
	TVec2<T> xx() const { return TVec2<T>(x, x); }
	TVec2<T> xy() const { return TVec2<T>(x, y); }
	TVec2<T> xz() const { return TVec2<T>(x, z); }
	TVec2<T> yx() const { return TVec2<T>(y, x); }
	TVec2<T> yy() const { return TVec2<T>(y, y); }
	TVec2<T> yz() const { return TVec2<T>(y, z); }
	TVec2<T> zx() const { return TVec2<T>(z, x); }
	TVec2<T> zy() const { return TVec2<T>(z, y); }
	TVec2<T> zz() const { return TVec2<T>(z, z); }

	TVec3<T> xxx() const { return TVec3<T>(x, x, x); }
	TVec3<T> xxy() const { return TVec3<T>(x, x, y); }
	TVec3<T> xxz() const { return TVec3<T>(x, x, z); }
	TVec3<T> xyx() const { return TVec3<T>(x, y, x); }
	TVec3<T> xyy() const { return TVec3<T>(x, y, y); }
	TVec3<T> xyz() const { return TVec3<T>(x, y, z); }
	TVec3<T> xzx() const { return TVec3<T>(x, z, x); }
	TVec3<T> xzy() const { return TVec3<T>(x, z, y); }
	TVec3<T> xzz() const { return TVec3<T>(x, z, z); }
	TVec3<T> yxx() const { return TVec3<T>(y, x, x); }
	TVec3<T> yxy() const { return TVec3<T>(y, x, y); }
	TVec3<T> yxz() const { return TVec3<T>(y, x, z); }
	TVec3<T> yyx() const { return TVec3<T>(y, y, x); }
	TVec3<T> yyy() const { return TVec3<T>(y, y, y); }
	TVec3<T> yyz() const { return TVec3<T>(y, y, z); }
	TVec3<T> yzx() const { return TVec3<T>(y, z, x); }
	TVec3<T> yzy() const { return TVec3<T>(y, z, y); }
	TVec3<T> yzz() const { return TVec3<T>(y, z, z); }
	TVec3<T> zxx() const { return TVec3<T>(z, x, x); }
	TVec3<T> zxy() const { return TVec3<T>(z, x, y); }
	TVec3<T> zxz() const { return TVec3<T>(z, x, z); }
	TVec3<T> zyx() const { return TVec3<T>(z, y, x); }
	TVec3<T> zyy() const { return TVec3<T>(z, y, y); }
	TVec3<T> zyz() const { return TVec3<T>(z, y, z); }
	TVec3<T> zzx() const { return TVec3<T>(z, z, x); }
	TVec3<T> zzy() const { return TVec3<T>(z, z, y); }
	TVec3<T> zzz() const { return TVec3<T>(z, z, z); }

	friend std::ostream& operator<<( std::ostream& lhs, const TVec3<T> rhs )
	{
		lhs << "[" << rhs.x << "," << rhs.y << "," << rhs.z  << "]";
		return lhs;
	}

	static TVec3<T> xAxis() { return TVec3<T>( 1, 0, 0 ); }
	static TVec3<T> yAxis() { return TVec3<T>( 0, 1, 0 ); }
	static TVec3<T> zAxis() { return TVec3<T>( 0, 0, 1 ); }

	static TVec3<T> NaN()   { return TVec3<T>( tmath<T>::NaN(), tmath<T>::NaN(), tmath<T>::NaN() ); }
};

template <class T>
class TVec4
{
 public:
	T x,y,z,w;

	typedef T							TYPE;
	typedef T							value_type;
	static const int DIM = 4;

	TVec4()
		: x( 0 ), y( 0 ), z( 0 ), w( 0 )
	{}
	TVec4( T nx, T ny, T nz, T nw = 0 )
		: x( nx ), y( ny ), z( nz ), w( nw )
	{}
	TVec4( const TVec3<T>& src, T aW = 0 )
		: x( src.x ), y( src.y ), z( src.z ), w( aW )
	{}
	TVec4( const TVec4<T>& src )
		: x( src.x ), y( src.y ), z( src.z ), w( src.w )
	{}
	template<typename FromT>
	TVec4( const TVec4<FromT>& src )
		: x( static_cast<T>( src.x ) ), y( static_cast<T>( src.y ) ), z( static_cast<T>( src.z ) ),w( static_cast<T>( src.w ) )
	{}
	explicit TVec4( const T *d ) : x( d[0] ), y( d[1] ), z( d[2] ), w( d[3] ) {}
	
	void set( T ax, T ay, T az, T aw )
	{
		x = ax; y = ay; z = az; w = aw;
	}
	
	void set( const TVec4<T> &rhs )
	{
		x = rhs.x; y = rhs.y; z = rhs.z; w = rhs.w;
	}
	
	TVec4<T>& operator=( const TVec4<T>& rhs )
	{
		x = rhs.x; y = rhs.y; z = rhs.z; w = rhs.w;
		return *this;
	}

	template<typename FromT>
	TVec4<T>& operator=( const TVec4<FromT>& rhs )
	{
		x = static_cast<T>(rhs.x); y = static_cast<T>(rhs.y); z = static_cast<T>(rhs.z); w = static_cast<T>(rhs.w);
		return *this;
	}

	T& operator[]( int n )
	{
		assert( n >= 0 && n <= 3 );
		return (&x)[n];
	}

	const T& operator[]( int n )  const
	{
		assert( n >= 0 && n <= 3 );
		return (&x)[n];
	}

	T*	ptr() const { return &(const_cast<TVec4*>( this )->x); }

	const TVec4<T>	operator+( const TVec4<T>& rhs ) const { return TVec4<T>( x + rhs.x, y + rhs.y, z + rhs.z, w + rhs.w ); }
	const TVec4<T>	operator-( const TVec4<T>& rhs ) const { return TVec4<T>( x - rhs.x, y - rhs.y, z - rhs.z, w - rhs.w ); }
	const TVec4<T>	operator*( const TVec4<T>& rhs ) const { return TVec4<T>( x * rhs.x, y * rhs.y, z * rhs.z, w * rhs.w ); }
	const TVec4<T>	operator/( const TVec4<T>& rhs ) const { return TVec4<T>( x / rhs.x, y / rhs.y, z / rhs.z, w / rhs.w ); }
	TVec4<T>&		operator+=( const TVec4<T>& rhs ) { x += rhs.x; y += rhs.y; z += rhs.z; w += rhs.w; return *this; }
	TVec4<T>&		operator-=( const TVec4<T>& rhs ) {	x -= rhs.x;	y -= rhs.y;	z -= rhs.z;	w -= rhs.w;	return *this; }
	TVec4<T>&		operator*=( const TVec4<T>& rhs ) { x *= rhs.x; y *= rhs.y; z *= rhs.z;	w *= rhs.w;	return *this; }
	TVec4<T>&		operator/=( const TVec4<T>& rhs ) {	x /= rhs.x;	y /= rhs.y;	z /= rhs.z;	w /= rhs.w;	return *this; }
	const TVec4<T>	operator/( T rhs ) const { return TVec4<T>( x / rhs, y / rhs, z / rhs, w / rhs ); }
	TVec4<T>&		operator+=( T rhs ) {	x += rhs; y += rhs; z += rhs; w += rhs; return *this; }
	TVec4<T>&		operator-=( T rhs ) {	x -= rhs; y -= rhs; z -= rhs; w -= rhs;	return * this; }
	TVec4<T>&		operator*=( T rhs ) { x *= rhs; y *= rhs; z *= rhs; w *= rhs; return * this; }
	TVec4<T>&		operator/=( T rhs ) { x /= rhs; y /= rhs; z /= rhs; w /= rhs;	return * this; }

	TVec4<T>			operator-() const { return TVec4<T>( -x, -y, -z, -w ); } // unary negation

	bool operator==( const TVec4<T>& rhs ) const
	{
		return ( x == rhs.x ) && ( y == rhs.y ) && ( z == rhs.z ) && ( w == rhs.w );
	}

	bool operator!=( const TVec4<T>& rhs ) const 
	{ 
		return ! (*this == rhs); 
	}

	T dot( const TVec4<T> &rhs ) const
	{
		return x*rhs.x + y*rhs.y + z*rhs.z;
	}

	TVec4<T> cross( const TVec4<T> &rhs ) const
	{
		return TVec4<T>( y*rhs.z - rhs.y*z, z*rhs.x - rhs.z*x, x*rhs.y - rhs.x*y );
	}

	T distance( const TVec4<T> &rhs ) const
	{
		return ( *this - rhs ).length();
	}

	T distanceSquared( const TVec4<T> &rhs ) const
	{
		return ( *this - rhs ).lengthSquared();
	}

	T length() const
	{
		// For most vector operations, this assumes w to be zero.
		return tmath<T>::sqrt( x*x + y*y + z*z + w*w );
	}

	T lengthSquared() const
	{
		// For most vector operations, this assumes w to be zero.
		return x*x + y*y + z*z + w*w;
	}

	void normalize()
	{
		T invS = ((T)1) / length();
		x *= invS;
		y *= invS;
		z *= invS;
		w *= invS;
	}
	
	TVec4<T> normalized() const 
	{
		T invS = ((T)1) / length();
		return TVec4<T>( x*invS, y*invS, z*invS, w*invS );
	}

	// Tests for zero-length
	void safeNormalize()
	{
		T s = lengthSquared();
		if( s > 0 ) {
			T invS = ((T)1) / tmath<T>::sqrt( s );
			x *= invS;
			y *= invS;
			z *= invS;
			w  = (T)0;
		}
	}

	//! Limits the length of a TVec4 to \a maxLength, scaling it proportionally if necessary.
	void limit( T maxLength )
	{
		T lenSq = lengthSquared();

		if( ( lenSq > maxLength * maxLength ) && ( lenSq > 0 ) ) {
			T ratio = maxLength / tmath<T>::sqrt( lenSq );
			x *= ratio;
			y *= ratio;
			z *= ratio;
			w *= ratio;
		}
	
		/*
		T lengthSquared = x * x + y * y + z * z + w * w;

		if( ( lengthSquared > maxLength * maxLength ) && ( lengthSquared > 0 ) ) {
			T ratio = maxLength / tmath<T>::sqrt( lengthSquared );
			x *= ratio;
			y *= ratio;
			z *= ratio;
			w *= ratio;
		}
		*/
	}

	//! Returns a copy of the TVec4 with its length limited to \a maxLength, scaling it proportionally if necessary.
	TVec4<T> limited( T maxLength ) const
	{
		T lenSq = lengthSquared();

		if( ( lenSq > maxLength * maxLength ) && ( lenSq > 0 ) ) {
			T ratio = maxLength / tmath<T>::sqrt( lenSq );
			return TVec4<T>( x * ratio, y * ratio, z * ratio, w * ratio );
		}
		else
			return *this;

		/*
		T lengthSquared = x * x + y * y + z * z + w * w;

		if( ( lengthSquared > maxLength * maxLength ) && ( lengthSquared > 0 ) ) {
			T ratio = maxLength / tmath<T>::sqrt( lengthSquared );
			return TVec4<T>( x * ratio, y * ratio, z * ratio, w * ratio );
		}
		else
			return *this;
		*/
	}

	void invert()
	{
		x = -x; y = -y; z = -z; w = -w;
	}

	TVec4<T> inverse() const
	{
		return TVec4<T>( -x, -y, -z, -w );
	}

	TVec4<T> lerp( T fact, const TVec4<T>& r ) const
	{
		return (*this) + ( r - (*this) ) * fact;
	}

	void lerpEq( T fact, const TVec4<T> &rhs )
	{
		x = x + ( rhs.x - x ) * fact; y = y + ( rhs.y - y ) * fact; z = z + ( rhs.z - z ) * fact; w = w + ( rhs.w - w ) * fact;
	}

	static TVec4<T> max()
	{
		return TVec4<T>( std::numeric_limits<T>::max(), std::numeric_limits<T>::max(), std::numeric_limits<T>::max(), std::numeric_limits<T>::max() );
	}

	static TVec4<T> zero()
	{
		return TVec4<T>( static_cast<T>( 0 ), static_cast<T>( 0 ), static_cast<T>( 0 ), static_cast<T>( 0 ) );
	}

	static TVec4<T> one()
	{
		return TVec4<T>( static_cast<T>( 1 ), static_cast<T>( 1 ), static_cast<T>( 1 ), static_cast<T>( 1 ) );
	}

	TVec4<T> slerp( T fact, const TVec3<T> &r ) const
	{
		T cosAlpha, alpha, sinAlpha;
		T t1, t2;
		TVec4<T> result;

		// get cosine of angle between vectors (-1 -> 1)
		cosAlpha = dot( r );

		// get angle (0 -> pi)
		alpha = tmath<T>::acos( cosAlpha );

		// get sine of angle between vectors (0 -> 1)
		sinAlpha = tmath<T>::sin( alpha );

		// this breaks down when sinAlpha = 0, i.e. alpha = 0 or pi
		t1 = tmath<T>::sin( ((T)1 - fact) * alpha) / sinAlpha;
		t2 = tmath<T>::sin( fact * alpha ) / sinAlpha;

		// interpolate src vectors
		return *this * t1 + r * t2;
	}

	// derived from but not equivalent to Quaternion::squad
	TVec4<T> squad( T t, const TVec4<T> &tangentA, const TVec4<T> &tangentB, const TVec4<T> &end ) const
	{
		TVec4<T> r1 = slerp( t, end );
		TVec4<T> r2 = tangentA.slerp( t, tangentB );
		return r1.slerp( 2 * t * (1-t), r2 );
	}

	// GLSL inspired swizzling functions.
	TVec2<T> xx() const { return TVec2<T>(x, x); }
	TVec2<T> xy() const { return TVec2<T>(x, y); }
	TVec2<T> xz() const { return TVec2<T>(x, z); }
	TVec2<T> yx() const { return TVec2<T>(y, x); }
	TVec2<T> yy() const { return TVec2<T>(y, y); }
	TVec2<T> yz() const { return TVec2<T>(y, z); }
	TVec2<T> zx() const { return TVec2<T>(z, x); }
	TVec2<T> zy() const { return TVec2<T>(z, y); }
	TVec2<T> zz() const { return TVec2<T>(z, z); }

	TVec3<T> xxx() const { return TVec3<T>(x, x, x); }
	TVec3<T> xxy() const { return TVec3<T>(x, x, y); }
	TVec3<T> xxz() const { return TVec3<T>(x, x, z); }
	TVec3<T> xyx() const { return TVec3<T>(x, y, x); }
	TVec3<T> xyy() const { return TVec3<T>(x, y, y); }
	TVec3<T> xyz() const { return TVec3<T>(x, y, z); }
	TVec3<T> xzx() const { return TVec3<T>(x, z, x); }
	TVec3<T> xzy() const { return TVec3<T>(x, z, y); }
	TVec3<T> xzz() const { return TVec3<T>(x, z, z); }
	TVec3<T> yxx() const { return TVec3<T>(y, x, x); }
	TVec3<T> yxy() const { return TVec3<T>(y, x, y); }
	TVec3<T> yxz() const { return TVec3<T>(y, x, z); }
	TVec3<T> yyx() const { return TVec3<T>(y, y, x); }
	TVec3<T> yyy() const { return TVec3<T>(y, y, y); }
	TVec3<T> yyz() const { return TVec3<T>(y, y, z); }
	TVec3<T> yzx() const { return TVec3<T>(y, z, x); }
	TVec3<T> yzy() const { return TVec3<T>(y, z, y); }
	TVec3<T> yzz() const { return TVec3<T>(y, z, z); }
	TVec3<T> zxx() const { return TVec3<T>(z, x, x); }
	TVec3<T> zxy() const { return TVec3<T>(z, x, y); }
	TVec3<T> zxz() const { return TVec3<T>(z, x, z); }
	TVec3<T> zyx() const { return TVec3<T>(z, y, x); }
	TVec3<T> zyy() const { return TVec3<T>(z, y, y); }
	TVec3<T> zyz() const { return TVec3<T>(z, y, z); }
	TVec3<T> zzx() const { return TVec3<T>(z, z, x); }
	TVec3<T> zzy() const { return TVec3<T>(z, z, y); }
	TVec3<T> zzz() const { return TVec3<T>(z, z, z); }

	TVec4<T> xxxx() const { return TVec4<T>(x, x, x, x); }
	TVec4<T> xxxy() const { return TVec4<T>(x, x, x, y); }
	TVec4<T> xxxz() const { return TVec4<T>(x, x, x, z); }
	TVec4<T> xxxw() const { return TVec4<T>(x, x, x, w); }
	TVec4<T> xxyx() const { return TVec4<T>(x, x, y, x); }
	TVec4<T> xxyy() const { return TVec4<T>(x, x, y, y); }
	TVec4<T> xxyz() const { return TVec4<T>(x, x, y, z); }
	TVec4<T> xxyw() const { return TVec4<T>(x, x, y, w); }
	TVec4<T> xxzx() const { return TVec4<T>(x, x, z, x); }
	TVec4<T> xxzy() const { return TVec4<T>(x, x, z, y); }
	TVec4<T> xxzz() const { return TVec4<T>(x, x, z, z); }
	TVec4<T> xxzw() const { return TVec4<T>(x, x, z, w); }
	TVec4<T> xxwx() const { return TVec4<T>(x, x, w, x); }
	TVec4<T> xxwy() const { return TVec4<T>(x, x, w, y); }
	TVec4<T> xxwz() const { return TVec4<T>(x, x, w, z); }
	TVec4<T> xxww() const { return TVec4<T>(x, x, w, w); }
	TVec4<T> xyxx() const { return TVec4<T>(x, y, x, x); }
	TVec4<T> xyxy() const { return TVec4<T>(x, y, x, y); }
	TVec4<T> xyxz() const { return TVec4<T>(x, y, x, z); }
	TVec4<T> xyxw() const { return TVec4<T>(x, y, x, w); }
	TVec4<T> xyyx() const { return TVec4<T>(x, y, y, x); }
	TVec4<T> xyyy() const { return TVec4<T>(x, y, y, y); }
	TVec4<T> xyyz() const { return TVec4<T>(x, y, y, z); }
	TVec4<T> xyyw() const { return TVec4<T>(x, y, y, w); }
	TVec4<T> xyzx() const { return TVec4<T>(x, y, z, x); }
	TVec4<T> xyzy() const { return TVec4<T>(x, y, z, y); }
	TVec4<T> xyzz() const { return TVec4<T>(x, y, z, z); }
	TVec4<T> xyzw() const { return TVec4<T>(x, y, z, w); }
	TVec4<T> xywx() const { return TVec4<T>(x, y, w, x); }
	TVec4<T> xywy() const { return TVec4<T>(x, y, w, y); }
	TVec4<T> xywz() const { return TVec4<T>(x, y, w, z); }
	TVec4<T> xyww() const { return TVec4<T>(x, y, w, w); }
	TVec4<T> xzxx() const { return TVec4<T>(x, z, x, x); }
	TVec4<T> xzxy() const { return TVec4<T>(x, z, x, y); }
	TVec4<T> xzxz() const { return TVec4<T>(x, z, x, z); }
	TVec4<T> xzxw() const { return TVec4<T>(x, z, x, w); }
	TVec4<T> xzyx() const { return TVec4<T>(x, z, y, x); }
	TVec4<T> xzyy() const { return TVec4<T>(x, z, y, y); }
	TVec4<T> xzyz() const { return TVec4<T>(x, z, y, z); }
	TVec4<T> xzyw() const { return TVec4<T>(x, z, y, w); }
	TVec4<T> xzzx() const { return TVec4<T>(x, z, z, x); }
	TVec4<T> xzzy() const { return TVec4<T>(x, z, z, y); }
	TVec4<T> xzzz() const { return TVec4<T>(x, z, z, z); }
	TVec4<T> xzzw() const { return TVec4<T>(x, z, z, w); }
	TVec4<T> xzwx() const { return TVec4<T>(x, z, w, x); }
	TVec4<T> xzwy() const { return TVec4<T>(x, z, w, y); }
	TVec4<T> xzwz() const { return TVec4<T>(x, z, w, z); }
	TVec4<T> xzww() const { return TVec4<T>(x, z, w, w); }
	TVec4<T> xwxx() const { return TVec4<T>(x, w, x, x); }
	TVec4<T> xwxy() const { return TVec4<T>(x, w, x, y); }
	TVec4<T> xwxz() const { return TVec4<T>(x, w, x, z); }
	TVec4<T> xwxw() const { return TVec4<T>(x, w, x, w); }
	TVec4<T> xwyx() const { return TVec4<T>(x, w, y, x); }
	TVec4<T> xwyy() const { return TVec4<T>(x, w, y, y); }
	TVec4<T> xwyz() const { return TVec4<T>(x, w, y, z); }
	TVec4<T> xwyw() const { return TVec4<T>(x, w, y, w); }
	TVec4<T> xwzx() const { return TVec4<T>(x, w, z, x); }
	TVec4<T> xwzy() const { return TVec4<T>(x, w, z, y); }
	TVec4<T> xwzz() const { return TVec4<T>(x, w, z, z); }
	TVec4<T> xwzw() const { return TVec4<T>(x, w, z, w); }
	TVec4<T> xwwx() const { return TVec4<T>(x, w, w, x); }
	TVec4<T> xwwy() const { return TVec4<T>(x, w, w, y); }
	TVec4<T> xwwz() const { return TVec4<T>(x, w, w, z); }
	TVec4<T> xwww() const { return TVec4<T>(x, w, w, w); }
	TVec4<T> yxxx() const { return TVec4<T>(y, x, x, x); }
	TVec4<T> yxxy() const { return TVec4<T>(y, x, x, y); }
	TVec4<T> yxxz() const { return TVec4<T>(y, x, x, z); }
	TVec4<T> yxxw() const { return TVec4<T>(y, x, x, w); }
	TVec4<T> yxyx() const { return TVec4<T>(y, x, y, x); }
	TVec4<T> yxyy() const { return TVec4<T>(y, x, y, y); }
	TVec4<T> yxyz() const { return TVec4<T>(y, x, y, z); }
	TVec4<T> yxyw() const { return TVec4<T>(y, x, y, w); }
	TVec4<T> yxzx() const { return TVec4<T>(y, x, z, x); }
	TVec4<T> yxzy() const { return TVec4<T>(y, x, z, y); }
	TVec4<T> yxzz() const { return TVec4<T>(y, x, z, z); }
	TVec4<T> yxzw() const { return TVec4<T>(y, x, z, w); }
	TVec4<T> yxwx() const { return TVec4<T>(y, x, w, x); }
	TVec4<T> yxwy() const { return TVec4<T>(y, x, w, y); }
	TVec4<T> yxwz() const { return TVec4<T>(y, x, w, z); }
	TVec4<T> yxww() const { return TVec4<T>(y, x, w, w); }
	TVec4<T> yyxx() const { return TVec4<T>(y, y, x, x); }
	TVec4<T> yyxy() const { return TVec4<T>(y, y, x, y); }
	TVec4<T> yyxz() const { return TVec4<T>(y, y, x, z); }
	TVec4<T> yyxw() const { return TVec4<T>(y, y, x, w); }
	TVec4<T> yyyx() const { return TVec4<T>(y, y, y, x); }
	TVec4<T> yyyy() const { return TVec4<T>(y, y, y, y); }
	TVec4<T> yyyz() const { return TVec4<T>(y, y, y, z); }
	TVec4<T> yyyw() const { return TVec4<T>(y, y, y, w); }
	TVec4<T> yyzx() const { return TVec4<T>(y, y, z, x); }
	TVec4<T> yyzy() const { return TVec4<T>(y, y, z, y); }
	TVec4<T> yyzz() const { return TVec4<T>(y, y, z, z); }
	TVec4<T> yyzw() const { return TVec4<T>(y, y, z, w); }
	TVec4<T> yywx() const { return TVec4<T>(y, y, w, x); }
	TVec4<T> yywy() const { return TVec4<T>(y, y, w, y); }
	TVec4<T> yywz() const { return TVec4<T>(y, y, w, z); }
	TVec4<T> yyww() const { return TVec4<T>(y, y, w, w); }
	TVec4<T> yzxx() const { return TVec4<T>(y, z, x, x); }
	TVec4<T> yzxy() const { return TVec4<T>(y, z, x, y); }
	TVec4<T> yzxz() const { return TVec4<T>(y, z, x, z); }
	TVec4<T> yzxw() const { return TVec4<T>(y, z, x, w); }
	TVec4<T> yzyx() const { return TVec4<T>(y, z, y, x); }
	TVec4<T> yzyy() const { return TVec4<T>(y, z, y, y); }
	TVec4<T> yzyz() const { return TVec4<T>(y, z, y, z); }
	TVec4<T> yzyw() const { return TVec4<T>(y, z, y, w); }
	TVec4<T> yzzx() const { return TVec4<T>(y, z, z, x); }
	TVec4<T> yzzy() const { return TVec4<T>(y, z, z, y); }
	TVec4<T> yzzz() const { return TVec4<T>(y, z, z, z); }
	TVec4<T> yzzw() const { return TVec4<T>(y, z, z, w); }
	TVec4<T> yzwx() const { return TVec4<T>(y, z, w, x); }
	TVec4<T> yzwy() const { return TVec4<T>(y, z, w, y); }
	TVec4<T> yzwz() const { return TVec4<T>(y, z, w, z); }
	TVec4<T> yzww() const { return TVec4<T>(y, z, w, w); }
	TVec4<T> ywxx() const { return TVec4<T>(y, w, x, x); }
	TVec4<T> ywxy() const { return TVec4<T>(y, w, x, y); }
	TVec4<T> ywxz() const { return TVec4<T>(y, w, x, z); }
	TVec4<T> ywxw() const { return TVec4<T>(y, w, x, w); }
	TVec4<T> ywyx() const { return TVec4<T>(y, w, y, x); }
	TVec4<T> ywyy() const { return TVec4<T>(y, w, y, y); }
	TVec4<T> ywyz() const { return TVec4<T>(y, w, y, z); }
	TVec4<T> ywyw() const { return TVec4<T>(y, w, y, w); }
	TVec4<T> ywzx() const { return TVec4<T>(y, w, z, x); }
	TVec4<T> ywzy() const { return TVec4<T>(y, w, z, y); }
	TVec4<T> ywzz() const { return TVec4<T>(y, w, z, z); }
	TVec4<T> ywzw() const { return TVec4<T>(y, w, z, w); }
	TVec4<T> ywwx() const { return TVec4<T>(y, w, w, x); }
	TVec4<T> ywwy() const { return TVec4<T>(y, w, w, y); }
	TVec4<T> ywwz() const { return TVec4<T>(y, w, w, z); }
	TVec4<T> ywww() const { return TVec4<T>(y, w, w, w); }
	TVec4<T> zxxx() const { return TVec4<T>(z, x, x, x); }
	TVec4<T> zxxy() const { return TVec4<T>(z, x, x, y); }
	TVec4<T> zxxz() const { return TVec4<T>(z, x, x, z); }
	TVec4<T> zxxw() const { return TVec4<T>(z, x, x, w); }
	TVec4<T> zxyx() const { return TVec4<T>(z, x, y, x); }
	TVec4<T> zxyy() const { return TVec4<T>(z, x, y, y); }
	TVec4<T> zxyz() const { return TVec4<T>(z, x, y, z); }
	TVec4<T> zxyw() const { return TVec4<T>(z, x, y, w); }
	TVec4<T> zxzx() const { return TVec4<T>(z, x, z, x); }
	TVec4<T> zxzy() const { return TVec4<T>(z, x, z, y); }
	TVec4<T> zxzz() const { return TVec4<T>(z, x, z, z); }
	TVec4<T> zxzw() const { return TVec4<T>(z, x, z, w); }
	TVec4<T> zxwx() const { return TVec4<T>(z, x, w, x); }
	TVec4<T> zxwy() const { return TVec4<T>(z, x, w, y); }
	TVec4<T> zxwz() const { return TVec4<T>(z, x, w, z); }
	TVec4<T> zxww() const { return TVec4<T>(z, x, w, w); }
	TVec4<T> zyxx() const { return TVec4<T>(z, y, x, x); }
	TVec4<T> zyxy() const { return TVec4<T>(z, y, x, y); }
	TVec4<T> zyxz() const { return TVec4<T>(z, y, x, z); }
	TVec4<T> zyxw() const { return TVec4<T>(z, y, x, w); }
	TVec4<T> zyyx() const { return TVec4<T>(z, y, y, x); }
	TVec4<T> zyyy() const { return TVec4<T>(z, y, y, y); }
	TVec4<T> zyyz() const { return TVec4<T>(z, y, y, z); }
	TVec4<T> zyyw() const { return TVec4<T>(z, y, y, w); }
	TVec4<T> zyzx() const { return TVec4<T>(z, y, z, x); }
	TVec4<T> zyzy() const { return TVec4<T>(z, y, z, y); }
	TVec4<T> zyzz() const { return TVec4<T>(z, y, z, z); }
	TVec4<T> zyzw() const { return TVec4<T>(z, y, z, w); }
	TVec4<T> zywx() const { return TVec4<T>(z, y, w, x); }
	TVec4<T> zywy() const { return TVec4<T>(z, y, w, y); }
	TVec4<T> zywz() const { return TVec4<T>(z, y, w, z); }
	TVec4<T> zyww() const { return TVec4<T>(z, y, w, w); }
	TVec4<T> zzxx() const { return TVec4<T>(z, z, x, x); }
	TVec4<T> zzxy() const { return TVec4<T>(z, z, x, y); }
	TVec4<T> zzxz() const { return TVec4<T>(z, z, x, z); }
	TVec4<T> zzxw() const { return TVec4<T>(z, z, x, w); }
	TVec4<T> zzyx() const { return TVec4<T>(z, z, y, x); }
	TVec4<T> zzyy() const { return TVec4<T>(z, z, y, y); }
	TVec4<T> zzyz() const { return TVec4<T>(z, z, y, z); }
	TVec4<T> zzyw() const { return TVec4<T>(z, z, y, w); }
	TVec4<T> zzzx() const { return TVec4<T>(z, z, z, x); }
	TVec4<T> zzzy() const { return TVec4<T>(z, z, z, y); }
	TVec4<T> zzzz() const { return TVec4<T>(z, z, z, z); }
	TVec4<T> zzzw() const { return TVec4<T>(z, z, z, w); }
	TVec4<T> zzwx() const { return TVec4<T>(z, z, w, x); }
	TVec4<T> zzwy() const { return TVec4<T>(z, z, w, y); }
	TVec4<T> zzwz() const { return TVec4<T>(z, z, w, z); }
	TVec4<T> zzww() const { return TVec4<T>(z, z, w, w); }
	TVec4<T> zwxx() const { return TVec4<T>(z, w, x, x); }
	TVec4<T> zwxy() const { return TVec4<T>(z, w, x, y); }
	TVec4<T> zwxz() const { return TVec4<T>(z, w, x, z); }
	TVec4<T> zwxw() const { return TVec4<T>(z, w, x, w); }
	TVec4<T> zwyx() const { return TVec4<T>(z, w, y, x); }
	TVec4<T> zwyy() const { return TVec4<T>(z, w, y, y); }
	TVec4<T> zwyz() const { return TVec4<T>(z, w, y, z); }
	TVec4<T> zwyw() const { return TVec4<T>(z, w, y, w); }
	TVec4<T> zwzx() const { return TVec4<T>(z, w, z, x); }
	TVec4<T> zwzy() const { return TVec4<T>(z, w, z, y); }
	TVec4<T> zwzz() const { return TVec4<T>(z, w, z, z); }
	TVec4<T> zwzw() const { return TVec4<T>(z, w, z, w); }
	TVec4<T> zwwx() const { return TVec4<T>(z, w, w, x); }
	TVec4<T> zwwy() const { return TVec4<T>(z, w, w, y); }
	TVec4<T> zwwz() const { return TVec4<T>(z, w, w, z); }
	TVec4<T> zwww() const { return TVec4<T>(z, w, w, w); }
	TVec4<T> wxxx() const { return TVec4<T>(w, x, x, x); }
	TVec4<T> wxxy() const { return TVec4<T>(w, x, x, y); }
	TVec4<T> wxxz() const { return TVec4<T>(w, x, x, z); }
	TVec4<T> wxxw() const { return TVec4<T>(w, x, x, w); }
	TVec4<T> wxyx() const { return TVec4<T>(w, x, y, x); }
	TVec4<T> wxyy() const { return TVec4<T>(w, x, y, y); }
	TVec4<T> wxyz() const { return TVec4<T>(w, x, y, z); }
	TVec4<T> wxyw() const { return TVec4<T>(w, x, y, w); }
	TVec4<T> wxzx() const { return TVec4<T>(w, x, z, x); }
	TVec4<T> wxzy() const { return TVec4<T>(w, x, z, y); }
	TVec4<T> wxzz() const { return TVec4<T>(w, x, z, z); }
	TVec4<T> wxzw() const { return TVec4<T>(w, x, z, w); }
	TVec4<T> wxwx() const { return TVec4<T>(w, x, w, x); }
	TVec4<T> wxwy() const { return TVec4<T>(w, x, w, y); }
	TVec4<T> wxwz() const { return TVec4<T>(w, x, w, z); }
	TVec4<T> wxww() const { return TVec4<T>(w, x, w, w); }
	TVec4<T> wyxx() const { return TVec4<T>(w, y, x, x); }
	TVec4<T> wyxy() const { return TVec4<T>(w, y, x, y); }
	TVec4<T> wyxz() const { return TVec4<T>(w, y, x, z); }
	TVec4<T> wyxw() const { return TVec4<T>(w, y, x, w); }
	TVec4<T> wyyx() const { return TVec4<T>(w, y, y, x); }
	TVec4<T> wyyy() const { return TVec4<T>(w, y, y, y); }
	TVec4<T> wyyz() const { return TVec4<T>(w, y, y, z); }
	TVec4<T> wyyw() const { return TVec4<T>(w, y, y, w); }
	TVec4<T> wyzx() const { return TVec4<T>(w, y, z, x); }
	TVec4<T> wyzy() const { return TVec4<T>(w, y, z, y); }
	TVec4<T> wyzz() const { return TVec4<T>(w, y, z, z); }
	TVec4<T> wyzw() const { return TVec4<T>(w, y, z, w); }
	TVec4<T> wywx() const { return TVec4<T>(w, y, w, x); }
	TVec4<T> wywy() const { return TVec4<T>(w, y, w, y); }
	TVec4<T> wywz() const { return TVec4<T>(w, y, w, z); }
	TVec4<T> wyww() const { return TVec4<T>(w, y, w, w); }
	TVec4<T> wzxx() const { return TVec4<T>(w, z, x, x); }
	TVec4<T> wzxy() const { return TVec4<T>(w, z, x, y); }
	TVec4<T> wzxz() const { return TVec4<T>(w, z, x, z); }
	TVec4<T> wzxw() const { return TVec4<T>(w, z, x, w); }
	TVec4<T> wzyx() const { return TVec4<T>(w, z, y, x); }
	TVec4<T> wzyy() const { return TVec4<T>(w, z, y, y); }
	TVec4<T> wzyz() const { return TVec4<T>(w, z, y, z); }
	TVec4<T> wzyw() const { return TVec4<T>(w, z, y, w); }
	TVec4<T> wzzx() const { return TVec4<T>(w, z, z, x); }
	TVec4<T> wzzy() const { return TVec4<T>(w, z, z, y); }
	TVec4<T> wzzz() const { return TVec4<T>(w, z, z, z); }
	TVec4<T> wzzw() const { return TVec4<T>(w, z, z, w); }
	TVec4<T> wzwx() const { return TVec4<T>(w, z, w, x); }
	TVec4<T> wzwy() const { return TVec4<T>(w, z, w, y); }
	TVec4<T> wzwz() const { return TVec4<T>(w, z, w, z); }
	TVec4<T> wzww() const { return TVec4<T>(w, z, w, w); }
	TVec4<T> wwxx() const { return TVec4<T>(w, w, x, x); }
	TVec4<T> wwxy() const { return TVec4<T>(w, w, x, y); }
	TVec4<T> wwxz() const { return TVec4<T>(w, w, x, z); }
	TVec4<T> wwxw() const { return TVec4<T>(w, w, x, w); }
	TVec4<T> wwyx() const { return TVec4<T>(w, w, y, x); }
	TVec4<T> wwyy() const { return TVec4<T>(w, w, y, y); }
	TVec4<T> wwyz() const { return TVec4<T>(w, w, y, z); }
	TVec4<T> wwyw() const { return TVec4<T>(w, w, y, w); }
	TVec4<T> wwzx() const { return TVec4<T>(w, w, z, x); }
	TVec4<T> wwzy() const { return TVec4<T>(w, w, z, y); }
	TVec4<T> wwzz() const { return TVec4<T>(w, w, z, z); }
	TVec4<T> wwzw() const { return TVec4<T>(w, w, z, w); }
	TVec4<T> wwwx() const { return TVec4<T>(w, w, w, x); }
	TVec4<T> wwwy() const { return TVec4<T>(w, w, w, y); }
	TVec4<T> wwwz() const { return TVec4<T>(w, w, w, z); }
	TVec4<T> wwww() const { return TVec4<T>(w, w, w, w); }

	friend std::ostream& operator<<( std::ostream& lhs, const TVec4<T>& rhs )
	{
		lhs << "[" << rhs.x << "," << rhs.y << "," << rhs.z << "," << rhs.w << "]";
		return lhs;
	}

	static TVec4<T> xAxis() { return TVec4<T>( 1, 0, 0, 0 ); }
	static TVec4<T> yAxis() { return TVec4<T>( 0, 1, 0, 0 ); }
	static TVec4<T> zAxis() { return TVec4<T>( 0, 0, 1, 0 ); }
	static TVec4<T> wAxis() { return TVec4<T>( 0, 0, 0, 1 ); }

	static TVec4<T> NaN()   { return TVec4<T>( tmath<T>::NaN(), tmath<T>::NaN(), tmath<T>::NaN(), tmath<T>::NaN() ); }
};

//! Converts a coordinate from rectangular (Cartesian) coordinates to polar coordinates of the form (radius, theta)
template<typename T>
TVec2<T> toPolar( TVec2<T> car )
{
	const T epsilon = (T)0.0000001;
	T theta;
	if( tmath<T>::abs( car.x ) < epsilon ) { // x == 0
		if( tmath<T>::abs( car.y ) < epsilon ) theta = 0;
		else if( car.y > 0 ) theta = (T)M_PI / 2;
		else theta = ( (T)M_PI * 3 ) / 2;
	}
	else if ( car.x > 0 ) {
		if( car.y < 0 ) theta = tmath<T>::atan( car.y / car.x ) + 2 * (T)M_PI;
		else theta = tmath<T>::atan( car.y / car.x );
	}
	else // car.x < 0
		theta = (tmath<T>::atan( car.y / car.x ) + M_PI );

	return TVec2<T>( car.length(), theta );
}

//! Converts a coordinate from polar coordinates of the form (radius, theta) to rectangular coordinates
template<typename T>
TVec2<T> fromPolar( TVec2<T> pol )
{
	return TVec2<T>( tmath<T>::cos( pol.y ) *  pol.x , tmath<T>::sin( pol.y ) *  pol.x );
}

template<typename T,typename Y> inline TVec2<T> operator *( Y s, const TVec2<T> &v ) { return TVec2<T>( v.x * s, v.y * s ); }
template<typename T,typename Y> inline TVec2<T> operator *( const TVec2<T> &v, Y s ) { return TVec2<T>( v.x * s, v.y * s ); }
template<typename T,typename Y> inline TVec3<T> operator *( Y s, const TVec3<T> &v ) { return TVec3<T>( v.x * s, v.y * s, v.z * s ); }
template<typename T,typename Y> inline TVec3<T> operator *( const TVec3<T> &v, Y s ) { return TVec3<T>( v.x * s, v.y * s, v.z * s ); }
template<typename T,typename Y> inline TVec4<T> operator *( Y s, const TVec4<T> &v ) { return TVec4<T>( v.x * s, v.y * s, v.z * s, v.w * s ); }
template<typename T,typename Y> inline TVec4<T> operator *( const TVec4<T> &v, Y s ) { return TVec4<T>( v.x * s, v.y * s, v.z * s, v.w * s ); }

template <typename T> T dot( const TVec2<T>& a, const TVec2<T>& b ) { return a.dot( b ); }
template <typename T> T dot( const TVec3<T>& a, const TVec3<T>& b ) { return a.dot( b ); }
template <typename T> T dot( const TVec4<T>& a, const TVec4<T>& b ) { return a.dot( b ); }

template <typename T> TVec3<T> cross( const TVec3<T>& a, const TVec3<T>& b ) { return a.cross( b ); }
template <typename T> TVec4<T> cross( const TVec4<T>& a, const TVec4<T>& b ) { return a.cross( b ); }

template <typename T> bool isNaN( const TVec2<T>& a ) { return std::isnan( a.x ) || std::isnan( a.y ); }
template <typename T> bool isNaN( const TVec3<T>& a ) { return std::isnan( a.x ) || std::isnan( a.y ) || std::isnan( a.z ); }
template <typename T> bool isNaN( const TVec4<T>& a ) { return std::isnan( a.x ) || std::isnan( a.y ) || std::isnan( a.z ) || std::isnan( a.w ); }


///////////////////////////////////////////////////////////////////////////////////////////////////
//		Typedefs
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef TVec2<uint32_t>		Vec2ui;
typedef TVec2<int32_t>		Vec2i;
typedef TVec2<float>		Vec2f;
typedef TVec2<double>		Vec2d;

typedef TVec3<uint32_t>		Vec3ui;
typedef TVec3<int32_t>		Vec3i;
typedef TVec3<float>		Vec3f;
typedef TVec3<double>		Vec3d;

typedef TVec4<uint32_t>		Vec4ui;
typedef TVec4<int32_t>		Vec4i;
typedef TVec4<float>		Vec4f;
typedef TVec4<double>		Vec4d;

