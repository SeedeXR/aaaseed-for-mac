
#ifdef AAA_DOREMI_H
#error "DOREMI_H included more than once."
#endif
#define AAA_DOREMI_H 1


#ifndef	AAA_TRACKERS_H
#	include "trackers.h"
#endif

#if AAA_TRACKER_DOREMI()

#	ifndef	AAA_OBJ_UI_H
#		include "infrastructure/obj/obj_ui.h"
#	endif

class	c_serial;

class	c_doremi final : public c_obj_active_ui
{
	FACTORY_DECLARE( c_doremi, c_obj_active_ui );
protected:
	bool			_b_open;
	bool			b_verbose_;

	c_serial*		serial_;
	INT32			com_port_nb_;

	bool			_b_trig_start_ui;	
	bool			_b_trig_stop_ui;	
	bool			_b_trig_seq_ui[20];	

	bool			_b_to_go_trig_ui;
	INT32			to_go_seq_;

public:

			void	set_port( INT32 port_id );
			void	init();
	virtual	void	param_init_pt();

			void	open();
			void	close();

	virtual	void	update();
			void	update_low();

			void	send_goto( INT32 seq_index ) ;
};

extern	c_doremi*		doremi;

#endif //#if AAA_TRACKER_DOREMI()

