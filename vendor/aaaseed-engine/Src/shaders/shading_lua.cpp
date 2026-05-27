#include "shading_lua.h"
#include "shading.h"
#include "language/lua/aaalua_glue.h"
#include "language/lua/aaalua_array.h"
#include "math/v_base.h"
//#include "time/aaa_time.h"

namespace aaalua
{
namespace n_shading
{
	//	sha = shading.get_cur()
	AAALUACALL( get_cur )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 0 );		
		if( c_shading* sha	= c_shading::cur )
			return l.return_obj_ref( (c_obj_ui*)sha );
		l.error_message( "no current shading" );
		return l.return_nothing();
	}
	
	//todo generalize and use arg_nb or table/array size
	AAALUACALL( set_vert_vec4_spe )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32		CONST	nb_arg	=	l.get_arg_nb( 2, 19 );
		c_shading*	CONST	sha		=	l.get_aaa_obj_of_class<c_shading>( 1 );
		if( sha )
		{
			FP32* RESTRICT dst = sha->get_vs_vec4();
			if( nb_arg == 2 )
			{
				INT32 CONST	type = l.get_type( 2 );
				if( c_lua_state::is_type_user_data(type) )
				{
					FP32 CONST * CONST RESTRICT src = n_array::get_ud_data_fp32( l.get_state(), 2 );
					// this was not faster
					//MEMCPY( dst, src, 18 * sizeof(FP32), __FUNCTION__ );
					//__movsb( (UINT8*)dst, (UINT8 CONST *)src, 18 * sizeof(FP32) );
					cpy_v3( dst,	src		);
					cpy_v3( dst+4,	src+3	);
					cpy_v3( dst+8,	src+6	);
					cpy_v3( dst+12, src+9	);
					cpy_v3( dst+16, src+12	);
					cpy_v3( dst+20, src+15	);
				}
				else if( c_lua_state::is_type_table(type) )
				{	//accept arguments in a table
					//INT32 nb_fp32 = (INT32) lua_objlen( l.get_state(), 1 );	//	lua_rawlen for lua 5.2
					l.get_table_field_raw( 2, 1 );		*(dst+0)	= l.get_value_direct<FP32>(-1);
					l.get_table_field_raw( 2, 2 );		*(dst+1)	= l.get_value_direct<FP32>(-1);
					l.get_table_field_raw( 2, 3 );		*(dst+2)	= l.get_value_direct<FP32>(-1);

					l.get_table_field_raw( 2, 4 );		*(dst+4)	= l.get_value_direct<FP32>(-1);
					l.get_table_field_raw( 2, 5 );		*(dst+5)	= l.get_value_direct<FP32>(-1);
					l.get_table_field_raw( 2, 6 );		*(dst+6)	= l.get_value_direct<FP32>(-1);

					l.get_table_field_raw( 2, 7 );		*(dst+8)	= l.get_value_direct<FP32>(-1);
					l.get_table_field_raw( 2, 8 );		*(dst+9)	= l.get_value_direct<FP32>(-1);
					l.get_table_field_raw( 2, 9 );		*(dst+10)	= l.get_value_direct<FP32>(-1);

					l.get_table_field_raw( 2, 10 );		*(dst+12)	= l.get_value_direct<FP32>(-1);
					l.get_table_field_raw( 2, 11 );		*(dst+13)	= l.get_value_direct<FP32>(-1);
					l.get_table_field_raw( 2, 12 );		*(dst+14)	= l.get_value_direct<FP32>(-1);

					l.get_table_field_raw( 2, 13 );		*(dst+16)	= l.get_value_direct<FP32>(-1);
					l.get_table_field_raw( 2, 14 );		*(dst+17)	= l.get_value_direct<FP32>(-1);
					l.get_table_field_raw( 2, 15 );		*(dst+18)	= l.get_value_direct<FP32>(-1);

					l.get_table_field_raw( 2, 16 );		*(dst+20)	= l.get_value_direct<FP32>(-1);
					l.get_table_field_raw( 2, 17 );		*(dst+21)	= l.get_value_direct<FP32>(-1);
					l.get_table_field_raw( 2, 18 );		*(dst+22)	= l.get_value_direct<FP32>(-1);

					l.pop(18);
				}
			}
			else
			{
				*(dst+0)	= l.get_value_direct<FP32>(2);
				*(dst+1)	= l.get_value_direct<FP32>(3);
				*(dst+2)	= l.get_value_direct<FP32>(4);

				*(dst+4)	= l.get_value_direct<FP32>(5);
				*(dst+5)	= l.get_value_direct<FP32>(6);
				*(dst+6)	= l.get_value_direct<FP32>(7);

				*(dst+8)	= l.get_value_direct<FP32>(8);
				*(dst+9)	= l.get_value_direct<FP32>(9);
				*(dst+10)	= l.get_value_direct<FP32>(10);

				*(dst+12)	= l.get_value_direct<FP32>(11);
				*(dst+13)	= l.get_value_direct<FP32>(12);
				*(dst+14)	= l.get_value_direct<FP32>(13);

				*(dst+16)	= l.get_value_direct<FP32>(14);
				*(dst+17)	= l.get_value_direct<FP32>(15);
				*(dst+18)	= l.get_value_direct<FP32>(16);

				*(dst+20)	= l.get_value_direct<FP32>(17);
				*(dst+21)	= l.get_value_direct<FP32>(18);
				*(dst+22)	= l.get_value_direct<FP32>(19);
			}
		}
		return l.return_nothing();
	}


	void	register_shading( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );
	
		l.define_table( "shading" );

			ADD_FN(	get_cur );
			ADD_FN(	set_vert_vec4_spe );

		l.pop( 1 );	//pop new table
	}

}	//end namespace n_shading
}	//end namespace aaalua
