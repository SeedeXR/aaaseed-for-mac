#include "sound_bass.h"
#include "snd_player_bass.h"
#include "snd_input_bass.h"
#include "infrastructure/param/param_declare.h"
#include "snd_master.h"

C_PCHAR_C	c_sound_bass::get_error_str_short( UINT32 err )
{
	C_PCHAR	s;
	switch( err )
	{
	case BASS_OK :				s = "BASS_OK"; break;
	case BASS_ERROR_MEM :		s = "BASS_ERROR_MEM"; break;
	case BASS_ERROR_FILEOPEN :	s = "BASS_ERROR_FILEOPEN"; break;
	case BASS_ERROR_DRIVER :	s = "BASS_ERROR_DRIVER"; break;
	case BASS_ERROR_BUFLOST :	s = "BASS_ERROR_BUFLOST"; break;
	case BASS_ERROR_HANDLE :	s = "BASS_ERROR_HANDLE"; break;
	case BASS_ERROR_FORMAT :	s = "BASS_ERROR_FORMAT"; break;
	case BASS_ERROR_POSITION :	s = "BASS_ERROR_POSITION"; break;
	case BASS_ERROR_INIT :		s = "BASS_ERROR_INIT"; break;
	case BASS_ERROR_START :		s = "BASS_ERROR_START"; break;
	case BASS_ERROR_ALREADY :	s = "BASS_ERROR_ALREADY"; break;
//		case BASS_ERROR_NOPAUSE :	s = "BASS_ERROR_NOPAUSE"; break;
	case BASS_ERROR_NOCHAN :	s = "BASS_ERROR_NOCHAN"; break;
	case BASS_ERROR_ILLTYPE :	s = "BASS_ERROR_ILLTYPE"; break;
	case BASS_ERROR_ILLPARAM :	s = "BASS_ERROR_ILLPARAM"; break;
	case BASS_ERROR_NO3D :		s = "BASS_ERROR_NO3D"; break;
	case BASS_ERROR_NOEAX :		s = "BASS_ERROR_NOEAX"; break;
	case BASS_ERROR_DEVICE :	s = "BASS_ERROR_DEVICE"; break;
	case BASS_ERROR_NOPLAY :	s = "BASS_ERROR_NOPLAY"; break;
	case BASS_ERROR_FREQ :		s = "BASS_ERROR_FREQ"; break;
	case BASS_ERROR_NOTFILE :	s = "BASS_ERROR_NOTFILE"; break;
	case BASS_ERROR_NOHW :		s = "BASS_ERROR_NOHW"; break;
	case BASS_ERROR_EMPTY :		s = "BASS_ERROR_EMPTY"; break;
	case BASS_ERROR_NONET :		s = "BASS_ERROR_NONET"; break;
	case BASS_ERROR_CREATE :	s = "BASS_ERROR_CREATE"; break;
	case BASS_ERROR_NOFX :		s = "BASS_ERROR_NOFX"; break;
//	case BASS_ERROR_PLAYING :	s = "BASS_ERROR_PLAYING"; break;
	case BASS_ERROR_NOTAVAIL :	s = "BASS_ERROR_NOTAVAIL"; break;
	case BASS_ERROR_DECODE :	s = "BASS_ERROR_DECODE"; break;
	case BASS_ERROR_DX :		s = "BASS_ERROR_DX"; break;
	case BASS_ERROR_TIMEOUT :	s = "BASS_ERROR_TIMEOUT"; break;
	case BASS_ERROR_FILEFORM :	s = "BASS_ERROR_FILEFORM"; break;
	case BASS_ERROR_SPEAKER :	s = "BASS_ERROR_SPEAKER"; break;
	case BASS_ERROR_VERSION :	s = "BASS_ERROR_VERSION"; break;
	case BASS_ERROR_CODEC 	:	s = "BASS_ERROR_CODEC"; break;
	case BASS_ERROR_ENDED 	:	s = "BASS_ERROR_ENDED"; break;
	case BASS_ERROR_BUSY 	:	s = "BASS_ERROR_BUSY"; break;
	case BASS_ERROR_UNKNOWN :	s = "BASS_ERROR_UNKNOWN"; break;
	default :					s = "UNKNOWN"; break;
	}
	return s;
}

C_PCHAR_C	c_sound_bass::get_error_str( UINT32 err )
{
	C_PCHAR	s;
	switch( err )
	{
	case BASS_OK :				s = "all is OK";						break;
	case BASS_ERROR_MEM :		s = "memory error";						break;
	case BASS_ERROR_FILEOPEN :	s = "can't open the file";				break;
	case BASS_ERROR_DRIVER :	s = "can't find a free/valid driver";	break;
	case BASS_ERROR_BUFLOST :	s = "the sample buffer was lost";		break;
	case BASS_ERROR_HANDLE :	s = "invalid handle";					break;
	case BASS_ERROR_FORMAT :	s = "unsupported sample format";		break;
	case BASS_ERROR_POSITION :	s = "invalid position";					break;
	case BASS_ERROR_INIT :		s = "BASS_Init has not been successfully called";	break;
	case BASS_ERROR_START :		s = "BASS_Start has not been successfully called";	break;
	case BASS_ERROR_ALREADY :	s = "already initialized/paused/whatever";			break;
		//		case BASS_ERROR_NOPAUSE :	s = "BASS_ERROR_NOPAUSE"; break;
	case BASS_ERROR_NOCHAN :	s = "can't get a free channel"; break;
	case BASS_ERROR_ILLTYPE :	s = "an illegal type was specified"; break;
	case BASS_ERROR_ILLPARAM :	s = "an illegal parameter was specified"; break;
	case BASS_ERROR_NO3D :		s = "no 3D support"; break;
	case BASS_ERROR_NOEAX :		s = " no EAX support"; break;
	case BASS_ERROR_DEVICE :	s = "illegal device number"; break;
	case BASS_ERROR_NOPLAY :	s = "not playing"; break;
	case BASS_ERROR_FREQ :		s = "illegal sample rate"; break;
	case BASS_ERROR_NOTFILE :	s = "the stream is not a file stream"; break;
	case BASS_ERROR_NOHW :		s = "no hardware voices available"; break;
	case BASS_ERROR_EMPTY :		s = "the MOD music has no sequence data"; break;
	case BASS_ERROR_NONET :		s = "no internet connection could be opened"; break;
	case BASS_ERROR_CREATE :	s = " couldn't create the file"; break;
	case BASS_ERROR_NOFX :		s = "effects are not available"; break;
		//	case BASS_ERROR_PLAYING :	s = "BASS_ERROR_PLAYING"; break;
	case BASS_ERROR_NOTAVAIL :	s = "requested data is not available"; break;
	case BASS_ERROR_DECODE :	s = " the channel is a \"decoding channel\""; break;
	case BASS_ERROR_DX :		s = "a sufficient DirectX version is not installed"; break;
	case BASS_ERROR_TIMEOUT :	s = "connection timeout"; break;
	case BASS_ERROR_FILEFORM :	s = "unsupported file format"; break;
	case BASS_ERROR_SPEAKER :	s = " unavailable speaker"; break;
	case BASS_ERROR_VERSION :	s = "invalid BASS version (used by add-ons)"; break;
	case BASS_ERROR_CODEC 	:	s = "codec is not available/supported"; break;
	case BASS_ERROR_ENDED 	:	s = "the channel/file has ended"; break;
	case BASS_ERROR_BUSY 	:	s = "the device is busy"; break;
	case BASS_ERROR_UNKNOWN :	s = "some other mystery problem"; break;
	default :					s = "UNKNOWN"; break;
	}
	return s;
}

C_PCHAR_C	c_sound_bass::get_input_type( INT32 s )
{
	C_PCHAR	type;
	switch ( s & BASS_INPUT_TYPE_MASK )
	{
	case BASS_INPUT_TYPE_DIGITAL:	type = "digital";		break;
	case BASS_INPUT_TYPE_LINE:		type = "line-in";		break;
	case BASS_INPUT_TYPE_MIC:		type = "microphone";	break;
	case BASS_INPUT_TYPE_SYNTH:		type = "midi synth";	break;
	case BASS_INPUT_TYPE_CD:		type = "analog cd";		break;
	case BASS_INPUT_TYPE_PHONE:		type = "telephone";		break;
	case BASS_INPUT_TYPE_SPEAKER:	type = "pc speaker";	break;
	case BASS_INPUT_TYPE_WAVE:		type = "wave/pcm";		break;
	case BASS_INPUT_TYPE_AUX:		type = "aux";			break;
	case BASS_INPUT_TYPE_ANALOG:	type = "analog";		break;
	default:						type = "undefined";		break;
	}
	return type;
}

void	c_sound_bass::report_error( INT32 h, C_PCHAR_C str_fn )
{
	ERR_PRINT_STRING( "sound_bass %s() error %s : %s.", str_fn, get_error_str_short( h ), get_error_str( h ) );
}

void	c_sound_bass::report_error( C_PCHAR_C str_fn )
{
	report_error( dll_bass.BASS_ErrorGetCode(), str_fn );
}

FACTORY_CREATE_V1( c_sound_bass, sound_bass, sound Input Bass, sound_bass );

namespace n_sound_bass
{
	CONSTEXPR INT32	BASE_PARAM_NB	= 2;
	CONSTEXPR INT32	INPUT_PARAM_NB	= 5;
	CONSTEXPR INT32	OUTPUT_PARAM_NB	= 5;
	CONSTEXPR INT32	GROUP_NB		= 2;

	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	INPUT_PARAM_NB
									+	OUTPUT_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_ON(	active				)
		PARAM_DEF_INT32(	channel_nb_asked,	1, 2,	1, 2	)	
		PARAM_DEF_GROUP( Input, INPUT_PARAM_NB )
			PARAM_DEF_INT32_POS_ZERO(	in_device )
			PARAM_DEF_INT32_LOCKED(		in_device_nb )
			PARAM_DEF_INT32_POS(		in_channel_nb,	0, 2 )
			PARAM_DEF_SYMBO_PSTR_ZERO(	in_sample_rate,		snd::sample_rate_str )
			PARAM_DEF_SYMBO_PSTR_ZERO(	in_bits,			snd::sample_bits_str )
		PARAM_DEF_GROUP( Output, OUTPUT_PARAM_NB )
			PARAM_DEF_INT32_POS_ZERO(	out_device )
			PARAM_DEF_INT32_LOCKED(		out_device_nb )
			PARAM_DEF_INT32_POS(		out_channel_nb,	0, 2 )
			PARAM_DEF_SYMBO_PSTR_ZERO(	out_sample_rate,	snd::sample_rate_str )
			PARAM_DEF_SYMBO_PSTR_ZERO(	out_bits,			snd::sample_bits_str )
	};
};

void	c_sound_bass::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, _channel_nb_asked_def	);

	++h;
		param_set_pt( h, _device_in				);
		param_set_pt( h, _device_in_count		);
		param_set_pt( h, _nb_channel_in			);
		param_set_pt( h, _s_sample_rate_in_ui	);
		param_set_pt( h, _s_sample_bits_in_ui	);
	
	++h;
		param_set_pt( h, _device_out			);
		param_set_pt( h, _device_out_count		);
		param_set_pt( h, _nb_channel_out		);
		param_set_pt( h, _s_sample_rate_out_ui	);
		param_set_pt( h, _s_sample_bits_out_ui	);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE( c_sound_bass )
{
	param_init_with( n_sound_bass::param, n_sound_bass::PARAM_NB_MAX );
}

c_sound_bass::~c_sound_bass()
{
}



// using struct as local variable fuck up the printing when Visual

void	dump_device_one( C_PCHAR pre, INT32 index, BASS_DEVICEINFO& device_info )
{
	C_PCHAR str = (device_info.flags & BASS_DEVICE_ENABLED) ? "Enabled" : "Disabled";
	snd::PRINT( "%s %d : %.64s", pre, index, str );
	if( device_info.name )
		snd::PRINT( "\t\tName : %.64s",	device_info.name		);
	if( device_info.driver )
		snd::PRINT( "\t\tDriver : %.64s",	device_info.driver	);
	if( device_info.flags & BASS_DEVICE_DEFAULT )
		snd::PRINT( "\t\tis Default Device"						);
	if( device_info.flags & BASS_DEVICE_INIT )
		snd::PRINT( "\t\tis already Bass initialized"			);	
}

INT32	c_sound_bass::enumerate_device_out()
{
	snd::PRINT( "Bass --- Enumerate :" );
	BASS_DEVICEINFO	device_info;
	INT32 count = 0; // the device counter, 0 is a no Sound device
	while( dll_bass.BASS_GetDeviceInfo( count, &device_info ) )
	{
		// count it
		dump_device_one( "Out Device", count, device_info );
		++count;
	}
	snd::PRINT( "Out Device(s) detected: %d", count );
	return count-1;
}

INT32	c_sound_bass::enumerate_device_in()
{
	snd::PRINT( "Bass --- Enumerate Input :" );
	BASS_DEVICEINFO	device_info;
	INT32 count = 0; // the device counter
	while( dll_bass.BASS_RecordGetDeviceInfo( count, &device_info ) )
	{
		++count; // count it
		dump_device_one( "Record Device", count, device_info );
	}
	snd::PRINT( "Record Device(s) detected: %d", count );
	return count;
}

bool	c_sound_bass::load_lib()
{
	return dll_bass.init();
}

void	c_sound_bass::unload_lib()
{
	dll_bass.deinit();
}

bool	c_sound_bass::open_lib()
{
	if( !load_lib() )
	{
		snd::ERR_PRINT( "dll not loaded" );
		return false;
	}

	DWORD version = dll_bass.BASS_GetVersion();
	snd::PRINT( "Bass version %d.%d.%d.%d loaded", version>>24, (version>>16)&0xff, (version>>8)&0xff, version&0xff );
	//	check that BASS 2.4 is present
	if( HIWORD(version) != BASSVERSION )	
	{
		snd::ERR_PRINT( "Bass version 2.4 was not loaded" );
		return false;
	}
	return true;
}

void	c_sound_bass::close_lib()
{
	unload_lib();
}

void	c_sound_bass::init_output()
{
	if( _b_output_enable[0] )
		return;

	snd::PRINT( "enabling sound output on default device" );
	// setup output - get latency
	if( !dll_bass.BASS_Init( -1, snd::SAMPLE_PER_SEC_DEF, BASS_DEVICE_LATENCY, 0, nullptr ) )	//todo remoce default ?
	{
		snd::ERR_PRINT( "can't initialize device for output" );
		_b_output_enable[ 0 ] = false;
		return;
	}

	snd::PRINT( "playback" );
	DWORD		buflen;
	BASS_INFO	info;
	INT32		index;

	dll_bass.BASS_GetInfo( &info );
//	_output_nb = info.speakers;
	snd::PRINT(		"speakers : %d", info.speakers );

	// default buffer size = update period + 'minbuf'
	//	15 ms update period
	INT32 CONST UPDATE_PERIOD = 50;
	INT32 CONST BUFFER_BASE = 500;
	buflen = dll_bass.BASS_SetConfig(BASS_CONFIG_BUFFER, BUFFER_BASE + info.minbuf);
	dll_bass.BASS_SetConfig( BASS_CONFIG_UPDATEPERIOD, UPDATE_PERIOD );	//hack beyrouth

	DWORD			device = dll_bass.BASS_GetDevice();
	BASS_DEVICEINFO	device_info;
	dll_bass.BASS_GetDeviceInfo(device, &device_info);
	dll_bass.BASS_GetInfo(&info);
	index = dll_bass.BASS_GetDevice();
	snd::PRINT( "Device %d :", index );
	C_PCHAR name = device_info.name;
	if( reinterpret_cast<uintptr_t>(name) < 0x10000 )
		snd::PRINT(	"\t\tHuh device name Troubles, address is %d", name );
	else
		snd::PRINT(	"\t\t%s", name );
	snd::PRINT(		"\t\tlatency : %dms", (INT32)info.latency );
	snd::PRINT(		"\t\tminbuf  : %dms", (INT32)info.minbuf );
	snd::PRINT(		"\t\tbuffer  : %dms", (INT32)dll_bass.BASS_GetConfig (BASS_CONFIG_BUFFER ) ); //  info.minbuf );
	snd::PRINT(		"\t\tupdate period : %dms", (INT32)dll_bass.BASS_GetConfig( BASS_CONFIG_UPDATEPERIOD ) );
	snd::PRINT(		"\t\tds version : %d (effects %s)", (INT32)info.dsver, info.dsver < 8 ? "disabled" : "enabled" );
	_b_output_enable[ 0 ] = true;
}

//todosnd
/*
void	c_sound_bass::deinit_output()
{
	delete_player_all();
	if( _b_output_enable[ 0 ] )
	{
		dll_bass.BASS_Free();
		_b_output_enable[ 0 ] = false;
	}
}
*/

c_sound_input*	c_sound_bass::create_input_new()
{
	return new c_sound_input_bass;
}

c_sound_player*	c_sound_bass::create_player_new()
{
	return new c_sound_player_bass;
}


////void	c_sound_bass::update_output()
////{
////	if( !bass_stream && wave_filename.is_not_empty() )
////	{
////		if( ERR( bass_load_from_file( &bass_stream, wave_filename.get(), b_loop ) ) )
////		{
////			dealloc();
////			wave_filename.erase();
////		}
////		else
////		{
////			BASS_CHANNELINFO	info;
////			BASS_ChannelGetInfo( bass_stream, &info);
////			freq_int = info.freq;
////			DWORD	length = BASS_ChannelGetLength( bass_stream );
////			duration = BASS_ChannelBytes2Seconds( bass_stream, length);
////			byte_per_sec = length / duration;
////			if( info.flags & BASS_SAMPLE_8BITS )
////				bit_nb = 8;
////			else if( info.flags & BASS_SAMPLE_FLOAT )
////				bit_nb = 32;
////			else
////				bit_nb = 16;
////			frequency = info.freq;
////			channel_nb = info.chans;
////		}
////	}
////	if( bass_stream )
////	{
////		BASS_CHANNELINFO	info;
////
////		//		BASS_Update();	//hack beyrouth
////		BASS_ChannelGetInfo( bass_stream, &info);
////		freq_int = info.freq;
////		position = BASS_ChannelGetPosition( bass_stream );
////		time = BASS_ChannelBytes2Seconds( bass_stream, position );
////
////		if( freq_last != freq )
////		{
////			if( BASS_ChannelSetAttributes( bass_stream, freq_int*freq, -1, -101 ) )
////			{
////				freq_last = freq;
////			}
////			else
////			{
////				bass_error_report( BASS_ErrorGetCode(), "SetFrequency" );
////			}
////		}
////		if ( jump_to != 0.)
////		{
////			UINT32	pos;
////			pos = BASS_ChannelSeconds2Bytes( bass_stream, jump_to );
////			if( !BASS_ChannelSetPosition( bass_stream, pos ) )
////			{
////				bass_error_report( BASS_ErrorGetCode(), "SetPosition" );
////			}
////			jump_to = 0.;
////		}
////		//		}
////		//todo opt
////		//	if( bass_stream )
////		//		{
////		DWORD	stat;
////		stat = BASS_ChannelIsActive( bass_stream );
////		b_play = ( stat & BASS_ACTIVE_PLAYING );
////
////		if( volume_ui != volume )
////		{
////			if( BASS_ChannelSetAttributes( bass_stream, -1, (INT)(volume_ui*100), -101 ) )
////			{
////				volume = volume_ui;
////			}
////			else
////			{
////				bass_error_report( BASS_ErrorGetCode(), "SetVolume" );
////			}
////		}
////		if( pan_ui != pan )
////		{
////			if( BASS_ChannelSetAttributes( bass_stream, -1, -1, (INT) interpolate( -100, 100, pan_ui) ) )
////			{
////				pan = pan_ui;
////			}
////			else
////			{
////				bass_error_report( BASS_ErrorGetCode(), "SetPan" );
////			}
////		}
////		if ( b_play )
////		{
////			if ( b_trig_stop )
////			{
////				if( !BASS_ChannelStop( bass_stream ) )
////				{
////					bass_error_report( BASS_ErrorGetCode(), "Stop" );
////				}
////				b_trig_stop = false;
////			}
////		}
////		else
////		{
////			if ( b_trig_start )
////			{
////				b_trig_start = false;
////				if ( BASS_ChannelPlay( bass_stream, TRUE ) )
////				{
////					if( b_loop )
////					{
////						BASS_CHANNELINFO info;
////						BASS_ChannelGetInfo( bass_stream, &info ); // get info
////						if( !(info.flags & BASS_SAMPLE_LOOP ) )
////						{
////							info.flags |= BASS_SAMPLE_LOOP; // toggle the looping flag
////							BASS_ChannelSetFlags( bass_stream, info.flags ); // apply the change
////						}
////					}
////					else
////					{
////						BASS_CHANNELINFO info;
////						BASS_ChannelGetInfo( bass_stream, &info ); // get info
////						if( info.flags & BASS_SAMPLE_LOOP )
////						{
////							info.flags ^= BASS_SAMPLE_LOOP; // toggle the looping flag
////							BASS_ChannelSetFlags( bass_stream, info.flags ); // apply the change
////						}
////					}
////					if( BASS_ErrorGetCode() )
////						bass_error_report( BASS_ErrorGetCode(), "SetLoop" );
////				}
////				else
////				{
////					bass_error_report( BASS_ErrorGetCode(), "Play" );
////				}
////			}
////		}
////	}
////}
////

//
//void	c_sound_bass::init_output()
//{
//
//	if( !b_output_enable_ )
//		{
//		SOUND_PRINT( "enabling sound output" );
//		// setup output - get latency
//		if( !BASS_Init( device_out_, frequency_, BASS_DEVICE_LATENCY, 0, nullptr ) )
//			{
//			SOUND_ERR( "can't initialize device for output" );
//			b_output_enable_ = false;
//			return;
//			}
//
//
//		SOUND_PRINT( "playback" );
//		DWORD		buflen;
//		BASS_INFO	info;
//		INT32		index;
//
//		BASS_GetInfo( &info );
//		nb_output_ = info.speakers;
//
//		// default buffer size = update period + 'minbuf'
//		//	15 ms update period
//		BASS_SetConfig( BASS_CONFIG_UPDATEPERIOD, update_period_ );	//hack beyrouth
//		buflen = BASS_SetConfig( BASS_CONFIG_BUFFER, buffer_ms_ + info.minbuf );
//
//		index = BASS_GetDevice();
//		SOUND_PRINT_STRING( "Device %d :", index );
//		SOUND_PRINT_STRING( "\t%s", BASS_GetDeviceDescription( index ) );
//		SOUND_PRINT_STRING( "\tlatency : %dms", info.latency );
//		SOUND_PRINT_STRING( "\tminbuf : %dms", info.minbuf );
//		SOUND_PRINT_STRING( "\tds version : %d (effects %s)", info.dsver, info.dsver < 8 ? "disabled": "enabled" );
//		b_output_enable_ = true;
//		}
//}
//
//////void	c_sound_bass::init_input( INT32 channel_nb, bool b_stereo )
//////{
//////	if( !b_input_enable_ )
//////		{
//////		//		LPGUID	p_guid;
//////		SOUND_PRINT( "enabling sound input" );
/////
//////		// Select Device
//////		// todonow this should be a settings
//////		if( !BASS_RecordInit( device_in_ ) )
//////			{
//////			SOUND_ERR( "can't initialize device for input" );
//////			b_input_enable_ = false;
//////			return;
//////			}
//////
//////		SOUND_PRINT_STRING( "Device %d : %s", device_in_, BASS_RecordGetDeviceDescription( device_in_ ) );
//////		// select master input = -1
//////		BASS_RecordSetInput( -1, BASS_INPUT_ON );
//////
//////		BASS_RECORDINFO		info;
//////		BASS_RecordGetInfo( &info );
//////		nb_input_ = info.inputs;
//////
//////		SOUND_PRINT_STRING( "Driver : %s", info.driver );
//////		SOUND_PRINT_STRING( "Nb Of Inputs %d", info.inputs );
//////
//////		print_wave_mode( info.formats );
//////
//////		// start recording @ 44100hz 16-bit stereo
//////		//		if( !( bass_record_chan = BASS_RecordStart( SOUND_SAMPLE_PER_SEC, channel_nb_ui, 0, &Bass_RecordingCallback, (DWORD)this ) ) )
//////		if( !( bass_record_chan_ = BASS_RecordStart( SOUND_SAMPLE_PER_SEC, channel_nb_ui_, 0, 0, 0 ) ) )
//////		{
//////			//Error("Couldn't start recording");
//////			//free( recbuf );
//////			//recbuf = 0;
//////			b_input_enable_ = false;
//////		}
//////		else
//////		{
//////			b_input_enable_ = true;
////////			reset_fft();
//////		}
//////	}
//////}
//
////void	c_sound_bass::deinit_input( INT32 channel_nb, bool b_stereo )
////{
////	if( b_input_enable_ )
////		{
////		BASS_Free();
////		b_input_enable_ = false;
////		}
////}
////
////void	c_sound_bass::deinit_output()
////{
////	if( b_output_enable_ )
////		{
////		BASS_RecordFree();
////		b_output_enable_ = false;
////		}
////}
////
////void	c_sound_bass::update_input()
////{
////
////	if( bass_record_chan_ )
////	{
////		INT32	length;
////		INT32	pos;
////
////		length = BASS_ChannelGetData( bass_record_chan_, 0, BASS_DATA_AVAILABLE );
////
////		//				printf( "ChannelGetData length : %d\n", length);
////		pos = length - SOUND_BYTE_PER_CAPTURE;
////		// todonow does it work for pos = 0 ?
////		if( pos > 0 )
////		{
////			// Flush buffer to get the last 2048 bytes
////			BASS_ChannelGetData( bass_record_chan_, 0, pos );
////			//					BASS_ChannelSetPosition( bass_record_chan, pos );
////
////			BASS_ChannelGetData( bass_record_chan_, sound_buffer_, SOUND_BYTE_PER_CAPTURE );
////			++capture_count_;
////		}
////		//todonowfranz
////		// move this to sound input bdd, fft should be processed at this level
////		//if( gb_verbose_fft )
////		//{
////		//	if ( ( ftt_capture_count_ & 0x7f ) == 0 )
////		//	{
////		//		sprintf( err_str, "capture %d", ftt_capture_count_ );
////		//		VERBOSE_PRINTF( err_str );
////		//	}
////		//}
////	}
////}
//
