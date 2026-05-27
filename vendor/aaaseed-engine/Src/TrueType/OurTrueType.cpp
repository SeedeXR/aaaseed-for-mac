
#include "truetype/ourtruetype.h"

#include "infrastructure/bind/bind.h"

#include "obj_ui/aaarect.h"
#include "gol/gol_matrix.h"
#include "ftgl/aaa_ftgl.h"
#include "SdfText/SdfFont.h"

#if AAA_TRUETYPE1_USE()
#	include "TrueType/GLTTOutlineFont.h"
#	include "TrueType/GLTTFont.h"
#	include "TrueType/AAA_FTFace.h"
#	include <stdlib.h>

// truetype 2 is needed by FTGL but it dealt within aaa_ftgl
#	if AAA_TRUETYPE2_USE()
#		include <lib_use.h>
//		AAA_LIB_USE_D( "freetype28MT_"AAA_LIB_VSTOOL_STR() )
//		AAA_LIB_USE( "freetype28MT_"AAA_LIB_VSTOOL_STR()"_d" )
#		if AAA_DEBUG()
			AAA_LIB_USE( "FreeType2/freetype28MTd_"AAA_LIB_VSTOOL_STR() )
#		else
			AAA_LIB_USE( "FreeType2/freetype28MT_"AAA_LIB_VSTOOL_STR() )
#		endif
#	else
#		if AAA_DEBUG()
//todox64
#			define AAA_LIB_USE_CONFIGURATION()	"Debug"	
#		else
#			define AAA_LIB_USE_CONFIGURATION()	"Debug"
#		endif
		AAA_LIB_USE( "freetype_"AAA_LIB_USE_CONFIGURATION()"_"AAA_LIB_USE_PLATFORM()"_120" )
#	endif	//AAA_TRUETYPE2_USE()
#endif	//AAA_TRUETYPE1_USE()

namespace aaa::font {

//todoqq	deallocate

::c_bind*	g_bank_def = nullptr;
REAL	g_precision;

#if AAA_TRUETYPE1_USE()
	static	AAA_FTFace*			gs_face[FONT_MAX_NB];
	static	GLTTFont*			gs_font[FONT_MAX_NB];
	static	GLTTOutlineFont*	gs_font_outline[FONT_MAX_NB];
#else
	void err_print_unused( C_PCHAR_C signature )
	{
		err_print( "function %s() not implemented, AAA_TRUETYPE1_USE() defined as %d in this executable version.", signature, AAA_TRUETYPE1_USE() );
	}
#endif //#if AAA_TRUETYPE1_USE()

static AAA_ERR	allocate()
{
	g_bank_def = ::c_bind::get_new( "font_bind" );
	g_bank_def->set( FONT_MAX_NB, "Font Bind", "font_bind", aaa::file::TYPE_IO_FONT, FONT_MAX_NB/32, true, true );
	return ( g_bank_def ) ? AAA_OK : ERR_ANY ; 
}

static void	deallocate()
{
	if( g_bank_def )
	{
#if AAA_TRUETYPE1_USE()
		for( INT32 i = 0; i < FONT_MAX_NB; ++i )
		{
			SAFE_DELETE(  gs_font_outline[ i ] );
			SAFE_DELETE(  gs_font[ i ] );
			SAFE_DELETE(  gs_face[ i ] );
		}
#endif
		SAFE_DELETE( g_bank_def );
	}
}

void	deinit()
{
	deallocate();
}

AAA_ERR	init()
{
	AAA_ERR	retcode;

#if AAA_TRUETYPE1_USE()
	for( INT32 i = 0; i < FONT_MAX_NB; ++i )
	{

		gs_face[ i ] = nullptr;
		gs_font[ i ] = nullptr;
		gs_font_outline[ i ] = nullptr;
	}
#endif

	retcode = allocate();
	if( NOERR( retcode ) )
		g_bank_def->set_focus();
	else
		BOX_ERR( "Can't allocate the font list" );
	return retcode;
}

#if AAA_TRUETYPE1_USE()
void	load_low( INT32 font_bind )
{
	gs_face[			font_bind ]	= new AAA_FTFace;
	gs_font[			font_bind ]	= new GLTTFont(			gs_face[ font_bind ] );
	gs_font_outline[	font_bind ]	= new GLTTOutlineFont(	gs_face[ font_bind ] );

	if( !gs_face[ font_bind ]->open( aaa::font::g_bank_def->get_str( font_bind ) ) )
		ERR_PRINT_STRING( "unable to open ttf file" );

	gs_font[			font_bind ]->set_precision( g_precision );
	gs_font_outline[	font_bind ]->set_precision( g_precision );

	int point_size = 20;
	if( ! gs_font[ font_bind ]->create( point_size ) )
		ERR_PRINT_STRING( "unable to create font" );
	if( ! gs_font_outline[ font_bind ]->create( point_size ) )
		ERR_PRINT_STRING( "unable to create outine font" );
}

void	load( INT32 font_bind )
{
	if( !gs_face[ font_bind ] )
		font_load_low( font_bind );
}


// int getWidth( const char* text );
//  int getHeight() const;
#define	BUF_LEN	4096
template<class T>
static FINLINE void	render_low( UCHAR* pt, T* a, REAL align_hori, REAL interline )
{
	if( a )
	{

		REAL	offset_v = a->get_height() * interline;
		CHAR	buf[ BUF_LEN ];
		INT32	buf_len;
		CHAR*	dst;
		REAL	offset_h;
		REAL	offset_h_last = 0;

		GOL::translate_y( offset_v );
		offset_v = -offset_v;

		while( *pt )
		{
			dst = buf;
			buf_len = BUF_LEN;
			while( *pt >= 32 && --buf_len > 0 )
				*dst++ = *pt++;
			*dst = 0;
			offset_h = -a->get_width( buf ) * align_hori;
			GOL::translate( offset_h - offset_h_last, offset_v );
			offset_h_last = offset_h;
			a->output( buf );
			if( *pt == 0 )
				break;
			++pt;
		}
	}
}

template<class T>
static FINLINE void	render_direct_low( UCHAR* pt, T* a, REAL align_hori, REAL interline )
{
	if( a )
	{
		REAL	offset_v = a->get_height() * interline;
		CHAR	buf[ BUF_LEN ];
		INT32	buf_len;
		CHAR*	dst;
		//REAL	offset_h;
		//REAL	offset_h_last = 0;

//		GOL::translate_y( offset_v );
		offset_v = -offset_v;
		while( *pt )
		{
			dst = buf;
			buf_len = BUF_LEN;
			while( *pt >= 32 && --buf_len > 0 )
				*dst++ = *pt++;
			//offset_h = -a->get_width( buf ) * align_hori;
//			GOL::translate( offset_h-offset_h_last , offset_v, 0);
			if( *pt == '\n' )
				GOL::translate_y( offset_v );
			*dst = 0;
			//offset_h_last = offset_h;
			a->output( buf );
			if( *pt == 0 )
				break;
			++pt;
		}
		GOL::translate_y( offset_v );
	}
}
#endif



void	render_char( UINT8 CONST ch, INT32 CONST font_bind, bool CONST b_outline )
{
#if AAA_TRUETYPE1_USE()
	font_load( font_bind);
	if( b_outline )
		gs_font_outline[ font_bind ]->output( ch );
	else
		gs_font[font_bind]->output( ch );
#else
	err_print_unused( __FUNCTION__ );
#endif
}

void render(		o_str CONST & str_in, INT32 CONST font_bind, bool CONST b_outline, REAL align_hori, REAL interline )
{
#if AAA_TRUETYPE1_USE()
	if( str )
	{
		load( font_bind );
		if( b_outline )
			font_render_low( (UCHAR*)str, gs_font_outline[ font_bind ], align_hori, interline );
		else
			font_render_low( (UCHAR*)str, gs_font[ font_bind ], align_hori, interline );
	}
#else
	c_ftfont* ftfont = c_ftfont::get( font_bind, b_outline ? c_ftfont::FTGL_OUTLINE : c_ftfont::FTGL_POLYGON, 72 );
	if( ftfont )
	{
		auto layout = ftfont->get_layout();
		layout->Render( str_in.get(), str_in.get_len() );
	}
#endif
}

//todox64 we need this eventually
void	render( C_PCHAR_C str, INT32 CONST font_bind, bool CONST b_outline, REAL align_hori, REAL interline )
{
#if AAA_TRUETYPE1_USE()
	if( str )
	{
		load( font_bind );
		if( b_outline )
			font_render_low( (UCHAR*)str, gs_font_outline[ font_bind ], align_hori, interline );
		else
			font_render_low( (UCHAR*)str, gs_font[ font_bind ], align_hori, interline );
	}
#else
	err_print_unused( __FUNCTION__ );
#endif
}


#if AAA_TRUETYPE1_USE()
void	render_direct( CHAR* str, INT32 font_bind, bool b_outline, REAL align_hori, REAL interline )
{
	if( str )
	{
		load( font_bind );
		if( b_outline )
			render_direct_low( (UCHAR*)str, gs_font_outline[ font_bind ], align_hori, interline );
		else
			render_direct_low( (UCHAR*)str, gs_font[ font_bind ], align_hori, interline );
	}
}

//todoopt	this is only a start but update font lib first
REAL	get_width( C_PCHAR_C str, INT32 CONST font_bind, bool CONST b_outline )
{
	REAL	r;
	if( str )
	{
		font_load( font_bind );
		if( b_outline )
			r = (REAL)gs_font_outline[ font_bind ]->get_width( str );
		else
			r = (REAL)gs_font[ font_bind ]->get_width( str );
	}
	else
		r = 0;
	return r;
}

template<class T>
static FINLINE void	fill_pos_low( REAL* point, CONST UCHAR* pt, T* a, REAL align_hori, REAL interline )
{
	if( a )
	{
		REAL	offset_v = a->get_height() * interline;
		CHAR	buf[ BUF_LEN ];
		CHAR	buf_char[ 2 ];
		INT32	buf_len;
		CHAR*	dst;
		REAL	offset_h;
		//REAL	offset_h_last = 0;
		REAL	offset_v_abs;

		buf_char[ 1 ] = 0;

//		GOL::translate_y( offset_v );
		offset_v = -offset_v;
		offset_v_abs = 0;

		while( *pt )
		{
			//group line
			dst = buf;
			buf_len = BUF_LEN;
			while( *pt >= 32 && --buf_len > 0 )
				*dst++ = *pt++;
			*dst = 0;
			//place it
			offset_h = -a->get_width( buf ) * align_hori;
//			GOL::translate( offset_h-offset_h_last , offset_v, 0);
			//offset_h_last = offset_h;
			//extract it
			dst = buf;
			while( *dst )
			{
				*buf_char = *dst++;
				*point++ = offset_h;
				offset_h += a->get_width( buf_char );
				*point++ = offset_v_abs;
			}
			*point++ = offset_h;
			*point++ = offset_v_abs;
			offset_v_abs += offset_v;

			//next
			if( *pt == 0 )
				break;
			++pt;
		}
	}
}
#endif	//#if AAA_TRUETYPE1_USE()

void	fill_pos( REAL* pt, C_PCHAR_C str, INT32 CONST font_bind, bool CONST b_outline, REAL CONST align_hori, REAL CONST interline )
{
#if AAA_TRUETYPE1_USE()
	if( str )
	{
		load( font_bind );
		if( b_outline )
			fill_pos_low( pt, (UCHAR*)str, gs_font_outline[ font_bind ], align_hori, interline );
		else
			fill_pos_low( pt, (UCHAR*)str, gs_font[ font_bind ], align_hori, interline );
	}
#else
		err_print_unused( __FUNCTION__ );
#endif
}


REAL	get_height( INT32 CONST font_bind, bool CONST b_outline )
{
#if AAA_TRUETYPE1_USE()
	load( font_bind );
	if(	b_outline )
	{
		if( gs_font_outline[font_bind])
			return gs_font_outline[font_bind]->get_height();
	}
	else
	{
		if( gs_font[ font_bind ] )
			return gs_font[font_bind]->get_height();
	}
#else
	err_print_unused( __FUNCTION__ );
#endif
	return 1;
}

static	void	MAACALLBACK menu_fn( INT32	in )
{
	g_bank_def->set_index_cur( in );
}

INT32	bind_menu_build()
{
	return g_bank_def->menu_build( 0, menu_fn );
}

void	load_from_file( UINT32 CONST index, o_str CONST & filename_in )
{
	c_ftfont::erase( index );
	sdf::erase( index ); 
	//o_str& fname = o_str::push_name();
		//fname.compact_fname( filename_in );
	//g_bank_def->set_item( index, fname );
	g_bank_def->set_item( index, filename_in );
	//o_str::pop_name();
}

void	load_from_file( o_str CONST & filename_in )
{
	load_from_file( g_bank_def->get_index_cur(), filename_in );
}

} //namespace aaa::font