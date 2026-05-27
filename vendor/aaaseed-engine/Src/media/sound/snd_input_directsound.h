
#ifdef AAA_SNDINPUT_DS_H
#error "SNDINPUT_DS_H included more than once."
#endif
#define AAA_SNDINPUT_DS_H 1


#ifndef AAA_SOUND_INPUT_H
#	include "sound_input.h"
#endif
#ifdef	WIN32
#	ifndef AAA_AAA_OS_H
#			include "aaa_os.h"
#	endif
#	ifndef _MMEAPI_H_
#		include "mmeapi.h"	//need by dsound.h in fact
#	endif
#	ifndef __DSOUND_INCLUDED__
#		include <dsound.h>
#	endif
#endif

class c_sound_input_ds final : public c_sound_input
{
private :
	static	CONST	INT32		SOUND_BUFFER_NB = 4;	//for directsound
	DSBPOSITIONNOTIFY			rgdsbpn[SOUND_BUFFER_NB + 1];
	// DirectX stuff
	LPDIRECTSOUNDCAPTUREBUFFER	_lpDSBuffer;
	HANDLE						_h_NotifyEvent[SOUND_BUFFER_NB + 1];
	HANDLE						_h_notify_thread;
	LPDIRECTSOUNDCAPTURE		_p_DSCapture;
	LPDIRECTSOUNDNOTIFY			_lpDsNotify;	// pointer to the interface
	WAVEFORMATEX				_wfx;
	DWORD						_dwThreadId;


	bool						_b_buffer_read;
	bool						_b_notification_handling;
	bool						_b_notification_ask_stop;

			void	release_notification_events();
protected:
	virtual	bool	enable_low();
	virtual	bool	disable_low();

public:
	friend	DWORD	CALLBACK	HandleNotifications( LPVOID lpvoid );

	c_sound_input_ds();
	virtual	~c_sound_input_ds();

	virtual	void	init();
	virtual	void	deinit();

	virtual	void	update();
};



