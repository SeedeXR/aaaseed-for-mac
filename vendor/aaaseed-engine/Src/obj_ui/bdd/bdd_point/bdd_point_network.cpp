#include "bdd_point_network.h"
#include "gol/gol_draw.h"
#include "math/gainbias.h"
#include "time/aaa_time.h"
#include "math/rand.h"
#include "draw/render.h"

FACTORY_CREATE_PROP_V1( c_bdd_point_network, bdd_point_network, Point Network 2003, bdd_point_network, sub_menu="Point"; );

//todonow do it in 3d
//			this is just the 2d fn extended
static	FINLINE void	draw_link( REAL CONST* a, REAL CONST* b, REAL size, REAL size_ratio, REAL u_start, REAL u_stop )
{
	REAL	vec[3];
	REAL	normal[3];
	//	REAL	norm;

//	vec[1] = 0;

	normal[0] = a[2]-b[2];
	normal[1] = 0.;	
	normal[2] = b[0]-a[0];

	normalize_v2( normal[0], normal[2] );
	size *= .5;
	normal[0] *= size;
	normal[2] *= size;

	GOL::begin( c_render::get_cur()->get_draw_primitive());
	vec[0] = a[0] - normal[0] * size_ratio;
	vec[1] = a[1];
	vec[2] = a[2] - normal[2] * size_ratio;
	GOL::texcoord2( u_start, 1 );
	GOL::vertex3v( vec );

	vec[0] = a[0] + normal[0] * size_ratio;
	vec[2] = a[2] + normal[2] * size_ratio;
	GOL::texcoord2( u_start, 0 );
	GOL::vertex3v( vec );

	vec[0] = b[0] - normal[0];
	vec[1] = b[1];
	vec[2] = b[2] - normal[2];
	GOL::texcoord2( u_stop, 1 );
	GOL::vertex3v( vec );

	vec[0] = b[0] + normal[0];
	vec[2] = b[2] + normal[2];
	GOL::texcoord2( u_stop, 0 );
	GOL::vertex3v( vec );

	//				tmp += miguel_rand_lin.get_ufloat()*5.;
	//				GOL::texcoord2( tmp, 1);
	GOL::end();
}

namespace	n_bdd_point_network
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 15 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS

			PARAM_DEF_INT32_POS(	seed,				7777, 8888	)
			PARAM_DEF_INT32(		nb,					0, 1,		0, BDD_POINT_NETWORK_LINK_NB	)
			//	{	nullptr,	PARAM_REAL,	"range",		0, 1,		PARAM_MIN_REAL, PARAM_MAX_REAL,	nullptr, nullptr },
			PARAM_DEF_REAL_ZERO(	refresh_speed	)

	//		{	nullptr,	PARAM_REAL,	"connect_angle_max",0, 1,		PARAM_MIN_REAL, PARAM_MAX_REAL,	nullptr, nullptr },
			PARAM_DEF_REAL_ZERO(	connect_dist_min	)
			PARAM_DEF_REAL_ONE(		connect_dist_max	)
			PARAM_DEF_INT32(		connect_try_max,	0, 1,		0, 10000	)
//todo	was changing the bdd_src change the draw() to use it
//			PARAM_DEF_BOOL_OFF( connect_use_y )

			PARAM_DEF_REAL_INF(		size_min,			0, .01	)
			PARAM_DEF_REAL_INF(		size_max,			0, .01	)
			PARAM_DEF_GAIN(			size_gain			)
			PARAM_DEF_BIAS(			size_bias			)
			PARAM_DEF_REAL_ONE(		size_ratio			)

			PARAM_DEF_REAL_ONE(		map_min				)
			PARAM_DEF_REAL_ONE(		map_max				)
			PARAM_DEF_REAL_ZERO(		map_speed_min		)
			PARAM_DEF_REAL_ONE(		map_speed_max		)
	};
}

void	c_bdd_point_network::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt( h, seed			);
	param_set_pt( h, nb				);
	//	param_set_pt( h, range		);
	param_set_pt( h, refresh_speed	);

//	param_set_pt( h, connect_angle_max);
	param_set_pt( h, connect_dist_min	);
	param_set_pt( h, connect_dist_max	);
	param_set_pt( h, connect_try_max	);
//	param_set_pt( h, b_connect_use_y	);

	param_set_pt( h, size_min	);
	param_set_pt( h, size_max	);
	param_set_pt( h, size_gain	);
	param_set_pt( h, size_bias	);
	param_set_pt( h, size_ratio	);

	param_set_pt( h, map_min		);
	param_set_pt( h, map_max		);
	param_set_pt( h, map_speed_min	);
	param_set_pt( h, map_speed_max	);

	err_param_init_pt(h);
}

void	c_bdd_point_network::init()
{
	param_init_with( n_bdd_point_network::param, n_bdd_point_network::PARAM_NB_MAX ); // bdd_point_network_param, BDD_POINT_NETWORK_NB_MAX);
}

CONSTRUCTOR_CREATE(c_bdd_point_network)
{
	INT32*	pt_good_index = good_index;
	for( INT32 i = BDD_POINT_NETWORK_LINK_NB; i > 0; --i )
		*pt_good_index++ = -1;
	init();
}

c_bdd_point_network::~c_bdd_point_network()
{
}

void	c_bdd_point_network::update()
{
	size_gain_fac = gain_factor( size_gain);
	size_bias_fac = bias_factor( size_bias);
}

//todonow	real static
static	c_rand_lin		a_rand;

c_bdd*	bdd_point_src = nullptr;	//hack

void	c_bdd_point_network::draw_single()
{
	c_bdd*	bdd_src = get_bdd_prev();
	if( !bdd_src )
	{
		ERR_PRINT_STRING( "No bdd_cur." );
		return;
	}
	INT32	nb_pt = bdd_src->get_point_nb();
	if( nb_pt <= 0 )
	{
		if( bdd_point_src )
		{
			nb_pt = bdd_point_src->get_point_nb();
			if( nb_pt <= 0 )
				return;
			bdd_src = bdd_point_src;
		}
		else
			return;
	}
	bdd_point_src = bdd_src;	
	REAL*	pt_base = bdd_src->get_points();

	if( !pt_base )
		return;
	
	REAL	connect_dist_squared;
	REAL	connect_dist_min_squared;
	CONST	REAL*	pa;
	CONST	REAL*	pb;
	//INT32	i;
	INT32	offset;
	REAL	tmp;
	REAL	speed;
	REAL	size;
	INT32*	pt_good_index = good_index;
	REAL	time_offset;

	connect_dist_min_squared = connect_dist_min * connect_dist_min;
	connect_dist_squared = connect_dist_max * connect_dist_max;
	time_offset = REAL(aaa::time::get()) * refresh_speed;

	--pt_good_index;
	for( INT32 i=0; i<nb; ++i )
	{
		++pt_good_index;
		
		tmp = REAL(i)/REAL(nb) + time_offset;
		a_rand.set_seed( seed + i + INT32(tmp) );

		offset = IMOD( i, nb_pt );
		pa = pt_base + offset * 3;
		if( connect_dist_max == 0. )
		{
			pb = pt_base + INT32( a_rand.get_fp32_max(nb_pt-1) ) * 3;
		}
		else
		{
			//if( !b_connect_use_y )
			//	pa[1] = 0;
			offset = *pt_good_index;
			if( offset >= 0 )
			{
				pb = pt_base + offset * 3;
				//if( !b_connect_use_y )
				//	pb[1] = 0;
				if( !is_dist_squared_inside_v3r( pa, pb, connect_dist_min_squared, connect_dist_squared ) )
					pb = nullptr;					
			}
			else
			{
				pb = nullptr;
			}
			if( !pb )
			{
				INT32	try_index = connect_try_max;
				do
				{
					//offset = INT32( miguel_rand_lin.get_ufloat() * (pt_nb-1));
					if( ++offset >= nb_pt )
						offset = 0;
					pb = pt_base + offset * 3;
					//if( !b_connect_use_y )
					//	pb[1] = 0;
					if( is_dist_squared_inside_v3r( pa, pb, connect_dist_min_squared, connect_dist_squared ) &&  (pa != pb) )
					{
						*pt_good_index = offset;
						break;
					}
				}
				while( --try_index >0 );
					if( try_index == 0 )
					{
						if( pa == pb )
							if( ++offset >= nb_pt )
								offset = 0;
						*pt_good_index = offset;
						continue;
					}
			}
		}

		size = a_rand.get_fp32_01();
		if( size_gain != REAL(.5) )
			gain_with_factor( size, size_gain_fac);
		if( size_bias != REAL(.5) )
			bias_with_factor( size, size_bias_fac);
		size = interpolate( size_min, size_max,  size );

		tmp =	interpolate( map_min, map_max, a_rand.get_fp32_01() );
		speed = REAL(aaa::time::get()) * tmp * interpolate( map_speed_min, map_speed_max, a_rand.get_fp32_01() );

		draw_link( pa, pb, size, size_ratio, speed, speed+tmp );
	}
}

