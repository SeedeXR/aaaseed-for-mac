
#ifdef AAA_SPEED_H
#error "SPEED_H included more than once."
#endif
#define AAA_SPEED_H 1


#ifndef AAA_AAA_TIME_H
#	include "aaa_time.h"
#endif
#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
	
class	c_speed final : public c_obj
{
private:
	aaa::time::ST_TIME	_start;	//	times in the higher resolution possiblaaa::timee
	aaa::time::ST_TIME	_last;

	bool*				_pb_print;
	//	values in macro sec
	//	overload after 2000 sec
	UINT32				_interval;
	UINT32				_min;
	UINT32				_max;
	UINT32				_total;

	UINT32				_hit;
	UINT32				_total_frame;

	UINT32				_monitor_freq;
	UINT32				_video_freq;

	REAL				_fps_average;
	REAL				_fps_last;

	DOUBLE				_interval_last;
	DOUBLE				_interval_last_ms;
	DOUBLE				_interval_avg_ms;
	bool				_b_print_stat_on;
	bool				_b_timing_master;

	bool				_b_update_needed;
	bool				_b_run;

	o_str				_message;
	o_str				_name;
	UINT32				_indent;
	c_obj_ui*			_obj;

			void	store_interval( UINT32 interval );
			void	print_interval( C_PCHAR_C str_pre, DOUBLE interval );
	FINLINE	void	update_run();
	FINLINE	void	do_print();

public:
			c_speed( bool CONST b_master, UINT32 CONST indent = 0, bool* pb_print = nullptr, C_PCHAR_C mess = nullptr, c_obj_ui* CONST obj = nullptr );
			void	restart();

			void	begin();
			void	end();
//unused
//			void	begin_end();
	
			void	print();
			void	update();

	FINLINE	void	set_indent(		UINT32 CONST indent )	{	_indent = indent;		}
	FINLINE	void	set_message(	C_PCHAR_C mess )		{	_message.set( mess );	}
	FINLINE	void	set_name(		C_PCHAR_C  name )		{	_name.set( name );		}
	FINLINE	void	set_obj(		c_obj_ui* CONST	obj )	{	_obj = obj;				}
			void	set_frequency( UINT32 CONST monitor_f, UINT32 CONST video_f );

	FINLINE	REAL	get_fps_average()		{	update();	return _fps_average;		}
	FINLINE	REAL*	get_pt_fps_average()	{	update();	return &_fps_average;		}
	FINLINE	REAL	get_fps_last()			{	update();	return _fps_last;			}

	FINLINE	DOUBLE	get_interval_last()		{	update();	return _interval_last;		}
	FINLINE	DOUBLE	get_interval_last_ms()	{	update();	return _interval_last_ms;	}
	FINLINE	DOUBLE*	get_pt_interval_ms()	{	update();	return &_interval_last_ms;	}
	FINLINE	DOUBLE	get_interval_avg_ms()	{	update();	return _interval_avg_ms;	}

			void	flip_print_stat();
			bool	is_print_stat();
			void	print_stat();
			void	print_last();

			void	build_comment( o_str& o, bool CONST b_on, bool CONST b_group = true );
};

class	c_speed_ui final : public c_obj_ui
{
	FACTORY_DECLARE( c_speed_ui, c_obj_ui );

private:
	REAL		_fps_last_ui;
	REAL		_fps_avg_ui;
	REAL		_interval_ui;
	REAL		_interval_avg_ui;
	c_speed*	_timing;

public:
	virtual	void	param_init_pt();
	virtual void	update();
	FINLINE	void	set_timing( c_speed* CONST timing )		{	_timing = timing;	}
};

class	c_speed_master final : public c_obj_active_ui
{
	FACTORY_DECLARE( c_speed_master, c_obj_active_ui );

private:
	bool		_b_run_ui;
	bool		_b_run_trig_ui;
	bool		_b_print_trig_ui;
	bool		_b_print_run_ui;
	bool		_b_print_module_ui;
	bool		_b_print_group_ui;
	bool		_b_print_layer_ui;
	bool		_b_print_bdd_ui;
	REAL		_print_limit_ui;

	bool		_b_run;
	bool		_b_print;

public:
	FINLINE	bool	is_run()				CONST	{ return _b_run;				}
	FINLINE	bool	is_print()				CONST	{ return _b_print;				}
	FINLINE	bool*	get_pb_print_module()			{ return &_b_print_module_ui;	}
	FINLINE	bool*	get_pb_print_group()			{ return &_b_print_group_ui;	}
	FINLINE	bool*	get_pb_print_layer()			{ return &_b_print_layer_ui;	}
	FINLINE	bool*	get_pb_print_bdd()				{ return &_b_print_bdd_ui;		}
	FINLINE	REAL	get_print_limit()		CONST	{ return _print_limit_ui;		}

	virtual void	param_init_pt();
			void	update();
};

extern	c_speed_master*	g_speed_master;

