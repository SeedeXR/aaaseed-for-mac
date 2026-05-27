
#ifdef AAA_NIB_H
#error "NIB_H included more than once."
#endif
#define AAA_NIB_H 1


#define	APP_SPECIAL_NIB()	0

#if APP_SPECIAL_NIB()

#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_FLUX_FILTER_H
#	include "infrastructure/flux_filter.h"
#endif
#ifndef AAA_DELTA_T_H
#	include "time/delta_t.h"
#endif
#ifndef __BDD_PART_H__
	#include "obj_ui/bdd/bdd_point/bdd_part.h"
#endif

class	c_nib_talisman final : public c_obj 
{
protected:
	V3D		target;

public:
static	REAL	bottom_max;
static	REAL	target_radius;
static	REAL	target_dist_in;

//	REAL	mass;
	REAL	speed_max;
	REAL	accel_max;
	REAL	top;
	REAL	bottom;

	REAL	radius_int;
	REAL	radius_ext;

	V3D		pos;
	V3D		speed;

	c_nib_talisman();
	~c_nib_talisman()	{};
	
	void	make_target();
	void	move( REAL dt, c_nib_talisman* ta, c_nib_talisman* tb);
};

struct	c_nib_place final : public c_obj 
{
public:
	V3D		pos;
	REAL	radius;
};

#define	NIB_PLACE_NB	1

#define	PART_CLOSE_NB_MAX	3

class	c_nib final : public c_obj_active_ui
{
	FACTORY_DECLARE(c_nib,c_obj_active_ui);
public:
	BOOL	b_verbose;
private:
	BOOL	b_verbose_channel[10];

	c_delta_t	delta_t;

	INT32	central_host;
	REAL	turn_by_host;
	REAL	turn_offset;
	REAL	focal;
	BOOL	b_ship_active;
	REAL	ship_in_ui[5];
	REAL	ship_in[4];
	BOOL	b_ship_fast;

	REAL	ship_tra_factor;
	REAL	ship_tra_factor_fast_ui;

	REAL	ship_center_dist;
	REAL	ship_center_speed;
	REAL	ship_outside_dist;
	REAL	ship_outside_speed;

	REAL	ship_top;
	REAL	ship_y[4];
	REAL	ship_speed[4];

	REAL	ship_tra_hori_factor;

	REAL	ship_rot_factor;
	REAL	ship_rot_factor_normal_ui;
	REAL	ship_rot_factor_fast_ui;

	REAL	ship_roll_influence;
	REAL	ship_roll_filter;

	REAL	ship_max_up;
	REAL	ship_max_bottom;

	c_flux_filter	flux_ship_roll;
	c_flux_filter	flux_ship_yaw;
	c_flux_filter	flux_ship_pitch;

	BOOL	b_trig_reset_tra;
	BOOL	b_trig_reset_tra_bottom;
	BOOL	b_trig_reset_rot;

	V3D		cam_pos;
	V3D		cam_pos_last;
	V3D		cam_disp;
	V3D		cam_rot;
	V3D		cam_euler;
	REAL	cam_angle;
	REAL	cam_turb_value;

	REAL	mic_offset;

//	REAL	ring_dist_skin_max;
	c_nib_talisman	ring;
	c_nib_talisman	rutelin;
	c_nib_talisman	schwert;
	REAL	schwert_rot_threshold;
	V3D		schwert_rot;
	V3D		schwert_rot_last;
	BOOL	b_schwert_rot_state[3];

	REAL	dragon_dist_max;

	V3D		dragon_pos;
	V3D		dragon_speed;
	V3D		dragon_target;
	BOOL	b_dragon_active;
	REAL	dragon_chasing_radius;
	REAL	dragon_wandering_radius;
	REAL	dragon_target_radius_town;
	REAL	dragon_target_radius;
	REAL	dragon_accel_max;
	REAL	dragon_speed_max;
	REAL	dragon_accel_max_return;
	REAL	dragon_speed_max_return;
	INT32	dragon_activity;

	BOOL	b_sound;
	BOOL	b_on_14bits;
	INT32	rgba_bind;
	REAL	rgba_size_u;
	REAL	rgba_size_v;
	INT32	rgba_big_bind;
	REAL	rgba_big_size_u;
	REAL	rgba_big_size_v;
	REAL		storm_value;
	c_nib_place	storm;

	REAL	cos_y,sin_y;
	REAL	time;

	REAL	sound_forward_factor;
	REAL	sound_slide_factor;
	REAL	sound_roll_factor;
	V3D		mic[5];

	BOOL	b_udp;
	INT32	udp_count;
	CHAR	udp_buf[128];
	CHAR*	udp_pt;

	BOOL	b_grid;
	BOOL	b_borgnole;

	c_nib_place		place[NIB_PLACE_NB];
	c_particle*		part_to_send[PART_CLOSE_NB_MAX];

public:

			void	init();
	virtual	void	param_init_pt();

	REAL	get_turb_val( REAL* pos);

	virtual	void	update();
	void	update_dragon();
	void	update_sound();

	void	send_value( INT32 value );
	void	send_value_on_two( REAL val );
//	void	send_always_value_on_two( REAL val);
	void	send_value_on_three( REAL val );
	REAL	send_prox_azi( REAL* center, REAL* pos, REAL dist_max );


	void	send_rgba_low( REAL* pos, INT32 which);
	void	net_send();
	void	net_receive( CHAR* pt);
	BOOL	net_receive();

	void	udp_begin();
	void	udp_send( INT32 value);
	void	udp_end();
};

extern	c_nib *		nib;

/*
	REAL	spiral_start_[3];
	REAL	spiral_cyl_y_;
	REAL	spiral_cyl_r_;
	REAL	spiral_depth_;

  BOOL	b_speed_height_variable;
	REAL	speed_height_gain;
	REAL	speed_height_bias;
	REAL	speed_nif_factor;
	REAL	speed_town_factor;
	REAL	speed_town_height;

	c_flux_filter	jewel_filtered;
	REAL			jewel_filter_factor;
	REAL			jewel_rot_factor;
	REAL	jewel_dist;
	REAL	jewel_pano;
	REAL	Jewel_nuance[4];
*/

#endif	//APP_SPECIAL_NIB()

