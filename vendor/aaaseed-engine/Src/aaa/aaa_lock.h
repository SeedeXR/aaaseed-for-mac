
#ifdef AAA_AAA_LOCK_H
#error "AAA_LOCK_H included more than once."
#endif
#define AAA_AAA_LOCK_H 1


#if !defined(AAA_AAA_MUTEX_H)
#	include "aaa/aaa_mutex.h"
#endif

#if !defined(AAA_AAA_LOCK_GUARD_H)
#	include <aaa/aaa_lock_guard.h>
#endif


namespace aaa {

	//-------------------------------------------------------------------------

	typedef aaa::MUTEX						mutex_t;
	typedef aaa::lock_guard<aaa::mutex_t>	lock_guard_t;
	typedef aaa::unique_lock<aaa::mutex_t>	unique_lock_t;

	//-------------------------------------------------------------------------

} // namespace aaa
