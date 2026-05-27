
#ifdef AAA_SOUND_NEW_H
#error "SOUND_NEW_H included more than once."
#endif
#define AAA_SOUND_NEW_H 1


#ifndef AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif

class c_snd_input;
class c_sound_input;
class c_sound_player;

class c_sound : public c_obj_active_ui
{
	FACTORY_ABSTRACT_DECLARE( c_sound, c_obj_active_ui );
public:
	enum	PLAYER_STATUS : INT32
	{
		STATUS_STOP = 0,
		STATUS_PLAYING,
		STATUS_PAUSED,
		STATUS_LOOPING,
		STATUS_ERROR,
		STATUS_UNKNOWN,
		PLAYER_STATUS_MAX_NB,
	};
	static	C_PCHAR_C	player_status_str[PLAYER_STATUS_MAX_NB];

	static	INT32 CONST	INPUT_NB_MAX = 32;
	static	INT32 CONST	OUTPUT_NB_MAX = 32;

public:
protected :

	INT32		_device_out_count;		// Nb of devices present in system
	INT32		_device_in_count;		// Nb of devices present in system
	INT32		_devices_channel_nb;

	INT32		_channel_nb_asked_def;

//	bool		_b_enable;
//	bool		_b_input_enable[ INPUT_NB_MAX ];
	bool		_b_output_enable[ OUTPUT_NB_MAX ];

	INT32		_device_in;				// Device we want to use for input
	INT32		_device_out;			// Device we want to use for output

//	INT32		_output_nb;
//	INT32		_input_nb;
	//void		set_input_nb( INT32 nb )	{ _input_nb = nb;			}

//	INT32		_frequency;
	INT32		_nb_channel_in;
	INT32		_nb_channel_out;

	INT32		_s_sample_rate_in_ui;
	INT32		_s_sample_bits_in_ui;

	INT32		_s_sample_rate_out_ui;
	INT32		_s_sample_bits_out_ui;

//	UINT8		_sound_buffer[SOUND_BYTE_PER_CAPTURE];	// the sound

	std::list<c_sound_input*>	_sound_input;
	std::vector<c_snd_input*>	_snd_inputs;

	std::list<c_sound_player*>	_sound_player;

			c_sound_input*		find_input_by_id( INT32 id );
	virtual c_sound_input*		create_input_new() = 0;

	virtual c_sound_player*		create_player_new();

public:
	C_PCHAR_C					get_lib_name();

	virtual	void				open();
	virtual	void				close();

//	virtual	void				init_input()	= 0;
//	virtual	void				init_output()	= 0;
//	virtual	void				deinit_output()	= 0;

//	virtual	void				update()		= 0;
//	virtual	void				update_input();
//	virtual	void				update_output();

//	virtual	o_str*				get_name( INT32 in );

	c_sound_input*				add_input();
	bool						delete_input(	c_sound_input* pt );
	void						delete_input_all();

	c_sound_player*				add_player( INT32 device_index );
	bool						delete_player( c_sound_player* sound_player );
	void						delete_player_all();

//			bool				is_enabled()		{ return _b_enable; };
			INT32				get_device_out()			CONST {	return _device_out;		}
			INT32				get_device_in()				CONST {	return _device_in;		}

//todo really do it
			INT32				get_device_out_nb()			CONST {	return _device_out_count;		}
			INT32				get_device_in_nb()			CONST {	return _device_in_count;		}
			INT32				get_devices_channel_nb()	CONST {	return _devices_channel_nb;		}

			INT32				get_channel_nb_asked_def()	CONST {	return _channel_nb_asked_def;	}

	virtual INT32				enumerate_device_out();
	virtual INT32				enumerate_device_in();
	virtual void				enumerate();

	virtual bool				open_control_panel();

	virtual void				update();
			void				dealloc_snd_input();
			void				alloc_snd_input( INT32 nb_asked );
			void				save_snd_input( o_str CONST & filename_in );
			void				load_snd_input( o_str CONST & filename_in );
				
			c_snd_input*		get_by_channel( INT32 channel_in );
};



