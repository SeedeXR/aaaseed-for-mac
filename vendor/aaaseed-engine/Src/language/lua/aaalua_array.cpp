#include "aaalua_array.h"
#include "aaalua_exp.h"
#include "aaalua_glue.h"
#include "gol/gol_base.h"

namespace aaalua
{
namespace n_array
{
	//todo eventually move size and capacity to UINT32
	template< typename T >
	struct ud_info
	{
		T*		data;
		INT32	capacity;	
		INT32	size;
		GLenum	gl_type;
	};

	template< typename T >
	FINLINE ud_info<T>* get_ud_info( lua_State* L, int CONST index )
	{
//todo	FP32* parray = (FP32*) luaL_checkudata( L, 1, ARRAY_NAME );
		auto udi = (ud_info<T>*) lua_touserdata( L, index );
		if( udi == nullptr )
			tag_error( L, index, LUA_TUSERDATA );
		return udi;
	}
	template< typename T, GLenum gl_type >
	FINLINE ud_info<T>* get_ud_info_type( lua_State* L, int CONST index )
	{
		auto udi = get_ud_info<T>( L, index );
		if( udi->gl_type != gl_type )
			luaL_argerror( L, 1, lua_pushfstring( L, "array: got array with the wrong kind of data" ) );
		return udi;
	}
	template< typename T, GLenum gl_type >
	FINLINE T* get_data( lua_State* L, int CONST index, INT32& size )
	{
		auto udi = get_ud_info_type< T, gl_type > ( L, index );
		size = udi->size;
		return udi->data;
	}
	template< typename T, GLenum gl_type >
	FINLINE T* get_data( lua_State* L, int CONST index )
	{
		auto udi = get_ud_info_type< T, gl_type > ( L, index );
		return udi->data;
	}
	template< typename T >
	FINLINE T* get_data_as( lua_State* L, int CONST index )
	{
		auto udi = (ud_info<T>*) lua_touserdata( L, index );
		if( udi == nullptr )
			tag_error( L, index, LUA_TUSERDATA );
		return udi->data;
	}
	template< typename T >
	FINLINE void store_data_info( ud_info<T>* udi, T* CONST data, INT32 CONST size )
	{
		udi->data = data;
		if( !data )
		{
			udi->capacity	= 0;
			udi->size		= 0;
		}
		else
		{
			udi->capacity	= size;
			udi->size		= size;
		}
	}

	template< typename T >
	AAALUACALL( clear )
	{
		LUAAAA_START( L, __FUNCTION__ )
		auto udi = get_ud_info<T>( L, 1 );
		INT32 size = udi->size;
		for( INT32 i=0; i<size; ++i )
			udi->data[i] = T(0);
		return 0; 
	}
	
	//	set( ud, id [,val1 [,val2] .. ]
	template< typename T >
	AAALUACALL( set )
	{
		LUAAAA_START( L, __FUNCTION__ )
		INT32 nb = l.get_arg_nb_min(3) - 2;
		auto udi = get_ud_info<T>( L, 1 );
		INT32 index_out = l.get_value_direct<INT32>(2) - 1;
		nb = MIN( nb, udi->size - index_out )  ;
		INT32 index_in = 3;
		while( nb-- > 0 )
		{
			udi->data[index_out] = T( lua_tonumber( L, index_in++ ) );
			++index_out;
		}
		return l.return_nothing();
	}

	//	fill( ud, val [,start [,stop] ]
	template< typename T >
	AAALUACALL( fill )
	{
		LUAAAA_START( L, __FUNCTION__ )
		INT32 CONST	nb_arg = l.get_arg_nb_min_max( 2, 4 );
		auto udi = get_ud_info<T>( L, 1 );
		T val = T(lua_tonumber( L, 2 ));
		INT32 i = 0;
		INT32 stop = udi->size;
		if( nb_arg >= 3 )
		{
			i = MAX( l.get_value_direct<INT32>(3)-1, 0 );
			if( nb_arg == 4 )
				stop = MIN( l.get_value_direct<INT32>(4), stop );
		}
		for( ; i<stop; ++i )
			udi->data[i] = val;
		return 0; 
	}

	template< typename T >
	// metatable method for handling "array[index]"
	AAALUACALL( __index )
	{ 
		auto udi = get_ud_info<T>( L, 1 );
		INT32 index = INT32(lua_tointeger( L, 2 ));
		if( index == 0  )	// avoid extra tests when d is not 0
		{
			int type = lua_type( L, 2 );
			if( type != LUA_TSTRING )  
				tag_error( L, 2, LUA_TNUMBER );

			const char* key = lua_tostring( L, 2 );

			lua_getmetatable( L, 1 );
			lua_getfield( L, -1, key );
			// Either key is name of a method in the metatable
			if( !lua_isnil(L, -1) )
				return 1;

			// ... or its a field access, so recall as self.get(self, value).
			//lua_settop(L, 2);
			//return array_get(L);

			luaL_argerror( L, 2, lua_pushfstring( L, "array: got index \"%s\" can't find this", key ) );

		}
		if( INSIDE_MIN_MAX( index, 1, udi->size ) )
		{
			lua_pushnumber( L, udi->data[index-1] );
			return 1;
		}
		lua_pushnil( L );
		return 1;
	}

	template< typename T >
	// metatable method for handle "array[index] = value"
	AAALUACALL( __newindex )
	{ 
		auto udi = get_ud_info<T>( L, 1 );
		//int index = luaL_checkint(L, 2);
		INT32 index = INT32(lua_tointeger(L, 2));
		//	FP32 value = (FP32) luaL_checknumber( L, 3 );
		auto value = lua_tonumber( L, 3 );
		if( INSIDE_MIN_MAX( index, 1, udi->size ) )
		{
			udi->data[index-1] = T(value);
			return 0;
		}
		if( index < 1 )
			luaL_argerror( L, 2, lua_pushfstring( L, "array: got index %d ", index ) );
		else
			luaL_argerror( L, 2, lua_pushfstring( L, "array: got index %d with an array of size %d", index, udi->size ) );
		return 0;
	}

	template< typename T >
	AAALUACALL( __len )
	{
		LUAAAA_START( L, __FUNCTION__ )
		auto udi = get_ud_info<T>( L, 1 );
		return l.return_int32( udi->size ); 
	}

	template< typename T >
	// metatable method for handle garbage collection
	AAALUACALL( ___gc )	//	visual choke on __gc
	{ 
		auto udi = get_ud_info<T>( L, 1 );
		FREE_ALIGNED( udi->data );
		store_data_info( udi, (T*)nullptr, 0 );
		return 0; 
	}

	template< typename T >
	// resize the table
	AAALUACALL( resize  )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.get_arg_nb_min( 2 );
		auto udi = get_ud_info<T>( L, 1 );
		INT32 size = l.get_int32( 2 );
		store_data_info( udi, (T*) REALLOC_ALIGNED( udi->data, size * sizeof(T), 0 ), size );
		return 0;
	}

	template< typename T >
	// create a metatable for our array type
	static void create_type( lua_State* CONST L, C_PCHAR_C ARRAY_NAME )
	{
		static CONST luaL_Reg array_meta[] =
		{
			{	"__index",		__index<T>		},
			{	"__newindex",	__newindex<T>	},
			{	"__len",		__len<T>		},
			{	"__gc",			___gc<T>		},
			{	"free",			___gc<T>		},
			{	"set",			set<T>			},
			{	"resize",		resize<T>		},
			{	"clear",		clear<T>		},
			{	"fill",			fill<T>			},
			{	nullptr,		nullptr			}
		};
		aaalua::new_metatable( L, ARRAY_NAME );
	//	luaL_register( L, ARRAY_NAME, array_fp32 );
		luaL_openlib( L, nullptr, array_meta, 0 );
	//	luaI_openlib
	//	luaI_openlib( L, ARRAY_NAME, array_fp32_reg, 0 );
		lua_pop(L, 1);
	}

	template< typename T, GLenum gl_type >
	// expose an array to lua, by storing it in a userdata with the array metatable
	int get_new( lua_State* L, INT32 CONST index, C_PCHAR_C name  )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 size = INT32(luaL_checkinteger( L, index ));
		auto udi = (ud_info<T>*) lua_newuserdata( L, sizeof(ud_info<T>) );
		udi->gl_type = gl_type;
		store_data_info( udi, (T*) MALLOC_ALIGNED( size * sizeof(T), 0 ), size );

		luaL_getmetatable( L, name );
		lua_setmetatable(L, -2);

		return 1;
	}

	//	input	(obj_ref or obj_name), id (start at one)
	//	set_dataset_float(	bdd, id, v1 [,vn...] )	-- id start at 1

	CONSTEXPR C_PCHAR_C names[]	=
	{
		"array_int8",	"array_uint8",
		"array_int16",	"array_uint16",
		"array_int32",	"array_uint32",
		"array_fp32",
		"array_fp64"
	};
	//we should deal also with
	//	GL_HALF_FLOAT,
	//	GL_FIXED,
	//	GL_INT_2_10_10_10_REV, GL_UNSIGNED_INT_2_10_10_10_REV and GL_UNSIGNED_INT_10F_11F_11F_REV

	AAALUACALL( new_int8 )		{	return get_new< INT8,		GL_BYTE				>	( L, 1, names[0] );	}
	AAALUACALL( new_uint8 )		{	return get_new< UINT8,		GL_UNSIGNED_BYTE	>	( L, 1, names[1] );	}
	AAALUACALL( new_int16 )		{	return get_new< INT16,		GL_SHORT			>	( L, 1, names[2] );	}
	AAALUACALL( new_uint16 )	{	return get_new< UINT16,		GL_UNSIGNED_SHORT	>	( L, 1, names[3] );	}
	AAALUACALL( new_int32 )		{	return get_new< INT32,		GL_INT				>	( L, 1, names[4] );	}
	AAALUACALL( new_uint32 )	{	return get_new< UINT32,		GL_UNSIGNED_INT		>	( L, 1, names[5] );	}
	AAALUACALL( new_fp32 )		{	return get_new< FP32,		GL_FLOAT			>	( L, 1, names[6] );	}
	AAALUACALL( new_fp64 )		{	return get_new< DOUBLE,		GL_DOUBLE			>	( L, 1, names[7] );	}



	void	register_array( lua_State* CONST L )
	{
		LUAAAA_START( L, __FUNCTION__ );
	
		create_type<	INT8	>	( L, names[0] );
		create_type<	UINT8	>	( L, names[1] );
		create_type<	INT16	>	( L, names[2] );
		create_type<	UINT16	>	( L, names[3] );
		create_type<	INT32	>	( L, names[4] );
		create_type<	UINT32	>	( L, names[5] );
		create_type<	FP32	>	( L, names[6] );
		create_type<	DOUBLE	>	( L, names[7] );

		l.define_table( "array" );

			ADD_FN( new_int8	);
			ADD_FN( new_uint8	);
			ADD_FN( new_int16	);
			ADD_FN( new_uint16	);
			ADD_FN( new_int32	);
			ADD_FN( new_uint32	);
			ADD_FN( new_fp32	);
			ADD_FN( new_fp64	);

		l.pop( 1 );	//pop new table
	}

	INT8*	get_ud_data_int8	( lua_State* CONST L, int CONST index, INT32& size	)	{	return get_data< INT8,		GL_BYTE				>( L, index, size	);	}
	UINT8*	get_ud_data_uint8	( lua_State* CONST L, int CONST index, INT32& size	)	{	return get_data< UINT8,		GL_UNSIGNED_BYTE	>( L, index, size	);	}
	INT16*	get_ud_data_int16	( lua_State* CONST L, int CONST index, INT32& size	)	{	return get_data< INT16,		GL_SHORT			>( L, index, size	);	}
	UINT16*	get_ud_data_uint16	( lua_State* CONST L, int CONST index, INT32& size	)	{	return get_data< UINT16,	GL_UNSIGNED_SHORT	>( L, index, size	);	}
	INT32*	get_ud_data_int32	( lua_State* CONST L, int CONST index, INT32& size	)	{	return get_data< INT32,		GL_INT				>( L, index, size	);	}
	UINT32*	get_ud_data_uint32	( lua_State* CONST L, int CONST index, INT32& size	)	{	return get_data< UINT32,	GL_UNSIGNED_INT		>( L, index, size	);	}
	FP32*	get_ud_data_fp32	( lua_State* CONST L, int CONST index, INT32& size	)	{	return get_data< FP32,		GL_FLOAT			>( L, index, size	);	}
	FP32*	get_ud_data_fp32	( lua_State* CONST L, int CONST index				)	{	return get_data_as< FP32						>( L, index			);	}
	DOUBLE*	get_ud_data_fp64	( lua_State* CONST L, int CONST index, INT32& size	)	{	return get_data< DOUBLE,	GL_DOUBLE			>( L, index, size	);	}

}	//end namespace n_array
}	//end namespace aaalua


