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

#include "FTInternals.h"
#include "FTBitmapFontImpl.h"


#if AAASEED()
#   ifndef	AAA_GOL_H
#       include "gol/gol.h"
#   endif
#else
#	include "FTGL/FTLibrary.h"
#endif

//
//  FTBitmapFont
//


FTBitmapFont::FTBitmapFont( char const * const fontFilePath) :
    FTFont(new FTBitmapFontImpl(this, fontFilePath))
{}


FTBitmapFont::FTBitmapFont( unsigned char const * const pBufferBytes, FTGL::VECTOR_SIZE_TYPE const bufferSizeInBytes ) :
    FTFont(new FTBitmapFontImpl(this, pBufferBytes, bufferSizeInBytes))
{}


FTBitmapFont::~FTBitmapFont()
{}


FTGlyph* FTBitmapFont::MakeGlyph(FT_GlyphSlot ftGlyph)
{
    return new FTBitmapGlyph(ftGlyph);
}


//
//  FTBitmapFontImpl
//


template <typename T>
inline FTPoint FTBitmapFontImpl::RenderI(const T* string, const int len,
                                         FTPoint position, FTPoint spacing,
                                         int renderMode)
{
    // Protect GL_BLEND
#if AAASEED()
	GOL::push_att();
	GOL::push_blend(false);
#else
	bool LegacyOpenGLState = FTLibrary::Instance().GetLegacyOpenGLStateSet();
	if( LegacyOpenGLState )
	{
		GOL::push_attrib(GL_COLOR_BUFFER_BIT);
		GOL::disable_blend();
	}
#endif

    // Protect glPixelStorei() calls (also in FTBitmapGlyphImpl::RenderImpl)
    GOL::push_client_attrib(GL_CLIENT_PIXEL_STORE_BIT);
        GOL::set_pixel_storei(GL_UNPACK_LSB_FIRST, GL_FALSE);
        GOL::set_pixel_storei(GL_UNPACK_ALIGNMENT, 1);

        FTPoint tmp = FTFontImpl::Render(string, len,
                                         position, spacing, renderMode);
    GOL::pop_client_attrib();

#if AAASEED()
	GOL::pop_att();
#else
	if( LegacyOpenGLState )
        GOL::pop_attrib();
#endif
    return tmp;
}


FTPoint FTBitmapFontImpl::Render(const char * string, const int len,
                                 FTPoint position, FTPoint spacing,
                                 int renderMode)
{
    return RenderI(string, len, position, spacing, renderMode);
}


FTPoint FTBitmapFontImpl::Render(const wchar_t * string, const int len,
                                 FTPoint position, FTPoint spacing,
                                 int renderMode)
{
    return RenderI(string, len, position, spacing, renderMode);
}

