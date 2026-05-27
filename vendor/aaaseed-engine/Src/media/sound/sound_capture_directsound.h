
#ifdef AAA_SNDINPUT__DS_H
#error "SNDINPUT__DS_H included more than once."
#endif
#define AAA_SNDINPUT__DS_H 1


#ifndef AAA_DELTA_T_H
#	include "delta_t.h"
#endif
#ifndef AAA_SOUND_CAPTURE_H
#	include "sound_capture.h"
#endif
#ifdef	WIN32
#	ifndef __DSOUND_INCLUDED__
#		include <dsound.h>
#	endif
#endif


class c_sound_capture_ds final : public c_sound_capture
{
private :
	static	const	INT32		SOUND_BUFFER_NB = 4;	//for directsound
	DSBPOSITIONNOTIFY			rgdsbpn[SOUND_BUFFER_NB + 1];
	// DirectX stuff
	LPDIRECTSOUNDCAPTUREBUFFER	lpDSBuffer_;
	HANDLE						h_NotifyEvent_[SOUND_BUFFER_NB + 1];
	HANDLE						h_notify_thread_;
	LPDIRECTSOUNDCAPTURE		p_DSCapture_;
	LPDIRECTSOUNDNOTIFY			lpDsNotify_;  // pointer to the interface
	WAVEFORMATEX				wfx_;
	DWORD						dwThreadId_;

	BOOL						b_buffer_read_;
	BOOL						b_sound_capture_thread_on_;

	INT32						id_;

			//void	update_low()		{};
			void	release_notification_events();
			BOOL	enable_low();
			void	disable_low();
//	virtual	void	sample_size();

public:

	friend	DWORD	CALLBACK	HandleNotifications( LPVOID lpvoid );

	c_sound_capture_ds();
	virtual	~c_sound_capture_ds();

	virtual void	init();
	virtual	void	deinit();

	virtual	void	update();

	virtual	INT32	enable( INT32 id, INT32 device, INT32 channel_first, INT32 channel_nb, INT32 sample_rate, INT32 sampe_bits, INT32 capture_size_bits_ui_ );
	virtual	BOOL	disable( );
	virtual UINT8*	get_sound_buffer( );
//	virtual	INT32	get_buffer_size()
			INT32	get_id()				{ return id_; };
	virtual	INT32	get_capture_count()		{ return capture_count_; };

			BOOL	IsEnable()				{ return b_enable_; };

};




