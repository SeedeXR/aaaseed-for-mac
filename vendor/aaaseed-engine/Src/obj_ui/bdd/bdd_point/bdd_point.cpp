#include "bdd_point.h"
#include "obj_ui/com/net.h"
#include "gol/gol.h"
#include "gol/gol_draw.h"
#include "draw/model.h"
#include "math/rand.h"
#include "draw/color.h"
#include "infrastructure/seedfile.h"
#include "asc_line.h"
#include "obj_ui/bdd/bdd_old/aichi.h"


class	c_dot
{
public:
	struct	st_dot
	{
		REAL				pos[3];
		REAL				pos_filtered[3];
		REAL				speed[3];
		REAL				speed_norm;
		REAL				radius;
		aaa::time::ST_TIME	time;
		bool				b_moving;
	};
private:
	std::deque<st_dot>	_data;
	aaa::time::ST_TIME	_timestamp;
	bool				_b_moving_last;
	INT32				_moving_count;

public:
	c_dot();
	~c_dot();

			void				init();
			void				deinit();

			void				push_front(		c_bdd_point* CONST bdd_point, REAL CONST * CONST p, REAL CONST r );

	FINLINE	INT32				get_nb()		CONST	{	return (INT32)_data.size();	}
			void				get_raw(		INT32 CONST index, REAL* CONST p, REAL& r );
			void				get_filtered(	INT32 CONST index, REAL* CONST  p, REAL& r );
			void				get_average(	INT32 CONST index, INT32 nb, REAL* CONST p, REAL& r );
	FINLINE	aaa::time::ST_TIME&	get_timestamp()			{	return _timestamp;	}

	FINLINE	st_dot*				get_pdot(		INT32 CONST index );

			void				update(			c_bdd_point* bdd_point );

	FINLINE	bool				is_moving()		CONST	{	return _b_moving_last; };

};

FINLINE c_dot::st_dot*	c_dot::get_pdot( INT32 CONST index )
{
	return (index < get_nb()) ? &_data[index] : nullptr;
}

void	c_dot::update( c_bdd_point* bdd_point )
{
	INT32 nb = get_nb() - bdd_point->_dot_sample_nb_max;
	for(;nb>0;--nb)
		_data.pop_back();

	struct	st_dot*	d = get_pdot( 0 );
	if( d )
	{
		REAL s = norm_v3r( d->speed );
		d->speed_norm = s;
		if( _b_moving_last )
		{
			if( s < bdd_point->_speed_stop_threshold )
			{
				_b_moving_last = false;
				_moving_count = 0;
			}
		}
		else
		{
			if( s > bdd_point->_speed_start_threshold )
			{
				++_moving_count;
				if( _moving_count > bdd_point->_speed_start_count)
					_b_moving_last = true;
			}
			else
				_moving_count = 0;
		}
		d->b_moving = _b_moving_last;
	}
	else
		debug_break( "c_dot::update() empty point");
}



void	c_dot::push_front( c_bdd_point* CONST bdd_point, REAL CONST * CONST p, REAL CONST r )
{
	aaa::time::store( &_timestamp );

	struct	st_dot	dot0;
	aaa::time::store( &dot0.time );
	cpy_v3( dot0.pos, p );
	dot0.radius = r;

	struct	st_dot* d1 = get_pdot( 0 );
	if( d1 )
	{
		// get delta time
		DOUBLE dt = aaa::time::get_interval_sec( _timestamp, d1->time );
		if( dt != 0. )
		{
			REAL	speed[3];
			REAL	acc[3];
			// compute speed
			sub_v3( speed, p, d1->pos_filtered );
			scale_v3( speed, 1./dt );
			// compute accel
			sub_v3( acc, speed, d1->speed );
			scale_v3( acc, 1./dt);
			// limit acc
			if( bdd_point->_b_filter_active )
				limit_v3r( acc, bdd_point->_limit_accel );
			// recompute speed
			add_scale_v3( speed, d1->speed, acc, dt );
			// limit speed
			if( bdd_point->_b_filter_active )
				limit_v3r( speed, bdd_point->_limit_speed );
			// store speed
			cpy_v3( dot0.speed, speed );
			// recompute pos
			scale_v3( speed, dt );
			if( bdd_point->_b_filter_active )
			{
				// limit move
				limit_v3r( speed, bdd_point->_limit_dist);
				//	don't use all
				scale_v3( speed, bdd_point->_inter);
			}
			// move
			add_v3( dot0.pos_filtered, d1->pos_filtered, speed );
		}
		else
		{
			clear_v3( dot0.speed );
			cpy_v3( dot0.pos_filtered, d1->pos_filtered );
		}
	}
	else
	{
		cpy_v3( dot0.pos_filtered, p );
		clear_v3( dot0.speed );
	}
	_data.push_front( dot0 );

	update( bdd_point );
}

void	c_dot::get_raw( INT32 CONST index, REAL* CONST p, REAL& r )
{
	if( index < get_nb() )
	{
		cpy_v3( p, _data[index].pos );
		r = _data[index].radius;
	}
	else
		clear_v3( p );
}

void	c_dot::get_average( INT32 CONST index, INT32 nb, REAL* CONST p, REAL& r )
{
	if( index < get_nb() )
	{
		REAL	vec[3];
		
		nb = MAX1( nb );
		INT32 index_end = CLAMP( index+nb-1, 0, get_nb()-1 );
		cpy_v3( vec, _data[index].pos );
		for( INT32 i=index+1; i<=index_end; i++ )
			add_v3( vec, _data[i].pos );

		scale_v3( p, vec, 1./REAL(nb) );
	}
	else
		clear_v3( p );
}

void	c_dot::get_filtered( INT32 CONST index, REAL* CONST p, REAL& r )
{
	if( index < get_nb() )
	{
		cpy_v3( p, _data[index].pos_filtered );
		r = _data[index].speed_norm;	//hack was radius
	}
	else
	{
		clear_v3( p );
		r = 0.;
	}
}

void	c_dot::init()
{
	_b_moving_last	= false;
	_moving_count	= 0;
}

c_dot::c_dot()
{
	init();
}

void	c_dot::deinit()
{
	_data.clear();
}

c_dot::~c_dot()
{
	deinit();
}

FACTORY_CREATE_PROP_V1( c_bdd_point, bdd_point, Points, bdd_point, sub_menu="Point"; );

c_bdd_point*	bdd_point_cur = nullptr;

namespace	n_bdd_point
{
	CONSTEXPR INT32	BASE_PARAM_NB		= 16 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32	FILTER_PARAM_NB		= 5;
	CONSTEXPR INT32	ANALYSE_PARAM_NB	= 12;
	CONSTEXPR INT32	DRAW_PARAM_NB		= 9;
	CONSTEXPR INT32	PLAYBACK_PARAM_NB	= 9;
	CONSTEXPR INT32	DEBUG_PARAM_NB		= 3;
	CONSTEXPR INT32	GROUP_PARAM_NB		= 5;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+ FILTER_PARAM_NB
									+ ANALYSE_PARAM_NB
									+ DRAW_PARAM_NB
									+ PLAYBACK_PARAM_NB
									+ DEBUG_PARAM_NB
									+ GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS

		PARAM_DEF_POINT_XYZ(	origin		)	
		PARAM_DEF_POINT_XY(		offset		)
		PARAM_DEF_SCALE_XY(		sca			)
		PARAM_DEF_POINT_XY(		tra			)

		PARAM_DEF_BOOL_OFF(		restart_trig )	//todo should be restart_trig
		PARAM_DEF_REAL_POS(		time_to_live,					1., 2.	)
		PARAM_DEF_INT32(		sample_nb_max,					1, 64,	1, 8*1024*1024 	)

		PARAM_DEF_BOOL_ON(		net_receive		)
		PARAM_DEF_BOOL_OFF(		net_send		)
		PARAM_DEF_BOOL_OFF(		net_send_filtered )
//		{	nullptr,	PARAM_INT32,	"datagrid_dst",			1, 0,	0, DATAGRID_NB-1,	nullptr, nullptr },	

		PARAM_DEF_GROUP_CLOSED( Filter, FILTER_PARAM_NB )
//			{	nullptr,	PARAM_INT32,	"average_size",		1, 2,	1, 128.,			nullptr, nullptr },	
			PARAM_DEF_BOOL_OFF(			filter_active			)
			PARAM_DEF_REAL_POS(			inter,					1., .5	)
			PARAM_DEF_REAL_POS_ONE(		limit_dist				)
			PARAM_DEF_REAL_POS_ONE(		limit_speed				)
			PARAM_DEF_REAL_POS_ONE(		limit_acceleration		)

		PARAM_DEF_GROUP_CLOSED( Analyze, ANALYSE_PARAM_NB )	
			PARAM_DEF_BOOL_OFF(			analyse_active			)
			PARAM_DEF_REAL_POS(			speed_stop_threshold,	0, 1	)
			PARAM_DEF_REAL_POS(			speed_start_threshold,	0, 2	)
			PARAM_DEF_INT32_POS(		speed_start_count,		0, 2	)
			PARAM_DEF_INT32_LOCKED(		points_nb				)
			PARAM_DEF_INT32_LOCKED(		points_nb_moving		)
			PARAM_DEF_REAL_LOCKED(		speed_min				)
			PARAM_DEF_REAL_LOCKED(		speed_max				)
			PARAM_DEF_REAL_LOCKED(		speed_sigma_moving		)
			PARAM_DEF_REAL_LOCKED(		speed_sigma				)
			PARAM_DEF_BOOL_OFF(			net_send_analyse		)
			PARAM_DEF_INT32_POS_ZERO(	net_send_analyse_skip_nb	)

		PARAM_DEF_GROUP_CLOSED( Draw, DRAW_PARAM_NB )
			PARAM_DEF_REAL_POS_ONE(		time_interval		)
			PARAM_DEF_REAL_POS_ONE(		max_time_interval	)
			PARAM_DEF_INT32_POS(		max_sample,			1, 1024	)
			PARAM_DEF_BOOL_ON(			draw_line			)
			PARAM_DEF_BOOL_OFF(			draw_point			)
			PARAM_DEF_BOOL_ON(			draw_raw			)
			PARAM_DEF_BOOL_OFF(			draw_filtered		)
			PARAM_DEF_BOOL_OFF(			draw_link			)
			PARAM_DEF_BOOL_OFF(			draw_multiple_raw	)

		PARAM_DEF_GROUP( Playback, PLAYBACK_PARAM_NB )
			PARAM_DEF_FILENAME(			tracking_file, aaa::file::TYPE_IO_BDD_POINT, 0 )
			PARAM_DEF_REAL_SAVE_NOT(	time,				1, 0,		0, PARAM_MAX_REAL	)
			PARAM_DEF_REAL_SAVE_NOT(	timestamp,			1, 0,		0, PARAM_MAX_REAL	)
			PARAM_DEF_BOOL_ON(			play				)
			PARAM_DEF_BOOL_ON(			loop				)
			PARAM_DEF_BOOL_OFF(			play_trig			)
			PARAM_DEF_BOOL_OFF(			verbose				)
			PARAM_DEF_BOOL_ON(			verbose_time		)
			PARAM_DEF_BOOL_ON(			verbose_dot			)

			PARAM_DEF_REAL_POS_ZERO(	trax_default_y		)
		PARAM_DEF_GROUP( Debug, DEBUG_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			debug				)
			PARAM_DEF_INT32_LOCKED(		points_nb_free		)
			PARAM_DEF_INT32_LOCKED(		points_frame_nb_max	)
	};
}

//INT32	average_size;
void	c_bdd_point::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt_3( h, _origin );

	param_set_pt_2(	h, _offset					);
	param_set_pt_2(	h, _sca						);
	param_set_pt_2(	h, _tra						);
	param_set_pt(		h, _b_restart_trig_ui		);
	param_set_pt(		h, _dot_time_to_live		);
	param_set_pt(		h, _dot_sample_nb_max		);

	param_set_pt(		h, _b_net_receive_ui		);
	param_set_pt(		h, _b_net_send_ui			);
	param_set_pt(		h, _b_net_send_filtered_ui	);
	
	++h;
	//	param_set_pt( h, average_size);
		param_set_pt( h, _b_filter_active			);
		param_set_pt( h, _inter						);
		param_set_pt( h, _limit_dist				);
		param_set_pt( h, _limit_speed				);
		param_set_pt( h, _limit_accel				);

	++h;
		param_set_pt( h, _b_analyse_active_ui		);
		param_set_pt( h, _speed_stop_threshold		);
		param_set_pt( h, _speed_start_threshold		);
		param_set_pt( h, _speed_start_count			);	
		param_set_pt( h, _points_nb					);
		param_set_pt( h, _points_nb_moving			);
		param_set_pt( h, _speed_min					);
		param_set_pt( h, _speed_max					);
		param_set_pt( h, _speed_sigma				);
		param_set_pt( h, _speed_sigma_moving		);
		param_set_pt( h, _b_net_send_analyse_ui		);
		param_set_pt( h, _net_send_analyse_skip_nb	);

	++h;
		param_set_pt( h, _draw_interval				);
		param_set_pt( h, _max_interval				);
		param_set_pt( h, _max_sample				);
		param_set_pt( h, _b_draw_line_ui			);
		param_set_pt( h, _b_draw_point_ui			);
		param_set_pt( h, _b_draw_raw_ui				);
		param_set_pt( h, _b_draw_filtered_ui		);
		param_set_pt( h, _b_draw_link_ui			);
		param_set_pt( h, _b_draw_multiple_raw_ui	);

	++h;
		param_set_pt( h, _tracking_fname			);
		param_set_pt( h, _time_ui					);
		param_set_pt( h, _timestamp_ui				);
		param_set_pt( h, _b_play_ui					);
		param_set_pt( h, _b_play_loop_ui			);
		param_set_pt( h, _b_play_restart_ui			);
		param_set_pt( h, _b_verbose					);
		param_set_pt( h, _b_verbose_time			);
		param_set_pt( h, _b_verbose_dot				);
	
		param_set_pt( h, _trax_default_y			);

	++h;
		param_set_pt( h, _b_debug_ui				);
		param_set_pt( h, _points_nb_free			);
		param_set_pt( h, _dot_data_max_nb			);
		

	
//	speed_threshold

	err_param_init_pt(h);
}

void	c_bdd_point::lock()
{
	_lock.lock();
}

void	c_bdd_point::unlock()
{
	_lock.unlock();
}

void c_bdd_point::init()
{
	_f_tracking = nullptr;
	_time_stamp = 0.0f;
	_time_stopped = 0.0f;
	_b_play_started = false;
	_b_play_stopped = false;
	_b_play_end = false;
	_time_ui = 0.0f;
	_timestamp_ui = 0.0f;
	_frame_index = 0;

	_points_nb = 0;
	_points_nb_moving = 0;
	_speed_min = 0;
	_speed_max = 0;
	_speed_sigma = 0;
	_speed_sigma_moving = 0;
}

CONSTRUCTOR_CREATE( c_bdd_point )
{
	param_init_with( n_bdd_point::param, n_bdd_point::PARAM_NB_MAX );
	init();
	clear_dots();
	alloc();
}

void c_bdd_point::alloc()
{
/*	if( nb_allocated_ui != boids.get_nb_allocated() )
	{
		pos = (REAL *) REALLOC( pos, nb_allocated_ui*3*sizeof(REAL) );
		boids.alloc( nb_allocated_ui);
	}
*/
}

void c_bdd_point::dealloc()
{
	for( auto const & p_dot : _dots_free )
		delete p_dot;
	_dots_free.clear();
}

c_bdd_point::~c_bdd_point()
{
	if( _f_tracking )
		c_file::FCLOSE( _f_tracking );
	clear_dots();
	dealloc();
}

void	c_bdd_point::clear_dots()
{
	for( auto const & p : _dots )
	{
		c_dot* p_dot = p.second;
		p_dot->deinit();
		_dots_free.push_back( p_dot );
	}
	_dots.clear();
#if	APP_SPECIAL_AICHI()
	if( aichi )
		aichi->dot_clear();
#endif
	for( INT32 i=0; i<DOTS_ID_FOR_TRAXS_NB; ++i )
		_dots_id_for_traxs[i] = 0;
}

void	c_bdd_point::update()
{
	std::lock_guard<c_bdd_point> guard(*this);

	bdd_point_cur = this;

	//can't receive and send for now (feedback)
	if( _b_net_receive_ui )
		_b_net_send_ui = false; 
	if( _b_net_send_ui )
		_b_net_receive_ui = false;

	if( _b_restart_trig_ui )
	{	//todo
		clear_dots(); 
		_b_restart_trig_ui = false;
	}
	if( _b_play_restart_ui )
	{
		play_restart();
		_b_play_restart_ui = false;
	}
	if( _b_play_ui && !_b_play_end )
		play_from_file();
	else
		play_stop();

	_points_nb		= (INT32)_dots.size();
	if( _b_debug_ui )
		_points_nb_free = (INT32)_dots_free.size();

	check_dot_time();

	c_model::cur->get_size_v3(_size);

	if( _b_analyse_active_ui )
		analyse();

	if( _b_net_send_ui && net )	
		net->osc_flush_async( 1 );	//make sure the flush is done only here
/*
c_boids::boid_bulk::iterator	it;
REAL*	p;

	//	get all position and number
	p = pos;
	for( it = boids.living.begin(); it != boids.living.end(); it++, p+=3 )
		cpy_v3( p, (*it)->pos);
	nb_to_draw = (p-pos)/3;	// avoid this : nb_to_draw = boids.living.size();

	//	deform if needed
	if( c_def_node::get_cur()->is_deforming() )
		c_def_node::get_cur()->apply( pos, nb_to_draw );

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

void	c_bdd_point::analyse()
{
	auto	it = _dots.begin();
	REAL	norm;

	INT32 nb = (INT32)_dots.size();
	INT32 i = nb;
	_speed_sigma = 0;
	_speed_sigma_moving = 0;
	_points_nb_moving = 0;
	_speed_min = std::numeric_limits<REAL>::max();
	_speed_max = 0;

	for( ; i>0; --i, ++it )	//, p+=3 )
	{
		c_dot* dot = it->second;
		c_dot::st_dot* pd = dot->get_pdot(0);
		norm = pd->speed_norm;
		_speed_sigma += norm;
		if( dot->is_moving() )
		{
			_speed_sigma_moving += norm;
			++_points_nb_moving;
		}
		_speed_min = MIN( _speed_min, norm );
		_speed_max = MAX( _speed_max, norm );
//		dot->update();	// NO DONE IN PUSH
	}
	if( _b_net_send_analyse_ui )	//done only when analyse
	{	
		++_frame_index;
		if( (_net_send_analyse_skip_nb == 0) || IMOD( _frame_index, _net_send_analyse_skip_nb+1) == 0 )
		{
			CHAR	buf[256];
			sprintf( buf, "DOTS_INFO %d %d %d %.2f %.2f %.2f",
				_frame_index, _points_nb, _points_nb_moving, _speed_sigma, _speed_sigma_moving, _speed_max);
			net->send_osc_str( 2, buf);
		}
	}
}

void	c_bdd_point::draw_single()
{
	std::lock_guard<c_bdd_point> guard(*this);

	_nb_to_draw = (INT32)_dots.size();
	//if( c_multiple::cur )
	//	c_multiple::cur->set_nb( nb_to_draw );

	INT32	i = _nb_to_draw;
	auto it = _dots.begin();
	for( ; i>0; --i, ++it )	//, p+=3 )
	{
		REAL		p[4];
		//c_multiple::cur->set_index( it->first );
		c_dot*	dot = it->second;
		INT32	nb = MIN( dot->get_nb(), _max_sample );

		if( _b_draw_line_ui )
		{
			if( _b_draw_raw_ui )
			{
				GOL::color4( 0,0,1, .5 );
				GOL::begin( GL_LINE_STRIP );
				for( INT32 j=0; j<nb; ++j )
				{
					c_dot::st_dot* pd = dot->get_pdot( j );
					DOUBLE dt = aaa::time::get_interval_sec( dot->get_timestamp(), pd->time );
					if( dt > _draw_interval )
						break;
					mul_add_v3( p, pd->pos, _size, _origin );
					GOL::vertex3v(p);
				}
				GOL::end();
			}

			if( _b_draw_filtered_ui )
			{
				DOUBLE dt_last = 0.;
//				GOL::color4( 0,1,0, .5);
				c_color::get_cur()->draw();
				GOL::begin( GL_LINE_STRIP );
				for( INT32 j=0; j<nb; ++j )
				{
					c_dot::st_dot* pd = dot->get_pdot( j);
					DOUBLE dt = aaa::time::get_interval_sec( dot->get_timestamp(), pd->time );
					if( dt > _draw_interval )
						break;
					if( (dt-dt_last) > _max_interval )
						break;
					dt_last = dt;
					mul_add_v3( p, pd->pos_filtered, _size, _origin);
					GOL::vertex3v(p);
				}
				GOL::end();
			}
		}
		if( _b_draw_point_ui )
		{
			if( _b_draw_raw_ui )
			{
				GOL::color4( 0, 0, 1, .5);
				GOL::set_point_size( 1.);
				GOL::begin( GL_POINTS );
				for( INT32 j=0; j<nb; ++j )
				{
					c_dot::st_dot* pd = dot->get_pdot( j);
					DOUBLE dt = aaa::time::get_interval_sec( dot->get_timestamp(), pd->time );
					if( dt > _draw_interval )
						break;
					mul_add_v3( p, pd->pos, _size, _origin);				
					GOL::vertex3v(p);
				}
				GOL::end();
			}

			if( _b_draw_filtered_ui )
			{
				DOUBLE dt_last = 0;
				c_color::get_cur()->draw();
//				GOL::color4( 0,1,0, .5);
				GOL::begin( GL_POINTS );
				for( INT32 j=0; j<nb; ++j )
				{
					c_dot::st_dot* pd = dot->get_pdot( j);
					DOUBLE dt = aaa::time::get_interval_sec( dot->get_timestamp(), pd->time );
					if( dt > _draw_interval )
						break;
					if( (dt-dt_last) > _max_interval )
						break;
					dt_last = dt;
					mul_add_v3( p, pd->pos_filtered, _size, _origin);
					GOL::set_point_size( pd->b_moving ? FP32(4): FP32(1) );

					GOL::vertex3v(p);
				}
				GOL::end();
			}
		}
		if( _b_draw_link_ui )
		{
			GOL::begin( GL_LINES );
				for( INT32 j=0; j<nb; ++j )
				{
					c_dot::st_dot* pd = dot->get_pdot( j);
					REAL dt = REAL(aaa::time::get_interval_sec( dot->get_timestamp(), pd->time ));
					if( dt > _draw_interval )
						break;
					GOL::color4( 1,0,0, REAL(.25)*(_draw_interval-dt)/_draw_interval );
					mul_add_v3( p, pd->pos, _size, _origin);
					GOL::vertex3v(p);
					mul_add_v3( p, pd->pos_filtered, _size, _origin);
					GOL::vertex3v(p);
				}
			GOL::end();
		}
	}
}

void	c_bdd_point::draw_multiple()
{
	std::lock_guard<c_bdd_point> guard(*this);

	auto it = _dots.begin();
	_nb_to_draw = (INT32)_dots.size();
	c_multiple::cur->set_nb( _nb_to_draw );
	INT32 i = _nb_to_draw;
/*
	if( c_multiple::cur->is_align_normal() )
	{
		REAL	nor[3];
		REAL	u[3];
		REAL	v[3];

		for( ; i>0; --i, ++it )	//, p+=3 )
		{
			c_multiple::cur->index_set( (*it)->first );
			normalize_v3r( nor, (*it)->speed );
			cross_normalize_v3r( v, nor, (*it)->speed_old );
			cross_normalize_v3r( u, nor, v);
//			normalize_v3r( nor, b->speed );
//			make_axe( u, v, nor);
			multiple_draw_one_at_tgn( p, u, v, nor );
		}
	}
	else
*/
	{
		for( ; i>0; --i, ++it )	//, p+=3 )
		{
			c_dot*	dot;
			REAL	p[4];

			c_multiple::cur->set_index( it->first );
			dot = it->second;
			if( _b_draw_multiple_raw_ui )
				dot->get_raw( 0, p, p[3] );
			else
				dot->get_filtered( 0, p, p[3] );
			mul_add_v3( p, _size, _origin );
			c_multiple::cur->align_then_draw( p );
		}
	}
}

INT32	c_bdd_point::get_point_nb()
{
	return _nb_to_draw;
}

bool	c_bdd_point::get_point( REAL* CONST dst, INT32 CONST index )
{
	auto it = _dots.find( index + 1 );
	if( it == _dots.end() )
	{
		clear_v3( dst );
		//ERR_PRINT_STRING( "%s() can find dot", __FUNCTION__ );
		return false;
	}

	{
		REAL dummy;
		it->second->get_raw( 0, dst, dummy );
		return true;
	}
}

namespace {
	c_rand_lin	rnd;
}

bool	c_bdd_point::get_point_and_speed_rnd_time_rel( REAL* CONST dst, REAL* CONST speed, REAL t_in )
{
	INT32 nb = (INT32)_dots.size();
	c_dot* dot = nullptr;
	
	if( nb >= 1 )
	{
/*		if (nb == 1 )
			{
			i = 0;
			}
		else
*/
		{
			INT32	nb_try;
			nb_try = 0;
			do
			{
				++nb_try;
				INT32 i = rnd.get_uint32() >> 12;
				if( i < 0 )
					i = -i;		
				i = IMOD( i, nb );
				auto it = _dots.begin();
				for( ; i>0; --i )
					++it;				
				dot = it->second;
				if( dot->is_moving() )
					break;
			}
			while( nb_try < 100 );
	//		printf( "i %d nb %d\n", i, nb);�
		}

	}
	if( dot )
	{
		c_dot::st_dot*	pd = dot->get_pdot( 0 );
		if( pd )
		{
			mul_add_v3( dst, pd->pos_filtered, _size, _origin);
			mul_v3( speed, pd->speed, _size);
			return true;
		}
	}
	
	clear_v3( dst );
	clear_v3( speed );
	return false;
}

INT32	c_bdd_point::ctl_index_from_id( INT32 id )
{
	for( INT32 i=0; i<DOTS_ID_FOR_TRAXS_NB; ++i )
	{
		if( _dots_id_for_traxs[i] == id )
			return i;
	}
	return -1;
}

void	c_bdd_point::ctl_get_pos( REAL* dst, INT32 index, bool b_filtered )
{
	INT32	id;
	IF_THIS_NULL()
		set_v3( dst, REAL_BIG_VALUE );
	else
	{
		index = IMOD( index, DOTS_ID_FOR_TRAXS_NB );
		id = _dots_id_for_traxs[index];
		if( id )
		{	//todo this really heavy
			auto it = _dots.find( id );
			if ( it == _dots.end() )
				ERR_PRINT_STRING( "bdd_point can find dot" );
			else
			{
				c_dot*	dot;
				REAL dummy;
				dot = it->second;
				if( b_filtered )
					dot->get_filtered( 0, dst, dummy );
				else
					dot->get_raw( 0, dst, dummy );
				mul_add_v3( dst, _size, _origin);
			}
		}
		else
		{
			*dst = REAL(index)*REAL(.5) - REAL(4);
			*++dst = _trax_default_y;
			*++dst = 0;
		}
	}
}

void	c_bdd_point::new_dot( INT32 CONST id )
{
	c_dot*	p_dot;
	if( _dots_free.empty() )
		p_dot = new c_dot;
	else
	{
		p_dot = _dots_free.back();
		p_dot->init();
		_dots_free.pop_back();
	}
	_dots[id] = p_dot;

	INT32	ind = ctl_index_from_id( 0 );
	if ( ind >= 0 )
		_dots_id_for_traxs[ind] = id;
}

void	c_bdd_point::new_dot( INT32 CONST id, REAL CONST x, REAL CONST y, REAL CONST z, REAL CONST r )
{
	set_dot_pos( id, x,y,z, r );
}

void	c_bdd_point::delete_dot( INT32 CONST id )
{
	// If no match is found for the key, end() is returned
	auto it = _dots.find( id );
	if ( it == _dots.end() )
		ERR_PRINT_STRING( "bdd_point can find dot to delete" );
	else
	{
#if	APP_SPECIAL_AICHI()
		if( aichi )
			aichi->dot_delete( id );
#endif
		c_dot*	dot = it->second;
		_dots_free.push_back( dot);
		dot->deinit();
		_dots.erase( it);
		INT32	ind = ctl_index_from_id( id );
		if ( ind >= 0 )
			_dots_id_for_traxs[ind] = 0;
	}
	if( _b_net_send_ui )
	{
		CHAR	buf[256];
		sprintf( buf, "DOT_OFF %d", id );
		net->send_osc_str( 1, buf);
	}
}

void	c_bdd_point::set_dot_pos( INT32 CONST id, REAL CONST * CONST p, REAL r )
{
	bool	b_dot_on;

	auto it = _dots.find( id );
	if( it == _dots.end() )
	{
		//generate dot
		b_dot_on = true;
		new_dot( id );
		it = _dots.find( id );
		if ( it == _dots.end() )
		{
			ERR_PRINT_STRING( "bdd_point can find dot to set pos" );
			return;
		}
	}
	else
		b_dot_on = false;

	c_dot*	dot = it->second;
#if	APP_SPECIAL_AICHI()
	if( aichi && aichi->b_dot_flip_y )
		p[1] = -p[1];
#endif
	dot->push_front( this, p, r );

	if( _b_net_send_ui )
	{
		CHAR	buf[256];
		REAL	p_filtered[3];
		REAL CONST * pt;
		if( _b_net_send_filtered_ui )
		{
			dot->get_filtered( 0, p_filtered, r );
			pt = p_filtered;
		}
		else
			pt = p;
#if	APP_SPECIAL_AICHI()
		if( aichi && aichi->b_dot_flip_y )
			p[1] = -p[1];
#endif	
		if( b_dot_on )
			sprintf( buf, "DOT_ON %d %.2f %.2f", id, p[0], p[1] );
		else
			sprintf( buf, "DOT %d %.2f %.2f %.2f", id, p[0], p[1], r );
		net->send_osc_str( 1, buf );
	}
}

bool	c_bdd_point::set_point(		INT32 index, REAL CONST * src )
{
	set_dot_pos( index+1, src, 1 );
	return true;
}

void	c_bdd_point::check_dot_time()
{
	if( _dots.size() <= 0 )
		return;

	bool		b_dot_clear = false;
	aaa::time::ST_TIME		time_current;
	aaa::time::store( &time_current );
	_dot_data_max_nb = 0;
	for( auto const & p : _dots )
	{
		c_dot* p_dot = p.second;
		if( p_dot )
		{
			_dot_data_max_nb = MAX( _dot_data_max_nb, p_dot->get_nb() );
			if( p_dot->get_nb() > 0 )
			{
				if( aaa::time::get_interval_sec( time_current, p_dot->get_timestamp() ) > _dot_time_to_live )
				{
					b_dot_clear = true;
					break;
				}
			}
		}
	}
	if( b_dot_clear )
		clear_dots();
}

void	c_bdd_point::treat_str( CHAR* str )
{
	std::lock_guard<c_bdd_point> guard(*this);

	CHAR*	pt;
	INT32	id;
	if( str_is_equal( str, "DOT_RESET", 9 ) )
	{
		clear_dots();
	}
	else if ( str_is_equal( str, "DOT_OFF", 7 ) )
	{
		pt = str + 8;
		if( sscanf( pt, "%d", &id ) == 1 )
			delete_dot( id );
	}
	else if( str_is_equal( str, "DOT_ON", 6 ) )
	{
		REAL	vec[3];
		pt = str + 7;
		if( sscanf( pt, "%d %g %g", &id, &vec[0], &vec[1] ) == 4 )
		{
			vec[2] = 0.;
			set_dot_pos( id, vec, 0 );
		}
	}
	else if ( str_is_equal( str, "DOT", 3 ) )
	{
		REAL	vec[3];
		REAL	r;
		pt = str + 4;
		if( sscanf( pt, "%d %g %g %g", &id, &vec[0], &vec[1], &r ) == 4 )
		{
			vec[0] = ( (vec[0]+_offset[0]) * _sca[0] ) + _tra[0];
			vec[1] = ( (vec[1]+_offset[1]) * _sca[1] ) + _tra[1];
			vec[2] = 0.;
			set_dot_pos( id, vec, r );
		}
	}
}

void	c_bdd_point::receive_osc( CHAR* str )
{
	if( _b_net_receive_ui )
		treat_str( str );
}

AAA_ERR	c_bdd_point::load_data_from_filename( o_str CONST & filename, INT32 type_io )
{
	switch(type_io)
	{
	case aaa::file::TYPE_IO_BDD_POINT:
		read_tracking_from_file( filename );
		//	place a relative path in the filename
		_tracking_fname.set_fname_relative( filename );
		return AAA_OK;
	}
	return ERR_TYPE_UNUSED;
}

AAA_ERR	c_bdd_point::load_do_after( o_str CONST & filename )
{
	AAA_ERR retcode = AAA_OK;

	if( !_tracking_fname.is_empty() )
		read_tracking_from_file( _tracking_fname );
		
	return retcode;
}

void	c_bdd_point::read_tracking_from_file( o_str CONST & filename )
{
	if( _f_tracking )
	{
		c_file::FCLOSE( _f_tracking );
		_f_tracking = nullptr;
	}

	_f_tracking = c_file::FOPEN( filename, "rt" );
	if( !_f_tracking )
	{
		ERR_PRINT_STRING( "bdd_point, error opening tracking file" );
	}
	else
	{
		_time_stamp = 0.0f;
	}
}

void	c_bdd_point::play_init()
{
	_time_stamp = 0.0f;
	_time_stopped = 0.0f;
	_time_ui = 0.0f;
	_timestamp_ui = 0.0f;
	_b_play_started = false;
	if( _f_tracking )
		c_file::FSEEK_SET( _f_tracking, 0 );
}

void	c_bdd_point::play_restart()
{
	play_init();
	_b_play_end = false;
}

void	c_bdd_point::play_stop()
{
	if( _b_play_started && !_b_play_stopped )
	{
		_b_play_stopped = true;
		aaa::time::store( &_time_stop );
	}
}

void	c_bdd_point::play_end()
{
	play_init();

	_b_play_end = true;
	//	b_play = false;
}

void	c_bdd_point::play_from_file()
{
	CHAR				buf[1024];
	CHAR				str_time[256];
	aaa::time::ST_TIME	time_current;

	if( _f_tracking )
	{
		if( !_b_play_started && !_b_play_end )
		{
			//	time_cur = 0.0f;
			aaa::time::store( &_time_stamp_begin );
			_b_play_started = true;
			_time_ui = 0.0f;
			_timestamp_ui = 0.0f;
		}
		else
		{
			if( _b_play_stopped )
			{
				aaa::time::store( &time_current );
				_time_stopped += REAL(aaa::time::get_interval_sec( time_current, _time_stop ));
				_b_play_stopped = false;
			}
		}

		aaa::time::store( &time_current );
		//	DOUBLE	time_cur = time_macro_sec_get_interval(&time_current, &time_stamp_begin) / 1000000.0f;
		REAL	time_cur = REAL(aaa::time::get_interval_sec( time_current, _time_stamp_begin )) - _time_stopped;
		_time_ui = time_cur ;
		DOUBLE	time = 0.0f;
		while( _time_stamp <= time_cur && !_b_play_end )
		{
			_timestamp_ui = _time_stamp ;
			if( asc_line::get_next_line_no_empty( _f_tracking, buf, 1024) > 0 )
			{
				// test for new time_stamp
				if( str_is_equal( buf, "TIME_STAMP", 10 ) )
				{
					sscanf( buf, "%256s %lf\n", &str_time, &time );
					_time_stamp = REAL(time);
					if( _b_verbose && _b_verbose_time )
						VERBOSE_PRINT_STRING( "Time stamp : %f\tTime Current : %f", time, time_cur );
				}
				else if(strncmp( buf, "DOT", 3) == 0 ) // test for DOT, else discard line
				{	
					treat_str( buf);
					if( _b_verbose && _b_verbose_dot )
						VERBOSE_PRINT_STRING( "%s", buf );
				}
			}
			else
			{
				// end of file
				if( _b_play_loop_ui )
				{
					// restart play
					play_restart();
				}
				else
				{
					// stop
					play_end();
				}
			}
		}
	}
}