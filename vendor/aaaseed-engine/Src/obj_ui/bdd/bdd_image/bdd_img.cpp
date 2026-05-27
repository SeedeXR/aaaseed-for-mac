#include "bdd_img.h"
#include "image/bind_img.h"
#include "image/bind_img_2d.h"
#include "image/img_master.h"
#include "opencv2/core/ocl.hpp"
#include "spy.h"


#if AAA_LIB_USE_OPENCV_CUDA()
#	include "opencv2/cudaimgproc.hpp"
#	include "opencv2/cudaarithm.hpp"
#	include "opencv2/cudawarping.hpp"
#	include "opencv2/cudafilters.hpp"
#endif

#include "draw/tex.h"
#include <opencv2/core/opengl.hpp>


FACTORY_ABSTRACT_CREATE_V1( c_bdd_img_base, bdd_img_base, Bdd Image Base );

CONSTRUCTOR_ABSTRACT_CREATE( c_bdd_img_base )	{}
EMPTY_DESTRUCTOR( c_bdd_img_base )

//todo probably need to be used in more bdd_img than bdd_img_analyse (Maa Sept 2017)
bool	c_bdd_img_base::process_img_changed( c_img_2d* img )
{
	//	we do it only when image changed
	//		perhaps we need a force compute so we do it also when param change
	if( !img || img->is_state_unique( _unique_id_last ) )
		return false;
	set_unique_id_last( img->get_state_unique() );
	inc_process_index();
	return true;
}


C_PCHAR_C	c_bdd_img::str_rotation[CV_ROT_MAX] =
{
	"None",
	"90",
	"180",
	"270"
};

C_PCHAR_C	c_bdd_img::str_src_2_resize[RESIZE_SRC_MAX] =
{
	"No",
	"scr_a",
	"src_b"
};

/*
	INTER_NEAREST        = 0,
	// bilinear interpolation
	INTER_LINEAR         = 1,
	// bicubic interpolation
	INTER_CUBIC          = 2,
	// resampling using pixel area relation. It may be a preferred method for image decimation, as
	//  it gives moire'-free results. But when the image is zoomed, it is similar to the INTER_NEAREST
	//  method.
	INTER_AREA           = 3,
	// Lanczos interpolation over 8x8 neighborhood
	INTER_LANCZOS4       = 4,
	// mask for interpolation codes
	INTER_MAX            = 7,
	// flag, fills all of the destination image pixels. If some of them correspond to outliers in the
	//  source image, they are set to zero
	WARP_FILL_OUTLIERS   = 8,
	// flag, inverse transformation
	// For example, polar transforms:
	//  - flag is __not__ set: \f$dst( \phi , \rho ) = src(x,y)\f$
	//  - flag is set: \f$dst(x,y) = src( \phi , \rho )\f$
	WARP_INVERSE_MAP     = 16
*/
C_PCHAR_C	c_bdd_img::str_interpolation_mode[RESIZE_INTER_MAX] =
{
	"Nearest",
	"Linear",
	"Cubic",
	"Area",
	"Lanczos4"
};

C_PCHAR_C	c_bdd_img::str_img_base[2]	=	{	"base", "no_move_to_gpu"	};

c_bdd_img*	c_bdd_img::bdd_img_cur = nullptr;

FACTORY_ABSTRACT_CREATE_V1( c_bdd_img, bdd_img, Bdd Image );

namespace
{
	o_str o_src_dst;
}

INT32	c_bdd_img::param_init_pt_start()
{
	INT32	h = param_init_pt_no_geo();

	param_set_pt( h, get_pt_active()		);
	param_set_pt( h, _b_always_update_ui	);
	param_set_pt( h, get_process_index_pt()	);

	return h;
}

void	c_bdd_img::param_init_pt_src( INT32& h, bool b_src_2, bool b_resize )
{
	c_param* p;

	o_src_dst.set( _b_swap_src_ui ? _src_b_img_index : get_src_img_index_ui() );
	if( b_src_2 )
	{
		o_src_dst.add_char( ',' );
		o_src_dst.add( _src_b_img_index );
	}
	o_src_dst.add( "->" );
	o_src_dst.add( _dst_img_index_ui );
	if( _b_cuda_use_ui )
		o_src_dst.add( " Cuda" );
	if( _b_mono_ui )
		o_src_dst.add( " Mono" );
	if( b_resize )
	{
		if( _b_crop )
			o_src_dst.add( " Crop" );
		if( _b_resize )
			o_src_dst.add( " Resize" );
		if( _b_rotate_ui )
			o_src_dst.add( " Rotate" );
		o_src_dst.add_space();
		o_src_dst.add( _resize_size[0] );
		o_src_dst.add_char( 'x' ); 
		o_src_dst.add( _resize_size[1] );
	}
	get_param(h)->set_comment( o_src_dst );

	++h;
		param_set_pt( h, _b_verbose_ui				);
		param_attach_obj( h, _prev					);
		param_set_pt( h, get_src_img_index_ui_pt()	);
		param_set_pt( h, _b_src_img_prev_ui			);
		param_set_pt_2( h, _src_size_ui				);
		param_set_pt( h, _o_src_pixel_format		);
		param_set_pt( h, _dst_img_index_ui			);
		param_set_pt_2( h, _dst_size_out			);
		param_set_pt( h, _o_pixel_format_out		);
		param_set_pt( h, _b_keep_data_ui			);

		p = get_param(h);
		if( aaa::c_cv::b_cuda_use_out )
		{
			p->clear_comment();
			p->set_unused( false );
		}
		else
		{
			p->set_comment( (aaa::c_cv::cuda_device_nb > 0) ? "Disabled" : "No device" );
			p->set_unused( true );
		}
		p->set_pt( &_b_cuda_use_ui );
		++h;

		p = get_param(h);
		if( aaa::c_cv::b_opencl_use_out )
		{
			p->clear_comment();
			p->set_unused( false );
		}
		else
		{
			p->set_comment( (aaa::c_cv::b_opencl_can_out) ? "Disabled" : "Can't" );
			p->set_unused( true );
		}
		p->set_pt( &_b_opencl_ui );
		++h;

		p = get_param(h);
		if( aaa::c_cv::b_opengl_use_out )
		{
			p->clear_comment();
			p->set_unused( false );
		}
		else
		{
			p->set_comment( (aaa::c_cv::b_opengl_can_out) ? "Disabled" : "Can't" );
			p->set_unused( true );
		}
		p->set_pt( &_b_opengl_tex_ui );
		++h;
#if !AAA_OPENCV_GL_USE()
		p->set_comment( "Unimplemented"		);
		p->set_unused( true );
#endif
//		param_set_pt( h, _b_opengl_tex_ui				);

		param_set_pt( h, _b_mono_ui						);
		param_set_pt( h, _s_compo_ui					);
		param_set_pt( h, _mono_compo_factor_1_ui		);
		param_set_pt( h, _mono_compo_factor_2_ui		);
		param_set_pt( h, _mono_compo_offset_ui			);
		param_set_pt( h, _b_mono_equalize_histogram_ui	);
		param_set_pt( h, _b_float_ui					);
	//	param_set_pt( h, _b_pbo_use_ui					);
}

void	c_bdd_img::param_init_pt_src_b( INT32& h )
{
	param_set_pt( h, _src_b_img_index	);
	param_set_pt_2( h, _src_b_size		);
	param_set_pt( h, _b_swap_src_ui		);
	param_set_pt( h, _s_compo_b_ui		);
	param_set_pt( h, _src_2_resize_ui	);
}

void	c_bdd_img::param_init_pt_resize( INT32& h )
{
	_b_init_start_long = true;
	if( _b_crop )
	{
		o_str* o = get_param(h)->get_comment_always();
		o->set( _crop_size[0] );
		o->add_char( 'x' ); 
		o->add( _crop_size[1] );
	}
	else
		get_param(h)->clear_comment();
	++h;
		param_set_pt( h, _b_crop_ui			);
		param_set_pt( h, _crop_left_ui		);
		param_set_pt( h, _crop_right_ui		);
		param_set_pt( h, _crop_bottom_ui	);
		param_set_pt( h, _crop_top_ui		);
		param_set_pt_2( h, _crop_size		);

	if( _b_resize )
	{
		o_str* o = get_param(h)->get_comment_always();
		o->set( _resize_factor[0] );
		o->add_space(); 
		o->add( _resize_factor[1] );
		o->add( " -> " );
		o->add( _resize_size[0] );
		o->add_char( 'x' ); 
		o->add( _resize_size[1] );
	}
	else
		get_param(h)->clear_comment();
	++h;
		param_set_pt(	h, _b_resize_ui			);
		param_set_pt_2(	h, _resize_factor		);
		param_set_pt_2(	h, _resize_size			);
		param_set_pt(	h, _s_resize_inter_ui	);


	get_param(h)->set_comment_bool( _b_rotate_ui );
	++h;
		param_set_pt( h, _b_rotate_ui		);
		param_set_pt( h, _rotate_angle_ui	);
}

INT32	c_bdd_img::param_init_pt_start_src_resize( bool b_src_2, bool b_resize )
{
	INT32	h = param_init_pt_start();
	param_init_pt_src( h, b_src_2, b_resize );
	if( b_src_2 )
		param_init_pt_src_b( h );
	if( b_resize )
		param_init_pt_resize( h );
	return h;
}

//used at init when a subclass need to force the src pass to make one channel data   
void	c_bdd_img::force_mono()
{
	_b_mono_ui = true;		// we need mono image
	p_param param = get_param_by_name( "mono" );
	param->set_lock();
}

CONSTRUCTOR_ABSTRACT_CREATE( c_bdd_img )
	,_ipl_in				{nullptr}
	,_ipl_mono				{nullptr}
#if AAA_OPENCV_GL_USE()
	,_texture_in			{nullptr}
#endif
	,_b_init_start_long		{false}
	,_b_size_changed		{false}
	,_src_cur				{nullptr}
	,_b_src_is_gpu			{false}
	,_b_src_bgr				{false}
	,_b_src_b				{false}
	,_b_src_b_bgr			{false}
	,_b_swap_src_ui			{false}
	,_src_b_cur				{nullptr}
	,_src_b_size			{0,0}
	,_src_b_size_last		{0,0}
#if AAA_LIB_USE_OPENCV_CUDA()
	,_src_cur_gpu			{nullptr}
	,_src_b_cur_gpu			{nullptr}
#endif
	,_b_content_changed		{false}
	,_unique_id_last_b		{0}
	,_b_float				{false}
	,_b_mono				{false}
	,_p_in_to_use			{nullptr}
	,_src_pixel_format		{aaa::PIXEL_FORMAT::UNKNOWN}
	,_pixel_format_out		{aaa::PIXEL_FORMAT::UNKNOWN}
	,_src_size_ui			{0,0}
	,_src_size				{0,0}
	,_src_size_last			{0,0}
	,_prev					{nullptr}
	,_dst_size_out			{0,0}
	,_crop_size				{0,0}
	,_resize_size			{0,0}
{
}

void	c_bdd_img::release_img_src()
{
	IF_THIS_NULL_RETURN();

	_ipl_in.release();
	_ipl_mono.release();

	_mono.release();
	_mono_sub.release();
	_mono_b.release();
	
	_cropped.release();
	//_cropped_gpu.release();
	_resized.release();
	_float.release();

	_rotated.release();
	_flipped.release();
	_resized_b.release();
	_src_umat.release();
	_src_b_umat.release();

//		_rgb_split.clear();
//		_rgb_extract.clear();
//		_rgb_split_b.clear();

	_src_cur = nullptr;
	//_dst_cur = nullptr;
	_src_b_cur = nullptr;


#if AAA_LIB_USE_OPENCV_CUDA()
	_mono_gpu.release();
	_mono_b_gpu.release();
	_rotated_gpu.release();
	_flipped_gpu.release();
	_resized_gpu.release();
	_resized_b_gpu.release();
	_float_gpu.release();

	_src_gpumat.release();
	_src_b_gpumat.release();

	_rgb_split_gpu.clear();

	_rgb_split_b_gpu.clear();

//	_dst_cur_gpu = nullptr;
	_src_cur_gpu = nullptr;
	_src_b_cur_gpu	= nullptr;
#endif

}

c_bdd_img::~c_bdd_img()
{
	if( this == bdd_img_cur )
		bdd_img_cur = nullptr;	// avoid crash later...
	release_img_src();
}

void c_bdd_img::update_after_process()
{

}

c_img_2d*	c_bdd_img::update_part_low( INT32 src_img_index, UINT32& unique_id, INT32& src_size_x, INT32& src_size_x_last, INT32& src_size_y, INT32& src_size_y_last )
{
	_img_start = nullptr;

	_prev		= ( bdd_img_cur == this ) ? nullptr : bdd_img_cur;
	bdd_img_cur = this;

	if( is_active() )
	{
		c_img_2d*	img = g_bind_img_2d->get_ready( src_img_index );
		if( IS_NULL( img ) )
		{
			err_print( "%s() : no image source at bind %d", __FUNCTION__, src_img_index );
			goto error;
		}
		if( !img->check_data_valid( __FUNCTION__ ) )
		{
			err_print( "%s() : image source has no valid data at bind %d", __FUNCTION__, src_img_index );
			goto error;
		}

		_src_size_ui[0] = img->get_size_x();
		_src_size_ui[1] = img->get_size_y();

		_b_size_changed = false;
		if( !_b_always_update_ui && !g_img_master->is_bdd_img_force_compute() && img->is_state_unique(unique_id) )
		{
			// todo maybe need to check right image in case of stereo ?
			_b_content_changed = false;
			return nullptr;
		}
		unique_id = img->get_state_unique();
		_img_start = img;

		src_size_x = img->get_size_x();
		src_size_y = img->get_size_y();

		if( src_size_x_last != src_size_x || src_size_y_last != src_size_y  )
		{
			src_size_x_last = src_size_x;
			src_size_y_last = src_size_y;
			_b_size_changed = true;
		}

		_b_content_changed = true;
		inc_process_index();

		return img;
	}
error:
	src_size_x = 0;
	src_size_y = 0;
	return nullptr;
}

c_img_2d*	c_bdd_img::update_part_1()
{
	c_img_2d* img = update_part_low( _b_swap_src_ui ? _src_b_img_index : get_src_img_index_ui(), get_unique_id_last_ref(), _src_size[0], _src_size_last[0], _src_size[1], _src_size_last[1] );
	if( img )
		_src_pixel_format	= img->get_pixel_format();
	return img;
}

bool	c_bdd_img::update_img_src()
{
	_p_in_to_use = nullptr;
	c_img_2d*	img = update_part_1();
	if( !img )
		return false;
	_o_src_pixel_format.set( aaa::c_pixel_format::get_name( _src_pixel_format ) );

	if( _ipl_in.build_using( img, _b_size_changed ) )
	{
		_src_size[0] = _ipl_in.get_size_x();
		_src_size[1] = _ipl_in.get_size_y();

		if( _b_mono_ui )
		{
			if( img->get_data_type() == aaa::PIXEL_TYPE::FLOAT_32 )
			{
				_p_in_to_use = &_ipl_in;
			}
			else
			{
				//_ipl_mono.create_gray( _ipl_in, aaa::c_cv::get_opencv_depth_from_pixel_type( img->get_data_type(), __FUNCTION__ ), "Gray image" );
				_ipl_mono.create_gray( _ipl_in, IPL_DEPTH_8U, "Gray image" );
				_ipl_in.to_gray( _ipl_mono, _s_compo_ui );
				_p_in_to_use = &_ipl_mono;
			}
		}
		else
		{
			_p_in_to_use = &_ipl_in;
		}
		return true;
	}
	else
	{
		_src_size[0] = 0;
		_src_size[1] = 0;
	}
	return false;
}

c_img_ipl*	c_bdd_img::get_in_mono_to_use()
{
	if( get_in_to_use()->get_channel_nb() == 1 )
	{
		return get_in_to_use();
	}
	return nullptr;
}

INT32	c_bdd_img::build_dst_index( INT32 index )
{
	if( index == -1 )
		return _dst_img_index_ui;
	return index;
}



/*
bool	c_bdd_img::build_cv_mat( cv::UMat& mat, UINT32 img_index, cv::UMat& mat_ref, CHAR* mess, aaa::PIXEL_FORMAT type )
{
	return build_cv_mat( mat, img_index, mat_ref.cols, mat_ref.rows, mess, aaa::PIXEL_FORMAT::UNKNOWN == type ? c_pixel_format::get_pixel_type_from_channel_nb(mat_ref.channels()) : type );
}
*/
//bool	c_bdd_img::build_cv_mat( cv::UMat& mat, UINT32 img_index, cv::UMat& mat_ref, CHAR* mess, aaa::PIXELTYPE type )
//{
//	return build_cv_mat( mat, img_index, mat_ref.cols, mat_ref.rows, mess, aaa::PIXELTYPE::UNDEFINED == type ? c_pixel_format::get_pixel_type_from_channel_nb( mat_ref.channels() ) : type );
//}
/*
bool	c_bdd_img::build_cv_mat( cv::UMat& mat, UINT32 img_index, UINT32 sx, UINT32 sy, CHAR* mess, aaa::PIXEL_FORMAT type )
{
	c_img_2d*	img = g_bind_img_2d->get_img( img_index, sx, sy, type, true, nullptr, __FUNCTION__ );
	if ( IS_NULL( img ) )
	{
		ERR_PRINT_STRING("%s() : no image source at bind %d", __FUNCTION__, img_index );
		return false;
	}

	cv::Mat	tmp( sy, sx, get_cv_type( type ), (UINT8 *)img->get_data_uint8(), img->get_pitch() );
//	mat = tmp.getUMat( cv::ACCESS_RW );
	mat = tmp.getUMat( cv::ACCESS_RW );
	//if( _b_src_bgr )
	//	img->flip_bgr();
	//GOOD_PRINT_STRING( "cv::Mat image created : %s with %d channel", mess, mat_ref.channels() );
	return true;
}
*/
template< class MAT >
bool	c_bdd_img::is_mat_equal( MAT* mat_a, MAT * mat_b )
{
	if(		mat_a->size()		!=	mat_b->size()
		||	mat_a->depth()		!=	mat_b->depth()
		||	mat_a->channels()	!=	mat_b->channels()
		)
	{
		err_print( "%s() source a and b should have the same size, depth and channel nb", __FUNCTION__ );
		return false;
	}
	return true;
}

namespace
{
	int get_cv_conversion_to_gray( aaa::PIXEL_FORMAT pf )
	{
		int conversion;
		switch(pf)
		{
		case aaa::PIXEL_FORMAT::BGRA_32FP:
		case aaa::PIXEL_FORMAT::BGRA_16:
		case aaa::PIXEL_FORMAT::BGRA_8:		conversion = cv::COLOR_BGRA2GRAY;	break;
		case aaa::PIXEL_FORMAT::RGBA_32FP:
		case aaa::PIXEL_FORMAT::RGBA_16:
		case aaa::PIXEL_FORMAT::RGBA_8:		conversion = cv::COLOR_RGBA2GRAY;	break;
		case aaa::PIXEL_FORMAT::BGR_32FP:
		case aaa::PIXEL_FORMAT::BGR_16:
		case aaa::PIXEL_FORMAT::BGR_8:		conversion = cv::COLOR_BGR2GRAY;	break;
		case aaa::PIXEL_FORMAT::RGB_32FP:
		case aaa::PIXEL_FORMAT::RGB_16:
		case aaa::PIXEL_FORMAT::RGB_8:		conversion = cv::COLOR_RGB2GRAY;	break;
		//todo other cases
		default:							conversion = -42;					break;	//	cv mode are positive or 0
		}
		return conversion;
	}
}

#if AAA_LIB_USE_OPENCV_CUDA()
//todo deal with rg image
bool c_bdd_img::convert_to_mono_gpu( cv::cuda::GpuMat*& src, aaa::PIXEL_FORMAT pf, aaa::COMPO compo, cv::cuda::GpuMat* mono, std::vector<cv::cuda::GpuMat>* split )
{
	//todoq add missing COMPO mode
	bool b_ok = true;
	if( compo == aaa::COMPO::GREY )
	{
		//todo other cases
		int conversion = get_cv_conversion_to_gray( pf ); 
		if( conversion != -42 )
		{
			SPY_PUSH_RANGE( "cv::cuda::cvtColor", spy::IMG_LOW );
		//		mono->create( src->size(), src->type() );
				cv::cuda::cvtColor( *src, *mono, conversion );
			SPY_POP_RANGE();
			src = mono;
		}
		else
			b_ok = false;
	}
	else if( compo >= aaa::COMPO::RED_MINUS_GREEN )
	{
		INT32 a,b;
		if( aaa::c_pixel_format::make_compo_index( pf, compo, a, b ) )
		{
			SPY_PUSH_RANGE( "cv::cuda::split", spy::IMG_LOW );
				cv::cuda::split( *src, *split );
			SPY_POP_RANGE();
			SPY_PUSH_RANGE( "cv::cuda::addWeighted", spy::IMG_LOW );
				cv::cuda::addWeighted( (*split)[a], _mono_compo_factor_1_ui, (*split)[b], _mono_compo_factor_2_ui, _mono_compo_offset_ui*256., *mono );
			//cv::cuda::subtract( (*split)[a], (*split)[b], *mono );
			SPY_POP_RANGE();
			src = mono;
		}
		else
			b_ok = false;
	}
	else
	{
		INT32 a,b;
		if( aaa::c_pixel_format::make_compo_index( pf, compo, a, b ) )
		{
			SPY_PUSH_RANGE( "cv::cuda::split", spy::IMG_LOW );
				cv::cuda::split( *src, *split );
			SPY_POP_RANGE();
			src = &(*split)[a];
		}
		else
			b_ok = false;
	}
	return b_ok;
}
#endif

//todo deal with rg image
 
bool c_bdd_img::convert_to_mono( cv::UMat*& src, aaa::PIXEL_FORMAT pf, aaa::COMPO compo, cv::UMat* mono )
{
	//todoq add missing COMPO mode
	bool b_ok = true;
	if( compo == aaa::COMPO::GREY )
	{	// grey
		int conversion = get_cv_conversion_to_gray( pf ); 
		if( conversion != -42 )
		{
			SPY_PUSH_RANGE( "cv::cvtColor", spy::IMG_LOW );
				cv::cvtColor( *src, *mono, conversion );
			SPY_POP_RANGE();
			src = mono;
		}
		else
			b_ok = false;
	}
	else if( compo >= aaa::COMPO::RED_MINUS_GREEN )
	{	// all differences
		INT32 a,b;
		if( aaa::c_pixel_format::make_compo_index( pf, compo, a, b ) )
		{
			//todo will not work for the b thing (the other image)
			SPY_PUSH_RANGE( "cv::extractChannel", spy::IMG_LOW );
				cv::extractChannel( *src, _mono_sub,	b );
			SPY_POP_RANGE();
			SPY_PUSH_RANGE( "cv::extractChannel", spy::IMG_LOW );
				cv::extractChannel( *src, *mono,		a );
			SPY_POP_RANGE();
			SPY_PUSH_RANGE( "cv::addWeighted", spy::IMG_LOW );
				cv::addWeighted( *mono, _mono_compo_factor_1_ui, _mono_sub, _mono_compo_factor_2_ui, _mono_compo_offset_ui*256., *mono );
				//cv::subtract( *mono, _mono_sub, *mono );
			SPY_POP_RANGE();
			src = mono;
		}
		else
			b_ok = false;
	}
	else if( compo == aaa::COMPO::MAX_COMPO || compo == aaa::COMPO::MIN_COMPO )
	{	// min and max
		std::vector<cv::Mat> planes(3);
		SPY_PUSH_RANGE( "cv::split", spy::IMG_LOW );
			cv::split( *src, planes );
		SPY_POP_RANGE();
		SPY_PUSH_RANGE( "cv::min/max", spy::IMG_LOW );
			if( compo == aaa::COMPO::MAX_COMPO )
			{
				cv::Mat( max( planes[2], cv::max( planes[1], planes[0] ) ) ).copyTo( *mono );
	//			cv::Mat( cv::max( planes[2], cv::max( planes[1], planes[0] ) ) );
			}
			else
			{
				cv::Mat( min( planes[2], cv::min( planes[1], planes[0] ) ) ).copyTo( *mono );
			}
		SPY_POP_RANGE();
		src = mono;
	}
	else
	{
		INT32 a;
		if( pf == aaa::PIXEL_FORMAT::RG_16FP )
		{
			err_print( "in %s() cv::extractChannel will crash for RG_16FP (OpenCV 412): we skip ", __FUNCTION__ );
			b_ok = false;
		}
		else if( aaa::c_pixel_format::make_compo_index( pf, compo, a ) )
		{
			SPY_PUSH_RANGE( "cv::extractChannel", spy::IMG_LOW );
				try
				{	
					cv::extractChannel( *src, *mono, a );
					src = mono;
				}
				//catch( ... )
				//{
				//	err_print( "Exception in %s().", __FUNCTION__ );
				//	b_ok = false;
				//}
				//catch( std::exception & e )
				//{
				//	err_print( "Standard Exception in %s() failed with error %s.", __FUNCTION__, e.what() );
				//	b_ok = false;
				//}
				catch( cv::Exception& e )
				{
					err_print( "Open CV Exception in %s(): %s", __FUNCTION__, e.what() );
					b_ok = false;
				}
			SPY_POP_RANGE();
		
		}
		else
			b_ok = false;
	}
	return b_ok;
}

bool	c_bdd_img::update_mat_src_private( bool& b_change_it )
{
	b_change_it = false;
	SPY_PUSH_RANGE_FUNCTION( spy::IMG_LOW );

	INT32 sx, sy;
	_b_float = false;
	cv::ocl::setUseOpenCL( _b_opencl_ui && aaa::c_cv::b_opencl_use_out );

	if( _b_src_img_prev_ui && _prev )
	{
#if AAA_LIB_USE_OPENCV_CUDA()
		if( _prev->is_gpu_use() )
		{
			if( is_gpu_use() )
				_src_cur_gpu = _prev->_src_cur_gpu;
			else
				_prev->_src_cur_gpu->copyTo( *_src_cur );
		}
		else
		{
			if ( is_gpu_use() )
				_src_cur_gpu->upload( *_prev->_src_cur );
			else
				_src_cur = _prev->_src_cur;
		}
#else
		_src_cur = _prev->_src_cur;
#endif
		_b_src_bgr	= _prev->is_src_bgr();
		INT32 type; 
#if AAA_LIB_USE_OPENCV_CUDA()
		if( is_gpu_use() )
		{
			sx		= _src_cur_gpu->cols;
			sy		= _src_cur_gpu->rows;
			type	= _src_cur_gpu->type();
		}
		else
#endif
		{
			sx		= _src_cur->cols;
			sy		= _src_cur->rows;
			type	= _src_cur->type();
		}
		_src_pixel_format	= aaa::c_cv::get_pixel_format_from_cv( type, _b_src_bgr );
		_b_float			= aaa::c_pixel_format::is_format_fp32( _src_pixel_format );
		//if( _src_pixel_format == aaa::PIXEL_FORMAT::RGBA_FP32 )
		//	_b_float = true;
	}
	else
	{
		bool b_ocl = cv::ocl::useOpenCL();
#if AAA_OPENCV_GL_USE()
		if( _b_opengl_tex_ui && aaa::c_cv::b_opengl_use_out
#if 1
			&& b_ocl
#endif
			)

		{
		//	CL_INVALID_CONTEXT
			INT32 index =  _b_swap_src_ui ? _src_b_img_index : get_src_img_index_ui();	//was Mars 2017 _src_img_index_ui );
			tex_2d_bind_no_gpu_move( index );

			tex2d.get_pixel_size( index, sx, sy );
//			auto info = tex2d.get_info( index );

//			_texture_in = new cv::ogl::Texture2D( info->get_size_y(), info->get_size_x(), cv::ogl::Texture2D::Format::RGBA, info->get_name(), false );
			//todo alloc/free at some point
//			_texture_in = new cv::ogl::Texture2D( sx, sy, cv::ogl::Texture2D::Format(GL_RED), tex2d.get_name_gl(index), false );
			_texture_in = new cv::ogl::Texture2D( sx, sy, cv::ogl::Texture2D::Format::RGBA, tex2d.get_name_gl(index), false );
			cv::ogl::convertFromGLTexture2D( *_texture_in, _src_umat );
		}
		else
#endif
		{
			c_img_2d*	img = update_part_1();
			if( !img )
				goto exit_on_error;

			sx = img->get_size_x();
			sy = img->get_size_y();

			_b_src_bgr = aaa::c_pixel_format::is_bgr( _src_pixel_format );

			cv::Mat	tmp( sy, sx, aaa::c_cv::get_cv_type( img->get_pixel_format() ), img->get_data(), img->get_byte_pitch() );

			if( b_ocl )
				tmp.copyTo( _src_umat );
			else
				_src_umat = tmp.getUMat( cv::ACCESS_RW );
		}
		_src_cur = &_src_umat;
	}


	_b_src_is_gpu = is_gpu_use();


	if( _b_init_start_long )
	{
		_b_crop = _b_crop_ui && (_crop_left_ui != 0. || _crop_right_ui != 0. || _crop_bottom_ui != 0. || _crop_top_ui != 0.);
		if( _b_crop )
		{
			if( _src_cur )
			{
				b_change_it = true;
				INT32 xl	=	MIN( I_FLOOR( _crop_left_ui*sx ), sx-2 );
				INT32 sxn	=	MAX( 2, sx - xl - I_FLOOR( _crop_right_ui*sx ) );
				INT32 yb	=	MIN( I_FLOOR( _crop_bottom_ui*sy ), sy-2 );
				INT32 syn	=	MAX( 2, sy - yb - I_FLOOR( _crop_top_ui*sy ) );

				cv::Rect my_roi( xl, yb, sxn, syn );

				//gpu case crashed later so we do it in cpu for now
				cv::UMat cropped_ref( *_src_cur, my_roi );
				//	Copy the data into new matrix
				//_cropped.create( syn, sxn, cropped_ref.type() );
				SPY_PUSH_RANGE( "crop", spy::IMG_LOW );
					cropped_ref.copyTo( _cropped );
				SPY_POP_RANGE();
				_src_cur = &_cropped;

				sx	=	sxn;
				sy	=	syn;
			}
			else
			{
				err_print( "%s() _src_cur is null can't crop", __FUNCTION__ );
				goto exit_on_error;
			}
		}	// end if( _b_crop )
		_crop_size[0] = sx;
		_crop_size[1] = sy;
	}

#if AAA_LIB_USE_OPENCV_CUDA()
	if( _b_src_is_gpu && !(_b_src_img_prev_ui && _prev) )
	{
		if( _src_cur )
		{
			//_dst_cur_gpu.upload( _src_cur );
			//_src_cur_gpu = _dst_cur_gpu;
			_src_gpumat.upload( *_src_cur );
			_src_cur_gpu = &_src_gpumat;
			//_dst_cur_gpu = _src_cur_gpu;
		}
		else
		{
			err_print( "%s() _src_cur is null can't build a _src_cur_gpu", __FUNCTION__ );
			goto exit_on_error;
		}
	}
#endif

	//todo deal with all the mono : we do it in this test but what happen after
	_b_mono = aaa::c_pixel_format::get_channel_nb( _src_pixel_format ) == 1;
	if( _b_mono_ui && !_b_mono )
	{
		bool b_ok;
		//todoq add missing COMPO mode
#if AAA_LIB_USE_OPENCV_CUDA()
		if( _b_src_is_gpu )
			b_ok	= convert_to_mono_gpu(	_src_cur_gpu,	_src_pixel_format,	_s_compo_ui, &_mono_gpu,	&_rgb_split_gpu );
		else
#endif
			b_ok	= convert_to_mono(		_src_cur,		_src_pixel_format,	_s_compo_ui, &_mono		);
		if( !b_ok )
			goto exit_on_error;

		b_change_it = true;
		_b_mono = true;
	}

	if( _b_mono && !_b_float && _b_mono_equalize_histogram_ui )
	{
#if AAA_LIB_USE_OPENCV_CUDA()
		if( _b_src_is_gpu )
		{
			SPY_PUSH_RANGE( "cv::cuda::equalizeHist", spy::IMG_LOW );
				cv::cuda::equalizeHist( *_src_cur_gpu, *_src_cur_gpu );
			SPY_POP_RANGE();
		}
		else
#endif
		{
			SPY_PUSH_RANGE( "cv::equalizeHist", spy::IMG_LOW );
				cv::equalizeHist( *_src_cur, *_src_cur );
			SPY_POP_RANGE();
		}
		b_change_it = true;
	}


	if( _b_init_start_long )
	{
		if( _b_rotate_ui && ( _rotate_angle_ui != CV_ROT_NONE ) )
		{
			if( _rotate_angle_ui == CV_ROT_90 ||  _rotate_angle_ui == CV_ROT_270 )
			{
				UINT32	flip_code = _rotate_angle_ui == CV_ROT_90 ? 0 : 1;
				SPY_PUSH_RANGE( "cv::transpose/flip", spy::IMG_LOW );
#if AAA_LIB_USE_OPENCV_CUDA()
				if( _b_src_is_gpu )
				{
					if( _b_mono || aaa::c_pixel_format::get_channel_nb(_src_pixel_format) == 4 )
					{
						cv::cuda::transpose( *_src_cur_gpu, _rotated_gpu );
					}
					else
					{
						cv::cuda::split( *_src_cur_gpu, _rgb_split_gpu );
						// cv::cuda::transpose only works on 1, 4 or 8 element size, so we split it
						switch( _rgb_split_gpu.size() )
						{
						case 4:	cv::cuda::transpose( _rgb_split_gpu[3], _rgb_split_gpu[3] );
						case 3:	cv::cuda::transpose( _rgb_split_gpu[2], _rgb_split_gpu[2] );
						case 2:	cv::cuda::transpose( _rgb_split_gpu[1], _rgb_split_gpu[1] );
						case 1:	cv::cuda::transpose( _rgb_split_gpu[0], _rgb_split_gpu[0] );
						}
						cv::cuda::merge(	_rgb_split_gpu,		_rotated_gpu		);
					}
					cv::cuda::flip(		_rotated_gpu, _flipped_gpu, flip_code	);
					_src_cur_gpu = &_flipped_gpu;
				}
				else
#endif
				{
					cv::transpose( *_src_cur, _rotated );
					cv::flip(		_rotated, _flipped, flip_code );
					_src_cur = &_flipped;
				}
				SWAP( sx, sy );
				b_change_it = true;
				SPY_POP_RANGE();
			}
			else if( _rotate_angle_ui == CV_ROT_180 )
			{
				SPY_PUSH_RANGE( "cv::flip", spy::IMG_LOW );
#if AAA_LIB_USE_OPENCV_CUDA()
				if( _b_src_is_gpu )
				{
					cv::cuda::flip(	*_src_cur_gpu,	_rotated_gpu,	-1 );
					_src_cur_gpu = &_rotated_gpu;
				}
				else
#endif
				{
					cv::flip(		*_src_cur,		_rotated,		-1 );
					_src_cur = &_rotated;
				}
				b_change_it = true;
				SPY_POP_RANGE();
			}
		}

		if( _b_float_ui )
		{
			SPY_PUSH_RANGE( "cv::convertTo 32fp", spy::IMG_LOW );
#if AAA_LIB_USE_OPENCV_CUDA()
			if( _b_src_is_gpu )
			{
				_src_cur_gpu->convertTo( _float_gpu, CV_32F, (1. / 255.) );
				_src_cur_gpu = &_float_gpu;
			}
			else
#endif
			{
				_src_cur->convertTo( _float, CV_32F, (1. / 255.) );
				_src_cur = &_float;
			}
			_b_float = _b_float_ui;
			b_change_it = true;
			SPY_POP_RANGE();
		}

		_b_resize = _b_resize_ui && (_resize_factor[0] != 1. || _resize_factor[1] != 1.);
		if( _b_resize )
		{

#if AAA_LIB_USE_OPENCV_CUDA()
			if( _b_src_is_gpu )
			{
				auto depth = (*_src_cur_gpu).depth();
				if( depth>CV_32F || depth==CV_32S )

				{
					err_print( "OpenCV : can't resize this depth %d", depth );
					goto exit_on_error;
				}
				auto ch_nb = (*_src_cur_gpu).channels();
				if( ch_nb!=1 && ch_nb!=3 && ch_nb!=4 )
				{
					err_print( "OpenCV : can't resize with channel nb of %d", ch_nb );
					goto exit_on_error;
				}
			}
#endif
			{
				sx = MAX( 2, I_FLOOR( sx * _resize_factor[0] ) );
				sy = MAX( 2, I_FLOOR( sy * _resize_factor[1] ) );
				//	resize
				auto size = cv::Size(sx,sy);
#if AAA_LIB_USE_OPENCV_CUDA()
				if( _b_src_is_gpu )
				{
					SPY_PUSH_RANGE( "cv::cuda::resize", spy::IMG_LOW );
						cv::cuda::resize( *_src_cur_gpu, _resized_gpu, size, 0, 0, _s_resize_inter_ui );
					SPY_POP_RANGE();
					_src_cur_gpu = &_resized_gpu;
				}
				else
#endif
				{
					//	_resized.create( sy, sx, get_cv_type( img->get_pixel_type() ) );
					SPY_PUSH_RANGE( "cv::resize", spy::IMG_LOW );
						cv::resize( *_src_cur, _resized, size, 0, 0, _s_resize_inter_ui );
					SPY_POP_RANGE();
					_src_cur = &_resized;
				}
			}
			b_change_it = true;
		}
		_resize_size[0] = sx;
		_resize_size[1] = sy;
	}

	_src_size[0] = sx;
	_src_size[1] = sy;
	_o_src_pixel_format.set( aaa::c_pixel_format::get_name( _src_pixel_format ) );

	if( _src_size_last[0] != sx || _src_size_last[1] != sy )
	{
		_b_size_changed = true;
	}
	_src_size_last[0] = sx;
	_src_size_last[1] = sy;

	SPY_POP_RANGE();
	return true;

exit_on_error:
	SPY_POP_RANGE();
	return false;
}

bool	c_bdd_img::update_mat_src( bool& b_change_it )
{
	bool retcode;
	try
	{	
		retcode = update_mat_src_private( b_change_it );
	}
	catch( cv::Exception& e )
	{
		ERR_PRINT_STRING( "Open CV Exception in %s(): %s", __FUNCTION__, e.what() );
		retcode = false;
	}
	return retcode;
}

bool	c_bdd_img::update_mat_src_2_private( bool b_size_equal )
{
	bool b_change_it;
	if( !update_mat_src_private( b_change_it ) )
		return false;

	c_img_2d*	img = update_part_low( _b_swap_src_ui ? get_src_img_index_ui() : _src_b_img_index, _unique_id_last_b, _src_b_size[0], _src_b_size_last[0], _src_b_size[1], _src_b_size_last[1] );
	if( !img )
		return false;

	//	INT32	sx, sy;
	//	sx = img->get_size_x();
	//	sy = img->get_size_y();
	aaa::PIXEL_FORMAT pf = img->get_pixel_format();
	_b_src_b_bgr = aaa::c_pixel_format::is_bgr( pf );
	cv::Mat	tmp( _src_b_size[1], _src_b_size[0], aaa::c_cv::get_cv_type( pf ), (UINT8 *)img->get_data_uint8(), img->get_byte_pitch() );
#if AAA_LIB_USE_OPENCV_CUDA()
	if( is_gpu_use() )
	{
		_src_b_gpumat.upload( tmp );
		_src_b_cur_gpu = &_src_b_gpumat;
	}
	else
#endif
	{
		_src_b_umat	= tmp.getUMat( cv::ACCESS_RW );
		_src_b_cur	= &_src_b_umat;
	}

	//_b_mono = ( img->get_channel_nb() == 1 );
	if( _b_mono_ui && img->get_channel_nb() != 1 )
	{
		bool b_ok;
#if AAA_LIB_USE_OPENCV_CUDA()
		if( is_gpu_use() )
			b_ok	= convert_to_mono_gpu(	_src_b_cur_gpu,	pf,	_s_compo_b_ui, &_mono_b_gpu,	&_rgb_split_b_gpu );
		else
#endif
			b_ok	= convert_to_mono(		_src_b_cur,		pf,	_s_compo_b_ui, &_mono_b		);
		if( !b_ok )
			return false;
		_b_mono = true;
	}

	if( _src_2_resize_ui > RESIZE_SRC_NONE )
	{
		if( _src_cur->size() != _src_b_cur->size() )
		{
			if( _src_2_resize_ui == RESIZE_SRC_A )
			{
				//	resize
#if AAA_LIB_USE_OPENCV_CUDA()
				if( is_gpu_use() )
				{
					SPY_PUSH_RANGE( "cv::cuda::resize", spy::IMG_LOW );
						cv::cuda::resize( *_src_b_cur_gpu, _resized_b_gpu, _src_cur->size() );
					SPY_POP_RANGE();				
					_src_b_cur_gpu = &_resized_b_gpu;
				}
				else
#endif
				{
					SPY_PUSH_RANGE( "cv::resize", spy::IMG_LOW );
						//_resized.create( sy, sx, get_cv_type( img->get_channel_nb() ) );
						cv::resize( *_src_b_cur, _resized_b, _src_cur->size() );
					SPY_POP_RANGE();				
					_src_b_cur = &_resized_b;
				}
				cpy_v2( _src_b_size, _src_size );
			}
			else if( _src_2_resize_ui == RESIZE_SRC_B )
			{
				//	resize
#if AAA_LIB_USE_OPENCV_CUDA()
				if( is_gpu_use() )
				{
					SPY_PUSH_RANGE( "cv::cuda::resize", spy::IMG_LOW );
						cv::cuda::resize( *_src_b_cur_gpu, _resized_b_gpu, _src_b_cur->size() );
					SPY_POP_RANGE();				
					_src_b_cur_gpu = &_resized_b_gpu;
				}
				else
#endif
				{
					SPY_PUSH_RANGE( "cv:resize", spy::IMG_LOW );
						//_resized.create( sy, sx, get_cv_type( img->get_channel_nb() ) );
						cv::resize( *_src_b_cur, _resized_b, _src_b_cur->size() );
					SPY_POP_RANGE();					
					_src_b_cur = &_resized_b;
				}
				cpy_v2( _src_size, _src_b_size );
			}
		}
	}

//	_src_b_size_x = sx;
//	_src_b_size_y = sy;

	//if( _src_b_size_x != _src_b_size_x_last || _src_b_size_y != _src_b_size_y_last )
	//{
	//	_b_size_changed = true;
	//}
	//_src_b_size_x_last = _src_b_size_x;
	//_src_b_size_y_last = _src_b_size_y;
	if( ( _src_2_resize_ui == RESIZE_SRC_NONE ) && b_size_equal )
	{
#if AAA_LIB_USE_OPENCV_CUDA()
		if( is_gpu_use() )	
		{
			if( !is_mat_equal( _src_cur_gpu, _src_b_cur_gpu ) )
				return false;
		}
		else
#endif
		{
			if( !is_mat_equal( _src_cur, _src_b_cur ) )
				return false;
		}
	}
	return true;
}

bool	c_bdd_img::update_mat_src_2( bool b_size_equal )
{
	bool retcode;
	try
	{	
		retcode = update_mat_src_2_private( b_size_equal );
	}
	catch( cv::Exception& e )
	{
		ERR_PRINT_STRING( "Open CV Exception in %s(): %s", __FUNCTION__, e.what() );
		retcode = false;
	}
	return retcode;
}

void		c_bdd_img::validate_dst_change( c_img_2d* img )
{
	if( img )
	{
		img->set_changed();			
		if( img == _img_start )
			set_unique_id_last( img->get_state_unique() );
		_dst_size_out[0] = img->get_size_x();
		_dst_size_out[1] = img->get_size_y();
		_pixel_format_out	= img->get_pixel_format();
		_o_pixel_format_out.set( aaa::c_pixel_format::get_name( _pixel_format_out ) );
	}
}

void		c_bdd_img::validate_dst_change( INT32 index )
{
	index = build_dst_index( index );
	validate_dst_change( g_bind_img_2d->get_ready( index ) );
}

bool		c_bdd_img::build_dst_image( c_img_ipl& dst, INT32 index, c_img_ipl& ref, C_PCHAR_C name, INT32 channel_nb )
{
	index = build_dst_index( index );
	bool ret = dst.build( index, ref, name, channel_nb );
	return ret;
}

void	c_bdd_img::update_img_dst( c_img_2d* img, INT32 index )
{
	if( index >= 0 )
	{
		tex_2d_bind_no_gpu_move( index );
		img->move_to_gpu( __FUNCTION__, index );
	}
	validate_dst_change( img );
}

void	c_bdd_img::update_dst_index( cv::Mat& mat, INT32 index, C_PCHAR_C mess, bool CONST b_bgr, bool CONST b_move_to_gpu, CONST c_pbo* pbo )
{
	//todo
	//index = build_dst_index( index );
	//if ( index >= 0 && _b_keep_data_ui )
	{
		aaa::PIXEL_FORMAT	format		=  aaa::c_cv::get_pixel_format_from_cv( mat.type(), b_bgr );
		if( format != aaa::PIXEL_FORMAT::UNKNOWN )
		{
			c_img_2d*	img = g_bind_img_2d->get_always( index );
			if( IS_NULL( img ) )
			{
				ERR_PRINT_STRING( "%s() : no image source at bind %d", __FUNCTION__, index );
				return;
			}
			img->copy_from_src( mat.data, (INT32)mat.step, format, format, mat.cols, mat.rows, false );
			update_img_dst( img, b_move_to_gpu ? index : -42 );
		}
		else
		{
			ERR_PRINT_STRING( "%s() unknown OpenCV format", __FUNCTION__ );
		}
	}
}

void	c_bdd_img::update_dst_index( cv::UMat& umat, INT32 index, C_PCHAR_C mess, bool CONST b_bgr, bool CONST b_move_to_gpu, CONST c_pbo* pbo )
{
	index = build_dst_index( index );
	if( index >= 0 && _b_keep_data_ui )
	{
		// if opencl we need to download the data first to cpu
		if( cv::ocl::useOpenCL() )
		{
			cv::Mat	tmp = umat.getMat( cv::ACCESS_READ );
			update_dst_index( tmp, index, mess, b_bgr, b_move_to_gpu, pbo );
		}
		else
		{
			// if not opencl we can access directly the data
			aaa::PIXEL_FORMAT	format = aaa::c_cv::get_pixel_format_from_cv( umat.type(), b_bgr );
			if( format != aaa::PIXEL_FORMAT::UNKNOWN )
			{
				c_img_2d*	img = g_bind_img_2d->get_always( index );
				if( IS_NULL( img ) )
				{
					ERR_PRINT_STRING( "%s() : no image source at bind %d", __FUNCTION__, index );
					return;
				}

				if( umat.u )
				{
					img->copy_from_src( umat.u->data, (INT32)umat.step, format, format, umat.cols, umat.rows, false );
					update_img_dst( img, b_move_to_gpu ? index : -42 );
				}
				else
				{
					ERR_PRINT_STRING( "%s() umat.u is null this should not happen", __FUNCTION__ );
				}
			}
			else
			{
				ERR_PRINT_STRING( "%s() unknown OpenCV format", __FUNCTION__ );
			}
		}
	}
}

#if AAA_LIB_USE_OPENCV_CUDA()
void	c_bdd_img::update_dst_index( cv::cuda::GpuMat& gpu_mat, INT32 index, C_PCHAR_C mess, bool CONST b_bgr, bool CONST b_move_to_gpu, CONST c_pbo* pbo )
{
	index = build_dst_index( index );
	if( index >= 0 && _b_keep_data_ui && !gpu_mat.empty() )
	{
		aaa::PIXEL_FORMAT	type		=  aaa::c_cv::get_pixel_format_from_cv( gpu_mat.type(), b_bgr );
		if( type != aaa::PIXEL_FORMAT::UNKNOWN )
		{
			c_img_2d*	img = g_bind_img_2d->get_img( index, type, gpu_mat.cols, gpu_mat.rows, true, nullptr, __FUNCTION__ );
			if( IS_NULL( img ) )
			{
				ERR_PRINT_STRING( "%s() : no image source at bind %d", __FUNCTION__, index );
				return;
			}
			cv::Mat tmp( gpu_mat.rows, gpu_mat.cols, gpu_mat.type(), img->get_data(), img->get_byte_pitch() );
			gpu_mat.download( tmp );

			update_img_dst( img, b_move_to_gpu ? index : -42 );
		}
		else
		{
			ERR_PRINT_STRING( "%s() unknown OpenCV format", __FUNCTION__ );
		}
	}
}
#endif

void	c_bdd_img::update_dst_index( INT32 index_dst, C_PCHAR_C mess, bool CONST b_src_bgr, bool CONST b_move_to_gpu, c_pbo CONST * pbo )
{
	index_dst = build_dst_index( index_dst );
	if( index_dst >= 0 && _b_keep_data_ui )
	{
#if AAA_LIB_USE_OPENCV_CUDA()
		if( _b_src_is_gpu )
			update_dst_index(	*_src_cur_gpu,	index_dst, mess, _b_src_bgr, b_move_to_gpu, pbo );
		else
#endif
			update_dst_index(	*_src_cur,		index_dst, mess, _b_src_bgr, b_move_to_gpu, pbo );
	}
}

void	c_bdd_img::update_dst_index( INT32 index_dst, C_PCHAR_C mess, bool CONST b_move_to_gpu, c_pbo CONST * pbo )
{
	update_dst_index( index_dst, mess, _b_src_bgr, b_move_to_gpu, pbo );
}