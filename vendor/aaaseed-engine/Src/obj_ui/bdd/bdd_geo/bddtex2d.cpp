#include "obj_ui/bdd/bdd_geo/bddtex2d.h"
#include "ui/alphabet.h"
#ifndef AAA_OURTRUETYPE_H
#	include "truetype/ourtruetype.h"
#endif
#include "draw/model.h"
#include "gol/gol_matrix.h"
#include "draw/axe.h"
#include "obj_ui/deformer/def_node.h"
#ifdef	WIN32
#	include "platform/win32/win_dlg_util.h"
#endif

FACTORY_CREATE_PROP_V1( c_bdd_tex2d, bdd_tex2d, Text 2D, tex2d, sub_menu="Geometry"; );

namespace {
	INT32 CONSTEXPR	TYPE_MAX_NB = 5;	
	C_PCHAR_C	type_str[TYPE_MAX_NB] =
	{
		"GLUT_LINE",
		"MAA_LINE",
		"FONT_OUTLINE",
		"FONT_FILL",
		"FONT_FILL_THEN_OUTLINE",
	};
};

namespace n_bdd_text2d
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 15 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		BDD_BASE_PARAMS
		
		PARAM_DEF_POINT_XYZ(	origin				)
		PARAM_DEF_AXE_X(		axe					)

		PARAM_DEF_SYMBO_PSTR(	type,				0,3,	type_str					)
		PARAM_DEF_INT32(		font,				0,1,	0,aaa::font::FONT_MAX_NB-1	)
		PARAM_DEF_REAL_ONE(		align_horizontal	)
		PARAM_DEF_REAL_ONE(		align_vertical		)
		PARAM_DEF_REAL_ONE(		interline			)
		PARAM_DEF_BOOL_OFF(		draw_point			)

		PARAM_DEF_BOOL_OFF(		generate_trig		)
		PARAM_DEF_INT32(		generate_index,		1,0,	0,99	)
		PARAM_DEF_BOOL_LOCKED(	generating			)
		PARAM_DEF_BOOL_LOCKED(	generate_done		)

		PARAM_DEF_STR(			text				)
	};
}

static	CHAR	_generate_buf[ aaa::dialog::STR_LEN_MAX ];
extern	bool	b_str_generating;
extern	bool	b_str_generate_done;

void	c_bdd_tex2d::param_init_pt()
{
	INT32	h = param_init_pt_geo();

	param_set_pt_3( h, _origin );
	param_set_pt( h, _s_axe);

	param_set_pt( h, _s_type );
	param_set_pt( h, _s_font );
	param_set_pt( h, _align_hori );
	param_set_pt( h, _align_vert );
	param_set_pt( h, _interline );
	param_set_pt( h, _b_draw_point_ui );

	param_set_pt( h, _b_generate_trig_ui );
	param_set_pt( h, _s_generate );
	param_set_pt( h, b_str_generating );
	param_set_pt( h, b_str_generate_done );

	param_set_pt( h, _text);

	err_param_init_pt( h );
}

void c_bdd_tex2d::init()
{
	_text.erase();
	_point = nullptr;
	_point_deformed = nullptr;
	_point_nb = 0;
	_s_type_last = -1;
	_s_font_last = -1;
}

CONSTRUCTOR_CREATE( c_bdd_tex2d )
{
	param_init_with( n_bdd_text2d::param, n_bdd_text2d::PARAM_NB_MAX ); // bdd_tex2d_param, BDD_TEX2D_PARAM_NB_MAX);

	init();
}
EMPTY_DESTRUCTOR( c_bdd_tex2d )

void c_bdd_tex2d::dealloc_point()
{
	if( _point_nb )
	{
		SAFE_DELETE_ARRAY( _point );
		_point_deformed = nullptr;
		_point_nb = 0;
	}
}

void c_bdd_tex2d::set_text( C_PCHAR_C in )		{	_text.set( in );	}
void c_bdd_tex2d::add_text( C_PCHAR_C in )		{	_text.add( in );	}

void c_bdd_tex2d::set_text( o_str CONST & in )	{	_text.set( in );	}
void c_bdd_tex2d::add_text( o_str CONST & in )	{	_text.add( in );	}

FINLINE	void c_bdd_tex2d::alloc_point( INT32 nb_in )
{
	if( _point_nb < nb_in )
	{
		dealloc_point();
		_point = new  REAL[ nb_in * 3 * 2 ];
		if( _point )
		{
			_point_nb = nb_in;
			_point_deformed = _point + nb_in * 3;
		}
	}
}


extern	void th_generate_str( INT32 s_generate, CHAR* buf, INT32 size );

/*	INT32	s_type;
	INT32	s_type_last;
	INT32	s_font;
	INT32	s_font_last;
	REAL	font_size_over_one;
*/

void c_bdd_tex2d::update()
{
	c_model::cur->get_size_v3( _size );
	if( _s_type >= 2 )
	{
		if( _s_font_last != _s_font || _s_type_last != _s_type )
		{
			_s_font_last = _s_font;
			_s_type_last = _s_type;
		}
		c_model::cur->get_size_scaled_v3( _size, OVER_ONE( aaa::font::get_height( _s_font, _s_type == 2 ) ) );
	}
	else
		c_model::cur->get_size_v3( _size );

	alloc_point( _text.get_len() + 1 );

	aaa::font::fill_pos( _point_deformed, _text.get(), _s_font, _s_type == 2, _align_hori );
	if( _point )
	{
		//INT32	i;
		REAL*	src = _point_deformed;
		REAL*	dst = _point;
		INT32	i_u, i_v;

		axe_build_index( i_u, i_v, _s_axe );

		for( INT32 i = _text.get_len() + 1; i > 0; --i )
		{
			*(dst + i_u) = _origin[ i_u ] + *src++ * _size[ 0 ];
			*(dst + i_v) = _origin[ i_v ] + *src++ * _size[ 1 ];
			*(dst + _s_axe) = _origin[ _s_axe ];
			dst += 3;
		}
	}

	c_def_node* def_cur = c_def_node::get_cur();
	if( def_cur )
		def_cur->apply( _point_deformed, _point, _text.get_len() + 1 );

	if( _b_generate_trig_ui )
	{
		_b_generate_trig_ui = false;
		if( !b_str_generating )
			th_generate_str( _s_generate, _generate_buf, aaa::dialog::STR_LEN_MAX );
	}
	if( b_str_generate_done )
	{
		_text.set( _generate_buf );
		b_str_generate_done = false;
	}

}

void c_bdd_tex2d::draw()
{
	if( !_text.is_empty() )
	{
		bool	b_outline = ( _s_type == 2 );
		if( c_def_node::get_cur()->is_deforming() )
		{
			//INT32 i;
			INT32 i_max = _text.get_len();
			REAL* pt = _point_deformed;
			UINT8 ch;
			for( INT32 i = 0; i < i_max; ++i )
			{
				ch = *(_text.get() + i);
				if( ch > 31 )
				{
					GOL::matrix::push();
						GOL::matrix::translate3v( pt );
						GOL::matrix::rotate_align_on_z( _s_axe );
						GOL::matrix::scale3v( _size );
						aaa::font::render_char( ch, _s_font, b_outline );
						if( _s_type == 4 )
						{
							GOL::matrix::pop();
							GOL::matrix::push();
							GOL::matrix::translate3v( pt);
							GOL::matrix::rotate_align_on_z( _s_axe );
							GOL::matrix::scale3v( _size );
							aaa::font::render_char( ch, _s_font, true );
						}
					GOL::matrix::pop();
				}
				pt += 3;
			}
		}
		else
		{
			GOL::matrix::push();
				GOL::matrix::translate3v( _origin );
				GOL::matrix::rotate_align_on_z( _s_axe );

				GOL::matrix::scale3v( _size );
				if( _s_type >= 2 )
				{
					aaa::font::render( _text.get(), _s_font, b_outline, _align_hori, _interline );
					if( _s_type == 4 )
					{
						GOL::matrix::pop();
						GOL::matrix::push();
						GOL::matrix::translate3v( _origin );
						GOL::matrix::rotate_align_on_z( _s_axe );
						GOL::matrix::scale3v( _size );
						aaa::font::render( _text.get(), _s_font, true, _align_hori, _interline );
					}
				}
				else
				{
					aaa::alphabet::draw_str( _text.get(), _s_type ? 0 : 1 );
				}
			GOL::matrix::pop();
		}

		if( _b_draw_point_ui )
		{
			//INT32 i;
			REAL* pt = c_def_node::get_cur()->is_deforming() ? _point_deformed : _point;
			for( INT32 i = _text.get_len() + 1; i > 0; --i )
			{
				GOL::matrix::push();
					GOL::matrix::translate3v( pt );
					GOL::matrix::scale3v( _size );
					n_axe::draw_null_3d_list();
				GOL::matrix::pop();
				pt += 3;
			}
		}
	}
}
