#include "Triangle_Host.h"


#if AAA_USE_FLEX()

#	include "spy.h"
#	include "aaa_mem.h"
#	include "Buffer_Cuda_Host.h"
#	include "math/v.h"
#	include "Particle_Host.h"
#	include "err.h"


c_triangle_host::c_triangle_host()
	:_triangle_nb_max		(0)
	,_triangle_nb_cur		(0)
	,_flex_indices_buffer	(nullptr)
	,_flex_normal_buffer	(nullptr)
{
}

c_triangle_host::~c_triangle_host()
{
	release();
}

void c_triangle_host::release()
{
	SAFE_DELETE( _flex_indices_buffer );
	SAFE_DELETE( _flex_normal_buffer );
}

void c_triangle_host::reset()
{
	_triangle_nb_cur = 0;
	_triangle_nb_needed = 0;
}

void c_triangle_host::set_size( NvFlexLibrary * library, UINT32 CONST triangle_nb_max )
{
	if( _triangle_nb_max != triangle_nb_max )
	{
		_triangle_nb_max = triangle_nb_max;
		release();

		if( _triangle_nb_max > 0 )
		{
			_flex_indices_buffer = new c_flex_buffer_cuda_host();
			_flex_indices_buffer->reset( library, _triangle_nb_max, 3*sizeof(UINT32) );

			_flex_normal_buffer = new c_flex_buffer_cuda_host();
			_flex_normal_buffer ->reset( library, _triangle_nb_max, 3*sizeof(FP32) );
		}
	}
}

void c_triangle_host::append( UINT32 CONST i1, UINT32 CONST i2, UINT32 CONST i3, FP32 CONST * CONST normal )
{
	//TODO: add log if buffer is full
	if( _triangle_nb_cur < _triangle_nb_max )
	{
		UINT32*	 pt_indices = (UINT32*)_flex_indices_buffer->get_data();
		FP32* pt_normal  = (FP32*)_flex_normal_buffer->get_data();

		pt_indices += _triangle_nb_cur * 3;
		set_v3( pt_indices, i1,i2,i3 );

		cpy_v3( pt_normal + _triangle_nb_cur * 3, normal );

		++_triangle_nb_cur;
	}
}

bool c_triangle_host::validate ( c_particle_host * particle_host )
{
	if( _triangle_nb_cur > 0 )
	{
		bool b_valid = true;
		//use a fully built particle host, walks through the indices and build rest lengths accordingly
		UINT32*		pt_indices		= (UINT32*)_flex_indices_buffer->get_data();

		UINT32 indices_nb = _triangle_nb_cur * 3;

		for( UINT32 i = 0 ; i < indices_nb; i++)
		{
			UINT32 index = pt_indices[ i ];

			if( index >= particle_host->get_nb_max() )
			{
				ERR_PRINT_STRING("triangle overflow   ( from )   :    %i.", index);
				ERR_PRINT_STRING("triangle overflow   ( max  )   :    %i.", particle_host->get_nb_max());
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

void c_triangle_host::send_to_solver(NvFlexSolver* solver)
{
	if( _triangle_nb_max > 0 && _triangle_nb_cur > 0 )
	{
		_flex_indices_buffer->copy( _triangle_nb_cur * sizeof(UINT32) * 3 );
		_flex_normal_buffer->copy(  _triangle_nb_cur * sizeof(FP32) * 3 );

		NvFlexSetDynamicTriangles( solver, _flex_indices_buffer->get_flex_buffer(), _flex_normal_buffer->get_flex_buffer(), _triangle_nb_cur );
	}
}

void* c_triangle_host::get_indices_data() CONST
{
	return _flex_indices_buffer->get_data();
}

#endif //#if AAA_USE_FLEX()
