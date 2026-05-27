#include "lcp.h"
#include "infrastructure/data/datacube.h"
#include "ui/aaa_menu.h"
#include "ui/seedmenu.h"
#include "ui/flatland.h"
#include "ui/keyboard.h"
		 
#include "draw/aaa_glut.h"
#ifndef AAA_OURTRUETYPE_H
#	include "truetype/ourtruetype.h"
#endif
#include "time/ourtime.h"
#include "file/file_csv.h"
#include "obj_ui/com/net.h"
#include "infrastructure/viewport.h"
#include "infrastructure/param/param_st.h"
		 
#include <vector>

FACTORY_CREATE_V1( c_lcp, lcp, Lcp, lcp );

//	DEPARTEMENT
#define	REG_NB	24
#define	DEP_NB	106
#define	CIRC_NB	577

#define	FX3_LEVEL		-3
#define	FX2_LEVEL		-2
#define	FX1_LEVEL		-1
#define	NONE_LEVEL		0
#define	FRANCE_LEVEL	1
#define	REG_LEVEL		2
#define	DEP_LEVEL		3
#define	CIRC_LEVEL		4
#define	RESULT_LEVEL	5
#define	LEVEL_NB		5

#define	REG_CH	1
#define	DEP_CH	2
#define	CIRC_CH	3

CHAR	str_name[256];

#define	CIRC_FRANCE	0
#define	CIRC_PARTI	-1
#define	CIRC_TITRE	-2
FINLINE	BOOL	is_valid_circ( INT32 in)	{ return (1 <= in) && (in <= CIRC_NB); }

class	c_reg
{
public:
	INT32	dep_nb;
	INT32	deps[32];
};

class	c_dep
{
public:
	INT32	reg;
	INT32	circ_nb;
	INT32	first;
	INT32	last;
};

class	c_circ
{
public:
	INT32	dep;
};

class	c_reg	regs[REG_NB+1];
class	c_dep	deps[DEP_NB+1];
class	c_circ	circs[CIRC_NB+1];


c_lcp*	lcp;

namespace n_lcp
{
	static	CONST INT32	BASE_PARAM_NB	=	3;
	static	CONST INT32	REG_PARAM_NB	=	2;
	static	CONST INT32	DEP_PARAM_NB	=	6;
	static	CONST INT32	CIRC_PARAM_NB	=	1;
	static	CONST INT32	CAMERA_PARAM_NB	=	1;
	static	CONST INT32	DIF_PARAM_NB	=	9;
	static	CONST INT32	GROUP_PARAM_NB	=	5;
	static	CONST INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
										+	REG_PARAM_NB
										+	DEP_PARAM_NB
										+	CIRC_PARAM_NB
										+	CAMERA_PARAM_NB
										+	DIF_PARAM_NB
										+	GROUP_PARAM_NB;

	static ST_PARAM	param[PARAM_NB_MAX] =
	{
		ST_PARAM_BOOL_OFF( active )
		{	NULL,	PARAM_INT32,	"level",					2., 1.,	1., 5., NULL, NULL },
		{	NULL,	PARAM_REAL,		"level_real",				1., 0., -PARAM_INFINI, PARAM_INFINI, NULL, NULL },

		ST_PARAM_GROUP( Region, REG_PARAM_NB )
			ST_PARAM_BOOL_OFF( Region_visibility )
			{	NULL,	PARAM_INT32,	"Region_numero",			2., 1.,	1., REG_NB, NULL, NULL },
		//	{	NULL,	PARAM_INT32,	"Region_name",				2., 1.,	1., 22., NLL, NULL },

		ST_PARAM_GROUP( Departement, DEP_PARAM_NB )
			ST_PARAM_BOOL_OFF( Departement_visibility )
			{	NULL,	PARAM_INT32,	"Departement_nb_in_region",	1., 0.,	0., 16., NULL, NULL },
			{	NULL,	PARAM_INT32,	"Departement_numero",		2., 1.,	1., DEP_NB, NULL, NULL },
			{	NULL,	PARAM_INT32,	"Departement_Circonscription_first",		2., 1.,	1., CIRC_NB, NULL, NULL },
			{	NULL,	PARAM_INT32,	"Departement_Circonscription_last",		2., 1.,	1., CIRC_NB, NULL, NULL },
			{	NULL,	PARAM_INT32,	"Departement_Circonscription_nb",		2., 1.,	1., CIRC_NB, NULL, NULL },
		//	{	NULL,	PARAM_INT32,	"Departement_name",			2., 1.,	1., 1000., NULL, NULL },

		ST_PARAM_GROUP( Circonscription, CIRC_PARAM_NB )
			{	NULL,	PARAM_INT32,	"Circonscription_numero",2., 1.,	1., CIRC_NB, NULL, NULL },

		ST_PARAM_GROUP( Camera, CAMERA_PARAM_NB )
			{	NULL,	PARAM_REAL,	"camera_freq",	0., 1.,		-PARAM_INFINI, PARAM_INFINI, NULL, NULL },

		ST_PARAM_GROUP( Diffusion, DIF_PARAM_NB )
			ST_PARAM_BOOL_OFF( dif )
			ST_PARAM_BOOL_OFF( dif_use_list )
			ST_PARAM_BOOL_OFF( dif_auto )
			ST_PARAM_BOOL_OFF( dif_trig )
			{	NULL,	PARAM_INT32,	"font",			1., 0.,		1., 32, NULL, NULL },
			{	NULL,	PARAM_REAL,		"font_size",	2., 16.,	1., PARAM_INFINI, NULL, NULL },
			{	NULL,	PARAM_INT32,	"circ_ask",		1., 0.,		-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
			{	NULL,	PARAM_INT32,	"dif_start",	1., 0.,		-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
			ST_PARAM_BOOL_ON( dif_draw )
		};
}

void	c_lcp::param_init_pt()
{
INT32	h=0;
	param_set_pt( h, get_pt_active() );
	param_set_pt( h, level);
	param_set_pt( h, level_real);

	h++;
	param_set_pt( h, b_reg);
	param_set_pt( h, reg_cur);

	h++;
	param_set_pt( h, b_dep);
	param_set_pt( h, dep_nb);
	param_set_pt( h, dep_cur);
	param_set_pt( h, circ_first);
	param_set_pt( h, circ_last);
	param_set_pt( h, circ_nb);

	h++;
	param_set_pt( h, circ_cur);

	h++;
	param_set_pt( h, camera_freq);

	h++;
	param_set_pt( h, b_dif);
	param_set_pt( h, b_use_list);;
	param_set_pt( h, b_auto);
	param_set_pt( h, b_dif_trig);
	param_set_pt( h, s_font);
	param_set_pt( h, font_size);
	param_set_pt( h, asked );
	param_set_pt( h, dif_start);
	param_set_pt( h, b_dif_draw);
//	param_set_pt( h, iq_dif);

	err_param_init_pt(h);
}

void	c_lcp::init()
{
	b_first = TRUE;
	level = FRANCE_LEVEL;
}

void	c_lcp::dif_init()
{
	dif.clear();
	dif.push_back( 0);
	dif.push_back( 0);
	iq_ui = 0;
}

void	c_lcp::dif_remove()
{
	if( 0 < iq_ui && iq_ui < INT32(dif.size())-1 )
		{
		dif.erase( dif.begin() + iq_ui );
		if ( iq_ui >= INT32(dif.size())-1 )
			--iq_ui;
		}
}

void	c_lcp::dif_add()
{
	switch( level)
		{
		case REG_LEVEL:
			dif_add_reg(reg_cur);
			break;
		case DEP_LEVEL:
			dif_add_dep(dep_cur);
			break;
		case CIRC_LEVEL:
		case RESULT_LEVEL:
			dif_add_circ(circ_cur);
			break;
		default:
			dif_add_circ(asked );
			break;
		}
}

void	c_lcp::dif_add_reg(INT32 reg)
{
c_reg*	p_reg = &regs[reg]; 
//INT32	i;
INT32	nb = p_reg->dep_nb;

	for( INT32 i = 1; i <= nb; ++i )
		dif_add_dep( p_reg->deps[i] );
}

void	c_lcp::dif_add_dep(INT32 dep)
{
INT32	i = deps[dep].first;
INT32	last = deps[dep].last;
	for( ; i<=last; ++i )
		dif_add_circ(i);
}

void	c_lcp::dif_add_circ(INT32 circ)
{
	if( !is_valid_circ(circ) || datacube_def.get_real( CIRC_CH, circ, 11) )
		{
		//if( dif.size() )
			iq_ui++;
		dif.insert( dif.begin() + iq_ui, circ);
		}
	else
		Beep( 200, 10);
}

#define	DIF_LEN_MAX	(CIRC_NB*5)
void	c_lcp::dif_save()
{
REAL*	data;
//INT32	i;
INT32	nb;
	nb = dif.size()+1;
	if( nb >= DIF_LEN_MAX )
		BOX_ERR( "liste de dif trop longue" );
	data = (REAL*) MALLOC( sizeof(REAL)*nb );
	if( data )
		{
		CHAR	fname[_MAX_PATH];
		BOOL	b_need_save = FALSE;

		strcpy( fname, get_my_filename() );
		strcat( fname, ".dif.csv");

			{
			REAL*	pt = data;
			--nb;
			*pt++ = (REAL) nb;

			for( INT32 i = 0; i < nb; ++i )
				{
				*pt++ = (REAL)dif.at( i );
				}
			}
		file_csv::write_float( fname, data, 1, nb+1);
		FREE_AND_NULL(data);
		}
}

void	c_lcp::dif_load()
{
REAL*	data;
//INT32	i;
INT32	nb = 0;
	dif.clear();
	iq_ui = -1;
	data = (REAL*) MALLOC( sizeof(REAL)*DIF_LEN_MAX );
	if( data )
		{
		CHAR	fname[_MAX_PATH];
		BOOL	b_need_save = FALSE;

		strcpy( fname, get_my_filename() );
		strcat( fname, ".dif.csv");
		if( file_csv::read_float( fname, data, 1, DIF_LEN_MAX) )
			{
			REAL*	pt = data;
			nb = (INT32)*pt++;

			for( INT32 i = 0; i < nb; ++i )
				{
				dif_add_circ( (INT32)*pt++);
				}
			}
		FREE_AND_NULL(data);
		}
	if( nb < 2 )
		dif_init();
}

AAA_ERR	c_lcp::save_do_after( C_PCHAR_C filename )
{
	dif_save();
	return AAA_OK;
}


AAA_ERR	c_lcp::load_do_after( C_PCHAR_C filename )
{
	dif_load();
	return AAA_OK;
}

//extern	void n_axe::axe_3D_draw();
void	c_lcp::draw_on_top( INT32 x_size, INT32 y_size, INT32 x_split )
{
REAL	size = 1.;
bool	b_outline = false;	//(s_type==2);
vector<INT32>::iterator	it;
INT32	circ;
INT32	dep;
INT32	reg;
INT32	i;

INT32	reg_queue;
INT32	dep_queue;

CHAR	str[256];

	if( viewport_render.get_x() == 0 ||  net->is_slave() || !b_dif_draw )
		return;
	GOL::viewport( 0, 0, x_split, y_size);
//	rendering_set_ortho( 100., 100. );

	GOL::set_matrix_projection();
	GOL::load_identity();
	GOL::set_matrix_modelview();
	GOL::load_identity();

	glOrtho( 0, x_split, -y_size, 0, -15, 15);
//	rendering_ortho_draw_vline( 0, x_right_char, 32);
//	rendering_ortho_draw_hline( 0, pwin->get_h(), 32);
//	GOL::translate( 0., pwin->get_h(), 0.);
//	GOL::translate( 1., 1., 0.);
//	axe_3D_draw();

	GOL::push_matrix();
		GOL::scale( font_size/font_get_size( s_font, b_outline ) );

		GOL::color_white3();
		font_render_direct( "", s_font, b_outline, 0.);

		if( b_dif)
			{
			GOL::color3( .5+SIN_INT(n_aaatime::get())*.5, 0, 0);
			font_render_direct( "Diffusion   Diffusion   Diffusion  Diffusion", s_font, b_outline, 0.);
			if( is_valid_circ(asked ) )
				{
				GOL::color3( .3, .3, 1.);
				make_reg_name( str, reg_ask );
				font_render_direct( str, s_font, b_outline, 0.);
				reg_queue = reg_cur;

				GOL::color3( .6, .6, 1.);
				strcpy( str, "   "),
				make_dep_name( str+3, dep_ask );
				font_render_direct( str, s_font, b_outline, 0.);
				dep_queue = dep_cur;

				GOL::color_white3();
				strcpy( str, "      "),
				make_circ_name( str+6, asked );
				font_render_direct( str, s_font, b_outline, 0.);
				}
			else
				{
				GOL::color_white3();
				if ( asked == CIRC_FRANCE )
					{
					font_render_direct( "France", s_font, b_outline, 0.);
					}
				else if( asked == CIRC_PARTI )
					{
					font_render_direct( "Assembl�e", s_font, b_outline, 0.);
					}
				else if( asked == CIRC_TITRE )
					{
					font_render_direct( "Carton titre", s_font, b_outline, 0.);
					}
				}
			GOL::color3( .75+SIN_INT(n_aaatime::get())*.25, 0, 0);
			font_render_direct( "___________________________________", s_font, b_outline, 0.);
			font_render_direct( "", s_font, b_outline, 0.);
			}
		else
			{
			}
		if( b_dif)
			i = iq_dif+1;
		else
			i = 0;
		it = dif.begin()+i;
		for( ; it != dif.end(); ++it, ++i )
			{
			circ = *it;
			if( is_valid_circ( circ) )
				{
				dep = circs[circ].dep;
				reg = deps[dep].reg;
				//Reg
				if( reg_queue != reg )
					{
					GOL::color3( .3, .3, 1.);
					reg_queue = reg;
					make_reg_name( str, reg );
					font_render_direct( str, s_font, b_outline, 0.);
					}
				//Dep
				if( dep_queue != dep )
					{
					GOL::color3( .6, .6, 1.);
					dep_queue = dep;
					strcpy( str, "   "),
					make_dep_name( str+3, dep );
					font_render_direct( str, s_font, b_outline, 0.);
					}
				//Circ
				if( !b_dif && iq_ui == i )
					GOL::color_red3();
				else
					GOL::color_white3();
				strcpy( str, "      "),
				make_circ_name( str+6, circ );
				font_render_direct( str, s_font, b_outline, 0.);
				}
			else
				{
				if( !b_dif && iq_ui == i )
					GOL::color_red3();
				else
					GOL::color_white3();
				if ( circ == CIRC_FRANCE )
					{
					font_render_direct( "France", s_font, b_outline, 0.);
					}
				else if( circ == CIRC_PARTI )
					{
					font_render_direct( "Assembl�e", s_font, b_outline, 0.);
					}
				else if( circ == CIRC_TITRE )
					{
					font_render_direct( "Carton titre", s_font, b_outline, 0.);
					}
				}
			if( !b_dif && iq_ui == i )
				{
				GOL::color_green3();
				switch( level)
					{
					case REG_LEVEL:
						make_reg_name( str, reg_cur );
						break;
					case DEP_LEVEL:
						strcpy( str, "   "),
						make_dep_name( str+3, dep_cur );
						break;
					case CIRC_LEVEL:
					case RESULT_LEVEL:
						strcpy( str, "      "),
						make_circ_name( str+6, circ_cur );
						break;
					default:
						if ( asked == CIRC_FRANCE )
							{
							strcpy( str, "France"); 
							}
						else if( asked == CIRC_PARTI )
							{
							strcpy( str, "Assembl�e");
							}
						else if( asked == CIRC_TITRE )
							{
							strcpy( str, "Carton titre");
							}
						break;
					}
				font_render_direct( str, s_font, b_outline, 0.);
				}
			}
	GOL::pop_matrix();

/*	GOL::push_matrix();
CHAR*	text="1 toto toto toto\n";
		GOL::color_white3();
//		GOL::translatev( origin);
//		GOL::rotate_xyz( .3, .3, .3);
//		GOL::rotate_align_on_x( 2);
//				scale_v3r( size, 1./font_get_size( s_font, b_outline ) );
//			GOL::scalev( size);
		GOL::scale( 20./font_get_size( s_font, b_outline ) );
		font_render_direct( text, s_font, b_outline, 0.);
		GOL::color_red3();
		font_render_direct( text, s_font, b_outline, 0.);
		GOL::color_green3();
		font_render_direct( "test1 ", s_font, b_outline, 0.);
		font_render_direct( "test2", s_font, b_outline, 0.);
		font_render_direct( "test3", s_font, b_outline, 0.);
	GOL::pop_matrix();
*/
}

CONSTRUCTOR_CREATE(c_lcp)
{
	param_init_with( n_lcp::param, n_lcp::PARAM_NB_MAX ); // lcp_param, LCP_PARAM_NB_MAX);
	init();
}

c_lcp::~c_lcp()
{
}

void	c_lcp::reset()
{
}

REAL	c_lcp::dif_next()
{
REAL	delay = 1;
	if( net->is_master() )
		{
		if ( b_auto )
			{
			if( b_reading )
				{
				b_reading = FALSE;
				if( b_use_list )
					{
					iq_dif++;
					if( iq_dif >= INT32(dif.size()) )
						{
						iq_dif = 0;
						}
					asked = dif.at(iq_dif);
					if ( asked == CIRC_PARTI )
						{
						level = FRANCE_LEVEL;
						}
					else if ( asked == CIRC_TITRE )
						{
						level = FX3_LEVEL;
						}
					}
				else
					{
					asked++;
					if( asked > CIRC_NB )
						asked = 0;
					}
//				level_ask = RESULT_LEVEL;
//							camera_freq = 0;
				}
			else
				{
				b_reading = TRUE;
				if ( level == RESULT_LEVEL )
					delay = 6;
				else if( level == FRANCE_LEVEL)
					delay = 2;
				else
					delay = 3;
				}
			}
		else if( b_dif_trig && (!is_valid_circ(asked ) || asked == circ_cur) && n_aaatime::get() > time_next_trig )
			{
			time_next_trig = n_aaatime::get() + 2.;
			iq_dif++;
			if( iq_dif >= INT32(dif.size()) )
				{
				iq_dif = INT32(dif.size())-1;
				b_dif= FALSE;
				}
			delay = 0;
			}
		}
	return delay;
}

void	c_lcp::update()
{
	if( !is_active() )
		return;

	if( b_first )
		{
		b_dif = FALSE;
		b_first = FALSE;
		}

	if( b_dif )
		{
		if( !b_dif_last)
			{	//	the dif start
			iq_dif = dif_start;
			time_next = n_aaatime::get();
			time_next_trig = n_aaatime::get();
			asked = dif_start;
			b_reading = FALSE;
			set_circ( CIRC_FRANCE);		
			level = FRANCE_LEVEL;
			}
		if( net->is_master() )
			{
			if( iq_dif >= dif.size() )
				iq_dif = dif.size()-1;
			}
		set_circ_ask( asked );
		if(  n_aaatime::get() > time_next )
			{		
			REAL delay = 1;
//			camera_freq = 1;
			switch( level)
				{
				case RESULT_LEVEL:
					if( asked != circ_cur )
						{	//remonter
						level = CIRC_LEVEL;
						}
					else	//pret
						delay = dif_next();
					break;
				case CIRC_LEVEL:
					if( dep_ask != dep_cur )
						{	//remonter
						level = DEP_LEVEL;
						delay = 2;
						}
					else if( asked != circ_cur )
						{	//changer circ
						set_circ( asked );
						}
					else
						{	//descendre
						level = RESULT_LEVEL;
						}
					break;
				case DEP_LEVEL:
					if( reg_ask != reg_cur )
						{	//remonter
						level = REG_LEVEL;
						delay = 2.;
						}
					else if( dep_ask != dep_cur )
						{	//changer dep
						set_circ( asked );
						delay = 2;
						}
					else
						{	//descendre
						level = CIRC_LEVEL;
						set_circ( asked );
						}
					break;
				case REG_LEVEL:
					if( !is_valid_circ(asked ) )
						{	//remonter
						level = FRANCE_LEVEL;
						}
					else if( reg_ask != reg_cur )
						{	//changer reg
						set_circ( asked );
/*							{
						c_bdd_tri*	tri = c_bdd_tri::get_from_channel( LEVE_REG);
						REAL	a[3];
						REAL	b[3];
						if( tri )
							{
							tri->get_origin_from_control( a, reg_ask );
							tri->get_origin_from_control( b, reg_cur );
							g_camera_cur = dist_v3r( a, b);
							if( g_camera_cur != 0 )
								g_camera_cur =
							g_camera_cur =+ 1;
							}
						}
*/							delay = 2;
						}
					else
						{	//descendre
						level = DEP_LEVEL;
						set_circ( asked );
						delay = 2;
						}
					break;
				case FRANCE_LEVEL:
					if( is_valid_circ(asked ) )
						{	//descendre
						level = REG_LEVEL;
						set_circ( asked );
						delay = 2;
						}
					else	//pret
						{
						if( asked == CIRC_FRANCE )
							{
							delay = dif_next();
							}
						else if ( asked == CIRC_PARTI )
							{
							level = NONE_LEVEL;
							}
						else if ( asked == CIRC_TITRE )
							{
							level = NONE_LEVEL;
							}
						}
					break;
				case NONE_LEVEL:
					if( is_valid_circ(asked ) || asked == CIRC_FRANCE )
						{	//descendre
						level = FRANCE_LEVEL;
						set_circ( asked );
						}
					else	//pret
						{
						if ( asked == CIRC_PARTI )
							{
							level = FX1_LEVEL;
							delay = 2;
							}
						else if ( asked == CIRC_TITRE )
							{
							level = FX1_LEVEL;
							delay = 2;
							}
						}
					break;
				case FX1_LEVEL:
					if( is_valid_circ(asked ) || asked == CIRC_FRANCE )
						{	//descendre
						level = NONE_LEVEL;
						set_circ( asked );
						}
					else	//pret
						{
						if ( asked == CIRC_PARTI )
							{
							level = FX2_LEVEL;
							}
						else if ( asked == CIRC_TITRE )
							{
							level = FX2_LEVEL;
							}
						}
					break;
				case FX2_LEVEL:
					if( is_valid_circ(asked ) || asked == CIRC_FRANCE )
						{	//descendre
						level = FX1_LEVEL;
						set_circ( asked );
						}
					else	//pret
						{
						if ( asked == CIRC_TITRE )
							{
							level = FX3_LEVEL;
							}
						else
							delay = dif_next();
						}
					break;
				case FX3_LEVEL:
					if( asked != CIRC_TITRE )
						{	//descendre
						level = FX2_LEVEL;
						set_circ( asked );
						delay = 3;
						}
					else
						delay = dif_next();
					break;
				}
			time_next = n_aaatime::get()+delay;
			}
/*		else */
		}
	circ_first = deps[dep_cur].first;
	circ_last = deps[dep_cur].last;
	circ_nb = deps[dep_cur].circ_nb;

	b_dif_last = b_dif;
//	reg_obj_index = datacube_def.get_real( 1, reg_cur, 2)	;
}



void	MAACALLBACK	lcp_menu_do( int value)
{
	MENU_STORE( lcp_menu_do, value);
	lcp->menu_do(value);
}


void	c_lcp::set_reg( INT32 value)
{
	reg_cur = WRAP_ID( value, REG_NB );
	dep_cur = regs[reg_cur].deps[1];
	circ_cur = deps[dep_cur].first;
}

void	c_lcp::set_dep( INT32 value)
{
	dep_cur = WRAP_ID( value, DEP_NB );
	circ_cur = deps[dep_cur].first;
	reg_cur = deps[dep_cur].reg;
}

void	c_lcp::set_circ( INT32 value)
{
	if ( value > 577 )
		value = 1;
	if( is_valid_circ(value) )
		{
		circ_cur = WRAP_ID( value, CIRC_NB );
		dep_cur = circs[circ_cur].dep;
		reg_cur = deps[dep_cur].reg;
		}
	else
		{
		asked = value;
		}
		
}

void	c_lcp::set_circ_ask( INT32 value)
{
	if( is_valid_circ(value) )
		{
		asked = WRAP_ID( value, CIRC_NB );
		dep_ask = circs[asked].dep;
		reg_ask = deps[dep_ask].reg;
//		level_ask = RESULT_LEVEL;
		}
	else
		{
		asked = value;
		dep_ask = 0;
		reg_ask = 0;
//		level_ask = FRANCE_LEVEL;
		}
}


void	c_lcp::menu_do( INT32 value)
{
	if( value < 1000 )
		level = CLAMP( value, 1, LEVEL_NB);
	else if( INSIDE( value, REG_LEVEL*1000, REG_LEVEL*1000+999) )
		{
		level = REG_LEVEL;
		set_reg( value - REG_LEVEL*1000);
		}
	else if( INSIDE( value, DEP_LEVEL*1000, DEP_LEVEL*1000+999) )
		{
		level = DEP_LEVEL;
		set_dep( value - DEP_LEVEL*1000);
		}
	else if( INSIDE( value, CIRC_LEVEL*1000, CIRC_LEVEL*1000+999) )
		{
		level = CIRC_LEVEL;
		set_circ( value - CIRC_LEVEL*1000);
		}
}


void	c_lcp::menu_init()
{
	datacube_def.load( "pref/default");
	datacube_def.update();
}

void	c_lcp::data_init()
{
INT32	circ;
INT32	dep;
INT32	reg;
INT32	tmp;
c_dep*	p_dep;
c_reg*	p_reg;

	p_dep = deps;
	for( dep=1; dep<=DEP_NB; ++dep)
		{
		p_dep++;
		p_dep->circ_nb = datacube_def.get_real( DEP_CH, dep, 6);
		p_dep->reg = datacube_def.get_real( DEP_CH, dep, 4);
		p_dep->first = CIRC_NB;
		p_dep->last = 0;
		}
	for( circ=1; circ<=CIRC_NB; ++circ )
		{
		dep = datacube_def.get_real( CIRC_CH, circ, 1);
		p_dep = &deps[dep];
		if( p_dep->first > circ )
			p_dep->first = circ;
		if( p_dep->last < circ )
			p_dep->last = circ;
		circs[circ].dep = dep; 
		}
	p_dep = deps;
	for( dep=1; dep<=DEP_NB; ++dep )
		{
		p_dep++;
		if( p_dep->circ_nb != (p_dep->last-p_dep->first+1) )
			ERR_PRINT_STRING( "Bad Number Nb of Circ on departement %d", dep);
		}
	GOOD_PRINT_STRING( "Departement and Circonscription loaded" );

	p_reg = regs;
	for( reg=1; reg<=REG_NB; ++reg )
		{
		p_reg++;
		p_reg->dep_nb = 0;
		p_reg->deps[0] = 0;
		}
	for( dep=1; dep<=DEP_NB; ++dep )
		{
		reg = datacube_def.get_real( DEP_CH, dep, 4);
		if( reg )
			{
			p_reg = &regs[reg];
			p_reg->dep_nb++;
			p_reg->deps[p_reg->dep_nb] = dep;
			}
		}
	p_reg = regs;
	for( reg=1; reg<=REG_NB; ++reg )
		{
		p_reg++;
		tmp = INT32(datacube_def.get_real( REG_CH, reg, 4));
		if( p_reg->dep_nb != tmp )
			ERR_PRINT_STRING( "Bad Number Nb of Departement on Region %d", reg);
		}
	GOOD_PRINT_STRING( "Regions loaded" );
}

void	c_lcp::make_reg_name( CHAR* str, INT32 reg_in)
{
	strcpy( str, datacube_def.get_str( REG_CH, reg_in, 3));
}

void	c_lcp::make_dep_name( CHAR* str, INT32 dep_in)
{
CHAR*	pt;
CHAR*	num;
	pt = datacube_def.get_str( DEP_CH, dep_in, 3);
	num = datacube_def.get_str( DEP_CH, dep_in, 2);
	sprintf( str, "%s %s", num, pt);
}

void	c_lcp::make_circ_name( CHAR* str, INT32 circ_in)
{
CHAR*	num;
CHAR*	nom;
	num = datacube_def.get_str( CIRC_CH, circ_in, 5);
	nom = datacube_def.get_str( CIRC_CH, circ_in, 6);
	sprintf( str, "%s %s", num, nom);
}

void	c_lcp::menu_add_dep(INT32 i)
{
CHAR	str[256];
	make_dep_name( str, i);
	menu::add_item( str, DEP_LEVEL*1000+i);
}

void	c_lcp::menu_add_circ(INT32 i)
{
CHAR	str[256];
	make_circ_name( str, i);
	menu::add_item( str, CIRC_LEVEL*1000+i);
}

void	c_lcp::menu_add_dep_circ( INT32 dep_in, INT32 sub_dep)
{
INT32	circ;
INT32	last;
INT32	sub_circ;

CHAR*	pt;
CHAR	str[256];
CHAR*	num;
	last = deps[dep_in].last;
	sub_circ = menu::create( lcp_menu_do);
	for( circ=deps[dep_in].first; circ<=last; ++circ )
		menu_add_circ(circ);

	menu::set_cur( sub_dep);
	pt = datacube_def.get_str( DEP_CH, dep_in, 3);
	num = datacube_def.get_str( DEP_CH, dep_in, 2);
	sprintf( str, "%s %s", num, pt);
	menu::add_menu_sub( str, sub_circ );
}

void	c_lcp::menu_add()
{
INT32	menu_lcp;
INT32	sub_level;
INT32	sub_reg;
INT32	sub_dep[7];
INT32	sub_dep_all;
INT32	sub_dep_circ[7];
//	INT32	sub_dep_circ_all;
//INT32	i;
CHAR	str[256];

	data_init();

	sub_level = menu::create( lcp_menu_do);
	menu::add_item( "France", FRANCE_LEVEL);
	menu::add_item( "Region", REG_LEVEL);
	menu::add_item( "Departement", DEP_LEVEL);
	menu::add_item( "Circonscription", RESULT_LEVEL);

	sub_reg = menu::create( lcp_menu_do);
	for( INT32 i=1; i<=REG_NB; ++i )
		{
		make_reg_name( str, i);
		menu::add_item( str, REG_LEVEL*1000+i);
		}

	sub_dep[0] = menu::create( lcp_menu_do);
	for( INT32 i=1; i<=19; ++i )
		menu_add_dep(i);
	menu_add_dep( 105);
	menu_add_dep( 106);
	for( INT32 i=21; i<=23; ++i )
		menu_add_dep(i);

	sub_dep[1] = menu::create( lcp_menu_do);
	for( INT32 i=24; i<=48; ++i )
		menu_add_dep(i);

	sub_dep[2] = menu::create( lcp_menu_do);
	for( INT32 i=49; i<=74; ++i )
		menu_add_dep(i);

	sub_dep[3] = menu::create( lcp_menu_do);
	menu_add_dep(76);
	for( INT32 i=78; i<=90; ++i )
		menu_add_dep(i);

	sub_dep[4] = menu::create( lcp_menu_do);
	menu_add_dep(75);
	menu_add_dep(77);
	for( INT32 i=91; i<=95; ++i )
		menu_add_dep(i);

	sub_dep[5] = menu::create( lcp_menu_do);
	for( INT32 i=96; i<=100; ++i )
		menu_add_dep(i);

	sub_dep[6] = menu::create( lcp_menu_do);
	for( INT32 i=101; i<=104; ++i )
		menu_add_dep(i);

	sub_dep_all = menu::create( lcp_menu_do);
	menu::add_menu_sub( " 1 � 23  Ain Creuze", sub_dep[0] );
	menu::add_menu_sub( "24 � 48  Dordogne Loz�re", sub_dep[1] );
	menu::add_menu_sub( "49 � 74  Maine-et-Loire Savoie", sub_dep[2] );
	menu::add_menu_sub( "76 � 90  Seine-Maritime...", sub_dep[3] );
	menu::add_menu_sub( "Paris et R�gion parisienne", sub_dep[4] );
	menu::add_menu_sub( "Dom", sub_dep[5] );
	menu::add_menu_sub( "Tom", sub_dep[6] );

	sub_dep_circ[0] = menu::create( lcp_menu_do);
	for( INT32 i=1; i<=19; ++i )
		menu_add_dep_circ( i, sub_dep_circ[0] );
	menu_add_dep_circ( 105, sub_dep_circ[0] );
	menu_add_dep_circ( 106, sub_dep_circ[0] );
	for( INT32 i=21; i<=23; ++i )
		menu_add_dep_circ(i, sub_dep_circ[0] );

	sub_dep_circ[1] = menu::create( lcp_menu_do);
	for( INT32 i=24; i<=48; ++i )
		menu_add_dep_circ(i, sub_dep_circ[1] );

	sub_dep_circ[2] = menu::create( lcp_menu_do);
	for( INT32 i=49; i<=74; ++i )
		menu_add_dep_circ(i, sub_dep_circ[2] );

	sub_dep_circ[3] = menu::create( lcp_menu_do);
	menu_add_dep_circ(76, sub_dep_circ[3] );
	for( INT32 i=78; i<=90; ++i )
		menu_add_dep_circ(i, sub_dep_circ[3] );

	sub_dep_circ[4] = menu::create( lcp_menu_do);
	menu_add_dep_circ(75, sub_dep_circ[4] );
	menu_add_dep_circ(77, sub_dep_circ[4] );
	for( INT32 i=91; i<=95; ++i )
		menu_add_dep_circ(i, sub_dep_circ[4] );

	sub_dep_circ[5] = menu::create( lcp_menu_do);
	for( INT32 i=96; i<=100; ++i )
		menu_add_dep_circ(i, sub_dep_circ[5] );

	sub_dep_circ[6] = menu::create( lcp_menu_do);
	for( INT32 i=101; i<=104; ++i )
		menu_add_dep_circ(i, sub_dep_circ[6] );

//	sub_dep_circ_all = menu::create( lcp_menu_do);
/*
	menu::add_menu_sub( " 1 � 23  Ain Creuze", sub_dep_circ[0] );
	menu::add_menu_sub( "24 � 48  Dordogne Loz�re", sub_dep_circ[1] );
	menu::add_menu_sub( "49 � 74  Maine-et-Loire Savoie", sub_dep_circ[2] );
	menu::add_menu_sub( "76 � 90  Seine-Maritime...", sub_dep_circ[3] );
	menu::add_menu_sub( "Paris et R�gion parisienne", sub_dep_circ[4] );
	menu::add_menu_sub( "Dom", sub_dep_circ[5] );
	menu::add_menu_sub( "Tom", sub_dep_circ[6] );
*/
	menu_lcp = menu::create( lcp_menu_do);
	menu::add_menu_sub( "Niveau", sub_level );

	menu::add_separator();
	menu::add_menu_sub( "Regions", sub_reg );
	menu::add_menu_sub( "D�partements", sub_dep_all );
//	menu::add_menu_sub( "Circonscriptions", sub_dep_circ_all );

	
	menu::add_separator();
	menu::add_menu_sub( " 1 � 23  Ain Creuze", sub_dep_circ[0] );
	menu::add_menu_sub( "24 � 48  Dordogne Loz�re", sub_dep_circ[1] );
	menu::add_menu_sub( "49 � 74  Maine-et-Loire Savoie", sub_dep_circ[2] );
	menu::add_menu_sub( "76 � 90  Seine-Maritime...", sub_dep_circ[3] );
	menu::add_menu_sub( "Paris et R�gion parisienne", sub_dep_circ[4] );
	menu::add_menu_sub( "Dom", sub_dep_circ[5] );
	menu::add_menu_sub( "Tom", sub_dep_circ[6] );

	menu_set_main( menu_lcp );
}

BOOL	c_lcp::do_key( UINT8* c, BOOL b_special, INT32* modifiers, INT32* x, INT32* y)
{
BOOL	b_return = FALSE;
	if( net->is_slave() )
		return b_return;
	if( !flatland::is_draw_focus() && modifier::is_none(*modifiers)  )
		{
		b_return = TRUE;
		if( b_special)
			{
			switch (*c)
				{
				case GLUT_KEY_PAGE_UP:
					iq_ui = 0;
					break;
				case GLUT_KEY_PAGE_DOWN:
					iq_ui = dif.size()-2;
					break;
				case GLUT_KEY_UP:
					--iq_ui;
					break;
				case GLUT_KEY_DOWN:
					++iq_ui;
					break;
				default:
					b_return = TRUE;
					break;
				}
			}
		else
		switch (*c)
			{
			//Suppr
			case 127:	dif_remove();							break;
			//Return
			case 13:	dif_add();								break;
			//Backspace
			case 'N':
				level = IMOD( level-2, LEVEL_NB) + 1;
				break;
			case 'n':
				level = IMOD( level, LEVEL_NB) + 1;
				if( level == CIRC_LEVEL)
					level = RESULT_LEVEL;
				break;
			case 'c':
			case 'C':	level = RESULT_LEVEL;					break;
			case 'd':
			case 'D':	level = DEP_LEVEL;						break;
			case 'r':
			case 'R':	level = REG_LEVEL;						break;
			case 'f':
			case 'F':	set_circ( CIRC_FRANCE );
						level = FRANCE_LEVEL;
						break;
			case ' ':	iq_dif++;								break;
			case 's':
			case 'S':	b_dif = !b_dif;							break;
			case 't':
			case 'T':	set_circ( CIRC_TITRE );
						level = FX3_LEVEL;
						break;
			case 'a':
			case 'A':	set_circ( CIRC_PARTI );
						level = FX2_LEVEL;
						break;

			case 'w':
			case 'W':
			case 'p':
			case 'P':	b_return = FALSE;
						break;

			case '-':
				switch( level)
					{
					case FRANCE_LEVEL:
						break;
					case REG_LEVEL:
						set_reg( reg_cur-1);
						break;
					case DEP_LEVEL:
						++dep_cur;
						if (dep_cur == 20)
							dep_cur = 19;
						set_dep( dep_cur);
						break;
					case CIRC_LEVEL:
					case RESULT_LEVEL:
						set_circ( circ_cur-1);
						break;
					}
				break;
			case '+':
				switch( level)
					{
					case FRANCE_LEVEL:
						break;
					case REG_LEVEL:
						set_reg( reg_cur+1);
						break;
					case DEP_LEVEL:
						++dep_cur;
						if (dep_cur == 20)
							dep_cur = 21;
						set_dep( dep_cur);
						break;
					case CIRC_LEVEL:
					case RESULT_LEVEL:
						set_circ( circ_cur+1);
						break;
					}
				break;
			case 9:	//keep tab
				b_return = FALSE;
				break;
			default:
				b_return = TRUE;
				break;
			}
		iq_ui = WRAP_ID( iq_ui, INT32(0), (INT32)(dif.size()-1 ) );
		}
	return b_return;
}

INT32	c_lcp::get_bdd_tri_id( char* name_in)
{
INT32	id_out = 0;
INT32	r;
INT32	f1;
INT32	f2;

CHAR	str[256];
	if ( str_is_equal_nocase( name_in, "dep", 3) )
		{
		r = sscanf( name_in+4, "%d", &f1);
		if( r == 1 )
			{
			id_out = f1 ;
			DBG_PRINT_STRING( "Dep %d", f1 );
			}
		else
			{
			ERR_PRINT_STRING( " Dep Can't sscanf name");
			id_out = 256*256*256;
			}
		}
	else if ( str_is_equal_nocase( name_in, "reg", 3) )
		{
		r = sscanf( name_in+4, "%d", &f1);
		if( r == 1 )
			{
			id_out = f1 ;
			DBG_PRINT_STRING( "Reg %d", f1 );
			}
		else
			{
			ERR_PRINT_STRING( " Reg Can't sscanf name");
			id_out = 256*256*256;
			}
		}
	else if ( str_is_equal_nocase( name_in, "circ", 4) )
		{
		r = sscanf( name_in+5, "%d_%d", &f1, &f2);
		if( r == 2 )
			{
			id_out = deps[f1].first + f2 - 1 ;
			DBG_PRINT_STRING( "Dep %d Circ %d -> id=%d", f1, f2, id_out );
			}
		else
			{
			ERR_PRINT_STRING( " Circ Can't sscanf name");
			id_out = 256*256*256;
			}
		}
	else if ( str_is_equal_nocase( name_in, "vil", 3) )
		{
		r = sscanf( name_in+4, "%d", &f1);
		if( r == 1 )
			{
			id_out = f1 ;
			DBG_PRINT_STRING( "%s -> Ville %d", name_in, f1 );
			strcpy( str, name_in+8);
			strcpy( name_in, str);
			}
		else
			{
			ERR_PRINT_STRING( " Ville Can't sscanf name");
			id_out = 256*256*256;
			}
		}
	return id_out;
}

//06 09 92 20 38 Bertrand