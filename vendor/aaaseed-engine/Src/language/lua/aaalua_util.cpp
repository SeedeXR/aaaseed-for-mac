#include "aaalua_util.h"
#include "aaalua_wrap.h"
#include "aaalua_master.h"
#include "aaalua_debug.h"
#include "aaalua_glue.h"
#include "math/v_base.h"
#include "spy.h"

//extern "C" {
//#include "lua.h"
//#include "lualib.h"
//#include "lauxlib.h"
	// Include LuaSocket headers
#include "lua_socket/luasocket.h"
#include "lua_socket/mime.h"
//#include "lua_sec/ssl.h"
extern int luaopen_ssl_core(lua_State* L);
//}


namespace {
			INT32	lua_count = 0;
	CONST	INT32	LUA_HEADER_ERROR_NB = 3;
	C_PCHAR_C	LUA_HEADER_ERROR[LUA_HEADER_ERROR_NB] =
	{
		"# LUA   ",
		"# LUA - ",
		"# LUA | "
	};
/*
	C_PCHAR_C	lua_reserved_word[] =
	{
		"and",		"break",	"do",	"else",		"elseif",
		"end",		"false",	"for",	"function",	"if",
		"in",		"local",	"nil",	"not",		"or",
		"repeat",	"return",	"then",	"true",		"until",
		"while",	nullptr
	};
*/
}
CONSTEXPR	C_PCHAR_C	str_table_compile_fn = "__aaa_fn";


//ERROR
C_PCHAR_C	c_lua_state::get_lua_error_header()
{
	lua_count = (++lua_count) % LUA_HEADER_ERROR_NB ;
	return LUA_HEADER_ERROR[lua_count];
}

o_str*	c_lua_state::get_script_filename()
{
	c_obj_ui* lua_caller = c_lua_wrap::get_caller();
	if( lua_caller && c_lua_wrap::is_instance( lua_caller ) )
		return &((c_lua_wrap*) lua_caller)->get_script_filename();
	else
		return nullptr;
}

C_PCHAR_C	c_lua_state::get_script_debug_name_str()
{
	c_obj_ui* lua_caller = c_lua_wrap::get_caller();
	if( lua_caller && c_lua_wrap::is_instance( lua_caller ) )
		return ((c_lua_wrap*) lua_caller)->get_script_debug_name_str();
	else
		return nullptr;
}

void	c_lua_state::print_script_debug_name()
{
	C_PCHAR_C	script_filename = get_script_debug_name_str();
	if( script_filename )
		ERR_PRINT_STRING( "LUA in %s", script_filename );
}

//todo do better for callers using this (build using the root when needed
C_PCHAR_C	c_lua_state::get_obj_debug_str( c_obj_ui* CONST obj )
{
	C_PCHAR str;
	if( obj )
	{
		str = obj->get_name_search_str();
		if( !str || *str==0 )
			str = obj->get_class_name();
	}
	else
		str = "null object";
	return str;
}

void	c_lua_state::error_build_message( CHAR* CONST dst, INT32 CONST len, C_PCHAR_C mess ) CONST
{	//we assume 512 of Len
	if( _fn_name )
		snprintf( dst, len, "%.200s() : %.200s", _fn_name+3, mess );
	else
		snprintf( dst, len, "%s: %.200s", get_lua_error_header(), mess );
}

void	c_lua_state::error_message( C_PCHAR_C fmt, ... )  CONST
{
	va_list args;
	va_start( args, fmt );
	CHAR	mess[256];
	vsnprintf( mess, 255, fmt, args );
	va_end(args);

	CHAR	str[512];
	error_build_message( str, sizeof(str)-1, mess );
	ERR_PRINT_STRING( str );
}

void	c_lua_state::error_and_escape( C_PCHAR_C fmt, ... )	 CONST
{
	va_list args;
	va_start( args, fmt );
	CHAR	mess[256];
	vsnprintf( mess, 255, fmt, args );
	va_end( args );

	luaL_error( _L, mess );
/*
	g_lua_master->inc_error();
	CHAR	str[512];
	error_build_message( str, sizeof(str)-1, mess );

	BOX_ERR( aaalua_debug_build_trace( str ) );
	aaalua_debug_set_level(0);
*/
}

namespace {
	//todo better : can it overflow
	CHAR	buf_pop_str[8*1024];
	CHAR*	buf_pop_str_end = buf_pop_str + 7*1024;	//hack but decent protection
	CHAR	title_pop_str[4096];
}

//todo cup this what happened if nothing on the stack or other errors
void	c_lua_state::pop_str_and_message( C_PCHAR_C header, bool CONST b_edit ) CONST
{
	HEADER_PRINT_STRING( get_lua_error_header(), header );
// There was a lua error or a return value to fetch from the stack
	C_PCHAR	str		= nullptr;
	PCHAR	buf_cur	= nullptr;

	if( !is_nil() )
	{
		str = get_str();
		if( str )
		{
			buf_cur = buf_pop_str;
			*buf_cur = 0;

			CHAR*	mark = (CHAR*) str;
			CHAR*	cur = mark-1;
			while( true )
			{
				CHAR c = *++cur;
				if( c < 32 )
				{
					bool b_break = (*cur==0) || buf_cur > buf_pop_str_end;
					*cur = 0;
					if( mark != cur )
					{
						// make more readable by removing enclosing "[string " and "]"
						if( str_is_equal( mark, "[string ", 8 ) )
						{
							mark = mark + 8;
							{
								CHAR* p = mark;
								INT32	state = 0;
								while( *++p >= 32 )
								{
									if( state==0 )
									{
										if( *p==']' )
										{
											state=1;
											*p = *(p+1);
										}
									}
									else
										*(p-1) = *p;
								}
								if( state!=0 )
									*(p-1) = *p;
								buf_cur += sprintf( buf_cur, "%s\n", mark );
							}
						}
						else
							buf_cur += sprintf( buf_cur, "%s\n", mark );
						HEADER_PRINT_STRING( LUA_HEADER_ERROR[lua_count], mark );
					}
					if( b_break )
						break;
					*cur = c;
					mark = cur+1;
				}
			}
		}
		pop(1);
	}

	if( g_lua_master->is_error_trig_dialog() )
	{
		//edit_file( CHAR* filename = nullptr, INT32 line=-1 );
		//sprintf( title, "Old AAASeed LUA %s", header );
		//if( !BOX_ASK_WAR( title, buf ) )
		//	g_lua_master->clear_error_trig_dialog();

		INT32 line	= -42;
		sprintf( title_pop_str, "AAASeed LUA %s", header );
		if( str )
		{
			buf_cur = str_move_past_char( buf_pop_str, ':' );
			if( aaalua_debug_get_level() > 0 )
			{	// we get the info with aaalua_debug_build_trace so we can strip the buf
				buf_cur = str_move_past_char( buf_cur, ':' );
				CHAR*	buf_end = str_find_str( buf_cur, "stack traceback" );
				if( buf_end )
					*buf_end = 0;
			}
			else
			{
				//	we can get the line nb from the str between the 2 :
				sscanf( buf_cur, "%d:", &line );
				buf_cur = buf_pop_str;
			}
		}
		else
		{
			buf_cur = (PCHAR)"AAASeed didn't get a str with error message from lua.\nSo Maa ???";
		}

		CHAR* trace					= aaalua_debug_build_trace( nullptr );
		o_str CONST &  script_name	= aaalua_debug_get_script_name_o_str();
		if( line <= 0 )
			line = aaalua_debug_get_line();
		c_obj_ui* lua_caller	= c_lua_wrap::get_caller();
		bool b_text_direct = lua_caller && c_lua_wrap::is_instance( lua_caller ) && ((c_lua_wrap*) lua_caller)->is_text_direct();
		if( b_edit && !b_text_direct && g_lua_master->is_error_trig_editor() && g_lua_master->is_error_trig_dialog() )
		{
			if( !script_name.is_empty() )
				g_lua_master->trig_edit_file( script_name, line );
			else if( lua_caller && c_lua_wrap::is_instance( lua_caller ) )
				g_lua_master->trig_edit_file( ((c_lua_wrap*) lua_caller)->get_script_debug_name(), line );
		}
		bool b;
		if( b_text_direct )
			b = BOX_LUA( title_pop_str, "%s\n%s\n%sin\n%s", header, buf_cur, trace, ((c_lua_wrap*) lua_caller)->get_text().get() );
		else
			b = BOX_LUA( title_pop_str, "%s\n%s\n%s", header, buf_cur, trace );
		if( !b )
			g_lua_master->set_error_trig_dialog( false );
	}
	aaalua_debug_set_level( 0 );
	g_lua_master->inc_error();
}

void	c_lua_state::pop_str_and_print( C_PCHAR_C arg1 ) CONST
{
	o_str& o = o_str::push_name();
		o.set( arg1 );
		o.add( " returned" );
		pop_str_and_message( o.get() );
	o_str::pop_name();
	
	//CHAR mess[1024];
	//sprintf( mess, "%.512s returned ", arg1 );
	//pop_str_and_message( mess );
}

//todo cup this what happened if nothing on the stack or other errors
void	c_lua_state::pop_str_and_print( C_PCHAR_C arg1, C_PCHAR_C arg2 ) CONST
{
	o_str& o = o_str::push_name();
		o.set( arg1 );
		o.add( "(\"" );
		o.add( arg2 );
		o.add( "\") returned" );
		pop_str_and_message( o.get() );
	o_str::pop_name();

	//CHAR	mess[1024+100];
	//sprintf( mess, "%.512s(\"%.512s\") returned ", arg1, arg2 );
	//pop_str_and_message( mess );
}

void	c_lua_state::pop_str_and_print_error(  C_PCHAR_C error, C_PCHAR_C pt1, C_PCHAR_C pt2, C_PCHAR_C pt3 ) CONST
{
	CHAR format[1024];
	if( pt1 )
	{
		if( pt2 )
		{
			if( pt3 )
				snprintf( format, sizeof(format)-1, "ERROR %.64s : %.256s(\"%.256s.%.256s\") ", error, pt1, pt2, pt3 ); 
			else
				snprintf( format, sizeof(format)-1, "ERROR %.64s : %.256s(\"%.256s\") ", error, pt1, pt2 ); 
		}
		else
			snprintf( format, sizeof(format)-1, "ERROR %.64s : %.512s ", error, pt1 );
	}
	else
		snprintf( format, sizeof(format)-1, "ERROR %.64s : ", error );
	SET_COLOR_ERR();
	pop_str_and_message( format, strcmp( "COMPILE FILE", error )!=0 );
	SET_COLOR_NORMAL();
}

namespace {
	int load_jit_module( lua_State* L, C_PCHAR notfound )
	{
		lua_getglobal( L, "require" );
		lua_pushliteral( L, "jit." );
		lua_pushvalue( L, -3 );
		lua_concat( L, 2 );
		int err = lua_pcall( L, 1, 1, 0 );

		if( err )
		{
			CONST char* msg = lua_tostring( L, -1 );
			if( msg ) //&& str_is_equal(msg, "module ", 7) )
			{
				ERR_PRINT_STRING( "LUA : %s", notfound );
				BOX_ERR( "%s\n%s", notfound, msg );
			}
			//	return report( L, 1 );
			lua_remove( L, -1 );
			lua_remove( L, -1 );
			return 1;
		}
		lua_getfield( L, -1, "start" );
		lua_remove( L, -2 );  // drop module table
		return 0;
	}

	// start optimizer
	int do_jit_opt( lua_State* L, char CONST * opt )
	{
		GOOD_PRINT_STRING( "starting load jit optimizer module" );
		lua_pushliteral( L, "opt" );
		if( load_jit_module( L, "LuaJIT optimizer module not installed" ) )
			return 1;
		lua_remove( L, -2 );  // drop module name
		if( *opt )
			lua_pushstring( L, opt );
		//was	return report(L, lua_pcall( L, *opt ? 1 : 0, 0, 0 ) );
		INT32 ret = lua_pcall( L, *opt ? 1 : 0, 0, 0 );
		if( ret==0 )
		{
			lua_getglobal( L, "jit" );
			lua_getfield( L, -1, "version" );	
			//  stack : jit | jit.version
			C_PCHAR pt_ver = lua_tostring( L, -1 );
			GOOD_PRINT_STRING( "Lua optimizer module loaded : %s", pt_ver );
			lua_pop( L, 2 );
		}
		return ret;
	}
}

bool luaMC_getglobal2 (lua_State * L, const char *name1, const char *name2)
{
	lua_getglobal (L, name1);
	if (!lua_isnil (L, -1))
	{
		lua_getfield (L, -1, name2);
		lua_remove (L, -2);
	}
	return !lua_isnil (L, -1);
}

void
luaMaa_requiref( lua_State * L, c_lua_state* l, const char *modname, lua_CFunction openf, const char *global_name )
{
	STACK_ENFORCE_EMPTY_BEGIN( l );
	luaMC_getglobal2( L, "package", "loaded" );

	lua_pushcfunction (L, openf);
	lua_pushstring (L, modname);
	lua_call( L, 1, 1 );
	l->set_global( global_name );

//	lua_setfield( L, -2, modname );

	lua_pop( L, 1 );             /* package.loaded */
	STACK_ENFORCE_EMPTY_END( l );
}
// Function to register LuaSocket's core module
void open_lua_socket( lua_State* L, c_lua_state* l )
{
//	STACK_ENFORCE_EMPTY_BEGIN( l );

	//luaopen_socket_core( L );
	//// Register the socket core module
	luaMaa_requiref( L, l, "socket.core", luaopen_socket_core, "socket" );	//, 1 );
	luaMaa_requiref( L, l, "mime.core",   luaopen_mime_core,    "mime"  );	//, 1 );
//	luaMaa_requiref( L, l, "ssl.core",    luaopen_ssl_core,     "ssl"   );

//	lua_pop(L, 1); // Remove library from stack after registration

	//// Register the MIME core module
	//luaMC_requiref( L, "mime.core", luaopen_mime_core);	//, 1 );
	//lua_pop(L, 1); // Remove library from stack after registration

	// Additional LuaSocket modules can be registered here similarly
//	STACK_ENFORCE_EMPTY_END( l );
}

bool	c_lua_state::open()
{
	if( _L )
		return true;

	_L = luaL_newstate();
	if( _L )	
	{
		STACK_ENFORCE_EMPTY_BEGIN( this );

		luaL_openlibs( _L );	// opens standard lib : base, table, io, os, string, math, debug, package

		open_lua_socket( _L, this );

		//luaopen_socket_core( _L );
		//luaopen_mime_core( _L );

//	example to open individual lib
//		luaopen_table( _L );
//		luaopen_io( _L );		// trigger a crash, but do we need io here

		pop_all();	// the previous luaopen left table on the stack

		lua_getglobal( _L, "_VERSION" );
		C_PCHAR pt_ver = lua_tostring( _L, -1 );
		GOOD_PRINT_STRING( "Lua opened : %s", pt_ver );
		lua_pop( _L, 1 );

		//	load jit optimizer module
		do_jit_opt( _L, "" );

		define_table_global( str_table_compile_fn );

		aaalua::register_all_fn( _L );

		STACK_ENFORCE_EMPTY_END( this );
	}
	return _L != 0;
}


void	c_lua_state::close()
{
	if( _L )
	{
//		lua_setgcthreshold(L, 0);  // collected garbage
		STACK_ENFORCE_EMPTY_BEGIN( this );
		aaalua::unregister_all_fn( _L );
		STACK_ENFORCE_EMPTY_END( this );
		lua_close( _L );
		_L = nullptr;
	}
}

C_PCHAR_C	c_lua_state::get_type_str( INT32 type )
{
	switch (type)
	{
	case LUA_TNIL:				return "Nil";			break;
	case LUA_TBOOLEAN:			return "Boolean";		break;
	case LUA_TLIGHTUSERDATA:	return "LightUserData";	break;
	case LUA_TNUMBER:			return "Number";		break;
	case LUA_TSTRING:			return "String";		break;
	case LUA_TTABLE:			return "Table";			break;
	case LUA_TFUNCTION:			return "Function";		break;
	case LUA_TUSERDATA:			return "UserData";		break;
	case LUA_TTHREAD:			return "Thread";		break;
	default:					return "None";			break;
	}
}

namespace {
	void aaalua_stack_dump( lua_State* L, C_PCHAR_C mess, bool CONST b_only_non_empty )
	{
		INT32	top = lua_gettop(L);
		if( top )
		{
			if( mess && *mess )
				VERBOSE_PRINT_STRING( "lua stack %d element : %s", top, mess );
			else
				VERBOSE_PRINT_STRING( "lua stack have %d elements", top );
			for( INT32 i = 1; i <= top; ++i )
			{
				INT32 t = lua_type( L, i );
				switch (t)
				{
				case LUA_TNIL:				VERBOSE_PRINT_STRING( "nil" );														break;
				case LUA_TBOOLEAN:			VERBOSE_PRINT_STRING( "bool\t`%s'",		lua_toboolean(L, i) ? "true" : "false" );	break;
				case LUA_TLIGHTUSERDATA:	VERBOSE_PRINT_STRING( "userdata\t0x%x",	lua_topointer(L, i) );						break;
				case LUA_TNUMBER:			VERBOSE_PRINT_STRING( "number\t%g",		lua_tonumber(L, i) );						break;
				case LUA_TSTRING:			VERBOSE_PRINT_STRING( "string\t`%s'",	lua_tostring(L, i) );						break;
				case LUA_TTABLE:			VERBOSE_PRINT_STRING( "table\t0x%x",	lua_topointer(L, i) );						break;
				case LUA_TFUNCTION:			VERBOSE_PRINT_STRING( "function\t0x%x",	lua_topointer(L, i) );						break;
				case LUA_TUSERDATA:			VERBOSE_PRINT_STRING( "userdata\t0x%x",	lua_topointer(L, i) );						break;
				case LUA_TTHREAD:			VERBOSE_PRINT_STRING( "thread\t0x%x",	lua_topointer(L, i) );						break;
				default:					VERBOSE_PRINT_STRING( "\t%s",			lua_typename(L, t) );						break;
				}
			}
		}
		else if( !b_only_non_empty )
		{
			if( mess && *mess )	{	VERBOSE_PRINT_STRING( "lua stack : %s", mess );	}
			VERBOSE_PRINT_STRING( "lua stack is empty", top );
		}
	}
}

void c_lua_state::print_stack(				C_PCHAR_C mess ) CONST	{	aaalua_stack_dump( _L, mess, false );	}
void c_lua_state::print_stack_non_empty(	C_PCHAR_C mess ) CONST	{	aaalua_stack_dump( _L, mess, true );	}


bool c_lua_state::check_stack( C_PCHAR_C mess ) CONST
{
	if( get_top()!=0 )
	{
		print_stack( mess );
		return false;
	}
	return true;
}

void c_lua_state::enforce_stack( INT32 CONST top, C_PCHAR_C fn_name, C_PCHAR_C mess ) CONST
{
	INT32 top_now = get_top();
	if( top_now != top )
	{
		CHAR str[512];
		snprintf( str, sizeof(str)-1, "%.240s() %.240s", fn_name, mess );

		print_stack( str );
		if( top==0 )
		{
			debug_break( "LUA non empty stack" );
			pop_all();
		}
		else
		{
			debug_break( "LUA stack change from %d to %d", top, top_now );
			if( top_now  > top )
			{
				pop( top_now - top );
			}
		}
	}
}

namespace {
	int __cdecl	traceback (lua_State* L)
	{
		aaalua_debug_grab_info( L );
		// call lua debug.traceback()
		lua_getglobal( L, "debug" );
		if( !lua_istable( L, -1 ) )
		{
			lua_pop( L, 1 );
			return 1;
		}
		lua_getfield( L, -1, "traceback" );
		if( !lua_isfunction(L, -1 ) )
		{
			lua_pop( L, 2 );
			return 1;
		}
		lua_pushvalue( L, 1 );		//	pass error message
		lua_pushinteger( L, 2 );	//	skip this function and traceback
		lua_call( L, 2, 1 );		//	call debug.traceback
		return 1;
	}

//READER

	typedef struct lua_memfile
	{
		CONST char*	text;
		size_t		size;
	} luaMemFile;

	CHAR CONST * aaalua_reader( lua_State*, void *ud, size_t *size )
	{
		// Convert the ud pointer (UserData) to a pointer of our structure
		lua_memfile* mf = (lua_memfile*) ud;
	
		// Are we done?
		if( mf->size == 0 )
			return nullptr;
	
		// Read everything at once
		// And set size to zero to tell the next call we're done
		*size = mf->size;
		mf->size = 0;
	
		// Return a pointer to the read text
		return mf->text;
	}
}

INT32	c_lua_state::compile( o_str& o, C_PCHAR_C name ) CONST
{
	//todo do it directly with o_str
	lua_memfile mf;
	mf.size = o.get_len();
	mf.text = o.get();

	if( g_lua_master->is_print_compile() )
		DBG_PRINT_STRING( "Lua compile %s", name );

	INT32 status;
	TBUF_ADD( tbuf::CH_LUA, 1., "lua_compile" );
	/*	traceback don't seem to work for compile which is not lua 
	if( g_lua_master->is_traceback() )
	{
		print_stack( "before lua_load() before add traceback" );
		lua_pushcfunction( _L, traceback );		//	push traceback function
		print_stack( "before lua_load()" );
		//	signal( SIGINT, laction );
		status = lua_load( _L, aaalua_reader, &mf, name );
		print_stack( "after lua_load()" );
		//	signal(SIGINT, SIG_DFL);
		lua_remove( _L, 1 );					//	remove traceback function
		print_stack( "after lua_load() lua_remove()" );
	}
	else
	*/
	{
		status = lua_load( _L, aaalua_reader, &mf, name );
	}

	// force a complete garbage collection in case of errors
	if ( status != 0 )
		lua_gc( _L, LUA_GCCOLLECT, 0 );
	TBUF_ADD( tbuf::CH_LUA, 0., nullptr );

	return status;
}

//but it don't seem to work because there is not exception in the lua lib which is C not C++
#define LUA_EXCEPTION 0

INT32	c_lua_state::pcall( INT32 CONST arg_nb, INT32 CONST result_nb ) CONST
{
	INT32 status;
	{
		TBUF_ADD_OBJ( tbuf::CH_LUA, 1., "lua pcall", c_lua_wrap::get_caller() );

#if LUA_EXCEPTION
		try 
		{
#endif
			if( g_lua_master->is_print_pcall() )
			{
				auto caller = c_lua_wrap::get_caller();
				if( caller && caller->is_class_name( "lua_wrap" ) )
				{
					auto lw = reinterpret_cast<c_lua_wrap*>(caller);
					DBG_PRINT_STRING( "pcall by %s", lw->get_script_debug_name_str() );
				}
			}

#define AAA_USE_PCALL() 1
#if AAA_USE_PCALL()	// we use pcall
			if( g_lua_master->is_traceback() )
			{ 
				INT32 base = lua_gettop(_L) - arg_nb;	// function index
				//print_stack( "before lua_pcall() add traceback" );
				lua_pushcfunction( _L, traceback );		//	push traceback function
				lua_insert( _L, base );					//	put it under chunk and args */
				//print_stack( "before lua_pcall()" );
				//	signal( SIGINT, laction );
				status = lua_pcall( _L, arg_nb, result_nb, base );	
				//print_stack( "after lua_pcall()" );
				//	signal( SIGINT, SIG_DFL );
				lua_remove( _L, base );					//	remove traceback function
				// force a complete garbage collection in case of errors
			}
			else
				status = lua_pcall( _L, arg_nb, result_nb, 0 );
#else	// we use call temporarly to debug in Visual Studio
			lua_call( _L, arg_nb, result_nb );
#endif
#if LUA_EXCEPTION
		}
		catch( const char *s )
		{  // Catch and convert exceptions.
			ERR_PRINT_STRING( "Lua caught exception string : %s", s );
			status = LUA_ERRRUN;
		}
		catch( std::exception& e )
		{
			ERR_PRINT_STRING( "Lua caught exception what field is  : %s", e.what() );
			status = LUA_ERRRUN;
		}
		catch(...)
		{
			ERR_PRINT_STRING( "Lua caught exception (...)" );
			status = LUA_ERRRUN;
		}
#endif

#if AAA_USE_PCALL()	// we use pcall
		if( status != LUA_OK )
		{
			DBG_PRINT_STRING( "Lua pcall Failed : we trigger lua_gc() to clean" );
			lua_gc( _L, LUA_GCCOLLECT, 0 );	// 0 mean nothing with LUA_GCCOLLECT
		}
#else
		status = LUA_OK;
#endif

		TBUF_ADD( tbuf::CH_LUA, 0., nullptr );
	}

	return status;
}

void	c_lua_state::add_fn_to_table( C_PCHAR_C name, lua_CFunction CONST fn, C_PCHAR_C str_help ) CONST
{
	push_string( name );
	lua_pushcfunction( _L, fn );
	set_table( -3 );

	if( str_help )
	{
		get_table_always( "doc" );
		set_field( name, str_help );
		pop(1);
	}
}

namespace aaalua
{
	int new_metatable( lua_State* CONST L, C_PCHAR tname )
	{
		lua_getfield( L, LUA_REGISTRYINDEX, tname );  // get registry.name
		if( !lua_isnil( L, -1 ) )  // name already in use ?
			return 0;  // leave previous value on top, but return 0
		lua_pop( L, 1 );
		lua_newtable( L );  // create metatable
		lua_pushvalue( L, -1 );
		lua_setfield( L, LUA_REGISTRYINDEX, tname );  // registry.name = metatable
		return 1;
	}
}

//////////////////////////////////////////////////////////////////////////
//UNUSED ????

/*
static void settabss (lua_State *L, CONST char *i, CONST char *v) 
{
	lua_pushstring(L, i);
	lua_pushstring(L, v);
	lua_rawset(L, -3);
}

static void settabsi (lua_State *L, CONST char *i, int v) 
{
	lua_pushstring(L, i);
	lua_pushnumber(L, (lua_Number)v);
	lua_rawset(L, -3);
}

static int getinfo (lua_State *L) 
{
	lua_Debug ar;
	CONST char *options = luaL_optstring( L, 2, "flnSu" );
	if( lua_isnumber( L, 1 ) )
	{
		if( !lua_getstack( L, (int)(lua_tonumber( L, 1 )), &ar ) )
		{
			lua_pushnil(L);  // level out of range
			return 1;
		}
	}
	else if( lua_isfunction( L, 1 ) )
	{
		lua_pushfstring( L, ">%s", options );
		options = lua_tostring( L, -1 );
		lua_pushvalue( L, 1 );
	}
	else
		return luaL_argerror( L, 1, "function or level expected" );
	if( !lua_getinfo( L, options, &ar ) )
		return luaL_argerror( L, 2, "invalid option" );
	lua_newtable( L );
	for( ; *options; options++ )
	{
		switch( *options )
		{
		case 'S':	settabss( L, "source", ar.source );
					settabss( L, "short_src", ar.short_src );
					settabsi( L, "linedefined", ar.linedefined );
					settabss( L, "what", ar.what );
					break;
		case 'l':	settabsi( L, "currentline", ar.currentline );
					break;
		case 'u':	settabsi( L, "nups", ar.nups );
					break;
		case 'n':	settabss( L, "name", ar.name );
					settabss( L, "namewhat", ar.namewhat );
					break;
		case 'f':	lua_pushliteral( L, "func" );
					lua_pushvalue( L, -3 );
					lua_rawset( L, -3 );
					break;
		}
	}
	return 1;  // return table
}
*/

namespace {
INT32 CONSTEXPR NB_MAX_ON_STACK = 1024*4;
}

template<typename T>
void	c_lua_state::get_vn_table( T* dst, INT32 CONST index, INT32 nb, INT32 CONST first ) CONST
{
	if( index <= 0 )
		error_and_escape( "%s() can't be called with a null or negative index.", __FUNCTION__ );
	--dst;
	INT32 i_beg = 1 + first; 
	do
	{	
		INT32 CONST loop_nb = MIN( nb, NB_MAX_ON_STACK );
		INT32 CONST i_end = i_beg + loop_nb;
		for( INT32 i=i_beg; i<i_end; ++i )
		{	
			get_table_field_raw( index, i );
			*++dst = get_value_direct<T>();
		}
		pop( loop_nb ); //we have to do this to avoid stack overflows
		nb -= loop_nb;
		i_beg = i_end;
	}
	while( nb > 0 );
}
#if AAA_REAL_IS_DOUBLE()
template void	c_lua_state::get_vn_table<REAL>( REAL* dst, INT32 CONST index, INT32 nb, INT32 CONST first ) CONST;
#endif
template void	c_lua_state::get_vn_table<FP32>( FP32* dst, INT32 CONST index, INT32 nb, INT32 CONST first ) CONST;

template<typename T>
void	c_lua_state::get_v3n_table_repeat_3( T* dst, INT32 CONST index, INT32 nb ) CONST
{
	if( index <= 0 )
		error_and_escape( "%s() can't be called with a null or negative index.", __FUNCTION__ );
	--dst;
	INT32 nb_on_stack = 0;
	INT32 id = 0;
	for( INT32 i=1; i<=nb; ++i )
	{
		// get 3 floats
		// copy it twice
		get_table_field_raw( index, ++id );
		*(dst+6) = *(dst+3)	= *dst = get_value_direct<T>();
		get_table_field_raw( index, ++id );
		*(dst+7) = *(dst+4)	= *(dst+1) = get_value_direct<T>();
		get_table_field_raw( index, ++id );
		*(dst+8) = *(dst+5)	= *(dst+2) = get_value_direct<T>();
		dst += 9;
		nb_on_stack += 3;
		if( nb_on_stack >= NB_MAX_ON_STACK )	//we have to do this to avoid stack overflows
		{
			pop( nb_on_stack );
			nb_on_stack = 0;
		}
	}
	pop( nb_on_stack );
}

#if AAA_REAL_IS_DOUBLE()
template void	c_lua_state::get_v3n_table_repeat_3<REAL>( REAL* dst, INT32 CONST index, INT32 nb ) CONST;
#endif															 
template void	c_lua_state::get_v3n_table_repeat_3<FP32>( FP32* dst, INT32 CONST index, INT32 nb ) CONST;
