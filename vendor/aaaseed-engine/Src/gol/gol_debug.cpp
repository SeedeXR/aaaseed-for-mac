
#include "gol/gol_debug.h"
#include "gol/gol_base.h"
#include "gol/gol_os.h"
#include "str_utils.h"
#include "err.h"



namespace GOL	{

bool	b_debug_can										=	false;
bool	b_debug_do										=	false;

bool	b_debug_callback_ui								=	false;
bool	b_debug_error_ui								=	false;

bool	b_debug_severity_low_ui							=	true;
bool	b_debug_severity_medium_ui						=	true;
bool	b_debug_severity_high_ui						=	true;
bool	b_debug_severity_notification_ui				=	true;

bool	b_debug_deprecated_ui							=	false;
bool	b_debug_undefined_ui							=	false;
bool	b_debug_portability_ui							=	false;

bool	b_debug_performance_ui										=	false;
bool	b_debug_performance_pbo_nvidia_ui							=	false;
bool	b_debug_performance_shader_recompiled_by_state_nvidia_ui	=	false;
bool	b_debug_performance_buffer_usage_ui							=	false;

bool	b_debug_other_ui											=	false;
bool	b_debug_other_uniform_array_ignored_nvidia_ui				=	false;
bool	b_debug_other_buffer_nvidia_ui								=	false;
bool	b_debug_other_buffer_detailed_nvidia_ui						=	false;
bool	b_debug_other_framebuffer_detailed_nvidia_ui				=	false;
bool	b_debug_other_texture_base_level_undefined_ui				=	false;
bool	b_debug_other_buffer_usage_ui								=	false;

bool	b_debug_callback											=	false;
bool	b_debug_error												=	true;

bool	b_debug_deprecated											=	true;
bool	b_debug_undefined											=	true;
bool	b_debug_portability											=	true;

bool	b_debug_performance											=	true;
bool	b_debug_performance_pbo_nvidia								=	true;
bool	b_debug_performance_shader_recompiled_by_state_nvidia		=	true;
bool	b_debug_performance_buffer_usage							=	true;

bool	b_debug_other												=	true;
bool	b_debug_other_uniform_array_ignored_nvidia					=	true;
bool	b_debug_other_buffer_nvidia									=	true;
bool	b_debug_other_buffer_detailed_nvidia						=	true;
bool	b_debug_other_framebuffer_detailed_nvidia					=	true;
bool	b_debug_other_texture_base_level_undefined					=	true;
bool	b_debug_other_buffer_usage									=	true;

UINT32	error_count					= 0;
UINT32	error_count_low				= 0;
UINT32	error_count_medium			= 0;
UINT32	error_count_high			= 0;
UINT32	error_count_notification	= 0;
	
void	GLAPIENTRY	debug_callback( GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam )
{
	++error_count;
	bool	b_show;
	C_PCHAR	str_severity;
	switch( severity )
	{
	case GL_DEBUG_SEVERITY_LOW:				b_show = GOL::b_debug_severity_low_ui;			str_severity = "LOW";			++error_count_low;			break;
	case GL_DEBUG_SEVERITY_MEDIUM:			b_show = GOL::b_debug_severity_medium_ui;		str_severity = "MEDIUM";		++error_count_medium;		break;
	case GL_DEBUG_SEVERITY_HIGH:			b_show = GOL::b_debug_severity_high_ui;			str_severity = "HIGH";			++error_count_high;			break;
	case GL_DEBUG_SEVERITY_NOTIFICATION:	b_show = GOL::b_debug_severity_notification_ui;	str_severity = "NOTIFICATION";	++error_count_notification;	break;
	default:								b_show = true;									str_severity = "UNDEFINED";		break;
	}
	if( b_show )
	{
		C_PCHAR	str_src;
		switch( source )
		{
		case GL_DEBUG_SOURCE_API:				str_src = "API";					break;
		case GL_DEBUG_SOURCE_WINDOW_SYSTEM:		str_src = "WINDOW_SYSTEM";			break;
		case GL_DEBUG_SOURCE_SHADER_COMPILER:	str_src = "SHADER_COMPILER";		break;
		case GL_DEBUG_SOURCE_THIRD_PARTY:		str_src = "THIRD_PARTY";			break;
		case GL_DEBUG_SOURCE_APPLICATION:		str_src = "APPLICATION";			break;
		case GL_DEBUG_SOURCE_OTHER:				str_src = "OTHER";					break;
		default:								str_src = "UNDEFINED";				break;
		}
 
		C_PCHAR	str_type;
		switch( type )
		{
		case GL_DEBUG_TYPE_ERROR:				str_type = "ERROR";					break;
		case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:	str_type = "DEPRECATED_BEHAVIOR";	break;
		case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:	str_type = "UNDEFINED_BEHAVIOR";	break;
		case GL_DEBUG_TYPE_PORTABILITY:			str_type = "PORTABILITY";			break;
		case GL_DEBUG_TYPE_PERFORMANCE:			str_type = "PERFORMANCE";			break;
		case GL_DEBUG_TYPE_MARKER:				str_type = "MARKER";				break;
		case GL_DEBUG_TYPE_PUSH_GROUP:			str_type = "PUSH_GROUP";			break;
		case GL_DEBUG_TYPE_POP_GROUP:			str_type = "POP_GROUP";				break;
		case GL_DEBUG_TYPE_OTHER:				str_type = "OTHER";					break;
		default:								str_type = "UNDEFINED";				break;
		}

		ERR_PRINT_STRING( "%u, debug  %s, type = %s, severity = %s, id = %d", error_count, str_src, str_type, str_severity, id );
		err_print( "%s", message );
	}
}
void	debug_set_callback( )
{
	glDebugMessageCallback( debug_callback, nullptr );
}
void	debug_remove_callback( )
{
	glDebugMessageCallback( nullptr, nullptr );
}

void	init_debug()
{
	b_debug_can = test_version_or_extensions( 0,0, "KHR_debug", "GL_ARB_debug_output" );
	b_debug_do = b_debug_can && CTX::b_start_with_debug;
	if( b_debug_do )
	{
		GOL::enable( GL_DEBUG_OUTPUT_SYNCHRONOUS );	// Callback is called as soon as there is an error
		// callback enabled when starting AAASeed, callback will be really set once pref are loaded
		debug_set_callback();
		b_debug_callback = true;
	}
}
void	deinit_debug()
{
	if( b_debug_do )
	{
		// remove callback
		debug_remove_callback();
		b_debug_callback = false;
	}
}

FINLINE void	debug_msg_control_type( GLenum CONST type, bool CONST b_on )
{
	glDebugMessageControl(	GL_DONT_CARE,	type,			GL_DONT_CARE,	0, nullptr, b_on ? GL_TRUE : GL_FALSE );
}
//FINLINE void	debug_msg_control_severity( GLenum CONST severity, bool CONST b_on )
//{
//	glDebugMessageControl(	GL_DONT_CARE,	GL_DONT_CARE,	severity,		0, nullptr, b_on ? GL_TRUE : GL_FALSE );
//}

void	update_debug()
{
	if( b_debug_do )
	{
		if( b_debug_callback_ui != b_debug_callback )
		{
			b_debug_callback = b_debug_callback_ui;
			if( b_debug_callback )
			{
				debug_set_callback();
/*
				debug_msg_control_type( GL_DEBUG_TYPE_ERROR,				b_debug_error		);
				debug_msg_control_type( GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR,	b_debug_deprecated	);
				debug_msg_control_type( GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR,	b_debug_undefined	);
				debug_msg_control_type( GL_DEBUG_TYPE_PORTABILITY,			b_debug_portability );
				debug_msg_control_type( GL_DEBUG_TYPE_PERFORMANCE,			b_debug_performance );
				debug_msg_control_type( GL_DEBUG_TYPE_OTHER,				b_debug_other		);
*/
			}
			else
			{
				debug_remove_callback();
			}
		}
		if( b_debug_callback )
		{
			CONSTEXPR GLenum debug_src = GL_DEBUG_SOURCE_API;
			if( b_debug_error != b_debug_error_ui )
			{
				b_debug_error = b_debug_error_ui;
				debug_msg_control_type( GL_DEBUG_TYPE_ERROR,				b_debug_error		);
			}
			if( b_debug_deprecated != b_debug_deprecated_ui )
			{
				b_debug_deprecated = b_debug_deprecated_ui;
				debug_msg_control_type( GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR,	b_debug_deprecated	);
			}
			if( b_debug_undefined != b_debug_undefined_ui )
			{
				b_debug_undefined = b_debug_undefined_ui;
				debug_msg_control_type( GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR,	b_debug_undefined	);
			}
			if( b_debug_portability != b_debug_portability_ui )
			{
				b_debug_portability = b_debug_portability_ui;
				debug_msg_control_type( GL_DEBUG_TYPE_PORTABILITY,			b_debug_portability	);
			}
			if(		b_debug_performance										!= b_debug_performance_ui
				||	b_debug_performance_pbo_nvidia							!= b_debug_performance_pbo_nvidia_ui
				||	b_debug_performance_shader_recompiled_by_state_nvidia	!= b_debug_performance_shader_recompiled_by_state_nvidia_ui
				||	b_debug_performance_buffer_usage						!= b_debug_performance_buffer_usage_ui
				)
			{
				b_debug_performance										= b_debug_performance_ui;
				CONSTEXPR GLenum debug_type = GL_DEBUG_TYPE_PERFORMANCE;
				debug_msg_control_type( debug_type,	b_debug_performance	);

				GLuint tab[2];

				tab[0] = 131154;	//nvidia pbo message
				glDebugMessageControl( debug_src, debug_type, GL_DONT_CARE, 1, tab,	b_debug_performance_pbo_nvidia_ui ? GL_TRUE : GL_FALSE );

				b_debug_performance_pbo_nvidia	= b_debug_performance_pbo_nvidia_ui;
						
				tab[0] = 131218;	//nvidia shader recompiled due to Gl State			
				glDebugMessageControl( debug_src, debug_type, GL_DONT_CARE, 1, tab,	b_debug_performance_shader_recompiled_by_state_nvidia_ui ? GL_TRUE : GL_FALSE );
				b_debug_performance_shader_recompiled_by_state_nvidia	= b_debug_performance_shader_recompiled_by_state_nvidia_ui;

				tab[0] = 131186;	//nvidia buffer usage
				glDebugMessageControl( debug_src, debug_type, GL_DONT_CARE, 1, tab,	b_debug_performance_buffer_usage_ui ? GL_TRUE : GL_FALSE );
				b_debug_performance_buffer_usage					= b_debug_performance_buffer_usage_ui;
			}

			if(		b_debug_other								!= b_debug_other_ui
				||	b_debug_other_uniform_array_ignored_nvidia	!= b_debug_other_uniform_array_ignored_nvidia_ui
				||	b_debug_other_buffer_nvidia					!= b_debug_other_buffer_nvidia_ui
				||	b_debug_other_buffer_detailed_nvidia		!= b_debug_other_buffer_detailed_nvidia_ui
				||	b_debug_other_framebuffer_detailed_nvidia	!= b_debug_other_framebuffer_detailed_nvidia_ui
				||	b_debug_other_texture_base_level_undefined	!= b_debug_other_texture_base_level_undefined_ui
				||	b_debug_other_buffer_usage					!= b_debug_other_buffer_usage_ui
				)
			{
				b_debug_other										= b_debug_other_ui;
				CONSTEXPR GLenum debug_type = GL_DEBUG_TYPE_OTHER;
				debug_msg_control_type( debug_type,	b_debug_other );

				GLuint tab[2];
			
				tab[0] = 131217;	//nvidia uniform array ignored message
				glDebugMessageControl( debug_src, debug_type, GL_DONT_CARE, 1, tab,	b_debug_other_uniform_array_ignored_nvidia_ui ? GL_TRUE : GL_FALSE );
				b_debug_other_uniform_array_ignored_nvidia	= b_debug_other_uniform_array_ignored_nvidia_ui;

				tab[0] = 131184;	//nvidia buffer message
				glDebugMessageControl( debug_src, debug_type, GL_DONT_CARE, 1, tab,	b_debug_other_buffer_nvidia_ui ? GL_TRUE : GL_FALSE );
				b_debug_other_buffer_nvidia		= b_debug_other_buffer_nvidia_ui;

				tab[0] = 131185;	//nvidia buffer detailed message
				glDebugMessageControl( debug_src, debug_type, GL_DONT_CARE, 1, tab,	b_debug_other_buffer_detailed_nvidia_ui ? GL_TRUE : GL_FALSE );
				b_debug_other_buffer_detailed_nvidia		= b_debug_other_buffer_detailed_nvidia_ui;

				tab[0] = 131169;	//nvidia framebuffer detailed message
				glDebugMessageControl( debug_src, debug_type, GL_DONT_CARE, 1, tab,	b_debug_other_framebuffer_detailed_nvidia_ui ? GL_TRUE : GL_FALSE );
				b_debug_other_framebuffer_detailed_nvidia	= b_debug_other_framebuffer_detailed_nvidia_ui;

				tab[0] = 131204;	//nvidia texture base level undefined
				glDebugMessageControl( debug_src, debug_type, GL_DONT_CARE, 1, tab,	b_debug_other_texture_base_level_undefined_ui ? GL_TRUE : GL_FALSE );
				b_debug_other_texture_base_level_undefined	= b_debug_other_texture_base_level_undefined_ui;
	
				tab[0] = 131188;	//nvidia buffer usage texture
				glDebugMessageControl( debug_src, debug_type, GL_DONT_CARE, 1, tab,	b_debug_other_buffer_usage_ui ? GL_TRUE : GL_FALSE );
				b_debug_other_buffer_usage					= b_debug_other_buffer_usage_ui;
			}
		}
	}
}

}	//namespace GOL
