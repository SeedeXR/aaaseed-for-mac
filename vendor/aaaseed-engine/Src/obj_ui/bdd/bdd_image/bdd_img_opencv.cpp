#include "bdd_img_opencv.h"
#include "image/bind_img.h"
#include "time/speed.h"
#include "opencv2/photo/photo.hpp"
#include "opencv2/video.hpp"
#include "image/img_compo.h"
#include "spy.h"

#if AAA_LIB_USE_OPENCV_CUDA()
#	include "opencv2/cudaarithm.hpp"
#	include "opencv2/cudafilters.hpp"
#	include "opencv2/cudaimgproc.hpp"
#	include "opencv2/photo/cuda.hpp"
#	include "opencv2/cudaoptflow.hpp"
#endif


//todo add adaptiveThreshold() ?

namespace{
	o_str sum_up;
};

c_opencv_pass::c_opencv_pass()
	:_status	{	aaa::c_cv::STATUS::ASKED	}
	,_speed		{nullptr}
	,_speed_ui	{nullptr}
	,_owner		{nullptr}
{
}

void c_opencv_pass::init( c_bdd_img* owner, C_PCHAR_C name )
{
	_owner = owner;
	_speed = new c_speed( true,	5, g_speed_master->get_pb_print_bdd(), name, owner );
	owner->obj_get( _speed_ui )->set_timing( _speed );
	_name.set( name );
}

c_opencv_pass::~c_opencv_pass()
{
	_dst.release();
#if AAA_LIB_USE_OPENCV_CUDA()
	_dst_gpu.release();
#endif
	SAFE_DELETE( _speed );
	SAFE_DELETE( _speed_ui );
}

void c_opencv_pass::begin_param_init( INT32& h )
{
	_speed->build_comment( sum_up, _b_active_ui );
	bool b_error = aaa::c_cv::build_comment_status( &sum_up, _b_active_ui, _status );
	auto param = _owner->get_param(h);
	param->set_comment( sum_up, b_error );
	++h;
		_owner->param_set_pt( h, _b_active_ui		);
		_owner->param_set_pt( h, _dst_index_ui		);
}

void c_opencv_pass::end_param_init( INT32& h )
{
	_speed_ui->update();
	_owner->param_attach_obj( h, _speed_ui  );
}

void c_opencv_pass::begin_process()
{
	SPY_PUSH_RANGE( _name, spy::IMG_LOW );
	_speed->begin();
}
void c_opencv_pass::end_process()
{
	if( _status == aaa::c_cv::STATUS::STARTED )
		_status = aaa::c_cv::STATUS::DONE;
#if AAA_LIB_USE_OPENCV_CUDA()
	if( _owner->is_gpu_use() )
		_owner->_src_cur_gpu = &_dst_gpu;
	else
#endif
		_owner->_src_cur = &_dst;

	_owner->update_dst_index( _dst_index_ui, _name.get() );
	_speed->end();
	SPY_POP_RANGE();
}

namespace img_opencv
{
	enum SMOOTH_METHOD
	{
		BLUR_BOX = 0,
		BLUR_BOX_NORMALIZED,
		BLUR_GAUSSIAN,
		BLUR_MEDIAN,
		BLUR_METHOD_NB
	};
	C_PCHAR_C	str_smooth_method[BLUR_METHOD_NB] =
	{
		"BOX",
		"BOX_NORMALIZED",
		"GAUSSIAN",
		"MEDIAN",
	};
	//INT32	cv_smooth_method[BLUR_METHOD_NB] =
	//{
	////	CV_BLUR_NO_SCALE,
	//	CV_BLUR,
	//	CV_GAUSSIAN,
	//	CV_MEDIAN,
	//};

	INT32 CONST	DIST_TYPE_NB = 8;
	C_PCHAR_C	dist_transform_type_str[DIST_TYPE_NB] =
	{
		"L1",
		"L2",
		"C",
		"L12",
		"FAIR",
		"WELCH",
		"HUBER",
		"USER"
	};

	INT32 CONST cv_dist_type[ DIST_TYPE_NB ] =
	{
		//	CV_BLUR_NO_SCALE,
		cv::DIST_L1,
		cv::DIST_L2,
		cv::DIST_C,
		cv::DIST_L12,
		cv::DIST_FAIR,
		cv::DIST_WELSCH,
		cv::DIST_HUBER,
		cv::DIST_USER
	};

	INT32 CONST	DIST_SIZE_NB = 3;
	C_PCHAR_C	dist_transform_size_str[DIST_SIZE_NB] =
	{
		"3",
		"5",
		"Precise"
	};
	INT32 CONST	cv_dist_size[ DIST_SIZE_NB ] =
	{
		//	CV_BLUR_NO_SCALE,
		3,
		5,
		cv::DIST_MASK_PRECISE
	};

	INT32 CONST EDGE_SIZE_NB = 17;
#define SCHARR -1
	C_PCHAR_C	edge_size_str[EDGE_SIZE_NB] =
	{
		"Scharr",
		"1",	"3",	"5",	"7",	"9",
		"11",	"13",	"15",	"17",	"19",
		"21",	"23",	"25",	"27",	"29",
		"31"
	};
	INT32 CONST	cv_edge_size[ EDGE_SIZE_NB ] =
	{
		SCHARR,
		1,		3,		5,		7,		9,
		11,		13,		15,		17,		19,
		21,		23,		25,		27,		29,
		31
	};

	INT32 CONST	CANNY_SIZE_NB = 3;
	C_PCHAR_C	canny_size_str[CANNY_SIZE_NB] =
	{
		"3",
		"5",
		"7",
	};
	INT32 CONST	cv_canny_size[ CANNY_SIZE_NB ] =
	{
		//	CV_BLUR_NO_SCALE,
		3,
		5,
		7
	};

	INT32 CONST	MORPH_TYPE_NB = 3;
	C_PCHAR_C	cv_morph_type_str[ MORPH_TYPE_NB ] =
	{
		"Rect",
		"Cross",
		"Ellipse"
	};
	INT32 CONST	cv_morph_type[ MORPH_TYPE_NB ] =
	{
		//	CV_BLUR_NO_SCALE,
		cv::MORPH_RECT,
		cv::MORPH_CROSS,
		cv::MORPH_ELLIPSE
	};

	INT32 CONST	CONVERT_TYPE_NB = 10;
	static	C_PCHAR_C	cv_convert_type_str[CONVERT_TYPE_NB] =
	{
		"HSL",
		"HSV",
		"YCrCb JPEG",
		"CIE XYZ.Rec 709",
		"CIE L*a*b*",
		"CIE L*u*v*",
		"Bayer BG",
		"Bayer GB",
		"Bayer RG",
		"Bayer GR"
	};
	INT32 CONST	cv_convert_type[ CONVERT_TYPE_NB ] =
	{
		cv::COLOR_RGB2HSV,
		cv::COLOR_RGB2HLS,
		cv::COLOR_RGB2YCrCb,
		cv::COLOR_RGB2XYZ,
		cv::COLOR_RGB2Lab,
		cv::COLOR_RGB2Luv,
		cv::COLOR_BayerBG2RGB,
		cv::COLOR_BayerGB2RGB,
		cv::COLOR_BayerRG2RGB,
		cv::COLOR_BayerGR2RGB
	};
	
	INT32 CONST POLY_N_METHOD_NB = 2;
	static	C_PCHAR_C	str_poly_n[POLY_N_METHOD_NB] =
	{
		"5",
		"7",
	};
	INT32 CONST	cv_poly_n[POLY_N_METHOD_NB] =
	{
		5,
		7,
	};
	C_PCHAR_C		dist_type_asked_str[4]	= {		GOL::int8_str,	GOL::int16_str,	GOL::fp16_str,	GOL::fp32_str};	//same names than
}


FACTORY_CREATE_PROP_V1( c_bdd_img_opencv, bdd_img_opencv, Image OpenCV, bdd_img_opencv, sub_menu="Image"; );


namespace n_bdd_img_opencv
{
	CONSTEXPR UINT32 BASE_PARAM_NB			= c_bdd_img::SUPER_RESIZE_PARAM_NB + 2;
	CONSTEXPR UINT32 EDGE_PARAM_NB			= 9;
	CONSTEXPR UINT32 CANNY_PARAM_NB			= 7;
	CONSTEXPR UINT32 CONVERT_PARAM_NB		= 4;
	CONSTEXPR UINT32 DISTRANSFORM_PARAM_NB	= 8;
	CONSTEXPR UINT32 INVERT_PARAM_NB		= 3;
	CONSTEXPR UINT32 BLUR_PARAM_NB			= 6;
	CONSTEXPR UINT32 BILATERAL_PARAM_NB		= 6;
	CONSTEXPR UINT32 HIPARAM_DEF_NB			= 3;
	CONSTEXPR UINT32 DENOISE_PARAM_NB		= 6;
	CONSTEXPR UINT32 BC_PARAM_NB			= 5;
	CONSTEXPR UINT32 DILATE_PARAM_NB		= 7;
	CONSTEXPR UINT32 ERODE_PARAM_NB			= 7;
	CONSTEXPR INT32  OPTFLOW_PARAM_NB		= 12;
	CONSTEXPR INT32  INPAINTING_PARAM_NB	= 7;
	CONSTEXPR UINT32 GROUP_PARAM_NB			= 15;

	CONSTEXPR INT32	PARAM_NB	=	BASE_PARAM_NB
								+	CONVERT_PARAM_NB
								+	BLUR_PARAM_NB
								+	BC_PARAM_NB	
								+	HIPARAM_DEF_NB
								+	DENOISE_PARAM_NB
								+	BILATERAL_PARAM_NB
								+	EDGE_PARAM_NB
								+	CANNY_PARAM_NB
								+	DILATE_PARAM_NB
								+	ERODE_PARAM_NB
								+	INVERT_PARAM_NB
								+	BC_PARAM_NB
								+	OPTFLOW_PARAM_NB
								+	INPAINTING_PARAM_NB
								+	DISTRANSFORM_PARAM_NB
								+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB] =
	{
		BDD_IMG_BASE_RESIZE_PARAMS

		PARAM_DEF_REAL_LOCKED(	process_time		)
//		PARAM_DEF_BOOL_OFF(			stereo_enable	)
//		PARAM_DEF_BOOL_OFF(			swap_source		)

//todo add or put in BASE_RESIZE
//			PARAM_DEF_GROUP_CLOSED( Invert, INVERT_PARAM_NB )
//			PARAM_DEF_BOOL_OFF(		invert				)
//			PARAM_DEF_IMG_DST(		invert_image_dst	)

		PARAM_DEF_GROUP_CLOSED(	Convert, CONVERT_PARAM_NB	)
			PARAM_DEF_BOOL_OFF(			convert_enable			)
			PARAM_DEF_IMG_DST2(			convert_img_dst			)
			PARAM_DEF_SYMBO_PSTR_ZERO(	convert_type,			img_opencv::cv_convert_type_str )
			PARAM_DEF_NONE(				convert_timing			)
		PARAM_DEF_GROUP_CLOSED(	Blur, BLUR_PARAM_NB	)
			PARAM_DEF_BOOL_OFF(			blur_enable				)
			PARAM_DEF_IMG_DST2(			blur_img_dst			)
			PARAM_DEF_SYMBO_PSTR(		blur_method,			img_opencv::BLUR_BOX_NORMALIZED, img_opencv::BLUR_GAUSSIAN,	img_opencv::str_smooth_method )
			PARAM_DEF_INT32_XY(			blur_size,				1, 0,	1, 1024	)
			PARAM_DEF_NONE(				blur_timing				)
		PARAM_DEF_GROUP_CLOSED(	Brightness Contrast, BC_PARAM_NB	)
			PARAM_DEF_BOOL_OFF(			bc_enable				)
			PARAM_DEF_IMG_DST2(			bc_img_dst				)
			PARAM_DEF_REAL_ZERO(		bc_min					)
			PARAM_DEF_REAL_ONE(			bc_max					)
			PARAM_DEF_NONE(				bc_timing				)
		PARAM_DEF_GROUP_CLOSED(	Equalize Histogram, HIPARAM_DEF_NB	)
			PARAM_DEF_BOOL_OFF(			equalize_hist_enable	)
			PARAM_DEF_IMG_DST2(			equalize_hist_img_dst	)
			PARAM_DEF_NONE(				equalize_hist_timing	)
		PARAM_DEF_GROUP_CLOSED(	Denoising, DENOISE_PARAM_NB	)
			PARAM_DEF_BOOL_OFF(			denoise_enable			)
			PARAM_DEF_IMG_DST2(			denoise_img_dst			)
			PARAM_DEF_REAL(				denoise_strenght,		1, 3,	0, 128	)
			PARAM_DEF_INT32(			denoise_template_size,	1, 7,	1, 512	)
			PARAM_DEF_INT32(			denoise_windows_size,	1, 21,	1, 512	)
			PARAM_DEF_NONE(				denoise_timing			)
		PARAM_DEF_GROUP_CLOSED(	Bilateral Filter, BILATERAL_PARAM_NB	)
			PARAM_DEF_BOOL_OFF(			bilateral_enable		)
			PARAM_DEF_IMG_DST2(			bilateral_img_dst		)
			PARAM_DEF_INT32(			bilateral_size,			-1, 1,	-1, 1024	)
			PARAM_DEF_REAL(				bilateral_sigma_color,	20, 0,	0, 1024		)
			PARAM_DEF_REAL(				bilateral_sigma_space,	1, 0,	0, 1024		)
			PARAM_DEF_NONE(				bilateral_timing		)
		PARAM_DEF_GROUP_CLOSED(	Edge, EDGE_PARAM_NB	)
			PARAM_DEF_BOOL_OFF(			edge_enable				)
			PARAM_DEF_IMG_DST2(			edge_img_dst			)
			PARAM_DEF_BOOL_OFF(			edge_laplacian			)
			PARAM_DEF_SYMBO_PSTR_ZERO(	edge_size,				img_opencv::edge_size_str	)
			PARAM_DEF_REAL_ZERO(		edge_offset				)
			PARAM_DEF_REAL_ONE(			edge_scale				)
			PARAM_DEF_INT32_XY(			sobel_order,			1, 0,	0, 30	)
			PARAM_DEF_NONE(				edge_timing				)
		PARAM_DEF_GROUP_CLOSED(	Canny, CANNY_PARAM_NB	)
			PARAM_DEF_BOOL_OFF(			canny_enable			)
			PARAM_DEF_IMG_DST2(			canny_img_dst			)
			PARAM_DEF_REAL_POS(			canny_threshold_min,	0, 1	)
			PARAM_DEF_REAL_POS(			canny_threshold_max,	0, 1	)
			PARAM_DEF_SYMBO_PSTR_ZERO(	canny_size,				img_opencv::canny_size_str	)
			PARAM_DEF_BOOL_OFF(			canny_l2gradient		)
			PARAM_DEF_NONE(				canny_timing			)
		PARAM_DEF_BOOL_OFF(		Dilate_first		)
		PARAM_DEF_GROUP_CLOSED(	Erode, ERODE_PARAM_NB	)
			PARAM_DEF_BOOL_OFF(			erode_enable			)
			PARAM_DEF_IMG_DST2(			erode_img_dst			)
			PARAM_DEF_SYMBO_PSTR_ZERO(	erode_type,				img_opencv::cv_morph_type_str	)
			PARAM_DEF_INT32_XY(			erode_size,				1, 0,	0, 1024	)
			PARAM_DEF_INT32(			erode_iteration,		1, 1,	1, 1024	)
			PARAM_DEF_NONE(				erode_timing			)
		PARAM_DEF_GROUP_CLOSED(	Dilate, DILATE_PARAM_NB	)
			PARAM_DEF_BOOL_OFF(			dilate_enable			)
			PARAM_DEF_IMG_DST2(			dilate_img_dst			)
			PARAM_DEF_SYMBO_PSTR_ZERO(	dilate_type,			img_opencv::cv_morph_type_str	)
			PARAM_DEF_INT32_XY(			dilate_size,			1, 0,	0, 1024	)
			PARAM_DEF_INT32(			dilate_iteration,		1, 1,	1, 1024	)
			PARAM_DEF_NONE(				dilate_timing			)
		PARAM_DEF_GROUP_CLOSED(	Invert, INVERT_PARAM_NB	)
			PARAM_DEF_BOOL_OFF(			invert_enable			)
			PARAM_DEF_IMG_DST2(			invert_img_dst			)
			PARAM_DEF_NONE(				invert_timing			)
		PARAM_DEF_GROUP_CLOSED(	Brightness Contrast 2, BC_PARAM_NB	)
			PARAM_DEF_BOOL_OFF(			bc_enable				)
			PARAM_DEF_IMG_DST2(			bc_img_dst				)
			PARAM_DEF_REAL_ZERO(		bc_min					)
			PARAM_DEF_REAL_ONE(			bc_max					)
			PARAM_DEF_NONE(				bc_timing				)
		PARAM_DEF_GROUP_CLOSED( Optical Flow, OPTFLOW_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			optical_flow_enable		)
			PARAM_DEF_IMG_DST2(			optical_flow_image_dst	)
			PARAM_DEF_REAL(				optical_flow_pyr_scale,		0.1,	0.5,	0.01,	0.999 )
			PARAM_DEF_INT32(			optical_flow_levels,		3,		1,		1,		16 )
			PARAM_DEF_INT32(			optical_flow_winsize,		15,		1,		1,		201 )				//todo refine
			PARAM_DEF_INT32(			optical_flow_iters,			3,		1,		1,		16 )
			PARAM_DEF_SYMBO_PSTR(		optical_flow_poly_n,		7,		5,		img_opencv::str_poly_n )	//todo more value than 5 and 7
			PARAM_DEF_REAL(				optical_flow_poly_sigma,	1.2,	1.1,	1.01,	1.99 )
			PARAM_DEF_BOOL_OFF(			optical_flow_initial_flow	)
			PARAM_DEF_BOOL_OFF(			optical_flow_gaussian		)
			PARAM_DEF_BOOL_ON(			optical_flow_Farneback		)	//todo should probably make a method paraneter and expose params of each method 
			PARAM_DEF_NONE(				optical_flow_timing			)
		PARAM_DEF_GROUP_CLOSED( Inpainting Smoothing, INPAINTING_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			inpaint_smooth				)
			PARAM_DEF_IMG_DST2(			inpaint_smooth_img_dst		)
			PARAM_DEF_REAL_POS(			inpaint_smooth_resize_x,	.5, .2	)
			PARAM_DEF_REAL_POS(			inpaint_smooth_resize_y,	.5, .2	)
			PARAM_DEF_REAL_POS(			inpaint_smooth_radius,		10, 5	)
			PARAM_DEF_INT32_POS(		inpaint_smooth_no_depth,	255, 0	)
			PARAM_DEF_NONE(				inpaint_smooth_timing		)
		PARAM_DEF_GROUP_CLOSED(	Distance, DISTRANSFORM_PARAM_NB	)
			PARAM_DEF_BOOL_OFF(			dist_enable				)
			PARAM_DEF_IMG_DST2(			dist_img_dst			)
			PARAM_DEF_SYMBO_PSTR_ZERO(	dist_type,				img_opencv::dist_transform_type_str )
			PARAM_DEF_SYMBO_PSTR_ZERO(	dist_size,				img_opencv::dist_transform_size_str )
			PARAM_DEF_SYMBO_PSTR_ZERO(	dist_out_type,			img_opencv::dist_type_asked_str )
			PARAM_DEF_DOUBLE_ONE(		dist_factor				)
			PARAM_DEF_DOUBLE_ZERO(		dist_offset				)
			PARAM_DEF_NONE(				dist_timing				)
	};
}


void	c_bdd_img_opencv::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start_src_resize( false, true );

	param_set_pt( h, _speed_bdd->get_pt_interval_ms()	);

	_pass_convert.begin_param_init( h );
		param_set_pt( h, _convert_type_ui				);
	_pass_convert.end_param_init( h );

	_pass_blur.begin_param_init( h );
		param_set_pt( h, _s_blur_method_ui				);
		param_set_pt( h, _blur_size_x_ui				);
		param_set_pt( h, _blur_size_y_ui				);
	_pass_blur.end_param_init( h );

	_pass_bc.begin_param_init( h );
		param_set_pt( h, _bc_min_ui						);
		param_set_pt( h, _bc_max_ui						);
	_pass_bc.end_param_init( h );

	_pass_hist.begin_param_init( h );
	_pass_hist.end_param_init( h );

	_pass_denoise.begin_param_init( h );
		param_set_pt( h, _denoise_strenght_ui			);
		param_set_pt( h, _denoise_template_ui			);
		param_set_pt( h, _denoise_windows_ui			);
	_pass_denoise.end_param_init( h );

	_pass_bilateral.begin_param_init( h );
		param_set_pt( h, _bilateral_size_ui				);
		param_set_pt( h, _bilateral_sigma_color_ui		);
		param_set_pt( h, _bilateral_sigma_space_ui		);
	_pass_bilateral.end_param_init( h );

//todo
/*
//	get_param(h)->set_comment( _b_edge_ui ? ( _b_laplacian_ui ? "Laplacian" : "Sobel" ) : nullptr );
	if( _b_edge_ui )
	{
		if( _edge_status == aaa::c_cv::STATUS::OK )
		{
			if( g_speed_master->is_run() )
			{
				CHAR	buf[256];
				sprintf( buf, "%s %.2f %.1f", _b_laplacian_ui ? "Laplacian" : "Sobel", _speed_edge->get_interval_last_ms(), _speed_edge->get_fps_average() );
				get_param(h)->set_comment( buf );
			}
			else
			{
				get_param(h)->set_comment( "ON" );
			}
		}
		else
		{
			aaa::c_cv::build_comment_status( &sum_up, _b_edge_ui, _edge_status );
			get_param(h)->set_comment( sum_up );
		}
	}
	else
	{
		get_param(h)->clear_comment();
	}
*/
	_pass_edge.begin_param_init( h );
		param_set_pt( h, _b_laplacian_ui				);
		param_set_pt( h, _edge_size_ui					);
		param_set_pt( h, _edge_offset_ui				);
		param_set_pt( h, _edge_scale_ui					);
		param_set_pt( h, _sobel_order_x_ui				);
		param_set_pt( h, _sobel_order_y_ui				);
	_pass_edge.end_param_init( h );

	_pass_canny.begin_param_init( h );
		param_set_pt( h, _canny_threshold_min_ui		);
		param_set_pt( h, _canny_threshold_max_ui		);
		param_set_pt( h, _canny_size_ui					);
		param_set_pt( h, _b_l2gradient_ui				);
	_pass_canny.end_param_init( h );

	param_set_pt( h, _b_dilate_first_ui			);

	_pass_erode.begin_param_init( h );
		param_set_pt( h, _erode_type_ui					);
		param_set_pt( h, _erode_size_x_ui				);
		param_set_pt( h, _erode_size_y_ui				);
		param_set_pt( h, _erode_iteration_ui			);
	_pass_erode.end_param_init( h );

	_pass_dilate.begin_param_init( h );
		param_set_pt( h, _dilate_type_ui				);
		param_set_pt( h, _dilate_size_x_ui				);
		param_set_pt( h, _dilate_size_y_ui				);
		param_set_pt( h, _dilate_iteration_ui			);
	_pass_dilate.end_param_init( h );

	_pass_invert.begin_param_init( h );
	_pass_invert.end_param_init( h );

	_pass_bc_2.begin_param_init( h );
		param_set_pt( h, _bc_2_min_ui					);
		param_set_pt( h, _bc_2_max_ui					);
	_pass_bc_2.end_param_init( h );

	_pass_optflow.begin_param_init( h );
		param_set_pt( h, _optflow_pyr_scale				);
		param_set_pt( h, _optflow_levels				);
		param_set_pt( h, _optflow_winsize				);
		param_set_pt( h, _optflow_iters					);
		param_set_pt( h, _optflow_poly_n_ui				);
		param_set_pt( h, _optflow_poly_sigma			);
		param_set_pt( h, _b_optflow_initial_flow_ui		);
		param_set_pt( h, _b_optflow_gaussian			);
		param_set_pt( h, _b_optflow_use_Farneback_ui	);
	_pass_optflow.end_param_init( h );

	_pass_inpaint.begin_param_init( h );
		param_set_pt( h, _inpaint_resize_x_ui			);
		param_set_pt( h, _inpaint_resize_y_ui			);
		param_set_pt( h, _inpaint_radius_ui				);
		param_set_pt( h, _inpaint_no_depth_ui			);
	_pass_inpaint.end_param_init( h );

	_pass_distrans.begin_param_init( h );
		param_set_pt( h, _distrans_type_ui				);
		param_set_pt( h, _distrans_size_ui				);
		param_set_pt( h, _s_distrans_out_type_ui		);
		param_set_pt( h, _distrans_factor_ui			);
		param_set_pt( h, _distrans_offset_ui			);
	_pass_distrans.end_param_init( h );

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bdd_img_opencv )
	,pass_cur	(nullptr)
{
	_speed_bdd = new c_speed( true,	4, g_speed_master->get_pb_print_bdd(), "update", this );
	obj_new( _speed_bdd_ui )->set_timing( _speed_bdd );

	_pass_convert	.init(	this,	"convert"				);
	_pass_hist		.init(	this,	"equalize histogram"	);
	_pass_denoise	.init(	this,	"denoise"				);
	_pass_bilateral	.init(	this,	"bilateral filter"		);
	_pass_blur		.init(	this,	"blur"					);
	_pass_bc		.init(	this,	"bc"					);
	_pass_bc_2		.init(	this,	"bc_2"					);
	_pass_invert	.init(	this,	"invert"				);
	_pass_edge		.init(	this,	"edge"					);
	_pass_canny		.init(	this,	"canny"					);
	_pass_dilate	.init(	this,	"dilate"				);
	_pass_erode		.init(	this,	"erode"					);
	_pass_optflow	.init(	this,	"optical flow"			);
	_pass_inpaint	.init(	this,	"inpainting"			);
	_pass_distrans	.init(	this,	"distance transform"	);

	param_init_with( n_bdd_img_opencv::param, n_bdd_img_opencv::PARAM_NB );
}

void	c_bdd_img_opencv::dealloc()
{
	IF_THIS_NULL_RETURN();

	_mat_dst_a.release();

	_img_prev.release();
//	_optflow_buf.release();
#if AAA_LIB_USE_OPENCV_CUDA()
	_img_prev_gpu.release();
//	_optflow_gpu_buf.release();
#endif

	_hist4_dst.release();
#if AAA_LIB_USE_OPENCV_CUDA()
	_hist4_gpu_dst.release();
#endif

	_inpainting_dst.release();
	_inpaint_small_dst.release();
	_inpaint_resize_dst.release();

	_distrans_tmp.release();
	_distrans_convert_src.release();

	obj_delete( _speed_bdd );
	obj_delete( _speed_bdd_ui );
}

c_bdd_img_opencv::~c_bdd_img_opencv()
{
	dealloc();
}

FINLINE INT32 c_bdd_img_opencv::get_src_channel()
{
#if AAA_LIB_USE_OPENCV_CUDA()
	return is_gpu_use() ? _src_cur_gpu->channels() : _src_cur->channels();
#else
	return _src_cur->channels();
#endif
}

FINLINE INT32 c_bdd_img_opencv::get_depth()
{
#if AAA_LIB_USE_OPENCV_CUDA()
	return is_gpu_use() ? _src_cur_gpu->depth() : _src_cur->depth();
#else
	return _src_cur->channels();
#endif
}

void	c_bdd_img_opencv::do_process()
{
	try //todo perhaps we need to do something about SPY_PUSH/POP
	{	
		if( _pass_convert.is_todo()		)
			do_convert(			_pass_convert );
		if( _pass_blur.is_todo()		)
			do_blur(			_pass_blur );	
		if( _pass_bc.is_todo()			)
			do_bc(				_pass_bc,  _bc_min_ui, _bc_max_ui );
		if( _pass_hist.is_todo()		)
			do_equalize_hist(	_pass_hist );
		if( _pass_denoise.is_todo()		)
			do_denoise(			_pass_denoise );
		if( _pass_bilateral.is_todo()	)
			do_bilateral_filter(_pass_bilateral );
		if( _pass_edge.is_todo()		)
			do_edge(			_pass_edge );
		if( _pass_canny.is_todo()		)
			do_canny(			_pass_canny );

		if( _b_dilate_first_ui && _pass_dilate.is_todo()		)
			do_dilate(			_pass_dilate );
		if( _pass_erode.is_todo()		)
			do_erode(			_pass_erode );
		if( !_b_dilate_first_ui && _pass_dilate.is_todo()		)
			do_dilate(			_pass_dilate );

		if( _pass_invert.is_todo()		)
			do_invert(			_pass_invert );
		if( _pass_bc_2.is_todo()			)
			do_bc(				_pass_bc_2,	_bc_2_min_ui, _bc_2_max_ui );

		if( _pass_optflow.is_todo()		)
			do_optical_flow(	_pass_optflow ); // experimental
	}
	catch( cv::Exception& e )
	{	
		ERR_PRINT_STRING( "Open CV Exception : %s", e.what() );
		if( pass_cur )
		{
			pass_cur->_status = aaa::c_cv::STATUS::ERR_CV_EXCEPTION;
			pass_cur = nullptr;
		}
	}

#if AAA_LIB_USE_OPENCV_CUDA()
	//because the two last stage use only cpu
	if( _pass_inpaint._b_active_ui && _pass_distrans._b_active_ui && is_gpu_use() && !_src_cur_gpu->empty() )
	{
		_src_cur		= &_pass_invert._dst;	//hack we use the _invert_dst buffer
		_src_cur_gpu->download( *_src_cur );		
		_b_src_is_gpu	= false;
	}
#endif

	if( _pass_inpaint.is_todo()	)
		do_inpaint_smooth( _pass_inpaint );
	if( _pass_distrans.is_todo() )
		do_distance_transform( _pass_distrans );

	//if( !build_cv_mat( _mat_dst_a, _dst_img_index_ui, _src_cur, "bdd_img_opencv" ) )
	//{
	//	err_print( "Error building dst" );
	//	return;
	//}
	//_src_cur.copyTo( _mat_dst_a );
	update_dst_index( _dst_img_index_ui, "bdd_img_opencv" );
	//hack for quartz face
	tex_2d_bind( _dst_img_index_ui );
}

void	c_bdd_img_opencv::update()
{
	_speed_bdd->begin();
	bool b_change_it;
	if( update_mat_src( b_change_it ) )
		do_process();
	_speed_bdd->end();
}

void	c_bdd_img_opencv::draw()
{
}

void	c_bdd_img_opencv::begin_pass(	c_opencv_pass& pass	)
{
	pass.begin_process();
	pass_cur = &pass;
}
void	c_bdd_img_opencv::end_pass(		c_opencv_pass& pass	)
{
	pass.end_process();
	pass_cur = nullptr;
}

void	c_bdd_img_opencv::do_invert( c_opencv_pass& pass )
{
	begin_pass( pass );

		//build_dst_index( _invert_dst, _invert_dst_index, "Invert Destination"  );
#if AAA_LIB_USE_OPENCV_CUDA()
		if( is_gpu_use() )
		{
			if( _b_float )
				//_invert_gpu_dst.create( _src_cur_gpu->size(), _src_cur_gpu->type() );
				cv::cuda::subtract( cv::Scalar(1,1,1), *_src_cur_gpu, pass._dst_gpu );
			else
				cv::cuda::bitwise_not( *_src_cur_gpu, pass._dst_gpu );
		}
		else
#endif
		{
			if( _b_float )
				cv::subtract( 1.0f, *_src_cur, pass._dst );
			else
				cv::bitwise_not( *_src_cur, pass._dst );
		}

	end_pass( pass );
}

// stores a single previous image for now
// todo extend to n images
// todo consider modifying image bind system to store n prev and n next images ( ex: video stream )
void	c_bdd_img_opencv::store_img_prev(  )
{
	//cv::UMat	tmp;
	//size_t		n = _imgs_prev.size();
	
	_time_got_frame_imgs_last = _time_got_frame_imgs;
	_time_got_frame_imgs = REAL(aaa::time::get_real_time());
#if AAA_LIB_USE_OPENCV_CUDA()
	if( is_gpu_use() )
		_src_cur_gpu->copyTo( _img_prev_gpu );
	else
#endif
		_src_cur->copyTo( _img_prev );

	//for( int i = 0; i < n ; i++ )
	//{
	//	_imgs_prev[_imgs_prev.begin()];
	//}
}

void	c_bdd_img_opencv::do_optical_flow( c_opencv_pass& pass )
{
	if( get_src_channel()!=1 )
	{
		pass._status = aaa::c_cv::STATUS::ERR_CH_1_ONLY;
		err_print( "optical flow need mono image ( for now ? )");
		return;
	}

	begin_pass( pass );

//#if AAA_LIB_USE_OPENCV_CUDA()
	//	if ( is_gpu_use() )		if ( _src_cur_gpu->channels() != 1 && _src_cur_gpu->channels() != 4 )	{	_optflow_status = aaa::c_cv::STATUS::ERR_1_4_CH;	return;		}
//#endif
		int flags = _b_optflow_initial_flow_ui ? cv::OPTFLOW_USE_INITIAL_FLOW : 0;
		if( _b_optflow_gaussian )
			flags |= cv::OPTFLOW_FARNEBACK_GAUSSIAN;
#if AAA_LIB_USE_OPENCV_CUDA()
		if( is_gpu_use() )
		{
			auto CONST & size_src = _src_cur_gpu->size();
			auto CONST & size_dst = pass._dst_gpu.size();
			if( size_src.width != size_dst.width || size_src.height != size_dst.height )
			{
	//			_optflow_gpu_buf.release();
	//			_optflow_gpu_buf.create( size_src, CV_8UC1 );
				pass._dst_gpu.release();
				pass._dst_gpu.create( size_src, CV_32FC2 );
				store_img_prev();
			}

			int winsize = (_optflow_winsize & 0xfffffffe) + 1;
			cv::Ptr<cv::cuda::FarnebackOpticalFlow> flow = flow->create( _optflow_levels, _optflow_pyr_scale, false, winsize, _optflow_iters,
																	img_opencv::cv_poly_n[_optflow_poly_n_ui], _optflow_poly_sigma, flags );
			flow->calc( _img_prev_gpu, *_src_cur_gpu, pass._dst_gpu );

	/*		cv::cuda::GpuMat flow_parts[2];
			cv::cuda::split( _optflow_gpu_buf, flow_parts );
			cv::cuda::GpuMat magnitude, angle, magn_norm;
			cv::cuda::cartToPolar( flow_parts[0], flow_parts[1], magnitude, angle, true );
			cv::cuda::normalize( magnitude, magn_norm, 0.0f, 1.0f, cv::NORM_MINMAX, -1 );
			cv::cuda::multiply( angle, ( ( 1.f / 360.f ) * ( 180.f / 255.f ) ), angle );
		
			//build hsv image
			cv::cuda::GpuMat hsvs[3], hsv, hsv8;
			hsvs[0] = angle;
			hsvs[1] = cv::cuda::GpuMat( angle.size(), CV_32FC1 );
			hsvs[1].setTo( 1. );
			hsvs[2] = magn_norm;
			cv::cuda::merge( hsvs,	3, hsv );
			hsv.convertTo( hsv8, CV_8U, 255.0 );
			cv::cuda::cvtColor( hsv8, _optflow_gpu_dst, cv::COLOR_HSV2BGR );
	*/

			store_img_prev();
		}
		else
#endif
		{
			auto CONST & size_src =  _src_cur->size();
			auto CONST & size_dst =  pass._dst.size();
			if( size_src.width != size_dst.width || size_src.height != size_dst.height )
			{
	//			_optflow_buf.release();
	//			_optflow_buf.create( size_src, CV_8UC1 );
				pass._dst.release();
				pass._dst.create( size_src, CV_32FC2 );
				store_img_prev();
			}

	/////////////////////////////////////////////////////

			if( _b_optflow_use_Farneback_ui )
			{
				cv::calcOpticalFlowFarneback( _img_prev, *_src_cur, pass._dst, _optflow_pyr_scale, _optflow_levels,
					_optflow_winsize, _optflow_iters,
					img_opencv::cv_poly_n[_optflow_poly_n_ui], _optflow_poly_sigma, flags );
			}
			else 
			{
				// calculate using Lucas-Kanade method
				// sample code from : https://docs.opencv.org/4.10.0/d4/dee/tutorial_optical_flow.html#autotoc_md1144

				std::vector<cv::Point2f> _prev_points;// , _next_points;

				cv::goodFeaturesToTrack( _img_prev, _prev_points, 100, 0.3, 7.0 );

				std::vector<uchar> _status;
				std::vector<float> _err;
				cv::TermCriteria _criteria = cv::TermCriteria::TermCriteria((cv::TermCriteria::COUNT)+(cv::TermCriteria::EPS), 10, 0.03);
				cv::calcOpticalFlowPyrLK( _img_prev, *_src_cur, _prev_points, pass._dst, _status, _err, cv::Size(15, 15), flags, _criteria );
			}

/////////////////////////////////////////////////////

/*		cv::Mat flow_parts[2];
		cv::split( _optflow_buf.getMat( cv::ACCESS_RW ), flow_parts );
		cv::Mat magnitude, angle, magn_norm;
		cv::cartToPolar( flow_parts[0], flow_parts[1], magnitude, angle, true );
		cv::normalize( magnitude, magn_norm, 0.0f, 1.0f, cv::NORM_MINMAX );
		angle *= ( ( 1.f / 360.f ) * ( 180.f / 255.f ) );

		//build hsv image
		cv::Mat hsvs[3], hsv, hsv8;
		hsvs[0] = angle;
		hsvs[1] = cv::Mat::ones( angle.size(), CV_32FC1 );
		hsvs[2] = magn_norm;
		cv::merge( hsvs, 3, hsv );
		hsv.convertTo( hsv8, CV_8U, 255.0 );
		cv::cvtColor( hsv8, _optflow_dst, cv::COLOR_HSV2BGR );
*/
			store_img_prev();
		}

	end_pass( pass );
}

void	c_bdd_img_opencv::do_edge( c_opencv_pass& pass )
{
	INT32 ch_nb = get_src_channel();
#if AAA_LIB_USE_OPENCV_CUDA()
	if( is_gpu_use() )
	{
		if( ch_nb != 1 && ch_nb != 4 )
		{	pass._status = aaa::c_cv::STATUS::ERR_CH_14_ONLY; return; }
	}
	else
#endif
	{
	//if( _src_cur->type() != 1 && _src_cur->type() != 4 )				return;
	}

	begin_pass( pass );


//	build_dst_index(  _edge_dst, _edge_dst_index, "Edge Destination"  );

		INT32	ksize	=	img_opencv::cv_edge_size[ _edge_size_ui ];
		INT32	order_x	=	_sobel_order_x_ui;
		INT32	order_y	=	_sobel_order_y_ui;
		if( _b_laplacian_ui )
		{
			ksize = MAX1( ksize );
#if AAA_LIB_USE_OPENCV_CUDA()
			if( is_gpu_use() )
			{
				CLAMP_REF( ksize, 1, 3 );
			}
#endif
		}
		else
		{
			if( ksize == -1 )
			{	//	Schaar case
				if( (order_x + order_y) != 1 )
				{
					order_x = 1;
					order_y = 0;
					err_print( "%s() Schaar case forcing order_x / order_y to 1 / 0", __FUNCTION__ );
				}
			}
			else
			{
				if( (order_x + order_y) == 0 )
				{
					order_x = 1;
					order_y = 0;
					err_print( "%s() Sobel forcing order_x / order_y to 1 / 0", __FUNCTION__ );
				}
				else
				{
					if( ksize > 1 )
					{
						order_x = MIN( order_x, ksize-1 );
						order_y = MIN( order_y, ksize-1 );
					}
					else
					{
						order_x = MIN( order_x, 2 );
						order_y = MIN( order_y, 2 );
					}
				}
			}
		}
		DOUBLE	offset = _edge_offset_ui * ( _b_float ? 1.0 : aaa::img::REAL_NEARLY_256 );
#if AAA_LIB_USE_OPENCV_CUDA()
		if( is_gpu_use() )
		{
			if( _b_laplacian_ui )
			{
				cv::Ptr<cv::cuda::Filter> openFilter = cv::cuda::createLaplacianFilter( _src_cur_gpu->type(), _src_cur_gpu->type(), ksize, _edge_scale_ui, cv::BORDER_DEFAULT );
				openFilter->apply( *_src_cur_gpu, pass._dst_gpu );
			//	cv::cuda::Laplacian( _src_cur_gpu, _edge_gpu_dst, -1, ksize, _edge_scale, cv::BORDER_DEFAULT );
			}
			else
			{
				cv::Ptr<cv::cuda::Filter> openFilter = cv::cuda::createSobelFilter( _src_cur_gpu->type(), _src_cur_gpu->type(), order_x, order_y, ksize, _edge_scale_ui, cv::BORDER_DEFAULT );
				openFilter->apply( *_src_cur_gpu, pass._dst_gpu );
				//cv::cuda::Sobel( _src_cur_gpu, _edge_gpu_dst, -1, order_x, order_y, ksize, _edge_scale, cv::BORDER_DEFAULT );
			}
		}
		else
#endif
		{
			if( _b_laplacian_ui )
				cv::Laplacian( *_src_cur, pass._dst, -1, ksize, _edge_scale_ui, offset, cv::BORDER_DEFAULT );
			else
				cv::Sobel( *_src_cur, pass._dst, -1, order_x, order_y, ksize, _edge_scale_ui, offset, cv::BORDER_DEFAULT );
		}

	end_pass( pass );
}

void	c_bdd_img_opencv::do_canny( c_opencv_pass& pass )
{
	INT32 ch_nb = get_src_channel();
	if( ch_nb!=1 )
		{	pass._status = aaa::c_cv::STATUS::ERR_CH_1_ONLY;	return;	}
	if( _b_float )
		{	pass._status = aaa::c_cv::STATUS::ERR_FLOAT;		return;	}
	if( get_depth()!=CV_8U )
		{	pass._status = aaa::c_cv::STATUS::ERR_UINT8_ONLY;	return;	}

	begin_pass( pass );

	//	C++: void gpu::Canny(const GpuMat& image, GpuMat& edges, double low_thresh, double high_thresh, int apperture_size=3, bool L2gradient=false )
	//	build_dst_index( _canny_dst, _canny_dst_index, "Canny Destination" );
		INT32 size_aperture = img_opencv::cv_canny_size[ _canny_size_ui ];
		DOUBLE th1 = _canny_threshold_min_ui * 256 * size_aperture * size_aperture;
		DOUBLE th2 = _canny_threshold_max_ui * 256 * size_aperture * size_aperture;
#if AAA_LIB_USE_OPENCV_CUDA()
		if( is_gpu_use() )
		{
			cv::Ptr<cv::cuda::CannyEdgeDetector> openFilter = cv::cuda::createCannyEdgeDetector( th1,th2, size_aperture, _b_l2gradient_ui );
			openFilter->detect( *_src_cur_gpu, pass._dst_gpu );
		//	cv::cuda::Canny( _src_cur_gpu, _canny_gpu_dst, th1, th2, ksize, _b_l2gradient_ui );
		}
		else
#endif
			cv::Canny( *_src_cur, pass._dst, th1,th2, size_aperture, _b_l2gradient_ui );

	end_pass( pass );
}

void	c_bdd_img_opencv::do_distance_transform( c_opencv_pass& pass )
{
	//if( _b_float )
	//	{	pass._status = aaa::c_cv::STATUS::ERR_FLOAT;	return;		}
	//if( get_src_channel()!=1 )
	//	{	pass._status = aaa::c_cv::STATUS::ERR_CH_1_ONLY;	return; }

	begin_pass( pass );

		//distanceTransform only accept one format as input (2024 OpenCV 4.10.0)
		if( _src_cur->type() != CV_8UC1 )
		{
			SPY_PUSH_RANGE( "src cv::convertTo 8bits", spy::IMG_LOW );
				_src_cur->convertTo( _distrans_convert_src, CV_8UC1,  1., 0. );
				_src_cur = &_distrans_convert_src;
			SPY_POP_RANGE();
		}
		// do it here
		SPY_PUSH_RANGE( "cv::distanceTransform", spy::IMG_LOW );
			auto distrans_size = (_distrans_type_ui > 2) ? 2 : _distrans_size_ui;
			cv::distanceTransform( *_src_cur, _distrans_tmp, img_opencv::cv_dist_type[ _distrans_type_ui ], img_opencv::cv_dist_size[ distrans_size ], CV_32F );
		SPY_POP_RANGE();
		// output format
		switch( _s_distrans_out_type_ui )
		{
		case 0:	SPY_PUSH_RANGE( "dst cv::convertTo 8bits", spy::IMG_LOW );
					_distrans_tmp.convertTo( pass._dst, CV_8UC1,  _distrans_factor_ui * 256.,			_distrans_offset_ui * 256. );
				SPY_POP_RANGE();
				break;
		case 1:	SPY_PUSH_RANGE( "dst cv::convertTo 16bits", spy::IMG_LOW );
					_distrans_tmp.convertTo( pass._dst, CV_16UC1, _distrans_factor_ui * (256.*256.),	_distrans_offset_ui * (256.*256.) );
				SPY_POP_RANGE();
				break;
		case 2:	SPY_PUSH_RANGE( "dst cv::convertTo fp16", spy::IMG_LOW );
					_distrans_tmp.convertTo( pass._dst, CV_16FC1, _distrans_factor_ui,					_distrans_offset_ui );
				SPY_POP_RANGE();
				break;
		case 3:	SPY_PUSH_RANGE( "dst cv::convertTo fp32", spy::IMG_LOW );
					_distrans_tmp.convertTo( pass._dst, CV_32FC1, _distrans_factor_ui ,					_distrans_offset_ui );
				SPY_POP_RANGE();
				break;
		}

	end_pass( pass );
}

void	c_bdd_img_opencv::do_bc( c_opencv_pass& pass, REAL min, REAL max )
{
	begin_pass( pass );

	//	build_dst_index(  _bc_dst, _bc_dst_index, "Brightness Contrast Destination"  );
		REAL scale = OVER_ONE_AS_REAL( max - min );
		REAL offset = -min * ( _b_float ? REAL(1) : aaa::img::REAL_NEARLY_256 ) * scale;
#if AAA_LIB_USE_OPENCV_CUDA()
		if( is_gpu_use() )
			_src_cur_gpu->convertTo( pass._dst_gpu, -1, scale, offset );
		else
#endif
			_src_cur->convertTo( pass._dst, -1, scale, offset );

	end_pass( pass );
}


void	c_bdd_img_opencv::do_blur_gauss( c_opencv_pass& pass )
{
	//2024 OpenCV 4.10 doc all number of channel supported: not sure for cuda version
	//INT32 ch_nb = get_src_channel();
	//if( ch_nb==2 && ch_nb>4 )
	//{
	//	pass._status = aaa::c_cv::STATUS::ERR_CH_134_ONLY;
	//	return;
	//}

	begin_pass( pass );

		INT32 sx = _blur_size_x_ui;
		INT32 sy = _blur_size_y_ui;

	//	build_dst_index(  _blur_dst, _blur_dst_index, "Gauss Destination"  );
	
		if( (sx & 1)== 0 )
			sx += 1;
		if( (sy & 1)== 0 )
			sy += 1;

#if AAA_LIB_USE_OPENCV_CUDA()
		if( is_gpu_use() )
		{
			if( sx > 31 )
			{
				err_print( "%s() sx : %d can't be more than 31 in gpu mode : using 31", __FUNCTION__, sx );
				sx = 31;
			}
			if( sy > 31 )
			{
				err_print( "%s() sy : %d can't be more than 31 in gpu mode : using 31", __FUNCTION__, sy );
				sy = 31;
			}
			DOUBLE sig_x = 0.3*((sx-1)*0.5 - 1) + 0.8;
			DOUBLE sig_y = 0.3*((sy-1)*0.5 - 1) + 0.8;
			//todo only CV_8UC1, CV_8UC4, CV_32FC1 and CV_32FC4
			cv::Ptr<cv::cuda::Filter> openFilter = cv::cuda::createGaussianFilter( _src_cur_gpu->type(), _src_cur_gpu->type(), cv::Size(sx,sy), sig_x,sig_y );
			openFilter->apply( *_src_cur_gpu, pass._dst_gpu );
	//		cv::cuda::GaussianBlur( _src_cur_gpu, _blur_gpu_dst, cv::Size( sx, sy ), sig_x, sig_y );
			//cv::cuda::blur( _scr_cur_gpu, _blur_gpumat, size );
		}
		else
#endif
		{
			//todo only src Source 8-bit or floating-point, 1-channel or 3-channel image
			DOUBLE sig_x = 0.3*((sx-1)*0.5 - 1) + 0.8;
			DOUBLE sig_y = 0.3*((sy-1)*0.5 - 1) + 0.8;
			cv::GaussianBlur( *_src_cur, pass._dst, cv::Size(sx,sy), sig_x,sig_y );
		}

	end_pass( pass );
}

void	c_bdd_img_opencv::do_blur( c_opencv_pass& pass )
{
	if( _s_blur_method_ui == img_opencv::BLUR_GAUSSIAN )
	{
		do_blur_gauss( pass );
		return;
	}


	INT32 ch_nb = get_src_channel();
	INT32 sx = _blur_size_x_ui;
	INT32 sy = _blur_size_y_ui;
#if AAA_LIB_USE_OPENCV_CUDA()
	if( is_gpu_use() )
	{
		switch( _s_blur_method_ui )
		{
		case img_opencv::BLUR_MEDIAN:
			pass._status = aaa::c_cv::STATUS::ERR_GPU;
			return;
		default:
			if( _b_float )
				{	pass._status = aaa::c_cv::STATUS::ERR_FLOAT;	return;	}
			break;
		}
	}
	else
#endif
	{
		switch( _s_blur_method_ui )
		{
		case img_opencv::BLUR_MEDIAN:
			if( ch_nb==2 && ch_nb>4 )
				{	pass._status = aaa::c_cv::STATUS::ERR_CH_134_ONLY;	return;	}
			// size 3,5,7,9....
			if( (sx & 1)== 0 )
				sx += 1;
			else if( sx==1 )
				sx = 3;
			//todo deal when ksize is 3 or 5 the image depth should be CV_8U, CV_16U, or CV_32F, for larger aperture sizes, it can only be CV_8U.
			break;
		}
	}

	
#if AAA_LIB_USE_OPENCV_CUDA()
//	if( is_gpu_use() )		if ( ch_nb != 1 && _ch_nb != 4 )	{	pass._status = aaa::c_cv::STATUS::ERR_1_4_CH;	return;		}
#endif

	begin_pass( pass );

	//		build_dst_index(  _blur_dst, _blur_dst_index, "Blur Destination"  );

		cv::Size size( sx,sy );
#if AAA_LIB_USE_OPENCV_CUDA()
		if( is_gpu_use() )
		{
			cv::Ptr<cv::cuda::Filter> openFilter;
			switch( _s_blur_method_ui )
			{
			case img_opencv::BLUR_BOX_NORMALIZED:	//deal with only CV_8UC1, CV_8UC4 and CV_32FC1
				openFilter = cv::cuda::createBoxFilter( _src_cur_gpu->type(), _src_cur_gpu->type(), size );
				break;
			case img_opencv::BLUR_MEDIAN:	//deal woth only Only CV_8UC1
				openFilter = cv::cuda::createMedianFilter( _src_cur_gpu->type(), _src_cur_gpu->type(), sx );
				break;
			}
			openFilter->apply( *_src_cur_gpu, pass._dst_gpu );
			//cv::cuda::boxFilter( _src_cur_gpu, _blur_gpu_dst, -1, size );
		}
		else
#endif
		{
			switch( _s_blur_method_ui )
			{
			case img_opencv::BLUR_BOX:
			case img_opencv::BLUR_BOX_NORMALIZED:
				cv::boxFilter( *_src_cur, pass._dst, _src_cur->type(), size, cv::Point(-1,-1), _s_blur_method_ui==img_opencv::BLUR_BOX_NORMALIZED );
				break;
			case img_opencv::BLUR_MEDIAN:
				cv::medianBlur( *_src_cur, pass._dst, sx );
				break;
			}
		}

	end_pass( pass );
}

void	c_bdd_img_opencv::do_dilate( c_opencv_pass& pass )
{
	if( _b_float )
	{
		pass._status = aaa::c_cv::STATUS::ERR_FLOAT;
		return;
	}
#if AAA_LIB_USE_OPENCV_CUDA()
//	if( is_gpu_use() )
//	{
//		if( (_src_cur_gpu->channels() != 1) && (_src_cur_gpu->channels() != 4) )	{	pass._status = aaa::c_cv::STATUS::ERR_1_4_CH;		return;		}
//	}
//	else
#endif
//		if( (_src_cur->channels()) != 1 && (_src_cur->channels() != 4) )			{	pass._status = aaa::c_cv::STATUS::ERR_1_4_CH;		return;		}

	begin_pass( pass );

	//	build_dst_index(  _dilate_dst, _dilate_dst_index, "Dilate Destination"  );
	//	UINT32	size_x = (_dilate_size_x_ui % 2 == 0) ? _dilate_size_x_ui + 1 : _dilate_size_x_ui;
	//	UINT32	size_y = (_dilate_size_y_ui % 2 == 0) ? _dilate_size_y_ui + 1 : _dilate_size_y_ui;
		UINT32	sx = _dilate_size_x_ui;
		UINT32	sy = _dilate_size_y_ui;
		cv::Mat	kernel = cv::getStructuringElement( img_opencv::cv_morph_type[ _dilate_type_ui ], cv::Size( 2*sx+1, 2*sy+1 ), cv::Point( sx, sy ) );
#if AAA_LIB_USE_OPENCV_CUDA()
		if( is_gpu_use() )
		{
			//todo add other mode e.g. MORPH_GRADIENT
			cv::Ptr<cv::cuda::Filter> openFilter = cv::cuda::createMorphologyFilter( cv::MORPH_DILATE, _src_cur_gpu->type(), kernel, cv::Point(-1,-1), _dilate_iteration_ui );
			openFilter->apply( *_src_cur_gpu, pass._dst_gpu );
		}
		else
#endif
			cv::dilate( *_src_cur, pass._dst, kernel, cv::Point(-1,-1), _dilate_iteration_ui );

	end_pass( pass );
}

void	c_bdd_img_opencv::do_erode( c_opencv_pass& pass )
{
	if( _b_float )
	{
		pass._status = aaa::c_cv::STATUS::ERR_FLOAT;
		return;
	}
#if AAA_LIB_USE_OPENCV_CUDA()
//	if( is_gpu_use() )
//	{
//		if( (_src_cur_gpu->channels() != 1) && (_src_cur_gpu->channels() != 4) )	{	pass._status = aaa::c_cv::STATUS::ERR_1_4_CH;		return;		}
//	}
//	else
#endif
//		if( (_src_cur->channels()) != 1 && (_src_cur->channels() != 4) )			{	pass._status = aaa::c_cv::STATUS::ERR_1_4_CH;		return;		}

	begin_pass( pass );

	//	build_dst_index(  _erode_dst, _erode_dst_index, "Erode Destination"  );
	//	UINT32	size_x = (_erode_size_x_ui % 2 == 0) ? _erode_size_x_ui + 1 : _erode_size_x_ui;
	//	UINT32	size_y = (_erode_size_y_ui % 2 == 0) ? _erode_size_y_ui + 1 : _erode_size_y_ui;
		UINT32	sx = _erode_size_x_ui;
		UINT32	sy = _erode_size_y_ui;
		cv::Mat	kernel = cv::getStructuringElement( img_opencv::cv_morph_type[ _dilate_type_ui ], cv::Size( 2*sx+1, 2*sy+1 ), cv::Point( sx, sy ) );
#if AAA_LIB_USE_OPENCV_CUDA()
		if( is_gpu_use() )
		{
			//todo add other mode e.g. MORPH_GRADIENT
			cv::Ptr<cv::cuda::Filter> openFilter = cv::cuda::createMorphologyFilter( cv::MORPH_ERODE, _src_cur_gpu->type(), kernel, cv::Point(-1,-1), _erode_iteration_ui );
			openFilter->apply( *_src_cur_gpu, pass._dst_gpu );
		}
		else
#endif
			cv::erode( *_src_cur, pass._dst, kernel, cv::Point(-1,-1), _erode_iteration_ui );

	end_pass( pass );
}

void	c_bdd_img_opencv::do_equalize_hist( c_opencv_pass& pass )
{
	if( _b_float )
	{
		pass._status = aaa::c_cv::STATUS::ERR_FLOAT;
		return;
	}

	INT32 ch_nb = get_src_channel();

	begin_pass( pass );

	//	build_dst_index(  _hist_dst, _hist_dst_index_ui, "Equalize Histogram Destination" );
#if AAA_LIB_USE_OPENCV_CUDA()
		if( is_gpu_use() )
		{
			switch( ch_nb )
			{
			case 1:
			case 2:
				cv::cuda::equalizeHist( *_src_cur_gpu, pass._dst_gpu );
				break;
			case 3:
			case 4:
				std::vector<cv::cuda::GpuMat>	channels; 
				//		std::vector<cv::cuda::GpuMat>	channels_rgba; 
				//		cv::cuda::split( _hist_gpumat, channels_rgba );					//split the image into channels

				cv::cuda::cvtColor(		*_src_cur_gpu,	pass._dst_gpu,	CV_BGR2YCrCb	);	//change the color image from BGR to YCrCb format
				cv::cuda::split(		pass._dst_gpu,	channels						);	//split the image into channels
				cv::cuda::equalizeHist(	channels[0],	channels[0]						);	//equalize histogram on the 1st channel (Y)
				cv::cuda::merge(		channels,		pass._dst_gpu					);	//merge 3 channels including the modified 1st channel into one image
				cv::cuda::cvtColor(		pass._dst_gpu,	pass._dst_gpu,	CV_YCrCb2BGR	);	//change the color image from YCrCb to BGR format (to display image properly)
				if( ch_nb == 4 )
				{
					//	cv::cuda::merge( channels, _hist_gpumat );
					cv::cuda::cvtColor(	pass._dst_gpu,	_hist4_gpu_dst,	CV_BGR2BGRA		);	//change the color image from YCrCb to BGR format (to display image properly)
					//_src_cur_gpu = &_hist4_gpu_dst;	//todonow
				}
				break;
			}
		}
		else
#endif
		{
			switch( ch_nb )
			{
			case 1:
			case 2:
				cv::equalizeHist( *_src_cur, pass._dst );
				break;
			case 3:
			case 4:
				std::vector<cv::Mat>	channels; 
				cv::cvtColor(		*_src_cur,		pass._dst,	cv::COLOR_BGR2YCrCb	);	//change the color image from BGR to YCrCb format
				cv::split(			pass._dst,		channels						);	//split the image into channels
				cv::equalizeHist(	channels[0],	channels[0]						);	//equalize histogram on the 1st channel (Y)
				cv::merge(			channels,		pass._dst						);	//merge 3 channels including the modified 1st channel into one image
				cv::cvtColor(		pass._dst,		pass._dst,	cv::COLOR_YCrCb2BGR	);	//change the color image from YCrCb to BGR format (to display image properly)
				if( ch_nb == 4 )
				{
					cv::cvtColor(	 pass._dst,		_hist4_dst,	cv::COLOR_BGR2BGRA	);	//change the color image from YCrCb to BGR format (to display image properly)
					//_src_cur = &_hist4_dst;	//todonow
				}
				break;
			}
		}

	end_pass( pass );
}

void	c_bdd_img_opencv::do_bilateral_filter( c_opencv_pass& pass )
{
	//if( _b_bilateral_adaptive_ui )
	//{	//todonow ?
	//	pass._status = aaa::c_cv::STATUS::ERR_UNIMPLEMENTED;
	//	return;
	//}

	begin_pass( pass );

	//	build_dst_index( _bilateral_dst, _bil_fil_dst_index_ui, "Bilateral Filter Destination"  );
		INT32	size_x = (_bilateral_size_ui % 2 ) == 0 ? _bilateral_size_ui + 1 : _bilateral_size_ui ;
#if AAA_LIB_USE_OPENCV_CUDA()
		if( is_gpu_use() )
			cv::cuda::bilateralFilter( *_src_cur_gpu, pass._dst_gpu, size_x, _bilateral_sigma_color_ui, _bilateral_sigma_space_ui );
		else
#endif
			cv::bilateralFilter( *_src_cur, pass._dst, size_x, _bilateral_sigma_color_ui, _bilateral_sigma_space_ui );

	end_pass( pass );
}

//todo separate colored
void	c_bdd_img_opencv::do_denoise( c_opencv_pass& pass )
{
	INT32 ch_nb = get_src_channel();
#if AAA_LIB_USE_OPENCV_CUDA()
	if( is_gpu_use() )
	{
		if( ch_nb > 3 )
			{ pass._status = aaa::c_cv::STATUS::ERR_CH_123_ONLY;	return; }
	}
#endif

	begin_pass( pass );

	//	build_dst_index( _denoise_dst, _denoise_dst_index_ui, "Denoise Destination"  );
		UINT32	template_window = (_denoise_template_ui % 2 ) == 0 ? _denoise_template_ui + 1 : _denoise_template_ui ;
		UINT32	search_window   = (_denoise_windows_ui  % 2 ) == 0 ? _denoise_windows_ui  + 1 : _denoise_windows_ui  ;
#if AAA_LIB_USE_OPENCV_CUDA()
		if( is_gpu_use() )
		{
			if( ch_nb == 3 )
				cv::cuda::fastNlMeansDenoisingColored( *_src_cur_gpu, pass._dst_gpu, _denoise_strenght_ui, float(search_window), template_window );
				//_gpu_denoising.labMethod( _src_cur_gpu, _denoise_gpu_dst, _denoise_strenght_ui, search_window, template_window );
			else
				cv::cuda::fastNlMeansDenoising( *_src_cur_gpu, pass._dst_gpu, _denoise_strenght_ui, search_window, template_window );
				//_gpu_denoising.simpleMethod( _src_cur_gpu, _denoise_gpu_dst, _denoise_strenght_ui, search_window, template_window );
		}
		else
#endif
		{
			if( ch_nb == 3 )
				cv::fastNlMeansDenoisingColored( *_src_cur, pass._dst, _denoise_strenght_ui, float(template_window), search_window );
			else
				cv::fastNlMeansDenoising( *_src_cur, pass._dst, _denoise_strenght_ui, template_window, search_window );
		}

	end_pass( pass );
}


void	c_bdd_img_opencv::do_inpaint_smooth( c_opencv_pass& pass )
{
	if( _b_float )
	{
		pass._status = aaa::c_cv::STATUS::ERR_FLOAT;
		return;	
	}
	INT32 ch_nb = get_src_channel();
	if( ch_nb != 1 && ch_nb != 3 )
	{
		pass._status = aaa::c_cv::STATUS::ERR_CH_13_ONLY;
		return;
	}

//todo
#if false
	begin_pass( pass );

//		build_dst_index( _inpaint_dst.getUMat( cv::ACCESS_RW ), _inpaint_dst_index_ui, "Inpaint Smooth"  );

		//use a smaller version of the image
		cv::resize( *_src_cur, _inpaint_small_dst, cv::Size(), _inpaint_resize_x_ui, _inpaint_resize_y_ui );

		//inpaint only the "unknown" pixels
		cv::inpaint( _inpaint_small_dst, (_inpaint_small_dst == _inpaint_no_depth_ui), _inpainting_dst, _inpaint_radius_ui, cv::INPAINT_TELEA );

		cv::resize( _inpainting_dst, _inpaint_resize_dst, _src_cur->size());
	
		_src_cur->copyTo( pass._dst );
		_inpaint_resize_dst.copyTo( pass._dst, (pass._dst == _inpaint_no_depth_ui) );  //add the original signal back over the inpaint
		//_src_cur = &_inpaint_dst.getUMat( cv::ACCESS_RW );	//todonow

	end_pass( pass );
#else
	pass._status = aaa::c_cv::STATUS::ERR_UNIMPLEMENTED;
#endif

}

void	c_bdd_img_opencv::do_convert( c_opencv_pass& pass )
{
	INT32 ch_nb = get_src_channel();
	if( _convert_type_ui >= 6 )
	{
		if( ch_nb != 1 )
		{
			pass._status = aaa::c_cv::STATUS::ERR_CH_1_ONLY;
			err_print( "%s() needs 1 channel image for Bayer Conversion", __FUNCTION__ );
			return;
		}
	}
	else
	{
		if( ch_nb != 3 &&  ch_nb != 4 )
		{
			pass._status = aaa::c_cv::STATUS::ERR_CH_34_ONLY;
			err_print( "%s() needs 3 or 4 channel image for conversion", __FUNCTION__ );
			return;
		}
	}

	begin_pass( pass );

	//	build_dst_index( _convert_dst, _convert_dst_index_ui, "Convert" );
	
#if AAA_LIB_USE_OPENCV_CUDA()
		if( is_gpu_use() )
		{
			auto format_cv = aaa::c_cv::get_cv_type( aaa::c_pixel_format::make_format_from_channel_type( _src_cur_gpu->channels() ) );
			pass._dst_gpu.create( _src_cur_gpu->size(), format_cv );
			cv::cuda::cvtColor( *_src_cur_gpu, pass._dst_gpu, img_opencv::cv_convert_type[_convert_type_ui] );
		}
		else
#endif
		{
			auto format_cv = aaa::c_cv::get_cv_type( aaa::c_pixel_format::make_format_from_channel_type( _src_cur->channels() ) );
			pass._dst.create( _src_cur->size(), format_cv );
			cv::cvtColor( *_src_cur, pass._dst, img_opencv::cv_convert_type[_convert_type_ui] );
		}

	end_pass( pass );
}


