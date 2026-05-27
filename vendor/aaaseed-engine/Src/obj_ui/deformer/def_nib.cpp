#include "def_nib.h"
#include "ui/strsymbo.h"
#include "math/gainbias.h"
#include "math/v.h"
#include "infrastructure/param/param_declare.h"


FACTORY_INSTANCE_PROP_V1( c_def_nib, def_nib, Deformer Nibelungen, def, sub_menu="Special"; );

static	C_PCHAR_C	str_effect[2] =
{
	"Repulsor",
	"Spiral",
};

namespace	n_def_nib
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 1 + c_deformer::BASE_PARAM_NB;
	CONSTEXPR INT32	WHERE_PARAM_NB	= 4;
	CONSTEXPR INT32	SRC_PARAM_NB	= 8;
	CONSTEXPR INT32	DPARAM_DEF_NB	= 8;
	CONSTEXPR INT32	HOW_PARAM_NB	= 2;
	CONSTEXPR INT32	SPIRAL_PARAM_NB	= 3;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 5;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	WHERE_PARAM_NB
									+	SRC_PARAM_NB
									+	DPARAM_DEF_NB
									+	HOW_PARAM_NB
									+	SPIRAL_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_SYMBO(	effect,	1, 0,	1, str_effect )
		PARAM_DEF_GROUP(	Where, WHERE_PARAM_NB )	
			PARAM_DEF_POINT_XYZ(	origin	)	
			PARAM_DEF_AXE_Y(		Axe )
		PARAM_DEF_GROUP( Source, SRC_PARAM_NB )	
			PARAM_DEF_SCALE_UVAF(	src_ext				)
			PARAM_DEF_SCALE_UVA(	src_int				)
			PARAM_DEF_REAL_ZERO(	src_int_factor		)
		PARAM_DEF_GROUP( Destination, DPARAM_DEF_NB )
			PARAM_DEF_SCALE_UVAF(	dst_ext				)
			PARAM_DEF_SCALE_UVA(	dst_int				)
			PARAM_DEF_REAL_ZERO(	dst_int_factor		)
		PARAM_DEF_GROUP( How, HOW_PARAM_NB )	
			PARAM_DEF_GAIN(			gain	)
			PARAM_DEF_BIAS(			bias	)
		PARAM_DEF_GROUP( Spiral, SPIRAL_PARAM_NB )	
			PARAM_DEF_REAL_ONE(		spiral_frequency	)
			PARAM_DEF_REAL_ONE(		spiral_radius		)
			PARAM_DEF_REAL_ONE(		spiral_interpolate	)
	};
}

CONSTRUCTOR_CREATE(c_def_nib)
{
	init_name_with( "Nib deformer" );
	param_init_with( n_def_nib::param, n_def_nib::PARAM_NB_MAX ); // def_nib_param, DEF_NIB_PARAM_NB_MAX);
	init();
}

void	c_def_nib::init()
{
}

void	c_def_nib::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt( h, s_effect);

	++h;
		param_set_pt_3( h, origin );
		param_set_pt( h, i_axe);

	++h;
		param_set_pt_4( h, src_ext_ui );
		param_set_pt_4( h, src_int_ui );

	++h;
		param_set_pt_4( h, dst_ext_ui );
		param_set_pt_4( h, dst_int_ui );

	++h;
		param_set_pt( h, gain);
		param_set_pt( h, bias);

	++h;
		param_set_pt( h, spiral_freq);
		param_set_pt( h, spiral_radius);
		param_set_pt( h, spiral_inter);


	err_param_init_pt(h);
}

c_def_nib::~c_def_nib()
{
}

void	c_def_nib::update()
{
	//	prepare the axes
	axe_build_index( i_u, i_v, i_axe);

	src_ext[i_u] =		src_ext_ui[0] * src_ext_ui[3];
	src_ext[i_v] =		src_ext_ui[1] * src_ext_ui[3];
	src_ext[i_axe] =	src_ext_ui[2] * src_ext_ui[3];

	src_int[i_u] =		src_int_ui[0] * src_int_ui[3] * src_ext[i_u];
	src_int[i_v] =		src_int_ui[1] * src_int_ui[3] * src_ext[i_v];
	src_int[i_axe] =	src_int_ui[2] * src_int_ui[3] * src_ext[i_axe];

	src_factor[0] = src_ext[0] - src_int[0];
	if( src_factor[0] != 0. )
		src_factor[0] = REAL( 1./src_factor[0] ); 

	src_factor[1] = src_ext[1] - src_int[1];
	if( src_factor[1] != 0. )
		src_factor[1] = REAL( 1./src_factor[1] );

	src_factor[2] = src_ext[2] - src_int[2];
	if( src_factor[2] != 0. )
		src_factor[2] = REAL( 1./src_factor[2] );

	dst_ext[i_u] =		dst_ext_ui[0] * dst_ext_ui[3] * src_ext[i_u];
	dst_ext[i_v] =		dst_ext_ui[1] * dst_ext_ui[3] * src_ext[i_v];
	dst_ext[i_axe] =	dst_ext_ui[2] * dst_ext_ui[3] * src_ext[i_axe];

	dst_int[i_u] =		dst_int_ui[0] * dst_int_ui[3] * dst_ext[i_u];
	dst_int[i_v] =		dst_int_ui[1] * dst_int_ui[3] * dst_ext[i_v];
	dst_int[i_axe] =	dst_int_ui[2] * dst_int_ui[3] * dst_ext[i_axe];

	dst_factor[0] = dst_ext[0] - dst_int[0];
	dst_factor[1] = dst_ext[1] - dst_int[1];
	dst_factor[2] = dst_ext[2] - dst_int[2];

	gain_fac = gain_factor( gain);
	bias_fac = bias_factor( bias);

	set_deforming( true );
}

void	c_def_nib::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	REAL	vec[3];
	REAL	def[3];
	REAL	dist;
	REAL	factor;

	if( src != dst )
		MEMCPY( dst, src, (size_t) (nb * sizeof(REAL) * 3), __FUNCTION__ );

	dst -= 3;
	for(; nb>0; --nb )
		{
		dst += 3;

		//	check fast in cube
		vec[0] = dst[0] - origin[0];
		if( vec[0] >= 0.)
			{
			if( vec[0] >= src_ext[0] )
				continue;
			def[0] = vec[0];
			}
		else
			{
			if( vec[0] <= -src_ext[0] )
				continue;
			def[0] = -vec[0];
			}

		vec[2] = dst[2] - origin[2];
		if( vec[2] >= 0.)
			{
			if( vec[2] >= src_ext[2] )
				continue;
			def[2] = vec[2];
			}
		else
			{
			if( vec[2] <= -src_ext[2] )
				continue;
			def[2] = -vec[2];
			}

		vec[1] = dst[1] - origin[1];
		if( vec[1] >= 0.)
			{
			if( vec[1] >= src_ext[1] )
				continue;
			def[1] = vec[1];
			}
		else
			{
			if( vec[1] <= -src_ext[1] )
				continue;
			def[1] = -vec[1];
			}


/*	old method
		def[1] = ABS( *(dst+1) - origin[1] );
		if( def[1] >= src_ext[1] )
			continue;
*/
		//	check in sphere
		if ( def[0] <= src_int[0] )
			{
			def[0] = 0.;
			dist = 0;
			}
		else
			{
			def[0] = (def[0]-src_int[0])*src_factor[0];
			dist = def[0] * def[0];
			}
		
		if ( def[1] <= src_int[1] )
			{
			def[1] = 0.;
			}
		else
			{
			def[1] = (def[1]-src_int[1])*src_factor[1];
			dist += def[1] * def[1];
			if( dist >= 1. )
				continue;
			}

		if ( def[2] <= src_int[2] )
			{
			def[2] = 0.;
			}
		else
			{
			def[2] = (def[2]-src_int[2])*src_factor[2];
			dist += def[2] * def[2];
			if( dist >= 1. )
				continue;
			}

		//	ok it's in
		factor = SQRT( dist );
		if ( gain != .5 )
			gain_with_factor( factor, gain_fac);
		if ( bias != .5 )
			bias_with_factor( factor, bias_fac);

		switch( s_effect )
			{
			case 0:
				normalize_scale_v3r( def, factor);

				if( vec[0] > 0 )
					dst[0] += -vec[0] + dst_int[0] + def[0] * dst_factor[0];
				else
					dst[0] -= +vec[0] + dst_int[0] + def[0] * dst_factor[0];
				if( vec[1] > 0 )
					dst[1] += -vec[1] + dst_int[1] + def[1] * dst_factor[1];
				else
					dst[1] -= +vec[1] + dst_int[1] + def[1] * dst_factor[1];
				if( vec[2] > 0 )
					dst[2] += -vec[2] + dst_int[2] + def[2] * dst_factor[2];
				else
					dst[2] -= +vec[2] + dst_int[2] + def[2] * dst_factor[2];
				break;
			case 1:
				{
				REAL	f[3];
				REAL	cos;
				REAL	sin;
				f[i_axe] = *(dst+i_axe);
				GET_SIN_COS_TURN( sin, cos , f[i_axe]*spiral_freq);
				f[i_u] = origin[i_u] + cos * spiral_radius;// * (1-factor);
				f[i_v] = origin[i_v] + sin * spiral_radius;// * (1-factor);
				interpolate_v3( dst, dst, f, spiral_inter*(1-factor) );
				}
				break;
			}
		}
}

