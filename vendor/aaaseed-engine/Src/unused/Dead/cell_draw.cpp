#include "cell_draw.h"
#ifndef AAA_OURTRUETYPE_H
#	include "truetype/ourtruetype.h"
#endif
#include "draw/shape.h"
#include "ui/alphabet.h"
#include "math/billboard.h"
#include "infrastructure/obj/root.h"
#include "infrastructure/param/param.h"
#include "infrastructure/param/connex.h"

FACTORY_CREATE_V1( c_cell_draw, cell_draw, Cell Draw, cell_draw );

CONST	INT32	CELL_DISTRIB_NB = 2;
CONST	INT32	CELL_SHAPE_NB = 2;

static	CHAR*	str_distrib[CELL_DISTRIB_NB] =
{
	"ON CIRCLE",
	"ON PLANE",
};

static	CHAR*	str_obj_def[CELL_SHAPE_NB] =
{
	"SPHERE",
	"CIRCLE",
};

namespace	n_cell_draw
{
	static	CONST	INT32	BASE_PARAM_NB	= 13;
	static	CONST	INT32	OBJ_PARAM_NB	= 9;
	static	CONST	INT32	TEXT_PARAM_NB	= 8;
	static	CONST	INT32	BRANCH_PARAM_NB	= 14;
	static	CONST	INT32	GROUP_PARAM_NB	= 3;
	static	CONST	INT32	PARAM_NB_MAX =	BASE_PARAM_NB
										+	OBJ_PARAM_NB
										+	TEXT_PARAM_NB
										+	BRANCH_PARAM_NB
										+	GROUP_PARAM_NB;

	static	ST_PARAM	param[PARAM_NB_MAX] =
	{
		ST_PARAM_BOOL_OFF( active )
		ST_PARAM_BOOL_OFF( active_branch_only )

		ST_PARAM_POINT_XYZ( position )
		{	NULL,	PARAM_REAL,	"size",	1, .5,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },

		ST_PARAM_REAL_ONE( size_factor )
		ST_PARAM_REAL_ONE( level_translate )
		ST_PARAM_REAL_ONE( level_radius )
		{	NULL,	PARAM_SYMBOLIC,	"distribution",		1., 0.,		0, CELL_DISTRIB_NB-1,		NULL, str_distrib },
		{	NULL,	PARAM_INT32,	"level_nb",			1., 5,		0, 32, NULL, NULL },
		ST_PARAM_REAL_ZERO( alpha_max )
		ST_PARAM_REAL_ZERO( alpha_min )

		ST_PARAM_GROUP_CLOSED( Obj, OBJ_PARAM_NB )	
			{	NULL,	PARAM_REAL,	"obj_size",		1., .1,		-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
			{	NULL,	PARAM_SYMBOLIC,	"obj_draw_default",		1., 0.,		0, CELL_SHAPE_NB-1,	NULL, str_obj_def },
			{	NULL,	PARAM_INT32,	"obj_nb_u",		1, 16,		1, 512,	NULL, NULL },
			{	NULL,	PARAM_INT32,	"obj_nb_v",		1, 16,		1, 512,	NULL, NULL },
			ST_PARAM_COLOR_RGBGA(obj)

		ST_PARAM_GROUP_CLOSED( Text, TEXT_PARAM_NB )	
			{	NULL,	PARAM_INT32,	"font",			0., 0.,		-1, FONT_MAX_NB-1,				NULL, NULL },
			ST_PARAM_BOOL_OFF( font_outline )
			{	NULL,	PARAM_REAL,		"text_size",	0., 1.,		-PARAM_INFINI, PARAM_INFINI,	NULL, NULL },
			ST_PARAM_COLOR_RGBGA(text)

		ST_PARAM_GROUP_CLOSED( Branch, BRANCH_PARAM_NB )	
			{	NULL,	PARAM_REAL,	"angle_start",	.25, .0,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
			{	NULL,	PARAM_REAL,	"angle_stop",	.75, 1.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },

			ST_PARAM_BOOL_ON( branch_draw )
			ST_PARAM_COLOR_RGBGA(branch)

			ST_PARAM_BOOL_ON( root_draw )
			ST_PARAM_COLOR_RGBGA(root)
	};
}

void	c_cell_draw::param_init_pt()
{
INT32	h = 0;
	param_set_pt( h, b_active_ );
	param_set_pt( h, b_active_only );

	param_set_pt_v3( h, cell_pos_start );
	param_set_pt( h, cell_size_start );
	
	param_set_pt( h, size_factor );
	param_set_pt( h, level_translate );
	param_set_pt( h, level_radius );
	param_set_pt( h, s_distrib_type );
	param_set_pt( h, level_max );
	param_set_pt( h, alpha_max );
	param_set_pt( h, alpha_min );

	++h;
		param_set_pt( h, obj_size );
		param_set_pt( h, s_draw_obj_def );
		param_set_pt( h, obj_nb_u );
		param_set_pt( h, obj_nb_v );
		param_set_pt_v4( h, obj_color_ui );
		param_set_pt( h, obj_color[3] );

	++h;
		param_set_pt( h, s_font );
		param_set_pt( h, b_font_outline );
		param_set_pt( h, text_size );
		param_set_pt_v4( h, text_color_ui );
		param_set_pt( h, text_color[3] );

	++h;
		param_set_pt( h, angle_start );
		param_set_pt( h, angle_stop );

		param_set_pt( h, b_branch_draw );
		param_set_pt_v4( h, branch_color_ui );
		param_set_pt( h, branch_color[3] );

		param_set_pt( h, b_root_draw );
		param_set_pt_v4( h, root_color_ui );
		param_set_pt( h, root_color[3] );

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_cell_draw)
	,list_base_(0)
{
	param_init_with( n_cell_draw::param, n_cell_draw::PARAM_NB_MAX ); // cell_draw_param, CELL_DRAW_PARAM_NB_MAX);
}
EMPTY_DESTRUCTOR(c_cell_draw)

void	c_cell_draw::update()
{
	if( s_font >= 0 )
		font_size_cache = text_size / font_get_size( s_font, b_font_outline);
	scale_v3r( text_color, text_color_ui, text_color_ui[3] );
	scale_v3r( obj_color, obj_color_ui, obj_color_ui[3] );
	scale_v3r( branch_color, branch_color_ui, branch_color_ui[3] );
	scale_v3r( root_color, root_color_ui, root_color_ui[3] );
	prepare_lists();
}

void	c_cell_draw::draw()
{
	if( is_active() )
	{
//		viewport_curve();
		update();

		the_root().cell_draw( cell_size_start, cell_pos_start, level_max );
		GOL::push_attrib( GL_LIGHTING_BIT);
			glDisable(GL_LIGHTING);
			GOL::color_red4();
			c_connex::draw();
		GOL::pop_attrib();
	}
}

void	c_cell_draw::prepare_lists()
{
	if( !list_base_ )
		list_base_ = GOL::gen_lists( CELL_SHAPE_NB );
	GOL::new_list( list_base_, GL_COMPILE );
		draw_sphere( 1., obj_nb_u, obj_nb_v);
	GOL::end_list();  
	GOL::new_list( list_base_+1, GL_COMPILE );
		draw_circle_x( obj_nb_u );
	GOL::end_list();  
}

void	c_cell_draw::draw_obj( REAL	size, REAL* pos )
{
	GOL::push_matrix();
	GOL::translatev( pos);
	GOL::scale( size );
		GOL::call_list( list_base_+s_draw_obj_def );
	GOL::pop_matrix();
}

FINLINE	void	c_cell_draw::draw_text( REAL size, CONST CHAR* CONST txt )
{
REAL tmp;
	tmp = size * obj_size;
	if( s_font < 0 )
	{
		GOL::translate( -tmp * .5, 0, 0 );
		tmp = size * text_size;
		GOL::scale( tmp, tmp, tmp * .4 );
		GOL::rotate_y_deg( -90. );
		n_alphabet::draw_str( (CHAR*)txt );
	}
	else
	{
		REAL size_text;
		g_camera_cur->force_new_matrix();
		billboard::do_x();
		size_text = size * font_size_cache;
		if( s_draw_obj_def == 0 )
			GOL::translate( -tmp * .8, -size_text * .2, 0 );
		GOL::rotate_y_deg( -90. );
		GOL::scale( size_text, size_text, size_text * .4 );
		font_render( (CHAR*)txt, s_font, b_font_outline, .5 );
	}
}

FINLINE	void	c_cell_draw::set_color( REAL* color, INT32 level)
{
REAL	c[4];
	cpy_v3r( c, color);
	c[3] = color[3] * interpolate(  alpha_max, alpha_min, REAL((level_max-level))/REAL(level_max) );
	GOL::color4v( c);
}

FINLINE	void	c_cell_draw::branch_make_pos( REAL* pos, REAL size, INT32 index, INT32 nb_u, INT32 nb_v)
{
	switch( s_distrib_type)
	{
	case 0:
		{
			REAL	a = angle_start + (angle_stop-angle_start)*index/nb_u;
			pos[2] += size*SIN_INT(a);
			pos[1] += size*COS_INT(a);
		}
		break;
	case 1:
		{
			INT32	v = index / nb_u;
			INT32	u = IMOD( index, nb_u );
			pos[2] += size * (2.*REAL(u+.5)/REAL(nb_u) - 1.);
			pos[1] -= size * (2.*REAL(v+.5)/REAL(nb_v) - 1.);
		}
		break;
	}
}

FINLINE	void	c_cell_draw::draw_branches( c_obj_ui* caller, REAL size, REAL* pos, INT32 level)
{
INT32	nb;
INT32	nb_to_draw;

	nb = caller->get_branch_nb();
//hacknow	got rid of it because of get_branch_active_nb() was removed
//	if( b_active_only )
//		nb_to_draw = caller->get_branch_active_nb();
//	else
		nb_to_draw = nb;

	if( nb_to_draw)
		{
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
			radius = size * level_radius * .5;
			size_new = (size * size_factor) / nb_to_draw;
			}
		pos_new[0] = pos[0] + size * level_translate;

		switch( s_distrib_type )
			{
			case 0:
				nb_u = nb_to_draw;
				nb_v = 1;
				size_new = PI * size;
				size_new /= nb_to_draw;
				break;
			case 1:
				{
				REAL tmp;
				tmp = sqrt( REAL(nb_to_draw) );
				nb_u = tmp;
				if( (tmp-nb_u) != 0.)
					nb_u++;
				tmp = REAL(nb_to_draw) / nb_u;
				nb_v = tmp;
				if( (tmp-nb_v) != 0.)
					nb_v++;
				size_new = MAX( nb_u, nb_v);
				size_new = size / size_new;
				}
				break;
			}
		//	draw sub_object
		for( INT32 count = 0, i=0; i<nb; i++)
			{
			obj = caller->get_branch( i);
//hack			if( obj && (!b_active_only || obj->get_active()) )
				{
				pos_new[1] = pos[1];
				pos_new[2] = pos[2];
				branch_make_pos( pos_new, radius, count++, nb_u, nb_v);
				obj->cell_draw( size_new*size_factor, pos_new, level);
				}
			}
		GOL::push_attrib( GL_LIGHTING_BIT);
			glDisable(GL_LIGHTING);
		//	draw down link
		if( b_branch_draw )
			{
			set_color( branch_color, level);
			GOL::begin( GL_LINES);
			for( INT32 count = 0, i=0; i<nb; i++)
				{
				obj = caller->get_branch( i);
//hack				if( obj && (!b_active_only || obj->get_active()) )
					{
					GOL::vertex3v( pos);
					GOL::vertex3v( obj->get_cell_pos() );
					}
				}
			GOL::end();
			}
		//	draw up link
		if( b_root_draw )
			{
			c_obj_ui*	l_root;
			set_color( root_color, level);
			GOL::begin( GL_LINES);
			for( INT32 count = 0, i=0; i<nb; i++)
				{
				obj = caller->get_branch( i);
				l_root = obj->get_root();
				if( obj )
					{
					if ( l_root)
						{
						if( caller != l_root )
							debug_break( "root seems to be wrong here"); 
//hack						if( !b_active_only || obj->get_active() )
							{
							GOL::vertex3v( l_root->get_cell_pos() );
							GOL::vertex3v( obj->get_cell_pos() );
							}
						}
					}
				else
					debug_break( "empty branch"); 
				}
			GOL::end();
			}
		GOL::pop_attrib();
		}
}

void	c_cell_draw::draw( c_obj_ui* caller, INT32 level, CONST CHAR* CONST txt)
{
REAL*	pos;
REAL	size;

	pos = caller->get_cell_pos();
	size = caller->get_cell_size();

	//draw object
	GOL::push_matrix();
		GOL::translatev( pos);
		GOL::scale( size );
		
		set_color( obj_color, level);
		caller->cell_draw_obj();

	//draw text
		set_color( text_color, level);
		GOL::push_attrib( GL_LIGHTING_BIT);
			glDisable(GL_LIGHTING);
			draw_text( size, txt );
		GOL::pop_attrib();
	GOL::pop_matrix();


	//draw tree
	if( --level > 0)
		draw_branches( caller, size, pos, level );
}

c_cell_draw		the_cell_draw;
c_cell_draw*	cell_draw_cur = &the_cell_draw;
