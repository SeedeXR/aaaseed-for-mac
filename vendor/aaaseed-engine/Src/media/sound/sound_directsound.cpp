#undef WIN32_LEAN_AND_MEAN

#include "sound_directsound.h"
#include "snd_master.h"
#include "snd_input_directsound.h"
#include "snd_player_directsound.h"

#include "infrastructure/param/param_declare.h"
#include "infrastructure/aaa_window.h"

#include <lib_use.h>
AAA_LIB_USE( "DSound" )

C_PCHAR_C ds_str_error(UINT32 err)
{
	C_PCHAR s;
	switch( err )
		{
		case DS_NO_VIRTUALIZATION:		s = "DS_NO_VIRTUALIZATION : Ok but we had to substitute the 3D algorithm";	break;
		//	case DS_INCOMPLETE:			s = "DS_INCOMPLETE : Ok but not all of the optional effects were obtained";	break;
		case DSERR_ALLOCATED:			s = "DSERR_ALLOCATED : resources (such as a priority level) were already being used by another caller";	break;
		case DSERR_CONTROLUNAVAIL:		s = "DSERR_CONTROLUNAVAIL : The control (vol, pan, etc.) requested by the caller is not available";	break;
		case DSERR_INVALIDPARAM:		s = "DSERR_INVALIDPARAM : An invalid parameter was passed";	break;
		case DSERR_INVALIDCALL:			s = "DSERR_INVALIDCALL : call is not valid for the current state of this object";	break;
		case DSERR_GENERIC:				s = "DSERR_GENERIC : An undetermined error occurred inside the DirectSound subsystem";		break;
		case DSERR_PRIOLEVELNEEDED:		s = "DSERR_PRIOLEVELNEEDED : The caller does not have the priority level required for the function to succeed";	break;
		case DSERR_OUTOFMEMORY:			s = "DSERR_OUTOFMEMORY : Not enough free memory is available to complete the operation";	break;
		case DSERR_BADFORMAT:			s = "DSERR_BADFORMAT : The specified WAVE format is not supported";		break;
		case DSERR_UNSUPPORTED:			s = "DSERR_UNSUPPORTED : The function called is not supported at this time";	break;
		case DSERR_NODRIVER:			s = "DSERR_NODRIVER : No sound driver is available for use";		break;
		case DSERR_ALREADYINITIALIZED:	s = "DSERR_ALREADYINITIALIZED : This object is already initialized";	break;
		case DSERR_NOAGGREGATION:		s = "DSERR_NOAGGREGATION : This object does not support aggregation";	break;
		case DSERR_BUFFERLOST:			s = "DSERR_BUFFERLOST : The buffer memory has been lost, and must be restored";	break;
		case DSERR_OTHERAPPHASPRIO:		s = "DSERR_OTHERAPPHASPRIO : Another app has a higher priority level, preventing this call from succeeding";	break;
		case DSERR_UNINITIALIZED:		s = "DSERR_UNINITIALIZED : This object has not been initialized";	break;
		case DSERR_NOINTERFACE:			s = "DSERR_NOINTERFACE : The requested COM interface is not available";	break;
		case DSERR_ACCESSDENIED:		s = "DSERR_ACCESSDENIED : Access is denied";	break;
		case DSERR_BUFFERTOOSMALL:		s = "DSERR_BUFFERTOOSMALL : Tried to create a DSBCAPS_CTRLFX buffer shorter than DSBSIZE_FX_MIN milliseconds";	break;
		case DSERR_DS8_REQUIRED:		s = "DSERR_DS8_REQUIRED : Attempt to use DirectSound 8 functionality on an older DirectSound object";	break;
		case DSERR_SENDLOOP:			s = "DSERR_SENDLOOP : A circular loop of send effects was detected";	break;
		case DSERR_BADSENDBUFFERGUID:	s = "DSERR_BADSENDBUFFERGUID : The GUID specified in an audio path file does not match a valid MIXIN buffer";	break;
		case DSERR_OBJECTNOTFOUND:		s = "DSERR_OBJECTNOTFOUND : The object requested was not found (numerically equal to DMUS_E_NOT_FOUND)";	break;
		case DSERR_FXUNAVAILABLE:		s = "DSERR_FXUNAVAILABLE :  The effects requested could not be found on the system, or they were found but in the wrong order, or in the wrong hardware/software locations";	break;
		default:						s = "Direct Sound error unknown by AAASeed";	break;
		}
	return s;
}

void	ds_err_show( UINT32 err_code, C_PCHAR_C mess )
{
	C_PCHAR_C str_detail = ds_str_error( err_code );
	ERR_PRINT_STRING( mess );
	ERR_PRINT_STRING( str_detail );

//	sprintf( err_str, "%s\n%s.", mess, str_detail );
//	BOX_ERR( "%s\n%s.", mess, str_detail );
}
//#endif	//WIN32

////global level function for retrieving and dumping the sound capture buffer
//DWORD CALLBACK HandleNotifications( LPVOID lpvoid )
//{
//	DWORD			hr = DS_OK;
//	DWORD			hRet = 0;
//	UINT32			index;
//	LPVOID			lpvAudioPtr1;
//	DWORD			dwAudioBytes1; 
//	LPVOID			lpvAudioPtr2; 
//	DWORD			dwAudioBytes2;
//	DWORD			capture_pos;
//	DWORD			read_pos;
//
//	c_sound_input_ds*	snd_input = (c_sound_input_ds*)lpvoid;
//
//	if( !snd_input )
//		{
//		ERR_PRINT_STRING( "DirectSound HandleNotifications() with NULL snd_input");
//		return FALSE;
//		}
//
//	sprintf( err_str, "DirectSound HandleNotifications() with snd_input : %X", snd_input );
//	SOUND_PRINT_STRING( err_str);
//	while( snd_input->b_sound_capture_thread_on_ )
//		{
//		hRet = WaitForMultipleObjects( c_sound_input_ds::SOUND_BUFFER_NB + 1, snd_input->h_NotifyEvent_, FALSE, 1000 );
//
//		index = hRet - WAIT_OBJECT_0;
//		if ( index < 0 || index > c_sound_input_ds::SOUND_BUFFER_NB )
//			{
//			if( index == WAIT_TIMEOUT )
//				{
//				ERR_PRINT_STRING( "DirectSound snd_input TimeOut" );
//				}
//			else if( index == c_sound_input_ds::SOUND_BUFFER_NB )
//				{
//				ERR_PRINT_STRING( "DirectSound Maa: last strange Object notified" );
//				}
//			else
//				ERR_PRINT_STRING( "DirectSound snd_input Notify Error" );
//			}
//		else if( index == c_sound_input_ds::SOUND_BUFFER_NB )
//			{
//			break;
//			}
//		else
//			{
//			// Deal with the event that got signaled.
//			//		fprintf(outerr,"Notify #%d ",index);
//			if( !snd_input->b_buffer_read_ )
//				{
//				snd_input->lpDSBuffer_->GetCurrentPosition( &capture_pos, &read_pos );
//
//				if(	snd_input->lpDSBuffer_->Lock( index * c_sound_input_ds::SOUND_BYTE_PER_CAPTURE, c_sound_input_ds::SOUND_BYTE_PER_CAPTURE,
//					&lpvAudioPtr1, &dwAudioBytes1, &lpvAudioPtr2, &dwAudioBytes2, nullptr ) == DS_OK )
//					{
//					if( ( dwAudioBytes1 + dwAudioBytes2 ) <= c_sound_input_ds::SOUND_BYTE_PER_CAPTURE )
//						{
//						snd_input->capture_count_++;
//						//if( gb_verbose_fft )
//						//{
//						//	if ( ( snd_input->capture_count_ & 0x7f ) == 0 )
//						//	{
//						//		sprintf( err_str, "capture %d", snd_input->capture_count_ );
//						//		VERBOSE_PRINTF( err_str );
//						//	}
//						//}
//
//						MEMCPY( (void*)snd_input->sound_buffer_, lpvAudioPtr1, dwAudioBytes1);
//						if( dwAudioBytes2 )
//							//todo try to use MEMCPY
//							MEMCPY( (void*)( snd_input->sound_buffer_ + dwAudioBytes1 ), lpvAudioPtr2, dwAudioBytes2 );
//						//					fprintf(outerr," P1:%X size1:%d P2:%x size2:%d ",
//						//							lpvAudioPtr1,  
//						//							dwAudioBytes1,  
//						//							lpvAudioPtr2,  
//						//							dwAudioBytes2);
//						}
//					else
//						ERR_PRINT_STRING( "DirectSound !Illegal buffer size!" );
//
//					snd_input->lpDSBuffer_->Unlock( lpvAudioPtr1, dwAudioBytes1, lpvAudioPtr2, dwAudioBytes2 );
//
//					//if( snd_input->u8_fft_done_ )
//					//{
//					//	if( gb_verbose_fft )
//					//		VERBOSE_PRINTF("skip fft");
//					//}
//					//else
//					//{
//					//}
//					}
//				else
//					{
//					ERR_PRINT_STRING( "DirectSound !Unable to lock!");
//					}
//				}
//			}
//
//		} // while
//	return TRUE;
//} 


//c_sound_input_ds::c_sound_input_ds()
//{
//}
//
//c_sound_input_ds::~c_sound_input_ds()
//{
//
//}
//
//void	c_sound_input_ds::release_notification_events()
//{
//	INT32	i;
//	lpDsNotify_->SetNotificationPositions( 0, nullptr );
//	for( i = 0; i < SOUND_BUFFER_NB + 1; ++i )
//	{
//		if( h_NotifyEvent_[i] )
//		{
//			CloseHandle( h_NotifyEvent_[i] );
//			h_NotifyEvent_[i] = nullptr;
//		}
//	}
//}
//
//void	c_sound_input_ds::init( INT32 device, LPGUID p_guid, INT32 nb_channel_in )
//{
//	UINT32			i;
//	DSCCAPS			st_capture_caps;
//	DSCBUFFERDESC	st_buffer_desc;
//	UINT32			u32_return;
//
//	if( !b_enable_ )
//		{
////		LPGUID	p_guid;
//		SOUND_PRINT_STRING( "DirectSound enabling sound input" );
//		device_ = device;
//
//		if( !p_guid )
//			{
//			return;
//			}
////		if( device )
////			{
////			if( sound )
//////				get_guid( device );	//hack we should also check we don't open twice the same device
////			if( !p_guid )
////				{
////				ERR_PRINT_STRING( "DirectSound :can't enable sound input. the device don't exist" );
////				return;
////				}
////			}
////		else
////			p_guid =  nullptr;
//
//		//the direct sound object
//		//todo	enumerate the different capture objects
//		u32_return = DirectSoundCaptureCreate( p_guid, &p_DSCapture_, nullptr );
//		if( u32_return != DS_OK )
//			{
//			ds_err_show( u32_return, "c_sound_directsound::init_input() DirectSoundCaptureCreate(), Can't create DirectSound Capture Object" );
//			return;
//			}
//
//		//the capture buffer
//		st_capture_caps.dwSize = sizeof( DSCCAPS );
//		u32_return = p_DSCapture_->GetCaps( &st_capture_caps );
//		if( u32_return != DS_OK )
//			{
//			ds_err_show( u32_return, "c_sound_directsound::init_input() GetCaps()" );
//			}
//		else
//			{
//			sprintf( err_str, "sound capture support these modes with %d channel :", INT32( st_capture_caps.dwChannels ) );
//			SOUND_PRINT_STRING( err_str);
//			print_wave_mode( st_capture_caps.dwFormats );
//			}
//		}
//
//		wfx_.wFormatTag = WAVE_FORMAT_PCM;
//		wfx_.nChannels = nb_channel_in;
//		wfx_.nSamplesPerSec = SOUND_SAMPLE_PER_SEC;
//		wfx_.nAvgBytesPerSec = SOUND_BYTE_PER_SECOND_MONO * nb_channel_in;
//		wfx_.nBlockAlign = SOUND_BYTE_PER_SAMPLE * nb_channel_in;
//		wfx_.wBitsPerSample = SOUND_BIT_PER_SAMPLE;
//		wfx_.cbSize = 0;
//
//		st_buffer_desc.dwSize = sizeof( DSCBUFFERDESC );
//		st_buffer_desc.dwFlags = 0;
//		st_buffer_desc.dwBufferBytes = SOUND_BYTE_PER_CAPTURE * SOUND_BUFFER_NB;
//		st_buffer_desc.dwReserved = 0;
//		st_buffer_desc.lpwfxFormat = &wfx_;
//		st_buffer_desc.dwFXCount = 0;
//		st_buffer_desc.lpDSCFXDesc = nullptr;
//		u32_return = p_DSCapture_->CreateCaptureBuffer( &st_buffer_desc, &lpDSBuffer_, nullptr );
//		if( u32_return != DS_OK )
//			{
//			ds_err_show( u32_return, "c_sound_directsound::init_input() CreateCaptureBuffer()");
//			b_enable_ = false;
//			return;
//			}
//
//		//		channel_nb_ = channel_nb_ui_;
//		//the notification interface
//		u32_return = lpDSBuffer_->QueryInterface( IID_IDirectSoundNotify, (void**)&lpDsNotify_ );
//		if( u32_return != DS_OK )
//		{
//			ds_err_show( u32_return, "c_sound_directsound::init_input() QueryInterface(), DirectSound CaptureBuffer interface query failed");
//			return;
//		}
//
//		// create the events
//		for( i = 0; i < SOUND_BUFFER_NB; ++i )
//			{
//			if( !( h_NotifyEvent_[i] = CreateEvent( nullptr, FALSE, FALSE, nullptr ) ) )
//				{
//				ERR_PRINT_STRING("event creation failed");
//				return;
//				}
//			rgdsbpn[i].dwOffset = SOUND_BYTE_PER_CAPTURE * ( i + 1 ) - 1;
//			rgdsbpn[i].hEventNotify = h_NotifyEvent_[i];
//			}
//		if( !( h_NotifyEvent_[i] = CreateEvent( nullptr, FALSE, FALSE, nullptr ) ) )
//			{
//			ERR_PRINT_STRING( "event creation failed" );
//			return;
//			}
//		rgdsbpn[i].dwOffset = DSBPN_OFFSETSTOP;
//		rgdsbpn[i].hEventNotify = h_NotifyEvent_[i];
//
//
//		// Now create the thread to wait on the events created.
//		//		u8_fft_done_ = 0;
//		b_buffer_read_ = false;
//		b_sound_capture_thread_on_ = true;
//
//		if( !( h_notify_thread_ = CreateThread( nullptr, 0, (LPTHREAD_START_ROUTINE)HandleNotifications, (LPVOID)this, 0, &dwThreadId_ ) ) )
//			{
//			ERR_PRINT_STRING("c_sound_directsound::init_input() thread creation failed");
//			return;
//			}
//
//		if( SetThreadPriority( h_notify_thread_, THREAD_PRIORITY_ABOVE_NORMAL ) == 0 )
//			ERR_PRINT_STRING("c_sound_directsound::init_input() Can't set thread priority");
//
//		u32_return = lpDsNotify_->SetNotificationPositions( SOUND_BUFFER_NB + 1, rgdsbpn );
//		if( u32_return != DS_OK )
//			{
//			ds_err_show( u32_return, "c_sound_directsound::init_input() SetNotificationPositions()" );
//			release_notification_events();
//			}
//
//		lpDSBuffer_->Start( DSCBSTART_LOOPING );
//		b_enable_ = true;
//		//		reset_fft();
//		}
//
//}
//
//void	c_sound_input_ds::deinit()
//{
//
//}

FACTORY_CREATE_V1( c_sound_directsound, sound_directsound, sound Input DirectSound, sound_ds );

namespace	n_sound_directsound
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 2;
	CONSTEXPR INT32 INPUT_PARAM_NB	= 5;
	CONSTEXPR INT32 OUTPUT_PARAM_NB	= 5;
	CONSTEXPR INT32 GROUP_NB		= 2;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	INPUT_PARAM_NB
									+	OUTPUT_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_ON(	Active				)
		PARAM_DEF_INT32(		channel_nb_asked,	1, 2,	1, 2	)			   
		PARAM_DEF_GROUP( Input, INPUT_PARAM_NB )
			PARAM_DEF_INT32_POS_ZERO(	in_device			)
			PARAM_DEF_INT32_LOCKED(		in_device_nb		)
			PARAM_DEF_INT32_POS(		in_channel_nb,		0, 2 )
			PARAM_DEF_SYMBO_PSTR(		in_sample_rate,		3, 4,	snd::sample_rate_str )
			PARAM_DEF_SYMBO_PSTR_ONE(	in_bits,					snd::sample_bits_str )
		PARAM_DEF_GROUP( Output, OUTPUT_PARAM_NB )
			PARAM_DEF_INT32_POS_ZERO(	out_device			)
			PARAM_DEF_INT32_LOCKED(		out_device_nb		)
			PARAM_DEF_INT32_POS(		out_channel_nb,		0, 2 )
			PARAM_DEF_SYMBO_PSTR(		out_sample_rate,	3, 4,	snd::sample_rate_str )
			PARAM_DEF_SYMBO_PSTR_ONE(	out_bits,					snd::sample_bits_str )
	};
};

void	c_sound_directsound::param_init_pt()
{
	INT32	h = 0;

	param_set_pt( h, get_pt_active()		);
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

	err_param_init_pt( h );
}

LPGUID	c_sound_directsound::get_in_guid( UINT32 index )
{
	if( index <= _sound_device_in_guid.size() )
		return &_sound_device_in_guid[ index - 1 ];
	else
		return nullptr;
}
LPGUID	c_sound_directsound::get_out_guid( UINT32 index )
{
	if( index <= _sound_device_out_guid.size() )
		return &_sound_device_out_guid[ index - 1 ];
	else
		return nullptr;
}



//todoqq start to deal with showing name, then select by name
/*	
o_str*	c_sound_directsound::get_name( INT32 in )
{
	return nullptr;
//	if ( in < sound_input_device_name_.size() )
//		return sound_input_device_name_[in - 1];
//	else
//		return nullptr;
}
*/

static BOOL CALLBACK ds_enum_proc( LPGUID lpGUID, LPCSTR lpszDesc, LPCSTR lpszDrvName, LPVOID lpContext )
{
	c_sound_directsound*	sound = (c_sound_directsound*)lpContext;
	std::vector<GUID>&		vec_guid = sound->_b_enum_input ? sound->_sound_device_in_guid : sound->_sound_device_out_guid;

	if( lpGUID )
		vec_guid.push_back( *lpGUID );

	snd::print_device( (INT32)vec_guid.size(), (CHAR*)lpszDesc, (CHAR*)lpszDrvName );

	return TRUE;
}

INT32	c_sound_directsound::enumerate_device_in()
{
	//	sound_device_index = 0;
	_sound_device_in_guid.clear();
	//	sound_input_device_name_.clear();
	snd::PRINT( "DirectSound --- Enumerate Input :" );
	_b_enum_input = true;
	if( FAILED( DirectSoundCaptureEnumerateA( (LPDSENUMCALLBACKA)ds_enum_proc, this ) ) )
		ERR_PRINT_STRING( "Can't DirectSoundCaptureEnumerate" );

	return (INT32)_sound_device_in_guid.size();
}

//todo deal really with UNICODE 
INT32	c_sound_directsound::enumerate_device_out()
{
	//	sound_device_index = 0;
	_sound_device_out_guid.clear();
	//	sound_output_device_name_.clear();
	snd::PRINT( "DirectSound --- Enumerate :" );
	_b_enum_input = false;
	if( FAILED( DirectSoundEnumerateA( (LPDSENUMCALLBACKA)ds_enum_proc, this ) ) )
		ERR_PRINT_STRING( "Can't DirectSoundEnumerate" );

	return (INT32)_sound_device_out_guid.size();
}


CONSTRUCTOR_CREATE( c_sound_directsound )
{
	for ( INT32 i = 0; i < OUTPUT_NB_MAX; ++i )
		_p_ds[ i ] = nullptr;

	param_init_with( n_sound_directsound::param, n_sound_directsound::PARAM_NB_MAX );
}

c_sound_directsound::~c_sound_directsound()
{
}

void	c_sound_directsound::close()
{
	c_sound::close();

	for ( INT32 i = 0; i < OUTPUT_NB_MAX; ++i )
	{
		if ( _p_ds[ i ])
		{
			_p_ds[ i ]->Release();
			_p_ds[ i ] = nullptr;
		}
	}
}

void	c_sound_directsound::init_output( INT32 index )
{
	if( !_b_output_enable[ index ] )
	{
		//get DirectSound interface
		//	LPGUID	guid = nullptr;
		LPCGUID guid = nullptr;
		if ( index > 0 )
			guid = get_out_guid( index );

		LPDIRECTSOUND	p_ds = nullptr;
		UINT32 u32_return = DirectSoundCreate( guid, &p_ds, nullptr );
		if( u32_return != DS_OK )
		{
			ds_err_show( u32_return, "init_output() DirectSoundCreate()");
		}
		else
		{
			snd::PRINT( "Device Output %d created", index );
			_p_ds[ index ] = p_ds;
			//setcooperativelevel of interface
			HWND hd_win = get_window_main_handle();
			if( !hd_win )
			{
				hd_win = GetForegroundWindow();
				if( !hd_win )
					hd_win = GetDesktopWindow();
			}
			u32_return = _p_ds[ index ]->SetCooperativeLevel( hd_win, DSSCL_PRIORITY );
			if( u32_return != DS_OK )
			{
				ds_err_show( u32_return, "init_output() SetCooperativeLevel()");
				delete_player_all();	//todosnd this a quick hack
			}
			else
				_b_output_enable[ index ]  = true;
		}
	}
}


c_sound_input*	c_sound_directsound::create_input_new()
{
	return new c_sound_input_ds;
}

c_sound_player*	c_sound_directsound::create_player_new()
{
	return new c_sound_player_ds;
}

