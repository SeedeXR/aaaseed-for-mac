
#ifdef AAA_DMX_H
#error "DMX_H included more than once."
#endif
#define AAA_DMX_H 1


#ifndef	AAA_TRACKERS_H
#	include "obj_ui/tracker/trackers.h"
#endif

#if AAA_TRACKER_DMX()

#	ifndef AAA_OBJ_UI_H
#		include "infrastructure/obj/obj_ui.h"
#	endif

class c_serial;
class	c_dmx final : public c_obj_active_ui
{
	FACTORY_DECLARE( c_dmx, c_obj_active_ui );
public:
//	static	bool	b_verbose;	todo
	static	INT32 CONST	DST_MIDI_SLOT_SIZE = 32;
protected:
	bool	_b_open_ui;
	bool	_b_open;
	bool	_b_receive_ui;
	bool	_b_send_ui;
	bool	_b_trig_loop_ui;
	bool	_b_trig_sync_ext_ui;

	INT32	s_com_id_;

	bool		b_verbose_receive_low_;
	bool		b_verbose_receive_;
	bool		b_verbose_send_;

	bool		_b_send_always_ui;

	INT32		index_out_start_;
	INT32		index_out_stop_;
	REAL		out_[512];
	REAL		out_ui_[512];

	INT32		dst_midi_channel_[ 512/DST_MIDI_SLOT_SIZE ];
	INT32		dst_midi_control_[ 512/DST_MIDI_SLOT_SIZE ];

	UINT8*		cinetix_buf_;
	UINT8*		cinetix_pt_;
	UINT8*		cinetix_end_;
protected:
	INT32		channel_midi_dst_[4];
	c_serial*	serial_;

			void	dealloc();
			void	cinetix_init();
			void	cinetix_send( INT32 slot, UINT8 value );
			void	cinetix_flush();
	virtual	void	update_send();
	virtual	void	update_receive();
public:
			void	init();

	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	AAA_ERR	open();
	virtual	void	close();

			void	send( INT32 slot, REAL value );
};

extern	c_dmx*		g_dmx_cur;

#endif	//#if AAA_TRACKER_DMX()
