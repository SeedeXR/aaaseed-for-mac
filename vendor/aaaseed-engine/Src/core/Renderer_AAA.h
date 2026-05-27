
#ifdef AAA_CORE_RENDERER_AAA_H
#error "RENDERER_AAA_H included more than once."
#endif
#define AAA_CORE_RENDERER_AAA_H 1


// Parent header
#ifndef AAA_CORE_RENDERER_H
#	include "Renderer.h"
#endif


namespace core
{
	/**
	* @class core::Renderer_AAA
	*
	* AAASeed main render engine.
	*/
	class renderer_AAA final : public renderer 
	{
	protected:
		C_NO_CPY_MOVE( renderer_AAA )
		/** Renderer_AAA class constructor. */
		renderer_AAA( void );
		/** Renderer_AAA class destructor */
		virtual ~renderer_AAA( void );


		/* \! Register to desired events. */
		virtual void register_to_events( void );
		/* \! Unregister from events. */
		virtual void unregister_to_events( void );


		/** \! Allocate and init class members. */
		virtual void init( void );

		/** \! Deallocate and release class members. */
		virtual void release( void );


	public:
		/** \! Create, init and return new pointer. */
		static renderer* create_ptr( void );


		/** Window move event callback. */
		static	void callback_move( INT32 CONST x, INT32 CONST y );
		virtual void callback_event_move( evt::MoveEventArgs CONST & p_args );

		/** Window resize/reshape event callback. */
		static	void callback_resize( INT32 CONST sx, INT32 CONST sy );
		virtual void callback_event_resize( evt::ResizeEventArgs CONST & p_args );

		/** Main display callback. */
		virtual void display( void );
	};

}	// namespace core

