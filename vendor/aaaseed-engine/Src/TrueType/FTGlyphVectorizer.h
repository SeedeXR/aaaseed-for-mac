
#ifdef AAA_FTGLYPHVECTORIZER_H
#error "FTGLYPHVECTORIZER_H included more than once."
#endif
#define AAA_FTGLYPHVECTORIZER_H 1


#ifndef AAA_OURTRUETYPE_H
#	include "ourtrueType.h"
#endif

#if AAA_TRUETYPE1_USE()

#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

struct TT_Outline_;
typedef struct TT_Outline_ TT_Outline;

class AAA_FTGlyph;

class FTGlyphVectorizer
{
public:
	struct POINT
	{
		DOUBLE x, y;
		void* data;
	};

	class Contour
    {
    public:
		INT32	max_points;
		POINT*	points;
		INT32	nPoints;
		bool	b_clockwise;
		DOUBLE	area;
		DOUBLE	x_min, x_max;
		DOUBLE	y_min, y_max;

		Contour()
		{
			b_clockwise = false;
			nPoints= 0;
			points= 0;
			max_points= 0;
			area= 0.;
			x_min= y_min= 1e20;
			x_max= y_max=-1e20;
		}
		~Contour()
		{
			delete[] points;
			points= 0;
			nPoints= 0;
		}

		bool exterior() CONST
		{
			return b_clockwise;
		}
	private:
		void add_point( DOUBLE x, DOUBLE y );

		friend FTGlyphVectorizer;
    };

protected:
	AAA_FTGlyph*	_glyph;
	TT_Outline*		_outline;
	// needed distance between two bezier curve evaluations (in pixels)
	DOUBLE			_precision;
	Contour**		_contours;
	int				_nContours;

public:
	FTGlyphVectorizer();
	virtual ~FTGlyphVectorizer();

	void			destroy();
	bool			init( AAA_FTGlyph* _glyph );
	void			setPrecision( double _precision );
	AAA_FTGlyph*	getGlyph() const
					{
						return _glyph;
					}
	bool		vectorize();

	INT32		getNContours() const
				{
					return _nContours;
				}

	// 0 <= c < nContours
	Contour*	getContour( INT32 c ) CONST
				{
					if( c < 0 || c > _nContours || _contours == 0 )
						return nullptr;
					return _contours[c];
				}

	DOUBLE		getBearingX() CONST;
	DOUBLE		getBearingY() CONST;
	DOUBLE		getAdvance() CONST;

	// 0 <= c < nContours
	// return the number of bezier control points in npts_return
	// return a newly allocated double[npts*2] of 2D control points
	DOUBLE*		getBezier( INT32 c, INT32* npts_return ) CONST;

private:
	bool		vectorizeContour( INT32 c );
	void		add_point( INT32 c, DOUBLE x, DOUBLE y );
	void		sortContours();
};

#endif //#if AAA_TRUETYPE1_USE()

