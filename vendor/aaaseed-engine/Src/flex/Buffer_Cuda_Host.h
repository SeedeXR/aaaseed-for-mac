#ifdef AAA_FLEX_BUFFER_CUDA_HOST_H
#error "FLEX/BUFFER_CUDA_HOST_H included more than once."
#endif
#define AAA_FLEX_BUFFER_CUDA_HOST_H 1


#ifndef AAA_FLEX_SDK_H
#	include "flex_sdk.h"
#endif

#if AAA_USE_FLEX()
#	ifndef AAA_OBJ_H
#		include "infrastructure/obj/obj.h"
#	endif

struct NvFlexBuffer;
struct NvFlexLibrary;

//todoso more check and no need to casttemplate by type
class c_flex_buffer_cuda_host final : public c_obj 
{
private:
	UINT32			_size;
	NvFlexBuffer*	_buffer_flex;
	void*			_buffer_data;

	void	release();

public:
	c_flex_buffer_cuda_host();
	~c_flex_buffer_cuda_host();


	void reset( NvFlexLibrary * CONST library, UINT32 CONST particles_nb, UINT32 CONST element_size, void CONST * CONST initial_data = nullptr );

	void copy( UINT32 CONST size );

	FINLINE NvFlexBuffer*	get_flex_buffer()	CONST	{	return _buffer_flex;	}
	FINLINE UINT32			get_size()			CONST	{	return _size;			}
	FINLINE void*			get_data()					{	return _buffer_data;	}

			void*			map();
			void			unmap();

	void readback_data();
	void readback_data( UINT32 CONST size );
};

#endif	//#if AAA_USE_FLEX()