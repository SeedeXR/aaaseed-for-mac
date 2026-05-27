
#include "capture_kinect.h"
#include "Kinect-win32-internal.h"
#include "wrap_libusb.h"
#include "Kinect-Utility.h"
#include "Kinect-win32-internal.h"	 
#ifndef AAA_SYSTEMUTILS_H
#	include "system/shared/SystemUtils.h"
#endif
#include <atlconv.h>	//T2OLE and OLE2T
//#define	interface	struct
#include "wrap_kinect.h"
#include "obj_ui/bdd/bdd_point/bdd_mocap.h"
#include "obj_ui/tracker/trackers.h"


CONST CHAR	KINECT_HEADER[] = "# KINECT ";
void	KINECT_PRINT_STRING( C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	trackers::PRINT_STRING_VA( KINECT_HEADER, fmt, args );
	va_end(args);
}

UINT32	c_capture_kinect::device_count = 0;

static 	Kinect::KinectFinder* kinect_finder = nullptr;

void	c_capture_kinect::callback_status( HRESULT hr )
{
	if( SUCCEEDED( hr ) )
	{
//		GOOD_PRINT_STRING( "%s() Init with %d", __FUNCTION__, hr );
	}
	else
	{
//		ERR_PRINT_STRING( "%s() init with %d", __FUNCTION__, hr );
	}
}

void CALLBACK kinect_sdk_status_callback( HRESULT hrStatus, const OLECHAR* instanceName, const OLECHAR* uniqueDeviceName, void * pUserData )
{
	//perhaps this was triggering a crash
	//GOOD_PRINT_STRING( "kinect_sdk_status_callback() : 0x%x", pUserData );
	//reinterpret_cast<c_capture_kinect *>(pUserData)->callback_status( hrStatus );	//, instanceName, uniqueDeviceName );
	c_capture_kinect::callback_status( hrStatus );	//, instanceName, uniqueDeviceName );
}


INT32	c_capture_kinect::do_enum( bool b_verbose )
{
	INT32	nb = 0;
	if( !b_allow )
		ERR_PRINT_STRING( "Kinect not allowed : goto to prefs (ctrl F10 then under tracker) to change this." );
	else
	{
		nb = 0;

		if( b_sdk_k4 )
		{
			if( k4_is_lib_loaded() )
			{
				INT32 nb_azure = k4_get_nb();
				KINECT_PRINT_STRING( "%s(): detected %d Kinect Azure.", __FUNCTION__, nb );
				nb += nb_azure;
			}
		}

		if( b_sdk_k2 )
		{
			if( k2_is_lib_loaded() )
			{
				KINECT_PRINT_STRING( "%s(): Kinect v2 lib is loaded, S one Kinect V2 is considered here.", __FUNCTION__ );
				nb += 1;
			}
		}
		else
		{	
			int num_device_v1 = 0;
			if( b_sdk_k1 )
			{
				if( !dll_kinect.is_loaded() )
					dll_kinect.init();

				if( dll_kinect.is_loaded() )
				{

					dll_kinect.NuiSetDeviceStatusCallback( kinect_sdk_status_callback, nullptr );	// HAVE TO BE CALLED ONCE ONLY (WAITING FROM FEEDBACK FROM ms
			
					HRESULT hr = dll_kinect.NuiGetSensorCount( &num_device_v1 );
					if( FAILED(hr) )
					{
						KINECT_PRINT_STRING( "%s() NuiGetSensorCount() Failed.", __FUNCTION__ );
						num_device_v1 = 0;
					}
					else
					{
						KINECT_PRINT_STRING( "%s(): Kinect Nui sdk detected %d Kinect v1.", __FUNCTION__, num_device_v1 );
						/*
						for( INT32 i = 0; i < num_devices; ++i )
						{
							INuiSensor* pNuiSensor = nullptr;

							hr = dll_kinect.NuiCreateSensorByIndex( i, &pNuiSensor);
							if( SUCCEEDED(hr) )
							{
								pNuiSensor->NuiDeviceConnectionId();
								hr = pNuiSensor->NuiStatus();
							}
							safe_release_interface( pNuiSensor );
						}
						*/
					}
				}
			}
			else
			{
				if( !dll_libusb.is_loaded() )
					dll_libusb.init();

				if( dll_libusb.is_loaded() )
				{
					if( !kinect_finder )
					{
						kinect_finder = new Kinect::KinectFinder;
						if( !kinect_finder )
						{
							ERR_PRINT_STRING( "%s() Can create a KinectFinder.",__FUNCTION__ );
							return 0;
						}
					}
					//	count camera

					//maa	Kinect::KinectFinder KF;
					num_device_v1 = kinect_finder->GetKinectCount();
					KINECT_PRINT_STRING( "%s(): KinectFinder using libusb detected %d Kinect v1.", __FUNCTION__, num_device_v1 );
				}
			}
			nb += num_device_v1;

		//	do_enum_format();
		}
	}

	if( b_verbose )
	{
		if( nb < 1 )
			ERR_PRINT_STRING( "%s() : No camera found",__FUNCTION__ );
		else
			KINECT_PRINT_STRING( "%d cameras found all Kinect kind", nb );
	}

	device_count = nb;
	c_kinect_ui::camera_nb_present = nb;

	return nb;
}

bool	c_capture_kinect::b_allow;

bool	c_capture_kinect::b_sdk_k1 = false;
bool	c_capture_kinect::b_sdk_k1_ui;

bool	c_capture_kinect::b_sdk_k2 = false;
bool	c_capture_kinect::b_sdk_k2_ui;

bool	c_capture_kinect::b_sdk_k4 = false;
bool	c_capture_kinect::b_sdk_k4_ui;

c_k1_kinect_listener::c_k1_kinect_listener( Kinect::Kinect *inK )
	:_k1_kinect(inK)
	, _capture_kinect(nullptr)
{
	_k1_kinect = inK;
	for( INT32 i = 0; i < 2048; ++i )
	{
		t_gamma[i] = (unsigned short)(powf(REAL(i / 2048.), 3) * DEPTH_RANGE );
	}
}

void c_k1_kinect_listener::DepthReceived( Kinect::Kinect* K )
{
	//KINECT_PRINT_STRING( "receive depth" );
	_capture_kinect->do_depth_stuff();
}

void c_k1_kinect_listener::ColorReceived( Kinect::Kinect* K )
{
	//KINECT_PRINT_STRING( "receive rgb" );
	_capture_kinect->do_rgb_stuff();
}

void c_k1_kinect_listener::Run( c_capture_kinect* capture_kinect )
{
	_capture_kinect = capture_kinect;
	//maa	SetupWindow();
	_k1_kinect->AddListener( this );

	c_kinect_ui* kui = capture_kinect->get_kinect();
	if( kui->is_depth_asked() )
		_k1_kinect->Run_depth();
	if( kui->is_rgb_asked() )
		_k1_kinect->Run_rgb();
	_k1_kinect->SetLedMode(Kinect::Led_Green);
}

void c_k1_kinect_listener::Stop()
{
	//maa	SetupWindow();
	_k1_kinect->SetLedMode( Kinect::Led_Red );
	_k1_kinect->Stop_depth();
	_k1_kinect->Stop_rgb();
	_k1_kinect->RemoveListener( this );
}


c_capture_kinect::c_capture_kinect()
	:_k1_kinect				(	nullptr	)
	,_ui					(	nullptr	)
	,_k1_kinect_listener	(	nullptr	)
	,_k1_sdk_p_nui_instance	(	nullptr	)
	,_k1_hd_event_depth		(	nullptr	)
	,_k1_hd_event_rgb		(	nullptr	)
	,_k1_hd_event_skeleton	(	nullptr	)
	,_k1_hd_stream_depth	(	nullptr	)
	,_k1_hd_stream_rgb		(	nullptr	)
	,_k1_hd_event_stop		(	nullptr	)
	,_k1_hd_nui_process		(	nullptr	)
	,_img_flux_rgb			(	nullptr	)
	,_img_flux_rgb_aligned	(	nullptr	)
	,_b_emitter_ir			(	false	)
	,_b_k1_sdk_mapping_ok	(	false	)
	,_k1_sdk_p_mapper		(	nullptr	)
	,_buf_depth				(	nullptr	)
	,_k2_time_depth			(	0		)
	,_k2_time_rgb			(	0		)
	,_k2_time_body			(	0		)
	,_k2_mapper				(	nullptr	)
	,_k4_device				(	nullptr	)
	,_k4_capture			(	nullptr	)
	,_k4_transformation		(	nullptr	)
	,_depth_deform_bind		{	-42		}
	,_k2_depth_to_cam_table	{	nullptr	}
{
	_image_flux_count = 3;
//	_serial_number = 0;
//	_camera_id = -1;
}

c_capture_kinect::~c_capture_kinect()
{
	close();
	SAFE_DELETE( _img_flux_rgb );
	SAFE_DELETE( _img_flux_rgb_aligned );
}

c_image_flux*	c_capture_kinect::get_image_flux( INT32 index )
{
	if( _b_running )
	{
		if( index == 1 )
		{	//	RGB
			if( _ui->is_k4() || _ui->is_k2() || (_ui->is_k1() && b_sdk_k1) )
				return _ui->is_rgb() ? _img_flux_rgb : nullptr;
			else
			{
				Kinect::KinectInternalData* KID = _k1_kinect->mInternalData;
				if( KID->RGBRunning )
					return _img_flux_rgb;
			}
		}
		else if( index == 2 )
		{
			//	RGB aligned
			if( _ui->is_k1() && b_sdk_k1 )
				return _ui->_b_rgb_align_asked_ui ? _img_flux_rgb_aligned : nullptr;
		}
	}
	return nullptr;
}

UINT32			c_capture_kinect::get_image_flux_bind( UINT32 CONST index )
{
	if( _ui )
	{
		if( index == 1 )
			return _ui->get_image_2_bind();
		if( index == 2 )
			return _ui->get_image_3_bind();
	}
	return 0;
}
bool			c_capture_kinect::is_keep_on_cpu( UINT32 CONST index )
{
	if( _ui )
		return (index == 1 ) ? _ui->_b_rgb_cpu_ui : _ui->_b_rgb_align_cpu_ui;
	return false;
}
bool			c_capture_kinect::is_keep_on_gpu( UINT32 CONST index )
{
	if( _ui )
		return (index == 1 ) ? _ui->_b_rgb_gpu_ui : _ui->_b_rgb_align_gpu_ui;
	return false;
}

void	c_capture_kinect::close_specific()
{
	stop();
	if( _ui->is_k4() )
		k4_close();
	else if( _ui->is_k2() )
		k2_close();
	_b_opened = false;
}

void	c_capture_kinect::c_init()
{
	b_sdk_k1 = b_sdk_k1_ui;
	b_sdk_k2 = b_sdk_k2_ui;
	b_sdk_k4 = b_sdk_k4_ui;

	do_enum( true );
	// Initialize COM
	//c_COM::init();

}

void	c_capture_kinect::c_deinit()
{
	SAFE_DELETE( kinect_finder );
	//c_COM::close();
}

C_PCHAR	c_capture_kinect::get_error_string( CONST HRESULT hr )
{
	switch( hr )
	{
	case E_NUI_DEVICE_NOT_CONNECTED				:	return "Device is not connected";		break;
	case E_NUI_DEVICE_NOT_READY					:	return "Device is not ready";			break;
	case E_NUI_ALREADY_INITIALIZED				:	return "Already initialized";			break;
	case E_NUI_NO_MORE_ITEMS					:	return "No More Items";					break;
	case E_NUI_FRAME_NO_DATA					:	return "Frame no data";					break;
	case E_NUI_STREAM_NOT_ENABLED				:	return "Stream not enabled";			break;
	case E_NUI_IMAGE_STREAM_IN_USE				:	return "Image stream already in use";	break;
	case E_NUI_FRAME_LIMIT_EXCEEDED				:	return "Frame limit exceeded";			break;
	case E_NUI_FEATURE_NOT_INITIALIZED			:	return "Feature not initialized";		break;
	case E_NUI_NOTGENUINE						:	return "Kinect not genuine";			break;
	case E_NUI_INSUFFICIENTBANDWIDTH			:	return "Insufficient Bandwidth";		break;
	case E_NUI_NOTSUPPORTED						:	return "The device is an unsupported model";	break;
	case E_NUI_DEVICE_IN_USE					:	return "Device already in use";			break;
	case E_NUI_DATABASE_NOT_FOUND				:	return "Database not found";			break;
	case E_NUI_DATABASE_VERSION_MISMATCH		:	return "Database version mismatch";		break;
	case E_NUI_HARDWARE_FEATURE_UNAVAILABLE		:	return "The requested feature is not available on this version of the hardware";			break;
	case E_NUI_NOTCONNECTED						:	return "The hub is no longer connected to the machine";	break;
	case E_NUI_NOTREADY							:	return "Some part of the device is not connected.";		break;
	case E_NUI_SKELETAL_ENGINE_BUSY				:	return "Skeletal engine is already in use";				break;
	case E_NUI_NOTPOWERED						:	return "Kinect is not powered. The hub and motor are connected, but the camera is not.";	break;
	case E_NUI_BADINDEX							:	return "Bad index passed in to NuiCreateInstanceByXXX";	break;

	case E_FAIL									:	return "error code is E_FAIL";			break;

	default										:	return "Unknown error code";			break;						
	}
	
}



void c_capture_kinect::transfert_skeleton( c_bdd_mocap* mo, NUI_SKELETON_DATA& skel, INT32 nb )
{
	std::lock_guard<c_bdd_mocap> guard(*mo);

	bool	b_flip = _ui->_b_skeleton_x_flip;
	REAL	sin, cos;
	GET_SIN_COS_TURN( sin, cos, _ui->_skeleton_rot_x );
	for( INT32 i=0; i<nb; ++i )
	{
		REAL	tra[3];
		Vector4& v = skel.SkeletonPositions[i] ;
		tra[0] = b_flip ? -v.x : v.x;
		REAL y = v.y;
		REAL z = v.z;
		tra[1] =  y * cos + z * sin;
		tra[2] = -y * sin + z * cos;
		mo->store_tra( i+1, tra );
	}
	//	we get the head position here
	//hack 
	if( _ui->_b_compute_head_pos_ui )
	{
		LONG	color_x;
		LONG	color_y;
		LONG	depth_x;
		LONG	depth_y;
		USHORT	depth_z;

		Vector4&	vb = skel.SkeletonPositions[2] ;
		Vector4&	vt = skel.SkeletonPositions[3] ;
		Vector4	v;
		v.x = (vb.x + vt.x) * REAL(.5);
		v.y = (vb.y + vt.y) * REAL(.5);
		v.z = (vb.z + vt.z) * REAL(.5);
		v.w = (vb.w + vt.w) * REAL(.5);
		NuiTransformSkeletonToDepthImage( v, &depth_x, &depth_y, &depth_z );
		_k1_sdk_p_nui_instance->NuiImageGetColorPixelCoordinatesFromDepthPixel( NUI_IMAGE_RESOLUTION_640x480, 0, depth_x, depth_y, depth_z<<3, &color_x, &color_y );

		_ui->_head_pos[0] = color_x * REAL(1./640.);
		_ui->_head_pos[1] = REAL(1) - color_y * REAL(1./480);
	}
}

void c_capture_kinect::k1_do_skeleton()
{
	if( !_b_opened )
		return;
	NUI_SKELETON_FRAME	skel_frame;
	if( FAILED( _k1_sdk_p_nui_instance->NuiSkeletonGetNextFrame( 0, &skel_frame ) ) )
		return;

	c_bdd_mocap*	mo1 = nullptr;
	c_bdd_mocap*	mo2 = nullptr;

	INT32 nb_point;
	bool b_smooth;

	{
		std::lock_guard<c_kinect_ui> guard(*_ui);

		INT32	skeleton_nb_tracked = 0;
		INT32	skeleton_nb_tracked_not = 0;
		//todo	deal with NUI_SKELETON_POSITION_ONLY
		REAL sin, cos;
		GET_SIN_COS_TURN( sin, cos, _ui->_skeleton_rot_x );
		for( INT32 i = 0 ; i < NUI_SKELETON_COUNT ; ++i )
		{
			NUI_SKELETON_DATA& skel = skel_frame.SkeletonData[i];
			c_kinect_ui::c_skel& aaaskel = _ui->_skels[i];

			aaaskel._id = skel.dwTrackingID;

			REAL x = skel.Position.x;
			REAL y = skel.Position.y;
			REAL z = skel.Position.z;

			aaaskel._pos[0] = x;
			aaaskel._pos[1] =  y * cos + z * sin;
			aaaskel._pos[2] = -y * sin + z * cos;
			aaaskel._pos[3] = skel.Position.w;

			switch( skel.eTrackingState  )
			{
			case NUI_SKELETON_TRACKED:
				aaaskel._b_tracked = true;
				++skeleton_nb_tracked;
				break;
			case NUI_SKELETON_POSITION_ONLY:
				aaaskel._b_tracked = false;
				++skeleton_nb_tracked_not;
				break;
			default:
				aaaskel._b_tracked = false;
				break;
			}
		}

		DWORD track_id[] = { 0, 0 };
		INT32 id = _ui->select_id();
		track_id[0] = id;
		_k1_sdk_p_nui_instance->NuiSkeletonSetTrackedSkeletons( track_id );

		_ui->_skeleton_nb_tracked		= skeleton_nb_tracked;
		_ui->_skeleton_nb_tracked_not	= skeleton_nb_tracked_not;

		// no skeletons!
		if( !skeleton_nb_tracked )
			return;

		mo1 = _ui->_bdd_target[0];
		mo2 = _ui->_bdd_target[1];

		nb_point = _ui->_b_skeleton_full_body ? NUI_SKELETON_POSITION_COUNT : NUI_SKELETON_POSITION_HIP_LEFT; //	20 : 12
		b_smooth = _ui->_b_skeleton_smooth;
	}	// do the unlock

	if( b_smooth )
	{
		HRESULT  hr = _k1_sdk_p_nui_instance->NuiTransformSmooth( &skel_frame, nullptr );
		if( hr != S_OK )
		{
			DBG_PRINT_STRING( " Can't smooth skeleton data" );
		}
	}
	if( mo1 || mo2 )
	{
		c_bdd_mocap*	mo = mo1 ? mo1 : mo2;
		//todo	deal with smooth params
		// smooth out the skeleton data

		// we found a skeleton, re-start the timer
		//	m_LastSkeletonFoundTime = -1;

		INT32 nb = 0;
		// draw each skeleton color according to the slot within they are found.
		for( int i = 0 ; i < NUI_SKELETON_COUNT ; ++i )
		{
			// Show skeleton only if it is tracked, and the center-shoulder joint is at least inferred.
			NUI_SKELETON_DATA& skel = skel_frame.SkeletonData[i];
			if	(	skel.eTrackingState == NUI_SKELETON_TRACKED
				&&	skel.eSkeletonPositionTrackingState[NUI_SKELETON_POSITION_SHOULDER_CENTER] != NUI_SKELETON_POSITION_NOT_TRACKED
				)
			{
				++nb;
				if( mo )
				{
					transfert_skeleton( mo, skel, nb_point );
					//todo	deal with more than 2
					if( mo2 && mo != mo2 )	{	mo = mo2;		}
					else					{	mo = nullptr;	}
				}
			}
		}
	//	_kinect_ui->lock();
	//		_kinect_ui->_skeleton_nb_tracked = nb;
	//	_kinect_ui->unlock();
	/*
		//maa for test
		REAL tra[3];
		REAL t = aaa::time::get();
		for(INT32 i=1; i<=12; ++i )
		{
			tra[0] = SIN_INT( t / 8. + i *.0625 );
			tra[1] = i * .1;
			tra[2] = 0.;
			mo->store_tra( i, tra );
		}
	*/
	}
	return;
}

DWORD WINAPI c_capture_kinect::k1_nui_process_callback( LPVOID pParam )
{
	c_capture_kinect*	pthis = (c_capture_kinect *) pParam;
	return pthis->k1_nui_process();
}

//int		m_FramesTotal;
//int		m_LastFPStime;
//int		m_LastFramesTotal;
INT32 c_capture_kinect::k1_nui_process()
{
	INT32 CONST	event_nb =  4;
	// Configure events to be listened on
	HANDLE	hEvents[event_nb] = {	_k1_hd_event_stop,	 _k1_hd_event_depth,	_k1_hd_event_rgb,	_k1_hd_event_skeleton	};
//	int		t;
//	int		dt;

	//	Main thread loop
	bool	b_process = true;
	while( b_process && _b_opened )	//	_b_opened set to false in destructor
	{
		//	Wait for an event to be signaled
		INT32	id_event = WaitForMultipleObjects( event_nb, hEvents, FALSE, 100 );

		//	Process signal events
		switch( id_event )
		{
		case WAIT_TIMEOUT:		continue;
		// If the stop event, stop looping and exit
		case WAIT_ABANDONED_0:
		case WAIT_OBJECT_0:		b_process = false;		continue;
		case 1:					k1_do_depth_sdk();		break;
		case 2:					k1_do_rgb_sdk();		break;
		case 3:					k1_do_skeleton();		break;
//		default:				break;
		}
		/*
		//	Perform FPS processing
		t = timeGetTime( );
		if( m_LastFPStime == -1 )
		{
			m_LastFPStime = t;
			m_LastFramesTotal = m_FramesTotal;
		}

		dt = t - m_LastFPStime;
		if( dt > 1000 )
		{
			m_LastFPStime = t;
			int FrameDelta = m_FramesTotal - m_LastFramesTotal;
			m_LastFramesTotal = m_FramesTotal;
			::PostMessageW(m_hWnd, WM_USER_UPDATE_FPS, IDC_FPS, FrameDelta);
		}

		//	Perform skeletal panel blanking
		if( m_LastSkeletonFoundTime == -1 )
			m_LastSkeletonFoundTime = t;
		dt = t - m_LastSkeletonFoundTime;
		*/
	}
	return (0);
}		

AAA_ERR	c_capture_kinect::open_driver_k1_sdk( INT32 CONST index, o_str CONST * CONST o_device_id )
{
	if( _k1_sdk_p_nui_instance )
		return AAA_OK;

	if( !dll_kinect.init() )
		return ERR_ANY;

	HRESULT	hr;
	if( o_device_id )
	{
		//T2OLE MACRO
		USES_CONVERSION;
		LPTSTR t = sysutils::utf8_to_unicode( o_device_id->get() );
		hr = dll_kinect.NuiCreateSensorById( T2OLE( t ), &_k1_sdk_p_nui_instance );
		sysutils::free_str_tmp( t );
	}
	else
	{
		hr = dll_kinect.NuiCreateSensorByIndex( index, &_k1_sdk_p_nui_instance );
	}
	if( FAILED(hr) )
	{
		ERR_PRINT_STRING( "Can't create Kinect sensor : %s", get_error_string( hr ) );
		return ERR_ANY;
	}

	// Get the status of the sensor, and if connected, then we can initialize it
	hr = _k1_sdk_p_nui_instance->NuiStatus();
	if( FAILED(hr)  )
	{
		sysutils::safe_release( _k1_sdk_p_nui_instance );
		ERR_PRINT_STRING( "Status of Kinect sensor is not OK : %s", get_error_string( hr ) );
		return ERR_ANY;
	}

	//NuiInstanceIndex
	BSTR	name = _k1_sdk_p_nui_instance->NuiDeviceConnectionId();

	_k1_sdk_p_nui_instance->NuiGetCoordinateMapper( &_k1_sdk_p_mapper );
	//CHAR	str_name[2048];
	//WideCharToMultiByte( CP_UTF8, 0, name, -1, str_name, 2048, 0, 0);
	//_o_video_name.set( str_name );
	//_o_device_id.set( str_name );		// device id and device name are the same for Kinect
	//if( name )
	//	::SysFreeString(name);

	//todoq OLE2T and T2OLE will raise an exception when there is a problem during conversion. Is this acceptable ?
	USES_CONVERSION;
	set_flux_name( OLE2T(name) );
	_o_device_id.set( OLE2T(name) );		// device id and device name are the same for Kinect
	_device_index_used = 0;

	return AAA_OK;
}

AAA_ERR	c_capture_kinect::open_driver( INT32 CONST index, o_str CONST * CONST o_device_id )
{	
	_ui->update_at_open();
	if( _ui->is_k4() )
	{
		if( ERR( k4_open( index, o_device_id ) ) )
			return ERR_ANY;
		set_flux_name( "Kinect Azure" );
	}
	else if( _ui->is_k2() )
	{
		if( ERR( k2_open( index, o_device_id ) ) )
			return ERR_ANY;
		set_flux_name( "Kinect v2" );
	}
	else
	{
		INT32 nb = do_enum( true );
		if( nb <= 0 )
			return ERR_ANY;
		set_src_y_inverted( false );

		SAFE_DELETE( _img_flux_rgb );
		_img_flux_rgb = new c_image_flux( nullptr, true );	//todo	we didn't build the non NULL case for first arg
		_img_flux_rgb->set_use_last_frame( true );
		_img_flux_rgb->set_flux_size_format( 640, 480, aaa::PIXEL_FORMAT::RGBA_8 );
		_img_flux_rgb->set_image_nb_to_keep( 4 );
		_img_flux_rgb->set_src_y_inverted( true );
		_img_flux_rgb->set_swap_red_blue( true );

		SAFE_DELETE( _img_flux_rgb_aligned );
		_img_flux_rgb_aligned = new c_image_flux( nullptr, true );
		_img_flux_rgb_aligned->set_use_last_frame( true );
		_img_flux_rgb_aligned->set_flux_size_format( 640, 480, aaa::PIXEL_FORMAT::RGBA_8 );
		_img_flux_rgb_aligned->set_image_nb_to_keep( 4 );
		_img_flux_rgb_aligned->set_src_y_inverted( true );
		//?	_img_flux_rgb->set_swap_red_blue( true );

		if( b_sdk_k1 )
		{
			if( ERR( open_driver_k1_sdk( index, o_device_id ) ) )
			{
				if( o_device_id )
					KINECT_PRINT_STRING( "error getting Kinect by device_id %s", o_device_id->get() );
				else
					KINECT_PRINT_STRING( "error getting Kinect by index %d", index );
				return ERR_ANY;
			}
			set_flux_name( "Kinect Sdk" );
		}
		else
		{
			//Kinect::KinectFinder KF;
			if( !kinect_finder )
			{
				KINECT_PRINT_STRING( "error getting Kinect: kinect finder is null" );
				return ERR_ANY;
			}

			_k1_kinect = kinect_finder->GetKinect( index, false );
			if( !_k1_kinect )
			{
				KINECT_PRINT_STRING( "error getting Kinect" );
				return ERR_ANY;
			}
			_k1_kinect->SetLedMode( Kinect::Led_BlinkingYellow );
	
			set_flux_name( "Kinect" );
		}
	}
	KINECT_PRINT_STRING( "Driver opened" );
	return AAA_OK;
}

bool	c_capture_kinect::set_depth_buf_size( INT32 sx, INT32 sy, aaa::PIXEL_FORMAT pixel_format )
{
	//INT32 framerate = _ps3_eye_ui->get_framerate();
	if( sx != get_size_x() ||  sy != get_size_y() || pixel_format != get_pixel_format_used() )
	{
		if( _ui->is_k4() )
		{
		}
		else if( _ui->is_k2() )
		{
		}
		else if( b_sdk_k1 )
			_img_flux_rgb_aligned->set_flux_size_format( sx, sy, aaa::PIXEL_FORMAT::RGBA_8 );

		INT32	byte_per_pixel = aaa::c_pixel_format::get_byte_per_pixel( pixel_format );
		//_cap_framerate = 60;	//todo

		// Allocate image buffer
		//todo refine using packing / line / alignment constraint
		INT32 pixel_nb = sx * sy;
		_buf_depth = (UINT8 *) REALLOC_ALIGNED( _buf_depth, pixel_nb * byte_per_pixel );
		//init_with_size( _cap_size_x, _cap_size_y,	(_src_bit_per_pixel==8 || _s_force_in_format==FORCE_IN_R_8)?1:4, __FUNCTION__ );
		set_flux_size_format( sx, sy, pixel_format );

		if( _buf_depth )
		{
			set_flux_size_format( sx,sy, pixel_format );
			MEMSET( _buf_depth, 128, pixel_nb * byte_per_pixel );
		}
		else
		{
			set_flux_size_format( 0,0, pixel_format );
			return false;
		}
	}
	return true;
}

AAA_ERR	c_capture_kinect::open_specific( INT32 CONST index, REAL CONST framerate, o_str CONST * CONST o_device_id )
{
	//printf("%s()", __FUNCTION__);
	if( !_b_opened )
	{
		bool b_open =  NOERR( open_driver( index, o_device_id ) );
		//_kinect_ui->_camera_nb_used = cam_nb;
		if( b_open )
			_b_opened = true;	//	set_size();
		else
			close();
	}
	return _b_opened ? AAA_OK : ERR_ANY;
}

/*
void	c_capture_kinect::ask_frame()
{
	//printf( "%s()", __FUNCTION__ );
	//if( _b_streaming )
	{
		//if( set_size( _ps3_eye_ui->get_size_x(), _ps3_eye_ui->get_size_y(), _ps3_eye_ui->get_framerate() ) )
		//	init_with_size( _cap_size_x, _cap_size_y, 4, __FUNCTION__ );
		//printf("before\n");
		if( _buffer_data )
		{
			// This function will block until a new frame is available
			// It will then fill the buffer with frame image data
			//bool	new_frame;
			tbuf_inc( c_tbuf_master::CH_CAPTURE_CALLBACK, 1., "kinect" );
			//tbuf_inc( c_tbuf_master::CH_VIDEO_GET_FRAME, 1., "kinect" );
			//tbuf_dec( c_tbuf_master::CH_VIDEO_GET_FRAME, 1., "kinect_get_frame" );
			//printf("after\n");
			//if( new_frame )
			{
				// Process/display video frame here
				// �
				// If your program is done set �done=true;�
				tbuf_inc( c_tbuf_master::CH_VIDEO_MOVE_FRAME, 1., "kinect_move_frame" );
				got_frame( (UINT8*)_buffer_data, __FUNCTION__ );
				tbuf_dec( c_tbuf_master::CH_VIDEO_MOVE_FRAME, 1., "kinect_move_frame" );
			}
			tbuf_add( c_tbuf_master::CH_CAPTURE_CALLBACK, 0., "kinect" );
		}
	}
}
*/


void	c_capture_kinect::update()
{
	_ui->update();
	if( !_b_opened )
		return;

	if( _ui->is_k4() )
		k4_update();
	else if( _ui->is_k2() )
		k2_update();
	else
	{
		if( b_sdk_k1 && !_k1_sdk_p_nui_instance )
		{
			ERR_PRINT_STRING( "Kinect opened but _p_nui_instance is NULL ?" );
			return;
		}
	
		if( _ui->_b_motor_active_ui && _ui->_motor_pos != _ui->_motor_pos_ui )
		{
			_ui->_motor_pos = _ui->_motor_pos_ui;
			if( b_sdk_k1 )
			{
				LONG pos = NUI_CAMERA_ELEVATION_MINIMUM + (LONG)(_ui->_motor_pos * (NUI_CAMERA_ELEVATION_MAXIMUM - NUI_CAMERA_ELEVATION_MINIMUM));
				_k1_sdk_p_nui_instance->NuiCameraElevationSetAngle(pos);

			}
			else
				_k1_kinect->SetMotorPosition( _ui->_motor_pos );
		}
	
		if( b_sdk_k1 )
		{
			if( _ui->_b_emitter_ir_ui != _b_emitter_ir )
			{
				_b_emitter_ir = _ui->_b_emitter_ir_ui;
				if( _k1_sdk_p_nui_instance )
					_k1_sdk_p_nui_instance->NuiSetForceInfraredEmitterOff( _b_emitter_ir ? TRUE : FALSE );
			}
			if( _b_running && _ui->_b_skeleton && _ui->_b_seated != _ui->_b_seated_ui )
			{
				if( HasSkeletalEngine(_k1_sdk_p_nui_instance) )
				{
					DWORD flags = _ui->_flags_skel;
					if( _ui->_b_seated_ui )
						flags |= NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT;
					HRESULT	hr = _k1_sdk_p_nui_instance->NuiSkeletonTrackingEnable( _k1_hd_event_skeleton, flags );
					if( FAILED( hr ) )
						ERR_PRINT_STRING( "Can't flip seated" );
					else
						_ui->_b_seated = _ui->_b_seated_ui;
				}
			}
		}
		else
		{
			Kinect::KinectInternalData* KID = _k1_kinect->mInternalData;

			bool	b_depth			=	KID->DepthRunning;
			bool	b_depth_asked	=	_ui->is_depth_asked();
			if( b_depth != b_depth_asked )
			{
				if( b_depth_asked )
					_k1_kinect->Run_depth();
				else
					_k1_kinect->Stop_depth();
			}

			bool	b_rgb		=	KID->RGBRunning;
			bool	b_rgb_asked	=	_ui->is_rgb_asked();
			if( b_rgb != b_rgb_asked )
			{
				if( b_rgb_asked )
					_k1_kinect->Run_rgb();
				else
					_k1_kinect->Stop_rgb();
			}
			_ui->set_depth( KID->DepthRunning );
			_ui->set_rgb( KID->RGBRunning );

			if( _b_running )
			{
				if( _ui->is_force_read_depth() )
					do_depth_stuff();
				if( _ui->is_force_read_rgb() )
					do_rgb_stuff();
			}
		}
		_ui->_rgb_align_img_index	= _img_flux_rgb_aligned->get_image_index();
	}
	if( _img_flux_rgb )
		_ui->_rgb_img_index	= _img_flux_rgb->get_image_index();
}

bool	c_capture_kinect::k1_run_sdk( bool b_stream_in )
{
//	DBG_HEAP_IS_CORRUPT();
	//	_kinect_ui->_b_skeleton_can = HasSkeletalEngine(_p_nui_instance);
	//	if( _kinect_ui->_b_skeleton_can )
	//		KINECT_PRINT_STRING( "Have a skeleton engine" );
	//	else
	//		KINECT_PRINT_STRING( "Don't have a skeleton engine" );

	DWORD flags;
	if( _ui->_b_skeleton_asked_ui )
	{
		if( _ui->_b_depth_asked_ui )
			flags = NUI_INITIALIZE_FLAG_USES_DEPTH_AND_PLAYER_INDEX | NUI_INITIALIZE_FLAG_USES_SKELETON;
		else
			flags = NUI_INITIALIZE_FLAG_USES_SKELETON;
	}
	else
		flags = _ui->_b_depth_asked_ui ? NUI_INITIALIZE_FLAG_USES_DEPTH : 0;

	if( _ui->_b_rgb_asked_ui )
		flags |= NUI_INITIALIZE_FLAG_USES_COLOR;

	if( !flags )
	{
		ERR_PRINT_STRING( "Can't open with no depth, no rgb and no skeleton" );
		goto error;
	}
//	DBG_HEAP_IS_CORRUPT();
	HRESULT	hr;
	hr = _k1_sdk_p_nui_instance->NuiInitialize( flags );
	if( hr == E_NUI_SKELETAL_ENGINE_BUSY )
	{
		ERR_PRINT_STRING( "Can't initialize kinect with skeleton" );
		flags = _ui->_b_depth_asked_ui ? NUI_INITIALIZE_FLAG_USES_DEPTH : 0;
		if( _ui->_b_rgb_asked_ui )
			flags |= NUI_INITIALIZE_FLAG_USES_COLOR;
		hr = _k1_sdk_p_nui_instance->NuiInitialize(flags);
	}
	if( FAILED( hr ) )
	{
		ERR_PRINT_STRING( "NuiInitialize failed on kinect v1 sensor" );
		goto error;
	}
//	DBG_HEAP_IS_CORRUPT();
	KINECT_PRINT_STRING( "Opened" );
	_k1_hd_event_depth		=	CreateEventW( nullptr, TRUE, FALSE, nullptr );
	if( !_k1_hd_event_depth )
	{
		ERR_PRINT_STRING( "CreateEvent error: %s", aaa::system::get_err_message().c_str());
		goto error;
	}
	_k1_hd_event_rgb		=	CreateEventW( nullptr, TRUE, FALSE, nullptr );
	if( !_k1_hd_event_rgb )
	{
		ERR_PRINT_STRING( "CreateEvent error: %s", aaa::system::get_err_message().c_str());
		goto error;
	}
	_k1_hd_event_skeleton	=	CreateEventW( nullptr, TRUE, FALSE, nullptr );
	if( !_k1_hd_event_skeleton )
	{
		ERR_PRINT_STRING( "CreateEvent error: %s", aaa::system::get_err_message().c_str());
		goto error;
	}
//	DBG_HEAP_IS_CORRUPT();
	if( _ui->_b_skeleton_asked_ui )
	{
		if( HasSkeletalEngine(_k1_sdk_p_nui_instance) )
		{
			_ui->_flags_skel = NUI_SKELETON_TRACKING_FLAG_TITLE_SETS_TRACKED_SKELETONS;
			DWORD flags = _ui->_flags_skel;
			if( _ui->_b_seated_ui )
				flags |= NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT;
			
			hr = _k1_sdk_p_nui_instance->NuiSkeletonTrackingEnable( _k1_hd_event_skeleton, flags );
			if( FAILED( hr ) )
			{
				ERR_PRINT_STRING( "Can't enable skeleton" );
				goto error;
			}
			_ui->_b_seated = _ui->_b_seated_ui;
			DWORD track_id[] = { 0, 0 };
			hr = _k1_sdk_p_nui_instance->NuiSkeletonSetTrackedSkeletons( track_id );
			if( FAILED( hr ) )
			{
				ERR_PRINT_STRING( "Can't select skeleton" );
				goto error;
			}
			_ui->_b_skeleton = true;
		}
	}
//	DBG_HEAP_IS_CORRUPT();
	if( _ui->_b_rgb_asked_ui )
	{
		hr = _k1_sdk_p_nui_instance->NuiImageStreamOpen(	NUI_IMAGE_TYPE_COLOR,
													NUI_IMAGE_RESOLUTION_640x480,
													0,
													NUI_IMAGE_STREAM_FRAME_LIMIT_MAXIMUM,
													_k1_hd_event_rgb,
													&_k1_hd_stream_rgb
												);
		if( FAILED( hr ) )
		{
			ERR_PRINT_STRING( "Can't open image stream" );
			goto error;
		}
		_ui->set_rgb( _ui->_b_rgb_asked_ui );
	}
//	DBG_HEAP_IS_CORRUPT();
	if( _ui->_b_depth_asked_ui )
	{
		bool	b_ok;
		if( _ui->_b_depth_start_in_high_def_ui )
			b_ok = set_depth_buf_size( 640, 480 );
		else
			b_ok = set_depth_buf_size( 320, 240 );
		if( !b_ok )
			goto error;

		hr = _k1_sdk_p_nui_instance->NuiImageStreamOpen(
				( _ui->_b_skeleton && HasSkeletalEngine(_k1_sdk_p_nui_instance) ) ? NUI_IMAGE_TYPE_DEPTH_AND_PLAYER_INDEX : NUI_IMAGE_TYPE_DEPTH,
				_ui->_b_depth_start_in_high_def_ui ? NUI_IMAGE_RESOLUTION_640x480 : NUI_IMAGE_RESOLUTION_320x240,
				_ui->_b_depth_start_with_near_mode_ui ? NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE : 0,
				NUI_IMAGE_STREAM_FRAME_LIMIT_MAXIMUM,
				_k1_hd_event_depth,
				&_k1_hd_stream_depth
			);
		if( FAILED( hr ) )
		{
			ERR_PRINT_STRING( "Can't open image depth stream, Kinect One don't support near mode, that could be it." );
			goto error;
		}
		_ui->set_depth( _ui->_b_depth_asked_ui );
/*
		hr = _p_nui_instance->NuiImageStreamSetImageFrameFlags(
				_hd_stream_depth,
				NUI_IMAGE_STREAM_FLAG_ENABLE_NEAR_MODE
			);
		if( FAILED( hr ) )
		{
			ERR_PRINT_STRING( "Can't enable near mode" );
		}
*/
	}
	//POCO_NO_UNWINDOWS
	// Start the Nui processing thread
//	DBG_HEAP_IS_CORRUPT();
	_k1_hd_event_stop		=	CreateEventW( nullptr, FALSE, FALSE, nullptr );
//	DBG_HEAP_IS_CORRUPT();
	if( !_k1_hd_event_stop )
	{
		ERR_PRINT_STRING( "CreateEvent error: %s", aaa::system::get_err_message().c_str());
		goto error;
	}
	_k1_hd_nui_process		=	CreateThread( nullptr, 0, k1_nui_process_callback, this, 0, nullptr );
//	DBG_HEAP_IS_CORRUPT();

	return true;
error:
	ERR_PRINT_STRING( "Can't run Kinect : %s", get_error_string( hr ) );
//	DBG_HEAP_IS_CORRUPT();
	return false;
}

AAA_ERR	c_capture_kinect::run_low( bool b_stream_in )
{
	if( _ui->is_k4() )
		return k4_run();

	if( _ui->is_k2() )
		return k2_run();

	if( b_sdk_k1 )
		return k1_run_sdk( b_stream_in );

	//	printf("%s()", __FUNCTION__);
	//	Start capturing
//	DBG_HEAP_IS_CORRUPT();
	if( !_k1_kinect_listener )
	{
		bool	b_ok = set_depth_buf_size( 640, 480 );
		if( b_ok )
			_k1_kinect_listener = new c_k1_kinect_listener(_k1_kinect);
	}
//	DBG_HEAP_IS_CORRUPT();
	if( _k1_kinect_listener )
	{
		_k1_kinect_listener->Run( this );
		return AAA_OK;
	}
//	DBG_HEAP_IS_CORRUPT();
	return ERR_ANY;
}

bool	c_capture_kinect::run( bool b_stream_in )
{
	if( _b_opened && !_b_running )
	{
//		bool	b_start = false;
//		DBG_HEAP_IS_CORRUPT();
		if( NOERR( run_low(b_stream_in) ) )
		{
//			DBG_HEAP_IS_CORRUPT();
//			b_start = true;
			KINECT_PRINT_STRING( "running" );
			_b_running = true;
			_b_streaming = true;	//todo should be _b_streaming = b_stream_in ?
			//ask_frame();
			KINECT_PRINT_STRING( "%s() done", __FUNCTION__ );
		}
		else
		{
//			DBG_HEAP_IS_CORRUPT();
			stop_low();
//			DBG_HEAP_IS_CORRUPT();
			err_print( "Kinect can't start" );
		}
//		DBG_HEAP_IS_CORRUPT();
	}
	return _b_running;
}

void	c_capture_kinect::k1_stop_sdk()
{
	_b_opened = false;

	if( _ui->_b_skeleton )
	{
		HRESULT hr = _k1_sdk_p_nui_instance->NuiSkeletonTrackingDisable();
		if( FAILED( hr ) )
		{
			ERR_PRINT_STRING( "Can't disable skeleton" );
		}
		_ui->_b_skeleton = false;
	}

/*
	// Stop the Nui processing thread
	if( _hd_event_stop != nullptr )
	{
		// Signal the thread
		SetEvent( _hd_event_stop );
		
		// Wait for thread to stop
		if( _hd_nui_process != nullptr )
		{
			spy::wait_for_single_object( _hd_nui_process, INFINITE, "c_capture_kinect::k1_stop_sdk()" );
			CloseHandle( _hd_nui_process );
			_hd_nui_process = nullptr;
		}
		CloseHandle(_hd_event_stop );
		_hd_event_stop = nullptr;
	}
*/
	

//crash
//	CloseHandle( _hd_stream_depth );	
	_k1_hd_stream_depth = nullptr;

//crash
//	CloseHandle( _hd_stream_rgb );
	_k1_hd_stream_rgb = nullptr;

/*
	if( _hd_event_skeleton && ( _hd_event_skeleton != INVALID_HANDLE_VALUE ) )
	{
		CloseHandle( _hd_event_skeleton );
		_hd_event_skeleton = nullptr;
	}
	if( _hd_event_depth && ( _hd_event_depth != INVALID_HANDLE_VALUE ) )
	{
		CloseHandle( _hd_event_depth );
		_hd_event_depth = nullptr;
	}
	if( _hd_event_rgb && ( _hd_event_rgb != INVALID_HANDLE_VALUE ) )
	{
		CloseHandle( _hd_event_rgb );
		_hd_event_rgb = nullptr;
	}
*/
	sysutils::safe_release( _k1_sdk_p_mapper );
	_k1_sdk_p_mapper = nullptr;
	if( _k1_sdk_p_nui_instance )
	{
		_k1_sdk_p_nui_instance->NuiShutdown();
		sysutils::safe_release( _k1_sdk_p_nui_instance );
	}

}

void	c_capture_kinect::stop_low()
{
	if( _ui->is_k4() )
		k4_stop();
	else if( _ui->is_k2() )
		k2_stop();
	else if( b_sdk_k1 )
		k1_stop_sdk();
	else
	{
		//mem::is_all_ok( "stop before" );
		if( _k1_kinect_listener )
		{
			_k1_kinect_listener->Stop();
			SAFE_DELETE( _k1_kinect_listener );
		}
	}

	_ui->set_depth( false );
	_ui->set_rgb( false );
	_ui->_b_skeleton	= false;

	KINECT_PRINT_STRING( "%s() stopped", __FUNCTION__ );
	_b_running = false;
}

void	c_capture_kinect::stop()
{
	if( _b_running )
		stop_low();
}


/*
#include "FaceTrackLib.h"

void	c_capture_kinect::ft_init()
{
	// This example assumes that the application provides
	// void* cameraFrameBuffer, a buffer for an image, and that there is a method
	// to fill the buffer with data from a camera, for example
	// cameraObj.ProcessIO(cameraFrameBuffer)

	// Create an instance of face tracker
	IFTFaceTracker* pFT = FTCreateFaceTracker();
	if(!pFT)
	{
		// Handle errors
	}

	FT_CAMERA_CONFIG myCameraConfig = { 640, 480, 1.0 }; // width, height, focal length

	HRESULT hr = pFT->Initialize( &myCameraConfig, nullptr, nullptr, nullptr );
	if( FAILED(hr) )
	{
		// Handle errors
	}

	// Create IFTResult to hold a face tracking result
	IFTResult* pFTResult = nullptr;
	hr = pFT->CreateFTResult(&pFTResult);
	if(FAILED(hr))
	{
		// Handle errors
	}

	// prepare Image and SensorData for 640x480 RGB images
	IFTImage* pColorFrame = FTCreateImage();
	if(!pColorFrame)
	{
		// Handle errors
	}

	// Attach assumes that the camera code provided by the application
	// is filling the buffer cameraFrameBuffer
	pColorFrame->Attach(640, 480, cameraFrameBuffer, FORMAT_UINT8_R8G8B8, 640*3);

	FT_SENSOR_DATA sensorData;
	sensorData.pVideoFrame = &colorFrame;
	sensorData.ZoomFactor = 1.0f;
	sensorData.ViewOffset = POINT(0,0);

	bool isTracked = false;
}

void c_capture_kinect::ft_test()
{	// Track a face
	while ( true )
	{
		// Call your camera method to process IO and fill the camera buffer
		cameraObj.ProcessIO(cameraFrameBuffer); // replace with your method

		// Check if we are already tracking a face
		if(!isTracked)
		{
			// Initiate face tracking. This call is more expensive and
			// searches the input image for a face.
			hr = pFT->StartTracking(&sensorData, nullptr, nullptr, pFTResult);
			if(SUCCEEDED(hr) && SUCCEEDED(pFTResult->Status))
			{
				isTracked = true;
			}

			else
			{
				// Handle errors
				isTracked = false;
			}
		}
		else
		{
			// Continue tracking. It uses a previously known face position,
			// so it is an inexpensive call.
			hr = pFT->ContinueTracking(&sensorData, nullptr, pFTResult);
			if(FAILED(hr) || FAILED (pFTResult->Status))
			{
				// Handle errors
				isTracked = false;
			}
		}

		// Do something with pFTResult.

		// Terminate on some criteria.
	}
}

void c_capture_kinect::ft_deinit()
{	// Clean up.
	pFTResult->Release();
	pColorFrame->Release();
	pFT->Release();
}*/