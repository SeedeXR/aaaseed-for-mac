
#include "speed.h"
#include "infrastructure/param/param_declare.h"
#include "infrastructure/namer.h"


//ST_SPEED*	speed_alloc()
//{
//	ST_SPEED* sp = new ST_SPEED;
//	sp->print_stat_on = false;
//	return	sp;
//}
//
//void		speed_free( ST_SPEED* sp )
//{
//	delete sp;
//}
//
//void	speed_frequency_set( ST_SPEED* sp, UINT32 monitor_f, UINT32 video_f )
//{
//	if( sp )
//	{
//		sp->monitor_freq = monitor_f;
//		sp->video_freq = video_f;
//	}
//}
//
//ST_SPEED*	speed_init( ST_SPEED* sp )
//{
//	if( !sp )
//		sp = speed_alloc();
//	sp->hit = 0;
//	sp->total = 0;
//	sp->min = aaa::BIGGEST<UINT32>;
//	sp->max = 0;
//	speed_frequency_set( sp, 76, 50 );
//	return	sp;
//}
//
//UINT32		speed_store_interval( ST_SPEED* sp, UINT32 interval )
//{
//	sp->hit++;
//	sp->interval = interval;
//	sp->total += interval;
//	if( interval < sp->min )	sp->min = interval;
//	if( interval > sp->max )	sp->max = interval;
//	if( sp->hit >= 100 )
//	{
//		if( sp->print_stat_on )
//			speed_print_stat( sp );
//		speed_init( sp );
//	}
//	return	interval;
//}
//
//ST_TIME *	speed_begin( ST_SPEED* sp )
//{
//	aaa::time::store(&sp->start);
//	return( &sp->start);
//}
//
//UINT32		speed_end( ST_SPEED* sp )
//{
//	aaa::time::store(&sp->last);
//	UINT32	interval = aaa::time::macro_sec_get_interval( &sp->last, &sp->start );
//	//	VERBOSE_PRINTF( "interval = %d", interval);
//	speed_store_interval( sp, interval );
//	return interval;
//}
//
//void		speed_print_interval( ST_SPEED * sp, DOUBLE	interval )
//{
//	DOUBLE	frame_freq = (DOUBLE) 1000000. / interval;
//	DOUBLE	frame_freq_monitor = (DOUBLE) sp->monitor_freq / CEIL( interval * sp->monitor_freq / 1000000. );
//	DOUBLE	frame_freq_video = (DOUBLE) sp->video_freq / CEIL( interval * sp->video_freq / 1000000. );
//
//	PRINT_STRING("MilSec %.1f, Frame %.1f  ->%.1f->%.1f\n", interval/1000., frame_freq,  frame_freq_monitor, frame_freq_video );
//}
//
//void		speed_print_stat_flip( ST_SPEED * sp )
//{
//	sp->print_stat_on = !sp->print_stat_on;
//}
//
//bool		speed_print_stat_get( ST_SPEED * sp )
//{
//	return	sp->print_stat_on;
//}
//
//void		speed_print_stat( ST_SPEED * sp )
//{
//	DOUBLE	interval;
//
//	PRINT_STRING("Frame Rate\n");
//
//	interval = (DOUBLE)	sp->max;
//	PRINT_STRING("Slower  : ");
//	speed_print_interval( sp, interval);
//
//	interval = (DOUBLE)	sp->total / sp->hit;
//	PRINT_STRING("Average : ");
//	speed_print_interval( sp, interval);
//	
//	interval = (DOUBLE)	sp->min;
//	PRINT_STRING("Faster  : ");
//	speed_print_interval( sp, interval);
//}
//
//REAL		speed_get_fps_average( ST_SPEED * sp )
//{
//	DOUBLE	fps = (sp->hit * 1000000.) / sp->total;
////	PRINT_STRING( "%d fps = %f\n", sp->hit, fps );
//	return REAL( fps );
//}
//
//REAL		speed_get_fps_last( ST_SPEED * sp )
//{
//	return REAL( (DOUBLE) 1000000. / sp->interval );
//}
//
//
//REAL		speed_get_interval_last( ST_SPEED * sp )
//{
//	return REAL( DOUBLE( sp->interval ) / 1000000. ) ;
//}
//
//
//void		speed_print_last( ST_SPEED * sp )
//{
//	PRINT_STRING("Rendering : ");
//	speed_print_interval( sp, (DOUBLE)	sp->interval);
//}


FACTORY_CREATE_V1( c_speed_master, speed_master, Speed Master, speed_master );

//								TIMING_MASTER
namespace	n_speed_master
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 10;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	= BASE_PARAM_NB
									+ GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(	active			)
		PARAM_DEF_BOOL_OFF(	run				)
		PARAM_DEF_BOOL_OFF(	run_trig		)
		PARAM_DEF_BOOL_OFF(	print_trig		)
		PARAM_DEF_BOOL_OFF(	print_run		)
		PARAM_DEF_REAL_POS(	print_limit,	10., .0	)
		PARAM_DEF_BOOL_OFF(	print_module	)
		PARAM_DEF_BOOL_OFF(	print_group		)
		PARAM_DEF_BOOL_OFF(	print_layer		)
		PARAM_DEF_BOOL_OFF(	print_bdd		)
	};
}

CONSTRUCTOR_CREATE(c_speed_master)
	,_b_run		{false}
	,_b_print	{false}
{
	set_name( "Speed Master" );
	param_init_with( n_speed_master::param, n_speed_master::PARAM_NB_MAX );
}

c_speed_master::~c_speed_master()
{
}

void	c_speed_master::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, get_pt_active()	);
	param_set_pt( h, _b_run_ui			);
	param_set_pt( h, _b_run_trig_ui		);
	param_set_pt( h, _b_print_trig_ui	);
	param_set_pt( h, _b_print_run_ui	);
	param_set_pt( h, _print_limit_ui	);
	param_set_pt( h, _b_print_module_ui	);
	param_set_pt( h, _b_print_group_ui	);
	param_set_pt( h, _b_print_layer_ui	);
	param_set_pt( h, _b_print_bdd_ui	);

	err_param_init_pt( h );
}

void	c_speed_master::update()
{
	_b_run = false;
	_b_print = false;
	if( is_active() )
	{
		_b_print = _b_print_run_ui;
		if( _b_print_trig_ui )
		{
			_b_run_trig_ui = true;
			_b_print = true;
		}
		_b_print_trig_ui = false;
		_b_run = _b_run_ui;
		if( _b_run_trig_ui )
		{
			_b_run = true;
		}
		_b_run_trig_ui = false;
	}
}

c_speed_master*	g_speed_master = nullptr;


//c_timing::c_timing()
//	:_start(.0f)
//	,_last(.0f)
//	,_dt(.0f)
//	,_obj(nullptr)
//{
//}
//
//c_timing::c_timing( c_obj_ui* obj, UINT32 indent, C_PCHAR mess )
//	:_start(.0f)
//	,_last(.0f)
//	,_dt(.0f)
//{
//	set_indent(		indent	);
//	set_message(	mess	);
//	set_obj(		obj	);
//}
//
//c_timing::c_timing( C_PCHAR name, UINT32 indent, C_PCHAR mess )
//	:_start(.0f)
//	,_last(.0f)
//	,_dt(.0f)
//{
//	set_indent(		indent	);
//	set_message(	mess	);
//	set_name(		name	);
//}
//
//void	c_timing::init()
//{
//	_start	= 0;
//	_last	= 0;
//	_dt	= .0f;
//}
//
//void	c_timing::begin()
//{
//	if( g_timing_master.is_run() )
//	{
//		_start =  aaa::time::get_real_time();
//	}
//}
//
//void	c_timing::print()
//{
//	if( _dt >= g_timing_master.get_print_limit() )
//	{
//		o_str	buf_indent;
//		for( UINT32 i = 0; i < _indent; ++i )
//		{
//			buf_indent.add_char( '\t' );
//		}
//		if( !_name.is_empty() )
//		{
//			TIMING_PRINT_STRING( "%s%s %s : %.4f ms", buf_indent.get(), _name.get(), _message.get(), _dt );
//		}
//		else if( _obj )
//		{
//			c_namer*	namer = _obj->get_namer();
//			if( namer )
//			{
//				TIMING_PRINT_STRING( "%s%s\\%s %s : %.4f ms", buf_indent.get(), _obj->get_name_str(), namer->get_dir(), _message.get(), _dt );
//			}
//			else
//			{
//				TIMING_PRINT_STRING( "%s%s %s : %.4f ms", buf_indent.get(), _obj->get_name_str(), _message.get(), _dt );
//			}
//		}
//		else
//		{
//			TIMING_PRINT_STRING( "%s %s : %.4f ms", buf_indent.get(), _message.get(), _dt );
//		}
//	}
//}
//
//void	c_timing::end( bool b_print )
//{
//	if( g_timing_master.is_run() )
//	{
//		_last = aaa::time::get_real_time();
//		_dt = ( _last - _start ) * 1000.0f;
//		if( b_print )
//		{
//			print();
//		}
//	}
//}


c_speed::c_speed( bool CONST b_master, UINT32 CONST indent, bool* pb_print, C_PCHAR_C mess, c_obj_ui* CONST obj )
	:_interval(0)
	,_min(aaa::BIGGEST<UINT32>)
	,_max(0)
	,_total(0)
	,_hit(0)
	,_total_frame(0)
	,_monitor_freq(76)
	,_video_freq(50)
	,_fps_average(.0f)
	,_fps_last(.0f)
	,_b_print_stat_on(false)
	,_b_update_needed(false)
	,_b_run(false)
	,_b_timing_master(b_master)
	,_pb_print(pb_print)
	,_interval_last		{0.}
	,_interval_last_ms	{0.}
	,_interval_avg_ms	{0.}
{
	_start.QuadPart		= 0;
	_last.QuadPart		= 0;
	set_indent(		indent	);
	set_message(	mess	);
	set_obj(		obj		);
}

void	c_speed::set_frequency( UINT32 CONST monitor_f, UINT32 CONST video_f )
{
	_monitor_freq = monitor_f;
	_video_freq = video_f;
}

void	c_speed::restart()
{
	_hit = 0;
	_total = 0;
	_min = aaa::BIGGEST<UINT32>;
	_max = 0;
	set_frequency( 76, 50 );
}

void	c_speed::store_interval( UINT32 CONST interval )
{
	++_hit;
	_interval = interval;
	_total += _interval;
	if( _interval < _min )
		_min = _interval;
	if( _interval > _max )
		_max = _interval;
	if( _hit >= 100 )
	{
		if( _b_print_stat_on )
			print_stat();
		restart();
	}
}

FINLINE void	c_speed::update_run()
{
	_b_run = !_b_timing_master || g_speed_master->is_run();
}

void	c_speed::begin()
{
	update_run();
	if( _b_run )
		aaa::time::store( &_start );
}

void	c_speed::end()
{
	if( _b_run )
	{
		aaa::time::store( &_last );
		UINT32 interval = aaa::time::get_interval_micro_sec( _last, _start );
		//	VERBOSE_PRINTF( "interval = %d", interval);
		store_interval( interval );
		_b_update_needed = true;
		do_print();
	}
}

//unused
//void	c_speed::begin_end()
//{
//	update_run();
//	if( _b_run )
//	{
//		store_interval( 0 );
//		_b_update_needed = true;
//		do_print();
//	}
//}


void	c_speed::update()
{
	if( _b_update_needed )
	{
		_b_update_needed	= false;
		_fps_average		= REAL( DOUBLE( (_hit * 1000000.) / _total ) );
		if( _interval != 0 )
		{
			_fps_last	= REAL( (DOUBLE) 1000000. / _interval ); 
		}
		else
		{
			_fps_last	= .0f;
		}
		_interval_avg_ms	= DOUBLE( _total ) / ( _hit * 1000. );
		_interval_last		= DOUBLE( _interval ) / 1000000.;
		_interval_last_ms	= _interval_last * 1000.;
	}
}

void	c_speed::print_interval( C_PCHAR_C str_pre, DOUBLE CONST interval )
{
	DOUBLE	frame_freq			= (DOUBLE) 1000000. / interval;
	DOUBLE	frame_freq_monitor	= (DOUBLE) _monitor_freq / CEIL( interval * _monitor_freq / 1000000. );
	DOUBLE	frame_freq_video	= (DOUBLE) _video_freq / CEIL( interval * _video_freq / 1000000. );

	DBG_PRINT_STRING( "%s : MilSec %.1f, Frame %.1f  ->%.1f->%.1f", str_pre, interval/1000., frame_freq,  frame_freq_monitor, frame_freq_video );
}

void	c_speed::flip_print_stat()
{
	_b_print_stat_on = !_b_print_stat_on;
}

bool	c_speed::is_print_stat()
{
	return	_b_print_stat_on;
}

void	c_speed::print_stat()
{
	DBG_PRINT_STRING( "Frame Rate\n" );

	print_interval( "Slower ", (DOUBLE)	_max );
	print_interval( "Average", (DOUBLE)	_total / _hit );
	print_interval( "Faster ", (DOUBLE)	_min );
}

void	c_speed::print_last()
{
	update();
	print_interval( "Rendering", (DOUBLE)_interval );
}

namespace {
	CONST	INT32 indents_max = 31;
	C_PCHAR_C indents = "\t\t\t\t" "\t\t\t\t" "\t\t\t\t" "\t\t\t\t"
						"\t\t\t\t" "\t\t\t\t" "\t\t\t\t" "\t\t\t";	//	yes 31 here
	FINLINE	C_PCHAR_C get_indent( INT32 indent )	{	return indents + CLAMP( indents_max-indent, 0, indents_max );	}
}

void	c_speed::print()
{
	update();
	if( _interval_last_ms >= g_speed_master->get_print_limit() )
	{
		if( !_name.is_empty() )
			TIMING_PRINT_STRING( "%s%s %s : %.4f ms", get_indent(_indent), _name.get(), _message.get(), _interval_last_ms );
		else if( _obj )
		{
			c_namer*	namer = _obj->get_namer();
			if( namer )
				TIMING_PRINT_STRING( "%s%s\\%s %s : %.4f ms", get_indent(_indent), _obj->get_name_str(), namer->get_dir().get(), _message.get(), _interval_last_ms );
			else
				TIMING_PRINT_STRING( "%s%s %s : %.4f ms", get_indent(_indent), _obj->get_name_str(), _message.get(), _interval_last_ms );
		}
		else
			TIMING_PRINT_STRING( "%s %s : %.4f ms", get_indent(_indent), _message.get(), _interval_last_ms );
	}
}

FINLINE	void c_speed::do_print()
{
	if( _b_timing_master && g_speed_master->is_print() && (!_pb_print || *_pb_print) )
		print();
}

void	c_speed::build_comment( o_str& o, bool b_on, bool b_group )
{
	if( b_on )
	{
		if( g_speed_master->is_run() )
		{
			CHAR	buf[256];
			snprintf( buf, sizeof(buf)-1, "%.2f %.1f", get_interval_last_ms(), get_fps_average() );
			o.set( buf );
		}
		else if( b_group )
			o.set( "ON" );
		else
			o.erase();
	}
	else
		o.erase();
}

FACTORY_CREATE_V1( c_speed_ui, speed_ui, Speed, speed_ui );

//								TIMING_MASTER
namespace	n_speed_ui
{
	CONSTEXPR INT32 BASE_PARAM_NB	=	4;
	CONSTEXPR INT32 GROUP_PARAM_NB	=	0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_REAL_LOCKED(	interval		)
		PARAM_DEF_REAL_LOCKED(	interval_avg	)
		PARAM_DEF_REAL_LOCKED(	fps				)
		PARAM_DEF_REAL_LOCKED(	fps_avg			)
	};
}

CONSTRUCTOR_CREATE(c_speed_ui)
	,_fps_last_ui(.0f)
	,_fps_avg_ui(.0f)
	,_interval_ui(.0f)
	,_interval_avg_ui(.0f)
	,_timing(nullptr)
{
	set_name( "Speed" );
	param_init_with( n_speed_ui::param, n_speed_ui::PARAM_NB_MAX );
}

c_speed_ui::~c_speed_ui()
{
	_timing = nullptr;	//	c_speed_ui is never the owner
}

void	c_speed_ui::param_init_pt()
{
	INT32	h = 0;

	//param_set_pt( h, get_pt_active()	);
	param_set_pt( h, _interval_ui		);
	param_set_pt( h, _interval_avg_ui	);
	param_set_pt( h, _fps_last_ui		);
	param_set_pt( h, _fps_avg_ui		);

	err_param_init_pt( h );
}

void	c_speed_ui::update()
{
	if( _timing )
	{
		_interval_ui		= REAL(	_timing->get_interval_last_ms() );
		_fps_last_ui		= _timing->get_fps_last();
		_fps_avg_ui			= _timing->get_fps_average();
		_interval_avg_ui	= REAL( _timing->get_interval_avg_ms() );
	}
}

