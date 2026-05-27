
#ifdef AAA_TAREA_H
#error "TAREA_H included more than once."
#endif
#define AAA_TAREA_H 1


#ifndef AAA_NATIVE_SYSTEM_H
#	include "Native_System.h"
#endif

#ifndef AAA_TMATH_H
#	include "tmath/TMath.h"
#endif

#ifndef AAA_TVECTOR_H
#	include "tmath/TVector.h"
#endif
#ifndef _VECTOR_
#	include <vector>
#endif

template<typename T>
class TRect;

template<typename T>
class TArea 
{
public:
	T	x1;
	T	y1;
	T	x2;
	T	y2;


 public:
	TArea( void );
	TArea( const TVec2<T> &UL, const TVec2<T> &LR );
	TArea( T aX1, T aY1, T aX2, T aY2 );
	explicit TArea( const TRect<T> &r );

	void set( T aX1, T aY1, T aX2, T aY2 );
	
	void clipBy( const TArea<T> &clip );
	TArea<T> getClipBy( const TArea<T> &clip ) const;

	//! Translates the TArea by \a off
	void offset( const TVec2<T> &off );
	//! Returns a copy of the TArea translated by \a off
	TArea<T> getOffset( const TVec2<T> &off ) const;
	//! Translates the TArea so that its upper-left corner is \a newUL
	void moveULTo( const TVec2<T> &newUL );
	//! Returns a copy of the TArea translated so that its upper-left corner is \a newUL
	TArea<T> getMoveULTo( const TVec2<T> &newUL ) const;
	//! Expands the TArea by \a expandX horizontally and \a expandY vertically. \a expandX is subtracted from \c x1 and added to \c x2 and \a expandY is subtracted from \c y1 and added to \c y2.
	void expand( T expandX, T expandY ) { x1 -= expandX; x2 += expandX; y1 -= expandY; y2 += expandY; }

	bool contains( const TVec2<T> &offset ) const;

	bool intersects( const TArea<T> &area ) const;

	//! Expands the TArea to include \a point in its interior
	void include( const TVec2<T> &point );
	//! Expands the TArea to include all points in \a points in its interior
	void include( const std::vector<TVec2<T> > &points );
	//! Expands the TArea to include \a rect in its interior
	void include( const TArea<T> &area );

	//! Returns the distance between the point \a pt and the rectangle. Points inside the TArea return \c 0.
	float distance( const TVec2<T> &pt ) const;
	//! Returns the squared distance between the point \a pt and the rectangle. Points inside the rectangle return \c 0.
	float distanceSquared( const TVec2<T> &pt ) const;

	//! Returns the nearest point on the Rect \a rect. Points inside the rectangle return \a pt.
	TVec2<T> closestPoint( const TVec2<T> &pt ) const;

	bool operator==( const TArea<T> &aArea ) const { return ( ( x1 == aArea.x1 ) && ( y1 == aArea.y1 ) && ( x2 == aArea.x2 ) && ( y2 == aArea.y2 ) ); }
	bool operator<( const TArea<T> &aArea ) const;

	const TArea<T> operator+( const TVec2<T> &o ) const { return this->getOffset( o ); }
	const TArea<T> operator-( const TVec2<T> &o ) const { return this->getOffset( -o ); }

	const TArea<T> operator+( const TArea<T>& rhs ) const { return TArea<T>( x1 + rhs.x1, y1 + rhs.y1, x2 + rhs.x2, y2 + rhs.y2 ); }
	const TArea<T> operator-( const TArea<T>& rhs ) const { return TArea<T>( x1 - rhs.x1, y1 - rhs.y1, x2 - rhs.x2, y2 - rhs.y2 ); }

	TArea<T>& operator+=( const TVec2<T> &o ) { offset( o ); return *this; }
	TArea<T>& operator-=( const TVec2<T> &o ) { offset( -o ); return *this; }

	static TArea<T>	proportionalFit( const TArea<T> &srcArea, const TArea<T> &dstArea, bool center, bool expand = false );

	static std::pair<TArea<T>, TVec2<T>> clippedSrcDst(const TArea<T> &srcSurfaceBounds, const TArea<T> &srcArea, const TArea<T> &dstSurfaceBounds, const TVec2<T> &dstLT);	



	T			getWidth() const { return x2 - x1; }
	T			getHeight() const { return y2 - y1; }
	TVec2<T>	getSize() const { return TVec2<T>( x2 - x1, y2 - y1 ); }
	TVec2<T>	getCenter() const { return TVec2<T>( ( x1 + x2 ) / 2.0, ( y1 + y2 ) / 2.0 ); }
	T			calcArea() const { return getWidth() * getHeight(); }


	T			getX1() const { return x1; }
	void		setX1( T aX1 ) { x1 = aX1; }
	T			getY1() const { return y1; }
	void		setY1( T aY1 ) { y1 = aY1; }
	T			getX2() const { return x2; }
	void		setX2( T aX2 ) { x2 = aX2; }
	T			getY2() const { return y2; }
	void		setY2( T aY2 ) { y2 = aY2; }
	T			getUL() const { return TVec2<T>( x1, y1 ); } // left-top offset
	T			getLR() const { return TVec2<T>( x2, y2 ); } // right-bottom offset
};


//=================================================================================================
template<typename T>
TArea<T>::TArea( void )
{}

//=================================================================================================
template<typename T>
TArea<T>::TArea(const TVec2<T> &UL, const TVec2<T> &LR)
{
	set( UL.x, UL.y, LR.x, LR.y );
}

//=================================================================================================
template<typename T>
TArea<T>::TArea( T aX1, T aY1, T aX2, T aY2 )
{ 
	set( aX1, aY1, aX2, aY2 ); 
}

//=================================================================================================
template<typename T>
TArea<T>::TArea( const TRect<T> &r )
{
	set( r.x1, r.y1, r.x2, r.y2);
}



//=================================================================================================
template<typename T>
void TArea<T>::set(T aX1, T aY1, T aX2, T aY2)
{
	if (aX1 <= aX2) {
		x1 = aX1;
		x2 = aX2;
	}
	else {
		x1 = aX2;
		x2 = aX1;
	}

	if (aY1 <= aY2) {
		y1 = aY1;
		y2 = aY2;
	}
	else {
		y1 = aY2;
		y2 = aY1;
	}
}



//=================================================================================================
template<typename T>
void TArea<T>::clipBy(const TArea<T> &clip)
{
	if (x1 < clip.x1)
		x1 = clip.x1;
	if (x2 < clip.x1)
		x2 = clip.x1;
	if (x1 > clip.x2)
		x1 = clip.x2;
	if (x2 > clip.x2)
		x2 = clip.x2;

	if (y1 < clip.y1)
		y1 = clip.y1;
	if (y2 < clip.y1)
		y2 = clip.y1;
	if (y1 > clip.y2)
		y1 = clip.y2;
	if (y2 > clip.y2)
		y2 = clip.y2;
}

//=================================================================================================
template<typename T>
TArea<T> TArea<T>::getClipBy(const TArea<T> &clip) const
{
	TArea<T> result(*this);
	result.clipBy(clip);
	return result;
}



//=================================================================================================
template<typename T>
void TArea<T>::offset(const TVec2<T> &o)
{
	x1 += o.x;
	x2 += o.x;
	y1 += o.y;
	y2 += o.y;
}

//=================================================================================================
template<typename T>
TArea<T> TArea<T>::getOffset(const TVec2<T> &offset) const
{
	return TArea<T>(x1 + offset.x, y1 + offset.y, x2 + offset.x, y2 + offset.y);
}



//=================================================================================================
template<typename T>
void TArea<T>::moveULTo(const TVec2<T> &newUL)
{
	set(newUL.x, newUL.y, newUL.x + getWidth(), newUL.y + getHeight());
}

//=================================================================================================
template<typename T>
TArea<T> TArea<T>::getMoveULTo(const TVec2<T> &newUL) const
{
	return TArea<T>(newUL.x, newUL.y, newUL.x + getWidth(), newUL.y + getHeight());
}



//=================================================================================================
template<typename T>
bool TArea<T>::contains(const TVec2<T> &offset) const
{
	return ((offset.x >= x1) && (offset.x < x2) && (offset.y >= y1) && (offset.y < y2));
}

//=================================================================================================
template<typename T>
bool TArea<T>::intersects(const TArea<T> &area) const
{
	if ((x1 >= area.x2) || (x2 < area.x1) || (y1 >= area.y2) || (y2 < area.y1))
		return false;
	else
		return true;
}



//=================================================================================================
template<typename T>
void TArea<T>::include(const TVec2<T> &point)
{
	if (x1 > point.x) x1 = point.x;
	if (x2 < point.x) x2 = point.x;
	if (y1 > point.y) y1 = point.y;
	if (y2 < point.y) y2 = point.y;
}

//=================================================================================================
template<typename T>
void TArea<T>::include(const std::vector<TVec2<T> > &points)
{
	for (size_t s = 0; s < points.size(); ++s)
		this->include(points[s]);
}

//=================================================================================================
template<typename T>
void TArea<T>::include(const TArea<T> &area)
{
	include( TVec2<T>(area.x1, area.y1) );
	include( TVec2<T>(area.x2, area.y2) );
}



//=================================================================================================
template<typename T>
float TArea<T>::distance(const TVec2<T> &pt) const
{
	float squaredDistance = 0;
	if (pt.x < x1) squaredDistance += (x1 - pt.x) * (x1 - pt.x);
	else if (pt.x > x2) squaredDistance += (pt.x - x2) * (pt.x - x2);
	if (pt.y < y1) squaredDistance += (y1 - pt.y) * (y1 - pt.y);
	else if (pt.y > y2) squaredDistance += (pt.y - y2) * (pt.y - y2);

	if (squaredDistance > 0)
		return tmath<T>::sqrt( squaredDistance );
	else
		return 0;
}

//=================================================================================================
template<typename T>
float TArea<T>::distanceSquared(const TVec2<T> &pt) const
{
	float squaredDistance = 0;
	if (pt.x < x1) squaredDistance += (x1 - pt.x) * (x1 - pt.x);
	else if (pt.x > x2) squaredDistance += (pt.x - x2) * (pt.x - x2);
	if (pt.y < y1) squaredDistance += (y1 - pt.y) * (y1 - pt.y);
	else if (pt.y > y2) squaredDistance += (pt.y - y2) * (pt.y - y2);

	return squaredDistance;
}



//=================================================================================================
template<typename T>
TVec2<T>	TArea<T>::closestPoint(const TVec2<T> &pt) const
{
	TVec2<T> result = pt;
	if (pt.x < (T)x1)
		result.x = (T)x1;
	else if (pt.x >(T)x2)
		result.x = (T)x2;
	if (pt.y < (T)y1)
		result.y = (T)y1;
	else if (pt.y >(T)y2)
		result.y = (T)y2;

	return result;
}



//=================================================================================================
template<typename T>
bool TArea<T>::operator<(const TArea<T> &aArea) const
{
	if (x1 != aArea.x1)
		return x1 < aArea.x1;
	if (y1 != aArea.y1)
		return y1 < aArea.y1;
	if (x2 != aArea.x2)
		return x2 < aArea.x2;
	if (y2 != aArea.y2)
		return y2 < aArea.y2;

	return false;
}



//=================================================================================================
template<typename T>
TArea<T> TArea<T>::proportionalFit(const TArea<T> &srcArea, const TArea<T> &dstArea, bool center, bool expand )
{
	T resultWidth, resultHeight;
	if (srcArea.getWidth() >= srcArea.getHeight()) { // wider than tall
		resultWidth = (expand) ? dstArea.getWidth() : tmath<T>::min(dstArea.getWidth(), srcArea.getWidth());
		resultHeight = resultWidth * srcArea.getHeight() / srcArea.getWidth();
		if (resultHeight > dstArea.getHeight()) { // maximized proportional would be too tall
			resultHeight = dstArea.getHeight();
			resultWidth = resultHeight * srcArea.getWidth() / srcArea.getHeight();
		}
	}
	else { // taller than wide
		resultHeight = (expand) ? dstArea.getHeight() : tmath<T>::min(dstArea.getHeight(), srcArea.getHeight());
		resultWidth = resultHeight * srcArea.getWidth() / srcArea.getHeight();
		if (resultWidth > dstArea.getWidth()) { // maximized proportional would be too wide
			resultWidth = dstArea.getWidth();
			resultHeight = resultWidth * srcArea.getHeight() / srcArea.getWidth();
		}
	}

	TArea<T> resultArea(0, 0, resultWidth, resultHeight);
	if (center)
		resultArea.offset( TVec2<T>((dstArea.getWidth() - resultWidth) / 2, (dstArea.getHeight() - resultHeight) / 2));
	resultArea.offset(dstArea.getUL());
	return resultArea;
}



//=================================================================================================
template<typename T>
std::pair<TArea<T>, TVec2<T>> TArea<T>::clippedSrcDst(const TArea<T> &srcSurfaceBounds, const TArea<T> &srcArea, const TArea<T> &dstSurfaceBounds, const TVec2<T> &dstLT)
{
	TArea<T> clippedSrc = srcArea.getClipBy(srcSurfaceBounds);
	TVec2<T> newDstLT = dstLT + (clippedSrc.getUL() - srcArea.getUL());
	TVec2<T> oldSrcLT = clippedSrc.getUL();
	clippedSrc.moveULTo(newDstLT);
	TArea<T> oldClippedDst = clippedSrc;
	clippedSrc.clipBy(dstSurfaceBounds);
	newDstLT = clippedSrc.getUL();
	clippedSrc.moveULTo(oldSrcLT + (clippedSrc.getUL() - oldClippedDst.getUL()));
	clippedSrc.clipBy(srcSurfaceBounds);
	return std::make_pair(clippedSrc, newDstLT);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//		Typedefs
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef TArea<float>		Areaf;
typedef TArea<double>		Aread;
typedef TArea<int32_t>		Areai;
typedef TArea<uint32_t>		Areaui;



