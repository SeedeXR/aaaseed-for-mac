
#ifdef AAA_EVT_DELEGATE_H
#error "EVT_DELEGATE_H included more than once."
#endif
#define AAA_EVT_DELEGATE_H 1

#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef AAA_AAA_MUTEX_H
#	include "aaa/aaa_mutex.h"
#endif
#ifndef AAA_ERR_H
#	include "err.h"
#endif

namespace evt
{
	template <class TArgs>
	class AbstractDelegate : public c_obj
		/// Base class for Delegate and Expire.
	{
	public:
		AbstractDelegate()									{}
		AbstractDelegate(const AbstractDelegate& /*del*/)	{}
		virtual ~AbstractDelegate()							{}

		virtual bool notify(const void* sender, TArgs& arguments) = 0;
			/// Invokes the delegate's callback function.
			/// Returns true if successful, or false if the delegate
			/// has been disabled or has expired.

		virtual bool equals(const AbstractDelegate& other) const = 0;
			/// Compares the AbstractDelegate with the other one for equality.

		virtual AbstractDelegate* clone() const = 0;
			/// Returns a deep copy of the AbstractDelegate.

		virtual void disable() = 0;
			/// Disables the delegate, which is done prior to removal.
		
		virtual const AbstractDelegate* unwrap() const
			/// Returns the unwrapped delegate. Must be overridden by decorators
			/// like Expire.
		{
			return this;
		}
	};


	template <> 
	class AbstractDelegate<void> : public c_obj
		/// Base class for Delegate and Expire.
	{
	public:
		AbstractDelegate()							{}
		AbstractDelegate(const AbstractDelegate&)	{}
		virtual ~AbstractDelegate()					{}

		virtual bool notify(const void* sender) = 0;
			/// Invokes the delegate's callback function.
			/// Returns true if successful, or false if the delegate
			/// has been disabled or has expired.

		virtual bool equals(const AbstractDelegate& other) const = 0;
			/// Compares the AbstractDelegate with the other one for equality.

		virtual AbstractDelegate* clone() const = 0;
			/// Returns a deep copy of the AbstractDelegate.

		virtual void disable() = 0;
			/// Disables the delegate, which is done prior to removal.
		
		virtual const AbstractDelegate* unwrap() const
			/// Returns the unwrapped delegate. Must be overridden by decorators
			/// like Expire.
		{
			return this;
		}
	};

	template <class TArgs> 
	class AbstractPriorityDelegate: public AbstractDelegate<TArgs>
		/// Base class for PriorityDelegate and PriorityExpire.
		///
		/// Extends AbstractDelegate with a priority value.
	{
	public:
		AbstractPriorityDelegate(int prio):
			_priority(prio)
		{
		}

		AbstractPriorityDelegate(const AbstractPriorityDelegate& del):
			AbstractDelegate<TArgs>(del),
			_priority(del._priority)
		{
		}

		virtual ~AbstractPriorityDelegate() 
		{
		}

		int priority() const
		{
			return _priority;
		}

	protected:
		int _priority;
	};

	template <class TObj, class TArgs, bool useSender=true>
	class Delegate final
		: public AbstractPriorityDelegate<TArgs>
	{
	public:
		typedef bool (TObj::*NotifyMethod)(const void*, TArgs&);

	protected:
		TObj*				_receiverObject;
		NotifyMethod		_receiverMethod;
		mutable aaa::MUTEX	_mutex;

	private:
		Delegate( void );
	public:
		Delegate( TObj* obj, NotifyMethod method, int32_t prio );
		Delegate( const Delegate & p_delegate );
		Delegate& operator = ( const Delegate & p_delegate );
		~Delegate( void );

		bool notify( const void * sender, TArgs & arguments );
		bool equals( const AbstractDelegate<TArgs> & other ) const;
		virtual AbstractDelegate<TArgs>* clone( void ) const;
		void disable( void );
	};



	template <class TObj, class TArgs>
	class Delegate<TObj, TArgs, false> final
		: public AbstractPriorityDelegate<TArgs>
	{
	public:
		typedef bool (TObj::*NotifyMethod)(TArgs&);

	protected:
		TObj*				_receiverObject;
		NotifyMethod		_receiverMethod;
		mutable aaa::MUTEX	_mutex;

	private:
		Delegate( void );
	public:
		Delegate( TObj* obj, NotifyMethod method, int32_t prio );
		Delegate( const Delegate & p_delegate );
		Delegate& operator = ( const Delegate & p_delegate );
		~Delegate( void );

		bool notify( const void * sender, TArgs & arguments );
		bool equals( const AbstractDelegate<TArgs> & other ) const;
		virtual AbstractDelegate<TArgs>* clone( void ) const;
		void disable( void );
	};



	template <class TObj>
	class Delegate<TObj, void, true> final
		: public AbstractPriorityDelegate<void>
	{
	public:
		typedef bool (TObj::*NotifyMethod)(const void*);

	protected:
		TObj*				_receiverObject;
		NotifyMethod		_receiverMethod;
		mutable aaa::MUTEX	_mutex;

	private:
		Delegate( void );
	public:
		Delegate( TObj* obj, NotifyMethod method, int32_t prio );
		Delegate( const Delegate & p_delegate );
		Delegate& operator = ( const Delegate & p_delegate );
		~Delegate(void );

		bool notify( const void * sender );
		bool equals( const AbstractDelegate<void> & other ) const;
		virtual AbstractDelegate<void>* clone( void ) const;
		void disable( void );
	};



	template <class TObj>
	class Delegate<TObj, void, false> final
		: public AbstractPriorityDelegate<void>
	{
	public:
		typedef void (TObj::*NotifyMethod)();

	protected:
		TObj*				_receiverObject;
		NotifyMethod		_receiverMethod;
		mutable aaa::MUTEX	_mutex;

	private:
		Delegate( void );
	public:
		Delegate( TObj* obj, NotifyMethod method, int32_t prio );
		Delegate( const Delegate & p_delegate );
		Delegate& operator = ( const Delegate & p_delegate );
		~Delegate( void );

		bool notify( const void * sender );
		bool equals( const AbstractDelegate<void> & other ) const;
		virtual AbstractDelegate<void> * clone( void ) const;
		void disable( void );
	};


	//=================================================================================================
	template <class TObj, class TArgs, bool useSender>
	Delegate<TObj, TArgs, useSender>::Delegate( void )
		: AbstractPriorityDelegate<TArgs>()
	{}

	//=================================================================================================
	template <class TObj, class TArgs, bool useSender>
	Delegate<TObj, TArgs, useSender>::Delegate( TObj* obj, NotifyMethod method, int32_t prio )
		: AbstractPriorityDelegate<TArgs>(prio)

		, _receiverObject(obj)
		, _receiverMethod(method)
	{}

	//=================================================================================================
	template <class TObj, class TArgs, bool useSender>
	Delegate<TObj, TArgs, useSender>::Delegate( const Delegate<TObj, TArgs, useSender> & p_delegate )
		: AbstractPriorityDelegate<TArgs>(p_delegate)

		, _receiverObject(p_delegate._receiverObject)
		, _receiverMethod(p_delegate._receiverMethod)
	{}

	//=================================================================================================
	template <class TObj, class TArgs, bool useSender>
	Delegate<TObj, TArgs, useSender> & Delegate<TObj, TArgs, useSender>::operator=( const Delegate<TObj, TArgs, useSender> & p_delegate )
	{
		if( &p_delegate != this )
		{
			::_pTarget        = p_delegate._pTarget;
			_receiverObject = p_delegate._receiverObject;
			_receiverMethod = p_delegate._receiverMethod;
			::_priority       = p_delegate._priority;
		}
		return *this;
	}

	//=================================================================================================
	template <class TObj, class TArgs, bool useSender>
	Delegate<TObj, TArgs, useSender>::~Delegate( void )
	{}

	//=================================================================================================
	template <class TObj, class TArgs, bool useSender>
	bool Delegate<TObj, TArgs, useSender>::notify( const void * sender, TArgs & arguments )
	{
		std::lock_guard<aaa::MUTEX> guard(_mutex);
		if( _receiverObject )
		{
			if(	!(_receiverObject->*_receiverMethod)(sender, arguments) ) 
			{
				return true;
			}
#if AAA_DEBUG()
			else
				debug_break( "Receiver class does not hold the target event method !!!" );
#endif
		}

		return false;
	}

	//=================================================================================================
	template <class TObj, class TArgs, bool useSender>
	bool Delegate<TObj, TArgs, useSender>::equals( const AbstractDelegate<TArgs> & other ) const
	{
		const Delegate* pOtherDelegate = dynamic_cast<const Delegate*>( other.unwrap() );
		return ( pOtherDelegate 
			&& ::priority() == pOtherDelegate->priority() 
			&& _receiverObject == pOtherDelegate->_receiverObject 
			&& _receiverMethod == pOtherDelegate->_receiverMethod );
	}

	//=================================================================================================
	template <class TObj, class TArgs, bool useSender>
	AbstractDelegate<TArgs>* Delegate<TObj, TArgs, useSender>::clone( void ) const
	{
		return new Delegate<TObj, TArgs, useSender>(*this);
	}

	//=================================================================================================
	template <class TObj, class TArgs, bool useSender>
	void Delegate<TObj, TArgs, useSender>::disable( void )
	{
		std::lock_guard<aaa::MUTEX> guard(_mutex);
		_receiverObject = 0;
	}





	//=================================================================================================
	template <class TObj, class TArgs>
	Delegate<TObj, TArgs, false>::Delegate( void )
		: AbstractPriorityDelegate<TArgs>()
	{}

	//=================================================================================================
	template <class TObj, class TArgs>
	Delegate<TObj, TArgs, false>::Delegate( TObj* obj, NotifyMethod method, int32_t prio )
		: AbstractPriorityDelegate<TArgs>(prio)

		, _receiverObject		( obj )
		, _receiverMethod		( method )
	{}

	//=================================================================================================
	template <class TObj, class TArgs>
	Delegate<TObj, TArgs, false>::Delegate( const Delegate<TObj, TArgs, false> & p_delegate )
		: AbstractPriorityDelegate<TArgs>( p_delegate )

		, _receiverObject(p_delegate._receiverObject)
		, _receiverMethod(p_delegate._receiverMethod)
	{}

	//=================================================================================================
	template <class TObj, class TArgs>
	Delegate<TObj, TArgs, false> & Delegate<TObj, TArgs, false>::operator=( const Delegate<TObj, TArgs, false> & p_delegate )
	{
		if( &p_delegate != this )
		{
			::_pTarget        = p_delegate._pTarget;
			_receiverObject = p_delegate._receiverObject;
			_receiverMethod = p_delegate._receiverMethod;
			::_priority       = p_delegate._priority;
		}
		return *this;
	}

	//=================================================================================================
	template <class TObj, class TArgs>
	Delegate<TObj, TArgs, false>::~Delegate( void )
	{}

	//=================================================================================================
	template <class TObj, class TArgs>
	bool Delegate<TObj, TArgs, false>::notify( const void * sender, TArgs & arguments )
	{
		std::lock_guard<aaa::MUTEX> guard(_mutex);
		if( _receiverObject)
		{
			if(	!(_receiverObject->*_receiverMethod)(arguments) )
			{
				return true;
			}
#if AAA_DEBUG()
			else
				debug_break( "Receiver class does not hold the target event method !!!" );
#endif
		}

		return false;
	}

	//=================================================================================================
	template <class TObj, class TArgs>
	bool Delegate<TObj, TArgs, false>::equals( const AbstractDelegate<TArgs> & other ) const
	{
		const Delegate* pOtherDelegate = dynamic_cast<const Delegate*>( other.unwrap() );
		return ( pOtherDelegate 
			&& ::priority() == pOtherDelegate->priority() 
			&& _receiverObject == pOtherDelegate->_receiverObject 
			&& _receiverMethod == pOtherDelegate->_receiverMethod );
	}

	//=================================================================================================
	template <class TObj, class TArgs>
	 AbstractDelegate<TArgs>* Delegate<TObj, TArgs, false>::clone( void ) const
	{
		return new Delegate<TObj, TArgs, false>(*this);
	}

	//=================================================================================================
	template <class TObj, class TArgs>
	void Delegate<TObj, TArgs, false>::disable( void )
	{
		std::lock_guard<aaa::MUTEX> guard(_mutex);
		_receiverObject = 0;
	}




	//=================================================================================================
	template <class TObj>
	Delegate<TObj, void, true>::Delegate( void )
		: AbstractPriorityDelegate<void>()
	{}

	//=================================================================================================
	template <class TObj>
	Delegate<TObj, void, true>::Delegate( TObj* obj, NotifyMethod method, int32_t prio )
		: AbstractPriorityDelegate<void>( prio )

		, _receiverObject		( obj )
		, _receiverMethod		( method )
	{}

	//=================================================================================================
	template <class TObj>
	Delegate<TObj, void, true>::Delegate( const Delegate<TObj, void, true> & p_delegate )
		: AbstractPriorityDelegate<void>( p_delegate )

		, _receiverObject	( p_delegate._receiverObject )
		, _receiverMethod	( p_delegate._receiverMethod )
	{}

	//=================================================================================================
	template <class TObj>
	Delegate<TObj, void, true> & Delegate<TObj, void, true>::operator=( const Delegate<TObj, void, true> & p_delegate )
	{
		if( &p_delegate != this )
		{
			::_pTarget        = p_delegate._pTarget;
			_receiverObject = p_delegate._receiverObject;
			_receiverMethod = p_delegate._receiverMethod;
			_priority       = p_delegate._priority;
		}
		return *this;
	}

	//=================================================================================================
	template <class TObj>
	Delegate<TObj, void, true>::~Delegate(void )
	{}

	//=================================================================================================
	template <class TObj>
	bool Delegate<TObj, void, true>::notify( const void * sender )
	{
		std::lock_guard<aaa::MUTEX> guard(_mutex);
		if( _receiverObject )
		{
			if(	!(_receiverObject->*_receiverMethod)(sender))
			{
				return true;
			}
#if AAA_DEBUG()
			else
				debug_break( "Receiver class does not hold the target event method !!!" );
#endif
		}

		return false;
	}

	//=================================================================================================
	template <class TObj>
	bool Delegate<TObj, void, true>::equals( const AbstractDelegate<void> & other ) const
	{
		const Delegate* pOtherDelegate = dynamic_cast<const Delegate*>( other.unwrap() );
		return ( pOtherDelegate 
			&& priority() == pOtherDelegate->priority() 
			&& _receiverObject == pOtherDelegate->_receiverObject 
			&& _receiverMethod == pOtherDelegate->_receiverMethod );
	}

	//=================================================================================================
	template <class TObj>
 AbstractDelegate<void>* Delegate<TObj, void, true>::clone( void ) const
	{
		return new Delegate<TObj, void, true>(*this);
	}

	//=================================================================================================
	template <class TObj>
	void Delegate<TObj, void, true>::disable( void )
	{
		std::lock_guard<aaa::MUTEX> guard(_mutex);
		_receiverObject = 0;
	}




	//=================================================================================================
	template <class TObj>
	Delegate<TObj, void, false>::Delegate( void )
	{}

	//=================================================================================================
	template <class TObj>
	Delegate<TObj, void, false>::Delegate( TObj* obj, NotifyMethod method, int32_t prio )
		: AbstractPriorityDelegate<void>( prio )

		, _receiverObject	( obj )
		, _receiverMethod	( method )
	{}

	//=================================================================================================
	template <class TObj>
	Delegate<TObj, void, false>::Delegate( const Delegate<TObj, void, false> & p_delegate )
		: AbstractPriorityDelegate<void>( p_delegate )

		, _receiverObject( p_delegate._receiverObject )
		, _receiverMethod( p_delegate._receiverMethod )
	{}

	//=================================================================================================
	template <class TObj>
	Delegate<TObj, void, false> & Delegate<TObj, void, false>::operator=( const Delegate<TObj, void, false> & p_delegate )
	{
		if( &p_delegate != this )
		{
			::_pTarget        = p_delegate._pTarget;
			_receiverObject = p_delegate._receiverObject;
			_receiverMethod = p_delegate._receiverMethod;
			_priority       = p_delegate._priority;
		}
		return *this;
	}


	//=================================================================================================
	template <class TObj>
	Delegate<TObj, void, false>::~Delegate( void )
	{}


	//=================================================================================================
	template <class TObj>
	bool Delegate<TObj, void, false>::notify( const void * sender )
	{
		std::lock_guard<aaa::MUTEX> guard(_mutex);
		if( _receiverObject )
		{
			if(	!(_receiverObject->*_receiverMethod)())
			{
				return true;
			}
#if AAA_DEBUG()
			else 
				debug_break( "Receiver class does not hold the target event method !!!" );
#endif
		}
		return false;
	}


	//=================================================================================================
	template <class TObj>
	bool Delegate<TObj, void, false>::equals( const AbstractDelegate<void> & other ) const
	{
		const Delegate<TObj, void, false>* pOtherDelegate = dynamic_cast<const Delegate<TObj, void, false>*>(other.unwrap());
		return ( pOtherDelegate 
			&& priority() == pOtherDelegate->priority() 
			&& _receiverObject == pOtherDelegate->_receiverObject 
			&& _receiverMethod == pOtherDelegate->_receiverMethod );
	}


	//=================================================================================================
	template <class TObj>
	AbstractDelegate<void> * Delegate< TObj, void, false >::clone( void ) const
	{
//		return new Delegate< TObj, void, false >::Delegate( *this );
		return new Delegate< TObj, void, false >( *this );
	}


	//=================================================================================================
	template <class TObj>
	void Delegate<TObj, void, false>::disable( void )
	{
		std::lock_guard<aaa::MUTEX> guard(_mutex);
		_receiverObject = 0;
	}
} // namespace evt

