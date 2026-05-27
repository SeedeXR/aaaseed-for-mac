#include "bdd_img_substrate.h"
#include "image/bind_img_2d.h"
#ifndef AAA_BITMAP_CONVERT_H
#	include "image/convert/bitmap_convert.h"
#endif
#include "draw/shape.h"
#include "draw/box.h"
#include "draw/model.h"
#include "math/rand.h"


static INT32 CONST NB_CRACKS_MAX = 16;

static	c_rand_lin	crack_rnd;
//todo
static INT32		dim_x;
static INT32		dim_y;
static REAL			lim_x;
static REAL			lim_y;
static FP32		color_draw[4];

static	UINT32*		grid;
static	c_img_2d*		img;

class c_crack : public c_obj
{
private:
	REAL	_x, _y;
	REAL	_deg;    // direction of travel in degrees
	REAL	_rad;
	REAL	_cos, _sin;
public:
	FINLINE void set_angle_deg( REAL CONST deg )
	{
		_deg = deg;	//	%360;
		_rad = REAL(DEG_TO_RAD( deg ));
		GET_SIN_COS_DEG( _sin, _cos, deg );
	}

	FINLINE void set( INT32 CONST x, INT32 CONST y, REAL CONST deg )
	{
		_x = REAL(x);
		_y = REAL(y);
		set_angle_deg( deg );
	}

	FINLINE void start_crack( INT32 CONST x, INT32 CONST y, REAL CONST deg )
	{
		set( x,y, deg );
		_x += REAL(0.61) * _cos;
		_y += REAL(0.61) * _sin;
	}

	c_crack()
	{
		set( 0, 0, crack_rnd.get_fp32_max(360) );
	}

	void find_start()
	{
		// pick random point
		INT32	px = 0;
		INT32	py = 0;

		// shift until crack is found
		bool	b_found = false;
		INT32	count = 0;
		do
		{
			px = INT32( crack_rnd.get_fp32_max(lim_x) );
			py = INT32( crack_rnd.get_fp32_max(lim_y) );

			if( grid[ py*dim_x + px ] < 10000 )
				b_found = true;
			if( ++count > 100000 )
				break;
		}
		while( !b_found );

		if( b_found )
		{
			// start crack
			int a = grid[ py * dim_x + px ];

			if( crack_rnd.get_fp32_01() < .5 )
				a -= 90;	// +int(random(-2, 2.1));
			else
				a += 90;	// +int(random(-2, 2.1));

			start_crack( px, py, REAL(a) );
		}
		else
		{
			//println("count: "+count);
		}
	}

	void move()
	{
//		float ran = 5 * abs( (_x + _y - dim_x) / (dim_x + dim_y) );
//		_t += random( -ran, ran );
		// continue cracking
		_x += REAL(0.42) * _cos;
		_y += REAL(0.42) * _sin;

		// bound check
		//REAL z = 0.33;
		//INT32 cx = INT32(_x + (crack_rnd.get_ufloat()*2-1)*z );  // add fuzz
		//INT32 cy = INT32(_y + (crack_rnd.get_ufloat()*2-1)*z );
		INT32 cx = INT32( _x );
		INT32 cy = INT32( _y );
		// draw sand painter
		//regionColor();

		// draw black crack
		//stroke(0, 85);
		//point(x + random(-z, z), y + random(-z, z));

		if( (0 <= cx) && (cx < dim_x) && (0 <= cy) && (cy < dim_y) )
		{
			img->set_xy_color4r( cx, cy, color_draw );

			UINT32 index = cy*dim_x + cx;
			INT32 val = grid[index];
			// safe to check
			if( val>10000 || (ABS(val - _deg) < 5) )
			{
				// continue cracking
				grid[index] = int(_deg);
			}
			else if( ABS( val - _deg) > 2)
			{
				// crack encountered (not self), stop cracking
				find_start();
//				makeCrack();
			}
		}
		else
		{
			// out of bounds, stop cracking
			find_start();
//			makeCrack();
		}
	}
};

FACTORY_CREATE_PROP_V1( c_bdd_img_substrate, bdd_img_substrate, Image Substrate, image_substrate, sub_menu="Image"; );

namespace n_img_substrate
{
	CONSTEXPR INT32 BASE_NB_MAX		=	c_bdd::NO_GEO_PARAM_NB + 8;
	CONSTEXPR INT32 COLOR_NB_MAX	=	10;
	CONSTEXPR INT32 GROUP_NB_MAX	=	1;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_NB_MAX
									+	COLOR_NB_MAX
									+	GROUP_NB_MAX;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_NO_GEO_BASE_PARAMS

		PARAM_DEF_INT32_XY(		size_pixel,			128, 1024,		4, 32*1024	)
		PARAM_DEF_BOOL_ON(		texture_size_min	)
		PARAM_DEF_INT32(		crack_seed_nb,		4, 2,			1, 32*1024	)
		PARAM_DEF_BOOL_OFF(		restart_trig		)
		PARAM_DEF_BOOL_OFF(		image_erase_trig	)
		PARAM_DEF_BOOL_ON(		move				)
		PARAM_DEF_INT32(		move_nb,			4, 8,			1, 1024*1024	)

		PARAM_DEF_GROUP_CLOSED(	Color,		COLOR_NB_MAX	)
			PARAM_DEF_COLOR_RGBGA(			color_back	)
			PARAM_DEF_COLOR_RGBGA_BLACK(	color_draw	)
	};
}

void c_bdd_img_substrate::param_init_pt()
{
	INT32	h = param_init_pt_no_geo();

	param_set_pt_2(	h, _size_pixel_ui		);
	param_set_pt(		h, _b_texture_size_min	);
	param_set_pt(		h, _crack_seed_nb		);
	param_set_pt(		h, _b_restart_trig_ui	);
	param_set_pt(		h, _b_img_erase_trig	);
	param_set_pt(		h, _b_move				);
	param_set_pt(		h, _move_nb				);

	++h;
		param_set_pt_rgbfa(	h, _color_back_ui	);
		param_set_pt_rgbfa(	h, _color_draw_ui	);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_bdd_img_substrate)
,_bind_last(-42)
,_img(nullptr)
,_grid(nullptr)
,_crack(nullptr)
,_hd_crack_a(nullptr)
,_hd_crack_a_cur(nullptr)
,_hd_crack_a_max(nullptr)
,_hd_crack_b(nullptr)
,_hd_crack_b_cur(nullptr)
,_hd_crack_b_max(nullptr)
{
	_size_grid[0] = 0;
	_size_grid[1] = 0;
	param_init_with( n_img_substrate::param, n_img_substrate::PARAM_NB_MAX );
	init();
}

c_bdd_img_substrate::~c_bdd_img_substrate()
{
	IF_FREE_ALIGNED_AND_NULL( _grid );
	dealloc_crack();
}

void	c_bdd_img_substrate::dealloc_crack()
{
	SAFE_DELETE_ARRAY( _crack );
	SAFE_DELETE_ARRAY( _hd_crack_a );
	SAFE_DELETE_ARRAY( _hd_crack_b );

	_hd_crack_a_cur = nullptr;
	_hd_crack_a_max = nullptr;

	_hd_crack_b_cur = nullptr;
	_hd_crack_b_max = nullptr;
}

void	c_bdd_img_substrate::alloc_crack()
{
	if( !_crack )
	{
		_crack		= new c_crack[NB_CRACKS_MAX];
		_hd_crack_a = new p_crack[NB_CRACKS_MAX];		
		_hd_crack_b = new p_crack[NB_CRACKS_MAX];
		if( !_crack || !_hd_crack_a || !_hd_crack_b )
		{
			dealloc_crack();
		}
	}
}

void	c_bdd_img_substrate::init_crack()
{
	alloc_crack();

	if( _crack )
	{
		_hd_crack_a_cur = _hd_crack_a;
		_hd_crack_a_max = _hd_crack_a_cur + NB_CRACKS_MAX;

		_hd_crack_b_cur = _hd_crack_b;
		_hd_crack_b_max = _hd_crack_b_cur + NB_CRACKS_MAX;
	}
}

void	c_bdd_img_substrate::swap_crack()
{
	_hd_crack_a_cur = _hd_crack_b_cur;
	_hd_crack_b_cur = _hd_crack_b;
	SWAP( _hd_crack_a_max, _hd_crack_b_max );
}

void	c_bdd_img_substrate::init()
{
	init_crack();
}

bool	c_bdd_img_substrate::can_implicit()
{
	return true;
}

void	c_bdd_img_substrate::draw()
{
	REAL	size_raw[3];
	c_model::cur->get_size_v3( size_raw );

	REAL size[3];
	uv_to_xyz_v3r( size, size_raw,  c_model::cur->get_axe() );
	draw_box( size );
}

//	this is ok because we work on 4 channel and that default alignment of pixel is 4 bytes
void	c_bdd_img_substrate::erase_img( c_img_2d* img )
{
	img->erase_filename();

/*
	UINT32*	data	= img->get_data_int32();
	INT32	sx		= img->get_size_x();
	INT32	sy		= img->get_size_y();

	UINT32	color;
	UINT32*	pt;

	for( INT32 i=0; i<sx; ++i )
	{
		color = 0xff006f00;	//abgr so it is redish
		pt = data + i;
		for( INT32 j=sy; j>0; --j )
		{
			*pt = color;
			pt += sx;
		}
	}
*/	
	FP32 color_back[4];
	scale_v3_cpy_v4( color_back, _color_back_ui );
	img->fill_rgba( color_back );

	_b_img_erase_trig = false;
}

void	c_bdd_img_substrate::alloc_grid()
{
	if( is_diff_v2( _size_grid, _size_pixel ) )
	{
		INT32 new_size = _size_pixel[0] * _size_pixel[1];
		if( new_size > _size_grid[0]*_size_grid[1] )
			_grid = (UINT32 *)REALLOC_ALIGNED(	_grid,	sizeof(UINT32) * new_size );
		if( _grid )
		{
			cpy_v2( _size_grid,  _size_pixel );
			_b_restart_trig_ui = true;
		}
		else
		{
			err_print( "Can't alloc grid" );
			clear_v2( _size_grid );
		}
	}

}

void	c_bdd_img_substrate::init_grid()
{
	erase_img( _img );

	UINT32 size = _size_grid[0] * _size_grid[1];
	for( UINT32 i=0; i<size; i++ )
	{
		_grid[i] = 10001;
	}
	// make random crack seeds
	for( INT32 k=0; k<_crack_seed_nb; k++ )
	{
		UINT32 i = UINT32( crack_rnd.get_fp32_max( _size_pixel[0] * _size_pixel[1] - .001 )  );
		_grid[i] = INT32( crack_rnd.get_fp32_max(360-.001) );
	}
	//todo very long at beginning : use location made in previous loop
	for( INT32 i = 0; i < NB_CRACKS_MAX; ++i )
	{
		_crack[i].find_start();
	}
}

void	c_bdd_img_substrate::sync_stupid()
{
	dim_x = _size_pixel[0];
	dim_y = _size_pixel[1];

	lim_x = dim_x - REAL(.001);
	lim_y = dim_y - REAL(.001);

	grid = _grid;
	img = _img;
}

void	c_bdd_img_substrate::compute()
{
	if( !_b_move )	{ return; }
	sync_stupid();
	scale_v3_cpy_v4( color_draw, _color_draw_ui );

	for( INT32 pass=_move_nb; pass>0; --pass )
	{
		for( UINT32 i = 0; i < NB_CRACKS_MAX; ++i )
		{
			_crack[i].move();
		}
	}
}

void	c_bdd_img_substrate::update()
{
	bool b_reload = false;
	if(		is_diff_v2( _size_pixel,  _size_pixel_ui )
		||	_bind_last != g_bind_img_2d->get_cur_index()
		)
	{
		_img = g_bind_img_2d->get_img_cur( aaa::PIXEL_FORMAT::RGBA_8, _size_pixel_ui[0], _size_pixel_ui[1], _b_texture_size_min, nullptr, __FUNCTION__ );
		if( _img && _img->get_data_valid_rgb_uint8( __FUNCTION__ ) )
		{
			cpy_v2( _size_pixel,  _size_pixel_ui );
			alloc_grid();
			b_reload = true;
		}
	}

	if( _b_restart_trig_ui )
	{
		sync_stupid();
		init_grid();
		_b_restart_trig_ui = false;
		erase_img(_img);
	}
	else
	{
		if( _b_img_erase_trig )
		{
			erase_img(_img);
		}
	}
	//	_color and _color_u are now sorted in ascending order by _color_u

	compute();
	b_reload = true;	//todo avoid forcing always like here

	if(		b_reload
		||	_bind_last != g_bind_img_2d->get_cur_index()
//		||	_b_force_nearest != _b_force_nearest_ui
		)
	{
		g_bind_img_2d->do_after_a_compute( );
		_bind_last = g_bind_img_2d->get_cur_index();
	}
}
