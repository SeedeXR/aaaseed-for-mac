
#ifdef AAA_TMATRIX_22_H
#error "TMATRIX_22_H included more than once."
#endif
#define AAA_TMATRIX_22_H 1


#ifndef AAA_NATIVE_SYSTEM_H
#	include "Native_System.h"
#endif
#ifndef AAA_TMATH_H
#	include "math/TMath.h"
#endif
#ifndef AAA_TVECTOR_H
#	include "math/TVector.h"
#endif

#ifndef _IOMANIP_
#	include <iomanip>
#endif



//////////////////////////////////////////////////////////////////////////////////////////////////////
// TMatrix22
template< typename T >
class TMatrix22
{
public:
	typedef T	TYPE;
	typedef T	value_type;
	//
	static const size_t DIM		= 2;
	static const size_t DIM_SQ	= DIM*DIM;
	static const size_t MEM_LEN	= sizeof(T)*DIM_SQ;

	//
	// This class is OpenGL friendly and stores the m as how OpenGL would expect it.
	// m[i,j]:
	// | m[0,0] m[0,1] |
	// | m[1,0] m[1,1] |
	//
	// m[idx]
	// | m[0] m[2] |
	// | m[1] m[3] |
	//
	union {
		T m[4];
		struct {
			// This looks like it's transposed from the above, but it's really not.
			// It just has to be written this way so it follows the right ordering
			// in the memory layout as well as being mathematically correct.
			T m00, m10;
			T m01, m11;
		};
		// [Cols][Rows]
		T mcols[2][2];
	};
	
	TMatrix22();

	TMatrix22( T s );

	// OpenGL layout - unless srcIsRowMajor is true
	TMatrix22( const T *dt, bool srcIsRowMajor = false );

	// OpenGL layout: m[0]=d0, m[1]=d1, m[2]=d2, m[3]=d3 - unless srcIsRowMajor is true
	TMatrix22( T d0, T d1, T d2, T d3, bool srcIsRowMajor = false );

	// Creates matrix with column vectors vx and vy
	TMatrix22( const TVec2<T> &vx, const TVec2<T> &vy );

	template< typename FromT >
	TMatrix22( const TMatrix22<FromT>& src );

	TMatrix22( const TMatrix22<T>& src );

						operator T*() { return (T*)m; }
						operator const T*() const { return (const T*)m; }

	TMatrix22<T>&		operator=( const TMatrix22<T>& rhs );
	TMatrix22<T>&		operator=( const T &rhs );

	template< typename FromT >
	TMatrix22<T>&		operator=( const TMatrix22<FromT>& rhs );

	bool				equalCompare( const TMatrix22<T>& rhs, T epsilon ) const;
	bool				operator==( const TMatrix22<T> &rhs ) const { return equalCompare( rhs, (T)EPSILON ); }
	bool				operator!=( const TMatrix22<T> &rhs ) const { return ! ( *this == rhs ); }

	TMatrix22<T>&		operator*=( const TMatrix22<T> &rhs );
	TMatrix22<T>&		operator+=( const TMatrix22<T> &rhs );
	TMatrix22<T>&		operator-=( const TMatrix22<T> &rhs );

	TMatrix22<T>&		operator*=( T s );
	TMatrix22<T>&		operator/=( T s );
	TMatrix22<T>&		operator+=( T s );
	TMatrix22<T>&		operator-=( T s );

	const TMatrix22<T>	operator*( const TMatrix22<T> &rhs ) const;
	const TMatrix22<T>	operator+( const TMatrix22<T> &rhs ) const;
	const TMatrix22<T>	operator-( const TMatrix22<T> &rhs ) const;

	// post-multiplies column vector [rhs.x rhs.y]
	const TVec2<T>		operator*( const TVec2<T> &rhs ) const;

	const TMatrix22<T>	operator*( T rhs ) const;
	const TMatrix22<T>	operator/( T rhs ) const;
	const TMatrix22<T>	operator+( T rhs ) const;
	const TMatrix22<T>	operator-( T rhs ) const;

	// Accessors
	T&					at( int row, int col );
	const T&			at( int row, int col ) const;

	// OpenGL layout - unless srcIsRowMajor is true
	void				set( const T *dt, bool srcIsRowMajor = false );
	// OpenGL layout: m[0]=d0, m[1]=d1, m[2]=d2, m[3]=d3 - unless srcIsRowMajor is true
	void				set( T d0, T d1, T d2, T d3, bool srcIsRowMajor = false );

	TVec2<T>				getColumn( int col ) const;
	void				setColumn( int col, const TVec2<T> &v );

	TVec2<T>				getRow( int row ) const;
	void				setRow( int row, const TVec2<T> &v );

	void				getColumns( TVec2<T> *c0, TVec2<T> *c1 ) const;
	void				setColumns( const TVec2<T> &c0, const TVec2<T> &c1 );

	void				getRows( TVec2<T> *r0, TVec2<T> *r1 ) const;
	void				setRows( const TVec2<T> &r0, const TVec2<T> &r1 );

	void				setToNull();
	void				setToIdentity();

	T					determinant() const;
	T					trace() const;

	TMatrix22<T>         diagonal() const;

	TMatrix22<T>			lowerTriangular() const;
	TMatrix22<T>			upperTriangular() const;

	void				transpose();
	TMatrix22<T>			transposed() const;

	void				invert (T epsilon = EPSILON ) { *this = inverted( epsilon ); }
	TMatrix22<T>			inverted( T epsilon = EPSILON ) const;

	// pre-multiplies row vector v - no divide by w
	TVec2<T>				preMultiply( const TVec2<T> &v ) const;

	// post-multiplies column vector v - no divide by w
	TVec2<T>				postMultiply( const TVec2<T> &v ) const;

	// post-multiplies column vector [rhs.x rhs.y]
	TVec2<T>				transformVec( const TVec2<T> &v ) const { return postMultiply( v ); }

	// rotate by radians (conceptually, rotate is before 'this')
	void				rotate( T radians ) { TMatrix22 rot = createRotation( radians ); TMatrix22 mat = *this; *this = rot*mat; }

	// concatenate scale (conceptually, scale is before 'this')
	void				scale( T s ) { TMatrix22 sc = createScale( s ); TMatrix22 mat = *this; *this = sc*mat; }
	void				scale( const TVec2<T> &v ) { TMatrix22 sc = createScale( v ); TMatrix22 mat = *this; *this = sc*mat; }

	// transposes rotation sub-matrix and inverts translation
	TMatrix22<T>			invertTransform() const;

	// returns an identity matrix
	static TMatrix22<T>	identity() { return TMatrix22( 1, 0, 0, 1 ); }
	// returns 1 filled matrix
	static TMatrix22<T>  one() { return TMatrix22( (T)1 ); }
	// returns 0 filled matrix
	static TMatrix22<T>  zero() { return TMatrix22( (T)0 ); }

	// creates rotation matrix
	static TMatrix22<T>	createRotation( T radians );

	// creates scale matrix
	static TMatrix22<T>	createScale( T s );
	static TMatrix22<T>	createScale( const TVec2<T> &v );
			
	friend std::ostream& operator<<( std::ostream& lhs, const TMatrix22<T>& rhs ) {
		for( int i = 0; i < 2; i++ ) {
			lhs << "|\t";
			for( int j = 0; j < 2; j++ ) {
				lhs << rhs.at( i, j ) << "\t";
			}
			lhs << "|" << std::endl;
		}
		
		return lhs;
	}

};

template< typename T >
TMatrix22<T>::TMatrix22()
{
	setToIdentity();
}

template< typename T >
TMatrix22<T>::TMatrix22( T s )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] = s;
	}
}

template< typename T >
TMatrix22<T>::TMatrix22( const T *dt, bool srcIsRowMajor )
{
	set( dt, srcIsRowMajor );
}

template< typename T >
TMatrix22<T>::TMatrix22( T d0, T d1, T d2, T d3, bool srcIsRowMajor )
{
	set( d0, d1, 
		d2, d3, srcIsRowMajor );
}

template< typename T >
TMatrix22<T>::TMatrix22( const TVec2<T> &vx, const TVec2<T> &vy )
{
	m00 = vx.x; m01 = vy.x;
	m10 = vx.y; m11 = vy.y;
}

template< typename T >
template< typename FromT >
TMatrix22<T>::TMatrix22( const TMatrix22<FromT>& src )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] = static_cast<T>( src.m[i] );
	}
}

template< typename T >
TMatrix22<T>::TMatrix22( const TMatrix22<T>& src )
{
	memcpy( m, src.m, MEM_LEN );
}

template< typename T >
TMatrix22<T>& TMatrix22<T>::operator=( const TMatrix22<T>& rhs )
{
	memcpy( m, rhs.m, MEM_LEN );
	return *this;
}

template< typename T >
TMatrix22<T>& TMatrix22<T>::operator=( const T &rhs )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] = rhs;
	}
	return *this;
}

template< typename T >
template< typename FromT >
TMatrix22<T>& TMatrix22<T>::operator=( const TMatrix22<FromT>& rhs )
{
	for( int i = 0; i < DIM_SQ; i++ ) {
		m[i] = static_cast<T>(rhs.m[i]);
	}
	return *this;
}

template< typename T >
bool TMatrix22<T>::equalCompare( const TMatrix22<T>& rhs, T epsilon ) const
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		T diff = fabs( m[i] - rhs.m[i] );
		if( diff >= epsilon )
			return false;
	}
	return true;
}

template< typename T >
TMatrix22<T>& TMatrix22<T>::operator*=( const TMatrix22<T> &rhs )
{
	TMatrix22<T> mat;

	mat.m[0] = m[0]*rhs.m[0] + m[2]*rhs.m[1];
	mat.m[1] = m[1]*rhs.m[0] + m[3]*rhs.m[1];

	mat.m[2] = m[0]*rhs.m[2] + m[2]*rhs.m[3];
	mat.m[3] = m[1]*rhs.m[2] + m[3]*rhs.m[3];

	*this = mat;

	return *this;
}

template< typename T >
TMatrix22<T>& TMatrix22<T>::operator+=( const TMatrix22<T> &rhs )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] += rhs.m[i];
	}
	return *this;
}

template< typename T >
TMatrix22<T>& TMatrix22<T>::operator-=( const TMatrix22<T> &rhs )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] -= rhs.m[i];
	}
	return *this;
}

template< typename T >
TMatrix22<T>& TMatrix22<T>::operator*=( T s )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] *= s;
	}
	return *this;
}

template< typename T >
TMatrix22<T>& TMatrix22<T>::operator/=( T s )
{
	T invS = (T)1/s;
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] *= invS;
	}
	return *this;
}

template< typename T >
TMatrix22<T>& TMatrix22<T>::operator+=( T s )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] += s;
	}
	return *this;
}

template< typename T >
TMatrix22<T>& TMatrix22<T>::operator-=( T s )
{
	for( int i = 0; i < DIM_SQ; ++i ) {
		m[i] -= s;
	}
	return *this;
}

template< typename T >
const TMatrix22<T> TMatrix22<T>::operator*( const TMatrix22<T> &rhs ) const
{
	TMatrix22<T> ret;

	ret.m[0] = m[0]*rhs.m[0] + m[2]*rhs.m[1];
	ret.m[1] = m[1]*rhs.m[0] + m[3]*rhs.m[1];

	ret.m[2] = m[0]*rhs.m[2] + m[2]*rhs.m[3];
	ret.m[3] = m[1]*rhs.m[2] + m[3]*rhs.m[3];

	return ret;
}

template< typename T >
const TMatrix22<T> TMatrix22<T>::operator+( const TMatrix22<T> &rhs ) const
{
	TMatrix22<T> ret;
	for( int i = 0; i < DIM_SQ; ++i ) {
		ret.m[i] = m[i] + rhs.m[i];
	}
	return ret;
}

template< typename T >
const TMatrix22<T> TMatrix22<T>::operator-( const TMatrix22<T> &rhs ) const
{
	TMatrix22<T> ret;
	for( int i = 0; i < DIM_SQ; ++i ) {
		ret.m[i] = m[i] - rhs.m[i];
	}
	return ret;
}

template< typename T >
const TVec2<T> TMatrix22<T>::operator*( const TVec2<T> &rhs ) const
{
	return TVec2<T>(
		m[0]*rhs.x + m[2]*rhs.y,
		m[1]*rhs.x + m[3]*rhs.y
		);
}


template< typename T >
const TMatrix22<T> TMatrix22<T>::operator*( T rhs ) const
{
	TMatrix22<T> ret;
	for( int i = 0; i < DIM_SQ; ++i ) {
		ret.m[i] = m[i]*rhs;
	}
	return ret;
}

template< typename T >
const TMatrix22<T> TMatrix22<T>::operator/( T rhs ) const
{
	TMatrix22<T> ret;
	T s = (T)1/rhs;
	for( int i = 0; i < DIM_SQ; ++i ) {
		ret.m[i] = m[i]*s;
	}
	return ret;
}

template< typename T >
const TMatrix22<T> TMatrix22<T>::operator+( T rhs ) const
{
	TMatrix22<T> ret;
	for( int i = 0; i < DIM_SQ; ++i ) {
		ret.m[i] = m[i] + rhs;
	}
	return ret;
}

template< typename T >
const TMatrix22<T> TMatrix22<T>::operator-( T rhs ) const
{
	TMatrix22<T> ret;
	for( int i = 0; i < DIM_SQ; ++i ) {
		ret.m[i] = m[i] - rhs;
	}
	return ret;
}

template< typename T >
T& TMatrix22<T>::at( int row, int col ) 
{
	assert(row >= 0 && row < DIM);
	assert(col >= 0 && col < DIM);
	return m[col*DIM + row];
}

template< typename T >
const T& TMatrix22<T>::at( int row, int col ) const 
{
	assert(row >= 0 && row < DIM);
	assert(col >= 0 && col < DIM);
	return m[col*DIM + row];
}

template< typename T >
void TMatrix22<T>::set( const T *dt, bool srcIsRowMajor )
{
	if( srcIsRowMajor ) {
		m00 = dt[0]; m01 = dt[2];
		m10 = dt[1]; m11 = dt[3];
	}
	else {
		memcpy( m, dt, MEM_LEN );
	}
}

template< typename T >
void TMatrix22<T>::set( T d0, T d1, T d2, T d3, bool srcIsRowMajor )
{
	if( srcIsRowMajor ) {
		m[0] = d0; m[2] = d1;
		m[1] = d2; m[3] = d3;
	} 
	else {
		m[0] = d0; m[2] = d2;
		m[1] = d1; m[3] = d3;
	}
}

template< typename T >
TVec2<T> TMatrix22<T>::getColumn( int col ) const
{
	size_t i = col*DIM;
	return TVec2<T>( 
		m[i + 0], 
		m[i + 1]
	);
}

template< typename T >
void TMatrix22<T>::setColumn( int col, const TVec2<T> &v )
{
	size_t i = col*DIM;
	m[i + 0] = v.x;
	m[i + 1] = v.y;
}

template< typename T >
TVec2<T> TMatrix22<T>::getRow( int row ) const 
{ 
	return TVec2<T>( 
		m[row + 0],
		m[row + 2]
	); 
}

template< typename T >
void TMatrix22<T>::setRow( int row, const TVec2<T> &v ) 
{ 
	m[row + 0] = v.x; 
	m[row + 2] = v.y;
}

template< typename T >
void TMatrix22<T>::getColumns( TVec2<T> *c0, TVec2<T> *c1 ) const
{
	*c0 = getColumn( 0 );
	*c1 = getColumn( 1 );
}

template< typename T >
void TMatrix22<T>::setColumns( const TVec2<T> &c0, const TVec2<T> &c1 )
{
	setColumn( 0, c0 );
	setColumn( 1, c1 );
}

template< typename T >
void TMatrix22<T>::getRows( TVec2<T> *r0, TVec2<T> *r1 ) const
{
	*r0 = getRow( 0 );
	*r1 = getRow( 1 );
}

template< typename T >
void TMatrix22<T>::setRows( const TVec2<T> &r0, const TVec2<T> &r1 )
{
	setRow( 0, r0 );
	setRow( 1, r1 );
}

template< typename T >
void TMatrix22<T>::setToNull()
{
	std::memset( m, 0, MEM_LEN );
}

template< typename T >
void TMatrix22<T>::setToIdentity()
{
	m00 = 1; m01 = 0;
	m10 = 0; m11 = 1;
}

template< typename T >
T TMatrix22<T>::determinant() const
{
	T det  = m[0]*m[3] - m[1]*m[2];
	return det;
}

template< typename T >
T TMatrix22<T>::trace() const
{
	return m00 + m11;
}

template< typename T >
TMatrix22<T> TMatrix22<T>::diagonal() const
{
	TMatrix22 ret;
	ret.m00 = m00; ret.m01 =   0;
	ret.m10 =   0; ret.m11 = m11;
	return ret;
}

template< typename T >
TMatrix22<T> TMatrix22<T>::lowerTriangular() const
{
	TMatrix22 ret;
	ret.m00 = m00; ret.m01 =   0;
	ret.m10 = m10; ret.m11 = m11;	
	return ret;
}

template< typename T >
TMatrix22<T> TMatrix22<T>::upperTriangular() const
{
	TMatrix22 ret;
	ret.m00 = m00; ret.m01 = m01;
	ret.m10 =   0; ret.m11 = m11;
	return ret;
}

template< typename T >
void TMatrix22<T>::transpose()
{
	T t;
	t = m01; m01 = m10; m10 = t;
}

template< typename T >
TMatrix22<T> TMatrix22<T>::transposed() const
{
	return TMatrix22<T>( 
		m[ 0], m[ 2],
		m[ 1], m[ 3]
	);
}

template< typename T >
TMatrix22<T> TMatrix22<T>::inverted( T epsilon ) const
{
	TMatrix22<T> inv( (T)0 );

	T det = m[0]*m[3] - m[1]*m[2];

	if( fabs( det ) > epsilon ) {
		T invDet = (T)1/det;
		inv.m[0] =  m[3]*invDet;
		inv.m[1] = -m[1]*invDet;
		inv.m[2] = -m[2]*invDet;
		inv.m[3] =  m[0]*invDet;
	}

	return inv;
}

template< typename T >
TVec2<T> TMatrix22<T>::preMultiply( const TVec2<T> &v ) const
{
	return TVec2<T>(
		v.x*m00 + v.y*m10,
		v.x*m01 + v.y*m11
		);
}

template< typename T >
TVec2<T> TMatrix22<T>::postMultiply( const TVec2<T> &v ) const
{
	return TVec2<T>(
		m00*v.x + m01*v.y,
		m10*v.x + m11*v.y
		);
}

template< typename T >
TMatrix22<T> TMatrix22<T>::invertTransform() const
{
	TMatrix22<T> ret;

	// transpose rotation part
	for( int i = 0; i < DIM; i++ ) {
		for( int j = 0; j < DIM; j++ ) {
			ret.at( j, i ) = at( i, j );
		}
	}

	return ret;
}

template< typename T >
TMatrix22<T> TMatrix22<T>::createRotation( T radians )
{
	TMatrix22<T> ret;
	T ac = cos( radians );
	T as = sin( radians );
	ret.m00 =  ac; ret.m01 = as;
	ret.m10 = -as; ret.m11 = ac;
	return ret;
}

template< typename T >
TMatrix22<T> TMatrix22<T>::createScale( T s )
{
	TMatrix22<T> ret;
	ret.m00 = s; ret.m01 = 0;
	ret.m10 = 0; ret.m11 = s;
	return ret;
}

template< typename T >
TMatrix22<T> TMatrix22<T>::createScale( const TVec2<T> &v )
{
	TMatrix22<T> ret;
	ret.m00 = v.x; ret.m01 =   0;
	ret.m10 =   0; ret.m11 = v.y;
	return ret;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//		Typedefs
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef TMatrix22<float>	Matrix22f;
typedef TMatrix22<double>	Matrix22d;


