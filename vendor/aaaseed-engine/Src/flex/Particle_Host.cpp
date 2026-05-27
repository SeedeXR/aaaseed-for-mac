#include "Particle_Host.h"



#if AAA_USE_FLEX()
#	include "spy.h"
#	include "aaa_mem.h"
#	include "Buffer_Cuda_Host.h"
#	include "math/v.h"

c_particle_host::c_particle_host()
	:_particle_nb_max			(0)
	,_particle_nb_needed		(0)
	,_particle_nb_cur			(0)
	,_flex_particle_buffer		(nullptr)
	,_flex_velocity_buffer		(nullptr)
	,_flex_texcoord_buffer		(nullptr)
	,_flex_active_buffer		(nullptr)
	,_flex_phase_buffer			(nullptr)
	,_particle_to_rigid_index	(nullptr)
{
}

c_particle_host::~c_particle_host()
{
	release();
}

void c_particle_host::release()
{
	SAFE_DELETE( _flex_particle_buffer );
	SAFE_DELETE( _flex_velocity_buffer );
	SAFE_DELETE( _flex_texcoord_buffer );
	SAFE_DELETE( _flex_active_buffer );
	SAFE_DELETE( _flex_phase_buffer );

	IF_FREE_AND_NULL( _particle_to_rigid_index	);
}

void c_particle_host::reset()
{
    _particle_nb_cur = 0;
	_particle_nb_needed = 0;
}

void c_particle_host::set_size( NvFlexLibrary * library, UINT32 CONST particle_nb_max )
{
	if( _particle_nb_max != particle_nb_max )
	{
		_particle_nb_max = particle_nb_max;
		release();

		if( _particle_nb_max > 0 )
		{
			_flex_particle_buffer = new c_flex_buffer_cuda_host();
			_flex_particle_buffer	->reset( library, _particle_nb_max , 4*sizeof(FP32) );

			_flex_velocity_buffer = new c_flex_buffer_cuda_host();
			_flex_velocity_buffer	->reset( library, _particle_nb_max , 3*sizeof(FP32) );

			_flex_texcoord_buffer = new c_flex_buffer_cuda_host();
			_flex_texcoord_buffer	->reset( library, _particle_nb_max , 2*sizeof(FP32) );

			_flex_active_buffer = new c_flex_buffer_cuda_host();
			_flex_active_buffer		->reset( library, _particle_nb_max , sizeof(UINT32) );

			_flex_phase_buffer = new c_flex_buffer_cuda_host();
			_flex_phase_buffer		->reset( library, _particle_nb_max , sizeof(UINT32) );

			_particle_to_rigid_index = ( INT32 *) MALLOC ( _particle_nb_max * sizeof ( INT32 ) );
		}
	}
}

void c_particle_host::append( FP32 CONST * CONST position, FP32 CONST inv_mass , FP32 CONST * CONST velocity, UINT32 CONST phase, FP32 CONST * CONST texcoord )
{
	++_particle_nb_needed;
	//TODO: add log if buffer is full ERR_PRINT_STRING()
	if( _particle_nb_cur < _particle_nb_max )
	{
		//todo remove glm
		FP32*	pt_particle	= (FP32*)_flex_particle_buffer->get_data();
		FP32*	pt_velocity	= (FP32*)_flex_velocity_buffer->get_data();
		FP32*	pt_texcoord	= (FP32*)_flex_texcoord_buffer->get_data();
		UINT32*	pt_active	= (UINT32*)_flex_active_buffer->get_data();
		UINT32*	pt_phase	= (UINT32*)_flex_phase_buffer->get_data();

		pt_particle += 4 * _particle_nb_cur;
		cpy_v3( pt_particle, position );
		pt_particle[3] = inv_mass;
		cpy_v3( pt_velocity + 3 * _particle_nb_cur, velocity );

		if( texcoord )
			cpy_v2( pt_texcoord + 2 * _particle_nb_cur, texcoord );
		
		pt_phase[    _particle_nb_cur ] = phase;
		pt_active[   _particle_nb_cur ] = _particle_nb_cur;

		//not assigned to a rigid yet
		_particle_to_rigid_index[ _particle_nb_cur ] = -1;

		++_particle_nb_cur;
	}
}

void c_particle_host::send_to_solver(NvFlexSolver* solver)
{
	if( _particle_nb_max > 0 && _particle_nb_cur > 0 )
	{
		_flex_particle_buffer->copy( _particle_nb_cur  * sizeof(FP32) * 4 );
		_flex_velocity_buffer->copy( _particle_nb_cur  * sizeof(FP32) * 3 );
		_flex_texcoord_buffer->copy( _particle_nb_cur  * sizeof(FP32) * 2 );
		_flex_active_buffer->copy(   _particle_nb_cur  * sizeof(UINT32)    );
		_flex_phase_buffer->copy(    _particle_nb_cur  * sizeof(UINT32)    );
	}

	//note, we could use copy desc if it's working in gl version
	NvFlexSetParticles(		solver, _flex_particle_buffer->get_flex_buffer(),	nullptr );
	NvFlexSetRestParticles(	solver, _flex_particle_buffer->get_flex_buffer(),	nullptr );
	NvFlexSetVelocities(	solver, _flex_velocity_buffer->get_flex_buffer(),	nullptr );
	NvFlexSetActive(		solver, _flex_active_buffer->get_flex_buffer(),		nullptr );
	NvFlexSetPhases(		solver, _flex_phase_buffer->get_flex_buffer(),		nullptr );

	NvFlexSetActiveCount(	solver, _particle_nb_cur );
}

#endif //#if AAA_USE_FLEX()