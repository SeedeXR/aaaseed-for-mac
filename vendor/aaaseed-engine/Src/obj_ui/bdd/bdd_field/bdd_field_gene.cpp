#include "bdd_field_gene.h"
#include "image/bind_img_3d.h"
#include "draw/model.h"
#include "obj_ui/bdd/bdd_geo/bdd_tri.h"
#include <draw\line.h>
#include "draw/primitive.h"


#if !defined(AAA_AAA_FILE_H)
#include "file/aaa_file.h"
#endif

class c_bdd_field_gene_file
{
public:
	enum size_e
	{
		header_size = 128,
		io_blk_size = 4096
	};

public:
	union header {
		struct {
			INT32       _type;                  //  4
			INT32       _version;               //  4
			INT64       _size;                  //  8
			INT32       _sx;                    //  4
			INT32       _sy;                    //  4
			INT32       _sz;                    //  4
			FP32     _origin[3];             // 12
			FP32     _bounding_box[3];       // 12
			glm::mat4   _transformation_matrix; // 64
		};
		char        _raw[header_size];
	} _header;

	CHAR *  _data{ nullptr };

public:
	c_bdd_field_gene_file();
	~c_bdd_field_gene_file();

	C_NO_CPY_MOVE(c_bdd_field_gene_file);

public:
	bool     read( o_str path );

	static bool     write( o_str path, c_bdd_field_gene const * ptr );
	CHAR *   acquire_data();
};

FACTORY_CREATE_PROP_V1( c_bdd_field_gene, bdd_field_gene, Field generator, field_generator, sub_menu = "Field"; );

namespace n_field_gene
{
	CONSTEXPR INT32 BASE_NB_MAX		= c_bdd::NO_GEO_PARAM_NB + 13;
	CONSTEXPR INT32 GROUP_NB_MAX	= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_NB_MAX
									+ GROUP_NB_MAX;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_NO_GEO_BASE_PARAMS

		PARAM_DEF_BOOL_OFF_SAVE_NOT(	draw			)
		PARAM_DEF_BOOL_OFF_SAVE_NOT(	draw_particle	)

		PARAM_DEF_BOOL_OFF_SAVE_NOT(	compute_trig	)
		PARAM_DEF_BOOL_OFF_SAVE_NOT(	save_mesh		)

		PARAM_DEF_REAL(		field_draw_size_factor,	2, 1,		0, 1000 )


		PARAM_DEF_REAL_INF(	scale,					.1,   1)
		PARAM_DEF_REAL(		max_threshold,			1.,   1.,       0.,	   20.)
		PARAM_DEF_REAL(		min_threshold,			0.20, 0.,       0.,	   20.)
		PARAM_DEF_REAL_INF(	decay_factor,			1.,   1.)
		PARAM_DEF_REAL(		precision,			    0.20, 1.,       0.01,  20.)
		PARAM_DEF_REAL(		draw_count,			    1., 1.,         0.1,    2.)
		PARAM_DEF_INT32(	draw_steps,			    0, 1,			0,		 20)

		PARAM_DEF_REF(		bdd_src )
	};
}

void c_bdd_field_gene::param_init_pt()
{
	INT32	h = param_init_pt_no_geo();

	param_set_pt(				h, _b_draw					);
	param_set_pt(				h, _b_draw_particles		);

	param_set_pt(				h, _b_compute_trig_ui		);
	param_set_pt(				h, _b_save_mesh				);

	param_set_pt(				h, _field_draw_size_factor	);

	param_set_pt(				h, _scale					);
	param_set_pt(				h, _max_threshold			);
	param_set_pt(				h, _min_threshold			);
	param_set_pt(				h, _decay_factor			);
	param_set_pt(				h, _precision				);
	param_set_pt(				h, _draw_count				);
	param_set_pt(				h, _draw_steps_ui			);

	param_set_pt_attach_obj(	h, _target_name_symbo,		_bdd_target_src );

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE(c_bdd_field_gene)
,_bdd_target_src(nullptr)
{
	param_init_with(n_field_gene::param, n_field_gene::PARAM_NB_MAX);
	init();
}

c_bdd_field_gene::~c_bdd_field_gene()
{
	FREE(_data);
	FREE(_col);
}

void c_bdd_field_gene::init()
{
	_b_loaded = false;
}

//-----------------------------------------------------------------------------

FINLINE INT32 c_bdd_field_gene::get_index_3d( INT32 CONST x, INT32 CONST y, INT32 CONST z ) CONST NOEXCEPT
{
	return x + y * _sx + z * _sxy ;
}
FINLINE INT32 c_bdd_field_gene::get_index_3d( INT32 CONST * CONST p_3i ) CONST NOEXCEPT
{
	return p_3i[0] + p_3i[1] * _sx + p_3i[2] * _sxy ;
}

void	c_bdd_field_gene::write_3d_v3( INT32 CONST * CONST pos, REAL CONST * CONST vec )
{
	INT32 CONST index = get_index_3d( pos );
	FP32* CONST data = &_data[ index * 4 ];
	REAL CONST norm = norm_squared_v3r( vec );
	if( norm > 0.)
	{	
		data[0] = vec[0];
		data[1] = vec[1];
		data[2] = vec[2];
		data[3] = SQRT( norm );
	}
	else
		clear_v4( data );

}

FINLINE FP32	get_new_distance(	FP32 CONST * CONST lhs, FP32 CONST * CONST rhs, FP32 CONST current_dist )
{
	FP32 delta = lhs[0] - rhs[0];
	FP32 dst = delta * delta;
	if( dst >= current_dist )
		return dst;

	delta = lhs[1] - rhs[1];
	dst += delta * delta;
	if( dst >= current_dist )
		return dst;

	delta = lhs[2] - rhs[2];
	dst += delta * delta;

	return dst;
}

//-----------------------------------------------------------------------------

void	c_bdd_field_gene::compute_test_field( )
{
	FP32 pt1[3], pt2[3];
	map_image_point( pt1, 0,0,0 );
	map_image_point( pt2, 0,0,1 );
	FP32 CONST delta = get_new_distance( pt1, pt2, FLT_MAX );

	//algo:
	// temp array for bfs
	// for vtx in mesh:
	//   remap in real world
	//   remap in grid
	//   find nearest pt in grid (floor / ceil test)
	//   start bfs from nearest pt
	//      triangular inequality then get_dist

	//todo: negative dists inside.
	if( !is_valid() )
		return;

	FP32* data = _data;
	for( INT32 i = 0; i < _data_size; ++i )
		data[i] = FLT_MAX;


	// apply human tank repulsive field first.
	// optimize next bfs.
	FP32 ht_mapped[3];
	FP32 ht_dist;
	bool b_ht_is_positive;
	INT32 ht_index;

	FP32 CONST z_wall_dist = 7.995f / 2.0f;
	for( INT32 ix = 0; ix < _sx; ++ix )
	{
		for( INT32 iy = 0; iy < _sy; ++iy )
		{
			for( INT32 iz = 0; iz < _sz; ++iz )
			{
				map_image_point( ht_mapped, ix,iy,iz );
				ht_index = get_index_3d( ix,iy,iz );

				// KL wall
				// plan z = -7.995 / 2.
				if(    ht_mapped[0] >= 0.0f && ht_mapped[0] <= 20.753f
					&& ht_mapped[1] >= 0.0f && ht_mapped[1] <=  8.77f )
				{
					ht_dist = -7.995f / 2.0f - ht_mapped[2];
					b_ht_is_positive = ht_dist >= 0;
					ht_dist *= ht_dist; // we work with squared dists.
					if (ht_dist < ABS(data[ht_index]))
					data[ht_index] = b_ht_is_positive ? ht_dist : -ht_dist;
				}

				// KR wall
				// plan z = 7.995 / 2.
				if(    ht_mapped[0] >= 0.0f && ht_mapped[0] <= 20.753f
					&& ht_mapped[1] >= 0.0f && ht_mapped[1] <=  8.77f )
				{
					ht_dist = ht_mapped[2] - 7.995f / 2.0f;
					b_ht_is_positive = ht_dist >= 0;
					ht_dist *= ht_dist; // we work with squared dists.
					if (ht_dist < ABS(data[ht_index]))
					data[ht_index] = b_ht_is_positive ? ht_dist : -ht_dist;
				}

				// KC wall
				// plan x = 20.753
				if(    ht_mapped[1] >= 0.0f         && ht_mapped[1] <=  8.77f
					&& ht_mapped[2] >= -z_wall_dist && ht_mapped[2] <= z_wall_dist )

				{
					ht_dist = ht_mapped[0] - 20.753f;
					b_ht_is_positive = ht_dist >= 0;
					ht_dist *= ht_dist; // we work with squared dists.
					if (ht_dist < ABS(data[ht_index]))
					data[ht_index] = b_ht_is_positive ? ht_dist : -ht_dist;
				}

				// SO floor
				// plan y = 0
				if(   ht_mapped[0] >= 0.0f           && ht_mapped[0] <= 20.753f
				   && ht_mapped[2] >= -z_wall_dist   && ht_mapped[2] <= z_wall_dist )
				{
					ht_dist = -ht_mapped[1];
					b_ht_is_positive = ht_dist >= 0;
					ht_dist *= ht_dist; // we work with squared dists.
					if (ht_dist < ABS(data[ht_index]))
						data[ht_index] = b_ht_is_positive ? ht_dist : -ht_dist;
				}
			}
		}
	}

	INT64 CONST queue_size = 5 * _sz * _sx * _sy; //todo why 5 ? maa say 
	INT32* queue = (INT32*) MALLOC(queue_size * sizeof(INT32));
	INT32 queue_idx = 0;
	INT32 queue_end = 0;
	;
	INT32 CONST vtx_count  = _bdd_target_src->get_point_nb();
	REAL CONST * CONST vertices = _bdd_target_src->get_points();
	REAL CONST * CONST normals  = _bdd_target_src->get_normals();

	glm::vec4 vertex;
	FP32   mapped_pos[3];
 //   INT32     snapped_pos[3];
	FP32   mapped_grid_pt[3];
	FP32   mapped_normal[3];
	FP32   direction[3];

	INT32   curr_x, curr_y, curr_z;
	FP32 current_dist;
	FP32 current_best;
	FP32 dist;

	INT32 index;
	for( INT32 vtx_id = 0; vtx_id < vtx_count; ++vtx_id )
	{
		// Load vertex.
		vertex[0] = vertices[3 * vtx_id + 0];
		vertex[1] = vertices[3 * vtx_id + 1];
		vertex[2] = vertices[3 * vtx_id + 2];
		vertex[3] = 1.0f;

		// Remap in global world.
		vertex = _transformation_matrix * vertex;
		mapped_pos[0] = vertex[0]; mapped_pos[1] = vertex[1]; mapped_pos[2] = vertex[2];


		// Load normals.
		vertex[0] = normals[3 * vtx_id + 0];
		vertex[1] = normals[3 * vtx_id + 1];
		vertex[2] = normals[3 * vtx_id + 2];
		vertex[3] = 0.0f;
		vertex = _transformation_matrix * vertex;
		mapped_normal[0] = vertex[0];
		mapped_normal[1] = vertex[1];
		mapped_normal[2] = vertex[2];

		queue_idx = 0;
		queue_end = 0;

		//if (dist >= ABS(current_dist))
		//    continue;

		FP32 f_x = (mapped_pos[0] - _origin[0]) / _bounding_box[0] * _sx;
		FP32 f_y = (mapped_pos[1] - _origin[1]) / _bounding_box[1] * _sy;
		FP32 f_z = (mapped_pos[2] - _origin[2]) / _bounding_box[2] * _sz;

		CLAMP_REF( f_x, 0, _sx-1 );
		CLAMP_REF( f_y, 0, _sy-1 );
		CLAMP_REF( f_z, 0, _sz-1 );

		INT32 ix = (INT32)f_x;
		INT32 iy = (INT32)f_y;
		INT32 iz = (INT32)f_z;

	
		index = get_index_3d( ix,iy,iz );
		current_dist = ABS(data[index]);
		map_image_point( mapped_grid_pt, ix,iy,iz );
		dist = get_new_distance( mapped_pos, mapped_grid_pt, current_dist );
		if( dist < current_dist )
		{
			sub_v3( direction, mapped_grid_pt, mapped_pos );
			data[index] = dot_v3r( mapped_normal, direction ) < 0 ? -dist : dist;
			//TODO: fix this in the other cells too.
			set_v3( queue + 3 * queue_end++, ix,iy,iz );
		}
	
		index = get_index_3d( ix,iy,iz+1 );
		current_dist = ABS(data[index]);
		map_image_point( mapped_grid_pt, ix,iy,iz+1 );
		dist = get_new_distance( mapped_pos, mapped_grid_pt, current_dist );
		if( dist < current_dist )
		{
			sub_v3( direction, mapped_grid_pt, mapped_pos );
			data[index] = dot_v3r(mapped_normal, direction) < 0 ? -dist : dist;
			set_v3( queue + 3 * queue_end++, ix,iy,iz+1 );
		}

		index = get_index_3d( ix,iy+1,iz );
		current_dist = ABS(data[index]);
		map_image_point( mapped_grid_pt, ix,iy+1,iz );
		dist = get_new_distance( mapped_pos, mapped_grid_pt, current_dist );
		if( dist < current_dist )
		{
			sub_v3( direction, mapped_grid_pt, mapped_pos );
			data[index] = dot_v3r(mapped_normal, direction) < 0 ? -dist : dist;
			set_v3( queue + 3 * queue_end++, ix,iy+1,iz );
		}

		index = get_index_3d( ix,iy+1,iz+1 );
		current_dist = ABS(data[index]);
		map_image_point( mapped_grid_pt, ix,iy+1,iz+1 );
		dist = get_new_distance( mapped_pos, mapped_grid_pt, current_dist );
		if( dist < current_dist )
		{
			sub_v3( direction, mapped_grid_pt, mapped_pos );
			data[index] = dot_v3r(mapped_normal, direction) < 0 ? -dist : dist;
			set_v3(queue + 3 * queue_end++, ix,iy+1,iz+1 );
		}

		index = get_index_3d( ix+1,iy,iz );
		current_dist = ABS(data[index]);
		map_image_point( mapped_grid_pt, ix+1,iy,iz );
		dist = get_new_distance( mapped_pos, mapped_grid_pt, current_dist );
		if( dist < current_dist )
		{
			sub_v3( direction, mapped_grid_pt, mapped_pos );
			data[index] = dot_v3r( mapped_normal, direction ) < 0 ? -dist : dist;
			//TODO: fix this in the other cells too.
			set_v3( queue + 3 * queue_end++, ix+1,iy,iz );
		}
	
		index = get_index_3d( ix+1,iy,iz+1 );
		current_dist = ABS(data[index]);
		map_image_point( mapped_grid_pt, ix+1,iy,iz+1 );
		dist = get_new_distance( mapped_pos, mapped_grid_pt, current_dist );
		if( dist < current_dist )
		{
			sub_v3( direction, mapped_grid_pt, mapped_pos );
			data[index] = dot_v3r(mapped_normal, direction) < 0 ? -dist : dist;
			set_v3( queue + 3 * queue_end++, ix+1,iy,iz+1 );
		}

		index = get_index_3d( ix+1,iy+1,iz );
		current_dist = ABS(data[index]);
		map_image_point( mapped_grid_pt, ix+1,iy+1,iz );
		dist = get_new_distance( mapped_pos, mapped_grid_pt, current_dist );
		if( dist < current_dist )
		{
			sub_v3( direction, mapped_grid_pt, mapped_pos );
			data[index] = dot_v3r(mapped_normal, direction) < 0 ? -dist : dist;
			set_v3( queue + 3 * queue_end++, ix+1,iy+1,iz );
		}

		index = get_index_3d( ix+1,iy+1,iz+1 );
		current_dist = ABS(data[index]);
		map_image_point( mapped_grid_pt, ix+1,iy+1,iz+1 );
		dist = get_new_distance( mapped_pos, mapped_grid_pt, current_dist );
		if( dist < current_dist )
		{
			sub_v3( direction, mapped_grid_pt, mapped_pos );
			data[index] = dot_v3r(mapped_normal, direction) < 0 ? -dist : dist;
			data[index] = dot_v3r(mapped_normal, direction) < 0 ? -dist : dist;
			set_v3( queue + 3 * queue_end++, ix+1,iy+1,iz+1 );
		}

		while( queue_idx < queue_end )
		{
			if( 3 * queue_end >= queue_size )
			{
				// debug test: queue is too small.
				//assert(false);
				return;
			}

			curr_x = queue[3 * queue_idx + 0];
			curr_y = queue[3 * queue_idx + 1];
			curr_z = queue[3 * queue_idx + 2];
			++queue_idx;

			// Case by case. No diagonal.
			// Faster than nested dx/dy/dz loops.
			if( curr_x > 0 )
			{
				index = get_index_3d( curr_x-1, curr_y, curr_z );
				current_best = ABS(data[index]);
				map_image_point( mapped_grid_pt, curr_x-1, curr_y, curr_z );
				FP32 dist = get_new_distance( mapped_pos, mapped_grid_pt, current_best );
				if( dist < current_best )
				{
					sub_v3( direction, mapped_grid_pt, mapped_pos );
					data[index] = (dot_v3r( mapped_normal, direction ) < 0) ? -dist : dist;
					set_v3( queue + 3 * queue_end++, curr_x-1, curr_y, curr_z );
				}
			}
			if( curr_x + 1 < _sx )
			{
				index = get_index_3d( curr_x+1, curr_y, curr_z );
				current_best = ABS(data[index]);
				map_image_point( mapped_grid_pt, curr_x+1, curr_y, curr_z );
				FP32 dist = get_new_distance( mapped_pos, mapped_grid_pt, current_best );
				if( dist < current_best )
				{
					sub_v3( direction, mapped_grid_pt, mapped_pos );
					data[index] = (dot_v3r( mapped_normal, direction ) < 0) ? -dist : dist;
					set_v3( queue + 3 * queue_end++, curr_x+1, curr_y, curr_z );
				}
			}

			if( curr_y > 0 )
			{
				index = get_index_3d( curr_x, curr_y-1, curr_z );
				current_best = ABS(data[index]);
				map_image_point( mapped_grid_pt, curr_x, curr_y-1, curr_z );
				FP32 dist = get_new_distance( mapped_pos, mapped_grid_pt, current_best );
				if( dist < current_best )
				{
					sub_v3( direction, mapped_grid_pt, mapped_pos );
					data[index] = (dot_v3r( mapped_normal, direction ) < 0) ? -dist : dist;
					set_v3( queue + 3 * queue_end++, curr_x, curr_y-1, curr_z );
				}
			}
			if( curr_y + 1 < _sy )
			{
				index = get_index_3d( curr_x, curr_y+1, curr_z );
				current_best = ABS(data[index]);
				map_image_point( mapped_grid_pt, curr_x, curr_y+1, curr_z );
				FP32 dist = get_new_distance( mapped_pos, mapped_grid_pt, current_best );
				if( dist < current_best )
				{
					sub_v3( direction, mapped_grid_pt, mapped_pos );
					data[index] = (dot_v3r( mapped_normal, direction ) < 0) ? -dist : dist;
					set_v3( queue + 3 * queue_end++, curr_x, curr_y+1, curr_z );
				}
			}
			if (curr_z > 0)
			{
				index = get_index_3d( curr_x, curr_y, curr_z-1 );
				current_best = ABS(data[index]);
				map_image_point( mapped_grid_pt, curr_x, curr_y, curr_z-1 );
				FP32 dist = get_new_distance( mapped_pos, mapped_grid_pt, current_best );
				if( dist < current_best )
				{
					sub_v3( direction, mapped_grid_pt, mapped_pos );
					data[index] = (dot_v3r( mapped_normal, direction ) < 0) ? -dist : dist;
					set_v3( queue + 3 * queue_end++, curr_x, curr_y, curr_z-1 );
				}
			}
			if (curr_z + 1 < _sz)
			{
				index = get_index_3d( curr_x, curr_y, curr_z+1 );
				current_best = ABS(data[index]);
				map_image_point( mapped_grid_pt, curr_x, curr_y, curr_z+1 );
				FP32 dist = get_new_distance( mapped_pos, mapped_grid_pt, current_best );
				if( dist < current_best )
				{
					sub_v3( direction, mapped_grid_pt, mapped_pos );
					data[index] = (dot_v3r( mapped_normal, direction ) < 0) ? -dist : dist;
					set_v3( queue + 3 * queue_end++, curr_x, curr_y, curr_z+1 );
				}
			}
		}

		//DBG_PRINT_STRING( "Queue end: %d", queue_end );
		
	}

	free(queue);

	//maa don't understand what this is doing : nothing ?
	// also it is far from optimim in particular the triple inside loops
	for( INT32 x = 1; x < _sx - 1; ++x )
		for( INT32 y = 1; y < _sy - 1 ; ++y )
			for( INT32 z = 1; z < _sz - 1; ++z )
			{
				//todoopt
				FP32 depth1 = _data[ get_index_3d(x,y,z) ] / SQRT(ABS( _data[ get_index_3d(x,y,z) ]));
				for( INT32 dx = 0; dx <= 1; ++dx )
					for( INT32 dy = 0; dy <= 1; ++dy )
						for( INT32 dz = 0; dz <= 1; ++dz )
						{
							if( dx*dy == 0 || dx*dz == 0 || dy*dz == 0 )
							{
								FP32 depth2 = _data[get_index_3d(x+dx, y+dy, z+dz)] / SQRT(ABS(_data[get_index_3d(x+dx, y+dy, z+dz)]));
								if (ABS(depth2 - depth1) > SQRT(delta)*2)
								{
									break;
								}
							}
						}
			}
}

//-----------------------------------------------------------------------------

void	c_bdd_field_gene::draw_as_vectors( REAL CONST * CONST size )
{
	if( _b_draw_particles )
	{
		INT32 CONST vtx_count  = _bdd_target_src->get_point_nb();
		REAL CONST * CONST vertices = _bdd_target_src->get_points();
		REAL CONST * CONST normals = _bdd_target_src->get_normals();

		glm::vec4 ver;
		ver.w = 1.;
		glm::vec4 nor;
		nor.w = 0.;
		for( INT32 i = 0; i < vtx_count; ++i )
		{
			INT32 ri = INT32(i * (_draw_count ? _draw_count : 0.01) * 1000);
			if( ri >= vtx_count )
				break;

			// write pos.
			cpy_v3( &ver.x, vertices + ri * 3 );
			glm::vec4 pos = _transformation_matrix * ver;

			cpy_v3( &nor.x, normals + ri * 3 );
			glm::vec4 normal = _transformation_matrix * nor;

			FP32 start[3];
			add_scale_v3( start, &pos.x, &normal.x, 5. );

			c_prim3::base.alloc_vertex( 2 * _draw_steps_ui );
			REAL* vec = c_prim3::base.get_vertex();
			INT32 nb = 0;
			for( int s = 0; s < _draw_steps_ui; ++s )
			{
				INT32 debug[3];
				snap_to_grid( debug, start );
				FP32 depth_start = _data[get_index_3d(debug)];

				cpy_v3( vec, start );
				vec += 3;
				FP32 pt[3];
				get_gradient( pt, start );
				REAL scaling = get_gradient_norm( start );
				normalize_scale_v3r( pt, scaling );

				//normalize_v3r(pt);
				add_v3( start, pt );
				cpy_v3( vec, start);
				vec += 3;

				++nb;
			
				// Debug negative dist.
				snap_to_grid( debug, start );
				FP32 depth_end = _data[get_index_3d(debug)];
				if( depth_start > depth_end && (ABS(depth_start) > 0.01 && ABS(depth_end) > 0.01) )
					break;
				if( depth_start < -1 && _draw_steps_ui >= 3 )
					break;

				// Stop when out of bound.
				FP32 f_x = (start[0] - _origin[0]) / _bounding_box[0] * _sx;
				FP32 f_y = (start[1] - _origin[1]) / _bounding_box[1] * _sy;
				FP32 f_z = (start[2] - _origin[2]) / _bounding_box[2] * _sz;
				if( f_x < 0 || f_x >= _sx || f_y < 0 || f_y >= _sy || f_z < 0 || f_z >= _sz )
					break;
			}
			c_prim3::base.draw( GL_LINES, nb*2 );
		}
	}
	else
	{
		// draw potiential lines.
		c_prim3::base.alloc_vertex( _sx * _sy * _sz );
		REAL* vec = c_prim3::base.get_vertex();

		FP32* col = _col;

		for( INT32 ix = 0; ix < _sx; ++ix )
		{
			for( INT32 iy = 0; iy < _sy; ++iy )
			{
				for( INT32 iz = 0; iz < _sz; ++iz )
				{

					map_image_point( vec, ix,iy,iz );
					FP32 dist = _data[get_index_3d(ix, iy, iz)];

					col[0] = _max_threshold / ABS(dist) > 1 ? 1 : _max_threshold / ABS(dist);
					col[1] = dist < 0 ? REAL(1) : REAL(0);
					col[2] = dist > 0 ? REAL(1) : REAL(0);
					//col[0] = dist < 0 ? 1 : 0;
					//col[1] = dist > 0 ? 1 : 0;
					//col[2] = 0;
					vec += 3;
					col += 3;
				}
			}
		}

		c_prim3::base.draw_color3( GL_POINTS, _sx * _sy * _sz, _col );
	}
}

void	c_bdd_field_gene::draw()
{
	if( _b_draw && is_valid() )
	{
		REAL size[3];
		c_model::cur->get_size_xyz_v3( size );
		if( _b_draw_normals )
			draw_normals( size );
		else
			draw_as_vectors( size );
	}
}

//-----------------------------------------------------------------------------

FP32*	c_bdd_field_gene::compute_normals()
{
	 if( !is_valid() )
		return nullptr;

	FP32* data = _data;
	INT32 CONST vtx_count	= _bdd_target_src->get_point_nb();
	REAL CONST * vertices	= _bdd_target_src->get_points();
	REAL CONST * normals	= _bdd_target_src->get_normals();

	glm::vec4 vec;
	vec[3] = 1.;
	glm::vec4 nor;
	nor[3] = 0.;
	for( INT32 i = 0; i < vtx_count; ++i )
	{
		// write pos.
		cpy_v3( &vec[0], vertices );
		vec = _transformation_matrix * vec;
		cpy_v3( data, &vec[0] );
		vertices += 3;
		data += 3;
		
		cpy_v3( &nor[0], normals );
		nor = _transformation_matrix * nor;
		cpy_v3( data, &nor[0] );
		normals += 3;
		data += 3;
	}
	return _data;
}

void	c_bdd_field_gene::draw_normals( REAL CONST * CONST size )
{
	FP32 const * data = _data;
	FP32 * col = _col;
	c_prim3::base.alloc_vertex( _sxy );
	c_prim3::base.alloc_color3( _sxy );
	REAL* vec = c_prim3::base.get_vertex();
	INT32 nb = 0;
	for( INT32 i = 0; i < _sx; ++i )
	{
		vec[0] = data[0];
		vec[1] = data[1];
		vec[2] = data[2];

		vec[3] = data[0] +  _scale * data[3];
		vec[4] = data[1] +  _scale * data[4];
		vec[5] = data[2] +  _scale * data[5];

		col[0] = 0.5; col[1] = 0.5; col[2] = 0.5;
		col[3] = 0.0; col[4] = 0.0; col[5] = 0.0;
		data += 6;
		vec += 6;
		col += 6;
		++nb;
	}
	c_prim3::base.draw_color3( GL_LINES, nb * 2, _col );
}

//-----------------------------------------------------------------------------

void	c_bdd_field_gene::update()
{
	//_draw_normals = true;
	_bdd_target_src = update_bdd_target( _target_name_symbo, _bdd_target_src );

	if( !_b_loaded )
		load();

	if( _b_save_mesh )
	{
		save();
		_b_save_mesh = false;
	}

	// todo: only draw on compute trig, serious issue here.
	if( _b_compute_trig_ui )
	{
		compute_size();

		//todo unused ?
		bool b_reload = false;
		if( is_diff_v3( _size_pixel, _size_pixel_asked ) )
		{
			INT32 current = _size_pixel[0] * _size_pixel[1] * _size_pixel[2];
			INT32 asked = _size_pixel_asked[0] * _size_pixel_asked[1] * _size_pixel_asked[2];
			if( !_data || asked > current )
			{
				_data_size = asked;
				if( _data )
					FREE(_data);
				_data = (FP32*) MALLOC(_data_size * sizeof(FP32));
				_col  = (FP32*) MALLOC((_b_draw_normals ? 1 : 3) * _data_size * sizeof(FP32));
			}
			if( _data )
			{
				MEMCLEAR(_data, _data_size * sizeof(FP32));
				cpy_v3( _size_pixel, _size_pixel_asked );
				_sx = _size_pixel[0];
				_sy = _size_pixel[1];
				_sz = _size_pixel[2];
				_ssx = INT32(_sx / 3.0f);
				_ssy = INT32(_sy / 3.0f);
				_ssz = INT32(_sz / 3.0f);
				_sxy = _sx * _sy;

				_transformation_matrix = ((c_bdd_mesh_static *)_bdd_target_src)->get_transform_matrix();

				// deactivated since it's quite long to compute. ED
				//_b_compute_trig_ui = true;

				b_reload = true;
			}
		}

		if( _b_draw_normals )
			compute_normals();
		else
			compute_test_field();

		//save();

		_b_compute_trig_ui = false;
	}
}

//-----------------------------------------------------------------------------

void	c_bdd_field_gene::compute_size()
{
	if( _b_draw_normals )
	{
		_size_pixel_asked[0] = _bdd_target_src->get_point_nb();
		_size_pixel_asked[1] = 2;
		_size_pixel_asked[2] = 3;
	}
	else
	{
		FP32 CONST extra_size = 2.0f;

		FP32 * bounding_box_min = ((c_bdd_mesh_static *)_bdd_target_src)->get_mesh_bbox_min();
		FP32 * bounding_box_max = ((c_bdd_mesh_static *)_bdd_target_src)->get_mesh_bbox_max();
		_bounding_box[0] = bounding_box_max[0] - bounding_box_min[0] + 2.0f * extra_size;
		_bounding_box[1] = bounding_box_max[1] - bounding_box_min[1] + 2.0f * extra_size;
		_bounding_box[2] = bounding_box_max[2] - bounding_box_min[2] + 2.0f * extra_size;

		_origin[0] = bounding_box_min[0] - extra_size;
		_origin[1] = bounding_box_min[1] - extra_size;
		_origin[2] = bounding_box_min[2] - extra_size;

		//_bounding_box[0] = 50.0f;
		//_bounding_box[1] = 20.0f;
		//_bounding_box[2] = 60.0f;

		//_origin[0] = -10.0f;
		//_origin[1] = -2.0f;
		//_origin[2] = -10.0f;

		_transformation_matrix = ((c_bdd_mesh_static *)_bdd_target_src)->get_transform_matrix();

		if (_precision == 0.)
			_precision = 0.01f;

		_size_pixel_asked[0] = (INT32)ceil(_bounding_box[0] / _precision);
		_size_pixel_asked[1] = (INT32)ceil(_bounding_box[1] / _precision);
		_size_pixel_asked[2] = (INT32)ceil(_bounding_box[2] / _precision);
	}
}

void	c_bdd_field_gene::map_image_point( FP32 * CONST dst, INT32 CONST * CONST pos )
{
	map_image_point( dst, pos[0], pos[1], pos[2] );
}

void	c_bdd_field_gene::map_image_point( FP32 * CONST dst, INT32 CONST x, INT32 CONST y, INT32 CONST z )
{
	//todo opt by storing _bounding_box[0]/_sx ... and use it to multiply
	dst[0] = (FP32)x/(FP32)_sx * _bounding_box[0] + _origin[0];
	dst[1] = (FP32)y/(FP32)_sy * _bounding_box[1] + _origin[1];
	dst[2] = (FP32)z/(FP32)_sz * _bounding_box[2] + _origin[2];
}

void	c_bdd_field_gene::snap_to_grid( INT32 * CONST dst, FP32 CONST * CONST vec )
{
	FP32 CONST f_x = (vec[0] - _origin[0]) / _bounding_box[0] * _sx;
	FP32 CONST f_y = (vec[1] - _origin[1]) / _bounding_box[1] * _sy;
	FP32 CONST f_z = (vec[2] - _origin[2]) / _bounding_box[2] * _sz;

	FP32 CONST t_x = f_x - (INT32)f_x;
	FP32 CONST t_y = f_y - (INT32)f_y;
	FP32 CONST t_z = f_z - (INT32)f_z;

	dst[0] = (INT32)f_x + (t_x >= 0.5f);
	dst[1] = (INT32)f_y + (t_y >= 0.5f);
	dst[2] = (INT32)f_z + (t_z >= 0.5f);

	// Clamp pos.
	CLAMP_REF( dst[0], 0, _sx-1 );
	CLAMP_REF( dst[1], 0, _sy-1 );
	CLAMP_REF( dst[2], 0, _sz-1 );
}

void	c_bdd_field_gene::get_gradient( FP32 * CONST dst, INT32 CONST * CONST pos )
{
	get_gradient( dst, pos[0],pos[1],pos[2] );
}
void	c_bdd_field_gene::get_gradient( FP32 * CONST dst, INT32 CONST x, INT32 CONST y, INT32 CONST z )
{
	clear_v3(dst);
	if( !is_valid() )
		return;

	FP32 CONST val = _data[ get_index_3d( x, y, z ) ];

	// x
	if( x > 0 )
		dst[0] = val - _data[ get_index_3d( x-1, y, z) ];
	if( x+1 < _sx )
		dst[0] += _data[ get_index_3d( x+1, y, z ) ] - val;

	// y
	if( y > 0 )
		dst[1] = val - _data[ get_index_3d( x, y-1, z ) ];
	if( y+1 < _sy )
		dst[1] += _data[ get_index_3d( x, y+1, z ) ] - val;

	// z
	if( z > 0 )
		dst[2] = val - _data[get_index_3d( x, y, z-1 ) ];
	if( z+1 < _sz )
		dst[2] += _data[ get_index_3d( x, y, z+1 ) ] - val;

	scale_v3( dst, .5 );
}

void	c_bdd_field_gene::get_gradient( FP32 * CONST dst, REAL CONST * CONST pos )
{
	clear_v3(dst);
	if( !is_valid() )
		return;

	FP32 f_x = (pos[0] - _origin[0]) / _bounding_box[0] * _sx;
	FP32 f_y = (pos[1] - _origin[1]) / _bounding_box[1] * _sy;
	FP32 f_z = (pos[2] - _origin[2]) / _bounding_box[2] * _sz;

	// test: better to return nul? ED
	//CLAMP_REF( f_x, 0, _sx-1 );
	//CLAMP_REF( f_y, 0, _sy-1 );
	//CLAMP_REF( f_z, 0, _sz-1 );

	if( f_x < 0.0f )
		return;
	if( f_x > _sx-1 )
		return;
	if( f_y < 0.0f )
		return;
	if( f_y > _sy-1 )
		return;
	if( f_z < 0.0f )
		return;
	if( f_z > _sz-1 )
		return;

	INT32 ix = (INT32)f_x;
	INT32 iy = (INT32)f_y;
	INT32 iz = (INT32)f_z;

	FP32 t_x = f_x - ix;
	FP32 t_y = f_y - iy;
	FP32 t_z = f_z - iz;

	FP32 gradient[3];
	FP32 ratio;

	ratio = (1-t_x) * (1-t_y);
	get_gradient( gradient, ix, iy, iz );
	add_scale_v3( dst, gradient, ratio * (1-t_z) );
	get_gradient( gradient, ix, iy, iz+1 );
	add_scale_v3( dst, gradient, ratio * t_z );

	ratio = (1-t_x) * t_y;
	get_gradient( gradient, ix, iy+1, iz);
	add_scale_v3( dst, gradient, ratio * (1-t_z) );
	get_gradient( gradient, ix, iy+1, iz+1 );
	add_scale_v3( dst, gradient, ratio * t_z );

	ratio = t_x * (1-t_y);
	get_gradient( gradient, ix+1, iy, iz );
	add_scale_v3( dst, gradient, ratio * (1-t_z) );
	get_gradient( gradient, ix+1, iy, iz+1 );
	add_scale_v3( dst, gradient, ratio * t_z );

	ratio = t_x * t_y;
	get_gradient( gradient, ix+1, iy+1, iz );
	add_scale_v3( dst, gradient, ratio * (1-t_z) );
	get_gradient( gradient, ix+1, iy+1, iz+1 );
	add_scale_v3( dst, gradient, ratio * t_z );
}


REAL    c_bdd_field_gene::get_distance( REAL CONST * CONST pos )
{
	if( !is_valid() )
		return 0.;

	REAL dist = 0.;
	FP32 f_x = (pos[0] - _origin[0]) / _bounding_box[0] * _sx;
	FP32 f_y = (pos[1] - _origin[1]) / _bounding_box[1] * _sy;
	FP32 f_z = (pos[2] - _origin[2]) / _bounding_box[2] * _sz;

	// test: better to return 0? ED
	CLAMP_REF( f_x, 0, _sx-1 );
	CLAMP_REF( f_y, 0, _sy-1 );
	CLAMP_REF( f_z, 0, _sz-1 );

	INT32 ix = (INT32)f_x;
	INT32 iy = (INT32)f_y;
	INT32 iz = (INT32)f_z;

	FP32 t_x = f_x - ix;
	FP32 t_y = f_y - iy;
	FP32 t_z = f_z - iz;

	FP32 ratio;

	ratio = (1-t_x) * (1-t_y);
	dist += ratio * (1-t_z)	* _data[ get_index_3d( ix, iy, iz ) ];
	dist += ratio * t_z		* _data[ get_index_3d( ix, iy, iz+1 ) ];

	ratio = (1-t_x) * t_y;
	dist += ratio * (1-t_z)	* _data[get_index_3d( ix, iy+1, iz ) ];
	dist += ratio * t_z		* _data[get_index_3d( ix, iy+1, iz+1 ) ];

	ratio = t_x * (1-t_y);
	dist += ratio * (1-t_z)	* _data[get_index_3d( ix+1, iy, iz ) ];
	dist += ratio * t_z		* _data[get_index_3d( ix+1, iy, iz+1 ) ];

	ratio = t_x * t_y;
	dist += ratio * (1-t_z)	* _data[get_index_3d( ix+1, iy+1, iz ) ];
	dist += ratio * t_z		* _data[get_index_3d( ix+1, iy+1, iz+1 ) ];

	return dist;
}

REAL    c_bdd_field_gene::get_gradient_norm( REAL CONST * CONST pos )
{
	return get_gradient_norm( pos, _min_threshold, _max_threshold, _scale, _decay_factor );
}

REAL	c_bdd_field_gene::get_gradient_norm(
	REAL CONST * CONST pos,
	REAL CONST fish_length,
	REAL CONST scale,
	REAL CONST decay_factor )
{
	return get_gradient_norm( pos, fish_length + _precision / 2.0f, MAX(2 * fish_length, 2*_precision), scale, decay_factor );
}

REAL	c_bdd_field_gene::get_gradient_norm(
	REAL CONST * CONST pos,
	REAL CONST min_threshold,
	REAL CONST max_threshold,
	REAL CONST scale,
	REAL CONST decay_factor)
{
	REAL dist = get_distance(pos);

	// no repulsor beyond a certain limit.
	if( dist > max_threshold )
		return 0.;
	if( dist < min_threshold )
		return scale;

	// between _min_threshold and _max_threshold
	FP32 t = (max_threshold - dist) / (MAX(max_threshold - min_threshold, 0.1f));
	t = pow(t, decay_factor);
	dist *= t;
	return dist;

	//RACE:callback_field
}

//-----------------------------------------------------------------------------

c_bdd_field_gene_file::c_bdd_field_gene_file()
{}

c_bdd_field_gene_file::~c_bdd_field_gene_file()
{
	IF_FREE_AND_NULL(_data);
}

bool c_bdd_field_gene_file::read( o_str path )
{
	if( !c_file::is_exist(path) )
		return false;
	FILE * file = c_file::FOPEN( path, "rb" );
	if( !file )
		return false;

	c_file::FSEEK_END(file, 0);
	INT64 sz = c_file::FTELL(file);
	if (sz < c_bdd_field_gene_file::header_size)
		goto close_and_return;

	c_file::FSEEK_SET(file, 0);

	// header.
	if( c_file::FREAD(&_header, 1, c_bdd_field_gene_file::header_size, file) != c_bdd_field_gene_file::header_size )
		goto close_and_return;

	// check data size.
	if( !_header._size )
		goto close_and_return;

	// check file full size.
	if( sz < (c_bdd_field_gene_file::header_size + _header._size) )
		goto close_and_return;

	// data alloc.
	IF_FREE_AND_NULL( _data );
	_data = (CHAR*)MALLOC(_header._size);
	if( !_data )
		goto close_and_return;

	{
		// data read.
		INT64 head  = 0;
		INT64 remaining = _header._size;
		INT64 rc = 0;
		do
		{
			if( remaining > c_bdd_field_gene_file::io_blk_size )
				sz = c_bdd_field_gene_file::io_blk_size;
			else 
				sz = remaining;

			rc = c_file::FREAD(_data + head, 1, sz, file);
			head        += rc;
			remaining   -= rc;

		}
		while( head < _header._size );
	}

	c_file::FCLOSE(file);
	return true;

close_and_return:
	c_file::FCLOSE(file);
	return false;
}

bool c_bdd_field_gene_file::write( o_str path, c_bdd_field_gene const * ptr )
{
	// check input data.
	if( !ptr || !ptr->_data || !ptr->_data_size || !ptr->_sx || !ptr->_sy || !ptr->_sz )
		return false;

	// open file.
	FILE * file = c_file::FOPEN(path, "wb+");
	if( !file )
		return false;

	// header.
	c_bdd_field_gene_file::header hd;
	MEMCLEAR( &hd, c_bdd_field_gene_file::header_size );
	hd._type    = 1;    // dummy atm
	hd._version = 1;    // dummy atm
	hd._size    = ptr->_data_size * sizeof(FP32); // todo -> ugly
	hd._sx      = ptr->_sx;
	hd._sy      = ptr->_sy;
	hd._sz      = ptr->_sz;
	cpy_v3( hd._bounding_box,	ptr->_bounding_box	);
	cpy_v3( hd._origin,			ptr->_origin		);
	hd._transformation_matrix = ptr->_transformation_matrix;

	INT64 head = c_file::FWRITE( &hd, 1, c_bdd_field_gene_file::header_size, file );
	if( head != c_bdd_field_gene_file::header_size )
	{
		c_file::FCLOSE(file);
		return false;
	}

	// data.
	head = 0;
	CHAR * data = (CHAR*)ptr->_data;
	INT64 remaining = hd._size;
	INT64 sz = 0;
	INT64 rc = 0;
	do
	{
		if( remaining > c_bdd_field_gene_file::io_blk_size )
			sz = c_bdd_field_gene_file::io_blk_size;
		else 
			sz = remaining;

		rc = c_file::FWRITE(data + head, 1, sz, file);
		head        += rc;
		remaining   -= rc;
	}
	while (head < hd._size);

	c_file::FCLOSE(file);
	return false;
}

CHAR * c_bdd_field_gene_file::acquire_data()
{
	CHAR * res = _data;
	_data = nullptr;
	return res;
}

void c_bdd_field_gene::save()
{
	// Save file.
	o_str path = "../../Media/Monaco/Distance_fields/";
	path += get_name_symbo();
	path += ".bin";
	bool b_write = c_bdd_field_gene_file::write(path, this);

	// Make sure it succeeded.
	if( !b_write )
		err_print( "failed to save file." );

}

void c_bdd_field_gene::load()
{
	c_bdd_field_gene_file file_reader;
	o_str path = "../../Media/Monaco/Distance_fields/";
	path += get_name_symbo();
	path += ".bin";
	bool read = file_reader.read(path);
	if( !read )
	{
		if( c_file::is_exist(path) )
		{
			// yet it exists: something wrong happened.
			err_print( "failed to read saved file." );
		}
		return;
	}

	_sx  = file_reader._header._sx;
	_sy  = file_reader._header._sy;
	_sz  = file_reader._header._sz;
	_sxy = _sx * _sy;
	_data_size = file_reader._header._size / sizeof(FP32); // todo -> ugly
	_data = (FP32*)file_reader.acquire_data();
	_col = (FP32*)malloc(_data_size * 3 * sizeof(FP32));
	
	_size_pixel_asked[0] = _sx; _size_pixel_asked[1] = _sy; _size_pixel_asked[2] = _sz;
	_size_pixel[0] = _sx; _size_pixel[1] = _sy; _size_pixel[2] = _sz;

	_transformation_matrix = file_reader._header._transformation_matrix;
	cpy_v3( _bounding_box,	 file_reader._header._bounding_box	);
	cpy_v3( _origin,		 file_reader._header._origin		);

	_b_loaded = true;
}

bool	c_bdd_field_gene::is_inside( REAL CONST x, REAL CONST y, REAL CONST z ) const
{
	return INSIDE_MIN_MAX( x, _origin[0], _origin[0] + _bounding_box[0] )
		&& INSIDE_MIN_MAX( y, _origin[1], _origin[1] + _bounding_box[1] )
		&& INSIDE_MIN_MAX( z, _origin[2], _origin[2] + _bounding_box[2] );
}
bool	c_bdd_field_gene::is_inside( REAL CONST * CONST vec ) const
{
	return is_inside( vec[0], vec[1], vec[2] );
}


void	c_bdd_field_gene::add_sphere( REAL* center, REAL radius )
{
	//GOOD_PRINT_STRING( "add_sphere at %f %f %f wuth radius %f", center[0], center[1], center[2], radius );

	for( INT32 ix = 0; ix < _sx; ++ix )
	{
		for( INT32 iy = 0; iy < _sy; ++iy )
		{
			for( INT32 iz = 0; iz < _sz; ++iz )
			{
				FP32 mapped[3];
				map_image_point( mapped, ix,iy,iz );
				INT32 index = get_index_3d( ix, iy, iz );
				FP32 dist = SQRT(get_new_distance(mapped, center, FLT_MAX)) - radius;
				bool b_is_positive = dist >= 0;
				dist *= dist;

				if( dist < ABS(_data[index]) )
					_data[index] = b_is_positive ? dist : -dist;
			}
		}
	}
}
