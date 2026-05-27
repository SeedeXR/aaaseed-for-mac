
#ifdef AAA_GL_ENGINE_H
#error "GL_ENGINE_H included more than once."
#endif
#define AAA_GL_ENGINE_H 1


#ifndef AAA_ERR_H
#	include "err.h"
#endif
#ifndef _VECTOR_
	#include <vector>
#endif
#ifndef AAA_AAA_LOCK_H
#	include "aaa/aaa_lock.h"
#endif


class system_node;

namespace gl
{

	class item;

	///////////////////////////////////////////////////////////////////////////////////////////////
	//		Engine class
	///////////////////////////////////////////////////////////////////////////////////////////////

	/**
	* @class gl::Engine
	*
	* @brief manage OpenGL contexts activations and utilities (queues etc)
	*/
	class engine final : public c_obj
	{
		static engine*	instance_unique;

	private:
		using QUEUE = std::vector< gl::item * >;

		// Queues INIT
		QUEUE	_queue_init_first;
		QUEUE	_queue_init_second;
		QUEUE*	_queue_init_process;
		QUEUE*	_queue_init_stock;
	
		// Queues UPDATE
		QUEUE	_queue_update_first;
		QUEUE	_queue_update_second;
		QUEUE*	_queue_update_process;
		QUEUE*	_queue_update_stock;
	
		// Queues RELEASE
		QUEUE	_queue_release_first;
		QUEUE	_queue_release_second;
		QUEUE*	_queue_release_process;
		QUEUE*	_queue_release_stock;
	
		// Queues DELETE
		QUEUE	_queue_delete_first;
		QUEUE	_queue_delete_second;
		QUEUE*	_queue_delete_process;
		QUEUE*	_queue_delete_stock;
	
		// Queues SWAPED state
		mutable aaa::mutex_t	_mutex_queues;

		//<! Active context node.
		system_node*			_p_system_node_active;
	
	private:
		// helper fn
		void set_queues( bool b_first );
	protected:
		//	Invalidate copy constructor and assignation operator.
		C_NO_CPY_MOVE( engine )
		/** gl::Engine class constructor. */
		engine( void );
		/** gl::Engine class destructor. */
		virtual ~engine( void );


		/** \! Allocate and init class members. */
		void init( void );
		/** \! Release and delete class members. */
		void release( void );

	public:
		/** Create Engine unique instance as gl::Engine pointer. */
		static engine * create_instance( void );

		/** Get Engine unique instance as gl::Engine pointer. */
		//static engine * get_instance( void );
		static FINLINE engine * get_instance( void )
		{
//			if( !instance_unique )
//				debug_break( "%s() null instance", __FUNCTION__ );
			return instance_unique;
		}

		/** Release gl::Engine unique instance */
		static void release_instance( void );


		/** Set target node context active.
		*  If context is already active no change will occur.
		*/
		bool context_make_current( system_node * p_pNode );

		/** Release target node context activation.
		* If context is already released no change will occur
		*/
		void context_done_current( system_node * p_pNode );
	
	
		/** Swap queues. */
		void swap_queues( void );


		/** Put target gl::Item pointer in initialization queue with mutex protection. */
		void add_queue_init(	gl::item * p_pItem );

		/** Put target gl::Item pointer in update queue with mutex protection. */
		void add_queue_update(	gl::item * p_pItem );

		/** Put target gl::Item pointer in release queue with mutex protection. */
		void add_queue_release( gl::item* p_pItem );
		
		/** Put target gl::Item pointer in deletion queue with mutex protection. */
		void add_queue_delete(	gl::item * p_pItem );

		/** Put target gl::Item pointer in release and deletion queues with mutex protection. */
		void add_queue_release_and_delete( gl::item* p_pItem );


		/** Clear queues. */
		void clear_queues( void );


		/** Iterates queues and call associated OpenGL commands */
		void process_queues( void );

		/** Empty deletion queue and delete contained objects. */
		void process_queue_delete( void );


		/** Lock engine. */
		void lock( void );

		/** Unlock engine. */
		void unlock( void );
		
	}; // Engine class

} // namespace gl

