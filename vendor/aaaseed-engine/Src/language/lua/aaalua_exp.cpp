
#include "aaalua_exp.h"
#include "aaalua_glue.h"

void tag_error( lua_State *L, int narg, int tag )
{
	luaL_typerror( L, narg, lua_typename(L, tag) );
}

#if 0 
static int libsize( const luaL_Reg* l )
{
	int size = 0;
	for ( ; l->name; ++l )
		++size;
	return size;
}

LUALIB_API void luaI_openlib( lua_State* L, const char* libname, const luaL_Reg* l, int nup )
{
	if( libname )
	{
		int size = libsize(l);
		/* check whether lib already exists */
		luaL_findtable( L, LUA_REGISTRYINDEX, "_LOADED", 1 );
		lua_getfield( L, -1, libname );  /* get _LOADED[libname] */
		if( !lua_istable( L, -1 ) )
		{  /* not found? */
			lua_pop( L, 1 );  /* remove previous result */
			/* try global variable (and create one if it does not exist) */
			if( luaL_findtable( L, LUA_GLOBALSINDEX, libname, size ) )
				luaL_error( L, "name conflict for module " LUA_QS, libname );
			lua_pushvalue( L, -1 );
			lua_setfield( L, -3, libname );  /* _LOADED[libname] = new table */
		}
		lua_remove( L, -2 );  /* remove _LOADED table */
		lua_insert( L, -(nup+1) );  /* move library table to below upvalues */
	}
	for( ; l->name; l++ )
	{
		int i;
		for( i=0; i<nup; i++ )  /* copy upvalues to the top */
			lua_pushvalue( L, -nup );
		lua_pushcclosure( L, l->func, nup );
		lua_setfield( L, -(nup+2), l->name );
	}
	lua_pop( L, nup );  /* remove upvalues */
}
#endif

#if !AAA_LIB_LUA_STATIC()
LUALIB_API void luaL_where( lua_State* L, int level )
{
	lua_Debug ar;
	if (lua_getstack(L, level, &ar))
	{  /* check function at level */
		lua_getinfo(L, "Sl", &ar);  /* get info about it */
		if (ar.currentline > 0)
		{  /* is there info? */
			lua_pushfstring(L, "%s:%d: ", ar.short_src, ar.currentline);
			return;
		}
	}
	lua_pushliteral(L, "");  /* else, no information available... */
}

LUALIB_API int luaL_error( lua_State* L, const char* fmt, ... )
{
	va_list argp;
	va_start(argp, fmt);
	luaL_where(L, 1);
	lua_pushvfstring(L, fmt, argp );
	va_end(argp);
	lua_concat(L, 2);
	return lua_error(L);
}

LUALIB_API const char* luaL_findtable( lua_State* L, int idx, const char* fname, int szhint )
{
	const char *e;
	lua_pushvalue(L, idx);
	do
	{
		e = strchr(fname, '.');
		if( !e )
			e = fname + strlen(fname);
		lua_pushlstring(L, fname, e - fname);
		lua_rawget(L, -2);
		if (lua_isnil(L, -1))
		{  /* no such field? */
			lua_pop(L, 1);  /* remove this nil */
			lua_createtable(L, 0, (*e == '.' ? 1 : szhint)); /* new table for field */
			lua_pushlstring(L, fname, e - fname);
			lua_pushvalue(L, -2);
			lua_settable(L, -4);  /* set new table into field */
		}
		else if (!lua_istable(L, -1))
		{  /* field has a non-table value? */
			lua_pop(L, 2);  /* remove table and value */
			return fname;  /* return problematic part of the name */
		}
		lua_remove(L, -2);  /* remove previous table */
		fname = e + 1;
	}
	while (*e == '.');
	return nullptr;
}

LUALIB_API int luaL_argerror( lua_State* L, int narg, const char* extramsg )
{
	lua_Debug ar;
	if(	!lua_getstack(L, 0, &ar) )  /* no stack frame? */
		return luaL_error( L, "bad argument #%d (%s)", narg, extramsg );
	lua_getinfo( L, "n", &ar );
	if( strcmp( ar.namewhat, "method" ) == 0 )
	{
		--narg;  /* do not count `self' */
		if (narg == 0)  /* error is in the self argument itself? */
			return luaL_error( L, "calling " LUA_QS " on bad self (%s)", ar.name, extramsg );
	}
	if( !ar.name )
		ar.name = "?";
	return luaL_error( L, "bad argument #%d to " LUA_QS " (%s)", narg, ar.name, extramsg );
}

LUALIB_API int luaL_typerror( lua_State* L, int narg, const char* tname )
{
	const char* msg;
	int type = lua_type(L,narg);
	if( type == LUA_TSTRING )
		msg = lua_pushfstring( L, "%s expected, got %s \"%s\"", tname, lua_typename( L, type ), lua_tostring( L, narg ) );
	else
		msg = lua_pushfstring( L, "%s expected, got %s", tname, lua_typename( L, type ) );
	return luaL_argerror( L, narg, msg );
}

LUALIB_API lua_Integer luaL_checkinteger( lua_State *L, int narg )
{
	lua_Integer d = lua_tointeger( L, narg );
	if( d == 0 && !lua_isnumber( L, narg ) )  // avoid extra test when d is not 0
		tag_error( L, narg, LUA_TNUMBER );
	return d;
}
#endif	//AAA_LIB_LUA_STATIC()

namespace aaalua
{
namespace n_exp
{
	void	register_exp( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		l.define_table( "exp" );
//			ADD_FN( new_fp32	);

		l.pop( 1 );	//pop new table
	}
}	//end namespace n_exp
}	//end namespace aaalua
