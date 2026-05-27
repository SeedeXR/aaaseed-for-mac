
#ifdef AAA_AAALUA_GLUE_H
#error "AAALUA_GLUE_H included more than once."
#endif
#define AAA_AAALUA_GLUE_H 1


#ifndef __LUA_HPP__
#	include "lua/lua.hpp"
#endif
#ifndef AAA_AAALUA_UTIL_H
#	include "aaalua_util.h"
#endif

namespace osc
{
	class	ReceivedMessage;
};

namespace	aaalua
{
	extern 	FP32	g_fp32[14];

	extern	void	register_all_fn(	lua_State* L );
	extern	void	unregister_all_fn(	lua_State* L );

	extern	bool	b_call_debug_ui;
	extern	bool	b_call_store_ui;
	extern	bool	b_call_print_ui;

	#define	ADD_FN( name )				l.add_fn_to_table( #name, name )
	#define	ADD_FN_WITH_HELP( name )	l.add_fn_to_table( #name, name, name##_help )

	extern	void	watchdog_net_send(	INT32 link_dst,				C_PCHAR_C str );
	extern	void	net_send(			INT32 link_dst, INT32 ch,	C_PCHAR_C str );
	extern	void	net_process(									C_PCHAR_C str );
	extern	void	osc_process_message( CONST osc::ReceivedMessage& msg );
};

#define AAA_LUA_STORE_CALLS() 0
#if AAA_LUA_STORE_CALLS()
#	define LUAAAA_START( a, b )									\
	c_lua_state	l( (a), (b) );									\
	if( aaalua::b_call_debug_ui )								\
	{															\					                                                                                                                                                                                               			\
		if( aaalua::b_call_store_ui )							\
		{														\
			l.get_global( "ccalls" );							\
				l.get_table_field_raw( -1, __FUNCTION__ );		\
				UINT32 nb_call = l.get_value_direct<UINT32>(-1);\
				l.pop(1);										\
				l.set_field( __FUNCTION__, ++nb_call );			\
			l.pop(1);											\
		}														\
		if( aaalua::b_call_print_ui )							\
		{														\
			DBG_PRINT_STRING( "lua call : %s", __FUNCTION__ );	\
		}														\
	}
#else
#	define LUAAAA_START( a, b )	c_lua_state	l( (a), (b) );	
#endif

#define	AAALUACALL( name )		INT32	name( lua_State* L )

#if AAA_DEBUG()
#	define AAALUA_CALL_FN( name, fn )		\
	AAALUACALL( name )						\
	{										\
		LUAAAA_START( L, __FUNCTION__ );	\
		fn();								\
		return l.return_nothing();			\
	}
#	define AAALUA_CALL_FN_RETURN_INT32( name, fn )	\
	AAALUACALL( name )								\
	{												\
		LUAAAA_START( L, __FUNCTION__ );			\
		return l.return_int32( fn() );				\
	}
#	define AAALUA_CALL_FN_RETURN_REAL( name, fn )	\
	AAALUACALL( name )								\
	{												\
		LUAAAA_START( L, __FUNCTION__ );			\
		return l.return_real( fn() );				\
	}
#else
#	define AAALUA_CALL_FN( name, fn	)	\
	AAALUACALL( name )					\
	{									\
		fn();							\
		return 0;						\
	}
#	define AAALUA_CALL_FN_RETURN_INT32( name, fn )	\
	AAALUACALL( name )								\
	{												\
		lua_pushinteger( L, fn() );					\
		return 1;									\
	}
#	define AAALUA_CALL_FN_RETURN_REAL( name, fn )	\
	AAALUACALL( name )								\
	{												\
		lua_pushnumber( L, fn() );					\
		return 1;									\
	}
#endif

#define AAALUA_CALL_FN_REAL( name, fn )		\
	AAALUACALL( name )						\
	{										\
		LUAAAA_START( L, __FUNCTION__ );	\
		l.check_arg_nb( 1 );				\
		fn( l.get_real( 1 ) );				\
		return l.return_nothing();			\
	}
#define AAALUA_CALL_FN_INT32( name, fn )	\
	AAALUACALL( name )						\
	{										\
		LUAAAA_START( L, __FUNCTION__ );	\
		l.check_arg_nb( 1 );				\
		fn( l.get_int32( 1 ) );				\
		return l.return_nothing();			\
	}
#define AAALUA_CALL_FN_BOOL( name, fn )		\
	AAALUACALL( name )						\
	{										\
		LUAAAA_START( L, __FUNCTION__ );	\
		l.check_arg_nb( 1 );				\
		fn( l.get_bool( 1 ) );				\
		return l.return_nothing();			\
	}

#if AAA_DEBUG()
#	define DBG_CHECK_ARG_NB(nb)	l.check_arg_nb( nb )
#else
#	define DBG_CHECK_ARG_NB(nb) l.check_arg_nb( nb )	// November 2025 Maa think it is more easy for most user to have an error message
#endif

