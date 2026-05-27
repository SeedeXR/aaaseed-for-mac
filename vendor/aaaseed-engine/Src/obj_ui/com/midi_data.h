
#ifdef AAA_MIDI_DATA_H
#error "MIDI_DATA_H included more than once."
#endif
#define AAA_MIDI_DATA_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef _MAP_
#	include <map>
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#ifndef AAA_MIDI_H
#	include "midi.h"
#endif
#ifndef AAA_MIDI_PATCH_H
#	include "midi_patch.h"
#endif


typedef struct SEQ_REC_MAP
{
	INT32	_channel_id;
	INT32	_control_index;
	REAL	_value;
} SEQ_REC_MAP;

//MIDI_DATA
class c_midi_data final : public c_obj
{
private:
	//	RECORD/PLAY
	std::multimap< INT32, SEQ_REC_MAP >*				_map_record;
	std::multimap< INT32, SEQ_REC_MAP >::iterator		_it_map_play;
	std::multimap< INT32, SEQ_REC_MAP >::iterator		_it_record_insert;
	typedef	std::pair< INT32, SEQ_REC_MAP >	Seq_Rec_Pair;

	std::vector< std::multimap< INT32, SEQ_REC_MAP >* >	_stack_record;

	INT32	_record_number;
	INT32	_dynamic_rec_start_date;
	INT32	_dynamic_play_start_date;
	INT32	_dynamic_pause_start_time;
	INT32	_dynamic_pause_time;
	INT32	_play_index;
	bool	_b_stop;
	bool	_b_playing;
	bool	_b_record;
//	bool	_loop;
	REAL	_duration;
	REAL	_time;

	//	INT32		slice_begin_;
	//	SEQ_REC*	add_dynamic_buffer();

	o_str			_o_data_name;
	c_midi_patchs	_patchs;
	REAL			_control[c_midi::CHANNEL_NB][c_midi::CONTROL_NB];

protected:
public:
	static CONSTEXPR	INT32	CONTROL_NB_IN_BANK = c_midi::CONTROL_NB;

	static	c_midi_patchs	global_patchs;

	c_midi_data();
	~c_midi_data();

	void	clear();
	void	record_copy_to(				c_midi_data *dst );
	void	channel_copy_to(			c_midi_data *dst, INT32 ch_id,	INT32 ctl_index_start=0, INT32 ctl_index_stop=CONTROL_NB_IN_BANK-1 );
	void	bank_copy_to(				c_midi_data *dst,				INT32 ctl_index_start=0, INT32 ctl_index_stop=CONTROL_NB_IN_BANK-1 );
	void	channel_interpolate(		c_midi_data *srca, c_midi_data *srcb, REAL t, INT32 ch );
	void	bank_interpolate(			c_midi_data *srca, c_midi_data *srcb, REAL t );
	void	send_to_net(				INT32 ch );
	void	send_to_net();

	void	save_dynamic(				INT32 index );
	void	load_dynamic(				INT32 index );

	void	dynamic_pause_begin();
	void	dynamic_pause_end();

	void	dynamic_restart();
	bool	dynamic_play(				SEQ_REC_MAP* data, INT32* date );
	void	dynamic_stop();
	void	dynamic_record(				INT32 channel_id, INT32 control_index, REAL what, INT32 when );
	void	set_record(					bool recording );
//	void	set_loop(					bool loop )					{ _b_loop = loop; };

	void	dynamic_new_map(			bool b_copy );
	void	dynamic_clear_data();	// clear rec data
	bool	is_playing()				{ return _b_playing;		}
	INT32	get_dynamic_numbers()		{ return _record_number;	}
	REAL	get_dynamic_duration()		{ return _duration;			}
	REAL	get_dynamic_time()			{ return _time;				}
	void	set_dynamic_record_start( INT32 when )	{ _dynamic_rec_start_date = when; _duration = 0.0; }
	//	void	dynamic_record_stop( INT32 when );

	o_str*			get_data_name()		{ return &_o_data_name;		}
	c_midi_patchs*	get_patchs()		{ return &_patchs;			}

			void	set_control(		INT32 ch_id, INT32 ctl_index, REAL val );
	FINLINE	void	set_control_low(	INT32 ch_id, INT32 ctl_index, REAL val );
	FINLINE	REAL	get_control(		INT32 ch_id, INT32 ctl_index );
			REAL*	get_control_pt(		INT32 ch_id, INT32 ctl_index );

	bool	data_is_empty();

	INT32	save_data(					 FILE* file, INT32 index );
	INT32	load_data(					 FILE* file, INT32 index, bool b_load_record_file );
};

class c_midi_universe
{
public:
	static	CONST	INT32	MIDI_BANK_NB	= 1000;
	static	c_midi_universe*	cur;

	FINLINE	static	INT32	make_valid_bank_nb( INT32 in )	{ return IMOD( in, MIDI_BANK_NB ); }

	c_midi_data		_midi_to_save_a;
	c_midi_data		_midi_to_save_b;
	c_midi_data		_midi_data_inter;
	c_midi_data		_midi_data;
	c_midi_data		_midi_bank[MIDI_BANK_NB];

	INT32	write_file_control(	FILE* file );
	INT32	read_file_control(	FILE* file,			bool b_load_record_file );
	REAL	get_control(		INT32 channel_id,	INT32 control_index );
	void	interpolate_bank(	REAL inter );
};

extern	void midi_init();
extern	void midi_deinit();

extern	void	midi_control_get_band(		INT32 channel_id, INT32 start_control_index, INT32 band_nb, REAL* p_bands );
extern	FP32	midi_velocity_get_float(	INT32 channel_id, INT32 control_index );
extern	void	midi_velocity_get_band(		INT32 start_note_index, INT32 band_nb, REAL* p_bands );


