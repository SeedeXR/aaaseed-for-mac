
#ifdef AAA_AAA_LOCK_GUARD_H
#error "AAA_LOCK_GUARD_H included more than once."
#endif
#define AAA_AAA_LOCK_GUARD_H 1


#if !defined(_UTILITY_)
#	include <utility>
#endif


namespace aaa {

	//-------------------------------------------------------------------------

	struct adopt_lock_t {};
	struct defer_lock_t {};

	constexpr adopt_lock_t adopt_lock{};
	constexpr defer_lock_t defer_lock{};

	//-------------------------------------------------------------------------

	template <class TMutex>
	class lock_guard final
	{

	private:
		TMutex * _mutex = nullptr;

	public:
		lock_guard(TMutex & p_mutex);
		lock_guard(TMutex & p_mutex, aaa::adopt_lock_t);
		~lock_guard();

	public:
		lock_guard(lock_guard<TMutex> && p_rhs) = delete;
		lock_guard(lock_guard<TMutex> const & p_rhs) = delete;
		lock_guard<TMutex>& operator=(lock_guard<TMutex> && p_rhs) = delete;
		lock_guard<TMutex>& operator=(lock_guard<TMutex> const & p_rhs) = delete;

	}; // class lock_guard

	//-------------------------------------------------------------------------

} // namespace aaa

template<class TMutex>
aaa::lock_guard<TMutex>::lock_guard(TMutex & p_mutex)
	: _mutex(&p_mutex)
{
	_mutex->lock();
}

template<class TMutex>
aaa::lock_guard<TMutex>::lock_guard(TMutex & p_mutex, aaa::adopt_lock_t)
	: _mutex(&p_mutex)
{}

template<class TMutex>
aaa::lock_guard<TMutex>::~lock_guard()
{
	_mutex->unlock();
}

//-----------------------------------------------------------------------------

namespace aaa {

	//-------------------------------------------------------------------------

	template <class TMutex>
	class unique_lock final
	{

	private:
		TMutex *	_mutex = nullptr;
		bool		_b_owns = false;

	public:
		unique_lock();
		unique_lock(TMutex & p_mutex);
		unique_lock(TMutex & p_mutex, aaa::adopt_lock_t);
		unique_lock(TMutex & p_mutex, aaa::defer_lock_t);
		~unique_lock();

	public:
		unique_lock(unique_lock<TMutex> && p_rhs);
		unique_lock(unique_lock<TMutex> const & p_rhs) = delete;
		unique_lock<TMutex>& operator=(unique_lock<TMutex> && p_rhs);
		unique_lock<TMutex>& operator=(unique_lock<TMutex> const & p_rhs) = delete;

	public:
		void swap(unique_lock<TMutex>& p_rhs);

	public:
		void lock();
		void unlock();

	}; // class unique_lock

	//-------------------------------------------------------------------------

} // namespace aaa

template<class TMutex>
aaa::unique_lock<TMutex>::unique_lock()
	: _mutex(nullptr)
	, _b_owns(false)
{}

template<class TMutex>
aaa::unique_lock<TMutex>::unique_lock(TMutex & p_mutex)
	: _mutex(&p_mutex)
	, _b_owns(false)
{
	lock();
}

template<class TMutex>
aaa::unique_lock<TMutex>::unique_lock(TMutex & p_mutex, aaa::adopt_lock_t)
	: _mutex(&p_mutex)
	, _b_owns(true)
{}

template<class TMutex>
aaa::unique_lock<TMutex>::unique_lock(TMutex & p_mutex, aaa::defer_lock_t)
	: _mutex(&p_mutex)
	, _b_owns(false)
{}

template<class TMutex>
aaa::unique_lock<TMutex>::~unique_lock()
{
	if( _b_owns )
		_mutex->unlock();
}

template<class TMutex>
aaa::unique_lock<TMutex>::unique_lock(aaa::unique_lock<TMutex> && p_rhs)
	: _mutex(p_rhs._mutex)
	, _b_owns(p_rhs._b_owns)
{
	p_rhs._mutex = nullptr;
	p_rhs._b_owns  = false;
}

template<class TMutex>
aaa::unique_lock<TMutex> & aaa::unique_lock<TMutex>::operator=(aaa::unique_lock<TMutex> && p_rhs)
{
	if( _b_owns )
		unlock();

	aaa::unique_lock<TMutex>(std::move(p_rhs)).swap(*this);

	p_rhs._mutex = nullptr;
	p_rhs._b_owns  = false;

	return *this;
}

template<class TMutex>
void aaa::unique_lock<TMutex>::swap(aaa::unique_lock<TMutex> & p_rhs)
{
	std::swap(_mutex, p_rhs._mutex);
	std::swap(_b_owns, p_rhs._b_owns);
}

namespace std
{
	template<class TMutex>
	inline void swap(
		aaa::unique_lock<TMutex> & p_lhs,
		aaa::unique_lock<TMutex> & p_rhs)

	{
		p_lhs.swap(p_rhs);
	}

} // namespace std

template<class TMutex>
void aaa::unique_lock<TMutex>::lock()
{
	if( !_mutex )
	{
		debug_break( "%s() null mutex", __FUNCTION__ );
		return;
	}
	_mutex->lock();
	_b_owns = true;
}

template <class TMutex>
void aaa::unique_lock<TMutex>::unlock()
{
	_b_owns = false;
	if( !_mutex )
	{
		debug_break( "%s() null mutex", __FUNCTION__ );
		return;
	}
	_mutex->unlock();
}
