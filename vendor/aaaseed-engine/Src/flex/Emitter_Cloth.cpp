#include "Emitter_Cloth.h"


#if AAA_USE_FLEX()
#	include "spy.h"
#	include "math/v.h"
#	include "Particle_Host.h"
#	include "Triangle_Host.h"
#	include "Spring_Host.h"
#	include "Buffer_Cuda_Host.h"


void flex_create_cloth_grid( INT32 CONST i_axe, c_particle_host * particle_host, c_spring_host * spring_host, c_triangle_host * triangles_host, st_flex_cloth_grid_creation_info CONST * CONST info )
{
	INT32	i_u;
	INT32	i_v;
	axe_build_index( i_u, i_v, i_axe );

	//get current index to allow building springs and triangles later
	UINT32 CONST current_index = particle_host->get_particle_nb();

	FP32 duv[2] = { FP32( OVER_ONE(info->nb_u) ), FP32( OVER_ONE(info->nb_v) ) };

	UINT32 CONST u_nb = info->nb_u;
	UINT32 CONST iu_last = u_nb - 1;
	UINT32 CONST iv_last = info->nb_v - 1;

	FP32 normal[3];
	normal[i_u] = 0.;
	normal[i_v] = 0.;
	normal[i_axe] = 1.;

	//build particles
	FP32 center[3];
	cpy_v3( center, info->center );
	FP32 uv[2];
	FP32 pos[3];	
	pos[i_axe] = center[i_axe]; 

	std::vector<INT32> anchor_indices;
	
	FP32 mass_inv;
	if( info->s_mass_mode == e_flex_mass_mode::INIT_MASS_DENSITY )
	{
		UINT32 particle_nb	= info->nb_u * info->nb_v;
		FP32 area		= info->size[0] * info->size[1];
		FP32 mass_total	= info->density * area;

		mass_inv = 1.0f / ( mass_total / (FP32)particle_nb );
	}
	else
		mass_inv = info->mass_inv;
	
	if( spring_host && !info->stretch.b_use && !info->bend.b_use && !info->shear.b_use )
		spring_host = nullptr;
			
	INT32 mod = info->grid_uv_modulo;
	if( mod <= 1 )
		mod = 0;

	FP32 stretch_length_factor	=  info->stretch.b_length_factor	? info->stretch.length_factor	: FP32(1.);
	FP32 bend_length_factor		=  info->bend.b_length_factor		? info->bend.length_factor		: FP32(1.);
	FP32 shear_length_factor	=  info->shear.b_length_factor		? info->shear.length_factor		: FP32(1.);
	FP32 tether_length_factor	=  info->tether.b_length_factor		? info->tether.length_factor	: FP32(1.);

	UINT32 pin_begin = info->pin_border_size - 1;
	UINT32 pin_end_u = iu_last - (info->pin_border_size - 1);
	UINT32 pin_end_v = iv_last - (info->pin_border_size - 1);

	for( UINT32 iv = 0; iv <= iv_last ; ++iv )
	{
		uv[1] = iv * duv[1];
		pos[i_v] = FP32(center[i_v] + (uv[1]-.5) * info->size[1]);
		
		for( UINT32 iu = 0; iu <= iu_last ; ++iu )
		{
			uv[0] = iu * duv[0];
			pos[i_u] = FP32(center[i_u] + (uv[0]-.5) * info->size[0]);
			
			FP32 inv_mass_part = mass_inv;
			//if none, just keep mass
			switch( info->s_pin_mode )
			{
				case e_flex_cloth_pin_mode::ALL_BORDERS:
					if( iv <= pin_begin || iu <= pin_begin || iv >= pin_end_v || iu >= pin_end_u )
						inv_mass_part = 0.0f;
					break;

				case e_flex_cloth_pin_mode::TOP_BORDER:
					if( iv >= pin_end_v )
						inv_mass_part = 0.0f;
					break;
				
				case e_flex_cloth_pin_mode::ALL_CORNERS:
					if( (iu <= pin_begin || iu >= pin_end_u) && (iv == 0 || iv >= pin_end_v) )
						inv_mass_part = 0.0f;
					break;
			}

			particle_host->append( pos, inv_mass_part, info->velocity, info->phase, uv );
			
			//add to anchor list
			if( inv_mass_part == 0.0f )
			{
				anchor_indices.push_back( particle_host->get_particle_nb() );
			}

			INT32 index = current_index + iv * u_nb + iu;
			if( triangles_host && iu > 0 && iv > 0 && info->b_triangle_use )
			{
				triangles_host->append(	index-u_nb-1, index-u_nb, index, normal );
				triangles_host->append( index-u_nb-1, index, index-1, normal );
			}

			if( spring_host )
			{		
				if( info->stretch.b_use )
				{
					if( iu > 0 && (!mod || iv % mod == 0) )
						spring_host->append( index, index-1,		info->stretch.stiffness,	stretch_length_factor	);
					if( iv > 0 && (!mod || iu % mod == 0) )
						spring_host->append( index, index-u_nb,		info->stretch.stiffness,	stretch_length_factor	);
				}
				if( info->bend.b_use )
				{
					if( iu > 1 && (!mod || iv % mod == 0) )
						spring_host->append( index, index-2,		info->bend.stiffness,		bend_length_factor	);
					if( iv > 1 && (!mod || iu % mod == 0) )
						spring_host->append( index, index-u_nb*2,	info->bend.stiffness,		bend_length_factor	);
				}
				if( info->shear.b_use )
				{
					if( iv > 0 && iu < iu_last )
						spring_host->append( index, index-u_nb+1,	info->shear.stiffness,		shear_length_factor	);
					if( iv > 0 && iu > 0 )
						spring_host->append( index, index-u_nb-1,	info->shear.stiffness,		shear_length_factor	);
				}
			}
		}
	}

	if( info->tether.b_use )
	{
		UINT32 particle_nb_cur = particle_host->get_particle_nb();
		FP32* particle_data = (FP32*)particle_host->get_particle_buffer()->get_data();

		//walk through vertices
		for( UINT32 i = current_index ; i < particle_nb_cur; ++i )
		{
			FP32 part[4];
			cpy_v4( part, particle_data + i * 4 );

			// do not tether anchors
			if( part[3] != 0.0f )
			{
				//check for the closest anchor
				FP32 min_dist_sqr = FLT_MAX;	//todo use define AAA value
				INT32 min_dist_idx = -1;

				for( UINT32 j = 0 ; j < anchor_indices.size() ; ++j )
				{
					UINT32 anchor_idx = anchor_indices.at(j);
					FP32 anc[4];
					cpy_v4( anc, particle_data + anchor_idx * 4 );

					FP32 dist_sqr = dist_squared_v3r( part, anc );

					if( dist_sqr < min_dist_sqr )
					{
						dist_sqr = min_dist_sqr;
						min_dist_idx = anchor_idx;
					}
				}

				if( min_dist_idx > -1 )
				{
					//note, to keep stiffness consistent in ui, we negate it here (tethers are push based)
					spring_host->append( i, min_dist_idx, info->tether.stiffness * -1.0f, tether_length_factor );
				}
			}
		}

	}

}

#endif //#if AAA_USE_FLEX()