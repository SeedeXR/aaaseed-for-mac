#include "aaa_dir_lua.h"
#include "aaa_file_lua.h"
#include "file/dirparser.h"
#include "aaa_dir.h"

namespace aaalua
{
namespace n_dir
{
	// Check Directory
	AAALUACALL( is_exist )
	{
		LUAAAA_START( L, __FUNCTION__ )
		l.check_arg_nb( 1 );
		C_PCHAR_C dir_name = l.get_str();
		if( dir_name )
			return l.return_bool( c_dir::is_exist( dir_name ) );
		l.error_message( "directory name not a string" );
		return l.return_nothing();
	}

	//todo return err_str
	// Create Directory
	AAALUACALL( create )
	{
		LUAAAA_START( L, __FUNCTION__ )
		l.check_arg_nb( 1 );
		C_PCHAR_C dir_name = l.get_str();
		if( dir_name )
			return l.return_bool( NOERR( c_dir::make( dir_name ) ) );
		l.error_message( "directory name not a string" );
		return l.return_nothing();
	}

	//todo return err_str
	// Remove Directory Recursively
	AAALUACALL( remove )
	{
		LUAAAA_START( L, __FUNCTION__ )
		l.check_arg_nb( 1 );
		C_PCHAR_C dir_name = l.get_str();
		if( dir_name )
			return l.return_bool( NOERR( c_dir::remove( dir_name ) ) );
		l.error_message( "directory name not a string" );
		return l.return_nothing();
	}

	//	input	none
	AAALUACALL(	set_def )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		C_PCHAR_C dir_name = l.get_str();
		if( dir_name )
		{
			c_dir::set_def( dir_name );
			return l.return_nothing();
		}
		l.error_message( "directory name not a string" );
		return l.return_nothing();
	}

	//	input	none
	AAALUACALL(	push_def )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		C_PCHAR_C dir_name = l.get_str();
		if( dir_name )
		{
			c_dir::push_def( dir_name );
			return l.return_nothing();
		}
		l.error_message( "directory name not a string" );
		return l.return_nothing();
	}

	//	input	none
	AAALUACALL(	pop_def )
	{
		LUAAAA_START( L, __FUNCTION__ );
		c_dir::pop_def();
		return l.return_string( c_dir::get_def() );
	}

	//	input	none
	AAALUACALL(	get_def )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_string( c_dir::get_def() );
	}

	AAALUACALL(	get_dir_start )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_string( c_dir::get_start() );
	}
	AAALUACALL(	get_dir_kernel )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_string( c_dir::get_kernel() );
	}
	AAALUACALL(	get_dir_user )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_string( c_dir::get_user() );
	}
	AAALUACALL(	get_dir_net )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_string( c_dir::get_net() );
	}

	//	input	path pattern
	template< bool B_DIR > 
	FINLINE INT32 get_dirs_or_files( lua_State* L, C_PCHAR_C fn_name )
	{
		LUAAAA_START( L, fn_name );
		INT32 CONST arg_nb = l.get_arg_nb( 1, 2 );

		C_PCHAR	str	= l.get_str( 1 );
		if( !str )
		{
			l.error_message( "directory name not a string" );
			return l.return_nothing();
		}
		C_PCHAR	pattern = ( arg_nb==2 && !l.is_nil(2)) ? l.get_str( 2 ) : pattern = "*.*";
		if( !pattern )
		{
			l.error_message( "pattern not a string" );
			return l.return_nothing();
		}

		INT32	i = 0;
		c_dir_parser pa( (CHAR*)str, pattern );
		if( B_DIR )
		{
			while( pa.get_next_dir() )
			{
				//PRINT_STRING( "  %s\n", pa.get_name() );
				if( i==0 )
					l.new_table();	//	create result table
				l.set_field( ++i, pa.get_name() );
			}
		}
		else
		{
			while( pa.get_next_file() )
			{
				//PRINT_STRING( "  %s\n", pa.get_name() );
				if( i==0 )
					l.new_table();	//	create result table
				l.set_field( ++i, pa.get_name() );
			}
		}
		if( i > 0 )
			return l.return_table();
		return l.return_nothing();
	}

	AAALUACALL( get_files )	{	return get_dirs_or_files< false >( L, __FUNCTION__ );	}
	AAALUACALL( get_dirs )	{	return get_dirs_or_files< true  >( L, __FUNCTION__ );	}
/*
int l_dir (lua_State* L)
{
	DIR *dir;
	struct dirent *entry;
	int i;
	CONST char *path = luaL_checkstring(L, 1);
	
	//	open directory
	dir = opendir(path);
	if( dir == nullptr ) {	//	error opening the directory?
		lua_pushnil(L);		//	return nil and ...
		lua_pushstring(L, strerror(errno));	// error message
		return 2; //	number of results
	}
	
	//	create result table
	lua_newtable(L);
	i = 1;
	while ((entry = readdir(dir)) != nullptr) {
		lua_pushnumber(L, ++i );				// push key
		lua_pushstring(L, entry->d_name);	// push value
		lua_settable(L, -3);
	}
	
	closedir(dir);
	return l.return_table();	// table is already on top
}
*/

/*

//forward declaration for the iterator function
int dir_iter );

c_dir_parser	parser;

int	l_dir )
{
	C_PCHAR path;
	path = luaL_checkstring( L, 1);

	//	create a userdatum to store a DIR address
	c_dir_parser**	hdp;
	hdp = (c_dir_parser**) lua_newuserdata( L, sizeof(c_dir_parser*) );
	*hdp = new c_dir_parser;
	
	//	set its metatable
	luaL_getmetatable( L, "LuaBook.dir" );
	lua_setmetatable( L, -2);

	//	try to open the given directory
	(*hdp)->set_dir( (CHAR*) path);
//	if( !(*d) )	//	error opening the directory
//		luaL_error(L, "cannot open %s: %s", path, strerror(errno));

//	creates and returns the iterator function
//	(its sole upvalue, the directory userdatum,
//	is already on the stack top
	lua_pushcclosure( L, dir_iter, 1);

	return l.return_table();
}

//	A subtle point here is that we must create the userdatum before opening the directory.
//	If we first open the directory, and then the call to lua_newuserdata raises an error,
//	we lose the DIR structure. With the correct order, the DIR structure, once created,
//	is immediately associated with the userdatum; whatever happens after that,
//	the __gc metamethod will eventually release the structure.

//	The next function is the iterator itself:

static int dir_iter )
{
	c_dir_parser* d = *(c_dir_parser **)lua_touserdata(L, lua_upvalueindex(1));


	if( d->get_next() )
	{
		lua_pushstring(L, d->get_name() );
		return 1;
	}
	else
		return 0;	//	no more values to return
}

//	The __gc metamethod closes a directory,
//	but it must take one precaution: Because we create the userdatum before opening the directory,
//	this userdatum will be collected whatever the result of opendir. If opendir fails, there will be nothing to close.

static int dir_gc (lua_State* L)
{
	c_dir_parser* d = *(c_dir_parser **)lua_touserdata(L, 1);
	if( d )
		delete d;
	return 0;
}

// Finally, there is the function that opens this one-function library:
int luaopen_dir (lua_State* L)
{
	luaL_newmetatable(L, "LuaBook.dir");

	//	set its __gc field
	lua_pushstring(L, "__gc");
	lua_pushcfunction(L, dir_gc);
	lua_settable(L, -3);

	//	register the `dir' function
	lua_pushcfunction(L, l_dir);
	lua_setglobal(L, "dir");

	return 0;
}
*/
	void	register_dir( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		l.define_table( "dir" );

			ADD_FN(		is_exist		);
			ADD_FN(		create			);
			ADD_FN(		remove			);
			l.add_fn_to_table( "rename",	n_file::rename );
			l.add_fn_to_table( "move",		n_file::move );

			ADD_FN(		get_def			);
			ADD_FN(		set_def			);
			ADD_FN(		push_def		);
			ADD_FN(		pop_def			);

			ADD_FN(		get_dir_start	);
			ADD_FN(		get_dir_kernel	);
			ADD_FN(		get_dir_user	);
			ADD_FN(		get_dir_net		);

			ADD_FN(		get_files		);
			ADD_FN(		get_dirs		);
		
		l.pop( 1 );	//pop new table
	}
}	//end namespace n_dir
}	//end namespace aaalua
