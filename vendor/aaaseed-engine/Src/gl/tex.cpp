// Unused in AAASeed
// Main header
#include "gl/tex.h"
#include "gol_tex.h"
#include "code_utils/aaa_mem.h"

C_PCHAR_C	gl::tex::get_type_name() { return "tex"; }

gl::tex * gl::tex::make( gl::tex_props const & props, bool const b_immediate )
{
	gl::tex * res = new gl::tex();
	res->init( props );
	res->request_init_ogl( b_immediate );
	return res;
}
void gl::tex::reclaim( gl::tex * const ptr, bool const b_immediate )
{
	ptr->request_release( b_immediate );
}

void gl::tex::init( gl::tex_props const & props )
{
	_props = props;
	_gl_internal_fmt = gl::get_gl_texture_internal_format(_props._format);
}

void gl::tex::init_ogl()
{
	GOL::gen_texture( &_gol_id );
	GOL::bind_texture_2d( _gol_id );

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S,		gl::get_gl_texture_wrap(_props._wrap));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T,		gl::get_gl_texture_wrap(_props._wrap));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,	gl::get_gl_texture_filter(_props._filter, _props._generate_mipmaps));
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,	gl::get_gl_texture_filter(_props._filter, _props._generate_mipmaps));

	glTexImage2D(
		GL_TEXTURE_2D, 
		0,
		_gl_internal_fmt,
		_props._width, 
		_props._height, 
		0,
		gl::get_gl_texture_format(_props._format),
		gl::get_gl_texture_data_type(_props._data_type),
		_props._data);

	if( _props._generate_mipmaps )
	{
		GOL::generate_mipmap_2d();
	}

	 GOL::bind_texture_2d( 0 );
}

void gl::tex::update_ogl()
{
	GOL::bind_texture_2d( _gol_id );
	glTexSubImage2D(
		GL_TEXTURE_2D, 
		0, 
		0, 
		0, 
		_props._width, 
		_props._height,
		gl::get_gl_texture_format(_props._format),
		gl::get_gl_texture_data_type(_props._data_type),
		_props._data);

	if( _props._generate_mipmaps )
	{
		GOL::generate_mipmap_2d();
	}

	 GOL::bind_texture_2d( 0 );
}
void gl::tex::release_ogl()
{
	GOL::delete_texture( &_gol_id );
}

void gl::tex::bind( uint32_t const bind_index ) const
{
	GOL::set_tex_unit( bind_index );
	GOL::bind_texture_2d( _gol_id );
}
void gl::tex::unbind( uint32_t const bind_index ) const
{
	GOL::set_tex_unit( bind_index );
	GOL::bind_texture_2d( 0 );
}

void gl::tex::bind_compute_read(			uint32_t const bind_index ) const
{
	glBindImageTexture( bind_index, _gol_id, 0, GL_FALSE, 0, GL_READ_ONLY, _gl_internal_fmt );
}
void gl::tex::unbind_compute_read(			uint32_t const bind_index ) const
{
	glBindImageTexture( bind_index,  0, 0, GL_FALSE, 0, GL_READ_ONLY, _gl_internal_fmt );
}

void gl::tex::bind_compute_write(			uint32_t const bind_index ) const
{
	glBindImageTexture( bind_index, _gol_id, 0, GL_FALSE, 0, GL_WRITE_ONLY, _gl_internal_fmt );
}
void gl::tex::unbind_compute_write(			uint32_t const bind_index ) const
{
	glBindImageTexture( bind_index,   0, 0, GL_FALSE, 0, GL_WRITE_ONLY, _gl_internal_fmt );
}

void gl::tex::bind_compute_read_write(		uint32_t const bind_index ) const
{
	glBindImageTexture( bind_index, _gol_id, 0, GL_FALSE, 0, GL_READ_WRITE, _gl_internal_fmt );
}
void gl::tex::unbind_compute_read_write(	uint32_t const bind_index ) const
{
	glBindImageTexture( bind_index,   0, 0, GL_FALSE, 0, GL_READ_WRITE, _gl_internal_fmt );
}

void gl::tex::update(
	char const *    p_data, 
	bool const      p_immediate)
{
	_props._data = p_data;
	request_update_ogl(p_immediate);
}

uint32_t gl::get_gl_texture_internal_format( gl::e_texture_format const p_in )
{
	uint32_t res = 0;
	switch(p_in)
	{
	case gl::e_texture_format::gray_scale:			res = GL_LUMINANCE;				break;
	case gl::e_texture_format::gray_scale_alpha:	res = GL_LUMINANCE_ALPHA;		break;
	case gl::e_texture_format::RGB:
	case gl::e_texture_format::BGR:					res = GL_RGB;					break;
	case gl::e_texture_format::RGB16F:
	case gl::e_texture_format::BGR16F:				res = GL_RGB16F;				break;
	case gl::e_texture_format::RGB32F:
	case gl::e_texture_format::BGR32F:				res = GL_RGB32F;				break;
	case gl::e_texture_format::RGBA:
	case gl::e_texture_format::BGRA:				res = GL_RGBA;					break;
	case gl::e_texture_format::RGBA16F:
	case gl::e_texture_format::BGRA16F:				res = GL_RGBA16F;				break;
	case gl::e_texture_format::RGBA32F:
	case gl::e_texture_format::BGRA32F:				res = GL_RGBA32F;				break;
	case gl::e_texture_format::depth:				res = GL_DEPTH_COMPONENT;		break;
	case gl::e_texture_format::depth16:				res = GL_DEPTH_COMPONENT16;		break;
	case gl::e_texture_format::depth24:				res = GL_DEPTH_COMPONENT24;		break;
	case gl::e_texture_format::depth32:				res = GL_DEPTH_COMPONENT32F;	break;
	case gl::e_texture_format::depth24_stencil8:	res = GL_DEPTH24_STENCIL8;		break;
	}
	return res;
}

uint32_t gl::get_gl_texture_format( gl::e_texture_format const p_in )
{
	uint32_t res = 0;
	switch(p_in)
	{
	case gl::e_texture_format::gray_scale:			res = GL_LUMINANCE;				break;
	case gl::e_texture_format::gray_scale_alpha:	res = GL_LUMINANCE_ALPHA;		break;
	case gl::e_texture_format::RGB:
	case gl::e_texture_format::RGB16F:
	case gl::e_texture_format::RGB32F:				res = GL_RGB;					break;
	case gl::e_texture_format::RGBA:
	case gl::e_texture_format::RGBA16F:
	case gl::e_texture_format::RGBA32F:				res = GL_RGBA;					break;
	case gl::e_texture_format::BGR:
	case gl::e_texture_format::BGR16F:
	case gl::e_texture_format::BGR32F:				res = GL_BGR;					break;
	case gl::e_texture_format::BGRA:
	case gl::e_texture_format::BGRA16F:
	case gl::e_texture_format::BGRA32F:				res = GL_BGRA;					break;
	case gl::e_texture_format::depth:
	case gl::e_texture_format::depth16:
	case gl::e_texture_format::depth24:
	case gl::e_texture_format::depth32:            
	case gl::e_texture_format::depth24_stencil8:	res = GL_DEPTH_COMPONENT;		break;
	}
	return res;
}

uint32_t gl::get_gl_texture_attachment( gl::e_texture_format const p_in )
{
	uint32_t res = 0;
	switch(p_in)
	{
	case gl::e_texture_format::gray_scale:         
	case gl::e_texture_format::gray_scale_alpha:   
	case gl::e_texture_format::RGB:
	case gl::e_texture_format::RGB16F:
	case gl::e_texture_format::RGB32F:             
	case gl::e_texture_format::RGBA:
	case gl::e_texture_format::RGBA16F:
	case gl::e_texture_format::RGBA32F:            
	case gl::e_texture_format::BGR:
	case gl::e_texture_format::BGR16F:
	case gl::e_texture_format::BGR32F:             
	case gl::e_texture_format::BGRA:
	case gl::e_texture_format::BGRA16F:
	case gl::e_texture_format::BGRA32F:				res = GL_COLOR_ATTACHMENT0;			break;
	case gl::e_texture_format::depth:
	case gl::e_texture_format::depth16:
	case gl::e_texture_format::depth24:
	case gl::e_texture_format::depth32:				res = GL_DEPTH_ATTACHMENT;			break;
	case gl::e_texture_format::depth24_stencil8:	res = GL_DEPTH_STENCIL_ATTACHMENT;	break;
	}
	return res;
}

uint32_t gl::get_gl_texture_data_type( gl::e_texture_data_type const p_in )
{
	uint32_t res = 0;
	switch(p_in)
	{
	case gl::e_texture_data_type::unsigned_byte:    res = GL_UNSIGNED_BYTE;				break;
	case gl::e_texture_data_type::real:             res = GL_FLOAT;						break;
	}
	return res;
}

uint32_t gl::get_gl_texture_filter( gl::e_texture_filter const p_in, bool const p_enable_mipmap )
{
	uint32_t res = 0;
	if( p_enable_mipmap )
	{
		switch(p_in)
		{
		case gl::e_texture_filter::linear:			res = GL_LINEAR_MIPMAP_LINEAR;		break;
		case gl::e_texture_filter::nearest:			res = GL_NEAREST_MIPMAP_NEAREST;    break;
		}
	}
	else
	{
		switch(p_in)
		{
		case gl::e_texture_filter::linear:			res = GL_LINEAR;					break;
		case gl::e_texture_filter::nearest:			res = GL_NEAREST;					break;
		}
	}
	return res;
}

uint32_t gl::get_gl_texture_wrap( gl::e_texture_wrap const p_in )
{
	uint32_t res = 0;
	switch(p_in)
	{
	case gl::e_texture_wrap::border:		res = GL_CLAMP_TO_BORDER;			break;
	case gl::e_texture_wrap::edge:			res = GL_CLAMP_TO_EDGE;				break;
	case gl::e_texture_wrap::repeat:        res = GL_REPEAT;					break;
	case gl::e_texture_wrap::mirror:        res = GL_MIRRORED_REPEAT;			break;
	case gl::e_texture_wrap::mirroredge:    res = GL_MIRROR_CLAMP_TO_EDGE;		break;
	}
	return res;
}
