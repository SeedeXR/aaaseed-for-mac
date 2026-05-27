#undef WIN32_LEAN_AND_MEAN

#ifndef AAA_SOUND_NEW_H
#	include "sound_new.h"
#endif		 		 
#ifndef AAA_SND_INPUT_H
#	include "snd_input.h"
#endif
#ifndef AAA_SOUND_INPUT_H
#	include "sound_input.h"
#endif
#include "snd_player.h"
#include "obj_ui/tracker/trackers.h"
#include "snd_master.h"

FACTORY_ABSTRACT_CREATE_V1( c_sound, sound, Sound Settings );


C_PCHAR_C	c_sound::player_status_str[PLAYER_STATUS_MAX_NB] = 
{
	"Stopped",
	"Playing",
	"Paused",
	"Playing and looping",
	"Error",
	"Unknown"
};

CONSTRUCTOR_ABSTRACT_CREATE( c_sound )
//	,_b_enable				(	false	)
//	,_input_nb				(	0	)
//	,_output_nb				(	0	)
	,_nb_channel_out		(	0	)
//todo we init it but they are unused
	,_device_in				(	0	)
	,_nb_channel_in			(	0	)
	,_s_sample_rate_in_ui	(	0	)
	,_s_sample_bits_in_ui	(	0	)
	,_device_out			(	0	)
	,_s_sample_rate_out_ui	(	0	)
	,_s_sample_bits_out_ui	(	0	)
{
	for ( INT32 i = 0; i < OUTPUT_NB_MAX; ++i )
		_b_output_enable[ i ] = false;
//	for ( INT32 i = 0; i < INPUT_NB_MAX; ++i )
//		_b_input_enable[ i ] = false;
}

c_sound::~c_sound()
{
	close();
}

//void	c_sound::init_input()		{}
//void	c_sound::init_output()		{}
//void	c_sound::deinit_output()		{}

void	c_sound::open()
{
	//	initialize COM
	enumerate();
	//	init_output();
//	_b_enable = true;
}

void	c_sound::close()
{
	//	close all sound input
	delete_input_all();
	// close all sound output
	delete_player_all();
}


/*
o_str*	c_sound::get_name( INT32 in )
{
	//o_str name;
	//name.add( "Unknown" );
	//return name;
	return nullptr;
}
*/

c_sound_player*		c_sound::create_player_new()
{
	//todo move to obj_ui
	ERR_PRINT_STRING( "%s() unimplemented for class %s", __FUNCTION__, get_class_name() );
	return nullptr;
}

c_sound_input*	c_sound::find_input_by_id( INT32 id )
{
	for( auto const & pt : _sound_input )
	{
		if( pt->get_id() == id )
		{
			//	printf( "%d\n", hd_wind_in);
			return pt;
		}
	}
	//	ERR_PRINT_STRING( "capture window unknown");
	return nullptr;
}


c_sound_input* 	c_sound::add_input()
{
	c_sound_input* pt = create_input_new();
	if( pt )
		_sound_input.push_back( pt );
	return pt;
}


bool	c_sound::delete_input( c_sound_input* pt )
{
	if( pt && pt->is_valid() )
	{
		// stop sound input
		pt->disable();
		// remove sound_input from list
		_sound_input.remove( pt );
		delete pt;
	}
	return true;
}
void	c_sound::delete_input_all()
{
	while( !_sound_input.empty() )
		delete_input( *_sound_input.begin() );
}

c_sound_player*	c_sound::add_player( INT32 device_index )
{
	c_sound_player* pt = create_player_new();
	if( pt )
	{
		pt->set_device( device_index );
		_sound_player.push_back( pt );
	}
	return pt;
}

bool	c_sound::delete_player( c_sound_player* pt )
{
	if( pt )
	{
		pt->stop();
		_sound_player.remove( pt );
		delete pt;
	}
	return true;
}

void	c_sound::delete_player_all()
{
	while( !_sound_player.empty() )
		delete_player( *_sound_player.begin() );
}



INT32	c_sound::enumerate_device_out()
{
	debug_break( "%s() unimplemented, Should not be called, subclass should define it", __FUNCTION__ );
	return 0;
}

INT32	c_sound::enumerate_device_in()
{
	debug_break( "%s() unimplemented, Should not be called, subclass should define it", __FUNCTION__ );
	return 0;
}

void	c_sound::enumerate()
{
	_device_out_count	= enumerate_device_out();
	_device_in_count	= enumerate_device_in();
	_devices_channel_nb	= _device_in_count * 2;
}

bool c_sound::open_control_panel()
{
	debug_break( "%s() unimplemented, Should not be there", __FUNCTION__ );
	return false;
}


void c_sound::dealloc_snd_input()
{
	for( auto& si : _snd_inputs )
	{
		si->disable();
		obj_delete( si );
	}
	_snd_inputs.clear();
}

//extern	c_fifo	fifo_menu_tracker;
void	c_sound::alloc_snd_input( INT32 nb_asked )
{
	if( _snd_inputs.size() == nb_asked )
		return;

//hack focus removed to avoid crash when going to zero input and back to one
//	c_fifo::push();
//	fifo_menu_tracker.set_cur();

	dealloc_snd_input();

	for( INT32 i = 0; i < nb_asked; ++i )
	{
		auto snd_input = c_snd_input::create_obj( (c_obj_ui*)trackers::node_tracker ) ;
//		snd_input->_sound_input = ;
		snd_input->_channel_nb_ui = get_channel_nb_asked_def();
		_snd_inputs.push_back( snd_input );
	}
}

void	c_sound::update( )
{
	alloc_snd_input( 1 );	//todosnd put it in g_master );

	for( auto const & si : _snd_inputs )
		si->update();
}

//todo these save and load should be generalized
//	with the trackers one eventually others
void	c_sound::save_snd_input( o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name( filename_in );
	filename.add( "__" );

	for( INT32 i=0; i<_snd_inputs.size(); ++i )
	{
		filename.set_char( -1, 'a' + i );
		_snd_inputs[i]->save_to_file_add_ext( filename );
	}
	o_str::pop_name();
}


void	c_sound::load_snd_input( o_str CONST & filename_in )
{
	o_str& filename = o_str::push_name( filename_in );
	filename.add( "__" );

	for( INT32 i=0; i<_snd_inputs.size(); ++i )	//todoqq how many should we load ?
	{
		filename.set_char( -1, 'a' + i );
		_snd_inputs[i]->load_from_file_add_ext( filename );
	}
	o_str::pop_name();
}

c_snd_input*	c_sound::get_by_channel( INT32 channel_in )
{
	INT32 snd_input_nb = (INT32)_snd_inputs.size();
	c_snd_input* input = nullptr;
	if( channel_in < 1 )
		debug_break( "%s() channel should be at least 1", __FUNCTION__  );
	else
	{
		INT32 nb = 0;
		for( INT32 i=0; i<snd_input_nb; ++i )
		{
			input = _snd_inputs[ i ];
			if( input )
			{
				nb += input->get_channel_nb();
				if( channel_in <= nb )
					break;
			}
		}
	}
	if( !input && snd_input_nb > 0 )
		input = _snd_inputs[ 0 ];
	return input;
}
