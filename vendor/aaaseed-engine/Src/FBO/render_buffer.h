
#ifdef AAA_RENDER_BUFFER_H
#error "RENDER_BUFFER_H included more than once."
#endif
#define AAA_RENDER_BUFFER_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef AAA_GOL_H
#	include "gol/gol.h"
#endif

/*!
Renderbuffer Class. This class encapsulates the Renderbuffer OpenGL
object described in the FramebufferObject (FBO) OpenGL spec. 
See the official spec at:
	http://oss.sgi.com/projects/ogl-sample/registry/EXT/framebuffer_object.txt
for complete details.

A "Renderbuffer" is a chunk of GPU memory used by FramebufferObjects to
represent "traditional" framebuffer memory (depth, stencil, and color buffers).
By "traditional," we mean that the memory cannot be bound as a texture. 
With respect to GPU shaders, Renderbuffer memory is "write-only." Framebuffer
operations such as alpha blending, depth test, alpha test, stencil test, etc.
read from this memory in post-fragement-shader (ROP) operations.

The most common use of Renderbuffers is to create depth and stencil buffers.
Note that as of 7/1/05, NVIDIA drivers to do not support stencil Renderbuffers.

Usage Notes:
  1) "internalFormat" can be any of the following:
	  Valid OpenGL internal formats beginning with:
		RGB, RGBA, DEPTH_COMPONENT

	  or a stencil buffer format (not currently supported 
	  in NVIDIA drivers as of 7/1/05).
		STENCIL_INDEX1_EXT
		STENCIL_INDEX4_EXT
		STENCIL_INDEX8_EXT
		STENCIL_INDEX16_EXT
*/

class c_render_buffer final : public c_obj
{
private:
	GLuint	_gol_id;

public:
	c_render_buffer();
	c_render_buffer(				GLenum internal_format, INT32 size_x, INT32 size_y );
	~c_render_buffer();
	
//			void	bind();
//			void	unbind();

			void	set_storage(	GLenum internal_format, INT32 size_x, INT32 size_y );
			UINT32	get_id() CONST	{	return _gol_id;	};
};

