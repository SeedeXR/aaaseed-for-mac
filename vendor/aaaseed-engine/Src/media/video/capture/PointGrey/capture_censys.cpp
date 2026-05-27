#include "capture_censys.h"

#if AAA_USE_POINT_GREY()
#include "infrastructure/param/param_declare.h"
#include "image/bind_img_2d.h"
#include "wrap_censys.h"


static	std::list<c_capture_censys*>	list_cap_censys;
bool	c_capture_censys::b_dll_loaded = false;
UINT32	c_capture_censys::device_count = 0;

INT32	c_capture_censys::do_enum( bool	b_verbose )
{
//	INT32	i;
//	UINT32	nb = 0;
//	o_str name;
//	o_str version;
//
//	DigiclopsError      de;
//	DigiclopsInfo*		camInfo;
//	UINT32*				camSize;
//
//
////todo use digiclopsBusEnumerateCameras( DigiclopsInfo*  parInfo, unsigned int*   puiSize );
//	de = digiclopsBusCameraCount(&nb);
//
//	if (de != DIGICLOPS_OK)
//		{
//		ERR_PRINT_STRING( "capture triclops could not count cameras");
//		return 0;
//		}
//
//	camInfo = new DigiclopsInfo[nb];
//	camSize = new UINT32[nb];
//
//	de = digiclopsBusEnumerateCameras( camInfo, camSize );
//	if (de != DIGICLOPS_OK)
//		{
//		ERR_PRINT_STRING( "capture triclops could not enumerate cameras");
//		return nb;
//		}
//
//	for(i = 0; i < nb; ++i )
//		{
//		if( b_verbose)
//			{
//			CAPTURE_PRINT_STRING( "PGR Triclops Device %d", i );
//			CAPTURE_PRINT_STRING( "        %s (%s) %s", digiclops_camera_device_str[camInfo[i].CameraDevice],
//														digiclops_camera_type_str[camInfo[i].CameraType],
//														digiclops_camera_resolution_str[camInfo[i].ImageSize] );
//			CAPTURE_PRINT_STRING( "serial number : %ld", camInfo[i].SerialNumber );
//			}
//		}
//	return nb;

	if( !b_dll_loaded )
	{
		b_dll_loaded = dll_censys.init();
		if( !b_dll_loaded )
		{
			return 0;
		}
		//INT32	err = wrap_censys_Init();
		//if( err != NO_ERROR )
		//{
		//	return 0;
		//}
		//b_dll_loaded = true;
	}
	device_count = 1;
	return 1;
}

void	c_capture_censys::c_init()
{
	list_cap_censys.clear();
	do_enum( true );
}

extern	c_capture_censys*	capture_censys_find_by_window_hd( HWND hd_wind_in)
{
	if( list_cap_censys.empty() )
		return nullptr;
	else if( hd_wind_in)
	{
		for( auto const & pt : list_cap_censys )
		{
			if( pt->get_hd_wind() == hd_wind_in )
			{
				//	printf( "%d\n", hd_wind_in);
				return pt;
			}
		}
		ERR_PRINT_STRING( "capture window unknown");
		return nullptr;
	}
	else
		return *list_cap_censys.begin();

}


void	capture_censys_add( c_capture_censys* pt )
{
	list_cap_censys.push_back( pt );
}

/*
//maa
void	capture_censys_remove( HWND hd_wind_in)
{
c_capture_censys*	pt;

	pt = capture_censys_find_by_window_hd( hd_wind_in);
	if( pt)
		list_cap_censys.remove( pt);
}
*/

void	c_capture_censys::c_deinit()
{
	while( !list_cap_censys.empty() )
	{
		c_capture_censys*	pt;
		pt = *list_cap_censys.begin();
		list_cap_censys.pop_front();		//hack is it thread/callback safe ?
		delete pt;
	}
}


c_capture_censys::c_capture_censys()
	:_image_flux_2		(	nullptr	)
	,_image_flux_3		(	nullptr	)
{
#if !AAA_WIN64()
	_pgrcamGuiContext = nullptr;
	_censys_inst = nullptr;
#endif	//#if !AAA_WIN64()
	_tracking_fps = 0.0f;

	_o_crossbar_name.set( "None" );

//	DBG_PRINT_STRING( "Capture Censys constructor" );

	_bumblebee_settings_0 = _censys->get_bumblebee( 0 );
	_bumblebee_settings_1 = _censys->get_bumblebee( 1 );

}

c_capture_censys::~c_capture_censys()
{
	close();
	SAFE_DELETE( _image_flux_2 );
	SAFE_DELETE( _image_flux_3 );
	////
	//// Destroy gui context.
	////
	//pgrcamguiDestroyContext( m_guicontext );

}

//void	c_capture::dlg()
//{
//	dlg_source();
////	dlg_format();
////	dlg_display();
//}

void	c_capture_censys::dlg_source()
{
//   //
//   // Show the camera selection dialog.
//   //
//	CameraGUIError	guierror;
//	INT32			iDialogStatus  = 0;
//
////	guierror = pgrcamguiCreateContext( &m_guicontext );
////	if( guierror != PGRCAMGUI_OK )
////		{
//////		return nullptr;
////		}
////	else
////		{
//	if (m_guicontext)
//		{
//		guierror = pgrcamguiShowCameraSelectionModal( m_guicontext, digiclops_context, &triclops_serial_number, &iDialogStatus );
//		if( guierror != PGRCAMGUI_OK )
//			{
//			printf("Error showing camera selection dialog." );
//	//		return nullptr;
//			}
//		}
}

void	c_capture_censys::dlg_display()
{
	//if (m_guicontext)
	//	{
	//	pgrcamguiToggleSettingsWindowState( m_guicontext, get_window_main_handle() );
	//	}
}

void	c_capture_censys::get_params()
{
	_camera_id = _censys->get_camera_id();

	if( _censys->get_print_trigger() )
	{
		_censys->set_print_trigger(false);
		print_settings();
	}
	
	if( _censys->is_reset_background() )
	{
		_censys->set_reset_background(false);
		reset_background();
	}

	_censys->set_tracking_fps( _tracking_fps );

	_b_censys3d_param_no_update = _censys->is_censys3d_param_no_update();

	_b_dot_send = _censys->is_dot_send();
	_b_dot_save = _censys->is_dot_save();

	_b_dot_send_verbose = _censys->is_dot_send_verbose();
	_b_dot_send_value_verbose = _censys->is_dot_send_value_verbose() ;
	_dot_speed = _censys->get_dot_speed();

	_m_censys_tracking.set_dot_send( _b_dot_send );
	_m_censys_tracking.set_dot_save( _b_dot_save );
	_m_censys_tracking.set_dot_send_verbose( _b_dot_send_verbose );
	_m_censys_tracking.set_dot_send_value_verbose( _b_dot_send_value_verbose );
	_m_censys_tracking.set_dot_speed( _dot_speed );

	_stereo_resolution_x = _censys->get_stereo_resolution_x();
	_stereo_resolution_y = _censys->get_stereo_resolution_y();

//	grabDelay = censys->get_GrabDelay();
	
	if( _position_x != _censys->get_position_x() || _position_y != _censys->get_position_y() || _position_z != _censys->get_position_z()
		|| _rotation_x != _censys->get_rotation_x() || _rotation_y != _censys->get_rotation_y() || _rotation_z != _censys->get_rotation_z())
		{
		_position_x = _censys->get_position_x();
		_position_y = _censys->get_position_y();
		_position_z = _censys->get_position_z();
		_rotation_x = _censys->get_rotation_x();
		_rotation_y = _censys->get_rotation_y();
		_rotation_z = _censys->get_rotation_z();
		set_transformation();
		}

	if( _min_z_distance != _censys->get_min_z_distance() || _max_z_distance != _censys->get_max_z_distance())
		{
		_min_z_distance = _censys->get_min_z_distance();
		_max_z_distance = _censys->get_max_z_distance();
		if ( _max_z_distance < _min_z_distance )
			_max_z_distance = _min_z_distance + 0.001;
		set_min_max_z_distance();
		}

	if(	_creep_rate != _censys->get_creep_rate())
		{
		_creep_rate = _censys->get_creep_rate();
		set_creep_rate();
		}

	if( _depth_threshold != _censys->get_depth_threshold())
		{
		_depth_threshold = _censys->get_depth_threshold();
		set_depth_threshold();
		}
	
	if( _point_threshold != _censys->get_point_threshold())
		{
		_point_threshold = _censys->get_point_threshold();
		set_point_threshold();
		}

	if( _granularity != _censys->get_granularity())
		{
		_granularity = _censys->get_granularity();
		set_granularity();
		}

	if( _proximity_filter_radius != _censys->get_proximity_filter_radius())
		{
		_proximity_filter_radius = _censys->get_proximity_filter_radius();
		set_proximity_filter_radius();
		}

	if( _minimum_height_drop != _censys->get_minimum_height_drop())
		{
		_minimum_height_drop = _censys->get_minimum_height_drop();
		set_min_height_drop_between_people();
		}
	
	if(	_min_height_people != _censys->get_min_height_people() || _max_height_people != _censys->get_max_height_people() || _voi_min_x != _censys->get_voi_min_x()
		|| _voi_min_y != _censys->get_voi_min_y() || _voi_max_x != _censys->get_voi_max_x() || _voi_max_y != _censys->get_voi_max_y())
		{
		_voi_min_x = _censys->get_voi_min_x();
		_voi_min_y = _censys->get_voi_min_y();
		_voi_max_x = _censys->get_voi_max_x();
		_voi_max_y = _censys->get_voi_max_y();
		_min_height_people = _censys->get_min_height_people();
		_max_height_people = _censys->get_max_height_people();
		set_simple_voi();
		}

	if( _minimum_path_points != _censys->get_minimum_path_points())
		{
		_minimum_path_points = _censys->get_minimum_path_points();
		set_min_path_points();
		}

	if( _min_path_points_penalty = _censys->get_min_path_points_penalty() )
		{
		_min_path_points_penalty = _censys->get_min_path_points_penalty();
		set_min_path_points_penalty();
		}
	
	if( _frames_before_deleting_person != _censys->get_frames_before_deleting_person())
		{
		_frames_before_deleting_person = _censys->get_frames_before_deleting_person();
		set_frames_before_deleting_person();
		}

	if( _frames_before_deleting_candidate_person = _censys->get_frames_before_deleting_candidate_person() )
		{
		_frames_before_deleting_candidate_person = _censys->get_frames_before_deleting_candidate_person();
		set_frames_before_deleting_candidate_person();
		}

	if( _frames_before_deleting_person_in_border = _censys->get_frames_before_deleting_person_in_border() )
		{
		_frames_before_deleting_person_in_border = _censys->get_frames_before_deleting_person_in_border();
		set_frames_before_deleting_person_in_border();
		}

	if( _max_valid_person_velocity != _censys->get_max_valid_person_velocity())
		{
		_max_valid_person_velocity = _censys->get_max_valid_person_velocity();
		set_max_valid_person_velocity();
		}

	if( _maximum_velocity_angle != _censys->get_maximum_velocity_angle())
		{
		_maximum_velocity_angle = _censys->get_maximum_velocity_angle();
		set_max_velocity_angle();
		}

	//closeMatchDistance = censys->get_closeMatchDistance();
	//maxExcursion = censys->get_maxExcursion();
	//matchTieThreshold = censys->get_matchTieThreshold();

	_dot_offset_x = _censys->get_dot_offset_x();
	_dot_offset_y = _censys->get_dot_offset_y();
	_dot_offset_z = _censys->get_dot_offset_z();

	_m_censys_tracking.set_dot_offset_x( _dot_offset_x );
	_m_censys_tracking.set_dot_offset_y( _dot_offset_y );
	_m_censys_tracking.set_dot_offset_z( _dot_offset_z );

	_dot_scale_x = _censys->get_dot_scale_x();
	_dot_scale_y = _censys->get_dot_scale_y();
	_dot_scale_z = _censys->get_dot_scale_z();

	_m_censys_tracking.set_dot_scale_x( _dot_scale_x );
	_m_censys_tracking.set_dot_scale_y( _dot_scale_y );
	_m_censys_tracking.set_dot_scale_z( _dot_scale_z );

	if( _b_opened )
	{
		_bumblebee_settings_0->get_params();
		_bumblebee_settings_1->get_params();
		_bumblebee_settings_0->set_censys3d_param_no_update( _b_censys3d_param_no_update );
		_bumblebee_settings_1->set_censys3d_param_no_update( _b_censys3d_param_no_update );
	}
}

void	c_capture_censys::init_cam()
{
		_bumblebee_settings_0->get_params();
		_bumblebee_settings_1->get_params();
		_bumblebee_settings_0->update_params();
		_bumblebee_settings_1->update_params();
}

#if !AAA_WIN64()
bool	c_capture_censys::run( bool b_stream_in )
{
	get_params();
	_b_running = true;
	return _b_running;
}

void	c_capture_censys::stop()
{
	CENSYS_ERROR cens_err;

	if( _censys_inst != INVALID_CENSYS_INSTANCE )
	{
		//// pause censys library
		//cens_err = censysSystemPause( censys_inst_ );
		//if( cens_err != censyserrorSUCCESS )
		//	{
		//	ERR_PRINT_STRING( "Censys 3d, system pause failed." );
		//	}
		//	Deleting the censys system.
		cens_err = dll_censys.censysSystemDeleteInstance( _censys_inst );
		if( cens_err != censyserrorSUCCESS )
		{
			ERR_PRINT_STRING( "Censys 3d, deletion of censys instance failed." );
		}
		_censys_inst = INVALID_CENSYS_INSTANCE;	//maa
	}
	_b_running = false;
	_b_streaming = false;
}

void	c_capture_censys::update()
{
	if( _b_running )
	{
		CENSYS_ERROR err = dll_censys.censysSystemProcessData( _censys_inst );
		if( err != censyserrorSUCCESS )
		{
			ERR_PRINT_STRING( "Censys 3d, process function failed." );
		}
	}

	//if( b_active_ )//&& !is_field_new() )
	//	ask_frame();
//	printf("censys 3d update\n");
}

#define CENSYS_INSTANCE_FILENAME ".\\censys3d.ini"
AAA_ERR	c_capture_censys::open_specific( INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_name )
{
	CENSYS_ERROR cens_err;

	if( !_b_opened )
	{
		// Creating the censys system
		cens_err = dll_censys.censysSystemCreateInstance( &_censys_inst, CENSYS_INSTANCE_FILENAME );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Initialization of censys system failed." );
			goto exit_on_error;
		}
		if( dll_censys.censysSystemGetSystemConfig( _censys_inst, &_censys_config ) != censyserrorSUCCESS  )
		{
			ERR_PRINT_STRING( "Error getting grab from files parameters." );
			goto exit_on_error;
		}

		//// Initialize the grabber
		//cens_err = censysGrabberInitialize( &censys_inst, FALSE );
		//if( cens_err != censyserrorSUCCESS || censys_inst == INVALID_CENSYS_INSTANCE )
		//{
		//	ERR_PRINT_STRING( "Initialization of censys3d grabber failed." );
		//	goto exit_on_error;
		//}

		get_params();

		// Setting system resolution.
		set_resolution();
//		set_flux_size_format( _stereo_resolution_x, _stereo_resolution_y,PIXEL_TYPE::RGBA );
		if( !_image_flux_2 )
		{
			_image_flux_2 = new c_image_flux( nullptr, true );
		}
	//	_image_flux_2->set_flux_size_format( _stereo_resolution_x, _stereo_resolution_y,PIXEL_TYPE::RGBA );
		if ( !_image_flux_3 )
		{
			_image_flux_3 = new c_image_flux( nullptr, true );
		}
	//	_image_flux_3->set_flux_size_format( _stereo_resolution_x, _stereo_resolution_y,PIXEL_TYPE::RGBA );

//		init_with_size( _stereo_resolution_x, _stereo_resolution_y,PIXEL_TYPE::RGBA );

		set_transformation();

		// set grabber callback function
		cens_err = dll_censys.censysSystemSetGrabberCallback( _censys_inst, callback_grabber, nullptr );
		if( cens_err != censyserrorSUCCESS )
		{
			ERR_PRINT_STRING( "Setting grabber callback failed." );
			goto exit_on_error;
		}

		// set segmentor callback function
		cens_err = dll_censys.censysSystemSetSegmentorCallback( _censys_inst, callback_segmentor, nullptr );
		if( cens_err != censyserrorSUCCESS )
		{
			ERR_PRINT_STRING( "Setting segmentor callback failed." );
			goto exit_on_error;
		}

		// set extractor callback function
		cens_err = dll_censys.censysSystemSetExtractorCallback( _censys_inst, callback_extractor, nullptr );
		if( cens_err != censyserrorSUCCESS )
		{
			ERR_PRINT_STRING( "Setting extractor callback failed." );
			goto exit_on_error;
		}

		// set tracker callback function
		cens_err = dll_censys.censysSystemSetTrackerCallback( _censys_inst, callback_tracker, nullptr );
		if( cens_err != censyserrorSUCCESS )
		{
			ERR_PRINT_STRING( "Setting tracker callback failed." );
			goto exit_on_error;
		}

		// Run the system
		cens_err = dll_censys.censysSystemInitialize( _censys_inst );
		if( cens_err != censyserrorSUCCESS )
		{
			ERR_PRINT_STRING( "Starting censys_3d failed. %s", dll_censys.censysGetLastError() );
			goto exit_on_error;
		}

		// cannot be run before calling censysSystemRun
		// Get Number of Devices
		INT32	nbDevices;

		_nb_camera = 0;
		cens_err = dll_censys.censysGrabberGetNumberOfDevices( _censys_inst, &nbDevices );
		if( cens_err != censyserrorSUCCESS )
		{
			ERR_PRINT_STRING( "Getting Number of Devices failed." );
			switch( cens_err )
			{
				case censyserrorSUCCESS:				GOOD_PRINT_STRING( "the operation was successful." );	break;
				case censyserrorINVALID_INSTANCE :		ERR_PRINT_STRING( "The CENSYS_INSTANCE is invalid." );	break;
				case censyserrorSYSTEM_NOT_CREATED :	ERR_PRINT_STRING( "system not created" );				break;
			}
			goto exit_on_error;
		}
		else
			{
			_nb_camera = nbDevices;

			_bumblebee_settings_0 = _censys->get_bumblebee( 0 );
			_bumblebee_settings_1 = _censys->get_bumblebee( 1 );

			_bumblebee_settings_0->set_camera_id( 0 );
			_bumblebee_settings_1->set_camera_id( 1 );

			_bumblebee_settings_0->get_triclops_context( _censys_inst );
			_bumblebee_settings_0->get_digiclops_context( _censys_inst );
			_bumblebee_settings_0->get_serial_number( _censys_inst );

			_bumblebee_settings_1->get_triclops_context( _censys_inst );
			_bumblebee_settings_1->get_digiclops_context( _censys_inst );
			_bumblebee_settings_1->get_serial_number( _censys_inst );

			_bumblebee_settings_0->get_params();
			_bumblebee_settings_1->get_params();
			_bumblebee_settings_0->update_params();
			_bumblebee_settings_1->update_params();
			
			//set frame_rate
			//bumblebee_settings_0->set_frame_rate();
			//bumblebee_settings_1->set_frame_rate();

			//debug
			CAPTURE_PRINT_STRING( "Censys 3d, nb of devices : %d", nbDevices );
			}

		capture_censys_add( this );
		_b_opened = true;

		//setting up parameters, since censys initialize from censys3d.ini
		set_min_max_z_distance();
		set_depth_threshold();
		set_creep_rate();
		set_simple_voi();
		set_granularity();
		set_point_threshold();
		set_proximity_filter_radius();
		set_min_height_drop_between_people();
		set_max_valid_person_velocity();
		set_frames_before_deleting_person();
		set_min_path_points();
		set_max_velocity_angle();
		set_frames_before_deleting_candidate_person();
		set_frames_before_deleting_person_in_border();
		set_min_path_points_penalty();
	}

	return AAA_OK;
exit_on_error:
	stop();
	close();
	return ERR_ANY;
}


void	c_capture_censys::close_specific()
{
	if( _b_opened )
	{
		stop();
		_b_opened = false;
	}
}

void	c_capture_censys::set_resolution()
{
	CENSYS_ERROR cens_err;

	if( _censys_inst != INVALID_CENSYS_INSTANCE)
	{
		cens_err = dll_censys.censysSystemSetResolution( _censys_inst, _stereo_resolution_y, _stereo_resolution_x );
		if( cens_err != censyserrorSUCCESS )
		{
			ERR_PRINT_STRING( "Censys 3d, setting system resolution failed." );
		}
		//else
		//	init_with_size( stereo_resolution_y, stereo_resolution_x, 4, __FUNCTION__ );
	}
}

void	c_capture_censys::set_transformation()
{
	CENSYS_ERROR cens_err;

	if( _b_opened && ( _censys_inst != INVALID_CENSYS_INSTANCE ) && !_b_censys3d_param_no_update )
	{
		cens_err = dll_censys.censysSegmentorSetTransformation( _censys_inst, _position_x, _position_y, _position_z, _rotation_x, _rotation_y, _rotation_z );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error setting Transformation." );
		}
	}
}

void	c_capture_censys::set_min_max_z_distance()
{
	CENSYS_ERROR cens_err;

	if( _b_opened && ( _censys_inst != INVALID_CENSYS_INSTANCE ) && !_b_censys3d_param_no_update )
	{
		cens_err = dll_censys.censysSystemSetMinMaxZDistance( _censys_inst, 1, _min_z_distance, _max_z_distance );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error setting min_max_z_distance." );
		}
	}
}

void	c_capture_censys::set_depth_threshold()
{
	CENSYS_ERROR cens_err;

	if( _b_opened && ( _censys_inst != INVALID_CENSYS_INSTANCE ) && !_b_censys3d_param_no_update )
	{
		cens_err = dll_censys.censysSegmentorSetDepthThreshold( _censys_inst, _depth_threshold );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error setting depth_threshold." );
		}
	}
}

void	c_capture_censys::set_creep_rate()
{
	CENSYS_ERROR cens_err;

	if( _b_opened && ( _censys_inst != INVALID_CENSYS_INSTANCE ) && !_b_censys3d_param_no_update )
	{
		cens_err = dll_censys.censysSegmentorSetCreepTime( _censys_inst, _creep_rate );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error setting creep_rate." );
		}
	}
}

void	c_capture_censys::set_simple_voi()
{
	CENSYS_ERROR cens_err;

	if( _b_opened && ( _censys_inst != INVALID_CENSYS_INSTANCE ) && !_b_censys3d_param_no_update )
	{
		cens_err = dll_censys.censysExtractorSetSimpleVOI( _censys_inst, _voi_min_x, _voi_max_x, _voi_min_y, _voi_max_y, _min_height_people, _max_height_people );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error setting simple_voi." );
		}
	}
}

void	c_capture_censys::set_granularity()
{
	CENSYS_ERROR cens_err;

	if( _b_opened && ( _censys_inst != INVALID_CENSYS_INSTANCE ) && !_b_censys3d_param_no_update )
	{
		cens_err = dll_censys.censysExtractorSetGranularity( _censys_inst, _granularity );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error setting granularity." );
		}
	}
}

void	c_capture_censys::set_point_threshold()
{
	CENSYS_ERROR	cens_err;

	if( _b_opened && ( _censys_inst != INVALID_CENSYS_INSTANCE ) && !_b_censys3d_param_no_update )
	{
		cens_err = dll_censys.censysExtractorSetPointThreshold( _censys_inst, _point_threshold );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error setting point_threshold." );
		}
	}
}

void	c_capture_censys::set_proximity_filter_radius()
{
	CENSYS_ERROR	cens_err;

	if( _b_opened && ( _censys_inst != INVALID_CENSYS_INSTANCE ) && !_b_censys3d_param_no_update )
	{
		cens_err = dll_censys.censysExtractorSetProximityFilterRadius( _censys_inst, _proximity_filter_radius );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error setting proximity_filter_radius." );
		}
	}
}

void	c_capture_censys::set_min_height_drop_between_people()
{
	CENSYS_ERROR	cens_err;

	if( _b_opened && ( _censys_inst != INVALID_CENSYS_INSTANCE ) && !_b_censys3d_param_no_update )
	{
		cens_err = dll_censys.censysExtractorSetMinHeightDropBetweenPeople( _censys_inst, _minimum_height_drop );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error setting min_height_drop_between_people." );
		}
	}
}

void	c_capture_censys::set_max_valid_person_velocity()
{
	CENSYS_ERROR	cens_err;

	if( _b_opened && ( _censys_inst != INVALID_CENSYS_INSTANCE ) && !_b_censys3d_param_no_update )
	{
		cens_err = dll_censys.censysTrackerSetMaxValidPersonVelocity( _censys_inst, _max_valid_person_velocity );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error setting max_valid_person_velocity." );
		}
	}
}

void	c_capture_censys::set_frames_before_deleting_person()
{
	CENSYS_ERROR	cens_err;

	if( _b_opened && ( _censys_inst != INVALID_CENSYS_INSTANCE ) && !_b_censys3d_param_no_update )
	{
		cens_err = dll_censys.censysTrackerSetFramesBeforeDeletingPerson( _censys_inst, _frames_before_deleting_person );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error setting frames_before_deleting_person." );
		}
	}
}

void	c_capture_censys::set_min_path_points()
{
	CENSYS_ERROR	cens_err;

	if( _b_opened && ( _censys_inst != INVALID_CENSYS_INSTANCE ) && !_b_censys3d_param_no_update )
	{
		cens_err = dll_censys.censysTrackerSetMinPathPoints( _censys_inst, _minimum_path_points );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error setting min_path_points." );
		}
	}
}

void	c_capture_censys::set_max_velocity_angle()
{
	CENSYS_ERROR	cens_err;

	if( _b_opened && ( _censys_inst != INVALID_CENSYS_INSTANCE ) && !_b_censys3d_param_no_update )
	{
		cens_err = dll_censys.censysTrackerSetMaxVelocityAngle( _censys_inst, _maximum_velocity_angle );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error setting max_velocity_angle." );
		}
	}
}

void	c_capture_censys::set_frames_before_deleting_candidate_person()
{
	CENSYS_ERROR	cens_err;

	if( _b_opened && ( _censys_inst != INVALID_CENSYS_INSTANCE ) && !_b_censys3d_param_no_update )
	{
		cens_err = dll_censys.censysTrackerSetFramesBeforeDeletingCandidatePerson( _censys_inst, _frames_before_deleting_candidate_person );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error setting frames_before_deleting_candidate_person." );
		}
	}
}

void	c_capture_censys::set_frames_before_deleting_person_in_border()
{
	CENSYS_ERROR	cens_err;

	if( _b_opened && ( _censys_inst != INVALID_CENSYS_INSTANCE ) && !_b_censys3d_param_no_update )
	{
		cens_err = dll_censys.censysTrackerSetFramesBeforeDeletingPersonInBorder( _censys_inst, _frames_before_deleting_person_in_border );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error setting frames_before_deleting_person_in_border." );
		}
	}
}

void	c_capture_censys::set_min_path_points_penalty()
{
	CENSYS_ERROR	cens_err;

	if( _b_opened && ( _censys_inst != INVALID_CENSYS_INSTANCE ) && !_b_censys3d_param_no_update )
	{
		cens_err = dll_censys.censysTrackerSetMinPathPointsPenalty( _censys_inst, _min_path_points_penalty );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error setting min_path_points_penalty." );
		}
	}
}

void	c_capture_censys::reset_background()
{
	CENSYS_ERROR	cens_err;

	if( _b_opened && ( _censys_inst != INVALID_CENSYS_INSTANCE ) && !_b_censys3d_param_no_update )
	{
		cens_err = dll_censys.censysSegmentorResetBackground( _censys_inst );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error resetting background." );
		}
	}
}

// print censys 3d settings for debug information
void	c_capture_censys::print_settings()
{
	if( _b_opened && ( _censys_inst != INVALID_CENSYS_INSTANCE ) )
	{
		CENSYS_ERROR	cens_err;

		CENSYS_PRINT_STRING( "Censys 3d settings :" );

		INT32	size_x, size_y;
		cens_err = dll_censys.censysSystemGetResolution( _censys_inst, &size_y, &size_x );
		if( cens_err != censyserrorSUCCESS )
		{
			ERR_PRINT_STRING( "Censys 3d, getting system resolution failed." );
		}
		else
		{
			CENSYS_PRINT_STRING( "\tSystem Resolution : %dx%d.", size_x, size_y );
		}

		DOUBLE	dPositionX, dposition_y, dposition_z, drotation_x, drotation_y, drotation_z;
		cens_err = dll_censys.censysSegmentorGetTransformation( _censys_inst, &dPositionX, &dposition_y, &dposition_z, &drotation_x, &drotation_y, &drotation_z );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error getting transformation." );
		}
		else
		{
			CENSYS_PRINT_STRING( "\tPosition X : %f\n\tPosition Y : %f\n\tPosition Z : %f\n\tRotation X : %f\n\tRotation Y : %f\n\tRotation Z : %f", dPositionX, dposition_y, dposition_z, drotation_x, drotation_y, drotation_z );
		}

		DOUBLE	dMinDistance, dMaxDistance;
		cens_err = dll_censys.censysSystemGetMinMaxZDistance( _censys_inst, 1, &dMinDistance, &dMaxDistance);
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error getting min_max_z_distance." );
		}
		else
		{
			CENSYS_PRINT_STRING( "\tMinimum Z range : %f\n\tMaximum Z range ; %f", dMinDistance, dMaxDistance );
		}

		DOUBLE	pDistance;
		cens_err = dll_censys.censysSegmentorGetDepthThreshold( _censys_inst, &pDistance);
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error getting depth_threshold." );
		}
		else
		{
			CENSYS_PRINT_STRING( "\tdepth_threshold : %f", pDistance);
		}

		DOUBLE	pCreepRate;
		cens_err = dll_censys.censysSegmentorGetCreepTime( _censys_inst, &pCreepRate);
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error getting creep_rate." );
		}
		else
		{
			CENSYS_PRINT_STRING( "\tcreep_rate : %f", pCreepRate);
		}

		DOUBLE	dMinX, dMaxX, dMinY, dMaxY, dTide, dMaxHeight;
		cens_err = dll_censys.censysExtractorGetVOIBounds( _censys_inst, &dMinX, &dMaxX, &dMinY, &dMaxY, &dTide, &dMaxHeight);
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys : error getting VOIBounds." );
		}
		else
		{
			CENSYS_PRINT_STRING( "\tMin X     : %f\n\tMax X     : %f\n\tMin Y     : %f\n\tMax Y     : %f\n\tTide      : %f\n\tMaxHeight : %f", dMinX, dMaxX, dMinY, dMaxY, dTide, dMaxHeight );
		}

		DOUBLE	dResolution;
		cens_err = dll_censys.censysExtractorGetGranularity( _censys_inst, &dResolution);
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error getting granularity." );
		}
		else
			CENSYS_PRINT_STRING( "\tgranularity : %f", dResolution);

		INT32	nThreshold;
		cens_err = dll_censys.censysExtractorGetPointThreshold( _censys_inst, &nThreshold );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error getting point_threshold." );
		}
		else
		{
			CENSYS_PRINT_STRING( "\tpoint_threshold : %d", nThreshold );
		}

		DOUBLE	dRadius;
		cens_err = dll_censys.censysExtractorGetProximityFilterRadius( _censys_inst, &dRadius );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error getting proximity_filter_radius." );
		}
		else
		{
			CENSYS_PRINT_STRING( "\tproximity_filter_radius : %f", dRadius );
		}

		DOUBLE	dHeightDrop;
		cens_err = dll_censys.censysExtractorGetMinHeightDropBetweenPeople( _censys_inst, &dHeightDrop );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error getting min_height_drop_between_people." );
		}
		else
		{
			CENSYS_PRINT_STRING( "\tmin_height_drop_between_people : %f", dHeightDrop );
		}

		DOUBLE	pMaxValidVelocity;
		cens_err = dll_censys.censysTrackerGetMaxValidPersonVelocity( _censys_inst, &pMaxValidVelocity );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error getting max_valid_person_velocity." );
		}
		else
		{
			CENSYS_PRINT_STRING( "\tmax_valid_person_velocity : %f", pMaxValidVelocity);
		}

		INT32	pFrames;
		cens_err = dll_censys.censysTrackerGetFramesBeforeDeletingPerson( _censys_inst, &pFrames );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error getting frames_before_deleting_person." );
		}
		else
		{
			CENSYS_PRINT_STRING( "\tframes_before_deleting_person : %d", pFrames );
		}

		INT32	nNumPoints;
		cens_err = dll_censys.censysTrackerGetMinPathPoints( _censys_inst, &nNumPoints );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error getting min_path_points." );
		}
		else
		{
			CENSYS_PRINT_STRING( "\tmin_path_points : %d", nNumPoints );
		}

		INT32	nPointsPenalty;
		cens_err = dll_censys.censysTrackerGetMinPathPointsPenalty( _censys_inst, &nPointsPenalty );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error getting min_path_points_penalty." );
		}
		else
		{
			CENSYS_PRINT_STRING( "\tmin_path_points_penalty : %d", nPointsPenalty );
		}

		DOUBLE	dMaxVelocityAngle;
		cens_err = dll_censys.censysTrackerGetMaxVelocityAngle( _censys_inst, &dMaxVelocityAngle );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error getting max_velocity_angle." );
		}
		else
		{
			CENSYS_PRINT_STRING( "\tmax_velocity_angle : %f", dMaxVelocityAngle );
		}

		INT32	nFrames;
		cens_err = dll_censys.censysTrackerGetFramesBeforeDeletingCandidatePerson( _censys_inst, &nFrames );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error getting frames_before_deleting_candidate_person." );
		}
		else
		{
			CENSYS_PRINT_STRING( "\tframes_before_deleting_candidate_person : %d", nFrames );
		}

		cens_err = dll_censys.censysTrackerGetFramesBeforeDeletingPersonInBorder( _censys_inst, &nFrames );
		if( cens_err != censyserrorSUCCESS || _censys_inst == INVALID_CENSYS_INSTANCE )
		{
			ERR_PRINT_STRING( "Censys 3d, error getting frames_before_deleting_person_in_border." );
		}
		else
		{
			CENSYS_PRINT_STRING( "\tframes_before_deleting_person_in_border : %d", nFrames );
		}

		_bumblebee_settings_0->print_settings();
		_bumblebee_settings_1->print_settings();
	}
}

void	c_capture_censys::do_got_frame( UINT32 index_flux, CENSYS3D_IMAGE_TYPE image_type, PSEGMENTOR_CAMERA_DATA camera_data )
{
	//st_frame_info	frame_info;
	CONST UINT8*		src		= nullptr;
	aaa::PIXEL_FORMAT	format	= aaa::PIXEL_FORMAT::DISP_16;
	UINT32	size_x, size_y;
	switch ( image_type )
	{
	case IMAGE_SCENE_DISPARITY:
		size_x	= camera_data->image_scene_disparity.width;
		size_y	= camera_data->image_scene_disparity.height;
		src		= (CONST UINT8*)camera_data->image_scene_disparity.data.pBuffer;
		break;
	case IMAGE_FORE_DISPARITY:
		size_x	= camera_data->image_foreground_disparity.width;
		size_y	= camera_data->image_foreground_disparity.height;
		src		= (CONST UINT8*)camera_data->image_foreground_disparity.data.pBuffer;
		break;
	case IMAGE_BACK_DISPARITY:
		size_x	= camera_data->image_background_disparity.width;
		size_y	= camera_data->image_background_disparity.height;
		src		= (CONST UINT8*)camera_data->image_background_disparity.data.pBuffer;
		break;
	case IMAGE_SCENE:
		format	= aaa::PIXEL_FORMAT::RGBA_8;
		size_x	= camera_data->image_scene.width;
		size_y	= camera_data->image_scene.height;
		src		= (CONST UINT8*)camera_data->image_scene.data.pBuffer;
		break;
	case IMAGE_FOREGROUND:
		format	= aaa::PIXEL_FORMAT::RGBA_8;
		size_x	= camera_data->image_foreground.width;
		size_y	= camera_data->image_foreground.height;
		src		= (CONST UINT8*)camera_data->image_foreground.data.pBuffer;
		break;
	case IMAGE_BACKGROUND:
		format	= aaa::PIXEL_FORMAT::RGBA_8;
		size_x	= camera_data->image_background.width;
		size_y	= camera_data->image_background.height;
		src		= (CONST UINT8*)camera_data->image_background.data.pBuffer;
		break;
	}
	//set_src_pixel_format( pixel_format );
	//set_flux_size_format( size_x, size_y, pixel_format );
	//	frame_info.pixel_format = pixel_format;
	//frame_info.size_x = pData->camera_data_array[ camera_index ].image_background.width;
	//	frame_info.size_y = 
	//		frame.src = src;
	switch( index_flux )
	{
	case 0 :
		set_flux_size_format( size_x, size_y, format );
		got_frame( src, "Censys image", 0, aaa::c_pixel_format::has_alpha(format) );
		break;
	case 1 :
		_image_flux_2->set_flux_size_format( size_x, size_y, format );
		_image_flux_2->got_frame( src, "Censys image 2", 0, aaa::c_pixel_format::has_alpha(format) );
		break;
	case 2 :
		_image_flux_3->set_flux_size_format( size_x, size_y, format );
		_image_flux_3->got_frame( src, "Censys image 3", 0, aaa::c_pixel_format::has_alpha(format) );
		break;
	}
}

//* Grabber layer callback function
void	__cdecl	c_capture_censys::callback_grabber( PGRABBER_DATA pData, void* pArg  )
{
//		DBG_PRINT_STRING("grabber callback");
}

//* Segmentor layer callback function
void	__cdecl	c_capture_censys::callback_segmentor( PSEGMENTOR_DATA pData, void* pArg )
{
	CENSYS3D_IMAGE_TYPE	image_type;
	INT32				camera_index;
	DBG_PRINT_STRING("segmentor callback");
	c_capture_censys*	p_cap = capture_censys_find_by_window_hd( 0 );

	if( p_cap->_b_running && pData->num_cameras > 0 )	//maa opened or running ?
	{
		// capture is open, setup cameras
		if( !p_cap->_b_init_cam )
		{
			p_cap->set_b_init_cam( true );
			p_cap->init_cam();
		}
		if( p_cap->_camera_id != 0 )
		{
			TBUF_INC( tbuf::CH_CAPTURE_CALLBACK, 1., "censys" );
			camera_index = p_cap->_camera_id - 1;
			if( camera_index == 1 )
			{
				image_type = p_cap->_bumblebee_settings_1->get_image_type();
			}
			else
			{
				image_type = p_cap->_bumblebee_settings_0->get_image_type();
				camera_index = 0;
			}
			//pData->camera_data_array[ camera_index ].image_scene_disparity
//			UINT8*		src				= nullptr;
			//st_frame_info	frame_info;
			//frame_info.src			= nullptr;
			//frame_info.pitch		= 0;
			//frame_info.pixel_format	=PIXEL_FORMAT::DISP_16;
			//switch( image_type )
			//{
			//case IMAGE_SCENE_DISPARITY :
			//	frame_info.size_x		= pData->camera_data_array[ camera_index ].image_scene_disparity.width;
			//	frame_info.size_y		= pData->camera_data_array[ camera_index ].image_scene_disparity.height;
			//	frame_info.src			= (UINT8*)pData->camera_data_array[ camera_index ].image_scene_disparity.data.pBuffer;
			//	break;
			//case IMAGE_FORE_DISPARITY :
			//	frame_info.size_x		= pData->camera_data_array[ camera_index ].image_foreground_disparity.width;
			//	frame_info.size_y		= pData->camera_data_array[ camera_index ].image_foreground_disparity.height;
			//	frame_info.src			= (UINT8*)pData->camera_data_array[ camera_index ].image_foreground_disparity.data.pBuffer;
			//	break;
			//case IMAGE_BACK_DISPARITY :
			//	frame_info.size_x		= pData->camera_data_array[ camera_index ].image_background_disparity.width;
			//	frame_info.size_y		= pData->camera_data_array[ camera_index ].image_background_disparity.height;
			//	frame_info.src			= (UINT8*)pData->camera_data_array[ camera_index ].image_background_disparity.data.pBuffer;
			//	break;
			//case IMAGE_SCENE: 
			//	frame_info.pixel_format	=PIXEL_FORMAT::RGBA;
			//	frame_info.size_x		= pData->camera_data_array[ camera_index ].image_scene.width;
			//	frame_info.size_y		= pData->camera_data_array[ camera_index ].image_scene.height;
			//	frame_info.src			= (UINT8*)pData->camera_data_array[ camera_index ].image_scene.data.pBuffer;
			//	break;
			//case IMAGE_FOREGROUND :
			//	frame_info.pixel_format =PIXEL_FORMAT::RGBA;
			//	frame_info.size_x		= pData->camera_data_array[ camera_index ].image_foreground.width;
			//	frame_info.size_y		= pData->camera_data_array[ camera_index ].image_foreground.height;
			//	frame_info.src			= (UINT8*)pData->camera_data_array[ camera_index ].image_foreground.data.pBuffer;
			//	break;
			//case IMAGE_BACKGROUND :
			//	frame_info.pixel_format	=PIXEL_FORMAT::RGBA;
			//	frame_info.size_x		= pData->camera_data_array[ camera_index ].image_background.width;
			//	frame_info.size_y		= pData->camera_data_array[camera_index].image_background.height;
			//	frame_info.src			= (UINT8*)pData->camera_data_array[ camera_index ].image_background.data.pBuffer;
			//	break;
			//}
		//	p_cap->set_src_pixel_format( frame_info.pixel_format );

		//	frame_info.pixel_format = pixel_format;
			//frame_info.size_x = pData->camera_data_array[ camera_index ].image_background.width;
			//	frame_info.size_y = 
		//		frame.src = src;
			p_cap->do_got_frame( 0, image_type, &pData->camera_data_array[ camera_index ] );
			//p_cap->got_frame( frame_info, "Censys image", aaa::c_pixel_format::has_alpha(frame_info.pixel_format) );

			CENSYS3D_IMAGE_TYPE	image_2_type	= p_cap->_bumblebee_settings_0->get_image_2_type();
			INT32				bind_2			= p_cap->_bumblebee_settings_0->get_image_2_bind();

			CENSYS3D_IMAGE_TYPE	image_3_type	= p_cap->_bumblebee_settings_0->get_image_3_type();
			INT32				bind_3			= p_cap->_bumblebee_settings_0->get_image_3_bind();

		//	src				= nullptr;
		//	frame_info.pixel_format	=PIXEL_FORMAT::DISP_16;
		//	tex_2d_bind( bind_2 );
//			switch( image_2_type )
//			{
//			case IMAGE_SCENE_DISPARITY :
//				//p_cap->_b_src_grey = true;
//				//p_cap->set_src_bit_per_pixel( 16 );
//				//p_cap->got_frame( (UINT8*)pData->camera_data_array[camera_index].image_scene_disparity.data.pBuffer, PIXEL_FORMAT::DISP_16 );
//				frame_info.size_x = pData->camera_data_array[camera_index].image_scene_disparity.width;
//				frame_info.size_y = pData->camera_data_array[camera_index].image_scene_disparity.height;
//				frame_info.src = (UINT8*)pData->camera_data_array[camera_index].image_scene_disparity.data.pBuffer;
//				break;
//			case IMAGE_FORE_DISPARITY :
//				//p_cap->_b_src_grey = true;
//				//p_cap->set_src_bit_per_pixel( 16 );
//			//	p_cap->got_frame( (UINT8*)pData->camera_data_array[camera_index].image_foreground_disparity.data.pBuffer, PIXEL_FORMAT::DISP_16 );
//				frame_info.size_x = pData->camera_data_array[camera_index].image_background.width;
//				frame_info.size_y = pData->camera_data_array[camera_index].image_background.height;
//				frame_info.src = (UINT8*)pData->camera_data_array[ camera_index ].image_foreground_disparity.data.pBuffer;
//				break;
//			case IMAGE_BACK_DISPARITY :
//				//p_cap->_b_src_grey = true;
//				//p_cap->set_src_bit_per_pixel( 16 );
//			//	p_cap->got_frame( (UINT8*)pData->camera_data_array[camera_index].image_background_disparity.data.pBuffer, PIXEL_FORMAT::DISP_16 );
//				frame_info.size_x = pData->camera_data_array[camera_index].image_background.width;
//				frame_info.size_y = pData->camera_data_array[camera_index].image_background.height;
//				frame_info.src = (UINT8*)pData->camera_data_array[ camera_index ].image_background_disparity.data.pBuffer;
//				break;
//			case IMAGE_SCENE: 
//				//p_cap->_b_src_grey = false;
//				//p_cap->set_src_bit_per_pixel( 32 );
//			//	p_cap->got_frame( (UINT8*)pData->camera_data_array[camera_index].image_scene.data.pBuffer, PIXEL_FORMAT::RGB_32, true );
//				frame_info.size_x = pData->camera_data_array[camera_index].image_background.width;
//				frame_info.size_y = pData->camera_data_array[camera_index].image_background.height;
//				frame_info.pixel_format =PIXEL_FORMAT::RGBA;
//				frame_info.src = (UINT8*)pData->camera_data_array[ camera_index ].image_scene.data.pBuffer;
//				break;
//			case IMAGE_FOREGROUND :
//			//	p_cap->_b_src_grey = false;
//			//	p_cap->set_src_bit_per_pixel( 32 );
//				//p_cap->got_frame( (UINT8*)pData->camera_data_array[camera_index].image_foreground.data.pBuffer, PIXEL_FORMAT::RGB_32, true );
//				frame_info.size_x = pData->camera_data_array[camera_index].image_background.width;
//				frame_info.size_y = pData->camera_data_array[camera_index].image_background.height;
//				frame_info.pixel_format =PIXEL_FORMAT::RGBA;
//				frame_info.src = (UINT8*)pData->camera_data_array[ camera_index ].image_foreground.data.pBuffer;
//				break;
//			case IMAGE_BACKGROUND :
//			//	p_cap->_b_src_grey = false;
//			//	p_cap->set_src_bit_per_pixel( 32 );
//				frame_info.size_x = pData->camera_data_array[camera_index].image_background.width;
//				frame_info.size_y = pData->camera_data_array[camera_index].image_background.height;
//				pixel_format =PIXEL_FORMAT::RGBA;
////				p_cap->got_frame( (UINT8*)pData->camera_data_array[camera_index].image_background.data.pBuffer, PIXEL_FORMAT::RGB_32, true );
//				frame_info.src = (UINT8*)pData->camera_data_array[ camera_index ].image_background.data.pBuffer;
//				break;
//			}
			//p_cap->set_src_pixel_format( pixel_format );
			p_cap->do_got_frame( 1, image_2_type, &pData->camera_data_array[camera_index] );
			//p_cap->_image_flux_2->got_frame( src, "Censys image", c_pixel_format::has_alpha(pixel_format) );

		//	tex_2d_bind( bind_3 );
			//switch( image_3_type )
			//{
			//case IMAGE_SCENE_DISPARITY :
			////	p_cap->_b_src_grey = true;
			//	//p_cap->set_src_bit_per_pixel( 16 );
			////	p_cap->got_frame( (UINT8*)pData->camera_data_array[camera_index].image_scene_disparity.data.pBuffer, PIXEL_FORMAT::DISP_16 );
			//	frame_info.size_x = pData->camera_data_array[camera_index].image_background.width;
			//	frame_info.size_y = pData->camera_data_array[camera_index].image_background.height;
			//	frame_info.src = (UINT8*)pData->camera_data_array[ camera_index ].image_scene_disparity.data.pBuffer;
			//	break;
			//case IMAGE_FORE_DISPARITY :
			////	p_cap->_b_src_grey = true;
			////	p_cap->set_src_bit_per_pixel( 16 );
			////	p_cap->got_frame( (UINT8*)pData->camera_data_array[camera_index].image_foreground_disparity.data.pBuffer, PIXEL_FORMAT::DISP_16 );
			//	frame_info.size_x = pData->camera_data_array[camera_index].image_background.width;
			//	frame_info.size_y = pData->camera_data_array[camera_index].image_background.height;
			//	frame_info.src = (UINT8*)pData->camera_data_array[ camera_index ].image_foreground_disparity.data.pBuffer;
			//	break;
			//case IMAGE_BACK_DISPARITY :
			////	p_cap->_b_src_grey = true;
			////	p_cap->set_src_bit_per_pixel( 16 );
			////	p_cap->got_frame( (UINT8*)pData->camera_data_array[camera_index].image_background_disparity.data.pBuffer, PIXEL_FORMAT::DISP_16 );
			//	frame_info.size_x = pData->camera_data_array[camera_index].image_background.width;
			//	frame_info.size_y = pData->camera_data_array[camera_index].image_background.height;
			//	frame_info.src = (UINT8*)pData->camera_data_array[ camera_index ].image_background_disparity.data.pBuffer;
			//	break;
			//case IMAGE_SCENE: 
			////	p_cap->_b_src_grey = false;
			////	p_cap->set_src_bit_per_pixel( 32 );
			////	p_cap->got_frame( (UINT8*)pData->camera_data_array[camera_index].image_scene.data.pBuffer, PIXEL_FORMAT::RGB_32, true );
			//	frame_info.size_x = pData->camera_data_array[camera_index].image_background.width;
			//	frame_info.size_y = pData->camera_data_array[camera_index].image_background.height;
			//	frame_info.pixel_format =PIXEL_FORMAT::RGBA;
			//	frame_info.src = (UINT8*)pData->camera_data_array[ camera_index ].image_scene.data.pBuffer;
			//	break;
			//case IMAGE_FOREGROUND :
			////	p_cap->_b_src_grey = false;
			////	p_cap->set_src_bit_per_pixel( 32 );
			////	p_cap->got_frame( (UINT8*)pData->camera_data_array[camera_index].image_foreground.data.pBuffer, PIXEL_FORMAT::RGB_32, true );
			//	frame_info.size_x = pData->camera_data_array[camera_index].image_background.width;
			//	frame_info.size_y = pData->camera_data_array[camera_index].image_background.height;
			//	frame_info.pixel_format =PIXEL_FORMAT::RGBA;
			//	frame_info.src = (UINT8*)pData->camera_data_array[ camera_index ].image_foreground.data.pBuffer;
			//	break;
			//case IMAGE_BACKGROUND :
			////	p_cap->_b_src_grey = false;
			////	p_cap->set_src_bit_per_pixel( 32 );
			////	p_cap->got_frame( (UINT8*)pData->camera_data_array[camera_index].image_background.data.pBuffer, PIXEL_FORMAT::RGB_32, true );
			//	frame_info.size_x = pData->camera_data_array[camera_index].image_background.width;
			//	frame_info.size_y = pData->camera_data_array[camera_index].image_background.height;
			//	frame_info.pixel_format =PIXEL_FORMAT::RGBA;
			//	frame_info.src = (UINT8*)pData->camera_data_array[ camera_index ].image_background.data.pBuffer;
			//	break;
			//}
			//p_cap->set_src_pixel_format( pixel_format );
	//		p_cap->got_frame( src, "Censys image", aaa::c_pixel_format::has_alpha(pixel_format) );
			p_cap->do_got_frame( 2, image_3_type, &pData->camera_data_array[camera_index] );
//			p_cap->_image_flux_3->got_frame( src, "Censys image", aaa::c_pixel_format::has_alpha(pixel_format) );
			TBUF_DEC( tbuf::CH_CAPTURE_CALLBACK, 0., "censys" );
		}
	}
}

//* Extractor layer callback function
void	__cdecl	c_capture_censys::callback_extractor( PEXTRACTOR_DATA pData, void* pArg )
{
//	DBG_PRINT_STRING("extractor callback");
}

//* Tracker layer callback function
void	__cdecl	c_capture_censys::callback_tracker( PTRACKER_DATA pData, void* pArg )
{
//	DBG_PRINT_STRING("tracker callback");

	c_capture_censys*	p_cap = capture_censys_find_by_window_hd( 0 );

	c_censys_tracking *p_censys_tracking = p_cap->get_censys_tracking();

	p_cap->tracking_fps_new();

	if( IS_NOT_NULL( p_censys_tracking ) )
	{
		// todo crashing in release
		// Get a copy of the tracked people data
		if( !p_censys_tracking->set_censys_tracking( *pData ) )
		{
			ERR_PRINT_STRING( "censys_3d::callback_tracker - failed to get tracker data." );
		}
	}
}
#endif #if !AAA_WIN64()

c_censys_tracking*	c_capture_censys::get_censys_tracking()
{
	return &_m_censys_tracking;
}


FACTORY_CREATE_V1( c_censys_ui, censys_ui, Capture Censys 3D, cap_censys );

namespace n_censys
{
	CONSTEXPR INT32 BASE_PARAM_NB		= 7;
	CONSTEXPR INT32 GROUP_NB			= 5;
	CONSTEXPR INT32 DOT_PARAM_NB		= 11;
	CONSTEXPR INT32 GRABBER_PARAM_NB	= 2;
	CONSTEXPR INT32 SEGMENTOR_PARAM_NB	= 10; 
	CONSTEXPR INT32 EXTRACTOR_PARAM_NB	= 10; 
	CONSTEXPR INT32 TRACKER_PARAM_NB	= 7;
= 	CONSTEXPR INT32 PARAM_NB_MAX		=	BASE_PARAM_NB
										+	DOT_PARAM_NB
										+	GRABBER_PARAM_NB
										+	SEGMENTOR_PARAM_NB
										+	EXTRACTOR_PARAM_NB
										+	TRACKER_PARAM_NB
										+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_INT32(		camera_id,							1, 1,	0, 2	)
		PARAM_DEF_BOOL_OFF(		print_settings_trigger				)
		PARAM_DEF_BOOL_OFF(		reset_background_trigger			)
		PARAM_DEF_BOOL_OFF(		censys3d_param_no_update			)
		PARAM_DEF_INT32_LOCKED(	fps_tracking						)

		PARAM_DEF_GROUP_CLOSED( Dot, DOT_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		send							)
			PARAM_DEF_BOOL_OFF(		save							)
			PARAM_DEF_INT32(		speed,							1, 2, 2, 4 )
			PARAM_DEF_BOOL_OFF(		verbose_send					)
			PARAM_DEF_BOOL_OFF(		verbose_send_value				)
			PARAM_DEF_POINT_XYZ(	offset							)
			PARAM_DEF_SCALE_XYZ(	scale							)

		PARAM_DEF_NONE(			bumblebee_1						)
		PARAM_DEF_NONE(			bumblebee_2						)
		PARAM_DEF_GROUP_CLOSED( Grabber, GRABBER_PARAM_NB )
			PARAM_DEF_INT32(		stereo_resolution_x,			1, 320, 1, 640 )
			PARAM_DEF_INT32(		stereo_resolution_y,			1, 240, 1, 480 )
			//	PARAM_DEF_REAL_POS_ZERO(		grabber_(delay)"	)
		PARAM_DEF_GROUP_CLOSED( Segmentor, SEGMENTOR_PARAM_NB )
			PARAM_DEF_REAL_INF(		position_x,						1, 5	)
			PARAM_DEF_REAL_INF(		position_y,						1, 5	)
			PARAM_DEF_REAL_INF(		position_z,						1, 5	)
			PARAM_DEF_REAL_INF(		rotation_x,						1, 3.141593 )
			PARAM_DEF_REAL_INF(		rotation_y,						1, 0	)
			PARAM_DEF_REAL_INF(		rotation_z,						1, 0	)
			PARAM_DEF_REAL_POS(		z_distance_min,					1, 0.5	)
			PARAM_DEF_REAL_POS(		z_distance_max,					1, 10	)
			PARAM_DEF_REAL(			creep_rate,						1, 0.1,	0, 1 )
			PARAM_DEF_REAL_POS(		depth_threshold,				1, 0.5	)
		PARAM_DEF_GROUP_CLOSED( Extractor, EXTRACTOR_PARAM_NB )
			PARAM_DEF_INT32_POS(	point_threshold,				1, 25	)
			PARAM_DEF_REAL_POS(		granularity,					1, 0.1	)
			PARAM_DEF_REAL_POS(		proximity_filter_radius,		1, 1	)
			PARAM_DEF_REAL_POS(		minimum_height_drop,			1, 0.05	)
			PARAM_DEF_REAL_INF(		voi_x_min,						1, 0	)
			PARAM_DEF_REAL_INF(		voi_x_max,						1, 10	)
			PARAM_DEF_REAL_INF(		voi_y_min,						1, 0	)
			PARAM_DEF_REAL_INF(		voi_y_max,						1, 10	)
			PARAM_DEF_REAL_POS(		height_people_min,				1, 1	)
			PARAM_DEF_REAL_POS(		height_people_max,				1, 2.1	)
		PARAM_DEF_GROUP_CLOSED( Tracker, TRACKER_PARAM_NB )
			PARAM_DEF_INT32(		minimum_path_points,			1, 3,	1, PARAM_MAX_INT32 )
			PARAM_DEF_INT32_POS(	minimum_path_points_penalty,	1, 3	)
			PARAM_DEF_INT32_POS(	frames_b4_del_person,			1, 5	)
			PARAM_DEF_INT32_POS(	frames_b4_del_candidate_person,	1, 5	)
			PARAM_DEF_INT32_POS(	frames_b4_del_person_in_border,	1, 5	)
			PARAM_DEF_REAL_POS(		max_velocity,					1, 5	)
			PARAM_DEF_REAL_POS(		max_velocity_angle,				1, 20	)
		//	PARAM_DEF_REAL_POS(		tracker_(close_match_distance),	1,		0.2	)
		//	PARAM_DEF_REAL_POS(		tracker_(max_excursion),		1,		2	)
		//	PARAM_DEF_REAL_POS(		tracker_(match_tie_threshold),	1,		0	)
	};
}

void	c_censys_ui::param_init_pt()
{
INT32	h = 0;

	param_set_pt( h, _camera_id_ui );
	param_set_pt( h, _b_print_trigger_ui );
	param_set_pt( h, _b_reset_background_ui );
	param_set_pt( h, _b_censys3d_param_no_update_ui );

	param_set_pt( h, _tracking_fps_ui );
	++h;
		param_set_pt( h, _b_dot_send_ui );
		param_set_pt( h, _b_dot_save_ui );
		param_set_pt( h, _dot_speed_ui );
		param_set_pt( h, _b_dot_send_verbose_ui );
		param_set_pt( h, _b_dot_send_value_verbose_ui );
		param_set_pt( h, _dot_offset_x_ui );
		param_set_pt( h, _dot_offset_y_ui );
		param_set_pt( h, _dot_offset_z_ui );
		param_set_pt( h, _dot_scale_x_ui );
		param_set_pt( h, _dot_scale_y_ui );
		param_set_pt( h, _dot_scale_z_ui );

	param_attach_obj( h, (c_obj_ui*) &_bumblebee[0] );
	param_attach_obj( h, (c_obj_ui*) &_bumblebee[1] );

	++h;
		param_set_pt( h, _stereo_resolution_x_ui );
		param_set_pt( h, _stereo_resolution_y_ui );
//	param_set_pt( h, GrabDelay_ui;

	++h;
		param_set_pt( h, _position_x_ui );	//Camera transformation parameters
		param_set_pt( h, _position_y_ui );
		param_set_pt( h, _position_z_ui );
		param_set_pt( h, _rotation_x_ui );
		param_set_pt( h, _rotation_y_ui );
		param_set_pt( h, _rotation_z_ui );
		param_set_pt( h, _min_z_distance_ui );
		param_set_pt( h, _max_z_distance_ui );
		param_set_pt( h, _creep_rate_ui );
		param_set_pt( h, _depth_threshold_ui );

	++h;
		// extractor settings
		param_set_pt( h, _point_threshold_ui );
		param_set_pt( h, _granularity_ui );
		param_set_pt( h, _proximity_filter_radius_ui );
		param_set_pt( h, _minimum_height_drop_ui );
		param_set_pt( h, _voi_min_x_ui );
		param_set_pt( h, _voi_max_x_ui );
		param_set_pt( h, _voi_min_y_ui );
		param_set_pt( h, _voi_max_y_ui );
		param_set_pt( h, _min_height_people_ui );
		param_set_pt( h, _max_height_people_ui );

	++h;
		// tracker settings
		param_set_pt( h, _minimum_path_points_ui );
		param_set_pt( h, _min_path_points_penalty_ui );
		param_set_pt( h, _frames_before_deleting_person_ui );
		param_set_pt( h, _frames_before_deleting_candidate_person_ui );
		param_set_pt( h, _frames_before_deleting_person_in_border_ui );
		param_set_pt( h, _max_valid_person_velocity_ui );
		param_set_pt( h, _maximum_velocity_angle_ui );

		////experimental
		//param_set_pt( h, closeMatchDistance_ui;
		//param_set_pt( h, maxExcursion_ui;
		//param_set_pt( h, matchTieThreshold_ui;

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_censys_ui )
{
	param_init_with( n_censys::param, n_censys::PARAM_NB_MAX );

	_tracking_fps_ui = 0;

	_bumblebee[0].set_root( this );
	_bumblebee[1].set_root( this );
}
EMPTY_DESTRUCTOR(c_censys_ui)

c_capture_bumblebee*	c_censys_ui::get_bumblebee( INT32 index )
{
	switch( index )
		{
		case 1 :
			return &_bumblebee[1]._capture_bumblebee;
			break;
		case 0 :
		default :
			return &_bumblebee[0]._capture_bumblebee;
			break;
		}
}

static void	make_name( o_str& dst, o_str CONST & src, INT32 index )
{
	dst.set( src );
	dst.add_char( 'a' + index );
}

AAA_ERR	c_censys_ui::load_do_after( o_str CONST & filename_in )
{
	INT32	retcode;
	o_str& filename = o_str::push_name();
		for( INT32 i=0; i<2; ++i )
		{
			make_name( filename, filename_in, i );
			retcode = _bumblebee[i].load_from_file_add_ext( filename );
			if( ERR( retcode) )
				ERR_PRINT_STRING( "Error loading param for bumblebee_[%d] in %s().", i, __FUNCTION__ );
		}
	o_str::pop_name();
	return retcode;		//todo refine
}

AAA_ERR	c_censys_ui::save_do_after( o_str CONST & filename_in )
{
	INT32	retcode;
	o_str& filename = o_str::push_name();
		for( INT32 i=0; i<2; ++i )
		{
			make_name( filename, filename_in, i );
			retcode = _bumblebee[i].save_to_file_add_ext( filename );
			if( ERR( retcode) )
				ERR_PRINT_STRING( "Error saving param for bumblebee_[%d] in %s().", i, __FUNCTION__ );
		}
	o_str::pop_name();
	return AAA_OK;
}

#endif	//#if AAA_USE_POINT_GREY()