
#ifdef AAA_SYSTEMEVENTHANDLER_H
#error "SYSTEMEVENTHANDLER_H included more than once."
#endif
#define AAA_SYSTEMEVENTHANDLER_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef	AAA_AAA_OS_H
#	include "aaa_os.h"
#endif


class system_node;


/**
* @class system_event_handler
* 
* @brief dispatch the wndProc to target event readers
*/
class system_event_handler final : public c_obj
{
private:
	system_node*	_p_node;
	HWND			_hwnd;
	WNDPROC			_window_proc_prev;

private:
	C_NO_CPY_MOVE( system_event_handler )
	/**
	* @brief static wndProc, used to intercept and dispatch events
	* @note static function
	*/
	static LRESULT CALLBACK wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
	/**
	* @brief handler dedicated wndProc
	*/
	LRESULT CALLBACK _wndProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

public:
//	C_NO_CPY_MOVE( system_event_handler )
	/**
	* @brief system_event_handler class constructor
	*
	* @param p_pNode parent system node as system_node pointer
	*/
	system_event_handler( system_node* p_pNode );
	/**
	* @brief system_event_handler class destructor
	*/
	virtual ~system_event_handler( void );
};

