#include "def_image.h"
#include "image/bind_img_2d.h"
#include "ui/strsymbo.h"
#include "draw/model.h"
#include "math/matrix.h"
#include "math/quaternions.h"
#include "infrastructure/compute_parallel.h"
#if	AAA_DEBUG()
#	include "float.h"
#endif
#include "infrastructure/param/param_declare.h"


FACTORY_INSTANCE_V1( c_def_image, def_image, Deformer Image, def );

//todo implement all this especially on video
/*
	"Average",
	"Max",
	"Max with decay",
	"Interpolate",
*/

namespace n_def_img
{
	CONSTEXPR INT32 BASE_PARAM_NB		= 8 + c_deformer::BASE_PARAM_NB;
	CONSTEXPR INT32 POSITION_PARAM_NB	= 18;
	CONSTEXPR INT32 GROUP_NB			= 1;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	POSITION_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[ PARAM_NB_MAX ] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_BIND_2D_ALONE( Image_src )

		PARAM_DEF_GROUP( Position, POSITION_PARAM_NB )
			PARAM_DEF_AXE_X(		axe_src					)
			PARAM_DEF_BOOL_ON(		scale_by_model			)
			PARAM_DEF_SCALE_UVF(	scale					)
			PARAM_DEF_BOOL_ON(		origin_at_image_center	)
			PARAM_DEF_POINT_XYZ(	origin					)
			PARAM_DEF_REAL_ZERO(	rotation				)
			PARAM_DEF_BOOL_OFF(		clamped					)
			PARAM_DEF_BOOL_OFF(		linear					)
			PARAM_DEF_BOOL_OFF(		ellipse					)
			PARAM_DEF_SCALE_UVF(	radius					)
			PARAM_DEF_INT32(		ellipse_point_nb,		3,12,	3,128	)
//			PARAM_DEF_POINT_XYZ( rotation )

		PARAM_DEF_AXE_Y(		axe_dst				)
		PARAM_DEF_SYMBO_PSTR(	Image_component,	1, INT32(aaa::COMPO::RED),		aaa::str_compo )
		PARAM_DEF_BOOL_OFF(		Gradient			)
		PARAM_DEF_REAL_ZERO(	Normal				)
		PARAM_DEF_BOOL_OFF(		use_RGB				)

//		{	nullptr,	PARAM_REAL,		"exponent",			0, 1,	PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },
//		PARAM_DEF_GAIN(			shape_gain			)
//		PARAM_DEF_BIAS(			shape_bia			)

		PARAM_DEF_REAL_ZERO(	strenght			)
		PARAM_DEF_REAL_ZERO(	offset				)
		PARAM_DEF_BOOL_OFF(		white_multiply		)
//		PARAM_DEF_BOOL_ON(		parallel			)
	};
}

CONSTRUCTOR_CREATE(c_def_image)
{
	init_name_with( "Def Image" );
	param_init_with( n_def_img::param, n_def_img::PARAM_NB_MAX );
	init();
}

void c_def_image::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt( h, _img_index );

	++h;
		param_set_pt(	h, _i_src_axe					);
		param_set_pt(	h, _b_scale_by_model			);
		param_set_pt_3(	h, _scale_ui					);
		param_set_pt(	h, _b_origin_at_image_center	);
		param_set_pt_3(	h, _origin						);
		param_set_pt(	h, _rotation					);
		param_set_pt(	h, _b_clamped_ui				);
		param_set_pt(	h, _b_linear_ui					);
		param_set_pt(	h, _b_ellipse					);
		param_set_pt_3(	h, _r_ui						);
		param_set_pt(	h, _ellipse_nb					);

	param_set_pt( h, _i_dst_axe			);
	param_set_pt( h, _s_compo			);
	param_set_pt( h, _b_gradient		);
	param_set_pt( h, _normal			);
	param_set_pt( h, _b_use_rgb			);

	param_set_pt( h, _strenght			);
	param_set_pt( h, _offset			);
	param_set_pt( h, _b_white_multiply	);
//	param_set_pt( h, _b_parallel_ui		);

	err_param_init_pt(h);
}

void c_def_image::init()
{
}

EMPTY_DESTRUCTOR(c_def_image)

void	c_def_image::update()
{
	_img = g_bind_img_2d->get_ready( _img_index );
	if( _img && _img->is_valid() )
	{
		scale_v2( _scale, _scale_ui, _scale_ui[2] );
		if( _scale[0] != 0. && _scale[1] != 0. )
		{
			//	prepare the axes
			axe_build_index( _i_src_u, _i_src_v, _i_src_axe );
			if( _b_scale_by_model )
				c_model::cur->scale_v2( _scale );
			if( _b_ellipse )
			{
				_r[0] = _r_ui[0] * _r_ui[2] / _scale[0];
				_r[1] = _r_ui[1] * _r_ui[2] / _scale[1];
			}
			set_deforming( true );
			return;
		}
	}
	set_deforming( false );
}

template <bool B_ROT>
void	c_def_image::apply_private( REAL CONST * src, REAL* dst, INT32 nb )
{
	REAL sin_rot;
	REAL cos_rot;

	if( B_ROT )
		GET_SIN_COS_TURN( sin_rot, cos_rot, _rotation );

	REAL o_u = _origin[ _i_src_u ];
	REAL o_v = _origin[ _i_src_v ];
	if( _b_origin_at_image_center )
	{
		o_u -= REAL(.5);
		o_v -= REAL(.5);
	}

	REAL over_sca_u = OVER_ONE_AS_REAL( _scale[0] );
	REAL over_sca_v = OVER_ONE_AS_REAL( _scale[1] );

//	bool b_parallel = _b_parallel_ui && aaa::parallel::is_use();
	if( _b_gradient )
	{
		INT32 i_dst_u;
		INT32 i_dst_v;
		axe_build_index( i_dst_u, i_dst_v, _i_dst_axe );

/*
		if( b_parallel )
		{
			PARALLEL_LIB::parallel_for( 0, nb, [&]( INT32 CONST i ) NOEXCEPT
			{
				REAL* src	=	get_src_pt( 0 ) + 3 * i;
				REAL* dst	=	get_dst_pt( 0 ) + 3 * i;

				REAL u = *(src + _i_src_u) * over_sca_u - o_u;
				REAL v = *(src + _i_src_v) * over_sca_v - o_v;
				// rotate u and v
				if( B_ROT )
				{
					u -= .5;
					v -= .5;
					REAL	u_rot =   u * cos_rot + v * sin_rot;
					REAL	v_rot = - u * sin_rot + v * cos_rot;
					u = u_rot + .5;
					v = v_rot + .5;
				}

				REAL vec[3];			
				if( _b_ellipse )
					_img->get_valid_gradient_from_uv_ellipse(	vec, u, v, _r[0], _r[1], _ellipse_nb,	_b_clamped_ui, _s_compo, _b_linear_ui );
				else
					_img->get_valid_gradient_from_uv(			vec, u, v,								_b_clamped_ui, _s_compo, _b_linear_ui );
			
				if( B_ROT )
				{
					REAL	r0 = vec[0] * cos_rot - vec[1] * sin_rot;
					REAL	r1 = vec[0] * sin_rot + vec[1] * cos_rot;
					vec[0] = r0;
					vec[1] = r1;
				}

				//to regroup to optimize
				if( _b_white_multiply )
					scale_v2( vec, _img->get_valid_value_from_uv( u, v, _b_clamped_ui, _s_compo, _b_linear_ui ) );

				u = vec[0] * _strenght;
				v = vec[1] * _strenght;
				if( _normal != 0. )
				{		
					u += vec[1] * _normal;
					v -= vec[0] * _normal;
				}

				*(dst + i_dst_u)	= *(src + i_dst_u) + u;
				*(dst + i_dst_v)	= *(src + i_dst_v) + v;
				*(dst + _i_dst_axe) = *(src + _i_dst_axe);
			}
			);
		}
		else
*/
		{
			for( ; nb>0; --nb )
			{
				REAL u = *(src + _i_src_u) * over_sca_u - o_u;
				REAL v = *(src + _i_src_v) * over_sca_v - o_v;
#if	AAA_DEBUG()
				if( _isnan(u) || _isnan(v) )
				{
					debug_break();
				}
#endif
				// rotate u and v
				if( B_ROT )
				{
					u -= REAL(.5);
					v -= REAL(.5);
					REAL u_rot =   u * cos_rot + v * sin_rot;
					REAL v_rot = - u * sin_rot + v * cos_rot;
					u = u_rot + REAL(.5);
					v = v_rot + REAL(.5);
				}

				REAL vec[3];		
				if( _b_ellipse )
					_img->get_valid_gradient_from_uv_ellipse(	vec, u, v, _r[0], _r[1], _ellipse_nb,	_b_clamped_ui, _s_compo, _b_linear_ui );
				else
					_img->get_valid_gradient_from_uv(			vec, u, v,								_b_clamped_ui, _s_compo, _b_linear_ui );
			
				if( B_ROT )
				{
					REAL	r0 = vec[0] * cos_rot - vec[1] * sin_rot;
					REAL	r1 = vec[0] * sin_rot + vec[1] * cos_rot;
					vec[0] = r0;
					vec[1] = r1;
				}

				//to regroup to optimize
				if( _b_white_multiply )
					scale_v2( vec, _img->get_valid_value_from_uv( u, v, _b_clamped_ui, _s_compo, _b_linear_ui ) );

				u = vec[0] * _strenght;
				v = vec[1] * _strenght;
				if( _normal != 0. )
				{		
					u += vec[1] * _normal;
					v -= vec[0] * _normal;
				}

				//if( B_ROT )
				//{
				//	//hackhack monaco is like _i_dst_axe==1 but why force this special case
				//	dst[2] = src[2] + u;
				//	dst[0] = src[0] + v;
				//	dst[1] = src[1];
				//}
				//else

				*(dst + i_dst_u)	= *(src + i_dst_u) + u;
				*(dst + i_dst_v)	= *(src + i_dst_v) + v;
				*(dst + _i_dst_axe) = *(src + _i_dst_axe);

				//next one
				src += 3;
				dst += 3;
			}
		}
	}
	else if( _b_use_rgb )
	{
/*		if( b_parallel )
		{
			PARALLEL_LIB::parallel_for( 0, nb, [&]( INT32 CONST i ) NOEXCEPT
			{
				REAL* src	=	get_src_pt( 0 ) + 3 * i;
				REAL* dst	=	get_dst_pt( 0 ) + 3 * i;

				REAL u = *(src + _i_src_u) * over_sca_u - o_u;
				REAL v = *(src + _i_src_v) * over_sca_v - o_v;
				// rotate u and v
				if( B_ROT )
				{
					u -= .5;
					v -= .5;
					REAL	u_rot =   u * cos_rot + v * sin_rot;
					REAL	v_rot = - u * sin_rot + v * cos_rot;
					u = u_rot + .5;
					v = v_rot + .5;
				}

				if( _b_ellipse )
					_img->get_valid_color3r_from_uv_ellipse( dst, u, v, _r[0], _r[1], _ellipse_nb, _b_clamped_ui, _b_linear_ui );
				else
					_img->get_valid_color3r_from_uv( dst, u, v, _b_clamped_ui, _b_linear_ui );

				add_scale_v3( dst, src, dst, _strenght );
			}
			);
		}
		else
*/
		{
			for( ; nb>0; --nb )
			{
				REAL u = *(src + _i_src_u) * over_sca_u - o_u;
				REAL v = *(src + _i_src_v) * over_sca_v - o_v;
				// rotate u and v
				if( B_ROT )
				{
					u -= REAL(.5);
					v -= REAL(.5);
					REAL u_rot =   u * cos_rot + v * sin_rot;
					REAL v_rot = - u * sin_rot + v * cos_rot;
					u = u_rot + REAL(.5);
					v = v_rot + REAL(.5);
				}

				if( _b_ellipse )
					_img->get_valid_color3r_from_uv_ellipse( dst, u,v, _r[0], _r[1], _ellipse_nb, _b_clamped_ui, _b_linear_ui );
				else
					_img->get_valid_color3r_from_uv( dst, u,v, _b_clamped_ui, _b_linear_ui );

				add_scale_v3( dst, src, dst, _strenght );

				//next one
				src += 3;
				dst += 3;
			}
		}
	}
	else
	{
/*
		if( b_parallel )
		{
			PARALLEL_LIB::parallel_for( 0, nb, [&]( INT32 CONST i ) NOEXCEPT
			{
				REAL* src	=	get_src_pt( 0 ) + 3 * i;
				REAL* dst	=	get_dst_pt( 0 ) + 3 * i;

				cpy_v3( dst, src );

				REAL u = *(src + _i_src_u) * over_sca_u - o_u;
				REAL v = *(src + _i_src_v) * over_sca_v - o_v;
				// rotate u and v
				if( B_ROT )
				{
					u -= .5;
					v -= .5;
					REAL	u_rot =   u * cos_rot + v * sin_rot;
					REAL	v_rot = - u * sin_rot + v * cos_rot;
					u = u_rot + .5;
					v = v_rot + .5;
				}

				REAL r;
				if( _b_ellipse )
					r = _img->get_valid_value_from_uv_ellipse( u, v, _r[0], _r[1], _ellipse_nb, _b_clamped_ui, _s_compo, _b_linear_ui );
				else
					r = _img->get_valid_value_from_uv( u, v, _b_clamped_ui, _s_compo, _b_linear_ui );

				if( _b_white_multiply )
					r *= r;

				r = r * _strenght + _offset;
				*(dst + _i_dst_axe) += r;
			}
			);
		}
		else
*/
		{
			for( ; nb>0; --nb )
			{
				cpy_v3( dst, src );

				REAL u = *(src + _i_src_u) * over_sca_u - o_u;
				REAL v = *(src + _i_src_v) * over_sca_v - o_v;
				// rotate u and v
				if( B_ROT )
				{
					u -= REAL(.5);
					v -= REAL(.5);
					REAL u_rot =   u * cos_rot + v * sin_rot;
					REAL v_rot = - u * sin_rot + v * cos_rot;
					u = u_rot + REAL(.5);
					v = v_rot + REAL(.5);
				}

				REAL r;
				if( _b_ellipse )
					r = _img->get_valid_value_from_uv_ellipse( u, v, _r[0], _r[1], _ellipse_nb, _b_clamped_ui, _s_compo, _b_linear_ui );
				else
					r = _img->get_valid_value_from_uv( u, v, _b_clamped_ui, _s_compo, _b_linear_ui );

				if( _b_white_multiply )
					r *= r;

				r = r * _strenght + _offset;
				*(dst + _i_dst_axe) += r;

				//next one
				src += 3;
				dst += 3;
			}
		}
	}
}

void	c_def_image::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	if( !_img )
	{
		err_print( "%() : no image source", __FUNCTION__ );
		return;	//todo un peu rapide non
	}
	if( !_img->is_valid() )
	{
		err_print( "%() : image source not valid", __FUNCTION__ );
		return;	//todo un peu rapide non
	}

	if( _rotation != 0. )
		apply_private< true >	( src, dst, nb );
	else
		apply_private< false >	( src, dst, nb );
}
