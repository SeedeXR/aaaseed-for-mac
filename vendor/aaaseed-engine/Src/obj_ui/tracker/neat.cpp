
#include "obj_ui/tracker/neat.h"

#if AAA_TRACKER_NEAT()
#include "draw/render.h"
#include "infrastructure/param/param_declare.h"
#include "strnum.h"
#include "spy.h"

#ifndef AAA_SYSTEMUTILS_H
#include "system/shared/SystemUtils.h"
#endif


bool	b_midi_done_with_neat = false;
bool	b_neat_done_with_midi = false;

FACTORY_CREATE_V1( c_neat, neat, Neat, neat );

static INT32 CONST	FILTER_FACTOR = .90;

INT32	neat_port_channel_one = 0;
INT32	neat_port_channel_two = 0;	
c_neat::TYPE	neat_tng_type_one = c_neat::TNG3;
c_neat::TYPE	neat_tng_type_two = c_neat::TNG3;

C_PCHAR_C	c_neat::str_type[TYPE_NB] =
{
	"TNG 2",
	"TNG 3",
};

c_neat*	neat1;
c_neat*	neat2;

#ifdef	WIN32

static	INT32	neat_count = 0;

//	LPOVERLAPPED_COMPLETION_ROUTINE
void	WINAPI	neat_marker_get( DWORD error_code, DWORD nb_byte, LPOVERLAPPED over )
{
	//INT32	i;
	INT32	nb_value;
	c_neat*	neat = nullptr;
	++neat_count;
	if( neat1 )
		{
		if( over == &(neat1->gover) )
			neat = neat1;
		}
	if(	!neat )
		{
		if( neat2 )
			{
			if( over == &(neat2->gover) )
				neat = neat2;
			}
		else
			{
			BOX_ERR("eh Man you got a neat serious pb");
			return;
			}
		}
	if( error_code )
		{
		ERR_PRINT_STRING( "%s() callback error", __FUNCTION__ );
		}
	if( nb_byte ==  1 )
		{
		//printf( "%d ", (INT32) buf[0]);
		if( neat->buf[0] == neat->tng_marker && neat->nb_recu>=neat->tng_seq_len )
			{
			//printf( "Ok\n");
			neat->nb_recu = 0;
			}
		else if(neat->nb_recu<neat->tng_seq_len)
			{
			neat->recu[neat->nb_recu]=(INT32)(neat->buf[0]);
			neat->nb_recu++;
			if(neat->nb_recu == neat->tng_seq_len)
				{
				switch ( neat->tng_type)
					{
					case c_neat::TNG2:
						nb_value = 4;
						break;
					case c_neat::TNG3:
						nb_value = 8;
						if( neat->tng_marker == 85 )
							neat->tng_marker = 170;
						else
							neat->tng_marker = 85;
						break;
					}
				neat->trame++;
				for( INT32 i=0; i<nb_value; ++i )
					{
#if	COOKING_OUT_AVERAGER
					REAL	tmp;
					neat->raw[i].put( recu[i], __FUNCTION__ );
					tmp = last[i]+(REAL)recu[i];
					last[i] = FILTER_FACTOR *tmp;
					tmp *= (1-FILTER_FACTOR);
					neat->control_put( i+1, tmp);
#else
					neat->sensor->control_put( i+1, neat->recu[i]);
#endif
					// hack
					neat->raw_data[i] = neat->recu[i];
					}
//				if( recu[0] != 0 || recu[1] != 0 || recu[2] != 0 || recu[3] != 0 )
//					printf( "\t%d\t%d\t%d\t%d\n", recu[0], recu[1], recu[2], recu[3]);
				}
			}
		}
	if( ReadFileEx( neat->hCom, (LPVOID) (neat->buf), 1,
							&(neat->gover), neat_marker_get ) )
		{
		}
	else
		{
		neat->dwError = aaa::system::get_err_last();
		ERR_PRINT_STRING( "neat_marker_get error : %d", neat->dwError );
		}
}

/*		if( ReadFile( hCom, (LPVOID) buf, 4, &read_nb, nullptr) )
			{
			if( read_nb ==  4 )
				{
				if( buf[0] != 0xff && buf[1] != 0xff && buf[2] != 0xff && buf[3] != 0xff )
					{
					if( buf[0] != 0 || buf[1] != 0 || buf[2] != 0 || buf[3] != 0 )

					printf( "\t%d\t%d\t%d\t%d\n", (INT32) buf[0], (INT32) buf[1], (INT32) buf[2], (INT32) buf[3]);
					}
				}
			else
				{
				printf( "ERG");
				}
			}
*/
#endif

void	c_neat::update_low()
{
//	ReadFileEx( hCom, (LPVOID) buf, 1,
//							&gover, neat_marker_get );
	//WaitForSingleObjectEx( hCom, 50, TRUE);
#ifdef	WIN32
//	Sleep( 0);
	spy::sleep_ex( 0, true, "neat" );
#endif
/*
	recu[0] = averager[0].get_average();
	recu[1] = averager[1].get_average();
	recu[2] = averager[2].get_average();
	recu[3] = averager[3].get_average();
*/
//	printf("count %d\n", neat_count );
//	if( recu[0] != 0 || recu[1] != 0 || recu[2] != 0 || recu[3] != 0 )
//		printf( "Car %d -> Trame %d :\t%d\t%d\t%d\t%d\n", neat_count, trame, recu[0], recu[1], recu[2], recu[3]);
}

void	c_neat::set_tng_type( TYPE type_in)
{
	tng_type = type_in;
	switch( tng_type)
		{
		case TNG2:
			tng_marker = 0xff;
			tng_seq_len = 4;
//			sensor->channel_nb = 4;
			break;
		case TNG3:
			tng_marker = 85;
			tng_seq_len = 9;
//			channel_nb = 8;
			break;
		}

}

//hackhack c_sensor change
namespace	n_neat
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 20;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 0;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(				active )
		PARAM_DEF_BOOL_ON(				open )
		PARAM_DEF_BOOL_OFF(				verbose )
		PARAM_DEF_SYMBO_LOCKED_PSTR(	tng_version,	c_neat::str_type	)

		PARAM_DEF_0_7(					control,	PARAM_DEF_REAL_ONE_ZERO	)
		PARAM_DEF_0_7(					button,		PARAM_DEF_REAL_ONE_ZERO	)
	};
}

void	c_neat::param_init_pt()
{
	INT32	h = 0;
	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _b_open_ui );
	param_set_pt( h, _b_verbose_ui );
	param_set_pt( h, tng_type );

	param_set_pt_n( h, sensor->data, 16 );

	err_param_init_pt( h );
}

//todoq	deal with alloc error :
//		no averager
CONSTRUCTOR_CREATE( c_neat )
{
	sensor = new c_sensor(16);
	param_init_with( n_neat::param, n_neat::PARAM_NB_MAX ); // neat_param, NEAT_PARAM_NB_MAX);
//hackhack c_sensor change
//	sensor.set_channel(16);
	trame = 0;
	nb_recu = 0;
	set_tng_type( TNG3);

#if	COOKING_OUT_AVERAGER
//INT32	i;
	for( INT32 i=0; i<9; ++i )
		last[i] = 0;
#endif
#ifdef	WIN32
	gover.Offset     = 0; 
	gover.OffsetHigh = 0;
	gover.hEvent     = nullptr; 
#endif
	set_active( true );
	calibrate_default();
}

c_neat::~c_neat()
{
	// todofranz dealloc
	close();
}

void	c_neat::calibrate_default()
{
//INT32	i;
//hackhack c_sensor change
INT32	channel_nb = 16;
	for( INT32 i = 1; i <= channel_nb; ++i )
		{
//hackhack c_sensor change
		sensor->control_set_filter_factor(i,.5);
//hackhack c_sensor change
//		averager[i].calibrate( 0., 1., 0., 254.);
		}
}
/*
	if( ReadFile( hCom, (LPVOID) buf, 1, &read_nb, nullptr) )
		{
		if( read_nb ==  1 )
			{
			printf( "%d ", (INT32) buf[0]);
			++nb;
			if( nb == 5)
				{
				nb = 0;
				printf("\n");
				}
			}
		else
			{
			printf( "read_nb = %d\n", read_nb);
			nb = 0;
			}
		}
	else
		nb = 0;
*/

void	c_neat::close()
{
	if( is_active() && _b_open )
	{
#ifdef	WIN32
		CloseHandle(hCom);  
		SWITCH_PRINT_STATE( "Close Neat async on COM%1d", port_nb);
#endif
		_b_open = false;
	}
}

#ifdef	_MSC_VER
#	pragma warning ( disable : 4800)
#endif
AAA_ERR	c_neat::open()
{
#ifdef	WIN32
char	port_str[7];

	if( !_b_open )
	{
		if( port_nb < 10 )
		{
			strcpy( port_str, "COMx:");
			port_str[3] = '0' + port_nb ;
		}
		else
		{
			strcpy( port_str, "COMxx:");
			strnum::make( &port_str[3], 2, port_nb);
		}

		wchar_t * wch = sysutils::utf8_to_unicode( port_str );
		hCom = CreateFileW(	wch,
							GENERIC_READ,	// | GENERIC_WRITE,
							0,				//	comm devices must be opened w/exclusive-access
							nullptr,		//	no security attrs
							OPEN_EXISTING,	//	comm devices must use OPEN_EXISTING
							FILE_FLAG_OVERLAPPED,
							nullptr			//	hTemplate must be NULL for comm devices
							);
		sysutils::free_str_tmp( wch );

		if(hCom == INVALID_HANDLE_VALUE)
		{
			dwError = aaa::system::get_err_last();

			//	handle error
			BOX_WAR( "CreateFile \"%s\" error : %d", port_str, dwError);
			//printf("CreateFile COM1 error : %d \n", dwError);
			goto exit_direct;
		}

		//
		// Omit the call to SetupComm to use the default queue sizes.
		// Get the current configuration.
		//
		BOOL fSuccess = GetCommState(hCom, &dcb);

		if(!fSuccess)
		{
			// Handle the error.
			BOX_WAR( "GetCommState \"%s\" error", port_str);
			goto exit;
		}

		//	Fill in the DCB: baud=9600, 8 data bits, no parity, 1 stop bit.
		switch( tng_type)
		{
		case TNG2:
			dcb.BaudRate = 9600;
			break;
		case TNG3:
			dcb.BaudRate = 19200;
			break;
		}
		dcb.ByteSize = 8;
		dcb.Parity = NOPARITY;
		dcb.StopBits = ONESTOPBIT;

		fSuccess = SetCommState(hCom, &dcb);

		if(!fSuccess)
		{
			//	Handle the error.
			BOX_WAR( "SetCommState \"%s\" error", port_str);
			goto exit;
		}


		if( ReadFileEx( hCom, (LPVOID) buf, 1,
							&gover, neat_marker_get ) )
		{
			CHAR	str[256];
			sprintf( str, "Open Neat async on \"%128s\"", port_str);
			SWITCH_PRINT_STATE( str, true );
			_b_open = true;
			return AAA_OK;
		}
		else
		{
			dwError = aaa::system::get_err_last();
			BOX_WAR( "ReadFileEx \"%s\" error : %d", port_str, dwError);
			goto exit;
		}
exit:
		close();
	}
exit_direct:
#endif
	return ERR_ANY;
}
#ifdef	_MSC_VER
#	pragma warning ( default : 4800)
#endif
REAL	neat_control_get( INT32 channel_id, INT32 control_id )
{
INT32	control0 = IMOD(control_id-1,8);
	if( channel_id == 1 )
		{
		if( neat1 )
//hackhack c_sensor change
			return neat1->control_get(control_id );
//			return 0.;
		else
			return 0.;
		}
	else if( channel_id == 2 )
		{
		if( neat2 )
//hackhack c_sensor change
			return neat2->control_get(control_id );
//			return 0.;
		else
			return 0.;
		}
	return 0.;
}

void	neat_control_set( INT32 channel_id, INT32 control_id, REAL value )
{
	if( channel_id == 1 )
		{
//hackhack c_sensor change
		if( neat1 )
			neat1->control_put( control_id, value);
		}
	else if( channel_id == 2 )
		{
//hackhack c_sensor change
		if( neat2 )
			neat2->control_put( control_id, value);
		}
}

void	neat_draw()
{
	if( neat1)
		neat1->draw();
	if( neat2)
		neat2->draw();
}

void	neat_init()
{
	if( neat_port_channel_one )
	{
		neat1 = new c_neat;
		neat1->set_tng_type( neat_tng_type_one );
		neat1->set_port_nb( neat_port_channel_one );
		neat1->open();
		neat1->set_focus();
	}
	if( neat_port_channel_two )
	{
		neat2 = new c_neat;
		neat2->set_tng_type( neat_tng_type_two );
		neat2->set_port_nb( neat_port_channel_two );
		neat2->open();
		neat2->set_focus();
	}
}

void	neat_update()
{
	if( neat1)
		neat1->update();
	if( neat2)
		neat2->update();
}

void	neat_open()
{
	if( neat1 )
		neat1->open();
	if( neat2 )
		neat2->open();
}

void	neat_close()
{
	if( neat1 )
		neat1->close();
	if( neat2 )
		neat2->close();
}

void	neat_toggle_draw_mode()
{
//hackhack c_sensor change

	if( neat1 )
		neat1->toggle_draw_mode();
	if( neat2 )
		neat2->toggle_draw_mode();

}

/*	synchronous version
void	c_neat::start()
{
	hCom = CreateFile(	"COM1",
						GENERIC_READ,	// | GENERIC_WRITE,
						0,				//	comm devices must be opened w/exclusive-access
						nullptr,		//	no security attrs
						OPEN_EXISTING,	//	comm devices must use OPEN_EXISTING
						0,				//	not overlapped I/O
						nullptr			//	hTemplate must be NULL for comm devices
						);

	if(hCom == INVALID_HANDLE_VALUE)
		{
		dwError = aaa::system::get_err_last();

		//	handle error
		printf("CreateFile COM1 error : %d \n", dwError);
		goto exit_direct;
		}

	//
	// Omit the call to SetupComm to use the default queue sizes.
	// Get the current configuration.
	//

	BOOL fSuccess = GetCommState(hCom, &dcb);

	if(!fSuccess)
		{
		// Handle the error.
		printf("GetCommState COM1 error\n");
		goto exit;
		}

	//	Fill in the DCB: baud=9600, 8 data bits, no parity, 1 stop bit.

	dcb.BaudRate = 9600;
	dcb.ByteSize = 8;
	dcb.Parity = NOPARITY;
	dcb.StopBits = ONESTOPBIT;

	fSuccess = SetCommState(hCom, &dcb);

	if(!fSuccess)
		{
		//	Handle the error.
		printf("SetCommState COM1 error\n");
		goto exit;
		}

	printf("neat ON\n");

	nb = 10;
	for(;;)
		{
		for(;;)
			{
			if( ReadFile( hCom, (LPVOID) buf, 1, &read_nb, nullptr) )
				{
				if( read_nb ==  1 )
					{
					//printf( "%d ", (INT32) buf[0]);
					if( buf[0] == 0xff )
						break;
					}
				else
					{
					printf( "ER");
					}
				}
			}
		if( ReadFile( hCom, (LPVOID) buf, 4, &read_nb, nullptr) )
			{
			if( read_nb ==  4 )
				{
				if( buf[0] != 0xff && buf[1] != 0xff && buf[2] != 0xff && buf[3] != 0xff )
					{
					if( buf[0] != 0 || buf[1] != 0 || buf[2] != 0 || buf[3] != 0 )

					printf( "\t%d\t%d\t%d\t%d\n", (INT32) buf[0], (INT32) buf[1], (INT32) buf[2], (INT32) buf[3]);
					}
				}
			else
				{
				printf( "ERG");
				}
			}
		}
	}
exit:
stop();
exit_direct:
}
*/

#endif	//#if AAA_TRACKER_NEAT()

