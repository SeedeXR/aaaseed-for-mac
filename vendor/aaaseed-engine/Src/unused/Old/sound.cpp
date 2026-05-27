#include "sound.h"
#include "aaa_file.h"
#include "sndinput.h"
#include "err.h"
//#include "bass.h"
#include "aaa_math.h"

#define	SOUND_HEADER  "# SOUND "
extern	void	SOUND_PRINT_STRING( const CHAR* const fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	TRACKER_PRINT_STRING( SOUND_HEADER, fmt, va_pass(args));
	va_end(args);
}

CHAR* bass_get_str_error( UINT32 err )
{
	CHAR*	s;
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
	case BASS_ERROR_NOPAUSE :	s = "BASS_ERROR_NOPAUSE"; break;
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
	case BASS_ERROR_PLAYING :	s = "BASS_ERROR_PLAYING"; break;
	case BASS_ERROR_NOTAVAIL :	s = "BASS_ERROR_NOTAVAIL"; break;
	case BASS_ERROR_DECODE :	s = "BASS_ERROR_DECODE"; break;
	case BASS_ERROR_DX :		s = "BASS_ERROR_DX"; break;
	case BASS_ERROR_TIMEOUT :	s = "BASS_ERROR_TIMEOUT"; break;
	case BASS_ERROR_FILEFORM :	s = "BASS_ERROR_FILEFORM"; break;
	case BASS_ERROR_SPEAKER :	s = "BASS_ERROR_SPEAKER"; break;
	case BASS_ERROR_UNKNOWN :	s = "BASS_ERROR_UNKNOWN"; break;
	default :					s = "BASS_ERROR_UNKNOWN"; break;
	}
	return s;
}

#ifdef	WIN32
#include <dsound.h>

CHAR* ds_get_str_error( UINT32 err )
{
CHAR* s;
	switch( err )
	{
	case DS_NO_VIRTUALIZATION:	s = "DS_NO_VIRTUALIZATION : Ok but we had to substitute the 3D algorithm";	break;
//		case DS_INCOMPLETE:			s = "DS_INCOMPLETE : Ok but not all of the optional effects were obtained";	break;
	case DSERR_ALLOCATED:		s = "DSERR_ALLOCATED : resources (such as a priority level) were already being used by another caller";	break;
	case DSERR_CONTROLUNAVAIL:	s = "DSERR_CONTROLUNAVAIL : The control (vol, pan, etc.) requested by the caller is not available";	break;
	case DSERR_INVALIDPARAM:	s = "DSERR_INVALIDPARAM : An invalid parameter was passed";	break;
	case DSERR_INVALIDCALL:		s = "DSERR_INVALIDCALL : call is not valid for the current state of this object";	break;
	case DSERR_GENERIC:			s = "DSERR_GENERIC : An undetermined error occurred inside the DirectSound subsystem";		break;
	case DSERR_PRIOLEVELNEEDED:	s = "DSERR_PRIOLEVELNEEDED : The caller does not have the priority level required for the function to succeed";	break;
	case DSERR_OUTOFMEMORY:		s = "DSERR_OUTOFMEMORY : Not enough free memory is available to complete the operation";	break;
	case DSERR_BADFORMAT:		s = "DSERR_BADFORMAT : The specified WAVE format is not supported";		break;
	case DSERR_UNSUPPORTED:		s = "DSERR_UNSUPPORTED : The function called is not supported at this time";	break;
	case DSERR_NODRIVER:		s = "DSERR_NODRIVER : No sound driver is available for use";		break;
	case DSERR_ALREADYINITIALIZED:	s = "DSERR_ALREADYINITIALIZED : This object is already initialized";	break;
	case DSERR_NOAGGREGATION:	s = "DSERR_NOAGGREGATION : This object does not support aggregation";	break;
	case DSERR_BUFFERLOST:		s = "DSERR_BUFFERLOST : The buffer memory has been lost, and must be restored";	break;
	case DSERR_OTHERAPPHASPRIO:	s = "DSERR_OTHERAPPHASPRIO : Another app has a higher priority level, preventing this call from succeeding";	break;
	case DSERR_UNINITIALIZED:	s = "DSERR_UNINITIALIZED : This object has not been initialized";	break;
	case DSERR_NOINTERFACE:		s = "DSERR_NOINTERFACE : The requested COM interface is not available";	break;
	case DSERR_ACCESSDENIED:	s = "DSERR_ACCESSDENIED : Access is denied";	break;
	case DSERR_BUFFERTOOSMALL:	s = "DSERR_BUFFERTOOSMALL : Tried to create a DSBCAPS_CTRLFX buffer shorter than DSBSIZE_FX_MIN milliseconds";	break;
	case DSERR_DS8_REQUIRED:	s = "DSERR_DS8_REQUIRED : Attempt to use DirectSound 8 functionality on an older DirectSound object";	break;
	case DSERR_SENDLOOP:		s = "DSERR_SENDLOOP : A circular loop of send effects was detected";	break;
	case DSERR_BADSENDBUFFERGUID:	s = "DSERR_BADSENDBUFFERGUID : The GUID specified in an audiopath file does not match a valid MIXIN buffer";	break;
	case DSERR_OBJECTNOTFOUND:	s = "DSERR_OBJECTNOTFOUND : The object requested was not found (numerically equal to DMUS_E_NOT_FOUND)";	break;
	case DSERR_FXUNAVAILABLE:	s = "DSERR_FXUNAVAILABLE :  The effects requested could not be found on the system, or they were found but in the wrong order, or in the wrong hardware/software locations";	break;
	default:					s = "Direct Sound error unknown by AAASeed";	break;
	}
	return s;
}

void	ds_err_show( UINT32 err_code, CHAR* mess)
{
CHAR* str_detail = ds_get_str_error( err_code);
	ERR_PRINT_STRING( mess );
	ERR_PRINT_STRING( str_detail );

	BOX_ERR( "%s\n%s.", mess, str_detail );
}
#endif	//WIN32


//#include <windows.h>
//#include <mmsystem.h>

LPDIRECTSOUND p_ds = NULL;

void	ds_free()
{
	if( p_ds )
	{
		//free the interface to directsound functions
		p_ds->Release();
		p_ds = NULL;
	}
}

INT32	sound_device_index;
vector<LPGUID>	sound_capture_device_guid;
vector<LPGUID>	sound_device_guid;

LPGUID	ds_get_guid(INT32 in)
{
	if ( in < sound_capture_device_guid.size() )
		return sound_capture_device_guid[in-1];
	else
		return NULL;
}

BOOL CALLBACK DSEnumProc(LPGUID lpGUID, 
						 LPCTSTR lpszDesc,
						 LPCTSTR lpszDrvName, 
						 LPVOID lpContext )
{
	if ( lpGUID != NULL )
	{
		SOUND_PRINT_STRING( "Device %d :", ++sound_device_index);
		SOUND_PRINT_STRING( "\t%s", lpszDesc );
		SOUND_PRINT_STRING( "\tdriver -> %s", lpszDrvName );
		( (vector<LPGUID>*)lpContext)->push_back( lpGUID);
	}
	return( TRUE );
}


void ds_enum()
{
	sound_device_index = 0;
	sound_device_guid.clear();;
	SOUND_PRINT_STRING( "DirectSoundEnumerate :" );
	if( FAILED(DirectSoundEnumerate((LPDSENUMCALLBACK)DSEnumProc, &sound_device_guid) ) )
		ERR_PRINT_STRING( "Can't DirectSoundEnumerate" );

	sound_device_index = 0;
	sound_capture_device_guid.clear();;
	SOUND_PRINT_STRING( "DirectSoundCaptureEnumerate :" );
	if( FAILED(DirectSoundCaptureEnumerate((LPDSENUMCALLBACK)DSEnumProc, &sound_capture_device_guid) ) )
		ERR_PRINT_STRING( "Can't DirectSoundEnumerate" );
}

void ds_init()
{
	UINT32 u32_return;

//	ds_enum();
	//get directsound interface
	u32_return = DirectSoundCreate( NULL, &p_ds, NULL);
	if( u32_return != DS_OK)
	{
		ds_err_show( u32_return, "in ds_init() DirectSoundCreate()");
	}
	else
	{
		HWND hd_win;

		//setcooperativelevel of interface
		hd_win = get_window_main_handle();
		if( hd_win == NULL )
		{
			hd_win = GetForegroundWindow();
			if (hd_win == NULL)
				hd_win = GetDesktopWindow();
		}

//		u32_return = p_ds->SetCooperativeLevel( hd_win, DSSCL_NORMAL);
		u32_return = p_ds->SetCooperativeLevel( hd_win, DSSCL_PRIORITY);
		if( u32_return != DS_OK)
		{
			ds_err_show( u32_return, "in ds_init() SetCooperativeLevel()");
			sound_free();
		}
	}
}


//typedef enum
//	{
//	SND_LIB_DS = 0,
//	SND_LIB_BASS,
//	SND_LIB_NB_MAX
//	}
//SND_LIB_TYPE;

INT32			s_sound_lib_name = SND_LIB_DS;
INT32			d_sound_lib_name;
//SND_LIB_TYPE	s_snd_lib = SND_LIB_BASS; //SND_LIB_DS; //SND_LIB_BASS;
extern	REAL	dummy_float[];
extern	INT32	dummy_int32[];

/*
// stream writer
//typedef DWORD (CALLBACK STREAMPROC)(HSTREAM handle, void *buffer, DWORD length, DWORD user);
//DWORD CALLBACK WriteStream(HSTREAM handle, short *buffer, DWORD length, DWORD user)
//DWORD __cdecl WriteStream( HSTREAM handle, short *buffer, DWORD length, DWORD user)
DWORD CALLBACK WriteStream(HSTREAM handle, void *buffer_in, DWORD length, DWORD user)
{
INT32	n;
float	s;
INT32	c;
//float f;
REAL	ph;
float	*buffer;
INT32	nb;
float	*pt;
REAL	vol;
REAL	f;
INT32	band_nb;	
	nb = length/(sizeof(float)*2);
	buffer = (float*)buffer_in;
	//memset( buffer_in, 0, length);
	pt = buffer; 
	for( INT32 i=0; i<nb; i++ )
		{
		*pt++ = 0.;
		*pt++ = 0.;
		}

	band_nb = CLAMP( dummy_int32[0], 1, FFT_SYNTH_BAND_MAX);
	
	vol = dummy_float[2]/REAL(band_nb);
	for( n=1; n<=band_nb; n++)
		{
		if( dummy_int32[0] <= 1)
			f = dummy_float[0];
		else
			f = interpolate( dummy_float[0], dummy_float[1], (n-1)/REAL(band_nb-1) );
		f = 20+f*20000.;
		f /= 44100.0;
//		if (!vol[n])
//			continue;
//		f = pow( 2.0, (n+3)/12.0) * TABLESIZE * 440.0/44100.0;

		ph = phase[n];
		pt = buffer;
		for ( c=0; c<nb; c++)
			{
//			s = sinetable[(int)((pos[n]+c)*f)&(TABLESIZE-1)]*vol[n]/MAXVOL;
			s = vol * SIN_INT( ph + REAL(f*c) )  ;
			s += *pt;
			*pt++ = s; // left and right channels are the same
			*pt++ = s;
			}
		vol *= dummy_float[3];
		ph = phase[n] + REAL(f*c); // update key's sine pos
		phase[n] = FMOD( ph, 1.);
		}


	//	pt = buffer;
	//	for ( c=0; c<nb; c++)
	//		{
	//		}
	//
	return length;
}
*/

/*
DWORD CALLBACK WriteStream(HSTREAM handle, void *buffer_in, DWORD length, DWORD user)
{
INT32	n;
INT32	s;
DWORD c;
//float f;
REAL	r;
REAL	ph;
short	*buffer;
FLOAT	*pt;

	memset( buffer_in, 0, length);
	buffer = (short*)buffer_in;
REAL	vol = ((1<<15)-1);

	for( n=0; n<dummy_int32[0]; n++)
		{
//		if (!vol[n])
//			continue;
//		f = pow(2.0,(n+3)/12.0)*TABLESIZE*440.0/44100.0;
//		for ( c=0; c<length/4 && vol[n]; c++)
		ph = phase[n];
		for ( c=0; c<length/8; c++)
			{
//			s = sinetable[(int)((pos[n]+c)*f)&(TABLESIZE-1)]*vol[n]/MAXVOL;
			r = vol * SIN_INT( ph + REAL(n*c)*dummy_float[0] )  ;
			s = r;
			s += (int)buffer[c*2];
			if (s>32767)
				s=32767;
			else if (s<-32768)
				s=-32768;
			buffer[c*2+1] = buffer[c*2] = s; // left and right channels are the same
//			if( vol[n]<MAXVOL )
//				vol[n]--;
			}
		vol *= dummy_float[1];
		ph = phase[n] + REAL(n*c)*dummy_float[0]; // update key's sine pos
		phase[n] = FMOD( ph, 1.);
		}
	return length;
}
*/

void	snd_bass_asio_free()
{
// todo
	// for now bass asio is only used for capture, and sound is for playback
}

BOOL	snd_bass_asio_open()
{
	// todo
	// for now bass asio is only used for capture, and sound is for playback
	return TRUE;
}

void	snd_bass_free()
{
	BASS_Free();
}

BOOL	snd_bass_open()
{
	BASS_INFO info;
	DWORD buflen;

	//	check that BASS 2.2 was loaded
	if( BASS_GetVersion() != MAKELONG(2,2) )
	{
		SOUND_PRINT_STRING( "BASS version 2.2 was not loaded" );
		return FALSE;
	}
	SOUND_PRINT_STRING( "BASS version 2.2 loaded" );

	//	10ms update period
	BASS_SetConfig( BASS_CONFIG_UPDATEPERIOD, 15 );	//hack beyrouth


	SOUND_PRINT_STRING( "BASS enumeration" );
	INT32	count = 1; // the device counter
	while( BASS_GetDeviceDescription( count ) )
	{
		SOUND_PRINT_STRING( "Device %d : %s", count, BASS_GetDeviceDescription( count ) );
		count++;
	}

	// setup output - get latency
	if( !BASS_Init( -1, 44100, BASS_DEVICE_LATENCY, 0, NULL) )
	{
		ERR_PRINT_STRING( "Can't initialize device");
		return FALSE;
	}

	//info.size=sizeof(info);
	SOUND_PRINT_STRING( "BASS playback" );
	INT32	index;
	index = BASS_GetDevice();
	SOUND_PRINT_STRING( "Device %d :", index );
	SOUND_PRINT_STRING( "\t%s", BASS_GetDeviceDescription( index ) );
	BASS_GetInfo( &info );
	SOUND_PRINT_STRING( "\tlatency : %dms", info.latency );
	SOUND_PRINT_STRING( "\tminbuf : %dms", info.minbuf );
	SOUND_PRINT_STRING( "\tds version : %d (effects %s)", info.dsver, info.dsver < 8 ? "disabled": "enabled" );

	// default buffer size = update period + 'minbuf'
	buflen = BASS_SetConfig( BASS_CONFIG_BUFFER, 500 + info.minbuf );
	return TRUE;
}

BOOL	b_sound_open = FALSE;

void	sound_init()
{
	if( !b_sound_open )
	{
		ds_enum();
		switch( s_sound_lib_name )
		{
		case SND_LIB_DS:
			ds_init();
			b_sound_open = (p_ds != NULL);
			break;
		case SND_LIB_BASS:
			b_sound_open = snd_bass_open();
			break;
		case SND_LIB_BASS_ASIO:
			b_sound_open = snd_bass_asio_open();
			break;
		}
	}
	if( b_sound_open )
		d_sound_lib_name = s_sound_lib_name;
}

void	sound_free()
{
	if( b_sound_open )
	{
		// close opened sound library, don't close the one selected in the interface
		switch( d_sound_lib_name )
		{
		case SND_LIB_DS :	
					ds_free();
					break;
		case SND_LIB_BASS :	
					snd_bass_free();
					break;
		case SND_LIB_BASS_ASIO :
					snd_bass_asio_free();
					break;
		}
		b_sound_open = FALSE;
	}
}

AAA_ERR	bass_load_from_file( HSTREAM* stream, char* filename, BOOL	b_loop )
{
	HSTREAM stream_handle;
	AAA_ERR	retcode;
	DWORD flags;
	retcode = ERR_ANY;

	if( b_loop )
		flags = BASS_SAMPLE_LOOP;
	else
		flags = 0;
	if( stream_handle = BASS_StreamCreateFile( FALSE, filename, 0, 0, flags ) )
	{
		*stream = stream_handle;
		retcode = AAA_OK;
	}
	else
	{
		INT err_code = BASS_ErrorGetCode();
		DBG_PRINT_STRING( "BASS, bass_load can't open sound file, %s.", bass_get_str_error( err_code ) );
	}
	return retcode;
}

//todo check the errors better
//	and close stuff
AAA_ERR ds_load_from_file( LPDIRECTSOUNDBUFFER *buffer, char* filename, UINT32* p_byte_per_sec )
{
AAA_ERR	retcode = ERR_ANY;

//hack
	if( p_ds)
	{
		//open a wav file
		UINT32	u32_return;
		HMMIO wavefile;

		wavefile = mmioOpen(filename,0,MMIO_READ|MMIO_ALLOCBUF);
		if( wavefile==NULL )
		{
			DBG_PRINT_STRING("ds_load can't mmioOpen");
			goto exit;
		}
		//find wave data
		MMCKINFO parent;
		memset(&parent,0,sizeof(MMCKINFO));
		parent.fccType = mmioFOURCC('W','A','V','E');
		mmioDescend(wavefile,&parent,0,MMIO_FINDRIFF);

		//find fmt data
		MMCKINFO child;
		memset(&child,0,sizeof(MMCKINFO));
		child.fccType = mmioFOURCC('f','m','t',' ');
		mmioDescend(wavefile,&child,&parent,0);

		//read the format
		WAVEFORMATEX wavefmt;
		mmioRead(wavefile,(char*)&wavefmt,sizeof(wavefmt));
		if(wavefmt.wFormatTag != WAVE_FORMAT_PCM)
		{
			BOX_ERR("ds_load mmioRead");
			goto exit;
		}

		//find the wave data chunk
		mmioAscend(wavefile,&child,0);
		child.ckid = mmioFOURCC('d','a','t','a');
		mmioDescend(wavefile,&child,&parent,MMIO_FINDCHUNK);

		//create a directsound buffer to hold wave data
		DSBUFFERDESC bufdesc;
		memset(&bufdesc,0,sizeof(DSBUFFERDESC));
		bufdesc.dwSize = sizeof(DSBUFFERDESC);
		bufdesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_STATIC ;
		bufdesc.dwBufferBytes = child.cksize;
		bufdesc.lpwfxFormat = &wavefmt;
		if( p_byte_per_sec )
			*p_byte_per_sec = wavefmt.nAvgBytesPerSec;

		u32_return = p_ds->CreateSoundBuffer( &bufdesc, &(*buffer), NULL);
		if( u32_return != DS_OK)
		{
			ds_err_show( u32_return, "in ds_load_from_file() CreateSoundBuffer()");
			goto exit;
		}
		else
		{
			//write wave data to directsound buffer you just created
			void *write1=0,*write2=0;
			unsigned long length1,length2;
			INT32	len;
			(*buffer)->Lock(0,child.cksize,&write1,&length1,&write2,&length2,DSBLOCK_ENTIREBUFFER );
			if(write1>0)
			{
				len = mmioRead(wavefile,(char*)write1,length1);
			}
			if(write2>0)
				mmioRead(wavefile,(char*)write2,length2);
			(*buffer)->Unlock(write1,length1,write2,length2);

			//close the wavefile, don't need it anymore, it's in the directsound buffer now
			mmioClose(wavefile,0);
			retcode = AAA_OK;
		}
	}
exit:
	return retcode;
}

/*
LRESULT CALLBACK WndFunc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	HDC         hdc;
	PAINTSTRUCT ps;
	DWORD freq,stat; //for directsound 

	switch (message)
	{
		case WM_PAINT:
			hdc = BeginPaint (hwnd, &ps);
			EndPaint (hwnd, &ps);
			break;

		case WM_KEYDOWN:
			switch(wParam)
			{
				case VK_ESCAPE:
					PostQuitMessage(0);
					break;

				case 0x31: //1 key - dsbuffer1 and duplicate1 use the same buffer, so there
						   //is no memory wasted.  if one is playing it tries the other.
						   //at most you can hear 2 at a time.
					dsbuffer1->GetStatus(&stat);
					if(stat != DSBSTATUS_PLAYING )
					{
						dsbuffer1->Play(0,0,0);
						break;
					}
					duplicate1->GetStatus(&stat);
					if(stat != DSBSTATUS_PLAYING )
					{
						duplicate1->Play(0,0,0);
						break;
					}
					break;

				case 0x32: //2 key - play dsbuffer2 sound 
					dsbuffer2->SetCurrentPosition(0);
					dsbuffer2->Play(0,0,0);
					break;

				case 0x33: //3 key - play dsbuffer3 sound 
					dsbuffer3->SetCurrentPosition(0);
					dsbuffer3->Play(0,0,DSBPLAY_LOOPING);
					break;
				case VK_SPACE: //stop dsbuffer3
					dsbuffer3->Stop();
					break;
				case VK_RIGHT:
				case VK_UP: //increase pitch dsbuffer3
					dsbuffer3->GetFrequency(&freq);
					dsbuffer3->SetFrequency(freq+500);
					break;
				case VK_LEFT:
				case VK_DOWN: //decrease pitch dsbuffer3
					dsbuffer3->GetFrequency(&freq);
					dsbuffer3->SetFrequency(freq-500);
					break;
			}
			break;

		case WM_DESTROY:
			OnDestroy();
			PostQuitMessage (0);
			break;
	}

	return DefWindowProc (hwnd, message, wParam, lParam);
}

//---------------------------------------------------------------------------//


void Show_Buffers_Playing(int line)
{
	DWORD stat_dsbuffer1;
	DWORD stat_duplicate1;
	DWORD stat_dsbuffer2;
	DWORD stat_dsbuffer3;

	//check play status of directsound buffers, display text red if playing, grey if not
	dsbuffer1->GetStatus(&stat_dsbuffer1);
	if(stat_dsbuffer1 == DSBSTATUS_PLAYING)
		Display_Text("dsbuffer1",0,line,255,0,0);
	else
		Display_Text("dsbuffer1",0,line,128,128,128);

	duplicate1->GetStatus(&stat_duplicate1);
	if(stat_duplicate1 == DSBSTATUS_PLAYING)
		Display_Text("duplicate1",0,line+16,255,0,0);
	else
		Display_Text("duplicate1",0,line+16,128,128,128);

	dsbuffer2->GetStatus(&stat_dsbuffer2);
	if(stat_dsbuffer2 == DSBSTATUS_PLAYING)
		Display_Text("dsbuffer2",0,line+32,255,0,0);
	else
		Display_Text("dsbuffer2",0,line+32,128,128,128);

	dsbuffer3->GetStatus(&stat_dsbuffer3);
	if(stat_dsbuffer3 == DSBSTATUS_LOOPING)
		Display_Text("dsbuffer3",0,line+48,255,0,0);
	else
		Display_Text("dsbuffer3",0,line+48,128,128,128);

}
*/

