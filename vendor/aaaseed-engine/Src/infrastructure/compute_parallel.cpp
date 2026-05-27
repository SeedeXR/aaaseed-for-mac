#include "compute_parallel.h"

#if AAA_LIB_USE_TBB()
#include <lib_use.h>
#	if	AAA_DEBUG()
		AAA_LIB_USE( "Tbb/tbb_debug" )
#	else
		AAA_LIB_USE( "Tbb/tbb" )
#	endif
#endif

namespace aaa
{
	namespace parallel
	{
		bool			b_use_ui = true;
		o_str			lib_name;
		UINT32			thread_nb_def = 8;
		FP32			cpu_core_logical_to_thread_nb_factor_ui = 1.;
#if AAA_LIB_USE_TBB()
		o_str			tbb_version;
		UINT32			tbb_thread_nb_def;
#endif

		void init()
		{
#if AAA_LIB_USE_TBB()
			tbb_version.set(	tbb::TBB_runtime_interface_version() );
			tbb_thread_nb_def	= tbb::task_scheduler_init::default_num_threads();
			lib_name			= "tbb" ;
#else
//			tbb_thread_nb_def	= 0;
			lib_name			= "concurrency";
#endif
		}
	}
}
