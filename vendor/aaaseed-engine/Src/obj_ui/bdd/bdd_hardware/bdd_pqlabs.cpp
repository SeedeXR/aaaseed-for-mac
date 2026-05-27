#include "bdd_pqlabs.h"
#include "obj_ui/bdd/bdd_point/bdd_blob.h"
//#include "tracker/PQLabs/PQMTClient.h"

#include "lib_wrappers/wrap_pqlabs.h"

#ifdef USE_LINKED_PQLABS_LIB
#	include <lib_use.h>
	AAA_LIB_USE( "PQMTClient" )
#endif

#include "obj_ui/tracker/trackers.h"

using namespace PQ_SDK_MultiTouch;

		CHAR		PQLABS_HEADER[] = "# PQLABS : ";

FACTORY_CREATE_PROP_V1( c_bdd_pqlabs, bdd_pqlabs, PQLabs, bdd_pqlabs, sub_menu="Hardware"; );

namespace n_bdd_pqlabs
{
	CONSTEXPR INT32 BASE_PARAM_NB		= 5 + c_bdd::NO_GEO_PARAM_NB;
	CONSTEXPR INT32 TRANSFER_PARAM_NB	=	9;
	CONSTEXPR INT32 INFO_PARAM_NB		=	6;
	CONSTEXPR INT32 SERVER_PARAM_NB		=	2;
	CONSTEXPR INT32 GROUP_PARAM_NB		=	3;
	CONSTEXPR INT32 PARAM_NB	=	BASE_PARAM_NB
								+	TRANSFER_PARAM_NB
								+	SERVER_PARAM_NB
								+	INFO_PARAM_NB
								+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB] =
	{
		BDD_NO_GEO_BASE_PARAMS
		PARAM_DEF_BOOL_OFF(		active )
		PARAM_DEF_BOOL_OFF(		verbose )

		PARAM_DEF_BOOL_OFF(		open_trig )
		PARAM_DEF_BOOL_OFF(		close_trig )
		PARAM_DEF_BOOL_LOCKED(	opened )

		PARAM_DEF_GROUP( Server, SERVER_PARAM_NB )
			PARAM_DEF_REF(			server_ip )
			PARAM_DEF_INT32_POS(	server_port, PQMT_DEFAULT_CLIENT_PORT, PQMT_DEFAULT_CLIENT_PORT )
//			PARAM_DEF_INT32_POS( move_threshold, 0, 3 )

		PARAM_DEF_GROUP( Info, INFO_PARAM_NB )
			PARAM_DEF_STR_LOCKED(		serial )
			PARAM_DEF_INT32_LOCKED_XY(	server_size )
			PARAM_DEF_INT32_LOCKED_XY(	screen_size )
			PARAM_DEF_INT32_LOCKED(		touch_nb )

		PARAM_DEF_GROUP_CLOSED( Transfer, TRANSFER_PARAM_NB )
			PARAM_DEF_REF(			transfer_bdd_target )
			PARAM_DEF_REAL_ONE(		transfer_blob_radius )
			PARAM_DEF_POINT_XYZ(	transfer_center )
			PARAM_DEF_SCALE_XYZF(	transfer_size )
	};
}

void	c_bdd_pqlabs::param_init_pt()
{
	INT32	h = param_init_pt_no_geo();

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _b_verbose );

	param_set_pt( h, _b_open_trig );
	param_set_pt( h, _b_close_trig );
	param_set_pt( h, _b_opened );

	++h;
		param_set_pt( h, _server_ip );
		param_set_pt( h, _server_port );
//		param_set_pt( h, _move_threshold );

	++h;
		param_set_pt( h, _serial );
		param_set_pt( h, _server_size_x );
		param_set_pt( h, _server_size_y );
		param_set_pt( h, _screen_size_x );
		param_set_pt( h, _screen_size_y );
		param_set_pt( h, _touch_nb );

	++h;
		param_set_pt_attach_obj(	h, _target_name_symbo,		_bdd_target );
		param_set_pt(				h, _transfer_blob_radius	);
		param_set_pt_3(			h, _transfer_translate		);
		param_set_pt_4(			h, _transfer_scale_ui		);

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bdd_pqlabs )
	,_b_opened			( false )
	,_bdd_target		( nullptr )
	,_screen_size_x		( 0 )
	,_screen_size_y		( 0 )
	,_server_size_x		( 0 )
	,_server_size_y		( 0 )
	,_touch_nb			( 0 )
	,_b_close_trig		( false )
	,_b_open_trig		( false )
{
	param_init_with( n_bdd_pqlabs::param, n_bdd_pqlabs::PARAM_NB );
}

c_bdd_pqlabs::~c_bdd_pqlabs()
{
	close();
}

void	c_bdd_pqlabs::set_callbacks()
{
	// set the functions on server callback
	PFuncOnReceivePointFrame	old_rf_func			= dll_pqlabs.SetOnReceivePointFrame(	&c_bdd_pqlabs::callback_pointframe,		this	);
	PFuncOnReceiveGesture		old_rg_func			= dll_pqlabs.SetOnReceiveGesture(		&c_bdd_pqlabs::callback_gesture,		this	);
	PFuncOnServerBreak			old_svr_break		= dll_pqlabs.SetOnServerBreak(			&c_bdd_pqlabs::callback_serverbreak,	this	);
	PFuncOnReceiveError			old_rcv_err_func	= dll_pqlabs.SetOnReceiveError(			&c_bdd_pqlabs::callback_error,			this	);
	PFuncOnGetDeviceInfo		old_gdi_func		= dll_pqlabs.SetOnGetDeviceInfo(		&c_bdd_pqlabs::callback_device_info,	this	);
}

AAA_ERR	c_bdd_pqlabs::open()
{
	if( _b_opened )
		return AAA_OK;

	int err_code = PQMTE_SUCCESS;

	set_callbacks();

	// connect server
	o_str ip;
	if ( _server_ip.get_len() == 0 )
	{
		ip.set( "127.0.0.1" );
	}
	else
	{
		ip.set( _server_ip.get() );
	}
	if( (err_code = dll_pqlabs.ConnectServer( ip.get(), _server_port ) ) != PQMTE_SUCCESS)
	{
		ERR_PRINT_STRING( " connect server fail, socket error code : %d", err_code );
		return err_code;
	}
	trackers::PRINT_STRING( PQLABS_HEADER, "Connected to server %s:%d", ip.get(), _server_port );

	// send request to server
	TouchClientRequest	tcq = {0};
	tcq.type = RQST_RAWDATA_ALL;
	if( ( err_code = dll_pqlabs.SendRequest(tcq) ) != PQMTE_SUCCESS )
	{
		ERR_PRINT_STRING( " send request fail, error code: %d", err_code );
		return err_code;
	}
	trackers::PRINT_STRING( PQLABS_HEADER, "request Raw Data succesful" );

	////////////you can set the move_threshold when the tcq.type is RQST_RAWDATA_INSIDE;
	////send threshold
	//int move_threshold = 0;// 0 pixel, receuve all the touch points that touching in the windows area of this client;
	//if((err_code = SendThreshold(move_threshold)) != PQMTE_SUCCESS){
	//	cout << " send threadhold fail, error code:" << err_code << endl;
	//	return err_code;
	//}
	
	//////// you can set the resolution of the touch point(raw data) here;
	//// setrawdata_resolution
	//int maxX = 32768, maxY = 32768;
	//if((err_code= SetRawDataResolution(maxX, maxY)) != PQMTE_SUCCESS){
	//	cout << " set raw data resolution fail, error code:" << err_code << endl;
	//}
	////////////////////////

	//get server resolution
	if((err_code = dll_pqlabs.GetServerResolution(callback_server_resolution, this)) != PQMTE_SUCCESS)
	{
		ERR_PRINT_STRING( "PQLabs couldn't get server resolution, error code: %d", err_code );
		dll_pqlabs.DisconnectServer();
		return err_code;
	}
	_b_opened = true;
	return AAA_OK;
}

void	c_bdd_pqlabs::close()
{
	if( _b_opened )
	{
		dll_pqlabs.DisconnectServer();
		_b_opened = false;
	}
}

void	MAACALLBACK	c_bdd_pqlabs:: callback_gesture(const TouchGesture & ges, void * call_back_object)
{
	// if OnReceiveGesture callback not set, call_back_oject is null for callback_pointframe
}

void	MAACALLBACK	c_bdd_pqlabs::callback_pointframe(int frame_id,int time_stamp,int moving_point_count,const TouchPoint * moving_point_array, void * call_back_object)
{
	c_bdd_pqlabs* pqlabs = static_cast<c_bdd_pqlabs*>(call_back_object);

	if( pqlabs )
	{
		pqlabs->do_frame( frame_id, time_stamp, moving_point_count, moving_point_array );
	}
}

void c_bdd_pqlabs::do_frame( int frame_id,int time_stamp,int moving_point_count,const TouchPoint * moving_point_array )
{
	c_blob blob;
	std::lock_guard<aaa::MUTEX> guard(_lock_blobs);
	
	_v_blobs.clear();
	REAL	ssx_over = 1.0f / (REAL)_server_size_x;
	REAL	ssy_over = 1.0f / (REAL)_server_size_y;
	for(int i = 0; i < moving_point_count; ++ i)
	{
		TouchPoint tp = moving_point_array[i];
		if( tp.point_event != TP_UP )
		{
			blob.set_quick( tp.id + 1, (REAL)tp.x * ssx_over, REAL(1) - (REAL)tp.y * ssy_over, (REAL)tp.dx * ssx_over, (REAL)tp.dy * ssx_over );
			if( _b_verbose )
				trackers::PRINT_STRING( PQLABS_HEADER, "PQLabs : point %d come at ( %d, %d ) width: %d, height: %d )", tp.id, tp.x ,tp.y, tp.dx, tp.dy );
			_v_blobs.push_back( blob );
		}
	}
}

void	c_bdd_pqlabs::transfer_blobs_to( BLOBS_CONT& blobs )
{
	REAL	pos_blob[ 2 ];
	REAL	sca[ 3 ];
	REAL	pos[ 2 ];		//todonow extend to 3
	c_blob	blob;

	scale_v3( sca, _transfer_scale_ui, _transfer_scale_ui[3] );
	
	{
		std::lock_guard<aaa::MUTEX> guard(_lock_blobs);
		UINT32	nb = (UINT32)_v_blobs.size();
		for( UINT32 i = 0; i < nb; ++i )
		{
			pos_blob[ 0 ] = _v_blobs[ i ]._center.get_x();
			pos_blob[ 1 ] = _v_blobs[ i ]._center.get_y();

			add_mul_v2r( pos, _transfer_translate, pos_blob, sca );

			blob.set_quick( _v_blobs[ i ].get_id(), pos[ 0 ], pos[ 1 ], _v_blobs[ i ]._box.get_size_x() * _transfer_blob_radius, _v_blobs[ i ]._box.get_size_y() * _transfer_blob_radius );
				//_transfer_blob_radius );
			blobs.push_back( blob );
		}
		_v_blobs.clear();
	}
}

// callback_serverbreak: function to handle when server break(disconnect or network error)
void	MAACALLBACK	c_bdd_pqlabs::callback_serverbreak(void * param, void * call_back_object)
{
	// when the server break, disconenct server;
	c_bdd_pqlabs* pqlabs = static_cast<c_bdd_pqlabs*>(call_back_object);
	if( pqlabs )
		pqlabs->close();
}

// callback_error: function to handle when some errors occur on the process of receiving touch datas.
void	MAACALLBACK	c_bdd_pqlabs::callback_error(int err_code,void * call_back_object)
{
	switch(err_code)
	{
	case PQMTE_RCV_INVALIDATE_DATA:
		ERR_PRINT_STRING( "PQLabs error: received invalidate data." );
		break;
	case PQMTE_SERVER_VERSION_OLD:
		ERR_PRINT_STRING( "PQLabs error: the multi-touch server is old for this client, please update the multi-touch server." );
		break;
	case PQMTE_EXCEPTION_FROM_CALLBACKFUNCTION:
		ERR_PRINT_STRING( "PQLabs **** some exceptions thrown from the call back functions." );
	//	assert(0); //need to add try/catch in the callback functions to fix the bug;
		break;
	default:
		ERR_PRINT_STRING( "PQLabs socket error, socket error code: %d", err_code );
	}
}

// callback_server_resolution: function to get the resolution of the server system.attention: not the resolution of touch screen. 
void	MAACALLBACK	c_bdd_pqlabs::callback_server_resolution(int x, int y, void * call_back_object)
{
	c_bdd_pqlabs* pqlabs = static_cast<c_bdd_pqlabs*>(call_back_object);

	trackers::PRINT_STRING( PQLABS_HEADER, "Server Resolution %dx%d", x, y );
	if( pqlabs )
		pqlabs->set_server_resolution( x, y );
}

void	c_bdd_pqlabs::set_server_resolution( INT32 x, INT32 y )
{
	_server_size_x = x;
	_server_size_y = y;
}

void	c_bdd_pqlabs::set_info( const CHAR* serial, INT32 w, INT32 h )
{
	_screen_size_x = w;
	_screen_size_y = h;
	_serial.set( serial );
}

// callback_device_info: function to get the information of the touch device.
void c_bdd_pqlabs::callback_device_info( const TouchDeviceInfo & device_info, void * call_back_object )
{
	INT32	sx	=	device_info.screen_width;
	INT32	sy	=	device_info.screen_height;
	trackers::PRINT_STRING( PQLABS_HEADER, "PQLabs, SerialNumber : %s,( %d, %d )", device_info.serial_number, sx, sy );
	if( call_back_object )
	{
		c_bdd_pqlabs* pqlabs = static_cast<c_bdd_pqlabs*>(call_back_object);
		if( pqlabs )
			pqlabs->set_info( device_info.serial_number, sx, sy );
	}
}

void	c_bdd_pqlabs::draw()
{
}

void	c_bdd_pqlabs::update()
{
	if( !is_active() )
		return;

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

	_touch_nb = (UINT32)_v_blobs.size();
	if( !_target_name_symbo.is_empty() )
	{
		if( !_bdd_target || !_bdd_target->is_name_symbo( _target_name_symbo ) || !_bdd_target->get_root() )
			_bdd_target = (c_bdd*) find_by_class_and_name_symbo( "bdd_blob", _target_name_symbo );
	}
	else
		_bdd_target = nullptr;
	if( _bdd_target )
		((c_bdd_blob*)_bdd_target)->register_as_src( this );
}
