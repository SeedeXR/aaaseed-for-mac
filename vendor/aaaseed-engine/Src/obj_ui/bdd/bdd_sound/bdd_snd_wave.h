
#ifdef AAA_SND_WAVE_H
#error "SND_WAVE_H included more than once."
#endif
#define AAA_SND_WAVE_H 1


#ifndef AAA_BDD_H
#	include "obj_ui/bdd/util/bdd.h"
#endif

class	c_sound_player;

class	c_bdd_snd_wave final : public c_bdd 
{
	FACTORY_DECLARE(c_bdd_snd_wave,c_bdd );
private:
	INT32				_device_index;
	INT32				_status;
	bool				_b_play;
	bool				_b_playing;
	bool				_b_trig_start_ui;
	bool				_b_trig_restart_ui;
	bool				_b_trig_stop_ui;
	bool				_b_loop_ui;
//	INT32				bind_snd;
	REAL				_freq;
	UINT32				_position;
//	UINT32				byte_per_sec_;
	REAL				_time;
	REAL				_jump_to;
	REAL				_volume_ui;
//	REAL				volume_;
	REAL				_pan_ui;
//	REAL				pan_;
	o_str				_wave_filename;
	o_str				_wave_filename_ui;
	REAL				_duration;
//	REAL				frequency_;
	INT32				_samplerate;
	INT32				_bit_nb;
	INT32				_channel_nb;

	c_sound_player*		_sound_player;
	bool				_b_sound_player_enabled;

public:

	void	init();
	void	alloc(); 
	void	dealloc();

			void	set_wave_filename(	o_str CONST & filename );
	virtual	AAA_ERR	load_do_after(		o_str CONST & filename );

	virtual	void	draw();
	virtual	void	update();

	virtual	void	param_init_pt();
	virtual	void	param_init();

	FINLINE	bool	is_playing()	{ return _b_playing; };
	FINLINE	void	start()			{ _b_trig_start_ui = true; };
	FINLINE	void	stop()			{ _b_trig_stop_ui = true; };
};

