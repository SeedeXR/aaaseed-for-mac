#include "Emitter_Shapes.h"

#if AAA_USE_FLEX()
#	include "spy.h"
#	include "math/v.h"
#	include "Particle_Host.h"
#	include "Triangle_Host.h"
#	include "Spring_Host.h"
#	include "Rigid_Host.h"


void flex_create_grid( c_particle_host * particle_host, c_rigid_host * rigid_host, st_flex_rigid_box_creation_info CONST * pt_info, FP32* mask )
{
	//get current index to allow building springs and triangles later
	UINT32 CONST current_index = particle_host->get_particle_nb();

	st_flex_rigid_desc rigid_desc = {};

	UINT32 pt_nb = pt_info->_dim[0] * pt_info->_dim[1] * pt_info->_dim[2];

	st_flex_rigid_point_desc* pt_indices = (st_flex_rigid_point_desc*)MALLOC(pt_nb * sizeof(st_flex_rigid_point_desc));


	rigid_desc._pt_points = pt_indices;

	UINT32 index_cur = 0;
	FP32 pos[3];
	FP32 position[3];

	FP32 inv_offset[3];
	inv_offset[0] = (float)pt_info->_dim[0] * pt_info->_spacing * 0.5f;
	inv_offset[1] = (float)pt_info->_dim[1] * pt_info->_spacing * 0.5f;
	inv_offset[2] = (float)pt_info->_dim[2] * pt_info->_spacing * 0.5f;

  UINT32 maskidx =0;

	for( UINT32 x = 0; x < pt_info->_dim[0] ; ++x )
	{
		pos[0] = x * pt_info->_spacing;

	  for( UINT32 y = 0 ; y < pt_info->_dim[1] ; ++y )
	  {
				pos[1] = y * pt_info->_spacing;

				bool pass = true;

				if (mask)
				{
					pass = mask[maskidx] == 1.0f;
				}

				if (pass)
				{
					for (UINT32 z = 0; z < pt_info->_dim[2]; ++z)
					{
						pos[2] = z * pt_info->_spacing;
						add_v3(position, pos, pt_info->_center);

						sub_v3(position, inv_offset);

						if (pt_info->_rigid)
						{
							UINT32 current_idx = particle_host->get_particle_nb();
							//we only care about index for now, local position is auto calculated
							pt_indices[index_cur]._index = current_idx;
						}

						++index_cur;

						//push particle
						particle_host->append(position, pt_info->_mass_inv, pt_info->_velocity, pt_info->_phase, nullptr);
					}
				}
				maskidx++;
		}
	}

	rigid_desc._points_nb = index_cur;

	if( pt_info->_rigid && rigid_host && rigid_desc._points_nb > 0)
	{
		clear_v3( rigid_desc._orientation );
		rigid_desc._orientation[3] = 1.0f;
		rigid_desc._auto_build_locals = true;
		rigid_desc._stiffness = pt_info->_stiffness;
		rigid_desc._tag = pt_info->_tag;

		rigid_host->append( &rigid_desc );
	}

	IF_FREE_AND_NULL( pt_indices );
}

void flex_create_sphere( c_particle_host * particle_host, c_rigid_host * rigid_host, st_flex_rigid_sphere_creation_info CONST * pt_info )
{
	//get current index to allow building springs and triangles later
	UINT32 CONST current_index = particle_host->get_particle_nb();

	st_flex_rigid_desc rigid_desc = {};

	UINT32 pt_nb_max = pt_info->_dim * pt_info->_dim * pt_info->_dim;
	FP32 fdim = (FP32)pt_info->_dim;

	FP32 dist_cmp_value = (FP32)(pt_info->_dim * pt_info->_dim) * 0.25f;

	st_flex_rigid_point_desc* pt_indices = (st_flex_rigid_point_desc*)MALLOC(pt_nb_max * sizeof(st_flex_rigid_point_desc));

	UINT32 index_cur = 0;
	FP32 pos[3];
	FP32 position[3];

	FP32 inv_offset[3];

	float foffset = fdim * pt_info->_spacing * 0.5f;
	set_v3( inv_offset, foffset );

	for( UINT32 x = 0; x < pt_info->_dim ; ++x )
	{
		FP32 sx2 = x - fdim * 0.5f;
		sx2 *= sx2;
		pos[0] = x * pt_info->_spacing;
	    for( UINT32 y = 0 ; y < pt_info->_dim ; ++y )
	    {
			FP32 sxy2 = y - fdim * 0.5f;
			sxy2 = sx2 + sxy2 * sxy2;
			pos[1] = y * pt_info->_spacing;
			for( UINT32 z = 0 ; z < pt_info->_dim ; ++z )
	        {
				FP32 sz = z - fdim * 0.5f;
				if( sxy2 + sz * sz <= dist_cmp_value )
				{
					pos[2] = z * pt_info->_spacing;
					add_v3( position, pos, pt_info->_center );

					sub_v3( position, inv_offset );

					if( pt_info->_rigid )
					{
						UINT32 current_idx = particle_host->get_particle_nb();
						//we only care about index for now, local position is auto calculated
						pt_indices[ index_cur ]._index = current_idx;
					}
				
					++index_cur;

					//push particle
					particle_host->append( position, pt_info->_mass_inv, pt_info->_velocity, pt_info->_phase, nullptr );
				}
			}
		}
	}

	if( pt_info->_rigid && rigid_host )
	{
		set_v4( rigid_desc._orientation, .0,.0,.0, 1. );
		rigid_desc._points_nb = index_cur;
		rigid_desc._pt_points = pt_indices;
		rigid_desc._auto_build_locals = true;
		rigid_desc._stiffness = pt_info->_stiffness;
		rigid_desc._tag = pt_info->_tag;

		rigid_host->append( &rigid_desc );
	}

	IF_FREE_AND_NULL( pt_indices );
}

void flex_create_cylinder( c_particle_host * particle_host, c_rigid_host * rigid_host, st_flex_rigid_cylinder_creation_info CONST * pt_info )
{
	//get current index to allow building springs and triangles later
	UINT32 CONST current_index = particle_host->get_particle_nb();

	st_flex_rigid_desc rigid_desc = {};

	UINT32 pt_nb_max = pt_info->_dim * pt_info->_dim * pt_info->_dim_z;
	FP32 fdim = (FP32)pt_info->_dim;

	FP32 dist_cmp_value = (FP32)(pt_info->_dim * pt_info->_dim) * 0.25f;

	st_flex_rigid_point_desc* pt_indices = (st_flex_rigid_point_desc*)MALLOC(pt_nb_max * sizeof(st_flex_rigid_point_desc));

	UINT32 index_cur = 0;
	FP32 pos[3];
	FP32 position[3];

	FP32 inv_offset[3];
	inv_offset[0] = fdim * pt_info->_spacing *0.5f;
	inv_offset[1] = fdim * pt_info->_spacing * 0.5f;
	inv_offset[2] = (float)pt_info->_dim_z*pt_info->_spacing*0.5f;

	for( UINT32 x = 0; x < pt_info->_dim ; ++x )
	{
		FP32 sx2 = x - fdim * 0.5f;
		sx2 *= sx2;
		if( sx2 <= dist_cmp_value )
		{
			pos[0] = x * pt_info->_spacing;
			for( UINT32 y = 0 ; y < pt_info->_dim ; ++y )
			{
				FP32 sxy2 = y - fdim * 0.5f;
				sxy2 = sx2 + sxy2 * sxy2;
				if( sxy2 <= dist_cmp_value )
				{
					pos[1] = y * pt_info->_spacing;
					for( UINT32 z = 0 ; z < pt_info->_dim_z ; ++z )
					{
						pos[2] = z * pt_info->_spacing;

						add_v3( position, pos, pt_info->_center );
						sub_v3( position, inv_offset );

						if( pt_info->_rigid )
						{
							UINT32 current_idx = particle_host->get_particle_nb();
							//we only care about index for now, local position is auto calculated
							pt_indices[ index_cur ]._index = current_idx;
						}
				
						++index_cur;
						//push particle
						particle_host->append( position, pt_info->_mass_inv, pt_info->_velocity, pt_info->_phase, nullptr );
					}
				}
			}
		}
	}

	if( pt_info->_rigid && rigid_host )
	{
		set_v4( rigid_desc._orientation, .0,.0,.0, 1. );
		rigid_desc._points_nb = index_cur;
		rigid_desc._pt_points = pt_indices;
		rigid_desc._auto_build_locals = true;
		rigid_desc._stiffness = pt_info->_stiffness;
		rigid_desc._tag = pt_info->_tag;

		rigid_host->append( &rigid_desc );
	}

	IF_FREE_AND_NULL( pt_indices );
}

#endif //#if AAA_USE_FLEX()
