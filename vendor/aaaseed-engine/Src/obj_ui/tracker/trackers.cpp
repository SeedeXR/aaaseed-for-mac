#include "obj_ui/tracker/trackers.h"
#include "infrastructure/obj/root.h"
#include "obj_ui/bdd/bdd_hardware/bdd_touchco.h"
#include "obj_ui/com/net.h"
#ifndef AAA_CAPTURE_VIDEO_H
#	include "media/video/capture/capture_video.h"
#endif
#include "aaavr.h"


c_node_ui*	trackers::node_tracker = nullptr;

#if AAA_TRACKER_SPACEBALL()
	//INT32	trackers::spaceball_port_nb = 0;
	bool	trackers::b_spaceball_start = false;
#endif	//#if AAA_TRACKER_SPACEBALL()

INT32	trackers::midi_nb				= 0;
INT32	trackers::ftdi_nb				= 0;

#if AAA_TRACKER_HID()
	INT32	trackers::hid_nb				= 0;
#endif	//#if AAA_TRACKER_HID()
#if AAA_TRACKER_ISENSE()
	INT32	trackers::isense_port_nb		= 0;
#endif	//#if AAA_TRACKER_ISENSE()
#if AAA_TRACKER_NVISION()
	INT32	trackers::nvision_port_nb		= 0;
#endif	//#if AAA_TRACKER_NVISION()
#if AAA_TRACKER_ANALOG_WAY()
	INT32	trackers::analog_way_port_nb	= 0;
#endif	//#if AAA_TRACKER_ANALOG_WAY()
#if AAA_TRACKER_DOREMI()
	NT32	trackers::doremi_port_nb		= 0;
#endif	//#if AAA_TRACKER_DOREMI()
#if AAA_TRACKER_GLOVE_5DT()
	INT32	trackers::glove_a_port_nb		= 0;
	INT32	trackers::glove_b_port_nb		= 0;
#endif	//#if AAA_TRACKER_GLOVE()

bool	trackers::b_start_with_joystick_a	= false;
bool	trackers::b_start_with_joystick_b	= false;
//bool	trackers::b_start_with_sound		= false;

#if AAA_TRACKER_MAGELLAN()
	bool	trackers::b_start_with_magellan		= false;
#endif	//#if AAA_TRACKER_MAGELLAN()
#if AAA_TRACKER_PCBIRD()
	bool	trackers::b_start_with_pcbird		= true;
#endif	//#if AAA_TRACKER_PCBIRD()
#if AAA_TRACKER_POLHEMUS()
	bool	trackers::b_start_with_polhemus		= true;
#endif	//#if AAA_TRACKER_POLHEMUS()
#if AAA_TRACKER_TOASTER()
	bool	trackers::b_start_with_toaster		= true;
#endif	//#if AAA_TRACKER_TOASTER()

bool	trackers::b_start_with_dmx			= true;

#if AAA_TRACKER_HYDRA()
	bool	trackers::b_start_with_hydra		= true;
#endif	//#if AAA_TRACKER_HYDRA()

//todoqq
//we should regroup all the tracker in one obj for the interface
#if	AAA_TRACKER()

#	if	AAA_TRACKER_SPACEBALL()
#		include "spaceball.h"
#	endif
#	if	AAA_TRACKER_MAGELLAN()
#		include "magellan.h"
#	endif
#	if	AAA_TRACKER_MIDI()
#		include "obj_ui/com/midi_data.h"
#	endif
#	if	AAA_TRACKER_NEAT()
#		include "obj_ui/tracker/neat.h"
#	endif
#	if	AAA_TRACKER_JOY()
#		include "obj_ui/tracker/joy.h"
		c_joy*		joy_a = nullptr;
		c_joy*		joy_b = nullptr;
#	endif
#	if	AAA_TRACKER_ISENSE()
#		include "aaa_isense.h"
#	endif
#	if	AAA_TRACKER_SND_INPUT()
#		include "media/sound/snd_master.h"
#		include "media/sound/sound_new.h"
#		ifndef AAA_SOUND_INPUT_H
#			include "media/sound/sound_input.h"
#		endif
#	endif
#	if	AAA_TRACKER_HMD()
#ifndef AAA_HMD_H
#	include "obj_ui/tracker/hmd.h"
#endif
#	endif
#	if	AAA_TRACKER_IGLASSES()
#		include "obj_ui/tracker/iglasses.h"
#	endif
#	if	AAA_TRACKER_PCBIRD()
#		include "aaa_ascension.h"
#	endif
#	if	AAA_TRACKER_POLHEMUS()
#		include "polhemus.h"
#	endif
#	if	AAA_TRACKER_NVISION()
#		include "nvision.h"
#	endif
#	if	AAA_TRACKER_ANALOG_WAY()
#		include "analog_way.h"
#	endif
#	if	AAA_TRACKER_GLOVE_5DT()	
#		include "glove5DT.h"
#	endif
#	if	AAA_TRACKER_TOASTER()
#		include "obj_ui/tracker/toaster.h"
#	endif
#	if	AAA_TRACKER_DMX()
#		include "obj_ui/tracker/dmx/dmx.h"
#	endif
#	if	AAA_TRACKER_DOREMI()
#		include "doremi.h"
#	endif
#	if	AAA_TRACKER_FTDI()
#		include "obj_ui/tracker/ftdi.h"
#	endif
#	if	AAA_TRACKER_HID()
#		include "obj_ui/tracker/hid.h"
#	endif
#	if AAA_TRACKER_HYDRA()
#		include "hydra.h"
#	endif
#	if	AAA_TRACKER_WACOM()
#		include "wacom/wacom.h"
//#		include "obj_ui/tracker/wacom/wacom_util.h"
#	endif
#	if	AAA_TRACKER_MEDIAPIPE()
#		include "tracking/mediapipe/mediapipe.h"
#	endif

	//	there is a problem that the user should know
static	CONST	CHAR TRACKER_HEADER[] = "# ";

void	trackers::PRINT_STRING_VA(	C_PCHAR_C h,	C_PCHAR_C fmt, va_list args )
{
	HEADER_PRINT_STRING_VA_GENE( aaa::mess::PRINT_TRACKERS, console::set_text_purple, h ? h : TRACKER_HEADER, fmt, args );
}

void	trackers::PRINT_STRING( C_PCHAR_C header, C_PCHAR fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	PRINT_STRING_VA( header, fmt, args );
	va_end(args);
}

extern	void	gypsy_close();

void	trackers::init()
{
	obj_new( node_tracker )->set_name( "NODE_TRACKERS" );

	if( c_aaavr::b_start_with )
	{
		c_aaavr::c_init();
		c_aaavr::cur = c_aaavr::create_obj( node_tracker );
		c_aaavr::cur->set_focus();
	}
#if AAA_TRACKER_DMX()
	if( b_start_with_dmx )
	{
		g_dmx_cur = c_dmx::create_obj( node_tracker );
		g_dmx_cur->set_focus();
	}
#endif
#if AAA_TRACKER_SND_INPUT()
	//	SOUND
	snd::g_master = snd::c_snd_master::create_obj( node_tracker );
	snd::g_master->set_focus();
#endif
#if AAA_TRACKER_SPACEBALL()
	//if( spaceball_port_nb )
	if(  b_spaceball_start)
	{
		spaceball = c_spaceball::create_obj( node_tracker );
	//	spaceball->set_port(spaceball_port_nb);
		spaceball->set_focus();
	}
#endif
#if AAA_TRACKER_MAGELLAN()
	if( b_start_with_magellan )
	{
		magellan = c_magellan::create_obj( node_tracker );
		magellan->set_focus();
	}
#endif
	midi_init();
	for( INT32 i = 0; i < midi_nb; ++i )
	{
		midi_array[i] = c_midi::create_obj( node_tracker );
		midi_array[i]->set_focus();
	}
	for( INT32 i = midi_nb; i < MIDI_OBJ_MAX_NB; ++i )
		midi_array[i] = nullptr;

#if AAA_TRACKER_JOY()
	if( b_start_with_joystick_a )
	{
		joy_a = c_joy::create_obj( node_tracker );
		joy_a->set_input(1);
		joy_a->set_focus();
	}
	if( b_start_with_joystick_b )
	{
		joy_b = c_joy::create_obj( node_tracker );
		joy_b->set_input(2);
		joy_b->set_focus();
	}
#endif
#if AAA_TRACKER_ISENSE()
	if( isense_port_nb )
	{
		isense_load_lib();
		isense = c_isense::create_obj( node_tracker );
		isense->set_port(isense_port_nb-1);
		isense->set_focus();
	}
#endif
#if AAA_TRACKER_PCBIRD()
	if( b_start_with_pcbird )
	{
		bird_load_lib();
		pcbird = c_pcbird::create_obj( node_tracker );
		pcbird->set_focus();
	}
#endif
#if AAA_TRACKER_POLHEMUS()
	if( b_start_with_polhemus )
	{
		polhemus = c_polhemus::create_obj( node_tracker );
		polhemus->set_focus();
	}
#endif
#if AAA_TRACKER_NVISION()
	if( nvision_port_nb )
	{
		g_nvision_cur = c_nvision::create_obj( node_tracker );
		g_nvision_cur->set_port(nvision_port_nb);
		g_nvision_cur->set_focus();
	}
#endif
#if AAA_TRACKER_ANALOG_WAY()
	if( analog_way_port_nb )
	{
		analog_way = c_analog_way::create_obj( node_tracker );
		analog_way->set_port(analog_way_port_nb);
		analog_way->set_focus();
	}
#endif
#if AAA_TRACKER_DOREMI()
	if( doremi_port_nb )
	{
		doremi = c_doremi::create_obj( node_tracker );
		doremi->set_port(doremi_port_nb);
		doremi->set_focus();
	}
#endif
#if AAA_TRACKER_GLOVE_5DT()
	fglove_load_lib();
	if( glove_a_port_nb )
	{
		glove_a = c_glove::create_obj( node_tracker );
		glove_a->set_port(glove_a_port_nb);
		glove_a->set_focus();
	}
	if( glove_b_port_nb )
	{
		glove_b = c_glove::create_obj( node_tracker );
		glove_b->set_port(glove_b_port_nb);
		glove_b->set_focus();
	}
#endif
#if AAA_TRACKER_TOASTER()
	if( b_start_with_toaster )
	{
		g_toaster_cur = c_toaster::create_obj( node_tracker );
		g_toaster_cur->set_focus();
	}
#endif
#if AAA_TRACKER_FTDI()
	ftdi_open();
	INT32 i = 0;
	for( ; i < ftdi_nb; ++i )
	{
		c_ftdi::array[ i ] = c_ftdi::create_obj( node_tracker );
		c_ftdi::array[ i ]->set_focus();
	}
	for( ; i < c_ftdi::OBJ_MAX_NB; ++i )
		c_ftdi::array[ i ] = nullptr;
#endif
#if AAA_TRACKER_HID()
	c_hid::c_init();
	{
		INT32 i = 0;
		for( ; i < hid_nb; ++i )
		{
			c_hid::array[ i ] = c_hid::create_obj( node_tracker );
			c_hid::array[ i ]->set_focus();
		}
		for( ; i < c_hid::OBJ_MAX_NB; ++i )
			c_hid::array[ i ] = nullptr;
	}
#endif
#if AAA_TRACKER_NEAT()
	neat_init();
#endif
#if AAA_LIB_USE_TOUCHCO()
	c_bdd_touchco::enumerate();
#endif
#if AAA_TRACKER_HYDRA()
	if( b_start_with_hydra )
	{
		c_hydra::lib_init();
		g_hydra = c_hydra::create_obj( node_tracker );
		g_hydra->set_focus();
	}
#endif
#if AAA_TRACKER_WACOM()
	if( c_wacom::b_start_with )
	{
		c_wacom::c_init();
		g_wacom = c_wacom::create_obj( node_tracker );
		g_wacom->set_focus();
	}
#endif
#if AAA_TRACKER_MEDIAPIPE()
	if( c_mediapipe::b_start_with )
	{
		c_mediapipe::c_init();
		g_mediapipe = c_mediapipe::create_obj( node_tracker );
		g_mediapipe->set_focus();
	}
#endif
	c_capture::c_init();
}

//todo deinit all inited
void	trackers::deinit()
{
	CHAR	header[] = "# tracker_deinit()";

	SAFE_DELETE( c_aaavr::cur );
	c_aaavr::c_deinit();
#if AAA_TRACKER_IGLASSES()
	iglasses_close();
#endif //AAA_TRACKER_IGLASSES

	gypsy_close();
	for( INT32 i = 0; i < MIDI_OBJ_MAX_NB; ++i )
		SAFE_DELETE( midi_array[i] );
	midi_deinit();
	trackers::PRINT_STRING( header, "MIDI done" );
//hack
	
#if AAA_TRACKER_JOY()
	SAFE_DELETE( joy_a );
	trackers::PRINT_STRING( header, "JOY_A done" );
	SAFE_DELETE( joy_b );
	trackers::PRINT_STRING( header, "JOY_B done" );
#endif
#if AAA_TRACKER_GLOVE_5DT()
	SAFE_DELETE( glove_a );
	trackers::PRINT_STRING( header, "GLOVE_A done" );
	SAFE_DELETE( glove_b );
	trackers::PRINT_STRING( header, "GLOVE_B done" );
	fglove_unload_lib();
#endif
#if AAA_TRACKER_TOASTER()
	SAFE_DELETE( g_toaster_cur );
	trackers::PRINT_STRING( header, "TOASTER done" );
#endif
#if AAA_TRACKER_SND_INPUT()
	trackers::PRINT_STRING( header, "SOUND inputs done" );
	SAFE_DELETE( snd::g_master );
	trackers::PRINT_STRING( header, "sound free done" );
#endif
#if AAA_TRACKER_FTDI()
	for( INT32 i = 0; i < c_ftdi::OBJ_MAX_NB; ++i )
		SAFE_DELETE( c_ftdi::array[i] );
	ftdi_close();
	trackers::PRINT_STRING( header, "FTDI done" );
#endif
#if AAA_TRACKER_HID()
	for( INT32 i = 0; i <  c_hid::OBJ_MAX_NB; ++i )
		SAFE_DELETE(  c_hid::array[i] );
	c_hid::c_deinit();
	trackers::PRINT_STRING( header, "HID done" );
#endif
#if AAA_TRACKER_PCBIRD()
	bird_unload_lib();
#endif
#if AAA_TRACKER_ISENSE()
	isense_unload_lib();
#endif
#if AAA_TRACKER_HYDRA()
	if( b_start_with_hydra )
	{
		c_hydra::lib_deinit();
		SAFE_DELETE( g_hydra );
		trackers::PRINT_STRING( header, "Hydra Razor done" );
	}
#endif
#if AAA_TRACKER_WACOM()
	c_wacom::c_deinit();
#endif
	c_capture::c_deinit();

	trackers::PRINT_STRING( header, "done" );
}

void	trackers::update_on_idle()
{
#if AAA_TRACKER_NEAT()
	neat_update();
#endif
}

void	trackers::update_before_render()
{
#if AAA_TRACKER_SND_INPUT()
	snd::g_master->update();	
#endif
	c_midi::update_all();
		
#if AAA_TRACKER_JOY()
	if_obj_update( joy_a );
	if_obj_update( joy_b );
#endif
#if AAA_TRACKER_ISENSE()
	if_obj_update( isense );
#endif
#if AAA_TRACKER_PCBIRD()
	if_obj_update( pcbird );
#endif
#if AAA_TRACKER_POLHEMUS()
	if_obj_update( polhemus );
#endif
#if AAA_TRACKER_DMX()
	if_obj_update( g_dmx_cur );
#endif
#if AAA_TRACKER_NVISION()
	if_obj_update( g_nvision_cur );
#endif
#if AAA_TRACKER_ANALOG_WAY()
	if_obj_update( analog_way );
#endif
#if AAA_TRACKER_DOREMI()
	if_obj_update( doremi );
#endif
#if AAA_TRACKER_SPACEBALL()
	if( spaceball && (!net || net->is_master()) )
		spaceball->update();
#endif
#if AAA_TRACKER_MAGELLAN()
	if( magellan && (!net || net->is_master()) )
		magellan->update();
#endif
#if AAA_TRACKER_GLOVE_5DT()
	if_obj_update( glove_a );
	if_obj_update( glove_b );
#endif
#if AAA_TRACKER_TOASTER()
	if_obj_update( g_toaster_cur );
#endif
#if AAA_TRACKER_FTDI()
	for( INT32 i = 0; i < ftdi_nb; ++i )
		if_obj_update( c_ftdi::array[ i ] );
#endif
#if AAA_TRACKER_HID()
	for( INT32 i = 0; i < hid_nb; ++i )
		if_obj_update(  c_hid::array[ i ] );
#endif
#if AAA_TRACKER_HYDRA()
	if_obj_update( g_hydra );
#endif
#if AAA_TRACKER_WACOM()
	if_obj_update( g_wacom );
#endif
#if AAA_TRACKER_MEDIAPIPE()
	if_obj_update( g_mediapipe );
#endif
	if_obj_update( c_aaavr::cur );
}

void	trackers::save( o_str CONST & filename_in )
{
	o_str& filename			= o_str::push_name( filename_in );	//for the tracker used twice
	o_str& filename_letter	= o_str::push_name( filename_in );
	filename_letter.add( "_b" );

	if_obj_save_add_ext( c_aaavr::cur,			filename );

#if AAA_TRACKER_SND_INPUT()
	if_obj_save_add_ext( snd::g_master,		filename );
#endif

#if AAA_TRACKER_SPACEBALL()
	if_obj_save_add_ext( spaceball,			filename );
#endif

#if AAA_TRACKER_MAGELLAN()
	if_obj_save_add_ext( magellan,			filename );
#endif
	if( midi_array[0] )
	{
		for( INT32 i = 0; i < MIDI_OBJ_MAX_NB; ++i )
		{
			if( !midi_array[i] )
				break;
			filename_letter.set_char( -1, 'a' + i );
			midi_array[i]->save_to_file_add_ext( filename_letter );
		}
	}
	//filename_letter.set_char( -1, 'a' );
	c_midi::save_global( nullptr );	//todo deceide if this strategy is definitive : it was filename_letter.get() and got changed for transCam;

#if AAA_TRACKER_JOY()
	if_obj_save_add_ext( joy_a,			filename );
	filename_letter.set_char( -1, 'b' );
	if_obj_save_add_ext( joy_b,			filename_letter );
#endif
#if AAA_TRACKER_ISENSE()
	if_obj_save_add_ext( isense,		filename );
#endif
#if AAA_TRACKER_PCBIRD()
	if_obj_save_add_ext( pcbird,		filename );
#endif
#if AAA_TRACKER_POLHEMUS()
	if_obj_save_add_ext( polhemus,		filename );
#endif
#if AAA_TRACKER_DMX()
	if_obj_save_add_ext( g_dmx_cur,		filename );
#endif
#if AAA_TRACKER_NVISION()
	if_obj_save_add_ext( g_nvision_cur,	filename );
#endif
#if AAA_TRACKER_ANALOG_WAY()
	if_obj_save_add_ext( analog_way,	filename );
#endif
#if AAA_TRACKER_DOREMI()
	if_obj_save_add_ext( doremi,		filename );
#endif
#if AAA_TRACKER_TOASTER()
	if_obj_save_add_ext( g_toaster_cur,	filename );
#endif
#if AAA_TRACKER_FTDI()
	for( INT32 i = 0; i < ftdi_nb; ++i )
	{
		filename_letter.set_char( -1, 'a' + i );
		if_obj_save_add_ext( c_ftdi::array[i], filename_letter );
	}
	filename_letter.set_char( -1, 'b' );
#endif
#if AAA_TRACKER_HID()
	for( INT32 i = 0; i < hid_nb; ++i )
	{
		filename_letter.set_char( -1, 'a' + i );
		if_obj_save_add_ext(  c_hid::array[i], filename_letter );
	}
	filename_letter.set_char( -1, 'b' );
#endif
#if AAA_TRACKER_HYDRA()
	if_obj_save_add_ext( g_hydra,		filename );
#endif
#if AAA_TRACKER_WACOM()
	if_obj_save_add_ext( g_wacom,		filename );
#endif
#if AAA_TRACKER_MEDIAPIPE()
	if_obj_save_add_ext( g_mediapipe,	filename );
#endif
#if AAA_TRACKER_GLOVE_5DT()
	if_obj_save_add_ext( glove_a,		filename );
	if_obj_save_add_ext( glove_b,		filename_letter );
#endif
	o_str::pop_name();
	o_str::pop_name();
}

void	trackers::load( o_str CONST & filename_in )
{
	o_str& filename			= o_str::push_name( filename_in );	//for the tracker used twice
	o_str& filename_letter	= o_str::push_name( filename_in );
	filename_letter.add( "_b" );

	DBG_HEAP_CHECK();

	if_obj_load_add_ext( c_aaavr::cur,	filename );

#if AAA_TRACKER_SND_INPUT()
	if_obj_load_add_ext( snd::g_master,	filename );
#endif

#if AAA_TRACKER_SPACEBALL()
	if_obj_load_add_ext( spaceball,		filename );
#endif
#if AAA_TRACKER_MAGELLAN()
	if_obj_load_add_ext( magellan,		filename );
#endif

	c_midi::load_patchers(				filename.get() );

	if( midi_array[0] )
	{
		for( INT32 i = 0; i < MIDI_OBJ_MAX_NB; ++i )
		{
			if( !midi_array[i] )
				break;
			filename_letter.set_char( -1, 'a' + i );
			midi_array[i]->load_from_file_add_ext( filename_letter );		
		}
	}
	filename_letter.set_char( -1, 'a' );
	c_midi::load_global( filename_letter.get() );
	filename_letter.set_char( -1, 'b' );

#if AAA_TRACKER_JOY()
	if_obj_load_add_ext( joy_a,			filename );
	if_obj_load_add_ext( joy_b,			filename_letter );
#endif
#if AAA_TRACKER_ISENSE()
	if_obj_load_add_ext( isense,		filename );
#endif
#if AAA_TRACKER_PCBIRD()
	if_obj_load_add_ext( pcbird,		filename );
#endif
#if AAA_TRACKER_POLHEMUS()
	if_obj_load_add_ext( polhemus,		filename );
#endif
#if AAA_TRACKER_DMX()
	if_obj_load_add_ext( g_dmx_cur,		filename );
#endif
#if AAA_TRACKER_NVISION()
	if_obj_load_add_ext( g_nvision_cur,	filename );
#endif
#if AAA_TRACKER_ANALOG_WAY()
	if_obj_load_add_ext( analog_way,	filename );
#endif
#if AAA_TRACKER_DOREMI()
	if_obj_load_add_ext( doremi,		filename );
#endif
#if AAA_TRACKER_TOASTER()
	if_obj_load_add_ext( g_toaster_cur,	filename );
#endif
#if AAA_TRACKER_FTDI()
	for( INT32 i = 0; i < ftdi_nb; ++i )
	{
		filename_letter.set_char( -1, 'a' + i );
		c_ftdi::array[ i ]->load_from_file_add_ext( filename_letter );
	}
	filename_letter.set_char( -1, 'b' );

#endif
#if AAA_TRACKER_HID()
	for( INT32 i = 0; i < hid_nb; ++i )
	{
		filename_letter.set_char( -1, 'a' + i );
		c_hid::array[ i ]->load_from_file_add_ext( filename_letter );
	}
	filename_letter.set_char( -1, 'b' );
#endif
#if AAA_TRACKER_HYDRA()
	if_obj_load_add_ext( g_hydra,			filename );
#endif
#if AAA_TRACKER_WACOM()
	if_obj_load_add_ext( g_wacom,			filename );
#endif
#if AAA_TRACKER_MEDIAPIPE()
	if_obj_load_add_ext( g_mediapipe,		filename );
#endif
#if AAA_TRACKER_GLOVE_5DT()
	if_obj_load_add_ext( glove_a,			filename );
	if_obj_load_add_ext( glove_b,			filename_letter );
#endif
	o_str::pop_name();
	o_str::pop_name();
}

void	trackers::draw()
{
#if AAA_TRACKER_HMD()
	euler_cur.draw();
#endif
#if AAA_TRACKER_NEAT()
	neat_draw();
#endif
}

#endif
