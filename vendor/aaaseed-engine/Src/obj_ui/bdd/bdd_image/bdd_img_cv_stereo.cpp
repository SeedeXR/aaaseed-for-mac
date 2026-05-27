#include "bdd_img_cv_stereo.h"
#include "time/speed.h"	 
#include "opencv2/photo/photo.hpp"
#include "spy.h"
		 
//#include "opencv2/cudaimgproc.hpp"
#if AAA_LIB_USE_OPENCV_CUDA()
#	include "opencv2/cudaarithm.hpp"
#	include "opencv2/cudawarping.hpp"
//#include "opencv2/calib3d.hpp"
#endif

#undef MIN
#undef MAX

//#include "Elas/elas.h"

C_PCHAR_C	c_bdd_img_cv_stereo::stereobm_str[CV_STEREOBM_CAMERA_MAX] =
{
	"Basic",
	"Fish Eye",
	"Narrow"
};
C_PCHAR_C	c_bdd_img_cv_stereo::stereobm_prefilter_str[CV_STEREOBM_PREFILTER_MAX] =
{
	"Normalized Response",
	"XSobel"
};
C_PCHAR_C	c_bdd_img_cv_stereo::stereobmgpu_preset_str[CV_STEREOBMGPU_PRESET_MAX] =
{
	"Basic",
	"Sobel pre-filtering"
};

C_PCHAR_C	c_bdd_img_cv_stereo::stereo_arithmetic_str[CV_STEREO_ARITHMETIC_MAX] =
{
	"16 bits signed",
	"32 bits floating point"
};

C_PCHAR_C	c_bdd_img_cv_stereo::stereo_type_str[STEREO_TYPE_MAX] =
{
	"Block Matching",
	"Block Matching GPU",
	"Constant Space BP GPU",
	"Belief Propagation GPU",
	"Semi-Global Block Matching",
	//"Elas"
	//"Variational Matching",
	//"Graph Cut"
};

C_PCHAR_C	c_bdd_img_cv_stereo::vm_penalization_str[PENALIZATION_MAX] =
{
	"Tichonov",
	"Charbonnier",
	"Perona Malik"
};

C_PCHAR_C	c_bdd_img_cv_stereo::vm_cycle_str[CYCLE_MAX] =
{
	"Cycle 0",
	"Cycle V"
};

FACTORY_CREATE_PROP_V1( c_bdd_img_cv_stereo, bdd_img_cv_stereo, Image OpenCV Stereo, bdd_img_cv_stereo, sub_menu="Image"; );

namespace n_bdd_img_cv_stereo
{
	CONSTEXPR INT32 BASE_PARAM_NB			= 11 + c_bdd_img::SUPER_PARAM_NB;
//	CONSTEXPR INT32 STEREOVAR_PARAM_NB		= 16;
	CONSTEXPR INT32 BILATERAL_PARAM_NB		= 7;
	CONSTEXPR INT32 THRESHOLD_PARAM_NB		= 4;
	CONSTEXPR INT32 SPECKLES_PARAM_NB		= 4;
	CONSTEXPR INT32 INPAINTING_PARAM_NB		= 4;
	CONSTEXPR INT32 STEREOSGBM_PARAM_NB		= 9;
	CONSTEXPR INT32 STEREOBM_PARAM_NB		= 13;
	CONSTEXPR INT32 STEREOBMGPU_PARAM_NB	= 4;
	CONSTEXPR INT32 STEREOBP_PARAM_NB		= 8;
	CONSTEXPR INT32 STEREOCSBP_PARAM_NB		= 12;
	CONSTEXPR INT32 GROUP_PARAM_NB			= 9;
	CONSTEXPR INT32 PARAM_NB	=	BASE_PARAM_NB
								+	BILATERAL_PARAM_NB
								+	THRESHOLD_PARAM_NB
								+	SPECKLES_PARAM_NB
								+	INPAINTING_PARAM_NB
								+	STEREOSGBM_PARAM_NB
//								+	STEREOVAR_PARAM_NB
								+	STEREOBM_PARAM_NB
								+	STEREOBMGPU_PARAM_NB
								+	STEREOBP_PARAM_NB
								+	STEREOCSBP_PARAM_NB
								+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB] =
	{
		BDD_IMG_BASE_PARAMS

		PARAM_DEF_INT32_POS_ZERO(	image_b					)
		PARAM_DEF_INT32_LOCKED_XY(	image_b_source_size		)
//		PARAM_DEF_BOOL_OFF(			mono					)
//		PARAM_DEF_BOOL_OFF(			swap_source				)
		PARAM_DEF_BOOL_OFF(			normalize				)
		PARAM_DEF_BOOL_OFF(			colored_disparity		)
		PARAM_DEF_SYMBO_PSTR_ONE(	type,					c_bdd_img_cv_stereo::stereo_type_str )
		PARAM_DEF_BOOL_OFF(			resize_disparity		)
		PARAM_DEF_REAL_POS(			resize_size_x,			.5, 1.					)
		PARAM_DEF_REAL_POS(			resize_size_y,			.5, 1.					)

		PARAM_DEF_REAL_LOCKED(		process_time			)
		PARAM_DEF_REAL_LOCKED(		process_fps				)
		PARAM_DEF_GROUP_CLOSED( Bilateral Filter, BILATERAL_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			bilateral_filter			)
			PARAM_DEF_INT32(			bilateral_nb_disparities,		64, 80,		16, 1024	)
			PARAM_DEF_INT32(			bilateral_size,					1, 5,		1, 1024		)
			PARAM_DEF_INT32(			bilateral_nb_iter,				1, 3,		1, 20		)
			PARAM_DEF_REAL_INF(			bilateral_edge_threshold,		0, 1					)
			PARAM_DEF_REAL_INF(			bilateral_max_disc_threshold,	0, 1					)
			PARAM_DEF_REAL_INF(			bilateral_sigma_range,			0, 1					)
		PARAM_DEF_GROUP_CLOSED( Threshold, THRESHOLD_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			threshold							)
			PARAM_DEF_INT32_POS(		threshold_min,			0, 0		)
			PARAM_DEF_INT32_POS(		threshold_max,			255, 255	)
			PARAM_DEF_BOOL_OFF(			threshold_normalize					)
		PARAM_DEF_GROUP_CLOSED( Speckles Filter, SPECKLES_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			speckle_filter									)
			PARAM_DEF_INT32_POS(		speckle_filter_value,			0, 255			)
			PARAM_DEF_INT32(			speckle_filter_max_size,		0, 1,	0, 2048	)
			PARAM_DEF_INT32(			speckle_filter_max_diff,		0, 1,	0, 2048	)
		PARAM_DEF_GROUP_CLOSED( Inpainting Smoothing, INPAINTING_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			inpaint_smooth											)
			PARAM_DEF_REAL_POS(			inpaint_smooth_size_x,			.5, .2					)
			PARAM_DEF_REAL_POS(			inpaint_smooth_size_y,			.5, .2					)
			PARAM_DEF_INT32_POS(		inpaint_smooth_no_depth,		0, 255					)
		PARAM_DEF_GROUP_CLOSED( StereoBM, STEREOBM_PARAM_NB )
			PARAM_DEF_SYMBO_PSTR_ZERO(	bm_type,								c_bdd_img_cv_stereo::stereo_arithmetic_str )
			PARAM_DEF_SYMBO_PSTR_ZERO(	bm_preset,								c_bdd_img_cv_stereo::stereobm_str )
			PARAM_DEF_SYMBO_PSTR_ZERO(	bm_pre_filter_type,						c_bdd_img_cv_stereo::stereobm_prefilter_str )
			PARAM_DEF_INT32(			bm_pre_filter_size,						5, 11,		5, 21		)		// preFilterSize must be odd and be within 5..255
			PARAM_DEF_INT32(			bm_pre_filter_cap,						1, 5,		1, 63		)		// preFilterCap must be within 1..63
			PARAM_DEF_INT32(			bm_sad_window_size,						5, 11,		5, 255		)		// SADWindowSize must be odd, be within 5..255 and be not larger than image width or height
			PARAM_DEF_INT32(			bm_min_disparity,						5, 10,		-1024, 1024	)
			PARAM_DEF_INT32(			bm_num_of_disparities,					64, 80,		16, 1024	)		// numberOfDisparities must be positive and divisible by 16
			PARAM_DEF_INT32_POS(		bm_texture_threshold,					0, 1					)		// texture threshold must be non-negative
			PARAM_DEF_INT32(			bm_uniqueness_ratio,					0, 1,		0, 255		)		// uniqueness ratio must be non-negative
			PARAM_DEF_INT32(			bm_speckle_window_size,					0, 1,		0, 255		)		// not sure of range
			PARAM_DEF_INT32(			bm_speckle_range,						0, 1,		0, 255		)		// not sure of range
			PARAM_DEF_BOOL_OFF(			bm_try_smaller_windows											)		// look buggy to mee ( 2022 April Maa )
		PARAM_DEF_GROUP_CLOSED( StereoBMGPU, STEREOBMGPU_PARAM_NB )
			PARAM_DEF_SYMBO_PSTR_ZERO(	bm_gpu_preset,							c_bdd_img_cv_stereo::stereobmgpu_preset_str )
			PARAM_DEF_INT32(			bm_gpu_nb_disparities,					64, 80,		0, 256		)
			PARAM_DEF_INT32(			bm_gpu_win_size,						19, 10,		2, 51		)
			PARAM_DEF_REAL_INF(			bm_average_tex_threshold,				.0, 1.0					)
		PARAM_DEF_GROUP_CLOSED( Constant Space Belief Propagation, STEREOCSBP_PARAM_NB )
			PARAM_DEF_SYMBO_PSTR_ZERO(	csbp_gpu_type,							c_bdd_img_cv_stereo::stereo_arithmetic_str )
			PARAM_DEF_BOOL_OFF(			csbp_estimate_param_trig									)
			PARAM_DEF_INT32(			csbp_nb_disparities,					128, 80,	1, 256	)	// not sure for max, can be negative ?
			PARAM_DEF_INT32(			csbp_nb_bp_iteration,					8, 10,		1, 128	)
			PARAM_DEF_INT32(			csbp_nb_of_levels,						4, 8,		1, 8	)
			PARAM_DEF_INT32(			csbp_nb_active_disparity,				4, 10,		1, 128	)
			PARAM_DEF_REAL_INF(			csbp_truncation_data_cost,				0, 1				)
			PARAM_DEF_REAL_INF(			csbp_data_weight,						0, 1				)
			PARAM_DEF_REAL_INF(			csbp_truncation_discontinuity_cost,		0, 1				)
			PARAM_DEF_REAL_INF(			csbp_discontinuity_single_jump,			0, 1				)
			PARAM_DEF_INT32_INF(		csbp_min_disparity_threshold,			5, 10				)
			PARAM_DEF_BOOL_OFF(			csbp_use_global_minimum_data_cost							)
		PARAM_DEF_GROUP_CLOSED( Belief Propagation, STEREOBP_PARAM_NB )
			PARAM_DEF_SYMBO_PSTR_ZERO(	bp_gpu_type,							c_bdd_img_cv_stereo::stereo_arithmetic_str )
			PARAM_DEF_INT32(			bp_gpu_nb_disparities,					64, 80,		1, 256	)	// not sure for max, can be negative ?
			PARAM_DEF_INT32(			bp_gpu_nb_bp_iteration,					5, 10,		1, 128	)
			PARAM_DEF_INT32(			bp_gpu_nb_of_levels,					5, 10,		1, 8	)
			PARAM_DEF_REAL_INF(			bp_gpu_truncation_data_cost,			0, 1				)
			PARAM_DEF_REAL_INF(			bp_gpu_data_weight,						0, 1				)
			PARAM_DEF_REAL_INF(			bp_gpu_truncation_discontinuity_cost,	0, 1				)
			PARAM_DEF_REAL_INF(			bp_gpu_discontinuity_single_jump,		0, 1				)
		PARAM_DEF_GROUP_CLOSED( StereoSGBM, STEREOSGBM_PARAM_NB )
			PARAM_DEF_INT32_INF(		sgbm_pre_filter_cap,					1, 5	)		// not sure of range
			PARAM_DEF_INT32_INF(		sgbm_sad_window_size,					5, 11	)
			PARAM_DEF_INT32_INF(		sgbm_min_disparity,						5, 10	)		// not sure of range
			PARAM_DEF_INT32_INF(		sgbm_num_of_disparities,				64, 80	)
			PARAM_DEF_INT32_INF(		sgbm_uniqueness_ratio,					0, 1	)
			PARAM_DEF_INT32_INF(		sgbm_speckle_window_size,				0, 1	)		// not sure of range
			PARAM_DEF_INT32_INF(		sgbm_speckle_range,						0, 1	)		// not sure of range
			PARAM_DEF_INT32_INF(		sgbm_disp12_max_diff,					0, 1	)		// not sure of range
			PARAM_DEF_BOOL_OFF(			sgbm_full_dp 									)
		//PARAM_DEF_GROUP_CLOSED( StereoVar, STEREOVAR_PARAM_NB )
		//	PARAM_DEF_INT32_INF(		vm_levels,								1, 5	)		// not sure of range
		//	PARAM_DEF_REAL_INF(			vm_pyr_scale,							5, 11	)
		//	PARAM_DEF_INT32_INF(		vm_n_it,								10, 5	)		// not sure of range
		//	PARAM_DEF_INT32_INF(		vm_min_disp,							64, 80	)
		//	PARAM_DEF_INT32_INF(		vm_max_disp,							0, 1	)
		//	PARAM_DEF_INT32_INF(		vm_poly_n,								0, 1	)		// not sure of range
		//	PARAM_DEF_REAL_INF(			vm_poly_sigma,							0, 1	)		// not sure of range
		//	PARAM_DEF_REAL_INF(			vm_fi,									0, 1	)		// not sure of range
		//	PARAM_DEF_REAL_INF(			vm_lambda,								0, 1	)		// not sure of range
		//	PARAM_DEF_SYMBO_PSTR_ZERO(	vm_penalization,						c_bdd_img_cv_stereo::vm_penalization_str )
		//	PARAM_DEF_SYMBO_PSTR_ZERO(	vm_cycle,								c_bdd_img_cv_stereo::vm_cycle_str )
		//	PARAM_DEF_BOOL_ON(			vm_use_smart_id									)
		//	PARAM_DEF_BOOL_ON(			vm_use_auto_params								)
		//	PARAM_DEF_BOOL_ON(			vm_use_initial_disparity						)
		//	PARAM_DEF_BOOL_ON(			vm_use_median_filtering							)
		//	PARAM_DEF_BOOL_OFF(			vm_use_equalize_hist							)
	};
}

static	o_str sum_up;

void	c_bdd_img_cv_stereo::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start_src_resize( false, false );

	param_set_pt( h, _src_b_img_index			);
	param_set_pt( h, _src_b_size_x				);
	param_set_pt( h, _src_b_size_y				);
//	param_set_pt( h, _b_mono_ui					);
//	param_set_pt( h, _b_swap_src				);

	_speed_normalize->build_comment( sum_up, _b_normalize_ui, false );
	get_param(h)->set_comment( sum_up );
	param_set_pt( h, _b_normalize_ui			);

	_speed_color->build_comment( sum_up, _b_colored_disp_ui );
	get_param(h)->set_comment( sum_up );
	param_set_pt( h, _b_colored_disp_ui			);
	if( g_speed_master->is_run() )
	{
		CHAR	buf[256];
		sprintf( buf, "ON %.2f", _speed_stereo->get_interval_last_ms() );
		get_param(h)->set_comment( buf );
	}
	else
	{
		get_param(h)->clear_comment();
	}
	param_set_pt( h, _s_stereo_algo_ui			);

	param_set_pt( h, _b_resize_ui				);
	param_set_pt_2( h, _resize_size_ui			);


	param_set_pt( h, _speed_bdd->get_pt_interval_ms()		);
	param_set_pt( h, _speed_bdd->get_pt_fps_average()		);

	_speed_bilateral->build_comment( sum_up, _b_bilateral_filter_ui );
	get_param(h)->set_comment( sum_up );
	++h;
		param_set_pt( h, _b_bilateral_filter_ui				);
		param_set_pt( h, _bilateral_disp_nb_ui				);
		param_set_pt( h, _bilateral_size_ui					);
		param_set_pt( h, _bilateral_iterations_ui			);
		param_set_pt( h, _bilateral_edge_threshold_ui		);
		param_set_pt( h, _bilateral_max_disc_threshold_ui	);
		param_set_pt( h, _bilateral_sigma_range_ui			);

	_speed_threshold->build_comment( sum_up, _b_threshold_ui );
	get_param(h)->set_comment( sum_up );
	++h;
		param_set_pt( h, _b_threshold_ui			);
		param_set_pt( h, _threshold_min_ui			);
		param_set_pt( h, _threshold_max_ui			);
		param_set_pt( h, _b_threshold_norm_ui		);

	_speed_speckles->build_comment( sum_up, _b_speckle_filter_ui );
	get_param(h)->set_comment( sum_up );
	++h;
		param_set_pt( h, _b_speckle_filter_ui			);
		param_set_pt( h, _speckle_filter_value_ui			);
		param_set_pt( h, _speckle_filter_max_size_ui			);
		param_set_pt( h, _speckle_filter_max_diff_ui		);


	_speed_inpaint->build_comment( sum_up, _b_inpaint_ui );
	get_param(h)->set_comment( sum_up );
	++h;
		param_set_pt( h, _b_inpaint_ui			);
		param_set_pt( h, _inpaint_size_x_ui			);
		param_set_pt( h, _inpaint_size_y_ui			);
		param_set_pt( h, _inpaint_no_depth_ui		);

	_speed_stereo->build_comment( sum_up, _s_stereo_algo_ui == STEREO_BM );
	get_param(h)->set_comment( sum_up );
	++h;
		param_set_pt( h, _bm_disp_type_ui				);
		param_set_pt( h, _bm_preset_ui					);
		param_set_pt( h, _bm_pre_filter_type_ui			);
		param_set_pt( h, _bm_pre_filter_size_ui			);
		param_set_pt( h, _bm_pre_filter_cap_ui			);
		param_set_pt( h, _bm_sad_window_size_ui			);
		param_set_pt( h, _bm_min_disparity_ui			);
		param_set_pt( h, _bm_num_disparities_ui			);
		param_set_pt( h, _bm_texture_threshold_ui		);
		param_set_pt( h, _bm_uniqueness_ratio_ui		);
		param_set_pt( h, _bm_speckle_window_size_ui		);
		param_set_pt( h, _bm_speckle_range_ui			);
		param_set_pt( h, _bm_b_try_smaller_windows_ui	);

	_speed_stereo->build_comment( sum_up, _s_stereo_algo_ui == STEREO_BM_GPU );
	get_param(h)->set_comment( sum_up );
	++h;
		param_set_pt( h, _bm_gpu_preset_ui				);
		param_set_pt( h, _bm_gpu_ndisp_ui				);
		param_set_pt( h, _bm_gpu_win_size_ui			);
		param_set_pt( h, _bm_average_tex_threshold_ui	);

	_speed_stereo->build_comment( sum_up, _s_stereo_algo_ui == STEREO_CSBP );
	get_param(h)->set_comment( sum_up );
	++h;
		param_set_pt( h, _csbp_gpu_disp_type_ui						);
		param_set_pt( h, _csbp_param_trig_ui						);
		param_set_pt( h, _csbp_gpu_ndisp_ui							);
		param_set_pt( h, _csbp_gpu_iters_ui							);
		param_set_pt( h, _csbp_gpu_levels_ui						);
		param_set_pt( h, _csbp_gpu_nr_plane_ui						);
		param_set_pt( h, _csbp_gpu_max_data_term_ui					);
		param_set_pt( h, _csbp_gpu_data_weight_ui					);
		param_set_pt( h, _csbp_gpu_max_disc_term_ui					);
		param_set_pt( h, _csbp_gpu_disc_single_jump_ui				);
		param_set_pt( h, _csbp_gpu_min_disp_th_ui					);
		param_set_pt( h, _csbp_gpu_b_use_local_init_data_cost_ui	);

	_speed_stereo->build_comment( sum_up, _s_stereo_algo_ui == STEREO_BP );
	get_param(h)->set_comment( sum_up );
	++h;
		param_set_pt( h, _bp_gpu_disp_type_ui				);
		param_set_pt( h, _bp_gpu_ndisp_ui					);
		param_set_pt( h, _bp_gpu_iters_ui					);
		param_set_pt( h, _bp_gpu_levels_ui					);
		param_set_pt( h, _bp_gpu_max_data_term_ui			);
		param_set_pt( h, _bp_gpu_data_weight_ui				);
		param_set_pt( h, _bp_gpu_max_disc_term_ui			);
		param_set_pt( h, _bp_gpu_disc_single_jump_ui		);

	_speed_stereo->build_comment( sum_up, _s_stereo_algo_ui == STEREO_SGBM );
	get_param(h)->set_comment( sum_up );
	++h;
		param_set_pt( h, _sgbm_pre_filter_cap_ui			);
		param_set_pt( h, _sgbm_sad_window_size_ui			);
		param_set_pt( h, _sgbm_min_disparity_ui				);
		param_set_pt( h, _sgbm_num_disparities_ui			);
		param_set_pt( h, _sgbm_uniqueness_ratio_ui			);
		param_set_pt( h, _sgbm_speckle_window_size_ui		);
		param_set_pt( h, _sgbm_speckle_range_ui				);
		param_set_pt( h, _sgbm_disp12_max_diff_ui			);
		param_set_pt( h, _sgbm_b_full_dp_ui					);	//unused
	//++h;
	//	param_set_pt( h, _vm_levels_ui						);
	//	param_set_pt( h, _vm_pyr_scale_ui					);
	//	param_set_pt( h, _vm_n_it_ui						);
	//	param_set_pt( h, _vm_min_disp_ui					);
	//	param_set_pt( h, _vm_max_disp_ui					);
	//	param_set_pt( h, _vm_poly_n_ui						);
	//	param_set_pt( h, _vm_poly_sigma_ui					);
	//	param_set_pt( h, _vm_fi_ui							);
	//	param_set_pt( h, _vm_lambda_ui						);
	//	param_set_pt( h, _vm_penalization_ui				);
	//	param_set_pt( h, _vm_cycle_ui						);
	//	param_set_pt( h, _vm_b_use_smart_id_ui				);
	//	param_set_pt( h, _vm_b_use_auto_params_ui			);
	//	param_set_pt( h, _vm_b_use_initial_disparity_ui		);
	//	param_set_pt( h, _vm_b_use_median_filtering_ui		);
	//	param_set_pt( h, _vm_b_use_equalize_hist_ui			);

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bdd_img_cv_stereo )
	//,_bm							(nullptr)
	//,_sgbm						(nullptr)
	//,_var							(nullptr)
	//,_csbp_gpu					(nullptr)
	//,_bp_gpu						(nullptr)
	//,_bm_gpu						(nullptr)
	,_bm_preset						(CV_STEREOBM_BASIC)
	//,_disparity_filter			(nullptr)
	,_s_stereo_algo					(0)
	,_b_gpu							(false)
	,_src_b_size_x					(0)
	,_src_b_size_y					(0)
	,_bilateral_disp_nb				(0)
	,_bilateral_size				(0)
	,_bilateral_iterations			(0)
	,_bilateral_edge_threshold		(.0)
	,_bilateral_max_disc_threshold	(.0)
	,_bilateral_sigma_range			(.0)
{
	_speed_bdd			= new c_speed( true, 4, g_speed_master->get_pb_print_bdd(),	"update"			, this );
	_speed_stereo		= new c_speed( true, 5,	g_speed_master->get_pb_print_bdd(),	"stereo"			, this );
	_speed_bilateral	= new c_speed( true, 5, g_speed_master->get_pb_print_bdd(),	"bilateral"			, this );
	_speed_normalize	= new c_speed( true, 5, g_speed_master->get_pb_print_bdd(),	"normalize"			, this );
	_speed_threshold	= new c_speed( true, 5, g_speed_master->get_pb_print_bdd(),	"threshold"			, this );
	_speed_color		= new c_speed( true, 5, g_speed_master->get_pb_print_bdd(),	"color_disparity"	, this );
	_speed_inpaint		= new c_speed( true, 5, g_speed_master->get_pb_print_bdd(),	"inpainting"		, this );
	_speed_speckles		= new c_speed( true, 6, g_speed_master->get_pb_print_bdd(),	"speckles filter"	, this );

	param_init_with( n_bdd_img_cv_stereo::param, n_bdd_img_cv_stereo::PARAM_NB );
}

void	c_bdd_img_cv_stereo::close()
{
	IF_THIS_NULL_RETURN();
	
	close_mat();

	_bm.release();
	_sgbm.release();
#if AAA_LIB_USE_OPENCV_CUDA()
	_csbp_gpu.release();
	_bp_gpu.release();
	_bm_gpu.release();
	_disparity_filter.release();
	//SAFE_DELETE( _bm );
//		SAFE_DELETE( _sgbm );
////		SAFE_DELETE( _var );
//		SAFE_DELETE( _csbp_gpu );
//		SAFE_DELETE( _bp_gpu );
//		SAFE_DELETE( _bm_gpu );

	//SAFE_DELETE( _disparity_filter );
#endif
	obj_delete( 	_speed_bdd );
	obj_delete( 	_speed_stereo );
	obj_delete( 	_speed_bilateral );
	obj_delete( 	_speed_normalize );
	obj_delete( 	_speed_threshold );
	obj_delete( 	_speed_speckles );
	obj_delete( 	_speed_color );
	obj_delete( 	_speed_inpaint );
}

void	c_bdd_img_cv_stereo::close_mat()
{
#if AAA_LIB_USE_OPENCV_CUDA()
	//if( !_disp_dst_cur.empty() )	{	_disp_dst_cur.release();		}	// = nullptr;	}
	//if( !_disp8_dst_cur.empty() )	{	_disp8_dst_cur.release();		}	// = nullptr;	}
	//if( !_gpu_dst_cur.empty() )		{	_gpu_dst_cur.release();		}	// = nullptr;	}

	//_src_a_gpu_dst.release();
	//_src_b_gpu_dst.release();
	_disp_gpu_dst.release();
	_disp_color_gpu_dst.release();
	_thresh_max_gpu_dst.release();
	_thresh_min_gpu_dst.release();
	_normalize_gpu_dst.release();
	_resize_gpu_dst.release();

	//_gpumat_transpose_a.release();
	//_gpumat_flip_a.release();
	//_gpumat_transpose_b.release();
	//_gpumat_flip_b.release();
#endif

	//_mat_transpose_a.release();
	//_mat_flip_a.release();
	//_mat_transpose_b.release();
	//_mat_flip_b.release();
	_disp_cpu_dst.release();
	_disp8_cpu_dst.release();
	_resize_cpu_dst.release();
	_normalize_cpu_dst.release();
}

c_bdd_img_cv_stereo::~c_bdd_img_cv_stereo()
{
	close();
}

void	c_bdd_img_cv_stereo::update()
{
	_speed_bdd->begin();
	if( (_s_stereo_algo_ui == STEREO_BM) || (_s_stereo_algo_ui == STEREO_SGBM) ) //|| (_s_stereo_algo_ui == STEREO_ELAS)  )
		_b_gpu = false;
	else
		_b_gpu = true;
	// opencv stereobm only accept grey image
	_b_mono = _b_mono_ui || (_s_stereo_algo_ui == STEREO_BM) || (_s_stereo_algo_ui == STEREO_BM_GPU);
	if( update_mat_src_2() )
	//if( update_mat_src( _b_swap_src ? _mat_src_b : _mat_src_a, _b_swap_src ? _mat_src_a : _mat_src_b ) )
	{
		do_process();
	}
	_speed_bdd->end();
}

void	c_bdd_img_cv_stereo::draw()
{
}

void	c_bdd_img_cv_stereo::do_process()
{
//	UINT32	channel = 1;
	//aaa::PIXEL_TYPE	type = aaa::PIXELTYPE::GREY;
	//if ( _b_gpu )
	//{
	//	type = _b_colored_disp_ui ? aaa::PIXEL_TYPE::RGBA : aaa::PIXEL_TYPE::GREY;
	//}
	//bool	b_ok = false;
	cpy_v2( _resized_size, _src_size );

	if( _b_resize_ui )
	{
		_resized_size[0] = MAX( 2, I_FLOOR( _resized_size[0] * _resize_size_ui[0] ) );
		_resized_size[1] = MAX( 2, I_FLOOR( _resized_size[1] * _resize_size_ui[1] ) );
	}
	//b_ok = build_cv_mat( _mat_dst, _dst_img_index_ui, _resized_sx, _resized_sy, "Disparity", type );
	//if( b_ok )
	if( update_mat_src_2() )
	{
		do_stereo_disparity();
	}
	//else
	//{
	//	ERR_PRINT_STRING( "Error building Disparity" );
	//	return;
	//}
}

INT32	c_bdd_img_cv_stereo::do_disparity_bm()
{
	if( _bm.empty() )
	{
		_bm = cv::StereoBM::create();
		//_bm = cv::createStereoBM( _bm_preset_ui, 1 );
		_bm_preset = _bm_preset_ui;
	}
	if( !_bm.empty() )
	{
		cv::Size	img_size			= _src_cur->size();
		INT32		number_disparities	= _bm_num_disparities_ui;
		number_disparities				= I_ROUND_CEIL(_bm_num_disparities_ui, 16);

		INT32	sad_window_size = _bm_sad_window_size_ui > 0 ? _bm_sad_window_size_ui : 9;
		sad_window_size = aaa::MIN( sad_window_size, img_size.width, img_size.height );
		if ((sad_window_size % 2) == 0 )
			{	++sad_window_size;	}

		UINT32	bm_pre_filter_type = _bm_pre_filter_size_ui;
		if ((bm_pre_filter_type % 2) == 0 )
			{	++bm_pre_filter_type;	}

		//if( _bm_preset != _bm_preset_ui )
		//{
		//	_bm->init( _bm_preset_ui, number_disparities, sad_window_size );
		//	_bm_preset = _bm_preset_ui;
		//}
		//_bm->state->trySmallerWindows	= _bm_b_try_smaller_windows_ui;
		////	_bm->state->roi1				= roi1;
		////	_bm->state->roi2				= roi2;
		//_bm->state->preFilterType		= _bm_pre_filter_type_ui;
		//_bm->state->preFilterCap		= _bm_pre_filter_cap_ui;		//31;
		//_bm->state->preFilterSize		= bm_pre_filter_type;
		//_bm->state->SADWindowSize		= sad_window_size;
		//_bm->state->minDisparity		= _bm_min_disparity_ui;			//0;
		//_bm->state->numberOfDisparities	= number_disparities;
		//_bm->state->textureThreshold	= _bm_texture_threshold_ui;		//10;
		//_bm->state->uniquenessRatio		= _bm_uniqueness_ratio_ui;		//15;
		//_bm->state->speckleWindowSize	= _bm_speckle_window_size_ui;	//100;
		//_bm->state->speckleRange		= _bm_speckle_range_ui;			//32;
		//_bm->state->disp12MaxDiff		= 1;

		_bm->setPreFilterType( _bm_pre_filter_type_ui );
		_bm->setPreFilterSize( bm_pre_filter_type );
		_bm->setPreFilterCap( _bm_pre_filter_cap_ui );
		_bm->setTextureThreshold( _bm_texture_threshold_ui );
		_bm->setUniquenessRatio( _bm_uniqueness_ratio_ui );
		_bm->setSmallerBlockSize( _bm_b_try_smaller_windows_ui ? 1 : 3 );
		_bm->setBlockSize( sad_window_size );
		_bm->setDisp12MaxDiff( 1 );
		_bm->setMinDisparity( _bm_min_disparity_ui );
		_bm->setNumDisparities( number_disparities );
		_bm->setSpeckleRange( _bm_speckle_range_ui );
		_bm->setSpeckleWindowSize( _bm_speckle_window_size_ui );

		_bm->compute( *_src_cur, *_src_b_cur, _disp_cpu_dst );	//, _bm_disp_type_ui == CV_STEREO_32BIT_FP ? CV_32F : CV_16S );
		_src_cur = &_disp_cpu_dst;

	//	_bm->operator()( src_a, src_b, disp, _bm_disp_type_ui == CV_STEREO_32BIT_FP ? CV_32F : CV_16S );
		return number_disparities;
	}
	return 0;
}

//INT32	c_bdd_img_cv_stereo::do_disparity_elas( cv::Mat& src_a, cv::Mat& src_b, cv::Mat& disp )
//{
//
//	// get image width and height
//	int32_t width  = src_a.cols;
//	int32_t height = src_a.rows;
//
//	// allocate memory for disparity images
//	const int32_t dims[3] = {width,height,width}; // bytes per line = width
//	float* D1_data = (float*)malloc( width * height * sizeof(float) );
//	float* D2_data = (float*)malloc( width * height * sizeof(float) );
//
//	// process
//	elas_stereo::Elas::parameters param;	//(elas_stereo::Elas::MIDDLEBURY);
//	param.postprocess_only_left = true;
//	elas_stereo::Elas elas( param );
//
//	elas.process( src_a.data, src_b.data, D1_data, D2_data,dims );
//
//	// find maximum disparity for scaling output disparity images to [0..255]
//	float disp_max = 0;
//	float disp_min = 1000000.;
//	for (int32_t i=0; i<width*height; i++) {
//		if (D1_data[i]>disp_max) disp_max = D1_data[i];
//		if (D1_data[i]<disp_min) disp_min = D1_data[i];
////		if (D2_data[i]>disp_max) disp_max = D2_data[i];
//	}
//
//	disp.create( height, width, CV_8UC1 );
//	// copy float to uchar
//	UINT8 *D1 = new UINT8[ width * height ];
//	//image<uchar> *D2 = new image<uchar>(width,height);
//	for (int32_t i=0; i<width*height; i++) {
//		disp.data[i] = (UINT8)max(255.0*D1_data[i]/disp_max,0.0);
////		D2->data[i] = (uint8_t)max(255.0*D2_data[i]/disp_max,0.0);
//	}
//
//	return 255;
//}

//INT32	c_bdd_img_cv_stereo::do_disparity_var( cv::Mat& src_a, cv::Mat& src_b, cv::Mat& disp )
//{
//	if( !_var )
//	{
//		_var = new cv::StereoVar();
//	}
//	if( _var )
//	{
//		cv::Size	img_size = src_a.size();
//		INT32	number_disparities = _vm_min_disp_ui > 0 ? _vm_min_disp_ui : ((img_size.width/8) + 15) & -16;
//
//		_var->levels		= _vm_levels_ui;			//3;			// ignored with USE_AUTO_PARAMS
//		_var->pyrScale		= _vm_pyr_scale_ui;			//0.5;			// ignored with USE_AUTO_PARAMS
//		_var->nIt			= _vm_n_it_ui;				//25;
//		_var->minDisp		= number_disparities;
//		_var->maxDisp		= _vm_max_disp_ui;			//0;
//		_var->poly_n		= _vm_poly_n_ui;			//3;
//		_var->poly_sigma	= _vm_poly_sigma_ui;		//0.0;
//		_var->fi			= _vm_fi_ui;				//15.0f;
//		_var->lambda		= _vm_lambda_ui;			//0.03f;
//		_var->penalization	= _vm_penalization_ui;		// ignored with USE_AUTO_PARAMS
//		_var->cycle			= _vm_cycle_ui;				// ignored with USE_AUTO_PARAMS
//		INT32	flag = 0;
//		if( _vm_b_use_smart_id_ui )
// 			flag |= _var->USE_SMART_ID;
//		if( _vm_b_use_auto_params_ui )
// 			flag |= _var->USE_AUTO_PARAMS;
//		if( _vm_b_use_initial_disparity_ui )
//			flag |= _var->USE_INITIAL_DISPARITY;
//		if( _vm_b_use_median_filtering_ui )
// 			flag |= _var->USE_MEDIAN_FILTERING;
//		if( _vm_b_use_equalize_hist_ui )
// 			flag |= _var->USE_EQUALIZE_HIST;
//		_var->flags			= flag;
//
//
//		//	Mat rgba( 100, 100, CV_8UC4, Scalar(1,2,3,4) );
//		cv::Mat bgr_a( src_a.rows, src_a.cols, CV_8UC3 );
//		cv::Mat bgr_b( src_b.rows, src_b.cols, CV_8UC3 );
//
//		int from_to[] = { 0,0, 1,1, 2,2  };
//		// maybe use cvtcolor
//		cv::mixChannels( &src_a, 1, &bgr_a, 1, from_to, 3 );
//		cv::mixChannels( &src_b, 1, &bgr_b, 1, from_to, 3 );
//
//
//		//cv::Mat rgba( 100, 100, CV_8UC4, Scalar(1,2,3,4) );
//		//cv::Mat bgr( rgba.rows, rgba.cols, CV_8UC3 );
//		//cv::Mat alpha( rgba.rows, rgba.cols, CV_8UC1 );
//
//		//// forming an array of matrices is a quite efficient operation,
//		//// because the matrix data is not copied, only the headers
//		//cv::Mat out[] = { bgr, alpha };
//		//// rgba[0] -> bgr[2], rgba[1] -> bgr[1],
//		//// rgba[2] -> bgr[0], rgba[3] -> alpha[0]
//		//int from_to[] = { 0,2, 1,1, 2,0, 3,3 };
//		//cv::mixChannels( &src_a, 1, out, 2, from_to, 4 );
//
//		//			_var->operator()( src_a, src_b, disp8 );
//		_var->operator()( bgr_a, bgr_b, disp );
//		return number_disparities;
//	}
//	return 0;
//}


//int roundUp(int numToRound, int multiple) 
//{ 
//	if (multiple == 0) 
//		return numToRound; 
//
//	int remainder = abs(numToRound) % multiple;
//	if (remainder == 0)
//		return numToRound;
//	if (numToRound < 0)
//		return -(abs(numToRound) - remainder);
//	return numToRound + multiple - remainder;
//}
INT32	c_bdd_img_cv_stereo::do_disparity_sgbm()
{
	if( _sgbm.empty() )
	{
		_sgbm = cv::StereoSGBM::create( _sgbm_min_disparity_ui, _sgbm_num_disparities_ui, _sgbm_sad_window_size_ui );
	//	_sgbm = cv::createStereoSGBM();
	//	_sgbm = new cv::StereoSGBM();
	}
	if( !_sgbm.empty() )
	{
		cv::Size	img_size = _src_cur->size();
		_sgbm->setPreFilterCap(			_sgbm_pre_filter_cap_ui );		//63;
		INT32 SADWindowSize = _sgbm_sad_window_size_ui > 0 ? _sgbm_sad_window_size_ui : 3;
		_sgbm->setBlockSize(		 SADWindowSize );
//		INT32	number_disparities = _sgbm_num_disparities_ui > 0 ? _sgbm_num_disparities_ui : ((img_size.width/8) + 15) & -16;
		INT32	number_disparities = I_ROUND_CEIL( _sgbm_num_disparities_ui,  16 );
		INT32	cn = _src_cur->channels();
		_sgbm->setP1(					8 * cn * SADWindowSize * SADWindowSize );
		_sgbm->setP2(					32 * cn * SADWindowSize * SADWindowSize );
		_sgbm->setMinDisparity(			_sgbm_min_disparity_ui );		// 0
		_sgbm->setNumDisparities(		number_disparities );
		_sgbm->setUniquenessRatio(		_sgbm_uniqueness_ratio_ui );	//10;
		_sgbm->setSpeckleWindowSize(	_sgbm_speckle_window_size_ui );
		_sgbm->setSpeckleRange(			_sgbm_speckle_range_ui );
		_sgbm->setDisp12MaxDiff(		_sgbm_disp12_max_diff_ui );
		//_sgbm->fullDP				= _sgbm_b_full_dp_ui ? true : false;

		_sgbm->compute( *_src_cur, *_src_b_cur, _disp_cpu_dst );
		_src_cur = &_disp_cpu_dst;
		//_sgbm->operator()( src_a, src_b, disp );
		return number_disparities;
	}
	return 0;
}

#if AAA_LIB_USE_OPENCV_CUDA()
INT32	c_bdd_img_cv_stereo::do_disparity_bm_pgu()
{
	if( _bm_gpu.empty() )
	{
		_bm_gpu = cv::cuda::createStereoBM();
		//_bm_gpu = new cv::cuda::StereoBM();
	}
	if( !_bm_gpu.empty() )
	{
		cv::Size	img_size			= _src_cur_gpu->size();
		INT32		number_disparities	= _bm_num_disparities_ui;
		number_disparities = I_ROUND_CEIL( _bm_num_disparities_ui, 16 );

		INT32	sad_window_size = _bm_sad_window_size_ui > 0 ? _bm_sad_window_size_ui : 9;
		sad_window_size = aaa::MIN( sad_window_size, img_size.width, img_size.height );
		if ( (sad_window_size % 2) == 0 )
		{
			++sad_window_size;
		}

		UINT32	bm_pre_filter_type = _bm_pre_filter_size_ui;
		if ( (bm_pre_filter_type % 2) == 0 )
		{
			++bm_pre_filter_type;
		}

		_bm_gpu->setPreFilterType( _bm_pre_filter_type_ui );
		_bm_gpu->setPreFilterSize( bm_pre_filter_type );
		_bm_gpu->setPreFilterCap( _bm_pre_filter_cap_ui );
		_bm_gpu->setTextureThreshold( _bm_texture_threshold_ui );
		_bm_gpu->setUniquenessRatio( _bm_uniqueness_ratio_ui );
		_bm_gpu->setSmallerBlockSize( _bm_b_try_smaller_windows_ui );	//todo 
		_bm_gpu->setBlockSize( sad_window_size );
		_bm_gpu->setDisp12MaxDiff( 1);
		_bm_gpu->setMinDisparity( _bm_min_disparity_ui );
		_bm_gpu->setNumDisparities( number_disparities );
		_bm_gpu->setSpeckleRange( _bm_speckle_range_ui );
		_bm_gpu->setSpeckleWindowSize( _bm_speckle_window_size_ui );

		_bm_gpu->compute( *_src_cur_gpu, *_src_b_cur_gpu, _disp_gpu_dst );
		_src_cur_gpu = &_disp_gpu_dst;

		//_bm_gpu->preset				= _bm_gpu_preset_ui;
		//_bm_gpu->ndisp				= _bm_gpu_ndisp_ui;
		//_bm_gpu->winSize			= _bm_gpu_win_size_ui;
		//_bm_gpu->avergeTexThreshold = _bm_average_tex_threshold_ui;*/
	//	_bm_gpu->operator()( src_a, src_b, disp );
		return _bm_gpu_ndisp_ui;
	}
	return 0;
}


INT32	c_bdd_img_cv_stereo::do_disparity_bp()
{
	if( _bp_gpu.empty() )
	{
		_bp_gpu = cv::cuda::createStereoBeliefPropagation();
	//	_bp_gpu = new cv::cuda::StereoBeliefPropagation();
	}
	if( !_bp_gpu.empty() )
	{
		// number of disparities,
		// number of BP iterations on each level,
		// number of levels,
		// truncation of data cost,
		// data weight,
		// truncation of discontinuity cost,
		// discontinuity single jump
		//! DataTerm = data_weight * min(fabs(I2-I1), max_data_term)
		//! DiscTerm = min(disc_single_jump * fabs(f1-f2), max_disc_term)
		//_bp_gpu->ndisp				= _bp_gpu_ndisp_ui;
		//_bp_gpu->iters				= _bp_gpu_iters_ui;
		//_bp_gpu->levels				= _bp_gpu_levels_ui;
		//_bp_gpu->max_data_term		= _bp_gpu_max_data_term_ui;
		//_bp_gpu->data_weight		= _bp_gpu_data_weight_ui;
		//_bp_gpu->max_disc_term		= _bp_gpu_max_disc_term_ui;
		//_bp_gpu->disc_single_jump	= _bp_gpu_disc_single_jump_ui;
		//_bp_gpu->msg_type			= _bp_gpu_disp_type_ui == CV_STEREO_32BIT_FP ? CV_32FC1 : CV_16SC1;

		//_bp_gpu->operator()( src_a, src_b, disp );

		_bp_gpu->setNumDisparities( _bp_gpu_ndisp_ui );
		//! number of BP iterations on each level
		_bp_gpu->setNumIters( _bp_gpu_iters_ui );
		//! number of levels
		_bp_gpu->setNumLevels( _bp_gpu_levels_ui );
		//! truncation of data cost
		_bp_gpu->setMaxDataTerm( _bp_gpu_max_data_term_ui );
		//! data weight
		_bp_gpu->setDataWeight( _bp_gpu_data_weight_ui );
		//! truncation of discontinuity cost
		_bp_gpu->setMaxDiscTerm( _bp_gpu_max_disc_term_ui );
		//! discontinuity single jump
		_bp_gpu->setDiscSingleJump( _bp_gpu_disc_single_jump_ui );


		_bp_gpu->setMsgType( _bp_gpu_disp_type_ui == CV_STEREO_32BIT_FP ? CV_32FC1 : CV_16SC1 );

		//_bp_gpu->estimateRecommendedParams( int width, int height, int& ndisp, int& iters, int& levels );

		_bp_gpu->compute( *_src_cur_gpu, *_src_b_cur_gpu, _disp_gpu_dst );
		_src_cur_gpu = &_disp_gpu_dst;
	//	number_disparities = _bp_gpu_ndisp_ui;
		return _bp_gpu_ndisp_ui;
	}
	return 0;
}

INT32	c_bdd_img_cv_stereo::do_disparity_csbp()
{
	if( _csbp_gpu.empty() )
	{
		_csbp_gpu = cv::cuda::createStereoConstantSpaceBP();
		//_csbp_gpu = new cv::cuda::StereoConstantSpaceBP();
	}
	if( !_csbp_gpu.empty() )
	{
		if(_csbp_param_trig_ui )
		{
			int ndisp, iters, levels, nr_plane;
			_csbp_gpu->estimateRecommendedParams( _src_cur_gpu->size().width, _src_cur_gpu->size().height, ndisp, iters, levels, nr_plane );
			_csbp_gpu_ndisp_ui = ndisp;
			_csbp_gpu_ndisp_ui = iters;
			_csbp_gpu_levels_ui = levels;
			_csbp_gpu_nr_plane_ui = nr_plane;
			_csbp_param_trig_ui = false;
		}
		// number of disparities,
		// number of BP iterations on each level,
		// number of levels,
		// number of active disparity on the first level,
		// truncation of data cost,
		// data weight,
		// truncation of discontinuity cost,
		// discontinuity single jump,
		// minimum disparity threshold
		_csbp_gpu->setNumDisparities(		_csbp_gpu_ndisp_ui );
		_csbp_gpu->setNumIters(				_csbp_gpu_iters_ui );
		_csbp_gpu->setNumLevels(			_csbp_gpu_levels_ui );
		_csbp_gpu->setNrPlane(				_csbp_gpu_nr_plane_ui );
	//	_csbp_gpu->max_data_term			_csbp_gpu_max_data_term_ui;
		_csbp_gpu->setDataWeight(			 _csbp_gpu_data_weight_ui );
		_csbp_gpu->setMaxDiscTerm(			_csbp_gpu_max_disc_term_ui );
		_csbp_gpu->setDiscSingleJump(		_csbp_gpu_disc_single_jump_ui );
		//_csbp_gpu->min_disp_th				_csbp_gpu_min_disp_th_ui;
		_csbp_gpu->setMsgType(				_csbp_gpu_disp_type_ui == CV_STEREO_32BIT_FP ? CV_32FC1 : CV_16SC1 );
		_csbp_gpu->setUseLocalInitDataCost(	_csbp_gpu_b_use_local_init_data_cost_ui ? false : true );

		_csbp_gpu->compute( *_src_cur_gpu, *_src_b_cur_gpu, _disp_gpu_dst );
		_src_cur_gpu = &_disp_gpu_dst;
		//_csbp_gpu->operator()( src_a, src_b, disp );

		return _csbp_gpu_ndisp_ui;
	}
	return 0;
}

void	c_bdd_img_cv_stereo::do_bilateral_filter_gpu()
{
	if(		_bilateral_disp_nb_ui != _bilateral_disp_nb
		||	_bilateral_size_ui != _bilateral_size
		||	_bilateral_iterations_ui != _bilateral_iterations
		||	_bilateral_edge_threshold_ui != _bilateral_edge_threshold
		||	_bilateral_max_disc_threshold_ui != _bilateral_max_disc_threshold
		||	_bilateral_sigma_range_ui != _bilateral_sigma_range
		)
	{
		//SAFE_DELETE( _disparity_filter );
		_bilateral_disp_nb				= _bilateral_disp_nb_ui;
		_bilateral_size					= _bilateral_size_ui;
		_bilateral_iterations			= _bilateral_iterations_ui;
		_bilateral_edge_threshold		= _bilateral_edge_threshold_ui;
		_bilateral_max_disc_threshold	= _bilateral_max_disc_threshold_ui;
		_bilateral_sigma_range			= _bilateral_sigma_range_ui;
	}

	if( _disparity_filter.empty() )
	{
		_disparity_filter = cv::cuda::createDisparityBilateralFilter(  _bilateral_disp_nb, _bilateral_size, _bilateral_iterations );
		//_disparity_filter = new cv::cuda::DisparityBilateralFilter( _bilateral_disp_nb, _bilateral_size, _bilateral_iterations, _bilateral_edge_threshold, _bilateral_max_disc_threshold, _bilateral_sigma_range );
	}
	if ( !_disparity_filter.empty() )
	{
		_disparity_filter->setNumDisparities(	_bilateral_disp_nb				);
		_disparity_filter->setRadius(			 _bilateral_size				);
		_disparity_filter->setNumIters(			_bilateral_iterations			);
		_disparity_filter->setEdgeThreshold(	_bilateral_edge_threshold		);
		_disparity_filter->setMaxDiscThreshold(	_bilateral_max_disc_threshold	);
		_disparity_filter->setSigmaRange(		_bilateral_sigma_range			);
		_disparity_filter->apply(				_disp_gpu_dst, *_src_cur_gpu, _disp_filter_gpu_dst );
		//_disparity_filter->operator()( _disp_gpu_dst, _gpu_dst_cur, _disp_filter_gpu_dst );
	}
	_src_cur_gpu = &_disp_filter_gpu_dst;
}
#endif

bool	c_bdd_img_cv_stereo::do_stereo_disparity()
{
	TBUF_ADD_OBJ( tbuf::CH_OPEN_CV, 1., "bdd_img_cv_stereo", this );
	//cv::Rect	roi1, roi2;
	//cv::Size	img_size = src_a.size();
	//cv::Mat	src_cur_a = src_a;
	//cv::Mat	src_cur_b = src_b;
	
	UINT32	number_disparities = 0;
	// GPU methods
#if AAA_LIB_USE_OPENCV_CUDA()
	if( is_gpu_use() )
	{
		//if( c_aaa_cv::is_cuda_use() )
		//{
		//	cv::cuda::device::bindTexture(  1 , _gpumat_src_a.operator cv::cuda::PtrStepSz<UINT8>);
		//	_src_a_gpu_dst.upload( src_a );
		//	_src_b_gpu_dst.upload( src_b );
		//	cv::cuda::GpuMat	gpumat_src_a = _src_a_gpu_dst;
		//	cv::cuda::GpuMat	gpumat_src_b = _src_b_gpu_dst;

			_speed_stereo->begin();
			if (_s_stereo_algo_ui == STEREO_BM_GPU)
			{
				number_disparities = do_disparity_bm_pgu();
			}
			else if (_s_stereo_algo_ui == STEREO_BP)
			{
				number_disparities = do_disparity_bp();
			}
			else if (_s_stereo_algo_ui == STEREO_CSBP)
			{
				number_disparities = do_disparity_csbp();
			}
			_speed_stereo->end();
			if( _b_bilateral_filter_ui )
			{
				_speed_bilateral->begin();
				do_bilateral_filter_gpu();
				_speed_bilateral->end();
			}
			if( _b_normalize_ui )
			{
				_speed_normalize->begin();
				//Normalize the result so we can display it
				cv::cuda::normalize( *_src_cur_gpu, _normalize_gpu_dst, .0, 255., cv::NORM_MINMAX, -1 );//, cv::cuda::GpuMat(), cv::cuda::GpuMat(), cv::cuda::GpuMat() );
				_src_cur_gpu = &_normalize_gpu_dst;
				_speed_normalize->end();
			}
			if ( _b_threshold_ui )
			{
				_speed_threshold->begin();
				cv::cuda::threshold( *_src_cur_gpu, _thresh_max_gpu_dst, (DOUBLE)_threshold_max_ui, 255.0, cv::THRESH_TOZERO_INV );
				cv::cuda::threshold( _thresh_max_gpu_dst, _thresh_min_gpu_dst, _threshold_min_ui, 255.0, cv::THRESH_TOZERO );
				_src_cur_gpu = &_thresh_min_gpu_dst;
				if( _b_threshold_norm_ui )
				{
					cv::cuda::normalize( *_src_cur_gpu, _normalize_gpu_dst, .0, 255., cv::NORM_MINMAX, -1 );//, cv::cuda::GpuMat(), cv::cuda::GpuMat(), cv::cuda::GpuMat() );
					_src_cur_gpu = &_normalize_gpu_dst;
				}
				_speed_threshold->end();
			}
			if( _b_colored_disp_ui )
			{
				_speed_color->begin();
				cv::cuda::drawColorDisp( *_src_cur_gpu, _disp_color_gpu_dst, number_disparities );
				_src_cur_gpu = &_disp_color_gpu_dst;
				_speed_color->end();
			}
			if( _b_resize_ui && is_diff_v2( _resize_size_ui, REAL(1.) ) )
			{
				cv::cuda::resize( *_src_cur_gpu, _resize_gpu_dst, cv::Size( _resized_size[0], _resized_size[1] ) );
				_src_cur_gpu = &_resize_gpu_dst;
			}

			//_gpu_dst_cur.download( dst );
		//}
		//else
		//{
		//	ERR_PRINT_STRING( "Error No CUDA device to execute disparity on GPU" );
		//	goto exit;
		//}
	}
	else
#endif
	{
		_speed_stereo->begin();
		if( _s_stereo_algo_ui == STEREO_BM )
		{
			number_disparities = do_disparity_bm();
		}
		// way too slow
		//else if( _s_stereo_algo_ui == STEREO_VAR )
		//{
		//	number_disparities = do_disparity_var( src_cur_a, src_cur_b, _disp_cpu );
		//}
		else if( _s_stereo_algo_ui == STEREO_SGBM )
		{
			number_disparities = do_disparity_sgbm();
		}
		// disable for now but good for hi res image
		//else if( _s_stereo_algo_ui == STEREO_ELAS )
		//{
		//	number_disparities = do_disparity_elas();
		//}

		_speed_speckles->begin();
		if( _b_speckle_filter_ui )
		{
			cv::filterSpeckles( _disp_cpu_dst, _speckle_filter_value_ui*16., _speckle_filter_max_size_ui, _speckle_filter_max_diff_ui*16 );
		}
		_speed_speckles->end();
		_src_cur = &_disp_cpu_dst;

		_disp_cpu_dst.convertTo( _disp8_cpu_dst, CV_8U, 255/(number_disparities*16.) );
		_src_cur = &_disp8_cpu_dst;

		_speed_stereo->end();
		if( _b_normalize_ui )
		{
			_speed_normalize->begin();
			//Normalize the result so we can display it
			cv::normalize( *_src_cur, _normalize_cpu_dst, 0, 255, cv::NORM_MINMAX, CV_8U );			// or disp ?
			_src_cur = &_normalize_cpu_dst;
			_speed_normalize->end();
		}
		if ( _b_threshold_ui )
		{
			_speed_threshold->begin();
			cv::threshold( *_src_cur, _thresh_max_cpu_dst, (DOUBLE)_threshold_max_ui, 255.0, cv::THRESH_TOZERO_INV );
			cv::threshold( _thresh_max_cpu_dst, _thresh_min_cpu_dst, _threshold_min_ui, 255.0, cv::THRESH_TOZERO );
			_src_cur = &_thresh_min_cpu_dst;
			if( _b_threshold_norm_ui )
			{
				cv::normalize( *_src_cur, _normalize_cpu_dst, .0, 255., cv::NORM_MINMAX, -1 );//, cv::cuda::GpuMat(), cv::cuda::GpuMat(), cv::cuda::GpuMat() );
				_src_cur = &_normalize_cpu_dst;
			}
			_speed_threshold->end();
		}
		//else
		//{
		//	_disp_cur.copyTo( dst );
		//}
		if( _b_resize_ui && is_diff_v2( _resize_size_ui, REAL(1.) ) )
		{
			cv::resize( *_src_cur, _resize_cpu_dst, cv::Size( _resized_size[0], _resized_size[1] ) );
			_src_cur = &_resize_cpu_dst;
		}
		//_disp_dst_cur.copyTo( dst );
	}

	_speed_inpaint->begin();
	if( _b_inpaint_ui )
	{
		//use a smaller version of the image
#if AAA_LIB_USE_OPENCV_CUDA()
		if( is_gpu_use() )
		{
			_src_cur_gpu->download( _inpaint_src );
			_b_src_is_gpu = false;
			cv::resize( _inpaint_src, _inpaint_resize_1, cv::Size(), _inpaint_size_x_ui, _inpaint_size_y_ui );
		}
		else
#endif
		{
			cv::resize( *_src_cur, _inpaint_resize_1, cv::Size(), _inpaint_size_x_ui, _inpaint_size_y_ui );	
		}
		//inpaint only the "unknown" pixels
		cv::inpaint( _inpaint_resize_2, (_inpaint_resize_2 == _inpaint_no_depth_ui), _inpaint_dst, 5.0,cv::INPAINT_TELEA );

		cv::resize( _inpaint_dst, _inpaint_resize_2, _src_cur->size());
#if AAA_LIB_USE_OPENCV_CUDA()
		if ( is_gpu_use() )
		{
			_inpaint_resize_2.copyTo( *_src_cur, (_inpaint_src == _inpaint_no_depth_ui) );  //add the original signal back over the inpaint
		}
		else
#endif
		{
			_inpaint_resize_2.copyTo( *_src_cur, (_src_cur->getMat( cv::ACCESS_RW ) == _inpaint_no_depth_ui) );  //add the original signal back over the inpaint
		}
	}
	_speed_inpaint->end();
	update_dst_index( _dst_img_index_ui, "Disparity" );

//	exit:
	TBUF_ADD_OBJ( tbuf::CH_OPEN_CV, 0., nullptr, this );
	return true;
}

