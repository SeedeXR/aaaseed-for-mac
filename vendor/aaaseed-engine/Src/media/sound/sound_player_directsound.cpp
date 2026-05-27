#include "sound_player_directsound.h"
#include "sound_directsound.h"


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
	b_enable_ = FALSE;
	p_ds_ = NULL;
	buffer_ = NULL;
	byte_per_sec_ = 0;
	b_playing_ = FALSE;
	b_loop_ = FALSE;
	sample_rate_ = 0;
	sample_bits_ = 0;
	duration_ = 0;
	volume_ = 0;
	pan_ = 0.5;
	channel_nb_ = 0;

}

void	c_sound_player_ds::deinit()
{
}

void	c_sound_player_ds::update()
{
}

AAA_ERR		c_sound_player_ds::load_from_file( const CHAR* filename )
{
	AAA_ERR	retcode = ERR_ANY;

	//hack
	if( p_ds_ )
		{
		//open a wav file
		UINT32	u32_return;
		HMMIO wavefile;

		wavefile = mmioOpen( (char*)filename, 0, MMIO_READ | MMIO_ALLOCBUF );
		if( wavefile == NULL )
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
			BOX_ERR( "ds_load mmioRead" );
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
		byte_per_sec_ = wavefmt.nAvgBytesPerSec;
		sample_bits_ = c_sound::sample_bits( wavefmt.wBitsPerSample );
		channel_nb_ = wavefmt.nChannels;
		sample_rate_ = c_sound::sample_rate( wavefmt.nSamplesPerSec );

		u32_return = p_ds_->CreateSoundBuffer( &bufdesc, &buffer_, NULL );
		if( u32_return != DS_OK)
			{
			ds_err_show( u32_return, "in ds_load_from_file() CreateSoundBuffer()" );
			goto exit;
			}
		else
			{
			//write wave data to directsound buffer you just created
			void *write1 = 0,*write2 = 0;
			unsigned long length1,length2;
			INT32	len;
			buffer_->Lock( 0, child.cksize, &write1, &length1, &write2, &length2, DSBLOCK_ENTIREBUFFER );
			if( write1 > 0 )
				{
				len = mmioRead( wavefile,(char*)write1, length1 );
				}
			if( write2 > 0 )
				mmioRead( wavefile, (char*)write2, length2 );
			buffer_->Unlock( write1, length1, write2, length2 );

			//close the wavefile, don't need it anymore, it's in the directsound buffer now
			mmioClose( wavefile, 0 );

			// now get the volume and panning, duration, etc...
//			buffer_->GetFrequency( (DWORD*)&sample_rate_ );

			get_pan_low();
			get_volume_low();

			duration_ = (REAL)child.cksize / (REAL)byte_per_sec_;


			b_enable_ = TRUE;
			retcode = AAA_OK;
			}
		}
	exit:
	return retcode;
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
	if( buffer_ )
		{
		buffer_->SetVolume( ( volume - 1. ) * 10000 );
		// check the volume
		get_volume_low();
		}
}

void	c_sound_player_ds::get_volume_low()
{
	if( buffer_ )
		{
		LONG volume;
		buffer_->GetVolume( &volume );
		volume_ = ( volume / 10000. ) + 1.;
		}
}

//		error_check( ds_buf->SetVolume( ( volume_ui - 1. ) * 10000 ), "SetVolume" );
//		volume = volume_ui;
//		}
//	if( pan_ui != pan )
//		{
void	c_sound_player_ds::set_pan( REAL pan )
{
	if( buffer_ )
		{
		buffer_->SetPan( interpolate( DSBPAN_LEFT, DSBPAN_RIGHT, ( pan - 0.5 ) * 20000. ) );
		// check the pan
		get_pan_low();
		}
}

void	c_sound_player_ds::get_pan_low()
{
	if( buffer_ )
		{
		LONG	pan;
		buffer_->GetPan( &pan );
		pan_ = ( pan / 20000. ) + 0.5;
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
	if( buffer_ )
		{
		buffer_->SetCurrentPosition( (UINT32)( (DOUBLE)byte_per_sec_ * pos ) );
		}
	//		error_check( ds_buf->SetCurrentPosition( (UINT32)(((DOUBLE)byte_per_sec)*jump_to) ), "SetCurrentPosition" );
	//		jump_to = 0.;
}

REAL	c_sound_player_ds::get_position()
{
	if( buffer_ )
		{
		DWORD	dummy;
		UINT32	position;
		buffer_->GetCurrentPosition( (LPDWORD)&position, &dummy );
		REAL pos;
		pos = ( (DOUBLE)position ) / ( (DOUBLE)byte_per_sec_ );
		return pos;
		}
	//	error_check( ds_buf->GetCurrentPosition( (LPDWORD)&position, &dummy), "GetCurrentPosition" );
	//	time = ((DOUBLE)position)/((DOUBLE)byte_per_sec);
	return 0.0;
}

INT32	c_sound_player_ds::get_status()
{
	DWORD	status;
	status = STATUS_ERROR;
	if( buffer_ )
		{
		buffer_->GetStatus( &status );
//		status = STATUS_STOP;
		if( status & DSBSTATUS_PLAYING )
			status = STATUS_PLAYING;
		if( status & DSBSTATUS_LOOPING )
			status = STATUS_LOOPING;
		if( status & DSBSTATUS_BUFFERLOST || status & DSBSTATUS_TERMINATED )
			status = STATUS_ERROR;
		}
	return status;
}

void	c_sound_player_ds::stop()
{
	if( buffer_ )
		{
		buffer_->Stop();
//		buffer_->SetCurrentPosition( 0 );
		}
}

void	c_sound_player_ds::pause()
{
	if( buffer_ )
		buffer_->Stop();
}

void	c_sound_player_ds::play()
{
	if( buffer_ )
		buffer_->Play( 0, 0, b_loop_ ? DSBPLAY_LOOPING : 0 );
}
