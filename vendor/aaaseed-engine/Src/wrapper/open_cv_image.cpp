#include "open_cv_image.h"
#include "obj_ui/bdd/bdd_image/bdd_img.h"
#include "image/bind_img_2d.h"
#include "spy.h"

//todo decide what we do with opl Mat Umap gpu opencl cuda ...

#include "opencv2/imgproc/imgproc_c.h"


void	c_img_ipl::release()
{
	if( _b_owner && _ipl )
	{
		if( _b_only_header )
		{
			cvReleaseImageHeader( &_ipl );
			_b_only_header = false;
		}
		else
		{
			cvReleaseImage( &_ipl );
		}
		_b_owner = false;
		_bind_index = -1;
	}
}

//todo we do this in different locations without the special float case we do here and should be generalized
bool	c_img_ipl::build_using( c_img_2d CONST * CONST src, bool& b_changed )
{
	//refine this too
	
//	int depth = c_aaa_cv::get_opencv_depth_from_pixel_type( src->get_data_type(), __FUNCTION__ );
	int depth = (src->get_data_type() == aaa::PIXEL_TYPE::FLOAT_32) ? IPL_DEPTH_32F : IPL_DEPTH_8U;

	if(	get_ipl() && _b_owner )	//todo test type here too
	{
		if( !is_equal_size( src->get_size_x(), src->get_size_y() )
			//	||	get_depth() !=  1
			||	src->get_channel_nb() != get_channel_nb()
			//	||	!_b_only_header
			//	||	!_b_owner
			)
		{
			release();
		}
	}
	if( !_ipl )
	{
		CvSize	size;
		size.width = src->get_size_x();
		size.height = src->get_size_y();
		
		_ipl = cvCreateImageHeader( size, depth, src->get_channel_nb() );
		//todo print_string( "Create Source image header : %d channel", _ipl_in->nChannels );
		_b_owner = true;
		_b_only_header = true;
		b_changed = true;
	}
	if( _ipl )
	{
		//todo check only at creation
		CHAR*	data = (CHAR *) src->get_data();
		if( data )
		{
			_ipl->imageData = data;
			return true;
		}
		else
		{
			ERR_PRINT_STRING( "%s() try to use img src with NULL data", __FUNCTION__ );
		}
	}
	return false;
}

void	c_img_ipl::set_changed()
{
	if( _bind_index >= 0 )
	{
		c_img_2d* img = g_bind_img_2d->get( _bind_index );
		img->erase_filename();
		img->set_changed();
	}
}

bool	c_img_ipl::build( INT32 CONST img_index, c_img_ipl& ref, C_PCHAR_C mess, INT32 CONST channel_nb )
{
	if( ref.get_ipl() )
	{
		if(	get_ipl() && _b_owner )
		{
			if( !is_equal_size( ref.get_size_x(), ref.get_size_y() )
				//	||	get_depth() !=  1
				||	channel_nb != get_channel_nb()
				//	||	!_b_only_header
				//	||	!_b_owner
			)
			{
				release();
			}
		}
		if( !_ipl )
		{
			CvSize size;
			size.width = ref.get_size_x();
			size.height = ref.get_size_y();
			_ipl = cvCreateImageHeader( size, ref.get_depth(), channel_nb );
			_ipl = cvCreateImageHeader( size, IPL_DEPTH_8U, channel_nb );
			//todo print_string( "Create Source image header : %d channel", _ipl_in->nChannels );
			_b_owner = true;
			_b_only_header = true;
		}
		c_img_2d* img = g_bind_img_2d->get_img( img_index, aaa::c_pixel_format::make_format_from_channel_type( get_channel_nb() ), get_size_x(), get_size_y(), true, nullptr, __FUNCTION__ );
		if( img && _ipl )
		{
			//todo check only at creation
			CHAR*	data = (CHAR *)img->get_data_uint8();
			if( data )
			{
				_ipl->imageData = data;
				//	patching cvCreateImageHeader to avoid crash in cvSet
				//		due to a an overflow because of align pbs
				//		322 width rgb image got a widthStep of 968 and crash but 966 don't crash
				_ipl->widthStep = _ipl->width * _ipl->nChannels;
				_bind_index = img_index;
				return true;
			}
			else
			{
				ERR_PRINT_STRING( "%s() try to use img src with NULL data", __FUNCTION__ );
			}
		}	
	}
	return false;
}

bool	c_img_ipl::build( INT32 CONST img_index, c_img_ipl& ref, C_PCHAR_C mess )
{ 
	return build( img_index, ref, mess, ref.get_channel_nb() );
}

bool	c_img_ipl::create( c_img_ipl& ref, INT32 CONST depth, INT32 CONST channel_nb, C_PCHAR_C mess )
{
	if(	get_ipl() && _b_owner )
	{
		if( !is_equal_size( ref.get_size_x(), ref.get_size_y() )
			//	||	get_depth() !=  1
			||	channel_nb != get_channel_nb()
			//	||	!_b_only_header
			//	||	!_b_owner
			)
		{
			release();
		}
	}
	if( !_ipl )
	{
		CvSize	size;
		size.width = ref.get_size_x();
		size.height = ref.get_size_y();
		_ipl = cvCreateImage( size, depth, channel_nb );
		//todo print_string( "Create Source image header : %d channel", _ipl_in->nChannels );
		_b_owner = true;
		_b_only_header = false;
	}
	return _ipl != nullptr;
}

void	ipl::to_gray( p_ipl CONST src, p_ipl CONST dst, aaa::COMPO CONST s_compo )
{
	if( !src || !dst )
		return;

	using namespace aaa;
	// Note: the destination must have one channel.
	if( src->nChannels != 1 && dst->nChannels == 1 ) 
	{
		switch(s_compo)
		{
		case COMPO::RED:
		case COMPO::GREEN:
		case COMPO::BLUE:
		case COMPO::ALPHA:
			{
				INT32 coi = cvGetImageCOI(src);
				INT32 tmp;
				switch( s_compo )
				{
				case COMPO::RED:	tmp = 1; break;
				case COMPO::GREEN:	tmp = 2; break;
				case COMPO::BLUE:	tmp = 3; break;
				case COMPO::ALPHA:	tmp = 4; break;
				}
				cvSetImageCOI( src, tmp );
				cvCopy( src, dst );
				cvSetImageCOI( src, coi );
			}
			break;
		case COMPO::GREY:
			if( strcmpi( src->colorModel, "BGRA" ) == 0 )
				cvCvtColor( src, dst, CV_BGRA2GRAY );
			else if( strcmpi( src->colorModel, "BGR" ) == 0 )
				cvCvtColor( src, dst, CV_BGR2GRAY );
			else if( strcmpi( src->colorModel, "RGB" ) == 0 )
				cvCvtColor( src, dst, CV_RGB2GRAY );
			break;
		//todo add missing types
		default:
			err_print( "%s() This type %s not unimplemented.", __FUNCTION__, str_compo[ INT32(s_compo) ] );
			break;
		}
	}
	else
	{
		if( src!=dst )
			cvCopy( src, dst );
	}
}

void	ipl::release( p_ipl& p )
{
	if( p )
	{
		//p->imageData = nullptr;
		cvReleaseImage( &p );
	}
}
//todo redo gray image


void	ipl::create_print( p_ipl CONST p_ipl, C_PCHAR_C str )
{
	GOOD_PRINT_STRING( "OpenCv image Created : %s with %d channel", str, p_ipl->nChannels );
}

bool	ipl::create( p_ipl& dst, p_ipl CONST ref, INT32 CONST depth, INT32 CONST channel_nb, C_PCHAR_C mess  )
{
	if( dst )
	{
		if(		dst->width == ref->width
			&&	dst->height == ref->height
			&&	dst->depth == depth
			&&	dst->nChannels == channel_nb
			)
		{
			return true;
		}
		cvReleaseImage( &dst );
	}
	if( !dst )
	{
		dst = cvCreateImage( cvSize( ref->width, ref->height ), depth, channel_nb );
		dst->origin = ref->origin;  // same vertical flip as source
		create_print( dst, mess );
		return true;
	}
	return false;
}

bool	ipl::create_gray( p_ipl& dst, p_ipl CONST ref, INT32 CONST depth, C_PCHAR_C mess )
{
	return	create( dst, ref, depth, 1, mess  );
}
bool	ipl::create( p_ipl& dst, p_ipl CONST ref, C_PCHAR_C mess )	
{
	return	create( dst, ref, ref->depth, ref->nChannels, mess );
}

c_img_2d*	ipl::to_aaa( p_ipl CONST src, INT32 CONST image_dst_index )
{
	if( src->nChannels!= 1 && src->nChannels!= 3 )
		return nullptr;

	c_img_2d*	p_img = g_bind_img_2d->get_with_image_size( image_dst_index, src->width, src->height, aaa::c_pixel_format::make_format_from_channel_type( src->nChannels ) );

	// todoq handle other depths like IPL_DEPTH_32S, ...
	if( p_img )
	{
		UINT8*	data = p_img->get_data_uint8();
		if( !data )
		{
			ERR_PRINT_STRING( "%s() try to use img src with NULL data", __FUNCTION__ );
		}
		else if( src->nChannels ==  1 )
		{
			c_img_ipl	img( src );
			--data;
			for( INT32 j=0; j < src->height; ++j )
			{
				for( INT32 i=0; i < src->width; ++i )
				{
					if( src->depth == IPL_DEPTH_32F )	//todoq treat other case
					{
						*++data = (UINT8)(img.get_pixel<pixel_gray_float>( i, j ) / 256.f );
					}
					else
					{
						*++data = img.get_pixel<pixel_gray>( i, j );
					}
				}
			}
			goto ok;
		}
		else if( src->nChannels ==  3 )
		{
			c_img_ipl	img( src );
			--data;
			for( INT32 j=0; j < src->height; ++j )
			{
				for( INT32 i=0; i < src->width; ++i )
				{
					pixel_rgb tmp = img.get_pixel<pixel_rgb>( i, j );
					*++data = tmp.b;
					*++data = tmp.g;
					*++data = tmp.r;
				}
			}
			goto ok;
		}
		else
		{
			ERR_PRINT_STRING( "ipl format unknown" );
		}
	}
	return nullptr;
ok:
	p_img->erase_filename();
	p_img->set_changed();
	return p_img;
}


//Maa	taken from opencv 2, we anticipate change in the 3.0
void AAA_cvSmooth( const CvArr* src, CvArr* dst,
					 int smoothtype CV_DEFAULT(CV_GAUSSIAN),
					 int size1 CV_DEFAULT(3),
					 int size2 CV_DEFAULT(0),
					 double sigma1 CV_DEFAULT(0),
					 double sigma2 CV_DEFAULT(0));

void
AAA_cvSmooth( const void* srcarr, void* dstarr, int smooth_type,
		int param1, int param2, double param3, double param4 )
{
	cv::Mat src = cv::cvarrToMat(srcarr), dst0 = cv::cvarrToMat(dstarr), dst = dst0;

	if( dst.size() != src.size() )
	{
		ERR_PRINT_STRING( "%s() size of src/dst differ", __FUNCTION__ );
		return;
	}
	if( smooth_type != CV_BLUR_NO_SCALE && dst.type() != src.type() )
	{
		ERR_PRINT_STRING( "%s() type of src/dst differ", __FUNCTION__ );
		return;
	}

	if( param2 <= 0 )
	{
		param2 = param1;
	}
	switch( smooth_type )
	{
	case CV_BLUR:
	case CV_BLUR_NO_SCALE:
		SPY_PUSH_RANGE( "cv::boxFilter", spy::IMG_LOW );
			cv::boxFilter( src, dst, dst.depth(), cv::Size(param1, param2), cv::Point(-1,-1), smooth_type == CV_BLUR, cv::BORDER_REPLICATE );
		SPY_POP_RANGE();
		break;
	case CV_GAUSSIAN:
		SPY_PUSH_RANGE( "cv::GaussianBlur", spy::IMG_LOW );
			cv::GaussianBlur( src, dst, cv::Size(param1, param2), param3, param4, cv::BORDER_REPLICATE );
		SPY_POP_RANGE();
		break;
	case CV_MEDIAN:
		SPY_PUSH_RANGE( "cv::medianBlur", spy::IMG_LOW );
			cv::medianBlur( src, dst, param1 );
		SPY_POP_RANGE();
		break;
	default:
		SPY_PUSH_RANGE( "cv::bilateralFilter", spy::IMG_LOW );
			cv::bilateralFilter( src, dst, param1, param3, param4, cv::BORDER_REPLICATE );
		SPY_POP_RANGE();
		break;
	}

	if( dst.data != dst0.data )
	{
		ERR_PRINT_STRING( "%s() formats of input/output arrays differ", __FUNCTION__ );
	}
}

bool	ipl::cv_smooth( IplImage CONST * CONST src, IplImage* CONST dst, INT32 CONST smooth_method, INT32 CONST size_x, INT32 CONST size_y )
{
	INT32 sx = size_x;
	INT32 sy = size_y;

	if( (sx & 1)==0 )
		sx += 1;
	if( (sy & 1)==0 )
		sy += 1;

	switch( smooth_method )
	{
	case CV_BLUR_NO_SCALE:
		{
			INT32 nb_ch = src->nChannels;
			if( nb_ch!=1 )
			{
				ERR_PRINT_STRING( "CV_BLUR_NO_SCALE filter only implemented for 1 channel images (src and dst). skipping it ." );
				return false;
			}
			nb_ch = dst->nChannels;
			if( nb_ch!=1 )
			{
				ERR_PRINT_STRING( "CV_BLUR_NO_SCALE filter only implemented for 1 channel images (src and dst). skipping it ." );
				return false;
			}
		}
		break;
	case CV_BILATERAL:
		{
			INT32 nb_ch = src->nChannels;
			if( nb_ch!=1 && nb_ch!=3 )
			{
				ERR_PRINT_STRING( "BILATERAL filter only implemented for 1 or 3 channel images. skipping it ." );
				return false;
			}
			sy = sx;	//	only implemented this way
		}
		break;
	}

// in 2.4.8.0 replaced by 
//	C++: void GaussianBlur(InputArray src, OutputArray dst, Size ksize, double sigmaX, double sigmaY = 0, int borderType = BORDER_DEFAULT)�
//	C++: void blur(InputArray src, OutputArray dst, Size ksize, Point anchor = Point(-1, -1), int borderType = BORDER_DEFAULT)
//	C++ : void medianBlur(InputArray src, OutputArray dst, int ksize)
//	C++ : void bilateralFilter(InputArray src, OutputArray dst, int d, double sigmaColor, double sigmaSpace, int borderType = BORDER_DEFAULT)
	AAA_cvSmooth( src, dst, smooth_method, sx, sy );
	return true;
}


