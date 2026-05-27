
#ifdef AAA_FRAMEBUFFEROBJECT_H
#error "FRAMEBUFFEROBJECT_H included more than once."
#endif
#define AAA_FRAMEBUFFEROBJECT_H 1


#ifndef AAA_GOL_H
#	include "gol/gol.h"
#endif
#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif

/*!
FramebufferObject Class. This class encapsulates the FramebufferObject
(FBO) OpenGL spec. See the official spec at:
	http://oss.sgi.com/projects/ogl-sample/registry/EXT/framebuffer_object.txt

for details.

A framebuffer object (FBO) is conceptually a structure containing pointers
to GPU memory. The memory pointed to is either an OpenGL texture or an
OpenGL RenderBuffer. FBOs can be used to render to one or more textures,
share depth buffers between multiple sets of color buffers/textures and
are a complete replacement for pbuffers.

Performance Notes:
  1) It is more efficient (but not required) to call Bind() 
	 on an FBO before making multiple method calls. For example:
		
	  FramebufferObject fbo;
	  fbo.Bind();
	  fbo.AttachTexture(GL_TEXTURE_2D, texId0, GL_COLOR_ATTACHMENT0);
	  fbo.AttachTexture(GL_TEXTURE_2D, texId1, GL_COLOR_ATTACHMENT1);
	  fbo.IsValid();

	To provide a complete encapsulation, the following usage
	pattern works correctly but is less efficient:

	  FramebufferObject fbo;
	  // NOTE : No Bind() call
	  fbo.AttachTexture(GL_TEXTURE_2D, texId0, GL_COLOR_ATTACHMENT0);
	  fbo.AttachTexture(GL_TEXTURE_2D, texId1, GL_COLOR_ATTACHMENT1);
	  fbo.IsValid();

	The first usage pattern binds the FBO only once, whereas
	the second usage binds/unbinds the FBO for each method call.

  2) Use FramebufferObject::Disable() sparingly. We have intentionally
	 left out an "Unbind()" method because it is largely unnecessary
	 and encourages rendundant Bind/Unbind coding. Binding an FBO is
	 usually much faster than enabling/disabling a pbuffer, but is
	 still a costly operation. When switching between multiple FBOs
	 and a visible OpenGL framebuffer, the following usage pattern 
	 is recommended:

	  FramebufferObject fbo1, fbo2;
	  fbo1.Bind();
		... Render ...
	  // NOTE : No Unbind/Disable here...

	  fbo2.Bind();
		... Render ...

	  // Disable FBO rendering and return to visible window
	  // OpenGL framebuffer.
	  FramebufferObject::Disable();
*/

class c_frame_buffer_object final : public c_obj
{
private:
					GLuint	_fbo_id;
					bool	_b_valid;

					void	framebuffer_texture_nd( GLenum attachment, GLenum tex_target, GLuint tex_id, INT32 mip_level, INT32 z_slice );
public:
	/// Disable all FBO rendering and return to traditional,
	/// windowing-system controlled framebuffer
	///  NOTE:
	///	 This is NOT an "unbind" for this specific FBO, but rather
	///	 disables all FBO rendering. This call is intentionally "static"
	///	 and named "Disable" instead of "Unbind" for this reason. The
	///	 motivation for this strange semantic is performance. Providing
	///	 "Unbind" would likely lead to a large number of unnecessary
	///	 FBO enablings/disabling.
	FINLINE	static	void	unbind()	{	GOL::bind_fbo(0);	}

			GLuint	get_attached_id_low( GLenum attachment );
	/// Free any resource bound to the "attachment" point of this FBO
			void	detach_low( GLenum attachment );
			void	attach_render_buffer_low( GLuint buffer_id, GLenum attachment = GL_COLOR_ATTACHMENT0 );
			void	attach_texture_low( GLenum tex_target, GLuint tex_id, GLenum attachment = GL_COLOR_ATTACHMENT0, INT32 mip_level = 0, INT32 z_slice = 0 );

public:
	c_frame_buffer_object();
	virtual ~c_frame_buffer_object();


	/// Bind this FBO as current render target
			void	bind();
			bool	bind_if_valid();
	/// Bind a texture to the "attachment" point of this FBO
			void	attach_texture( GLenum tex_target, GLuint tex_id, GLenum attachment = GL_COLOR_ATTACHMENT0, INT32 mip_level = 0, INT32 z_slice = 0 );

	/// Bind an array of textures to multiple "attachment" points of this FBO
	///  - By default, the first 'numTextures' attachments are used,
	///	starting with GL_COLOR_ATTACHMENT0_EXT
			void	attach_textures( INT32 nb_textures, GLenum tex_target[], GLuint tex_id[], GLenum attachment[] = nullptr, INT32 mip_level[] = nullptr, INT32 z_slice[] = nullptr );

	/// Bind a render buffer to the "attachment" point of this FBO
			void	attach_render_buffer( GLuint buffer_id, GLenum attachment = GL_COLOR_ATTACHMENT0 );

	/// Bind an array of render buffers to corresponding "attachment" points
	/// of this FBO.
	/// - By default, the first 'numBuffers' attachments are used,
	///   starting with GL_COLOR_ATTACHMENT0
			void	attach_render_buffers( INT32 nb_buffers, GLuint buffer_id[], GLenum attachment[] = nullptr );

	/// Free any resource bound to the "attachment" point of this FBO
			void	detach( GLenum attachment );

	/// Free any resources bound to any attachment points of this FBO
			void	detach_all();

			void	detach_render_buffer( GLenum attachment );

	/// Is this FBO currently a valid render target?
	///  - Sends output to std::cerr by default but can
	///	be a user-defined C++ stream
	///
	/// NOTE : This function works correctly in debug build
	///		mode but always returns "true" if NDEBUG is
	///		is defined (optimized builds)
			bool	check_valid();
	FINLINE bool	is_valid()	CONST		{	return _b_valid;	}

	/// BEGIN : Accessors
	/// Is attached type GL_RENDERBUFFER or GL_TEXTURE?
			GLenum	get_attached_type( GLenum attachment );

private:
	/// What is the Id of Renderbuffer/texture currently 
	/// attached to "attachment?"
			GLuint	get_attached_id( GLenum attachment );
public:	
	/// Which mipmap level is currently attached to "attachment?"
			GLint	get_attached_mip_level( GLenum attachment );

	/// Which cube face is currently attached to "attachment?"
			GLint	get_attached_cube_face( GLenum attachment );

	/// Which z-slice is currently attached to "attachment?"
			GLint	get_attached_z_slice( GLenum attachment );

	FINLINE	GLint	get_fbo_id()	CONST { return _fbo_id; }
	/// END : Accessors
};

