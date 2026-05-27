
#ifdef AAA_SYSTEM_EVENT_LISTENER_H
#error "SYSTEM_EVENT_LISTENER_H included more than once."
#endif
#define AAA_SYSTEM_EVENT_LISTENER_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef _DEQUE_
#	include <deque>
#endif
#ifndef AAA_NATIVE_SYSTEM_H
#	include "system/Native_system.h"
#endif
#ifndef AAA_NATIVE_TYPES_H
#	include "system/Native_types.h"
#endif
#ifndef AAA_SYSTEMEVENT_H
#	include "system/shared/SystemEvent.h"
#endif
#ifndef AAA_AAA_MUTEX_H
#	include "aaa/aaa_mutex.h"
#endif
/**
* @class system_event_listener
*
* @brief read and dispatch events from system message pump
* @note abstract base event reader class
*/
class system_event_listener final : public c_obj
{
protected:
	std::deque<evt::EVENT>	_deque_first;	//<! first event queue
	std::deque<evt::EVENT>	_deque_second;	//<! second event queue

	std::deque<evt::EVENT>*	_deque_front;	//<! front event queue shared pointer
	std::deque<evt::EVENT>*	_deque_back;	//<! back event queue shared pointer
	
	mutable aaa::MUTEX		_mutex;			//<! queue protection Mutex

protected:
	C_NO_CPY_MOVE( system_event_listener )
	/** system_event_listener class constructor. */
	system_event_listener( void );

	/** Alloc and init class members. */
	void init( void );

public:
	/** Create and return new system_event_listener pointer. */
	static system_event_listener * create_ptr( void );
	/** system_event_listener class destructor. */
	virtual ~system_event_listener( void );

	/** Swap event deque. */
	void swap_event( void );
	/** Add event at the back of the deque. */
	void push_back_event( evt::EVENT CONST event );

	/** Get front event and remove it from deque. */
	evt::EVENT get_event( void );

	/** Get active deque empty state. */
	bool empy_event( void );
};

