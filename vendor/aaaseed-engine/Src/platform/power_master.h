
#ifdef AAA_POWER_MASTER_H
#error "POWER_MASTER_H included more than once."
#endif
#define AAA_POWER_MASTER_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class	c_power_master final : public c_obj_ui
{
	FACTORY_DECLARE( c_power_master, c_obj_ui );
public:
private:
	bool	_b_can;
	bool	_b_plugged;
	bool	_b_charging;
	REAL	_battery_level;
	REAL	_time_left;
	REAL	_time_max;

	bool	_b_cpu_throttle_trig_ui;
	INT32	_cpu_throttle_ac_asked;
	INT32	_cpu_throttle_dc_asked;
	INT32	_cpu_throttle_ac;
	INT32	_cpu_throttle_dc;

	bool	_b_cpu_state_read_trig_ui;
	bool	_b_cpu_state_ac_write_trig_ui;
	bool	_b_cpu_state_dc_write_trig_ui;

	REAL	_cpu_state_ac_min_asked;
	REAL	_cpu_state_ac_max_asked;
	REAL	_cpu_state_dc_min_asked;
	REAL	_cpu_state_dc_max_asked;

	REAL	_cpu_state_ac_min;
	REAL	_cpu_state_ac_max;
	REAL	_cpu_state_dc_min;
	REAL	_cpu_state_dc_max;

	bool	_b_display_bright_trig_ui;
	REAL	_display_brightness_ac_asked;
	REAL	_display_brightness_dc_asked;
	REAL	_display_brightness_ac;
	REAL	_display_brightness_dc;

public:
	virtual	void	param_init_pt();
	virtual	void	update();

	void	c_init();
};

extern	c_power_master*	g_power_master;
