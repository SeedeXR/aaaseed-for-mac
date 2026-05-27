#include "sound_capture_directsound.h"
#include "sound_directsound.h"


//global level function for retrieving and dumping the sound capture buffer
DWORD CALLBACK HandleNotifications( LPVOID lpvoid )
{
	DWORD			hr = DS_OK;
	DWORD			hRet = 0;
	UINT32			index;
	LPVOID			lpvAudioPtr1;
	DWORD			dwAudioBytes1; 
	LPVOID			lpvAudioPtr2; 
	DWORD			dwAudioBytes2;
	DWORD			capture_pos;
	DWORD			read_pos;

	c_sound_capture_ds*	snd_input = (c_sound_capture_ds*)lpvoid;

	if( !snd_input )
	{
		ERR_PRINT_STRING( "DirectSound HandleNotifications() with NULL snd_input");
		return FALSE;
	}

//	sprintf( err_str, "DirectSound HandleNotifications() with snd_input : %X", snd_input );
	SOUND_PRINT_STRING( "DirectSound HandleNotifications() with snd_input : %X", snd_input );
	while( snd_input->b_sound_capture_thread_on_ )
	{
		hRet = WaitForMultipleObjects( c_sound_capture_ds::SOUND_BUFFER_NB + 1, snd_input->h_NotifyEvent_, FALSE, 1000 );

		index = hRet - WAIT_OBJECT_0;
		if ( index < 0 || index > c_sound_capture_ds::SOUND_BUFFER_NB )
		{
			if( index == WAIT_TIMEOUT )
			{
				ERR_PRINT_STRING( "DirectSound snd_input TimeOut" );
			}
			else if( index == c_sound_capture_ds::SOUND_BUFFER_NB )
			{
				ERR_PRINT_STRING( "DirectSound Maa: last strange Object notified" );
			}
			else
				ERR_PRINT_STRING( "DirectSound snd_input Notify Error" );
			}
		else if( index == c_sound_capture_ds::SOUND_BUFFER_NB )
			{
			break;
			}
		else
			{
			// Deal with the event that got signaled.
			//		fprintf(outerr,"Notify #%d ",index);
			if( !snd_input->b_buffer_read_ )
				{
				snd_input->lpDSBuffer_->GetCurrentPosition( &capture_pos, &read_pos );

				if(	snd_input->lpDSBuffer_->Lock( index * snd_input->byte_per_capture_, snd_input->byte_per_capture_,
					&lpvAudioPtr1, &dwAudioBytes1, &lpvAudioPtr2, &dwAudioBytes2, NULL ) == DS_OK )
					{
					if( ( dwAudioBytes1 + dwAudioBytes2 ) <= snd_input->byte_per_capture_ )
						{
						snd_input->capture_count_++;
						//if( b_verbose_fft )
						//{
						//	if ( ( snd_input->capture_count_ & 0x7f ) == 0 )
						//	{
						//		sprintf( err_str, "capture %d", snd_input->capture_count_ );
						//		VERBOSE_PRINTF( err_str );
						//	}
						//}

						MEMCPY( (void*)snd_input->sound_buffer_, lpvAudioPtr1, dwAudioBytes1);
						if( dwAudioBytes2 )
							//todo try to use MEMCPY
							MEMCPY( (void*)( snd_input->sound_buffer_ + dwAudioBytes1 ), lpvAudioPtr2, dwAudioBytes2 );
						//					fprintf(outerr," P1:%X size1:%d P2:%x size2:%d ",
						//							lpvAudioPtr1,  
						//							dwAudioBytes1,  
						//							lpvAudioPtr2,  
						//							dwAudioBytes2);
						}
					else
						ERR_PRINT_STRING( "DirectSound !Illegal buffer size!" );

					snd_input->lpDSBuffer_->Unlock( lpvAudioPtr1, dwAudioBytes1, lpvAudioPtr2, dwAudioBytes2 );

					//if( snd_input->u8_fft_done_)
					//{
					//	//if( b_verbose_fft )
					//	//	VERBOSE_PRINTF("skip fft");
					//}
					//else
					//{
					//}
					}
				else
					{
					ERR_PRINT_STRING( "DirectSound !Unable to lock!");
					}
				}
			}

		} // while
	return TRUE;
} 

c_sound_capture_ds::c_sound_capture_ds()
{
	init();
//	set_name( "Sound_Input_DirectSound" );
}

void	c_sound_capture_ds::init()
{
	b_enable_ = FALSE;

	bit_per_sample_ = SOUND_BIT_PER_SAMPLE;
	sample_rate_ = SOUND_SAMPLE_PER_SEC;
	capture_size_bit_ = SOUND_CAPTURE_SIZE_IN_BIT;

	id_ = 0;
	device_ = 0;			// Device we want to use for input

	channel_nb_ = 0;		// first input
	nb_input_ = 2;			// stereo input

	buffer_size_ = 0;
	capture_count_ = 0;
	sound_buffer_ = NULL;	// the sound

	h_notify_thread_ = (HANDLE)NULL;
	for( INT32 i = 0; i < SOUND_BUFFER_NB; i++ )
		h_NotifyEvent_[i] = NULL;
	p_DSCapture_ = NULL;
	lpDSBuffer_ = NULL;
	lpDsNotify_ = NULL;
}

c_sound_capture_ds::~c_sound_capture_ds()
{
	deinit();
}

void	c_sound_capture_ds::deinit()
{
	disable();
}

// Create a new input using parameters, return TRUE or FASLE
BOOL	c_sound_capture_ds::enable( INT32 id, INT32 device, INT32 channel, INT32 nb_input, INT32 sample_rate, INT32 sample_bits, INT32 capture_size_bits_ui_ )
{
	id_ = id;
	device_ = device;
	channel_first_ = channel;
	channel_nb_ = nb_input;
	sample_rate_ = sample_rate;
	bit_per_sample_ = sample_bits;
	capture_size_bit_ = capture_size_bits_ui_;

	// calculate all the size for sample per sec, byte, etc...
	sample_size();

	// init the sound buffer
	sound_buffer_ = new UINT8[byte_per_capture_];

	if( sound_buffer_ )
		if( enable_low() )
			{
			buffer_size_ = byte_per_capture_;
			b_enable_ = TRUE;
			return TRUE;
			}
	// could not create or enable the sound input
	return FALSE;

}

BOOL	c_sound_capture_ds::enable_low()
{
	//
	//	directsound capture section
	//
	DSCCAPS			st_capture_caps;
	DSCBUFFERDESC	st_buffer_desc;
	UINT32			u32_return;
	UINT32			i;


	if( !b_enable_ )
		{
		LPGUID	p_guid;
		SOUND_PRINT_STRING( "DirectSound enabling sound input" );

		if( device_ >= 0 )
			{
			if( sound )
				p_guid = sound->get_guid( device_ );	//hack we should also check we don't open twice the same device
			if( p_guid == NULL )
				{
				ERR_PRINT_STRING( "DirectSound :can't enable sound input. the device %d doesn't exist", device_ );
				return FALSE;
				}
			//else
			//	// Get Name of the device
			//	device_name_ = sound->get_name( device_ );
			}
		else
			p_guid =  NULL;


		//the direct sound object
		//todo	enumerate the different capture objects
		u32_return = DirectSoundCaptureCreate( p_guid, &p_DSCapture_, NULL );
		if( u32_return != DS_OK )
			{
			ds_err_show( u32_return, "c_sound_directsound::init_input() DirectSoundCaptureCreate(), Can't create DirectSound Capture Object" );
			return FALSE;
			}

		//the capture buffer
		st_capture_caps.dwSize = sizeof( DSCCAPS );
		u32_return = p_DSCapture_->GetCaps( &st_capture_caps );
		if( u32_return != DS_OK )
			{
			ds_err_show( u32_return, "c_sound_directsound::init_input() GetCaps()" );
			}
		else
			{
//			sprintf( err_str, "sound capture support these modes with %d channel :", INT32( st_capture_caps.dwChannels ) );
			SOUND_PRINT_STRING( "sound capture support these modes with %d channel :", INT32( st_capture_caps.dwChannels ) );
			if( st_capture_caps.dwFormats & WAVE_FORMAT_1M08  ) SOUND_PRINT_STRING( "\t11.025 kHz, mono,    8 bit" );
			if( st_capture_caps.dwFormats & WAVE_FORMAT_1M16  ) SOUND_PRINT_STRING( "\t11.025 kHz, mono,   16 bit" );
			if( st_capture_caps.dwFormats & WAVE_FORMAT_1S08  ) SOUND_PRINT_STRING( "\t11.025 kHz, stereo,  8 bit" );
			if( st_capture_caps.dwFormats & WAVE_FORMAT_1S16  ) SOUND_PRINT_STRING( "\t11.025 kHz, stereo, 16 bit" );

			if( st_capture_caps.dwFormats & WAVE_FORMAT_2M08  ) SOUND_PRINT_STRING( "\t22.05  kHz, mono,    8 bit" );
			if( st_capture_caps.dwFormats & WAVE_FORMAT_2M16  ) SOUND_PRINT_STRING( "\t22.05  kHz, mono,   16 bit" );
			if( st_capture_caps.dwFormats & WAVE_FORMAT_2S08  ) SOUND_PRINT_STRING( "\t22.05  kHz, stereo,  8 bit" );
			if( st_capture_caps.dwFormats & WAVE_FORMAT_2S16  ) SOUND_PRINT_STRING( "\t22.05  kHz, stereo, 16 bit" );

			if( st_capture_caps.dwFormats & WAVE_FORMAT_44M08 ) SOUND_PRINT_STRING( "\t44.1   kHz, mono,    8 bit" );
			if( st_capture_caps.dwFormats & WAVE_FORMAT_44S08 ) SOUND_PRINT_STRING( "\t44.1   kHz, stereo,  8 bit" );
			if( st_capture_caps.dwFormats & WAVE_FORMAT_44M16 ) SOUND_PRINT_STRING( "\t44.1   kHz, mono,   16 bit" );
			if( st_capture_caps.dwFormats & WAVE_FORMAT_44S16 ) SOUND_PRINT_STRING( "\t44.1   kHz, stereo, 16 bit" );

			if( st_capture_caps.dwFormats & WAVE_FORMAT_48M08 ) SOUND_PRINT_STRING( "\t48     kHz, mono,    8 bit" );
			if( st_capture_caps.dwFormats & WAVE_FORMAT_48S08 ) SOUND_PRINT_STRING( "\t48     kHz, stereo,  8 bit" );
			if( st_capture_caps.dwFormats & WAVE_FORMAT_48M16 ) SOUND_PRINT_STRING( "\t48     kHz, mono,   16 bit" );
			if( st_capture_caps.dwFormats & WAVE_FORMAT_48S16 ) SOUND_PRINT_STRING( "\t48     kHz, stereo, 16 bit" );

			if( st_capture_caps.dwFormats & WAVE_FORMAT_96M08 ) SOUND_PRINT_STRING( "\t96     kHz, mono,    8 bit" );
			if( st_capture_caps.dwFormats & WAVE_FORMAT_96S08 ) SOUND_PRINT_STRING( "\t96     kHz, stereo,  8 bit" );
			if( st_capture_caps.dwFormats & WAVE_FORMAT_96M16 ) SOUND_PRINT_STRING( "\t96     kHz, mono,   16 bit" );
			if( st_capture_caps.dwFormats & WAVE_FORMAT_96S16 ) SOUND_PRINT_STRING( "\t96     kHz, stereo, 16 bit" );	
			}

//		sample_ui_convert();

		wfx_.wFormatTag = WAVE_FORMAT_PCM;
		wfx_.nChannels = channel_nb_;
		wfx_.nSamplesPerSec = sample_rate_; //SOUND_SAMPLE_PER_SEC;
		wfx_.nAvgBytesPerSec = byte_per_sec_mono_ * channel_nb_;
		wfx_.nBlockAlign = byte_per_sample_ * channel_nb_;
		wfx_.wBitsPerSample = bit_per_sample_; //SOUND_BIT_PER_SAMPLE;
		wfx_.cbSize = 0;

		st_buffer_desc.dwSize = sizeof( DSCBUFFERDESC );
		st_buffer_desc.dwFlags = NULL;
		st_buffer_desc.dwBufferBytes = byte_per_capture_ * SOUND_BUFFER_NB;
		st_buffer_desc.dwReserved = NULL;
		st_buffer_desc.lpwfxFormat = &wfx_;
		st_buffer_desc.dwFXCount = 0;
		st_buffer_desc.lpDSCFXDesc = NULL;
		u32_return = p_DSCapture_->CreateCaptureBuffer( &st_buffer_desc, &lpDSBuffer_, NULL );
		if( u32_return != DS_OK )
			{
			ds_err_show( u32_return, "c_sound_directsound::init_input() CreateCaptureBuffer()");
			b_enable_ = FALSE;
			return FALSE;
			}

		//		channel_nb_ = channel_nb_ui_;
		//the notification interface
		u32_return = lpDSBuffer_->QueryInterface( IID_IDirectSoundNotify, (void**)&lpDsNotify_ );
		if( u32_return != DS_OK )
			{
			ds_err_show( u32_return, "c_sound_directsound::init_input() QueryInterface(), DirectSound CaptureBuffer interface query failed");
			return FALSE;
			}

		// create the events
		for( i = 0; i < SOUND_BUFFER_NB; i++ )
			{
			if( ( h_NotifyEvent_[i] = CreateEvent( NULL, FALSE, FALSE, NULL ) ) == NULL )
				{
				ERR_PRINT_STRING("event creation failed");
				return FALSE;
				}
			rgdsbpn[i].dwOffset = byte_per_capture_ * ( i + 1 ) - 1;
			rgdsbpn[i].hEventNotify = h_NotifyEvent_[i];
			}
		if( ( h_NotifyEvent_[i] = CreateEvent( NULL, FALSE, FALSE, NULL ) ) == NULL )
			{
			ERR_PRINT_STRING( "event creation failed" );
			return FALSE;
			}
		rgdsbpn[i].dwOffset = DSBPN_OFFSETSTOP;
		rgdsbpn[i].hEventNotify = h_NotifyEvent_[i];


		// Now create the thread to wait on the events created.
		//		u8_fft_done_ = 0;
		b_buffer_read_ = FALSE;
		b_sound_capture_thread_on_ = TRUE;

		if ( ( h_notify_thread_ = CreateThread( NULL, 0, (LPTHREAD_START_ROUTINE)HandleNotifications, (LPVOID)this, 0, &dwThreadId_ ) ) == NULL)
			{
			ERR_PRINT_STRING("c_sound_directsound::init_input() thread creation failed");
			return FALSE;
			}

		if( SetThreadPriority( h_notify_thread_, THREAD_PRIORITY_ABOVE_NORMAL ) == 0 )
			ERR_PRINT_STRING("c_sound_directsound::init_input() Can't set thread priority");

		u32_return = lpDsNotify_->SetNotificationPositions( SOUND_BUFFER_NB + 1, rgdsbpn );
		if( u32_return != DS_OK )
			{
			ds_err_show( u32_return, "c_sound_directsound::init_input() SetNotificationPositions()" );
			release_notification_events();
			}

		lpDSBuffer_->Start( DSCBSTART_LOOPING );
		b_enable_ = TRUE;
		return TRUE;
//		reset_fft();
	}
	return FALSE;
}


void	c_sound_capture_ds::disable_low()
{
	UINT32 u32_return;

	//hack ?
	b_sound_capture_thread_on_ = OFF;

	u32_return = DS_OK;
	if( b_enable_ )
	{
		if( p_DSCapture_ != NULL )
			{
			if( lpDSBuffer_ != NULL )
				{
				//kill the wait-for-notify thread
				u32_return = lpDSBuffer_->Stop();
				if( h_notify_thread_ )
					{
					while( b_sound_capture_thread_on_ );
					release_notification_events();
					}
				if( lpDsNotify_ )
					{
					u32_return = lpDsNotify_->Release();
					}
				//	here is the locking call
				u32_return = lpDSBuffer_->Release();
				lpDSBuffer_ = NULL;
				}
			//	here is the second	locking call
			//	if we skip the first one
			p_DSCapture_->Release();
			p_DSCapture_ = NULL;
			}
		b_enable_ = FALSE;
		}
}

//INT32	enable( INT32 id, INT32 device, INT32 channel_first, INT32 channel_nb, INT32 sample_rate, INT32 sampe_bits );
//{
//	id_ = id;
//	device_ = device;
//	channel_first_ = channel_first;
//	channel_nb_ = channel_nb;
//	sample_rate_ = sample_rate;
//	sample_bits_ = sample_bits;
//	enable_low();
//}

BOOL	c_sound_capture_ds::disable( )
{
	disable_low();
	if( b_enable_ )
		return FALSE;
	else
		return TRUE;
}

UINT8*	c_sound_capture_ds::get_sound_buffer( )
{
	return sound_buffer_;
}

void	c_sound_capture_ds::release_notification_events()
{
	lpDsNotify_->SetNotificationPositions( 0, NULL );
	for( INT32 i = 0; i < SOUND_BUFFER_NB + 1; i ++ )
		{
		if( h_NotifyEvent_[i] )
			{
			CloseHandle( h_NotifyEvent_[i] );
			h_NotifyEvent_[i] = (HANDLE)NULL;
			}
		}
}

void	c_sound_capture_ds::update()
{
}
