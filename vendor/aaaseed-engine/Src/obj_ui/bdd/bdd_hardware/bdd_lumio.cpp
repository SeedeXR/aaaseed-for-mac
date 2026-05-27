#include "bdd_lumio.h"


#if AAA_USE_LUMIO()
#	include "time/ourtime.h"
#	include "obj_ui/bdd/bdd_point/bdd_blob.h"
		 
		 
#	include "Thread/aaa_thread.h"
#	include "thread/scheduler.h"

#	include "system/shared/SystemUtils.h"
#	include "spy.h"

class c_thread_lumio : public c_thread
{
public:
	virtual	void	run()		{	run_it< c_bdd_lumio, -1 > ();	}
	c_thread_lumio() : c_thread( "lumio" )	{}
};

#	ifdef WIN32
#	include <lib_use.h>
	AAA_LIB_USE32( "hid" )
	AAA_LIB_USE32( "setupapi" )
#	endif
#endif	//#if AAA_USE_LUMIO()

FACTORY_CREATE_PROP_V1( c_bdd_lumio, bdd_lumio, Lumio Dual Touch, bdd_lumio, sub_menu="Hardware"; );


namespace n_bdd_lumio
{
	static	CONST	INT32	BASE_PARAM_NB		=	6 + c_bdd::GEO_PARAM_NB;
	static	CONST	INT32	TRANSFER_PARAM_NB	=	9;
	static	CONST	INT32	GROUP_PARAM_NB		=	1;	

	static	CONST	INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
											+	TRANSFER_PARAM_NB
											+	GROUP_PARAM_NB;
			
	static	c_param_def	param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_BOOL_OFF(		active			)
		PARAM_DEF_BOOL_OFF(		open			)
		PARAM_DEF_BOOL_OFF(		verbose			)
		PARAM_DEF_STR_LOCKED(	vendor_id		)
		PARAM_DEF_STR_LOCKED(	device_id		)
		PARAM_DEF_STR_LOCKED(	device_pathname	)

		PARAM_DEF_GROUP_CLOSED( Transfer, TRANSFER_PARAM_NB		)
			PARAM_DEF_REF(			transfer_bdd_target		)
			PARAM_DEF_REAL_ONE(		transfer_blob_radius	)
			PARAM_DEF_POINT_XYZ(	transfer_center			)
			PARAM_DEF_SCALE_XYZF(	transfer_size			)
	};
}	

	

void	c_bdd_lumio::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt( h, get_pt_active()	);
	param_set_pt( h, _b_open_ui			);
	param_set_pt( h, _b_verbose			);
	param_set_pt( h, _s_vendor_id		);
	param_set_pt( h, _s_device_id		);
	param_set_pt( h, _device_pathname	);

	++h;
		param_set_pt_attach_obj(	h, _target_name_symbo,		_bdd_target	);
		param_set_pt(				h, _transfer_blob_radius	);
		param_set_pt_3(				h, _transfer_translate		);
		param_set_pt_4(				h, _transfer_scale_ui		);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE( c_bdd_lumio )
#if AAA_USE_LUMIO()
,_p_lock_hid(nullptr)
,_p_thread_lumio(nullptr)
#endif	//#if AAA_USE_LUMIO()
{
	_bdd_target = nullptr;
	_b_open = false;
	param_init_with( n_bdd_lumio::param, n_bdd_lumio::PARAM_NB_MAX );
	//serial_ = nullptr;
#if AAA_USE_LUMIO()
	_h_dev_info = INVALID_HANDLE_VALUE;
	_device_handle = INVALID_HANDLE_VALUE;
//	_read_handle = INVALID_HANDLE_VALUE;
//	_write_handle = INVALID_HANDLE_VALUE;
	_io_handle = INVALID_HANDLE_VALUE;
#endif	//#if AAA_USE_LUMIO()
//	_h_event_object = nullptr;
	_b_hid_ok = false;
	_b_set_mouse_mode = false;
	_b_close_thread = false;
	_b_blobs[ 0 ] = false;
	_b_blobs[ 1 ] = false;
	_b_first = true;
	_b_process = false;
}

c_bdd_lumio::~c_bdd_lumio()
{
#if AAA_USE_LUMIO()
	close();
#endif
}

#if AAA_USE_LUMIO()
void	c_bdd_lumio::analyse_input_report()
{
	//GOOD_PRINT_STRING( "analyse_report_ex" );
	//DBG_PRINT_STRING( "_input_report : %#x %#x %#x %#x %#x %#x %#x %#x", _input_report[1],
	//	_input_report[2], _input_report[3], _input_report[4], _input_report[5], _input_report[6], _input_report[7], _input_report[8]
	//);
	//DBG_PRINT_STRING( "HID _input_report2 : %#x %#x %#x %#x %#x %#x %#x %#x", _input_report2[1],
	//	_input_report2[2], _input_report2[3], _input_report2[4], _input_report2[5], _input_report2[6], _input_report2[7], _input_report2[8]
	//);

	if( ( _input_report[ 1 ] == 0x7f ) && ( _input_report[ 2 ] == 0x9d ) ) 
	{
//		GOOD_PRINT_STRING( "Processing packets" );
		INT32	low_x1, low_y1, low_w_l1, high_x1, high_y1, high_w_l1, low_w_r1;
		INT32	low_x2, low_y2, low_w_l2, high_x2, high_y2, low_w_r2, high_w_r1 , high_w_l2, high_w_r2;

		low_x1 = _input_report[ 5 ];
		low_y1 = _input_report[ 6 ];
		low_w_l1 = _input_report[ 8 ];
		high_x1 = _input_report[ 4 ] >> 4;
		high_y1 = _input_report[ 7 ] & 15;
		high_w_l1 = _input_report[ 7 ] >> 4;

		low_w_r1 = _input_report2[ 1 ];
		low_x2 = _input_report2[ 3 ];
		low_y2 = _input_report2[ 5 ];
		low_w_l2 = _input_report2[ 6 ];
		low_w_r2 = _input_report2[ 8 ];
		high_w_r1 = _input_report2[ 2 ] & 15;
		high_x2 = _input_report2[ 4 ] & 15;
		high_y2 = _input_report2[ 4 ] >> 4;
		high_w_l2 = _input_report2[ 7 ] & 15;
		high_w_r2 = _input_report2[ 7 ] >> 4;

		INT32	op, op2, tagid, tagid2;

		op = _input_report[ 4 ] & 3;
		tagid = ( _input_report[ 4 ] >> 2 ) & 3;

		op2 = ( _input_report2[ 2 ] >> 4 ) & 3;
		tagid2 = ( _input_report2[ 2 ] >> 6 ) & 3;

		REAL	x1, y1, wl1, wr1, x2, y2, wr2, wl2;
		x1 = (REAL)( 256 * high_x1 + low_x1 ) / 2047.;
		y1 = 1. - (REAL)( 256 * high_y1 + low_y1 ) / 2047.;
		wl1 = ( (REAL)(256 * high_w_l1 ) + low_w_l1 ) / 2047;
		wr1 = ( (REAL)(256 * high_w_r1 ) + low_w_r1 ) / 2047;
		x2 = (REAL)( 256 * high_x2 + low_x2 ) / 2047.;
		y2 = 1. - (REAL)( 256 * high_y2 + low_y2 ) / 2047.;
		wl2 = ( (REAL)(256 * high_w_l2) + low_w_l2 ) / 2047;
		wr2 = ( (REAL)(256 * high_w_r2) + low_w_r2 ) / 2047;

		//	Xcoordinate1 = (256 * HighX1) + LowX1. 
		//	Ycoordinate1 = (256 * HighY1) + LowY1. 
		//	WidthL1 = (256 * High Width-L1) + Low Width-L1. 
		//	WidthR1 = (256 * High Width-R1) + Low Width-R1. 
		//	Xcoordinate2 = (256 * HighX2) + LowX2. 
		//	Ycoordinate2 = (256 * HighY2) + LowY2. 
		//	WidthL2 = (256 * High Width-L2) + Low Width-L2. 
		//	WidthR2 = (256 * High Width-R2) + Low Width-R2. 

//		GOOD_PRINT_STRING( "tagid : %d, tagid2 : %d", tagid, tagid2 );


		if( _input_report[ 3 ] == 0x7 )
		{
//			GOOD_PRINT_STRING( "1 Single Touch" );
//			GOOD_PRINT_STRING( "Single x1 %.2f, y1 %.2f, wl1 %.2f, wr1 %.2f", x1, y1, wl1, wr1 );

			bool _b_id = ( tagid == 1 || tagid == 2 );
			if( _b_id )
			{
				_blobs[ tagid - 1 ]._center.set_x( x1 );
				_blobs[ tagid - 1 ]._center.set_y( y1 );
				switch( op )
				{
	//			case	0 :	// Move
	//				{
	////					DBG_PRINT_STRING( "1 move %f, %f", x1, y1 );
	//				}
	//				break;
				case	1 : // Down
					// new blob
						_blobs[ tagid - 1 ]._b_size_valid = true;
						_b_blobs[ tagid - 1 ] = true;
		//				DBG_PRINT_STRING( "Down %d", tagid );
					break;
				case	2 : // Up
						_b_blobs[ tagid - 1 ] = false;
		//				DBG_PRINT_STRING( "Up %d", tagid );
					break;

				}
			}
		}
		else if( _input_report[ 3 ] == 0xd )
		{
			//GOOD_PRINT_STRING( "1 Dual Touch" );
			//GOOD_PRINT_STRING( "Dual x1 %.2f, y1 %.2f, wl1 %.2f, wr1 %.2f", x1, y1, wl1, wr1 );
			//GOOD_PRINT_STRING( "Dual x2 %.2f, y2 %.2f, wl2 %.2f, wr2 %.2f", x2, y2, wl2, wr2 );
			//GOOD_PRINT_STRING( "tagid : %d, tagid2 : %d", tagid, tagid2 );

			bool _b_id = ( tagid == 1 || tagid == 2 );
			bool _b_id2 = ( tagid2 == 1 || tagid2 == 2 );

			if( _b_id )
			{
				_blobs[ tagid - 1 ]._center.set_x( x1 );
				_blobs[ tagid - 1 ]._center.set_y( y1 );
				switch( op )
				{
				//case	0 :	// Move
				//	//				DBG_PRINT_STRING( "1 move %f, %f", x1, y1 );
				//	break;
				case	1 : // Down
					// new blob
						_b_blobs[ tagid - 1 ] = true;
						//					DBG_PRINT_STRING( "Down %d", tagid );
					break;
				case	2 : // Up
						_b_blobs[ tagid - 1 ] = false;
						//				DBG_PRINT_STRING( "Up %d", tagid );
					break;
				}

			}
			if( _b_id2 )
			{
				_blobs[ tagid2 - 1 ]._center.set_x( x2 );
				_blobs[ tagid2 - 1 ]._center.set_y( y2 );
				switch( op2 )
				{
		//		case	0 :	// Move
		////			DBG_PRINT_STRING( "1 move %f, %f", x2, y2 );
		//			break;
				case	1 : // Down
					// new blob
						_b_blobs[ tagid2 - 1 ] = true;
		//				DBG_PRINT_STRING( "Down %d", tagid2 );
					break;
				case	2 : // Up
						_b_blobs[ tagid2 - 1 ] = false;
	//					DBG_PRINT_STRING( "Down %d", tagid2 );
					break;
				}
			}

		}
	}
}
#endif	//#if AAA_USE_LUMIO()

void	c_bdd_lumio::update()
{
#if AAA_USE_LUMIO()
	if( is_active() )
	{

		//hack to be refined
		if( !_p_lock_hid )
			_p_lock_hid = new aaa::MUTEX;
		if( !_p_thread_lumio )
		{
			_p_thread_lumio = new c_thread_lumio;
			_p_thread_lumio->create( this, 0 );
		}
		if( _b_open != _b_open_ui )
		{
			if( _b_open_ui )
				open();
			else
				close();
		}
		if( _b_open )
		{
			if( !_target_name_symbo.is_empty() )
			{
				if( !_bdd_target->is_name_symbo( _target_name_symbo ) || !_bdd_target->get_root() )
					_bdd_target = (c_bdd*) find_by_class_and_name_symbo( "bdd_blob", _target_name_symbo );
			}
			else
				_bdd_target = nullptr;
			if( _bdd_target )
				((c_bdd_blob*)_bdd_target)->register_as_src( this );
		}
		//lumio_cur = this;
	}
#endif	//#if AAA_USE_LUMIO()
}

void	c_bdd_lumio::draw()
{
}

#if AAA_USE_LUMIO()
void	c_bdd_lumio::close()
{
	_b_close_thread = true;
	if ( _io_handle != INVALID_HANDLE_VALUE )
	{
		//CancelIoEx( _io_handle, nullptr );	//Vista+ only
		//TerminateThread( _thread_handle, 0 );	// hack, this should be done in a thread class
		//CloseHandle( _thread_handle );
		_p_thread_lumio->terminate();
	}
	if ( _b_set_mouse_mode )
	{
		set_mouse_mode();
	}

	close_handles();
	_h_dev_info = INVALID_HANDLE_VALUE;
	_b_open = false;
}

FINLINE	void	c_bdd_lumio::clear_feature_report()
{
	MEMCLEAR( _feature_report, MAX_INPUT_LENGHT );
}

FINLINE	void	c_bdd_lumio::clear_output_report()
{
	MEMCLEAR( _output_report, MAX_INPUT_LENGHT );
}

FINLINE	void	c_bdd_lumio::clear_input_report()
{
	MEMCLEAR( _input_report,  MAX_INPUT_LENGHT );
	MEMCLEAR( _input_report2, MAX_INPUT_LENGHT );
}

void	c_bdd_lumio::set_mouse_mode()
{
	bool b_single_ok = false;
	do 
	{
		// single touch mode
		//	0x7F     0x9B      0x1       0x0       0x0       0x0       0x0       0x0
		clear_output_report();
		_output_report[ 1 ] = 0x7F;
		_output_report[ 2 ] = 0x9B;
		_output_report[ 3 ] = 0x1;
		write_output_report();
		spy::sleep( 10, "sleep c_bdd_lumio::set_mouse_mode()" );

		// Check single touch mode
		// 0x7F     0x9B      0x0       0x0       0x0       0x0       0x0       0x0
		clear_output_report();
		_output_report[ 1 ] = 0x7F;
		_output_report[ 2 ] = 0x9B;
		write_output_report();
		spy::sleep( 50, "sleep c_bdd_lumio::set_mouse_mode() bis" );

		// check this
		// 0x7F     0x9B      0x1       0x0       N/C      N/C      N/C      N/C
		clear_input_report();
		read_input_report( 0, _input_report );
		if( ( _input_report[ 1 ] == 0x7f ) && ( _input_report[ 2 ] == 0x9b ) && ( _input_report[ 3 ] == 0x1 ) && ( _input_report[ 4 ] == 0x0 ) )
		{
			b_single_ok = true;
			GOOD_PRINT_STRING( "HID device in Single Touch Mode" );
		}
	}
	while( !b_single_ok );

	//// mouse mode
	//// 0xFE     0xED    0xDC      0x0       0x0       0x0       0x0      0x87
	//clear_feature_report();
	//_feature_report[ 1 ] = 0xFE;
	//_feature_report[ 2 ] = 0xED;
	//_feature_report[ 3 ] = 0xDC;
	//write_feature_report();
}

void	c_bdd_lumio::set_driver_mode()
{
	// mouse mode, set it to driver mode
	//0x75     0x76      0x0       0x0       0x0       0x0       0x0       0x0
	clear_feature_report();
	_feature_report[ 1 ] = 0x75;
	_feature_report[ 2 ] = 0x76;
	write_feature_report();
	spy::sleep( 2000, "sleep c_bdd_lumio::set_mouse_mode()" );
	_b_set_mouse_mode = false;
}

void	c_bdd_lumio::open()
{
#ifndef WIN64
	if ( _h_dev_info == INVALID_HANDLE_VALUE )
	{
		open_hid();
		_b_open = true;
		_b_hid_ok = false;
		//Mouse Mode: VID  0x0556 PID  0x3556
		//Driver Mode: VID  0x0592 PID  0x6956
		if ( find_device( 0x0556, 0x3556 ) )
		{
			// driver mode
			_s_device_id.set( "0x0556" );
			_s_vendor_id.set( "0x3556" );
			GetDeviceCapabilities();
			set_driver_mode();
			close();

		}
		else if ( find_device( 0x0592, 0x6956 ) )
		{
			// driver mode
			_s_device_id.set( "0x6956" );
			_s_vendor_id.set( "0x0592" );
			GetDeviceCapabilities();
			//get_handles();

			// Just to make sure, upon start up, set the controller to Dual Touch mode. This is 
			// done by opening the device and sending the following 8 bytes report:
			// 0x7F     0x9B      0x1       0x1       0x0       0x0       0x0       0x0
			clear_output_report();
			_output_report[ 1 ] = 0x7F;
			_output_report[ 2 ] = 0x9B;
			_output_report[ 3 ] = 0x1;
			_output_report[ 4 ] = 0x1;

			write_output_report();
			spy::sleep( 10, "sleep c_bdd_lumio::open()" );

			//Wait for 10 milliseconds and then check request from the controller the Touch Type 
			//	mode. This is done by sending the following 8 bytes report:
			//Byte 1  Byte 2  Byte 3  Byte 4  Byte 5  Byte 6  Byte 7  Byte 8
			//	0x7F     0x9B      0x0       0x0       0x0       0x0       0x0       0x0
			clear_output_report();
			_output_report[ 1 ] = 0x7F;
			_output_report[ 2 ] = 0x9B;
			write_output_report();

			//Byte 1  Byte 2  Byte 3  Byte 4  Byte 5  Byte 6  Byte 7  Byte 8
			//	0x7F     0x9B      0x1       0x1       N/C      N/C      N/C      N/C
			read_input_report( 50, _input_report );
			if ( ( _input_report[ 1 ] == 0x7f ) && ( _input_report[ 2 ] == 0x9b ) && ( _input_report[ 3 ] == 0x1 ) && ( _input_report[ 4 ] == 0x1 ) )
			{
				GOOD_PRINT_STRING( "HID device in DualTouch Mode" );
				_b_hid_ok = true;
				_p_thread_lumio->start();
				//start_loop_hid_read();
				}
			_b_set_mouse_mode = true;
		}
	}
#else
	debug_break( "c_bdd_lumio not implemented in x64 for now because of hid.lib" );
#endif
}


void	c_bdd_lumio::close_handles()
{
	if ( _h_dev_info != INVALID_HANDLE_VALUE )
	{
		SetupDiDestroyDeviceInfoList( _h_dev_info );
		display_last_error( "SetupDiDestroyDeviceInfoList" );
		_h_dev_info = INVALID_HANDLE_VALUE;
	}
	
	//Close open handles.
	if ( _device_handle != INVALID_HANDLE_VALUE )
		CloseHandle( _device_handle );

	if ( _io_handle != INVALID_HANDLE_VALUE )
		CloseHandle( _io_handle );
}

void	c_bdd_lumio::GetDeviceCapabilities()
{
	//Get the Capabilities structure for the device.
	PHIDP_PREPARSED_DATA	preparsed_data;

	/*
	API function: HidD_GetPreparsedData
	Returns: a pointer to a buffer containing the information about the device's capabilities.
	Requires: A handle returned by CreateFile. There's no need to access the buffer directly,
	but HidP_GetCaps and other API functions require a pointer to the buffer.
	*/
	HidD_GetPreparsedData( _device_handle, &preparsed_data );
	display_last_error( "HidD_GetPreparsedData : " );

	/*
	API function: HidP_GetCaps
	Learn the device's capabilities.
	For standard devices such as joysticks, you can find out the specific
	capabilities of the device.
	For a custom device, the software will probably know what the device is capable of,
	and the call only verifies the information.
	Requires: the pointer to the buffer returned by HidD_GetPreparsedData.
	Returns: a Capabilities structure containing the information.
	*/

	HidP_GetCaps( preparsed_data, &_hid_caps );

	//Display the capabilities
	GOOD_PRINT_STRING( "HID device capabilities :" );
	GOOD_PRINT_STRING( "\tUsage Page : %X", _hid_caps.UsagePage );
	GOOD_PRINT_STRING( "\tInput Report Byte Length : %d", _hid_caps.InputReportByteLength );
	GOOD_PRINT_STRING( "\tOutput Report Byte Length : %d", _hid_caps.OutputReportByteLength );
	GOOD_PRINT_STRING( "\tFeature Report Byte Length : %d", _hid_caps.FeatureReportByteLength );
	GOOD_PRINT_STRING( "\tNumber of Link Collection Nodes : %d", _hid_caps.NumberLinkCollectionNodes );
	GOOD_PRINT_STRING( "\tNumber of Input Button Caps : %d", _hid_caps.NumberInputButtonCaps );
	GOOD_PRINT_STRING( "\tNumber of InputValue Caps : %d", _hid_caps.NumberInputValueCaps );
	GOOD_PRINT_STRING( "\tNumber of InputData Indices : %d", _hid_caps.NumberInputDataIndices );
	GOOD_PRINT_STRING( "\tNumber of Output Button Caps : %d", _hid_caps.NumberOutputButtonCaps );
	GOOD_PRINT_STRING( "\tNumber of Output Value Caps : %d", _hid_caps.NumberOutputValueCaps );
	GOOD_PRINT_STRING( "\tNumber of Output Data Indices : %d", _hid_caps.NumberOutputDataIndices );
	GOOD_PRINT_STRING( "\tNumber of Feature Button Caps : %d", _hid_caps.NumberFeatureButtonCaps );
	GOOD_PRINT_STRING( "\tNumber of Feature Value Caps : %d", _hid_caps.NumberFeatureValueCaps );
	GOOD_PRINT_STRING( "\tNumber of Feature Data Indices : %d", _hid_caps.NumberFeatureDataIndices );

	display_last_error( "HidP_GetCaps: " );

	//No need for PreparsedData any more, so free the memory it's using.
	HidD_FreePreparsedData( preparsed_data );
	display_last_error( "HidD_FreePreparsedData : " ) ;
}


void	c_bdd_lumio::open_hid()
{
#ifndef WIN64
	// API function: HidD_GetHidGuid. Get the GUID for all system HIDs.
	// Returns: the GUID in HidGuid.
	HidD_GetHidGuid( &_hid_guid );	

	// API function: SetupDiGetClassDevs
	// Returns: a handle to a device information set for all installed devices.
	// Requires: the GUID returned by GetHidGuid.

	_h_dev_info = INVALID_HANDLE_VALUE;
	_h_dev_info = SetupDiGetClassDevs( &_hid_guid, nullptr, nullptr, DIGCF_PRESENT | DIGCF_INTERFACEDEVICE );
#endif
}

//void	c_bdd_lumio::register_for_device_notifications()
//{
//
//	// Request to receive messages when a device is attached or removed.
//	// Also see WM_DEVICECHANGE in BEGIN_MESSAGE_MAP(CUsbhidiocDlg, CDialog).
//
//	DEV_BROADCAST_DEVICEINTERFACE dev_broadcast_device_interface;
//	HDEVNOTIFY device_notification_handle;
//
//	dev_broadcast_device_interface.dbcc_size = sizeof( dev_broadcast_device_interface );
//	dev_broadcast_device_interface.dbcc_devicetype = DBT_DEVTYP_DEVICEINTERFACE;
//	dev_broadcast_device_interface.dbcc_classguid = _hid_guid;
////	dev_broadcast_device_interface.dbcc_name = _device_pathname.get();
//
//	//DEVICE_NOTIFY_ALL_INTERFACE_CLASSES
//	device_notification_handle = RegisterDeviceNotification( get_window_main_handle(), &dev_broadcast_device_interface, DEVICE_NOTIFY_WINDOW_HANDLE | DEVICE_NOTIFY_ALL_INTERFACE_CLASSES );
//
//}

bool	c_bdd_lumio::find_device( INT32 vendor_id, INT32 device_id )
{
	//Use a series of API calls to find a HID with a specified Vendor IF and Product ID.
	HIDD_ATTRIBUTES						attributes;
	SP_DEVICE_INTERFACE_DATA			dev_info_data;
	bool								_b_last_device = false;
	int									member_index = 0;
	LONG								result;	
	o_str								usage_description;
	PSP_DEVICE_INTERFACE_DETAIL_DATA	detail_data = nullptr;
	ULONG								length = 0;
	ULONG								required = 0;

	_device_handle = nullptr;
	_b_device_detected = false;

	dev_info_data.cbSize = sizeof( dev_info_data );

	//Step through the available devices looking for the one we want. 
	//Quit on detecting the desired device or checking all available devices without success.
	member_index = 0;
	_b_last_device = false;
	do
	{
		// API function: SetupDiEnumDeviceInterfaces
		// On return, MyDeviceInterfaceData contains the handle to a SP_DEVICE_INTERFACE_DATA structure for a detected device.
		// Requires: The DeviceInfoSet returned in SetupDiGetClassDevs. The HidGuid returned in GetHidGuid. An index to specify a device.
		result = SetupDiEnumDeviceInterfaces( _h_dev_info, 0, &_hid_guid, member_index, &dev_info_data );
		if( result != 0 )
		{
			//A device has been detected, so get more information about it.

			// API function: SetupDiGetDeviceInterfaceDetail
			// Returns: an SP_DEVICE_INTERFACE_DETAIL_DATA structure containing information about a device.
			// 	To retrieve the information, call this function twice. The first time returns the size of the structure in Length.
			// 	The second time returns a pointer to the data in DeviceInfoSet.
			// Requires: A DeviceInfoSet returned by SetupDiGetClassDevs
			// 	The SP_DEVICE_INTERFACE_DATA structure returned by SetupDiEnumDeviceInterfaces.
			// 	The final parameter is an optional pointer to an SP_DEV_INFO_DATA structure.
			// 	This application doesn't retrieve or use the structure.			
			// 	If retrieving the structure, set MyDeviceInfoData.cbSize = length of MyDeviceInfoData.
			// 	and pass the structure's address.

			//Get the Length value. The call will return with a "buffer too small" error which can be ignored.
			result = SetupDiGetDeviceInterfaceDetail( _h_dev_info, &dev_info_data, nullptr, 0, &length, nullptr );
			//Allocate memory for the hDevInfo structure, using the returned Length.
			detail_data = (PSP_DEVICE_INTERFACE_DETAIL_DATA)MALLOC(length);
			//Set cbSize in the detailData structure.
			detail_data -> cbSize = sizeof( SP_DEVICE_INTERFACE_DETAIL_DATA );
			//Call the function again, this time passing it the returned buffer size.
			result = SetupDiGetDeviceInterfaceDetail( _h_dev_info, &dev_info_data, detail_data, length, &required, nullptr );
			// Open a handle to the device.
			// To enable retrieving information about a system mouse or keyboard, don't request Read or Write access for this handle.
			
			// API function: CreateFile
			// Returns: a handle that enables reading and writing to the device.
			// Requires: The DevicePath in the detailData structure returned by SetupDiGetDeviceInterfaceDetail.
			_device_handle = CreateFile( detail_data->DevicePath, 0, FILE_SHARE_READ | FILE_SHARE_WRITE,
						(LPSECURITY_ATTRIBUTES)nullptr, OPEN_EXISTING, 0, nullptr );

			display_last_error( "CreateFile (Device Handle) : " );

			// API function: HidD_GetAttributes
			// Requests information from the device.
			// Requires: the handle returned by CreateFile.
			// Returns: a HIDD_ATTRIBUTES structure containing the Vendor ID, Product ID, and Product Version Number.
			// Use this information to decide if the detected device is the one we're looking for.


			//Set the Size to the number of bytes in the structure.

			attributes.Size = sizeof(attributes);

			result = HidD_GetAttributes( _device_handle, &attributes );

			display_last_error( "HidD_GetAttributes: " );
			//DisplayLastError("HidD_GetAttributes: ");
			//Is it the desired device?
			_b_device_detected = false;
			if( attributes.VendorID == vendor_id )
			{
				if( attributes.ProductID == device_id )
				{
					//Both the Vendor ID and Product ID match.
					_b_device_detected = true;
					_device_pathname.set( detail_data->DevicePath );
					//DisplayData("Device detected");

					////Register to receive device notifications.
					//register_for_device_notifications();

					_io_handle = CreateFile( detail_data->DevicePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ|FILE_SHARE_WRITE, 
						(LPSECURITY_ATTRIBUTES)nullptr, OPEN_EXISTING, 0, nullptr );

					display_last_error( "CreateFile write handle : ");

				} //if (Attributes.ProductID == ProductID)
				else
				{
					//The Product ID doesn't match.
					CloseHandle( _device_handle );
					_device_handle = nullptr;
				}

			} //if (Attributes.VendorID == VendorID)
			else
			{
				//The Vendor ID doesn't match.
				CloseHandle( _device_handle );
				_device_handle = nullptr;
			}

			//Free the memory used by the detailData structure (no longer needed).
			FREE_AND_NULL( detail_data );

		}  //if (Result != 0)
		else
			//SetupDiEnumDeviceInterfaces returned 0, so there are no more devices to check.
			_b_last_device = true;

		//If we haven't found the device yet, and haven't tried every available device, try the next one.
		++member_index;
	} // end do
	while( !_b_last_device && !_b_device_detected );

	if( _b_device_detected )
		GOOD_PRINT_STRING( "HID device detected 0x%04x, 0x%04x", vendor_id, device_id );
	else
		GOOD_PRINT_STRING( "HID device not detected 0x%04x, 0x%04x", vendor_id, device_id );
	return _b_device_detected;
}

void	c_bdd_lumio::display_last_error( C_PCHAR msg )
{
	if( _b_verbose )
	{
		WIN_ERR_PRINT( msg );
	}
}

void	c_bdd_lumio::write_feature_report()
{
	//Send a report to the device.

	DWORD	bytes_written = 0;
	INT		Index =0;
	BOOLEAN	result = FALSE;
	
	//The first byte is the report number.
	_feature_report[ 0 ] = 0;

	//Send a report to the device.

	// HidD_SetFeature : Sends a report to the device.
	// Returns: success or failure.
	// Requires: A device handle returned by CreateFile.
	// A buffer that holds the report.
	// The Output Report length returned by HidP_GetCaps,

	if( _device_handle != INVALID_HANDLE_VALUE )
		result = HidD_SetFeature( _device_handle, _feature_report, 9 ); //_hid_caps.FeatureReportByteLength );

	display_last_error( "HidD_SetFeature: " );

	if( !result )
	{
		//The write attempt failed, so close the handles, display a message,
		//and set MyDeviceDetected to FALSE so the next attempt will look for the device.
		//close();
		ERR_PRINT_STRING( "HID Can't write feature report to device");
		//_b_device_detected = false;
	}
	else
		GOOD_PRINT_STRING( "HID device : A Feature report was written to the device." );

}

void	c_bdd_lumio::write_output_report()
{
	//Send a report to the device.
	DWORD	bytes_written = 0;
	INT		index =0;
	BOOLEAN	result = FALSE;


	//The first byte is the report number.
	_output_report[ 0 ] = 0;

	// API Function: WriteFile Sends a report to the device.
	// Returns: success or failure.
	// Requires: A device handle returned by CreateFile. A buffer that holds the report. 
	// The Output Report length returned by HidP_GetCaps, A variable to hold the number of bytes written.

	if( _io_handle != INVALID_HANDLE_VALUE )
		result = WriteFile( _io_handle, _output_report, _hid_caps.OutputReportByteLength, &bytes_written, nullptr );

	//Display the result of the API call and the report bytes.
	display_last_error( "WriteFile : " );

	if( !result )
	{
		////The WriteFile failed, so close the handles, display a message,
		//close_handles();
		ERR_PRINT_STRING( "HID can't write to device" );
		//_b_device_detected = false;
	}
	else
	{
		if( _b_verbose )
			GOOD_PRINT_STRING( "An Output report was written to the HID device. %d Bytes written", bytes_written );
	}
}

void	c_bdd_lumio::read_input_report( INT32 interval, UCHAR* input_report )
{
	// Retrieve an Input report from the device.
	DWORD	result;
	DWORD	number_bytes_read;

	if( _io_handle != INVALID_HANDLE_VALUE )
	{
		result = ReadFile( _io_handle, input_report, _hid_caps.InputReportByteLength, &number_bytes_read, nullptr );
		if( number_bytes_read > 0 )
		{
			DBG_PRINT_STRING( "HID 1st read %d : %#x %#x %#x %#x %#x %#x %#x %#x", number_bytes_read, input_report[1],
				 input_report[2], input_report[3], input_report[4], input_report[5], input_report[6], input_report[7], input_report[8] );
		}		
	}
}
#endif	//#if AAA_USE_LUMIO()

void	c_bdd_lumio::transfer_blobs_to( BLOBS_CONT& blobs )
{
#if AAA_USE_LUMIO()
	REAL	pos_blob[ 2 ];
	REAL	sca[ 3 ];
	REAL	pos[ 2 ];		//todonow extend to 3
	c_blob	blob;

	scale_v3( sca, _transfer_scale_ui, _transfer_scale_ui[3] );

	{
		std::lock_guard<aaa::MUTEX> guard(*_p_lock_hid);
		for( INT32 i = 0; i < MAX_LUMIO_TOUCH; ++i )
		{
			if( _b_blobs[ i ] )
			{
				pos_blob[ 0 ] = _blobs[ i ]._center.get_x();
				pos_blob[ 1 ] = _blobs[ i ]._center.get_y();

				add_mul_v2r( pos, _transfer_translate, pos_blob, sca );

				blob.set_quick(  _blobs[ i ].get_id(), pos[ 0 ], pos[ 1 ], _transfer_blob_radius );
				blobs.push_back( blob );
			}
		}
	}
#else
	err_print( "LUMIO is not included in this executable, no blob to transfer here, skipping" );
#endif
}

#if AAA_USE_LUMIO()
//void
unsigned int
#ifdef	WIN32
//__cdecl
__stdcall
#endif
th_loop_hid_read( void* dummy )
{
	c_bdd_lumio*	l = (c_bdd_lumio *) dummy;

	l->loop_hid_read();
	GOOD_PRINT_STRING( "Lumio HID read loop ended" );
	c_thread::end_ex( l->get_thread_handle() );
	return 0;
}


void	c_bdd_lumio::start_loop_hid_read()
{
	_thread_handle = c_thread::begin_ex( "Lumio HID read", th_loop_hid_read, 0, (void *)this );
}

void	c_bdd_lumio::read_hid()
{
	if( _b_hid_ok && _io_handle != INVALID_HANDLE_VALUE )
	{	
		//clear_input_report();
		//read_input_report( 0, b_first ? _input_report : _input_report2 );
		DWORD	result;
		DWORD	number_bytes_read = 0;
		result = ReadFile( _io_handle, _b_first ? _input_report : _input_report2, _hid_caps.InputReportByteLength, &number_bytes_read, nullptr );
		//if ( number_bytes_read > 0 )
		//{
		//	if ( b_first )
		//	{
		//		DBG_PRINT_STRING( "HID 1st read %d : %#x %#x %#x %#x %#x %#x %#x %#x", number_bytes_read, _input_report[1],
		//			_input_report[2], _input_report[3], _input_report[4], _input_report[5], _input_report[6], _input_report[7], _input_report[8] );
		//	}
		//	else
		//	{
		//		DBG_PRINT_STRING( "HID 1st read %d : %#x %#x %#x %#x %#x %#x %#x %#x", number_bytes_read, _input_report2[1],
		//			_input_report2[2], _input_report2[3], _input_report2[4], _input_report2[5], _input_report2[6], _input_report2[7], _input_report2[8] );
		//	}
		//}		

		if( number_bytes_read > 0 )
		{
			if( _b_first )
			{
				if( _input_report[ 1 ] == 0x7f )
				{
					_b_first = false;
					_b_process = false;
				}
				else
				{
					ERR_PRINT_STRING( "Lumio HID wrong 1st lecture" );
				}
			} 
			else
			{
				_b_first = true;
				_b_process = true;
			}
		}

		if( _b_process )
		{
			{
				std::lock_guard<aaa::MUTEX> guard(*_p_lock_hid);
				analyse_input_report();
			}
			_b_process = false;
		}
	}
}
void	c_bdd_lumio::loop_hid_read()
{
	_b_first = true;
	_b_process = false;

	//set_process_priority_class( PRIO_CLASS_LOW );
	set_thread_priority( THREAD_PRIO_NORMAL );

	while( 1 )
	{
		read_hid();
		//spy::sleep( 5 );
		if( _b_close_thread )
		{
			break;
		}
	}
}

void	c_bdd_lumio::update_async()
{
	//set_process_priority_class( PRIO_CLASS_LOW );
	set_thread_priority( THREAD_PRIO_NORMAL );

	read_hid();
}
#endif	//#if AAA_USE_LUMIO()

