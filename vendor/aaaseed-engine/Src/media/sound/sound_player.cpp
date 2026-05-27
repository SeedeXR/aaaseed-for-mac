// * * * * * * * * * * * * * * * * * * * *
// *  Sound Player Abstract Class        *
// *                                     *
// * Date Created :  15/03/2006          *
// *                                     *
// * Date Modified : 15/03/2006          *
// *                                     *
// * Author :        Franz Hildgen       *
// *                                     *
// * * * * * * * * * * * * * * * * * * * *

#include "sound_player.h"
#ifndef AAA_SOUND_NEW_H
#	include "sound_new.h"
#endif

c_sound_player::c_sound_player()
{
	b_enable_ = FALSE;
	b_playing_ = FALSE;
	b_loop_ = FALSE;
	sample_rate_ = 0;
	sample_bits_ = 0;
	duration_ = 0.;
	volume_ = 0.;
	pan_ = 0.5;
	channel_nb_ = 0;

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

void	c_sound_player::set_volume( REAL volume )
{
}

void	c_sound_player::set_pan( REAL pan )
{
}

void	c_sound_player::set_position( REAL pos )
{
}

REAL	c_sound_player::get_position()
{
	return 0.0;
}

AAA_ERR	c_sound_player::load_from_file( const CHAR* filename )
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
	return STATUS_ERROR;
}