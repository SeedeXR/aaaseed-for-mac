#include "GLTTOutlineFont.h"

#if AAA_TRUETYPE1_USE()

#include "gol/gol.h"
#include "gol/gol_list.h"

#include "AAA_font_tmp.h"
#include "FTInstance.h"
#include "FTGlyphVectorizer.h"

GLTTOutlineFont::GLTTOutlineFont( AAA_FTFace* face )
{
	_face		= face;
	_instance	= nullptr;
	_font		= nullptr;
	_loaded		= nullptr;
	_list_base	= 0;
	_precision	= 4.;
}

GLTTOutlineFont::~GLTTOutlineFont()
{
	destroy();
	_face = nullptr;
}

void GLTTOutlineFont::destroy()
{
	delete[] _loaded;
	_loaded = nullptr;

	if( _list_base != 0 )
	{
		GOL::glDeleteLists( _list_base, 256 );	//hack
		_list_base = nullptr;
	}

	delete _font;
	_font = nullptr;

	delete _instance;
	_instance = nullptr;
}

void GLTTOutlineFont::set_precision( DOUBLE precision )
{
	_precision = precision;
}

bool GLTTOutlineFont::create( INT32 point_size )
{
	destroy();

	if( point_size < 1 )
	point_size= 1;

	_instance = new FTInstance(_face);

	if( ! _instance->create() )
		return false;

	INT32 resolution = 96;
	if( ! _instance->setResolutions(resolution,resolution) )
		return false;

	if( ! _instance->set_point_size(point_size) )
		return false;

//  int pixel_size= resolution * point_size / 72;

	_font = new AAA_font_tmp(_instance);

	if( ! _font->create() )
		return false;

	_list_base = GOL::gen_lists(256);
	if( _list_base == 0 )
		return false;

	_loaded = new bool [ 256 ];
	for( INT32 i= 0; i < 256; ++i )
		_loaded[i] = false;

	return true;
}

bool GLTTOutlineFont::load_glyph( INT32 i )
{
	if( i < 0 || i > 256 )
		return false;
	if( !_font || !_loaded )
		return false;

	if( _loaded[i] )
		return true;

	_loaded[i] = true;

	FTGlyphVectorizer vectorizer;
	vectorizer.setPrecision(_precision);

	INT32 list = _list_base + i;
	AAA_FTGlyph* glyph = _font->get_glyph(i);
	if( glyph == 0 )
	{
	err:
		GOL::new_list(list,GL_COMPILE);
		GOL::end_list();
		return true;
	}

	if( ! vectorizer.init(glyph) )
		goto err;

	vectorizer.vectorize();

	GOL::new_list( list, GL_COMPILE );

		for( INT32 j = 0; j < vectorizer.getNContours(); ++j )
		{
			FTGlyphVectorizer::Contour* contour = vectorizer.getContour(j);
			if( contour && contour->nPoints > 0 )
			{
				GOL::begin(GL_LINE_LOOP);
					for( INT32 k = 0; k < contour->nPoints; ++k )
					{
						FTGlyphVectorizer::POINT& p = contour->points[k];
						GOL::vertex2f( GLfloat(p.x), GLfloat(p.y) );
					}
				GOL::end();
			}
		}

		GOL::matrix::translate( GLfloat(vectorizer.getAdvance()), 0., 0. );
	GOL::end_list();

	return true;
}

void GLTTOutlineFont::load( INT32 from /* = 0 */, INT32 to /* = 255 */ )
{
	for( INT32 i= from; i <= to; ++i )
		load_glyph(i);
}

void GLTTOutlineFont::load( const char* text )
{
	if( text == 0 || _list_base == 0 )
		return;
	for(;;)
	{
		int ch = (unsigned char)*text;
		if( ch == 0 )
			break;
		++text;
		if( ! _loaded[ch] )
		load_glyph(ch);
	}
}

void GLTTOutlineFont::output( const char* text )
{
	if( text == 0 || _list_base == 0 )
		return;

	GOL::matrix::push();
	for(;;)
	{
		int ch= (unsigned char)*text;
		if( ch == 0 )
			break;
		++text;
		if( ! _loaded[ch] )
			load_glyph(ch);
		GOL::call_list( _list_base + ch );
	}

//  GOL::push_attrib( GL_LIST_BIT );
//  GOL::set_list_base(list_base);
//  GOL::call_list( strlen(text), GL_UNSIGNED_BYTE, (GLubyte*)text );
//  GOL::pop_attrib();

	GOL::matrix::pop();
}

void GLTTOutlineFont::output( CONST CHAR ch )
{
	if( _list_base == 0 || _loaded == 0 )
		return;

	if( ! _loaded[ch] )
		load_glyph(ch);

	GOL::call_list( _list_base + ch );
}

INT32 GLTTOutlineFont::get_height() CONST
{
	if( !_font )
		return 0;
	return _font->get_height();
}

INT32 GLTTOutlineFont::get_descender() CONST
{
	if( !_font )
		return 0;
	return _font->get_descender();
}

INT32 GLTTOutlineFont::get_width( C_PCHAR text )
{
	if( !_font )
		return 0;
	return _font->get_width(text);
}

#endif