
#ifdef AAA_EVT_EVENT_H
#error "EVT_EVENT_H included more than once."
#endif
#define AAA_EVT_EVENT_H 1

#ifndef AAA_EVT_UTILS_H
	#include "evt/Utils.h"
#endif


namespace evt
{
//	class event_listener;

	/** @class evt::Args
	* This is used to pass the data when notifying events.
	*/
	//todo make it derived from c_obj (deal with cpy/mve)
	class Args	
	{};


	class KeyEventArgs final
		: public evt::Args 
	{
	public:
		enum Type{
			pressed,
			released,
			text_input
		} type;

		INT32		key;
		INT32		mod;
		bool		b_special;
	};


	class MouseEventArgs final
		: public evt::Args 
	{
	public:
		enum Type{
			pressed,
			released,
			click_double,
			motion,
			motion_passive
		} type;

		INT32		button;
		INT32		x;
		INT32		y;
	};


	class MoveEventArgs final
		: public evt::Args 
	{
	public:
		INT32 x;
		INT32 y;
	};

	class ResizeEventArgs final
		: public evt::Args 
	{
	public:
		INT32 width;
		INT32 height;
	};


	class TouchEventArgs final
		: public evt::Args 
	{
	public:
		enum Type{
			down,
			up,
			move,
			doubleTap,
			cancel
		} type;

		INT32		id;
		INT32		time;
		INT32		numTouches;
		float		width;
		float		height;
		float		angle;
		float		minoraxis;
		float		majoraxis;
		float		pressure;
		float		xspeed;
		float		yspeed;
		float		xaccel;
		float		yaccel;
	};


	class MenuEventArgs final
		: public evt::Args
	{
	public:
		INT32		menu_id;
		UINT32		item_id;
	};

	class DeviceChangeArgs final
		: public evt::Args
	{
	public:
		bool		b_video;
		bool		b_arrival;
		bool		b_remove;
	};

	class core_events final : public c_obj
	{
	public:
		C_NO_CPY_MOVE( core_events )

		static TEvent<KeyEventArgs CONST> 		key_pressed;
		static TEvent<KeyEventArgs CONST> 		key_released;
		static TEvent<KeyEventArgs CONST> 		text_input;

		static TEvent<MouseEventArgs CONST>		mouse_motion_passive;
		static TEvent<MouseEventArgs CONST>		mouse_motion;
		static TEvent<MouseEventArgs CONST>		mouse_down;
		static TEvent<MouseEventArgs CONST>		mouse_click_double;
		static TEvent<MouseEventArgs CONST>		mouse_up;
		static TEvent<Args CONST>				mouse_enter;
		static TEvent<Args CONST>				mouse_leave;

//todo revive
		//static TEvent<TouchEventArgs>			touchDown;
		//static TEvent<TouchEventArgs>			touchUp;
		//static TEvent<TouchEventArgs>			touchMoved;
		//static TEvent<TouchEventArgs>			touchDoubleTap;
		//static TEvent<TouchEventArgs>			touchCancelled;

		static TEvent<MoveEventArgs CONST>		window_moved;
		static TEvent<ResizeEventArgs CONST>	window_resized;

		static TEvent<MenuEventArgs CONST>		menu;

		static TEvent<void>						focus_got;
		static TEvent<void>						focus_lost;

		static TEvent<void>						idle;
		static TEvent<void>						display;

		static TEvent<void>						close;

		static TEvent<void>						application_exit;
		static TEvent<void>						display_change;
		static TEvent<DeviceChangeArgs CONST>	device_change;

		/** \! Disable core events dispatch. */
		static void disable( void );
		/** \! Enable core events dispatch. */
		static void enable( void );
	};


	///////////////////////////////////////////////////////////////////////////////////////////////
	//		REGISTER TO EVENTS
	///////////////////////////////////////////////////////////////////////////////////////////////

	//=============================================================================================
	template<class LISTENER>
	void register_event_key( LISTENER * listener, INT32 prio=EVENT_ORDER_AFTER_APP )
	{
		add_listener( core_events::key_pressed,			listener, &LISTENER::callback_event_keyboard_down,	prio );
		add_listener( core_events::key_released,		listener, &LISTENER::callback_event_keyboard_up,	prio );
		add_listener( core_events::text_input,			listener, &LISTENER::callback_event_text_input,		prio );
	}

	template<class LISTENER>
	void register_event_mouse( LISTENER * listener, bool b_click_double=true, INT32 prio=EVENT_ORDER_AFTER_APP )
	{
		add_listener( core_events::mouse_down,				listener, &LISTENER::callback_event_mouse_down,			prio );
		add_listener( core_events::mouse_up,				listener, &LISTENER::callback_event_mouse_up,			prio );
		if( b_click_double )
			add_listener( core_events::mouse_click_double,	listener, &LISTENER::callback_event_mouse_click_double,	prio );
		add_listener( core_events::mouse_motion,			listener, &LISTENER::callback_event_motion,				prio );
		add_listener( core_events::mouse_motion_passive,	listener, &LISTENER::callback_event_motion_passive,		prio );
	}

//todo
	////=============================================================================================
	//template<class LISTENER>
	//void register_event_touch( LISTENER * listener, INT32 prio=EVENT_ORDER_AFTER_APP  )
	//{
	//	add_listener( core_events::touchDoubleTap,	listener, &LISTENER::callback_event_touchDoubleTap,	prio );
	//	add_listener( core_events::touchDown,		listener, &LISTENER::callback_event_touchDown,		prio );
	//	add_listener( core_events::touchMoved,		listener, &LISTENER::callback_event_touchMoved,		prio );
	//	add_listener( core_events::touchUp,			listener, &LISTENER::callback_event_touchUp,			prio );
	//}

	//=============================================================================================
	template<class LISTENER>
	void register_event_window_move( LISTENER * listener, INT32 prio=EVENT_ORDER_AFTER_APP )
	{
		add_listener( core_events::window_moved, listener, &LISTENER::callback_event_move, prio );
	}

	//=============================================================================================
	template<class LISTENER>
	void register_event_window_resize( LISTENER * listener, INT32 prio=EVENT_ORDER_AFTER_APP )
	{
		add_listener( core_events::window_resized, listener, &LISTENER::callback_event_resize, prio );
	}

	//=============================================================================================
	template<class LISTENER>
	void register_event_menu( LISTENER* listener, INT32 prio = EVENT_ORDER_AFTER_APP )
	{
		add_listener( core_events::menu, listener, &LISTENER::callback_event_menu, prio );
	}

	//=============================================================================================
	template<class LISTENER>
	void register_event_focus( LISTENER * listener, INT32 prio=EVENT_ORDER_AFTER_APP )
	{
		add_listener( core_events::focus_got, listener,		&LISTENER::callback_event_focus_got,	prio );
		add_listener( core_events::focus_lost, listener,	&LISTENER::callback_event_focus_lost,	prio );
	}

	//=============================================================================================
	template<class LISTENER>
	void register_event_close( LISTENER * listener, INT32 prio=EVENT_ORDER_AFTER_APP )
	{
		add_listener( core_events::close, listener, &LISTENER::callback_event_close, prio );
	}

	//=============================================================================================
	// no idle event 2023 Jan
	//template<class LISTENER>
	//void register_event_idle( LISTENER * listener, INT32 prio=EVENT_ORDER_AFTER_APP )
	//{
	//	add_listener( core_events::idle, listener, &LISTENER::callback_event_idle, prio );
	//}

	//=============================================================================================
	template<class LISTENER>
	void register_event_display( LISTENER * listener, INT32 prio=EVENT_ORDER_AFTER_APP )
	{
		add_listener( core_events::display, listener, &LISTENER::callback_event_display, prio );
	}

	//=============================================================================================
	template<class LISTENER>
	void register_event_display_change( LISTENER * listener, INT32 prio=EVENT_ORDER_AFTER_APP )
	{
		add_listener( core_events::display_change, listener, &LISTENER::callback_event_display_change, prio );
	}

	template<class LISTENER>
	void register_event_device_change( LISTENER * listener, INT32 prio=EVENT_ORDER_AFTER_APP )
	{
		add_listener( core_events::device_change, listener, &LISTENER::callback_event_device_change, prio );
	}

	//=============================================================================================
	template<class LISTENER>
	void register_event_application_exit( LISTENER * listener, INT32 prio=EVENT_ORDER_AFTER_APP )
	{
		add_listener( core_events::application_exit, listener, &LISTENER::callback_event_app_exit, prio );
	}



	///////////////////////////////////////////////////////////////////////////////////////////////
	//		UNREGISTER FROM EVENTS
	///////////////////////////////////////////////////////////////////////////////////////////////

	//=============================================================================================
	template<class LISTENER>
	void unregister_event_key( LISTENER * listener, INT32 prio=EVENT_ORDER_AFTER_APP  )
	{
		evt::remove_listener( core_events::key_pressed,		listener, &LISTENER::callback_event_keyboard_down,	prio );
		evt::remove_listener( core_events::key_released,	listener, &LISTENER::callback_event_keyboard_up,	prio );
		evt::remove_listener( core_events::text_input,		listener, &LISTENER::callback_event_text_input,		prio );
	}

	//=============================================================================================
	template<class LISTENER>
	void unregister_event_mouse( LISTENER * listener,  bool b_click_double=true, INT32 prio=EVENT_ORDER_AFTER_APP  )
	{
		evt::remove_listener( core_events::mouse_down,				listener, &LISTENER::callback_event_mouse_down,			prio );
		evt::remove_listener( core_events::mouse_up,				listener, &LISTENER::callback_event_mouse_up,			prio );
		if( b_click_double )
			evt::remove_listener( core_events::mouse_click_double,	listener, &LISTENER::callback_event_mouse_click_double,	prio );
		evt::remove_listener( core_events::mouse_motion,			listener, &LISTENER::callback_event_motion,				prio );
		evt::remove_listener( core_events::mouse_motion_passive,	listener, &LISTENER::callback_event_motion_passive,		prio );
	}

	////=============================================================================================
	//template<class LISTENER>
	//void unregister_event_touch( LISTENER * listener, INT32 prio=EVENT_ORDER_AFTER_APP  )
	//{
	//	evt::remove_listener( core_events::touchDoubleTap,	listener, &LISTENER::callback_event_ouchDoubleTap, prio );
	//	evt::remove_listener( core_events::touchDown,		listener, &LISTENER::callback_event_ouchDown, prio );
	//	evt::remove_listener( core_events::touchMoved,		listener, &LISTENER::callback_event_ouchMoved, prio );
	//	evt::remove_listener( core_events::touchUp,			listener, &LISTENER::callback_event_ouchUp, prio );
	//}

	//=============================================================================================
	template<class LISTENER>
	void unregister_event_window_move( LISTENER * listener, INT32 prio=EVENT_ORDER_AFTER_APP  )
	{
		evt::remove_listener( core_events::window_moved, listener, &LISTENER::callback_event_move, prio );
	}

	//=============================================================================================
	template<class LISTENER>
	void unregister_event_window_resize( LISTENER * listener, INT32 prio=EVENT_ORDER_AFTER_APP  )
	{
		evt::remove_listener( core_events::window_resized, listener, &LISTENER::callback_event_resize, prio );
	}

	//=============================================================================================
	template<class LISTENER>
	void unregister_event_menu( LISTENER * listener, INT32 prio = EVENT_ORDER_AFTER_APP )
	{
		evt::remove_listener( core_events::menu, listener, &LISTENER::callback_event_menu, prio );
	}

	//=============================================================================================
	template<class LISTENER>
	void unregister_event_focus( LISTENER * listener, INT32 prio=EVENT_ORDER_AFTER_APP  )
	{
		evt::remove_listener( core_events::focus_got,	listener, &LISTENER::callback_event_focus_got,	prio );
		evt::remove_listener( core_events::focus_lost,	listener, &LISTENER::callback_event_focus_lost,	prio );
	}

	//=============================================================================================
	template<class LISTENER>
	void unregister_event_close( LISTENER * listener, INT32 prio=EVENT_ORDER_AFTER_APP  )
	{
		evt::remove_listener( core_events::close, listener, &LISTENER::callback_event_close, prio );
	}

	//=============================================================================================
	// no idle event 2023 Jan
	//template<class LISTENER>
	//void unregister_event_idle( LISTENER * listener, INT32 prio=EVENT_ORDER_AFTER_APP  )
	//{
	//	evt::remove_listener( core_events::idle, listener, &LISTENER::callback_event_idle, prio );
	//}

	//=============================================================================================
	template<class LISTENER>
	void unregister_event_display( LISTENER * listener, INT32 prio=EVENT_ORDER_AFTER_APP  )
	{
		evt::remove_listener( core_events::display, listener, &LISTENER::callback_event_display, prio );
	}

	//=============================================================================================
	template<class LISTENER>
	void unregister_event_display_change( LISTENER * listener, INT32 prio=EVENT_ORDER_AFTER_APP  )
	{
		evt::remove_listener( core_events::display_change, listener, &LISTENER::callback_event_display_change, prio );
	}

	
	//=============================================================================================
	template<class LISTENER>
	void unregister_event_device_change( LISTENER * listener, INT32 prio=EVENT_ORDER_AFTER_APP  )
	{
		evt::remove_listener( core_events::device_change, listener, &LISTENER::callback_event_device_change, prio );
	}
	//=============================================================================================
	template<class LISTENER>
	void unregister_event_application_exit( LISTENER * listener, INT32 prio=EVENT_ORDER_AFTER_APP  )
	{
		evt::remove_listener( core_events::application_exit, listener, &LISTENER::callback_event_app_exit, prio );
	}


	///////////////////////////////////////////////////////////////////////////////////////////////////
	//		Event notification
	///////////////////////////////////////////////////////////////////////////////////////////////////

	void notify_key_pressed(			INT32 CONST key, INT32 CONST mod, bool CONST b_special );
	void notify_key_released(			INT32 CONST key, INT32 CONST mod, bool CONST b_special );
	void notify_text_input(				INT32 CONST key, INT32 CONST mod );

	void notify_key_event(				KeyEventArgs CONST & p_args );

	void notify_mouse_down(				INT32 CONST button,	INT32 CONST x, INT32 CONST y );
	void notify_mouse_up(				INT32 CONST button,	INT32 CONST x, INT32 CONST y );
	void notify_mouse_click_double(		INT32 CONST button,	INT32 CONST x, INT32 CONST y );
	void notify_mouse_motion(								INT32 CONST x, INT32 CONST y );
	void notify_mouse_passive_motion(						INT32 CONST x, INT32 CONST y );
	void notify_mouse_event(			MouseEventArgs CONST & p_args );

	void notify_window_move(			INT32 CONST x,	INT32 CONST y );
	void notify_window_move_event(		MoveEventArgs CONST & p_args);

	void notify_window_resize(			INT32 CONST sx,	INT32 CONST sy );
	void notify_window_resize_event(	ResizeEventArgs CONST & p_args);

	void notify_menu(					INT32 CONST menu_id, UINT32 CONST item_id );
	void notify_menu(					MenuEventArgs CONST & args );

	void notify_focus_got(				void );
	void notify_focus_lost(				void );

	void notify_idle(					void );

	void notify_display(				void );

	void notify_close(					void );

	void notify_app_exit(				void );

	void notify_display_change(			void );

	void notify_device_change(			bool CONST b_arrival, bool CONST b_remove, bool CONST b_video );


} // namespace evt


