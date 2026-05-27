
#ifdef AAA_AAALUA_WRAP_H
#error "AAALUA_WRAP_H included more than once."
#endif
#define AAA_AAALUA_WRAP_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif


class c_trax;
class c_lua_state;

class c_lua_wrap final : public c_obj_ui
{
	FACTORY_DECLARE( c_lua_wrap, c_obj_ui );
private:
//	static	bool			g_force_recheck;

			bool			_b_doit_ui;
			bool			_b_doit_trig_ui;
			bool			_b_file_check_ui;
			bool			_b_file_check_always_ui;
			bool			_b_file_reload_trig_ui;
			bool			_b_compiled_out;
			bool			_b_run_only_when_compiled_ui;
			bool			_b_skip_rest_ui;
			UINT32			_check_file_counter;
			time_t			_file_time;
			o_str			_text_ui;
			o_str			_lua_fn_name;
			o_str			_script_debug_name;
			o_str			_script_name_ui;
			o_str			_script_filename;
			bool			_b_edit_trig_ui;
			c_lua_state*	_l;
			bool			_b_text_direct;

private:

			void			compile();
			void			doit();
			void			set_text(			C_PCHAR_C str )		{	_text_ui.set(str); _b_compiled_out = false;	}
			void			set_text_direct(	bool CONST b )		{	_b_text_direct = b; if(b) _script_debug_name.set(_script_filename);	}
public:
	static	void			c_init();
	static	void			c_deinit();
	static	void			do_text(			C_PCHAR_C str,	C_PCHAR_C name );

	static	void			push_caller(		c_obj_ui* CONST obj );
	static	void			pop_caller();
	static	c_obj_ui*		get_caller();

			void			init();
	virtual	void			param_init_pt();
	virtual	bool			param_do_action(	c_param * CONST par,		CONST aaa::param::ACTION action );

			void			update();

			void			do_str(				C_PCHAR_C str, C_PCHAR_C name );

			void			set_do()									{	_b_doit_ui = true;						}
			void			set_do_trig()								{	_b_doit_trig_ui = true;					}
			void			set_file_check()							{	_b_file_check_ui = true;				}

			bool			make_script_filename( o_str CONST & name_in );
			void			set_script_filename_external( o_str CONST & name_in );
			void			set_script_filename( C_PCHAR_C str )		{	_script_filename.set( str );			}

			o_str&			get_text()									{	return _text_ui;						}
			o_str&			get_script_name()							{	return _script_name_ui;					}
			o_str&			get_script_filename()						{	return _script_filename;				}
			C_PCHAR_C		get_script_filename_str()					{	return _script_filename.get();			}
			o_str CONST &	get_script_debug_name()						{	return _script_debug_name;				}
			C_PCHAR_C		get_script_debug_name_str()					{	return _script_debug_name.get();		}


			bool			is_text_direct()							{	return _b_text_direct;					}
			
//			void			load_lua( CHAR* str );
//			void			draw();
	FINLINE	c_lua_state*	get_lua_state() { return _l; }

	FINLINE	void			get_trax_input( c_trax* CONST t );
			REAL			do_trax(		c_trax* CONST t,	C_PCHAR_C fn_name );
	FINLINE	bool			is_skip_rest()								{	return _b_skip_rest_ui;					}
private:
	FINLINE	bool			put_fn_on_stack(									C_PCHAR   fn_name );
	FINLINE	bool			put_table_fn_on_stack(		C_PCHAR_C table_name,	C_PCHAR_C fn_name );
	FINLINE	bool			do_fn_central(				c_obj_ui* CONST caller,	C_PCHAR_C fn_c_name,						C_PCHAR_C fn_name,	INT32 CONST nb_in,	INT32 CONST nb_out	);
	FINLINE	bool			do_table_fn_central(		c_obj_ui* CONST caller,	C_PCHAR_C fn_c_name, C_PCHAR_C table_name,	C_PCHAR_C fn_name,	INT32 CONST nb_in,	INT32 CONST nb_out	);
	FINLINE	bool			do_table_fn_central(		INT32 CONST nb_in,	INT32 CONST nb_out	);
public:
			bool			exits_function_by_name(		C_PCHAR_C fn_name );
			INT32			get_table_fn_as_ref(		C_PCHAR_C table_name, C_PCHAR_C fn_name );

	//	these name are build this way : _fn_ outputs _pass_ inputs 
			void			do_fn_pass_i(				c_obj_ui* CONST t,							C_PCHAR_C fn_name,	INT32		CONST a	);
			void			do_fn_pass_r(				c_obj_ui* CONST t,							C_PCHAR_C fn_name,	REAL		CONST a	);
			void			do_fn_pass_d(				c_obj_ui* CONST t,							C_PCHAR_C fn_name,	DOUBLE		CONST a	);
			void			do_fn_pass_bb(				c_obj_ui* CONST t,	C_PCHAR_C table_name,	C_PCHAR_C fn_name,	bool		CONST a,	bool		CONST b		);
			void			do_fn_pass_di(				c_obj_ui* CONST t,							C_PCHAR_C fn_name,	DOUBLE		CONST a,	INT32		CONST b		);
			void			do_fn_pass_si(				c_obj_ui* CONST t,							C_PCHAR_C fn_name,	C_PCHAR_C	CONST a,	INT32		CONST b		);
			void			do_fn_pass_is(				c_obj_ui* CONST t,							C_PCHAR_C fn_name,	INT32		CONST a,	C_PCHAR_C	b			);
			void			do_fn_pass_ii(				c_obj_ui* CONST t,							C_PCHAR_C fn_name,	INT32		CONST a,	INT32		CONST b		);
			void			do_fn_pass_ii(				c_obj_ui* CONST t,	C_PCHAR_C table_name,	C_PCHAR_C fn_name,	INT32		CONST a,	INT32		CONST b		);
			void			do_fn_pass_isi(				c_obj_ui* CONST t,							C_PCHAR_C fn_name,	INT32		CONST a,	C_PCHAR_C	b,			INT32 CONST c );
			bool			do_fn_b_pass_isi(			c_obj_ui* CONST t,							C_PCHAR_C fn_name,	INT32		CONST a,	C_PCHAR_C	b,			INT32 CONST c );
			void			do_fn_pass_isd(				c_obj_ui* CONST t,							C_PCHAR_C fn_name,	INT32		CONST a,	C_PCHAR_C	b,			DOUBLE CONST c );
			bool			do_fn_b_pass_isd(			c_obj_ui* CONST t,							C_PCHAR_C fn_name,	INT32		CONST a,	C_PCHAR_C	b,			DOUBLE CONST c );
			void			do_fn_pass_iss(				c_obj_ui* CONST t,							C_PCHAR_C fn_name,	INT32		CONST a,	C_PCHAR_C	b,			C_PCHAR_C c );
			bool			do_fn_b_pass_iss(			c_obj_ui* CONST t,							C_PCHAR_C fn_name,	INT32		CONST a,	C_PCHAR_C	b,			C_PCHAR_C c );
			void			do_fn_pass_iir(				c_obj_ui* CONST t,							C_PCHAR_C fn_name,	INT32		CONST a,	INT32		CONST b,	REAL CONST rc );
			void			do_fn_pass_iir3(			c_obj_ui* CONST t,	C_PCHAR_C table_name,	C_PCHAR_C fn_name,	INT32		CONST a,	INT32		CONST b,	REAL* v3 );
			bool			do_fn_b_pass_iiiiiii(		c_obj_ui* CONST t,	C_PCHAR_C table_name,	C_PCHAR_C fn_name,	INT32		CONST a,	INT32		CONST b,	INT32 CONST c, INT32 CONST d, INT32 CONST e, INT32 CONST f, INT32 CONST g );
			bool			do_fn_b_pass_ssiiiii(		c_obj_ui* CONST t,	C_PCHAR_C table_name,	C_PCHAR_C fn_name,	C_PCHAR_C	CONST a,	C_PCHAR_C	CONST b,	INT32 CONST c, INT32 CONST d, INT32 CONST e, INT32 CONST f, INT32 CONST g );
			void			do_fn_pass_rr(				c_obj_ui* CONST t,							C_PCHAR_C fn_name,	REAL		CONST a,	REAL		CONST b		);
			void			do_fn_pass_s(				c_obj_ui* CONST t,							C_PCHAR_C fn_name,	C_PCHAR_C	CONST a	);
			REAL			do_fn_r_pass_i(				c_obj_ui* CONST t,	C_PCHAR_C table_name,	C_PCHAR_C fn_name,	INT32		CONST a	);
			REAL			do_fn_r_pass_ii(			c_obj_ui* CONST t,	C_PCHAR_C table_name,	C_PCHAR_C fn_name,	INT32		CONST a,	INT32		CONST b,	REAL CONST ret_def = 0.		);
	// same than previous this one is for parallel C++ code calling lua
			REAL			do_fn_r_pass_ii_final(		c_obj_ui* CONST t,	C_PCHAR_C table_name,	C_PCHAR_C fn_name,	INT32		CONST a,	INT32		CONST b,	REAL CONST ret_def );
			REAL			do_fn_r_pass_ii_final(							INT32	fn_ref,								INT32		CONST a,	INT32		CONST b,	REAL CONST ret_def );
			void			do_fn_rn_pass_i(			c_obj_ui* CONST t,	C_PCHAR_C table_name,	C_PCHAR_C fn_name,	INT32		CONST a,	REAL*		CONST dst,	INT32 CONST nb_dst );
			void			do_fn_fn_pass_i(			c_obj_ui* CONST t,	C_PCHAR_C table_name,	C_PCHAR_C fn_name,	INT32		CONST a,	FP32*		CONST dst,	INT32 CONST nb_dst );

	virtual AAA_ERR			load_data_from_filename( o_str CONST & filename, INT32 CONST type_io );
	virtual AAA_ERR			load_do_after(	o_str CONST & filename );
	virtual AAA_ERR			save_do_after(	o_str CONST & filename );
};

extern	c_lua_wrap*		g_lua_wrap_master;
extern	c_lua_wrap*		g_lua_wrap_cur;
extern	c_lua_wrap*		g_lua_wrap_net;
