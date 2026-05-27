
#ifdef AAA_HMD_H
#error "HMD_H included more than once."
#endif
#define AAA_HMD_H 1


#ifndef AAA_TRACKERS_H
#	include "obj_ui/tracker/trackers.h"
#endif

#if	AAA_TRACKER_HMD()

extern	void	hmd_tracker_get();

#	if	AAA_TRACKER_VIO()

extern	bool	b_vio_stereo;
extern	bool	b_vio_tracker;
extern	INT32	vio_averager_value;
extern	REAL	vio_averager_filter_factor;
extern	bool	b_vio_move_object;

extern	void	vio_tracker_set( bool state );

extern	void	vio_set_averager();
extern	void	vio_tracker_toggle();
extern	void	vio_tracker_set_origin();
extern	void	vio_rendering_toggle();
#	endif	//	TRACKER_VIO

#define	USE_AVERAGER	1
#ifndef AAA_AVERAGER_H
#include "infrastructure/averager.h"
#endif

enum HMD_TYPE : INT32
{
	HMD_VIO = 0,
	HMD_POLHEMUS = 1
};

enum DATA_MODE : INT32
{
	RAW = 0,
	COOKED = 1,
	EULER = 2,
	CYBERMAXX = 3
};

enum SEND_MODE : INT32
{
	POLLED = 'P',
	CONTINUOUS = 'C'
};

enum SEND_FORMAT : INT32
{
	ASCII = 'A',
	BINARY = 'B'
};

FINLINE float fixed2Float(short fixed)	{	return fixed / (float) (1 << 14);	}
FINLINE short float2Fixed(float fl)		{	return (short) (fl * (1 << 14));	}

class c_euler
{
#if	USE_AVERAGER
protected:
	c_averager	_yaw;
	c_averager	_pitch;
	c_averager	_roll;
	c_averager	_x;
	c_averager	_y;
	c_averager	_z;
	INT32		_s_draw_flag;
public:
	c_euler();
	FINLINE	REAL	yaw()					{	return _yaw.get_raw();		}
	FINLINE	REAL	pitch()					{	return _pitch.get_raw();	}
	FINLINE	REAL	roll()					{	return _roll.get_raw();		}

	FINLINE	REAL	x()						{	return _x.get_raw();		}
	FINLINE	REAL	y()						{	return _y.get_raw();		}
	FINLINE	REAL	z()						{	return _z.get_raw();		}

	FINLINE	REAL	yaw_filtered()			{	return _yaw.get_average();		}
	FINLINE	REAL	pitch_filtered()		{	return _pitch.get_average();	}
	FINLINE	REAL	roll_filtered()			{	return _roll.get_average();		}

	FINLINE	REAL	x_filtered()			{	return _x.get_average();		}
	FINLINE	REAL	y_filtered()			{	return _y.get_average();		}
	FINLINE	REAL	z_filtered()			{	return _z.get_average();		}

	FINLINE	void	yaw( REAL newyaw)		{	_yaw.put(	newyaw,		nullptr );	}
	FINLINE	void	pitch( REAL newpitch)	{	_pitch.put(	newpitch,	nullptr );	}
	FINLINE	void	roll( REAL newroll)		{	_roll.put(	newroll,	nullptr );	}

	FINLINE	void	x( REAL newyaw)			{	_x.put(		newyaw,		nullptr );	}
	FINLINE	void	y( REAL newpitch)		{	_y.put(		newpitch,	nullptr );	}
	FINLINE	void	z( REAL newroll)		{	_z.put(		newroll,	nullptr );	}

	FINLINE	void	set_average_size( INT32	new_size)
											{
												_yaw.set_average_size( new_size);
												_pitch.set_average_size( new_size);
												_roll.set_average_size( new_size);
												_x.set_average_size( new_size);
												_y.set_average_size( new_size);
												_z.set_average_size( new_size);
											}
	FINLINE	INT32	get_average_size()		{	return _yaw.get_average_size(); }
	FINLINE	void	set_filter_factor( REAL	factor_in)
											{
												_yaw.set_filter_factor( factor_in);
												_pitch.set_filter_factor( factor_in);
												_roll.set_filter_factor( factor_in);
												_x.set_filter_factor( factor_in);
												_pitch.set_filter_factor( factor_in);
												_roll.set_filter_factor( factor_in);
											}
			void	draw_toggle();
			void	draw();
//	friend ostream& operator<<(ostream &os, CONST c_euler &p);
#else
protected:
	REAL	_yaw, _pitch, _roll;
public:
	FINLINE	REAL	yaw()					{	return _yaw;	}
	FINLINE	REAL	pitch()					{	return _pitch;	}
	FINLINE	REAL	roll()					{	return _roll;	}
	FINLINE	void	yaw( short newyaw)		{	_yaw	= fixed2Float(newyaw);		}
	FINLINE	void	pitch( short newpitch)	{	_pitch	= fixed2Float(newpitch);	}
	FINLINE	void	roll( short newroll)	{	_roll	= fixed2Float(newroll);		}
	FINLINE	void	yaw( REAL newyaw)		{	_yaw	= newyaw;		}
	FINLINE	void	pitch( REAL newpitch)	{	_pitch	= newpitch;	}
	FINLINE	void	roll( REAL newroll)		{	_roll	= newroll;		}
#endif

			void	print_euler();
};

//ostream &operator<<(ostream &os, CONST c_euler &p);

extern c_euler	euler_cur;

extern	bool	b_hmd_active;
extern	REAL	euler_angle[3];
extern	void	hmd_move_cam();

#endif // AAA_TRACKER_HMD()