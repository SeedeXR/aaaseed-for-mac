
#ifdef AAA_TBOX_H
#error "TBOX_H included more than once."
#endif
#define AAA_TBOX_H 1


#ifndef AAA_TVECTOR_H
#	include "math/TVector.h"
#endif
#ifndef AAA_TMATRIX_H
#	include "math/TMatrix.h"
#endif
#ifndef AAA_TRAY_H
#	include "math/TRay.h"
#endif


template< typename T >
class TBox 
{

protected:
	TVec3<T> mExtents[2];
	TVec3<T> mVerts[8];

public:
	TBox( void );
	TBox( const TVec3<T> &aMin, const TVec3<T> &aMax );

	bool intersects( const TRay<T> & p_ray );
	int32_t	intersect( const TRay<T> & p_ray, T p_intersections[2] );

	//! Expands the box so that it contains \a p_box
	void include( const TBox<T> & p_box );

	//! for use in frustum culling
	TVec3<T> getNegative( const TVec3<T> & p_normal ) const;
	TVec3<T> getPositive( const TVec3<T> & p_normal ) const;

	//! converts axis-aligned box to another coordinate space
	TBox<T> transformed( const Matrix44<T> &transform ) const;

	static bool calcTriangleIntersection( const TRay<T> &ray, const TVec3<T> &vert0, const TVec3<T> &vert1, const TVec3<T> &vert2, T * result );

	TVec3<T> getCenter( void ) const;
	TVec3<T> getSize( void ) const;

	const TVec3<T> & getMin( void ) const;
	const TVec3<T> & getMax( void ) const;
};



//=================================================================================================
template< typename T >
TBox<T>::TBox( void )
{}

//=================================================================================================
template< typename T >
TBox<T>::TBox( const TVec3<T> &aMin, const TVec3<T> &aMax )
{
	mExtents[0] = TVec3<T>( aMin.x, aMin.y, aMin.z );
	mExtents[1] = TVec3<T>( aMax.x, aMax.y, aMax.z );

	TVec3<T> extent( aMax.x - aMin.x, aMax.y - aMin.y, aMax.z - aMin.z );
	TVec3<T> mid( ( aMin.x + aMax.x ) / 2.0f, ( aMin.y + aMax.y ) / 2.0f, ( aMin.z + aMax.z ) / 2.0f );

	mVerts[0] = TVec3<T>( -0.5f, -0.5f,  0.5f ) * extent + mid;
	mVerts[1] = TVec3<T>(  0.5f, -0.5f,  0.5f ) * extent + mid;
	mVerts[2] = TVec3<T>( -0.5f,  0.5f,  0.5f ) * extent + mid;
	mVerts[3] = TVec3<T>(  0.5f,  0.5f,  0.5f ) * extent + mid;
	mVerts[4] = TVec3<T>( -0.5f,  0.5f, -0.5f ) * extent + mid;
	mVerts[5] = TVec3<T>(  0.5f,  0.5f, -0.5f ) * extent + mid;
	mVerts[6] = TVec3<T>( -0.5f, -0.5f, -0.5f ) * extent + mid;
	mVerts[7] = TVec3<T>(  0.5f, -0.5f, -0.5f ) * extent + mid;
}



//=================================================================================================
template< typename T >
bool TBox<T>::intersects( const TRay<T> &ray )
{
	float tmin, tmax, tymin, tymax, tzmin, tzmax;

	tmin  = (mExtents[  ray.getSignX()].x - ray.getOrigin().x) * ray.getInverseDirection().x;
	tmax  = (mExtents[1-ray.getSignX()].x - ray.getOrigin().x) * ray.getInverseDirection().x;
	tymin = (mExtents[  ray.getSignY()].y - ray.getOrigin().y) * ray.getInverseDirection().y;
	tymax = (mExtents[1-ray.getSignY()].y - ray.getOrigin().y) * ray.getInverseDirection().y;

	if( ( tmin > tymax ) || ( tymin > tmax) ) 
		return false;
	if( tymin > tmin )
		tmin = tymin;
	if( tymax < tmax )
		tmax = tymax;

	tzmin = (mExtents[  ray.getSignZ()].z - ray.getOrigin().z) * ray.getInverseDirection().z;
	tzmax = (mExtents[1-ray.getSignZ()].z - ray.getOrigin().z) * ray.getInverseDirection().z;

	if( ( tmin > tzmax ) || ( tzmin > tmax ) ) 
		return false;
	if( tzmin > tmin )
		tmin = tzmin;
	if( tzmax < tmax )
		tmax = tzmax;

	return tmin > 0;
}

//=================================================================================================
template< typename T >
int32_t TBox<T>::intersect( const TRay<T> &ray, T intersections[2] )
{
	int32_t i = 0;

	if( ray.calcTriangleIntersection( mVerts[2], mVerts[0], mVerts[1], &intersections[i] ) ) { // +Z
		i++;
	}
	else if( ray.calcTriangleIntersection( mVerts[1], mVerts[3], mVerts[2], &intersections[i] ) ) { // +Z
		i++;
	}

	if( ray.calcTriangleIntersection( mVerts[7], mVerts[5], mVerts[3], &intersections[i] ) ) { // +X
		i++; if( i > 2 ) return i;
	}
	else if( ray.calcTriangleIntersection( mVerts[1], mVerts[7], mVerts[3], &intersections[i] ) ) { // +X
		i++; if( i > 2 ) return i;
	}

	if( ray.calcTriangleIntersection( mVerts[3], mVerts[5], mVerts[4], &intersections[i] ) ) { // +Y
		i++; if( i > 2 ) return i;
	}
	else if( ray.calcTriangleIntersection( mVerts[2], mVerts[3], mVerts[4], &intersections[i] ) ) { // +Y
		i++; if( i > 2 ) return i;
	}

	if( ray.calcTriangleIntersection( mVerts[1], mVerts[0], mVerts[7], &intersections[i] ) ) { // -Y
		i++; if( i > 2 ) return i;
	}
	else if( ray.calcTriangleIntersection( mVerts[0], mVerts[6], mVerts[7], &intersections[i] ) ) { // -Y
		i++; if( i > 2 ) return i;
	}

	if( ray.calcTriangleIntersection( mVerts[4], mVerts[0], mVerts[2], &intersections[i] ) ) { // -X
		i++; if( i > 2 ) return i;
	}
	else if( ray.calcTriangleIntersection( mVerts[4], mVerts[6], mVerts[0], &intersections[i] ) ) { // -X
		i++; if( i > 2 ) return i;
	}

	if( ray.calcTriangleIntersection( mVerts[6], mVerts[4], mVerts[5], &intersections[i] ) ) { // -Z
		i++;
	}
	else if( ray.calcTriangleIntersection( mVerts[7], mVerts[6], mVerts[5], &intersections[i] ) ) { // -Z
		i++;
	}

	return i;
}



//=================================================================================================
template< typename T >
void TBox<T>::include( const TBox<T> & rhs )
{
	TVec3<T> minB = rhs.getMin();
	if(minB.x < mExtents[0].x) mExtents[0].x = minB.x;
	if(minB.y < mExtents[0].y) mExtents[0].y = minB.y;
	if(minB.z < mExtents[0].z) mExtents[0].z = minB.z;

	TVec3<T> maxB = rhs.getMax();
	if(maxB.x > mExtents[1].x) mExtents[1].x = maxB.x;
	if(maxB.y > mExtents[1].y) mExtents[1].y = maxB.y;
	if(maxB.z > mExtents[1].z) mExtents[1].z = maxB.z;

	// update vertices
	TVec3<T> extent( mExtents[1].x - mExtents[0].x, mExtents[1].y - mExtents[0].y, mExtents[1].z - mExtents[0].z );
	TVec3<T> mid( ( mExtents[0].x + mExtents[1].x ) / 2.0f, ( mExtents[0].y + mExtents[1].y ) / 2.0f, ( mExtents[0].z + mExtents[1].z ) / 2.0f );
	mVerts[0] = TVec3<T>( -0.5f, -0.5f,  0.5f ) * extent +  mid;
	mVerts[1] = TVec3<T>(  0.5f, -0.5f,  0.5f ) * extent + mid;
	mVerts[2] = TVec3<T>( -0.5f,  0.5f,  0.5f ) * extent + mid;
	mVerts[3] = TVec3<T>(  0.5f,  0.5f,  0.5f ) * extent + mid;
	mVerts[4] = TVec3<T>( -0.5f,  0.5f, -0.5f ) * extent + mid;
	mVerts[5] = TVec3<T>(  0.5f,  0.5f, -0.5f ) * extent + mid;
	mVerts[6] = TVec3<T>( -0.5f, -0.5f, -0.5f ) * extent + mid;
	mVerts[7] = TVec3<T>(  0.5f, -0.5f, -0.5f ) * extent + mid;
}



//=================================================================================================
template< typename T >
TVec3<T> TBox<T>::getPositive(const TVec3<T> &normal) const
{
	TVec3<T> result = getMin();
	TVec3<T> size = getSize();

	if (normal.x > 0)
		result.x += size.x;

	if (normal.y > 0)
		result.y += size.y;

	if (normal.z > 0)
		result.z += size.z;

	return(result);
}



//=================================================================================================
template< typename T >
TVec3<T> TBox<T>::getNegative(const TVec3<T> &normal) const
{
	TVec3<T> result = getMin();
	TVec3<T> size = getSize();

	if (normal.x < 0)
		result.x += size.x;

	if (normal.y < 0)
		result.y += size.y;

	if (normal.z < 0)
		result.z += size.z;

	return(result);
}



//=================================================================================================
template< typename T >
TBox<T> TBox<T>::transformed( const TMatrix44<T> &transform ) const
{
	TVec3<T> verts[8];

	for(size_t i=0;i<8;i++) 
		verts[i] = transform.transformPointAffine(mVerts[i]);

	TVec3<T> min = verts[0];
	TVec3<T> max = verts[0];

	for(size_t i=1;i<8;i++) {
		if(verts[i].x < min.x) min.x = verts[i].x;
		if(verts[i].y < min.y) min.y = verts[i].y;
		if(verts[i].z < min.z) min.z = verts[i].z;

		if(verts[i].x > max.x) max.x = verts[i].x;
		if(verts[i].y > max.y) max.y = verts[i].y;
		if(verts[i].z > max.z) max.z = verts[i].z;
	}

	return TBox<T>(min, max);
}	



//=================================================================================================
template< typename T >
TVec3<T> TBox<T>::getCenter( void ) const 
{ 
	return ( mExtents[1] + mExtents[0] ) * 0.5; 
}

//=================================================================================================
template< typename T >
TVec3<T> TBox<T>::getSize( void ) const 
{ 
	return mExtents[1] - mExtents[0]; 
}



//=================================================================================================
template< typename T >
const TVec3<T> & TBox<T>::getMin( void ) const 
{ 
	return mExtents[0]; 
}

//=================================================================================================
template< typename T >
const TVec3<T> & TBox<T>::getMax( void ) const 
{ 
	return mExtents[1]; 
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//		Typedefs
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef TBox<float>		Boxf;
typedef TBox<double>	Boxd;

