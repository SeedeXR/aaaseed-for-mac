
#ifdef AAA_TRECT_H
#error "TRECT_H included more than once."
#endif
#define AAA_TRECT_H 1


#ifndef AAA_NATIVE_SYSTEM_H
#	include "Native_System.h"
#endif

#ifndef AAA_TMATH_H
#	include "math/TMath.h"
#endif

#ifndef AAA_TVECTOR_H
#	include "math/TVector.h"
#endif

#ifndef AAA_TAREA_H
#	include "math/TArea.h"
#endif

#include <iomanip>

//////////////////////////////////////////////////////////////////////////////////////////////////////
// Matrix22
template<typename T> 
class TMatrixAffine2;


template<typename T>
class TRect 
{
public:
	T			x1;
	T			y1; 
	T			x2;
	T			y2;

public:
	TRect( void );
	/** Initializes the rectangle to be the bounding box of \a points **/
	TRect(const std::vector<TVec2<T> > &points);
	TRect(T aX1, T aY1, T aX2, T aY2);
	TRect(const TVec2<T> &v1, const TVec2<T> &v2);
	explicit TRect(const TArea<T> &area);

	void				set(T aX1, T aY1, T aX2, T aY2);

	T					getWidth( void ) const { return x2 - x1; }
	T					getHeight( void ) const { return y2 - y1; }
	T					getAspectRatio( void ) const { return getWidth() / getHeight(); }
	T					calcArea( void ) const { return getWidth() * getHeight(); }

	void				canonicalize( void ); // return rect w/ properly ordered coordinates
	TRect<T>			canonicalized( void ) const; // return rect w/ properly ordered coordinates

	void				clipBy(const TRect<T> &clip);
	TRect<T>			getClipBy(const TRect<T> &clip) const;
	TArea<T>			getInteriorArea( void ) const;
	void				offset(const TVec2<T> &offset);
	TRect<T>			getOffset(const TVec2<T> &off) const { TRect<T> result(*this); result.offset(off); return result; }
	void				inflate(const TVec2<T> &amount);
	TRect<T>			inflated(const TVec2<T> &amount) const;
	//! Translates the rectangle so that its center is at \a center
	void				offsetCenterTo(const TVec2<T> &center) { offset(center - getCenter()); }
	void				scaleCentered(const TVec2<T> &scale);
	void				scaleCentered(T scale);
	TRect<T>			scaledCentered(T scale) const;
	void				scale(T scale);
	void				scale(const TVec2<T> &scale);
	TRect<T>			scaled(T scale) const;
	TRect<T>			scaled(const TVec2<T> &scale) const;

	//! Returns a copy of the Rect transformed by \a matrix. Represents the bounding box of the transformed Rect when \a matrix expresses non-scale/translate operations.
	TRect<T>			transformCopy(const class TMatrixAffine2<T> &matrix) const;

	/** \brief Is a point \a pt inside the rectangle **/
	template<typename Y>
	bool				contains(const TVec2<Y> &pt) const { return (pt.x >= x1) && (pt.x <= x2) && (pt.y >= y1) && (pt.y <= y2); }
	//! Returns whether \a rect intersects with this
	bool				intersects(const TRect<T> &rect) const;

	//! Returns the distance between the point \a pt and the rectangle. Points inside the rectangle return \c 0.
	T					distance(const TVec2<T> &pt) const;
	//! Returns the squared distance between the point \a pt and the rectangle. Points inside the rectangle return \c 0.
	T					distanceSquared(const TVec2<T> &pt) const;

	//! Returns the nearest point on the Rect \a rect. Points inside the rectangle return \a pt.
	TVec2<T>			closestPoint(const TVec2<T> &pt) const;

	/** \return Scaled copy with the same aspect ratio centered relative to and scaled to fit inside \a other. If \a expand then the rectangle is expanded if it is smaller than \a other */
	TRect				getCenteredFit(const TRect<T> &other, bool expand) const;

	/** Expands the Rect to include \a point in its interior **/
	void				include(const TVec2<T> &point);
	/** Expands the Rect to include all points in \a points in its interior **/
	void				include(const std::vector<TVec2<T> > &points);
	/** Expands the Rect to include \a rect in its interior **/
	void				include(const TRect<T> &rect);

	const TRect<T>		operator+(const TVec2<T> &o) const { return this->getOffset(o); }
	const TRect<T>		operator-(const TVec2<T> &o) const { return this->getOffset(-o); }
	const TRect<T>		operator*(T s) const { return this->scaled(s); }
	const TRect<T>		operator/(T s) const { return this->scaled(((T)1) / s); }

	const TRect<T>		operator+(const TRect<T>& rhs) const { return TRect<T>(x1 + rhs.x1, y1 + rhs.y1, x2 + rhs.x2, y2 + rhs.y2); }
	const TRect<T>		operator-(const TRect<T>& rhs) const { return TRect<T>(x1 - rhs.x1, y1 - rhs.y1, x2 - rhs.x2, y2 - rhs.y2); }

	TRect<T>&			operator+=(const TVec2<T> &o) { offset(o); return *this; }
	TRect<T>&			operator-=(const TVec2<T> &o) { offset(-o); return *this; }
	TRect<T>&			operator*=(T s) { scale(s); return *this; }
	TRect<T>&			operator/=(T s) { scale(((T)1) / s); return *this; }

	T					getX1() const { return x1; }
	T					getY1() const { return y1; }
	T					getX2() const { return x2; }
	T					getY2() const { return y2; }

	TVec2<T>			getUpperLeft() const { return TVec2<T>(x1, y1); };
	TVec2<T>			getUpperRight() const { return TVec2<T>(x2, y1); };
	TVec2<T>			getLowerRight() const { return TVec2<T>(x2, y2); };
	TVec2<T>			getLowerLeft() const { return TVec2<T>(x1, y2); };
	TVec2<T>			getCenter() const { return TVec2<T>((x1 + x2) / 2, (y1 + y2) / 2); }
	TVec2<T>			getSize() const { return TVec2<T>(x2 - x1, y2 - y1); }
};


//=================================================================================================
template<typename T>
TRect<T>::TRect( void ) 
{}

//=================================================================================================
template<typename T>
TRect<T>::TRect(const std::vector<TVec2<T> > &points)
{
	x1 = numeric_limits<T>::max();
	x2 = -numeric_limits<T>::max();
	y1 = numeric_limits<T>::max();
	y2 = -numeric_limits<T>::max();
	for (size_t s = 0; s < points.size(); ++s)
		include(points[s]);
}

//=================================================================================================
template<typename T>
TRect<T>::TRect( T aX1, T aY1, T aX2, T aY2 ) 
{
	set( aX1, aY1, aX2, aY2 );
}

//=================================================================================================
template<typename T>
TRect<T>::TRect( const TVec2<T> &v1, const TVec2<T> &v2 ) 
{
	set( v1.x, v1.y, v2.x, v2.y );
}

//=================================================================================================
template<typename T>
TRect<T>::TRect(const TArea<T> &area)
{
	set((T)area.getX1(), (T)area.getY1(), (T)(area.getX2()), (T)(area.getY2()));
}



//=================================================================================================
template<typename T>
void TRect<T>::set(T aX1, T aY1, T aX2, T aY2)
{
	x1 = aX1;
	y1 = aY1;
	x2 = aX2;
	y2 = aY2;
}



//=================================================================================================
template<typename T>
void TRect<T>::canonicalize( void )
{
	if (x1 > x2) {
		T temp = x1;
		x1 = x2;
		x2 = temp;
	}

	if (y1 > y2) {
		T temp = y1;
		y1 = y2;
		y2 = temp;
	}
}

//=================================================================================================
template<typename T>
TRect<T> TRect<T>::canonicalized( void ) const
{
	TRect<T> result(*this);
	result.canonicalize();
	return result;
}



//=================================================================================================
template<typename T>
void TRect<T>::clipBy(const TRect &clip)
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
TRect<T> TRect<T>::getClipBy(const TRect &clip) const
{
	TRect<T> result(*this);
	result.clipBy(TRect<T>(clip));
	return result;
}



//=================================================================================================
template<typename T>
void TRect<T>::offset(const TVec2<T> &offset)
{
	x1 += offset.x;
	x2 += offset.x;
	y1 += offset.y;
	y2 += offset.y;
}



//=================================================================================================
template<typename T>
void TRect<T>::inflate(const TVec2<T> &amount)
{
	x1 -= amount.x;
	x2 += amount.x;
	y1 -= amount.y; // assume canonical rect has y1 < y2
	y2 += amount.y;
}

//=================================================================================================
template<typename T>
TRect<T> TRect<T>::inflated(const TVec2<T> &amount) const
{
	TRect<T> result(*this);
	result.inflate(amount);
	return result;
}



//=================================================================================================
template<typename T>
void TRect<T>::scaleCentered(const TVec2<T> &scale)
{
	T halfWidth = getWidth() * scale.x / 2.0f;
	T halfHeight = getHeight() * scale.y / 2.0f;
	const TVec2<T> center(getCenter());
	x1 = center.x - halfWidth;
	x2 = center.x + halfWidth;
	y1 = center.y - halfHeight;
	y2 = center.y + halfHeight;
}

//=================================================================================================
template<typename T>
void TRect<T>::scaleCentered(T scale)
{
	T halfWidth = getWidth() * scale / 2;
	T halfHeight = getHeight() * scale / 2;
	const TVec2<T> center(getCenter());
	x1 = center.x - halfWidth;
	x2 = center.x + halfWidth;
	y1 = center.y - halfHeight;
	y2 = center.y + halfHeight;
}

//=================================================================================================
template<typename T>
TRect<T> TRect<T>::scaledCentered(T scale) const
{
	T halfWidth = getWidth() * scale / 2;
	T halfHeight = getHeight() * scale / 2;
	const TVec2<T> center(getCenter());
	return TRect<T>(center.x - halfWidth, center.y - halfHeight, center.x + halfWidth, center.y + halfHeight);
}



//=================================================================================================
template<typename T>
void TRect<T>::scale(T s)
{
	x1 *= s;
	x2 *= s;
	y1 *= s;
	y2 *= s;
}

//=================================================================================================
template<typename T>
void TRect<T>::scale(const TVec2<T> &scale)
{
	x1 *= scale.x;
	y1 *= scale.y;
	x2 *= scale.x;
	y2 *= scale.y;
}

//=================================================================================================
template<typename T>
TRect<T> TRect<T>::scaled(T s) const
{
	return TRect<T>(x1 * s, y1 * s, x2 * s, y2 * s);
}

//=================================================================================================
template<typename T>
TRect<T> TRect<T>::scaled(const TVec2<T> &scale) const
{
	return TRect<T>(x1 * scale.x, y1 * scale.y, x2 * scale.x, y2 * scale.y);
}



//=================================================================================================
template<typename T>
TRect<T> TRect<T>::transformCopy(const TMatrixAffine2<T> &matrix) const
{
	TRect<T> result;
	result.x1 =  numeric_limits<T>::max();
	result.x2 = -numeric_limits<T>::max();
	result.y1 =  numeric_limits<T>::max();
	result.y2 = -numeric_limits<T>::max();

	result.include( matrix.transformPoint(TVec2<T>(x1, y1)) );
	result.include( matrix.transformPoint(TVec2<T>(x2, y1)) );
	result.include( matrix.transformPoint(TVec2<T>(x2, y2)) );
	result.include( matrix.transformPoint(TVec2<T>(x1, y2)) );

	return result;
}



//=================================================================================================
template<typename T>
bool TRect<T>::intersects(const TRect<T> &rect) const
{
	if ((x1 > rect.x2) || (x2 < rect.x1) || (y1 > rect.y2) || (y2 < rect.y1))
		return false;
	else
		return true;
}



//=================================================================================================
template<typename T>
T TRect<T>::distance(const TVec2<T> &pt) const
{
	T squaredDistance = 0;
	if (pt.x < x1) squaredDistance += (x1 - pt.x) * (x1 - pt.x);
	else if (pt.x > x2) squaredDistance += (pt.x - x2) * (pt.x - x2);
	if (pt.y < y1) squaredDistance += (y1 - pt.y) * (y1 - pt.y);
	else if (pt.y > y2) squaredDistance += (pt.y - y2) * (pt.y - y2);

	if (squaredDistance > 0)
		return tmath<T>::sqrt(squaredDistance);
	else
		return 0;
}



//=================================================================================================
template<typename T>
T TRect<T>::distanceSquared(const TVec2<T> &pt) const
{
	T squaredDistance = 0;
	if (pt.x < x1) squaredDistance += (x1 - pt.x) * (x1 - pt.x);
	else if (pt.x > x2) squaredDistance += (pt.x - x2) * (pt.x - x2);
	if (pt.y < y1) squaredDistance += (y1 - pt.y) * (y1 - pt.y);
	else if (pt.y > y2) squaredDistance += (pt.y - y2) * (pt.y - y2);

	return squaredDistance;
}



//=================================================================================================
template<typename T>
TVec2<T>	TRect<T>::closestPoint(const TVec2<T> &pt) const
{
	TVec2<T> result = pt;
	if (pt.x < x1) result.x = x1;
	else if (pt.x > x2) result.x = x2;
	if (pt.y < y1) result.y = y1;
	else if (pt.y > y2) result.y = y2;
	return result;
}



//=================================================================================================
template<typename T>
void TRect<T>::include(const TVec2<T> &point)
{
	if (x1 > point.x) x1 = point.x;
	if (x2 < point.x) x2 = point.x;
	if (y1 > point.y) y1 = point.y;
	if (y2 < point.y) y2 = point.y;
}



//=================================================================================================
template<typename T>
void TRect<T>::include(const std::vector<TVec2<T> > &points)
{
	for (size_t s = 0; s < points.size(); ++s)
		include(points[s]);
}



//=================================================================================================
template<typename T>
void TRect<T>::include(const TRect<T> &rect)
{
	include(TVec2<T>(rect.x1, rect.y1));
	include(TVec2<T>(rect.x2, rect.y2));
}



//=================================================================================================
template<typename T>
TArea<T> TRect<T>::getInteriorArea( void ) const
{
	TRect<T> canon = canonicalized();

	return Area(static_cast<int32_t>(ceil(canon.x1)), static_cast<int32_t>(ceil(canon.y1)),
		static_cast<int32_t>(floor(canon.x2)) + 1, static_cast<int32_t>(floor(canon.y2)) + 1);
}



//=================================================================================================
template<typename T>
TRect<T> TRect<T>::getCenteredFit(const TRect<T> &other, bool expand) const
{
	TRect<T> result = *this;
	result.offset(other.getCenter() - result.getCenter());

	bool isInside = ((result.getWidth() < other.getWidth()) && (result.getHeight() < other.getHeight()));
	if (expand || (!isInside)) { // need to do some scaling
		T aspectAspect = result.getAspectRatio() / other.getAspectRatio();
		if (aspectAspect >= 1.0f) { // result is proportionally wider so we need to fit its x-axis
			T scaleBy = other.getWidth() / result.getWidth();
			result.scaleCentered(scaleBy);
		}
		else { // result is proportionally wider so we need to fit its y-axis
			T scaleBy = other.getHeight() / result.getHeight();
			result.scaleCentered(scaleBy);
		}
	}

	return result;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//		Typedefs
///////////////////////////////////////////////////////////////////////////////////////////////////

typedef TRect<int32_t>		Recti;
typedef TRect<uint32_t>		Rectui;
typedef TRect<float>		Rectf;
typedef TRect<double>		Rectd;



// This class maps a rectangle into another rectangle
//class RectMapping {
//public:
//	RectMapping()
//		: mSrcRect(0, 0, 0, 0), mDstRect(0, 0, 0, 0) {}
//	RectMapping(const Rectf &aSrcRect, const Rectf &aDstRect)
//		: mSrcRect(aSrcRect), mDstRect(aDstRect) {}
//	RectMapping(const Rectf &aSrcRect, const Rectf &aDstRect, bool preserveSrcAspect);
//
//	Vec2f	map(const Vec2f &srcPoint) const;
//	Rectf	map(const Rectf &srcRect) const;
//
//private:
//	Rectf	mSrcRect, mDstRect;
//};
//
//extern void getClippedScaledRects(const Areaf &srcSurfaceBounds, const Rectf &srcRect, const Areaf &dstSurfaceBounds, const Areaf &dstArea, Rectf *resultSrcRect, Areaf *resultDstRect);

//RectMapping::RectMapping(const Rectf &aSrcRect, const Rectf &aDstRect, bool preserveSrcAspect)
//: mSrcRect(aSrcRect), mDstRect(aDstRect)
//{
//	if (preserveSrcAspect) {
//		float srcAspect = aSrcRect.getWidth() / (float)aSrcRect.getHeight();
//		float dstAspect = aDstRect.getWidth() / (float)aDstRect.getHeight();
//		if (srcAspect < dstAspect) { // src is narrower aspect
//			float heightRatio = mDstRect.getHeight() / mSrcRect.getHeight();
//			float effectiveWidth = mSrcRect.getWidth() * heightRatio;
//			float offsetX = (mDstRect.getWidth() - effectiveWidth) / 2.0f;
//			float dstWidth = mDstRect.getHeight() * srcAspect;
//			mDstRect.set(mDstRect.x1 + offsetX, mDstRect.getY1(), mDstRect.x1 + offsetX + dstWidth, mDstRect.getY2());
//		}
//		else { // src is wider aspect
//			float effectiveHeight = mSrcRect.getHeight() * (mDstRect.getWidth() / mSrcRect.getWidth());
//			float offsetY = (mDstRect.getHeight() - effectiveHeight) / 2.0f;
//			float dstHeight = mDstRect.getWidth() / srcAspect;
//			mDstRect.set(mDstRect.x1, mDstRect.getY1() + offsetY, mDstRect.x2, mDstRect.getY1() + offsetY + dstHeight);
//		}
//	}
//}
//
//Vec2f RectMapping::map(const Vec2f &srcPoint) const
//{
//	float x = (srcPoint.x - mSrcRect.getX1()) / mSrcRect.getWidth() * mDstRect.getWidth() + mDstRect.getX1();
//	float y = (srcPoint.y - mSrcRect.getY1()) / mSrcRect.getHeight() * mDstRect.getHeight() + mDstRect.getY1();
//
//	return Vec2f(x, y);
//}
//
//Rectf RectMapping::map(const Rectf &srcRect) const
//{
//	Vec2f ul = map(Vec2f(srcRect.x1, srcRect.y1));
//	Vec2f lr = map(Vec2f(srcRect.x2, srcRect.y2));
//
//	return Rectf(ul.x, ul.y, lr.x, lr.y);
//}
//
//// This routine assumes that srcRect maps into dstArea
//// and trims both the dst and the src proportionally to accommodate any clipping
//void getClippedScaledRects(const Area &srcSurfaceBounds, const Rectf &srcRect, const Area &dstSurfaceBounds, const Area &dstArea, Rectf *resultSrcRect, Area *resultDstArea)
//{
//	// clip the destination, create newSrcRect from mapping
//	Area clippedDst = dstArea.getClipBy(dstSurfaceBounds);
//	Rectf newSrcRect = RectMapping(Rectf(dstArea), srcRect).map(Rectf(clippedDst));
//
//	// clip the src, create newDstRect from mapping
//	newSrcRect.clipBy(Rectf(srcSurfaceBounds));
//	Rectf newDstRect = RectMapping(srcRect, Rectf(dstArea)).map(Rectf(newSrcRect));
//
//	// discretize and clip the destinaton
//	// the discretization is done to minimize the area
//	*resultDstArea = newDstRect.getInteriorArea();
//	resultDstArea->clipBy(dstSurfaceBounds);
//
//	// now map the discretized, clipped destination back once more to get the final source
//	*resultSrcRect = RectMapping(Rectf(dstArea), srcRect).map(Rectf(*resultDstArea));
//
//	// this is not kosher, but sometimes we need to fudge things on the src to keep it in bounds
//	resultSrcRect->clipBy(Rectf(srcSurfaceBounds));
//}





