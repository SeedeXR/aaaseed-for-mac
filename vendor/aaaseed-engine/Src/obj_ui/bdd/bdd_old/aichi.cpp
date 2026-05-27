#include "obj_ui/bdd/bdd_old/aichi.h"
#include "obj_ui/com/net.h"
#include "infrastructure/data/datacube.h"
#include "obj_ui/bdd/bdd_point/bdd_point.h"
#include "math/rand.h"
#include "infrastructure/layer/module.h"
#include "infrastructure/layer/layers.h"
#include "infrastructure/layer/layer.h"
#include "obj_ui/bdd/bdd_point/bdd_boid.h"
#include "infrastructure/bind/bind.h"
#include "draw/bind_img.h"

FACTORY_CREATE_V1( c_aichi, aichi, Aichi, aichi );

c_aichi*					aichi;
c_layers*					layers_words;
c_layers*					layers_com_emiter;
c_bdd_boid*					boids_words = NULL;
c_bdd_boid*					boids_com_emiter = NULL;
vector<c_aichi_contact*>	contacts_free;
INT32						contact_on_nb = 0;
INT32						contact_on_word_nb = 0;
INT32						contact_on_com_nb = 0;

void	c_aichi_point::erase_contact()
{
	map< INT32, c_aichi_contact* >::iterator	it;
	for( it=contacts.begin(); it!=contacts.end(); ++it )
	{
//		delete it->second;
		contacts_free.push_back( it->second );
		--contact_on_nb;
	}
	contacts.clear();
}

void		c_aichi_point::add_contact( INT32 id, REAL time_left_in, REAL  weight_step_in )
{
	map< INT32, c_aichi_contact*>::iterator	it = contacts.find( id );
	if( it == contacts.end() )
	{
		c_aichi_contact* con;
//		con = new c_aichi_contact;
		if( contacts_free.empty() )
			con = new c_aichi_contact;
		else
		{
			con = contacts_free.back();
			contacts_free.pop_back();
		}
		con->time_left = time_left_in;
		con->weight_step = weight_step_in;
		contacts[id] = con;
		++contact_on_nb;
	}
	else
	{
		debug_break( "try to add existing contact");
	}
}
void		c_aichi_point::del_contact( INT32 id )
{
	map< INT32, c_aichi_contact*>::iterator	it = contacts.find( id );
	if( it != contacts.end() )
	{
		c_aichi_contact* con = it->second;
//		delete con;
		contacts_free.push_back( con );
		contacts.erase(it);
		--contact_on_nb;
	}
//	b_touchable = TRUE;
}

c_aichi_contact*	c_aichi_point::find_contact( INT32 id )
{
	map< INT32, c_aichi_contact*>::iterator	it = contacts.find( id );
	if( it != contacts.end() )
		return it->second;
	else
		return NULL;
}

void	c_aichi_point::reset()
{
	weight = 1.;
	growth = 0.;
	growth_color = 0.;
}

c_aichi_point::c_aichi_point()
{
	reset();
}

void	c_aichi::contact_check( c_bdd_boid* bdd_boid, c_aichi_point* p_aichi_point_in, REAL size_factor, REAL size_y )
{
	map<INT32,c_dot*>			dots;
	map<INT32,c_dot*>::iterator	it;
	INT32			dot_nb;
	INT32			boid_nb;
	REAL			boid_pos[3];
	REAL			dot_pos[3];
	REAL			r;
	c_aichi_point*	p_aichi_point;
	//INT32			i;
	//INT32			j;
	REAL			contact_on_dist_squared;
	REAL			contact_off_dist_squared;

	if( !bdd_boid )
		return;
	if( !bdd_point_cur)
		return;

	contact_on_dist_squared = contact_on_dist*contact_on_dist;
	contact_off_dist_squared = contact_off_dist*contact_off_dist;

	it = bdd_point_cur->dots.begin();
	dot_nb = bdd_point_cur->dots.size(); 
	boid_nb = bdd_boid->get_point_nb();

	for( INT32 i = 0; i < dot_nb; ++i, ++it )
		{
		//get dot coor
		c_dot*	dot;
		INT32	dot_id;
		dot = it->second;
//		if( dot->is_moving() )
			{
			dot_id = it->first;
			dot->get_filtered( 0, dot_pos, r);
			p_aichi_point = p_aichi_point_in;
			for( INT32 j = 0; j < boid_nb; ++j )
				{
				if( p_aichi_point->b_touchable )
					{
					REAL				dist;
					REAL				tmp;
					c_aichi_contact*	contact;

					bdd_boid->get_point( boid_pos, j );

					dist = dot_pos[0] - boid_pos[0];
					dist = ABS(dist);
					dist -= p_aichi_point->size_x * size_factor * p_aichi_point->weight * .5;
					dist = MAX( REAL(0.), dist);
					dist *= dist;

					tmp = dot_pos[1] - boid_pos[1];
					tmp = ABS(tmp);
					tmp -= size_y * size_factor * p_aichi_point->weight * .5;
					tmp = MAX( REAL(0.), tmp);
					dist += tmp * tmp;
					
					contact = p_aichi_point->find_contact( dot_id );
					if( contact)
						{
						if( dist > contact_off_dist_squared )
							p_aichi_point->del_contact( dot_id );
						else
							contact_update( j);
						}
					else
						{
						if( dist <= contact_on_dist_squared )
							{
							p_aichi_point->add_contact( dot_id, contact_time, contact_step );
							contact_do( j, boid_pos[0], boid_pos[1]);
							contact_update( j);
							}
						}
					}	// if touchable
				p_aichi_point++;
				}//for j
			}//is_moving
		}//for i
}


namespace	n_aichi
{
	static	CONST	INT32	BASE_PARAM_NB			= 3;
	static	CONST	INT32	VIDEO_PARAM_NB			= 8;
	static	CONST	INT32	TRANSITION_PARAM_NB		= 5;
	static	CONST	INT32	CONTACT_PARAM_NB		= 4;
	static	CONST	INT32	WORD_WEIGHT_PARAM_NB	= WORD_BOID_NB+2;
	static	CONST	INT32	COM_EMI_PARAM_NB		= 11;
	static	CONST	INT32	COM_PARAM_NB			= 3;
	static	CONST	INT32	FILM_PARAM_NB			= 3;
	static	CONST	INT32	DATA_PARAM_NB			= 1;
	static	CONST	INT32	TEST_PARAM_NB			= 8;
	static	CONST	INT32	WORD_PARAM_NB			= 10;
	static	CONST	INT32	WORD_SIZE_PARAM_NB		= WORD_BOID_NB+2;
	static	CONST	INT32	GROUP_NB				= 11;

	static	CONST	INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
											+	VIDEO_PARAM_NB
											+	TRANSITION_PARAM_NB
											+	CONTACT_PARAM_NB
											+	WORD_WEIGHT_PARAM_NB
											+	COM_EMI_PARAM_NB
											+	COM_PARAM_NB
											+	FILM_PARAM_NB
											+	DATA_PARAM_NB
											+	TEST_PARAM_NB
											+	WORD_PARAM_NB
											+	WORD_SIZE_PARAM_NB
											+	GROUP_NB;

	static	ST_PARAM	param[PARAM_NB_MAX] =
	{
		ST_PARAM_BOOL_OFF( active )
		ST_PARAM_BOOL_OFF( running )
		ST_PARAM_BOOL_OFF( dot_flip_y )

		ST_PARAM_GROUP( Video, VIDEO_PARAM_NB )
			ST_PARAM_BOOL_OFF( time_code_active )
			{	NULL,	PARAM_TIMECODE,	"time_code",		1, 0,	1, PARAM_INFINI,	NULL, NULL },
			{	NULL,	PARAM_INT32,	"sequence_index",	1, 0,	-1, PARAM_INFINI,				NULL, NULL },
			{	NULL,	PARAM_STR|PARAM_SAVE_NOT,		"sequence_name",	1, 0,	1, PARAM_INFINI,	NULL, NULL },
			ST_PARAM_INT32_POS_ZERO( plan_index )
			{	NULL,	PARAM_STR|PARAM_SAVE_NOT,		"plan_name",		1, 0,	1, PARAM_INFINI,	NULL, NULL },
			ST_PARAM_BOOL_OFF( video_on )
			ST_PARAM_BOOL_OFF( video_done )

		ST_PARAM_GROUP( Transition, TRANSITION_PARAM_NB )
			ST_PARAM_REAL_POS_ONE( transition_video_time_length )
			ST_PARAM_REAL_POS_ONE( transition_choice_time_length )
			ST_PARAM_BOOL_OFF( transition_force )
			ST_PARAM_BOOL_OFF( transition_on )
			ST_PARAM_REAL_ZERO( transition_phase )

		ST_PARAM_GROUP( Contact, CONTACT_PARAM_NB )			
			{	NULL,	PARAM_INT32|PARAM_SAVE_NOT,		"contact_on_nb",			1, 0,	0, 1,	NULL, NULL },
			{	NULL,	PARAM_INT32|PARAM_SAVE_NOT,		"contact_on_word_nb",		1, 0,	0, 1,	NULL, NULL },
			{	NULL,	PARAM_INT32|PARAM_SAVE_NOT,		"contact_on_comment_nb",	1, 0,	0, 1,	NULL, NULL },
			{	NULL,	PARAM_INT32|PARAM_SAVE_NOT,		"contact_free_nb",			1, 0,	0, 1,	NULL, NULL },			

		ST_PARAM_GROUP( word_weight, WORD_WEIGHT_PARAM_NB )
			{	NULL,	PARAM_REAL,		"word_choice_threshold",	1, 2,	0, PARAM_INFINI,	NULL, NULL },
			ST_PARAM_BOOL_OFF( word_reset_trig )
			ST_PARAM_REAL_POS_ONE( word_1_weight )
			ST_PARAM_REAL_POS_ONE( word_2_weight )
			ST_PARAM_REAL_POS_ONE( word_3_weight )
			ST_PARAM_REAL_POS_ONE( word_4_weight )
			ST_PARAM_REAL_POS_ONE( word_5_weight )
			ST_PARAM_REAL_POS_ONE( word_6_weight )
			ST_PARAM_REAL_POS_ONE( word_7_weight )
			ST_PARAM_REAL_POS_ONE( word_8_weight )
			ST_PARAM_REAL_POS_ONE( word_9_weight )
			ST_PARAM_REAL_POS_ONE( word_10_weight )
			ST_PARAM_REAL_POS_ONE( word_11_weight )
			ST_PARAM_REAL_POS_ONE( word_12_weight )

		ST_PARAM_GROUP_CLOSED( Comment Emitter, COM_EMI_PARAM_NB )
			ST_PARAM_BOOL_OFF( com_active )
			{	NULL,	PARAM_INT32,	"com_emiter_datagrid_id",	1, 2,	1, DATAGRID_NB,		NULL, NULL },
			{	NULL,	PARAM_REAL,		"comment_life_time",		1, 3,	0.2, 10.,			NULL, NULL },
			ST_PARAM_REAL_POS_ONE( com_emiter_size_x_gb )
			ST_PARAM_REAL_POS_ONE( com_emiter_size_x_jp )
			ST_PARAM_REAL_POS_ONE( com_emiter_size_y )
			ST_PARAM_REAL_POS_ONE( com_emiter_size )
			ST_PARAM_REAL_ONE( video_offset_y )
			ST_PARAM_REAL_ONE( video_size_x )
			ST_PARAM_REAL_ONE( video_size_y )
			ST_PARAM_REAL_ONE( video_size_z )
//			{	NULL,	PARAM_REAL,		"speed_min_trigger",		1, 0,	0, 1,	NULL, NULL },

		ST_PARAM_GROUP_CLOSED( Commentaires, COM_PARAM_NB )	
			ST_PARAM_BOOL_OFF( com_net_send )
			{	NULL,	PARAM_INT32,	"com_datagrid_id",				1, 2,	1, DATAGRID_NB,		NULL, NULL },
			{	NULL,	PARAM_INT32,	"com_img_bind_index_start",		0, 64,	0, PARAM_INFINI,	NULL, NULL },
//			{	NULL,	PARAM_REAL,		"speed_min_trigger"		1, 0,	0, 1,	NULL, NULL },

		ST_PARAM_GROUP_CLOSED( Film, FILM_PARAM_NB )
			ST_PARAM_BOOL_OFF( film_net_send )
			ST_PARAM_REAL_ZERO( film_x )
			ST_PARAM_REAL_ZERO( film_y )

		ST_PARAM_GROUP_CLOSED( Data, DATA_PARAM_NB )
			{	NULL,	PARAM_INT32,	"data_datagrid_id",		1, 2,	1, DATAGRID_NB,	NULL, NULL },

		ST_PARAM_GROUP_CLOSED( Test, TEST_PARAM_NB )
			ST_PARAM_BOOL_OFF( send_dot_on_trig )
			ST_PARAM_BOOL_OFF( send_dot_trig )
			ST_PARAM_BOOL_OFF( send_dot_off_trig )
			ST_PARAM_BOOL_OFF( send_film_on_trig )
			ST_PARAM_BOOL_OFF( send_film_off_trig )
			ST_PARAM_BOOL_OFF( send_com_on_trig )
			{	NULL,	PARAM_STR|PARAM_SAVE_NOT,		"com_last",	1, 0,	0, 1,	NULL, NULL },
			{	NULL,	PARAM_INT32|PARAM_SAVE_NOT,		"com_on_nb",	1, 0,	0, 1,	NULL, NULL },

		ST_PARAM_GROUP_CLOSED( Words, WORD_PARAM_NB )
			ST_PARAM_BOOL_OFF( word_active )
			ST_PARAM_BOOL_OFF( word_net_send )
			ST_PARAM_REAL_POS_ZERO( contact_on_dist )
			ST_PARAM_REAL_POS_ZERO( contact_off_dist )
			ST_PARAM_REAL_POS_ONE( contact_step )
			ST_PARAM_REAL_POS_ONE( contact_time )
			ST_PARAM_REAL_POS_ONE( word_growth_speed )
			ST_PARAM_REAL_POS_ONE( word_decay_speed )
			ST_PARAM_REAL_POS_ONE( word_text_size_factor )
			{	NULL,	PARAM_INT32,	"word_datagrid_id",		1, 3,	1, DATAGRID_NB,	NULL, NULL },

		ST_PARAM_GROUP_CLOSED( word_size, WORD_SIZE_PARAM_NB )
			ST_PARAM_REAL_POS_ONE( word_size_y )
			ST_PARAM_REAL_POS_ONE( word_size_factor )
			ST_PARAM_REAL_POS_ZERO( word_1_size_x )
			ST_PARAM_REAL_POS_ZERO( word_2_size_x )
			ST_PARAM_REAL_POS_ZERO( word_3_size_x )
			ST_PARAM_REAL_POS_ZERO( word_4_size_x )
			ST_PARAM_REAL_POS_ZERO( word_5_size_x )
			ST_PARAM_REAL_POS_ZERO( word_6_size_x )
			ST_PARAM_REAL_POS_ZERO( word_7_size_x )
			ST_PARAM_REAL_POS_ZERO( word_8_size_x )
			ST_PARAM_REAL_POS_ZERO( word_9_size_x )
			ST_PARAM_REAL_POS_ZERO( word_10_size_x )
			ST_PARAM_REAL_POS_ZERO( word_11_size_x )
			ST_PARAM_REAL_POS_ZERO( word_12_size_x )
	};
}

void	c_aichi::param_init_pt()
{
INT32	h = 0;
//INT32	i;

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, b_running_ui);
	param_set_pt( h, b_dot_flip_y);

	h++;
	param_set_pt( h, b_tc_active_ui);
	param_set_pt( h, tc);
	param_set_pt( h, seq_cur_index);
	if( seq_cur)
		param_set_pt( h, seq_cur->name );
	else
		h++;
	param_set_pt( h, plan_cur_index);
	if( plan_cur)
		param_set_pt( h, plan_cur->name );
	else
		h++;
	param_set_pt( h, b_video_on);
	param_set_pt( h, b_video_done);

	h++;
//	param_set_pt( h, b_scramble);
	param_set_pt( h, transition_video_time_length);
	param_set_pt( h, transition_choice_time_length);
	param_set_pt( h, b_transition_force);
	param_set_pt( h, b_transition);
	param_set_pt( h, transition_phase);

	h++;	
	param_set_pt( h, contact_on_nb);
	param_set_pt( h, contact_on_word_nb);
	param_set_pt( h, contact_on_com_nb);
	param_set_pt( h, contact_free_nb);

	h++;	
	param_set_pt( h, word_choice_threshold );
	param_set_pt( h, b_word_reset_trig);
	for( INT32 i = 0; i < WORD_BOID_NB; ++i )
		param_set_pt( h, word_aichi_boid[i].weight );

	h++;
	param_set_pt( h, b_com_active);
	param_set_pt( h, com_emiter_datagrid_id );
	param_set_pt( h, com_life_time);
	param_set_pt( h, com_emiter_size_x_gb);
	param_set_pt( h, com_emiter_size_x_jp);
	param_set_pt( h, com_emiter_size_y);
	param_set_pt( h, com_emiter_size);
	param_set_pt( h, video_offset_y);
	param_set_pt_v3( h, video_size);
//	param_set_pt( h, speed_min_trig);

	h++;
	param_set_pt( h, b_com_net_send );
	param_set_pt( h, com_datagrid_id );
	param_set_pt( h, com_img_bind_start);

	h++;
	param_set_pt( h, b_film_net_send );
	param_set_pt( h, film_x);
	param_set_pt( h, film_y);


	h++;
	param_set_pt( h, data_datagrid_id );

	h++;
	param_set_pt( h, b_send_dot_on_trig);
	param_set_pt( h, b_send_dot_trig);
	param_set_pt( h, b_send_dot_off_trig);
	param_set_pt( h, b_send_film_on_trig);
	param_set_pt( h, b_send_film_off_trig);
	param_set_pt( h, b_send_com_on_trig);
	param_set_pt( h, com_fname_last);
	param_set_pt( h, com_on_nb);
	
	h++;
	param_set_pt( h, b_word_active);
	param_set_pt( h, b_word_net_send );
	param_set_pt( h, contact_on_dist);
	param_set_pt( h, contact_off_dist);
	param_set_pt( h, contact_step);
	param_set_pt( h, contact_time);
	param_set_pt( h, word_growth_speed );	
	param_set_pt( h, word_decay_speed );	
	param_set_pt( h, word_text_size_factor);
	param_set_pt( h, word_datagrid_id );

	h++;
	param_set_pt( h, word_size_y);
	param_set_pt( h, word_size_factor);
	for( INT32 i = 0; i < WORD_BOID_NB; ++i )
		param_set_pt( h, word_aichi_boid[i].size_x );


	err_param_init_pt( h );
}

void c_aichi::init()
{
	b_first = TRUE;
}

CONSTRUCTOR_CREATE( c_aichi )
{
	seq_cur = NULL;
	plan_cur = NULL;
	b_running = FALSE;

	param_init_with( n_aichi::param, n_aichi::PARAM_NB_MAX );
	init();
}

c_aichi::~c_aichi()
{
	deinit();
}

void	c_aichi::deinit()
{
	if( coms_on.size() )
		{
		coms_on.clear();
		}
}

static	c_rand_lin	rnd;

void	c_aichi::film_send( REAL x, REAL y)
{
	if( b_film_net_send && b_video_on )
		{
		if( ABS( film_x_last-x) > .01 || ABS( film_y_last-y) > .01 )
			{
			CHAR str[256];
			sprintf( str, "FILM %.2f %.2f", x, y);
			net->send_osc_str( 0, str);
			film_x_last = x;
			film_y_last = y;
			}
		}
}

void	c_aichi::contact_update( INT32 index )
{
	if( b_word_active)
		{
		word_aichi_boid[index].growth += 1;
		}
	else
		{
		}
}

void	c_aichi::contact_do( INT32 index, REAL x, REAL y )
{
	if( b_word_active)
		{
		REAL	tmp;
//todo		word_aichi_boid[index].growth += contact_step;
//		tmp = word_aichi_boid[index].growth + word_aichi_boid[index].weight;
		tmp = word_aichi_boid[index].weight / word_choice_threshold;
		tmp = MIN( tmp, REAL(1));
		if( b_dot_flip_y)
			y = -y;
		word_on_send( index, x, y, tmp );
		}
	else
		{
		if( index )	//zero is just a repulsor
			{
			com_aichi_boid[index].b_touchable = FALSE;
			com_aichi_boid[index].phase_left = 0.;
			com_on_send( (index&1)==1, x, y );
			}
		}
}

void	c_aichi::word_on_send( INT32 index, REAL x, REAL y, REAL p)
{
	if( b_word_net_send )
		{
		if( seqs.size() )
			{
			CHAR str[256];
			sprintf( str, "WORD_ON %s %f %f %f", seqs[index]->name.get(), x, y, p);
			net->send_osc_str( 0, str);
			}
		}
}


void	c_aichi::com_on_send( BOOL b_english, REAL x, REAL y)
{
	if( seq_cur && b_com_net_send && !b_transition)
		{
		if( plan_cur)
			{
			vector<c_com*>	coms;
			coms = plan_cur->coms;
			if( coms.size() )
				{
				CHAR str[256];
				INT32	i;

				i = rnd.get_uint32() >> 12;
				if( i < 0)
					i = -i;	
//				i = i & 0xffffe;
//				if( !b_english)
//					i += 1;
				i = IMOD( i, coms.size());

				com_on_add( coms[i], i, b_english, x, y);
				if( b_dot_flip_y )
					y = -y;

				sprintf( str, "COM_ON plan_a %s %f %f %f", coms[i]->fname.get(), x, y, 0.);
				*(str+12) = 'a'+plan_cur_index;
				com_fname_last.set( str);	//just for the ui
				net->send_osc_str( 0, str);
				}
			}
		}
}

void	c_aichi::update()
{
	if( !is_active() )
		return;
//
	if( bdd_point_cur)
		bdd_point_cur->lock();

//TEST
	if( b_send_dot_on_trig)
		{
		bdd_point_cur->dot_new( 1);
		b_send_dot_on_trig = FALSE;
		}
	if( b_send_dot_trig)
		{
		bdd_point_cur->dot_pos( 1, 3., 4., 0, 5);
		b_send_dot_trig = FALSE;
		}
	if( b_send_dot_off_trig)
		{
		bdd_point_cur->dot_delete( 1);
		b_send_dot_off_trig = FALSE;
		}
	if( b_send_film_on_trig)
		{
		net->send_film_on( "crise.avi", 0.);
		b_send_film_on_trig = FALSE;
		}
	if( b_send_film_off_trig)
		{
		net->send_film_off( "test");
		b_send_film_off_trig = FALSE;
		}
	if( b_send_com_on_trig )
		{
		com_on_send( TRUE, 1., 2. );
		com_on_send( FALSE, 1., -2. );
		b_send_com_on_trig = FALSE;
		}
//INIT
	if( b_first )
		{
		do_first();
		b_first = FALSE;
		}

//TIME
	delta_t.update();
	dt = delta_t.get_dt();

//
	if( b_running_ui != b_running )
		{
		if( b_running_ui)
			{
			s_word_choice = -1;
			b_word_active = TRUE;
			b_com_active = TRUE;
			b_word_reset_trig = TRUE;
			b_video_on = FALSE;
			b_transition = TRUE;
			transition_phase = 0.;
			}
		else
			{
			s_word_choice = -1;
			b_word_active = FALSE;
			for( INT32 i=0; i<COM_BOID_MAX_NB; ++i )
				{
				com_aichi_boid[i].b_touchable = TRUE;
				}
			}
		b_running = b_running_ui;
		}

	if( b_running && (b_word_active == FALSE) && b_video_done )
		{
		b_word_active = TRUE;
		b_word_reset_trig = TRUE;
		b_transition = TRUE;
		b_video_on = FALSE;
//		transition_phase = 0.;
		}
	if( b_word_reset_trig)
		{
		b_word_reset_trig = FALSE;
		for( INT32 i = 0; i < WORD_BOID_NB; ++i )
			word_aichi_boid[i].reset();
		}
//WORD
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
	if( b_word_active)
		words_update();
	if( b_running && s_word_choice >= 0 )
		{
		seq_cur_index = s_word_choice;
		s_word_choice = -1;
		b_word_active = FALSE;
		b_transition = TRUE;
		}
//SEQ
	if( seqs.size() )
		{
		//Seq
		c_seq* seq_next;
		if( b_word_active)
			seq_next = NULL;
		else
			{
			seq_cur_index = IMOD( seq_cur_index, seqs.size());
			seq_next = seqs[seq_cur_index];
			}

		if( seq_next != seq_cur )
			{
			CHAR	buf[256];
			if( seq_cur && seq_cur->name.is_not_empty() )
				sprintf( buf, "SEQ_OFF %s", seq_cur->name.get() );
			else
				sprintf( buf, "SEQ_OFF choice" );
			net->send_osc_str( 0, buf );

			seq_cur = seq_next;
			if( seq_cur && seq_cur->name.is_not_empty() )
				sprintf( buf, "SEQ_ON %s", seq_cur->name.get() );
			else
				sprintf( buf, "SEQ_ON choice" );
			net->send_osc_str( 0, buf );
			}

		if( seq_cur)
			{
			film_send( film_x , film_y);
			if( b_tc_active != b_tc_active_ui )
				{
				if( b_tc_active_ui)
					tc = 0;	//we need a 0 for the avi to send a command at start
				b_tc_active = b_tc_active_ui;
				}
			else if( b_tc_active)
				{
				tc += dt;
	//			vector<c_plan*>::iterator it;
				//UINT32	i;
				for( UINT32 i = 0; i < seq_cur->plans.size(); ++i )
					if( tc < seq_cur->plans[i]->out )
						{
						plan_cur_index = i;
						plan_cur = seq_cur->plans[plan_cur_index];
						break;
						}	
				}
			}
		if( b_com_active)
			com_on_update();
		}
	contact_on_word_nb = 0;
	for( INT32 i=0; i<WORD_BOID_NB; ++i )
		contact_on_word_nb += word_aichi_boid[i].contacts.size();
	contact_on_com_nb = 0;
	for( INT32 i=0; i<COM_BOID_MAX_NB; ++i )
		contact_on_com_nb += com_aichi_boid[i].contacts.size();
	contact_free_nb = contacts_free.size();

	if( bdd_point_cur)
		bdd_point_cur->unlock();
}

void	c_aichi::dot_delete( INT32 id )
{
//	if( b_word_active)
		{
		//INT32	i;
		for( INT32 i = 0; i < WORD_BOID_NB; ++i )
			word_aichi_boid[i].del_contact( id );
		for( INT32 i = 0; i < COM_BOID_MAX_NB; ++i )
			com_aichi_boid[i].del_contact( id );
		}
}

void	c_aichi::dot_clear()
{
//	if( b_word_active)
		{
		//INT32	i;
		for( INT32 i = 0; i < WORD_BOID_NB; ++i )
			word_aichi_boid[i].erase_contact();
		for( INT32 i = 0; i < COM_BOID_MAX_NB; ++i )
			com_aichi_boid[i].erase_contact();
		}
}


void	c_aichi::words_update()
{
	if( !layers_words)
		return;

c_layer* p_layer;
	p_layer = layers_words->layer_get( 0);
	if( p_layer)
		boids_words = (c_bdd_boid*) p_layer->get_bdd();

	if( !boids_words)
		return;

//INT32	i;
REAL	vec[3];
REAL	total;
REAL	tmp;

//	for( i=0; i<WORD_BOID_NB; ++i )
//		words[i].b_contact = FALSE;
	if ( !b_transition )
		contact_check( boids_words, word_aichi_boid, word_text_size_factor, word_size_y );

	total = 0;
	for( INT32 i = 0; i < WORD_BOID_NB; ++i )
		{	
		if( word_aichi_boid[i].growth != 0. )
			{
//			tmp = MIN( dt * word_growth_speed, word_aichi_boid[i].growth);
//			word_aichi_boid[i].growth -= tmp;
//			word_aichi_boid[i].growth = MAX( word_aichi_boid[i].growth - tmp, REAL(0.) );
//			word_aichi_boid[i].weight += tmp;
			word_aichi_boid[i].weight += word_aichi_boid[i].growth * dt * contact_step;
			}
		total += word_aichi_boid[i].weight ;
		}
	tmp = MIN( dt*word_decay_speed, total-REAL(12.) );
	total -= tmp;
	total /= 12.;
	if( total != 0)
		for( INT32 i = 0; i < WORD_BOID_NB; ++i )
			{
			word_aichi_boid[i].weight /= total;
			if( word_aichi_boid[i].weight > word_choice_threshold )
				s_word_choice = i;
			}

	vec[2] = 0.;
	for( INT32 i = 0; i < WORD_BOID_NB; ++i )
		{
		vec[1] = word_size_y * word_size_factor * word_aichi_boid[i].weight;
		vec[0] = word_aichi_boid[i].size_x * word_size_factor * word_aichi_boid[i].weight;
		boids_words->set_box_size( i, vec );
		datacube_def.set_real( word_datagrid_id, i+1, 1, word_aichi_boid[i].weight * word_text_size_factor );
		datacube_def.set_real( word_datagrid_id, i+1, 2, word_aichi_boid[i].weight / word_choice_threshold );
		
//		REAL	tmp;
		tmp = word_aichi_boid[i].growth;
		if( tmp > 0 )
			tmp = word_aichi_boid[i].growth_color + word_growth_speed * dt * tmp;
		else
			tmp = word_aichi_boid[i].growth_color - word_growth_speed * dt;
		word_aichi_boid[i].growth_color = CLAMP( tmp, REAL(0), REAL(1) );
		datacube_def.set_real( word_datagrid_id, i+1, 3, word_aichi_boid[i].growth_color );
		word_aichi_boid[i].growth = 0;
		}
}

void	c_aichi::com_on_add( c_com* com, INT32 index, BOOL b_english, REAL x, REAL y)
{
c_com_on*	p_com;
	p_com = new c_com_on;
	p_com->com = com;
//	p_com->index = index*2+(b_english?0:1);
	p_com->index = b_english ? com->index_gb : com->index_jp;
	p_com->age = 0.;
	p_com->val = 1.;
	p_com->x = x;
	p_com->y = y;
	coms_on.push_back( p_com);
}

void	c_aichi::com_on_update()
{
deque<c_com_on*>::iterator it;
//INT32	i;

	if( !layers_com_emiter)
		return;

c_layer* p_layer;
	p_layer = layers_com_emiter->layer_get( 0);
	if( p_layer)
		boids_com_emiter = (c_bdd_boid*) p_layer->get_bdd();

	if( !boids_com_emiter)
		return;

	if( !b_word_active && !b_transition )
		contact_check( boids_com_emiter, com_aichi_boid, 1, com_emiter_size_y );

	{
	REAL	vec[3];
	vec[0] = film_x;
	vec[1] = film_y + video_offset_y;
	vec[2] = 0.;
	boids_com_emiter->set_pos( 0, vec );
	boids_com_emiter->set_box_size( 0, video_size );
	}
REAL	vec[3];
	vec[2] = 0.;
	for( INT32 i = 0; i < COM_BOID_MAX_NB; ++i )
		{
		REAL tmp;
		if( (i&1)==0 )
			tmp = com_emiter_size_x_gb;
		else
			tmp = com_emiter_size_x_jp;
		com_aichi_boid[i].size_x = tmp * com_emiter_size;
		vec[1] = com_emiter_size_y * com_emiter_size;
		vec[0] = com_aichi_boid[i].size_x * com_emiter_size;
		boids_com_emiter->set_box_size( i+1, vec );

		if( com_aichi_boid[i].phase_left < 1.)
			{
			com_aichi_boid[i].phase_left += dt/com_life_time;
			if( com_aichi_boid[i].phase_left >= 1.)
				{
				com_aichi_boid[i].b_touchable = TRUE;
				}
			datacube_def.set_real( com_emiter_datagrid_id, i+1, 1, com_aichi_boid[i].phase_left);
			}

//		datacube_def.set_real( word_datagrid_id, i+1, 1, word_aichi_boid[i].weight * word_text_size_factor );
//		datacube_def.set_real( word_datagrid_id, i+1, 2, word_aichi_boid[i].weight / word_choice_threshold );
		}

	for ( it = coms_on.begin(); it != coms_on.end(); ++it )
		{
		(*it)->age += dt/com_life_time;
		}

	while( !coms_on.empty() )
		{
		it = coms_on.begin();
		if( (*it)->age >= 1.)
			{
			delete *it;
			coms_on.pop_front();
			}
		else
			break;
		}
	INT32	i = 0;
	for ( it = coms_on.begin(); it != coms_on.end(); ++it )
		{
		i++;
		datacube_def.set_str( com_datagrid_id, i, 1, (*it)->com->name.get() );
		datacube_def.set_real( com_datagrid_id, i, 2, (*it)->index );
		datacube_def.set_real( com_datagrid_id, i, 3,(*it)->x);
		datacube_def.set_real( com_datagrid_id, i, 4,(*it)->y);
		datacube_def.set_real( com_datagrid_id, i, 5,(*it)->age);
		}
	com_on_nb = coms_on.size();

}

void	c_aichi::do_first()
{
	com_parse_file();
	layers_words = c_module::get_cur()->layers_get_from_name_short( "Mots" );
	layers_com_emiter = c_module::get_cur()->layers_get_from_name_short( "MiniBlobs" );
//	already done thru reset
	for( INT32 i = 0; i < WORD_BOID_NB; ++i )
		{
		word_aichi_boid[i].b_touchable = TRUE;
//		word_aichi_boid[i].growth_color = 0.;
		}
}

void	c_aichi::com_parse_file()
{
	datacube_def.update();	//	make sure the datagrid is ready
INT32	row;
INT32	row_max;
INT32	col;
INT32	bind_index;

	row_max = datacube_def.get_row_nb( data_datagrid_id );
	row = 5;
	bind_index = com_img_bind_start;
	do
		{
		if( !datacube_def.is_default( data_datagrid_id, row, 1) )
			{
			c_seq*	seq;

			seq = new c_seq;
			seq->name.set( datacube_def.get_str( data_datagrid_id, row, 1)); 
			seqs.push_back( seq); 
			VERBOSE_PRINTF( "AICHI parsing : sequence %d-%s", seqs.size(), datacube_def.get_str( data_datagrid_id, row, 1) );
			do
				{
				if( !datacube_def.is_default( data_datagrid_id, row, 3) )
					{
					c_plan*	plan;

					plan = new c_plan;
					plan->name.set( datacube_def.get_str( data_datagrid_id, row, 3));
					plan->in = datacube_def.get_real( data_datagrid_id, row, 4); 
					plan->out = datacube_def.get_real( data_datagrid_id, row+2, 4); 
					VERBOSE_PRINTF( "AICHI parsing :     plan %s (%.2f/%.2f)", plan->name.get(), plan->in, plan->out );
					seq->plans.push_back( plan); 
					
					col = 6;
					while( !datacube_def.is_default( data_datagrid_id, row, col) )
						{
						c_com*	com;
						CHAR	fname[256];
						CHAR	bind_name[512];
						com = new c_com;
						com->name.set( datacube_def.get_str( data_datagrid_id, row, col));
						com_build_filename( fname, com->name.get() );
						com->fname.set( fname);
						VERBOSE_PRINTF( "AICHI parsing :        com \"%s\" named \"%s\"", com->name.get(), com->fname.get() );

						sprintf( bind_name, "Maps/%d_%s/%s/%s.tga", seqs.size()-1, seq->name.get(), seq->name.get(), com->fname.get() );
						VERBOSE_PRINTF( "texture %d : %s", bind_index, bind_name );
						bind_img::bind->set_item( bind_index, bind_name );
						//load_texture( bind_index, bind_name);
						com->index_gb = bind_index;
						bind_index++;

						sprintf( bind_name, "Maps/%d_%s/%s/%s_jp.tga", seqs.size()-1, seq->name.get(), seq->name.get(), com->fname.get() );
						VERBOSE_PRINTF( "texture %d : %s", bind_index, bind_name );
						bind_img::bind->set_item( bind_index, bind_name );
						//load_texture( bind_index, bind_name);
						com->index_jp = bind_index;
						bind_index++;

						plan->coms.push_back( com);
						col++;
						};
					}
				else
					break;
				row+=2;
				}			
			while( row <= row_max);	
			}
		row++;
		}
	while( row <= row_max);
}

void	c_aichi::com_build_filename( CHAR* dst, CHAR* src)
{
	CHAR	c;

	--dst;
	--src;
	while( (c=*++src) )
		{
		if( ('a'<=c && c<='z') || ('0'<=c && c<='9') )
			*++dst = c;
		else if ( 'A'<=c && c<='Z' )
			*++dst = c - ('A'-'a');
		else if ( c=='-' ||  c==' ' )
			*++dst = '_';
		}
	while( *dst == '_' )	// remove trailing underscore
		--dst;
	*++dst=0; 
}

/*
void	MAACALLBACK	lcp_menu_do( int value)
{
	MENU_STORE( lcp_menu_do, value);
	lcp->menu_do(value);
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
}

void	c_lcp::menu_init()
{
	datacube_def.load( "pref/default");
	datacube_def.update();
}
*/

BOOL	c_aichi::do_key( UINT8* c, BOOL b_special, INT32* modifiers, INT32* x, INT32* y)
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
