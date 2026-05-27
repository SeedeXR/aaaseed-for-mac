#include "aaa_file_master.h"
#include "infrastructure/param/param_declare.h"


FACTORY_CREATE_V1( c_file_master, file_master, File Master, file_master );

namespace n_file_master
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 2;
	CONSTEXPR INT32	GROUP_NB		= 0;
	CONSTEXPR INT32	PARAM_NB_MAX	= BASE_PARAM_NB
										+ GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(			use_filesystem	)
		PARAM_DEF_BOOL_OFF(			file_fast		)
	};
}

void c_file_master::param_init_pt_static()
{
	INT32	h = 0;

	param_set_pt( h, c_file::b_use_filesystem );
	param_set_pt( h, c_file::get_read_fast_pt() );

	err_param_init_pt(h);
}


CONSTRUCTOR_CREATE(c_file_master)
{
	param_init_with( n_file_master::param, n_file_master::PARAM_NB_MAX );
}

EMPTY_DESTRUCTOR(c_file_master)

