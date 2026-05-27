
#include "SdfText.h"
#include "SDFFont.h"

#include "err.h"
#include "spy.h"
#include "image/bind_img.h"
#include "math/v.h"

#include "gl/ibo.h"
#include "gl/vao.h"
#include "Unicode.h"
#include <sstream>
#include <numeric>
#include "gol/gol_tex.h"


namespace aaa::font {

void c_sdf_text::mesh_gl::draw() const
{
	if( vao && ibo )
	{
		vao->bind();
			ibo->draw_triangles();
		vao->unbind();
	}
}

//// http://stackoverflow.com/questions/236129/how-to-split-a-string-in-c
std::vector<std::string>& split( const std::string& s, const char delim, std::vector<std::string>& elems )
{
	std::stringstream ss( s );
	std::string item;
	while( std::getline( ss, item, delim ) )
	{
		if( !item.empty() )
			elems.push_back( item );
	}
	return elems;
}

std::vector<std::string> split( const std::string& s, const char delim )
{
	std::vector<std::string> elems;
	split( s, delim, elems );
	return elems;
}

// based on
//	https://github.com/Chlumsky/msdfgen

c_sdf_text::c_sdf_text()
	:_font						{nullptr}
	,_font_name					{}
	,_alignment_h				{ ALIGNMENT_H::CENTER }
	,_alignment_v				{ ALIGNMENT_V::MIDDLE }
	,_justification				{ TEXT_JUSTIFICATION::LEFT }
	,_max_lines					{0}			// Max number of lines, will show an ellipsis if on and needed
	,_lines_nb					{0}			// Number of lines computed
	,_scaling					{ 5.f }
	,_char_spacing				{}
	,_scale						{ 1.0f }
	,_line_height_scale			{ 1.0f }	// Scaling between 2 lines
	,_max_width					{}			// Max width before applying justification
	,_text_scale				{ 1.0f }	// Scale for height of Text
	,_text_scale_ar				{ 1.0f }	// Scale Aspect Ratio for Text
	,_scale_back				{ 1.05f }	// Scale for height of background
	,_scale_back_ar				{ 1.0f }	// Scale Aspect Ratio for background
	,_scale_back_line			{ 1.0f }	// Scale for height of background
	,_scale_back_line_ar		{ 1.0f }	// Scale Aspect Ratio for background
	,_b_compute_background		{ false }
	,_b_compute_lines			{ false }
	,_b_dirty					{ true }
	,_b_ellipsis				{ true }
	,_b_one_line				{ false }
	,_text						{}
	,_text_length				{0}
	,_vertex_buffer_size		{0}
	,_vertexBufferBackgroundSize	{0}
	,_line_max_width			{0}
	,_line_max_ascender			{0}
	,_line_max_descender		{0}
	,_b_is_ellipis_needed		{0}
	,_char_count				{0}

{
	init();
}

c_sdf_text::~c_sdf_text()
{
	de_init();
}

void	c_sdf_text::init()
{
}

void 	c_sdf_text::de_init()
{
	clear_geometry();
	//_sdf_vertex_data.pages.clear();
	release_mesh_background();
	release_mesh_lines();
	release_mesh_pages();
	_font = nullptr;
}

aaa::boxf	c_sdf_text::update()
{
	if( _b_dirty )
	{
		if( _text.length() > 0 )
		{
			_sdf_vertex_data.bounding_box = compute_vertices();
			create_mesh_pages();
		}
		else
		{
			_sdf_vertex_data.bounding_box.clear();
		}
		_b_dirty = false;
	}
	return _sdf_vertex_data.bounding_box;
}

c_sdf_text::PageData& c_sdf_text::get_page_data( INT32 CONST index )
{
	for( auto& page : _sdf_vertex_data.pages )
	{
		if( page.index == index )
			return page;
	}
	PageData vboPage;
	vboPage.index = index;
	_sdf_vertex_data.pages.push_back( vboPage );
	return _sdf_vertex_data.pages.back();
}

void	c_sdf_text::clear_geometry()
{
	for( auto& page : _sdf_vertex_data.pages )
	{
		page.vertex_count = 0;
		page.char_count = 0;
		page.p_vertices32   = nullptr;
		page.p_tex_coords32 = nullptr;
		page.p_u16_indices  = nullptr;
		FREE_ALIGNED_AND_NULL( page.vertices32 );
		FREE_ALIGNED_AND_NULL( page.tex_coords32 );
		FREE_ALIGNED_AND_NULL( page.u16_indices );
	}
	_sdf_vertex_data.back_vertices.clear();
	_sdf_vertex_data.back_tex_coords.clear();
	_sdf_vertex_data.back_indices.clear();
	_sdf_vertex_data.lines_vertices.clear();
	_sdf_vertex_data.lines_tex_coords.clear();
	_sdf_vertex_data.lines_indices.clear();
}


c_sdf_text::LineInfo c_sdf_text::compute_line_info( std::string CONST & line, REAL CONST scaling, REAL CONST width_space )
{
	LineInfo	lineInfo;
	const auto	words = split( line, ' ' );
	float		line_length{};
	float		lineAscender{};
	float		lineDescender{};
	bool		isFirstWord{ true };
	try
	{
		for( const auto& word : words )
		{
			std::u32string utf32Chars = aaa::text::toUtf32( word );
			auto [width, ascender, descender] = _font->get_string_width( utf32Chars );
			width *= scaling;
			ascender *= scaling;
			descender *= scaling;
			lineAscender = ::MAX( lineAscender, ascender );
			lineDescender = ::MIN( lineDescender, descender );
			lineInfo.LinesWords.push_back( utf32Chars );
			lineInfo.WordsWidth.push_back( width );
			line_length += ( isFirstWord ? .0f : width_space ) + width;
			isFirstWord = false;
			_char_count += (INT32)utf32Chars.length();
		}
	}
	catch( ... )
	{
	}
	lineInfo.LinesWidth = line_length;
	_line_max_width = ::MAX( _line_max_width, line_length );
	_line_max_ascender = ::MAX( _line_max_ascender, lineAscender );
	_line_max_descender = ::MIN( _line_max_descender, lineDescender );
	lineInfo.LineJustify = true;
	return lineInfo;
}

aaa::boxf	c_sdf_text::compute_vertices()
{
	if( _text_length < 64 )
		SPY_PUSH_RANGE2( "c_sdf_text::compute_vertices", spy::COL_1, _text.c_str() )
	else
		SPY_PUSH_RANGE( "c_sdf_text::compute_vertices", spy::COL_1 )

	aaa::boxf bounding_box;
	if( !_font )
	{
		ERR_PRINT_STRING( "SDF Text : font not set in %s()", __FUNCTION__ );
		return bounding_box;
	}

	//2025 November _char_spacing is always 0 for now
	_font->set_char_advance( _char_spacing * 4.0f );
	const auto scaling_used = 1.f / ( _font->_text_height * _scaling ) / _font->_scaling * 4.0f;	// Use TextHeight to normalize values
	auto widthSpace = ( _font->_space_width ) * scaling_used;

	std::string clean_string;
	try
	{
		clean_string = aaa::text::fix_utf8_string( _text );
	}
	catch(...)
	{
		ERR_PRINT_STRING( "%s() string is not properly encoded", __FUNCTION__ );
		return bounding_box;
	}
	
	std::vector<LineInfo>	lineInfos;
	INT32					index_line = 0;

	if( _b_one_line )
	{
		LineInfo lineInfo = compute_line_info( clean_string, scaling_used, widthSpace );
		lineInfo.LineJustify = false;
		lineInfos.emplace_back( lineInfo );
		++index_line;
	}
	else
	{
		std::stringstream	input( clean_string );
		for( std::string lin; std::getline( input, lin ); )
		{
			auto lines = _font->calculate_line_breaks( lin, _max_width / scaling_used );
			if( lines.empty() )
			{
				LineInfo lineInfo;
				lineInfos.push_back( lineInfo );
				++index_line;
				if( _max_lines > 0 && index_line == _max_lines )
				{
					// generate one more line than n_maxLines, to be sure that an ellipsis is needed
					break;
				}
			}
			else
			{
				for( const auto& line : lines )
				{
					LineInfo lineInfo = compute_line_info( line, scaling_used, widthSpace );
					lineInfos.emplace_back( lineInfo );
					if( _max_lines > 0 && index_line == _max_lines )
					{
						// generate one more line than n_maxLines, to be sure that an ellipsis is needed
						_b_is_ellipis_needed = true;
						break;
					}
					++index_line;
				}
				// Don't justify last line
				lineInfos[ lineInfos.size() - 1 ].LineJustify = false;
			}
		}
	}

	if( _max_lines > 0 && index_line > 0 && index_line == _max_lines )
	{
		// check for ellipsis
		if( _b_ellipsis && _b_is_ellipis_needed )
		{
			const auto lineindex{ index_line - 1 };
			const std::u32string ellipis = aaa::text::toUtf32( "..." );
			auto [widthEllipsis, a2, b2] = ( _font->get_string_width( ellipis ) );
			widthEllipsis *= scaling_used;
			auto& widthline = lineInfos[ lineindex ].LinesWidth;
			auto& widthbreak = lineInfos[ lineindex ].LinesWords;
			auto& widthword = lineInfos[ lineindex ].WordsWidth;

			if( widthline + widthEllipsis > _max_width )
			{
				float	width{ widthline };
				int		index{};
				for( int i = static_cast<int>( widthword.size() ) - 1; i > 0; i-- )
				{
					if( width - widthword[ i ] - widthSpace + widthEllipsis < _max_width )
					{
						index = i;
						widthline = width - lineInfos[ lineindex ].WordsWidth[ i ] - widthSpace + widthEllipsis;
						break;
					}
					width -= widthword[ i ] + widthSpace;
				}
				widthbreak.erase( widthbreak.begin() + index, widthbreak.end() );
				widthword.erase( widthword.begin() + index, widthword.end() );

				if( index > 0 )
				{
					widthbreak[ widthbreak.size() - 1 ].append( ellipis );
					widthword[ widthword.size() - 1 ] += widthEllipsis;
				}
			}
			else
			{
				if( widthbreak.size() > 0 )
				{
					widthline += widthEllipsis;
					widthbreak[ widthbreak.size() - 1 ].append( ellipis );
					widthword[ widthword.size() - 1 ] += widthEllipsis;
				}
			}
		}
	}
	std::size_t		idxBack{};
	int				lineIndex{};

	for( auto& page : _sdf_vertex_data.pages )
	{
		page.vertex_count = 0;
		page.char_count = 0;
	}

	for( const auto& line : lineInfos )
	{
		for( const auto& word : line.LinesWords )
		{
			const auto text_length = static_cast<int>( word.length() );
			for( int i = 0; i != text_length; ++i )
			{
				const auto bFound = _font->_chars.find( static_cast<uint32_t>( word[ i ] ) );
				if( bFound != _font->_chars.end() )
				{
					const auto& f = _font->_chars[ static_cast<uint32_t>( word[ i ] ) ];
					auto& pageVbo = get_page_data( f.Page );
					++pageVbo.char_count;
				}
			}
		}
	}
	for( auto & page : _sdf_vertex_data.pages )
	{
		UINT32 char_nb = page.char_count;
		if( page.allocated_count < char_nb )
		{
			INT32 vertice_nb = char_nb * 4;
			if( vertice_nb > 65536 )
			{	//we should switch to index in 32 bits in this case 
				err_print( "%s() text require more vertices than 65536, expect partial rendering" );
			}
			page.vertices32   = (FP32*)  REALLOC_ALIGNED_SIGNATURE( page.vertices32,   vertice_nb * 3 * sizeof( FP32 ), 16, __FUNCTION__ );
			page.tex_coords32 = (FP32*)  REALLOC_ALIGNED_SIGNATURE( page.tex_coords32, vertice_nb * 2 * sizeof( FP32 ), 16, __FUNCTION__ );
			page.u16_indices  = (UINT16*)REALLOC_ALIGNED_SIGNATURE( page.u16_indices,  char_nb * 6 * sizeof( UINT16 ),  16, __FUNCTION__ );
			page.allocated_count = char_nb;
		}
		page.p_vertices32   = page.vertices32;
		page.p_tex_coords32 = page.tex_coords32;
		page.p_u16_indices  = page.u16_indices;
		page.vertex_count   = 0;
	}

	float	y_ascender = _font->_ascender * scaling_used;
	float	y_height = _font->_line_height * scaling_used;
	y_height = _line_max_ascender - _line_max_descender;
	y_ascender = _line_max_ascender;
	// Real stuff here : compute vertices for each line
	float maxWidth{};
	for( const auto& line : lineInfos )
	{
		float	x_pen{ .0f };
		auto	spaceWidth = _font->_space_width * scaling_used;

		switch( _justification )
		{
		case TEXT_JUSTIFICATION::LEFT:
			break;
		case TEXT_JUSTIFICATION::CENTER:
			x_pen += ( _line_max_width - line.LinesWidth ) * .5f;
			break;
		case TEXT_JUSTIFICATION::JUSTIFY:
			if( line.LineJustify && line.WordsWidth.size() > 1 && lineIndex != line.WordsWidth.size() - 1 )
			{
				const auto words_width = std::accumulate( line.WordsWidth.begin(), line.WordsWidth.end(), .0f );
				spaceWidth = ( _line_max_width - words_width ) / ( line.WordsWidth.size() - 1 );
			}
			else
			{
				//	x += ( maxLineWidth - LinesWidth[ lineIndex ] ) * .5f;
			}
			break;
		case TEXT_JUSTIFICATION::RIGHT:
			x_pen += ( _line_max_width - line.LinesWidth );
			break;
		}

		float		CurX{};	//	= (float) ( _x - center_x ) * .5f;
		float		CurY{};	//	= (float) _y - i * .5f;
		float		DstX{};
		float		DstY{};
		const float	y_top = ( static_cast<float>( -lineIndex ) * _font->_line_height * _line_height_scale ) * scaling_used;
		const float	y_bottom = y_top - y_height;
		const float y_base = y_top - y_ascender;
		_sdf_vertex_data.lines_bounding_box.zero();
		bool		isFirstWord{ true };
		//y= y + LineHeight; //This can be used to flip rendering
		for( const auto& word : line.LinesWords )
		{
			const auto text_length = static_cast<int>( word.length() );
			for( int i = 0; i != text_length; ++i )
			{
				const auto bFound = _font->_chars.find( static_cast<uint32_t>( word[ i ] ) );
				if( bFound != _font->_chars.end() )
				{
					const auto& f = _font->_chars[ static_cast<uint32_t>( word[ i ] ) ];
					CurX = x_pen - ( f.XOffset ) * scaling_used;
					CurY = y_base - ( f.YOffset ) * scaling_used;
					if( f.isFlip )
					{
						DstX = CurX + f.Height * scaling_used;
						DstY = CurY + f.Width * scaling_used;
					}
					else
					{
						DstX = CurX + f.Width * scaling_used;
						DstY = CurY + f.Height * scaling_used;
					}

					const auto pageInfo = _font->get_page_info( f.Page );

					aaa::boxf box;
					box.set( isFirstWord ? ::MIN( CurX, x_pen ) : CurX, y_bottom, DstX, ::MAX( DstY, y_top ) );
					if( isFirstWord )
					{
						_sdf_vertex_data.lines_bounding_box.set( box );
						isFirstWord = false;
					}
					else
					{
						_sdf_vertex_data.lines_bounding_box.include( box );
					}
					bounding_box.include( box );

					auto& pageVbo = get_page_data( f.Page );

					*pageVbo.p_vertices32++ = CurX;
					*pageVbo.p_vertices32++ = CurY;
					*pageVbo.p_vertices32++ = .0f;
					*pageVbo.p_vertices32++ = DstX;
					*pageVbo.p_vertices32++ = CurY;
					*pageVbo.p_vertices32++ = .0f;
					*pageVbo.p_vertices32++ = DstX;
					*pageVbo.p_vertices32++ = DstY;
					*pageVbo.p_vertices32++ = .0f;
					*pageVbo.p_vertices32++ = CurX;
					*pageVbo.p_vertices32++ = DstY;
					*pageVbo.p_vertices32++ = .0f;

					//auto const point_a = glm::vec3( CurX, CurY, .0f );
					//auto const point_b = glm::vec3( DstX, CurY, .0f );
					//auto const point_c = glm::vec3( DstX, DstY, .0f );
					//auto const point_d = glm::vec3( CurX, DstY, .0f );
					auto const height = bounding_box.height();

					//auto tex_a = glm::vec2( .0f, .0f );
					//auto tex_b = glm::vec2( .0f, 1.0f );
					//auto tex_c = glm::vec2( 1.0f, 1.0f );
					//auto tex_d = glm::vec2( 1.0f, .0f );

					const float	tex_adv_x = 1.0f / pageInfo.width;	// Font texture atlas spacing.
					const float	tex_adv_y = 1.0f / pageInfo.height;	// Font texture atlas spacing.
					const auto originX = tex_adv_x * f.x;
					const auto originY = tex_adv_y * f.y;
					const auto topX = tex_adv_x * ( f.x + f.Width );
					const auto topY = tex_adv_y * ( f.y + f.Height );

					if( f.isFlip )
					{
						//tex_a = glm::vec2( topX, originY );
						//tex_b = glm::vec2( topX, topY );
						//tex_c = glm::vec2( originX, topY );
						//tex_d = glm::vec2( originX, originY );

						*pageVbo.p_tex_coords32++ = topX;
						*pageVbo.p_tex_coords32++ = originY;
						*pageVbo.p_tex_coords32++ = topX;
						*pageVbo.p_tex_coords32++ = topY;
						*pageVbo.p_tex_coords32++ = originX;
						*pageVbo.p_tex_coords32++ = topY;
						*pageVbo.p_tex_coords32++ = originX;
						*pageVbo.p_tex_coords32++ = originY;
					}
					else
					{
						*pageVbo.p_tex_coords32++ = originX;
						*pageVbo.p_tex_coords32++ = originY;
						*pageVbo.p_tex_coords32++ = topX;
						*pageVbo.p_tex_coords32++ = originY;
						*pageVbo.p_tex_coords32++ = topX;
						*pageVbo.p_tex_coords32++ = topY;
						*pageVbo.p_tex_coords32++ = originX;
						*pageVbo.p_tex_coords32++ = topY;

						//tex_a = glm::vec2( originX, originY );
						//tex_b = glm::vec2( topX, originY );
						//tex_c = glm::vec2( topX, topY );
						//tex_d = glm::vec2( originX, topY );
					}

					*pageVbo.p_u16_indices++ = pageVbo.vertex_count + 0;
					*pageVbo.p_u16_indices++ = pageVbo.vertex_count + 1;
					*pageVbo.p_u16_indices++ = pageVbo.vertex_count + 3;
					*pageVbo.p_u16_indices++ = pageVbo.vertex_count + 1;
					*pageVbo.p_u16_indices++ = pageVbo.vertex_count + 2;
					*pageVbo.p_u16_indices++ = pageVbo.vertex_count + 3;

					//auto& vertices = pageVbo.vertices;
					//vertices.emplace_back( point_a );
					//vertices.emplace_back( point_b );
					//vertices.emplace_back( point_c );
					//vertices.emplace_back( point_d );
					//auto& tex_coords = pageVbo.tex_coords;
					//tex_coords.emplace_back( tex_a );
					//tex_coords.emplace_back( tex_b );
					//tex_coords.emplace_back( tex_c );
					//tex_coords.emplace_back( tex_d );

					//auto& indices = pageVbo.indices;
					//indices.emplace_back( ( pageVbo.vertex_count + 0 ) );
					//indices.emplace_back( ( pageVbo.vertex_count + 1 ) );
					//indices.emplace_back( ( pageVbo.vertex_count + 3 ) );
					//indices.emplace_back( ( pageVbo.vertex_count + 1 ) );
					//indices.emplace_back( ( pageVbo.vertex_count + 2 ) );
					//indices.emplace_back( ( pageVbo.vertex_count + 3 ) );

					pageVbo.vertex_count += 4;

					//Only check kerning if there is greater then 1 character and if the check character is 1 less then the end of the string.
					if( text_length > 1 && i < text_length )
						x_pen += ( _font->get_kerning_pair( word[ i ], word[ i + 1 ] ) ) * scaling_used;

					x_pen += ( f.XAdvance ) * scaling_used + _char_spacing;
				}
				else
				{
					//LOG_ERROR( "{}() Char {} at pos {} not found", __FUNCTION__, static_cast<uint32_t>( utf32Chars[ i ] ), i );
				}
			}
			x_pen += spaceWidth;
		}
		x_pen -= spaceWidth;
		maxWidth = ::MAX( x_pen, maxWidth );
		if( _b_compute_lines )
		{
			const auto center = _sdf_vertex_data.lines_bounding_box.center();
			const auto newWidth = _sdf_vertex_data.lines_bounding_box.width() * _scale_back_line * .5f;
			const auto newHeight = ( _sdf_vertex_data.lines_bounding_box.height() + ( newWidth * 2.0f - _sdf_vertex_data.lines_bounding_box.width() ) ) * _scale_back_line_ar * .5f;

			const auto newMinY = center.y - newHeight;
			const auto newMaxY = center.y + newHeight;
			const auto newMinX = center.x - newWidth;
			const auto newMaxX = center.x + newWidth;

			auto const point_a = glm::vec3( newMinX, newMinY, .0f );
			auto const point_b = glm::vec3( newMinX, newMaxY, .0f );
			auto const point_c = glm::vec3( newMaxX, newMaxY, .0f );
			auto const point_d = glm::vec3( newMaxX, newMinY, .0f );

			auto const tex_a = glm::vec2( .0f, .0f );
			auto const tex_b = glm::vec2( .0f, 1.0f );
			auto const tex_c = glm::vec2( 1.0f, 1.0f );
			auto const tex_d = glm::vec2( 1.0f, .0f );

			auto& lines_vertices = _sdf_vertex_data.lines_vertices;
			lines_vertices.emplace_back( point_a );
			lines_vertices.emplace_back( point_b );
			lines_vertices.emplace_back( point_c );
			lines_vertices.emplace_back( point_d );

			auto& lines_tex_coords = _sdf_vertex_data.lines_tex_coords;
			lines_tex_coords.emplace_back( tex_a );
			lines_tex_coords.emplace_back( tex_b );
			lines_tex_coords.emplace_back( tex_c );
			lines_tex_coords.emplace_back( tex_d );

			auto& lines_indices = _sdf_vertex_data.lines_indices;
			lines_indices.emplace_back( static_cast<std::uint16_t>( idxBack + 0 ) );
			lines_indices.emplace_back( static_cast<std::uint16_t>( idxBack + 1 ) );
			lines_indices.emplace_back( static_cast<std::uint16_t>( idxBack + 3 ) );
			lines_indices.emplace_back( static_cast<std::uint16_t>( idxBack + 1 ) );
			lines_indices.emplace_back( static_cast<std::uint16_t>( idxBack + 2 ) );
			lines_indices.emplace_back( static_cast<std::uint16_t>( idxBack + 3 ) );
			idxBack += 4;
		}
		if ( _max_lines > 0 && lineIndex >= _max_lines - 1 )
		{
			break;
		}
		++lineIndex;
	}
	auto const center = bounding_box.center();

	// Rescale Text

	if( _text_scale!=FP32(1) || _text_scale_ar!=FP32(1) )
	{
		auto const delta2 = glm::vec3( center.x, center.y, .0f );
		auto const factor = glm::vec3( _text_scale, _text_scale * _text_scale_ar, FP32(1) );
		for( auto& page : _sdf_vertex_data.pages )
		{
			FP32* p_vert = page.vertices32;
			for( INT32 i = 0; i < page.vertex_count; ++i )
			{
				sub_v3( p_vert, &delta2[0] );
				mul_add_v3( p_vert, &factor[0], &delta2[0] );
				p_vert += 3;
			}
		}
		if( _b_compute_lines )
		{
			for( auto& vert : _sdf_vertex_data.lines_vertices )
			{
				vert -= delta2;
				vert *= factor;
				vert += delta2;
			}
		}
		bounding_box.scale_centered( glm::vec2( _text_scale, _text_scale * _text_scale_ar ) );
		if( _b_compute_lines )
			_sdf_vertex_data.lines_bounding_box.scale_centered( glm::vec2( _text_scale, _text_scale * _text_scale_ar ) );
	}

	idxBack = 0;

	if( _b_compute_background )
	{
		_sdf_vertex_data.back_bounding_box = bounding_box;
		_sdf_vertex_data.back_bounding_box.scale_centered( glm::vec2( _scale_back, _scale_back * _scale_back_ar ) );

		auto point_a = glm::vec3( _sdf_vertex_data.back_bounding_box.bottom_left(),		.0f );
		auto point_b = glm::vec3( _sdf_vertex_data.back_bounding_box.top_left(),		.0f );
		auto point_c = glm::vec3( _sdf_vertex_data.back_bounding_box.top_right(),		.0f );
		auto point_d = glm::vec3( _sdf_vertex_data.back_bounding_box.bottom_right(),	.0f );

		auto const tex_a = glm::vec2( .0f, .0f );
		auto const tex_b = glm::vec2( .0f, 1.0f );
		auto const tex_c = glm::vec2( 1.0f, 1.0f );
		auto const tex_d = glm::vec2( 1.0f, .0f );

		auto& back_vertices = _sdf_vertex_data.back_vertices;
		back_vertices.emplace_back( point_a );
		back_vertices.emplace_back( point_b );
		back_vertices.emplace_back( point_c );
		back_vertices.emplace_back( point_d );

		auto& back_tex_coords = _sdf_vertex_data.back_tex_coords;
		back_tex_coords.emplace_back( tex_a );
		back_tex_coords.emplace_back( tex_b );
		back_tex_coords.emplace_back( tex_c );
		back_tex_coords.emplace_back( tex_d );

		auto& back_indices = _sdf_vertex_data.back_indices;
		back_indices.emplace_back( static_cast<std::uint16_t>( idxBack + 0 ) );
		back_indices.emplace_back( static_cast<std::uint16_t>( idxBack + 1 ) );
		back_indices.emplace_back( static_cast<std::uint16_t>( idxBack + 3 ) );
		back_indices.emplace_back( static_cast<std::uint16_t>( idxBack + 1 ) );
		back_indices.emplace_back( static_cast<std::uint16_t>( idxBack + 2 ) );
		back_indices.emplace_back( static_cast<std::uint16_t>( idxBack + 3 ) );
	}
	_lines_nb = (int)( _max_lines > 0 ? ::MIN( lineInfos.size(), _max_lines ) : lineInfos.size() );

	auto delta = glm::vec3( FP32(0) );
	switch( _alignment_v )
	{
	case ALIGNMENT_V::MIDDLE:	delta.y = -bounding_box.center().y;	break;
	case ALIGNMENT_V::BOTTOM:	delta.y = -bounding_box.bottom();	break;
	case ALIGNMENT_V::TOP:		delta.y = -bounding_box.top();		break;
	}
	switch( _alignment_h )
	{
	case ALIGNMENT_H::LEFT:	delta.x = -bounding_box.left();		break;
	case ALIGNMENT_H::CENTER:	delta.x = -bounding_box.center().x;	break;
	case ALIGNMENT_H::RIGHT:	delta.x = -bounding_box.right();	break;
	}

	if( delta.x!=FP32(0) || delta.y!=FP32(0) )
	{
		for( auto& page : _sdf_vertex_data.pages )
		{
			FP32* p_vert = page.vertices32;
			for( INT32 i = 0; i < page.vertex_count; ++i )
			{
				add_v3( p_vert, &delta[0] );
				p_vert += 3;
			}
		}
		if( _b_compute_background )
		{
			for( auto& vert : _sdf_vertex_data.back_vertices )
				vert += delta;
			_sdf_vertex_data.back_bounding_box.translate( delta.x, delta.y );
		}
		if( _b_compute_lines )
		{
			for( auto& vert : _sdf_vertex_data.lines_vertices )
				vert += delta;
			_sdf_vertex_data.lines_bounding_box.translate( delta.x, delta.y );
		}
		bounding_box.translate( delta.x, delta.y );
	}

	if( bounding_box.width() == 0 )
	{
		//LOG_ERROR( "break here" );
	}

	if( _text_length < 64 )
		SPY_POP_RANGE2()
	else
		SPY_POP_RANGE()
	return bounding_box;
}


void	c_sdf_text::release_mesh( mesh_gl& mesh )
{
	gl::c_vao::release_and_null( mesh.vao );
	gl::ibo::release_and_null( mesh.ibo );
}

void	c_sdf_text::release_mesh_background()
{
	release_mesh( _sdf_vertex_data.mesh_back );
}

void	c_sdf_text::release_mesh_lines()
{
	release_mesh( _sdf_vertex_data.mesh_lines );
}

void	c_sdf_text::release_mesh_pages()
{
	for( auto& page : _sdf_vertex_data.pages )
	{
		release_mesh( page.mesh );
	}
}

void	c_sdf_text::create_mesh( std::vector<glm::vec3> CONST& vertices, std::vector<glm::vec2> CONST& tex_coords, std::vector<UINT16> CONST & indices, mesh_gl& mesh )
{
	gl::c_vao_props vao_props;
	//position
	vao_props.add_slot( GOL::VAO_TYPE::VEC3, (void*)vertices.data(), (UINT32)vertices.size(), true );
	// tex coords
	vao_props.add_slot( GOL::VAO_TYPE::VEC2, (void*)tex_coords.data(), (UINT32)tex_coords.size(), true );
	if( mesh.vao )
		mesh.vao->update( vao_props, true );
	else
		mesh.vao = gl::c_vao::make( vao_props, "sdf_text" );

	gl::ibo_props ibo_props( GOL::IBO_TYPE::IBO_UINT16, (UINT32)indices.size(), (void CONST*)indices.data(), true );
	if( mesh.ibo )
		mesh.ibo->update( ibo_props, true );
	else
		mesh.ibo = gl::ibo::make( ibo_props, "sdf_text" );
}


void	c_sdf_text::create_mesh( FP32 * vertices, UINT32 CONST vert_nb, FP32 * tex_coords, UINT32 CONST tex_nb, UINT16* indices, UINT32 CONST ind_nb, mesh_gl& mesh )
{
	gl::c_vao_props vao_props;
	//position
	vao_props.add_slot( GOL::VAO_TYPE::VEC3, vertices, vert_nb, true );
	// tex coords
	//auto vbo_uv = 
	vao_props.add_slot( GOL::VAO_TYPE::VEC2, tex_coords, tex_nb, true );
	if( mesh.vao )
		mesh.vao->update( vao_props, true );
	else
	{
	//	vbo_uv->set_index(2);	// normal is in 1
		mesh.vao = gl::c_vao::make( vao_props, "sdf_text" );
	}


	gl::ibo_props ibo_props( GOL::IBO_TYPE::IBO_UINT16, ind_nb, (void CONST*)indices, true );
	if( mesh.ibo )
		mesh.ibo->update( ibo_props, true );
	else
		mesh.ibo = gl::ibo::make( ibo_props, "sdf_text" );
}

void	c_sdf_text::create_mesh_background()
{
	//release_mesh_background();
	create_mesh( _sdf_vertex_data.back_vertices, _sdf_vertex_data.back_tex_coords, _sdf_vertex_data.back_indices, _sdf_vertex_data.mesh_back );
}

void	c_sdf_text::create_mesh_lines()
{
	//release_mesh_lines();
	create_mesh( _sdf_vertex_data.lines_vertices, _sdf_vertex_data.lines_tex_coords, _sdf_vertex_data.lines_indices, _sdf_vertex_data.mesh_lines );
}

void	c_sdf_text::create_mesh_pages()
{
	//release_mesh_pages();
	for( auto& page : _sdf_vertex_data.pages )
	{
		if( page.vertex_count > 0 )
		{
			create_mesh( page.vertices32, page.char_count * 4, page.tex_coords32, page.char_count * 4, page.u16_indices, page.char_count * 6, page.mesh );
		}
	}
}


void	c_sdf_text::draw_background() CONST
{
	_sdf_vertex_data.mesh_back.draw();
}

void	c_sdf_text::draw_lines() CONST
{
	_sdf_vertex_data.mesh_lines.draw();
}

void	c_sdf_text::draw() CONST
{
	if( _font )
	{
		// to restore opengl texture after draw
		UINT32 gl_name = GOL::get_texture_2d(); 
		for( auto& page : _sdf_vertex_data.pages )
		{
			tex_2d_bind( _font->get_bind_from_page( page.index ) );
			page.mesh.draw();
		}
		GOL::bind_texture_2d( gl_name );
	}
}

}	//namespace aaa::font