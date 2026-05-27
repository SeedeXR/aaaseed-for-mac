
#include "ptgrey_utils.h"

#if AAA_USE_POINT_GREY()

#include "capture_triclops.h"
#include "infrastructure/param/param_declare.h"
#include "image/bind_img_2d.h"

		 
#ifndef AAA_WRAP_DIGICLOPS_H
#	include "wrap_digiclops.h"
#endif		// sr@20100409
#include "wrap_triclops.h"		// sr@20100409
		 
#include "wrap_PGRFlyCaptureGui.h"
		 
#include "ui/keyboard.h"
		 
#include "obj_ui/tracker/trackers.h"

//extern c_PGRFlyCaptureGui_dll	dll_flycap_gui;

UINT8*	c_capture_triclops::m_ucSubpixelInvalidDisparityMapLUT = nullptr;

o_str	c_capture_triclops::version;
bool	c_capture_triclops::b_dll_digiclops_loaded = false;
bool	c_capture_triclops::b_dll_triclops_loaded = false;
UINT32	c_capture_triclops::device_count = 0;

/**
* Colours for creating the colourful disparity image.
*
* @note Note that these magic numbers are dependant on the magic numbers in
*		PGRBitmapTriclops::createDisparityImagePalette().
*/
typedef enum
{
	TRI_RED		= 130,
	TRI_GREEN	= 129,
	TRI_BLUE	= 128,
	TRI_CYAN	= 131,
	TRI_MAGENTA	= 132,
	TRI_YELLOW	= 133,

}	enum_CONSTANTS;


FACTORY_CREATE_V1( c_triclops_ui, triclops_ui, Capture Triclops, cap_triclops );

C_PCHAR_C	c_triclops_ui::digiclops_camera_type_str[2] =
{
	"Black and White",
	"Color",
};

C_PCHAR_C	c_triclops_ui::digiclops_camera_device_str[3] =
{
	"Digiclops",
	"Bumblebee",
	"Unknown",
};

C_PCHAR_C	c_triclops_ui::digiclops_camera_resolution_str[4] =
{
	"160 x 120",		// 160 x 120 resolution.
	"320 x 240",		// 320 x 240 resolution.
	"640 x 480",		// 640 x 480 resolution.
	"1024 x 768",		// 1024 x 768 resolution.
};


C_PCHAR_C	c_triclops_ui::triclops_stereo_quality_str[2] =
{
	"Standard",
	"Enhanced",
};

C_PCHAR_C	c_triclops_ui::triclops_camera_image_str[ TRICLOPS_IMAGE_MAX_NB ] =
{
	"No",
	"Left Raw",
	"Left Rectified",
	"Left Edge",
	"Right Raw",
	"Right Rectified",
	"Right Edge",
	"Depth Image",
};

static	CONST	INT32	DIGICLOPS_BANDWIDTH_MAX_NB = 4;
static	C_PCHAR_C	digiclops_bandwidth_str[ DIGICLOPS_BANDWIDTH_MAX_NB ] =
{
	"100%",
	"50%",
	"25%",
	"12%"
};
#if !AAA_WIN64()
static	DigiclopsFrameRate	digiclops_bandwidth[DIGICLOPS_BANDWIDTH_MAX_NB] = { DIGICLOPS_FRAMERATE_100, DIGICLOPS_FRAMERATE_050, DIGICLOPS_FRAMERATE_025, DIGICLOPS_FRAMERATE_012 };
#endif
static	std::list<c_capture*>	list_cap_triclops;

static	INT32	cam_nb = 0;

INT32	c_capture_triclops::do_enum( bool b_verbose )
{
	device_count = 0;
	UINT32			nb = 0;
	if( !b_dll_triclops_loaded )
	{
//		UINT32	err;			// wrapping the "triclops.dll" // sr@20100409

		b_dll_triclops_loaded = dll_triclops.init();
		if( !b_dll_triclops_loaded )
			return nb;
	//err = wrap_triclops_Init();
		//if( NO_ERROR!=err )
		//	return nb;
		//b_dll_triclops_loaded = true;
	}

	if( !b_dll_digiclops_loaded )
	{
		b_dll_digiclops_loaded = dll_digiclops.init();
		if( !b_dll_digiclops_loaded )
			return nb;
		//UINT32	err;			// wrapping the "digiclops.dll" // sr@20100409

		//err = wrap_digiclops_Init();
		//if( NO_ERROR!=err )
		//	return nb;
		//b_dll_digiclops_loaded = true;
	}

#if !AAA_WIN64()
	C_PCHAR	triclops_version = dll_triclops.triclopsVersion();
	if( triclops_version )
	{
		version.set( triclops_version );
		GOOD_PRINT_STRING( "triclops version : %s", version.get() );
	}

	if( cam_nb > 0 )
	{
		GOOD_PRINT_STRING( "capture triclops detected before %d camera, using it", cam_nb );
		return cam_nb;
	}

	DigiclopsError	de;

//todo use digiclopsBusEnumerateCameras( DigiclopsInfo*  parInfo, unsigned int*   puiSize );
	de = dll_digiclops.digiclopsBusCameraCount( &nb );
	if( de == DIGICLOPS_OK )
	{
		cam_nb = nb;
		GOOD_PRINT_STRING( "capture triclops %d camera counted", nb );
	}
	else
	{
		ERR_PRINT_STRING( "capture triclops could not count cameras : %s", get_digiclops_error_str(de) );
		return 0;
	}

	DigiclopsInfo*	cam_info = nullptr;
	UINT32*			cam_size = nullptr;
	if( nb > 0 )
	{
		o_str			name;
		o_str			version;

		//todo quick, if malloc doesn't work, it will crash!!!!
		cam_info = new DigiclopsInfo[nb];
		cam_size = new UINT32[nb];
		if( !cam_info || !cam_size )
			goto exit;
		de = dll_digiclops.digiclopsBusEnumerateCameras( cam_info, cam_size );
		if ( de != DIGICLOPS_OK)
		{
			ERR_PRINT_STRING( "capture triclops could not enumerate cameras");
			goto exit;
		}

		for( UINT32 i = 0; i < nb; ++i )
		{
			if( b_verbose)
			{
				CAPTURE_PRINT_STRING( "PGR Triclops Device %d", i );
				CAPTURE_PRINT_STRING( "        %s (%s) %s", c_triclops_ui::digiclops_camera_device_str[ cam_info[i].CameraDevice ],
															c_triclops_ui::digiclops_camera_type_str[ cam_info[i].CameraType ],
															c_triclops_ui::digiclops_camera_resolution_str[ cam_info[i].ImageSize ] );
				CAPTURE_PRINT_STRING( "        serial number : %ld", cam_info[i].SerialNumber );
			}
		}
	}

exit:
	SAFE_DELETE_ARRAY( cam_info );
	SAFE_DELETE_ARRAY( cam_size );
#endif //#if !AAA_WIN64()
	device_count = nb;
	return nb;
}

void	c_capture_triclops::c_init()
{
	list_cap_triclops.clear();
	do_enum( true );
}

extern	c_capture*	capture_triclops_find_by_window_hd( HWND hd_wind_in)
{
	for( auto const & pt : list_cap_triclops )
	{
		if( pt->get_hd_wind() == hd_wind_in )
		{
			return pt;
		}
	}
	ERR_PRINT_STRING( "capture triclops window unknown");
	return nullptr;
}

void	capture_triclops_add( c_capture* pt)
{
	list_cap_triclops.push_back( pt);
}

void	capture_triclops_remove( HWND hd_wind_in)
{
	c_capture*	pt = capture_triclops_find_by_window_hd( hd_wind_in);
	if( pt )
		list_cap_triclops.remove( pt);
}


void	c_capture_triclops::c_deinit()
{
	while( !list_cap_triclops.empty() )
	{
		c_capture*	pt = *list_cap_triclops.begin();
		list_cap_triclops.pop_front();		//hack is it thread/callback safe ?
		delete pt;
	}
}

c_capture_triclops::c_capture_triclops()
	:_triclops_serial_number	(	0	)
	,_serial_number				(	0	)
{
	_image_flux_count = 3;
	_img_2 = new c_image_flux( nullptr, false );	//todo	we didn't build the non NULL case for first arg
	_img_3 = new c_image_flux( nullptr, false );	//todo	we didn't build the non NULL case for first arg
	_img_2->set_image_nb_to_keep( 4 );
	_img_3->set_image_nb_to_keep( 4 );

//	clear_frame_ready();
}

c_capture_triclops::~c_capture_triclops()
{
	close();

	SAFE_DELETE( _img_2 );
	SAFE_DELETE( _img_3 );

#if !AAA_WIN64()
	// Destroy gui context.
	dll_flycap_gui.pgrcamguiDestroyContext( _m_guicontext );
#endif
}

#if !AAA_WIN64()
void	c_capture_triclops::dlg_source()
{
	// Show the camera selection dialog.
	CameraGUIError	guierror;
	//INT32			iDialogStatus  = 0;
	INT_PTR			dialog = 0;

	if (_m_guicontext )
	{
		guierror = dll_flycap_gui.pgrcamguiShowCameraSelectionModal( _m_guicontext, _digiclops_context, &_triclops_serial_number, &dialog ); //iDialogStatus );
		if( guierror != PGRCAMGUI_OK )
		{
			ERR_PRINT_STRING("Error showing camera selection dialog." );
		}
	}
}

void	c_capture_triclops::dlg_display()
{
	if (_m_guicontext )
	{
		dll_flycap_gui.pgrcamguiToggleSettingsWindowState( _m_guicontext, get_window_main_handle() );
	}
}

void	c_capture_triclops::update_params()
{
	if ( _b_opened )
	{
		TriclopsError	te;

		if ( _disparity_min != _triclops->get_disparity_min() || _disparity_max != _triclops->get_disparity_max() )
		{
			// set disparity range
			te = dll_triclops.triclopsSetDisparity( _triclops_context, _disparity_min, _disparity_max );

		}
		if ( _stereo_quality != _triclops->get_stereo_quality() )
		{
			// set stereo quality
			te = dll_triclops.triclopsSetStereoQuality( _triclops_context, _stereo_quality );
		}

		if ( _stereo_mask != _triclops->get_stereo_mask() )
		{
			// set stereo mask
			te = dll_triclops.triclopsSetStereoMask( _triclops_context, _triclops->get_stereo_mask() );
			_stereo_mask = _triclops->get_stereo_mask();
		}

		if( _b_edge_correlation != _triclops->is_edge_correlation() )
		{
			// set stereo mask
			te = dll_triclops.triclopsSetEdgeCorrelation( _triclops_context, TriclopsBool(_triclops->is_edge_correlation()) );
			_b_edge_correlation = _triclops->is_edge_correlation();
		}

		if( _edge_mask != _triclops->get_edge_mask() )
		{
			// set stereo mask
			te = dll_triclops.triclopsSetEdgeMask( _triclops_context, _triclops->get_edge_mask() );
			_edge_mask = _triclops->get_edge_mask();
		}
		if( _b_sub_pixel_interpolation != _triclops->is_sub_pixel_interpolation() )
		{
			// turn on sub-pixel interpolation
			te = dll_triclops.triclopsSetSubpixelInterpolation( _triclops_context, TriclopsBool(_triclops->get_sub_pixel_interpolation()) );
			_b_sub_pixel_interpolation = _triclops->get_sub_pixel_interpolation();
		}
		if( _b_sub_pixel_interpolation_strict != _triclops->get_sub_pixel_interpolation_strict() )
		{
			// make sure strict subpixel validation is on
			te = dll_triclops.triclopsSetStrictSubpixelValidation( _triclops_context, TriclopsBool(_triclops->is_sub_pixel_interpolation_strict()) );
			_b_sub_pixel_interpolation_strict = _triclops->get_sub_pixel_interpolation_strict();
		}

		if ( _sub_pixel_validation_mapping != _triclops->get_sub_pixel_validation_mapping() )
		{
			// make sure strict subpixel validation is on
			te = dll_triclops.triclopsSetSubpixelValidationMapping( _triclops_context, _triclops->get_sub_pixel_validation_mapping() );
			_sub_pixel_validation_mapping = _triclops->get_sub_pixel_validation_mapping();
		}

		if( _b_texture_validation != _triclops->is_texture_validation() )
		{
			// lets turn off all validation except subpixel and surface
			// this works quite well
			te = dll_triclops.triclopsSetTextureValidation( _triclops_context, TriclopsBool(_triclops->is_texture_validation()) );
			_b_texture_validation = _triclops->is_texture_validation();
		}
		if ( _texture_validation_threshold != _triclops->get_texture_validation_mapping() )
		{
			te = dll_triclops.triclopsSetTextureValidationThreshold( _triclops_context, _triclops->get_texture_validation_mapping() );
			_texture_validation_threshold = _triclops->get_texture_validation_mapping();
		}
		if ( _texture_validation_mapping != _triclops->get_texture_validation_mapping() )
		{
			te = dll_triclops.triclopsSetTextureValidationMapping( _triclops_context, _triclops->get_texture_validation_mapping() );
			_texture_validation_mapping = _triclops->get_texture_validation_mapping();
		}

		if( _b_surface_validation != _triclops->is_surface_validation() )
		{
			// turn on surface validation
			te = dll_triclops.triclopsSetSurfaceValidation( _triclops_context, TriclopsBool(_triclops->is_surface_validation()) );
			_b_surface_validation = _triclops->is_surface_validation();
		}
		if( _surface_validation_size != _triclops->get_surface_validation_size() )
		{
			te = dll_triclops.triclopsSetSurfaceValidationMapping( _triclops_context, _triclops->get_surface_validation_size() );
			_surface_validation_size = _triclops->get_surface_validation_size();
		}
		if( _surface_validation_difference != _triclops->get_surface_validation_difference() )
		{
			te = dll_triclops.triclopsSetSurfaceValidationSize( _triclops_context, _triclops->get_surface_validation_difference() );
			_surface_validation_difference = _triclops->get_surface_validation_difference();
		}
		if( _surface_validation_mapping != _triclops->get_surface_validation_mapping() )
		{
			te = dll_triclops.triclopsSetSurfaceValidationDifference( _triclops_context, _triclops->get_surface_validation_mapping() );
			_surface_validation_mapping = _triclops->get_surface_validation_mapping();
		}

		if( _b_uniqueness_validation != _triclops->is_uniqueness_validation() )
		{
			te = dll_triclops.triclopsSetUniquenessValidation( _triclops_context, TriclopsBool(_triclops->is_uniqueness_validation()) );
			_b_uniqueness_validation = _triclops->is_uniqueness_validation();
		}
		if ( _uniqueness_validation_threshold != _triclops->get_uniqueness_validation_threshold() )
		{
			te = dll_triclops.triclopsSetUniquenessValidationThreshold( _triclops_context, _triclops->get_uniqueness_validation_threshold() );
			_uniqueness_validation_threshold = _triclops->get_uniqueness_validation_threshold();
		}
		if ( _uniqueness_validation_mapping != _triclops->get_uniqueness_validation_mapping() )
		{
			te = dll_triclops.triclopsSetUniquenessValidationMapping( _triclops_context, _triclops->get_uniqueness_validation_mapping() );
			_uniqueness_validation_mapping = _triclops->get_uniqueness_validation_mapping();
		}

		if( _b_backforth_validation != _triclops->is_backforth_validation() )
		{
			te = dll_triclops.triclopsSetBackForthValidation( _triclops_context, TriclopsBool(_triclops->is_backforth_validation()) );
			_b_backforth_validation = _triclops->is_backforth_validation();
		}

		if ( _backforth_validation_mapping != _triclops->get_backforth_validation_mapping() )
		{
			te = dll_triclops.triclopsSetBackForthValidationMapping( _triclops_context, _triclops->get_backforth_validation_mapping() );
			_backforth_validation_mapping = _triclops->get_backforth_validation_mapping();
		}

		_image_type = _triclops->get_image_type();
		_image_2_type = _triclops->get_image_2_type();
		_image_3_type = _triclops->get_image_3_type();
		_image_2_bind = _triclops->get_image_2_bind();
		_image_3_bind = _triclops->get_image_3_bind();


		if( _stereo_resolution_x != _triclops->get_stereo_resolution_x() || _stereo_resolution_y != _triclops->get_stereo_resolution_y() )
		{
			// set up some stereo parameters:
			// set to 320x240 output images
			te = dll_triclops.triclopsSetResolution( _triclops_context, _triclops->get_stereo_resolution_y(), _triclops->get_stereo_resolution_x() );
			if( te )
			{
				ERR_PRINT_STRING( " calling triclopsSetResolution() : %s", triclops_get_error_str( te ) );
				ERR_PRINT_STRING( " calling triclopsSetResolution() : resolution impossible %d x %d", _triclops->get_stereo_resolution_x(), _triclops->get_stereo_resolution_y() );
			}
			_stereo_resolution_x = _triclops->get_stereo_resolution_x();
			_stereo_resolution_y = _triclops->get_stereo_resolution_y();
		}

		_disp_min = _triclops->get_disp_min();
		_disp_max = _triclops->get_disp_max();
	}
}

static	bool	need_stereo_process( TRICLOPS_IMAGE_TYPE type )
{
	switch( type )
	{
	case	TRI_IMAGE_EDGE_LEFT:
	case	TRI_IMAGE_EDGE_RIGHT:
	case	TRI_IMAGE_DEPTH:
		return true;
	default:
		return false;
	}
}

AAA_ERR	triclops_get_image( CONST TriclopsContext context, TriclopsImageType imageType,	TriclopsCamera camera, TriclopsImage* image )
{
	TriclopsError	err = dll_triclops.triclopsGetImage(  context,	imageType, camera, image );
	if( err )
	{
		ERR_PRINT_STRING( " Triclops : %s", triclops_get_error_str( err ) );
		return ERR_ANY;
	}
	return AAA_OK;
}

AAA_ERR	triclops_get_image_16( CONST TriclopsContext context, TriclopsImage16Type imageType,	TriclopsCamera camera, TriclopsImage16* image )
{
	TriclopsError	err = dll_triclops.triclopsGetImage16(  context,	imageType, camera, image );
	if( err )
	{
		ERR_PRINT_STRING( " Triclops : %s", triclops_get_error_str( err ) );
		return ERR_ANY;
	}
	return AAA_OK;
}

AAA_ERR	c_capture_triclops::triclops_got_image( c_image_flux* img_flux, TriclopsImageType type, TriclopsCamera which )
{
	TBUF_INC( tbuf::CH_CAPTURE_CALLBACK, 1., "triclops" );
	AAA_ERR retcode = ERR_ANY;
	if( NOERR( triclops_get_image( _triclops_context, type, which, &_img_reference ) ) )
	{
		if( _img_reference.data )
		{
			//img_flux->set_src_pixel_format( aaa::PIXEL_FORMAT::R_8 );
			img_flux->set_flux_size_format( _img_reference.ncols, _img_reference.nrows, aaa::PIXEL_FORMAT::R_8 );
		//	tbuf_inc( c_tbuf_master::CH_VIDEO_MOVE_FRAME, 1., "triclops_move_frame" );
			//img_flux->set_src_pitch(_img_reference.rowinc);
			img_flux->got_frame( (UINT8 *)_img_reference.data, "triclops image", _img_reference.rowinc );
		//	tbuf_dec( c_tbuf_master::CH_VIDEO_MOVE_FRAME, 1., "triclops_move_frame" );
			return AAA_OK;
		}
	}
	TBUF_DEC( tbuf::CH_CAPTURE_CALLBACK, 0., "triclops" );
	return retcode;
}

bool	c_capture_triclops::run( bool b_stream_in )
{
	TriclopsError	te;
	DigiclopsError	de;

	if( _b_opened && !_b_running )
	{
		// start grabbing
		de = dll_digiclops.digiclopsStart( _digiclops_context );
		if ( de == DIGICLOPS_OK )
		{
			GOOD_PRINT_STRING( "digiclopsStart() done and ok" );
			_b_running = true;
			//_b_opened = true;
		}
		//if (!(de == DIGICLOPS_OK))
		// goto exit_on_error;
	}

	update_params();

	// grab the image set
	de = dll_digiclops.digiclopsGrabImage( _digiclops_context );
	if( de )
	{
		ERR_PRINT_STRING( "calling digiclopsGrabImage() : %s", get_digiclops_error_str(de) );
		goto exit_on_error;
	}

	// grab the stereo data
	de = dll_digiclops.digiclopsExtractTriclopsInput( _digiclops_context, STEREO_IMAGE, &_img_stereo_data );
	if( de )
	{
		ERR_PRINT_STRING( "calling digiclopsExtractTriclopsInput() : %s", get_digiclops_error_str(de) );
		goto exit_on_error;
	}

	// grab the color image data
	// (note: if you are using a B&W Digiclops, this will of course be in B&W)

	//switch( _image_type )
	//	{
	//	case TRI_IMAGE_RIGHT:
	//		// retrieve RIGHT IMAGE
	//		de = digiclopsExtractTriclopsInput( _digiclops_context, RIGHT_IMAGE, &_img_color_data );
	//		break;
	//	case TRI_IMAGE_LEFT:
	//		de = digiclopsExtractTriclopsInput( _digiclops_context, LEFT_IMAGE, &_img_color_data );
	//		break;
	//	}

//	de = digiclopsExtractTriclopsInput( digiclops_context_, RIGHT_IMAGE, &img_color_data_ );
//TOP_IMAGE | LEFT_IMAGE | RIGHT_IMAGE | STEREO_IMAGE 
	// preprocessing the images
	te = dll_triclops.triclopsPreprocess( _triclops_context, &_img_stereo_data );
	if( te )
	{
		ERR_PRINT_STRING( "calling triclopsPreprocess() : %d", te );
		goto exit_on_error;
	}

	// stereo processing
	//	this test is really necessary to have full speed ( the stereo process is really slow )
	if( need_stereo_process(_image_type) || need_stereo_process(_image_2_type) || need_stereo_process(_image_3_type) )
	{
		te = dll_triclops.triclopsStereo( _triclops_context ) ;
		if( te )
		{
			ERR_PRINT_STRING( "calling triclopsStereo() : %d", te );
			goto exit_on_error;
		}
	}
	TBUF_INC( tbuf::CH_CAPTURE_CALLBACK, 1., "triclops" );

	switch (_image_type ) 
		{
		case TRI_IMAGE_NO:			break;
		case TRI_IMAGE_RAW_RIGHT:	triclops_got_image( this, TriImg_RAW,		TriCam_RIGHT );	break;
		case TRI_IMAGE_RECT_RIGHT:	triclops_got_image( this, TriImg_RECTIFIED, TriCam_RIGHT );	break;
		case TRI_IMAGE_EDGE_RIGHT:	triclops_got_image( this, TriImg_EDGE,		TriCam_RIGHT );	break;
		case TRI_IMAGE_RAW_LEFT:	triclops_got_image( this, TriImg_RAW,		TriCam_LEFT );	break;
		case TRI_IMAGE_RECT_LEFT:	triclops_got_image( this, TriImg_RECTIFIED, TriCam_LEFT );	break;
		case TRI_IMAGE_EDGE_LEFT:	triclops_got_image( this, TriImg_EDGE,		TriCam_LEFT );	break;
		case TRI_IMAGE_DEPTH:
			if( NOERR( triclops_get_image_16( _triclops_context, TriImg16_DISPARITY, TriCam_REFERENCE, &_img_depth_16 ) ) )
			{
			//	set_src_pixel_format( aaa::PIXEL_FORMAT::DISP_16 );
				set_flux_size_format( _img_depth_16.ncols, _img_depth_16.nrows, aaa::PIXEL_FORMAT::DISP_16 );
				set_disp_min_max( _disp_min, _disp_max );
				got_frame( (UINT8*) _img_depth_16.data, "triclops Depth" );
			}
			break;
		}

	_img_2->set_flip_vertical( is_flip_vertical() );
	_img_2->set_field_flip( _b_field_flip_order );
	//GOOD_PRINT_STRING( "img2 triclops_got_image" );
	switch (_image_2_type ) 
	{
	case TRI_IMAGE_NO:			break;
	case TRI_IMAGE_RAW_RIGHT:	triclops_got_image( _img_2, TriImg_RAW,			TriCam_RIGHT );	break;
	case TRI_IMAGE_RECT_RIGHT:	triclops_got_image( _img_2, TriImg_RECTIFIED,	TriCam_RIGHT );	break;
	case TRI_IMAGE_EDGE_RIGHT:	triclops_got_image( _img_2, TriImg_EDGE,		TriCam_RIGHT );	break;
	case TRI_IMAGE_RAW_LEFT:	triclops_got_image( _img_2, TriImg_RAW,			TriCam_LEFT );	break;
	case TRI_IMAGE_RECT_LEFT:	triclops_got_image( _img_2, TriImg_RECTIFIED,	TriCam_LEFT );	break;
	case TRI_IMAGE_EDGE_LEFT:	triclops_got_image( _img_2, TriImg_EDGE,		TriCam_LEFT );	break;
	case TRI_IMAGE_DEPTH:
		if( NOERR( triclops_get_image_16( _triclops_context, TriImg16_DISPARITY, TriCam_REFERENCE, &_img_depth_16 ) ) )
		{
		//	_img_2->set_src_pixel_format( aaa::PIXEL_FORMAT::DISP_16 );
			_img_2->set_flux_size_format( _img_depth_16.ncols, _img_depth_16.nrows, aaa::PIXEL_FORMAT::DISP_16 );
			_img_2->set_disp_min_max( _disp_min, _disp_max );
			_img_2->got_frame( (UINT8*) _img_depth_16.data, "triclops Depth 2" );
		}
		break;
	}
	//GOOD_PRINT_STRING( "img2 triclops_got_image done" );

	_img_3->set_flip_vertical( is_flip_vertical() );
	_img_3->set_field_flip( _b_field_flip_order );
	//GOOD_PRINT_STRING( "img3 triclops_got_image" );
	switch (_image_3_type ) 
	{
	case TRI_IMAGE_NO:			break;
	case TRI_IMAGE_RAW_RIGHT:	triclops_got_image( _img_3, TriImg_RAW,			TriCam_RIGHT );	break;
	case TRI_IMAGE_RECT_RIGHT:	triclops_got_image( _img_3, TriImg_RECTIFIED,	TriCam_RIGHT );	break;
	case TRI_IMAGE_EDGE_RIGHT:	triclops_got_image( _img_3, TriImg_EDGE,		TriCam_RIGHT );	break;
	case TRI_IMAGE_RAW_LEFT:	triclops_got_image( _img_3, TriImg_RAW,			TriCam_LEFT );	break;
	case TRI_IMAGE_RECT_LEFT:	triclops_got_image( _img_3, TriImg_RECTIFIED,	TriCam_LEFT );	break;
	case TRI_IMAGE_EDGE_LEFT:	triclops_got_image( _img_3, TriImg_EDGE,		TriCam_LEFT );	break;
	case TRI_IMAGE_DEPTH:
		if( NOERR( triclops_get_image_16( _triclops_context, TriImg16_DISPARITY, TriCam_REFERENCE, &_img_depth_16 ) ) )
		{
			_img_3->set_flux_size_format( _img_depth_16.ncols, _img_depth_16.nrows, aaa::PIXEL_FORMAT::DISP_16 );
		//	_img_3->set_src_pixel_format( aaa::PIXEL_FORMAT::DISP_16 );
			_img_3->set_disp_min_max( _disp_min, _disp_max );
		//	_frame_info.size_x			= _img_depth_16.ncols;
	//		_frame_info.size_y			= _img_depth_16.nrows;
		//	_frame_info.src				= (UINT8*) _img_depth_16.data;
	//		_frame_info.pixel_format	= _src_pixel_format;
	//		_frame_info.pitch			= 0;
			_img_3->got_frame( (UINT8*)_img_depth_16.data, "triclops Depth 3" );
		}
		break;
	}
	TBUF_DEC( tbuf::CH_CAPTURE_CALLBACK, 0., "triclops" );
	//GOOD_PRINT_STRING( "img3 triclops_got_image done" );
	return true;
exit_on_error:
	ERR_PRINT_STRING( "exiting %s() on error", __FUNCTION__ );
	return false;
}

void	c_capture_triclops::stop()
{
	dll_digiclops.digiclopsStop( _digiclops_context );

	dll_digiclops.digiclopsDestroyContext( _digiclops_context );
	dll_triclops.triclopsDestroyContext( _triclops_context );
	_b_running = false;
	_b_streaming = false;
}

void	c_capture_triclops::update()
{
	if( _b_running )//&& !is_field_new() )
		ask_frame();
}

AAA_ERR	c_capture_triclops::open_specific( INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_name )
{
	if( !_b_opened )
	{
		DigiclopsError		de;
		TriclopsError		te;
		CameraGUIError		guierror;
		bool				b_show_dialog = false;

		GOOD_PRINT_STRING( "in %s() we try to open", __FUNCTION__ );
		// open the Digiclops
		de = dll_digiclops.digiclopsCreateContext( &_digiclops_context );
		if( de == DIGICLOPS_OK )
			GOOD_PRINT_STRING( "called digiclopsCreateContext() : ok" );
		else
		{
			ERR_PRINT_STRING( "calling digiclopsCreateContext() : %s", get_digiclops_error_str(de) );
			goto exit_on_error;
		}

		capture_triclops_add( this );

		guierror = dll_flycap_gui.pgrcamguiCreateContext( &_m_guicontext );
		if( guierror == PGRCAMGUI_OK )
			GOOD_PRINT_STRING( "called pgrcamguiCreateContext() : ok" );
		else
		{
			ERR_PRINT_STRING( "calling pgrcamguiCreateContext() : %s", get_camera_gui_error_str( guierror ) );
		}

		//modifier::update( false );

		if( modifier::is_shift_on() )
		{
			b_show_dialog = true;
			dlg_source();
		}

		trackers::PRINT_STRING( "Triclops", "serial number = %d", _triclops_serial_number );
		if( _triclops_serial_number != 0 )
		{
			de = dll_digiclops.digiclopsInitializeFromSerialNumber( _digiclops_context, _triclops_serial_number );
			if( de == DIGICLOPS_OK )
				GOOD_PRINT_STRING( "called digiclopsInitializeFromSerialNumber(): ok" );
			else
			{
				ERR_PRINT_STRING( "calling digiclopsInitializeFromSerialNumber(): %s", get_digiclops_error_str(de) );
				goto exit_on_error;
			}
		}
		else
		{
			de = dll_digiclops.digiclopsInitialize( _digiclops_context, 0 );
			if( de == DIGICLOPS_OK )
				GOOD_PRINT_STRING( "called digiclopsInitialize(): ok" );
			else
			{
				ERR_PRINT_STRING( "calling digiclopsInitialize() : %s", get_digiclops_error_str(de) );
				goto exit_on_error;
			}
		}

		//
		// Create settings dialog
		//
//		CameraGUIError	guierror;

		//	guierror = pgrcamguiInitializeSettingsDialog( m_guicontext_, (GenericCameraContext)digiclops_context_ );
/*
		guierror = pgrcamguiInitializeSettingsDialog( _m_guicontext, _digiclops_context );
		if( guierror != PGRCAMGUI_OK )
		{
			ERR_PRINT_STRING( "calling pgrcamguiInitializeSettingsDialog() : %s", get_camera_gui_error_str( guierror ) );
		}
*/
		if( b_show_dialog )
		{
			dlg_display();
			b_show_dialog = false;
		}

		de = dll_digiclops.digiclopsSetFrameRate( _digiclops_context, digiclops_bandwidth[get_triclops()->get_bandwidth()] );
		if( de == DIGICLOPS_OK )
			GOOD_PRINT_STRING( "called digiclopsSetFrameRate(): ok" );
		else
		{
			ERR_PRINT_STRING( "calling digiclopsSetFrameRate(): %s", get_digiclops_error_str(de) );
			goto exit_on_error;
		}

		// get the camera module configuration
		de = dll_digiclops.digiclopsGetTriclopsContextFromCamera( _digiclops_context, &_triclops_context );
		if( de == DIGICLOPS_OK )
			GOOD_PRINT_STRING( "called digiclopsGetTriclopsContextFromCamera(): ok" );
		else
		{
			ERR_PRINT_STRING( "calling digiclopsGetTriclopsContextFromCamera() : %s", get_digiclops_error_str(de) );
			goto exit_on_error;
		}

		te = dll_triclops.triclopsGetSerialNumber( _triclops_context, &_serial_number );
		if( te == TriclopsErrorOk )
			GOOD_PRINT_STRING( "called triclopsGetSerialNumber(): ok" );
		else
		{
			ERR_PRINT_STRING( "calling triclopsGetSerialNumber() : %d", te );
			goto exit_on_error;
		}

		// set the digiclops to deliver the stereo image and right (color) image
		de = dll_digiclops.digiclopsSetImageTypes( _digiclops_context, STEREO_IMAGE | RIGHT_IMAGE );
		if( de == DIGICLOPS_OK )
			GOOD_PRINT_STRING( "called digiclopsSetImageTypes(): ok" );
		else
		{
			ERR_PRINT_STRING( "calling digiclopsSetImageTypes() : %s", get_digiclops_error_str(de) );
			goto exit_on_error;
		}

		// set the Digiclops resolution
		// use 'HALF' resolution when you need faster throughput, especially for
		// color images
		// digiclopsSetImageResolution( digiclops, DIGICLOPS_HALF );
		de = dll_digiclops.digiclopsSetImageResolution( _digiclops_context, DIGICLOPS_FULL );
		if( de == DIGICLOPS_OK )
			GOOD_PRINT_STRING( "called digiclopsSetImageResolution(): ok" );
		else
		{
			ERR_PRINT_STRING( "calling digiclopsSetImageResolution() : %s", get_digiclops_error_str(de) );
			goto exit_on_error;
		}

		DigiclopsOutputImageResolution pOutputImage;
		de = dll_digiclops.digiclopsGetImageResolution( _digiclops_context, &pOutputImage);
		INT32	size_x = 320;
		INT32	size_y = 240;
		switch( pOutputImage )
		{
		case DIGICLOPS_FULL:
			size_x = 640;
			size_y = 480;
			break;
		case DIGICLOPS_HALF:
			size_x = 320;
			size_y = 240;
			break;
		}
		set_flux_size_format( size_x, size_y, aaa::PIXEL_FORMAT::DISP_16 );

		_b_opened = true;
		GOOD_PRINT_STRING( "%s() done : ok", __FUNCTION__ );
	}

	return AAA_OK;
exit_on_error:
	ERR_PRINT_STRING( "exiting %s() on error", __FUNCTION__ );
	return ERR_ANY;
}

void	c_capture_triclops::close_specific()
{
	if( _b_opened )
	{
		stop();
		_b_opened = false;
	}
}

void	c_capture_triclops::ask_frame()
{
 //		TriclopsError			te;
 //		DigiclopsError			de;

	//// grab the image set
	//de = digiclopsGrabImage( _digiclops_context );

	//// grab the stereo data
	//de = digiclopsExtractTriclopsInput( _digiclops_context, STEREO_IMAGE, &_img_stereo_data );

	//// grab the color image data
	//// (note: if you are using a B&W Digiclops, this will of course be in B&W)
	//de = digiclopsExtractTriclopsInput( _digiclops_context, RIGHT_IMAGE, &_img_color_data );

	//// preprocessing the images
	//te = triclopsPreprocess( _triclops_context, &_img_stereo_data );

	//// stereo processing
	//te = triclopsStereo( _triclops_context ) ;

	//// retrieve the interpolated depth image from the context
	//te = triclopsGetImage16( _triclops_context, TriImg16_DISPARITY, TriCam_REFERENCE, &_img_depth_16 );

	//// retrieve RIGHT IMAGE
	//te = triclopsGetImage( _triclops_context, TriImg_RECTIFIED, TriCam_L_RIGHT, &_img_reference );

	//got_frame( _img_reference.data );
}

c_image_flux*	c_capture_triclops::get_image_flux( INT32 index )
{
	if( _b_streaming )
	{
		if ( index == 1 )
			return _image_2_type == TRI_IMAGE_NO ? nullptr : _img_2;
		else if ( index == 2 )
			return _image_3_type == TRI_IMAGE_NO ? nullptr : _img_3;
	}
	return nullptr;
}

void	c_capture_triclops::generate_lut()
{
	static bool		invalidInitted = false;
	TriclopsError	error;
	UINT8			ucMinDisparityMap;
	UINT8			ucMaxDisparityMap;
	int				nMinDisparity;
	int				nMaxDisparity;
	int				nDisparityOffset;
	int				i;

	error = dll_triclops.triclopsGetDisparity( _triclops_context, &nMinDisparity, &nMaxDisparity );
	error = dll_triclops.triclopsGetDisparityOffset( _triclops_context, &nDisparityOffset );
	error = dll_triclops.triclopsGetDisparityMapping( _triclops_context,  &ucMinDisparityMap, &ucMaxDisparityMap );

	// do not generate table if nothing has changed.
	if(( nMinDisparity == m_nLUTMinDisp ) && ( nMaxDisparity == m_nLUTMaxDisp ) &&
		( ucMinDisparityMap == m_ucLUTMinDispMap ) && ( ucMaxDisparityMap == m_ucLUTMaxDispMap ) )
	{
		return;
	}

	// store values that we're dealing with so we don't compute next time.
	m_nLUTMinDisp		= nMinDisparity-nDisparityOffset;
	m_nLUTMaxDisp		= nMaxDisparity-nDisparityOffset;
	m_ucLUTMinDispMap	= ucMinDisparityMap;
	m_ucLUTMaxDispMap	= ucMaxDisparityMap;

	// get the invalid pixel mappings
	UINT8	ucInvalidTexture;
	UINT8	ucInvalidUniqueness;

	dll_triclops.triclopsGetTextureValidationMapping( _triclops_context, &ucInvalidTexture );
	dll_triclops.triclopsGetUniquenessValidationMapping( _triclops_context, &ucInvalidUniqueness );

	float fScale = ((float)(m_ucLUTMaxDispMap - m_ucLUTMinDispMap)) / ((float)(m_nLUTMaxDisp-m_nLUTMinDisp))/256.0f;

	// handle valid pixels
	for( i = 0; i < DISPARITY_VALID_LUT_ENTRIES; ++i )
	{
		if( i << DISPARITY_LUT_SHIFT_BITS < 0xFF00 )
		{
			m_ucSubpixelValidDisparityMapLUT[ i ]= (UINT8)(((float)((i<<DISPARITY_LUT_SHIFT_BITS) - (m_nLUTMinDisp<<8)))*fScale + (float)ucMinDisparityMap);
		}
	}

	// handle invalid pixels
	if ( !invalidInitted )
	{
		invalidInitted = true;
		m_ucSubpixelInvalidDisparityMapLUT = new UINT8[256];

		for ( i = 0; i < 256; ++i )
		{
			if ( i == ucInvalidTexture )
			{
				m_ucSubpixelInvalidDisparityMapLUT[i] = TRI_RED;
			}
			else if (i == ucInvalidUniqueness )
			{
				m_ucSubpixelInvalidDisparityMapLUT[i] = TRI_GREEN;
			}
			else
			{
				m_ucSubpixelInvalidDisparityMapLUT[i] = TRI_BLUE;
			}
		}
	}
}
#endif //#if !AAA_WIN64()

namespace n_triclops
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 32;
	CONSTEXPR INT32 GROUP_NB		= 0;
    
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_INT32_LOCKED(	serial_number )
		PARAM_DEF_SYMBO_PSTR_ZERO(	bandwidth_use,		digiclops_bandwidth_str )
		PARAM_DEF_SYMBO_PSTR_ZERO(	image_type,			c_triclops_ui::triclops_camera_image_str )

		PARAM_DEF_SYMBO_PSTR_ZERO(	image_2_type,		c_triclops_ui::triclops_camera_image_str )
		PARAM_DEF_BIND_2D_ALONE(	image_2_bind		)

		PARAM_DEF_SYMBO_PSTR_ZERO(	image_3_type,		c_triclops_ui::triclops_camera_image_str )
		PARAM_DEF_BIND_2D_ALONE(	image_3_bind		)

		PARAM_DEF_REAL_ZERO(		disp_min			)
		PARAM_DEF_REAL_ONE(			disp_max			)

		PARAM_DEF_INT32(			stereo_resolution_x,				1, 320,		1, 640		)
		PARAM_DEF_INT32(			stereo_resolution_y,				1, 240,		1, 480		)
		PARAM_DEF_INT32_POS(		disparity_min,						6, 6		)
		PARAM_DEF_INT32_POS(		disparity_max,						16, 16		)
		PARAM_DEF_SYMBO_PSTR_ZERO(	stereo_quality,						c_triclops_ui::triclops_stereo_quality_str )
		PARAM_DEF_INT32_POS(		stereo_mask,						11, 11		)
		PARAM_DEF_BOOL_OFF(			edge_correlation					)
		PARAM_DEF_INT32_POS(		edge_mask,							11, 11		)
		PARAM_DEF_BOOL_OFF(			sub_pixel_interpolation				)
		PARAM_DEF_BOOL_OFF(			sub_pixel_interpolation_strict		)
		PARAM_DEF_INT32(			sub_pixel_interpolation_mapping,	1, 1,		0., 255			)
		PARAM_DEF_BOOL_OFF(			surface_validation					)
		PARAM_DEF_INT32(			surface_validation_size,			200, 200,	1, PARAM_MAX_INT32 )
		PARAM_DEF_REAL_POS(			surface_validation_difference,		1., 0.5		)
		PARAM_DEF_INT32(			surface_validation_mapping,			1, 0,		0., 255			)
		PARAM_DEF_BOOL_OFF(			texture_validation					)
		PARAM_DEF_REAL(				texture_validation_threshold,		1., 0.5,	0., 128.0		)
		PARAM_DEF_INT32(			texture_validation_mapping,			1, 0,		0., 255			)
		PARAM_DEF_BOOL_OFF(			uniqueness_validation				)
		PARAM_DEF_REAL(				uniqueness_validation_threshold,	1., 0.5,	0., 10.0		)
		PARAM_DEF_INT32(			uniqueness_validation_mapping,		1, 0,		0., 255			)
		PARAM_DEF_BOOL_OFF(			backforth_validation				)
		PARAM_DEF_INT32(			backforth_validation_mapping,		1, 0,		0., 255			)
	};
}

void	c_triclops_ui::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, _serial_number_ui );
	param_set_pt( h, _bandwidth );
	param_set_pt( h, _image_type_ui );
	param_set_pt( h, _image_2_type_ui );
	param_set_pt( h, _image_2_bind_ui );
	param_set_pt( h, _image_3_type_ui );
	param_set_pt( h, _image_3_bind_ui );
	param_set_pt( h, _disp_min_ui );
	param_set_pt( h, _disp_max_ui );
	param_set_pt( h, _stereo_resolution_x_ui );
	param_set_pt( h, _stereo_resolution_y_ui );

	param_set_pt( h, _disparity_min_ui );
	param_set_pt( h, _disparity_max_ui );

	param_set_pt( h, _stereo_quality_ui );
	param_set_pt( h, _stereo_mask_ui );
	param_set_pt( h, _b_edge_correlation_ui );
	param_set_pt( h, _edge_mask_ui );

	param_set_pt( h, _b_sub_pixel_interpolation_ui );
	param_set_pt( h, _b_sub_pixel_interpolation_strict_ui );
	param_set_pt( h, _sub_pixel_validation_mapping_ui );

	param_set_pt( h, _b_surface_validation_ui );
	param_set_pt( h, _surface_validation_size_ui );
	param_set_pt( h, _surface_validation_difference_ui );
	param_set_pt( h, _surface_validation_mapping_ui );

	param_set_pt( h, _b_texture_validation_ui );
	param_set_pt( h, _texture_validation_threshold_ui );
	param_set_pt( h, _texture_validation_mapping_ui );

	param_set_pt( h, _b_uniqueness_validation_ui );
	param_set_pt( h, _uniqueness_validation_threshold_ui );
	param_set_pt( h, _uniqueness_validation_mapping_ui );

	param_set_pt( h, _b_backforth_validation_ui );
	param_set_pt( h, _backforth_validation_mapping_ui );

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_triclops_ui )
,_serial_number_ui(0)
{
	param_init_with( n_triclops::param, n_triclops::PARAM_NB_MAX );
}

EMPTY_DESTRUCTOR( c_triclops_ui )

#endif //#if AAA_USE_POINT_GREY()