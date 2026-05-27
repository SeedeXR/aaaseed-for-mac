// Unused in AAASeed
// Main header
#include "fbo.h"

#if !defined(AAA_GOL_H)
#   include "gol.h"
#endif
#ifndef AAA_AAA_MEM_H
#   include "code_utils/aaa_mem.h"
#endif

namespace gl{

void fbo_props::set_resolution( uint32_t const size_x, uint32_t const size_y )
{
	_width  = size_x;
	_height = size_y;
}
void fbo_props::add_slot( fbo_slot && slot )
{
	_slots.emplace_back( std::move(slot) );
}
void fbo_props::add_slot( fbo_slot const & slot )
{
	_slots.emplace_back( slot );
}
void fbo_props::add_slot(
	e_texture_format	const	format,
	e_texture_data_type	const	data_type,
	e_texture_filter	const	filter,
	e_texture_wrap		const	wrap	)
{
	fbo_slot slot;
	slot._format	= format;
	slot._data_type = data_type;
	slot._filter    = filter;
	slot._wrap      = wrap;
	add_slot(std::move(slot));
}
void fbo_props::use_depth(
	e_texture_data_type const	data_type,
	e_texture_filter const		filter,
	e_texture_wrap const		wrap )
{
	_use_depth       = true;
	_depth_data_type = data_type;
	_depth_filter    = filter;
	_depth_wrap      = wrap;
}

namespace{
	UINT32	nb_instance = 0;
	UINT32	nb_instance_created = 0;
	void inc_count()
	{
		++nb_instance;
		++nb_instance_created;
	}

	CONSTEXPR GLbitfield ACCESS = GL_MAP_WRITE_BIT | GL_MAP_INVALIDATE_BUFFER_BIT;
}

UINT32*	ibo::get_nb_pt()			{	return &nb_instance;			}
UINT32*	ibo::get_nb_created_pt()	{	return &nb_instance_created;	}

C_PCHAR_C	fbo::get_type_name() { return "fbo"; }

fbo * fbo::make( fbo_props const & props, bool const b_immediate )
{
	fbo * res = new fbo();
	res->init( props );
	res->request_init_ogl( b_immediate );
	return res;
}
void fbo::reclaim( fbo * const ptr, bool const b_immediate )
{
	ptr->request_release( b_immediate );
}

fbo::fbo()
{
	 inc_count();
}
fbo::~fbo()
{
	IF_FREE_AND_NULL( _buffer_ids );

	if( nb_instance > 0 )
		--nb_instance;
	else
		debug_break( "%s() destructor called with fbo count already at zero", __FUNCTION__ );
}

void fbo::init(fbo_props const & props )
{
	_props = props;
}

void fbo::init_ogl()
{
	size_t slot_num = _props._slots.size() + (_props._use_depth ? 1 : 0);
	_buffer_ids = (uint32_t*)MALLOC( slot_num * sizeof(uint32_t) );

	glGenFramebuffers( 1, &_gol_id );

	uint32_t draw_buf[32] = { 0 };
	int tex_num = (int)_props._slots.size();
	for( int i = 0; i < tex_num; ++i )
	{
		auto const & slot = _props._slots[i];
		
		if( slot._ogl_id )
		{
			// externaly owned texture.
			_buffer_ids[i] = slot._ogl_id;
		}
		else
		{
			// create texture.
			glGenTextures(1, &_buffer_ids[i]);
			glBindTexture(GL_TEXTURE_2D, _buffer_ids[i]);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, get_gl_texture_wrap(slot._wrap));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, get_gl_texture_wrap(slot._wrap));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, get_gl_texture_filter(slot._filter, false));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, get_gl_texture_filter(slot._filter, false));

			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				get_gl_texture_internal_format(slot._format),
				_props._width,
				_props._height,
				0,
				get_gl_texture_format(slot._format),
				get_gl_texture_data_type(slot._data_type),
				NULL);

			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// link texture to FBO.
		GOL::bind_fbo( _gol_id );
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, 
			get_gl_texture_attachment(_props._depth_format) + i, 
			GL_TEXTURE_2D, 
			_buffer_ids[i], 
			0);
		GOL::bind_fbo( 0 );

		draw_buf[i] = GL_COLOR_ATTACHMENT0 + i;
	}

	if( _props._use_depth )
	{

		if( _props._depth_ogl_id )
		{
			// externaly owned texture.
			_buffer_ids[tex_num] = _props._depth_ogl_id;
		}
		else
		{
			// create texture.
			glGenTextures(1, &_buffer_ids[tex_num] );
			glBindTexture(GL_TEXTURE_2D, _buffer_ids[tex_num] );

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, get_gl_texture_wrap(_props._depth_wrap));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, get_gl_texture_wrap(_props._depth_wrap));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, get_gl_texture_filter(_props._depth_filter, false));
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, get_gl_texture_filter(_props._depth_filter, false));

			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				get_gl_texture_internal_format(_props._depth_format),
				_props._width,
				_props._height,
				0,
				get_gl_texture_format(_props._depth_format),
				get_gl_texture_data_type(_props._depth_data_type),
				NULL);

			glBindTexture(GL_TEXTURE_2D, 0);
		}

		// link texture to FBO.
		GOL::bind_fbo( _gol_id );
		glFramebufferTexture2D(
			GL_FRAMEBUFFER, 
			get_gl_texture_attachment(_props._depth_format), 
			GL_TEXTURE_2D, 
			_buffer_ids[tex_num], 
			0);
		GOL::bind_fbo( 0 );
	}
;
//	if( _props._slots.empty() )
//	{
//      glDrawBuffer(GL_NONE);
//		GOL:set_read_buffer( GL_NONE );
//	}

	set_draw_buffers( tex_num, draw_buf );

	for( int i = 0; i < tex_num; ++i )
	{
		glClearBufferfv( GL_COLOR, i, _props._slots.at(i)._clear_color );
	}
	GOL::bind_fbo( 0 );

}
void fbo::update_ogl()
{
	size_t inc = 0;
	for (auto const & itr : _props._slots) {
		if (!itr._ogl_id) {
			glBindTexture(GL_TEXTURE_2D, _buffer_ids[inc]);
			glTexImage2D(
				GL_TEXTURE_2D,
				0,
				get_gl_texture_internal_format(itr._format),
				_props._width,
				_props._height,
				0,
				get_gl_texture_format(itr._format),
				get_gl_texture_data_type(itr._data_type),
				NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		++inc;
	}

	// Depth textures
	if (_props._use_depth && !_props._depth_ogl_id) {
		glBindTexture(GL_TEXTURE_2D, _buffer_ids[inc]);
		glTexImage2D(
			GL_TEXTURE_2D, 
			0, 
			get_gl_texture_internal_format(_props._depth_format),
			_props._width, 
			_props._height, 
			0, 
			get_gl_texture_format(_props._depth_format),
			get_gl_texture_data_type(_props._depth_data_type),
			NULL);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
}
void fbo::release_ogl()
{
	glDeleteFramebuffers( 1, &_gol_id );

	int i = 0;
	int const slot_num = (int)_props._slots.size(); 
	for (; i < slot_num; ++i)
		if (!_props._slots[i]._ogl_id)
			glDeleteTextures(1, &(_buffer_ids[i]));
	
	if (_props._use_depth && !(_props._depth_ogl_id))
		glDeleteTextures(1, &(_buffer_ids[i]));
}

void fbo::bind() const
{
	GOL::bind_fbo( _gol_id );
}
void fbo::bind_and_clear() const
{
	GOL::bind_fbo( _gol_id );

	int const tex_num = (int)_props._slots.size();
	for (int i = 0; i < tex_num; ++i) {
		glClearBufferfv(GL_COLOR, i, _props._slots.at(i)._clear_color);
	}

	if (_props._use_depth) {
		static float const clear_depth = 1.0f;
		glClearBufferfv(GL_DEPTH, 0, &clear_depth);
	}
}
void fbo::unbind() const
{
	GOL::bind_fbo( 0 );
}

void fbo::bind_slot_read( uint32_t const slot_index, uint32_t const bind_index ) const
{
	GOL::set_tex_unit( bind_index );
	GOL::bind_texture_2d( _buffer_ids[slot_index] );
}
void fbo::unbind_slot_read(  uint32_t const slot_index, uint32_t const bind_index ) const
{
	GOL::set_tex_unit( bind_index );
	GOL::bind_texture_2d( 0 );
}
void fbo::bind_depth_read( uint32_t const bind_index ) const
{
	bind_slot_read( static_cast<uint32_t>(_props._slots.size()), bind_index );
}
void fbo::unbind_depth_read( uint32_t const bind_index) const
{
	unbind_slot_read( static_cast<uint32_t>(_props._slots.size()), bind_index );
}

void fbo::update_resolution( uint32_t const size_x,  uint32_t const size_y, bool const b_immediate )
{
	_props._width  = size_x;
	_props._height = size_y;
	request_update_ogl( b_immediate );
}

}