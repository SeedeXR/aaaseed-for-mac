
#ifdef AAA_SYSTEM_CURSOR_H
#error "SYSTEM_CURSOR_H included more than once."
#endif
#define AAA_SYSTEM_CURSOR_H 1

//////////////////////////////////////////////////
//                                              //
//  CAUTION :                                   //
//  this header is shared between target        //
//  system compilations (Windows, OSX, LINUX)   //
//                                              //
//////////////////////////////////////////////////


#ifndef	AAA_AAA_DEF_H
#	include "aaa_def.h"
#endif
#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif


// here for HWND
//#define WIN32_LEAN_AND_MEAN  // Exclude rarely-used Windows stuff
//#ifndef _WINDEF_
//#	include <windef.h>
//#endif
//#ifndef _WINDOWS_
//#include <windows.h>         // Defines architecture macros (e.g., _WIN32, _WIN64)
//#endif
#ifndef AAA_NATIVE_SYSTEM_H
#	include "system/Native_System.h"
#endif
#ifndef AAA_NATIVE_TYPES_H
#	include "system/Native_Types.h"
#endif
#if defined (NATIVE_OS_WIN32)
#	include <Shtypes.h>
#endif


namespace n_cursor
{
	enum STATE : INT32
	{
		CURSOR_RIGHT_ARROW              = 0x0000,
		CURSOR_LEFT_ARROW               = 0x0001,
		CURSOR_INFO                     = 0x0002,
		CURSOR_DESTROY                  = 0x0003,
		CURSOR_HELP                     = 0x0004,
		CURSOR_CYCLE                    = 0x0005,
		CURSOR_SPRAY                    = 0x0006,
		CURSOR_WAIT                     = 0x0007,
		CURSOR_TEXT                     = 0x0008,
		CURSOR_CROSSHAIR                = 0x0009,
		CURSOR_UP_DOWN                  = 0x000A,
		CURSOR_LEFT_RIGHT               = 0x000B,
		CURSOR_TOP_SIDE                 = 0x000C,
		CURSOR_BOTTOM_SIDE              = 0x000D,
		CURSOR_LEFT_SIDE                = 0x000E,
		CURSOR_RIGHT_SIDE               = 0x000F,
		CURSOR_TOP_LEFT_CORNER          = 0x0010,
		CURSOR_TOP_RIGHT_CORNER         = 0x0011,
		CURSOR_BOTTOM_RIGHT_CORNER      = 0x0012,
		CURSOR_BOTTOM_LEFT_CORNER       = 0x0013,
		CURSOR_INHERIT                  = 0x0064,
		CURSOR_NONE                     = 0x0065,
		CURSOR_FULL_CROSSHAIR           = 0x0066,
//		CURSOR_UNDEFINED				= 0xffff,
	};
	INT32 CONST CURSOR_IMAGE_NB = CURSOR_BOTTOM_LEFT_CORNER + 1;

	extern void show( bool const b_in );

	/** \! Set under cursor window cursor. */
	extern void force( HWND CONST hd_win );
	extern void force();

//	extern void set( STATE CONST cursor_type, HWND CONST handle );
//	extern void set( STATE CONST cursor_type );

	extern  void update_position_screen();
	/**
	* @brief get cursor position in screen coordinates
	*/
	extern void get_position_pixel_screen( INT32* CONST pos );
	/**
	* @brief get cursor position in main window coordinates
	*/
	extern void get_position_pixel_window_main( INT32* CONST pos );
	/**
	* @brief set cursor position
	* @param p_targetPosition cursor position as glm::vec2 
	*/
//	extern void set_position( Vec2i CONST & p_targetPosition );

#if AAA_NEW_DESIGN()
	CONSTEXPR STATE	AAA_CURSOR_MOVE				= CURSOR_INFO;
	CONSTEXPR STATE	AAA_CURSOR_MOVE_HORI		= CURSOR_LEFT_RIGHT;
	CONSTEXPR STATE	AAA_CURSOR_MOVE_VERT		= CURSOR_UP_DOWN;
	CONSTEXPR STATE	AAA_CURSOR_TURN				= CURSOR_CYCLE;
	CONSTEXPR STATE	AAA_CURSOR_DEF				= CURSOR_RIGHT_ARROW;
	CONSTEXPR STATE	AAA_CURSOR_LEFT				= CURSOR_LEFT_ARROW;
	CONSTEXPR STATE	AAA_CURSOR_NONE				= CURSOR_NONE;
	CONSTEXPR STATE	AAA_CURSOR_INACTIVE			= CURSOR_WAIT;
	CONSTEXPR STATE	AAA_CURSOR_INACTIVE_DOWN	= CURSOR_DESTROY;
#else
	CONSTEXPR INT32	AAA_CURSOR_MOVE				= GLUT_CURSOR_INFO;
	CONSTEXPR INT32	AAA_CURSOR_MOVE_HORI		= GLUT_CURSOR_LEFT_RIGHT;
	CONSTEXPR INT32	AAA_CURSOR_MOVE_VERT		= GLUT_CURSOR_UP_DOWN;
	CONSTEXPR INT32	AAA_CURSOR_TURN				= GLUT_CURSOR_CYCLE;
	CONSTEXPR INT32	AAA_CURSOR_DEF				= GLUT_CURSOR_RIGHT_ARROW;
	CONSTEXPR INT32	AAA_CURSOR_LEFT				= GLUT_CURSOR_LEFT_ARROW;
	CONSTEXPR INT32	AAA_CURSOR_NONE				= GLUT_CURSOR_NONE;
	CONSTEXPR INT32	AAA_CURSOR_INACTIVE			= GLUT_CURSOR_WAIT;
	CONSTEXPR INT32	AAA_CURSOR_INACTIVE_DOWN	= GLUT_CURSOR_DESTROY;
#endif

	extern	REAL			hide_delay;
	extern	bool			b_hide_when_static;
	extern	bool			b_hide_in_render;
	extern	bool			b_cursor_locked;
	extern	bool			b_cursor_visible;
	extern	bool			b_force_update_always;

	extern	void			update();
	extern	void			init();

	extern	void			flip_hide_in_render();
	extern	void			flip_hide_when_static();
	extern	void			flip();
	extern	void			set( n_cursor::STATE CONST cursor_in );
	extern	void			set_default();

	extern	bool			is_hide_delay();
};

