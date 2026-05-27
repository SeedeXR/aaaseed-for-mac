
#ifdef AAA_RENDER_H
#error "RENDER_H included more than once."
#endif
#define AAA_RENDER_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef	AAA_MULTIPLE_H
#	include "draw/multiple.h"
#endif


class	c_render;
//extern	c_render*	render_last;

//todo apply brain object here
class	c_render final : public c_obj_ui
{
	friend	class	c_layer;
	FACTORY_DECLARE(c_render,c_obj_ui);
//BASE
private:
	static	c_render*	def;
	static	c_render*	cur;
	static	c_render*	ui;
public:
	CONSTEXPR static	c_render*	get_def()					{	return def;	}
	CONSTEXPR static	void		set_def( c_render* ren )	{	def = ren;	}
	CONSTEXPR static	c_render*	get_cur()					{	return cur;	}
	CONSTEXPR static	void		set_cur( c_render* ren )	{	cur = ren;	}
	CONSTEXPR static	c_render*	get_ui()					{	return ui;	}
	CONSTEXPR static	void		set_ui( c_render* ren )		{	ui = ren;	}

private:
	bool	_b_clockwise_ui;
	INT32	_s_cull_ui;
	INT32	_s_front_mode_ui;
	INT32	_s_back_mode_ui;

	bool	_b_line_smooth_ui;
	REAL	_line_size;
	bool	_b_point_smooth_ui;
	REAL	_point_size;
	INT32	_point_mode;
	bool	_point_sprite_tex_ui;
	UINT32	_point_sprite_origin;

	bool	_b_light_ui;
	bool	_b_draw_using_normal_ui;
	bool	_b_light;
	bool	_b_gouraud_ui;
	INT32	_s_draw_primitive_ui;

	bool	_b_depth_test_ui;
	INT32	_s_depth_test_ui;
	bool	_b_depth_write_ui;

	INT32	_random_on_color;
	INT32	_random_on_color_store;

	bool	_b_multisample_ui;
//TOP
	bool	_b_top_line_ui;
	INT32	_s_top_line_cull;
	bool	_b_top_point_ui;
	INT32	_s_top_point_cull;
	FP32	_top_offset_factor_ui;
	FP32	_top_offset_units_ui;
	FP32	_top_color_ui[5];
//NORMAL
	bool	_b_top_normal_ui;
	REAL	_normal_len_point;
//	REAL	_normal_len_edge;
	REAL	_normal_len_face;
	bool	_b_normal_textured_ui;
	FP32	_normal_color_ui[5];

	bool	_b_top_draw;
	bool	_b_top_normal_draw;
//	REAL	_quad_normal_pull_ui;
//	REAL	_quad_normal_pull;
//	bool	_quad_normal_flip_ui;
//	REAL	_quad_normal_axe;
//	REAL	_quad_normal[4][3];

	static	bool	b_verbose;


protected:
	virtual	void	update_low();

public:
	static	void	begin();
	static	void	end();

	static	void	c_init();
	static	void	c_deinit();

	virtual	INT32	get_file_version_save() CONST;

			void	build_sum_up( o_str& o );
	//todo refine_draw_sump_up to do it by param
	//virtual	bool	is_draw_sum_up()			{ return true; }
	//virtual	void	draw_sum_up();

	virtual	void	param_init_pt();

	CONSTEXPR void	update()
		{
//			if( this != render_last )	//todoopt this was not enough we need a mecanism which detect change in obj or param
			{
				update_low();
//				render_last = this;
			}
		}
//	void	print_verbose();
//	void	set_verbose(bool CONST b);
//	void	flip_verbose();

//next	done by the "Brain" Object
//	CONSTEXPR bool		is_verbose()			CONST	{	return verbose; }
//BASE
	CONSTEXPR bool		is_clockwise()			CONST	{	return _b_clockwise_ui;		}
	CONSTEXPR INT32		get_cull()				CONST	{	return _s_cull_ui;			}
	CONSTEXPR INT32		get_front_mode()		CONST	{	return _s_front_mode_ui;	}
	CONSTEXPR INT32		get_back_mode()			CONST	{	return _s_back_mode_ui;		}

	CONSTEXPR bool		is_line_smooth()		CONST	{	return _b_line_smooth_ui;	}
	CONSTEXPR REAL		get_line_size()			CONST	{	return _line_size;			}
	CONSTEXPR bool		is_point_smooth()		CONST	{	return _b_point_smooth_ui;	}
	CONSTEXPR REAL		get_point_size()		CONST	{	return _point_size;			}

	CONSTEXPR bool		is_light()				CONST	{	return _b_light;				}
	CONSTEXPR bool		is_gouraud()			CONST	{	return _b_gouraud_ui;			}
	CONSTEXPR INT32		get_draw_primitive()	CONST	{	return _s_draw_primitive_ui;	}

	CONSTEXPR bool		is_draw_need_normal()	CONST	{	return is_light() || _b_draw_using_normal_ui;	}
	CONSTEXPR bool		is_depth_test()			CONST	{	return _b_depth_test_ui;	}
	CONSTEXPR INT32		get_random_on_color()	CONST	{	return _random_on_color;	}
	CONSTEXPR bool		is_multisample()		CONST	{	return _b_multisample_ui;	}
//TOP
	CONSTEXPR bool		is_top_draw()			CONST	{	return _b_top_draw;			}
	CONSTEXPR bool		is_top_normal_draw()	CONST	{	return _b_top_normal_draw;	}

	CONSTEXPR FP32 CONST *	get_top_color()		CONST	{	return _top_color_ui;						}	
	CONSTEXPR bool		is_top()				CONST	{	return _b_top_line_ui || _b_top_point_ui;	}
	CONSTEXPR bool		is_top_line()			CONST	{	return _b_top_line_ui;						}
	CONSTEXPR bool		is_top_point()			CONST	{	return _b_top_point_ui;						}

	CONSTEXPR REAL		get_normal_len_point()	CONST	{	return _normal_len_point;	}
//	CONSTEXPR REAL		get_normal_len_edge()	CONST	{	return _normal_len_edge;	}
	CONSTEXPR REAL		get_normal_len_face()	CONST	{	return _normal_len_face;	}
	CONSTEXPR bool		is_normal_needed()		CONST	
						{	return	is_draw_need_normal()
									|| is_top_normal_draw()
									|| c_multiple::cur->is_align_normal();
						}

//next	done by the "Brain" Object
//BASE
	//	Front
			void	print_front_mode();
			void	set_front_mode( INT32 CONST i );
			INT32	inc_front_mode();
			INT32	dec_front_mode();
	//	Back
			void	print_back_mode();
			void	set_back_mode( INT32 CONST i );
			INT32	inc_back_mode();
			INT32	dec_back_mode();
	//	Line Smooth
			void	print_line_smooth();
			void	set_line_smooth( bool CONST b_in );
	//	Line Size
			void	print_line_size();
			void	set_line_size( REAL CONST size_in );
	//	Point Size
			void	print_point_size();
			void	set_point_size( REAL CONST size_in );
	//	Culling
			void	print_cull();
			void	set_cull( INT32 CONST i );
			INT32	inc_cull();
			INT32	dec_cull();

	//	Gouraud
			void	print_gouraud();
			void	set_gouraud ( bool CONST b );
			bool	flip_gouraud();
	//	Base Primitive
			void	print_draw_primitive();
			void	inc_draw_primitive();
			void	dec_draw_primitive();
	
	//	Light
			void	print_light();
			void	set_light( bool CONST b );
			bool	flip_light();
	//	ClockWise
			void	print_clockwise();
			void	set_clockwise( bool CONST b );
			bool	flip_clockwise();
	//	Depth
			void	print_depth();
			void	set_depth( bool CONST b );
			bool	flip_depth();
//TOP
	//	Top
			void	begin_top();
			void	end_top();
			void	begin_top_line();
			void	end_top_line();
			void	begin_top_point();
			void	end_top_point();
	//	Line_on_top
			void	print_top_line();
			void	set_top_line( bool CONST b );
			bool	flip_top_line();
	//	Point_on_top
			void	print_top_point();
			void	set_top_point( bool CONST b );
			bool	flip_top_point();

	//	Normal
			void	normal_begin();
			void	normal_end();
	//	Normal_point
			void	print_normal_len_point();
			void	set_normal_len_point( REAL CONST b );
			REAL	flip_normal_len_point();
	//	Normal_edge
	//		void	print_normal_len_edge();
	//		void	set_normal_len_edge( REAL b );
	//		REAL	flip_normal_len_edge();
	//	Normal_face
			void	print_normal_len_face();
			void	set_normal_len_face( REAL CONST b );
			REAL	flip_normal_len_face();
	//	quad_normal	
//	void	c_render::quad_normal_build( REAL *n, INT32 quad_axe)
//			REAL*	quad_normal_update( INT32 quad_normal_axe_in, bool quad_normal_flip_in );

	//	Random_on_color
			void	print_random_on_color();
			void	set_random_on_color( INT32 CONST in );
			INT32	dec_random_on_color();
			INT32	inc_random_on_color();
};

