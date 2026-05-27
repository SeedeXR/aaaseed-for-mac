#include "FTInstance.h"

#if AAA_TRUETYPE1_USE()

#include "AAA_FTFace.h"
#include "AAA_FTGlyph.h"

#include "freetype1/freetype.h"

AAA_FTGlyph::AAA_FTGlyph( FTInstance* instance )
{
	_instance = instance;
	_glyph = nullptr;
	_metrics = new TT_Glyph_Metrics;
}

AAA_FTGlyph::~AAA_FTGlyph()
{
	delete _metrics;
	_metrics = nullptr;

	if( _glyph )
	{
		TT_Done_Glyph(*_glyph);
		delete _glyph;
		_glyph = nullptr;
	}

	_instance = nullptr;
}

bool AAA_FTGlyph::create( INT32 ascii_code )
{
	_ascii_code = ascii_code;

	if( _glyph )
	{
		delete _glyph;
		_glyph = nullptr;
	}

	if( !_instance )
		return false;
	if( _instance->getInstance() == 0 )
		return false;

	AAA_FTFace* face = _instance->getFace();
	if( face == 0 )
		return false;

	int glyph_index = face->getGlyphIndex(_ascii_code);

	_glyph = new TT_Glyph;

	TT_Error err = TT_New_Glyph( *face->getFace(), _glyph );
	if( err )
	{
		delete _glyph;
		_glyph = nullptr;
		return false;
	}

	err = TT_Load_Glyph( *(_instance->getInstance()), *_glyph, glyph_index, TTLOAD_DEFAULT );
	if( err )
	{
		delete _glyph;
		_glyph = nullptr;
		return false;
	}

	err = TT_Get_Glyph_Metrics( *_glyph, _metrics );
	if( err )
	{
		delete _metrics;
		_metrics = nullptr;
		return false;
	}

	return true;
}

INT32 AAA_FTGlyph::getBearingX() CONST
{
	return _metrics ? _metrics->bearingX : 0;
}

INT32 AAA_FTGlyph::getBearingY() CONST
{
	return _metrics ? _metrics->bearingY : 0;
//  return 0; //(metrics==0) ? 0 : metrics->bearingY;
			// metrics.bearingY is left uninitialized by freetype?!
}

INT32 AAA_FTGlyph::getAdvance() CONST
{
	return _metrics ? _metrics->advance : 0;
}

bool AAA_FTGlyph::get_bbox( INT32& xMin, INT32& yMin, INT32& xMax, INT32& yMax ) CONST
{
  if( !_metrics )
	return false;

  TT_BBox& bbox= _metrics->bbox; // glyph bounding box

  xMin= bbox.xMin;
  yMin= bbox.yMin;
  xMax= bbox.xMax;
  yMax= bbox.yMax;

  return true;
}

#endif	//#if AAA_TRUETYPE1_USE()

