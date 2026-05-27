#include "draw/face.h"
#include "draw/render.h"
#include "gol.h"
#include "infrastructure/layer/layer.h"

/*
void	face_draw( REAL size, c_img_2d* image )
{
	REAL   u, v;
	u = v = size;
	if( image )
		{
		REAL tmp;
		tmp = MAX( image->size_x, image->size_y );
		u *= image->size_x/tmp;
		v *= image->size_y/tmp;
		}
	draw_quad_with_axe_fn(	u,	v,	0,	2, FALSE );
}
*/

void	draw_quad_fn( REAL sx, REAL sy, REAL z )
{
	GOL::begin( c_render::get_cur()->get_draw_primitive() );
		if( c_layer::get_cur()->is_normal_draw() )
		{
			if( sx * sy >= 0 )
				GOL::normal3v( unit_z_v3f32 );
			else
				GOL::normal3v_neg( unit_z_v3f32 );
		}
		if( c_layer::get_cur()->is_need_uv() )
		{
			GOL::texcoord2v(	unit_y_v3f32	);
			GOL::vertex3(	-sx	,	 sy	,	z	);
			GOL::texcoord2v(	zero_v3f32		);
			GOL::vertex3(	-sx	,	-sy	,	z	);
			GOL::texcoord2v(	 one_v3f32		);
			GOL::vertex3(	 sx	,	 sy	,	z	);
			GOL::texcoord2v(	 unit_x_v3f32	);
			GOL::vertex3(	 sx	,	-sy	,	z	);
		}
		else
		{
			GOL::vertex3(	-sx	,	 sy	,	z	);
			GOL::vertex3(	-sx	,	-sy	,	z	);
			GOL::vertex3(	 sx	,	 sy	,	z	);
			GOL::vertex3(	 sx	,	-sy	,	z	);
		}
	GOL::end();
}
