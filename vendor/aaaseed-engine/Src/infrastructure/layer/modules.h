
#ifdef AAA_MODULES_H
#error "MODULES_H included more than once."
#endif
#define AAA_MODULES_H 1

#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef	AAA_AAA_GLOBAL_H
#	include "infrastructure/Data/aaa_global.h"
#endif

class	c_obj_value;
class	c_module;
class	c_layers;
class	c_bind;
class	c_lua_wrap;
class	c_traxs;
class	c_speed;
class	c_speed_ui;
class	c_transfo_trs;
class	c_transfo_three;

class	c_modules final : public c_obj_active_ui
{
	FACTORY_DECLARE( c_modules, c_obj_active_ui );
	static	c_modules*	cur;
	static	c_modules*	ui;
	static	c_modules*	main;
public:
	FINLINE	static	void		set_main( c_modules* modules )	{	main = modules;		}
	FINLINE	static	c_modules*	get_main()						{	return main;		}

	FINLINE	static	void		set_cur( c_modules* modules )	{	cur = modules;		}
//	FINLINE	static	void		set_cur_null()					{	set_cur( nullptr );	}
	FINLINE	static	c_modules*	get_cur()						{	return cur;			}

	FINLINE	static	void		set_ui( c_modules* modules )	{	ui = modules;		}
//	FINLINE	static	void		set_ui_null()					{	set_ui( nullptr );	}
	FINLINE	static	c_modules*	get_ui()						{	return ui;			}
private:
	bool					_b_lua_ui;
	c_lua_wrap*				_lua				{nullptr};

	static	bool	b_traxs_update;
	static	bool	is_traxs_update()		{ return b_traxs_update; }
public:
	static	bool*	get_traxs_update_pt()	{ return &b_traxs_update; }
private:
	c_module**				_hd_module			{nullptr};
	c_bind*					_bind				{nullptr};
	c_var_map<c_module*>	_module_map;

	c_obj_value*			_values				{nullptr};
	c_traxs*				_traxs				{nullptr};
	bool					_b_trax_ui;

	INT32					_module_nb;
//	INT32					_i_module_begin;
	INT32					_i_module_end;
	INT32					_i_module_ui;

	c_transfo_trs*			_transfo_base		{nullptr};
	c_transfo_three*		_transfo_base_three	{nullptr};

	c_speed*				_speed_modules		{nullptr};
	c_speed_ui*				_speed_modules_ui	{nullptr};

	bool					_b_module_param_prefix_always;

	//todo symetrize alloc on all objects
			AAA_ERR		alloc(); 
			void		dealloc();
public:
	virtual	void		prepare_for_ui();
	virtual	void		param_init_pt();
			void		init();

private:
			void		draw_module_low();
			void		draw_module_picking_low();

			void		draw_before();

			void		draw_module();
			void		draw_after();
public:
	virtual	void		draw();

	FINLINE	c_obj_value*	get_values() CONST				{	return _values;	}
	FINLINE	c_traxs*		get_traxs()	CONST				{	return _traxs;	}
	FINLINE	c_bind*			get_module_bind_list() CONST	{	return _bind;	}
//MODULE
			
			
			AAA_ERR		module_alloc();
			void		module_dealloc();
			void		module_init();
	FINLINE	INT32		module_get_nb_max() CONST			{	return _module_nb;	}

			c_module*	module_get_always_from_index( INT32 index );
	FINLINE	c_module*	module_get_from_index( INT32 index ) CONST;
			c_module*	module_get_from_name_short( C_PCHAR_C str );

			void		module_param_update_all();
			void		module_param_update( INT32 i );
			INT32		module_get_index_from_param( p_param param );
			void		module_menu_update();
//			void		module_menu_build( INT32 menu_id, PT_MENU_FN pt_fn );

			bool		swap_module( c_param* to_move, INT32 inc );
			void		module_load_from_file( INT32 i, o_str CONST & filename );
			void		module_load_from_file(	o_str CONST & filename );
			void		module_load_all();
//			void		module_save_states( c_module* pt, CHAR* filename_in );
			void		module_save_to_file(	o_str CONST & filename_in );

			void		module_forget(					INT32 index );
			c_module*	module_new(						INT32 index );
			c_module*	module_new();
			c_module*	module_new_from_file(	o_str CONST & filename_in, INT32 CONST index = -1 );

			INT32		module_ui_get_index() CONST		{	return _i_module_ui; }
			c_module*	module_ui_get();
			void		module_ui_set( INT32 index_in );
			void		module_ui_next();
			void		module_ui_prev();
			void		module_ui_refresh();
			void		module_map_build();

			void		traxs_set_focus();
			void		traxs_start_preroll();
			void		traxs_start_loop( bool b_in );
			void		traxs_stop_loop();
			void		traxs_draw();
			void		traxs_update();

			void		update();

			c_layers*	layers_get_from_name_short( C_PCHAR_C str );

	virtual AAA_ERR		load_do_before(	o_str CONST & filename );
	virtual AAA_ERR		load_do_after(	o_str CONST & filename );
	virtual AAA_ERR		save_do_before(	o_str CONST & filename );
	virtual AAA_ERR		save_do_after(	o_str CONST & filename );

	virtual c_obj_ui*	get_obj_sub_by_index( INT32 CONST index ) CONST;
};

FINLINE	c_module*	c_modules::module_get_from_index( INT32 index ) CONST
{
	if( 0 <= index && index < _module_nb )
	  	return _hd_module[index];
	else
		return nullptr;
}

extern	c_layers*	layers_get_from_name_short( C_PCHAR_C str );

extern	void		ui_sync();


