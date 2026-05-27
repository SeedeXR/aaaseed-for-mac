#ifdef AAA_FLEX_PARTICLE_HOST_H
#error "FLEX/PARTICLE_HOST_H included more than once."
#endif
#define AAA_FLEX_PARTICLE_HOST_H 1


#ifndef AAA_FLEX_SDK_H
#	include "flex_sdk.h"
#endif


#if AAA_USE_FLEX()
#	ifndef AAA_OBJ_H
#		include "infrastructure/obj/obj.h"
#	endif

class c_flex_buffer_cuda_host;
struct NvFlexLibrary;
struct NvFlexSolver;

class c_particle_host final : public c_obj
{
private:
	UINT32						_particle_nb_max;
	UINT32						_particle_nb_cur;
	UINT32						_particle_nb_needed;

	c_flex_buffer_cuda_host*	_flex_particle_buffer;
	c_flex_buffer_cuda_host*	_flex_velocity_buffer;
	c_flex_buffer_cuda_host*	_flex_texcoord_buffer;
	c_flex_buffer_cuda_host*	_flex_active_buffer;
	c_flex_buffer_cuda_host*	_flex_phase_buffer;
	
	INT32*						_particle_to_rigid_index;
public:
	c_particle_host();
	~c_particle_host();

	void reset();
	void release();
	void set_size( NvFlexLibrary * library, UINT32 CONST particle_nb_max );
	void append( FP32 CONST * CONST position, FP32 CONST inv_mass , FP32 CONST * CONST velocity, UINT32 CONST phase, FP32 CONST * CONST texcoord );
	void send_to_solver( NvFlexSolver * solver );

	FINLINE UINT32 get_nb_max()				CONST	{	return _particle_nb_max;	}
	FINLINE UINT32 get_particle_nb()		CONST	{	return _particle_nb_cur;	}
	FINLINE UINT32 get_nb_needed()			CONST	{	return _particle_nb_needed;	}
	FINLINE UINT32 is_overflow()			CONST	{	return _particle_nb_needed >= _particle_nb_max;	}

	FINLINE c_flex_buffer_cuda_host * get_particle_buffer() CONST {	return _flex_particle_buffer;	}
	FINLINE c_flex_buffer_cuda_host*  get_texcoord_buffer() CONST { return _flex_texcoord_buffer;   }
	FINLINE INT32* get_particle_to_rigid_data()		CONST	{	return _particle_to_rigid_index;	}
};

#endif	//#if AAA_USE_FLEX()
