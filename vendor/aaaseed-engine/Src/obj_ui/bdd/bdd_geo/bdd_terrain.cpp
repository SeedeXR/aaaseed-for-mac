#include "aaa_def.h"

#include "obj_ui/bdd/bdd_geo/bdd_terrain.h"
#include "draw/model.h"
#include "gol/gol_tex.h"
#include "gol/gol_light.h"
#include "gol/gol_draw.h"
#include "gol/gol_color.h"
#include "gol/gol_matrix.h"
#include "draw/mat.h"
#include "math/rand.h"
#include "draw/render.h"
#include "ui/flatland.h"
#include "math/noisturb.h"

FACTORY_CREATE_PROP_V1( c_bdd_terrain, bdd_terrain, Terrain, bdd_terrain, sub_menu="Procedural"; );


CONSTEXPR	INT32	R	= 0;
CONSTEXPR	INT32	G	= 1;
CONSTEXPR	INT32	B	= 2;

CONSTEXPR	REAL	TAILLE	 =4;

CONSTEXPR	INT32	X	= 0;
CONSTEXPR	INT32	Y	= 1;
CONSTEXPR	INT32	Z	= 2;
CONSTEXPR	INT32	XYZ	= 3;

struct	wave
{
	REAL	pos[XYZ];
	REAL	hau;
	REAL	lon;
};

static	struct	wave 	wave1	=
{
	{ -TAILLE*2., -TAILLE*2., 0.},
	REAL(TAILLE/400.),
	REAL(3.14/(TAILLE/25.))
};

static	struct	wave 	wave2	=
{
	{ 0., -TAILLE*2., 0.},
	REAL(TAILLE/400.),
	REAL(3.14/(TAILLE/45.))
};

static	FP32 col_vert[3] = { 0., 1., 0.};
static	FP32 col_rouge[3] = { 1., 0., 0.};
static	FP32 col_blanc[3] = { 1., 1., 1.};
static	FP32 col_mer[3] =  { 0., REAL(.4), REAL(.55) };

namespace n_bdd_terrain
{
	CONSTEXPR UINT32 PARAM_BASE_NB_MAX	= 14 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR UINT32 PARAM_NB_MAX		=	PARAM_BASE_NB_MAX;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_POINT_XYZ(		center )
		PARAM_DEF_INT32(			order,				2, 1,	1, PARAM_MAX_INT32 )
		PARAM_DEF_INT32(			rand,				1, 0,	0, 5 )
		PARAM_DEF_REAL_INF(			fractal_dim_min,	1, 2 )
		PARAM_DEF_REAL_INF(			fractal_dim_max,	2, 3 )
		PARAM_DEF_REAL_INF(			fractal_dim_freq,	0, 1 )
		PARAM_DEF_REAL_INF(			sea_level,			0, 1 )
		PARAM_DEF_BOOL_ON(			addition )
//		PARAM_DEF_BOOL_OFF(			psyche )
		PARAM_DEF_BOOL_OFF(			show_random )
		PARAM_DEF_INT32_POS(		seed,				0, 42 )
		PARAM_DEF_REAL_ZERO_ONE(	progression )
		PARAM_DEF_BOOL_OFF(			draw_color )
	};
}

void	c_bdd_terrain::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt_3(	h,	_center_ui );
	param_set_pt(		h,	_order_ui );
	param_set_pt(		h,	s_rand_ui_ );
	param_set_pt(		h,	_dim_min_ui );
	param_set_pt(		h,	_dim_max_ui );
	param_set_pt(		h,	_dim_freq_ui );
	param_set_pt(		h,	_sea_level_ui );
	param_set_pt(		h,	_b_addition_ui );
//	param_set_pt(		h,	b_psyche_ui_ );
	param_set_pt(		h,	_b_show_rnd_ui );
	param_set_pt(		h,	_seed_ui );
	param_set_pt(		h,	_progression_ui );
	param_set_pt(		h,	_b_draw_color	);

	err_param_init_pt(h);
}

void	c_bdd_terrain::init()
{
	_order = 0;
	_n_max = 0;
	_i_max = 0;
	_data = nullptr;
	_b_need_build = true;
	_s_rand = -1;
	_rand_cur = nullptr;
	_b_show_rnd = false;
//	if( factory.is_first() )
//		{
//		terre_main();
//		}
}


void	c_bdd_terrain::alloc()
{
	if( _order != _order_ui )
	{
		_n_max = POW2( _order_ui );
		_i_max = _n_max + 1;
		_data = (struct st_pave *) REALLOC_ALIGNED( _data, sizeof(struct st_pave) * (_i_max * (_i_max + 1)) );
		
		if( !_data )
		{
			_order = 0;
			_n_max = 0;
			_i_max = 0;
			err_print( "Can't alloc data" );
		}
		else
		{
			_order = _order_ui;
			_b_need_build = true;
		}
		dd_cur_ = _n_max*2;
		_b_need_build = true;
	}

	//	rnd = rnd_gauss;
//todo
//	rand_cur_ = new c_rand_gauss;
//	the_draw = terre_draw;
}

void	c_bdd_terrain::dealloc()
{
}

CONSTRUCTOR_CREATE(c_bdd_terrain)
{
	param_init_with( n_bdd_terrain::param, n_bdd_terrain::PARAM_NB_MAX);
	init();
	alloc();
}

c_bdd_terrain::~c_bdd_terrain()
{
	dealloc();
}


void	c_bdd_terrain::update()
{
	c_model::cur->get_size_v3(_size );
	_b_need_build = false;
	alloc();
	switch_rand();
	fill();
	if(		_b_show_rnd		!= _b_show_rnd_ui 
		||	_b_addition		!= _b_addition_ui
		||	_seed			!= _seed_ui
		||	_dim_min		!= _dim_min_ui
		||	_dim_max		!= _dim_max_ui
		||	_dim_freq		!= _dim_freq_ui
		||	_progression	!=	_progression_ui
		)
	{
		_b_need_build	=	true;
		_seed			=	_seed_ui;
		_b_addition		=	_b_addition_ui;
		_dim_min		=	_dim_min_ui;
		_dim_max		=	_dim_max_ui;
		_dim_freq		=	_dim_freq_ui;
		_progression	=	_progression_ui;
	}
	if( _b_need_build )
		build();
	if(		_sea_level != _sea_level_ui ) 
	{
		_b_color_done = false;
		_sea_level = _sea_level_ui;

	}
	make_color( 1 );
	if( !_b_normal_done )
		make_normal();
}

bool	c_bdd_terrain::can_implicit()
{
	return false;
}


void	c_bdd_terrain::fill()
{
	if(	_b_need_build
		|| is_diff_v3( _center, _center_ui )
		|| is_diff_v3( _size_last, _size )
		)
	{
		REAL	pos[3];
		REAL	step = OVER_ONE_AS_REAL( _i_max-1 );
		//INT32	a,b;
		struct st_pave*	p_pave;

		REAL	va[3]		= { _size[0], 0, 0 };
		REAL	vb[3]		= { 0, _size[1], 0 };
		REAL	delta[3]	= { 0, _size[1]*step, 0 };

		DBG_PRINT_STRING("terrain_fill");

		cpy_v3( _center, _center_ui );
		cpy_v3( _size_last, _size );

		for( INT32 a = 0; a < _i_max; ++a )
		{
			cpy_v3( pos, _center );
			add_scale_v3( pos, vb,  -.5 );
			add_scale_v3( pos, va, REAL(a)*step - .5 );

			p_pave = TERRE(a,0);

			for( INT32 b = 0; b < _i_max; ++b )
			{
				cpy_v3( p_pave->pos, pos );
				cpy_v3( p_pave->nor, unit_z_v4fp32 );

				p_pave->z = pos[2];
			
				add_v3( pos, delta );
				++p_pave;
				}
			}
		_b_normal_done = false;
		_b_color_done = false;
		_b_need_build = true;
	}
}

#define	FACTOR_REP	10
void	c_bdd_terrain::make_rnd()
{
	REAL f = _size[2] / _n_max / (FACTOR_REP/4);
	for( INT32 i=_n_max*_n_max*FACTOR_REP; i>0; --i )
	{
		INT32 a = INT32( _rand_cur->get_fp32_max(_i_max) );
		if( a>=0 && a<_i_max )
		{
			INT32 b = INT32( _rand_cur->get_fp32_max(_i_max) );
			if( b>=0 && b<_i_max )
				TERRE(a,b)->z += f;
		}
	}
}

void	c_bdd_terrain::make()
{
//	INT32	a,b;
	INT32	d;
	INT32	dd;
	REAL 	h;
	REAL	factor;
	//INT32	l;
	INT32	min;
	REAL	hau;

	min = INT32( _order * ( REAL(1) - _progression ) );

	DBG_PRINT_STRING( "make_terre") ;

	d = _n_max;
	hau = _size[2] * REAL(.5);
	h = hau/3;
//	bool	b_update_dim;
//	if( _dim_min == _dim_max || _dim_freq == 0. )
	{
		factor = POW_R( REAL(.5), REAL(.5) * (REAL(3)-_dim_min) );
//		b_update_dim = false;
	}
//	else
//		b_update_dim = true;
	TERRE(0,0)->z				= _rand_cur->get_fp32()*hau;	
	TERRE(0,_n_max )->z			= _rand_cur->get_fp32()*hau;	
	TERRE(_n_max,0)->z			= _rand_cur->get_fp32()*hau;	
	TERRE(_n_max,_n_max )->z	= _rand_cur->get_fp32()*hau;

	//	DBG_PRINT_STRING(" Factor = %f", (DOUBLE)factor);
	for( INT32 l = _order; l > min; --l )
	{
		d /= 2;
		dd = 2 * d;

		if( dd==0 )
		{
			err_print( "dd is 0, it should not happen." );
			return;
		}
	
//		if( !b_update_dim )
		{
			h *= factor + _rand_cur->get_fp32() * ( REAL(1) - factor ) * REAL(2.2);
		}
		//		printf(" %d delta I -> %f", d, h);
		for( INT32 a = d; a <= _n_max - d; a += dd )
		{
			for( INT32 b = d; b <= _n_max - d; b += dd )
			{
				struct st_pave*	p = TERRE( a, b );
/*
			if( b_update_dim )
				{
					set_v3r( vec, p->pos[0], p->pos[1], 0 );
					REAL f = turbulence( vec, _dim_freq, 1 );
					factor = POW( .5, .5 * ( 3.-(_dim_min*(1-f)+_dim_max*f) ) );
					h = hau * .333333 * (factor + _rand_cur->get_fp32() * ( 1. - factor ) * 2.2 );
				}
*/
				p->z =	(	TERRE( a - d, b - d )->z
						+	TERRE( a - d, b + d )->z
						+	TERRE( a + d, b - d )->z
						+	TERRE( a + d, b + d )->z	) * REAL(.25) + _rand_cur->get_fp32() * h;
			}
		}
/*		if (b_show_cons)
			{
			GOL::color_black3();
			//clear();
			_b_color_done = false;
			make_color(1);
			draw_low_intermediate(dd );
			}
*/		if( _b_addition )
		{
			for ( INT32 a = 0; a <= _n_max; a += dd )
				for ( INT32 b = 0; b <= _n_max; b += dd )
					TERRE( a, b )->z += _rand_cur->get_fp32() * h;
		}
		h *= factor + _rand_cur->get_fp32() * ( REAL(1) - factor ) * REAL(2.2) ;
		//		DBG_PRINT_STRING("\t delta II -> %f", (DOUBLE)h);
		for( INT32 a = d; a <= _n_max - d; a += dd )
		{
			TERRE(a,0)->z =
				( TERRE( a+d, 0 )->z
				+ TERRE( a-d, 0 )->z
				+ TERRE( a, d )->z )
				/ REAL(3) + _rand_cur->get_fp32() * h;
			TERRE(a,_n_max )->z =
				( TERRE( a+d, _n_max )->z
				+ TERRE( a-d, _n_max )->z
				+ TERRE( a, _n_max-d )->z )
				/ REAL(3) + _rand_cur->get_fp32() * h;
			TERRE(0,a)->z =
				( TERRE( 0, a+d )->z
				+ TERRE( 0, a-d )->z
				+ TERRE( d, a )->z )
				/ REAL(3) + _rand_cur->get_fp32() * h;
			TERRE(_n_max,a)->z =
				( TERRE( _n_max, a+d )->z
				+ TERRE( _n_max, a-d )->z
				+ TERRE( _n_max-d, a )->z )
				/ REAL(3) + _rand_cur->get_fp32() * h;
		}
		for ( INT32 a = d; a <= _n_max - d; a += dd )
		{
			for ( INT32 b = dd; b <= _n_max - d; b += dd )
			{
				TERRE( a, b )->z =
					( TERRE( a, b-d )->z
					+ TERRE( a, b+d )->z
					+ TERRE( a-d, b )->z
					+ TERRE( a+d, b )->z )
					* REAL(.25) + _rand_cur->get_fp32() * h;
			}
		}
		for ( INT32 a = dd; a <= _n_max - d; a += dd )
		{
			for ( INT32 b = d; b <= _n_max - d; b += dd )
			{
				TERRE(a,b)->z =
					( TERRE( a, b-d )->z
					+ TERRE( a, b+d )->z
					+ TERRE( a-d, b )->z
					+ TERRE( a+d, b )->z )
					* REAL(.25) + _rand_cur->get_fp32() * h;
			}
		}
		if( _b_addition )
		{
			for ( INT32 a = 0; a <= _n_max; a += dd )
				for ( INT32 b = 0; b <= _n_max; b += dd )
					TERRE( a, b)->z += _rand_cur->get_fp32() * h;
			for ( INT32 a = d; a <= _n_max - d; a += dd )
				for ( INT32 b = d; b <= _n_max - d; b += dd )
					TERRE( a, b)->z += _rand_cur->get_fp32() * h;
		}
	}
	dd_cur_ = dd;
	_b_color_done = false;
}

void	c_bdd_terrain::switch_rand()
{
	if( _s_rand != s_rand_ui_ )
		{
		_s_rand = s_rand_ui_;
		_b_need_build = true;
		delete _rand_cur;
		switch( _s_rand )
			{
				// todofranz dealloc
			case 0:	_rand_cur = new c_rand_lin;			break;
			case 1:	_rand_cur = new c_rand_gauss_slick;	break;
				//clean
				//			((c_rand_gauss_slick*)rand_cur_ )->set_focus();
			case 2:	_rand_cur = new c_rand_gauss;		break;
			case 3:	_rand_cur = new c_rand_max;			break;
			case 4:	_rand_cur = new c_rand_flip;		break;
			case 5:	_rand_cur = new c_rand_exp;			break;
				//clean
				//			((c_rand_exp*)rand_cur_ )->set_focus();
			}
		}
}

void	c_bdd_terrain::build()
{
	_rand_cur->set_seed( _seed );
	_b_show_rnd = _b_show_rnd_ui;
	if ( _b_show_rnd )
		make_rnd();
	else
		make();
}

void	c_bdd_terrain::find_min_max()
{
	zmax_ = zmin_ = TERRE( 0, 0 )->z;
	for( INT32 a = 0; a < _i_max; ++a )
	{
		struct st_pave*	p_pave = TERRE( a, 0 )-1;
		for( INT32 b = 0; b < _i_max; ++b )
			UPDATE_MIN_MAX_SAFE( zmin_, zmax_, (++p_pave)->z );
	}
}

REAL	c_bdd_terrain::make_wave( REAL pos[XYZ] )
{
	REAL x,y,c1;

	x = pos[X] - wave1.pos[X];
	y = pos[Y] - wave1.pos[Y];
	c1 = fabs( COS_RAD( SQRT(x*x+y*y) * wave1.lon) );
	x = pos[X] - wave2.pos[X];
	y = pos[Y] - wave2.pos[Y];
	c1 += fabs ( COS_RAD( SQRT(x*x+y*y) * wave2.lon) );
	c1 *=  pos[Z] * REAL(.1);
	return( c1 + pos[Z] );
	/*	c1 *=  pos[Z];
	return( c1 );
	*/
}

void	c_bdd_terrain::make_color( INT32 f_z )
{
	if( _b_color_done )
		return;

	struct st_pave*	p_pave;
	REAL			z;
	FP32			c1;

	DBG_PRINT_STRING("c_bdd_terrain::make_color");
	find_min_max();
	for( INT32 a = 0; a < _i_max; ++a )
	{
		p_pave = TERRE( a, 0 );
		for( INT32 b = 0; b < _i_max; ++b )
		{
			z = p_pave->z;
			if( z >= _sea_level )
			{
				p_pave->pos[Z] = z;
				z /= zmax_;
				p_pave->col[R] = REAL(.3) + z * REAL(.7);
				p_pave->col[G] = REAL(.3) + z * REAL(.7);
				p_pave->col[B] = z;
			}
			else
			{
				c1 = REAL(1) - z / zmin_ * REAL(.75);
				if( f_z )
				{
					if ( z < zmin_*.5 )
						z = zmin_-z;
					z -= _sea_level; 
					p_pave->pos[Z] = z / (_size[2]*5);
					p_pave->pos[Z] = make_wave( p_pave->pos );
				}
				scale_v3( p_pave->col, col_mer, c1 );
			}
			++p_pave;
		}
	}
	_b_color_done = true;
	_b_normal_done = false;
}

void	c_bdd_terrain::make_normal()
{
	if( _b_normal_done )
		return;

	DBG_PRINT_STRING( "c_bdd_terrain::make_normal" );
	for( INT32 a = _i_max - 1; a > 0; --a )
	{
		struct st_pave*	p_pave1 = TERRE( a,		_i_max-1		);
		struct st_pave*	p_pave2 = TERRE( a-1,	_i_max-1	);
		struct st_pave*	p_pave3 = TERRE( a+1,	_i_max-1	);
		for( INT32 b = _i_max - 1; b > 0; --b )
		{
			normal_of_4_point_v3r( p_pave1->nor, p_pave3->pos, p_pave2->pos, (p_pave1-1)->pos, (p_pave1+1)->pos );
			--p_pave1;
			--p_pave2;
			--p_pave3;
		}
	}
	_b_normal_done = true;
}

void	c_bdd_terrain::draw_low_intermediate( INT32 f_clear )
{
	bool	b_nor = is_normal_draw();
	INT32	prim = c_render::get_cur()->get_draw_primitive();
	for( INT32 a = f_clear; a < _i_max; a += f_clear )
	{
		struct st_pave*	p_pave0 = TERRE(	a-f_clear,	0	);
		struct st_pave*	p_pave1 = TERRE(	a,			0	);
		GOL::begin(prim);
			if( _b_draw_color )
			{
				for( INT32 i = _i_max; i>0; i-=f_clear )
				{
					GOL::color3v(	p_pave0->col	);
					if( b_nor )
						GOL::normal3v(	p_pave0->nor	);
					GOL::vertex3v(	p_pave0->pos	);
					p_pave0 += f_clear;

					GOL::color3v(	p_pave1->col	);
					if( b_nor )
						GOL::normal3v(	p_pave1->nor	);
					GOL::vertex3v(	p_pave1->pos	);
					p_pave1 += f_clear;
				}
			}
			else
			{
				for( INT32 i = _i_max; i>0; i-=f_clear )
				{
					if( b_nor )
						GOL::normal3v(	p_pave0->nor	);
					GOL::vertex3v(	p_pave0->pos	);
					p_pave0 += f_clear;

					if( b_nor )
						GOL::normal3v(	p_pave1->nor	);
					GOL::vertex3v(	p_pave1->pos	);
					p_pave1 += f_clear;
				}
			}
		GOL::end();
	}
	//opengl	lmbind(LMODEL, 0);
}

void	c_bdd_terrain::draw_low()
{
	bool	b_nor = is_normal_draw();
	INT32	prim = c_render::get_cur()->get_draw_primitive();
	for( INT32 a = 1; a < _i_max; ++a )
	{
		struct st_pave*	p_pave0 = TERRE( a - 1, 0 );
		struct st_pave*	p_pave1 = TERRE( a, 0 ) ;
		GOL::begin( prim );
			if( _b_draw_color )
			{
				for( INT32 i = _i_max; i>0; --i )
				{
					GOL::color3v(	p_pave0->col	);
					if( b_nor )
						GOL::normal3v(	p_pave0->nor	);
					GOL::vertex3v(	p_pave0->pos	);
					++p_pave0;

					GOL::color3v(	p_pave1->col	);
					if( b_nor )
						GOL::normal3v(	p_pave1->nor	);
					GOL::vertex3v(	p_pave1->pos	);
					++p_pave1;
				}
			}
			else
			{
				for( INT32 i = _i_max; i>0; --i )
				{
					if( b_nor )
						GOL::normal3v(	p_pave0->nor	);
					GOL::vertex3v(	p_pave0->pos	);
					++p_pave0;

					if( b_nor )
						GOL::normal3v(	p_pave1->nor	);
					GOL::vertex3v(	p_pave1->pos	);
					++p_pave1;
				}
			}			
		GOL::end();
	}
}


void	c_bdd_terrain::draw_curves()
{
	if ( c_flatland::b_draw_curve )
	{	//todo these disable are dangerous 
		GOL::set_texture_0D();
		//		GOL::matrix::push();
		//		rendering_set_ortho_with_secu_margin( 100, 100);
		//		c_rand::draw_in_rect(( 0, 0, 100, 100);
		c_rand::draw_in_rect( -4., -4., 8., 8.);
		//		GOL::matrix::pop();
	}
}

void	c_bdd_terrain::draw()
{
//INT32	axe;
//REAL	resolution;

//	axe = c_model::cur->get_axe();
//	resolution = c_model::cur->get_resolution();

//	GOL::matrix::push();
//		GOL::translatev( center_ );
//		GOL::scalev( _size );

		GOL::color_material( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
		GOL::enable_color_material();

			GOL::matrix::push();
				GOL::matrix::rotate_x_deg( -90.);
		//		GOL::translate(-TAILLE/2., -TAILLE/2., 0);
				if ( _progression == 1. )
					draw_low();
				else
					draw_low_intermediate( dd_cur_ );
			GOL::matrix::pop();
			draw_curves();

		GOL::disable_color_material();
		c_materials::get_cur()->reset();
//	GOL::matrix::pop();
}

INT32	c_bdd_terrain::get_point_nb()								{	return _i_max * _i_max ;			}
REAL*	c_bdd_terrain::get_point_pt( INT32 CONST index )
{
	if( IS_INDEX_VALID( index, _i_max * _i_max) )
	{
		INT32	iv = index / _i_max;
		INT32	iu = index - iv * _i_max;
		struct st_pave*	p_pave = TERRE( iu, iv );
		return p_pave->pos;
	}
	return nullptr;
}

