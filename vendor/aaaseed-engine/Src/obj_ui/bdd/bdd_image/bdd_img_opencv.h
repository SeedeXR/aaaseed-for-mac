
#ifdef AAA_BDD_IMG_OPENCV_H
#error "BDD_IMG_OPENCV_H included more than once."
#endif
#define AAA_BDD_IMG_OPENCV_H 1


#ifndef AAA_BDD_IMG_H
#	include "obj_ui/bdd/bdd_image/bdd_img.h"
#endif

class c_speed;
class c_speed_ui;
/**
 *  @ingroup OpenCV
 * \class c_bdd_img_opencv
 * \brief Implementation of multiple OpenCV processes in one bdd
 *
 *  \note It inherits from c_bdd_img and uses its update_mat_src() 
 *  function to handle the cv::Mat and cv::UMat types from opencv
 *  ........

 *  \note Update to opencv4.1 is a work in progress
 *

 *  \param This is a param comment...
 */

class c_opencv_pass
{
public:
	bool				_b_active_ui;
	aaa::c_cv::STATUS	_status;
	c_speed*			_speed;
	c_speed_ui*			_speed_ui;
	INT32				_dst_index_ui;
	c_bdd_img*			_owner;

	cv::UMat			_dst;
#if AAA_LIB_USE_OPENCV_CUDA()
	cv::cuda::GpuMat	_dst_gpu;
#endif
	o_str				_name;

public:
	c_opencv_pass();
	~c_opencv_pass();

	bool	is_todo()
	{	
		_status = _b_active_ui ? aaa::c_cv::STATUS::STARTED : aaa::c_cv::STATUS::NO;
		return _b_active_ui;
	}

	void	init( c_bdd_img* owner, C_PCHAR_C name );

	void	begin_param_init(	INT32& h );
	void	end_param_init(		INT32& h );

	void	begin_process();
	void	end_process();
};

class	c_bdd_img_opencv final : public c_bdd_img
{
	FACTORY_DECLARE( c_bdd_img_opencv, c_bdd_img );
public:
private:
	c_opencv_pass*		pass_cur;	
protected:
	cv::UMat			_mat_dst_a;

	c_speed*			_speed_bdd;
	c_speed_ui*			_speed_bdd_ui;

// Convert
	c_opencv_pass		_pass_convert;
	UINT32				_convert_type_ui;

// Equalize Histogram
	c_opencv_pass		_pass_hist;
	cv::UMat			_hist4_dst;
#if AAA_LIB_USE_OPENCV_CUDA()
	cv::cuda::GpuMat	_hist4_gpu_dst;
#endif

// Denoise
	c_opencv_pass		_pass_denoise;
	REAL				_denoise_strenght_ui;
	UINT32				_denoise_template_ui;
	UINT32				_denoise_windows_ui;
//	cv::cuda::FastNonLocalMeansDenoising	_gpu_denoising;

//	Bilateral Filter
	c_opencv_pass		_pass_bilateral;
	INT32				_bilateral_size_ui;
	REAL				_bilateral_sigma_color_ui;
	REAL				_bilateral_sigma_space_ui;

//	BLur
	c_opencv_pass		_pass_blur;
	UINT32				_s_blur_method_ui;
	UINT32				_blur_size_x_ui;
	UINT32				_blur_size_y_ui;

//	Brightness Contrast
	c_opencv_pass		_pass_bc;
	REAL				_bc_min_ui;
	REAL				_bc_max_ui;
//	Brightness Contrast 2
	c_opencv_pass		_pass_bc_2;
	REAL				_bc_2_min_ui;
	REAL				_bc_2_max_ui;

//	Invert
	c_opencv_pass		_pass_invert;

//	Canny
	c_opencv_pass		_pass_edge;
	bool				_b_laplacian_ui;
	UINT32				_edge_size_ui;
	INT32				_sobel_order_x_ui;
	INT32				_sobel_order_y_ui;
	REAL				_edge_offset_ui;
	REAL				_edge_scale_ui;

//	Canny
	c_opencv_pass		_pass_canny;
	REAL				_canny_threshold_min_ui;
	REAL				_canny_threshold_max_ui;
	UINT32				_canny_size_ui;
	bool				_b_l2gradient_ui;

//	Dilate
	bool				_b_dilate_first_ui;
	c_opencv_pass		_pass_dilate;
	UINT32				_dilate_size_x_ui;
	UINT32				_dilate_size_y_ui;
	UINT32				_dilate_type_ui;
	UINT32				_dilate_iteration_ui;

//	Erode
	c_opencv_pass		_pass_erode;
	UINT32				_erode_size_x_ui;
	UINT32				_erode_size_y_ui;
	UINT32				_erode_type_ui;
	UINT32				_erode_iteration_ui;

//	optical flow
	c_opencv_pass		_pass_optflow;
	REAL				_time_got_frame_imgs;
	REAL				_time_got_frame_imgs_last;
	//std::vector< cv::UMat* >	_imgs_prev;
	cv::UMat			_img_prev;
//	cv::UMat			_optflow_buf;
#if AAA_LIB_USE_OPENCV_CUDA()
	cv::cuda::GpuMat	_img_prev_gpu;
//	cv::cuda::GpuMat	_optflow_gpu_buf;
#endif
	REAL				_optflow_pyr_scale;
	INT32				_optflow_levels;
	INT32				_optflow_winsize;
	INT32				_optflow_iters;
	INT32				_optflow_poly_n_ui;
	REAL				_optflow_poly_sigma;
	bool				_b_optflow_initial_flow_ui;
	bool				_b_optflow_gaussian;
	bool				_b_optflow_use_Farneback_ui;

//	Inpainting
	c_opencv_pass		_pass_inpaint;
	cv::UMat			_inpainting_dst;
	cv::UMat			_inpaint_small_dst;
	cv::UMat			_inpaint_resize_dst;
	REAL				_inpaint_resize_x_ui;
	REAL				_inpaint_resize_y_ui;
	REAL				_inpaint_radius_ui;
	INT32				_inpaint_no_depth_ui;

//	distance transform
	c_opencv_pass		_pass_distrans;
	cv::UMat			_distrans_tmp;
	cv::UMat			_distrans_convert_src;
	UINT32				_distrans_type_ui;
	UINT32				_distrans_size_ui;
	INT32				_s_distrans_out_type_ui;
	DOUBLE				_distrans_factor_ui;
	DOUBLE				_distrans_offset_ui;

	FINLINE INT32		get_src_channel();
	FINLINE INT32		get_depth();

private:
			void	dealloc();

			void	store_img_prev();

			void	do_process();

			void	begin_pass(				c_opencv_pass& pass	);
			void	end_pass(				c_opencv_pass& pass	);

			void	do_invert(				c_opencv_pass& pass	);
			void	do_optical_flow(		c_opencv_pass& pass	);
			void	do_canny(				c_opencv_pass& pass	);
			void	do_edge(				c_opencv_pass& pass	);
			void	do_distance_transform(	c_opencv_pass& pass	);
			void	do_resize(				c_opencv_pass& pass	);
			void	do_blur(				c_opencv_pass& pass	);
			void	do_blur_gauss(			c_opencv_pass& pass	);
			void	do_bc(					c_opencv_pass& pass,	REAL min, REAL max	);
			void	do_erode(				c_opencv_pass& pass	);
			void	do_dilate(				c_opencv_pass& pass	);
			void	do_equalize_hist(		c_opencv_pass& pass	);
			void	do_denoise(				c_opencv_pass& pass	);
			void	do_inpaint_smooth(		c_opencv_pass& pass	);
			void	do_bilateral_filter(	c_opencv_pass& pass	);
			void	do_convert(				c_opencv_pass& pass	);

//			void	build_dst_index( cv::Mat& dst, INT32 index, CHAR* mess, INT32 channel_nb = 0 );
//			void	update_dst_index( cv::Mat& dst, INT32 index );

public:
			void	init();
	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw();
	virtual bool	can_implicit() final override { return true; }
};
