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

#include <cassert>
#include <string> // For memset

#include "FTGL/ftgl.h"

#include "FTInternals.h"

#include "../FTGlyph/FTTextureGlyphImpl.h"
#include "./FTTextureFontImpl.h"

#if AAASEED()
#   include "gol/gol.h"
#   include "gol/gol_tex.h"
//#	include "image/bind_img.h"
#	include "image/bind_img_2d.h"
#	include "draw/tex.h"
//#	include "math/aaa_math.h"
#else
#	include "FTGL/FTLibrary.h"
#endif

//
//  FTTextureFont
//


FTTextureFont::FTTextureFont(char const * const fontFilePath) :
    FTFont(new FTTextureFontImpl(this, fontFilePath))
{}


FTTextureFont::FTTextureFont( const unsigned char * const pBufferBytes, FTGL::VECTOR_SIZE_TYPE const bufferSizeInBytes ) :
    FTFont(new FTTextureFontImpl(this, pBufferBytes, bufferSizeInBytes))
{}


FTTextureFont::~FTTextureFont()
{}


FTGlyph* FTTextureFont::MakeGlyph(FT_GlyphSlot ftGlyph)
{
    FTTextureFontImpl *myimpl = dynamic_cast<FTTextureFontImpl *>(impl);
    if(!myimpl)
    {
        return NULL;
    }

    return myimpl->MakeGlyphImpl(ftGlyph);
}


//
//  FTTextureFontImpl
//


static inline GLuint ClampSize(GLuint in, GLuint maxTextureSize)
{
    // Find next power of two
    --in;
    in |= in >> 16;
    in |= in >> 8;
    in |= in >> 4;
    in |= in >> 2;
    in |= in >> 1;
    ++in;

    // Clamp to max texture size
    return in < maxTextureSize ? in : maxTextureSize;
}

void FTTextureFontImpl::__init()
{
	_texture_size_max	= 0;
	_texture_width		= 0;
	_texture_height		= 0;
	_glyph_width		= 0;
	_glyph_height		= 0;
	_padding			= 3;
	_offset_x			= 0;
	_offset_y			= 0;

	load_flags = FT_LOAD_NO_HINTING | FT_LOAD_NO_BITMAP;
	//_glyph_rem =
	_glyph_nb = face.GlyphCount();
}

FTTextureFontImpl::FTTextureFontImpl( FTFont * const ftFont, const char* const fontFilePath )
:   FTFontImpl( ftFont, fontFilePath )
{
    __init();
}


FTTextureFontImpl::FTTextureFontImpl( FTFont * const ftFont, const unsigned char * const pBufferBytes, FTGL::VECTOR_SIZE_TYPE const bufferSizeInBytes )
:   FTFontImpl(ftFont, pBufferBytes, bufferSizeInBytes)
{
    __init();
}


FTTextureFontImpl::~FTTextureFontImpl()
{
	release_textures();
}


FTGlyph* FTTextureFontImpl::MakeGlyphImpl(FT_GlyphSlot ftGlyph)
{
	_glyph_width  = static_cast<int>(charSize.Width() + 0.5f);
	if( _glyph_width < 1 )
		_glyph_width = 1;

	_glyph_height = static_cast<int>(charSize.Height() + 0.5f);
	if( _glyph_height < 1 )
		_glyph_height = 1;

    if( _tex_ids.empty() )
    {
        _tex_ids.push_back( create_texture_one() );
        _offset_x = _offset_y = _padding;
    }

    if( _offset_x > (_texture_width - _glyph_width))
    {
        _offset_x = _padding;
        _offset_y += _glyph_height;

        if( _offset_y > (_texture_height - _glyph_height))
        {
            _tex_ids.push_back( create_texture_one() );
            _offset_y = _padding;
        }
    }

    FTTextureGlyph* tempGlyph = new FTTextureGlyph(ftGlyph, tex2d.get_name_gl( _tex_ids[_tex_ids.size() - 1] ),
                                                    _offset_x,_offset_y, _texture_width,_texture_height);
    _offset_x += static_cast<int>(tempGlyph->BBox().Upper().X() - tempGlyph->BBox().Lower().X() + _padding + 0.5);

//	--_glyph_rem;

    return tempGlyph;
}

void FTTextureFontImpl::compute_texture_size()
{
    if( _texture_size_max==0 )
    {
#if 1
		//maa force it here to avoid texture with 32K width wasting space
		//texture_size_max = MIN( POW2_EQUAL_OR_SUP( MAX( glyphWidth, glyphHeight ) * 4), GOL::tex_size_max );	
		_texture_size_max = MIN( POW2_EQUAL_OR_SUP( _glyph_height * 5 ), GOL::tex_size_max );	
#else
        _texture_size_max = GOL::tex_size_max;	
        //glGetIntegerv( GL_MAX_TEXTURE_SIZE, (GLint*)&texture_size_max );
        //assert(texture_size_max); // Indicates an invalid OpenGL context
#endif
    }

    // Texture width required for numGlyphs glyphs. Will probably not be
    // large enough, but we try to fit as many glyphs in one line as possible
    _texture_width = ClampSize( _glyph_width * _glyph_nb + _padding * 2, _texture_size_max );

    // Number of lines required for that many glyphs in a line
    int tmp = MAX( (_texture_width - _padding * 2) / _glyph_width, 1 );
    tmp = (_glyph_nb + (tmp - 1) ) / tmp; // round division up

    // Texture height required for tmp lines of glyphs
    _texture_height = ClampSize( _glyph_height * tmp + _padding * 2, _texture_size_max );
}

void FTTextureFontImpl::release_textures()
{
	 //todoft       GOL::delete_textures( (GLsizei)textureIDList.size(), (GLuint* const)&textureIDList[0] );
	for( INT32 CONST id : _tex_ids )
		tex2d.release( id );
	_tex_ids.clear();
}


namespace GOL {
	extern	void	set_swizzle_for_gl_alpha_mode( GLenum CONST target );
}	//namespace GOL

INT32 FTTextureFontImpl::create_texture_one()
{
    compute_texture_size();

    //int size = texture_width * texture_height;
	// 2025 December maa change it yo use AAAstuff
	//UINT8* texture_mem = (UINT8*) MALLOC_ALIGNED( size );
    //MEMCLEAR( texture_mem, size );
    //GLuint textID;
    //GOL::gen_texture( &textID );

	INT32 tex_id = tex2d.acquire();

#if 1
	// we need to change the img or img will take over tex later
	c_img_2d* img = g_bind_img_2d->get_always( tex_id );
	img->init_with_size_no_cpu_mem( _texture_width,_texture_height, aaa::PIXEL_FORMAT::R_8, __FUNCTION__ );
	img->set_gpu_move( false );
#endif

	tex2d.bind( tex_id );
	
	GOL::set_tex_2d_wrap_s( GL_CLAMP );
	GOL::set_tex_2d_wrap_t( GL_CLAMP );
	GOL::set_tex_2d_min_mag( GL_LINEAR, GL_LINEAR );

	//GOL::tex_image_2d(   0, GL_INTERNAL_TYPE_USED, texture_width,texture_height,	GL_RED, GL_UNSIGNED_BYTE, nullptr );
	//tex2d.image or tex2d.image_level version fuck it up because it use GL_R8 as internal format
	//we would like GL_RED or GL_R8 for internal format but we do not work for text even with the right swizzle
	//maa tried even with swizzle set bu could not solve it probably the default shader and/or tex dim state
	tex2d.image_level_gl_internal_format(	0, _texture_width,_texture_height, GL_ALPHA, 1, GOL::INTERNAL_TYPE::UINT_8,
											nullptr, GL_RED, GL_UNSIGNED_BYTE, false, false );
	GOL::set_swizzle_for_gl_alpha_mode( GL_TEXTURE_2D );

	// 2025 December maa change it to use AAAstuff
	//FREE_ALIGNED( texture_mem );

    return tex_id;
}


bool FTTextureFontImpl::FaceSize( const unsigned int size, const unsigned int res )
{
    if( !_tex_ids.empty() )
    {
		release_textures();
        //_glyph_rem =
		_glyph_nb = face.GlyphCount();
    }

    return FTFontImpl::FaceSize(size, res);
}


template <typename T>
inline FTPoint FTTextureFontImpl::RenderI(const T* string, const int len,
                                          FTPoint position, FTPoint spacing,
                                          int renderMode)
{
	// Protect GL_TEXTURE_2D and optionally GL_BLEND
	//GOL::push_attrib( GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT );

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
    //maatmp  GOL::set_texture_2D();
    GOL::set_texture_2D_private();
	//maatmp    GOL::set_texture_2D();
	auto env_mode_pushed = GOL::get_tex_env_mode();
	if( env_mode_pushed != GL_MODULATE )
		GOL::set_tex_env_mode( GL_MODULATE );
	else
		env_mode_pushed = 0;

		FTTextureGlyphImpl::ResetActiveTexture();

		//GOL::set_blend_func( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );
		//GOL::set_tex_env_mode( GL_MODULATE );
		FTPoint tmp = FTFontImpl::Render( string, len, position, spacing, renderMode );

	if( env_mode_pushed )
		GOL::set_tex_env_mode( env_mode_pushed );
	
	//  GOL::pop_attrib();

	return tmp;
}


FTPoint FTTextureFontImpl::Render(const char * string, const int len,
                                  FTPoint position, FTPoint spacing,
                                  int renderMode)
{
    return RenderI(string, len, position, spacing, renderMode);
}


FTPoint FTTextureFontImpl::Render(const wchar_t * string, const int len,
                                  FTPoint position, FTPoint spacing,
                                  int renderMode)
{
    return RenderI(string, len, position, spacing, renderMode);
}

