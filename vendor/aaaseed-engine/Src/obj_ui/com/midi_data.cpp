#include "midi_data.h"
#include "file/file_csv.h"
#include "file/asc_parser.h"
#include "asc_line.h"
#include "obj_ui/com/net.h"
#include "time/aaa_time.h"
#include "obj_ui/tracker/mackie.h"
#include "obj_ui/tracker/neat.h"
#include "math/v.h"
#include "spy.h"
#include "strnum.h"
#include "infrastructure/param/param_focus.h"
#include "infrastructure/layer/layer.h"
#include "infrastructure/layer/layers.h"
#include "infrastructure/layer/module.h"
#include "infrastructure/param/trax.h"
#include "infrastructure/param/traxs.h"
#include "obj_ui/com/osc/OscPrintReceivedElements.h"

//	convention to know if we start at 0 or
//	if a parameter or a variable is named
//		index is start at 0
//		id it start at 1
//	array are indexed (start at 0)
//	by default functiion should use id

namespace{
	REAL	midi_velocity[c_midi::CHANNEL_NB][c_midi::NOTE_NB];
}

void	midi_velocity_clear()
{
	clear_vr( &(midi_velocity[0][0]), c_midi::CHANNEL_NB * c_midi::NOTE_NB );
}

void	midi_init()
{
	midi_velocity_clear();
	c_mackie::alloc_slices();
}
void	midi_deinit()
{
	c_mackie::dealloc_slices();
}

namespace
{
	c_midi_universe	midi_universe_def;
	CHAR str_midi[128];	//todo	do better

	//to we don't deal with -1 anymore
	CHAR record_data_name[]		=	"record_data.---.csv";
	CHAR record_data_name_pat[]	=	"record_data.*";
	C_PCHAR midi_record_make_name( INT32 nb )
	{
		strnum::make( record_data_name + 12, 3, nb );
		return record_data_name;
	}

	CHAR dynamic_record_data_name[]	=	"record_data.---.---.csv";
	C_PCHAR midi_dynamic_record_make_name( INT32 nb, INT32 version )
	{
		strnum::make( dynamic_record_data_name + 12, 3, nb );
		strnum::make( dynamic_record_data_name + 16, 3, version );
		return dynamic_record_data_name;
	}
}

c_midi_universe*	c_midi_universe::cur = &midi_universe_def;
c_midi_patchs		c_midi_data::global_patchs;

FINLINE	REAL	c_midi_data::get_control( INT32 ch_id, INT32 ctl_index )
{
	return _control [ch_id-1] [ctl_index];
}
REAL*	c_midi_data::get_control_pt( INT32 ch_id, INT32 ctl_index )
{
	return &_control [ch_id-1] [ctl_index];
}


FINLINE	void	c_midi_data::set_control_low( INT32 ch_id, INT32 ctl_index, REAL val )
{
	_control [ch_id-1] [ctl_index] = val;
}

FINLINE	void	c_midi_data::set_control( INT32 ch_id, INT32 ctl_index, REAL val )
{	
	set_control_low( ch_id, ctl_index, val );

	INT32	ch, ctl;
	_patchs.begin_find();
	while( _patchs.find_and_get_other( ch_id, ctl_index, ch, ctl ) )
		set_control_low( ch, ctl, val );	//todo we should do set_control() but we have first to limit recursion

	global_patchs.begin_find();
	while( global_patchs.find_and_get_other( ch_id, ctl_index, ch, ctl ) )
		set_control_low( ch, ctl, val );	//todo we should do set_control() but we have first to limit recursion
}

c_midi_data::c_midi_data()
{
	clear();
	if( _map_record )
		_it_record_insert = _map_record->begin();
	//else
	//	_it_record_insert = nullptr;
}

//todo solve asymmetry, e.g. probably version issues on load message in next function
void	c_midi_data::save_dynamic( INT32 index )
{
	// generate file name
	C_PCHAR buf;

	INT32	size = (INT32)_stack_record.size();
	for( INT32 i = 0; i < size; ++i )
	{
		if( i == 0 )
			buf = midi_record_make_name( index );
		else
			buf = midi_dynamic_record_make_name( index, i );

		FILE* f_save = file_csv::open( buf, true );
			INT32	nb_write = 0;
			// save the full buffers
			std::multimap< INT32, SEQ_REC_MAP >::iterator	it;
	//		SEQ_REC_MAP*	pt;
			//todo should be indexed by i no ? (Maa ask, Claude confirm)
			for( it = _stack_record[i]->begin(); it != _stack_record[i]->end(); ++it )
			{
				fprintf( f_save, "%d; %d; %d; %f\n", it->first, it->second._channel_id, it->second._control_index, it->second._value );
				++nb_write;
			}
			c_midi::CLASS_PRINT_STRING( "Bank %d, Sequencer save, %d record saved in version %d", index, nb_write, i );
		file_csv::close( f_save );
	}

	if( !_map_record )
		return;
	if( _map_record->size() > 0 )
	{
		if( size > 0 )
			buf = midi_dynamic_record_make_name( index, size );
		else
			buf = midi_record_make_name( index );

		FILE* f_save = file_csv::open( buf, true );
			INT32	nb_write = 0;
			// save the full buffers
			std::multimap< INT32, SEQ_REC_MAP >::iterator	it;
	//		SEQ_REC_MAP*	pt;
			for( it = _map_record->begin(); it != _map_record->end(); ++it )
			{
			//	*pt = it->second;
				fprintf( f_save, "%d; %d; %d; %f\n", it->first, it->second._channel_id, it->second._control_index, it->second._value );
				++nb_write;
			}
			c_midi::CLASS_PRINT_STRING( "Bank %d, Sequencer save, %d record saved", index, nb_write );
		file_csv::close( f_save );
	}
}

void	c_midi_data::load_dynamic( INT32 index )
{
	// generate file name
	C_PCHAR	buf;
	INT32	version = 0;

	for(;;)
	{
		if( version == 0 )
			buf = midi_record_make_name( index );
		else
			buf = midi_dynamic_record_make_name( index, version );

		// first check if file exist
		if( !c_file::is_exist( buf ) )
			return;
		//todo file_csv implie no vfile ....
		FILE* f_open = file_csv::open( buf, false );
			if( f_open )
			{
				char	str_scan[256];
		//		//load slice current first
				snprintf( str_scan, 255, "%%d; %%d; %%d; %%f\n" );
				INT32	when, channel_id, control_index;
				_dynamic_rec_start_date = 0;
				when = channel_id = 0;
				control_index = -1;
				REAL value;
				value = 0.0;
				// reset duration and record_number;
				_duration = 0.0f;
				_record_number = 0;
				// create a new multimap
				dynamic_new_map( false );
		//		_it_record_insert = map_record_.begin();
				while( fscanf( f_open, str_scan, &when, &channel_id, &control_index, &value ) == 4 )
				{
					SEQ_REC_MAP	buf_map;
					buf_map._channel_id		= channel_id;
					buf_map._control_index	= control_index;
					buf_map._value			= value;
					_it_record_insert = _map_record->insert( _it_record_insert, Seq_Rec_Pair( when, buf_map ) );
					_duration = when * REAL(.0001);
					++_record_number;	
				}
			}
		//	_b_record = false;
			c_midi::CLASS_PRINT_STRING( "Bank %d, Sequencer load, %d record loaded in version %d", index, _record_number, version );
		file_csv::close( f_open );
		_b_playing = false;
		++version;
		_dynamic_pause_time = 0;
	}
}

INT32	c_midi_data::save_data( FILE* file, INT32 index )
{
	INT32	nb_write = 0;
	if( !data_is_empty() )
	{
		SPY_PUSH_RANGE( "midi_data::save_data", spy::FILE_LOW );

			CHAR	line[1024];

			snprintf( line, 1023, "bank_add( %d );\n", index );
			fprintf( file, line );

			if( !_o_data_name.is_empty() )
			{
				snprintf( line, 1023, "set_name( \"%s\" );\n", _o_data_name.get() );
				fprintf( file, line );
			}

			_patchs.save( file );
			fprintf( file, "data =\n{\n" );
			nb_write = file_csv::write_real_grid( file, &_control[0][0], c_midi::CONTROL_NB, c_midi::CHANNEL_NB );
			fprintf( file, "};\n" );

			// save dynamic data
			//todo maa remove -1 in nov 2017
			if( index >= 0 )
				save_dynamic( index );

		SPY_POP_RANGE();
	}
	return nb_write;
}

INT32	c_midi_data::load_data( FILE* file, INT32 index, bool b_load_record_file )
{
	INT32			nb_read = 0;
	SPY_PUSH_RANGE( "midi_data::load_data", spy::FILE );

		CHAR			line[1026];
		c_asc_parser_cstring	parser;

		_o_data_name.erase();
		while( asc_line::get_next_line( file, line, 1024 ) != EOF ) 
		{
			CHAR*	str;
			parser.set_start( line );
		//		DBG_PRINT_STRING( line );
			if( str = parser.get_next_word() )
			{
				if( str_is_equal( str, "set_name(" ) )
				{
					CHAR* str2;
					str2 = parser.get_next_word();
					if( str2 )
					{
						if( c_midi::b_verbose_read_bank )
							c_midi::CLASS_PRINT_STRING( "Bank %3d named : \"%s\"", index, str2 );
						_o_data_name.set( str2 );
					}
		//				fget( file, "\"%s\"\n", str );	//todo check
				}
				else if( str_is_equal( str, "add_patch(" ) )
				{
					CHAR* str2 = parser.get_rest();
					if( str2 )
						_patchs.add( str2 );
				}
				else if( str_is_equal( str, "data" ) )
				{
					if( c_midi::b_verbose_read_bank )
						_patchs.print();
					str = parser.get_next_word();
					if( str_is_equal( str, "=" ) )
					{
						if( asc_line::get_next_line( file, line, 1024 ) != EOF )
						{
							if( str_is_equal( line, "{\n" ) )
								nb_read += file_csv::read_float_grid( file, &_control[0][0], c_midi::CONTROL_NB, c_midi::CHANNEL_NB );
						}
					}
					break;
				}
			}
			else
				break;
		}

		// load dynamic data
		//todo only load when we access the bank
		//todo maa remove -1 in nov 2017
		if( b_load_record_file && index >= 0 )
			load_dynamic( index );

	SPY_POP_RANGE();
	return nb_read;
}

void	c_midi_data::clear()
{
	REAL*	pt = &_control[0][0] - 1;
	for( INT32 i = c_midi::CONTROL_NB * c_midi::CHANNEL_NB; i > 0; --i )
		*++pt = 0.;
	_dynamic_rec_start_date = 0;
	// clear data
	while( _map_record )
	{
		dynamic_clear_data();
		_map_record = _stack_record.back();
		_stack_record.pop_back();
	}

}

bool	c_midi_data::data_is_empty()
{
	REAL*	pt = &_control[0][0]-1;
	for( INT32 i = c_midi::CONTROL_NB * c_midi::CHANNEL_NB; i > 0; --i )
		if( *++pt != 0. )
			return false;
	return true;
}

void	c_midi_data::channel_copy_to( c_midi_data* dst, INT32 ch_id, INT32 ctl_index_start, INT32 ctl_index_stop )
{
	//todo make sure we don't go out of bound, we limit the range
	//  should be done by the callers ?
	if( ctl_index_start <= ctl_index_stop )
	{
		REAL*	RESTRICT pt_src = &_control     [ch_id-1] [c_midi::bank_direct_begin] - 1;
		REAL*	RESTRICT pt_dst = &dst->_control[ch_id-1] [ctl_index_start] - 1;
		for( INT32 i = ctl_index_stop - ctl_index_start + 1; i > 0; --i )
			*++pt_dst = *++pt_src;
	}
}

/*
void	c_midi_data::record_copy_to( c_midi_data *dst )
{
	//INT32	i;
	//REAL*	pt = &control[ch-1][0]-1;
	//REAL*	pt_dst = &dst->control[ch-1][0]-1;
	//for( i = CONTROL_NB_IN_BANK; i>0; --i )
	//	*++pt_dst = *++pt;

//	if( !list_record_event_.empty() )
//		{
//		INT32	index, index_int ;
//		index = 0;
//		index = _record_number / ( SEQ_EVENTS_NB ) - 1;
//		index_int = IMOD( _record_number, ( SEQ_EVENTS_NB ) );
//		list<SEQ_REC*>::iterator	it;
//		SEQ_REC*	pt;
//
//		INT32	index_for;
//		index_for = 0;
//		// hack, use MEMCPY for fast version, only for testing Christian
//		for( it = list_record_event_.begin(); it != list_record_event_.end(); ++it )
//			{
//			pt = nullptr;
//			pt = *it;
//			if( pt )
//				{
////				INT32	record_index;
////				record_index = IMOD( _record_number, SEQ_EVENTS_NB );
//				if( index == index_for )
//					{
//					for( INT32 i = 0; i < index_int; ++i )
//						{
//						dst->dynamic_record( pt->channel_id_, pt->control_id_, pt->value_, pt->date_ );
//						++pt;
//						}
//					}
//				else
//					for( INT32 i = 0; i < SEQ_EVENTS_NB; ++i )
//						{
//						dst->dynamic_record( pt->channel_id_, pt->control_id_, pt->value_, pt->date_ );
//						++pt;
//						}
//				}
//			++index_for;
//			}
//		}
}
*/

void	c_midi_data::send_to_net( INT32 ch )
{
	REAL*	pt = &_control[ ch-1 ][0] - 1;
	for( INT32 i = 0; i < CONTROL_NB_IN_BANK; ++i )
		net->send_midi_control( ch, i, *++pt );
}

void	c_midi_data::send_to_net()
{
	send_to_net( 1 );
	send_to_net( 2 );
	send_to_net( 3 );
	send_to_net( 4 );
	send_to_net( 5 );
	send_to_net( 6 );
	send_to_net( 9 );
	send_to_net( 10 );
}

void	c_midi_data::bank_copy_to( c_midi_data* dst, INT32 ctl_start, INT32 ctl_stop )
{
	//	was a mackie hack ?
	//	channel_copy_to( dst, 1 );	//2,3,4,5,6,9,10
	for( INT32 i = 1; i <= c_midi::CHANNEL_NB; ++i )
		channel_copy_to( dst, i, ctl_start, ctl_stop );

//danger it trigger crash
//hack		was just removed 
#if 0
	dst->_patchs = _patchs;
#endif
	// copy recording
	//	record_copy_to( dst );
}

void	c_midi_data::channel_interpolate( c_midi_data* srca, c_midi_data* srcb, REAL t, INT32 ch_id )
{
	--ch_id;
	interpolate_v1r_list( &_control[ch_id][0], &srca->_control[ch_id][0], &srcb->_control[ch_id][0], t, c_midi::CONTROL_NB );
}

void	c_midi_data::dynamic_restart()
{
	_b_stop = false;
	_b_playing = false;
	_play_index = 0;
	_dynamic_pause_time = 0;
}

void	c_midi_data::dynamic_pause_begin()
{
	_dynamic_pause_start_time = INT32(aaa::time::get_real_time() * 1000.);
}

void	c_midi_data::dynamic_pause_end()
{
	INT32 dynamic_pause_stop_time = INT32(aaa::time::get_real_time() * 1000.);
	_dynamic_pause_time += dynamic_pause_stop_time - _dynamic_pause_start_time;
}

bool	c_midi_data::dynamic_play( SEQ_REC_MAP*	data, INT32* date )
{
	//	printf( "dynamic_play\n" );
	if( _record_number == 0 )
	{
		// no record
		_play_index = 0;
		_b_stop = true;
		_b_playing = false;
		return false;
	}

	if( _play_index == _record_number )
	{
		_play_index = 0;
		_b_stop = true;
		_b_playing = false;
		_it_map_play = _map_record->begin();
		return false;
	}

	if( _it_map_play == _map_record->end() )
	{
		_play_index = 0;
		_b_stop = true;
		_b_playing = false;
	//	return false;
		_it_map_play = _map_record->begin();
	}

	INT32	t;
	if( !_b_playing )
	{
		_dynamic_play_start_date = INT32(aaa::time::get_real_time() * 1000.);
		t = 0;
		_it_map_play = _map_record->begin();
		_b_playing = true;
//		++_it_map_play;
	}
	else
	{
		t =	INT32(aaa::time::get_real_time() * 1000.) - _dynamic_play_start_date - _dynamic_pause_time;
	}


	if( _it_map_play->first < t )
	{
		*data = _it_map_play->second;
		*date = _it_map_play->first;
		++_play_index;
		_time = t / 1000.0f; 
		if( _play_index == _record_number )
		{
			_b_stop = true;
			_b_playing = false;
		}
		// go to next record
		++_it_map_play;
		return true;
	}
	return false;
}

void	c_midi_data::dynamic_stop()
{
}

void	c_midi_data::dynamic_clear_data()
{
//	while( _map_record )
	if( _map_record )
	{
		_map_record->clear();
		_it_record_insert = _map_record->begin();
		_it_map_play = _map_record->begin();
	}
	_record_number = 0;
	_duration = 0.0f;
	_time = 0.0f;
	_play_index = 0;
}

void	c_midi_data::dynamic_record( INT32 channel_id, INT32 control_index, REAL what, INT32 when )
{
	if( !_b_record )
		return;

	SEQ_REC_MAP	buf;
//	INT32		index;
//	INT32		index_buffer;
//	index_buffer = IMOD( _record_number, SEQ_EVENTS_NB );

	buf._channel_id = channel_id;
	buf._control_index = control_index;
	buf._value = what;


	if( !_map_record )
	{
		// no recording yet
		dynamic_new_map( false );
	}
	_it_record_insert = _map_record->insert( _it_record_insert, Seq_Rec_Pair( when - _dynamic_rec_start_date, buf ) );


	_duration = MAX( _duration, ( REAL ) ( ( when - _dynamic_rec_start_date ) / 1000.0 ) );
	++_record_number;
	//if( vec_record_.empty() || index_buffer == 0 )
	//	{
	//	// no recording buffer created or current one is full
	//	p_buf = add_dynamic_buffer();
	//	if( !p_buf )
	//		{
	//		// could not create buffer, stop recording
	//		_b_record = false;
	//		return;
	//		}
	//	// first record
	//	index = 0;
	//	}
	//else
	//	{
	//	// get last buffer
	//	p_buf = vec_record_.back();
	//	index = index_buffer;
	//	}
	//// go to current index
	//p_buf = p_buf + index;
	//// add data
	//p_buf->date_  = when - _dynamic_rec_start_date;
	//p_buf->channel_id_ = channel_id;
	//p_buf->control_index = control_index;
	//p_buf->value_ = what;
	//_duration = when / 1000.0;
	//++_record_number;
}

void	c_midi_data::bank_interpolate( c_midi_data* src_a, c_midi_data* src_b, REAL t )
{
	SPY_PUSH_RANGE( "midi_data::bank_interpolate", spy::COL_1 );
		//INT32	i;
		// we don't interpolate ch 16 ( MIDI_CHANNEL_NB == 16 ) because its a special case
		for( INT32 i = 1; i <c_midi::CHANNEL_NB; ++i )
			channel_interpolate( src_a, src_b, t, i );
		_patchs = c_midi_universe::cur->_midi_data._patchs;
	SPY_POP_RANGE();
}

void	c_midi_data::set_record( bool recording )
{
	_b_record = recording;
}

void	c_midi_data::dynamic_new_map( bool b_copy )
{
	if( _map_record )
	{
		// add current recording to stack
		_stack_record.push_back( _map_record );
	}

	std::multimap< INT32, SEQ_REC_MAP >*	new_map;
	new_map = new std::multimap< INT32, SEQ_REC_MAP >;// ( _map_record );
	// copy data from previous recording
	if( _map_record && b_copy )
		new_map->insert( _map_record->begin(), _map_record->end() );
	// the new recording is the current recording
	_map_record = new_map;
	// iterators on first element
	_it_record_insert = _map_record->begin();
	_it_map_play = _map_record->begin();

	//	_b_record = true;
}

c_midi_data::~c_midi_data()
{
	// destructor
	//todoqqq implement destructor, clear data structures
	// todofranz dealloc
}


void	c_midi::set_bank( INT32 in )
{
	in = c_midi_universe::make_valid_bank_nb( in );
	if( bank_cur != in )
	{
		if( b_verbose_event )
			DBG_PRINT_STRING( "%s( %d )", __FUNCTION__, in );

		b_dynamic_record = false;
		bank_cur = in;
		bank_cur_ui = in;
		if( b_bank_direct )
		{
			load_bank();
		}
		if( _mackie->_s_midi_controller == midi::MACKIE )
		{
			_mackie->set_line_a_left( c_midi_universe::cur->_midi_bank[bank_cur].get_data_name() );
//			mackie->send_assignement( bank_cur_ );
/*			if( ( bank_cur_ & 0x3 ) == 0 )
				mackie->send_time_code( "          " );
			else if( ( bank_cur_ & 0x3 ) == 1 )
				mackie->send_time_code( "AAASTAGE  " );
			else if( ( bank_cur_ & 0x3 ) == 2 )
				mackie->send_time_code( "TRANSCAM  " );
			else if( ( bank_cur_ & 0x3 ) == 3 )
				mackie->send_time_code( "CHEZ UTRAM" );
*/
		}
	}
		
}

void	c_midi::load_bank()
{
	c_midi_universe::cur->_midi_bank[bank_cur].bank_copy_to( &( c_midi_universe::cur->_midi_data), c_midi::bank_direct_begin, c_midi::bank_direct_end );
	c_midi::PRINT_STRING( "bank %3d loaded", bank_cur );
	if( c_net::b_midi_send )
		c_midi_universe::cur->_midi_bank[bank_cur].send_to_net();	//hacknow
	_mackie->set_need_sync();
}

void	c_midi::save_bank_a()
{
	c_midi_universe::cur->_midi_to_save_a.bank_copy_to( &( c_midi_universe::cur->_midi_bank[bank_cur]), c_midi::bank_direct_begin, c_midi::bank_direct_end );
	c_midi::PRINT_STRING( "store A saved in bank %3d", bank_cur );
	load_bank();
}

void	c_midi::save_bank_b()
{
	c_midi_universe::cur->_midi_to_save_b.bank_copy_to( &( c_midi_universe::cur->_midi_bank[bank_cur]), c_midi::bank_direct_begin, c_midi::bank_direct_end );
	c_midi::PRINT_STRING( "store B saved in bank %3d", bank_cur );
	load_bank();
}

void	c_midi::ask_bank_save_a()
{
	//if ( static_get_control( ch, 44 ) == 1. )
	b_bank_a_save_trig = true;
}

void	c_midi::ask_bank_save_b()
{
	//if ( static_get_control( ch, 44 ) == 1. )
	b_bank_b_save_trig = true;
}

void	c_midi::store_bank_a()
{
	c_midi::PRINT_STRING( "stored in A" );
	c_midi_universe::cur->_midi_data_inter.bank_copy_to( &( c_midi_universe::cur->_midi_to_save_a), c_midi::bank_direct_begin, c_midi::bank_direct_end );
}

void	c_midi::store_bank_b()
{
	c_midi::PRINT_STRING( "stored in B" );
	c_midi_universe::cur->_midi_data_inter.bank_copy_to( &( c_midi_universe::cur->_midi_to_save_b), c_midi::bank_direct_begin, c_midi::bank_direct_end );
}

void	c_midi::freeze_bank_inter()
{
	c_midi::PRINT_STRING( "Freeze Interpolation" );
	c_midi_universe::cur->_midi_data_inter.bank_copy_to( &( c_midi_universe::cur->_midi_data), c_midi::bank_direct_begin, c_midi::bank_direct_end );
	_mackie->set_need_sync();
}

void	c_midi::ask_bank_prev()
{
//	c_midi::PRINT_STRING( "stored in A" );
//	_b_dynamic_play = false;
	b_dynamic_record = false;
	if( b_verbose_event )
		DBG_PRINT_STRING( "%s()", __FUNCTION__ );		
	bank_cur_ui = c_midi_universe::make_valid_bank_nb( bank_cur_ui - 1 );
}

void	c_midi::ask_bank_next()
{
//	c_midi::PRINT_STRING( "stored in A" );
//	_b_dynamic_play = false;
	b_dynamic_record = false;
	if( b_verbose_event )
		DBG_PRINT_STRING( "%s()", __FUNCTION__ );		
	bank_cur_ui = c_midi_universe::make_valid_bank_nb( bank_cur_ui + 1 );
}

void	c_midi::ask_bank_load()
{
	b_bank_load_trig = true;
}

void	c_midi::switch_bank_direct()
{
	b_bank_direct = !b_bank_direct;
	ask_bank_load();
}

FINLINE	bool	c_midi::is_valid_channel( INT32 channel_id )
{
	if( OUTSIDE_MIN_MAX( channel_id, 1, c_midi::CHANNEL_NB ) )
	{
		ERR_PRINT_STRING( "MIDI : CHANNEL %d impossible (channel in [%d,%d]), skipping", channel_id, 1, c_midi::CHANNEL_NB ); 
		return false;
	}
	return true;
}
FINLINE	bool	c_midi::is_valid_channel_control( INT32 ch_id, INT32 ctl_index )
{
	if( !is_valid_channel( ch_id ) )
		return false;
	if( OUTSIDE_MIN_MAX( ctl_index, 0, c_midi::CONTROL_NB-1 ) )
	{
		ERR_PRINT_STRING( "MIDI CHANNEL %d : CONTROL %d impossible (control in [%d,%d]), skipping", ch_id, ctl_index, 0, c_midi::CONTROL_NB-1 ); 
		return false;
	}
	return true;
}
FINLINE	bool	c_midi::is_valid_channel_note( INT32 ch_id, INT32 note_index )
{
	if( !is_valid_channel( ch_id ) )
		return false;
	if( OUTSIDE_MIN_MAX( note_index, 0, c_midi::NOTE_NB-1 ) )
	{
		ERR_PRINT_STRING( "MIDI CHANNEL %d : NOTE %d impossible (note in [%d,%d]), skipping", ch_id, note_index, 0, c_midi::NOTE_NB-1 ); 
		return false;
	}
	return true;
}

namespace
{
	static	bool	b_midi_auto_connect_trax = false;

	void	auto_connect_trax( INT32 channel_id, INT32 control_index )
	{
		if( focus_param::get_param() && focus_param::get_obj() )
		{
			c_traxs* p_traxs;
			if( c_layer::get_ui() )
				p_traxs = c_layers::get_ui()->get_traxs_always();
			else
				p_traxs = c_module::get_ui()->get_traxs();
			if( p_traxs )
			{
				c_trax* p_trax = p_traxs->trax_plug_out( focus_param::get_obj(), focus_param::get_param() );
				if( p_trax )
					p_trax->set_fn_midi( channel_id - p_traxs->get_channel_offset(), control_index - p_traxs->get_control_offset() );
			}
		}
	}
}
void	c_midi::trig_auto_connect_trax()
{
	CLASS_PRINT_STRING( "AutoConnect -> Waiting for midi input" );
	b_midi_auto_connect_trax = true;
}

//move this on the OSC side
bool get_ii( CONST osc::ReceivedMessage& msg, INT32& a, INT32& b )
{
	osc::ReceivedMessageArgumentIterator it = msg.ArgumentsBegin();
	osc::ReceivedMessageArgumentIterator it_end = msg.ArgumentsEnd();
	if( it == it_end )
		return false;
	a = INT32( it->get_as_double() );
	if( ++it == it_end )
		return false;
	b = INT32( it->get_as_double() );
	return true;
}

bool get_iif( CONST osc::ReceivedMessage& msg, INT32& a, INT32& b, REAL& f )
{
	osc::ReceivedMessageArgumentIterator it = msg.ArgumentsBegin();
	osc::ReceivedMessageArgumentIterator it_end = msg.ArgumentsEnd();
	if( it == it_end )
		return false;
	a = INT32( it->get_as_double() );
	if( ++it == it_end )
		return false;
	b = INT32( it->get_as_double() );
	if( ++it == it_end )
		return false;
	f = REAL(it->get_as_double());
	return true;
}

void	c_midi::osc_process_message( CONST osc::ReceivedMessage& msg )
{
	INT32 ch,index;
	REAL value;
	o_str& o = o.push_name( msg.AddressPattern() + 10 );
		CHAR c = o.get_char( 0 );
		if( c == 'c' || c == 'C' )
		{
			if( o.is_equal_nocase( "ctrl" ) || o.is_equal_nocase( "control" ) )
			{
				if( get_iif( msg, ch, index, value ) )
				{
					if( b_verbose )
						VERBOSE_PRINT_STRING( "OSC receive MIDI channel(%d) control(%d) : %f", ch, index, value );
					static_set_control_silent( ch, index, value, 0 );
				}
				else
					ERR_PRINT_STRING( "OSC receive MIDI control : wrong arguments not iif" );
			}
		}
		else if( c == 'n' || c == 'N' )
		{
			if( o.is_equal_nocase( "note/on" ) || o.is_equal_nocase( "note_on" ) )
			{
				if( get_iif( msg, ch, index, value ) )
				{
					if( b_verbose )
						VERBOSE_PRINT_STRING( "OSC receive MIDI channel(%d) note_on(%d) : %f", ch, index, value );
					static_set_velocity( ch, index, value );
				}
				else
					ERR_PRINT_STRING( "OSC receive MIDI note_on : wrong arguments not iif" );
			}
			else if( o.is_equal_nocase( "note/off" ) || o.is_equal_nocase( "note_off" ) )
			{
				if( get_ii( msg, ch, index ) )
				{
					if( b_verbose )
						VERBOSE_PRINT_STRING( "OSC receive MIDI channel(%d) note_off(%d) : %f", ch, index );
					static_set_velocity( ch, index, 0. );
				}
				else
					ERR_PRINT_STRING( "OSC receive MIDI note_off : wrong arguments not ii" );
			}
		}
	o.pop_name();
}

//todoq change param order in next fns
bool	c_midi::static_set_control_silent( INT32 channel_id, INT32 control_index, REAL value, INT32 when, bool b_send, bool b_record )
{
	if( !is_valid_channel_control( channel_id, control_index ) )
		return false;

	//	if( b_control_send_to_lua )
	//		g_lua_wrap_master->do_fn_pass_iir( nullptr, "aaa.midi.hook_set_control", channel_id, control_index, what );

	control_change_last_channel_id_out    = channel_id;
	control_change_last_control_index_out = control_index;

	c_midi_universe::cur->_midi_data.set_control( channel_id, control_index, value );
	if( channel_id != 16 )
	{
		//hack maybe we should put data in midi_data, and copy when needed
		//	midi_universe_cur->midi_data.dynamic_record( channel_id, control_index, what, when );
		if( b_record )
			c_midi_universe::cur->_midi_bank[bank_cur].dynamic_record( channel_id, control_index, value, when );
	}

	if( b_send && c_net::b_midi_send && is_obj_exist_and_active(net) )
		net->send_midi_control( channel_id, control_index, value, when );	//todo deal with when param


	if( b_midi_auto_connect_trax )
	{
		auto_connect_trax( channel_id, control_index );
		b_midi_auto_connect_trax = false;
	}

	if( channel_id != 16 )	//	for mackie and BCF these are controls (e.g. next bank, next slice.. ) 
		midi_update_auto_patch( channel_id, control_index );
	
	return true;
}

//todoq change param order in next fns
bool	c_midi::static_set_control( INT32 channel_id, INT32 control_index, REAL value, INT32 when, bool b_send, bool b_record )
{
	if( !static_set_control_silent( channel_id, control_index, value, when, b_send, b_record ) )
		return false;

	if( b_verbose && b_verbose_control_change )
	{
		snprintf( str_midi, sizeof(str_midi)-1, "CONTROL %2d --> %f", control_index, value );
		class_print_verbose( str_midi, channel_id, when );
	}

	return true;
}

void	c_midi::static_set_velocity( INT32 channel_id, INT32 note_index, REAL vel, INT32 when, bool b_send )
{
	if( !is_valid_channel_note( channel_id, note_index ) )
		return;
	midi_velocity[channel_id-1][note_index] = vel;

	if( b_verbose && b_verbose_velocity_master && b_verbose_velocity )
	{
		snprintf( str_midi, sizeof(str_midi)-1, "NOTE    %2d VELOCITY %.3f", note_index, vel ); 
		class_print_verbose( str_midi, channel_id, when );
	}
	if( b_send && c_net::b_midi_send && is_obj_exist_and_active(net) )
		net->send_midi_velocity( channel_id, note_index, vel, when );	//todo deal with when param
}

void	c_midi::static_set_noteoff( INT32 channel_id, INT32 note_index, INT32 when )
{
	if( !is_valid_channel_note( channel_id, note_index ) )
		return;
	static_set_velocity( channel_id, note_index, 0., when );
	if( b_verbose && b_verbose_velocity_master && b_verbose_noteoff )
	{
		snprintf( str_midi, sizeof(str_midi)-1, "NOTEOFF %2d", note_index );
		class_print_verbose( str_midi, channel_id, when );
	}
}

void	c_midi::static_set_noteon( INT32 channel_id, INT32 note_index, REAL vel, INT32 when )
{
	if( b_route_noteon_as_control_change_ui )
	{
		if( channel_id == 16 )	//probably the mackie so we don't root
		{
			bank_cur_ui = c_midi_universe::make_valid_bank_nb( 100 * (note_index + INT32(vel*127) ) );
			//set_bank( 100 * which + vel );
		}
		else
		{
			static_set_control( channel_id, note_index, vel, when );
			return;
		}
	}
	if( !is_valid_channel_note( channel_id, note_index ) )
		return;
	if( vel == 0 )
		static_set_noteoff( channel_id, note_index, when );
	else
	{
		static_set_velocity( channel_id, note_index, vel, when );
		if( b_verbose && b_verbose_velocity_master && b_verbose_noteon )
		{
			snprintf( str_midi, sizeof(str_midi)-1, "NOTEON  %2d VELOCITY %.3f", note_index, vel );
			class_print_verbose( str_midi, channel_id, when );
		}
	}
}

REAL	c_midi_universe::get_control( INT32 channel_id, INT32 control_index )
{
	CLAMP_REF( channel_id,    1, c_midi::CHANNEL_NB );
	CLAMP_REF( control_index, 0, c_midi::CONTROL_NB-1 );
	if( channel_id == 16 )
		return _midi_data.get_control( channel_id, control_index );
	else
		return _midi_data_inter.get_control( channel_id, control_index );
}


REAL	c_midi::static_get_control( INT32 channel_id, INT32 control_index )
{
#if AAA_TRACKER_NEAT()
	if( b_midi_done_with_neat )
		return neat_control_get( channel_id, control_index );
	else
#endif //#if AAA_TRACKER_NEAT()
		return c_midi_universe::cur->get_control( channel_id, control_index );
}

//archi	should deal with several and list
namespace {
	INT32	program_change_number = -42;
}

void	c_midi::store_program_change( INT32 channel_id, INT32 program_index, INT32 when )
{
	if( c_midi::b_verbose && c_midi::b_verbose_program_change )
	{
		snprintf( str_midi, sizeof(str_midi)-1, "PROGRAM CHANGE %d", program_index );
		class_print_verbose( str_midi, channel_id, when );
	}
	program_change_number = program_index;
	//	if( c_midi::b_program_send_to_lua )
	//		g_lua_wrap_master->do_fn_pass_ii( nullptr, "aaa", "midi.hook_program_change", channel_id, program_change_number );
}

INT32	midi_program_change_get()
{
	if( program_change_number >= 0 )
	{
		INT32	tmp = program_change_number;
		program_change_number = -42;
		return tmp;
	}
	return 0;		
}

INT32	c_midi_universe::write_file_control( FILE* file )
{
	INT32	nb_write = 0;
	if( file )
	{
		nb_write = _midi_data.save_data( file, -1 );
		for( INT32 i = 0; i < c_midi_universe::MIDI_BANK_NB; ++i )
			nb_write += _midi_bank[i].save_data( file, i );
	}
	return nb_write;
}

INT32	c_midi_universe::read_file_control( FILE* file, bool b_load_record_file )
{
	INT32	nb_read = 0;
	if( file )
	{
		CHAR	line[1026];
		INT32	index;
		while( asc_line::get_next_line( file, line, 1024 ) != EOF ) 
		{
			if( sscanf( line, "bank_add( %d );\n", &index ) == 1 )
			{ 
				c_midi_data* p_bank;
				if( index < 0 )
					p_bank = &( _midi_data );
				else
					p_bank = &( _midi_bank[index] );		//todo test index, it could be higher than current allocation
				nb_read += p_bank->load_data( file, index, b_load_record_file );
			}
			else if( str_is_equal( line, "};\n" ) )
			{
			}
			else
				ERR_PRINT_STRING( "Error parsing Midi bank" );
		}
	}
	c_midi::CLASS_PRINT_STRING( "midi_control_file_read DONE" );
	return nb_read;
}

void	c_midi_universe::interpolate_bank( REAL inter )
{
	if( inter != 0. )
		_midi_data_inter.bank_interpolate( &_midi_data, &_midi_bank[c_midi::bank_cur], inter );	//make sure the value is ok
	else
		_midi_data.bank_copy_to( &_midi_data_inter );
}

void	midi_control_get_band( INT32 channel_id, INT32 start_control_index, INT32 band_nb, FP32* p_bands )
{
	for( INT32 i = start_control_index; i < start_control_index + band_nb; ++i )
		*p_bands++ = c_midi_universe::cur->get_control( channel_id, i );
}


FP32	midi_velocity_get_float( INT32 channel_id, INT32 note_index )
{
	INT32 channel_index	= CLAMP( channel_id-1, 0, c_midi::CHANNEL_NB-1 );
	note_index			= CLAMP( note_index,   0, c_midi::NOTE_NB-1 );
	return	midi_velocity[channel_index][note_index];
}


void	midi_velocity_get_band( INT32 start_note_index, INT32 band_nb, FP32* p_bands )
{
	for( INT32 i = start_note_index; i < start_note_index + band_nb; ++i )
		*p_bands++ = midi_velocity_get_float( 1, i );
}
