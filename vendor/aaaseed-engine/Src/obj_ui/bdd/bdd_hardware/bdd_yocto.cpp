
#include "bdd_yocto.h"
#define AAA_LIB_USE_YOCTOLIB() 0
#if AAA_LIB_USE_YOCTOLIB()
#	include "yoctolib/yocto_digitalio.h"
#	ifndef AAA_LIB_USE_H
#		include <lib_use.h>
#	endif
	AAA_LIB_USE_MESSAGE( "Setupapi" )
#endif


FACTORY_CREATE_PROP_V1( c_bdd_yocto, bdd_yocto, Yocto Puce, bdd_yocto, sub_menu = "Hardware"; );

namespace n_bdd_yocto
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 5 + c_bdd::NO_GEO_PARAM_NB;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	= BASE_PARAM_NB
									+ GROUP_PARAM_NB;

	CONST c_param_def param[ PARAM_NB_MAX ] =
	{
		BDD_NO_GEO_BASE_PARAMS
		PARAM_DEF_BOOL_OFF(		active				)
		PARAM_DEF_BOOL_OFF(		open				)
		PARAM_DEF_BOOL_OFF(		verbose				)
		PARAM_DEF_REAL_POS_ONE(	polling_interval	)
		PARAM_DEF_INT32_LOCKED(	input				)
	};
}

void	c_bdd_yocto::param_init_pt()
{
	INT32	h = param_init_pt_no_geo();

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _b_open_ui );
	param_set_pt( h, _b_verbose_ui );
	param_set_pt( h, _polling_interval_ui );
	param_set_pt( h, _input );

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bdd_yocto )
,_b_open		{false}
{
	param_init_with( n_bdd_yocto::param, n_bdd_yocto::PARAM_NB_MAX );
}

c_bdd_yocto::~c_bdd_yocto()
{
	close();
}

void	c_bdd_yocto::open()
{
#if AAA_LIB_USE_YOCTOLIB()
	std::string errmsg;
	YAPI::DisableExceptions();
	std::string version;
	std::string date;
	YapiWrapper::getAPIVersion( version, date );
	GOOD_PRINT_STRING( "Using Yocto API %s %s", version.c_str(), date.c_str() );

	if( YAPI::RegisterHub( "usb", errmsg ) != YAPI::SUCCESS )
	{
		ERR_PRINT_STRING( "Yocto Lib error RegisterHub : %s", errmsg.c_str() );
		goto exit;
	}

	io = YDigitalIO::FirstDigitalIO();
	if( io == nullptr )
	{
		ERR_PRINT_STRING( "Yocto Lib error FirstDigitalIO" );
		goto exit;
	}
	if( io->isOnline() == false )
	{
		ERR_PRINT_STRING( "Yocto Lib error isOnline" );
		goto exit;
	}
	if( io->setPortDirection( 0b0000 ) < 0 )
	{
		ERR_PRINT_STRING( "Yocto Lib error setPortDirection" );
		goto exit;
	}
	if( io->setPortOpenDrain( 0b1111 ) < 0 )
	{
		ERR_PRINT_STRING( "Yocto Lib error setPortOpenDrain" );
		goto exit;
	}
	if( io->setPortPolarity( 0b1111 ) < 0 )
	{
		ERR_PRINT_STRING( "Yocto Lib error setPortPolarity" );
		goto exit;
	}
	_b_open = true;
	return;
exit:
	close();
#endif
}

void	c_bdd_yocto::close()
{
	_b_open = false;
	_input = 0;
#if AAA_LIB_USE_YOCTOLIB()
	YAPI::FreeAPI();
#endif
}

void	c_bdd_yocto::update()
{
#if AAA_LIB_USE_YOCTOLIB()
	if( is_active() )
	{
		if( _b_open != _b_open_ui )
		{
			if( _b_open_ui )
				open();
			else
				close();
		}
		if( _b_open )
		{
			if( _delta_t.update() )
				_check_time = 0.;
			_check_time += _delta_t.get_dt();
			if( _check_time > _polling_interval_ui )
			{
				_check_time = 0.;
				if( io->isOnline() == true )
				{
					_input = io->get_portState();
					if( _input != Y_PORTSTATE_INVALID )
					{
						if( _b_verbose_ui )
							GOOD_PRINT_STRING( "Yocto Puce input : %d", _input );
					}
					else
					{
						ERR_PRINT_STRING( "Yocto Lib error get_portState" );
					}
				}
			}
		}
	}
#endif
}

void	c_bdd_yocto::draw()
{
}

