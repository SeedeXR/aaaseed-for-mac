#include "bdd_gl_part.h"
#include <glm/glm.hpp>
#include "math/rand.h"

#include "draw/model.h"
#include "Shaders/shading.h"
#include "spy.h"
#include "gol/gol_draw.h"

#include "gl/ubo.h"
#include "gl/ssbo.h"
#include "flex/Emitter_Image.h"
#include "draw/render.h"


FACTORY_CREATE_PROP_V1( c_bdd_gl_part, bdd_gl_part, GL Part, bdd_gl_part, sub_menu="Point"; );

namespace GOL
{
	// 64 bytes, compact version used for drawing and simple compute 
	typedef struct {
		glm::vec4	pos;
		glm::vec4	vel;
		glm::vec4	pos_b;
		glm::vec4	dum;
	} st_particle_compact;
	
	// 112 bytes, richer structure for more complex work
	typedef struct {
		// start as the compact version
		glm::vec4	pos;
		glm::vec4	vel;
		glm::vec4	pos_b;
		glm::vec4	dum;

		glm::vec4	accel;
		glm::vec4	pos_c;

		float		mass;
		float		radius;
		float		life;
		float		life_over_one;
	} st_particle_full;

	typedef struct {
		UINT32		_active_nb;
		UINT32		_prim_nb;
		UINT32		_first;
		UINT32		_base_instance;
		UINT32		_free_nb;
	} st_particle_accum_buffer;
}	//namespace GOL

namespace {
	c_rand_lin		sta_rand;
	CONSTEXPR UINT32 PARTICLES_NB_MAX = 1024 * 1024 * 256;
			  
	CONSTEXPR INT32	BASE_PARAM_NB			= c_bdd::GEO_PARAM_NB + 13;
	CONSTEXPR INT32	INIT_VALUE_PARAM_NB		= 6;
	CONSTEXPR INT32	INIT_DUM_PARAM_NB		= 9;
	CONSTEXPR INT32	GROUP_PARAM_NB			= 4;
	CONSTEXPR INT32	INIT_COMPACT_PARAM_NB	= 3 * (c_placer::PARAM_ALL_NB + 1) + INIT_DUM_PARAM_NB;
	CONSTEXPR INT32	INIT_FULL_PARAM_NB		= 2 * (c_placer::PARAM_ALL_NB + 1) + INIT_VALUE_PARAM_NB;
	CONSTEXPR INT32	BLOCK_SIZE_PARAM_NB		= 4;
	CONSTEXPR INT32 FILTER_PARAM_NB			= 13;
	CONSTEXPR INT32	PARAM_NB	=	BASE_PARAM_NB
								+	INIT_COMPACT_PARAM_NB
								+	INIT_FULL_PARAM_NB
								+	BLOCK_SIZE_PARAM_NB
								+   FILTER_PARAM_NB
								+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_BOOL_ON(		compute						)
		PARAM_DEF_BOOL_ON(		draw						)
		PARAM_DEF_BOOL_ON(		point_full_version			)
		PARAM_DEF_INT32(		point_nb,					1, 1024*32,		1, PARTICLES_NB_MAX )
		PARAM_DEF_BOOL_OFF(		point_nb_alloc_when_changed	)
		PARAM_DEF_INT32(		point_nb_used,				0, 1024*32,		1, PARTICLES_NB_MAX )
		PARAM_DEF_INT32(		point_instance_nb,			2, 1,			1, 256 )
		PARAM_DEF_BOOL_OFF(		grid_2d						)
		PARAM_DEF_INT32(		nb_u,						32, 256,		2, PARTICLES_NB_MAX )
		PARAM_DEF_BOOL_OFF(		restart_trig				)
		PARAM_DEF_BOOL_OFF(		lifetime_compute			)

		PARAM_DEF_GROUP_CLOSED(	Init,			INIT_COMPACT_PARAM_NB )
			PARAM_DEF_GROUP_CLOSED(	placer,				c_placer::PARAM_ALL_NB )
				PARAM_PLACER(			placer )
			PARAM_DEF_GROUP_CLOSED(	placer_b,			c_placer::PARAM_ALL_NB )
				PARAM_PLACER(			placer_b )
			PARAM_DEF_GROUP_CLOSED(	placer_velocity,	c_placer::PARAM_ALL_NB )
				PARAM_PLACER(			placer_velocity )
			//PARAM_DEF_GROUP_CLOSED(	Dum, INIT_DUM_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		dum_circle_centered	)
			PARAM_DEF_BOOL_OFF(		dum1_forced			)
			PARAM_DEF_REAL_ZERO(	dum1_value			)
			PARAM_DEF_BOOL_OFF(		dum2_forced			)
			PARAM_DEF_REAL_ZERO(	dum2_value			)
			PARAM_DEF_BOOL_OFF(		dum3_forced			)
			PARAM_DEF_REAL_ZERO(	dum3_value			)
			PARAM_DEF_BOOL_OFF(		dum4_forced			)
			PARAM_DEF_REAL_ZERO(	dum4_value			)

		PARAM_DEF_GROUP_CLOSED(	Init Full,			INIT_FULL_PARAM_NB )
			PARAM_DEF_GROUP_CLOSED(	placer_c,			c_placer::PARAM_ALL_NB )
				PARAM_PLACER(			placer_c )
			PARAM_DEF_GROUP_CLOSED(	placer_accel,		c_placer::PARAM_ALL_NB )
				PARAM_PLACER(			placer_accel )
			//PARAM_DEF_GROUP_CLOSED(	value_init, INIT_VALUE_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		mass_forced			)
			PARAM_DEF_REAL_ONE(		mass_value			)
			PARAM_DEF_BOOL_OFF(		radius_forced		)
			PARAM_DEF_REAL_ONE(		radius_value		)
			PARAM_DEF_REAL_POS(		life_span,			 2., 1. )
			PARAM_DEF_REAL_INF(		life_span_offset,	.125, 0. )

		PARAM_DEF_GROUP_CLOSED( group_nb, BLOCK_SIZE_PARAM_NB )
			PARAM_DEF_INT32(		group_nb_x,			32, 1,	1, 1024*1024 )
			PARAM_DEF_INT32(		group_nb_y,			32, 1,	1, 1024*1024 )
			PARAM_DEF_INT32(		group_nb_z,			32, 1,	1, 1024*1024 )
			PARAM_DEF_BOOL_OFF(		dispatch_indirect	)

		PARAM_DEF_GROUP_CLOSED(	Filter, FILTER_PARAM_NB 	)
			PARAM_DEF_BOOL_OFF(			emitter_image				)
			PARAM_DEF_INT32_XY(			emitter_image_size,			1,128,	1,8192	)
			PARAM_DEF_POINT_FP32_XYZ(	emitter_image_color_min     )
			PARAM_DEF_REAL(				emitter_image_coverage,		1,.1,	0,1		)
			PARAM_DEF_INT32(			emitter_emit_nb,			1,32,	1,65536	)
			PARAM_DEF_BOOL_OFF(			emitter_counter_read		)
			PARAM_DEF_INT32_LOCKED(		emitter_counter_out			)	
			PARAM_DEF_BOOL_OFF(			free_indices_counter_read	)
			PARAM_DEF_INT32_LOCKED(		free_indices_counter_out	)
			PARAM_DEF_BOOL_OFF(			draw_indices_indirect		)

		PARAM_DEF_BOOL_OFF(		verbose			)
		PARAM_DEF_STR_LOCKED(	primitive_used	)

	};

	PARAM_DEF_MAKE_INDEX( init				);
	PARAM_DEF_MAKE_INDEX( init_full			);
	PARAM_DEF_MAKE_INDEX( placer			);
	PARAM_DEF_MAKE_INDEX( placer_b			);
	PARAM_DEF_MAKE_INDEX( placer_velocity	);
	PARAM_DEF_MAKE_INDEX( placer_c			);
	PARAM_DEF_MAKE_INDEX( placer_accel		);
}

namespace {
	o_str	o_tmp;
}

void	c_bdd_gl_part::prepare_for_ui()
{
	INT32 h = 1;
	prepare_for_ui_geo( h );

	//	placer's param_set need to be called before
	o_tmp.erase();
	if( _placer.get_type()			!=	c_placer::TYPE_NO )
		o_tmp.add( "pos "	);
	if( _placer_b.get_type()		!=	c_placer::TYPE_NO )
		o_tmp.add( "pos_b "	);
	if( _placer_vel.get_type()		!=	c_placer::TYPE_NO )
		o_tmp.add( "vel "	);
	get_param( PARAM_INDEX_init )->set_comment( o_tmp );

	o_tmp.erase();
	if( _placer_c.get_type()		!=	c_placer::TYPE_NO )
		o_tmp.add( "pos_c "	);
	if( _placer_accel.get_type()	!=	c_placer::TYPE_NO )
		o_tmp.add( "accel "	);
	get_param( PARAM_INDEX_init_full )->set_comment( o_tmp );

	_placer.		build_comment( o_tmp );
	get_param( PARAM_INDEX_placer			)->set_comment( o_tmp );

	_placer_b.		build_comment( o_tmp );
	get_param( PARAM_INDEX_placer_b			)->set_comment( o_tmp );

	_placer_vel.	build_comment( o_tmp );
	get_param( PARAM_INDEX_placer_velocity	)->set_comment( o_tmp );

	_placer_c.		build_comment( o_tmp );
	get_param( PARAM_INDEX_placer_c			)->set_comment( o_tmp );

	_placer_accel.	build_comment( o_tmp );
	get_param( PARAM_INDEX_placer_accel		)->set_comment( o_tmp );

}

void	c_bdd_gl_part::param_init_pt_static()
{
	INT32	h = param_init_pt_geo();

	param_set_pt( h, _b_compute_ui				);
	param_set_pt( h, _b_draw_ui					);
	param_set_pt( h, _b_particle_full_ui		);
	param_set_pt( h, _particle_nb_ui			);
	param_set_pt( h, _b_particle_realloc_always	);
	param_set_pt( h, _particle_nb_used_ui		);
	param_set_pt( h, _pass_nb_ui				);
	param_set_pt( h, _b_grid_2d_ui				);
	param_set_pt( h, _nb_u_ui					);
	param_set_pt( h, _b_restart_trig_ui			);
	param_set_pt( h, _b_lifetime_compute_ui		);

	++h;
		UINT32 index_placer_header = h;
		++h;
			_placer.		param_set( this, h );
		++h;
			_placer_b.		param_set( this, h );
		++h;
			_placer_vel.	param_set( this, h );
		param_set_pt( h, _b_dum_circle		);
		for( INT32 i=0; i<4; ++i )
		{
			param_set_pt( h, _b_dum_forced[i]	);
			param_set_pt( h, _dum_value[i]		);
		}

	++h;
		++h;
			_placer_c.		param_set( this, h );
		++h;
			_placer_accel.	param_set( this, h );
		param_set_pt( h, _b_mass_forced		);
		param_set_pt( h, _mass_value		);
		param_set_pt( h, _b_radius_forced	);
		param_set_pt( h, _radius_value		);
		param_set_pt( h, _life_span			);
		param_set_pt( h, _life_span_offset	);

	++h;
		param_set_pt_3(	h, _group_nb_ui			);
		param_set_pt(	h, _b_dispatch_indirect	);

	++h;
		param_set_pt(   h,	_b_emit_img_ui			);
		param_set_pt_2(	h,	_emit_img_size_ui		);
		param_set_pt_3( h,	_emit_img_color_min_ui	);
		param_set_pt(   h,	_emit_img_coverage_ui	);
		param_set_pt(   h,	_emit_nb_ui				);
		param_set_pt(   h,	_b_emit_counter_read_ui	);
		param_set_pt(   h,	_emit_counter_out_ui	);

		param_set_pt(	h,	_b_free_indices_counter_read_ui		);
		param_set_pt(	h,	_free_indices_counter_out_ui		);
		param_set_pt(	h,	_b_draw_indices_indirect_ui			);

	//++h;
	param_set_pt( h, _b_verbose					);
	param_set_pt( h, _draw_prim_used			);

	//++h;

	err_param_init_pt( h );
}

gl::ubo*	ubo_particle_nb = nullptr;

CONSTRUCTOR_CREATE( c_bdd_gl_part )
	,_particles				(nullptr)
	,_buffer_particle		(nullptr)
//	,_particle_nb_ubo		(nullptr)
	,_particle_nb_allocated	(0)
	,_particle_nb			(0)
	,_nb_u					(0)
	,_b_lifetime_compute	(false)
	,_gol_id_dispatch		(0)
	,_buffer_indices_free	(nullptr)
	,_buffer_indices_active	(nullptr)
	,_buffer_accum			(nullptr)
	,_emitter_img			(nullptr)
{
	param_init_with( param, PARAM_NB );

	if( is_obj_first() )
		ubo_particle_nb = gl::ubo::make( 4, true, nullptr, "particle_nb" );
}

c_bdd_gl_part::~c_bdd_gl_part()
{
	if( is_obj_first() )
		gl::ubo::release_and_null( ubo_particle_nb );
	release_gl_buffers();
	SAFE_DELETE( _emitter_img );
}

void c_bdd_gl_part::release_gl_buffers()
{
	gl::ssbo::release_and_null( _buffer_particle );

	for( gl::ssbo* buffer : _buffers_rw )
		gl::ssbo::release_and_null( buffer );
	_buffers_rw.clear();

	_particle_nb_allocated	= 0;
	_particle_nb			= 0;

	gl::ssbo::release_and_null( _buffer_indices_free	);
	gl::ssbo::release_and_null( _buffer_indices_active	);
	gl::ssbo::release_and_null( _buffer_accum			);
}

namespace {
	CONSTEXPR	INT32		BUFFER_RW_NB_MAX = 8;
	CONSTEXPR	INT32		BUFFER_OUT_NB = 4;		// vec4 for position, normal, color, attribute
													//  they are the first in the buffers_rw
				CHAR		name[] = "buffer_rw__";
}

AAA_ERR	c_bdd_gl_part::alloc_particle()
{
	//todo _b_lifetime_change compute don't have to force release 
	if( _b_particle_full != _b_particle_full_ui || _b_lifetime_compute != _b_lifetime_compute_ui)
	{
		release_gl_buffers();	//refine just particle buffer
		_b_particle_full = _b_particle_full_ui;
		_b_lifetime_compute = _b_lifetime_compute_ui;
	}

	UINT32	nb_needed = _particle_nb_ui;
	if( _particle_nb_allocated == nb_needed )
		return AAA_OK;

	//todo deal with change in full or not full
	if( _b_particle_realloc_always || _particle_nb_allocated < nb_needed )
	{
		int buffer_size = nb_needed * get_particle_size();
		int out_buffer_size = nb_needed * sizeof(glm::vec4);

		_particles = (UINT8*) REALLOC_ALIGNED_SIGNATURE( _particles, buffer_size , __FUNCTION__ );
		if( !_particles )
			goto alloc_failed;

		release_gl_buffers();

		_buffer_particle = gl::ssbo::make( buffer_size,		false, nullptr, "particle"   );
		if( !_buffer_particle )
			goto alloc_failed;

		for( INT32 i=0; i<BUFFER_OUT_NB; ++i )
		{
			name[sizeof(name)-2] = 0x30+i;
			gl::ssbo* buffer = gl::ssbo::make( out_buffer_size,	false, nullptr, name );
			if( buffer )
				_buffers_rw.push_back( buffer );
			else
				goto alloc_failed;
		}
		if( _b_lifetime_compute )
		{
			_buffer_indices_free	= gl::ssbo::make( nb_needed * sizeof(INT32), false, nullptr, "free indices"   );	
			_buffer_indices_active	= gl::ssbo::make( nb_needed * sizeof(INT32), false, nullptr, "active indices" );

			GOL::st_particle_accum_buffer init;
			init._active_nb = 0;
			init._free_nb = nb_needed; // we will reset anyway
			init._prim_nb = 1; //instance count, always 1 otherwise no draw
			init._first = 0;
			init._base_instance = 0;

			_buffer_accum = gl::ssbo::make( sizeof(GOL::st_particle_accum_buffer), false, (char*)&init, "accumulator" );
		}

		_particle_nb_allocated	= nb_needed;
		_b_restart_trig_ui		= true;
	}
	_particle_nb = _particle_nb_ui;
	return AAA_OK;

alloc_failed:
	release_gl_buffers();
	err_print( "can't alloc buffer" );
	return ERR_MEM_BASE;
}


//todo already done for opencl avoid duplication
void c_bdd_gl_part::place( c_placer& placer, UINT32 nb, float* dst )
{
	placer.update();

	UINT32 stride = get_particle_size() / sizeof(float);
	if( placer.is_input_needed() )
	{
		float* src = &( ((GOL::st_particle_full*)_particles)->pos_b.x );
		placer.place( nb, dst, stride, src, stride );
	}
	else
	{
		FP32	size[3];
		c_model::cur->get_size_v3( size );	
		placer.place( nb, dst, stride, dst, stride );
		for( UINT32 id=nb; id>0; --id )
		{
			mul_v3( dst, size );
			dst += stride;
		}
	}
}

void	c_bdd_gl_part::init_particle()
{
	if( _b_verbose )
		DBG_PRINT_STRING( "%s()", __FUNCTION__ );

	SPY_PUSH_RANGE( "bdd_gl_part::init_particle()", spy::COL_4 );

		REAL	size[3];
		c_model::cur->get_size_v3( size );

		UINT32 nb = _particle_nb_allocated;

		GOL::st_particle_full* p = (GOL::st_particle_full*)_particles;
		//DBG_PRINT_STRING( "%s() with dt %f on dataset %d : start %d, nb %d.", __FUNCTION__, _kernel_param.dt, _dataset_id, 0, nb );
		_nb_u = _nb_u_ui;
		INT32 stride = get_particle_size();

// pos vel pos_b common to full and compact
		if( _b_grid_2d_ui )
		{
			INT32 nb_v = _particle_nb_used / _nb_u;
			REAL fu = OVER_ONE_AS_REAL( _nb_u );
			REAL fv = OVER_ONE_AS_REAL( nb_v );
			for( INT32 iv = 0; iv < nb_v; ++iv )
			{
				REAL v  = ((iv+REAL(.5)) * fv - REAL(.5)) * size[0];
				for( INT32 iu = 0; iu < _nb_u; ++iu )
				{
					REAL u  = ((iu+REAL(.5)) * fu - REAL(.5)) * size[1];
					p->pos.x = u;
					p->pos.y = v;
					p->pos.z = 0.0f;
					p->pos.w = 1.0f;
					p = (GOL::st_particle_full*) ( ((UINT8*)p) + stride );
				}
			}
			for( UINT32 i = nb - (nb_v * _nb_u); i > 0; --i )
			{
				p->pos.x = 0.0f;
				p->pos.y = 0.0f;
				p->pos.z = 0.0f;
				p->pos.w = 1.0f;
				p = (GOL::st_particle_full*)(((UINT8*)p) + stride);
			}
			p = (GOL::st_particle_full*)_particles;
		}
		else
			place( _placer,			nb, &(p->pos.x) );

		place( _placer_vel,		nb, &(p->vel.x)		);
		place( _placer_b,		nb, &(p->pos_b.x)	);

		if( _b_particle_full )
		{
			place( _placer_c,		nb, &(p->pos_c.x)	);
			// todo p->vel.set( 0, 0, 0, 0 );
			place( _placer_accel,	nb, &(p->accel.x)	);
		}

		for( UINT32 id=0; id<nb; ++id )
		{	//todo strategy should be refined, lua used and fourth coor treated
			p = (GOL::st_particle_full*)(_particles + id * stride);
			//hack
			INT32 count = sta_rand.get_count();
			if( _b_particle_full )
			{
				p->mass				= _b_mass_forced	? _mass_value	: sta_rand.get_fp32_01();
				p->radius			= _b_radius_forced	? _radius_value	: sta_rand.get_fp32_01();
				p->life				= _life_span + sta_rand.get_fp32_max(_life_span_offset);
				p->life_over_one	= OVER_ONE_AS_FP32( p->life );
			}

			if( _b_dum_circle )
			{
				sta_rand.get_circle( p->dum[0], p->dum[1] );
				sta_rand.get_circle( p->dum[2], p->dum[3] );
			}
			else
			{
				p->dum[0]	=	_b_dum_forced[0]	? _dum_value[0]	: sta_rand.get_fp32_01();
				p->dum[1]	=	_b_dum_forced[1]	? _dum_value[1]	: sta_rand.get_fp32_01();	
				p->dum[2]	=	_b_dum_forced[2]	? _dum_value[2]	: sta_rand.get_fp32_01();	
				p->dum[3]	=	_b_dum_forced[3]	? _dum_value[3]	: sta_rand.get_fp32_01();	
			}

			//hack this avoid patterns
			if( ((sta_rand.get_count() - count) & 0x1) == 0 )
				sta_rand.get_fp32_01();
		
			//why this was here
			//_particles_pos[i*2] = p.pos;
			//_particles_pos[i*2+1] = p.pos;
		}


		_b_restart_trig_ui = false;

	SPY_POP_RANGE()
}

void	c_bdd_gl_part::restart()
{
	_b_restart_trig_ui = true;
}

INT32	c_bdd_gl_part::get_particle_size()
{
	return _b_particle_full ? sizeof(GOL::st_particle_full) : sizeof(GOL::st_particle_compact);
}

void	c_bdd_gl_part::update()
{
	//rebuilds buffers if required
	alloc_particle();

	if( _b_compute_ui )
	{
		if( _b_restart_trig_ui )
		{
			init_particle();

			int alloc_size = _particle_nb_allocated * get_particle_size();

			//also copy ssbo data
			_buffer_particle->write( _particles, alloc_size, 0, true );
		}
		
		_particle_nb_used = MIN( _particle_nb_used_ui, _particle_nb );

		compute();

		if( _b_free_indices_counter_read_ui && _buffer_accum )
			_buffer_accum->read( &_free_indices_counter_out_ui, 4, 0 );
	}

	INT32 prim = c_render::get_cur()->get_draw_primitive();
	switch( prim )
	{
	case GL_LINES:
	case GL_LINE_LOOP:
	case GL_LINE_STRIP:
		prim = GL_POINTS;
		break;
	default:
		break;
	}
	_draw_prim_used.set( GOL::draw_primitive_str[prim] );

}

void	c_bdd_gl_part::compute()
{
	auto sha = c_shading::get_cur();
	if( sha && sha->is_compute_valid() )
	{
		bool b_need_bind_render;
		if( !sha->bind_compute(0) )
			b_need_bind_render = false;
		else
		{
			b_need_bind_render = true;

			SPY_PUSH_RANGE( "bdd_gl_part::compute()", spy::GOL_HIGH );
//BIND
				_buffer_particle->bind(0);

				if( _b_lifetime_compute )
				{
					_buffer_indices_free->bind(1);
					_buffer_indices_active->bind(2);
					_buffer_accum->bind(3);

					//reset accum counters
					GOL::st_particle_accum_buffer init;
					init._active_nb		= 0;
					init._free_nb		= 0; // we will reset anyway
					init._prim_nb		= 1; // instance count, always 1 otherwise no draw
					init._first			= 0;
					init._base_instance	= 0;

					_buffer_accum->write( &init, sizeof(GOL::st_particle_accum_buffer), 0, true );
				}

				CONSTEXPR UINT32 BUFFER_BEGIN = 4;
				for( INT32 i=0; i<BUFFER_OUT_NB; ++i )
					_buffers_rw[i]->bind( BUFFER_BEGIN + i );

				ubo_particle_nb->bind_and_write( 0, &_particle_nb_used, 4 );

//COMPUTE
	//same but two calls
	//			_particle_nb_ubo->bind( 0 );
	//			_particle_nb_ubo->write( &_particle_nb_used, 4, 0, true );

				//todo move to GOL
				//todo check we have compute
				//todo glGet GL_MAX_COMPUTE_WORK_GROUP_COUNT with glGetIntegeri_v
				//todo GL_MAX_COMPUTE_WORK_GROUP_SIZE 
				//todo understand glDispatchCompute
				//todo : idivup since we might not have a multiple of group size
				st_workgroup wg;
				wg.x = _particle_nb_used / ( _group_nb_ui[0] * _group_nb_ui[1] * _group_nb_ui[2] );
				wg.y = _group_nb_ui[1];
				wg.z = _group_nb_ui[1];
				if( _b_dispatch_indirect )
				{
					if( !_gol_id_dispatch )
					{
						GOL::gen_buffer( _gol_id_dispatch );
						_workgroup.x = 0;
						_workgroup.y = 0;
						_workgroup.z = 0;
					}
					GOL::bind_buffer( GL_DISPATCH_INDIRECT_BUFFER, _gol_id_dispatch );

					if( _workgroup.x != wg.x || _workgroup.y != wg.y || _workgroup.y != wg.y )
					{
						_workgroup.x = wg.x;
						_workgroup.y = wg.y;
						_workgroup.z = wg.z;
						GOL::set_buffer_data( GL_DISPATCH_INDIRECT_BUFFER, sizeof(_workgroup), &_workgroup, GL_STATIC_DRAW );
					}
					GOL::dispatch_compute_indirect(	0 );
				}
				else
					GOL::dispatch_compute( wg.x, wg.y, wg.z );

	// got an error in Monaco October Session
	//			GOL::dispatch_compute_group_size(	nb_workgroup,		_group_nb_ui[1],	_group_nb_ui[2],
	//												_group_nb_ui[0],	_group_nb_ui[1],	_group_nb_ui[2]	);

				GOL::barrier_memory( GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT | GL_COMMAND_BARRIER_BIT);

//UNBIND
				for( INT32 i=BUFFER_OUT_NB-1; i>=0; --i )
					_buffers_rw[i]->unbind( BUFFER_BEGIN + i );
				if( _b_lifetime_compute )
				{
					_buffer_indices_free->unbind(1);
					_buffer_indices_active->unbind(2);
					_buffer_accum->unbind(3);
				}
				_buffer_particle->unbind(0);

			SPY_POP_RANGE()
		}

		if( _b_emit_img_ui )
		{
			//filter image in slot 2, does only need its own buffers attached
			if( sha->bind_compute(2) )
			{	
				b_need_bind_render = true;
				if( !_emitter_img )
					_emitter_img = new c_emitter_img;
				if( !_emitter_img )
					err_print( "%s() Can't alloc emitter_image : ", __FUNCTION__ );
				else
				{
					_emitter_img->set_size( _emit_img_size_ui[0], _emit_img_size_ui[1] );

					auto emit = _emitter_img->get_emit();

					cpy_v3( emit->filter._color_min, &_emit_img_color_min_ui[0] );
					emit->filter._coverage = _emit_img_coverage_ui;

					_emitter_img->filter();

					//buffer emit in slot 3
					if( sha->bind_compute(3) )
					{
//BIND			
						_buffer_particle->bind(0);
						if( _b_lifetime_compute )
						{
							_buffer_indices_free->bind(1);
							_buffer_indices_active->bind(2);
							_buffer_accum->bind(3);
						}
						CONSTEXPR UINT32 BUFFER_BEGIN = 4;
						for( INT32 i=0; i<BUFFER_OUT_NB; ++i )
							_buffers_rw[i]->bind( BUFFER_BEGIN + i );
//COMPUTE
// 						//dispatch buffer gl
						//to add a by second like in flex
						UINT32 max_emit = _emit_nb_ui;
						_emitter_img->emit( max_emit, BUFFER_BEGIN + BUFFER_OUT_NB );
//UNBIND
						for( INT32 i=BUFFER_OUT_NB-1; i>=0; --i )
							_buffers_rw[i]->unbind( BUFFER_BEGIN + i );
						if( _b_lifetime_compute )
						{
							_buffer_accum->unbind(3);
							_buffer_indices_active->unbind(2);
							_buffer_indices_free->unbind(1);
						}
						_buffer_particle->unbind(0);
//READ
						if( _b_emit_counter_read_ui )
							_emit_counter_out_ui = _emitter_img->read_counter();
					}
				}
			}
	
		}
		if( b_need_bind_render )
			sha->bind_render();
	}
}

void	c_bdd_gl_part::draw()
{
	auto sha = c_shading::get_cur();
	//todo refine
	if( _b_draw_ui && sha && sha->is_compute_valid() )
	{
		SPY_PUSH_RANGE( "bdd_gl_part::draw()", spy::GOL_HIGH );

		//BIND
			for( INT32 i=0; i<BUFFER_OUT_NB; ++i )
				_buffers_rw[i]->bind( i );
			if( _b_lifetime_compute_ui )
				_buffer_indices_active->bind(BUFFER_OUT_NB);

		//DRAW
			INT32 prim = c_render::get_cur()->get_draw_primitive();
			switch( prim )
			{
			default:
			case GL_POINTS:
				//todo what we do here for _pass_nb_ui
				if( _b_draw_indices_indirect_ui && _b_lifetime_compute_ui )
				{
					GOL::bind_buffer( GL_DRAW_INDIRECT_BUFFER, _buffer_accum->get_gol_id() );
						//GOL::draw_arrays_instanced( GL_POINTS, _free_indices_counter_out_ui, 1 );
						GOL::draw_arrays_indirect( GL_POINTS, nullptr );
					GOL::unbind_buffer( GL_DRAW_INDIRECT_BUFFER );
				}
				else     
					GOL::draw_arrays_instanced( GL_POINTS, _particle_nb_used, _pass_nb_ui );
				break;
			case GL_TRIANGLES:
				GOL::draw_arrays_instanced( GL_TRIANGLES, _particle_nb_used * 6, _pass_nb_ui );
				break;
			case GL_TRIANGLE_STRIP:
			case GL_TRIANGLE_FAN:
			case GL_QUADS:
			case GL_QUAD_STRIP:
			case GL_POLYGON:
				GOL::draw_arrays_instanced( prim, 4, _particle_nb_used * _pass_nb_ui );
				break;
			}
			//glDrawArraysIndirect()
			//draw elements
			
		//UNBIND
			if( _b_lifetime_compute_ui )
				_buffer_indices_active->unbind(BUFFER_OUT_NB);
			for( INT32 i=BUFFER_OUT_NB-1; i>=0; --i )
				_buffers_rw[i]->unbind( i );

		SPY_POP_RANGE();
	}
}
