#include "spaceball.h"

#if AAA_TRACKER_SPACEBALL()
#include "spaceball_glue.h"
#include "obj_ui/com/serial.h"

FACTORY_CREATE_V1( c_spaceball, spaceball, Spaceball, spaceball );

bool			b_verbose_spaceball_low = false;
c_spaceball*	spaceball = nullptr;

#if 0
c_serial*		serial_space = nullptr;

void	space_print_block( UINT8 * bl)
{
	if( b_verbose_spaceball_low )
	{
		DBG_PRINT_STRING( (CHAR*) bl );
		while( *bl != '\r' && *bl != 0 )
		{
			DBG_PRINT_STRING( "%d ", (INT32) *bl );
			++bl;
		}
	}
}

//AAA_ERR	retcode;
void	space_ask_ball_data()
{
	serial_space->write( (CONST unsigned char *)"d\r", 2  ) ;
	if( b_verbose_spaceball_low)
		VERBOSE_PRINT_STRING("asking ball data");
}

void	space_ask_key_data()
{
	serial_space->write( (CONST UINT8 *)"k\r", 2  ) ;
	if( b_verbose_spaceball_low)
		VERBOSE_PRINT_STRING("asking key data");
}

AAA_ERR	space_read_block()
{
	AAA_ERR	retcode = ERR_ANY;
	UINT8	in[256];
	INT32	i = 0;
	while(1)
	{
		retcode = serial_space->read( &in[i], 1);
		if( NOERR(retcode) )
		{
			if( in[i] == '\r' )
			{
				in[i+1] = 0;
				space_print_block( in);
				break;
			}
			++i;
			in[i] = 0;
		}
		else
			break;
	}
	return retcode;
}

namespace{
bool	b_ready = false;
};

void	space_poll_block()
{
	UINT8	buf[64];
	//INT32	i = 0;
	INT32	count = 0;
	char	in;
	while(1)
	{
		if( NOERR( serial_space->read( (UINT8 *)&in, 1) ) )
		{
			if( b_ready && (in == 'D') )
			{
				for( INT32 i = 0; i < 64; ++i )
					buf[i] = 0;
				buf[0] = in;
				if( NOERR (serial_space->read( &buf[1], 15) ) )
				{
					if( buf[15] == '\r' )
					{
						buf[15] = 0;
						if( b_verbose_spaceball_low )
							space_print_block( buf);
						b_ready = true;
						break;
					}
					else
						ERR_PRINT_STRING( "spaceball ending serial ball block");
				}
				else
					ERR_PRINT_STRING( "error spaceball reading serial ball block");
				space_ask_ball_data();
			}
			else if( b_ready && (in == 'K') )
			{
				for( INT32 i = 0; i < 64; ++i )
					buf[i] = 0;
				buf[0] = in;
				if( NOERR( serial_space->read( &buf[1], 3) ) )
				{
					if( buf[3] == '\r' )
					{
						buf[3] = 0;
						if( b_verbose_spaceball_low )
						{
							space_print_block( buf);
							VERBOSE_PRINT_STRING("key data in");
						}
						b_ready = true;
						break;
					}
					else
					{
						ERR_PRINT_STRING( "spaceball ending serial key block");
					}
				}
				else
				{
					ERR_PRINT_STRING( "spaceball reading serial key block");
				}
			}
			else if( in == '\r' )
				b_ready = true;
			else if( ready && (in != 0) )
			{
				if( b_verbose_spaceball_low )
					VERBOSE_PRINT_STRING( "got %d", (INT32) buf[0] );
				space_ask_ball_data();
//				space_ask_key_data();
				b_ready = false;
			}
		}
		else
		{
			if( b_verbose_spaceball_low )
				ERR_PRINT_STRING( "spaceball reading serial");
		}
		if( count > 5 )
		{
			count = 0;
			space_ask_ball_data();
		}
		else
			++count;
	}

	switch ( in )
		{
		case 'D':
			{
				UINT8 *m = (UINT8 *)&buf[3];
				REAL	tmp;
	//			INT32	i;
				space_ask_ball_data();

				if( spaceball )
				{
					spaceball->set_data_in( 0, -(REAL)( (short)( m[0] << 8 ) + m[1] ) / 32767.0);
					spaceball->set_data_in( 1, -(REAL)( (short)( m[2] << 8 ) + m[3] ) / 32767.0);
					spaceball->set_data_in( 2,  (REAL)( (short)( m[4] << 8 ) + m[5] ) / 32767.0);
					spaceball->set_data_in( 3, -(REAL)( (short)( m[6] << 8 ) + m[7] ) / 32767.0);
					spaceball->set_data_in( 4, -(REAL)( (short)( m[8] << 8 ) + m[9] ) / 32767.0);
					spaceball->set_data_in( 5,  (REAL)( (short)( m[10] << 8 ) + m[11] ) / 32767.0);
					if( b_verbose_spaceball_low )
					{
						GOOD_PRINT_STRING( "SpaceBall tx=%.4f, ty=%.4f, tz=%.4f, rx=%.4f, ry=%.4f, rz=%.4f",
							spaceball->get_data_in( 0 ),
							spaceball->get_data_in( 1 ),
							spaceball->get_data_in( 2 ),
							spaceball->get_data_in( 3 ),
							spaceball->get_data_in( 4 ),
							spaceball->get_data_in( 5 )
							);
					}
					//todo make fn for this
					for( INT32 i = 3; i < 6; ++i )
					{
						tmp = spaceball->get_data_in(i) * 32.;
						CLAMP_REF( tmp, REAL(-1.), REAL(1.) );
						spaceball->set_data_in( i, tmp);
					}
				}
			}
			break ;
		case 'K' :
			{
				bool	b_change = false;
				REAL	tmp;

				tmp = ((buf[2] & 0x20) != 0)?1.:0.;
				b_change = (spaceball->get_data_in(6) != tmp );
				spaceball->set_data_in( 6, tmp );

				tmp = ((buf[1] & 0x10) != 0)?1.:0.;
				b_change |= (spaceball->get_data_in(7) != tmp );
				spaceball->set_data_in( 7, tmp );

				tmp = ((buf[1] & 0x0) != 0)?1.:0.;
				b_change |= (spaceball->get_data_in(8) != tmp );
				spaceball->set_data_in( 8, tmp );

#if	AAA_DEBUG()
				if( b_change )
				{
					if( b_verbose_spaceball_low )
					{
						GOOD_PRINT_STRING( "Spaceball buttons %s %s %s",
							(spaceball->get_data_in(6)==0.)?"ON ":"OFF",
							(spaceball->get_data_in(7)==0.)?"ON ":"OFF",
							(spaceball->get_data_in(8)==0.)?"ON ":"OFF"
							);
					}
				}
#endif
	//			buttons = ((buf[1]-'@')<<3) + (buf[2]-'@') ; // One bit per button
	//			if( b_verbose_spaceball )
	//				printf( " BUTTONS : %x\n", buttons);
			}
			break ;
		}

}

static void
#ifdef	WIN32
__cdecl
#endif
th_space_ball_loop( void *dummy)
{
	if( b_verbose_spaceball_low)
	{
		GOOD_PRINT_STRING( "Spaceball looping");
	}
	while(1)
	{
//		serial_space->wait_for_str( (unsigned char *)"K");
		space_poll_block();
	}
	thread_end();
}
  
void space_ball_loop()
{
	thread_begin( th_space_ball_loop);
}


//todo space_close, and object
AAA_ERR	space_open( INT32 com_port_nb )
{
	AAA_ERR	retcode;

//	for ( i=0; i>3; ++i )
//		c_spaceball::button_in[i] = false;

	//open com
	serial_space = new c_serial;
	serial_space->set_baudrate( c_serial::BAUDRATE_9600);
	serial_space->open( com_port_nb);

	//ckeck it
		//flush
		space_read_block();

		//reset
		serial_space->write( (CONST UINT8 *)"@RESET\r", 7);
		//get first
		retcode = serial_space->wait_for_str( (UINT8 *)"@1");
		if( ERR(retcode) )
			goto exit;
		//flush
		space_read_block();

		//get second
		retcode = serial_space->wait_for_str( (UINT8 *)"@2");
		if( ERR(retcode) )
			goto exit;
		//flush
		space_read_block();


	// Make sure we were not in the middle of an operation.
	serial_space->write( (CONST UINT8 *)"\r" ,1 );	
	serial_space->write( (CONST UINT8 *)"Z\r", 2);
	// Set Spatial mode for both Translation and Rotation,
	serial_space->write( (CONST UINT8 *)"MSSV\r", 4 ) ;

//	serial_space->write( (CONST UINT8 *)"P\x00\x40\x00\x40\r", 6);
//	serial_space->write( (CONST UINT8 *)"CB\r", 3);
//	serial_space->write( (CONST UINT8 *)"CP\r", 3);

	space_ask_ball_data() ; // Request first data packet.

	space_ball_loop();
	return AAA_OK;
exit:
	DBG_PRINT_STRING( "Can't find spaceball on Com%d:", com_port_nb);
	return retcode;
}
#endif //#if 0

CONSTRUCTOR_CREATE(c_spaceball)
,_b_open		{false}	
{
	sb_init();
	param_init_with( nullptr, 0 );
}

c_spaceball::~c_spaceball()
{
	close();
}

void c_spaceball::update()
{
	open();
	update_sensor_6dof();
}

#if 0
void	c_spaceball::set_port( INT32 port_id )
{
	_port_nb = port_id;
}
#endif

AAA_ERR	c_spaceball::open()
{
	if( !_b_open )
		_b_open = sb_open();
	return _b_open ? AAA_OK : ERR_ANY;
//	return	space_open( _port_nb );
}

void	c_spaceball::close()
{
	if( _b_open )
	{
		//hack make it better
		sb_deinit();
		_b_open = false;
	}
}
#endif	//#if AAA_TRACKER_SPACEBALL()