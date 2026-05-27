
#ifdef AAA_MAP_H
#error "MAP_H included more than once."
#endif
#define AAA_MAP_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif
#ifndef AAA_GOL_MATRIX_H
#	include "gol/gol_matrix.h"
#endif
#ifndef AAA_TEX_H
#	include "tex.h"
#endif

typedef	FP32	UVx4[4][2];
typedef	FP32	UVWx8[8][3];


class	c_bdd_uv;
class	c_seedcam;

class	c_map final : public c_obj_ui
{
	friend	class	c_map_last;
	friend	class	c_layer;
	FACTORY_DECLARE( c_map, c_obj_ui );

	static c_map*	def;
	static c_map*	cur;
	static c_map*	ui;

public:
	FINLINE	static	c_map*	get_def()				{	return def;			}
	FINLINE	static	void	set_def( c_map* map )	{	def = map;			}

	FINLINE	static	c_map*	get_cur()				{	return cur;			}
	FINLINE	static	void	set_cur( c_map* map )	{	cur = map;			}
	FINLINE	static	void	set_cur_def()			{	cur = def;			}

	FINLINE	static	c_map*	get_ui()				{	return ui;			}
	FINLINE	static	void	set_ui( c_map* map )	{	ui = map;			}
	FINLINE	static	void	set_ui_def()			{	ui = def;			}

	static	bool			b_lod_allow_ui;
	static	bool			is_lod_allow()			{	return b_lod_allow_ui;	}

	enum TEX_PROJ : UINT32
	{
		PROJ_OBJECT = 0,
		PROJ_EYE,
		PROJ_SPHERE,
		PROJ_REFLECTION,
		PROJ_NORMAL,
		PROJ_CAMERA,
		PROJ_CAMERA_MODELVIEW,
		TEX_PROJ_NB
	};

	static UVx4 CONST quad_uv_01;
	//no sure this is good
	static UVx4	 CONST quad_uv_01_flipped;
	static UVWx8 CONST box_uvw_01;

private:
	static	bool		b_need_texture_identity;
//MAPPING
	UINT32		_s_tex_use_ui;
	UINT32		_s_tex_use;

	bool		_b_feedback_ui;
	bool		_b_feedback;

	bool		_b_implicit_ui;
//	bool		_b_implicit;

	bool		_b_uv_src_bdd_use_ui;
	o_str		_uv_src_bdd_name_symbo;
	c_bdd_uv*	_uv_src_bdd;
	INT32		_uv_src_bdd_dataset_id;

	bool		_b_tex_hexa_ui;
	REAL		_tex_hexa_delta_ui[2];
	REAL		_tex_hexa_offset_ui[4];

	TEX_PROJ	_s_tex_method;
	INT32		_tex_axe;

	bool		_b_flip_uv_ui;
	bool		_b_mirror_ui[2];

	REAL		_uvw_ori_ui[3];
	//REAL		uv_ori_last_[2];
	REAL		_uvw_ui[3];
	REAL		_uvw_factor_ui;
	REAL		_uvw[3];
	//REAL		uv_last_[2];
	bool		_b_coor_center_ui;

	REAL		_uvw_speed[3];
	REAL		_speed_factor;
	bool		_b_restart_trig_ui;
	REAL		_uvw_phase[3];
	REAL		_uvw_min_max[6];
	c_delta_t	_delta_t;

	bool		_b_by_side_ui;
	REAL		_uvw_side[3][2];

	UINT32		_s_wrap_ui[3];
	REAL		_lod_min_ui;
	REAL		_lod_max_ui;

	bool		_b_force_min_mag_ui;
	UINT32		_s_minification_ui;
	UINT32		_s_magnification_ui;

	bool		_b_rot_base_ui;
	REAL		_rot_base_ui[3];
	REAL		_rot_other_ui[3];
	bool		_b_lookat_ui;
	REAL		_eye[3];
	REAL		_target[3];

	UVx4		_quad_uv;
	UVWx8		_quad_uvw;

	bool		_b_quad_uv_need_compute;
	bool		_b_quad_uvw_need_compute;	//todo implement really

//BLEND
	UINT32		_s_tex_env_mode_ui;
	bool		_b_blend_ui;
	UINT32		_s_blend_equation_ui;
	UINT32		_s_blend_src_ui;
	UINT32		_s_blend_dst_ui;
	bool		_b_blend_separate_ui;
	UINT32		_s_blend_alpha_equation_ui;
	UINT32		_s_blend_alpha_src_ui;
	UINT32		_s_blend_alpha_dst_ui;
	FP32		_blend_color_ui[5];
	UINT32		_s_alpha_test_ui;
	REAL		_alpha_threshold_ui;
	bool		_b_verbose;

	bool		_b_polygon_smooth_ui;
	GOL::HINT	_polygon_smooth_hint_ui;

	FP32		_tex_env[5];

	bool		_b_tex_env_use_ui;
//CAMERA
	UINT32		_s_camera_use_ui;
	o_str		_target_name_symbo;
	INT32		_camera_index;
	c_seedcam*	_camera;

public:
				void			build_sum_up( o_str& o );
		virtual	void			param_init_pt();
		virtual	void			param_init();
		virtual	void			update();
		FINLINE	void			update_blending();

				void			print_verbose();
				void			set_verbose( bool CONST b );
				void			flip_verbose();
//next	done by the "Brain" Object
		FINLINE	bool			is_verbose()			CONST	{	return _b_verbose;					}
//MAP
		FINLINE	bool			is_texture_feedback()	CONST	{	return _b_feedback;					}
		FINLINE	UINT32			get_tex_use_ui()		CONST	{	return this ? _s_tex_use_ui : 0;	}
		FINLINE	UINT32			get_tex_use()			CONST	{	return _s_tex_use;					}
		FINLINE	bool			is_3d()					CONST	{	return ( get_tex_use() == 3 );		} 
		FINLINE	bool			is_2d()					CONST	{	return ( get_tex_use() == 2 );		}
		FINLINE	bool			is_1d()					CONST	{	return ( get_tex_use() == 1 );		}
		FINLINE	bool			is_0d()					CONST	{	return ( get_tex_use() == 0 );		}

static	void					reset_texture_matrix();
		void					set_texture_matrix_base();
		//hack ? 
		FINLINE	bool			is_implicit()			CONST	{	return this ? _b_implicit_ui : false;			}
//				void			update_implicit_full();
//		FINLINE	void			update_implicit()				{	_b_implicit = _b_implicit_ui && !is_0d();	}

		FINLINE	bool			is_tex_hexa()					CONST	{	return _b_tex_hexa_ui;						}
		FINLINE	void			get_hexa_delta(	REAL* dst )		CONST	{	cpy_v2( dst, _tex_hexa_delta_ui );			}
		FINLINE	void			get_hexa_offset( REAL* dst )	CONST	{	scale_v3( dst, _tex_hexa_offset_ui, _tex_hexa_offset_ui[3] );	}

		FINLINE	TEX_PROJ		get_tex_proj()			CONST	{	return _s_tex_method;						}
		FINLINE	bool			is_tex_proj_object()	CONST	{	return _s_tex_method == PROJ_OBJECT;		}
		FINLINE	INT32			get_tex_axe()			CONST	{	return _tex_axe;							}
	
//		FINLINE	bool			is_normal_needed()				{	return !is_0d() && !_b_implicit && ( _s_tex_method == PROJ_SPHERE || _s_tex_method == PROJ_REFLECTION || _s_tex_method == PROJ_NORMAL ); }
		FINLINE	bool			is_normal_needed()		CONST	{	return INSIDE_MIN_MAX( _s_tex_method, PROJ_SPHERE, PROJ_NORMAL ); }

		FINLINE	bool			is_flip_uv()			CONST	{	return _b_flip_uv_ui;	}
		FINLINE	bool			is_mirror_u()			CONST	{	return _b_mirror_ui[0];	}
		FINLINE	bool			is_mirror_v()			CONST	{	return _b_mirror_ui[1];	}
		FINLINE	REAL			get_u_ui()				CONST	{	return _uvw_ui[0];		}
		FINLINE	REAL			get_v_ui()				CONST	{	return _uvw_ui[1];		}
		FINLINE	REAL			get_factor_ui()			CONST	{	return _uvw_ui[2];		}
		FINLINE	REAL			get_u()					CONST	{	return _uvw[0];			}
		FINLINE	REAL			get_v()					CONST	{	return _uvw[1];			}
		FINLINE	REAL			get_u_ori()				CONST	{	return _uvw_ori_ui[0];	}
		FINLINE	REAL			get_v_ori()				CONST	{	return _uvw_ori_ui[1];	}
		FINLINE	REAL			get_w_ori()				CONST	{	return _uvw_ori_ui[2];	}
	//	FINLINE	INT32			get_wrap_u()			CONST	{	return s_wrap[0];		}
	//	FINLINE	INT32			get_wrap_v()			CONST	{	return s_wrap[1];		}

//tex3d check after here
		FINLINE	REAL			get_u_min()				CONST	{	return _uvw_min_max[0];		}
		FINLINE	REAL			get_u_max()				CONST	{	return _uvw_min_max[3];		}
		FINLINE	REAL			get_v_min()				CONST	{	return _uvw_min_max[1];		}
		FINLINE	REAL			get_v_max()				CONST	{	return _uvw_min_max[4];		}
		FINLINE	REAL			get_w_min()				CONST	{	return _uvw_min_max[2];		}
		FINLINE	REAL			get_w_max()				CONST	{	return _uvw_min_max[5];		}

				void			reset_quad_uv(										);
				void			set_quad_uv(					FP32* uv			);
				void			set_quad_uv_from_uv_min_max(	FP32* uv_min_max	);
				void			set_quad_uv_from_uvw(			FP32* uvw_min_max	);
				void			set_quad_uvw_from_uvw(			FP32* uvw_min_max	);

		FINLINE	void			compute_quad_uv()				{	set_quad_uv_from_uvw(  _uvw_min_max );	}
		FINLINE	void			compute_quad_uvw()				{	set_quad_uvw_from_uvw( _uvw_min_max );	}
static	FINLINE	FP32 CONST *	get_quad_uv()					{
																	if( cur )
																	{
																		if( cur->_b_quad_uv_need_compute )
																			cur->compute_quad_uv();
																		return cur->_quad_uv[0];
																	}
																	else
																		return c_map::quad_uv_01[0];
																}
static	FINLINE	FP32 CONST *	get_quad_uvw()					{
																	if( cur )
																	{
																		if( cur->_b_quad_uvw_need_compute )
																			cur->compute_quad_uvw();
																		return cur->_quad_uvw[0];
																	}
																	else
																		return c_map::box_uvw_01[0];
																}
//BLEND
		FINLINE	INT32			get_tex_env_mode()		CONST	{	return _s_tex_env_mode_ui;		}
		FINLINE	bool			is_blend()				CONST	{	return _b_blend_ui;				}
		FINLINE	bool			is_blend_separate()		CONST	{	return _b_blend_separate_ui;	}
		FINLINE	REAL			get_alpha_threshold()	CONST	{	return _alpha_threshold_ui;		}

//MAP
		//	Texture usage
				void	print_tex_use_ui();
				void	set_tex_use_ui( INT32 value_in );
				INT32	inc_tex_use_ui();
				INT32	dec_tex_use_ui();
		//	texture coord
				void	print_flip_uv();
				void	set_flip_uv( bool value_in );
				void	flip_flip_uv();

				void	set_texture_feedback( bool flag );
				void	flip_texture_feedback();

				void	print_mirror_u();
				void	set_mirror_u( bool value_in );
				void	flip_mirror_u();

				void	print_mirror_v();
				void	set_mirror_v( bool value_in );
				void	flip_mirror_v();

				void	print_implicit_ui();
				void	set_implicit_ui( bool b_in );
				void	flip_implicit_ui();

				void	print_tex_mode();
				void	set_tex_mode( INT32 value_in );
				INT32	inc_tex_mode();
				INT32	dec_tex_mode();

				void	print_tex_axe();
				void	set_tex_axe( INT32 value_in );
				INT32	inc_tex_axe();
				INT32	dec_tex_axe();

				void	set_uv( REAL u, REAL v );
				void	set_uvw( REAL CONST* vec );
				void	update_uvw();
				void	tex_draw();

				void	set_uv_ori( REAL u, REAL v );
				void	set_uvw_ori( REAL CONST* vec );

	//	Texture wrap
	//			void	print_wrap();
	//			void	set_wrap( bool b );
	//			bool	flip_wrap();

				void	do_wrap_1d();
				void	do_wrap_2d();
				void	do_wrap_3d();

		FINLINE	void	draw_0d()
				{
					GOL::set_texture_0D();
				}
		FINLINE	void	draw_1d()
				{
					do_wrap_1d();
					//update_implicit();	//todotex
					tex_draw();
				}
		FINLINE	void	draw_2d()
				{
					//todo avoid systematic call ?
					do_wrap_2d();
					//update_implicit();	//todotex
					tex_draw();
				}
		FINLINE	void	draw_3d()
				{
					//todo avoid systematic call ?
					do_wrap_3d();
					//update_implicit();	//todotex
					tex_draw();
				}

//BLEND
	//	Texture environment mode
				//void		print_tex_env_mode();
				//void		set_tex_env_mode( INT32 value_in );
				//INT32		inc_tex_env_mode();
				//INT32		dec_tex_env_mode();
				//	Blend
				void		print_blend();
				void		set_blend( bool b );
				bool		flip_blend();

				void		tex_env_update();

		FINLINE	c_bdd_uv*	get_uv_src_bdd()			CONST	{	return _uv_src_bdd; }
		FINLINE	INT32		get_uv_src_bdd_dataset_id()	CONST	{	return _uv_src_bdd_dataset_id; }
};


class c_map_last
{
private:
	c_bdd_uv*	_bdd_uv;

	bool		_b_flip_uv;
	bool		_tex_mirror_u;
	bool		_tex_mirror_v;
	bool		_b_tex_hexa;

	REAL		_tex_u_min;
	REAL		_tex_u_max;
	REAL		_tex_v_min;
	REAL		_tex_v_max;

	REAL		_tex_hexa_offset[3];
	REAL		_tex_hexa_delta[2];

public:
	void	force_change_state()
	{
		_tex_u_min		=	aaa::QUIET_NAN<REAL>;
		_tex_u_max		=	aaa::QUIET_NAN<REAL>;
		_tex_v_min		=	aaa::QUIET_NAN<REAL>;
		_tex_v_max		=	aaa::QUIET_NAN<REAL>;
		_tex_mirror_u	=	false;
		_tex_mirror_v	=	false;
		_bdd_uv			=	(c_bdd_uv*)-1;
		clear_v3( _tex_hexa_offset );
	}
	c_map_last()
	{
		force_change_state();
	}
	bool is_changed( c_map CONST * map )
	{
		//	we can't use bdd in a finer because there is nothing in bdd to state change
		c_bdd_uv* bdd_uv = map->get_uv_src_bdd();
		if( bdd_uv || _bdd_uv )
		{
			_bdd_uv = bdd_uv;
			return true;
		}
		_bdd_uv = nullptr;

		bool	b_tex_hexa		= map->is_tex_hexa();
		REAL	tex_hexa_offset[3];
		REAL	tex_hexa_delta[3];
		if( b_tex_hexa )
		{
			map->get_hexa_offset(	tex_hexa_offset );
			map->get_hexa_delta(	tex_hexa_delta );
		}

		if(	_b_flip_uv			!=	map->is_flip_uv()
			||	_tex_u_min		!=	map->get_u_min()
			||	_tex_u_max		!=	map->get_u_max()
			||	_tex_v_min		!=	map->get_v_min()
			||	_tex_v_max		!=	map->get_v_max()
			||	_tex_mirror_u	!=	map->is_mirror_u()
			||	_tex_mirror_v	!=	map->is_mirror_v()
			//todo not optimum
			||	_b_tex_hexa		!=	b_tex_hexa
			||	( b_tex_hexa && (is_diff_v3(  _tex_hexa_offset, tex_hexa_offset ) || is_diff_v2(  _tex_hexa_delta, tex_hexa_delta )) )
			)
			{
				return true;
			}
		return false;
	}
	void		store_change(	c_map CONST * map	)
	{
		_b_flip_uv		=	map->is_flip_uv();
		_tex_u_min		=	map->get_u_min();
		_tex_u_max		=	map->get_u_max();
		_tex_v_min		=	map->get_v_min();
		_tex_v_max		=	map->get_v_max();
		_tex_mirror_u	=	map->is_mirror_u();
		_tex_mirror_v	=	map->is_mirror_v();
		_b_tex_hexa		=	map->is_tex_hexa();
		if( _b_tex_hexa )
		{
			map->get_hexa_offset(	_tex_hexa_offset );
			map->get_hexa_delta(	_tex_hexa_delta );
		}
	}
};

