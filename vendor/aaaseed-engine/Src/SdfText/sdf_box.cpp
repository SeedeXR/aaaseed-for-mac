
#include "sdf_box.h"


#include <limits>
#include <ostream>

using std::numeric_limits;

namespace aaa
{


template<typename T>
BoxT<T>::BoxT( const std::vector<vec2T> &points )
{
	m_PosLB.x = std::numeric_limits<T>::max();
	m_PosRT.x = -std::numeric_limits<T>::max();
	m_PosLB.y = std::numeric_limits<T>::max();
	m_PosRT.y = -std::numeric_limits<T>::max();
	include( points );
	//for( std::size_t s = 0; s < points.size(); ++s )
	//{
	//	include( points[ s ] );
	//}
}

template<typename T>
void BoxT<T>::canonicalize()
{
	if ( m_PosLB.x > m_PosRT.x )
	{
		T temp = m_PosLB.x;
		m_PosLB.x = m_PosRT.x;
		m_PosRT.x = temp;
	}

	if ( m_PosLB.y > m_PosRT.y )
	{
		T temp = m_PosLB.y;
		m_PosLB.y = m_PosRT.y;
		m_PosRT.y = temp;
	}
}

template<typename T>
BoxT<T> BoxT<T>::canonicalized() const
{
	BoxT<T> result( *this );
	result.canonicalize();
	return result;
}

template<typename T>
void BoxT<T>::clip( const BoxT &clip )
{
	if ( m_PosLB.x < clip.m_PosLB.x )
		m_PosLB.x = clip.m_PosLB.x;
	if ( m_PosRT.x < clip.m_PosLB.x )
		m_PosRT.x = clip.m_PosLB.x;
	if ( m_PosLB.x > clip.m_PosRT.x )
		m_PosLB.x = clip.m_PosRT.x;
	if ( m_PosRT.x > clip.m_PosRT.x )
		m_PosRT.x = clip.m_PosRT.x;

	if ( m_PosLB.y < clip.m_PosLB.y )
		m_PosLB.y = clip.m_PosLB.y;
	if ( m_PosRT.y < clip.m_PosLB.y )
		m_PosRT.y = clip.m_PosLB.y;
	if ( m_PosLB.y > clip.m_PosRT.y )
		m_PosLB.y = clip.m_PosRT.y;
	if ( m_PosRT.y > clip.m_PosRT.y )
		m_PosRT.y = clip.m_PosRT.y;
}

template<typename T>
BoxT<T> BoxT<T>::clipped( const BoxT &clip ) const
{
	BoxT<T> result( *this );
	result.clip( BoxT<T>( clip ) );
	return result;
}

template<typename T>
void BoxT<T>::translate( const vec2T &offset ) noexcept
{
	m_PosLB.x += offset.x;
	m_PosRT.x += offset.x;
	m_PosLB.y += offset.y;
	m_PosRT.y += offset.y;
}


template<typename T>
void BoxT<T>::translate( const T x, const T y ) noexcept
{
	m_PosLB.x += x;
	m_PosRT.x += x;
	m_PosLB.y += y;
	m_PosRT.y += y;
}

template<typename T>
BoxT<T> BoxT<T>::translated( const vec2T &off ) const noexcept
{
	BoxT result( *this );
	result.translate( off );
	return result;
}

//template<typename T>
//void BoxT<T>::moveTL( const vec2T &newUL )
//{
//	set( newUL.x, newUL.y, newUL.x + width(), newUL.y + height() );
//}
//
//template<typename T>
//BoxT<T> BoxT<T>::movedTL( const vec2T &newUL ) const
//{
//	BoxT result( *this );
//	result.moveTL( newUL );
//	return result;
//}

template<typename T>
void BoxT<T>::moveBL( const vec2T &newBL )
{
	set( newBL.x, newBL.y, newBL.x + width(), newBL.y + height() );
}
template<typename T>
BoxT<T> BoxT<T>::movedBL( const vec2T &newUL ) const
{
	BoxT result( *this );
	result.moveBL( newUL );
	return result;
}


template<typename T>
void BoxT<T>::inflate( const vec2T &amount )
{
	m_PosLB.x -= amount.x;
	m_PosRT.x += amount.x;
	m_PosLB.y -= amount.y; // assume canonical rect has m_PosLB.y < m_PosRT.y
	m_PosRT.y += amount.y;
}

template<typename T>
BoxT<T> BoxT<T>::inflated( const vec2T &amount ) const
{
	BoxT<T> result( *this );
	result.inflate( amount );
	return result;
}

template<typename T>
void BoxT<T>::scale_centered( const vec2T &scale )
{
	const T halfWidth = T( width() * scale.x * 0.5 );
	const T halfHeight = T( height() * scale.y * 0.5 );
	const auto centerTmp = center();
	m_PosLB.x = centerTmp.x - halfWidth;
	m_PosRT.x = centerTmp.x + halfWidth;
	m_PosLB.y = centerTmp.y - halfHeight;
	m_PosRT.y = centerTmp.y + halfHeight;
}

template<typename T>
void BoxT<T>::scale_centered( const T scale )
{
	const T halfWidth = T( width() * scale * 0.5 );
	const T halfHeight = T( height() * scale * 0.5 );
	const auto centerTmp = center();
	m_PosLB.x = centerTmp.x - halfWidth;
	m_PosRT.x = centerTmp.x + halfWidth;
	m_PosLB.y = centerTmp.y - halfHeight;
	m_PosRT.y = centerTmp.y + halfHeight;
}

template<typename T>
BoxT<T> BoxT<T>::scaled_centered( const vec2T &scale ) const
{
	const T halfWidth = T( width() * scale.x * 0.5 );
	const T halfHeight = T( height() * scale.y * 0.5 );
	const auto centerTmp = center();
	return BoxT<T>( centerTmp.x - halfWidth, centerTmp.y - halfHeight, centerTmp.x + halfWidth, centerTmp.y + halfHeight );
}

template<typename T>
BoxT<T> BoxT<T>::scaled_centered( const T scale ) const
{
	const T halfWidth = T( width() * scale * 0.5 );
	const T halfHeight = T( height() * scale * 0.5 );
	const auto centerTmp = center();
	return BoxT<T>( centerTmp.x - halfWidth, centerTmp.y - halfHeight, centerTmp.x + halfWidth, centerTmp.y + halfHeight );
}

template<typename T>
void BoxT<T>::scale( const T s )
{
	m_PosLB.x *= s;
	m_PosRT.x *= s;
	m_PosLB.y *= s;
	m_PosRT.y *= s;
}

template<typename T>
void BoxT<T>::scale( const vec2T &scale )
{
	m_PosLB.x *= scale.x;
	m_PosLB.y *= scale.y;
	m_PosRT.x *= scale.x;
	m_PosRT.y *= scale.y;
}

template<typename T>
BoxT<T> BoxT<T>::scaled( const T s ) const
{
	return BoxT<T>( m_PosLB.x * s, m_PosLB.y * s, m_PosRT.x * s, m_PosRT.y * s );
}

template<typename T>
BoxT<T> BoxT<T>::scaled( const vec2T &scale ) const
{
	return BoxT<T>( m_PosLB.x * scale.x, m_PosLB.y * scale.y, m_PosRT.x * scale.x, m_PosRT.y * scale.y );
}

template<typename T>
BoxT<T> BoxT<T>::transformed( const mat3T& matrix ) const
{
	auto centerTmp = center();
	auto extents = glm::abs( m_PosRT - centerTmp );

	const auto x = matrix * vec3T( extents.x, 0, 0 );
	const auto y = matrix * vec3T( 0, extents.y, 0 );

	extents = vec2T( glm::abs( x ) + glm::abs( y ) );
	centerTmp = vec2T( matrix * vec3T( centerTmp, 1 ) );

	return BoxT<T>( centerTmp.x - extents.x, centerTmp.y - extents.y, centerTmp.x + extents.x, centerTmp.y + extents.y );
}

template<typename T>
void BoxT<T>::transform( const mat3T &matrix )
{
	auto centerTmp = center();
	auto extents = glm::abs( m_PosRT - centerTmp );

	const auto x = matrix * vec3T( extents.x, 0, 0 );
	const auto y = matrix * vec3T( 0, extents.y, 0 );

	extents = vec2T( glm::abs( x ) + glm::abs( y ) );
	centerTmp = vec2T( matrix * vec3T( centerTmp, 1 ) );

	m_PosLB.x = centerTmp.x - extents.x;
	m_PosLB.y = centerTmp.y - extents.y;
	m_PosRT.x = centerTmp.x + extents.x;
	m_PosRT.y = centerTmp.y + extents.y;
}

//template<typename T>
//bool boxT<T>::intersects( const boxT<T> &rect ) const
//{
//	if ( ( m_PosLB.x > rect.m_PosRT.x ) || ( m_PosRT.x < rect.m_PosLB.x ) || ( m_PosLB.y > rect.m_PosRT.y ) || ( m_PosRT.y < rect.m_PosLB.y ) )
//		return false;
//	else
//		return true;
//}

template<typename T>
T BoxT<T>::distance( const vec2T &pt ) const
{
	T squaredDistance = 0;
	if ( pt.x < m_PosLB.x )
	{
		squaredDistance += ( m_PosLB.x - pt.x ) * ( m_PosLB.x - pt.x );
	}
	else if ( pt.x > m_PosRT.x )
	{
		squaredDistance += ( pt.x - m_PosRT.x ) * ( pt.x - m_PosRT.x );
	}
	if ( pt.y < m_PosLB.y )
	{
		squaredDistance += ( m_PosLB.y - pt.y ) * ( m_PosLB.y - pt.y );
	}
	else if ( pt.y > m_PosRT.y )
	{
		squaredDistance += ( pt.y - m_PosRT.y ) * ( pt.y - m_PosRT.y );
	}

	if ( squaredDistance > 0 )
	{
		return T( std::sqrt( squaredDistance ) );
	}
	else
	{
		return 0;
	}
}

template<typename T>
T BoxT<T>::distance_squared( const vec2T &pt ) const
{
	T squaredDistance = 0;
	if ( pt.x < m_PosLB.x )
	{
		squaredDistance += ( m_PosLB.x - pt.x ) * ( m_PosLB.x - pt.x );
	}
	else if ( pt.x > m_PosRT.x )
	{
		squaredDistance += ( pt.x - m_PosRT.x ) * ( pt.x - m_PosRT.x );
	}
	if ( pt.y < m_PosLB.y )
	{
		squaredDistance += ( m_PosLB.y - pt.y ) * ( m_PosLB.y - pt.y );
	}
	else if ( pt.y > m_PosRT.y )
	{
		squaredDistance += ( pt.y - m_PosRT.y ) * ( pt.y - m_PosRT.y );
	}

	return squaredDistance;
}

template<typename T>
typename BoxT<T>::vec2T BoxT<T>::closest_point( const vec2T &pt ) const
{
	auto result = pt;
	if ( pt.x < m_PosLB.x )
	{
		result.x = m_PosLB.x;
	}
	else if ( pt.x > m_PosRT.x )
	{
		result.x = m_PosRT.x;
	}
	if ( pt.y < m_PosLB.y )
	{
		result.y = m_PosLB.y;
	}
	else if ( pt.y > m_PosRT.y )
	{
		result.y = m_PosRT.y;
	}
	return result;
}

template<typename T>
void BoxT<T>::include( const vec2T &point )
{
	if ( m_PosLB.x > point.x ) m_PosLB.x = point.x;
	if ( m_PosRT.x < point.x ) m_PosRT.x = point.x;
	if ( m_PosLB.y > point.y ) m_PosLB.y = point.y;
	if ( m_PosRT.y < point.y ) m_PosRT.y = point.y;
}

template<typename T>
void BoxT<T>::include( const vec3T &point )
{
	if ( m_PosLB.x > point.x ) m_PosLB.x = point.x;
	if ( m_PosRT.x < point.x ) m_PosRT.x = point.x;
	if ( m_PosLB.y > point.y ) m_PosLB.y = point.y;
	if ( m_PosRT.y < point.y ) m_PosRT.y = point.y;
}


template<typename T>
void BoxT<T>::include( const std::vector<vec2T> &points )
{
	for( std::size_t s = 0; s < points.size(); ++s )
		include( points[ s ] );
}

template<typename T>
void BoxT<T>::include( const std::vector<vec3T> &points )
{
	for( std::size_t s = 0; s < points.size(); ++s )
		include( points[ s ] );
}

template<typename T>
void BoxT<T>::include( const BoxT<T> &rect )
{
	include( rect.m_PosLB );
	include( rect.m_PosRT );
}

//template<typename T>
//Area boxT<T>::getInteriorArea() const
//{
//	boxT<T> canon = canonicalized();
//	
//	return Area( static_cast<int32_t>( ceil( canon.x1 ) ), static_cast<int32_t>( ceil( canon.m_PosLB.y ) ), 
//		static_cast<int32_t>( floor( canon.m_PosRT.x ) ) + 1, static_cast<int32_t>( floor( canon.m_PosRT.y ) ) + 1 );
//}

template<typename T>
BoxT<T> BoxT<T>::centered_fit( const BoxT<T> &other, const bool expand ) const
{
	BoxT<T> result = *this;
	result.translate( other.center() - result.center() );

	const bool isInside = ( ( result.width() < other.width() ) && ( result.height() < other.height() ) );
	if ( expand || ( !isInside ) )
	{ // need to do some scaling
		const T aspectAspect = result.aspect_ratio() / other.aspect_ratio();
		if ( aspectAspect >= 1.0f )
		{ // result is proportionally wider so we need to fit its x-axis
			const T scaleBy = other.width() / result.width();
			result.scale_centered( scaleBy );
		}
		else
		{ // result is proportionally wider so we need to fit its y-axis
			const T scaleBy = other.height() / result.height();
			result.scale_centered( scaleBy );
		}
	}

	return result;
}

template<typename T>
BoxT<T> BoxT<T>::centered_fill( const BoxT<T> &other, const bool contract ) const
{
	BoxT<T> result = *this;
	result.translate( other.center() - result.center() );

	const bool otherIsInside = ( ( result.width() > other.width() ) && ( result.height() > other.height() ) );
	if ( contract || ( !otherIsInside ) )
	{ // need to do some scaling
		const T aspectAspect = result.aspect_ratio() / other.aspect_ratio();
		if ( aspectAspect <= 1.0f )
		{ // result is proportionally wider so we need to fit its x-axis
			const T scaleBy = other.width() / result.width();
			result.scale_centered( scaleBy );
		}
		else
		{ // result is proportionally wider so we need to fit its y-axis
			const T scaleBy = other.height() / result.height();
			result.scale_centered( scaleBy );
		}
	}

	return result;
}

BoxMapping::BoxMapping( const boxf &aSrcRect, const boxf &aDstRect, const bool preserveSrcAspect )
	: mSrcRect( aSrcRect )
	, mDstRect( aDstRect )
{
	if ( preserveSrcAspect )
	{
		const float srcAspect = aSrcRect.width() / (float)aSrcRect.height();
		const float dstAspect = aDstRect.width() / (float)aDstRect.height();
		if ( srcAspect < dstAspect )
		{ // src is narrower aspect
			const float heightRatio = mDstRect.height() / mSrcRect.height();
			const float effectiveWidth = mSrcRect.width() * heightRatio;
			const float offsetX = ( mDstRect.width() - effectiveWidth ) / 2.0f;
			const float dstWidth = mDstRect.height() * srcAspect;
			mDstRect.set( mDstRect.m_PosLB.x + offsetX, mDstRect.bottom(), mDstRect.m_PosLB.x + offsetX + dstWidth, mDstRect.top() );
		}
		else
		{ // src is wider aspect
			const float effectiveHeight = mSrcRect.height() * ( mDstRect.width() / mSrcRect.width() );
			const float offsetY = ( mDstRect.height() - effectiveHeight ) / 2.0f;
			const float dstHeight = mDstRect.width() / srcAspect;
			mDstRect.set( mDstRect.m_PosLB.x, mDstRect.bottom() + offsetY, mDstRect.m_PosRT.x, mDstRect.bottom() + offsetY + dstHeight );
		}
	}
}

glm::vec2 BoxMapping::map( const glm::vec2 &srcPoint ) const
{
	const float x = ( srcPoint.x - mSrcRect.left() ) / mSrcRect.width() * mDstRect.width() + mDstRect.left();
	const float y = ( srcPoint.y - mSrcRect.bottom() ) / mSrcRect.height() * mDstRect.height() + mDstRect.bottom();

	return glm::vec2( x, y );
}

boxf BoxMapping::map( const boxf &srcRect ) const
{
	const glm::vec2 ul = map( srcRect.m_PosLB );
	const glm::vec2 lr = map( srcRect.m_PosRT );

	return boxf( ul.x, ul.y, lr.x, lr.y );
}



template<typename T>
std::ostream& operator<< ( std::ostream& o, const BoxT<T>& rect )
{
	return o << "(" << rect.m_PosLB.x << ", " << rect.m_PosLB.y << ")-(" << rect.m_PosRT.x << ", " << rect.m_PosRT.y << ")";
}

template class BoxT<float>;
template class BoxT<double>;
//template class BoxT<std::size_t>;
} // namespace lc
