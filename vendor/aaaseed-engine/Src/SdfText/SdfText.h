#pragma once

#ifdef AAA_SDFTEXT_H
#error "SDFTEXT_H included more than once."
#endif
#define AAA_SDFTEXT_H 1


#if !defined(AAA_AAA_STR_H)
#	include "code_utils/aaa_str.h"
#endif

#if !defined(AAA_SDF_BOX_H)
#	include "sdf_box.h"
#endif


namespace gl
{
	class ibo;
	class c_vao;
}

namespace aaa::font
{
	class c_sdf_font;

	class c_sdf_text
	{
	public:
		struct mesh_gl
		{
			gl::c_vao*	vao	{ nullptr };
			gl::ibo*	ibo	{ nullptr };

			void draw() const;
		};

		enum class TEXT_JUSTIFICATION
		{
			LEFT = 0,
			CENTER,
			JUSTIFY,
			RIGHT
		};

		enum class ALIGNMENT_H : INT32
		{
			CENTER,
			LEFT,
			RIGHT,
		};

		enum class ALIGNMENT_V : INT32
		{
			MIDDLE,
			TOP,
			BOTTOM,
		};

		void		init();
		void		de_init();

		aaa::boxf	update();
		void		draw() CONST;
		void		draw_background() CONST;
		void		draw_lines() CONST;

		void	set_scale( REAL CONST scale ) noexcept { _scale = scale; }
		void	set_line_height( REAL CONST scale ) noexcept
		{
			_b_dirty = _b_dirty || _line_height_scale != scale;
			_line_height_scale = scale;
		}

		void	set_text( std::string_view CONST& text )
		{
			_b_dirty = _b_dirty || _text != text;
			_text = text;
			_text_length = (INT32)_text.length();
		}
		void	set_text( std::string CONST& text )
		{
			_b_dirty = _b_dirty || _text != text;
			_text = text;
			_text_length = (INT32)_text.length();
		}
		void	set_text( o_str CONST & text )
		{
			set_text( std::string_view( text.get(), text.get_len() ) );
		}

		float	get_line_heigth() CONST NOEXCEPT { return _line_height_scale; }
		void	set_align_horizontal( ALIGNMENT_H CONST align ) NOEXCEPT
		{
			_b_dirty = _b_dirty || _alignment_h != align;
			_alignment_h = align;
		}
		void	set_align_vertical( ALIGNMENT_V CONST align ) NOEXCEPT
		{
			_b_dirty = _b_dirty || _alignment_v != align;
			_alignment_v = align;
		}
		void	set_justification( TEXT_JUSTIFICATION CONST align ) NOEXCEPT
		{
			_b_dirty = _b_dirty || _justification != align;
			_justification = align;
		}

		// If line is > than width, then justification is applied
		void	set_line_width( REAL CONST scale ) NOEXCEPT
		{
			_b_dirty = _b_dirty || _max_width != scale;
			_max_width = scale;
		}

		// Set max number of lines
		void	set_line_max_nb( INT32 CONST nb ) NOEXCEPT
		{
			_b_dirty = _b_dirty || _max_lines != nb;
			_max_lines = nb;
		}
		auto	get_line_width() CONST NOEXCEPT { return _max_width; }
		auto	get_line_max_nb() CONST NOEXCEPT { return _max_lines; }
		auto	get_lines_nb() CONST NOEXCEPT { return _lines_nb; }

		void	set_text_scale( FP32 CONST scale ) NOEXCEPT
		{
			_b_dirty = _b_dirty || _text_scale != scale;
			_text_scale = scale;
		}
		auto	get_text_scale() CONST NOEXCEPT { return _text_scale; }
		void	set_text_scale_ar( FP32 CONST scale )
		{
			_b_dirty = _b_dirty || _text_scale_ar != scale;
			_text_scale_ar = scale;
		}
		auto	get_text_scale_ar() CONST NOEXCEPT { return _text_scale_ar; }

		void	set_background_scale( REAL CONST scale ) NOEXCEPT
		{
			_b_dirty = _b_dirty || _scale_back != scale;
			_scale_back = scale;
		}
		auto	get_background_scale() CONST NOEXCEPT { return _scale_back; }
		void	set_background_scale_ar( REAL CONST scale )
		{
			_b_dirty = _b_dirty || _scale_back != scale;
			_scale_back = scale;
		}
		auto	get_background_scale_ar() CONST NOEXCEPT { return _scale_back; }

		void	set_ellipsis( bool CONST b_show ) NOEXCEPT
		{
			_b_ellipsis = b_show;
		}
		auto	is_ellipsis() CONST NOEXCEPT { return _b_ellipsis; }

		void	set_char_spacing( REAL CONST advance ) NOEXCEPT
		{
			_b_dirty = _b_dirty || _char_spacing != advance;
			_char_spacing = advance;
		}

		void	set_line_background_scale( REAL CONST scale ) NOEXCEPT
		{
			_b_dirty = _b_dirty || _scale_back_line != scale;
			_scale_back_line = scale;
		}
		auto	get_line_background_scale() CONST NOEXCEPT { return _scale_back_line; }
		void	set_line_background_scale_ar( REAL CONST scale )
		{
			_b_dirty = _b_dirty || _scale_back_line_ar != scale;
			_scale_back_line_ar = scale;
		}
		auto	get_line_background_scale_ar() CONST NOEXCEPT { return _scale_back_line_ar; }

		void	set_is_one_line( bool CONST b_one ) { _b_one_line = b_one; }

		// Return Bounding Box (including Background) : (minX,MinY,maxX,maxY)
		auto	get_bounding_box()
		{
			if( _b_dirty )
				update();
			return _sdf_vertex_data.bounding_box;
		};

		c_sdf_text();
		~c_sdf_text();

		void	set_font( c_sdf_font* font )
		{
			_b_dirty = _font != font;
			_font = font;
		}

		FINLINE REAL get_line_max_ascender()	{ return _line_max_ascender; }
	private:
		struct LineInfo
		{
			std::vector<std::u32string>	LinesWords;				// vector of words for each line (width constraints can create new lines)
			std::vector<REAL>			WordsWidth;				// word width for each line
			REAL						LinesWidth{};			// width of each line
			bool						LineJustify{ false };	// line needs to be justified
		};

		aaa::font::c_sdf_font*	_font;

		std::string				_font_name;

		ALIGNMENT_H			_alignment_h;
		ALIGNMENT_V			_alignment_v;
		TEXT_JUSTIFICATION		_justification;
		INT32					_max_lines;		// Max number of lines, will show an ellipsis if on and needed
		INT32					_lines_nb;		// Number of lines computed

		REAL					_scaling;
		REAL					_char_spacing;
		REAL					_scale;
		REAL					_line_height_scale;		// Scaling between 2 lines
	//	float					m_Length;
		REAL					_max_width;				// Max width before applying justification
		FP32					_text_scale;			// Scale for height of Text
		FP32					_text_scale_ar;			// Scale Aspect Ratio for Text
		REAL					_scale_back;			// Scale for height of background
		REAL					_scale_back_ar;			// Scale Aspect Ratio for background
		REAL					_scale_back_line;		// Scale for height of background
		REAL					_scale_back_line_ar;	// Scale Aspect Ratio for background

		bool					_b_compute_background;
		bool					_b_compute_lines;
	//	bool					_b_draw_background;
	//	bool					_b_draw_lines;

		bool					_b_dirty;
		bool					_b_ellipsis;
		bool					_b_one_line;
		std::string				_text;

		INT32					_text_length;
		INT32					_vertex_buffer_size;
		INT32					_vertexBufferBackgroundSize;

		REAL					_line_max_width;
		REAL					_line_max_ascender;
		REAL					_line_max_descender;
		bool					_b_is_ellipis_needed;
		INT32					_char_count;



		struct PageData
		{
			UINT32						index			{0xffff};	// page index for this vbo
			UINT16						vertex_count	{0};	// use when computing vertices, need index per page

			UINT32						allocated_count	{0};
			UINT32						char_count		{0};
			FP32*						vertices32		{nullptr};
			FP32*						tex_coords32	{nullptr};
			UINT16*						u16_indices		{nullptr};
			FP32*						p_vertices32	{nullptr};
			FP32*						p_tex_coords32	{nullptr};
			UINT16*						p_u16_indices	{nullptr};
			mesh_gl						mesh;
		};

		struct SDFVertexData
		{
			std::vector<PageData>		pages;

			std::vector<glm::vec3>		back_vertices;
			std::vector<glm::vec2>		back_tex_coords;
			std::vector<std::uint16_t>	back_indices;

			std::vector<glm::vec3>		lines_vertices;
			std::vector<glm::vec2>		lines_tex_coords;
			std::vector<UINT16>			lines_indices;

			boxf						bounding_box{};			// Bounding Box for Text
			boxf						back_bounding_box	{};	// Bounding Box for background
			boxf						lines_bounding_box	{};	// Bounding Box for Lines

			mesh_gl						mesh_back;
			mesh_gl						mesh_lines;

			//gl::c_vao*	vao_background{ nullptr };
			//gl::ibo*		ibo_background{ nullptr };
			//gl::c_vao*	vao_lines{ nullptr };
			//gl::ibo*		ibo_lines{ nullptr };

		};

		SDFVertexData	_sdf_vertex_data;
		SDFVertexData&	get_vertex_data()
		{
			return _sdf_vertex_data;
		}

		PageData&	get_page_data( INT32 CONST index );

		void		create_mesh( std::vector<glm::vec3> CONST&, std::vector<glm::vec2> CONST&, std::vector<UINT16> CONST&, mesh_gl& );
		void		create_mesh( FP32 * vertices, UINT32 CONST vert_nb, FP32 * tex_coords, UINT32 CONST tex_nb, UINT16* indices, UINT32 CONST ind_nb, mesh_gl& mesh );
		void		create_mesh_pages();
		void		create_mesh_background();
		void		create_mesh_lines();

		void		release_mesh( mesh_gl& );
		void		release_mesh_background();
		void		release_mesh_lines();
		void		release_mesh_pages();

		void		clear_geometry();		// clear _sdf_vertex_data
		LineInfo	compute_line_info( std::string CONST& line, REAL CONST scaling, REAL CONST width_space );

		boxf		compute_vertices();
	};

}	//namespace aaa::font
