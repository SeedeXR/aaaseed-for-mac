#include "multi_screen.h"
#include "infrastructure/viewport.h"
#include "math/v.h"
#include "infrastructure/param/param_declare.h"

FACTORY_CREATE_V1( c_multi_screen, multi_screen, Multiple Screen, multi_screen );

namespace n_multi_screen
{
	CONSTEXPR INT32 PARAM_BASE_NB	= 22;
	CONSTEXPR INT32 PARAM_GROUP_NB	= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	PARAM_BASE_NB
									+	PARAM_GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_INT32(		x_nb,			2, 1,	1, PARAM_MAX_INT32 )
		PARAM_DEF_BOOL_OFF(		x_right_to_left	)
		PARAM_DEF_INT32_LOCKED(	x_index			)
		PARAM_DEF_REAL_ONE(		x_overlap		)
		PARAM_DEF_BOOL_ON(		x_compensate_frustum )
		PARAM_DEF_REAL_ZERO(	x_rot_x_step	)
		PARAM_DEF_REAL_ZERO(	x_rot_y_step	)
		PARAM_DEF_REAL_ZERO(	x_rot_z_step	)
		PARAM_DEF_REAL_ZERO(	x_sca_x_step	)
		PARAM_DEF_REAL_ZERO(	x_sca_y_step	)
		PARAM_DEF_REAL_ZERO(	x_sca_z_step	)

		PARAM_DEF_INT32(		y_nb,			2, 1,	1, PARAM_MAX_INT32 )
		PARAM_DEF_BOOL_OFF(		y_top_to_bottom	)
		PARAM_DEF_INT32_LOCKED(	y_index			)
		PARAM_DEF_REAL_ONE(		y_overlap		)
		PARAM_DEF_BOOL_ON(		y_compensate_frustum )
		PARAM_DEF_REAL_ZERO(	y_rot_x_step	)
		PARAM_DEF_REAL_ZERO(	y_rot_y_step	)
		PARAM_DEF_REAL_ZERO(	y_rot_z_step	)
		PARAM_DEF_REAL_ZERO(	y_sca_x_step	)
		PARAM_DEF_REAL_ZERO(	y_sca_y_step	)
		PARAM_DEF_REAL_ZERO(	y_sca_z_step	)
	};
}

c_multi_screen*	c_multi_screen::def = nullptr;
c_multi_screen*	c_multi_screen::cur = nullptr;

void	c_multi_screen::param_init_pt()
{
	INT32	h=0;

	param_set_pt(		h, _x_nb				);
	param_set_pt(		h, _b_x_right_to_left	);
	param_set_pt(		h, _x_index				);
	param_set_pt(		h, _x_overlap			);
	param_set_pt(		h, _b_x_frustum			);
	param_set_pt_3(	h, _x_rot_step			);
	param_set_pt_3(	h, _x_sca_step			);

	param_set_pt(		h, _y_nb				);
	param_set_pt(		h, _b_y_top_to_bottom	);
	param_set_pt(		h, _y_index				);
	param_set_pt(		h, _y_overlap			);
	param_set_pt(		h, _b_y_frustum			);
	param_set_pt_3(	h, _y_rot_step			);
	param_set_pt_3(	h, _y_sca_step			);

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_multi_screen )
{
	param_init_with( n_multi_screen::param, n_multi_screen::PARAM_NB_MAX );
}

c_multi_screen::~c_multi_screen()
{
	if( cur == this )
		cur = nullptr;
}

void	c_multi_screen::update()
{
	_x_index = -1;
	_y_index = 0;
	_index = -1;
	_nb = _x_nb * _y_nb;
	c_multi_screen::cur = this;
}

bool	c_multi_screen::next()
{
	++_index;
	++_x_index;
	if( _x_index >= _x_nb )
	{
		++_y_index;
		if( _y_index >= _y_nb )
			return false;
		_x_index = 0;
	}
	return true;
}

void	c_multi_screen::set_view( INT32 x, INT32 y, INT32 sx, INT32 sy )
{
	if( _x_nb > 1 )
	{
		REAL	tmp = REAL( sx ) / _x_nb;
		INT32	ind = _b_x_right_to_left ? ( _x_nb-_x_index-1 ) : _x_index ;
		x += INT32( ind * tmp );
		sx = INT32( tmp );
	}
	if( _y_nb > 1 )
	{
		REAL	tmp = REAL( sy ) / _y_nb;
		INT32	ind = _b_y_top_to_bottom ? ( _y_nb-_y_index-1 ) : _y_index ;
		y += INT32( ind * tmp );
		sy = INT32( tmp );
	}
	viewport_render->set_rect_do( x, y, sx, sy );
}

void	c_multi_screen::do_scale( REAL* tmp )
{
	add_scale_v3( tmp, _x_sca_step, REAL( _x_index ) );
	add_scale_v3( tmp, _y_sca_step, REAL( _y_index ) );
}

void	c_multi_screen::do_rot( REAL* tmp )
{
	add_scale_v3( tmp, _x_rot_step, REAL( _x_index ) );
	add_scale_v3( tmp, _y_rot_step, REAL( _y_index ) );
}

void	c_multi_screen::adapt_for_cam( FP32& left, FP32& right, FP32& top, FP32& bottom )
{
	if( _x_nb > 1 )
	{
		DOUBLE sx = ( right-left ) / ( _x_nb - ( _x_nb-1 ) * _x_overlap );
		if( _b_x_frustum )
			left += FP32(_x_index * ( 1.- _x_overlap ) * sx);
		else
			left /= FP32( _x_nb ) ;
		right = FP32(left + sx);
	}
	if( _y_nb > 1 )
	{
		DOUBLE sy = ( top-bottom ) / ( _y_nb - ( _y_nb-1 ) * _y_overlap );
		if( _b_y_frustum )
			bottom += FP32( _y_index * ( 1.- _y_overlap ) * sy);
		else
			bottom /= FP32( _y_nb ) ;
		top = FP32(bottom + sy);
	}
}
