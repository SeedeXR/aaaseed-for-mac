
#include "bdd_img_cv_calibrate.h"
#include "bdd_img_cv_stereo_point.h"
#include "draw/bind_img.h"
#include "infrastructure/namer.h"
#include "gol.h"
#include "spy.h"
#include "time/ourtime.h"
#include "wrapper/aaa_opencv.h"


FACTORY_CREATE_PROP_V1( c_bdd_img_cv_stereo_point, bdd_img_cv_stereo_point, Image OpenCV Stereo Point, bdd_img_cv_stereo_point, sub_menu="Image"; );

namespace n_bdd_img_cv_stereo_point
{
	static	CONST	INT32	BASE_PARAM_NB				= 10 + c_bdd_img::SUPER_PARAM_NB;
	static	CONST	INT32	THRESHOLD_PARAM_NB			= 8;
	static	CONST	INT32	BOUNDING_BOX_PARAM_NB		= 12;
	static	CONST	INT32	TRANSFO_PARAM_NB			= 8;
	static	CONST	UINT32	DISPARITY_MATRIX_PARAM_NB	= 16;
	static	CONST	INT32	GROUP_PARAM_NB				= 4;

	static	CONST	INT32	PARAM_NB				=	BASE_PARAM_NB
													+	THRESHOLD_PARAM_NB
													+	BOUNDING_BOX_PARAM_NB
													+	TRANSFO_PARAM_NB
													+	DISPARITY_MATRIX_PARAM_NB
													+	GROUP_PARAM_NB;

	static	ST_PARAM	param[PARAM_NB] =
	{
		BDD_IMG_BASE_PARAMS
		ST_PARAM_BOOL_OFF(		use_opencv_reproject	)
		ST_PARAM_BOOL_OFF(		use_bdd_ref				)
		ST_PARAM_REF(			calibrate_ref_name		)
		ST_PARAM_BOOL_OFF(		draw					)
		ST_PARAM_BOOL_OFF(		draw_points				)
		ST_PARAM_BOOL_OFF(		draw_disparity			)
		ST_PARAM_BOOL_OFF(		draw_bbox				)
		ST_PARAM_BOOL_OFF(		normalize				)
		ST_PARAM_REAL_LOCKED(	speed_bdd				)
		ST_PARAM_REAL_LOCKED(	speed_reproject			)
		ST_PARAM_GROUP( Threshold, THRESHOLD_PARAM_NB )
			ST_PARAM_BOOL_OFF(		threshold				)
			ST_PARAM_BOOL_OFF(		threshold_reset_trig	)
			ST_PARAM_POINT_XYZ(		threshold_min			)
			ST_PARAM_POINT_XYZ(		threshold_max			)

		ST_PARAM_GROUP( Bounding Box, BOUNDING_BOX_PARAM_NB )
			ST_PARAM_XYZ_LOCKED(	bbox_min			)
			ST_PARAM_XYZ_LOCKED(	bbox_max			)
			ST_PARAM_XYZ_LOCKED(	thresh_min			)
			ST_PARAM_XYZ_LOCKED(	thresh_max			)

		ST_PARAM_GROUP( Transfo, TRANSFO_PARAM_NB )
			ST_PARAM_BOOL_OFF(		transfo_do		)
			ST_PARAM_POINT_XYZ(		transfo_center		)
			ST_PARAM_SCALE_XYZF(	transfo_size		)


#define	ST_PARAM_ARRAY_16( param_name )\
	ST_PARAM_REAL_INF( param_name##_01, 1.0, .0 )\
	ST_PARAM_REAL_INF( param_name##_02, 1.0, .0 )\
	ST_PARAM_REAL_INF( param_name##_03, 1.0, .0 )\
	ST_PARAM_REAL_INF( param_name##_04, 1.0, .0 )\
	ST_PARAM_REAL_INF( param_name##_05, 1.0, .0 )\
	ST_PARAM_REAL_INF( param_name##_06, 1.0, .0 )\
	ST_PARAM_REAL_INF( param_name##_07, 1.0, .0 )\
	ST_PARAM_REAL_INF( param_name##_08, 1.0, .0 )\
	ST_PARAM_REAL_INF( param_name##_09, 1.0, .0 )\
	ST_PARAM_REAL_INF( param_name##_10, 1.0, .0 )\
	ST_PARAM_REAL_INF( param_name##_11, 1.0, .0 )\
	ST_PARAM_REAL_INF( param_name##_12, 1.0, .0 )\
	ST_PARAM_REAL_INF( param_name##_13, 1.0, .0 )\
	ST_PARAM_REAL_INF( param_name##_14, 1.0, .0 )\
	ST_PARAM_REAL_INF( param_name##_15, 1.0, .0 )\
	ST_PARAM_REAL_INF( param_name##_16, 1.0, .0 )

		ST_PARAM_GROUP_CLOSED( Disparity Depth Matrix, DISPARITY_MATRIX_PARAM_NB )
			ST_PARAM_ARRAY_16( q )
	};
}

static	o_str sum_up;

void	c_bdd_img_cv_stereo_point::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt( h, _b_cv_reproject_ui				);
	param_set_pt( h, _b_use_bdd_ref_ui				);
	param_set_pt( h, _calibrate_bdd_sym_name_ui		);

	param_set_pt( h, _b_draw_ui						);
	param_set_pt( h, _b_draw_points_ui				);
	param_set_pt( h, _b_draw_disp_ui				);
	param_set_pt( h, _b_draw_bbox_ui				);
	param_set_pt( h, _b_normalize_ui				);

	param_set_pt( h, _speed_bdd->get_pt_interval_ms()			);
	param_set_pt( h, _speed_stereo_points->get_pt_interval_ms()	);

	++h;
		param_set_pt( h, _b_threshold_ui				);
		param_set_pt( h, _b_threshold_trig_ui			);
		param_set_pt_v3( h, _threshold_min_ui			);
		param_set_pt_v3( h, _threshold_max_ui			);

	++h;
		param_set_pt_v3( h, _bbox_min				);
		param_set_pt_v3( h, _bbox_max				);
		param_set_pt_v3( h, _bbox_thresh_min		);
		param_set_pt_v3( h, _bbox_thresh_max		);

	++h;
		param_set_pt( h, _b_transfo_ui				);
		param_set_pt_v3( h, _transfo_translate_ui	);
		param_set_pt_v4( h, _transfo_scale_ui		);

	++h;
		param_set_pt_v( h, _q_ui, 16				);

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bdd_img_cv_stereo_point )
	,_bdd_target		( nullptr )
	,_points_nb			( 0 )
{
	_b_mono_ui = true;
	_speed_bdd				= new c_speed( TRUE, 4, "update"		, this );
	_speed_stereo_points	= new c_speed( TRUE, 4, "reproject3D"	, this );

	param_init_with( n_bdd_img_cv_stereo_point::param, n_bdd_img_cv_stereo_point::PARAM_NB );
}

void	c_bdd_img_cv_stereo_point::close()
{
	if( this )
	{
		close_mat();
		delete	_speed_bdd;
		delete	_speed_stereo_points;
	}
}

void	c_bdd_img_cv_stereo_point::close_mat()
{
	_src_cur		= NULL;
	_src_cur_gpu	= NULL;
	_points_3d_gpu.release();
	_points_3d.release();
}

c_bdd_img_cv_stereo_point::~c_bdd_img_cv_stereo_point()
{
	close();
}

void	c_bdd_img_cv_stereo_point::update()
{
	_speed_bdd->begin();
	if( update_mat_src() )
	{
		do_process();
		if( _b_threshold_trig_ui )
		{
			_b_threshold_trig_ui = FALSE;
			cpy_v3r( _threshold_min_ui, _bbox_min );
			cpy_v3r( _threshold_max_ui, _bbox_max );
		}
	}
	_speed_bdd->end( g_speed_master->is_print_bdd() );
}


void	c_bdd_img_cv_stereo_point::draw_bbox( REAL* min, REAL* max )
{
	// Draw Bounding box
	GOL::begin( GL_LINE_LOOP );
		GOL::vertex3( min[0], min[1], min[2] );
		GOL::vertex3( min[0], max[1], min[2] );
		GOL::vertex3( max[0], max[1], min[2] );
		GOL::vertex3( max[0], min[1], min[2] );
	GOL::end();
	GOL::begin( GL_LINE_LOOP );
		GOL::vertex3( min[0], min[1], max[2] );
		GOL::vertex3( min[0], max[1], max[2] );
		GOL::vertex3( max[0], max[1], max[2] );
		GOL::vertex3( max[0], min[1], max[2] );
	GOL::end();
	GOL::begin( GL_LINES );
		GOL::vertex3( min[0], min[1], min[2] );
		GOL::vertex3( min[0], min[1], max[2] );
		GOL::end();
	GOL::begin( GL_LINES );
		GOL::vertex3( min[0], max[1], min[2] );
		GOL::vertex3( min[0], max[1], max[2] );
	GOL::end();
	GOL::begin( GL_LINES );
		GOL::vertex3( max[0], max[1], min[2] );
		GOL::vertex3( max[0], max[1], max[2] );
	GOL::end();
	GOL::begin( GL_LINES );
		GOL::vertex3( max[0], min[1], min[2] );
		GOL::vertex3( max[0], min[1], max[2] );
	GOL::end();
}

void	c_bdd_img_cv_stereo_point::draw()
{
	if( _b_draw_ui )
	{

		GOL::push_matrix();

		GOL::color_white4();
		GOL::begin( GL_POINTS );
			if( _b_draw_points_ui )
			{
				size_t	k = 0;
				for ( INT32 j = 0; j < _points_3d.rows; j++)
				{
					for( INT32 i = 0; i < _points_3d.cols; i++)
					{
						cv::Point3f p = _points_3d.at<cv::Point3f>(j,i);
						if( p.z != REAL_BIG_VALUE )
						{
							REAL	r[3];
							r[0] = p.x;
							r[1] = p.y;
							r[2] = p.z;
							GOL::vertex3v( r );
						}
					}
				}
			}
			if( _b_draw_disp_ui )
			{
				for ( INT32 i = 0; i < _src_cur.rows; i++)
				{
					CONST	UINT8*	Mi = _src_cur.ptr<UINT8>(i);
					for( INT32 j = 0; j < _src_cur.cols; j++)
					{
						REAL	r[3];
						r[0] = static_cast<REAL>(i);
						r[1] = static_cast<REAL>(j);
						r[2] = static_cast<REAL>(Mi[j]);
						GOL::vertex3v( r );
					}
				}
			}
		GOL::end();
		if( _b_draw_bbox_ui )
		{
			REAL	bbox_min[3];
			REAL	bbox_max[3];
			REAL	bbox_t_min[3];
			REAL	bbox_t_max[3];
			if( _b_normalize_ui )
			{
				bbox_min[0] = -1.0;
				bbox_min[1] = -1.0;
				bbox_min[2] = -1.0;
				bbox_max[0] = 1.0;
				bbox_max[1] = 1.0;
				bbox_max[2] = 1.0;
				bbox_t_min[0] = -1.0;
				bbox_t_min[1] = -1.0;
				bbox_t_min[2] = -1.0;
				bbox_t_max[0] = 1.0;
				bbox_t_max[1] = 1.0;
				bbox_t_max[2] = 1.0;
			}
			else
			{
				cpy_v3r( bbox_min, _bbox_min );
				cpy_v3r( bbox_max, _bbox_max );
				cpy_v3r( bbox_t_min, _bbox_thresh_min );
				cpy_v3r( bbox_t_max, _bbox_thresh_max );
			}
			if( _b_transfo_ui )
			{
				REAL	sca[3];
				scale_v3r( sca, _transfo_scale_ui, _transfo_scale_ui[3] );
				add_mul_v3r( bbox_min, _transfo_translate_ui, bbox_min, sca );
				add_mul_v3r( bbox_max, _transfo_translate_ui, bbox_max, sca );
				add_mul_v3r( bbox_t_min, _transfo_translate_ui, bbox_t_min, sca );
				add_mul_v3r( bbox_t_max, _transfo_translate_ui, bbox_t_max, sca );
			}
			GOL::color_blue4();
			draw_bbox( bbox_min, bbox_max );
			GOL::color_green4();
			draw_bbox( bbox_t_min, bbox_t_max );
		}
		GOL::pop_matrix();
	}
}

void	c_bdd_img_cv_stereo_point::do_normalize()
{
	REAL	min[3];
	REAL	scale[3];
	if( _b_threshold_ui )
	{
		for( size_t i = 0 ; i < 3; ++i )
		{
			min[i]		= _threshold_min_ui[i];
			scale[i]	= 2.f / ( _threshold_max_ui[i] - _threshold_min_ui[i] );
		}
	}
	else
	{
		for( size_t i = 0 ; i < 3; ++i )
		{
			min[i]		= _bbox_min[i];
			scale[i]	= 2.f / ( _bbox_max[i] - _bbox_min[i] );
		}
	}

	for ( INT32 j = 0; j < _points_3d.rows; j++ )
	{
		for( INT32 i = 0; i < _points_3d.cols; i++ )
		{
			cv::Point3f p = _points_3d.at<cv::Point3f>( j, i );
			if( p.z != REAL_BIG_VALUE )
			{
				p.x = ( p.x - min[0] ) * scale[0] - 1.f;
				p.y = ( p.y - min[1] ) * scale[1] - 1.f;
				p.z = ( p.z - min[2] ) * scale[2] - 1.f;

				_points_3d.at<cv::Point3f>(j,i) = p;
			}
		}
	}
}

void	c_bdd_img_cv_stereo_point::do_transfert()
{
	REAL	sca[3];

	scale_v3r( sca, _transfo_scale_ui, _transfo_scale_ui[3] );

	for ( INT32 j = 0; j < _points_3d.rows; j++ )
	{
		for( INT32 i = 0; i < _points_3d.cols; i++ )
		{
			cv::Point3f p = _points_3d.at<cv::Point3f>( j, i );
			if( p.z != REAL_BIG_VALUE )
			{
				p.x = ( p.x * sca[0] ) + _transfo_translate_ui[0] ;
				p.y = ( p.y * sca[1] ) + _transfo_translate_ui[1] ;
				p.z = ( p.z * sca[2] ) + _transfo_translate_ui[2] ;
				_points_3d.at<cv::Point3f>(j,i) = p;
			}
		}
	}
}

void	c_bdd_img_cv_stereo_point::do_process()
{
	tbuf_add( c_tbuf_master::CH_OPEN_CV, 1., "bdd_img_cv_stereo", this );
	_speed_stereo_points->begin();
	if( _b_use_bdd_ref_ui && _calibrate_bdd_sym_name_ui.is_not_empty() )
	{

		if( !_bdd_target->is_sym_name( _calibrate_bdd_sym_name_ui ) || !_bdd_target->get_root() )
		{
			_bdd_target = NULL;
			c_bdd* bdd = find_bdd_by_sym_name( _calibrate_bdd_sym_name_ui );
			if( bdd )
			{
				if( bdd->is_class_name( "bdd_img_cv_calibrate" ) )
					_bdd_target = reinterpret_cast<c_bdd_img_cv_calibrate*>(bdd);
				else
					ERR_PRINT_STRING( "%s() found a bdd but this not a bdd_img_cv_calibrate", __FUNCTION__ );
			}
		}
		if( _bdd_target )
		{
			aaa_to_mat_matrix( _q, _bdd_target->get_matrix_q(), 4, 4 );
		}
		else
		{
			goto goto_fail;
		}
	}
	else
	{
		//_bdd_target = NULL;
		aaa_to_mat_matrix( _q, _q_ui, 4, 4 );
	}
	_points_nb = 0;
	for( size_t i = 0; i < 3; i++ )
	{
		_bbox_min[i]		= REAL_BIG_VALUE;
		_bbox_max[i]		= -REAL_BIG_VALUE;
		_bbox_thresh_min[i] = REAL_BIG_VALUE;
		_bbox_thresh_max[i] = -REAL_BIG_VALUE;
	}
	if( _b_cv_reproject_ui )
	{
		// GPU methods
		if( _b_gpu_enable_ui )
		{
			if( c_aaa_cv::is_gpu_ok() )
			{
			//	cv::gpu::GpuMat points_3d_gpu(_src_cur_gpu.size(), CV_32FC4 );
				cv::gpu::reprojectImageTo3D( _src_cur_gpu, _points_3d_gpu, _q, 3 );
			//	cv::gpu::cvtColor( points_3d_gpu, _points_3d_gpu, CV_BGRA2BGR );
				_points_3d_gpu.download( _points_3d );
			}
		}
		else
		{
			cv::reprojectImageTo3D( _src_cur, _points_3d, _q );
		}
		for( INT32 j = 0; j < _points_3d.rows; j++)
		{
			for( INT32 i = 0; i < _points_3d.cols; i++)
			{
				cv::Point3f p = _points_3d.at<cv::Point3f>(j,i);
				if( p.x > _bbox_max[0] )
					_bbox_max[0] = p.x;
				if( p.x < _bbox_min[0] )
					_bbox_min[0] = p.x;
				if( p.y > _bbox_max[1] )
					_bbox_max[1] = p.y;
				if( p.y < _bbox_min[1] )
					_bbox_min[1] = p.y;
				if( p.z > _bbox_max[2] )
					_bbox_max[2] = p.z;
				if( p.z < _bbox_min[2] )
					_bbox_min[2] = p.z;

				BOOL	b_discarded = FALSE;
				if( ( p.z < _threshold_min_ui[2] ) || ( p.z > _threshold_max_ui[2] ) ||
					( p.y < _threshold_min_ui[1] ) || ( p.y > _threshold_max_ui[1] ) ||
					( p.x < _threshold_min_ui[0] ) || ( p.x > _threshold_max_ui[0] ) )
				{
					if ( _b_threshold_ui )
					{
						p.x = .0;
						p.y = .0;
						p.z = REAL_BIG_VALUE;
						_points_3d.at<cv::Point3f>(j,i) = p;
						b_discarded = TRUE;
					}
				}
				else
				{
					if( p.x > _bbox_thresh_max[0] )
						_bbox_thresh_max[0] = p.x;
					if( p.x < _bbox_thresh_min[0] )
						_bbox_thresh_min[0] = p.x;
					if( p.y > _bbox_thresh_max[1] )
						_bbox_thresh_max[1] = p.y;
					if( p.y < _bbox_thresh_min[1] )
						_bbox_thresh_min[1] = p.y;
					if( p.z > _bbox_thresh_max[2] )
						_bbox_thresh_max[2] = p.z;
					if( p.z < _bbox_thresh_min[2] )
						_bbox_thresh_min[2] = p.z;
				}
				if( !b_discarded )
					_points_nb++;
			}
		}
	}
	else
	{
		_points_3d.create(_src_cur.size(), CV_MAKETYPE(CV_32FC1, 3));
		//Get the interesting parameters from Q
		double	Q03, Q13, Q23, Q32, Q33;
		Q03 = _q.at<float>(0,3);
		Q13 = _q.at<float>(1,3);
		Q23 = _q.at<float>(2,3);
		Q32 = _q.at<float>(3,2);
		Q33 = _q.at<float>(3,3);
		double px, py, pz;
		for( int i = 0; i < _src_cur.rows; i++ )
		{
		//	uchar* rgb_ptr = img_rgb.ptr<uchar>(i);
			uchar* disp_ptr = _src_cur.ptr<uchar>(i);
			float*  dptr = _points_3d.ptr<float>(i);
			for ( int j = 0; j < _src_cur.cols; j++ )
			{
				BOOL	b_discarded = FALSE;
				//Get 3D coordinates
				uchar d = disp_ptr[j];
				if ( d == 0 )
				{
						//continue; //Discard bad pixels
					px = .0;
					py = .0;
					pz = REAL_BIG_VALUE;
					b_discarded = TRUE;
				}
				else
				{
					double pw = -1.0 * static_cast<double>(d) * Q32 + Q33;
					px = static_cast<double>(j) + Q03;
					py = static_cast<double>(i) + Q13;
					pz = Q23;

					px = px/pw;
					py = py/pw;
					pz = pz/pw;
				}
				if( !b_discarded )
				{
					if( pz > _bbox_max[2] )
						_bbox_max[2] = pz;
					if( pz < _bbox_min[2] )
						_bbox_min[2] = pz;
					if( px > _bbox_max[0] )
						_bbox_max[0] = px;
					if( px < _bbox_min[0] )
						_bbox_min[0] = px;
					if( py > _bbox_max[1] )
						_bbox_max[1] = py;
					if( py < _bbox_min[1] )
						_bbox_min[1] = py;
				}

			//	if( ((REAL)pz < _threshold_min_ui) || ((REAL)pz > _threshold_max_ui) )
				if( ( pz < _threshold_min_ui[2] ) || ( pz > _threshold_max_ui[2] ) ||
					( py < _threshold_min_ui[1] ) || ( py > _threshold_max_ui[1] ) ||
					( px < _threshold_min_ui[0] ) || ( px > _threshold_max_ui[0] ) )
				{
					if( _b_threshold_ui )
					{
						px = .0;
						py = .0;
						pz = REAL_BIG_VALUE;
						b_discarded = TRUE;
					}
				}
				else
				{
					if( px > _bbox_thresh_max[0] )
						_bbox_thresh_max[0] = px;
					if( px < _bbox_thresh_min[0] )
						_bbox_thresh_min[0] = px;
					if( py > _bbox_thresh_max[1] )
						_bbox_thresh_max[1] = py;
					if( py < _bbox_thresh_min[1] )
						_bbox_thresh_min[1] = py;
					if( pz > _bbox_thresh_max[2] )
						_bbox_thresh_max[2] = pz;
					if( pz < _bbox_thresh_min[2] )
						_bbox_thresh_min[2] = pz;
				}

				dptr[j*3]	= (float)px;
				dptr[j*3+1] = (float)py;
				dptr[j*3+2] = (float)pz;

				if( !b_discarded )
					_points_nb++;
			}
		}
	}
	if( _b_normalize_ui )
		do_normalize();

	if( _b_transfo_ui )
		do_transfert();

goto_fail:
	_speed_stereo_points->end( g_speed_master->is_print_bdd() );

	tbuf_add( c_tbuf_master::CH_OPEN_CV, 0., NULL, this );
}

void	c_bdd_img_cv_stereo_point::mat_to_aaa_matrix( CONST cv::Mat& mat, REAL* values, size_t size )
{
	size_t	k = 0;
	for ( INT32 i = 0; i < mat.rows; i++)
	{
		CONST	REAL*	Mi = mat.ptr<float>(i);
		for( INT32 j = 0; j < mat.cols; j++)
		{
			values[k] = Mi[j];
			k++;
			if( k > size )
			{
				ERR_PRINT_STRING( "%s(), trying to access wrong index", __FUNCTION__ );
				break;
			}
		}
	}
}

void	c_bdd_img_cv_stereo_point::aaa_to_mat_matrix( cv::Mat& mat, CONST REAL* values, UINT32 rows, UINT32 cols )
{
	size_t	k = 0;
	mat.create( rows, cols, CV_32F);
	for ( INT32 i = 0; i < mat.rows; i++)
	{
		float*	Mi = mat.ptr<float>(i);
		for( INT32 j = 0; j < mat.cols; j++)
		{
			Mi[j] = values[k];
			k++;
		}
	}
}

FINLINE	INT32	c_bdd_img_cv_stereo_point::get_point_nb()
{
	if( !_points_3d.empty() )
	{
//		return _points_nb;
		return _points_3d.cols * _points_3d.rows;
	}
	return 0;
}

void	c_bdd_img_cv_stereo_point::get_point( REAL* dst, INT32 index )
{
	INT32	ind = index - 1;
	if( ind >= 0 && ind < _points_3d.cols * _points_3d.rows )
	{
		INT32	i = MIN( ind / _points_3d.cols, _points_3d.rows-1 );
		INT32	j = MIN( ind % _points_3d.cols, _points_3d.cols-1 );
		cv::Point3f p = _points_3d.at<cv::Point3f>(i,j);
		set_v3r( dst, p.x, p.y, p.z );
	}
	else
		clear_v3r( dst );
}

