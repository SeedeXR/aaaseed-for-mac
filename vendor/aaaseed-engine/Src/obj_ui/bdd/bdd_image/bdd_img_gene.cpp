#include "bdd_img_gene.h"
#include "image/bind_img_2d.h"
#include "image/bind_img_3d.h"
#ifndef AAA_BITMAP_CONVERT_H
#	include "image/convert/bitmap_convert.h"
#endif
#include "draw/box.h"
#include "draw/model.h"


FACTORY_CREATE_PROP_V1( c_bdd_img_gene, bdd_img_gene, Image generator blank, image_generator, sub_menu="Image"; );


namespace n_img_gene
{
	CONSTEXPR INT32 BASE_NB_MAX		=	c_bdd::NO_GEO_PARAM_NB + 8;
	CONSTEXPR INT32 GROUP_NB_MAX	=	0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_NB_MAX
									+	GROUP_NB_MAX;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_NO_GEO_BASE_PARAMS

		PARAM_DEF_BOOL_ON(		tex_3d	)
		PARAM_DEF_BOOL_ON(		sphere	)

		PARAM_DEF_INT32_XYZ(	size_asked,			5, 32,		4, 64*1024	)

		PARAM_DEF_REAL(			min,				.1, 0.,		0., 1.	)
		PARAM_DEF_REAL(			max,				.9, 1.,		0., 1.	)

		PARAM_DEF_BOOL_ON(		texture_size_min	)
	};
}

void c_bdd_img_gene::param_init_pt()
{
	INT32	h = param_init_pt_no_geo();

	param_set_pt(	h, _b_3d_ui				);
	param_set_pt(	h, _b_sphere_ui			);

	param_set_pt_3(	h, _size_pixel_ui		);
	param_set_pt_2(	h, _min_max_ui			);
	param_set_pt(	h, _b_texture_size_min	);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_bdd_img_gene)
{
	param_init_with( n_img_gene::param, n_img_gene::PARAM_NB_MAX);
	init();
}

c_bdd_img_gene::~c_bdd_img_gene()
{
}

void c_bdd_img_gene::init()
{
}

bool	c_bdd_img_gene::can_implicit()
{
	return true;
}

void	c_bdd_img_gene::draw()
{
	INT32	axe = c_model::cur->get_axe();
	REAL size_raw[3];
	REAL size[3];
	c_model::cur->get_size_v3(size_raw);
	uv_to_xyz_v3r( size, size_raw, axe );
	draw_box( size );
}

//	this is ok because we work on 4 channel and that default alignment of pixel is 4 bytes
void c_bdd_img_gene::compute_2d( c_img_2d* img )
{
	UINT8* data8 = img->get_data_valid_rgb_uint8( __FUNCTION__ );
	if( !data8 )	{	return;	}

	UINT32*	data	=	(UINT32*)data8;

	INT32	sx		= img->get_size_x();
	INT32	sy		= img->get_size_y();

	img->erase_filename();

	UINT32	color;
	UINT32*	pt;

	for( INT32 i=0; i<sx; ++i )
	{
		color = 0xffffff00;	//abgr so it is cyan
		pt = data + i;
		for( INT32 j=sy; j>0; --j )
		{
			*pt = color;
			pt += sx;
		}
	}
}

//todo could be optimized
void c_bdd_img_gene::compute_3d( c_img_3d* img )
{
	UINT8* data8 = img->get_data_valid_rgb_uint8( __FUNCTION__ );
	if( !data8 )
		return;

	INT32	sx		= img->get_size_x();
	INT32	sy		= img->get_size_y();
	INT32	sz		= img->get_size_z();

	img->erase_filename();

	REAL min = _min_max_ui[0];
	REAL max = _min_max_ui[1];

	if( _b_sphere_ui )
	{
		min *= min;
		max *= max;
	}

	--data8;
	for( INT32 iz=0; iz<sz; ++iz )
	{
		REAL vz = ABS( REAL(iz)/sz - REAL(.5) ) * REAL(2);
		for( INT32 iy=0; iy<sy; ++iy )
		{
			REAL vy = ABS( REAL(iy)/sy - REAL(.5) ) * REAL(2);
			for( INT32 ix=0; ix<sx; ++ix )
			{
				REAL vx = ABS( REAL(ix)/sz - REAL(.5) ) * REAL(2);
				if( _b_sphere_ui )			
					*++data8 = INSIDE( vx*vx + vy*vy + vz*vz, min , max ) ? 0xff : 0;
				else
					*++data8 = ( aaa::MAX(vx,vy,vz) > min ) && ( aaa::MAX(vx,vy,vz) < max ) ? 0xff : 0;
			}
		}
	}
}

void	c_bdd_img_gene::update()
{
	bool b_reload = false;
	if( _b_3d_ui )
	{
		if(		is_diff_v3( _size_pixel,  _size_pixel_ui )
			||	_bind_last_3d != g_bind_img_3d->get_cur_index()
			)
		{
			c_img_3d* img = g_bind_img_3d->get_img_cur( aaa::PIXEL_FORMAT::R_8, _size_pixel_ui[0], _size_pixel_ui[1], _size_pixel_ui[2], _b_texture_size_min, nullptr, __FUNCTION__ );

			if( img && img->get_data_valid_rgb_uint8( __FUNCTION__ ) )
			{
				cpy_v3( _size_pixel,  _size_pixel_ui );
				//	_color and _color_u are now sorted in ascending order by _color_u			
				compute_3d( img );
				b_reload = true;
			}
		}
		if(		b_reload
			||	_bind_last_3d != g_bind_img_3d->get_cur_index()
	//		||	_b_force_nearest != _b_force_nearest_ui
			)
		{
			g_bind_img_3d->do_after_a_compute( );
			_bind_last_3d = g_bind_img_3d->get_cur_index();
		}
	}
	else
	{
		if(		is_diff_v2( _size_pixel,  _size_pixel_ui )
			||	_bind_last_2d != g_bind_img_2d->get_cur_index()
			)
		{
			c_img_2d* img = g_bind_img_2d->get_img_cur( aaa::PIXEL_FORMAT::RGBA_8, _size_pixel_ui[0], _size_pixel_ui[1], _b_texture_size_min, nullptr, __FUNCTION__ );

			if( img && img->get_data_valid_rgb_uint8( __FUNCTION__ ) )
			{
				cpy_v2( _size_pixel,  _size_pixel_ui );
				//	_color and _color_u are now sorted in ascending order by _color_u			
				compute_2d( img );
				b_reload = true;
			}
		}
		if(		b_reload
			||	_bind_last_2d != g_bind_img_2d->get_cur_index()
	//		||	_b_force_nearest != _b_force_nearest_ui
			)
		{
			g_bind_img_2d->do_after_a_compute( );
			_bind_last_2d = g_bind_img_2d->get_cur_index();
		}
	}
}
