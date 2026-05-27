
#ifdef AAA_GOL_OS_H
#error "GOL_OS_H included more than once."
#endif
#define AAA_GOL_OS_H 1


#ifndef AAA_GOL_BASE_H
#	include "gol_base.h"
#endif
#ifndef __WGLEW_H__
#	include "wglew.h"
#endif

class o_str;

namespace GOL	{

static INT32 CONST VERSION_MAJOR_REQUIRED = 2;
static INT32 CONST VERSION_MINOR_REQUIRED = 1;
extern o_str o_glew_version;

namespace OS	{

extern	bool	b_vsync_asked_ui;
extern	bool	b_vsync_can;
extern	bool	b_vsync_allow_ui;
extern	INT32	vsync_state;

void	print_extension_wgl();
bool	have_extension_wgl(	C_PCHAR_C ext );

FINLINE	bool	is_vsync_can()
{
	return b_vsync_can;
}

extern void	enable_vsync();
extern void disable_vsync();

FINLINE	void	set_vsync( bool CONST in )	//Nvidia allow other values -2, -1
{
	if( b_vsync_allow_ui && is_vsync_can() )
	{
		if( in )
		{
			if( vsync_state != 1 )
				enable_vsync();
		}
		else
		{
			if( vsync_state != 0 )
				disable_vsync();
		}
	}
}

extern	void	init_vsync();
extern	void	init();

}	//namespace OS

namespace CTX	{

enum PROFILE : INT32
	{
	PROFILE_NO = 0,
	PROFILE_CORE,
	PROFILE_COMPATIBILTY,
	PROFILE_MAX_NB,
	};

extern	C_PCHAR_C	str_profile[PROFILE_MAX_NB];

extern	bool		b_start_with_depth;
extern	bool		b_start_with_double;
extern	bool		b_start_with_alpha;
extern	bool		b_start_with_stereo;
extern	bool		b_start_with_stencil;
extern	bool		b_start_with_multisample;

extern	INT32		opengl_version_asked_major;
extern	INT32		opengl_version_asked_minor;
extern	INT32		s_start_profile;
extern	bool		b_start_with_debug;

extern	INT32		start_window_x;
extern	INT32		start_window_y;

extern	INT32		start_window_sx;
extern	INT32		start_window_sy;

extern	bool		b_ogl_is_nvidia;
extern	bool		b_ogl_is_amd;
extern	bool		b_ogl_is_intel;
extern	bool		b_ogl_is_mesa;
extern	bool		b_ogl_is_apple;

FINLINE	bool	is_nvidia()		{ return b_ogl_is_nvidia;	}
FINLINE	bool	is_amd()		{ return b_ogl_is_amd;		}
FINLINE	bool	is_intel()		{ return b_ogl_is_intel;	}
FINLINE	bool	is_mesa()		{ return b_ogl_is_mesa;		}
FINLINE	bool	is_apple()		{ return b_ogl_is_apple;	}

extern	INT32	get_version_major()			;
extern	INT32	get_version_minor()			;	

extern	INT32*	get_version_major_pt()		;
extern	INT32*	get_version_minor_pt()		;

extern	o_str*	get_vendor_pt()				;
extern	o_str*	get_renderer_pt()			;
extern	o_str*	get_version_pt()			;
extern	o_str*	get_profile_pt()			;
extern	bool*	get_forward_compatible_pt()	;
extern	bool*	get_debug_pt()				;
extern	bool*	get_robust_access_pt()		;
extern	bool*	get_no_error_pt()			;
extern	o_str*	get_shading_pt()			;

extern	void	init_info()					;
extern	void	print_versions()			;

}	//namespace CTX
}	//namespace GOL
