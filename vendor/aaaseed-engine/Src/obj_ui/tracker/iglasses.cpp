#include "iglasses.h"

#if AAA_TRACKER_IGLASSES()

#include "err.h"
#include "obj_ui/com/serial.h"
#include "aaa_util.h"
#include "Thread/aaa_thread.h"
#include "gol/gol.h"
#include <stdio.h>

extern	void	iglasses_putByte( char byte );
extern	void	iglasses_flush();
extern	void	iglasses_putBytes( CHAR CONST *str, int nChars );

static	HMD_TYPE	hmd_type		= HMD_VIO;

void	polhemus_boresight()
{
	switch( hmd_type )
	{	//todo
	case HMD_VIO:
		break;
	case HMD_POLHEMUS:
		iglasses_putBytes("\nB01\r", 5);
		break;
	}
}

static bool b_verbose = false;

static	bool		b_continuous;
static	bool		b_iglasses_running = false;
static	bool		b_iglasses_start_done = false;
static	REAL		angle_turn[3] = {0.,0.,0.};
static	REAL		angle_last[3]= {0,0,0};

static	c_serial*	serial_iglasses	= nullptr;

void iglasses_putBytes( CHAR CONST *str, int nChars)
{
	serial_iglasses->write( (UINT8 CONST *)str, nChars);
}

INT32	count_received = 0;
//ostream &operator<<(ostream &os, CONST c_euler &p)
//{
//     return os << p.yaw() << " " << p.pitch() << " " << p.roll();
//}

void	iglasses_flip_verbose()
{
	b_verbose = !b_verbose;
}

AAA_ERR iglasses_print_version()
{
AAA_ERR	retcode = AAA_OK;
UINT8	c[128];

	switch( hmd_type)
		{	//todo should not be here
		case HMD_VIO:
			iglasses_putBytes( "!V\r", 3);
			retcode = serial_iglasses->wait_for( 'M');
			if( NOERR(retcode) )
				{
				retcode = serial_iglasses->read( c, 58);
				if( NOERR(retcode) )
					{
					c[58] = 0;
		/*			for( i=0; i<58; ++i )
						if( c[i] == 0 )
							c[i] = 32;
		*/
					GOOD_PRINT_STRING( "Got vio tracker revision string :");
					GOOD_PRINT_STRING( "\t%s", c);
					}
				retcode = serial_iglasses->read( c);
				if( NOERR(retcode) )
					{
					if( c[0] == 'O' )
						GOOD_PRINT_STRING( "Tracker passed internal test" );
					else if( c[0] == 'E' )
						ERR_PRINT_STRING( "Tracker failed internal test" );
					else
						ERR_PRINT_STRING( "Tracker strange answer to internal test" );
					}
				else
					ERR_PRINT_STRING( "Could not read tracker internal test result" );
			}
			break;
		case HMD_POLHEMUS:
			iglasses_putBytes( "cu\x00c", 3);
//			iglasses_flush();
			retcode = serial_iglasses->read_line( c, 128);
			iglasses_putBytes( "\x00c", 1);
			retcode = serial_iglasses->read_line( c, 128);
			iglasses_putBytes( "S", 1);
			retcode = serial_iglasses->read_line( c, 128);
			if( NOERR(retcode) )
				{
				GOOD_PRINT_STRING( "Got Polhemus tracker status string :");
				GOOD_PRINT_STRING( "\t%s", c);
				}
			break;
		}
	return retcode;
}

AAA_ERR iglasses_init(int ttynum)		/* param is which tty to use */
{
	if( !serial_iglasses)
		{
			// todofranz dealloc
		serial_iglasses = new c_serial;
		if( hmd_type == HMD_POLHEMUS )
			serial_iglasses->set_baudrate( c_serial::BAUDRATE_115200);
		serial_iglasses->open(ttynum);
		}

	iglasses_print_version();
	return iglasses_reset( 0, 0, EULER, CONTINUOUS);
}

void iglasses_close()
{
	iglasses_stop();
	if( serial_iglasses )
		serial_iglasses->close();
	serial_iglasses = nullptr;
}
/*
	switch( hmd_type)
		{
		case HMD_VIO:
			break;
		case HMD_POLHEMUS:
			break;
		}
*/


#ifdef WIN32
//#	include <strstrea.h>
#else
#	include <iostream.h>
#endif


static	int iglasses_getByte()
{
	int		ret;
	UINT8	buf; 

	ret = serial_iglasses->read( &buf);
	if( ERR(ret) )
		buf = 0xff;
/*
#if	AAA_DEBUG()
	if( b_verbose)
		{
		if( buf == 0xff)
			cerr << "get [error]" << endl;
		else if(buf <= 32)
			cerr << "get [" << (int) buf << "]" << endl;
		else
			cerr << "get [" << buf << "] = " << (int) buf << endl;
		}
#endif
*/
	return buf;
}

void iglasses_flush()
{
AAA_ERR	retcode;
//	return;

	retcode = serial_iglasses->flush();
}

AAA_ERR iglasses_reset( INT32 mf, INT32 tf, DATA_MODE dm, SEND_MODE sm, SEND_FORMAT sf)
{
//INT32	i;
	b_continuous = (sm == CONTINUOUS);
	count_received = 0;
	for( INT32 i=0; i<=2; ++i )
		{
		angle_turn[i] = 0.;
		angle_last[i] = 0.;
		}
	switch( hmd_type)
		{
		case HMD_VIO:
			iglasses_putBytes("!\r", 2);
		//	iglasses_flush();
			serial_iglasses->wait_for( 'O');
			iglasses_putBytes("!R\r", 3);
			serial_iglasses->wait_for( 'O');
		//	if('O' != iglasses_getByte())
		//		goto exit;

			mf = MIN( mf, (INT32) 7);
			tf = MIN( tf, (INT32) 7);

			char cmd[128];
			sprintf(cmd, "!M%d,%c,%c,%d,%d\r", dm, sm, sf, mf, tf);
#if	AAA_DEBUG()
			DBG_PRINT_STRING( "I-Glasses Tracker : Reseting with : ", cmd );
#endif
			iglasses_putBytes(cmd, strlen(cmd ));
			if('O' != iglasses_getByte())
				goto exit;
			if( b_continuous )
				serial_iglasses->write( (UINT8 *)"S");
			break;
		case HMD_POLHEMUS:
			if( b_continuous )
				iglasses_putBytes("C", 1);
			break;
		}
	return AAA_OK;
exit:
	ERR_PRINT_STRING( "I-Glasses Tracker : Couldn't initialize");
	return ERR_ANY;

}

/*
void iglasses_putByte( UINT8 byte)
{
#if	AAA_DEBUG()
	if( b_verbose)
		cerr << "put [" << byte << "] = " << (int) byte << endl;
#endif
	serial_iglasses->write(&byte);
}
*/


#define FBITS 14L
#define SHR(n) ((n)>>FBITS)
#define SHL(n) ((n)<<FBITS)
#define ONE (1L<<FBITS)
#define FONE ((float)ONE)
#define TOFIXED(n) (long)((n)*ONE)
#define TOFLOAT(n) ((float)(n)/FONE)

bool	b_euler_asked = false;
UINT8	buf_euler[256];

void iglasses_ask_euler()
{
	if( !b_continuous )
	{
		iglasses_putBytes("S", 1);
		b_euler_asked = true;
		serial_iglasses->read_ask( buf_euler, 8);
	}
}

AAA_ERR polhemus_get_euler(c_euler &p)
{
AAA_ERR	retcode;
REAL angle[3];
UINT8 buf[6*7+1];
//INT32	i;
	if( b_continuous)
		{
		retcode = serial_iglasses->wait_for_str( (UINT8 *)"01");
		if( NOERR(retcode) )
			{
			retcode = serial_iglasses->read( &buf[0], 1);
			retcode = serial_iglasses->read( &buf[0], 6*7);
			}
		}
	else
		{
		retcode = ERR_NOT_IMPLEMENTED_YET;
		}
	if( ERR(retcode) )
		{
		ERR_PRINT_STRING( "Polhemus Tracker : Cant read packet");
		goto exit;
		}
	for( INT32 i=2; i>=0; --i )
		{
		buf[(i+4)*7] = 0;
		angle[i] = atof( (CONST char *)&buf[(i+3)*7]);	
		if( ABS( angle[i] - angle_last[i] ) > 180. )
			{
			if( angle[i] < 0 )
				angle_turn[i] += 360.;
			else
				angle_turn[i] -= 360.;
			}
		angle_last[i] = angle[i];
		angle[i] += angle_turn[i];
		}

	//emitor text on the top cable on the back
	p.yaw( angle[0]);
	p.pitch( -angle[1] );
	p.roll( angle[2]);
/*
	p.pitch( angle[0] -180.);
	p.roll( angle[2] + 90.);
*/
	for( INT32 i=2; i>=0; --i )
		{
		buf[(i+1)*7] = 0;
		angle[i] = atof( (CONST char *)&buf[i*7]);	
		}
	//	it is dependent to the position of the emitor
	//	should calibrate back to the emitor
	p.x( angle[1]);
	p.y( -angle[2]);
	p.z( -angle[0]);

	if( b_verbose)
	{
		++count_received;
		if( !(count_received & 0x3f))
		{
			p.print_euler();
			VERBOSE_PRINT_STRING(" Euler got %d packets", count_received );
		}
	}
		
	b_euler_asked = false;
//	iglasses_flush();
	return AAA_OK;
exit:
	b_euler_asked = false;
//	iglasses_flush();
	return ERR_ANY;
}

#define	PACKET_SIZE	8
AAA_ERR iglasses_get_euler(c_euler &p)
{
AAA_ERR	retcode;
INT32 yaw,pitch,roll;
REAL f_val;
UINT8 checksum;
UINT8 * buf;
//INT32	i;
INT32	index;
INT32	loop_count;

	if( b_continuous)
		{
		retcode = serial_iglasses->wait_for( 0xff);
		if( NOERR(retcode) )
			{
			buf_euler[0] = 0xff;
			retcode = serial_iglasses->read( &buf_euler[1], PACKET_SIZE-1);
			index = 0;
			}
		}
	else
	{
		if( !b_euler_asked )
			iglasses_ask_euler();
		retcode = serial_iglasses->read_get();
	}
	buf = buf_euler;
	if( ERR(retcode) )
		{
		ERR_PRINT_STRING( "I-Glasses Tracker : Cant read packet");
		goto exit;
		}
	if(0xff != buf[0])
		{
		ERR_PRINT_STRING( "I-Glasses Tracker : Bad packet");
		goto exit;
		}

	loop_count = 1;
	for(;;)
		{
		checksum = 0xff;
		for( INT32 i=1; i<PACKET_SIZE-1; ++i )
			checksum += buf[i];
		if(checksum == buf[PACKET_SIZE-1])
			break;
		else
			{
			ERR_PRINT_STRING( "I-Glasses Tracker : Bad Checksum");
			if( !b_continuous)
				goto exit;
			}
		if( index > 128)
			goto exit;
		INT32	i;
		for( i=1; i<PACKET_SIZE; ++i )
			{
			if( buf[i] == 0xff )
				break;
			}
		if( i == PACKET_SIZE)
			{
			DBG_PRINT_STRING( "I-Glasses Tracker : no Oxff in this packet");
			goto exit;
			}
		else
			{
			ERR_PRINT_STRING( "found other 0xff trying again");
			retcode = serial_iglasses->read( &buf_euler[index+PACKET_SIZE], i);
			index += i;
			if( ERR(retcode) )
				BOX_WAR("iglasses_get_euler error");
			buf = &buf_euler[index];
			}
		++loop_count;
		}

	yaw = (INT16)(buf[1] << 8) | buf[2];
	pitch = (INT16)(buf[3] << 8) | buf[4];
	roll = (INT16)(buf[5] << 8) | buf[6];

	f_val = TOFLOAT(yaw*180);
	if( ABS( f_val - angle_last[0] ) > 180. )
		{
		if( f_val < 0 )
			angle_turn[0] += 360.;
		else
			angle_turn[0] -= 360.;
		}
	angle_last[0] = f_val;
	f_val += angle_turn[0];
	p.yaw( f_val);


	f_val = TOFLOAT(pitch*180);
	p.pitch( f_val);

	f_val = TOFLOAT(roll*180);
	p.roll( f_val);

	if( b_verbose)
	{
		++count_received;
		if( !(count_received & 0x3f))
		{
			p.print_euler();
			VERBOSE_PRINT_STRING(" Euler got %d packets", count_received );
		}
	}
		
	b_euler_asked = false;
//	iglasses_flush();
	return AAA_OK;
exit:
	b_euler_asked = false;
//	iglasses_flush();
	return ERR_ANY;
}

/*
AAA_ERR iglasses_getEulerPacket(c_euler &p)
{
long sum = 0xff;
unsigned char ch[3][2];
int ang;
int b;
//	iglasses_flush();
	iglasses_putBytes("S", 1);

	if(0xff != iglasses_getByte())
		{
		ERR_PRINT_STRING( "I-Glasses Tracker : Bad packet");
		goto exit;
		}

	for ( ang = 0; ang < 3; ++ang )
		for ( b = 0; b < 2; ++b )
			{
			int ret = iglasses_getByte();
			if(ret < 0)
				{
				ERR_PRINT_STRING( "I-Glasses Tracker : Bad packet");
				goto exit;
				}
			else
				sum += ch[ang][b] = ret;
			}


	if( (0xff & sum) != iglasses_getByte())
		{
		ERR_PRINT_STRING( "I-Glasses Tracker : Bad Checksum");
		goto exit;
		}

	p.yaw(*(short *) &ch[0]);
	p.pitch(*(short *) &ch[1]);
	p.roll(*(short *) &ch[2]);

	p.print_euler();

	return AAA_OK;
exit:
	iglasses_flush();
	return ERR_ANY;
}
*/

c_euler	euler_cur;

AAA_ERR	get_euler( c_euler &p )
{
	switch( hmd_type)
		{
		case HMD_VIO:
			return	iglasses_get_euler( p);
		case HMD_POLHEMUS:
			return	polhemus_get_euler( p);
		default:
			return	ERR_NOT_IMPLEMENTED_YET;
		}
}

void
#ifdef	WIN32
__cdecl
#endif
iglasses_loop( void *dummy )
{
	AAA_ERR	retcode = ERR_ANY;
	//INT32	i;
	//ph added for hack
	//	hmd_type = HMD_VIO;
	switch( hmd_type)
		{	//todo should not be here
		case HMD_VIO:
			retcode	= iglasses_init(1);
			break;
		case HMD_POLHEMUS:
			retcode	= iglasses_init(6);
			break;
		}
	euler_cur.set_average_size(7);
	if( NOERR(retcode) )
		{
		for( INT32 i = euler_cur.get_average_size()+3; i>0; --i )
			{
			retcode = get_euler( euler_cur);
			if( NOERR(retcode) )
				break;
			}
		}
	b_iglasses_running = NOERR(retcode);
	b_iglasses_start_done = true;

	while( b_iglasses_running )
	{
		switch( hmd_type)
		{
			case HMD_VIO:
				get_euler( euler_cur );
				break;
			case HMD_POLHEMUS:
				get_euler( euler_cur );
				break;
		}
	}
	thread_end();
}

AAA_ERR	iglasses_start( HMD_TYPE loc_hmd_type)
{
AAA_ERR	retcode = ERR_ANY;
	hmd_type = loc_hmd_type;

	thread_begin( iglasses_loop );

	while( !b_iglasses_start_done );
	
	if( b_iglasses_running )
		retcode = AAA_OK;
	else
	{
		ERR_PRINT_STRING( "I-Glasses Tracker : Couldn't initialize");
		retcode = ERR_ANY;
	}
	b_iglasses_start_done = false;
	return	retcode;

/*	if( ERR(iglasses_init(1)) )
		{
		ERR_PRINT_STRING( "I-Glasses Tracker : Couldn't initialize");
		}

//	iglasses_putBytes( "!V\r", 3);

	// Repeatedly poll the serial for the orientation and print it out
	while (1)
		{
		c_euler packet;

		iglasses_get_euler(packet);
		}

	iglasses_close();
*/
}

AAA_ERR	iglasses_stop()
{
AAA_ERR	retcode = AAA_OK;
	if( b_iglasses_running )
	{
		b_iglasses_running = false;
		retcode = AAA_OK;
	}
	return retcode;
}

#endif //AAA_TRACKER_IGLASSES
