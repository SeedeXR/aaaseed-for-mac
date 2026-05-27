#include "gol/gol_os.h"
#include "gol/gol_tex.h"
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#include "err.h"
#include "system/win32/SystemError.h"


namespace GOL	{

o_str o_glew_version;

namespace OS	{

bool	b_vsync_asked_ui	= false;
bool	b_vsync_allow_ui	= false;
bool	b_vsync_can			= false;
INT32	vsync_state = -1;

bool	have_extension_wgl( C_PCHAR_C ext )
{
	if( !ext || *ext==0 )
		return false;

	bool b_found = false;
	//this is also an wglGetExtensionsStringARB version using an hdc as input argument
	if( wglGetExtensionsStringEXT )
	{
		C_PCHAR	str = (C_PCHAR) wglGetExtensionsStringEXT();
		if( str )
			b_found = strstr( str, ext ) != nullptr;
	}

	if( b_found )
		GOOD_PRINT_STRING(		"WGL Extension found %s",		ext );
	else
		WARNING_PRINT_STRING(	"WGL Extension not found %s",	ext );

	return b_found;
}

void	print_extension_wgl()
{
	GOOD_PRINT_STRING( "Extensions WGL : " );
	if( wglGetExtensionsStringEXT )
	{
		PRINT_STRING_BY_BLOCK( (char *)wglGetExtensionsStringEXT(), 80 );
	}
	//C_PCHAR str = wglGetExtensionsStringEXT();
	//HDC hdc = ::GetDC(hwnd);
}

void	init_vsync()
{
	b_vsync_can = have_extension_wgl( "WGL_EXT_swap_control" );
}

void	enable_vsync()
{
	if( is_vsync_can() )
	{
		BOOL b = wglSwapIntervalEXT( 1 );
		if( b )
		{
			vsync_state = wglGetSwapIntervalEXT();
			GOOD_PRINT_STRING( "Enable Vsync succeed state is now: %d", vsync_state );
		}
		else
		{
			ERR_PRINT_STRING( "%s() Can not Enable vsync : error was %s", __FUNCTION__, aaa::system::get_err_message().c_str() );
			vsync_state = wglGetSwapIntervalEXT();
			ERR_PRINT_STRING( "Vsync state is now : %d", vsync_state );
		}
	}
}
void	disable_vsync()
{
	if( is_vsync_can() )
	{
		BOOL b = wglSwapIntervalEXT( 0 );
		if( b )
		{
			vsync_state = wglGetSwapIntervalEXT();
			GOOD_PRINT_STRING( "Disable Vsync succeed state is now: %d", vsync_state );
		}
		else
		{
			ERR_PRINT_STRING( "%s() Can not Disable vsync : error was %s", __FUNCTION__, aaa::system::get_err_message().c_str() );
			vsync_state = wglGetSwapIntervalEXT();
			ERR_PRINT_STRING( "Vsync state is now : %d", vsync_state );
		}
	}
}
/*
UINT	wglGetGPUIDsAMD(UINT maxCount, UINT *ids);
INT		wglGetGPUInfoAMD(UINT id, INT property, GLenum dataType,UINT size, void *data);
UINT	wglGetContextGPUIDAMD(HGLRC hglrc);
HGLRC	wglCreateAssociatedContextAMD(UINT id);
HGLRC	wglCreateAssociatedContextAttribsAMD(UINT id, HGLRC hShareContext,  const int *attribList);					 
BOOL	wglDeleteAssociatedContextAMD(HGLRC hglrc);
BOOL	wglMakeAssociatedContextCurrentAMD(HGLRC hglrc);
HGLRC	wglGetCurrentAssociatedContextAMD(void);	
VOID	wglBlitContextFramebufferAMD( HGLRC dstCtx, GLint srcX0, GLint srcY0,
										GLint srcX1, GLint srcY1, GLint dstX0,
										GLint dstY0, GLint dstX1, GLint dstY1,
										GLbitfield mask, GLenum filter);
*/

void	init()
{
	static bool b_launch_first = true;		
	if( b_launch_first )
	{
		GLenum	glew_err = glewInit();
		if( glew_err != GLEW_OK )
		{	//	problem: glewInit failed, something is seriously wrong
			BOX_ERR( "GlewInit() Error: %s", glewGetErrorString( glew_err ) );
			return;
		}
		o_glew_version.set( (C_PCHAR_C) glewGetString( GLEW_VERSION ) );
		GOOD_PRINT_STRING( "Glew Inited version is : %s", o_glew_version.get() );

		print_extension_wgl();


		if( glGetError() != GL_NO_ERROR )
		{
			// Log error
			ERR_PRINT_STRING( "%s() fatal error, OpenGL extensions initialization failed", __FUNCTION__ );
			
			// Create fatal error window and exit app
			//sysutils::createFatalErrorWindow( "OpenGL initialization failed.\n Minimum required version is 4.2. \n Please check your hardware and drivers capabilities." );
		}
		
		// Retrieve OpenGL version
		INT32 version_major = GOL::get_integer( GL_MAJOR_VERSION );
		INT32 version_minor = GOL::get_integer( GL_MINOR_VERSION );
		
		
		// Test OpenGL version
		if( version_major < VERSION_MAJOR_REQUIRED )
		{
			// Log error
			ERR_PRINT_STRING( "fatal error, OpenGL version is %i . %i \n", version_major, version_minor );
			
			// Create fatal error window and exit app
			//sysutils::createFatalErrorWindow( "OpenGL initialization failed.\n \n Please update your drivers." );

		}
		else
		{
			if( version_minor < VERSION_MINOR_REQUIRED )
			{
				// Log error
				ERR_PRINT_STRING( "fatal error, OpenGL version is %i . %i \n", version_major, version_minor );
				
				// Create fatal error window and exit app
				//sysutils::createFatalErrorWindow( "OpenGL initialization failed.\n \n Please update your drivers." );
			}
		}

//done by Glew apparently 
#if 0
		// Load GL_ARB_debug_output
		glDebugMessageControlARB = (PFNGLDEBUGMESSAGECONTROLARBPROC) wglGetProcAddress("glDebugMessageControlARB");
		glDebugMessageInsertARB = (PFNGLDEBUGMESSAGEINSERTARBPROC) wglGetProcAddress("glDebugMessageInsertARB");
		glDebugMessageCallbackARB = (PFNGLDEBUGMESSAGECALLBACKARBPROC) wglGetProcAddress("glDebugMessageCallbackARB");
		glGetDebugMessageLogARB = (PFNGLGETDEBUGMESSAGELOGARBPROC) wglGetProcAddress("glGetDebugMessageLogARB");
#endif

		b_launch_first = false;
	}
}

}	//namespace OS

namespace CTX	{

C_PCHAR_C	str_profile[PROFILE_MAX_NB] =
{
	"No",
	"Core",
	"Compatibility"
};

bool	b_start_with_depth			= true;
bool	b_start_with_double			= true;
bool	b_start_with_alpha			= true;
bool	b_start_with_stereo			= true;
bool	b_start_with_stencil		= true;
bool	b_start_with_multisample;
INT32	opengl_version_asked_major	= 0;
INT32	opengl_version_asked_minor	= 0;
INT32	s_start_profile				= PROFILE_NO;
bool	b_start_with_debug			= false;

INT32	start_window_x				= 0;
INT32	start_window_y				= 0;
INT32	start_window_sx				= 320;
INT32	start_window_sy				= 240;

o_str	vendor						;
o_str	renderer					;
o_str	version						;
o_str	ctx_profile					;
bool	b_ctx_forward_compatible	;
bool	b_ctx_debug					;
bool	b_ctx_robust_access			;
bool	b_ctx_no_error				;
o_str	shading						;

o_str*	get_vendor_pt()				{	return &vendor;						}
o_str*	get_renderer_pt()			{	return &renderer;					}
o_str*	get_version_pt()			{	return &version;					}
o_str*	get_profile_pt()			{	return &ctx_profile;				}
bool*	get_forward_compatible_pt()	{	return &b_ctx_forward_compatible;	}
bool*	get_debug_pt()				{	return &b_ctx_debug;				}
bool*	get_robust_access_pt()		{	return &b_ctx_robust_access;		}
bool*	get_no_error_pt()			{	return &b_ctx_no_error;				}
o_str*	get_shading_pt()			{	return &shading;					}


bool	b_ogl_is_nvidia	= false;
bool	b_ogl_is_amd	= false;
bool	b_ogl_is_intel	= false;
bool	b_ogl_is_mesa	= false;
bool	b_ogl_is_apple	= false;

INT32	version_major = 0;
INT32	version_minor = 0;
	
INT32	get_version_major()			{	return version_major;	}
INT32	get_version_minor()			{	return version_minor;	}

INT32*	get_version_major_pt()		{	return &version_major;	}
INT32*	get_version_minor_pt()		{	return &version_minor;	}

void	init_info()
{
	vendor.set(   (CHAR*)glGetString( GL_VENDOR )	);
	renderer.set( (CHAR*)glGetString( GL_RENDERER )	);
	version.set(  (CHAR*)glGetString( GL_VERSION )	);

	b_ogl_is_nvidia	= false;
	b_ogl_is_amd	= false;
	b_ogl_is_intel	= false;
	b_ogl_is_mesa	= false;
	b_ogl_is_apple	= false;

	if(			str_find_str_nocase( vendor.get(), "nvidia" )	)
		b_ogl_is_nvidia	= true;
	else if(	str_find_str_nocase( vendor.get(), "ati" ) || str_find_str_nocase( vendor.get(), "amd" ) )
		b_ogl_is_amd	= true;
	else if(	str_find_str_nocase( vendor.get(), "intel" )	)
		b_ogl_is_intel	= true;
	else
	{
		if(	str_find_str_nocase( vendor.get(), "mesa" )	|| str_find_str_nocase( renderer.get(), "mesa" ) )
			b_ogl_is_mesa = true;
		if(	str_find_str_nocase( vendor.get(), "apple" ) || str_find_str_nocase( renderer.get(), "apple" ) )
			b_ogl_is_apple = true;
	}

	if( sscanf( version.get(), "%d.%d", &version_major, &version_minor ) != 2 )
		ERR_PRINT_STRING( "Can't extract OpenGl version numbers from string %.256s", version.get() );

	if( version_major < 3 ) 
	{
		ctx_profile.set( "No" );
		//_pixelFormat.setOption(DeprecatedFunctions);
	} 
	else 
	{
		GLint value = 0;
		if( test_version_or_extensions( 3,2 ) )
			value = GOL::get_integer( GL_CONTEXT_PROFILE_MASK );
		C_PCHAR str;
		switch( value )
		{
		case GL_CONTEXT_CORE_PROFILE_BIT:			str = "Core";			break;
		case GL_CONTEXT_COMPATIBILITY_PROFILE_BIT:	str = "Compatibility";	break;
		default:									str = "No";				break;
		}
		ctx_profile.set( str );	

		value = GOL::get_integer( GL_CONTEXT_FLAGS );
		b_ctx_forward_compatible	= value & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT ;
		b_ctx_debug					= value & GL_CONTEXT_FLAG_DEBUG_BIT ;
		b_ctx_robust_access			= value & GL_CONTEXT_FLAG_ROBUST_ACCESS_BIT ;
		b_ctx_no_error				= value & GL_CONTEXT_FLAG_NO_ERROR_BIT ;
	}
	
	if( test_version_or_extensions( 4,5 ) )
	{
		b_named_buffer_can			= true;
		b_direct_state_access_can	= true;
	}
	else
	{
		b_named_buffer_can			= false;
		b_direct_state_access_can	= false;
	}

	if( test_version_or_extensions( 2,0, "GL_ARB_shading_language_100" ) )
		shading.set( (CHAR*)glGetString( GL_SHADING_LANGUAGE_VERSION ) );
	else
		shading.set( "No Shader support" );

	b_copy_image_sub_can = test_version_or_extensions( 4,3 );
}

void	print_versions()
{
	DBG_PRINT_STRING(  "################## Open GL Context Begin ##################" );
	GOOD_PRINT_STRING( "Vendor   : %s", vendor.get() );
	GOOD_PRINT_STRING( "Renderer : %s", renderer.get() );
	GOOD_PRINT_STRING( "Version  : %s", version.get() );
	GOOD_PRINT_STRING( "Shading  : %s", shading.get() );
	GOOD_PRINT_STRING( "Profile  : %s", ctx_profile.get() );
	GOOD_PRINT_STRING( "\tForward Compatibility : %s", b_ctx_forward_compatible	? "Yes" : "No" );
	GOOD_PRINT_STRING( "\tDebug                 : %s", b_ctx_debug				? "Yes" : "No" );
	GOOD_PRINT_STRING( "\tRobust Access         : %s", b_ctx_robust_access		? "Yes" : "No" );
	GOOD_PRINT_STRING( "\tNo Error              : %s", b_ctx_no_error			? "Yes" : "No" );
	DBG_PRINT_STRING(  "################## Open GL Context End ####################" );
}

}	//namespace CTX

}	//namespace GOL