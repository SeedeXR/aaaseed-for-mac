#include "bdd_deptina.h"
#include "image/bind_img_2d.h"
#include "file/file_csv.h"


FACTORY_CREATE_PROP_V1( c_bdd_deptina, bdd_deptina, Deptina, bdd_deptina, sub_menu = "Special"; );

namespace n_bdd_deptina
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 10 + 81;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32 PARAM_NB	= BASE_PARAM_NB
								+ GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB] =
	{
		PARAM_DEF_BOOL_OFF(				run				)
		PARAM_DEF_BIND_2D_CURRENT_SEL(	src_bind		)
		PARAM_DEF_BIND_2D_CURRENT_SEL(	mask_bind		)
		PARAM_DEF_INT32_POS(			nb_u,			16, 9		)
		PARAM_DEF_INT32_POS(			nb_v,			16, 9		)
		PARAM_DEF_BOOL_OFF(				crop						)
		PARAM_DEF_INT32_POS(			crop_left,		100, 0		)
		PARAM_DEF_INT32_POS(			crop_top,		100, 0		)
		PARAM_DEF_INT32_POS(			crop_right,		100, 0		)
		PARAM_DEF_INT32_POS(			crop_bottom,	100, 0		)

		PARAM_DEF_01_64( color, PARAM_DEF_FP32_ONE )
		PARAM_DEF_FP32_ONE( color_65 )
		PARAM_DEF_FP32_ONE( color_66 )
		PARAM_DEF_FP32_ONE( color_67 )
		PARAM_DEF_FP32_ONE( color_68 )
		PARAM_DEF_FP32_ONE( color_69 )
		PARAM_DEF_FP32_ONE( color_70 )
		PARAM_DEF_FP32_ONE( color_71 )
		PARAM_DEF_FP32_ONE( color_72 )
		PARAM_DEF_FP32_ONE( color_73 )
		PARAM_DEF_FP32_ONE( color_74 )
		PARAM_DEF_FP32_ONE( color_75 )
		PARAM_DEF_FP32_ONE( color_76 )
		PARAM_DEF_FP32_ONE( color_77 )
		PARAM_DEF_FP32_ONE( color_78 )
		PARAM_DEF_FP32_ONE( color_79 )
		PARAM_DEF_FP32_ONE( color_80 )
		PARAM_DEF_FP32_ONE( color_81 )

	};
}

void	c_bdd_deptina::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, get_pt_active()		);
	param_set_pt( h, _bind_src_ui			);
	param_set_pt( h, _bind_mask_ui			);
	param_set_pt( h, _nb_u_ui				);
	param_set_pt( h, _nb_v_ui				);
	param_set_pt( h, _b_crop_ui				);
	param_set_pt( h, _left_ui				);
	param_set_pt( h, _top_ui				);
	param_set_pt( h, _right_ui				);
	param_set_pt( h, _bottom_ui				);

	param_set_pt_n( h, _color_max, MIN( _nb_zone, 81 )		);

	//err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_bdd_deptina )
	,_nb				(	0		)
	,_img_index_last	(	-1		)
	,_nb_u				(	0		)
	,_nb_v				(	0		)
	,_nb_zone			(	0		)
	,_color_max			(	nullptr	)
{
	alloc( 9, 9 );
	param_init_with( n_bdd_deptina::param, n_bdd_deptina::PARAM_NB );
}

c_bdd_deptina::~c_bdd_deptina()
{
	FREE_AND_NULL(_color_max);
}

void	c_bdd_deptina::alloc( UINT32 nb_u, UINT32 nb_v )
{
	FREE_AND_NULL(_color_max);
	_color_max = new FP32[ nb_u * nb_v ];
	_nb_u = nb_u;
	_nb_v = nb_v;
	_nb_zone = _nb_u * _nb_v;
}

void	c_bdd_deptina::update()
{
	if( is_active() )
	{
		INT32 bind_src	= g_bind_img_2d->build_index_from_param( _bind_src_ui );
		INT32 bind_mask	= g_bind_img_2d->build_index_from_param( _bind_mask_ui );

		c_img_2d* img_src	= g_bind_img_2d->get( bind_src );
		c_img_2d* img_mask	= g_bind_img_2d->get_ready( bind_mask );

		if( IS_NULL( img_src ) || IS_NULL( img_mask ) )
		{
			ERR_PRINT_STRING( "Source or Mask images is NULL in %s()", __FUNCTION__ );
			return;
		}

		{
			UINT32	img_index = img_src->get_state_unique();
			if( _img_index_last == img_index )
				return;		// no new image
			_img_index_last = img_index;
		}

		if ( _nb_u_ui != _nb_u || _nb_v_ui != _nb_v)
		{
			alloc( _nb_u_ui, _nb_v_ui );
		}

		// Clear max
		MEMCLEAR( _color_max, sizeof(REAL) * 81 );
		UINT32	size_x = img_src->get_size_x();
		UINT32	size_y = img_src->get_size_y();

		UINT32	sx_begin	= 0;
		UINT32	sx_end		= size_x;
		UINT32	sy_begin	= 0;
		UINT32	sy_end		= size_y;
		UINT32	sx			= size_x;
		UINT32	sy			= size_y;
		if (_b_crop_ui)
		{
			sx_begin	= MIN( _left_ui, size_x );
			sx_end		= MAX0( size_x - _right_ui );
			sy_begin	= MIN( _bottom_ui, size_y );
			sy_end		= MAX0( size_y - _top_ui );
			sx			= sx_end - sx_begin;
			sy			= sy_end - sy_begin;
		}
		for ( auto j = sy_begin; j < sy_end; ++j )
		{
			REAL v = (REAL)( j - sy_begin ) / (REAL)sy;
			for ( auto i = sx_begin; i < sx_end; ++i )
			{
				REAL u = (REAL)( i - sx_begin ) / (REAL)sx;

				FP32	color[4];
				img_mask->get_color4r_from_uv_linear( color, u, v, false );
				
				UINT32	r = UINT32( color[0] * 255 );
				UINT32	g = UINT32( color[1] * 255 );
				UINT32	b = UINT32( color[2] * 255 );
				UINT32	a = UINT32( color[3] * 255 );
				if ( r < _nb_zone )
				{
					if( r > 0 && r < 82 && a == 255 && g == 0 && b == 0 )
					{
						if ( img_src->is_valid() && img_src->is_valid_xy( i, j ) )
						{
							FP32 color2[4];
							img_src->get_color3r_from_xy( color2, i, j );

							if( _color_max[r-1] < color2[0] )
							{
								_color_max[r-1] = color2[0];
							}
						}
					}
					else
					{
					//	DBG_PRINT_STRING("Color not OK : %d, %d, %d, %d", r, g, b, a );
					}
				}
			}
		}
	}
}

void	c_bdd_deptina::draw()
{

}

