#include "obj_ui/tracker/mackie.h"
#include "time/aaa_time.h"
#include "slice.h"
#include "strnum.h"
#include "obj_ui/com/midi_data.h"
#include "ui/seed_ui.h"

			
//todo check the send_gbut() o mackie

namespace
{
	c_slice*	slices = nullptr;
}

void		c_mackie::alloc_slices()						{	SAFE_NEW_ARRAY( slices, c_mackie::SLICE_NB );	}
void		c_mackie::dealloc_slices()						{	SAFE_DELETE_ARRAY( slices );					}
c_slice*	c_mackie::get_slices()							{	return slices;									}
c_slice*	c_mackie::get_slice( INT32 index )				{	return &slices[index];							}
void		c_mackie::save_slices( o_str CONST & filename )		{	c_slice::save( slices, SLICE_NB, filename );	}
void		c_mackie::load_slices( o_str CONST & filename )		{	c_slice::load( slices, SLICE_NB, filename );	}

namespace {
	INT32 CONST	BUT_OFF		= 0;
	INT32 CONST	BUT_ON		= 0x7f;
	INT32 CONST	BUT_FLASH	= 1;
	
	INT32 CONST FADER_CHANNEL		= 1;
	INT32 CONST FADER_SHIFT_CHANNEL	= 9;
	INT32 CONST VPOT_CHANNEL		= 2;
	INT32 CONST VPOT_SHIFT_CHANNEL	= 10;
}

bool	c_mackie::b_but_global[c_mackie::BUT_CHANNEL_NB];

//todo we don't use _b_need_sync  anymore perhaps this is because it is not set correcly and we need to call this more
void	c_mackie::set_need_sync()
{
	_b_need_sync = true;
	_fader_tochange_time = aaa::time::get_real_time_from_start();
}

c_mackie::c_mackie()
	:_b_fader_shift(false)
	,_b_vpot_shift(false)
	,_midi(nullptr)
{
	_fader_sensitivity = -1;
	set_need_sync();
	init();
	set_slice_direct( 0 );
}
c_mackie::~c_mackie()
{
}

void	c_mackie::init()
{
	for( INT32 i = 0; i < FADER_NB; ++i )
	{
		_b_fader_contact[i] = false;
		_b_fader_contact_ask[i] = false;
		_fader[i] = -1;
	}
	for( INT32 i = 0; i < VPOT_NB; ++i )
	{
		_vpot[i] = -1;
		_b_vpot_pushed[i] = false;
	}
	for( INT32 j = 0; j < BUT_CHANNEL_NB; ++j )
		for( INT32 i = 0; i < BUT_CONTROL_NB; ++i )
			_but[j][i] = -1;
	for( INT32 i = 0; i < GBUT_NB; ++i )
		_gbut[i] = -1;
//	_b_fader_send = true;
	_fader_tochange_time = -1;
	_fader_change_interval = .2;
	_slice_offset_ui = 0;
	_s_midi_controller = midi::CONTROLLER_REGULAR;
}

void	c_mackie::set_line_a_left( o_str* in )
{
	_o_left.set( in->get() );
	make_line_a();
}

void	c_mackie::make_line_a_low()
{
//			mackie->send_line( 0, 0, mackie_line_a.get() );
//	send_line( 0, 55-midi_bank[bank_cur].name.get_len(), midi_bank[bank_cur].name.get() );
	CHAR	line[56];	// 7 case de 7 char + 1 case de 6 + char null
	INT32	len;
	CHAR*	pt;
	CHAR*	end;

	line[55] = 0;

	pt = line;
	//	message on the leds
	len = _mess_base.get_len();
	strncpy( line, _mess_base.get(), len );
	pt += len;
	*pt++ = ' ';
	*pt++ = ' ';

	len = _slice_cur->_info[0].get_len();
	strncpy( pt, _slice_cur->_info[0].get(), len );
	pt += len;

	len = _o_left.get_len();
	end = line+55-len;
	strncpy( end, _o_left.get(), len );

	for( ; pt < end; ++pt )
		*pt = ' ';

	mackie_send_line( 0, 0, line );
}

void	c_mackie::make_line_a()
{
	if( _s_midi_controller == midi::MACKIE )
		make_line_a_low();
}

void	c_mackie::make_line_b_low()
{
	CHAR line[56];	// 7 case de 7 char + 1 case de 6 + char null
//	if( slice_cur->info[b_vpot_shift?4:3].get_len() )
	{
//		INT32	i;
		INT32	index;

		line[55] = 0;

		INT32 ctl_offset = 1 + get_control_offset();
		for( INT32 i = 0; i < COL_NB; ++i )
		{
			if( _b_fader_contact[i] )
				index = _b_fader_shift ? 2 : 1;
			else
				index = _b_vpot_shift ? 4 : 3;
			if( !_slice_cur->_info[index].is_empty() )
				strncpy( &line[i * 7], _slice_cur->_info[index].get() + i * 7, ( i == 7 ) ? 6 : 7 );
			else
			{
				if( _b_vpot_shift )
					strncpy( &line[i * 7], "    b  ", ( i == 7 ) ? 6 : 7 );
				else
					strncpy( &line[i * 7], "       ", ( i == 7 )? 6 : 7 );
				strnum::make_with_blank( &line[i * 7], 3, i + ctl_offset );
			}
		}
		mackie_send_line( 1, 0, line );
	}
}

FINLINE	void	c_mackie::make_line_b()
{
	if( _s_midi_controller == midi::MACKIE )
		make_line_b_low();
}

//todo perhaps we need check on bounds here
void	c_mackie::ask_slice_prev()				{	_control_offset_ui -= 8;	}
void	c_mackie::ask_slice_next()				{	_control_offset_ui += 8;	}
void	c_mackie::ask_slice( INT32 slice_in )
{
	if( c_midi::b_verbose_event )
		DBG_PRINT_STRING( "%s( %d )", __FUNCTION__, slice_in );	
	_control_offset_ui = _slice_offset_ui + slice_in * 8;
}

void	c_mackie::ask_slice_prev_row()
{
	if( c_midi::b_verbose_event )
		DBG_PRINT_STRING( "%s()", __FUNCTION__ );	
	if( _control_offset_ui >= 64 )
		_control_offset_ui -= 64;
}
void	c_mackie::ask_slice_next_row()
{
	if( c_midi::b_verbose_event )
		DBG_PRINT_STRING( "%s()", __FUNCTION__ );		
	if( _control_offset_ui < 64 )
		_control_offset_ui += 64;
}

void	c_mackie::set_slice_offset(	INT32 slice_offset )
{
	if( c_midi::b_verbose_event )
		DBG_PRINT_STRING( "%s( %d )", __FUNCTION__, slice_offset );
	_slice_offset_ui	= slice_offset;
}

void	c_mackie::flip_fader_shift()
{
	_b_fader_shift_ui = !_b_fader_shift_ui;
	if( c_midi::b_verbose_event )
		SWITCH_PRINT_STATE( "Asked Flip Fader shift", _b_fader_shift_ui );	
}
void	c_mackie::flip_vpot_shift()	
{
	_b_vpot_shift_ui = !_b_vpot_shift_ui;
	if( c_midi::b_verbose_event )
		SWITCH_PRINT_STATE( "Asked Flip VPot shift", _b_vpot_shift_ui );
}
void	c_mackie::flip_but_shift()
{
	_b_but_shift_ui = !_b_but_shift_ui;
	if( c_midi::b_verbose_event )
		SWITCH_PRINT_STATE( "Asked Flip Button shift", _b_but_shift_ui );
}

void	c_mackie::set_fader_sensitivity( REAL in )
{
	INT32	tmp;
	tmp = INT32( in * 5. );
	CLAMP_REF( tmp, 0, 5 );
	if( _fader_sensitivity != tmp )
	{
		UINT8 out_str[10];
		out_str[0] = 0xf0;
		out_str[1] = 0x00;
		out_str[2] = 0x00;
		out_str[3] = 0x66;
		out_str[4] = 0x14;
		out_str[5] = 0x0e;
		out_str[7] = tmp;
		out_str[8] = 0xf7;
		for( INT32 i = 0; i < FADER_NB; ++i )
		{
			out_str[6] = i;
			_midi->send_bytes( out_str, 9 );
		}
		_fader_sensitivity = tmp;
	}
}

void	c_mackie::fader_set_shift( bool in )
{
	if( _midi->is_active_open_out() )
	{
		if( _b_fader_shift != in )
		{
			_b_fader_shift = in;
			SWITCH_PRINT_STATE( "MIDI Fader Shift", in );
			if( _s_midi_controller == midi::BCF2000 )
			{
				_midi->set_channel( 16 );
				_midi->send_control_change( 2, _b_fader_shift ? 127 : 0 );
			}
			set_need_sync();
	//todo
	//		make_line_b();
		}
	}
}

void	c_mackie::fader_ask_contact_change( INT32 index, bool in )
{
	if( INSIDE_MIN_MAX( index, 0, FADER_NB-1 ) )
	{
		if( _b_fader_contact_ask[index] != in )
		{
			if( !in )
				_fader_off_date[index] = aaa::time::get_real_time_from_start();
			_b_fader_contact_ask[index] = in;
			_b_fader_contact_change = true;
		}
	}
	else
		err_print( "%s() No fader index %d.", __FUNCTION__, index );
}

FINLINE INT32	c_mackie::fader_get_channel( INT32 index )
{
	return ( _b_fader_shift && _slice_cur->fader_have_shift( index ) ) ? FADER_SHIFT_CHANNEL : FADER_CHANNEL;
}


void	c_mackie::vpot_set_shift( bool in )
{
	if( _midi->is_active_open_out() )
	{
		if( _b_vpot_shift != in )
		{
			_b_vpot_shift = in;
			SWITCH_PRINT_STATE( "MIDI VPot Shift", in );
			make_line_b();
			if( _s_midi_controller == midi::BCF2000 )
			{
				_midi->set_channel( 16 );
				_midi->send_control_change( 1, _b_vpot_shift ? 127 : 0 );
			}
			set_need_sync();
		}
	}
}

FINLINE	INT32	c_mackie::vpot_get_channel( INT32 index )
{
	return ( _b_vpot_shift && _slice_cur->vpot_have_shift( index ) ) ? VPOT_SHIFT_CHANNEL : VPOT_CHANNEL;
}

void	c_mackie::vpot_do_push( INT32 index, bool b_down, INT32 when )	
{
	if( b_down )
	{
		REAL time_loc = REAL(aaa::time::get_real_time());
		if( ui::is_double( time_loc, REAL(_vpot_pushed_date[index]) ) )
		{
			REAL r;
//todo
/*			r = midi_control_get_float( 2, control+mackie->control_offset );
			if( r < 0. )
				r = 0.;
			else if( r > 1. )
				r = 1.;
			else if( 0.<r && r<=1. )
				r = 0.;
			else
				r = 1.;
*/
			r = 0;
			_midi->static_set_control( vpot_get_channel( index ), index + get_control_offset(), r, when );
//bad							mackie->send_vpot( control, r );
		}
		if( _s_midi_controller == midi::BCF2000 )
			_b_vpot_pushed[index] = !_b_vpot_pushed[index];
		else
			_b_vpot_pushed[index] = true;
		_vpot_pushed_date[index] = time_loc;
	}
	else
		_b_vpot_pushed[index] = false;
}

void    c_mackie::vpot_do_inc(	INT32 index, INT32 inc, INT32 when )
{
	if( _b_vpot_pushed[index] )
		inc /= 8;
	INT32 ch_id = vpot_get_channel( index );
	index += get_control_offset();
	REAL value = c_midi_universe::cur->get_control( ch_id, index ) + REAL( inc ) / REAL(128);
	_midi->static_set_control( ch_id, index, value, when );
	//bad mackie->send_vpot( control, value );
}

void	c_mackie::but_set_shift( bool in )
{
	if( _midi->is_active_open_out() )
	{
		if( _b_but_shift != in )
		{
			_b_but_shift = in;
			SWITCH_PRINT_STATE( "MIDI Button Shift", in );
			if( _s_midi_controller == midi::BCF2000 )
			{
				_midi->set_channel( 16 );
				_midi->send_control_change( 3, _b_but_shift ? 127 : 0 );
			}
			set_need_sync();
		}
	}
}
/*
FINLINE	INT32	c_mackie::but_get_channel( INT32 ch_in )
{
	return b_but_shift ? ch_in+2 : ch_in;
}
*/

void	c_mackie::set_slice_direct( INT32 in )
{
	if( c_midi::b_verbose_event )
		DBG_PRINT_STRING( "change slice to : %d", in );

	_slice_cur_index = in;
	_slice_cur = &slices[in];
	_control_offset = in * COL_NB;

	if( _midi && _midi->is_active_open_out() )
	{
		make_line_a();
		make_line_b();
	}
	set_need_sync();
}

void	c_mackie::set_slice( INT32 in )
{
	in = IMOD( in, SLICE_NB );
	if( _slice_cur_index != in )
		set_slice_direct( in );
}

UINT8	cs32_code[] = 
{
	0x03, 0x06, 0x0c, 0x18, 0x30, 0x21
};
INT32	cs32_index = 0;

void	c_mackie::update()
{
	REAL	tmp;

	if( _b_slice_prev_trig )
	{
		ask_slice_prev();				
		_b_slice_prev_trig = false;
	}
	if( _b_slice_next_trig )
	{
		ask_slice_next();				
		_b_slice_next_trig = false;
	}
	if( _s_midi_controller == midi::MACKIE && _b_bank_interpolate_allow )
	{
		tmp = c_midi::static_get_control( 16, 62 );
		tmp = CLAMP_01( tmp * REAL(1.1) - REAL(.05) );	//to avoid mackie slider problem
		if( tmp == 0. )	//flash copy button
			send_gbut( 13, BUT_OFF );
		else
			send_gbut( 13, BUT_FLASH );
	}
	else
		tmp = 0;
	//todo this done for each mackie instead of once for all the mackie which should be in sync
	c_midi_universe::cur->interpolate_bank( tmp );	//todo avoid to do it when no interpolation needed

	DOUBLE	the_time = aaa::time::get();

/*
static	REAL	behringer_time_last = 0.;

	if( ( aaa::time::get()-.4 )>behringer_time_last )
	{
		behringer_time_last = the_time;
		midi->set_channel( 1 );
		for( INT32 i=1; i<=8; ++i )
		{
			INT32	tmp;
			REAL	val;
			//	if redone usr nrpn function
			midi->send_control_change( 98, 0 );
			midi->send_control_change( 97, i );
			//val = SIN_INT( aaa::time::get()*.5+REAL( i )*.125 )*.5+.5;
			val = midi->get_control( 1, i );
			tmp = val * ( ( 1<<14 )-1 );
			midi->send_control_change( 5, ( tmp>>7 )&0x7f );
			midi->send_control_change( 37, tmp&0x7f );
		}
	}
*/
	if( _s_midi_controller == midi::MACKIE )
	{
		CHAR	bank_str[11] = "000       ";
		if( c_midi::get_dynamic_duration() > 0 )
		{
			if( _b_dynamic_play )
			{
				INT32	i;
				i = IMOD( ( INT32 )( the_time * 5. ), 3 );
				strcpy( &bank_str[3], "__" );
				if( i != 2 )
					bank_str[3+i] = '-';
				strnum::make( bank_str + 5, 2, c_midi::get_dynamic_play_percent() );
			}
			else //if( c_midi::get_dynamic_duration() > 0 )
			{
				strnum::make( bank_str + 3, 2, INT32( c_midi::get_dynamic_duration() / REAL(60) ) );
				strnum::make( bank_str + 5, 2, IMOD( INT32(c_midi::get_dynamic_duration()), 60 ) );
			}
		}
		else
		{
			INT32	i;
			i = IMOD( ( INT32 )( the_time * 5. ), 5 );
			strcpy( &bank_str[3], "____" );
			if( i != 4 )
				bank_str[3+i] = '-';
		}
		strnum::make( bank_str, 3, c_midi::bank_cur );
		strnum::make( bank_str + 7, 3, c_midi::digit_all );
		mackie_send_time_code( bank_str );	
		mackie_send_assignement_none();	//todo should be done at start only
//		send_assignement( bank_cur );
	}
	else if( _s_midi_controller == midi::CS32 )
	{
		INT32 i = IMOD( (INT32)( the_time * 3. ), 6 );
		if( i != cs32_index )
		{
			cs32_index = i;
			_midi->send_bytes_3( 0xbe, 0x5d, cs32_code[5 - i] );	//left display
			_midi->send_bytes_3( 0xbe, 0x5e, cs32_code[i] );		//right display
		}
	}
	fader_set_shift( _b_fader_shift_ui	);
	vpot_set_shift(  _b_vpot_shift_ui	);
	but_set_shift(   _b_but_shift_ui	);
	CLAMP_REF( _control_offset_ui, 0, c_midi::CONTROL_NB - 8 );
	set_slice( _control_offset_ui / 8 );
//	if( b_need_sync )
		sync_with_data();
}

FINLINE	void	c_mackie::send_fader( INT32 control_index, REAL value )
{
	if( INSIDE_MIN_MAX( control_index, 0, FADER_NB-1 ) )
	{
		INT32 tmp = INT32( CLAMP_01(value) * REAL( ( 1<<14 )-1 ) );
		if( _fader[control_index] != tmp )
		{
			if( _s_midi_controller == midi::MACKIE )
			{
				if( ( aaa::time::get_real_time_from_start() - _fader_off_date[control_index] ) > _fader_change_interval && !_b_fader_contact[control_index] )
				{
					_midi->send_bytes_3( 0xe0+control_index, tmp&0x7f, ( tmp>>7 )&0x7f );
					_fader[control_index] = tmp;
				}
			}
			else if( _s_midi_controller == midi::BCF2000 )
			{
				//val = SIN_INT( aaa::time::get()*.5+REAL( i )*.125 )*.5+.5;
				//val = midi->get_control( 1, control );
				//tmp = val * ( ( 1<<14 )-1 );
				_midi->set_channel( 1 );
				_midi->send_nrpn_or_14bits( control_index, tmp );
				_fader[control_index] = tmp;
			}
		}
	}
	else
		err_print( "%s() No fader index %d.", __FUNCTION__, control_index );
}

FINLINE	void	c_mackie::send_vpot( INT32 control_index, REAL value )
{
	if( INSIDE_MIN_MAX( control_index, 0, VPOT_NB-1 ) )
	{
		INT32	tmp;
		if( _s_midi_controller == midi::MACKIE )
		{
			tmp = INT32( value * 10. ) + 1;
			CLAMP_REF( tmp, 1, 11 );
			if( value > 1. || value < 0. )
				tmp |= 0x40;
			if( _vpot[control_index] != tmp )
			{
				_vpot[control_index] = tmp;
				_midi->send_bytes_3( 0xb0, 0x30 + control_index, tmp );
			}
		}
		else if( _s_midi_controller == midi::BCF2000 )
		{
			// the  address the led ring is the same that the vpot us to send (and in the same mode nrpn here)
			//value = .75;
			INT32 tmp = INT32( CLAMP_01(value) * REAL( ( 1<<14 )-1 ) );
			if( _vpot[control_index] != tmp || _b_vpot_need_to_send[control_index] )
			{
				_vpot[control_index] = tmp;
				_b_vpot_need_to_send[control_index] = false;
				_midi->set_channel( 2 );
				_midi->send_nrpn( control_index, tmp );
				//_midi->set_channel( 2 );
				//_midi->send_control_change( control_index, tmp );	//this does not function to change the led vpot
			}
		}
	}
	else
		err_print( "%s() No vpot index %d.", __FUNCTION__, control_index );
}

void	c_mackie::do_but( INT32 ch_id, INT32 ctl_index, bool b_state, INT32 when )
{
	if( b_state || _s_midi_controller == midi::BCF2000 )
	{
		REAL	value;
		INT32	ctl_offset;
		
		if( _s_midi_controller == midi::BCF2000 && ch_id == 17 )	//maa raz
		{
		}
		else if( _s_midi_controller == midi::BCF2000 && _b_but_shift )
		{
			ch_id += 8;
		}
		INT32 index = ch_id - 1 - BUT_CHANNEL_OFFSET;
		if( ch_id != 17 && b_but_global[index] )//maa raz
			ctl_offset = 0;
		else
			ctl_offset = get_control_offset();

		if( _s_midi_controller == midi::BCF2000 && ch_id == 17 )
		{
			c_midi::static_set_control( 2, ctl_index + ctl_offset, 0., when );
			return;
		}
		if( !b_but_global[index] && _slice_cur->_b_but_exclusive[index] )
		{
			value = REAL(ctl_index);
			ctl_index = 0;
		}
		else
			value = ( c_midi::static_get_control( ch_id, ctl_index + ctl_offset ) == 0. ) ? REAL(1): REAL(0);
		c_midi::static_set_control( ch_id, ctl_index + ctl_offset, value, when );	//hack we don't use the date
	}
}

FINLINE	void	c_mackie::send_but_remap( INT32 channel_but_0, INT32 control_index, INT32 in )
{
	if( INSIDE_MIN_MAX( channel_but_0, 0, BUT_CHANNEL_NB-1) )
	{
		if( INSIDE_MIN_MAX( channel_but_0, 0, BUT_CHANNEL_NB-1) )
		{
			INT32* pbut = &_but[channel_but_0][control_index];
			if( *pbut != in )
			{
				*pbut = in;
				if( _s_midi_controller == midi::MACKIE )
				{
					_midi->send_bytes_3( 0x90, ( channel_but_0<<3 ) + control_index, in );
				}
				else if( _s_midi_controller == midi::BCF2000 )
				{
 					_midi->set_channel( channel_but_0+3 );	//todo check
					_midi->send_control_change( control_index+1, in );	//todo control_index + 1	 
				}
			}
		}
		else
			err_print( "%s() No button with control index %d.", __FUNCTION__, control_index );
	}
	else
		err_print( "%s() No button with channel index %d.", __FUNCTION__, channel_but_0 );
}

/*
void	c_mackie::set_gbut_to_change( INT32 control, INT32 in )
{
INT32	tmp;
	
	CLAMP_REF( control, 1, GBUT_CONTROL_NB )-1;
	gbut_to_change[control] = in;
}
*/

FINLINE	void	c_mackie::send_gbut( INT32 control_index, INT32 in )
{
	if( _midi->is_active_open_out() )
	{
		if( INSIDE_MIN_MAX( control_index,  0, GBUT_NB-1 ) )
		{
			if( _gbut[control_index] != in )
			{
				_gbut[control_index] = in;
				_midi->send_bytes_3( 0x90, 0x28 + control_index-2, in );	//hack clean this -2
			}
		}
		else
			err_print( "%s() No gbut index %d.", __FUNCTION__, control_index );
	}
}

void	c_mackie::mackie_send_assignement_low( INT32 char_left, INT32 char_right )
{
	if( _midi->is_active_open_out() )
	{
		if( _s_midi_controller != midi::MACKIE )
			return;
		if( assignement_[0] != char_left )
		{
			assignement_[0] = char_left;
			_midi->send_bytes_3( 0xb0, 0x4a, char_left );
		}
		if( assignement_[1] != char_right )
		{
			assignement_[1] = char_right;
			_midi->send_bytes_3( 0xb0, 0x4b, char_right );
		}
	}
}

void	c_mackie::mackie_send_assignement( INT32 number )
{
	if( _s_midi_controller != midi::MACKIE )
		return;
	mackie_send_assignement_low( ( number % 10 ) + 0x30, ( number / 10 ) + 0x30 );	//make conversion better
}

void	c_mackie::mackie_send_assignement_none()
{
	if( _s_midi_controller != midi::MACKIE )
		return;
	mackie_send_assignement_low( 0x20, 0x20 );
}

void	c_mackie::mackie_send_time_code( INT32 pos, INT32 in )
{
	if( _s_midi_controller != midi::MACKIE )
		return;
	in &= 0x3f;
	if( time_code_[pos] != in )
	{
		time_code_[pos] = in;
		_midi->send_bytes_3( 0xb0, 0x3f + TIME_CODE_NB - pos, in );
	}
}

void	c_mackie::mackie_send_time_code( CHAR* str )
{
	if( _s_midi_controller != midi::MACKIE )
		return;
	if( _midi->is_active_open_out() )
	{
		for( INT32 i = 0; i < TIME_CODE_NB; ++i )
			mackie_send_time_code( i, *( str + i ) );
	}
}

void	c_mackie::mackie_send_line( INT32 line_index, INT32 char_index, CHAR* in )
{
	if( _s_midi_controller == midi::MACKIE && _midi )
	{
		INT32 len = (INT32)strlen( in );
		if( len )
		{
			len = MIN( len, 1024 - 16 );

			_out_buf[0] = 0xf0;
			_out_buf[1] = 0x00;
			_out_buf[2] = 0x00;
			_out_buf[3] = 0x66;
			_out_buf[4] = 0x14;
			_out_buf[5] = 0x12;
			if( line_index )
				_out_buf[6] = 0x38 + char_index;
			else
				_out_buf[6] = 0x00 + char_index;
			strncpy( ( CHAR* )&_out_buf[7], in, len );
			_out_buf[7 + len] = 0xf7;

			len += 8;
			_midi->send_bytes( _out_buf, len );	///hack not sure midi is the right one
		}
	}
}

void	c_mackie::sync_with_data()
{
	if( (_s_midi_controller == midi::MACKIE || _s_midi_controller == midi::BCF2000) && _midi->is_active_open_out() )
	{
		//INT32	i;
		//INT32	j;
		INT32 but_row_nb;
		INT32 ctl_offset = get_control_offset();
		if( ( aaa::time::get_real_time_from_start() - _fader_tochange_time ) > _fader_change_interval )	//minimize fader change to protect physical interface
		{
			//fader
			for( INT32 i = 0; i < COL_NB; ++i )	//only 8
				send_fader( i, c_midi::static_get_control( fader_get_channel(i), i + ctl_offset ) );
			if( _s_midi_controller == midi::MACKIE )
				send_fader( 8, c_midi::static_get_control( 16, 62 ) );	//8 is a special case
		}

		//vpot
		for( INT32 i = 0; i < VPOT_NB; ++i )
			send_vpot( i, c_midi::static_get_control( vpot_get_channel( i ), i + ctl_offset ) );

		//but
		INT32	ch_but_offset;
		switch( _s_midi_controller )
		{
		case midi::MACKIE:		but_row_nb = BUT_CHANNEL_NB;
								ch_but_offset = 0;
								break;

		case midi::BCF2000:		but_row_nb = BEHRINGER_BUT_CHANNEL_NB;
								ch_but_offset = _b_but_shift ? 2 : 0;
								break;

		default:				but_row_nb = 0;
								ch_but_offset = 0;
								debug_break( "%s() this type of controller is not recognized here", __FUNCTION__ );
								break;
		}

		for( INT32 j=0; j < but_row_nb; ++j )
		{
			INT32 index = j + ch_but_offset;
			INT32 ch_id = j + ch_but_offset + BUT_CHANNEL_OFFSET + 1;
			if( b_but_global[ index ] )
			{
				for( INT32 i = 0; i < BUT_CONTROL_NB; ++i )
					send_but_remap( j, i, c_midi::static_get_control( ch_id, i ) ? BUT_ON : BUT_OFF );
			}
			else if( _slice_cur->_b_but_exclusive[index] )
			{
				INT32 ref = INT32( c_midi::static_get_control( ch_id, ctl_offset ) );
				for( INT32 i = 0; i < BUT_CONTROL_NB; ++i )
					send_but_remap( j, i, ( ref==i ) ? BUT_ON : BUT_OFF );
			}
			else
			{
				for( INT32 i = 0; i < BUT_CONTROL_NB; ++i )
					send_but_remap( j, i, c_midi::static_get_control( ch_id, i + ctl_offset ) ? BUT_ON : BUT_OFF );
			}
		}
		if( _s_midi_controller == midi::BCF2000 )
		{
			if( _b_fader_contact_change )
			{
				for( INT32 i = 0; i < FADER_NB; ++i )	//only 8
					_b_fader_contact[i] = _b_fader_contact_ask[i];
				_b_fader_contact_change = false;
			}
		}
		else if( _s_midi_controller == midi::MACKIE )
		//but
		{
			send_gbut( 2, _b_vpot_shift ? BUT_FLASH : BUT_OFF );
			send_gbut( 12, _b_fader_shift ? BUT_FLASH : BUT_OFF );
			send_gbut( 7, c_midi::b_bank_direct ? BUT_FLASH : BUT_OFF );

			send_gbut( 36, _slice_offset_ui == 0 ? BUT_ON : BUT_OFF );
			send_gbut( 37, _slice_offset_ui == 128 ? BUT_ON : BUT_OFF );
			send_gbut( 38, _slice_offset_ui == 256 ? BUT_ON : BUT_OFF );

			send_gbut( 48, c_midi::b_dynamic_loop ? BUT_ON : BUT_OFF );	// looping LED
			send_gbut( 55, c_midi::b_dynamic_pause ? BUT_FLASH : BUT_OFF );	// pausing LED
			send_gbut( 56, c_midi::b_dynamic_play ? ( c_midi::b_dynamic_loop ? BUT_FLASH : BUT_ON ) : BUT_OFF );	// playing LED
			send_gbut( 57, c_midi::b_dynamic_record ? BUT_FLASH : BUT_OFF );	// recording LED

			// update display when playing
			_b_dynamic_play = c_midi::b_dynamic_play;
			//update display when recording
			_b_dynamic_record = c_midi::b_dynamic_record;
			if( _b_fader_contact_change )
			{
				for( INT32 i = 0; i < FADER_NB; ++i )	//only 8
					_b_fader_contact[i] = _b_fader_contact_ask[i];
				make_line_b();
				_b_fader_contact_change = false;
			}
			//	for( i=1; i<=LED_NB; ++i )
			//		send_led( i, c_midi::get_control( i/8+2,( i%8 )+1 ) );

			//	mackie.send_line( 1, mackie_line_b.get() );
		}
		_b_need_sync = false;
	}
}
