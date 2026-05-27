
#ifdef AAA_TRACKER_H
#error "TRACKER_H included more than once."
#endif
#define AAA_TRACKER_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_AVERAGER_H
#	include "infrastructure/averager.h"
#endif

class	c_tracker : public c_obj_active_ui
{
	FACTORY_ABSTRACT_DECLARE(c_tracker,c_obj_active_ui);
protected:
	bool		_b_open;
	bool		_b_open_ui;
	bool		_b_verbose_ui;
public:
//			c_tracker( INT32 channel_nb_in);

private:
//			AAA_ERR	init( INT32 channel_nb_in);
			AAA_ERR	init();
protected:
			void	make_name( o_str& dst, o_str CONST & src, INT32 index );
public:
//			void	read_from_file( C_PCHAR_C filename );
	virtual	void	idle();

	virtual AAA_ERR	start()		{ set_active( true ); return AAA_OK; }
	virtual void	stop()		{ set_active( false ); }

	virtual	AAA_ERR	open() = 0;
	virtual void	close() = 0;

			void	update();
	virtual void	update_low() = 0;

//	virtual	void	calibrate_default()=0;
			void	calibrate_start( INT32 channel_start, INT32 channel_stop );
			void	calibrate_stop(  INT32 channel_start, INT32 channel_stop );
			void	calibrate_start();
			void	calibrate_stop();

//			void	draw();
//			void	toggle_draw_mode();
			void	set_draw_mode( INT32 draw_mode_in);
};

class c_neat;
class c_sensor_6dof;

//abstract class
class	c_sensor : public c_obj
{
	friend c_neat;
	friend c_sensor_6dof;

private:
	INT32		draw_mode;
protected:
	INT32		channel_nb;
	c_averager	*averager;
	REAL		*data;	
	bool		b_verbose;

public:
			c_sensor(		INT32 channel_nb_in );
	virtual ~c_sensor();
private:
			AAA_ERR	init(	INT32 channel_nb_in );
			AAA_ERR	alloc(	INT32 channel_nb_in );
			void	dealloc();
public:
//			void	read_from_file( C_PCHAR_C filename );

	FINLINE	REAL	control_get( INT32 control_id )
						{	return data[control_id-1]; }
	FINLINE	void	control_set_filter_factor( INT32 control_id, REAL filter_in )
						{	averager[control_id-1].set_filter_factor(filter_in); }
	FINLINE	void	control_put( INT32 control_id, REAL value_in)
						{
						averager[control_id-1].put( value_in, nullptr );
						data[control_id-1] = averager[control_id-1].get_average();
						}

//	virtual	void	calibrate_default()=0;
			void	calibrate_start( INT32 channel_start, INT32 channel_stop );
			void	calibrate_stop(	 INT32 channel_start, INT32 channel_stop );
			void	calibrate_start();
			void	calibrate_stop();

			void	draw();
			void	toggle_draw_mode();
			void	set_draw_mode( INT32 draw_mode_in );
};

