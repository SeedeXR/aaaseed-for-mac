#include "bdd_img_smooth.h"
#include <set>
#include <deque>
#include "spy.h"
#include "image/img_compo.h"


FACTORY_CREATE_PROP_V1( c_bdd_img_smooth, bdd_img_smooth, Image Smooth, bdd_img_smooth, sub_menu="Image"; );

CONST	INT32 MAA_MIN_X	= CV_BILATERAL	+	1;
CONST	INT32 MAA_MAX_X	= MAA_MIN_X		+	1 ;
CONST	INT32 MAA_GROW	= MAA_MAX_X		+	1 ;

CONST	INT32	SMOOTH_METHOD_MAX_NB = 8;
static	INT32	cv_smooth_method[ SMOOTH_METHOD_MAX_NB ] =
{
	CV_BLUR_NO_SCALE,
	CV_BLUR,
	CV_GAUSSIAN,
	CV_MEDIAN,
	CV_BILATERAL,
	MAA_MIN_X,
	MAA_MAX_X,
	MAA_GROW
};

static	C_PCHAR_C	str_smooth[SMOOTH_METHOD_MAX_NB] =
{
	"BLUR_NO_SCALE",
	"BLUR",
	"GAUSSIAN",
	"MEDIAN",
	"BILATERAL",
	"MIN_X",
	"MAX_X",
	"GROW"
};

namespace n_bdd_img_smooth
{
	CONSTEXPR INT32	BASE_PARAM_NB	=	9 + c_bdd_img::SUPER_PARAM_NB;
	CONSTEXPR INT32	GROUP_PARAM_NB	=	0;

	CONSTEXPR INT32	PARAM_NB	=	BASE_PARAM_NB
								+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB] =
	{
		BDD_IMG_BASE_PARAMS

		PARAM_DEF_BOOL_ON(			smooth				)
		PARAM_DEF_BIND_2D_ALONE(	smooth_image_dst	)
		PARAM_DEF_BASE_STR(			TYPE_SYMBOLIC,		smooth_method,	1,2,	1,	// method zero(BLUR_NO_SCALE) provoke cv crash
																				PT_NB_STR(str_smooth),		str_smooth	)
		PARAM_DEF_INT32_XY(			smooth_size,		1, 3,	1, 256	)

//		PARAM_DEF_BOOL_OFF(			grow	)

		PARAM_DEF_BOOL_OFF(			bc					)
		PARAM_DEF_BIND_2D_ALONE(	bc_image_dst		)
		PARAM_DEF_REAL_ZERO(		bc_min				)
		PARAM_DEF_REAL_ONE(			bc_max				)
	};
}


void	c_bdd_img_smooth::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start_src_resize( false, false );

	param_set_pt( h, _b_smooth_ui			);
	param_set_pt( h, _smooth_image_dst_ui	);
	param_set_pt( h, _s_smooth_ui			);
	param_set_pt( h, _smooth_size_x_ui		);
	param_set_pt( h, _smooth_size_y_ui		);

//	param_set_pt( h, _b_grow			);

	param_set_pt( h, _b_bc_active_ui	);
	param_set_pt( h, _bc_image_dst_ui	);
	param_set_pt( h, _bc_min_ui			);
	param_set_pt( h, _bc_max_ui			);

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bdd_img_smooth )
	,_ipl_smooth	(nullptr)
	,_ipl_bc		(nullptr)
	,_bc_lutmat		(nullptr)
{
	param_init_with( n_bdd_img_smooth::param, n_bdd_img_smooth::PARAM_NB );
}

void	c_bdd_img_smooth::close()
{
	IF_THIS_NULL_RETURN();

	_ipl_smooth.release();
	_ipl_bc.release();
	cvReleaseMat( &_bc_lutmat );
}

c_bdd_img_smooth::~c_bdd_img_smooth()
{
	close();
}

//todo process in draw ?
void	c_bdd_img_smooth::update()
{	
	if( update_img_src() )
		do_process();
}

void	c_bdd_img_smooth::draw()
{
}

void	c_bdd_img_smooth::do_process()
{
	TBUF_ADD_OBJ( tbuf::CH_OPEN_CV, 1., "bdd_img_smooth", this );

	try
	{
		c_img_ipl*	img_cur = get_in_to_use();
		if( !img_cur->get_ipl() )	//todo have a valid fns
			return;

		////	grow special stuff
		//if( _b_grow )
		//{

		//}

		// smooth filter
		if( _b_smooth_ui )
		{
			if( build_dst_image( _ipl_smooth, _smooth_image_dst_ui, *img_cur, "Smooth image", img_cur->get_channel_nb() ) )
			//if( _ipl_smooth.build( _smooth_image_dst_ui, *img_cur, "Smooth image" ) )
			{
				if( do_smooth( img_cur, &_ipl_smooth ) )
				{
					_ipl_smooth.set_changed();
					img_cur = &_ipl_smooth;
					validate_dst_change( _smooth_image_dst_ui );
				}
			}
		}



		// brightness & contrast filter
		if( _b_bc_active_ui )
		{
			if( build_dst_image( _ipl_bc, _bc_image_dst_ui, *img_cur, "Brightness Contrast image", img_cur->get_channel_nb() ) )
			//if( _ipl_bc.build( _bc_image_dst_ui, *img_cur, "Brightness Contrast image" ) )
			{
				// mapping from input to output values via lookup table, recalculated when brightness or contrast changed
				do_bc( img_cur, &_ipl_bc );
				_ipl_bc.set_changed();
				img_cur = &_ipl_bc;
				validate_dst_change( _bc_image_dst_ui );
			}
		}
	}
	catch( cv::Exception& e ) {	ERR_PRINT_STRING( "Open CV Exception : %s", e.what() );	}

	TBUF_ADD_OBJ( tbuf::CH_OPEN_CV, 0., nullptr, this );

//	_output = ipl_cur;
}

//todo avoid duplication og code see multitouch 
void c_bdd_img_smooth::bc_update_lut( void )
{	
	if( _bc_min == _bc_max )
	{
		uchar v = uchar( _bc_min * aaa::img::REAL_NEARLY_256 );
		for( INT32 i = 0; i < 256; ++i )
			_bc_lut[i] = v;
	}
	else
	{
		REAL CONST b = _bc_min * REAL(255);
		REAL CONST a = aaa::img::REAL_NEARLY_256 / (REAL(255)*(_bc_max-_bc_min));
		uchar*	pt = _bc_lut-1;
		for( REAL r=0; r<255.5; r+=1 )
		{
			*++pt = uchar( CLAMP( (r-b)*a, 0., 255.) );
		}
	}

	// already done	cvSetData( _bc_lutmat, _bc_lut, 0 );
}

void	c_bdd_img_smooth::do_bc( c_img_ipl CONST * CONST src, c_img_ipl* CONST dst )
{
	if( !check_src_dst( src, dst ) )
		return;
	if( !_bc_lutmat )
	{
		_bc_lutmat = cvCreateMatHeader( 1, 256, CV_8UC1 );	//todo dealloc
		cvSetData( _bc_lutmat, _bc_lut, 0 );
	}
	if( _bc_min != _bc_min_ui || _bc_max != _bc_max_ui )
	{
		_bc_min = _bc_min_ui;
		_bc_max = _bc_max_ui;
		bc_update_lut();
	}
	cvLUT( src->get_ipl(), dst->get_ipl(), _bc_lutmat ); 
}

bool	c_bdd_img_smooth::do_smooth_opencv( c_img_ipl CONST * CONST src, c_img_ipl* CONST dst )
{
	return ipl::cv_smooth( src->get_ipl(), dst->get_ipl(), cv_smooth_method[_s_smooth_ui], _smooth_size_x_ui, _smooth_size_y_ui );
}

bool	c_bdd_img_smooth::do_smooth( c_img_ipl CONST * CONST src, c_img_ipl* CONST dst )
{
	if( !check_src_dst( src, dst ) )
		return false;
	if( _s_smooth_ui < MAA_MIN_X )
		return do_smooth_opencv( src, dst );
	else if( _s_smooth_ui < MAA_MAX_X )
		return do_smooth_maa( src, dst );
	else
		return do_grow( src, dst );
}

INT32	vector_find_max( std::deque<INT32> &vec )
{
	INT32 CONST	size = (INT32)vec.size();
	INT32 max = 0;
	for( INT32 i=0; i<size; ++i )
	{
		INT32 val = vec[i]; 
		if( max<val )
			max = val;
	}
	return max;
}

INT32	vector_find_min( std::deque<INT32> &vec )
{
	INT32 CONST	size = (INT32)vec.size();
	INT32 min = 255;
	for( INT32 i=0; i<size; ++i )
	{
		INT32 val = vec[i]; 
		if( min > val )
			min = val;
	}
	return min;
}

FINLINE	INT32	find_range( std::deque<INT32> &vec )
{
	INT32 CONST	size = (INT32)vec.size();
	INT32	min = 255;
	INT32	max = 0;
	for( INT32 i=0; i<size; ++i )
	{
		INT32 val = vec[i]; 
		if( min > val )
			min = val;
		if( max < val )
			max = val;
	}
	return max - min;
}

bool	c_bdd_img_smooth::do_smooth_maa( c_img_ipl CONST * CONST src, c_img_ipl* CONST dst )
{
	if( src->get_channel_nb() == 1 )
	{
		INT32				i_beg	= _smooth_size_x_ui;
		INT32				sx		= src->get_size_x();
		INT32				sy		= src->get_size_y();
		INT32				i_end	= sx - _smooth_size_x_ui;
		std::deque<INT32>	past;

		for( INT32 j=0; j < sy; ++j )
		{
			INT32	i;
			INT32	p;
			past.clear();
			for( i=0 ;i < i_beg; ++i )
			{
				p = src->get_pixel<ipl::pixel_gray>( i, j );
				past.push_back(p);
				dst->set_pixel<ipl::pixel_gray>( i, j, find_range(past) );
			}
			for( ;i < sx; ++i )
			{
				past.pop_front();
	
				p = src->get_pixel<ipl::pixel_gray>( i, j );
				past.push_back(p);
				dst->set_pixel<ipl::pixel_gray>( i, j, find_range(past) );
			}
		}
		//	(*img_hd)->set_changed( true );
		return true;
	}

	ERR_PRINT_STRING( "filter only implemented for 1 channel image. skipping it ." );
	return false;
}

bool	c_bdd_img_smooth::do_grow( c_img_ipl CONST * CONST src, c_img_ipl* CONST dst )
{
	if( src->get_channel_nb() !=  1 )
	{
		ERR_PRINT_STRING( "filter only implemented for 1 channel image. skipping it ." );
		return false;
	}

	cvCopy( src->get_ipl(), dst->get_ipl(), 0 );

	INT32			i_beg	= _smooth_size_x_ui;
	INT32			sx		= dst->get_size_x();
	INT32			sy		= dst->get_size_y();
	INT32			i_end	= sx - _smooth_size_x_ui;

	for( INT32 nb=_smooth_size_x_ui; nb>0; --nb )
	{
		for( INT32 j=1; j < sy-1; ++j )
		{
			UINT8	p;
			for( INT32 i=1; i < sx-1; ++i )
			{
				p = dst->get_pixel<ipl::pixel_gray>( i, j );
				if( p==0 )
				{
					p = dst->get_pixel<ipl::pixel_gray>( i-1, j );
					p = MAX( p, dst->get_pixel<ipl::pixel_gray>( i+1, j ) );
					p = MAX( p, dst->get_pixel<ipl::pixel_gray>( i, j-1 ) );
					p = MAX( p, dst->get_pixel<ipl::pixel_gray>( i, j+1 ) );
					if( p > 0 )
					{
						--p;
						dst->set_pixel<ipl::pixel_gray>( i, j, p );
					}
				}
			}
		}
	}

	return true;
}

