
#ifdef AAA_AAA_FTGLYPH_H
#error "AAA_FTGLYPH_H included more than once."
#endif
#define AAA_AAA_FTGLYPH_H 1



#ifndef AAA_OURTRUETYPE_H
#	include "ourtrueType.h"
#endif

#if AAA_TRUETYPE1_USE()

#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

struct TT_Glyph_;
typedef struct TT_Glyph_ TT_Glyph;
struct TT_Glyph_Metrics_;
typedef struct TT_Glyph_Metrics_ TT_Glyph_Metrics;

class FTInstance;


class AAA_FTGlyph
{
protected:
	FTInstance*			_instance;
	TT_Glyph*			_glyph;
	INT32				_ascii_code;
	TT_Glyph_Metrics*	_metrics;

public:
	AAA_FTGlyph( FTInstance* instance );
	virtual ~AAA_FTGlyph();

	bool create( INT32 ascii_code );
	INT32 getAsciiCode() CONST
	{
		return _ascii_code;
	}
	TT_Glyph* getGlyph()
	{
		return _glyph;
	}
	FTInstance* getInstance()
	{
		return _instance;
	}

	INT32 getBearingX() CONST;
	INT32 getBearingY() CONST;
	INT32 getAdvance() CONST;

	bool get_bbox( INT32& xMin, INT32& yMin, INT32& xMax, INT32& yMax ) CONST;
};

#endif	//#if AAA_TRUETYPE1_USE()
