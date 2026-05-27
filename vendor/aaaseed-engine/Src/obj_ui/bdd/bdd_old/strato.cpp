#include "strato.h"
#include "obj_ui/bdd/bdd_sound/bdd_snd_wave.h"
#include "strnum.h"
#include "time/ourtime.h"
#include "obj_ui/bdd/bdd_spe/bdd_boxes.h"

FACTORY_CREATE_V1( c_strato, strato, Strato, strato );

CONST	INT32	SND_NB = 25;

c_strato*	strato;

namespace n_strato
{
	static	CONST	INT32	BASE_PARAM_NB		= 31;
	static	CONST	INT32	PARAM_NB_MAX =	BASE_PARAM_NB;

	static	ST_PARAM	param[PARAM_NB_MAX] =
		{
		ST_PARAM_BOOL_OFF( active )
		ST_PARAM_BOOL_OFF( reset_trig )
		ST_PARAM_BOOL_ON( army )

		ST_PARAM_GROUP( Auto, 7 )
		ST_PARAM_BOOL_OFF( auto_on )
		{	NULL,	PARAM_REAL,	"auto_delay",			1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	NULL,	PARAM_REAL,	"auto_delay_left",		1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	NULL,	PARAM_REAL,	"auto_angle_tolerance",	1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	NULL,	PARAM_REAL,	"pitch_angle_in",	1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	NULL,	PARAM_REAL,	"yaw_angle_in",		1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	NULL,	PARAM_REAL,	"roll_angle_in",	1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },

		ST_PARAM_GROUP( In, 4 )
		{	NULL,	PARAM_REAL,	"image_in",	1., 0.,	0., 11., NULL, NULL },
		{	NULL,	PARAM_REAL,	"pitch_in",	1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	NULL,	PARAM_REAL,	"yaw_in",	1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	NULL,	PARAM_REAL,	"roll_in",	1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },

		ST_PARAM_GROUP( Out, 4 )
		{	NULL,	PARAM_INT32,	"image_out",	1., 0.,	0., 10., NULL, NULL },
		{	NULL,	PARAM_REAL,	"pitch_out",	1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	NULL,	PARAM_REAL,	"yaw_out",		1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	NULL,	PARAM_REAL,	"roll_out",		1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },

		ST_PARAM_GROUP( Sound, 6 )
		{	NULL,	PARAM_REAL,	"background_volume",	1., 0.,	0., 1., NULL, NULL },
		{	NULL,	PARAM_REAL,	"silent_time",	1., 0.,	0., 1., NULL, NULL },
		ST_PARAM_BOOL_OFF( trig_play )
		{	NULL,	PARAM_INT32,	"snd_to_play",	1., 0.,	0., SND_NB, NULL, NULL },
		{	NULL,	PARAM_INT32,	"snd_playing",	1., 0.,	0., SND_NB, NULL, NULL },
		{	NULL,	PARAM_INT32,	"snd_playing_last",	1., 0.,	0., SND_NB, NULL, NULL },

		ST_PARAM_GROUP( Focal, 2 )
		{	NULL,	PARAM_REAL,	"focal_min",		1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
		{	NULL,	PARAM_REAL,	"focal_max",		1., 0.,	-PARAM_INFINI, PARAM_INFINI, NULL, NULL },
	};
}

void	c_strato::param_init_pt()
{
INT32	h=0;
	param_set_pt( h, get_pt_active() );
	param_set_pt( h, b_trig_reset);
	param_set_pt( h, b_army);

	h++;
	param_set_pt( h, b_auto);
	param_set_pt( h, auto_delay);
	param_set_pt( h, auto_delay_left);
	param_set_pt( h, auto_angle_tolerance);
	param_set_pt_v3( h, auto_angle );

	h++;
	param_set_pt( h, image_in);
	param_set_pt_v3( h, angle_in );

	h++;
	param_set_pt( h, s_image_out);
	param_set_pt_v3( h, angle_out );

	h++;
	param_set_pt( h, background_volume);
	param_set_pt( h, silent_len);
	param_set_pt( h, b_trig_play);
	param_set_pt( h, snd_index_to_play);
	param_set_pt( h, snd_index_playing);
	param_set_pt( h, snd_index_playing_last);

	h++;
	param_set_pt( h, focal_min);
	param_set_pt( h, focal_max);

	err_param_init_pt(h);
}
void	c_strato::deinit()
{
	IF_FREE_AND_NULL( hd_bdd_snd );
}

void	c_strato::init()
{
	if( b_army)
	{
		c_bdd_snd_wave**	hd;
		c_bdd_snd_wave*		p;
		char	filename[32];

		strcpy( filename,  "./Audio/s_01.wav" );

		//INT32	i;
		hd_bdd_snd = (c_bdd_snd_wave **) MALLOC( sizeof(c_bdd_snd_wave*) * SND_NB );
		hd = hd_bdd_snd;
		for( INT32 i=0; i<SND_NB; ++i )
		{
			p = new c_bdd_snd_wave;
			*hd = p;
			strnum::make( filename+10, 2, i+1 );
			GOOD_PRINT_STRING( "Loading %s", filename );
			p->set_wave_filename( filename);
			p->update();
			hd++;
		}
	}
	else
		hd_bdd_snd = NULL;
	b_first = FALSE;
}

CONSTRUCTOR_CREATE(c_strato)
{
	param_init_with( n_strato::param, n_strato::PARAM_NB_MAX ); // strato_param, STRATO_PARAM_NB_MAX);
	s_image_last = 0;
	snd_index_playing_last = 0;
	b_first = TRUE;
}

c_strato::~c_strato()
{
	deinit();
}

void	c_strato::reset()
{
	b_trig_reset = FALSE;
	image_in = 1.;
	b_auto = FALSE;

	if( b_army)
		{
		if( snd_index_playing && hd_bdd_snd)
			{
			c_bdd_snd_wave*		p;
			p = *(hd_bdd_snd+snd_index_playing-1);
			p->stop();
			p->update();
			}
		snd_index_playing = 0;
		snd_index_playing_last = 0;
		snd_index_to_play = 1;
		}
	time_ready = n_aaatime::get()+5.;
}

void	c_strato::update()
{
	c_bdd_snd_wave*		p;
	REAL				d;

	if( !is_active() )
		return;

	if( b_first )
		init();
	if( b_trig_reset )
		reset();
	d = dist_v3r( angle_last, angle_in);
	if( d < auto_angle_tolerance )
		{
		if( auto_start_time == 0. )
			auto_start_time = n_aaatime::get();
		else
			{
			auto_delay_left = auto_delay - (n_aaatime::get() - auto_start_time);
			if( auto_delay_left < 0)
				b_auto = TRUE;
			}
		}
	else
		{
		auto_start_time = 0.;
		if( b_auto )
			reset();
		}
	cpy_v3r( angle_last, angle_in );

	if( b_auto )
		{
		image_in += .002;
		if( image_in >= 11. )
			image_in = 1.;
		angle_out[0] = auto_angle[0]*.15;
		angle_out[1] = auto_angle[1]*2.;
		angle_out[2] = auto_angle[2]*.2;
		}
	else
		{
		cpy_v3r( angle_out, angle_in );
		}
	s_image_out = image_in;

	if ( b_army )
		{
		if( s_image_last != s_image_out )
			{
			switch( s_image_out )
				{
				case 1:		snd_index_to_play = 1;	break;
				case 2:		snd_index_to_play = 3;	break;
				case 3:		snd_index_to_play = 4;	break;
				case 5:		snd_index_to_play = 6;	break;
				case 8:		snd_index_to_play = 21; break;
				case 10:	snd_index_to_play = 25;	break;
				default:	snd_index_to_play = 0;	break;
				}
			//todo	select by ref using lua
			ERR_PRINT_STRING( "%s() correction have to be done here", __FUNCTION__ );
			//p_bdd_boxes = c_bdd_boxes::get_from_channel(s_image_out);
			s_image_last = s_image_out;
			}

		if( snd_index_playing && hd_bdd_snd)
			{
			p = *(hd_bdd_snd+snd_index_playing-1);
			p->update();
			if( !p->is_playing() )
				{
				snd_index_playing_last = snd_index_playing;
				snd_index_playing = 0;
				time_ready = n_aaatime::get()+silent_len;
				}
			}
		else
			{
			if( p_bdd_boxes )
				{
				INT32	i;
				i = p_bdd_boxes->get_control_in( FMOD(angle_out[1]+.5, 1.)-.5, -angle_out[0]*2., 0. );
				if( i && i!=snd_index_playing_last )
					snd_index_to_play = i;
				}
			if ( n_aaatime::get()>time_ready && snd_index_to_play && hd_bdd_snd )
				{
				b_trig_play = FALSE;
				p = *(hd_bdd_snd+snd_index_to_play-1);
				p->start();
				snd_index_playing = snd_index_to_play;
				snd_index_to_play = 0;
				p->update();		
				}
			}
		}

}
