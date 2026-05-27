
#ifdef AAA_LAYER_H
#error "LAYER_H included more than once."
#endif
#define AAA_LAYER_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef	_MAP_
#	include <map>
#endif

extern	bool	gb_bbox_see_on_cur;
extern	void	flip_bbox_see_on_cur();

class	c_transfo_three;
class	c_transfo_trs;
class	c_bdd;
class	c_model;
class	c_color;
class	c_fog;
class	c_def_node;
class	c_render;
class	c_map;
class	c_texturing;
class	c_shading;
class	c_stencil;
class	c_clip;
class	c_tex_video;
class	c_lights_switch;
class	c_material;
class	c_tex_anim;
class	c_multiple;

class	c_bdd_mocap;
class	c_speed;
class	c_speed_ui;

class	c_layer final : public c_obj_active_ui
{
	friend c_bdd;	//done for testing only
	FACTORY_DECLARE( c_layer, c_obj_active_ui );
private:
	static	c_layer*	ui;
	static	c_layer*	cur;

	//	we need a stable order to avoid same data generate different files
	struct bdd_less //	: std::binary_function< c_factory_base*, c_factory_base*, bool >
	{
		bool operator() ( const c_factory_base* a,  const c_factory_base* b) const
		{
			return  strcmp( a->get_class_name(), b->get_class_name() ) < 0 ;
		}
	};
	typedef std::map< c_factory_base*, c_bdd*, bdd_less >	bdd_map;
	void load_bdd( c_factory_base* pf, o_str CONST & filename_in );
public:	
	FINLINE	static	void		set_cur(	c_layer* CONST layer )	{	cur = layer;			}
	FINLINE	static	void		set_cur_null()						{	set_cur(	nullptr );	}
	FINLINE	static	c_layer*	get_cur()							{	return cur;				}

	FINLINE	static	void		set_ui(		c_layer* CONST layer )	{	ui = layer;				}
	FINLINE	static	void		set_ui_null()						{	set_ui(		nullptr );	}
	FINLINE	static	c_layer*	get_ui()							{	return ui;				}

	static	bool		b_allow_list;
	static	bool		b_force_list;
	static	bool		b_preload_data;
	static	bool		b_verbose_ui;
private:
//todo reorganize so padding don't fucked up the size of obj_ui
//todo we can use id on all the obj_ui pointer, should we do it ?
	bool			_b_shader_uniform_update_ui;
	bool			_b_shader_bdd_only_ui;
	INT32			_s_bdd_use_ui;
	INT32			_s_bdd;
	INT32			_s_bdd_ui;
	c_bdd*			_bdd;

	bool			_b_rendering_ui;
	c_render*		_render;

	INT32			_s_multiple_ui;
	c_multiple*		_multiple;
	c_multiple*		_multiple_for_bdd;
	c_multiple*		_multiple_prev;

	INT32			_s_texturing_ui;
	c_texturing*	_texturing;

	INT32			_s_shading_ui;
	c_shading*		_shading;
	c_shading*		_shading_last;
	bool			_b_shading_restore;

	bool			_b_mapping_ui;
	c_map*			_map;
	bool			_b_need_uv;

	INT32			_s_stencil_ui;
	c_stencil*		_stencil;

	bool			_b_tex_video_ui;
	c_tex_video*	_tex_video;

	bool			_b_model_ui;
	c_model*		_model;
	c_model*		_model_for_bdd;

	INT32			_s_tex_anim_ui;
	c_tex_anim*		_tex_anim;
	c_tex_anim*		_tex_anim_last;

	bool			_b_color_ui;
	c_color*		_color;
	c_color*		_color_for_bdd;

	bool			_b_material_use_color_ui;
	INT32			_material_use_color_face_ui;
	INT32			_material_use_color_dst_ui;

	bool				_b_lights_switch_ui;
	c_lights_switch*	_lights_switch;

	INT32			_s_fog_ui;
	c_fog*			_fog;

	INT32			_s_deformer_ui;
	c_def_node*		_deformer_ui;

	INT32			_s_clip_ui;
	c_clip*			_clip;

	bool			_b_transfo;
	bool			_b_push;
	bool			_b_pop;

	bool			_b_push_ui;
	bool			_b_pop_ui;
	bool			_b_camera_linked_ui;
	INT32			_s_transfo1_ui;
	INT32			_s_transfo_three_ui;
	INT32			_mocap_channel_ui;
	INT32			_mocap_node_ui;
	INT32			_s_transfo2_ui;

	INT32			_bind_1d_out;
	INT32			_bind_1d_ui;

	INT32			_bind_2d_out;
	INT32			_bind_2d_ui;
	INT32			_bind_2d_offset_ui;
	INT32			_bank_2d_ui;

	INT32			_bind_3d_out;
	INT32			_bind_3d_ui;

	bool			_b_debug_break_ui;

	INT32			_mat_front;
	INT32			_mat_back;
	INT32			_mat_front_ui;
	INT32			_mat_back_ui;

	bdd_map				_bdds;

	c_bdd*				_bdd_prev;

	c_transfo_trs*		_transfo_trs1;
	c_transfo_three*	_transfo_three;
	c_transfo_trs*		_transfo_trs2;
	c_bdd_mocap*		_mocap_direct;

	bool			_b_origin_draw_ui;

//todo remove or clean at least (_b_use_list is never set ...)
	UINT32			_gl_list;
	bool			_b_use_list_ui;
	bool			_b_use_list;

	bool			_b_logic_op_ui;
	INT32			_s_logic_op_ui;

	INT32			_id_local_ui;

	c_def_node*		_def_cur;
	bool			_b_normal_draw;
	bool			_b_normal_needed;

	o_str			_comment;

	c_speed*		_speed_layer;
	c_speed*		_speed_draw;
	c_speed*		_speed_update;
	c_speed_ui*		_speed_layer_ui;
	c_speed_ui*		_speed_draw_ui;
	c_speed_ui*		_speed_update_ui;

private:
	UINT32			_picking_ref_ui;

			void	clear_draw_list();
			void	init_speed();

	FINLINE	void	update_tex_bind();
	virtual	void	update();
	virtual	void	draw();
public:
			void	update_draw_shader_bdd();
	virtual	void	update_then_draw();
	virtual	bool	param_do_action( c_param * CONST par, CONST aaa::param::ACTION action );

//	virtual	void	alloc() {}; 
//	virtual	void	dealloc() {};

			void		set_ui_with_cur();

			void		build_sum_up( o_str& o ) CONST;

	virtual void		param_init_pt_static();
	virtual	void		param_init_pt();
	virtual	void		prepare_for_ui();

	static	void		update_obj_uis();

			void		init();

			void		bdd_set_focus() CONST;

private:
	FINLINE	void		bdd_draw();
	FINLINE	void		bdd_set_by_index( INT32 CONST index );

public:
			void		set_id_local( INT32 CONST id_in );
	FINLINE	INT32		get_id_local()	CONST		{	return _id_local_ui; }

			c_bdd*		bdd_get_always( c_factory_base * CONST pf );

	template<class T>
	FINLINE	T*			bdd_get_always()			{	return (T*) bdd_get_always( &(T::the_factory()) );	} 
			void		bdd_switch_list();
			void		bdd_switch( C_PCHAR_C sel );
			void		bdd_switch( INT32 CONST model_ind );
			void		bdd_switch_add( INT32 CONST i );

			void		bdd_menu_update();

			void		set_bind_1d_ui(	INT32 CONST bind_index );
	FINLINE	INT32		get_bind_1d() CONST							{	return _bind_1d_out;		}

			void		set_bind_2d_ui( INT32 CONST bind_index );
	FINLINE	INT32		get_bank_2d() CONST							{	return _bank_2d_ui;			}
	FINLINE	INT32		get_bind_2d() CONST							{	return _bind_2d_out;		}

			void		set_bind_3d_ui(	INT32 CONST bind_index );
	FINLINE	INT32		get_bind_3d() CONST							{	return _bind_3d_out;		}

	virtual	AAA_ERR		save_to_file(					o_str CONST &	filename_in );
			AAA_ERR		layer_load_from_existing_file(	o_str CONST &	filename, bool b_load_all );
	virtual	AAA_ERR		load_from_existing_file(		o_str CONST &	filename );

			void		info_str_make(		CHAR* CONST str ) CONST;
			void		info_str_make_bis(	CHAR* CONST str ) CONST;

	FINLINE	INT32		get_bdd_selector()		CONST				{	return _s_bdd;			}
	FINLINE	c_bdd*		get_bdd()				CONST				{	return _bdd;			}
	FINLINE	void		set_bdd(			c_bdd* CONST bdd_in )	{	_bdd = bdd_in;		}	//todo check index needed ?
			void		set_bdd_external(	c_bdd* CONST bdd_in );
	FINLINE	c_bdd*		get_bdd_prev()			CONST				{	return _bdd_prev;		}
	FINLINE	bool*		get_b_color_pt()							{	return &_b_color_ui;	}
	FINLINE	bool*		get_b_model_pt()		 					{	return &_b_model_ui;	}
	FINLINE	INT32*		get_s_multiple_pt()							{	return &_s_multiple_ui;	}
	FINLINE	INT32*		get_mat_front_ui_pt()						{	return &_mat_front_ui;	}
	FINLINE	INT32*		get_mat_back_ui_pt()						{	return &_mat_back_ui;	}
			c_material*	get_material_front()	CONST;
			c_material*	get_material_back()		CONST;
	
	c_render*			get_render();
	c_map*				get_mapping();
	c_texturing*		get_texturing();
	c_shading*			get_shading();
	c_stencil*			get_stencil();
	c_clip*				get_clip();
	c_tex_video*		get_tex_video();
	c_model*			get_model();
	c_multiple*			get_multiple();
	c_transfo_trs*		get_transfo_trs1();
	c_transfo_trs*		get_transfo_trs2();
	c_def_node*			get_deformer()			CONST;
	c_color*			get_color();

	//void				set_render(    c_render*	CONST render	);
	//void				set_mapping(   c_map*		CONST mapping	);
	//void				set_shading(   c_shading*	CONST shading	);
	//void				set_texturing( c_texturing* CONST texturing );

	FINLINE	UINT32		get_picking_ref()					{	return _picking_ref_ui; }

	virtual	o_str*		get_comment()						{	return &_comment;							}
	FINLINE	c_color*	get_color_for_bdd()		CONST		{	return _color_for_bdd;						}
	FINLINE	c_model*	get_model_for_bdd()		CONST		{	return _model_for_bdd;						}
	FINLINE	c_multiple*	get_multiple_for_bdd()	CONST		{	return _multiple_for_bdd;					}
	FINLINE	bool		is_normal_draw()		CONST		{	return this ? _b_normal_draw : false;		}
	FINLINE	bool		is_normal_needed()		CONST		{	return this ? _b_normal_needed : false;		}

	FINLINE	void		set_picking_ref( UINT32 CONST in )	{	_picking_ref_ui = in;						}
	FINLINE	bool		is_camera_linked()		CONST		{	return this ? _b_camera_linked_ui : false;	}
	FINLINE	bool		is_need_uv()			CONST		{	return this ? _b_need_uv: false;			}

	virtual	void		become_ui();
};

extern	void	obj_focus_update();

