
#ifdef AAA_SYSTEM_EVENT_READER_H
#error "SYSTEM_EVENT_READER_H included more than once."
#endif
#define AAA_SYSTEM_EVENT_READER_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef AAA_NATIVE_SYSTEM_H
#	include "system/Native_system.h"
#endif
#ifndef AAA_NATIVE_TYPES_H
#	include "system/Native_types.h"
#endif
#ifndef _UTILITY_
#	include <utility>
#endif
#ifndef	AAA_AAA_OS_H
#	include "aaa_os.h"
#endif

class system_event_listener;
class c_system_context_menu;

/**
* @class system_event_reader
*
* @brief read and dispatch events from system message pump
* @note abstract base event reader class
*/
class system_event_reader final : public c_obj
{
	//friend class system_event_reader;

protected:
	bool		_b_size_move;	// win32 size-move state
//todo check why it is unused
//	bool		_b_resizing;	// win32 size-move state
	bool		_b_dirty;		// win32 size-move state

//	uint32_t	_sx;
//	uint32_t	_sy;

	bool		_b_mouse_initialized;
	bool		_b_mouse_inside;
//	bool		_b_mouse_hidden;
	bool		_b_mouse_down;
	
	// Mouse double click utilities
#if !AAASEED()
	INT32		m_prevClick;
	timeb		m_prevTime;
#endif

public:
	C_NO_CPY_MOVE( system_event_reader )
	/** system_event_reader class constructor. */
	system_event_reader( void );
	/** system_event_reader class destructor. */
	virtual ~system_event_reader( void );

public:
	/** \! Set active context menu. */
	static void set_context_menu( c_system_context_menu * p_pMenu );

public:
	/** System event message parser. */
	virtual std::pair<LRESULT,bool> handle_event( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam );

protected:
	virtual LRESULT handle_enter_size_move	( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam );
	virtual LRESULT handle_exit_size_move	( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam );
//	virtual LRESULT handleMoving			( SystemEventListener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam );
	virtual LRESULT handle_move				( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam );
//	virtual LRESULT handleSizing			( SystemEventListener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam );
	virtual LRESULT handle_size				( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam );
	virtual LRESULT handle_paint			( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam );
	virtual LRESULT handle_erase_background	( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam );
	virtual LRESULT handle_key				( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam );
	virtual LRESULT handle_char				( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam );
	virtual LRESULT handle_mouse_down		( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam );
	virtual LRESULT handle_mouse_up			( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam );
	virtual LRESULT handle_motion			( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam );
	virtual LRESULT handle_focus			( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam );
	virtual LRESULT handle_mouse_leave		( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam );
	virtual LRESULT handle_get_min_max_info	( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam );
	virtual LRESULT handle_drop				( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam );
	virtual LRESULT handle_close			( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam );
//	virtual LRESULT handle_idle				( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam );
	virtual LRESULT handle_menu				( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam );
	virtual LRESULT handle_display_change	( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam );
	virtual LRESULT handle_device_change	( system_event_listener & p_listener, HWND CONST hwnd, UINT CONST message, WPARAM CONST wparam, LPARAM CONST lparam );
};

