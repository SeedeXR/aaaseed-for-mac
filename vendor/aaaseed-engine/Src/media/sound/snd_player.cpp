#include "snd_player.h"
#ifndef AAA_SOUND_NEW_H
#	include "sound_new.h"
#endif


c_sound_player::c_sound_player()
{
	_b_enabled = false;
	_b_playing = false;
	_b_loop = false;
	_sample_rate = 0;
	_sample_bits = 0;
	_duration = 0.;
	_volume = 0.;
	_pan = 0.;
	_channel_nb = 0;
}

c_sound_player::~c_sound_player()
{
	deinit();
}

void	c_sound_player::init()
{
}

void	c_sound_player::deinit()
{
}

void	c_sound_player::update()
{
}

void	c_sound_player::set_position( REAL pos )
{
}

REAL	c_sound_player::get_position()
{
	return 0.0;
}

AAA_ERR	c_sound_player::load_from_file( o_str CONST & filename )
{
	return ERR_ANY;
}

void	c_sound_player::play()
{
}	

void	c_sound_player::pause()
{
}	

void	c_sound_player::stop()
{
}

INT32	c_sound_player::get_status()
{
	return c_sound::STATUS_ERROR;
}