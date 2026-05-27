//
//	to compile lua read https://msdn.microsoft.com/en-us/library/f2ccy3wt.aspx
//		then lua doc

#include "aaalua_master.h"
#include "aaalua_wrap.h"
#include "aaalua_util.h"
#include "aaalua_debug.h"
#include "aaalua_glue.h"
#include "infrastructure/param/trax.h"
#include "obj_ui/com/net.h"
#include "infrastructure/seedfile.h"
#include "infrastructure/param/param_declare.h"
#ifndef AAA_INCLUDED_OSCRECEIVEDELEMENTS_H
//#	include "obj_ui/com/osc/OscReceivedElements.h"
#endif

#include "math/v_base.h"
#include "spy.h"
#ifndef AAA_SYSTEMUTILS_H
#	include "system/shared/SystemUtils.h"
#endif
#include "aaa/execute.h"

#include <lib_use.h>
#if AAA_LIB_LUA_STATIC()
#	if AAA_WIN64()
		AAA_LIB_USE_D( "lua51gc64s" )
#	else
		AAA_LIB_USE_D( "lua51s" )	
#	endif
#else
	AAA_LIB_USE( "lua51" )
#endif
//bool		c_lua_wrap::g_force_recheck = false;

/*
#include "luna.h"

class Account {
	lua_Number m_balance;
public:
	static CONST char className[];
	static Luna<Account>::RegType methods[];

	Account(lua_State* L)      { m_balance = luaL_checknumber(L, 1); }
	int deposit (lua_State* L) { m_balance += luaL_checknumber(L, 1); return 0; }
	int withdraw(lua_State* L) { m_balance -= luaL_checknumber(L, 1); return 0; }
	int balance (lua_State* L) { lua_pushnumber(L, m_balance); return 1; }
	~Account() { printf("deleted Account (%p)\n", this); }
};

CONST char Account::className[] = "Account";

#define method(class, name) {#name, &class::name}

Luna<Account>::RegType Account::methods[] = {
	method(Account, deposit),
	method(Account, withdraw),
	method(Account, balance),
	{0,0}
};
*/
/*
int main(int argc, char *argv[])
{

//	Luna<Account>::Register(L);

//	if(argc>1) lua_dofile(L, argv[1]);

}
*/
namespace {
	INT32					memory_used_ui;
	bool					b_gc_memory_report_ui;
/*
	// Catch C++ exceptions and convert them to Lua error messages.
	// Customize as needed for your own exception classes.
	int wrap_exceptions( lua_State* L, lua_CFunction f )
	{
		try
		{	
			return f(L);  // Call wrapped function and return result.
		}
		catch (const char *s)
		{  // Catch and convert exceptions.
			lua_pushstring(L, s);
		}
		catch (std::exception& e)
		{
			lua_pushstring(L, e.what());
		}
		catch (...)
		{
			lua_pushliteral( L, "caught (...)" );
		}
		return lua_error(L);  // Rethrow as a Lua error.
	}

	static int myinit(lua_State *L)
	{
		// Define wrapper function and enable it.
		lua_pushlightuserdata(L, (void *)wrap_exceptions);
		luaJIT_setmode(L, -1, LUAJIT_MODE_WRAPCFUNC|LUAJIT_MODE_ON);
		lua_pop(L, 1);
	}
*/
}


FACTORY_CREATE_V1( c_lua_master, lua_master, Lua Master, lua_master );

namespace
{
	static	C_PCHAR_C	editor_symbo[5] =
	{
		"Custom",
		"NotePad++",
		"SublimeText 2",
		"SublimeText 3",
		"Visual Studio Code"
	};

	CONSTEXPR INT32 BASE_PARAM_NB	= 24;
	CONSTEXPR INT32 GC_PARAM_NB		= 5;
	CONSTEXPR INT32 CALL_PARAM_NB	= 5;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 2;
	CONSTEXPR INT32 PARAM_NB_MAX	= BASE_PARAM_NB
								+	CALL_PARAM_NB
								+	GC_PARAM_NB
								+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(		save				)
		PARAM_DEF_BOOL_ON(		file_check_active	)
		PARAM_DEF_BOOL_OFF(		reload_all_trig		)
		PARAM_DEF_BOOL_OFF(		run_always_force	)
		PARAM_DEF_BOOL_ON(		do_lock				)
		PARAM_DEF_BOOL_OFF(		hook_event			)
		PARAM_DEF_BOOL_ON(		verbose_debug		)
		PARAM_DEF_BOOL_OFF(		verbose_return		)
		PARAM_DEF_BOOL_OFF(		verbose_aaa			)
		PARAM_DEF_BOOL_OFF(		dump_stack			)
		PARAM_DEF_BOOL_OFF(		dump_stack_trig		)
		PARAM_DEF_BOOL_ON(		traceback			)
//		PARAM_DEF_BOOL_OFF(		compile_trig_dialog	)
		PARAM_DEF_BOOL_OFF(		error_trig_dialog	)
		PARAM_DEF_BOOL_OFF(		error_memory_skip_dialog	)
		PARAM_DEF_BOOL_OFF(		print_trig_dialog	)
		PARAM_DEF_BOOL_OFF(		error_trig_editor	)
		PARAM_DEF_SYMBO_PSTR(	editor,				2, 1,	editor_symbo )
		PARAM_DEF_FILENAME(		editor_path,		aaa::file::TYPE_IO_LUA_EDITOR, 0 )

		PARAM_DEF_GROUP_CLOSED(	garbage Collector,		GC_PARAM_NB	)
			PARAM_DEF_BOOL_ON(		gc_do					)
			PARAM_DEF_INT32(		gc_step,				1, 25,	1, PARAM_MAX_INT32	)
			PARAM_DEF_INT32(		gc_step_multiplier,		1, 200,	1, PARAM_MAX_INT32	)
			PARAM_DEF_INT32(		gc_pause,				1, 100,	1, PARAM_MAX_INT32	)
			PARAM_DEF_REAL_INF(		gc_time_buffer_value,	1, 2	)

		PARAM_DEF_BOOL_ON(		memory_report		)
		PARAM_DEF_INT32_LOCKED(	memory_used			)

		PARAM_DEF_GROUP_CLOSED(	Call,		CALL_PARAM_NB	)
			PARAM_DEF_BOOL_OFF(		call_debug			)
			PARAM_DEF_BOOL_OFF(		call_store			)
			PARAM_DEF_BOOL_OFF(		call_print			)
			PARAM_DEF_BOOL_OFF(		print_compile		)
			PARAM_DEF_BOOL_OFF(		print_pcall			)
	
		PARAM_DEF_UINT32_LOCKED(	ref_obj_nb			)
		PARAM_DEF_UINT32_LOCKED(	ref_param_nb		)
		PARAM_DEF_INT32_LOCKED(		error				)
		PARAM_DEF_INT32_LOCKED(		error_total			)
	};

	PARAM_DEF_MAKE_INDEX( error_trig_editor	);
	PARAM_DEF_MAKE_INDEX( editor );
}

void	c_lua_master::param_init_pt_static()
{
	INT32	h = 0;
	param_set_pt( h, _b_save_ui					);
	param_set_pt( h, _b_file_check_do_ui		);
	param_set_pt( h, _b_reload_all_trig_ui		);
	param_set_pt( h, _b_run_always_ui			);
	param_set_pt( h, _b_do_lock_ui				);
	param_set_pt( h, _b_hook_event_ui			);
	param_set_pt( h, _b_verbose_debug_ui		);
	param_set_pt( h, _b_verbose_return_ui		);
	param_set_pt( h, _b_verbose_aaa_ui			);
	param_set_pt( h, _b_dump_stack_ui			);
	param_set_pt( h, _b_dump_stack_trig_ui		);
	param_set_pt( h, _b_traceback_ui			);
//	param_set_pt( h, _b_compile_trig_dialog_ui	);
	param_set_pt( h, _b_error_trig_dialog_ui	);
	param_set_pt( h, _b_errmem_skip_dialog_ui	);
	param_set_pt( h, _b_print_trig_dialog_ui	);
	param_set_pt( h, _b_error_trig_editor_ui	);
	param_set_pt( h, _s_editor_ui				);
	param_set_pt( h, _editor_name_ui			);

	++h;
		param_set_pt( h, _b_gc_do_ui				);
		param_set_pt( h, _gc_step_ui				);
		param_set_pt( h, _gc_step_mul_ui			);
		param_set_pt( h, _gc_pause_ui				);
		param_set_pt( h, _gc_time_buffer_value_ui	);


	param_set_pt( h, b_gc_memory_report_ui	);
	param_set_pt( h, memory_used_ui			);

	++h;
		param_set_pt( h, aaalua::b_call_debug_ui	);
		param_set_pt( h, aaalua::b_call_store_ui	);
		param_set_pt( h, aaalua::b_call_print_ui	);
		param_set_pt( h, _b_print_compile_ui		);
		param_set_pt( h, _b_print_pcall_ui			);

	param_set_pt( h, c_obj_ui::get_ref_obj_nb_pt()		);
	param_set_pt( h, c_obj_ui::get_ref_param_nb_pt()	);
	param_set_pt( h, _error_by_frame_ui					);
	param_set_pt( h, _error_total_ui					);

	err_param_init_pt(h);
}

void	c_lua_master::prepare_for_ui()
{
	bool b = !is_error_trig_dialog();
	param_set_unused( PARAM_INDEX_error_trig_editor, b );
	param_set_unused_2( PARAM_INDEX_editor, b || !is_error_trig_editor() );
}

void	c_lua_master::init()
{
	param_init_with( param, PARAM_NB_MAX );
}

void	c_lua_master::update()
{
	if( _error_by_frame_ui == 0 )
	{	//no error we can stop systematic recheck
		set_force_recheck( false );
	}
	else
	{	//init count for last frame but keep systematic recheck
		_error_by_frame_ui = 0;
	}
	if( _b_dump_stack_trig_need_clear )
	{
		g_lua_master->disable_dump_stack();
		_b_dump_stack_trig_need_clear = false;
	}
	if( _b_dump_stack_trig_ui )
	{
		g_lua_master->enable_dump_stack();
		_b_dump_stack_trig_ui = false;
		_b_dump_stack_trig_need_clear = true;
	}

	lua_State* L = g_lua_wrap_master->get_lua_state()->get_state();
	if( _gc_pause != _gc_pause_ui )
	{
		_gc_pause = _gc_pause_ui;
		lua_gc( L, LUA_GCSETPAUSE,		_gc_pause		);
	}
	if( _gc_step_mul != _gc_step_mul_ui )
	{
		_gc_step_mul = _gc_step_mul_ui;
		lua_gc( L, LUA_GCSETSTEPMUL,	_gc_step_mul	);
	}

	if( _b_gc_do_ui || b_gc_memory_report_ui )
	{
		lua_do_lock();

		if( _b_gc_do_ui )
		{
			SPY_PUSH_RANGE( "lua garbage collector", spy::LUA );
			TBUF_ADD( tbuf::CH_LUA, _gc_time_buffer_value_ui, "garbage collector" );
				lua_gc( L, LUA_GCSTEP, _gc_step_ui		);
			TBUF_ADD( tbuf::CH_LUA, 0., "garbage collector done" );
			SPY_POP_RANGE();
		}
		if( b_gc_memory_report_ui )
		{
			SPY_PUSH_RANGE( "lua memory report", spy::LUA );
				memory_used_ui = lua_getgccount( L );
			SPY_POP_RANGE();
		}

		lua_do_unlock();
	}
}

AAA_ERR	c_lua_master::load_data_from_filename( o_str CONST & filename, INT32 type_io )
{
	switch( type_io )
	{
	case aaa::file::TYPE_IO_LUA_EDITOR:
		_editor_name_ui.set( filename );
		return AAA_OK;
	}
	return ERR_TYPE_UNUSED;
}

namespace {
	c_lua_state				global_unique_state;
}


CONSTRUCTOR_CREATE(c_lua_master)
,_error_by_frame_ui(0)
,_error_total_ui(0)
,_b_force_recheck(false)
,_b_dump_stack_trig_need_clear(false)
,_gc_step_mul		{-1}
,_gc_pause			{-1}
{
	if( is_obj_first() )	//todo	should be singleton
		init();
}

c_lua_master::~c_lua_master()
{
}

c_lua_master*	g_lua_master = nullptr;


namespace {

	bool build_editor_path( o_str * p_path, INT32 CONST p_editor ) 
	{
		static CONSTEXPR CHAR CONST * editor_name[] =
		{
			"",
			"\\Notepad++\\notepad++.exe",
			"\\Sublime Text 2\\sublime_text.exe",
			"\\Sublime Text 3\\sublime_text.exe",
			"\\Microsoft VS Code\\Code.exe"
		};

		static CONSTEXPR sysutils::FOLDER_KNOWN folder_type[] =
		{
			sysutils::FOLDER_KNOWN::PROGRAM_FILES_x64,
			sysutils::FOLDER_KNOWN::PROGRAM_FILES_x86,
			sysutils::FOLDER_KNOWN::APP_DATA_LOCAL,
		};

		CONSTEXPR INT32 nb = ARRAY_SIZE( folder_type );
		for( INT32 i = 0; i < nb; ++i )
		{
			if( sysutils::get_folder_path_known( p_path, folder_type[i] ) )
			{
				p_path->add( editor_name[p_editor] );

				if( c_file::is_exist( p_path->get() ) )
					return true;
			}
		}

		// VS code special case.
		if( p_editor == 4 )
		{
			if( sysutils::get_folder_path_known( p_path, folder_type[2] ) )
			{
				p_path->add( "\\Programs" );
				p_path->add( editor_name[4] );

				if( c_file::is_exist( p_path->get() ) )
					return true;
			}
		}

		p_path->erase();
		return false;
	}
}

//todo this is not only a lua functionality
//	create an editor master object ?
bool	c_lua_master::trig_edit_file( o_str CONST & filename_in, INT32 CONST line )
{
	if( filename_in.get_len() > 450 )
	{
		debug_break( "%s() filename is too long : %s", __FUNCTION__, filename_in.get() );
		return false;
	}

	INT32	s_editor	=	get_editor();
	o_str	editor;
	if( s_editor > 0 )
	{
		// build editor path.
		if( !build_editor_path( &editor, s_editor) )
			editor.set( get_editor_name() );
	}
	else
		editor.set( get_editor_name() );

	if( editor.is_empty() )
	{
		debug_break( "No text editor name to launch, please define" );
		return false;
	}

	o_str filename;
	filename.add_char( '"' );
	filename.add( filename_in );
	filename.add_char( '"' );

	o_str o;
	if( line > 0 )
	{
		switch( s_editor )
		{
		default:
		case 1:
			o.set( "-n" );
			o.add( line );
			o.add_space();
			o.add( filename );
			break;
		case 2:
		case 3:
			o.set( filename );
			o.add_char( ':' );
			o.add( line );
			break;
		case 4:
			//			o.set( "-r " );
			o.set( "--reuse-window " );
			o.add( "--goto " );
			o.add( filename );
			o.add_char( ':' );
			o.add( line );
			break;
		}
	}
	else
	{
		o.set( filename );
	}

	/*
	C_PCHAR_C	str_dir = c_file::dir_get_def();
	if( line > 0 )
	sprintf( str, "\"%s\" -n%d \"%s\\%s\"\n", editor->get(), line, str_dir, filename.get() );
	else
	sprintf( str, "\"%s\" \"%s\\%s\"\n", editor->get(), str_dir, filename.get() );
	*/
	/*
	C_PCHAR_C	str_dir = c_file::dir_get_def();
	if( script->is_not_empty() )
	sprintf( str, "\"%s\" %s\\%s\n", editor->get(), str_dir, script->get() );
	else
	{
	o_str	o_dir;
	c_namer::build_dir( o_dir, this );
	sprintf( str, "\"%s\" %s\\%sfx.lua\n", editor->get(), str_dir, o_dir.get() );
	}
	*/

	o.convert_to_backslash();
	//	GOOD_PRINT_STRING( "Execute : %s", o.get() );
	//aaa::execute_shell( editor.get(), o.get() );

	return NOERR( aaa::execute_process( editor.get(), o.get() ) );
}