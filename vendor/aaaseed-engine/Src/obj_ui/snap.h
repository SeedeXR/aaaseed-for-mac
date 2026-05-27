
#ifdef AAA_SNAP_H
#error "SNAP_H included more than once."
#endif
#define AAA_SNAP_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class	c_snap final : public c_obj_active_ui
{
	FACTORY_DECLARE( c_snap, c_obj_active_ui );
public:
	static	c_snap* cur;	//todo singleton
protected:
	INT32	_type;

	bool	_b_active_ui;	//todo solve with obj_active use
	bool	_b_shoot;
	bool	_b_shoot_ui;
	bool	_b_run;
	bool	_b_run_ui;

	INT32	_pano_size_x;
	REAL	_line_pos;
	INT32	_file_start_nb;
	FP32	_start;
	REAL	_stop;
	REAL	_step;
	FP32	_current;
	INT32	_frame_nb;
	INT32	_preroll;
	bool	_b_load_frame_ui;
	bool	_b_record_trax;

/*	REAL	tra_start[3];
	REAL	tra_end[3];
	REAL	rot_start[3];
	REAL	rot_end[3];
*/
private:
			bool	active_set( bool CONST b_in );
			bool	set_shoot(	bool CONST b_in );
			bool	set_run(	bool CONST b_in );
			void	goto_start();
			void	goto_end();
public:

	virtual	void	param_init_pt();
	virtual	void	param_init();

			void	set_start( FP32 start_in = 0. )	{ _start = start_in; }
			void	set_stop ( REAL stop_in  = 0. )		{ _stop  = stop_in; }
			void	set_step ( REAL step_in  = 0. )		{ _step  = step_in; }
			void	set_preroll( INT32 preroll_in )		{ _preroll = preroll_in; }
			void	set_frame_nb( INT32 frame_in );

	virtual void	update();
			FP32	get_time()		{ return _current; }
			REAL	get_step()		{ return _step; }
			INT32	get_frame_nb()	{ return is_active() ? _frame_nb : -1; }

			void	start_it();
			void	stop_it();

			void	start_shoot();
			void	stop_shoot();
			void	flip_shoot();
			bool	is_shoot();
			void	do_shoot( INT32 which_buffer );
//			bool	active_is();

			void	run_next();
			void	run_prev();
			void	run_resume();
			void	run_start();
			void	run_stop();
			void	run_flip();

			void	record_flip();
};


