
#include "gol/gol_base.h"
#include "gol/gol_os.h"
#include "str_utils.h"
#include "err.h"

#include <lib_use.h>
#ifdef	GLEW_STATIC	//define in preprocessor
//#	if AAA_DEBUG()
//		AAA_LIB_USE( "glew32sd" )
//#	else
		AAA_LIB_USE( "glew32s" )
//#	endif
#else
//#	if AAA_DEBUG()
//		AAA_LIB_USE( "glew32d" )
//#	else
		AAA_LIB_USE( "glew32" )
//#	endif
#endif
AAA_LIB_USE_MESSAGE( "opengl32" )	//	link with Microsoft OpenGL lib
AAA_LIB_USE_MESSAGE( "glu32" )		//	link with Microsoft OpenGL Utility lib

extern "C" {
	//Turn on high power graphics for NVidia cards on laptops (with built in graphics cards + Nvidia cards)
	//	http://developer.download.nvidia.com/devzone/devcenter/gamegraphics/files/OptimusRenderingPolicies.pdf
	__declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
	//	https://gpuopen.com/amdpowerxpressrequesthighperformance/​
	__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;
}

namespace GOL	{

/*
void do_matrox_hack()
{
	static bool b_opengl32dll = false;	//<! convenience driver awakening boolean
	if( !b_opengl32dll ) 
	{
		GOL::get_integer( GL_DEPTH_BITS );
		b_opengl32dll = true;
	}
}
*/

bool	__b_state_cache_no = false;	

#define OPENGL_HEADER "# OpenGL : "
	
void	DBG_PRINT_STRING(		C_PCHAR_C fmt, ... )
{
	va_list	args;
	va_start( args, fmt );
	SET_COLOR_DBG();
	HEADER_PRINT_STRING_VA( OPENGL_HEADER,		fmt,	args );
	va_end(args);
}

void	GOOD_PRINT_STRING(		C_PCHAR_C fmt, ... )	{	va_list	args;	va_start( args, fmt );	HEADER_PRINT_STRING_VA( OPENGL_HEADER,			fmt,	args );	va_end( args );	}
void	GOOD_PRINT_STRING_1T(	C_PCHAR_C fmt, ... )	{	va_list	args;	va_start( args, fmt );	HEADER_PRINT_STRING_VA( OPENGL_HEADER "    ",	fmt,	args );	va_end( args );	}
void	GOOD_PRINT_STRING_2T(	C_PCHAR_C fmt, ... )	{	va_list	args;	va_start( args, fmt );	HEADER_PRINT_STRING_VA( OPENGL_HEADER "\t",		fmt,	args );	va_end( args );	}
void	GOOD_PRINT_STRING_3T(	C_PCHAR_C fmt, ... )	{	va_list	args;	va_start( args, fmt );	HEADER_PRINT_STRING_VA( OPENGL_HEADER "\t    ",	fmt,	args );	va_end( args );	}
void	GOOD_PRINT_STRING_4T(	C_PCHAR_C fmt, ... )	{	va_list	args;	va_start( args, fmt );	HEADER_PRINT_STRING_VA( OPENGL_HEADER "\t\t",	fmt,	args );	va_end( args );	}
void	WARNING_PRINT_STRING(	C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	WARNING_HEADER_PRINT_STRING_VA( OPENGL_HEADER, fmt, args );
	va_end(args);
}
//
//	ERR
//
void	ERR_PRINT_STRING(		C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	ERR_HEADER_PRINT_STRING_VA( OPENGL_HEADER, fmt, args );
	va_end(args);
}

#undef OPENGL_HEADER

bool	b_check_error = false;
bool	b_check_error_by_frame    = false;
bool	b_check_error_by_frame_ui = false;
bool	b_check_error_by_layer    = false;
bool	b_check_error_by_layer_ui = false;
bool	b_check_error_by_call     = false;
bool	b_check_error_by_call_ui  = false;

C_PCHAR_C	get_error_str( GLenum CONST err )
{
	switch(err)
	{
	case GL_INVALID_ENUM:					return "GL_INVALID_ENUM";					break;
	case GL_INVALID_VALUE:					return "GL_INVALID_VALUE";					break;
	case GL_INVALID_OPERATION:				return "GL_INVALID_OPERATION";				break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:	return "GL_INVALID_FRAMEBUFFER_OPERATION";	break;
	case GL_STACK_OVERFLOW:					return "GL_STACK_OVERFLOW";					break;
	case GL_STACK_UNDERFLOW:				return "GL_STACK_UNDERFLOW";				break;
	case GL_OUT_OF_MEMORY:					return "GL_OUT_OF_MEMORY";					break;
	default:								return "UNKNOWN GL ERROR";					break;
	}
}

void	print_errors_low( INT32 err, C_PCHAR_C str )
{
	INT32	same_error_max = 0;
	INT32	prev = err;
	//todo use gluErrorString ?
	do
	{
		C_PCHAR_C	where_str = get_print_where();
		C_PCHAR_C	error_str = get_error_str(err);
		if( where_str )
			ERR_PRINT_STRING( "GL Error in %s : %s : %s", where_str, str, error_str );
		else
			ERR_PRINT_STRING( "GL Error : %s : %s", str, error_str );
		err = glGetError();
		if( err != prev )
		{
			prev = err;
			same_error_max = 8;
		}
		else
		{
			if( --same_error_max <= 0 )
			{
				ERR_PRINT_STRING("GL looping on the same error, AAASeed now stop to print this error.");
				break;
			}
		}
	}
	while( err != GL_NO_ERROR );
}
//
//	UTILS
//
C_PCHAR_C get_str_from_glenum( GLenum CONST gl_enum )
{
	C_PCHAR str = nullptr;
	switch( gl_enum )
	{
	case GL_BYTE:				str = "BYTE";								break;
	case GL_UNSIGNED_BYTE:		str = "UNSIGNED_BYTE";						break;
	case GL_SHORT:				str = "SHORT";								break;
	case GL_UNSIGNED_SHORT:		str = "UNSIGNED_SHORT";						break;
	case GL_INT:				str = "INT";								break;
	case GL_UNSIGNED_INT:		str = "UNSIGNED_INT";						break;
	case GL_HALF_FLOAT:			str = "HALF_FLOAT";							break;
	case GL_FLOAT:				str = "FLOAT";								break;
	case GL_DOUBLE:				str = "DOUBLE";								break;
	case GL_2_BYTES:			str = "2_BYTES";							break;
	case GL_3_BYTES:			str = "3_BYTES";							break;
	case GL_4_BYTES:			str = "4_BYTES";							break;

	default:					str = "AAASeed don't know this GL enum";	break;
	}
	return str;
}
//
//	EXTENSION
//
bool	have_extension( C_PCHAR ext )
{
	if( !ext || *ext==0 )
		return false;

	bool b_found = false;

	if( GLEW_VERSION_3_0 )
	{
		GLint n = get_integer( GL_NUM_EXTENSIONS );
		for( GLint i = 0; i < n; ++i )
		{
			C_PCHAR one_ext = (C_PCHAR) glGetStringi( GL_EXTENSIONS, i );
			//printf("%s\n", one_ext );
			if( str_is_equal_nocase( one_ext, ext ) )
			{
				b_found = true;
				break;
			}
		}
	}
	else
	{	// GL_EXTENSIONS in glGetString depreciated in opengl 3.0
		C_PCHAR	str = (C_PCHAR) glGetString( GL_EXTENSIONS );
		if( str )
			b_found = strstr( str, ext ) != nullptr;
	}

	if( b_found )
		GOOD_PRINT_STRING(		"GL Extension found %s",		ext );
	else
		WARNING_PRINT_STRING(	"GL Extension not found %s",	ext );

	return b_found;
}

void	print_extension()
{
	GOOD_PRINT_STRING( "Extensions : " );

	if( GLEW_VERSION_3_0 )
	{
		GLint n = get_integer( GL_NUM_EXTENSIONS );
		for( GLint i = 0; i < n; ++i )
		{
			C_PCHAR ext = (C_PCHAR) glGetStringi( GL_EXTENSIONS, i );
			GOOD_PRINT_STRING( "\t\t %s", ext );
		}
	}
	else
	{	// GL_EXTENSIONS in glGetString depreciated in opengl 3.0
		C_PCHAR	str = (C_PCHAR) glGetString( GL_EXTENSIONS );
		if( str )
			//because extension can be really really really long
			PRINT_STRING_BY_BLOCK( str, 80 );
	}
}

bool test_version_or_extensions( INT32 CONST major_min, INT32 CONST minor_min, C_PCHAR_C ext_a, C_PCHAR_C ext_b, C_PCHAR_C ext_c )
{
	if( major_min > 0 )
	{
		if( CTX::get_version_major() > major_min
			|| (CTX::get_version_major() == major_min && CTX::get_version_minor() >= minor_min) )
			return true;
	}

	if( !ext_a )
	{
		WARNING_PRINT_STRING( "Need version %d.%d at minimum", major_min, minor_min );
		return false;
	}
	if( have_extension( ext_a ) )
		return true;

	if( !ext_b )
	{
		WARNING_PRINT_STRING( "Need version %d.%d at minimum or extension %s", major_min, minor_min, ext_a );
		return false;
	}
	if( have_extension( ext_b ) )
		return true;

	if( !ext_c )
	{
		WARNING_PRINT_STRING( "Need version %d.%d at minimum or extension %s or %s", major_min, minor_min, ext_a, ext_b );
		return false;
	}
	if( have_extension( ext_c ) )
		return true;

	WARNING_PRINT_STRING( "Need version %d.%d at minimum or extension %s or %s or %s", major_min, minor_min, ext_a, ext_b, ext_c );
	return false;
}

C_PCHAR_C	str_yes = "ON";
C_PCHAR_C	str_no = "OFF";
C_PCHAR_C	get_boolean_str( GLenum CONST pname )
{
	GLboolean	param;
	glGetBooleanv( pname, &param );
	return param ? str_yes : str_no ;
}

CONSTEXPR INT16 gl_hint[3] = { GL_DONT_CARE, GL_FASTEST, GL_NICEST };

INT32		get_byte_nb_from_type_enum( GLenum CONST type )
{
	switch( type )
	{
	case GL_BYTE:
	case GL_UNSIGNED_BYTE:	return 1;
	case GL_SHORT:
	case GL_UNSIGNED_SHORT:
	case GL_HALF_FLOAT:		return 2;
	case GL_INT:
	case GL_UNSIGNED_INT:
	case GL_FIXED:
	case GL_FLOAT:			return 4;
	case GL_DOUBLE:			return 8;
	default:
		debug_break( "GLenum type not supported 0x%08x", type );
		return 8;
	}
}

bool b_named_buffer_can				= false;
bool b_named_buffer_asked_ui;
bool b_named_buffer_use				= false;

bool b_direct_state_access_can		= false;
bool b_direct_state_access_asked_ui;
bool b_direct_state_access_use		= false;

INT32 buffer_nb_current		=	0;
INT32 buffer_nb_generated	=	0;

void gen_buffers( GLsizei CONST n, GLuint* CONST buffers )
{	
	glGenBuffers( n, buffers );
	buffer_nb_current += n;
	buffer_nb_generated += n;
}

INT32 const ID_INIT_VALUE = 0;
void gen_buffer( GLuint& buffer	)
{
	if( buffer != 0 )
		debug_break( "ask to generate a buffer id from a non zero value" );
	else
	{		
		gen_buffers( 1, &buffer );
		if( buffer == 0 )
		{
			debug_break( "GOL did not generates a buffer, Should not happen: GOL not reliable from here" );
			--buffer_nb_current;
			--buffer_nb_generated;
		}
	}
}

	
void delete_buffers( GLsizei n, GLuint* buffers )
{
	glDeleteBuffers( n, buffers );
	buffer_nb_current -= n;
	--buffers;
	for( ; n>0; --n )
		*++buffers = 0;
}

void delete_buffer_private( GLuint& buffer )
{
	glDeleteBuffers( 1, &buffer );	
	buffer = 0;
	--buffer_nb_current;
}

}	//namespace GOL
