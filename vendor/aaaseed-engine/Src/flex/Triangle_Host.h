#ifdef AAA_FLEX_TRIANGLE_HOST_H
#error "FLEX/TRIANGLE_HOST_H included more than once."
#endif
#define AAA_FLEX_TRIANGLE_HOST_H 1


#ifndef AAA_FLEX_SDK_H
#	include "flex_sdk.h"
#endif

#if AAA_USE_FLEX()
#	ifndef AAA_OBJ_H
#		include "infrastructure/obj/obj.h"
#	endif

class c_flex_buffer_cuda_host;
class c_particle_host;
struct NvFlexLibrary;
struct NvFlexSolver;

class c_triangle_host final : public c_obj
{
private:
	UINT32						_triangle_nb_max;
	UINT32						_triangle_nb_cur;
	UINT32						_triangle_nb_needed;

	c_flex_buffer_cuda_host*	_flex_indices_buffer;
	c_flex_buffer_cuda_host*	_flex_normal_buffer;

public:
	c_triangle_host();
	~c_triangle_host();

	void reset();
	void release();
	void set_size( NvFlexLibrary * library, UINT32 CONST triangle_nb_max );
	void append( UINT32 CONST i1, UINT32 CONST i2, UINT32 CONST i3, FP32 CONST * CONST  normal );

	bool validate ( c_particle_host * particle_host );
	void send_to_solver( NvFlexSolver * solver );

	FINLINE UINT32 get_nb()			CONST	{	return _triangle_nb_cur;	}
	FINLINE UINT32 get_nb_max()		CONST	{	return _triangle_nb_max;	}
	FINLINE UINT32 get_nb_needed()	CONST	{	return _triangle_nb_needed;	}
	FINLINE UINT32 is_overflow()	CONST	{	return _triangle_nb_needed >= _triangle_nb_max;	}

	void* get_indices_data() CONST;
};

#endif	//#if AAA_USE_FLEX()
