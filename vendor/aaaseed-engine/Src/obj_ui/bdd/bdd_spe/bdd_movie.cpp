#include "err.h"
#include "file/fname.h"
#include "infrastructure/aaa_window.h"
#include "infrastructure/viewport.h"
#include "infrastructure/seedfile.h"
#include "media/video/movie_ms.h"
#include "obj_ui/bdd/bdd_spe/bdd_movie.h"

FACTORY_CREATE_PROP_V1( c_bdd_movie, bdd_movie, Movie Window, bdd_movie, sub_menu="Old"; );

namespace n_bdd_movie
{
	static	CONST	INT32	BASE_PARAM_NB	= 9;
	static	CONST	INT32	PARAM_NB_MAX	= BASE_PARAM_NB;

	static	ST_PARAM	param[PARAM_NB_MAX] =
	{
		ST_PARAM_BOOL_OFF( trig_play )
		ST_PARAM_BOOL_OFF( trig_start )
		ST_PARAM_BOOL_OFF( trig_stop )
		ST_PARAM_BOOL_OFF( trig_rewind )
		ST_PARAM_BOOL_OFF( trig_close )
		ST_PARAM_BOOL_OFF( loop )
		ST_PARAM_REAL_ZERO( pos_x )
		ST_PARAM_REAL_ZERO( pos_y )
		ST_PARAM_FILENAME( movie_filename, TYPE_IO_MOVIE, 0 )
	};
}

void	c_bdd_movie::param_init_pt()
{
	INT32	h = 0;
	param_set_pt( h, _b_trig_open );
	param_set_pt( h, _b_trig_start );
	param_set_pt( h, _b_trig_stop );
	param_set_pt( h, _b_trig_rewind );
	param_set_pt( h, _b_trig_close );
	param_set_pt( h, _b_loop );
	param_set_pt_v2( h, _pos_ui  );
	param_set_pt( h, _movie_filename );

	err_param_init_pt( h );
}

void c_bdd_movie::init()
{
	param_init_with( n_bdd_movie::param, n_bdd_movie::PARAM_NB_MAX ); // bdd_movie_param, BDD_MOVIE_PARAM_NB_MAX);
	_pos[0] = _pos[1] = 0;
}

void c_bdd_movie::param_init()
{
//	movie_filename.erase();
	_b_movie_loaded = FALSE;
	_b_play = FALSE;
}


void c_bdd_movie::alloc()
{
}

void c_bdd_movie::dealloc()
{
}

CONSTRUCTOR_CREATE(c_bdd_movie)
{
	init();
	alloc();
}

c_bdd_movie::~c_bdd_movie()
{
	dealloc();
}

void	c_bdd_movie::set_movie_filename( char* CONST filename )
{
char	tmp_filename[_MAX_PATH];
	_b_movie_loaded = FALSE;
		fname::get_rel_name( tmp_filename, filename );
	_movie_filename.set( tmp_filename);
}

AAA_ERR	c_bdd_movie::load_do_after( CONST CHAR* CONST filename )
{
	set_movie_filename( _movie_filename.get() );
	return AAA_OK;
}

void	c_bdd_movie::update()
{
/*
	if( !ds_buf && *movie_filename)
		{
		}
	if( ds_buf )
		{
		DWORD	dummy;
		ds_buf->GetCurrentPosition( (LPDWORD)&position, &dummy);
		time = ((DOUBLE)position)/((DOUBLE)byte_per_sec);
		if( freq_last != freq )
			{
			ds_buf->SetFrequency( (UINT32)(((DOUBLE)freq_int)*freq) );
			freq_last = freq;
			}
		if ( jump_to != 0.)
			{
			ds_buf->SetCurrentPosition( (UINT32)(((DOUBLE)byte_per_sec)*jump_to) );
			jump_to = 0.;
			}
		}
*/
}

extern	void	movie_idle();

void	c_bdd_movie::update_position()
{
	REAL	loc_pos[2];
	c_viewport*	viewport	=	c_viewport::get_cur();
	loc_pos[0]	=	viewport->convert_cano_to_pix_x( _pos_ui[0] );
	loc_pos[1]	=	get_render_window_sy() - viewport-> convert_cano_to_pix_y( _pos_ui[1] );
//we call it continuously to allow permanent updates
//	if ( pos[0] != loc_pos[0] || pos[1] != loc_pos[1] )
		{
		movie_position( loc_pos[0], loc_pos[1] );
//hackbpi		movie_position( loc_pos[0], loc_pos[1], 352, 288 );
		//		pos[0] = loc_pos[0];
//		pos[1] = loc_pos[1];
		}
}

void	c_bdd_movie::open()
{
	update_position();
	_b_trig_open = FALSE;
	movie_choose( _movie_filename.get() );
	update_position();
}

void	c_bdd_movie::start()
{
	update_position();
	_b_trig_start = FALSE;
	movie_play();
}

void	c_bdd_movie::stop()
{
	_b_trig_stop = FALSE;
	movie_stop();
}

void	c_bdd_movie::rewind()
{
	_b_trig_rewind = FALSE;
	movie_rewind();
}

void	c_bdd_movie::close()
{
	_b_trig_close = FALSE;
	movie_close();
}

BOOL	c_bdd_movie::is_playing()
{
	return movie_is_playing();
}

void	c_bdd_movie::draw()
{
	if ( _b_trig_open )
		{
		open();
		}
	if ( _b_trig_start )
		{
		start();
		}
	if ( _b_trig_stop )
		{
		stop();
		}
	if ( _b_trig_rewind )
		{
		rewind();
		}
	if ( _b_trig_close )
		{
		close();
		}
//	b_play = movie_is_playing();	// bug compilateur

	update_position();
	Sleep(0);
}
