#include "aaa_def.h"
#ifndef AAA_OURTRUETYPE_H
#	include "truetype/OurTrueType.h"
#endif
#ifndef AAA_AAA_FTGL_H
#	include "aaa_ftgl.h"
#endif
#include <stdlib.h>
#include "infrastructure/bind/bind.h"
#include "obj_ui/aaarect.h"
#include "gol/gol.h"

#include <unordered_map>
#include "id_unique.h"


#if	!AAA_WATCHDOG()

#	if	AAA_VSTOOL() >= 143
//todo check if it is better recompiling freetype with v145 toolset
#		define AAA_LOCAL_LIB_VSTOOL_STR()  "v143"
#	else
#		define AAA_LOCAL_LIB_VSTOOL_STR()  AAA_LIB_VSTOOL_STR()
#	endif	//AAA_VSTOOL() >= 143

#	include <lib_use.h>
	//AAA_LIB_USE( "FTGL/ftgl_"AAA_LIB_USE_PLATFORM_SHORT()"_"AAA_LIB_VSTOOL_STR()"_"AAA_LIB_USE_CONFIGURATION() )
	//ftgl require freetype
#	if AAA_DEBUG()
		AAA_LIB_USE( "FreeType2/freetype28MTd_"AAA_LOCAL_LIB_VSTOOL_STR() )
#	else
		AAA_LIB_USE( "FreeType2/freetype28MT_"AAA_LOCAL_LIB_VSTOOL_STR() )
#	endif
#endif //AAA_WATCHDOG

namespace {
	// map for fonts
	typedef	std::unordered_map< UINT32, c_ftfont* > FONT_MAP;
	FONT_MAP			_fonts_map		[ aaa::font::FONT_MAX_NB ];
	id_unique::c_u32	_fonts_id_unique[ aaa::font::FONT_MAX_NB ];
}

o_str c_ftfont::ftgl_version;

void c_ftfont::c_init()
{
	//for( INT32 i = 0; i<FONT_MAX_NB; ++i )
	//	_fonts_id_unique[ i ].change();
	ftgl_version.set( FTGL::GetString( FTGL::CONFIG_VERSION ) );
#if !AAASEED()
	FTLibrary::Instance().LegacyOpenGLState(false);
#endif
}
void c_ftfont::c_deinit()
{  
	for( INT32 i = 0; i<aaa::font::FONT_MAX_NB; ++i )
		erase( i );
}

void c_ftfont::erase( UINT32 CONST s_font )
{
	FONT_MAP& map = _fonts_map[ s_font ];
	for( auto const & pt : map )
		delete pt.second;
	_fonts_id_unique[ s_font ].change();
	map.clear();
}


UINT32	c_ftfont::get_unique_id( UINT32 CONST s_font )
{
	return _fonts_id_unique[ s_font ].get();
}

c_ftfont*	c_ftfont::get( UINT32 CONST s_font, UINT32 CONST type, UINT32 CONST face_size, UINT32 CONST face_dpi )
{
	FONT_MAP& map = _fonts_map[ s_font ];
	//if( !map.empty() )
	//	return nullptr;
	// type 4 bits, dpi 13 bits, size 13 bits
	UINT32 key = ( (type & 0xf) << 26) | ( (face_dpi & 0x1fff) << 13 ) | ( face_size & 0x1fff ) ;

	FONT_MAP::iterator	it;
	it = map.find( key );
	if( it != map.end() )
		return it->second;

	c_ftfont* ftfont = new c_ftfont( s_font, type, face_size, face_dpi );
	if( ftfont && !ftfont->get_font() )
		SAFE_DELETE( ftfont );
	
	if( ftfont )
		map[ key ] = ftfont;
	return ftfont;
}

FTFont*	c_ftfont::alloc_font( C_PCHAR_C font_name, UINT32 CONST type )
{
	FTFont* font = nullptr;
	if( font_name == nullptr )
		err_print( "FTGL : Can't open font with null pointer name" );
	else if( *font_name == 0 )
		err_print( "FTGL : Can't open font with empty name");
	else
	{	
		switch( type )
		{
		case FTGL_BITMAP:	font = new FTBitmapFont(  font_name ); break;
		case FTGL_PIXMAP:	font = new FTPixmapFont(  font_name ); break;
		case FTGL_OUTLINE:	font = new FTOutlineFont( font_name ); break;
		case FTGL_POLYGON:	font = new FTPolygonFont( font_name ); break;
		case FTGL_EXTRUDE:	font = new FTExtrudeFont( font_name ); break;
		case FTGL_TEXTURE:	font = new FTTextureFont( font_name ); break;
		case FTGL_BUFFER:	font = new FTBufferFont(  font_name ); break;
		default:			err_print( "%s() This font type unimplemented : %d", __FUNCTION__, type );
							break;
		}
		if( font && font->Error() )
		{
			err_print( "FTGL : Failed to open font \"%s\"", font_name );
			SAFE_DELETE( font );
		}
	//	font->CharMap( FT_ENCODING_UNICODE );
	}
	return font;
}

void	c_ftfont::dealloc( )
{
	SAFE_DELETE( _ft_font );
}

void	c_ftfont::alloc(  UINT32 CONST s_font, UINT32 CONST type, UINT32 CONST face_size, UINT32 CONST face_dpi )
{
	_ft_font = alloc_font( aaa::font::g_bank_def->get_str( s_font ), type );
	if( _ft_font )
	{
		_s_font = s_font;
		_type = type;
		_face_size = face_size;
		_face_dpi = face_dpi;

		set_depth( 1 );
		set_outset( 0 );
		_b_use_display_list = true;
		set_use_display_list( false );
	}
	_ft_layout.SetFont( _ft_font );
	_b_need_glyph_build = true;
	update();
}

c_ftfont::c_ftfont( UINT32 CONST s_font, UINT32 CONST type, UINT32 CONST face_size, UINT32 CONST face_dpi )
	: _ft_font				( nullptr )
	,_s_font				( -42 )
	,_type					( -42 )
	,_face_size				( 0 )
	,_face_dpi				( 0 )
	,_depth					( -42 )
	,_outset				( -42 )
	,_b_use_display_list	( false )
	,_b_need_glyph_build	( true )
{
	alloc( s_font, type, face_size, face_dpi );
}

c_ftfont::~c_ftfont()
{
	dealloc();
}

void	c_ftfont::set_depth( FP32 CONST depth )
{
	if( _depth != depth )
	{
		if( _ft_font )
		{
			_depth = depth;		
			_b_need_glyph_build = true;
		}
	}
}
void	c_ftfont::set_outset( FP32 CONST outset )
{
	if( _outset != outset )
	{
		_outset = outset;			
		_b_need_glyph_build = true;
	}
}
void	c_ftfont::update()
{
	if( _b_need_glyph_build )
	{
		if( _ft_font )
		{
			_ft_font->Depth( _depth * _face_size );
			_ft_font->Outset( _outset * _face_size * FP32(.2) );	// .2 is experimental to have [-1,1] on outset give decent parametrisation
			_ft_font->FaceSize( _face_size, _face_dpi );	//to trigger Glyph rebuild
			_line_height_over_one = OVER_ONE_AS_FP32( _ft_font->LineHeight()	);
			//_face_size_over_one	  = OVER_ONE( _ft_font->Ascender() + _font->Descender() );
			_ascender_over_one	  = OVER_ONE_AS_FP32( _ft_font->Ascender() );
		}
		_b_need_glyph_build = false;
	}
}