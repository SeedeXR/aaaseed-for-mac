// tests/unit/aaalua_layer_app_shim/Thread/aaa_thread.h
//
// Tests-private SHADOW for vendor/aaaseed-engine/Src/code_utils/Thread/aaa_thread.h.
// Used ONLY by the aaaseed_layer_app_compile_tests OBJECT-lib target ; added
// BEFORE the engine include path so this file wins for the compile-only build.
//
// Why a shadow : the real header pulls Win32 HANDLE + the THREAD_PRIORITY_*
// MSVC constants and types (P_THREAD_EX_FN, etc.) directly into the c_thread
// class body. Pulling on Mac fires 14 unknown-identifier errors. app.cpp
// itself does NOT call c_thread methods ; the type is pulled transitively
// from image/bind_img_base.h (which subclasses c_thread for an async img
// loader). For app.cpp's compile-only build we need a class declaration
// that satisfies the bind_img_base.h surface (`c_thread( const char* )`,
// `get_arg`, `set_can_run`, `is_can_run`, `sleep`, `create`, `start`,
// `shutdown`, `join`) without the Win32 baggage. Faked-class doctrine
// (c108/c112) : declare only what's needed, no member subobjects that
// would force layout matching at link.
//
// CAUTION : this shim cannot be used in a target that LINKS aaa_thread.cpp
// (the symbol mangling matches by qualified name+sig, but member offsets
// would mismatch). aaaseed_layer_app_compile_tests is OBJECT-only, so
// link never happens. Compile-time symbol use only.

#ifdef AAA_AAA_THREAD_H
#error "AAA_THREAD_H included more than once."
#endif
#define AAA_AAA_THREAD_H 1


#ifndef AAA_AAA_MUTEX_H
#	include "aaa/aaa_mutex.h"
#endif
#ifndef	AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#ifndef AAA_ERR_H
#	include "err.h"
#endif

#include <atomic>
#include <cstdint>

//	Posix-side function pointer type (mirrors the vendor #else branch).
typedef void (*P_THREAD_FN) (void *data);

//	c_thread faked-class declaration. NO Win32 HANDLE, NO THREAD_PRIORITY_*
//	enum values. Just enough surface for bind_img_base.h's inline
//	subclassing pattern to type-check.
class	c_thread
{
private:
	//	No real fields needed for compile-only ; faked-class doctrine.
protected:
			void		sleep(		INT32 CONST msec );
public:
			c_thread( C_PCHAR_C name );
	virtual	~c_thread();

			bool	create( unsigned int CONST stackSize = 0 );
			bool	create( void* CONST args, unsigned int CONST stackSize = 0 );
			void	start();
			void	join();

			void	resume();
			void	suspend();
			void	shutdown();
			void	terminate();

			enum thread_priority : INT32
			{
				TimeCritical	= 0,
				Highest			= 1,
				AboveNormal		= 2,
				Normal			= 3,
				BelowNormal		= 4,
				Lowest			= 5,
				Idle			= 6
			};
			bool	set_priority( thread_priority CONST priority );

protected:
			void*	get_arg() CONST;
			void	set_arg(		void* CONST a );
			bool	is_can_run();
			void	set_can_run(	bool CONST b );

public:
	virtual	void	run() = 0;

	template< class T, int SLEEP_TIME >
	void	run_it()
	{
		T*	l = (T*) get_arg();
		if( l )
		{
			set_can_run( true );
			while( is_can_run() )
			{
				l->update_async();
				sleep( SLEEP_TIME );
			}
		}
	}
};
