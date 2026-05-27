#include "upstream.h"
#include "obj_ui/com/net.h"
#include "obj_ui/bdd/bdd_point/bdd_point.h"
#include "obj_ui/bdd/bdd_point/bdd_data_upstream.h"


FACTORY_CREATE_V1( c_upstream, upstream, Upstream, upstream );

c_upstream*				upstream;

#define	MATCH_NB	12

INT32	seq_order[MATCH_NB*3] =
{
	1, 16, 21,
	19, 6, 14,
	14, 21, 8,
	12, 9, 22,
	15, 18, 20,
	13, 11, 5,
	3, 16, 15,
	2, 6, 19,
	6, 5, 10,
	21, 9, 13,
	22, 5, 18,
	20, 14, 16,
};


void	c_phase::begin()
{
	b_active = TRUE;
	progression = 0;
	time = 0.;
}

void	c_phase::clear()
{
	b_active = FALSE;
	progression = 0;
	time = 0.;
	intensity = 0.;
}

void	c_phase::end()
{
	b_active = FALSE;
//	progression = 0;
}

c_phase::c_phase()
{
	b_active = FALSE;
	progression = 0;
}

void	c_phase::update( REAL dt)
{
	if( b_active)
		{
		REAL tmp;
		time += dt ;					
		if( len != 0 )
			progression = time/len;

		if( ease_in <= 0)
			intensity = (time>0.) ? 1 : 0 ;
		else
			{
			intensity = time/ease_in;
			intensity = CLAMP( intensity, REAL(0), REAL(1) );
			}
		
		tmp = time - len;
		if( tmp > 0. )
			{
			if( ease_out <= 0. )
				intensity = 0 ;
			else
				{
				intensity = 1 - tmp/ease_out;
				intensity = CLAMP( intensity, REAL(0), REAL(1) );
				}
			if( intensity <= 0. )
				b_active = FALSE;
			}
		}
}

namespace	n_upstream
{
	char*	phase_str[UP_NB] =
	{
		"Attente",
		"Decompte",
		"Essai",
		"Score",
		"Rembobinage",
		"Sequence transition",
		"Score Final",
		"Evacuation",
	};

	static	CONST	INT32	BASE_PARAM_NB			= 10;
	static	CONST	INT32	PHASES_PARAM_NB			= 8;
	static	CONST	INT32	PHASE_PARAM_NB			= 6;
	static	CONST	INT32	SCORE_PARAM_NB			= 3;
	static	CONST	INT32	GROUP_NB				= 2+UP_NB;
	static	CONST	INT32	PARAM_NB_MAX =
				BASE_PARAM_NB
			+	PHASES_PARAM_NB
			+	SCORE_PARAM_NB
			+	PHASE_PARAM_NB*UP_NB
			+	GROUP_NB;

	static	ST_PARAM	param[PARAM_NB_MAX] =
	{
		ST_PARAM_BOOL_OFF( active )
		ST_PARAM_BOOL_OFF( on )
		ST_PARAM_BOOL_ON( jardin )
		ST_PARAM_GROUP( Phase stuff, PHASES_PARAM_NB )
			ST_PARAM_BOOL_OFF( prev_trig )
			ST_PARAM_BOOL_OFF( next_trig )
			ST_PARAM_BOOL_OFF( restart_trig )
			{	NULL,	PARAM_INT32,				"Phase",			1, 0,	0, UP_NB-1,	NULL, NULL },
			{	NULL,	PARAM_SYMBOLIC,				"Phase_name",		1, 0,	0, UP_NB-1,	NULL, phase_str },
			{	NULL,	PARAM_REAL,					"progression",		1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_INT32,				"essai",			1, 0,	0, UP_NB-1,	NULL, NULL },
			{	NULL,	PARAM_INT32,				"manche",			1, 0,	0, UP_NB-1,	NULL, NULL },
		ST_PARAM_GROUP( Score, SCORE_PARAM_NB )
			{	NULL,	PARAM_INT32,				"score_in",				1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_INT32,				"score_out",			1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_INT32,				"score_total",			1, 0,	0, PARAM_INFINI,	NULL, NULL },
		{	NULL,	PARAM_REAL,					"rewind",				0, 1,	0, UP_NB-1,	NULL, NULL },
		{	NULL,	PARAM_INT32,				"match_index",			1, 0,	0, MATCH_NB-1,	NULL, NULL },
		ST_PARAM_BOOL_OFF( sequence_one_by_one )
		{	NULL,	PARAM_INT32,				"sequence_index",		1, 0,	0, SEQ_UPSTREAM_MAX_NB-1,	NULL, NULL },
		{	NULL,	PARAM_INT32,				"palette_index",		1, 0,	0, 11,	NULL, NULL },
		ST_PARAM_BOOL_OFF( RESTART_MATCH )
		ST_PARAM_BOOL_OFF( RESTART )
		ST_PARAM_GROUP_CLOSED( Attente, PHASE_PARAM_NB )
			ST_PARAM_BOOL_OFF( attente_active )
			{	NULL,	PARAM_REAL,				"attente_len",			1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"attente_ease_in",		1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"attente_ease_out",		1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"attente_progression",	1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"attente_intensity",	1, 0,	0, PARAM_INFINI,	NULL, NULL },
		ST_PARAM_GROUP_CLOSED( Decompte, PHASE_PARAM_NB )
			ST_PARAM_BOOL_OFF( decompte_active )
			{	NULL,	PARAM_REAL,				"decompte_len",			1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"decompte_ease_in",		1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"decompte_ease_out",	1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"decompte_progression",	1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"decompte_intensity",	1, 0,	0, PARAM_INFINI,	NULL, NULL },
		ST_PARAM_GROUP_CLOSED( Essai, PHASE_PARAM_NB )
			ST_PARAM_BOOL_OFF( essai_active )
			{	NULL,	PARAM_REAL,				"essai_len",			1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"essai_ease_in",		1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"essai_ease_out",		1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"essai_progression",	1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"essai_intensity",		1, 0,	0, PARAM_INFINI,	NULL, NULL },
		ST_PARAM_GROUP_CLOSED( Score, PHASE_PARAM_NB )
			ST_PARAM_BOOL_OFF( score_active )
			{	NULL,	PARAM_REAL,				"score_len",			1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"score_ease_in",		1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"score_ease_out",		1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"score_progression",	1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"score_intensity",		1, 0,	0, PARAM_INFINI,	NULL, NULL },
		ST_PARAM_GROUP_CLOSED( Rembobinage, PHASE_PARAM_NB )
			ST_PARAM_BOOL_OFF( rembobinage_active )
			{	NULL,	PARAM_REAL,				"rembobinage_len",			1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"rembobinage_ease_in",		1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"rembobinage_ease_out",		1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"rembobinage_progression",	1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"rembobinage_intensity",	1, 0,	0, PARAM_INFINI,	NULL, NULL },
		ST_PARAM_GROUP_CLOSED( Sequence Transition, PHASE_PARAM_NB )
			{	NULL,	PARAM_BOOL,				"seq_trans_active",			1, 0,	0, 1,				NULL, NULL },
			{	NULL,	PARAM_REAL,				"seq_trans_len",			1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"seq_trans_ease_in",		1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"seq_trans_ease_out",		1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"seq_trans_progression",	1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"seq_trans_intensity",		1, 0,	0, PARAM_INFINI,	NULL, NULL },
		ST_PARAM_GROUP_CLOSED( Score Final, PHASE_PARAM_NB )
			ST_PARAM_BOOL_OFF( score_final_active )
			{	NULL,	PARAM_REAL,				"score_final_len",			1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"score_final_ease_in",		1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"score_final_ease_out",		1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"score_final_progression",	1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"score_final_intensity",	1, 0,	0, PARAM_INFINI,	NULL, NULL },
		ST_PARAM_GROUP_CLOSED( Evacuation, PHASE_PARAM_NB )
			ST_PARAM_BOOL_OFF( evacuation_active )
			{	NULL,	PARAM_REAL,				"evacuation_len",			1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"evacuation_ease_in",		1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"evacuation_ease_out",		1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"evacuation_progression",	1, 0,	0, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_REAL,				"evacuation_intensity",		1, 0,	0, PARAM_INFINI,	NULL, NULL },
	};
}

void	c_upstream::param_init_pt()
{
INT32	h = 0;
	param_set_pt( h, get_pt_active() );
	param_set_pt( h, b_on);
	param_set_pt( h, b_jardin);

	h++;
	param_set_pt( h, b_prev_trig);
	param_set_pt( h, b_next_trig);
	param_set_pt( h, b_restart_trig);
	param_set_pt( h, s_phase_ui);
	param_set_pt( h, s_phase_ui);
	param_set_pt( h, progression);
	param_set_pt( h, essai);
	param_set_pt( h, manche);

	h++;
	param_set_pt( h, score_in);
	param_set_pt( h, score_out);
	param_set_pt( h, score_total);

	param_set_pt( h, rewind);

	param_set_pt( h, match_index);
	param_set_pt( h, b_seq_one_by_one);
	param_set_pt( h, seq_index);
	param_set_pt( h, pal_index);
	param_set_pt( h, b_restart_match_trig);
	param_set_pt( h, b_main_restart_trig);

	for( INT32 i=0; i<UP_NB; ++i )
		{
		h++;
		param_set_pt( h, phases[i].b_active);
		param_set_pt( h, phases[i].len);
		param_set_pt( h, phases[i].ease_in);
		param_set_pt( h, phases[i].ease_out);
		param_set_pt( h, phases[i].progression);
		param_set_pt( h, phases[i].intensity);
		}

	err_param_init_pt(h);
}

void c_upstream::init()
{
	b_first = TRUE;
	phase_cur = NULL;
	seq_index = 0;
}

CONSTRUCTOR_CREATE(c_upstream)
{
	param_init_with( n_upstream::param, n_upstream::PARAM_NB_MAX);
	init();
}

c_upstream::~c_upstream()
{
}

//static	c_rand_lin	rnd;

void	c_upstream::phase_begin()
{
	if( net && b_jardin )//&& s_phase != UP_DEOOMPTE )
		{
		CHAR	buf[1024];
		sprintf( buf, "PHASE %d", s_phase);
		net->send_osc_str( 0, buf);
		}
	phase_cur->begin();
}

void	c_upstream::seq_send()
{
	if( net && b_jardin)
		{
		CHAR	buf[1024];
		sprintf( buf, "SEQ %d", seq_index);
		net->send_osc_str( 0, buf);
		}
}


void	c_upstream::seq_change()
{
		if( b_seq_one_by_one)
//			do 
				{
				seq_index = IMOD( ++seq_index, 32);
				}
//			while();
		else
			seq_index = seq_order[ match_index*3+manche ];
	pal_index = IMOD( ++pal_index, 12);
	seq_send();
}

void	c_upstream::phase_restart()
{
	phase_last = NULL;
	s_phase_ui = s_phase = 7;
	phase_cur = &phases[s_phase_ui];
	essai = 0;
	manche = 0;
	for( INT32 i=0; i<UP_NB; ++i )
		phases[i].clear();
	phase_cur->begin();
}

void	c_upstream::update()
{
	if( !is_active() )
		return;
//
	if( bdd_point_cur)
		bdd_point_cur->lock();
//INIT
	if( b_first )
		{
		do_first();
		b_first = FALSE;
		}

	if( delta_t.update() ) // || b_restart_trig) )
		{
		}
	dt = delta_t.get_dt();
	if( phase_last)
		phase_last->update( dt);
//	if( ( s_phase_ui != UP_DEOOMPTE ) || b_on )
	if( b_on )
		phase_cur->update( dt);
//	if( net && b_jardin && s_phase == UP_DEOOMPTE && b_on_last == FALSE && b_on == TRUE )
	if( net && b_jardin && b_on_last == FALSE && b_on == TRUE )
		{
		CHAR	buf[1024];
		sprintf( buf, "PHASE %d", s_phase);
		net->send_osc_str( 0, buf);
		}

	if( phase_cur->time > phase_cur->len )
		b_next_trig = TRUE;

//todonow sync machines
//net->send_film_on( "crise.avi", 0.);	
	if( b_restart_trig)
		{
		phase_restart();
		b_restart_trig = FALSE;
		}
	if( b_main_restart_trig)
		{
		match_index = 0;
		pal_index = 0;
		main_restart();
		b_main_restart_trig = FALSE;
		}
	if( b_restart_match_trig)
		{
		main_restart();
		b_restart_match_trig = FALSE;
		}

	if( b_prev_trig)
		{
		--s_phase_ui;
		b_prev_trig = FALSE;
		}
	if( b_next_trig)
		{
		switch( s_phase_ui)
			{
			case UP_EVACUATION:
				match_index = IMOD( ++match_index, MATCH_NB);
				++s_phase_ui;
				score_total = 0;
				essai = 0;
				manche = 0;
				seq_change();
				break;
			case UP_ATTENTE:
				++s_phase_ui;
				break;
			case UP_DEOOMPTE:
				seq_send();
				++s_phase_ui;
				break;
			case UP_ESSAI:
			case UP_SCORE_FINAL:
				++s_phase_ui;
				break;
			case UP_SCORE:
				score_total += score_in;
				if( essai < 2)
					s_phase_ui = UP_REMBOBINAGE;
				else if( manche < 2)
					{
					essai = 0;
					++manche;
					seq_change();
					s_phase_ui = UP_SEQ_TRANS;
					}
				else
					s_phase_ui = UP_SCORE_FINAL;
				break;
			case UP_REMBOBINAGE:
				++essai;
				s_phase_ui = UP_DEOOMPTE;
				break;
			case UP_SEQ_TRANS:
				s_phase_ui = UP_DEOOMPTE;
				break;
			}
		b_next_trig = FALSE;
		}

	s_phase_ui = IMOD( s_phase_ui, UP_NB);


	if( s_phase != s_phase_ui)
		{
		phase_last = phase_cur;
		phase_last->time = phase_last->len;
		s_phase = s_phase_ui;
		phase_cur = &phases[s_phase];
		phase_begin();
		}
	progression = phase_cur->progression;
	if( UP_ESSAI <= s_phase_ui &&  s_phase_ui <= UP_SCORE  )
		score_out = score_in;
	else
		score_out = 0;
	switch( s_phase_ui)
		{
		case UP_ATTENTE:
		case UP_DEOOMPTE:
			break;
		case UP_ESSAI:
			rewind = 1;
			break;
		case UP_SCORE_FINAL:
		case UP_EVACUATION:
		case UP_SCORE:
		case UP_SEQ_TRANS:
			break;
		case UP_REMBOBINAGE:
			rewind = 1.-phase_cur->progression;
			break;
		}
	b_on_last = b_on;
//TIME
//	delta_t.update();
//	dt = delta_t.get_dt();

//
/*	if( b_running_ui != b_running )
		{
		if( b_running_ui)
			{
//			b_transition = TRUE;
//			transition_phase = 0.;
			}
		else
			{
			}
		b_running = b_running_ui;
		}
*/
/*
	if( b_transition || b_transition_force )
		{
		if( transition_phase < 1.)
			{
			REAL tmp;
			tmp = dt/ transition_choice_time_length;
			tmp = MIN( tmp, 1-transition_phase);
			transition_phase += tmp;
			if( transition_phase == 1. )
				{
				b_transition = FALSE;
				}
			}
		else
			{
			REAL tmp;
			b_video_on = TRUE;
			tmp = dt/ transition_video_time_length;
			tmp = MIN( tmp, 2-transition_phase);
			transition_phase += tmp;
			if( transition_phase >= 2. )
				{
				b_transition = FALSE;
				transition_phase = 0.;
				}
			}
		}
*/

	if( bdd_point_cur)
		bdd_point_cur->unlock();
}

void	c_upstream::do_first()
{
	phase_last = NULL;
	match_index = 0;
	pal_index = 0;
	phase_restart();
}

void	c_upstream::main_restart()
{
	seq_index = -1;
	seq_change();
	phase_restart();
	for( INT32 i=0; i<UP_NB; ++i )
		phases[i].progression = 0;

}

BOOL	c_upstream::do_key( UINT8* c, BOOL b_special, INT32* modifiers, INT32* x, INT32* y)
{

BOOL	b_return = FALSE;
/*	if( net->is_slave() )
		return b_return;
	if( !b_draw_focus && modifier_is_none(*modifiers)  )
		{
		b_return = TRUE;
		if( b_special)
			{
			switch (*c)
				{
				case GLUT_KEY_PAGE_UP:
					break;
				case GLUT_KEY_PAGE_DOWN:
					break;
				case GLUT_KEY_UP:
					break;
				case GLUT_KEY_DOWN:
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
			case 127:	break;
			//Return
			case 13:	break;
			//Backspace
			case 'N':
				break;
			case 'n':
				break;
			case 'c':
			case 'C':	break;
			case 'd':
			case 'D':	break;
			case 'r':
			case 'R':	break;
			case 'f':
			case 'F':	break;
			case ' ':	break;
			case 's':
			case 'S':	break;
			case 't':
			case 'T':	break;
			case 'a':
			case 'A':	break;
			case 'w':
			case 'W':
			case 'p':
			case 'P':	b_return = FALSE;
						break;
			case '-':	break;
			case '+':	break;
			case 9:	//kepp tab
				b_return = FALSE;
				break;
			default:
				b_return = TRUE;
				break;
			}
		}
*/
	return b_return;
}
