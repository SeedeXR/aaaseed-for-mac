
#ifdef AAA_SNDINPUT__BASS_ASIO_H
#error "SNDINPUT__BASS_ASIO_H included more than once."
#endif
#define AAA_SNDINPUT__BASS_ASIO_H 1



#ifndef AAA_DELTA_T_H
#	include "delta_t.h"
#endif
#ifndef AAA_SND_INPUT_H
#	include "snd_input.h"
#endif

#ifdef WIN32
#	ifndef BASSASIO_H
#		include "bassasio.h"
#	endif
#endif


class c_snd_input_bass_asio final : public c_snd_input
{
	FACTORY_DECLARE(c_snd_input_bass_asio,c_snd_input);
private :
//
	INT32	asio_buf_lenght;

	void	update_low()		{};

public:

	friend	void CALLBACK bass_asio_proc( BOOL input, DWORD channel, void *buffer, DWORD lenght, DWORD user );

			INT32	get_asio_buf_lenght()		{ return asio_buf_lenght; };
	
	void	param_init_pt();
	void	param_init();

	AAA_ERR	save_do_after( char* const filename );
	AAA_ERR	load_do_after( char* const filename );

	virtual	void	disable();
	virtual	BOOL	enable();
	virtual	void	init();
	virtual	void	update();
	virtual	INT32	get_capture_count()		{ return capture_count_; };

};




