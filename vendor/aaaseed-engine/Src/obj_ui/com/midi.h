
#ifdef AAA_MIDI_H
#error "MIDI_H included more than once."
#endif
#define AAA_MIDI_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#ifdef	WIN32
#	ifndef _INC_MMSYSTEM
#		ifndef AAA_AAA_OS_H
#			include "aaa_os.h"
#		endif
#		include "Mmsystem.h"
#	endif
#endif


extern	INT32	midi_program_change_get();

#define	AAA_MIDI_STREAM()			0

class	c_mackie;
namespace osc
{
	class	ReceivedMessage;
}

class	c_midi final : public c_obj_active_ui
{
	FACTORY_DECLARE( c_midi, c_obj_active_ui );
private:
	static	void	build_verbose_str( CHAR* dst, INT32 len, CHAR* str, INT32 channel_id, INT32 date );
public:
	static	AAA_ERR	load_patchers(  C_PCHAR_C filename );

	static	CONSTEXPR INT32	CHANNEL_NB = 16;	// channel in midi are in [1,16] the index to address arrays will be in [0,15]
	static	CONSTEXPR INT32	CONTROL_NB = 512;	// MIDI have 128 control [0,127] but we extend to [0,511]
	static	CONSTEXPR INT32	NOTE_NB = 128;	// we just have 128 notes [0,127]

	static	CONSTEXPR INT32	OUT_BANK_NB = 8;
	static	CONSTEXPR INT32	OUT_BANK_CTL_NB = 16;

	static	void	CLASS_PRINT_STRING(		C_PCHAR_C fmt, ... );
			void	PRINT_STRING(			C_PCHAR_C fmt, ... );
	static	void	class_print_verbose(	CHAR* str, INT32 channel_id, INT32 date );
			void	print_verbose(			CHAR* str, INT32 channel_id, INT32 date );

	static	CONST	INT32	MIDI_SEQ_NB		= 1000;

	static	void	save_global( C_PCHAR_C filename );
	static	void	load_global( C_PCHAR_C filename ); 
	static	void	update_all();
	static	void	set_focus_all();
	static	void	trig_auto_connect_trax();

//	static	bool	b_control_send_to_lua;
//	static	bool	b_program_send_to_lua;

//VERBOSE
	static	bool	b_verbose_low_in;
	static	bool	b_verbose_low_out;
	static	bool	b_verbose;
	static	bool	b_verbose_display_date;
	static	bool	b_verbose_display_time;
	static	bool	b_verbose_display_count;
	static	bool	b_verbose_display_channel;
	static	bool	b_verbose_velocity_master;
	static	bool	b_verbose_noteon;
	static	bool	b_verbose_noteoff;
	static	bool	b_verbose_velocity;
	static	bool	b_verbose_aftertouch_poly;
	static	bool	b_verbose_aftertouch_channel;
	static	bool	b_verbose_program_change;
	static	bool	b_verbose_control_change;
	static	bool	b_verbose_pitchbend;
	static	bool	b_verbose_system_ui;
	static	bool	b_verbose_clock;
	static	bool	b_verbose_timecode;
	static	bool	b_verbose_out_callback;
	static	bool	b_verbose_read_bank;
	static	bool	b_verbose_event;

	static	bool	b_route_noteon_as_control_change_ui;

	static	INT32	bank_cur;
	static	INT32	bank_cur_ui;

	static	INT32	dynamic_numbers_ui;
	static	REAL	dynamic_duration_ui;
	static	INT32	dynamic_status_ui;
	static	REAL	dynamic_time_ui;
	static	bool	b_record_rec_ui;
	static	bool	b_dynamic_play_trig_ui;
	static	bool	b_dynamic_stop_trig_ui;
	static	bool	b_dynamic_clear_trig_ui;
	static	bool	b_dynamic_loop_ui;
//	static	bool	b_playing_ui;

	static	bool	b_bank_direct;
	static	INT32	bank_direct_begin;
	static	INT32	bank_direct_end;
	static	bool	b_bank_load_trig;
	static	bool	b_bank_a_store_trig;
	static	bool	b_bank_b_store_trig;
	static	bool	b_bank_a_save_trig;
	static	bool	b_bank_b_save_trig;
	static	INT32	digit_ui;
	static	INT32	digit_all;
	static	bool	b_dynamic_record;
	static	bool	b_dynamic_play;
	static	bool	b_dynamic_pause;
	static	bool	b_dynamic_loop;

	static	void	set_verbose( bool CONST in );
	static	void	flip_verbose();

			void	set_bank( INT32 in );
			void	load_bank();
			void	save_bank_a();
			void	save_bank_b();
			void	store_bank_a();
			void	store_bank_b();
			void	ask_bank_save_a();
			void	ask_bank_save_b();
			void	freeze_bank_inter();
			void	interpolate_bank( REAL inter );
			void	ask_bank_prev();
			void	ask_bank_next();
			void	ask_bank_load();
			void	switch_bank_direct();

	static	INT32	get_dynamic_play_percent();
	static	REAL	get_dynamic_duration();

protected:
	INT32	_in_index_opened_out;
	INT32	_out_index_opened_out;
	INT32	_in_index_asked_ui;
	INT32	_out_index_asked_ui;
	bool	_b_active_in_ui;
	bool	_b_active_out_ui;
	bool	_b_open_in;
	bool	_b_open_out;
	bool	_b_enum_in_trig_ui;
	bool	_b_enum_out_trig_ui;
	bool	_b_open_in_asked_ui;
	bool	_b_open_out_asked_ui;		
	bool	_b_skip_close_on_exit;
	bool	_b_close_do_unprepare;

	INT32	_in_message_nb;
	INT32	_out_message_nb;

	o_str	_in_device_name_out;
	o_str	_out_device_name_out;

	o_str	_midi_header;

	UINT8	_channel_zero_based;
	INT32	_channel_ui				[OUT_BANK_NB];
	INT32	_control_ui				[OUT_BANK_NB];

	bool	_b_control_patch_ui;
		
	bool	_b_channel_listen_all;
	bool	_b_channel_listen		[CHANNEL_NB];
	INT32	_control_index_offset	[CHANNEL_NB];
	INT32	_channel_dst			[CHANNEL_NB];

	INT32	channel_reroute( INT32 ch_in );
	REAL	_control_factor			[CHANNEL_NB];

	INT32	_msb_stored;
	INT32   _ctl_stored;
	INT32	_b3_stored;

	o_str	_o_verbose_name_ui;
	o_str	_o_verbose_name; 

#if	AAA_MIDI_STREAM()
	HMIDISTRM	_midi_out_handle;
#else
	HMIDIOUT	_midi_out_handle;
#endif
	HMIDIIN		_midi_in_handle;
	MIDIHDR		_midi_out_st;
	MIDIHDR		_midi_out_buf;

	bool		_b_filter;
	bool		_b_filter_control_change;
	bool		_b_filter_program_change;
	c_mackie*	_mackie;

	bool		_b_nrpn;
	INT32		_s_nrpn;
	INT32		_nrpn_ctl_index;
	INT32		_nrpn_value;
	bool		_b_bcf_14bits;

	//	RECORD/PLAY
//	static CONST	INT32	MIDI_EVENTS_NB	= 1024 * 256;
//	INT32		_midi_event[2][MIDI_EVENTS_NB * 2];
	INT32*		_record_buf			{nullptr};
	INT32*		_play_buf			{nullptr};

	INT32		_record_index;
	INT32		_play_index;
	INT32		_play_nb;
	bool		_b_record;
	bool		_b_record_allow;
	bool		_b_play;
	bool		_b_play_allow;

	bool		_b_start_trigger;
	bool		_b_stop_trigger;
	bool		_b_save_trigger;
	bool		_b_load_trigger;

	bool		_b_dynamic_restart_trig;
	bool		_b_dynamic_stop_trig;
	bool		_b_dynamic_play_trig;
	bool		_b_dynamic_record_trig;
//	bool		_b_dynamic_play;
	bool		_b_dynamic_clear_trig;
	bool		_b_dynamic_loop_trig;

	INT32		_sequencer_nb;

	INT32		_date_cur;

	INT32		_param_index_bank_name;
	INT32		_param_index_ctl	[OUT_BANK_NB];

/*
	CONST	INT32	TC_NUF_NB	= 256;
	INT32	tc_buf_[][TC_NUF_NB][2];
	INT32	tc_buf_count_;
*/

			bool	_b_tc_trig;
	static	REAL	tc_time_in;		//only_one_tc for now
	static	REAL	tc_time_offset;	//only_one_tc for now
	static	REAL	tc_time_out;	//only_one_tc for now
			INT32	_tc_type;
			INT32	_tc_frame;
			INT32	_tc_second;
			INT32	_tc_minute;
			INT32	_tc_hour;
	static	INT32	control_change_last_channel_id_out;
	static	INT32	control_change_last_control_index_out;

//	bool	b_midi_timer = false;
public:	//todo add a parameter to pass the origin midi obj, net....
	static	FINLINE	bool	is_valid_channel(			INT32 channel_id	);	// [1,16]
	static	FINLINE	bool	is_valid_channel_control(	INT32 channel_id,	INT32 control_index );	// ctl in [0,511]
	static	FINLINE	bool	is_valid_channel_note(		INT32 channel_id,	INT32 note_index );	// note in [0,127]

	static	void	osc_process_message( CONST osc::ReceivedMessage& msg ); 

	static	bool	static_set_control_silent(	INT32 channel_id, INT32 control_index,	REAL value, INT32 when = 0, bool b_send = true, bool b_record = true );
	static	bool	static_set_control(			INT32 channel_id, INT32 control_index,	REAL value, INT32 when = 0, bool b_send = true, bool b_record = true );
	static	REAL	static_get_control(			INT32 channel_id, INT32 control_index	);
	static	void	static_set_velocity(		INT32 channel_id, INT32 note_index,		REAL vel,	INT32 when = 0, bool b_send = true );
	static	void	static_set_noteoff(			INT32 channel_id, INT32 note_index,					INT32 when = 0 );
	static	void	static_set_noteon( 			INT32 channel_id, INT32 note_index,		REAL vel,	INT32 when = 0 );
	static	void	store_program_change(		INT32 channel_id, INT32 program_index,				INT32 when = 0 );

			void	print_low( C_PCHAR_C  src, INT32 data );
			void	print_low( C_PCHAR_C  src, UINT8 status, UINT8 b1, UINT8 b2 );
			void	print_low( C_PCHAR_C  src, UINT8 status, UINT8 b1 );

private:
			void	treat_in( INT32 data, INT32 date );
			void	print_error( MMRESULT err_code );

	// PLAY/RECORD
			void	alloc_play_record_buf();
			void	dealloc_play_record_buf();
			void	flip_buf();
			void	record_start();
			void	record_stop();
			void	play_start();
			void	play_stop();
			void	start(	INT32 date );
			void	stop(	INT32 date );
			void	resume(	INT32 date );
			void	play_update();
			void	record( INT32 data, INT32 date );

			void	seq_save();
			void	seq_load();
public:
			void	in_callback(	UINT wMsg, DWORD_PTR p1, DWORD_PTR p2 );
			void	out_callback(	UINT wMsg, DWORD_PTR p1, DWORD_PTR p2 );
			void	param_update_pt();
	virtual void	prepare_for_ui();
	virtual	void	param_init_pt();

			INT32	enumerate_in();
			INT32	enumerate_out();
			void	open();
			void	open_in();
			void	open_out();
			void	close_in();
			void	close_out();
			void	open_in_low(  INT32 device_index );
			void	open_out_low( INT32 device_index );

	virtual	void	update();

	virtual	AAA_ERR	save_do_after( o_str CONST & filename );
	virtual	AAA_ERR	load_do_after( o_str CONST & filename );

	FINLINE	void	set_channel( INT32 channel_id ) { _channel_zero_based = ( channel_id - 1 ) & 0xf; }
	FINLINE	INT32	get_channel()			CONST		{ return _channel_zero_based + 1; }
	FINLINE bool	is_active_open_in()		CONST		{ return is_active() && _b_open_in; }
	FINLINE bool	is_active_open_out()	CONST		{ return is_active() && _b_open_out; }

			UINT32	send_bytes( UCHAR* str, INT32 len );
			UINT32	send_bytes_2( UINT8 status, UINT8 b1 );
			UINT32	send_bytes_3( UINT8 status, UINT8 b1, UINT8 b2 );

	//	Base Out Messages
			UINT32	send_note_off(				UINT8 note_index, UINT8 velocity );
			UINT32	send_note_on(				UINT8 note_index, UINT8 velocity );
			UINT32	send_polyphonic_pressure(	UINT8 note_index, UINT8 value );
			UINT32	send_control_change(		UINT8 control_index, UINT8 value );
			UINT32	send_program_change(		UINT8 program_index );
			UINT32	send_channel_pressure(		UINT8 value );

			void	send_nrpn(					UINT8 control_index, INT32 value );
			void	send_14bits(				UINT8 control_index, INT32 value );
			void	send_nrpn_or_14bits(		UINT8 control_index, INT32 value );

			void	channel_listen_init();
			void	out_send_test();
};

static	CONSTEXPR	INT32	MIDI_OBJ_MAX_NB = 26;
extern	c_midi*	midi_array[MIDI_OBJ_MAX_NB];

