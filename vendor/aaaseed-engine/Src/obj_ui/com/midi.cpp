#include "midi.h"
#include "midi_data.h"
#include "time/aaa_time.h"
#include "obj_ui/tracker/mackie.h"
#include "obj_ui/tracker/trackers.h"
#include "ui/seed_ui.h"
#include "ui/strsymbo.h"
#include "infrastructure/param/param_declare.h"
#include "spy.h"
#include "file/dirparser.h"
#include "obj_ui/tracker/slice.h"
#include "file/file_csv.h"
#include "aaa/aaa_mutex.h"


namespace
{
//todo	move index_midi in the object
	CONST	CHAR	MIDI_HEADER[] = "# MIDI ";
}

void	c_midi::CLASS_PRINT_STRING( C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	trackers::PRINT_STRING_VA( MIDI_HEADER, fmt, args );
	va_end( args );
}

void	c_midi::PRINT_STRING( C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	trackers::PRINT_STRING_VA( _midi_header.get(), fmt, args );
	va_end( args );
}

namespace
{
	aaa::MUTEX	midi_in_lock;
#if AAA_DEBUG()
	INT32		midi_in_count = 0;
#endif

	INT32	midi_file_write( FILE* file, INT32* data, INT32 x, C_PCHAR_C s1, C_PCHAR_C s2 )
	{
		if( file )
		{
			SPY_PUSH_RANGE( "midi_file_read", spy::FILE_LOW );
				INT32	nb_write = 0;
				//		INT32	i;
				for( INT32 i = 0; i < x; ++i )
				{
					fprintf( file, "%s %d %s %8x\n", s1, *data, s2, *( data + 1 ) );
					data += 2;
					++nb_write;
				}
			SPY_POP_RANGE();
			return nb_write;
		}
		return 0;
	}

	INT32	midi_file_read( FILE* file, INT32* data, C_PCHAR_C s1, C_PCHAR_C s2 )
	{
		if( file )
		{
			SPY_PUSH_RANGE( "midi_file_read", spy::FILE_LOW );
				INT32	nb_read = 0;
				char	str_scan[256];
				snprintf( str_scan, 255, "%s %%d %s %%8x\n", s1 ,s2 );
				while( fscanf( file, str_scan, data, data+1 ) == 2 )
				{
					data += 2;
					++nb_read;
				}
			SPY_POP_RANGE();
			return nb_read;
		}
		return 0;
	}

	o_str	midi_global_name;
	CHAR	midi_bank_ext[] = "midi_bank";

	INT32	midi_timeGetTime_start;
	INT32	midi_print_count;
	INT32	midi_date_start;

	INT32	midi_seq_file_write( FILE *file, INT32 *data, INT32 x )
	{
		INT32	nb_write = 0;
		if( file )
		{
			for( INT32 i = 0; i < x; ++i )
			{
				fprintf( file, "%d; %8x\n", *data, *( data + 1 ) );
				data += 2;
				++nb_write;
			}
		}
		return nb_write;
	}

	INT32	midi_seq_file_read( FILE *file, INT32 *data )
	{
		INT32	nb_read = 0;
		if( file )
		{
			char	str_scan[256];
			snprintf( str_scan, 255, "%%d; %%8x\n" );
			while( fscanf( file, str_scan, data, data + 1 ) == 2 )
			{
				data += 2;
				++nb_read;
			}
		}
		return nb_read;
	}
}


FACTORY_CREATE_V1( c_midi, midi, Midi, midipref );

//MIDI
c_midi*	midi_array[MIDI_OBJ_MAX_NB];

//bool	c_midi::b_control_send_to_lua;
//bool	c_midi::b_program_send_to_lua;

bool	c_midi::b_verbose = true;
bool	c_midi::b_verbose_display_date;
bool	c_midi::b_verbose_display_time;
bool	c_midi::b_verbose_display_count;
bool	c_midi::b_verbose_display_channel;
bool	c_midi::b_verbose_velocity_master;
bool	c_midi::b_verbose_noteon;
bool	c_midi::b_verbose_noteoff;
bool	c_midi::b_verbose_velocity;
bool	c_midi::b_verbose_aftertouch_poly;
bool	c_midi::b_verbose_aftertouch_channel;
bool	c_midi::b_verbose_program_change;
bool	c_midi::b_verbose_control_change;
bool	c_midi::b_verbose_pitchbend;
bool	c_midi::b_verbose_system_ui;
bool	c_midi::b_verbose_timecode;
bool	c_midi::b_verbose_clock;
bool	c_midi::b_verbose_low_in;
bool	c_midi::b_verbose_low_out;
bool	c_midi::b_verbose_out_callback;
bool	c_midi::b_verbose_read_bank;
bool	c_midi::b_verbose_event;

bool	c_midi::b_route_noteon_as_control_change_ui;

INT32	c_midi::bank_cur			= -1;
INT32	c_midi::bank_cur_ui			= 0;
bool	c_midi::b_bank_direct		= false;
INT32	c_midi::bank_direct_begin	= 0;
INT32	c_midi::bank_direct_end		= 127;
bool	c_midi::b_bank_load_trig	= false;
bool	c_midi::b_bank_a_store_trig = false;
bool	c_midi::b_bank_b_store_trig = false;
bool	c_midi::b_bank_a_save_trig	= false;
bool	c_midi::b_bank_b_save_trig	= false;
INT32	c_midi::digit_ui			= -1;
INT32	c_midi::digit_all			= 0;
bool	c_midi::b_dynamic_record	= false;
bool	c_midi::b_dynamic_play		= false;
bool	c_midi::b_dynamic_pause		= false;
bool	c_midi::b_dynamic_loop		= false;

INT32	c_midi::dynamic_numbers_ui		= 0;
REAL	c_midi::dynamic_duration_ui		= 0.0;
INT32	c_midi::dynamic_status_ui		= 0;
REAL	c_midi::dynamic_time_ui			= 0.0;
bool	c_midi::b_record_rec_ui			= false;
bool	c_midi::b_dynamic_play_trig_ui	= false;
bool	c_midi::b_dynamic_stop_trig_ui	= false;
bool	c_midi::b_dynamic_clear_trig_ui	= false;
bool	c_midi::b_dynamic_loop_ui		= false;
//bool	c_midi::b_playing_ui			= false;

INT32	c_midi::control_change_last_channel_id_out		=	0;
INT32	c_midi::control_change_last_control_index_out	=	-1;

namespace
{
	CONSTEXPR INT32	STR_MIDI_LEN = 256;
	thread_local CHAR str_midi[STR_MIDI_LEN];	//todo	do better
	CONSTEXPR CHAR  STR_IN[]  =  "IN  <-- ";
	CONSTEXPR CHAR  STR_OUT[] = "OUT --> ";

	c_midi_patcher_in	midi_control_patcher_in;
}

void c_midi::update_all()
{
	
	control_change_last_channel_id_out = 0;
	control_change_last_control_index_out = -1;
	bool b_bank_interpolated = false;	//todo if no midui is acyive we need to do this but perhaps we have to avoid to do it for several mackies
	for( INT32 i = 0; i < MIDI_OBJ_MAX_NB; ++i )
	{
		c_midi* p_midi = midi_array[i];
		if( p_midi )
		{
			p_midi->update();
			if( p_midi->is_active() )
				b_bank_interpolated = true;
		}
		else
			break;
	}
	if( !b_bank_interpolated )
		c_midi_universe::cur->interpolate_bank( 0 );
}

void c_midi::set_focus_all()
{
	for( INT32 i = 0; i < MIDI_OBJ_MAX_NB; ++i )
	{
		c_midi* p_midi = midi_array[i];
		if( p_midi )
			p_midi->set_focus();
		else
			break;
	}
}



//todo patch are not saved ?
AAA_ERR	c_midi::load_patchers( C_PCHAR_C filename )
{
	return midi_control_patcher_in.load_data( filename );
}

void	c_midi::build_verbose_str( CHAR* dst, INT32 len, CHAR* str, INT32 channel_id, INT32 date )
{
	CHAR* dst_begin = dst;
	//	pstr += snprintf( pstr, len, "%8d ", aaa::time::get_real_time() );
	if( c_midi::b_verbose_display_time )
		dst += snprintf( dst, len, "%8d ", timeGetTime() - midi_timeGetTime_start );
	if( c_midi::b_verbose_display_count )
		dst += snprintf( dst, len - (dst-dst_begin), "%8d ", midi_print_count++ );
	if( c_midi::b_verbose_display_date )
		dst += snprintf( dst, len - (dst-dst_begin), "DATE %8d ", date - midi_date_start );
	if( c_midi::b_verbose_display_channel )
		dst += snprintf( dst, len - (dst-dst_begin), "CH %2d ", channel_id );
	snprintf( dst, len - (dst-dst_begin), "-> %s", str );
}

//todoq add info on which midi obj in midi print
void	c_midi::class_print_verbose( CHAR* str, INT32 channel_id, INT32 date )
{
	CHAR	str_loc[256];
	c_midi::build_verbose_str( str_loc, 255, str, channel_id, date );
	c_midi::CLASS_PRINT_STRING( str_loc );
}

void	c_midi::print_verbose( CHAR* str, INT32 channel_id, INT32 date )
{
	CHAR	str_loc[256];
	build_verbose_str( str_loc, 255, str, channel_id, date );
	PRINT_STRING( str_loc );
}

void c_midi::set_verbose( bool in )
{
	b_verbose = in;
	SWITCH_PRINT_STATE( "Midi Verbose", b_verbose );
}

void c_midi::flip_verbose()
{
	set_verbose( !b_verbose );
}


void	c_midi::print_error( MMRESULT err_code )
{
	C_PCHAR	mess = nullptr;
	switch ( err_code )
	{
	case MMSYSERR_ALLOCATED:	mess = "The specified resource is already allocated.";		break;
	case MMSYSERR_BADDEVICEID:	mess = "The specified device identifier is out of range.";	break;
	case MMSYSERR_INVALFLAG:	mess = "The flags specified by dwFlags are invalid.";		break;
	case MMSYSERR_INVALPARAM:	mess = "The specified pointer or structure is invalid.";	break;
	case MMSYSERR_INVALHANDLE:	mess = "The specified device handle is invalid.";			break;
	case MMSYSERR_NOMEM:		mess = "The system is unable to allocate or lock memory.";  break;
	case MMSYSERR_NOTSUPPORTED:	mess = "function isn't supported.";							break;
	}
	if( mess )
		c_midi::PRINT_STRING( "%s : %s", get_name_str(), mess );
	else
		c_midi::PRINT_STRING( "%s : Error number %d", get_name_str(), err_code );
}
/*
void MidiErrorMessageBox( MMRESULT mmr )
{
	midiOutGetErrorText( mmr, szTemp, sizeof( szTemp ) );
	MessageBox( get_window_main_handle(), szTemp, szAppTitle, MB_OK | MB_ICONSTOP | MB_TOPMOST );
#if	AAA_DEBUG()
	wsprintf( szDebug, "Midi subsystem error: %s", szTemp );
	DebugPrint( szDebug );
#endif
}
*/


/*
char toto_str[1000000];
char* toto;

static	INT32	timer_id;
static	INT32	timer_hit;

void CALLBACK midi_timer_fn( 
  HWND hwnd,     // handle of window for timer messages
  UINT uMsg,     // WM_TIMER message
  UINT idEvent,  // timer identifier
  DWORD dwTime   // current system time
 )
{
	toto += sprintf( toto, "%8d %8.3f\n", timer_hit++, aaa::time::get_real_time() );
}
static	void	midi_timer_start()
{
	if( !b_midi_timer )
		{
		timer_id = SetTimer( 	nullptr,              // handle of window for timer messages
				0,          // timer identifier
				1,           // time-out value
				midi_timer_fn	// address of timer procedure );	
			 );
		if( timer_id == 0 )
			{
			LPVOID lpMsgBuf;
			DWORD error = aaa::system::get_err_last();
			FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,		// source and processing options
				nullptr,								// pointer to  message source
				error,								// requested message identifier
				0,									// language identifier for requested message
				( LPTSTR ) &lpMsgBuf,					// pointer to message buffer
				0,									// maximum size of message buffer
				nullptr								// address of array of message inserts
				 );
			MessageBox( nullptr, ( CONST char * )lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION | MB_TOPMOST );
			LocalFree( lpMsgBuf ); 
			}
		else
			GOOD_PRINT_STRING( "MIDI timer started" );

		toto = toto_str;
		timer_hit = 0;
		b_midi_timer = true;
		}
}


static	void	midi_timer_stop()
{
	if( b_midi_timer )
		{
		if ( KillTimer( nullptr,	// handle of window that installed timer
					timer_id )	// timer identifier );
					== FALSE )
			{
			LPVOID lpMsgBuf;
			DWORD error = aaa::system::get_err_last();
			FormatMessage( 
				FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,		// source and processing options
				nullptr,								// pointer to  message source
				error,								// requested message identifier
				0,									// language identifier for requested message
				( LPTSTR ) &lpMsgBuf,					// pointer to message buffer
				0,									// maximum size of message buffer
				nullptr								// address of array of message inserts
				 );
			MessageBox( nullptr, ( CONST char * )lpMsgBuf, "Error", MB_OK | MB_ICONINFORMATION | MB_TOPMOST );
			LocalFree( lpMsgBuf ); 
			}
		else
			GOOD_PRINT_STRING( "MIDI timer stopped" );
		b_midi_timer = false;

		printf( toto_str );
		}
}
*/

namespace
{
	CONST	INT32	MIDI_EVENTS_NB	= 1024 * 256;
}

void	c_midi::alloc_play_record_buf()
{
	if( !_play_buf )
	{
		INT32 size = MIDI_EVENTS_NB * sizeof(INT32) * 2;
		_record_buf	= (INT32*) MALLOC( size );
		_play_buf	= (INT32*) MALLOC( size );
	}
}
void	c_midi::dealloc_play_record_buf()
{
	IF_FREE_AND_NULL( _record_buf );
	IF_FREE_AND_NULL( _play_buf );
}

void	c_midi::flip_buf()
{
	SWAP( _record_buf, _play_buf );
}

void	c_midi::record_start()
{
	if( _b_record_allow )
	{
		// todo now, push current record buffer and create new one
		_b_record = true;
		_record_index = 0;
	}
}

void	c_midi::record_stop()
{
	if( _b_record )
	{
		_b_record = false;
		if( BOX_ASK_WAR( "Confirmation", "Keep recording ?" ) )
		{
			flip_buf();
			_play_nb = _record_index;
			_b_record_allow = false;
		}
	}
}

void	c_midi::play_start()
{
	if( _b_play_allow )
	{
		_b_play = true;
		_play_index = 0;
	}
}

void	c_midi::play_stop()
{
	if( _b_play )
		_b_play = false;
}

void	c_midi::start( INT32 date )
{
	midi_print_count = 0;
	midi_date_start = date;
	aaa::time::start();	//hack ? archi ?
//	midi_timer_start();
	record_start();
	play_start();
}


void	c_midi::stop( INT32 date )
{
//	midi_timer_stop();	
	record_stop();
	play_stop();
}

//todo maa say : is it really a resume ?
void	c_midi::resume( INT32 date )
{
//	midi_timer_stop();	
	record_stop();
	play_stop();
}

void	c_midi::play_update()
{
	if( _play_buf && _b_play )
	{
		INT32	t =	INT32( aaa::time::get_real_time() * 1000 );
//		INT32	t =	aaa::time::get()*1000;
		INT32*	p = _play_buf + _play_index * 2;

		while( *p <= t )
		{
			if( _play_index < _play_nb )
			{
				treat_in( *( p+1 ), *p );
				p += 2;
				++_play_index;
			}
			else
			{
				stop( t );
				break;
			}
		}
	}	
}

void	c_midi::record( INT32 data, INT32 date )
{
	alloc_play_record_buf();
	if( _record_index < MIDI_EVENTS_NB - 1 )
	{
		INT32*	p = _record_buf + 2 * _record_index;
		++_record_index;
		if( midi_date_start == 0 )
			midi_date_start = date;
		*p		=	date - midi_date_start;	
		*++p	=	data;
	}
	else
		ERR_PRINT_STRING( "MIDI recording buffer overflow" );
}

/*
FINLINE	bool	c_midi::is_valid_control( INT32 in )	//	[1,128]
{
	if( in < 1 || in > MIDI_CONTROL_NB )
		{
		sprintf( err_str, "MIDI CONTROL %d impossible, skipping", in ); 
		ERR_PRINT_STRING( err_str );
		return false;
		}
	return true;
}
*/

void	c_midi::print_low( C_PCHAR_C src, INT32 data )
{
	INT32	data_last = ( data>>24 )&0xff;
	if( data_last )
		PRINT_STRING( "%s%s%x %x %x %x", _o_verbose_name.get(), src, data&0xff, ( data>>8 )&0xff, ( data>>16 )&0xff, data_last );
	else
		PRINT_STRING( "%s%s%x %x %x",    _o_verbose_name.get(), src, data&0xff, ( data>>8 )&0xff, ( data>>16 )&0xff );
}

void	c_midi::print_low( C_PCHAR_C src, UINT8 status, UINT8 b1, UINT8 b2 )
{
	PRINT_STRING( "%s%s%x %x %x", _o_verbose_name.get(), src, INT32( status ), INT32( b1 ), INT32( b2 ) );
}

void	c_midi::print_low( C_PCHAR_C src, UINT8 status, UINT8 b1 )
{
	PRINT_STRING( "%s%s%x %x", _o_verbose_name.get(), src, INT32( status ), INT32( b1 ) );
}

void	c_midi::seq_save()
{
	if( _record_buf )
	{
		//	generate file name
		CHAR	buf[64];
		//	midi_seq_make_name( buf, sequencer_nb_ );
		snprintf( buf, 63, "sequencer.data" );

		FILE* file = file_csv::open( buf, true );
			if( file )
			{
				INT32 nb_write = midi_seq_file_write( file, _record_buf, _record_index );
				PRINT_STRING( "Sequencer save, %d record( s ) saved", nb_write );
			}
		file_csv::close( file );
	}
}

void	c_midi::seq_load()
{
	//	generate file name
	CHAR	buf[64];
	//	midi_seq_make_name( buf, sequencer_nb_ );
	snprintf( buf, 63, "sequencer.data" );

	FILE* file = file_csv::open( buf, false );
		if( file )
		{
			alloc_play_record_buf();
			INT32 nb_read = midi_seq_file_read( file, _record_buf );
			PRINT_STRING( "Sequencer load, %d record( s ) load", nb_read );
		}
	file_csv::close( file );
}

INT32	c_midi::channel_reroute( INT32 ch_id )
{
	INT32 ch_new = _channel_dst[ch_id-1];
	if( ch_new == 0 )	//	0 is nominal
		ch_new = ch_id;
	return ch_new;	//	result in [1,16]
}

void	c_midi::treat_in( INT32 data, INT32 when )
{
	INT8	channel_index = ( data & 0xf );	//internal in fn from 0 to 15
	INT8	status = ( data & 0xf0 ) >> 4;

	if( b_verbose_low_in )
		print_low( STR_IN, data );
	if( _b_record )
		record( data, when );
	if( status == 0xf )
	{
		switch( channel_index )
		{
		case 0x1:
			{
/* format is
0nnn dddd	where nnn is one of 7 possible values which tell you what dddd represents. Here are the 7 values, and what each causes dddd to represent.
Value          dddd
  0	   Current Frames Low Nibble
  1	   Current Frames High Nibble
  2	   Current Seconds Low Nibble
  3	   Current Seconds High Nibble
  4	   Current Minutes Low Nibble
  5	   Current Minutes High Nibble
  6	   Current Hours Low Nibble
  7	   Current Hours High Nibble and SMPTE Type
0nnn x yy d
where nnn is 7. x is unused and set to 0. d is bit 4 of the Hours Time. yy tells the SMPTE Type as follows:

0 = 24 fps
1 = 25 fps
2 = 30 fps ( Drop-Frame )
3 = 30 fps
*/
				INT32 sel = ( data>>12 )&0xf;
				INT32 d = ( data>>8 )&0xf;
				switch( sel )
				{
				case 0:	_tc_frame	=	( _tc_frame		& 0xf0 ) + d;					break;
				case 1:	_tc_frame	=	( _tc_frame		& 0x0f ) + ( d << 4 );			break;
				case 2:	_tc_second	=	( _tc_second	& 0xf0 ) + d;					break;
				case 3:	_tc_second	=	( _tc_second	& 0x0f ) + ( d << 4 );			break;
				case 4:	_tc_minute	=	( _tc_minute	& 0xf0 ) + d;					break;
				case 5:	_tc_minute	=	( _tc_minute	& 0x0f ) + ( d << 4 );			break;
				case 6:	_tc_hour	=	( _tc_hour		& 0xf0 ) + d;					break;
				case 7:	_tc_hour	=	( _tc_hour		& 0x0f ) + ( ( d & 0x1 ) << 4 );
						_tc_type = ( d >> 1 ) & 0x3;
						_b_tc_trig = true;
						break;
				}
//				if ( b_verbose_timecode_ )
//					sprintf( str_midi, "TIMECODE" );
				if( _b_tc_trig )
				{
					_b_tc_trig = false;
					tc_time_in = _tc_hour * REAL(3600) + _tc_minute * REAL(60) + REAL( _tc_second );
					switch( _tc_type )
					{
					case 0:	tc_time_in += REAL( _tc_frame )/REAL(24);	break;
					case 1:	tc_time_in += REAL( _tc_frame )/REAL(25);	break;
					case 2:	tc_time_in += REAL( _tc_frame )/REAL(30);	break;
					case 3:	tc_time_in += REAL( _tc_frame )/REAL(30);	break;
					default:	break;
					}
					if( b_verbose_timecode )
						snprintf( str_midi, STR_MIDI_LEN-1, "TIMECODE %2d:%2d:%2d:%2d", _tc_hour, _tc_minute, _tc_second, _tc_frame );
				}
			}
			break;
		}	//switch( status )
		if( c_midi::b_verbose && b_verbose_system_ui )
		{
			switch( channel_index )
			{
			case 0x1:
//					if ( b_verbose_timecode_ )
//						sprintf( str_midi, "TIMECODE" );				
				break;
			case 0x8:
				if ( b_verbose_clock )
					snprintf( str_midi, STR_MIDI_LEN-1, "SYSTEM CLOCK" );
				else
					*str_midi = 0;
				break;
			case 0xa:
				start( when );
				snprintf( str_midi, STR_MIDI_LEN-1, "SYSTEM START" );
				break;
			case 0xb:
				resume( when );
				snprintf( str_midi, STR_MIDI_LEN-1, "SYSTEM RESUME" );
				break;
			case 0xc:
				stop( when );
				snprintf( str_midi, STR_MIDI_LEN-1, "SYSTEM STOP" );
				break;
			default:
				snprintf( str_midi, STR_MIDI_LEN-1, "SYSTEM data %8x", data );
				break;
			}
			if ( *str_midi )
				print_verbose( str_midi, channel_index + 1, when );
		}
	}	// emd status = 0xf
	else if( _mackie->_s_midi_controller == midi::MACKIE )
	{
		switch( status )
		{
		case 0xe:	//fader
			{
				INT32	i_val;
				REAL value;
				i_val = ( ( data >> 8 ) & 0x7f ) | ( ( data >> 9 ) & 0x3f80 );
				if( i_val == 0 )
					value = 0.;
				else
					value = REAL( i_val )/REAL( 0x3ff0 );
				if( channel_index == 8 )	//this the ninth slider of the mackie (left to the slice of 8) 
					static_set_control( 16, 62, value, when );	//ch 16 ctrl 62 for now
				else	// this is the slider from 0 to 7
					static_set_control( _mackie->fader_get_channel( channel_index + 1 ), channel_index + _mackie->get_control_offset(), value, when );
			}
			break;
		case 0x9:	//button
			{
				INT32	but = ( data >> 8 ) & 0x7f;
				INT32	control;
				INT32	ch;
				REAL	value;
					
				if( but < 0x20 )	// 4 by 8 but on top of slider will end up ch 3-6 ctl 1-8
				{
					_mackie->do_but( ( but >> 3 ) + 1 + c_mackie::BUT_CHANNEL_OFFSET, but & 7, ( ( data >> 16 ) & 0x7f ) != 0, when );
				}
				else if( but < 0x28 )	// push vpot
				{
					_mackie->vpot_do_push( but & 7, (data >> 16) != 0, when );
				}
				else if( but < 0x68 )	//	control
				{
					ch = 16;
					control = ( but - 0x28 );
					// don't record this midi data
					if( data >> 16 )	//only push
					{
						bool b_used = false;
						if( _mackie->_b_master )
						{
							b_used = true;
							DBG_PRINT_STRING( "Mackie Master event control is %d", control );
							//bank
							switch( control )
							{
							case 6:		ask_bank_prev();		break;	// prev bank
							case 7:		ask_bank_next();		break;	// next bank
							case 9:		ask_bank_load();		break;	// load bank
							case 5:		switch_bank_direct();	break;	// switch bank direct
							case 40:	store_bank_a();			break;	// store A
							case 41:	store_bank_b();			break;	// store B
							case 42:	ask_bank_save_a();		break;	//	save bank_a
							case 43:	ask_bank_save_b();		break;	//	save bank_b
								
							case 11:	//todo check this and below
								freeze_bank_inter();
								static_set_control( 16, 62, 0, when );	//ch 16 ctrl 62 for now
//bad							mackie->send_fader( 8, 0 );
								break;
							case 44:	midi_trig_auto_patch();				break;
							case 45:	midi_trig_auto_depatch();			break;
								
							case 50:	set_bank( digit_all );				break;	//	marked Solo on Mackie
								
							case 46:	_b_dynamic_loop_trig = true;		break;	// looping on/off
							case 47:	_b_dynamic_clear_trig = true;		break;	// clear recording
							case 51:	_b_dynamic_restart_trig = true;		break;	// rewind sequencer
							case 54:	_b_dynamic_play_trig = true;		break;	// start seq playback
							case 53:
								// stop seq recording or playback
								if( b_dynamic_record )
								{
									// we need to know the stop time of the recording to calculate duration
									c_midi_universe::cur->_midi_bank[bank_cur].dynamic_record( -1, -1, 0, when );
								}
								_b_dynamic_stop_trig = true;
								break;
							case 55:
								// start seq recording
								if( b_dynamic_record )
								{
									// already recording, clear current data
									//c_midi_universe::cur->midi_bank[bank_cur_].dynamic_record_stop( when );
									c_midi_universe::cur->_midi_bank[bank_cur].dynamic_clear_data();
								}
								c_midi_universe::cur->_midi_bank[bank_cur].set_dynamic_record_start( when );
								_b_dynamic_record_trig = true;
								break;
							case 30:
							case 31:
							case 34:
							case 35:
							case 36:
							case 32:
							case 33:
							case 37:
							case 38:
							case 39:
								switch( control )
								{
								case 30:	digit_ui = 1;	break;
								case 31:	digit_ui = 2;	break;
								case 34:	digit_ui = 3;	break;
								case 35:	digit_ui = 4;	break;
								case 36:	digit_ui = 5;	break;
								case 32:	digit_ui = 6;	break;
								case 33:	digit_ui = 7;	break;
								case 37:	digit_ui = 8;	break;
								case 38:	digit_ui = 9;	break;
								case 39:	digit_ui = 0;	break;
								}
								if( b_verbose_event )
									DBG_PRINT_STRING( "%s%s %d", str_midi, "MACKIE DIGIT", digit_ui );
								break;
							default:
								b_used = false;
								DBG_PRINT_STRING( "%sUnused Mackie control %d (master)", str_midi, control );
								break;
							}
						}

						if( INSIDE_MIN_MAX( control, 14, 29 ) )
						{	//	direct switch to slice, two rows of 8 buts
							//todoqqq shift button -> slice 16-32
							_mackie->ask_slice( control - 14 );
						}
						else
						{
							switch( control )
							{
							case 58:	_mackie->ask_slice_prev();			break;	// prev slice
							case 59:	_mackie->ask_slice_next();			break;	// next slice
							case 57:	_mackie->ask_slice_next_row();		break;					
							case 56:	_mackie->ask_slice_prev_row();		break;	
								//mackie->control_offset_ui = I_MOD( mackie->control_offset_ui - MIDI_CONTROL_NB/2, MIDI_CONTROL_NB );
							case 34:	_mackie->set_slice_offset( 0 );		break;
							case 35:	_mackie->set_slice_offset( 128 );	break;
							case 36:	_mackie->set_slice_offset( 256 );	break;
							case 10:	_mackie->flip_fader_shift();		break;
							case 0:		_mackie->flip_vpot_shift();			break;
							default:
								if( !b_used )
									DBG_PRINT_STRING( "%sUnused Mackie control %d", str_midi, control );
							}
						}
					}

					value = ( ( data >> 16 ) & 0x7f ) ? REAL(1) : REAL(0);
					static_set_control( ch, control, value, when );
					//mackie->send_but( ch, control, value );
				}
				else if ( but <= 0x70 ) 	// touch fader
				{
					control = but - 0x68;
					_mackie->fader_ask_contact_change( control, ( data >> 16 ) != 0 );
				}
			}
			break;
		case 0xb:	//v-pot and  juggle
			{
				if( channel_index != 0 )
					break;
				INT32	control;
				INT32	inc;
				if( ( ( data >> 8 ) & 0x7f ) == 0x3c )
				{
					control = 8;
					inc = 1;
				}
				else
				{
					control = ( data >> 8 )  & 0x7;
					inc		= ( data >> 16 ) & 0xf;
				}
				if( data & ( 1 << ( 16 + 6 ) ) )
					inc = -inc;
				if( control == 8 )	//jog
				{
					REAL	value;
					value = bank_cur_ui + REAL( inc );	///128. ;
					value = FMOD( value, REAL(c_midi_universe::MIDI_BANK_NB) );
					bank_cur_ui = c_midi_universe::make_valid_bank_nb( INT32(value) );
					static_set_control( 16, 63, value, when );
				}
				else
				{
					_mackie->vpot_do_inc( control, inc, when );
				}
			}
			break;
		}
	}	// we finish to treat mackie specific midi data 
	//else if( mackie_->s_midi_controller_ == MIDI_CS32 )
	//{

	//}
	//else means not MACKIE
	else if ( _b_channel_listen_all || _b_channel_listen[ channel_index ] )
	{
		INT32	channel_id = channel_reroute( channel_index+1 );
		if( ( data != 0xf8 ) && ( data != 0xfe ) )
		{
			switch( status )
			{
			case 0x8:	// note off
				static_set_noteoff( channel_id, ( ( data & 0x7f00 ) >> 8 ), when );
				break;
			case 0x9:	// note on
				static_set_noteon( channel_id, ( ( data & 0x7f00 ) >> 8 ), REAL(((data >> 16) & 0x7f) / 127.), when );
				break;
			case 0xa:	// aftertouch
				if( c_midi::b_verbose && b_verbose_aftertouch_poly )
				{
					snprintf( str_midi, STR_MIDI_LEN-1, "AFTERTOUCH POLY: data %8x", data );
					print_verbose( str_midi, channel_id, when );
				}
				break;
			case 0xb:	// control
				if( _b_filter && _b_filter_control_change )
					break;

				{
					INT32 ctl_index = ( data & 0x7f00 ) >> 8;
					INT32 b3 = data >> 16;
					if( _mackie->_s_midi_controller == midi::BCF2000 && channel_id == 5 )
					{
						//todo we have to edit because ctl are 1-8 and we just get push
						if( INSIDE_MIN_MAX( ctl_index, 1, 8 ) ) 
							_mackie->vpot_do_push( ctl_index, true, when );
						break;
					}
					else if( _b_bcf_14bits && _mackie->_s_midi_controller == midi::BCF2000 && channel_id == 1 )
					{
						if( INSIDE_MIN_MAX( ctl_index, 0x20, 0x27 ) )		// CONTROL 20-27
						{
							ctl_index = ctl_index - 32 + _mackie->get_control_offset();
							static_set_control( channel_id, ctl_index, (_msb_stored | b3 ) / REAL( (1<<14) - 1 ), when );
							break;
						}
						else if( INSIDE_MIN_MAX( ctl_index, 0x0, 0x7 ) )	// CONTROL 0-7
						{
							_msb_stored = b3 << 7;
							break;
						}
						else if( INSIDE_MIN_MAX( ctl_index, 0x40, 0x47 ) )	// CONTROL 64-71
						{
							INT32 inc = b3 == 1 ? -8 : 8;
							_mackie->vpot_do_inc( ctl_index-0x40, inc, when );
							break;
						}
						else if( ctl_index == 0xf  )	// CONTROL 15
						{
							_ctl_stored = ctl_index;
							_b3_stored = b3;
							break;
						}
						else if( ctl_index == 0x2f )	// CONTROL 47
						{
							if( INSIDE_MIN_MAX( _b3_stored, 0, 7 ) )
							{
								switch( b3 )
								{
								case 0:
								case 0x40:	_mackie->fader_ask_contact_change( _b3_stored, b3 != 0 ); break;
								case 7:
								case 0x47:	_mackie->vpot_do_push( _b3_stored, b3 != 7, when ); break;
								case 2:
								case 0x42:	_mackie->do_but( 2 + c_mackie::BUT_CHANNEL_OFFSET, _b3_stored, b3 != 2, when ); break;
								case 3:
								case 0x43:	_mackie->do_but( 1 + c_mackie::BUT_CHANNEL_OFFSET, _b3_stored, b3 != 3, when ); break;
								}
							}
							else if( _b3_stored  == 0xf )
							{
								switch( b3 )
								{
								// preset
								case 0x1:	break;	//release but
								case 0x41:	break;
								case 0x3:	break;	//release but
								case 0x43:	break;
								}
							}
							else if( _b3_stored  == 0xe )
							{
								switch( b3 )
								{
								// slice bottom right
								case 0x03:	break;	//release but
								case 0x43:	_mackie->ask_slice_prev();	break;
								case 0x04:	break;	//release but
								case 0x44:	_mackie->ask_slice_next();	break;
								// second row frm bottom, top f slice
								case 0x01:	break;	//release but
								case 0x41:	break;
								case 0x02:	break;	//release but
								case 0x42:	break;
								}
							}
							break;
						}
					}
					if( _b_nrpn && (_mackie->_s_midi_controller != midi::BCF2000 || channel_index != 5) )
					{
						//sometimes we get 2 0x63 one after another
						if( ctl_index == 0x63 )
						{
							_s_nrpn = 1;
							_nrpn_ctl_index = b3 << 7;
							return;
						}
						switch( _s_nrpn )
						{
						case 1:
							if( ctl_index == 0x62 )
							{
								_s_nrpn = 2;
								_nrpn_ctl_index += b3;									
							}
							else
								_s_nrpn = 0;
							return;
						case 2:
							if( ctl_index == 0x6 )
							{
								_s_nrpn = 3;
								_nrpn_value = b3 << 7;									
							}
							else
								_s_nrpn = 0;
							return;
						case 3:
							_s_nrpn = 0;
							if( ctl_index == 0x26 )
							{
								--_nrpn_ctl_index;
								_nrpn_value += b3;
								if( channel_id == 2 )
								{
									REAL inc = REAL( _nrpn_value & 0x1fff );					
									if( _nrpn_value & 0x2000 )	//relative 3
										inc = -inc;
									//if( mackie->vpot_pushed[control] )
									//	inc /= 8;
									INT32 ch_id = _mackie->vpot_get_channel( _nrpn_ctl_index );
									INT32 tmp_ctl_index = _nrpn_ctl_index + _mackie->get_control_offset();
									REAL value = static_get_control( ch_id, tmp_ctl_index ) + REAL( inc ) / REAL(128);
									static_set_control( ch_id, tmp_ctl_index, value, when );
									_mackie->_b_vpot_need_to_send[_nrpn_ctl_index] = true; 
								}
								else
								{
									INT32 tmp =  _mackie->fader_get_channel( _nrpn_ctl_index );
									static_set_control( tmp, _nrpn_ctl_index + _mackie->get_control_offset(),
														( REAL( _nrpn_value )*_control_factor[channel_id-1] ) / REAL( ( 128 * 128 ) - 1 ),
														when );
								}
							}
							return;
						}
					}
					if( _mackie->_s_midi_controller == midi::BCF2000 && channel_id == 16 )
					{
						//if( b3 == 127 )
						switch( ctl_index )
						{
							case 1:		_mackie->flip_vpot_shift();		break;
							case 2:		_mackie->flip_fader_shift();	break;
							case 3:		_mackie->flip_but_shift();		break;
							case 9:		ask_bank_prev();				break;	//	prev bank
							case 10:	ask_bank_next();				break;	//	next bank
							case 13:	_mackie->ask_slice_prev();		break;
							case 14:	_mackie->ask_slice_next();		break;
						}
						static_set_control( channel_id, ctl_index, ( REAL( b3 ) ) / REAL(127), when );
					}
					else if( _mackie->_s_midi_controller == midi::BCF2000 && 3 <= channel_id && channel_id <= 4 )
						_mackie->do_but( channel_id, ctl_index-1, b3 == 127, when );
					else if( _mackie->_s_midi_controller == midi::BCF2000 && 5 == channel_id )
						_mackie->do_but( 17, ctl_index-1, b3 == 127, when );
					else
					{
						INT32 ch = channel_index + 1;	//	we start at 1 (midi style)
						if( _b_control_patch_ui )
							midi_control_patcher_in.patch( ch, ctl_index );
						static_set_control( channel_reroute( ch ), ctl_index+_control_index_offset[ch-1],
												( REAL( b3 ) * _control_factor[ch-1] ) / REAL(127), when );
					}
				}
				break;
			case 0xc:
				if( !_b_filter || !_b_filter_program_change )
					store_program_change( channel_id, ( (data & 0xff00)>>8 ), when );
				break;
			case 0xd:
				if( c_midi::b_verbose && b_verbose_aftertouch_channel )
				{
					snprintf( str_midi, STR_MIDI_LEN-1, "AFTERTOUCH CHANNEL : data %8x", data );
					print_verbose( str_midi, channel_id, when );
				}
				break;
			case 0xe:
				if( c_midi::b_verbose && b_verbose_pitchbend )
				{
					snprintf( str_midi, STR_MIDI_LEN-1, "PITCH BEND : data %8x", data );
					print_verbose( str_midi, channel_id, when );
				}
				break;
			default:
				if( c_midi::b_verbose )
				{
					snprintf( str_midi, STR_MIDI_LEN-1, "STATUS %1x : data %8x", status, data ); 
					print_verbose( str_midi, channel_id, when );
				}
				break;
			}
		}
	}
}

void c_midi::in_callback( UINT wMsg, DWORD_PTR p1, DWORD_PTR p2 )
{
	_date_cur = INT32(p2);
	switch( wMsg )
	{
	case MIM_OPEN:		if( c_midi::b_verbose )
							PRINT_STRING( "IN open Message" );								
						break;
	case MIM_CLOSE:		if( c_midi::b_verbose )
							PRINT_STRING( "IN close Message" );
						break;
	case MIM_DATA:		++_in_message_nb;
						treat_in( INT32(p1), INT32(p2) );
						//spy::sleep(100);
						break;
	case MIM_LONGDATA:	++_in_message_nb;
						if( c_midi::b_verbose )
							PRINT_STRING( "IN long data Message" );
						break;
	case MIM_ERROR:		if( c_midi::b_verbose )
							PRINT_STRING( "IN error Message" );
						break;
	case MIM_LONGERROR:	if( c_midi::b_verbose )
							PRINT_STRING( "IN long error Message" );
						break;
	case MIM_MOREDATA:	++_in_message_nb;
						if( c_midi::b_verbose )
							PRINT_STRING( "IN more data Message" );
						break;
	default:			if( c_midi::b_verbose )
							PRINT_STRING( "IN mess %d : unknown callback Message", wMsg );
						break;
	}
}
 
void c_midi::out_callback( UINT wMsg, DWORD_PTR p1, DWORD_PTR p2 )
{
	if( b_verbose_out_callback )
	{
		switch( wMsg )
		{
		case MOM_OPEN:	PRINT_STRING( "OUT open Message" );									break;
		case MOM_CLOSE:	PRINT_STRING( "OUT close Message" );								break;
		case MOM_DONE:	PRINT_STRING( "OUT done Message" );									break;
		default:		ERR_PRINT_STRING( "OUT mess %d : unknown callback Message", wMsg );	break;
		}
	}
}

void CALLBACK midi_in_callback(	HMIDIIN hMidiIn,
								UINT wMsg,
								DWORD_PTR dwInstance,
								DWORD_PTR p1,
								DWORD_PTR p2 )
{
#if AAA_DEBUG()
	if( midi_in_count )
		WARNING_PRINT_STRING( "overlapping midi_in_callback()" );
	++midi_in_count;
#endif
	{
		std::lock_guard<aaa::MUTEX> guard(midi_in_lock);
		c_midi*	p_midi = ( c_midi* )dwInstance;
		if( p_midi )
			p_midi->in_callback( wMsg, p1, p2 );
	}
#if AAA_DEBUG()
	--midi_in_count;
#endif
}

void CALLBACK midi_out_callback( HMIDIOUT hMidiOut,
								UINT wMsg,
								DWORD_PTR dwInstance,
								DWORD_PTR p1,
								DWORD_PTR p2 )
{
	c_midi*	p_midi = ( c_midi* )dwInstance;
	if( p_midi )
		p_midi->out_callback( wMsg, p1, p2 );
}

void	c_midi::save_global( C_PCHAR_C filename_in )
{
	C_PCHAR_C path = filename_in ? filename_in : midi_global_name.get();

	SPY_PUSH_RANGE( "midi::save_global", spy::FILE );
	o_str& filename = o_str::push_name( path );

		filename.add_ext( midi_bank_ext );

		FILE* file = file_csv::open( filename, true );
			c_midi_universe::cur->write_file_control( file ); 
		file_csv::close( file );

		filename.drop_ext();
		//hack remove _a 
		filename.drop_at( -2 );
		c_mackie::save_slices( filename );

	o_str::pop_name();
	SPY_POP_RANGE();
}

void	c_midi::load_global( C_PCHAR_C filename_in )
{
	midi_global_name.set( filename_in );

	SPY_PUSH_RANGE( "midi::load_global", spy::FILE_HIGH );
	o_str& filename	= o_str::push_name( filename_in );
	o_str& dir		= o_str::push_name();

		dir.set_dir_name( filename );
		bool b_load_record_file = c_dir_parser::have_file( dir.get(), "record_data.*.csv" );

		filename.add_ext( midi_bank_ext );
		if( c_file::is_exist( filename ) )
		{
			FILE*	file = file_csv::open( filename, false ); 
				c_midi_universe::cur->read_file_control( file, b_load_record_file );
			file_csv::close( file );
		}

		filename.drop_ext();
		//hack remove _a 
		filename.drop_at( -2 );
		c_mackie::load_slices( filename );

	o_str::pop_name();
	o_str::pop_name();
	SPY_POP_RANGE();
}

namespace {
	CONST CHAR  data_ext[] = "data";
};

AAA_ERR	c_midi::save_do_after( o_str CONST & filename_in )
{
	INT32	nb_write = 0;
	stop( 0 );
	if( _play_buf && _play_nb )
	{
		o_str& filename = o_str::push_name( filename_in );
			filename.add_ext( data_ext );
			FILE* file = file_csv::open( filename, true );
				nb_write = midi_file_write( file, _play_buf, _play_nb, "Date", "Hex" );
			file_csv::close( file );
		o_str::pop_name();
	}
	return nb_write == _play_nb ? AAA_OK : ERR_ANY;
}

AAA_ERR	c_midi::load_do_after( o_str CONST & filename_in )
{
	stop( 0 );
	_play_nb = 0;

	o_str& filename = o_str::push_name( filename_in );
		filename.add_ext( data_ext );
		if( c_file::is_exist( filename ) )
		{
			alloc_play_record_buf();
			FILE* file = file_csv::open( filename, false );
				_play_nb = midi_file_read( file, _play_buf, "Date", "Hex" );
			file_csv::close( file );
		}
	o_str::pop_name();
	return _play_nb ? AAA_OK : ERR_ANY; 	//hack ???
}

void	c_midi::channel_listen_init()
{
	bool*	pt = _b_channel_listen;
	_b_channel_listen_all = false;
	for( INT32 i = CHANNEL_NB; i > 0; --i )
		*pt++ = true;
}


//todo add lock on certain param

namespace n_midi
{
	enum	BANK_RECORD_STATUS : INT32
	{
		BANK_STOP = 0,
		BANK_PLAYING,
		BANK_PAUSING,
		BANK_LOOPING,
		BANK_RECORDING,
		BANK_PLAYING_RECORDING,
		BANK_UNKNOWN,
		BANK_STATUS_MAX_NB,
	};

	C_PCHAR_C	bank_record_status_str[BANK_STATUS_MAX_NB] = 
	{
		"Stopped",
		"Playing",
		"Pausing",
		"Playing and looping",
		"Recording",
		"Playing and recording",
		"Unknown"
	};

	C_PCHAR_C	str_midi_channel_dst[] =
	{
		"Nominal",
	};
	
	C_PCHAR_C	str_midi_controller[midi::CONTROLLER_TYPE_NB] =
	{
		"CS32_by_JLCooper",
		"Regular",
		"MCU_By_Mackie",
		"BCF2000_by_Behringer",
	};

	CONSTEXPR INT32 BASE_PARAM_NB						= 8;
	CONSTEXPR INT32 MIDI_TEK_PARAM_NB_MAX				= 2;
	CONSTEXPR INT32 MIDI_ACTIVE_IN_NB_MAX				= 8;
	CONSTEXPR INT32 MIDI_ACTIVE_OUT_NB_MAX				= MIDI_ACTIVE_IN_NB_MAX;
	CONSTEXPR INT32 MIDI_BANK_PARAM_NB_MAX				= 10;
	CONSTEXPR INT32 MIDI_BANK_RECORD_PARAM_NB_MAX		= 9;
	CONSTEXPR INT32 MIDI_MACKIE_PARAM_NB_MAX			= 12;
	CONSTEXPR INT32 MIDI_MACKIE_FADER_PARAM_NB_MAX		= ( 2 + c_mackie::FADER_NB );
	CONSTEXPR INT32 MIDI_MACKIE_SLICE_PARAM_NB_MAX = c_mackie::SLICE_NB + 4 + 1;	// + 1 for group Slices ALL

	CONSTEXPR INT32 MIDI_SEQUENCER_PARAM_NB_MAX			= 9;
	CONSTEXPR INT32 MIDI_FILTER_PARAM_NB_MAX			= 4;
	CONSTEXPR INT32 MIDI_ALLOW_PARAM_NB_MAX				= 2;
	CONSTEXPR INT32 MIDI_CHANNEL_PARAM_NB_MAX			= c_midi::CHANNEL_NB + 1;
	CONSTEXPR INT32 MIDI_CHANNEL_CHANGE_PARAM_NB_MAX	= c_midi::CHANNEL_NB * 3;
	CONSTEXPR INT32 MIDI_OUT_PARAM_NB_MAX				= c_midi::OUT_BANK_CTL_NB + 2;
	CONSTEXPR INT32 MIDI_VERBOSE_PARAM_NB_MAX			= 23;
	CONSTEXPR INT32 GROUP_NB							= 14 + c_midi::OUT_BANK_NB;

	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	MIDI_TEK_PARAM_NB_MAX
									+	MIDI_ACTIVE_IN_NB_MAX
									+	MIDI_ACTIVE_OUT_NB_MAX
									+	MIDI_BANK_PARAM_NB_MAX
									+	MIDI_BANK_RECORD_PARAM_NB_MAX
									+	MIDI_MACKIE_PARAM_NB_MAX
									+	MIDI_MACKIE_FADER_PARAM_NB_MAX
									+	MIDI_MACKIE_SLICE_PARAM_NB_MAX
									+	MIDI_SEQUENCER_PARAM_NB_MAX
									+	MIDI_FILTER_PARAM_NB_MAX
									+	MIDI_ALLOW_PARAM_NB_MAX
									+	MIDI_CHANNEL_PARAM_NB_MAX
									+	MIDI_CHANNEL_CHANGE_PARAM_NB_MAX
									+	MIDI_OUT_PARAM_NB_MAX * c_midi::OUT_BANK_NB
									+	MIDI_VERBOSE_PARAM_NB_MAX
									+	GROUP_NB;

	//{	nullptr,	PARAM_SYMBO_ZERO,	( "channel_"#name##"_channel_dst" ),	1, 0,	0, MIDI_CHANNEL_NB,		nullptr, str_midi_channel_dst },
#define	PARAM_DEF_CHANNEL( name )\
	PARAM_DEF_SYMBO_ZERO(	name##_channel_dst,		1, 0,		0, c_midi::CHANNEL_NB, str_midi_channel_dst )\
	PARAM_DEF_INT32_ZERO(	name##_control_index_offset )\
	PARAM_DEF_REAL_ONE(		name##_control_factor )
	//{	nullptr,	PARAM_INT32,		"o"###nb##"_channel_ui",	2, 1,	1, c_midi::CHANNEL_NB,								nullptr, nullptr },\
	//{	nullptr,	PARAM_INT32,		"o"###nb##"_control_ui",	2, 1,	1, c_midi::CONTROL_NB-c_midi::OUT_BANK_CTL_NB+1,	nullptr, nullptr },\

#define	PARAM_DEF_OUT( nb )\
	PARAM_DEF_GROUP_CLOSED( Out_##nb, MIDI_OUT_PARAM_NB_MAX )\
		PARAM_DEF_INT32(	o##nb##_channel_ui,			2,1,		1, c_midi::CHANNEL_NB )\
		PARAM_DEF_INT32(	o##nb##_control_ui,			1,0,		0, c_midi::CONTROL_NB - c_midi::OUT_BANK_CTL_NB - 1 )\
		PARAM_DEF_00_15(	o##nb##_value, PARAM_DEF_REAL_ZERO_SAVE_NOT )

	CONST c_param_def param[PARAM_NB_MAX] =
	{
	PARAM_DEF_BOOL_OFF(		active )

	PARAM_DEF_GROUP(		IN_state,			MIDI_ACTIVE_IN_NB_MAX )
		PARAM_DEF_BOOL_OFF(			in_enum_trig			)
		PARAM_DEF_BOOL_OFF(			in_active				)
		PARAM_DEF_SYMBO_NEG(		in_device,				0,-1,		-1,PARAM_MAX_INT32,	gstr::no )
		PARAM_DEF_BOOL_OFF(			in_open					)
		PARAM_DEF_BOOL_LOCKED(		in_opened				)
		PARAM_DEF_INT32_LOCKED(		in_opened_device_index	)
		PARAM_DEF_STR_LOCKED(		in_opened_device_name	)
		PARAM_DEF_INT32_ZERO_ONE(	in_message_nb			)

	PARAM_DEF_GROUP(		OUT_state,			MIDI_ACTIVE_OUT_NB_MAX )
		PARAM_DEF_BOOL_OFF(			out_enum_trig			)
		PARAM_DEF_BOOL_OFF(			out_active				)
		PARAM_DEF_SYMBO_NEG(		out_device,				0,-1,		-1,PARAM_MAX_INT32,	gstr::no )
		PARAM_DEF_BOOL_OFF(			out_open				)
		PARAM_DEF_BOOL_LOCKED(		out_opened				)
		PARAM_DEF_INT32_LOCKED(		out_opened_device_index	)
		PARAM_DEF_STR_LOCKED(		out_opened_device_name	)
		PARAM_DEF_INT32_ZERO_ONE(	out_message_nb			)

	PARAM_DEF_GROUP_CLOSED(	TEK,				MIDI_TEK_PARAM_NB_MAX )
		PARAM_DEF_BOOL_OFF(		skip_close_on_exit		)
		PARAM_DEF_BOOL_ON(		close_do_unprepare		)

	PARAM_DEF_GROUP_CLOSED(	Bank,				MIDI_BANK_PARAM_NB_MAX + MIDI_BANK_RECORD_PARAM_NB_MAX + 1 )
		PARAM_DEF_INT32(		bank_cur,				1,0,		0,c_midi_universe::MIDI_BANK_NB-1 )
		PARAM_DEF_STR_SAVE_NOT(	bank_name				)
		PARAM_DEF_BOOL_OFF(		bank_direct				)
		PARAM_DEF_INT32(		bank_direct_begin,		1,0,		0,c_midi_data::CONTROL_NB_IN_BANK-1 )
		PARAM_DEF_INT32(		bank_direct_end,		63,127,		0,c_midi_data::CONTROL_NB_IN_BANK-1 )
		PARAM_DEF_BOOL_OFF(		bank_load_trig			)
		PARAM_DEF_BOOL_OFF(		bank_store_a_trig		)
		PARAM_DEF_BOOL_OFF(		bank_store_b_trig		)
		PARAM_DEF_BOOL_OFF(		bank_a_save_trig		)
		PARAM_DEF_BOOL_OFF(		bank_b_save_trig		)

		PARAM_DEF_GROUP_CLOSED(	recording,		MIDI_BANK_RECORD_PARAM_NB_MAX )
			PARAM_DEF_INT32_SAVE_NOT(	number,			1,0,		0,PARAM_MAX_INT32 )
			PARAM_DEF_REAL_POS_ZERO(	duration		)
			PARAM_DEF_SYMBO_LOCKED(		status,			1,0,		1,bank_record_status_str )
			PARAM_DEF_REAL_POS_ZERO(	time			)
			PARAM_DEF_BOOL_OFF(			rec_trigger		)
			PARAM_DEF_BOOL_OFF(			play_trigger	)
			PARAM_DEF_BOOL_OFF(			stop_trigger	)
			PARAM_DEF_BOOL_OFF(			clear_trigger	)
			PARAM_DEF_BOOL_OFF(			loop			)

	PARAM_DEF_SYMBO_PSTR_ONE(	Midi_controller,	str_midi_controller )

	PARAM_DEF_GROUP_CLOSED(	Allow,				MIDI_ALLOW_PARAM_NB_MAX )
		PARAM_DEF_BOOL_OFF(		allow_Non_Registered_Parameter_Number )
		PARAM_DEF_BOOL_OFF(		allow_BCF_14_bits )

	PARAM_DEF_GROUP_CLOSED(	Mackie Control Universal,	MIDI_MACKIE_PARAM_NB_MAX )
		PARAM_DEF_BOOL_OFF(			mackie_master					)
		PARAM_DEF_BOOL_ON(			mackie_bank_interpolate_allow	)
		PARAM_DEF_STR(				mackie_mess						)
		PARAM_DEF_INT32_LOCKED(		control_offset					)	//,1,0,	0,c_midi::CONTROL_NB - 8 - 1 )
		PARAM_DEF_BOOL_OFF(			fader_shift						)
		PARAM_DEF_BOOL_OFF(			vpot_shift						)
		PARAM_DEF_BOOL_OFF(			but_shift						)
		PARAM_DEF_a_d(				but_global, PARAM_DEF_BOOL_OFF	)
		PARAM_DEF_BOOL_OFF(			verbose							)

	PARAM_DEF_GROUP_CLOSED(	Mackie fader,		MIDI_MACKIE_FADER_PARAM_NB_MAX )
		PARAM_DEF_REAL(			fader_sensitivity,					0,1,	0,1 )
		PARAM_DEF_DOUBLE(		fader_change_interval,				1,.2,	0,1 )
		PARAM_DEF_9(			fader_contact,	PARAM_DEF_BOOL_OFF )

	PARAM_DEF_GROUP_CLOSED(	Slices,				MIDI_MACKIE_SLICE_PARAM_NB_MAX )
		PARAM_DEF_BOOL_OFF(		slice_prev_trig			)
		PARAM_DEF_BOOL_OFF(		slice_next_trig			)
		PARAM_DEF_INT32_LOCKED(	slice_index				)
		PARAM_DEF_NONE(			slice_current			)
		PARAM_DEF_GROUP_CLOSED(	Slices ALL,	c_mackie::SLICE_NB	)
			PARAM_DEF_0_63(			Slice,	PARAM_DEF_NONE	)

	PARAM_DEF_GROUP_CLOSED(	Sequencer,			MIDI_SEQUENCER_PARAM_NB_MAX )
		PARAM_DEF_INT32(		seq_cur,		1, 0,	0, c_midi::MIDI_SEQ_NB - 1 )
		PARAM_DEF_BOOL_OFF(		trigger_start	)
		PARAM_DEF_BOOL_OFF(		trigger_stop	)
		PARAM_DEF_BOOL_OFF(		trigger_save	)
		PARAM_DEF_BOOL_OFF(		trigger_load	)
		PARAM_DEF_BOOL_OFF(		record_allow	)
		PARAM_DEF_BOOL_OFF(		record_on		)
		PARAM_DEF_BOOL_OFF(		play_allow		)
		PARAM_DEF_BOOL_OFF(		play_on			)

	PARAM_DEF_GROUP_CLOSED(	Filter,				MIDI_FILTER_PARAM_NB_MAX )
		PARAM_DEF_BOOL_OFF(		filter_active					)
		PARAM_DEF_BOOL_ON(		filter_program_change			)
		PARAM_DEF_BOOL_ON(		filter_control_change			)
		PARAM_DEF_BOOL_OFF(		route_note_as_control_change	)

	PARAM_DEF_GROUP_CLOSED(	Channel_in,			MIDI_CHANNEL_PARAM_NB_MAX )
		PARAM_DEF_BOOL_OFF(		channel_all		)
		PARAM_DEF_01_16(		channel,		PARAM_DEF_BOOL_ON	)

//	PARAM_DEF_BOOL_OFF( send_to_lua_control )
//	PARAM_DEF_BOOL_OFF( send_to_lua_program )
	PARAM_DEF_BOOL_OFF(		patch_control_csv )
	PARAM_DEF_GROUP_CLOSED(	Channel Change,		MIDI_CHANNEL_CHANGE_PARAM_NB_MAX )
		PARAM_DEF_01_16( channel, PARAM_DEF_CHANNEL )

	PARAM_DEF_REAL_ZERO(	timecode_offset )
	PARAM_DEF_REAL_LOCKED(	timecode		)
	PARAM_DEF_INT32_LOCKED(	date			)
	PARAM_DEF_INT32_LOCKED(	control_change_last_channel	)
	PARAM_DEF_INT32_LOCKED(	control_change_last_control	)

	PARAM_DEF_OUT(			1 )
	PARAM_DEF_OUT(			2 )
	PARAM_DEF_OUT(			3 )
	PARAM_DEF_OUT(			4 )
	PARAM_DEF_OUT(			5 )
	PARAM_DEF_OUT(			6 )
	PARAM_DEF_OUT(			7 )
	PARAM_DEF_OUT(			8 )

	PARAM_DEF_GROUP_CLOSED(	Verbose_stuff,		MIDI_VERBOSE_PARAM_NB_MAX )
		PARAM_DEF_STR(			verbose_name				)
		PARAM_DEF_BOOL_OFF(		verbose_low_in				)
		PARAM_DEF_BOOL_OFF(		verbose_low_out				)
		PARAM_DEF_BOOL_OFF(		verbose_master				)
		PARAM_DEF_BOOL_OFF(		verbose_display_date		)
		PARAM_DEF_BOOL_OFF(		verbose_display_time		)
					 
		PARAM_DEF_BOOL_OFF(		verbose_display_count		)
		PARAM_DEF_BOOL_ON(		verbose_display_channel		)
		PARAM_DEF_BOOL_OFF(		verbose_velocity_master		)
		PARAM_DEF_BOOL_OFF(		verbose_noteon				)
		PARAM_DEF_BOOL_OFF(		verbose_velocity			)
					 
		PARAM_DEF_BOOL_OFF(		verbose_noteoff				)
		PARAM_DEF_BOOL_OFF(		verbose_program_change		)
		PARAM_DEF_BOOL_OFF(		verbose_control_change		)
		PARAM_DEF_BOOL_OFF(		verbose_aftertouch_poly		)
		PARAM_DEF_BOOL_OFF(		verbose_aftertouch_channel	)
					 
		PARAM_DEF_BOOL_OFF(		verbose_pitchbend			)
		PARAM_DEF_BOOL_OFF(		verbose_system				)
		PARAM_DEF_BOOL_OFF(		verbose_midi_timecode		)
		PARAM_DEF_BOOL_OFF(		verbose_midi_clock			)
		PARAM_DEF_BOOL_OFF(		verbose_out_callback		)
					 
		PARAM_DEF_BOOL_OFF(		verbose_read_bank			)
		PARAM_DEF_BOOL_OFF(		verbose_event				)
	};

	PARAM_DEF_MAKE_INDEX( IN_state		);
	PARAM_DEF_MAKE_INDEX( OUT_state		);
	PARAM_DEF_MAKE_INDEX( Bank			);
	PARAM_DEF_MAKE_INDEX( Slices		);
	PARAM_DEF_MAKE_INDEX( slice_current );
};


void	c_midi::prepare_for_ui()
{
	o_str& o = o_str::push_name();
		o.set( _in_index_asked_ui );
		o.add_space();
		o.add( _in_device_name_out );
		if( !_b_active_in_ui )
			o.add( " Inactive" );
		if( _b_open_in )
			o.add( " Open" );	
		get_param(n_midi::PARAM_INDEX_IN_state)->set_comment( o, !_b_active_in_ui || !_b_open_in );

		o.set( _out_index_asked_ui );
		o.add_space();
		o.add( _out_device_name_out );
		if( !_b_active_out_ui )
			o.add( " Inactive" );
		if( _b_open_out )
			o.add( " Open" );
		get_param(n_midi::PARAM_INDEX_OUT_state)->set_comment( o, !_b_active_out_ui || !_b_open_out );

		o.set( " is " );
		o.add(  bank_cur_ui	);
		get_param(n_midi::PARAM_INDEX_Bank)->set_comment( o );

		o.set( " is " );
		o.add( _mackie->_slice_cur_index );
		get_param(n_midi::PARAM_INDEX_Slices)->set_comment( o );
	o_str::pop_name();

	param_attach_obj_no_inc( n_midi::PARAM_INDEX_slice_current, c_mackie::get_slice(_mackie->_slice_cur_index) );
}

void	c_midi::param_update_pt()
{
	param_set_pt_no_inc( _param_index_bank_name, c_midi_universe::cur->_midi_bank[bank_cur_ui].get_data_name() );

	for( INT32 i = 0; i < OUT_BANK_NB; ++i )
	{
		INT32 h = _param_index_ctl[i];
		REAL* pt = c_midi_universe::cur->_midi_data.get_control_pt( _channel_ui[i], _control_ui[i] );
		for( INT32 nb = OUT_BANK_CTL_NB; nb > 0; --nb )
			param_set_pt( h, pt++ );
	}
}

void	c_midi::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, get_pt_active() );
	++h;
		param_set_pt(		h, _b_enum_in_trig_ui		);
		param_set_pt(		h, _b_active_in_ui			);
		param_set_unused_2(	h, !_b_active_in_ui	);
		param_set_pt(		h, _in_index_asked_ui		);
		param_set_unused(	h, !_b_active_in_ui );
		param_set_pt(		h, _b_open_in_asked_ui		);
		param_set_pt(		h, _b_open_in				);
		param_set_pt(		h, _in_index_opened_out		);
		param_set_pt(		h, _in_device_name_out		);
		param_set_pt(		h, _in_message_nb			);

	++h;
		param_set_pt(		h, _b_enum_out_trig_ui		);
		param_set_pt(		h, _b_active_out_ui			);
		param_set_unused_2( h, !_b_active_out_ui );
		param_set_pt(		h, _out_index_asked_ui		);
		param_set_unused(	h, !_b_active_out_ui );
		param_set_pt(		h, _b_open_out_asked_ui		);
		param_set_pt(		h, _b_open_out				);
		param_set_pt(		h, _out_index_opened_out	);
		param_set_pt(		h, _out_device_name_out		);
		param_set_pt(		h, _out_message_nb			);

	++h;
		param_set_pt( h, _b_skip_close_on_exit	);
		param_set_pt( h, _b_close_do_unprepare	);

	++h;
		param_set_pt( h, bank_cur_ui			);
		_param_index_bank_name = h++;
		param_set_pt( h, b_bank_direct			);
		param_set_pt( h, bank_direct_begin		);
		param_set_pt( h, bank_direct_end		);

		param_set_pt( h, b_bank_load_trig		);
		param_set_pt( h, b_bank_a_store_trig	);
		param_set_pt( h, b_bank_b_store_trig	);
		param_set_pt( h, b_bank_a_save_trig		);
		param_set_pt( h, b_bank_b_save_trig		);

		++h;
			param_set_pt( h, dynamic_numbers_ui			);
			param_set_pt( h, dynamic_duration_ui		);
			param_set_pt( h, dynamic_status_ui			);
			param_set_pt( h, dynamic_time_ui			);
			param_set_pt( h, b_record_rec_ui			);
			param_set_pt( h, b_dynamic_play_trig_ui		);
			param_set_pt( h, b_dynamic_stop_trig_ui		);
			param_set_pt( h, b_dynamic_clear_trig_ui	);
			param_set_pt( h, b_dynamic_loop_ui			);

	param_set_pt( h, _mackie->_s_midi_controller );

	++h;
		param_set_pt( h, _b_nrpn		);
		param_set_pt( h, _b_bcf_14bits	);
		
	++h;
		param_set_pt( h, _mackie->_b_master					);
		param_set_pt( h, _mackie->_b_bank_interpolate_allow );
		param_set_pt( h, _mackie->_mess_base				);
		param_set_pt( h, _mackie->_control_offset_ui		);
		param_set_pt( h, _mackie->_b_fader_shift_ui			);
		param_set_pt( h, _mackie->_b_vpot_shift_ui			);
		param_set_pt( h, _mackie->_b_but_shift_ui			);
		param_set_pt_n( h, c_mackie::b_but_global, c_mackie::BUT_CHANNEL_NB );
		param_set_pt( h, _mackie->_b_verbose				);
	++h;
		param_set_pt( h, _mackie->_fader_sensitivity_ui		);
		param_set_pt( h, _mackie->_fader_change_interval	);
		//hack should be in mackie
		param_set_pt_n( h, _mackie->_b_fader_contact, c_mackie::FADER_NB );
		++h;
			param_set_pt( h, _mackie->_b_slice_prev_trig	);
			param_set_pt( h, _mackie->_b_slice_next_trig	);
			param_set_pt( h, _mackie->_slice_cur_index		);
			param_attach_obj( h, c_mackie::get_slice( _mackie->_slice_cur_index ) );
			++h;
				for( INT32 i = 0; i < c_mackie::SLICE_NB; ++i )
					param_attach_obj( h, c_mackie::get_slice(i) );

//	param_set_pt( h, channel_zero_based );
	++h;
		param_set_pt( h, _sequencer_nb		);
		param_set_pt( h, _b_start_trigger	);
		param_set_pt( h, _b_stop_trigger	);
		param_set_pt( h, _b_save_trigger	);
		param_set_pt( h, _b_load_trigger	);
		param_set_pt( h, _b_record_allow	);
		param_set_pt( h, _b_record			);
		param_set_pt( h, _b_play_allow		);
		param_set_pt( h, _b_play			);

	++h;
		param_set_pt( h, _b_filter								);
		param_set_pt( h, _b_filter_program_change				);
		param_set_pt( h, _b_filter_control_change				);
		param_set_pt( h, b_route_noteon_as_control_change_ui	);

	++h;
		param_set_pt( h, _b_channel_listen_all );
		param_set_pt_n( h, _b_channel_listen, c_midi::CHANNEL_NB );

	//	param_set_pt( h, b_control_send_to_lua	);
	//	param_set_pt( h, b_program_send_to_lua	);
		param_set_pt( h, _b_control_patch_ui	);

	++h;
		for( INT32 i = 0; i < c_midi::CHANNEL_NB; ++i )
		{
			param_set_pt( h, _channel_dst[i]			);
			param_set_pt( h, _control_index_offset[i]	);
			param_set_pt( h, _control_factor[i]			);
		}

		param_set_pt( h, tc_time_offset	);
		param_set_pt( h, tc_time_out	);
		param_set_pt( h, _date_cur		);

		param_set_pt( h, control_change_last_channel_id_out );
		param_set_pt( h, control_change_last_control_index_out );

		for( INT32 i = 0; i < OUT_BANK_NB; ++i )
		{
			++h;
				param_set_pt( h, _channel_ui[i] );
				param_set_pt( h, _control_ui[i] );
				_param_index_ctl[i] = h;
				h += OUT_BANK_CTL_NB;
		}

	++h;
		param_set_pt( h, _o_verbose_name_ui				);
		param_set_pt( h, b_verbose_low_in				);
		param_set_pt( h, b_verbose_low_out				);
		param_set_pt( h, b_verbose						);
		param_set_pt( h, b_verbose_display_date			);
		param_set_pt( h, b_verbose_display_time			);
		param_set_pt( h, b_verbose_display_count		);
		param_set_pt( h, b_verbose_display_channel		);
		param_set_pt( h, b_verbose_velocity_master		);
		param_set_pt( h, b_verbose_noteon				);
		param_set_pt( h, b_verbose_velocity				);
		param_set_pt( h, b_verbose_noteoff				);
		param_set_pt( h, b_verbose_program_change		);
		param_set_pt( h, b_verbose_control_change		);
		param_set_pt( h, b_verbose_aftertouch_poly		);
		param_set_pt( h, b_verbose_aftertouch_channel	);
		param_set_pt( h, b_verbose_pitchbend			);
		param_set_pt( h, b_verbose_system_ui			);
		param_set_pt( h, b_verbose_timecode				);
		param_set_pt( h, b_verbose_clock				);
		param_set_pt( h, b_verbose_out_callback			);
		param_set_pt( h, b_verbose_read_bank			);
		param_set_pt( h, b_verbose_event				);

	err_param_init_pt( h );
	param_update_pt();
}

REAL c_midi::tc_time_in = 0;
REAL c_midi::tc_time_offset = 0;
REAL c_midi::tc_time_out = 0;

CONSTRUCTOR_CREATE( c_midi )
,_midi_in_handle			(nullptr)
,_b_open_in					(false)
,_b_open_out				(false)
,_in_index_opened_out		(-1)		//	-1 is not opened
,_out_index_opened_out		(-1)		//	-1 is not opened
,_nrpn_ctl_index			(0)
,_msb_stored				{0}
,_ctl_stored				{0}
,_b3_stored					{0}
,_in_message_nb				{0}
,_out_message_nb			{0}
{
	CHAR	str[] = "Midi_X";
	str[5] = 'a' + get_factory()->get_obj_count() - 1;
	set_name( str );

	_midi_header.set("# ");
	_midi_header.add(str);
	_midi_header.add_space();
	
	//we have to do this or param_update_pt() will crash before param are set in param_init_to_ina() called in param_init_with
	for( INT32 i=0; i<OUT_BANK_NB; ++i )
	{
		_channel_ui[i] = i+1;
		_control_ui[i] = 1;
	}

	// we need mackie to do param_init_with() below
	_mackie = new c_mackie;	//todo change mackie to c_obj and do get_obj
	_mackie->_midi = this;


	param_init_with( n_midi::param, n_midi::PARAM_NB_MAX );
	channel_listen_init();
	_play_index = 0;
	_sequencer_nb = 0;
	_date_cur = 0;

	_b_dynamic_restart_trig = false;
	_b_dynamic_stop_trig = false;
	_b_dynamic_play_trig = false;
	_b_dynamic_record_trig = false;
	_b_dynamic_clear_trig = false;

	b_dynamic_record = false;
	b_dynamic_play = false;
}

c_midi::~c_midi()
{
	close_in();
	close_out();
	obj_delete( _mackie );
	dealloc_play_record_buf();
}

INT32	c_midi::get_dynamic_play_percent()
{
	INT32	percent = 0;

	if( c_midi_universe::cur->_midi_bank[bank_cur].get_dynamic_duration() > REAL(0) )
		percent = INT32( c_midi_universe::cur->_midi_bank[bank_cur].get_dynamic_time() / c_midi_universe::cur->_midi_bank[bank_cur].get_dynamic_duration() * REAL(100) );
//	percent = ( c_midi_universe::cur->midi_bank[bank_cur_].get_dynamic_play_index() * 1000 /	c_midi_universe::cur->midi_bank[bank_cur_].get_dynamic_numbers() );
	//hack
	if( percent >= 100 )
		percent = 99;
	return percent;
}

REAL	c_midi::get_dynamic_duration()
{
	REAL	duration = c_midi_universe::cur->_midi_bank[bank_cur].get_dynamic_duration();
	return duration;
}

void c_midi::update()
{
	if( !is_active() )
		return;

	if( trackers::b_start_with_dmx )
	{
		ERR_PRINT_STRING( "because of a windows bug AAASeed can't deal with midi and serial's dmx." );
		ERR_PRINT_STRING( "disabling midi, change param start_with_dmx to use midi." );
		set_active( false );
		return;
	}

	if( _o_verbose_name_ui.is_empty() )
		_o_verbose_name.erase();
	else
	{
		_o_verbose_name.set(_o_verbose_name_ui);
		_o_verbose_name.add_space();
	}
// IN
	if( _b_enum_in_trig_ui )
	{
		_b_enum_in_trig_ui = false;
		enumerate_in();
	}
	if( _b_active_in_ui )
	{
		bool b_open_trig = false;
		bool b_close_trig = false;
		if( _b_open_in_asked_ui != _b_open_in )
		{
			if( _b_open_in_asked_ui )
				b_open_trig = true;
			else
				b_close_trig = true;
		}
		else if( _b_open_in )
		{
			if( _in_index_asked_ui != _in_index_opened_out )
			{
				b_close_trig = true;
				if( _in_index_asked_ui >=0 )
					b_open_trig = true;
			}
		}
		if( b_close_trig )
			close_in();
		if( b_open_trig )
			open_in();
	}

// OUT
	if( _b_enum_out_trig_ui )
	{
		_b_enum_out_trig_ui = false;
		enumerate_out();
	}
	if( _b_active_out_ui )
	{
		bool b_open_trig = false;
		bool b_close_trig = false;
		if( _b_open_out_asked_ui != _b_open_out )
		{
			if( _b_open_out_asked_ui )
				b_open_trig = true;
			else
				b_close_trig = true;
		}
		else if( _b_open_out )
		{
			if( _out_index_asked_ui != _out_index_opened_out )
			{
				b_close_trig = true;
				if( _out_index_asked_ui >=0 )
					b_open_trig = true;
			}
		}
		if( b_close_trig )
			close_out();
		if( b_open_trig )
			open_out();
	}

	tc_time_out = tc_time_in + tc_time_offset;

	auto& midi_data = c_midi_universe::cur->_midi_bank[bank_cur];
	dynamic_numbers_ui	= midi_data.get_dynamic_numbers();
	dynamic_duration_ui	= midi_data.get_dynamic_duration();
	dynamic_time_ui		= midi_data.get_dynamic_time();

	//todo should we do everything
	//hack wrong date
	if( _b_stop_trigger )
	{
		stop(  0 );
		_b_stop_trigger	 = false;
	}
	if( _b_start_trigger )
	{
		start( 0 );
		_b_start_trigger = false;
	}
	if( _b_save_trigger )
	{	
		seq_save();
		_b_save_trigger	 = false;
	}
	if( _b_load_trigger )
	{	
		seq_load();		
		_b_load_trigger	 = false;
	}

	if( digit_ui >= 0 )
	{
		digit_all = IMOD( digit_all * 10 + digit_ui, 1000 );
		if( b_verbose_event )
			DBG_PRINT_STRING( "%s%s %d", str_midi, "Digit all now", digit_all );
		digit_ui = -1;
	}

	if( b_dynamic_play_trig_ui )
	{
		b_dynamic_play_trig_ui = false;
		_b_dynamic_play_trig = true;
	}
	if( b_dynamic_stop_trig_ui )
	{
		b_dynamic_stop_trig_ui = false;
		_b_dynamic_stop_trig = true;
	}
	if( _b_dynamic_loop_trig )
	{
		b_dynamic_loop = !b_dynamic_loop;
//		midi_data.set_loop( b_dynamic_loop );
		_b_dynamic_loop_trig = false;
	}
	if( _b_dynamic_clear_trig )
	{
		_b_dynamic_clear_trig = false;
		midi_data.dynamic_clear_data();
	}
	if( _b_dynamic_restart_trig )
	{
		midi_data.set_record( false );
		midi_data.bank_copy_to( &( c_midi_universe::cur->_midi_data ), c_midi::bank_direct_begin, c_midi::bank_direct_end );
		midi_data.dynamic_restart();
		b_dynamic_pause = false;
		_b_dynamic_restart_trig = false;
	}
	if( _b_dynamic_stop_trig )
	{
		if( b_dynamic_play )
		{
			// playing, pause
			//b_dynamic_play = true;
			if( b_dynamic_pause )
			{
				b_dynamic_record = false;
				b_dynamic_play = false;
				b_dynamic_pause = false;
			}
			else
			{
				midi_data.dynamic_pause_begin();
				b_dynamic_pause = true;
			}
		}
		else
		{
			b_dynamic_record = false;
			b_dynamic_play = false;
			b_dynamic_pause = false;
		}
		_b_dynamic_stop_trig = false;
	}
	if( _b_dynamic_play_trig )
	{
		if( b_dynamic_pause )
		{
			// paused, so restart playing
			midi_data.dynamic_pause_end();
			b_dynamic_pause = false;
		}
		else
		{
			b_dynamic_record = false;
			midi_data.set_record( false );
			midi_data.bank_copy_to( &( c_midi_universe::cur->_midi_data), c_midi::bank_direct_begin, c_midi::bank_direct_end );
			midi_data.dynamic_restart();
		}
		b_dynamic_play = true;
		_b_dynamic_play_trig = false;
	}
	if( _b_dynamic_record_trig )
	{
		//// stop play
		//_b_dynamic_play = false;
		//// reset recording
		//if( !_b_dynamic_record )
		//	{
		//	midi_universe_cur->_midi_bank[bank_cur_].dynamic_clear_data();
		//	}
		//midi_universe_cur->_midi_bank[bank_cur_].set_record( true );
		//_b_dynamic_record = true;
		//_b_dynamic_record_trig = false;
		// stop play
		//_b_dynamic_play = false;
		// reset recording
		if( !b_dynamic_record )
		{
			if( b_dynamic_play )
			{
				// already playing, so create a new recording
				midi_data.dynamic_new_map( true );
			}
		}
		//if( !_b_dynamic_record )
		//	{
		//	// do we really clear or do nothing ???
		//	midi_universe_cur->_midi_bank[bank_cur_].dynamic_clear_data();
		//	}
		midi_data.set_record( true );
		b_dynamic_pause = false;
		b_dynamic_record = true;
		_b_dynamic_record_trig = false;
	}
	if( b_dynamic_play )
	{
		if( !b_dynamic_pause )
		{
			SEQ_REC_MAP	data;
			data._channel_id	= 0;
			data._control_index	= 0;
			data._value			= 0;
			INT32	date;

			while( midi_data.dynamic_play( &data, &date ) )
			{
				if( ( data._channel_id == -1 ) || ( date < 0 ) )
				{
					// last record
					if( b_dynamic_loop )
						_b_dynamic_restart_trig = true;
					else
						b_dynamic_play = false; //_b_dynamic_stop_trig = true;
				}
				else
					static_set_control( data._channel_id, data._control_index, data._value, date, true, false );
			}
		}
	}

	dynamic_status_ui = n_midi::BANK_STOP;
	if( b_dynamic_play )
	{
		if( b_dynamic_pause )
			dynamic_numbers_ui = n_midi::BANK_PAUSING;
		else
			dynamic_status_ui = n_midi::BANK_PLAYING;
	}
	if( b_dynamic_loop )
	{
		dynamic_status_ui = n_midi::BANK_LOOPING;
	}
	if( b_dynamic_record )
	{
		if( b_dynamic_play )
			dynamic_status_ui = n_midi::BANK_PLAYING_RECORDING;
		else
			dynamic_status_ui = n_midi::BANK_RECORDING;
	}

	set_bank( bank_cur_ui );	//todo should be out of mackie burt...
	if( b_bank_load_trig )		
	{	
		load_bank();
		b_bank_load_trig	= false;
	}
	if( b_bank_a_store_trig )	
	{	
		store_bank_a();
		b_bank_a_store_trig = false;
	}
	if( b_bank_b_store_trig )	
	{	
		store_bank_b();
		b_bank_b_store_trig = false;
	}
	if( b_bank_a_save_trig )	
	{	
		save_bank_a();
		b_bank_a_save_trig	= false;
	}
	if( b_bank_b_save_trig )	
	{	
		save_bank_b();
		b_bank_b_save_trig	= false;
	}

	_mackie->update();
	play_update();

	param_update_pt();
}


void	c_midi::open_in_low( INT32 device_index )
{
	if( _b_open_in )
		return;

	MMRESULT result_code;
	_s_nrpn = 0;	//make sure we reset it when we start
	try
	{
		result_code = midiInOpen( &_midi_in_handle, (UINT)device_index, ( DWORD_PTR ) midi_in_callback, ( DWORD_PTR )this, CALLBACK_FUNCTION|MIDI_IO_STATUS );
	}
	catch( ... )
	{
		box_err( "Error in midiInOpen()" );
		return;
	}

	if( result_code == MMSYSERR_NOERROR )
	{
		PRINT_STRING( "IN %d Opened", device_index );
		result_code = midiInStart( _midi_in_handle );
		if( result_code == MMSYSERR_NOERROR )
		{
			PRINT_STRING( "IN %d Started", device_index );
			_b_open_in = true;
			_in_index_opened_out = device_index;
			_in_message_nb = 0;

			MIDIINCAPS caps;
			MMRESULT result = midiInGetDevCaps( device_index, &caps, sizeof( MIDIINCAPS ) );
			if( result == MMSYSERR_NOERROR )
			{
				//PRINT_STRING( "IN %d : %S Manufacturer %d Product %d", i, caps.szPname, caps.wMid, caps.wPid );
				_in_device_name_out.set( caps.szPname );
			}
			else
				_in_device_name_out.set( "Did not got device name" );
		}
	}
	else
		print_error( result_code );
	//	update();
}

//todo remove this
extern	INT32	dummy_int32[];

bool b_test = true;
INT32 target = 0;

void	c_midi::out_send_test()
{
//	send_bytes_3( 0xe0, 0x7f, 0x3f );
	++target;
	b_test = !b_test;

		
/*	for( INT32 i=0; i<64; ++i )
		send_bytes_3( 0xbe, i, b_test?0:0x7f );
	for( INT32 i=0; i<64; ++i )
		send_bytes_3( 0xbd, i, b_test?0:0x7f );
	for( INT32 i=0; i<64; ++i )
		send_bytes_3( 0xbd, 0x40+i, !b_test?0:0x7f );
*/
/*
//CS32
	send_bytes_3( 0xbe, 0x4f, b_test ? 1 : 0 );	//record
	send_bytes_3( 0xbe, 0x57, b_test ? 1 : 0 );	//arrow left 
	send_bytes_3( 0xbe, 0x58, b_test ? 1 : 0 );	//arrow right
	send_bytes_3( 0xbe, 0x59, b_test ? 1 : 0 );	//arrow up
	send_bytes_3( 0xbe, 0x5a, b_test ? 1 : 0 );	//arrow down
	send_bytes_3( 0xbe, 0x5b, b_test ? 1 : 0 );	//dot left
	send_bytes_3( 0xbe, 0x5c, b_test ? 1 : 0 );	//dot right
	send_bytes_3( 0xbe, 0x5d, target );	//aficheur left
	send_bytes_3( 0xbe, 0x5e, target );	//aficheur right and ON all unused from 0x40 to 0X5f
*/
	set_channel(4);
	send_note_on( dummy_int32[0], dummy_int32[1] );

	//	send_bytes_3( 0xbe, 0x40 + target, b_test?0:0x7f );
	/*
	send_note_off( dummy_int32[0], dummy_int32[1] );
	send_note_on( dummy_int32[0], dummy_int32[1] );
	send_polyphonic_pressure( dummy_int32[0], dummy_int32[1] );
	send_control_change( dummy_int32[0], dummy_int32[1] ); 
	send_program_change( dummy_int32[0] );
	send_channel_pressure( dummy_int32[1] );
	*/
}

void	c_midi::open_out_low( INT32 device_index )
{
	if( _b_open_out )
		return;
	MMRESULT	result_code;
	//_out_index = -1;
	try
	{
#if	AAA_MIDI_STREAM()
			UINT		stream_dev_id = dev_id;
			result_code = midiStreamOpen( &midi_out_stream, &stream_dev_id, ( DWORD )1,
							( DWORD )midi_out_callback, ( DWORD )dev_id, CALLBACK_FUNCTION );
#else
			result_code = midiOutOpen( &_midi_out_handle, device_index, ( DWORD_PTR ) midi_out_callback, 222, CALLBACK_FUNCTION );
#endif
	}
	catch( ... )
	{
		box_err( "Error in midiOutOpen()" );
		return;
	}
	if( result_code == MMSYSERR_NOERROR )
	{
		PRINT_STRING( "OUT %d Opened", device_index );

		_midi_out_st.lpData = (CHAR*) MALLOC( sizeof(char)*1024 );
		_midi_out_st.dwBufferLength = 1024;
		_midi_out_st.dwFlags = 0;
#if	AAA_MIDI_STREAM()
		result_code = midiOutPrepareHeader( ( HMIDIOUT )midi_out_stream, &_midi_out_st, sizeof( MIDIHDR ) ); 
#else
		result_code = midiOutPrepareHeader( _midi_out_handle, &_midi_out_st, sizeof( MIDIHDR ) ); 
#endif
		if( result_code == MMSYSERR_NOERROR )
		{
			PRINT_STRING( "OUT %d Ready", device_index );

#if	AAA_MIDI_STREAM()
			result_code = midiStreamRestart( midi_out_stream ); 
#else
			result_code = midiOutReset( _midi_out_handle ); 
#endif
			if( result_code == MMSYSERR_NOERROR )
			{
				_b_open_out = true;
				_out_index_opened_out = device_index;
				_out_message_nb = 0;

				MIDIOUTCAPS caps;
				MMRESULT result = midiOutGetDevCaps( device_index, &caps, sizeof( MIDIINCAPS ) );
				if( result == MMSYSERR_NOERROR )
				{
					//PRINT_STRING( "IN %d : %S Manufacturer %d Product %d", i, caps.szPname, caps.wMid, caps.wPid );
					_out_device_name_out.set( caps.szPname );
				}
				else
					_out_device_name_out.set( "Did not got device name" );
//				out_send_test();
				goto exit;
			}
		}
	}
	else
		print_error( result_code );
exit:
	;
//	update();
}

void	c_midi::close_in()
{
	if( !_b_open_in )
		return;
	if( c_obj_ui::b_aaa_exiting && _b_skip_close_on_exit )
	{
		DBG_PRINT_STRING( "%s IN skipping close as asked", MIDI_HEADER );
		return;
	}

	{
		MMRESULT result_code;
		result_code = midiInStop( _midi_in_handle );
		if( result_code == MMSYSERR_NOERROR )
		{
			PRINT_STRING( "IN Stopped" );
			result_code = midiInClose( _midi_in_handle );
			if( result_code == MMSYSERR_NOERROR )
			{
				PRINT_STRING( "IN Closed" );
				_b_open_in = false;
				_in_device_name_out.erase();
			}
			else
				ERR_PRINT_STRING( "% IN troubles Closing : error %d", MIDI_HEADER, result_code );
		}
		else
			ERR_PRINT_STRING( "%s IN troubles Stopping : error %d", MIDI_HEADER, result_code );
	}
}

void	c_midi::close_out()
{
	if( !_b_open_out )
		return;
	if( c_obj_ui::b_aaa_exiting && _b_skip_close_on_exit )
	{
		DBG_PRINT_STRING( "%s OUT skipping close as asked", MIDI_HEADER );
		return;
	}

	{
		MMRESULT result_code;
#if	AAA_MIDI_STREAM()
		result_code = midiStreamStop( _midi_out_handle );
		if( result_code == MMSYSERR_NOERROR )
		{
			PRINT_STRING( "OUT Stopped" );
			result_code = midiOutUnprepareHeader( ( HMIDIOUT )midi_out_stream, &_midi_out_st, sizeof( MIDIHDR ) );
			result_code = midiStreamClose( _midi_out_handle );
			if( result_code == MMSYSERR_NOERROR )
			{
				PRINT_STRING( "OUT Closed" );
				_b_open_out = false;
			}
		}
		else
			ERR_PRINT_STRING( "%s OUT troubles Stopping Stream : error %d", MIDI_HEADER, result_code );
#else
		if( _b_close_do_unprepare ) 
		{
			result_code = midiOutUnprepareHeader( _midi_out_handle, &_midi_out_st, sizeof( MIDIHDR ) ); 
			if( result_code == MMSYSERR_NOERROR )
				PRINT_STRING( "OUT Header Unprepared" );
			else
				ERR_PRINT_STRING( "%s OUT troubles Unpreparing Header : error %d", MIDI_HEADER, result_code );
		}
		else
			DBG_PRINT_STRING( "%s OUT skipping Unprepare as asked", MIDI_HEADER );

		result_code = midiOutClose( _midi_out_handle );
		if( result_code == MMSYSERR_NOERROR )
		{
			_b_open_out = false;
			PRINT_STRING( "OUT Closed" );
			_out_index_opened_out = -1;
			_out_device_name_out.erase();
		}
		else
			ERR_PRINT_STRING( "%s OUT troubles closing : error %d", MIDI_HEADER, result_code );
#endif
	}

	FREE_AND_NULL( _midi_out_st.lpData );
	_midi_out_st.dwBufferLength = 0;

//// clear recording data, since it's dynamic
//for( INT32 i = 0; i < MIDI_BANK_NB; ++i )
//	{
//	c_midi_universe::cur->midi_bank[i].dynamic_clear_data();
//	}
}

INT32	c_midi::enumerate_in()
{
	INT32	nb = midiInGetNumDevs();
	PRINT_STRING( "IN There is %d device%s%s", nb, nb>1 ? "s" : "", nb>0 ? " :" : "." );
	for( INT32 i = 0; i < nb; ++i )
	{
		MIDIINCAPS	caps;
		MMRESULT result = midiInGetDevCaps( i, &caps, sizeof( MIDIINCAPS ) );
		if( result == MMSYSERR_NOERROR )
		{
			PRINT_STRING( "IN %d : %S Manufacturer %d Product %d", i, caps.szPname, caps.wMid, caps.wPid );
		}
		else
		{
			print_error( result );
			PRINT_STRING( "can't read Caps for MidiIn %d.", i );
		}
	}
	return nb;
}

INT32	c_midi::enumerate_out()
{
	INT32	nb = midiOutGetNumDevs();
	PRINT_STRING( "OUT There is %d device%s%s", nb, nb>1 ? "s" : "", nb>0 ? " :" : "." );
	for( INT32 i = 0; i < nb; ++i )
	{
		MIDIOUTCAPS	caps;
		MMRESULT result = midiOutGetDevCaps( i, &caps, sizeof( MIDIOUTCAPS ) );
		if( result == 0 )
		{
			PRINT_STRING( "OUT %d : %S Manufacturer %d Product %d", i, caps.szPname, caps.wMid, caps.wPid );
		}
		else
		{
			print_error( result );
			PRINT_STRING( "can't read Caps for MidiOut %d.", i );
		}
	}
	return nb;
}

void	c_midi::open_in()
{
	if( _in_index_asked_ui >=0 )
	{
		INT32 nb = enumerate_in();
		if( _in_index_asked_ui < nb )
			open_in_low( _in_index_asked_ui );
		else
			PRINT_STRING( "There is %d MIDI IN device, and you asked from the %d index.", nb, _in_index_asked_ui );
	} 
}

void	c_midi::open_out()
{
	if( _out_index_asked_ui >= 0 )
	{
		INT32 nb = enumerate_out();
		if( _out_index_asked_ui < nb )
			open_out_low( _out_index_asked_ui );
		else
			PRINT_STRING( "There is %d MIDI OUT device, and you asked from the %d index.", nb, _out_index_asked_ui );
	}
}

/*
void	c_midi::open()
{
	open_in();
	open_out();
}
*/

UINT32 c_midi::send_bytes( UINT8* str, INT32 len ) 
{
	MMRESULT result_code = 0;
	if( _b_open_out )
	{
#if 1
		_midi_out_buf.lpData = ( CHAR* )str;
		_midi_out_buf.dwBufferLength = len;
		_midi_out_buf.dwFlags = 0;
		result_code = midiOutPrepareHeader( ( HMIDIOUT )_midi_out_handle, &_midi_out_buf, sizeof( MIDIHDR ) );
		if( result_code == MMSYSERR_NOERROR )
		{
			result_code = midiOutLongMsg( ( HMIDIOUT )_midi_out_handle, &_midi_out_buf, sizeof( MIDIHDR ) );
			if( result_code != MMSYSERR_NOERROR )
				print_error( result_code );
			midiOutUnprepareHeader( ( HMIDIOUT )_midi_out_handle, &_midi_out_buf, sizeof( MIDIHDR ) );
		}
		if( result_code != MMSYSERR_NOERROR )
			print_error( result_code );
#else
		MEMCPY( _midi_out_st.lpData, str, len );
		_midi_out_st.dwBufferLength = len;
		_midi_out_st.dwFlags = 2;	//this what validate header does (disassembly)
		midiOutPrepareHeader( ( HMIDIOUT )_midi_out_handle, &_midi_out_st, sizeof( MIDIHDR ) );
		result_code = midiOutLongMsg( ( HMIDIOUT )_midi_out_handle, &_midi_out_st, sizeof( MIDIHDR ) );
		if( result_code != MMSYSERR_NOERROR )
			print_error( result_code );
#endif
	}
	return result_code;
} 

UINT32	c_midi::send_bytes_2( UINT8 status, UINT8 b1 ) 
{ 
	MMRESULT result_code = 0;
	if( _b_open_out )
	{
		if( b_verbose_low_out )
			print_low( STR_OUT, status, b1 );
		union	{	INT32 l; INT8 b[4]; }	u; 

		u.b[0] = status;
		u.b[1] = b1;
		//u.b[2] = 0;
		//u.b[3] = 0; 

		result_code = midiOutShortMsg( ( HMIDIOUT )_midi_out_handle, u.l ); 
		if( result_code == MMSYSERR_NOERROR )
			++_out_message_nb;
		else
			print_error( result_code );
	}
	else
	{
		if( b_verbose_low_out )
			print_low( "OUT asked with device inactive ", status, b1 );
	}
	return result_code;
}
UINT32	c_midi::send_bytes_3( UINT8 status, UINT8 b1, UINT8 b2 ) 
{
	MMRESULT result_code = 0;
	if( _b_open_out )
	{
		if( b_verbose_low_out )
			print_low( STR_OUT, status, b1, b2 );

		union	{	INT32 l; INT8 b[4]; }	u; 
		u.b[0] = status;
		u.b[1] = b1;
		u.b[2] = b2;
		//u.b[3] = 0; 
		result_code = midiOutShortMsg( ( HMIDIOUT )_midi_out_handle, u.l ); 
		if ( result_code == MMSYSERR_NOERROR )
			++_out_message_nb;
		else
			print_error( result_code );
	}
	else
	{
		if( b_verbose_low_out )
			print_low( "OUT asked with device inactive ", status, b1, b2 );
	}
	return result_code;
} 

//	Base Out Messages
UINT32	c_midi::send_note_off( UINT8 note_index, UINT8 velocity )
	{	return send_bytes_3( 0x80|_channel_zero_based, note_index,		velocity	);	}
UINT32	c_midi::send_note_on( UINT8 note_index, UINT8 velocity )
	{	return send_bytes_3( 0x90|_channel_zero_based, note_index,		velocity	);	}
UINT32	c_midi::send_polyphonic_pressure( UINT8 note_index, UINT8 value )
	{	return send_bytes_3( 0xa0|_channel_zero_based, note_index,		value		);	}
UINT32	c_midi::send_control_change( UINT8 control_index, UINT8 value )
	{	return send_bytes_3( 0xb0|_channel_zero_based, control_index,	value		);	}
UINT32	c_midi::send_program_change( UINT8 program_index )	
	{	return send_bytes_2( 0xc0|_channel_zero_based, program_index				);	}
UINT32	c_midi::send_channel_pressure( UINT8 value )	
	{	return send_bytes_2( 0xd0|_channel_zero_based, value						);	}

void	c_midi::send_nrpn( UINT8 control_index, INT32 value ) 
{	
	UINT8 b1 = 0xb0 | _channel_zero_based;
	send_bytes_3( b1, 0x63, 0 );
	send_bytes_3( b1, 0x62, control_index + 1 );
	send_bytes_3( b1, 0x06, ( value>>7 ) & 0x7f );
	send_bytes_3( b1, 0x26, value & 0x7f );
}
void	c_midi::send_14bits( UINT8 control_index, INT32 value ) 
{	
	UINT8 b1 = 0xb0 | _channel_zero_based;
	send_bytes_3( b1, control_index,	value >> 7 );
	send_bytes_3( b1, control_index+32, value & 0x7f );
}
void	c_midi::send_nrpn_or_14bits( UINT8 control_index, INT32 value ) 
{	
	if( _b_bcf_14bits )
		send_14bits( control_index, value );
	else
		send_nrpn( control_index, value );
}
