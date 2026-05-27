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

#include <math.h>

#include "FTGL/ftgl.h"

#include "FTInternals.h"
#include "FTTextureGlyphImpl.h"

#if AAASEED()
#   include "gol/gol.h"
#   include "gol/gol_tex.h"
#   include "draw/rect.h"
#endif

#define FTGL_ASSERTS_SHOULD_SOFT_FAIL

#ifdef FTGL_ASSERTS_SHOULD_SOFT_FAIL
#   define FTASSERT_FAIL do {} while(0)
#else
#   define FTASSERT_FAIL do { int *a = (int*)0x0; *a = 0xD15EA5ED; } while(0)
#endif

#define FTASSERT(x) \
    if (!(x)) \
    { \
        static int count = 0; \
        if (count++ < 8) \
            fprintf(stderr, "ASSERTION FAILED (%s:%d): %s\n", \
                    __FILE__, __LINE__, #x); \
        FTASSERT_FAIL; \
        if (count == 8) \
            fprintf(stderr, "\\__ last warning for this assertion\n"); \
    }


//
//  FTGLTextureGlyph
//

FTTextureGlyph::FTTextureGlyph(FT_GlyphSlot glyph, int id, int xOffset,
                               int yOffset, int width, int height) :
    FTGlyph(new FTTextureGlyphImpl(glyph, id, xOffset, yOffset, width, height))
{}


FTTextureGlyph::~FTTextureGlyph()
{}


const FTPoint& FTTextureGlyph::Render(const FTPoint& pen, int renderMode)
{
    FTTextureGlyphImpl *myimpl = dynamic_cast<FTTextureGlyphImpl *>(impl);
    return myimpl->RenderImpl(pen, renderMode);
}


//
//  FTGLTextureGlyphImpl
//

//maa
GLint FTTextureGlyphImpl::name_gl_active = 0;

FTTextureGlyphImpl::FTTextureGlyphImpl( FT_GlyphSlot glyph, int id, int x_offset, int y_offset, int width, int height )
:   FTGlyphImpl	(glyph),
    _dst_width	(0),
    _dst_height	(0),
    _name_gl	(id)
{
    /* FIXME: need to propagate the render mode all the way down to
     * here in order to get FT_RENDER_MODE_MONO aliased fonts.
     */

    err = FT_Render_Glyph( glyph, FT_RENDER_MODE_NORMAL );
    if( err || glyph->format != ft_glyph_format_bitmap )
    {
        return;
    }

    FT_Bitmap      bitmap = glyph->bitmap;

    _dst_width  = bitmap.width;
    _dst_height = bitmap.rows;


    if( _dst_width && _dst_height )
    {
        GOL::bind_texture_2d( _name_gl );

        GOL::push_client_attrib(GL_CLIENT_PIXEL_STORE_BIT);

		//todofont use GOL
       GOL::set_pixel_storei(GL_UNPACK_LSB_FIRST, GL_FALSE);
       GOL::set_pixel_storei(GL_UNPACK_ROW_LENGTH, 0);
       GOL::set_pixel_storei(GL_UNPACK_ALIGNMENT, 1);

        GLint w,h;
		//todofont this get is bad
        glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_WIDTH, &w );
        glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_HEIGHT, &h );

        FTASSERT(x_offset >= 0);
        FTASSERT(y_offset >= 0);
        FTASSERT(_dst_width >= 0);
        FTASSERT(_dst_height >= 0);
        FTASSERT(x_offset + _dst_width <= w);
        FTASSERT(y_offset + _dst_height <= h);

        if( y_offset + _dst_height > h )
        {
            // We'll only get here if we are soft-failing our asserts. In that
            // case, since the data we're trying to put into our texture is
            // too long, we'll only copy a portion of the image.
            _dst_height = h - y_offset;
        }
        if( _dst_height >= 0 )
        {
            // convert bitmap fonts
            std::vector <unsigned char> data_converted;
            if(bitmap.num_grays == 1)
            {
                bBox = FTBBox( 0, 0, 0, float(_dst_width), float(_dst_height), 0 );
                data_converted.resize(_dst_width * _dst_height, 0);
                int n = 0;
                for( int y = 0; y < _dst_height; ++y )
                {
                    unsigned char* src = bitmap.pitch < 0
                      ? bitmap.buffer + (y - _dst_height + 1) * bitmap.pitch
                      : bitmap.buffer + y * bitmap.pitch;
                    unsigned char c = 0;
                    for(int x = 0; x < _dst_width; ++x)
                    {
                        if (x % 8 == 0)
                          c = *src++;
                        data_converted[n++] = ((c >> (7 - (x % 8))) & 1) * 255;
                    }
                }
            }

#if 1
			tex2d.sub_image(	 	0, x_offset,y_offset,	 _dst_width,_dst_height,
									1, GL_ALPHA, GL_UNSIGNED_BYTE, !data_converted.empty() ? data_converted.data() : bitmap.buffer, false );
#else								
            GOL::tex_sub_image_2d(	0, x_offset y_offset,	_dst_width, _dst_height,
									   GL_ALPHA, GL_UNSIGNED_BYTE, !data_converted.empty() ? data_converted.data() : bitmap.buffer );
#endif  
        }

        GOL::pop_client_attrib();
    }

  
//  23---67 drawn with triangle_strip
//  |    |
//  |    |
//  |    |
//  01---45


    _uv[2] = _uv[0] = static_cast<float>(x_offset) / static_cast<float>(width);
    _uv[5] = _uv[1] = static_cast<float>(y_offset) / static_cast<float>(height);
    _uv[6] = _uv[4] = static_cast<float>(x_offset + _dst_width) / static_cast<float>(width);
    _uv[7] = _uv[3] = static_cast<float>(y_offset + _dst_height) / static_cast<float>(height);

    _corner = FTPoint(glyph->bitmap_left, glyph->bitmap_top);
}


FTTextureGlyphImpl::~FTTextureGlyphImpl()
{}


const FTPoint& FTTextureGlyphImpl::RenderImpl( const FTPoint& pen,  int /*renderMode*/ )
{
	if( name_gl_active != _name_gl )
    {
        GOL::bind_texture_2d( _name_gl );
		//GOL::set_tex_2d_magnification( GL_LINEAR );	//todotex remove
		//this seems to be needed
		GOL::set_tex_2d_minification( GL_LINEAR );
        name_gl_active = _name_gl;
    }

    FP32 x = floor( pen.Xf() + _corner.Xf() );
    FP32 y = floor( pen.Yf() + _corner.Yf() );
    _vertex[3]  = _vertex[0] = x;
    _vertex[7] = _vertex[1] = y;
    _vertex[6]  = _vertex[9] = x + _dst_width;
    _vertex[10]  = _vertex[4] = y - _dst_height;
    _vertex[11] = _vertex[8] = _vertex[5] = _vertex[2] = pen.Zf();

    draw_triangle_strip_4xyz_uv( _vertex, _uv );

    return advance;
}

