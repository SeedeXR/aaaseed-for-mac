#include "AAA_font_tmp.h"

#if AAA_TRUETYPE1_USE()

#include "FTInstance.h"

#include "AAA_FTGlyph.h"


AAA_font_tmp::AAA_font_tmp( FTInstance* instance )
{
	_instance	= instance;
	_glyphs		= nullptr;
}

AAA_font_tmp::~AAA_font_tmp()
{
	 destroy();
	_instance = nullptr;
}

void AAA_font_tmp::destroy()
{
	if( _glyphs )
	{
		for( int i= 0; i < 256; ++i )
			delete _glyphs[i];
		delete[] _glyphs;
		_glyphs = nullptr;
	}
}

bool AAA_font_tmp::create()
{
	destroy();
	if( !_instance )
		return false;
	int i;
	_glyphs= new AAA_FTGlyph* [ 256 ];
	for( i= 0; i < 256; ++i )
		_glyphs[i]= 0;

	for( i= 0; i < 256; ++i )
	{
		AAA_FTGlyph* glyph = new AAA_FTGlyph(_instance);
		if( ! glyph->create(i) )
		{
			delete glyph;
			continue;
		}
		_glyphs[i]= glyph;
	}
	return true;
}


INT32 AAA_font_tmp::get_height() CONST
{
	if( !_instance )
		return 0;
	return _instance->get_sy();
}

INT32 AAA_font_tmp::get_descender() CONST
{
	if( !_instance )
		return 0;
	return _instance->getDescender();
}

INT32 AAA_font_tmp::get_width( C_PCHAR text )
{
	if( !text || !_glyphs )
		return 0;
	int w = 0;
	for(;;)
	{
		int ch= (unsigned char) *(text++);
		if( ch == 0 )
			break;
		if( _glyphs[ch] == 0 )
			continue;
		w += _glyphs[ch]->getAdvance();
	}
	return w / 64;
}

// Contributed by Gerard L. Lanois <gerard@msi.com>
void AAA_font_tmp::get_bbox( C_PCHAR text, INT32& llx, INT32& lly, INT32& urx, INT32& ury ) CONST
{
	llx = lly = urx = ury = 0;

	if( text == 0 || _glyphs == 0 )
		return;

	bool b_first = true;

	for(;;)
	{
		int ch = (unsigned char) *(text++);
		if( ch == 0 )
			break;

		if( _glyphs[ch] == 0 )
			continue;

		int xMin, yMin, xMax, yMax;
		if( ! _glyphs[ch]->get_bbox( xMin, yMin, xMax, yMax ) )
			continue;

		if( b_first )
		{
			llx = xMin;
			b_first = false;
		}

		if( yMin < lly )
			lly = yMin;

		if( yMax > ury )
			ury = yMax;

		urx += _glyphs[ch]->getAdvance();
	}

	llx >>= 6 ;	//was /= 64
	lly >>= 6;
	urx >>= 6;
	ury >>= 6;
}

#endif	//#if AAA_TRUETYPE1_USE()
