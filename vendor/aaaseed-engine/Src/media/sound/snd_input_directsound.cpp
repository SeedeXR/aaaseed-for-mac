#include "snd_input_directsound.h"
#include "sound_directsound.h"
#include "snd_master.h"
#include "spy.h"


//global level function for retrieving and dumping the sound capture buffer
DWORD	CALLBACK	HandleNotifications( LPVOID lpvoid )
{
	DWORD		hr = DS_OK;
	DWORD		hRet = 0;
	UINT32		index;
	LPVOID		lpvAudioPtr1;
	DWORD		dwAudioBytes1; 
	LPVOID		lpvAudioPtr2; 
	DWORD		dwAudioBytes2;
	DWORD		capture_pos;
	DWORD		read_pos;

	c_sound_input_ds*	sound_input = (c_sound_input_ds*)lpvoid;

	if( !sound_input )
	{
		ERR_PRINT_STRING( "DirectSound HandleNotifications() with NULL sound_input");
		return FALSE;
	}

	snd::PRINT( "DirectSound HandleNotifications() with sound_input : %X", sound_input );
	sound_input->_b_notification_handling = true;
	bool*	pb = &(sound_input->_b_notification_ask_stop);
	//this a hack to avoid certain case the _b_handling_notification / _b_sound_capture_thread_on don't work in release
	while( !*pb )
	{
		hRet = WaitForMultipleObjects( c_sound_input_ds::SOUND_BUFFER_NB + 1, sound_input->_h_NotifyEvent, FALSE, 1000 );
		if( *pb )
		{
			break;
		}	
		if( hRet == WAIT_FAILED )
		{
			ERR_PRINT_STRING( "DirectSound WaitForMultipleObjects failed : stopping this sound input" );
			break;
		}

		index = hRet - WAIT_OBJECT_0;
		if ( index < 0 || index > c_sound_input_ds::SOUND_BUFFER_NB )
		{
			if( index == WAIT_TIMEOUT )
				ERR_PRINT_STRING( "DirectSound sound_input TimeOut" );
			else if( index == c_sound_input_ds::SOUND_BUFFER_NB )
				ERR_PRINT_STRING( "DirectSound Maa: last strange Object notified" );
			else 
				ERR_PRINT_STRING( "DirectSound sound_input Notify Error" );
		}
		else if( index == c_sound_input_ds::SOUND_BUFFER_NB )
			break;
		else
		{
			// Deal with the event that got signaled.
			//		fprintf(outerr,"Notify #%d ",index);
			if( !sound_input->_b_buffer_read )
			{
				sound_input->_lpDSBuffer->GetCurrentPosition( &capture_pos, &read_pos );
				UINT32 bpc = sound_input->get_byte_per_capture();
				if(	sound_input->_lpDSBuffer->Lock( index * bpc, bpc, &lpvAudioPtr1, &dwAudioBytes1, &lpvAudioPtr2, &dwAudioBytes2, 0 ) == DS_OK )
				{
					if( ( dwAudioBytes1 + dwAudioBytes2 ) <= bpc )
					{
						c_buffer_stream&	sound_buffer = sound_input->get_capture_stream();
						sound_buffer.put( (UINT8*)lpvAudioPtr1, dwAudioBytes1 );
						//MEMCPY( (void*)sound_input->_sound_buffer, lpvAudioPtr1, dwAudioBytes1 );
						if( dwAudioBytes2 )
						{
							sound_buffer.put( (UINT8*)lpvAudioPtr2, dwAudioBytes2 );
							//MEMCPY( (void*)( sound_input->_sound_buffer + dwAudioBytes1 ), lpvAudioPtr2, dwAudioBytes2 );
						}
						//	fprintf(outerr," P1:%X size1:%d P2:%x size2:%d ", lpvAudioPtr1, dwAudioBytes1, lpvAudioPtr2, dwAudioBytes2);
					}
					else
						ERR_PRINT_STRING( "DirectSound !Illegal buffer size!" );
					sound_input->_lpDSBuffer->Unlock( lpvAudioPtr1, dwAudioBytes1, lpvAudioPtr2, dwAudioBytes2 );
				}
				else
				{
					ERR_PRINT_STRING( "DirectSound !Unable to lock!");
				}
			}
		}
	} // while
	if( sound_input )
		sound_input->_b_notification_handling = false;
	return TRUE;
}

c_sound_input_ds::c_sound_input_ds()
	:_b_notification_handling(false)
{
	init();
}

void	c_sound_input_ds::init()
{
	c_sound_input::init();

	_h_notify_thread = (HANDLE)nullptr;
	for( INT32 i = 0; i < SOUND_BUFFER_NB; ++i )
	{
		_h_NotifyEvent[i] = nullptr;
	}
	_p_DSCapture = nullptr;
	_lpDSBuffer = nullptr;
	_lpDsNotify = nullptr;
}

c_sound_input_ds::~c_sound_input_ds()
{
	deinit();
}

void	c_sound_input_ds::deinit()
{
	disable();
}

bool	c_sound_input_ds::enable_low()
{
	//	directsound capture section
	LPGUID p_guid = nullptr;

	auto lib = reinterpret_cast<c_sound_directsound*>(snd::g_master->get_lib());
//	auto device_id = get_device();
	auto device_id = lib->get_device_in();
	if( device_id )
	{

		if( lib )
			p_guid = lib->get_in_guid( device_id );	//hack we should also check we don't open twice the same device
		if( p_guid == nullptr )
		{
			ERR_PRINT_STRING( "DirectSound :can't enable sound input. the device don't exist" );
			return false;
		}
		//else
		//	// Get Name of the device
		//	device_name_ = sound->get_name( device_ );
	}
	else
		p_guid =  nullptr;

	//the direct sound object
	//todo	enumerate the different capture objects
	UINT32 u32_return = DirectSoundCaptureCreate( p_guid, &_p_DSCapture, nullptr );
	if( u32_return != DS_OK )
	{
		ds_err_show( u32_return, "c_sound_directsound::init_input() DirectSoundCaptureCreate(), Can't create DirectSound Capture Object" );
		return false;
	}

	//the capture buffer
	DSCCAPS			st_capture_caps;
	st_capture_caps.dwSize = sizeof( DSCCAPS );
	u32_return = _p_DSCapture->GetCaps( &st_capture_caps );
	if( u32_return != DS_OK )
	{
		ds_err_show( u32_return, "c_sound_directsound::init_input() GetCaps()" );
	}
	else
	{
		snd::PRINT( "sound capture support these modes with %d channel :", INT32( st_capture_caps.dwChannels ) );
		snd::print_wave_modes( st_capture_caps.dwFormats );
	}

	_wfx.wFormatTag			= WAVE_FORMAT_PCM;
	_wfx.nChannels			= get_input_nb();
	_wfx.nSamplesPerSec		= get_sample_per_sec_mono();
	_wfx.nAvgBytesPerSec	= get_byte_per_sec();
	_wfx.nBlockAlign		= get_byte_per_sample();
	_wfx.wBitsPerSample		= get_bit_per_sample_mono();
	_wfx.cbSize				= 0;

	DSCBUFFERDESC	buffer_desc;

	buffer_desc.dwSize = sizeof( DSCBUFFERDESC );
	buffer_desc.dwFlags = 0;
	buffer_desc.dwBufferBytes = get_byte_per_capture() * SOUND_BUFFER_NB;
	buffer_desc.dwReserved = 0;
	buffer_desc.lpwfxFormat = &_wfx;
//	these 2 field are not unused anyhow when dwFlags is 0
//		not using it make code compatible with more version of included	
//		but now 2014 cause trouble
	buffer_desc.dwFXCount = 0;
	buffer_desc.lpDSCFXDesc = nullptr;
	u32_return = _p_DSCapture->CreateCaptureBuffer( &buffer_desc, &_lpDSBuffer, nullptr );
	if( u32_return != DS_OK )
	{
		ds_err_show( u32_return, "c_sound_input_ds::enable_low() CreateCaptureBuffer()" );
		return false;
	}

	//		channel_nb_ = channel_nb_ui_;
	//the notification interface
//todox64
//#if !AAA_WIN64()
	u32_return = _lpDSBuffer->QueryInterface( IID_IDirectSoundNotify, reinterpret_cast<void**>(&_lpDsNotify) );
//#else
//	u32_return = DSERR_GENERIC;
//#endif
	if( u32_return != DS_OK )
	{
		ds_err_show( u32_return, "c_sound_directsound::init_input() QueryInterface(), DirectSound CaptureBuffer interface query failed");
		return false;
	}

	// create the events
	UINT32	i;
	for( i = 0; i < SOUND_BUFFER_NB; ++i )
	{
		if( !( _h_NotifyEvent[i] = CreateEvent( nullptr, FALSE, FALSE, nullptr ) ) )
		{
			ERR_PRINT_STRING("event creation failed");
			return false;
		}
		rgdsbpn[i].dwOffset = get_byte_per_capture() * ( i + 1 ) - 1;
		rgdsbpn[i].hEventNotify = _h_NotifyEvent[i];
	}
	if( !( _h_NotifyEvent[i] = CreateEvent( nullptr, FALSE, FALSE, nullptr ) ) )
	{
		ERR_PRINT_STRING( "event creation failed" );
		return false;
	}
	rgdsbpn[i].dwOffset = DSBPN_OFFSETSTOP;
	rgdsbpn[i].hEventNotify = _h_NotifyEvent[i];

	// Now create the thread to wait on the events created.
	//		u8_fft_done_ = 0;
	_b_buffer_read = false;
	_b_notification_ask_stop = false;
	_h_notify_thread = CreateThread( nullptr, 0, (LPTHREAD_START_ROUTINE)HandleNotifications, (LPVOID)this, 0, &_dwThreadId );
	if( !_h_notify_thread )
	{
		ERR_PRINT_STRING("c_sound_directsound::init_input() thread creation failed");
		return false;
	}
	if( SetThreadPriority( _h_notify_thread, THREAD_PRIORITY_ABOVE_NORMAL ) == 0 )
	{
		ERR_PRINT_STRING("c_sound_directsound::init_input() Can't set thread priority");
	}

	u32_return = _lpDsNotify->SetNotificationPositions( SOUND_BUFFER_NB + 1, rgdsbpn );
	if( u32_return != DS_OK )
	{
		ds_err_show( u32_return, "c_sound_directsound::init_input() SetNotificationPositions()" );
		release_notification_events();
	}

	_lpDSBuffer->Start( DSCBSTART_LOOPING );
	return true;
}

bool	c_sound_input_ds::disable_low()
{
	UINT32 u32_return = DS_OK;

	if( _p_DSCapture )
	{
		if( _lpDSBuffer )
		{
			//kill the wait-for-notify thread
			u32_return = _lpDSBuffer->Stop();

			if( _h_notify_thread )
			{
				// ask the thread to stop
				_b_notification_ask_stop = true;
				//	wait for thread to stop
				INT32 nb_loop = 1000;
				while( _b_notification_handling )
				{
					spy::sleep( 1, "sleep c_sound_input_ds::disable_low" );
					if( --nb_loop < 0 )
					{
						ERR_PRINT_STRING( "%s() waited for the task to stop too many time, we stop waiting", __FUNCTION__ );
						break;
					}
				}	//	wait for thread to stop
				_h_notify_thread = nullptr;
			}
			release_notification_events();

			if( _lpDsNotify )
			{
				u32_return = _lpDsNotify->Release();
			}
			//	here is the locking call
			u32_return = _lpDSBuffer->Release();
			_lpDSBuffer = nullptr;
		}
		//	here is the second	locking call
		//	if we skip the first one
		_p_DSCapture->Release();
		_p_DSCapture = nullptr;
	}

	return true;
}

void	c_sound_input_ds::release_notification_events()
{
	_lpDsNotify->SetNotificationPositions( 0, nullptr );
	for( INT32 i = 0; i < SOUND_BUFFER_NB + 1; ++i )
	{
		if( _h_NotifyEvent[i] )
		{
			CloseHandle( _h_NotifyEvent[i] );
			_h_NotifyEvent[i] = nullptr;
		}
	}
}

void	c_sound_input_ds::update()
{
}
