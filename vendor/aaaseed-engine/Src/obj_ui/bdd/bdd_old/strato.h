
#ifdef AAA_STRATO_H
#error "STRATO_H included more than once."
#endif
#define AAA_STRATO_H 1


#define	APP_SPECIAL_STRATO()	0

#if APP_SPECIAL_STRATO()

#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class	c_bdd_snd_wave;
class	c_bdd_boxes;

class	c_strato final : public c_obj_active_ui
{
	FACTORY_DECLARE(c_strato,c_obj_active_ui);
private:
	BOOL				b_auto;
	BOOL				b_army;
	REAL				auto_delay;
	REAL				auto_delay_left;
	REAL				auto_start_time;
	REAL				auto_angle_tolerance;
	REAL				auto_angle[3];

	BOOL				b_trig_reset;

	REAL				image_in;
	INT32				s_image_out;
	INT32				s_image_last;
	REAL				angle_in[3];
	REAL				angle_out[3];
	REAL				angle_last[3];

	BOOL				b_first;
	BOOL				b_trig_play;
	INT32				snd_index_to_play;
	INT32				snd_index_playing;
	INT32				snd_index_playing_last;

	REAL				background_volume;
	REAL				silent_len;
	REAL				time_ready;
	c_bdd_snd_wave**	hd_bdd_snd;
	c_bdd_boxes*		p_bdd_boxes;

	REAL				focal_min;
	REAL				focal_max;

private:
			void	deinit();
public:

			void	init();
			void	reset();

	virtual	void	param_init_pt();
	virtual	void	update();
};

extern	c_strato*	strato;

#endif

