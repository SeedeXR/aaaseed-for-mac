#include "bdd_img_cv_multitouch.h"
#include "opencv2/video.hpp"
#if AAA_LIB_USE_OPENCV_CUDA()
#	include "opencv2/cudaarithm.hpp"
#	include "opencv2/cudafilters.hpp"
#	include "opencv2/cudaimgproc.hpp"
#	include "opencv2/photo/cuda.hpp"
#	include "opencv2/cudaoptflow.hpp"
#endif

#include "time/aaa_time.h"

#include "image/bind_img_2d.h"
#include "image/img_compo.h"

#undef MIN
#undef MAX

FACTORY_CREATE_PROP_V1( c_bdd_img_cv_multitouch, bdd_img_cv_multitouch, Image CV MultiTouch, bdd_img_cv_multitouch, sub_menu="Image"; );

CONST	INT32	SMOOTH_METHOD_NB = 4;
static	C_PCHAR_C	str_smooth[SMOOTH_METHOD_NB] =
{
//	"BLUR_NO_SCALE",
	"BLUR",
	"GAUSSIAN",
	"MEDIAN",
	"BILATERAL"
};

static	INT32	cv_smooth_method[SMOOTH_METHOD_NB] =
{
//	CV_BLUR_NO_SCALE,
	CV_BLUR,
	CV_GAUSSIAN,
	CV_MEDIAN,
	CV_BILATERAL
};

CONST	INT32	POLY_N_METHOD_NB = 2;
static	C_PCHAR_C	str_poly_n[POLY_N_METHOD_NB] =
{
	"5",
	"7",
};

static	INT32	cv_poly_n[POLY_N_METHOD_NB] =
{
	5,
	7,
};

CONST	INT32	DIFF_METHOD_NB = 6;	
static	C_PCHAR_C	str_diff[DIFF_METHOD_NB] =
{
	//	"BLUR_NO_SCALE",
	"No",
	"Difference absolue",
	"Difference",
	"Difference inverse",
	"Min",
	"Max"
};

namespace n_bdd_img_cv_multitouch
{
	CONSTEXPR INT32 BASE_PARAM_NB			= c_bdd_img::SUPER_PARAM_NB;
	CONSTEXPR INT32 INVERT_PARAM_NB			= 2;
	CONSTEXPR INT32 SMOOTH_PARAM_NB			= 5;
	CONSTEXPR INT32 REMANENCE_PARAM_NB		= 17;
	CONSTEXPR INT32 AGITATION_PARAM_NB		= 3;
	CONSTEXPR INT32 BACKGROUND_PARAM_NB		= 7;
	CONSTEXPR INT32 LAPLACE_PARAM_NB		= 3;
	CONSTEXPR INT32 SOBEL_PARAM_NB			= 5;
	CONSTEXPR INT32 BC_PARAM_NB				= 4;
	CONSTEXPR INT32 DILATE_ERODE_PARAM_NB	= 7;
	CONSTEXPR INT32 DIFF_PARAM_NB			= 2;
	CONSTEXPR INT32 OPTFLOW_PARAM_NB		= 11;
	CONSTEXPR INT32 SMOOTH2_PARAM_NB		= SMOOTH_PARAM_NB;
	CONSTEXPR INT32 HIGHPASS_PARAM_NB		= 10;
	CONSTEXPR INT32 BC_OUT_PARAM_NB			= BC_PARAM_NB;
	CONSTEXPR INT32 RECTIFY_PARAM_NB		= 7;


	CONSTEXPR INT32 GROUP_PARAM_NB			= 15;

	CONSTEXPR INT32 PARAM_NB	=	BASE_PARAM_NB
								+	INVERT_PARAM_NB
								+	SMOOTH_PARAM_NB
								+	REMANENCE_PARAM_NB
								+	AGITATION_PARAM_NB
								+	BACKGROUND_PARAM_NB
								+	LAPLACE_PARAM_NB
								+	SOBEL_PARAM_NB
								+	BC_PARAM_NB
								+	DILATE_ERODE_PARAM_NB
								+	DIFF_PARAM_NB
								+	OPTFLOW_PARAM_NB
								+	SMOOTH2_PARAM_NB
								+	HIGHPASS_PARAM_NB
								+	BC_OUT_PARAM_NB
								+	RECTIFY_PARAM_NB
								+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB] =
	{
		BDD_IMG_BASE_PARAMS

		PARAM_DEF_GROUP_CLOSED( Invert, INVERT_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		invert				)
			PARAM_DEF_IMG_DST(		invert_image_dst	)

		PARAM_DEF_GROUP_CLOSED( Smooth, SMOOTH_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		smooth				)
			PARAM_DEF_IMG_DST(		smooth_image_dst	)
			PARAM_DEF_SYMBO_PSTR(	smooth_method,		1, 3,	str_smooth )
			PARAM_DEF_INT32_XY(		smooth_size,		1, 3,	1, 256 )
	//		PARAM_DEF_BOOL_OFF(		smooth_dst_8bit		)

		PARAM_DEF_GROUP_CLOSED( Remanence, REMANENCE_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			remanence					)
			PARAM_DEF_BOOL_OFF(			remanence_max_maa			)
			PARAM_DEF_IMG_DST(			remanence_image_dst			)
			PARAM_DEF_BOOL_OFF(			remanence_image_ref			)
			PARAM_DEF_BIND_2D_ALONE(	remanence_image_ref_src		)
			PARAM_DEF_BOOL_LOCKED(		remanence_save_trig			)
			PARAM_DEF_REF(				remanence_save_filename		)
			PARAM_DEF_BOOL_OFF(			remanence_restart_trig		)
			PARAM_DEF_BOOL_OFF(			remanence_accumulate_max	)
			PARAM_DEF_BOOL_OFF(			remanence_accumulate_min	)
			PARAM_DEF_BOOL_ON(			remanence_accumulate		)
			PARAM_DEF_REAL_ONE(			remanence_time				)
			//todo PARAM_DEF_BOOL_OFF( remanence_color )
			PARAM_DEF_BOOL_OFF(			remanence_diff				)
			PARAM_DEF_SYMBO_PSTR(		remanence_diff_method,		2, 1,	 str_diff )
			PARAM_DEF_REAL_ZERO(		remanence_diff_min			)
			PARAM_DEF_REAL_ONE(			remanence_diff_factor		)
			PARAM_DEF_INT32_LOCKED(		remanence_max_found			)

		PARAM_DEF_GROUP_CLOSED( Grey and Agitation, AGITATION_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		agitation			)
			PARAM_DEF_REAL_LOCKED(	grey_average		)
			PARAM_DEF_REAL_LOCKED(	agitation_result	)

		PARAM_DEF_GROUP_CLOSED( Background, BACKGROUND_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		background					)
			PARAM_DEF_IMG_DST(		background_image_dst		)
			PARAM_DEF_BOOL_OFF(		background_reset			)
			PARAM_DEF_BOOL_OFF(		background_absolute			)
			PARAM_DEF_BOOL_OFF(		background_divide			)
			PARAM_DEF_REAL_ONE(		background_divide_add		)
			PARAM_DEF_REAL_ONE(		background_divide_factor	)

		PARAM_DEF_GROUP_CLOSED( Laplace, LAPLACE_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		laplace				)
			PARAM_DEF_IMG_DST(		laplace_image_dst	)
			PARAM_DEF_INT32(		laplace_size,		5, 3,	1, 256 )

		PARAM_DEF_GROUP_CLOSED( Sobel, SOBEL_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		sobel				)
			PARAM_DEF_IMG_DST(		sobel_image_dst		)
			PARAM_DEF_INT32(		sobel_dx,			2, 1,	0, 256 )
			PARAM_DEF_INT32(		sobel_dy,			2, 1,	0, 256 )
			PARAM_DEF_INT32(		sobel_aperture,		2, 1,	-1, 7 )

		PARAM_DEF_GROUP_CLOSED( Brightness Contrast, BC_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		bc				)
			PARAM_DEF_IMG_DST(		bc_image_dst	)
			PARAM_DEF_REAL_ZERO(	bc_min			)
			PARAM_DEF_REAL_ONE(		bc_max			)

		PARAM_DEF_GROUP_CLOSED( Diff, DIFF_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		diff				)
			PARAM_DEF_SYMBO_PSTR(	diff_method,		2, 1,	 str_diff )

		PARAM_DEF_GROUP_CLOSED( Dilate Erode, DILATE_ERODE_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		erode_first			)
			PARAM_DEF_BOOL_OFF(		dilate				)
			PARAM_DEF_IMG_DST(		dilate_image_dst	)
			PARAM_DEF_INT32(		dilate_pass,		2, 1,	1, 256 )
			PARAM_DEF_BOOL_OFF(		erode				)
			PARAM_DEF_IMG_DST(		erode_image_dst		)
			PARAM_DEF_INT32(		erode_pass,			2, 1,	1, 256 )

		PARAM_DEF_GROUP_CLOSED( OpticalFlow, OPTFLOW_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		optical_flow				)
			PARAM_DEF_IMG_DST(		optical_flow_image_dst		)
			PARAM_DEF_REAL(			optical_flow_pyr_scale,		0.02,	0.5,	0.01,	0.99 )
			PARAM_DEF_INT32(		optical_flow_levels,		3,		5,		1,		8 )
			PARAM_DEF_INT32(		optical_flow_winsize,		1,		13,		1,		255 )
			PARAM_DEF_INT32(		optical_flow_iters,			1,		10,		1,		16 )
			PARAM_DEF_SYMBO_PSTR(	optical_flow_poly_n,		7,		5,		str_poly_n )
			PARAM_DEF_REAL(			optical_flow_poly_sigma,	1.2,	1.1,	1.01,	1.99 )
			PARAM_DEF_BOOL_OFF(		optical_flow_initial_flow	)
			PARAM_DEF_BOOL_OFF(		optical_flow_gaussian		)
			PARAM_DEF_BOOL_OFF(		optical_flow_convert		)

		PARAM_DEF_GROUP_CLOSED( Smooth2, SMOOTH2_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		smooth2				)
			PARAM_DEF_IMG_DST(		smooth2_image_dst	)
			PARAM_DEF_SYMBO_PSTR(	smooth2_method,		1, 2,	str_smooth )
			PARAM_DEF_INT32_XY(		smooth2_size,		1, 3,	1, 256 )
	//		PARAM_DEF_BOOL_OFF(		smooth2_dst_8bit		)

		PARAM_DEF_GROUP_CLOSED( Highpass, HIGHPASS_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			highpass			)
			PARAM_DEF_IMG_DST(			highpass_image_dst	)
			PARAM_DEF_BOOL_OFF(			highpass_simple		)
			PARAM_DEF_SYMBO_PSTR(		highpass_simple_smooth_method,	1, 2,	str_smooth )
			PARAM_DEF_INT32_POS_ZERO(	highpass_simple_smooth_size	)
			PARAM_DEF_SYMBO_PSTR(		highpass_simple_noise_method,	1, 2,	str_smooth )
			PARAM_DEF_INT32_POS_ZERO(	highpass_simple_noise_size	)
			PARAM_DEF_BOOL_OFF(			highpass_erode_dilate		)
			PARAM_DEF_INT32_POS_ZERO(	highpass_filter				)
			PARAM_DEF_INT32_POS_ZERO(	highpass_scale				)

		PARAM_DEF_GROUP_CLOSED( Brightness Contrast Out, BC_OUT_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		bc_out				)
			PARAM_DEF_IMG_DST(		bc_out_image_dst	)
			PARAM_DEF_REAL_ZERO(	bc_out_min			)
			PARAM_DEF_REAL_ONE(		bc_out_max			)

		PARAM_DEF_GROUP_CLOSED( Rectify, RECTIFY_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			rectify				)
			PARAM_DEF_IMG_DST(			rectify_image_dst	)
			PARAM_DEF_BOOL_OFF(			rectify_auto_trig	)
			PARAM_DEF_REAL_POS_ZERO(	rectify_level		)
			PARAM_DEF_BOOL_OFF(			rectify_max			)
			PARAM_DEF_REAL_POS_ONE(		rectify_max_level	)
			PARAM_DEF_BOOL_OFF(			rectify_inverse		)
	};
}


void	c_bdd_img_cv_multitouch::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start_src_resize( false, false );

	get_param(h)->set_comment_bool( _b_invert_active );
	++h;
		param_set_pt( h, _b_invert_active );
		param_set_pt( h, _invert_image_dst );

	get_param(h)->set_comment_bool( _b_smooth_active );
	++h;
		param_set_pt( h, _b_smooth_active );
		param_set_pt( h, _smooth_image_dst );
		param_set_pt( h, _s_smooth );
		param_set_pt( h, _smooth_size_x );
		param_set_pt( h, _smooth_size_y );
//		param_set_pt( h, _b_smooth_8bit );

	get_param(h)->set_comment_bool( _b_remanence_active );
	++h;
		param_set_pt( h, _b_remanence_active );
		param_set_pt( h, _b_remanence_max_maa );
		param_set_pt( h, _remanence_image_dst );
		param_set_pt( h, _b_remanence_ref_in_bind );
		param_set_pt( h, _remanence_ref_bind );

		param_set_pt( h, _b_remanence_save_trig );
		param_set_pt( h, _remanence_filename_ui );
		param_set_pt( h, _b_remanence_restart_trig_ui );
		param_set_pt( h, _b_remanence_accumulate_max );
		param_set_pt( h, _b_remanence_accumulate_min );
		param_set_pt( h, _b_remanence_accumulate );
		param_set_pt( h, _remanence_time );
//todo doit	param_set_pt( h, _b_remanence_color );
		param_set_pt( h, _b_remanence_diff );
		param_set_pt( h, _s_remanence_diff_method );
		param_set_pt( h, _remanence_diff_min );
		param_set_pt( h, _remanence_diff_factor );
		param_set_pt( h, _remanence_max_found );

	get_param(h)->set_comment_bool( _b_agitation_active );
	++h;
		param_set_pt( h, _b_agitation_active );
		param_set_pt( h, _grey_average_out );
		param_set_pt( h, _agitation_out );

	get_param(h)->set_comment_bool( _b_background_active );
	++h;
		param_set_pt( h, _b_background_active );
		param_set_pt( h, _background_image_dst );
		param_set_pt( h, _b_background_reset_trig );
		param_set_pt( h, _b_background_absolute );	
		param_set_pt( h, _b_background_divide );
		param_set_pt( h, _background_divide_add );
		param_set_pt( h, _background_divide_factor );

	get_param(h)->set_comment_bool( _b_laplace_active );
	++h;
		param_set_pt( h, _b_laplace_active );
		param_set_pt( h, _laplace_image_dst );
		param_set_pt( h, _laplace_size);

	get_param(h)->set_comment_bool( _b_sobel_active );
	++h;
		param_set_pt( h, _b_sobel_active );
		param_set_pt( h, _sobel_image_dst );
		param_set_pt( h, _sobel_dx );
		param_set_pt( h, _sobel_dy );
		param_set_pt( h, _sobel_aperture );

	get_param(h)->set_comment_bool( _b_bc_active );
	++h;
		param_set_pt( h, _b_bc_active );
		param_set_pt( h, _bc_image_dst );
		param_set_pt( h, _bc_min_ui );
		param_set_pt( h, _bc_max_ui );

	get_param(h)->set_comment_bool( _b_diff_active );
	++h;
		param_set_pt( h, _b_diff_active );
		param_set_pt( h, _s_diff_method );
		get_param(h)->set_comment_bool( _b_dilate_active||_b_erode_active );

	++h;
		param_set_pt( h, _b_erode_first );
		param_set_pt( h, _b_dilate_active );
		param_set_pt( h, _dilate_image_dst );
		param_set_pt( h, _dilate_size );
		param_set_pt( h, _b_erode_active );
		param_set_pt( h, _erode_image_dst );
		param_set_pt( h, _erode_size );
	
	get_param(h)->set_comment_bool( _b_optflow_active );
	++h;
		param_set_pt( h, _b_optflow_active			);
		param_set_pt( h, _optflow_image_dst			);
		param_set_pt( h, _optflow_pyr_scale			);
		param_set_pt( h, _optflow_levels			);
		param_set_pt( h, _optflow_winsize			);
		param_set_pt( h, _optflow_iters				);
		param_set_pt( h, _optflow_poly_n_ui			);
		param_set_pt( h, _optflow_poly_sigma		);
		param_set_pt( h, _b_optflow_initial_flow_ui	);
		param_set_pt( h, _b_optflow_gaussian		);
		param_set_pt( h, _b_optflow_convert			);
		
		get_param(h)->set_comment_bool( _b_smooth2_active );
	++h;
		param_set_pt( h, _b_smooth2_active );
		param_set_pt( h, _smooth2_image_dst );
		param_set_pt( h, _s_smooth2 );
		param_set_pt( h, _smooth2_size_x );
		param_set_pt( h, _smooth2_size_y );
//		param_set_pt( h, _b_smooth2_8bit );

	get_param(h)->set_comment_bool( _b_highpass_active );
	++h;
		param_set_pt( h, _b_highpass_active );
		param_set_pt( h, _highpass_image_dst );
		param_set_pt( h, _b_simple_highpass );
		param_set_pt( h, _simple_highpass_smooth_method );
		param_set_pt( h, _simple_highpass_smooth_size );
		param_set_pt( h, _simple_highpass_noise_method );
		param_set_pt( h, _simple_highpass_noise_size );
		param_set_pt( h, _b_highpass_erode_dilate );
		param_set_pt( h, _highpass_filter );
		param_set_pt( h, _highpass_scale );

	get_param(h)->set_comment_bool( _b_bc_out_active );
	++h;
		param_set_pt( h, _b_bc_out_active );
		param_set_pt( h, _bc_out_image_dst );
		param_set_pt( h, _bc_out_min_ui );
		param_set_pt( h, _bc_out_max_ui );

	get_param(h)->set_comment_bool( _b_rectify_active );
	++h;
		param_set_pt( h, _b_rectify_active );
		param_set_pt( h, _rectify_image_dst );
		param_set_pt( h, _b_rectify_auto_trig );
		param_set_pt( h, _rectify_level );
		param_set_pt( h, _b_rectify_to_max );
		param_set_pt( h, _rectify_max_level );
		param_set_pt( h, _b_rectify_inverse );

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bdd_img_cv_multitouch )
	,_remanence_img_last					( nullptr )
	,_remanence_img_last_last				( nullptr )
	,_agitation_img_last					( nullptr )
	,_agitation_out							( 0. )
	,_grey_average_out						( 0. )
	//,_remanence_img_tick					( nullptr )
	,_remanence_max_found					( 0 )
	//,_back_nPolyMask(nullptr)
{
	param_init_with( n_bdd_img_cv_multitouch::param, n_bdd_img_cv_multitouch::PARAM_NB );
	force_mono();
}

void	c_bdd_img_cv_multitouch::dealloc()
{
	IF_THIS_NULL_RETURN();

	_invert_dst.release();
	_smooth_dst.release();
	_smooth2_dst.release();
	_dilate_dst.release();
	_erode_dst.release();
	_img_prev.release();
	_optflow_dst.release();
	_remanence_dst.release();
	_remanence_data.release();
	_remanence_buf.release();
	_remanence_ref.release();
	_remanence_img_ref.release();
//todo add the other release
#if AAA_LIB_USE_OPENCV_CUDA()
	_remanence_gpu_buf.release();
	_remanence_gpu_dst.release();
#endif

	_back_dst.release();
	_hp_dst.release();
	_rectify_dst.release();
	_bc_dst.release();
	_bc_out_dst.release();
	_bc_lutmat.release();
	_laplace_dst.release();
	_laplace_float.release();
	_sobel_dst.release();
	_hp_buffer.release();
	_hp_outra.release();
	_hp_outra2.release();
	_back_ref.release();
	_back_for_div.release();
	_b_background_reset_trig = true;

	//SAFE_DELETE_ARRAY(_back_polyMask);

	_hp_element.release();
	_hp_element2.release();
	SAFE_DELETE( _remanence_img_last );
	SAFE_DELETE( _remanence_img_last_last );
	//SAFE_DELETE( _remanence_img_write );
//	SAFE_DELETE( _remanence_img_tick );
	SAFE_DELETE( _agitation_img_last );
}

c_bdd_img_cv_multitouch::~c_bdd_img_cv_multitouch()
{
	dealloc();
}

//todo process in draw ?
void	c_bdd_img_cv_multitouch::update()
{
	bool b_change_it;
	if( !update_mat_src( b_change_it ) )
		return;
	if( is_size_changed() )
		dealloc();
	if( is_content_changed() )
		do_process();
}

void	c_bdd_img_cv_multitouch::draw()
{
}

FINLINE	c_img_ipl*	c_bdd_img_cv_multitouch::become_cur( c_img_ipl* src, INT32 index ) 
{
	src->set_changed();
	validate_dst_change( index );
	return src;
}

void	c_bdd_img_cv_multitouch::do_process()
{
	try
	{
		if( _b_invert_active	)
			do_invert(); // OK
		if( _b_smooth_active	)
#if AAA_LIB_USE_OPENCV_CUDA()
			do_smooth( _s_smooth, _smooth_size_x, _smooth_size_y, &_smooth_status, &_smooth_dst, &_smooth_gpu_dst, _smooth_image_dst ); // OK
#else
			do_smooth( _s_smooth, _smooth_size_x, _smooth_size_y, &_smooth_status, &_smooth_dst, _smooth_image_dst ); // OK
#endif

		if( _b_remanence_active )
			do_remanence(); // Almost OK see img_ref and max maa apparently using image_ref doesn't work
		if( _b_agitation_active )
			do_agitation(); // OK
		if( _b_background_active )
			do_background(); // OK
		if( _b_laplace_active )
			do_laplace();	 // OK
		if( _b_sobel_active )
			do_sobel();		 // OK
		if( _b_diff_active )
			do_diff();
		if( _b_erode_active && _b_erode_first )
			do_erode();		 // OK
		if( _b_dilate_active )
			do_dilate();	 // OK
		if( _b_erode_active && !_b_erode_first )
			do_erode();		 // OK
		if( _b_optflow_active )
			do_optical_flow(); // experimental

		if( _b_bc_active )
			do_bc( &_bc_dst, _bc_min_ui, _bc_max_ui ); // OK

		if( _b_smooth2_active	)	
#if AAA_LIB_USE_OPENCV_CUDA()
			do_smooth( _s_smooth2, _smooth2_size_x, _smooth2_size_y, &_smooth2_status, &_smooth2_dst, &_smooth2_gpu_dst, _smooth2_image_dst ); // OK
#else
			do_smooth( _s_smooth2, _smooth2_size_x, _smooth2_size_y, &_smooth2_status, &_smooth2_dst, _smooth2_image_dst ); // OK
#endif

#if AAA_LIB_USE_OPENCV_CUDA()																													  //because the two last stage use only cpu
		if( is_gpu_use() && !_src_cur_gpu->empty() && _b_bc_out_active )
		{
			_src_cur_gpu->download( *_src_cur );		
			_b_src_is_gpu	= false;
		}
#endif

		if( _b_highpass_active )
			do_highpass(); // Almost OK see how to best adapt blur params from old cvSmooth to new separate functions (gaussianblur, blur...)
		if( _b_bc_out_active )
			do_bc( &_bc_out_dst, _bc_out_min_ui, _bc_out_max_ui ); // OK
		if( _b_rectify_active )
			do_rectify(); // OK

		/* old call to highpass
		// highpass filter
		if( _b_highpass_active )
		{
			if( build_dst_image( _ipl_hp, _highpass_image_dst, *img_cur, "Highpass 1 image" ) )
			{
				ipl::create(		_hp_buffer,	img_cur->get_ipl(),					"Highpass 2 image" );
				ipl::create_gray(	_hp_outra,	img_cur->get_ipl(),	IPL_DEPTH_16S,	"Highpass 3 image" );
				ipl::create_gray(	_hp_outra2,	img_cur->get_ipl(),	IPL_DEPTH_16S,	"Highpass 4 image" );

				do_highpass( img_cur, &_ipl_hp );
				img_cur = become_cur( &_ipl_hp, _highpass_image_dst );
			}
		}
		*/
		
		//todo sometimes we already did it
		update_dst_index( _dst_img_index_ui, "bdd_img_cv_multitouch", false, true );
	}
	catch( cv::Exception& e )
	{
		ERR_PRINT_STRING( "Open CV Exception : %s", e.what() );
	}
}

void	c_bdd_img_cv_multitouch::do_invert()
{
#if AAA_LIB_USE_OPENCV_CUDA()
	if( is_gpu_use() )
	{
		if( _b_float )
		{
			//_invert_gpu_dst.create( _src_cur_gpu->size(), _src_cur_gpu->type() );
			cv::cuda::subtract( cv::Scalar(1,1,1), *_src_cur_gpu, _invert_gpu_dst );
		}
		else
			cv::cuda::bitwise_not( *_src_cur_gpu, _invert_gpu_dst );
		_src_cur_gpu = &_invert_gpu_dst;
	}
	else
#endif
	{
		if ( _b_float )
			cv::subtract( 1.0f, *_src_cur, _invert_dst );
		else
			cv::bitwise_not( *_src_cur, _invert_dst );
		_src_cur = &_invert_dst;
	}
	DBG_PRINT_STRING( "_src_cur depth, channels : %d, %d \n", _src_cur->depth(), _src_cur->channels() );
	update_dst_index( _invert_image_dst, "Invert Destination" );
}

#if AAA_LIB_USE_OPENCV_CUDA()
void	c_bdd_img_cv_multitouch::do_smooth( INT32 s_smooth, INT32 sx, INT32 sy, aaa::c_cv::STATUS* status, cv::UMat* dst, cv::cuda::GpuMat* gpu_dst, INT32 image_dst )
#else
void	c_bdd_img_cv_multitouch::do_smooth( INT32 s_smooth, INT32 sx, INT32 sy, aaa::c_cv::STATUS* status, cv::UMat* dst, INT32 image_dst )
#endif
{
#if AAA_LIB_USE_OPENCV_CUDA()
	auto src_type = is_gpu_use() ? _src_cur_gpu->type() : _src_cur->type();
#else
	auto src_type = _src_cur->type();
#endif
	auto dst_type = src_type;
//	INT32 dst_type = _b_smooth_8bit ? CV_8UC1 : _src_cur_gpu->type();	
	if( cv_smooth_method[s_smooth] == CV_GAUSSIAN )
	{	
		if( (sx & 1)== 0 )
			sx += 1;
		if( (sy & 1)== 0 )	
			sy += 1;
#if AAA_LIB_USE_OPENCV_CUDA()
		if( is_gpu_use() )
		{
			if( sx > 31 )
			{
				err_print( "%s() sx : %d can't be more than 31 in gpu mode ", __FUNCTION__, sx );
				sx = 31;
			}
			if( sy > 31 )
			{
				err_print( "%s() sy : %d can't be more than 31 in gpu mode ", __FUNCTION__, sy );
				sy = 31;
			}
			DOUBLE sig_x = 0.3*((sx-1)*0.5 - 1) + 0.8;
			DOUBLE sig_y = 0.3*((sy-1)*0.5 - 1) + 0.8;
			cv::Ptr<cv::cuda::Filter> openFilter = cv::cuda::createGaussianFilter( src_type, dst_type, cv::Size( sx, sy ), sig_x, sig_y );
			openFilter->apply( *_src_cur_gpu, *gpu_dst );
//			cv::cuda::GaussianBlur( _src_cur_gpu, _blur_gpu_dst, cv::Size( sx, sy ), sig_x, sig_y );
			//cv::cuda::blur( _scr_cur_gpu, _blur_gpumat, size );
			_src_cur_gpu = gpu_dst;
		}
		else
#endif
		{

			DOUBLE sig_x = 0.3*((sx-1)*0.5 - 1) + 0.8;
			DOUBLE sig_y = 0.3*((sy-1)*0.5 - 1) + 0.8;
			cv::GaussianBlur( *_src_cur, *dst, cv::Size( sx, sy ), sig_x, sig_y );
			_src_cur = dst;
		}
	}
	else
	{
		if( _b_float )
		{
			*status = aaa::c_cv::STATUS::ERR_FLOAT;
			return;
		}
#if AAA_LIB_USE_OPENCV_CUDA()
		if( is_gpu_use() )
		{
			if( _src_cur_gpu->channels() != 1 && _src_cur_gpu->channels() != 4 )
			{
				*status = aaa::c_cv::STATUS::ERR_CH_14_ONLY;
				return;
			}
		}
#endif
		//else					if ( _src_cur->channels() != 1 || _src_cur->channels() != 4 )			{ return; }

		cv::Size size( sx, sy );
#if AAA_LIB_USE_OPENCV_CUDA()
		if( is_gpu_use() )
		{
			cv::Ptr<cv::cuda::Filter> openFilter = cv::cuda::createBoxFilter( src_type, dst_type, size );
			openFilter->apply( *_src_cur_gpu, *gpu_dst );
			//cv::cuda::boxFilter( _src_cur_gpu, _blur_gpu_dst, -1, size );
			_src_cur_gpu = gpu_dst;
		}
		else
#endif
		{
			cv::blur( *_src_cur, *dst, size );
			_src_cur = dst;
		}
		
	}
	*status = aaa::c_cv::STATUS::DONE;
	update_dst_index( image_dst, "Smooth image" );
}
// stores a single previous image for now
// todo extend to n images
// todo consider modifying image bind system to store n prev and n next images ( ex: video stream )
void	c_bdd_img_cv_multitouch::store_img_prev(  )
{
	//cv::UMat	tmp;
	//size_t		n = _imgs_prev.size();
	
	_time_got_frame_last_imgs = _time_got_frame_imgs;
	_time_got_frame_imgs = REAL(aaa::time::get_real_time());
#if AAA_LIB_USE_OPENCV_CUDA()
	if( is_gpu_use() )
	{
		_src_cur_gpu->copyTo( _img_prev_gpu );
	}
	else
#endif
	{
		_src_cur->copyTo( _img_prev );
	}
	//for( int i = 0; i < n ; i++ )
	//{
	//	_imgs_prev[_imgs_prev.begin()];
	//}
}

void	c_bdd_img_cv_multitouch::do_optical_flow()
{
//#if AAA_LIB_USE_OPENCV_CUDA()
//	if ( is_gpu_use() )		if ( _src_cur_gpu->channels() != 1 && _src_cur_gpu->channels() != 4 )	{	_optflow_status = aaa::c_cv::STATUS::ERR_1_4_CH;	return;		}
//#endif
	int flags = _b_optflow_initial_flow_ui ? cv::OPTFLOW_USE_INITIAL_FLOW : 0;
	if( _b_optflow_gaussian )
		flags |= cv::OPTFLOW_FARNEBACK_GAUSSIAN;
#if AAA_LIB_USE_OPENCV_CUDA()
	if( is_gpu_use() )
	{
		if( _img_prev_gpu.empty() )
			store_img_prev();
		if( _optflow_gpu_dst.size().width != _src_cur_gpu->size().width || _optflow_gpu_dst.size().height != _src_cur_gpu->size().height )
		{
			_optflow_gpu_dst.release();
			_optflow_gpu_dst.create( _src_cur_gpu->size(), CV_32FC2 );
			store_img_prev();
		}
		cv::Ptr<cv::cuda::FarnebackOpticalFlow> flow = flow->create( _optflow_levels, _optflow_pyr_scale, false, _optflow_winsize, _optflow_iters,
																cv_poly_n[_optflow_poly_n_ui], _optflow_poly_sigma, flags );
		flow->calc( _img_prev_gpu, *_src_cur_gpu, _optflow_gpu_dst );
		store_img_prev();
		if( _b_optflow_convert )
		{
			cv::cuda::GpuMat flow_parts[2];
			cv::cuda::split( _optflow_gpu_dst, flow_parts );
			cv::cuda::GpuMat magnitude, angle, magn_norm;
			cv::cuda::cartToPolar( flow_parts[0], flow_parts[1], magnitude, angle, true );
			cv::cuda::normalize( magnitude, magn_norm, 0.0f, 1.0f, cv::NORM_MINMAX, -1 );
			cv::cuda::multiply( angle, ( ( 1.f / 360.f ) * ( 180.f / 255.f ) ), angle );

			//build hsv image
			cv::cuda::GpuMat hsvs[3], hsv, hsv8;
			hsvs[0] = angle;
			hsvs[1] = cv::cuda::GpuMat( angle.size(), CV_32F );
			hsvs[2] = magn_norm;
			hsvs[1].setTo( 1. );
			cv::cuda::merge( hsvs, 3, hsv );
			hsv.convertTo( hsv8, CV_8U, 255.0 );
			cv::cuda::cvtColor( hsv8, _optflow_gpu_dst, cv::COLOR_HSV2BGR );
		}
//		*_src_cur_gpu = bgr;//.getUMat( cv::ACCESS_RW );
		_src_cur_gpu = &_optflow_gpu_dst;//.getUMat( cv::ACCESS_RW );
	}
	else
#endif
	{
		if( _img_prev.empty() )
			store_img_prev();
		if( _optflow_dst.size().width != _src_cur->size().width || _optflow_dst.size().height != _src_cur->size().height )
		{
			_optflow_dst.release();
			_optflow_dst.create(_src_cur->size(), CV_32FC2);
			store_img_prev();
		}
		cv::calcOpticalFlowFarneback( _img_prev, *_src_cur, _optflow_dst, _optflow_pyr_scale, _optflow_levels,
										_optflow_winsize, _optflow_iters,
										cv_poly_n[_optflow_poly_n_ui], _optflow_poly_sigma, flags );
		cv::Mat flow_parts[2];
		store_img_prev();
		if( _b_optflow_convert )
		{
			cv::split( _optflow_dst.getMat( cv::ACCESS_RW ), flow_parts );
			cv::Mat magnitude, angle, magn_norm;
			cv::cartToPolar( flow_parts[0], flow_parts[1], magnitude, angle, true );
			cv::normalize( magnitude, magn_norm, 0.0f, 1.0f, cv::NORM_MINMAX );
			angle *= ( ( 1.f / 360.f ) * ( 180.f / 255.f ) );

			//build hsv image
			cv::Mat hsvs[3], hsv, hsv8;
			hsvs[0] = angle;
			hsvs[1] = cv::Mat::ones( angle.size(), CV_32F );
			hsvs[2] = magn_norm;
			cv::merge( hsvs, 3, hsv );
			hsv.convertTo( hsv8, CV_8U, 255.0 );
			cv::cvtColor( hsv8, _optflow_dst, cv::COLOR_HSV2BGR );
		}

		//_src_cur = &bgr.getUMat( cv::ACCESS_RW );
		_src_cur = &_optflow_dst;
	}
	update_dst_index( _optflow_image_dst, "Optflow image" );
}

void	c_bdd_img_cv_multitouch::do_remanence()
{
	if( _b_remanence_max_maa )
		do_remanence_max_maa();
	else
	{
		_time_got_frame_last = _time_got_frame;
		_time_got_frame = REAL(aaa::time::get_real_time());	//todo time not this way here
		do_remanence_base();
	}
	_src_cur = &_remanence_dst;
	update_dst_index( _remanence_image_dst, "Remanence image" );
}

//todo fucked up by Jean
/*
c_img_2d*	cv_umat_to_img( cv::UMat remanence_buf, c_img_2d* img, bool b_bgr )
{
	aaa::PIXEL_FORMAT	format = aaa::c_cv::get_pixel_format_from_cv( remanence_buf.type(), b_bgr );
	if( format != aaa::PIXEL_FORMAT::UNKNOWN )
	{
		if( !img )
			img = img->create( "Remanence image write" );
		img->copy_from_src( remanence_buf.u->data, (INT32)remanence_buf.step, format, format, remanence_buf.cols, remanence_buf.rows, false );
		return img;
	}
	else
		return nullptr;
}

void	c_bdd_img_cv_multitouch::do_save_remanence( C_PCHAR_C filename )
{
	//save remanence
	if( !_remanence_buf.empty() )
	{
		//c_img_2d*	img = ipl::to_aaa( _remanence_buf, _remanence_ref_bind );
		bool b_bgr = false; // is it really mono here ?
		c_img_2d* img = nullptr;
		cv_umat_to_img( _remanence_buf, img, b_bgr );
		if( img )
		{
			img->write( filename, c_img_utils::FILE_TYPE::PNG );
		}
		//jean se passe quoi pour le img created in cv_umat_to_img()
	}
}
*/

void	c_bdd_img_cv_multitouch::do_remanence_max_maa( )
{
	//	means the remanence_ref should be associated with the bind number 
	if( !_b_remanence_ref_in_bind )
		return;
	//	get the img from the image bind
	c_img_2d*	img	= g_bind_img_2d->get_always( _remanence_ref_bind );
	if( !img )
	{
		err_print( "%s() : can't get an image at %d", __FUNCTION__, _remanence_ref_bind );
		return;
	}
	if( !_remanence_img_last )
		_remanence_img_last			= c_img_2d::create( __FUNCTION__ );
	if( !_remanence_img_last_last )
		_remanence_img_last_last	= c_img_2d::create( __FUNCTION__ );
//	if( _remanence_accumulate_tick && !_remanence_img_tick )
//		_remanence_img_tick = c_img_2d::create( __FUNCTION__ );
	
	INT32 size_x	=	_src_cur->size().width;
	INT32 size_y	=	_src_cur->size().height;
	INT32 ch_nb		=	_src_cur->channels();

	//todo perhaps we need more precision than i8
	if( !img->is_size_format( size_x,size_y, aaa::PIXEL_FORMAT::R_8 ) )
	{
		GOOD_PRINT_STRING( "Max Maa creating image ref in bind %d", _remanence_ref_bind );
		_b_remanence_restart_trig_ui = true;
		img->init_with_size( size_x,size_y, aaa::PIXEL_FORMAT::R_8, __FUNCTION__ );
		img->set_cpu_keep( true );			//	we don't want to check all the time or reload
//		img->reset_time_modification();		//hack when flag free_when_loaded_in_gpu is active this should force needed reloading
	}
	if( !_remanence_img_last->is_size_format( size_x,size_y, aaa::PIXEL_FORMAT::R_8 ) )
	{
		_remanence_img_last->init_with_size( size_x,size_y, aaa::PIXEL_FORMAT::R_8, __FUNCTION__ );
		_remanence_img_last->set_cpu_keep( true );			//	we don't want to check all the time or reload
		_remanence_img_last->fill_compo( 0, 0. );
	}
	if( !_remanence_img_last_last->is_size_format( size_x,size_y, aaa::PIXEL_FORMAT::R_8 ) )
	{
		_remanence_img_last_last->init_with_size( size_x,size_y, aaa::PIXEL_FORMAT::R_8, __FUNCTION__ );
		_remanence_img_last_last->set_cpu_keep( true );			//	we don't want to check all the time or reload
		_remanence_img_last_last->fill_compo( 0, 0. );
	}
/*
	if( _remanence_accumulate_tick && !_remanence_img_tick->is_size_format( size_x,size_y, aaa::PIXEL_FORMAT::RGBA_8 ) )
	{
		_remanence_img_tick->init_with_size( size_x,size_y, aaa::PIXEL_FORMAT::RGBA_8, __FUNCTION__ );
		_remanence_img_tick->set_data_keep( true );			//	we don't want to check all the time or reload
		_remanence_img_tick->fill_rgba( 0., 0., 0., 0. );
	}
*/	
	if( _b_remanence_restart_trig_ui )
	{
		img->fill_compo( 0, 0. );
		_b_remanence_restart_trig_ui = false;
	}

	if( ch_nb != 1 )
	{
		err_print( "%s() : not implemented yet for %d channel", __FUNCTION__,ch_nb );
		return;
	}

	_remanence_max_found = 0;

	if( _remanence_dst.empty() )//getMat( cv::ACCESS_READ ).empty() )
	{
		ERR_PRINT_STRING( "%s() Null img dst ", __FUNCTION__ );
	}
	else
	{
		UINT8*	p_ref	=	img->get_data_uint8();
		UINT8*	p_src	=	_src_cur->getMat(cv::ACCESS_READ ).ptr(0); //src->get_data_pt_hack();
		UINT8*	p_dst	=	_remanence_dst.getMat( cv::ACCESS_READ ).ptr(0); //dst->get_data_pt_hack();

		INT32	i32		=	size_x * size_y;

		if( !p_ref || !p_src || !p_dst )
		{
			ERR_PRINT_STRING( "%s() Null img data ", __FUNCTION__ );
		}
		else
		{
			--p_ref;
			--p_src;
			--p_dst;

			if( _b_remanence_accumulate_max )
			{
				do
				{	
					INT32 v = INT32(*++p_src) - INT32(*++p_ref);
					if( v > 0 )
					{
						*p_ref = *p_src;
						*++p_dst = 255;
						_remanence_max_found += v;
					}
					else
						*++p_dst = 0;
				}
				while( --i32 );
				img->set_changed();
			}
			else if( _b_remanence_accumulate_min )
			{
				do
				{	
					INT32 v = INT32(*++p_src) - INT32(*++p_ref);
					if( v < 0 )
					{
						*p_ref = *p_src;
						*++p_dst = 255;
						_remanence_max_found -= v;
					}
					else
						*++p_dst = 0;
				}
				while( --i32 );
				img->set_changed();
			}
			else
			{
				UINT8*	p_prev	=	_remanence_img_last->get_data_uint8();
				UINT8*	p_prev2	=	_remanence_img_last_last->get_data_uint8();

				if( !p_prev || !p_prev2 )
					ERR_PRINT_STRING( "%s() Null img data ", __FUNCTION__ );
				else
				{
					--p_prev;
					--p_prev2;

					INT32	limit	=	CLAMP( INT32( _remanence_diff_min * aaa::img::REAL_NEARLY_256 ), 0, 255 );
					do
					{	
						INT32 ref	=	*++p_ref;
						INT32 cur	=	*++p_src;
						INT32 prev	=	*++p_prev;
						++p_prev2;
						*++p_dst = 0;
						if( (cur - ref) > limit )
						{
							if( (prev - ref) > limit )
							{
								if( (INT32(*p_prev2) - ref) > limit )
									*p_dst = 255;			
							}
						}
						*p_prev2	=	prev;
						*p_prev		=	cur;
					}
					while( --i32 );
				}
			}
		}
	}
}

bool	my_create_gray( cv::UMat* ref, cv::UMat* dst )
{
	if( dst )
	{
		if(		dst->size().width == ref->size().width
			&&	dst->size().height == ref->size().height
			&&	dst->depth() == ref->depth()
			&&	dst->channels() == ref->channels()
			)
			return true;

		dst->release();
	}
	if( !dst )
		return false;
	else if( dst->empty() )
	{
		dst->create( ref->size(), ref->type() ); // size = lol !!!
		//dst->origin = ref->origin;  // same vertical flip as source
		//create_print( dst, mess );
		return true;
	}
	return false;
}

//cv::Mat tmp;
//cv::Mat tmp1;
//cv::Mat tmp2;

void	c_bdd_img_cv_multitouch::do_diff()
{

#if AAA_LIB_USE_OPENCV_CUDA()
	if( is_gpu_use() )
	{
		if( _remanence_gpu_buf.empty() || _remanence_gpu_buf.size().width != _src_cur->size().width || _remanence_gpu_buf.size().height != _src_cur->size().height )
			_src_cur_gpu->copyTo( _remanence_gpu_buf );

		switch( _s_diff_method )
		{
		case 0:	_src_cur_gpu->copyTo(											_remanence_gpu_dst );	break;	// we could avoid the copy	
		case 1:	cv::cuda::absdiff(		*_src_cur_gpu,		_remanence_gpu_buf,	_remanence_gpu_dst );	break;	//Difference absolue
		case 2:	cv::cuda::subtract(		_remanence_gpu_buf, *_src_cur,			_remanence_gpu_dst );	break;	//Difference
		case 3:	cv::cuda::subtract(		*_src_cur_gpu,		_remanence_gpu_buf,	_remanence_gpu_dst );	break;	//Difference inverse"
		case 4:	cv::cuda::min(			*_src_cur_gpu,		_remanence_gpu_buf,	_remanence_gpu_dst );	break;	//Min
		case 5:	cv::cuda::max(			*_src_cur_gpu,		_remanence_gpu_buf,	_remanence_gpu_dst );	break;	//Max
		}
		_src_cur_gpu->copyTo( _remanence_gpu_buf );
		_src_cur_gpu = &_remanence_gpu_dst;	
	}
#endif
	{
//		tmp = _src_cur->getMat(cv::ACCESS_READ);
		if( _remanence_buf.empty() || _remanence_buf.size().width != _src_cur->size().width || _remanence_buf.size().height != _src_cur->size().height )
			_src_cur->copyTo( _remanence_buf );

		switch( _s_diff_method )
		{
		case 0:	_src_cur->copyTo(												_remanence_dst );		break;	// we could avoid the copy	
				//cv::bitwise_not(		*_src_cur,			_remanence_dst );	break;
		case 1:	cv::absdiff(			*_src_cur,			_remanence_buf,		_remanence_dst );		break;	//Difference absolue
//			case 2:	cv::addWeighted(		*_src_cur, 1.,		_remanence_buf,	-1., 0., _remanence_dst );	break;
		case 2:	cv::subtract(			_remanence_buf,		*_src_cur,			_remanence_dst );		break;	//Difference
		case 3:	cv::subtract(			*_src_cur,			_remanence_buf,		_remanence_dst );		break;	//Difference inverse"
		case 4:	cv::min(				*_src_cur,			_remanence_buf,		_remanence_dst );		break;	//Min
		case 5:	cv::max(				*_src_cur,			_remanence_buf,		_remanence_dst );		break;	//Max
		}
		_src_cur->copyTo( _remanence_buf );
		_src_cur = &_remanence_dst;

//		tmp.release();
	}
	update_dst_index( _remanence_image_dst, "Diff image" );
}

void	c_bdd_img_cv_multitouch::do_remanence_base()
{
	if ( _b_remanence_ref_in_bind )	//	means the remanence_ref should be associated with the bind number 
	{
		if ( update_img_ref() )
			_remanence_img_ref.convertTo( _remanence_ref, CV_32F, 256. );
	}

	if( _remanence_data.empty() )	//todo make it better should trigger also when format change
		_b_remanence_restart_trig_ui = true;
	
	//	opencv force us to convert to float first and scale by 256 for precision
	_src_cur->convertTo( _remanence_buf, CV_32F, 256. );
	cv::UMat ref;
	if( _b_remanence_restart_trig_ui )
	{
		if( _b_remanence_ref_in_bind )
		{
			if( !_remanence_ref.empty() ) // get image reference
				ref = _remanence_ref;
		}
		else
			ref = _remanence_buf; //	we copy to our reference buffer
		if( !ref.empty() )
		{
			ref.copyTo( _remanence_data );
			_b_remanence_restart_trig_ui = false;
		}
	}

	INT32	channel_nb = _src_cur->channels();
	if( _b_remanence_ref_in_bind )
	{
		if( !_remanence_ref.empty() )
		{
			//todo something wrong here there is probably one buffer to get rid of, why a copy here ?
			_remanence_ref.copyTo( _remanence_data );
			if( channel_nb == 1 )
				ref = _remanence_data;
		}
	}
	else
	{
		if( channel_nb == 1 )
		{
			if(		 _b_remanence_accumulate_max )
				cv::max( _remanence_data, _remanence_buf, _remanence_data );	
			else if( _b_remanence_accumulate_min )
				cv::min( _remanence_data, _remanence_buf, _remanence_data );

			DOUBLE	factor;
			bool	b_do_accum;
			if( _remanence_time != 0. && _b_remanence_accumulate )
			{
				DOUBLE dt = _time_got_frame - _time_got_frame_last;
				if( dt < 0. )
					dt = .04;
				factor = MIN1( dt / _remanence_time ); //min make sure remanence not totally fucked when stalled
				b_do_accum = true;
			}
			else
			{
				factor = 0.;
				b_do_accum = false;
			}
			if( b_do_accum )
			{	//	a lot from the old image and a little from the new
				if( factor != 0. )
					cv::addWeighted( _remanence_data, 1.-factor, _remanence_buf, factor, 0., _remanence_data );
			}
			ref = _remanence_data;
		}
	}
	if( !ref.empty() )
	{
//todo fucked up by Jean
/*
		if( _b_remanence_save_trig )
		{
			bool b_bgr = false; // is it really mono here ?
			c_img_2d* img = nullptr;
			cv_umat_to_img( _remanence_buf, img, b_bgr );
			if( img )
				img->write( _remanence_filename_ui.get(), c_img_utils::FILE_TYPE::PNG );
			//jean se passe quoi pour le img created in cv_umat_to_img()
			_b_remanence_save_trig = false;
		}
*/
		if( _b_remanence_diff )
		{
			switch( _s_remanence_diff_method )
			{
			case 0:	ref.copyTo( _remanence_buf );							break;	// we could avoid the copy	
			case 1:	cv::absdiff(	ref, _remanence_buf, _remanence_buf );	break;	//Difference absolue
			case 2:	cv::subtract(	_remanence_buf, ref, _remanence_buf );	break;	//Difference
			case 3:	cv::subtract(	ref, _remanence_buf, _remanence_buf );	break;	//Difference inverse"
			case 4:	cv::min(		ref, _remanence_buf, _remanence_buf );	break;	//Min
			case 5:	cv::max(		ref, _remanence_buf, _remanence_buf );	break;	//Max
			}
			REAL	diff_min = _remanence_diff_min * aaa::img::REAL_NEARLY_256;
			_remanence_buf.convertTo( _remanence_dst, CV_8UC1, _remanence_diff_factor/aaa::img::REAL_NEARLY_256, -diff_min*_remanence_diff_factor );
		}
		else
			ref.convertTo( _remanence_dst, CV_8UC1, 1/256. );
	}
}

void	c_bdd_img_cv_multitouch::do_agitation()
{
	if( !_agitation_img_last )
		_agitation_img_last = c_img_2d::create( __FUNCTION__ );

	INT32 size_x	=	_src_cur->size().width;
	INT32 size_y	=	_src_cur->size().height;
	INT32 ch_nb		=	_src_cur->channels();

	if( ch_nb != 1 )
	{
		err_print( "%s() : not implemented yet for %d channel", __FUNCTION__, ch_nb );
		return;
	}

	if( !_agitation_img_last->is_size_format( size_x,size_y, aaa::PIXEL_FORMAT::R_8 ) )
	{
		_agitation_img_last->init_with_size( size_x,size_y, aaa::PIXEL_FORMAT::R_8, __FUNCTION__ );
		_agitation_img_last->set_cpu_keep( true );			//	we don't want to check all the time or reload
		_agitation_img_last->fill_compo( 0, 0. );
	}

	INT64	val			= 0;
	INT64	average		= 0;
	INT32	pixel_nb	= size_x * size_y;
	if( _src_cur->depth() == 32 ) // Is it fp32 ?
	{
		FP32*	p_src	=	_src_cur->getMat(cv::ACCESS_READ).ptr<float>();//src->get_data_float_pt_hack();
		UINT8*		p_prev	=	_agitation_img_last->get_data_uint8();

		if( !p_src || !p_prev )
		{
			err_print( "%s() : one of the source data is Null", __FUNCTION__ );
		}
		else
		{
			--p_src;
			--p_prev;
			do
			{
				INT32 cur =	INT32((*++p_src) * 256.);
				average += cur;
				val += ABS( *++p_prev - cur );
				*p_prev	= cur; 
			}
			while( --pixel_nb );
		}
	}
	else
	{
		UINT8*	p_src		=	_src_cur->getMat(cv::ACCESS_READ).ptr<uchar>();
		UINT8*	p_prev		=	_agitation_img_last->get_data_uint8();

		if( !p_src || !p_prev )
		{
			err_print( "%s() : one of the source data is Null", __FUNCTION__ );
		}
		else
		{
			--p_src;
			--p_prev;
			do
			{
				INT32 cur =	*++p_src;
				average += cur;
				val += ABS( *++p_prev - cur );
				*p_prev	= cur; 
			}
			while( --pixel_nb );
		}
	}
	pixel_nb	=	size_x * size_y;
	_agitation_out		= REAL( DOUBLE( val )	  / DOUBLE( pixel_nb ) );
	_grey_average_out	= REAL( DOUBLE( average ) / DOUBLE( pixel_nb ) );
}

void	c_bdd_img_cv_multitouch::do_background()
{
	if( _b_background_reset_trig || _back_ref.empty() )
	{
		if( _back_ref.empty() )
			_back_ref = _src_cur->clone();
		if( !_back_ref.empty() )
			_src_cur->copyTo( _back_ref );
		/*		if(!_back_mask )
		{
		_back_mask = cvCreateImage( cvSize( _ipl_back_ref->width, _ipl_back_ref->height), _ipl_back_ref->depth, 1);
		_back_mask->origin = _ipl_back_ref->origin;  // same vertical flip as reference
		cvSet( _back_mask, cvScalar(0,0,0));
		}
		cvAdd( _ipl_back_ref, _back_mask, _ipl_back_ref );
		*/
		/*		if( _back_nPolyMask )
		{
		cvSet( _back_mask, cvScalar( 255, 255, 255 ) );			
		cvFillConvexPoly( _back_mask, _back_polyMask, _back_nPolyMask, cvScalar( 0, 0, 0 ) );
		}
		*/
		_b_background_reset_trig = false;
	}

	if( _b_background_absolute )
	{	//	dst = abs(src-ref)
		cv::absdiff( *_src_cur, _back_ref, _back_dst );
	}
	else
	{	// dst = src-ref
		cv::subtract( *_src_cur, _back_ref, _back_dst );
	}

	if( _b_background_divide )
	{
		if( !_back_for_div.empty() )
			_back_ref.copyTo( _back_for_div );
		else
			_back_for_div = _back_ref.clone();
		//	dst(mask) = src(mask) + value
		//	cvAddS( _ipl_back_ref, _background_divide_sub*REAL_NEARLY_256, _ipl_back_for_div );
		cv::addWeighted( _back_ref, 1., _back_ref, 0., _background_divide_add*aaa::img::REAL_NEARLY_256, _back_for_div );
		//	element-wise division/inversion with scaling: 
		//		dst(idx) = src1(idx) * scale / src2(idx)
		//		or dst(idx) = scale / src2(idx) if src1 == 0
		cv::divide( _back_dst, _back_for_div, _back_dst, _background_divide_factor*aaa::img::REAL_NEARLY_256 );
	}
	_src_cur = &_back_dst;
	update_dst_index( _background_image_dst, "Background image" );
}

void	c_bdd_img_cv_multitouch::do_laplace()
{
	INT32	tmp = _laplace_size;
	if( (tmp & 1)== 0 )
		tmp += 1;
	if( tmp > 7 )
		tmp = 7;
	cv::Laplacian( *_src_cur, _laplace_float, CV_16S, tmp );
	_laplace_float.convertTo( _laplace_dst, _src_cur->type() );
	_src_cur = &_laplace_dst;
	update_dst_index( _laplace_image_dst, "Laplace image" );
}


void	c_bdd_img_cv_multitouch::do_sobel()
{
	if( _sobel_aperture == -1 )
	{
		if( _sobel_dx != 0 && _sobel_dy != 0 )
		{
			_sobel_dy = 0;
		}
	}
	if( _sobel_dx==0 && _sobel_dy==0 )
	{
		_sobel_dx = 1;
	}
	INT32	tmp = aaa::MAX( _sobel_aperture, _sobel_dx+1, _sobel_dy+1 );
	if( (tmp & 1)== 0 )
		tmp += 1;

	cv::Sobel( *_src_cur, _sobel_dst, _src_cur->depth(), _sobel_dx, _sobel_dy, tmp );

	_src_cur = &_sobel_dst;
	update_dst_index( _sobel_image_dst, "Sobel image" );
}

void	c_bdd_img_cv_multitouch::do_erode()
{
	cv::Mat kernel = cv::getStructuringElement( cv::MORPH_RECT, cv::Size( _erode_size, _erode_size ) );
	cv::erode( *_src_cur, _erode_dst, kernel );

	_src_cur = &_erode_dst;
	update_dst_index( _erode_image_dst, "Erode image" );
}

void	c_bdd_img_cv_multitouch::do_dilate()
{
	cv::Mat kernel = cv::getStructuringElement( cv::MORPH_RECT, cv::Size( _dilate_size, _dilate_size ) );
	cv::dilate( *_src_cur, _dilate_dst, kernel );
	
	_src_cur = &_dilate_dst;
	update_dst_index( _dilate_image_dst, "Dilate image" );
}


void c_bdd_img_cv_multitouch::bc_update_lut( REAL min, REAL max )
{
	if( min == max )
	{
		uchar v = uchar( min * aaa::img::REAL_NEARLY_256 );
		for( INT32 i = 0; i < 256; ++i )
			_bc_lutmat.at<uchar>(i) = v;
	}
	else
	{
		REAL CONST b	=	min * REAL(255);
		REAL CONST a	=	aaa::img::REAL_NEARLY_256 / (REAL(255)*(max-min));
		uchar* p = _bc_lutmat.data;
		for( int i = 0; i < 256; ++i )
			_bc_lutmat.at<uchar>(i) = uchar( CLAMP( (REAL(i)-b)*a, 0., 255.) );
	}
}

void	c_bdd_img_cv_multitouch::do_bc( cv::UMat* dst, REAL min, REAL max  )
{
	auto type = _src_cur->type();
	if( type == CV_8UC1 )
	{	
		if( _bc_lutmat.empty() )
			_bc_lutmat.create( 1, 256, CV_8UC1 );
		if( _bc_min != min || _bc_max != max )
		{
			_bc_min = min;
			_bc_max = max;
			bc_update_lut( min, max );
		}
		cv::LUT( *_src_cur, _bc_lutmat, *dst );
	}
	else// if( _src_cur->type() == CV_16UC1 )
	{
		cv::addWeighted( *_src_cur, 1./(max-min), *_src_cur, 0., min, *dst, -1 );
//todo	//	cv::cuda::addWeighted( (*split)[a], _mono_compo_factor_1_ui, (*split)[b], _mono_compo_factor_2_ui, _mono_compo_offset_ui*256., *mono );
	}
	//else
	//{
	//	ERR_PRINT_STRING( "%s() only support CV_8UC1 type (8bits)", __FUNCTION__ );
	//	return;
	//}
	_src_cur = dst;
	update_dst_index( _bc_image_dst, "Brightness Contrast image" );
}

void	c_bdd_img_cv_multitouch::do_highpass()
{
	if( _b_simple_highpass )
	{
		// create the unsharp mask using a linear average filter
		if( cv_smooth_method[_simple_highpass_smooth_method] == CV_BLUR )
			cv::blur( *_src_cur, _hp_buffer, cv::Size( _simple_highpass_smooth_size, _simple_highpass_smooth_size ) );
		else if( cv_smooth_method[_simple_highpass_smooth_method] == CV_GAUSSIAN )
		{
			// todo : Do something with the sigmas (x y)
			cv::GaussianBlur( *_src_cur, _hp_buffer, cv::Size( _simple_highpass_smooth_size, _simple_highpass_smooth_size ), 0. );
		}
		else if( cv_smooth_method[_simple_highpass_smooth_method] == CV_MEDIAN )
		{
			// todo : lock smooth_size to odds > 1 : 3,5,7...
			cv::medianBlur( *_src_cur, _hp_buffer, 3 );
		}
		else if( cv_smooth_method[_simple_highpass_smooth_method] == CV_BILATERAL )
		{
			// todo : Deal with the sigmas (color, space)
			cv::bilateralFilter( *_src_cur, _hp_buffer, _simple_highpass_smooth_size, 10., 10. );
		}

		cv::subtract( *_src_cur, _hp_buffer, _hp_buffer );

		// filter out the noise using a median filter
		// todo : lock noise_size to odds > 1 : 3,5,7...
		cv::medianBlur( _hp_buffer, _hp_dst, 3 );
	}
	else
	{
		_src_cur->convertTo( _hp_outra, CV_16S );
		my_create_gray( &_hp_outra, &_hp_outra2 ); // replaces ipl create_gray from before
		cv::GaussianBlur( _hp_outra, _hp_outra2, cv::Size( (_highpass_filter * 2) + 3, (_highpass_filter * 2) + 3  ), 0. );
		cv::subtract( _hp_outra, _hp_outra2, _hp_outra2 );
		if( _b_highpass_erode_dilate )
		{
			_hp_outra2.convertTo( _hp_dst, CV_8U, ((double)_highpass_scale + 1.0 ), 0. );
			cv::erode( _hp_dst, _hp_dst, _hp_element );
			cv::GaussianBlur( _hp_dst, _hp_dst, cv::Size( 11, 11 ), 0. );
			cv::dilate( _hp_dst, _hp_dst, _hp_element );
		}
		else
		{
			_hp_outra2.convertTo( _hp_dst, CV_8U, ((double)_highpass_scale + 1.0), 0. );
			cv::erode( _hp_dst, _hp_dst, _hp_element );
			cv::GaussianBlur( _hp_dst, _hp_dst, cv::Size( 11, 11 ), 0. );
			cv::dilate( _hp_dst, _hp_dst, _hp_element2 );
			cv::dilate( _hp_dst, _hp_dst, _hp_element );
		}
	}
	
	_src_cur = &_hp_dst;
	update_dst_index( _highpass_image_dst, "Highpass image" );
}

void	c_bdd_img_cv_multitouch::do_rectify()
{
	if( _b_rectify_auto_trig )
	{
		INT32	sx = _src_cur->size().width;
		INT32	sy = _src_cur->size().height;

		UINT8	highest = 0;
		for( INT32 y = 0; y < sy; ++y )
		{
			for( INT32 x = 0; x < sx; ++x )
			{
				UINT8	tmp = _src_cur->getMat( cv::ACCESS_READ ).at<uchar>( y, x );
				if( tmp > highest )
					highest = tmp;
			}
			if( highest == 255 )
				break;
		}

		_rectify_level = highest / aaa::img::REAL_NEARLY_256;
		_b_rectify_auto_trig = false;
	}

//	CvThreshType type;
	INT32 type;
	if( _b_rectify_inverse )
		type = _b_rectify_to_max ? CV_THRESH_BINARY_INV : CV_THRESH_TOZERO_INV ;
	else
		type = _b_rectify_to_max ? CV_THRESH_BINARY : CV_THRESH_TOZERO ;
	cv::threshold( *_src_cur, _rectify_dst,
					double( _rectify_level * aaa::img::REAL_NEARLY_256 ),
					double( _rectify_max_level * aaa::img::REAL_NEARLY_256 ),
					type );
	
	_src_cur = &_rectify_dst;
	update_dst_index( _rectify_image_dst, "Rectify image" );
}


//FINLINE UINT32	do_diff( bool b_diff, REAL cur, REAL ref, REAL min, REAL factor )
//{
//	if( b_diff )
//	{
//		cur = ABS( cur - ref ) - min;
//		if( cur < 0 )	return 0;
//		else
//		{
//			cur *= factor;
//			if( cur < 255. )
//				return UINT32(cur);
//			else
// 				return 255;
//		}
//	}
//	return UINT32(ref);
//}

bool	c_bdd_img_cv_multitouch::update_img_ref()
{
	if( _remanence_ref_bind < 0 )	//	strange check because the param is initialized and positif 
		return false;

	//	get the img from the image bind
	c_img_2d*	img	= g_bind_img_2d->get_ready( _remanence_ref_bind );
	//	if it doesn't exist : create it at the appropriate size 
	if( !img )
	{
		img = g_bind_img_2d->get_always( _remanence_ref_bind );
		if( !img )
		{
			err_print( "%s() : no image ref at bind %d", __FUNCTION__, _remanence_ref_bind );
			return false;
		}
		img->init_with_size( _src_size[0], _src_size[1], aaa::PIXEL_FORMAT::RGBA_8, __FUNCTION__ );
		img->set_cpu_keep( true );
	}
	else if( img->is_empty() && img->is_time_modification() ) //hack
	{
		img->set_cpu_keep( true );			//	we don't want to check all the time or reload
		img->reset_time_modification();		//hack when flag free_when_loaded_in_gpu is active this should force needed reloading
	}
	//	test if the binded ref changed ( or if it is the first time )
	if( img->is_state_unique( _remanence_ref_bind_unique_id_last ) )
		return false;
	_remanence_ref_bind_unique_id_last = img->get_state_unique();

	//last
	_src_cur->copyTo( _remanence_img_ref );
	if( !_remanence_img_ref.empty() ) // do we also need to check if size is ok ?
		return false;

	cv::UMat	source;
	cv::UMat	mono;
	//bool		b_changed;
	//if( !ipl_source.build_using( img, b_changed ) ) // updated line should only build and not copy (+use b_changed) ?
	if( !aaa::c_cv::img_to_cv_umat( img, source ) )
		return false;

	my_create_gray( &source, &mono );

	// _remanence_img_ref is created from _src_cur so we check that the ref in bind has same size as _src_cur
	if( _src_size[0] != img->get_size_x() || _src_size[1] != img->get_size_y() )
	{
		// need to resize
		// explicitly specify dsize=dst.size(); fx and fy will be computed from that.
		cv::resize( mono, _remanence_img_ref, _remanence_img_ref.size(), 0,0, CV_INTER_CUBIC );
		return true;
	}
	else
	{
		//cvCopy( ipl_mono.get_ipl(), _ipl_remanence_img_ref.get_ipl() );
		mono.copyTo( _remanence_img_ref);
		return true;
	}
	//
	return false;
}
