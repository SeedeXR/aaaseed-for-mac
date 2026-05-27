#include "GLTTFont.h"

#if AAA_TRUETYPE1_USE()

#include <stdio.h>
#include "gol/gol.h"
#include "gol/gol_list.h"

#include "AAA_font_tmp.h"
#include "FTInstance.h"
#include "AAA_FTGlyph.h"
#include "FTGlyphVectorizer.h"

#include "GLTTGlyphPolygonizer.h"


GLTTFont::GLTTFont( AAA_FTFace* face )
{
	_face		= face;
	_instance	= nullptr;
	_font		= nullptr;
	_b_loaded	= nullptr;
	_list_base	= 0;
	_precision	= 8.;
}

GLTTFont::~GLTTFont()
{
	destroy();
	_face = nullptr;
}

void GLTTFont::destroy()
{
	delete[] _b_loaded;
	_b_loaded = nullptr;

	if( _list_base != 0 )
	{
		GOL::delete_lists( _list_base, 256 );	//hack
		_list_base = 0;
	}

	delete _font;
	_font = nullptr;

	delete _instance;
	_instance = nullptr;
}

void GLTTFont::set_precision( DOUBLE precision )
{
	_precision = precision;
}

bool GLTTFont::create( INT32 point_size )
{
	destroy();

	if( point_size < 1 )
		point_size = 1;

	_instance = new FTInstance(_face);

	if( ! _instance->create() )
		return false;

	INT32 resolution = 96;
	if( !_instance->setResolutions(resolution,resolution) )
		return false;

	if( !_instance->set_point_size(point_size) )
		return false;

	_font = new AAA_font_tmp(_instance);

	if( !_font->create() )
		return false;

	_list_base = GOL::gen_lists(256);
	if( _list_base == 0 )
		return false;

	_b_loaded= new bool [ 256 ];
	for( int i= 0; i < 256; ++i )
		_b_loaded[i] = false;

	return true;
}

bool GLTTFont::load_glyph( int i )
{
	if( i < 0 || i > 256 )
		return false;

	if( _list_base == 0 || !_b_loaded )
		return false;

	if( _b_loaded[i] )
		return true;

	_b_loaded[i] = true;

	GLTTGlyphPolygonizer polygonizer;

	polygonizer.setPrecision(_precision);

	int list = _list_base + i;
	AAA_FTGlyph* glyph= _font->get_glyph(i);

	if( glyph == 0 )
	{
		err:
		GOL::new_list( list, GL_COMPILE );
		GOL::end_list();
		return true;
	}

	if( !polygonizer.init(glyph) )
		goto err;

	GOL::new_list( list, GL_COMPILE );

		polygonizer.polygonize();
		GOL::matrix::translate( GLfloat(polygonizer.getAdvance()), 0., 0. );

	GOL::end_list();

	return true;
}

void GLTTFont::load( INT32 from /* = 0 */, INT32 to /* = 255 */ )
{
	for( INT32 i= from; i <= to; ++i )
		load_glyph(i);
}

void GLTTFont::load( C_PCHAR text )
{
	if( !text || !_list_base )
		return;

	for(;;)
	{
		int ch = (unsigned char)*text;
		if( ch == 0 )
			break;
		++text;

		if( ! _b_loaded[ch] )
			load_glyph(ch);
	}
}

void GLTTFont::output( C_PCHAR text )
{
	if( !text || _list_base == 0 || !_b_loaded )
		return;

	GOL::matrix::push();
		for(;;)
		{
			int ch= (unsigned char)*text;
			if( ch == 0 )
				break;
			++text;

			if( ! _b_loaded[ch] )
				load_glyph(ch);

			GOL::call_list( _list_base + ch );
		}

//	GOL::push_attrib( GL_LIST_BIT );
//	GOL::set_list_base(list_base);
//	GOL::call_list( strlen(text), GL_UNSIGNED_BYTE, (GLubyte*)text );
//	GOL::pop_attrib();

	GOL::matrix::pop();
}

void GLTTFont::output( CONST CHAR ch )
{
	if( _list_base == 0 || !_b_loaded )
		return;

	if( ! _b_loaded[ch] )
		load_glyph(ch);

	GOL::call_list( _list_base + ch );
}

INT32 GLTTFont::get_height() CONST
{
	if( !_font )
		return 0;
	return _font->get_height();
}

INT32 GLTTFont::get_descender() CONST
{
	if( !_font )
		return 0;
	return _font->get_descender();
}

INT32 GLTTFont::get_width( C_PCHAR text )
{
	if( !_font )
		return 0;
	return _font->get_width(text);
}

void GLTTFont::get_bbox( C_PCHAR text, INT32& llx, INT32& lly, INT32& urx, INT32& ury ) CONST
{
	llx = lly = urx = ury = 0;
	if( !_font )
		return;
	_font->get_bbox( text, llx, lly, urx, ury );
}

#endif //#if AAA_TRUETYPE1_USE()
