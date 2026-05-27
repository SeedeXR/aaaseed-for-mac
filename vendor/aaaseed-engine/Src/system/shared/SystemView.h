
#ifdef AAA_SYSTEM_VIEW_H
#error "SYSTEM_VIEW_H included more than once."
#endif
#define AAA_SYSTEM_VIEW_H 1


#ifndef AAA_NATIVE_THREAD_H
#	include "thread/win32/Native_Thread.h"
#endif
#ifndef AAA_SYSTEMEVENT_H
#	include "System/shared/SystemEvent.h"
#endif
#ifndef _VECTOR_
#	include <vector>
#endif

namespace core
{
	class renderer;
}

class system_node;
class system_event_listener;


/**
* @class system_view
*
* Threaded view, manages event dispatch via dedicated callbacks 
* and system dependent window / OpenGL context / event reader 
*/
class system_view
	: public NativeT::Thread
{
protected:
	INT32							_x;							//<! View position on X-axis (in screen coordinates)
	INT32							_y;							//<! View position on Y-axis (in screen coordinates)
	INT32							_sx;						//<! View width (in screen coordinates)
	INT32							_sy;						//<! View height (in screen coordinates)

	system_node *					_p_system_node;				//<! System node pointer containing event reader / window / OpenGL context.
	system_node *					_p_system_node_master;		//<! Convenience shared master system node pointer.
	
	system_event_listener *			_p_system_event_listener;	//<! Convenience shared event listener pointer.	
	evt::EVENT						_event_current;				//<! Current system event (event to dispatch).

	std::vector<core::renderer*>	_renderers;					//<! Display renderer vector.		

	static INT32 CONST X_DEF	= 50;
	static INT32 CONST Y_DEF	= 50;
	static INT32 CONST SX_DEF	= 800;
	static INT32 CONST SY_DEF	= 600;
protected:
	C_NO_CPY_MOVE( system_view )
	/** system_view class constructor. */
	system_view(							INT32 CONST x = X_DEF,	INT32 CONST y = Y_DEF,	INT32 CONST sx = SX_DEF,	INT32 CONST sy = SY_DEF,	system_node * p_system_node = nullptr	);
	/** system_view class destructor */
	virtual ~system_view( void );

public:
	/** \! Create, init and return new system_view pointer, do not start thread. 
	* Example :
	*	system_view * ptr = system_view::create_ptr();
	*	ptr->Start();
	*/
	static system_view * create_ptr(		INT32 CONST x = X_DEF,	INT32 CONST y = Y_DEF,	INT32 CONST sx = SX_DEF,	INT32 CONST sy = SY_DEF,	system_node * p_system_node = nullptr	);
	/** \! Create, init and return new system_view pointer once its thread is started. 
	* Example :
	*	system_view * ptr = system_view::create_ptr_wait();
	*/
	static system_view * create_ptr_wait(	INT32 CONST x = X_DEF,	INT32 CONST y = Y_DEF,	INT32 CONST sx = SX_DEF,	INT32 CONST sy = SY_DEF,	system_node * p_system_node = nullptr	);

	/** \! Release and delete pointer class members once its thread is stopped. 
	* Example :
	*	system_view::release_ptr_wait( ptr );
	*	ptr = nullptr;
	*/
	static void release_ptr_wait( system_view* CONST p );

	
protected:
	/** Thread initialization function, init members inside thread run. */
	virtual void init( void );	
	/** Thread release function, release members inside thread run before exit. */
	virtual void uninit( void );
	/** Thread main run. */
	virtual void run( void );

protected:
	/** Process system incoming user events. */
	void process_event( void ) CONST;

protected:
	/** \! Keyboard down event callback. */
	virtual void callback_keyboard_down(		INT32 CONST key_code, INT32 CONST mod, bool CONST b_special ) CONST;
	/** \! Keyboard up event callback. */
	virtual void callback_keyboard_up(			INT32 CONST key_code, INT32 CONST mod, bool CONST b_special ) CONST;

	/** \! Text input event callback. */
	virtual void callback_text_input(			INT32 CONST key, INT32 CONST mod ) CONST;

	/** \! Mouse enter event callback. */
	virtual void callback_mouse_enter(			void ) CONST;
	/** \! Mouse leave event callback. */
	virtual void callback_mouse_leave(			void ) CONST;

	/** \! Mouse down event callback. */
	virtual void callback_mouse_down(			INT32 CONST button, INT32 CONST x, INT32 CONST y ) CONST;
	/** \! Mouse up event callback. */
	virtual void callback_mouse_up(				INT32 CONST button, INT32 CONST x, INT32 CONST y ) CONST;
	/** \! Mouse double click callback. */
	virtual void callback_mouse_click_double(	INT32 CONST button, INT32 CONST x, INT32 CONST y ) CONST;

	/** \! Mouse move while pressed event callback. */
	virtual void callback_motion(				INT32 CONST x, INT32 CONST y ) CONST;
	/** \! Mouse move event callback. */
	virtual void callback_motion_passive(		INT32 CONST x, INT32 CONST y ) CONST;

	/** \! Window move event callback. */
	virtual void callback_move(					INT32 CONST x, INT32 CONST y ) CONST;

	/** \! Window resize/reshape event callback. */
	virtual void callback_reshape(				INT32 CONST sx, INT32 CONST sy ) CONST;

	/** \! Contextual menu event enter|exit callback. */
	virtual void callback_menu_loop(			bool b_enter ) CONST;
	/** \! Contextual menu event callback. */
	virtual void callback_menu(					INT32 CONST menu_id, UINT32 CONST item_id ) CONST;

	/** \! Window gain focus event callback. */
	virtual void callback_focus_got(			void ) CONST;
	/** \! Window lost focus event callback. */
	virtual void callback_focus_lost(			void ) CONST;

	/** \! Close event callback. */
	virtual void callback_close(				void ) CONST;

	/** \! Idle event callback. */
	virtual void callback_idle(					void ) CONST;

	/** \! Main display callback. */
	virtual void callback_display(				void ) CONST;

	/** \! Display change event callback. */
	virtual void callback_display_change(		void ) CONST;

	/** \! Device change event callback. */
	virtual void callback_device_change(		bool CONST b_arrival, bool CONST b_remove, bool CONST b_video ) CONST;

public:
	/** \! Register a renderer pointer. 
	* Return false if controller already registered. */
	bool register_renderer(		core::renderer *		renderer );
	/** \! Unregister a renderer pointer. 
	* Return false if controller not registered. */
	bool unregister_renderer(	core::renderer CONST *	renderer );
	/** \! Unregister and release  a renderer pointer. 
	* Return false if controller not registered. */
	bool release_renderer(		core::renderer CONST *	renderer );

public:
	/** Get system node. Please use with caution. */
	FINLINE	system_node * get_system_node( void ) CONST	{	return _p_system_node;	}
};

