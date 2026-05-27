
#ifdef AAA_AAA_FTFACE_H
#error "AAA_FTFACE_H included more than once."
#endif
#define AAA_AAA_FTFACE_H 1


#ifndef AAA_OURTRUETYPE_H
#	include "ourtrueType.h"
#endif

#if AAA_TRUETYPE1_USE()

#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

#if AAA_TRUETYPE2_USE()
#	include <ft2build.h>
#	include "freetype2/freetype/freetype.h"
//struct FT_Face_;
//typedef struct TT_Face_ FT_Face;
#	define	AAA_FT_FACE FT_Face
#else
	struct TT_Face_;
	typedef struct TT_Face_ TT_Face;
#	define	AAA_FT_FACE TT_Face
#endif

class FTEngine;

class AAA_FTFace
{
private:
	FTEngine*		_engine;
	AAA_FT_FACE*	_face;

	INT32			_indices[256];
	INT32*			_CP_table;

public:
	AAA_FTFace( FTEngine* engine = nullptr );
	virtual ~AAA_FTFace();

	bool open( C_PCHAR_C filename );
	INT32 CP_Translate( INT32 code ) CONST;
private:
	bool makeIndicesTable();

public:
	FTEngine* getEngine() CONST
	{
		return _engine;
	}

	AAA_FT_FACE* getFace() CONST
	{
		return _face;
	}

	INT32 getGlyphIndex( INT32 ascii_code ) CONST
	{
		if( ascii_code < 0 || ascii_code > 255 )
			ascii_code = 0;
		return _indices[ascii_code];
	}
};

#endif //#if AAA_TRUETYPE1_USE()

