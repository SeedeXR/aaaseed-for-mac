#include "bdd_img_cv_img_proc.h"
#include "draw/bind_img.h"
#include "spy.h"
#include "opencv2/xphoto.hpp"

#include "SuperPixels/Tools.h"

FACTORY_CREATE_PROP_V1( c_bdd_img_cv_img_proc, bdd_img_cv_img_proc, Image OpenCV Image Processing, bdd_img_cv_img_proc, sub_menu="Image"; );

namespace n_bdd_img_cv_img_proc
{
	static	CONST	INT32	BASE_PARAM_NB			= 0 + c_bdd_img::SUPER_PARAM_NB;
	static	CONST	INT32	SEEDS_PARAM_NB			= 12;
	static	CONST	INT32	SEEDS_REVISED_PARAM_NB	= 12;
	static	CONST	INT32	EDGE_PARAM_NB			= 3;
	static	CONST	INT32	DTFILTER_PARAM_NB		= 9;
	static	CONST	INT32	GUIDED_PARAM_NB			= 7;
	//static	CONST	INT32	WB_PARAM_NB				= 3;
	//static	CONST	INT32	DCT_DENOISE_PARAM_NB	= 4;
	static	CONST	INT32	GROUP_PARAM_NB			= 5;

	static	CONST	INT32	PARAM_NB				=	BASE_PARAM_NB
													+	SEEDS_PARAM_NB
													+	SEEDS_REVISED_PARAM_NB
													+	EDGE_PARAM_NB
													+	DTFILTER_PARAM_NB
													+	GUIDED_PARAM_NB
													//+	WB_PARAM_NB
													//+	DCT_DENOISE_PARAM_NB
													+	GROUP_PARAM_NB;


	static	ST_PARAM	param[PARAM_NB] =
	{
		BDD_IMG_BASE_PARAMS
		ST_PARAM_GROUP_CLOSED( Superpixels , SEEDS_PARAM_NB )
			ST_PARAM_BOOL_OFF(		superpixel_active							)
			ST_PARAM_IMG_DST2(		superpixel_img_dst							)
			ST_PARAM_INT32_POS(		superpixel_nb,				800, 400		)
			ST_PARAM_INT32_LOCKED(	superpixel_count							)
			ST_PARAM_INT32(			superpixel_iterations,		8, 4,	1, 1000	)
			ST_PARAM_INT32_POS(		superpixel_block_levels,	8, 4			)
			ST_PARAM_INT32(			superpixel_prior,			0, 2,	0, 5	)
			ST_PARAM_INT32_POS(		superpixel_histogram_bin,	7, 5			)
			ST_PARAM_BOOL_OFF(		superpixel_double_step						)
			ST_PARAM_BOOL_OFF(		superpixel_lav								)
			ST_PARAM_BOOL_OFF(		superpixel_mask								)
			ST_PARAM_BOOL_OFF(		superpixel_label							)
		ST_PARAM_GROUP_CLOSED( SuperpixelsRevised , SEEDS_PARAM_NB )
			ST_PARAM_BOOL_OFF(		seeds_revised_active							)
			ST_PARAM_IMG_DST2(		seeds_revised_img_dst							)
			ST_PARAM_INT32_POS(		seeds_revised_nb,				800, 400		)
			ST_PARAM_INT32_LOCKED(	seeds_revised_count								)
			ST_PARAM_INT32(			seeds_revised_iterations,		8, 4,	1, 1000	)
			ST_PARAM_INT32_POS(		seeds_revised_neighborhood,		1, 2			)
			ST_PARAM_REAL_POS(		seeds_revised_confidence,		0.1, .2			)
			ST_PARAM_INT32_POS(		seeds_revised_histogram_bin,	7, 5			)
			ST_PARAM_REAL_POS(		seeds_revised_spatial_weigth,	.25, .1			)
			ST_PARAM_BOOL_OFF(		seeds_revised_mean								)
			ST_PARAM_BOOL_OFF(		seeds_revised_mask								)
			ST_PARAM_BOOL_OFF(		seeds_revised_label								)
		ST_PARAM_GROUP_CLOSED( StructuredEdge, EDGE_PARAM_NB )
			ST_PARAM_BOOL_OFF(		edge_active				)
			ST_PARAM_IMG_DST2(		edge_img_dst			)
			ST_PARAM_STR(			edge_model_filename		)
		ST_PARAM_GROUP_CLOSED( DomainTransformFilter, DTFILTER_PARAM_NB )
			ST_PARAM_BOOL_OFF(		dt_filter_active						)
			ST_PARAM_IMG_DST2(		dt_filter_img_dst						)
			ST_PARAM_INT32_POS(		dt_filter_sigma_spatial,	5, 10		)
			ST_PARAM_INT32_POS(		dt_filter_sigma_color,		10, 25		)
			ST_PARAM_INT32_POS(		dt_filter_detail_constrat,	50, 100		)
			ST_PARAM_INT32_POS(		dt_filter_edges_gamma,		50, 100		)
			ST_PARAM_INT32_POS(		dt_filter_details_level,	50, 200		)
			ST_PARAM_BOOL_OFF(		dt_filter_detail						)
			ST_PARAM_BOOL_OFF(		dt_filter_stylize						)
		ST_PARAM_GROUP_CLOSED( GuidedFilter, GUIDED_PARAM_NB )
			ST_PARAM_BOOL_OFF(			guided_filter_active							)
			ST_PARAM_IMG_DST2(			guided_filter_img_dst							)
			ST_PARAM_INT32_POS_ZERO(	guided_filter_mask_bind							)
			ST_PARAM_INT32_POS(			guided_filter_radius,	4, 8					)
			ST_PARAM_REAL(				guided_filter_eps,		0.0, .2,	0., 255.	)
			ST_PARAM_BOOL_OFF(			guided_filter_smooth							)
			ST_PARAM_BOOL_OFF(			guided_filter_enhance							)
		//ST_PARAM_GROUP_CLOSED( WhiteBalance, WB_PARAM_NB )
		//	ST_PARAM_BOOL_OFF(			wb_active				)
		//	ST_PARAM_IMG_DST2(			wb_img_dst				)
		//	ST_PARAM_INT32_POS(			wb_algorithm,	4, 8	)
		//ST_PARAM_GROUP_CLOSED( Dct Denoise, DCT_DENOISE_PARAM_NB )
		//	ST_PARAM_BOOL_OFF(			dct_denoise_active				)
		//	ST_PARAM_IMG_DST2(			dct_denoise_img_dst				)
		//	ST_PARAM_INT32_POS(			dct_denoise_sigma,		4, 8	)
		//	ST_PARAM_INT32_POS(			dct_denoise_block_sze,	16, 8	)
	};
}

void	c_bdd_img_cv_img_proc::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	++h;
		param_set_pt( h, _b_sp_enable_ui			);
		param_set_pt( h, _sp_dst_index_ui			);
		param_set_pt( h, _sp_nb_ui					);
		param_set_pt( h, _sp_count					);
		param_set_pt( h, _sp_nb_iteration_ui		);
		param_set_pt( h, _sp_num_levels_ui			);
		param_set_pt( h, _sp_prior_ui				);
		param_set_pt( h, _sp_histogram_bins_ui		);
		param_set_pt( h, _b_sp_double_step_ui		);
		param_set_pt( h, _b_sp_use_lav_ui			);
		param_set_pt( h, _b_sp_mask_ui				);
		param_set_pt( h, _b_sp_label_ui				);
	++h;
		param_set_pt( h, _b_spr_enable_ui			);
		param_set_pt( h, _spr_dst_index_ui			);
		param_set_pt( h, _spr_nb_ui					);
		param_set_pt( h, _spr_count					);
		param_set_pt( h, _spr_nb_iteration_ui		);
		param_set_pt( h, _spr_neighborhood_ui		);
		param_set_pt( h, _spr_confidence_ui			);
		param_set_pt( h, _spr_histogram_bins_ui		);
		param_set_pt( h, _spr_spatial_weigth_ui		);
		param_set_pt( h, _b_spr_mean_ui				);
		param_set_pt( h, _b_spr_mask_ui				);
		param_set_pt( h, _b_spr_label_ui			);
		++h;
		param_set_pt( h, _b_edge_enable_ui			);
		param_set_pt( h, _edge_dst_index_ui			);
		param_set_pt( h, _edge_model_filename_ui	);
	++h;
		param_set_pt( h, _b_dt_enable_ui				);
		param_set_pt( h, _dt_dst_index_ui				);
		param_set_pt( h, _dt_filter_sigma_spatial_ui	);
		param_set_pt( h, _dt_filter_sigma_color_ui		);
		param_set_pt( h, _dt_filter_detail_constrat_ui	);
		param_set_pt( h, _dt_filter_edges_gamma_ui		);
		param_set_pt( h, _dt_filter_details_level_ui	);
		param_set_pt( h, _b_dt_detail_ui				);
		param_set_pt( h, _b_dt_stylize_ui				);
	++h;
		param_set_pt( h, _b_guided_enable_ui		);
		param_set_pt( h, _guided_dst_index_ui		);
		param_set_pt( h, _guided_mask_bind_ui		);
		param_set_pt( h, _guided_radius_ui			);
		param_set_pt( h, _guided_eps_ui				);
		param_set_pt( h, _b_guided_smooth_ui		);
		param_set_pt( h, _b_guided_enhance_ui		);
	//++h;
	//	param_set_pt( h, _b_wb_enable_ui			);
	//	param_set_pt( h, _wb_dst_index_ui			);
	//	param_set_pt( h, _wb_algorithm_ui			);
	//++h;
	//	param_set_pt( h, _b_dct_enable_ui			);
	//	param_set_pt( h, _dct_dst_index_ui			);
	//	param_set_pt( h, _dct_sigma_ui				);
	//	param_set_pt( h, _dct_size_ui				);

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bdd_img_cv_img_proc )
	,_sp_count					(	FALSE	)

{
	param_init_with( n_bdd_img_cv_img_proc::param, n_bdd_img_cv_img_proc::PARAM_NB );
}

void	c_bdd_img_cv_img_proc::close()
{
	if( this )
	{
		close_mat();

		_seeds.release();
		_edge_struc.release();
	}
}

void	c_bdd_img_cv_img_proc::close_mat()
{
	_seeds_label.release();
	_seeds_mask.release();
	_seeds_convert.release();
	_seeds_result.release();

	_edge_forest.release();
	_edge_convert.release();
	_edge_out.release();

	_dt_filtered.release();
	_dt_filtered_grey.release();
	_gradX.release();
	_gradY.release();
	_gradMagnitude.release();
	_multiplier.release();
	_dt_dst.release();
	_frame8u.release();
	_frameLab.release();
	_frameLab2.release();
	_frameLabCn[3].release();
	_layer0.release();
	_layer1.release();
	_layer2.release();
	_detailLayer1.release();
	_detailLayer2.release();


}

c_bdd_img_cv_img_proc::~c_bdd_img_cv_img_proc()
{
	close();
}

void	c_bdd_img_cv_img_proc::update()
{
	 cv::setNumThreads(12);
	if( update_mat_src() )
	{
		do_process();
	}
}

void	c_bdd_img_cv_img_proc::draw()
{
}

void	c_bdd_img_cv_img_proc::do_process()
{
	TBUF_ADD( tbuf::CH_OPEN_CV, 1., "bdd_img_cv_img_proc", this );
	if( _b_sp_enable_ui && do_superpixels() )
	{
		update_dst_index( *_src_cur, _sp_dst_index_ui, "Superpixels" );
	}
	if ( _b_spr_enable_ui && do_seeds_revised() )
	{
		update_dst_index( *_src_cur, _sp_dst_index_ui, "Superpixels Revised" );
	}
	if ( _b_edge_enable_ui && do_edge_structured() )
	{
		update_dst_index( *_src_cur, _edge_dst_index_ui, "Structured Edge" );
	}
	if ( _b_dt_enable_ui && do_dt_filter() )
	{
		update_dst_index( *_src_cur, _dt_dst_index_ui, "DT Filter" );
	}
	if ( _b_guided_enable_ui && do_guided_filter() )
	{
		update_dst_index( *_src_cur, _guided_dst_index_ui, "Guided Filter" );
	}
	//if( _b_wb_enable_ui && do_white_balance() )
	//{
	//	update_dst_index( *_src_cur, _wb_dst_index_ui, "Automatic White Balance" );
	//}
	//if ( _b_dct_enable_ui && do_dct_denoise() )
	//{
	//	update_dst_index( *_src_cur, _dct_dst_index_ui, "DCT Denoise" );
	//}
	TBUF_ADD( tbuf::CH_OPEN_CV, 0., NULL, this );

}

BOOL	c_bdd_img_cv_img_proc::do_superpixels()
{
	if( _sp_nb != _sp_nb_ui || _sp_num_levels != _sp_num_levels_ui || _sp_prior != _sp_prior_ui || _sp_histogram_bins != _sp_histogram_bins_ui || _b_sp_double_step != _b_sp_double_step_ui )
	{
		_sp_nb				= _sp_nb_ui;
		_sp_num_levels		= _sp_num_levels_ui;
		_sp_prior			= _sp_prior_ui;
		_sp_histogram_bins	= _sp_histogram_bins_ui;
		_b_sp_double_step	= _b_sp_double_step_ui;
		_seeds.release();
	}
//	BOOL	b_init = FALSE;
	if( _seeds.empty() || _b_size_changed )
	{
		cv::cvtColor( *_src_cur, _seeds_convert, _b_sp_use_lav_ui ? cv::COLOR_BGR2Lab : cv::COLOR_BGR2HSV );
		_seeds = cv::ximgproc::createSuperpixelSEEDS( _seeds_convert.size().width, _seeds_convert.size().height, _seeds_convert.channels(), _sp_nb, _sp_num_levels, _sp_prior, _sp_histogram_bins, _b_sp_double_step ? true : false );
		//b_init = TRUE;


	//	SEEDSRevisedMeanPixels seeds(image, superpixels, numberOfBins, neighborhoodSize, minimumConfidence, spatialWeight);
	}
	if ( !_seeds.empty() )
	{
		cv::cvtColor( *_src_cur, _seeds_convert, _b_sp_use_lav_ui ? cv::COLOR_BGR2Lab : cv::COLOR_BGR2HSV );
		_seeds_result = *_src_cur;
		_seeds->iterate( _seeds_convert, _sp_nb_iteration_ui );
		_sp_count = _seeds->getNumberOfSuperpixels();
		/* get the contours for displaying */
		_seeds->getLabelContourMask( _seeds_mask, false );

	//	BOOL			_b_sp_label_ui;
	//	BOOL			_b_sp_mask_ui;
		
		if( _b_sp_label_ui )
		{
			/* retrieve the segmentation result */
			_seeds->getLabels( _seeds_label );
			//const int num_label_bits = 2;
			//_seeds_label &= (1 << num_label_bits) - 1;
			//_seeds_label *= 1 << (16 - num_label_bits);
			double min, max;
			cv::minMaxLoc( _seeds_label, &min, &max );
			_seeds_label.convertTo( _seeds_label2, CV_8UC1, 255. / _sp_count );
			_src_cur = &_seeds_label2;
		}
		else if( _b_sp_mask_ui )
		{
			_src_cur = &_seeds_mask;
		}
		else
		{
			_seeds_result.setTo( cv::Scalar( 255, 0, 0 ), _seeds_mask );
			_src_cur = &_seeds_result;
		}

		return TRUE;
	}
	return FALSE;
}
BOOL	c_bdd_img_cv_img_proc::do_seeds_revised()
{
	//if( _spr_nb != _spr_nb_ui || _spr_neighborhood != _spr_neighborhood_ui || _sp_histogram_bins != _sp_histogram_bins_ui || _spr_confidence != _spr_confidence_ui || _spr_spatial_weigth != _spr_spatial_weigth_ui )
	//{
	//	_sp_nb					= _sp_nb_ui;
	//	_spr_neighborhood		= _spr_neighborhood_ui;
	//	_spr_confidence			= _spr_confidence_ui;
	//	_sp_histogram_bins		= _sp_histogram_bins_ui;
	//	_spr_spatial_weigth		= _spr_spatial_weigth_ui;
	//	SAFE_DELETE( _seeds_revised );
	//}

	//if( !_seeds_revised || _b_size_changed )
	{
	//	cv::cvtColor( *_src_cur, _seeds_convert, _b_sp_use_lav_ui ? cv::COLOR_BGR2Lab : cv::COLOR_BGR2HSV );
		_src_cur->copyTo( _sr_src );
		SEEDSRevised  seeds_revised( _sr_src, _spr_nb_ui, _spr_histogram_bins_ui, _spr_neighborhood_ui, _spr_confidence_ui );
		seeds_revised.initialize();
		seeds_revised.iterate( _sp_nb_iteration_ui );


		if( _b_spr_mean_ui )
		{
			meanImage = Draw::meanImage( seeds_revised.getLabels(), _sr_src );
			meanImage.copyTo( umeanImage );
			_src_cur = &umeanImage;
		}
		else if ( _b_spr_label_ui )
		{
			labelImage = Draw::labelImage( seeds_revised.getLabels(), _sr_src );
			labelImage.copyTo( ulabelImage );
			_src_cur = &ulabelImage;

		}
		else if ( _b_spr_mask_ui )
		{
			int bgr[] = { 0, 0, 204 };
			contourImage = Draw::contourImage( seeds_revised.getLabels(), _sr_src, bgr );
			contourImage.copyTo( ucontourImage );
			_src_cur = &ucontourImage;

		}

		return TRUE;
	}
	return FALSE;
}
BOOL	c_bdd_img_cv_img_proc::do_edge_structured()
{
	if( _edge_model_filename_ui != _edge_model_filename )
	{
		_edge_model_filename = _edge_model_filename_ui;
		_edge_struc.release();
	}
	//	BOOL	b_init = FALSE;
	if( _edge_struc.empty() && !_edge_model_filename.is_empty() )
	{
		_edge_struc = cv::ximgproc::createStructuredEdgeDetection( _edge_model_filename.get() );
		//b_init = TRUE;
	}
	if( !_edge_struc.empty() )
	{
		if(_src_cur->channels() == 4 )
		{
			cv::cvtColor( *_src_cur, _edge_3, cv::COLOR_BGRA2BGR );
			_edge_3.convertTo( _edge_convert, cv::DataType<float>::type, 1 / 255.0 );
		}
		else if( _src_cur->channels() != 3 )
		{
			ERR_PRINT_STRING( "%s() needs a 3 channels image" );
			return FALSE;
		}
		else
		{
			_src_cur->convertTo( _edge_convert, cv::DataType<float>::type, 1 / 255.0 );
		}
		_edge_forest.create( _edge_convert.size(), _edge_convert.type() );
		_edge_struc->detectEdges( _edge_convert, _edge_forest );
		_edge_forest.convertTo( _edge_out, CV_8UC1, 255. );
		_src_cur = &_edge_out;
		return TRUE;
	}
	return FALSE;
}

BOOL	c_bdd_img_cv_img_proc::do_dt_filter()
{

	if( _b_dt_stylize_ui )
	{
		//blur frame
		//Mat filtered;
		cv::ximgproc::dtFilter( *_src_cur, *_src_cur, _dt_filtered, _dt_filter_sigma_spatial_ui, _dt_filter_sigma_color_ui, cv::ximgproc::DTF_NC );

		//compute grayscale blurred frame
		//Mat filteredGray;
		cv::cvtColor( _dt_filtered, _dt_filtered_grey, cv::COLOR_BGR2GRAY );

		//find gradients of blurred image
		//Mat gradX, gradY;
		cv::Sobel( _dt_filtered_grey, _gradX, CV_32F, 1, 0, 3, 1.0 / 255 );
		cv::Sobel( _dt_filtered_grey, _gradY, CV_32F, 0, 1, 3, 1.0 / 255 );

		//compute magnitude of gradient and fit it accordingly the gamma parameter
		//Mat gradMagnitude;
		cv::magnitude( _gradX, _gradY, _gradMagnitude );
		cv::pow( _gradMagnitude, _dt_filter_edges_gamma_ui / 100.0, _gradMagnitude );

		//multiply a blurred frame to the value inversely proportional to the magnitude
		_multiplier = 1.0 / (1.0 + _gradMagnitude);
		cv::cvtColor( _multiplier, _multiplier, cv::COLOR_GRAY2BGR );
		cv::multiply( _dt_filtered, _multiplier, _dt_dst, 1, _src_cur->type() );
		_src_cur = &_dt_dst;
	
	}
	else if ( _b_dt_detail_ui )
	{
		//Mat frame;
		_src_cur->convertTo( _frame8u, CV_32F, 1.0 / 255 );

		//Decompose image to 3 Lab channels
		//Mat frameLab, frameLabCn[3];
		cv::cvtColor( _frame8u, _frameLab, cv::COLOR_BGR2Lab );
		cv::split( _frameLab, _frameLabCn );

		//Generate progressively smoother versions of the lightness channel
		_layer0 = _frameLabCn[0]; //first channel is original lightness
		cv::ximgproc::dtFilter( _layer0, _layer0, _layer1, _dt_filter_sigma_spatial_ui, _dt_filter_sigma_color_ui, cv::ximgproc::DTF_IC );
		cv::ximgproc::dtFilter( _layer1, _layer1, _layer2, 2 * _dt_filter_sigma_spatial_ui, _dt_filter_sigma_color_ui, cv::ximgproc::DTF_IC );

		//Compute detail layers
		_detailLayer1 = _layer0 - _layer1;
		_detailLayer2 = _layer1 - _layer2;

		double cBase		= _dt_filter_detail_constrat_ui / 100.0;
		double cDetails1	= _dt_filter_details_level_ui / 100.0;
		double cDetails2	= 2.0 - _dt_filter_details_level_ui / 100.0;

		//Generate lightness
		double meanLigtness = cv::mean( _frameLabCn[0] )[0];
		_frameLabCn[0] = cBase*(_layer2 - meanLigtness) + meanLigtness; //fit contrast of base (most blurred) layer
		_frameLabCn[0] += cDetails1*_detailLayer1; //add weighted sum of detail layers to new lightness
		_frameLabCn[0] += cDetails2*_detailLayer2; //

		//Update new lightness
		cv::merge( _frameLabCn, 3, _frameLab );
		cv::cvtColor( _frameLab, _frameLab2, cv::COLOR_Lab2BGR );
		_frameLab2.convertTo( _dt_dst, CV_8U, 255 );
		_src_cur = &_dt_dst;	
	}
	else
	{
		cv::ximgproc::dtFilter( *_src_cur, *_src_cur, _dt_filtered, _dt_filter_sigma_spatial_ui, _dt_filter_sigma_color_ui, cv::ximgproc::DTF_RF );
		_src_cur = &_dt_filtered;
	}

	return TRUE;
}
BOOL	c_bdd_img_cv_img_proc::do_guided_filter()
{

	if ( _b_guided_smooth_ui )
	{
		//Smoothing
		double eps = _guided_eps_ui * _guided_eps_ui; // try eps=0.1^2, 0.2^2, 0.4^2
		eps *= 255. * 255.;				// Because the intensity range of our images is [0, 255]
		cv::ximgproc::guidedFilter( *_src_cur, *_src_cur, _guided_dst, _guided_radius_ui, eps );
		_src_cur = &_guided_dst;

	}
	else if ( _b_guided_enhance_ui )
	{
		_src_cur->convertTo( _guided_convert, CV_32F, 1.0 / 255.0 );
		double eps = _guided_eps_ui * _guided_eps_ui;
		cv::ximgproc::guidedFilter( _guided_convert, _guided_convert, _guided_enhanced2, _guided_radius_ui, eps );
		_guided_enhanced = (_guided_convert - _guided_enhanced2) * 5 + _guided_enhanced2;
		_guided_enhanced.convertTo( _guided_dst, CV_8U );
		_src_cur = &_guided_dst;
	}
	else
	{
		//// do it only once ? or do it in update, we need the mask
		//c_img*	img = bind_img::get_ready( _guided_mask_bind_ui );
		//if ( IS_NULL( img ) )
		//{
		//	err_print( "%s() : no image source at bind %d", __FUNCTION__, _guided_mask_bind_ui );
		//	return FALSE;
		//}
		//if ( !img->check_data_valid( __FUNCTION__ ) )
		//{
		//	err_print( "%s() : image source has no valid data at bind %d", __FUNCTION__, _guided_mask_bind_ui );
		//	return FALSE;
		//}
		//cv::Mat	tmp( img->get_size_x(), img->get_size_y(), get_cv_type( img->get_pixel_type() ), (UINT8 *)img->get_data_uint8(), img->get_pitch() );
		//_guided_mask = tmp.getUMat( cv::ACCESS_RW );
		double	eps = _guided_eps_ui * _guided_eps_ui;
		eps *= 255. * 255.;			// Because the intensity range of our images is [0, 255]
//		cv::ximgproc::guidedFilter( _guided_mask, *_src_cur, _guided_dst, _guided_radius_ui, eps );
		cv::ximgproc::guidedFilter( *_src_cur, *_src_cur, _guided_dst, _guided_radius_ui, eps );
		_src_cur = &_guided_dst;
	}

	return TRUE;
}

//BOOL	c_bdd_img_cv_img_proc::do_dct_denoise()
//{
//
//	// only 1 or 3 channel Image
//	if( _src_cur->channels() == 4 )
//	{
//		cv::cvtColor( *_src_cur, _dct_src, cv::COLOR_BGRA2RGB );
//	}
//	else
//	{
//		_src_cur->copyTo( _dct_src );
//	}
//	_dct_dst.create( _dct_src.size(), _dct_src.type() );
//	cv::xphoto::dctDenoising( _dct_src, _dct_dst, _dct_sigma_ui, _dct_size_ui );
//	_dct_dst.copyTo( _dct_out );
//	_src_cur = &_dct_out;
//	return TRUE;
//}
//
//BOOL	c_bdd_img_cv_img_proc::do_white_balance()
//{
//	_src_cur->copyTo( _wb_src );
//	//cv::Mat res( src.size(), src.type() );
//	_wb_dst.create( _wb_src.size(), _wb_src.type() );
//	cv::xphoto::balanceWhite( _wb_src, _wb_dst, cv::xphoto::WHITE_BALANCE_SIMPLE);
//	_wb_dst.copyTo( _wb_out );
//	_src_cur = &_wb_out;
//	return TRUE;
//}


