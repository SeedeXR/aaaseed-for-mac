#include "bdd_watchdog.h"
#include "time/aaa_time.h"

#if !AAA_WIN64()
#	include "quancom/qlib.h"
#	include "lib_wrappers/wrap_qlib.h"
#endif



#define	WATCHDOG_HEADER  "# WATCHDOG "

bool	c_bdd_watchdog::b_dll_loaded = false;

FACTORY_CREATE_PROP_V1( c_bdd_watchdog, bdd_watchdog, Watchdog, bdd_watchdog, sub_menu="Special"; );

namespace	n_bdd_watchdog
{
	CONSTEXPR UINT8	BASE_PARAM_NB	= 4 + c_bdd::NO_GEO_PARAM_NB;
	CONSTEXPR UINT8 GROUP_PARAM_NB	= 0;
	CONSTEXPR UINT32 PARAM_NB	=	BASE_PARAM_NB
								+	GROUP_PARAM_NB;
			
	CONST c_param_def param[PARAM_NB] =
	{
		BDD_NO_GEO_BASE_PARAMS
		PARAM_DEF_BOOL_OFF(		active				)
		PARAM_DEF_REAL_POS_ONE(	timeout				)
		PARAM_DEF_BOOL_OFF(		verbose			)
		PARAM_DEF_REAL_POS(		startup_delay_from,	0, 16 )
	};
}


void	c_bdd_watchdog::param_init_pt()
{
	INT32	h = param_init_pt_no_geo();

	param_set_pt( h, get_pt_active()		);
	param_set_pt( h, _timeout				);
	param_set_pt( h, _b_verbose				);
	param_set_pt( h, _startup_delay_from	);
	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bdd_watchdog )
{
	_b_init = false;

	param_init_with( n_bdd_watchdog::param, n_bdd_watchdog::PARAM_NB );
}

c_bdd_watchdog::~c_bdd_watchdog()
{
	if( cur == this )
		cur = nullptr;
	close();
}

#if !AAA_WIN64()
void	c_bdd_watchdog::close()
{
	IF_THIS_NULL_RETURN();

	if( _b_init )
	{
		dll_qlib.QAPIExtWatchdog( _handle, JOB_DISABLE_WATCHDOG );
		HEADER_PRINT_STRING( WATCHDOG_HEADER, "Watchdog card disabled." );

		dll_qlib.QAPIExtCloseCard( _handle );
		HEADER_PRINT_STRING( WATCHDOG_HEADER, "Watchdog card closed." );
		_b_init = false;
	}
	if( b_dll_loaded )
		dll_qlib.deinit();
		//wrap_qlib_Term();
	b_dll_loaded = false;
	cur = nullptr;	//todo a little short lib should be done in static
}

bool	c_bdd_watchdog::open_dll()
{
	if( !b_dll_loaded )
		b_dll_loaded = dll_qlib.init();
	//	b_dll_loaded = wrap_qlib_Init() == 0;
	return b_dll_loaded;
}

static	bool	b_try_init = true;

//todo init is a start so name it or split it
void	c_bdd_watchdog::init_low()
{
	if( b_try_init && aaa::time::get_real_time_from_start() > _startup_delay_from )
	{
		_handle = dll_qlib.QAPIExtOpenCard( PWDOG2N, 0 );
		if ( _handle )
		{
			HEADER_PRINT_STRING( WATCHDOG_HEADER, "Watchdog card PWDOG2N opened." );
		}
		else
		{	//should be red an error !!!!!
			HEADER_PRINT_STRING( WATCHDOG_HEADER, "Watchdog card PWDOG2N not found." );
			b_try_init = false;
			return;
		}
		_b_init = true;
		dll_qlib.QAPIExtWatchdog( _handle, JOB_DISABLE_WATCHDOG );
		dll_qlib.QAPIExtWatchdog( _handle, JOB_ENABLE_WATCHDOG );
		HEADER_PRINT_STRING( WATCHDOG_HEADER, "Watchdog card enabled." );
	}
}
#endif

void	c_bdd_watchdog::update()
{
#if !AAA_WIN64()
	if( _b_active )
	{
		if( !_b_init )
		{
			// try to load dll once
			if( !b_dll_loaded )
				b_dll_loaded = dll_qlib.init();
				//b_dll_loaded = wrap_qlib_Init();
			if( b_dll_loaded )
				init_low();
		}
		else
		{
			dll_qlib.QAPIExtWatchdog( _handle, JOB_RETRIGGER_WATCHDOG );
			if( _b_verbose )
			{
				HEADER_PRINT_STRING( WATCHDOG_HEADER, "Watchdog card retriggered." );
			}
		}
	}
#endif
	cur = this;
}


c_bdd_watchdog*	c_bdd_watchdog::cur				= nullptr;
//REAL			c_bdd_watchdog::_startup_time	= 0.;


void	c_bdd_watchdog::c_init()
{
//	_startup_time = aaa::time::get();
}

void	c_bdd_watchdog::c_deinit()
{
	cur->close();	//todo cur means only one we should limit to one
}
