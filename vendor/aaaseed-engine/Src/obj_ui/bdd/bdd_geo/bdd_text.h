
#ifdef AAA_BDD_TEXT_H
#error "AAA_BDD_TEXT_H included more than once."
#endif
#define AAA_BDD_TEXT_H 1


#if !defined(AAA_AAA_DEF_H)
#	include "aaa_def.h"
#endif
#if !defined(AAA_BDD_H)
#	include "obj_ui/bdd/util/bdd.h"
#endif
#if !defined(AAA_AAA_FTGL_H)
#	include "ftgl/aaa_ftgl.h"
#endif
#if !defined(AAA_SDFTEXT_H)
#	include "SdfText/SdfText.h"
#endif
#ifndef _UNORDERED_MAP_
#	include <unordered_map>
#endif

//class FTPoint;
#if AAA_NEW_DESIGN()
#	define	AAA_BDD_TEXT_RENDER_UNICODE() 1
#else
#	define	AAA_BDD_TEXT_RENDER_UNICODE() 0
#endif	//AAA_NEW_DESIGN

#if	AAA_BDD_TEXT_RENDER_UNICODE()
#	ifndef _STRING_
#		include <string>
#	endif
#endif

#define	AAA_BDD_TEXT_SDF_CACHE_BY_INSTANCE() 0

class	c_bdd_text final : public c_bdd 
{
	FACTORY_DECLARE( c_bdd_text, c_bdd );
public:
	enum FONT_TYPE : INT32
	{
		FONT_TYPE_MAA_LINE = 0,
		FONT_TYPE_GLUT_LINE,
		FONT_TYPE_TEXTURE,
		FONT_TYPE_BUFFER,
		FONT_TYPE_OUTLINE,
		FONT_TYPE_POLYGON,
		FONT_TYPE_EXTRUDE,
		FONT_TYPE_BITMAP,
		FONT_TYPE_PIXMAP,
		FONT_TYPE_SDF,
		FONT_TYPE_MAX_NB
	};

private:
	bool			_b_draw_ready;
	FP32			_offset[3];

	REAL			_origin[3];
	INT32			_s_axe;

	FONT_TYPE		_s_type_ui;
	FONT_TYPE		_s_type_used;
	INT32			_s_font_ui;
	INT32			_s_font;
	UINT32			_font_unique_id;

	bool			_b_fit_ui;	
	FP32			_fit_size_ui[2];

	REAL			_interline_ui;
	REAL			_interline;
	INT32			_face_size_ui;
	INT32			_face_size;
	INT32			_face_dpi_ui;
	INT32			_face_dpi;

	FP32			_shadow_color_ui[5];
	FP32			_shadow_offset_ui[4];
	bool			_b_shadow_ui;

	bool			_b_generate_trig_ui;
	INT32			_s_generate;
	o_str			_text;

#if	AAA_BDD_TEXT_RENDER_UNICODE()
//	std::string		_string;
#else
	wchar_t*		_buf_wchar;
	INT32			_buf_wchar_size;
	void			alloc_wchar( INT32 CONST size );
#endif

	FTBBox			_bbox;
	bool			_b_draw_bbox_ui;
	bool			_b_draw_box_line_ui;
	FP32			_bbox_min_out[3];
	FP32			_bbox_max_out[3];

	bool			_b_font_changed;

	REAL			_size[3];

//	REAL*			_point;
//	REAL*			_point_deformed;
//	INT32			_point_nb;

//	bool			_b_draw_point_ui;

	INT32			_buffer_id_ui;			// Cache sdftext geometry using unique id (id will be set from lua)
	INT32			_buffer_id;
	INT32			_s_alignment_hori_ui;
	INT32			_s_alignment_vert_ui;
	INT32			_s_text_justification_ui;
	INT32			_s_text_justification;

	INT32			_font_char_map_nb;
	REAL			_line_length_ui;
	REAL			_line_length;
	//REAL			_depth;
	//REAL			_outset;
	REAL			_depth_ui;
	REAL			_outset_ui;
//	bool			_b_use_display_list;
	int				_render_mode;
	bool			_b_render_front_ui;
	bool			_b_render_side_ui;
	bool			_b_render_back_ui;	

	bool			_b_use_display_list_ui;
//	REAL			_displacement_ui[3];
//	FTPoint			_ft_displacement;

	o_str			_font_name;

	
private :
	c_ftfont*		_ft_font;

//	FTSimpleLayout*	_layout;

#if AAA_BDD_TEXT_SDF_CACHE_BY_INSTANCE()
	std::unordered_map< UINT32, aaa::font::c_sdf_text* >	SDF_CACHE;
#endif

	aaa::font::c_sdf_text*									_sdf_text;

			void	init();
	//		void	dealloc_font(	INT32 index		);
			void	dealloc();
			void	set_font( FONT_TYPE	s_type_asked );
			void	draw_bounding_box();
			void	draw_text();
public:
			void	set_text( C_PCHAR_C in );
			void	add_text( C_PCHAR_C in );

//			void	dealloc_point();
//			void	alloc_point(	INT32		nb_in		);
//			void	set_type(		FONT_TYPE	s_type_in	)	{	_s_type	= s_type_in;		}
//			void	set_font(		INT32		s_font_in	)	{	_s_font	= s_font_in;		}
//			void	set_axe(		INT32		s_axe_in	)	{	_s_axe	= s_axe_in;			}
//			void	set_align_hori(	REAL		align_hori_in )	{ _align_hori = align_hori_in;	}

	virtual	void	param_init_pt_static() final override;
	virtual	void	prepare_for_ui() final override;
	virtual bool	can_implicit() final override { return true; }

	virtual	void	draw() final override;
	virtual	void	update() final override;

};
