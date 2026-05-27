
#ifdef AAA_BDD_IMG_CV_BACK_H
#error "BDD_IMG_CV_BACK_H included more than once."
#endif
#define AAA_BDD_IMG_CV_BACK_H 1


#ifndef AAA_BDD_IMG_H
#	include "obj_ui/bdd/bdd_image/bdd_img.h"
#endif

#ifndef OPENCV_VIDEO_HPP
#	include "opencv2/video/video.hpp"
#endif
#if AAA_LIB_USE_OPENCV_CUDA()
#	ifndef __OPENCV_CUDABGSEGM_HPP__
#		include "opencv2/cudabgsegm.hpp"
#	endif
#endif


class	c_bdd_img_cv_back_segm final : public c_bdd_img
{
	FACTORY_DECLARE( c_bdd_img_cv_back_segm, c_bdd_img );
public:
	enum CV_BACK_SEG_TYPE : INT32
	{
		BACK_SEG_MOG = 0,
		BACK_SEG_MOG2,
		BACK_SEG_GMG,
		BACK_SEG_FGDSTAT,
//		STEREO_VAR,
//		STEREO_GC,
		BACK_SEG_MAX
	};

	static	C_PCHAR_C	back_seg_type_str[BACK_SEG_MAX];

protected:
	INT32				_back_img_index_ui;
	INT32				_fore_img_index_ui;
	cv::UMat			_mat_src_a;

	cv::UMat			_mat_dst;
	cv::UMat			_mat_dst_back;
	cv::UMat			_fore_dst_back;
#if AAA_LIB_USE_OPENCV_CUDA()
	cv::cuda::GpuMat	_gpumat_back;
	cv::cuda::GpuMat	_gpumat_fore_mask;
	cv::cuda::GpuMat	_gpumat_foreground;
#endif
	cv::UMat			_mat_fore;
	cv::UMat			_mat_back;
	cv::UMat			_mat_fore_mask;

	cv::UMat			_mat_mog2_3cn;

	bool				_b_back_enable_ui;
	bool				_b_fore_enable_ui;

	bool				_b_background_reset_ui;

	UINT32				_s_stereo_algo_ui;
	UINT32				_s_stereo_algo;
	bool				_b_gpu;

#if AAA_LIB_USE_OPENCV_CUDA()
	//cv::cuda::BackgroundSubtractorFGD*		_fgd_stat_gpu;
	cv::Ptr<cv::cuda::BackgroundSubtractorMOG>	_mog_gpu;
	cv::Ptr<cv::cuda::BackgroundSubtractorMOG2>	_mog2_gpu;
	//cv::cuda::BackgroundSubtractorGMG*		_gmg_gpu;
#endif
	cv::Ptr<cv::BackgroundSubtractorMOG2>		_mog2_cpu;
	//cv::BackgroundSubtractorMOG*	_mog_cpu;
//	cv::BackgroundSubtractorMOG2*	_mog2_cpu;
	//cv::BackgroundSubtractorGMG*	_gmg_cpu;

	//FGDStatModel
	UINT32		_fgd_out_channel_nb_ui;	// 3 or 4 channel, 4 is faster
	UINT32		_fgd_out_channel_nb;	// 3 or 4 channel, 4 is faster
	INT32		_fgd_stat_lc_ui;		// Quantized levels per 'color' component. Power of two, typically 32, 64 or 128.
	INT32		_fgd_stat_n1c_ui;		// Number of color vectors used to model normal background color variation at a given pixel.
	INT32		_fgd_stat_n2c_ui;		// Number of color vectors retained at given pixel.  Must be > N1c, typically ~ 5/3 of N1c.
	// Used to allow the first N1c vectors to adapt over time to changing background.
	INT32		_fgd_stat_lcc_ui;		// Quantized levels per 'color co-occurrence' component.  Power of two, typically 16, 32 or 64.
	INT32		_fgd_stat_n1cc_ui;		// Number of color co-occurrence vectors used to model normal background color variation at a given pixel.
	INT32		_fgd_stat_n2cc_ui;		// Number of color co-occurrence vectors retained at given pixel.  Must be > N1cc, typically ~ 5/3 of N1cc.
	// Used to allow the first N1cc vectors to adapt over time to changing background.
	//unused
	//	bool		_b_fgd_stat_is_obj_without_holes_ui;	// If true we ignore holes within foreground blobs. Defaults to true.
	INT32		_fgd_stat_perform_morphing_ui;			// Number of erode-dilate-erode foreground-blob cleanup iterations.
	// These erase one-pixel junk blobs and merge almost-touching blobs. Default value is 1.
	REAL		_fgd_stat_alpha1_ui;	// How quickly we forget old background pixel values seen. Typically set to 0.1.
	REAL		_fgd_stat_alpha2_ui;	// "Controls speed of feature learning". Depends on T. Typical value circa 0.005.
	REAL		_fgd_stat_alpha3_ui;	// Alternate to alpha2, used (e.g.) for quicker initial convergence. Typical value 0.1.

	REAL		_fgd_stat_delta_ui;		// Affects color and color co-occurrence quantization, typically set to 2.
	REAL		_fgd_stat_t_ui;			// A percentage value which determines when new features can be recognized as new background. (Typically 0.9).
	REAL		_fgd_stat_min_area_ui;	// Discard foreground blobs whose bounding box is smaller than this threshold.

	//MOG
	bool		_mog_b_gpu_ui;
	INT32		_mog_gaussian_nb_ui;
	INT32		_mog_gaussian_nb;
	REAL		_mog_learning_rate_ui;
	INT32		_mog_history_ui;
	INT32		_mog_history;
	REAL		_mog_var_threshold_ui;
	REAL		_mog_background_ratio_ui;
	REAL		_mog_background_ratio;
	REAL		_mog_noiseSigma_ui;
	REAL		_mog_noiseSigma;
//	REAL	_mog_nb_mixtures_ui;

	// MOG2
	bool		_mog2_b_gpu_ui;
	INT32		_mog2_gaussian_nb_ui;
	INT32		_mog2_gaussian_nb;
	REAL		_mog2_learning_rate_ui;
	INT32		_mog2_history_ui;
	INT32		_mog2_history;
	//! here it is the maximum allowed number of mixture components.
	//! Actual number is determined dynamically per pixel
	REAL		_mog2_var_threshold_ui;
	REAL		_mog2_var_threshold;
	// threshold on the squared Mahalanobis distance to decide if it is well described
	// by the background model or not. Related to Cthr from the paper.
	// This does not influence the update of the background. A typical value could be 4 sigma
	// and that is varThreshold=4*4=16; Corresponds to Tb in the paper.

	///////////////////////////
	//// less important parameters - things you might change but be carefull
	//////////////////////////

	//float backgroundRatio;
	//// corresponds to fTB=1-cf from the paper
	//// TB - threshold when the component becomes significant enough to be included into
	//// the background model. It is the TB=1-cf from the paper. So I use cf=0.1 => TB=0.
	//// For alpha=0.001 it means that the mode should exist for approximately 105 frames before
	//// it is considered foreground
	//// float noiseSigma;
	//float varThresholdGen;

	////corresponds to Tg - threshold on the squared Mahalan. dist. to decide
	////when a sample is close to the existing components. If it is not close
	////to any a new component will be generated. I use 3 sigma => Tg=3*3=9.
	////Smaller Tg leads to more generated components and higher Tg might make
	////lead to small number of components but they can grow too large
	//float fVarInit;
	//float fVarMin;
	//float fVarMax;

	////initial variance  for the newly generated components.
	////It will will influence the speed of adaptation. A good guess should be made.
	////A simple way is to estimate the typical standard deviation from the images.
	////I used here 10 as a reasonable value
	//// min and max can be used to further control the variance
	//float fCT; //CT - complexity reduction prior
	////this is related to the number of samples needed to accept that a component
	////actually exists. We use CT=0.05 of all the samples. By setting CT=0 you get
	////the standard Stauffer&Grimson algorithm (maybe not exact but very similar)

	////shadow detection parameters
	//bool bShadowDetection; //default 1 - do shadow detection
	//unsigned char nShadowDetection; //do shadow detection - insert this value as the detection result - 127 default value
	//float fTau;
	//// Tau - shadow threshold. The shadow is detected if the pixel is darker
	////version of the background. Tau is a threshold on how much darker the shadow can be.
	////Tau= 0.5 means that if pixel is more than 2 times darker then it is not shadow
	////See: Prati,Mikic,Trivedi,Cucchiarra,"Detecting Moving Shadows...",IEEE PAMI,2003.


	//GMG
	bool		_gmg_b_gpu_ui;
	INT32		_gmg_max_feature_ui;
	REAL		_gmg_learning_rate_ui;
	INT32		_gmg_num_initialization_frames_ui;
	INT32		_gmg_quantization_levels_ui;
	REAL		_gmg_background_prior_ui;
	REAL		_gmg_decision_threshold_ui;
	INT32		_gmg_smoothing_radius_ui;			// odd for cpu version
	bool		_gmg_b_update_background_model_ui;


//	cv::cuda::DisparityBilateralFilter	_disparity_filter;		// crash when releasing, OpenCV Bug

private:
			//void	update_dst_index( cv::UMat& dst, INT32 index );
			void	close_mat();
			void	close();
			void	do_process();
			bool	do_background_segmentation( cv::UMat& dst, cv::UMat& dst_back );

			//void	do_mog_cpu( cv::UMat& src_a, cv::Mat& dst, cv::Mat& back );
			void	do_mog2_cpu( cv::UMat& src_a, cv::UMat& dst, cv::UMat& back );
			//void	do_gmg_cpu( cv::UMat& src_a, cv::UMat& dst, cv::Mat& back );


#if AAA_LIB_USE_OPENCV_CUDA()
			void	do_mog( cv::cuda::GpuMat& src_a, cv::cuda::GpuMat& dst, cv::cuda::GpuMat& back );
			void	do_mog2( cv::cuda::GpuMat& src_a, cv::cuda::GpuMat& dst, cv::cuda::GpuMat& back );
			//void	do_gmg( cv::cuda::GpuMat& src_a, cv::cuda::GpuMat& dst );
			//void	do_fgd(  cv::cuda::GpuMat& src_a, cv::cuda::GpuMat& dst, cv::cuda::GpuMat& back );
#endif
			//bool	build_dst_index( cv::UMat& dst, INT32 index, cv::UMat& ref, CHAR* mess, aaa::PIXEL_TYPE type );

public:
			void	init();
	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw();
	virtual bool	can_implicit() final override { return true; }
};


