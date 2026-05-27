
#include "trackers.h"

#if	AAA_TRACKER_HMD()
#include "tracker.h"
#ifndef AAA_HMD_H
#	include "obj_ui/tracker/hmd.h"
#endif
#include "draw/seeddraw.h"
#include "obj_ui/tracker/iglasses.h"
#include "err.h"
#include "draw/stereo.h"
#include "gol/gol.h"


bool	b_hmd_active = false;

REAL	euler_angle[3]; // yaw, pitch, roll
REAL	euler_angle_origin[3];

REAL	hmd_tra_origin[3];
REAL	hmd_tra[3];

//todo should be local
extern	bool	dummy_bool[];
extern	INT32	dummy_int32[];
extern	REAL	dummy_float[];

void hmd_tracker_get()
{
	if( b_hmd_active )
	{
		if( dummy_bool[0] )
			euler_angle[0] = dummy_float[0];
		else
			euler_angle[0] = euler_cur.yaw_filtered();

		if( dummy_bool[1] )
			euler_angle[1] = dummy_float[1];
		else
			euler_angle[1] = euler_cur.pitch_filtered();

		euler_angle[1] -= euler_angle_origin[1];

		if( dummy_bool[2] )
			euler_angle[2] = dummy_float[2];
		else
			euler_angle[2] = euler_cur.roll_filtered() - euler_angle_origin[2];

		hmd_tra[0] = (euler_cur.x_filtered() - hmd_tra_origin[0])*.01;
		hmd_tra[1] = (euler_cur.y_filtered() - hmd_tra_origin[1])*.01;
		hmd_tra[2] = (euler_cur.z_filtered() - hmd_tra_origin[2])*.01;
	}
}

#	if	AAA_TRACKER_VIO()
bool	b_vio_stereo = false;
bool	b_vio_tracker = false;
INT32	vio_averager_value = 1;
REAL	vio_averager_filter_factor = .9;
bool	b_vio_move_object = false;

void vio_set_averager()
{
	euler_cur.set_average_size(vio_averager_value);
	euler_cur.set_filter_factor(vio_averager_filter_factor);
}


void vio_tracker_set_origin()
{
	if( b_vio_tracker)
	{
		polhemus_boresight();
		//todo move it to the tracker
//		euler_yaw_origin = euler_cur.yaw_filtered();
//		euler_pitch_origin = euler_cur.pitch_filtered();
//		euler_roll_origin = euler_cur.roll_filtered();

		hmd_tra_origin[0] = euler_cur.x_filtered();
		hmd_tra_origin[1] = euler_cur.y_filtered();
		hmd_tra_origin[2] = euler_cur.z_filtered();
	}
}




void	vio_tracker_set( bool state )
{
	if( b_vio_tracker != state )
	{
		if( state )
		{
			b_hmd_active = b_vio_tracker = NOERR( iglasses_start(HMD_POLHEMUS) );
			if( b_vio_tracker)
				vio_tracker_set_origin();
			else
				BOX_ERR( "I-Glasses Tracker : Couldn't initialize" );
		}
		else
		{
			iglasses_stop();
			b_hmd_active = b_vio_tracker = false;
		}
		if( b_vio_tracker == state )
			SWITCH_PRINT_STATE( "I-Glasses Tracker", b_vio_tracker);
	}
}

void vio_tracker_toggle()
{
	vio_tracker_set( !b_vio_tracker);
}


void vio_rendering_toggle()
{
	b_vio_stereo = !b_vio_stereo;
	SWITCH_PRINT_STATE( "I-Glasses Stereo", b_vio_stereo);
	g_stereo->set_active( b_vio_stereo );
	reset_erased_screens();
	draw_stencil();
}
#	endif	//AAA_TRACKER_VIO

void	hmd_move_object()
{
	if( b_hmd_active && b_vio_move_object )
	{
		GOL::translatev( hmd_tra);
		GOL::rotatev( euler_angle, GOL::GOL_ORDER_YXZ | GOL::GOL_ORDER_X_NEG | GOL::GOL_ORDER_Y_NEG | GOL::GOL_ORDER_Z_NEG );
	}
}

void	hmd_move_cam()
{
	if( b_hmd_active && !b_vio_move_object )
	{
		GOL::rotatev( euler_angle, GOL::GOL_ORDER_ZXY );
		GOL::translate_negv( hmd_tra );
	}
}

#include "draw/render.h"

#define	EULER_BUF_SIZE	256
#define	TRA_BUF_SIZE	32
c_euler::c_euler()
{
	_s_draw_flag = 0;
	_yaw.set_color( 1, 0, 0);
	_yaw.set_size( EULER_BUF_SIZE);
	_pitch.set_color( 0, 1, 0);
	_pitch.set_size( EULER_BUF_SIZE);
	_roll.set_color( 0, 0, 1);
	_roll.set_size( EULER_BUF_SIZE);
	_x.set_color( 1, 0, 0);
	_x.set_size( TRA_BUF_SIZE);
	_y.set_color( 0, 1, 0);
	_y.set_size( TRA_BUF_SIZE);
	_z.set_color( 0, 0, 1);
	_z.set_size( TRA_BUF_SIZE);
}

void	c_euler::draw()
{
	if( _s_draw_flag )
	{
		GOL::set_ortho_with_secu_margin( 100, 100 );	
		if( _s_draw_flag & 1 )
		{
			_yaw.draw_curve_in_rect(0,50,100,100,1/180.);
			_pitch.draw_curve_in_rect(0,50,100,100,1/180.);
			_roll.draw_curve_in_rect(0,50,100,100,1/180.);
		}
		if( _s_draw_flag & 2 )
		{
			_x.draw_curve_in_rect(0,50,100,100,1/100.);
			_y.draw_curve_in_rect(0,50,100,100,1/100.);
			_z.draw_curve_in_rect(0,50,100,100,1/100.);
		}
	}
}

void	c_euler::draw_toggle()
{
	_s_draw_flag = (_s_draw_flag -1) & 0x3;
}

void	c_euler::print_euler()
{
	VERBOSE_PRINT_STRING( "Polhemus Euler : x %f, y %f, z %f", x(), y(), z() );
	VERBOSE_PRINT_STRING( "I-Glasses Euler : yaw %f, pitch %f, roll %f", yaw(), pitch(), roll() );
}

#endif	//AAA_TRACKER_HMD
