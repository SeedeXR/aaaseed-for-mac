
#ifdef AAA_SOUND_BASS_H
#error "SOUND_BASS_H included more than once."
#endif
#define AAA_SOUND_BASS_H 1


#ifndef AAA_SOUND_NEW_H
#	include "sound_new.h"
#endif

class	c_sound_input_bass;
class	c_sound_player_bass;
class	c_sound_bass_asio;
class	c_sound_input_bass_asio;


class	c_sound_bass final : public c_sound
{
	friend c_sound_input_bass_asio;
	friend c_sound_bass_asio;
	friend c_sound_input_bass;
	friend c_sound_player_bass;

	FACTORY_DECLARE( c_sound_bass, c_sound )

protected:
	static	void			report_error(			INT32 h, C_PCHAR_C str_fn );
	static	void			report_error(			C_PCHAR_C str_fn );
	static	C_PCHAR_C		get_error_str_short(	UINT32 err );
	static	C_PCHAR_C		get_error_str(			UINT32 err );

public:
	static	bool			load_lib();
	static	void			unload_lib();

	static	bool			open_lib();
	static	void			close_lib();

	static	C_PCHAR_C		get_input_type(			INT32 s );
private :
//	static void		dump_one_device( BASS_DEVICEINFO& info );
//	INT32			_sound_device_index;
public:
	virtual	void			param_init_pt();

	virtual	void			init_output();

	//	virtual	o_str*	get_name( INT32 device );

	virtual c_sound_input*	create_input_new();
	virtual c_sound_player*	create_player_new();

	virtual	INT32			enumerate_device_out();
	virtual	INT32			enumerate_device_in();

};




