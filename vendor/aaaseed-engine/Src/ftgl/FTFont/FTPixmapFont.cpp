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
#include "FTPixmapFontImpl.h"

#if AAASEED()
#   include "gol/gol.h"
#   include "gol/gol_tex.h"
#else
#	include "FTGL/FTLibrary.h"
#endif

//
//  FTPixmapFont


FTPixmapFont::FTPixmapFont(char const * const fontFilePath) :
    FTFont(new FTPixmapFontImpl(this, fontFilePath))
{}


FTPixmapFont::FTPixmapFont( const unsigned char * const pBufferBytes, FTGL::VECTOR_SIZE_TYPE const bufferSizeInBytes ) :
    FTFont(new FTPixmapFontImpl(this, pBufferBytes, bufferSizeInBytes))
{}


FTPixmapFont::~FTPixmapFont()
{}


FTGlyph* FTPixmapFont::MakeGlyph(FT_GlyphSlot ftGlyph)
{
    return new FTPixmapGlyph(ftGlyph);
}


//
//  FTPixmapFontImpl
//


FTPixmapFontImpl::FTPixmapFontImpl( FTFont * const ftFont, const char* const fontFilePath)
: FTFontImpl(ftFont, fontFilePath)
{
    load_flags = FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP;
}


FTPixmapFontImpl::FTPixmapFontImpl( FTFont * const ftFont, const unsigned char * const pBufferBytes, FTGL::VECTOR_SIZE_TYPE const bufferSizeInBytes )
: FTFontImpl(ftFont, pBufferBytes, bufferSizeInBytes)
{
    load_flags = FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP;
}


template <typename T>
inline FTPoint FTPixmapFontImpl::RenderI(const T* string, const int len,
                                         FTPoint position, FTPoint spacing,
                                         int renderMode)
{
    //todo do it with gol
    // Protect GL_TEXTURE_2D, glPixelTransferf() and optionally GL_BLEND
    ///GOL::push_attrib(GL_ENABLE_BIT | GL_PIXEL_MODE_BIT | GL_COLOR_BUFFER_BIT
    //              | GL_POLYGON_BIT);
    GOL::push_att();

    // Protect glPixelStorei() calls (made by FTPixmapGlyphImpl::RenderImpl).
    GOL::push_client_attrib(GL_CLIENT_PIXEL_STORE_BIT);

    // Needed on OSX
    //GOL::set_polygon_mode(GL_FRONT_AND_BACK, GL_FILL);
    GOL::push_polygon_mode_same( GL_FILL );

#if AAASEED()
#else
    if( FTLibrary::Instance().GetLegacyOpenGLStateSet() )
    {
        //maatmp   GOL::enable_blend();
        GOL::enable_blend_low();
        /*
        * Note: This is the historic legacy behaviour.
        *
        * A better blending function (see
        * https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=742469) is:
        *
        *   glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA,
        *                       GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        *
        * To use it, set
        *
        *   FTLibrary::Instance().LegacyOpenGLState(false);
        *
        * and set GL_BLEND and the blending function yourself.
        */
        //GOL::set_blend_func(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        GOL::set_blend_func( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA );
    }
#endif

    GOL::set_texture_0D();

    GLfloat ftglColour[4];
    //todo this is bad for performance and really Legacy stuff
    glGetFloatv( GL_CURRENT_RASTER_COLOR, ftglColour );

    GOL::set_pixel_transferf( GL_RED_SCALE,   ftglColour[0] );
    GOL::set_pixel_transferf( GL_GREEN_SCALE, ftglColour[1] );
    GOL::set_pixel_transferf( GL_BLUE_SCALE,  ftglColour[2] );
    GOL::set_pixel_transferf( GL_ALPHA_SCALE, ftglColour[3] );

    FTPoint tmp = FTFontImpl::Render( string, len, position, spacing, renderMode );

GOL::pop_client_attrib();

GOL::pop_att();
//GOL::pop_attrib();

return tmp;
}


FTPoint FTPixmapFontImpl::Render(const char * string, const int len,
                                 FTPoint position, FTPoint spacing,
                                 int renderMode)
{
    return RenderI(string, len, position, spacing, renderMode);
}


FTPoint FTPixmapFontImpl::Render(const wchar_t * string, const int len,
                                 FTPoint position, FTPoint spacing,
                                 int renderMode)
{
    return RenderI(string, len, position, spacing, renderMode);
}

