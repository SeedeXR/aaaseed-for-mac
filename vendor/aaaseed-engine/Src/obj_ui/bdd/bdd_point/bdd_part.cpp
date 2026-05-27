#include "bdd_part.h"

#include "image/bind_img_2d.h"
#include "draw/tex_anim.h"
#include "draw/render.h"
#include "draw/color.h"
#include "draw/mat.h"

#include "time/aaa_time.h"

#include "math/noisturb.h"
#include "math/rand.h"

#include "obj_ui/bdd/bdd_point/bdd_point.h"
#include "obj_ui/deformer/def_node.h"

#include <algorithm>
#include "math/billboard.h"

#include "gol/gol.h"
#include "gol/gol_draw.h"
#include "gol/gol_list.h"

#include "infrastructure/compute_parallel.h"
#if AAA_VSTOOL() >= 141
#	include <execution>
#endif

enum RENDER_TYPE : INT32
{
	RENDER_LINE = 0,
	RENDER_POINT,
	RENDER_QUAD,
	RENDER_NB_MAX,
};

static	C_PCHAR_C	str_render_type[RENDER_NB_MAX] =
{
	"Line",
	"Point",
	"Quad",
};

enum SORT_TYPE : INT32
{
	SORT_NO = 0,
	SORT_Z,
	SORT_Z_REVERSE,
	SORT_DIST,
	SORT_DIST_REVERSE,
	SORT_NB_MAX
};

static	C_PCHAR_C	str_render_sort[SORT_NB_MAX] =
{
	"No",
	"Depth",
	"Depth_front_to_back",
	"Distance",
	"Distance_front_to_back",
};

FACTORY_CREATE_PROP_V1( c_bdd_particle, bdd_particle, Particle, part, sub_menu="Point"; );

c_bdd_particle*	c_bdd_particle::cur = nullptr;

namespace	n_bdd_particle
{
//ttk this should not be : alloc dyn of jitter vector
	CONSTEXPR INT32	JITTER_VEC_NB_MAX				= (1024*32);
	CONSTEXPR INT32	EMISSION_IMAGE_PARAM_NB_MAX		= 7;

	CONSTEXPR INT32	EMISSION_POS_PARAM_NB	 =	10
													+	c_placer::PARAM_ALL_NB
													+	EMISSION_IMAGE_PARAM_NB_MAX
													+	1;	//	group EMISSION_IMAGE_PARAM_NB_MAX

	CONSTEXPR INT32	BASE_PARAM_NB				= c_bdd::GEO_PARAM_NB + 2;
	CONSTEXPR INT32	LIFE_PARAM_NB				= 14;
	CONSTEXPR INT32	LIVING_BOX_PARAM_NB			= 9;
	CONSTEXPR INT32	EMISSION_SPEED_PARAM_NB		= 18;
	CONSTEXPR INT32	MOTION_PARAM_NB				= 14;
	CONSTEXPR INT32	TURBULENCE_PARAM_NB			= 13;
	CONSTEXPR INT32	RENDER_COLOR_PARAM_NB		= 10;
	CONSTEXPR INT32	VISIBILITY_MAP_PARAM_NB		= 7;
	CONSTEXPR INT32	SORT_PARAM_NB				= 8;
	CONSTEXPR INT32	RENDER_PARAM_NB				= 25;
	CONSTEXPR INT32	CLOSE_PARAM_NB				= 3;
	CONSTEXPR INT32	MOCAP_PARAM_NB				= 0;
	CONSTEXPR INT32	NET_PARAM_NB				= 4;
	CONSTEXPR INT32	MISC_PARAM_NB				= 2;
	CONSTEXPR INT32	GROUP_PARAM_NB				= 14;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	LIFE_PARAM_NB
									+	LIVING_BOX_PARAM_NB
									+	EMISSION_SPEED_PARAM_NB
									+	EMISSION_POS_PARAM_NB
									+	MOTION_PARAM_NB
									+	TURBULENCE_PARAM_NB
									+	RENDER_COLOR_PARAM_NB
									+	VISIBILITY_MAP_PARAM_NB
									+	SORT_PARAM_NB
									+	RENDER_PARAM_NB
									+	CLOSE_PARAM_NB
									+	MOCAP_PARAM_NB
									+	NET_PARAM_NB
									+	MISC_PARAM_NB
									+	GROUP_PARAM_NB;				
	CONST	UINT32 PART_NB_MAX = 1024*1024*4;	//todo find how to set the limit higher without the debug version crashing when too big

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS

		PARAM_DEF_SYMBO_PSTR_ZERO(	render_type,				str_render_type )
		PARAM_DEF_REAL_INF(			render_min_size,			1, 0 )
		PARAM_DEF_GROUP_CLOSED( Life and Death, LIFE_PARAM_NB		)
			PARAM_DEF_BOOL_OFF(			restart_trig				)
			PARAM_DEF_INT32(			nb_allocated,				1024*8, 512,	0, PART_NB_MAX	)
			PARAM_DEF_INT32(			nb_max,						500,	128,	0, PART_NB_MAX	)
			PARAM_DEF_INT32(			nb_min,						1.,		0.,		0, PART_NB_MAX	)
			PARAM_DEF_REAL_POS(			nb_by_sec,					500,	128		)
			PARAM_DEF_REAL_POS_ZERO(	nb_to_create_trig			)
			PARAM_DEF_INT32_LOCKED(		nb_living_now				)
			PARAM_DEF_BOOL_OFF(			flow_continuous				)
			PARAM_DEF_BOOL_OFF(			regular						)

			PARAM_DEF_REAL_POS(			life_time,					.1, 1			)
			PARAM_DEF_REAL(				life_time_jitter,			.1, 0,			0, .99	)
			PARAM_DEF_GAIN(				life_time_jitter_gain		)
			PARAM_DEF_BIAS(				life_time_jitter_bias		)
			PARAM_DEF_SYMBO_PSTR_ZERO(	id_generation,				c_poid::id_generator_str	)

		PARAM_DEF_GROUP_CLOSED( Emission Position, EMISSION_POS_PARAM_NB )
			PARAM_DEF_REF(				emission_bdd				)			
			PARAM_DEF_BOOL_OFF(			emission_use_previous		)
			PARAM_DEF_POINT_XYZ(		emission_origin				)

			PARAM_DEF_AXE_Y( 			emission_axe				)

			PARAM_DEF_SCALE_UVA(		emission_box_size			)
			PARAM_DEF_REAL_ZERO(		emission_box_size_factor	)

			PARAM_PLACER(				emission_mask				)
	
			PARAM_DEF_GROUP_CLOSED( Emission Image, EMISSION_IMAGE_PARAM_NB_MAX )
				PARAM_DEF_BOOL_OFF(			emission_image_use )
				PARAM_DEF_BIND_2D_ALONE(	emission_image_bind			)	//todo deal with the up limit
	//			{	nullptr,	PARAM_FILENAME,	"emission_image_filename",	TYPE_IO_PART_EMISSION_IMAGE, 0.,	0, 0, nullptr, nullptr },
				PARAM_DEF_REAL_INF(			emission_image_u_offset,	.5,	0	)
				PARAM_DEF_REAL_INF(			emission_image_v_offset,	.5,	0	)
				PARAM_DEF_REAL_INF(			emission_image_u_factor,	0, 1	)
				PARAM_DEF_REAL_INF(			emission_image_v_factor,	0, 1	)
				PARAM_DEF_BOOL_OFF(			emission_image_uv_clamped	)

		PARAM_DEF_GROUP_CLOSED( Emission Speed, EMISSION_SPEED_PARAM_NB )
			PARAM_DEF_POINT_UV(			emission_speed						)
			PARAM_DEF_REAL_ONE(			emission_speed_axe					)
			PARAM_DEF_REAL_ZERO(		emission_speed_factor				)
			PARAM_DEF_REAL_ONE(			emission_object_speed_factor		)
			PARAM_DEF_BOOL_OFF(			emission_object_speed_interval		)
			PARAM_DEF_REAL_ZERO(		emission_object_speed_interval_min	)
			PARAM_DEF_REAL_ONE(			emission_object_speed_interval_max	)

		//	Jitter
			PARAM_DEF_REAL(				jitter_cone_angle,			90./8., 0.,	0., 180.	)
			PARAM_DEF_REAL_ONE(			jitter_speed_abs			)
			PARAM_DEF_REAL_ZERO(		jitter_jitter				)
			PARAM_DEF_GAIN(				jitter_jitter_gain			)
			PARAM_DEF_BIAS(				jitter_jitter_bias			)
			PARAM_DEF_INT32(			jitter_vec_nb,				128,	64*256,		1, JITTER_VEC_NB_MAX	)
			PARAM_DEF_INT32(			jitter_seed,				7777,	8888,		0, PARAM_MAX_INT32		)
			PARAM_DEF_SCALE_UVA( 		jitter_factor				)

		PARAM_DEF_GROUP_CLOSED(	Living Box, LIVING_BOX_PARAM_NB )
			PARAM_DEF_SYMBO_PSTR_ZERO(	living_box_type,			c_poid::str_living_box_type )
			PARAM_DEF_REAL_ONE(			living_box_bounce_factor	)
			PARAM_DEF_POINT_XYZ(		living_box_center			)
			PARAM_DEF_SCALE_XYZF(		living_box_size				)

		PARAM_DEF_GROUP_CLOSED( Motion, MOTION_PARAM_NB )
			PARAM_DEF_BOOL_ON(			move		)
		//	Physic
			PARAM_DEF_POINT_XYZ( 		Accel		)
			PARAM_DEF_POINT_XYZ( 		Speed		)
			PARAM_DEF_POINT_XYZ( 		Offset		)
			PARAM_DEF_BOOL_OFF( 		field_speed	)
			PARAM_DEF_BOOL_OFF( 		field_acc	)
	//		PARAM_DEF_BOOL_OFF( def_on_acceleration )
			PARAM_DEF_REAL_ZERO( 		viscosity	)
			PARAM_DEF_REAL_ZERO( 		speed_max	)

		PARAM_DEF_GROUP_CLOSED( Turbulence, TURBULENCE_PARAM_NB )
			PARAM_DEF_REAL_ZERO(		turb_accel			)
			PARAM_DEF_REAL_ZERO(		turb_speed			)
			PARAM_DEF_BOOL_ON(			turb_fractal_sum	)
			PARAM_DEF_REAL_INF(			turb_freq,			2,	1		)
			PARAM_DEF_INT32(			turb_harm_nb,		2,	1,		1, PARAM_MAX_INT32	)
			PARAM_DEF_GAIN(				turb_gain			)
			PARAM_DEF_BIAS(				turb_bias			)
			PARAM_DEF_REAL_INF(			turb_offset_X,		12.2345, 0	)
			PARAM_DEF_REAL_INF(			turb_offset_Y,		12.2345, 0	)
			PARAM_DEF_REAL_INF(			turb_offset_Z,		12.2345, 0	)
			PARAM_DEF_SCALE_XYZ(		turb_factor			)

		PARAM_DEF_GROUP_CLOSED( Render Color, RENDER_COLOR_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			render_life_color )
			PARAM_DEF_REAL_ONE_ZERO(	render_life_color_ease_in	)
			PARAM_DEF_REAL_ONE_ZERO(	render_life_color_ease_out	)
			PARAM_DEF_BIND_2D_ALONE(	render_life_image_bind		)	//todo deal with the up limit
	//		{	nullptr,	PARAM_FILENAME,	"render_life_image",		TYPE_IO_PART_COLOR_MAP, 0.,	0, 0, nullptr, nullptr },
			PARAM_DEF_BOOL_OFF(			render_life_color_map		)
			PARAM_DEF_REAL_INF(			color_map_v,				0, .5	)
			PARAM_DEF_REAL_INF(			color_map_dv,				0, .5	)
			PARAM_DEF_GAIN(				color_map_alpha_gain		)
			PARAM_DEF_BIAS(				color_map_alpha_bias		)
			PARAM_DEF_BOOL_OFF(			render_map_interp			)

		PARAM_DEF_GROUP_CLOSED( Visibilty, VISIBILITY_MAP_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			visibility_image			)
			PARAM_DEF_BIND_2D_ALONE(	visibility_image_bind		)	//todo deal with the up limit
			PARAM_DEF_REAL_INF(			visibility_image_u,			0, .5	)
			PARAM_DEF_REAL_INF(			visibility_image_v,			0, .5	)
			PARAM_DEF_REAL_ONE(			visibility_image_u_factor	)
			PARAM_DEF_REAL_ONE(			visibility_image_v_factor	)
			PARAM_DEF_BOOL_OFF(			visibility_uv_clamped		)

		PARAM_DEF_GROUP_CLOSED( Sort , SORT_PARAM_NB )
			PARAM_DEF_SYMBO_PSTR_ZERO(	sort_render,		str_render_sort )
			PARAM_DEF_REAL_LOCKED(		sort_min			)
			PARAM_DEF_REAL_LOCKED(		sort_max			)
			PARAM_DEF_BOOL_OFF(			sort_ease_color		)
			PARAM_DEF_REAL_ZERO(		sort_color_begin	)
			PARAM_DEF_REAL_ZERO(		sort_color_end		)
			PARAM_DEF_REAL_ONE_ZERO(	sort_color_ease_in	)
			PARAM_DEF_REAL_ONE_ZERO(	sort_color_ease_out	)

		PARAM_DEF_GROUP_CLOSED( Render Geo, RENDER_PARAM_NB )
			PARAM_DEF_BOOL_ON(			clipping )
		//todoq	avoid value 0	
			PARAM_DEF_BOOL_OFF(			render_line_smooth		)
			PARAM_DEF_REAL_INF(			render_aperture,		10, 1	)

			PARAM_DEF_BOOL_OFF(			render_size_life		)
			PARAM_DEF_BIAS(				render_size_life_bias	)
			PARAM_DEF_SCALE_UVAF(		render_size_end			)

			PARAM_DEF_BOOL_ON(			internal_def			)

			PARAM_DEF_REAL_ZERO(		rotate_freq				)
			PARAM_DEF_REAL_ZERO(		rotate_space_freq		)
			PARAM_DEF_AXE_Y(			rotate_space_axe		)
			PARAM_DEF_REAL_ZERO(		rotate_id_factor		)

			PARAM_DEF_SCALE_XYZ(		rotate_direction		)
			PARAM_DEF_SCALE_UVAF(		render_scale			)
			PARAM_DEF_SCALE_UVA(		render_field_size		)
			PARAM_DEF_REAL_ZERO(		render_field_size_facto	)

		PARAM_DEF_GROUP_CLOSED( Close, CLOSE_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			close_active		)
			PARAM_DEF_REAL_ONE(			close_dist			)
			PARAM_DEF_INT32_LOCKED(		close_nb			)

		PARAM_DEF_GROUP_CLOSED( Mocap, MOCAP_PARAM_NB )
	//		{	nullptr,	PARAM_INT32,	"mocap_feed_channel",		1, 0,	0, c_bdd_mocap::MOCAP_CHANNEL_NB_MAX, nullptr, nullptr },	
		//todoq	avoid value 0
		PARAM_DEF_GROUP_CLOSED( Net, NET_PARAM_NB )	
			PARAM_DEF_BOOL_OFF(			net_received		)
			PARAM_DEF_BOOL_OFF(			net_send			)
			PARAM_DEF_INT32(			net_channel,		2, 1,	1, c_net::CHANNEL_NB		)
			PARAM_DEF_INT32(			net_channel_sub,	2, 1,	1, c_net::CHANNEL_SUB_NB	)

		PARAM_DEF_GROUP_CLOSED( Misc, MISC_PARAM_NB )
			PARAM_DEF_INT32(			special,			1, 0,	0, 2	)
			PARAM_DEF_BOOL_OFF(			feed_bdd_point		)
	};
}

//todoq check effect on ttk_part was param_init()
void	c_bdd_particle::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt( h, _s_render_type );
	param_set_pt( h, _render_size_min );
//	life
	++h;
		param_set_pt( h, _b_restart_trig_ui );
		param_set_pt( h, _nb_allocated_ui );
		param_set_pt( h, _nb_max_ui );
		param_set_pt( h, _nb_min );
		param_set_pt( h, _nb_by_sec );
		param_set_pt( h, _nb_to_create_trig );
		param_set_pt( h, _nb_part_alive );
		param_set_pt( h, _b_flow_continuous );
		param_set_pt( h, _b_regular );

		param_set_pt( h, _life_time );
		param_set_pt( h, _life_time_jitter );
		param_set_pt( h, _life_time_jitter_gain );
		param_set_pt( h, _life_time_jitter_bias );
		param_set_pt( h, _s_id_generator );

//	emission position
	++h;
		param_attach_obj_no_inc( h, (c_obj_ui*) _emission_bdd );
		param_set_pt( h, _emission_bdd_name_sym );
		param_set_pt( h, _b_emission_use_prev_ui );
		param_set_pt_3( h, _emission_origin );
		param_set_pt( h, _emission_axe );

		param_set_pt_3( h, _emission_box_size_ui );
		param_set_pt( h, _emission_box_size_factor );

		_placer.param_set( this, h );

		++h;
			param_set_pt( h, _b_emission_image_use_ui );
			param_set_pt( h, _s_bind_emission_map );
			param_set_pt( h, _emission_image_u_offset );
			param_set_pt( h, _emission_image_v_offset );
			param_set_pt( h, _emission_image_u_factor );
			param_set_pt( h, _emission_image_v_factor );
			param_set_pt( h, _b_emission_image_clamped_ui );
//	emission speed
	++h;
		param_set_pt_3( h, _emission_speed_ui );
		param_set_pt( h, _emission_speed_factor );
		param_set_pt( h, _emission_object_speed_factor );		//ie
		param_set_pt( h, _b_emission_object_speed_interval );	//ie
		param_set_pt( h, _emission_object_speed_interval_min );	//ie
		param_set_pt( h, _emission_object_speed_interval_max );	//ie
//	Jitter
		param_set_pt( h, _jitter_cone_angle );
		param_set_pt( h, jitter_speed_abs );
		param_set_pt( h, jitter_jitter );
		param_set_pt( h, jitter_jitter_gain );
		param_set_pt( h, jitter_jitter_bias );
		param_set_pt( h, _jitter_vec_nb );
		param_set_pt( h, _jitter_seed );
		param_set_pt_3( h, _jitter_factor );
//	living box
	++h;
		param_set_pt( h, _s_living_box_type );
		param_set_pt( h, _living_box_bounce_factor );
		param_set_pt_3( h, _living_box_center );
		param_set_pt_3( h, _living_box_size_ui );
		param_set_pt( h, _living_box_size_factor );	
//	Motion
	++h;
		param_set_pt( h, b_move );
		param_set_pt_3( h, _accel );
		param_set_pt_3( h, _speed );
		param_set_pt_3( h, _offset );

		param_set_pt( h, b_field_speed_ui );
		param_set_pt( h, b_field_acc_ui );
		param_set_pt( h, viscosity );
		param_set_pt( h, _speed_max );

//	turbulence
	++h;
		param_set_pt( h, turb_speed );
		param_set_pt( h, turb_accel );
		param_set_pt( h, b_turb_fractal_sum );
		param_set_pt( h, turb_freq );
		param_set_pt( h, turb_harm_nb );
		param_set_pt( h, turb_gain );
		param_set_pt( h, turb_bias );
		param_set_pt_3( h, turb_offset_ui );
		param_set_pt_3( h, turb_factor );

//	Rendering
	++h;
		param_set_pt( h, _b_render_life_color );
		param_set_pt( h, _render_life_color_ease_in );
		param_set_pt( h, _render_life_color_ease_out );
		param_set_pt( h, _s_bind_life_color_map );
		param_set_pt( h, _b_render_life_color_map_ui );
		param_set_pt( h, color_map_v );
		param_set_pt( h, color_map_dv );
		param_set_pt( h, color_map_alpha_gain );
		param_set_pt( h, color_map_alpha_bias );
		param_set_pt( h, b_render_map_interp );

	++h;
		param_set_pt( h, _b_visibility_ui );
		param_set_pt( h, s_bind_visibility );
		param_set_pt( h, img_visibility_u_ori );
		param_set_pt( h, img_visibility_v_ori );
		param_set_pt( h, img_visibility_u_factor );
		param_set_pt( h, img_visibility_v_factor );
		param_set_pt( h, _b_visibility_clamped_ui );

	++h;
		param_set_pt( h, _s_render_sort );
		param_set_pt( h, _render_sort_min );
		param_set_pt( h, _render_sort_max );
		param_set_pt( h, _b_sort_ease_color );
		param_set_pt( h, _sort_color_begin );
		param_set_pt( h, _sort_color_end );
		param_set_pt( h, _sort_color_ease_in );
		param_set_pt( h, _sort_color_ease_out );

	++h;
		param_set_pt( h, _b_clipping_ui );
		param_set_pt( h, _b_render_line_smooth );
		param_set_pt( h, _render_aperture );

		param_set_pt( h, _b_render_size_life );
		param_set_pt( h, _render_size_life_bias );	
		param_set_pt_4( h, _render_size_death_ui );

		param_set_pt( h, b_internal_def );

		param_set_pt( h, rotate_freq );
		param_set_pt( h, rotate_space_freq );
		param_set_pt( h, rotate_space_axe );
		param_set_pt( h, rotate_id_factor );
		param_set_pt_3( h, rotate_direction );

		param_set_pt_4( h, _scale_ui );
		param_set_pt_4( h, _field_size_ui );

	++h;
		param_set_pt( h, _b_part_close );
		param_set_pt( h, _part_close_dist );
		param_set_pt( h, _part_close_nb );

	++h;
	//	param_set_pt( h, mocap_feed_channel);

	++h;
		param_set_pt( h, _net_buf.get_receive_ui_pt() );
		param_set_pt( h, _net_buf.get_send_ui_pt() );
		param_set_pt( h, _net_buf.get_net_channel_pt() );
		param_set_pt( h, _net_buf.get_net_channel_sub_pt() );

	++h;
		param_set_pt( h, s_special );
		param_set_pt( h, _b_feed_bdd_point_ui );

	err_param_init_pt(h);
}

AAA_ERR	c_bdd_particle::load_do_before(  o_str CONST & filename )
{
	//todoqibc	dirty and slow but works
	dealloc_part();
	return AAA_OK;
}

//todo get rid of this stuff already done in obj_ui
void c_bdd_particle::init()
{
	_nb_allocated = 0;
	_nb_max = 0;
	_nb_active = 0;
//	index_start = index_stop = 0;

	jitter_jitter_last = jitter_jitter - REAL(1);

	_field_in = nullptr;
	_field_out = nullptr;
	_vec_jitter = nullptr;
	clear_v3( _emission_origin_last	);
	clear_v3( _emission_origin_speed_last );
	clear_v3( _jitter_factor_last );

	_part_close_nb = 0;
}


//	build a new set of jitter vectors
void c_bdd_particle::init_vec_jitter( INT32 CONST nb_in )
{
	if( !_vec_jitter )
		alloc_vec_jitter( n_bdd_particle::JITTER_VEC_NB_MAX );
	if( _vec_jitter )
	{
		c_rand_lin	rand_loc;
		REAL*		vec;
		REAL		tmp;
		REAL		s;
		rand_loc.set_seed( _jitter_seed );

		vec = _vec_jitter;
		for( INT32 i = 0 ; i < nb_in ; ++i )
		{
			do
			{
				*(vec+_i_axe_u)			= rand_loc.get_fp32() * REAL(2) * _jitter_factor[0];
				*(vec+_i_axe_v)			= rand_loc.get_fp32() * REAL(2) * _jitter_factor[1];
				*(vec+_emission_axe)	= rand_loc.get_fp32() * REAL(2) * _jitter_factor[2];
				s = norm_squared_v3r( vec );
			}
			while( s > 1. );

			s = SQRT(s);
			tmp = rand_loc.get_fp32() + REAL(.5);
			gain_slick_if_needed( tmp, jitter_jitter_gain );
			bias_slick_if_needed( tmp, jitter_jitter_bias );
			s = (REAL(1) + jitter_jitter * (tmp-REAL(.5)) * REAL(2)) / s;

			scale_v3( vec, s );
			vec += 3;
		}
		jitter_jitter_last		= jitter_jitter;
		jitter_jitter_gain_last = jitter_jitter_gain;
		jitter_jitter_bias_last = jitter_jitter_bias;
		_jitter_vec_nb_last		= _jitter_vec_nb;
		_jitter_seed_last		= _jitter_seed;
		cpy_v3( _jitter_factor_last, _jitter_factor  );
		_emission_axe_last		= _emission_axe;
	}
}

void c_bdd_particle::alloc_vec_jitter( INT32 CONST nb_in )
{
	if( !_vec_jitter )
	{
		_vec_jitter = new REAL[nb_in*3];
		init_vec_jitter( _jitter_vec_nb );
	}
}

void c_bdd_particle::dealloc_vec_jitter()
{
	SAFE_DELETE_ARRAY( _vec_jitter );
}

void c_bdd_particle::alloc_part( INT32 CONST nb_in )
{
	if( _field_in )
		return;

	_field_in = new REAL[nb_in*3];
	if( !_field_in )
		goto exit;

	_field_out = new REAL[nb_in*3];
	if( !_field_out )
		goto exit;

	if( !_poids.alloc(nb_in) )
		goto exit;
	if( !_looper.set_capacity(nb_in) )
		goto exit;

	_nb_active = 0;
	//	index_stop = 0;
	_b_restart_trig_ui = true;
	_nb_allocated_ui = _nb_allocated = nb_in;	
	_nb_max = _nb_max_ui;
	return;
exit:
	dealloc();
}

#if	PART_VECTOR
struct part_kill_one 
{
	c_bdd_particle* bdd_part;
	part_kill_one(c_bdd_particle* target) : bdd_part(target)	{}
	void operator()( c_particle* pa ) {	bdd_part->kill_one( pa );	}
};
#endif

void c_bdd_particle::dealloc_part()
{
	if( _field_in )
	{
		LOOPER_BEGIN()
			kill_one( pp );
		LOOPER_END()
		_looper.dealloc();
		_poids.dealloc();

		SAFE_DELETE_ARRAY( _field_in );
		SAFE_DELETE_ARRAY( _field_out );

		_nb_active = _nb_allocated = 0;
//		index_stop = 0;
		_nb_max = 0;
	}
}

//todoqq check if we need this fn
void c_bdd_particle::alloc()
{
	alloc_part( MAX(_nb_allocated_ui, _nb_max_ui) );
	alloc_vec_jitter( n_bdd_particle::JITTER_VEC_NB_MAX );
}

void c_bdd_particle::dealloc()
{
	dealloc_part();
	dealloc_vec_jitter();
}

CONSTRUCTOR_CREATE(c_bdd_particle)
	,_nb_part_alive(0)
	,_b_render_life_color_map(false)
	,_emission_bdd(nullptr)
	,_emission_axe_last(-42)
	,_id_generator(0)
{
	param_init_with( n_bdd_particle::param, n_bdd_particle::PARAM_NB_MAX ); // bdd_particle_param, BDD_PARTICLE_PARAM_NB_MAX);

	init();
//	alloc();
}

c_bdd_particle::~c_bdd_particle()
{
	if( cur == this )
		cur = nullptr;
	dealloc();
}

FINLINE	void c_bdd_particle::kill_one( c_particle* CONST pt )
{
	if( _b_feed_bdd_point )
		bdd_point_cur->delete_dot( pt->get_id() );
	if( _net_buf.is_send() )
		net_send_kill( _poids.get_index(pt) );
	if( pt->is_active() )
		--_nb_active;
	pt->set_dead();
}

bool c_bdd_particle::kill_by_id( INT32 id )
{
	c_particle*	p = nullptr;
	if( _s_id_generator == c_poid::ID_GENERATOR_FROM_INDEX )
		p = _poids.get_pt_from_index( id-1 );
	else
		ERR_PRINT_STRING( "id generator is not by index, skipping kill" );
	if( p )
	{
		kill_one( p );
		return true;
	}
	return false;
}

void c_bdd_particle::add_particle_at( REAL* pos )
{
	++_nb_to_create_trig;
	if( pos )
	{
		_add_pos.push_back( pos[0] );
		_add_pos.push_back( pos[1] );
		_add_pos.push_back( pos[2] );
	}
	else
	{
		_add_pos.push_back( 0. );
		_add_pos.push_back( 0. );
		_add_pos.push_back( 0. );
	}
}

//hack
extern	c_rand_lin		rand_create_and_place;

FINLINE	c_particle* c_bdd_particle::create_one( INT32 CONST index, REAL CONST birth_time, bool CONST b_ressucitate )
{
	c_particle* pa = _poids.get_pt_from_index(index);
	switch( _s_id_generator )
	{
	case c_poid::ID_GENERATOR_RAND:			pa->set_alive( (rand_create_and_place.get_uint32() >> 12) + 1 );	break;	// +1 is for lua
	case c_poid::ID_GENERATOR_UNIQUE:		pa->set_alive( ++_id_generator );									break;	// preincrement to start at 1 for Lua
	case c_poid::ID_GENERATOR_FROM_INDEX:
	default:								pa->set_alive( index + 1 );											break;	// +1 is for Lua							
	}

	//	life
	REAL tmp = rand_create_and_place.get_fp32_01();
	gain_slick_if_needed( tmp, _life_time_jitter_gain );
	bias_slick_if_needed( tmp, _life_time_jitter_bias );
	tmp = _life_time * ( REAL(1) + _life_time_jitter * (tmp - REAL(.5)) * REAL(2) ) ;

	pa->set_life( birth_time, tmp );
	

//	pa->id =  rand_create_and_place.get_uint32();

//	if get_uint32 bug ?
//	pa->id =  RAND_I32();

	tmp = color_map_v + rand_create_and_place.get_fp32() * color_map_dv;
	pa->set_color_map_v( CLAMP_01( tmp ) );

	//	if it just died
	if( !b_ressucitate )
	{
		++_nb_active;
//		if ( index < index_start )
//			index_start = index;
//		else if ( index >= index_stop )
//			index_stop = index;
	}
	return pa;
}


bool c_bdd_particle::place_and_create_one( INT32 CONST index, REAL CONST birth_time, REAL CONST time_cur, bool CONST b_ressucitate )
{
	REAL	vec[4];

	if( _placer.place_one( index + 1, vec ) )
	{
		REAL	pos[3];
		REAL	speed[3];
		//	where
		if( _emission_bdd )
			_emission_bdd->get_point_and_speed_rnd_time_rel( pos, speed, REAL(_delta_t.get_t()) - birth_time );
		else if( _b_emission_use_prev_ui )
			c_layer::get_cur()->get_bdd_prev()->get_point_and_speed_rnd_time_rel( pos, speed, REAL(_delta_t.get_t()) - birth_time );
		else
		{
			clear_v3( pos );
			clear_v3( speed );
		}
		if( _b_emission_object_speed_interval )
		{
			REAL nor = norm_v3r( speed );
			if( OUTSIDE( nor, _emission_object_speed_interval_min, _emission_object_speed_interval_max ) )
				return false;
		}
			
		//	consider box
		pos[_i_axe_u]		+= vec[0] * _emission_box_size[0];
		pos[_i_axe_v]		+= vec[1] * _emission_box_size[1];
		pos[_emission_axe]	+= vec[2] * _emission_box_size[2];

		if( !_add_pos.empty() )
		{
			REAL vec[3];
			vec[2] = _add_pos.back();		
			_add_pos.pop_back();
			vec[1] = _add_pos.back();
			_add_pos.pop_back();
			vec[0] = _add_pos.back();
			_add_pos.pop_back();
			add_v3( pos, vec );
			clear_v3( speed );
		}

	
		REAL	pos_origin[3];
		REAL	vec_inter[3];
		if( _dt >= 0. )
		{
			DOUBLE inter = (birth_time-_delta_t.get_t_last()) / _dt;
			interpolate_v3(	pos_origin,	_emission_origin_last,			_emission_origin,		inter );
			interpolate_v3(	vec_inter,	_emission_origin_speed_last,	_emission_origin_speed,	inter );
		}
		else
		{
			cpy_v3(			pos_origin, _emission_origin );
			cpy_v3(			vec_inter, _emission_origin_speed );
		}

		c_particle* pa = create_one( index, birth_time, b_ressucitate );

		pa->set_dt( time_cur - birth_time );

		add_v3( pa->get_pos(), pos, pos_origin );
		add_v3( speed, vec_inter ); 

		//	speed including jitter initialization
		REAL*	jitter = _vec_jitter + (pa->get_id() % _jitter_vec_nb) * 3;

		//todo	use emission_object_speed_factor_last for real smoothness ?
		scale_v3( speed, _emission_object_speed_factor );

		//nib
		//todo opt
		pa->get_speed()[0] = speed[0] + _emission_speed[0] + jitter[0] * _jitter_speed[0];
		pa->get_speed()[1] = speed[1] + _emission_speed[1] + jitter[1] * _jitter_speed[1];
		pa->get_speed()[2] = speed[2] + _emission_speed[2] + jitter[2] * _jitter_speed[2];

		if( _b_deformer )
		{	//todo check this work with net transmition
			//todoopt	regroup all these to call deformer in one pass only
			c_def_node::get_cur()->apply( pa->get_pos_to_draw(), pa->get_pos(), 1 );
		}
		if( _b_emission_image_use_ui )
		{
			//todoqq		f_axe_u = -f_axe_u;
			_img_emission_map->get_color4r_from_uv_nearest(
					pa->get_color_pt(),
					(( _emission_axe == 0 ) ? -( vec[0] + REAL(.5) ) : ( vec[0] + REAL(.5) ) ) * _emission_image_u_factor + _emission_image_u_offset,
					( vec[1] + REAL(.5) ) * _emission_image_v_factor + _emission_image_v_offset,
					_b_emission_image_clamped_ui
					);
		}
		if( _b_feed_bdd_point )
			bdd_point_cur->new_dot( pa->get_id() );
		if( _net_buf.is_send() )
			net_send_create( index, time_cur );
		return true;
	}
	return false;
}


void c_bdd_particle::place_one_on_side( c_particle* CONST pa, INT32 CONST axe )
{
	INT32	i_u;
	INT32	i_v;
	axe_build_index( i_u, i_v, axe	);

	pa->get_pos()[i_u] = rand_create_and_place.get_fp32() * _living_box_size[i_u] + _living_box_center[i_u];
	pa->get_pos()[i_v] = rand_create_and_place.get_fp32() * _living_box_size[i_v] + _living_box_center[i_v];
}

/*
FINLINE	REAL c_bdd_particle::get_part_age_rel( c_particle * pa)
{
	return pa->get_age_rel_at( delta_t.get_t() );
}

FINLINE	REAL c_bdd_particle::get_part_age_rel_at( c_particle * pa, REAL time)
{
	return pa->get_age_rel_at( time);
}
*/

void	c_bdd_particle::restart()
{
	_b_restart_trig_ui = true;
}


#if	APP_SPECIAL_NIB()
extern	bool	b_nib;
#endif

//c_rand_lin	rand_on_time;
namespace
{
CONSTEXPR	bool b_strict_box = true;
}

void	c_bdd_particle::copy_pos_to( REAL* dst )
{
	LOOPER_BEGIN()
		cpy_v3( dst, pp->get_pos() );
		dst += 3;
	LOOPER_END()
}
void	c_bdd_particle::copy_pos_to_draw_from( REAL CONST * src )
{
	LOOPER_BEGIN()
		REAL*		pos = pp->get_pos_to_draw();
		cpy_v3( pp->get_pos_to_draw_last(), pos );
		cpy_v3( pos, src );
		src += 3;
	LOOPER_END()
}

void	c_bdd_particle::copy_pos_to_pos_to_draw()
{
	LOOPER_BEGIN()
		pp->cpy_pos_to_pos_to_draw();
	LOOPER_END()
}

void	c_bdd_particle::update()
{
	cur = this;
//hack ?
	if( !draw::is_render_first_pass() )
		return;

	_net_buf.update();
	_b_feed_bdd_point = _b_feed_bdd_point_ui && bdd_point_cur;

	if( _delta_t.update() || _b_restart_trig_ui )
	{
		_nb_max = 0;
		_id_generator = 0;
		_b_restart_trig_ui = false;
//		if( s_special == 2 )
//			c_tex_anim::cur->load_textures();
	}
	_dt = REAL(_delta_t.get_dt());
	if( _dt > 1. )
	{
//		_nb_max = 0;
//		_part_id = 0;
//		_b_restart_trig_ui = false;
//hack monart avoid restart
	//	_dt = REAL(.04);
	//	_delta_t.force_dt( _dt );
	}

	//	prepare the emission axes
	axe_build_index_vert( _i_axe_u, _i_axe_v, _emission_axe );

	_placer.update();
	//todo this is a _target_name_symbo pattern and should be generalized as a pattern
	//	check with c_bdd::update_bdd_target
	if( !_emission_bdd_name_sym.is_empty() )
	{
		if( !_emission_bdd || !_emission_bdd->is_name_symbo( _emission_bdd_name_sym ) || !_emission_bdd->get_root() )
			_emission_bdd = find_bdd_by_name_symbo( _emission_bdd_name_sym );
	}
	else
		_emission_bdd = nullptr;

	if( _emission_bdd )
	{
	}
	if( _b_emission_image_use_ui )
	{
		_img_emission_map = g_bind_img_2d->get_ready( _s_bind_emission_map );
		_b_emission_image_use_ui = _img_emission_map->is_ok();
	}
	if( _b_render_life_color_map_ui )
	{
		_img_life_color_map = g_bind_img_2d->get_ready( _s_bind_life_color_map );	//todoqq	this way to heavy doan the pipe the check for read is too much 
		_b_render_life_color_map = _img_life_color_map->is_ok();
	}
	else
		_b_render_life_color_map = false; 
	if( _b_visibility_ui )
	{
		img_visibility = g_bind_img_2d->get_ready( s_bind_visibility );	//todoqq	this way to heavy down the pipe the check for read is too much 
		_b_visibility = img_visibility->is_ok();
	}
	else
		_b_visibility = false; 

	if( _dt != 0. )
	{	//	prepare the living box
		if( _s_living_box_type )
		{
			REAL living_box_size_half[3];
			scale_v3( _living_box_size, _living_box_size_ui, _living_box_size_factor );
			scale_v3( living_box_size_half, _living_box_size, .5 );
			sub_v3( _living_box_min, _living_box_center, living_box_size_half );
			add_v3( _living_box_max, _living_box_center, living_box_size_half );
		}

		//	prepare the emission box
		scale_v3( _emission_box_size, _emission_box_size_ui, _emission_box_size_factor );

		//	prepare the emission speed
		_emission_speed[_i_axe_u]		= _emission_speed_ui[0] * _emission_speed_factor;
		_emission_speed[_i_axe_v]		= _emission_speed_ui[1] * _emission_speed_factor;
		_emission_speed[_emission_axe]	= _emission_speed_ui[2] * _emission_speed_factor;

		//	update jitter
		if(		jitter_jitter_last != jitter_jitter
			|| 	jitter_jitter_gain_last != jitter_jitter_gain
			||	jitter_jitter_bias_last != jitter_jitter_bias
			||	_jitter_vec_nb_last != _jitter_vec_nb
			||	_jitter_seed_last != _jitter_seed
			||	is_diff_v3( _jitter_factor_last, _jitter_factor )
			||	_emission_axe_last != _emission_axe
			)
			init_vec_jitter( _jitter_vec_nb );

		if( _dt <= 0. )
			clear_v3( _emission_origin_speed );
		else
			sub_then_scale_v3r( _emission_origin_speed, _emission_origin, _emission_origin_last, REAL(1)/_dt );

		//todoqq	f_axe_u = emission_map_u/(_emission_box_size[0] );
		//todoqq	f_axe_v = emission_map_v/(_emission_box_size[1] );
		//todoqq	if ( emission_axe == 0 )
		//todoqq		f_axe_u = -f_axe_u;

		REAL	emission_speed_norm;
		REAL	jitter_speed_loc;
		REAL	tmp;
		//	prepare jitter stuff
		emission_speed_norm = norm_v3r( _emission_speed );

		if( _jitter_cone_angle < 90.)
			tmp = SIN_TURN( _jitter_cone_angle/90.*.25 );
		else
			tmp = 1 - COS_TURN( _jitter_cone_angle/90.*.25 );
		jitter_speed_loc = emission_speed_norm * tmp + jitter_speed_abs;

		//	prepare factors
		set_v3( _jitter_speed, jitter_speed_loc );	//todo this is strange
		//todo restore dynamic jitter
		//jitter_speed[i_dim] = param_strenght_influence[i_dim] * jitter_speed_loc;
	//	tmp = turb_accel / turb_freq;
		scale_v3( turb_accel_factor, turb_factor, turb_accel / turb_freq );
		scale_v3( turb_speed_factor, turb_factor, turb_speed / turb_freq );
		scale_v3( turb_offset, turb_offset_ui, 1. / turb_freq );

		_b_turb_factor = false;

		_b_turb_factor_axe[0] = ( turb_accel_factor[0] != 0. ) || ( turb_speed_factor[0] != 0. );
		_b_turb_factor = _b_turb_factor_axe[0];

		_b_turb_factor_axe[1] = ( turb_accel_factor[1] != 0. ) || ( turb_speed_factor[1] != 0. );
		_b_turb_factor |= _b_turb_factor_axe[1];

		_b_turb_factor_axe[2] = ( turb_accel_factor[2] != 0. ) || ( turb_speed_factor[2] != 0. );
		_b_turb_factor |= _b_turb_factor_axe[2];
	}
	
	_scale[_i_axe_u]		= _scale_ui[0] * _scale_ui[3];
	_scale[_i_axe_v]		= _scale_ui[1] * _scale_ui[3];
	_scale[_emission_axe]	= _scale_ui[2] * _scale_ui[3];
	_b_scale = !is_all_one_v3( _scale );

//todoqq	use it
//	if( _b_render_life_color_map )
//	{
//		color_map_alpha_gain_factor = gain_slick_factor(color_map_alpha_gain);
//		color_map_alpha_bias_factor = bias_slick_factor(color_map_alpha_bias);
//	}

	//	MULTIPLE UPDATE START
//	if we render with polygons
//	todoq solve update when rendering switch
//todoqq	this test is bad and should be c_bdd_multiple::is_rendering_multuple_pass or equivalent
	if( c_multiple::cur->is_render_multiple() || _s_render_type == RENDER_QUAD
			//(	render_face_surface_last != render_face_surface
			//||	render_face_ratio_last != render_face_ratio
			//||	render_face_normal_axe_last != render_face_normal_axe
			//)
		)
	{
		REAL	tmp;
		INT32	i_axe, i_u, i_v;
		if( c_multiple::cur )	// crash if RENDER_QUAD and no c_multiple::cur // crash if RENDER_QUAD and no c_multiple::cur 
		{
			//layer didn't update it so we force it
			if( !c_multiple::cur->is_render_multiple() )
				c_multiple::cur->bdd_do_force_update();

			//	prepare the render axes
			i_axe = c_multiple::cur->get_axe();

			cpy_v3( _render_size, c_multiple::cur->get_size() );
			tmp = aaa::MAX( _render_size[0], _render_size[1], _render_size[2] );

			_b_multiple_use_list = c_multiple::cur->can_use_list();	
			_b_multiple_use_list &= !c_multiple::cur->is_align_normal();	//hack for now
			_b_multiple_use_list &= !c_multiple::cur->is_align_random();
		}
		else
		{
			set_v3( _render_size, 1 );
			tmp = 1.;
			i_axe = 2;
			_b_multiple_use_list = false;
		}
		axe_build_index_vert( i_u, i_v, i_axe );
		
		if( _b_render_size_life )
			//todo check if the two sizes are really different
		{
			REAL tmp_b;
			_render_size_death[i_u]		= _render_size_death_ui[0] * _render_size_death_ui[3];
			_render_size_death[i_v]		= _render_size_death_ui[1] * _render_size_death_ui[3];
			_render_size_death[i_axe]	= _render_size_death_ui[2] * _render_size_death_ui[3];
			tmp_b = aaa::MAX( _render_size_death[0], _render_size_death[1], _render_size_death[2] );
			tmp = MAX( tmp, tmp_b );

			if( _b_multiple_use_list )
				multiple_compile_one( one_v4fp32 );
			_render_size_life_bias_factor = bias_factor( _render_size_life_bias );
		}
		else if( _b_multiple_use_list )
			multiple_compile_one( _render_size );

		_render_size_half = tmp * REAL(.5);

		//render_face_surface_last = render_face_surface;
		//render_face_ratio_last = render_face_ratio;
		//render_face_normal_axe_last = render_face_normal_axe;
	}
	else
		_render_size_half = .5;

	//	MULTIPLE UPDATE STOP

	_field_size[_i_axe_u]		= _field_size_ui[0] * _field_size_ui[3];
	_field_size[_i_axe_v]		= _field_size_ui[1] * _field_size_ui[3];
	_field_size[_emission_axe]	= _field_size_ui[2] * _field_size_ui[3];

	INT32 nb_to_create_this_frame = 0;
	if( _dt != 0. )
	{
		INT32 nb_max_wanted = MAX( _nb_allocated_ui, _nb_max_ui );
		if( nb_max_wanted != _nb_allocated  )
		{
			dealloc_part();
			alloc_part( nb_max_wanted );
			if( _nb_allocated == 0 )
			{
				//todo clarify this		
				ERR_PRINT_STRING( "%s() can't allocate so many particle dividing constraint by 4", __FUNCTION__ );
				if( _nb_allocated_ui > _nb_max_ui )
					_nb_allocated_ui /= 4;
				else
					_nb_max_ui /= 4;
			}

		}
		else
		{
			INT32 i = _nb_max_ui - _nb_max;
			if( i != 0 )
			{
				INT32	index;
				if( i > 0 )
					index = _nb_max;
				else
				{
					index = _nb_max_ui;
					i = -i;
				}
				for(  ; i>0; --i )
					kill_one( _poids.get_pt_from_index(index++) );
				_nb_max = _nb_max_ui;
			}
		}
		_nb_allocated_ui = _nb_allocated;

		//	find out how many new particle we need
		//	if( aaa::time::get_update_count() == 0 )
		
		if( _b_flow_continuous )
		{
			REAL tmp = _nb_by_sec * _dt;
			tmp += _nb_to_create_trig;
			_nb_to_create_trig = 0;
			nb_to_create_this_frame = I_FLOOR(tmp);
			tmp -= nb_to_create_this_frame;	//	floating left over for next frame
			if( _b_regular )
			{
				_nb_to_create_left_over += tmp;
				if( _nb_to_create_left_over > 1. || _nb_to_create_left_over < 0. )
				{
					++nb_to_create_this_frame;
					_nb_to_create_left_over = 0.;
				}
			}
			else if ( rand_create_and_place.get_fp32_01() < tmp )
				++nb_to_create_this_frame;
		}
		else
		{
			//todonow change in emergency (2025 December)
			//was _nb_max - _nb_active before StarSky production
			nb_to_create_this_frame = _nb_min - _nb_active;
		}

		nb_to_create_this_frame = aaa::MAX( nb_to_create_this_frame, 0, _nb_min - _nb_active );
		_b_accel = is_not_null_v3( _accel );
		_b_speed = is_not_null_v3( _speed );
		_b_offset = is_not_null_v3( _offset );
	}
	b_field_size = is_not_null_v3( _field_size);

	auto def = c_def_node::get_cur();
	if( !def->is_deforming() )
		def = nullptr;
	b_field = def && (((b_field_speed_ui||b_field_acc_ui) && b_move) || b_field_size );
	b_field_speed = b_field && b_field_speed_ui;
	b_field_acc = b_field && b_field_acc_ui;
	_b_deformer = !b_field && def && b_internal_def;

	if( b_move && _dt != 0. )
	{
		create_and_kill( nb_to_create_this_frame );

		//	prepare the speed field
		if( b_field )
		{
			copy_pos_to( _field_in );
			//todoq	we should have a memory zone for this tmp things
			def->get_field( _field_out, _field_in, _looper.get_nb() );
		}
#if	APP_SPECIAL_NIB()
		if( s_special && b_nib)
			nib_move();
		else
#endif
			move();
	}

	if( _b_deformer )
	{
		copy_pos_to( _field_out );
		//todoq	we should have a memory zone for this tmp things
		c_def_node::get_cur()->apply( _field_out, _looper.get_nb() );
		copy_pos_to_draw_from( _field_out );
	}
	else
		copy_pos_to_pos_to_draw();

	do_sort();
	_sort_color_nor_factor = 0.;
	if( _s_render_sort )
	{
		if( _b_sort_ease_color && (_sort_color_begin != _sort_color_end) )
			_sort_color_nor_factor = REAL(1.) / ( _sort_color_end - _sort_color_begin );
	}
//todo revive this
//	else if( _b_part_close )
//		do_close();

	cpy_v3( _emission_origin_last,			_emission_origin		);
	cpy_v3( _emission_origin_speed_last,	_emission_origin_speed	);
}

INT32	c_bdd_particle::get_point_nb()
{
	return _looper.get_nb();
}

//todo check index
FINLINE	c_particle*	c_bdd_particle::get_particle( INT32 CONST index )
{
	if( _s_render_sort )
		return _part_sort[index];
	else
		return _looper.get(index);
}

//todo check get_particle
bool	c_bdd_particle::get_point( REAL* CONST dst, INT32 CONST index )
{
	c_particle* p = get_particle( index );
	p->cpy_pos_to_draw(dst);
	return true;
}

INT32	c_bdd_particle::get_point_and_id( REAL* CONST dst, INT32 CONST index )
{
	c_particle* p = get_particle( index );
	p->cpy_pos_to_draw(dst);
	return p->get_id();
}

static	c_rand_lin		rand_point;

//todo use t_in
bool	c_bdd_particle::get_point_and_speed_rnd_time_rel( REAL* CONST dst, REAL* CONST speed, REAL t_in )
{
	UINT32 nb = get_point_nb(); 
	if( nb )
	{
		//	get one finger
		UINT32	i = (rand_point.get_uint32()>>8) % nb;
		c_particle*	p = get_particle( i );
		REAL inter = t_in / REAL(_delta_t.get_dt());
		interpolate_v3( dst,	p->get_pos_to_draw(),	p->get_pos_last(),		inter );
		interpolate_v3( speed,	p->get_speed(),			p->get_speed_last(),	inter );
		return true;
	}

	clear_v3( dst ); 
	clear_v3( speed );
	return false;
}

static	CONST	INT32	NET_CODE_CREATE = 0x11111111;
static	CONST	INT32	NET_CODE_KILL	= 0x22222222;

void c_bdd_particle::net_send_create( INT32 CONST index, REAL CONST	t )
{
	_net_buf.get_buffer_ready();
	_net_buf.flush_if_less_than( 100 );
	_net_buf.write_value( NET_CODE_CREATE, index );
	_poids.get_pt_from_index(index)->write_to_mem( &_net_buf );
}

void c_bdd_particle::net_send_kill( INT32 CONST index )
{
	_net_buf.get_buffer_ready();
	_net_buf.flush_if_less_than( 100 );
	_net_buf.write_value( NET_CODE_KILL, index );
}


void c_bdd_particle::create_and_kill( INT32 nb_to_create_this_frame )
{
	REAL	t = REAL(_delta_t.get_t());

/*
//todo re implement but with proba playing on number of changes
//		instead of individual test
UINT32		*p_id;
	pa = part;
	for ( index=nb; index>0; --index )
	{
		//	change id sometimes
		if ( rand_on_time.get_ufloat() < probability_on_time )
			*p_id = rand_on_time.get_uint32_with_seed( pa->id );
		++pa;
	}
*/
//#if	AAA_NET()
	if( _net_buf.is_receive() )
	{
		while( c_net_blk* blk_in = net->blk_take_by_type_channel( c_net::BLK_OBJ_DATA, _net_buf.get_net_channel() ) )
		{
			CONST UINT8*	pt		=	blk_in->get_data_pt();
			CONST UINT8*	pt_max	=	pt + blk_in->get_len();
			while( pt < pt_max )
			{	
				INT32	index;
				INT32	what;
				pt = c_net::read_value( pt, &what );
				switch( what )
				{
				case NET_CODE_CREATE:	pt = c_net::read_value( pt, &index );
										pt = _poids.get_pt_from_index(index)->read_from_mem( pt, t );
										break;
				case NET_CODE_KILL:		pt = c_net::read_value( pt, &index );
										_poids.get_pt_from_index(index)->set_dead();
										break;
				default:				debug_break( "bad net code" );
										break;
				}
			}
			net->blk_free( blk_in );
		}

		_looper.clear();
		INT32	index = -1;
		INT32	nb_to_process = _nb_active;
		while( ++index < _nb_max )
		{
			c_particle*	pa = _poids.get_pt_from_index(index);
			if ( pa->is_active() )
			{
				pa->set_dt( _dt );	//todo contradiction with time received ?
				_looper.add( pa );
			}
		}
		//	index_stop_ = index;
	}
	else
//#endif
	{
		_looper.clear();
		//	first pass life and death
		INT32	index = -1;
		INT32	nb_to_process = _nb_active;
		while( (++index < _nb_max) && (nb_to_process || nb_to_create_this_frame) )
		{
			c_particle* pa = _poids.get_pt_from_index(index);
			if( !pa->is_active() )
			{
				if( nb_to_create_this_frame )
				{
					REAL	loc_dt = _b_flow_continuous ? (_dt * rand_create_and_place.get_fp32_01()) : _dt;
					if( place_and_create_one( index, t - loc_dt, t, false ) )
						_looper.add( pa );
					--nb_to_create_this_frame;
				}
			}
			else if( t > pa->get_death() )
			{
				--nb_to_process;
				if( nb_to_create_this_frame )
				{
					kill_one( pa );
					//hack	reverse part of kill_one (we should be cleaner)
					
					REAL tmp;
					if( _b_flow_continuous )
						tmp = t - rand_create_and_place.get_fp32_max(_dt);
					else
						tmp = pa->get_death();
					if(	place_and_create_one( index, tmp, t, true) )
						_looper.add( pa );
					++_nb_active;
					--nb_to_create_this_frame;
				}
				else
					kill_one( pa );
			}
			else
			{
				--nb_to_process;
				pa->set_dt( _dt );
				_looper.add( pa );
			}
		}
		if( _net_buf.is_send() )
			_net_buf.flush();
//		index_stop = index;
	}
	_nb_part_alive = _looper.get_nb();
}

template< bool B_TURB, INT32 S_LIVING_BOX >
FINLINE	void c_bdd_particle::move_one( c_particle *pa )
{
	REAL	speed[3];
	REAL	pos_change[3];
	REAL	loc_dt = pa->get_dt();

	pa->cpy_to_last();	//hack this need for one fn only but make the update more heavy
	if( B_TURB )
	{
		REAL	loc_accel[3];
		REAL	f[3];
		REAL	tmp;

		cpy_v3( loc_accel, _accel );
		cpy_v3( speed, pa->get_speed() );
		cpy_v3( f, pa->get_pos() );			
		for( INT32 i_dim=0; i_dim<3; ++i_dim )
		{
			f[i_dim] += turb_offset[i_dim];
			//	move it : influence of the turbulence
			if( _b_turb_factor_axe[i_dim] )
			{
				FP32 vec[3];
				scale_v3( vec, f, turb_freq );
				if( b_turb_fractal_sum )
					tmp = fractalsum( vec, turb_harm_nb );
				else
					tmp = turbulence( vec, turb_harm_nb );
				//ttk	optimize gain and bias with factors everywhere
				gain_slick_if_needed( tmp, turb_gain);
				bias_slick_if_needed( tmp, turb_bias);
				tmp -= .5;
				//	calculate acceleration
				loc_accel[i_dim] += tmp * turb_accel_factor[i_dim];
				//	calculate speed
				speed[i_dim] += tmp * turb_speed_factor[i_dim];
			}
		}
		if( b_field_acc )
			add_v3( loc_accel, _field_tmp );
		add_scale_v3( speed, loc_accel, loc_dt );
	}
	else
	{
		if( b_field_acc )
		{
			REAL	loc_accel[3];
			if( _b_accel )
				add_v3( loc_accel, _field_tmp, _accel );
			else		
				cpy_v3( loc_accel, _field_tmp);
			add_scale_v3( speed, pa->get_speed(), loc_accel, loc_dt );
		}
		else
		{
			if( _b_accel )
				add_scale_v3(	speed, pa->get_speed(), _accel, loc_dt );
			else			
				cpy_v3(			speed, pa->get_speed() );
		}
	}
	//	calculate speed
	if ( b_field_speed )
		add_v3( speed, _field_tmp );
	_field_tmp += 3;

	if( viscosity != 0 )
	{
		scale_v3( speed, 1-viscosity );
		//speed -= viscosity*speed*speed;
	}
	if( _speed_max != 0 )
		limit_v3r( speed, _speed_max );
	cpy_v3( pa->get_speed(), speed );

	
	//	what come now don't accumulate in the poid because the poid speed is saved just before
	if( _b_speed )
		add_then_scale_v3r( pos_change, speed, _speed, loc_dt );
	else
		scale_v3( pos_change, speed, loc_dt);
	if( _b_offset )
		add_v3( pos_change, _offset);

	if( S_LIVING_BOX )
	{
		REAL	f[3];
		bool	b_exit = false;

		add_v3( f, pos_change, pa->get_pos() );

//todo
		//if( INSIDE_MIN_MAX( S_LIVING_BOX, c_poid::LIVING_BOX_WRAP_X, c_poid::LIVING_BOX_WRAP_Z ) )
		//{
		//	switch( S_LIVING_BOX )
		//	{
		//	case c_poid::LIVING_BOX_WRAP_X:
		//		pa->get_pos()[0] = FWRAP( f[0], _living_box_min[0], _living_box_max[0] );
		//		break;
		//	case c_poid::LIVING_BOX_WRAP_Y:
		//		pa->get_pos()[1] = FWRAP( f[1], _living_box_min[1], _living_box_max[1] );
		//		break;
		//	case c_poid::LIVING_BOX_WRAP_Z:
		//		pa->get_pos()[2] = FWRAP( f[2], _living_box_min[2], _living_box_max[2] );
		//		break;
		//	}
		//}
		//else
		{
			for( INT32 i_dim = 0; i_dim < 3; ++i_dim )
			{
				REAL	tmp = f[i_dim];
				if ( tmp < _living_box_min[i_dim] )
				{	//	particle out on the negative side
					switch( S_LIVING_BOX )
					{
					case c_poid::LIVING_BOX_DIE:
						kill_one( pa );
						b_exit = true;
						break;
					case c_poid::LIVING_BOX_WRAP:
//					case c_poid::LIVING_BOX_BOUNCE_SIDE_WRAP_AXE:	//todo
					case c_poid::LIVING_BOX_WRAP_RANDOM:
						tmp += _living_box_size[i_dim];
						if( b_strict_box && tmp < _living_box_min[i_dim] )
						//	force back strictly in the box
						{
							REAL loc_delta = tmp - pa->get_pos()[i_dim] - _living_box_center[i_dim];
							tmp = FMOD( tmp, _living_box_size[i_dim]);
							pa->get_pos()[i_dim] = tmp - loc_delta + _living_box_center[i_dim];
						}
						else
							pa->get_pos()[i_dim] += _living_box_size[i_dim];
						if( S_LIVING_BOX == c_poid::LIVING_BOX_WRAP_RANDOM )
						{
							place_one_on_side( pa, i_dim );
							b_exit = true;
						}
						break;
					case c_poid::LIVING_BOX_BOUNCE:
						pa->get_speed()[i_dim] *= -_living_box_bounce_factor;
						pa->get_speed()[ IMOD(i_dim+1,3) ] *= _living_box_bounce_factor;
						pa->get_speed()[ IMOD(i_dim+2,3) ] *= _living_box_bounce_factor;
						break;
					}
					if( b_exit )
						break;
				}
				else if ( tmp > _living_box_max[i_dim] )
				{	//	particle out the positive side
					switch( S_LIVING_BOX )
					{
					case c_poid::LIVING_BOX_DIE:
						kill_one( pa );
						b_exit = true;
						break;
					case c_poid::LIVING_BOX_WRAP:
//					case c_poid::LIVING_BOX_BOUNCE_SIDE_WRAP_AXE:	//todo
					case c_poid::LIVING_BOX_WRAP_RANDOM:
						tmp -= _living_box_size[i_dim];
						if ( b_strict_box && tmp > _living_box_max[i_dim] )
						//	force back strictly in the box
						{
							REAL loc_delta = tmp - pa->get_pos()[i_dim] - _living_box_center[i_dim];
							tmp = FMOD( tmp, _living_box_size[i_dim]);
							pa->get_pos()[i_dim] = tmp - loc_delta + _living_box_center[i_dim];
						}
						else
							pa->get_pos()[i_dim] -= _living_box_size[i_dim];
						if ( S_LIVING_BOX == c_poid::LIVING_BOX_WRAP_RANDOM )
						{
							place_one_on_side( pa, i_dim );
							b_exit = true;
						}
						break;
					case c_poid::LIVING_BOX_BOUNCE:
						pa->get_speed()[i_dim] *= -_living_box_bounce_factor;
						pa->get_speed()[ IMOD(i_dim+1,3) ] *= _living_box_bounce_factor;
						pa->get_speed()[ IMOD(i_dim+2,3) ] *= _living_box_bounce_factor;
						break;
					}
					if( b_exit )
						break;
				}
				else
					pa->get_pos()[i_dim] = tmp;
			}
		}
	}
	else
		add_v3( pa->get_pos(), pos_change );
}

#if	PART_VECTOR
struct part_move_one 
{
	c_bdd_particle* bdd_part;
	part_move_one(c_bdd_particle* target) : bdd_part(target)	{}
	FINLINE void operator()( c_particle* pa ) {	bdd_part->move_one( pa );	}
};
#endif

void c_bdd_particle::move()
{
/*
//todo reimplement but with proba playing on number of changes
//		instead of individual test
UINT32		*p_id;
	pa = part;
	for ( index=nb; index>0; --index )
	{
		//	change id sometimes
		if ( rand_on_time.get_ufloat() < probability_on_time )
			*p_id = rand_on_time.get_uint32_with_seed( pa->id );
		++pa;
	}
*/

//	second pass move the living one
	_field_tmp = _field_out;

	if( _b_turb_factor )
	{
		switch( _s_living_box_type )
		{
		case 0:	LOOPER_BEGIN()	move_one< true , 0 > ( pp ); LOOPER_END()	break;
		case 1:	LOOPER_BEGIN()	move_one< true , 1 > ( pp ); LOOPER_END()	break;
		case 2:	LOOPER_BEGIN()	move_one< true , 2 > ( pp ); LOOPER_END()	break;
		case 3:	LOOPER_BEGIN()	move_one< true , 3 > ( pp ); LOOPER_END()	break;
		case 4:	LOOPER_BEGIN()	move_one< true , 4 > ( pp ); LOOPER_END()	break;
		}
	}
	else
	{
		switch( _s_living_box_type)
		{
		case 0:	LOOPER_BEGIN()	move_one< false , 0 > ( pp ); LOOPER_END()	break;
		case 1:	LOOPER_BEGIN()	move_one< false , 1 > ( pp ); LOOPER_END()	break;
		case 2:	LOOPER_BEGIN()	move_one< false , 2 > ( pp ); LOOPER_END()	break;
		case 3:	LOOPER_BEGIN()	move_one< false , 3 > ( pp ); LOOPER_END()	break;
		case 4:	LOOPER_BEGIN()	move_one< false , 4 > ( pp ); LOOPER_END()	break;
		}
	}
}

void c_bdd_particle::do_close()
{
	_part_close.clear();	//	part_close use to sort : can't really do both at the same time
	_part_close_nb = 0;
	REAL CONST dist_squared = _part_close_dist * _part_close_dist;
	auto cam_pos_pt = c_seedcam::get_cur()->get_position_pt();

	LOOPER_BEGIN()
		REAL	dist = get_dist_squared_if_in_dist_squared_v3r( pp->get_pos_to_draw(), cam_pos_pt, dist_squared );
		if( dist < dist_squared )
		{
			pp->set_sort( SQRT(dist) );
			_part_close.push_back(pp);
		}
	LOOPER_END()
	_part_close_nb = (INT32)_part_close.size();
}

class c_part_less	{	public: FINLINE bool operator() ( CONST c_particle* CONST a, CONST c_particle* CONST b ) CONST NOEXCEPT	{	return a->get_sort() < b->get_sort();	}	};
class c_part_more	{	public: FINLINE bool operator() ( CONST c_particle* CONST a, CONST c_particle* CONST b ) CONST NOEXCEPT	{	return a->get_sort() > b->get_sort();	}	};

//todoopt use template
void	c_bdd_particle::do_sort()
{
	_part_sort.clear();
	if( _looper.get_nb() == 0 )
		return;

	if( _s_render_sort )
	{
		c_seedcam*	cam = c_seedcam::get_cur();
		bool		b_dist_to_cam = _s_render_sort == SORT_DIST || _s_render_sort == SORT_DIST_REVERSE ;
		FP32 CONST * cam_pos_pt;
		if( b_dist_to_cam )
			cam_pos_pt = cam->get_position_pt();

		LOOPER_BEGIN()
			if( _b_feed_bdd_point )
				bdd_point_cur->set_dot_pos( pp->get_id(), pp->get_pos(), 0 );

			if( b_dist_to_cam )
				pp->set_sort( dist_squared_v3r( pp->get_pos_to_draw(), cam_pos_pt ) );
			else
				pp->set_sort( cam->coor_bdd_to_camera_z( pp->get_pos_to_draw() ) );
			_part_sort.push_back(pp);
		LOOPER_END()


		switch( _s_render_sort )
		{
			case SORT_Z_REVERSE:
			case SORT_DIST_REVERSE:
				if( aaa::parallel::is_use() )
#if AAA_VSTOOL() >= 141
					std::sort( std::execution::par_unseq, _part_sort.begin(), _part_sort.end(), c_part_less() );
#else
					PARALLEL_LIB::parallel_sort( _part_sort.begin(), _part_sort.end(), c_part_less() );
#endif				
				else
					//_part_sort.sort( c_part_less() );
					sort( _part_sort.begin(), _part_sort.end(), c_part_less() );
				break;
			case SORT_Z:
			case SORT_DIST:
				if( aaa::parallel::is_use() )
#if AAA_VSTOOL() >= 141
					std::sort( std::execution::par_unseq, _part_sort.begin(), _part_sort.end(), c_part_more() );
#else
					PARALLEL_LIB::parallel_sort( _part_sort.begin(), _part_sort.end(), c_part_more() );
#endif	
				else
					//_part_sort.sort( c_part_more() );
					sort( _part_sort.begin(), _part_sort.end(), c_part_more() );
				break;
		}
		_render_sort_min = _part_sort.front()->get_sort();
		_render_sort_max = _part_sort.back()->get_sort();
	}
	else
	{
		if( _b_feed_bdd_point )
		{
			LOOPER_BEGIN()
				bdd_point_cur->set_dot_pos( pp->get_id(), pp->get_pos(), 0 );	
			LOOPER_END()
		}
	}
}

extern	REAL	g_nif_dist_color_int;
extern	REAL	g_nif_dist_color_ext;

static	UINT8	white4ub[4] = { 255,255,255,255 };

FINLINE	REAL compute_ease( REAL val, REAL ease_in, REAL ease_out )
{
	if( ease_in > 0. && val < ease_in )
		return val / ease_in;
	if( ease_out > 0. && (REAL(1) - val) < ease_out )
		return (REAL(1) - val) / ease_out;
	return 1;
}

FINLINE	void c_bdd_particle::do_color( c_particle* pa )
{
	FP32	col[4];

	if( _b_emission_image_use_ui )
	{		
		if( c_render::get_cur()->is_light() )
			cpy_v4( col, pa->get_color_pt() );
		else
			mul_v4( col, pa->get_color_pt(), c_color::get_cur()->get_color_pt() );
	}
	else
	{
		//todo understand if it was correct to have the same code
		//if( c_render::get_cur()->is_light() )
		//	cpy_v4( col, c_color::get_cur()->get_color_pt() );
		//else
			cpy_v4( col, c_color::get_cur()->get_color_pt() );
	}
	if( _b_visibility )
	{
		REAL	u,v;
		FP32	colorb[4];
		REAL*	f = pa->get_pos_to_draw();
		u	= f[0];
		v	= -f[1];

		u	= (u-img_visibility_u_ori) * img_visibility_u_factor;
		v	= (v-img_visibility_v_ori) * img_visibility_v_factor;

		img_visibility->get_color4r_from_uv_nearest( colorb, u, v, _b_visibility_clamped_ui );
		if( colorb[3] <= 0 )
			return;
		mul_v4( col, colorb );
	}
	if( _b_render_life_color )
	{	
		REAL	age = pa->get_age_rel_at( REAL(_delta_t.get_t()) );
		if ( _b_render_life_color_map )
		{
			FP32	color[4];
			//img_glcolor_from_uv( _img_life_color_map, tmp, pa->color_map_v);
			//todoqq optimize this and under
			_img_life_color_map->get_color4r_from_uv_nearest( color, age, (REAL(1) - pa->get_color_map_v() ), true );
			//if( !c_render::get_cur()->is_light() )
				mul_v4( col, color );
		}
		col[3] *= compute_ease( age, _render_life_color_ease_in, _render_life_color_ease_out );
	}
	if( _sort_color_nor_factor!=0. )
	{		
		REAL nor = pa->get_sort();
		nor = MAX0( nor-_sort_color_begin ) * _sort_color_nor_factor;
		col[3] *= compute_ease( nor, _sort_color_ease_in, _sort_color_ease_out );
	}
//	if( c_render::get_cur()->is_light() )
//	{
//		c_materials::cur->back_cur_set_alpha( col[3] );
//		c_materials::cur->front_cur_set_alpha( col[3] );
//	}
//	else
		GOL::color4v( col );
}

FINLINE	void c_bdd_particle::draw_single_line_one( c_particle* pa, c_seedcam* cam, bool b_color_change, REAL t, REAL t_last, bool b_color_inter )
{
	FP32 f[3];
	pa->cpy_pos_to_draw( f );

	if( _b_deformer )
	{	//todo	this is done quickly the whole deformer update move draw should be change to avoid duplicates
		FP32 tmp[3];
		sub_v3( tmp, f, pa->get_pos_to_draw_last() );
		add_scale_v3( f, tmp, -_render_aperture );
	}
	else
		add_scale_v3( f, pa->get_speed(), -_render_aperture * pa->get_dt() );
	if( _b_scale )
		mul_v3( f, _scale );

	if( cam && !cam->bbox_is_in_world( f, 0 ) )
		return;

	//	tmp = img_get_grey_from_uv( *img_tex, f[i_axe_u]*f_axe_u+emission_map_u*0.5, f[i_axe_v]*f_axe_v+emission_map_v*0.5);
	//	f[map_axe] += tmp * (f[map_axe]-(*start)[map_axe]) * 40.;
	//		GOL::color4( tmp,tmp,tmp, tmp);

	//Colors
	if( b_color_change )
		do_color( pa );
	//		else if ( !map_cur->is_0d() )
	//		{
	//todoqq	GOL::texcoord2( f[i_axe_u]*f_axe_u + .5, f[i_axe_v]*f_axe_v + .5);
	//		}
	GOL::vertex3v( f );

	//	STOP
	pa->cpy_pos_to_draw( f );
	if( _b_scale )
		mul_v3( f, _scale );

	//	tmp = img_get_grey_from_uv( *img_tex, f[i_axe_u]*f_axe_u+emission_map_u*0.5, f[i_axe_v]*f_axe_v+emission_map_v*0.5);
	//	f[map_axe] += tmp * (f[map_axe]-(*start)[map_axe]) *10.e;
	//	(*stop)[map_axe] = f[map_axe];
	//	GOL::color4( tmp,tmp,tmp, tmp);

	//todo regroup with the rest
	if( b_color_inter )
	{
		REAL tmp;
		if( t_last < pa->get_birth() )
			tmp = 0.;
		else
			tmp = pa->get_age_rel_at( t_last );
		//todo	see before 
		if ( _b_render_life_color_map )
		{	//todo opt calculation of tmp with the previous one
			FP32	color[4];
			//todoqq optimize this and under
			_img_life_color_map->get_color4r_from_uv_nearest( color, tmp, REAL(1)-pa->get_color_map_v(), true );
			if( !c_render::get_cur()->is_light() )
				mul_v4( color, c_color::get_cur()->get_color_pt() );
			GOL::color4v( color );
			//img_glcolor_from_uv( _img_life_color_map, tmp, pa->color_map_v);
		}
		else
		{
			if( tmp < _render_life_color_ease_in && _render_life_color_ease_in != 0. )
				tmp /= _render_life_color_ease_in;
			else if( (REAL(1) - tmp) < _render_life_color_ease_out && _render_life_color_ease_out != 0. )
				tmp =  (REAL(1)-tmp) / _render_life_color_ease_out;
			else
				tmp = 1;
			c_color::get_cur()->draw_with_alpha_scaled( tmp );
		}
	}
	GOL::vertex3v(f);
}

FINLINE	void c_bdd_particle::draw_single_point_one( c_particle* pa, c_seedcam* cam, bool b_color_change )
{
	REAL f[3];
	pa->cpy_pos_to_draw( f );
	if( _b_scale )
		mul_v3( f, _scale );

	if( cam && !cam->bbox_is_in_world( f, _render_size_half ) )
		return;

	//Colors
	if( b_color_change )
		do_color( pa );
	c_tex_anim::cur->bind_this( pa->get_id() & 0xff );
	GOL::vertex3v( f );
}

namespace
{
	FP32 CONST *	uv_quad;
	REAL				render_size_min;
	REAL				render_size_factor;

	/*
	void	loc_draw_rect_at( REAL u, REAL v, REAL* pos, INT32 axe )
	{
		REAL	f[3];
		INT32	i_u;
		INT32	i_v;
		axe_build_index_vert( i_u, i_v, axe );
		REAL*	p_u = &f[i_u];
		REAL*	p_v = &f[i_v];
		REAL*	p_axe = &f[ axe ];
		REAL	pos_u = pos[i_u];
		REAL	pos_v = pos[i_v];

		//	fake depth
		REAL tmp = pos[axe];
		tmp = CLAMP_01( (tmp + 4.) * .125 ) * render_size_factor + render_size_min ;
		u *= tmp;
		v *= tmp;

		if( axe == 0 )
			u *= -.5;
		else
			u *= .5;
		v *= .5;
		*p_axe = pos[axe];

		if ( c_layer::get_cur()->is_need_uv() )
		{
			UVx4 CONST &	uv = *uv_quad;

			GOL::texcoord2v( uv[0] );
			*p_u = pos_u-u;
			*p_v = pos_v+v;
			GOL::vertex3v( f );

			GOL::texcoord2v( uv[1] );
			*p_v = pos_v-v;
			GOL::vertex3v( f );

			GOL::texcoord2v( uv[3] );
			*p_u = pos_u+u;
			GOL::vertex3v( f );

			GOL::texcoord2v( uv[2] );
			*p_v = pos_v+v;
			GOL::vertex3v( f );
		}
		else
		{
			*p_u = pos_u-u;
			*p_v = pos_v+v;
			GOL::vertex3v( f );

			*p_v = pos_v-v;
			GOL::vertex3v( f );

			*p_u = pos_u+u;
			*p_v = pos_v+v;
			GOL::vertex3v( f );

			*p_v = pos_v-v;
			GOL::vertex3v( f );
		}
	}
	*/

	void	loc_draw_rect_at_rot_z( REAL u, REAL v, REAL *pos, REAL angle )
	{
		REAL	f[3];
		REAL	cu;
		REAL	su;

		GET_SIN_COS_TURN( su, cu, angle );

		REAL	xv = -v*su;
		REAL	yv = v*cu;

		cu *= u;
		su *= u;
		//u *= .5;
		//v *= .5;

		f[2] = pos[2];
		if ( c_layer::get_cur()->is_need_uv() )
		{
			FP32 CONST * uv = uv_quad;

			GOL::texcoord2v( uv );
			f[0] = pos[0]-cu-xv;
			f[1] = pos[1]-su-yv;
			GOL::vertex3v( f );

			GOL::texcoord2v( uv+2 );
			f[0] = pos[0]+cu-xv;
			f[1] = pos[1]+su-yv;
			GOL::vertex3v( f );

			GOL::texcoord2v( uv+6 );
			f[0] = pos[0]+cu+xv;
			f[1] = pos[1]+su+yv;
			GOL::vertex3v( f );

			GOL::texcoord2v( uv+4 );
			f[0] = pos[0]-cu+xv;
			f[1] = pos[1]-su+yv;
			GOL::vertex3v( f );
		}
		else
		{
			f[0] = pos[0]-cu-xv;
			f[1] = pos[1]-su-yv;
			GOL::vertex3v( f );

			f[0] = pos[0]+cu-xv;
			f[1] = pos[1]+su-yv;
			GOL::vertex3v( f );

			f[0] = pos[0]+cu+xv;
			f[1] = pos[1]+su+yv;
			GOL::vertex3v( f );

			f[0] = pos[0]-cu+xv;
			f[1] = pos[1]-su+yv;
			GOL::vertex3v( f );
		}
	}
}

FINLINE	void c_bdd_particle::draw_single_quad_one( c_particle* pa, c_seedcam* cam, bool b_color_change, REAL*& p_field_size, REAL t, REAL t_last, bool b_rot_and_scale )
{
	REAL	size[3];
	REAL*	size_pt;

	FP32 f[3];
	pa->cpy_pos_to_draw( f );
	if( _b_scale )
		mul_v3( f, _scale );
	if( cam && !cam->bbox_is_in_world( f, _render_size_half ) )
		return;

	//Colors
	if( b_color_change )	
		do_color( pa );


	if( _b_render_size_life )
	{
		size_pt = size;
		FP32 tmp = pa->get_age_rel_at( REAL(_delta_t.get_t()) );
		bias_with_factor( tmp, _render_size_life_bias_factor );
		interpolate_v3( size, _render_size, _render_size_death, tmp );
		if( p_field_size )
		{
			add_mul_v3r( size, p_field_size, _field_size );
			p_field_size += 3;
		}
	}
	else if( p_field_size )
	{
		size_pt = size;
		add_mul_v3r( size, _render_size, p_field_size, _field_size );
		p_field_size += 3;
	}
	else
		size_pt = _render_size;

	INT32 id = pa->get_id() & 0xff;
	c_tex_anim::cur->bind_this( id );

/*
	if( b_rot_and_scale )
	{
		REAL	rot;
		GOL::matrix::push();
		//	Position	
		GOL::translatev( f);

		rot = t*rotate_freq + (pa->get_id()&0xffff) * rotate_id_factor + pa->get_pos_to_draw()[rotate_space_axe] * rotate_space_freq;
		if( rot != 0. )
			GOL::rotatev( rot, rotate_direction );
		if( _b_multiple_use_list )
		{
			//todo still buggy
			//todo create other mode
			if( c_multiple::cur->is_align_billboard() )
				billboard::do_x( f);
			GOL::scalev( size_pt );
			GOL::call_list( get_render_list_multiple_name());
		}
		else
		{
			c_multiple::cur->set_index( pa->get_id() );
			c_multiple::cur->align_then_draw( zero_v4fp32, size_pt );
		}
		GOL::matrix::pop();
	}
	else
	{
		c_multiple::cur->set_index( pa->get_id() );
		if(  c_multiple::cur->is_align_normal() )
		{
			REAL	nor[3];
			if( _b_deformer )
			{	//todo	this is done quickly the whole defoemer update move draw should be change to avoid duplicates
				sub_v3( nor, f, pa->get_pos_to_draw_last() );
			}
			else
				normalize_v3r( nor, pa->get_speed() );
			c_multiple::cur->draw_one_at_nor( f, size_pt, nor );
		}
		else
		{
#if 1
			c_multiple::cur->align_then_draw( f, size_pt );
			//draw_rect_at( .1, .1, f, 0 );
#else
			GOL::matrix::push();
			GOL::translatev( f );
			draw_rect( .1,.1, 0 );	
			GOL::matrix::pop();
#endif
		}
	}
*/
//	loc_draw_rect_at( *size_pt * (id>>4)*(1./16.), *(size_pt+1) * (id&0xf)*(1./16.), f, 2 );
	REAL tmp = f[2];
	tmp = CLAMP_01( (tmp + REAL(4)) * REAL(.125) );
	tmp = tmp * tmp;
	tmp  = tmp * render_size_factor + render_size_min ;
	loc_draw_rect_at_rot_z( *size_pt * tmp * ((id>>4)*REAL(1./16.)+REAL(2./16.)), *(size_pt+1) * tmp, f, (id&0xf)*REAL(.5/16.) );
}

void c_bdd_particle::draw_single()
{

	bool	b_color_change = _b_emission_image_use_ui || _b_render_life_color;
	REAL	t = REAL(_delta_t.get_t());
	
	//	GOL::point_size( rendering_cur->get_point_size() );

	//REAL		tmp;
	c_seedcam*	cam = (_b_clipping_ui && gb_allow_clipping) ? c_seedcam::get_cur() : nullptr;
	switch( _s_render_type )
	{
	case RENDER_LINE:
		{
			REAL	t_last = REAL(_delta_t.get_t_last());
			bool	b_color_inter = b_render_map_interp && _b_render_life_color && !_b_emission_image_use_ui;
			GOL::push_att();
				GOL::push_line_smooth( _b_render_line_smooth
					//		&& rendering_cur->get_tex_use()!=2		//was bug intergraph Gl
					);

				GOL::begin( GL_LINES );
					LOOPER_BEGIN()
						draw_single_line_one( pp, cam, b_color_change, t, t_last, b_color_inter );
					LOOPER_END()
				GOL::end();
			GOL::pop_att();
		}	//end case
		break;
	case RENDER_POINT:
		{
			//REAL point_size_max = c_render::get_cur()->get_point_size();
			//REAL point_size_min = point_size_max*.1;
			//REAL point_size_factor = (point_size_max - point_size_min) / 256.;
			//point_size_min /= 256.;

			GOL::begin( GL_POINTS );
				LOOPER_BEGIN()
					draw_single_point_one( pp, cam, b_color_change );
				LOOPER_END()
			GOL::end();
		}	//end case
		break;
	case RENDER_QUAD:
		{
			REAL*	p_field_size;
			REAL	t_last = REAL(_delta_t.get_t_last());

			if( b_field && b_field_size )
				p_field_size = _field_out;
			else
				p_field_size = nullptr;

			//b_rot_and_scale = (rotate_freq!=0.) || (rotate_space_freq!=0.) || (rotate_id_factor!=0.) || c_multiple::cur->is_align_billboard();
			bool b_rot_and_scale = (rotate_freq!=0.) || (rotate_space_freq!=0.) || (rotate_id_factor!=0.);

			uv_quad = c_map::get_quad_uv();
			render_size_min = _render_size_min;
			render_size_factor = REAL(1) - _render_size_min;
			GOL::begin( GL_QUADS );	//	c_render::get_cur()->get_draw_primitive() );
				// bug intergraph need to be after glbegin
				if( c_layer::get_cur()->is_normal_draw() )
					GOL::normal3v( unit_z_v4fp32 );

				if( _s_render_sort )
				{
					for( auto const & p_particle : _part_sort )
						draw_single_quad_one( p_particle, cam, b_color_change, p_field_size, t, t_last,	b_rot_and_scale );
				}
				else
				{
					LOOPER_BEGIN()
						draw_single_quad_one( pp, cam, b_color_change, p_field_size, t, t_last,	b_rot_and_scale );
					LOOPER_END()
				}
			GOL::end();
		}
	}	//end switch
}

//todo move to template
FINLINE	void c_bdd_particle::draw_multiple_one( c_particle* pa, c_seedcam* cam, bool b_color_change, REAL*& p_field_size, REAL t, REAL t_last, bool b_rot_and_scale  )
{
	FP32	f[3];
	REAL	tmp;

	REAL	size[3];
	REAL*	size_pt;

	pa->cpy_pos_to_draw( f );
	if( _b_scale )
		mul_v3( f, _scale );

	if( cam && !cam->bbox_is_in_world( f, _render_size_half ) )
		return;

	//Colors
	if( b_color_change )
		do_color( pa );

	if( _b_render_size_life )
	{
		size_pt = size;
		tmp = pa->get_age_rel_at( REAL(_delta_t.get_t()) );
		bias_with_factor( tmp, _render_size_life_bias_factor );
		interpolate_v3( size, _render_size, _render_size_death, tmp );
		if( p_field_size )
		{
			add_mul_v3r( size, p_field_size, _field_size );
			p_field_size += 3;
		}
	}
	else if( p_field_size )
	{
		size_pt = size;
		add_mul_v3r( size, _render_size, p_field_size, _field_size );
		p_field_size += 3;
	}
	else
		size_pt = _render_size;

	c_tex_anim::cur->bind_this( pa->get_id() & 0xff );

	if( b_rot_and_scale )
	{
		REAL	rot;
		GOL::matrix::push();
			//	Position	
			GOL::matrix::translate3v( f);

			rot = t*rotate_freq + (pa->get_id()&0xffff) * rotate_id_factor + pa->get_pos_to_draw()[rotate_space_axe] * rotate_space_freq;
			if( rot != 0. )
				GOL::matrix::rotatev( rot, rotate_direction );
			if( _b_multiple_use_list )
			{
				//todo still buggy
				//todo create other mode
				if( c_multiple::cur->is_align_billboard() )
						billboard::do_x( f);
				GOL::matrix::scale3v( size_pt );
				GOL::call_list( get_render_list_multiple_name());
			}
			else
			{
				c_multiple::cur->set_index( pa->get_id() );
				c_multiple::cur->align_then_draw( zero_v4fp32, size_pt );
			}
		GOL::matrix::pop();
	}
	else
	{
		if( _b_multiple_use_list )
		{
			GOL::matrix::push();
				//	Position	
				GOL::matrix::translate3v( f);
				GOL::matrix::scale3v( size_pt );
				GOL::call_list( get_render_list_multiple_name());
			GOL::matrix::pop();
		}
		else
		{
			c_multiple::cur->set_index( pa->get_id() );
			if(  c_multiple::cur->is_align_normal() )
			{
				REAL	nor[3];
				if( _b_deformer )
				{	//todo	this is done quickly the whole defoemer update move draw should be change to avoid duplicates
					sub_v3( nor, f, pa->get_pos_to_draw_last() );
				}
				else
					normalize_v3r( nor, pa->get_speed() );
				c_multiple::cur->draw_one_at_nor( f, size_pt, nor );
			}
			else
			{
#if 1
				c_multiple::cur->align_then_draw( f, size_pt );
				//draw_rect_at( .1, .1, f, 0 );
#else
				GOL::matrix::push();
					GOL::matrix::translatev( f );
					draw_rect( .1,.1, 0 );	
				GOL::matrix::pop();
#endif
			}
		}
	}
}

void c_bdd_particle::draw_multiple()
{ 
#if	APP_SPECIAL_NIB()
	if( s_special==2 && b_nib )
	{
		nib_draw();
		return;
	}
#endif
	REAL*		p_field_size;
	REAL		t = REAL(_delta_t.get_t());
	REAL		t_last = REAL(_delta_t.get_t_last());

	bool		b_color_change = _b_emission_image_use_ui || _b_render_life_color || _b_visibility || (_sort_color_nor_factor!=0.) ;
	bool		b_clipping_loc = _b_clipping_ui && gb_allow_clipping;
	c_seedcam*	cam = b_clipping_loc ? c_seedcam::get_cur() : nullptr ;

	bool b_rot_and_scale;

	if( b_field && b_field_size )
		p_field_size = _field_out;
	else
		p_field_size = nullptr;

	b_rot_and_scale = (rotate_freq!=0.) || (rotate_space_freq!=0.) || (rotate_id_factor!=0.) || c_multiple::cur->is_align_billboard();

	if( _s_render_sort )
	{
		for( auto const & p_particle : _part_sort )
			draw_multiple_one( p_particle, cam, b_color_change, p_field_size, t, t_last,	b_rot_and_scale );
	}
	else
	{
		LOOPER_BEGIN()
			draw_multiple_one( pp, cam, b_color_change, p_field_size, t, t_last,	b_rot_and_scale );
		LOOPER_END()
	}
}
	
#if	APP_SPECIAL_NIB()

extern	REAL	g_part_dist_max;
extern	REAL	g_part_dist_alpha;
extern	REAL	g_part_alpha_base;

void c_bdd_particle::nib_move()
{
	c_particle*		pa;
	LOOPER_BEGIN()
		pp->get_pos()[1] += pp->get_speed()[1] * pp->get_dt();
	LOOPER_END()
}

void c_bdd_particle::nib_draw()
{
	FP32		f[3];
	//REAL		nor[3];
	REAL		pos[3];
	REAL		h;
	REAL		rot_y;
	REAL		radius;

	c_particle*		pa;

	REAL	origin_over = 1. / _emission_origin[1];
	REAL*	uv;
	REAL	point[4][3];

	REAL	dist_part_max_squared	= g_part_dist_max * g_part_dist_max;
	REAL	dist_alpha				= g_part_dist_alpha;
	REAL	dist_alpha_squared		= dist_alpha * dist_alpha;
	REAL	alpha_base				= g_part_alpha_base * REAL_NEARLY_256 * c_color::cur->get_color_alpha();
	REAL	alpha_factor			= alpha_base / (g_part_dist_max-dist_alpha);

	REAL	part_close_dist_squared;
	REAL	cam_pos[3];
	UINT8	alpha;

//	GOL::color_material( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
//	GOL::enable_color_material();

	uv = c_map::get_cur()->get_quad();
			
	if( GOL::b_draw_avoid_vertex_use )
	{
		GOL::unbind_vao_secu();
		GOL::enable_client_state_vertex();
		GOL::set_pointer_vertex3( point );

		GOL::enable_client_state_texcoor();
		GOL::set_pointer_texcoor( 2, GL_REAL, uv );
	}

	c_seedcam*	cam = c_seedcam::get_cur();
	if( _b_part_close )
	{
		_part_close.clear();
		part_close_dist_squared = _part_close_dist * _part_close_dist;
		cam->get_coor( cam_pos );
	}
	else
		part_close_dist_squared = .0;

	GOL::set_texture_2D();

	LOOPER_BEGIN()
		pp->cpy_pos_to_draw( f );

		radius = dist_squared_v3r( f, cam_pos );
		pp->set_sort( radius );

		if( radius < part_close_dist_squared )
			_part_close.push_back(pp);
		if( pp->get_sort() >= dist_part_max_squared )
			continue;			

		h = 1. - pp->get_pos_to_draw()[1]*origin_over;
		radius = _render_size[1] * .5 * CLAMP( (.25-h)*10., 1., 2.5 );
/*
		nor[0] = cos;
		nor[1] = 1.;
		nor[2] = sin;
		GOL::normal3v( f);
*/
		if( h >.3)	// at the top with texture
		{
			REAL	cos;
			REAL	sin;
			REAL	tmp;
			
			//CLAMP
			if( !cam->bbox_is_in_world( f, radius ) )
				continue;

			//DO COLOR
			if( pp->get_sort() < dist_alpha_squared )
				alpha = alpha_base;
			else
				alpha = alpha_base-((SQRT(pp->get_sort())-dist_alpha)*alpha_factor);
			//todoq	store in part
			if( h>.98)
				{
				if( h >= 1. )
					white4ub[3] = 0.;	//transparent
				else
					white4ub[3] = alpha*(1.-h)*50.;	//dissapear
				}
			else
				white4ub[3] = alpha;
			GOL::color4ubv( white4ub);

			//TEXTURE
			c_tex_anim::cur->bind_this( pp->get_id() );
		
			//ROTATION
			rot_y = (pp->get_id()&0xffff) * rotate_id_factor * .0025;
			GET_SIN_COS_TURN( sin, cos, rot_y );

			tmp = _render_size[0] * .5;
			cos *= tmp;
			sin *= tmp;
	
			if( GOL::b_use_draw_arrays)
			{
				REAL*	p = &point[0][0];
				*p = f[0]-sin;
				*++p = f[1]+radius;
				*++p = f[2]+cos;

				*++p = f[0]-sin;
				*++p = f[1]-radius;
				*++p = f[2]+cos;
				
				*++p = f[0]+sin;
				*++p = f[1]-radius;
				*++p = f[2]-cos;
				
				*++p = f[0]+sin;
				*++p = f[1]+radius;
				*++p = f[2]-cos;

				GOL::draw_arrays( GL_QUADS, 4 );
			}
			else
			{
				GOL::begin( GL_QUADS );
					GOL::texcoord2v( uv );
					pos[1] = f[1]+radius;
					pos[0] = f[0]-sin;
					pos[2] = f[2]+cos;
					GOL::vertex3v( pos );

					GOL::texcoord2v( uv+2 );
					pos[1] = f[1]-radius;
					GOL::vertex3v( pos );
					
					GOL::texcoord2v( uv+4 );
					pos[0] = f[0]+sin;
					pos[2] = f[2]-cos;
					GOL::vertex3v( pos );
					
					GOL::texcoord2v( uv+6 );
					pos[1] = f[1]+radius;
					GOL::vertex3v( pos );
				GOL::end();
			}
		}
		else if( h > .2 )
		{
			REAL	cos;
			REAL	sin;
			
			//CLAMP
			if( !cam->bbox_is_in_world( f, radius ) )
				continue;

			//DO COLOR
			if( pp->get_sort() < dist_alpha_squared )
				alpha = alpha_base;
			else
				alpha = alpha_base - ( (SQRT(pp->get_sort())-dist_alpha)*alpha_factor );
			alpha *= 1./255.;	//hack
			pp->set_color_alpha( alpha );
			GOL::color4( pp->get_color() );

			//TEXTURE
			tex_2d_bind( c_tex_anim::cur->get_start()-1 );

			//ROTATION
			rot_y = (pp->get_id()&0xffff)*rotate_id_factor*.0025;
			GET_SIN_COS_TURN( sin, cos, rot_y );
			cos *= radius;
			sin *= radius;

			if( GOL::b_draw_avoid_vertex_use )
			{
				REAL*	p = &point[0][0];
				*p = f[0]-sin;
				*++p = f[1]+radius;
				*++p = f[2]+cos;

				*++p = f[0]-sin;
				*++p = f[1]-radius;
				*++p = f[2]+cos;
				
				*++p = f[0]+sin;
				*++p = f[1]-radius;
				*++p = f[2]-cos;
				
				*++p = f[0]+sin;
				*++p = f[1]+radius;
				*++p = f[2]-cos;

				GOL::draw_arrays( GL_QUADS, 4 );
			}
			else
			{
				GOL::begin( GL_QUADS );
					GOL::texcoord2v( uv );
					pos[1] = f[1]+radius;
					pos[0] = f[0]-sin;
					pos[2] = f[2]+cos;
					GOL::vertex3v( pos );

					GOL::texcoord2v( uv+2 );
					pos[1] = f[1]-radius;
					GOL::vertex3v( pos );
					
					GOL::texcoord2v( uv+4 );
					pos[0] = f[0]+sin;
					pos[2] = f[2]-cos;
					GOL::vertex3v( pos );
					
					GOL::texcoord2v( uv+6 );
					pos[1] = f[1]+radius;
					GOL::vertex3v( pos );
				GOL::end();
			}
		}
		else
		{
			REAL	c2;
			REAL	s2;
			REAL*	p = &point[0][0];
			REAL	tmp;
			REAL	cos;
			REAL	sin;

			h /= .2;

			tmp = 1.-h;
			tmp *= tmp;
			tmp *= tmp;
			radius = (tmp*8+1.) * radius;
			
			//CLAMP
			if( !cam->bbox_is_in_world( f, radius*1.5) )
				continue;

			//DO COLOR
			if( pp->get_sort() < dist_alpha_squared )
				alpha = alpha_base;
			else
				alpha = alpha_base - ( (SQRT(pp->get_sort())-dist_alpha)*alpha_factor );
			if( h<.4)
				alpha = alpha*h*2.5;
			alpha *= 1./255.;	//hack
			pp->set_color_alpha( alpha );
			GOL::color4( pp->get_color() );
		
			//TEXTURE
			tex_2d_bind( c_tex_anim::cur->get_start()-1 );

			//ROTATION
			rot_y = (pp->get_id()&0xffff)*rotate_id_factor*.0025;
			rot_y = interpolate( F_FLOOR( rot_y * 4. +.5) * .25, rot_y, h*h );
			GET_SIN_COS_TURN( sin, cos, rot_y );
			cos *= radius;
			sin *= radius;

			GET_SIN_COS_TURN( s2, c2, h*.25 );

			*p = f[0]	-sin-c2*cos;
			*++p = f[1]	+radius*s2;
			*++p = f[2]	+cos-c2*sin;

			*++p = f[0]	-sin+c2*cos;
			*++p = f[1]	-radius*s2;
			*++p = f[2]	+cos+c2*sin;
			
			*++p = f[0]	+sin+c2*cos;	
			*++p = f[1]	-radius*s2;
			*++p = f[2]	-cos+c2*sin;
			
			*++p = f[0]	+sin-c2*cos;
			*++p = f[1]	+radius*s2;
			*++p = f[2]	-cos-c2*sin;
b_draw_avoid_vertex_use
			if( GOL::b_draw_avoid_vertex_use)
				GOL::draw_arrays( GL_QUADS, 4 );
			else
			{
				GOL::begin( GL_QUADS );
					GOL::texcoord2v( uv );
					GOL::vertex3v( &point[0][0] );
					GOL::texcoord2v( uv+2 );
					GOL::vertex3v( &point[1][0] );			
					GOL::texcoord2v( uv+4 );
					GOL::vertex3v( &point[2][0] );		
					GOL::texcoord2v( uv+6 );
					GOL::vertex3v( &point[3][0] );
				GOL::end();
			} 
		}
	LOOPER_END()
	_part_close_nb = _part_close.size();

//	GOL::disable_color_material();
//	material_reset();

	if( GOL::b_draw_avoid_vertex_use )
	{
		GOL::disable_client_state_vertex();
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
	}
}

#endif
