#include "bdd_img_cv_mix.h"
#include "image/bind_img_2d.h"
#include "image/img_compo.h"
#include "spy.h"


FACTORY_CREATE_PROP_V1( c_bdd_img_cv_mix, bdd_img_cv_mix, Image OpenCV Mix, bdd_img_cv_mix, sub_menu="Image"; );

enum CV_MIX_METHOD : INT32
{
	MIX_ADD = 0,
	MIX_SUB_ABS,
	MIX_A_SUB_B,
	MIX_B_SUB_A,
	MIX_MIN,
	MIX_MAX,
	MIX_MUL,
	MIX_DIV,
	CV_MIX_METHOD_MAX_NB
};

static	C_PCHAR_C	str_mix[CV_MIX_METHOD_MAX_NB] =
{
	"ADD",
	"ABS(A-B)",
	"A-B",
	"B-A",
	"MIN",
	"MAX",
	"MUL",
	"DIV",
};

namespace n_bdd_img_cv_mix
{
	CONSTEXPR INT32 BASE_PARAM_NB	=	c_bdd_img::SUPER_SRC_2_PARAM_NB + 5;
	CONSTEXPR INT32 GROUP_PARAM_NB	=	0;
	CONSTEXPR INT32 PARAM_NB	=	BASE_PARAM_NB
								+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB] =
	{
		BDD_IMG_BASE_2_PARAMS

		PARAM_DEF_SYMBO_PSTR_ZERO(	mix_method,	str_mix		)
		PARAM_DEF_REAL_ONE(			factor_a	)
		PARAM_DEF_REAL_ONE(			factor_b	)
		PARAM_DEF_REAL_ZERO(		offset		)
		PARAM_DEF_REAL_ONE(			scale		)
	};
}

void	c_bdd_img_cv_mix::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start_src_resize( true, false );

	param_set_pt( h, _s_mix		);
	param_set_pt( h, _factor_a	);
	param_set_pt( h, _factor_b	);
	param_set_pt( h, _offset	);
	param_set_pt( h, _scale		);

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bdd_img_cv_mix )
	//,_ipl_dst		( nullptr )
	//,_ipl_src_b	( nullptr )
{
	param_init_with( n_bdd_img_cv_mix::param, n_bdd_img_cv_mix::PARAM_NB );
}

void	c_bdd_img_cv_mix::close()
{
	IF_THIS_NULL_RETURN();

	_mat_dst.release();
#if AAA_LIB_USE_OPENCV_CUDA()
	_mat_gpu_dst.release();
#endif
	//_ipl_dst.release();
	//_ipl_src_b.release();
}

c_bdd_img_cv_mix::~c_bdd_img_cv_mix()
{
	close();
}

void	c_bdd_img_cv_mix::update()
{	
	if( update_mat_src_2() )
	{
		if( _src_cur->channels() == _src_b_cur->channels() )
		{
			do_process();
		}
		else
		{
			ERR_PRINT_STRING( "c_bdd_img_cv_mix : source have different channels number %d and %d", _src_cur->channels(), _src_b_cur->channels() );
		}
	}
}

void	c_bdd_img_cv_mix::draw()
{	
}

void	c_bdd_img_cv_mix::do_process()
{
//	bool b_ok = build_cv_mat( _mat_dst, _dst_img_index, _src_cur, "Mix" );
//	bool b_ok = build_cv_mat( _mat_dst, _dst_img_index, _src_size_x, _src_size_y, "Mix" );
//	build_dst_index( _mat_dst, _dst_img_index_ui, "Mix" );
//	if( b_ok )
	{
		cv::UMat*	dst = &_mat_dst;
		bool	b_mix_ok = false;
#if AAA_LIB_USE_OPENCV_CUDA()
		if( is_gpu_use() )
		{
			cv::cuda::GpuMat*	src_a = _src_cur_gpu;
			cv::cuda::GpuMat*	src_b = _src_b_cur_gpu;
			if( _b_swap_src_ui )
			{
				src_a = _src_b_cur_gpu;
				src_b = _src_cur_gpu;
			}
			b_mix_ok = do_mix_gpu( *src_a,  *src_b, _mat_gpu_dst );
			if( b_mix_ok )
				_mat_gpu_dst.download( *dst );
		}
		else
#endif
		{
			cv::UMat*	src_a = _src_cur;
			cv::UMat*	src_b = _src_b_cur;
			if( _b_swap_src_ui )
			{
				src_a = _src_b_cur;
				src_b = _src_cur;
			}
			b_mix_ok = do_mix_cpu( *src_a, *src_b, *dst );
		}
		if( b_mix_ok )
		{
			c_img_2d*	img	= g_bind_img_2d->get_ready( _dst_img_index_ui  );
			img->set_changed();

		}
	}
}

//bool	c_bdd_img_smooth::do_smooth_opencv( c_img_ipl* src, c_img_ipl* dst )
//{
//	return ipl::cv_smooth( src->get_ipl(), dst->get_ipl(), cv_smooth_method[_s_smooth], _smooth_size_x, _smooth_size_y );
//}

//bool	c_bdd_img_cv_mix::do_mix( c_img_ipl* src_a, c_img_ipl* src_b, c_img_ipl* dst )
//{
//	tbuf_add( c_tbuf_master::CH_OPEN_CV, 1., "bdd_img_cv_mix", this );
//	ipl::p_ipl	a = src_a->get_ipl();
//	ipl::p_ipl	b = src_b->get_ipl();
//	ipl::p_ipl	d = dst->get_ipl();
//
//	if( !a || !b || !d )
//		return false;
//	if(		src_a->get_size_x()	!= src_b->get_size_x()
//		||	src_a->get_size_y()	!= src_b->get_size_y()
//		||	src_a->get_depth()	!= src_b->get_depth()
//		||	src_a->get_channel_nb()	!= src_b->get_channel_nb()
//		)
//	{
//		ERR_PRINT_STRING( "%s() source a and b should have the same size, channel nb for now", __FUNCTION__ );
//		ERR_PRINT_STRING( "\t source a : %d x %d ch %d depth %d", src_a->get_size_x(), src_a->get_size_y(), src_a->get_channel_nb(), src_a->get_depth() );
//		ERR_PRINT_STRING( "\t source b : %d x %d ch %d depth %d", src_b->get_size_x(), src_b->get_size_y(), src_b->get_channel_nb(), src_b->get_depth() );
//		return false;
//	}
//
//	switch( _s_mix )
//	{
//	case MIX_ADD:		cvAddWeighted( a, _factor_a, b, _factor_b, _offset * REAL_NEARLY_256, d );	break;
//	case MIX_SUB_ABS:	cvAbsDiff( a, b, d );			break;
//	case MIX_A_SUB_B:	cvSub( a, b, d );				break;
//	case MIX_B_SUB_A:	cvSub( b, a, d );				break;
//	case MIX_MIN:		cvMin( a, b, d );				break;
//	case MIX_MAX:		cvMax( a, b, d );				break;
//	case MIX_MUL:		cvMul( a, b, d, _scale/256. );	break;
//	case MIX_DIV:		cvDiv( a, b, d, _scale*256. );	break;
//	}
//	tbuf_add( c_tbuf_master::CH_OPEN_CV, 0., nullptr, this );
//	return true;
//}

bool	c_bdd_img_cv_mix::do_mix_cpu( cv::UMat& src_a, cv::UMat& src_b, cv::UMat& dst ) //c_img_ipl* src_a, c_img_ipl* src_b, c_img_ipl* dst )
{
	SPY_PUSH_RANGE_FUNCTION( spy::IMG_LOW );
	TBUF_ADD_OBJ( tbuf::CH_OPEN_CV, 1., "bdd_img_cv_mix", this );

	switch( _s_mix )
	{
	case MIX_ADD:		cv::addWeighted(	src_a, _factor_a,	src_b, _factor_b, _offset * aaa::img::REAL_NEARLY_256,
																			dst					);		break;
	case MIX_SUB_ABS:	cv::absdiff(		src_a,				src_b,		dst					);		break;
	case MIX_A_SUB_B:	cv::subtract(		src_a,				src_b,		dst					);		break;
	case MIX_B_SUB_A:	cv::subtract(		src_b,				src_a,		dst					);		break;
	case MIX_MIN:		cv::min(			src_a,				src_b,		dst					);		break;
	case MIX_MAX:		cv::max(			src_a,				src_b,		dst					);		break;
	case MIX_MUL:		cv::multiply(		src_a,				src_b,		dst, _scale/256.	);		break;
	case MIX_DIV:		cv::divide(			src_a,				src_b,		dst, _scale*256.	);		break;
	}

	TBUF_ADD_OBJ( tbuf::CH_OPEN_CV, 0., nullptr, this );
	SPY_POP_RANGE();
	return true;
}

#if AAA_LIB_USE_OPENCV_CUDA()
//#include "opencv2/cudaimgproc.hpp"
#include "opencv2/cudaarithm.hpp"
//#include "opencv2/cudawarping.hpp"
bool	c_bdd_img_cv_mix::do_mix_gpu( cv::cuda::GpuMat& src_a, cv::cuda::GpuMat& src_b, cv::cuda::GpuMat& dst ) //c_img_ipl* src_a, c_img_ipl* src_b, c_img_ipl* dst )
{
	SPY_PUSH_RANGE_FUNCTION( spy::IMG_LOW );
	TBUF_ADD_OBJ( tbuf::CH_OPEN_CV, 1., "bdd_img_cv_mix", this );

	switch( _s_mix )
	{
	case MIX_ADD:		cv::cuda::addWeighted(	src_a, _factor_a,	src_b, _factor_b, _offset * aaa::img::REAL_NEARLY_256,
																				dst					);		break;
	case MIX_SUB_ABS:	cv::cuda::absdiff(		src_a,				src_b,		dst					);		break;
	case MIX_A_SUB_B:	cv::cuda::subtract(		src_a,				src_b,		dst					);		break;
	case MIX_B_SUB_A:	cv::cuda::subtract(		src_b,				src_a,		dst					);		break;
	case MIX_MIN:		cv::cuda::min(			src_a,				src_b,		dst					);		break;
	case MIX_MAX:		cv::cuda::max(			src_a,				src_b,		dst					);		break;
	case MIX_MUL:		cv::cuda::multiply(		src_a,				src_b,		dst, _scale/256.	);		break;
	case MIX_DIV:		cv::cuda::divide(		src_a,				src_b,		dst, _scale*256.	);		break;
	}

	TBUF_ADD_OBJ( tbuf::CH_OPEN_CV, 0., nullptr, this );
	SPY_POP_RANGE();
	return true;
}
#endif
