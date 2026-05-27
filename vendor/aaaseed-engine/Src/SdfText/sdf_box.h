
#ifdef AAA_SDF_BOX_H
#error "SDF_BOX_H included more than once."
#endif
#define AAA_SDF_BOX_H 1


#ifndef AAA_AAA_UTIL_H
#	include "aaa_util.h"
#endif
#if !defined(_VECTOR_)
#	include <vector>
#endif

#include <glm/glm.hpp>



namespace aaa
{

template< typename T >
class BoxT
{
public:
	typedef glm::tvec2<T, glm::defaultp>	vec2T;
	typedef glm::tvec3<T, glm::defaultp>	vec3T;
	typedef glm::tmat3x3<T, glm::defaultp>	mat3T;

	BoxT()
	{
	}
	BoxT( const T xMin, const T yMin, const T xMax, const T yMax )
		: m_PosLB( { xMin, yMin } )
		, m_PosRT( { xMax, yMax } )
	{
		//set( xMin, yMin, xMax, yMax );
	}
	BoxT( const glm::tvec2< T >& pos, const glm::tvec2< T >& size )
		: m_PosLB( pos )
		, m_PosRT( { pos.x + size.x, pos.y + size.y } )
	{
	}
	/// Initializes the rectangle to be the bounding box of \a points
	BoxT( const std::vector<vec2T> &points );
	/// copy constructor
	BoxT( const BoxT &other )
		: m_PosLB( other.m_PosLB )
		, m_PosRT( other.m_PosRT )
	{
	}

	/// Clear
	void	clear()
	{
		m_PosRT = {};
		m_PosLB = {};
	}

	/// return width
	T	width() const noexcept
	{
		return m_PosRT.x - m_PosLB.x;
	}
	/// return height
	T	height() const noexcept
	{
		return m_PosRT.y - m_PosLB.y;
	}
	T	aspect_ratio() const
	{
		if ( height() == static_cast<T>( 0 ) )
		{
			return T(1);
		}
		return width() / height();
	}

	/// return left position
	T	x() const noexcept
	{
		return m_PosLB.x;
	}
	/// return bottom position
	T	y() const noexcept
	{
		return m_PosLB.y;
	}
	/// return top
	T	top() const noexcept
	{
		return m_PosRT.y;
	}
	/// return right
	T	right() const noexcept
	{
		return m_PosRT.x;
	}
	/// return left
	T	left() const noexcept
	{
		return x();
	}
	/// return bottom
	T	bottom() const noexcept
	{
		return y();
	}
	/// return area
	T	area() const noexcept
	{
		return width() * height();
	}
	/// set from other box
	void	set( const BoxT<T>& a ) noexcept
	{
		m_PosLB = a.m_PosLB;
		m_PosRT = a.m_PosRT;
	}
	/// set bottom left and top right
	void	set( const T xMin, const T yMin, const T xMax, const T yMax ) noexcept
	{
		m_PosLB.x = xMin;
		m_PosLB.y = yMin;
		m_PosRT.x = xMax;
		m_PosRT.y = yMax;
	}
	/// recenter box on origin
	void	recenter() noexcept
	{
		const auto w = width() * 0.5;
		const auto h = height() * 0.5;
		m_PosLB.x = T(-w);
		m_PosLB.y = T(-h);
		m_PosRT.x = T(w);
		m_PosRT.y = T(h);
	}
	/// clear box
	void	zero()
	{
		m_PosLB = vec2T();
		m_PosRT = vec2T();
	}

	vec2T	top_left() const { return vec2T( m_PosLB.x, m_PosRT.y ); };
	vec2T	top_right() const { return m_PosRT; };
	vec2T	bottom_right() const { return vec2T( m_PosRT.x, m_PosLB.y ); };
	vec2T	bottom_left() const { return m_PosLB; };
	vec2T	center() const { return vec2T( ( m_PosLB.x + m_PosRT.x ) / 2, ( m_PosLB.y + m_PosRT.y ) / 2 ); }
	vec2T	size() const { return vec2T( width(), height() ); }

	/// translate box
	void	translate( const T x, const T y ) noexcept;
	void	translate( const vec2T &offset ) noexcept;
	BoxT	translated( const vec2T& val ) const noexcept;

	/// Translates the box so that its upper-left corner is newUL
	void	moveBL( const vec2T &newUL );
	/// Returns a copy of the box translated so that its upper-left corner is newUL
	BoxT	movedBL( const vec2T &newUL ) const;

	void	canonicalize(); // return rect w/ properly ordered coordinates
	BoxT	canonicalized() const; // return rect w/ properly ordered coordinates

	void	clip( const BoxT &clip );
	BoxT	clipped( const BoxT &clip ) const;

	void	inflate( const vec2T &amount );
	BoxT	inflated( const vec2T &amount ) const;

	/// Translates the rectangle so that its center is at center
	void		recenter( const vec2T &newCenter ) { translate( newCenter - center() ); }
	void		scale_centered( const vec2T &scale );
	BoxT		scaled_centered( const vec2T &scale ) const;
	void		scale_centered( const T scale );
	BoxT		scaled_centered( const T scale ) const;
	void		scale( const T scale );
	void		scale( const vec2T &scale );
	BoxT		scaled( const T scale ) const;
	BoxT		scaled( const vec2T &scale ) const;

	/// Transforms the Box by matrix. Represents the bounding box of the transformed Rect when matrix expresses non-scale/translate operations.
	void		transform( const mat3T &matrix );
	/// Returns a copy of the Box transformed by matrix. Represents the bounding box of the transformed Rect when matrix expresses non-scale/translate operations.
	BoxT		transformed( const mat3T &matrix ) const;

	/// Is a point pt inside the rectangle
	template<typename Y>
	bool		contains( const glm::tvec2<Y, glm::defaultp> &pt ) const noexcept
	{
		return ( pt.x >= left() ) && ( pt.x <= right() ) && ( pt.y >= bottom() ) && ( pt.y <= top() );
	}
	bool		contains( const vec2T &pt ) const noexcept
	{
		return ( pt.x >= left() ) && ( pt.x <= right() ) && ( pt.y >= bottom() ) && ( pt.y <= top() );
	}

	/// Returns the distance between the point \a pt and the rectangle. Points inside the rectangle return \c 0.
	T			distance( const vec2T &pt ) const;
	/// Returns the squared distance between the point \a pt and the rectangle. Points inside the rectangle return \c 0.
	T			distance_squared( const vec2T &pt ) const;

	/// Returns the nearest point on the Rect \a rect. Points inside the rectangle return \a pt.
	vec2T		closest_point( const vec2T &pt ) const;

	/// return Scaled copy with the same aspect ratio centered relative to and scaled to fit inside other. If expand then the rectangle is expanded if it is smaller than other
	BoxT		centered_fit( const BoxT &other, const bool expand ) const;
	/// return Scaled copy with the same aspect ratio centered relative to and scaled so it completely fills other. If contract then the rectangle is contracted if it is larger than other
	BoxT		centered_fill( const BoxT &other, const bool contract ) const;

	/// Expands the Rect to include point in its interior
	void		include( const vec2T &point );
	void		include( const vec3T &point );
	/// Expands the Rect to include all points in points in its interior
	void		include( const std::vector<vec2T > &points );
	/// Expands the Rect to include all points in points in its interior
	void		include( const std::vector<vec3T > &points );
	/// Expands the Rect to include rect in its interior
	void		include( const BoxT &rect );

	///// return true if point is inside box
	//bool inside( const T x, const T y ) const noexcept
	//{
	//	if ( ( left() <= x ) && ( x <= left() + width() ) && ( bottom() <= y ) && ( y <= bottom() + height() ) )
	//		return true;
	//	return false;
	//}
	/// return true if box overlap
	bool overlap( const BoxT<T>& b, const T spacing ) const noexcept
	{
		return !( right() + spacing <= b.left() || b.right() + spacing <= left() || top() + spacing <= b.bottom() || b.top() + spacing <= bottom() );
	}
	/// return true if box b fits
	bool can_fit( const BoxT<T>& b ) const noexcept
	{
		return width() >= b.width() && height() >= b.height();
	}
	/// return true if box b is contain inside
	bool contains( const BoxT<T>& b ) const noexcept
	{
		return b.left() >= left() && b.bottom() >= bottom() && b.right() <= right() && b.top() <= top();
	}
	/// return true if box are equal
	bool operator==( const BoxT<T>& b ) const noexcept
	{
		return m_PosLB == b.m_PosLB && m_PosRT == b.m_PosRT;
	}

	const BoxT<T>		operator+( const vec2T &o ) const { return this->translated( o ); }
	const BoxT<T>		operator-( const vec2T &o ) const { return this->translated( -o ); }
	const BoxT<T>		operator*( const T s ) const { return this->scaled( s ); }
	const BoxT<T>		operator/( const T s ) const { return this->scaled( ( (T)1 ) / s ); }

	const BoxT<T>		operator+( const BoxT<T>& rhs ) const { return BoxT<T>( m_PosLB.x + rhs.m_PosLB.x, m_PosLB.y + rhs.m_PosLB.y, m_PosRT.x + rhs.m_PosRT.x, m_PosRT.y + rhs.m_PosRT.y ); }
	const BoxT<T>		operator-( const BoxT<T>& rhs ) const { return BoxT<T>( m_PosLB.x - rhs.m_PosLB.x, m_PosLB.y - rhs.m_PosLB.y, m_PosRT.x - rhs.m_PosRT.x, m_PosRT.y - rhs.m_PosRT.y ); }

	BoxT<T>&			operator+=( const vec2T &o ) { translate( o ); return *this; }
	BoxT<T>&			operator-=( const vec2T &o ) { translate( -o ); return *this; }
	BoxT<T>&			operator*=( T s ) { scale( s ); return *this; }
	BoxT<T>&			operator/=( T s ) { scale( ( (T)1 ) / s ); return *this; }



	/// return bssf score
	T score_bssf( const BoxT<T>& b )
	{
		return ::MIN( width() - b.width(), height() - b.height() );
	}
	/// return split boxes
	std::vector<BoxT<T>> make_splits( const BoxT<T> b, const T spacing )
	{
		std::vector<BoxT<T>> result;
		result.reserve( 4 );
		//result.clear();

		if ( x() + spacing < b.x() )
		{
			result.push_back( BoxT<T>{ glm::vec2(x(), y()), glm::vec2(b.x() - spacing, y() + height()) } );
		}

		if ( right() > b.right() + spacing )
		{
			result.push_back( BoxT<T>{ glm::vec2(b.right() + spacing, bottom()), glm::vec2(right(), bottom() + height()) } );
		}

		if ( top() > b.top() + spacing )
		{
			result.push_back( BoxT<T>{ glm::vec2(left(), b.top() + spacing), glm::vec2(left() + width(), top()) } );
		}

		if ( bottom() + spacing < b.bottom() )
		{
			result.push_back( BoxT<T>{ glm::vec2(left(), bottom()), glm::vec2(left() + width(), b.bottom() - spacing) } );
		}
		return result;
	}
	/// return true if boxes overlaps
	static bool overlap( const BoxT<T>& a, const BoxT<T>& b, const T spacing ) noexcept
	{
		return !( a.right() + spacing <= b.left() || b.right() + spacing <= a.left() || a.top() + spacing <= b.bottom() || b.top() + spacing <= a.bottom() );
	}
	/// return split boxes
	static void make_splits( const BoxT<T> a, const BoxT<T> b, std::vector< BoxT<T> >& result, const T spacing )
	{
		result.clear();

		if ( a.x() + spacing < b.x() )
		{
			result.push_back( BoxT<T>{ glm::vec2( a.x(), a.y() ), glm::vec2( b.x() - a.x() - spacing, a.height() ) } );
		}

		if ( a.right() > b.right() + spacing )
		{
			result.push_back( BoxT<T>{ glm::vec2( b.right() + spacing, a.bottom() ), glm::vec2( a.right() - b.right() - spacing, a.height() ) } );
		}

		if ( a.top() > b.top() + spacing )
		{
			result.push_back( BoxT<T>{glm::vec2( a.left(), b.top() + spacing ), glm::vec2( a.width(), a.top() - b.top() - spacing ) } );
		}

		if ( a.bottom() + spacing < b.bottom() )
		{
			result.push_back( BoxT<T>{ glm::vec2( a.left(), a.bottom() ), glm::vec2( a.width(), b.bottom() - a.bottom() - spacing ) } );
		}
	}
	/// return true if box b fits
	static bool can_fit( const BoxT<T>& a, const BoxT<T>& b ) noexcept
	{
		return a.width() >= b.width() && a.height() >= b.height();
	}
	/// return true if box b is contain inside
	static bool contains( const BoxT<T>& a, const BoxT<T>& b ) noexcept
	{
		return b.left() >= a.left() && b.bottom() >= a.bottom() && b.right() <= a.right() && b.top() <= a.top();
	}
	vec2T m_PosLB	{};		// bottom left position
	vec2T m_PosRT	{};		// top right position
	int	id{};
};

using boxd = BoxT< double >;
using boxf = BoxT< float >;
using boxs = BoxT< std::size_t >;


template< typename T >
bool operator==( const BoxT<T>& a, const BoxT<T>& b ) noexcept
{
	return a.x() == b.x() && a.y() == b.y() && a.width() == b.width() && a.height() == b.height();
}

// This class maps a rectangle into another rectangle
class  BoxMapping
{
public:
	BoxMapping()
		: mSrcRect( 0, 0, 0, 0 )
		, mDstRect( 0, 0, 0, 0 )
	{
	}
	BoxMapping( const boxf &aSrcRect, const boxf &aDstRect )
		: mSrcRect( aSrcRect ), mDstRect( aDstRect )
	{
	}
	BoxMapping( const boxf &aSrcRect, const boxf &aDstRect, const bool preserveSrcAspect );

	glm::vec2	map( const glm::vec2 &srcPoint ) const;
	boxf		map( const boxf &srcRect ) const;

private:
	boxf	mSrcRect;
	boxf	mDstRect;
};

}

