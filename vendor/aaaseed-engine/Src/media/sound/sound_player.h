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

#ifdef AAA_SOUND_PLAYER_H
#error "SOUND_PLAYER_H included more than once."
#endif
#define AAA_SOUND_PLAYER_H 1

#ifndef AAA_ERR_H
#	include "err.h"
#endif
//#ifndef AAA_SOUND_NEW_H
//#	include "sound_new.h"
//#endif


class c_sound_player
{
protected :
	BOOL		b_enable_;

	BOOL		b_playing_;
	BOOL		b_loop_;
	INT32		sample_rate_;
	INT32		sample_bits_;
	REAL		duration_;
	REAL		volume_;
	REAL		pan_;
	INT32		channel_nb_;


public:
//	virtual	const char*	const	get_class_name()	{ return "sound_player"; };
//	virtual	const char*	const	get_fname_ext()		{ return "sound_player"; };

	c_sound_player();
	virtual	~c_sound_player();

	virtual void	init();
	virtual	void	deinit();

	virtual	void	update();

	virtual AAA_ERR	load_from_file( const CHAR* filename );

			REAL	get_volume()			{ return volume_; };
			REAL	get_pan()				{ return pan_; } ;
			INT32	get_sample_rate()		{ return sample_rate_; };
			REAL	get_duration()			{ return duration_; };
			INT32	get_sample_bits()		{ return sample_bits_; };
			INT32	get_channel_nb()		{ return channel_nb_; };


	virtual	void	set_volume( REAL volume );
	virtual	void	set_pan( REAL pan );
	virtual	void	set_position( REAL pos );
	virtual	REAL	get_position();

			void	loop( BOOL b_loop )		{ b_loop_ = b_loop; };

	virtual	void	play();
	virtual	void	pause();
	virtual	void	stop();

	virtual BOOL	is_playing()		{ return b_playing_; };

	virtual	INT32	get_status();

	BOOL	IsEnable()					{ return b_enable_; };

};



