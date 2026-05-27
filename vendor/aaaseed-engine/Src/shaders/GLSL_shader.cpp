#include "GLSL_shader.h"
#include "err.h"
#include "shader_master.h"
#ifndef	AAA_GOL_SHADER_H
#	include "gol/gol_shader.h"
#endif
#include "gol/gol_os.h"


c_shader_glsl::c_shader_glsl( c_shader::SHADER_TYPE shader_type )
	: c_shader(shader_type)
	,_shader_id	{0}
{
}
c_shader_glsl::~c_shader_glsl()
{
	dealloc();
}

void	c_shader_glsl::dealloc()
{
	_b_valid = false;
	if( _shader_id != 0 )
	{
		GOL::delete_shader( _shader_id );
		_shader_id = 0;
	}
}

GLenum	c_shader_glsl::get_gl_type( SHADER_TYPE shader_type_in )
{
	GLenum shader_type_gl;
	switch( shader_type_in )
	{
	case c_shader::SHADER_FRAGMENT:		shader_type_gl = GL_FRAGMENT_SHADER;	break;
	case c_shader::SHADER_VERTEX:		shader_type_gl = GL_VERTEX_SHADER;		break;
	case c_shader::SHADER_GEOMETRY:		shader_type_gl = GL_GEOMETRY_SHADER;	break;
	case c_shader::SHADER_COMPUTE:		shader_type_gl = GL_COMPUTE_SHADER;		break;
	default:							shader_type_gl = GL_INVALID_ENUM;		break;
	}
	return shader_type_gl;
}

namespace {
	CONSTEXPR INT32 INCLUDE_NB = c_shader_master::INCLUDE_NB;
	GLchar CONST * 	source[			INCLUDE_NB+1];
	GLint			source_len[		INCLUDE_NB+1];
	o_str			o_source_text[	INCLUDE_NB+1];

}

void	c_shader_glsl::load()
{
	_b_loaded = false;
	_b_valid = false;
	if( _fpath.get_len() <= 0 )
	{
		ERR_PRINT_STRING( "GLSL_shader, filename is empty" );
		return;
	}

	GLenum	shader_type_gl = get_gl_type( _s_shader_type );
	// Create the shader object
	if( shader_type_gl == GL_INVALID_ENUM )
	{
		debug_break( "GLSL : unknown shader type %d", _s_shader_type );
		return;
	}

	_shader_id = GOL::create_shader( shader_type_gl );
	if( _shader_id == 0 )
	{
		ERR_PRINT_STRING( "GLSL_shader could not create glCreateShader" );
		return;
	}
	
	INT32 nb			= 0;
	INT32 line_offset	= 0;
	// Load the "includced"
	for( INT32 i=0; i<INCLUDE_NB; ++i )
	{
		o_str& o = o_source_text[i];
		o_str* o_name;
		if( i==0 )
		{
			if( GOL::CTX::is_nvidia() )
				o_name = &g_shader_master->_include_file_name_nvidia;
			else if( GOL::CTX::is_amd() )
				o_name = &g_shader_master->_include_file_name_amd;
			else if( GOL::CTX::is_intel() )
				o_name = &g_shader_master->_include_file_name_intel;
			else
				o_name = &g_shader_master->_include_file_name[0];
		}
		else
			o_name = &g_shader_master->_include_file_name[i];
		if( !o_name->is_empty() )
		{
			auto err = o.read_file( o_name->get() );
			if( ERR(err) )
			{
				ERR_PRINT_STRING( "%s() could not read and so prepend \"%s\"",  __FUNCTION__, o_name->get() );
			}
			else
			{
				source[nb] = o.get();
				source_len[nb] = o.get_len();
				line_offset += o.get_line_nb()-1;	// x lines lines means x-1 return
				++nb;
			}
		}
		else
			break;
	}

	// Load the shader source
	
	o_str& name_mess = o_str::push_name();
	name_mess.compact_fname( _fpath );
	
	o_str& o = o_source_text[nb];
	auto err = o.read_file( _fpath.get() );

	if( ERR(err) )
	{
		ERR_PRINT_STRING( "GLSL_shader::load, could not read \"%s\"", name_mess.get() );
		return;
	}

	_b_loaded = true;
	source[nb] = o.get();
	source_len[nb] = o.get_len();
	++nb;
	GOL::shader_source( _shader_id, nb, source, source_len );
	GOL::check_error( "after GOL::shader_source()" );

	// Compile the shader
	//glCompileShader( _shader_id );
	GOL::compile_shader( _shader_id );
//	GOL::get_error( "After compile_shader" );

	// Get compile status
	GLint b_compiled;
	GOL::get_shader_iv( _shader_id, GL_COMPILE_STATUS, &b_compiled );

	if( b_compiled )
	{
		GOL::GOOD_PRINT_STRING( "Shader Line offset %d: \"%s\" compiled", line_offset, name_mess.get() );
		GOL::print_info_log( false, _shader_id, false );	//to get the warnings
		_b_valid = true;
	}
	else
	{
		// Print out the info log
		GOL::ERR_PRINT_STRING( "Shader Line offset %d: \"%s\" could not Compile :", line_offset, name_mess.get() );
		GOL::print_info_log( false, _shader_id, true );

		// cleanup shader
		dealloc();
	}

	o_str::pop_name();
}

c_shader_glsl::operator GLuint () CONST
{
	return _shader_id;
}
