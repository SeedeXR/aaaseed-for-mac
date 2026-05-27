// Unused in AAASeed
// Main header
#include "gl/shader.h"


#if !defined(AAA_GOL_SHADER_H)
#include "gol_shader.h"
#endif

#ifndef AAA_AAA_MEM_H
#include "code_utils/aaa_mem.h"
#endif

C_PCHAR_C	gl::shader::get_type_name() { return "shader"; }

gl::shader * gl::shader::make(
	gl::shader_props const &	p_props, 
	bool const					p_immediate)
{
	gl::shader * res = new gl::shader();
	res->init(p_props);
	res->request_init_ogl(p_immediate);
	return res;
}
void gl::shader::reclaim(
	gl::shader *				p_ptr, 
	bool const					p_immediate)
{
	p_ptr->request_release(p_immediate);
}

void gl::shader::init( gl::shader_props const & p_props )
{
	_pipeline = p_props._pipeline;
	MEMCPY( _prgms, p_props._prgms, 5 * sizeof(char const*), __FUNCTION__ );
}

bool gl::shader::is_compiled(uint32_t const p_shader_id)
{
	GLint res = 0;
	GOL::get_shader_iv( p_shader_id, GL_COMPILE_STATUS, &res );
	if( res == GL_FALSE )
	{
		GLint len = 0;
		GOL::get_shader_iv( p_shader_id, GL_INFO_LOG_LENGTH, &len );
		//todo avoid alloc / free 
		char * log = (char*)MALLOC( len );
		glGetShaderInfoLog(p_shader_id, len, &len, &log[0]);

		GOL::ERR_PRINT_STRING("Shader build failed: \n%s", log);
		FREE_AND_NULL( log );
	}
	return (res == GL_TRUE);
}

void gl::shader::init_compute()
{
	int const idx = 0;
	_prgm_ids[idx] = glCreateShader(GL_COMPUTE_SHADER);
	glShaderSource(_prgm_ids[idx], 1, &_prgms[idx], NULL);
	glCompileShader(_prgm_ids[idx]);
	if (is_compiled(_prgm_ids[idx]))
	{
		glAttachShader(_id, _prgm_ids[idx]);
		_valid = true;
	}
}
void gl::shader::init_pixel()
{
	int idx      = 0;
	int compiled = 0;

	_prgm_ids[idx] = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(_prgm_ids[idx], 1, &_prgms[idx], NULL);
	glCompileShader(_prgm_ids[idx]);
	if( is_compiled(_prgm_ids[idx]) )
	{
		glAttachShader(_id, _prgm_ids[idx]);
		++compiled;
	}
	++idx;

	_prgm_ids[idx] = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(_prgm_ids[idx], 1, &_prgms[idx], NULL);
	glCompileShader(_prgm_ids[idx]);
	if( is_compiled(_prgm_ids[idx]) )
	{
		glAttachShader(_id, _prgm_ids[idx]);
		++compiled;
	}

	if( compiled == 2 )
	{
		_valid = true;
	}
}
void gl::shader::init_geometry()
{
	int idx      = 0;
	int compiled = 0;

	_prgm_ids[idx] = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(_prgm_ids[idx], 1, &_prgms[idx], NULL);
	glCompileShader(_prgm_ids[idx]);
	if( is_compiled(_prgm_ids[idx]) )
	{
		glAttachShader(_id, _prgm_ids[idx]);
		++compiled;
	}
	++idx;

	_prgm_ids[idx] = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(_prgm_ids[idx], 1, &_prgms[idx], NULL);
	glCompileShader(_prgm_ids[idx]);
	if( is_compiled(_prgm_ids[idx]) )
	{
		glAttachShader(_id, _prgm_ids[idx]);
		++compiled;
	}
	++idx;

	_prgm_ids[idx] = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(_prgm_ids[idx], 1, &_prgms[idx], NULL);
	glCompileShader(_prgm_ids[idx]);
	if( is_compiled(_prgm_ids[idx]) )
	{
		glAttachShader(_id, _prgm_ids[idx]);
		++compiled;
	}

	if( compiled == 3 )
	{
		_valid = true;
	}
}
void gl::shader::init_tessellation()
{
	int idx      = 0;
	int compiled = 0;

	_prgm_ids[idx] = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(_prgm_ids[idx], 1, &_prgms[idx], NULL);
	glCompileShader(_prgm_ids[idx]);
	if( is_compiled(_prgm_ids[idx]) )
	{
		glAttachShader(_id, _prgm_ids[idx]);
		++compiled;
	}
	++idx;

	_prgm_ids[idx] = glCreateShader(GL_TESS_CONTROL_SHADER);
	glShaderSource(_prgm_ids[idx], 1, &_prgms[idx], NULL);
	glCompileShader(_prgm_ids[idx]);
	if( is_compiled(_prgm_ids[idx]) )
	{
		glAttachShader(_id, _prgm_ids[idx]);
		++compiled;
	}
	++idx;

	_prgm_ids[idx] = glCreateShader(GL_TESS_EVALUATION_SHADER);
	glShaderSource(_prgm_ids[idx], 1, &_prgms[idx], NULL);
	glCompileShader(_prgm_ids[idx]);
	if( is_compiled(_prgm_ids[idx]) )
	{
		glAttachShader(_id, _prgm_ids[idx]);
		++compiled;
	}
	++idx;

	_prgm_ids[idx] = glCreateShader(GL_GEOMETRY_SHADER);
	glShaderSource(_prgm_ids[idx], 1, &_prgms[idx], NULL);
	glCompileShader(_prgm_ids[idx]);
	if( is_compiled(_prgm_ids[idx]) )
	{
		glAttachShader(_id, _prgm_ids[idx]);
		++compiled;
	}
	++idx;

	_prgm_ids[idx] = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(_prgm_ids[idx], 1, &_prgms[idx], NULL);
	glCompileShader(_prgm_ids[idx]);
	if( is_compiled(_prgm_ids[idx]) )
	{
		glAttachShader(_id, _prgm_ids[idx]);
		++compiled;
	}
	++idx;

	if( compiled == 5 )
	{
		_valid = true;
	}
}
void gl::shader::init_ogl()
{
	_id = glCreateProgram();
	switch( _pipeline )
	{
	case gl::shader_pipeline_e::compute:        init_compute();      break;
	case gl::shader_pipeline_e::pixel:          init_pixel();        break;
	case gl::shader_pipeline_e::geometry:       init_geometry();     break;
	case gl::shader_pipeline_e::tessellation:   init_tessellation(); break;
	}

	if( is_valid() ) 
	{
		glLinkProgram( _id );
	}
}
void gl::shader::update_ogl()
{
	release_ogl();
	init_ogl();
}
void gl::shader::release_ogl()
{
	int num = 0;
	switch( _pipeline )
	{
	case gl::shader_pipeline_e::compute:        num = 1; break;
	case gl::shader_pipeline_e::pixel:          num = 2; break;
	case gl::shader_pipeline_e::geometry:       num = 3; break;
	case gl::shader_pipeline_e::tessellation:   num = 5; break;
	}

	for( int i = 0; i < num; ++i )
		glDeleteShader(_prgm_ids[i]);

	glDeleteProgram(_id);
	_valid = false;
}

void gl::shader::bind() const
{
	glUseProgram( _id );
}
void gl::shader::unbind() const
{
	glUseProgram( 0 );
}
//void gl::shader::dispatch(	uint32_t const p_num_group_x, uint32_t const p_num_group_y, uint32_t const p_num_group_z	) const
//{
//	glUseProgram( _id );
//	glDispatchCompute( p_num_group_x, p_num_group_y, p_num_group_z );
//}

void gl::shader::update(
	GLchar const *	p_prgm1, 
	bool const		p_immediate)
{
	_prgms[0] = p_prgm1;
	request_update_ogl( p_immediate );
}
void gl::shader::update(
	GLchar const *	p_prgm1, 
	GLchar const *	p_prgm2, 
	bool const		p_immediate)
{
	_prgms[0] = p_prgm1;
	_prgms[1] = p_prgm2;
	request_update_ogl( p_immediate );
}
void gl::shader::update(
	GLchar const *	p_prgm1, 
	GLchar const *	p_prgm2, 
	GLchar const *	p_prgm3, 
	bool const		p_immediate)
{
	_prgms[0] = p_prgm1;
	_prgms[1] = p_prgm2;
	_prgms[2] = p_prgm3;
	request_update_ogl( p_immediate );
}
void gl::shader::update(
	GLchar const *	p_prgm1, 
	GLchar const *	p_prgm2, 
	GLchar const *	p_prgm3, 
	GLchar const *	p_prgm4, 
	GLchar const *	p_prgm5, 
	bool const		p_immediate)
{
	_prgms[0] = p_prgm1;
	_prgms[1] = p_prgm2;
	_prgms[2] = p_prgm3;
	_prgms[3] = p_prgm4;
	_prgms[4] = p_prgm5;
	request_update_ogl( p_immediate );
}
