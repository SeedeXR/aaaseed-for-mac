#include "Buffer_Cuda_Host.h"

#if AAA_USE_FLEX()

#	include "aaa_mem.h"


c_flex_buffer_cuda_host::c_flex_buffer_cuda_host()
	:_size(0)
	,_buffer_flex(nullptr)
	,_buffer_data(nullptr)
{
}

c_flex_buffer_cuda_host::~c_flex_buffer_cuda_host()
{
	release();
}

void* c_flex_buffer_cuda_host::map()
{
	return NvFlexMap( _buffer_flex, eNvFlexMapWait );
}
void c_flex_buffer_cuda_host::unmap()
{
	NvFlexUnmap( _buffer_flex );
}

void c_flex_buffer_cuda_host::readback_data()
{
	readback_data( _size );
}

void c_flex_buffer_cuda_host::readback_data( UINT32 CONST size )
{
	if( _buffer_data && size <= _size )
	{
		void* mapped_data = map();

			MEMCPY( _buffer_data, mapped_data, size, __FUNCTION__ );

		unmap();
	}
}

void c_flex_buffer_cuda_host::release()
{
	if( _buffer_flex )
	{
		NvFlexFreeBuffer(_buffer_flex);
		_buffer_flex = nullptr;
	}
	IF_FREE_AND_NULL( _buffer_data );
}

void c_flex_buffer_cuda_host::reset( NvFlexLibrary * CONST library, UINT32 CONST particle_nb, UINT32 CONST element_size, void CONST * CONST initial_data )
{
	release();

	 //pass library
	_buffer_flex = NvFlexAllocBuffer( library, particle_nb, element_size, NvFlexBufferType::eNvFlexBufferHost );

	//create cpu accessible data
	_buffer_data = MALLOC( particle_nb * element_size );
	_size = particle_nb * element_size;

	if( initial_data )
	{
		void* mapped_data = map();
		MEMCPY( mapped_data, initial_data, _size, __FUNCTION__ );
		unmap();
	}
}

void c_flex_buffer_cuda_host::copy( UINT32 CONST size )
{
	if( _buffer_data && size <= _size )
	{
		void* mapped_data = map();
		MEMCPY( mapped_data, _buffer_data, size, __FUNCTION__ );
		unmap();
	}
}

#endif //#if AAA_USE_FLEX()