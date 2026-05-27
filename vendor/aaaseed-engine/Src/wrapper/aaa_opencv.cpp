#include "aaa_opencv.h"

#include "opencv2/core/ocl.hpp"

#include "image/img.h"

//#include "OpenCL/aaa_opencl.h"

#pragma message ( "\tOpenCV compiling for version " AAA_LIB_USE_OPENCV_VERSION_ASKED() )

#include <lib_use.h>
#define OPENCV_LIB_PATH_PRIVATE(s) "OpenCv/opencv"AAA_LIB_USE_OPENCV_VERSION_STR()"/opencv_"#s##AAA_LIB_USE_OPENCV_VERSION_STR()
#if	AAA_DEBUG()
#	define AAA_LIB_USE_CV(s)		AAA_LIB_USE( OPENCV_LIB_PATH_PRIVATE( s )"d")
#else
#	define AAA_LIB_USE_CV(s)		AAA_LIB_USE( OPENCV_LIB_PATH_PRIVATE( s ) )
#endif


#if	AAA_LIB_USE_OPENCV_VERSION() < 400
#	error "AAASeed don't support these versions (openCV Major version < 4) "
		//AAA_LIB_USE_CV( world			)
		//AAA_LIB_USE_CV( calib3d		)
		//AAA_LIB_USE_CV( core			)
		//AAA_LIB_USE_CV( features2d	)
		//AAA_LIB_USE_CV( imgproc		)
		//AAA_LIB_USE_CV( objdetect		)
		//AAA_LIB_USE_CV( photo			)
		//AAA_LIB_USE_CV( video			)
		//AAA_LIB_USE_CV( videoio		)
#else
#	if CV_VERSION_MAJOR != 4
#		error "OpenCV includes have a incompatible Major version (Not 4), for Visual C++ : edit the include dirs"
#	endif
#	if AAA_LIB_USE_OPENCV_VERSION() == 410
#		if CV_VERSION_MINOR != 1
#			error "OpenCV includes have a incompatible Minor version (Not 1), for Visual C++ : edit the include dirs"
#		endif
#		if CV_VERSION_REVISION != 0
			__pragma( message( "OpenCV includes have a incompatible Revision version : " AAA_STRING(CV_VERSION_REVISION) " instead of 0)" ) )
//#			error "OpenCV includes have a incompatible Revision version (Not 0), for Visual C++ : edit the include dirs"
#		endif
#	elif AAA_LIB_USE_OPENCV_VERSION() == 412
#		if CV_VERSION_MINOR != 1
#			error "OpenCV includes have a incompatible Minor version (Not 1), for Visual C++ : edit the include dirs"
#		endif
#		if CV_VERSION_REVISION != 2
#			error "OpenCV includes have a incompatible Revision version (Not 2), for Visual C++ : edit the include dirs"
#		endif
#	elif AAA_LIB_USE_OPENCV_VERSION() == 4100
#		if CV_VERSION_MINOR != 10
#			error "OpenCV includes have a incompatible Minor version (Not 10), for Visual C++ : edit the include dirs"
#		endif
#		if CV_VERSION_REVISION != 0
#			error "OpenCV includes have a incompatible Revision version (Not 0), for Visual C++ : edit the include dirs"
#		endif
#	elif AAA_LIB_USE_OPENCV_VERSION() == 4120
#		if CV_VERSION_MINOR != 12
#			error "OpenCV includes have a incompatible Minor version (Not 12), for Visual C++ : edit the include dirs"
#		endif
#		if CV_VERSION_REVISION != 0
#			error "OpenCV includes have a incompatible Revision version (Not 0), for Visual C++ : edit the include dirs"
#		endif
#	elif AAA_LIB_USE_OPENCV_VERSION() == 4130
#		if CV_VERSION_MINOR != 13
#			error "OpenCV includes have a incompatible Minor version (Not 13), for Visual C++ : edit the include dirs"
#		endif
#		if CV_VERSION_REVISION != 0
#			error "OpenCV includes have a incompatible Revision version (Not 0), for Visual C++ : edit the include dirs"
#		endif	
#	else
#		error AAA_LIB_USE_OPENCV_VERSION() for version 4 can only be 410 412 or 4100
#	endif
AAA_LIB_USE_CV( "world" )
#endif

#if AAA_LIB_USE_OPENCV_CUDA()
	//AAA_LIB_USE_CV( cudaarithm	)
	//AAA_LIB_USE_CV( cudabgsegm	)
	//AAA_LIB_USE_CV( cudafilters	)
	//AAA_LIB_USE_CV( cudaimgproc	)
	//AAA_LIB_USE_CV( cudastereo	)
	//AAA_LIB_USE_CV( cudawarping	)
#endif	//AAA_LIB_USE_OPENCV_CUDA

#undef OPENCV_LIB_PATH_PRIVATE



bool	aaa::c_cv::b_display_info_trig			= false;
bool	aaa::c_cv::b_use_ipp_ui					= true;
static	bool	b_use_ipp;

bool	aaa::c_cv::b_opencl_can_out				= false;
bool	aaa::c_cv::b_opencl_allow_ui;
bool	aaa::c_cv::b_opencl_use_out				= false;
o_str	aaa::c_cv::opencl_device_wanted;

bool	aaa::c_cv::b_cuda_start_with			= false;
bool	aaa::c_cv::b_cuda_allow_ui				= false;
bool	aaa::c_cv::b_cuda_use_out				= false;
//bool	aaa::c_cv::b_opencl_start_with			= false;
INT32	aaa::c_cv::cuda_device_nb				= 0;
UINT32	aaa::c_cv::cuda_device_index_asked_ui	= 0;
UINT32	aaa::c_cv::cuda_device_index_used;

// we force it because there is no direct way to detect it from the dll
bool	aaa::c_cv::b_opengl_can_out				=
#ifdef HAVE_OPENGL	// this in the openCV Include or not, totally unclear
	true;	
#else
	false;
#endif
bool	aaa::c_cv::b_opengl_allow_ui			= false;
bool	aaa::c_cv::b_opengl_use_out				= false;

INT32	aaa::c_cv::opencv_version_major			= 0;
INT32	aaa::c_cv::opencv_version_minor			= 0;
INT32	aaa::c_cv::opencv_version_revision		= 0;
//UINT32	aaa::c_cv::ocl_device_use;

//todo use	cvSetMemoryManager
C_PCHAR	aaa::c_cv::get_lib_info_str()
{
	return cv::getBuildInformation().c_str();
}

void	aaa::c_cv::c_display_info_opencl()
{
	if( !cv::ocl::haveOpenCL() )
	{
		ERR_PRINT_STRING( "OpenCV : No OpenCL" );
	}
	else
	{
		PRINT_CR();
		GOOD_PRINT_STRING( "OpenCV and OpenCL : Begin --------------------" );
		GOOD_PRINT_STRING( "OpenCV : Have OpenCL" );
//		{
//			std::vector<cv::ocl::PlatformInfo> platform_info;
//			cv::ocl::getPlatfomsInfo( platform_info );
//			ocl_platform_nb = platform_info.size();
//			for( auto const & info : platform_info )
//				GOOD_PRINT_STRING( "cv::ocl %d device from %s : %s - %s", info.deviceNumber(), info.vendor().c_str(), info.name().c_str(), info.version().c_str() );
//		}

		CONST cv::ocl::Device& device = cv::ocl::Device::getDefault();
		if( device.ptr() )
		{
			GOOD_PRINT_STRING( "cv::ocl used device is : %s",	device.name().c_str() );
			GOOD_PRINT_STRING( "\tversion\t: %s",				device.version().c_str() );
			GOOD_PRINT_STRING( "\tvendor\t: %s",				device.vendorName().c_str() );
			GOOD_PRINT_STRING( "\tOpenCL_C_Version\t: %s",		device.OpenCL_C_Version().c_str() );
			GOOD_PRINT_STRING( "\tOpenCLVersion\t: %s",			device.OpenCLVersion().c_str() );
			GOOD_PRINT_STRING( "\tdevice version major\t: %d",	device.deviceVersionMajor() );
			GOOD_PRINT_STRING( "\tdevice version minor\t: %d",	device.deviceVersionMinor() );
			GOOD_PRINT_STRING( "\tdriver version\t: %s",		device.driverVersion().c_str() );
		}
		else
		{
			GOOD_PRINT_STRING( "cv::ocl no default device for now." );
		}


#if AAA_LIB_USE_OPENCV_VERSION() < 330
//todox64	vector destruction trigger exception
			{
				AAA_OPENCL::init();
			}

			cv::ocl::Context context;
			if ( !context.create( cv::ocl::Device::TYPE_DEFAULT ) )
			{
				ERR_PRINT_STRING( "Failed creating the context..." );
				return;
			}

			GOOD_PRINT_STRING( "cv::ocl %d GPU devices are detected.", context.ndevices() );
			for( UINT8 i = 0; i < context.ndevices(); i++ )
			{
				cv::ocl::Device device = context.device( i );
				GOOD_PRINT_STRING( "cv::ocl %d device %s, available %s, image support %s, OpenCL C Version %s", i, device.name().c_str(), device.available() ? "YES" : "NO", device.imageSupport() ? "YES" : "NO", device.OpenCL_C_Version().c_str() );
			}

			cv::ocl::Device( context.device( ocl_device_use ) ); //Here is where you change which GPU to use (e.g. 0 or 1)

			if( context.ndevices() > 0 )
			{
				auto ctx = cv::ogl::ocl::initializeContextFromGL();
//				auto ctx = cv::ocl::initializeContextFromGL();
			}
#endif //#if AAA_LIB_USE_OPENCV_VERSION() < 330
		GOOD_PRINT_STRING( "OpenCV and OpenCL : End ----------------------" );
		PRINT_CR();
	}
}

void	aaa::c_cv::c_display_info_cuda()
{
	cuda_device_nb = 0;
#if AAA_LIB_USE_OPENCV_CUDA()
	PRINT_CR();
	GOOD_PRINT_STRING( "OpenCV and CUDA : Begin --------------------" );
	if( b_cuda_start_with )
	{
		INT32 nb = cv::cuda::getCudaEnabledDeviceCount();
		cuda_device_nb = (nb<=0) ? 0 : nb;
		if( nb == -1 )
			GOOD_PRINT_STRING( "OpenCV : CUDA driver is not installed or is incompatible." );
		else if( nb == 0 )
			GOOD_PRINT_STRING( "OpenCV : was compiled without CUDA support" );
		else
		{
			GOOD_PRINT_STRING( "OpenCV : %d CUDA devices :", cuda_device_nb );
			for( INT32 i=0; i<cuda_device_nb; ++i )
			{
				//GOOD_PRINT_STRING( "OpenCV : CUDA device index %d", i );
				cv::cuda::printShortCudaDeviceInfo( i );	
			}
		}
	}
	else
	{
		DBG_PRINT_STRING( "OpenCV : opencv_start_with_cuda set to OFF forcing 0 CUDA device" );
	}

	if( cuda_device_nb > 0 )
	{
		cuda_device_index_used = ::MIN( UINT32(cuda_device_nb-1), cuda_device_index_asked_ui );
		cv::cuda::setDevice( cuda_device_index_used );
		GOOD_PRINT_STRING( "OpenCV : use CUDA device %d", cuda_device_index_used );
		cv::cuda::printCudaDeviceInfo( cuda_device_index_used );
		//cv::cuda::setGlDevice( cuda_device_index_used );	//crash on 412 / 41000=
	}
	GOOD_PRINT_STRING( "OpenCV and CUDA : End ----------------------" );
	PRINT_CR();
#else
	if( b_cuda_start_with )
	{
		ERR_PRINT_STRING( "OpenCV : this version of AAASeed compiled for OpenCV with no CUDA." ); 
		ERR_PRINT_STRING( "OpenCV :   change opencv_start_with_cuda set to OFF to avoid this message with this version." );
	}
#endif

}

void	aaa::c_cv::c_display_info()
{
	//CONST std::string& info = cv::getBuildInformation();
	//GOOD_PRINT_STRING(	"OpenCV : %s",	info.c_str()	);
	GOOD_PRINT_STRING( "OpenCV version wanted at AAASeed build is %d / %s", AAA_LIB_USE_OPENCV_VERSION(), AAA_LIB_USE_OPENCV_VERSION_ASKED() );
	GOOD_PRINT_STRING( "OpenCV includes used for AAASeed build are      version %d.%d rev %d %s", CV_VERSION_MAJOR, CV_VERSION_MINOR, CV_VERSION_REVISION, CV_VERSION_STATUS );
	GOOD_PRINT_STRING( "OpenCV dynamic libraries loaded for AAASeed are version %d.%d rev %d", opencv_version_major, opencv_version_minor, opencv_version_revision );
	GOOD_PRINT_STRING( "OpenCV used library version : %s", get_lib_info_str() );

	c_display_info_cuda();
	c_display_info_opencl();
}

namespace {
	int error_callback( int status, const char* func_name,
							const char* err_msg, const char* file_name,
							int line, void* userdata )
	{
		ERR_PRINT_STRING( "OPENCV : in fn %.256s in file %.256s at line %d", func_name, file_name, line );
		ERR_PRINT_STRING( "\t%.4000s", err_msg );	//
		return 0;
	}
};

void	aaa::c_cv::c_init()
{
	cvSetErrMode( CV_ErrModeParent );
	cvRedirectError( error_callback );

	b_use_ipp = !b_use_ipp_ui;

	//	b_use_opencl	= !b_use_opencl_ui;
	o_str o( "OPENCV_OPENCL_DEVICE=" );
	o.add( opencl_device_wanted );
	_putenv( o.get() );

	opencv_version_major	= cv::getVersionMajor();
	opencv_version_minor	= cv::getVersionMinor();
	opencv_version_revision = cv::getVersionRevision();

	c_update();
	b_opencl_can_out = cv::ocl::haveOpenCL();
	c_display_info();
}

void	aaa::c_cv::c_update()
{
	if( b_use_ipp != b_use_ipp_ui )
	{
		b_use_ipp = b_use_ipp_ui;
		if( b_use_ipp )
			GOOD_PRINT_STRING( "Loading and plugging Intel Integrated Performance Primitives DLL" );
		else
			GOOD_PRINT_STRING( "Unplugging Intel Integrated Performance Primitives DLL" );
		cvUseOptimized( b_use_ipp ? 1 : 0 );
	}

	bool b_opencl_asked = b_opencl_can_out && b_opencl_allow_ui;
	if( b_opencl_use_out != b_opencl_asked )
	{
//		if( bool_opencl_wanted )	GOOD_PRINT_STRING( "OpenCV will now try to use OpenCl" );
//		else						GOOD_PRINT_STRING( "OpenCV will now disable his use of OpenCl");
		cv::ocl::setUseOpenCL( b_opencl_asked );
		b_opencl_use_out = cv::ocl::useOpenCL();
		if( b_opencl_use_out != b_opencl_asked )
		{
			ERR_PRINT_STRING( "OPENCV Error OpenCL can is : %s, and use is : %s.", b_opencl_can_out?"True":"False", b_opencl_use_out?"True":"False" );
			if( b_opencl_asked )
			{
				ERR_PRINT_STRING( "OPENCV forcing OpenCL can to false" );
				b_opencl_can_out = false;
			}
		}
	}

	if( cuda_device_nb > 0 )
	{
		if( ::MIN( cuda_device_index_asked_ui, cuda_device_nb-1 ) != cuda_device_index_used ) 
			c_display_info_cuda();
	}
	b_cuda_use_out = b_cuda_allow_ui && (cuda_device_nb > 0);

#if AAA_OPENCV_GL_USE()	
	b_opengl_use_out = b_opengl_allow_ui && b_opengl_can_out;
#endif

	if( b_display_info_trig )
	{
		b_display_info_trig = false;
		c_display_info();
	}
}

INT32	aaa::c_cv::get_cv_type( aaa::PIXEL_FORMAT format )
{
	switch( format )
	{
	case aaa::PIXEL_FORMAT::R_8:		return CV_8UC1;
	case aaa::PIXEL_FORMAT::R_16:		return CV_16UC1;
	case aaa::PIXEL_FORMAT::DEPTH_16:
	case aaa::PIXEL_FORMAT::R_16FP:		return CV_16FC1;
	case aaa::PIXEL_FORMAT::DEPTH_32:
	case aaa::PIXEL_FORMAT::R_32FP:		return CV_32FC1;

	case aaa::PIXEL_FORMAT::RG_8:		return CV_8UC2;
	case aaa::PIXEL_FORMAT::RG_16:		return CV_16UC2;
	case aaa::PIXEL_FORMAT::RG_16FP:	return CV_16FC2;
	case aaa::PIXEL_FORMAT::RG_32FP:	return CV_32FC2;

	case aaa::PIXEL_FORMAT::RGB_8:
	case aaa::PIXEL_FORMAT::BGR_8:		return CV_8UC3;
	case aaa::PIXEL_FORMAT::RGB_16:
	case aaa::PIXEL_FORMAT::BGR_16:		return CV_16UC3;
	case aaa::PIXEL_FORMAT::RGB_16FP:
	case aaa::PIXEL_FORMAT::BGR_16FP:	return CV_16FC3;
	case aaa::PIXEL_FORMAT::RGB_32FP:
	case aaa::PIXEL_FORMAT::BGR_32FP:	return CV_32FC3;

	case aaa::PIXEL_FORMAT::RGBA_8:
	case aaa::PIXEL_FORMAT::BGRA_8:		return CV_8UC4;
	case aaa::PIXEL_FORMAT::RGBA_16:
	case aaa::PIXEL_FORMAT::BGRA_16:	return CV_16UC4;
	case aaa::PIXEL_FORMAT::RGBA_16FP:
	case aaa::PIXEL_FORMAT::BGRA_16FP:	return CV_16FC4;
	case aaa::PIXEL_FORMAT::RGBA_32FP:
	case aaa::PIXEL_FORMAT::BGRA_32FP:	return CV_32FC4;

	default:							return CV_8UC4;
	}
}

aaa::PIXEL_FORMAT	aaa::c_cv::get_pixel_format_from_cv( INT32 CONST cv_type, bool CONST b_bgr )
{
	switch ( cv_type )
	{
	case CV_8UC1:	return aaa::PIXEL_FORMAT::R_8;
	case CV_16UC1:	return aaa::PIXEL_FORMAT::R_16;
	case CV_16FC1:	return aaa::PIXEL_FORMAT::R_16FP;
	case CV_32FC1:	return aaa::PIXEL_FORMAT::R_32FP;

	case CV_8UC2:	return aaa::PIXEL_FORMAT::RG_8;
	case CV_16UC2:	return aaa::PIXEL_FORMAT::RG_16;
	case CV_16FC2:	return aaa::PIXEL_FORMAT::RG_16FP;
	case CV_32FC2:	return aaa::PIXEL_FORMAT::RG_32FP;

	case CV_8UC3:	return b_bgr	?	aaa::PIXEL_FORMAT::BGR_8		:	aaa::PIXEL_FORMAT::RGB_8;
	case CV_16UC3:	return b_bgr	?	aaa::PIXEL_FORMAT::BGR_16		:	aaa::PIXEL_FORMAT::RGB_16;
	case CV_16FC3:	return b_bgr	?	aaa::PIXEL_FORMAT::BGR_16FP		:	aaa::PIXEL_FORMAT::RGB_16FP;
	case CV_32FC3:	return b_bgr	?	aaa::PIXEL_FORMAT::BGR_32FP		:	aaa::PIXEL_FORMAT::RGB_32FP;

	case CV_8UC4:	return b_bgr	?	aaa::PIXEL_FORMAT::BGRA_8		:	aaa::PIXEL_FORMAT::RGBA_8;
	case CV_16UC4:	return b_bgr	?	aaa::PIXEL_FORMAT::BGRA_16		:	aaa::PIXEL_FORMAT::RGBA_16;
	case CV_16FC4:	return b_bgr	?	aaa::PIXEL_FORMAT::BGRA_16FP	:	aaa::PIXEL_FORMAT::RGBA_16FP;
	case CV_32FC4:	return b_bgr	?	aaa::PIXEL_FORMAT::BGRA_32FP	:	aaa::PIXEL_FORMAT::RGBA_32FP;

	default:		return aaa::PIXEL_FORMAT::UNKNOWN;
	}
}
int					aaa::c_cv::get_opencv_depth_from_pixel_type( aaa::PIXEL_TYPE CONST type_src, C_PCHAR_C fn_name )
{
	int depth;
	switch( type_src )
	{
	case aaa::PIXEL_TYPE::FLOAT_32:		depth =  IPL_DEPTH_32F;	break;
	case aaa::PIXEL_TYPE::UINT_16:		depth =  IPL_DEPTH_16U;	break;
	case aaa::PIXEL_TYPE::UINT_8:		depth =  IPL_DEPTH_8U;	break;
	default:
		DBG_PRINT_STRING( "%s() AAASeed don't deal with %s", fn_name, c_pixel_format::get_pixel_type_name(type_src) );
		depth =  IPL_DEPTH_8U;
		break;
	}
	return depth;
}

bool	aaa::c_cv::img_to_cv_umat( c_img_2d* img, cv::UMat& mat )
{
	if( IS_NOT_NULL( img ) )
	{
		cv::Mat	tmp( img->get_size_y(), img->get_size_x(), get_cv_type( img->get_pixel_format() ), (UINT8 *)img->get_data_uint8() );
		mat = tmp.getUMat( cv::ACCESS_RW );
		return true;
	}
	return false;
}

bool	aaa::c_cv::img_to_cv_mat( c_img_2d* img, cv::Mat& mat )
{
	if( IS_NOT_NULL( img ) )
	{
		cv::Mat	tmp( img->get_size_y(), img->get_size_x(), get_cv_type( img->get_pixel_format() ), (UINT8 *)img->get_data_uint8() );
		mat = tmp;
		return true;
	}
	return false;
}


// needed to hack in draw_contours
#define	CV_DRAWING_STORAGE_BLOCK	((1 << 12) - 256)
typedef	struct	CvPolyEdge
{
	int	x, dx;
	union
	{
		struct	CvPolyEdge*	next;
		int	y0;
	};
	int	y1;
} CvPolyEdge;
static	CONST	cv::Point	icvCodeDeltas[8] =	{ {1, 0}, {1, -1}, {0, -1}, {-1, -1}, {-1, 0}, {-1, 1}, {0, 1}, {1, 1} };
#define	XY_SHIFT	16
#define	XY_ONE		(1 << XY_SHIFT)

static	cv::Scalar_<double>	CV_GREEN = CV_RGB( 0, 255, 0 );
#define	CV_RGBA( r, g, b, a )	cvScalar( (b), (g), (r), (a) )
static	CvScalar	CV_GREEN_ALPHA = CV_RGBA( 0, 255, 0, 255 );
#define	CV_AA		16

#define __BEGIN__ __CV_BEGIN__
#define __END__  __CV_END__
#define EXIT __CV_EXIT__


FINLINE	void	aaa::c_cv::draw_line( cv::Point CONST p0, cv::Point CONST p1, void CONST * CONST color )
{
	cv::line( _line_dst->getMat(cv::ACCESS_RW), p0, p1, CV_GREEN_ALPHA, 1, 8, 0 );	// draw a line from p0 to P1
	/*	GOL::begin( GL_LINES );
	GOL::vertex2f( REAL(p0.x)/256. , REAL(p0.y)/256. );
	GOL::vertex2f( REAL(p1.x)/256. , REAL(p1.y)/256. );
	GOL::end();
	*/
}

//	build from opencv icvCollectPolyEdges
FINLINE	void	aaa::c_cv::collect_poly_edges( CvSeq* CONST v, CvContour* CONST edges, void CONST * CONST color, int CONST line_type, int CONST shift )
{
	UINT32	count = (UINT32)v->total;
	CvRect	bounds = edges->rect;
	int		elem_type = CV_MAT_TYPE(v->flags);

	CvSeqReader reader;
	CvSeqWriter writer;

	cvStartReadSeq( v, &reader );
	cvStartAppendToSeq( (CvSeq*)edges, &writer );

	for( UINT32 i = 0; i < count; ++i )
	{
		cv::Point pt0, pt1, t0, t1;
		CvPolyEdge edge;
		CV_READ_EDGE( pt0, pt1, reader );

		if( elem_type == CV_32SC2 )
		{
			pt0.x = (pt0.x) << (XY_SHIFT - shift);
			pt0.y = (pt0.y) >> shift;
			pt1.x = (pt1.x) << (XY_SHIFT - shift);
			pt1.y = (pt1.y) >> shift;
		}
		else
		{
			Cv32suf x, y;
			assert( shift == 0 );

			x.i = pt0.x; y.i = pt0.y;
			pt0.x = cvRound( (x.f) * XY_ONE );
			pt0.y = cvRound(y.f);
			x.i = pt1.x; y.i = pt1.y;
			pt1.x = cvRound( (x.f) * XY_ONE );
			pt1.y = cvRound(y.f);
		}

		if( line_type < CV_AA )
		{
			t0.y = pt0.y; t1.y = pt1.y;
			t0.x = (pt0.x + (XY_ONE >> 1)) >> XY_SHIFT;
			t1.x = (pt1.x + (XY_ONE >> 1)) >> XY_SHIFT;
			draw_line( t0, t1, color );
		}
		else
		{
			t0.x = pt0.x; t1.x = pt1.x;
			t0.y = pt0.y << XY_SHIFT;
			t1.y = pt1.y << XY_SHIFT;
			draw_line( t0, t1, color );
		}

		if( pt0.y == pt1.y )
			continue;

		if( pt0.y > pt1.y )
			CV_SWAP( pt0, pt1, t0 );

		bounds.y = ::MIN( bounds.y, pt0.y );
		bounds.height = ::MAX( bounds.height, pt1.y );

		if( pt0.x < pt1.x )
		{
			bounds.x = ::MIN( bounds.x, pt0.x );
			bounds.width = ::MAX( bounds.width, pt1.x );
		}
		else
		{
			bounds.x = ::MIN( bounds.x, pt1.x );
			bounds.width = ::MAX( bounds.width, pt0.x );
		}

		edge.y0 = pt0.y;
		edge.y1 = pt1.y;
		edge.x = pt0.x;
		edge.dx = (pt1.x - pt0.x) / (pt1.y - pt0.y);
		assert( edge.y0 < edge.y1 );

		CV_WRITE_SEQ_ELEM( edge, writer );
	}

	edges->rect = bounds;
	cvEndWriteSeq( &writer );
}

// Draw contour in texture and extract points to polyline
void	aaa::c_cv::process_contour( c_polyline_2d* polyline, std::vector<cv::Point> cont,
									double CONST * CONST col_external, double CONST * CONST col_hole, int CONST thickness, int CONST line_type )
{
	size_t n = cont.size();
	if( polyline )
		polyline->clear();
	for( size_t i = 0; i < n-1; i++ )
	{
		cv::line( _line_dst->getMat(cv::ACCESS_RW), cont[i], cont[i+1], CV_GREEN_ALPHA, thickness, line_type, 0 );
		if( polyline )
			polyline->add_point( (REAL(cont[i].x)+.5f) * _x_factor, (REAL(cont[i].y)+.5f) * _y_factor );
	}
}
// Extract contour points to polyline
void	aaa::c_cv::extract_contour( c_polyline_2d* polyline, std::vector<cv::Point> cont )
{
	size_t n = cont.size();
	if( polyline )
		polyline->clear();
	for( size_t i = 0; i < n-1; i++ )
	{
		if( polyline )
			polyline->add_point( (REAL(cont[i].x)+.5f) * _x_factor, (REAL(cont[i].y)+.5f) * _y_factor );
	}
}
/*
void	aaa::c_cv::draw_contours( c_polyline_2d* polyline, cv::UMat*  img,  std::vector< std::vector<cv::Point> > cont,
											double* col_external, double* col_hole, 
											int  maxLevel, int thickness,
											int line_type )
{
	CV_FUNCNAME( "aaa::c_cv::draw_contours" );

	// Copy pasta from opencv drawing.cpp cv::draw_contours(...)
	CvMat image = img->getMat( cv::ACCESS_RW );
	CvMat _cimage = cvMat( image );

	size_t ncontours = cont.size();
	size_t i = 0, first = 0, last = ncontours;
	std::vector<CvSeq> seq;
	std::vector<CvSeqBlock> block;

	if( !last )
		return;

	seq.resize(last);
	block.resize(last);
	
	for( i = first; i < last; i++ )
	{
		size_t n = cont[i].size();
		for( size_t j = 0; j < n-1; j++ )
		{
			draw_line( cont[i][j], cont[i][j+1], col_external );
		}
	}
   /* for( i = first; i < last; i++ )
		seq[i].first = 0;

	for( i = first; i < last; i++ )
	{
		cv::Mat ci = cv::Mat( cont[i] );
		if( ci.empty() )
			continue;
		int npoints = ci.checkVector(2, CV_32S);
		CV_Assert( npoints > 0 );
		cvMakeSeqHeaderForArray( CV_SEQ_POLYGON, sizeof(CvSeq), sizeof(cv::Point),
								 ci.ptr(), npoints, &seq[i], &block[i] );
	}

	for( i = first; i < last; i++ )
	{
		seq[i].h_next = i < last-1 ? &seq[i+1] : 0;
		seq[i].h_prev = i > first ? &seq[i-1] : 0;
	}
	// End of copy pasta from opencv drawing.cpp cv::draw_contours(...)

	CvSeq*	contour = &seq[first];
	CvSeq*	contour0 = contour;
	CvSeq*	h_next = 0;
	CvMemStorage*	st = 0;
	__BEGIN__;

	CvMat*	mat = &_cimage;
	int		coi = 0;
	CvMat	stub;

	CV_CALL( mat = cvGetMat( mat, &stub, &coi ) );

	if( line_type == CV_AA && CV_MAT_DEPTH(mat->type) != CV_8U )
		line_type = 8;

	if( !contour )
		EXIT;

	if( coi != 0 )
		CV_ERROR( CV_BadCOI, "cvUnsupportedFormat" );

	if( thickness < -1 || thickness > 255 )
		CV_ERROR( CV_StsOutOfRange, "" );

	if( maxLevel < 0 )
	{
		h_next = contour->h_next;
		contour->h_next = 0;
		maxLevel = -maxLevel+1;
	}

	CvSeq*		tseq = 0;
	CvContour*	edges = 0;
	if( thickness < 0 )
	{
		if( contour->storage )
			st = cvCreateChildMemStorage( contour->storage );
		else
			st = cvCreateMemStorage( CV_DRAWING_STORAGE_BLOCK );
		tseq = cvCreateSeq( 0, sizeof(CvContour), sizeof(cv::Point), st );
		edges = (CvContour*)cvCreateSeq( 0, sizeof(CvContour), sizeof(CvPolyEdge), st );
	}

	CvSeqWriter	writer;
	memset( &writer, 0, sizeof(writer) );

	CvTreeNodeIterator iterator;
	cvInitTreeNodeIterator( &iterator, contour, maxLevel );

	if( polyline )
		polyline->clear();
	while( (contour = (CvSeq*)cvNextTreeNode( &iterator )) != 0 )
	{
		CvSeqReader	reader;
		UINT32			count = (UINT32)contour->total;
		int			elem_type = CV_MAT_TYPE(contour->flags);
		void*		clr = (contour->flags & CV_SEQ_FLAG_HOLE) == 0 ? col_external : col_hole;

		cvStartReadSeq( contour, &reader, 0 );

		if( CV_IS_SEQ_CHAIN_CONTOUR( contour ))
		{
			cv::Point	pt = ((CvChain*)contour)->origin;
			cv::Point	prev_pt = pt;
			char		prev_code = reader.ptr ? reader.ptr[0] : '\0';

			if( thickness < 0 )
			{
				cvClearSeq( tseq );
				cvStartAppendToSeq( tseq, &writer );
				CV_WRITE_SEQ_ELEM( pt, writer );
			}

			for( UINT32 i = 0; i < count; ++i )
			{
				char	code;
				CV_READ_SEQ_ELEM( code, reader );

				assert( (code & ~7) == 0 );

				if( code != prev_code )
				{
					prev_code = code;
					if( thickness >= 0 )
					{
					//	draw_line( prev_pt, pt, clr);	//	, thickness, line_type, 2, 0 );
					}
					else
					{
						CV_WRITE_SEQ_ELEM( pt, writer );
					}
					prev_pt = pt;
				}
				pt.x += icvCodeDeltas[(int)code].x;
				pt.y += icvCodeDeltas[(int)code].y;
			}

			if( thickness >= 0 )
			{
			//	draw_line( prev_pt, ((CvChain*)contour)->origin, clr);	// thickness, line_type, 2, 0 );
			}
			else
			{
				CV_WRITE_SEQ_ELEM( pt, writer );
				cvEndWriteSeq( &writer );
				CV_CALL( collect_poly_edges( tseq, edges, col_external, line_type, 0 ) );
			}
		}
		else if( CV_IS_SEQ_POLYLINE( contour ))
		{
			if( thickness >= 0 )
			{
				cv::Point	pt1, pt2;
				//int shift = 0;

				count -= !CV_IS_SEQ_CLOSED(contour);
				if( polyline )
					polyline->set_closed( CV_IS_SEQ_CLOSED(contour) );
				if( elem_type == CV_32SC2 )
				{
					CV_READ_SEQ_ELEM( pt1, reader );
				}
				else
				{
					CvPoint2D32f	pt1f;
					CV_READ_SEQ_ELEM( pt1f, reader );
					pt1.x = cvRound( (pt1f.x) * XY_ONE );
					pt1.y = cvRound( (pt1f.y) * XY_ONE );
					//shift = XY_SHIFT;
				}
				if( polyline )
					polyline->add_point( (REAL(pt1.x)+.5f) * _x_factor, (REAL(pt1.y)+.5f) * _y_factor );

				for( UINT32 i = 0; i < count; ++i )
				{
					if( elem_type == CV_32SC2 )
					{
						CV_READ_SEQ_ELEM( pt2, reader );
					}
					else
					{
						CvPoint2D32f	pt2f;
						CV_READ_SEQ_ELEM( pt2f, reader );
						pt2.x = cvRound( pt2f.x * XY_ONE );
						pt2.y = cvRound( pt2f.y * XY_ONE );
					}
					if( polyline )
						polyline->add_point( (REAL(pt2.x)+.5f) * _x_factor, (REAL(pt2.y)+.5f) * _y_factor );
				//	draw_line( pt1, pt2, clr);	// thickness, line_type, 2, shift );
					pt1 = pt2;
				}
			}
			else
			{
				CV_CALL( collect_poly_edges( contour, edges, col_external, line_type, 0 ));
			}
		}
	}
	//	
	if( polyline )	//	the end point is a repeat of the first one
		polyline->pop_point();

	//	if( thickness < 0 )
	//	{
	//		CV_CALL( icvFillEdgeCollection( mat, edges, ext_buf ));
	//	}

	__END__;

	if( h_next && contour0 )
		contour0->h_next = h_next;

	cvReleaseMemStorage( &st );
}
*/
/*
void	aaa::c_cv::extract_contours( c_polyline_2d* polyline, std::vector< std::vector<cv::Point> > cont )
{
//	CV_FUNCNAME( "aaa::c_cv::extract_contours" );
	
	// Copy pasta from opencv drawing.cpp cv::draw_contours(...)
	size_t ncontours = cont.size();
	size_t i = 0, first = 0, last = ncontours;
	std::vector<CvSeq> seq;
	std::vector<CvSeqBlock> block;

	if( !last )
		return;

	seq.resize(last);
	block.resize(last);

	for( i = first; i < last; i++ )
		seq[i].first = 0;

	for( i = first; i < last; i++ )
	{
		cv::Mat ci = cv::Mat( cont[i] );
		if( ci.empty() )
			continue;
		int npoints = ci.checkVector(2, CV_32S);
		CV_Assert( npoints > 0 );
		cvMakeSeqHeaderForArray( CV_SEQ_POLYGON, sizeof(CvSeq), sizeof(cv::Point),
								 ci.ptr(), npoints, &seq[i], &block[i] );
	}

	for( i = first; i < last; i++ )
	{
		seq[i].h_next = i < last-1 ? &seq[i+1] : 0;
		seq[i].h_prev = i > first ? &seq[i-1] : 0;
	}
	// End of copy pasta from opencv drawing.cpp cv::draw_contours(...)
	CvSeq*	contour = &seq[first];
	if( !contour )
		return;
	if( !polyline )
		return;

	CvSeq*	contour0 = contour;
	CvSeq*	h_next  = contour->h_next;
	contour->h_next = 0;

	__BEGIN__;

	CvTreeNodeIterator	iterator;
	cvInitTreeNodeIterator( &iterator, contour, 2 );

	if( polyline )
		polyline->clear();

	while( (contour = (CvSeq*)cvNextTreeNode( &iterator )) != 0 )
	{
		CvSeqReader	reader;
		int			count = contour->total;
		int			elem_type = CV_MAT_TYPE(contour->flags);
	
		cvStartReadSeq( contour, &reader, 0 );

		if( CV_IS_SEQ_CHAIN_CONTOUR( contour ) )
		{
		}
		else if( CV_IS_SEQ_POLYLINE( contour ) )
		{
			cv::Point	pt1;
			bool b_closed = CV_IS_SEQ_CLOSED(contour);
			polyline->set_closed( b_closed );
			if( !b_closed )
				--count;

			for( ; count >= 0; --count )
			{
				if( elem_type == CV_32SC2 )
				{
					CV_READ_SEQ_ELEM( pt1, reader );
				}
				else
				{
					CvPoint2D32f pt1f;
					CV_READ_SEQ_ELEM( pt1f, reader );
					pt1.x = cvRound( (pt1f.x) * XY_ONE );
					pt1.y = cvRound( (pt1f.y) * XY_ONE );
				}
				polyline->add_point( (REAL(pt1.x)+.5f) * _x_factor, (REAL(pt1.y)+.5f) * _y_factor );
			}
		}
	}
	polyline->pop_point();	//	the end point is a repeat of the first one
		
	__END__;

	if( h_next && contour0 )
		contour0->h_next = h_next;
}
*/
bool	aaa::c_cv::build_comment_status( o_str* CONST o, bool CONST b_on, aaa::c_cv::STATUS CONST status )
{
	if( b_on )
	{
		C_PCHAR str = nullptr;
		switch( status )
		{
		case STATUS::NO:					str = "No";						break;
		case STATUS::ASKED:					str = "Asked";					break;
		case STATUS::STARTED:				str = "Started";				break;
		case STATUS::DONE:					str = "OK";						break;
		case STATUS::ERR_FLOAT:				str = "Not On Float";			break;
		case STATUS::ERR_UINT8_ONLY:		str = "channel 8bits Only";		break;
		case STATUS::ERR_CH_1_ONLY:			str = "1 channel Only";			break;
		case STATUS::ERR_CH_13_ONLY:		str = "1 or 3 channel Only";	break;
		case STATUS::ERR_CH_134_ONLY:		str = "1,3 or 4 channel Only";	break;			
		case STATUS::ERR_CH_123_ONLY:		str = "1,2 or 3 channel Only";	break;
		case STATUS::ERR_CH_14_ONLY:		str = "1 or 4 channel Only";	break;
		case STATUS::ERR_CH_34_ONLY:		str = "3 or 4 channel Only";	break;
		case STATUS::ERR_CH_4_ONLY:			str = "4 channel Only";			break;
//		case STATUS::ERR_CHANNEL:			str = "Wrong channel nb";		break;
		case STATUS::ERR_CPU:				str = "No CPU support";			break;
		case STATUS::ERR_GPU:				str = "No GPU support";			break;
		case STATUS::ERR_CV_EXCEPTION:		str = "OpenCV exception";		break;
		case STATUS::ERR_UNIMPLEMENTED:		str = "Unimplemented";			break;
		default:							str = "status Unknown";
											ERR_PRINT_STRING( "In %s() got an Unknow status %d", __FUNCTION__, status );
											break;
		}
		if( str )
		{
			o->set( str );
			return status >= STATUS::ERR_FLOAT;
		}
	}
	o->erase();
	return false;
}


