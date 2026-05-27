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

#include <string>

#include "FTGL/ftgl.h"

#include "FTInternals.h"
#include "FTBitmapGlyphImpl.h"
#include "gol/gol.h"
#include "gol/gol_tex.h"

//
//  FTGLBitmapGlyph
//


FTBitmapGlyph::FTBitmapGlyph(FT_GlyphSlot glyph) :
    FTGlyph(new FTBitmapGlyphImpl(glyph))
{}


FTBitmapGlyph::~FTBitmapGlyph()
{}


const FTPoint& FTBitmapGlyph::Render(const FTPoint& pen, int renderMode)
{
    FTBitmapGlyphImpl *myimpl = dynamic_cast<FTBitmapGlyphImpl *>(impl);
    return myimpl->RenderImpl(pen, renderMode);
}


//
//  FTGLBitmapGlyphImpl
//


FTBitmapGlyphImpl::FTBitmapGlyphImpl(FT_GlyphSlot glyph)
:   FTGlyphImpl(glyph),
    _dst_width(0),
    _dst_height(0),
    _data(0)
{
    err = FT_Render_Glyph(glyph, FT_RENDER_MODE_MONO);
    if(err || ft_glyph_format_bitmap != glyph->format)
    {
        return;
    }

    FT_Bitmap bitmap = glyph->bitmap;

    unsigned int src_width = bitmap.width;
    unsigned int src_height = bitmap.rows;
    unsigned int src_pitch = bitmap.pitch;

    _dst_width = src_width;
    _dst_height = src_height;
    _dst_pitch = src_pitch;

    if(_dst_width && _dst_height)
    {
        _data = new unsigned char[_dst_pitch * _dst_height];
        unsigned char* dest = _data + ((_dst_height - 1) * _dst_pitch);

        unsigned char* src = bitmap.buffer;

        for(unsigned int y = 0; y < src_height; ++y)
        {
            memcpy(dest, src, src_pitch);
            dest -= _dst_pitch;
            src += src_pitch;
        }
    }

    _pos = FTPoint(glyph->bitmap_left, static_cast<int>(src_height) - glyph->bitmap_top, 0.0);
}


FTBitmapGlyphImpl::~FTBitmapGlyphImpl()
{
    delete [] _data;
}


const FTPoint& FTBitmapGlyphImpl::RenderImpl(const FTPoint& pen, int renderMode)
{
    (void)renderMode;

    if(_data)
    {
        FP32 x = pen.Xf() + _pos.Xf();
        FP32 y = pen.Yf() - _pos.Yf();

		//todo use GOL
        GOL::bitmap(0, 0, 0.0f, 0.0f, x,y, (const GLubyte*)0);
        GOL::set_pixel_storei(GL_UNPACK_ROW_LENGTH, _dst_pitch * 8);
        GOL::bitmap(_dst_width, _dst_height, 0.f, 0.f, 0.f, 0.f, (const GLubyte*)_data );
        GOL::bitmap(0, 0, 0.f, 0.f, -x, -y, (const GLubyte*)0);
    }

    return advance;
}

