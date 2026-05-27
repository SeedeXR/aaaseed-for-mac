#include "bdd_null.h"
#include "ui/alphabet.h"
#include "gol/gol.h"
#include "draw/model.h"
#include "draw/axe.h"
#include "math/billboard.h"


FACTORY_CREATE_PROP_V1( c_bdd_null, bdd_null, Null, null, sub_menu="Helper"; );

static	C_PCHAR_C	str_draw_number[5] =
{
	"No",
	"X",
	"Y",
	"Z",
	"Billboard",
};

namespace	n_bdd_null
{
	CONSTEXPR INT32 PARAM_NB_MAX = 6 + c_bdd::GEO_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_POINT_XYZ(		origin )
		PARAM_DEF_SYMBO_MIN_MAX(	dim,		2,	3,	2, PT_NB_STR(gstr::dim),	gstr::dim )
		PARAM_DEF_SYMBO_PSTR_ZERO(	draw_text,	str_draw_number )
		PARAM_DEF_STR(				text )
	};
}

void	c_bdd_null::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt_3( h, _origin );
	param_set_pt( h, _s_dim );
	param_set_pt( h, _s_draw_text );
	param_set_pt( h, _text	);

	err_param_init_pt(h);
}

void c_bdd_null::init()
{
	param_init_with( n_bdd_null::param, n_bdd_null::PARAM_NB_MAX);
	_text.erase();
}

CONSTRUCTOR_CREATE(c_bdd_null)
{
	init();
}
EMPTY_DESTRUCTOR(c_bdd_null)

void c_bdd_null::update()
{
	c_model::cur->get_size_v3( _size );
}

void c_bdd_null::draw_single()
{
	GOL::matrix::push();
		GOL::matrix::translate3v( _origin );
		GOL::matrix::scale3v( _size );
		if( _s_dim == 3 )
			n_axe::draw_null_3d_list();
		else
			n_axe::draw_null_2d_list( c_model::cur->get_axe() );
		if( _s_draw_text )
		{
			GOL::matrix::scale( REAL(.1) );
			aaa::alphabet::draw_str_begin();

				switch( _s_draw_text )
				{
				case 4:							
					GOL::matrix::translate3v( _origin );
					billboard::do_y( _origin );
					break;
				case 1:
					GOL::matrix::rotate_y_deg( 90. );
					break;
				case 2:
					GOL::matrix::rotate_x_deg( -90. );
					GOL::matrix::rotate_z_deg( -90. );
					break;
				}
				if( _text.is_empty() )
					aaa::alphabet::draw_str( "Null" );
				else
					aaa::alphabet::draw_str( _text.get() );

			aaa::alphabet::draw_str_end();
		}
	GOL::matrix::pop();
}

void c_bdd_null::draw_multiple()
{
	c_multiple::cur->set_nb( 1);
	c_multiple::cur->align_then_draw( _origin );
}

INT32	c_bdd_null::get_point_nb()
{
	return 1;
}

REAL*	c_bdd_null::get_point_pt( INT32 CONST index )
{
	return _origin;
}
