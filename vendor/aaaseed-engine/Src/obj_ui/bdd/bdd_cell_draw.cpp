#include "bdd_cell_draw.h"
#include "truetype/ourtruetype.h"
#include "gol/gol_light.h"
#include "gol/gol_list.h"
#include "draw/shape.h"
#include "ui/alphabet.h"
#include "math/billboard.h"
#include "strnum.h"
#include "draw/render.h"
#include "draw/geo/sphere.h"
#include "ftgl/aaa_ftgl.h"


FACTORY_CREATE_PROP_V1( c_bdd_cell_draw, bdd_cell_draw, Cell Draw, bdd_cell_draw, sub_menu="UI"; );

c_bdd_cell_draw*	c_bdd_cell_draw::cur = nullptr;

enum CELL_DISTRIB : INT32
{
	DISTRIB_CIRCLE = 0,
	DISTRIB_SQUARE,
	DISTRIB_NB
};

static	C_PCHAR_C	str_distrib[DISTRIB_NB] =
{
	"ON CIRCLE",
	"ON PLANE",
};

CONST INT32 CELL_SHAPE_NB = 4;
static	C_PCHAR_C	str_shape_def[CELL_SHAPE_NB] =
{
	"HEXAGON",
	"CIRCLE",
	"DISK",
	"SPHERE",
};

namespace n_bdd_cell_draw
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 16;
	CONSTEXPR INT32 OBJ_PARAM_NB	= 10;
	CONSTEXPR INT32 TEXT_PARAM_NB	= 13;
	CONSTEXPR INT32 BRANCH_PARAM_NB	= 14;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 3;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	OBJ_PARAM_NB
									+	TEXT_PARAM_NB
									+	BRANCH_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_UINT32_INF(		obj_begin_id,		2,1	)
		PARAM_DEF_UINT32_INF(		obj_level_up,		1,0	)
		PARAM_DEF_BOOL_OFF(			active_branch_only )
	
		PARAM_DEF_POINT_XYZ(		position			)
		PARAM_DEF_REAL_INF(			size,				1,.5	)

//		PARAM_DEF_REAL_ONE(			size_factor			)
		PARAM_DEF_REAL_ONE(			level_translate		)
//		PARAM_DEF_REAL_ONE(			level_translate_factor	)
		PARAM_DEF_REAL_ONE(			level_radius		)
		PARAM_DEF_FP32_ONE(			level_tra_factopr	)
		PARAM_DEF_FP32_ZERO(		level_rot_factor	)
		PARAM_DEF_SYMBO_PSTR_ZERO(	distribution,	str_distrib	)
		PARAM_DEF_INT32(			level_nb,			1,5,	1,32	)
		PARAM_DEF_REAL_ONE(			alpha_begin			)
		PARAM_DEF_REAL_ONE(			alpha_end			)

		PARAM_DEF_GROUP_CLOSED( Obj, OBJ_PARAM_NB )
			PARAM_DEF_BOOL_ON(			obj_draw			)
			PARAM_DEF_REAL_ONE(			obj_size			)
			PARAM_DEF_SYMBO_PSTR_ZERO(	obj_shape_default,	str_shape_def )
			PARAM_DEF_INT32(			obj_nb_u,			1, 16,		1, 512	)
			PARAM_DEF_INT32(			obj_nb_v,			1, 16,		1, 512	)
			PARAM_DEF_COLOR_RGBGA(		obj)

		PARAM_DEF_GROUP_CLOSED( Text, TEXT_PARAM_NB )
			PARAM_DEF_BOOL_ON(			text_draw				)
			PARAM_DEF_INT32(			font,					0,1,	-2,aaa::font::FONT_MAX_NB-1	)
			PARAM_DEF_BOOL_ON(			font_outline			)
			PARAM_DEF_POINT_FP32_XYZ(	text_offset				)
			PARAM_DEF_FP32_ONE(			text_size				)
			PARAM_DEF_FP32_ONE(			text_factor_horizontal	)
			PARAM_DEF_COLOR_RGBGA(		text					)

		PARAM_DEF_GROUP_CLOSED( Branch, BRANCH_PARAM_NB )
			PARAM_DEF_REAL_INF(		angle_begin,	.25,0	)
			PARAM_DEF_REAL_INF(		angle_end,		.75,1	)

			PARAM_DEF_BOOL_ON(		branch_draw		)	// branch or root draw achieve the same resume but in two different ways
			PARAM_DEF_COLOR_RGBGA(	branch			)

			PARAM_DEF_BOOL_OFF(		root_draw		)
			PARAM_DEF_COLOR_RGBGA(	root			)

		PARAM_DEF_BOOL_ON( connexion_draw )
	};
}


void	c_bdd_cell_draw::param_init_pt()
{
	INT32	h = 0;
	param_set_pt( h, _obj_id_begin_ui	);
	param_set_pt( h, _obj_level_up_ui	);
	param_set_pt( h, _b_active_only		);

	param_set_pt_3( h, _cell_pos_begin );
	param_set_pt(	h, _cell_size_begin );

//	param_set_pt( h, _size_factor );
	param_set_pt( h, _level_translate		);

	//	param_set_pt( h, _level_translate_factor );
	param_set_pt( h, _level_radius			);
	param_set_pt( h, _level_tra_factor_ui	);
	param_set_pt( h, _level_rot_factor_ui	);
	param_set_pt( h, _s_distrib_type_ui		);
	param_set_pt( h, _level_max_ui			);
	param_set_pt( h, _alpha_begin_ui		);
	param_set_pt( h, _alpha_end_ui			);

	++h;
		param_set_pt(	h, _b_obj_draw );
		param_set_pt(	h, _obj_size );
		param_set_pt(	h, _s_draw_obj_def );
		param_set_pt(	h, _obj_nb_u );
		param_set_pt(	h, _obj_nb_v );
		param_set_pt_4( h, _obj_color_ui );
		param_set_pt(	h, _obj_color[3] );

	++h;
		param_set_pt(	h, _b_text_draw	);
		param_set_pt(	h, _s_font				);
		param_set_pt(	h, _b_font_outline		);
		param_set_pt_3(	h, _text_tra_ui			);
		param_set_pt(	h, _text_size_ui		);
		param_set_pt(	h, _text_factor_hori_ui	);
		param_set_pt_4( h, _text_color_ui		);
		param_set_pt(	h, _text_color[3]		);

	++h;
		param_set_pt( h, _angle_begin );
		param_set_pt( h, _angle_end );

		param_set_pt(	h, _b_branch_draw );
		param_set_pt_4( h, _branch_color_ui );
		param_set_pt(	h, _branch_color[3] );

		param_set_pt(	h, _b_root_draw );
		param_set_pt_4( h, _root_color_ui );
		param_set_pt(	h, _root_color[3] );

	param_set_pt( h, _b_connex_draw );

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_bdd_cell_draw)
//,_list_base		(0)
,_obj_begin	(nullptr)
,_ft_font	(nullptr)
{
	param_init_with( n_bdd_cell_draw::param, n_bdd_cell_draw::PARAM_NB_MAX ); // cell_draw_param, CELL_DRAW_PARAM_NB_MAX);
}
c_bdd_cell_draw::~c_bdd_cell_draw()
{
	if( cur == this )
		cur = nullptr;
}

void	c_bdd_cell_draw::update()
{
	_font_size_cached = _text_size_ui;
	if( _s_font >= 0 )
	{
		//_font_size_cached = _text_size / aaa::font::get_height( _s_font, _b_font_outline );
		CONSTEXPR INT32 BASE_SIZE = 72;
		_ft_font = c_ftfont::get( _s_font, _b_font_outline ? c_ftfont::FTGL_OUTLINE : c_ftfont::FTGL_POLYGON,  BASE_SIZE );
		if( _ft_font )
		{
			_ft_font->get_layout()->SetLineLength( BASE_SIZE * 4096 );	// this make long lines (avoid cr) 
			_font_size_cached *= _ft_font->get_line_height_over_one() ;
		}
	}

	scale_v3( _text_color,   _text_color_ui,   _text_color_ui[3]   );
	scale_v3( _obj_color,    _obj_color_ui,    _obj_color_ui[3]    );
	scale_v3( _branch_color, _branch_color_ui, _branch_color_ui[3] );
	scale_v3( _root_color,   _root_color_ui,   _root_color_ui[3]   );
//	prepare_lists();

	_obj_begin = (c_obj_ui*)c_obj_ui::get_from_id(_obj_id_begin_ui);
	for( INT32 nb = _obj_level_up_ui; nb > 0; --nb )
	{
		if( _obj_begin )
		{
			auto up = _obj_begin->get_root();
			if (up)
				_obj_begin = up;
			else
				break;
		}
	}

	cur = this;
}

void	c_bdd_cell_draw::draw()
{
	if( _obj_begin )
	{
		_obj_begin->set_cell_pos_size_frame( _cell_pos_begin, _cell_size_begin, draw::get_frame() );
		//then draw
		draw_obj_internal( _obj_begin, _level_max_ui );

		if( _b_connex_draw )
		{
			GOL::push_att();	// GL_LIGHTING_BIT );
				GOL::push_lighting( false );
				GOL::push_color3v( GOL::fp32_red );
				c_connex::draw();
			GOL::pop_att();
		}
	}
}

//void	c_bdd_cell_draw::prepare_lists()
//{
//	if( !_list_base )
//		_list_base = GOL::gen_lists( CELL_SHAPE_NB );
//	if( _list_base )
//	{
//		GOL::compile_list( _list_base );
//			draw_sphere( 1., _obj_nb_u, _obj_nb_v );
//		GOL::end_list();
//		GOL::compile_list( _list_base+1 );
//			draw_circle_z( GL_TRIANGLE_FAN, _obj_nb_u );
//		GOL::end_list();
//		GOL::compile_list( _list_base+2 );
//			draw_circle_z( GL_LINE_LOOP, _obj_nb_u );
//		GOL::end_list();
//	}
//}

void	c_bdd_cell_draw::cell_draw_obj_default( REAL CONST size )
{
//	if( _list_base )
//		GOL::call_list( _list_base + s_draw_obj_def );
	switch( _s_draw_obj_def )
	{
	case 0:
		draw_circle_z( GL_LINE_LOOP, 6, size );
		break;
	case 1:
		draw_circle_z( GL_LINE_LOOP, _obj_nb_u, size );
		break;
	case 2:
		draw_circle_z( GL_TRIANGLE_FAN, _obj_nb_u, size );
		break;
	case 3:
		draw_sphere( size, _obj_nb_u, _obj_nb_v );
		break;
	}
}

namespace {

	CHAR	str_id[16] = "(";
	o_str	o_tmp;
}

FINLINE	void	c_bdd_cell_draw::draw_text( c_obj_ui* CONST obj, INT32 CONST level )
{
	INT32 id = obj->get_obj_ui_id();
	{
		CHAR* pt = str_id+1;
		INT32 inc;
		//set_color( _branch_color, level );
		inc = strnum::sprint( pt, 12, id );
		pt += inc;
		*pt = ')';
		//*++pt = '\n'; problem
		*++pt = ' ';
		*++pt = 0;
		o_tmp.set( str_id );
	}

	GOL::push_att();
		GOL::push_lighting( false );
		GOL::push_color();

		set_color( _text_color, level );

		auto name = obj->get_name_symbo().get();
		if( !*name )
			name = obj->get_name_str();
		if( name )
			o_tmp.add( name );
		else
			o_tmp.add( obj->get_class_name() );
		
		if( _s_font < 0 )
		{
			aaa::alphabet::draw_str_xyzv(o_tmp.get(),	_text_tra_ui,	_text_size_ui*_text_factor_hori_ui, _text_size_ui, 2 + _s_font );
	//		GOL::rotate_y_deg( -90. );
	//		GOL::matrix::scale( _text_size );
	//		aaa::alphabet::draw_str( o_tmp.get(), 2 + _s_font );
		}
		else
		{
			if( _ft_font )
			{
				GOL::matrix::translate3v( _text_tra_ui );
				GOL::matrix::scale( _font_size_cached*_text_factor_hori_ui, _font_size_cached, _font_size_cached );
				auto layout = _ft_font->get_layout();
		
				//GOL::matrix::get_matrix_modelview( _mat_view );
				//billboard::do_x();
//				if( _s_draw_obj_def == 0 )
//					GOL::matrix::translate3v( _text_tra );	//* .2 );

				//FTPoint		ft_displacement;
				//_bbox = 
				//layout->BBox( str_in.get(), str_in.get_len(), ft_displacement );
				//layout->BBox( str_in.get(), str_in.get_len(), ft_displacement );
				layout->Render( o_tmp.get(), o_tmp.get_len() ); //, ft_displacement, FTGL::RENDER_ALL );
			}
			//todox64
			//aaa::font::render( o_tmp, _s_font, _b_font_outline, .5, 1.0 );
		}
	GOL::pop_att();
}

FINLINE	void	c_bdd_cell_draw::set_color( FP32* color, INT32 CONST level )
{
	FP32	c[4];
	cpy_v3( c, color);
	c[3] = color[3] * interpolate(  _alpha_begin_ui, _alpha_end_ui, REAL((_level_max_ui-level))/REAL(_level_max_ui) );
	GOL::color4v( c );
}

FINLINE	void	c_bdd_cell_draw::branch_make_pos( REAL* CONST pos, REAL CONST size, INT32 CONST index, INT32 CONST nb_u, INT32 CONST nb_v)
{
	switch( _s_distrib_type_ui)
	{
	case DISTRIB_CIRCLE:
		{
			REAL a = _angle_begin + (_angle_end-_angle_begin)*index/nb_u;
			pos[0] += size*COS_TURN(a);
			pos[1] += size*SIN_TURN(a);
		}
		break;
	case DISTRIB_SQUARE:
		{
			INT32 u = IMOD( index, nb_u );
			INT32 v = index / nb_u;		
			pos[0] += size * (REAL(2*u+1)/REAL(nb_u) - REAL(1));
			pos[1] -= size * (REAL(2*v+1)/REAL(nb_v) - REAL(1));
		}
		break;
	}
}

FINLINE	void	c_bdd_cell_draw::draw_branches( c_obj_ui* CONST caller, REAL CONST size, REAL CONST * CONST pos, INT32 CONST level )
{
	INT32	nb;
	INT32	nb_to_draw;

	nb = caller->get_branch_nb();
	//hacknow	got rid of it because of get_branch_active_nb() was removed
	if( _b_active_only )
		nb_to_draw = caller->get_branch_active_nb();
	else
		nb_to_draw = nb;

	if( !nb_to_draw)
		return;
	//INT32		i;
	//INT32		count;
	REAL		pos_new[3];
	REAL		size_new;
	REAL		radius;
	c_obj_ui*	obj;
	INT32		nb_u;
	INT32		nb_v;

	if( nb_to_draw == 1)
	{
		radius = 0;
		size_new = size;
	}
	else
	{
		radius = REAL(size * _level_radius * .5);
		size_new = size / nb_to_draw;
	}
	pos_new[2] = pos[2] - size * _level_translate;

	switch( _s_distrib_type_ui )
	{
	case DISTRIB_CIRCLE:
		nb_u = nb_to_draw;
		nb_v = 1;
		size_new = REAL(PI * size);
		size_new /= nb_to_draw;
		break;
	default:
	case DISTRIB_SQUARE:
		{
		INT32 tmp = I_CEIL( sqrt(REAL(nb_to_draw)) );
		nb_u = tmp;
		tmp = I_CEIL( REAL(nb_to_draw) / nb_u );
		nb_v = tmp;
		size_new = REAL( MAX( nb_u, nb_v ) );
		size_new = REAL( size / size_new ); 
		}
		break;
	}
	
	size_new *= _level_radius;
	
	//	draw sub_object
	for( INT32 count = 0, i=0; i<nb; ++i )
	{
		obj = caller->get_branch( i );
		if( obj && (!_b_active_only || obj->is_active()) )
		{
			pos_new[0] = pos[0];
			pos_new[1] = pos[1];
			branch_make_pos( pos_new, radius, count++, nb_u, nb_v );

			//store in the object
			obj->set_cell_pos_size_frame( pos_new, size_new, draw::get_frame() );

			//then draw
			draw_obj_internal( obj, level );
		}
	}

	GOL::push_att();
		GOL::push_lighting( false );
		GOL::push_color();

		//	draw down link
		if( _b_branch_draw )
		{
			set_color( _branch_color, level );
			GOL::begin( GL_LINES );
				for( INT32 /*count = 0,*/ i=0; i<nb; ++i )
				{
					obj = caller->get_branch( i );
					if( obj && (!_b_active_only || obj->is_active()) )
					{
						GOL::vertex3v( pos );
						GOL::vertex3v( obj->get_cell_pos() );
					}
				}
			GOL::end();
		}

		//	draw up link
		if( _b_root_draw )
		{
			c_obj_ui*	l_root;
			set_color( _root_color, level );
			GOL::begin( GL_LINES);
				for( INT32/* count = 0,*/ i=0; i<nb; ++i )
				{
					obj = caller->get_branch( i);
					l_root = obj->get_root();
					if( obj )
					{
						if ( l_root )
						{
							if( caller != l_root )
								debug_break( "root seems to be wrong here"); 
						if( !_b_active_only || obj->is_active() )
							{
								GOL::vertex3v( l_root->get_cell_pos() );
								GOL::vertex3v( obj->get_cell_pos() );
							}
						}
					}
					else
						debug_break( "empty branch" ); 
				}
			GOL::end();
		}
	GOL::pop_att();
}

void	c_bdd_cell_draw::draw_obj_internal( c_obj_ui* CONST obj, INT32 level )
{
	REAL CONST * CONST	pos		= obj->get_cell_pos();
	REAL				size	= obj->get_cell_size();

	GOL::matrix::push();
		//draw object
		GOL::matrix::translate3v( pos );
		GOL::matrix::scale( size );
		GOL::matrix::rotate_y( _level_rot_factor_ui * pos[0] );
		GOL::matrix::rotate_x( -_level_rot_factor_ui * pos[1] );
//		GOL::matrix::rotatev( _level_rot_factor_ui, -pos[1], pos[0] );

		if( _b_obj_draw )
		{
			set_color( _obj_color, level );
			obj->cell_draw_obj( _obj_size );
		}

	//draw text
		if( _b_text_draw )
			draw_text( obj, level );

	GOL::matrix::pop();

	//draw tree
	if( --level > 0 )
		draw_branches( obj, size, pos, level );
	//draw_branches( obj, 1., zero_v4fp32, level );
}
