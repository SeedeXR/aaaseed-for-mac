
#ifndef AAA_CAPTURE_VIDEO_H
#	include "capture_video.h"
#endif

#ifndef AAA_PTGREY_UTILS_H
#	include "PointGrey/ptgrey_utils.h"
#endif
#if AAA_USE_POINT_GREY()
#	include "PointGrey/capture_censys.h"
#	include "PointGrey/capture_flycapture.h"
#	include "PointGrey/capture_flycapture2.h"
#	include "PointGrey/capture_triclops.h"
#endif	//#if AAA_USE_POINT_GREY()

#include "capture_display.h"
#include "capture_dshow.h"
#include "capture_ps3_eye.h"
#include "capture_duo3d.h"
#include "capture_jai.h"
#include "capture_realsense.h"
#include "capture_vfw.h"
#include "kinect/capture_kinect.h"
#include "uEye/capture_ueye.h"
#include "../tex_video.h"		 
#include "obj_ui/tracker/trackers.h"


static INT32	device_enum_count = 0;
void c_capture::inc_device_enum_count()
{
	++device_enum_count;
}
INT32 c_capture::get_device_enum_count()
{
	return device_enum_count;
}
//static	INT32 CONST &		get_device_enum_count_ref();
//INT32 CONST & c_capture_dshow::get_device_enum_count_ref()	{	return device_enum_id_count;	}

#define	CAPTURE_HEADER  "# CAPTURE "
void	c_capture::CAPTURE_PRINT_STRING( C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	trackers::PRINT_STRING_VA( CAPTURE_HEADER, fmt, args );
	va_end( args );
}


C_PCHAR_C	c_capture::size_str[static_cast<INT32>(CAPTURE_SIZE::MAX_NB)] =
{
	"Custom",
	"PAL",
	"NTSC",
	"720p/i",
	"1920p/i",
};

bool	c_capture::b_init_ptgrey_ui;
bool	c_capture::b_init_jai_ui;

bool	c_capture::b_verbose = false;	//todo un ui : c_tex_video_master
void c_capture::set_verbose( bool b_in )
{
	if( b_verbose != b_in )
	{
		b_verbose = b_in;
		SWITCH_PRINT_STATE( "Capture Verbose", b_verbose );
	}
}

void c_capture::flip_verbose()
{
	set_verbose( !b_verbose );
}

void	c_capture::c_init()
{
#if AAA_USE_POINT_GREY()
	if( b_init_ptgrey_ui )
	{
#	if AAA_USE_POINT_GREY_PGRFLYCAPTURE()
		c_capture_flycapture::c_init();
#	endif
		c_capture_flycap2::c_init();
		c_capture_triclops::c_init();
		c_capture_censys::c_init();
	}
#endif	//#if AAA_USE_POINT_GREY()

	c_capture_vfw::c_init();
	c_capture_dshow::c_init();
#if AAA_USE_PS3EYE()
	c_capture_ps3_eye::c_init();
#endif //#if AAA_USE_PS3EYE()
	c_capture_kinect::c_init();
	c_capture_ueye::c_init();
	c_capture_display::c_init();

#if AAA_USE_DUO3D()
	c_capture_duo3d::c_init();
#endif	//#if AAA_USE_DUO3D()

	if( b_init_jai_ui )
		c_capture_jai::c_init();
	c_capture_realsense::c_init();
}

void	c_capture::c_deinit()
{
#if AAA_USE_POINT_GREY()
	c_capture_triclops::c_deinit();
	c_capture_censys::c_deinit();
	c_capture_flycap2::c_deinit();
#	if AAA_USE_POINT_GREY_PGRFLYCAPTURE()
	//c_capture_flycapture::c_deinit();
#	endif
#endif	//#if AAA_USE_POINT_GREY()
	c_capture_vfw::c_deinit();
	c_capture_dshow::c_deinit();
#if AAA_USE_PS3EYE()
	//c_capture_ps3_eye::c_deinit();
#endif //#if AAA_USE_PS3EYE()
	c_capture_kinect::c_deinit();
	c_capture_ueye::c_deinit();
#if AAA_USE_DUO3D()
	c_capture_duo3d::c_deinit();
#endif	//#if AAA_USE_DUO3D()
	c_capture_display::c_deinit();
	c_capture_jai::c_deinit();
	c_capture_realsense::c_deinit();
}

void	c_capture::trig_device_changed()
{
	c_capture_dshow::do_enum( true );
}

void	c_capture::stop()
{
	if( _b_opened )
		_b_opened = false;
}

c_capture::c_capture()
	: c_image_flux				(	nullptr, true	)	//todo implement a non NULL buf for c_image_flux ?
	,_b_opened					(	false			)
	,_device_index_used			(	-1				)
	,_b_stream					(	false			)
	,_b_running					(	false			)
	,_b_streaming				(	false			)
	,_b_preview					(	false			)
	,_hd_wind					(	nullptr			)
	,_s_field					(	0				)
	,_frames_dropped			(	-1				)
	,_frames_delivered			(	-1				)
	,_crossbar_index			(	-1				)
	,_o_crossbar_name			(	"None"			)
	,_b_capture_force_default	(	false			)
	,_b_capture_force_fps		(	false			)
	,_avi_size					(	.0f				)
	,_avi_frame_nb				(	0				)
	,_image_flux_count			(	1				)
{
}

c_capture::~c_capture()
{
	stop();
}

bool	c_capture::open( INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_id )
{
	IF_THIS_NULL_RETURN_FALSE();

	//GOOD_PRINT_STRING( "%s last was %s", filename, _filename_open_and_valid.get() );
	_device_index_used = -1;
	if( NOERR( open_specific( index, framerate, device_id ) ) )
	{
//		set_capturing( true );
		return true;
	}
	return false;
}

void	c_capture::close()
{
	close_specific();
	_device_index_used = -1;
//	set_capturing( false );
}


void	c_capture::set_default_size( INT32 CONST x, INT32 CONST y )
{	_capture_size_x = x;
	_capture_size_y = y;
}
