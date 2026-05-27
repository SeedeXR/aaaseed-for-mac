
#ifdef AAA_SOUND_BASS_ASIO_H
#error "SOUND_BASS_ASIO_H included more than once."
#endif
#define AAA_SOUND_BASS_ASIO_H 1


#ifndef AAA_SOUND_NEW_H
#	include "sound_new.h"
#endif
#ifndef AAA_WRAP_BASSASIO_H
#	include "lib_wrappers/wrap_bass_asio.h"
#endif

class	c_sound_bass_asio final : public c_sound
{
	FACTORY_DECLARE( c_sound_bass_asio, c_sound )
	friend class c_sound_input_bass_asio;
private :
	// Bass Asio
	INT32		_asio_buf_len;

	INT32		_latency_in;
	INT32		_latency_out;
	REAL		_rate;

//	INT32			channel_nb_ui_;

//			CHAR*	get_str_error( UINT32 err );
protected:
//			void	error_report( INT32 h, CHAR* str_fn );
//	static			CHAR*	get_error_str()
public:
	static	C_PCHAR_C	get_error_str();
	static	INT32		get_error();
	static	void		report_error( INT32 h, C_PCHAR_C str_fn );
	static	void		report_error( C_PCHAR_C str_fn );

	static	C_PCHAR_C	get_format_str( DWORD format );
	static	void		print_channel_info( bool b_input, INT32 channel );
	static	void		print_info( BASS_ASIO_INFO* info );
	static	void		print_rate_latency( bool b_input, REAL rate, INT32 latency );

	static	double		get_rate();
	static	INT32		get_latency( bool b_input );

	static	bool		open_lib();
	static	void		close_lib();

	virtual	void		param_init_pt();

//	virtual	AAA_ERR		save_do_after( o_str CONST & filename );
//	virtual	AAA_ERR		load_do_after( o_str CONST & filename );


	virtual	void		close();
//	virtual LPGUID		get_guid( UINT32 in );

			bool		open_driver();
//			bool		open_device();

//	virtual	void		play_from_file( C_PCHAR filename );

//	virtual	void		play_start();
//	virtual	void		play_stop();
//	virtual	void		play_pause();
//	virtual	void		set_loop( bool b_looping );
//	virtual	void		set_volume( REAL volume );
//	virtual	void		set_pan( REAL pan );

//	virtual	void		update();
//	virtual	void		update_input();
//	virtual	void		update_output();
	virtual	c_sound_input*	create_input_new();

	virtual INT32			enumerate_device_out();

	virtual bool			open_control_panel();
};





