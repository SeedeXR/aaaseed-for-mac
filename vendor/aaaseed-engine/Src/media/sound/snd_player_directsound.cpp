
#undef WIN32_LEAN_AND_MEAN

#include "snd_player_directsound.h"
#include "sound_directsound.h"
#include "snd_master.h"
#ifndef AAA_SYSTEMUTILS_H
#	include "system/shared/SystemUtils.h"
#endif


c_sound_player_ds::c_sound_player_ds()
{
	init();
}

c_sound_player_ds::~c_sound_player_ds()
{
	deinit();
}

void	c_sound_player_ds::init()
{
	_b_enabled = false;
	_p_ds = nullptr;
	_buffer = nullptr;
	_byte_per_sec = 0;
	_b_playing = false;
	_b_loop = false;
	_sample_rate = 0;
	_sample_bits = 0;
	_duration = 0;
	_volume = 0;
	_pan = 0;
	_channel_nb = 0;

}

void	c_sound_player_ds::deinit()
{
}

void	c_sound_player_ds::update()
{
}

AAA_ERR		c_sound_player_ds::load_from_file( o_str CONST & filename )
{
	//hack
	if( !_p_ds )
		return ERR_ANY;

	AAA_ERR	retcode = ERR_ANY;
	//open a wav file
	UINT32	u32_return;
	HMMIO wavefile;

	wchar_t * txt = sysutils::utf8_to_unicode( filename.get() );
	wavefile = mmioOpenW( txt, 0, MMIO_READ | MMIO_ALLOCBUF );
	sysutils::free_str_tmp( txt );
	if( !wavefile )
	{
		DBG_PRINT_STRING( "ds_load can't mmioOpen" );
		goto exit;
	}
	//find wave data
	MMCKINFO parent;
	memset( &parent, 0, sizeof( MMCKINFO ) );
	parent.fccType = mmioFOURCC( 'W','A','V','E' );
	mmioDescend( wavefile, &parent, 0, MMIO_FINDRIFF );

	//find fmt data
	MMCKINFO child;
	memset( &child, 0, sizeof( MMCKINFO ) );
	child.fccType = mmioFOURCC( 'f','m','t',' ' );
	mmioDescend( wavefile, &child, &parent, 0 );

	//read the format
	WAVEFORMATEX wavefmt;
	mmioRead( wavefile,(CHAR*)&wavefmt, sizeof( wavefmt ));
	if(wavefmt.wFormatTag != WAVE_FORMAT_PCM)
	{
//		BOX_ERR( "ds_load mmioRead" );
		goto exit;
	}

	//find the wave data chunk
	mmioAscend( wavefile, &child, 0 );
	child.ckid = mmioFOURCC( 'd','a','t','a' );
	mmioDescend( wavefile, &child, &parent, MMIO_FINDCHUNK);

	//create a directsound buffer to hold wave data
	DSBUFFERDESC bufdesc;
	memset( &bufdesc, 0, sizeof(DSBUFFERDESC) );
	bufdesc.dwSize = sizeof(DSBUFFERDESC);
	bufdesc.dwFlags = DSBCAPS_CTRLPAN | DSBCAPS_CTRLVOLUME | DSBCAPS_CTRLFREQUENCY | DSBCAPS_STATIC ;
	bufdesc.dwBufferBytes = child.cksize;
	bufdesc.lpwfxFormat = &wavefmt;
//		if ( p_byte_per_sec_ )
//			*p_byte_per_sec_ = wavefmt.nAvgBytesPerSec;
	_byte_per_sec = wavefmt.nAvgBytesPerSec;
	_sample_bits = snd::sample_bits( wavefmt.wBitsPerSample );
	_channel_nb = wavefmt.nChannels;
	_sample_rate = snd::sample_rate( wavefmt.nSamplesPerSec );

	u32_return = _p_ds->CreateSoundBuffer( &bufdesc, &_buffer, nullptr );
	if( u32_return != DS_OK )
	{
		ds_err_show( u32_return, "in ds_load_from_file() CreateSoundBuffer()" );
		goto exit;
	}
	else
	{
		//write wave data to directsound buffer you just created
		void*	write1 = 0;
//			void*	write2 = 0;
		unsigned long length1;
//			unsigned long length2;
		INT32	len;
		_buffer->Lock( 0, child.cksize, &write1, &length1, nullptr, nullptr, DSBLOCK_ENTIREBUFFER );
//			buffer_->Lock( 0, child.cksize, &write1, &length1, &write2, &length2, DSBLOCK_ENTIREBUFFER );
		if( write1 > (void*)0 )
		{
			len = mmioRead( wavefile,(char*)write1, length1 );
		}
//			if( write2 > 0 )
//				mmioRead( wavefile, (char*)write2, length2 );
		_buffer->Unlock( write1, length1, nullptr, 0 );
//			buffer_->Unlock( write1, length1, write2, length2 );

		//close the wavefile, don't need it anymore, it's in the directsound buffer now
		mmioClose( wavefile, 0 );

		// now get the volume and panning, duration, etc...
//			buffer_->GetFrequency( (DWORD*)&sample_rate_ );

		get_pan_low();
		get_volume_low();

		_duration = (REAL)child.cksize / (REAL)_byte_per_sec;


		_b_enabled = true;
		retcode = AAA_OK;
	}
exit:
	return retcode;
}

void	c_sound_player_ds::set_frequency( REAL freq )
{
	if( _freq != freq )
	{
		//error_check( _buffer->SetFrequency( (UINT32)(((DOUBLE)44100)*freq) ), "SetFrequency" );
		_buffer->SetFrequency( (UINT32)(((DOUBLE)44100)*freq) );	//hack use snd::SAMPLE_PER_SEC_DEF ot a dynamic value
		_freq = freq;
	}
}

//		error_check( ds_buf->GetFrequency( (DWORD *)&freq_int ), "GetFrequency" );
//	}
//if( ds_buf )
//	{
//	DWORD	dummy;
//	error_check( ds_buf->GetCurrentPosition( (LPDWORD)&position, &dummy), "GetCurrentPosition" );
//	time = ((DOUBLE)position)/((DOUBLE)byte_per_sec);
//	if( freq_last != freq )
//		{
//		error_check( ds_buf->SetFrequency( (UINT32)(((DOUBLE)freq_int)*freq) ), "SetFrequency" );
//		freq_last = freq;
//		}
//	if ( jump_to != 0.)
//		{
//		error_check( ds_buf->SetCurrentPosition( (UINT32)(((DOUBLE)byte_per_sec)*jump_to) ), "SetCurrentPosition" );
//		jump_to = 0.;
//		}
//	}
////todo opt
//if( ds_buf )
//	{
//	DWORD	stat;
//	error_check( ds_buf->GetStatus( &stat ), "GetStatus" );
//	b_play = ( stat & DSBSTATUS_PLAYING );// || ( stat == DSBSTATUS_LOOPING );

//	if( volume_ui != volume )
//		{
void	c_sound_player_ds::set_volume( REAL volume )
{
	if( _buffer )
	{
		_buffer->SetVolume( LONG( ( volume - 1. ) * 10000 ) );
		// check the volume
		get_volume_low();
	}
}

void	c_sound_player_ds::get_volume_low()
{
	if( _buffer )
	{
		LONG	volume;
		_buffer->GetVolume( &volume );
		_volume = REAL( ( volume / 10000. ) + 1. );
	}
}

//		error_check( ds_buf->SetVolume( ( volume_ui - 1. ) * 10000 ), "SetVolume" );
//		volume = volume_ui;
//		}
//	if( pan_ui != pan )
//		{
void	c_sound_player_ds::set_pan( REAL pan )
{
	if( _buffer )
	{
		_buffer->SetPan( interpolate( DSBPAN_LEFT, DSBPAN_RIGHT, pan ) );
//		buffer_->SetPan( interpolate( DSBPAN_LEFT, DSBPAN_RIGHT, ( pan - 0.5 ) * 20000. ) );
		// check the pan
		get_pan_low();
	}
}

void	c_sound_player_ds::get_pan_low()
{
	if( _buffer )
	{
		LONG	pan;
		_buffer->GetPan( &pan );
		_pan = REAL( ( pan / 20000. ) * 2. );
	}

}
//		error_check( ds_buf->SetPan( interpolate( DSBPAN_LEFT, DSBPAN_RIGHT, pan_ui) ), "SetPan" );
//		pan = pan_ui;
//		}
//	if ( b_play )
//		{
//		if ( b_trig_stop )
//			{


void	c_sound_player_ds::set_position( REAL pos )
{
	if( _buffer )
	{	//DSBFREQUENCY_ORIGINAL
		_buffer->SetCurrentPosition( (UINT32)( (DOUBLE)_byte_per_sec * pos ) );
	}
	//		error_check( ds_buf->SetCurrentPosition( (UINT32)(((DOUBLE)byte_per_sec)*jump_to) ), "SetCurrentPosition" );
	//		jump_to = 0.;
}

REAL	c_sound_player_ds::get_position()
{
	if( _buffer )
	{
		DWORD	dummy;
		UINT32	position;
		_buffer->GetCurrentPosition( (LPDWORD)&position, &dummy );
		REAL	pos;
		pos = REAL( ( (DOUBLE)position ) / ( (DOUBLE)_byte_per_sec ) );
		return pos;
	}
	//	error_check( ds_buf->GetCurrentPosition( (LPDWORD)&position, &dummy), "GetCurrentPosition" );
	//	time = ((DOUBLE)position)/((DOUBLE)byte_per_sec);
	return 0.0;
}

INT32	c_sound_player_ds::get_status()
{
	INT32	ret_status = c_sound::STATUS_ERROR;
	if( _buffer )
	{
		DWORD	status;
		_buffer->GetStatus( &status );
//		status = STATUS_STOP;
		ret_status = c_sound::STATUS_STOP;
		if( status & DSBSTATUS_PLAYING )
			ret_status = c_sound::STATUS_PLAYING;
		if( status & DSBSTATUS_LOOPING )
			ret_status = c_sound::STATUS_LOOPING;
		if( status & DSBSTATUS_BUFFERLOST || status & DSBSTATUS_TERMINATED )
			ret_status = c_sound::STATUS_ERROR;
	}
	return ret_status;
}

void	c_sound_player_ds::stop()
{
	if( _buffer )
	{
		_buffer->Stop();
//		buffer_->SetCurrentPosition( 0 );
	}
}

void	c_sound_player_ds::pause()
{
	if( _buffer )
		_buffer->Stop();
}

void	c_sound_player_ds::play()
{
	if( _buffer )
	{
		HRESULT	hr;
		hr = _buffer->Play( 0, 0, _b_loop ? DSBPLAY_LOOPING : 0 );
		if( hr != DS_OK )
		{
			if( hr == E_OUTOFMEMORY )
			{
				ERR_PRINT_STRING( "Could not start wave playback in sound_player_ds : E_OUTOFMEMORY" );
			}
			else
				ERR_PRINT_STRING( "Could not start wave playback in sound_player_ds" );
		}
	}
}
