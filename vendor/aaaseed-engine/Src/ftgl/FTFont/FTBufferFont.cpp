/*
 * FTGL - OpenGL font library
 *
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

#include <wchar.h>

#include "FTGL/ftgl.h"

#include "FTInternals.h"
#include "FTBufferFontImpl.h"
#include "FTGL/FTLibrary.h"

#if AAASEED()
#   include "gol/gol.h"
#   include "gol/gol_tex.h"
#   include "draw/rect.h"
#endif

//
//  FTBufferFont
//


FTBufferFont::FTBufferFont(char const * const fontFilePath) :
    FTFont(new FTBufferFontImpl(this, fontFilePath))
{}


FTBufferFont::FTBufferFont(unsigned char const * const pBufferBytes, FTGL::VECTOR_SIZE_TYPE const bufferSizeInBytes) :
    FTFont(new FTBufferFontImpl(this, pBufferBytes, bufferSizeInBytes))
{}


FTBufferFont::~FTBufferFont()
{}


FTGlyph* FTBufferFont::MakeGlyph(FT_GlyphSlot ftGlyph)
{
    FTBufferFontImpl *myimpl = dynamic_cast<FTBufferFontImpl *>(impl);
    if(!myimpl)
    {
        return NULL;
    }

    return myimpl->MakeGlyphImpl(ftGlyph);
}


//
//  FTBufferFontImpl
//
void FTBufferFontImpl::__init()
{
    load_flags = FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP;

    GOL::gen_textures( BUFFER_CACHE_SIZE, idCache );

    for( int i = 0; i < BUFFER_CACHE_SIZE; i++ )
    {
        str_cache[i].erase();
        GOL::bind_texture_2d_and_init( idCache[i] );
    }

    lastString = 0;
}


FTBufferFontImpl::FTBufferFontImpl( FTFont * const ftFont, const char* const fontFilePath ) :
    FTFontImpl(ftFont, fontFilePath),
    buffer(new FTBuffer())
{
    __init();
}


FTBufferFontImpl::FTBufferFontImpl( FTFont * const ftFont, const unsigned char * const pBufferBytes, FTGL::VECTOR_SIZE_TYPE const bufferSizeInBytes ) :
    FTFontImpl(ftFont, pBufferBytes, bufferSizeInBytes),
    buffer(new FTBuffer())
{
    __init();
}


FTBufferFontImpl::~FTBufferFontImpl()
{
    GOL::delete_textures( BUFFER_CACHE_SIZE, idCache );

    for(int i = 0; i < BUFFER_CACHE_SIZE; i++)
        str_cache[i].free_data();

    delete buffer;
}


FTGlyph* FTBufferFontImpl::MakeGlyphImpl(FT_GlyphSlot ftGlyph)
{
    return new FTBufferGlyph(ftGlyph, buffer);
}


bool FTBufferFontImpl::FaceSize(const unsigned int size,
                                const unsigned int res)
{
    for( int i = 0; i < BUFFER_CACHE_SIZE; i++ )
        str_cache[i].erase();

    return FTFontImpl::FaceSize(size, res);
}


static inline GLuint NextPowerOf2(GLuint in)
{
     in -= 1;

     in |= in >> 16;
     in |= in >> 8;
     in |= in >> 4;
     in |= in >> 2;
     in |= in >> 1;

     return in + 1;
}


inline int StringCompare(void const *a, char const *b, int len)
{
    return len < 0 ? strcmp((char const *)a, b)
                   : strncmp((char const *)a, b, len);
}


inline int StringCompare(void const *a, wchar_t const *b, int len)
{
    return len < 0 ? wcscmp((wchar_t const *)a, b)
                   : wcsncmp((wchar_t const *)a, b, len);
}


inline char *StringCopy(char const *s, int len)
{
    if(len < 0)
    {
        return strdup(s);
    }
    else
    {
#ifdef HAVE_STRNDUP
        return strndup(s, len);
#else
        char *s2 = (char*)malloc(len + 1);
        memcpy(s2, s, len);
        s2[len] = 0;
        return s2;
#endif
    }
}


inline wchar_t *StringCopy(wchar_t const *s, int len)
{
    if(len < 0)
    {
#if defined HAVE_WCSDUP
        return wcsdup(s);
#else
        len = (int)wcslen(s);
#endif
    }

    wchar_t *s2 = (wchar_t *)malloc((len + 1) * sizeof(wchar_t));
    memcpy(s2, s, len * sizeof(wchar_t));
    s2[len] = 0;
    return s2;
}

namespace GOL {
	extern	void	set_swizzle_for_gl_alpha_mode( GLenum CONST target );
	extern	void	set_swizzle_for_gl_red_mode( GLenum CONST target );
}	//namespace GOL

template <typename T>
inline FTPoint FTBufferFontImpl::RenderI( const T* const string, int const len,
                                         FTPoint position, FTPoint spacing,
                                         int renderMode)
{
	const float padding = 3.0f;
	int width, height, texWidth, texHeight;
	int cacheIndex = -1;
	bool inCache = false;

    // Protect blending functions, GL_TEXTURE_2D and optionally GL_BLEND
	//   GOL::push_attrib(GL_COLOR_BUFFER_BIT | GL_ENABLE_BIT | GL_TEXTURE_BIT);

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

    GOL::set_texture_2D();

    auto env_mode_pushed = GOL::get_tex_env_mode();
    if( env_mode_pushed != GL_MODULATE )
        GOL::set_tex_env_mode( GL_MODULATE );
    else
        env_mode_pushed = 0;

		// Search whether the string is already in a texture we uploaded
		for(int n = 0; n < BUFFER_CACHE_SIZE; n++)
		{
			int i = (lastString + n + BUFFER_CACHE_SIZE) % BUFFER_CACHE_SIZE;
			if( str_cache[i].is_str_equal( C_PCHAR_C(string), len) )
			{
				cacheIndex = i;
				inCache = true;
				break;
			}
		}

		// If the string was not found, we need to put it in the cache and compute
		// its new bounding box.
		if( !inCache )
		{
			// FIXME: this cache is not very efficient. We should first expire
			// strings that are not used very often.
			cacheIndex = lastString;
			lastString = (lastString + 1) % BUFFER_CACHE_SIZE;

			// FIXME: only the first N bytes are copied; we want the first N chars.
			str_cache[cacheIndex].set( C_PCHAR_C(string), len );
			bboxCache[cacheIndex] = BBox(string, len, FTPoint(), spacing);
		}

		FTBBox bbox = bboxCache[cacheIndex];

		width = static_cast<int>(bbox.Upper().X() - bbox.Lower().X()
								  + padding + padding + 0.5);
		height = static_cast<int>(bbox.Upper().Y() - bbox.Lower().Y()
								   + padding + padding + 0.5);

		texWidth = NextPowerOf2(width);
		texHeight = NextPowerOf2(height);

		FP32 width_over_1  = OVER_ONE_AS_FP32( texWidth );
		FP32 height_over_1 = OVER_ONE_AS_FP32( texHeight );

		// If the string was not found, we need to render the text in a new
		// texture buffer, then upload it to the OpenGL layer.
		if( !inCache )
		{
			buffer->Size(texWidth, texHeight);
			buffer->Pos(FTPoint(padding, padding) - bbox.Lower());

			advanceCache[cacheIndex] = FTFontImpl::Render( string, len, FTPoint(), spacing, renderMode );

			GOL::bind_texture_2d( idCache[cacheIndex] );
        
			// Protect glPixelStorei() calls
			GOL::push_client_attrib(GL_CLIENT_PIXEL_STORE_BIT);

				GOL::set_pixel_storei(GL_UNPACK_LSB_FIRST, GL_FALSE);
				GOL::set_pixel_storei(GL_UNPACK_ROW_LENGTH, 0);
				GOL::set_pixel_storei(GL_UNPACK_ALIGNMENT, 1);

				/* TODO: use glTexSubImage2D later? */
	#if 1
				GOL::tex_image_2d( 0, GL_ALPHA, texWidth, texHeight,
									GL_ALPHA, GL_UNSIGNED_BYTE, (GLvoid *)buffer->Pixels()  );
				// next line not needed but maa don't understand why 
				GOL::set_swizzle_for_gl_alpha_mode( GL_TEXTURE_2D );
	#else
				GOL::tex_image_2d( 0, GL_R8, texWidth, texHeight,
									GL_RED, GL_UNSIGNED_BYTE, (GLvoid *)buffer->Pixels()  );
				GOL::set_swizzle_for_gl_red_mode( GL_TEXTURE_2D );
	#endif


			GOL::pop_client_attrib();

			buffer->Size(0, 0);
		}

		GOL::bind_texture_2d( idCache[cacheIndex] );
		//GOL::set_tex_2d_magnification( GL_LINEAR);
		GOL::set_tex_2d_minification( GL_LINEAR);

		FTPoint low = position + bbox.Lower() - FTPoint(padding, padding);
		FTPoint up = position + bbox.Upper() + FTPoint(padding, padding);

		GOL::normal3v( unit_z_v4fp32 );

		_uv[2] = _uv[0] = 0.;
		_uv[7] = _uv[3] = 1.;
		_uv[5] = _uv[1] = height_over_1 * (texHeight - height);
		_uv[6] = _uv[4] = width_over_1 * width;

		_vertex[3]  = _vertex[0] = low.Xf();
		_vertex[7] = _vertex[1] = up.Yf();
		_vertex[6]  = _vertex[9] = up.Xf();
		_vertex[10]  = _vertex[4] = low.Yf();
		_vertex[11] = _vertex[8] = _vertex[5] = _vertex[2] = position.Zf();

		draw_triangle_strip_4xyz_uv( _vertex, _uv );
		

    if( env_mode_pushed )
        GOL::set_tex_env_mode( env_mode_pushed );

    //GOL::pop_attrib();

    return position + advanceCache[cacheIndex];
}


FTPoint FTBufferFontImpl::Render(const char * string, const int len,
                                 FTPoint position, FTPoint spacing,
                                 int renderMode)
{
    return RenderI(string, len, position, spacing, renderMode);
}


FTPoint FTBufferFontImpl::Render(const wchar_t * string, const int len,
                                 FTPoint position, FTPoint spacing,
                                 int renderMode)
{
    return RenderI(string, len, position, spacing, renderMode);
}

