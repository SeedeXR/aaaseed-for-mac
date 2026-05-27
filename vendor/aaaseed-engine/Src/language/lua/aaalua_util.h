
#ifdef AAA_AAALUA_UTIL_H
#error "AAALUA_UTIL_H included more than once."
#endif
#define AAA_AAALUA_UTIL_H 1


#ifndef __LUA_HPP__
#	include "lua/lua.hpp"
#endif
#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

#define AAA_LIB_LUA_STATIC() 1

class c_obj_ui;

class c_lua_state final	// : public c_obj
{
public:
	static			C_PCHAR_C	get_type_str( INT32 CONST type );
	static			C_PCHAR_C	get_lua_error_header();
	static			o_str*		get_script_filename();
	static			C_PCHAR_C	get_script_debug_name_str();
	static			void		print_script_debug_name();
	static			C_PCHAR_C	get_obj_debug_str( c_obj_ui* CONST obj );
private:
	lua_State*		_L;
	C_PCHAR_C		_fn_name;
public:
	FINLINE	c_lua_state( lua_State* CONST L = nullptr, C_PCHAR_C fn_name = nullptr )
		:_L			{L}
		,_fn_name	{fn_name}
	{
	}

	FINLINE			lua_State*	get_state()		CONST		{ return _L; }
//	FINLINE			void		set_info(					lua_State* L,			CHAR* header );	

					INT32		compile(					o_str& o,				C_PCHAR_C name ) CONST;
					INT32		pcall(						INT32 CONST arg_nb,		INT32 CONST result_nb ) CONST;
					bool		open();
					void		close(); 
//ERROR
					void		error_build_message(		CHAR* CONST dst,		INT32 CONST len, C_PCHAR_C mess	)  CONST;
					void		error_message(				C_PCHAR_C fmt,			... )  CONST;
					void		error_and_escape(			C_PCHAR_C fmt,			... )  CONST;
private:
					void		pop_str_and_message(		C_PCHAR_C header,		bool CONST b_edit = true ) CONST;
public:
					void		pop_str_and_print(			C_PCHAR_C arg1			) CONST;
					void		pop_str_and_print(			C_PCHAR_C arg1,			C_PCHAR_C arg2 ) CONST;

					void		pop_str_and_print_error(	C_PCHAR_C error,		C_PCHAR_C pt1 = nullptr,
																					C_PCHAR_C pt2 = nullptr,
																					C_PCHAR_C pt3 = nullptr ) CONST;
//STACK
	FINLINE			INT32		get_top()		CONST								{ return lua_gettop( _L );			}
	FINLINE			INT32		get_arg_nb()	CONST								{ return lua_gettop( _L );			}
	FINLINE			INT32		get_arg_nb_min(				INT32 CONST min			) CONST;
	FINLINE			INT32		get_arg_nb_max(				INT32 CONST max			) CONST;
	FINLINE			INT32		get_arg_nb_min_max(			INT32 CONST min,		INT32 CONST max ) CONST;
	FINLINE			void		check_arg_nb(				INT32 CONST nb_asked	) CONST;
	FINLINE			void		check_arg_nb_escape_no(		INT32 CONST nb_asked	) CONST;
	FINLINE			INT32		get_arg_nb(					INT32 CONST val_1		) CONST;
	FINLINE			INT32		get_arg_nb(					INT32 CONST val_1,		INT32 CONST val_2	) CONST;
	FINLINE			INT32		get_arg_nb(					INT32 CONST val_1,		INT32 CONST val_2,	INT32 CONST val_3	) CONST;
	FINLINE			INT32		get_arg_nb(					INT32 CONST val_1,		INT32 CONST val_2,	INT32 CONST val_3,	INT32 CONST val_4	) CONST;
	template < INT32 CONST modulo >
	FINLINE			INT32		get_arg_nb_multiple() CONST;

	FINLINE			void		pop(						INT32 CONST nb ) CONST	{	lua_pop( _L, nb );					}
	FINLINE			void		pop_all() CONST;

	FINLINE			INT32		get_type(					INT32 CONST index ) CONST		{ return lua_type( _L, index );		}
	FINLINE			C_PCHAR_C	get_str_from_type(			INT32 CONST type )  CONST		{ return lua_typename( _L, type );	}

	FINLINE	static	bool		is_type_none(				INT32 CONST type )				{ return type == LUA_TNONE;			}
	FINLINE	static	bool		is_type_nil(				INT32 CONST type )				{ return type == LUA_TNIL;			}
	FINLINE static	bool		is_type_bool(				INT32 CONST type )				{ return type == LUA_TBOOLEAN;		}
	FINLINE static	bool		is_type_light_user_data(	INT32 CONST type )				{ return type == LUA_TLIGHTUSERDATA;	}
	FINLINE static	bool		is_type_number(				INT32 CONST type )				{ return type == LUA_TNUMBER;			}
	FINLINE static	bool		is_type_string(				INT32 CONST type )				{ return type == LUA_TSTRING;			}
	FINLINE static	bool		is_type_table(				INT32 CONST type )				{ return type == LUA_TTABLE;			}
	FINLINE static	bool		is_type_function(			INT32 CONST type )				{ return type == LUA_TFUNCTION;		}
	FINLINE static	bool		is_type_user_data(			INT32 CONST type )				{ return type == LUA_TUSERDATA;		}
	FINLINE static	bool		is_type_thread(				INT32 CONST type )				{ return type == LUA_TTHREAD;			}

	FINLINE			bool		is_none_or_nil(				INT32 CONST index=-1 )	CONST	{ return lua_isnoneornil( _L, index );				}
	FINLINE			bool		is_nil(						INT32 CONST index=-1 )	CONST	{ return is_type_nil(				get_type(index) );	}
	FINLINE			bool		is_bool(					INT32 CONST index=-1 )	CONST	{ return is_type_bool(			get_type(index) );	}
	FINLINE			bool		is_light_user_data(			INT32 CONST index=-1 )	CONST	{ return is_type_light_user_data(	get_type(index) );	}
	FINLINE			bool		is_number(					INT32 CONST index=-1 )	CONST	{ return is_type_number(			get_type(index) );	}
	FINLINE			bool		is_string(					INT32 CONST index=-1 )	CONST	{ return is_type_string(			get_type(index) );	}
	FINLINE			bool		is_table(					INT32 CONST index=-1 )	CONST	{ return is_type_table(			get_type(index) );	}
	FINLINE			bool		is_function(				INT32 CONST index=-1 )	CONST	{ return is_type_function(		get_type(index) );	}
	FINLINE			bool		is_user_data(				INT32 CONST index=-1 )	CONST	{ return is_type_user_data(		get_type(index) );	}
	FINLINE			bool		is_thread(					INT32 CONST index=-1 )	CONST	{ return is_type_thread(			get_type(index) );	}

	// GET_VALUE_DIRECT()
	//
	template < typename T >
	FINLINE			T			get_value_direct_int(		INT32 CONST index=-1 )	CONST	{ return T(		lua_tointeger(  _L, index ));	}
	template < typename T >
	FINLINE			T			get_value_direct(			INT32 CONST index=-1 )	CONST	{ return T(		lua_tonumber(  _L, index ));	}
	template <>
	FINLINE			INT32		get_value_direct<INT32>(	INT32 CONST index )		CONST	{ return get_value_direct_int<INT32> ( index );	}
	template <>
	FINLINE			UINT32		get_value_direct<UINT32>(	INT32 CONST index )		CONST	{ return get_value_direct_int<UINT32>( index );	}
	template <>
	FINLINE			INT64		get_value_direct<INT64>(	INT32 CONST index )		CONST	{ return get_value_direct_int<INT64> ( index );	}
	// valid until popped
	template <>
	FINLINE			C_PCHAR_C	get_value_direct<C_PCHAR_C>( INT32 CONST index )	CONST	{ return lua_tostring( _L, index );	}
	template <>
	FINLINE			bool		get_value_direct<bool>(		INT32 CONST index )		CONST
	{
		if( is_bool(index) )
			return lua_toboolean( _L, index ) == 1;
		return lua_tonumber( _L, index ) != 0.;
	}

	// GET_VALUE()
	//
	template < typename T >
	FINLINE			T			get_value_int( INT32 CONST index ) CONST
	{
		T val = get_value_direct_int<T>( index );
		if( val != T(0) )
			return val;
		if( !is_number(index) )
			error_and_escape( "at position %d no number but a %s", index, get_type_str( get_type(index) ) );
		return T(0);
	}
	template < typename T >
	FINLINE			T			get_value_str( INT32 CONST index ) CONST		//	str is valid only till value popped
	{
		if( is_nil(index) )
		{
			error_and_escape( "at position %d nil where string needed", index );
			return nullptr;	//todo check but should be removed
		}
		return get_value_direct<T>( index );
	}
	template < typename T >
	FINLINE			T			get_value( INT32 CONST index=-1 ) CONST
	{
		switch( get_type(index) )
		{
		//error( "no real but a nil");	//todo have a more selective fn
		case LUA_TBOOLEAN:	return lua_toboolean(_L,index) ? T(1.) : T(0.);		break;
		case LUA_TNIL:		return T(0.);										break;
		}
		return get_value_direct<T>( index );
	}
	template < typename T >
	FINLINE			T			get_value_or_1( INT32 CONST index=-1 ) CONST
	{
		switch( get_type(index) )
		{
		//error( "no real but a nil");	//todo have a more selective fn
		case LUA_TBOOLEAN:	return lua_toboolean(_L,index) ? T(1.) : T(0.);		break;
		case LUA_TNIL:		return T(1.);										break;
		}
		return get_value_direct<T>( index );
	}
	template <>
	FINLINE			INT32		get_value( INT32 CONST index ) CONST { return get_value_int<INT32>		( index ); }
	template <>
	FINLINE			UINT32		get_value( INT32 CONST index ) CONST { return get_value_int<UINT32>		( index ); }
	template <>
	FINLINE			INT64		get_value( INT32 CONST index ) CONST { return get_value_int<INT64>		( index ); }
	template <>
	FINLINE			C_PCHAR_C	get_value( INT32 CONST index ) CONST { return get_value_str<C_PCHAR_C>	( index ); }
	template <>
	FINLINE			bool		get_value( INT32 CONST index ) CONST { return get_value_direct<bool>	( index ); }

	// GET_MULTIPLE TEMPLATES
	//
	template < INT32 NB, typename T >
	FINLINE	void get_vn_core( T* CONST dst, INT32 CONST index ) CONST
	{
		*dst = get_value_direct<T>(index);
		if constexpr (NB > 1)
			get_vn_core< NB-1 >( dst+1, index+1 );
	}
	template < INT32 NB, typename T >
	FINLINE	void get_vn( T* CONST dst, INT32 index ) CONST
	{
		get_vn_core< NB >( dst, index );
	}
	template < typename T >
	void get_vn( T* CONST dst, INT32 index, INT32 nb ) CONST
	{
		for( INT32 i = 0; i < nb; ++i )
			dst[i] = get_value_direct<T>( index+i );
	}

	template < typename T > FINLINE	void get_v2	( T* CONST dst, INT32 CONST index ) CONST { get_vn<  2 >( dst, index ); }
	template < typename T > FINLINE	void get_v3	( T* CONST dst, INT32 CONST index ) CONST { get_vn<  3 >( dst, index ); }
	template < typename T > FINLINE	void get_v4	( T* CONST dst, INT32 CONST index ) CONST { get_vn<  4 >( dst, index ); }
	template < typename T > FINLINE	void get_v5	( T* CONST dst, INT32 CONST index ) CONST { get_vn<  5 >( dst, index ); }
	template < typename T > FINLINE	void get_v6	( T* CONST dst, INT32 CONST index ) CONST { get_vn<  6 >( dst, index ); }
	template < typename T >	FINLINE	void get_v7	( T* CONST dst, INT32 CONST index ) CONST { get_vn<  7 >( dst, index ); }
	template < typename T >	FINLINE	void get_v8	( T* CONST dst, INT32 CONST index ) CONST { get_vn<  8 >( dst, index ); }
	template < typename T >			void get_v9	( T* CONST dst, INT32 CONST index ) CONST { get_vn<  9 >( dst, index ); }
	template < typename T >         void get_v10( T* CONST dst, INT32 CONST index ) CONST { get_vn< 10 >( dst, index ); }
	template < typename T >         void get_v11( T* CONST dst, INT32 CONST index ) CONST { get_vn< 11 >( dst, index ); }
	template < typename T >         void get_v12( T* CONST dst, INT32 CONST index ) CONST { get_vn< 12 >( dst, index ); }
	template < typename T >         void get_v13( T* CONST dst, INT32 CONST index ) CONST { get_vn< 13 >( dst, index ); }
	template < typename T >         void get_v14( T* CONST dst, INT32 CONST index ) CONST { get_vn< 14 >( dst, index ); }
	template < typename T >         void get_v15( T* CONST dst, INT32 CONST index ) CONST { get_vn< 15 >( dst, index ); }
	template < typename T >         void get_v16( T* CONST dst, INT32 CONST index ) CONST { get_vn< 16 >( dst, index ); }

	FINLINE			FP32		get_fp32(		INT32 CONST index=-1 ) CONST	{ return get_value<FP32>(index); }
	FINLINE			REAL		get_real(		INT32 CONST index=-1 ) CONST	{ return get_value<REAL>(index); }
	FINLINE			FP32		get_fp32_or_1(	INT32 CONST index=-1 ) CONST	{ return get_value_or_1<FP32>(index); }

	//	these fn do it from a table	FINLINE			REAL		get_real(					INT32 CONST index=-1	) CONST	{	return get_value<REAL>(index); }
	template<typename T> FINLINE void	get_v2_table(			T* dst,	INT32 index ) CONST;
	template<typename T> FINLINE void	get_v3_table(			T* dst,	INT32 index ) CONST;
	template<typename T> FINLINE void	get_v4_table(			T* dst,	INT32 index ) CONST;
	template<typename T> FINLINE void	get_v5_table(			T* dst,	INT32 index ) CONST;
	template<typename T>		 void	get_vn_table(			T* dst,	INT32 CONST index,	INT32 nb,	INT32 CONST first = 0 ) CONST;
	template<typename T>		 void	get_v3n_table_repeat_3(	T* dst,	INT32 CONST index,	INT32 nb	) CONST;

	FINLINE			DOUBLE		get_double(					INT32 CONST index		) CONST	{ return get_value<DOUBLE>(index); }

	FINLINE			INT32		get_int32(					INT32 CONST index		) CONST	{ return get_value<INT32>(index); }
	FINLINE			void		get_int32_v2(				INT32* dst,			INT32 index=-2 ) CONST { get_v2<INT32>( dst, index ); }
	FINLINE			void		get_int32_v3(				INT32* dst,			INT32 index=-3 ) CONST { get_v3<INT32>( dst, index ); }

	FINLINE			UINT32		get_uint32(					INT32 CONST index		) CONST	{ return get_value<UINT32>	(index); }

	FINLINE			INT64		get_int64(					INT32 CONST index		) CONST	{ return get_value<INT64>		(index); }

	FINLINE			bool		get_bool(					INT32 CONST index=-1	) CONST	{ return get_value<bool>(index); }

	FINLINE			C_PCHAR_C	get_str(					INT32 CONST index=-1	) CONST	{ return get_value<C_PCHAR_C>	(index); }
	FINLINE			C_PCHAR_C	get_str_or_nil(				INT32 CONST index		) CONST;
	FINLINE	CONST	void*		get_userdata(				INT32 CONST index		) CONST;

	FINLINE			p_param		get_param(					INT32 CONST index=-1	) CONST;
	FINLINE			c_obj_ui*	get_obj_from_ref(			UINT32 CONST  ref		) CONST;
	FINLINE			c_obj_ui*	get_aaa_obj(				INT32 CONST index		) CONST;
	FINLINE			c_obj_ui*	get_aaa_obj_or_nil(			INT32 CONST index		) CONST;

	template< class T >
	FINLINE			T*			get_aaa_obj_of_class(		INT32 CONST index		) CONST;
	FINLINE			c_obj_ui*	get_aaa_obj_of_class(		INT32 CONST index,		C_PCHAR_C class_name ) CONST;
	FINLINE			c_obj_ui*	get_aaa_obj_derived_from(	INT32 CONST index,		C_PCHAR_C class_name ) CONST;

	FINLINE			p_param		get_aaa_param_from_name(	INT32 CONST index		) CONST;
	FINLINE			p_param		get_aaa_param_from_stack(	INT32 CONST nb			) CONST;

	template< class T >
	FINLINE			T			pop_value() CONST
	{
		T t = get_value<T>();
		pop( 1 );
		return t;
	}
	FINLINE			FP32		pop_fp32()		CONST	{	return pop_value<FP32>  (); }
	FINLINE			REAL		pop_real()		CONST	{	return pop_value<REAL>  (); }
	FINLINE			INT32		pop_int32()		CONST	{	return pop_value<INT32> (); }
	FINLINE			UINT32		pop_uint32()	CONST	{	return pop_value<UINT32>(); }
	FINLINE			INT64		pop_int64()		CONST	{	return pop_value<INT64> (); }
//	FINLINE			C_PCHAR		pop_str()		CONST;
	FINLINE			c_obj_ui*	pop_aaa_obj()	CONST;

	FINLINE			void		push_nil()														CONST	{ lua_pushnil( _L );				}
	FINLINE			void		push_bool(					bool CONST b )						CONST	{ lua_pushboolean( _L, b ? 1 : 0 );	}
	template < typename T >
	FINLINE			void		push_int(					T CONST i )							CONST	{ lua_pushinteger( _L, i );		}
	template < typename T1, typename T2 >
	FINLINE			void		push_int_2(					T1 CONST i1, T2 CONST i2 )			CONST	{ push_int( i1 );				push_int( i2 );		}

	FINLINE			void		push_double(				DOUBLE CONST d )					CONST	{ lua_pushnumber( _L, d );		}
	FINLINE			void		push_double_2(				DOUBLE CONST d1, DOUBLE CONST d2 )	CONST	{ push_double(d1);				push_double(d2);	}

	FINLINE			void		push_real(					REAL CONST r )						CONST	{ lua_pushnumber( _L, r );		}
	FINLINE			void		push_real_2(				REAL CONST r1, REAL CONST r2 )		CONST	{ push_real( r1 );				push_real( r2 );	}
	FINLINE			void		push_real_v2(				REAL CONST * CONST v )				CONST	{ push_real_2( v[0], v[1] );		}
	FINLINE			void		push_real_v3(				REAL CONST * CONST v )				CONST	{ push_real_v2(v);				push_real(v[2]); 	}
	FINLINE			void		push_real_v4(				REAL CONST * CONST v )				CONST	{ push_real_v3(v);				push_real(v[3]); 	}
	FINLINE			void		push_real_v5(				REAL CONST * CONST v )				CONST	{ push_real_v4(v);				push_real(v[4]); 	}
	FINLINE			void		push_real_v6(				REAL CONST * CONST v )				CONST	{ push_real_v5(v);				push_real(v[5]); 	}

	FINLINE			void		push_fp32(					FP32 CONST r )						CONST	{ lua_pushnumber( _L, r );		}
	FINLINE			void		push_fp32_v2(				FP32 CONST * CONST v )				CONST	{ push_fp32(v[0]);				push_fp32(v[1]);	}
	FINLINE			void		push_fp32_v3(				FP32 CONST * CONST v )				CONST	{ push_fp32_v2(v);				push_fp32(v[2]);	}
	FINLINE			void		push_fp32_v4(				FP32 CONST * CONST v )				CONST	{ push_fp32_v3(v);				push_fp32(v[3]);	}
	FINLINE			void		push_fp32_v5(				FP32 CONST * CONST v )				CONST	{ push_fp32_v4(v);				push_fp32(v[4]);	}
	FINLINE			void		push_fp32_v6(				FP32 CONST * CONST v )				CONST	{ push_fp32_v5(v);				push_fp32(v[5]);	}

	FINLINE			void		push_string(				C_PCHAR_C str )						CONST	{ lua_pushstring(  _L, str );						}
	FINLINE			void		push_string(				C_PCHAR_C str, INT32 CONST len ) 	CONST	{ lua_pushlstring( _L, str, len );					}
	FINLINE			void		push_string(				o_str CONST & o )					CONST	{ lua_pushlstring( _L, o.get(), o.get_len() );		}
	template< class T >
	FINLINE			void		push_obj_ref(				T* CONST obj )						CONST;	  
																										  
	FINLINE			void		remove(						INT32 CONST index )					CONST	{ lua_remove( _L, index );		}

	FINLINE			void		new_table()										CONST;
	FINLINE			void		define_table_global(		C_PCHAR_C name )	CONST;
	FINLINE			void		define_table_global_get(	C_PCHAR_C name )	CONST;
	FINLINE			void		define_table(				C_PCHAR_C name )	CONST;
	FINLINE			void		get_table_always(			C_PCHAR_C name )	CONST;

	//assume table id at the top of the stack
	//	assume that table is at the top
	template< typename INT_TYPE >
	FINLINE			void		set_field( INT32 CONST key, INT_TYPE CONST value )	CONST
	{
		push_int( key );
		push_int<INT_TYPE>( value );
		set_table( -3 );
	}
	template< typename INT_TYPE >
	FINLINE			void		set_field( C_PCHAR_C key, INT_TYPE CONST value )	CONST
	{
		push_string( key );
		push_int<INT_TYPE>( value );
		set_table( -3 );
	}

	FINLINE			void		set_field(					INT32 CONST key,	FP32 CONST value )	CONST;
	FINLINE			void		set_field(					INT32 CONST key,	DOUBLE CONST value )CONST;
	FINLINE			void		set_field(					C_PCHAR_C key,		DOUBLE CONST value )CONST;
	FINLINE			void		set_field(					C_PCHAR_C key,		REAL CONST value )	CONST;
	FINLINE			void		set_field(					INT32 key,			PCHAR value )		CONST;
	FINLINE			void		set_field(					C_PCHAR_C key,		PCHAR value )		CONST;
	FINLINE			void		set_field(					INT32 key,			C_PCHAR value )		CONST;
	FINLINE			void		set_field(					C_PCHAR_C key,		C_PCHAR value )		CONST;
	FINLINE			void		set_field_char(				INT32 CONST key,	CHAR CONST value )	CONST;
	FINLINE			void		set_field_nil(				INT32 CONST key		)					CONST;
	FINLINE			void		set_field_bool(				INT32 CONST key,	bool CONST b	)	CONST;

	FINLINE			void		get_table_field(			INT32 CONST index	) CONST						{ lua_gettable(	 _L, index );				}
	FINLINE			void		get_table_field_raw(		INT32 CONST index,	INT32 CONST key ) CONST		{ lua_rawgeti(	 _L, index, key );			}
	FINLINE			void		get_table_field_raw(		INT32 CONST index,	C_PCHAR_C key ) CONST		{ push_string(key);
																											  lua_rawget(	 _L, -2 );
																											}
	FINLINE			void		get_field(					INT32 CONST index,	C_PCHAR_C key ) CONST		{ lua_getfield(  _L, index, key );			}
	FINLINE			void		set_global(					C_PCHAR_C name		) CONST						{ lua_setglobal( _L, name );				}
	FINLINE			void		get_global(					C_PCHAR_C name		) CONST						{ lua_getglobal( _L, name );				}
	FINLINE			void		set_table(					INT32 CONST index	) CONST						{ lua_settable(	 _L, index );				}

	FINLINE			void		set_field_consecutive_v2(	INT32 CONST index, FP32 CONST x, FP32 CONST y )									CONST;
	FINLINE			void		set_field_consecutive_v2(	INT32 CONST index, FP32 CONST * CONST v )										CONST;
	FINLINE			void		set_field_consecutive_v3(	INT32 CONST index, FP32 CONST x, FP32 CONST y, FP32 CONST z )					CONST;
	FINLINE			void		set_field_consecutive_v3(	INT32 CONST index, FP32 CONST * CONST v )										CONST;
	FINLINE			void		set_field_consecutive_v3n(	INT32       index, FP32 CONST *       v, INT32 CONST nb,	INT32 stride = 0 )	CONST;
	FINLINE			void		set_field_consecutive_v4(	INT32 CONST index, FP32 CONST * CONST v )										CONST;
	FINLINE			void		set_field_consecutive_vn(	INT32       index, FP32 CONST *       v,	INT32 CONST nb )					CONST;

	FINLINE			INT32		ref()														CONST	{ return luaL_ref(	_L, LUA_REGISTRYINDEX);					}
//	FINLINE			bool		is_ref( INT32 ref )	LUA_NOREF
	FINLINE			void		push_ref( INT32 lua_ref )									CONST	{ lua_rawgeti( _L, LUA_REGISTRYINDEX, lua_ref );			}	
	FINLINE			void		unref( INT32 lua_ref )										CONST	{ return luaL_unref(	_L, LUA_REGISTRYINDEX, lua_ref );	}
	

	//	swap the 2 elt at the top of the stack
	FINLINE			void		swap()														CONST	{ lua_insert(		_L, -2 );				}



	CONSTEXPR		INT32		return_nothing()											CONST	{ return 0;	}
	FINLINE			INT32		return_nil()												CONST	{ push_nil();					return 1;	}
	template < typename T >
	FINLINE			INT32		return_int(				T CONST i			)				CONST	{ push_int( i );				return 1;	}
	template < typename T1, typename T2 >
	FINLINE			INT32		return_int_2(			T1 CONST  i1,		T2 CONST i2 )	CONST	{ push_int_2( i1, i2 );			return 2;	}
	FINLINE			INT32		return_int32(			INT32 CONST i		)				CONST	{ push_int( i );				return 1;	}
	FINLINE			INT32		return_uint32(			UINT32 CONST i		)				CONST	{ push_int( i );				return 1;	}
	FINLINE			INT32		return_int64(			INT64 CONST i		)				CONST	{ push_int( i );				return 1;	}
	FINLINE			INT32		return_bool(			bool CONST b		)				CONST	{ push_bool( b );				return 1;	}
	FINLINE			INT32		return_false()												CONST	{ lua_pushboolean( _L, 0 );		return 1;	}	
	FINLINE			INT32		return_true()												CONST	{ lua_pushboolean( _L, 1 );		return 1;	}
	FINLINE			INT32		return_double(			DOUBLE CONST r		)				CONST	{ push_double( r );				return 1;	}
	FINLINE			INT32		return_real(			REAL CONST r		)				CONST	{ push_real( r );				return 1;	}
	FINLINE			INT32		return_double_2(		DOUBLE CONST r1,	DOUBLE CONST r2 ) CONST	{ push_double_2( r1, r2 );		return 2;	}
	FINLINE			INT32		return_real_2(			REAL CONST r1,		REAL CONST r2 )	 CONST	{ push_real_2( r1, r2 );		return 2;	}
	FINLINE			INT32		return_real_v2(			REAL CONST * CONST pt	)			CONST	{ push_real_v2( pt );			return 2;	}
	FINLINE			INT32		return_real_v3(			REAL CONST * CONST pt	)			CONST	{ push_real_v3( pt );			return 3;	}
	FINLINE			INT32		return_real_v4(			REAL CONST * CONST pt	)			CONST	{ push_real_v4( pt );			return 4;	}
	FINLINE			INT32		return_real_v5(			REAL CONST * CONST pt	)			CONST	{ push_real_v5( pt );			return 5;	}
	FINLINE			INT32		return_real_v6(			REAL CONST * CONST pt	)			CONST	{ push_real_v6( pt );			return 6;	}
	FINLINE			INT32		return_fp32(			FP32 CONST r		)				CONST	{ push_fp32( r );				return 1;	}
	FINLINE			INT32		return_fp32_v2(			FP32 CONST * CONST pt	)			CONST	{ push_fp32_v2( pt );			return 2;	}
	FINLINE			INT32		return_fp32_v3(			FP32 CONST * CONST pt	)			CONST	{ push_fp32_v3( pt );			return 3;	}
	FINLINE			INT32		return_fp32_v4(			FP32 CONST * CONST pt	)			CONST	{ push_fp32_v4( pt );			return 4;	}
	FINLINE			INT32		return_fp32_v5(			FP32 CONST * CONST pt	)			CONST	{ push_fp32_v5( pt );			return 5;	}
	FINLINE			INT32		return_fp32_v6(			FP32 CONST * CONST pt	)			CONST	{ push_fp32_v6( pt );			return 6;	}
	FINLINE			INT32		return_string(			C_PCHAR_C str			)			CONST;
	FINLINE			INT32		return_string(			o_str CONST & o			)			CONST;
	FINLINE			INT32		return_string_or_nil(	C_PCHAR_C str			)			CONST;	// if str is empty return nil
	FINLINE			INT32		return_string_or_nil(	o_str CONST & o			)			CONST;	// if str is empty return nil
	FINLINE			INT32		return_table()												CONST	{ return 1;	}
	FINLINE			INT32		return_userdatum()											CONST	{ return 1;	}
	

	template< class T >
	FINLINE			INT32		return_obj_ref( T* CONST obj ) CONST;
	template< class T >
	FINLINE			INT32		return_obj_ref( T* CONST obj, C_PCHAR_C str ) CONST;

					void		print_stack(			C_PCHAR_C mess = nullptr ) CONST;
					void		print_stack_non_empty(	C_PCHAR_C mess = nullptr ) CONST;
					bool		check_stack(			C_PCHAR_C mess = nullptr ) CONST;

					void		enforce_stack(			INT32 CONST top=0,  C_PCHAR_C fn_name = nullptr, C_PCHAR_C mess = nullptr ) CONST;

#if	AAA_DEBUG()
	#define		STACK_ENFORCE_EMPTY( l, mess )		(l)->enforce_stack( 0, __FUNCTION__, mess );
	#define		STACK_ENFORCE( l, t, mess )			(l)->enforce_stack( t, __FUNCTION__, mess );
	#define		STACK_ENFORCE_STORE( l )			INT32 CONST __STACK_ENFORCE_TOP_PRIVATE__ = (l)->get_top();
	#define		STACK_ENFORCE_SAME( l )				(l)->enforce_stack( __STACK_ENFORCE_TOP_PRIVATE__, __FUNCTION__ );
	#define		STACK_ENFORCE_EMPTY_END( l )		STACK_ENFORCE_EMPTY( l, "end" );
	#define		STACK_ENFORCE_EMPTY_BEGIN( l )		STACK_ENFORCE_EMPTY( l, "begin" );
#else
	#define		STACK_ENFORCE_EMPTY( l, mess )
	#define		STACK_ENFORCE( l, t, mess )
	#define		STACK_ENFORCE_STORE( l )
	#define		STACK_ENFORCE_SAME( l )
	#define		STACK_ENFORCE_EMPTY_END( l )
	#define		STACK_ENFORCE_EMPTY_BEGIN( l )
#endif
					void		add_fn_to_table(		C_PCHAR_C name, lua_CFunction CONST fn, C_PCHAR_C str_help = nullptr ) CONST;
};

namespace aaalua
{
	extern int new_metatable( lua_State* L, C_PCHAR_C tname );
}


// ARGUMENTS
//
FINLINE	INT32	c_lua_state::get_arg_nb_min( INT32 CONST min ) CONST
{
	INT32 n = get_arg_nb();
	if( n < min )
		error_and_escape( "not enough arguments : %d instead of %d at least.", n, min );
	return n;
}
FINLINE	INT32	c_lua_state::get_arg_nb_max( INT32 CONST max ) CONST
{
	INT32 n = get_arg_nb( max );
	if( n > max )
		error_and_escape( "too many arguments : %d instead of %d at most.", n, max );
	return n;
}
FINLINE	INT32	c_lua_state::get_arg_nb_min_max( INT32 CONST min, INT32 CONST max ) CONST
{
	INT32 n = get_arg_nb_min( min );
	if( n > max )
		error_and_escape( "too many arguments : %d instead of %d.", n, max );
	return n;
}
FINLINE	void	c_lua_state::check_arg_nb( INT32 CONST nb_asked ) CONST
{
	INT32 n = get_arg_nb();
	if( n != nb_asked )
		error_and_escape( "not the right arguments number : %d instead of %d.", n, nb_asked );
}
FINLINE	void	c_lua_state::check_arg_nb_escape_no( INT32 CONST nb_asked ) CONST
{
	INT32 n = get_arg_nb();
	if( n != nb_asked )
		error_message( "not the right arguments number : %d instead of %d.", n, nb_asked );
}
FINLINE	INT32	c_lua_state::get_arg_nb( INT32 CONST val_1 ) CONST
{
	INT32 n = get_arg_nb();
	if( n != val_1 )
		error_and_escape( "%d arguments instead of %d.", n, val_1 );
	return n;
}
FINLINE	INT32	c_lua_state::get_arg_nb( INT32 CONST val_1, INT32 CONST val_2 ) CONST
{
	INT32 n = get_arg_nb();
	if( n != val_1 && n != val_2 )
		error_and_escape( "%d arguments instead of %d or %d.", n, val_1, val_2 );
	return n;
}
FINLINE	INT32	c_lua_state::get_arg_nb( INT32 CONST val_1, INT32 CONST val_2, INT32 CONST val_3 ) CONST
{
	INT32 n = get_arg_nb();
	if( n != val_1 && n != val_2 && n != val_3 )
		error_and_escape( "%d arguments instead of %d or %d or %d.", n, val_1, val_2, val_3 );
	return n;
}
FINLINE	INT32	c_lua_state::get_arg_nb( INT32 CONST val_1, INT32 CONST val_2, INT32 CONST val_3, INT32 CONST val_4 ) CONST
{
	INT32 n = get_arg_nb();
	if( n != val_1 && n != val_2 && n != val_3 && n != val_4 )
		error_and_escape( "%d arguments instead of %d or %d or %d or %d.", n, val_1, val_2, val_3, val_4 );
	return n;
}
template < INT32 CONST modulo >
FINLINE	INT32	c_lua_state::get_arg_nb_multiple() CONST
{
	INT32 n = get_arg_nb();
	if( !n || (n % modulo) != 0 )
		error_and_escape( "%d arguments not a multiple of %d.", n, modulo );
	return n;
}

// POP
//
FINLINE void	c_lua_state::pop_all() CONST
{
	INT32	nb = get_top();
	if( nb )
		pop( nb );
}

// GET
//
FINLINE	C_PCHAR_C	c_lua_state::get_str_or_nil( INT32 CONST index ) CONST		//	str is valid only till value popped
{
	return is_nil(index) ? nullptr : lua_tostring( _L, index );
}

FINLINE	CONST	void*	c_lua_state::get_userdata( INT32 CONST index ) CONST
{
	if( is_nil(index) )
	{
		error_and_escape( "at position %d nil where UserData needed", index );
		return nullptr;	//todo check but should be removed
	}
	return lua_touserdata( _L, index );
}


FINLINE	c_obj_ui*	c_lua_state::get_obj_from_ref( UINT32 CONST ref ) CONST
{
	c_obj_ui* obj = c_obj_ui::get_obj_from_ref( ref );
	if( !obj )
		error_and_escape( "ref 0x%x -> %.128s", ref, c_obj_ui::get_ref_error_str() );
	return obj;
}

FINLINE	p_param		c_lua_state::get_param( INT32 CONST index ) CONST
{
	UINT32 ref = get_value_direct<UINT32>(index);
	p_param param = c_obj_ui::get_param_from_ref( ref );
	if( !param )
		error_and_escape( "ref 0x%x -> %.128s", ref, c_obj_ui::get_ref_error_str() );
	return param;
}


FINLINE	c_obj_ui*	c_lua_state::get_aaa_obj( INT32 CONST index ) CONST
{
	INT32		t = get_type( index );
	c_obj_ui*	obj;

	if( is_type_number(t) )
	{
		obj = get_obj_from_ref( get_value_direct<UINT32>( index ) );
	}
	else if( is_type_string(t) )
	{
		C_PCHAR_C	name = get_str( index );
		if( *name==0 )
			error_and_escape( "at position %d can't find object with an empty string.", index );
		obj = c_obj_ui::find_from_top_by_name( name );
		if( !obj )
			error_and_escape( "at position %d can't find object with ref or name \"%.128s\".", index, name );
	}
	else if( is_type_nil(t) )
	{
		obj = nullptr;
		error_and_escape( "at position %d got a NIL: not a reference number or a name of object", index );
	}	
	else
	{
		obj = nullptr;
		error_and_escape( "at position %d this is a lua_%.32s, not a reference number or a name of object", index, get_type_str(t) );
	}	
	return obj;
}

FINLINE	c_obj_ui*	c_lua_state::get_aaa_obj_or_nil( INT32 CONST index ) CONST
{
	INT32		t = get_type( index );
	c_obj_ui*	obj;

	if( is_type_number(t) )
	{
		obj = get_obj_from_ref( get_value_direct<UINT32>( index ) );
	}
	else if( is_type_string(t)  )
	{
		C_PCHAR_C	name = get_str( index );
		obj = c_obj_ui::find_from_top_by_name( name );
		if( !obj )
		{
			error_and_escape( "at position %d can't find object with ref or name \"%.128s\" .", index, name );
		}
	}
	else
	{
		obj = nullptr;
		//error_and_escape( "this is a lua_%.32s, not a reference number or a name of object", get_type_str(t) );
	}	
	return obj;
}

template< class T >
FINLINE	T*	c_lua_state::get_aaa_obj_of_class( INT32 CONST index ) CONST
{
	c_obj_ui*	obj = get_aaa_obj( index );
	if( obj )
	{
		if( !obj->is_class<T>() )
		{
			error_and_escape( "at position %d object is not from class %.64s but of class %.64s", index, T::the_factory().get_class_name(), obj->get_class_name() );
			//obj = nullptr;
		}
	}
	return static_cast<T*>(obj);
}

FINLINE	c_obj_ui*	c_lua_state::get_aaa_obj_of_class( INT32 CONST index, C_PCHAR_C class_name ) CONST
{
	c_obj_ui*	obj = get_aaa_obj( index );
	if( obj )
	{
		if( !obj->is_class_name( class_name ) )
		{
			error_and_escape( "at position %d object is not from class %.64s but of class %.64s", index, class_name, obj->get_class_name() );
			//obj = nullptr;
		}
	}
	return obj;
}

FINLINE	c_obj_ui*	c_lua_state::get_aaa_obj_derived_from( INT32 CONST index, C_PCHAR_C class_name ) CONST
{
	c_obj_ui*	obj = get_aaa_obj( index );
	if( obj )
	{
		if( !obj->is_inherited_from( class_name ) )
		{
			error_and_escape( "at position %d object of class %.64s is not derived from class %.64s.", index, obj->get_class_name(), class_name );
			//obj = nullptr
		}
	}
	return obj;
}

FINLINE	c_obj_ui*	c_lua_state::pop_aaa_obj() CONST
{
	c_obj_ui* obj = get_aaa_obj( -1 );
	pop( 1 );
	return obj;
}

//	input	(obj_ref or obj_name) param_name
FINLINE	p_param	c_lua_state::get_aaa_param_from_name( INT32 CONST index ) CONST
{
	c_obj_ui* obj = get_aaa_obj( index );
	if( obj )
	{
		C_PCHAR_C	name = get_str( index+1 );
		p_param		param = obj->get_param_by_name( name );
		if( param )
			return param;
		else
			error_and_escape( "at position %d can't find param %.64s in object %.128s", index, name, c_lua_state::get_obj_debug_str(obj) );
	}
	else
	{
		C_PCHAR_C	name	= get_str( index+1 );
		error_and_escape( "\tat position %d can't get object so can't search param %.128s", index, name );
	}
	return nullptr;
}

//todo rewrote from top of the stack
//	input	(obj_ref or obj_name) param_name
FINLINE	p_param	c_lua_state::get_aaa_param_from_stack( INT32 nb ) CONST
{
	if( nb == 2 )
		return get_aaa_param_from_name( 1 );
	else
	{
		if( is_nil( 1 ) )
			error_and_escape( "param ref is nil" );
		return get_param( 1 );
	}
}

// TABLE

FINLINE	void	c_lua_state::new_table() CONST
{
	lua_newtable( _L );			//	creates a table and put it on the stack
}

FINLINE	void	c_lua_state::define_table_global( C_PCHAR_C name ) CONST
{
	new_table();		//	creates a table
	set_global( name );	//	'name' = table
}

FINLINE	void	c_lua_state::define_table_global_get( C_PCHAR_C name ) CONST
{
	new_table();		//	creates a table
	set_global( name );	//	'name' = table
	get_global( name );
}

FINLINE	void	c_lua_state::define_table( C_PCHAR_C name ) CONST
{
	new_table();
	lua_setfield( _L, -2, name );
	lua_getfield( _L, -1, name );
}

FINLINE	void	c_lua_state::get_table_always( C_PCHAR_C name ) CONST
{
	//todo redo when we move to lua 5.2
	lua_getfield( _L, -1, name );
	if( !is_table(-1) )
	{
		pop(1);
		define_table(name);
	}
}


template<typename T>
FINLINE void	c_lua_state::get_v2_table( T* dst, INT32 index ) CONST
{
	get_table_field_raw( index, 1 );
	*dst = get_value_direct<T>();
	if( index<0 )
		--index;	
	get_table_field_raw( index, 2 );
	*++dst = get_value_direct<T>();
	pop(2);
}
#if AAA_REAL_IS_DOUBLE()
template void	c_lua_state::get_v2_table<REAL>( REAL* dst, INT32 index ) CONST;
#endif
template void	c_lua_state::get_v2_table<FP32>( FP32* dst, INT32 index ) CONST;

template<typename T>
FINLINE void	c_lua_state::get_v3_table( T* dst, INT32 index ) CONST
{
	get_table_field_raw( index,			1 );
	*dst = get_value_direct<T>();
	if( index<0 )
	{
		get_table_field_raw( --index,	2 );
		*++dst = get_value_direct<T>();
		--index;
	}
	else
	{
		get_table_field_raw( index,		2 );
		*++dst = get_value_direct<T>();
	}
	get_table_field_raw( index,			3 );
	*++dst = get_value_direct<T>();
	pop(3);
}
#if AAA_REAL_IS_DOUBLE()
template void	c_lua_state::get_v3_table<REAL>( REAL* dst, INT32 index ) CONST;
#endif
template void	c_lua_state::get_v3_table<FP32>( FP32* dst, INT32 index ) CONST;

template<typename T>
FINLINE void	c_lua_state::get_v4_table( T* dst, INT32 index ) CONST
{
	get_table_field_raw( index,			1 );
	*dst = get_value_direct<T>();
	if( index<0 )
	{
		get_table_field_raw( --index,	2 );
		*++dst = get_value_direct<T>();
		get_table_field_raw( --index,	3 );
		*++dst = get_value_direct<T>();
		--index;
	}
	else
	{
		get_table_field_raw( index,		2 );
		*++dst = get_value_direct<T>();
		get_table_field_raw( index,		3 );
		*++dst = get_value_direct<T>();
	}
	get_table_field_raw( index,			4 );
	*++dst	= get_value_direct<T>();
	pop(4);
}
#if AAA_REAL_IS_DOUBLE()
template void	c_lua_state::get_v4_table<REAL>( REAL* dst, INT32 index ) CONST;
#endif
template void	c_lua_state::get_v4_table<FP32>( FP32* dst, INT32 index ) CONST;


template<typename T>
FINLINE void	c_lua_state::get_v5_table( T* dst, INT32 index ) CONST
{
	get_table_field_raw( index,			1 );
	*dst = get_value_direct<T>();
	if( index<0 )
	{
		get_table_field_raw( --index,	2 );
		*++dst = get_value_direct<T>();
		get_table_field_raw( --index,	3 );
		*++dst = get_value_direct<T>();
		get_table_field_raw( --index,	4 );
		*++dst = get_value_direct<T>();
		 --index;
	}
	else
	{
		get_table_field_raw( index,		2 );
		*++dst = get_value_direct<T>();
		get_table_field_raw( index,		3 );
		*++dst = get_value_direct<T>();
		get_table_field_raw( index,		4 );
		*++dst = get_value_direct<T>();
	}
	get_table_field_raw( index,			5 );
	*++dst = get_value_direct<T>();
	pop(5);
}
#if AAA_REAL_IS_DOUBLE()
template void	c_lua_state::get_v5_table<REAL>( REAL* dst, INT32 index ) CONST;
#endif
template void	c_lua_state::get_v5_table<FP32>( FP32* dst, INT32 index ) CONST;


//	assume that table is at the top 
FINLINE	void	c_lua_state::set_field( INT32 CONST key, DOUBLE value ) CONST
{
	push_int( key );
	push_double( value );
	set_table( -3 );
}

//	assume that table is at the top 
FINLINE	void	c_lua_state::set_field( C_PCHAR_C key, DOUBLE value ) CONST
{
	push_string( key );
	push_double( value );
	set_table( -3 );
}

//	assume that table is at the top 
FINLINE	void	c_lua_state::set_field( INT32 CONST key, FP32 value ) CONST
{
	push_int( key );
	push_fp32( value );
	set_table( -3 );
}

//	assume that table is at the top 
FINLINE	void	c_lua_state::set_field( C_PCHAR_C key, REAL value ) CONST
{
	push_string( key );
	push_real( value );
	set_table( -3 );
}

//	assume that table is at the top 
FINLINE	void c_lua_state::set_field( INT32 CONST key, PCHAR value ) CONST
{
	push_int( key );
	push_string( value );
	set_table( -3 );
}
//	assume that table is at the top 
FINLINE	void c_lua_state::set_field( C_PCHAR_C key, PCHAR value ) CONST
{
	push_string( key );
	push_string( value );
	set_table( -3 );
}
//	assume that table is at the top 
FINLINE	void c_lua_state::set_field( INT32 CONST key, C_PCHAR value ) CONST
{
	push_int( key );
	push_string( value );
	set_table( -3 );
}
//	assume that table is at the top 
FINLINE	void c_lua_state::set_field( C_PCHAR_C key, C_PCHAR value ) CONST
{
	push_string( key );
	push_string( value );
	set_table( -3 );
}

//	assume that table is at the top 
FINLINE	void	c_lua_state::set_field_char( INT32 CONST key, CHAR CONST value ) CONST
{
	push_int( key );
	push_string( &value, 1 );
	set_table( -3 );
}
//	assume that table is at the top 
FINLINE	void c_lua_state::set_field_nil( INT32 CONST key ) CONST
{
	push_int( key );
	push_nil();
	set_table( -3 );
}
//	assume that table is at the top 
FINLINE	void	c_lua_state::set_field_bool( INT32 key, bool CONST b ) CONST
{
	push_int( key );
	push_bool( b );
	set_table( -3 );
}

//	CONSECUTIVE
FINLINE	 void	c_lua_state::set_field_consecutive_v2( INT32 CONST index, FP32 CONST x, FP32 CONST y ) CONST
{
	lua_pushnumber( _L, x );
	lua_rawseti(	_L, -2, index );
	lua_pushnumber( _L, y );
	lua_rawseti(	_L, -2, index+1 );
}
FINLINE	 void	c_lua_state::set_field_consecutive_v2( INT32 CONST index, FP32 CONST * CONST v ) CONST
{
	lua_pushnumber( _L, v[0] );
	lua_rawseti(	_L, -2, index );
	lua_pushnumber( _L, v[1] );
	lua_rawseti(	_L, -2, index+1 );
}

FINLINE	 void	c_lua_state::set_field_consecutive_v3( INT32 CONST index, FP32 CONST x, FP32 CONST y, FP32 CONST z ) CONST
{
	lua_pushnumber( _L, x );
	lua_rawseti(	_L, -2, index );
	lua_pushnumber( _L, y );
	lua_rawseti(	_L, -2, index+1 );
	lua_pushnumber( _L, z );
	lua_rawseti(	_L, -2, index+2 );
}
FINLINE	 void	c_lua_state::set_field_consecutive_v3( INT32 CONST index, FP32 CONST * CONST v ) CONST
{
	lua_pushnumber( _L, v[0] );
	lua_rawseti(	_L, -2, index );
	lua_pushnumber( _L, v[1] );
	lua_rawseti(	_L, -2, index+1 );
	lua_pushnumber( _L, v[2] );
	lua_rawseti(	_L, -2, index+2 );
}
FINLINE	void	c_lua_state::set_field_consecutive_v3n(	INT32 index, FP32 CONST * v, INT32 CONST nb, INT32 stride ) CONST
{
	stride = (stride == 0) ? 1 : stride - 2;
	INT32 index_end = index + nb*3 - 1;
	for( ; index<=index_end; )
	{
		lua_pushnumber( _L, *v++ );
		lua_rawseti(	_L, -2, index++ );
		lua_pushnumber( _L, *v++ );
		lua_rawseti(	_L, -2, index++ );
		lua_pushnumber( _L, *v );
		lua_rawseti(	_L, -2, index++ );
		v += stride;
	}
}
FINLINE	 void	c_lua_state::set_field_consecutive_v4( INT32 CONST index, FP32 CONST * CONST v ) CONST
{
	lua_pushnumber( _L, v[0] );
	lua_rawseti(	_L, -2, index );
	lua_pushnumber( _L, v[1] );
	lua_rawseti(	_L, -2, index+1 );
	lua_pushnumber( _L, v[2] );
	lua_rawseti(	_L, -2, index+2 );
	lua_pushnumber( _L, v[3] );
	lua_rawseti(	_L, -2, index+3 );
}
FINLINE	 void	c_lua_state::set_field_consecutive_vn( INT32 index, FP32 CONST * v, INT32 CONST nb ) CONST
{
	INT32 index_end = index + nb - 1;
	for( ; index<=index_end; ++index )
	{
		lua_pushnumber( _L, *v++ );	
		lua_rawseti( _L, -2, index );
	}
}


// RETURN
//
FINLINE	INT32	c_lua_state::return_string(			C_PCHAR_C str )		CONST	{	push_string( str );		return 1;	}
FINLINE	INT32	c_lua_state::return_string(			o_str CONST & o )	CONST	{	push_string( o );		return 1;	}

FINLINE	INT32	c_lua_state::return_string_or_nil(	C_PCHAR_C str )	CONST
{
	if( str && *str )
		return return_string( str );
	return return_nil();
}
FINLINE	INT32	c_lua_state::return_string_or_nil(	o_str CONST & o ) CONST
{
	if( !o.is_empty() )
		return return_string( o );
	return return_nil();
}

template< class T >
FINLINE	void	c_lua_state::push_obj_ref( T* CONST obj ) CONST
{
	if( obj )
		push_int( obj->get_obj_ref() );
	else
		push_nil();
}

template< class T >
FINLINE	INT32	c_lua_state::return_obj_ref( T* CONST obj ) CONST
{
	push_obj_ref( obj );
	return 1;
}

template< class T >
FINLINE	INT32	c_lua_state::return_obj_ref( T* CONST obj, C_PCHAR_C str ) CONST
{
	// 2025 Feb Mâa removed : too verbose lua should check that C returned nil
	//if( !obj )
	//	error_message( str );
	return return_obj_ref( obj );
}

	