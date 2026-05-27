#ifdef AAA_FLEX_SPRING_HOST_H
#error "FLEX/SPRING_HOST_H included more than once."
#endif
#define AAA_FLEX_SPRING_HOST_H 1


#ifndef AAA_FLEX_SDK_H
#	include "flex_sdk.h"
#endif


#if AAA_USE_FLEX()
#	ifndef AAA_OBJ_H
#		include "infrastructure/obj/obj.h"
#	endif

class c_flex_buffer_cuda_host;
struct NvFlexLibrary;
class c_particle_host;
struct NvFlexSolver;

class c_spring_host final : public c_obj
{
private:
	UINT32						_spring_nb_max;
	UINT32						_spring_nb_cur;
	UINT32						_spring_nb_needed;

	c_flex_buffer_cuda_host*	_flex_indices_buffer;
	c_flex_buffer_cuda_host*	_flex_stiffness_buffer;
	c_flex_buffer_cuda_host*	_flex_rest_length_buffer;

	

public:
	c_spring_host();
	~c_spring_host();

	FINLINE UINT32 get_nb()			CONST	{	return _spring_nb_cur;	}
	FINLINE UINT32 get_nb_max()		CONST	{	return _spring_nb_max;	}
	FINLINE UINT32 get_nb_needed()	CONST	{	return _spring_nb_needed;	}
	FINLINE UINT32 is_overflow()	CONST	{	return _spring_nb_needed >= _spring_nb_max;	}

	void reset();
	void release();
	void set_size( NvFlexLibrary * library, UINT32 CONST spring_nb_max );
	void append(			UINT32 CONST from_idx, UINT32 CONST to_idx, FP32 CONST stiffness, FP32 CONST rest_length );
	void append_indices(	UINT32 CONST from_idx, UINT32 CONST to_idx, FP32 CONST stiffness );
	
	bool validate ( c_particle_host * particle_host );
	void build_rest_length( c_particle_host * particle_host );

	void send_to_solver( NvFlexSolver * solver );

	void* get_indices_data() CONST;	
};
#endif	//#if AAA_USE_FLEX()
