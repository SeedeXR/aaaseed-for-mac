#include "color_space_lua.h"
#include "language/lua/aaalua_glue.h"
#include "color_space.h"

namespace	aaalua
{
namespace	n_color
{
	AAALUACALL( rgb_to_hsv )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 3 );
		FP32	vec[3];
		l.get_v3( vec, 1 );
		aaa::color::hsv_from_rgb( vec );
		return l.return_fp32_v3( vec );
	}
	AAALUACALL( hsv_to_rgb )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 3 );
		FP32	vec[3];
		l.get_v3( vec, 1 );
		aaa::color::rgb_from_hsv( vec );
		return l.return_fp32_v3( vec );
	}
	AAALUACALL( rgb_to_grey )
	{
		LUAAAA_START( L, __FUNCTION__ );
		DBG_CHECK_ARG_NB( 3 );
		FP32	vec[3];
		l.get_v3( vec, 1 );
		return l.return_real( aaa::color::rgb_to_grey( vec ) );
	}

	void	register_color( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		l.define_table( "color" );
			ADD_FN( rgb_to_hsv				);
			ADD_FN( hsv_to_rgb				);
			ADD_FN( rgb_to_grey				);	
		l.pop( 1 );	//pop new table
	}
}	//end namespace n_color
}	//end namespace aaalua