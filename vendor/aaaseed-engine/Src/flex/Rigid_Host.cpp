#include "Rigid_Host.h"

#if AAA_USE_FLEX()
#	include "spy.h"
#	include "aaa_mem.h"
#	include "Buffer_Cuda_Host.h"
#	include "math/v.h"
#	include "Particle_Host.h"
#	include "err.h"



c_rigid_host::c_rigid_host()
	:_rigid_nb_max(0)
	, _rigid_indices_nb_max(0)
	, _rigid_nb_cur(0)
	, _rigid_indices_nb_cur(0)
	, _rigid_indices_nb_needed(0)
	, _rigid_nb_needed(0)
	, _flex_translation_buffer(nullptr)
	, _flex_rotation_buffer(nullptr)
	, _flex_stiffness_buffer(nullptr)
	, _flex_offsets_buffer(nullptr)
	, _flex_indices_buffer(nullptr)
	, _flex_rest_position_buffer(nullptr)
	, _size_tags(nullptr)

{
}

c_rigid_host::~c_rigid_host()
{
	release();
}

void c_rigid_host::release()
{
	SAFE_DELETE(_flex_translation_buffer);
	SAFE_DELETE(_flex_rotation_buffer);
	SAFE_DELETE(_flex_stiffness_buffer);
	SAFE_DELETE(_flex_offsets_buffer);
	SAFE_DELETE(_flex_indices_buffer);
	SAFE_DELETE(_flex_rest_position_buffer);

	IF_FREE_AND_NULL(_size_tags);

}

void c_rigid_host::reset()
{
	_rigid_nb_cur = 0;
	_rigid_indices_nb_cur = 0;

	_rigid_nb_needed = 0;
	_rigid_indices_nb_needed = 0;
}

void c_rigid_host::set_size( NvFlexLibrary* library, UINT32 CONST rigid_nb_max, UINT32 CONST rigid_indices_nb_max )
{
	if( _rigid_nb_max != rigid_nb_max || _rigid_indices_nb_max != rigid_indices_nb_max )
	{
		_rigid_nb_max = rigid_nb_max;
		_rigid_indices_nb_max = rigid_indices_nb_max;
		release();

		if( _rigid_nb_max > 0 && _rigid_indices_nb_max > 0 )
		{
			_flex_translation_buffer = new c_flex_buffer_cuda_host();
			_flex_translation_buffer->reset(library, _rigid_nb_max, 3 * sizeof(FP32));

			_flex_rotation_buffer = new c_flex_buffer_cuda_host();
			_flex_rotation_buffer->reset(library, _rigid_nb_max, 4 * sizeof(FP32));

			_flex_stiffness_buffer = new c_flex_buffer_cuda_host();
			_flex_stiffness_buffer->reset(library, _rigid_nb_max, sizeof(FP32));

			//offset rigid + 1
			_flex_offsets_buffer = new c_flex_buffer_cuda_host();
			_flex_offsets_buffer->reset(library, _rigid_nb_max + 1, sizeof(UINT32));

			//per indices
			_flex_indices_buffer = new c_flex_buffer_cuda_host();
			_flex_indices_buffer->reset(library, _rigid_indices_nb_max, sizeof(UINT32));

			_flex_rest_position_buffer = new c_flex_buffer_cuda_host();
			_flex_rest_position_buffer->reset(library, _rigid_indices_nb_max, 3 * sizeof(FP32));

			_size_tags = (st_size_tag*)MALLOC(_rigid_nb_max * sizeof(st_size_tag));
		}
	}
}

void c_rigid_host::append( st_flex_rigid_desc CONST* rigid_desc )
{
	if( _rigid_nb_cur < _rigid_nb_max )
	{
		//check for over on points
		if( _rigid_indices_nb_cur + rigid_desc->_points_nb < _rigid_indices_nb_max )
		{
			FP32* pt_translation = (FP32*)_flex_translation_buffer->get_data();
			FP32* pt_rotation = (FP32*)_flex_rotation_buffer->get_data();
			FP32* pt_stiffness = (FP32*)_flex_stiffness_buffer->get_data();
			UINT32* pt_offsets = (UINT32*)_flex_offsets_buffer->get_data();
			FP32* pt_rest_positions = (FP32*)_flex_rest_position_buffer->get_data();
			UINT32* pt_indices = (UINT32*)_flex_indices_buffer->get_data();

			//copy indices first and increment
			for (UINT32 i = 0; i < rigid_desc->_points_nb; ++i)
			{
				cpy_v3(pt_rest_positions + _rigid_indices_nb_cur * 3, rigid_desc->_pt_points[i]._position);
				pt_indices[_rigid_indices_nb_cur] = rigid_desc->_pt_points[i]._index;

				++_rigid_indices_nb_cur;
			}

			cpy_v3(pt_translation + _rigid_nb_cur * 3, rigid_desc->_position);
			cpy_v4(pt_rotation + _rigid_nb_cur * 4, rigid_desc->_orientation);

			pt_stiffness[_rigid_nb_cur] = rigid_desc->_stiffness;

			//special case for offset, if first rigid we set the item at 0 too
			if (_rigid_nb_cur == 0)
				pt_offsets[0] = 0;

			pt_offsets[_rigid_nb_cur + 1] = _rigid_indices_nb_cur;

			_size_tags[_rigid_nb_cur]._tag = rigid_desc->_tag;
			_size_tags[_rigid_nb_cur]._first_idx = rigid_desc->_pt_points[0]._index;

			++_rigid_nb_cur;
		}
		else
			ERR_PRINT_STRING("%s() Maximum rigid indices nb %d reached : Skipping", __FUNCTION__, _rigid_indices_nb_max);
	}
	else
		ERR_PRINT_STRING( "%s() Maximum rigid nb %d reached : Skipping", __FUNCTION__, _rigid_nb_max );
}

bool c_rigid_host::validate( c_particle_host* particle_host )
{
	if( _rigid_indices_nb_cur > 0 )
	{
		bool b_valid = true;
		UINT32* pt_indices = (UINT32*)_flex_indices_buffer->get_data();

		for( UINT32 i = 0; i < _rigid_indices_nb_cur; ++i )
		{
			UINT32 index = pt_indices[i];
			if( index >= particle_host->get_nb_max() )
			{
				ERR_PRINT_STRING( "rigid index overflow   ( from )   :    %i.", index);
				ERR_PRINT_STRING( "rigid index overflow   ( max  )   :    %i.", particle_host->get_nb_max());
				b_valid = false;
			}
		}
		return b_valid;
	}
	else
		return true;
}

void c_rigid_host::build_local_rest_positions(c_particle_host* particle_host)
{
	//set all particles indices to -1



	if (_rigid_nb_cur == 0)
		return;
	//first compute particle buffer center
	//FP32 particles_center[3];
	//clear_v3( particles_center );

	//not sure we really want to use that
	/*{
		FP32* pt_particle_position =	(FP32*)particle_host->get_particle_buffer()->get_data();
		UINT32 particle_nb = particle_host->get_particle_nb();

		for( UINT32 i = 0 ; i < particle_nb ; ++ i)
			add_v3( particles_center, pt_particle_position + i * 4 );

		FLOAT inv_nb = 1.0f / (FP32)particle_nb;
		scale_v3( particles_center, inv_nb );
	}*/

	//calculate rigid center of mass
	{
		UINT32* pt_offsets = (UINT32*)_flex_offsets_buffer->get_data();
		UINT32* pt_indices = (UINT32*)_flex_indices_buffer->get_data();
		//get particles in world space
		FP32* pt_particle_position = (FP32*)particle_host->get_particle_buffer()->get_data();
		//rigid positions
		FP32* pt_rigid_local_positions = (FP32*)_flex_rest_position_buffer->get_data();
		FP32* pt_translation = (FP32*)_flex_translation_buffer->get_data();

		INT32* pt_rigid_index = particle_host->get_particle_to_rigid_data();

		for (UINT32 i = 0; i < _rigid_nb_cur; i++)
		{
			UINT32 idx_start = pt_offsets[i];
			UINT32 idx_end = pt_offsets[i + 1];

			FP32 center_mass[3];
			clear_v3(center_mass);

			FP32 min_bounds[3];
			set_v3(min_bounds, FLT_MAX);

			FP32 max_bounds[3];
			set_v3(max_bounds, -FLT_MAX);


			INT32 indices_nb = idx_end - idx_start;

			//calculate center of mass
			for (UINT32 j = idx_start; j < idx_end; ++j)
			{
				UINT32 idx = pt_indices[j];

				FP32 local_position[3];
				//set_v3( &local_position[0], pt_particle_position + idx * 4);
				// 
				// 
				//sub_v3( local_position, pt_particle_position + idx * 4, particles_center );

				local_position[0] = pt_particle_position[idx * 4];
				local_position[1] = pt_particle_position[idx * 4 + 1];
				local_position[2] = pt_particle_position[idx * 4 + 2];

				add_v3(center_mass, local_position);

				min_v3(min_bounds, local_position);
				max_v3(max_bounds, local_position);
			}

			FLOAT inv_nb = 1.0f / (FP32)indices_nb;
			scale_v3(center_mass, inv_nb);

			//scale now in max bounds
			sub_v3(max_bounds, min_bounds);

			st_size_tag* st = &_size_tags[i];

			cpy_v3(&st->_size[0], max_bounds);

			//add_v3( center_mass, particles_center );

			//store center of mass as translation vector
			cpy_v3(pt_translation + _rigid_nb_cur * 3, center_mass);

			//now store the local rest positions in local space 
			for (UINT32 j = idx_start; j < idx_end; ++j)
			{
				UINT32 idx = pt_indices[j];
				sub_v3(pt_rigid_local_positions + j * 3, pt_particle_position + idx * 4, center_mass);

				//also set the particle rigid id 
				pt_rigid_index[idx] = i;
			}
		}
	}
}

void c_rigid_host::send_to_solver(NvFlexSolver* solver)
{
	if (_rigid_nb_max > 0 && _rigid_nb_cur > 0 && _rigid_indices_nb_max > 0 && _rigid_indices_nb_cur > 0)
	{
		_flex_translation_buffer->copy(_rigid_nb_cur * sizeof(FP32) * 3);
		_flex_rotation_buffer->copy(_rigid_nb_cur * sizeof(FP32) * 4);
		_flex_stiffness_buffer->copy(_rigid_nb_cur * sizeof(FP32));

		UINT32 offset_nb = _rigid_nb_cur + 1;
		_flex_offsets_buffer->copy(offset_nb * sizeof(UINT32));

		_flex_indices_buffer->copy(_rigid_indices_nb_cur * sizeof(UINT32));
		_flex_rest_position_buffer->copy(_rigid_indices_nb_cur * sizeof(FP32) * 3);

		NvFlexSetRigids(solver
			, _flex_offsets_buffer->get_flex_buffer()
			, _flex_indices_buffer->get_flex_buffer()
			, _flex_rest_position_buffer->get_flex_buffer()
			, nullptr // rest normals if we want later
			, _flex_stiffness_buffer->get_flex_buffer()
			, nullptr //thresholds for plastc later if we want
			, nullptr //plastic deformation (creep) later if we want
			, _flex_rotation_buffer->get_flex_buffer()
			, _flex_translation_buffer->get_flex_buffer()
			, _rigid_nb_cur
			, _rigid_indices_nb_cur);
  }
}

#endif //#if AAA_USE_FLEX()