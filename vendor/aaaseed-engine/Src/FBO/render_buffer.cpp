#include "render_buffer.h"
#include "err.h"

namespace {
GLint	max_size = 0;
}

c_render_buffer::c_render_buffer()
:_gol_id( GOL::gen_render_buffer() )
{
	if( max_size==0 )	//a b_first will not be better
						//todo should be with the pref
		max_size = GOL::get_integer( GL_MAX_RENDERBUFFER_SIZE );
}

c_render_buffer::c_render_buffer( GLenum internal_format, int size_x, int size_y )
:_gol_id( GOL::gen_render_buffer() )
{
	set_storage( internal_format, size_x, size_y );
}

c_render_buffer::~c_render_buffer()
{
	GOL::delete_render_buffer( _gol_id );
}

/*
void	c_render_buffer::bind() 
{
	do_bind( m_buf_id_ );
}

void	c_render_buffer::unbind() 
{
	do_bind( 0 );
}
*/

void	c_render_buffer::set_storage( GLenum internal_format, INT32 size_x, INT32 size_y )
{
	if( size_x > max_size || size_y > max_size )
	{
		ERR_PRINT_STRING( "FBO Renderbuffer : Size too big ( %d, %d )", size_x, size_y );
		return;
	}

	// Guarded bind no done inside (GOL::bind_render_buffer)
	//GLint	id_cur;
	//GOL::get_integer( GL_RENDERBUFFER_BINDING, &id_cur );
	//if( id_cur != (GLint)_gol_id )
	//	bind( _gol_id );
	GOL::bind_render_buffer( _gol_id );

	// Allocate memory for renderBuffer
	glRenderbufferStorage( GL_RENDERBUFFER, internal_format, size_x, size_y );

	// Guarded unbind
	//	if( id_cur != (GLint)_gol_id )
	//		do_bind( id_cur );
}


