#include "bdd_touchco.h"
#if AAA_LIB_USE_TOUCHCO()
#	include "tracker/TouchCo/TC.h"
#endif
#include "image/img.h"
#include "image/bind_img_2d.h"
#include "obj_ui/tracker/trackers.h"

#if AAA_LIB_USE_TOUCHCO()
C_PCHAR_C	touchco_get_error_str( TC_STATUS err )
{
	C_PCHAR	s;
	switch( err )
	{
	case TC_OK:						s = "TC_OK";					break;
	case TC_DEVICE_NOT_FOUND:		s = "TC_DEVICE_NOT_FOUND";		break;
	case TC_DEVICE_NOT_OPENED:		s = "TC_DEVICE_NOT_OPENED";		break;
	case TC_DEVICE_ALREADY_OPENED:	s = "TC_DEVICE_ALREADY_OPENED";	break;
	case TC_DEVICE_TIMEOUT:			s = "TC_DEVICE_TIMEOUT";		break;
	case TC_OPEN_PORT_ERROR:		s = "TC_OPEN_PORT_ERROR";		break;
	case TC_IO_ERROR:				s = "TC_IO_ERROR";				break;
	case TC_ERROR:					s = "TC_ERROR";					break;
	default:						s = "UNKNOWN TOUCHCO ERROR";	break;
	}
	return s;
}
FINLINE	bool	TC_FAILED(TC_STATUS sta) { return !TC_SUCCESS(sta); }
#endif	//#if AAA_LIB_USE_TOUCHCO()

static	bool		b_verbose = false;
		CHAR		TC_PRINT_HEADER[] = "# TOUCHCO : ";

bool	c_bdd_touchco::b_dll_loaded = false;

FACTORY_CREATE_PROP_V1( c_bdd_touchco, bdd_touchco, Touch CO, bdd_touchco, sub_menu="Hardware"; );

namespace n_bdd_touch
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 9 + c_bdd::NO_GEO_PARAM_NB;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 0;	
	CONSTEXPR INT32 PARAM_NB	= BASE_PARAM_NB
								+ GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB] =
	{
		BDD_NO_GEO_BASE_PARAMS
		PARAM_DEF_BOOL_OFF(			active		)
		PARAM_DEF_BOOL_OFF(			verbose		)

		PARAM_DEF_INT32_POS_ZERO(	device		)
		PARAM_DEF_BOOL_OFF(			close_trig	)
		PARAM_DEF_BOOL_OFF(			open_trig	)
		PARAM_DEF_BOOL_LOCKED(		opened		)

		PARAM_DEF_REAL_POS(			min,		1, 0 )
		PARAM_DEF_REAL_POS(			max,		100, 2500 )
		PARAM_DEF_BOOL_OFF(			show_min_max )
	};
}

void	c_bdd_touchco::param_init_pt()
{
	INT32	h = param_init_pt_no_geo();

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, b_verbose );

	param_set_pt( h, _device_index );
	param_set_pt( h, _b_close_trig );
	param_set_pt( h, _b_open_trig );
	param_set_pt( h, _b_opened );

	param_set_pt( h, _min );
	param_set_pt( h, _max );
	param_set_pt( h, _b_min_max_show );

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bdd_touchco )
	,_b_opened		( false )
	,_b_first		( true )
#if !AAA_WIN64()
	,_tc_handle		( NULL )
#endif
{
	param_init_with( n_bdd_touch::param, n_bdd_touch::PARAM_NB );
#if AAA_LIB_USE_TOUCHCO()
	if( !b_dll_loaded )
	{
		//	b_dll_loaded = wrap_touchco_Init() == 0;
		b_dll_loaded = dll_touchco.init();
	}
	enumerate();
#endif	//#if AAA_LIB_USE_TOUCHCO()
}

c_bdd_touchco::~c_bdd_touchco()
{
#if AAA_LIB_USE_TOUCHCO()
	close();
	//todo we assume only one touchco, add a lib_open_count approach to handle multiple toucho bdd...
	//if( b_dll_loaded )	wrap_touchco_Term();
	if( b_dll_loaded )
		dll_touchco.deinit();
#endif
}

#if AAA_LIB_USE_TOUCHCO()
INT32	c_bdd_touchco::enumerate()
{
	if( !b_dll_loaded )
	{
		//b_dll_loaded = wrap_touchco_Init() == 0;
		b_dll_loaded = dll_touchco.init();
	}
	unsigned int	device_nb = 0;	// Number of devices detected

	if( b_dll_loaded )
	{
		// Scan for available devices
		TC_STATUS		tc_status = dll_touchco.TC_GetNumDevices( &device_nb );

		if( TC_FAILED(tc_status) )
		{
			if( tc_status != TC_DEVICE_NOT_FOUND )
			{
				ERR_PRINT_STRING( "TouchCo Error : %s() %s", __FUNCTION__, touchco_get_error_str(tc_status) );
			}
		}
		trackers::PRINT_STRING( TC_PRINT_HEADER, "found %d device", device_nb );
	}
	return device_nb;
}

AAA_ERR	c_bdd_touchco::open()
{
	if( _b_opened )
		return AAA_OK;

	enumerate();
	if( !b_dll_loaded )
		return ERR_ANY;

	// Open device by index
	TC_STATUS	tc_status = dll_touchco.TC_OpenByIndex( &_tc_handle, _device_index );
	if( TC_FAILED(tc_status) )
	{
		ERR_PRINT_STRING( "TouchCo Error : %s() can't open device %d : %s ", __FUNCTION__, _device_index, touchco_get_error_str(tc_status) );
		return ERR_ANY;
	}
	trackers::PRINT_STRING( TC_PRINT_HEADER, "Opened device %d", _device_index );

	//todo
	// OR Open device by serial number
	/*if ((tcStatus = TC_OpenBySerialNumber(&handle, serialNumber)) != TC_OK )
	return tcStatus;*/
	_b_opened = true;

	// Initialize force image to the proper size for device
	tc_status = dll_touchco.TC_InitForceFrame( _tc_handle, &_tc_frame_force );
	if( TC_FAILED(tc_status) )
	{
		ERR_PRINT_STRING( "TouchCo Error : %s() can't TC_InitForceFrame : %s ", __FUNCTION__, touchco_get_error_str(tc_status) );
		return ERR_ANY;
	}

	// Initialize contact array to proper size for device
	tc_status = dll_touchco.TC_InitContactFrame( _tc_handle, &_tc_frame_contact );
	if( TC_FAILED(tc_status) )
	{
		ERR_PRINT_STRING( "TouchCo Error : %s() can't TC_InitContactFrame : %s ", __FUNCTION__, touchco_get_error_str(tc_status) );
		return ERR_ANY;
	}

	// Retrieve sensor information
	tc_status = dll_touchco.TC_GetSensorInfo( _tc_handle, &_tc_sensor_info );
	if( TC_FAILED(tc_status) )
	{
		ERR_PRINT_STRING( "TouchCo Error : %s() can't TC_GetSensorInfo : %s ", __FUNCTION__, touchco_get_error_str(tc_status) );
		return ERR_ANY;
	}

	trackers::PRINT_STRING( TC_PRINT_HEADER, "sensitive pixel %d by %d",		_tc_sensor_info.cols, _tc_sensor_info.rows		);
	trackers::PRINT_STRING( TC_PRINT_HEADER, "max contacts %d",				_tc_sensor_info.maxContacts						);
	trackers::PRINT_STRING( TC_PRINT_HEADER, "size %.1f by %.1f milliliters",	_tc_sensor_info.width, _tc_sensor_info.height	);
	trackers::PRINT_STRING( TC_PRINT_HEADER, "model %s",						_tc_sensor_info.model							);
	trackers::PRINT_STRING( TC_PRINT_HEADER, "firmware %s",					_tc_sensor_info.firmwareVersion					);
	trackers::PRINT_STRING( TC_PRINT_HEADER, "serial number %s",				_tc_sensor_info.serialNumber					);

	// Enable Peak Separation contact detection
	if ((tc_status = dll_touchco.TC_SetOption( _tc_handle, TC_PEAK_SEPARATION, TC_ENABLED)) != TC_OK)
	{
		return ERR_ANY;
	}

	return AAA_OK;
}

void	c_bdd_touchco::close()
{
	if( _b_opened && _tc_handle )
	{
		// Close open handle
		TC_STATUS	tc_status = dll_touchco.TC_Close(_tc_handle);
		trackers::PRINT_STRING( TC_PRINT_HEADER, "device Closed" );
		if( TC_FAILED(tc_status) )
		{
			ERR_PRINT_STRING( "TouchCo Error : %s() error : %s ", __FUNCTION__, touchco_get_error_str(tc_status) );
			return;
		}
		_tc_handle = nullptr;
		_b_opened = false;
	}
}
#endif	//#if AAA_LIB_USE_TOUCHCO()

void	c_bdd_touchco::draw()
{
}

void	c_bdd_touchco::update()
{
#if AAA_LIB_USE_TOUCHCO()
	if( !is_active() )
		return;

	if( _b_first )
	{
		_b_first = false;
		open();
	}
	if( _b_open_trig )
	{	
		open();
		_b_open_trig = false;
	}
	if( _b_close_trig )
	{	
		close();
		_b_close_trig = false;
	}

	if( !_b_opened )
		return;

	// Get latest force image
	TC_STATUS	tc_status = dll_touchco.TC_GetForceFrame( _tc_handle, &_tc_frame_force );
	if( TC_FAILED(tc_status) )
	{
		ERR_PRINT_STRING( "TouchCo Error : TC_GetForceFrame() failed : %s ", touchco_get_error_str(tc_status) );
		return;
	} 

	// Print out force image
	INT32	rows = _tc_sensor_info.rows;
	INT32	cols = _tc_sensor_info.cols;

	c_img_2d*	img = g_bind_img_2d->get_img_cur( aaa::PIXEL_FORMAT::R_8, cols, rows, true, nullptr, __FUNCTION__ );
	if( !img )
		return;

	DOUBLE	_factor = DIV( 255.9, (DOUBLE)(_max - _min) );

	UINT8*	dst = img->get_data_uint8();
	if( !dst )
	{
		ERR_PRINT_STRING( "%s() dst is Null", __FUNCTION__ );
		return;
	}

	INT32	src_index;
	INT32	dst_index;
	REAL	v;
	for( INT32 y = rows; y > 0; --y )
	{
		src_index = (rows-y) * cols;
		dst_index = (y-1) * cols;
		for( INT32 x = cols; x > 0; --x )
		{
			v = _tc_frame_force.forces[ src_index ];
			if( v <= _min )			{	*(dst + dst_index) = 0;		}
			else if ( v >= _max )	{	*(dst + dst_index) = 255;	}
			else					{	*(dst + dst_index) = _b_min_max_show ? 128 : ( v - _min ) * _factor;	}

			//if( _tc_frame_force.forces[ index ] > 0 )
			//{
			//	REAL r = _tc_frame_force.forces[ index ];
			//}
			++src_index;
			++dst_index;
		}
	}
	g_bind_img_2d->do_after_a_compute();
	//was
	//INT32	i = c_layer::get_cur()->get_bind_2d();
	//bind_img::move_to_gpu( i );
#endif	//#if AAA_LIB_USE_TOUCHCO()
}

