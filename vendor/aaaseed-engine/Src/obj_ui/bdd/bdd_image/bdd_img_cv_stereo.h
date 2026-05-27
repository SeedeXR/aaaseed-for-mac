
#ifdef AAA_BDD_IMG_CV_STEREO_H
#error "BDD_IMG_CV_STEREO_H included more than once."
#endif
#define AAA_BDD_IMG_CV_STEREO_H 1


#ifndef AAA_BDD_IMG_H
#	include "obj_ui/bdd/bdd_image/bdd_img.h"
#endif
#ifndef OPENCV_CALIB3D_HPP
#	include "opencv2/calib3d/calib3d.hpp"
#endif
#if AAA_LIB_USE_OPENCV_CUDA()
#	ifndef __OPENCV_CUDASTEREO_HPP__
#		include "opencv2/cudastereo.hpp"
#	endif
#endif

class c_speed;

class	c_bdd_img_cv_stereo final : public c_bdd_img
{
	FACTORY_DECLARE( c_bdd_img_cv_stereo, c_bdd_img );
public:
	enum CV_STEREO_TYPE : INT32
	{
		STEREO_BM = 0,
		STEREO_BM_GPU,
		STEREO_CSBP,
		STEREO_BP,
		STEREO_SGBM,
//		STEREO_ELAS,
//		STEREO_VAR,
//		STEREO_GC,
		STEREO_TYPE_MAX
	};

	static	C_PCHAR_C	stereo_type_str[STEREO_TYPE_MAX];

	enum CV_VM_PENALIZATION : INT32
	{
		PENALIZATION_TICHONOV = 0,
		PENALIZATION_CHARBONNIER,
		PENALIZATION_PERONA_MALIK,
		PENALIZATION_MAX
	};
	static	C_PCHAR_C	vm_penalization_str[PENALIZATION_MAX];

	enum CV_VM_CYCLE : INT32
	{
		CYCLE_O = 0,
		CYCLE_V,
		CYCLE_MAX
	};
	static	C_PCHAR_C	vm_cycle_str[CYCLE_MAX];

	enum	CV_STEREOBM_CAMERA_TYPE : INT32
	{
		CV_STEREOBM_BASIC=0,
		CV_STEREOBM_FISH_EYE,
		CV_STEREOBM_NARROW,
		CV_STEREOBM_CAMERA_MAX
	};
	static	C_PCHAR_C	stereobm_str[CV_STEREOBM_CAMERA_MAX];

	enum	CV_STEREOBM_PREFILTER_TYPE : INT32
	{
		CV_STEREOBM_NORMALIZED_RESPONSE=0,
		CV_STEREOBM_XSOBEL,
		CV_STEREOBM_PREFILTER_MAX
	};
	static	C_PCHAR_C	stereobm_prefilter_str[CV_STEREOBM_PREFILTER_MAX];
	
	enum	CV_STEREOBMGPU_PRESET_TYPE : INT32
	{
		CV_STEREOBMGPU_BASIC=0,
		CV_STEREOBMGPU_XSOBEL,
		CV_STEREOBMGPU_PRESET_MAX
	};
	static	C_PCHAR_C	stereobmgpu_preset_str[CV_STEREOBMGPU_PRESET_MAX];

	enum	CV_STEREO_ARITHMETIC_TYPE : INT32
	{
		CV_STEREO_16BIT_SIGNED=0,
		CV_STEREO_32BIT_FP,
		CV_STEREO_ARITHMETIC_MAX
	};
	static	C_PCHAR_C	stereo_arithmetic_str[CV_STEREO_ARITHMETIC_MAX];

protected:
//	bool		_b_swap_src;
	INT32		_src_b_img_index;
	UINT32		_src_b_size_x;
	UINT32		_src_b_size_y;

	
	c_speed*	_speed_bdd;
	c_speed*	_speed_stereo;
	c_speed*	_speed_bilateral;
	c_speed*	_speed_normalize;
	c_speed*	_speed_threshold;
	c_speed*	_speed_speckles;
	c_speed*	_speed_inpaint;
	c_speed*	_speed_color;

//	cv::UMat		_mat_src_a;
//	cv::UMat		_mat_src_b;

	//cv::UMat		_mat_dst;

	bool		_b_normalize_ui;

	bool		_b_threshold_ui;
	UINT32		_threshold_min_ui;
	UINT32		_threshold_max_ui;
	bool		_b_threshold_norm_ui;

	bool		_b_speckle_filter_ui;
	UINT32		_speckle_filter_value_ui;
	UINT32		_speckle_filter_max_size_ui;
	UINT32		_speckle_filter_max_diff_ui;

	bool		_b_inpaint_ui;
	REAL		_inpaint_size_x_ui;
	REAL		_inpaint_size_y_ui;
	INT32		_inpaint_no_depth_ui;

	bool		_b_resize_ui;
	REAL		_resize_size_ui[2];
	UINT32		_resized_size[2];

	bool		_b_bilateral_filter_ui;
	INT32		_bilateral_disp_nb_ui;
	INT32		_bilateral_size_ui;
	INT32		_bilateral_iterations_ui;
	REAL		_bilateral_edge_threshold_ui;
	REAL		_bilateral_max_disc_threshold_ui;
	REAL		_bilateral_sigma_range_ui;
	INT32		_bilateral_disp_nb;
	INT32		_bilateral_size;
	INT32		_bilateral_iterations;
	REAL		_bilateral_edge_threshold;
	REAL		_bilateral_max_disc_threshold;
	REAL		_bilateral_sigma_range;

	bool		_b_colored_disp_ui;

	// Block Matching
	UINT32		_bm_disp_type_ui;
	UINT32		_bm_preset;
	UINT32		_bm_preset_ui;
	// pre-filtering (normalization of input images)
	UINT32		_bm_pre_filter_type_ui;
	UINT32		_bm_pre_filter_size_ui;	// averaging window size: ~5x5..21x21
	UINT32		_bm_pre_filter_cap_ui;	// the output of pre-filtering is clipped by [-preFilterCap,preFilterCap]
	// correspondence using Sum of Absolute Difference (SAD)
	UINT32		_bm_sad_window_size_ui;		// ~5x5..21x21
	INT32		_bm_min_disparity_ui;		// minimum disparity (can be negative)
	INT32		_bm_num_disparities_ui;		// maximum disparity - minimum disparity (> 0)
	// post-filtering
	UINT32		_bm_texture_threshold_ui;	// the disparity is only computed for pixels with textured enough neighborhood
	UINT32		_bm_uniqueness_ratio_ui;	// accept the computed disparity d* only if
								// SAD(d) >= SAD(d*)*(1 + uniquenessRatio/100.)
								// for any d != d*+/-1 within the search range.
	UINT32		_bm_speckle_window_size_ui;	// disparity variation window
	UINT32		_bm_speckle_range_ui;			// acceptable range of variation in window
	bool		_bm_b_try_smaller_windows_ui;	// if 1, the results may be more accurate, at the expense of slower processing 

	// Semi-Global Block Matching
	INT32		_sgbm_min_disparity_ui;
	INT32		_sgbm_num_disparities_ui;
	INT32		_sgbm_sad_window_size_ui;
	INT32		_sgbm_pre_filter_cap_ui;
	INT32		_sgbm_uniqueness_ratio_ui;
	INT32		_sgbm_speckle_window_size_ui;
	INT32		_sgbm_speckle_range_ui;
	INT32		_sgbm_disp12_max_diff_ui;
	bool		_sgbm_b_full_dp_ui;

	//// Variational Matching
	//INT32		_vm_levels_ui;
	//REAL		_vm_pyr_scale_ui;
	//INT32		_vm_n_it_ui;
	//INT32		_vm_min_disp_ui;
	//INT32		_vm_max_disp_ui;
	//INT32		_vm_poly_n_ui;
	//REAL		_vm_poly_sigma_ui;
	//REAL		_vm_fi_ui;
	//REAL		_vm_lambda_ui;
	//INT32		_vm_penalization_ui;
	//INT32		_vm_cycle_ui;
	//bool		_vm_b_use_smart_id_ui;
	//bool		_vm_b_use_auto_params_ui;
	//bool		_vm_b_use_initial_disparity_ui;
	//bool		_vm_b_use_median_filtering_ui;
	//bool		_vm_b_use_equalize_hist_ui;

	// Constant Space Belief Propagation GPU
	INT32		_csbp_gpu_ndisp_ui;
	INT32		_csbp_gpu_iters_ui;
	INT32		_csbp_gpu_levels_ui;
	INT32		_csbp_gpu_nr_plane_ui;
	REAL		_csbp_gpu_max_data_term_ui;
	REAL		_csbp_gpu_data_weight_ui;
	REAL		_csbp_gpu_max_disc_term_ui;
	REAL		_csbp_gpu_disc_single_jump_ui;
	INT32		_csbp_gpu_min_disp_th_ui;
	UINT32		_csbp_gpu_disp_type_ui;
	bool		_csbp_gpu_b_use_local_init_data_cost_ui;
	bool		_csbp_param_trig_ui;

	// Efficient Belief Propagation
	INT32		_bp_gpu_ndisp_ui;
	INT32		_bp_gpu_iters_ui;
	INT32		_bp_gpu_levels_ui;
	REAL		_bp_gpu_max_data_term_ui;
	REAL		_bp_gpu_data_weight_ui;
	REAL		_bp_gpu_max_disc_term_ui;
	REAL		_bp_gpu_disc_single_jump_ui;
	UINT32		_bp_gpu_disp_type_ui;

	// Block Matching GPU
	INT32		_bm_gpu_preset_ui;
	INT32		_bm_gpu_ndisp_ui;
	INT32		_bm_gpu_win_size_ui;
	// If avergeTexThreshold  == 0 => post procesing is disabled
	// If avergeTexThreshold != 0 then disparity is set 0 in each point (x,y) where for left image
	// SumOfHorizontalGradiensInWindow(x, y, winSize) < (winSize * winSize) * avergeTexThreshold
	// i.e. input left image is low textured.
	REAL		_bm_average_tex_threshold_ui;

	UINT32								_s_stereo_algo_ui;
	UINT32								_s_stereo_algo;
	bool								_b_gpu;

	cv::Ptr<cv::StereoBM>				_bm;
	cv::Ptr<cv::StereoSGBM>				_sgbm;
//	cv::StereoVar*						_var;
#if AAA_LIB_USE_OPENCV_CUDA()
	cv::Ptr<cv::cuda::StereoConstantSpaceBP>	_csbp_gpu;
	cv::Ptr<cv::cuda::StereoBeliefPropagation>	_bp_gpu;
	cv::Ptr<cv::cuda::StereoBM>					_bm_gpu;

//	cv::cuda::GpuMat					_gpu_dst_cur;
//	cv::cuda::GpuMat					_src_a_gpu_dst;
//	cv::cuda::GpuMat					_src_b_gpu_dst;
	cv::cuda::GpuMat					_disp_gpu_dst;
	cv::cuda::GpuMat					_disp_color_gpu_dst;
	cv::cuda::GpuMat					_disp_filter_gpu_dst;
	cv::cuda::GpuMat					_thresh_max_gpu_dst;
	cv::cuda::GpuMat					_thresh_min_gpu_dst;
	cv::cuda::GpuMat					_normalize_gpu_dst;
	cv::cuda::GpuMat					_resize_gpu_dst;
#endif
	cv::UMat		_disp_cpu_dst;
	cv::UMat		_disp8_cpu_dst;
	cv::UMat		_resize_cpu_dst;
	cv::UMat		_normalize_cpu_dst;
	cv::UMat		_thresh_max_cpu_dst;
	cv::UMat		_thresh_min_cpu_dst;

	cv::Mat			_inpaint_src;
	cv::Mat			_inpaint_resize_1;
	cv::Mat			_inpaint_dst;
	cv::Mat			_inpaint_resize_2;

	//cv::UMat		_disp_dst_cur;
	//cv::UMat		_disp8_dst_cur;
#if AAA_LIB_USE_OPENCV_CUDA()
	cv::Ptr<cv::cuda::DisparityBilateralFilter>	_disparity_filter;		// crash when releasing, OpenCV Bug
#endif

private:
			void	close_mat();
	//		bool	update_mat_src( cv::UMat& src_a, cv::UMat& src_b );
			void	close();
			void	do_process();
			bool	do_stereo_disparity();
#if AAA_LIB_USE_OPENCV_CUDA()
			void	do_bilateral_filter_gpu();
#endif
			INT32	do_disparity_bm();
//			INT32	do_disparity_elas( cv::Mat& src_a, cv::Mat& src_b, cv::Mat& dst );
//			INT32	do_disparity_var( cv::Mat& src_a, cv::Mat& src_b, cv::Mat& dst );
			INT32	do_disparity_sgbm();
#if AAA_LIB_USE_OPENCV_CUDA()
			INT32	do_disparity_bm_pgu();
			INT32	do_disparity_bp();
			INT32	do_disparity_csbp();
#endif


public:
			void	init();
	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw();
	virtual bool	can_implicit() final override { return true; }
};
