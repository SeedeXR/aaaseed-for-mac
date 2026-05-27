#include "bdd_img_cv_calibrate.h"
#include "image/bind_img_2d.h"
#include "time/speed.h"
#include "wrapper/aaa_opencv.h"
#include "spy.h"
#include "file/file_csv.h"
#include "strnum.h"
#include "asc_line.h"
#include "file/aaa_file.h"

#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/imgproc/imgproc.hpp"


#include <iterator>

C_PCHAR_C	c_bdd_img_cv_calibrate::pattern_str[PATTERN_CALIBRATION_MAX_NB] =
{
	"Chessboard",
	"Asymmetric Circles",
	//"Circles"	// crash....
};

FACTORY_CREATE_PROP_V1( c_bdd_img_cv_calibrate, bdd_img_cv_calibrate, Image OpenCV Calibrate, bdd_img_cv_calibrate, sub_menu="Image"; );


namespace n_bdd_img_cv_calibrate
{
	static	CONST	UINT32	BASE_PARAM_NB				= 14 + c_bdd_img::SUPER_PARAM_NB;
	static	CONST	UINT32	CHESSBOARD_PARAM_NB			= 16;
	static	CONST	UINT32	CALIB_PARAM_NB				= 20;
	static	CONST	UINT32	CAMERA_MATRIX_PARAM_NB		= 9;
	static	CONST	UINT32	DIST_COEFF_PARAM_NB			= 8;
	static	CONST	UINT32	TRANSLATION_PARAM_NB		= 3;
	static	CONST	UINT32	RECTIFY_MATRIX_PARAM_NB		= 9;
	static	CONST	UINT32	PROJECTION_MATRIX_PARAM_NB	= 12;
	static	CONST	UINT32	DISPARITY_MATRIX_PARAM_NB	= 16;
	static	CONST	UINT32	GROUP_PARAM_NB				= 15;

	static	CONST	INT32	PARAM_NB		=	BASE_PARAM_NB
											+	CHESSBOARD_PARAM_NB
											+	CALIB_PARAM_NB
											+	CAMERA_MATRIX_PARAM_NB * 5
											+	DIST_COEFF_PARAM_NB * 2
											+	TRANSLATION_PARAM_NB
											+	RECTIFY_MATRIX_PARAM_NB * 2
											+	PROJECTION_MATRIX_PARAM_NB * 2
											+	DISPARITY_MATRIX_PARAM_NB
											+	GROUP_PARAM_NB;

	static	C_PARAM_DEF	param[PARAM_NB] =
	{
		BDD_IMG_BASE_PARAMS

		ST_PARAM_INT32_POS_ZERO(	image_b					)
		ST_PARAM_INT32_LOCKED(		image_b_source_size_x	)
		ST_PARAM_INT32_LOCKED(		image_b_source_size_y	)
		ST_PARAM_BOOL_OFF(			mono					)
		ST_PARAM_IMG_DST(			dst_b					)
		ST_PARAM_BOOL_OFF(			stereo_enable			)
//		ST_PARAM_BOOL_OFF(			swap_source				)
		ST_PARAM_BOOLC_OFF(			rectify_enable			)
		ST_PARAM_BOOL_OFF(			undistort_trig			)
		ST_PARAM_BOOL_LOCKED(		is_vertical_stereo		)
		ST_PARAM_BOOL_OFF(			draw_epipolar_lines		)
		ST_PARAM_INT32(				dataset_id,				2, 1,		1, c_bdd_img_cv_calibrate::DATASET_ID_MAX )
		ST_PARAM_REF(				dataset_name			)
		ST_PARAM_REAL_LOCKED(		process_time			)
		ST_PARAM_REAL_LOCKED(		remap_time				)

		ST_PARAM_GROUP_CLOSED( Chessboard, CHESSBOARD_PARAM_NB )
			ST_PARAM_SYMBO_PSTR_ZERO(	pattern,						c_bdd_img_cv_calibrate::pattern_str )
			ST_PARAM_INT32_POS(			nb_u,							7, 9	)
			ST_PARAM_INT32_POS(			nb_v,							4, 6	)
			ST_PARAM_REAL_POS(			square_size,					1, 2.5	)
			ST_PARAM_BOOL_OFF(			cb_adaptive_threshold			)
			ST_PARAM_BOOL_OFF(			cb_fast_check					)
			ST_PARAM_BOOL_OFF(			cb_normalize					)
			ST_PARAM_BOOL_OFF(			find_chessboard_trig			)
			ST_PARAM_BOOL_OFF(			run								)
			ST_PARAM_REAL_POS(			interval,						1, 2.5	)
			ST_PARAM_BOOL_OFF(			reset_trig						)
			ST_PARAM_BOOL_OFF(			draw_chessboard_corner			)
			ST_PARAM_IMG_DST(			image_a_chessboard				)
			ST_PARAM_IMG_DST(			image_b_chessboard				)
			ST_PARAM_INT32_LOCKED(		frames_chess_nb					)
			ST_PARAM_REAL_LOCKED(		chess_process_time				)

		ST_PARAM_GROUP_CLOSED( Calibration, CALIB_PARAM_NB )
			ST_PARAM_BOOL_OFF(		calibrate_trig					)
			ST_PARAM_BOOL_OFF(		tangential_param_enabled		)
			ST_PARAM_BOOL_OFF(		radial8_coeff_enabled			)
			ST_PARAM_BOOL_OFF(		use_intrinsic_guess				)
			ST_PARAM_BOOL_OFF(		fix_aspect_ratio				)
			ST_PARAM_BOOL_OFF(		fix_principal_point				)
			ST_PARAM_BOOL_ON(		fix_k1							)
			ST_PARAM_BOOL_ON(		fix_k2							)
			ST_PARAM_BOOL_ON(		fix_k3							)
			ST_PARAM_BOOL_OFF(		fix_k4							)
			ST_PARAM_BOOL_OFF(		fix_k5							)
			ST_PARAM_BOOL_OFF(		fix_k6							)
			ST_PARAM_BOOL_OFF(		stereo_fix_focal_point			)
			ST_PARAM_BOOL_OFF(		stereo_same_focal_length		)
			ST_PARAM_BOOL_OFF(		stereo_use_uncalibrated_method	)
			ST_PARAM_REAL_LOCKED(	reprojection_error				)
			ST_PARAM_REAL_LOCKED(	reprojection_error_avg			)
			ST_PARAM_REAL_LOCKED(	calibrate_process_time			)
			ST_PARAM_REAL_LOCKED(	rectify_time					)
			ST_PARAM_REAL_LOCKED(	undistort_time					)

#define	ST_PARAM_ARRAY_8( param_name )\
		ST_PARAM_REAL_INF_SAVE_NOT( param_name##_01, 1.0, .0 )\
		ST_PARAM_REAL_INF_SAVE_NOT( param_name##_02, 1.0, .0 )\
		ST_PARAM_REAL_INF_SAVE_NOT( param_name##_03, 1.0, .0 )\
		ST_PARAM_REAL_INF_SAVE_NOT( param_name##_04, 1.0, .0 )\
		ST_PARAM_REAL_INF_SAVE_NOT( param_name##_05, 1.0, .0 )\
		ST_PARAM_REAL_INF_SAVE_NOT( param_name##_06, 1.0, .0 )\
		ST_PARAM_REAL_INF_SAVE_NOT( param_name##_07, 1.0, .0 )\
		ST_PARAM_REAL_INF_SAVE_NOT( param_name##_08, 1.0, .0 )

#define	ST_PARAM_ARRAY_9( param_name )\
	ST_PARAM_ARRAY_8( param_name )\
	ST_PARAM_REAL_INF_SAVE_NOT( param_name##_09, 1.0, .0 )
#define	ST_PARAM_ARRAY_12( param_name )\
	ST_PARAM_ARRAY_9( param_name )\
	ST_PARAM_REAL_INF_SAVE_NOT( param_name##_10, 1.0, .0 )\
	ST_PARAM_REAL_INF_SAVE_NOT( param_name##_11, 1.0, .0 )\
	ST_PARAM_REAL_INF_SAVE_NOT( param_name##_12, 1.0, .0 )
#define	ST_PARAM_ARRAY_16( param_name )\
	ST_PARAM_ARRAY_12( param_name )\
	ST_PARAM_REAL_INF_SAVE_NOT( param_name##_13, 1.0, .0 )\
	ST_PARAM_REAL_INF_SAVE_NOT( param_name##_14, 1.0, .0 )\
	ST_PARAM_REAL_INF_SAVE_NOT( param_name##_15, 1.0, .0 )\
	ST_PARAM_REAL_INF_SAVE_NOT( param_name##_16, 1.0, .0 )

		ST_PARAM_GROUP_CLOSED( Camera Matrix Left, CAMERA_MATRIX_PARAM_NB )
			ST_PARAM_ARRAY_9( left_p )
		ST_PARAM_GROUP_CLOSED( Distorsions Coefficients Left, DIST_COEFF_PARAM_NB )
			ST_PARAM_ARRAY_8( left_d )
		ST_PARAM_GROUP_CLOSED( Camera Matrix Right, CAMERA_MATRIX_PARAM_NB )
			ST_PARAM_ARRAY_9( right_p )
		ST_PARAM_GROUP_CLOSED( Distorsions Coefficients Right, DIST_COEFF_PARAM_NB )
			ST_PARAM_ARRAY_8( right_d )
		ST_PARAM_GROUP_CLOSED( Rotation Matrix, CAMERA_MATRIX_PARAM_NB )
			ST_PARAM_ARRAY_9( rot )
		ST_PARAM_GROUP_CLOSED( Translation, TRANSLATION_PARAM_NB )
			ST_PARAM_REAL_INF_SAVE_NOT( tra_x, 1.0, .0 )
			ST_PARAM_REAL_INF_SAVE_NOT( tra_y, 1.0, .0 )
			ST_PARAM_REAL_INF_SAVE_NOT( tra_z, 1.0, .0 )
		ST_PARAM_GROUP_CLOSED( Essential Matrix, CAMERA_MATRIX_PARAM_NB )
			ST_PARAM_ARRAY_9( ess )
		ST_PARAM_GROUP_CLOSED( Fundamental Matrix, CAMERA_MATRIX_PARAM_NB )
			ST_PARAM_ARRAY_9( fun )
		ST_PARAM_GROUP_CLOSED( Rectification Transform 1, RECTIFY_MATRIX_PARAM_NB )
			ST_PARAM_ARRAY_9( r1 )
		ST_PARAM_GROUP_CLOSED( Rectification Transform 2, RECTIFY_MATRIX_PARAM_NB )
			ST_PARAM_ARRAY_9( r2 )
		ST_PARAM_GROUP_CLOSED( Projection Matrix 1, PROJECTION_MATRIX_PARAM_NB )
			ST_PARAM_ARRAY_12( p1 )
		ST_PARAM_GROUP_CLOSED( Projection Matrix 2, PROJECTION_MATRIX_PARAM_NB )
			ST_PARAM_ARRAY_12( p2 )
		ST_PARAM_GROUP_CLOSED( Disparity Depth Matrix, DISPARITY_MATRIX_PARAM_NB )
			ST_PARAM_ARRAY_16( q )
	};
}

void	c_bdd_img_cv_calibrate::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start_src_resize( false, false );

	param_set_pt( h, _src_b_img_index					);
	param_set_pt( h, _src_b_size_x						);
	param_set_pt( h, _src_b_size_y						);
	param_set_pt( h, _b_mono_ui							);
	param_set_pt( h, _dst_b_img_index					);
	param_set_pt( h, _b_stereo_ui						);
//	param_set_pt( h, _b_swap_src						);
	param_set_pt( h, _b_rectify_enable					);
	param_set_pt( h, _b_undistort_trig_ui				);
	param_set_pt( h, _b_is_vertical_stereo				);
	param_set_pt( h, _b_draw_epipolar_ui				);
	param_set_pt( h, _dataset_id_ui						);
	param_set_pt( h, _calibration_data->_name			);
	param_set_pt( h, _speed_bdd->get_pt_interval_ms()	);
	param_set_pt( h, _speed_remap->get_pt_interval_ms()	);

	++h;
		param_set_pt( h, _s_pattern_ui							);
		get_param( h )->set_comment( "Nb of internal corners" );
		param_set_pt( h, _nb_u									);
		get_param( h )->set_comment( "Nb of internal corners" );
		param_set_pt( h, _nb_v									);
		param_set_pt( h, _square_size_ui						);
		param_set_pt( h, _b_cb_adaptive_thresh_ui				);
		param_set_pt( h, _b_cb_fast_check_ui					);
		param_set_pt( h, _b_cb_normalize_ui						);
		param_set_pt( h, _find_chessboard_trig_ui				);
		param_set_pt( h, _b_run_ui								);
		param_set_pt( h, _interval_continous_ui					);
		param_set_pt( h, _b_reset_trig_ui						);
		param_set_pt( h, _b_draw_corner_ui						);
		param_set_pt( h, _chess_a_img_index						);
		param_set_pt( h, _chess_b_img_index						);
		param_set_pt( h, _frames_chessboard_nb					);
		param_set_pt( h, _speed_chess->get_pt_interval_ms()		);
	++h;
		param_set_pt( h, _b_calibrate_trig_ui					);
		param_set_pt( h, _b_tangential_param_enabled_ui			);
		param_set_pt( h, _b_radial8_coeff_enabled_ui			);
		param_set_pt( h, _b_use_intrinsic_guess_ui				);
		param_set_pt( h, _b_fix_aspect_ratio_ui					);
		param_set_pt( h, _b_fix_principal_point_ui				);
		param_set_pt( h, _b_calib_fix_k1_ui						);
		param_set_pt( h, _b_calib_fix_k2_ui						);
		param_set_pt( h, _b_calib_fix_k3_ui						);
		param_set_pt( h, _b_calib_fix_k4_ui						);
		param_set_pt( h, _b_calib_fix_k5_ui						);
		param_set_pt( h, _b_calib_fix_k6_ui						);
		param_set_pt( h, _b_fix_focal_point_ui					);
		param_set_pt( h, _b_same_focal_length_ui				);
		param_set_pt( h, _b_use_uncalibrated_ui					);
		param_set_pt( h, _retroprojection_error					);
		param_set_pt( h, _retroprojection_error_avg				);
		param_set_pt( h, _speed_calibrate->get_pt_interval_ms()	);
		param_set_pt( h, _speed_rectify->get_pt_interval_ms()	);
		param_set_pt( h, _speed_undistort->get_pt_interval_ms()	);

	++h;
		param_set_pt_v( h, _calibration_data->_cam_matrix_a_ui, 9	);
	++h;
		param_set_pt_v( h, _calibration_data->_coeffs_a_ui, 8		);
	++h;
		param_set_pt_v( h, _calibration_data->_cam_matrix_b_ui, 9	);
	++h;
		param_set_pt_v( h, _calibration_data->_coeffs_b_ui, 8		);
	++h;
		param_set_pt_v( h, _calibration_data->_rot_matrix_ui, 9		);
	++h;
		param_set_pt_v( h, _calibration_data->_tra_vector_ui, 3		);
	++h;
		param_set_pt_v( h, _calibration_data->_ess_matrix_ui, 9		);
	++h;
		param_set_pt_v( h, _calibration_data->_fun_matrix_ui, 9		);
	++h;
		param_set_pt_v( h, _calibration_data->_r1_ui, 9				);
	++h;
		param_set_pt_v( h, _calibration_data->_r2_ui, 9				);
	++h;
		param_set_pt_v( h, _calibration_data->_p1_ui, 12			);
	++h;
		param_set_pt_v( h, _calibration_data->_p2_ui, 12			);
	++h;
		param_set_pt_v( h, _calibration_data->_q_ui, 16				);

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bdd_img_cv_calibrate )
	,_retroprojection_error			( .0 )
	,_frames_chessboard_nb			( 0 )
	,_t_continous_last				( .0f )
	//,_b_undistort_map				( FALSE )
	,_b_stereo						( FALSE )
	,_retroprojection_error_avg		( .0f )
	,_b_calibrate					( FALSE )
	,_b_rectify						( FALSE )
	,_b_rectify_do_init				( TRUE )
	,_b_undistort_map				( FALSE )
	,_dataset_id					( -1 )
	,_dataset_id_ui					( 1 )
{
	_speed_bdd			= new c_speed( true, 4, g_speed_master->get_pb_print_bdd(), "update"			, this );
	_speed_chess		= new c_speed( true, 5, g_speed_master->get_pb_print_bdd(),	"chessboard"		, this );
	_speed_calibrate	= new c_speed( true, 5, g_speed_master->get_pb_print_bdd(),	"calibrate"			, this );
	_speed_rectify		= new c_speed( true, 5,	g_speed_master->get_pb_print_bdd(),	"rectify"			, this );
	_speed_undistort	= new c_speed( true, 5, g_speed_master->get_pb_print_bdd(),	"undistort"			, this );
	_speed_remap		= new c_speed( true, 5, g_speed_master->get_pb_print_bdd(),	"remap"				, this );

	set_dataset( _dataset_id_ui );
	param_init_with( n_bdd_img_cv_calibrate::param, n_bdd_img_cv_calibrate::PARAM_NB );
}

void	c_bdd_img_cv_calibrate::close()
{
	if( this )
	{
		_cam_mat_a.release();
		_dist_coeffs_a.release();
		_cam_mat_b.release();
		_dist_coeffs_b.release();
		_rot_mat.release();
		_tra_vec.release();
		_ess_mat.release();
		_fun_mat.release();
		_r1.release();
		_r2.release();
		_p1.release();
		_p2.release();
		_map_a_1.release();
		_map_a_2.release();
		_map_b_1.release();
		_map_b_2.release();
	//	_mat_src_a.release();
	//	_mat_src_b.release();
		_src_cur		= nullptr;
#ifdef AAA_LIB_USE_OPENCV_CUDA
		_src_cur_gpu	= nullptr;
#endif
		_mat_dst_a.release();
		_mat_dst_b.release();
		_mat_chess_a.release();
		_mat_chess_b.release();

		obj_delete( _speed_bdd );
		obj_delete( _speed_chess );
		obj_delete( _speed_calibrate );
		obj_delete( _speed_rectify );
		obj_delete( _speed_undistort );
		obj_delete( _speed_remap );
	}
}

c_bdd_img_cv_calibrate::~c_bdd_img_cv_calibrate()
{
	close();
}


void	c_bdd_img_cv_calibrate::update()
{
	_speed_bdd->begin();
//	if( update_mat_src( _b_swap_src ? _mat_src_b : _mat_src_a, _b_swap_src ? _mat_src_a : _mat_src_b ) )
	if( update_mat_2_src() )
	{
		if( _b_reset_trig_ui )
		{
			_object_points.clear();
			_image_points_a.clear();
			_image_points_b.clear();
			_rvecs.clear();
			_tvecs.clear();
			_lines[0].clear();
			_lines[1].clear();

			_frames_chessboard_nb = 0;
			_b_reset_trig_ui = FALSE;
		}
		set_dataset( _dataset_id_ui );
		do_process();
		update_dst_index( _dst_img_index_ui, "Left Rectified" );
		update_dst_b_index( _dst_b_img_index, "Right Rectified" );
	}
	_speed_bdd->end();
}

void	c_bdd_img_cv_calibrate::draw()
{
}

void	c_bdd_img_cv_calibrate::do_process()
{
	//if( !build_cv_mat( _mat_dst_a, _dst_img_index_ui, _mat_src_a, "destination left" ) )
	//{
	//	ERR_PRINT_STRING( "Error building dst left" );
	//	return;
	//}
	if( _b_stereo )
	{
		//if( !build_cv_mat( _mat_dst_b, _dst_b_img_index, _mat_src_b, "destination right" ) )
		//{
		//	ERR_PRINT_STRING( "Error building dst left" );
		//	return;
		//}
		//todo strange we should arrive here even
		if ( !is_mat_equal( _src_cur, _src_b_cur ) )
		{
			return;
		}
	}

	if( _b_run_ui )
	{
		// test time
		REAL	t = n_aaatime::get();
		if( (t - _t_continous_last) > _interval_continous_ui )
		{
			_find_chessboard_trig_ui = TRUE;
			_t_continous_last = t;
		}
	}
	if( _find_chessboard_trig_ui )
	{
		do_find_chessboard_points();
		_find_chessboard_trig_ui = FALSE;
	}
	if( _b_calibrate_trig_ui )
	{
		if( _frames_chessboard_nb == 0 )
		{
			ERR_PRINT_STRING( "Can't calibrate if no chessboard points" );
		}
		else
		{
			do_calibrate();
			do_rectify();
			mat_to_aaa();
		}
		_b_calibrate_trig_ui = FALSE;
	}
	if( _b_rectify_enable )
	{
		if ( _b_undistort_trig_ui || is_size_changed() )
		{
			_b_undistort_trig_ui = FALSE;
			_b_undistort_map = FALSE;
		}
		if( !_b_undistort_map )
		{
			aaa_to_mat();
			do_undistort();
		}
		if( _b_undistort_map )
		{
			do_remap();
		}
		else
		{
			ERR_PRINT_STRING( "%s() can't remap, rectification not known", __FUNCTION__ );
		}
	}
}

BOOL	c_bdd_img_cv_calibrate::find_chessboard_low( cv::UMat& src, cv::UMat& chess, UINT32 chess_index,  std::vector<cv::Point2f>&  corners )
{

	cv::Size	board_size( _nb_u, _nb_v );
	UINT32		flag = 0;
	if( _b_cb_adaptive_thresh_ui )		{ flag |= cv::CALIB_CB_ADAPTIVE_THRESH;	}
	if( _b_cb_fast_check_ui )			{ flag |= cv::CALIB_CB_FAST_CHECK;		}
	if( _b_cb_normalize_ui )			{ flag |= cv::CALIB_CB_NORMALIZE_IMAGE;	}

	BOOL	b_found = cv::findChessboardCorners( src, board_size, corners, flag );
	if( b_found )
	{
		cv::Mat		gray;
		if( _b_mono_ui || src.channels() == 1 )
		{
			src.copyTo( gray );
		}
		else
		{
			cv::cvtColor( src, gray, cv::COLOR_BGR2GRAY );
		}
		//Calibration will suffer without subpixel interpolation
		cv::cornerSubPix( gray, corners, cv::Size(5,5), cv::Size(-1,-1), 
							cv::TermCriteria(cv::TermCriteria::MAX_ITER + cv::TermCriteria::EPS, 
							30,		// max number of iterations 
							0.1));	// min accuracy

		//Draw the corners
		if( _b_draw_corner_ui )
		{
			//if( build_cv_mat( chess, chess_index, src, "destination chess" ) )
			{
				src.copyTo( chess );
				cv::drawChessboardCorners( chess, board_size, corners, b_found ? true : false );
				update_dst_index( chess, chess_index, "Chessboard", TRUE );
			}
			//else
			//{
			//	ERR_PRINT_STRING( "Error building chess" );
			//}
		}
	}
	return b_found;
}

// Open chessboard images and extract corner points
void	c_bdd_img_cv_calibrate::do_find_chessboard_points()
{
	_speed_chess->begin();
	cv::Size	board_size( _nb_u, _nb_v );

	// the points on the chessboard
	std::vector<cv::Point2f>	image_corners_a;	// 2d image position of chessboard corners for left image
	std::vector<cv::Point2f>	image_corners_b;	// 2d image position of chessboard corners for right image
	std::vector<cv::Point3f>	object_corners;		// 3d position of chessboard corners

	object_corners.reserve( _nb_u * _nb_v );
	// 3D Scene Points:
	// Initialize the chessboard corners in the chessboard reference frame. The corners are at 3D location (X,Y,Z)= (i,j,0)
	switch( _s_pattern_ui )
	{
	case PATTERN_CHESSBOARD:
//	case PATTERN_CIRCLES_GRID:
		for( UINT32 j = 0; j < _nb_v; ++j )
		{
			for( UINT32 i = 0; i < _nb_u; ++i )
			{
				object_corners.push_back( cv::Point3f( float( i * _square_size_ui ), float( j * _square_size_ui ), .0f ) );
			}
		}
		break;
	case PATTERN_ASYMMETRIC_CIRCLES_GRID:
		for( UINT32 i = 0; i < _nb_v; i++ )
		{
			for( UINT32 j = 0; j < _nb_u; j++ )
			{
				object_corners.push_back( cv::Point3f( float( ( 2 * j + i % 2 ) * _square_size_ui ), float( i * _square_size_ui ), .0f ) );
			}
		}
		break;	
	}

	// 2D Image points:
	BOOL	b_found_a = FALSE;
	BOOL	b_found_b = FALSE;
	INT32	count = 0;
	// Get the chessboard corners
	if( _s_pattern_ui == PATTERN_CHESSBOARD )
	{

		b_found_a = find_chessboard_low( *_src_b_cur, _mat_chess_a, _chess_a_img_index, image_corners_a );
		if( b_found_a )
		{
			if( _b_stereo && ( image_corners_a.size() == (_nb_u * _nb_v) ) ) // board_size.area() ) )
			{
				b_found_b = find_chessboard_low( *_src_b_cur, _mat_chess_b, _chess_b_img_index, image_corners_b );
			}
		}
	}
	else
	{
		b_found_a = cv::findCirclesGrid( *_src_b_cur, board_size, image_corners_a, PATTERN_ASYMMETRIC_CIRCLES_GRID ? cv::CALIB_CB_ASYMMETRIC_GRID : cv::CALIB_CB_SYMMETRIC_GRID );
		if( _b_stereo )
		{
			b_found_b = cv::findCirclesGrid( *_src_b_cur, board_size, image_corners_b, PATTERN_ASYMMETRIC_CIRCLES_GRID ? cv::CALIB_CB_ASYMMETRIC_GRID : cv::CALIB_CB_SYMMETRIC_GRID );
		}
	}
	if( b_found_a )
	{
		if( _b_stereo )
		{
			if( b_found_b )
			{
				// If we have a good board, add it to our data
				if ( image_corners_a.size() == image_corners_b.size() )
				{
					// Add image and scene points from one view
					// 2D image points from one view
					_image_points_a.push_back( image_corners_a );
					_image_points_b.push_back( image_corners_b );
					// corresponding 3D scene points
					_object_points.push_back( object_corners );
					_frames_chessboard_nb = (INT32)_image_points_a.size();
				}
			}
		}
		else
		{
			// If we have a good board, add it to our data
			if ( image_corners_a.size() == board_size.area() )
			{
				// Add image and scene points from one view
				// 2D image points from one view
				_image_points_a.push_back( image_corners_a );
				// corresponding 3D scene points
				_object_points.push_back( object_corners );
				_frames_chessboard_nb = (INT32)_image_points_a.size();
			}
		}
	}
	_speed_chess->end();
}

REAL	c_bdd_img_cv_calibrate::compute_reprojection_errors()
{
	std::vector<cv::Point2f>	image_points_2;
	UINT32						total_points = 0;
	REAL						total_err = .0f;
	REAL						err = .0f;
	UINT32						size = (UINT32)_object_points.size();

	for( UINT32 i = 0; i < size; ++i )
	{
		cv::projectPoints( cv::UMat( _object_points[i] ), _rvecs[i], _tvecs[i], _cam_mat_a, _dist_coeffs_a, image_points_2 );
		err = cv::norm( cv::UMat( _image_points_a[i] ), cv::UMat( image_points_2 ), CV_L2 );
		total_err		+= err*err;
		total_points	+= (UINT32)_object_points[i].size();
	}
	return std::sqrt( total_err / total_points );
}

REAL	c_bdd_img_cv_calibrate::stereo_compute_reprojection_errors()
{
	// CALIBRATION QUALITY CHECK
	// because the output fundamental matrix implicitly includes all the output information,
	// we can check the quality of calibration using the epipolar geometry constraint: m2^t*F*m1=0

	 _lines[0].clear();
	 _lines[1].clear();

	DOUBLE	avgErr = 0;
	UINT32	nframes = (UINT32)_object_points.size();
	UINT32	N = _nb_u * _nb_v;

	for( UINT32 i = 0; i < nframes; i++ )
	{
		std::vector<cv::Point2f>&	pt0 = _image_points_a[i];
		std::vector<cv::Point2f>&	pt1 = _image_points_b[i];

		cv::undistortPoints( pt0, pt0, _cam_mat_a, _dist_coeffs_a, cv::Mat(), _cam_mat_a);
		cv::undistortPoints( pt1, pt1, _cam_mat_b, _dist_coeffs_b, cv::Mat(), _cam_mat_b);
		cv::computeCorrespondEpilines( pt0, 1, _fun_mat, _lines[0] );
		cv::computeCorrespondEpilines( pt1, 2, _fun_mat, _lines[1] );

		for( UINT32 j = 0; j < N; j++ )
		{
			double err =	fabs( pt0[j].x * _lines[1][j].x + pt0[j].y * _lines[1][j].y + _lines[1][j].z )
						+	fabs( pt1[j].x * _lines[0][j].x + pt1[j].y * _lines[0][j].y + _lines[0][j].z );
			avgErr += err;
		}
	}
	return avgErr / (nframes*N);
}

// Calibrate the camera
void	c_bdd_img_cv_calibrate::do_calibrate()
{
	_speed_calibrate->begin();
	// Set the calibration options
// 8radialCoeffEnabled should be true if 8 radial coefficients are required (5 is default)
// tangentialParamEnabled should be true if tangential distortion is present
	INT32	flag = 0;
	if( !_b_tangential_param_enabled_ui )	{ flag += CV_CALIB_ZERO_TANGENT_DIST;	}
	if( _b_radial8_coeff_enabled_ui )		{ flag += CV_CALIB_RATIONAL_MODEL;		}

	if( _b_use_intrinsic_guess_ui )			{ flag += CV_CALIB_USE_INTRINSIC_GUESS;	}	// for stereo calibration ?

	////initialized throw ratio = 1.0f
	//float f = 1024;
	//cameraMatrix = (Mat1d(3, 3) <<
	//	f, 0, f/2,
	//	0, f, f/2,
	//	0, 0, 1);
	//initialized throw ratio = 1.0f
	//float f = projectorResolution.y;
	//cameraMatrix = (Mat1d(3, 3) <<
	//	f, 0, projectorResolution.x / 2.,
	//	0, f, projectorResolution.y / 2.,
	//	0, 0, 1);

	_cam_mat_a = (cv::Mat1d(3, 3) << 	_src_size_x, .0, _src_size_x / 2.,
										.0, _src_size_y, _src_size_y / 2.,
										.0, .0, 1.	);


	if( _b_fix_aspect_ratio_ui )			{ flag += CV_CALIB_FIX_ASPECT_RATIO ;	}
	if( _b_fix_principal_point_ui  )		{ flag += CV_CALIB_FIX_PRINCIPAL_POINT;	}

	if( _b_calib_fix_k1_ui  )				{ flag += CV_CALIB_FIX_K1;	}
	if( _b_calib_fix_k2_ui  )				{ flag += CV_CALIB_FIX_K2;	}
	if( _b_calib_fix_k3_ui  )				{ flag += CV_CALIB_FIX_K3;	}
	if( _b_calib_fix_k4_ui  )				{ flag += CV_CALIB_FIX_K4;	}
	if( _b_calib_fix_k5_ui  )				{ flag += CV_CALIB_FIX_K5;	}
	if( _b_calib_fix_k6_ui  )				{ flag += CV_CALIB_FIX_K6;	}

	_rvecs.clear();
	_tvecs.clear();

	_b_rectify = FALSE;

	_cam_mat_a = cv::Mat::eye( 3, 3, CV_64F );
	_cam_mat_b = cv::Mat::eye( 3, 3, CV_64F );
	if( _b_fix_aspect_ratio_ui )
	{
		_cam_mat_a.at<double>( 0,0 ) = 1.0;
		_cam_mat_b.at<double>( 0,0 ) = 1.0;
	}
	// 5 or 8 values depending on flags
	_dist_coeffs_a = cv::Mat::zeros( 8, 1, CV_64F );
	_dist_coeffs_b = cv::Mat::zeros( 8, 1, CV_64F );

	if( _b_stereo )
	{
		// stereo parameters only
		if( _b_same_focal_length_ui  )		{ flag += CV_CALIB_SAME_FOCAL_LENGTH;	}
		if( _b_fix_focal_point_ui  )		{ flag += CV_CALIB_FIX_FOCAL_LENGTH;	}
		_retroprojection_error = cv::stereoCalibrate(	_object_points, _image_points_a, _image_points_b,
														_cam_mat_a, _dist_coeffs_a, _cam_mat_b, _dist_coeffs_b, _src_cur->size(),
														_rot_mat, _tra_vec, _ess_mat, _fun_mat,
														flag, 
														cvTermCriteria(CV_TERMCRIT_ITER+CV_TERMCRIT_EPS, 100, 1e-5) );
													//	flag );		//CV_CALIB_FIX_ASPECT_RATIO + CV_CALIB_ZERO_TANGENT_DIST + CV_CALIB_SAME_FOCAL_LENGTH );
	}
	else
	{
		// start calibration
		cv::Mat	cam_m;
		cv::Mat	dist_c;
		_retroprojection_error = cv::calibrateCamera(	_object_points,			// the 3D points
														_image_points_a,		// the image points
														_src_cur->size(),		// image size
														_cam_mat_a,				// output camera matrix
														_dist_coeffs_a,			// output distortion matrix
														_rvecs, _tvecs,			// Rs, Ts 
														flag  );				// set options
														//	,CV_CALIB_USE_INTRINSIC_GUESS);
		_retroprojection_error_avg = compute_reprojection_errors();


	//	BOOL	b_ok = cv::checkRange(_camera_matrix_l) && cv::checkRange(_dist_coeffs_l);
	//	_camera_matrix_l = cv::getOptimalNewCameraMatrix(_camera_matrix_l, _dist_coeffs_l, image_size, 1.0 );
	}

	_b_calibrate = TRUE;
	_speed_calibrate->end();

	//cv::FileStorage fs( "m1.xml", cv::FileStorage::WRITE );

	//fs << "camera_matrix" << _camera_matrix_l;
	//fs << "distortion_coefficients" << _dist_coeffs_l;

	//fs << "rvecs" << rvecs;
	//fs << "tvecs" << tvecs;
//	mat_to_aaa();
}

void	c_bdd_img_cv_calibrate::mat_to_aaa_array( CONST cv::Mat& mat, REAL* values, INT32 size )
{
	//const double*	Mi = _dist_coeffs_a.ptr<double>(1);
	INT32 k = 0;
	CONST	double*	Mi = mat.ptr<double>(0);
	for( INT32 i = 0; i < mat.rows; ++i )
	{
		values[k] = Mi[i];
		++k;
		if( k > size )
		{
			ERR_PRINT_STRING( "%s(), trying to access wrong index", __FUNCTION__ );
			break;
		}
	}
}

void	c_bdd_img_cv_calibrate::mat_to_aaa_matrix( CONST cv::Mat& mat, REAL* values, INT32 size )
{
	INT32	k = 0;
	for( INT32 i = 0; i < mat.rows; ++i )
	{
		CONST	double*	Mi = mat.ptr<double>(i);
		for( INT32 j = 0; j < mat.cols; j++)
		{
			values[k] = Mi[j];
			++k;
			if( k > size )
			{
				ERR_PRINT_STRING( "%s(), trying to access wrong index", __FUNCTION__ );
				return;
			}
		}
	}
}

void	c_bdd_img_cv_calibrate::mat_to_aaa()
{
	// copy data to aaaseed structure
	mat_to_aaa_matrix(	_cam_mat_a,		_calibration_data->_cam_matrix_a_ui,	9 );
	mat_to_aaa_array(	_dist_coeffs_a, _calibration_data->_coeffs_a_ui,		8 );

	mat_to_aaa_matrix(	_cam_mat_b,		_calibration_data->_cam_matrix_b_ui,	9 );
	mat_to_aaa_array(	_dist_coeffs_b, _calibration_data->_coeffs_b_ui,		8 );

	// need to copy R and T
	mat_to_aaa_matrix(	_rot_mat,		_calibration_data->_rot_matrix_ui,		9 );
	mat_to_aaa_array(	_tra_vec,		_calibration_data->_tra_vector_ui,		3 );

	mat_to_aaa_matrix(	_ess_mat,		_calibration_data->_ess_matrix_ui,		9 );
	mat_to_aaa_matrix(	_fun_mat,		_calibration_data->_fun_matrix_ui,		9 );

	mat_to_aaa_matrix(	_r1,			_calibration_data->_r1_ui,				9 );
	mat_to_aaa_matrix(	_r2,			_calibration_data->_r2_ui,				9 );
	mat_to_aaa_matrix(	_p1,			_calibration_data->_p1_ui,				12 );
	mat_to_aaa_matrix(	_p2,			_calibration_data->_p2_ui,				12 );
	mat_to_aaa_matrix(	_q,				_calibration_data->_q_ui,				16 );
}

void	c_bdd_img_cv_calibrate::aaa_to_mat_array( cv::Mat& mat, CONST REAL* values, INT32 size )
{
	//const double*	Mi = _dist_coeffs_a.ptr<double>(1);
	INT32 k = 0;
	//mat = cv::Mat::zeros( size, 1, CV_64F );
	mat.create( size, 1, CV_64F);
	double*	Mi = mat.ptr<double>(0);
	for( INT32 i = 0; i < mat.rows; i++)
	{
		Mi[i] = values[k];
		++k;
		if( k > size )
		{
			ERR_PRINT_STRING( "%s(), trying to access wrong index", __FUNCTION__ );
			break;
		}
	}
}

void	c_bdd_img_cv_calibrate::aaa_to_mat_matrix( cv::Mat& mat, CONST REAL* values, UINT32 rows, UINT32 cols )
{
	INT32	k = 0;
	mat.create( rows, cols, CV_64F);
	for ( INT32 i = 0; i < mat.rows; i++)
	{
		double*	Mi = mat.ptr<double>(i);
		for( INT32 j = 0; j < mat.cols; j++)
		{
			Mi[j] = values[k];
			++k;
		}
	}
}

void	c_bdd_img_cv_calibrate::aaa_to_mat()
{

	aaa_to_mat_matrix(	_cam_mat_a,		_calibration_data->_cam_matrix_a_ui,	3, 3	);
	aaa_to_mat_array(	_dist_coeffs_a,	_calibration_data->_coeffs_a_ui,		8		);

	aaa_to_mat_matrix(	_cam_mat_b,		_calibration_data->_cam_matrix_b_ui,	3, 3	);
	aaa_to_mat_array(	_dist_coeffs_b,	_calibration_data->_coeffs_b_ui,		8		);

	aaa_to_mat_matrix(	_rot_mat,		_calibration_data->_rot_matrix_ui,		3, 3	);
	aaa_to_mat_array(	_tra_vec,		_calibration_data->_tra_vector_ui,		3		);

	aaa_to_mat_matrix(	_ess_mat,		_calibration_data->_ess_matrix_ui,		3, 3	);
	aaa_to_mat_matrix(	_fun_mat,		_calibration_data->_fun_matrix_ui,		3, 3	);

	aaa_to_mat_matrix(	_r1,			_calibration_data->_r1_ui,				3, 3	);
	aaa_to_mat_matrix(	_r2,			_calibration_data->_r2_ui,				3, 3	);
	aaa_to_mat_matrix(	_p1,			_calibration_data->_p1_ui,				3, 4	);
	aaa_to_mat_matrix(	_p2,			_calibration_data->_p2_ui,				3, 4	);
	aaa_to_mat_matrix(	_q,				_calibration_data->_q_ui,				4, 4	);
}

// pre compute rectification matrices
void	c_bdd_img_cv_calibrate::do_rectify()
{
	_speed_rectify->begin();
	//aaa_to_mat();
	_b_undistort_map = FALSE;
	if( _b_stereo )
	{
		// HARTLEY'S METHOD use intrinsic parameters of each camera, but compute the rectification transformation directly from the fundamental matrix
		if( _b_use_uncalibrated_ui )
		{
			if( _frames_chessboard_nb > 0 )
			{
				std::vector< cv::Point2f >	all_points[2];
				for( UINT32 i = 0; i < _frames_chessboard_nb; i++ )
				{
					std::copy( _image_points_a[i].begin(), _image_points_a[i].end(), std::back_inserter(all_points[0]) );
					std::copy( _image_points_b[i].begin(), _image_points_b[i].end(), std::back_inserter(all_points[1]) );
				}
				cv::Mat	F = cv::findFundamentalMat( all_points[0], all_points[1], cv::FM_8POINT );
				cv::Mat	H1, H2;
				cv::stereoRectifyUncalibrated( all_points[0], all_points[1], F, _src_cur->size(), H1, H2, 3 );

				cv::Mat	p1, p2, q;
				_r1 = _cam_mat_a.inv() * H1 * _cam_mat_a;
				_r2 = _cam_mat_b.inv() * H2 * _cam_mat_b;
				cv::Mat(all_points[0]).copyTo( _p1 );
				cv::Mat(all_points[1]).copyTo( _p2 );
			}
		}
		// CALIBRATED (BOUGUET'S METHOD)
		else
		{
			cv::stereoRectify( _cam_mat_a, _dist_coeffs_a, _cam_mat_b, _dist_coeffs_b, _src_cur->size(), _rot_mat, _tra_vec, _r1, _r2, _p1, _p2, _q, 0 );
			_b_is_vertical_stereo = fabs( _p2.at<double>(1, 3) ) > fabs( _p2.at<double>(0, 3) );
		}
	}
	else
	{
		cv::Mat	r1, p1;
		// calculate undistort mapping
		cv::initUndistortRectifyMap(	_cam_mat_a,				// computed camera matrix
										_dist_coeffs_a,			// computed distortion matrix
										r1,						// optional rectification (none) 
										p1,						// camera matrix to generate undistorted
										_src_cur->size(),		// size of undistorted
										CV_32FC1,				// type of output map
										_map_a_1, _map_a_2 );	// the x and y mapping functions
	}
	_b_rectify = TRUE;
	_speed_rectify->end();
}

// remove distortion in an image (after calibration)
void	c_bdd_img_cv_calibrate::do_undistort()
{
	_speed_undistort->begin();
	if( _b_stereo )
	{
		_b_is_vertical_stereo = fabs( _p2.at<double>(1, 3) ) > fabs( _p2.at<double>(0, 3) );
		//Precompute maps for cvRemap()
		cv::initUndistortRectifyMap( _cam_mat_a, _dist_coeffs_a, _r1, _p1, _src_cur->size(), CV_16SC2, _map_a_1, _map_a_2 );
		cv::initUndistortRectifyMap( _cam_mat_b, _dist_coeffs_b, _r2, _p2, _src_cur->size(), CV_16SC2, _map_b_1, _map_b_2 );
	}
	else
	{
		cv::Mat	r1, p1;
		// calculate undistort mapping
		cv::initUndistortRectifyMap(	_cam_mat_a,		// computed camera matrix
										_dist_coeffs_a,			// computed distortion matrix
										r1,						// optional rectification (none) 
										p1,						// camera matrix to generate undistorted
										_src_cur->size(),		// size of undistorted
										CV_32FC1,				// type of output map
										_map_a_1, _map_a_2 );	// the x and y mapping functions
	}
	_b_undistort_map = TRUE;
	_speed_undistort->end();
}

void	c_bdd_img_cv_calibrate::do_remap()
{
	_speed_remap->begin();
	// Apply mapping functions
	cv::remap( *_src_cur, _mat_dst_a, _map_a_1, _map_a_2, cv::INTER_LINEAR ); // interpolation type
	//c_img_2d*	img_a	= bind_img::get_ready( _dst_img_index_ui  );
	//img_a->set_changed();

	if( _b_stereo )
	{
		// Apply mapping functions
		cv::remap( *_src_b_cur, _mat_dst_b, _map_b_1, _map_b_2, cv::INTER_LINEAR ); // interpolation type
		//c_img_2d*	img_b	= bind_img::get_ready( _dst_b_img_index );
		//img_b->set_changed();

		if( _b_draw_epipolar_ui )
		{
			if( !_b_is_vertical_stereo )
			{
				for( INT32 j = 0; j < _src_cur->size().height; j += 16 )
				{
					cv::line( _mat_dst_a, cv::Point( 0, j ), cv::Point( _src_cur->size().width, j ), cv::Scalar(0,255,0) );
					cv::line( _mat_dst_b, cv::Point( 0, j ), cv::Point( _src_cur->size().width, j ), cv::Scalar(0,255,0) );
				}
			}
			else
			{
				for( INT32 j = 0; j < _src_cur->size().width; j += 16 )
				{
					cv::line( _mat_dst_a, cv::Point( j, 0 ), cv::Point( j, _src_cur->size().height ), cv::Scalar(0,255,0) );
					cv::line( _mat_dst_b, cv::Point( j, 0 ), cv::Point( j, _src_cur->size().height ), cv::Scalar(0,255,0) );
				}
			}
		}
	}
	_speed_remap->end();
}

namespace	{	CHAR CONST calib_ext[]	=  "calibration_data.0000" ;	};

AAA_ERR	c_bdd_img_cv_calibrate::load_do_after( o_str CONST & filename_in )
{
	c_file::push_vfile();	//not saved in the vfile we want to access it as a single file
		o_str& filename = fname::push_name( filename_in );
			filename.replace_ext( calib_ext );

			for( INT32 i = 1; i <= DATASET_ID_MAX; ++i )
			{
				filename.set_digits( -4, 4, i );
				if( c_file::is_exist(filename) )
				{
					c_calibration_data* data = get_data( i );
					data->load( filename );
				}
			}
		fname::pop_name();
	c_file::pop_vfile();

	return AAA_OK;
}

AAA_ERR	c_bdd_img_cv_calibrate::save_do_after( o_str CONST & filename_in )
{
	c_file::push_vfile();
		o_str& filename = fname::push_name( filename_in );
			filename.replace_ext( calib_ext );

			for( INT32 i = 1; i <= DATASET_ID_MAX; ++i )
			{
				c_calibration_data* data = find_data( i );
				if( data )
				{
					filename.set_digits( -4, 4, i );
					data->save( filename );
				}
			}
		fname::pop_name();
	c_file::pop_vfile();

	return AAA_OK;
}

void	c_bdd_img_cv_calibrate::set_dataset( INT32 dataset_id )
{
	if( _dataset_id != dataset_id )
	{
		dataset_id = WRAP_ID( dataset_id, DATASET_ID_MAX );
		_dataset_id = dataset_id;
		_calibration_data = find_data( dataset_id );
		if( !_calibration_data )
		{
			_calibration_data = get_data( dataset_id );
		}
		_b_undistort_map = FALSE;
	}
}

void	c_bdd_img_cv_calibrate::c_calibration_data::init_array( REAL* data, size_t nb )
{
	REAL*	pt = data-1;
	for( size_t i = 0; i < nb; ++i )
	{
		*++pt = .0f;
	}
}

void	c_bdd_img_cv_calibrate::c_calibration_data::init()
{
	init_array( _cam_matrix_a_ui,	9 );
	init_array(	_coeffs_a_ui,		8 );
	init_array(	_cam_matrix_b_ui,	9 );
	init_array(	_coeffs_b_ui,		8 );

	init_array(	_rot_matrix_ui,		9 );
	init_array(	_tra_vector_ui,		3 );
	init_array(	_ess_matrix_ui,		9 );
	init_array(	_fun_matrix_ui,		9 );

	init_array(	_r1_ui,				9 );
	init_array(	_r2_ui,				9 );
	init_array(	_p1_ui,				12 );
	init_array(	_p2_ui,				12 );
	init_array(	_q_ui,				16 );
}

c_bdd_img_cv_calibrate::c_calibration_data::c_calibration_data()
{
	init();
}

void	c_bdd_img_cv_calibrate::c_calibration_data::save( o_str CONST & name_info )
{
	FILE* file = file_csv::open( name_info.get(), TRUE );
		if( !file )
			ERR_PRINT_STRING( "%s : error opening file %s", __FUNCTION__, name_info );
		else
		{
			file_csv::write_real_line( file, _cam_matrix_a_ui,	9 );
			file_csv::write_real_line( file, _coeffs_a_ui,		8 );
			file_csv::write_real_line( file, _cam_matrix_b_ui,	9 );
			file_csv::write_real_line( file, _coeffs_b_ui,		8 );

			file_csv::write_real_line( file, _rot_matrix_ui,	9 );
			file_csv::write_real_line( file, _tra_vector_ui,	3 );
			file_csv::write_real_line( file, _ess_matrix_ui,	9 );
			file_csv::write_real_line( file, _fun_matrix_ui,	9 );

			file_csv::write_real_line( file, _r1_ui,			9 );
			file_csv::write_real_line( file, _r2_ui,			9 );
			file_csv::write_real_line( file, _p1_ui,			12 );
			file_csv::write_real_line( file, _p2_ui,			12 );
			file_csv::write_real_line( file, _q_ui,				16 );
	
			if( _name.get_len() > 0 )	fprintf( file, "%s\n",	_name.get() );
			else						fprintf( file, "\n"					 );
		}
	file_csv::close( file );
}

void	c_bdd_img_cv_calibrate::c_calibration_data::load( o_str CONST & name_info )
{
	FILE* file = file_csv::open( name_info.get(), FALSE );
		if( !file )
			ERR_PRINT_STRING( "%s : error opening file %s", __FUNCTION__, name_info );
		else
		{
			file_csv::read_float_grid( file, _cam_matrix_a_ui,	9,	1 );
			file_csv::read_float_grid( file, _coeffs_a_ui,		8,	1 );
			file_csv::read_float_grid( file, _cam_matrix_b_ui,	9,	1 );
			file_csv::read_float_grid( file, _coeffs_b_ui,		8,	1 );

			file_csv::read_float_grid( file, _rot_matrix_ui,	9,	1 );
			file_csv::read_float_grid( file, _tra_vector_ui,	3,	1 );
			file_csv::read_float_grid( file, _ess_matrix_ui,	9,	1 );
			file_csv::read_float_grid( file, _fun_matrix_ui,	9,	1 );

			file_csv::read_float_grid( file, _r1_ui,			9,	1 );
			file_csv::read_float_grid( file, _r2_ui,			9,	1 );
			file_csv::read_float_grid( file, _p1_ui,			12,	1 );
			file_csv::read_float_grid( file, _p2_ui,			12,	1 );
			file_csv::read_float_grid( file, _q_ui,				16,	1 );

			INT32	buf_len = 1024;
			CHAR	buffer[ 1024 ];
			CHAR	param[ 256 ];
			asc_line::get_next_line( file, buffer, buf_len );

			//sscanf( buffer, "calibration_name = \"%s\" ;\n", param );
			sscanf( buffer, "%s\n", param );
			_name.set( param );
		}
	file_csv::close( file );
}
