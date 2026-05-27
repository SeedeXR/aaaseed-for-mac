#include "obj_ui/bdd/bdd_geo/bdd_text.h"
#include "infrastructure/bind/bind.h"
#include "infrastructure/viewport.h"
#include "ui/alphabet.h"
#ifndef AAA_AAA_FTGL_H
#	include "ftgl/aaa_ftgl.h"
#endif
#include "draw/model.h"
#include "gol/gol.h"
#include "gol/gol_tex.h"
#include "gol/gol_matrix.h"
#include "draw/axe.h"
#include "obj_ui/deformer/def_node.h"
#include "draw/rect.h"
#ifdef	WIN32
#	include "platform/win32/win_dlg_util.h"
#endif
#ifndef AAA_OURTRUETYPE_H
#	include "truetype/OurTrueType.h"
#endif
#include "system/shared/SystemUtils.h"
#include "SdfText/SDFFont.h"


using namespace std::string_literals; // enables s-suffix for std::string literals
//todo	move to http://www.angelcode.com/products/bmfont/
//			or https://code.google.com/p/freetype-gl/
//			or valve paper


// Convert 8-bit characters from the active locale to UTF-8.
// Text and Result MAY be the same buffer.
std::string&	UTF8Encode( IN CONST std::string& Text, OUT std::string& Result )
{
	int		InputChars = (int)strlen(Text.c_str())+0;	// MSVCP strings tend to append extra nulls, so don't
														// process them.

	// We need to first convert the ASCII input to Unicode before we can convert it to UTF-8...
	int		UnicodeChars = MultiByteToWideChar(CP_ACP, 0, Text.c_str(), InputChars, 0, 0);
	LPWSTR	UnicodeBuffer = (LPWSTR)alloca(UnicodeChars*sizeof(WCHAR));
	MultiByteToWideChar(CP_ACP, 0, Text.c_str(), InputChars, UnicodeBuffer, UnicodeChars);

	// Now that we've got everything translated to Unicode, we can (finally) convert it to UTF-8.
	int		UTF8Chars = WideCharToMultiByte(CP_UTF8, 0, UnicodeBuffer, UnicodeChars, 0, 0, 0, 0);
	LPSTR	UTF8Buffer = (LPSTR)alloca(UTF8Chars);
	WideCharToMultiByte(CP_UTF8, 0, UnicodeBuffer, UnicodeChars, UTF8Buffer, UTF8Chars, 0, 0);

	// Store it in the std::string passed in.  Note that this string will be null terminated just like
	// any other C-style string, but it'll have the special encodings for multibyte characters.
	Result.assign(UTF8Buffer, UTF8Chars);

	return Result;
}

//int UTF8Encode2(IN CONST char& Text, OUT char& Result)
//{
//	int InputChars = strlen(&Text)+0;	// MSVCP strings tend to append extra nulls, so don't
//										// process them.
//
//	// We need to first convert the ASCII input to Unicode before we can convert it to UTF-8...
//	int UnicodeChars = MultiByteToWideChar(CP_ACP, 0, &Text, InputChars, 0, 0);
//	LPWSTR UnicodeBuffer = (LPWSTR)alloca(UnicodeChars*sizeof(WCHAR));
//	MultiByteToWideChar(CP_ACP, 0, &Text, InputChars, UnicodeBuffer, UnicodeChars);
//
//	// Now that we've got everything translated to Unicode, we can (finally) convert it to UTF-8.
//	int UTF8Chars = WideCharToMultiByte(CP_UTF8, 0, UnicodeBuffer, UnicodeChars, 0, 0, 0, 0);
//	LPSTR UTF8Buffer = (LPSTR)alloca(UTF8Chars);
//	WideCharToMultiByte(CP_UTF8, 0, UnicodeBuffer, UnicodeChars, UTF8Buffer, UTF8Chars, 0, 0);
//
//	// Store it in the std::string passed in.  Note that this string will be null terminated just like
//	// any other C-style string, but it'll have the special encodings for multibyte characters.
////	Result.assign(UTF8Buffer, UTF8Chars);
//
//	return UTF8Chars;
//}

// Convert UTF-8 characters to 8-bit characters from the active locale.
// UTF8 and Result MAY be the same buffer.
std::string&	UTF8Decode( IN CONST std::string& UTF8, OUT std::string& Result )
{
	int		InputBytes = (int)strlen(UTF8.c_str())+0;	// MSVCP strings tend to append extra nulls, so don't
														// process them.

	// Again, we need to convert the UTF-8 string to Unicode before we can convert it to 8-bit.
	int		UnicodeChars = MultiByteToWideChar(CP_UTF8, 0, UTF8.c_str(), InputBytes, 0, 0);
	LPWSTR	UnicodeBuffer = (LPWSTR)alloca(UnicodeChars*sizeof(WCHAR));
	MultiByteToWideChar(CP_UTF8, 0, UTF8.c_str(), InputBytes, UnicodeBuffer, UnicodeChars);

	// Now that we've got everything translated to Unicode, we can convert it to 8-bit characters.
	int		SingleByteChars = WideCharToMultiByte(CP_ACP, 0, UnicodeBuffer, UnicodeChars, 0, 0, 0, 0);
	LPSTR	SingleByteBuffer = (LPSTR)alloca(SingleByteChars);
	WideCharToMultiByte(CP_ACP, 0, UnicodeBuffer, UnicodeChars, SingleByteBuffer, SingleByteChars, 0, 0);

	Result.assign(SingleByteBuffer, SingleByteChars);

	return Result;
}


#if !AAA_BDD_TEXT_SDF_CACHE_BY_INSTANCE()
namespace {
//todofont	check we free every thing well in this case
	std::unordered_map< UINT32, aaa::font::c_sdf_text* >	SDF_CACHE;
}
#endif

FACTORY_CREATE_PROP_V1( c_bdd_text, bdd_text, Text 3D, text, sub_menu="Geometry"; );

namespace {
	CONSTEXPR	C_PCHAR_C	_font_type_str[ c_bdd_text::FONT_TYPE_MAX_NB ] =
	{
		"MAA_LINE",
		"GLUT_LINE",
		"FONT_TEXTURE",
		"FONT_BUFFER",
		"FONT_OUTLINE",
		"FONT_POLYGON",
		"FONT_EXTRUDE",
		"FONT_BITMAP",
		"FONT_PIXMAP",
		"FONT_SDF"
	};

	enum ALIGN_HORI : INT32
	{
		ALIGN_HORI_LEFT		= 0,
		ALIGN_HORI_CENTER,
		ALIGN_HORI_RIGHT,
		ALIGN_HORI_MAX_NB
	};
	static	C_PCHAR_C	align_hori_str[ ALIGN_HORI_MAX_NB ] =
	{
		"Left",
		"Center",
		"Right",
	};

	enum ALIGN_VERT : INT32
	{
		ALIGN_VERT_LINE_FIRST = 0,
		ALIGN_VERT_BOX_BOTTOM,
		ALIGN_VERT_BOX_MIDDLE,
		ALIGN_VERT_BOX_TOP,
		ALIGN_VERT_MAX_NB
	};
	CONSTEXPR	C_PCHAR_C	align_vert_str[ ALIGN_VERT_MAX_NB ] =
	{
		"line_first",
		"bottom",
		"middle",
		"top",
	};

	enum TEXT_JUSTIFICATION : INT32
	{
		TEXT_JUSTIFICATION_LEFT = 0,
		TEXT_JUSTIFICATION_CENTER,
		TEXT_JUSTIFICATION_RIGHT,
		TEXT_JUSTIFICATION_JUSTIFY,
		TEXT_JUSTIFICATION_MAX_NB
	};
	CONSTEXPR	C_PCHAR_C	text_justification_str[ TEXT_JUSTIFICATION_MAX_NB ] =
	{
		"Left",
		"Center",
		"Right",
		"Justify"
	};

	CONSTEXPR FP32 X_FACTOR_MAA_LINE  = FP32(.48);
	CONSTEXPR FP32 Y_FACTOR_GLUT_LINE = FP32(.833);
}

namespace n_bdd_text
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 30 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32 TEK_PARAM_NB	= 4;
	CONSTEXPR INT32 OUT_PARAM_NB	= 6;
	CONSTEXPR INT32 SHADOW_PARAM_NB	= 9;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 3;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	TEK_PARAM_NB
									+	OUT_PARAM_NB
									+	SHADOW_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
		
		PARAM_DEF_POINT_XYZ(	origin				)
		PARAM_DEF_AXE_X(		axe					)

		PARAM_DEF_SYMBO(		type,				c_bdd_text::FONT_TYPE_GLUT_LINE, c_bdd_text::FONT_TYPE_TEXTURE, c_bdd_text::FONT_TYPE_MAX_NB-1, _font_type_str )
		PARAM_DEF_INT32(		font,				0,1,	0,aaa::font::FONT_MAX_NB-1 )
		
		PARAM_DEF_GROUP_CLOSED( TEK, TEK_PARAM_NB )
			PARAM_DEF_STR_LOCKED(	font_name			)
			PARAM_DEF_INT32_LOCKED(	font_char_map_nb	)
			PARAM_DEF_SYMBO_LOCKED(	type_used,		c_bdd_text::FONT_TYPE_GLUT_LINE, c_bdd_text::FONT_TYPE_TEXTURE, c_bdd_text::FONT_TYPE_MAX_NB-1, _font_type_str )
			PARAM_DEF_INT32_POS(	buffer_id,			1,0		)

		PARAM_DEF_SYMBO_PSTR(	alignment,			ALIGN_HORI_LEFT,			ALIGN_HORI_RIGHT,				align_hori_str			)
		PARAM_DEF_SYMBO_PSTR(	alignment_vertical, ALIGN_VERT_LINE_FIRST,		ALIGN_VERT_BOX_BOTTOM,			align_vert_str			)
		PARAM_DEF_SYMBO_PSTR(	text_justification, TEXT_JUSTIFICATION_RIGHT,	TEXT_JUSTIFICATION_LEFT,		text_justification_str	)

		PARAM_DEF_BOOL_OFF(			fit			)
		PARAM_DEF_SCALE_FP32_UV(	fit_size	)

		PARAM_DEF_REAL_ONE(		interline			)
		PARAM_DEF_REAL_ONE(		line_length			)
		PARAM_DEF_REAL_POS_ONE(	depth				)
		PARAM_DEF_REAL_ZERO(	outset				)
		PARAM_DEF_BOOL_ON(		render_front		)
		PARAM_DEF_BOOL_ON(		render_side			)
		PARAM_DEF_BOOL_ON(		render_back			)
		PARAM_DEF_BOOL_OFF(		use_display_list	)
		
		PARAM_DEF_INT32(		face_size,			24,12,	1,(1<<13)-1	)	//	we use 13 bits in our font fast access allso itthe lib explode over 8192 (experimental)
		PARAM_DEF_INT32(		face_dpi,			96,72,	1,(1<<13)-1	)	//	we use 13 bits in our font fast access
//		PARAM_DEF_POINT_XYZ(	displacement		)

		PARAM_DEF_BOOL_OFF(		shadow_draw	)
		PARAM_DEF_GROUP_CLOSED( Shadow, SHADOW_PARAM_NB )
			PARAM_DEF_POINT_FP32_UVAF(		shadow_offset	)
			PARAM_DEF_COLOR_RGBGA_BLACK(	shadow			)

		PARAM_DEF_BOOL_OFF(		draw_bounding_box	)
		PARAM_DEF_BOOL_OFF(		draw_line_box		)
	
		PARAM_DEF_GROUP_CLOSED( Out, OUT_PARAM_NB )
			PARAM_DEF_FP32_LOCKED_XYZ( bounding_box_min	)
			PARAM_DEF_FP32_LOCKED_XYZ( bounding_box_max	)

//		PARAM_DEF_BOOL_OFF( draw_point )
		PARAM_DEF_BOOL_OFF(		generate_trig		)
		PARAM_DEF_INT32(		generate_index,		1,0,	0,99	)
		PARAM_DEF_BOOL_LOCKED(	generating			)
		PARAM_DEF_BOOL_LOCKED(	generate_done		)

		PARAM_DEF_STR(			text				)
	};

	PARAM_DEF_MAKE_INDEX( TEK			);
	PARAM_DEF_MAKE_INDEX( depth			);
	PARAM_DEF_MAKE_INDEX( outset		);
	PARAM_DEF_MAKE_INDEX( render_front	);
	PARAM_DEF_MAKE_INDEX( render_side	);	
	PARAM_DEF_MAKE_INDEX( render_back	);
	PARAM_DEF_MAKE_INDEX( Shadow		);	
}

static	CHAR	ftgl_generate_buf[ aaa::dialog::STR_LEN_MAX ];
extern	bool	b_str_generating;
extern	bool	b_str_generate_done;

void	c_bdd_text::prepare_for_ui()
{
	INT32 h = 1;
	prepare_for_ui_geo( h );

	h = n_bdd_text::PARAM_INDEX_TEK;
	auto param = get_param( h );
	param->get_comment_always()->set_fname_pure( _font_name );
	param_set_unused( h + 1, _b_fit_ui );
	param_set_unused( h + 2, _b_fit_ui );

	bool b_extrude_not = _s_type_ui != FONT_TYPE_EXTRUDE;
	param_set_unused( n_bdd_text::PARAM_INDEX_depth			,b_extrude_not );
	param_set_unused( n_bdd_text::PARAM_INDEX_render_front	,b_extrude_not );
	param_set_unused( n_bdd_text::PARAM_INDEX_render_side	,b_extrude_not );
	param_set_unused( n_bdd_text::PARAM_INDEX_render_back	,b_extrude_not );
	param_set_unused( n_bdd_text::PARAM_INDEX_outset		,OUTSIDE_MIN_MAX( _s_type_used, FONT_TYPE_OUTLINE, FONT_TYPE_EXTRUDE ) );
	param_set_unused_n( n_bdd_text::PARAM_INDEX_Shadow		,!_b_shadow_ui, n_bdd_text::SHADOW_PARAM_NB+1 );
}

void	c_bdd_text::param_init_pt_static()
{
	INT32 h = param_init_pt_geo();

	param_set_pt_3( h, _origin	);
	param_set_pt( h, _s_axe		);

	param_set_pt( h, _s_type_ui			);
	param_set_pt( h, _s_font_ui			);

	++h;
		param_set_pt( h, _font_name			);
		param_set_pt( h, _font_char_map_nb	);
		param_set_pt( h, _s_type_used		);
		param_set_pt( h, _buffer_id_ui		);

	param_set_pt( h, _s_alignment_hori_ui		);
	param_set_pt( h, _s_alignment_vert_ui		);
	param_set_pt( h, _s_text_justification_ui	);

	param_set_pt( h, _b_fit_ui				);
	param_set_pt_2( h, _fit_size_ui			);

	param_set_pt( h, _interline_ui			);
	param_set_pt( h, _line_length_ui		);
	param_set_pt( h, _depth_ui				);
	param_set_pt( h, _outset_ui				);
	param_set_pt( h, _b_render_front_ui		);
	param_set_pt( h, _b_render_side_ui		);
	param_set_pt( h, _b_render_back_ui		);
	param_set_pt( h, _b_use_display_list_ui );

	param_set_pt( h, _face_size_ui			);
	param_set_pt( h, _face_dpi_ui			);
//	param_set_pt_3( h, _displacement_ui		);
	param_set_pt( h, _b_shadow_ui			);
	++h;
		param_set_pt_4(		h, _shadow_offset_ui	);
		param_set_pt_rgbfa( h, _shadow_color_ui		);

	param_set_pt( h, _b_draw_bbox_ui		);
	param_set_pt( h, _b_draw_box_line_ui	);

	++h;
		param_set_pt_3( h, _bbox_min_out );
		param_set_pt_3( h, _bbox_max_out );

//	param_set_pt( h, _b_draw_point_ui );

	param_set_pt( h, _b_generate_trig_ui	);
	param_set_pt( h, _s_generate			);
	param_set_pt( h, b_str_generating		);
	param_set_pt( h, b_str_generate_done	);

	param_set_pt( h, _text					);

	err_param_init_pt( h );
}

void c_bdd_text::init()
{
	_text.erase();
//	_point = nullptr;
//	_point_deformed = nullptr;
//	_point_nb = 0;
	_s_type_used = FONT_TYPE(-42);
	_s_font = -42;
	_b_font_changed = false;
	_interline = -42;
	_face_size = -42;
	_face_dpi = -42;
	_s_text_justification = -42;
	_line_length = -42;
//	_depth = -42;
//	_outset = REAL(-42);
	_font_unique_id	= 0;
	clear_v3( _bbox_min_out );
	clear_v3( _bbox_max_out );
//	_b_use_display_list = false;
}

CONSTRUCTOR_CREATE( c_bdd_text )
,_ft_font			{nullptr}
#if	!AAA_BDD_TEXT_RENDER_UNICODE()
,_buf_wchar			{nullptr}
,_buf_wchar_size	{0}
#endif
//,_layout{}			{nullptr}
,_sdf_text			{nullptr}
{
	param_init_with( n_bdd_text::param, n_bdd_text::PARAM_NB_MAX ); // bdd_tex2d_param, BDD_TEX2D_PARAM_NB_MAX);

	init();
}

c_bdd_text::~c_bdd_text()
{
	dealloc();
}

#if	!AAA_BDD_TEXT_RENDER_UNICODE()
void c_bdd_text::alloc_wchar( INT32 CONST size )
{
	if( _buf_wchar_size < size )
	{
		_buf_wchar = (wchar_t*) REALLOC_SIGNATURE( _buf_wchar, size * sizeof(wchar_t), __FUNCTION__ );
		_buf_wchar_size = _buf_wchar ? size : 0; 
	}
}
#endif

void	c_bdd_text::dealloc()
{
#if	!AAA_BDD_TEXT_RENDER_UNICODE()
	IF_FREE_AND_NULL( _buf_wchar );
	_buf_wchar_size = 0;
#endif
#if AAA_BDD_TEXT_SDF_CACHE_BY_INSTANCE()
	for( auto & text : SDF_CACHE )
		SAFE_DELETE( text.second );
	SDF_CACHE.clear();
#endif
//	SAFE_DELETE( _sdf_text );
//	dealloc_point();
//	dealloc_fonts();
}

/*
void c_bdd_text::dealloc_point()
{
	SAFE_DELETE_ARRAY( _point );
//	if ( _point_nb )
//	{
//		delete [] _point;
//		_point = nullptr;
		_point_deformed = nullptr;
		_point_nb = 0;
//	}
}
FINLINE	void c_bdd_text::alloc_point( INT32 nb_in )
{
	if ( _point_nb < nb_in )
	{
		dealloc_point();
		_point = new  REAL[ nb_in * 3 * 2 ];
		if ( _point )
		{
			_point_nb = nb_in;
			_point_deformed = _point + nb_in * 3;
		}
	}
}
*/

void c_bdd_text::set_text( C_PCHAR_C in )
{
	_text.set( in );
}

void c_bdd_text::add_text( C_PCHAR_C in )
{
	_text.add( in );
}

extern	void th_generate_str( INT32 s_generate, CHAR* buf, INT32 size );

void	c_bdd_text::set_font( FONT_TYPE	s_type_asked )
{
	UINT32 unique_id = c_ftfont::get_unique_id(_s_font_ui);
	bool b_font_new_change = _font_unique_id != unique_id	// in fact we don't need _s_font != _s_font_ui anymore
		|| _s_font != _s_font_ui
		|| _s_type_used != s_type_asked;

	if( s_type_asked == c_bdd_text::FONT_TYPE_SDF )
	{
		// Cache sdftext geometry using unique id (id will be set from lua)
		if( SDF_CACHE.find( _buffer_id_ui ) == SDF_CACHE.end() )
		{
			auto text = new aaa::font::c_sdf_text();
			if( !text )
			{
				ERR_PRINT_STRING( "Memory allocation issue in %d", __FUNCTION__ );
				return;
			}
			SDF_CACHE[ _buffer_id_ui ] = text;
		}

		_sdf_text = SDF_CACHE[ _buffer_id_ui ];

		if( b_font_new_change || _buffer_id_ui != _buffer_id )
		{
			// Init font, load json data : move to OurTrueType for use by aaaseed internally
			auto font = aaa::font::sdf::get( _s_font_ui );
			if( font->is_loaded() )
			{
				_sdf_text->set_font( font );
		
				_font_unique_id = unique_id;
				_s_font = _s_font_ui;
				_s_type_used = s_type_asked;

				_buffer_id = _buffer_id_ui;

				_font_name.set( aaa::font::g_bank_def->get_o_str( _s_font_ui ) );
				_font_char_map_nb = font->get_char_count();
			}
			else
			{
				_sdf_text->set_font( nullptr );
				_font_char_map_nb = 0;
			}
		}
	}
	else
	{		
		if( b_font_new_change
			|| _face_size != _face_size_ui
			|| _face_dpi != _face_dpi_ui
			)
		{
			//todosecond arg is now check for correspondance between the 2 enums
			c_ftfont* ftfont = c_ftfont::get( _s_font_ui, s_type_asked - c_bdd_text::FONT_TYPE_TEXTURE, _face_size_ui, _face_dpi_ui );
			_ft_font = ftfont;
			if( ftfont )
			{
				//_s_font = _s_font_ui;
				_font_unique_id = unique_id;
				_s_font = _s_font_ui;	
				_s_type_used = s_type_asked;

				_face_size = _face_size_ui;
				_face_dpi = _face_dpi_ui;		
				_b_font_changed = true;

				//_layout = _ft_font->get_layout();

				_font_name.set( aaa::font::g_bank_def->get_o_str( _s_font_ui ) );
				_font_char_map_nb = _ft_font->get_font()->CharMapCount();
			}
		}
		if( _ft_font )
		{
			if( INSIDE_MIN_MAX( _s_type_used, FONT_TYPE_OUTLINE, FONT_TYPE_EXTRUDE ) )	//Geometry type
			{
				if( _s_type_used == FONT_TYPE_EXTRUDE )
				{
					_ft_font->set_depth( _depth_ui );
					_render_mode = 0;
					if( _b_render_front_ui )
						_render_mode |= FTGL::RENDER_FRONT;
					if( _b_render_side_ui )
						_render_mode |= FTGL::RENDER_SIDE;
					if( _b_render_back_ui )
						_render_mode |= FTGL::RENDER_BACK;
				}
				//FTFont* ft = _ft_font->get_font();
				//if( _s_type_used == FONT_TYPE_EXTRUDE && (_b_font_changed || _depth != _depth_ui) )
				//{
				//	ft->Depth( _depth_ui );
				//	_depth = _depth_ui;
				//}

				_ft_font->set_outset( _outset_ui );	// .01 is experimental to have [-1,1] on outset quive good result
		

				//if( _b_font_changed || _outset != _outset_ui )
				//{
				//	ft->Outset( _outset_ui );
				//	_outset = _outset_ui;
				//}

			}
			_ft_font->set_use_display_list( _b_use_display_list_ui );
			_ft_font->update();
		}
	}
}

static	FINLINE	bool can_use( o_str& o )
{
	INT32	len = o.get_len();
	if( len == 0 )
		return false;
	else
	{
		CONST UINT8* str = (CONST UINT8*) o.get();
		while( len > 0 )
		{
			//INT32 val = UINT8(*str);
			//GOOD_PRINT_STRING(  "0x%x", val );
			if( INSIDE_MIN_MAX( *str, UINT8(224), UINT8(255) ) )
			{
				break;
				//return false;
			}
			--len;
			++str;
		}
	}
	return true;
}

namespace {
	FTPoint ft_point_zero;
}
//todofont we should add check for chanhged eventually a caching mecanism by str
void c_bdd_text::update()
{
	_b_draw_ready = false;

	REAL size_over_one = 1.;

	FONT_TYPE s_type_asked = _s_type_ui;
	if( _b_generate_trig_ui )
	{
		_b_generate_trig_ui = false;
		if( !b_str_generating )
			th_generate_str( _s_generate, ftgl_generate_buf, aaa::dialog::STR_LEN_MAX );
	}

	if( b_str_generate_done )
	{
		_text.set( ftgl_generate_buf );
		b_str_generate_done = false;
	}

	o_str CONST & font_path = aaa::font::g_bank_def->get_o_str( _s_font_ui );
	if( font_path.is_ending_with( ".json" ) )
	{
		// Font bind is a json file, this is a sdf font
		s_type_asked = FONT_TYPE::FONT_TYPE_SDF;
	}

	if( _text.is_empty() )
	{
	}
	else if( s_type_asked <= FONT_TYPE::FONT_TYPE_GLUT_LINE )
	{	// OpenGl Line fonts
		_s_type_used = s_type_asked;
		clear_v3( _bbox_min_out );
		aaa::alphabet::get_str_translate( _bbox_max_out, _text.get(), s_type_asked );
		//todo vertical and left side (for Maa type only) still need work
		if( s_type_asked == FONT_TYPE::FONT_TYPE_MAA_LINE )
		{
			_bbox_max_out[0] *= X_FACTOR_MAA_LINE;
		}
		else
		{
			_bbox_max_out[1] *= Y_FACTOR_GLUT_LINE;
		}
		_b_draw_ready = true;
	}
	else if( s_type_asked == FONT_TYPE::FONT_TYPE_SDF )
	{	// Signed distance field
		set_font( s_type_asked );
		if( _sdf_text && _font_char_map_nb > 0 )
		{
			//std::string str_utf8;
			std::string str_utf8 = _text.to_string();
			//auto str1 = u8"testãingãã"s;
			//auto chinese_test = u8"书、杂志等中区别于图片的）正文，文字材料";
			//std::string str_chinese = chinese_test;
		//	UTF8Encode( s1, str_utf8 );
			_sdf_text->set_text( str_utf8 );

			aaa::font::c_sdf_text::TEXT_JUSTIFICATION text_justification;
			switch( _s_text_justification )
			{
			case TEXT_JUSTIFICATION_RIGHT:		text_justification = aaa::font::c_sdf_text::TEXT_JUSTIFICATION::RIGHT;		break;
			case TEXT_JUSTIFICATION_CENTER:		text_justification = aaa::font::c_sdf_text::TEXT_JUSTIFICATION::CENTER;		break;
			case TEXT_JUSTIFICATION_JUSTIFY:	text_justification = aaa::font::c_sdf_text::TEXT_JUSTIFICATION::JUSTIFY;	break;
			case TEXT_JUSTIFICATION_LEFT:
			default:							text_justification = aaa::font::c_sdf_text::TEXT_JUSTIFICATION::LEFT;		break;
			}
			_sdf_text->set_justification( text_justification );

/*
			aaa::font::c_sdf_text::ALIGNMENT_H align_h;
			switch( _s_alignment_hori_ui )
			{
			case ALIGN_HORI_RIGHT:				align_h	= aaa::font::c_sdf_text::ALIGNMENT_H::RIGHT;	break;	
			case ALIGN_HORI_CENTER:				align_h	= aaa::font::c_sdf_text::ALIGNMENT_H::CENTER;	break;
			//case ALIGN_HORI_LEFT:				
			default:							align_h	= aaa::font::c_sdf_text::ALIGNMENT_H::LEFT;	break;
			}
			_sdf_text->set_align_horizontal( align_h );

			aaa::font::c_sdf_text::ALIGNMENT_V align_v;
			switch( _s_alignment_vert_ui )
			{			
			case ALIGN_VERT_BOX_MIDDLE:			align_v = aaa::font::c_sdf_text::ALIGNMENT_V::MIDDLE;	break;
			case ALIGN_VERT_BOX_TOP:			align_v = aaa::font::c_sdf_text::ALIGNMENT_V::TOP;		break;
			//case ALIGN_VERT_LINE_FIRST:
			//case ALIGN_VERT_BOX_BOTTOM:		
			default:							align_v = aaa::font::c_sdf_text::ALIGNMENT_V::BOTTOM;	break;
			}
			_sdf_text->set_align_vertical( align_v );
*/

			_sdf_text->set_line_width( _line_length_ui );
			_sdf_text->set_line_height( _interline_ui );
			_sdf_text->set_is_one_line( _text.get_line_nb() == 1 );

			aaa::boxf CONST bbox = _sdf_text->update();

			_bbox_min_out[0] = bbox.left();
			_bbox_min_out[1] = bbox.bottom();
			_bbox_min_out[2] = 0.;

			_bbox_max_out[0] = bbox.right();
			_bbox_max_out[1] = bbox.top();
			_bbox_max_out[2] = 0.;

			size_over_one = _sdf_text->get_line_max_ascender();
			//c_model::cur->get_size_scaled_v3( _size, size_over_one );
			//mul_v3( _bbox_min_out, _size );
			//mul_v3( _bbox_max_out, _size );

			_line_length = _line_length_ui;

			_b_draw_ready = true;
		}
	}
	else
	{	// ftGL
		set_font( s_type_asked );
		
		if( _ft_font )
		{
			auto layout = _ft_font->get_layout();

			REAL tmp = _line_length_ui * _face_size;
			if( _b_font_changed || _line_length !=  tmp )
			{
				layout->SetLineLength( tmp );
				_line_length = tmp;
			}
			REAL inter = _interline_ui / ( _ft_font->get_ascender_over_one() / _ft_font->get_line_height_over_one()) ;
			if( _b_font_changed || _interline != inter )
			{
				_interline = inter;
				layout->SetLineSpacing( _interline );
			}

			if( _b_font_changed || _s_text_justification != _s_text_justification_ui )
			{
				_s_text_justification = _s_text_justification_ui;
				FTGL::TextAlignment align;
				switch( _s_text_justification )
				{
				case TEXT_JUSTIFICATION_CENTER:		align = FTGL::ALIGN_CENTER;		break;
				case TEXT_JUSTIFICATION_JUSTIFY:	align = FTGL::ALIGN_JUSTIFY;	break;
				case TEXT_JUSTIFICATION_RIGHT :		align = FTGL::ALIGN_RIGHT;		break;
				//case TEXT_JUSTIFICATION_LEFT:
				default:							align = FTGL::ALIGN_LEFT;		break;
				}
				layout->SetAlignment( align );
			}

			size_over_one = _ft_font->get_ascender_over_one();


//			if( _s_alignment_hori_ui != ALIGN_HORI_LEFT || _s_alignment_vert_ui != ALIGN_VERT_LINE_FIRST )
			{
#if	AAA_BDD_TEXT_RENDER_UNICODE()
				//std::string s1( _text.get() );
				//_string = _text.to_string();
				//UTF8Encode( s1, _string );
				_bbox = layout->BBox( _text.get(), _text.get_len(), ft_point_zero );
#else
	//			if( can_use( _text ) )
	//			{	
	//				_bbox = _layout->BBox( _text.get(), _text.get_len() );
	//			}
				INT32 len = _text.get_len();
				int size_needed = MultiByteToWideChar( CP_UTF8, 0, _text.get(), len, 0, 0 );
				alloc_wchar( size_needed + 1 );
				sysutils::utf8_to_unicode( _buf_wchar, size_needed, _text.get(), len );
				// end string
				_buf_wchar[ size_needed ] = 0;
				_bbox = layout->BBox( _buf_wchar, size_needed, ft_point_zero );
#endif

				_bbox_min_out[0] = _bbox.Lower().Xf();
				_bbox_min_out[1] = _bbox.Lower().Yf();
				_bbox_min_out[2] = _bbox.Lower().Zf();

				_bbox_max_out[0] = _bbox.Upper().Xf();
				_bbox_max_out[1] = _bbox.Upper().Yf();
				_bbox_max_out[2] = _bbox.Upper().Zf();

				_b_draw_ready = true;
			}
		}
		_b_font_changed = false;
	}

	if( _b_draw_ready )
	{
		c_model::cur->get_size_scaled_v3( _size, size_over_one );
		mul_v3( _bbox_min_out, _size );
		mul_v3( _bbox_max_out, _size );

		switch( _s_alignment_hori_ui )
		{
		case ALIGN_HORI_RIGHT:			_offset[0] = -_bbox_max_out[0];										break;
		case ALIGN_HORI_CENTER:			_offset[0] = -(_bbox_min_out[0] + _bbox_max_out[0] ) * REAL(.5);	break;
		case ALIGN_HORI_LEFT:			
		default:						_offset[0] = -_bbox_min_out[0];										break;
		}

		switch( _s_alignment_vert_ui )
		{
		case ALIGN_VERT_BOX_BOTTOM:		_offset[1] = -_bbox_min_out[1];										break;
		case ALIGN_VERT_BOX_MIDDLE:		_offset[1] = -(_bbox_max_out[1] + _bbox_min_out[1]) * REAL(.5);		break;		
		case ALIGN_VERT_BOX_TOP:		_offset[1] = -_bbox_max_out[1];										break;						
		case ALIGN_VERT_LINE_FIRST:
		default:						_offset[1] = 0.;													break;
		}
		_offset[2] = 0.;
	}
	else
	{
		clear_v3( _bbox_min_out );
		clear_v3( _bbox_max_out );
	}

/*
	alloc_point( _text.get_len() + 1 );

	if ( _point )
	{
		//INT32	i;
		REAL*	src = _point_deformed;
		REAL*	dst = _point;
		INT32	i_u, i_v;

		axe_build_index( i_u, i_v, _s_axe );

		for( INT32 i = _text.get_len() + 1; i > 0; --i )
		{
			*(dst + i_u) = _origin[ i_u ] + *src++ * _size[ 0 ];
			*(dst + i_v) = _origin[ i_v ] + *src++ * _size[ 1 ];
			*(dst + _s_axe) = _origin[ _s_axe ];
			dst += 3;
		}
	}

	auto def = c_def_node::get_cur();
	if( def->is_deforming() )
		def->apply( _point_deformed, _point, _text.get_len() + 1 );
*/

}

void c_bdd_text::draw_bounding_box()
{
	//todo draw 3D box when extruded
	if( _b_draw_bbox_ui || _b_draw_box_line_ui )
	{
		GOL::push_att();
		//GOL::disable_texture_and_force_update_later();
		GOL::push_texture_dim( 0 );
		GOL::push_line_smooth( true );
		GOL::push_color();
					
			// Draw the front face
			FP32 y1 = _bbox_min_out[1] + _offset[1];
			FP32 y2 = _bbox_max_out[1] + _offset[1];

			if( _b_draw_box_line_ui )
			{
				//	GOL::color3( 1, 1, 1 );
				// Render guides that mark the edges of the wrap region.
				GOL::push_color3( 0.5, 1.0, 1.0 );
				draw_rect_line_at_z( _bbox_min_out[0]+_offset[0],y1, _line_length*_size[0]+_bbox_min_out[0]+_offset[0],y2, _bbox_max_out[2]+_offset[2] );
			}

			if( _b_draw_bbox_ui )
			{
				GOL::color_green();
				draw_rect_line_at_z( _bbox_min_out[0]+_offset[0],y1, _bbox_max_out[0]+_offset[0],y2, _bbox_min_out[2]+_offset[2] );
			}

		GOL::pop_att();
	}
}

//todofont do we keep tra and sca here (unused for the moment)
void c_bdd_text::draw_text()
{
	if( _s_type_used <= FONT_TYPE::FONT_TYPE_GLUT_LINE )	//	OpenGL line Fonts
		aaa::alphabet::draw_str( _text.get(), _s_type_used == FONT_TYPE::FONT_TYPE_MAA_LINE ? 0 : 1 );
	else if( _s_type_used == FONT_TYPE::FONT_TYPE_SDF )	// Signed distance font
		_sdf_text->draw();
	else if( _ft_font )	// ftfl
	{
		auto layout = _ft_font->get_layout();

//		ftgl_set_font_attribute( _s_font, _depth, _outset, _b_use_display_list );
//		ftgl_font_render( &_simple_layout, _text.get(), _s_font, _s_type_ui, _face_size, _align_hori, _interline, _line_length );

		//FTPoint disp( _offset[0], _offset[1], _offset[2] );
		if( _s_type_used <= FONT_TYPE::FONT_TYPE_EXTRUDE )
		{
			//todo check id texture or bffer require something
			// 
			//GOL::push_attrib( GL_CURRENT_BIT | GL_ENABLE_BIT | GL_TEXTURE_BIT );
			//GOL::disable_texture_and_force_update_later();
			//GOL::set_texture_2D();

			// to restore opengl texture after draw
			UINT32 gl_name_last = GOL::get_texture_2d(); 
				if( _s_type_used == FONT_TYPE::FONT_TYPE_EXTRUDE )
				{
					if( _render_mode != 0 )	//avoid drawing extruded if no front, side or back 
					{
#if	AAA_BDD_TEXT_RENDER_UNICODE()
						layout->Render( _text.get(), _text.get_len(), ft_point_zero, _render_mode );
#else
						//if( can_use( _text ) )
						//	layout->Render( _text.get(), _text.get_len(), _ft_displacement, _render_mode );
						layout->Render( _buf_wchar, _text.get_len(), ft_point_zero, _render_mode );
#endif
					}
				}
				else
				{
#if	AAA_BDD_TEXT_RENDER_UNICODE()
					layout->Render( _text.get(), _text.get_len(), ft_point_zero );
#else
					//if( can_use( _text ) )	//todo refine
					//	layout->Render( _text.get(), _text.get_len() );
					layout->Render( _buf_wchar, _text.get_len(), ft_point_zero );
#endif				
				}
			GOL::bind_texture_2d( gl_name_last );
			//GOL::pop_attrib();
		}
		else	//	deal with FONT_TYPE_BITMAP or FONT_TYPE_PIXMAP
		{
			GOL::matrix::push();
				GOL::matrix::load_identity();
				c_viewport*	viewport = c_viewport::get_cur();
				gluOrtho2D( 0, viewport->get_sx(), 0, viewport->get_sy() );
				GOL::matrix::set_modelview();
				GOL::matrix::push();
					GOL::matrix::load_identity();
					GOL::raster_pos_2f( _origin[ 0 ] + _offset[0] * _size[0], _origin[ 1 ] + _offset[1] * _size[1] );

#if	AAA_BDD_TEXT_RENDER_UNICODE()
					layout->Render( _text.get(), _text.get_len() );
#else
//						if( can_use( _text ) )
//							layout->Render( _text.get(), _text.get_len() );
					layout->Render( _buf_wchar, _text.get_len() );
#endif	
				GOL::matrix::pop();
				GOL::matrix::set_modelview();
			GOL::matrix::pop();
		}
	}
}

void c_bdd_text::draw()
{
	if( !_b_draw_ready )
		return;

	FP32 tra[3];
	FP32 sca[3];

	cpy_v3( tra, _offset );
	cpy_v3( sca, _size );

	if( _b_fit_ui )
	{
		FP32 s[3];
		sub_v3( s, _bbox_max_out, _bbox_min_out );
		if( _fit_size_ui[0] != 0 && s[0] > _fit_size_ui[0] )
		{
			FP32 f = _fit_size_ui[0] / s[0];
			sca[0] *= f;
			tra[0] *= f;
		}
		if( _fit_size_ui[1] != 0 && s[1] > _fit_size_ui[1] )
			sca[1] *= _fit_size_ui[1] / s[1];
	}
	else
	{
//		FP32 s[3];
//		sub_v3( s, _bbox_max_out, _bbox_min_out );
//		if( _fit_size_ui[0] != 0. )
//			sca[0] *= _fit_size_ui[0] / s[0];;
//		if( _fit_size_ui[1] != 0.  )
//			sca[1] *= _fit_size_ui[1] / s[1];
	}
		
	if( _s_type_used <= FONT_TYPE::FONT_TYPE_GLUT_LINE )	//	OpenGL line Fonts
	{
		if( _s_type_used == FONT_TYPE::FONT_TYPE_MAA_LINE )	// Maa Font
		{
			tra[1] = REAL(-.325) * sca[1];
			sca[0] *= X_FACTOR_MAA_LINE;
		}
		else	// Roman font from Glut
		{
			sca[1] *= Y_FACTOR_GLUT_LINE;
		}
	}
	else if( _s_type_used == FONT_TYPE::FONT_TYPE_SDF )	// Signed distance font
	{
	}
	else if( _ft_font )	// ftgl
	{
		if( _s_type_used <= FONT_TYPE::FONT_TYPE_EXTRUDE )
		{
		}
		else	//	deal with FONT_TYPE_BITMAP or FONT_TYPE_PIXMAP
		{
			clear_v3( tra );
			clear_v3( sca );
		}
	}
	
	GOL::matrix::push();
		GOL::matrix::translate3v( _origin );
		GOL::matrix::rotate_align_on_z( _s_axe );

		draw_bounding_box();

		if( _b_shadow_ui )
		{
			FP32 o[4];
			GOL::push_att();

				scale_v3_cpy_v4( o, _shadow_color_ui );
				o[3] = _shadow_color_ui[3] * GOL::color_cur[3];
				GOL::push_color4v( o );

				if( _s_type_used == FONT_TYPE::FONT_TYPE_SDF )
					scale_v3( o, _shadow_offset_ui, _shadow_offset_ui[3] * sca[1] );
				else
					scale_v3( o, _shadow_offset_ui, _shadow_offset_ui[3] * sca[1] * _face_size );

				add_v3( o, tra );

				GOL::matrix::push();
					GOL::matrix::translate3v( o );
					GOL::matrix::scale3v( sca );
					draw_text();
				GOL::matrix::pop();

			GOL::pop_att();
			
		}

		GOL::matrix::translate3v( tra );
		GOL::matrix::scale3v( sca );
		draw_text();

		//GOL::matrix::push();
		//	tra[0] += .1;
		//	tra[1] -= .1;
		//	GOL::matrix::translate3v( tra );
		//	GOL::matrix::scale3v( sca );
		//	draw_text( tra, sca );
		//GOL::matrix::pop();
	GOL::matrix::pop();

	/*
	if( _b_draw_point_ui )
	{
		REAL* pt;
		if ( c_def_node::get_cur()->is_deforming() )
			pt = _point_deformed;
		else
			pt = _point;
		for( INT32 i = _text.get_len() + 1; i > 0; --i )
		{
			GOL::matrix::push();
			GOL::translatev( tra );
			GOL::scalev( sca );
			n_axe::draw_null_3d();
			GOL::matrix::pop();
			pt += 3;
		}
	}
	*/
//	GOL::disable_texture_and_force_update_later();
}
