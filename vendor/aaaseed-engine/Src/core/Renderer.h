
#ifdef AAA_CORE_RENDERER_H
#error "RENDERER_H included more than once."
#endif
#define AAA_CORE_RENDERER_H 1


// Event header
#ifndef AAA_EVT_EVENT_H 
#	include "evt/Event.h"
#endif
#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif


namespace core
{
	/**
	* @class core::renderer
	*
	* Abstract base render engines class.
	* Forces display callbacks functions definitions.
	*/
	class renderer : public c_obj
	{
	protected:
		C_NO_CPY_MOVE( renderer )
		/** Renderer class constructor. */
		renderer( void );
		/** Renderer class destructor */
		virtual ~renderer( void );


		/* \! Register to desired events. */
		virtual void register_to_events( void ) = 0;
		/* \! Unregister from events. */
		virtual void unregister_to_events( void ) = 0;


		/** \! Allocate and init class members. */
		virtual void init( void ) = 0;

		/** \! Deallocate and release class members. */
		virtual void release( void ) = 0;


	public:
		/** \! Release and delete pointer. 
		* Example :
		*	core::renderer::release_ptr( ptr );
		*	ptr = nullptr;
		*/
		static void release_ptr( core::renderer* p );


		/** Window move event callback. */
		virtual void callback_event_move( evt::MoveEventArgs CONST & p_args ) = 0;

		/** Window resize/reshape event callback. */
		virtual void callback_event_resize( evt::ResizeEventArgs CONST & p_args ) = 0;

		/** Main display callback. */
		virtual void display( void ) = 0;
	};

}	// namespace core

