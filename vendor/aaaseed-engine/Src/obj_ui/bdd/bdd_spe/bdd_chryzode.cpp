#include "bdd_chryzode.h"
#include "gol/gol.h"
#include "draw/line.h"
#include "draw/model.h"


FACTORY_CREATE_PROP_V1( c_bdd_chryzode, bdd_chryzode, Chryzode, bdd_chryzode, sub_menu="Experimental"; );

namespace n_bdd_chryzode
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 8 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32	PARAM_NB_MAX	= BASE_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS

		PARAM_DEF_POINT_XYZ(		center		)
		PARAM_DEF_INT32_INF(		seed,		0,3	)
		PARAM_DEF_INT32_POS(		modulo,		0,7	)
		PARAM_DEF_INT32_ZERO(		modulo_inc	)
		PARAM_DEF_INT32_POS_ONE(	iter		)
		PARAM_DEF_REAL_ONE(			factor		)
	};
}

void	c_bdd_chryzode::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt_3( h, _center );
	param_set_pt( h, _seed );
	param_set_pt( h, _mod );
	param_set_pt( h, _mod_inc );
	param_set_pt( h, _iter );
	param_set_pt( h, _factor );

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_bdd_chryzode)
{
	param_init_with( n_bdd_chryzode::param, n_bdd_chryzode::PARAM_NB_MAX ); // bdd_chryzode_param, BDD_CHRYZODE_NB_MAX);
}
EMPTY_DESTRUCTOR(c_bdd_chryzode)

void	c_bdd_chryzode::update()
{
	c_model::cur->get_size_v3( _size );
	_daxe = _size[2] / REAL(_iter );
}


void	c_bdd_chryzode::draw_one( INT32 inc, INT32 modulo, REAL CONST z )
{	
	INT32	i,j;
	REAL	cp,sp;
	REAL	c,s;
	REAL	tmp;

	i = 0;
	while( ++i < modulo)
	{
		tmp = REAL(i)/REAL(modulo);
		cp = COS_TURN( tmp);
		sp = SIN_TURN( tmp*_factor );
//		GET_SIN_COS_TURN( cp, sp, tmp);

		j = IMOD(i*inc,modulo);

		tmp = REAL(j)/REAL(modulo);
		c = COS_TURN( tmp);
		s = SIN_TURN( tmp*_factor );
//		GET_SIN_COS_TURN( c, s, tmp);

		draw_line( cp*_size[0], sp*_size[1], z, c*_size[0], s*_size[1], z );	
	}
}


void	c_bdd_chryzode::draw()
{
	INT32	modulo = _mod;
	REAL z = 0;
	for( INT32 i=_iter; i>0; --i )
	{
		draw_one( _seed, modulo, z );
		if( _mod_inc )
			modulo += _mod_inc;
		else
			modulo += _mod;
		z += _daxe;
	}
}
