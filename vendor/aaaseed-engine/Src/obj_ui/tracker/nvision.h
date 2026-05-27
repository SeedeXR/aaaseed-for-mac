
#ifdef AAA_NVISION_H
#error "NVISION_H included more than once."
#endif
#define AAA_NVISION_H 1


#ifndef AAA_TRACKERS_H
#	include "obj_ui/tracker/trackers.h"
#endif

#if AAA_TRACKER_NVISION()
#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class	c_serial;

class	c_nvision final : public c_obj_active_ui
{
	FACTORY_DECLARE( c_nvision, c_obj_active_ui );
public:
	CONSTEXPR static INT32 BUT_NB = 2;

protected:
	bool			_b_open;
	bool			_b_verbose;
	bool			_b_verbose_low;

	REAL			_but_time_push				[BUT_NB];
	bool			_b_but						[BUT_NB];
	bool			_b_but_short				[BUT_NB];
	bool			_b_but_short_trig_push		[BUT_NB];
	bool			_b_but_short_trig_release	[BUT_NB];
	bool			_b_but_long					[BUT_NB];
	bool			_b_but_long_trig_push		[BUT_NB];
	bool			_b_but_long_trig_release	[BUT_NB];

	REAL			_short_sum;	
	REAL			_short_trig_push_sum;	
	REAL			_short_trig_release_sum;	
	REAL			_long_sum;	
	REAL			_long_trig_push_sum;	
	REAL			_long_trig_release_sum;	
	REAL			_time_long;
	c_serial*		_serial;

	INT32			_com_port_nb;
	
public:
			void	set_port( INT32 port_id );
			void	init();
	virtual	void	param_init_pt();

			void	open();
			void	close();
//	void	process_channel( INT32 i);

	virtual	void	update();
			void	update_low();
};

extern	c_nvision*		g_nvision_cur;

#endif	//#if AAA_TRACKER_NVISION()

