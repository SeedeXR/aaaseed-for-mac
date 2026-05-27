
#ifdef AAA_SOUND_DIRECTSOUND_H
#error "SOUND_DIRECTSOUND_H included more than once."
#endif
#define AAA_SOUND_DIRECTSOUND_H 1


#ifndef AAA_SOUND_NEW_H
#	include "sound_new.h"
#endif
#ifdef	WIN32
#	ifndef __DSOUND_INCLUDED__
#		include <dsound.h>
#	endif
#endif

void	ds_err_show( UINT32 err_code, C_PCHAR_C mess );

class	c_sound_input_ds;
class	c_sound_player_ds;

class c_sound_directsound final : public c_sound
{
	FACTORY_DECLARE( c_sound_directsound, c_sound )
private :

	LPDIRECTSOUND			_p_ds[ OUTPUT_NB_MAX ];
public:
	bool					_b_enum_input;

	std::vector<GUID>		_sound_device_in_guid;
	std::vector<GUID>		_sound_device_out_guid;
//	std::vector<o_str*>		sound_input_device_name_;
//	std::vector<o_str*>		sound_output_device_name_;
//	INT32					_sound_device_index;


//	void					ds_enum();


	C_PCHAR_C				get_str_error( UINT32 err );

protected:

public:
	virtual	void			param_init_pt();

	virtual	void			init_output( INT32 index );
//	virtual	void			deinit_output();

	virtual	void			close();
	virtual	LPGUID			get_out_guid( UINT32 device );
	virtual	LPGUID			get_in_guid( UINT32 device );
	
//	virtual	o_str*	get_name( INT32 device );

	virtual c_sound_input*	create_input_new();
	virtual c_sound_player*	create_player_new();

	virtual INT32			enumerate_device_out();
	virtual INT32			enumerate_device_in();
};




