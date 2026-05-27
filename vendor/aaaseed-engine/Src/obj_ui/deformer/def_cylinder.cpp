#include "def_cylinder.h"
#include "ui/strsymbo.h"
#include "math/gainbias.h"
#include "math/v.h"
#include "infrastructure/param/param_declare.h"


FACTORY_INSTANCE_V1( c_def_tocylinder, def_tocylinder, Deformer To Cylinder even Cone, def );

namespace	n_def_tocylinder
{
	CONSTEXPR INT32	BASE_PARAM_NB	= c_deformer::BASE_PARAM_NB + 18;
	CONSTEXPR INT32	GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		DEFORMER_BASE_PARAMS

		PARAM_DEF_REAL_ONE(		amount	)	//todo generalize to all deformer

		PARAM_DEF_AXE_Z(		axe_src )
		PARAM_DEF_AXE_X(		axe_dst )
		PARAM_DEF_BOOL_OFF(		flip_uv	)

		PARAM_DEF_REAL_ONE(		u_factor	)
		PARAM_DEF_REAL_ZERO(	u_offset	)

		PARAM_DEF_REAL_ONE(		v_factor	)
		PARAM_DEF_REAL_ZERO(	v_offset	)

		PARAM_DEF_BOOL_ON(		final			)
		PARAM_DEF_REAL_ONE(		radius			)
		PARAM_DEF_REAL_INF(		cone_angle,		0, 0.25	)

		PARAM_DEF_BOOL_OFF(		side			)
		PARAM_DEF_INT32(		side_nb,		4, 3,	2,	32	)
		PARAM_DEF_REAL_ONE(		side_amount		)

		PARAM_DEF_BOOL_OFF(		spiral			)
		PARAM_DEF_REAL_ONE(		spiral_factor	)

		PARAM_DEF_REAL_ONE(		lissajous_ratio	)
		PARAM_DEF_REAL_ONE(		size_ratio		)
	};
}

CONSTRUCTOR_CREATE(c_def_tocylinder)
	,_b_side(false)
	,_b_spiral(false)
{
	init_name_with( "ToCylinder" );
	param_init_with( n_def_tocylinder::param, n_def_tocylinder::PARAM_NB_MAX ); // def_tocylinder_param, DEF_TOCYLINDER_PARAM_NB_MAX);
	init();
}

void	c_def_tocylinder::init()
{
}

void	c_def_tocylinder::param_init_pt()
{
	INT32	h = SUPER::param_init_pt_start();

	param_set_pt( h, _def_amount		);

	param_set_pt( h, _axe_src			);
	param_set_pt( h, _axe_dst			);
	param_set_pt( h, _b_flip_uv			);

	param_set_pt( h, _u_factor			);
	param_set_pt( h, _u_offset			);
	param_set_pt( h, _v_factor			);
	param_set_pt( h, _v_offset			);

	param_set_pt( h, _b_final			);
	param_set_pt( h, _radius			);
	param_set_pt( h, _cone_angle		);

	param_set_pt( h, _b_side_ui			);
	param_set_pt( h, _side_nb			);
	param_set_pt( h, _side_amount		);

	param_set_pt( h, _b_spiral			);
	param_set_pt( h, _spiral_factor_ui	);

	param_set_pt( h, _lissajous_ratio	);
	param_set_pt( h, _size_ratio		);

	err_param_init_pt(h);
}

c_def_tocylinder::~c_def_tocylinder()
{
}

void	c_def_tocylinder::update()
{
	bool b_def = _def_amount!=0.;

	set_deforming( b_def );
	if( !b_def )
		return;

	//	prepare the axes
	axe_build_index_vert( _i_u_dst, _i_v_dst, _axe_dst );
	axe_build_index_vert( _i_u_src, _i_v_src, _axe_src );
	if( _b_flip_uv )
		SWAP( _i_u_src, _i_v_src );

	_b_side = _b_side_ui ? _side_amount!=0. : false;

	if( _b_side )
	{
		for( INT32 i=0; i<_side_nb; ++i )
		{
			st_pt_info * p = &pts[i];

			DOUBLE ph = REAL(i)/REAL(_side_nb) - .25;
			REAL c = COS_TURN(ph);
			REAL s = SIN_TURN(ph);
			p->pos[0] = _radius * c;
			p->pos[1] = _radius * s;

			ph = REAL(i+.5)/REAL(_side_nb) - .25;
			c = COS_TURN(ph);
			s = SIN_TURN(ph);
			p->nor[0] = c;
			p->nor[1] = s;
		}
		pts[_side_nb] = pts[0];
	}
	_spiral_factor = _b_spiral ? _spiral_factor_ui : 0;
}

void	c_def_tocylinder::apply_side( REAL CONST * src, REAL* dst, INT32 nb )
{
	REAL vec[3];
	REAL cc = COS_TURN( _cone_angle );
	REAL cs = SIN_TURN( _cone_angle );

	for( ; nb>0; --nb )
	{
		REAL ra = src[_axe_src];
		REAL ru	= src[_i_u_src] * -_u_factor + _u_offset;
		REAL rv	= src[_i_v_src] * _v_factor +  _v_offset;

		if( _b_final )
		{
			ra += rv * cs + ru * _spiral_factor;

			// cylinder / cone magic happen here
			REAL turn = REAL( (ru+.25) * _side_nb );
			INT32 i_prev = I_FLOOR( turn );
			REAL rest = turn - i_prev;
			i_prev = WRAP_INDEX( i_prev, _side_nb-1 );

			//todo does lissajous still make sense ?
			REAL s = SIN_TURN(ru);
			REAL c = COS_TURN(ru*_lissajous_ratio) * _size_ratio;
			REAL rad = _radius + rv * cc;

			REAL pos[2];
			interpolate_v2( pos, pts[i_prev].pos, pts[i_prev+1].pos, rest );

			pos[0] += pts[i_prev].nor[0] * rv * cc;
			pos[1] += pts[i_prev].nor[1] * rv * cc;

			ru = interpolate( rad*c, pos[0], _side_amount );
			rv = interpolate( rad*s, pos[1], _side_amount );
		}

		vec[_axe_dst] = ra;
		vec[_i_u_dst] = ru;
		vec[_i_v_dst] = rv;

		interpolate_v3( dst, vec, _def_amount );

		src += 3;
		dst += 3;
	}
}

void	c_def_tocylinder::apply_no_side( REAL CONST * src, REAL* dst, INT32 nb )
{
	REAL vec[3];
	REAL cc = COS_TURN( _cone_angle );
	REAL cs = SIN_TURN( _cone_angle );

	for( ; nb>0; --nb )
	{
		REAL ra = *(src+_axe_src);
		REAL ru	= *(src+_i_u_src ) * -_u_factor + _u_offset;
		REAL rv	= *(src+_i_v_src ) * _v_factor +  _v_offset;

		if( _b_final )
		{
			ra += rv * cs + ru * _spiral_factor;

			//todo does lissajous still make sense ?
			REAL s = SIN_TURN(ru);
			REAL c = COS_TURN(ru*_lissajous_ratio) * _size_ratio;

			// cylinder/cone magic happen here
			rv = _radius + rv * cc;
			ru = rv * c;
			rv = rv * s;
		}

		vec[_axe_dst] = ra;
		vec[_i_u_dst] = ru;
		vec[_i_v_dst] = rv;

		interpolate_v3( dst, vec, _def_amount );

		src += 3;
		dst += 3;
	}
}

void	c_def_tocylinder::apply( REAL CONST * src, REAL* dst, INT32 nb )
{
	if( _b_side )
		apply_side( src, dst, nb );
	else
		apply_no_side( src, dst, nb );
}

