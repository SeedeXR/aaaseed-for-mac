
#ifdef AAA_AAALUA_MASTER_H
#error "AAALUA_MASTER_H included more than once."
#endif
#define AAA_AAALUA_MASTER_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#ifndef AAA_AAA_MUTEX_H
#	include "aaa/aaa_mutex.h"
#endif

class c_lua_master final : public c_obj_ui
{
	FACTORY_DECLARE( c_lua_master, c_obj_ui );

private:
	aaa::MUTEX_RECURSIVE	_lock;

	bool	_b_save_ui;
	bool	_b_file_check_do_ui;
	bool	_b_reload_all_trig_ui;
	bool	_b_run_always_ui;
	bool	_b_do_lock_ui;
	bool	_b_hook_event_ui;
	bool	_b_verbose_debug_ui;
	bool	_b_verbose_return_ui;
	bool	_b_verbose_aaa_ui;
	bool	_b_dump_stack_ui;
	bool	_b_dump_stack_trig_ui;
	bool	_b_dump_stack_trig_need_clear;
	bool	_b_traceback_ui;
//	bool	_b_compile_trig_dialog_ui;
	bool	_b_error_trig_dialog_ui;
	bool	_b_errmem_skip_dialog_ui;
	bool	_b_error_trig_editor_ui;

	bool	_b_print_trig_dialog_ui;
	bool	_b_print_compile_ui;
	bool	_b_print_pcall_ui;

	INT32	_s_editor_ui;
	o_str	_editor_name_ui;

	INT32	_error_by_frame_ui;
	INT32	_error_total_ui;
	bool	_b_force_recheck;
	bool	_b_gc_do_ui;
	INT32	_gc_step_ui;
	INT32	_gc_step_mul;
	INT32	_gc_step_mul_ui;
	INT32	_gc_pause;
	INT32	_gc_pause_ui;
	REAL	_gc_time_buffer_value_ui;

public:
	FINLINE	void	lock()
	{
		if( is_do_lock() )
			_lock.lock();
	}
	FINLINE	void	unlock()
	{
		if( is_do_lock() )
			_lock.unlock();
	}

			void			init();
	virtual	void			param_init_pt_static();
	virtual void			prepare_for_ui();

	FINLINE	bool			is_reload_all()				CONST		{	return _b_reload_all_trig_ui;		}
	FINLINE	void			reset_reload_all()						{	_b_reload_all_trig_ui = false;		}

	FINLINE	bool			is_save()					CONST		{	return _b_save_ui;					}
	FINLINE	bool			is_file_check()				CONST		{	return _b_file_check_do_ui;			}
	FINLINE	bool			is_run_always()				CONST		{	return _b_run_always_ui;			}
	FINLINE	bool			is_do_lock()				CONST		{	return _b_do_lock_ui;				}
	FINLINE	bool			is_hook_event()				CONST		{	return _b_hook_event_ui;			}
	FINLINE	bool			is_verbose_debug()			CONST		{	return _b_verbose_debug_ui;			}
	FINLINE	bool			is_verbose_return()			CONST		{	return _b_verbose_return_ui;		}
	FINLINE	bool			is_verbose_aaa()			CONST		{	return _b_verbose_aaa_ui;			}
	FINLINE	bool			is_dump_stack()				CONST		{	return _b_dump_stack_ui;			}
	FINLINE	void			set_dump_stack(			bool CONST b )	{	_b_dump_stack_ui = b;				}
	FINLINE	void			disable_dump_stack()					{	set_dump_stack( false );			}
	FINLINE	void			enable_dump_stack()						{	set_dump_stack( true );				}
	FINLINE	bool			is_error_trig_dialog()		CONST		{	return _b_error_trig_dialog_ui;		}
	FINLINE	void			set_error_trig_dialog(	bool CONST b )	{	_b_error_trig_dialog_ui = b;		}
	FINLINE	bool			is_error_trig_editor()		CONST		{	return _b_error_trig_editor_ui;		}
	FINLINE	bool			is_print_trig_error()		CONST		{	return _b_print_trig_dialog_ui;		}
	FINLINE	bool			is_traceback()				CONST		{	return _b_traceback_ui;				}
//	FINLINE	bool			is_compile_trig_dialog()	CONST		{	return _b_compile_trig_dialog_ui;	}
//	FINLINE	bool			set_compile_trig_dialog(bool CONST b )	{	_b_compile_trig_dialog_ui = b;		}
	FINLINE	bool			is_errmem_skip_dialog()		CONST		{	return _b_errmem_skip_dialog_ui;	}
	FINLINE	bool			is_print_compile()			CONST		{	return _b_print_compile_ui;			}
	FINLINE	bool			is_print_pcall()			CONST		{	return _b_print_pcall_ui;			}

	FINLINE	INT32			get_editor()				CONST		{	return _s_editor_ui;				}
	FINLINE	o_str CONST &	get_editor_name()			CONST		{	return _editor_name_ui;				}

	FINLINE	bool			is_force_recheck()			CONST		{	return _b_force_recheck;			}
	FINLINE void			set_force_recheck(		bool CONST b )	{	_b_force_recheck = b;				}
	FINLINE	void			inc_error()								{
																		++_error_by_frame_ui;
																		++_error_total_ui;
																		set_force_recheck( true );
																	}
	FINLINE	INT32			get_error_total_nb()		CONST		{	return _error_total_ui;				}

	virtual AAA_ERR			load_data_from_filename( o_str CONST & filename, INT32 type_io );

			void			update();

			//todo move trigger_edit_file in a better place move also param out of lua_master too
			bool			trig_edit_file( o_str CONST & filename, INT32 CONST line = 0 );	// if neg or null we will not ask the editor to go to a line nb 
};

extern	c_lua_master*	g_lua_master;

FINLINE	void	lua_do_lock()	{	g_lua_master->lock();	}
FINLINE	void	lua_do_unlock()	{	g_lua_master->unlock();	}

