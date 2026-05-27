
#ifdef AAA_TMATRIX_AFFINE_22_H
#error "TMATRIX_AFFINE_22_H included more than once."
#endif
#define AAA_TMATRIX_AFFINE_22_H 1


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
// TMatrixAffine2
//! Represents a two dimensional affine transformation
template<typename T>
class TMatrixAffine2
{
  public:
	typedef T	TYPE;
	typedef T	value_type;
	//
	static const size_t MEM_LEN	= sizeof(T)*6;

	//
	// This class is OpenGL friendly and stores the m as how OpenGL would expect it.
	// m[i,j]:
	// | m[0,0] m[0,1] m[0,2] |
	// | m[1,0] m[1,1] m[1,2] |
	//
	// m[idx]
	// | m[0] m[2] m[4] |
	// | m[1] m[3] m[5] |
	//
	// mathematically this is:
	// | m[0] m[2] m[4] |
	// | m[1] m[3] m[5] |
	// |  0    0    1   |
	union {
		T m[6];
		struct {
			// This looks like it's transposed from the above, but it's really not.
			// It just has to be written this way so it follows the right ordering
			// in the memory layout as well as being mathematically correct.
			T m00, m10;
			T m01, m11;
			T m02, m12;
		};
		// [Cols][Rows]
		T mcols[3][2];
	};
	
	TMatrixAffine2();
	TMatrixAffine2( T s );
	TMatrixAffine2( const T *dt );
	// m[0]=d0, m[1]=d1, m[2]=d2 ... m[5]=d5
	TMatrixAffine2( T d0, T d1, T d2, T d3, T d4, T d5 );
	// Creates matrix with column vectors vx, vy and z
	TMatrixAffine2( const TVec2<T> &vx, const TVec2<T> &vy, const TVec2<T> &vz ); 

	template<typename FromT>
	TMatrixAffine2( const TMatrixAffine2<FromT> &src );
	TMatrixAffine2( const TMatrixAffine2<T> &src );

	TMatrixAffine2<T>&		operator=( const TMatrixAffine2<T>& rhs );
	TMatrixAffine2<T>&		operator=( T rhs );

	template<typename FromT>
	TMatrixAffine2<T>&		operator=( const TMatrixAffine2<FromT> &rhs );

	bool				equalCompare( const TMatrixAffine2<T>& rhs, T epsilon ) const;
	bool				operator==( const TMatrixAffine2<T> &rhs ) const { return equalCompare( rhs, (T)EPSILON ); }
	bool				operator!=( const TMatrixAffine2<T> &rhs ) const { return ! ( *this == rhs ); }

	TMatrixAffine2<T>&		operator*=( const TMatrixAffine2<T> &rhs );
	TMatrixAffine2<T>&		operator+=( const TMatrixAffine2<T> &rhs );
	TMatrixAffine2<T>&		operator-=( const TMatrixAffine2<T> &rhs );

	TMatrixAffine2<T>&		operator*=( T s );
	TMatrixAffine2<T>&		operator/=( T s );
	TMatrixAffine2<T>&		operator+=( T s );
	TMatrixAffine2<T>&		operator-=( T s );

	const TMatrixAffine2<T>	operator*( const TMatrixAffine2<T> &rhs ) const;
	const TMatrixAffine2<T>	operator+( const TMatrixAffine2<T> &rhs ) const;
	const TMatrixAffine2<T>	operator-( const TMatrixAffine2<T> &rhs ) const;

	//! post-multiplies column vector [rhs.x rhs.y 1]
	TVec2<T>				transformPoint( const TVec2<T> &rhs ) const;
	//! post-multiplies column vector [rhs.x rhs.y 1]
	const TVec2<T>		operator*( const TVec2<T> &rhs ) const;
	//! post-multiplies column vector [rhs.x rhs.y 0]
	TVec2<T>				transformVec( const TVec2<T> &v ) const;

	const TMatrixAffine2<T>	operator*( T rhs ) const;
	const TMatrixAffine2<T>	operator/( T rhs ) const;
	const TMatrixAffine2<T>	operator+( T rhs ) const;
	const TMatrixAffine2<T>	operator-( T rhs ) const;

	// Accessors
	T&					at( int row, int col );
	const T&			at( int row, int col ) const;

	T&					operator[]( int idx ) { return m[idx]; }
	const T&			operator[]( int idx ) const { return m[idx]; }


	void				set( const T *dt );
	// m[0]=d0, m[1]=d1, m[2]=d2 ... m[5]=d5
	void				set( T d0, T d1, T d2, T d3, T d4, T d5 );

	TVec2<T>				getColumn( int col ) const;
	void				setColumn( int col, const TVec2<T> &v );

	TVec3<T>				getRow( int row ) const;
	void				setRow( int row, const TVec3<T> &v );

	void				getColumns( TVec2<T> *c0, TVec2<T> *c1, TVec2<T> *c2 ) const;
	void				setColumns( const TVec2<T> &c0, const TVec2<T> &c1, const TVec2<T> &c2 );

	void				getRows( TVec3<T> *r0, TVec3<T> *r1, TVec3<T> *r2 ) const;
	void				setRows( const TVec3<T> &r0, const TVec3<T> &r1, const TVec3<T> &r2 );

	//! Sets the matrix to all zeros
	void				setToNull();
	//! Sets the matrix to the identity matrix.
	void				setToIdentity();

	//! Returns whether the matrix is singular (and consequently not invertible) or not
	bool				isSingular() const;

	//! Returns a copy of the matrix inverted. \a epsilon specifies the tolerance for testing for singularity.
	void				invert(T epsilon = EPSILON ) { *this = invertCopy( epsilon ); }
	//! Returns a copy of the matrix inverted. \a epsilon specifies the tolerance for testing for singularity.
	TMatrixAffine2<T>	invertCopy( T epsilon = EPSILON ) const;

	//! concatenate translation by \a v (conceptually, translate is before 'this')
	void 				translate( const TVec2<T> &v );
	//! Returns a copy of the matrix translated by \a v
	TMatrixAffine2 		translateCopy( const TVec2<T> &v ) const { TMatrixAffine2 result = *this; result.translate( v ); return result; }

	//! concatenate rotation by \a radians (conceptually, rotate is before 'this')
	void				rotate( T radians ) { *this *= TMatrixAffine2<T>::makeRotate( radians ); }
	//! concatenate rotation by \a radians around the point \a pt (conceptually, rotate is before 'this')
	void				rotate( T radians, const TVec2<T> &pt ) { *this *= TMatrixAffine2<T>::makeRotate( radians, pt ); }
	//! Returns a copy of the matrix rotate by \a radians 
	TMatrixAffine2 		rotateCopy( const TVec2<T> &v ) const { TMatrixAffine2 result = *this; result.rotate( v ); return result; }
	//! Returns a copy of the matrix rotate by \a radians around the point \a pt
	TMatrixAffine2 		rotateCopy( const TVec2<T> &v, const TVec2<T> &pt ) const { TMatrixAffine2 result = *this; result.rotate( v, pt ); return result; }

	//! concatenate scale (conceptually, scale is before 'this')
	void				scale( T s );
	//! concatenate scale (conceptually, scale is before 'this')
	void				scale( const TVec2<T> &v );
	//! Returns a copy of the matrix scaled by \a s
	TMatrixAffine2		scaleCopy( T s ) const { TMatrixAffine2 result = *this; result.scale( s ); return result; }
	//! Returns a copy of the matrix scaled by \a v
	TMatrixAffine2		scaleCopy( const TVec2<T> &v ) const { TMatrixAffine2 result = *this; result.scale( v ); return result; }

	// returns an identity matrix
	static TMatrixAffine2<T>	identity() { return TMatrixAffine2( 1, 0, 0, 1, 0, 0 ); }
	// returns 1 filled matrix
	static TMatrixAffine2<T>  one() { return TMatrixAffine2( (T)1 ); }
	// returns 0 filled matrix
	static TMatrixAffine2<T>  zero() { return TMatrixAffine2( (T)0 ); }

	// creates translation matrix
	static TMatrixAffine2<T>	makeTranslate( const TVec2<T> &v );

	// creates rotation matrix by \a radians
	static TMatrixAffine2<T>	makeRotate( T radians );
	// creates rotation matrix by \a radians around the point \a pt
	static TMatrixAffine2<T>	makeRotate( T radians, const TVec2<T> &pt );

	// creates scale matrix
	static TMatrixAffine2<T>	makeScale( T s );
	static TMatrixAffine2<T>	makeScale( const TVec2<T> &v );

	static TMatrixAffine2<T>	makeSkewX( T radians );
	static TMatrixAffine2<T>	makeSkewY( T radians );

	friend std::ostream& operator<<( std::ostream &lhs, const TMatrixAffine2<T> &rhs ) 
	{
		for( int i = 0; i < 2; i++) {
			lhs << " |";
			for( int j = 0; j < 3; j++) {
				lhs << std::setw( 12 ) << std::setprecision( 5 ) << rhs.m[j*2+i];
			}
			lhs << "|" << std::endl;
		}
		
		return lhs;
	}
};

template<typename T>
TMatrixAffine2<T>::TMatrixAffine2()
{
	setToIdentity();
}

template<typename T>
TMatrixAffine2<T>::TMatrixAffine2( T s )
{
	for( int i = 0; i < 6; ++i )
		m[i] = s;
}

template<typename T>
TMatrixAffine2<T>::TMatrixAffine2( const T *dt )
{
	set( dt );
}

template<typename T>
TMatrixAffine2<T>::TMatrixAffine2( T d0, T d1, T d2, T d3, T d4, T d5 )
{
	set( d0, d1, d2, d3, d4, d5 );
}

template<typename T>
TMatrixAffine2<T>::TMatrixAffine2( const TVec2<T> &vx, const TVec2<T> &vy, const TVec2<T> &vz )
{
	m00 = vx.x; m01 = vy.x; m02 = vz.x;
	m10 = vx.y; m11 = vy.y; m12 = vz.y;
}

template<typename T>
template<typename FromT >
TMatrixAffine2<T>::TMatrixAffine2( const TMatrixAffine2<FromT> &src )
{
	for( int i = 0; i < 6; ++i ) {
		m[i] = static_cast<T>( src.m[i] );
	}
}

template<typename T>
TMatrixAffine2<T>::TMatrixAffine2( const TMatrixAffine2<T>& src )
{
	memcpy( m, src.m, MEM_LEN );
}

template<typename T>
TMatrixAffine2<T>& TMatrixAffine2<T>::operator=( const TMatrixAffine2<T>& rhs )
{
	memcpy( m, rhs.m, MEM_LEN );
	return *this;
}

template<typename T>
TMatrixAffine2<T>& TMatrixAffine2<T>::operator=( T rhs )
{
	for( int i = 0; i < 6; ++i ) {
		m[i] = rhs;
	}
	return *this;
}

template<typename T>
template<typename FromT >
TMatrixAffine2<T>& TMatrixAffine2<T>::operator=( const TMatrixAffine2<FromT>& rhs )
{
	for( int i = 0; i < 6; i++ ) {
		m[i] = static_cast<T>(rhs.m[i]);
	}
	return *this;
}

template<typename T>
bool TMatrixAffine2<T>::equalCompare( const TMatrixAffine2<T>& rhs, T epsilon ) const
{
	for( int i = 0; i < 6; ++i ) {
		if( tmath<T>::abs(m[i] - rhs.m[i]) >= epsilon )
			return false;
	}
	return true;
}

template<typename T>
TMatrixAffine2<T>& TMatrixAffine2<T>::operator*=( const TMatrixAffine2<T> &rhs )
{
	TMatrixAffine2<T> mat;

	mat.m[0] = m[0]*rhs.m[0] + m[2]*rhs.m[1];
	mat.m[1] = m[1]*rhs.m[0] + m[3]*rhs.m[1];

	mat.m[2] = m[0]*rhs.m[2] + m[2]*rhs.m[3];
	mat.m[3] = m[1]*rhs.m[2] + m[3]*rhs.m[3];

	mat.m[4] = m[0]*rhs.m[4] + m[2]*rhs.m[5] + m[4];
	mat.m[5] = m[1]*rhs.m[4] + m[3]*rhs.m[5] + m[5];

	*this = mat;
	return *this;
}

template<typename T>
TMatrixAffine2<T>& TMatrixAffine2<T>::operator+=( const TMatrixAffine2<T> &rhs )
{
	for( int i = 0; i < 6; ++i )
		m[i] += rhs.m[i];
	return *this;
}

template<typename T>
TMatrixAffine2<T>& TMatrixAffine2<T>::operator-=( const TMatrixAffine2<T> &rhs )
{
	for( int i = 0; i < 6; ++i )
		m[i] -= rhs.m[i];
	return *this;
}

template<typename T>
TMatrixAffine2<T>& TMatrixAffine2<T>::operator*=( T s )
{
	for( int i = 0; i < 6; ++i )
		m[i] *= s;
	return *this;
}

template<typename T>
TMatrixAffine2<T>& TMatrixAffine2<T>::operator/=( T s )
{
	T invS = 1 / s;
	for( int i = 0; i < 6; ++i )
		m[i] *= invS;
	return *this;
}

template<typename T>
TMatrixAffine2<T>& TMatrixAffine2<T>::operator+=( T s )
{
	for( int i = 0; i < 6; ++i )
		m[i] += s;
	return *this;
}

template<typename T>
TMatrixAffine2<T>& TMatrixAffine2<T>::operator-=( T s )
{
	for( int i = 0; i < 6; ++i )
		m[i] -= s;
	return *this;
}

template<typename T>
const TMatrixAffine2<T> TMatrixAffine2<T>::operator*( const TMatrixAffine2<T> &rhs ) const
{
	TMatrixAffine2<T> ret;

	ret.m[0] = m[0]*rhs.m[0] + m[2]*rhs.m[1];
	ret.m[1] = m[1]*rhs.m[0] + m[3]*rhs.m[1];

	ret.m[2] = m[0]*rhs.m[2] + m[2]*rhs.m[3];
	ret.m[3] = m[1]*rhs.m[2] + m[3]*rhs.m[3];

	ret.m[4] = m[0]*rhs.m[4] + m[2]*rhs.m[5] + m[4];
	ret.m[5] = m[1]*rhs.m[4] + m[3]*rhs.m[5] + m[5];

	return ret;
}

template<typename T>
const TMatrixAffine2<T> TMatrixAffine2<T>::operator+( const TMatrixAffine2<T> &rhs ) const
{
	TMatrixAffine2<T> ret;
	for( int i = 0; i < 6; ++i )
		ret.m[i] = m[i] + rhs.m[i];
	return ret;
}

template<typename T>
const TMatrixAffine2<T> TMatrixAffine2<T>::operator-( const TMatrixAffine2<T> &rhs ) const
{
	TMatrixAffine2<T> ret;
	for( int i = 0; i < 6; ++i )
		ret.m[i] = m[i] - rhs.m[i];
	return ret;
}

template<typename T>
TVec2<T> TMatrixAffine2<T>::transformPoint( const TVec2<T> &rhs ) const
{
	return TVec2<T>( rhs.x * m[0] + rhs.y * m[2] + m[4], rhs.x * m[1] + rhs.y * m[3] + m[5] );
}

template<typename T>
const TVec2<T> TMatrixAffine2<T>::operator*( const TVec2<T> &rhs ) const
{
	return TVec2<T>( rhs.x * m[0] + rhs.y * m[2] + m[4], rhs.x * m[1] + rhs.y * m[3] + m[5] );
}

template<typename T>
TVec2<T> TMatrixAffine2<T>::transformVec( const TVec2<T> &v ) const
{
	return TVec2<T>( v.x * m[0] + v.y * m[2], v.x * m[1] + v.y * m[3] );
}

template<typename T>
const TMatrixAffine2<T> TMatrixAffine2<T>::operator*( T rhs ) const
{
	TMatrixAffine2<T> ret;
	for( int i = 0; i < 6; ++i )
		ret.m[i] = m[i]*rhs;
	return ret;
}

template<typename T>
const TMatrixAffine2<T> TMatrixAffine2<T>::operator/( T rhs ) const
{
	TMatrixAffine2<T> ret;
	T invS = 1 / rhs;
	for( int i = 0; i < 6; ++i )
		ret.m[i] = m[i] * invS;
	return ret;
}

template<typename T>
const TMatrixAffine2<T> TMatrixAffine2<T>::operator+( T rhs ) const
{
	TMatrixAffine2<T> ret;
	for( int i = 0; i < 6; ++i )
		ret.m[i] = m[i] + rhs;
	return ret;
}

template<typename T>
const TMatrixAffine2<T> TMatrixAffine2<T>::operator-( T rhs ) const
{
	TMatrixAffine2<T> ret;
	for( int i = 0; i < 6; ++i )
		ret.m[i] = m[i] - rhs;
	return ret;
}

template<typename T>
T& TMatrixAffine2<T>::at( int row, int col ) 
{
	assert(row >= 0 && row < 2);
	assert(col >= 0 && col < 3);
	return m[col*2 + row];
}

template<typename T>
const T& TMatrixAffine2<T>::at( int row, int col ) const 
{
	assert(row >= 0 && row < 2);
	assert(col >= 0 && col < 3);
	return m[col*2 + row];
}

template<typename T>
void TMatrixAffine2<T>::set( const T *d )
{
	m[0] = d[0]; m[3] = d[3];
	m[1] = d[1]; m[4] = d[4];
	m[2] = d[2]; m[5] = d[5];
}

template<typename T>
void TMatrixAffine2<T>::set( T d0, T d1, T d2, T d3, T d4, T d5 )
{
	m[0] = d0; m[3] = d3;
	m[1] = d1; m[4] = d4;
	m[2] = d2; m[5] = d5;
}

template<typename T>
TVec2<T> TMatrixAffine2<T>::getColumn( int col ) const
{
	size_t i = col*2;
	return TVec2<T>( 
		m[i + 0], 
		m[i + 1]
	);
}

template<typename T>
void TMatrixAffine2<T>::setColumn( int col, const TVec2<T> &v )
{
	size_t i = col*2;
	m[i + 0] = v.x;
	m[i + 1] = v.y;
}

template<typename T>
TVec3<T> TMatrixAffine2<T>::getRow( int row ) const 
{ 
	return TVec3<T>( 
		m[row +  0],
		m[row +  3],
		m[row +  6]
	); 
}

template<typename T>
void TMatrixAffine2<T>::setRow( int row, const TVec3<T> &v ) 
{ 
	m[row +  0] = v.x; 
	m[row +  3] = v.y; 
	m[row +  6] = v.z; 
}

template<typename T>
void TMatrixAffine2<T>::getColumns( TVec2<T> *c0, TVec2<T> *c1, TVec2<T> *c2 ) const
{
	*c0 = getColumn( 0 );
	*c1 = getColumn( 1 );
	*c2 = getColumn( 2 );
}

template<typename T>
void TMatrixAffine2<T>::setColumns( const TVec2<T> &c0, const TVec2<T> &c1, const TVec2<T> &c2 )
{
	setColumn( 0, c0 );
	setColumn( 1, c1 );
	setColumn( 2, c2 );
}

template<typename T>
void TMatrixAffine2<T>::getRows( TVec3<T> *r0, TVec3<T> *r1, TVec3<T> *r2 ) const
{
	*r0 = getRow( 0 );
	*r1 = getRow( 1 );
	*r2 = getRow( 2 );
}

template<typename T>
void TMatrixAffine2<T>::setRows( const TVec3<T> &r0, const TVec3<T> &r1, const TVec3<T> &r2 )
{
	setRow( 0, r0 );
	setRow( 1, r1 );
	setRow( 2, r2 );
}

template<typename T>
void TMatrixAffine2<T>::setToNull()
{
	std::memset( m, 0, MEM_LEN );
}

template<typename T>
void TMatrixAffine2<T>::setToIdentity()
{
	m[0] = 1; m[2] = 0; m[4] = 0;
	m[1] = 0; m[3] = 1; m[5] = 0;
}

template<typename T>
bool TMatrixAffine2<T>::isSingular() const
{
	return fabs( m[0] * m[3] - m[2] * m[1] ) <= (T)EPSILON;
}

template<typename T>
TMatrixAffine2<T> TMatrixAffine2<T>::invertCopy( T epsilon ) const
{
	TMatrixAffine2<T> inv;

	inv.m[0] = m[3];
	inv.m[1] = -m[1];
	inv.m[2] = -m[2];
	inv.m[3] = m[0];
	inv.m[4] = m[2]*m[5] - m[3]*m[4];
	inv.m[5] = m[1]*m[4] - m[0]*m[5];

	T det = m[0]*inv.m[0] + m[1]*inv.m[2];

	if( fabs( det ) > epsilon ) {
		T invDet = 1 / det;
		inv.m[0] *= invDet;
		inv.m[1] *= invDet;
		inv.m[2] *= invDet;
		inv.m[3] *= invDet;
		inv.m[4] *= invDet;
		inv.m[5] *= invDet;
	}

	return inv;
}

template<typename T>
void TMatrixAffine2<T>::translate( const TVec2<T> &v )
{
	m[4] += m[0]*v.x + m[2]*v.y;
	m[5] += m[1]*v.x + m[3]*v.y;
}

template<typename T>
void TMatrixAffine2<T>::scale( T s )
{
	m[0] *= s;
	m[1] *= s;
		
	m[2] *= s;
	m[3] *= s;
}
	
template<typename T>
void TMatrixAffine2<T>::scale( const TVec2<T> &s )
{
	m[0] *= s.x;
	m[1] *= s.x;

	m[2] *= s.y;
	m[3] *= s.y;
}

template<typename T>
TMatrixAffine2<T>	TMatrixAffine2<T>::makeTranslate( const TVec2<T> &v )
{
	TMatrixAffine2<T> ret;

	ret.m[0] = 1;
	ret.m[1] = 0;

	ret.m[2] = 0;
	ret.m[3] = 1;
	
	ret.m[4] = v.x;
	ret.m[5] = v.y;

	return ret;	
}

template<typename T>
TMatrixAffine2<T> TMatrixAffine2<T>::makeRotate( T radians )
{
	T sine   = tmath<T>::sin( radians );
	T cosine = tmath<T>::cos( radians );

	TMatrixAffine2<T> ret;

	ret.m[0] = cosine;
	ret.m[1] = sine;

	ret.m[2] = -sine;
	ret.m[3] = cosine;
	
	ret.m[4] = 0;
	ret.m[5] = 0;

	return ret;
}

template<typename T>
TMatrixAffine2<T> TMatrixAffine2<T>::makeRotate( T radians, const TVec2<T> &pt )
{
	T sine   = tmath<T>::sin( radians );
	T cosine = tmath<T>::cos( radians );

	TMatrixAffine2<T> ret;

	ret.m[0] = cosine;
	ret.m[1] = sine;

	ret.m[2] = -sine;
	ret.m[3] = cosine;
	
	ret.m[4] = pt.x - cosine * pt.x + sine * pt.y;
	ret.m[5] = pt.y - sine * pt.x - cosine * pt.y;

	return ret;
}

template<typename T>
TMatrixAffine2<T> TMatrixAffine2<T>::makeScale( T s )
{
	TMatrixAffine2<T> ret;

	ret.m[0] = s;
	ret.m[1] = 0;

	ret.m[2] = 0;
	ret.m[3] = s;
	
	ret.m[4] = 0;
	ret.m[5] = 0;

	return ret;
}

template<typename T>
TMatrixAffine2<T> TMatrixAffine2<T>::makeScale( const TVec2<T> &s )
{
	TMatrixAffine2<T> ret;

	ret.m[0] = s.x;
	ret.m[1] = 0;

	ret.m[2] = 0;
	ret.m[3] = s.y;
	
	ret.m[4] = 0;
	ret.m[5] = 0;

	return ret;
}

template<typename T>
TMatrixAffine2<T>	TMatrixAffine2<T>::makeSkewX( T radians )
{
	TMatrixAffine2<T> ret;

	ret.m[0] = 1;
	ret.m[1] = 0;

	ret.m[2] = tmath<T>::tan( radians );
	ret.m[3] = 1;
	
	ret.m[4] = 0;
	ret.m[5] = 0;

	return ret;	
}

template<typename T>
TMatrixAffine2<T>	TMatrixAffine2<T>::makeSkewY( T radians )
{
	TMatrixAffine2<T> ret;

	ret.m[0] = 1;
	ret.m[1] = tmath<T>::tan( radians );

	ret.m[2] = 0;
	ret.m[3] = 1;
	
	ret.m[4] = 0;
	ret.m[5] = 0;

	return ret;	
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//		Typedefs
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef TMatrixAffine2<float> 	MatrixAffine2f;
typedef TMatrixAffine2<double> 	MatrixAffine2d;

