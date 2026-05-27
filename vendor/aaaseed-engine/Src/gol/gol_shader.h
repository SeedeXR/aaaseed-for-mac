
#ifdef AAA_GOL_SHADER_H
#error "GOL_SHADER_H included more than once."
#endif
#define AAA_GOL_SHADER_H 1

#ifndef AAA_GOL_BASE_H
#	include "gol_base.h"
#endif

namespace GOL	{

extern	bool	b_shaders_can;
extern	bool	b_shaders_allow;
extern	bool	b_shaders_use;

extern	bool	b_shaders_include_can;
extern	bool	b_shaders_include_allow;
extern	bool	b_shaders_include_use;

extern	UINT32	program_used;

extern	void	init_shaders();

extern	UINT32	create_shader(			GLenum CONST type );
extern	void	delete_shader(			UINT32 CONST shader		);
extern	void	shader_source(			UINT32 CONST shader,	GLsizei CONST count, GLchar CONST * CONST * string, GLint CONST * CONST length );
extern	void	compile_shader(			UINT32 CONST shader		);
extern	void	get_shader_iv(			UINT32 CONST shader,	GLenum CONST pname, GLint* CONST param );

extern	UINT32	create_program();
extern	void	delete_program(			UINT32 CONST program );
extern	void	link_program(			UINT32 CONST program );
extern	void	validate_program(		UINT32 CONST program );

FINLINE UINT32	get_program_used()									{	return program_used;				}
FINLINE void	use_program(			UINT32 CONST program )
{
	if( program_used != program )
	{
		//DBG_PRINT_STRING( "program %d to %d", program_cur, program );
		glUseProgram( program );
		program_used = program;
	}
}

FINLINE	void	attach_shader(			UINT32 CONST program, UINT32 CONST shader )						{	glAttachShader( program, shader );	}
FINLINE	void	detach_shader(			UINT32 CONST program, UINT32 CONST shader )						{	glDetachShader( program, shader );	}
FINLINE	INT32	get_uniform_location(	UINT32 CONST program, GLchar CONST * CONST name )				{	return glGetUniformLocation( program, name );	}
FINLINE	INT32	get_attrib_location(	UINT32 CONST program, GLchar CONST * CONST name )				{	return glGetAttribLocation( program, name );	}
FINLINE	void	program_parameter(		UINT32 CONST program, GLenum CONST pname, INT32 CONST value )	{	glProgramParameteri( program, pname, value );	}

extern	void	get_program_iv(			UINT32 CONST program, GLenum CONST pname, GLint * CONST param );
extern	void	get_program_infolog(	UINT32 CONST program, GLsizei CONST buf_size, GLsizei* CONST len, GLchar* CONST infolog );
extern	void	print_info_log(			bool CONST b_program, GLuint CONST gl_handle, bool CONST b_error, C_PCHAR_C start = nullptr );


// Uniform
FINLINE void	set_uniform_1i(										UINT32 CONST location, INT32   CONST v1 )									{	glUniform1i(  location, v1 );				}
FINLINE void	set_uniform_1u(										UINT32 CONST location, UINT32  CONST v1 )									{	glUniform1ui( location, v1 );				}
FINLINE void	set_uniform_1iv(									UINT32 CONST location, INT32   CONST count, GLint CONST * CONST pvalue )	{	glUniform1iv( location, count, pvalue );	}

FINLINE void	set_uniform_1f(										UINT32 CONST location, GLfloat CONST v1 )									{	glUniform1f(  location, v1 );				}
FINLINE void	set_uniform_2f(										UINT32 CONST location, GLfloat CONST v1, GLfloat CONST v2 )					{	glUniform2f(  location, v1, v2 );			}
FINLINE void	set_uniform_3f(										UINT32 CONST location, GLfloat CONST v1, GLfloat CONST v2, GLfloat CONST v3 )
																																				{	glUniform3f(  location, v1, v2, v3 );		}
FINLINE void	set_uniform_4f(										UINT32 CONST location, GLfloat CONST v1, GLfloat CONST v2, GLfloat CONST v3, GLfloat CONST v4 )
																																				{	glUniform4f(  location, v1, v2, v3, v4 );	}

FINLINE void	set_uniform_1fv(									UINT32 CONST location, INT32   CONST count, GLfloat CONST * CONST pvalue )	{	glUniform1fv( location, count, pvalue );	}	
FINLINE void	set_uniform_2fv(									UINT32 CONST location, INT32   CONST count, GLfloat CONST * CONST pvalue )	{	glUniform2fv( location, count, pvalue );	}	
FINLINE void	set_uniform_3fv(									UINT32 CONST location, INT32   CONST count, GLfloat CONST * CONST pvalue )	{	glUniform3fv( location, count, pvalue );	}	
FINLINE void	set_uniform_4fv(									UINT32 CONST location, INT32   CONST count, GLfloat CONST * CONST pvalue )	{	glUniform4fv( location, count, pvalue );	}	

FINLINE void	set_program_uniform_1i(		UINT32 CONST program,	UINT32 CONST location, INT32   CONST v1 )									{	glProgramUniform1i(  program, location, v1 );				}
FINLINE void	set_program_uniform_1u(		UINT32 CONST program,	UINT32 CONST location, UINT32  CONST v1 )									{	glProgramUniform1ui( program, location, v1 );				}
FINLINE void	set_program_uniform_1iv(	UINT32 CONST program,	UINT32 CONST location, INT32   CONST count, GLint CONST * CONST pvalue )	{	glProgramUniform1iv( program, location, count, pvalue );	}

FINLINE void	set_program_uniform_1f(		UINT32 CONST program,	UINT32 CONST location, GLfloat CONST v1 )									{	glProgramUniform1f(  program, location, v1 );				}
FINLINE void	set_program_uniform_2f(		UINT32 CONST program,	UINT32 CONST location, GLfloat CONST v1, GLfloat CONST v2 )					{	glProgramUniform2f(  program, location, v1, v2 );			}
FINLINE void	set_program_uniform_1fv(	UINT32 CONST program,	UINT32 CONST location, INT32   CONST count, GLfloat CONST * CONST pvalue )	{	glProgramUniform1fv( program, location, count, pvalue );	}	
FINLINE void	set_program_uniform_2fv(	UINT32 CONST program,	UINT32 CONST location, INT32   CONST count, GLfloat CONST * CONST pvalue )	{	glProgramUniform2fv( program, location, count, pvalue );	}	
FINLINE void	set_program_uniform_3fv(	UINT32 CONST program,	UINT32 CONST location, INT32   CONST count, GLfloat CONST * CONST pvalue )	{	glProgramUniform3fv( program, location, count, pvalue );	}	
FINLINE void	set_program_uniform_4fv(	UINT32 CONST program,	UINT32 CONST location, INT32   CONST count, GLfloat CONST * CONST pvalue )	{	glProgramUniform4fv( program, location, count, pvalue );	}	


// Geometry
extern	bool	b_shaders_geometry_can;
extern	bool	b_shaders_geometry_allow;
extern	bool	b_shaders_geometry_use;
extern	INT32	shaders_geometry_vertices_max_nb;
extern	INT32	shaders_geometry_components_max_nb;

// Compute
extern	bool	b_shaders_compute_can;
extern	bool	b_shaders_compute_allow;
extern	bool	b_shaders_compute_use;
extern	INT32	shaders_compute_work_group_count_max[3];
extern	INT32	shaders_compute_work_group_size_max[3];

FINLINE void	dispatch_compute(				UINT32 CONST workgroup_nb_x,	UINT32 CONST workgroup_nb_y,	UINT32 CONST workgroup_nb_z )
{
	glDispatchCompute( workgroup_nb_x, workgroup_nb_y, workgroup_nb_z );
}
FINLINE void	dispatch_compute_indirect(		INT32 CONST offset )
{
	glDispatchComputeIndirect( (GLintptr) offset );
}
// ok but we need to check extension GL_ARB_compute_variable_group_size
//FINLINE void	dispatch_compute_group_size(	UINT32 CONST workgroup_nb_x,	UINT32 CONST workgroup_nb_y,	UINT32 CONST workgroup_nb_z,
//												UINT32 CONST group_nb_x,		UINT32 CONST group_nb_y,		UINT32 CONST group_nb_z )
//{																
//	glDispatchComputeGroupSizeARB(	workgroup_nb_x,	workgroup_nb_y,	workgroup_nb_z,
//									group_nb_x,		group_nb_y,		group_nb_z		);
//}

FINLINE void	dispatch_compute_1d( UINT32 CONST workgroup_nb_x, UINT32 CONST group_size_x )
{
	INT32 tm1 = group_size_x - 1;
	INT32 nb = (workgroup_nb_x + tm1) / group_size_x;
	glDispatchCompute( nb, 1, 1 );
}

FINLINE void	dispatch_compute_2d( UINT32 CONST workgroup_nb_x, UINT32 CONST group_size_x,
										UINT32 CONST workgroup_nb_y, UINT32 CONST group_size_y)
{
	INT32 tm1x = group_size_x - 1;
	INT32 nbx = (workgroup_nb_x + tm1x) / group_size_x;

	INT32 tm1y = group_size_y - 1;
	INT32 nby = (workgroup_nb_y + tm1y) / group_size_y;
	glDispatchCompute( nbx, nby, 1 );
}

}
