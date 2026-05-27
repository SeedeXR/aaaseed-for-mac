#include "gol/gol_shader.h"
#include "aaa_os.h"
#include "err.h"

namespace GOL	{

bool	b_shaders_can				= false;
bool	b_shaders_allow				= true;
bool	b_shaders_use				= false;

bool	b_shaders_include_can		= false;
bool	b_shaders_include_allow		= false;
bool	b_shaders_include_use		= false;

bool	b_shaders_geometry_can		= false;
bool	b_shaders_geometry_allow	= false;
bool	b_shaders_geometry_use		= false;
INT32	shaders_geometry_vertices_max_nb	= 0;
INT32	shaders_geometry_components_max_nb	= 0;
	
bool	b_shaders_compute_can		= false;
bool	b_shaders_compute_allow		= false;
bool	b_shaders_compute_use		= false;
INT32	shaders_compute_work_group_count_max[3]	= {0};
INT32	shaders_compute_work_group_size_max[3]	= {0};

UINT32	program_used = 0;

void	init_shaders()
{
	b_shaders_include_can = false;

	b_shaders_geometry_can = false;
	b_shaders_compute_can = false;

	b_shaders_can = test_version_or_extensions( 2,0 ); // not 100% sure on this one
	if( !b_shaders_can )
	{
		b_shaders_can = true;
		C_PCHAR_C shader_ext[] = { "GL_ARB_shader_objects", "GL_ARB_vertex_shader", "GL_ARB_fragment_shader", "GL_ARB_vertex_program", "GL_ARB_fragment_program", nullptr };
		for( C_PCHAR_C* hd = shader_ext; *hd != nullptr; ++hd )
		{
			if( !have_extension( *hd ) )
			{
				WARNING_PRINT_STRING( "can't use shader because a missing OpenGL extension" );
				WARNING_PRINT_STRING( "\t%s", *hd );
				b_shaders_can = false;
			}
		}
	}
	if( b_shaders_can )
		GOOD_PRINT_STRING( "Shader allowed" );
	else
		return;

	if( test_version_or_extensions( 3,2, "GL_ARB_geometry_shader4", "GL_EXT_geometry_shader4") )
	{
		b_shaders_geometry_can = true;
		shaders_geometry_vertices_max_nb = get_integer( GL_MAX_GEOMETRY_OUTPUT_VERTICES );
		shaders_geometry_components_max_nb = get_integer( GL_MAX_GEOMETRY_TOTAL_OUTPUT_COMPONENTS );
		GOL::enable( GL_PROGRAM_POINT_SIZE );	//	necessary for vertex shader to change point size
		GOOD_PRINT_STRING( "Geometry Shader allowed" );
	}
	else
		WARNING_PRINT_STRING( "No Geometry Shader" );

	if( test_version_or_extensions( 0,0, "GL_ARB_shading_language_include" ) )
	{
		WGL_GET_PROC( glNamedStringARB );	//don't need it with glew ?
		if( glNamedStringARB )
		{
			GOOD_PRINT_STRING( "Shader include allowed" );
			b_shaders_include_can = true;
		}
		else
			ERR_PRINT_STRING( "Can't find glNamedStringARB(), so can't use Shader include" );
	}
	else
	{
		WARNING_PRINT_STRING( "No shader Include" );
	}

	if( test_version_or_extensions( 4,3, "GL_ARB_compute_shader") )
	{
		//todo need to refine
		b_shaders_compute_can = true;
		test_version_or_extensions( 4,3, "GL_ARB_shader_storage_buffer_object" );	//ssbo ?

		get_integer_by_index( GL_MAX_COMPUTE_WORK_GROUP_COUNT, shaders_compute_work_group_count_max, 3 );
		GOOD_PRINT_STRING( "  WorkGroup Count         %d x %d x %d",
								shaders_compute_work_group_count_max[0],
								shaders_compute_work_group_count_max[1],
								shaders_compute_work_group_count_max[2]	);

		get_integer_by_index( GL_MAX_COMPUTE_WORK_GROUP_SIZE, shaders_compute_work_group_size_max, 3 );
		GOOD_PRINT_STRING( "Compute :");
		GOOD_PRINT_STRING( "  WorkGroup Size          %d x %d x %d",
								shaders_compute_work_group_size_max[0],
								shaders_compute_work_group_size_max[1],
								shaders_compute_work_group_size_max[2]	);
		GOOD_PRINT_STRING( "  WorkGroup Invocations   %d",	get_integer( GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS ) );
//			number of invocations in a single local work group (i.e., the product of the three dimensions) that may be dispatched to a compute shader.

		GOOD_PRINT_STRING( "  uniform block           %d",	get_integer( GL_MAX_COMPUTE_UNIFORM_BLOCKS )		);
		GOOD_PRINT_STRING( "  Share Memory size       %d",	get_integer( GL_MAX_COMPUTE_SHARED_MEMORY_SIZE )	);
		GOOD_PRINT_STRING( "  texture Unit            %d",	get_integer( GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS )	);
		GOOD_PRINT_STRING( "  uniform componemt       %d",	get_integer( GL_MAX_COMPUTE_UNIFORM_COMPONENTS )	);
		GOOD_PRINT_STRING( "  atomic counters         %d",	get_integer( GL_MAX_COMPUTE_ATOMIC_COUNTERS )		);
		GOOD_PRINT_STRING( "  atomic counters buffers %d",	get_integer( GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS ));
	}
	else
		WARNING_PRINT_STRING( "No Compute Shader" );
}



UINT32	create_program()											{	return glCreateProgram();			}
void	delete_program(			UINT32 CONST program )				{	glDeleteProgram( program );			}
void	link_program(			UINT32 CONST program )				{	glLinkProgram( program );			}
void	validate_program(		UINT32 CONST program )				{	glValidateProgram( program );		}


UINT32	create_shader(			GLenum CONST type )					{	return glCreateShader( type );		}
void	delete_shader(			UINT32 CONST shader )				{	glDeleteShader( shader );			}

void	shader_source(			UINT32 CONST shader, GLsizei CONST count, GLchar CONST * CONST * string, GLint CONST * CONST length )
{
	glShaderSource(	shader, count, string, length );
}
void	compile_shader(			UINT32 CONST shader )
{
	if( b_shaders_include_use )
	{
		GLchar CONST * CONST	path = "/";
		glCompileShaderIncludeARB( shader, 1, &path, nullptr );
	}
	else
		glCompileShader( shader );
}

void	get_shader_iv(			UINT32 CONST shader, GLenum CONST pname, GLint* CONST param )
{
	glGetShaderiv(		shader, pname, param );
}
void	get_shader_infolog(		UINT32 CONST shader, GLsizei CONST buf_size, GLsizei* CONST len, GLchar* CONST infolog )
{
	glGetShaderInfoLog( shader, buf_size, len, infolog );
}

void	get_program_iv(			UINT32 CONST program, GLenum CONST pname, GLint * CONST param )
{
	glGetProgramiv( program, pname, param );
}
void	get_program_infolog(	UINT32 CONST program, GLsizei CONST buf_size, GLsizei* CONST len, GLchar* CONST infolog )
{
	glGetProgramInfoLog( program, buf_size, len, infolog );
}

namespace
{
	GLchar 	info_log[ 1024 * 8 ];
}

void	print_info_log(			bool CONST b_program, GLuint CONST gl_handle, bool CONST b_error, C_PCHAR_C start )
{
	GLsizei	info_log_length = 0;
	*info_log = 0;
//	glGetShader with the value GL_INFO_LOG_LENGTH
	// Print out the info log
	if( b_program )
		get_program_infolog( gl_handle, sizeof(info_log), &info_log_length, info_log );
	else
		get_shader_infolog(  gl_handle, sizeof(info_log), &info_log_length, info_log );

	GOL::get_error( "Info log" );
	C_PCHAR_C what = b_program ? "program" : "shader";
		
	if( info_log_length > 0 )
	{
		C_PCHAR header = "shader %s %s : ";
		if( start )
		{
			if( b_error )
				ERR_PRINT_STRING( header, what, start );
			else
				DBG_PRINT_STRING( header, what, start );
		}
		//DBG_PRINT_STRING( "info_log len %d : %s", info_log_length, info_log );
		CHAR*	mark = info_log;
		CHAR*	cur = mark-1;
		//todo double security stopping with info_log_length too
		header = "\t%s ";
		for( ; ; )
		{
			if( *++cur < 32 )
			{
				bool b_break = (*cur==0);
				*cur = 0;
				if( mark != cur )
				{
					if( b_error )
						ERR_PRINT_STRING( header, mark );
					else
						DBG_PRINT_STRING( header, mark );
				}
				if( b_break )
					break;
				mark = cur + 1;
			}
		}
	}
	else
	{
		C_PCHAR header = "%s %s";
		if( start )
			GOOD_PRINT_STRING( header, what, start );
	}
}

}	//namespace GOL
