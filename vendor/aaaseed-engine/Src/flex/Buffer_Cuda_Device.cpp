#include "Buffer_Cuda_Device.h"


#if AAA_USE_FLEX()

c_flex_buffer_cuda_device::c_flex_buffer_cuda_device()
	:_buffer_flex(nullptr)
{
}

c_flex_buffer_cuda_device::~c_flex_buffer_cuda_device()
{
	release();
}


void c_flex_buffer_cuda_device::release()
{
	if( _buffer_flex )
	{
		NvFlexFreeBuffer( _buffer_flex );
		_buffer_flex = nullptr;
	}
}

void c_flex_buffer_cuda_device::reset( NvFlexLibrary* library, UINT32 particle_nb, UINT32 element_size )
{
	if( library )
	{
		release();

		 //pass library
		_buffer_flex = NvFlexAllocBuffer( library, particle_nb, element_size, NvFlexBufferType::eNvFlexBufferDevice );
	}
}

#endif //#if AAA_USE_FLEX()