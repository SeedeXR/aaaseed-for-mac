#include "obj_ui/bdd/bdd_spe/bdd_array.h"
#include "infrastructure/layer/layers.h"
#include "math/rand.h"
#include "draw/seedcam.h"
#include "draw/model.h"
#include "gol/gol.h"
#include "gol/gol_matrix.h"

FACTORY_CREATE_PROP_V1( c_bdd_array, bdd_array, Array, bdd_array, sub_menu="To Sort"; );

C_PCHAR_C	str_array_draw_mode[3] =
{
	"Elements",
	"Torus",
};

namespace	n_bdd_array
{
	CONSTEXPR INT32	BASE_PARAM_NB	=	21 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	c_bdd_array::BDD_ARRAY_SRC_MAX_NB;

	CONST c_param_def param[ PARAM_NB_MAX ] =
	{
		BDD_BASE_PARAMS

		PARAM_DEF_SYMBO_PSTR_ONE(	draw_mode,		str_array_draw_mode )
		PARAM_DEF_INT32_POS_ONE(	seed			)
		PARAM_DEF_REAL_ZERO(		clip_angle		)

		PARAM_DEF_REAL_ZERO(		tra_factor		)
		PARAM_DEF_REAL_ZERO(		tra_min_u		)
		PARAM_DEF_REAL_ZERO(		tra_max_u		)
		PARAM_DEF_REAL_ZERO(		tra_min_v		)
		PARAM_DEF_REAL_ZERO(		tra_max_v		)
		PARAM_DEF_REAL_ZERO(		tra_min_axe		)
		PARAM_DEF_REAL_ZERO(		tra_max_axe		)

		PARAM_DEF_REAL_ZERO(		scale_factor	)
		PARAM_DEF_REAL_ZERO(		scale_min_u		)
		PARAM_DEF_REAL_ZERO(		scale_max_u		)
		PARAM_DEF_REAL_ZERO(		scale_min_v		)
		PARAM_DEF_REAL_ZERO(		scale_max_v		)
		PARAM_DEF_REAL_ZERO(		scale_min_axe	)
		PARAM_DEF_REAL_ZERO(		scale_max_axe	)

		PARAM_DEF_REAL(				proba_x,		1., 0., 0., 1. )
		PARAM_DEF_REAL(				proba_z,		1., 0., 0., 1. )

		PARAM_DEF_INT32(			nb_to_draw,	0., 1., 0., c_bdd_array::BDD_ARRAY_MAX_NB )
		PARAM_DEF_INT32(			nb_elt,		0., 1., 0., c_bdd_array::BDD_ARRAY_MAX_NB )

		PARAM_DEF_REAL_ONE(			proba_01 )
		PARAM_DEF_REAL_ONE(			proba_02 )
		PARAM_DEF_REAL_ONE(			proba_03 )
		PARAM_DEF_REAL_ONE(			proba_04 )
		PARAM_DEF_REAL_ONE(			proba_05 )
		PARAM_DEF_REAL_ONE(			proba_06 )
		PARAM_DEF_REAL_ONE(			proba_07 )
		PARAM_DEF_REAL_ONE(			proba_08 )
		PARAM_DEF_REAL_ONE(			proba_09 )
		PARAM_DEF_REAL_ONE(			proba_10 )
		PARAM_DEF_REAL_ONE(			proba_11 )
		PARAM_DEF_REAL_ONE(			proba_12 )
		PARAM_DEF_REAL_ONE(			proba_13 )
		PARAM_DEF_REAL_ONE(			proba_14 )
		PARAM_DEF_REAL_ONE(			proba_15 )
		PARAM_DEF_REAL_ONE(			proba_16 )
	};
}

void	c_bdd_array::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	//	Emission
	param_set_pt( h, draw_mode );
	param_set_pt( h, seed );
	param_set_pt( h, clip_angle );

	param_set_pt( h, tra_factor );
	param_set_pt( h, min[0] );
	param_set_pt( h, max[0] );
	param_set_pt( h, min[1] );
	param_set_pt( h, max[1] );
	param_set_pt( h, min[2] );
	param_set_pt( h, max[2] );

	param_set_pt( h, scale_factor );
	param_set_pt( h, scale_min[0] );
	param_set_pt( h, scale_max[0] );
	param_set_pt( h, scale_min[1] );
	param_set_pt( h, scale_max[1] );
	param_set_pt( h, scale_min[2] );
	param_set_pt( h, scale_max[2] );

	param_set_pt( h, proba_x );
	param_set_pt( h, proba_z );

	param_set_pt( h, nb_to_draw) ;
	param_set_pt( h, nb_elt );

	param_set_pt_n( h, proba, BDD_ARRAY_SRC_MAX_NB );

	err_param_init_pt(h);
}

void c_bdd_array::param_init()
{
	b_created_all = false;
	sum_last = -1.;
}

void c_bdd_array::create_elt( INT32 index, INT32 seed )
{
	ST_ARRAY_ELT*	pelt;

	pelt = &elt[index];
	pelt->scale = 1;
	pelt->rotation = 0;
	pelt->dist = 5;
}

void c_bdd_array::alloc()
{
}

void c_bdd_array::dealloc()
{
}

CONSTRUCTOR_CREATE(c_bdd_array)
{
	param_init_with( n_bdd_array::param, n_bdd_array::PARAM_NB_MAX ); // bdd_array_param, BDD_ARRAY_PARAM_NB_MAX);

	alloc();
}

c_bdd_array::~c_bdd_array()
{
	dealloc();
}

void	c_bdd_array::build()
{
}

void	c_bdd_array::update()
{
	REAL	sum;
	REAL*	p_proba;

	c_model::cur->get_axes( i_u, i_v, i_axe );

	if( !b_created_all )
	{
		for( INT32 i = 0; i < BDD_ARRAY_MAX_NB; ++i )
			create_elt( i, 0 );
		b_created_all = true;
	}
	
	//	add all the proba
	sum = 0;
	p_proba = proba;
	for( INT32 i = 0; i < nb_to_draw; ++i )
		sum += *p_proba++;

	//hack in rare case it will fail
	//	do it only when needed
	if ( sum_last != sum )
	{
		INT32	index;
		REAL	step;
		REAL	value;
		REAL	limit;
		index = -1;
		limit = 0.;
		value = 0;
		step = 1. / REAL( BDD_ARRAY_FAST_PROBA );
		for( INT32 i = 0; i < BDD_ARRAY_FAST_PROBA; ++i )
		{
			while ( value >= limit && index<( nb_to_draw-1 ) )
			{
				++index;
				limit += proba[index]/sum;
			}
			proba_fast[i] = index; 
			value += step;
		}
		sum_last = sum;
	}
}

void	c_bdd_array::draw_single()
{
	c_bdd_multiple::draw_single();
}

void	c_bdd_array::draw_multiple()
{
	ST_ARRAY_ELT*	pelt;
	//REAL	x,y,z;
	//INT32			i;
	INT32			nb_hack;
	c_rand_lin		rnd;
	c_rand_lin		rnd_seed;
	REAL			r[3];

//	if( this == bdd_cur)
//		return;

	c_multiple::cur->set_nb( nb_to_draw );

	pelt = &elt[0];
	r[0] = r[1] = r[2] = 0.;
	r[i_axe] = 1.;
	
	switch ( draw_mode)
	{
	case 0:
		for( INT32 i = 0; i < nb_to_draw; ++i )
		{
			REAL vec[3];
			GOL::matrix::push();
			//		x = pelt->dist * SIN_INT(pelt->angle);
			//		y = pelt->dist * COS_INT(pelt->angle);
				GOL::matrix::rotate_y( REAL(i)/REAL(nb_elt) );

			//	GOL::translate( min[0] * tra_factor, min[1] * tra_factor, min[2] * tra_factor);
				scale_v3( vec, min, tra_factor); 
			//		GOL::rotate_y( pelt->rotation);
			//		GOL::scale( 1, 1, 1);

				c_multiple::cur->align_then_draw( vec );
				c_multiple::cur->next_index();

			GOL::matrix::pop();
		}
		break;
	case 1:
		rnd_seed.set_seed(seed);
		for( INT32 i = 0; i < nb_elt; ++i )
		{
			REAL	vec[3];
			REAL	angle;
			REAL	tmp;
			INT32	dist_mod;
			REAL	dist_frac;
			REAL	dist;

			//	to be sure to be stable by elt
			rnd.set_seed(rnd_seed.get_uint32());
			dist = interpolate( min[0] * tra_factor, max[0] * tra_factor, rnd.get_fp32_01() );

			tmp = ( max[0] - min[0] ) * tra_factor;
			dist_frac = FMOD( dist,tmp);
			dist_mod = INT32( (dist - dist_frac) / tmp );
			rnd.set_seed( (dist_mod+1215155) * rnd_seed.get_uint32() );

			angle = rnd.get_fp32_max( 360 );
			tmp = c_seedcam::get_cur()->get_yaw() + angle + REAL(90);
			tmp = FMOD(tmp, REAL(360));
			tmp -= 180.;
			if ( (-clip_angle < tmp) && (tmp < clip_angle) )
			{
				//nb_hack = IMOD( (rnd.get_uint32())>>2, nb_to_draw);
				nb_hack = proba_fast[rnd.get_uint32()&(BDD_ARRAY_FAST_PROBA-1)];

				GOL::matrix::push();
				//		x = pelt->dist * SIN_INT(pelt->angle);
				//		y = pelt->dist * COS_INT(pelt->angle);
					GOL::matrix::rotatev_deg( angle, r );
					GOL::matrix::translate( dist_frac + min[0] * tra_factor );
//							interpolate( min[1] * tra_factor, max[1] * tra_factor, rnd.get_ufloat()),"
//							interpolate( min[2] * tra_factor, max[2] * tra_factor, rnd.get_ufloat())
//:							);
					GOL::matrix::rotatev( rnd.get_fp32_01(), r );
					if( rnd.get_fp32_01() < proba_x )
						GOL::matrix::rotate_x( rnd.get_fp32_01() );
					if( rnd.get_fp32_01() < proba_z )
						GOL::matrix::rotate_z( rnd.get_fp32_01() );
				//		GOL::rotate_y( pelt->rotation);
					GOL::matrix::scale(
							interpolate( scale_min[0] * scale_factor, scale_max[0] * scale_factor, rnd.get_fp32_01() ),
							interpolate( scale_min[1] * scale_factor, scale_max[1] * scale_factor, rnd.get_fp32_01() ),
							interpolate( scale_min[2] * scale_factor, scale_max[2] * scale_factor, rnd.get_fp32_01() )
							);
					clear_v3( vec);
					c_multiple::cur->align_then_draw( vec);
					c_multiple::cur->next_index();

				GOL::matrix::pop();
			}
			++pelt;
		}
		break;
	}
}
