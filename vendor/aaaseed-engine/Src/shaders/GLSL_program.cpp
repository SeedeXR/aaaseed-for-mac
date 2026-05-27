#include "GLSL_program.h"
#include "GLSL_shader.h"
#include "GLSL_uniforms.h"
#include "gol/gol.h"
#include "shader_master.h"


#if AAA_DEBUG()
namespace {
	bool	b_check_all = false;	//maa was useful to check intel driver crashs
	FINLINE void DBG_CHECK_ALL( C_PCHAR_C str )
	{
		if( b_check_all )
			GOL::check_error( "attach vertex" );
	}
}
#else
#	define DBG_CHECK_ALL( str )
#endif

INT32	c_program_glsl::get_attribute_location( C_PCHAR_C name )
{
	if( _program_id )
	{
		//glUseProgramObjectARB( _program );

		/*		char *name2;
		GLint active_attribs, max_length;

		glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTES, &active_attribs);
		glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &max_length);

		name2 = (CHAR*)MALLOC(max_length + 1);

		for (unsigned i = 0; i < active_attribs; ++i ) {
		GLint size;
		GLenum type;

		glGetActiveAttrib(_program, i, max_length + 1, nullptr,
		&size, &type, name2);
		printf("%s is at location %d\n", name2, glGetAttribLocation(_program, name2));
		}
		FREE(name2);
		//GLint nb;
		//glGetProgramiv( _program, GL_ACTIVE_ATTRIBUTES, &nb );
		*/
		//glGetActiveAttrib( _program, 0, length,)
		return GOL::get_attrib_location( _program_id, name );
	}
	return -1;
}

c_program_glsl::c_program_glsl()
	:_program_id	(0)
	,_b_valid		(false)
{
}
c_program_render_glsl::c_program_render_glsl()
	:_p_vertex_shader	{nullptr}
	,_p_geometry_shader	{nullptr}
	,_p_fragment_shader	{nullptr}
{
	_uniforms_glsl = new c_uniforms_render_glsl;
}
c_program_compute_glsl::c_program_compute_glsl()
	:_p_compute_shader	{nullptr}
{
	_uniforms_glsl = new c_uniforms_compute_glsl;
}

c_program_glsl::~c_program_glsl()
{
	if( _program_id )
	{
		GOL::delete_program( _program_id );
		_program_id = 0;
	}
}
c_program_render_glsl::~c_program_render_glsl()
{
	detach_shaders();
	delete _uniforms_glsl;
}
c_program_compute_glsl::~c_program_compute_glsl()
{
	detach_shader();
	delete _uniforms_glsl;
}

INT32	c_program_glsl::get_program_always()
{
	if( _program_id != 0 )
		return _program_id;
	_program_id = GOL::create_program();
	if( _program_id == 0 )
		// error creating the program object
		GOL::ERR_PRINT_STRING( "%s() : could not create shader program object", __FUNCTION__ );
	return _program_id;
}

void	c_program_render_glsl::bind()
{
	use_program();
	if( _b_bind_first )
	{
		//we do it here as an optimization because it is a static array 1 2 3 4
		get_glsl_uniforms()->init_aaatex_and_samplers();
		_b_bind_first = false;
	}
#ifdef	SHADER_SCREEN_DEFORMATION
	shading->update_deformation();
#endif
}
void	c_program_compute_glsl::bind()
{
	use_program();
	if( _b_bind_first )
	{
		//we do it here as an optimization because it is a static array 1 2 3 4
		get_glsl_uniforms()->init_aaatex_and_samplers();
		_b_bind_first = false;
	}
}

void	c_program_render_glsl::detach_shaders()
{
	// if shaders are already attach to program, detach it
	auto program_id = get_program_id();
	if( _p_vertex_shader )
	{
		GOL::detach_shader( program_id, _p_vertex_shader->get_gl_shader() );
		_p_vertex_shader = nullptr;
	}
	if( _p_fragment_shader )
	{
		GOL::detach_shader( program_id, _p_fragment_shader->get_gl_shader() );
		_p_fragment_shader = nullptr;
	}
	if( _p_geometry_shader )
	{
		GOL::detach_shader( program_id, _p_geometry_shader->get_gl_shader() );
		_p_geometry_shader = nullptr;
	}
}

void	c_program_compute_glsl::detach_shader()
{
	if( _p_compute_shader )
	{
		GOL::detach_shader( get_program_id(), _p_compute_shader->get_gl_shader() );
		_p_compute_shader = nullptr;
	}
}

bool	c_program_render_glsl::attach_shaders( c_shader CONST * CONST vertex_shader, c_shader CONST * CONST fragment_shader, c_shader CONST * CONST geometry_shader )
{
	_b_valid = false;

	// We need the program
	auto program_id = get_program_always();
	if( program_id == 0 )
		return _b_valid;

	// Attach the shaders to the program
	_p_vertex_shader = (c_shader_glsl*)vertex_shader;
	GOL::attach_shader( program_id, _p_vertex_shader->get_gl_shader() );
	DBG_CHECK_ALL( "attach vertex" );

	_p_fragment_shader = (c_shader_glsl*)fragment_shader;
	GOL::attach_shader( program_id, _p_fragment_shader->get_gl_shader() );
	DBG_CHECK_ALL( "attach fragment" );

	if( geometry_shader )
	{
		_p_geometry_shader = (c_shader_glsl*)geometry_shader;
		GOL::attach_shader( program_id, _p_geometry_shader->get_gl_shader() );
		DBG_CHECK_ALL( "attach geometry" );

		// setting input vertices type
		GLenum	geometry_in;
		switch( _geometry_in )
		{
		case c_program_render_glsl::GSI_POINTS				:	geometry_in = GL_POINTS;				break;
		case c_program_render_glsl::GSI_LINES				:	geometry_in = GL_LINES;					break;
		case c_program_render_glsl::GSI_LINES_ADJACENCY		:	geometry_in = GL_LINES_ADJACENCY;		break;
		case c_program_render_glsl::GSI_TRIANGLES			:	geometry_in = GL_TRIANGLES;				break;
		case c_program_render_glsl::GSI_TRIANGLES_ADJACENCY	:	geometry_in = GL_TRIANGLES_ADJACENCY;	break;
		default:
			err_print( "GLSL program : unknown geometry input type" );
			goto exit;
		}
		GOL::program_parameter( program_id, GL_GEOMETRY_INPUT_TYPE_ARB , geometry_in );
		DBG_CHECK_ALL( "geometry in param" );

		// setting output vertices type
		GLenum	geometry_out;
		switch( _geometry_out )
		{
		case c_program_render_glsl::GSO_POINTS				:	geometry_out = GL_POINTS;				break;
		case c_program_render_glsl::GSO_LINE_STRIP			:	geometry_out = GL_LINE_STRIP;			break;
		case c_program_render_glsl::GSO_TRIANGLE_STRIP		:	geometry_out = GL_TRIANGLE_STRIP;		break;
		default:
			err_print( "GLSL program : unknown geometry output type" );
			goto exit;
		}
		GOL::program_parameter( program_id, GL_GEOMETRY_OUTPUT_TYPE_ARB, geometry_out );
			
		INT32	nb = MIN( GOL::shaders_geometry_vertices_max_nb, _vertices_nb );
		GOL::program_parameter( program_id, GL_GEOMETRY_VERTICES_OUT_ARB, nb );
		DBG_CHECK_ALL( "geometry out param" );
	}

	// Link the shaders in the program
	GOL::link_program( program_id );
	DBG_CHECK_ALL( "link_program" );

	GLint	b_linked;
	GOL::get_program_iv( program_id, GL_LINK_STATUS, &b_linked );

	// Print out the info log
	if( !b_linked )
	{
		if( _p_geometry_shader )
		{
			err_print( "GLSL link problem. in %s / %s / %s",	_p_vertex_shader->get_shader_fpath().get(),
																	_p_geometry_shader->get_shader_fpath().get(),
																	_p_fragment_shader->get_shader_fpath().get()
																	);											
		}
		else
		{
			err_print( "GLSL link problem. in %s / %s",		_p_vertex_shader->get_shader_fpath().get(),
																	_p_fragment_shader->get_shader_fpath().get()
																	);	
		}
	}

	GOL::print_info_log( true,  program_id, !b_linked, b_linked ? "linked" : "could not link program :" );
	if( !b_linked )
	{
		goto exit;
	}
	_b_valid = true;

	if( g_shader_master->is_do_validate() )
	{
		// validate program
		GOL::validate_program( program_id );
		GLint b_validated;
		GOL::get_program_iv( program_id , GL_VALIDATE_STATUS, &b_validated );
		GOL::print_info_log( true, program_id, !b_validated, b_validated ? "validated" : "could not validate program : " );
	}
exit:
	// if shaders are already attach to program, detach it
	detach_shaders();
	return _b_valid;	//retcode not used for now
}

bool	c_program_compute_glsl::attach_shader( c_shader CONST * CONST compute_shader )
{
	_b_valid = false;
	// We need the program
	auto program_id = get_program_always();
	if( program_id == 0 )
		return _b_valid;

	// Attach the shader to the program
	_p_compute_shader = (c_shader_glsl*)compute_shader;
	GOL::attach_shader( program_id, _p_compute_shader->get_gl_shader() );
	DBG_CHECK_ALL( "attach compute" );

	//todo deal with compute particularities

	// Link the shader in the program
	GOL::link_program( program_id );
	DBG_CHECK_ALL( "link_program" );

	GLint	b_linked;
	GOL::get_program_iv( program_id, GL_LINK_STATUS, &b_linked );

	// Print out the info log
	if( !b_linked )
		err_print( "GLSL link problem. in %s",	_p_compute_shader->get_shader_fpath().get()	);											

	GOL::print_info_log( true,  program_id, !b_linked, b_linked ? "linked" : "could not link program :" );
	if( !b_linked )
		goto exit;

	_b_valid = true;

	if( g_shader_master->is_do_validate() )
	{
		// validate program
		GOL::validate_program( program_id );
		GLint b_validated;
		GOL::get_program_iv( program_id , GL_VALIDATE_STATUS, &b_validated );
		GOL::print_info_log( true, program_id, !b_validated, b_validated ? "validated" : "could not validate program : " );
	}

exit:
	// if shaders are already attach to program, detach it
	detach_shader();
	return _b_valid;	//retcode not used for now
}


bool	c_program_render_glsl::update_shaders( c_shader CONST * CONST vertex_shader, c_shader CONST * CONST fragment_shader, c_shader CONST * CONST geometry_shader )
{
	// if shaders are already attach to program, detach it
	detach_shaders();
	// attach new shaders to program
	//but intel crashed here
	attach_shaders( vertex_shader, fragment_shader, geometry_shader );

	if( _b_valid )
	{
		_uniforms_glsl->init( get_program_id() );
		_b_bind_first = true; 
	}
	return _b_valid;
}

bool	c_program_compute_glsl::update_shader( c_shader CONST * CONST compute_shader )
{
	// if shaders are already attach to program, detach it
	detach_shader();
	// attach new shaders to program
	//but intel crashed here
	attach_shader( compute_shader );

	if( _b_valid )
	{
		_uniforms_glsl->init( get_program_id() );
		_b_bind_first = true; 
	}
	return _b_valid;
}

//
//c_glsl_program::operator GLuint () CONST
//{
//	return _program;
//}

void	c_program_render_glsl::set_geometry( GEOMETRY_SHADER_INPUT CONST geo_in, GEOMETRY_SHADER_OUTPUT CONST geo_out, INT32 CONST nb )	
{
	_geometry_in	= geo_in;
	_geometry_out	= geo_out;
	_vertices_nb	= nb;
}


 