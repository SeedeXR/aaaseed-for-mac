#include "bdd_opencl_part.h"
#include "spy.h"
#include "draw/tex.h"
#include "math/rand.h"
#ifndef AAA_SEED_UI_H
#	include "ui/seed_ui.h"
#endif
#include "image/bind_img.h"
#include "image/bind_img_2d.h"
#include "Shaders/shading.h"
#include "draw/model.h"
#include "draw/render.h"
#include "language/lua/aaalua_master.h"	//todo move trigger_edit_file in a better place

#if	0
INT32	adapt_to_work_group_size( INT32 nb, INT32 size )
{
	INT32	nb_new = ROUND_FLOOR( nb, size );
	if( nb_new != nb )	{	nb_new += size;	}
	return nb_new;
}
#endif

typedef struct{
	float4	pos;
	float4	vel;
	float4	pos_b;
	float	dum1;	float	dum2;	float	dum3;	float	dum4;
	float4	accel;
	float4	pos_c;
	float	mass;
	float	radius;
	float	life;
	float	life_over_one;
	// need this to make sure the float2 vel is aligned to a 16 byte boundary
} cl_particle_full;

typedef struct{
	float4	pos;
	float4	vel;
	float4	pos_b;
	float4	dum;
} cl_particle_compact;


// maa say be careful with structure aligned to follow cl side and have the same side
//	no LF make error number offet of 1 only
//#define LF	"\n"
#define LF
o_str o_include_particle_full(
"	typedef struct{" LF
"	float4 pos;" LF
"	float4 vel;" LF
"	float4 pos_b;" LF
"	float dum1;		float dum2;		float dum3;		float dum4;" LF
"	float4 accel;" LF
"	float4 pos_c;" LF
"	float mass;" LF
"	float radius;" LF
"	float life;" LF
"	float life_over_one;" LF
"	} Particle;\n"
);
o_str o_include_particle(
"	typedef struct{" LF
"	float4 pos;" LF
"	float4 vel;" LF
"	float4 pos_b;" LF
"	float4 dum;" LF
"	} Particle;\n"
);

o_str o_include_param(
"	typedef struct{" LF
"	float	line;	float	color;		float	normal;		float	attrib;" LF
"	float	dt;		uint	i_offset; 	uint	nb_by_set;	uint	float_by_set;" LF

"	float2	mouse_pos;" LF

"	float	p01;	float	p02;	float	p03;	float	p04;" LF
"	float	p05;	float	p06;	float	p07;	float	p08;" LF
"	float	p09;	float	p10;	float	p11;	float	p12;" LF
"	float	p13;	float	p14;	float	p15;	float	p16;" LF
"	float	p17;	float	p18;" LF
	
"	float4	v01;	float4	v02;	float4	v03;	float4	v04;" LF

"	} kernel_param;" LF

"	typedef struct{" LF
"		float	v[200];" LF
"	} dataset_param;\n" LF
);

FACTORY_CREATE_PROP_V1( c_bdd_opencl_part, bdd_opencl_part, OpenCL Part, bdd_opencl_part, sub_menu="Point"; );

static	c_rand_lin		sta_rand;
static	CONST	UINT32	PARTICLES_NB_MAX = 1024 * 1024 * 256;

static	CONST	UINT32	ARG_PART_ID		= 0;
static	CONST	UINT32	ARG_POINT_ID	= ARG_PART_ID + 1;
static	CONST	UINT32	ARG_COLOR_ID	= ARG_PART_ID + 2;
static	CONST	UINT32	ARG_NORMAL_ID	= ARG_PART_ID + 3;
static	CONST	UINT32	ARG_ATTRIB_ID	= ARG_PART_ID + 4;

static	CONST	UINT32	ARG_KERNEL_ID	= ARG_PART_ID + 5;
static	CONST	UINT32	ARG_DATASET_ID	= ARG_PART_ID + 6;

//static	CONST	UINT32	ARG_TEX_ID		= ARG_PART_ID + 7;
//todo move and do an opencl master
static	bool	b_opencl_do = false;

FINLINE bool IS_BDD_OPENCL_PART_DO()	{	return AAA_OPENCL::c_opencl::gb_opencl_possible && AAA_OPENCL::c_opencl::gb_opencl_allow_ui && b_opencl_do;	}

static	C_PCHAR_C	str_texture_usage[ 3 ] = 
{
	"READ",
	"WRITE",
	"READ_WRITE"
};
static INT32	cl_texture_usage[ 3 ] =
{
	CL_MEM_READ_ONLY,
	CL_MEM_WRITE_ONLY,
	CL_MEM_READ_WRITE,
};

namespace
{
#define	PARAM_DEF_TEXTURE( name )\
	PARAM_DEF_BOOL_OFF(			name##_use		)\
	PARAM_DEF_SYMBO_PSTR_ZERO(	name##_usage,	str_texture_usage )\
	PARAM_DEF_BIND_2D_CURRENT(	name##_bind		)

	CONSTEXPR INT32 BASE_PARAM_NB		= c_bdd::GEO_PARAM_NB + 17;
	CONSTEXPR INT32 DEVICE_PARAM_NB		= 8;
	CONSTEXPR INT32 KERNEL_PARAM_NB		= 6;
	CONSTEXPR INT32 VALUE_INIT_PARAM_NB	= 15;
	CONSTEXPR INT32 TEX_PARAM_NB		= c_bdd_opencl_part::TEXTURE_NB * 3 + 2;
	CONSTEXPR INT32 TEK_PARAM_NB		= 11;
    
	CONSTEXPR INT32 ARG_FIRPARAM_DEF_NB	= 6;
	CONSTEXPR INT32 ARG_MOUSE_PARAM_NB	= 2;
	CONSTEXPR INT32 ARG_FLOAT_PARAM_NB	= c_bdd_opencl_part::KERNEL_FLOAT_NB;
	CONSTEXPR INT32 ARG_FLOAT4_PARAM_NB	= c_bdd_opencl_part::KERNEL_FLOAT4_NB * 4;
    
	CONSTEXPR INT32 ARG_PARAM_NB		= ARG_FIRPARAM_DEF_NB +  ARG_FLOAT_PARAM_NB + ARG_FLOAT4_PARAM_NB + 3;
   
	CONSTEXPR INT32 DATASET_PARAM_NB	= 1 + 16;
	CONSTEXPR INT32 MOUSE_PARAM_NB		= 6;
	CONSTEXPR INT32 GROUP_PARAM_NB		= 8;
   
	CONSTEXPR INT32 INIT_PARAM_NB		= 5 * (c_placer::PARAM_ALL_NB + 1) + VALUE_INIT_PARAM_NB + 1;

	CONSTEXPR INT32 PARAM_NB	=	BASE_PARAM_NB
								+	DEVICE_PARAM_NB
								+	KERNEL_PARAM_NB
								+	INIT_PARAM_NB
								+	TEX_PARAM_NB
								+	TEK_PARAM_NB
								+	ARG_PARAM_NB
								+	DATASET_PARAM_NB
								+	MOUSE_PARAM_NB
								+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_BOOL_OFF(		active			)

		PARAM_DEF_GROUP_CLOSED(	Device, DEVICE_PARAM_NB )
			PARAM_DEF_SYMBO_NEG(		platform,				0, -1,	-1, 12, AAA_OPENCL::str_platform_default		)
			PARAM_DEF_STR_LOCKED(		platform_name			)
			PARAM_DEF_SYMBO_PSTR_ZERO(	device_type,			AAA_OPENCL::str_cl_device_type )
			PARAM_DEF_INT32_POS(		device_index,			1, 0		)
			PARAM_DEF_STR_LOCKED(		device_name				)
			PARAM_DEF_STR_LOCKED(		device_version			)
			PARAM_DEF_STR_LOCKED(		device_driver_version	)
			PARAM_DEF_STR_LOCKED(		device_version			)


		PARAM_DEF_INT32(		dataset_nb,			2, 1,		1, 1024 )
		PARAM_DEF_INT32(		dataset_id,			2, 1,		1, 1024 )

		PARAM_DEF_BOOL_ON(		point_full_version	)
		PARAM_DEF_INT32(		point_nb,			1, 1024*32,	1, PARTICLES_NB_MAX )
		PARAM_DEF_BOOL_OFF(		point_nb_alloc_when_changed		)
		PARAM_DEF_INT32(		point_nb_used,		0, 1024*32,	1, PARTICLES_NB_MAX )
//		PARAM_DEF_INT32_LOCKED(	offset_maa	 )

		PARAM_DEF_BOOL_OFF(		grid_2d			)
		PARAM_DEF_INT32(		nb_u,		32, 256,		2, PARTICLES_NB_MAX )

		PARAM_DEF_BOOL_OFF_SAVE_NOT(	restart_trig	)


		PARAM_DEF_GROUP_CLOSED(	Init,			INIT_PARAM_NB )
			PARAM_DEF_GROUP_CLOSED(	placer,				c_placer::PARAM_ALL_NB )
				PARAM_PLACER(			placer )
			PARAM_DEF_GROUP_CLOSED(	placer_b,			c_placer::PARAM_ALL_NB )
				PARAM_PLACER(			placer_b )
			PARAM_DEF_GROUP_CLOSED(	placer_c,			c_placer::PARAM_ALL_NB )
				PARAM_PLACER(			placer_c )
			PARAM_DEF_GROUP_CLOSED(	placer_velocity,	c_placer::PARAM_ALL_NB )
				PARAM_PLACER(			placer_velocity )
			PARAM_DEF_GROUP_CLOSED(	placer_accel,		c_placer::PARAM_ALL_NB )
				PARAM_PLACER(			placer_accel )

			PARAM_DEF_GROUP_CLOSED(	value_init, VALUE_INIT_PARAM_NB )		
				PARAM_DEF_BOOL_OFF(		mass_forced			)
				PARAM_DEF_REAL_ONE(		mass_value			)
				PARAM_DEF_BOOL_OFF(		radius_forced		)
				PARAM_DEF_REAL_ONE(		radius_value		)
				PARAM_DEF_BOOL_OFF(		dum_circle_centered	)
				PARAM_DEF_BOOL_OFF(		dum1_forced			)
				PARAM_DEF_REAL_ZERO(	dum1_value			)
				PARAM_DEF_BOOL_OFF(		dum2_forced			)
				PARAM_DEF_REAL_ZERO(	dum2_value			)
				PARAM_DEF_BOOL_OFF(		dum3_forced			)
				PARAM_DEF_REAL_ZERO(	dum3_value			)
				PARAM_DEF_BOOL_OFF(		dum4_forced			)
				PARAM_DEF_REAL_ZERO(	dum4_value			)
				PARAM_DEF_REAL_POS(		life_span,			 2., 1. )
				PARAM_DEF_REAL_INF(		life_span_offset,	.125, 0. )

		PARAM_DEF_GROUP_CLOSED(	Texture, TEX_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		texture_bind_switch_auto	)
			PARAM_DEF_BOOL_OFF(		texture_bind_switch			)
			PARAM_DEF_8(			texture,					PARAM_DEF_TEXTURE	)
	
		PARAM_DEF_GROUP_CLOSED(	Kernel, KERNEL_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		reload_trig			)
			PARAM_DEF_FILENAME(		filename,			aaa::file::TYPE_IO_OPENCL, 0 )
			PARAM_DEF_REF(			kernel_name			)
			PARAM_DEF_INT32_POS(	block_size_u,		8, 64 )
			PARAM_DEF_INT32_POS(	block_size_v,		8, 64 )
			PARAM_DEF_INT32_POS(	block_size_w,		4, 1 )
			
		PARAM_DEF_BOOL_LOCKED(	ready_to_run			)
		PARAM_DEF_INT32_LOCKED(	run_count				)
		PARAM_DEF_REAL_POS(		time_interval_max,		0.05, 0.04 )
		PARAM_DEF_BOOL_ON(		real_time				)
		PARAM_DEF_REAL_POS(		time_internal,			1., 0.02 )
		PARAM_DEF_BOOL_OFF(		dataset_restart_trig	)

		PARAM_DEF_GROUP_CLOSED(	TEK, TEK_PARAM_NB )
			PARAM_DEF_BOOL_ON(		bdd_opencl_part_do	)
			PARAM_DEF_BOOL_OFF(		verbose				)
			PARAM_DEF_BOOL_LOCKED(	use_gl_interop		)
			PARAM_DEF_BOOL_ON(		do_gol_finish		)
			PARAM_DEF_BOOL_OFF(		do_gol_flush		)
			PARAM_DEF_BOOL_ON(		do_acquire_release	)
			PARAM_DEF_BOOL_ON(		wait_post_acquire	)
			PARAM_DEF_INT32(		maa_loop_nb,		0, 1,	0, 1024 )
			PARAM_DEF_BOOL_ON(		wait_post_run		)
			PARAM_DEF_BOOL_ON(		wait_post_release	)
			PARAM_DEF_BOOL_ON(		do_cl_finish		)

		PARAM_DEF_BOOL_OFF(		draw_quads		)

		PARAM_DEF_GROUP_CLOSED(	Kernel_Argument, ARG_PARAM_NB + MOUSE_PARAM_NB + 1 )
			PARAM_DEF_GROUP(			first, ARG_FIRPARAM_DEF_NB )
				PARAM_DEF_FP32_ZERO(		line			)
				PARAM_DEF_FP32_ZERO(		color			)
				PARAM_DEF_FP32_ZERO(		normal			)
				PARAM_DEF_FP32_ZERO(		attrib			)
				PARAM_DEF_INT32_POS_ONE(	nb_by_set		)
				PARAM_DEF_INT32_POS_ZERO(	float_by_set	)
			
			PARAM_DEF_GROUP_CLOSED(	mouse, MOUSE_PARAM_NB )
				PARAM_DEF_POINT_XY(				mouse_offset	)
				PARAM_DEF_SCALE_XY(				mouse_scale		)
				PARAM_DEF_FP32_ZERO_SAVE_NOT(	mouse_pos_x		)
				PARAM_DEF_FP32_ZERO_SAVE_NOT(	mouse_pos_y		)

			PARAM_DEF_GROUP(		float, ARG_FLOAT_PARAM_NB )
				PARAM_DEF_01_18(		param, PARAM_DEF_FP32_ZERO	)

		PARAM_DEF_GROUP_CLOSED(	float4, ARG_FLOAT4_PARAM_NB )
			PARAM_DEF_04(			vec,		PARAM_DEF_POINT_FP32_XYZW	)

		PARAM_DEF_GROUP_CLOSED(	Dataset_data, DATASET_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		float_pass_by_address	)
			PARAM_DEF_16(			float,		PARAM_DEF_FP32_ZERO	)
	};
}

static o_str	o_tmp;
static UINT32	index_init;
static UINT32	index_stride;
static UINT32	index_texture;

void	c_bdd_opencl_part::prepare_for_ui()
{
	INT32 h = 1;
	prepare_for_ui_geo( h );

	h = index_init;

	//	placer's param_set need to be called before
	o_tmp.erase();
	if( _placer.get_type()			!=	c_placer::TYPE_NO )
		o_tmp.add( "pos "	);
	if( _placer_b.get_type()		!=	c_placer::TYPE_NO )
		o_tmp.add( "pos_b " );
	if( _placer_c.get_type()		!=	c_placer::TYPE_NO )
		o_tmp.add( "pos_c " );
	if( _placer_vel.get_type()		!=	c_placer::TYPE_NO )
		o_tmp.add( "vel "	);
	if( _placer_accel.get_type()	!=	c_placer::TYPE_NO )
		o_tmp.add( "accel " );

	get_param(h)->set_comment( o_tmp );

	++h;
		_placer.		build_comment( o_tmp );
		get_param(h)->set_comment( o_tmp );
	h += index_stride;
		_placer_b.		build_comment( o_tmp );
		get_param(h)->set_comment( o_tmp );
	h += index_stride;
		_placer_c.		build_comment( o_tmp );
		get_param(h)->set_comment( o_tmp );
	h += index_stride;
		_placer_vel.	build_comment( o_tmp );
		get_param(h)->set_comment( o_tmp );
	h += index_stride;
		_placer_accel.	build_comment( o_tmp );
		get_param(h)->set_comment( o_tmp );

	o_tmp.erase();
	h = index_texture;
	for( UINT32 i = 0; i < TEXTURE_NB; ++i )
	{
		auto& tui = _texture_ui[i];
		if( tui.b_use )
		{
			C_PCHAR str;
			o_tmp.add( tui.bind );
			switch( tui.usage )
			{
			case 0:	str = "_R";		break;
			case 1:	str = "_W";		break;
			case 2:	str = "_RW";	break;
			}
			o_tmp.add( str );

			o_tmp.add_space();
		}
	}
	get_param(h)->set_comment( o_tmp );
}

void	c_bdd_opencl_part::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt( h, get_pt_active()			);

	++h;
		param_set_pt( h, _platform				);
		param_set_pt( h, _platform_name			);
		param_set_pt( h, _device_type			);
		param_set_pt( h, _device				);
		param_set_pt( h, _device_name			);
		param_set_pt( h, _device_version		);
		param_set_pt( h, _device_driver_version	);
		param_set_pt( h, _device_vendor			);

	param_set_pt( h, _dataset_nb_ui				);
//	param_set_max_no_inc( h, _dataset_nb_ui );	// work only when in interface
	param_set_pt( h, _dataset_id				);

	param_set_pt( h, _b_particle_full_ui		);
	param_set_pt( h, _particle_nb_ui			);
	param_set_pt( h, _b_particle_realloc_always	);
	param_set_pt( h, _particle_nb_used			);

//	param_set_pt( h, _particle_offset			);

	param_set_pt( h, _b_grid_2d_ui				);
	param_set_pt( h, _nb_u_ui					);

	param_set_pt( h, _b_restart_trig_ui			);


	index_init = h;
	++h;
		UINT32 index_placer_header = h;
		++h;
			_placer.		param_set( this, h );
		index_stride = h - index_placer_header;
		++h;
			_placer_b.		param_set( this, h );
		index_placer_header = h;
		++h;
			_placer_c.		param_set( this, h );
		index_placer_header = h;
		++h;
			_placer_vel.	param_set( this, h );
		index_placer_header = h;
		++h;
			_placer_accel.	param_set( this, h );
	
	++h;
		param_set_pt( h, _b_mass_forced		);
		param_set_pt( h, _mass_value		);
		param_set_pt( h, _b_radius_forced	);
		param_set_pt( h, _radius_value		);
		param_set_pt( h, _b_dum_circle		);
		for( INT32 i=0; i<4; ++i )
		{
			param_set_pt( h, _b_dum_forced[i]	);
			param_set_pt( h, _dum_value[i]		);
		}
		param_set_pt( h, _life_span			);
		param_set_pt( h, _life_span_offset	);

		index_texture = h;
		++h;
			param_set_pt( h, _b_texture_switch_auto_ui );
			param_set_pt( h, _b_texture_switch_ui );
			for( UINT32 i = 0; i < TEXTURE_NB; ++i )
			{
				auto& tui = _texture_ui[i];
				param_set_pt( h, tui.b_use	);
				param_set_pt( h, tui.usage	);
				param_set_pt( h, tui.bind	);
			}

	++h;
		param_set_pt( h, _b_reload_file_trig	);
		param_set_pt( h, _script_filename_ui	);
		param_set_pt( h, _kernel_name			);
		param_set_pt( h, _block_size[0]			);
		param_set_pt( h, _block_size[1]			);
		param_set_pt( h, _block_size[2]			);

	param_set_pt( h, _b_ready_to_run			);
	param_set_pt( h, _run_count					);
	param_set_pt( h, _time_interval_max			);
	param_set_pt( h, _b_real_time				);
	param_set_pt( h, _time_interval				);
	param_set_pt( h, _b_dataset_restart_trig_ui	);

	++h;
		param_set_pt( h, b_opencl_do			);
		param_set_pt( h, _b_verbose				);
		param_set_pt( h, _b_use_gl_out			);
		param_set_pt( h, _b_do_gol_finish		);
		param_set_pt( h, _b_do_gol_flush		);
		param_set_pt( h, _b_do_acquire_release	);
		param_set_pt( h, _b_wait_acquire		);
		param_set_pt( h, _maa_loop_nb			);
		param_set_pt( h, _b_wait_run			);
		param_set_pt( h, _b_wait_release		);
		param_set_pt( h, _b_do_cl_finish		);

	param_set_pt( h, _b_draw_quad_ui			);

	++h;
		++h;
			param_set_pt( h, _kernel_param.line			);
			param_set_pt( h, _kernel_param.color		);
			param_set_pt( h, _kernel_param.normal		);
			param_set_pt( h, _kernel_param.attrib		);

			param_set_pt( h, _kernel_param.nb_by_set	);
			param_set_pt( h, _kernel_param.float_by_set	);

		++h;
			param_set_pt_2( h, _mouse_offset			);
			param_set_pt_2( h, _mouse_scale				);
			param_set_pt( h, _kernel_param.mouse_pos.x	);	
			param_set_pt( h, _kernel_param.mouse_pos.y	);

		++h;
			for( uint32_t i=0; i<ARG_FLOAT_PARAM_NB; ++i )
				param_set_pt( h, _kernel_param.p[i]		);

		++h;
			for( uint32_t i=0; i<ARG_FLOAT4_PARAM_NB/4; ++i )
			{
				param_set_pt( h, _kernel_param.v[i].x	);
				param_set_pt( h, _kernel_param.v[i].y	);
				param_set_pt( h, _kernel_param.v[i].z	);
				param_set_pt( h, _kernel_param.v[i].w	);
			}

	++h;
		param_set_pt(	h,	_b_dataset_use_pointer		);
		param_set_pt_n(	h,	_dataset_param->v, DATASET_PARAM_NB-1 );

	err_param_init_pt( h );
}

bool	c_bdd_opencl_part::param_do_action( c_param * CONST par, CONST aaa::param::ACTION action )
{
	if( action == aaa::param::ACTION::PARAM_SIGN || action == aaa::param::ACTION::PARAM_OPEN )
	{
		switch( par->get_type() )
		{
		case TYPE_FILENAME:	//todo refine doing it inside the object itself
			switch( static_cast<aaa::file::TYPE_IO>( INT32(par->get_def()) ) )
			{
			case aaa::file::TYPE_IO_OPENCL:
				g_lua_master->trig_edit_file( get_script_filename() );
				return true;
			default:
				break;
			}
			break;
		}
	}
	return false;
}

bool	c_bdd_opencl_part::is_param_changed()
{
	if( is_diff_v3( _param_ui, _param ) )
	{
		cpy_v3( _param, _param_ui );
		return true;
	}
	return false;
}

//	unique opencl context lead to leak on _program in c_opencl obj (use gDEBugger)
//	multiple context lock gDEBugger 5.8
//	now in pref
//todo 	we should split c_opencl in 2 objs c_opencl_context and c_opencl_program
AAA_OPENCL::c_opencl* main_open_cl = nullptr;

void	c_bdd_opencl_part::c_init()
{
	main_open_cl = nullptr;
}

void	c_bdd_opencl_part::c_deinit()
{
	SAFE_DELETE( main_open_cl );
}

CONSTRUCTOR_CREATE( c_bdd_opencl_part )
#if AAA_OPENCL_USE()
	,_point( "point" )
	,_color( "color" )
	,_normal( "normal" )
	,_attrib( "attrib" )
	,_p_event_run(nullptr)
	,_vec_mem(nullptr)
#endif //AAA_OPENCL_USE
	,_b_init_done(false)
	,_b_use_gl_out(true)
	,_vbo(0)
	,_particles(nullptr)
	,_open_cl(nullptr)
	,_cl_dataset_float(nullptr)
	,_cl_particles_in(nullptr)
	,_cl_points_out(nullptr)
	,_dataset_nb(0)
	,_particle_nb_allocated(0)
	,_particle_nb(0)
	//,_block_size(256)
	,_b_reload_file_trig(false)
	,_b_program_compiled(false)
	,_b_program_submitted(false)
	,_b_kernel_loaded(false)
	//,_b_binding_cl_img(false)
	,_b_binding_cl_buf(false)
	,_b_define_buffer(false)
	,_b_texture_buffer_ok(false)
	,_b_binding_vbo_gl(false)
	,_b_ready_to_run(false)
	,_b_post_run_need_wait(false)
	,_b_post_run_need_release(false)
	//,_nb_v(0)
	,_nb_u(0)
	,_texture_nb_loaded(0)
	,_run_count(0)
	,_b_need_set_arg_particle(true)
	,_texture_nb_wanted(0)
	,_script_file_time( c_file::TIME_UNDEFINED )
{
	if( is_obj_first() )
		c_init();

	_dataset_param = (dataset_param*) MALLOC_ALIGNED( sizeof(dataset_param), 512 );
	for( UINT32 i = 0; i < TEXTURE_NB; ++i )
	{
		auto& td = _texture_data[i];
		td.bind_wanted	= -42;
		td.usage_loaded	= -42; 
		td.bind_loaded	= -43;	// mean unused for now
		td.gl			= nullptr;
		td.reuse_index	= -42;	// < 0 mean no reuse
	}
	param_init_with( param, PARAM_NB );
	//todo	this is done here to make sure we create opencl context before gl texture for example
	//		it should be refined using a master object
	init_low();
}

c_bdd_opencl_part::~c_bdd_opencl_part()
{
	deinit_low();
	FREE_ALIGNED_AND_NULL( _dataset_param );

	if( is_obj_first() )
		c_deinit();
}

UINT32	c_bdd_opencl_part::get_dataset_index()
{
	return WRAP_ID( _dataset_id, _dataset_nb );
}

#if AAA_OPENCL_USE()
AAA_ERR	c_bdd_opencl_part::alloc_particle()
{
	UINT32	nb_needed = _particle_nb_ui * _dataset_nb_ui;
//chanel	nb_needed = adapt_to_work_group_size( nb_needed, _open_cl->get_kernel_workgroup_size() );
	if( _particle_nb_allocated == nb_needed )
		return AAA_OK;
	//todo use full not full
	if( _b_particle_realloc_always || _particle_nb_allocated < nb_needed )
	{
		//if( !_open_cl->kernel_set_arg( ARG_PART_ID, (cl::Buffer*)nullptr, "clMemParticlesIn" ) )
		//{
		//	debug_break( "%s() can't set to NULL", __FUNCTION__ );
		//}
		SAFE_DELETE( _cl_particles_in );
		//todo we should unbind undefine buffer before

		DBG_PRINT_STRING( "%s()", __FUNCTION__ );
		_particles = (UINT8*) REALLOC_ALIGNED_SIGNATURE( _particles, nb_needed * ( _b_particle_full ? sizeof(cl_particle_full) : sizeof(cl_particle_compact) ), __FUNCTION__ );
		if( !_particles )
			return ERR_MEM_BASE;
		
		_particle_nb_allocated	= nb_needed;

		_b_define_buffer	= false;
		_b_binding_vbo_gl	= false;
		_b_restart_trig_ui	= true;
	}
	//this is not enough : work only for one dataset
	_particle_nb	= _particle_nb_ui;
	_dataset_nb		= _dataset_nb_ui;
	return AAA_OK;
}

FINLINE	void c_bdd_opencl_part::place( c_placer& placer, UINT32 nb, float* dst )
{
	placer.update();

	UINT32 stride = ( _b_particle_full ? sizeof(cl_particle_full) : sizeof(cl_particle_compact) ) / sizeof(float);
	if( placer.is_input_needed() )
	{
		float* src = &(((cl_particle_full*)_particles)->pos_b.x);
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

void	c_bdd_opencl_part::init_particle()
{
	if( _b_verbose )	DBG_PRINT_STRING( "%s()", __FUNCTION__ );

	REAL	size[3];
	c_model::cur->get_size_v3( size );

	UINT32 nb  = _particle_nb_allocated;

	cl_particle_full* p;
	//DBG_PRINT_STRING( "%s() with dt %f on dataset %d : start %d, nb %d.", __FUNCTION__, _kernel_param.dt, _dataset_id, 0, nb );
	_nb_u = _nb_u_ui;
	INT32 stride = _b_particle_full ? sizeof(cl_particle_full) : sizeof(cl_particle_compact);
	p = (cl_particle_full*)_particles;
	if( _b_grid_2d_ui )
	{
		INT32 nb_v = _particle_nb_used / _nb_u;
		REAL fu = OVER_ONE_AS_REAL( _nb_u ) ;
		REAL fv = OVER_ONE_AS_REAL( nb_v );
		for( INT32 iv = 0; iv < nb_v; ++iv )
		{
			REAL v  = ((iv+REAL(.5)) * fv - REAL(.5)) * size[0];
			for( INT32 iu = 0; iu < _nb_u; ++iu )
			{
				REAL u  = ((iu+REAL(.5)) * fu - REAL(.5)) * size[1];
				p->pos.set( u, v, REAL(0), REAL(0) );
				p = (cl_particle_full*) ( ((UINT8*)p) + stride );
			}
		}
		for( UINT32 i = nb - (nb_v * _nb_u); i > 0; --i )
		{
			p->pos.set( REAL(0),REAL(0),REAL(0), REAL(1) );	//todo better
			p = (cl_particle_full*) ( ((UINT8*)p) + stride );
		}
	}
	else
		place( _placer,			nb, &(p->pos.x) );

	p = (cl_particle_full*)_particles;
	place( _placer_b,		nb, &(p->pos_b.x)	);
	place( _placer_vel,		nb, &(p->vel.x)	); 
	if( _b_particle_full )
	{
		place( _placer_c,		nb, &(p->pos_c.x)	);
		// todo p->vel.set( 0, 0, 0, 0 );
		place( _placer_accel,	nb, &(p->accel.x)	);
	}

	for( UINT32 id=0; id<nb; ++id )
	{	//todo strategy should be refined, lua used and fourth coor treated
		p = (cl_particle_full*)(_particles + id * stride);
		//hack
		INT32 count = sta_rand.get_count();
		if( _b_particle_full )
		{
			p->mass		=	_b_mass_forced		? _mass_value	: sta_rand.get_fp32_01();
			p->radius	=	_b_radius_forced	? _radius_value	: sta_rand.get_fp32_01();
		}
		if( _b_dum_circle )
		{
			sta_rand.get_circle( p->dum1, p->dum2 );
			sta_rand.get_circle( p->dum3, p->dum4 );
		}
		else
		{
			p->dum1		=	_b_dum_forced[0]	? _dum_value[0]	: sta_rand.get_fp32_01();
			p->dum2		=	_b_dum_forced[1]	? _dum_value[1]	: sta_rand.get_fp32_01();	
			p->dum3		=	_b_dum_forced[2]	? _dum_value[2]	: sta_rand.get_fp32_01();	
			p->dum4		=	_b_dum_forced[3]	? _dum_value[3]	: sta_rand.get_fp32_01();	
		}
		if( _b_particle_full )
		{
			p->life				= _life_span + _life_span_offset * sta_rand.get_fp32_01();
			p->life_over_one	= OVER_ONE_AS_FP32( p->life );
		}
		//hack this avoid patterns
		if( ((sta_rand.get_count() - count) & 0x1) == 0 )
			sta_rand.get_fp32_01();
		
		//why this was here
		//_particles_pos[i*2] = p.pos;
		//_particles_pos[i*2+1] = p.pos;
	}

	//	_CL_MEM_USE_HOST_PTR is not ok for restart so we use CL_MEM_COPY_HOST_PTR instead
	//todo this should be solve or clarified
	if( _cl_particles_in )
	{
		_open_cl->kernel_set_arg_direct( ARG_PART_ID, 0, nullptr, "clMemParticlesIn Release" );
		SAFE_DELETE( _cl_particles_in );
		_b_need_set_arg_particle = true;
	}
	_cl_particles_in = _open_cl->buffer_create( CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, stride * nb, _particles );
	if( !_cl_particles_in )
	{
		err_print( "Open cl buffer creation failed on particles" );
		return;
	}
	_b_need_set_arg_particle = true;
	_b_restart_trig_ui = false;
}

//BINDING
void	c_bdd_opencl_part::bind_vbo_data()
{
	if( _b_verbose )	DBG_PRINT_STRING( "%s()", __FUNCTION__ );
	if( _b_use_gl_out )
	{
/*
		// now done in be_ready() 
		_point.bind_vbo_data();
		_color.bind_vbo_data();
		_normal.bind_vbo_data();
		_attrib.bind_vbo_data();
		GOL::unbind_buffer( GL_ARRAY_BUFFER );
*/
	}
	else if( !_b_binding_vbo_gl )
	{

		if( !_vbo )
			GOL::gen_vbo( _vbo );

		GOL::bind_set_unbind_buffer( GL_ARRAY_BUFFER, _vbo, _point.get_data_size(), _point.get_data(), GL_DYNAMIC_COPY );	//	was GL_DYNAMIC_DRAW
		_b_binding_vbo_gl = true;
		_b_define_buffer = false;
	}
}

//cl::Image2DGL*  bidon = nullptr;
//CHAR	bidon_data[256];
void	c_bdd_opencl_part::release_texture_by_id( INT32 i, bool b_delete )
{
	SAFE_DELETE( _vec_mem );	// if we don't that texture are not released, and glTexImage2D will fail
/*
	if( !bidon )
	{
		tex_2d_bind( 0 );
		make_texture_complete( 0 )
		bidon = _open_cl->image_2d_gl_create( CL_MEM_READ_ONLY, tex::get_name_2d( 0 ) );
		//bidon = _open_cl->buffer_create( CL_MEM_READ_ONLY, 256, bidon_data );
	}
	//if( bidon )
	//{
		_open_cl->kernel_set_arg( ARG_TEX_ID + i, bidon, "bidon" );
	//}
	*/
	//if( _src_texture_gl[i] )
	//tex_2d_bind( _texture_src[i] );

	auto& td = _texture_data[i];
	SAFE_DELETE( td.gl );
	if( td.reuse_index>=0 )
		td.reuse_index = -42;
	else
	{
		if( b_delete )
			tex2d.delete_user( td.bind_loaded, this );	//	here we need to specify the index
		else
			tex2d.remove_user( td.bind_loaded, this );	//	here we need to specify the index
		td.bind_loaded	= -43;
		td.usage_loaded	= -43;
	//	_texture_bind[i] = -42;
		td.reuse_index = -42;
	}
}

void	c_bdd_opencl_part::release_textures( bool b_delete )
{
	if( _b_verbose )
		DBG_PRINT_STRING( "%s()", __FUNCTION__ );
	if( !_b_use_gl_out )
		return;

	for( UINT32 i = 0; i < _texture_nb_loaded; ++i )
	{
		if( _texture_data[i].gl )
			release_texture_by_id( i, b_delete );
	}
	_b_texture_buffer_ok = false;
}

void	c_bdd_opencl_part::release_texture( c_obj_ui* obj, INT32 bind )
{
	if( _b_verbose )
		DBG_PRINT_STRING( "%s()", __FUNCTION__ );
	if( !_b_use_gl_out )
		return;

	if( bind >= 0 )
	{
		for( UINT32 i = 0; i < _texture_nb_loaded; ++i )
		{
			if( _texture_data[i].reuse_index<0 && _texture_data[i].bind_loaded == bind )
				release_texture_by_id( i, false );
		}
		if( !obj || obj != this )
			_b_texture_buffer_ok = false;
	}
	else
	{
		ERR_PRINT_STRING( "%s() called with an invalid bind %d", __FUNCTION__, bind );
		//release_textures( false );
	}
}


bool	c_bdd_opencl_part::check_textures()
{
	bool	b_ok = true;

	INT32	bind_to_restore	=	-42;
	INT32	nb = 0;
	for( INT32 i = 0; i < TEXTURE_NB; ++i )
	{
		INT32 index = i;
		if( i < 2 )
		{
			if( _b_texture_switch_auto_ui )
				_b_texture_switch_ui = ((_run_count & 1) == 1);
			if( _b_texture_switch_ui )
				index = 1 - index;
		}
		auto& tui = _texture_ui[i];

		if( tui.b_use )
		{
			INT32 bind = _texture_ui[index].bind;
			//check we don't use twice the same texture
			//	because we don't deal with the case yet and it crash the app
			INT32	index_found = -42;
			{
				for( INT32 j=0; j<nb; ++j )
				{
					if( _texture_data[j].bind_wanted==bind )
					{
						index_found = j;
						break;
					}
				}
				//if( index_found )
				//{
				//	err_print( "all used texture must be distinct for now, %d used twice", bind );
				//	b_ok = false;
				//	nb = 0;
				//	break;
				//}
			}
		
			auto& td = _texture_data[nb];
			if( index_found>=0 )
			{
				td.reuse_index = index_found;
				++nb;
			}
			else
			{
				td.reuse_index = -42;
				tex_2d_bind( bind );
				INT32 x,y;
				tex2d.get_size_cur( x, y );
				if( x==0 || y==0 )
				{
					err_print( "%s() No Texture %d", __FUNCTION__, bind );
					b_ok = false;
					nb = 0;
					break;
				}
				else
				{
					INT32 usage =  tui.usage;
					auto& td = _texture_data[nb];
					td.bind_wanted	= bind;
					td.usage_wanted	= usage;
					//	if texture or usage have changed we need to redefine also the args
					if( td.bind_loaded != bind || td.usage_loaded != usage )	
						_b_texture_buffer_ok = false;
					++nb;
				}
			}
		}
	}
	//	if nb changed we need to redefine also the args
	if( _texture_nb_loaded != nb )
		_b_texture_buffer_ok = false;
	_texture_nb_wanted = nb;

	if( bind_to_restore != -42 )
		tex_2d_bind( bind_to_restore );

	return b_ok;
}

void	c_bdd_opencl_part::define_buffer_texture()
{
	if( _b_verbose )
		DBG_PRINT_STRING( "%s()", __FUNCTION__ );
	if( !_b_use_gl_out )
		return;

	SAFE_DELETE( _vec_mem );	// if we don't that texture are not released (_texture_gl[])
	_b_texture_buffer_ok = true;

	INT32	bind_to_restore	= -42;
	INT32	nb				= 0;
	for( UINT32 i = 0; i < _texture_nb_wanted; ++i )
	{
		if( bind_to_restore == -42 )
			bind_to_restore = g_bind_img_2d->get_cur_index();

		auto& td = _texture_data[i];
		if( td.reuse_index>=0 )
			++nb;
		else
		{
			INT32 bind =  td.bind_wanted;
			INT32 usage =  td.usage_wanted;
			SAFE_DELETE( td.gl );
			// this strategy don't work when several ref to the same bind (it even crash)
			tex2d.ask_release_from_users( bind, this );

			//make sure it is loaded on gpu guess
		
			tex_2d_bind( bind );	
//			GOL::make_tex_complete_2d( bind );
			if( usage >= 1 )
				GOL::set_tex_2d_minification( GL_NEAREST );

			cl::Image2DGL* img_2dgl = _open_cl->image_2d_gl_create( cl_texture_usage[usage], tex2d.get_name_gl( bind ) );

			if( img_2dgl )
			{
				tex2d.add_user( this );	//	the index/bind is done implicitly by tex_2d_bind
				td.gl			= img_2dgl;
				td.bind_loaded	= bind;
				td.usage_loaded	= usage;
				//if ( !_open_cl->kernel_set_arg( ARG_TEX_ID + i, _src_texture_gl[ i ], "src_texture_gl") )
				//	err_print( "Open cl binding failed on buffer src_texture %d.", i );
				++nb;
			}
			else
			{
				_b_texture_buffer_ok = false;
				//no reason to do anything tex::mark_not_using( this );
				err_print( "%s() binding failed on object texture %d at bind %d", __FUNCTION__, i, bind );
				nb = 0;
				break;
			}
		}
	}
	_texture_nb_loaded = nb;
//	_texture_nb_wanted = 0;

	if( bind_to_restore != -42 )
		tex_2d_bind( bind_to_restore );
}

#define GOTO_EXIT( str )				{	buf_error = str;  goto exit;	}
#define ON_NULL_GOTO_EXIT( buf, str )	if( !buf )	GOTO_EXIT( str )


void	c_bdd_opencl_part::define_buffer()
{
	if( _b_verbose )	DBG_PRINT_STRING( "%s()", __FUNCTION__ );

	C_PCHAR buf_error = nullptr;
	//	define buffer

	if( _b_use_gl_out )
	{
/*		
		//done now in be_ready()
		if( ERR( _point.create_cl_buffer( _open_cl ) ) )	GOTO_EXIT( "point" );
		if( ERR( _color.create_cl_buffer( _open_cl ) ) )	GOTO_EXIT( "color" );
		if( ERR( _normal.create_cl_buffer( _open_cl ) ) )	GOTO_EXIT( "normal" );
		if( ERR( _attrib.create_cl_buffer( _open_cl ) ) )	GOTO_EXIT( "attrib" );
*/
	}
	else if( !_b_define_buffer )
	{
		SAFE_DELETE( _cl_points_out );
		_cl_points_out = _open_cl->buffer_create( CL_MEM_READ_WRITE, _point.get_data_size(), _point.get_data() );
		ON_NULL_GOTO_EXIT( _cl_points_out, "cl_particle" );
	}

	if( _b_dataset_use_pointer )
	{
		SAFE_DELETE( _cl_dataset_float );
		_cl_dataset_float = _open_cl->buffer_create( CL_MEM_READ_ONLY, sizeof(_dataset_param->v), nullptr );
		if( !_cl_dataset_float )
		{
			err_print( "Open cl buffer creation failed on dataset" );
			goto exit;
		}
	}
	_b_define_buffer = true;
	return;
exit:
	err_print( "Open cl binding failed on buffer %s.", buf_error ? buf_error : "????" );
}

/*
void	c_bdd_opencl_part::set_cl_arg_img()
{
	if( _b_verbose )	{	DBG_PRINT_STRING( "%s()", __FUNCTION__ );	}

	CHAR* buf_error = nullptr;

	//	submit the args
	if ( _b_use_gl_out )
	{
		for ( UINT32 i = 0; i < TEXTURE_SRC_NB; ++i )
		{
			if ( _b_texture_src[i] && _src_texture_gl[i] )
			{	//todo the first argument should be even more dynamic
				if ( !_open_cl->kernel_set_arg( ARG_TEX_ID + i, _src_texture_gl[ i ], "_src_texture_gl[i]") )	{ buf_error = "texture";  goto exit;	}
			}
		}
	}
	_b_binding_cl_img = true;
	return;
exit:
	err_print( "Open cl image binding failed on %s.", buf_error ? buf_error : "????" );
}
*/

void	c_bdd_opencl_part::set_cl_arg_buf()
{
	if( _b_verbose )
		DBG_PRINT_STRING( "%s()", __FUNCTION__ );

	C_PCHAR buf_error = nullptr;
	
	//	submit the args
	if( _b_use_gl_out )
	{
/*
		//done now in be_ready()
		if( !_open_cl->kernel_set_arg( 1, _point.get_cl_mem(),	 _point.get_name_str()	) )	{ buf_error =  _point.get_name_str();	goto exit;	}
		if( !_open_cl->kernel_set_arg( 2, _color.get_cl_mem(),	 _color.get_name_str()	) )	{ buf_error =  _color.get_name_str();	goto exit;	} 
		if( !_open_cl->kernel_set_arg( 3, _normal.get_cl_mem(),	 _normal.get_name_str()	) )	{ buf_error =  _normal.get_name_str();	goto exit;	}
		if( !_open_cl->kernel_set_arg( 4, _attrib.get_cl_mem(),	 _attrib.get_name_str()	) )	{ buf_error =  _attrib.get_name_str();	goto exit;	}
*/
	}
	else
	{
		if( !_open_cl->kernel_set_arg( ARG_POINT_ID, _cl_points_out, "clMemParticlesOut" ) )
		{
			buf_error = "cl_particle position";
			goto exit;
		}
	}
	_b_binding_cl_buf = true;
	return;
exit:
	err_print( "Open cl buffer binding failed on %s.", buf_error ? buf_error : "????" );
}
#endif //AAA_OPENCL_USE

void	c_bdd_opencl_part::init_low()
{
#if AAA_OPENCL_USE()
	if( !IS_BDD_OPENCL_PART_DO() )
		return;
	//hack to avoid a second one )
	// init device
	if( !_open_cl )
	{
		if( AAA_OPENCL::c_opencl::gb_context_by_program_ui )
		{
			AAA_OPENCL::c_opencl*	open_cl = new AAA_OPENCL::c_opencl();
			if( !open_cl )
			{
				err_print( "Can't create a c_opencl instance" );
				return;
			}
			open_cl->setup( _platform, _device_type, _device, _b_use_gl_out );
			// check if OpenGL is supported!!!
			if( !open_cl->is_valid() )
			{
				err_print( "Did not create a valid c_opencl instance" );
				delete open_cl;
				return;
			}
			_open_cl = open_cl;
		}
		else
		{
			if( !main_open_cl )
			{
				main_open_cl = new AAA_OPENCL::c_opencl();
				if( !main_open_cl )
					return;
				main_open_cl->setup( _platform, _device_type, _device, _b_use_gl_out );
			}
			// check if OpenGL is supported!!!
			if( !main_open_cl->is_valid() )
			{
				err_print( "No main valid c_opencl instance" );
				return;
			}
			_open_cl = main_open_cl;
		}
	}
	_point.set_cl_context( _open_cl );
	_color.set_cl_context( _open_cl );
	_normal.set_cl_context( _open_cl );
	_attrib.set_cl_context( _open_cl );


	for( INT32 i = 0; i < DATASET_FLOAT_NB; ++i )
	{
		_dataset_param->v[i] = 0.;
	}

	//_b_use_gl_out = _b_use_gl_out && _open_cl->gl_interop_can();
	_device_name = _open_cl->get_device_name();
	_device_version = _open_cl->get_device_version();
	_device_vendor = _open_cl->get_device_vendor();
	_platform_name = _open_cl->get_platform_name();
	_device_driver_version = _open_cl->get_device_driver_version();

	if( !_open_cl->is_valid() )
		return;

	_point.set_kernel_arg_index( ARG_POINT_ID );
	_color.set_kernel_arg_index( ARG_COLOR_ID );
	_normal.set_kernel_arg_index( ARG_NORMAL_ID );
	_attrib.set_kernel_arg_index( ARG_ATTRIB_ID );
#endif //AAA_OPENCL_USE
	_b_init_done = true;
}

void	c_bdd_opencl_part::deinit_low()
{
	if( !_b_init_done )
		return;

#if AAA_OPENCL_USE()
	SAFE_DELETE( _p_event_run );

	release_textures( true );

	SAFE_DELETE( _cl_dataset_float );
	SAFE_DELETE( _cl_points_out );

	SAFE_DELETE( _cl_particles_in );
	FREE_ALIGNED_AND_NULL( _particles );

	//FREE_ALIGNED_AND_NULL( _particles_pos );
	SAFE_DELETE( _vec_mem );
#endif //AAA_OPENCL_USE

	GOL::delete_vbo( _vbo );

#if AAA_OPENCL_USE()
	if( _open_cl != main_open_cl )
	{
//todo we use a unique object so we can't just close it
//			we should delete the kernel I think
//			_open_cl->close();
		obj_delete( _open_cl );
	}
#endif //AAA_OPENCL_USE
	_particle_nb = 0;
	_dataset_nb = 0;
	_b_program_submitted = false;
	_b_program_compiled = false;
	_b_init_done = false;
}

#if AAA_OPENCL_USE()
bool	c_bdd_opencl_part::load_program()
{
	//if( _b_loaded )
	//{
	//	_open_cl->close();
	//	_b_loaded = false;
	//}	
	// load and compile OpenCL program
	_b_program_submitted	= true;
	_b_program_compiled		= false;
	_b_kernel_loaded		= false;
	_b_need_set_arg_particle = true;
	o_str o_include( _b_particle_full ? o_include_particle_full : o_include_particle );
	o_include.add( o_include_param );
	if( _open_cl->program_build_from_file( _script_filename, o_include ) )
	{
		_b_program_compiled = true;
		if( _open_cl->kernel_load( &_kernel_name ) )
		{
			_b_kernel_loaded = true;
			/* Check group size against group size returned by kernel */
			for( INT32 i=0; i<3; ++i )
			{
				if( ( _block_size[i] ) > _open_cl->get_kernel_workgroup_size() )
				{
					ERR_PRINT_STRING( "%s() Max Group Size supported on the kernel : %d", __FUNCTION__, _open_cl->get_kernel_workgroup_size() );
					_block_size[i] = _open_cl->get_kernel_workgroup_size();
				}
			}
		}
	}
	_b_binding_cl_buf = false;
	//_b_binding_cl_img = false;
	return _b_program_compiled;
}

static	CHAR	texture_arg_name[] = "img_";

void	c_bdd_opencl_part::update_kernels( INT32 dataset_id )
{
	if( !_cl_particles_in )
		return;
	if( !_b_use_gl_out && !_cl_points_out )
		return;

	SPY_PUSH_RANGE( "bdd_opencl_part::update_kernels", spy::UPDATE );

		if( _b_need_set_arg_particle )
		{
			if( !_open_cl->kernel_set_arg( ARG_PART_ID, _cl_particles_in, "clMemParticlesIn" ) )	
			{
				err_print( "Open cl arg binding failed on particles" );
				goto exit;
			}
			_b_need_set_arg_particle = false;
		}

		_b_post_run_need_wait		= false;
		_b_post_run_need_release	= false;

		FP32 x,y;
		c_mouse::get_cur()->get_xy_render( x,y );
		_kernel_param.mouse_pos.x = x;
		_kernel_param.mouse_pos.y = y;
		_kernel_param.mouse_pos.x = (_kernel_param.mouse_pos.x + _mouse_offset[0]) * _mouse_scale[0];
		_kernel_param.mouse_pos.y = (_kernel_param.mouse_pos.y + _mouse_offset[1]) * _mouse_scale[1];

		for( INT32 i=0; i<3; ++i )
		{
			CLAMP_REF( _block_size[i], (UINT32)1, _open_cl->get_kernel_workgroup_size() );
		}
		UINT32 elt_offset;
		UINT32 elt_nb;
		if( dataset_id > 0 )
		{
			elt_offset	=  ( dataset_id -1 ) * _particle_nb;
			elt_nb		= _particle_nb_used;
		}
		else
		{
			elt_offset	= 0;
			elt_nb		= _particle_nb * _dataset_nb;
		}
		//chanel		elt_nb = adapt_to_work_group_size( elt_nb, _block_size_x );
		_kernel_param.i_offset = elt_offset;

		if( _b_do_gol_finish )
			GOL::finish();	//	this the right way for portability
		if( _b_do_gol_flush )
			GOL::flush();	//	can be faster than finish but a hack

		cl_int	status;

		{
			INT32	arg_slot = ARG_DATASET_ID;
			if( !_open_cl->kernel_set_arg( ARG_KERNEL_ID,	&_kernel_param,		"aaa"		) )
			{
				err_print( "OpenCL arg binding failed on _kernel_param aaa" );
				goto exit;
			}

			if( _kernel_param.float_by_set != 0 )
			{
				if( _b_dataset_use_pointer )
				{
					cl::Event events;
					if( !_open_cl->buffer_write( _cl_dataset_float, true, 0, sizeof(_dataset_param->v), _dataset_param->v, 0, &events ) )
						goto exit;
					SPY_PUSH_RANGE( "wait buffer_write", spy::OPENCL );
						status = events.wait();
					SPY_POP_RANGE();
					if ( AAA_OPENCL::check_for_error( __FUNCTION__, "Event::wait() failed. (event[1])", status) )
						goto exit;
					if( !_open_cl->kernel_set_arg( arg_slot++, _cl_dataset_float, "pds" ) )	
					{
						err_print( "OpenCL arg binding failed on dataset_float" );
						goto exit;
					}
				}
				else
				{
					if( !_open_cl->kernel_set_arg( arg_slot++,	_dataset_param,	"ds"	) )
					{
						err_print( "OpenCL arg binding failed on dataset_param, try to float_by_set to 0" );
						goto exit;
					}
				}
			}

			//	we use delete because the cl:vector structure and the reference system don't retain/release correctly 
			SAFE_DELETE( _vec_mem );
			_vec_mem = new VECTOR_CLASS<cl::Memory>;
			_point.push_on( _vec_mem );
			_color.push_on( _vec_mem );
			_normal.push_on( _vec_mem );
			_attrib.push_on( _vec_mem );
			for( UINT32 i = 0; i < _texture_nb_loaded; ++i )
			{
				texture_arg_name[3] = '1' + i;
				INT32 reuse_index = _texture_data[i].reuse_index;
				INT32 index = (reuse_index>=0) ? reuse_index : i;
				auto img2d_gl = _texture_data[index].gl;
				if( _open_cl->kernel_set_arg( arg_slot++, img2d_gl, texture_arg_name ) )
				{
					if( reuse_index<0 )
						_vec_mem->push_back( *img2d_gl );
				}
				else
				{
					err_print( "Open cl binding failed on buffer src_texture %d.", i );
					goto exit;
				}
			}
		}

		if( _b_do_acquire_release && _b_use_gl_out )
		{
			cl::Event	event;
			_b_post_run_need_release = true;
			if( !_open_cl->gl_objects_acquire( _vec_mem, 0, &event ) )
				goto exit;
			if( _b_wait_acquire )
			{
				// Wait for the acquisition to be done
				SPY_PUSH_RANGE( "wait_acquire", spy::OPENCL );
					status = event.wait();
				SPY_POP_RANGE();
				if ( AAA_OPENCL::check_for_error( __FUNCTION__, "Event::wait() failed after gl_objects_acquire()", status ) )
					goto release_and_exit;
			}
		}

		if( 1 )
		{
			cl::NDRange	globalThreads(	elt_nb			);
			cl::NDRange	localThreads(	_block_size[0], _block_size[1], _block_size[2] );
			cl::NDRange	offset(			elt_offset		);

			for( UINT32 i=_maa_loop_nb; i>0; --i )
			{
				SAFE_DELETE( _p_event_run );
				_p_event_run = new cl::Event;
				if( !_open_cl->kernel_run( offset, globalThreads, localThreads, nullptr, _p_event_run ) )
				{
					SPY_PUSH_RANGE( "wait_run failed", spy::OPENCL );
						status = _p_event_run->wait();
					SPY_POP_RANGE();
					goto release_and_exit;
				}
				++_run_count;
				_b_post_run_need_wait = true;
				if( _b_wait_run )
				{
					_b_post_run_need_wait = false;
					SPY_PUSH_RANGE( "wait_run", spy::OPENCL );
						status = _p_event_run->wait();
					SPY_POP_RANGE();
					if( AAA_OPENCL::check_for_error( __FUNCTION__, "Event::wait() failed after kernel_run().", status ) )
						goto release_and_exit;
				}
			}
		}

release_and_exit:
		update_kernels_post();
exit:
	SPY_POP_RANGE();
	return;
}

void	c_bdd_opencl_part::update_kernels_post()
{
	SPY_PUSH_RANGE( "bdd_opencl_part::update_kernels_post", spy::UPDATE );

		if( _b_post_run_need_wait )
		{
			_b_post_run_need_wait = false;
			SPY_PUSH_RANGE( "wait_run_post", spy::OPENCL );
				cl_int	status = _p_event_run->wait();
			SPY_POP_RANGE();
			AAA_OPENCL::check_for_error( __FUNCTION__, "Event::wait() failed after kernel_run().", status );
		}	
	//release_and_exit:
		if( _b_post_run_need_release )
		{
			bool		b_ok;
			cl::Event	event;
			_b_post_run_need_release = false;
			if( _b_use_gl_out )
				b_ok = _open_cl->gl_objects_release( _vec_mem, 0, &event );
			else
				b_ok = _open_cl->buffer_read( _cl_points_out, true, 0, _particle_nb_used * 2 * sizeof(float4), _point.get_data(), 0, &event );

			if( b_ok && _b_wait_release )
			{
				// Wait for the operation to be done
				SPY_PUSH_RANGE( "wait_release", spy::OPENCL );
					cl_int	status = event.wait();
				SPY_POP_RANGE();
				AAA_OPENCL::check_for_error( __FUNCTION__, "Event::wait() failed.", status );
				//	if ( AAA_OPENCL::check_for_error( __FUNCTION__, "Event::wait() failed.", status) )
				//		return;
			}
		}

	SPY_POP_RANGE();
	if( _b_do_cl_finish )
		_open_cl->finish();
	return;
}

void	c_bdd_opencl_part::prepare_to_run()
{
	_b_ready_to_run = false;

	if( _script_filename_ui.is_empty() )
		return;
	if( _script_filename.is_empty() )	//todo we can't change _script_filename_ui on the fly, ths test will work only once
	{
		//todo doesn't work with absolute path!!!
		if( !c_obj_ui::make_script_filename( _script_filename, _script_filename_ui, "cl" ) )
		{
			debug_break( "%s() can't build script filename !", __FUNCTION__ );
			return;
		}
	}

	if( !is_active() )						return;

	SPY_PUSH_RANGE( "bdd_opencl_part::prepare_to_run", spy::UPDATE );

		if( _delta_t.update() )
		{
			DBG_PRINT_STRING( "opencl_part restart" );
			_b_restart_trig_ui = true;
		}

		if( !_b_init_done )
		{
			init_low();
			if( !_b_init_done )
				goto exit;
		}
		if( _b_particle_full != _b_particle_full_ui )
		{
			_b_reload_file_trig = true;
			_b_particle_full	= _b_particle_full_ui;
		}

		if( _b_reload_file_trig || c_file::is_time_changed( _script_filename, _script_file_time ) || !_b_program_submitted )
		{
			_b_reload_file_trig = false;
			//	reset also
			release_textures( false );
			if( !check_textures() )	
				goto exit;
			_point.destroy_cl_buffer();
			_color.destroy_cl_buffer();
			_normal.destroy_cl_buffer();
			_attrib.destroy_cl_buffer();
			if( load_program() )
				_script_file_time =  c_file::get_mdate( _script_filename );
		}
		else
		{
			if( !check_textures() )	
				goto exit;
		}
		//todo we don't deal with just a change of kernel name
		if( !_b_kernel_loaded )	
			goto exit;

		if( !_open_cl->kernel_bind( &_kernel_name ) )
			goto exit;

		if( _particle_nb_ui < _particle_nb_used )
			_particle_nb_ui = _particle_nb_used;
		//todo _cl_particles_in and _cl_particles_out have to be invalidated and the kernel stopped
		if( ERR( alloc_particle() ) )					goto exit;
		//todo eventually deal with the double size of line being always allocated
		{
			INT32	nb = _particle_nb_allocated * 2;
			if(	ERR( _point .alloc_float4( nb ) ) )
				goto exit;
			if(	ERR( _color .alloc_float4( _kernel_param.color  != 0. ? nb : 0 ) ) )
				goto exit;
			if(	ERR( _normal.alloc_float4( _kernel_param.normal != 0. ? nb : 0 ) ) )
				goto exit;
			if(	ERR( _attrib.alloc_float4( _kernel_param.attrib != 0. ? nb : 0 ) ) )
				goto exit;
	//	if(	ERR( _color .alloc_float4( nb ) ) )	goto exit;
	//	if(	ERR( _normal.alloc_float4( nb ) ) )	goto exit;
	//	if(	ERR( _attrib.alloc_float4( nb ) ) )	goto exit;
		}
		bind_vbo_data();

		if( ERR(_point.be_ready()) )
			goto exit;
		if( ERR(_color.be_ready()) )
			goto exit;
		if( ERR(_normal.be_ready()) )
			goto exit;
		if( ERR(_attrib.be_ready()) )
			goto exit;

		//_b_define_buffer_texture = false;
		if( !_b_texture_buffer_ok )
			define_buffer_texture();
		if( !_b_texture_buffer_ok )
			goto exit;

		define_buffer();
		//todo regroup in a fn
		if( _b_real_time )
			_kernel_param.dt = MIN( REAL(_delta_t.get_dt()), _time_interval_max );
		else
			_kernel_param.dt = _time_interval;

		if( _nb_u_ui != _nb_u )
			_b_restart_trig_ui = true;
		if( _b_restart_trig_ui )
		{
			init_particle();
			_run_count = 0;
		}

		if( _b_dataset_restart_trig_ui )
		{
			_kernel_param.dt = -_kernel_param.dt;
			_b_dataset_restart_trig_ui = false;
		}
		//printf( "%d, %f", _dataset_id, _kernel_param.dt );

	//	if( !_b_binding_cl_img )
	//		set_cl_arg_img(); }
	//	if( !_b_binding_cl_img )
	//		goto exit;

		if( !_b_binding_cl_buf )
			set_cl_arg_buf();
		if( !_b_binding_cl_buf )
			goto exit;

		_b_ready_to_run = true;
exit:
	SPY_POP_RANGE();
	return;
}
#endif //AAA_OPENCL_USE

void	c_bdd_opencl_part::update_dataset( UINT32 dataset_id )
{
#if AAA_OPENCL_USE()
	if( IS_BDD_OPENCL_PART_DO() )
	{
		prepare_to_run();
		if( _b_ready_to_run )
			update_kernels( dataset_id );
	}
#endif //AAA_OPENCL_USE
}

void	c_bdd_opencl_part::update()
{
#if AAA_OPENCL_USE()
	if( IS_BDD_OPENCL_PART_DO() )
		update_dataset( _dataset_id );
#endif
}


//todoq use c_vao
void	c_bdd_opencl_part::draw_dataset( UINT32 dataset_id )
{
#if AAA_OPENCL_USE()
	if( !IS_BDD_OPENCL_PART_DO() )
		return;	

//	update_kernels_post();
	if( !_b_ready_to_run )
		return;

	SPY_PUSH_RANGE( "bdd_opencl_part::draw_dataset", spy::DRAW );

		INT32 elt_nb = _particle_nb_used; 
		bool b_draw_prim = _b_grid_2d_ui && _b_draw_quad_ui;

		if( b_draw_prim )
		{
			INT32 nb_v = elt_nb / _nb_u;
			_guf_index.set_data( nullptr, nullptr, nullptr );
			_guf_index.alloc_for_strip( _nb_u, nb_v, __FUNCTION__ );
		}

		GLint	loc = -1;
		if( _kernel_param.line != 0. )	{ elt_nb *=  2; }

		GOL::unbind_vao_secu();

		if( _b_use_gl_out )
		{
			if( _kernel_param.color != 0. )
			{
				_color.bind_vbo();
				GOL::set_pointer_color4f( 0, 0 );
				//was glColorPointer( 4, GL_FLOAT, 0, 0 );

			}
			if( _kernel_param.normal != 0. )
			{
				_normal.bind_vbo();
				GOL::set_pointer_normal3f( 0, sizeof(cl_float)*4 );
				//was glNormalPointer( GL_FLOAT, sizeof(cl_float)*4, 0 );
			}
			if( _kernel_param.attrib != 0. && c_shading::get_cur() )
			{
				loc = c_shading::get_cur()->get_render_attribute_location( "aaa_attrib" );
				if ( loc > -1 )
				{
					_attrib.bind_vbo();
					GOL::set_vertex_attrib_pointer( loc, 4, GL_FLOAT );
				}
			}
			_point.bind_vbo();
		}
		else
		{
			_color.bind_vbo();
			GOL::set_buffer_data_sub( GL_ARRAY_BUFFER, 0, sizeof(float4) * elt_nb, _color.get_data() );

			_normal.bind_vbo();
			GOL::set_buffer_data_sub( GL_ARRAY_BUFFER, 0, sizeof(float4) * elt_nb, _normal.get_data() );

			GOL::bind_buffer( GL_ARRAY_BUFFER, _vbo );
			GOL::set_buffer_data_sub( GL_ARRAY_BUFFER, 0, sizeof(float4) * elt_nb, _point.get_data() );
		}

		if( _kernel_param.color != 0. )		GOL::enable_client_state_color();
		else								GOL::disable_client_state_color();

		if( _kernel_param.normal != 0. )	GOL::enable_client_state_normal();
		else								GOL::disable_client_state_normal();

		if( loc > -1 )						GOL::enable_vertex_attrib_array( loc );

		GOL::disable_client_state_texcoor();

		GOL::set_pointer_vertex4f( (CP_CVOID)0 );	//	if we want to offset here : _offset_maa*sizeof(GL_FLOAT)*4 );
		GOL::unbind_buffer( GL_ARRAY_BUFFER );

		//GOL::enable_client_state_vertex();

		if( b_draw_prim )
			_guf_index.draw_as_strip( c_render::get_cur()->get_draw_primitive() );
		else
			GOL::draw_arrays( (_kernel_param.line == 0.) ? GL_POINTS : GL_LINES, elt_nb, ( dataset_id - 1 ) * _particle_nb );

		if( loc > -1 )						GOL::disable_vertex_attrib_array( loc );
		if( _kernel_param.normal != 0. )	GOL::disable_client_state_normal();
		if( _kernel_param.color != 0. )		GOL::disable_client_state_color();

	//	GOL::disable_client_state_vertex();
	SPY_POP_RANGE();
#endif //AAA_OPENCL_USE
}


void	c_bdd_opencl_part::draw()
{
#if AAA_OPENCL_USE()
	if( IS_BDD_OPENCL_PART_DO() )
		draw_dataset( _dataset_id );
#endif //AAA_OPENCL_USE
}

void	c_bdd_opencl_part::restart()
{
	_b_restart_trig_ui = true;
}
