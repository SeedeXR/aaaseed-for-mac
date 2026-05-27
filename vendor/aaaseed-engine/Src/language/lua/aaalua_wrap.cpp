//
//	to compile lua read https://msdn.microsoft.com/en-us/library/f2ccy3wt.aspx
//		then lua doc

#include "aaalua_wrap.h"
#include "aaalua_master.h"
#include "aaalua_glue.h"
#include "infrastructure/param/trax.h"
#include "infrastructure/seedfile.h"
#include "infrastructure/param/param_declare.h"
#include "math/v_base.h"
#include "spy.h"
#include "aaaseed.h"
#include "infrastructure/obj/root.h"

c_lua_wrap*		g_lua_wrap_cur = nullptr;

extern C_PCHAR_C	str_table_compile_fn;

c_lua_wrap*		g_lua_wrap_net = nullptr;

namespace
{
	c_lua_state		global_unique_state;
}

FACTORY_CREATE_V1( c_lua_wrap, lua_wrap, Lua wrapper, lua_wrap );

void	c_lua_wrap::c_init()
{
	global_unique_state.open();
}
void	c_lua_wrap::c_deinit()
{
	global_unique_state.close();
}
void	c_lua_wrap::do_text( C_PCHAR_C str,	C_PCHAR_C name )
{
	static c_lua_wrap * lua_wrap_tex_trick  = nullptr;
	if( !lua_wrap_tex_trick )
		node_pref->obj_get( lua_wrap_tex_trick );
	lua_wrap_tex_trick->do_str( str, name );
}

namespace n_lua_wrap
{
	CONSTEXPR INT32 BASE_PARAM_NB	=	12;
	CONSTEXPR INT32 GROUP_PARAM_NB	=	0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(		doit					)
		PARAM_DEF_BOOL_OFF(		doit_trig				)
		PARAM_DEF_BOOL_OFF(		run_only_when_compiled	)
		PARAM_DEF_BOOL_ON(		file_check				)
		PARAM_DEF_BOOL_OFF(		file_check_always		)
		PARAM_DEF_BOOL_OFF(		file_reload_trig		)
		PARAM_DEF_BOOL_OFF(		skip_rest				)
		PARAM_DEF_FILENAME(		filename,				aaa::file::TYPE_IO_LUA, 0	)
		PARAM_DEF_STR_LOCKED(	text					)	//	because we don't save from AAASeed but edit (notepad++ usually)
		PARAM_DEF_BOOL_OFF(		edit_trig				)
		PARAM_DEF_BOOL_LOCKED(	compiled				)
		PARAM_DEF_NONE(			lua master				)
	};
}


void	c_lua_wrap::param_init_pt()
{
	INT32	h = 0;

	param_set_pt(		h, _b_doit_ui					);
	param_set_pt(		h, _b_doit_trig_ui				);
	param_set_pt(		h, _b_run_only_when_compiled_ui	);
	param_set_pt(		h, _b_file_check_ui				);
	param_set_pt(		h, _b_file_check_always_ui		);
	param_set_pt(		h, _b_file_reload_trig_ui		);
	param_set_pt(		h, _b_skip_rest_ui				);
	param_set_pt(		h, _script_name_ui				);
	param_set_pt(		h, _text_ui						);
	param_set_pt(		h, _b_edit_trig_ui				);

	param_set_pt(		h, _b_compiled_out				);
	param_attach_obj(	h, g_lua_master					);

	err_param_init_pt(h);
}

bool	c_lua_wrap::param_do_action( c_param * CONST par, CONST aaa::param::ACTION action )
{
	if( action == aaa::param::ACTION::PARAM_SIGN || action == aaa::param::ACTION::PARAM_OPEN )
	{
		o_str CONST & o = par->get_name();
		//		if( o.is_starting_with_nocase( "bind_2d", 7 ) || o.is_equal( "bank_2d" ) )
		if( o.is_equal( "text" ) )
		{
			g_lua_master->trig_edit_file( get_script_debug_name() );
			return true;
		}
		else if( o.is_equal( "filename" ) )
		{
//			if( _script_name_ui.is_empty() )
				aaa::file::ask_type_io_load( aaa::file::TYPE_IO_LUA, this );
//				aaa::file::load_type_io( aaa::file::TYPE_IO_LUA, _script_filename, this );
//			else
//				g_lua_master->trig_edit_file( get_script_debug_name() );
			return true;
		}
	}
	return false;
}



void c_lua_wrap::init()
{
	//	set_name("Lua");
	param_init_with( n_lua_wrap::param, n_lua_wrap::PARAM_NB_MAX);
}

CONSTRUCTOR_CREATE(c_lua_wrap)
,_check_file_counter(0)
,_b_text_direct(false)
,_file_time(c_file::TIME_UNDEFINED)
{
	_l = &global_unique_state;
	_b_compiled_out = false;
	init();
} 


c_lua_wrap::~c_lua_wrap()
{
	if( !_lua_fn_name.is_empty() )
	{
		//we erase the table use by the object
		if( _l->get_state() ) //	avoid crash on exit
		{
			_l->get_global( str_table_compile_fn );
			_l->push_nil();
			lua_setfield( _l->get_state(), -2, _lua_fn_name.get() );	// str_table_compile_fn[_lua_fn_name"] = nil
			_l->pop(1);	// pop table str_table_compile_fn
		}
	}
}


//	set variable
//		lua_pushnumber( L, 20);
//		lua_setglobal( L, "in0" );

/*
_l->define_table_global( "aaa_in");	//todo define once only

void	c_lua_wrap::define_table_global()
{
	lua_newtable( l_master.get_state() );		//	creates a table
	l_master.set_field( "r", .2 );				//	table.r =
	l_master.set_field( 1, .2 );
	l_master.set_field( "g", .6 );				//	table.g =
	l_master.set_field( 2, .6);
	l_master.set_field( "b", 1. );				//	table.b =
	l_master.set_field( 3, 1.2 );
	lua_setglobal( l_master.get_state(), "aaa_in");	//	'name' = table
}

void	c_lua_wrap::fill_table()
{
	_l->get_global( "aaa_in");	//	'name' = table
	_l->set_field( "r", .2);	//	table.r =
	_l->set_field( 1, .2);
	_l->set_field( "g", .6);	//	table.g =
	_l->set_field( 2, .6);
	_l->set_field( "b", 1.);	//	table.b =
	_l->set_field( 3, 1.2);
	_l->pop(1);					//	table left on the stack
}
*/

FINLINE	void	c_lua_wrap::get_trax_input( c_trax* CONST t )
{
	p_param	param;
	INT32	i;

	_l->get_global( "aaa" );
	//  stack : aaa

	_l->get_field( -1, "tin" );	
	//  stack : aaa | aaa.tin
	//	one call to clean at the end is cheaper
	//	lua_remove(_l->get_state(), -2);

	// now
	if( param = t->get_plug_in_first() )
	{
		i = 1;			
		do 
		{
			REAL val = param->get_value_as_real();
			_l->set_field( i, val );
		}
		while( param = t->get_plug_in(i) );
	}
	else
		i = 0;
	//	set number of input
	_l->set_field( "nb", i );
	//	put a ref on object
	//	_l->set_field( "trax", obj_get);
	//	tables are still on the stack
	_l->pop_all();	// aaa and in on the stack
}

FINLINE	bool	c_lua_wrap::put_fn_on_stack( C_PCHAR fn_name )
{
	if( !fn_name || *fn_name==0 )
		return false;
	STACK_ENFORCE_EMPTY( _l, (CHAR*)fn_name );

//todo	check overflow to avoid crash and optimize
	CHAR		str[256];
	CHAR* CONST pt_end	= str + sizeof(str); 
	CHAR*		pt		= str;	
	for(;;)
	{
		CHAR c = *fn_name++;
		if( c=='.' || c==0 )
		{
			*pt = 0;
			_l->get_global( str );
			if( _l->is_nil() )	//	we don't find str
			{
				_l->pop(1);
				return false;
			}
			else
			{
				if( c==0 )
					return true;
				break;
			}
		}
		if( pt < pt_end )
			*pt++ = c;
		else
			return false;
	}

	pt = str;
	for(;;)
	{
		CHAR c = *fn_name++;
		if( c=='.' || c==0 )
		{
			*pt = 0;
			_l->get_field( -1, str );
			if( _l->is_nil() )	//	we don't find str
			{
				_l->pop(2);
				return false;
			}
			else
			{
				_l->remove( -2 );
				if( c==0 )
					return true;
				pt = str;
			}
		}
		else
		{
			if( pt < pt_end )
				*pt++ = c;
			else
			{
				_l->pop(1);
				return false;
			}
		}
	}	
}

bool	c_lua_wrap::exits_function_by_name( C_PCHAR_C fn_name )
{
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );

	bool ret;
	if( put_fn_on_stack( fn_name ) )
	{
		if( _l->is_function(-1) )
			ret = true;
		_l->pop(1);
	}
	else
		ret = false;

	STACK_ENFORCE_SAME( _l );
	lua_do_unlock();
	return ret;
}

/*
FINLINE	bool	put_table_fn_on_stack_final( c_lua_state* l, C_PCHAR_C table_name, C_PCHAR_C fn_name )
{
	l->get_global( table_name );
	//  stack : table
	if( l->is_table() )
	{
		l->get_field( -1, fn_name );
		if( l->is_function() )	//	we found field fn_name
		{
			l->remove( -2 );
			return true;
		}
		l->pop(2);
		return false;
	}
	ERR_PRINT_STRING( "%s() can't find table %s so neither member %s", __FUNCTION__, table_name, fn_name );
	l->pop(1);
	return false;
}
*/

FINLINE	bool	c_lua_wrap::put_table_fn_on_stack( C_PCHAR_C table_name, C_PCHAR_C fn_name )
{
	STACK_ENFORCE_EMPTY( _l, (CHAR*)fn_name );

	if( table_name && *table_name!=0 )
	{	// find a field into a table
		_l->get_global( table_name );
		//  stack : table
		if( _l->is_table() )
		{
			_l->get_field( -1, fn_name );
			if( _l->is_function() )	//	we found field fn_name
			{
				_l->remove( -2 );
				return true;
			}
			_l->pop(2);
			return false;
		}
		ERR_PRINT_STRING( "%s() can't find table %s so neither member %s", __FUNCTION__, table_name, fn_name );
		_l->pop(1);
		return false;
	}

	//	or into the global table
	_l->get_global( fn_name );
	if( _l->is_function() )	//	we don't found fn_name
		return true;
	_l->pop(1);
	return false;
}


INT32	c_lua_wrap::get_table_fn_as_ref( C_PCHAR_C table_name, C_PCHAR_C fn_name )
{
	STACK_ENFORCE_EMPTY( _l, (CHAR*)fn_name );

	INT32 ref = LUA_NOREF;
	if( table_name && *table_name!=0 )
	{	// find a field into a table
		_l->get_global( table_name );
		//  stack : table
		if( _l->is_table() )
		{
			_l->get_field( -1, fn_name );
			if( _l->is_function() )	//	we found field fn_name
			{
				ref = _l->ref();
				_l->pop(1);
				return ref;
			}
			_l->pop(2);
			return LUA_NOREF;
		}
		ERR_PRINT_STRING( "%s() can't find table %s so neither member %s", __FUNCTION__, table_name, fn_name );
		_l->pop(1);
		return LUA_NOREF;
	}

	//	or into the global table
	_l->get_global( fn_name );
	if( _l->is_function() )	//	we don't found fn_name
		return _l->ref();
	_l->pop(1);
	return LUA_NOREF;
}

static	std::vector<c_obj_ui*>	caller_stack;

void		c_lua_wrap::push_caller( c_obj_ui* CONST obj )	{	caller_stack.push_back(obj);	}
void		c_lua_wrap::pop_caller()						{	caller_stack.pop_back();		}
c_obj_ui*	c_lua_wrap::get_caller()						{	return caller_stack.empty() ? nullptr : caller_stack.back(); }

/*
FINLINE	bool	c_lua_wrap::find_function( C_PCHAR_C fn_name )
{
}
*/

REAL	c_lua_wrap::do_trax( c_trax* CONST t, C_PCHAR_C fn_name )
{
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );

	REAL	val = 0.;
	get_trax_input(t);

	_l->get_global( fn_name );
	// the function takes 0 parameters and will return results one for now
	push_caller( t );
	auto lua_err = _l->pcall( 0, 1 );
	pop_caller();
	if( lua_err != LUA_OK )
	{
		ERR_PRINT_STRING( "trying %s", fn_name );
		_l->pop_str_and_print_error( "PCALL", "lua::do_trax()" );
	}
	else
	{
		if( _l->is_nil() )
		{	// pick the error message
			ERR_PRINT_STRING( "%strax %.6s fn %s() : no return value",  _l->get_lua_error_header(), t->get_name_str(), fn_name );
			_l->pop_all();
		}
		else
		{
			val = _l->get_value_direct<REAL>();
			if( g_lua_master->is_verbose_return() )
				_l->pop_str_and_print( "lua::do_trax()" );
			else
				_l->pop(1);
		}
	}

	STACK_ENFORCE_SAME( _l );
	lua_do_unlock();
	return val;
}

//fn_c_name is used for table name also
FINLINE	bool	c_lua_wrap::do_table_fn_central(	c_obj_ui* CONST caller,	C_PCHAR_C fn_c_name, C_PCHAR_C table_name, C_PCHAR_C fn_name, INT32 CONST nb_in,	INT32 CONST nb_out	)
{
	push_caller( caller );
	auto lua_err = _l->pcall( nb_in, nb_out );
	pop_caller();
	bool b_ok = lua_err == LUA_OK;
	if( b_ok )
	{
		if( nb_out != 0 && _l->is_none_or_nil(1) )
		{	// pick the error message
			if( caller )
			{	//todo should we go here
				ERR_PRINT_STRING( "%s %s() %.s fn %s.%s() : no return value",	_l->get_lua_error_header(), fn_c_name, caller->get_name_str(), table_name, fn_name );
			}
			else
				ERR_PRINT_STRING( "%s %s() fn %s.%s() : no return value",		_l->get_lua_error_header(), fn_c_name, table_name, fn_name );
			_l->pop_all();
			b_ok = false;
		}
	}
	else
		_l->pop_str_and_print_error( "PCALL", fn_c_name, table_name,  fn_name );
	return b_ok;
}

//fn_c_name is used for table name also
FINLINE	bool	c_lua_wrap::do_fn_central(	c_obj_ui* CONST caller,	C_PCHAR_C fn_c_name, C_PCHAR_C fn_name, INT32 CONST nb_in, INT32 CONST nb_out	)
{
	push_caller( caller );
	auto lua_err = _l->pcall( nb_in, nb_out );
	pop_caller();
	bool b_ok = lua_err == LUA_OK;
	if( b_ok )
	{
		if( nb_out != 0 && _l->is_none_or_nil(1) )
		{	// pick the error message
			if( caller )
				ERR_PRINT_STRING( "%s %s() %.s fn %s() : no return value",	_l->get_lua_error_header(), fn_c_name, caller->get_name_str(), fn_name );
			else
				ERR_PRINT_STRING( "%s %s() fn %s() : no return value",		_l->get_lua_error_header(), fn_c_name, fn_name );
			_l->pop_all();
			b_ok = false;
		}
	}
	else
		_l->pop_str_and_print_error( "PCALL", fn_c_name, fn_name );
	return b_ok;
}

FINLINE	bool	c_lua_wrap::do_table_fn_central(	INT32 CONST nb_in,	INT32 CONST nb_out	)
{
	auto lua_err = _l->pcall( nb_in, nb_out );
	bool b_ok = lua_err == LUA_OK;
	if( b_ok )
	{
		if( nb_out != 0 && _l->is_none_or_nil(1) )
		{	// pick the error message

			ERR_PRINT_STRING( "%s : no return value",		_l->get_lua_error_header() );
			_l->pop_all();
			b_ok = false;
		}
	}
	else
		_l->pop_str_and_print_error( "PCALL in %s()", __FUNCTION__ );
	return b_ok;
}

REAL	c_lua_wrap::do_fn_r_pass_ii(	c_obj_ui* CONST t,	C_PCHAR_C table_name, C_PCHAR_C fn_name,
															INT32 CONST a, INT32 CONST b, REAL CONST ret_def )
{
	REAL	val = ret_def;
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );

		if( put_table_fn_on_stack( table_name, fn_name ) )
		{
			_l->push_int( a );
			_l->push_int( b );
			// the function takes 2 parameters and return one result
			bool b_ok = do_table_fn_central(	t,	__FUNCTION__, table_name, fn_name,	2,	1	);
			if( b_ok )
			{
				val = _l->get_value_direct<REAL>();
				if( g_lua_master->is_verbose_return() )
					_l->pop_str_and_print( table_name, fn_name );
				else
					_l->pop(1);
			}
		}

	STACK_ENFORCE_SAME( _l );
	lua_do_unlock();
	return val;
}

namespace
{
	aaa::MUTEX	lock_final;
}

void	c_lua_wrap::do_fn_pass_i(		c_obj_ui* CONST t,	C_PCHAR_C fn_name,
															INT32 CONST a )
{
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );
	if( put_fn_on_stack( fn_name ) )
	{
		_l->push_int( a );
		// the function takes 1 parameter and no return
		do_fn_central(	t,	__FUNCTION__, fn_name,	1,	0	);
	}
	STACK_ENFORCE_SAME( _l );
	lua_do_unlock();
}
void	c_lua_wrap::do_fn_pass_r(		c_obj_ui* CONST t,	C_PCHAR_C fn_name,
															REAL CONST a )
{
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );
	if( put_fn_on_stack( fn_name ) )
	{
		_l->push_real( a );
		// the function takes 1 parameter and no return
		do_fn_central(	t,	__FUNCTION__, fn_name,	1,	0	);
	}
	STACK_ENFORCE_SAME( _l );
	lua_do_unlock();
}
void	c_lua_wrap::do_fn_pass_d(		c_obj_ui* CONST t,	C_PCHAR_C fn_name,
															DOUBLE CONST a )
{
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );
	if( put_fn_on_stack( fn_name ) )
	{
		_l->push_double( a );
		// the function takes 1 parameter and no return
		do_fn_central(	t,	__FUNCTION__, fn_name,	1,	0	);
	}
	STACK_ENFORCE_SAME( _l );
	lua_do_unlock();
}


void	c_lua_wrap::do_fn_pass_bb(		c_obj_ui* CONST t,	C_PCHAR_C table_name, C_PCHAR_C fn_name,
															bool CONST a, bool CONST b )
{
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );
	if( put_table_fn_on_stack( table_name, fn_name ) )
	{
		_l->push_bool( a );
		_l->push_bool( b );
		// the function takes 2 parameters and no return
		do_table_fn_central(	t,	__FUNCTION__, table_name, fn_name,	2,	0	);
	}
	STACK_ENFORCE_SAME( _l );
	lua_do_unlock();
}
void	c_lua_wrap::do_fn_pass_di(		c_obj_ui* CONST  t,	C_PCHAR_C fn_name,
															DOUBLE CONST a, INT32 CONST b )
{
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );
	if( put_fn_on_stack( fn_name ) )
	{
		_l->push_double( a );
		_l->push_int( b );
		// the function takes 2 parameters and no return
		do_fn_central(	t,	__FUNCTION__, fn_name,	2,	0	);
	}
	STACK_ENFORCE_SAME( _l );
	lua_do_unlock();
}
void	c_lua_wrap::do_fn_pass_si(		c_obj_ui* CONST t,	C_PCHAR_C fn_name,
															C_PCHAR_C a, INT32 CONST b )
{
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );
	if( put_fn_on_stack( fn_name ) )
	{
		_l->push_string( a );
		_l->push_int( b );
		// the function takes 2 parameters and no return
		do_fn_central(	t,	__FUNCTION__, fn_name,	2,	0	);
	}
	STACK_ENFORCE_SAME( _l );
	lua_do_unlock();
}
void	c_lua_wrap::do_fn_pass_is(		c_obj_ui* CONST t,	C_PCHAR_C fn_name,
															INT32 CONST a, C_PCHAR_C b )
{
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );
	if( put_fn_on_stack( fn_name ) )
	{
		_l->push_int( a );
		_l->push_string( b );
		// the function takes 2 parameters and no return
		do_fn_central(	t,	__FUNCTION__, fn_name,	2,	0	);
	}
	STACK_ENFORCE_SAME( _l );
	lua_do_unlock();
}
void	c_lua_wrap::do_fn_pass_ii(		c_obj_ui* CONST t,	C_PCHAR_C fn_name,
															INT32 CONST a, INT32 CONST b )
{
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );
	if( put_fn_on_stack( fn_name ) )
	{
		_l->push_int( a );
		_l->push_int( b );
		// the function takes 2 parameters and no return
		do_fn_central(	t,	__FUNCTION__, fn_name,	2,	0	);
	}
	STACK_ENFORCE_SAME( _l );
	lua_do_unlock();
}
void	c_lua_wrap::do_fn_pass_ii(		c_obj_ui* CONST t,	C_PCHAR_C table_name, C_PCHAR_C fn_name,
															INT32 CONST a, INT32 CONST b )
{
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );
	if( put_table_fn_on_stack( table_name, fn_name ) )
	{
		_l->push_int( a );
		_l->push_int( b );
		// the function takes 2 parameters and no return
		do_table_fn_central(	t,	__FUNCTION__, table_name, fn_name,	2,	0	);
	}
	STACK_ENFORCE_SAME( _l );
	lua_do_unlock();
}

void	c_lua_wrap::do_fn_pass_isi(		c_obj_ui* CONST t,	C_PCHAR_C fn_name,
															INT32 CONST a, C_PCHAR_C b, INT32 CONST c )
{
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );
	if( put_fn_on_stack( fn_name ) )
	{
		_l->push_int( a );
		_l->push_string( b );
		_l->push_int( c );
		// the function takes 3 parameters and no return
		do_fn_central(	t,	__FUNCTION__, fn_name,	3,	0	);
	}
	STACK_ENFORCE_SAME( _l );
	lua_do_unlock();
}
bool	c_lua_wrap::do_fn_b_pass_isi(		c_obj_ui* CONST t,	C_PCHAR_C fn_name,
																INT32 CONST a, C_PCHAR_C b, INT32 CONST c )
{
	bool b_ret = false;
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );
	if( put_fn_on_stack( fn_name ) )
	{
		_l->push_int( a );
		_l->push_string( b );
		_l->push_int( c );
		// the function takes 3 parameters and one return
		bool b_ok = do_fn_central(	t,	__FUNCTION__, fn_name,	3,	1	);
		if( b_ok )
		{
			b_ret = _l->get_bool(-1);
			if( g_lua_master->is_verbose_return() )
				_l->pop_str_and_print( fn_name );
			else
				_l->pop(1);
		}
	}
	STACK_ENFORCE_SAME( _l );
	lua_do_unlock();
	return b_ret;
}
void	c_lua_wrap::do_fn_pass_isd(		c_obj_ui* CONST t,	C_PCHAR_C fn_name,
															INT32 CONST a, C_PCHAR_C b, DOUBLE CONST c )
{
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );
	if( put_fn_on_stack( fn_name ) )
	{
		_l->push_int( a );
		_l->push_string( b );
		_l->push_double( c );
		// the function takes 3 parameters and no return
		do_fn_central(	t,	__FUNCTION__, fn_name,	3,	0	);
	}
	STACK_ENFORCE_SAME( _l );
	lua_do_unlock();
}
bool	c_lua_wrap::do_fn_b_pass_isd(	c_obj_ui* CONST t,	C_PCHAR_C fn_name,
															INT32 CONST a, C_PCHAR_C b, DOUBLE CONST c )
{
	bool b_ret = false;
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );
	if( put_fn_on_stack( fn_name ) )
	{
		_l->push_int( a );
		_l->push_string( b );
		_l->push_double( c );
		// the function takes 2 parameters and return one result
		bool b_ok = do_fn_central(	t,	__FUNCTION__, fn_name,	3,	1	);
		if( b_ok )
		{
			b_ret = _l->get_bool(-1);
			if( g_lua_master->is_verbose_return() )
				_l->pop_str_and_print( fn_name );
			else
				_l->pop(1);
		}
	}
	STACK_ENFORCE_SAME( _l );
	lua_do_unlock();
	return b_ret;
}
void	c_lua_wrap::do_fn_pass_iss(		c_obj_ui* CONST t,	C_PCHAR_C fn_name,
															INT32 CONST a, C_PCHAR_C b, C_PCHAR_C c )
{
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );
	if( put_fn_on_stack( fn_name ) )
	{
		_l->push_int( a );
		_l->push_string( b );
		_l->push_string( c );
		// the function takes 3 parameters and no return
		do_fn_central(	t,	__FUNCTION__, fn_name,	3,	0	);
	}
	STACK_ENFORCE_SAME( _l );
	lua_do_unlock();
}
bool	c_lua_wrap::do_fn_b_pass_iss(	c_obj_ui* CONST t,	C_PCHAR_C fn_name,
															INT32 CONST a, C_PCHAR_C b, C_PCHAR_C c )
{
	bool b_ret = false;
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );
	if( put_fn_on_stack( fn_name ) )
	{
		_l->push_int( a );
		_l->push_string( b );
		_l->push_string( c );
		// the function takes 3 parameters and return one result
		bool b_ok = do_fn_central(	t,	__FUNCTION__, fn_name,	3,	1	);
		if( b_ok )
		{
			b_ret = _l->get_bool(-1);
			if( g_lua_master->is_verbose_return() )
				_l->pop_str_and_print( fn_name );
			else
				_l->pop(1);
		}
	}
	STACK_ENFORCE_SAME( _l );
	lua_do_unlock();
	return b_ret;
}

void	c_lua_wrap::do_fn_pass_iir(		c_obj_ui* CONST t,	C_PCHAR_C fn_name,
															INT32 CONST a, INT32 CONST b, REAL CONST rc )
{
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );
	if( put_fn_on_stack( fn_name ) )
	{
		_l->push_int( a );
		_l->push_int( b );
		_l->push_real( rc );
		// the function takes 3 parameters and no return
		do_fn_central(	t,	__FUNCTION__, fn_name,	3,	0	);
	}
	STACK_ENFORCE_SAME( _l );
	lua_do_unlock();
}

void	c_lua_wrap::do_fn_pass_iir3(	c_obj_ui* CONST t,	C_PCHAR_C table_name,	C_PCHAR_C fn_name,
															INT32 CONST a, INT32 CONST b, REAL* CONST v3 )
{
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );
	if( put_table_fn_on_stack( table_name, fn_name ) )
	{
		_l->push_int( a );
		_l->push_int( b );
		_l->push_real_v3( v3 );
		// the function takes 5 parameters and no return
		do_table_fn_central(	t,	__FUNCTION__, table_name, fn_name,	5,	0	);
	}
	STACK_ENFORCE_SAME( _l );
	lua_do_unlock();
}

REAL	c_lua_wrap::do_fn_r_pass_ii_final(	c_obj_ui* CONST t,	C_PCHAR_C table_name, C_PCHAR_C fn_name,
																INT32 CONST a, INT32 CONST b, REAL CONST ret_def )
{
	REAL	val = ret_def;
	lock_final.lock();
	//STACK_ENFORCE_STORE( _l );

		//if( put_table_fn_on_stack_final( _l, table_name, fn_name ) )
		if( put_table_fn_on_stack( table_name, fn_name ) )
		{
		 	_l->push_int( a );
			_l->push_int( b );
			// the function takes 2 parameters and return one result
			bool b_ok = do_table_fn_central(	t,	__FUNCTION__, table_name, fn_name,	2,	1	);
			//lua_call( _l->get_state(), 2, 1 );
			//bool b_ok = _l->pcall( 2, 1 ) == 0;
			if( b_ok )
			{
				val = _l->get_value_direct<REAL>();
				if( g_lua_master->is_verbose_return() )
					_l->pop_str_and_print( table_name, fn_name );
				else
					_l->pop(1);
			}
		}

	//STACK_ENFORCE_SAME( _l );
	lock_final.unlock();
	return val;
}

REAL	c_lua_wrap::do_fn_r_pass_ii_final(	INT32 CONST lua_ref, INT32 CONST a, INT32 CONST b, REAL CONST ret_def )
{
	REAL	val = ret_def;

	if( lua_ref == LUA_NOREF )
	{
	}
	else
	{
		lock_final.lock();
		//STACK_ENFORCE_STORE( _l );

			_l->push_ref( lua_ref );
			_l->push_int( a );
			_l->push_int( b );
				// the function takes 2 parameters and return one result
			bool b_ok = do_table_fn_central(	2,	1	);
			//lua_call( _l->get_state(), 2, 1 );
			//bool b_ok = _l->pcall( 2, 1 ) == 0;
			if( b_ok )
			{
				val = _l->get_value_direct<REAL>();
				_l->pop(1);
			}

		//STACK_ENFORCE_SAME( _l );
		lock_final.unlock();
	}

	return val;
}

REAL	c_lua_wrap::do_fn_r_pass_i(	 c_obj_ui* CONST t,		C_PCHAR_C table_name, C_PCHAR_C fn_name,
															INT32 CONST a )
{
	REAL	val = 0.;
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );

	if( put_table_fn_on_stack( table_name, fn_name ) )
	{
		_l->push_int( a );
		// the function takes 1 parameter and return one result
		bool b_ok = do_table_fn_central(	t,	__FUNCTION__, table_name, fn_name,	1,	1	);
		if( b_ok )
		{
			val = _l->get_value_direct<REAL>();
			if( g_lua_master->is_verbose_return() )
				_l->pop_str_and_print( table_name, fn_name );
			else
				_l->pop(1);
		}
	}
	STACK_ENFORCE_SAME( _l );
	lua_do_unlock();
	return val;
}

bool	c_lua_wrap::do_fn_b_pass_iiiiiii(	c_obj_ui* CONST t,	C_PCHAR_C table_name,	C_PCHAR_C fn_name,
																INT32 CONST a, INT32 CONST b, INT32 CONST c, INT32 CONST d, INT32 CONST e, INT32 CONST f, INT32 CONST g )
{
	bool b_ret = false;
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );
	if( put_table_fn_on_stack( table_name, fn_name ) )
	{
		_l->push_int( a );
		_l->push_int( b );
		_l->push_int( c );
		_l->push_int( d );
		_l->push_int( e );
		_l->push_int( f );
		_l->push_int( g );
		// the function takes 7 parameters and return one result
		bool b_ok = do_table_fn_central(	t,	__FUNCTION__, table_name, fn_name,	7,	1	);
		if( b_ok )
		{
			b_ret = _l->get_bool(-1);
			if( g_lua_master->is_verbose_return() )
				_l->pop_str_and_print( table_name, fn_name );
			else
				_l->pop(1);
		}
	}
	STACK_ENFORCE_SAME( _l );
	lua_do_unlock();
	return b_ret;
}

bool	c_lua_wrap::do_fn_b_pass_ssiiiii(	c_obj_ui* CONST t,	C_PCHAR_C table_name, C_PCHAR_C fn_name,
																C_PCHAR_C a, C_PCHAR_C b, INT32 CONST c, INT32 CONST d, INT32 CONST e, INT32 CONST f, INT32 CONST g )
{
	bool b_ret = false;
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );
	if( put_table_fn_on_stack( table_name, fn_name ) )
	{
		_l->push_string( a );
		_l->push_string( b );
		_l->push_int( c );
		_l->push_int( d );
		_l->push_int( e );
		_l->push_int( f );
		_l->push_int( g );
		// the function takes 7 parameters and return one result
		bool b_ok = do_table_fn_central(	t,	__FUNCTION__, table_name, fn_name,	7,	1	);
		if( b_ok )
		{
			b_ret = _l->get_bool(-1);
			if( g_lua_master->is_verbose_return() )
				_l->pop_str_and_print( table_name, fn_name );
			else
				_l->pop(1);
		}
	}
	STACK_ENFORCE_SAME( _l );
	lua_do_unlock();
	return b_ret;
}

void	c_lua_wrap::do_fn_pass_rr(		c_obj_ui* CONST t,	C_PCHAR_C fn_name,
															REAL CONST a, REAL CONST b )
{
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );
	if( put_fn_on_stack( fn_name ) )
	{
		_l->push_real( a );
		_l->push_real( b );
		// the function takes 2 parameters and no return
		do_fn_central(	t,	__FUNCTION__, fn_name,	2,	0	);
	}
	STACK_ENFORCE_SAME( _l );
	lua_do_unlock();
}

void	c_lua_wrap::do_fn_pass_s(		c_obj_ui* CONST t,	C_PCHAR_C fn_name,	C_PCHAR_C a )
{
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );
	if( put_fn_on_stack( fn_name ) )
	{
		_l->push_string( a );
		// the function takes 1 parameter and no return
		do_fn_central(	t,	__FUNCTION__, fn_name,	1,	0	);
	}
	STACK_ENFORCE_SAME( _l );
	lua_do_unlock();
}

void	c_lua_wrap::do_fn_rn_pass_i(	c_obj_ui* CONST t,	C_PCHAR_C table_name, C_PCHAR_C fn_name,
															INT32 CONST a, REAL* CONST dst, INT32 CONST nb_dst )
{
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );
	bool	b_failed = true;
	if( put_table_fn_on_stack( table_name, fn_name ) )
	{
		_l->push_int( a );
		// the function takes 1 parameter and return nb_dst result
		bool b_ok = do_table_fn_central(	t,	__FUNCTION__, table_name, fn_name,	1,	nb_dst	);
		if( b_ok )
		{
			_l->get_vn( dst, -1, nb_dst );
			b_failed = false;
			if( g_lua_master->is_verbose_return() )
				_l->pop_str_and_print( table_name, fn_name );	//hack this is bad
			else
				_l->pop( nb_dst );
		}
	}
	STACK_ENFORCE_SAME( _l );
	if( b_failed )
		set_vn( dst, REAL(0.), nb_dst );

	lua_do_unlock();
}
void	c_lua_wrap::do_fn_fn_pass_i(	c_obj_ui* CONST t,	C_PCHAR_C table_name, C_PCHAR_C fn_name,
															INT32 CONST a, FP32* CONST dst, INT32 CONST nb_dst )
{
	lua_do_lock();
	STACK_ENFORCE_STORE( _l );
	bool	b_failed = true;
	if( put_table_fn_on_stack( table_name, fn_name ) )
	{
		_l->push_int( a );
		// the function takes 1 parameter and return nb_dst results
		bool b_ok = do_table_fn_central(	t,	__FUNCTION__, table_name, fn_name,	1,	nb_dst	);
		if( b_ok )
		{
			_l->get_vn( dst, -1, nb_dst );
			b_failed = false;
			if( g_lua_master->is_verbose_return() )
				_l->pop_str_and_print( table_name, fn_name );	//hack this is bad
			else
				_l->pop( nb_dst );
		}
	}
	STACK_ENFORCE_SAME( _l );
	if( b_failed )
		set_vn( dst, float(0.), nb_dst );

	lua_do_unlock();
}

void	c_lua_wrap::compile()
{
	if( _b_compiled_out )
		return;
	if( _text_ui.is_empty() )
		return;	
	// Load the command and try to execute it...
	if( _script_filename.is_empty() )
	{
//		if( !make_script_filename( _script_name_ui ) )
//		{
			debug_break( "%s() no script name", __FUNCTION__ );
			return;
//		}		
	}

	SPY_PUSH_RANGE2( "lua compile", spy::LUA, _script_filename );
		push_caller( this );
		bool b_ok = _l->compile( _text_ui, get_script_debug_name_str() ) == 0;
		pop_caller();
	SPY_POP_RANGE2();

	if( b_ok )			
	{
		if( _lua_fn_name.is_empty() )
		{
			_lua_fn_name.add_char( '_' );
			_lua_fn_name.add( get_obj_ui_id() );
			_lua_fn_name.add( "aaa" );
		}
		//	pop compiled chunk in a lua fn
		_l->get_global( str_table_compile_fn );
		_l->swap();
		lua_setfield( _l->get_state(), -2, _lua_fn_name.get() );
		_l->pop(1);
		_b_compiled_out = true;
	}
	else
	{
//		if( g_lua_master->is_compile_trig_dialog() )
//			g_lua_master->set_error_trig_dialog( true );
		if( is_text_direct() )
			_l->pop_str_and_print_error( "COMPILE TEXT", _l->get_script_debug_name_str() );
		else
		{
			o_str CONST & fname = get_script_debug_name();
			if( g_lua_master->is_error_trig_editor() && g_lua_master->is_error_trig_dialog() )
			{
				//hack quickly done : too quickly
				if( !fname.is_empty() )
				{
					INT32	line = 1;
					CHAR*	str;
					if( !_l->is_nil() )
					{
						str = (CHAR*) _l->get_str();
						str = str_move_past_char( str, ':' );
						sscanf( str, "%d:", &line );
					}
					g_lua_master->trig_edit_file( fname, line );
				}
			}
			_l->pop_str_and_print_error( "COMPILE FILE", fname.get() );
		}
	}
}

extern	C_PCHAR_C	str_table_compile_fn = "__aaa_fn";

void	c_lua_wrap::doit()
{
	lua_do_lock();
		bool	b_run;
		if( !_b_compiled_out )
		{
			compile();
			b_run = _b_compiled_out;
		}
		else
			b_run = !_b_run_only_when_compiled_ui || g_lua_master->is_run_always();

		if( b_run )
		{
			STACK_ENFORCE_STORE( _l );
				SPY_PUSH_RANGE( _script_name_ui, spy::LUA );
					// we get back the compiled chunk as a function
					_l->get_global( str_table_compile_fn );
					lua_getfield( _l->get_state(), -1, _lua_fn_name.get() );
					_l->swap();
					_l->pop(1);
					// the function takes 0 parameters and will return 1 result
					push_caller( this );
						auto lua_err = _l->pcall( 0, 1 );
					pop_caller();
				SPY_POP_RANGE();

				if( lua_err == LUA_OK )
				{
					if( g_lua_master->is_verbose_return() )
						_l->pop_str_and_print( get_script_debug_name_str() );
					else
						_l->pop_all();
				}
				else
				{
					C_PCHAR name = get_script_name().get();
					//C_PCHAR err_msg = _l->get_str(-1);
					//_l->pop(1);

					switch(lua_err)
					{
					case LUA_ERRRUN:
						_l->pop_str_and_print_error( name, "runtime error" );
						break;
					case LUA_ERRMEM:	
						{
							auto str = "memory allocation error from Lua. For such errors, Lua does not call the error handler function" ;
							if( g_lua_master->is_errmem_skip_dialog() && g_lua_master->is_error_trig_dialog() )
							{
								g_lua_master->set_error_trig_dialog( false );
								_l->pop_str_and_print_error( name, str );
								g_lua_master->set_error_trig_dialog( true );
							}
							else
								_l->pop_str_and_print_error( name, str );
						}
						break;
					case LUA_ERRERR:
						_l->pop_str_and_print_error( name, "error while running the error handler function." );
						break;
					default:
						//todo deakl with LUA_YIELD and LUA_ERRSYNTAX and LUA_ERRGCMM (lua 5.4) ?
						{
							CHAR str[256];
							sprintf( str, "Lua emitted a %d error undocumented", lua_err);
							_l->pop_str_and_print_error( name, str );
						}
						break;
					}
				}
			STACK_ENFORCE_SAME( _l )
			if( g_lua_master->is_dump_stack() )
				_l->print_stack_non_empty( _script_filename.get() );
		}

	lua_do_unlock();
}

void	c_lua_wrap::update()
{
	if( _b_edit_trig_ui )
	{
		g_lua_master->trig_edit_file( get_script_filename() );
		_b_edit_trig_ui = false;
	}

	if( _b_file_reload_trig_ui || g_lua_master->is_reload_all() )
	{
		_b_file_reload_trig_ui = false;
		_b_doit_trig_ui = true;
		_file_time = c_file::TIME_UNDEFINED;	//	force reload
	}

	if( _b_doit_ui || _b_doit_trig_ui )
	{
		bool b_check = false;
		if( g_lua_master->is_file_check() && _b_file_check_ui )
		{
			if( _b_file_check_always_ui )
				b_check = true;
			else
			{
				_check_file_counter = ++_check_file_counter & 0xf;
				b_check = _check_file_counter == (get_obj_ui_id() & 0xf);
			}
		}
		if( b_check || g_lua_master->is_force_recheck() || _file_time == c_file::TIME_UNDEFINED ) // means text not loaded yet
		{
			// g_lua_wrap_net call doit directly 
			//todo should we do a c_lua_wrap and a c_lua_wrap_ui ?
			// if( this == g_lua_wrap_net || check_script_file_changed() )
#if AAA_DEBUG()
			if( this == g_lua_wrap_net )
				debug_break( "in %s() with g_lua_wrap_net : this should not happen", __FUNCTION__ );
#endif
			if( _script_filename.is_empty() )
			{
				err_print( "%s() no script name", __FUNCTION__ );
//				make_script_filename( _script_name_ui ) ;	// load and save call it but useful when created by script (with the right root obj)
				return;
			}	
			if( c_file::is_time_changed( _script_filename, _file_time ) )
			{
				if( _file_time == c_file::TIME_UNDEFINED )
				{	//in case dir was move we try to regenerate script_file_name
					make_script_filename( _script_name_ui );
					c_file::is_time_changed( _script_filename, _file_time );
					if( _file_time == c_file::TIME_UNDEFINED )
					{
						err_print( "%s() no script file %s", __FUNCTION__, _script_filename.get() );
						return;
					}
				}
				if( ERR( _text_ui.read_file(_script_filename) ) )
				{
					_file_time = c_file::TIME_UNDEFINED;	//	force reload
				}
				_b_compiled_out = false;
			}
		}
		doit();
		_b_doit_trig_ui = false;
	}

	g_lua_wrap_cur = this;
}
//unused
/*
void	c_lua_wrap::load_lua( CHAR* filename_in )
{
	//CHAR	filename[AAA_PATH_MAX()];

	if( make_script_filename( filename_in ) )
	{
		c_file::push_vfile();
		_file_time = c_file::get_mdate( filename );
		_text_ui.read_file( filename );
		c_file::pop_vfile();
		_b_doit_trig_ui = true;
	}
	else
		debug_break( "%s() can't make script name", __FUNCTION__ );
}
*/

bool	c_lua_wrap::make_script_filename( o_str CONST & name_in )
{
//#if DEBUG
//	if( name_in.get_char(1)==':' )
//		DBG_PRINT_STRING( "name is absolute" );
//#endif
	return c_obj_ui::make_script_filename( _script_filename, name_in, "lua", &_script_debug_name );
}

//hack do it well
//here we just use the name and ignore the path
//we don't do any check
//we could for the .lua file do be in the same directory and have a message if we try otherwise
//perhaps a simple text edit would be better here
void	c_lua_wrap::set_script_filename_external( o_str CONST & name_in )
{
	//{
	//	o_str& filename_rel = o_str::push_name();
	//	//	place a relative path in the filename
	//	filename_rel.set_fname_relative( name_in );
	//	//		set_data_filename( filename_rel );
	//	o_str::pop_name();
	//}
	//c_obj_ui* fa = get_root();
	//fa->get_my_filename();

	_script_name_ui.set( name_in );
	make_script_filename( _script_name_ui );
	_b_file_reload_trig_ui = true;
}

AAA_ERR	c_lua_wrap::load_data_from_filename( o_str CONST & filename, INT32 CONST type_io )
{
	switch(type_io)
	{
	case aaa::file::TYPE_IO_LUA:
		set_script_filename_external( filename );
		return AAA_OK;
	}
	return ERR_TYPE_UNUSED;
}

AAA_ERR	c_lua_wrap::load_do_after( o_str CONST & filename_in )
{
	make_script_filename( _script_name_ui );
	if( _script_name_ui.is_empty() )
		_script_name_ui.set( _script_filename );
	return AAA_OK;
}



AAA_ERR	c_lua_wrap::save_do_after( o_str CONST & filename_in )
{
	if( g_lua_master->is_save() )	//	usually script are edited by the user in notepad++
	{
		bool b_name_ok = make_script_filename( _script_name_ui );
		if( b_name_ok )
		{
			c_file::push_vfile();
			_text_ui.write_file( get_script_filename_str() );
			//todo do we need it ? 
			//	_file_time = c_file::get_mdate( filename);
			c_file::pop_vfile();
		}
		else
			debug_break( "%s() can't make script name", __FUNCTION__  );
	}
	return AAA_OK;
}


//	unused check it
//bool	c_lua_wrap::callback_change( void* data )
//{
//	bool	b  = false;
//	if( ((void*)&_text_ui ) == data )
//		_b_compiled_out = false, b = true;	
//	return b;
//}

void c_lua_wrap::do_str( C_PCHAR_C str, C_PCHAR_C name )
{
	set_script_filename( name );
	set_text( str );
	set_text_direct( true );
	doit();
 }

c_lua_wrap*		g_lua_wrap_master = nullptr;
