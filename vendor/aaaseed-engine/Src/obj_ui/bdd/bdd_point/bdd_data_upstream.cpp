#include "obj_ui/bdd/bdd_point/bdd_data_upstream.h"
#include "obj_ui/bdd/bdd_edit/keys.h"
#include "draw/model.h"
#include "draw/color.h"
#include "math/rand.h"
#include "file/aaa_file.h"
#include "asc_line.h"
#include "infrastructure/data/datacube.h"
#include "file/fname.h"
#include "io.h"
#include "obj_ui/bdd/bdd_point/bdd_point.h"
#include "obj_ui/com/net.h"
#include "infrastructure/seedfile.h"
#include "obj_ui/bdd/bdd_edit/bdd_curve_edit.h"
#include "infrastructure/layer/layer.h"
#include "infrastructure/layer/layers.h"


st_door*	c_door_stream::push( REAL time_in )
{
	struct	st_door	data_new;

	data_new.time_ = time_in;
	if( time_begin_ > time_in)
		time_begin_ = time_in;
	if( time_end_ < time_in)
		time_end_ = time_in;

	data_.push_back( data_new );

	return &data_.back();
}

INT32	c_door_stream::get_nb()		{	return data_.size();	}

st_door*	c_door_stream::get_door( INT32 index )
{
	if( index < get_nb() )
		return &data_[index];
	else
		return NULL;
}

void	c_door_stream::init()
{
	time_begin_ = REAL_BIG_VALUE;
	time_end_ = -REAL_BIG_VALUE;
	data_.clear();
}

c_door_stream::c_door_stream()
{
	init();
}

void	c_door_stream::deinit()
{
	data_.clear();
}

c_door_stream::~c_door_stream()
{
	deinit();
}

c_door_state::c_door_state()
{
	init();
}

void	c_door_state::init()
{
	b_active_ = TRUE;
	presence_ = 1.;
	owner_id_ = -1;
	new_owner_id_ = -1;
}


void	c_seq_upstream::clear()
{
	map<INT32,c_door_stream*>::iterator		it;
	c_door_stream*							p_stream;

	for( it = door_streams_.begin(); it != door_streams_.end(); ++it )
	{
		p_stream = it->second;
		p_stream->deinit();
		door_stream_free_.push_back( p_stream );
	}
	door_streams_.clear();
	name_.erase();
	b_format_old_ = TRUE;
}

void	c_seq_upstream::dealloc()
{
	vector<c_door_stream*>::iterator		it;
	c_door_stream*							p_dot;

	for( it = door_stream_free_.begin(); it != door_stream_free_.end(); ++it )
	{
		p_dot = *it;
		delete p_dot;
	}
	door_stream_free_.clear();
}

c_door_stream*	c_seq_upstream::get_door_stream_new( INT32 obj_id )
{
	c_door_stream*	p_door_stream;

	if( door_stream_free_.empty() )
		p_door_stream = new c_door_stream;	//todo check result
	else
		{
		p_door_stream = door_stream_free_.back();
		p_door_stream->init();
		door_stream_free_.pop_back();
		}
	door_streams_[obj_id] = p_door_stream;
	return p_door_stream;
}

c_door_stream*	c_seq_upstream::get_door_stream_always( INT32 obj_id )
{
	map< INT32, c_door_stream*>::iterator	it;

	it = door_streams_.find( obj_id );
	if ( it == door_streams_.end() )
		{
		get_door_stream_new( obj_id );
		it = door_streams_.find( obj_id );
		if ( it == door_streams_.end() )
			{
			ERR_PRINT_STRING( "c_bdd_data_upstream::get_door_stream() can't find stream" );
			return NULL;
			}
		}
	return it->second;
}

c_door_state*	c_seq_upstream::get_door_state( INT32 obj_id )
{
	map< INT32, c_door_stream*>::iterator	it;

	it = door_streams_.find( obj_id );
	if ( it == door_streams_.end() )
	{
		debug_break();
		return NULL;
	}
	return &it->second->state_;	
}

void	c_seq_upstream::reset()
{
map<INT32,c_door_stream*>::iterator	it;
	for( it = door_streams_.begin(); it != door_streams_.end(); ++it )
	{
		c_door_stream*	p_door;
		c_door_state*	p_state;
		p_door = it->second;
		p_state = &p_door->state_;
		p_state->init();
		erased_nb_ = 0;
	}
}

//fill a structure no door_stream
void	c_seq_upstream::create( INT32 nb )
{
	for( INT32	i=0; i<nb; ++i )
	{
		get_door_stream_always( i );
	}
	b_format_old_ = FALSE;
}

FACTORY_CREATE_PROP_V1( c_bdd_data_upstream, bdd_data_upstream, Data Upstream, bdd_data_upstream, sub_menu="Special"; );

c_bdd_data_upstream*	bdd_data_upstream_cur = NULL;


namespace	n_bdd_data_upstream
{
	static	CONST	INT32	BASE_PARAM_NB		= 22 + c_bdd::GEO_PARAM_NB;
	static	CONST	INT32	CONTACT_PARAM_NB	= 3;
	static	CONST	INT32	DRAW_PARAM_NB		= 8;
	static	CONST	INT32	SRQ_PARAM_NB		= 4;
	static	CONST	INT32	SRQ_NB				= SEQ_UPSTREAM_MAX_NB;
	static	CONST	INT32	GROUP_PARAM_NB		= 2;
	static	CONST	INT32	PARAM_NB_MAX = BASE_PARAM_NB
											+ CONTACT_PARAM_NB
											+ DRAW_PARAM_NB
											+ (SRQ_PARAM_NB+1)*SRQ_NB
											+ GROUP_PARAM_NB;


#define PARAM_SEQ_OLD( nb )\
		ST_PARAM_GROUP_CLOSED( Sequence_##nb, SRQ_PARAM_NB )	\
			ST_PARAM_REAL_ZERO( _time_offset##nb )	\
			{	NULL,	PARAM_REAL,	"vanish_tine_"###nb,	2, 1,		0.01, PARAM_INFINI,				NULL, NULL },\
			{	NULL,	PARAM_REAL,	"appear_tine_"###nb,	2, 1,		0.01, PARAM_INFINI,				NULL, NULL },
#define PARAM_SEQ( nb )\
		ST_PARAM_GROUP_CLOSED( Sequence_##nb, SRQ_PARAM_NB )	\
			ST_PARAM_BOOL_ON( used )	\
			ST_PARAM_REAL_ZERO( _time_offset##nb )	\
			{	NULL,	PARAM_REAL,	"vanish_time_"###nb,	2, 1,		0.01, PARAM_INFINI,				NULL, NULL },\
			{	NULL,	PARAM_REAL,	"appear_time_"###nb,	2, 1,		0.01, PARAM_INFINI,				NULL, NULL },

	static	ST_PARAM	param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
		ST_PARAM_POINT_XYZ( origin )
		{	NULL,	PARAM_INT32,	"datagrid_dst",			1, 0,		0, DATAGRID_NB - 1,				NULL, NULL },
		{	NULL,	PARAM_INT32,	"sequence_index",		1, 0,		0, SEQ_UPSTREAM_MAX_NB - 1,		NULL, NULL },
		ST_PARAM_FILENAME( tracking_file,	TYPE_IO_BDD_DATA_UPSTREAM, 0.	)
		ST_PARAM_REAL_POS_ZERO( duration )
		ST_PARAM_BOOL_ON( play )
		ST_PARAM_BOOL_ON( loop )
		ST_PARAM_BOOLC_OFF( restart_trig )

		ST_PARAM_REAL_POS_ONE( time_factor )
		{	NULL,	PARAM_REAL|PARAM_SAVE_NOT,	"time",		1., 0.,		0., PARAM_INFINI,				NULL, NULL },

		ST_PARAM_BOOL_OFF( do_door )
		{	NULL,	PARAM_REAL,		"fps",				1, 12,		0, PARAM_INFINI,					NULL, NULL },
		{	NULL,	PARAM_REAL,		"upstream_width",	1, 100,		0, PARAM_INFINI,					NULL, NULL },
		{	NULL,	PARAM_REAL,		"upstream_height",	1, 200,		0, PARAM_INFINI,					NULL, NULL },
		{	NULL,	PARAM_INT32,	"x_max",			1, 720,		0, PARAM_INFINI,					NULL, NULL },
		{	NULL,	PARAM_INT32,	"y_max",			1, 544,		0, PARAM_INFINI,					NULL, NULL },

		ST_PARAM_GROUP_CLOSED( Contact, CONTACT_PARAM_NB )
			ST_PARAM_REAL_POS_ONE( contact_dist )
			ST_PARAM_REAL_POS_ZERO( contact_only_first )
			ST_PARAM_REAL_POS_ZERO( contact_reset_every_frame )
		ST_PARAM_GROUP_CLOSED( Draw, DRAW_PARAM_NB )
			ST_PARAM_REAL_POS_ONE( time_interval )
			ST_PARAM_REAL_POS_ONE( max_time_interval )
			{	NULL,	PARAM_INT32,	"max_sample",				1, 1024,	0, PARAM_INFINI,				NULL, NULL },
			ST_PARAM_BOOL_ON( draw_trajectory_line )
			ST_PARAM_BOOL_OFF( draw_trajectory_point )
			ST_PARAM_BOOL_OFF( draw_box )
			ST_PARAM_BOOL_OFF( draw_door )
			ST_PARAM_BOOL_OFF( draw_back_to_front )

		ST_PARAM_BOOL_ON( reset_on_restart )
		ST_PARAM_INT32_POS_ZERO( erased_nb )

		ST_PARAM_REAL_POS_ONE( vanish_time_factor )
		ST_PARAM_REAL_POS_ONE( appear_time_factor )
		
		PARAM_SEQ(00)
		PARAM_SEQ(01)
		PARAM_SEQ(02)
		PARAM_SEQ(03)
		PARAM_SEQ(04)
		PARAM_SEQ(05)
		PARAM_SEQ(06)
		PARAM_SEQ(07)
		PARAM_SEQ(08)
		PARAM_SEQ(09)
		PARAM_SEQ(10)
		PARAM_SEQ(11)
		PARAM_SEQ(12)
		PARAM_SEQ(13)
		PARAM_SEQ(14)
		PARAM_SEQ(15)
		PARAM_SEQ(16)
		PARAM_SEQ(17)
		PARAM_SEQ(18)
		PARAM_SEQ(19)
		PARAM_SEQ(20)
		PARAM_SEQ(21)
		PARAM_SEQ(22)
		PARAM_SEQ(23)
		PARAM_SEQ(24)
		PARAM_SEQ(25)
		PARAM_SEQ(26)
		PARAM_SEQ(27)
		PARAM_SEQ(28)
		PARAM_SEQ(29)
		PARAM_SEQ(30)
		PARAM_SEQ(31)
	};
}

void	c_bdd_data_upstream::param_init_pt()
{
INT32	h = param_init_pt_geo();

	param_set_pt_v3( h, origin_ );

	param_set_pt( h, upstream_datagrid_id_ );
	param_set_pt( h, seq_ui_index_ );
	param_set_pt( h, tracking_fname_ );
	param_set_pt( h, seq_cur_->duration_ );
	param_set_pt( h, b_play_ );
	param_set_pt( h, b_play_loop_ );
	param_set_pt( h, _b_restart_trig_ui );
	param_set_pt( h, time_factor_ );
	param_set_pt( h, time_ui_ );

	param_set_pt( h, b_door_do_ );
	
	param_set_pt( h, upstream_fps_ );
	param_set_pt( h, upstream_width_ );
	param_set_pt( h, upstream_height_ );
	param_set_pt( h, upstream_x_max_ );
	param_set_pt( h, upstream_y_max_ );

	h++;
	param_set_pt( h, contact_dist_ );
	param_set_pt( h, b_contact_only_first_ );
	param_set_pt( h, b_contact_reset_every_frame_ );
	
	h++;
	param_set_pt( h, draw_interval_ );
	param_set_pt( h, max_interval_ );
	param_set_pt( h, max_sample_ );
	param_set_pt( h, b_draw_traj_line_ );
	param_set_pt( h, b_draw_traj_point_ );
	param_set_pt( h, b_draw_box_ );
	param_set_pt( h, b_draw_door_ );
	param_set_pt( h, b_draw_back_to_front_ );


	param_set_pt( h, b_seq_reset_on_restart_ );
	param_set_pt( h, erased_nb_ );

	param_set_pt( h,vanish_time_factor_ );
	param_set_pt( h,appear_time_factor_ );
	for( INT32	i = 0; i < SEQ_UPSTREAM_MAX_NB; ++i )
	{
		h++;
		param_set_pt( h, &_seqs[i].b_used_ );
		param_set_pt( h, &_seqs[i].video_offset_ );
		param_set_pt( h, &_seqs[i].vanish_time_ );	
		param_set_pt( h, &_seqs[i].appear_time_ );
	}
	err_param_init_pt( h );
}

void c_bdd_data_upstream::init()
{
	time_stamp_ = 0.0f;
	time_stopped_ = 0.0f;
	b_play_started_ = FALSE;
	b_play_stopped_ = FALSE;
	b_play_end_ = FALSE;
	time_ui_ = 0.0f;
	frame_index_ = 0;
}

CONSTRUCTOR_CREATE( c_bdd_data_upstream )
{
	seqs_clear();
	seq_set( 0 );
	param_init_with( n_bdd_data_upstream::param, n_bdd_data_upstream::PARAM_NB_MAX );
	init();
	alloc();
}

void c_bdd_data_upstream::alloc()
{
	for( INT32 i = 0; i < SEQ_UPSTREAM_MAX_NB; ++i )
	{
		_seqs[i].duration_ = 0.0;
		//	_seqs[i].vanish_time_ = 1.;
		//	_seqs[i].appear_time_ = 1.;
		_seqs[i].video_offset_ = 0.0;
		_seqs[i].erased_nb_ = 0.0;
	}
}

void c_bdd_data_upstream::dealloc()
{
	for( INT32 i = 0; i < SEQ_UPSTREAM_MAX_NB; ++i )
		_seqs[i].dealloc();
}

c_bdd_data_upstream::~c_bdd_data_upstream()
{
	seqs_clear();
	dealloc();
}

void	c_bdd_data_upstream::seqs_clear()
{
	for( INT32 i = 0; i < SEQ_UPSTREAM_MAX_NB; ++i )
		_seqs[i].clear();
	seq_cur_ = NULL;
	seq_cur_index_ = -1;
	seq_nb_ = 0;
}

void	c_bdd_data_upstream::seq_set( INT32 index_in )
{
	// update filename
	if( seq_cur_index_ != index_in )
		{
		seq_cur_index_ = index_in;
		seq_cur_ = &_seqs[seq_cur_index_];
		tracking_fname_ = seq_cur_->name_.get();
		seq_cur_->reset();
		DBG_PRINT_STRING( "Upstream switching to seq %d : ", seq_cur_index_, tracking_fname_ );
		}
}

extern BOOL gb_render_first_pass;

void	c_bdd_data_upstream::update()
{
	bdd_data_upstream_cur = this;
	//todo
	if( !draw::is_render_first_pass() )
		return;
	param_init_pt();	//hack architecture bug on updating parameter with changing adresses

	c_model::cur->get_size_v3r( size_ );

	seq_set( seq_ui_index_ );

	if( _b_restart_trig_ui )
	{	//todo
	//	clear();
		time_ui_ = 0.0f;
		b_play_started_ = FALSE;
		b_play_end_ = FALSE;
		_b_restart_trig_ui = false;
	}
	if( b_play_ && !b_play_end_ )
		play();
	else
		if( b_play_end_ )
			if( b_play_loop_ )
				{
				time_ui_ = 0.0f;
				b_play_started_ = FALSE;
				b_play_end_ = FALSE;
				}

	if( delta_t_.update() ) // || _b_restart_trig_ui) )
		{
		if( b_seq_reset_on_restart_ )
			seq_cur_->reset();
		}
	dt_ = delta_t_.get_dt();
	time_cur_ = time_factor_ * ( time_ui_ + seq_cur_->video_offset_ );

	if( seq_cur_->duration_ != 0. )	
		time_cur_ = CLAMP( time_cur_, REAL(0), seq_cur_->duration_ - REAL( 1. / 12. ) );
	if( time_cur_ < time_last_ )	//todonow attention to double screen
		{
		if( b_seq_reset_on_restart_ )
			seq_cur_->reset();
		}
	time_last_ = time_cur_;

	build_door_cur( time_cur_ );	//this where we build the data
	if( b_door_do_ )
		{
		contact_check();
		}
	points_nb_ = seq_cur_->door_streams_.size();
//	dot_check_time();
	erased_nb_ = seq_cur_->erased_nb_;

//deform to be done
/*
c_boids::boid_bulk::iterator	it;
REAL*	p;

	//	get all position and number
	p = pos;
	for( it = boids.living.begin(); it != boids.living.end(); it++, p+=3 )
		cpy_v3r( p, (*it)->pos);
	nb_to_draw = (p-pos)/3;	// avoid this : nb_to_draw = boids.living.size();

	//	deform if needed
	if( c_def_node::cur->is_deforming() )
		c_def_node::cur->apply( pos, nb_to_draw );

	//	feed mocap if needed
	{
	c_bdd_mocap*	mocap;
	mocap = c_bdd_mocap::get_from_channel( mocap_feed_channel);
	if( mocap )
		{
		p = pos;
		for( it = boids.living.begin(); it != boids.living.end(); it++, p+=3 )
			mocap->store_tra( (*it)->index, p);
		}
	}
*/
}

/*
void	c_bdd_data_upstream::analyse()
{
map<INT32,c_door_stream*>::iterator		it = door_streams.begin();
INT32							i;
INT32							nb;
REAL							norm;

	nb = door_streams.size();
	i = nb;
	speed_sigma = 0;
	speed_min = PARAM_INFINI;
	speed_max = 0;

	for( ; i>0; i--, ++it )	//, p+=3 )
		{
		c_door_stream*		dot;
		st_door*	pd;

		dot = it->second;
		pd = dot->door(0);
		norm = pd->speed_norm;
		speed_sigma += norm;
		speed_min = MIN( speed_min, norm);
		speed_max = MAX( speed_max, norm);
//		dot->update();	// NO DONE IN PUSH
		}
//todonow
	if( b_net_send_analyse )	//done only when analyse
		{	
		frame_index++;
		if( (net_send_analyse_skip_nb == 0) || IMOD( frame_index, net_send_analyse_skip_nb+1) == 0 )
			{
			CHAR	buf[256];
			sprintf( buf, "DOTS_INFO %d %d %d %.2f %.2f %.2f",
				frame_index, points_nb, points_nb_moving, speed_sigma, speed_sigma_moving, speed_max);
			net->send_osc_str( 2, buf);
			net->osc_flush(2);
			}
		}

}
*/
void	c_bdd_data_upstream::draw_single()
{
	if( b_draw_door_ )
		draw_door();
	if( b_draw_box_ )
		draw_box();
	draw_traj();
}

void	c_bdd_data_upstream::draw_traj()
{
map<INT32,c_door_stream*>::iterator		it;
INT32							i;

	it = seq_cur_->door_streams_.begin();
	nb_to_draw_ = seq_cur_->door_streams_.size();
//	c_multiple::cur->set_nb( nb_to_draw);
	i = nb_to_draw_;

	for( ; i > 0; i--, ++it )
		{
		c_door_stream*		dot;
		REAL		p[3];
		INT32		nb;
		st_door*	pd;

//		c_multiple::cur->index_set( it->first );
		dot = it->second;
		nb = MIN( dot->get_nb(), max_sample_ );

		if( b_draw_traj_line_ )
			{
//			GOL::color4( 0, 0, 1, .5);
			GOL::begin( GL_LINE_STRIP );
			for( INT32 j = 0; j < nb; ++j )
				{
				pd = dot->get_door( j );
//todonow	this is the old way
//					dt = n_aaatime::sec_get_interval( &dot->timestamp, &pd->time );
//					if( dt > draw_interval )
//						break;
				mul_add_v3r( p, pd->pos_, size_, origin_ );
				GOL::vertex3v( p );
				}
			GOL::end();
			}
		if( b_draw_traj_point_ )
			{
//			GOL::color4( 0, 0, 1, .5);
//			GOL::point_size( 1.);
			GOL::begin( GL_POINTS );
			for( INT32 j = 0; j < nb; ++j )
				{
				pd = dot->get_door( j );
//todonow	this is the old way
//					dt = n_aaatime::sec_get_interval( &dot->timestamp, &pd->time );
//					if( dt > draw_interval )
//						break;
				mul_add_v3r( p, pd->pos_, size_, origin_ );
				GOL::vertex3v( p );
				}
			GOL::end();
			}
		}
}


void	c_bdd_data_upstream::draw_door()
{
REAL					height_skew;
REAL					door_half_width;
REAL					door_half_height;
st_door*				p_door;
map<REAL,st_door>::iterator	it;	
c_door_state*				p_state;

	for( it = doors_cur_.begin(); it != doors_cur_.end(); ++it )
		{
		REAL		pos[3];
		REAL		skew_1;
		REAL		skew_2;

		p_door = &it->second;
		p_state = seq_cur_->get_door_state( p_door->id_ );
		if( p_state->b_active_ )
			if( p_state->b_touched_ )
				GOL::color3( 0, 1, 0 );
			else
				GOL::color3( 1, 1, 1 );
		else
			GOL::color3( 1, 0, 0 );

		pos[2] = p_door->pos_[2];

		GOL::begin( GL_LINE_STRIP );

		height_skew = 2. * p_door->size_[1] * upstream_width_ * SIN_DEG( p_door->rot_ ) / upstream_y_max_ ;
//		REAL factor = door_vector_cur[i].size[1] / ( upstream_height / upstream_y_max + height_skew );

		if( height_skew < 0)
			{
			skew_1 = 0;
			skew_2 = height_skew;
			}
		else
			{
			skew_1 = height_skew;
			skew_2 = 0;
			}
		door_half_width = p_door->size_[0] * 0.5f;
		door_half_height = p_door->size_[1] * 0.5f;

		if( p_door->rot_ > 90 )
			door_half_width = -door_half_width;

		// x0, y0
		pos[0] = p_door->pos_[0] - door_half_width;
		pos[1] = p_door->pos_[1] - door_half_height + skew_1;
		GOL::vertex3v(pos);

		// x1, y1
		pos[0] = p_door->pos_[0] - door_half_width;
		pos[1] = p_door->pos_[1] + door_half_height + skew_2;;
		GOL::vertex3v(pos);

		// x2, y2
		pos[0] = p_door->pos_[0] + door_half_width;
		pos[1] = p_door->pos_[1] + door_half_height - skew_1;
		GOL::vertex3v(pos);

		// x3, y3
		pos[0] = p_door->pos_[0] + door_half_width;
		pos[1] = p_door->pos_[1] - door_half_height - skew_2;
		GOL::vertex3v(pos);

		GOL::end();			
		}

}

void	c_bdd_data_upstream::draw_box()
{
REAL					door_half_width;
REAL					door_half_height;
st_door*				p_door;
map<REAL,st_door>::iterator	it;	
c_door_state*				p_state;

	for( it = doors_cur_.begin(); it != doors_cur_.end(); ++it )
		{
		REAL		pos[3];

		p_door = &it->second;
		p_state = seq_cur_->get_door_state( p_door->id_ );
		if( p_state->b_active_ )
			if( p_state->b_touched_ )
				GOL::color3( 0, 1, 0 );
			else
				GOL::color3( 1, 1, 1 );
		else
			GOL::color3( 1, 0, 0 );

		pos[2] = p_door->pos_[2];

		GOL::begin( GL_LINE_LOOP );

		door_half_width = p_door->size_[0] * 0.5f;
		door_half_height = p_door->size_[1] * 0.5f;

		// x0, y0
		pos[0] = p_door->pos_[0] - door_half_width;
		pos[1] = p_door->pos_[1] - door_half_height;
		GOL::vertex3v( pos );

		// x1, y1
		pos[0] = p_door->pos_[0] - door_half_width;
		pos[1] = p_door->pos_[1] + door_half_height;;
		GOL::vertex3v( pos );

		// x2, y2
		pos[0] = p_door->pos_[0] + door_half_width;
		pos[1] = p_door->pos_[1] + door_half_height;
		GOL::vertex3v( pos );

		// x3, y3
		pos[0] = p_door->pos_[0] + door_half_width;
		pos[1] = p_door->pos_[1] - door_half_height;
		GOL::vertex3v( pos );

		GOL::end();			
		}

}

void	c_bdd_data_upstream::draw_multiple()
{
INT32						row;

	nb_to_draw_ = doors_cur_.size();
	c_multiple::cur->set_nb( nb_to_draw_ );
	row = 1;
		{
		map<REAL, st_door>::iterator	it;
		for( it = doors_cur_.begin(); it != doors_cur_.end(); ++it )
			{
			st_door*			p_door;
			c_door_state*		p_state;
			REAL				size[3];
			REAL				skew;
			INT32				the_id;

			p_door = &it->second;
			p_state = seq_cur_->get_door_state( p_door->id_ );

			skew =  2. * upstream_width_ * SIN_DEG( p_door->rot_ ) / ( upstream_y_max_ );

			c_multiple::cur->set_index( row - 1 );
			if( p_state->b_touched_ )
				the_id = p_state->owner_id_ ;
			else
				the_id = p_door->id_ ;

			datacube_def.set_real( upstream_datagrid_id_, 1, 1, the_id );
			c_multiple::cur->set_parameter( 1, the_id );

			datacube_def.set_real( upstream_datagrid_id_, 1, 2, p_state->b_active_ ? 1 : 0 );
			datacube_def.set_real( upstream_datagrid_id_, 1, 3, p_state->b_touched_ ? 1 : 0 );
			datacube_def.set_real( upstream_datagrid_id_, 1, 4, p_state->presence_ );
			c_multiple::cur->set_parameter( 2, p_state->b_active_ ? 1 : 0 );
			c_multiple::cur->set_parameter( 3, p_state->b_touched_ ? 1 : 0 );
			c_multiple::cur->set_parameter( 4, p_state->presence_ );

			datacube_def.set_real( upstream_datagrid_id_, 1, 5, skew  );
			c_multiple::cur->set_parameter( 5, skew  );

			c_multiple::cur->set_parameter( 8, p_door->left_ );
			c_multiple::cur->set_parameter( 9, p_door->right_ );
			c_multiple::cur->set_parameter( 10, p_door->y_[0] );
			c_multiple::cur->set_parameter( 11, p_door->y_[1] );
			c_multiple::cur->set_parameter( 12, p_door->y_[2] );
			c_multiple::cur->set_parameter( 13, p_door->y_[3] );
			row++;

//			cpy_v3r( pos, p_door->pos);
//			pos[1] += p_door->size[1]*skew*.5;
			cpy_v3r( size, p_door->size_ );
			if( p_door->rot_ > 90 )
				size[0] = -size[0];

			if( seq_cur_->b_format_old_ )
				c_multiple::cur->align_then_draw( p_door->pos_, size );
			else
				c_multiple::cur->align_then_draw( zero_v3r);
			}
		}
}

INT32	c_bdd_data_upstream::get_point_nb()
{
	return nb_to_draw_;
}

void	c_bdd_data_upstream::get_point( REAL* dst, INT32 index )
{
//	cpy_v3r( dst, &pos[index*3] );
}

static	c_rand_lin	rnd;
void	c_bdd_data_upstream::get_point_and_speed_rnd_time_rel( REAL* pos, REAL* speed, REAL t_in )
{
	map<INT32,c_door_stream*>::iterator		it;
	INT32	nb;
	INT32	i;
	c_door_stream*	dot;

	clear_v3r( pos );
	clear_v3r( speed );

	nb = seq_cur_->door_streams_.size();
	dot = NULL;
	
	if( nb >= 1)
	{
/*		if (nb == 1 )
			{
			i = 0;
			}
		else
*/		{
			INT32	nb_try;
			nb_try = 0;
			do
			{
				nb_try++;
				i = rnd.get_uint32() >> 12;
				if( i < 0)
					i = -i;		
				i = IMOD( i, nb );
				it = seq_cur_->door_streams_.begin();
				for( ; i > 0; --i )
					++it;				
				dot = it->second;
				break;	//todo this had sense when the moving condition existed (aichi/bdd_point)
			}
			while( nb_try < 100 );
	//		printf( "i %d nb %d\n", i, nb);
		}

	}
	if( dot)
	{
		struct	st_door*	pd;
		pd = dot->get_door( 0 );
		if( pd )
		{
			mul_add_v3r( pos, pd->pos_, size_, origin_ );
//			mul_v3r( speed_pt, pd->speed, size);	//todonow better that this next clear_v3r
			clear_v3r( speed );
		}
	}
	else
	{
	}
}

c_obj_ui*	ldoor = NULL;
p_param		p_doora = NULL;
p_param		p_doorb = NULL;
p_param		p_doorc = NULL;

void	c_bdd_data_upstream::build_door_cur( REAL time )
{
map< INT32, c_door_stream*>::iterator	it;
INT32	i;
	if( !p_doora)
		{
		ldoor = (c_layer*) c_obj_ui::find_from_top_by_search_name( "Mod/Upstream/Door/fx.layers_param" );
		if( !ldoor )	//	avoid crash in other projects
			return;
		p_doora = ldoor->find_param_by_name( "Layer_A" );
		p_doorb = ldoor->find_param_by_name( "Layer_B" );
		p_doorc = ldoor->find_param_by_name( "Layer_C" );
		}
	// clear current door vector
	doors_cur_.clear();
	it = seq_cur_->door_streams_.begin();

	if( seq_cur_->b_format_old_ )
		{	//	format upstream Paris (import from flash)
		p_doora->set_value_from_real_type_number( 1 );
		p_doorb->set_value_from_real_type_number( 1 );
		p_doorc->set_value_from_real_type_number( 0 );
//		n_aaatime::store( &time_current );
		i = seq_cur_->door_streams_.size();
		for( ; i>0; i--, ++it )	// iterate on door_streams
			{
			c_door_stream*	door;
			door = it->second;
			if( door )
				{
				INT32		door_nb;
				st_door*	door_data;
				st_door*	door_data_prev;

				door_data = NULL;
				door_data_prev = NULL;
				door_nb = door->get_nb();
				if( door_nb > 0 )
					{
					// iterate on key to find prev and next
					for( INT32 j = 0; j < door_nb; ++j )	
						{
						door_data = door->get_door( j );
						if( door_data )
							{
							if( door_data->time_ > time_cur_ )
								{
								st_door		door_new;
								// time is lower than current time, we have found the next door position
								if( door_data_prev )
									{
									REAL	vec[3];
									REAL	dif;
									REAL	t = ( time_cur_ - door_data_prev->time_ ) / ( door_data->time_ - door_data_prev->time_ );
									interpolate_v3r( vec, door_data_prev->pos_, door_data->pos_, t );
									mul_add_v3r( door_new.pos_, vec, size_, origin_ );

									interpolate_v3r( vec, door_data_prev->size_, door_data->size_, t );
									mul_v3r(door_new.size_, vec, size_ );
									dif = door_data_prev->rot_ - door_data->rot_;
									if( dif > 180. )
										door_new.rot_ = interpolate( door_data_prev->rot_, door_data->rot_ + 360., t );
									else if( dif < -180 )
										door_new.rot_ = interpolate( door_data_prev->rot_, door_data->rot_ - 360., t );
									else
										door_new.rot_ = interpolate( door_data_prev->rot_, door_data->rot_, t );
									door_new.depth_ = interpolate( door_data_prev->depth_, door_data->depth_, t );
									door_new.id_ = it->first;

									if( b_contact_reset_every_frame_ )
										it->second->state_.b_active_ = TRUE;
									it->second->state_.b_touched_ = FALSE;
									doors_cur_[ b_draw_back_to_front_?-door_new.depth_:door_new.depth_ ] = door_new;
									}
								else
									{
									// don't have the door on the prev frame, so we need to fade in the new door
									// todonow
									}
								break;
								}
							else
								door_data_prev = door_data;

							}
						else
							{
							if( door_data_prev )
								{
								// we don't the door on the next frame so, we have to fade out the current door
								// todo now

								}
							}
						}
					}
				}
			}
		}
	else
		{	// format Bucarest (AAASeed curve editor)
		c_bdd_curve_edit*	pd;
		c_control_key*		p_control;
		INT32				ic;

		p_doora->set_value_from_real_type_number( 0 );
		p_doorb->set_value_from_real_type_number( 0 );
		p_doorc->set_value_from_real_type_number( 1 );
		
		pd = c_bdd_curve_edit::get_from_channel( seq_cur_index_ );
		if( pd )			
			{
			ic = 1;			
	//		n_aaatime::store( &time_current );
			while( p_control = pd->control_get_if_exist( ic ) )
				{
				if(  p_control->is_value( time_cur_ ) )
					{
					st_door	door_new;
					REAL	rdx;
					REAL	rdy;
					REAL	x;
					REAL	y;
					REAL	ldx;
					REAL	ldy;
					REAL	left;
					REAL	right;
					REAL	top;
					REAL	bottom;
					REAL	delta;

					x = p_control->get_value( time_cur_, 0 );
					y = p_control->get_value( time_cur_, 1 );

					p_control = pd->control_get_if_exist( ic + 1 );
					rdx = p_control->get_value( time_cur_, 0 );
					rdy = p_control->get_value( time_cur_, 1 );

					p_control = pd->control_get_if_exist( ic + 2 );
					ldx = p_control->get_value( time_cur_, 0 );
					ldy = p_control->get_value( time_cur_, 1 );

					right = x + rdx;
					left = x + ldx;

					if( rdx != 0 )
						{
						delta = rdy / rdx;
						if ( delta > 3 )
							delta = 3;
						else if( delta < -3 )
							delta = -3;
						}
					else
						delta = 0;
					
					door_new.y_[0] = y + ldy + ldx * delta;					
					door_new.y_[1] = y + ldx * delta;					
					door_new.y_[2] = y + rdy;					
					door_new.y_[3] = y + rdy + ldy;					
					if( ldx * delta > 0 )
						{
						top = y + ldx * delta;
						bottom = y + rdy + ldy;
						}
					else
						{
						top = y+ rdy;
						bottom = y + ldy + ldx * delta;
						}

					//still used for collision;

					door_new.pos_[0] = (right + left) * .5;
					door_new.pos_[1] = (top + bottom) * .5;
					door_new.pos_[2] = 0;

					door_new.size_[0] = (right - left);
					door_new.size_[1] = (top - bottom);
					door_new.size_[2] = 1.;

					door_new.left_ = left;
					door_new.right_ = right;

					door_new.rot_ = 0.;
					door_new.depth_ = +door_new.pos_[1] - door_new.size_[1]*.5;
					door_new.id_ = it->first;

					if( b_contact_reset_every_frame_ )
						it->second->state_.b_active_ = TRUE;
					it->second->state_.b_touched_ = FALSE;
					doors_cur_[ b_draw_back_to_front_?-door_new.depth_:door_new.depth_ ] = door_new;
					}
				it++;
				ic += 4;
				}	//while
			}
		else
			ERR_PRINT_STRING( "no curve data");
		}
}

void	c_bdd_data_upstream::contact_check()
{
st_door*						p_door;
map<REAL,st_door>::iterator		it_door;
map<INT32,c_dot*>*				p_dots;
map<INT32,c_dot*>::iterator		it_dot;
c_door_state*					p_state;
INT32							id_door;

REAL			contact_on_dist = contact_dist_;
//REAL			contact_off_dist = contact_dist;
REAL			contact_on_dist_squared;
//REAL			contact_off_dist_squared;
REAL*			door_pos;
REAL*			dot_pos;
INT32			door_nb;
	if( !bdd_point_cur)
		return;

	contact_on_dist_squared = contact_on_dist*contact_on_dist;
//	contact_off_dist_squared = contact_off_dist*contact_off_dist;

	p_dots = &bdd_point_cur->dots;
//	dot_nb = bdd_point_cur->dots.size(); 
//	boid_nb = bdd_boid->get_point_nb();

	for( it_dot = p_dots->begin(); it_dot != p_dots->end(); ++it_dot )
		{
		dot_pos = it_dot->second->get_pdot(0)->pos_filtered;
		door_nb = doors_cur_.size();
		it_door = doors_cur_.end();
		for( ; door_nb>0; --door_nb )
			{
			--it_door;
			p_door = &it_door->second;
			id_door = p_door->id_;
			p_state = seq_cur_->get_door_state( id_door );

//				if( p_aichi_point->b_touchable )
			if( p_state->b_active_ )
				{
				REAL		dist;
				REAL		tmp;
//					c_contact*	contact;
				door_pos = p_door->pos_;

				dist = door_pos[0] - dot_pos[0];
				dist = ABS( dist );
				dist -= p_door->size_[0] * .5;
				dist = MAX( REAL(0.), dist);
				dist *= dist;

				tmp = door_pos[1] - dot_pos[1];
				tmp = ABS( tmp );
				tmp -= p_door->size_[1] * .5;
				tmp = MAX( REAL(0.), tmp);
				dist += tmp * tmp;

				if( dist <= contact_on_dist_squared )
					{	
//					REAL tmp;
					tmp = p_state->presence_;
					//up 
					if( seq_cur_->vanish_time_ == 0.)
						seq_cur_->vanish_time_ = 1.;
					tmp -= dt_ / (seq_cur_->vanish_time_*vanish_time_factor_ );
					p_state->presence_ = tmp;
					if( p_state->b_touched_ )
						{
						if( it_dot->first == p_state->owner_id_ )
							p_state->new_owner_id_ = it_dot->first;
						}
					else
						{
						p_state->new_owner_id_ = it_dot->first;
						p_state->b_touched_ = TRUE;
						}
					if( tmp <= 0)
						{
						p_state->b_active_ = FALSE;
						seq_cur_->erased_nb_++;
						if( net )
							{
							CHAR	buf[1024];
							sprintf( buf, "CONTACT %d", it_dot->first);
							net->send_osc_str( 0, buf);
							net->osc_flush( 0 );
							}
						}
					if( b_contact_only_first_ )
						break;
					}

/*				contact = p_aichi_point->find_contact( dot_id );
				if( contact)
					{
					if( dist > contact_off_dist_squared )
						p_aichi_point->del_contact( dot_id );
					else
						contact_update( j);
					}
				else
					{
					if( dist <= contact_on_dist_squared )
						{
						p_aichi_point->add_contact( dot_id, contact_time, contact_step );
						contact_do( j, boid_pos[0], boid_pos[1]);
						contact_update( j);
						}
					}
*/				}	// if touchable
			}	//door
		}	//point
	door_nb = doors_cur_.size();
	it_door = doors_cur_.end();
	for( ; door_nb > 0; --door_nb )
		{
		--it_door;
		p_door = &it_door->second;
		id_door = p_door->id_;
		p_state = seq_cur_->get_door_state( id_door );
		if( p_state->b_active_ )
			{
			if( p_state->b_touched_ )
				{			
/*
				if( net && p_state->owner_id != p_state->mew_owner_id )
					{
					CHAR	buf[1024];
					sprintf( buf, "CONTACT %d", p_state->mew_owner_id );
					net->send_osc_str( 0, buf);
					net->osc_flush( 0);
					}
*/
				p_state->owner_id_ = p_state->new_owner_id_;
				}
			else
				{
				REAL tmp;
				p_state->owner_id_ = -1;
				tmp = p_state->presence_;
				//up protect
				if( seq_cur_->appear_time_ == 0. )
					seq_cur_->appear_time_ = 1. ;
				tmp += dt_ / (seq_cur_->appear_time_ * appear_time_factor_ );
				tmp  = MIN( REAL(1.), tmp );
				p_state->presence_ = tmp;
				}
			p_state->new_owner_id_ = -1;
			}

		}
}

/*
void	c_bdd_data_upstream::dot_new( INT32 id, REAL x, REAL y )
{
	dot_pos( id, x, y, 0, 0);
}
*/

/*
void	c_bdd_data_upstream::dot_delete( INT32 id )
{
map< INT32, c_door_stream*>::iterator it;

	// If no match is found for the key, end() is returned
	it = door_streams.find( id );
	if ( it == door_streams.end() )
		ERR_PRINT_STRING( "c_bdd_data_upstream can find dot to delete" );
	else
		{
//		if( aichi)
//			aichi->dot_delete( id );
		c_door_stream*	dot = it->second;
		door_stream_free.push_back( dot);
		dot->deinit();
		door_streams.erase( it);
		}
}
*/

/*
void	c_bdd_data_upstream::dot_pos( INT32 id, REAL* p, REAL r )
{
BOOL	b_dot_on;
map< INT32, c_door_stream*>::iterator it;

	it = door_streams.find( id );
	if ( it == door_streams.end() )
		{
		//generate dot
		b_dot_on = TRUE;
		dot_new( id );
		it = door_streams.find( id );
		if ( it == door_streams.end() )
			{
			ERR_PRINT_STRING( "c_bdd_data_upstream can find dot to set pos" );
			return;
			}
		}
	else
		b_dot_on = FALSE;

	c_door_stream*	dot = it->second;
//	if( aichi && aichi->b_dot_flip_y )
//		p[1] = -p[1];

//	dot->push( p, r);	//todonow we need the time to do it
}
*/

/*
void	c_bdd_data_upstream::dot_check_time()
{
//todonow	keep it ?

map< INT32, c_door_stream*>::iterator it;
ST_TIME		time_current;
BOOL		b_dot_clear = FALSE;
INT32		i;

	if( door_streams.size() > 0 )
		{
		n_aaatime::store( &time_current );
		it = door_streams.begin();
		i = door_streams.size();
		for( ; i>0; i--, ++it )
			{
			c_door_stream*	dot;
			dot = it->second;
			if( dot )
				{
				if( dot->get_nb() > 0 )
					if( n_aaatime::sec_get_interval( &time_current, &dot->time_begin ) > dot_ttl )
						{
						b_dot_clear = TRUE;
						break;
						}
				}
			}
		}
	if( b_dot_clear )
		clear();

}
*/

//void	c_bdd_data_upstream::treat_str( CHAR* str)
//{
//
//	CHAR*	pt;
//	INT32	id;
//	if( str_is_equal( str, "DOT_RESET", 9 ) )
//		{
//		clear();
//		}
//	else if ( str_is_equal( str, "DOT_OFF", 7 ) )
//		{
//		pt = str + 8;
//		if( sscanf( pt, "%d", &id ) == 1 )
//			dot_delete( id );
//		}
//	else if( str_is_equal( str, "DOT_ON", 6 ) )
//		{
//		REAL	vec[3];
//		pt = str + 7;
//		if( sscanf( pt, "%d %g %g", &id, &vec[0], &vec[1] ) == 4 )
//			{
//			vec[2] = 0.;
//			dot_pos( id, vec, 0 );
//			}
//		}
//	else if( str_is_equal( str, "DOT", 3 ) )
//		{
//		REAL	vec[3];
//		REAL	r;
//		pt = str + 4;
//		if( sscanf( pt, "%d %g %g %g", &id, &vec[0], &vec[1], &r ) == 4 )
//			{
//			vec[2] = 0.;
//			dot_pos( id, vec, r );
//			}
//		}
//}

void	c_bdd_data_upstream::read_data( FILE* file )
{
	if( !file )
		return;

	// todoq, function uses 16460 bytes of stack, consider moving some data to heap!!!
	CHAR*		buf = (CHAR*) MALLOC(4069*4);
	INT32		frame_nb;

	//CHAR		str_time[256];
	//ST_TIME		time_current;

	REAL		object_nb, object_id, object_depth;
	REAL		object_x, object_y, object_w, object_h, object_rot;
	REAL		f;
	INT32		tmp;

	//REAL		frame_time;
	BOOL		b_upstream_all = FALSE;
	REAL		file_time;

	seq_cur_->clear();
	
	frame_nb = -1;
	file_time = .0;
	while( asc_line::get_next_line_no_empty( file, buf, sizeof(buf) ) > 0 )
		{
		frame_nb++;
		file_time = REAL( frame_nb ) / upstream_fps_;

		CHAR*	pl;
		pl = buf;
		object_nb = 0;
		for( INT32 i = -1;; ++i )
			{
			if( *pl == ',' ||  *pl == ';')
				pl++;
			else
				{
				tmp = sscanf( pl, "%f", &f);
				if( tmp == 1)
					{
					if( i == -1 )
						object_nb = f;
					else
						switch( i % 7 )
							{
							case 0 :
								object_id = f;
								object_depth = 0;
								object_x = .0;
								object_y = .0;
								object_w = .0;
								object_h = .0;
								object_rot = .0;
								break;
							case 1 :
								object_depth = f;
								break;
							case 2 :
								object_x = f;
								break;
							case 3 :
								object_y = f;
								break;
							case 4 :
								object_w = f;
								break;
							case 5 :
								object_h = f;
								break;
							case 6 :
								object_rot = f;
								b_upstream_all = TRUE;
								break;
							}
					}
				else
					{
					//debug_break(); //SC08 - TODO faudrait voir pourquoi je passe toujours la sur fx.layers_aaa_all
					break;
					}
				do
					pl++;
				while ( *pl != ',' && *pl != ';');
				pl++;
				}
			if( i >= 0 )
				{
				if( b_upstream_all ) 
					{
					c_door_stream*	p_door_stream;
					st_door*		p_door;

					p_door_stream = seq_cur_->get_door_stream_always( object_id );
					p_door = p_door_stream->push( file_time );

					p_door->pos_[0]		= object_x / upstream_x_max_;
					p_door->pos_[1]		= 1. - object_y / upstream_y_max_;
					p_door->pos_[2]		= 0.;

					p_door->size_[0]	= object_w / upstream_x_max_;
					p_door->size_[1]	= object_h / upstream_y_max_;
					p_door->size_[2]	= 0.;

					p_door->rot_		= object_rot;
					p_door->depth_		= object_depth;
					}
				//else
				//	{

				//	}
//				n++;
//				if( object_nb > 0 )
//					{
//
//					REAL height_skew = upstream_width * SIN_DEG( object_rot );
//					//if( object_rot > 0.0 )
//					//	height_skew = -height_skew;
//					REAL factor = object_h / ( upstream_height + height_skew );
//
////							lock();
////							dot_delete(object_id );
//					dot_pos( object_id , object_x / upstream_x_max, (object_y + upstream_height * factor) / upstream_y_max,	0.0, 0.0 );
//					dot_pos( object_id , object_x / upstream_x_max, object_y/ upstream_y_max,								0.0, 0.0 );
//					if( object_rot > 90 )
//						{
//						dot_pos( object_id , (object_x - object_w) / upstream_x_max, (object_y + height_skew * factor) / upstream_y_max,	0.0, 0.0 );
//						dot_pos( object_id , (object_x - object_w) / upstream_x_max, (object_y + object_h) / upstream_y_max,		0.0, 0.0 );
//						}
//					else
//						{
//						dot_pos( object_id , (object_x + object_w) / upstream_x_max, (object_y + height_skew * factor) / upstream_y_max,	0.0, 0.0 );
//						dot_pos( object_id , (object_x + object_w) / upstream_x_max, (object_y + object_h) / upstream_y_max,		0.0, 0.0 );
//						}
//
////							unlock();
////							dot_pos( object_id, object_x / 720, object_y / 544, 0.0, 0.0);
//					//if( b_verbose && b_verbose_dot )
//					//	{
//					//	printf("%d %f %f\n", object_id, object_x, object_y );
//					//	printf("%d %f %f\n", object_id,	object_x / upstream_x_max, (object_y + upstream_height * factor ) / upstream_y_max);//,	0.0, 0.0 );
//					//	printf("%d %f %f\n", object_id, object_x / upstream_x_max, object_y/ upstream_y_max );//,								0.0, 0.0 );
//					//	printf("%d %f %f\n", object_id, (object_x + object_w) / upstream_x_max, (object_y + height_skew * factor) / upstream_y_max);//,	0.0, 0.0 );
//					//	printf("%d %f %f\n", object_id, (object_x + object_w) / upstream_x_max, (object_y + object_h) / upstream_y_max );//,		0.0, 0.0 );
//					//	}
//					}	
				b_upstream_all = FALSE;
				}
			if ( *pl == 0 )
				break;
			}
		}
	DBG_PRINT_STRING( "file_time %f", file_time );
	seq_cur_->duration_ = file_time;
	FREE_AND_NULL( buf );
}


void	c_bdd_data_upstream::load_data_from_filename( CONST CHAR* CONST filename, INT32 type_io )
{
	switch( type_io )
	{
		case TYPE_IO_BDD_DATA_UPSTREAM:
			read_data_from_file( filename );
			{
				char filename_rel[_MAX_PATH];
				//	place a relative path in the filename
				fname::get_rel_name( filename_rel, filename );
				tracking_fname_.set( filename_rel );
			}
			break;
	}
}

void c_bdd_data_upstream::load_files()
{
	struct _finddata_t			c_file;
	//vector<struct _finddata_t>	try_vec;
	long						hFile;
	//CHAR						str[_MAX_PATH];
	INT32						read_index = -1;

	//	c_file::dir_get_cwd( str, _MAX_PATH);
	seqs_clear();

	if( (hFile = _findfirst( "*", &c_file )) == -1L )
		ERR_PRINT_STRING( "No files in current directory!" );
	else
	{
		do
		{
			if( c_file.attrib & _A_SUBDIR )
			{
				DBG_PRINT_STRING( "dir %s skip", c_file.name );
			}
			else
			{
				if( strcmp( c_file.name, "Thumbs.db") != 0 && !fname::is_to_be_ignored( c_file.name ) )
				{
					seq_set( ++read_index);
					DBG_PRINT_STRING( "%s", c_file.name );
					read_data_from_file( c_file.name);
					seq_cur_->name_.set( c_file.name );
					//todo check error
					seq_nb_++;
//					try_vec.push_back( c_file);
				}
//				DBG_PRINT_STRING( "%d %-12s %.24s  %9ld",	the_vec.size(), c_file.name, ctime( &( c_file.time_write ) ), c_file.size );
			}
		}
		while( _findnext( hFile, &c_file ) == 0 );
		_findclose( hFile );
	}
	for( INT32 i = seq_nb_; i<23; ++i )	//hack
	{
		seq_set( i);
		seq_cur_->create( 1024);	//hack
	}
}

AAA_ERR	c_bdd_data_upstream::load_do_after( CONST CHAR* CONST filename)
{
AAA_ERR retcode = AAA_OK;

	c_file::push_vfile();
		c_file::dir_push_def_from_filename( filename );
		if( NOERR( c_file::dir_set_def( "upstream_data" ) ) )
			load_files();
		c_file::dir_pop_def();
		
		if( tracking_fname_.is_not_empty() )
			read_data_from_file( tracking_fname_.get() );
	c_file::pop_vfile();
		
	return retcode;
}

void	c_bdd_data_upstream::read_data_from_file( CONST char * CONST filename )
{
	FILE*	file = c_file::FOPEN( filename, "rt" );
	if( !file )
	{
		ERR_PRINT_STRING( "bdd_data_upstream, error opening tracking file");
	}
	else
	{
		read_data( file );
		c_file::FCLOSE( file );
	}
}

void	c_bdd_data_upstream::play()
{
	if( !b_play_started_ && !b_play_end_ )
		{
	//	time_cur = 0.0f;
		n_aaatime::store( &time_stamp_begin_ );
		b_play_started_ = TRUE;
		time_ui_ = 0.0f;
		//todo, this is not right
		time_stopped_ = 0.0f;
//		timestamp_ui = 0.0f;
		}
	else
		{
		// New Time
		ST_TIME		time_current;
		n_aaatime::store( &time_current );
//		DOUBLE	time_cur = time_macro_sec_get_interval(&time_current, &time_stamp_begin) / 1000000.0f;
		REAL	time_cur = n_aaatime::sec_get_interval(&time_current, &time_stamp_begin_ ) - time_stopped_;
		time_ui_ = time_cur ;
		if( time_ui_ > seq_cur_->duration_ )
			b_play_end_ = TRUE;
		}
}

void	c_bdd_data_upstream::draw_single_curve()
{
	//todo
}

void	c_bdd_data_upstream::draw_door_curve()
{
	//todo
}

void	c_bdd_data_upstream::draw_box_curve()
{
	//todo
}

void	c_bdd_data_upstream::draw_traj_curve()
{
	//todo
}

void	c_bdd_data_upstream::draw_multiple_curve()
{
	//todo
}

