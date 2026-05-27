#include "snd_input_bass.h"
//#include "sound.h"

FACTORY_CREATE_V1( c_snd_input_bass, snd_input_bass, sound input Bass, snd_input_bass );

static const INT32 SND_INPUT_BASS_PARAM_NB_MAX = 11;

ST_PARAM	snd_input_bass_param[SND_INPUT_BASS_PARAM_NB_MAX] =
{
	{	NULL,	PARAM_BOOL,		"Active",					0, 1,		0, 1,				NULL, NULL },
	{	NULL,	PARAM_REAL,		"value_inactive",			0, 1,		0, 1,				NULL, NULL },
	//hack
	{	NULL,	PARAM_INT32,	"device",					1, 0,		0, PARAM_INFINI,	NULL, NULL },
	{	NULL,	PARAM_INT32,	"channel_in_nb",			0, 2,		0, PARAM_INFINI,	NULL, NULL },
	//hack
	{	NULL,	PARAM_BOOL,		"flip_stereo",				1, 0,		0, 1,				NULL, NULL },

	{	NULL,	PARAM_BOOL,		"enable",					0, 1,		0, 1,				NULL, NULL },
	{	NULL,	PARAM_BOOL,		"Calibration_left",			1, 0,		0, 1,				NULL, NULL },
	{	NULL,	PARAM_BOOL,		"Calibration_right",		1, 0,		0, 1,				NULL, NULL },
	{	NULL,	PARAM_BOOL,		"Calibration_auto",			1, 0,		0, 1,				NULL, NULL },
	{	NULL,	PARAM_REAL,		"Calibration_auto_delay",	.001, 10,	0, PARAM_INFINI,	NULL, NULL },

	{	NULL,	PARAM_REAL,		"test_freq",				1000, 0,	0, 22000,			NULL, NULL },
};

void	c_snd_input::param_init_pt()
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
	param_set_pt( h, freq_test);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_snd_input_bass)
{
#if	USE_FFT_DENPO
	alloc_fft_denpo();
#endif
	channel_nb = 2;
	init();
	set_name("Sound_Input_Bass");
	param_init_with( snd_input_bass_param, SND_INPUT_BASS_PARAM_NB_MAX);
}

void	c_snd_input_bass::init()
{
	INT32	i;
	ftt_capture_count = 0;
	fft_count = 0;

	b_enabled = FALSE;

}

void	c_snd_input_bass::param_init()
{
}

c_snd_input_bass::~c_snd_input_bass()
{
	disable();
}

void	c_snd_input_bass::update_low()
{
	INT32	length, pos;

	if( bass_record_chan )
		{
		length = BASS_ChannelGetData( bass_record_chan, 0, BASS_DATA_AVAILABLE );
		//				DBG_PRINT_STRING( "ChannelGetData lenght : %d", length);
		pos = length - 2048;
		if( pos > 0 )
			{
			// Flush buffer to get the last 2048 bytes
			BASS_ChannelGetData( bass_record_chan, 0, pos );
			//					BASS_ChannelSetPosition( bass_record_chan, pos );

			BASS_ChannelGetData( bass_record_chan, sound_buffer, 2048 );
			ftt_capture_count++;
			}
		if( b_verbose_fft )
			{
			if ( (ftt_capture_count & 0x7f) == 0 )
				{
				sprintf( err_str, "capture %d", ftt_capture_count);
				VERBOSE_PRINTF( err_str);
				}
			}
		}
}

void	c_snd_input_bass::disable()
{
	if( b_enabled )
		{
		BASS_ChannelStop( bass_record_chan );
		// release all BASS stuff
		BASS_RecordFree();
		b_enabled = FALSE;
		//		BASS_Free();
		}
}

BOOL	c_snd_input_bass::enable()
{
	if( !b_enabled)
		{
		SOUND_INPUT_PRINT_STRING( "enabling sound_capture (Bass)" );

		// Select Device
		// todonow this should be a settings
		BASS_RecordInit( device );

		//INT32	count = 0; // the device counter
		//while ( BASS_RecordGetDeviceDescription( count ) ) 
		//	{
		//	sprintf( err_str, "Device %d : %s", count, BASS_RecordGetDeviceDescription( count ) );
		//	SOUND_PRINT_STRING( err_str);
		//	count++;
		//	}
		sprintf( err_str, "Device %d : %s", device, BASS_RecordGetDeviceDescription( device ) );
		SOUND_INPUT_PRINT_STRING( err_str);
		BASS_RecordSetInput( -1, BASS_INPUT_ON );
		BASS_RECORDINFO info;
		BASS_RecordGetInfo( &info );
		sprintf( err_str, "Driver : %s", info.driver);
		SOUND_INPUT_PRINT_STRING( err_str );
		sprintf( err_str, "Nb Of Inputs %d", info.inputs );
		SOUND_INPUT_PRINT_STRING( err_str );
		if( info.formats & WAVE_FORMAT_1M08  ) SOUND_INPUT_PRINT_STRING( "\t11.025 kHz, mono,   8  bit" );
		if( info.formats & WAVE_FORMAT_1M16  ) SOUND_INPUT_PRINT_STRING( "\t11.025 kHz, mono,   16 bit" );
		if( info.formats & WAVE_FORMAT_1S08  ) SOUND_INPUT_PRINT_STRING( "\t11.025 kHz, stereo, 8  bit" );
		if( info.formats & WAVE_FORMAT_1S16  ) SOUND_INPUT_PRINT_STRING( "\t11.025 kHz, stereo, 16 bit" );

		if( info.formats & WAVE_FORMAT_2M08  ) SOUND_INPUT_PRINT_STRING( "\t22.05  kHz, mono,   8  bit" );
		if( info.formats & WAVE_FORMAT_2M16  ) SOUND_INPUT_PRINT_STRING( "\t22.05  kHz, mono,   16 bit" );
		if( info.formats & WAVE_FORMAT_2S08  ) SOUND_INPUT_PRINT_STRING( "\t22.05  kHz, stereo, 8  bit" );
		if( info.formats & WAVE_FORMAT_2S16  ) SOUND_INPUT_PRINT_STRING( "\t22.05  kHz, stereo, 16 bit" );

		if( info.formats & WAVE_FORMAT_44M08 ) SOUND_INPUT_PRINT_STRING( "\t44.1   kHz, mono,   8  bit" );
		if( info.formats & WAVE_FORMAT_44S08 ) SOUND_INPUT_PRINT_STRING( "\t44.1   kHz, stereo, 8  bit" );
		if( info.formats & WAVE_FORMAT_44M16 ) SOUND_INPUT_PRINT_STRING( "\t44.1   kHz, mono,   16 bit" );
		if( info.formats & WAVE_FORMAT_44S16 ) SOUND_INPUT_PRINT_STRING( "\t44.1   kHz, stereo, 16 bit" );

		if( info.formats & WAVE_FORMAT_48M08 ) SOUND_INPUT_PRINT_STRING( "\t48     kHz, mono,   8  bit" );
		if( info.formats & WAVE_FORMAT_48S08 ) SOUND_INPUT_PRINT_STRING( "\t48     kHz, stereo, 8  bit" );
		if( info.formats & WAVE_FORMAT_48M16 ) SOUND_INPUT_PRINT_STRING( "\t48     kHz, mono,   16 bit" );
		if( info.formats & WAVE_FORMAT_48S16 ) SOUND_INPUT_PRINT_STRING( "\t48     kHz, stereo, 16 bit" );

		if( info.formats & WAVE_FORMAT_96M08 ) SOUND_INPUT_PRINT_STRING( "\t96     kHz, mono,   8  bit" );
		if( info.formats & WAVE_FORMAT_96S08 ) SOUND_INPUT_PRINT_STRING( "\t96     kHz, stereo, 8  bit" );
		if( info.formats & WAVE_FORMAT_96M16 ) SOUND_INPUT_PRINT_STRING( "\t96     kHz, mono,   16 bit" );
		if( info.formats & WAVE_FORMAT_96S16 ) SOUND_INPUT_PRINT_STRING( "\t96     kHz, stereo, 16 bit" );	

		wfx.wFormatTag = WAVE_FORMAT_PCM;
		wfx.nChannels = channel_nb_ui;

		wfx.nSamplesPerSec = SOUND_SAMPLE_PER_SEC;
		wfx.nAvgBytesPerSec = SOUND_BYTE_PER_SECOND_MONO * channel_nb_ui;
		wfx.nBlockAlign = SOUND_BYTE_PER_SAMPLE * channel_nb_ui;
		wfx.wBitsPerSample = SOUND_BIT_PER_SAMPLE;
		wfx.cbSize = 0;

		// start recording @ 44100hz 16-bit stereo
		if( !( bass_record_chan = BASS_RecordStart( SOUND_SAMPLE_PER_SEC, channel_nb_ui, 0, 0, 0 ) ) )
			{
			//Error("Couldn't start recording");
			//free( recbuf );
			//recbuf = 0;
			b_enabled = FALSE;
			}
		else
			{
			b_enabled = TRUE;
			reset_fft();
			}
		}
	return b_enabled;

}
