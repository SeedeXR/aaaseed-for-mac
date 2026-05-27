
#ifdef AAA_BDD_FLEXUS_H
#error "BDD_FLEXUS_H included more than once."
#endif
#define AAA_BDD_FLEXUS_H 1


#ifndef	AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif

class	c_layers;
class	c_bdd_mocap;
class	c_bdd_tube_path;
class	c_def_node;

class	c_bdd_flexus final : public c_bdd
{
	FACTORY_DECLARE( c_bdd_flexus, c_bdd );
private:
	c_layers*			_layers_mocap;	//redo this
	c_layers*			_layers_flexus;	//redo this
	c_bdd_mocap*		_bdd_mocap;
	c_bdd_tube_path*	_bdd_tube_path;
	c_def_node*			_deformer_flexus;
	bool				_b_camera_deform;

	//In
	REAL		_in_lr;
	REAL		_in_forward_speed;
	REAL		_hmd_rot[3];
	//INT32	navigation_method;
	REAL		_s_size;
	REAL		_t_size;
	REAL		_rot_factor;
	REAL		_tra_factor;
	bool		_b_freeze_ship_position;

	//Out
	REAL		_cam_rot[3];
	REAL		_s;
	REAL		_coor_in_path[3];
	REAL		_cam_pos[3];
	REAL		_cam_pos_last[3];
	REAL		_cam_target[3];
	REAL		_angle_cam;
	REAL		_angle_ship_to_world;
	REAL		_angle_ship_to_tube;
	REAL		_angle_camera_to_tube;
	REAL		_phase_in_front;
	REAL		_pitch;
	REAL		_speed;

	c_delta_t	_delta_t;
	DOUBLE		_pitch_ship_to_world;
	bool		_b_nav_ship;
	INT32		_tube_channel;

	bool		_b_restart_trig_ui;

public:
			void	init();
	virtual	void	param_init_pt();
			REAL	get_dt();

			void	ship_update();

	virtual	void	update();
	virtual	void	draw();

	REAL*				get_coor_in_path()					{ return _coor_in_path; }
	REAL*				get_cam_rot()						{ return _cam_rot; }
	REAL*				get_cam_pos()						{ return _cam_pos; }
	REAL*				get_hmd_rot()						{ return _hmd_rot; }

	c_bdd_tube_path*	get_bdd_tube_path()					{ return _bdd_tube_path; }
	c_bdd_mocap*		get_bdd_mocap()						{ return _bdd_mocap; }
	c_def_node*			get_deformer_flexus()				{ return _deformer_flexus; }
	c_layers*			get_layers_flexus()					{ return _layers_flexus;	}

	REAL				get_t_size()				CONST		{ return _t_size; }
	REAL				get_s_size()				CONST		{ return _s_size; }
	INT32				get_tube_channel()			CONST		{ return _tube_channel; }
//	void				set_freeze_ship_position( bool b_in )	{	_b_freeze_ship_position = b_in; }
	DOUBLE				get_pitch_ship_to_world()	CONST		{ return _pitch_ship_to_world; }
};
extern	c_bdd_flexus*	bdd_flexus;


