#include "def_repulsor.h"
#include "ui/strsymbo.h"
#include "math/gainbias.h"
#include "math/v.h"
#include "infrastructure/param/param_declare.h"


FACTORY_INSTANCE_V1( c_def_repulsor, def_repulsor, Deformer Repulsor, def );

static	C_PCHAR_C	str_type[3] =
{
	"Planar",
	"Cylindrical",
	"Spherical",
};

static	C_PCHAR_C	str_effect[2] =
{
	"Repulsor",
	"Spiral",
};

namespace	n_def_repulsor
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 2 + c_deformer::BASE_PARAM_NB;
	CONSTEXPR INT32	WHERE_PARAM_NB	= 4;
	CONSTEXPR INT32	SRC_PARAM_NB	= 4;
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

		PARAM_DEF_SYMBO(		type,	1, 0,	2, str_type		)
		PARAM_DEF_SYMBO(		effect,	1, 0,	1, str_effect	)

		PARAM_DEF_GROUP(		Where, WHERE_PARAM_NB )
			PARAM_DEF_POINT_XYZ(	origin	)
	//		{	nullptr,	PARAM_SYMBOLIC,	"dim",			1, 0,		0, 3, nullptr, gstr::dim },
			PARAM_DEF_AXE_Y(		Axe )
		PARAM_DEF_GROUP( Source, SRC_PARAM_NB )	
			PARAM_DEF_SCALE_UVAF(	src_ext		)

	//		{	nullptr,	PARAM_REAL,	"src_int_u",		0., 1.,	PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },
	//		{	nullptr,	PARAM_REAL,	"src_int_v",		0., 1.,	PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },
	//		{	nullptr,	PARAM_REAL,	"src_int_axe",		0., 1.,	PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },
	//		{	nullptr,	PARAM_REAL,	"src_int_factor",	1., 0.,	PARAM_MIN_REAL, PARAM_MAX_REAL, nullptr, nullptr },
		PARAM_DEF_GROUP( Destination, DPARAM_DEF_NB )
			PARAM_DEF_SCALE_UVAF(	dst_ext		)
			PARAM_DEF_SCALE_UVA(	dst_int		)
			PARAM_DEF_REAL_ZERO(	dst_int_factor	)
		PARAM_DEF_GROUP( How, HOW_PARAM_NB )	
			PARAM_DEF_GAIN(			gain	)
			PARAM_DEF_BIAS(			bias	)
		PARAM_DEF_GROUP( Spiral, SPIRAL_PARAM_NB )	
			PARAM_DEF_REAL_ONE(		spiral_frequency	)	
			PARAM_DEF_REAL_ONE(		spiral_radius		)
			PARAM_DEF_REAL_ONE(		spiral_interpolate	)	
	};
}

CONSTRUCTOR_CREATE(c_def_repulsor)
{
	init_name_with( "Repulsor" );
	param_init_with( n_def_repulsor::param, n_def_repulsor::PARAM_NB_MAX ); // def_repulsor_param, DEF_REPULSOR_PARAM_NB_MAX);
	init();
}

void	c_def_repulsor::init()
{
}

void	c_def_repulsor::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt( h, s_type );
	param_set_pt( h, s_effect );

	++h;
		param_set_pt_3( h, origin );
	//	param_set_pt( h, dim);
		param_set_pt( h, i_axe );

	++h;
		param_set_pt_4( h, src_ext_ui );
	//	param_set_pt_4( h, src_int_ui );

	++h;
		param_set_pt_4( h, dst_ext_ui );
		param_set_pt_4( h, dst_int_ui );

	++h;
		param_set_pt( h, gain );
		param_set_pt( h, bias );

	++h;
		param_set_pt( h, spiral_freq );
		param_set_pt( h, spiral_radius );
		param_set_pt( h, spiral_inter );

	err_param_init_pt(h);
}

c_def_repulsor::~c_def_repulsor()
{
}

void	c_def_repulsor::update()
{
	//	prepare the axes
	axe_build_index( i_u, i_v, i_axe);

	src_ext[i_u] =		src_ext_ui[0] * src_ext_ui[3];
	src_ext[i_v] =		src_ext_ui[1] * src_ext_ui[3];
	src_ext[i_axe] =	src_ext_ui[2] * src_ext_ui[3];
//	scale_v3( src_ext, src_ext_ui, src_ext_ui[3]);

//	src_int[i_u] =		src_int_ui[0] * src_int_ui[3] * src_ext[i_u];
//	src_int[i_v] =		src_int_ui[1] * src_int_ui[3] * src_ext[i_v];
//	src_int[i_axe] =	src_int_ui[2] * src_int_ui[3] * src_ext[i_axe];

//	src_factor[0] = OVER_ONE( src_ext[0] - src_int[0]);
//	src_factor[1] = OVER_ONE( src_ext[1] - src_int[1]);
//	src_factor[2] = OVER_ONE( src_ext[2] - src_int[2]);
	src_factor[0] = OVER_ONE_AS_REAL( src_ext[0] );
	src_factor[1] = OVER_ONE_AS_REAL( src_ext[1] );
	src_factor[2] = OVER_ONE_AS_REAL( src_ext[2] );

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

FINLINE	bool	process_for_ext( REAL &out, REAL lim_ext, REAL in)
{
	out = in;
	return ABS(out) >= lim_ext;
}

FINLINE	void	process_for_int( REAL &out, REAL &dist, REAL factor, REAL in)
{
/*	old method
def[1] = ABS( *(dst+1) - origin[1] );
if( def[1] >= src_ext[1] )
continue;
*/
		out = in*factor;
		dist += out * out;
}

FINLINE	void	c_def_repulsor::apply_1d( REAL*	dst, INT32 nb )
{
	REAL		vec;
	REAL		vec_abs;
	REAL		v;
	REAL		dist;
	REAL		factor;

	dst -= 3;
	for( ; nb>0; --nb )
		{
		dst += 3;	//	done here because of continue
		
		//	check fast in cube
		if( process_for_ext( vec, src_ext[i_axe], dst[i_axe] - origin[i_axe] ) )
			continue;
		vec_abs = ABS( vec);

		//	check in sphere
		dist = 0;
		process_for_int( v, dist, src_factor[i_axe], vec_abs);
		if( dist >= 1. )
			continue;
		if( dist < 0. )
			dist = 0.;
		else
			dist = SQRT( dist );

		//	ok it's in
		factor = dist;
		if ( gain != .5 )
			gain_with_factor( factor, gain_fac);
		if ( bias != .5 )
			bias_with_factor( factor, bias_fac);

		switch( s_effect )
			{
			case 0:
				if (dist != 0)
					v *= REAL(factor / dist);
				else
					v = 0;
				dist = dst_int[i_axe] + v * dst_factor[i_axe] - vec_abs;
				dst[i_axe] +=  (vec>0) ? dist : -dist;
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
		}	//for(; nb>0; --nb )
}
	
FINLINE	void	c_def_repulsor::apply_2d( REAL*	dst, INT32 nb )
{
	REAL		vec[2];
	REAL		vec_abs[2];
	REAL		v[2];
	REAL		dist;
	REAL		factor;

	dst -= 3;
	for(; nb>0; --nb )
		{
		dst += 3;	//	done here because of continue
		//	check fast in cube
		if( process_for_ext( vec[0], src_ext[i_u], dst[i_u] - origin[i_u] ) )
			continue;
		if( process_for_ext( vec[1], src_ext[i_v], dst[i_v] - origin[i_v] ) )
			continue;
		abs_v2r( vec_abs, vec);

		//	check in sphere
		dist = 0;
		process_for_int( v[0], dist, src_factor[i_u], vec_abs[0]);
		process_for_int( v[1], dist, src_factor[i_v], vec_abs[1]);
		if( dist >= 1. )
			continue;
		if( dist < 0. )
			dist = 0.;
		else
			dist = SQRT( dist );

		//	ok it's in
		factor = dist;
		if ( gain != .5 )
			gain_with_factor( factor, gain_fac);
		if ( bias != .5 )
			bias_with_factor( factor, bias_fac);

		switch( s_effect )
			{
			case 0:
				scale_v2( v, factor/dist);
				factor = dst_int[i_u] + v[0] * dst_factor[i_u] - vec_abs[0];
				dst[i_u] +=  (vec[0]>0) ? factor : -factor;
				factor = dst_int[i_v] + v[1] * dst_factor[i_v] - vec_abs[1];
				dst[i_v] +=  (vec[1]>0) ? factor : -factor;
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
		}	//for(; nb>0; --nb )
}	

FINLINE	void	c_def_repulsor::apply_3d( REAL*	dst, INT32 nb )
{
	REAL		vec[3];
	REAL		vec_abs[3];
	REAL		v[3];
	REAL		dist;
	REAL		factor;

	dst -= 3;
	for(; nb>0; --nb )
		{
		dst += 3;	//	done here because of continue
		//	check fast in cube
		if( process_for_ext( vec[0], src_ext[0], dst[0] - origin[0] ) )
			continue;
		if( process_for_ext( vec[2], src_ext[2], dst[2] - origin[2] ) )
			continue;
		if( process_for_ext( vec[1], src_ext[1], dst[1] - origin[1] ) )
			continue;
		abs_v3r( vec_abs, vec);

		//	check in sphere
		dist = 0;
		process_for_int( v[0], dist, src_factor[0], vec_abs[0]);
		process_for_int( v[1], dist, src_factor[1], vec_abs[1]);
		process_for_int( v[2], dist, src_factor[2], vec_abs[2]);
		if( dist >= 1. )
			continue;
		if( dist < 0. )
			dist = 0.;
		else
			dist = SQRT( dist );

		//	ok it's in
		factor = dist;
		if ( gain != .5 )
			gain_with_factor( factor, gain_fac);
		if ( bias != .5 )
			bias_with_factor( factor, bias_fac);

		switch( s_effect )
			{
			case 0:
				scale_v3( v, factor/dist);
				dist = dst_int[0] + v[0] * dst_factor[0] - vec_abs[0];
				dst[0] +=  (vec[0]>0) ? dist : -dist;
				dist = dst_int[1] + v[1] * dst_factor[1] - vec_abs[1];
				dst[1] +=  (vec[1]>0) ? dist : -dist;
				dist = dst_int[2] + v[2] * dst_factor[2] - vec_abs[2];
				dst[2] +=  (vec[2]>0) ? dist : -dist;
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
		}	//for(; nb>0; --nb )
}

void	c_def_repulsor::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	if( src != dst )
		MEMCPY( dst, src, (size_t) (nb * sizeof(REAL) * 3), __FUNCTION__ );
	
	switch( s_type)
	{
		case 0:	apply_1d( dst, nb ); break;
		case 1:	apply_2d( dst, nb ); break;
		case 2:	apply_3d( dst, nb ); break;
	}

}

