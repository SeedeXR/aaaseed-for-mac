
#ifdef AAA_TRACKERS_H
#error "TRACKERS_H included more than once."
#endif
#define AAA_TRACKERS_H 1


#ifndef	AAA_AAA_DEF_H
#	include "aaa_def.h"
#endif

// AAA_TRACKER() is defined in Src/aaa_build_config.h, force-included in every TU.
// Watchdog override kept as a guard in case AAA_WATCHDOG() is ever re-enabled.
#if	AAA_WATCHDOG()
#	undef  AAA_TRACKER
#	define AAA_TRACKER()	0
#endif
#ifndef AAA_TRACKER
#	error "AAA_TRACKER() must be defined in aaa_build_config.h"
#endif

#if	AAA_TRACKER()

#	ifndef AAA_AAA_STR_H
#		include "aaa_str.h"
#	endif


#	define	AAA_TRACKER_SPACEBALL()		1
#	define	AAA_TRACKER_MAGELLAN()		0
#	define	AAA_TRACKER_MIDI()			1
#	define	AAA_TRACKER_JOY()			1
#	define	AAA_TRACKER_SND_INPUT()		1
#	define	AAA_TRACKER_NEAT()			0	// ADC convertor using RS232 to communicate

//this is not really clean when define
//	bit we don't really use it now (maa 2016)
#	define	AAA_TRACKER_HMD()			0
#	define	AAA_TRACKER_IGLASSES()		AAA_TRACKER_HMD()
#	define	AAA_TRACKER_VIO()			AAA_TRACKER_HMD()

#if AAA_WIN64()
#	define	AAA_TRACKER_PCBIRD()		0	// Bird by ascension	//todox64
#	define	AAA_TRACKER_POLHEMUS()		0	//todox64
#	define	AAA_TRACKER_HYDRA()			0	//todox64
#	define	AAA_TRACKER_GLOVE_5DT()		0	//todox64
#	define	AAA_TRACKER_ISENSE()		0	//todox64
#else
#	define	AAA_TRACKER_PCBIRD()		1	// Bird by ascension
#	define	AAA_TRACKER_POLHEMUS()		1
#	define	AAA_TRACKER_HYDRA()			1
#	define	AAA_TRACKER_GLOVE_5DT()		1
#	define	AAA_TRACKER_ISENSE()		1
#endif

#	define	AAA_TRACKER_NVISION()		0
#	define	AAA_TRACKER_ANALOG_WAY()	0

#	define	AAA_TRACKER_TOASTER()		0	// interface done by the Kitchen
#	define	AAA_TRACKER_DMX()			1
#	define	AAA_TRACKER_DOREMI()		0
#	define	AAA_TRACKER_FTDI()			1
#	define	AAA_TRACKER_HID()			0
#	define	AAA_TRACKER_WACOM()			1
#	define	AAA_TRACKER_MEDIAPIPE()		1


class c_node_ui;

class trackers
{
public:
//	static	INT32	spaceball_port_nb;
	static	bool	b_spaceball_start;

	static	INT32	midi_nb;
	static	INT32	isense_port_nb;
	static	INT32	nvision_port_nb;
	static	INT32	analog_way_port_nb;
	static	INT32	doremi_port_nb;
	static	INT32	glove_a_port_nb;
	static	INT32	glove_b_port_nb;
	static	INT32	ftdi_nb;
	static	INT32	hid_nb;

	static	bool	b_start_with_joystick_a;
	static	bool	b_start_with_joystick_b;
//	static	bool	b_start_with_sound;
	static	bool	b_start_with_magellan;
	static	bool	b_start_with_pcbird;
	static	bool	b_start_with_polhemus;
	static	bool	b_start_with_toaster;
	static	bool	b_start_with_dmx;
	static	bool	b_start_with_hydra;

	static	void	init();
	static	void	deinit();
	static	void	update_on_idle();
	static	void	update_before_render();
	static	void	save( o_str CONST & filename );
	static	void	load( o_str CONST & filename );
	static	void	draw();

	//AAA_TRACKER	there is a problem that the user should know
	static	void	PRINT_STRING(			C_PCHAR_C header,	C_PCHAR_C fmt,	...				);
	static	void	PRINT_STRING_VA(		C_PCHAR_C h,		C_PCHAR_C fmt,	va_list args	);

	static	c_node_ui*	node_tracker;
};

#	if AAA_TRACKER_JOY()
	class c_joy;
	extern	c_joy*		joy_a;
	extern	c_joy*		joy_b;
#	endif

#endif	//AAA_TRACKER

