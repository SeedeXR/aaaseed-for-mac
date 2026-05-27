#include "bdd_img_erode_dilate.h"
#include "spy.h"


FACTORY_CREATE_PROP_V1( c_bdd_img_erode_dilate, bdd_img_erode_dilate, Image Erode Dilate, bdd_img_erode_dilate, sub_menu="Image"; );


namespace n_bdd_img_erode_dilate
{

	CONSTEXPR INT32	BASE_PARAM_NB	= 7 + c_bdd_img::SUPER_PARAM_NB;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32	PARAM_NB	=	BASE_PARAM_NB
								+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB] =
	{
		BDD_IMG_BASE_PARAMS

		PARAM_DEF_BOOL_OFF(			dilate_first )

		PARAM_DEF_BOOL_OFF(			erode )
		PARAM_DEF_BIND_2D_ALONE(	erode_image_dst )
		PARAM_DEF_INT32(			erode_pass,		2, 1,	1, 256	)

		PARAM_DEF_BOOL_OFF(			dilate )
		PARAM_DEF_BIND_2D_ALONE(	dilate_image_dst )
		PARAM_DEF_INT32(			erode_pass,		2, 1,	1, 256	)
	};
}


void	c_bdd_img_erode_dilate::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start_src_resize( false, false );

	param_set_pt( h, _b_dilate_first );

	param_set_pt( h, _b_erode_active );
	param_set_pt( h, _erode_image_dst );
	param_set_pt( h, _erode_size);

	param_set_pt( h, _b_dilate_active );
	param_set_pt( h, _dilate_image_dst );
	param_set_pt( h, _dilate_size);

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bdd_img_erode_dilate )
	,_ipl_dilate	( nullptr )
	,_ipl_erode		( nullptr )
{
	param_init_with( n_bdd_img_erode_dilate::param, n_bdd_img_erode_dilate::PARAM_NB );
	force_mono();
}

void	c_bdd_img_erode_dilate::release()
{
	IF_THIS_NULL_RETURN();
	
	_ipl_erode.release();
	_ipl_dilate.release();
}

c_bdd_img_erode_dilate::~c_bdd_img_erode_dilate()
{
	release();
}

void	c_bdd_img_erode_dilate::do_process()
{
	TBUF_ADD_OBJ( tbuf::CH_OPEN_CV, 1., "bdd_img_erode_dilate", this );
	c_img_ipl*	ipl_cur = get_in_to_use();
	
	if( _b_dilate_first )
	{
		if( _b_dilate_active )
			ipl_cur = do_dilate( ipl_cur );
		if( _b_erode_active )
			ipl_cur = do_erode( ipl_cur );
	}
	else
	{
		if( _b_erode_active )
			ipl_cur = do_erode( ipl_cur );
		if( _b_dilate_active )
			ipl_cur = do_dilate( ipl_cur );
	}
	TBUF_ADD_OBJ( tbuf::CH_OPEN_CV, 0., nullptr, this );
//	_output = ipl_cur;
}

void	c_bdd_img_erode_dilate::update()
{	
#if AAA_LIB_USE_OPENCV_VERSION() < 330
	if( update_img_src() )
	{
		if( is_size_changed() )
			release();
		if( is_content_changed() )
			do_process();
	}
#else
	err_print( "%s() Unimplemented for now in : %s", __FUNCTION__, AAA_LIB_USE_OPENCV_VERSION_ASKED() );
#endif
}

void	c_bdd_img_erode_dilate::draw()
{	
}

c_img_ipl*	c_bdd_img_erode_dilate::do_erode( c_img_ipl* src )
{
#if AAA_LIB_USE_OPENCV_VERSION() < 330
	if( _ipl_erode.build( _erode_image_dst, *src, "Erode image" ) )
	{
		cvErode( src->get_ipl(), _ipl_erode.get_ipl(), nullptr, _erode_size );
		_ipl_erode.set_changed();
		return &_ipl_erode;
	}
#endif
	return nullptr;
}

c_img_ipl*	c_bdd_img_erode_dilate::do_dilate( c_img_ipl* src )
{
#if AAA_LIB_USE_OPENCV_VERSION() < 330
	if( _ipl_dilate.build( _dilate_image_dst, *src, "Dilate image" ) )
	{
		cvDilate( src->get_ipl(), _ipl_dilate.get_ipl(), nullptr, _dilate_size );
		_ipl_dilate.set_changed();
		return &_ipl_dilate;
	}
#endif
	return nullptr;
}
