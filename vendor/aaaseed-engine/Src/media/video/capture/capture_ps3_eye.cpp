#undef WIN32_LEAN_AND_MEAN

#include "capture_ps3_eye.h"

#if AAA_USE_PS3EYE()
#	if !AAA_WIN64()
#		include "IPS3EyeLib.h"
#		include "PS3EyeMulticam.h"
#	endif
#	include "infrastructure/param/param_declare.h"
#endif

#if AAA_USE_PS3EYE()
bool	c_capture_ps3_eye::b_allow;

bool	c_capture_ps3_eye::b_dll_loaded = false;
UINT32	c_capture_ps3_eye::device_count = 0;

#include "wrap_PS3EyeMulticam.h"	// sr@20100409

/*
typedef enum
{
	R320x240,
	R640x480,
}PS3EYE_RESOLUTION;
*/


//#define MULTIPLE_CAM	1
FACTORY_CREATE_V1( c_ps3_eye_ui, ps3_eye_ui, PS3 USB Cam, ps3_eye_ui );

namespace n_ps3_eye_ui
{
	static	C_PCHAR_C	str_format[ R640x480 + 1 ] =
	{
		"320 x 240",
		"640 x 480"
	};

	CONSTEXPR INT32 BASE_PARAM_NB	= 11;
	CONSTEXPR INT32 GROUP_NB		= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_ON(			use_driver_multiple )	//the other trigger crashes now
		PARAM_DEF_INT32_LOCKED(		camera_nb_present	)
		PARAM_DEF_INT32(			camera_nb_asked,	2, 1, 1, PARAM_MAX_INT32 )
		PARAM_DEF_INT32_LOCKED(		camera_nb_used		)
		PARAM_DEF_BOOL_OFF(			color				)
		PARAM_DEF_SYMBO_PSTR_ZERO(	format,	str_format	)
		PARAM_DEF_INT32(			framerate,			100, 50, 1, 500 )
		PARAM_DEF_BOOL_ON(			wait				)
		PARAM_DEF_BOOL_ON(			led					)
		PARAM_DEF_BOOL_OFF(			setting_trig		)
		PARAM_DEF_BOOL_OFF(			allow_stop_close	)
	};
}

void	c_ps3_eye_ui::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, _b_driver_multiple_ui );
	param_set_pt( h, _camera_nb_present );
	param_set_pt( h, _camera_nb_asked );
	param_set_pt( h, _camera_nb_used );
	param_set_pt( h, _b_color_ui );
	param_set_pt( h, _s_format );
	param_set_pt( h, _framerate );
	param_set_pt( h, _b_wait_ui );
	param_set_pt( h, _b_led_ui );
	param_set_pt( h, _b_setting_trig_ui );
	param_set_pt( h, _b_allow_stop_close_ui );

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_ps3_eye_ui )
{
	param_init_with( n_ps3_eye_ui::param, n_ps3_eye_ui::PARAM_NB_MAX );
}
EMPTY_DESTRUCTOR(c_ps3_eye_ui)


//static	list<c_capture*>	list_cap_ps3_eye;
void c_capture_ps3_eye::do_enum_format()
{
//#ifndef MULTIPLE_CAM

	// Query supported video formats
	PS3EYE_FORMAT*	format = IPS3EyeLib::GetFormats();
	INT32			nb = IPS3EyeLib::GetNumFormats();
	for( INT32 i = 0; i < nb; ++i )
	{
		INT32	sx, sy, rate;
		CHAR*	description;
		sx = format[i].width;
		sy = format[i].height;
		rate = format[i].rate;
		description = format[i].formatTxt;
		// Display available formats �
		CAPTURE_PRINT_STRING( "\t- %d x %d at %d fps, %s", sx, sy, rate, description );
	}
}

INT32	c_capture_ps3_eye::do_enum( bool b_verbose )
{
	if( !b_allow )
	{
		ERR_PRINT_STRING( "Ps3Eye not allowed : goto to prefs (ctrl F10 then under tracker) to change this." );
		return 0;
	}

	INT32	nb = 0;
	if( !b_dll_loaded )
	{
		b_dll_loaded = dll_ps3eyemulticam.init();
		if( !b_dll_loaded )
		{
			return 0;
		}
//		INT32	err = wrap_PS3EyeMulticam_Init();
//		if( err != NO_ERROR )
//		{
////			ERR_PRINT_STRING( "Error loading PS3EyeMulticam.dll" );
//			return 0;
//		}
//		b_dll_loaded = true;
	}

	//	count camera

//#ifdef MULTIPLE_CAM
	nb = dll_ps3eyemulticam.PS3EyeMulticamGetCameraCount();
//#else
//	int nb = IPS3EyeLib::GetNumCameras();
//#endif

	if( b_verbose )
	{
		if( nb == 0 )
			ERR_PRINT_STRING( "%s() : No camera found", __FUNCTION__ );
		else
			CAPTURE_PRINT_STRING( "%s() : %d cameras found", __FUNCTION__, nb );
	}
//it is buggy	do_enum_format();
	device_count = nb;
	return nb;
}

void	c_capture_ps3_eye::c_init()
{
	do_enum( true );
}

void	c_capture_ps3_eye::c_deinit()
{
}

c_capture_ps3_eye::c_capture_ps3_eye()
	:_buffer_data			(	nullptr	)
	,_cap_size_x			(	0		)
	,_cap_size_y			(	0		)
	,_cap_framerate			(	0		)
	,_ps3_eye_ui			(	nullptr	)
	,_ps3_eye				(	nullptr	)		//	for open
//	,_b_started				(	false	)	//	for started
	,_b_driver_multiple		(	false	)
	,_b_led					(	false	)
	,_src_bit_per_pixel		(	0	)
{
	_b_stream = false;
	set_frame_callback( false );// this sync (waiting) for the moment
	_o_crossbar_name.set( "None" );
	//	clear_frame_ready();
}

c_capture_ps3_eye::~c_capture_ps3_eye()
{
	close();
//	obj_delete( _ps3_eye );	//	avoid trouble ?
}

void	c_capture_ps3_eye::ask_frame()
{
	//printf( "%s()", __FUNCTION__ );
	if( _b_running )
	{
		if( !_b_driver_multiple )
		{
			//	_ps3_eye->ColorBar( true );
			if ( !_ps3_eye->IsCapturing() )
			{
				CAPTURE_PRINT_STRING( "PS3 Eye is not capturing" );
				return;
			}
			//bool	is_color_bar;
			//is_color_bar = _ps3_eye->IsColorBar();
		}
		//if( set_size( _ps3_eye_ui->get_size_x(), _ps3_eye_ui->get_size_y(), _ps3_eye_ui->get_framerate() ) )
		//	init_with_size( _cap_size_x, _cap_size_y, 4, __FUNCTION__ );
		//printf("before\n");
		if( _buffer_data )
		{
			// This function will block until a new frame is available
			// It will then fill the buffer with frame image data
			bool	new_frame;
			TBUF_INC( tbuf::CH_CAPTURE_CALLBACK, 1., "ps3" );
			TBUF_INC( tbuf::CH_VIDEO_GET_FRAME, 1., "ps3_get_frame" );
			if( _b_driver_multiple )
				new_frame = dll_ps3eyemulticam.PS3EyeMulticamGetFrame( _buffer_data, (_ps3_eye_ui->is_wait()==TRUE)?2000:0 );
			else
				new_frame = _ps3_eye->GetFrame( _buffer_data, _src_bit_per_pixel, true, (_ps3_eye_ui->is_wait()==TRUE)?true:false );
			TBUF_DEC( tbuf::CH_VIDEO_GET_FRAME, 1., "ps3_get_frame" );
			//printf("after\n");
			if( new_frame )
			{
				// Process/display video frame here
				// �
				// If your program is done set �done=true;�	
				//_frame_info.size_x			= get_size_x();
				//_frame_info.size_y			= get_size_y();
				//_frame_info.src				= (UINT8*)_buffer_data;
				//_frame_info.pixel_format	= get_src_pixel_format();
				//_frame_info.pitch			= 0;
				got_frame( (UINT8*)_buffer_data, "ps3" );
			
			}
			TBUF_ADD( tbuf::CH_CAPTURE_CALLBACK, 0., "ps3" );
		}
	}
}

void	c_capture_ps3_eye::update()
{
	//mem::is_all_ok( "before" );
	IF_THIS_NULL_RETURN();
	
	if( _b_running )
	{
		if( _b_driver_multiple )
		{
			if( _ps3_eye_ui->_b_setting_trig_ui )
			{
				_ps3_eye_ui->_b_setting_trig_ui = false;
				dll_ps3eyemulticam.PS3EyeMulticamShowSettings();
			}
		}
		else
		{
			if( _b_led != _ps3_eye_ui->is_led() )
			{
				_b_led = _ps3_eye_ui->is_led();
				_ps3_eye->LED( _b_led );
			}
		}
		ask_frame();
	}		
	//mem::is_all_ok( "after" );
}

AAA_ERR	c_capture_ps3_eye::open_driver( INT32 index )
{
	int nb = do_enum( true );
	if( nb == 0 )
		return ERR_ANY;
	//printf( "%s()", __FUNCTION__ );
	//if( _ps3_eye )
	//{
	//	CAPTURE_PRINT_STRING( "%s() : PS3 Eye Driver already opened", __FUNCTION__ );
	//	return AAA_OK;
	//}
	//	count camera
	//	Create PS3EyeLib object
	if ( !_ps3_eye )
		_ps3_eye = IPS3EyeLib::Create();
	if ( _ps3_eye == nullptr )
	{
		ERR_PRINT_STRING( "%s() can't open PS3 Eye driver",  __FUNCTION__ );
		return ERR_ANY;
	}
	set_flux_name( "PS3 Eye" );
	CAPTURE_PRINT_STRING( "%s() : PS3 Eye Driver opened", __FUNCTION__ );
	return AAA_OK;
}

//capSetUserData()
bool	c_capture_ps3_eye::set_size()
{
	INT32 framerate = _ps3_eye_ui->get_framerate();
//	if( size_x != _cap_size_x || size_y != _cap_size_y || framerate != _cap_framerate )
	{
		INT32	byte_per_pixel;
		if( _b_driver_multiple )
		{
			dll_ps3eyemulticam.PS3EyeMulticamGetFrameDimensions( _cap_size_x, _cap_size_y );
			_cap_framerate = framerate;
			byte_per_pixel = 1;
		}
		else
		{
			INT32 size_x = _ps3_eye_ui->get_format()?640:320;
			INT32 size_y = _ps3_eye_ui->get_format()?480:240;
			_ps3_eye->SetFormat( IPS3EyeLib::GetFormatIndex( size_x, size_y, framerate ) );
			_cap_size_x = _ps3_eye->GetWidth();
			_cap_size_y = _ps3_eye->GetHeight();
			_cap_framerate = _ps3_eye->GetRate();
			if( size_x!=_cap_size_x || size_y!=_cap_size_y || framerate!=_cap_framerate )
			{
				ERR_PRINT_STRING( "PS3eye Can't set to %d x %d at %d frame", size_x, size_y, framerate );
				FREE_ALIGNED_AND_NULL( _buffer_data );
				//ERR_PRINT_STRING( "PS3eye		instead set it to %d x %d at %d frame", _cap_size_x, _cap_size_y, _cap_framerate );
			}
			else
			{
				GOOD_PRINT_STRING( "PS3eye set to %d x %d at %d frame", size_x, size_y, framerate );
			}
			if( _ps3_eye_ui->is_color() )
				byte_per_pixel = 3;
			else
				byte_per_pixel = 1;
		}
		aaa::PIXEL_FORMAT	format = aaa::PIXEL_FORMAT::RGB_8;
		if( byte_per_pixel == 1 )
		{
			_src_bit_per_pixel = 8;
			format = aaa::PIXEL_FORMAT::R_8;
		}
		else
		{
			_src_bit_per_pixel = 24;
			//set_src_pixel_format( PIXEL_FORMAT::RGB );
		}
		// Allocate image buffer (we are going to capture 24bit RGB images)
		// The supported color depths are 16, 24 and 32
		INT32 pixel_nb = _cap_size_x * _cap_size_y;
		_buffer_data = (PBYTE) REALLOC_ALIGNED( _buffer_data, pixel_nb * byte_per_pixel, 0 );
		//init_with_size( _cap_size_x, _cap_size_y,	(_src_bit_per_pixel==8 || _s_force_in_format==FORCE_IN_R_8)?1:4, __FUNCTION__ );
//		init_with_size( _cap_size_x, _cap_size_y,	(get_src_bit_per_pixel()==8) ? 1 : 4, __FUNCTION__ );
	//	PIXEL_TYPE	type = ( byte_per_pixel == 1 ) ? PIXEL_TYPE::GREY : PIXEL_TYPE::RGBA;	

		if( _buffer_data )
		{
			set_flux_size_format( _cap_size_x, _cap_size_y, format );
			MEMSET( _buffer_data, 128, pixel_nb*byte_per_pixel );
			return true;
		}
		else
		{
			set_flux_size_format( 0,0, format );
			return false;
		}
	}
	return true;
}

AAA_ERR	c_capture_ps3_eye::open_specific( INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_name )
{
	//printf("%s()", __FUNCTION__);
	if( !_b_opened )
	{
		_b_driver_multiple = _ps3_eye_ui->is_driver_multiple();
		bool	b_open;
		INT32	cam_nb;
		_ps3_eye_ui->_camera_nb_present = do_enum( true );

		if( _b_driver_multiple )
		{
			set_src_y_inverted( true );
			cam_nb = MIN( _ps3_eye_ui->_camera_nb_asked, _ps3_eye_ui->_camera_nb_present );
			b_open =  (cam_nb>=1) && dll_ps3eyemulticam.PS3EyeMulticamOpen( cam_nb, _ps3_eye_ui->get_format()?VGA:QVGA, _ps3_eye_ui->get_framerate() );
			
		}
		else
		{
			set_src_y_inverted( false );
			cam_nb = 1;
			b_open =  NOERR( open_driver(0) );
		}

		_ps3_eye_ui->_camera_nb_used =  b_open ? cam_nb : 0;
		if( b_open )
		{
			// Decide on the format to use (Example: 320�240 * 30fps)
			// Select this format
			//_o_video_name.set( "PS3 Eye" );
			_b_opened = set_size();
			if( _b_opened )
				CAPTURE_PRINT_STRING( "%s() done", __FUNCTION__ );
		}
	}
	return _b_opened ? AAA_OK : ERR_ANY;
}

void	c_capture_ps3_eye::close_low()
{
	if( _b_opened )
	{
		CAPTURE_PRINT_STRING( "%s() begin", __FUNCTION__ );
		_b_opened = false;
		if( _b_driver_multiple )
			dll_ps3eyemulticam.PS3EyeMulticamClose();
		else
		{
			//mem::is_all_ok( "stop middle" );
			//	stop_callback();
			//	disconnect_driver();
		}
		FREE_ALIGNED_AND_NULL( _buffer_data );
		CAPTURE_PRINT_STRING( "%s() done", __FUNCTION__ );
	}
}

void	c_capture_ps3_eye::close_specific()
{

	if( _b_opened && (_b_driver_multiple || _ps3_eye_ui->_b_allow_stop_close_ui) )	//hack	maa : should not be there but avoid crash 
	{
		stop();
		CAPTURE_PRINT_STRING( "%s() begin", __FUNCTION__ );
		close_low();
		CAPTURE_PRINT_STRING( "%s() done", __FUNCTION__ );
	}
}

bool	c_capture_ps3_eye::run( bool b_stream_in )
{
	if( _b_opened && !_b_running )
	{
		//	printf("%s()", __FUNCTION__);
		//	Start capturing
		bool	b_run = false;
		if( _b_driver_multiple )
			b_run = dll_ps3eyemulticam.PS3EyeMulticamStart();
		else
		{
			if( _ps3_eye )
			{
				_ps3_eye->StartCapture();
				b_run = _ps3_eye->IsCapturing();
			}
		}
		if( b_run )
		{
			_b_running = true;
			ask_frame();
			CAPTURE_PRINT_STRING( "%s() done", __FUNCTION__ );
		}
		else
		{
			err_print( "can't run" );
		}
	}
	return _b_running;
}

void	c_capture_ps3_eye::stop()
{
	if( _b_running && (_b_driver_multiple || _ps3_eye_ui->_b_allow_stop_close_ui) )	//hack	maa : should not be there but avoid crash 
	{
		//mem::is_all_ok( "stop before" );
		_b_running = false;
		if( _b_driver_multiple )
			dll_ps3eyemulticam.PS3EyeMulticamStop();
		else
		{
			if ( _ps3_eye )
			{
				//now	_ps3_eye->LED( false );
				_ps3_eye->StopCapture();
				CAPTURE_PRINT_STRING( "%s() stop done waiting", __FUNCTION__ );
				while( _ps3_eye->IsCapturing() );
				CAPTURE_PRINT_STRING( "%s() stopped", __FUNCTION__ );
			}
		}
		//close_low();
		//mem::is_all_ok( "stop end" );
	}
}

#endif	//#if AAA_USE_PS3EYE()