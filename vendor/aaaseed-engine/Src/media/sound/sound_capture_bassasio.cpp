
#include "snd_input_bassasio.h"
//#include "sound.h"

FACTORY_CREATE_V1( c_snd_input_bass_asio, snd_input_bass_asio, sound input Bass ASIO, snd_input_bass_asio );

static	const INT32	SND_INPUT_BASS_ASIO_PARAM_NB_MAX = 11;

ST_PARAM	snd_input_bass_asio_param[SND_INPUT_BASS_ASIO_PARAM_NB_MAX] =
{
	{	NULL,	PARAM_BOOL,		"Active",					0., 1.,		0., 1.,				NULL, NULL },
	{	NULL,	PARAM_REAL,		"value_inactive",			0., 1.,		0., 1.,				NULL, NULL },
	//hack
	{	NULL,	PARAM_INT32,	"device",					1, 0,		0., PARAM_INFINI,	NULL, NULL },
	{	NULL,	PARAM_INT32,	"channel_in_nb",			0., 2,		0., PARAM_INFINI,	NULL, NULL },
	//hack
	{	NULL,	PARAM_BOOL,		"flip_stereo",				1., 0.,		0., 1.,				NULL, NULL },

	{	NULL,	PARAM_BOOL,		"enable",					0., 1.,		0., 1.,				NULL, NULL },
	{	NULL,	PARAM_BOOL,		"Calibration_left",			1., 0.,		0., 1.,				NULL, NULL },
	{	NULL,	PARAM_BOOL,		"Calibration_right",		1., 0.,		0., 1.,				NULL, NULL },
	{	NULL,	PARAM_BOOL,		"Calibration_auto",			1., 0.,		0., 1.,				NULL, NULL },
	{	NULL,	PARAM_REAL,		"Calibration_auto_delay",	.001, 10.,	0, PARAM_INFINI,	NULL, NULL },

	//		{	NULL,	PARAM_REAL,	"out_min",		1., 0.,		-PARAM_INFINI, PARAM_INFINI,		NULL, NULL },
	//		{	NULL,	PARAM_REAL,	"out_max",		0., 1.,		-PARAM_INFINI, PARAM_INFINI,		NULL, NULL },

	{	NULL,	PARAM_REAL,		"test_freq",				1000., 0.,	0., 22000.,			NULL, NULL },
};

void	c_snd_input_bass_asio::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, b_active);
	param_set_pt( h, value_inactive);
	param_set_pt( h, device);
	param_set_pt( h, channel_nb_ui);
	param_set_pt( h, b_stereo_flip);
	param_set_pt( h, b_enabled_ui);
	param_set_pt_v2( h, b_calibrate_ui );
	param_set_pt( h, b_calibrate_auto);
	param_set_pt( h, calibrate_auto_delay);
	//	param_set_pt( h, out_min);
	//	param_set_pt( h, out_max);
	param_set_pt( h, freq_test);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_snd_input_bass_asio)
{
#if	USE_FFT_DENPO
	alloc_fft_denpo();
#endif
	channel_nb = 2;
	init();
	set_name("Sound_Input_Bass_ASIO");
	param_init_with( snd_input_bass_asio_param, SND_INPUT_BASS_ASIO_PARAM_NB_MAX);
}

void	c_snd_input_bass_asio::init()
{
	INT32	i;
	ftt_capture_count = 0;
	fft_count = 0;

	b_enabled = FALSE;

}

void	c_snd_input_bass_asio::param_init()
{
}

c_snd_input_bass_asio::~c_snd_input_bass_asio()
{
	//	TerminateThread( h_notify_thread, 0);   
	disable();
}


void CALLBACK bass_asio_proc( BOOL input, DWORD channel, void *buffer, DWORD lenght, DWORD user )
{
	c_snd_input_bass_asio*	snd_input = (c_snd_input_bass_asio*)user;

	if( !snd_input)
	{
		ERR_PRINT_STRING( "bass_asio_proc() with NULL snd_input");
		return;
	}

	if( input )
	{
		INT32 pos;
		pos = lenght - snd_input->get_asio_buf_lenght();

		if( pos >= 0 )
		{
			MEMCPY( (void*)snd_input->sound_buffer, buffer, snd_input->get_asio_buf_lenght() );
		}
	}

}

void	c_snd_input_bass_asio::disable()
{
	if( b_enabled )
	{
		BASS_ASIO_Stop();

		// release library
		BASS_ASIO_Free();
		b_enabled = FALSE;
	}
}

BOOL	c_snd_input_bass_asio::enable()
{
	if( !b_enabled)
		{
		//SOUND_INPUT_PRINT_STRING( "enabling sound_capture (Bass Asio)" );

		//// Select Device
		//// todonow this should be a settings
		//BASS_ASIO_Init( device );


		//sprintf( err_str, "Device %d : %s", device, BASS_ASIO_GetDeviceDescription( device ) );
		//SOUND_INPUT_PRINT_STRING( err_str );

		//BASS_ASIO_INFO info;
		//BASS_ASIO_GetInfo( &info );
		//sprintf( err_str, "Name : %s", info.name );
		//SOUND_INPUT_PRINT_STRING( err_str );
		//sprintf( err_str, "Version : %d", info.version );
		//SOUND_INPUT_PRINT_STRING( err_str );
		//sprintf( err_str, "Nb Of Inputs : %d", info.inputs );
		//SOUND_INPUT_PRINT_STRING( err_str );
		//sprintf( err_str, "Nb Of Outputs : %d", info.outputs );
		//SOUND_INPUT_PRINT_STRING( err_str );
		//sprintf( err_str, "Buffer Min (samples) : %d", info.bufmin );
		//SOUND_INPUT_PRINT_STRING( err_str );
		//sprintf( err_str, "Buffer Max (samples) : %d", info.bufmax );
		//SOUND_INPUT_PRINT_STRING( err_str );
		//sprintf( err_str, "Default buffer (samples) : %d", info.bufpref );
		//SOUND_INPUT_PRINT_STRING( err_str );
		//sprintf( err_str, "Buffer Length Granularity : %d", info.bufgran );
		//SOUND_INPUT_PRINT_STRING( err_str );

		//// Print channel info
		//BASS_ASIO_CHANNELINFO	info_chan;
		//for( INT32 i = 0; i < info.inputs; i++)
		//{
		//	BASS_ASIO_ChannelGetInfo( TRUE, i, &info_chan );
		//	sprintf( err_str, "Input Channel %d :", i );
		//	SOUND_INPUT_PRINT_STRING( err_str );
		//	sprintf( err_str, "    - Name : %s", info_chan.name );
		//	SOUND_INPUT_PRINT_STRING( err_str );
		//	sprintf( err_str, "    - Group : %d", info_chan.group );
		//	SOUND_INPUT_PRINT_STRING( err_str );
		//	switch( info_chan.format )
		//	{
		//	case BASS_ASIO_FORMAT_16BIT :
		//		sprintf( err_str, "    - Format : 16 bit integer" );
		//		break;
		//	case BASS_ASIO_FORMAT_24BIT :
		//		sprintf( err_str, "    - Format : 24 bit integer" );
		//		break;
		//	case BASS_ASIO_FORMAT_32BIT :
		//		sprintf( err_str, "    - Format : 32 bit integer" );
		//		break;
		//	case BASS_ASIO_FORMAT_FLOAT :
		//		sprintf( err_str, "    - Format : 32 bit floating point" );
		//		break;
		//	default :
		//		sprintf( err_str, "    - Format : unknown" );
		//		break;
		//	}
		//	SOUND_INPUT_PRINT_STRING( err_str );
		//	INT32 rate_chan = BASS_ASIO_ChannelGetRate( TRUE, i );
		//	sprintf( err_str, "    - Rate : %g", rate_chan );
		//	SOUND_INPUT_PRINT_STRING( err_str );
		//}

		//for( INT32 i = 0; i < info.outputs; i++)
		//{
		//	BASS_ASIO_ChannelGetInfo( FALSE, i, &info_chan );
		//	sprintf( err_str, "Output Channel %d :", i );
		//	SOUND_INPUT_PRINT_STRING( err_str );
		//	sprintf( err_str, "    - Name : %s", info_chan.name );
		//	SOUND_INPUT_PRINT_STRING( err_str );
		//	sprintf( err_str, "    - Group : %d", info_chan.group );
		//	SOUND_INPUT_PRINT_STRING( err_str );
		//	switch( info_chan.format )
		//	{
		//	case BASS_ASIO_FORMAT_16BIT :
		//		sprintf( err_str, "    - Format : 16 bit integer" );
		//		break;
		//	case BASS_ASIO_FORMAT_24BIT :
		//		sprintf( err_str, "    - Format : 24 bit integer" );
		//		break;
		//	case BASS_ASIO_FORMAT_32BIT :
		//		sprintf( err_str, "    - Format : 32 bit integer" );
		//		break;
		//	case BASS_ASIO_FORMAT_FLOAT :
		//		sprintf( err_str, "    - Format : 32 bit floating point" );
		//		break;
		//	default :
		//		sprintf( err_str, "    - Format : unknown" );
		//		break;
		//	}
		//	SOUND_INPUT_PRINT_STRING( err_str );
		//	sprintf( err_str, "    - Rate : %g", BASS_ASIO_ChannelGetRate( FALSE, i ) );
		//	SOUND_INPUT_PRINT_STRING( err_str );
		//}

		// Set channel to 16 bit
		BASS_ASIO_ChannelSetFormat( TRUE, 0, BASS_ASIO_FORMAT_16BIT );
		BASS_ASIO_ChannelSetFormat( TRUE, 1, BASS_ASIO_FORMAT_16BIT );
		// Set channel rate to 44khz
		BASS_ASIO_SetRate( 44100 );
		BASS_ASIO_ChannelSetRate( TRUE, 0, 44100);
		BASS_ASIO_ChannelSetRate( TRUE, 1, 44100);

		BASS_ASIO_ChannelEnable( TRUE, 0, bass_asio_proc, (DWORD)this );
		BASS_ASIO_ChannelJoin( TRUE, 1, 0 );

		asio_buf_lenght = 512 * 2 * 2;

		if( !( BASS_ASIO_Start( 512 ) ) )
		{
			//Error("Couldn't start recording");
			//free( recbuf );
			//recbuf = 0;
			b_enabled = FALSE;
		}
		else
		{
			double rate;
			rate = BASS_ASIO_GetRate();	// get the sample rate
			sprintf( err_str, "Sample Rate : %.0f Hz", rate );
			SOUND_INPUT_PRINT_STRING( err_str );
			sprintf( err_str, "Sample Format : 16 bit integer" );
			SOUND_INPUT_PRINT_STRING( err_str );
			INT32 latency;
			latency = BASS_ASIO_GetLatency( TRUE );
			sprintf( err_str, "Input Latency  : %d samples, %g ms", latency, latency * 1000 / rate );
			SOUND_INPUT_PRINT_STRING( err_str );
			latency = BASS_ASIO_GetLatency( FALSE );
			sprintf( err_str, "Output Latency : %d samples, %g ms", latency, latency * 1000 / rate );
			SOUND_INPUT_PRINT_STRING( err_str );
			b_enabled = TRUE;
			reset_fft();
		}
	}
	return b_enabled;

}
