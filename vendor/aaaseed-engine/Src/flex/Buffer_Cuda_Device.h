#ifdef AAA_FLEX_BUFFER_CUDA_DEVICE_H
#error "FLEX/BUFFER_CUDA_DEVICE_H included more than once."
#endif
#define AAA_FLEX_BUFFER_CUDA_DEVICE_H 1


#ifndef AAA_FLEX_SDK_H
#	include "flex_sdk.h"
#endif
#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif


#if AAA_USE_FLEX()

struct NvFlexLibrary;
struct NvFlexBuffer;

class c_flex_buffer_cuda_device
{
private:
//unused
//	INT32			_size;
	NvFlexBuffer*	_buffer_flex;

	void release();

public:
	c_flex_buffer_cuda_device();
	~c_flex_buffer_cuda_device();

	void reset( NvFlexLibrary * library, UINT32 particles_nb, UINT32 element_size );
	FINLINE NvFlexBuffer* get_flex_buffer() CONST	{	return _buffer_flex;	}
};

#endif //#if AAA_USE_FLEX()