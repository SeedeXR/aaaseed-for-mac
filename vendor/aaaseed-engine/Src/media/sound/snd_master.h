
#ifdef AAA_SND_MASTER_H
#error "SND_MASTER_H included more than once."
#endif
#define AAA_SND_MASTER_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class c_sound;
class c_sound_player;
class c_snd_input;
class c_sound_input;

namespace snd
{
static	INT32	CONST	BIT_PER_SAMPLE_MONO_DEF	= 16;
static	INT32	CONST	SAMPLE_PER_SEC_DEF		= 44100;
static	INT32	CONST	SAMPLE_PER_CAPTURE_DEF	= 1 << 11;

enum LIB_TYPE : INT32
{
	LIB_NONE = 0,
	LIB_DS,
	LIB_BASS,
	LIB_BASS_ASIO,
	LIB_NB_MAX,
};

enum SAMPLE_RATE : INT32
{
	SAMPLE_RATE_11025 = 0, 
	SAMPLE_RATE_22050,
	SAMPLE_RATE_44100,
	SAMPLE_RATE_48000,
	SAMPLE_RATE_96000,
	SAMPLE_RATE_192000,
	SAMPLE_RATE_UNKNOWN,
	SAMPLE_RATE_MAX_NB,
};

enum SAMPLE_BITS : INT32
{
	SAMPLE_BITS_8 = 0,
	SAMPLE_BITS_16,
	SAMPLE_BITS_24,
	SAMPLE_BITS_32,
	SAMPLE_BITS_FP,
	SAMPLE_BITS_UNKNOWN,
	SAMPLE_BITS_MAX_NB,
};

extern	C_PCHAR_C		sample_rate_str[SAMPLE_RATE_MAX_NB];
extern	C_PCHAR_C		sample_bits_str[SAMPLE_BITS_MAX_NB];
extern	C_PCHAR_C		lib_name_str[LIB_NB_MAX];

extern	SAMPLE_RATE		sample_rate( INT32 sample_rate );
extern	SAMPLE_BITS		sample_bits( INT32 sample_bits );

extern	void	PRINT(				C_PCHAR fmt, ... );
extern	void	ERR_PRINT(			C_PCHAR fmt, ... );	
extern	void	print_device(		INT32 id, C_PCHAR_C name, C_PCHAR_C driver );
extern	void	print_wave_modes(	DWORD mask );

class	c_snd_master final : public c_obj_ui
{
	FACTORY_DECLARE( c_snd_master, c_obj_ui );

public:
private:
	c_sound*		_lib;
	c_sound*		_lib_ds;
	c_sound*		_lib_bass;
	c_sound*		_lib_bass_asio;

//	INT32			_input_buffer_captured;

public:
	INT32			_s_lib_asked_ui;
	INT32			_s_lib_used;
	
	REAL			_volume;
	REAL			_pan	{0.};

	bool			_b_enum_trig;

	c_sound_input*	_sound_input;


	virtual	void	param_init_pt();
	virtual	void	update();

	virtual AAA_ERR	save_do_after( o_str CONST & filename_in );
	virtual AAA_ERR	load_do_after( o_str CONST & filename_in );

	c_sound_input*	get_device_input();

	c_sound*		get_lib();
	C_PCHAR_C		get_lib_name();
	c_snd_input*	get_by_channel( INT32 channel_in );

	bool			delete_player( c_sound_player* pt );
	c_sound_player*	add_player( INT32 device_index );

//	void			inc_input_buffer_captured()		{	++_input_buffer_captured;	}
};

extern	c_snd_master*	g_master;

}	//namespace


