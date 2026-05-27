
#ifdef AAA_JOY_H
#error "JOY_H included more than once."
#endif
#define AAA_JOY_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef	AAA_FLUX_FILTER_H
#	include "infrastructure/flux_filter.h"
#endif

class	c_joy final : public c_obj_active_ui
{
	FACTORY_DECLARE( c_joy, c_obj_active_ui );

public:
	CONSTEXPR	static INT32	BUT_NB			= 32;
	CONSTEXPR	static INT32	DIRECTION_NB	= 6;

private:
	bool			_b_active_ui;	//todo solve with the obj_active one
	bool			_b_reconnect_ui;
	bool			_b_open_at_load_ui;
	bool			_b_open_at_load;

	INT32			_e_channel;

	//todo create a class or struct by direction and move it to cpp
	REAL			_in[DIRECTION_NB];
	REAL			_out[DIRECTION_NB];
	bool			_b_but[BUT_NB];
	bool			_b_but_trig_down[BUT_NB];
	bool			_b_but_trig_up[BUT_NB];
	INT32			_hex[DIRECTION_NB+2];

	bool			_dir_active[DIRECTION_NB];
	REAL			_neutral_center[DIRECTION_NB];
	REAL			_neutral_extreme[DIRECTION_NB];
	REAL			_gain[DIRECTION_NB];
	REAL			_bias[DIRECTION_NB];
	REAL			_filter[DIRECTION_NB];
	REAL			_factor[DIRECTION_NB];
	REAL			_offset[DIRECTION_NB];

	bool			_b_calibrate[DIRECTION_NB];
	bool			_b_calibrate_auto_min[DIRECTION_NB];
	bool			_b_calibrate_auto_max[DIRECTION_NB];
	INT32			_min[DIRECTION_NB];
	INT32			_max[DIRECTION_NB];

	c_flux_filter	_flux[DIRECTION_NB];

public:
			void	set_input( INT32 channel_id);
			void	init();
	virtual	void	param_init();
	virtual	void	param_init_pt();

			void	start();
			void	process_channel( INT32 i);
			void	calibrate_channel( INT32 i);

	virtual	void	update();
	virtual	AAA_ERR	load_do_after( o_str CONST & filename );
};
