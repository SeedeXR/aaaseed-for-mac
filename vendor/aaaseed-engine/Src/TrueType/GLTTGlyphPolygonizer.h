
#ifdef AAA_GLTTGLYPHPOLYGONIZER_H
#error "GLTTGLYPHPOLYGONIZER_H included more than once."
#endif
#define AAA_GLTTGLYPHPOLYGONIZER_H 1


#ifndef AAA_OURTRUETYPE_H
#	include "ourtrueType.h"
#endif

#if AAA_TRUETYPE1_USE()

#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

class AAA_FTGlyph;
class FTGlyphVectorizer;
class GLTTGlyphPolygonizerHandler;


class GLTTGlyphPolygonizer
{
protected:
	AAA_FTGlyph*		_glyph;

	FTGlyphVectorizer*	_vectorizer;
	bool				_b_own_vectorizer;

public:
	GLTTGlyphPolygonizer( FTGlyphVectorizer* vectorizer = 0 );

	virtual ~GLTTGlyphPolygonizer();

	void setPrecision( DOUBLE precision );

	bool init( AAA_FTGlyph* _glyph );

	bool polygonize( GLTTGlyphPolygonizerHandler* handler = 0 );

	FTGlyphVectorizer* getVectorizer() CONST
	{
		return _vectorizer;
	}

	DOUBLE getBearingX() CONST;
	DOUBLE getBearingY() CONST;
	DOUBLE getAdvance() CONST;
};

#endif //#if AAA_TRUETYPE1_USE()
