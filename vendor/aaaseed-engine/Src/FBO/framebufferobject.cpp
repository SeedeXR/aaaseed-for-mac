#include "framebufferobject.h"
#include "err.h"

//todo adapt a final strategy on this
//  we deal with bind unbind in calling object now
#if 1
#	define	BIND_GUARDED()
#	define	BIND_UNGUARDED()
#else
// Only bind and unbind if different than the currently bound FBO
#	define	BIND_GUARDED() \
	auto fbo_id_saved = GOL::get_fbo_id_cur();	\
	if( _fbo_id != fbo_id_saved )	\
		GOL::bind_fbo( _fbo_id );
#	define	BIND_UNGUARDED() \
	if( _fbo_id != fbo_id_saved )	\
		GOL::bind_fbo( fbo_id_saved );
#endif

c_frame_buffer_object::c_frame_buffer_object()
{
	unbind();
	// Bind this FBO so that it actually gets created now
	_fbo_id = GOL::gen_fbo();
	bind();
	unbind();
}

c_frame_buffer_object::~c_frame_buffer_object() 
{
	GOL::delete_fbo( _fbo_id );
}

void	c_frame_buffer_object::bind() 
{
	GOL::bind_fbo( _fbo_id );
}


void	c_frame_buffer_object::framebuffer_texture_nd( GLenum attachment, GLenum tex_target, GLuint tex_id, INT32 mip_level, INT32 z_slice )
{
	switch( tex_target )
	{
	case GL_TEXTURE_2D :
	case GL_TEXTURE_RECTANGLE_ARB :
		glFramebufferTexture2D( GL_FRAMEBUFFER, attachment, tex_target, tex_id, mip_level );
		break;
	case GL_TEXTURE_1D :
		glFramebufferTexture1D( GL_FRAMEBUFFER, attachment, GL_TEXTURE_1D, tex_id, mip_level );
		break;
	case GL_TEXTURE_3D :
		glFramebufferTexture3D( GL_FRAMEBUFFER, attachment, GL_TEXTURE_3D, tex_id, mip_level, z_slice );
		break;
	}
}

bool c_frame_buffer_object::check_valid()
{
	BIND_GUARDED();
		C_PCHAR	str = nullptr;

		GLenum status = glCheckFramebufferStatus( GL_FRAMEBUFFER );
		GOL::check_error_debug( "after glCheckFramebufferStatus" );
		_b_valid = (status == GL_FRAMEBUFFER_COMPLETE);	 // Everythng's OK
		if( _b_valid )
		{
			GOOD_PRINT_STRING( "%s() FBO %d : GL_FRAMEBUFFER_COMPLETE", __FUNCTION__, get_fbo_id() );
		}
		else
		{
			switch( status )
			{
			case GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT:			str = "GL_FRAMEBUFFER_INCOMPLETE_ATTACHMENT";			break;
			case GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT:	str = "GL_FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT";	break;
			case GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT:		str = "GL_FRAMEBUFFER_INCOMPLETE_DIMENSIONS_EXT";		break;
			case GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT:			str = "GL_FRAMEBUFFER_INCOMPLETE_FORMATS_EXT";			break;
			case GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER:			str = "GL_FRAMEBUFFER_INCOMPLETE_DRAW_BUFFER";			break;
			case GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER:			str = "GL_FRAMEBUFFER_INCOMPLETE_READ_BUFFER";			break;
			case GL_FRAMEBUFFER_UNSUPPORTED:					str = "GL_FRAMEBUFFER_UNSUPPORTED";						break;
			case GL_FRAMEBUFFER_UNDEFINED:						str = "GL_FRAMEBUFFER_UNDEFINED";						break;
			case GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE:			str = "GL_FRAMEBUFFER_INCOMPLETE_MULTISAMPLE";			break;
			case GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS:		str = "GL_FRAMEBUFFER_INCOMPLETE_LAYER_TARGETS";		break;
			default:
				str = "Unknown Error";
				break;
			}
			//( str )
				ERR_PRINT_STRING( "FBO internal id %d : %s", get_fbo_id(), str );
		}
	BIND_UNGUARDED();
	return _b_valid;
}

bool c_frame_buffer_object::bind_if_valid()
{
	if( is_valid() )
	{
		bind();
		return true;
	}
	unbind();
	return false;
}

/// Accessors
GLenum	c_frame_buffer_object::get_attached_type( GLenum attachment )
{
	// Returns GL_RENDERBUFFER or GL_TEXTURE
	BIND_GUARDED();
		GLint	type = 0;
		glGetFramebufferAttachmentParameteriv( GL_FRAMEBUFFER, attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE, &type );
	BIND_UNGUARDED();
	return GLenum( type );
}

GLuint	c_frame_buffer_object::get_attached_id_low( GLenum attachment )
{
	GLint	id = 0;
	glGetFramebufferAttachmentParameteriv( GL_FRAMEBUFFER, attachment, GL_FRAMEBUFFER_ATTACHMENT_OBJECT_NAME, &id );
	return GLuint( id );
}

GLuint	c_frame_buffer_object::get_attached_id( GLenum attachment )
{
	BIND_GUARDED();
		GLuint	id = get_attached_id_low( attachment );
	BIND_UNGUARDED();
	return id;
}

GLint	c_frame_buffer_object::get_attached_mip_level( GLenum attachment )
{
	BIND_GUARDED();
		GLint	level = 0;
		glGetFramebufferAttachmentParameteriv( GL_FRAMEBUFFER, attachment, GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL, &level );
	BIND_UNGUARDED();
	return level;
}

GLint	c_frame_buffer_object::get_attached_cube_face( GLenum attachment )
{
	BIND_GUARDED();
		GLint	level = 0;
		glGetFramebufferAttachmentParameteriv( GL_FRAMEBUFFER, attachment, GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE, &level );
	BIND_UNGUARDED();
	return level;
}

GLint	c_frame_buffer_object::get_attached_z_slice( GLenum attachment )
{
	BIND_GUARDED();
		GLint	slice = 0;
		glGetFramebufferAttachmentParameteriv( GL_FRAMEBUFFER, attachment, GL_FRAMEBUFFER_ATTACHMENT_TEXTURE_3D_ZOFFSET_EXT, &slice );
	BIND_UNGUARDED();
	return slice;
}

void	c_frame_buffer_object::attach_texture_low( GLenum tex_target, GLuint tex_id, GLenum attachment, int mip_level, int z_slice )
{
	if( get_attached_id_low( attachment==GL_DEPTH_STENCIL_ATTACHMENT ? GL_DEPTH_ATTACHMENT : attachment ) == tex_id )
	{
		ERR_PRINT_STRING( "FBO %d : Attach Texture PERFORMANCE WARNING", _fbo_id );
		ERR_PRINT_STRING( "\tRedundant bind of texture ( id = %d )", tex_id );
	}
	framebuffer_texture_nd( attachment, tex_target, tex_id, mip_level, z_slice );
	_b_valid = false;
}

void	c_frame_buffer_object::attach_texture( GLenum tex_target, GLuint tex_id, GLenum attachment, int mip_level, int z_slice )
{
	BIND_GUARDED();
		attach_texture_low( tex_target, tex_id, attachment, mip_level, z_slice );
	BIND_UNGUARDED();
}

void	c_frame_buffer_object::attach_textures( INT32 nb_tex, GLenum tex_target[], GLuint tex_id[], GLenum attachment[], INT32 mip_level[], INT32 z_slice[] )
{
	for( INT32 i = 0; i < nb_tex; ++i )
	{
		attach_texture( tex_target[i], tex_id[i], attachment ? attachment[i] : ( GL_COLOR_ATTACHMENT0 + i ), mip_level ? mip_level[i] : 0, z_slice ? z_slice[i] : 0 );
	}
}

void	c_frame_buffer_object::attach_render_buffer_low( GLuint buffer_id, GLenum attachment )
{
	if( get_attached_id( attachment ) == buffer_id )
	{
		ERR_PRINT_STRING( "FBO %d : attach_render_buffer PERFORMANCE WARNING", _fbo_id );
		ERR_PRINT_STRING( "\tRedundant bind of Renderbuffer ( id = %d )", buffer_id );
	}
	glFramebufferRenderbuffer( GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, buffer_id );
	_b_valid = false;
}

void	c_frame_buffer_object::attach_render_buffer( GLuint buffer_id, GLenum attachment )
{
	BIND_GUARDED();
		attach_render_buffer_low( buffer_id, attachment );
	BIND_UNGUARDED();
}

void	c_frame_buffer_object::attach_render_buffers( INT32 nb_buffers, GLuint buffer_id[], GLenum attachment[] )
{
	BIND_GUARDED();
	for( INT32 i = 0; i < nb_buffers; ++i )
	{
		attach_render_buffer_low( buffer_id[i], attachment ? attachment[i] : ( GL_COLOR_ATTACHMENT0 + i ) );
	}
	BIND_UNGUARDED();
}

void	c_frame_buffer_object::detach_low( GLenum attachment )
{
	GLenum	type = get_attached_type( attachment );
	switch( type )
	{
	case GL_NONE:
		break;
	case GL_RENDERBUFFER:
		attach_render_buffer_low( 0, attachment );
		break;
	case GL_DEPTH_ATTACHMENT:
	case GL_STENCIL_ATTACHMENT:
	case GL_TEXTURE:
		attach_texture_low( GL_TEXTURE_2D, 0, attachment );
		break;
	case GL_FRAMEBUFFER_DEFAULT:
		ERR_PRINT_STRING( "FBO %d : unbind_attachment error, we don't deal with GL_FRAMEBUFFER_DEFAULT", _fbo_id );
		break;
	default:
		ERR_PRINT_STRING( "FBO %d : unbind_attachment error, unknown attached resource type", _fbo_id );
		break;
	}
	_b_valid = false;
}

void	c_frame_buffer_object::detach( GLenum attachment )
{
	BIND_GUARDED();
		detach_low( attachment );
	BIND_UNGUARDED();
}

//shouldn't we deal here with depth stencil when we use it as texture and not a render buffer
void	c_frame_buffer_object::detach_all()
{
	BIND_GUARDED();
		for( INT32 i = 0; i < GOL::fbo_color_attachment_nb_max; ++i )
			detach_low( GL_COLOR_ATTACHMENT0 + i );
		detach_low( GL_DEPTH_ATTACHMENT );
		detach_low( GL_STENCIL_ATTACHMENT );
	BIND_UNGUARDED();
}

void	c_frame_buffer_object::detach_render_buffer( GLenum attachment )
{
	glDeleteRenderbuffers( _fbo_id, &attachment );
	_b_valid = false;
}
