#include "SDFFont.h"
#include "aaa_util.h"
#include "JsonHelper.h"
#include "truetype/OurTrueType.h"
#include "image/bind_img_2d.h"
#include "Unicode.h"
#include <rapidjson/error/en.h>

#include <fstream>
#include <sstream>


// try to use https://msdf-bmfont.donmccurdy.com/ by json parsing crash 
// can use characters set 
//abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ [\]^_`!"#$%&'()*+,-./0123456789:;<=>?@{|}~������������������������������������������������������挜�������

namespace aaa::font
{

namespace sdf
{
	INT32 g_bind_2d_begin;
	INT32 g_bind_2d_end;

	c_sdf_font* sdf_fonts[ FONT_MAX_NB ];
	INT32 bind_free_offset = 0;

	FINLINE INT32 make_index( INT32 CONST font_bind )
	{
		return CLAMP( font_bind, 0, FONT_MAX_NB-1 );
	}

	void	load( INT32 font_bind )
	{
		font_bind = make_index( font_bind );
		sdf_fonts[ font_bind ] = new aaa::font::c_sdf_font;
		o_str CONST & filename = aaa::font::g_bank_def->get_o_str( font_bind );
		if( !sdf_fonts[ font_bind ]->open(filename) )
			ERR_PRINT_STRING( "unable to use sdf json file %s", filename.get() );
	}

	void	deallocate()
	{
		if( aaa::font::g_bank_def )
		{
			for( INT32 i = 0; i < FONT_MAX_NB; ++i )
				SAFE_DELETE( sdf_fonts[i] );
		}
	}

	AAA_ERR	init()
	{
		return AAA_OK;
	}
	void	deinit()
	{
		deallocate();
	}

	c_sdf_font* get( INT32 font_bind )
	{
		font_bind = make_index( font_bind );
		if( sdf_fonts[ font_bind ] == nullptr )
			load( font_bind );
		return sdf_fonts[ font_bind ];
	}

	void erase( INT32 font_bind )
	{
		font_bind = make_index( font_bind );
		SAFE_DELETE( sdf_fonts[font_bind] );
	}


	struct LineProcessor
	{
		LineProcessor( std::vector<std::string> *strings ) : mStrings( strings ) {}
		void operator()( CONST CHAR *line, std::size_t len ) CONST { mStrings->push_back( std::string( line, len ) ); }
		mutable std::vector<std::string> *mStrings = nullptr;
	};

	struct LineMeasure
	{
		LineMeasure( REAL max_width, CONST c_sdf_font::CharToGlyph &chars, CONST std::vector<c_sdf_font::KerningInfo>& kerning )
			: _max_width( max_width ), _chars( chars ), _kerning( kerning )
		{
		}

		BOOL operator()( CONST CHAR *line, size_t CONST len ) CONST
		{
			REAL measured_width = 0.f;
			REAL pen = .0f;
			try
			{
				std::u32string CONST utf32_chars = aaa::text::toUtf32( std::string( line, len ) );
				for( const auto& ch : utf32_chars )
				{
					auto glyph_index_it = _chars.find( static_cast<uint32_t>( ch ) );
					if( _chars.end() == glyph_index_it )
					{
						continue;
					}
					pen += glyph_index_it->second.XAdvance;
					measured_width = pen;
				}
			}
			catch ( ... )
			{
			}
			BOOL result = _max_width > .0 ? ( measured_width <= _max_width ) : TRUE;
			return result;
		}

		REAL										_max_width = 0;
		CONST c_sdf_font::CharToGlyph				&_chars;
		CONST std::vector<c_sdf_font::KerningInfo>	&_kerning;
	};
}	//end namespace sdf


INT32 c_sdf_font::get_bind_from_page( UINT32 CONST page )
{
	if( page < _pages_info.size() )
		return _pages_info[ page ].bind;
	return 0;
}

bool c_sdf_font::open( o_str CONST & filename )
{
	_font_type = FontType::NONE;

	auto ext = filename.get_ext();
	if( str_is_diff_nocase( ext, "json" ) )
	{
		DBG_PRINT_STRING( "No sdf font, don't try to load" );
		return false;
	}

	if( c_file::is_exist( filename ) )
	{
		_font_path = filename;
		o_str fontpath = _font_path;
		fontpath.drop_fname();
		DBG_PRINT_STRING( "fontpath.drop_fname() %s", fontpath.get() );
		//input stream to operate on file
		std::stringstream buffer;
		{
			std::ifstream in( filename.get(), std::ios::in );
			if( in.fail() )
				return false;

			_b_loaded = false;
		
			buffer << in.rdbuf();
			in.close();
		}
		_font_type = FontType::MSDF;
		rapidjson::Document document;
#if 0
		document.Parse( buffer.str().c_str() );
#else
		rapidjson::ParseResult result = document.Parse(buffer.str().c_str());
		if( !result )
		{
			// Handle parse error
			debug_break( "JSON parse error: %s (at offset %zu)\n", rapidjson::GetParseError_En(result.Code()), result.Offset() );
			// Optionally exit or return an error code here
			return false;
		}
#endif
		INT32 member_parsed_nb = 0;
		if( document.HasMember( "info" ) )
		{
			const auto& info = document[ "info" ];
			_face_name                    = aaa::json::get_string( info, "face",               "" );
			_text_height                  = aaa::json::get_fp32(   info, "size",               REAL(0) );
			_ascender                     = aaa::json::get_fp32(   info, "Ascender",           REAL(0) );
			const auto Descender          = aaa::json::get_fp32(   info, "Descender",          REAL(0) );
			_line_height                  = aaa::json::get_fp32(   info, "Height",             REAL(0) );
			const auto MaxAdvanceWidth    = aaa::json::get_fp32(   info, "MaxAdvanceWidth",    REAL(0) );
			const auto MaxAdvanceHeight   = aaa::json::get_fp32(   info, "MaxAdvanceHeight",   REAL(0) );
			const auto UnderlinePosition  = aaa::json::get_fp32(   info, "UnderlinePosition",  REAL(0) );
			const auto UnderlineThickness = aaa::json::get_fp32(   info, "UnderlineThickness", REAL(0) );

			_space_width		          = aaa::json::get_fp32(   info, "SpaceAdvance",		REAL(0) );
			const auto TabAdvance         = aaa::json::get_fp32(   info, "TabAdvance",			REAL(0) );

			_scaling                      = aaa::json::get_fp32(   info, "scaling",				REAL(1) );
			const auto bold               = aaa::json::get_int(    info, "bold",				0 );
			const auto italic             = aaa::json::get_int(    info, "italic",				0 );
			_smooth_pixels                = aaa::json::get_fp32(   info, "smooth",				REAL(0) );
			++member_parsed_nb;
		}
		if( document.HasMember( "common" ) )
		{
			const auto& common = document[ "common" ];
			_base    = aaa::json::get_fp32( common, "base",   REAL(0) );
			_scale_x = aaa::json::get_fp32( common, "scaleW", REAL(0) );
			_scale_y = aaa::json::get_fp32( common, "scaleH", REAL(0) );
			_pages   = aaa::json::get_int(  common, "pages",  0 );
			++member_parsed_nb;
		}
		_pages_info.clear();
		if( document.HasMember( "pages" ) )
		{
			const auto& pages = document[ "pages" ];
			for( rapidjson::SizeType index = 0; index < pages.Size(); ++index )
			{
				const auto& glyph = pages[ index ];
				PageTexInfo pageInfo;
				pageInfo.index    = index;
				pageInfo.width    = aaa::json::get_int(    glyph, "scaleW",   0 );
				pageInfo.height   = aaa::json::get_int(    glyph, "scaleH",   0 );
				pageInfo.filename = aaa::json::get_string( glyph, "filename", "" );
				_pages_info.push_back( pageInfo );
				++member_parsed_nb;
			}
			
		}
		if( document.HasMember( "chars" ) )
		{
			const auto& chars = document[ "chars" ];
			for( rapidjson::SizeType index = 0; index < chars.Size(); ++index )
			{
				const auto& glyph = chars[ index ];
				Metrics metric;

				const auto codepoint = aaa::json::get_int( glyph, "id",	      0 );
				if( codepoint > 0 )
				{
					metric.x		= aaa::json::get_int(  glyph, "x",        0 );
					metric.y		= aaa::json::get_int(  glyph, "y",        0 );
					metric.Width	= aaa::json::get_int(  glyph, "width",    0 );
					metric.Height	= aaa::json::get_int(  glyph, "height",   0 );
					metric.XOffset	= aaa::json::get_fp32( glyph, "xoffset",  REAL(0) );
					metric.YOffset	= aaa::json::get_fp32( glyph, "yoffset",  REAL(0) );
					metric.XAdvance = aaa::json::get_fp32( glyph, "xadvance", REAL(0) );
					metric.Page		= aaa::json::get_int(  glyph, "page",     0 );
					metric.isFlip	= aaa::json::get_bool( glyph, "rotated", false );
					_chars[ codepoint ] = metric;
				}
				++member_parsed_nb;
			}
		}

		if( document.HasMember( "kernings" ) )
		{
			const auto& kernings = document[ "kernings" ];
			for( rapidjson::SizeType index = 0; index < kernings.Size(); ++index )
			{
				const auto& kern = kernings[ index ];
				KerningInfo kerning;

				kerning.First  = aaa::json::get_int( kern, "first",  0 );
				kerning.Second = aaa::json::get_int( kern, "second", 0 );
				kerning.Amount = aaa::json::get_fp32( kern, "amount", REAL(0) );
				_kerning_pairs.push_back( kerning );
				++member_parsed_nb;
			}
		}
		_kern_count = (INT32)_kerning_pairs.size();

		if( member_parsed_nb==0 )
		{
			ERR_PRINT_STRING( "%s() no json member read: %s probably an incompatible json file", __FUNCTION__, filename.get() );
			_b_loaded = false;
		}
		else
			_b_loaded = load_textures();
	}
	return _b_loaded;
}


REAL c_sdf_font::get_kerning_pair( INT32 CONST first, INT32 CONST second )
{
	if( _b_loaded )
	{
		if( _kern_count )
		{
			const auto it = std::find_if( _kerning_pairs.begin(), _kerning_pairs.end(), [first, second]( const KerningInfo& info ) { return ( info.First == first && info.Second == second ); } );
			if( it != _kerning_pairs.end() )
				return ( *it ).Amount;
		}
	}
	return 0.f;
}

std::tuple<REAL, REAL, REAL> c_sdf_font::get_string_width( std::string CONST& string )
{
	std::u32string utf32Chars;
	try
	{
		utf32Chars = aaa::text::toUtf32( string );
	}
	catch( ... )
	{
	}
	return get_string_width( utf32Chars );
}

std::tuple<REAL, REAL, REAL> c_sdf_font::get_string_width( std::u32string CONST & string )
{
	REAL	total		{ .0f };
	REAL	ascender	{};
	REAL	descender	{};
	if( _b_loaded )
	{
		auto const len = string.length();
		for( std::size_t i = 0; i != len; ++i )
		{
			const auto codepoint = static_cast<uint32_t>( string[ i ] );
			const auto bFound = _chars.find( codepoint );
			if( bFound != _chars.end() )
			{
				const auto& glyph = _chars[ codepoint ];
				if( len > 1 && i < len )
					total += get_kerning_pair( codepoint, static_cast<uint32_t>( string[ i + 1 ] ) );

				total += glyph.XAdvance;
				descender = ::MIN( descender, -glyph.YOffset );
				ascender = ::MAX( ascender, glyph.Height - glyph.YOffset );
			}
		}
	}
	return 	std::make_tuple( total, ascender, descender );
}

c_sdf_font::c_sdf_font()
	:_font_type		{FontType::NONE}
	,_line_height	{0}
	,_scaling		{ 1.0f }
	,_text_height	{0}
	,_ascender		{0}
	,_space_width	{0}
	,_smooth_pixels	{0}
	,_base			{0}
	,_scale_x		{1.}
	,_scale_y		{1.}
	,_x_advance		{0}	
	,_pages			{0}
	,_kern_count	{0}
{
}

c_sdf_font::~c_sdf_font()
{
	_chars.clear();
	_kerning_pairs.clear();
}

void c_sdf_font::add_kerning_pair( INT32 CONST first, INT32 CONST second, INT32 CONST amount )
{
	KerningInfo kerning;
	kerning.First	= first;
	kerning.Second	= second;
	kerning.Amount	= static_cast<float>( amount ) / _line_height;
	_kerning_pairs.push_back( kerning );
}

void c_sdf_font::add_char( INT32 CONST id, INT32 CONST x, INT32 CONST y, INT32 CONST w, INT32 CONST h, INT32 CONST xoffset, INT32 CONST yoffset, INT32 CONST xadvance, INT32 CONST page, INT32 CONST chnl )
{
	Metrics chardesc;
	chardesc.x			= x;
	chardesc.y			= y;
	chardesc.Width		= w;
	chardesc.Height		= h;
	chardesc.Sx			= static_cast<float> ( w )        / _line_height;
	chardesc.Sy			= static_cast<float> ( h )        / _line_height;
	chardesc.XOffset	= static_cast<float> ( xoffset )  / _line_height;
	chardesc.YOffset	= static_cast<float> ( yoffset )  / _line_height;
	chardesc.XAdvance	= static_cast<float> ( xadvance ) / _line_height;
	chardesc.Page		= page;
	chardesc.Channel	= chnl;

	_chars.insert( std::unordered_map<int, Metrics>::value_type( id, chardesc ) );
}

std::vector<std::string> c_sdf_font::calculate_line_breaks( std::string CONST text, REAL CONST maxWidth )
{
	std::vector<std::string> result;
	if( _b_loaded )
	{
		try
		{
			std::function<void( CONST CHAR*, size_t )> lineFn = sdf::LineProcessor( &result );
			aaa::text::lineBreakUtf8( text.c_str(), sdf::LineMeasure( maxWidth, _chars, _kerning_pairs ), lineFn );
		}
		catch ( ... )
		{
		}
	}
	return result;
}

bool	c_sdf_font::load_textures()
{
	o_str fontpath = _font_path;
	fontpath.drop_fname();
	INT32 tex_loaded_nb = 0;
	for( auto& page : _pages_info )
	{
		auto CONST bind_cur = sdf::g_bind_2d_begin + sdf::bind_free_offset;
		if( INSIDE( bind_cur, sdf::g_bind_2d_begin, sdf::g_bind_2d_end ) )
		{
			o_str image_path = fontpath;
			image_path.add( page.filename );
			auto CONST ret = g_bind_img_2d->load_texture( bind_cur, image_path, false, true );
			if( ret == AAA_OK )
			{
				page.bind =  bind_cur;
				++sdf::bind_free_offset;
				DBG_PRINT_STRING( "Font Texture loaded %s", image_path.get() );
				++tex_loaded_nb;
			}
			else
			{
				ERR_PRINT_STRING( "Font Texture couldn't be loaded %s", image_path.get() );
				return false;
			}
		}
		else
		{
			ERR_PRINT_STRING( "%s() Can't load font texture in bind %d", __FUNCTION__, bind_cur );
			ERR_PRINT_STRING( "  should be in [%d,%d]:", sdf::g_bind_2d_begin, sdf::g_bind_2d_end );
			ERR_PRINT_STRING( "  see Pref font_sdf_bind_2d_begin/end in group Misc." );
			return false;
		}
	}
	if( tex_loaded_nb==0 )
	{
		ERR_PRINT_STRING( "%s() no textures loaded can be due to json file with no pages infos", __FUNCTION__ );
		return false;
	}
	return true;
}


}	// namespace aaa::font

