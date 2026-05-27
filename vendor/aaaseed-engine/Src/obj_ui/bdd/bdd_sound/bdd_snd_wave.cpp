#include "bdd_snd_wave.h"

#include "media/sound/snd_player.h"
#ifndef AAA_SOUND_NEW_H
#	include "media/sound/sound_new.h"
#endif

#include "infrastructure/seedfile.h"
#include "media/sound/snd_master.h"


FACTORY_CREATE_PROP_V1( c_bdd_snd_wave, bdd_snd_wave, Sound Wave, bdd_snd_wave, sub_menu="Sound"; );

namespace n_bdd_sound_wave
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 15;
	CONSTEXPR INT32 FILE_INFO_NB	= 3;
	CONSTEXPR INT32 GROUP_NB		= 1;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	FILE_INFO_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_INT32_POS_ZERO(	device_index	)
		PARAM_DEF_SYMBO_LOCKED_PSTR(	status,	c_sound::player_status_str )
		PARAM_DEF_BOOL_OFF(			play			)
		PARAM_DEF_BOOL_OFF(			trig_start		)
		PARAM_DEF_BOOL_OFF(			trig_restart	)
		PARAM_DEF_BOOL_OFF(			trig_stop		)
		PARAM_DEF_BOOL_OFF(			loop			)
	//		{	nullptr,	PARAM_INT32,	"bind_sound",	1, 0,		0, 32,	nullptr, nullptr },
		PARAM_DEF_REAL_ZERO_ONE(	volume			)
		PARAM_DEF_REAL(				pan,			1, .5,	0, 1	)
		PARAM_DEF_REAL_INF(			freq,			.8, 1	)
		PARAM_DEF_INT32(			position,		1, 0,	0, 32	)
//		{	nullptr,	PARAM_INT32,	"byte_per_sec",					1, 0,	0, PARAM_MAX_INT32,					nullptr, nullptr },
		PARAM_DEF_REAL_LOCKED(		duration		)
		PARAM_DEF_REAL_INF(			time,			1, .8	)
		PARAM_DEF_REAL_ZERO(		jump_to			)
		PARAM_DEF_FILENAME(			wave_filename, aaa::file::TYPE_IO_SND_WAVE, 0 )
		PARAM_DEF_GROUP_CLOSED( Info, FILE_INFO_NB )	
			PARAM_DEF_SYMBO_LOCKED_PSTR(	sample_rate,	snd::sample_rate_str	)
			PARAM_DEF_SYMBO_LOCKED_PSTR(	sample_bits,	snd::sample_bits_str	)
			PARAM_DEF_INT32_LOCKED(			channel_nb		)
	};
};

void	c_bdd_snd_wave::param_init_pt()
{
	INT32	h = 0;
	param_set_pt( h, _device_index );
	param_set_pt( h, _status );
	param_set_pt( h, _b_play );
	param_set_pt( h, _b_trig_start_ui );
	param_set_pt( h, _b_trig_restart_ui );
	param_set_pt( h, _b_trig_stop_ui );
	param_set_pt( h, _b_loop_ui );
//	param_set_pt( h, bind_snd );
	param_set_pt( h, _volume_ui );
	param_set_pt( h, _pan_ui );
	param_set_pt( h, _freq );
	param_set_pt( h, _position );
//	param_set_pt( h, byte_per_sec_ );
	param_set_pt( h, _duration );
	param_set_pt( h, _time );
	param_set_pt( h, _jump_to );
	param_set_pt( h, _wave_filename_ui );
	++h;
		param_set_pt( h, _samplerate );
		param_set_pt( h, _bit_nb );
		param_set_pt( h, _channel_nb );

	err_param_init_pt( h );
	//if( b_trig_start )
	//	printf("b_trig_start in parama_ini_pt\n");

}

void c_bdd_snd_wave::init()
{
	param_init_with( n_bdd_sound_wave::param, n_bdd_sound_wave::PARAM_NB_MAX );
//	volume_ = -1.;
//	pan_ = -2.;
}

void c_bdd_snd_wave::param_init()
{
	_status = c_sound::STATUS_UNKNOWN;
//	_b_play = false;
	_b_playing = false;
//	_b_trig_restart_ui = false;
//	ds_buf_ = nullptr;
//	bass_stream_ = nullptr;
//	jump_to_ = 0.;
	_samplerate = 0;
	_bit_nb = 0;
	_channel_nb = 0;
	_duration = 0.0f;
	_b_sound_player_enabled = false;
	_sound_player = nullptr;
}


void c_bdd_snd_wave::alloc()
{
}

void c_bdd_snd_wave::dealloc()
{
	if( _sound_player )
	{
		//	sound_player_->stop();
		//	hack, sound is already deinitialized when we get here
		snd::g_master->delete_player( _sound_player );
		//	delete sound_player_;
		_sound_player = nullptr;
	}
	//	if( C_sound::S_lib == SND_LIB_BASS )
	//	{
	//		if( bass_stream_ )
	//		{
	//			BASS_StreamFree( bass_stream_ );
	//			bass_stream_ = 0;
	//		}
	//	}
}

CONSTRUCTOR_CREATE(c_bdd_snd_wave)
{
	init();
	alloc();
}

c_bdd_snd_wave::~c_bdd_snd_wave()
{
	dealloc();
}

void	c_bdd_snd_wave::set_wave_filename( o_str CONST & filename )
{
	dealloc();
	_wave_filename_ui.set_fname_relative( filename );
}

AAA_ERR	c_bdd_snd_wave::load_do_after( o_str CONST & filename )
{
	set_wave_filename( _wave_filename_ui );
	return AAA_OK;
}

//void	c_bdd_snd_wave::update_ds()
//{
	//if( !ds_buf && !wave_filename.is_empty() )
	//	{
	//	if( ERR( ds_load_from_file(&ds_buf, wave_filename.get(), &byte_per_sec)) )
	//		{
	//		dealloc();
	//		wave_filename.erase();
	//		}
	//	else
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
	//		error_check( ds_buf->SetVolume( ( volume_ui - 1. ) * 10000 ), "SetVolume" );
	//		volume = volume_ui;
	//		}
	//	if( pan_ui != pan )
	//		{
	//		error_check( ds_buf->SetPan( interpolate( DSBPAN_LEFT, DSBPAN_RIGHT, pan_ui) ), "SetPan" );
	//		pan = pan_ui;
	//		}
	//	if ( b_play )
	//		{
	//		if ( b_trig_stop )
	//			{
	//			error_check( ds_buf->Stop(), "Stop");
	//			error_check( ds_buf->SetCurrentPosition( 0 ), "SetCurrentPosition");
	//			b_trig_stop = false;
	//			}
	//		}
	//	else
	//		{
	//		if ( b_trig_start )
	//			{
	//			error_check( ds_buf->SetCurrentPosition( 0 ), "SetCurrentPosition");
	//			error_check( ds_buf->Play(0,0,b_loop?DSBPLAY_LOOPING:0), "Play");
	//			b_trig_start = false;
	//			}
	//		}
	//	}
//}

//void	c_bdd_snd_wave::update_bass()
//{
//	if( !bass_stream && !wave_filename.is_empty() )
//		{
//		if( ERR( bass_load_from_file( &bass_stream, wave_filename.get(), b_loop ) ) )
//			{
//			dealloc();
//			wave_filename.erase();
//			}
//		else
//			{
//			BASS_CHANNELINFO	info;
//			BASS_ChannelGetInfo( bass_stream, &info);
//			freq_int = info.freq;
//			DWORD	length = BASS_ChannelGetLength( bass_stream );
//			duration = BASS_ChannelBytes2Seconds( bass_stream, length);
//			byte_per_sec = length / duration;
//			if( info.flags & BASS_SAMPLE_8BITS )
//				bit_nb = 8;
//			else if( info.flags & BASS_SAMPLE_FLOAT )
//				bit_nb = 32;
//			else
//				bit_nb = 16;
//			frequency = info.freq;
//			channel_nb = info.chans;
//			}
//		}
//	if( bass_stream )
//		{
//		BASS_CHANNELINFO	info;
//
////		BASS_Update();	//hack beyrouth
//		BASS_ChannelGetInfo( bass_stream, &info);
//		freq_int = info.freq;
//		position = BASS_ChannelGetPosition( bass_stream );
//		time = BASS_ChannelBytes2Seconds( bass_stream, position );
//
//		if( freq_last != freq )
//			{
//			if( BASS_ChannelSetAttributes( bass_stream, freq_int*freq, -1, -101 ) )
//				{
//				freq_last = freq;
//				}
//			else
//				{
//				bass_error_report( BASS_ErrorGetCode(), "SetFrequency" );
//				}
//			}
//		if ( jump_to != 0.)
//			{
//			UINT32	pos;
//			pos = BASS_ChannelSeconds2Bytes( bass_stream, jump_to );
//			if( !BASS_ChannelSetPosition( bass_stream, pos ) )
//				{
//				bass_error_report( BASS_ErrorGetCode(), "SetPosition" );
//				}
//			jump_to = 0.;
//			}
////		}
//	//todo opt
////	if( bass_stream )
////		{
//		DWORD	stat;
//		stat = BASS_ChannelIsActive( bass_stream );
//		b_play = ( stat & BASS_ACTIVE_PLAYING );
//
//		if( volume_ui != volume )
//			{
//			if( BASS_ChannelSetAttributes( bass_stream, -1, (INT)(volume_ui*100), -101 ) )
//				{
//				volume = volume_ui;
//				}
//			else
//				{
//				bass_error_report( BASS_ErrorGetCode(), "SetVolume" );
//				}
//			}
//		if( pan_ui != pan )
//			{
//			if( BASS_ChannelSetAttributes( bass_stream, -1, -1, (INT) interpolate( -100, 100, pan_ui) ) )
//				{
//				pan = pan_ui;
//				}
//			else
//				{
//				bass_error_report( BASS_ErrorGetCode(), "SetPan" );
//				}
//			}
//		if ( b_play )
//			{
//			if ( b_trig_stop )
//				{
//				if( !BASS_ChannelStop( bass_stream ) )
//					{
//					bass_error_report( BASS_ErrorGetCode(), "Stop" );
//					}
//				b_trig_stop = false;
//				}
//			}
//		else
//			{
//			if ( b_trig_start )
//				{
//				b_trig_start = false;
//				if ( BASS_ChannelPlay( bass_stream, TRUE ) )
//					{
//					if( b_loop )
//						{
//						BASS_CHANNELINFO info;
//						BASS_ChannelGetInfo( bass_stream, &info ); // get info
//						if( !(info.flags & BASS_SAMPLE_LOOP ) )
//							{
//							info.flags |= BASS_SAMPLE_LOOP; // toggle the looping flag
//							BASS_ChannelSetFlags( bass_stream, info.flags ); // apply the change
//							}
//						}
//					else
//						{
//						BASS_CHANNELINFO info;
//						BASS_ChannelGetInfo( bass_stream, &info ); // get info
//						if( info.flags & BASS_SAMPLE_LOOP )
//							{
//							info.flags ^= BASS_SAMPLE_LOOP; // toggle the looping flag
//							BASS_ChannelSetFlags( bass_stream, info.flags ); // apply the change
//							}
//						}
//					if( BASS_ErrorGetCode() )
//						bass_error_report( BASS_ErrorGetCode(), "SetLoop" );
//					}
//				else
//					{
//					bass_error_report( BASS_ErrorGetCode(), "Play" );
//					}
//				}
//			}
//		}
//}

//void	c_bdd_snd_wave::update_bass_asio()
//{
	// this part is tricky, bass asio is only interesting for input, do we need low latency output
	// todo
	// we can use bass to read file, and bass asio to play it, but this is not the right place to do this
	// low level stuff have to move elsewhere, we need to rewrite the way the sound is handle. One class for input and output for one sound library
	// because for bass asio we need to copy sound data in a callback

//}

void	c_bdd_snd_wave::update()
{
//if( !bass_stream && !wave_filename.is_empty() )
	if( snd::g_master->is_active() && !_wave_filename_ui.is_empty() )
	{
		if( !_sound_player )
		{
			_b_sound_player_enabled = false;
			// get sound player from sound
			_sound_player = snd::g_master->add_player( _device_index );
			if( !_sound_player )
			{
				// could not create sound player
				ERR_PRINT_STRING( "c_bdd_snd_wave, could not get sound player" );
				return;
			}
		}
		//hack this was done in emergency
		if( strcmp( _wave_filename.get(), _wave_filename_ui.get() ) != 0 )
		{
			_sound_player->stop();
			_wave_filename = _wave_filename_ui;
			_sound_player->load_from_file( _wave_filename );
		}		
		if( _sound_player->is_enable() )
		{
			_duration = _sound_player->get_duration();
			_samplerate = _sound_player->get_sample_rate();
			_channel_nb = _sound_player->get_channel_nb();
			_bit_nb = _sound_player->get_sample_bits();
			_b_sound_player_enabled = true;
		}
	}
	if( _b_sound_player_enabled )
	{
		REAL tmp;
		tmp = _volume_ui * snd::g_master->_volume;
		if( tmp != _sound_player->get_volume() )
			_sound_player->set_volume( tmp );
		tmp = CLAMP( _pan_ui +  snd::g_master->_pan, -1, 1 );
		if( tmp  != _sound_player->get_pan() )
			_sound_player->set_pan( tmp );
		if( _b_loop_ui != _sound_player->is_loop() )
			_sound_player->set_loop( _b_loop_ui );
		if( _freq != _sound_player->get_frequency() )
		{
			_sound_player->set_frequency( _freq );
		}
		_status = _sound_player->get_status();
		if( _b_playing && _status == c_sound::STATUS_STOP )
		{
			// file as finish playing
			_b_play = false;
			_b_playing = false;
		}
		if ( _jump_to != 0.)
		{
			_sound_player->set_position( _jump_to );
			_jump_to = 0.;
		}

		if( _b_play )
		{
			if( ( _status != c_sound::STATUS_PLAYING ) && ( _status != c_sound::STATUS_LOOPING ) )
			{
				_sound_player->play();
				_b_playing = true;
			}
		}
		else
		{
			if( ( _status == c_sound::STATUS_PLAYING ) || ( _status == c_sound::STATUS_LOOPING ) )
			{
				_sound_player->stop();
			}
		}
		if( _b_trig_stop_ui )
		{
			_sound_player->stop();
			_b_trig_stop_ui = false;
			_b_play = false;
		}
		if( _b_trig_start_ui )
		{
			_b_trig_start_ui = false;
			_sound_player->play();
			_b_play = true;
			_b_playing = true;
		}
		if( _b_trig_restart_ui )
		{
			_b_trig_restart_ui = false;
			_sound_player->stop();
			_sound_player->set_position( 0.0 );
			_sound_player->play();
			_b_playing = true;
		}
		_status = _sound_player->get_status();
		_time = _sound_player->get_position();
	}

}

//void	c_bdd_snd_wave::ds_error_report( INT32	h, char* str_fn )
//{
////	ERR_PRINT_STRING( "bdd_snd_wave %s error %s.", str_fn, ds_get_str_error( h ) );
//}
//
//void	c_bdd_snd_wave::bass_error_report( INT32 h, char* str_fn )
//{
////	ERR_PRINT_STRING( "bdd_snd_wave %s error %s.", str_fn, bass_get_str_error( h ) );
//}

void	c_bdd_snd_wave::draw()
{
}
