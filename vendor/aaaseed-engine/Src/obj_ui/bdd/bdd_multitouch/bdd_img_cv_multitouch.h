
#ifdef AAA_BDD_IMG_CV_MULTITOUCH_H
#error "BDD_IMG_CV_MULTITOUCH_H included more than once."
#endif
#define AAA_BDD_IMG_CV_MULTITOUCH_H 1


#ifndef AAA_BDD_IMG_H
#	include "obj_ui/bdd/bdd_image/bdd_img.h"
#endif

class	c_bdd_img_cv_multitouch final : public c_bdd_img
{
	FACTORY_DECLARE( c_bdd_img_cv_multitouch, c_bdd_img );
private:

public:

protected:
	bool				_b_invert_active;
	INT32				_invert_image_dst;
	cv::UMat			_invert_dst;
#if AAA_LIB_USE_OPENCV_CUDA()
	cv::cuda::GpuMat	_invert_gpu_dst;
#endif

	bool				_b_smooth_active;
	aaa::c_cv::STATUS	_smooth_status;
	INT32				_smooth_image_dst;
	cv::UMat			_smooth_dst;
#if AAA_LIB_USE_OPENCV_CUDA()
	cv::cuda::GpuMat	_smooth_gpu_dst;
#endif
	INT32				_s_smooth;
	INT32				_smooth_size_x;
	INT32				_smooth_size_y;
//	bool				_b_smooth_8bit;

	bool				_b_smooth2_active;
	aaa::c_cv::STATUS	_smooth2_status;
	INT32				_smooth2_image_dst;
	cv::UMat			_smooth2_dst;
#if AAA_LIB_USE_OPENCV_CUDA()
	cv::cuda::GpuMat	_smooth2_gpu_dst;
#endif
	INT32				_s_smooth2;
	INT32				_smooth2_size_x;
	INT32				_smooth2_size_y;
//	bool				_b_smooth2_8bit;

	bool		_b_diff_active;
	INT32		_s_diff_method;

	bool		_b_remanence_active;
	bool		_b_remanence_max_maa;
	INT32		_remanence_image_dst;

	bool		_b_remanence_ref_in_bind;
//	INT32		_remanence_accumulate_tick;
	INT32		_remanence_ref_bind;
	UINT32		_remanence_ref_bind_unique_id_last	{0};
	cv::UMat	_remanence_ref;
	c_img_2d*	_remanence_img_last;
	c_img_2d*	_remanence_img_last_last;
	//c_img_2d*	_remanence_img_write;
//	c_img_2d*	_remanence_img_tick;
	cv::UMat	_remanence_img_ref;
	INT32		_remanence_max_found;
//	c_img_2d*	_remanence_max_ref_maa;

	bool		_b_agitation_active;
	REAL		_agitation_out;
	REAL		_grey_average_out;
	c_img_2d*	_agitation_img_last;

	cv::UMat			_remanence_dst;
	cv::UMat			_remanence_data;
	cv::UMat			_remanence_buf;
#if AAA_LIB_USE_OPENCV_CUDA()	//we do diff using remanence diff hack we 
	cv::cuda::GpuMat	_remanence_gpu_dst;
	cv::cuda::GpuMat	_remanence_gpu_buf;
#endif
	o_str		_remanence_filename_ui;
//	o_str		_remanence_filename;
//todo fucked up by Jean
	bool		_b_remanence_save_trig;

	REAL		_remanence_time;
	bool		_b_remanence_accumulate;
	bool		_b_remanence_accumulate_min;
	bool		_b_remanence_accumulate_max;
//	bool		_b_remanence_color;
	bool		_b_remanence_diff;
	INT32		_s_remanence_diff_method;
	bool		_b_remanence_restart_trig_ui;
	REAL		_remanence_diff_min;
	REAL		_remanence_diff_factor;
	REAL		_time_got_frame;
	REAL		_time_got_frame_last;

	bool		_b_background_active;
	INT32		_background_image_dst;
	cv::UMat	_back_dst;
	bool		_b_background_reset_trig;
	bool		_b_background_absolute;
	cv::UMat	_back_ref;
	cv::UMat	_back_for_div;
	bool		_b_background_divide;
	REAL		_background_divide_add;
	REAL		_background_divide_factor;

	bool		_b_laplace_active;
	INT32		_laplace_image_dst;
	cv::UMat	_laplace_dst;
	cv::UMat	_laplace_float;
	INT32		_laplace_size;

	bool		_b_sobel_active;
	INT32		_sobel_image_dst;
	cv::UMat	_sobel_dst;
	INT32		_sobel_dx;
	INT32		_sobel_dy;
	INT32		_sobel_aperture;

	bool		_b_erode_first;
	bool		_b_dilate_active;
	INT32		_dilate_image_dst;
	INT32		_dilate_size;
	bool		_b_erode_active;
	INT32		_erode_image_dst;
	INT32		_erode_size;

	cv::UMat	_dilate_dst;
	cv::UMat	_erode_dst;

	// Brightness filter
	bool		_b_bc_active;
	INT32		_bc_image_dst;
	cv::UMat	_bc_dst;
	REAL		_bc_min_ui;
	REAL		_bc_max_ui;

	bool		_b_bc_out_active;
	INT32		_bc_out_image_dst;
	cv::UMat	_bc_out_dst;
	REAL		_bc_out_min_ui;
	REAL		_bc_out_max_ui;

	REAL		_bc_min;
	REAL		_bc_max;
	cv::Mat		_bc_lutmat;

	cv::UMat	_hp_dst;
	bool		_b_highpass_active;
	INT32		_highpass_image_dst;

	bool		_b_rectify_active;
	INT32		_rectify_image_dst;
	cv::UMat	_rectify_dst;

	INT32		_background_threshold;
	bool		_b_simple_highpass;
	INT32		_simple_highpass_smooth_method;
	INT32		_simple_highpass_smooth_size;
	INT32		_simple_highpass_noise_method;
	INT32		_simple_highpass_noise_size;
	bool		_b_highpass_erode_dilate;
	INT32		_highpass_filter;
	INT32		_highpass_scale;
	REAL		_rectify_level;
	bool		_b_rectify_auto_trig;
	REAL		_rectify_max_level;
	bool		_b_rectify_to_max;
	bool		_b_rectify_inverse;

	//HighPass filter
	cv::Mat		_hp_element;
	cv::Mat		_hp_element2;

	cv::UMat	_hp_outra;
	cv::UMat	_hp_outra2;
	cv::UMat	_hp_buffer;


	//Background filter
	//	bool		_back_ownsImage;
	//	INT32		_back_updateThreshold;		// anything above this threshold is considered a 'press' and not part of the background
	//	INT32		_back_count;
	//	INT32		_back_currentRow;
	//	CvPoint*	_back_polyMask;
	//	INT32		_back_nPolyMask;
	
	bool						_b_optflow_active;
	INT32						_optflow_image_dst;
	REAL						_optflow_pyr_scale;
	INT32						_optflow_levels;
	INT32						_optflow_winsize;
	INT32						_optflow_iters;
	INT32						_optflow_poly_n_ui;
	REAL						_optflow_poly_sigma;
	bool						_b_optflow_initial_flow_ui;
	bool						_b_optflow_gaussian;
	bool						_b_optflow_convert;
	REAL						_time_got_frame_imgs;
	REAL						_time_got_frame_last_imgs;
	//std::vector< cv::UMat* >	_imgs_prev;
	cv::UMat					_img_prev;
	cv::UMat					_optflow_dst;
#if AAA_LIB_USE_OPENCV_CUDA()
	cv::cuda::GpuMat			_img_prev_gpu;
	cv::cuda::GpuMat			_optflow_gpu_dst;
#endif

private:
			bool		update_img_ref();
	FINLINE	c_img_ipl*	become_cur( c_img_ipl* src, INT32 index );
			void		do_save_remanence( C_PCHAR_C filename );

public:
	//	void	background_set_mask (void * vaPoints, int xGrid, int yGrid );
	
	void	store_img_prev();
	void	do_optical_flow();
	void	do_invert();
#if AAA_LIB_USE_OPENCV_CUDA()
	void	do_smooth(		INT32 s_smooth, INT32 sx, INT32 sy, aaa::c_cv::STATUS* status, cv::UMat* dst, cv::cuda::GpuMat* gpu_dst, INT32 image_dst  );
#else
	void	do_smooth(		INT32 s_smooth, INT32 sx, INT32 sy, aaa::c_cv::STATUS* status, cv::UMat* dst, INT32 image_dst  );
#endif
	void	do_remanence();
	void	bc_update_lut(	REAL min,			REAL max );
	void	do_bc(			cv::UMat* dst,		REAL min,	REAL max );
	void	do_diff();
	void	do_remanence_max_maa();
	void	do_remanence_base();
	void	do_agitation();
	void	do_rectify();
	void	do_background();
	void	do_highpass();
	void	do_laplace();
	void	do_sobel();
	void	do_dilate();
	void	do_erode();
	void	do_process();

	void	dealloc();

public:
			void	init();
	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw();
	virtual bool	can_implicit() final override { return true; }
};
