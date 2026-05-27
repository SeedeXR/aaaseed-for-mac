#include "Buffer_Gl.h"

#if AAA_USE_FLEX()

#	include "err.h"
#	if AAA_USE_CUDA()

#		include "cuda_runtime_api.h"

#		if AAA_USE_CUDA_MIN()
		//enum cudaError
		//{
		//	cudaSuccess                           =      0,
		//	cudaErrorApiFailureBase               =  10000
		//};
//typedef struct cudaGraphicsResource *cudaGraphicsResource_t;
		
//		typedef int cudaError;//
//#			define CUDARTAPI
			extern __host__ int cudaGraphicsUnregisterResource( pt_res resource );
			extern __host__ int cudaGraphicsGLRegisterBuffer( pt_res * resource, GLuint buffer, unsigned int flags );
			extern __host__ int cudaGraphicsMapResources( int count, pt_res * resources, void * stream = nullptr );
			extern __host__ int cudaGraphicsResourceGetMappedPointer( void **devPtr, size_t *size, pt_res resource);
			extern __host__ int cudaGraphicsUnmapResources( int count, pt_res * resources, void * stream = nullptr );
//			extern __host__ cudaError_t __stdcall cudaMemcpy(void *dst, const void *src, size_t count, INT32 kind);
			extern __host__ int cudaMemcpy( void *dst, const void *src, size_t count, INT32 kind );
#			define cudaSuccess 0
#		else
#			include "cuda_gl_interop.h"
#		endif

#		define CUDA_SUCCESSED( code ) (code == cudaSuccess)

#		include "lib_use.h"
//	AAA_LIB_USE_MESSAGE( "x64/cudart" )
	AAA_LIB_USE_MESSAGE( "x64/cudart_static" ) 

#		endif	//#if AAA_USE_CUDA()

c_flex_buffer_gl::c_flex_buffer_gl()
	:_buffer_flex				{nullptr}
	,_buffer_gl					{nullptr}
	,_buffer_cuda_resource		{nullptr}
	,_buffer_flex_gl_interop	{nullptr}
{}

c_flex_buffer_gl::~c_flex_buffer_gl()
{
	release();
}

void c_flex_buffer_gl::release()
{
	if( _buffer_flex )
	{
		NvFlexFreeBuffer(_buffer_flex);
		_buffer_flex = nullptr;
	}
	if( _buffer_flex_gl_interop )
	{
		NvFlexUnregisterOGLBuffer(_buffer_flex_gl_interop);
		_buffer_flex_gl_interop = nullptr;
	}
#if AAA_USE_CUDA()
	if( _buffer_cuda_resource )
	{
		cudaGraphicsUnregisterResource(_buffer_cuda_resource);
		_buffer_cuda_resource = nullptr;
	}
#endif	//#if AAA_USE_CUDA()
	gl::ssbo::release_and_null( _buffer_gl );

}

void c_flex_buffer_gl::reset( NvFlexLibrary * CONST library, UINT32 CONST particle_nb, UINT32 CONST element_size, void* CONST initial_data )
{
	release();

	//create gl ssbo
	_buffer_gl = gl::ssbo::make( particle_nb * element_size,	false, (const char*)initial_data, "flex_buffer" );

	//pass library
	_buffer_flex_gl_interop = NvFlexRegisterOGLBuffer( library, _buffer_gl->get_gol_id(), particle_nb, element_size );
	_buffer_flex = NvFlexAllocBuffer( library, particle_nb, element_size, NvFlexBufferType::eNvFlexBufferDevice );

#if AAA_USE_CUDA()
#	if AAA_USE_CUDA_MIN()
	auto result = cudaGraphicsGLRegisterBuffer( &_buffer_cuda_resource, _buffer_gl->get_gol_id(), 0 );
#	else
	auto result = cudaGraphicsGLRegisterBuffer( &_buffer_cuda_resource, _buffer_gl->get_gol_id(), cudaGraphicsRegisterFlagsNone );
#endif

	if( !CUDA_SUCCESSED( result ) )
		err_print( "Cuda Register GL buffer failed." );

	copy_gl_to_flex();
#endif	//#if AAA_USE_CUDA()
}

void c_flex_buffer_gl::copy_gl_to_flex()
{
#if AAA_USE_CUDA()
	void* flex_data_ptr = NvFlexMap( _buffer_flex, eNvFlexMapWait );

	auto map_gl_result = cudaGraphicsMapResources( 1, &_buffer_cuda_resource );
	if( CUDA_SUCCESSED( map_gl_result ) )
	{
		void* pt_gl_interop = nullptr;
		size_t size;
		auto err = cudaGraphicsResourceGetMappedPointer( &pt_gl_interop, &size, _buffer_cuda_resource );
		if( CUDA_SUCCESSED( err ) )
		{
#	if AAA_USE_CUDA_MIN()
			auto copy_result = cudaMemcpy( flex_data_ptr, pt_gl_interop, size, 3 );
#	else
			auto copy_result = cudaMemcpy( flex_data_ptr, pt_gl_interop, size, cudaMemcpyKind::cudaMemcpyDeviceToDevice );
#	endif
			//
			if( !CUDA_SUCCESSED( copy_result ) )
				err_print( "Cuda GL memcpy failed." );

			auto err = cudaGraphicsUnmapResources(1, &_buffer_cuda_resource );
			if( !CUDA_SUCCESSED( err ) )
				err_print( "Cuda GL pointer unmap failed." );
		}
		else
			err_print( "Cuda GL get mapped pointer failed." );
	}
	else
		err_print( "Cuda GL map failed." );

	NvFlexUnmap( _buffer_flex );
#	endif	//#if AAA_USE_CUDA()
}

#endif //#if AAA_USE_FLEX()