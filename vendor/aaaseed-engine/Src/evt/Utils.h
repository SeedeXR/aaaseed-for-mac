
#ifndef AAA_EVT_EVENT_H
#error "EventUtils shouldn't be included directly, include evt/Event.h"
#endif

#ifdef AAA_EVT_UTILS_H
#error "EVT_UTILS_H included more than once."
#endif
#define AAA_EVT_UTILS_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef Foundation_PriorityEvent_INCLUDED
#	include "Poco/PriorityEvent.h"
#endif
#ifndef Foundation_PriorityDelegate_INCLUDED
#	include "Poco/PriorityDelegate.h"
#endif
#ifndef __EVT_DELEGATE_H__ 
#	include "evt/Delegate.h"
#endif


namespace evt
{
	/** \! Define TEvent as a POCO FIFOEvent
	* to create your own events use: TEvent<argType> myEvent */
	template <typename ArgumentsType>
	class TEvent final
		: public Poco::PriorityEvent<ArgumentsType>, public c_obj	//todo check it is done well (cpy...)
	{
	public:
		TEvent( void )
			: Poco::PriorityEvent<ArgumentsType>()
		{}

		/* \! Allow copy of events, by copying everything except the mutex. */
		TEvent(const TEvent<ArgumentsType> & p_ref)
			: Poco::PriorityEvent<ArgumentsType>()
		{
			p_ref._mutex.lock();
			::_mutex.lock();
			::_strategy = p_ref._strategy;
			::_mutex.unlock();
			p_ref._mutex.unlock();

			::_enabled = p_ref._enabled;
		}

		TEvent<ArgumentsType> & operator=(const TEvent<ArgumentsType> & p_ref)
		{
			if(&p_ref == this) return *this;

			p_ref._mutex.lock();
			::_mutex.lock();
			::_strategy = p_ref._strategy;
			::_mutex.unlock();
			p_ref._mutex.unlock();

			::_enabled = p_ref._enabled;
		
			return *this;
		}

	};


	enum EventOrder : INT32
	{
		EVENT_ORDER_BEFORE_APP	= 0,
		EVENT_ORDER_APP			= 100,
		EVENT_ORDER_AFTER_APP	= 200
	};


	///////////////////////////////////////////////////////////////////////////////////////////////
	//		ADD LISTENERS
	///////////////////////////////////////////////////////////////////////////////////////////////

	/** \! Register any method of any class to an event.
	* the method must provide one of the following
	* signatures:
	*	void method(ArgumentsType & p_args)
	*	void method(const void * p_pSender, ArgumentsType &p_args)
	* ie:
	*	add_listener(addon.newIntEvent, this, &Class::method) */

	//=============================================================================================
	template <class ST_EVENT_TYPE, typename ArgumentsType, class ListenerClass>
	static void add_listener( ST_EVENT_TYPE & event, ListenerClass  * p_pListener, void (ListenerClass::*listenerMethod)(const void*, ArgumentsType&), int32_t prio=EVENT_ORDER_AFTER_APP )
	{
		event -= Poco::priorityDelegate(p_pListener, listenerMethod, prio);
		event += Poco::priorityDelegate(p_pListener, listenerMethod, prio);
	}

	//=============================================================================================
	template <class ST_EVENT_TYPE, typename ArgumentsType, class ListenerClass>
	static void add_listener( ST_EVENT_TYPE & event, ListenerClass  * p_pListener, void (ListenerClass::*listenerMethod)(ArgumentsType&), int32_t prio=EVENT_ORDER_AFTER_APP )
	{
		event -= Poco::priorityDelegate(p_pListener, listenerMethod, prio);
		event += Poco::priorityDelegate(p_pListener, listenerMethod, prio);
	}

	//=============================================================================================
	template <class ListenerClass>
	static void add_listener( evt::TEvent<void> & event, ListenerClass  * p_pListener, void (ListenerClass::*listenerMethod)(const void*), int32_t prio=EVENT_ORDER_AFTER_APP )
	{
		event -= Poco::priorityDelegate(p_pListener, listenerMethod, prio);
		event += Poco::priorityDelegate(p_pListener, listenerMethod, prio);
	}

	//=============================================================================================
	template <class ListenerClass>
	static void add_listener( evt::TEvent<void> & event, ListenerClass  * p_pListener, void (ListenerClass::*listenerMethod)(), int32_t prio=EVENT_ORDER_AFTER_APP )
	{
		event -= Poco::priorityDelegate(p_pListener, listenerMethod, prio);
		event += Poco::priorityDelegate(p_pListener, listenerMethod, prio);
	}

	//=============================================================================================
	template <class ST_EVENT_TYPE, typename ArgumentsType, class ListenerClass>
	static void add_listener( ST_EVENT_TYPE & event, ListenerClass  * p_pListener, bool (ListenerClass::*listenerMethod)(const void*, ArgumentsType&), int32_t prio=EVENT_ORDER_AFTER_APP )
	{
		event -= evt::Delegate<ListenerClass,ArgumentsType,true>(p_pListener, listenerMethod, prio);
		event += evt::Delegate<ListenerClass,ArgumentsType,true>(p_pListener, listenerMethod, prio);
	}

	//=============================================================================================
	template <class ST_EVENT_TYPE, typename ArgumentsType, class ListenerClass>
	static void add_listener( ST_EVENT_TYPE & event, ListenerClass  * p_pListener, bool (ListenerClass::*listenerMethod)(ArgumentsType&), int32_t prio=EVENT_ORDER_AFTER_APP )
	{
		event -= evt::Delegate<ListenerClass,ArgumentsType,false>(p_pListener, listenerMethod, prio);
		event += evt::Delegate<ListenerClass,ArgumentsType,false>(p_pListener, listenerMethod, prio);
	}

	//=============================================================================================
	template <class ListenerClass>
	static void add_listener( evt::TEvent<void> & event, ListenerClass  * p_pListener, bool (ListenerClass::*listenerMethod)(const void*), int32_t prio=EVENT_ORDER_AFTER_APP )
	{
		event -= evt::Delegate<ListenerClass,void,true>(p_pListener, listenerMethod, prio);
		event += evt::Delegate<ListenerClass,void,true>(p_pListener, listenerMethod, prio);
	}

	//=============================================================================================
	template <class ListenerClass>
	static void add_listener( evt::TEvent<void> & event, ListenerClass  * p_pListener, bool (ListenerClass::*listenerMethod)(), int32_t prio=EVENT_ORDER_AFTER_APP )
	{
		event -= evt::Delegate<ListenerClass,void,false>(p_pListener, listenerMethod, prio);
		event += evt::Delegate<ListenerClass,void,false>(p_pListener, listenerMethod, prio);
	}


	///////////////////////////////////////////////////////////////////////////////////////////////
	//		REMOVE LISTENERS
	///////////////////////////////////////////////////////////////////////////////////////////////

	/** Unregister any method of any class to an event.
	* the method must provide one the following
	* signatures:
	*	void method(ArgumentsType & p_args)
	*	void method(const void * p_pSender, ArgumentsType &p_args)
	* ie:
	*	add_listener(addon.newIntEvent, this, &Class::method) */

	//=============================================================================================
	template <class ST_EVENT_TYPE, typename ArgumentsType, class ListenerClass>
	static void remove_listener( ST_EVENT_TYPE & event, ListenerClass  * p_pListener, void (ListenerClass::*listenerMethod)(const void*, ArgumentsType&), int32_t prio=EVENT_ORDER_AFTER_APP )
	{
		event -= Poco::priorityDelegate(p_pListener, listenerMethod, prio);
	}

	//=============================================================================================
	template <class ST_EVENT_TYPE, typename ArgumentsType, class ListenerClass>
	static void remove_listener( ST_EVENT_TYPE & event, ListenerClass  * p_pListener, void (ListenerClass::*listenerMethod)(ArgumentsType&), int32_t prio=EVENT_ORDER_AFTER_APP ) 
	{
		event -= Poco::priorityDelegate(p_pListener, listenerMethod, prio);
	}

	//=============================================================================================
	template <class ListenerClass>
	static void remove_listener( evt::TEvent<void> & event, ListenerClass  * p_pListener, void (ListenerClass::*listenerMethod)(const void*), int32_t prio=EVENT_ORDER_AFTER_APP )
	{
		event -= Poco::priorityDelegate(p_pListener, listenerMethod, prio);
	}

	//=============================================================================================
	template <class ListenerClass>
	static void remove_listener( evt::TEvent<void> & event, ListenerClass  * p_pListener, void (ListenerClass::*listenerMethod)(), int32_t prio=EVENT_ORDER_AFTER_APP )
	{
		event -= Poco::priorityDelegate(p_pListener, listenerMethod, prio);
	}

	//=============================================================================================
	template <class ST_EVENT_TYPE, typename ArgumentsType, class ListenerClass>
	static void remove_listener( ST_EVENT_TYPE & event, ListenerClass  * p_pListener, bool (ListenerClass::*listenerMethod)(const void*, ArgumentsType&), int32_t prio=EVENT_ORDER_AFTER_APP )
	{
		event -= evt::Delegate<ListenerClass,ArgumentsType,true>(p_pListener, listenerMethod, prio);
	}

	//=============================================================================================
	template <class ST_EVENT_TYPE, typename ArgumentsType, class ListenerClass>
	static void remove_listener( ST_EVENT_TYPE & event, ListenerClass  * p_pListener, bool (ListenerClass::*listenerMethod)(ArgumentsType&), int32_t prio=EVENT_ORDER_AFTER_APP )
	{
		event -= evt::Delegate<ListenerClass,ArgumentsType,false>(p_pListener, listenerMethod, prio);
	}

	//=============================================================================================
	template <class ListenerClass>
	static void remove_listener( evt::TEvent<void> & event, ListenerClass  * p_pListener, bool (ListenerClass::*listenerMethod)(const void*), int32_t prio=EVENT_ORDER_AFTER_APP )
	{
		event -= evt::Delegate<ListenerClass,void,true>(p_pListener, listenerMethod, prio);
	}

	//=============================================================================================
	template <class ListenerClass>
	static void remove_listener( evt::TEvent<void> & event, ListenerClass  * p_pListener, bool (ListenerClass::*listenerMethod)(), int32_t prio=EVENT_ORDER_AFTER_APP )
	{
		event -= evt::Delegate<ListenerClass,void,false>(p_pListener, listenerMethod, prio);
	}


	///////////////////////////////////////////////////////////////////////////////////////////////
	//		NOTIFY EVENTS
	///////////////////////////////////////////////////////////////////////////////////////////////

	/** \! Notifies an event so all the registered listeners get called
	* ie:
	*	evt::notify_event( p_intEvent, p_intArgument, this )
	*
	* or in case there's no sender:
	*	evt::notify_event( p_intEvent, p_intArgument ) 
	*
	* or in case there is no sender and no argument
	*	evt::notify_event( event ) */

	//=============================================================================================
	template <class ST_EVENT_TYPE, typename ArgumentsType, typename SenderType>
	static void notify_event( ST_EVENT_TYPE & event, ArgumentsType & p_args, SenderType * p_pSender )
	{
		event.notify( p_pSender, p_args );
	}

	//=============================================================================================
	template <class ST_EVENT_TYPE, typename ArgumentsType>
	static void notify_event( ST_EVENT_TYPE & event, ArgumentsType & p_args )
	{
		event.notify( nullptr, p_args );
	}

	//=============================================================================================
	template <class ST_EVENT_TYPE, typename ArgumentsType, typename SenderType>
	static void notify_event( ST_EVENT_TYPE & event, const ArgumentsType & p_args, SenderType * p_pSender )
	{
		event.notify( p_pSender, p_args );
	}

	//=============================================================================================
	template <class ST_EVENT_TYPE, typename ArgumentsType>
	static void notify_event( ST_EVENT_TYPE & event, const ArgumentsType & p_args )
	{
		event.notify( nullptr, p_args );
	}

	//=============================================================================================
	template <typename SenderType>
	static void notify_event( evt::TEvent<void> & event, SenderType * p_pSender )
	{
		event.notify( p_pSender );
	}

	//=============================================================================================
	template <class ST_EVENT_TYPE>
	static void notify_event( ST_EVENT_TYPE & event)
	{
		event.notify( nullptr );
	}

} // namespace evt

