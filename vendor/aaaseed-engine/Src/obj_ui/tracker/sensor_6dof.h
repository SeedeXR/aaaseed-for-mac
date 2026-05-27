
#ifdef AAA_SENSOR_6DOF_H
#error "SENSOR_6DOF_H included more than once."
#endif
#define AAA_SENSOR_6DOF_H 1


#ifndef	AAA_TRACKER_H
#	include "tracker.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif
#ifndef AAA_V_BASE_H
#	include "math/v_base.h"
#endif

class c_seedcam;

class	c_sensor_6dof : public c_obj_ui
{
	FACTORY_DECLARE( c_sensor_6dof, c_obj_ui );
public:
	static INT32 CONST CHANNEL_NB = 6;
	static INT32 CONST VALUE_NB = 8;
	static INT32 CONST BUTTON_NB = 8;
protected:
	c_delta_t	_delta_t;
	REAL		_sensibility_tra_ui;
	REAL		_sensibility_rot_ui;
	REAL		_sensibility_ui;
	c_sensor*	_sensor;	//hack


	c_seedcam*	_cam_target;

	REAL		_tra[3];
	REAL		_rot[3];

	REAL		_tra_factor[4];
	REAL		_tra_offset[3];
	REAL		_tra_speed[3];
	REAL		_tra_bias;
	REAL		_tra_filter;
	bool		_b_tra_offset_trig_ui;

	REAL		_rot_factor[4];
	REAL		_rot_offset[3];
	REAL		_rot_speed[3];
	REAL		_rot_bias;
	REAL		_rot_filter;

	bool		_b_camera_influence_ui;
	bool		_b_camera_influence_target_ui;
	bool		_b_camera_influence_tra_ui;
	bool		_b_camera_influence_rot_ui;

	FP32		_mat[16];

	REAL		_data_in		[CHANNEL_NB];
	bool		_b_pressed_out	[BUTTON_NB];
	bool		_b_touched_out	[BUTTON_NB];
	REAL		_value_out		[VALUE_NB];
	bool		_b_button_change_out;
	bool		_b_absolute;
	INT32		_s_rotation_order;

	INT32		_vibrate_time;
	bool		_b_vibrate_ui;


public:
	FINLINE	void		set_vibrate( bool CONST b )										{	_b_vibrate_ui = b;							}
	FINLINE	bool		is_vibrate()											CONST	{	return _b_vibrate_ui;						}

	FINLINE	void		set_vibrate_time( INT32 CONST value )							{	_vibrate_time = value;						}
	FINLINE	INT32		get_vibrate_time()										CONST	{	return _vibrate_time;						}

	FINLINE	void		set_data_in( INT32 CONST index, REAL CONST value )				{	_data_in[index] = value;					}
	FINLINE	REAL		get_data_in( INT32 CONST index)							CONST	{	return _data_in[index];						}

	FINLINE	void		set_value( INT32 CONST index, REAL CONST value )				{	_value_out[index] = value;					}
	FINLINE	REAL		get_value( INT32 CONST index)							CONST	{	return _value_out[index];					}

	FINLINE	void		set_button_change( bool CONST b )								{	_b_button_change_out = b;					}

	FINLINE	void		set_button( INT32 CONST index, bool CONST value )				{	_b_pressed_out[index] = value;				}
	FINLINE	bool		get_button( INT32 CONST index)							CONST	{	return _b_pressed_out[index];				}

	FINLINE	void		set_touch( INT32 CONST index, bool CONST value )				{	_b_touched_out[index] = value;				}
	FINLINE	bool		get_touch( INT32 CONST index)							CONST	{	return _b_touched_out[index];				}

	FINLINE	void		set_tra_in( REAL CONST* CONST tra )								{	cpy_v3( &_data_in[0], tra );				}
	FINLINE	void		set_tra_in( REAL CONST x, REAL CONST y, REAL CONST z )			{	set_v3( &_data_in[0], x,y,z );				}

	FINLINE	void		set_rot_in( REAL CONST* CONST rot )								{	cpy_v3( &_data_in[3], rot );				}
	FINLINE	void		set_rot_in( REAL CONST yaw, REAL CONST pitch, REAL CONST roll )	{	set_v3( &_data_in[3], yaw, pitch, roll );	}

			void		set_matrix_in( CONST float* m );

			void		init();
	virtual	void		param_init_pt();

			void		update_sensor_6dof();
//	virtual	void		calibrate_default() {};

			void		set_absolute( bool CONST b_in )									{	_b_absolute = b_in;		}
			void		set_rotation_order( INT32 CONST in )							{	_s_rotation_order = in;	}

	//hack potential trouble when target deleted but emergency FAF prod
	//virtual bool		set_target( INT32 slot, c_obj_ui* target );		//slot start at 1
	//virtual c_obj_ui*	get_target( INT32 slot ) CONST;					//slot start at 1

};
