
#ifdef AAA_HID_H
#error "HID_H included more than once."
#endif
#define AAA_HID_H 1


#ifndef AAA_TRACKERS_H
#	include "obj_ui/tracker/trackers.h"
#endif

#if AAA_TRACKER_HID()
#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

class	c_hid final : public c_obj_active_ui
{
	FACTORY_DECLARE( c_hid, c_obj_active_ui );
public:
	static	bool	b_dll_loaded;
	static	void	c_init();
	static	void	c_deinit();
private:
	o_str		_serial_to_open;
	bool		_b_open;
	//INT32		_device_id;
	//INT32		_pin[ 8 ];
	//bool		_b_val_out[ 8 ];
	//bool		_b_val_in[ 8 ];
	//INT32		_state_last;
	//UINT8		_mask_last;

			UINT8	create_bitmask();
public:
	static	CONSTEXPR INT32	OBJ_MAX_NB = 26;	// a to z
	static	c_hid*			array[ OBJ_MAX_NB ];

	virtual	void	param_init_pt();
	virtual	void	update();
			void	open();
			void	close();
			void	enumerate();
};

#endif