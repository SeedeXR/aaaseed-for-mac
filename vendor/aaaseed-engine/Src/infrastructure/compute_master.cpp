
#include "infrastructure/compute_master.h"
#include "infrastructure/compute_parallel.h"
#include "infrastructure/param/param_declare.h"
#include "cpu.h"

FACTORY_CREATE_V1( c_compute_master, compute_master, Compute Master, compute_master );

namespace	n_compute_master
{
#if AAA_LIB_USE_TBB()
	CONSTEXPR INT32	BASE_PARAM_NB	= 8;
#else
	CONSTEXPR INT32	BASE_PARAM_NB	= 6;
#endif
	CONSTEXPR INT32	GROUP_NB		= 0;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_INT32(			compute_task_max_nb,				2, 4,	1, PARAM_MAX_INT32	)
		PARAM_DEF_INT32_LOCKED(		compute_task_nb						)
		PARAM_DEF_BOOL_OFF(			use_parallel						)
		PARAM_DEF_STR_LOCKED(		parallel_library					)
		PARAM_DEF_FP32(				core_logical_to_thread_nb_factor, 	1., .5,		0.,	1024*65	)
		PARAM_DEF_UINT32_LOCKED(	parallel_thread_nb					)
#if AAA_LIB_USE_TBB()
		PARAM_DEF_STR_LOCKED(	tbb_version				)
		PARAM_DEF_INT32_LOCKED(	tbb_thread_nb_default	)
#endif
	};
}

void	c_compute_master::param_init_pt()
{
	INT32	h = 0;
	param_set_pt( h, _compute_task_max_nb_ui			);
	param_set_pt( h, _compute_task_count				);
	param_set_pt( h, aaa::parallel::b_use_ui			);
	param_set_pt( h, aaa::parallel::lib_name			);
	param_set_pt( h, aaa::parallel::cpu_core_logical_to_thread_nb_factor_ui		);
	param_set_pt( h, aaa::parallel::thread_nb_def		);
#if AAA_LIB_USE_TBB()
	param_set_pt( h, aaa::parallel::tbb_version			);
	param_set_pt( h, aaa::parallel::tbb_thread_nb_def	);
#endif

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_compute_master)
{
	aaa::parallel::init();
	param_init_with( n_compute_master::param, n_compute_master::PARAM_NB_MAX );
}
c_compute_master::~c_compute_master()
{
}
void c_compute_master::param_init()
{
//	compute_task_max_nb = 1;
	_compute_task_count = 0;
}

void c_compute_master::update()
{
	aaa::parallel::thread_nb_def = MAX(1, UINT32( aaa::parallel::cpu_core_logical_to_thread_nb_factor_ui * c_cpu::one->get_cpu_core_logical_nb() ) );
}

void c_compute_master::set( INT32 nb )
{
	_compute_task_count = nb;
	TBUF_ADD_OBJ( tbuf::CH_COMPUTE_TASK, REAL(nb), "set", this );
}

void c_compute_master::inc()
{
	bool	b = false;
//	DBG_PRINT_STRING("c_compute_master::inc %d", compute_task_count);
	{
		std::lock_guard<aaa::MUTEX> guard(_access);
		set( _compute_task_count + 1 );
		if( _compute_task_count == _compute_task_max_nb_ui )	//hack compute_task_max_nb_ can be change async
			b = true;
	}
	if( b )
	{
		DBG_PRINT_STRING( "Compute Master max task limit" );
		_compute.lock();
	}
//	DBG_PRINT_STRING("c_compute_master::inc %d", compute_task_count);
}

void c_compute_master::dec()
{
//	bool	b = false;
//	DBG_PRINT_STRING("c_compute_master::dec %d", compute_task_count);
	{
		std::lock_guard<aaa::MUTEX> guard(_access);
		if( _compute_task_count == _compute_task_max_nb_ui )
			_compute.unlock();
		set( _compute_task_count - 1 );
	}
//	DBG_PRINT_STRING("c_compute_master::dec %d", compute_task_count);
}

c_compute_master*	g_compute_master = nullptr;
