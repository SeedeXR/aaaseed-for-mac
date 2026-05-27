#include "bdd_miguel.h"
#include "obj_ui/deformer/def_node.h"
#include "draw/model.h"
#include "draw/render.h"
#include "math/rand.h"
#include "time/aaa_time.h"
#include "draw/color.h"
#include "infrastructure/data/datagrid.h"
#include "obj_ui/bdd/bdd_geo/bddtex2d.h"
#include "ui/alphabet.h"
#ifndef AAA_OURTRUETYPE_H
#	include "truetype/ourtruetype.h"
#endif
#include "gol/gol_tex.h"
#include "gol/gol_matrix.h"
#include "gol/gol_draw.h"

//haut en tombant 16,64
//1880 haut
//1756	au cercle


FINLINE void	draw_link( REAL* a, REAL *b, REAL size, REAL size_ratio, REAL u_start, REAL u_stop)
{
	REAL	pt[3];
	REAL	normal[3];
	//REAL	norm;

	pt[1] = 0;

	normal[0] = a[2]-b[2];
	normal[1] = 0.;	
	normal[2] = b[0]-a[0];

	normalize_v2( normal[0], normal[2] );
	size *= .5;
	normal[0] *= size;
	normal[2] *= size;

	GOL::begin( c_render::get_cur()->get_draw_primitive());
		pt[0] = a[0] - normal[0]*size_ratio;
		pt[2] = a[2] - normal[2]*size_ratio;
		GOL::texcoord2( u_start, 1);
		GOL::vertex3v( pt);

		pt[0] = a[0] + normal[0]*size_ratio;
		pt[2] = a[2] + normal[2]*size_ratio;
		GOL::texcoord2( u_start, 0);
		GOL::vertex3v( pt);


		pt[0] = b[0] + normal[0];
		pt[2] = b[2] + normal[2];
		GOL::texcoord2( u_stop, 0);
		GOL::vertex3v( pt);

		pt[0] = b[0] - normal[0];
		pt[2] = b[2] - normal[2];
		GOL::texcoord2( u_stop, 1);
		GOL::vertex3v( pt);

		//	tmp += miguel_rand_lin.get_ufloat()*5.;
		//	GOL::texcoord2( tmp, 1);
	GOL::end();

}

FACTORY_CREATE_PROP_V1( c_bdd_miguel_2003nb, bdd_miguel_2003nb, Bdd Nuit Blanche 2003, bdd_miguel_2003nb, sub_menu="Special"; sel0="Avec Miguel 2003nb"; );

c_bdd_miguel_2003nb*	bdd_miguel_2003nb_cur = nullptr;

CONST	INT32	BDD_MIGUEL_POINT_NB	 = 4096;

namespace n_bdd_miguel
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 18 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS

		PARAM_DEF_INT32(		point_nb,			0, 1,		0, BDD_MIGUEL_POINT_NB	)

		PARAM_DEF_POINT_XYZ(	center	)
		PARAM_DEF_REAL_ONE(		radius_int	)
		PARAM_DEF_REAL_ONE(		radius_ext	)
		PARAM_DEF_GAIN(			gain		)
		PARAM_DEF_BIAS(			bias		)

		PARAM_DEF_REAL_ZERO(	speed_min	)
		PARAM_DEF_REAL_ONE(		speed_max	)
		PARAM_DEF_GAIN(			speed_gain	)
		PARAM_DEF_BIAS(			speed_bias	)

		PARAM_DEF_INT32(		font_type,			0, 2,		0, 	3	)
		PARAM_DEF_INT32(		font,				0, 1,		0, aaa::font::FONT_MAX_NB-1	)
		PARAM_DEF_REAL_ONE(		text_size			)
		PARAM_DEF_REAL_ONE(		text_offset_vert	)
	
		PARAM_DEF_REAL_ONE(		text_rot_speed_y	)
		PARAM_DEF_REAL_ONE(		text_rot_speed_z	)
	};
}

void	c_bdd_miguel_2003nb::param_init_pt()
{
	INT32	h = param_init_pt_geo();	

	param_set_pt( h, point_nb_to_draw);

	param_set_pt_3( h, center );

	param_set_pt( h, radius_int);
	param_set_pt( h, radius_ext);
	param_set_pt( h, gain);
	param_set_pt( h, bias);

	param_set_pt( h, speed_min);
	param_set_pt( h, speed_max);
	param_set_pt( h, speed_gain);
	param_set_pt( h, speed_bias);

	param_set_pt( h, s_type);
	param_set_pt( h, s_font);
	param_set_pt( h, text_size);
	param_set_pt( h, text_offset_vert);
	param_set_pt( h, text_rot_speed_y);
	param_set_pt( h, text_rot_speed_z);

	err_param_init_pt(h);
}

void	c_bdd_miguel_2003nb::init()
{
	param_init_with( n_bdd_miguel::param, n_bdd_miguel::PARAM_NB_MAX ); // bdd_miguel_param, BDD_MIGUEL_PARAM_NB_MAX);
}

void	c_bdd_miguel_2003nb::dealloc()
{
	FREE_AND_NULL( points );
	FREE_AND_NULL( points_def );
	FREE_AND_NULL( angles );
}

void	c_bdd_miguel_2003nb::alloc()
{
	points =		(REAL*) MALLOC( sizeof(REAL)*3 *BDD_MIGUEL_POINT_NB );
	points_def =	(REAL*) MALLOC( sizeof(REAL)*3 *BDD_MIGUEL_POINT_NB );
	angles =		(REAL*) MALLOC( sizeof(REAL)*   BDD_MIGUEL_POINT_NB );
	point_nb = BDD_MIGUEL_POINT_NB;
}

CONSTRUCTOR_CREATE(c_bdd_miguel_2003nb)
{
	init();
	alloc();
	p_datagrid = c_bdd_datagrid::create_obj();
	p_datagrid->set_root( this );
	p_datagrid->set_data_filename( "nom1.csv" );
	p_datagrid->read( __FUNCTION__ );

	obj_get( p_tex=nullptr );	//todo should be this ? use obj_ui/root ?
}

c_bdd_miguel_2003nb::~c_bdd_miguel_2003nb()
{
	dealloc();
}

static	c_rand_lin		miguel_rand_lin;

void	c_bdd_miguel_2003nb::build_one(INT32 index)
{
	REAL*	pt;
	REAL	angle;
	REAL	s;
	REAL	c;
	REAL	radius;
	REAL	speed;

	pt = points + index*3;
	
	speed = miguel_rand_lin.get_fp32_01();
	if ( speed_gain != .5 )
		gain_with_factor( speed, speed_gain_fac);
	if ( speed_bias != .5 )
		bias_with_factor( speed, speed_bias_fac);
	speed = interpolate( speed_min, speed_max, speed );
	angle = REAL(index) / REAL(point_nb_to_draw) + REAL(aaa::time::get()) * speed;

	*(angles+index) = angle;
	GET_SIN_COS_TURN( s, c, angle);

	radius = miguel_rand_lin.get_fp32_01();
	if ( gain != .5 )
		gain_with_factor( radius, gain_fac);
	if ( bias != .5 )
		bias_with_factor( radius, bias_fac);

	radius *= radius_ext-radius_int;
	radius += radius_int;

	*pt		= center[0] + radius * s;
	*(pt+1) = center[1];
	*(pt+2) = center[2] + radius * c;
}

void	c_bdd_miguel_2003nb::build()
{
	miguel_rand_lin.set_seed( 8888);
	for( INT32 i=0; i<point_nb_to_draw; ++i )
		build_one( i);
}

void	c_bdd_miguel_2003nb::do_deform()
{
//hack don't work if using object don't call update
	auto def = c_def_node::get_cur();
	if( def->is_deforming() )
	{
		def->apply( points_def, points, point_nb);	//todoq symetrize this special case
		points_to_draw = points_def;
	}
	else
		points_to_draw = points;
}

void	c_bdd_miguel_2003nb::update()
{
	gain_fac = gain_factor( gain);
	bias_fac = bias_factor( bias);

	speed_gain_fac = gain_factor( speed_gain);
	speed_bias_fac = bias_factor( speed_bias);

	build();
	do_deform();

	p_tex->set_type( s_type);
	p_tex->set_axe( c_multiple::cur ? c_multiple::cur->get_axe() : 1 );	//todo remove ?
	p_tex->set_font( s_font);
	p_tex->set_align_hori( .5);

	bdd_miguel_2003nb_cur = this;
}

void	c_bdd_miguel_2003nb::draw_multiple()
{
	REAL*	pt;
	REAL	a[3];
	REAL	size[3];
	//INT32	i;
	REAL	rot_z;
	INT32	s_siret;
	REAL	tmp;

//	GOL::set_texture_2D();
	a[1] = 0;
	size[1] = 1;

	auto def = c_def_node::get_cur();
	if( def->is_deforming() )
		def->set_deforming( false );
	else
		def = nullptr;	

	c_multiple::cur->set_nb( point_nb);
	pt = points_to_draw;
	for( INT32 i=point_nb_to_draw; i>0; --i )
	{
		c_multiple::cur->set_index( i );

		a[0] = pt[0];
		a[2] = pt[2];

		size[0] = c_multiple::cur->get_size(0) * pt[1];
		size[2] = c_multiple::cur->get_size(1) * pt[1];		
		c_multiple::cur->align_then_draw( a, size );

		GOL::disable_texture();
//		printf( "%s\t%s\n",	p_datagrid->get_str(0,i), p_datagrid->get_str(1,i));

		REAL o = REAL(aaa::time::get()) * text_rot_speed_z;
		rot_z = FMOD( miguel_rand_lin.get_fp32_01() + o );
		tmp = REAL(i) / REAL(point_nb_to_draw) + o;
		miguel_rand_lin.set_seed( 8888 + i + INT32(tmp)*32553 );

		s_siret = INT32( miguel_rand_lin.get_fp32_max(4500) );

		if( rot_z < .5 )
			p_tex->set_text( p_datagrid->get_str( 0 ,s_siret) );
		else
		{
			p_tex->set_text( p_datagrid->get_str( 1 ,s_siret) );
			rot_z += .5;
		}
		size[0] = text_size * pt[1];
		size[2] = text_size * pt[1];
		p_tex->update();

		GOL::matrix::push();
			GOL::matrix::translate3v( a );
			GOL::matrix::rotate_y( i * REAL(.12478) + REAL(aaa::time::get()) * text_rot_speed_y );
			GOL::matrix::rotate_z( rot_z - REAL(.25) );
			GOL::matrix::translate_x( text_offset_vert );
			GOL::matrix::scale3v( size );
			p_tex->draw();
		GOL::matrix::pop();
		GOL::set_texture_2D();

/*		
//		p_tex->align_hori = 0;
		cpy_v3r( p_tex->origin, a);	//because of rotation in bdd_tex ?
		p_tex->update();
		p_tex->draw();

*/		//		multiple_index_next();
		pt += 3;
	}
	if( def )
		def->set_deforming( true );

}

void	c_bdd_miguel_2003nb::draw_single()
{
}

FACTORY_CREATE_PROP_V1( c_bdd_point_network_2003nb, bdd_point_network_2003nb, Network Nuit Blanche 2003, bdd_point_network_2003nb, sub_menu="Special"; );


namespace	n_bdd_point_network
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 16 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 0;

	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS

		PARAM_DEF_INT32_POS(	seed,				7777, 8888	)
		PARAM_DEF_INT32(		nb,					0, 1,		0, BDD_POINT_NETWORK_LINK_NB	)
	//	{	nullptr,	PARAM_REAL,	"range",			0, 1,			PARAM_MIN_REAL, PARAM_MAX_REAL,	nullptr, nullptr },
		PARAM_DEF_REAL_ZERO(	refresh_speed	)

		PARAM_DEF_REAL_ONE(		connect_angle_max	)
		PARAM_DEF_REAL_ZERO(	connect_dist_max	)
		PARAM_DEF_INT32(		connect_try_max,	0, 1,		0, 10000	)
		PARAM_DEF_BOOL_OFF(		connect_use_y )

		PARAM_DEF_REAL_INF(		size_min,			0, .01	)
		PARAM_DEF_REAL_INF(		size_max,			0, .01	)
		PARAM_DEF_GAIN(			size_gain		)
		PARAM_DEF_BIAS(			size_bias		)
		PARAM_DEF_REAL_ONE(		size_ratio		)

		PARAM_DEF_REAL_ONE(		map_min			)
		PARAM_DEF_REAL_ONE(		map_max			)
		PARAM_DEF_REAL_ZERO(	map_speed_min	)
		PARAM_DEF_REAL_ONE(		map_speed_max	)
	};
}

void	c_bdd_point_network_2003nb::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt( h, seed);
	param_set_pt( h, nb);
//	param_set_pt( h, range);
	param_set_pt( h, refresh_speed);

	param_set_pt( h, connect_angle_max);
	param_set_pt( h, connect_dist_max);
	param_set_pt( h, connect_try_max);
	param_set_pt( h, b_connect_use_y);

	param_set_pt( h, size_min);
	param_set_pt( h, size_max);
	param_set_pt( h, size_gain);
	param_set_pt( h, size_bias);
	param_set_pt( h, size_ratio);

	param_set_pt( h, map_min);
	param_set_pt( h, map_max);
	param_set_pt( h, map_speed_min);
	param_set_pt( h, map_speed_max);

	err_param_init_pt(h);
}

void	c_bdd_point_network_2003nb::init()
{
	param_init_with( n_bdd_point_network::param, n_bdd_point_network::PARAM_NB_MAX );
}

CONSTRUCTOR_CREATE(c_bdd_point_network_2003nb)
{
	INT32*	pt_good_index = good_index;
	for( INT32 i = BDD_POINT_NETWORK_LINK_NB; i > 0; --i )
		*pt_good_index++ = -1;
	init();
}

c_bdd_point_network_2003nb::~c_bdd_point_network_2003nb()
{
}

void	c_bdd_point_network_2003nb::update()
{
	size_gain_fac = gain_factor( size_gain);
	size_bias_fac = bias_factor( size_bias);
}

void	c_bdd_point_network_2003nb::draw_multiple()
{
}

void	c_bdd_point_network_2003nb::draw_single()
{
	REAL*	pa;
	REAL*	pb;
	//INT32	i;
	INT32	offset;
	REAL	tmp;
	REAL	speed;
	REAL	size;
	REAL	angle;
	REAL	dangle;
	INT32*	pt_good_index = good_index;
	REAL	time_offset = REAL(aaa::time::get()) * refresh_speed;

	if( !bdd_miguel_2003nb_cur )
	{
		ERR_PRINT_STRING( "No bdd_miguel_cur." );
		return;
	}

	INT32	pt_nb = bdd_miguel_2003nb_cur->point_nb_to_draw;
	REAL*	pt_base = bdd_miguel_2003nb_cur->points_to_draw;
	REAL*	angle_base = bdd_miguel_2003nb_cur->angles;
	REAL	connect_dist_squared = connect_dist_max * connect_dist_max;

	pa = pt_base;
	--pt_good_index;
	for( INT32 i=0; i<nb; ++i )
	{
		++pt_good_index;

/*		tmp = miguel_rand_lin.get_ufloat();�
		size = miguel_rand_lin.get_ufloat();
		if( tmp < .05 )
			GOL::color4( size,size,0, 1 );
		else if( tmp < .20 )
			GOL::color4( 0,size,0, 1 );
		else if( tmp < .55 )
			GOL::color4( 0,0,size, 1 );
		else
			GOL::color4( size,size,size, 1 );
*/
		tmp = REAL(i)/REAL(nb) + time_offset;
		miguel_rand_lin.set_seed( seed + i + INT32(tmp) );

//		offset = INT32( miguel_rand_lin.get_ufloat() * (pt_nb-1));
//		if( offset < 0 || offset >= pt_nb )
//			debug_break( "bdd_miguerl offset trashed");
		offset = IMOD( i, pt_nb );
		angle = *(angle_base + offset);
		pa = pt_base + offset * 3;
		if( connect_dist_max == 0. )
		{
			pb = pt_base + INT32( miguel_rand_lin.get_fp32_max(pt_nb-1) )* 3;
		}
		else
		{
			if( !b_connect_use_y)
				pa[1] = 0;
			offset = *pt_good_index;
			if( offset >= 0 )
			{
				pb = pt_base + offset * 3;
				if( !b_connect_use_y)
					pb[1] = 0;
				if( !is_dist_squared_less_v3r( pa, pb, connect_dist_squared ) )
					pb = nullptr;					
			}
			else
			{
				pb = nullptr;
			}
			if( !pb)
			{
				INT32	try_index = connect_try_max;
				do
				{
					//offset = INT32( miguel_rand_lin.get_ufloat() * (pt_nb-1));
					if( ++offset >= pt_nb )
						offset = 0;
					dangle = angle - *(angle_base + offset);
					if(  connect_angle_max > dangle && dangle > -connect_angle_max  )
					{
						pb = pt_base + offset * 3;
						if( !b_connect_use_y)
							pb[1] = 0;
						if( is_dist_squared_less_v3r( pa, pb, connect_dist_squared ) &&  (pa != pb) )
						{
							*pt_good_index = offset;
							break;
						}
					}
				}
				while( --try_index >0 );
				if( try_index == 0 )
				{
					if( pa == pb )
						if( ++offset >= pt_nb )
							offset = 0;
					*pt_good_index = offset;
					continue;
				}
			}
		}
		
		size = miguel_rand_lin.get_fp32_01();
		if ( size_gain != .5 )
			gain_with_factor( size, size_gain_fac);
		if ( size_bias != .5 )
			bias_with_factor( size, size_bias_fac);
		size = interpolate( size_min, size_max,  size );

		tmp =	interpolate( map_min, map_max, miguel_rand_lin.get_fp32_01() );
		speed = REAL(aaa::time::get()) * tmp * interpolate( map_speed_min, map_speed_max, miguel_rand_lin.get_fp32_01() );

		draw_link( pa, pb, size, size_ratio, speed, speed+tmp );
	}
/*
	switch( s_draw)
	{
		case 0: draw_line(); break;
		case 1: draw_polygon(); break;
		case 2: draw_tri(); break;
	}
*/
}

