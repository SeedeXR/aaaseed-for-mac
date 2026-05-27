
#ifdef AAA_TOUCH_WINDOWS_H
#error "TOUCH_WINDOWS_H included more than once."
#endif
#define AAA_TOUCH_WINDOWS_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef __wtypes_h__
#	include "WTypes.h"
#endif
#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_BLOB_H
#	include "obj_ui/bdd/bdd_multitouch/blob.h"
#endif


typedef BOOL (WINAPI *pSDARP)(ORIENTATION_PREFERENCE orientation);
//typedef BOOL (WINAPI* pSETAUTOROTATION)(BOOL bEnable);

namespace aaa
{
	typedef enum ORIENTATION
	{
		ORIENTATION_NONE = 0,
		ORIENTATION_LANDSCAPE,
		ORIENTATION_PORTRAIT,
		ORIENTATION_LANDSCAPE_FLIPPED,
		ORIENTATION_PORTRAIT_FLIPPED,
		ORIENTATION_MAX_NB,
	} ORIENTATION;
};

class	c_multitouch_master final : public c_obj_ui
{
	FACTORY_DECLARE( c_multitouch_master, c_obj_ui );
public:
private:
	pSDARP	pfnSetDisplayAutoRotationPreferences;
//	pSETAUTOROTATION	pfnSetAutoRotation;
	UINT32	_orientation_ui;
	UINT32	_orientation;

//	bool	_b_autorotate;

	bool	_b_transfered;

	bool	_b_multitouch_possible;
	bool	_b_multitouch_integrated;
	bool	_b_multitouch_external;
	bool	_b_pen_integrated;
	bool	_b_pen_external;

	bool	_b_verbose_touch;
	bool	_b_verbose_gesture;

	INT32	_blob_nb;
//	bool	_b_blank_frame_needed;

	HWND	_hd_window;
	bool	_b_ui_win_disabled;
	bool	_b_ui_win_disabled_ui;
	bool	_b_disable_win_o;
	bool	_b_disable_win_o_ui;

	bool	_b_non_coalesced;
	bool	_b_palm_detection;

	void	hook_keyboard_clear();
	void	hook_keyboard_set();
public:
	static	void			static_attach_to_windows( HWND hwnd, ULONG flags = -42 );
	virtual	void			param_init_pt();
	virtual	void			update();

			void			init();
			void			attach_to_windows( HWND hwnd );
			BOOL			receive_touch_event_win( HWND handle, UINT message, WPARAM wParam, LPARAM lParam );
		
			void			transfer_blobs_to( BLOBS_CONT& blobs );
			c_blob CONST &	get_blob_index(INT32 i);
			INT32			get_blob_nb()		{	return _blob_nb;	}
			bool			is_content_changed();
			void			mark_read();
		
			bool			is_possible()		{	return _b_multitouch_possible;	}
		
			void			lock();
			void			unlock();
};

extern	c_multitouch_master*	g_multitouch_master;
