#ifndef LUASOCKET_H
#define LUASOCKET_H
/*=========================================================================*\
* LuaSocket toolkit
* Networking support for the Lua language
* Diego Nehab
* 9/11/1999
\*=========================================================================*/

/*-------------------------------------------------------------------------* \
* Current socket library version
\*-------------------------------------------------------------------------*/
#define LUASOCKET_VERSION    "LuaSocket 3.1.0"
#define LUASOCKET_COPYRIGHT  "Copyright (C) 1999-2013 Diego Nehab"

/*-------------------------------------------------------------------------*\
* This macro prefixes all exported API functions
\*-------------------------------------------------------------------------*/
//#ifndef LUASOCKET_API
//#ifdef _WIN32
//#define LUASOCKET_API
////#define LUASOCKET_API __declspec(dllexport)
//#else
//#define LUASOCKET_API __attribute__ ((visibility ("default")))
//#endif
//#endif

//#ifndef __LUA_HPP__
//#	include "lua/lua.hpp"
//#endif

#ifdef __cplusplus
extern "C" {
#endif
#include "lua/lua.h"
#include "lua/lauxlib.h"
//#include "lua/lualib.h"
//#include "lua/luajit.h"
//#include "compat.h"
#ifdef __cplusplus
}
#endif

//#include "lua/lua.h"
//#include "lua/lauxlib.h"


/*-------------------------------------------------------------------------*\
* Initializes the library.
\*-------------------------------------------------------------------------*/
//LUASOCKET_API 
extern int luaopen_socket_core( lua_State * L );

#endif /* LUASOCKET_H */
