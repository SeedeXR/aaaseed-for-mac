
#ifdef AAA_BDD_IMG_CV_CALIBRATE_H
#error "BDD_IMG_CV_CALIBRATE_H included more than once."
#endif
#define AAA_BDD_IMG_CV_CALIBRATE_H 1


#ifndef AAA_BDD_IMG_H
#	include "obj_ui/bdd/bdd_image/bdd_img.h"
#endif
#ifndef AAA_REGISTRY_GENERIC_H
#	include "infrastructure/factory/registry_generic.h"
#endif


class c_speed;

class	c_bdd_img_cv_calibrate final : public c_bdd_img
{
	FACTORY_DECLARE( c_bdd_img_cv_calibrate, c_bdd_img );
private:
public:
	enum PATTERN_CALIBRATION_TYPE : INT32
	{
		PATTERN_CHESSBOARD= 0, 
		PATTERN_ASYMMETRIC_CIRCLES_GRID,
//		PATTERN_CIRCLES_GRID,
		PATTERN_CALIBRATION_MAX_NB,
	};
	static	C_PCHAR_C	pattern_str[PATTERN_CALIBRATION_MAX_NB];

	static	CONST	INT32	DATASET_ID_MAX = 9999;
private:
	class	c_calibration_data
	{
	public:
		o_str	_name;
		REAL	_cam_matrix_a_ui[9];	// 3x3 matrix
		REAL	_coeffs_a_ui[8];		// 8 max
		REAL	_cam_matrix_b_ui[9];	// 3x3 matrix
		REAL	_coeffs_b_ui[8];		//	8 max

		REAL	_rot_matrix_ui[9];		// Output rotation matrix between the 1st and the 2nd camera coordinate systems.
		REAL	_tra_vector_ui[3];		// Output translation vector between the coordinate systems of the cameras.
		REAL	_ess_matrix_ui[9];		// Output essential matrix.
		REAL	_fun_matrix_ui[9];		// Output fundamental matrix.

		REAL	_r1_ui[9];		// Output 3x3 rectification transform (rotation matrix) for the first camera.
		REAL	_r2_ui[9];		// Output 3x3 rectification transform (rotation matrix) for the second camera.
		REAL	_p1_ui[12];		// Output 3x4 projection matrix in the new (rectified) coordinate systems for the first camera.
		REAL	_p2_ui[12];		// Output 3x4 projection matrix in the new (rectified) coordinate systems for the second camera.
		REAL	_q_ui[16];		// Output 4x4 disparity-to-depth mapping matrix.

		void	init();
		void	init_array( REAL* data, size_t nb );
		c_calibration_data();
		//~c_calibration_data();
		void load( o_str CONST & name_info );
		void save( o_str CONST & name_info );
	};

	//c_map_server< INT32, c_curve_info >	_map_curve_info;
	c_vector_server< c_calibration_data >		_map_calibration_data;
	c_calibration_data*							_calibration_data;
	FINLINE	c_calibration_data* 	find_data( UINT32 id )	{	return  _map_calibration_data.find( id );	}
	FINLINE	c_calibration_data* 	get_data( UINT32 id )	{	return  _map_calibration_data.get( id );	}
protected:
	INT32		_src_b_img_index;
	UINT32		_src_b_size_x;
	UINT32		_src_b_size_y;
	UINT32		_src_b_size_x_last;
	UINT32		_src_b_size_y_last;

	INT32		_dst_b_img_index;
	INT32		_chess_a_img_index;
	INT32		_chess_b_img_index;
//	BOOL		_b_swap_src;

	BOOL		_b_undistort_trig_ui;

//	cv::UMat		_mat_src_a;
//	cv::UMat		_mat_src_b;

	cv::UMat		_mat_dst_a;
	cv::UMat		_mat_dst_b;

	cv::UMat		_mat_chess_a;
	cv::UMat		_mat_chess_b;

	UINT32		_nb_u;	// board size X
	UINT32		_nb_v;	// board size Y

	REAL		_square_size_ui;

	BOOL		_b_cb_adaptive_thresh_ui;
	BOOL		_b_cb_fast_check_ui;
	BOOL		_b_cb_normalize_ui;
	BOOL		_s_pattern_ui;
	BOOL		_b_reset_trig_ui;	// reset calibration data
	BOOL		_b_add_chessboard_trig;		// look in current img for chessboard

	BOOL		_b_calibrate_trig_ui;
	REAL		_interval_continous_ui;
	REAL		_t_continous_last;
	BOOL		_b_run_ui;


	c_speed*		_speed_bdd;
	c_speed*		_speed_chess;
	c_speed*		_speed_calibrate;
	c_speed*		_speed_rectify;
	c_speed*		_speed_undistort;
	c_speed*		_speed_remap;

	//REAL		_cam_matrix_a_ui[9];	// 3x3 matrix
	//REAL		_coeffs_a_ui[8];		// 8 max
	//REAL		_cam_matrix_b_ui[9];	// 3x3 matrix
	//REAL		_coeffs_b_ui[8];		//	8 max

	//REAL		_rot_matrix_ui[9];		// Output rotation matrix between the 1st and the 2nd camera coordinate systems.
	//REAL		_tra_vector_ui[3];		// Output translation vector between the coordinate systems of the cameras.
	//REAL		_ess_matrix_ui[9];		// Output essential matrix.
	//REAL		_fun_matrix_ui[9];		// Output fundamental matrix.

	//REAL		_r1_ui[9];		// Output 3x3 rectification transform (rotation matrix) for the first camera.
	//REAL		_r2_ui[9];		// Output 3x3 rectification transform (rotation matrix) for the second camera.
	//REAL		_p1_ui[12];		// Output 3x4 projection matrix in the new (rectified) coordinate systems for the first camera.
	//REAL		_p2_ui[12];		// Output 3x4 projection matrix in the new (rectified) coordinate systems for the second camera.
	//REAL		_q_ui[16];		// Output 4x4 disparity-to-depth mapping matrix.

	std::vector< std::vector< cv::Point3f > >	_object_points;
	std::vector< std::vector< cv::Point2f > >	_image_points_a;
	std::vector< std::vector< cv::Point2f > >	_image_points_b;

	//Output rotations and translations
	std::vector<cv::Mat>			_rvecs;
	std::vector<cv::Mat>			_tvecs;

	std::vector<cv::Point3f>		_lines[2];

	// output Matrices
	cv::Mat		_cam_mat_a;		// Input/output first camera matrix:. If any of CV_CALIB_USE_INTRINSIC_GUESS ,
								// CV_CALIB_FIX_ASPECT_RATIO , CV_CALIB_FIX_INTRINSIC , or CV_CALIB_FIX_FOCAL_LENGTH
								// are specified, some or all of the matrix components must be initialized.
	cv::Mat		_dist_coeffs_a;	// Input/output vector of distortion coefficients
								// (k_1, k_2, p_1, p_2[, k_3[, k_4, k_5, k_6]]) of 4, 5, or 8 elements. The output vector length depends on the flags.
	cv::Mat		_cam_mat_b;		// Input/output second camera matrix.
	cv::Mat		_dist_coeffs_b;	// Input/output lens distortion coefficients for the second camera.
	cv::Mat		_rot_mat;		// Output rotation matrix between the 1st and the 2nd camera coordinate systems.
	cv::Mat		_tra_vec;		// Output translation vector between the coordinate systems of the cameras.
	cv::Mat		_ess_mat;		// Output essential matrix.
	cv::Mat		_fun_mat;		// Output fundamental matrix.
	cv::Mat		_r1;			// Output 3x3 rectification transform (rotation matrix) for the first camera.
	cv::Mat		_r2;			// Output 3x3 rectification transform (rotation matrix) for the second camera.
	cv::Mat		_p1;			// Output 3x4 projection matrix in the new (rectified) coordinate systems for the first camera.
	cv::Mat		_p2;			// Output 3x4 projection matrix in the new (rectified) coordinate systems for the second camera.
	cv::Mat		_q;				// Output 4x4 disparity-to-depth mapping matrix.

	// used in image undistortion 
	cv::Mat		_map_a_1;
	cv::Mat		_map_a_2;
	cv::Mat		_map_b_1;
	cv::Mat		_map_b_2;

	BOOL		_b_undistort_map;
	BOOL		_b_calibrate;
	BOOL		_b_rectify;
	BOOL		_b_rectify_force_ui;
	BOOL		_b_rectify_trig_ui;
	BOOL		_b_rectify_do_init;

	bool		_b_rectify_enable;
	BOOL		_b_stereo;
	BOOL		_b_stereo_ui;

	BOOL		_b_use_intrinsic_guess_ui;
	BOOL		_b_fix_aspect_ratio_ui;
	BOOL		_b_fix_principal_point_ui;
	BOOL		_b_same_focal_length_ui;
	BOOL		_b_fix_focal_point_ui;
	BOOL		_b_calib_fix_k1_ui;
	BOOL		_b_calib_fix_k2_ui;
	BOOL		_b_calib_fix_k3_ui;
	BOOL		_b_calib_fix_k4_ui;
	BOOL		_b_calib_fix_k5_ui;
	BOOL		_b_calib_fix_k6_ui;

	BOOL		_b_radial8_coeff_enabled_ui;
	BOOL		_b_tangential_param_enabled_ui;
	BOOL		_find_chessboard_trig_ui;
	BOOL		_b_draw_corner_ui;
	REAL		_retroprojection_error;
	REAL		_retroprojection_error_avg;
	UINT32		_frames_chessboard_nb;

	BOOL		_b_use_uncalibrated_ui;
	BOOL		_b_draw_epipolar_ui;
	BOOL		_b_is_vertical_stereo;

	INT32		_dataset_id;
	INT32		_dataset_id_ui;

private:
			void	do_process();
			// Open the chessboard images and extract corner points
			void	do_find_chessboard_points();
			// Calibrate the camera
			void	do_calibrate();
			// Remove distortion in an image (after calibration)
			void	do_remap();
			void	do_rectify();
			void	do_undistort();

			BOOL	find_chessboard_low( cv::UMat& src, cv::UMat& chess, UINT32 chess_index, std::vector<cv::Point2f>&  corners );
			REAL	stereo_compute_reprojection_errors();
			REAL	compute_reprojection_errors();

	//		bool	update_mat_src( cv::UMat& src_a, cv::UMat& src_b );
			void	mat_to_aaa();
			void	aaa_to_mat();
			void	mat_to_aaa_array( CONST cv::Mat& mat, REAL* values, INT32 size );
			void	mat_to_aaa_matrix( CONST cv::Mat& mat, REAL* values, INT32 size );
			void	aaa_to_mat_array( cv::Mat& mat, CONST REAL* values, INT32 size );
			void	aaa_to_mat_matrix( cv::Mat& mat, CONST REAL* values, UINT32 rows, UINT32 cols );

			void	set_dataset( INT32 dataset_id );

			void	close();
public:
			REAL*	get_matrix_q()			{	return _calibration_data->_q_ui; }
			void	init();
	virtual	void	param_init_pt();

	virtual	void	update();
	virtual	void	draw();
	virtual	BOOL	can_implicit() { return TRUE; }

	virtual AAA_ERR	load_do_after( o_str CONST & filename_in );
	virtual AAA_ERR	save_do_after( o_str CONST & filename_in );
};
