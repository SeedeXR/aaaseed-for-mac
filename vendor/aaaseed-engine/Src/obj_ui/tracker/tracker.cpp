#include "err.h"
#include "tracker.h"
#include "draw/render.h"
#include "gol/gol_light.h"
#include "gol/gol_matrix.h"

FACTORY_ABSTRACT_CREATE_V1( c_tracker, tracker, Tracker );
CONSTRUCTOR_ABSTRACT_CREATE(c_tracker)
,_b_open{false}
{}

EMPTY_DESTRUCTOR(c_tracker)

AAA_ERR	c_tracker::init()
{
	return AAA_OK;
}

void	c_tracker::update()
{
	if( _b_open != _b_open_ui )
	{
		if( _b_open_ui )
			_b_open_ui = NOERR(open());
		else
			close();
		_b_open = _b_open_ui;
	}
	if( _b_open && is_active() )
		update_low();
}

void	c_tracker::idle()
{
}

void	c_tracker::make_name( o_str& dst, o_str CONST & src, INT32 index )
{
	dst.set( src );
	dst.add_char( '.' );
	dst.add_char( 'a' + index );
}

c_sensor::c_sensor( INT32 channel_nb_in )
{
	b_verbose = false;
	init( channel_nb_in );
}

c_sensor::~c_sensor()
{
	dealloc();
}

//hackhack should change dynamically the channel_nb
AAA_ERR	c_sensor::alloc( INT32 channel_nb_in)
{
	AAA_ERR	retcode = ERR_ANY;

	averager = new c_averager[channel_nb_in];
	data = new REAL[channel_nb_in];
	if( averager && data)
	{
		channel_nb = channel_nb_in;
		for( INT32 i=0; i<channel_nb; ++i )
			data[i] = 0.;
		switch( channel_nb )
		{
		case 4:	averager[3].set_color( 1, 1, 1);
		case 3:	averager[2].set_color( 0, 0, 1);
		case 2:	averager[1].set_color( 0, 1, 0);
		case 1:	averager[0].set_color( 1, 0, 0);
				break;
		default:
				for( INT32 i=0; i<channel_nb; ++i )
					averager[i].set_color( (REAL)i/(REAL)channel_nb, 0, 0);
				break;
		}
		retcode = AAA_OK;
	}
	else
		dealloc();
	return retcode;
}

AAA_ERR	c_sensor::init( INT32 channel_nb_in )
{
	draw_mode = 0;
	channel_nb = 0;
	averager = nullptr;
	data = nullptr;
	return alloc( channel_nb_in);
}
void	c_sensor::dealloc()
{
	SAFE_DELETE_ARRAY( averager );
	SAFE_DELETE_ARRAY( data );
}

void	c_sensor::calibrate_start( INT32 channel_start, INT32 channel_stop)
{
	for( INT32 i=channel_start-1; i<channel_stop; ++i )
	{
		averager[i].calibrate_start();
	}
}

void	c_sensor::calibrate_stop( INT32 channel_start, INT32 channel_stop)
{
	for( INT32 i=channel_start-1; i<channel_stop; ++i )
	{
		averager[i].calibrate_stop();
	}
}

void	c_sensor::calibrate_start()
{
	calibrate_start( 1, channel_nb );
}

void	c_sensor::calibrate_stop()
{
	calibrate_stop( 1, channel_nb );
}

void	c_sensor::set_draw_mode( INT32	draw_mode_in )
{
	draw_mode = draw_mode_in & 0x3;
}

void	c_sensor::toggle_draw_mode()
{
	set_draw_mode( draw_mode -1 );
}


#define	SLIDER_SIZE_X			10
#define	SLIDER_SIZE_Y			50
#define	SLIDER_OFFSET_X			1
#define	SLIDER_OFFSET_Y			1
#define	SLIDER_SPACE			2

void	c_sensor::draw()
{
	if( draw_mode)
	{
		REAL	height;
		height = REAL(100)/(REAL)channel_nb;

		GOL::matrix::set_ortho_with_secu_margin( 100, 100 );
		GOL::disable_lighting();
		
		if( draw_mode & 1 )
		{
			for( INT32 i=channel_nb-1; i>=0; --i )
			{
				averager[i].draw_curve_in_rect( 0, i*height, 100, height, 1. );
			}
		}

		if( draw_mode & 2 )
		{
			for( INT32 i=0; i<channel_nb; --i )
			{
				averager[i].draw_slider_in_rect( 
							REAL(SLIDER_OFFSET_X + i * (SLIDER_SIZE_X+SLIDER_SPACE)),
							SLIDER_OFFSET_Y,
							SLIDER_SIZE_X,
							SLIDER_SIZE_Y);
			}
		}
	}
}
