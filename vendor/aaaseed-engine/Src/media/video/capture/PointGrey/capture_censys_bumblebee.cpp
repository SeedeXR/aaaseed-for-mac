#include "ptgrey_utils.h"

#if AAA_USE_POINT_GREY()

#include "capture_censys_bumblebee.h"
#include "infrastructure/param/param_declare.h"
#include "wrap_PGRFlyCaptureGui.h"
#include "wrap_triclops.h"
#include "wrap_censys.h"
#ifndef AAA_WRAP_DIGICLOPS_H
#	include "wrap_digiclops.h"
#endif
		 
#include "obj_ui/tracker/trackers.h"

//extern c_PGRFlyCaptureGui_dll	dll_flycap_gui;
//extern c_triclops_dll			dll_triclops;
//extern	c_censys_dll			dll_censys;

#define	CENSYS_HEADER  "# CENSYS "

void	CENSYS_PRINT_STRING( C_PCHAR_C fmt, ...)
{
	va_list args;
	va_start( args, fmt );
	trackers::PRINT_STRING_VA( CENSYS_HEADER, fmt, args );
	va_end(args);
}

FACTORY_CREATE_V1( c_bumblebee, bumblebee, Bumblebee, bumblebee );

// manage different camera in censys 3d library
static	C_PCHAR_C	censys_camera_image_str[ CENSYS3D_IMAGE_MAX_NB ] =
{
	"Scene Disparity Image",
	"Foreground Disparity Image",	  
	"Background Disparity Image",
	"Scene Image",
	"Foreground Image",
	"Background Image",
};

static	C_PCHAR_C	censys_frame_rate_str[ CENSYS_FRAMERATE_MAX_NB ] =
{
	"100 %",
	"50 %",
	"25 %",
	"12 %",
};

#if !AAA_WIN64()
static	C_PCHAR_C	censys_camera_property[DIGICLOPS_PROPERTY_TOTAL] = 
{
	"Auto Exposure",
	"Shutter",
	"Gain",
	"Delta Gain Top/Right",
	"Delta Gain Left/Right",
	"Auto Gain Low",
	"Auto Gain High",
	"Auto Shutter Low",
	"Auto Shutter High",
	"Software White Balance",
	"Hardware White Balance",
};
#endif	//#if !AAA_WIN64()

static	C_PCHAR_C	censys_stereo_quality[TRICLOPS_STEREO_QUALITY_MAX_NB] = 
{
	"Standard",
	"Enhanced",
};

namespace n_bumblebee
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 23;
	CONSTEXPR INT32	GROUP_NB		= 0;

	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_INT32_LOCKED(		serial_number )
		PARAM_DEF_INT32_LOCKED(		camera_id )
		PARAM_DEF_SYMBO_PSTR_ZERO(	grabber_frame_rate,		censys_frame_rate_str )
		
		PARAM_DEF_SYMBO_PSTR_ZERO(	image_type,			censys_camera_image_str )
		PARAM_DEF_SYMBO_PSTR_ZERO(	image_2_type,		censys_camera_image_str )
		PARAM_DEF_BIND_2D_ALONE(	image_2_bind )
		PARAM_DEF_SYMBO_PSTR_ZERO(	image_3_type,		censys_camera_image_str )
		PARAM_DEF_BIND_2D_ALONE(	image_3_bind )

		PARAM_DEF_BOOL_OFF_SAVE_NOT(	camera_property_dialog )
		PARAM_DEF_INT32(				camera_exposure, 1, 396., 1., 1023. )
		PARAM_DEF_BOOL_OFF(				camera_exposure_auto )
		PARAM_DEF_INT32(				camera_shutter, 1, 356, 2, 533 )
		PARAM_DEF_BOOL_OFF(				camera_shutter_auto )
		PARAM_DEF_INT32(				camera_gain, 1, 737., 220., 1020 )
		PARAM_DEF_BOOL_OFF(				camera_gain_auto )
		PARAM_DEF_SYMBO_PSTR_ZERO(		stereo_quality,		censys_stereo_quality )
		PARAM_DEF_BOOL_OFF(				sub_pixel_interpolation )
		PARAM_DEF_REAL_POS(				segmentor_delta_disparity, 1., 0.1 )
		PARAM_DEF_INT32(				segmentor_edge_mask, 1, 5, 0, 15 )
		PARAM_DEF_INT32(				segmentor_stereo_mask, 1, 11, 0, 23 )
		PARAM_DEF_INT32_POS(			segmentor_segmentation_value, 1, 200 )
		PARAM_DEF_REAL(					segmentor_texture_validation_threshold, 1., 1.0, 0., 128. )
		PARAM_DEF_REAL(					segmentor_uniqueness_validation_threshold, 1., 1.0, 0., 10. )
	};
}

void	c_bumblebee::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, _serial_number_ui );
	param_set_pt( h, _camera_id_ui );
	param_set_pt( h, _frame_rate_ui );
	param_set_pt( h, _image_type_ui );

	param_set_pt( h, _image_2_type_ui );
	param_set_pt( h, _image_2_bind_ui );

	param_set_pt( h, _image_3_type_ui );
	param_set_pt( h, _image_3_bind_ui  );

	param_set_pt( h, _b_camera_dialog_ui );
	param_set_pt( h, _exposure_ui );
	param_set_pt( h, _b_exposure_auto_ui );
	param_set_pt( h, _shutter_ui );
	param_set_pt( h, _b_shutter_auto_ui );
	param_set_pt( h, _gain_ui );
	param_set_pt( h, _b_gain_auto_ui );

	param_set_pt( h, _b_sub_pixel_interpolation_ui );
	param_set_pt( h, _stereo_quality_ui );

	param_set_pt( h, _delta_disparity_ui );
	param_set_pt( h, _edge_mask_ui );
	param_set_pt( h, _stereo_mask_ui );
	param_set_pt( h, _segmentation_value_ui );
	param_set_pt( h, _texture_validation_threshold_ui );
	param_set_pt( h, _uniqueness_validation_threshold_ui );

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bumblebee )
{
	param_init_with( n_bumblebee::param, n_bumblebee::PARAM_NB_MAX );

	_serial_number_ui = 0;
	_camera_id_ui = 0;
	_b_camera_dialog_ui = false ;
	_capture_bumblebee.set_bumblebee_ui( this );
}
EMPTY_DESTRUCTOR( c_bumblebee )

c_capture_bumblebee::c_capture_bumblebee()
{
#if !AAA_WIN64()
	_triclops_context = nullptr;
	_digiclops_context = nullptr;
	_pgrcamGuiContext = nullptr;
#endif //#if !AAA_WIN64()
	_serial_number = 0;
	_camera_id = -1;
}
c_capture_bumblebee::~c_capture_bumblebee()
{
#if !AAA_WIN64()
	close_specific();
#endif //#if !AAA_WIN64()
}

#if !AAA_WIN64()
void	c_capture_bumblebee::close_specific()
{
	CameraGUIError	pgr_gui_err;

	_triclops_context = nullptr;
	_digiclops_context = nullptr;

	// destroy existing camera control dialog
	if( IS_NOT_NULL( _pgrcamGuiContext ) )
		{
		pgr_gui_err = dll_flycap_gui.pgrcamguiDestroyContext( _pgrcamGuiContext );
		if( pgr_gui_err != PGRCAMGUI_OK )
			{
			ERR_PRINT_STRING( "Censys 3d, bumblebee : error destroying Camera Dialog context." );
			}
		_pgrcamGuiContext = nullptr;
		}
}

void	c_capture_bumblebee::get_params()
{
	if(	_frame_rate != _bumblebee_ui->get_frame_rate() )
	{
		_frame_rate = (DigiclopsFrameRate) _bumblebee_ui->get_frame_rate();
		set_frame_rate();
	}
	if( _image_type != _bumblebee_ui->get_image_type() )
		_image_type = _bumblebee_ui->get_image_type();
	if( _image_2_type != _bumblebee_ui->get_image_2_type() )
		_image_2_type = _bumblebee_ui->get_image_2_type();
	if( _image_2_bind != _bumblebee_ui->get_image_2_bind() )
		_image_2_bind = _bumblebee_ui->get_image_2_bind();
	if( _image_3_type != _bumblebee_ui->get_image_3_type() )
		_image_3_type = _bumblebee_ui->get_image_3_type();
	if( _image_3_bind != _bumblebee_ui->get_image_3_bind() )
		_image_3_bind = _bumblebee_ui->get_image_3_bind();

	if( _b_camera_dialog != _bumblebee_ui->is_camera_dialog() )
	{
		_b_camera_dialog = _bumblebee_ui->is_camera_dialog();
		set_camera_dialog();
	}

	if( _gain != _bumblebee_ui->get_gain() || _b_gain_auto != _bumblebee_ui->is_gain_auto())
	{
		_gain = _bumblebee_ui->get_gain();
		_b_gain_auto = _bumblebee_ui->is_gain_auto();
		set_camera_gain();
	}

	if( _shutter != _bumblebee_ui->get_shutter() || _b_shutter_auto != _bumblebee_ui->is_shutter_auto())
	{
		_shutter = _bumblebee_ui->get_shutter();
		_b_shutter_auto = _bumblebee_ui->is_shutter_auto();
		set_camera_shutter();
	}

	if( _exposure != _bumblebee_ui->get_exposure() || _b_exposure_auto != _bumblebee_ui->is_exposure_auto())
	{
		_exposure = _bumblebee_ui->get_exposure();
		_b_exposure_auto = _bumblebee_ui->is_exposure_auto();
		set_camera_exposure();
	}

	if ( _delta_disparity != _bumblebee_ui->get_delta_disparity() )
	{	
		_delta_disparity = _bumblebee_ui->get_delta_disparity();
		set_delta_disparity();
	}

	if ( _b_sub_pixel_interpolation != _bumblebee_ui->is_sub_pixel_interpolation() )
	{	
		_b_sub_pixel_interpolation = _bumblebee_ui->is_sub_pixel_interpolation();
		set_sub_pixel_interpolation();
	}

	if ( _stereo_quality != _bumblebee_ui->get_stereo_quality() )
	{	
		_stereo_quality = _bumblebee_ui->get_stereo_quality();
		set_stereo_quality();
	}

	if ( _edge_mask != _bumblebee_ui->get_edge_mask() )
	{	
		_edge_mask = _bumblebee_ui->get_edge_mask();
		set_edge_mask(); 
	}

	if ( _stereo_mask != _bumblebee_ui->get_stereo_mask() )
	{	
		_stereo_mask = _bumblebee_ui->get_stereo_mask();
		set_stereo_mask();
	}

	if ( _segmentation_value != _bumblebee_ui->get_segmentation_value() )
	{	
		_segmentation_value = _bumblebee_ui->get_segmentation_value();
		set_segmentation_value();
	}

	if ( _texture_validation_threshold != _bumblebee_ui->get_texture_validation_threshold() )
	{	
		_texture_validation_threshold = _bumblebee_ui->get_texture_validation_threshold();
		set_texture_validation_threshold();
	}

	if ( _uniqueness_validation_threshold != _bumblebee_ui->get_uniqueness_validation_threshold() )
	{	
		_uniqueness_validation_threshold = _bumblebee_ui->get_uniqueness_validation_threshold();
		set_uniqueness_validation_threshold();
	}
}

void	c_capture_bumblebee::update_params()
{
	set_frame_rate();
	set_camera_gain();
	set_camera_shutter();
	set_camera_exposure();
	set_delta_disparity();
	set_sub_pixel_interpolation();
	set_stereo_quality();
	set_edge_mask(); 
	set_stereo_mask();
	set_segmentation_value();
	set_texture_validation_threshold();
	set_uniqueness_validation_threshold();
}

void	c_capture_bumblebee::set_stereo_mask()
{
	if( IS_NOT_NULL( _triclops_context ) )
	{
		TriclopsError	tri_err;
		tri_err = dll_triclops.triclopsSetStereoMask( _triclops_context , (INT32) _stereo_mask );
		if( tri_err != TriclopsErrorOk )
		{
			ERR_PRINT_STRING( "Censys 3d, bumblebee : error setting stereo mask, it should be an even integer." );
		}
	}
}

void	c_capture_bumblebee::set_edge_mask()
{
	if( IS_NOT_NULL( _triclops_context ) )
	{
		TriclopsError	tri_err;
		tri_err = dll_triclops.triclopsSetEdgeMask( _triclops_context, _edge_mask );
		if( tri_err != TriclopsErrorOk )
		{
			ERR_PRINT_STRING( "Censys 3d, bumblebee : error setting edge mask, it should be an even integer." );
		}
	}
}

void	c_capture_bumblebee::set_delta_disparity()
{
	if( IS_NOT_NULL( _triclops_context ) )
	{
		TriclopsError	tri_err;
		tri_err = dll_triclops.triclopsSetSurfaceValidationDifference( _triclops_context, _delta_disparity );
		if( tri_err != TriclopsErrorOk )
		{
			ERR_PRINT_STRING( "Censys 3d, bumblebee : error setting delta disparity." );
		}
	}
}

void	c_capture_bumblebee::set_segmentation_value()
{
	if( IS_NOT_NULL( _triclops_context ) )
	{
		TriclopsError	tri_err;
		tri_err = dll_triclops.triclopsSetSurfaceValidationSize( _triclops_context, _segmentation_value );
		if( tri_err != TriclopsErrorOk )
		{
			ERR_PRINT_STRING( "Censys 3d, bumblebee : error setting segmentation value (triclopsSetSurfaceValidationSize)." );
		}
	}
}

void	c_capture_bumblebee::set_texture_validation_threshold()
{
	if( IS_NOT_NULL( _triclops_context ) )
	{
		TriclopsError	tri_err;
		tri_err = dll_triclops.triclopsSetTextureValidationThreshold( _triclops_context, _texture_validation_threshold );
		if( tri_err != TriclopsErrorOk )
		{
			ERR_PRINT_STRING( "Censys 3d, bumblebee : error setting texture validation threshold (triclopsSetTextureValidationThreshold)." );
		}
	}
}

void	c_capture_bumblebee::set_uniqueness_validation_threshold()
{
	if( IS_NOT_NULL( _triclops_context ) )
	{
		TriclopsError	tri_err;
		tri_err = dll_triclops.triclopsSetUniquenessValidationThreshold( _triclops_context, _uniqueness_validation_threshold );
		if( tri_err != TriclopsErrorOk )
		{
			ERR_PRINT_STRING( "Censys 3d, bumblebee : error setting uniqueness validation threshold (triclopsSetUniquenessValidationThreshold)." );
		}
	}
}

void	c_capture_bumblebee::set_sub_pixel_interpolation()
{
	if( IS_NOT_NULL( _triclops_context ) )
	{
		TriclopsError	tri_err;
		tri_err = dll_triclops.triclopsSetSubpixelInterpolation( _triclops_context, TriclopsBool(_b_sub_pixel_interpolation) );
		if( tri_err != TriclopsErrorOk )
		{
			ERR_PRINT_STRING( "Censys 3d, bumblebee : error setting sub pixel interpolation." );
		}
	}
}

void	c_capture_bumblebee::set_stereo_quality()
{
	if( IS_NOT_NULL( _triclops_context ) )
	{
		TriclopsError	tri_err;
		tri_err = dll_triclops.triclopsSetStereoQuality( _triclops_context, (TriclopsStereoQuality) _stereo_quality );
		if( tri_err != TriclopsErrorOk )
		{
			ERR_PRINT_STRING( "Censys 3d, bumblebee : error setting stereo quality." );
		}
	}
}

void	c_capture_bumblebee::set_camera_dialog()
{
	if( _b_camera_dialog )
		open_camera_dialog();
	else
		close_camera_dialog();
}

void	c_capture_bumblebee::close_camera_dialog()
{

	// destroy existing camera control dialog
	if( IS_NOT_NULL( _pgrcamGuiContext ) )
	{
		CameraGUIError	pgr_gui_err;
		pgr_gui_err = dll_flycap_gui.pgrcamguiDestroyContext( _pgrcamGuiContext );
		if( pgr_gui_err != PGRCAMGUI_OK )
		{
			ERR_PRINT_STRING( "Censys 3d, bumblebee, error destroying Camera Dialog context." );
		}
		_pgrcamGuiContext = nullptr;
	}
}

void	c_capture_bumblebee::open_camera_dialog()
{

	if( IS_NOT_NULL( _digiclops_context ) )
	{
		CameraGUIError	pgr_gui_err;
		pgr_gui_err = dll_flycap_gui.pgrcamguiCreateContext( &_pgrcamGuiContext );
		if( pgr_gui_err != PGRCAMGUI_OK )
		{
			ERR_PRINT_STRING( "Censys 3d, bumblebee, error creating Camera Dialog context." );
		}
		else
		{
			pgr_gui_err = dll_flycap_gui.pgrcamguiInitializeSettingsDialog( _pgrcamGuiContext, (GenericCameraContext) _digiclops_context );
			if( pgr_gui_err != PGRCAMGUI_OK )
			{
				ERR_PRINT_STRING( "Censys 3d, bumblebee, error initializing Camera Dialog." );
			}
			else
			{
				pgr_gui_err = dll_flycap_gui.pgrcamguiToggleSettingsWindowState( _pgrcamGuiContext, get_window_main_handle() );
				if( pgr_gui_err != PGRCAMGUI_OK )
				{
					ERR_PRINT_STRING( "Censys 3d, bumblebee, error showing Camera Dialog." );
				}
			}
		}
	}
	else
	{
		ERR_PRINT_STRING( "Censys 3d, bumblebee, can't open camera dialog, no digiclops context." );
	}
}

void	c_capture_bumblebee::set_camera_gain()
{
	if( IS_NOT_NULL( _digiclops_context ) && !_b_censys3d_param_no_update )
	{
		DigiclopsError	digi_err;
		digi_err = dll_digiclops.digiclopsSetCameraProperty( _digiclops_context, DIGICLOPS_GAIN, _gain, 0, _b_gain_auto );
		if( digi_err != DIGICLOPS_OK )
		{
			ERR_PRINT_STRING( "Censys 3d, error setting gain_ for camera %d : %s.", _camera_id, get_digiclops_error_str(digi_err) );
		}
	}
}

void	c_capture_bumblebee::set_camera_shutter()
{
	if( IS_NOT_NULL( _digiclops_context ) && !_b_censys3d_param_no_update )
	{
		DigiclopsError	digi_err;
		digi_err = dll_digiclops.digiclopsSetCameraProperty( _digiclops_context, DIGICLOPS_SHUTTER, _shutter, 0, _b_shutter_auto );
		if( digi_err != DIGICLOPS_OK )
		{
			ERR_PRINT_STRING( "Censys 3d, error setting shutter_ for camera %d : %s.", _camera_id, get_digiclops_error_str(digi_err) );
		}
	}
}
void	c_capture_bumblebee::set_camera_exposure()
{
	if( IS_NOT_NULL( _digiclops_context ) && !_b_censys3d_param_no_update )
	{
		DigiclopsError	digi_err;
		digi_err = dll_digiclops.digiclopsSetCameraProperty( _digiclops_context, DIGICLOPS_AUTO_EXPOSURE, _exposure, 0, _b_exposure_auto );
		if( digi_err != DIGICLOPS_OK )
		{
			ERR_PRINT_STRING( "Censys 3d, error setting exposure_ for camera %d : %s.", _camera_id, get_digiclops_error_str(digi_err) );
		}
	}
}

void	c_capture_bumblebee::get_triclops_context( CENSYS_INSTANCE censys_inst )
{
	CENSYS_ERROR	cens_err;
	cens_err = dll_censys.censysSegmentorGetTriclopsContext( censys_inst, _camera_id, &_triclops_context );
	if( cens_err != censyserrorSUCCESS )
	{
		ERR_PRINT_STRING( "Censys 3d, can't get Triclops context for camera %d.", _camera_id );
	}
}

void	c_capture_bumblebee::get_digiclops_context( CENSYS_INSTANCE censys_inst )
{
	CENSYS_ERROR	cens_err;
	cens_err = dll_censys.censysGrabberGetDigiclopsContext( censys_inst, _camera_id, &_digiclops_context );
	if( cens_err != censyserrorSUCCESS )
	{
		ERR_PRINT_STRING( "Censys 3d, can't get Digiclops context for camera %d.", _camera_id );
	}
}

void	c_capture_bumblebee::get_serial_number( CENSYS_INSTANCE censys_inst )
{
	CENSYS_ERROR	cens_err;
	cens_err = dll_censys.censysGrabberGetDeviceSerialNumber( censys_inst, _camera_id, &_serial_number );
	if( cens_err != censyserrorSUCCESS )
	{
		ERR_PRINT_STRING( "Censys 3d, can't get serial number for camera %d.", _camera_id );
	}
	else
		_bumblebee_ui->set_serial_number( _serial_number );
}

void	c_capture_bumblebee::set_frame_rate()
{
	if( IS_NOT_NULL( _digiclops_context ) && !_b_censys3d_param_no_update )
	{
		DigiclopsError	digi_err;
		digi_err = dll_digiclops.digiclopsSetFrameRate( _digiclops_context, _frame_rate );
		if( digi_err != DIGICLOPS_OK )
		{
			ERR_PRINT_STRING( "Censys 3d, error setting frame rate for camera %d : %s.", _camera_id, get_digiclops_error_str(digi_err) );
		}
	}
}

void	c_capture_bumblebee::print_settings()
{
	TriclopsError	tri_err;
	DigiclopsError	digi_err;

	CENSYS_PRINT_STRING( "\tSettings for camera %d :", _camera_id );

	if( IS_NOT_NULL( _triclops_context ) )
	{
		INT32	size;
		tri_err = dll_triclops.triclopsGetStereoMask( _triclops_context, &size );
		if( tri_err != TriclopsErrorOk )
		{
			ERR_PRINT_STRING( "Censys 3d, error getting stereo mask." );
		}
		else
		{
			CENSYS_PRINT_STRING( "\t\tStereoMask : %d", size);
		}

		INT32	masksize;
		tri_err = dll_triclops.triclopsGetEdgeMask( _triclops_context, &masksize );
		if( tri_err != TriclopsErrorOk )
		{
			ERR_PRINT_STRING( "Censys 3d, error getting edge mask." );
		}
		else
		{
			CENSYS_PRINT_STRING( "\t\tEdgeMask : %d", masksize);
		}

		float	diff;
		tri_err = dll_triclops.triclopsGetSurfaceValidationDifference( _triclops_context, &diff );
		if( tri_err != TriclopsErrorOk )
		{
			ERR_PRINT_STRING( "Censys 3d, error getting delta disparity." );
		}
		else
		{
			CENSYS_PRINT_STRING( "\t\tDelta disparity : %f", diff);
		}

		tri_err = dll_triclops.triclopsGetSurfaceValidationSize( _triclops_context , &size );
		if( tri_err != TriclopsErrorOk )
		{
			ERR_PRINT_STRING( "Censys 3d, error getting segmentation value (triclopsSetSurfaceValidationSize)." );
		}
		else
		{
			CENSYS_PRINT_STRING( "\t\tSegmentation value : %d", size);
		}

		float	value;
		tri_err = dll_triclops.triclopsGetTextureValidationThreshold( _triclops_context, &value );
		if( tri_err != TriclopsErrorOk )
		{
			ERR_PRINT_STRING( "Censys 3d, error getting texture validation threshold (triclopsSetTextureValidationThreshold)." );
		}
		else
		{
			CENSYS_PRINT_STRING( "\t\tTexture validation threshold : %f", value);
		}

		tri_err = dll_triclops.triclopsGetUniquenessValidationThreshold( _triclops_context, &value );
		if( tri_err != TriclopsErrorOk )
		{
			ERR_PRINT_STRING( "Censys 3d, error getting uniqueness validation threshold (triclopsGetUniquenessValidationThreshold)." );
		}
		else
		{
			CENSYS_PRINT_STRING( "\t\tUniqueness validation threshold : %f", value);
		}
	}

	if( IS_NOT_NULL( _digiclops_context ) )
	{
		DigiclopsFrameRate	pFrameRate;
		digi_err = dll_digiclops.digiclopsGetFrameRate( _digiclops_context, &pFrameRate );
		if( digi_err != DIGICLOPS_OK )
		{
			ERR_PRINT_STRING( "Censys 3d, error getting digiclops framerate : %s.", get_digiclops_error_str(digi_err) );
		}
		else
		{
			CENSYS_PRINT_STRING( "\t\tFramerate : %s %%", censys_frame_rate_str[pFrameRate]);
		}

		bool	b_present, b_auto, b_manual;
		LONG	plMin;
		LONG	plMax;
		LONG	plDefault;
		for( INT32 j = DIGICLOPS_AUTO_EXPOSURE; j < DIGICLOPS_PROPERTY_TOTAL; ++j )
		{
			digi_err = dll_digiclops.digiclopsGetCameraPropertyRange(	_digiclops_context, (DigiclopsCameraProperty)j, &b_present, &plMin, &plMax, &plDefault, &b_auto, &b_manual );
			if( digi_err != DIGICLOPS_OK )
			{
				ERR_PRINT_STRING( "Censys 3d, error getting digiclops camera property : %s.", get_digiclops_error_str(digi_err) );
			}
			else
			{
				CENSYS_PRINT_STRING( "\t\tProperty %s :", censys_camera_property[j]);
				CENSYS_PRINT_STRING( "\t\t\tPresent %d\tMin %d\tMax %d", b_present, plMin, plMax );
				CENSYS_PRINT_STRING( "\t\t\tDefault %d\tAuto %d\tManual %d", plDefault, b_auto, b_manual);
			}
		}
	}
}
#endif //#if !AAA_WIN64()

void	c_capture_bumblebee::set_camera_id( INT32 p_in )
{ 
	_camera_id = p_in;

	_bumblebee_ui->set_camera_id( _camera_id );

}

#endif	//#if AAA_USE_POINT_GREY()