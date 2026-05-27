#include "bdd_img_cv_background.h"
#include "image/bind_img_2d.h"
#include "spy.h"


C_PCHAR_C	c_bdd_img_cv_back_segm::back_seg_type_str[BACK_SEG_MAX] =
{
	"MOG",
	"MOG2",
	"GMG",
	"FGDStatModel GPU",
};

FACTORY_CREATE_PROP_V1( c_bdd_img_cv_back_segm, bdd_img_cv_back_segm, Image OpenCV Background Segmentation, bdd_img_cv_back_segm, sub_menu="Image"; );

namespace n_bdd_img_cv_back_segm
{
	CONSTEXPR INT32 BASE_PARAM_NB		= 6 + c_bdd_img::SUPER_PARAM_NB;
	CONSTEXPR INT32 MOG_PARAM_NB		= 7;
	CONSTEXPR INT32 MOG2_PARAM_NB		= 5;
	CONSTEXPR INT32 GMG_PARAM_NB		= 9;
	CONSTEXPR INT32 FGD_STAT_PARAM_NB	= 1;
	CONSTEXPR INT32 GROUP_PARAM_NB		= 4;
	CONSTEXPR INT32 PARAM_NB	=	BASE_PARAM_NB
								+	MOG_PARAM_NB
								+	MOG2_PARAM_NB
								+	GMG_PARAM_NB
								+	FGD_STAT_PARAM_NB
								+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB] =
	{
		BDD_IMG_BASE_PARAMS

		PARAM_DEF_SYMBO_PSTR_ZERO(	algorithm,			c_bdd_img_cv_back_segm::back_seg_type_str )
		PARAM_DEF_BOOL_OFF(			background_reset	)
		PARAM_DEF_BOOL_ON(			background_enable	)
		PARAM_DEF_IMG_DST2(			background_img_dst	)
		PARAM_DEF_BOOL_ON(			foreground_enable	)
		PARAM_DEF_IMG_DST2(			foreground_img_dst	)
		PARAM_DEF_GROUP_CLOSED( MOG , MOG_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		mog_gpu													)
			PARAM_DEF_INT32(		mog_gaussian_nb,				-1, 2,		-1, 21		)
			PARAM_DEF_REAL_POS(		mog_learning_rate,				0, 1					)
			PARAM_DEF_INT32(		mog_history,					19, 10,		0, 1024		)
			PARAM_DEF_REAL_INF(		mog_var_threshold,				.0, 1.0					)
			PARAM_DEF_REAL_INF(		mog_background_ratio,			.0, 1.0					)
			PARAM_DEF_REAL_INF(		mog_noise_sigma,				.0, 1.0					)
		PARAM_DEF_GROUP_CLOSED( MOG2 , MOG2_PARAM_NB )
			PARAM_DEF_BOOL_OFF(		mog2_gpu												)
			PARAM_DEF_INT32(		mog2_gaussian_nb,				-1, 2,		-1, 21		)
			PARAM_DEF_REAL_POS(		mog2_learning_rate,				0, 1					)
			PARAM_DEF_INT32(		mog2_history,					19, 10,		0, 1024		)
			PARAM_DEF_REAL_INF(		mog2_var_threshold,				.0, 1.0					)

		PARAM_DEF_GROUP_CLOSED( GMG, GMG_PARAM_NB )
			PARAM_DEF_BOOL_OFF(			gmg_gpu													)
			PARAM_DEF_INT32(			gmg_max_feature,				64, 80,		0, 256		)
			PARAM_DEF_REAL_ONE_ZERO(	gmg_learning_rate										)
			PARAM_DEF_INT32(			gmg_num_initialization_frames,	0, 10,		0, 1024		)
			PARAM_DEF_INT32(			gmg_quantization_levels,		0, 10,		0, 1024		)
			PARAM_DEF_REAL_INF(			gmg_background_prior,			.0, 1.0					)
			PARAM_DEF_REAL_INF(			gmg_decision_threshold,			.0, 1.0					)
			PARAM_DEF_INT32(			gmg_smoothing_radius,			0, 10,		0, 1024		)
			PARAM_DEF_BOOL_OFF(			gmg_update_background									)

		PARAM_DEF_GROUP_CLOSED( FGDStat, FGD_STAT_PARAM_NB )
			PARAM_DEF_INT32(			fgf_out_channel,				4, 3,	3, 4			)
		//	PARAM_DEF_IMG_DST2(			fgf_fore_regions_img_dst		)

		//	PARAM_DEF_INT32(			fgd_Lc,				32, 64,		0, 1024		)
		//	PARAM_DEF_INT32(			fgd_N1c,				32, 64,		0, 1024		)
		//	PARAM_DEF_INT32(			fgd_N2c,				32, 64,		0, 1024		)
		//	PARAM_DEF_INT32(			fgd_Lcc,				32, 64,		0, 1024		)
		//	PARAM_DEF_INT32(			fgd_Lc,				32, 64,		0, 1024		)
		//	PARAM_DEF_INT32(			fgd_Lc,				32, 64,		0, 1024		)
		//	PARAM_DEF_REAL_ONE_ZERO(	fgd_,				.0, 1.0					)
		//	PARAM_DEF_INT32(			fgd_,		0, 10,		0, 1024		)
		//	PARAM_DEF_INT32(			fgd_,		0, 10,		0, 1024		)
		//	PARAM_DEF_REAL_INF(			fgd_,			.0, 1.0					)
		//	PARAM_DEF_REAL_INF(			fgd_,			.0, 1.0					)
		//	PARAM_DEF_INT32(			fgd_,			0, 10,		0, 1024		)
		//	PARAM_DEF_BOOL_OFF(			fgd_,			0, 10,		0, 1024		)
	};
}


void	c_bdd_img_cv_back_segm::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start_src_resize( false, false );

	param_set_pt( h, _s_stereo_algo_ui			);
	param_set_pt( h, _b_background_reset_ui		);
	param_set_pt( h, _b_back_enable_ui			);
	param_set_pt( h, _back_img_index_ui			);
	param_set_pt( h, _b_fore_enable_ui			);
	param_set_pt( h, _fore_img_index_ui			);
	++h;
		param_set_pt( h, _mog_b_gpu_ui				);
		param_set_pt( h, _mog_gaussian_nb_ui		);
		param_set_pt( h, _mog_learning_rate_ui		);
		param_set_pt( h, _mog_history_ui			);
		param_set_pt( h, _mog_var_threshold_ui		);
		param_set_pt( h, _mog_background_ratio_ui	);
		param_set_pt( h, _mog_noiseSigma_ui			);
	++h;
		param_set_pt( h, _mog2_b_gpu_ui				);
		param_set_pt( h, _mog2_gaussian_nb_ui		);
		param_set_pt( h, _mog2_learning_rate_ui		);
		param_set_pt( h, _mog2_history_ui			);
		param_set_pt( h, _mog2_var_threshold_ui		);
	++h;
		param_set_pt( h, _gmg_b_gpu_ui						);
		param_set_pt( h, _gmg_max_feature_ui				);
		param_set_pt( h, _gmg_learning_rate_ui				);
		param_set_pt( h, _gmg_num_initialization_frames_ui	);
		param_set_pt( h, _gmg_quantization_levels_ui		);
		param_set_pt( h, _gmg_background_prior_ui			);
		param_set_pt( h, _gmg_decision_threshold_ui			);
		param_set_pt( h, _gmg_smoothing_radius_ui			);
		param_set_pt( h, _gmg_b_update_background_model_ui	);	//unused
	++h;
		param_set_pt( h, _fgd_out_channel_nb_ui		);
	//	param_set_pt( h, _fore_region_img_index_ui			);

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bdd_img_cv_back_segm )
	//,_mog_gpu					( nullptr )
	//,_mog_cpu					( nullptr )
	//,_mog2_gpu				( nullptr )
	//,_mog2_cpu				( nullptr )
	//,_fgd_stat_gpu			( nullptr )
	,_s_stereo_algo				( 0 )
	,_b_gpu						( false )
	,_mog_gaussian_nb			( -1 )		// default is -1
	,_mog_history				( 0 )
	,_mog_background_ratio		( .0f )
	,_mog2_gaussian_nb			( -1 )
	,_mog2_history				( 0 )
	,_mog2_var_threshold		( .0f )
	,_fgd_out_channel_nb		( 3 )
{
	param_init_with( n_bdd_img_cv_back_segm::param, n_bdd_img_cv_back_segm::PARAM_NB );
}

void	c_bdd_img_cv_back_segm::close()
{
	IF_THIS_NULL_RETURN();

	close_mat();

#if AAA_LIB_USE_OPENCV_CUDA()
	// MOG
	//if( _mog_gpu )
	//{
	//	_mog_gpu->release();
	//}
	//SAFE_DELETE( _mog_gpu );
	_mog_gpu.release();
//	_mog_cpu->release();
	//SAFE_DELETE( _mog_cpu );

	// MOG2
	//if( _mog2_gpu )
	//{
	//	_mog2_gpu->release();
	//}
	_mog2_gpu.release();
	//SAFE_DELETE( _mog2_gpu );
#endif
	//	_mog2_cpu->release();
	//SAFE_DELETE( _mog2_cpu );
	_mog2_cpu.release();

	//// FGD
	//if( _fgd_stat_gpu )
	//{
	//	_fgd_stat_gpu->release();
	//}
	//SAFE_DELETE( _fgd_stat_gpu );

	//// GMG
	//_gmg_gpu.release();
	//_gmg_cpu.release();

}

void	c_bdd_img_cv_back_segm::close_mat()
{
#if AAA_LIB_USE_OPENCV_CUDA()
	_gpumat_back.release();
	_gpumat_fore_mask.release();
#endif

	_mat_fore.release();
	_mat_back.release();
}

c_bdd_img_cv_back_segm::~c_bdd_img_cv_back_segm()
{
	close();
}

void	c_bdd_img_cv_back_segm::update()
{
	switch(  _s_stereo_algo_ui )
	{
	case BACK_SEG_MOG :			_b_gpu = true;				break;
	case BACK_SEG_MOG2 :		_b_gpu = _mog2_b_gpu_ui;	break;
	//case BACK_SEG_GMG :			_b_gpu = _gmg_b_gpu_ui;		break;
	//case BACK_SEG_FGDSTAT :		_b_gpu = true;				break;
	default:					_b_gpu = false;				break;
	}
	bool b_change_it;
	if( update_mat_src(b_change_it) )
	{
		do_process();
		_b_background_reset_ui = false;
	}
}

void	c_bdd_img_cv_back_segm::draw()
{
}

void	c_bdd_img_cv_back_segm::do_process()
{
	//UINT32	channel = 1;
	aaa::PIXEL_FORMAT	type = aaa::PIXEL_FORMAT::R_8;
#if AAA_LIB_USE_OPENCV_CUDA()
	if( _b_gpu )
	{
		if( _s_stereo_algo_ui == BACK_SEG_FGDSTAT )
		{
			if( _src_cur_gpu->channels() == 1 )
			{
				ERR_PRINT_STRING( "FGDStatModel only support 3 or 4 channel image" );
				return;
			}
			type = aaa::c_pixel_format::make_format_from_channel_type( _src_cur_gpu->channels() );
		}
		else if (_s_stereo_algo_ui == BACK_SEG_MOG || _s_stereo_algo_ui == BACK_SEG_MOG2 )
		{
			type = aaa::PIXEL_FORMAT::R_8;		// The output foreground mask as an 8-bit binary image.
		}
		else
		{
			type = aaa::c_pixel_format::make_format_from_channel_type( _src_cur_gpu->channels() );
		}
	}
	else
#endif
	{
		if(_s_stereo_algo_ui == BACK_SEG_MOG || _s_stereo_algo_ui == BACK_SEG_MOG2 )
		{
			if( _src_cur->channels() == 4 )
			{
				cv::cvtColor( *_src_cur, _mat_mog2_3cn, cv::COLOR_BGRA2BGR );
				_src_cur = &_mat_mog2_3cn;
				type = aaa::PIXEL_FORMAT::RGB_8;
			}
			else
			{
				type = aaa::PIXEL_FORMAT::R_8;		// The output foreground mask as an 8-bit binary image.
			}
		}
		//else if( _s_stereo_algo_ui == BACK_SEG_MOG2 )
		//{
		//	type = PIXEL_TYPE::RGB;		// The output foreground mask as an 8-bit binary image.
		//	
		//	cv::cvtColor( _src_cur, _mat_mog2_3cn, cv::COLOR_BGRA2BGR );
		//	_src_cur = _mat_mog2_3cn;
		//}
		else
		{
			type = aaa::c_pixel_format::make_format_from_channel_type( _src_cur->channels() );
		}
	}
	bool b_ok = true;
	//b_ok = build_dst_index( _mat_dst, _dst_img_index_ui, *_src_cur, "Foreground Mask", aaa::PIXEL_TYPE::GREY );
	//if( _b_back_enable_ui )
	//{
	//	b_ok &= build_dst_index( _mat_dst_back, _back_img_index_ui, *_src_cur, "Background", type );
	//}
	//if( _b_fore_enable_ui )
	//{
	//	b_ok &= build_dst_index( _fore_dst_back, _fore_img_index_ui, *_src_cur, "Foreground", aaa::PIXEL_TYPE::RGBA );
	//}
	if( b_ok )
	{
		do_background_segmentation( _mat_dst, _mat_dst_back );
	}
	else
	{
		ERR_PRINT_STRING( "Error building Foreground" );
		return;
	}
}

#if AAA_LIB_USE_OPENCV_CUDA()
void	c_bdd_img_cv_back_segm::do_mog( cv::cuda::GpuMat& src_a, cv::cuda::GpuMat& dst, cv::cuda::GpuMat& back )
{
	//if( _mog_gaussian_nb != _mog_gaussian_nb_ui )
	//{
	//	_mog_gaussian_nb		= _mog_gaussian_nb_ui;
	//	if( _mog_gpu )
	//	{
	//		_mog_gpu->release();
	//	}
	//	SAFE_DELETE( _mog_gpu );
	//}
	bool b_init = is_size_changed();
	if (_mog_gpu.empty() )
	{
		_mog_gpu = cv::cuda::createBackgroundSubtractorMOG();
	//	_mog_gpu = new cv::cuda::MOG_GPU( _mog_gaussian_nb );
		b_init = true;
	}
	if ( !_mog_gpu.empty() )
	{
		if( b_init || _b_background_reset_ui )
		{
			//_mog_gpu->initialize( src_a.size(), 0.01f );
			_mog_gpu->clear();
		}

		//virtual int getNMixtures() const = 0;
		//virtual void setNMixtures( int nmix ) = 0;

		//virtual double getBackgroundRatio() const = 0;
		//virtual void setBackgroundRatio( double backgroundRatio ) = 0;

		_mog_gpu->setNMixtures(			_mog_gaussian_nb_ui );
		_mog_gpu->setBackgroundRatio(	 _mog_background_ratio_ui );
		_mog_gpu->setHistory(			_mog_history_ui );
		_mog_gpu->setNoiseSigma(		_mog_noiseSigma_ui );
		_mog_gpu->setBackgroundRatio(	_mog_var_threshold_ui );

		_mog_gpu->apply( src_a, dst, _mog_learning_rate_ui );
		_mog_gpu->getBackgroundImage( back );
	}
}

//void	c_bdd_img_cv_back_segm::do_mog_cpu( cv::Mat& src_a, cv::Mat& dst, cv::Mat& back  )
//{
//	if( _mog_history_ui != _mog_history || _mog_gaussian_nb != _mog_gaussian_nb_ui || _mog_background_ratio_ui != _mog_background_ratio || _mog_noiseSigma_ui != _mog_noiseSigma )
//	{
//		_mog_history			= _mog_history_ui;
//		_mog_gaussian_nb		= _mog_gaussian_nb_ui;
//		_mog_background_ratio	= _mog_background_ratio_ui;
//		_mog_noiseSigma			= _mog_noiseSigma_ui;
//		SAFE_DELETE( _mog_cpu );
//	}
//	bool	b_init = _b_size_changed;
//	if( !_mog_cpu )
//	{
//		_mog_cpu = new cv::BackgroundSubtractorMOG( _mog_history, _mog_gaussian_nb, _mog_background_ratio, _mog_noiseSigma );
//		b_init = true;
//	}
//	if( _mog_cpu )
//	{
//		if( b_init || _b_background_reset_ui )
//		{
//			_mog_cpu->initialize( src_a.size(), 0.01f );
//		}
//	//	_mog_cpu->backgroundRatio	= _mog_backgroundRatio_ui;
//	//	_mog_cpu->history			= _mog_history_ui;
//	//	_mog_cpu->noiseSigma		= _mog_noiseSigma_ui;
//	//	_mog_cpu->varThreshold		= _mog_varThreshold_ui;
//
//		_mog_cpu->operator()( src_a, dst, _mog_learning_rate_ui );
//		_mog_cpu->getBackgroundImage( back );
//	}
//}

void	c_bdd_img_cv_back_segm::do_mog2( cv::cuda::GpuMat& src_a, cv::cuda::GpuMat& dst, cv::cuda::GpuMat& back )
{

	//if( _mog2_gaussian_nb != _mog2_gaussian_nb_ui )
	//{
	//	_mog2_gaussian_nb		= _mog2_gaussian_nb_ui;
	//	if( _mog2_gpu )
	//	{
	//		_mog2_gpu->release();
	//	}
	//	SAFE_DELETE( _mog2_gpu );
	//}
	bool b_init = is_size_changed();
	if( _mog2_gpu.empty() )
	{
		_mog2_gpu = cv::cuda::createBackgroundSubtractorMOG2();
		//_mog2_gpu = new cv::cuda::MOG2_GPU( _mog2_gaussian_nb );
		b_init = true;
	}
	if( !_mog2_gpu.empty() )
	{
		if( b_init || _b_background_reset_ui )
		{
			//_mog2_gpu->initialize( src_a.size(), 0.01f );
			_mog2_gpu->clear();
		}
		//_mog2_gpu->backgroundRatio	= _mog_backgroundRatio_ui;
		//_mog2_gpu->history			= _mog2_history_ui;
	//	_mog2_gpu->noiseSigma		= _mog_noiseSigma_ui;
		///_mog2_gpu->varThreshold		= _mog2_var_threshold_ui;
		_mog2_gpu->setNMixtures( _mog2_gaussian_nb_ui );
		//_mog2_gpu->setBackgroundRatio( _mog_background_ratio_ui );
		_mog2_gpu->setHistory( _mog2_history_ui );
	//	_mog2_gpu->setNoiseSigma( _mog_noiseSigma_ui );
		_mog2_gpu->setBackgroundRatio( _mog2_var_threshold_ui );

		_mog2_gpu->apply( src_a, dst, _mog_learning_rate_ui );
		_mog2_gpu->getBackgroundImage( back );
	}
}
#endif

void	c_bdd_img_cv_back_segm::do_mog2_cpu( cv::UMat& src_a, cv::UMat& dst, cv::UMat& back )
{
	if( _mog2_history != _mog2_history_ui || _mog2_var_threshold != _mog2_var_threshold_ui )
	{
		_mog2_history			= _mog2_history_ui;
		_mog2_var_threshold		= _mog2_var_threshold_ui;
	//	_mog2_var_Threshold		= _mog2_history_ui;
//		_mog2_cpu = nullptr;
		_mog2_cpu.release();
		//_mog2_cpu = nullptr;
		//SAFE_RELEASE( _mog2_cpu );
		//SAFE_DELETE( _mog2_cpu );
		//_mog2_cpu.empty()
	}
	bool b_init = is_size_changed();
	if( _mog2_cpu.empty() )
	{
		_mog2_cpu = cv::createBackgroundSubtractorMOG2( _mog2_history, _mog2_var_threshold );
		//_mog2_cpu = new cv::BackgroundSubtractorMOG2( _mog2_history, _mog2_var_threshold );
		b_init = true;
	}
	if( _mog2_cpu && src_a.channels() == 3 )
	{
		if( b_init || _b_background_reset_ui )
		{
			//_mog2_cpu->initialize( src_a.size(), 0.01f );
			_mog2_cpu->clear();
		}
		_mog2_cpu->apply( src_a, dst, _mog2_learning_rate_ui );
		//_mog2_cpu->operator()( src_a, dst, _mog2_learning_rate_ui );
		_mog2_cpu->getBackgroundImage( back );
	}
}

//void	c_bdd_img_cv_back_segm::do_gmg_cpu( cv::Mat& src_a, cv::Mat& dst, cv::Mat& back )
//{
//	//if (!_gmg_cpu)
//	//{
//	//	_gmg_cpu = new cv::BackgroundSubtractorGMG();
//	//	_gmg_cpu->initialize( src_a.size(), 0, 255 );
//	//}
//	if( _b_size_changed || _b_background_reset_ui )
//	{
//		_gmg_cpu.initialize( src_a.size(), 0, 255 );
//	}
////	if (_gmg_cpu)
//	{
//		INT32	smoothing_radius = _gmg_smoothing_radius_ui;	// must be odd
//		if ((smoothing_radius % 2) == 0 )
//			++smoothing_radius;
//
//		_gmg_cpu.maxFeatures				= _gmg_max_feature_ui;
//		_gmg_cpu.numInitializationFrames	= _gmg_num_initialization_frames_ui;
//		_gmg_cpu.smoothingRadius			= smoothing_radius;
//		_gmg_cpu.quantizationLevels			= _gmg_quantization_levels_ui;
//		_gmg_cpu.backgroundPrior			= _gmg_background_prior_ui;
//		_gmg_cpu.decisionThreshold			= _gmg_decision_threshold_ui;
//		_gmg_cpu.updateBackgroundModel		= _gmg_b_update_background_model_ui ? true : false;
//
////		_gmg_cpu->decisionThreshold
//		_gmg_cpu( src_a, dst, _mog2_learning_rate_ui );
//		_gmg_cpu.getBackgroundImage( back );
//	}
//}

//void	c_bdd_img_cv_back_segm::do_gmg( cv::cuda::GpuMat& src_a, cv::cuda::GpuMat& dst )
//{
//	//if (!_gmg_gpu)
//	//{
//	//	_gmg_gpu = new cv::cuda::GMG_GPU();
//	//	_gmg_gpu->initialize( src_a.size() );
//	//}
//	if( _b_size_changed || _b_background_reset_ui )
//	{
//		_gmg_gpu.initialize( src_a.size() );
//	}
//	//	if (_gmg_gpu)
//	{
//		_gmg_gpu.maxFeatures				= _gmg_max_feature_ui;
//		_gmg_gpu.numInitializationFrames	= _gmg_num_initialization_frames_ui;
//		_gmg_gpu.smoothingRadius			= _gmg_smoothing_radius_ui;
//		_gmg_gpu.quantizationLevels			= _gmg_quantization_levels_ui;
//		_gmg_gpu.backgroundPrior			= _gmg_background_prior_ui;
//		_gmg_gpu.decisionThreshold			= _gmg_decision_threshold_ui;
//		_gmg_gpu.updateBackgroundModel		= _gmg_b_update_background_model_ui ? true : false;
//
//		_gmg_gpu( src_a, dst, _mog2_learning_rate_ui );
//	//	_gmg_gpu->getBackgroundImage( back );
//	}
//}

//void	c_bdd_img_cv_back_segm::do_fgd( cv::cuda::GpuMat& src_a, cv::cuda::GpuMat& dst, cv::cuda::GpuMat& back )
//{
//	if( _fgd_out_channel_nb_ui != _fgd_out_channel_nb )
//	{
//		if( _fgd_stat_gpu )
//		{
//			_fgd_stat_gpu->release();
//		}
//		SAFE_DELETE( _fgd_stat_gpu );
//		_fgd_out_channel_nb = _fgd_out_channel_nb_ui;
//	}
//	bool	b_init = _b_size_changed;
//	if (!_fgd_stat_gpu)
//	{
//		_fgd_stat_gpu = new cv::cuda::FGDStatModel( _fgd_out_channel_nb );
//		b_init = true;
//	}
//	if (_fgd_stat_gpu)
//	{
//		if( b_init || _b_background_reset_ui )
//		{
//			_fgd_stat_gpu->create( src_a );
//		}
//		_fgd_stat_gpu->update( src_a );
//
//		dst		= _fgd_stat_gpu->foreground;
//		back	= _fgd_stat_gpu->background;
//	}
//}

bool	c_bdd_img_cv_back_segm::do_background_segmentation( cv::UMat& dst, cv::UMat& dst_back )
{
	TBUF_ADD_OBJ( tbuf::CH_OPEN_CV, 1., "bdd_img_cv_back_segm", this );
	
	// GPU methods
#if AAA_LIB_USE_OPENCV_CUDA()
	if( _b_gpu )
	{
		if( aaa::c_cv::is_cuda_use() )
		{
			cv::cuda::GpuMat*	gpumat_src_a = _src_cur_gpu;

			// test source size
			if( gpumat_src_a->empty() )
				return false;

			if (_s_stereo_algo_ui == BACK_SEG_MOG)
			{
				do_mog( *gpumat_src_a, _gpumat_fore_mask, _gpumat_back );
				_src_cur_gpu = &_gpumat_fore_mask;
			}
			else if (_s_stereo_algo_ui == BACK_SEG_MOG2)
			{
				do_mog2( *gpumat_src_a, _gpumat_fore_mask, _gpumat_back );
				_src_cur_gpu = &_gpumat_fore_mask;
			}
			//else if (_s_stereo_algo_ui == BACK_SEG_GMG)
			//{
			//	do_gmg( gpumat_src_a, _gpumat_fore_mask );
			//	_src_cur_gpu = _gpumat_fore_mask;
			//}
			//else if (_s_stereo_algo_ui == BACK_SEG_FGDSTAT)
			//{
			//	do_fgd( gpumat_src_a, _gpumat_fore_mask, _gpumat_back );
			//	_src_cur_gpu = _gpumat_fore_mask;
			//}
			//if( _gpumat_fore_mask.size() != cv::Size(0,0) )
			//	_gpumat_fore_mask.download( dst );
			//if( _b_back_enable_ui && _gpumat_back.size() != cv::Size(0,0) )
			//{
			//	_gpumat_back.download( dst_back );
			//}
			if( _b_fore_enable_ui && _gpumat_fore_mask.size() != cv::Size(0,0) )
			{
				if( !_gpumat_foreground.empty() )
					_gpumat_foreground.setTo( cv::Scalar::all(0) );
				gpumat_src_a->copyTo( _gpumat_foreground, _gpumat_fore_mask );
				//_gpumat_foreground.download( _fore_dst_back );
			}
		}
		else
		{
			ERR_PRINT_STRING( "Error No CUDA device to execute background segmentation on GPU" );
			goto exit;
		}
	}
	else
#endif
	{
		//if (_s_stereo_algo_ui == BACK_SEG_MOG)
		//{
		//	do_mog_cpu( _src_cur, _mat_fore, _mat_back );
		//	_src_cur = _mat_fore;
		//}
		//else
		if (_s_stereo_algo_ui == BACK_SEG_MOG2)
		{
			do_mog2_cpu( *_src_cur, _mat_fore, _mat_back );
			_src_cur = &_mat_fore;
		}
		//else if (_s_stereo_algo_ui == BACK_SEG_GMG)
		//{
		//	do_gmg_cpu( _src_cur, _mat_fore, _mat_back );
		//	_src_cur = _mat_fore;
		//}
		_mat_fore.copyTo( dst );
		_mat_back.copyTo( dst_back );
		if( _b_fore_enable_ui && _mat_fore_mask.size() != cv::Size(0,0) )
		{
		//	cv::UMat*	src_img = _src_cur;
			_src_cur->copyTo( _fore_dst_back, _mat_fore_mask );
		//	_mat_foreground.download( _fore_dst_back );
		}
	}
	//b_ok = build_dst_index( _mat_dst, _dst_img_index_ui, *_src_cur, "Foreground Mask", aaa::PIXEL_TYPE::GREY );
	//if( _b_back_enable_ui )
	//{
	//	b_ok &= build_dst_index( _mat_dst_back, _back_img_index_ui, *_src_cur, "Background", type );
	//}
	//if( _b_fore_enable_ui )
	//{
	//	b_ok &= build_dst_index( _fore_dst_back, _fore_img_index_ui, *_src_cur, "Foreground", aaa::PIXEL_TYPE::RGBA );
	//}
	update_dst_index( _mat_dst, _dst_img_index_ui, "Foreground Mask", _b_src_bgr );
	//c_img_2d*	img	= bind_img::get_ready( _dst_img_index  );
	//img->set_changed();
	if( _b_back_enable_ui )
	{
		update_dst_index( _mat_dst_back, _back_img_index_ui, "Background", _b_src_bgr );
		//c_img_2d*	img	= bind_img::get_ready( _back_img_index_ui  );
		//img->set_changed();
	}
	if( _b_fore_enable_ui )
	{
		update_dst_index( _fore_dst_back, _fore_img_index_ui, "Foreground", _b_src_bgr );
//		c_img_2d*	img	= bind_img::get_ready( _fore_img_index_ui  );
//		img->set_changed();
	}
#if AAA_LIB_USE_OPENCV_CUDA()
exit:
#endif
	TBUF_ADD_OBJ( tbuf::CH_OPEN_CV, 0., nullptr, this );
	return true;
}

//bool	c_bdd_img_cv_back_segm::build_dst_index( cv::UMat& dst, INT32 index, cv::UMat& ref, CHAR* mess, aaa::PIXEL_TYPE type )
//{
//	if( index == -1 )
//		index = _dst_img_index_ui;
//	if( index >= 0 )
//	{
//		return build_cv_mat( dst, index, ref, mess, type );
//	}
//	return false;
//}

//void	c_bdd_img_cv_back_segm::update_dst_index( cv::UMat& dst, INT32 index )
//{
//	if( index == -1 )
//		index = _dst_img_index_ui;
//	if( index >= 0 )
//	{
//		//if( is_gpu_use() )
//		//{
//		//	_src_cur_gpu.download( dst );
//		//}
//		c_img_2d*	img	= bind_img::get_ready( index );
//		if( img )
//			img->set_changed();
//	}
//}
