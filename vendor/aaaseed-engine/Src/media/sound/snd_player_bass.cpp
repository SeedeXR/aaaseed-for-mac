#include "snd_player_bass.h"
#include "sound_bass.h"
#include "snd_master.h"

c_sound_player_bass::c_sound_player_bass()
:_byte_per_sec(0)
,_bass_stream (0)
{
	init();
}

c_sound_player_bass::~c_sound_player_bass()
{
	deinit();
}

void	c_sound_player_bass::init()
{
	_b_enabled = false;
////	buffer_ = nullptr;
//	_byte_per_sec = 0;
	_b_playing = false;
	_b_loop = false;
	_sample_rate = 0;
	_sample_bits = 0;
	_duration = 0;
	_volume = 0;
	_pan = 0;
	_channel_nb = 0;
//	_bass_stream = 0;
}

void	c_sound_player_bass::deinit()
{
}

void	c_sound_player_bass::update()
{
}

AAA_ERR		c_sound_player_bass::load_from_file( o_str CONST & filename )
{
	AAA_ERR	retcode = ERR_ANY;
	if( _bass_stream )
	{
		dll_bass.BASS_StreamFree( _bass_stream );
		_bass_stream = 0;
		_b_enabled = false;
	}

	_bass_stream = dll_bass.BASS_StreamCreateFile( FALSE, filename.get(), 0, 0, 0 );
	if( _bass_stream == 0 )
	{
		c_sound_bass::report_error( dll_bass.BASS_ErrorGetCode(), "c_sound_player_bass::open() " );
		return retcode;
	}
	else
	{
		_duration = (REAL) dll_bass.BASS_ChannelBytes2Seconds( _bass_stream, dll_bass.BASS_ChannelGetLength( _bass_stream, BASS_POS_BYTE ) ); // the time length
//		byte_per_sec_ = wavefmt.nAvgBytesPerSec;
//		sample_bits_ = c_sound::sample_bits( wavefmt.wBitsPerSample );
//		channel_nb_ = wavefmt.nChannels;
//		sample_rate_ = c_sound::sample_rate( wavefmt.nSamplesPerSec );

		BASS_CHANNELINFO	info;

		if( !dll_bass.BASS_ChannelGetInfo( _bass_stream, &info ) )
		{
			c_sound_bass::report_error( __FUNCTION__ );
		}
		_channel_nb = info.chans;
//		freq_ = info.freq;
		_sample_bits = snd::sample_bits( info.origres );
		_sample_rate = snd::sample_rate( info.freq );

		float	freq;
		//REAL	volume;
		//REAL	pan;
		dll_bass.BASS_ChannelGetAttribute( _bass_stream, BASS_ATTRIB_FREQ, &freq );
		//BASS_ChannelGetAttribute( _bass_stream, BASS_ATTRIB_PAN, &pan );
		//BASS_ChannelGetAttribute( _bass_stream, BASS_ATTRIB_VOL, &volume );
		
		get_volume_low();
		get_pan_low();
//		volume_ = volume;
//		pan_ = pan;
		_b_enabled = true;
		return TRUE;
	}
	return FALSE;
}

void	c_sound_player_bass::set_volume( REAL volume )
{
	if( _b_enabled )
	{
		//BASS_ChannelSetAttribute( _bass_stream, -1, (int) ( volume * 100 ), -101 );
		dll_bass.BASS_ChannelSetAttribute( _bass_stream, BASS_ATTRIB_VOL, volume );
		get_volume_low();
	}
}

void	c_sound_player_bass::get_volume_low()
{
	if( _b_enabled )
	{
		float	volume;
		dll_bass.BASS_ChannelGetAttribute( _bass_stream, BASS_ATTRIB_VOL, &volume );
		_volume = (REAL)volume;
	}
}

void	c_sound_player_bass::set_pan( REAL pan )
{
	if( _b_enabled )
	{
		dll_bass.BASS_ChannelSetAttribute( _bass_stream, BASS_ATTRIB_PAN, pan );
		get_pan_low();
	}

}
// TODO	: hack bug, why + for the panning in the set and get!!!!
void	c_sound_player_bass::get_pan_low()
{
	if( _b_enabled )
	{
		float pan;	//todo perhaps change _pan type
		dll_bass.BASS_ChannelGetAttribute( _bass_stream, BASS_ATTRIB_PAN, &pan );
		_pan = pan;
	}

}

void	c_sound_player_bass::set_position( REAL pos )
{
	if( _b_enabled )
	{
		//todo check for error
		dll_bass.BASS_ChannelSetPosition( _bass_stream, dll_bass.BASS_ChannelSeconds2Bytes( _bass_stream, pos ), BASS_POS_BYTE );
	}
}

REAL	c_sound_player_bass::get_position()
{
	if( _b_enabled )
	{
	//	QWORD	pos;
		REAL	pos = (REAL) dll_bass.BASS_ChannelBytes2Seconds( _bass_stream, dll_bass.BASS_ChannelGetPosition( _bass_stream, BASS_POS_BYTE ) );
		return pos;
	}
	return 0.0;
}

INT32	c_sound_player_bass::get_status()
{
	INT32	ret_status;
	ret_status = c_sound::STATUS_ERROR;

	if( _b_enabled )
	{
		DWORD status;
		status = dll_bass.BASS_ChannelIsActive( _bass_stream );
		switch( status )
		{
		case BASS_ACTIVE_STOPPED :	ret_status = c_sound::STATUS_STOP;		break;
		case BASS_ACTIVE_PLAYING :	ret_status = c_sound::STATUS_PLAYING;	break;
		case BASS_ACTIVE_PAUSED :	ret_status = c_sound::STATUS_PAUSED;	break;
		case BASS_ACTIVE_STALLED :	ret_status = c_sound::STATUS_ERROR;		break;
		default:					ret_status = c_sound::STATUS_UNKNOWN;	break;
		}
	}
	return ret_status;
}

void	c_sound_player_bass::stop()
{
	if( _b_enabled )
	{
		// todo check error
		dll_bass.BASS_ChannelStop( _bass_stream );
	}
		
}

void	c_sound_player_bass::pause()
{
	if( _b_enabled )
	{
		dll_bass.BASS_ChannelPause( _bass_stream );
	}
}

void	c_sound_player_bass::play()
{
	if( _b_enabled )
	{
		dll_bass.BASS_ChannelPlay( _bass_stream, _b_loop ? TRUE : FALSE );
	}
}

void	c_sound_player_bass::set_loop( bool	b_loop )
{
	if( _b_enabled )
	{
		//// todo check for error
		//BASS_CHANNELINFO info;
		//BASS_ChannelGetInfo( _bass_stream, &info ); // get info
		//if( b_loop )
		//{
		//	info.flags &= BASS_SAMPLE_LOOP;
		//}
		//else
		//{
		//	if( info.flags |= BASS_SAMPLE_LOOP )
		//	{
		//		info.flags ^= BASS_SAMPLE_LOOP; 
		//	}
		//}
		_b_loop = b_loop;
		//BASS_ChannelFlags( _bass_stream,info.flags ); // apply the change
		dll_bass.BASS_ChannelFlags( _bass_stream, b_loop ? BASS_SAMPLE_LOOP : 0, BASS_SAMPLE_LOOP);
	}
}
