
#include "AAA_FTFace.h"

#if AAA_TRUETYPE1_USE()

#include "freetype1/freetype.h"
#include "FTEngine.h"



AAA_FTFace::AAA_FTFace( FTEngine* engine /* = nullptr */ )
{
	if( !engine )
		_engine = FTEngine::getStaticEngine();
	else
		_engine = engine;

	_face = nullptr;
	_CP_table = nullptr;
	for( INT32 i= 0; i < 256; ++i )
		_indices[i]= 0;
}

AAA_FTFace::~AAA_FTFace()
{
	if( _face )
	{
		TT_Close_Face(*_face);
		delete _face;
		_face = nullptr;
	}
	_engine = nullptr;
}

bool AAA_FTFace::open( C_PCHAR_C filename )
{
	if( _face )
		return false; // already open?!
	if( !filename || *filename == 0 )
		return false;

	if( !_engine )
		return false;
	if( !_engine->getEngine() )
		return false;

	_face = new AAA_FT_FACE;
#if AAA_TRUETYPE2_USE()
	if( FT_Open_Face( *(_engine->getEngine()), nullptr, filename, _face ) )
#else
	if( TT_Open_Face( *(_engine->getEngine()), filename, _face ) )
#endif
	{
		delete _face;
		_face = nullptr;
		return false;
	}

  return makeIndicesTable();
}

INT32 AAA_FTFace::CP_Translate( INT32 code ) CONST
{
	if( !_CP_table )
		return code;
	return _CP_table[code];
}
bool AAA_FTFace::makeIndicesTable()
{
	if( !_face )
		return false;

	// First, look for a Unicode charmap
	TT_CharMap	char_map;
	int			n = TT_Get_CharMap_Count(*_face);
	int			i;

	for( i= 0; i < n; ++i )
	{
		short unsigned platform = 0;
		short unsigned encoding = 0;
		TT_Get_CharMap_ID( *_face, i, &platform, &encoding );
		if( (platform == 3 && encoding == 1 )  ||
			(platform == 0 && encoding == 0 ) )
		{
			TT_Get_CharMap( *_face, i, &char_map );
			break;
		}
	}

	if( i == n )
	{
		TT_Face_Properties  properties;
		TT_Get_Face_Properties( *_face, &properties );
		int num_glyphs = properties.num_Glyphs;

		for( int ascii_code= 0; ascii_code < 256; ++ascii_code )
		{
			//      int glyph_index= ascii_code - int(' ') + 1;
			int glyph_index= TT_Char_Index( char_map, CP_Translate(ascii_code) );
			if( glyph_index < 0 || glyph_index >= num_glyphs )
				glyph_index= 0;

			_indices[ascii_code] = glyph_index;
		}
	}
	else
	{
		for( int ascii_code= 0; ascii_code < 256; ++ascii_code )
		{
			int glyph_index= TT_Char_Index( char_map, ascii_code );
			if( glyph_index < 0 )
				glyph_index = 0;  // FIXME! default code

			_indices[ascii_code] = glyph_index;
		}
	}

	return true;
}

#endif	//#if AAA_TRUETYPE1_USE()
