#include "midi_data.h"
#include "language/lua/aaalua_glue.h"

namespace aaalua
{
namespace n_midi
{
	// fn helper
	FINLINE void set_ch_get_index_value( c_lua_state CONST & l, c_midi*& midi, INT32&	index, INT32& value )
	{
		l.check_arg_nb( 4 );
		midi		= l.get_aaa_obj_of_class<c_midi>( 1 );
		INT32 ch_id	= l.get_int32( 2 );
		index		= l.get_int32( 3 );
		value		= CLAMP( l.get_int32( 4 ), 0,127 );
		midi->set_channel( ch_id );	//set midi channel to be used
	}

	//	aaa.midi.send_note_on( obj, channel, note, velocity )
	AAALUACALL( send_note_on )
	{
		LUAAAA_START( L, __FUNCTION__ );
		c_midi*	midi;
		INT32	note_index, velocity;
		set_ch_get_index_value( l, midi, note_index, velocity );	
		midi->send_note_on( note_index, velocity );
		return l.return_nothing();
	}

	//	aaa.midi.send_note_off( obj, channel, note, velocity )
	AAALUACALL( send_note_off )
	{
		LUAAAA_START( L, __FUNCTION__ );
		c_midi*	midi;
		INT32	note_index, velocity;
		set_ch_get_index_value( l, midi, note_index, velocity );
		midi->send_note_off( note_index, velocity );
		return l.return_nothing();
	}

	//	aaa.midi.send_note_pressure( obj, channel, note, velocity )
	AAALUACALL( send_note_pressure )
	{
		LUAAAA_START( L, __FUNCTION__ );
		c_midi*	midi;
		INT32	note_index, velocity;
		set_ch_get_index_value( l, midi, note_index, velocity );
		midi->send_polyphonic_pressure( note_index, velocity );
		return l.return_nothing();
	}

	//	aaa.midi.send_control_change( obj, channel, control, value )
	AAALUACALL( send_control_change )
	{
		LUAAAA_START( L, __FUNCTION__ );
		c_midi*	midi;
		INT32	control_index, value;
		set_ch_get_index_value( l, midi, control_index, value );
		midi->send_control_change( control_index, value );
		return l.return_nothing();
	}

	//	aaa.midi.send_program_change( obj, channel, program )
	AAALUACALL( send_program_change )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 3 );
		c_midi*	midi		= l.get_aaa_obj_of_class<c_midi>( 1 );
		INT32	ch_id		= l.get_int32( 2 );
		INT32	pgm_index	= l.get_int32( 3 );
		midi->set_channel( ch_id );
		midi->send_program_change( pgm_index );
		return l.return_nothing();
	}

	//	value = aaa.midi.get_control( channel, control  )
	AAALUACALL( get_control )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		INT32	ch_id		= l.get_int32( 1 );
		INT32	ctl_index	= l.get_int32( 2 );
		REAL val = c_midi::static_get_control( ch_id, ctl_index );
		return l.return_real( val );
	}

	//	value = aaa.midi.get_velocity( channel, note )
	AAALUACALL( get_velocity )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 2 );
		INT32	ch_id		= l.get_int32( 1 );
		INT32	note_index	= l.get_int32( 2 );
		FP32	val = midi_velocity_get_float( ch_id, note_index );
		return l.return_fp32( val );
	}

	//	aaa.midi.set_control( channel, control, value )
	AAALUACALL( set_control )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 3 );
		INT32	ch_id		= l.get_int32( 1 );
		INT32	ctl_index	= l.get_int32( 2 );
		REAL	val			= l.get_real( 3 );
		c_midi::static_set_control_silent( ch_id, ctl_index, val, 0, true, true );
		return l.return_nothing();
	}
	
	//	aaa.midi.load_global( filename )
	AAALUACALL( load_global )
	{
		LUAAAA_START( L, __FUNCTION__ );
		l.check_arg_nb( 1 );
		C_PCHAR_C filename = l.get_str();
		c_midi::load_global( filename );
		return l.return_nothing();
	}

	//	aaa.midi.save_global()
	//	aaa.midi.save_global( filename )
	AAALUACALL( save_global )
	{
		LUAAAA_START( L, __FUNCTION__ );
		INT32 arg_nb = l.get_arg_nb_max( 1 );
		C_PCHAR_C filename = (arg_nb>0) ? l.get_str() : nullptr;
		c_midi::save_global( filename );
		return l.return_nothing();
	}

	//	aaa.midi.add_patch_global( channel, control, channel_dst, control_dst )
	AAALUACALL( add_patch_global )
	{
		LUAAAA_START( L, __FUNCTION__ );
		//INT32 arg_nb =
		l.get_arg_nb( 4 );
		c_midi_data::global_patchs.add(	l.get_int32(1), l.get_int32(2), l.get_int32(3), l.get_int32(4) );
		return l.return_nothing();
	}

	void	register_midi( lua_State* L )
	{
		LUAAAA_START( L, __FUNCTION__ );

		l.define_table( "midi" );
			ADD_FN( get_control			);
			ADD_FN( set_control			);
			ADD_FN( get_velocity		);

			ADD_FN( send_note_on		);
			ADD_FN( send_note_off		);
			ADD_FN( send_note_pressure	);
			ADD_FN( send_control_change	);
			ADD_FN( send_program_change	);

			ADD_FN( load_global			);
			ADD_FN( save_global			);

			ADD_FN( add_patch_global	);

		lua_pop( L, 1 );	//pop table "midi"
	}

}	//end namespace n_midi
}	//end namespace aaalua
