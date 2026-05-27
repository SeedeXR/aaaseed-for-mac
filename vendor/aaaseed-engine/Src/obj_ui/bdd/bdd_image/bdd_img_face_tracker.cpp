#include "bdd_img_face_tracker.h"
#include "face_tracker.h"
#include "draw/model.h"
#include "draw/primitive.h"
#include "draw/box.h"
#include "ui/alphabet.h"
#include "time/speed.h"
#include "gol/gol_matrix.h"
#include "infrastructure/seedfile.h"
#include "spy.h"


FACTORY_CREATE_PROP_V1( c_bdd_img_face_tracker, bdd_img_face_tracker, Image Face Tracker, bdd_img_face_tracker, sub_menu="Image"; );

namespace
{
	CONSTEXPR UINT32 BASE_PARAM_NB		= c_bdd_img::SUPER_RESIZE_PARAM_NB + 18;
	CONSTEXPR UINT32 TIMING_PARAM_NB	= 3;
	CONSTEXPR UINT32 IMG_OUT_PARAM_NB	= 4;
	CONSTEXPR UINT32 OUT_PARAM_NB		= 2;
	CONSTEXPR UINT32 FACE_PARAM_NB		= 8;
	CONSTEXPR UINT32 GROUP_PARAM_NB		= 3;
	CONSTEXPR INT32  PARAM_NB	=	BASE_PARAM_NB
								+	TIMING_PARAM_NB
								+	IMG_OUT_PARAM_NB
								+	OUT_PARAM_NB
								+	(FACE_PARAM_NB+1) * c_bdd_img_face_tracker::FACE_UI_NB
								+	GROUP_PARAM_NB;

#define	PARAM_DEF_FACE_INFO( nb )\
	PARAM_DEF_GROUP_CLOSED(		face_##nb, FACE_PARAM_NB	)\
		PARAM_DEF_BOOL_LOCKED(		face_##nb##_active		)\
		PARAM_DEF_REAL_LOCKED(		face_##nb##_certainty	)\
		PARAM_DEF_XYZ_LOCKED(		face_##nb##_tra			)\
		PARAM_DEF_XYZ_LOCKED(		face_##nb##_rot			)


	CONST c_param_def param[PARAM_NB] =
	{
		BDD_IMG_BASE_RESIZE_PARAMS

		PARAM_DEF_GROUP_CLOSED(	Timings, TIMING_PARAM_NB	)
			PARAM_DEF_REAL_LOCKED(	process_time			)
			PARAM_DEF_REAL_LOCKED(	detect_time				)
			PARAM_DEF_REAL_LOCKED(	landmark_time			)

		PARAM_DEF_SYMBO_PSTR_ZERO(	tracker,				c_face_tracker::type_str	)
		PARAM_DEF_FILENAME(			dlib_landmark_detector,	aaa::file::TYPE_IO_LANDMARK_DETECTOR, 0	)
		PARAM_DEF_INT32(			face_nb_max,			2, 1,	1, c_face_tracker::FACE_NB_MAX )
		PARAM_DEF_INT32(			detect_frame_skip,		1, 3,	0, 1024 )
		PARAM_DEF_REAL_ZERO_ONE(	detect_resize			)
		PARAM_DEF_INT32_LOCKED_XY(	detect_size				)
		PARAM_DEF_INT32(			detect_pyramid_levels_nb_max,	1, 8,	1, 1024 )	//	zero have no sense
		PARAM_DEF_INT32(			failures_max,			1, 4,	1, 32 )
//		PARAM_DEF_INT32(			extend_pixel,			1, 0,	0, 128 )
		PARAM_DEF_BOOL_OFF(			landmark_keep_rgb		)
//		PARAM_DEF_REAL_ZERO_ONE(	max_travel				)
		PARAM_DEF_INT32(			average_nb,				2, 1,	1, c_face_tracker::c_face_info::AVERAGE_NB_MAX )
		PARAM_DEF_DOUBLE_ONE_ZERO(	filter_landmark			)
	
		PARAM_DEF_BOOL_OFF(			draw_cube				)
		PARAM_DEF_BOOL_ON(			draw_point				)
		PARAM_DEF_BOOL_ON(			draw_face_2d			)
		PARAM_DEF_BOOL_ON(			draw_face_3d			)
		PARAM_DEF_BOOL_OFF(			draw_face_number		)
		PARAM_DEF_REAL_POS(			draw_face_number_size,	1., .1	)

		PARAM_DEF_GROUP(	Img Out, IMG_OUT_PARAM_NB		)
			PARAM_DEF_BOOL_ON(		img_out					)
			PARAM_DEF_BOOL_OFF(		img_out_mark			)
			PARAM_DEF_BOOL_OFF(		img_out_draw_tracker	)
			PARAM_DEF_BOOL_OFF(		img_out_draw_box		)

		PARAM_DEF_GROUP_CLOSED(	Out, OUT_PARAM_NB		)
			PARAM_DEF_INT32_LOCKED(	face_detected_nb		)
			PARAM_DEF_INT32_LOCKED(	model_active_nb			)

		PARAM_DEF_FACE_INFO( 1 )
		PARAM_DEF_FACE_INFO( 2 )
		PARAM_DEF_FACE_INFO( 3 )
		PARAM_DEF_FACE_INFO( 4 )
	};

}


//INT32 extend_pixel;
//REAL max_travel;

void	c_bdd_img_face_tracker::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start_src_resize( false, true );

	++h;
		param_set_pt( h, _speed_bdd->get_pt_interval_ms()	);
		param_set_pt( h, _face_tracker->_speed_detect->get_pt_interval_ms()		);
		param_set_pt( h, _face_tracker->_speed_landmark->get_pt_interval_ms()	);

	param_set_pt( h, _face_tracker->_s_type_ui							);
	param_set_pt( h, _face_tracker->_dlib_landmark_detection_filename	);
	param_set_pt( h, _face_tracker->_model_max_ui						);
	param_set_pt( h, _face_tracker->_detect_frame_skip_ui				);
	param_set_pt( h, _detect_resize_ui									);
	param_set_pt( h, _detect_size_x										);
	param_set_pt( h, _detect_size_y										);
	param_set_pt( h, _face_tracker->_detect_pyramid_level_max_ui		);
	param_set_pt( h, _face_tracker->_failures_max_ui					);
//	param_set_pt( h, extend_pixel										);
	param_set_pt( h, _b_keep_rgb										);
//	param_set_pt( h, max_travel											);
	param_set_pt( h, _face_tracker->_average_nb							);
	param_set_pt( h, _face_tracker->_filter_ui							);

	param_set_pt( h, _b_draw_cube_ui			);
	param_set_pt( h, _b_draw_points_ui			);
	param_set_pt( h, _b_draw_face_2d_ui			);
	param_set_pt( h, _b_draw_face_3d_ui			);
	param_set_pt( h, _b_draw_face_number_ui		);
	param_set_pt( h, _draw_face_number_size_ui	);
	
	++h;
		param_set_pt( h, _face_tracker->_b_cv_draw				);
		param_set_pt( h, _b_mark_image							);
		param_set_pt( h, _face_tracker->_b_cv_draw_tracker		);
		param_set_pt( h, _face_tracker->_b_cv_draw_box			);

	++h;
		param_set_pt( h, _face_tracker->_face_detected_nb		);
		param_set_pt( h, _model_active_nb						);

	for( INT32 i=0; i<FACE_UI_NB; ++i )
	{
		++h;
		const auto& fi = _face_tracker->_face_infos[i];
			param_set_pt(		h, fi._b_active		);
			param_set_pt(		h, fi._certainty	);
			param_set_pt_3(	h, fi._tra			);
			param_set_pt_3(	h, fi._rot			);
	}

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bdd_img_face_tracker )
{
	_face_tracker = new c_face_tracker;

	_speed_bdd = new c_speed( false, 4, g_speed_master->get_pb_print_bdd(),	"update"			, this );

//	obj_get(	_speed_bdd_ui		);
//	_speed_bdd_ui->set_timing(			_speed_bdd			);

	param_init_with( param, PARAM_NB );
}

void	c_bdd_img_face_tracker::dealloc()
{
	IF_THIS_NULL_RETURN();

	_depth.release();
	_detect.release();
	_landmark.release();
//	_mat_dst_a.release();

	obj_delete( _face_tracker );
	obj_delete( _speed_bdd );
//	obj_delete( _speed_bdd_ui );
}

c_bdd_img_face_tracker::~c_bdd_img_face_tracker()
{
	dealloc();
}

void	c_bdd_img_face_tracker::update()
{
	_speed_bdd->begin();

	_face_tracker->update_engine( 0, nullptr );
	//todo make it better

	auto eng = _face_tracker->get_engine();
	if( eng->is_grey_forced() )
		_b_mono_ui = eng->is_grey_used();
	if( eng->is_float_forced() )
		_b_float_ui = eng->is_float_used();

	bool b_change_it;
	if( update_mat_src(b_change_it) )
	{
		update_tracker();

		//if( !build_cv_mat( _mat_dst_a, _dst_img_index_ui, _src_cur, "bdd_img_opencv" ) )
		//{
		//	err_print( "Error building dst" );
		//	return; 
		//}
		//_src_cur.copyTo( _mat_dst_a );
		update_dst_index( _dst_img_index_ui, "bdd_img_opencv" );
		//hack for quartz face
		//tex_2d_bind( _dst_img_index_ui );

		SPY_PUSH_RANGE( "store_landmark", spy::COL_1 );
			if( _src_cur )
			{
				c_model::cur->get_size_v3(_size);
				//neg_v3r( _size );
				_face_tracker->store_landmark( REAL(_landmark.cols), REAL(_landmark.rows) );
			}
		SPY_POP_RANGE();
	}

	_face_tracker->process_landmark( _size );

	_speed_bdd->end();
}

void	c_bdd_img_face_tracker::update_tracker()
{		
	bool use_depth = false;	//maa	!depth_directories.empty();	

/*
	if(use_depth)
	{
		char* dst = new char[100];
		std::stringstream sstream;

		sstream << depth_directories[f_n] << "\\depth%05d.png";
		sprintf(dst, sstream.str().c_str(), frame_count + 1);
		// Reading in 16-bit png image representing depth
		Mat_<short> depth_image_16_bit = imread(string(dst), -1);

		// Convert to a floating point depth image
		if(!depth_image_16_bit.empty())
		{
			depth_image_16_bit.convertTo(depth_image, CV_32F);
		}
		else
		{
			WARN_STREAM( "Can't find depth image" );
		}
	}
*/
	SPY_PUSH_RANGE( "Prepare Images", spy::IMG_LOW );
//	Mat_<uchar> grayscale_image_in;
	bool b_detect = (get_process_index() % (_face_tracker->_detect_frame_skip_ui + 1 )) == 0;

	if( _src_cur->channels() == 4 )
		cv::cvtColor( *_src_cur, _landmark, _b_keep_rgb ? cv::COLOR_RGBA2RGB : cv::COLOR_RGBA2BGR ); 
	else if( _src_cur->channels() == 3 )
	{
		if( _b_keep_rgb )
			_landmark = *_src_cur;
		else
			cv::cvtColor( *_src_cur, _landmark, cv::COLOR_RGB2BGR );

	}
	else if( _src_cur->channels() == 1 )
	{
		//	grayscale_image_in = _src_cur->clone();
		//_src_cur->copyTo( _grayscale_image );
		_landmark = *_src_cur;
	}
	else
	{
		ERR_PRINT_STRING( "%d channel unimplemented here", _src_cur->channels() );
		return;
	}

	cv::UMat* detect;
	if( b_detect )
	{
		detect = &_landmark;
		if( _detect_resize_ui!=1. )
		{
			INT32 size_x = INT32(_landmark.cols * _detect_resize_ui);
			INT32 size_y = INT32(_landmark.rows * _detect_resize_ui);
			
			if( size_x>=4 && size_y>=4 )
			{
				// Resize image for face detection
				SPY_PUSH_RANGE( "cv::resize", spy::IMG_LOW );
					cv::resize( _landmark, _detect, cv::Size(size_x,size_y), 0, 0 );
				SPY_POP_RANGE();
				detect = &_detect;
			}
		}	
		_detect_size_x = detect->cols;
		_detect_size_y = detect->rows;
	}
	else
	{
		detect = nullptr;
	}
	SPY_POP_RANGE();

	_face_tracker->update( detect, &_landmark, _depth );
	_src_cur = &_landmark;

	_model_active_nb = _face_tracker->get_model_active_nb();

	if( _face_tracker->_b_cv_draw )
	{
		if( _b_mark_image )
		{
			INT32 sx = _landmark.cols;
			INT32 sy = _landmark.rows;
			for( INT32 i = get_process_index() % 32; i < sx; i+=32 )
			{
				cv::Point p1( i, 0 );
				cv::Point p2( i, sy/2 );
				cv::line( _landmark, p1, p2, cv::Scalar(256,0,256), 1 );
			}
		}

		_src_cur = &_landmark;
	}
}
void draw_prim( DOUBLE* pts, int nb, GLenum prim )
{
	auto vec = c_prim3::base.alloc_vertex( nb );
	//todo do a fn here
	for( INT32 i=0; i<nb; ++i )
	{
		cpy_v3( vec, pts );
		vec += 3;
		pts += 3;
	}
	c_prim3::base.draw( prim, nb );
}

void	c_bdd_img_face_tracker::draw_points( DOUBLE* pts, INT32 nb )
{
	draw_prim( pts, nb, GL_POINTS );
}

void c_bdd_img_face_tracker::draw_line( DOUBLE* pts, int start, int stop, bool b_loop )
{
	draw_prim(  pts + (start-1) * 3, stop - start + 1, b_loop ? GL_LINE_LOOP : GL_LINE_STRIP );
}

void	c_bdd_img_face_tracker::draw_landmark( DOUBLE* pts, INT32 nb, bool b_text )
{
	draw_line(	pts,	1,	17, false	);		//	side chin
	draw_line(	pts,	18,	22, false	);		//	eye
	draw_line(	pts,	23,	27, false	);		//	eye
	draw_line(	pts,	28,	31, false	);		//	nose vertical

	//draw_line(	30, 33, false );

	draw_line(	pts,	32,	36, false	);
	draw_line(	pts,	37,	42, true	);
	draw_line(	pts,	43,	48, true	);
	draw_line(	pts,	49,	60, true	);
	draw_line(	pts,	61,	68, true	);

	if(	b_text	)
	{
		for( int i = 0; i < nb; ++i )
		{
			GOL::matrix::push();
				GOL::matrix::translatedv( pts + i*3 );
				GOL::matrix::scale( _draw_face_number_size_ui );
				aaa::alphabet::draw_int32( i+1, 0 );
			GOL::matrix::pop();
		}
	}
}

void	c_bdd_img_face_tracker::draw()
{
/*
	INT32 nb = _face_detections.size();

	for( auto const & r : _face_detections )
	{
		draw_rect_line( r.x / _src_size_x, r.y / _src_size_y, r.width / _src_size_x, r.height / _src_size_y );
	}
*/
	if( _src_cur )
	{
		c_model::cur->get_size_v3(_size);
		//neg_v3r( _size );

		//REAL sx = _src_cur->cols;
		//REAL sy = _src_cur->rows;
		for( UINT32 i = 0; i < _face_tracker->_active_models.size(); ++i )
		{
			if( _face_tracker->_active_models[i] )
			{				
				if( _b_draw_cube_ui )
				{
					REAL	pos[3];
					REAL	rot[3];
					_face_tracker->get_cube_pos_rot( i, pos, rot );

					GOL::matrix::push( );
						GOL::matrix::translate( pos[0]*_size[0], pos[1]*_size[1], pos[2]*_size[2] );
						GOL::matrix::rotate_z( rot[2] );
						GOL::matrix::rotate_y( rot[1] );
						GOL::matrix::rotate_x( rot[0] );
						//
						draw_cube( REAL(1) );
					GOL::matrix::pop();
				}
				
				c_face_tracker::c_face_info& fi = _face_tracker->_face_infos[i];
				if( _b_draw_points_ui )
					draw_points( fi._point[0], fi.get_point_nb() );

				if( _b_draw_face_2d_ui )
					draw_landmark( fi._point[0], fi.get_point_nb(), _b_draw_face_number_ui );

				if( _b_draw_face_3d_ui )
					draw_landmark( fi._point_3d[0], fi.get_point_nb(), _b_draw_face_number_ui );
			}
		}
	}
}

INT32	c_bdd_img_face_tracker::get_point_dataset_nb()
{	
	return _model_active_nb;
}

FINLINE INT32	c_bdd_img_face_tracker::get_face_dataset()
{	
	return ( _dataset_id <= _model_active_nb ) ? _dataset_id : 0;
}

bool	c_bdd_img_face_tracker::set_point_dataset( INT32 dataset_id )
{
	if( dataset_id <= _model_active_nb )
	{
		_dataset_id = dataset_id;
		return true;
	}
	return false;
}

INT32	c_bdd_img_face_tracker::get_point_nb()
{
	INT32 id = get_face_dataset();
	return (id > 0) ? _face_tracker->_face_infos[id-1].get_point_nb() : 0;
}

bool	c_bdd_img_face_tracker::get_point( REAL* CONST dst, INT32 CONST index )	
{
	INT32 id = get_face_dataset();
	if( id > 0 && index < get_point_nb() )
	{
		cpy_v2( dst, _face_tracker->_face_infos[id-1]._point_cano[index] );
		dst[2] = 0.;
		return true;
	}
	clear_v3( dst );
	return false;
}
