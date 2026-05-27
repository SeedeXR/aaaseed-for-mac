#include "plage.h"
#include "infrastructure/layer/layers.h"
#include "time/ourtime.h"
//#include "infrastructure/param/trax.h"
#include "arcom88.h"
#include "infrastructure/seedfile.h"
#include "file/fname.h"
#include "file/file_csv.h"
#include "file/aaa_file.h"

FACTORY_CREATE_V1( c_plage, plage, Plage, plage );

c_plage*	plage;

namespace	n_plage
{
	static	CONST	INT32	BASE_PARAM_NB		= 13;
	static	CONST	INT32	GROUP_PARAM_NB		= 0;
	static	CONST	INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
											+	GROUP_PARAM_NB;

	static	ST_PARAM	param[PARAM_NB_MAX] =
	{
		ST_PARAM_BOOL_OFF( game )
		ST_PARAM_BOOL_OFF( ecoute )
		ST_PARAM_BOOL_OFF( play )
		ST_PARAM_BOOL_OFF( trig_start )
		ST_PARAM_BOOL_OFF( trig_stop )
		{	(void *) NULL,	PARAM_INT32,	"display",		1., 0.,		0., 6.,				NULL, NULL },
		{	(void *) NULL,	PARAM_REAL,		"delay",		0., 16.,	0., PARAM_INFINI,	NULL, NULL },
		{	(void *) NULL,	PARAM_REAL,		"time_in",		0., 16.,	0., PARAM_INFINI,	NULL, NULL },
		{	(void *) NULL,	PARAM_REAL,		"jump_to",		0., 16.,	0., PARAM_INFINI,	NULL, NULL },
		{	(void *) NULL,	PARAM_STR,		"filename",		0., 0.,		0., PARAM_INFINI,	NULL, NULL },
		{	(void *) NULL,	PARAM_INT32,	"mode",			1., 0.,		0., 2.,				NULL, NULL },
		ST_PARAM_BOOL_OFF( wait )
		ST_PARAM_BOOL_OFF( started )
	};
}

void	c_plage::param_init_pt()
{
INT32	h=0;
INT32	i=0;
	param_set_pt( h, b_game);
	param_set_pt( h, b_ecoute);
	param_set_pt( h, b_play);
	param_set_pt( h, b_trig_start);
	param_set_pt( h, b_trig_stop);
	param_set_pt( h, display);
	param_set_pt( h, delay);
	param_set_pt( h, time_in);
	param_set_pt( h, jump_to);
	param_set_pt( h, filename);
	param_set_pt( h, mode);
	param_set_pt( h, b_wait);
	param_set_pt( h, b_running);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_plage)
{
	trig_count = 0;
	param_init_with( n_plage::param, n_plage::PARAM_NB_MAX ); // plage_param, PLAGE_PARAM_NB_MAX);
	set_free();
}

c_plage::~c_plage()
{
}

void	c_plage::update()
{
	if ( trig_count > 0 )
		{
		if ( --trig_count == 0 )
			{
			b_trig_start = FALSE;
			b_trig_stop = FALSE;
			}
		}
	jump_to = 0.;
	if( b_game)
		{
		INT32	pod_in = arcom->get_in_just_one();
		INT32	display_new = get_pod(time_in+delay);
		b_wait = !b_ecoute && b_running && ( pod_waiting > 0 );
		if( b_wait )
			{
			display = pod_waiting;
			if( pod_in )//&& pod_in != pod_last)
				{
				if ( pod_in == pod_waiting )
					{
					pod_waiting = -1;
					if( index_waiting == 0 )
						start();
					}
				else
					{
					if ( index_waiting <= 1)
						jump_to = .001;
					else
						jump_to = time[index_waiting-1];
					time_in = jump_to;
					}
				}
			if ( index_cur > index_waiting )
				{
				ERR_PRINT_STRING( "J'attend le Pod %d",display);
				if ( index_waiting <= 1)
					jump_to = .001;
				else
					jump_to = time[index_waiting-1];
				time_in = jump_to;
				}
			}
		else
			{
			if( b_ecoute)
				display = display_new;
			else
				{
				if ( index_cur > index_waiting )
					{
					pod_waiting = display_new;
					index_waiting = index_cur;
					}
				else
					display = 0;
				}
			}
		if( pod_in)
			pod_last = pod_in;
		}
	else
		display = 0;
	arcom->set_out_one(display);
}

void	c_plage::set_free()
{
	b_game = FALSE;
	b_ecoute = FALSE;
	mode = 0;
	stop();
	b_running = TRUE;
}

void	c_plage::set_game()
{
	b_game = TRUE;
	b_ecoute = FALSE;
	mode = 2;
	stop();
}

void	c_plage::set_ecoute()
{
	b_game = TRUE;
	b_ecoute = TRUE;
	mode = 1;
	stop();
}

#define	TRIG_COUNT	2
void	c_plage::start()
{
	b_trig_start = TRUE;
	trig_count = TRIG_COUNT;
	b_running = TRUE;
}

void	c_plage::stop()
{
	b_trig_stop = TRUE;
	trig_count = TRIG_COUNT;
	b_running = FALSE;
}

void	c_plage::start_game()
{
	index_waiting = 0;
	pod_waiting = get_pod(index_waiting);
	stop();
	b_running = TRUE;
}



void	c_plage::start_stop()
{
	if( b_game)
		{
		if( b_play )
			stop();
		else if ( b_ecoute )
			start();
		else
			start_game();
		}
}

INT32	c_plage::get_pod(REAL t)
{
//INT32	i;
INT32	ok = 0;
	for( INT32 i=0; i<index_nb; ++i )
		{
		if( t < time[i] )
			{
			if ( i == 0)
				i = 1;
			index_cur = i-1;
			return pod[index_cur];
			}
		}
	index_cur = 0;
	return 0;
}

void	c_plage::load_song()
{
char * filename_load;
char tmp_filename[_MAX_PATH];

	b_game = FALSE;

	filename_load = get_filename_load_by_value( TYPE_IO_SND_WAVE);
	if ( filename_load && *filename_load )
		{
		fname::cpy_and_replace_ext( tmp_filename, filename_load, "txt")
		if ( aaa_filename_get_exist( tmp_filename) )
			{
			index_nb = file_csv::read_float( tmp_filename, (REAL *)data, 2, PLAGE_SEQ_NB_MAX);
			if ( index_nb & 1 )
				BOX_ERR( "Ce fichier .csv n'est pas compose de paires");
			else
				{
				//INT32	i;
				REAL*	pt;
				index_nb /= 2;
				load_data_from_filename( filename_load, TYPE_IO_SND_WAVE);
				pt = &data[0][0];
				for( INT32 i=0; i<=index_nb; ++i )
					{
					time[i] = *pt++;
					pod[i] = (INT32)*pt++;
					}
				b_game = TRUE;
				}
			}
		else
			BOX_ERR( "Le fichier .csv associ� au fichier .wav n'existe pas\nCe programme ne peut donc pas charger une chanson" );
		}
}
