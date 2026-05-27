#include "snd_input_bass.h"
#include "sound_bass.h"
#include "snd_master.h"



c_sound_input_bass::c_sound_input_bass()
{
	init();
}

void	c_sound_input_bass::init()
{
	c_sound_input::init();
}

c_sound_input_bass::~c_sound_input_bass()
{
	deinit();
}

void	c_sound_input_bass::deinit()
{
	disable();
}

bool	c_sound_input_bass::enable_low()
{
	// Select Device
	//todonow this should be a settings
	if( !dll_bass.BASS_RecordInit( get_device() ) )
	{
		int err = dll_bass.BASS_ErrorGetCode();
		if( err == BASS_ERROR_ALREADY )
		{
			snd::ERR_PRINT( "device already initialized, code should be changed here" );
		}
		else
		{
			snd::ERR_PRINT( "can't initialize device for input %d with error : %s.", get_device(), c_sound_bass::get_error_str( err ) );
			return false;
		}
	}

	{
		BASS_DEVICEINFO	info;
		dll_bass.BASS_RecordGetDeviceInfo( get_device() , &info );
		get_device_name()->set( info.name );
	}

	//int	n;
	C_PCHAR	name;
	for( INT32 n = 0; name = dll_bass.BASS_RecordGetInputName( n ); ++n )
	{
		float	vol;
		INT32	s = dll_bass.BASS_RecordGetInput( n, &vol );
		C_PCHAR_C	str_on = (s & BASS_INPUT_OFF) ? "OFF" : "ON";
		snd::PRINT( "  Inputs %d : %s [%s : %g]", n, name, str_on, (double)vol );
		_o_type.set( c_sound_bass::get_input_type( s ) );
		snd::PRINT( "              %s", _o_type.get() );
		//	printf("%s [%s : %g]\n", name, s&BASS_INPUT_OFF?"OFF":"ON", vol );
	}
	//SOUND_PRINT_STRING( "Device %d : %s", device_, BASS_RecordGetDeviceDescription( device_ ) );
	// select master input = -1
	dll_bass.BASS_RecordSetInput( -1, BASS_INPUT_ON, 0.75 );

	BASS_RECORDINFO		info;
	dll_bass.BASS_RecordGetInfo( &info );
//	_nb_input = info.inputs;

	BASS_DEVICEINFO	device_info;
	dll_bass.BASS_RecordGetDeviceInfo( get_device(), &device_info );
	snd::PRINT( "Name : %s",		device_info.name );
	snd::PRINT( "Driver : %s",		device_info.driver );
	snd::PRINT( "Nb Of Inputs %d",	info.inputs );

	//snd::print_wave_modes( info.formats );

	// start recording @ 44100hz 16-bit stereo
	//		if( !( bass_record_chan = BASS_RecordStart( SOUND_SAMPLE_PER_SEC, channel_nb_ui, 0, &Bass_RecordingCallback, (DWORD)this ) ) )
	bool b;
	if( !( _bass_record_chan = dll_bass.BASS_RecordStart( snd::SAMPLE_PER_SEC_DEF, get_input_nb(), 0, nullptr, nullptr ) ) )
	{
		ERR_PRINT_STRING( "Sound BASS, Couldn't start recording.");
		//free( recbuf );
		//recbuf = 0;
		b = false;
	}
	else
		b = true;

	//if( !b_enable_ )
	//{
	//	SOUND_PRINT_STRING( "DirectSound enabling sound input" );

	//	wfx_.wFormatTag = WAVE_FORMAT_PCM;
	//	wfx_.nChannels = channel_nb_;
	//	wfx_.nSamplesPerSec = sample_rate_; //SOUND_SAMPLE_PER_SEC;
	//	wfx_.nAvgBytesPerSec = byte_per_sec_mono_ * channel_nb_;
	//	wfx_.nBlockAlign = byte_per_sample_ * channel_nb_;
	//	wfx_.wBitsPerSample = bit_per_sample_; //SOUND_BIT_PER_SAMPLE;
	//	wfx_.cbSize = 0;

	//	b_enable_ = true;
	//	return true;
	//}
	return b;
}

bool	c_sound_input_bass::disable_low()
{
	if( dll_bass.BASS_ChannelStop( _bass_record_chan ) )
		return true;
	snd::ERR_PRINT( "can't stop input" );
	return false;
}

CONST	INT32	SOUND_TMP_BUFFER = 	192 * 1024;
static	UINT8	sound_tmp_buffer[ SOUND_TMP_BUFFER ];

void	c_sound_input_bass::update()
{
	if( _bass_record_chan )
	{
		INT32 len = dll_bass.BASS_ChannelGetData( _bass_record_chan, 0, BASS_DATA_AVAILABLE ); //BASS_DATA_FFT2048
		if( len <= 0 )
		{
			if( len==-1 )
				c_sound_bass::report_error( dll_bass.BASS_ErrorGetCode(), "BASS_ChannelGetData() in c_sound_input_bass::update()" );
		}
		else
		{
			//				DBG_PRINT_STRING( "ChannelGetData length : %d", length);
			/*	was
			INT32	pos = length - _buffer_size;	//hack is buffer_size_ the best thing
			if( pos > 0 )
			{
				// Flush buffer to get the last buffer_size_ bytes
				BASS_ChannelGetData( _bass_record_chan, 0, pos );
				//	BASS_ChannelSetPosition( bass_record_chan, pos );

				BASS_ChannelGetData( _bass_record_chan, _sound_buffer, _buffer_size );
			}
			*/
			INT32	pos = len - SOUND_TMP_BUFFER;	//hack is buffer_size_ the best thing
			if( pos > 0 )
			{
				// Flush buffer to get the last buffer_size_ bytes
				dll_bass.BASS_ChannelGetData( _bass_record_chan, 0, pos );
				len -= pos;
			}
			dll_bass.BASS_ChannelGetData( _bass_record_chan, sound_tmp_buffer, len );
			get_capture_stream().put( sound_tmp_buffer, len );
			//if( gb_verbose_fft_ )
			//	{
			//	if ( ( ftt_capture_count_ & 0x7f ) == 0 )
			//		{
			//		sprintf( err_str_, "capture %d", ftt_capture_count );
			//		VERBOSE_PRINTF( err_str_ );
			//		}
			//	}
		}
	}

//	DBG_HEAP_IS_CORRUPT();
}


//void	c_sound_input_bass::init()
//{
//	//INT32	i;
//	//ftt_capture_count_ = 0;
//	//fft_count_ = 0;
//
//	//b_enabled_ = false;
//	b_enable_ = false;
//
//	bit_per_sample_ = SOUND_BIT_PER_SAMPLE;
//	sample_rate_ = SOUND_SAMPLE_PER_SEC;
//	capture_size_bit_ = SOUND_CAPTURE_SIZE_IN_BIT;
//
//	id_ = 0;
//	device_ = 0;			// Device we want to use for input
//
//	channel_nb_ = 0;		// first input-
//	nb_input_ = 2;			// stereo input
//
//	capture_count_ = 0;
//	sound_buffer_ = nullptr;	// the sound
//
//}
//
//
//void	c_sound_input_bass::update_low()
//{
//	INT32	length, pos;
//
//	//if( bass_record_chan_ )
//	//	{
//	//	length = BASS_ChannelGetData( bass_record_chan_, 0, BASS_DATA_AVAILABLE );
//	//	//				DBG_PRINT_STRING( "ChannelGetData length : %d", length);
//	//	pos = length - 2048;
//	//	if( pos > 0 )
//	//		{
//	//		// Flush buffer to get the last 2048 bytes
//	//		BASS_ChannelGetData( bass_record_chan_, 0, pos );
//	//		//					BASS_ChannelSetPosition( bass_record_chan, pos );
//
//	//		BASS_ChannelGetData( bass_record_chan_, sound_buffer_, 2048 );
//	//		++ftt_capture_count_;
//	//		}
//	//	if( gb_verbose_fft_ )
//	//		{
//	//		if ( ( ftt_capture_count_ & 0x7f ) == 0 )
//	//			{
//	//			sprintf( err_str_, "capture %d", ftt_capture_count );
//	//			VERBOSE_PRINTF( err_str_ );
//	//			}
//	//		}
//	//	}
//}
//
//bool	c_sound_input_bass::disable()
//{
//	bool	b_return;
//
//	b_return = true;
//	if( b_enable_ )
//	{
//		BASS_ChannelStop( bass_record_chan_ );
//		// release all BASS stuff
//		b_return = BASS_RecordFree();
//		b_enable_ = false;
//		//		BASS_Free();
//	}
//	return b_return;
//}
//
//bool	c_sound_input_bass::enable( INT32 id, INT32 device, INT32 channel, INT32 nb_input, INT32 sample_rate, INT32 sample_bits, INT32 capture_size_bits )
//{
//	id_ = id;
//	device_ = device;
//	channel_first_ = channel;
//	channel_nb_ = nb_input;
//	sample_rate_ = sample_rate;
//	bit_per_sample_ = sample_bits;
//	capture_size_bit_ = capture_size_bits;
//
//	// calculate all the size for sample per sec, byte, etc...
//	compute_sample_size();
//
//	// init the sound buffer
//	sound_buffer_ = new UINT8[byte_per_capture_];
//
//	if( sound_buffer_ )
//		if( enable_low() )
//		{
//			buffer_size_ = byte_per_capture_;
//			b_enable_ = true;
//			return true;
//		}
//		// could not create or enable the sound input
//		return false;
//
//}
//
//bool	c_sound_input_bass::enable_low()
//{
//	if( !b_enable_ )
//	{
//		SOUND_PRINT_STRING( "enabling sound_input (Bass)" );
//
//		// Select Device
//		// todonow this should be a settings
//		BASS_RecordInit( device_ );
//
//		//INT32	count = 0; // the device counter
//		//while ( BASS_RecordGetDeviceDescription( count ) ) 
//		//	{
//		//	sprintf( err_str, "Device %d : %s", count, BASS_RecordGetDeviceDescription( count ) );
//		//	SOUND_PRINT_STRING( err_str);
//		//	++count;
//		//	}
//		SOUND_PRINT_STRING( "Device %d : %s", device_, BASS_RecordGetDeviceDescription( device_ ) );
//		BASS_RecordSetInput( -1, BASS_INPUT_ON );
//		BASS_RECORDINFO info;
//		BASS_RecordGetInfo( &info );
//		SOUND_PRINT_STRING( "Driver : %s", info.driver);
//		SOUND_PRINT_STRING( "Nb Of Inputs %d", info.inputs );
//		print_wave_mode( info.formats );
//
//		wfx_.wFormatTag = WAVE_FORMAT_PCM;
//		wfx_.nChannels = channel_nb_;
//
//		wfx_.wFormatTag = WAVE_FORMAT_PCM;
//		wfx_.nChannels = channel_nb_;
//		wfx_.nSamplesPerSec = sample_rate_; //SOUND_SAMPLE_PER_SEC;
//		wfx_.nAvgBytesPerSec = byte_per_sec_mono_ * channel_nb_;
//		wfx_.nBlockAlign = byte_per_sample_ * channel_nb_;
//		wfx_.wBitsPerSample = bit_per_sample_; //SOUND_BIT_PER_SAMPLE;
//		wfx_.cbSize = 0;
//		//wfx.nSamplesPerSec = SOUND_SAMPLE_PER_SEC;
//		//wfx.nAvgBytesPerSec = SOUND_BYTE_PER_SECOND_MONO * channel_nb_;
//		//wfx.nBlockAlign = SOUND_BYTE_PER_SAMPLE * channel_nb_;
//		//wfx.wBitsPerSample = SOUND_BIT_PER_SAMPLE;
//		//wfx.cbSize = 0;
//
//		// start recording @ 44100hz 16-bit stereo
//		if( !( bass_record_chan_ = BASS_RecordStart( SOUND_SAMPLE_PER_SEC, channel_nb_, 0, 0, 0 ) ) )
//		{
//			//Error("Couldn't start recording");
//			//free( recbuf );
//			//recbuf = 0;
//			b_enable_ = false;
//		}
//		else
//		{
//			b_enable_ = true;
////			reset_fft();
//		}
//	}
//	return b_enable_;
//
//}
