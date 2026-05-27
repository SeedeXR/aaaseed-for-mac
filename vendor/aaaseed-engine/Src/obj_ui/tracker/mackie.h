
#ifdef AAA_MACKIE_H
#error "MACKIE_H included more than once."
#endif
#define AAA_MACKIE_H 1


#ifndef	AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif

class c_slice;

static	CONST	INT32	BEHRINGER_BUT_CHANNEL_NB = 2;

namespace midi
{
	enum CONTROLLER_TYPE : INT32
	{
		CS32 = 0,
		CONTROLLER_REGULAR = 1,
		MACKIE,
		BCF2000,
		CONTROLLER_TYPE_NB
	};

}
class c_midi;

class	c_mackie final : public c_obj
{
friend class c_midi;
private:
	static	CONST	INT32	COL_NB				= 8;
	
	static	CONST	INT32	BUT_CONTROL_NB		= COL_NB;
	static	CONST	INT32	BUT_CHANNEL_NB		= 4;
	static	CONST	INT32	BUT_CHANNEL_OFFSET	= 2;
	static	CONST	INT32	TIME_CODE_NB		= 10;

	static	bool	b_but_global[BUT_CHANNEL_NB];

	static	CONST	INT32	GBUT_NB				= 61;
	INT32	_gbut[GBUT_NB];
public:
	static	CONST	INT32	VPOT_NB				= COL_NB ;
	static	CONST	INT32	FADER_NB			= COL_NB + 1;
	static	CONST	INT32	SLICE_NB			= 64;

protected:
	midi::CONTROLLER_TYPE	_s_midi_controller;

	c_slice*				_slice_cur;
	INT32					_slice_cur_index;	
	INT32					_control_offset;
	bool					_b_slice_prev_trig;
	bool					_b_slice_next_trig;

	INT32	_control_offset_ui;

	INT32	_slice_offset_ui;	//todo check

	c_midi*	_midi;
	bool	_b_need_sync;

	bool	_b_fader_shift_ui;
	bool	_b_vpot_shift_ui;
	bool	_b_but_shift_ui;

	bool	_b_fader_shift;
	bool	_b_vpot_shift;
	bool	_b_but_shift;

	bool	_b_master;
	bool	_b_bank_interpolate_allow;
	o_str	_mess_base;
	o_str	_o_left;
	REAL	_fader_sensitivity_ui;

	INT32	_fader					[FADER_NB];
	bool	_b_fader_contact		[FADER_NB];
	bool	_b_fader_contact_ask	[FADER_NB];
	DOUBLE	_fader_off_date			[FADER_NB];
	bool	_b_fader_contact_change;
//	bool	_b_fader_send;
	DOUBLE	_fader_tochange_time;
	DOUBLE	_fader_change_interval;
	INT32	_fader_sensitivity;
			
	bool	_b_vpot_need_to_send	[VPOT_NB];
	INT32	_vpot					[VPOT_NB];
	bool	_b_vpot_pushed			[VPOT_NB];
	DOUBLE	_vpot_pushed_date		[VPOT_NB];

	INT32	_but[BUT_CHANNEL_NB][BUT_CONTROL_NB];

	UINT8	_out_buf[1024];

//	INT32	gbut_to_change[GBUT_CONTROL_NB];

	INT32	assignement_[2];
	INT32	time_code_[TIME_CODE_NB];

	bool	_b_dynamic_play;
	bool	_b_dynamic_record;
//	INT32	_dyn_play_percent;
//	REAL	_duration;
//	INT32	_dyn_rec_duration_min;
//	INT32	_dyn_rec_duration_sec;
//	bool	_b_bottom;

	bool	_b_verbose;

private:
			void		set_slice_direct(	INT32 in			);

public:
	FINLINE	INT32		get_control_offset()	{	return _control_offset;	}

			c_mackie();
			~c_mackie();

			void		init();

			void		update();

			void		set_slice(			INT32 in			);
			void		ask_slice_prev();
			void		ask_slice_next();
			void		ask_slice_prev_row();
			void		ask_slice_next_row();
			void		ask_slice(			INT32 slice_in		);
			void		set_slice_offset(	INT32 slice_offset	);
			void		flip_fader_shift();
			void		flip_vpot_shift();
			void		flip_but_shift();

//			INT32		get_slice_current()		{ return control_offset_ui_; }

	FINLINE	void		send_fader(								INT32 control_index, REAL value );
	FINLINE	void		send_vpot(								INT32 control_index, REAL value );
	FINLINE	void		send_but_remap(	INT32 channel_but_0,	INT32 control_index, INT32 in );
//	FINLINE	void		set_gbut_to_change( INT32 control, INT32 in);
	FINLINE	void		send_gbut(								INT32 control_index, INT32 in );

			void		do_but(			INT32 channel_id,		INT32 control_index, bool b_state, INT32 when );

			void		mackie_send_assignement_low( INT32 char_left, INT32 char_right );
			void		mackie_send_assignement( INT32 number );
			void		mackie_send_assignement_none();
			void		mackie_send_time_code( INT32 pos, INT32 in );
			void		mackie_send_time_code( CHAR* str );
			void		mackie_send_line( INT32 line_index, INT32 char_index, CHAR* in );

			void		set_fader_sensitivity( REAL in );
			void		set_line_a_left( o_str* in );
			void		make_line_a_low();
			void		make_line_b_low();
			void		make_line_a();
			void		make_line_b();

			void		fader_set_shift(			bool in );
	FINLINE	INT32	fader_get_channel(			INT32 index );
			void		fader_ask_contact_change(	INT32 index, bool in );

			void		vpot_set_shift(				bool in );
	FINLINE	INT32	vpot_get_channel(			INT32 control_index );
				void    vpot_do_push(				INT32 index, bool b_down, INT32 when );
				void    vpot_do_inc(				INT32 index, INT32 inc, INT32 when ); 	

			void		but_set_shift(		bool in );
//	FINLINE	INT32		but_get_channel( INT32 control );

			void		set_control_offset( INT32 in );
			void		set_need_sync();
			void		sync_with_data();

	static	c_slice*	get_slices();
	static	c_slice*	get_slice( INT32 index );
	static	void		alloc_slices();
	static	void		dealloc_slices();
	static	void		save_slices( o_str CONST & filename );
	static	void		load_slices( o_str CONST & filename );
};
