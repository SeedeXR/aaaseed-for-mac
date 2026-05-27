
#ifdef AAA_GLTTGLYPHPOLYGONIZERHANDLER_H
#error "GLTTGLYPHPOLYGONIZERHANDLER_H included more than once."
#endif
#define AAA_GLTTGLYPHPOLYGONIZERHANDLER_H 1


#ifndef AAA_OURTRUETYPE_H
#	include "ourtrueType.h"
#endif

#if AAA_TRUETYPE1_USE()

#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif 

#ifndef AAA_FTGlyphVectorizer_H
#include "FTGlyphVectorizer.h"
#endif

class GLTTGlyphPolygonizerHandler
{
	friend class GLTTGlyphPolygonizer;

protected:
	bool _b_verbose;
	GLTTGlyphPolygonizer* _polygonizer; // set by GLTTGlyphPolygonizer

public:
	GLTTGlyphPolygonizerHandler( bool b_verbose = true );

	virtual ~GLTTGlyphPolygonizerHandler();

	virtual void begin( INT32 type );
	virtual void vertex( FTGlyphVectorizer::POINT* point );
	virtual void end();
	virtual void error( INT32 error );
	//hack not sure about the types
	virtual void combine( DOUBLE coords[3], void  *d[4], FP32 w[4], void  **dataOut );
};

#endif	//#if AAA_TRUETYPE1_USE()
