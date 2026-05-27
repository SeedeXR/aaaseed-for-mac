
#ifdef AAA_ANALOG_WAY_H
#error "ANALOG_WAY_H included more than once."
#endif
#define AAA_ANALOG_WAY_H 1


#ifndef AAA_TRACKERS_H
#	include "obj_ui/tracker/trackers.h"
#endif

#if AAA_TRACKER_ANALOG_WAY()
#	ifndef	AAA_OBJ_UI_H
#		include "infrastructure/obj/obj_ui.h"
#	endif

class	c_serial;

class	c_analog_way final : public c_obj_active_ui
{
	FACTORY_DECLARE(c_analog_way,c_obj_active_ui);
protected:
	bool			_b_open;
	bool			b_verbose;

	c_serial*		serial;
	INT32			com_port_nb;

	bool			_b_trig_switch;	
	
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

extern	c_analog_way*		analog_way;

#endif //#if AAA_TRACKER_ANALOG_WAY()
