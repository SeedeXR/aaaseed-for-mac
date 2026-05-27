
#ifdef AAA_BELGA_H
#error "BELGA_H included more than once."
#endif
#define AAA_BELGA_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_AVERAGER_H
#	include "infrastructure/averager.h"
#endif
class	c_seedcam;

#define	 BELGA_SEQ_NB_MAX	128
class	c_belga final : public c_obj_active_ui
{
	FACTORY_DECLARE(c_belga,c_obj_active_ui);
public:
	BOOL		b_monoscreen;
	REAL		ring_yaw_range;
	REAL		ring_angle;
	REAL		ring_rot;
	REAL		ring_alpha;
	REAL		ring_alpha_bias;
	REAL		ring_angle_delta;
	REAL		ring_sticker_rot;
	REAL		ring_sticker_rot_factor;
	INT32		ring_select;
	REAL		ring_rot_select;
	INT32		map_fond;
	INT32		map_sphere;
	INT32		ring_map;
	BOOL		b_sous_titre;
	INT32		map_sous_titre;

	REAL		time_last_joystick_move;
	REAL		tempo_out;

	BOOL		b_joy_loc;
	BOOL		b_joy_loc_ok;
	REAL		joy[3];
	BOOL		b_joy_but[5];
	INT32		joy_raw[4];
	REAL		joy_accel_threshold;
	REAL		joy_accel_step_more;
	REAL		joy_accel_step_less;
	REAL		joy_accel_max;
	REAL		joy_accel_value[3];
	REAL		joy_accel_feedback_start;
	BOOL		b_feedback_on;

	REAL			joy_neutral;
	REAL			joy_gain;
	REAL			joy_filter;
	REAL			joy_factor;
	REAL			joy_ring_vanish;
	c_averager		ave1;
	c_averager		ave2;
	c_averager*		joy_averager[2];

	REAL		vol_ui;
	REAL		vol;
	INT32		page_nb;

	BOOL		trig_joy_start;
	BOOL		trig_joy_stop;
	BOOL		trig_page;
	BOOL		trig_page_flash;
	BOOL		trig_halt;
	BOOL		trig_quit;
	BOOL		trig_switch_off;

	INT32		borne_cur;

	BOOL		button_old;
	BOOL		b_soustitre_on;
	INT32		soustitre_bind;
	REAL		soustitre_time_switch;
	REAL		soustitre_timeoff;

	BOOL		the_button[4];

	c_seedcam*	cam;

	BOOL	snd_nappe_start;
	BOOL	snd_nappe_stop;
	BOOL	snd_click_in;
	BOOL	snd_click_out;
	BOOL	snd_varispeed_start;
	BOOL	snd_varispeed_stop;
	REAL	snd_varispeed_volume;
	REAL	snd_varispeed_speed;

	INT32	player_cur;

private:

public:

			void	init();
			void	joy_process( INT32 which);

	virtual	void	param_init_pt();
	virtual	void	update();
			void	net_receive();

			void	players_ready(BOOL b_snd);
			void	player_play(INT32 i);
};

extern	c_belga*	belga;

void	belga_bind_ring( INT32 in);

