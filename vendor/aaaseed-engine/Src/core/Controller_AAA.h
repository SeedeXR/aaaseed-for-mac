
#ifdef AAA_CORE_CONTROLLER_AAA_H
#error "CONTROLLER_AAA_H included more than once."
#endif
#define AAA_CORE_CONTROLLER_AAA_H 1


// Parent header
#ifndef AAA_CORE_CONTROLLER_H
#	include "core/Controller.h"
#endif
#ifndef AAA_SYSTEMMOUSE_H
#	include "system/shared/SystemMouse.h"
#endif

namespace core
{
	/**
	* @class core::Controller_AAA
	*
	* Global AAASeed controller.
	* Extends core::Controller
	*/
	class AAA_controller final
		: public core::Controller
	{
	private:
		static DOUBLE		mouse_up_time;

		INT32				_mouse_x;
		INT32				_mouse_y;
//		bool				_b_mouse_changed;

		void	set_mouse_xy( INT32 CONST x, INT32 CONST y, C_PCHAR_C str_caller );

	protected:
		C_NO_CPY_MOVE( AAA_controller )
		/** Controller_AAA class constructor. */
		AAA_controller( void );
		/** Controller_AAA class destructor */
		virtual ~AAA_controller( void );


		/* \! Register to desired events. */
		virtual void register_to_events( void );
		/* \! Unregister from events. */
		virtual void unregister_to_events( void );


		/** \! Allocate and init class members. */
		void init( void );
		/** \! Deallocate and release class members. */
		virtual void release( void );


		///////////////////////////////////////////////////////////////////////////////////////////
		//		UTILS
		///////////////////////////////////////////////////////////////////////////////////////////

		/** \! Simplify mouse event callback. */
		void callback_event_mouse( mouse::BUTTON CONST button, mouse::STATE CONST state, INT32 CONST x, INT32 CONST y );
	public:
		/** \! Create, init and return new  scene::Controller pointer. */
		static AAA_controller * create_ptr( void );


		///////////////////////////////////////////////////////////////////////////////////////////
		//		EVENT CALLBACKS
		///////////////////////////////////////////////////////////////////////////////////////////

		/** \! Window menu callback. */
		virtual void callback_event_menu(			evt::MenuEventArgs CONST & arg );

		/** Keyboard down event callback. */
		virtual void callback_event_keyboard_down(	evt::KeyEventArgs CONST & args );
		/** Keyboard up event callback. */
		virtual void callback_event_keyboard_up(	evt::KeyEventArgs CONST & args );
		/** Text input event callback. */
		virtual void callback_event_text_input(		evt::KeyEventArgs CONST & args );


		/** Mouse down event callback. */
		virtual void callback_event_mouse_down(			evt::MouseEventArgs CONST & args );
		/** Mouse up event callback. */
		virtual void callback_event_mouse_up(			evt::MouseEventArgs CONST & args );
		/** Mouse double click callback. */
		virtual void callback_event_mouse_click_double(	evt::MouseEventArgs CONST & args );
		/** Mouse move while pressed event callback. */
		virtual void callback_event_motion(				evt::MouseEventArgs CONST & args );
		/** Mouse move event callback. */
		virtual void callback_event_motion_passive(		evt::MouseEventArgs CONST & args );


		/** \! Window focus got callback. */
		void callback_event_focus_got( void );
		/** \! Window focus lost callback. */
		void callback_event_focus_lost( void );


		/** \! Idle event callback. */
		// no idle event 2023 Jan
		//void callback_event_idle( void );

		virtual void callback_event_close(	void );

		/** \! Display change event callback. */
		virtual void callback_event_display_change();
		
		/** \! Device change event callback. */
		virtual void callback_event_device_change(		evt::DeviceChangeArgs CONST & args );

	private:
		static void callback_keyboard_low( INT32 CONST key_code, INT32 CONST modifier, INT32 CONST x, INT32 CONST y, bool CONST b_special );
		static void generate_mouse_event( mouse::BUTTON CONST button, mouse::STATE CONST state, INT32 CONST x, INT32 CONST y );
		static void callback_motion_low( INT32 CONST x, INT32 CONST y, bool CONST b_passive );
	};

}	// namespace core

