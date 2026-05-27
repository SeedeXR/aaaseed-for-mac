
#ifdef AAA_TOASTER_H
#error "TOASTER_H included more than once."
#endif
#define AAA_TOASTER_H 1


#ifndef	AAA_TRACKERS_H
#	include "trackers.h"
#endif

#if AAA_TRACKER_TOASTER()

#	ifndef	AAA_FLUX_FILTER_H
#		include "infrastructure/flux_filter.h"
#	endif
#	ifndef	AAA_TRACKER_H
#		include "tracker.h"
#	endif

class	c_toaster final : public c_tracker
{
	FACTORY_DECLARE(c_toaster,c_tracker);
public:
	static	INT32 CONST	CHANNEL_NB = 16;
protected:
//	c_delta_t	delta_t;
	INT32			_start;
	INT32			_stop;

	bool			_b_enable[CHANNEL_NB];
	bool			_b_calibrate_ui[CHANNEL_NB];
	bool			_b_calibrate[CHANNEL_NB];
	REAL			_min[CHANNEL_NB];
	REAL			_max[CHANNEL_NB];

	REAL			_zero[CHANNEL_NB];
	REAL			_one[CHANNEL_NB];
	bool			_b_invert[CHANNEL_NB];

	REAL			_bias[CHANNEL_NB];
	REAL			_gain[CHANNEL_NB];
	REAL			_filter[CHANNEL_NB];

	REAL			_in[CHANNEL_NB];
	REAL			_out[CHANNEL_NB];

	c_flux_filter	_flux[CHANNEL_NB];

	c_sensor*		_p_sensor;

public:

	FINLINE	void	set_data_in( INT32 index, REAL value )	{	_in[index] = value; }
	FINLINE	REAL	get_data_in( INT32 index )				{	return _in[index]; }
	FINLINE	REAL	get_data_out( INT32 index )				{	return _out[index]; }

			void	init();
	virtual	void	param_init_pt();

	virtual	void	update_low();
	virtual	AAA_ERR	open();
	virtual void	close();

//	virtual	void	calibrate_default() {};

			void	process_control( INT32 i );
};

extern	c_toaster*		g_toaster_cur;

#endif	//#if AAA_TRACKER_TOASTER()
