/*
 * FTGL - OpenGL font library
 *
 * Copyright (c) 2001-2004 Henry Maddocks <ftgl@opengl.geek.nz>
 * Copyright (c) 2008 Sam Hocevar <sam@hocevar.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef AAA_FTGL_CONFIG_H
#   include "ftgl_config.h"
#endif

#include "FTGL/ftgl.h"

#include "FTGlyphContainer.h"
#include "FTFace.h"
#include "FTCharmap.h"


FTGlyphContainer::FTGlyphContainer(FTFace* f)
:   _face(f),
    _err(0)
{
    _glyphs.push_back(nullptr);
    _char_map = new FTCharmap(_face);
}


FTGlyphContainer::~FTGlyphContainer()
{
    GlyphVector::iterator it;
    for( it = _glyphs.begin(); it != _glyphs.end(); ++it )
        delete *it;

    _glyphs.clear();
    delete _char_map;
}


bool FTGlyphContainer::set_encoding(FT_Encoding encoding)
{
    bool result = _char_map->CharMap(encoding);
    _err = _char_map->Error();
    return result;
}


unsigned int FTGlyphContainer::get_font_index( FTGL::CHARACTER_CODE const charCode) const
{
    return _char_map->FontIndex(charCode);
}


void FTGlyphContainer::add(FTGlyph* tempGlyph, FTGL::CHARACTER_CODE const charCode)
{
    _char_map->InsertIndex(charCode, _glyphs.size());
    _glyphs.push_back(tempGlyph);
}


const FTGlyph* FTGlyphContainer::get( FTGL::CHARACTER_CODE const charCode ) const
{
	FTGL::GLYPH_INDEX const index = _char_map->GlyphListIndex(charCode);
    return (index < _glyphs.size()) ? _glyphs[index] : nullptr;
}

FTGlyph* FTGlyphContainer::get_changeable( FTGL::CHARACTER_CODE const charCode ) const
{
	FTGL::GLYPH_INDEX const index = _char_map->GlyphListIndex(charCode);
    return (index < _glyphs.size()) ? _glyphs[index] : nullptr;
}


FTBBox FTGlyphContainer::get_bbox( FTGL::CHARACTER_CODE const charCode) const
{
    return get(charCode)->BBox();
}


float FTGlyphContainer::get_advance(	FTGL::CHARACTER_CODE const char_code,
										FTGL::CHARACTER_CODE const char_code_next )
{
#if 0 //tried by maa to align with render but bad fuckup BBox case
	auto const left  = get_font_index( char_code );
	auto const right = get_font_index( char_code_next );

	float advance = _face->KernAdvance( left, right ).X();
	if( !_face->Error() )
    {
		FTGlyph const * const glyph = get( char_code );
		if( glyph )
			advance += glyph->Advance();
	}
	return 0.;
#else
	FTGlyph const * const glyph = get( char_code );
    if( glyph )
	{
		auto const left  = get_font_index( char_code );
		auto const right = get_font_index( char_code_next );
		return _face->KernAdvance( left, right ).Xf() + glyph->Advance();
	}
	return 0.0f;
#endif
}

FTPoint FTGlyphContainer::render(	FTGL::CHARACTER_CODE const char_code,
									FTGL::CHARACTER_CODE const char_code_next,
									FTPoint penPosition, int renderMode)
{
    auto const left  = get_font_index( char_code );
    auto const right = get_font_index( char_code_next );

    FTPoint advance = _face->KernAdvance( left, right );
    if( !_face->Error() )
    {
		FTGlyph * const glyph = get_changeable( char_code );
        if( glyph )
            advance = advance + glyph->Render( penPosition, renderMode );
    }

    return advance;
}
