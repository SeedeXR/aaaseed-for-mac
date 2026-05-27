#include "bdd_whisper.h"
//#include "gol.h"
//#include "draw/model.h"
//#include "draw/axe.h"
//#include "math/billboard.h"


FACTORY_CREATE_PROP_V1( c_bdd_whisper, bdd_whisper, AI Speech to Text, bdd_whisper, sub_menu="AI"; );

namespace	n_bdd_whisper
{
	static	INT32 CONST		PARAM_NB_MAX = 0 + c_bdd::NO_GEO_PARAM_NB;

	static	c_param_def	param[PARAM_NB_MAX] =
	{
		BDD_NO_GEO_BASE_PARAMS
//		PARAM_DEF_POINT_XYZ( origin )
	};
}

void	c_bdd_whisper::param_init_pt_static()
{
	INT32	h = param_init_pt_no_geo();

//	param_set_pt_3( h, _origin );

	err_param_init_pt(h);
}

void c_bdd_whisper::init()
{
	param_init_with( n_bdd_whisper::param, n_bdd_whisper::PARAM_NB_MAX);
}

CONSTRUCTOR_CREATE(c_bdd_whisper)
{
	init();
}
EMPTY_DESTRUCTOR(c_bdd_whisper)

void c_bdd_whisper::update()
{
//	c_model::cur->get_size_v3( _size);
}

void c_bdd_whisper::draw_single()
{
	//GOL::matrix::push();
	//	GOL::matrix::translate3v( _origin );
	//	GOL::matrix::scale3v( _size );
	//	n_axe::draw_null_3d_list();
	//GOL::matrix::pop();
}

/*
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
*/