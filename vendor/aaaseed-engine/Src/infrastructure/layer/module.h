
#ifdef AAA_MODULE_H
#error "MODULE_H included more than once."
#endif
#define AAA_MODULE_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef	AAA_AAA_GLOBAL_H
#	include "infrastructure/Data/aaa_global.h"
#endif

class	c_obj_value;
class	c_lua_wrap;
class	c_traxs;
class	c_bind;
class	c_lights;
class	c_layers;
class	c_seedcam;
class	c_multi_screen;
class	c_materials;
class	c_trax;
class	c_speed;
class	c_speed_ui;

class	c_module final : public c_obj_active_ui
{
	FACTORY_DECLARE( c_module, c_obj_active_ui );
private:
	static	c_module*	cur;
	static	c_module*	ui;
	static	bool		b_force_load_all;
public:
			static	o_str CONST &	make_param_prefix( INT32 index );

	FINLINE	static	void		set_cur( c_module* module )	{	cur = module;		}
	FINLINE	static	void		set_cur_null()				{	set_cur( nullptr );	}
	FINLINE	static	c_module*	get_cur() 					{	return cur;			}

			static	void		set_ui( c_module* module );
	FINLINE	static	void		set_ui_null()				{	set_ui( nullptr );	}
	FINLINE	static	c_module*	get_ui()					{	return ui;			}
public:
	static	bool		b_save_one_file;
	static	bool		b_load_one_file;

private:
	static	bool	b_traxs_update;
	static	bool	is_traxs_update()			{	return b_traxs_update;		}
public:
	static	bool*	get_traxs_update_pt()		{	return &b_traxs_update;		}
	static	bool*	get_force_load_all_pt()		{	return &b_force_load_all;	}
private:
	c_layers**				_hd_layers;

	c_bind*					_bind;
	c_var_map<c_layers*>	_layers_map;

	bool					_b_multiple_only_ui;
	INT32					_id_local_ui;

	bool					_b_multi_screen_ui;
	c_multi_screen*			_multi_screen;
	bool					_b_materials_ui;
	c_materials*			_materials;
	bool					_b_lights_ui;
	c_lights*				_lights;
	c_obj_value*			_values;
	bool					_b_lua_ui;
	INT32					_lua_recursion;
	c_lua_wrap*				_lua;
	c_traxs*				_traxs;
	c_seedcam*				_camera;
	bool					_b_trax_ui;

#if AAA_STATE_COMPILE()
	c_state_master*			_state_control;
	bool					_b_state; 
#endif //AAA_STATE_COMPILE

	bool					_b_load_all_ui;
	bool					_b_loaded_all_out;

	o_str					_comment;

	INT32					_layers_nb;
//	INT32					_i_layers_begin;
	INT32					_i_layers_end;
	INT32					_i_layers_cur;
	o_str					_dir_name;

	bool					_b_used_for_picking_ui;
	bool					_b_flush_at_end_ui;
	bool					_b_finish_at_end_ui;

	bool					_b_time_buffer_include_ui;
	REAL					_time_buffer_value;

	REAL					_camera_rot_ui[3];
	REAL					_camera_sca_ui[3];

	c_speed*				_speed_module;
	c_speed_ui*				_speed_module_ui;

public:
			void	do_hack_new_module()	{	 _b_loaded_all_out = true;	}	// so it can be saved
																			//todo refine creation paths ?
																			//todo what happen when we create from Lua
	//todonow	make it dynamic
	static	CONSTEXPR INT32	LAYERS_NB = 96;

//todo symmetrize alloc on all objects
			AAA_ERR	alloc(); 
			void	dealloc();

	FINLINE	INT32	get_id_local()	CONST		{	return _id_local_ui;	}
			void	set_id_local( INT32 CONST id_local );

//	virtual	void	update();

			void	build_sum_up( o_str& o ) CONST;
	virtual	void	prepare_for_ui();
	virtual	void	param_init_pt();

	FINLINE	void	update();

	FINLINE	bool	is_used_for_picking() CONST	{	return _b_used_for_picking_ui;	}

private:
	FINLINE	void	draw_common();
	FINLINE	void	draw_for_picking_low();
			void	draw_by_multiple_low();

			AAA_ERR	load_do_after_low( o_str CONST & filename_in );

public:
	virtual	void	draw();
	FINLINE	void	draw_for_picking();
	FINLINE	void	draw_by_multiple()	//now check with picking
	{
		if( is_active() )
			draw_by_multiple_low();
	}
	virtual	bool	update_then_draw_lua();
			void	update_then_draw_for_picking();
	virtual	void	update_then_draw();

	FINLINE		c_obj_value*	get_values()							{ return _values; }
	FINLINE		c_seedcam*		get_camera()							{ return _camera; }
	FINLINE		c_bind*			get_layers_bind_list()					{ return _bind; }
//	FINLINE		C_PCHAR_C		get_dir_name() CONST					{ return _dir_name.get(); }	//now dir_name is a hack and should vanish
//	FINLINE		void			set_dir_name( C_PCHAR name_in )			{ _dir_name.set(name_in); }
	FINLINE		void			set_dir_name( o_str CONST & name_in )	{ _dir_name.set(name_in); }

	FINLINE		REAL CONST *	get_camera_rot()	CONST				{ return _camera_rot_ui; }
	FINLINE		REAL CONST *	get_camera_sca()	CONST				{ return _camera_sca_ui; }

//LAYERS
				AAA_ERR		layers_alloc();
				void		layers_dealloc();
//				void		layers_init();

				c_layers*	layers_get_always_from_index(	INT32 index );
	FINLINE		c_layers*	layers_get_from_index(			INT32 index ) CONST;
				c_layers*	layers_get_from_name_short(		C_PCHAR_C str );

				void		layers_param_update(			INT32 i );
				void		layers_menu_update();
				INT32		layers_menu_build(				INT32 menu_id, PT_MENU_FN pt_fn );

				bool		swap_layers(					c_param* to_move, INT32 inc );
//				void		layers_load_from_file(			C_PCHAR_C filename_in);
				void		layers_save_to_file(			o_str CONST & filename_in );

				void		layers_forget(					INT32 index );
				c_layers*	layers_new(						INT32 index );
				c_layers*	layers_new();
				void		layers_new_from_file(			o_str CONST & filename_in );

				INT32		layers_ui_get_index() CONST			{	return _i_layers_cur; }
				c_layers*	layers_ui_get();
				void		layers_ui_set();
				void		layers_ui_set(	INT32 index_in );
				INT32		layers_get_index_from_param( p_param param );

	virtual		void		become_ui();

				void		layers_ui_next();
				void		layers_ui_prev();
				void		layers_ui_refresh();
				void		layers_map_build();


	FINLINE		bool		is_lua() CONST				{	return _b_lua_ui; }

				void		traxs_set_focus();
				void		create_traxs();
	FINLINE		c_traxs*	get_traxs();
				c_trax*		traxs_plug_out(	INT32 index, c_obj_ui* obj, p_param param );
				c_trax*		traxs_plug_in(	INT32 index, c_obj_ui* obj, p_param param );

				void		traxs_start_preroll();
				void		traxs_start_loop( bool b_in );
				void		traxs_stop_loop();
				void		traxs_draw();
				void		traxs_update();

		virtual	AAA_ERR		load_do_before(	o_str CONST &	filename_in );
		virtual	AAA_ERR		load_do_after(	o_str CONST &	filename_in );
		virtual	AAA_ERR		save_do_before(	o_str CONST &	filename_in );
				void		load_states(	bool b_just_rest );
				void		save_states(	o_str CONST &	filename_in );	//	handle the module saving

		//this is a special case (mainly because of current dir)
		virtual AAA_ERR		load_from_file(	o_str CONST &	filename_in );
//		virtual AAA_ERR		save_to_file(	C_PCHAR_C		filename );

		virtual c_obj_ui*	get_obj_sub_by_index( INT32 CONST index ) CONST;

#if AAA_STATE_COMPILE()
				void		state_layers_do_action();
#endif
		virtual	o_str*		get_comment()			{ return &_comment; }
};

FINLINE	c_layers*	c_module::layers_get_from_index( INT32 index ) CONST
{
	if( 0 <= index && index < _layers_nb )
		return _hd_layers[index];
	else
		return nullptr;
}

FINLINE	c_layers*	module_get_layers_from_name_short( C_PCHAR_C str )	//todonow used ?
{
	return c_module::get_cur()->layers_get_from_name_short( str );
}

extern	CHAR	g_str_module_traxs[];


