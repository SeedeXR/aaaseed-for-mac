#ifdef AAA_FLEX_BUFFER_GL_H
#error "FLEX/BUFFER_GL_H included more than once."
#endif
#define AAA_FLEX_BUFFER_GL_H 1


#ifndef AAA_FLEX_SDK_H
#	include "flex_sdk.h"
#endif


#if AAA_USE_FLEX()
#	ifndef AAA_OBJ_H
#		include "infrastructure/obj/obj.h"
#	endif
#	if !defined(AAA_GL_SSBO_H)
#		include "gl/ssbo.h"
#	endif

#define AAA_USE_CUDA() 1
#define AAA_USE_CUDA_MIN() 0	//set it to 1 to reduce use of cuda includes does not function anymore (2026 January)
//#define AAA_USE_CUDA_PATH "Cuda/cuda131/"
//#	define AAA_USE_CUDA_VERSION_STR() "Cuda/cuda131"
//#define OPENCV_LIB_PATH_PRIVATE(s) "OpenCv/opencv"AAA_LIB_USE_OPENCV_VERSION_STR()"/opencv_"#s##AAA_LIB_USE_OPENCV_VERSION_STR()

struct NvFlexBuffer;
struct NvFlexLibrary;
#if AAA_USE_CUDA_MIN()
	typedef void  * pt_res;
#else
	struct cudaGraphicsResource;
#endif

class c_flex_buffer_gl final : public c_obj
{
private:
	NvFlexBuffer*			_buffer_flex;
	gl::ssbo*				_buffer_gl;
#if AAA_USE_CUDA_MIN()
	pt_res					_buffer_cuda_resource;
#else
	cudaGraphicsResource*	_buffer_cuda_resource;
#endif
//	void*			_buffer_cuda_resource;
	NvFlexBuffer*	_buffer_flex_gl_interop;

	void release();

public:
	c_flex_buffer_gl();
	~c_flex_buffer_gl();

	void reset( NvFlexLibrary* CONST library, UINT32 CONST particles_nb, UINT32 CONST element_size, void* CONST initial_data = nullptr );
	void copy_gl_to_flex();

	FINLINE NvFlexBuffer*	get_flex_buffer()				CONST	{	return _buffer_flex;			}
	FINLINE gl::ssbo*		get_ssbo()						CONST	{	return _buffer_gl;				}
	FINLINE NvFlexBuffer*	get_flex_buffer_gl()			CONST	{	return _buffer_flex_gl_interop;	}

	FINLINE void			bind_gl( UINT32 CONST slot )	CONST	{	_buffer_gl->bind(slot);			}
	FINLINE void			unbind_gl( UINT32 CONST slot )	CONST	{	_buffer_gl->unbind(slot);		}

};

#endif	//#if AAA_USE_FLEX()