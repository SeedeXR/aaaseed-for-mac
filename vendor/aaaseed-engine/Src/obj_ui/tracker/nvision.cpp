#include "nvision.h"

#if AAA_TRACKER_NVISION()

#include "obj_ui/com/serial.h"
#include "time/aaa_time.h"
#include "infrastructure/param/param_declare.h"

FACTORY_CREATE_V1( c_nvision, nvision, NVision, nvision );
c_nvision*		g_nvision_cur = nullptr;

namespace	n_nvision
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 5;
	CONSTEXPR INT32 BUT_PARAM_NB	= 7;
	CONSTEXPR INT32 SUM_PARAM_NB	= 6;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	( BUT_PARAM_NB + 1 ) * c_nvision::BUT_NB
									+	SUM_PARAM_NB + 1
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF( active )
		PARAM_DEF_BOOL_OFF( open )
		PARAM_DEF_BOOL_OFF( verbose )
		PARAM_DEF_BOOL_OFF( verbose_low_level )

		PARAM_DEF_REAL_ONE_ZERO( time_long )

		PARAM_DEF_GROUP( left, BUT_PARAM_NB )
			PARAM_DEF_BOOL_OFF( button_left )
			PARAM_DEF_BOOL_OFF( button_left_short )
			PARAM_DEF_BOOL_OFF( button_left_short_trig_push )
			PARAM_DEF_BOOL_OFF( button_left_short_trig_release )
			PARAM_DEF_BOOL_OFF( button_left_long )
			PARAM_DEF_BOOL_OFF( button_left_long_trig_push )
			PARAM_DEF_BOOL_OFF( button_left_long_trig_release )
		PARAM_DEF_GROUP( right, BUT_PARAM_NB )
			PARAM_DEF_BOOL_OFF( button_right )
			PARAM_DEF_BOOL_OFF( button_right_short )
			PARAM_DEF_BOOL_OFF( button_right_short_trig_push )
			PARAM_DEF_BOOL_OFF( button_right_short_trig_release )
			PARAM_DEF_BOOL_OFF( button_right_long )
			PARAM_DEF_BOOL_OFF( button_right_long_trig_push )
			PARAM_DEF_BOOL_OFF( button_right_long_trig_release )
		PARAM_DEF_GROUP( global, SUM_PARAM_NB )
			PARAM_DEF_REAL_ONE_ZERO( global_short				)
			PARAM_DEF_REAL_ONE_ZERO( global_short_trig_push		)
			PARAM_DEF_REAL_ONE_ZERO( global_short_trig_release	)
			PARAM_DEF_REAL_ONE_ZERO( global_long					)
			PARAM_DEF_REAL_ONE_ZERO( global_long_trig_push		)
			PARAM_DEF_REAL_ONE_ZERO( global_long_trig_release	)
		};
}

void	c_nvision::param_init_pt()
{
	INT32	h=0;
	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _b_open		);
	param_set_pt( h, _b_verbose		);
	param_set_pt( h, _b_verbose_low	);
	param_set_pt( h, _time_long		);

	for( INT32 i = 0; i < BUT_NB; ++i )
	{
		++h;
		param_set_pt( h, _b_but						[i] );
		param_set_pt( h, _b_but_short				[i] );
		param_set_pt( h, _b_but_short_trig_push		[i] );
		param_set_pt( h, _b_but_short_trig_release	[i] );
		param_set_pt( h, _b_but_long				[i] );
		param_set_pt( h, _b_but_long_trig_push		[i] );
		param_set_pt( h, _b_but_long_trig_release	[i] );
	}

	++h;
	param_set_pt( h, _short_sum					);
	param_set_pt( h, _short_trig_push_sum		);
	param_set_pt( h, _short_trig_release_sum	);
	param_set_pt( h, _long_sum					);
	param_set_pt( h, _long_trig_push_sum		);
	param_set_pt( h, _long_trig_release_sum		);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE( c_nvision )
{
	param_init_with( n_nvision::param, n_nvision::PARAM_NB_MAX ); // nvision_param, NVISION_PARAM_NB_MAX);
	_serial = nullptr;
}

c_nvision::~c_nvision()
{
	close();
}

void	c_nvision::set_port( INT32 port_id)
{
	_com_port_nb = port_id;
}

static	unsigned	char buf[128];
void	c_nvision::update_low()
{
INT32 nb;

	for( INT32 i = 0; i < BUT_NB; ++i )
	{
		_b_but_short_trig_push		[i] = false;
		_b_but_short_trig_release	[i] = false;
		_b_but_long_trig_push		[i] = false;
		_b_but_long_trig_release	[i] = false;
	}
	_short_trig_push_sum = 0;
	_short_trig_release_sum = 0;
	_long_trig_push_sum = 0;
	_long_trig_release_sum = 0;
	nb = _serial->read_get_nb_ready();
	if( nb != 0 )
	{
		nb = MIN( 128, nb );
		if( NOERR( _serial->read( buf, nb ) ) )
		{
			if( _b_verbose_low )
			{
				VERBOSE_PRINT_STRING( "NVIsion received :" ); 
				for( INT32 i = 0; i < nb; ++i )
					VERBOSE_PRINT_STRING( "\t%2x", (INT32)buf[i] );
			}
			for( INT32 i = 0; i < nb; ++i )
			{
				if( buf[i] != 0x80 )
				{
					bool b;
					for( INT32 index = 0; index < BUT_NB; ++index )
					{
						b = (buf[i] & (1<<(5-index))) != 0;
						if( _b_but[index] )	//if was pushed
						{
							if( !b ) //if just released
							{
								_b_but[index] = false;
								if( _b_but_long[index] )
								{
									_b_but_long[index] = false;
									_b_but_long_trig_release[index] = true;
									if( _b_verbose )
										GOOD_PRINT_STRING( "NVision %s button long released", index?"Left":"Right" );
								}
								else
								{
									_b_but_short[index] = false;
									_b_but_short_trig_release[index] = true;
									if( _b_verbose )
										GOOD_PRINT_STRING( "NVision %s button short released", index?"Left":"Right" );
								}
							}
						}
						else	//if was released
						{
							if( b )	//if just pushed
							{
								_b_but[index] = true;
								if( _time_long==0. )
								{
									_b_but_short[index] = true;
									_b_but_short_trig_push[index] = true;
									if( _b_verbose )
										GOOD_PRINT_STRING( "NVision %s button short push", index?"Left":"Right" );
								}
								else
									_but_time_push[index] = aaa::time::get();
							}
						}
					}
				}
			}
		}
	}
	if( _time_long!=0. )
	{
		for( INT32 index = 0; index < BUT_NB; ++index )
		{
			if( _b_but[index] && !_b_but_long[index] )
				{
				if( (aaa::time::get() - _but_time_push[index]) > _time_long )
				{
					_b_but_long[index] = true;
					_b_but_long_trig_push[index] = true;
					if( _b_verbose )
						GOOD_PRINT_STRING( "NVision %s button long push", index?"Left":"Right" );
				}
			}
		}
	}


	if( _b_but_short[0] )
		_short_sum = -1;
	else
		_short_sum = 0;
	if( _b_but_short[1] )
		_short_sum += 1;

	if( _b_but_short_trig_push[0] )
		_short_trig_push_sum = -1;
	else
		_short_trig_push_sum = 0;
	if( _b_but_short_trig_push[1] )
		_short_trig_push_sum += 1;

	if( _b_but_short_trig_release[0] )
		_short_trig_release_sum = -1;
	else						
		_short_trig_release_sum = 0;
	if( _b_but_short_trig_release[1] )
		_short_trig_release_sum += 1;

	if( _b_but_long[0] )
		_long_sum = -1;
	else		
		_long_sum = 0;
	if( _b_but_long[1] )
		_long_sum += 1;

	if( _b_but_long_trig_push[0] )
		_long_trig_push_sum = -1;
	else					
		_long_trig_push_sum = 0;
	if( _b_but_long_trig_push[1] )
		_long_trig_push_sum += 1;

	if( _b_but_long_trig_release[0] )
		_long_trig_release_sum = -1;
	else		
		_long_trig_release_sum = 0;
	if( _b_but_long_trig_release[1] )
		_long_trig_release_sum += 1;
}

void	c_nvision::update()
{
	if( is_active() )	//todoqqq generalize this
	{
		if( _b_open )
		{
			if( !_serial )
				open();
			if( _serial )
				update_low();
		}
		else
		{
			if( _serial )
				close();
		}
	}
}

void	c_nvision::close()
{
	if( _serial )
	{
		_serial->close();
		obj_delete( _serial );
	}
	_b_open = false;
}

void	c_nvision::open()
{
	if( !_serial )
	{
		bool b = false;
		_serial = new c_serial;
		if( _serial )
		{
			_serial->set_baudrate( c_serial::BAUDRATE_1200 );
			if( NOERR( _serial->open( _com_port_nb ) ) )
				b = true;
		}
		if( !b )
		{
			box_err( "Can't start NVision" );
			close();
		}
	}
}

#endif	//#if AAA_TRACKER_NVISION()