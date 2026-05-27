#include "bdd_mocap.h"
#include "draw/render.h"
#include "draw/model.h"
#include "strnum.h"
#include "time/aaa_time.h"
#include "draw/colorrnd.h"
#include "draw/tex_anim.h"
#include "file/file_csv.h"
#include "file/file_hrc.h"
#include "file/aaa_dir.h"
#include "ui/alphabet.h"
#include "math/rand.h"
#include "stream_util.h"
#include "infrastructure/layer/layers.h"
#include "infrastructure/layer/module.h"
#include "obj_ui/bdd/bdd_old/special.h"
#include "obj_ui/tracker/aaa_gypsy.h"
#include "draw/axe.h"
#include "math/billboard.h"
#include "infrastructure/bind/bind.h"
#include "obj_ui/com/net.h"
#include "file/asc_parser.h"
#include "gol/gol_color.h"
#include "gol/gol_draw.h"
#include "gol/gol_light.h"
#include "obj_ui/com/net_blk.h"
#include "file/dir_pool.h"
#ifndef AAA_MOCAP_FORMAT_BVH_H
#	include "mocap/mocap_format_bvh.h"
#endif

#ifndef AAA_MOCAP_ANIMATOR_H
#	include "mocap/mocap_Animator.h"
#endif

//
//	SEQ
//
c_seq_mocap::c_seq_mocap()
:_node_nb(0)
,_offset_abs(0)
,_offset_rel(0)
,_start_file(0)
,_stop_file(0)
,_frame_nb_rel(0)
,_frame_nb_abs(0)
,_b_loop(true)
,_frame_in(0)
,_frame_out(0)
,_frame_nb_to_play(0)
{
}
void	c_seq_mocap::update()
{
	_frame_in	=	MIN(	_frame_in,	(_frame_nb_rel-1) );
	_frame_out	=	MIN(	_frame_out,	(_frame_nb_rel-_frame_in-1) );
	INT32 nb = _frame_nb_rel - _frame_in - _frame_out;
	_frame_nb_to_play = nb;
}

//
//	SEGMENT
//
static	INT32	segment_bvh_def[24][2] =
{
	1,	2,		//	0
	2,	8,
	8,	9,
	9,	22,
	22,	24,
	1,	3,		//	1
	3,	10,
	10,	11,
	11,	23,
	23,	25,
	1,	4,		//	10
	4,	5,
	5,	6,
	6,	7,
	7,	12,	
	12,	13,		//	15
	13,	14,
	14,	20,
	7,	15,
	15,	16,
	16,	17,		//	20
	17,	21,
	7,	18,
	18,	19,		//	23
};


static	INT32	segment_bvh_def_assimp[24][2] =
{
	1,	2,		//	0
	2,	3,
	3,	4,
	4,	5,
	1,	6,
	6,	7,		//	1
	7,	8,
	8,	9,
	1,	10,
	10,	11,
	11,	12,		//	10
	12,	13,
	13,	14,
	14,	15,
	11,	17,	
	17,	18,		//	15
	18,	19,
	19,	20,
	11,	22,
	22,	23,
	16,	17,		//	20
	17,	21,
	7,	18,
	18,	19,		//	23
};

static	INT32	segment_kinect1_def[19][2] =
{				//	this the kinect skeleton, 10 for the upper part
	3,	4,
	3,	5,		//			4
	5,	6,		//			|
	6,	7,		//		9---3---5
	7,	8,		//		|	|	|
	3,	9,		//	   10	|	6
	9,	10,		//		|	|	|
	10,	11,		//	12-11	2	7-8
	11,	12,		//			|
	1,	2,		//	   17---1---13
	2,	3,		//		|		|
	1,	13,		//		|		|
	13,	14,		//	   18	    14
	14,	15,		//		|		|
	15,	16,		//		|		|
	1,	17,		// 20--19	    15--16
	17,	18,
	18,	19,
	19,	20,
};

static	INT32	segment_kinect2_def[24][2] =
{				//	this the kinect v2 skeleton, 15 for the upper part
	3,	4,
	21,	3,		//			   4
	21,	5,		//			   |
	5,	6,		//			   3
	6,	7,		//		   9---21--5
	7,	8,		//		   |   |   |
	8,  22,		//		  10   |   6
	7,  23,		//		   |   |   |
	21,	9,		//	24-12-11   2   7-8-22
	9,	10,		//		  25   |   23
	10,	11,		//             |
	11,	12,		//		  17---1---13
	12, 24,		//		   |	   |
	11, 25,		//		   |	   |
	1,	2,		//		  18	   14
	2,	3,		//		   |	   |
	1,	13,		//		   |	   |
	13,	14,		//	  20--19	   15--16
	14,	15,
	15,	16,
	1,	17,
	17,	18,
	18,	19,
	19,	20,
};
c_mocap_segment::c_mocap_segment()
	:_nb(0)
{
}

void	c_mocap_segment::get_segment_point_id( INT32 seg_id, INT32& id_a, INT32& id_b )
{
	if( is_id_valid( seg_id ) )
	{
		seg_id = seg_id-1;
		id_a = _point_index[seg_id][0];
		id_b = _point_index[seg_id][1];
	}
	else
	{
		id_a = 0;
		id_b = 0;
	}
}
void	c_mocap_segment::init_point_index_from_table( INT32 nb, INT32* table )
{
	INT32	seg_byte_size = sizeof(INT32) * 2;
	nb = MIN( nb, SEGMENT_NB_MAX );
	MEMCPY( _point_index, table, nb * seg_byte_size, __FUNCTION__ );
	_nb = nb;
}
void	c_mocap_segment::init_kinect1()	{	init_point_index_from_table( 19, segment_kinect1_def[0] );	}
void	c_mocap_segment::init_kinect2()	{	init_point_index_from_table( 24, segment_kinect2_def[0] );	}
void	c_mocap_segment::init_bvh_def()	{	init_point_index_from_table( 24, segment_bvh_def[0] );	}

void	c_mocap_segment::read_point_from_file_csv( C_PCHAR_C filename )
{
	if( c_file::is_exist( filename ) )
		_nb = file_csv::read_int32_grid( filename, _point_index[0], 2, SEGMENT_NB_MAX ) / 2;
	else
		_nb = 0;
}

void	c_mocap_segment::normalize_weight()
{
	//	get the max
	REAL max = get_max_of_array( _weight, _nb );
	max = OVER_ONE_AS_REAL( max );

	//	normalize
	REAL* pw = _weight-1;
	for( INT32 i = _nb; i > 0; --i )
		*++pw *= max;
}

static	c_rand_lin		rand_segment_place;

//todo	init segment_weight and check update strategy
INT32	c_mocap_segment::get_id_rnd( INT32 id_begin, INT32 id_end, bool b_only_hands_for_other )
{
	INT32	id_seg;
	INT32	i = 10;

	if( b_only_hands_for_other )
	{
		if( rand_segment_place.get_fp32_01() > .5 )
			id_seg = 5;
		else
			id_seg = 9;
	}
	else
	{
		REAL mul = id_end - id_begin + REAL(.99999);
		//	find segment
		do
			id_seg = id_begin + INT32( rand_segment_place.get_fp32_max(mul) );
		while( _weight[id_seg-1] < rand_segment_place.get_fp32_01() && i-->0 );
	}

	return id_seg;
}

//
//	MOCAP
//
//todo	make sure that load gyp and bhv interactively from nothing works
FACTORY_CREATE_PROP_V1( c_bdd_mocap, bdd_mocap, Motion Capture, mocap, sub_menu="Point"; );

c_instance_by_channel< c_bdd_mocap, c_bdd_mocap::MOCAP_CHANNEL_NB_MAX >	c_bdd_mocap::inst_by_channel;

c_bdd_mocap*	c_bdd_mocap::def = nullptr;
c_bdd_mocap*	c_bdd_mocap::cur = nullptr;
c_bdd_mocap*	c_bdd_mocap::ui = nullptr;

static	C_PCHAR_C	str_origin[4] =
{
	"Internal",
	"3DMax",
	"Softimage",
	"Gypsy BVH",
};

namespace	n_bdd_mocap
{
	CONSTEXPR INT32	BASE_PARAM_NB		=	c_bdd::GEO_PARAM_NB + 2 + 1;
	CONSTEXPR INT32	INFO_PARAM_NB		=	7;
	CONSTEXPR INT32	PLAY_PARAM_NB		=	14;
	CONSTEXPR INT32	ORIGIN_PARAM_NB		=	8;
	CONSTEXPR INT32	CAN_PARAM_NB		=	3;
	CONSTEXPR INT32	USE_PARAM_NB		=	5;
	CONSTEXPR INT32	TRA_SCALE_PARAM_NB	=	4;
	CONSTEXPR INT32	RENDER_PARAM_NB		=	15;
	CONSTEXPR INT32	NET_PARAM_NB		=	2;
	CONSTEXPR INT32	GROUP_PARAM_NB		=	8;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	INFO_PARAM_NB
									+	PLAY_PARAM_NB
									+	ORIGIN_PARAM_NB
									+	CAN_PARAM_NB
									+	USE_PARAM_NB
									+	TRA_SCALE_PARAM_NB
									+	RENDER_PARAM_NB
									+	NET_PARAM_NB
									+	GROUP_PARAM_NB;

	CONSTEXPR INT32	INDEX_SEQ_NB_PARAM		=	BASE_PARAM_NB + 1 + INFO_PARAM_NB + 1 + 5 -2;
	CONSTEXPR INT32	INDEX_FRAME_CUR_PARAM	=	INDEX_SEQ_NB_PARAM + 7;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_BOOL_OFF(	use_gypsy	)

		PARAM_DEF_GROUP(		info,		INFO_PARAM_NB )
			PARAM_DEF_INT32(		channel_id,					2, 1,		1, c_bdd_mocap::MOCAP_CHANNEL_NB_MAX )
			PARAM_DEF_INT32_LOCKED(	nb_sequence					)
	//no param_lock but be careful see param_init_pt
			PARAM_DEF_INT32(		nb_node,					4, 1,		1, PARAM_MAX_INT32 )
			PARAM_DEF_INT32(		nb_frame,					100, 256,	0, PARAM_MAX_INT32 )
			PARAM_DEF_SYMBO_LOCKED(	from,						1, 0,		PT_NB_STR(str_origin), str_origin	)
			PARAM_DEF_FILENAME(		mocap_filename,				aaa::file::TYPE_IO_BDD_MOCAP_DATA, 0		)
			PARAM_DEF_FILENAME(		actor_filename,				aaa::file::TYPE_IO_BDD_MOCAP_ACTOR, 0	)

		PARAM_DEF_BOOL_OFF( clear_data_trig )

		PARAM_DEF_GROUP( play, PLAY_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		playing						)
			PARAM_DEF_BOOL_OFF(		restart_trig				)
			PARAM_DEF_BOOL_OFF(		live						)
			PARAM_DEF_BOOL_OFF(		reference_trig				)
			PARAM_DEF_INT32(		sequence_cur,				1, 0,		0, 128				)
			PARAM_DEF_INT32(		frame_in,					100, 0,		0, PARAM_MAX_INT32	)
			PARAM_DEF_INT32(		frame_out,					100, 0,		0, PARAM_MAX_INT32	)
			PARAM_DEF_REAL_LOCKED(	frame_by_sec_source			)
			PARAM_DEF_REAL_ONE(		time_factor					)
			PARAM_DEF_REAL_LOCKED(	frame_by_sec				)
			PARAM_DEF_REAL_LOCKED(	time_len					)
			PARAM_DEF_REAL_POS_ONE(	frame_cur					)
			PARAM_DEF_REAL_ZERO(	phase						)
			PARAM_DEF_BOOL_ON(		loop						)

		PARAM_DEF_GROUP_CLOSED( Origin, ORIGIN_PARAM_NB )
			PARAM_DEF_POINT_XYZ(		origin					)
			PARAM_DEF_SCALE_XYZF_ZERO(	origin_on_node			)
			PARAM_DEF_INT32(			origin_on_node_id,		2, 1,		1, PARAM_MAX_INT32	)

		PARAM_DEF_GROUP_CLOSED( Can, CAN_PARAM_NB )
			PARAM_DEF_BOOL_LOCKED(	can_rot						)
			PARAM_DEF_BOOL_LOCKED(	can_tra						)
			PARAM_DEF_BOOL_LOCKED(	can_sca						)

		PARAM_DEF_GROUP( Use, USE_PARAM_NB )
			PARAM_DEF_BOOL_ON(		use_rot						)
			PARAM_DEF_BOOL_ON(		use_tra						)
			PARAM_DEF_BOOL_ON(		use_sca						)
			PARAM_DEF_BOOL_ON(		interpolate					)
			PARAM_DEF_BOOL_ON(		interpolate_rotation		)
		
		PARAM_DEF_GROUP_CLOSED( translation scale, TRA_SCALE_PARAM_NB )
			PARAM_DEF_SCALE_XYZF(	tra_scale					)

	//todoqq	switch to render instead of draw
		PARAM_DEF_GROUP( render, RENDER_PARAM_NB )
			PARAM_DEF_INT32(		draw_node_start,			2, 1,		1, PARAM_MAX_INT32 )
			PARAM_DEF_INT32(		draw_node_end,				2, 1,		1, PARAM_MAX_INT32	)
			PARAM_DEF_BOOL_OFF(		draw_origins				)
			PARAM_DEF_REAL_ONE(		size_origins				)
			PARAM_DEF_BOOL_OFF(		draw_segment				)
			PARAM_DEF_INT32(		draw_segment_start,			2, 1,		1, c_mocap_segment::SEGMENT_NB_MAX )
			PARAM_DEF_INT32(		draw_segment_end,			2, 1,		1, c_mocap_segment::SEGMENT_NB_MAX )
			PARAM_DEF_REAL_INF(		draw_segment_len_default,	0, 1		)
			PARAM_DEF_SYMBO_PSTR(	draw_number,				4, 0,		gstr::draw_number	)
			PARAM_DEF_REAL_ONE(		size_number					)
			PARAM_DEF_BOOL_OFF(		draw_path					)
			PARAM_DEF_REAL_POS(		draw_path_resolution,		1, 0		)
			PARAM_DEF_REAL_INF(		draw_path_start,			1, 0		)
			PARAM_DEF_REAL_INF(		draw_path_stop,				0, 1		)
			PARAM_DEF_REAL_INF(		draw_path_step_max,			0, 1		)
		
		PARAM_DEF_BOOL_OFF( only_hands_for_other )

		PARAM_DEF_GROUP_CLOSED( Net, NET_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		net_send					)
			PARAM_DEF_SYMBO_ZERO(	net_channel,				1, 0,		0, c_net::CHANNEL_NB, gstr::no )
	};
}

//todo	should add something to the base class not to save the file values

void	c_bdd_mocap::param_init_pt()
{
	INT32	h = param_init_pt_geo();
	param_set_pt( h, _b_use_gypsy );

	++h;
		param_set_pt( h, _channel_id );
		param_set_pt( h, _seq_nb );
		if( _s_origin == NATIVE )
		{
			get_param(h)->clear_lock();
			param_set_pt( h, _node_nb_ui );
			get_param(h)->clear_lock();
			param_set_pt( h, _frame_nb_ui );
		}
		else
		{
			get_param(h)->set_lock();
			param_set_pt( h, _node_nb_ui );
			get_param(h)->set_lock();
			param_set_pt( h, _frame_nb_ui );
		}
		param_set_pt( h, _s_origin			);
		param_set_pt( h, _mocap_filename	);
		param_set_pt( h, _actor_filename	);	

	param_set_pt( h, _b_clear_data_trig_ui );
	++h;
		param_set_pt( h, _b_playing				);
		param_set_pt( h, _b_restart_trig_ui		);
		param_set_pt( h, _b_live_ui				);
		param_set_pt( h, _b_reference_trig		);
		param_set_pt( h, _seq_cur_ui			);
		param_set_pt( h, _seq->_frame_in		);
		param_set_pt( h, _seq->_frame_out		);
		param_set_pt( h, _frame_by_sec_source	);
		param_set_pt( h, _time_factor_ui		);
		param_set_pt( h, _frame_by_sec			);
		param_set_pt( h, _time_len				);
		param_set_pt( h, _frame_cur				);
		param_set_pt( h, _phase					);
		param_set_pt( h, _seq->_b_loop			);

	++h;
		param_set_pt_3(	h,	_origin_ui			);
		param_set_pt_4(	h,	_origin_on_node_ui	);
		param_set_pt(		h,	_origin_on_node_id	);

	++h;
		param_set_pt( h, _b_rot_possible );
		param_set_pt( h, _b_tra_possible );
		param_set_pt( h, _b_sca_possible );

	++h;
		param_set_pt( h, _b_rot_ui );
		param_set_pt( h, _b_tra_ui );
		param_set_pt( h, _b_sca_ui );
		param_set_pt( h, _b_interpolate );
		param_set_pt( h, _b_interpolate_rot );

	++h;
		param_set_pt_4( h, _tra_scale_ui );

	++h;
		param_set_pt( h, _node_start );
		param_set_pt( h, _node_end );
		param_set_pt( h, _b_draw_origins_ui );
		param_set_pt( h, _size_origins );
		param_set_pt( h, _b_draw_segment_ui );
		param_set_pt( h, _segment_start );
		param_set_pt( h, _segment_end );
		param_set_pt( h, _segment_len_default_ui );
		param_set_pt( h, _s_draw_number );
		param_set_pt( h, _size_number );
		param_set_pt( h, _b_draw_path_ui );
		param_set_pt( h, _path_resolution );
		param_set_pt( h, _path_start );
		param_set_pt( h, _path_stop );
		param_set_pt( h, _path_dist_max );

	param_set_pt( h, _b_segment_only_hands_for_other_ui );

	++h;
		param_set_pt( h, _b_net_send_ui );
		param_set_pt( h, _net_channel );

	err_param_init_pt(h);
}

void	c_bdd_mocap::param_init()
{
	_seq_nb = 0;
	_node_nb = 0;
	_frame_nb = 0;
	_channel_id = 1;
	_frame_by_sec = 25.;
	_time_len = 1;
	_phase_last = -1;
	_b_rot_possible = false;
	_b_tra_possible = false;
	_b_sca_possible = false;
	_frame_by_sec_source = 25.;
}

void	c_bdd_mocap::lock()		{	_data_lock.lock();		}
void	c_bdd_mocap::unlock()	{	_data_lock.unlock();	}

/*
AAA_ERR	c_bdd_mocap::load_do_before( o_str CONST & filename)
{
	if( !data )
		_b_mocap_loaded = false;
	return	AAA_OK;
}
*/

AAA_ERR	c_bdd_mocap::load_do_after( o_str CONST & filename )
{
	if( !_data )
	{
		_b_mocap_loaded = false;
		_b_actor_loaded = false;
	}
	return	AAA_OK;
}

//todoq	should be dynamic
static	CONST	INT32	NODE_MAX_NB		=	32;

void	c_bdd_mocap::init()
{
	_b_valid_data = false;
	_b_mocap_loaded = false;
	_b_actor_loaded = false;
	_data = nullptr;
	_data_cur = nullptr;
	_data_live_past = nullptr;
	_data_live = nullptr;
	_node_bdd_index = nullptr;
	_node_bdd_multi = nullptr;
	_len_by_node = nullptr;
	_real_by_node = 0;

	_mocap_filename.erase();
	_actor_filename.erase();

	obj_get( _node_name );
	_node_name->set( NODE_MAX_NB, "Mocap node", nullptr, aaa::file::TYPE_IO_NONE );

	_seq_cur = -1;
	_seq = _seq_array;
	_gypsy_skel = nullptr;
	_s_origin = NATIVE;

	_mocap_reader = nullptr;

	clear_data();
}

void	c_bdd_mocap::clear_data()
{
	if( _node_bdd_index && _node_bdd_multi )
	{
//		mem::is_all_ok( "mocap::clear_data() before" );
		for( INT32 i=_seq_nb*_node_nb-1; i>=0; --i )	//todo make clear fn
		{
			_node_bdd_index[i] = 0;
		}
		for( INT32 i=_node_nb-1; i>=0; --i )	//todo make clear fn
		{
			_node_bdd_multi[i] = false;	//hack add for NATIVE
			_len_by_node[i] = 0;	
		}
//		mem::is_all_ok( "mocap::clear_data() after" );
	}
	_frame_native_offset = 0;
	_frame_native_nb = 0;
	_b_store_native = false;
	_b_valid_data = false;
}

bool	c_bdd_mocap::alloc( INT32 frame_nb_in, INT32 node_nb_in )
{
	if( _real_by_node > 0 )
	{
		INT32	size_frame_in_real = node_nb_in * _real_by_node;

		_data			= new REAL	[ frame_nb_in * size_frame_in_real ];
		_data_cur		= new REAL	[ 3 * size_frame_in_real ];
		_node_bdd_index	= new INT32	[ _seq_nb*node_nb_in ];
		_node_bdd_multi = new bool	[ node_nb_in ];
		_len_by_node	= new REAL	[ node_nb_in ];

		if ( _data && _data_cur && _node_bdd_index && _node_bdd_multi && _len_by_node )
		{
			_node_nb		= node_nb_in;
			_real_by_frame	= size_frame_in_real;
			_data_live_past	= _data_cur + size_frame_in_real;
			_data_live		= _data_live_past + size_frame_in_real;
			_frame_nb		= frame_nb_in;
			clear_data();
			return true;
		}
		//	mem::is_all_ok( "mocap::alloc()" );	
		dealloc();	
	}

	return false;
}

//todo	check if no pb because of point_for_normal
//hack ? got rid of -1
FINLINE	REAL*	c_bdd_mocap::get_data( INT32 frame )
{
	//todoqq
	//#if	AAA_DEBUG()
	//		if( frame < 0 || frame >= _seq->len_rel )
	//			debug_break( "Bug frame in c_bdd_mocap::get_data()" );
	//#endif
	if( get_frame_nb_to_play() <= 0 )
	{
		debug_break( "we should pass by here" );
	}
	//hack ? got rid of -1
	//frame = IMOD( frame + _frame_native_offset-1, _seq->len_rel );
	frame = compute_frame( frame );
	if( _s_origin != NATIVE )
		frame += _seq->_offset_abs + _seq->_offset_rel;
	return _data + frame * _real_by_frame;
}

FINLINE	REAL*	c_bdd_mocap::get_data_index( INT32 index )
{
//#if	AAA_DEBUG()
	if( index < 0 || index > get_frame_nb_to_play() )
		debug_break( "Bug in c_bdd_mocap::get_data_index()" );
//#endif	
	if( _s_origin != NATIVE )
		index += _seq->_offset_abs + _seq->_offset_rel;
	return _data + index * _real_by_frame;
}

void	c_bdd_mocap::push_data_low( REAL* buf, INT32 len )
{
	REAL* dst = get_data_index( _frame_native_offset );
	MEMCPY( dst, buf, len * sizeof(REAL), __FUNCTION__ );
	if( _seq->_frame_nb_rel <= 1 )	//check what we reakky do with frame_in and frame_out
	{
		for( INT32 i = 0; i < _node_nb; ++i )
		{
			*( dst + _offset_s ) = 0;
			_len_by_node[i] = 0;
			dst += _real_by_node;
		}
	}
	else
	{
		REAL* prev = get_data(-1);
		for( INT32 i = 0; i < _node_nb; ++i )
		{
			REAL len =  *( prev + _offset_s ) + dist_v3r( dst + _offset_tra, prev + _offset_tra );
			*( dst + _offset_s ) = len;
			_len_by_node[i] = len;
			dst += _real_by_node;
			prev += _real_by_node;
		}
	}
//	mem::is_all_ok( "mocap::push_data_low()" );
	_frame_native_nb		=	MIN(	_frame_native_nb + 1,		_frame_nb );
	_frame_native_offset	=	IMOD(	_frame_native_offset + 1,	_frame_nb );
	_seq->_frame_nb_rel		=	_frame_native_nb;
	_b_store_native			=	false;
	_b_valid_data			=	true;

}

void	c_bdd_mocap::push_data()
{
	if( _net_channel && net )
	{	//todo make it better //and machine independent
		if( _b_net_send_ui )
		{
			if( _b_store_native )
			{
				INT32 size = INT32( _real_by_frame * sizeof(REAL) );
				if( (c_net::get_blk_obj_data_size_max()-32) > size )
					net->sendto_link0( _net_channel, c_net::BLK_OBJ_DATA, (UINT8*)_data_live, size );
				push_data_low( _data_live, _real_by_frame );
			}
		}
		else
		{
			c_net_blk*	blk_in;
			while( blk_in = net->blk_take_by_type_channel( c_net::BLK_OBJ_DATA, _net_channel ) )
			{
				push_data_low( (REAL*) blk_in->get_data_pt(), _real_by_frame );
				net->blk_free( blk_in );
			}
		}
	}
	else
	{
		if( _b_store_native )
		{
			push_data_low( _data_live, _real_by_frame );
		}
	}
}

void	c_bdd_mocap::store_tra( INT32 node_id, REAL CONST * tra )
{
	if( _s_origin == NATIVE )
	{
//		mem::is_all_ok( "mocap::store_tra()" );
		if( _node_nb && _data_live )
		{
			INT32	node_index	= IMOD( node_id-1, _node_nb );
			INT32	offset		= node_index * _real_by_node + _offset_tra;
			cpy_v3( _data_live_past + offset,	_data_live + offset );
			cpy_v3( _data_live + offset, tra );
			_b_store_native = true;
		}
	}
}

void	c_bdd_mocap::dealloc()
{
//	mem::is_all_ok( "mocap::dealloc() before");
	SAFE_DELETE_ARRAY( _data );
	SAFE_DELETE_ARRAY( _data_cur );
	SAFE_DELETE_ARRAY( _node_bdd_index );
	SAFE_DELETE_ARRAY( _node_bdd_multi );
	SAFE_DELETE_ARRAY( _len_by_node );
	_data_live_past = nullptr;
	_data_live = nullptr;
//	mem::is_all_ok( "mocap::dealloc()");
}

CONSTRUCTOR_CREATE(c_bdd_mocap)
	,_node_name(nullptr)
	,_frame_length(0.)
	,_time_factor(0.)
	,_segment_len_default(REAL(554564564.))	//hack
	,_real_by_frame(0)
	,_offset_tra(-1)
	,_offset_rot(-1)
	,_offset_mat(-1)
	,_offset_sca(-1)
	,_b_tra(false)
	,_b_rot(false)
	,_b_sca(false)
	,_mocap_reader( nullptr )
{
	init();
	param_init_with( n_bdd_mocap::param, n_bdd_mocap::PARAM_NB_MAX ); //  bdd_mocap_param, BDD_MOCAP_PARAM_NB);
}

c_bdd_mocap::~c_bdd_mocap()
{
	if( cur == this )
		cur = def;
	if( ui == this )
		ui = def;

	dealloc();
	obj_delete( _node_name );

#if AAA_LIB_USE_ASSIMP()
	obj_delete( _mocap_reader );
#endif	//#if AAA_LIB_USE_ASSIMP()
}

AAA_ERR	c_bdd_mocap::load_data()
{
	AAA_ERR	retcode = AAA_OK;
	if( !_b_mocap_loaded )
	{
		retcode = load_data_from_file( _mocap_filename.get() );
		if ( NOERR(retcode) )
		{
			for( INT32 i = 0; i < _node_nb; ++i )
				_len_by_node[i] = compute_len( i+1 );
		}
//		HEAP_IS_CORRUPT();
	}
	if( NOERR(retcode) )
	{
		// RC modif for actor files ----------------------------------------------------------------------------------------------------------------
		//if ( _file_type == MOCAP_BVH )
		//{
		//	if( !_b_actor_loaded && _actor_filename.is_not_empty() )
		//		retcode = load_actor_from_file( _actor_filename.get() );
		//}
	}
	return	retcode;
}

AAA_ERR	c_bdd_mocap::load_data_from_filename( o_str CONST & filename, INT32 type_io )
{
	switch(type_io)
	{
	case aaa::file::TYPE_IO_BDD_MOCAP:		return load_from_file(			filename );
	case aaa::file::TYPE_IO_BDD_MOCAP_DATA:	return load_data_from_file(		filename.get() );
	case aaa::file::TYPE_IO_BDD_MOCAP_ACTOR:	return load_actor_from_file(	filename.get() );
	}
	return ERR_TYPE_UNUSED;
}

void	c_bdd_mocap::get_matrix_rows( REAL* CONST a, REAL* CONST b, REAL* CONST c, INT32 CONST node_id )
{
	if( _b_valid_data && _offset_mat >= 0 && is_valid_node( node_id ) )
		aaa::matrix::extract_rows_from_mat3( a, b, c, get_data_cur( node_id ) + _offset_mat );
	else
	{
		clear_v3(a);
		clear_v3(b);
		clear_v3(c);
	}
}

void	c_bdd_mocap::transfo_do( INT32 CONST node_id )
{
	REAL*	pt = get_data_cur( node_id );
	if( _b_tra )
		do_translate( pt + _offset_tra );
	if( _b_rot )
	{
		if( _offset_mat >= 0 )
			do_matrix_from_mat3( pt + _offset_mat );
		else
			do_rotate( pt + _offset_rot );
	}
	if( _b_sca )
		do_scale( pt + _offset_sca );
}

FINLINE	bool	c_bdd_mocap::get_data_cur_and_next( REAL* &cur, REAL* &next, REAL &frame, INT32 CONST node_id )
{
	if( _b_live_ui )
	{
		//todoqq get the speed in real time
		INT32 offset = get_node_offset(node_id);
		cur		= _data_live_past	+ offset;
		next	= _data_live		+ offset;
		return false;
	}
	else
	{
		INT32	index = I_FLOOR( frame );
		frame -= index;
		cur = get_data( index, node_id );
		if( index == get_frame_nb_to_play()-1 )
		{
			if( _seq->_b_loop )
				next = get_data( 0, node_id );
			else
			{
				if( get_frame_nb_to_play() <= 1 )
					next = cur;
				else
				{
					next = get_data( index-1, node_id );
					frame = 1;
				}
			}
			return true;
		}
		else
		{
			next = get_data( index+1, node_id );
			return false;
		}
	}
}

void	c_bdd_mocap::get_tra( REAL* dst, INT32 CONST node_id, REAL CONST phase_in )
{
	if( !_b_valid_data )
		return;

	REAL	s = REAL(compute_frame_from_phase_no_in( phase_in ));
	REAL*	prev;
	REAL*	next;
	get_data_cur_and_next( prev, next, s, node_id );

	if( _b_interpolate )
	{
		interpolate_v3( dst, prev+_offset_tra, next+_offset_tra, s );
		mul_add_v3( dst, _tra_scale, _origin );
	}
	else
		mul_add_v3( dst, prev+_offset_tra, _tra_scale, _origin );
}

//hack check scale present
//hack do rot
void	c_bdd_mocap::get_sca( REAL* dst, INT32 CONST node_id, REAL CONST phase_in )
{
	if( !_b_valid_data )
		return;

	REAL	s = REAL(compute_frame_from_phase_no_in( phase_in ));
	REAL*	prev;
	REAL*	next;
	get_data_cur_and_next( prev, next, s, node_id );
		
	if( _b_interpolate )
		interpolate_v3( dst, prev+_offset_sca, next+_offset_sca, s );
	else
		cpy_v3( dst, prev+_offset_sca );
}

bool	c_bdd_mocap::get_point_on_seg( REAL* CONST dst, REAL CONST when, INT32 CONST seg_id, REAL CONST where )
{
	if( !_b_valid_data )
	{
		clear_v3( dst );
		return false;
	}

	INT32	index;
	INT32	ia, ib;
	_segments.get_segment_point_id( seg_id, ia, ib );

	REAL	f;
	//	find mocap frame and prev
//	f = FMOD( when, time_len) * ABS(_frame_by_sec);
	f = when / _time_factor * ABS(_frame_by_sec);
	f = REAL(compute_frame(INT32(f)));
//	f = FMOD( frame_in, _seq->len_rel ) ; 
	
	//todoqq get the speed in real time
	REAL*	prev;
	REAL*	next;
	get_data_cur_and_next( prev, next, f, 1 );

	//	interpolate in time
	REAL	a[3];
	index = get_node_offset(ia) + _offset_tra;
	interpolate_v3( a, prev + index , next + index, f );

	REAL	b[3];
	index = get_node_offset(ib) + _offset_tra;
	interpolate_v3( b, prev + index, next + index, f );

	//	interpolate on segment
	interpolate_v3( dst, a, b, where );
	mul_add_v3( dst, _tra_scale, _origin );

	return true;
}


//todo manage to cut the memmove/memcpy when not needed
void	c_bdd_mocap::build_data( REAL* dst_data, REAL src_frame, INT32 src_node_start, INT32 src_node_stop )
{
	REAL	f;
	REAL*	prev;
	REAL*	next;
	REAL*	dst;

	//DBG_PRINT_STRING( "%x %f, %d %d\", dst_data, src_frame, src_node_start, src_node_stop);
	f = src_frame;

	//todo check if the concept is useful and transpose it
	//#if	BELGA
	//	if( get_data_cur_and_next( prev, next, &f, src_node_start) )
	//		special_mocap_end_seq( this);

	get_data_cur_and_next( prev, next, f, src_node_start );

	dst = dst_data;
	switch( _file_type )
	{
	case MOCAP_TRC:
	case MOCAP_HRC_SIGAUD:
		if( _b_interpolate )
		{
			for( INT32 i=src_node_start; i<=src_node_stop; ++i )
			{
				interpolate_v3( dst + _offset_tra, prev + _offset_tra, next + _offset_tra, f );
				dst += _real_by_node;
				prev += _real_by_node;
				next += _real_by_node;
			}
		}
		else
		{
			for( INT32 i=src_node_start; i<=src_node_stop; ++i )
			{
				cpy_v3( dst + _offset_tra, prev + _offset_tra );
				dst += _real_by_node;
				prev += _real_by_node;
			}
		}
		break;
	case MOCAP_NATIVE:
	case MOCAP_MAA:
	case MOCAP_BVH:
		if( _b_live_ui && _gypsy_skel )
		{
			_gypsy_skel->get_data_live( _data_live, _real_by_node );
			prev = _data_live;
			for( INT32 i=src_node_start; i<=src_node_stop; ++i )
			{
				MEMCPY( dst, prev, _real_by_node*sizeof(REAL), __FUNCTION__ );
				//printf( "%f %f %f\n", *(prev+_offset_rot), *(prev+_offset_rot+1), *(prev+_offset_rot+2) );
				dst += _real_by_node;
				prev += _real_by_node;
			}
		}
		else
		{
			//printf( "before b_interpolate\n" );
			if( _b_interpolate )
			{
				//todo copy the whole thing first
				for( INT32 i=src_node_start; i<=src_node_stop; ++i )
				{
					//printf( "interpolate loop %d\n", i );
					MEMCPY( dst, prev, _real_by_node*sizeof(REAL), __FUNCTION__ );
					//printf( "after memcpy\n" );
					//printf( "%f %f %f\n", *(prev+_offset_rot), *(prev+_offset_rot+1), *(prev+_offset_rot+2) );
					if( _b_rot && _b_interpolate_rot )
					{
						if( _offset_rot >= 0 )
							interpolate_v3( dst + _offset_rot, prev + _offset_rot, next + _offset_rot, f );
						else
							interpolate_v3r_list( dst + _offset_mat, prev + _offset_mat, next + _offset_mat, f, 3 );
					}
					//printf( "after rot\n" );
					if( _b_tra )
						interpolate_v3( dst + _offset_tra, prev + _offset_tra, next + _offset_tra, f );
					//printf( "after tra\n" );
					if( _b_sca )
						interpolate_v3( dst + _offset_sca, prev + _offset_sca, next + _offset_sca, f );
					//printf( "after scale\n" );

					dst += _real_by_node;
					prev += _real_by_node;
					next += _real_by_node;
				}
			}
			else
			{
				//printf( "before regular loop\n" );
				for( INT32 i=src_node_start; i<=src_node_stop; ++i )
				{
					MEMCPY( dst, prev, _real_by_node * sizeof(REAL), __FUNCTION__ );
					//printf( "%f %f %f\n", *(prev+_offset_rot), *(prev+_offset_rot+1), *(prev+_offset_rot+2) );

					dst += _real_by_node;
					prev += _real_by_node;
				}
			}
		}
		break;
	}

	if( _b_tra )
	{
		if( _b_tra )
		{
			REAL	factor = g_app->get_global_size_factor();
			scale_v3( _tra_scale, _tra_scale_ui, _tra_scale_ui[3]*factor );
			scale_v3( _origin, _origin_ui, factor );
		}

		if( !is_all_one_v3( _tra_scale ) || is_not_null_v3( _origin ) )
		{
			dst = dst_data + _offset_tra;
			for( INT32 i = src_node_start; i <= src_node_stop; ++i )
			{
				mul_add_v3( dst, _tra_scale, _origin );
				//mul_v3( dst, _tra_scale );
				//printf( "%f %f %f\n", *(prev+_offset_rot), *(prev+_offset_rot+1), *(prev+_offset_rot+2) );
				dst += _real_by_node;
			}
		}

		scale_v3( _origin_on_node, _origin_on_node_ui, _origin_on_node_ui[3] );
		if( is_not_null_v3(_origin_on_node) )
		{
			REAL off[3];
			cpy_v3( off, get_data_node( _data_cur, _origin_on_node_id ) + _offset_tra );
			sub_v3( off, _origin, off );
			mul_v3( off, _origin_on_node );
			REAL* dst = _data_cur + _offset_tra;
			for( INT32 i = 1; i <= _node_nb; ++i )
			{
				add_v3( dst, off );
				//printf( "%f %f %f\n", *(prev+_offset_rot), *(prev+_offset_rot+1), *(prev+_offset_rot+2) );
				dst += _real_by_node;
			}
			add_v3(  _origin, off );
		}
	}
}

void	c_bdd_mocap::restart()
{
	_b_restart_trig_ui = true;
}

//todo manage to cut the memmove when not needed
void	c_bdd_mocap::update()
{
	std::lock_guard<c_bdd_mocap> guard(*this);

	inst_by_channel.set( _channel_id, this );
	c_bdd_mocap::cur = this;

	load_data();
	if ( _file_type == MOCAP_BVH )
	{
		if( _gypsy_skel )
		{
			_b_live_ui = _gypsy_skel->set_live( _b_live_ui );
			if( _b_reference_trig )
			{
				_b_reference_trig = false;
				_gypsy_skel->set_refrence_position();
			}
		}
		else
			_b_live_ui = false;
	}
	if( _frame_nb_ui != _frame_nb || _node_nb_ui != _node_nb )
	{
		dealloc();
		alloc( _frame_nb_ui, _node_nb_ui );
	}
	if( _b_clear_data_trig_ui )
	{
		clear_data();
		_b_clear_data_trig_ui = false;
	}
	
	if( _b_valid_data || _s_origin == NATIVE )
	{
		if( _seq_cur_ui >= _seq_nb )
		{
			_seq_cur_ui = 0;
			DBG_PRINT_STRING( "I had an invalid sequence number" );
		}
//		mem::is_all_ok( "mocap::set_seq()");
		set_seq( _seq_cur_ui );
	}
//	mem::is_all_ok( "mocap::push_data()");
	if( _s_origin == NATIVE )
		push_data();
//	mem::is_all_ok( "mocap::push_data()" );
	if( _b_valid_data )
	{
		_b_tra = _b_tra_ui && _b_tra_possible;
		_b_rot = _b_rot_ui && _b_rot_possible;
		_b_sca = _b_sca_ui && _b_sca_possible;

		_seq->update();

		get_param( n_bdd_mocap::INDEX_SEQ_NB_PARAM )->set_max( REAL(_seq_nb - 1) ); //  BDD_MOCAP_PARAM_INDEX_SEQ_NB)->set_max( _seq_nb -1);

		_segment_start	=	_segments.clamp_id( _segment_start );
		_segment_end	=	_segments.clamp_id( _segment_end );

		if( _segment_len_default != _segment_len_default_ui )
			build_segment_weight();

		if( _delta_t.update() || _b_restart_trig_ui )
		{
			_b_restart_trig_ui = false;
			_phase = 0.;
			_frame_cur = 0;
		}
		if( _time_factor_ui == 0. )
			_time_factor = 0.;
		else
			_time_factor = _time_factor_ui;
		_frame_by_sec = _frame_by_sec_source * _time_factor;
		_frame_length = OVER_ONE_AS_REAL( _frame_by_sec );
		_time_len = ABS( _seq->_frame_nb_rel * _frame_length );
		if( _b_playing || _time_factor_ui == 0. )	//we force it but perhaps not a good idea
		{
			_phase += REAL(_delta_t.get_dt()*_frame_by_sec) * OVER_ONE_AS_REAL(get_frame_nb_to_play());
			//	find _frame_nb and interframe value
			_frame_cur = REAL( compute_frame_from_phase( _phase ) );
		}
		else if( _phase_last != _phase )
		{
			_frame_cur = REAL( compute_frame_from_phase( _phase ) );
		}
		else
		{
			_frame_cur = REAL( compute_frame_from_phase( _phase ) );
			_phase = REAL(_frame_cur-get_frame_start()) * OVER_ONE_AS_REAL(get_frame_nb_to_play());	//todo make a fn for this ?
		}

		build_data( _data_cur, _frame_cur, 1, _node_nb );

		//todo only when drawn
		if( c_multiple::cur->is_render_multiple() && c_multiple::cur->can_use_list() )
			multiple_compile_one();
		_phase_last = _phase;
//		mem::is_all_ok( "mocap::update() at end" );
	}
}

//todo ?	this is not scaled, update strategy ?
REAL	c_bdd_mocap::compute_len( INT32 CONST node_id )
{
	REAL	len = 0;
	if( _b_valid_data && is_valid_node( node_id ) )
	{
		c_seq_mocap*	seq = &_seq_array[0];	//	store on seq 0 &_seq_array[node_id-1];
												//todo redeal with this it made code unclear
		REAL*	cur = get_data( 0, node_id ) + _offset_tra;
		REAL*	next;
		for( INT32 i=1; i<seq->_frame_nb_rel-1; ++i )
		{
			next = get_data( i, node_id ) + _offset_tra;
			len += dist_v3r( cur, next );
			cur = next;
		}
		next = get_data( 0 ) + _offset_tra;
		len += dist_v3r( cur, next );
	}
	return len;
}

REAL	c_bdd_mocap::get_len( INT32 CONST node_id )
{
	if( _b_valid_data )
	{
		INT32	node_index	= CLAMP( node_id, 1, _node_nb ) - 1;	//todo check
		return	_len_by_node[node_index];
	}
	return 0.;
}

//todo specialized fn for tgn ?
FINLINE	void	c_bdd_mocap::get_point_tangent_raw_no_check( REAL* dst, REAL* tgn, INT32 CONST node_id, REAL s )
{
	get_tra( dst, node_id, s );	
	get_tra( tgn, node_id, s + OVER_ONE_AS_REAL(get_frame_nb_to_play()) );	//find next point
	sub_v3( tgn, dst );
}

FINLINE	void	c_bdd_mocap::get_point_tangent_no_check( REAL* dst, REAL* tgn, INT32 CONST node_id, REAL s )
{
	get_point_tangent_raw_no_check( dst, tgn, node_id, s );
	normalize_v3r( tgn );
}

//todo specialized fn for tgn
void	c_bdd_mocap::get_tangent( REAL* dst, INT32 CONST node_id, REAL CONST s )
{
	REAL	tmp[3];
	get_point_tangent( tmp, dst, node_id, s );
}

#if	!AAA_REAL_IS_DOUBLE()
//todo look wrong to me used in track check
void	c_bdd_mocap::get_tangent( REAL* dst, INT32 CONST node_id )
{
	//	s = FMOD( phase_in, 1.) * _seq->len_rel;
	REAL s = REAL(_seq->_frame_nb_rel - 1);

	//if( _b_valid_data && s  ) // 2025 Nov Maa remove test on s which have no sense
	if( _b_valid_data  )
	{
		REAL*	prev;
		REAL*	next;
		get_data_cur_and_next( prev, next, s, node_id );

		sub_v3( dst, prev, next );
		normalize_v3r( dst );
	}
	else
		clear_v3( dst );
}
#endif

void	c_bdd_mocap::get_tangent( DOUBLE* dst, INT32 CONST node_id )
{
	//	s = FMOD( phase_in, 1.) * _seq->len_rel;
	REAL s = REAL(_seq->_frame_nb_rel - 1);

	//if( _b_valid_data && s  ) // 2025 Nov Maa remove test on s which have no sense
	if( _b_valid_data )
	{
		REAL*	prev;
		REAL*	next;
		get_data_cur_and_next( prev, next, s, node_id );

		sub_v3( dst, prev, next );
		normalize_v3d( dst );
	}
	else
		clear_v3( dst );
}

//todo compute several points
FINLINE	void	c_bdd_mocap::coor_to_world_one_low( REAL * dst, INT32 CONST node_id, REAL CONST * CONST src )
{
	//todoopt this is here so if src and dst are equal we are still ok but this is not optimal deformer should handle
	REAL	s = src[0];
	REAL	t = src[2];
	REAL	y = src[1];

/*
	REAL	tgn[3];
	REAL	axe_z[3];

	get_point_tangent_raw_no_check( dst, tgn, node_id, s );

	//	find z direction
	//was	normalize_v3r( tgn );
	//was	cross_normalize_v3r( axe_z, unit_y_v4fp32, tgn );
	cross_normalize_y_v3r( axe_z, tgn );	//	reverse of now
	add_scale_v3( dst, axe_z, -t );		//	so minus reverse it again
	dst[1] += y;
*/
	REAL	tgn[3], axe_z[3], axe_y[3];

	//find point and tangent
	get_point_tangent_no_check( dst, tgn, node_id, s );

	build_normal_vectors_v3r( tgn, axe_y, axe_z );

	add_scale_v3( dst, axe_z, -t );		//	minus to reverse the sign
	add_scale_v3( dst, axe_y, y );			//	dst[1] += y;
}

void	c_bdd_mocap::coor_to_world_one( REAL * dst, INT32 CONST node_id, REAL CONST * CONST src )
{
	if( _b_valid_data )
		coor_to_world_one_low( dst, node_id, src );
}

void	c_bdd_mocap::coor_to_world( REAL* dst, INT32 CONST node_id, REAL CONST * src, INT32 nb )
{
	if( !_b_valid_data )
		return;

	for ( ; nb>0; --nb )
	{
		coor_to_world_one_low( dst, node_id, src );
		dst += 3;
		src += 3;
	}
}

static	INT32	hack_nb_point_read;

AAA_ERR	c_bdd_mocap::read_point_from_stream( FILE* file, INT32 nb )
{
	INT32	j;
	INT32	retcode = AAA_OK;
	INT32	tmp;
	DOUBLE	r;
	REAL*	pt;
	DOUBLE	f[3];

	pt = _data;
	hack_nb_point_read = 0;

	switch( _file_type )
	{
	case MOCAP_TRC:
		do
		{
			tmp = fscanf( file, "%d", &j );
			//printf("%d:",i);
			if ( tmp != 1 )
				break;

			tmp = fscanf( file, "%lf", &r );
			//printf("%f.  ",r);
			if ( tmp != 1)
				break;

			for( INT32 i = _node_nb; i > 0; --i )
			{
				tmp = fscanf( file, "\t%lf\t%lf\t%lf", f, f + 1, f + 2 );
				if ( tmp != 3 )
				{
					//3retcode = ERR_ANY;
					ERR_PRINT_STRING( "Pb reading point data" );
					goto exit;
				}
				scale_v3( pt, f, .001 );
				pt += 3;
			}
			++hack_nb_point_read;
		}
		while( true );
		break;
	default:
		ERR_PRINT_STRING( "%s() : unknown mocap file type", __FUNCTION__ );
		break;
	}
exit:
	return retcode;
}

AAA_ERR	c_bdd_mocap::read_data_from_trc_stream( FILE* file )
{
	AAA_ERR	retcode = ERR_FILE_BASE;
	INT32	tmp;
	char	str[64];

	while( true )
	{
		tmp = fscanf( file, "%63s", str );
		if( tmp == 1 )
		{
			if ( strcmp( str, "Z36" ) == 0 )
				break;
		}
		else
			goto exit;
	}

	//get_next_line_starting_with( "1");
	retcode = read_point_from_stream( file, 10000000 );
exit:
	return retcode;
}

AAA_ERR	c_bdd_mocap::load_actor_from_file( CONST CHAR *CONST filename )
{
	AAA_ERR	retcode = ERR_FILE_BASE;

	if( *filename )
	{
		o_str	o;
		c_dir_pool::cur->expand_fname( o, filename );
		//	_file_type = MOCAP_BVH;
		if( _gypsy_skel )
		{
			if( NOERR( _gypsy_skel->read_actor_from_filename( o.get() ) ) )
			{
				_b_actor_loaded = true;
				//retcode = AAA_OK;
				_actor_filename.set( filename );
				retcode = _gypsy_skel->get_data( _data, _real_by_node );
			}
		}
		else
		{
			ERR_PRINT_STRING( "No Skeleton for actor file %s", o.get() );
			goto exit;
		}
	}

exit:
	if( ERR(retcode) )
		ERR_PRINT_STRING( "Could not read MoCap actor %s.", filename );
	return retcode;
}


AAA_ERR	c_bdd_mocap::load_data_from_file( CONST CHAR *CONST filename )
{
	AAA_ERR	retcode = ERR_FILE_BASE;

	INT32	node_nb_to_load = 0;
	INT32	frame_nb_to_load = 0;
	o_str	o;

	// usually the default
	_offset_tra = -1;
	_offset_rot = -1;
	_offset_mat = -1;
	_offset_sca = -1;
	_seq_nb = 1;

	if( filename && *filename )
	{
		c_dir_pool::cur->expand_fname( o, filename );
		auto ext = o.get_ext();
		if( str_is_equal_nocase( ext, "trc" ) )
		{
			GOOD_PRINT_STRING( "MoCap: Reading  trc file %s", o.get());
			_s_origin = FROM_SOFTIMAGE;
			_file_type = MOCAP_TRC;
			node_nb_to_load = 36;
			frame_nb_to_load = 10000;
			_frame_by_sec_source = 60.0f;
			_offset_tra = 0;
			_b_mocap_loaded = true;
		}
		else if( str_is_equal_nocase( ext, "hrc" ) )
		{
			GOOD_PRINT_STRING( "MoCap: Reading  hrc file %s", o.get());
			_s_origin = FROM_SOFTIMAGE;
			_file_type = MOCAP_HRC_SIGAUD;
			file_hrc_count( o.get(), &node_nb_to_load, &frame_nb_to_load );
			_frame_by_sec_source = 25.;
			_offset_tra = 0;
			_b_mocap_loaded = true;
		}
		else if( str_is_equal_nocase( ext, "maa" ) )
		{
			GOOD_PRINT_STRING( "MoCap: Reading  maa file %s", o.get());
			_s_origin = FROM_MAX;
			_file_type = MOCAP_MAA;
			if( NOERR(file_maa_count( o.get(), &node_nb_to_load, &frame_nb_to_load )) )
			{
				_frame_by_sec_source = 25.;
				_offset_tra = 3;
				_offset_rot = 0;
				_offset_sca = 6;
				_b_mocap_loaded = true;
			}
		}
		else if( str_is_equal_nocase( ext, "bvh" ) )
		{
			_s_origin = FROM_BVH;
			_file_type = MOCAP_BVH;
			if( _b_use_gypsy )
			{
				GOOD_PRINT_STRING( "%s(): Reading Gypsy file %s", __FUNCTION__, o.get() );
				if( !_gypsy_skel )
					_gypsy_skel = new c_gypsy_skel;
				if( _gypsy_skel )
				{
					if( NOERR( _gypsy_skel->read_bvh_from_filename(o.get()) ) )
					{
						node_nb_to_load			= _gypsy_skel->get_node_nb();
						frame_nb_to_load		= _gypsy_skel->get_frame_nb();
						_frame_by_sec_source	= _gypsy_skel->get_frame_by_sec();
						//file_maa_count( s.get(), &node_nb_to_load, &frame_nb_to_load );
						_offset_tra = 0;
						_offset_mat = 3;
						_b_mocap_loaded = true;
					}
				}
			}
			else
			{
#if AAA_LIB_USE_ASSIMP()
				GOOD_PRINT_STRING( "%s(): Reading Assimp file %s", __FUNCTION__, o.get() );
				if( !_mocap_reader )
					_mocap_reader = mocap::c_bvh_data::create_method();
				if( _mocap_reader->import_data( o.get() ) )
				{

					// Grab base infos
					node_nb_to_load		 = _mocap_reader->get_node_nb();
					frame_nb_to_load	 = _mocap_reader->get_frame_nb( 0 );
					_frame_by_sec_source = _mocap_reader->get_frame_by_sec( 0 );
					// Config offsets
					_offset_tra = 0;
					//_offset_mat = 3;
					//_offset_rot = 3;
					//_offset_sca = 7;
					// Set MOCAP loaded state
					_b_mocap_loaded = true;
				}
#else
				GOOD_PRINT_STRING( "%s(): Assimp lib not included in this executable, can't read %s", __FUNCTION__, o.get() );
				_b_mocap_loaded = false;
#endif	//#if AAA_LIB_USE_ASSIMP()
			}		
		}
		else
		{
			ERR_PRINT_STRING( "File %s unknown type.", o.get());
			goto exit;
		}
	}
	else
	{
//		mem::is_all_ok( "mocap:: initializing for Native)");
		GOOD_PRINT_STRING( "MoCap: initializing for Native" );
		_file_type = MOCAP_NATIVE;
		frame_nb_to_load = _frame_nb_ui;
		node_nb_to_load = _node_nb_ui;
		_frame_by_sec_source = 25.;
		_offset_tra = 0;
		_b_mocap_loaded = true;
		_seq = _seq_array;
		_seq->_offset_abs = 0;
		_seq->_offset_rel = 0;
		_seq->_frame_nb_rel = _frame_nb;
		_seq->_frame_nb_abs = _frame_nb;
		_seq->_node_nb = _node_nb;
		_seq->_frame_nb_to_play = 0;	//todo init correctly always
		_seq_nb = 1;
	}

	if( _b_mocap_loaded )
	{
		_real_by_node = 0;

		_b_tra_possible = _offset_tra >= 0;
		if( _b_tra_possible )
			_real_by_node += 3;

		_b_rot_possible = (_offset_rot >= 0) || (_offset_mat >= 0);
		if( _offset_rot >= 0 )
			_real_by_node += 3;

		if( _offset_mat >= 0 )
			_real_by_node += 9;

		_b_sca_possible = _offset_sca >= 0;
		if( _b_sca_possible )
			_real_by_node += 3;

		//	we store curvilinear data here
		_offset_s = _real_by_node;
		++_real_by_node;

//		mem::is_all_ok( "mocap::dealloc() before");
		dealloc();
//		mem::is_all_ok( "mocap::dealloc()");
		if( alloc( frame_nb_to_load, node_nb_to_load ) )
		{
//			mem::is_all_ok( "mocap::alloc() after" );
			_node_nb_ui = _node_nb;
			_frame_nb_ui = _frame_nb;
		}
		else
		{
			ERR_PRINT_STRING( "Can't Allocate %d frames for %d nodes", frame_nb_to_load, node_nb_to_load );
			retcode = ERR_MEM_BASE;
			goto exit;
		}
		GOOD_PRINT_STRING( "Allocated : %d frames for %d nodes", _frame_nb, _node_nb );

		switch( _file_type )
		{
		case MOCAP_NATIVE:
			retcode = AAA_OK;
			break;

		case MOCAP_TRC:
			{
				FILE*	file = nullptr;
				file = c_file::FOPEN( o.get(), "rt" );
				if( !file )
				{
					ERR_PRINT_STRING( "Can't read %s file.", o.get() );
					goto exit;
				}
				//todo add FSEEK fn with FOPEN
				c_file::FSEEK_SET( file, INT32(0) );
				retcode = read_data_from_trc_stream(file);
				_frame_nb = hack_nb_point_read;
				c_file::FCLOSE(file);
			}
			break;

		case MOCAP_HRC_SIGAUD:
			retcode = file_hrc_read( o.get(), _node_nb, _frame_nb, _data );
			_seq = _seq_array;
			_seq->_offset_abs = 0;
			_seq->_offset_rel = 0;
			_seq->_frame_nb_rel = _frame_nb;
			_seq->_frame_nb_abs = _frame_nb;
			_seq->_node_nb = _node_nb;
			_seq_nb = 1;
			break;

		case MOCAP_MAA:
			retcode = file_maa_read( o.get(), _data );
			break;

		case MOCAP_BVH:
			if( _b_use_gypsy )
				retcode = _gypsy_skel->get_data( _data, _real_by_node );
			else
			{
#if AAA_LIB_USE_ASSIMP()
				retcode = _mocap_reader->retrieve_data( _data, _real_by_node, 0 );
#else
				retcode = ERR_NOT_COMPILED;
#endif	//#if AAA_LIB_USE_ASSIMP()
			}

			_seq = _seq_array;
			_seq->_offset_abs = 0;
			_seq->_offset_rel = 0;
			_seq->_frame_nb_rel = _frame_nb;
			_seq->_frame_nb_abs = _frame_nb;
			_seq->_node_nb = _node_nb;
			_seq_nb = 1;
			break;
		}
	}
exit:
	//delete [] line;
	if( NOERR(retcode) )
	{
		if( _file_type != MOCAP_NATIVE )
		{
			CHAR			tmp_filename[AAA_PATH_MAX()];
			CHAR CONST *	pt;
			//	message
			GOOD_PRINT_STRING( "Loaded a MoCap : %d frames for %d nodes.", _frame_nb, _node_nb );
			//	place a relative path in the filename
			_mocap_filename.set_fname_relative( o );
			if( _file_type == MOCAP_BVH )
				pt = get_my_filename();
			else
				pt = o.get();
			if( pt )
			{
				fname::cpy_and_replace_ext( tmp_filename, pt, "segment.csv" );
				_segments.read_point_from_file_csv( tmp_filename );
			}
			if( _file_type == MOCAP_BVH && _segments.is_empty() )
			{
				if( _gypsy_skel )
				{
					_segments.init_bvh_def();
				}
				else
				{
#if AAA_LIB_USE_ASSIMP()
					INT32* array = _mocap_reader->get_segment_point_index();
					_segments.init_point_index_from_table( _mocap_reader->get_segment_nb(), array );
					_mocap_reader->free_segment_point_index( array );
#endif	//#if AAA_LIB_USE_ASSIMP()
				}
			}
			_seq->update();
			build_segment_weight();
			_b_valid_data = true;
//			HEAP_IS_CORRUPT();
		}
		else
		{

			_segments.init_kinect1();
		}
	}
	else
	{
		if( _file_type != MOCAP_NATIVE )
		{
			ERR_PRINT_STRING( "Could not read MoCap data %s.", o.get() );
		}
		dealloc();
		_b_valid_data = false;
	}
	return retcode;
}

FINLINE	void c_bdd_mocap::get_draw_node_start_stop( INT32& start, INT32& end )
{
	start	= _node_start;
	end		= _node_end;
	ORDER( start, end );
	CLAMP( start,	1, _node_nb );
	CLAMP( end,		1, _node_nb );
}

void	c_bdd_mocap::draw_number()
{
	if( !_b_valid_data )
		return;

	bool	b_rot_store = _b_rot;
	_b_rot = (_s_draw_number >4) && _b_rot_possible;

	GOL::color_white();

	INT32	i_beg;
	INT32	i_end;
	get_draw_node_start_stop( i_beg, i_end );
	for( INT32 i = i_beg; i<=i_end; ++i )
	{
		GOL::matrix::push();
			if( _s_draw_number != 4 )
				transfo_do( i );
			switch( _s_draw_number )
			{
			case 0:
				break;
			case 4:
				{
				REAL	f[3];
				get_tra_cur( f, i );
				GOL::matrix::translate3v( f );
				billboard::do_y( f );
				}
				break;
			case 1:
			case 5:
				GOL::matrix::rotate_y_deg( 90. );
				break;
			case 2:
			case 6:
				GOL::matrix::rotate_x_deg( -90. );
				GOL::matrix::rotate_z_deg( -90. );
				break;
			case 3:
			case 7:
				break;
			}
			GOL::matrix::scale( _size_number );
			//todo
			//	GOL::scale( size_speed_offset + size_speed_factor[0] * ABS(r[0]-rp[0]),
			//				size_speed_offset + size_speed_factor[1] * ABS(r[1]-rp[1]),
			//				size_speed_offset + size_speed_factor[2] * ABS(r[2]-rp[2])
			//				);
			aaa::alphabet::draw_int32( i, 0 );
		GOL::matrix::pop();
	}

	_b_rot = b_rot_store;
}

void	c_bdd_mocap::draw_origins()
{
	if( !_b_valid_data )
		return;

	INT32	i_beg;
	INT32	i_end;
	GOL::push_att();
	GOL::push_color();
		get_draw_node_start_stop( i_beg, i_end );
		for( INT32 i = i_beg; i<=i_end; ++i )
		{
			GOL::matrix::push();
				transfo_do( i );
				n_axe::draw_origin_geo( _size_origins );
			GOL::matrix::pop();
		}
	GOL::pop_att();
}

//todo move to segment
void	c_bdd_mocap::build_segment_weight()
{
	INT32 nb = _segments.get_nb();
	if( nb == 0 )
		return;

	INT32	ia, ib;
	REAL	max;

	_segment_len_default = _segment_len_default_ui;

	//	calculate dist
	for( INT32 id = 1; id <= nb; ++id )
	{
		_segments.get_segment_point_id( id, ia, ib );
		if( ia>=0 && ib>=0 )
		{
			max = dist_v3r( get_data( 0, ia ), get_data( 0, ia ) );
			if( max == 0. )
				max = _segment_len_default;
			_segments.set_weight( id, max );
		}
	}
	_segments.normalize_weight();
}

//todonow protect when gypsy data was not loaded
bool	c_bdd_mocap::get_point_rnd( REAL* CONST dst, REAL CONST when )
{
	return get_point_on_seg( dst, when, get_segment_rnd(), rand_segment_place.get_fp32_01() );
}

//todo	init segment_weight and check update strategy
INT32	c_bdd_mocap::get_segment_rnd()
{
	return _segments.get_id_rnd( _segment_start, _segment_start, _b_segment_only_hands_for_other_ui );
}

bool	c_bdd_mocap::get_point_and_speed_rnd_time_abs( REAL* CONST dst, REAL* CONST speed, REAL CONST when )
{
	if( _frame_native_nb < 2 )
	{
		clear_v3( dst );
		clear_v3( speed );
		return false;
	}

	INT32	i = get_segment_rnd();
	REAL	f = rand_segment_place.get_fp32_01();

	get_point_on_seg( dst,		when,					i, f );
	get_point_on_seg( speed,	when + _frame_length,	i, f );
		
	sub_then_scale_v3r( speed, dst, _frame_by_sec );
	//hack chacana
	/*	if( i == segment_end )
		{
			f = rand_segment_place.get_ufloat()*.66;
			pos_pt[1] += f;
			pos_pt[0] += rand_segment_place.get_fp32()*f;
			pos_pt[2] += rand_segment_place.get_fp32()*f;
		}
	*/
	return true;
}

bool	c_bdd_mocap::get_point_and_speed_rnd_time_rel( REAL* CONST dst, REAL* CONST speed, REAL CONST when )
{
	//todo the time here is strange, why ?
	return get_point_and_speed_rnd_time_abs( dst, speed, REAL(( _frame_cur*OVER_ONE(_frame_by_sec) + when ) * _time_factor) );
}

void	c_bdd_mocap::draw_segment()
{
	if( !_b_valid_data )
		return;

	REAL*	r1;
	REAL*	r2;
	INT32	ia,	ib;

	GOL::begin( GL_LINES );
		for( INT32 id = _segment_start; id<=_segment_end; ++id )
		{
			_segments.get_segment_point_id( id, ia, ib );
			if( ia>=0 && ib>=0 )
			{
				r1 = get_data_cur(ia) + _offset_tra;
				r2 = get_data_cur(ib) + _offset_tra;
				GOL::vertex3v( r1 );
				GOL::vertex3v( r2 );
			}
		}
	GOL::end();
}

void	c_bdd_mocap::draw_multiple()
{
	if( !_b_valid_data )
		return;

//hack moved after because it was fucking up the current data
//	if( _b_draw_path_ui )
//	{
//		draw_path_multiple();
//		return;
//	}

	INT32	color_mat;
	INT32	i_beg;
	INT32	i_end;
	get_draw_node_start_stop( i_beg, i_end );
	c_multiple::cur->set_nb( i_beg - i_end + 1 );

	if ( color_mat = c_render::get_cur()->get_random_on_color() )
		c_color_random::begin();
	/*		for( i = 1; i<=_node_nb; ++i )
			{
				c_tex_anim::cur->bind_next();
				if ( color_mat)
					do_random_on_color();
				GOL::matrix::push();
					transfo_do( i );
	//todo			GOL::scale(	size_speed_offset + size_speed_factor[0] * ABS(r[0]-rp[0]),
	//							size_speed_offset + size_speed_factor[1] * ABS(r[1]-rp[1]),
	//							size_speed_offset + size_speed_factor[2] * ABS(r[2]-rp[2])
	//							);
					GOL::call_list( get_render_list_multiple_name());
				GOL::matrix::pop();
			}
	*/
	//hack	should be cleaned extended....
	REAL	pos[3];
	REAL	rot[3];
	REAL	size[3];
	clear_v3( pos );
	clear_v3( rot );
	set_v3( size, 1. );

	for( INT32 i = i_beg; i <= i_end; ++i )
	{
		c_tex_anim::cur->bind_next();
		if ( color_mat )
			c_color_random::set();
		REAL*	pt = get_data_cur( i );
		if( _b_tra )
			cpy_v3( pos, pt + _offset_tra );

		if( _b_rot )
		//if( _offset_mat >= 0 )
		//	do_matrix( pt+_offset_mat);
		//else
			cpy_v3( rot, pt + _offset_rot );
		c_multiple::cur->set_index( i );
//maa 2014 march removed this
/*
		if( _b_sca )
		{
			REAL*	p = pt + _offset_sca;
			size[0] = *(p+1);
			size[1] = *(p+2);
			size[2] = *(p+0);
			//cpy_v3( size, pt+_offset_sca);
			c_multiple::cur->align_then_draw( pos, size );
		}
		else
*/
		c_multiple::cur->align_then_draw( pos );
	}
	if ( color_mat )
		c_color_random::end();

//hack moved after because it was fucking up the current data
//	infact it seems there is a problem with frame_in / frame_out
	if( _b_draw_path_ui )
	{
		draw_path_multiple();
		return;
	}

}

void	c_bdd_mocap::draw_path_multiple()
{
	if( !_b_valid_data )
		return;

	INT32	i_beg;
	INT32	i_end;
	get_draw_node_start_stop( i_beg, i_end );
	c_multiple::cur->set_nb( i_beg - i_end + 1 );

	INT32	color_mat = c_render::get_cur()->get_random_on_color();
	if ( color_mat )
		c_color_random::begin();
	REAL	ds;
	REAL	stop;
	REAL	start;
	REAL	point[3];
	//INT32	no;

	stop = _path_stop;
	start = _path_start;
	ORDER( start, stop );

	ds = stop-start;
	if( ds <= 0. )	//hack
		ds = 1.;
	ds *= OVER_ONE_AS_REAL( _path_resolution * _frame_nb );

	//if( _b_playing )
	{
		stop += _phase;
		start += _phase;
	}
	REAL	d2 = _path_dist_max*_path_dist_max;
	for( INT32 i=i_beg; i<=i_end; ++i )
	{
		c_tex_anim::cur->bind_next();
		if ( color_mat )
			c_color_random::set();
		c_multiple::cur->set_index( i );
		REAL last[3];
		clear_v3( last );
		for( REAL s = start; s<=stop; s+=ds )
		{
			get_tra( point, i, s );
			if( is_dist_squared_less_v3r( point, last, d2 ) )
				c_multiple::cur->align_then_draw( point );
			cpy_v3( last, point );
		}
	}
	if ( color_mat )
		c_color_random::end();
}

void	c_bdd_mocap::draw_path()
{
	if( !_b_valid_data )
		return;

	REAL	ds;
	REAL	stop;
	REAL	start;
	REAL	point[3];
	//INT32	no;

	stop = _path_stop;
	start = _path_start;
	ORDER( start, stop );

	ds = stop-start;
	if( ds <= 0. )	//hack
		ds = 1.;
	ds *= OVER_ONE_AS_REAL( _path_resolution * _frame_nb );

	//if( _b_playing )
	{
		stop += _phase;
		start += _phase;
	}
	INT32	i_beg;
	INT32	i_end;
	get_draw_node_start_stop( i_beg, i_end );
	for( INT32 no = i_beg; no <= i_end; ++no )
	{
		GOL::begin( GL_LINE_STRIP);
		for( REAL s = start; s <= stop; s += ds )
		{
			get_tra( point, no, s );
			GOL::vertex3v( point );
		}
		GOL::end();
	}
}

void	c_bdd_mocap::draw_single()
{
	if( !_b_valid_data )
		return;

	GOL::push_att();

		GOL::push_lighting( false );
		GOL::push_color();

		if( _b_draw_path_ui )
			draw_path();

		if( _b_draw_origins_ui )
			draw_origins();
		if( _b_draw_segment_ui )
			draw_segment();
		if( _s_draw_number )
			draw_number();

	GOL::pop_att();
}

AAA_ERR	c_bdd_mocap::file_maa_count_one( FILE* file, INT32* ch_nb, INT32* start, INT32* stop )
{
	INT32	l_start;
	INT32	l_stop;
	INT32	l_ch_nb;
	INT32	tmp;
	AAA_ERR	retcode = ERR_ANY;
	char	buf[256];

	for(;;)
	{
		tmp = fscanf( file, "%256s", buf );
		if( tmp == 1 )
		{
			if( str_is_equal_nocase( buf, "frame_start" ) )
				break;
			else if( str_is_equal_nocase( buf, "origin" ) )
			{
				tmp = fscanf( file, "%256s", buf );
				if( tmp == 1 )
				{
					tmp = fscanf( file, "%256s", buf );
					if( tmp == 1 )
					{
						if( str_is_equal_nocase( buf, "max" ) )
							_s_origin = FROM_MAX;
						else if( str_is_equal_nocase( buf, "softimage" ) )
							_s_origin = FROM_SOFTIMAGE;
					}
					else
						goto exit;
				}
			}
		}
		else
			goto exit;
	}
	//	tmp = stream_read_until_str_nocase( file, "frame_start" );	
	//	if( tmp == 1)
	{
		tmp = fscanf( file, " %d frame_end %d nodes %d", &l_start, &l_stop, &l_ch_nb );
		if( tmp == 3 )
		{
			*start = l_start;
			*stop = l_stop;
			*ch_nb = l_ch_nb;
			retcode = AAA_OK;
		}
	}
exit:
	return retcode;
}

AAA_ERR	c_bdd_mocap::file_maa_count_one( C_PCHAR_C filename, INT32 *ch_nb, INT32 *start, INT32 *stop )
{
	FILE*	file;
	AAA_ERR	retcode = ERR_ANY;
	if( filename )
	{
		CHAR	fname[AAA_PATH_MAX()];
		if( fname::have_ext( filename ) )
			strcpy( fname, filename );
		else
		{
			fname::add_ext( fname, filename, "mocap.maa" );
		}
		//	open file
		file = c_file::FOPEN( fname, "rt" );
		if( !file )
		{
			ERR_PRINT_STRING( "Can't Open maa file %s.", fname );
		}
		else
		{
			retcode = file_maa_count_one( file, ch_nb, start, stop );
			if( ERR(retcode) )
			{
				ERR_PRINT_STRING( "Can't Open read header in file %s.", fname );
			}
			else
			{
				GOOD_PRINT_STRING( "Counted a MoCap : start %d stop on %d node.", *start, *stop, *ch_nb );
			}
			c_file::FCLOSE(file);
		}
	}
	return retcode;
}

//todo make parsing fn like fscanf for grid
//	use <stl> directly
AAA_ERR	c_bdd_mocap::file_maa_count( C_PCHAR_C filename, INT32 *nb_ch, INT32 *nb_frame )
{
	FILE	*file;
	INT32	key_nb = 0;
	AAA_ERR	retcode = ERR_ANY;
	INT32	count_frame = 0;
	INT32	count_node = 0;

	CHAR	fname[AAA_PATH_MAX()];
	CHAR	name[AAA_PATH_MAX()];

	_seq_nb = 0;
	_seq = _seq_array;

	fname::cpy_and_replace_ext( fname, filename, "datagrid" );
	//	open file
	file = c_file::FOPEN( fname, "rt" );
	if( !file )
	{
		INT32	start;
		INT32	stop;
		INT32	node;
		ERR_PRINT_STRING( "No datagrid file %s.", fname );
		retcode = file_maa_count_one( filename, &node, &start, &stop );
		if( NOERR(retcode) )
		{
			_seq->_offset_abs = count_frame;
			_seq->_offset_rel = start;
			_seq->_frame_nb_rel = stop-start + 1;
			_seq->_frame_nb_abs = stop-start + 1;
			_seq->_node_nb = node;
			count_frame = stop-start+1;
			count_node = node;
			_seq_nb = 1;
			*nb_ch = count_node;
			*nb_frame = count_frame;
		}
	}
	else
	{
		CHAR	dir[AAA_PATH_MAX()];
		//	change directory
		fname::cpy_dir( dir, fname );	//todo do better
		c_dir::push_def( dir );

		while( true )
		{
			INT32	start_grid = 0;
			INT32	stop_grid = 0;
			INT32	start_file;
			INT32	stop_file = 0;
			INT32	node = 0;
			INT32	tmp = fscanf( file, "%260s %d %d\n", name, &start_grid, &stop_grid );
			if( tmp == 3 )
			{
				if( str_is_equal_nocase( name, "idem" ) )
				{	//todo check idem first
					_seq->_offset_abs = (_seq-1)->_offset_abs;
					_seq->_offset_rel = start_grid - start_file;
					_seq->_frame_nb_rel = stop_grid - start_grid + 1;
					_seq->_frame_nb_abs = stop_file - start_file + 1;
					_seq->_start_file = start_file;
					_seq->_stop_file = stop_file;
					_seq->_node_nb = node;
				}
				else
				{
					retcode = file_maa_count_one( name, &node, &start_file, &stop_file );
					if( NOERR(retcode) )
					{
						_seq->_offset_abs = count_frame;
						_seq->_offset_rel = start_grid - start_file;
						_seq->_frame_nb_rel = stop_grid - start_grid + 1;
						_seq->_frame_nb_abs = stop_file - start_file + 1;
						_seq->_start_file = start_file;
						_seq->_stop_file = stop_file;
						_seq->_node_nb = node;
						count_frame += stop_file - start_file + 1;
						count_node = MAX( count_node, node );
					}
				}
				if( _seq->_frame_nb_rel > _seq->_frame_nb_abs )
				{
					ERR_PRINT_STRING( "sequence %d is long of %d frame when file is %d frame long\nAAASeed will change this sequence",
									_seq_nb, _seq->_frame_nb_rel, _seq->_frame_nb_abs );
					_seq->_frame_nb_rel = _seq->_frame_nb_abs - 1;
					_seq->_start_file = 0;
					_seq->_stop_file = _seq->_frame_nb_abs - 1;
				}
			}
			else if( tmp == EOF )
			{
				*nb_ch = count_node;
				*nb_frame = count_frame;
				retcode = AAA_OK;
				break;
			}
			else
			{
				ERR_PRINT_STRING( "Can't Parse datagrid file %s.", fname );
				c_dir::pop_def();
				goto exit;
			}
			++_seq_nb;
			++_seq;
		}
		c_dir::pop_def();
	}
exit:
	_seq = _seq_array;
	if ( file )
		c_file::FCLOSE( file );

	return retcode;
}

//todo deal with size
bool	stream_read_line( FILE* stream, CHAR* dst, INT32 size )
{
	INT32	c;
	for(;;)
	{
		c = getc( stream );
		if( c == 0x0a  )
		{
			*dst = 0;
			return true;
		}
		else if( c == EOF )
		{
			*dst = 0;
			return true;
		}
		*dst++ = c;
	}
	return false;
}

AAA_ERR	c_bdd_mocap::file_maa_read_one( FILE* file, REAL* data, INT32 frame_start, INT32 frame_stop )
{
	AAA_ERR	retcode = ERR_ANY;
	INT32	tmp;
	REAL*	pt_frame;
	REAL*	pt_node;
	REAL*	pt;
	INT32	frame;
	INT32	nb_ch = _seq->_node_nb;
	INT32	node_nb;
	INT32*	node_index = nullptr;

	INT32	spin[4] = {0,0,0,0};
	DOUBLE	f[3];

	tmp = stream_read_until_str_nocase( file, "frame_start" );
	if( tmp != 1)
		goto error;
	tmp = stream_read_until_str_nocase( file, "frame_end" );
	if( tmp != 1)
		goto error;
	tmp = stream_read_until_str_nocase( file, "nodes" );
	if( tmp != 1)
		goto error;
	tmp = fscanf( file, " %d", &node_nb );
	if( tmp != 1)
		goto error;

	node_index = new INT32[node_nb];
	if( !node_index )
		goto end;
	for ( INT32 i = 0; i < node_nb; ++i )
	{
		INT32	nindex;
		CHAR	node_str[AAA_PATH_MAX()];

		node_index[i] = -1;
		tmp = stream_read_until_str_nocase( file, "node" );
		tmp = fscanf( file, "%d \"%260s\"", &nindex, node_str );
		if( tmp != 2)
			goto error;
		if ( node_str[strlen(node_str)-1] == '\"' )
			node_str[strlen(node_str)-1] = 0;
		if( str_is_equal_nocase( (CONST char*)node_str, "cam", 3 ) )
		{
			node_index[nindex] = _node_name->find( node_str );
		}
		else
		{
			CHAR*	p_str = node_str;
			INT32	nb;
			while ( *p_str != '_' )
			{
				if( *p_str == 0 )
					break;
				++p_str;
			}
			*p_str = 0;
			node_index[nindex] = _node_name->find( node_str );
			*p_str = '_';
			nb = (*(p_str+1)-'0')*10+*(p_str+2)-'0';
			_node_bdd_index[_seq_nb*node_nb + node_index[nindex]]=nb;
			if ( nb )
				GOOD_PRINT_STRING( "Non zero node %s", node_str );
		}
		if( node_index[nindex] < 0 )
		{
			debug_break( "Bug in c_bdd_mocap::file_maa_read_one()" );
		}
	}

	INT32	j;
	for( j = frame_start; j <= frame_stop; ++j )
	{
		tmp = stream_read_until_str_nocase( file, "frame" );
		if( tmp != 1 )
			goto error;

		tmp = fscanf( file, "%d\n", &frame );
		if( tmp != 1 && frame != j )
			goto error;
		
		pt_frame = data + ( j - frame_start ) * _real_by_frame;
		pt_node = pt_frame;
		for( INT32 i = 0; i < node_nb; ++i )
		{
			clear_v3(	pt_node + _offset_rot );
			clear_v3(	pt_node + _offset_tra );
			set_v3(		pt_node + _offset_sca,	1. );
			pt_node += _real_by_node;
		}
		pt_node = pt_frame;
		CHAR buf[512];
		c_asc_parser parser;
		for ( INT32 i = 0; i < nb_ch; ++i )
		{
			if( !stream_read_line( file, buf, 512 ) )
				goto error;

			parser.set_start( (CHAR*) buf );	//	cast because parser change temporary char value

			while( true )
			{
				CHAR* command = parser.get_next_word();
				if( !command )
					break;
				if( !parser.get_next_double(	f[0] ) )	goto error;
				if( !parser.get_next_double(	f[1] ) )	goto error;
				if( !parser.get_next_double(	f[2] ) )	goto error;

				//tmp = sscanf( buf, "%255s %f %f %f", command, f, f+1, f+2 );
				//if( tmp != 4 )
				//{
				//	if( tmp==0 )
				//		break;
				//	else
				//		goto error;
				//}

				str_convert_to_lowercase( command );
				if( str_is_equal( command, "rot" ) )
				{
					/*
					switch( _s_origin )
					{
					case FROM_MAX:			tmp = fscanf( file, " %f %f %f", f, f+1, f+2 );	break;
					case FROM_SOFTIMAGE:	tmp = fscanf( file, " %f %f %f", f, f+1, f+2 );	break;
					}
					if( tmp != 3 )
						goto error;
					*/
					pt = pt_node + _offset_rot;
					scale_v3( pt, f, REAL(1./360.) );

					if( j != frame_start )	//	deal with rot jumping from 180. to -180.
					{					//	so we can interpolate
						//INT32	d;
						REAL*	prev = pt - _real_by_frame;
						REAL	f1;
						//REAL	n,p;
						for( INT32 d = 0; d < 3; ++d )
						{
							//n = *pt;
							//p = *prev;
							f1 = *pt - *prev;
							if( f1 > .5 )	//	the only solution is quaternion
							{	//algo angle cont
								*pt -= F_FLOOR( f1 + .5 );
								++spin[d];
							}
							else if( f1 < -.5 )
							{
								*pt -= CEIL( f1 - .5 );
								++spin[d];
							}
							++pt;
							++prev;
						}
					}
				}
				else if( str_is_equal( command, "tra" ) )
				{
					/*
					tmp = fscanf( file, "%f %f %f", f, f+1, f+2 );
					if( tmp != 3 )
						goto error;
					*/
					pt = pt_node + _offset_tra;
					switch( _s_origin )
					{
					case FROM_MAX:			*(pt)   =  REAL(*(f));
											*(pt+1) =  REAL(*(f+2));
											*(pt+2) = -REAL(*(f+1));
											break;
					case FROM_SOFTIMAGE:	cpy_v3( pt, f );
											break;
					}
				}
				else if( str_is_equal( command, "sca" ) )
				{
					/*
					tmp = fscanf( file, "%f %f %f", f, f+1, f+2 );
					if( tmp != 3 )
						goto error;
					*/
					pt = pt_node + _offset_sca;
					switch( _s_origin )
					{
					case FROM_MAX:			*(pt)   =  REAL(*(f));
											*(pt+1) =  REAL(*(f+2));
											*(pt+2) = -REAL(*(f+1));
											break;
					case FROM_SOFTIMAGE:	cpy_v3( pt, f );
											break;
					}
				}
			}
			pt_node += _real_by_node;
		}
	}
error:
	SAFE_DELETE_ARRAY( node_index );

end:
	if ( j == frame_stop + 1 )
	{
		GOOD_PRINT_STRING( "Compensate %d times -> Spin %d %d %d", spin[0] + spin[1] + spin[2], spin[0], spin[1], spin[2] );
		retcode = AAA_OK;
	}
	else
	{
		ERR_PRINT_STRING( "pb reading maa mocap file" );
	}
	return retcode;
}

AAA_ERR	c_bdd_mocap::file_maa_read_one( C_PCHAR_C filename, REAL* data, INT32 frame_start, INT32 frame_stop )
{
	//char	fname[AAA_PATH_MAX()];
	//char	root[AAA_PATH_MAX()];
	//char	ext[AAA_PATH_MAX()];
	//INT32	number_len;
	FILE*	file;
	AAA_ERR	retcode=AAA_OK;
	//INT32	i;	
	/*	fname::serie_split( (char*) filename, root, &i, &number_len, ext);
		for(i=0;i<nb_pt; ++i )
			{
			//hack
			if(	nb_pt != 1)
				{
				fname::serie_make( fname, root, i+1, number_len, ext);
				//	open file
				file = c_file::FOPEN( fname, "rt");
				}
			else
	*/
	CHAR	fname[AAA_PATH_MAX()];
	if( fname::have_ext( (char*) filename) )
		strcpy( fname, filename );
	else
	{
		fname::add_ext( fname, filename, "mocap.maa" );
	}
	file = c_file::FOPEN( fname, "rt" );
	if( !file )
	{
		ERR_PRINT_STRING( "Can't Open maa file %s.", fname );
	}
	else
	{
		retcode = file_maa_read_one( file, data, frame_start, frame_stop );
		c_file::FCLOSE(file);
	}
	return retcode;
}

AAA_ERR	c_bdd_mocap::file_maa_read( C_PCHAR_C filename, REAL* data )
{
	FILE	*file;
	INT32	key_nb = 0;
	AAA_ERR	retcode = ERR_ANY;

	CHAR	fname[AAA_PATH_MAX()];
	CHAR	name[AAA_PATH_MAX()];

	_seq_nb = 0;
	_seq = _seq_array;

	fname::cpy_and_replace_ext( fname, filename, "bind" );
	_node_name->load_from_file( fname );

	fname::cpy_and_replace_ext( fname, filename, "datagrid" );
	//	open file
	file = c_file::FOPEN( fname, "rt" );
	if( !file )
	{
		ERR_PRINT_STRING( "No datagrid file %s.", fname );
		retcode = file_maa_read_one( filename, data, 0, _frame_nb-1 );
		if( NOERR(retcode) )
			_seq_nb = 1;
	}
	else
	{
		INT32	tmp;
		INT32	count_frame = 0;
		INT32	count_node = 0;
		char	dir[AAA_PATH_MAX()];

		//	change directory
		fname::cpy_dir( dir, fname );	//todo do better
		c_dir::push_def( dir );
				
		while( true )
		{
			INT32	start_grid;
			INT32	stop_grid;
			tmp = fscanf( file, "%260s %d %d\n", name, &start_grid, &stop_grid );
			if( tmp == 3 )
			{
				if ( str_is_equal_nocase( name, "idem") )
				{
					INT32	*p;
					INT32	i;
					p = &_node_bdd_index[_seq_nb*_node_nb];
					for ( i=0; i<_node_nb; ++i )
					{
						*p = *(p-_node_nb);
						++p;
					}
				}
				else
				{
					retcode = file_maa_read_one( name, data+_seq->_offset_abs*_real_by_frame, _seq->_start_file, _seq->_stop_file );
					if( ERR(retcode) )
					{
						c_dir::pop_def();
						goto exit;
					}
				}
			}
			else if( tmp == EOF )
			{
				break;
			}
			else
			{
				ERR_PRINT_STRING( "Can't Parse datagrid file %s.", fname );
				c_dir::pop_def();
				goto exit;
			}
			++_seq_nb;
			++_seq;
		}

		c_dir::pop_def();
	}
	//{
	//INT32	s;
	//INT32	no;
	INT32*	pt;
	for( INT32 no = 0; no < _node_nb; ++no )
	{
		pt = &_node_bdd_index[no];
		_node_bdd_multi[no] = false;
		for( INT32 s = 0; s < _seq_nb; ++s )
		{
			if( *pt != 0)
			{
				_node_bdd_multi[no] = true;
				break;
			}
			pt += _node_nb;
		}
	}
	//}
exit:
	_seq = _seq_array;
	if( file )
		c_file::FCLOSE( file );

	return retcode;
}

static	void make_name( CHAR* dst, C_PCHAR_C base, INT32 number )
{
	CHAR*	pt;
	strcpy( dst, base );
	pt = dst+strlen( dst );
	*pt++ = '_';
	strnum::make( pt, 2, number );
	*(pt+2) = 0;
}

void	c_bdd_mocap::set_seq( INT32 index )
{
	if( index < 0 && index >= _seq_nb )
		index = 0;

	if( _seq_cur == index )
		return;

	CHAR	str[256];
	for( INT32 no = 0; no<_node_nb; ++no )
	{
		if( _node_bdd_multi[no] )	//todo check this and get_layers_from_name_short philo
		{
			c_layers*	p;
			if( _seq_cur >= 0 )
			{
				make_name( str, _node_name->get_str(no), _node_bdd_index[_seq_cur*_node_nb+no] );
				if( p = c_module::get_cur()->layers_get_from_name_short(str) )
				{
					p->set_active(false);
					SWITCH_PRINT_STATE( str, false );
				}
			}
			make_name( str, _node_name->get_str(no), _node_bdd_index[index*_node_nb+no] );
			if( p = c_module::get_cur()->layers_get_from_name_short(str) )
			{
				p->set_active(true);
				SWITCH_PRINT_STATE( str, true );
			}
		}
	}
	special_mocap_set_seq( index );
	_seq_cur_ui = _seq_cur = index;
	_seq = &_seq_array[_seq_cur];

	get_param( n_bdd_mocap::INDEX_FRAME_CUR_PARAM )->set_max( REAL(_seq->_frame_nb_rel) );
}

INT32	c_bdd_mocap::get_point_nb()								{	return _node_nb;			}
bool	c_bdd_mocap::get_point( REAL* dst, INT32 CONST index )
{
	get_tra_cur( dst, index+1 );
	return true;
}

INT32	c_bdd_mocap::get_segment_nb()
{
	return _segments.get_nb();
}
void	c_bdd_mocap::get_segment( REAL* a, REAL* b, INT32 seg_id )
{
	//todo avoid interpolation
	//this is fast and dirty

	INT32 ida, idb;
	_segments.get_segment_point_id( seg_id, ida, idb );
	if( ida == idb )
	{
		clear_v3( a );
		clear_v3( b );
	}
	else
	{
		get_tra_cur(	a,	ida	);
		get_tra_cur(	b,	idb	);
	}
}

