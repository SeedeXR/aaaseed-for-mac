
#ifdef AAA_COMPUTE_PARALLEL_H
#error "COMPUTE_PARALLEL_H included more than once."
#endif
#define AAA_COMPUTE_PARALLEL_H 1


#define	AAA_LIB_USE_TBB() 0

#ifndef AAA_SPY_H
#	include "spy.h"
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif


#if AAA_LIB_USE_TBB()
// Intel TBB library
#	include		<Tbb/task_scheduler_init.h>
#	include		<Tbb/parallel_for.h>
//#	include		<Tbb/blocked_range.h>
//#	include		<blocked_range2d.h>
#	define	PARALLEL_LIB		tbb
#else
#	ifndef _PPL_H
#		include		<ppl.h>
#	endif
#	define	PARALLEL_LIB	concurrency
#endif


namespace aaa
{
	namespace parallel
	{
		extern bool		b_use_ui;
		extern UINT32	thread_nb_def;
		extern FP32		cpu_core_logical_to_thread_nb_factor_ui;
#if AAA_LIB_USE_TBB()
		extern o_str	tbb_version;
		extern UINT32	tbb_thread_nb_def;
#endif

		FINLINE	bool	is_use()			{	return b_use_ui;		}
		FINLINE	UINT32	get_thread_nb_def()	{	return thread_nb_def;	}
		extern o_str	lib_name;
		void			init();

		template< typename FUNCTION >
		FINLINE void call( INT32 nb, FUNCTION&& fn) NOEXCEPT
		{
			SPY_PUSH_RANGE_FUNCTION( spy::IMG_LOW );
				if( aaa::parallel::is_use() )
					PARALLEL_LIB::parallel_for( 0,nb, [&](INT32 j) NOEXCEPT { fn(j); } );
				else
					for( INT32 j = 0; j < nb; ++j )
						fn(j);
			SPY_POP_RANGE();
		}
	}
}
