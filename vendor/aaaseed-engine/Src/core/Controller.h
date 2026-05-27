
#ifdef AAA_CORE_CONTROLLER_H
#error "CONTROLLER_H included more than once."
#endif
#define AAA_CORE_CONTROLLER_H 1


// Event header
#ifndef AAA_EVT_EVENT_H 
#	include "evt/Event.h"
#endif

namespace core
{
	/**
	* @class core::Controller
	*
	* Abstract base user interactions controler.
	* Forces user callbacks functions definitions.
	*/
	class Controller : public c_obj
	{
	protected:
		C_NO_CPY_MOVE( Controller )
		/** Controller class constructor. */
		Controller( void );
		/** Controller class destructor */
		virtual ~Controller( void );


		/* \! Register to desired events. */
		virtual void register_to_events( void ) = 0;
		/* \! Unregister from events. */
		virtual void unregister_to_events( void ) = 0;


		/** \! Deallocate and release class members. */
		virtual void release( void ) = 0;


	public:
		/** \! Release and delete pointer class members. 
		* Example :
		*	core::Controller::release_ptr( ptr );
		*	ptr = nullptr;
		*/
		static void release_ptr( core::Controller * pPtr );


		/** Keyboard down event callback. */
		virtual void callback_event_keyboard_down(		evt::KeyEventArgs CONST & p_args ) = 0;
		/** Keyboard up event callback. */
		virtual void callback_event_keyboard_up(		evt::KeyEventArgs CONST & p_args ) = 0;
		/** Text input event callback. */
		virtual void callback_event_text_input(			evt::KeyEventArgs CONST & p_args ) = 0;


		/** Mouse down event callback. */
		virtual void callback_event_mouse_down(			evt::MouseEventArgs CONST & p_args ) = 0;
		/** Mouse up event callback. */
		virtual void callback_event_mouse_up(			evt::MouseEventArgs CONST & p_args ) = 0;
		/** Mouse double click callback. */
		virtual void callback_event_mouse_click_double(	evt::MouseEventArgs CONST & p_args ) = 0;
		/** Mouse move while pressed event callback. */
		virtual void callback_event_motion(				evt::MouseEventArgs CONST & p_args ) = 0;
		/** Mouse move event callback. */
		virtual void callback_event_motion_passive(		evt::MouseEventArgs CONST & p_args ) = 0;
	};

}	// namespace core

