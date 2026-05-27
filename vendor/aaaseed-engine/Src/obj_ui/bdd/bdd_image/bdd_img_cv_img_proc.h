
#ifdef AAA_BDD_IMG_CV_IMG_PROC_H
#error "BDD_IMG_CV_IMG_PROC included more than once."
#endif
#define AAA_BDD_IMG_CV_IMG_PROC_H 1

#pragma	once

#ifndef AAA_BDD_IMG_H
#	include "obj_ui/bdd/bdd_image/bdd_img.h"
#endif

#ifndef __OPENCV_XIMGPROC_HPP__
#include "opencv2/ximgproc.hpp"
#endif
#ifndef SEEDS_REVISED_H
#	include "SuperPixels/SeedsRevised.h"
#endif

class	c_bdd_img_cv_img_proc final : public c_bdd_img
{
	FACTORY_DECLARE( c_bdd_img_cv_img_proc, c_bdd_img );
public:
//	enum CV_BACK_SEG_TYPE
//	{
//		BACK_SEG_MOG = 0,
//		BACK_SEG_MOG2,
//		BACK_SEG_GMG,
//		BACK_SEG_FGDSTAT,
////		STEREO_VAR,
////		STEREO_GC,
//		BACK_SEG_MAX
//	};
//
//	static	CHAR*	back_seg_type_str[BACK_SEG_MAX];
//

protected:
//	INT32				_back_img_index_ui;
//	INT32				_fore_img_index_ui;
//	cv::UMat				_mat_src_a;

	cv::UMat				_mat_dst;
	//cv::UMat				_mat_dst_back;
	//cv::UMat				_fore_dst_back;



	UINT32				_s_stereo_algo_ui;
	UINT32				_s_stereo_algo;
	BOOL				_b_gpu;

	// Forest Edge
	cv::Ptr<cv::ximgproc::StructuredEdgeDetection>		_edge_struc;
	cv::Mat												_edge_3;
	cv::Mat												_edge_forest;
	cv::Mat												_edge_convert;
	cv::UMat											_edge_out;
	BOOL												_b_edge_enable_ui;
	INT32												_edge_dst_index_ui;
	o_str												_edge_model_filename;
	o_str												_edge_model_filename_ui;


	// Superpixels
	cv::Ptr<cv::ximgproc::SuperpixelSEEDS>	_seeds;
	cv::Mat									_seeds_label;
	cv::UMat								_seeds_label2;
	cv::UMat								_seeds_mask;
	cv::UMat								_seeds_convert;
	cv::UMat								_seeds_result;
	//SEEDSRevisedMeanPixels*					_seeds_revised;
	cv::Mat									_sr_src;
	cv::Mat									meanImage;
	cv::Mat									labelImage;
	cv::Mat									contourImage;
	cv::UMat								umeanImage;
	cv::UMat								ulabelImage;
	cv::UMat								ucontourImage;

	BOOL			_b_sp_enable_ui;
	INT32			_sp_dst_index_ui;

	UINT32			_sp_nb_ui;	// Desired number of superpixels.Note that the actual number may be smaller due to restrictions( depending on the image size and num_levels ).Use getNumberOfSuperpixels() to get the actual number.
	INT32			_sp_nb;
	UINT32			_sp_count;
	UINT32			_sp_num_levels_ui;			// Number of block levels.The more levels, the more accurate is the segmentation, but needs more memory and CPU time.
	INT32			_sp_num_levels;
	UINT32			_sp_prior_ui;				// enable 3x3 shape smoothing term if > 0. A larger value leads to smoother shapes.prior must be in the range[0, 5].
	INT32			_sp_prior;
	UINT32			_sp_histogram_bins_ui;		// Number of histogram bins.
	INT32			_sp_histogram_bins;
	BOOL			_b_sp_double_step_ui;		// If true, iterate each block level twice for higher accuracy.
	BOOL			_b_sp_double_step;
	UINT32			_sp_nb_iteration_ui;
	BOOL			_b_sp_use_lav_ui;
	BOOL			_b_sp_label_ui;
	BOOL			_b_sp_mask_ui;

	BOOL			_b_spr_enable_ui;
	INT32			_spr_dst_index_ui;
	UINT32			_spr_nb_ui;
	INT32			_spr_nb;
	UINT32			_spr_count;
	UINT32			_spr_neighborhood_ui;
	INT32			_spr_neighborhood;
	REAL			_spr_confidence_ui;
	REAL			_spr_confidence;
	UINT32			_spr_histogram_bins_ui;
	INT32			_spr_histogram_bins;
	REAL			_spr_spatial_weigth_ui;
	REAL			_spr_spatial_weigth;
	UINT32			_spr_nb_iteration_ui;
	BOOL			_b_spr_mean_ui;
	BOOL			_b_spr_label_ui;
	BOOL			_b_spr_mask_ui;



	// Domain Transform Filter
	cv::UMat		_dt_filtered;
	cv::UMat		_dt_filtered_grey;
	cv::UMat		_gradX;
	cv::UMat		_gradY;
	cv::Mat			_gradMagnitude;
	cv::Mat			_multiplier;
	cv::UMat		_dt_dst;

	cv::UMat		_frame8u;
	cv::Mat			_frameLab;
	cv::Mat			_frameLab2;
	cv::Mat			_frameLabCn[3];
	cv::Mat			_layer0;
	cv::Mat			_layer1;
	cv::Mat			_layer2;
	cv::Mat			_detailLayer1;
	cv::Mat			_detailLayer2;

	BOOL			_b_dt_enable_ui;
	INT32			_dt_dst_index_ui;
	UINT32			_dt_filter_sigma_spatial_ui;
	UINT32			_dt_filter_sigma_color_ui;
	UINT32			_dt_filter_detail_constrat_ui;
	UINT32			_dt_filter_edges_gamma_ui;
	UINT32			_dt_filter_details_level_ui;
	BOOL			_b_dt_detail_ui;
	BOOL			_b_dt_stylize_ui;

	// Guided Filter
	BOOL			_b_guided_enable_ui;
	INT32			_guided_dst_index_ui;
	INT32			_guided_mask_bind_ui;
	UINT32			_guided_radius_ui;
	REAL			_guided_eps_ui;
	BOOL			_b_guided_smooth_ui;
	BOOL			_b_guided_enhance_ui;


	cv::UMat		_guided_dst;
	cv::Mat			_guided_convert;
	cv::Mat			_guided_enhanced;
	cv::Mat			_guided_enhanced2;
//	cv::UMat			_guided_enhanced2;
	//cv::Mat			_guided_enhanced;
	cv::UMat		_guided_mask;


	// Not realtime
	//// Automatic White Balance
	//BOOL			_b_wb_enable_ui;
	//INT32			_wb_dst_index_ui;
	//INT32			_wb_algorithm_ui;
	//cv::Mat			_wb_src;
	//cv::Mat			_wb_dst;
	//cv::UMat		_wb_out;

	//// DCT Denoising
	//BOOL			_b_dct_enable_ui;
	//INT32			_dct_dst_index_ui;
	//INT32			_dct_sigma_ui;
	//INT32			_dct_size_ui;
	//cv::Mat			_dct_src;
	//cv::Mat			_dct_dst;
	//cv::UMat		_dct_out;



private:
			void	close_mat();
			void	close();
			void	do_process();
			BOOL	do_superpixels();
			BOOL	do_seeds_revised();
			BOOL	do_edge_structured();
			BOOL	do_dt_filter();
			BOOL	do_guided_filter();
			BOOL	do_dct_denoise();
			BOOL	do_white_balance();

public:
			void	init();
	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw();
	virtual	BOOL	can_implicit() { return TRUE; }
};

#endif	//__BDD_IMG_CV_IMG_PROC_H__
