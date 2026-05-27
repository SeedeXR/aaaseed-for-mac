#include "bdd_ben.h"
#include "gol/gol.h"
#include "draw/model.h"
#include "draw/axe.h"
#include "math/billboard.h"


FACTORY_CREATE_PROP_V1( c_bdd_ben, bdd_ben, bdd de test pour benoit, bdd_ben, sub_menu="Test"; );

namespace	n_bdd_ben
{
	CONSTEXPR INT32 PARAM_NB_MAX = 3 + c_bdd::GEO_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_POINT_XYZ( origin )
	};
}

void	c_bdd_ben::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt_3( h, _origin );

	err_param_init_pt(h);
}

void c_bdd_ben::init()
{
	param_init_with( n_bdd_ben::param, n_bdd_ben::PARAM_NB_MAX);
}

CONSTRUCTOR_CREATE(c_bdd_ben)
{
	init();
}
EMPTY_DESTRUCTOR(c_bdd_ben)

void c_bdd_ben::update()
{
	c_model::cur->get_size_v3( _size);
}

void c_bdd_ben::draw_single()
{
	GOL::matrix::push();
		GOL::matrix::translate3v( _origin );
		GOL::matrix::scale3v( _size );
		n_axe::draw_null_3d_list();
	GOL::matrix::pop();
}

void c_bdd_ben::draw_multiple()
{
	c_multiple::cur->set_nb( 1 );
	c_multiple::cur->align_then_draw( _origin );
}

INT32	c_bdd_ben::get_point_nb()
{
	return 1;
}

REAL*	c_bdd_ben::get_point_pt( INT32 CONST index )
{
	return _origin;
}
