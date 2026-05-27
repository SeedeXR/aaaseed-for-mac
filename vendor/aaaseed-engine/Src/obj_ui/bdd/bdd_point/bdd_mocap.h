
#ifdef AAA_BDD_MOCAP_H
#error "BDD_MOCAP_H included more than once."
#endif
#define AAA_BDD_MOCAP_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_GOL_MATRIX_H
#	include "gol/gol_matrix.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif
#ifndef AAA_AAA_CONST_H
#	include "infrastructure/aaa_const.h"
#endif
#ifndef AAA_AAA_MUTEX_H
#	include "aaa/aaa_mutex.h"
#endif

class	c_bind;
class	c_gypsy_skel;

namespace mocap
{
	class c_bvh_data;
}

CONST	INT32	MOCAP_SEQ_NB_MAX = 32;

class c_seq_mocap final : public c_obj
{
public:
	INT32	_node_nb;
	INT32	_offset_abs;
	INT32	_offset_rel;
	INT32	_start_file;
	INT32	_stop_file;
	INT32	_frame_nb_rel;
	INT32	_frame_nb_abs;
	bool	_b_loop;
	INT32	_frame_in;
	INT32	_frame_out;
	INT32	_frame_nb_to_play;

	c_seq_mocap();
			void	update();
	FINLINE INT32	get_frame_start()	{	return _frame_in;	}
};

class c_mocap_segment final : public c_obj
{
public:
	//todo these two should be dynamic
	static CONST	INT32	SEGMENT_NB_MAX = 64;
private:
	INT32	_nb;
	INT32	_point_index[SEGMENT_NB_MAX][2];
	REAL	_weight[SEGMENT_NB_MAX];			//todo init
public:
	c_mocap_segment();

			void	init_point_index_from_table( INT32 nb, INT32* table );

			void	init_kinect1();
			void	init_kinect2();
			void	init_bvh_def();

			void	read_point_from_file_csv( C_PCHAR_C filename );

			void	normalize_weight();

	FINLINE	bool	is_empty()							{	return _nb==0;					}
	FINLINE	INT32	get_nb()							{	return _nb;						}
	FINLINE	INT32	clamp_id( INT32 id )				{	return CLAMP( id, 1, _nb );		}
	FINLINE	bool	is_id_valid( INT32 id )				{	return 0 < id && id <= _nb ;	}

			void	get_segment_point_id( INT32 seg_id, INT32& id_a, INT32& id_b );

	FINLINE	void	set_weight( INT32 id, REAL val )	{	id = clamp_id(id); _weight[id-1] = val;		}
	FINLINE	REAL	get_weight( INT32 id )				{	id = clamp_id(id); return _weight[id-1];	}
			INT32	get_id_rnd( INT32 id_begin, INT32 id_end, bool b_only_hands_for_other );
};

enum	MOCAP_TYPE : INT32
{
	MOCAP_NATIVE = 0, 
	MOCAP_TRC,
	MOCAP_HRC_SIGAUD,
	MOCAP_MAA,
	MOCAP_BVH,
	MOCAP_END_ENUM
};

enum	DATA_ORIGIN : INT32
{
	NATIVE = 0,
	FROM_MAX,
	FROM_SOFTIMAGE,
	FROM_BVH
};



class	c_bdd_mocap final
	: public c_bdd_multiple
{
	FACTORY_DECLARE(c_bdd_mocap,c_bdd_multiple);
public:
	static	CONST	INT32	MOCAP_CHANNEL_NB_MAX = CHANNEL_NB_MAX;
private:
	static	c_instance_by_channel< c_bdd_mocap, MOCAP_CHANNEL_NB_MAX > inst_by_channel;
public:
	static	c_bdd_mocap*	get_from_channel( INT32 channel_id )	{	return inst_by_channel.get( channel_id );	}
public:
	static	c_bdd_mocap*	def;
	static	c_bdd_mocap*	cur;
	static	c_bdd_mocap*	ui;

private:
	INT32				_channel_id;		//	use to find mocap object by Channel		

	bool				_b_valid_data;
	REAL*				_data;
	REAL*				_data_live_past;
	REAL*				_data_live;
	REAL*				_data_cur;
	REAL*				_len_by_node;

	//record position (search for native)
	INT32				_frame_native_offset;
	INT32				_frame_native_nb;
	bool				_b_store_native;
	bool				_b_use_gypsy;

	INT32				_frame_nb;
	INT32				_frame_nb_ui;
	REAL				_frame_cur;

	INT32				_node_nb;
	INT32				_node_nb_ui;
	REAL				_frame_by_sec_source;

	REAL				_time_factor;
	REAL				_time_factor_ui;
	REAL				_frame_by_sec;
	REAL				_frame_length;
	REAL				_time_len;
	REAL				_phase;
	REAL				_phase_last;

	bool				_b_live_ui;
	c_delta_t			_delta_t;

	bool				_b_draw_segment_ui;

	c_mocap_segment		_segments;

	INT32				_node_start;
	INT32				_node_end;
	INT32				_segment_start;
	INT32				_segment_end;
	REAL				_segment_len_default;
	REAL				_segment_len_default_ui;
	bool				_b_segment_only_hands_for_other_ui;

	INT32				_s_draw_number;
	bool				_b_draw_origins_ui;
	REAL				_size_number;
	REAL				_size_origins;

	bool				_b_draw_path_ui;
	REAL				_path_resolution;
	REAL				_path_start;
	REAL				_path_stop;

	UINT32				_real_by_node;
	UINT32				_real_by_frame;
	INT32				_offset_tra;
	INT32				_offset_rot;
	INT32				_offset_mat;
	INT32				_offset_sca;
	INT32				_offset_s;

	bool				_b_interpolate;
	bool				_b_interpolate_rot;
	bool				_b_tra;
	bool				_b_rot;
	bool				_b_sca;
	bool				_b_tra_ui;
	bool				_b_rot_ui;
	bool				_b_sca_ui;
	bool				_b_rot_possible;
	bool				_b_tra_possible;
	bool				_b_sca_possible;

	REAL				_origin_ui[3];
	REAL				_origin[3];
	REAL				_tra_scale_ui[4];
	REAL				_tra_scale[3];

	INT32				_seq_nb;
	INT32				_seq_cur;
	INT32				_seq_cur_ui;
	bool				_b_playing;
	bool				_b_restart_trig_ui;
	bool				_b_reference_trig;
	bool				_b_clear_data_trig_ui;

	MOCAP_TYPE			_file_type;
	INT32				_s_origin;
	REAL				_origin_on_node[3];
	REAL				_origin_on_node_ui[4];
	INT32				_origin_on_node_id;
	o_str				_mocap_filename;
	o_str				_actor_filename;
	bool				_b_mocap_loaded;
	bool				_b_actor_loaded;	//todo why unused ?

	INT32				_net_channel;
	bool				_b_net_send_ui;

	mutable aaa::MUTEX	_data_lock;
	REAL				_path_dist_max;
	
	c_gypsy_skel*		_gypsy_skel;
	mocap::c_bvh_data*	_mocap_reader;

			void	init();
			void	init_pt_data();

public:
	c_bind*			_node_name;
	INT32*			_node_bdd_index;
	bool*			_node_bdd_multi;

	c_seq_mocap*	_seq;
	c_seq_mocap		_seq_array[MOCAP_SEQ_NB_MAX];	//	store all the sequence
													//todoq should be dynamic
			void	push_data_low(		REAL* buf,			INT32 len );
			void	push_data();

			bool	alloc(				INT32 nb_frame_in,	INT32 node_nb_in ); 
			void	dealloc();
			void	clear_data();

	virtual	void	param_init_pt();
	virtual	void	param_init();
	virtual	void	update();
	virtual	void	restart();
			void	center();
	virtual	void	draw_single();
	virtual	void	draw_multiple();
			void	draw_origins();
			void	draw_number();
			void	draw_segment();
			void	draw_path();
private:
			bool	get_point_on_seg(	REAL* CONST pt_in, REAL CONST when, INT32 CONST seg_id, REAL CONST where );
			INT32	get_segment_rnd();
	FINLINE	void	get_draw_node_start_stop( INT32& start, INT32& stop );
			void	draw_path_multiple();


	FINLINE	INT32	get_node_offset(	INT32 node_id )
					{
						if( is_valid_node( node_id ) )
							return (node_id-1) * _real_by_node;
						else
							return 0;
					}
	FINLINE	REAL*	get_data_node(		REAL* data,	INT32 node_id )	{	return data + get_node_offset( node_id );	}
	//cache this for speed
	FINLINE INT32	get_frame_nb_to_play()			{	return _seq->_frame_nb_to_play;		}
	FINLINE INT32	get_frame_start()				{	return _seq->get_frame_start();		}
	FINLINE INT32	compute_frame( INT32 frame )
					{	return get_frame_start() + IMOD( frame + _frame_native_offset, get_frame_nb_to_play() );	}
	FINLINE INT32	compute_frame_from_phase_no_in( REAL phase )
					{	return I_FLOOR( FMOD( phase ) * get_frame_nb_to_play() );		}
	FINLINE INT32	compute_frame_from_phase( REAL phase )
					{	return get_frame_start() + compute_frame_from_phase_no_in( phase );		}


public:
	virtual	bool	get_point_rnd(						REAL* CONST dst,				    REAL CONST when ) final override;
	virtual	bool	get_point_and_speed_rnd_time_abs(	REAL* CONST dst, REAL* CONST speed,	REAL CONST when ) final override;
	virtual	bool	get_point_and_speed_rnd_time_rel(	REAL* CONST dst, REAL* CONST speed,	REAL CONST when ) final override;
			void	build_segment_weight();

	virtual	AAA_ERR	load_do_after(				o_str CONST & filename	);
			AAA_ERR	read_point_from_stream(		FILE* file,					INT32 nb );
			AAA_ERR	read_data_from_trc_stream(	FILE* file					);

	virtual	AAA_ERR	load_data_from_filename(	o_str CONST & filename, INT32 type_io );
	virtual	AAA_ERR	load_data();
			AAA_ERR	load_data_from_file(		CONST CHAR *CONST filename );
			AAA_ERR	load_actor_from_file(		CONST CHAR *CONST filename );

	FINLINE	bool	is_valid_node(		INT32 node_id )
					{
#if	AAA_DEBUG()
						//todoqq
						//		if( node_id < 1 || node_id > _node_nb || _seq->len_rel == 0)
						//		debug_break( "%s() invalid node id", __FUNCTION__ );
#endif
						return INSIDE_MIN_MAX( node_id, 1, _node_nb );
					};

	FINLINE	REAL*	get_data(			INT32 frame );
	FINLINE	REAL*	get_data_index(		INT32 index );
	FINLINE	REAL*	get_data(			INT32 frame,	INT32 CONST node_id	)	{	return get_data_node(	get_data(frame),	node_id );	}
	FINLINE	REAL*	get_data_cur(						INT32 CONST node_id	)	{	return get_data_node(	_data_cur,			node_id );	}

	FINLINE	bool	get_data_cur_and_next( REAL* &cur, REAL* &next, REAL &frame, INT32 CONST node_id );
			void	build_data(			REAL* dst_data,	REAL src_frame, INT32 src_node_start, INT32 src_node_stop );

	FINLINE	void	get_tra_cur(		REAL* CONST dst,	INT32 CONST node_id	);
#if	!AAA_REAL_IS_DOUBLE()
	FINLINE	void	get_tra_cur(		DOUBLE* CONST dst,	INT32 CONST node_id	);
#endif
	FINLINE	void	get_sca_cur(		DOUBLE* CONST dst,	INT32 CONST node_id	);
	FINLINE	void	get_axe_cur(		DOUBLE* CONST dst,	INT32 CONST node_id,	INT32 CONST axe );
public:
			void	get_matrix_rows(		REAL* CONST a,	REAL* CONST b,	REAL* CONST c,	INT32 CONST node_id	);

	FINLINE	void	do_translate(			REAL CONST * CONST tra );
	FINLINE	void	do_rotate(				REAL CONST * CONST rot );
	FINLINE	void	do_scale(				REAL CONST * CONST sca );
	FINLINE	void	do_matrix_from_mat3(	REAL CONST * CONST mat );

			void	transfo_do(			INT32 CONST node_id	);

private:
	FINLINE	void	coor_to_world_one_low(			REAL* dst,				INT32 CONST node_id,	REAL CONST * CONST src	);
	FINLINE	void	get_point_tangent_raw_no_check(	REAL* dst, REAL* tgn,	INT32 CONST node_id,	REAL phase_in		);
	FINLINE	void	get_point_tangent_no_check(	REAL* dst, REAL* tgn,	INT32 CONST node_id,	REAL phase_in		);
public:
			void	coor_to_world_one(				REAL* dst,				INT32 CONST node_id,	REAL CONST * CONST src		);
			void	coor_to_world(					REAL* dst,				INT32 CONST node_id,	REAL CONST *       src,	INT32 nb=1	);
			void	get_tra(						REAL* dst,				INT32 CONST node_id,	REAL CONST phase_in		);
			void	get_sca(						REAL* dst,				INT32 CONST node_id,	REAL CONST phase_in		);
			void	get_tangent(					REAL* dst,				INT32 CONST node_id,	REAL CONST phase_in		);
#if	!AAA_REAL_IS_DOUBLE()
			void	get_tangent(					REAL* dst,				INT32 CONST node_id						);	
#endif
			void	get_tangent(					DOUBLE* dst,			INT32 CONST node_id						);
	FINLINE	void	get_point_tangent(				REAL* dst, REAL* tgn,	INT32 CONST node_id,	REAL phase_in	)
					{
						if( _b_valid_data )
							get_point_tangent_no_check( dst, tgn, node_id, phase_in );
					}

			REAL	compute_len(		INT32 node_id	);
			REAL	get_len(			INT32 node_id	);

	//count
			AAA_ERR	file_maa_count_one(	C_PCHAR_C filename,	INT32* node_nb,	INT32* start,	INT32* stop	);
			AAA_ERR	file_maa_count_one( FILE *file,			INT32* node_nb,	INT32* start,	INT32* stop	);
			AAA_ERR	file_maa_count(		C_PCHAR_C filename,	INT32* nb_pt,	INT32* nb_frame	);

	//read
			AAA_ERR	file_maa_read_one(	C_PCHAR_C filename,	REAL* data,	INT32 frame_start,	INT32 frame_stop	);
			AAA_ERR	file_maa_read_one(	FILE* file,			REAL* data,	INT32 frame_start,	INT32 frame_stop	);
			AAA_ERR	file_maa_read(		C_PCHAR_C filename,	REAL* data	);

			void	set_seq(			INT32 index );

			void	store_tra(			INT32 node_id, REAL CONST * tra );
			void	lock();
			void	unlock();

	FINLINE	void	set_kinect1()	{	_segments.init_kinect1();	}
	FINLINE	void	set_kinect2()	{	_segments.init_kinect2();	}

	//todo should be completed
	virtual	INT32	get_point_nb() final override;
	virtual	bool	get_point(			REAL* dst, INT32 CONST index ) final override;

	virtual	INT32	get_segment_nb();
	virtual	void	get_segment(		REAL* a, REAL* b, INT32 seg_id );
};

FINLINE	void	c_bdd_mocap::get_tra_cur( REAL* CONST dst, INT32 CONST node_id )
{
	if( _b_valid_data && is_valid_node( node_id ) )
		cpy_v3( dst, get_data_cur( node_id ) + _offset_tra );
	else
		clear_v3( dst );
}

#if	!AAA_REAL_IS_DOUBLE()
FINLINE	void	c_bdd_mocap::get_tra_cur( DOUBLE* CONST dst, INT32 CONST node_id )
{
	if( _b_valid_data && is_valid_node( node_id ) )
		cpy_v3( dst, get_data_cur( node_id ) + _offset_tra );
	else
		clear_v3( dst );
}
#endif

//hack check scale present
//hack do rot
FINLINE	void	c_bdd_mocap::get_sca_cur( DOUBLE* CONST dst, INT32 CONST node_id )
{
	if( _b_valid_data && is_valid_node( node_id ) )
		cpy_v3( dst, get_data_cur( node_id ) + _offset_sca );
	else
		clear_v3( dst );
}

FINLINE	void	c_bdd_mocap::get_axe_cur( DOUBLE* CONST dst, INT32 CONST node_id, INT32 CONST axe )
{	//todo do it always
	if( _b_valid_data && _offset_mat >= 0 && is_valid_node( node_id ) )
		cpy_v3( dst, get_data_cur( node_id ) + _offset_mat + axe*3 );
	else
		clear_v3( dst );
}

FINLINE	void	c_bdd_mocap::do_translate( REAL CONST * CONST tra )
{
	GOL::matrix::translate3v( tra );
}

FINLINE	void	c_bdd_mocap::do_rotate( REAL CONST * rot )
{
	switch( _s_origin )
	{
	case FROM_MAX:			GOL::matrix::rotatev( rot, aaa::matrix::ORDER_XZY | aaa::matrix::ORDER_X_NEG | aaa::matrix::ORDER_Z_NEG );	break;
	case FROM_SOFTIMAGE:	GOL::matrix::rotatev( rot, aaa::matrix::ORDER_ZYX );														break;
	}
}

FINLINE	void	c_bdd_mocap::do_scale( REAL CONST * CONST sca )
{
	GOL::matrix::scale3v( sca );
}


FINLINE	void	c_bdd_mocap::do_matrix_from_mat3( REAL CONST * CONST src )
{
	glm::mat4 m4;
	glm::mat3 CONST & m3 = *(glm::mat3 CONST *)src;
	m4[0] = glm::vec4( m3[0], 0 );
	m4[1] = glm::vec4( m3[1], 0 );
	m4[2] = glm::vec4( m3[2], 0 );
	m4[3] = glm::vec4( 0, 0, 0, 1);

//	glm::mat4 m( (glm::mat3 CONST *)mat3 );
	GOL::matrix::mul_matrix( &m4 );
}
