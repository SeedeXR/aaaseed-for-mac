#include "bdd_line.h"
#include "ui/alphabet.h"
#include "gol/gol.h"
#include "draw/model.h"
#include "draw/axe.h"
#include "draw/line.h"
#include "math/billboard.h"


FACTORY_CREATE_PROP_V1( c_bdd_line, bdd_line, Line, bdd_line, sub_menu="Geometry"; );

namespace	n_bdd_line
{
	CONSTEXPR INT32 POINT_PARAM_NB	= 5;
	CONSTEXPR INT32 PARAM_NB_MAX	=	POINT_PARAM_NB * 2
									+	c_bdd::GEO_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
		PARAM_DEF_POINT_XYZ(	origin )
		PARAM_DEF_SYMBO(		origin_draw_text,	1, 0,	4, gstr::draw_number )
		PARAM_DEF_STR(			origin_text )
		PARAM_DEF_POINT_XYZ(	point )
		PARAM_DEF_SYMBO(		point_draw_text,	1, 0,	4, gstr::draw_number )
		PARAM_DEF_STR(			point_text )
	};
}

void	c_bdd_line::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	for( INT32 i = 0; i < 2; ++i )
	{
		param_set_pt_3( h, _point + i*3 );
		param_set_pt( h, _s_draw_text[i] );
		param_set_pt( h, _text[i] );
	}
	err_param_init_pt(h);
}

void c_bdd_line::init()
{
	param_init_with( n_bdd_line::param, n_bdd_line::PARAM_NB_MAX );
	_text[0].erase();
	_text[1].erase();
}

CONSTRUCTOR_CREATE(c_bdd_line)
{
	init();
}
EMPTY_DESTRUCTOR(c_bdd_line)

void c_bdd_line::update()
{
	c_model::cur->get_size_v3( _size );
}

void c_bdd_line::draw_single()
{
	draw_point(0);
	draw_point(1);
	draw_line( _point, _point+3 );		
}

void c_bdd_line::draw_point( INT32 i)
{
	GOL::matrix::push();
		GOL::matrix::translate3v( _point+i*3 );
		GOL::matrix::scale3v( _size );
		n_axe::draw_null_3d_list();
		if( _s_draw_text[i] )	//todo do a fn with this see bdd_null too
		{
			GOL::matrix::scale( REAL(.1) );
			aaa::alphabet::draw_str_begin();

				switch( _s_draw_text[i] )
				{
				case 4:							
					GOL::matrix::translate3v( _point+i*3 );
					billboard::do_y( _point+i*3 );
					break;
				case 1:
					GOL::matrix::rotate_y_deg( 90. );
					break;
				case 2:
					GOL::matrix::rotate_x_deg( -90. );
					GOL::matrix::rotate_z_deg( -90. );
					break;
				}
				if( _text[i].is_empty() )
					aaa::alphabet::draw_str( "Null" );
				else
					aaa::alphabet::draw_str( _text[i].get() );

			aaa::alphabet::draw_str_end();
		}
	GOL::matrix::pop();
}

void c_bdd_line::draw_multiple()
{
	c_multiple::cur->set_nb( 2);
	c_multiple::cur->align_then_draw( _point );
	c_multiple::cur->align_then_draw( _point+3 );
}
