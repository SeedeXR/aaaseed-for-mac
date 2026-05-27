#include "fbo_lua.h"
#include "language/lua/aaalua_glue.h"
#include "fbo.h"

namespace aaalua
{
namespace n_fbo
{
	AAALUACALL( unbind )
	{
		LUAAAA_START( L, __FUNCTION__ );
		c_fbo::set_cur( nullptr );	
		return l.return_nothing();
	}
	AAALUACALL(	get_cur )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_obj_ref( c_fbo::get_cur(), "No current Fbo" );
	}
	AAALUACALL(	get_prev )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_obj_ref( c_fbo::get_prev(), "No previous Fbo" );
	}


	void	register_fbo( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		l.define_table( "fbo" );
			ADD_FN( unbind				);
			ADD_FN( get_cur				);
			ADD_FN( get_prev			);
		l.pop( 1 );	//pop new table
	}

}	//end namespace n_camera
}	//end namespace aaalua