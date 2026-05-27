
#ifdef AAA_BDD_IMG_CV_STEREO_POINT_H
#error "BDD_IMG_CV_STEREO_POINT_H included more than once."
#endif
#define AAA_BDD_IMG_CV_STEREO_POINT_H 1


#ifndef AAA_BDD_IMG_H
#	include "obj_ui/bdd/bdd_image/bdd_img.h"
#endif
#ifndef __OPENCV_CALIB3D_HPP__
#	include "opencv2/calib3d/calib3d.hpp"
#endif
#ifndef __OPENCV_GPU_HPP__
#	include "opencv2/gpu/gpu.hpp"
#endif
#ifndef AAA_OURTIME_H
#	include "time/ourtime.h"
#endif

class	c_bdd_img_cv_calibrate;

class	c_bdd_img_cv_stereo_point final : public c_bdd_img
{
	FACTORY_DECLARE( c_bdd_img_cv_stereo_point, c_bdd_img );
public:

protected:
	c_speed*				_speed_bdd;
	c_speed*				_speed_stereo_points;

	BOOL					_b_cv_reproject_ui;
	BOOL					_b_use_bdd_ref_ui;
	o_str					_calibrate_bdd_sym_name_ui;
	c_bdd_img_cv_calibrate*	_bdd_target;

	BOOL					_b_threshold_ui;
	BOOL					_b_threshold_trig_ui;
	REAL					_threshold_min_ui[3];
	REAL					_threshold_max_ui[3];
	REAL					_bbox_min[3];
	REAL					_bbox_max[3];
	REAL					_bbox_thresh_min[3];
	REAL					_bbox_thresh_max[3];

	BOOL					_b_draw_ui;
	BOOL					_b_draw_points_ui;
	BOOL					_b_draw_disp_ui;
	BOOL					_b_draw_bbox_ui;

	BOOL					_b_normalize_ui;

	cv::Mat					_mat_dst;

//	BOOL					_b_normalize_ui;

	REAL					_q_ui[16];
	cv::Mat					_q;		// Transformation Matrix

	cv::gpu::GpuMat			_points_3d_gpu;
	cv::Mat					_points_3d;

	INT32					_points_nb;
	
	BOOL					_b_transfo_ui;
	REAL					_transfo_translate_ui[3];
	REAL					_transfo_scale_ui[4];


private:
			void	do_normalize();
			void	do_transfert();
			void	draw_bbox( REAL* min, REAL* max );
			void	close_mat();
			void	close();
			void	do_process();
			void	aaa_to_mat_matrix( cv::Mat& mat, CONST REAL* values, UINT32 rows, UINT32 cols );
			void	mat_to_aaa_matrix( CONST cv::Mat& mat, REAL* values, size_t size );
public:
			void	init();
	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw();
	virtual	BOOL	can_implicit() { return TRUE; }

			void	get_point( REAL* dst, INT32 index );
			INT32	get_point_nb();
};

