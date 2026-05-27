
#ifdef AAA_BDD_VOXEL_H
#error "BDD_VOXEL_H included more than once."
#endif
#define AAA_BDD_VOXEL_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

struct	st_vox;
class	c_img_2d;

class c_color_map final : public c_obj
{
private:
	c_img_2d*	_img;
//	INT32	_dum;
	REAL	_off[2];
	REAL	_fac[2];
public:
			c_color_map();

			void	init();
			void	set_img(	c_img_2d* img );
			void	set_coor(	REAL ou, REAL ov, REAL fu, REAL fv );

	FINLINE	void	get_color_u(	FP32* color,	REAL u	);
	FINLINE	void	get_color_uv(	FP32* color,	REAL u,	REAL v );
};

class	c_bdd_voxel final : public c_bdd_multiple 
{
	FACTORY_DECLARE( c_bdd_voxel, c_bdd_multiple );
private:
	static	INT32 CONST COLOR_NB			=	8;
	static	INT32 CONST COLOR_MAP_NB		=	8;
	st_vox*			_vox;
	INT32*			_index;
	INT32			_render_target;

	bool			_b_box;
	bool			_b_box_force_ui;
	bool			_b_box_disable_ui;
	FP32			_box_min[3];
	FP32			_box_max[3];
	REAL			_box_center_ui[3];
	REAL			_box_range_ui[3];
	bool			_b_box_inverse_ui;

	REAL			_scale[3];
	REAL			_scale_ui[4];
	REAL			_offset_ui[3];

	bool			_b_img_save_trig_ui;
	bool			_b_img_compute_use_alpha_ui;
	bool			_b_img_compute_ui;

	FP32			_col_out[4];

	c_img_2d*		_img;

	bool			_b_color_map;

	bool			_b_swapped;

	c_color_map		_map[COLOR_MAP_NB];
	c_color_map*	_p_map[COLOR_MAP_NB];
	FP32			_color_factor[4];
	FP32			_color_factor_ui[5];

	FP32			_color_offset[4];
	FP32			_color_offset_ui[5];

	FP32			_color_force[3];
	FP32			_color_force_ui[4];

	FP32			_colors[COLOR_NB][4];
	FP32*		_colors_hd[COLOR_NB];

	FP32			_color_img_clear[4];

	//INT32			_point_nb_allocated_ui;
	INT32			_point_nb_allocated;
	INT32			_point_nb_used;

	INT32			_index_nb_used;
	INT32			_index_nb_allocated;

	REAL			_led_size;
	bool			_b_led_sphere_ui;
	INT32			_led_sphere_seg_nb;

	INT32			_s_blend_mode;

//	INT32			_bind_dst_ui;
//	INT32			_bind_dst;
	INT32			_bind_last;

	INT32			_pixel_nb[2];
	INT32			_pixel_nb_ui[2];

	FP32			_color_base_ui[5];
	FP32			_color_base_offset[4];
	FP32			_color_base_factor[4];

	REAL			_min_out[3];
	REAL			_max_out[3];


	FINLINE	void	build_color(	FP32* color,	FP32 CONST t );

	FINLINE	void	render_voxel(	st_vox* vox,	FP32 CONST * CONST color );
	FINLINE	void	render_voxel(	st_vox* vox,	FP32 CONST t );
	FINLINE	void	render_voxel(	st_vox* vox,	FP32 CONST t, FP32 CONST * CONST color_in );

	void	init();
	bool	alloc_point( INT32 nb ); 
	bool	alloc_index( INT32 nb ); 
	void	dealloc();

	c_img_2d*	get_img();
	c_img_2d*	get_img_valid();

	c_color_map*	get_color_map(	INT32 id )	{ return _p_map[ CLAMP( id, 1, COLOR_MAP_NB ) - 1]; }
	FINLINE  FP32 CONST*	build_color_out( st_vox* vox );
public:
	struct st_render_op
	{
		o_str	name;
		INT32	nb_buf;
		INT32	buf[3];
		INT32	nb_arg;
		FP32	args[16];
		C_PCHAR	err;
	};
//	virtual	void	restart();

	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw_single();
	virtual	void	draw_multiple();

	virtual void	erase_points();
	//specialized code
			st_vox*	add_point(			REAL* pos			);
			void	add_point_line_y(	REAL* pos,			REAL dy,		INT32 nb,		INT32 img_col		);
			void	set_line_y(			INT32 index,	bool b_active, bool b_forced	);
//			void	erase_color(		REAL* color			);
			void	render_in_img(		INT32	target		);

			void	begin_render();
			void	end_render();
			void	before_render( bool b_swap );
			void	after_render();

			void	render_plane_axe(	INT32 axe,				REAL beg,		REAL end		);
			void	render_sphere(		REAL CONST* CONST pos,							REAL size,		REAL delta_full,	REAL delta_grad		);
			void	render_boule(		REAL CONST* CONST pos,							REAL size,		REAL inside_cano	);
			void	render_cube(		REAL CONST* CONST pos,							REAL size,		REAL inside_cano	);
			void	render_segment(		REAL CONST* CONST a,	REAL CONST* CONST b,	REAL size,		REAL inside_cano	);
			void	render_cylinder_y(	REAL CONST* CONST pos,							REAL size,		REAL delta_full,	REAL delta_grad		);
			void	render_noise_2d(	FP32 CONST* pos,		FP32 CONST* size,	FP32 offset,	FP32 factor,		FP32 size_change,	bool b_only_slice	);
			void	render_noise_3d(	FP32 CONST* pos,		FP32 CONST* size		);
			void	render_texture(		INT32 color_map_id,		REAL curvature,		REAL offset,	REAL size,			REAL inside_cano	);

			bool	render_op(			st_render_op* op	);		

			void	clear_buffer(		INT32	dst			);
			void	fill_buffer(		INT32	dst,										FP32* color		);
			void	scale_buffer(		INT32	dst,		INT32	src,					FP32	fa			);
			void	scale_buffer(		INT32	dst,		INT32	src,					FP32* color		);
			void	cpy_buffer(			INT32	dst,		INT32	src		);
			void	add_buffer_over(	INT32	dst,		INT32	a,		INT32 b			);
			void	add_buffer(			INT32	dst,		INT32	a,		INT32 b			);
			void	sub_buffer(			INT32	dst,		INT32	a,		INT32 b			);
			void	mul_buffer(			INT32	dst,		INT32	a,		INT32 b			);
			void	min_buffer(			INT32	dst,		INT32	a,		INT32 b			);
			void	max_buffer(			INT32	dst,		INT32	a,		INT32 b			);
			void	mix_buffer(			INT32	dst,		INT32	a,		INT32 b,		REAL fa,			REAL fb	);

			void	rgb_to_hsv(			INT32	dst,		INT32	src		);
			void	hsv_to_rgb(			INT32	dst,		INT32	src		);

	virtual	void	set_color(			INT32 index,		FP32* col	);
	virtual	void	use_color(			INT32* index,		INT32 nb		);

	virtual	void	set_color_map(		INT32 index,		INT32 bind,		REAL u,			REAL v,				REAL fu, REAL fv	);
	virtual	void	use_color_map(		INT32* index,		INT32 nb		);

//	virtual	void	draw_normal_point( REAL len );


	FINLINE	void	transform_pos(		REAL* dst, REAL CONST* CONST  src )		{	mul_add_v3( dst, src, _scale, _offset_ui );	}
	FINLINE	REAL	transform_pos_axe(	REAL coor, INT32 axe )					{	return coor * _scale[axe] + _offset_ui[axe];	}

//	virtual	bool	set_point_by_id(	INT32 id,			REAL* src	);
//	virtual	bool	get_point_by_id(	INT32 id, REAL* dst );
};


