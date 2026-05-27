#include "Spring_Host.h"


#if AAA_USE_FLEX()
#	include "Particle_Host.h"
#	include "spy.h"
#	include "aaa_mem.h"
#	include "math/v.h"
#	include "Buffer_Cuda_Host.h"
#	include "err.h"




c_spring_host::c_spring_host()
	:_spring_nb_max				(0)
	,_spring_nb_cur				(0)	
	,_spring_nb_needed			(0)	
	,_flex_indices_buffer		(nullptr)
	,_flex_stiffness_buffer		(nullptr)
	,_flex_rest_length_buffer	(nullptr)
{
}

c_spring_host::~c_spring_host()
{
	release();
}

void c_spring_host::release()
{
	SAFE_DELETE( _flex_indices_buffer );
	SAFE_DELETE( _flex_stiffness_buffer );
	SAFE_DELETE( _flex_rest_length_buffer );
}

void c_spring_host::reset()
{
	_spring_nb_cur = 0;
	_spring_nb_needed = 0;
}

void c_spring_host::set_size( NvFlexLibrary * library, UINT32 CONST spring_nb_max )
{
	if( _spring_nb_max != spring_nb_max )
	{
		_spring_nb_max = spring_nb_max;
		release();

		if( _spring_nb_max > 0 )
		{
			_flex_indices_buffer = new c_flex_buffer_cuda_host();
			_flex_indices_buffer	->reset( library, _spring_nb_max*2,	sizeof(UINT32 ) );

			_flex_stiffness_buffer = new c_flex_buffer_cuda_host();
			_flex_stiffness_buffer	->reset( library, _spring_nb_max,	sizeof(FP32 ) );

			_flex_rest_length_buffer = new c_flex_buffer_cuda_host();
			_flex_rest_length_buffer->reset( library, _spring_nb_max,	sizeof(FP32 ) );
		}
	}
}


void c_spring_host::append( UINT32 CONST from_idx, UINT32 CONST to_idx, FP32 CONST stiffness, FP32 CONST rest_length )
{
	//TODO: add log if buffer is full
	if( _spring_nb_cur < _spring_nb_max )
	{
		UINT32*	pt_indices		= (UINT32*)_flex_indices_buffer    ->get_data();
		FP32*	pt_stiffness	= (FP32*)  _flex_stiffness_buffer  ->get_data();
		FP32*	pt_rest_length	= (FP32*)  _flex_rest_length_buffer->get_data();

		pt_indices		[ _spring_nb_cur * 2 ]		= from_idx;
		pt_indices		[ _spring_nb_cur * 2 + 1 ]	= to_idx;
		pt_stiffness	[ _spring_nb_cur ]			= stiffness;
		pt_rest_length	[ _spring_nb_cur ]			= rest_length;

		++_spring_nb_cur;
	}

	++_spring_nb_needed;
}

bool c_spring_host::validate ( c_particle_host * particle_host )
{
	if( _spring_nb_cur > 0 )
	{
		bool b_valid = true;
		//use a fully built particle host, walks through the indices and build rest lengths accordingly
		UINT32*		pt_indices		= (UINT32*)_flex_indices_buffer->get_data();
		for( UINT32 i = 0 ; i < _spring_nb_cur; i++ )
		{
			UINT32 from = pt_indices[ i * 2 ];
			UINT32 to   = pt_indices[ i * 2 + 1 ];
			if( from >= particle_host->get_nb_max() )
			{
				ERR_PRINT_STRING("spring overflow   (from)   :    %i.", from);
				ERR_PRINT_STRING("spring overflow   (max )   :    %i.", particle_host->get_nb_max());
				b_valid = false;
			}
			if( from >= particle_host->get_nb_max() )
			{
				ERR_PRINT_STRING("spring overflow   (to)     :    %i.", to);
				ERR_PRINT_STRING("spring overflow   (max )   :    %i.", particle_host->get_nb_max());
				b_valid = false;
			}
		}
		return b_valid;
	}
	else
	{
		return true;
	}
}

void c_spring_host::build_rest_length( c_particle_host * particle_host )
{
	if( _spring_nb_cur > 0 )
	{
		UINT32 valid_write_index = 0;

		//use a fully built particle host, walks through the indices and build rest lengths accordingly
		UINT32*		pt_indices		= (UINT32*)_flex_indices_buffer->get_data();
		FP32*	pt_rest_length	= (FP32*)_flex_rest_length_buffer->get_data();
		FP32*	pt_particles	= (FP32*)particle_host->get_particle_buffer()->get_data();
		FP32*	pt_stiffness	= (FP32*)_flex_stiffness_buffer->get_data();

		for( UINT32 i = 0 ; i < _spring_nb_cur; i++)
		{
			UINT32 from = pt_indices[i*2];
			UINT32 to = pt_indices[i*2+1];

			//bounds check here
			if( from < particle_host->get_nb_max() && to < particle_host->get_nb_max() )
			{
				FP32* p1 = pt_particles + 4 * from;
				FP32* p2 = pt_particles + 4 * to;
				FP32 d = dist_v3r( p1, p2 );

				pt_rest_length[ valid_write_index ] *= d;	// we put 1 first when we don't use it
				pt_indices[ valid_write_index * 2] = from;
				pt_indices[ valid_write_index * 2 + 1] = to;
				pt_stiffness[ valid_write_index ] = pt_stiffness[ i ];

				++valid_write_index;
			}
		}

		_spring_nb_cur = valid_write_index;
	}
}

void c_spring_host::append_indices( UINT32 CONST from_idx, UINT32 CONST to_idx, FP32 CONST stiffness )
{
	//put a rest length of 0, when we call that function we will build it later
	append( from_idx, to_idx, stiffness, 0.0f );
}

void c_spring_host::send_to_solver( NvFlexSolver* solver )
{
	if( _spring_nb_max > 0 && _spring_nb_cur > 0 )
	{
		_flex_indices_buffer->copy( _spring_nb_max * 2 * sizeof(UINT32) );
		_flex_stiffness_buffer->copy( _spring_nb_max * sizeof(FP32) );
		_flex_rest_length_buffer->copy( _spring_nb_max * sizeof(FP32) );

		NvFlexSetSprings(
			solver, 
			_flex_indices_buffer->get_flex_buffer(), 
			_flex_rest_length_buffer->get_flex_buffer(),
			_flex_stiffness_buffer->get_flex_buffer(), _spring_nb_cur
			);
	}
}

void* c_spring_host::get_indices_data() CONST
{
	return _flex_indices_buffer->get_data();
}

#endif //#if AAA_USE_FLEX()