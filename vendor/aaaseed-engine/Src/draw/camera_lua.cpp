#include "camera_lua.h"
#include "language/lua/aaalua_glue.h"
#include "seedcam.h"

namespace aaalua
{
namespace n_camera
{
	template<void (c_seedcam::*METHOD)(FP32*CONST) CONST >
	FINLINE INT32 call_seedcam_v3(lua_State* L, const char* lua_name)
	{
		LUAAAA_START(L, lua_name);
		INT32 arg_nb = l.get_arg_nb(3, 4);
		c_seedcam* cam;
		if (arg_nb == 3)
		{
			cam = c_seedcam::get_cur();
			l.get_v3(g_fp32, 1);
		}
		else
		{
			cam = l.get_aaa_obj_of_class<c_seedcam>(1);
			l.get_v3(g_fp32, 2);
		}
		// Call the member function
		(cam->*METHOD)(g_fp32);
		return l.return_fp32_v3(g_fp32);
	}
	#define COOR_CALL(lua_name, method)  AAALUACALL(lua_name)  { return call_seedcam_v3<&c_seedcam::method>(L, #lua_name); }
	COOR_CALL( world_to_cam,	coor_world_to_camera )
	COOR_CALL( cam_to_world,	coor_camera_to_world )
	COOR_CALL( world_to_screen,	coor_world_to_screen )
	COOR_CALL( screen_to_world,	coor_screen_to_world )
	COOR_CALL( cam_to_screen,	coor_camera_to_screen )
	COOR_CALL( screen_to_cam,	coor_screen_to_camera )


	//hack for a quick try (maraval/chercheur/editor)
	//	input	(obj_ref or obj_name)
	AAALUACALL(	set_cur )
	{
		LUAAAA_START( L, __FUNCTION__ );
		if( l.get_arg_nb( 0, 1 ) > 0 )
		{
			c_seedcam*	cam	= l.get_aaa_obj_of_class<c_seedcam>( 1 );
			c_seedcam::set_cur( cam );
		}
		else
			c_seedcam::set_cur_null();
		return l.return_nothing();
	}
	//	no input
	AAALUACALL(	get_cur )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_obj_ref( c_seedcam::get_cur(), "No current camera" );
	}

	AAALUACALL(	set_ui )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		c_seedcam*	cam		= l.get_aaa_obj_of_class<c_seedcam>( 1 );
		c_seedcam::set_ui( cam );
		return l.return_nothing();
	}
	AAALUACALL(	get_ui )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_obj_ref( c_seedcam::get_ui(), "No UI camera" );
	}
	AAALUACALL(	get_ui_or_find )
	{
		LUAAAA_START( L, __FUNCTION__ );
		return l.return_obj_ref( c_seedcam::get_ui_or_find(), "No UI camera, no camera for UI" );
	}

	void	register_camera( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		l.define_table( "camera" );
			ADD_FN( world_to_cam		);
			ADD_FN( cam_to_world		);
			ADD_FN( world_to_screen		);
			ADD_FN( screen_to_world		);
			ADD_FN( cam_to_screen		);
			ADD_FN( screen_to_cam		);
			ADD_FN( set_cur				);
			ADD_FN( get_cur				);
			ADD_FN( set_ui				);
			ADD_FN( get_ui				);
			ADD_FN( get_ui_or_find		);	
		l.pop( 1 );	//pop new table
	}

}	//end namespace n_camera
}	//end namespace aaalua