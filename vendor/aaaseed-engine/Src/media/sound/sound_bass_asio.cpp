#include "sound_bass_asio.h"
#include "sound_bass.h"
#include "infrastructure/param/param_declare.h"
#include "snd_input_bass_asio.h"
#include "snd_master.h"

void	c_sound_bass_asio::report_error( INT32 h, C_PCHAR_C str_fn )
{
	snd::ERR_PRINT( "%s() error %s : %s.", str_fn, c_sound_bass::get_error_str_short( h ), c_sound_bass::get_error_str( h ) );
}

void	c_sound_bass_asio::report_error( C_PCHAR_C str_fn )
{
	report_error( c_sound_bass_asio::get_error(), str_fn );
}

bool	c_sound_bass_asio::open_lib()
{
	return wrap_bassasio_Init() == NO_ERROR;
}

void	c_sound_bass_asio::close_lib()
{
	wrap_bassasio_Term();
}

FACTORY_CREATE_V1( c_sound_bass_asio, sound_bass_asio, sound Input Bass ASIO, sound_bass_asio );

namespace n_sound_bass_asio
{
	CONSTEXPR INT32	BASE_PARAM_NB		= 5;
	CONSTEXPR INT32	INPUT_PARAM_NB		= 5;
	CONSTEXPR INT32	OUTPUT_PARAM_NB		= 5;
	CONSTEXPR INT32	GROUP_NB			= 2;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	INPUT_PARAM_NB
									+	OUTPUT_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_ON(		Active				)
		PARAM_DEF_INT32(		channel_nb_asked,	1,18,	1,64	)
		PARAM_DEF_REAL_LOCKED(	rate				)
		PARAM_DEF_INT32_LOCKED(	latency_in			)
		PARAM_DEF_INT32_LOCKED(	latency_out			)

		PARAM_DEF_GROUP( Input, INPUT_PARAM_NB )
			PARAM_DEF_INT32_LOCKED(		in_nb				)
			PARAM_DEF_INT32_POS(		in_device,			1,0 )
			PARAM_DEF_INT32_POS(		in_channel_nb,		0,2 )
			PARAM_DEF_SYMBO_PSTR_ZERO(	in_sample_rate,		snd::sample_rate_str )
			PARAM_DEF_SYMBO_PSTR_ZERO(	in_bits,			snd::sample_bits_str )
		PARAM_DEF_GROUP( Output, OUTPUT_PARAM_NB )
			PARAM_DEF_INT32_LOCKED(		out_nb				)
			PARAM_DEF_INT32_POS(		out_device,			1,0 )
			PARAM_DEF_INT32_POS(		out_channel_nb,		0,2 )
			PARAM_DEF_SYMBO_PSTR_ZERO(	out_sample_rate,	snd::sample_rate_str )
			PARAM_DEF_SYMBO_PSTR_ZERO(	out_bits,			snd::sample_bits_str )
	};
};

void	c_sound_bass_asio::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, get_pt_active()		);
	param_set_pt( h, _channel_nb_asked_def	);                                                      
	param_set_pt( h, _rate					);
	param_set_pt( h, _latency_out			);
	param_set_pt( h, _latency_out			);

	++h;
		param_set_pt( h, _device_in_count		);
		param_set_pt( h, _device_in				);
		param_set_pt( h, _nb_channel_in			);
		param_set_pt( h, _s_sample_rate_in_ui	);
		param_set_pt( h, _s_sample_bits_in_ui	);
	++h;
		param_set_pt( h, _device_out_count		);
		param_set_pt( h, _device_out			);
		param_set_pt( h, _nb_channel_out		);
		param_set_pt( h, _s_sample_rate_out_ui	);
		param_set_pt( h, _s_sample_bits_out_ui	);

	err_param_init_pt( h );
}


INT32	c_sound_bass_asio::get_error()
{
	return BASS_ASIO_ErrorGetCode();
}

C_PCHAR_C	c_sound_bass_asio::get_error_str()
{
	return c_sound_bass::get_error_str( get_error() );
}

C_PCHAR_C	c_sound_bass_asio::get_format_str( DWORD format )
{
	C_PCHAR str;
	switch( format )
	{
	case BASS_ASIO_FORMAT_16BIT:	str = "16 bit integer";			break;
	case BASS_ASIO_FORMAT_24BIT:	str = "24 bit integer";			break;
	case BASS_ASIO_FORMAT_32BIT:	str = "32 bit integer";			break;
	case BASS_ASIO_FORMAT_FLOAT:	str = "32 bit floating point";	break;
	case BASS_ASIO_FORMAT_DSD_LSB:	str = "DSD (LSB 1st)";			break;
	case BASS_ASIO_FORMAT_DSD_MSB:	str = "DSD (MSB 1st)";			break;
	default :						str = "Unknown";				break;
	}
	return str;
}

void  c_sound_bass_asio::print_channel_info( bool b_input, INT32 channel )
{
	BOOL B_input = b_input ? TRUE : FALSE;
	BASS_ASIO_CHANNELINFO info_chan;
	BASS_ASIO_ChannelGetInfo( B_input, channel, &info_chan );
	snd::PRINT( "%s Channel %d :", b_input ? "Input" : "Output", channel );
	snd::PRINT( "        Name : %s", info_chan.name );
	snd::PRINT( "        Group : %d", info_chan.group );
	snd::PRINT( "        Format : %s", get_format_str(info_chan.format) );

	DOUBLE rate_chan = BASS_ASIO_ChannelGetRate( B_input, channel );
	if( rate_chan==-1 )
		snd::ERR_PRINT( "ChannelGetRate on %s %d Error %s", b_input?"In":"Out", channel, get_error_str() );
	else
	{
		if( rate_chan==0 )
		{
			rate_chan = BASS_ASIO_GetRate();
			if( rate_chan==-1 )
				snd::ERR_PRINT( "GetRate Error %s", get_error_str() );

		}
		if( rate_chan >= 0 )
			snd::PRINT( "    - Rate : %g", rate_chan );
	}
}

double  c_sound_bass_asio::get_rate()
{
	double rate = BASS_ASIO_GetRate();	// get the sample rate
	snd::PRINT( "Sample Rate : %.0f Hz", rate );
//	snd::PRINT( "Sample Format : 16 bit integer" );
	return rate;
}

INT32  c_sound_bass_asio::get_latency( bool b_input )
{
	return BASS_ASIO_GetLatency( b_input ? TRUE : FALSE );
}

void  c_sound_bass_asio::print_rate_latency( bool b_input, REAL rate, INT32 latency )
{
	snd::PRINT( "%sput Latency  : %d samples, %g ms", b_input ? "Ïn" : "Out", latency, latency * 1000 / rate );
}


void  c_sound_bass_asio::print_info( BASS_ASIO_INFO* info )
{
	snd::PRINT( "Name : %s",						info->name		);
	snd::PRINT( "Version : %d",						info->version	);
	snd::PRINT( "Nb Of Inputs : %d",				info->inputs	);
	snd::PRINT( "Nb Of Outputs : %d",				info->outputs	);
	snd::PRINT( "Buffer Min (samples) : %d",		info->bufmin	);
	snd::PRINT( "Buffer Max (samples) : %d",		info->bufmax	);
	snd::PRINT( "Default buffer (samples) : %d",	info->bufpref	);
	snd::PRINT( "Buffer Length Granularity : %d",	info->bufgran	);
}

/*
void	c_sound_bass_asio::error_report( INT32 h, CHAR* str_fn )
{
	ERR_PRINT_STRING( "c_sound_bass_asio %s error %s.", str_fn, get_error_str( h ) );
}
*/

CONSTRUCTOR_CREATE(c_sound_bass_asio)
	,_rate			(	0	)
	,_latency_in	(	0	)
	,_latency_out	(	0	)
{					
	param_init_with( n_sound_bass_asio::param, n_sound_bass_asio::PARAM_NB_MAX );

	open_driver();
}

c_sound_bass_asio::~c_sound_bass_asio()
{
	close();	//not call by c_sound destructor
}

INT32	c_sound_bass_asio::enumerate_device_out()
{
	INT32 count = 0;
	snd::PRINT( "enumeration" );
	BASS_ASIO_DEVICEINFO 	info;
	for( INT32 i = 0; BASS_ASIO_GetDeviceInfo(i, &info ); ++i)
	{
		snd::print_device( i, info.name, info.driver );
		++count; // count it
	}
	snd::PRINT( "Device(s) detected: %d", count );
//	_device_input_count = count;
//	_devices_channel_nb = _device_input_count * 4 ; //hack we have to open the device one by one
	return count;
}

bool	c_sound_bass_asio::open_driver()
{
	DWORD version = BASS_ASIO_GetVersion();
	snd::PRINT( "BassAsio version %d.%d.%d.%d loaded", version>>24, (version>>16)&0xff, (version>>8)&0xff, version&0xff );
	if( HIWORD( version ) != BASSASIOVERSION )
	{
		snd::ERR_PRINT( "BassAsio version %d.%d was not loaded", BASSASIOVERSION>>8 & 0xff, BASSASIOVERSION & 0xff );
		return false;
	}
	return true;
}

//todo
/*
void	c_sound_bass_asio::init_output()
{
//	_b_output_enable[ 0 ] = false;
}
*/

void	c_sound_bass_asio::close()
{
	//we need to call it first
	c_sound::close();
//todosnd
//	if( !_b_input_enable )
	{
		BASS_ASIO_Stop();

		// release library
		BASS_ASIO_Free();
	}
}

//void	c_sound_bass_asio::update_input()
//{
//
//}

bool	c_sound_bass_asio::open_control_panel()
{
	BOOL ret = BASS_ASIO_ControlPanel();
	if( !ret )
		report_error( "BASS_ASIO_ControlPanel" );
	return ret==TRUE;
}

c_sound_input*	c_sound_bass_asio::create_input_new()
{
	auto pt = new c_sound_input_bass_asio;
	pt->_sound = this;
	return pt;
}
