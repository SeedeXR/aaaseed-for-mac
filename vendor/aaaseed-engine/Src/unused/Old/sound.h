
#ifdef AAA_SOUND_H
#error "SOUND_H included more than once."
#endif
#define AAA_SOUND_H 1


#ifndef AAA_ERR_H
#	include "err.h"
#endif
#ifdef	WIN32
#	include <dsound.h>
#endif
#ifndef BASS_H
#	include "bass.h"
#endif

enum SND_LIB_TYPE
{
	SND_LIB_DS = 0,
	SND_LIB_BASS,
	SND_LIB_BASS_ASIO,
	SND_LIB_NB_MAX,
};


extern INT32	s_sound_lib_name;

extern	void	SOUND_PRINT_STRING( const CHAR* const fmt, ...);

extern	void	sound_init();
extern	void	sound_free();

#ifdef	WIN32
extern	AAA_ERR ds_load_from_file( LPDIRECTSOUNDBUFFER *buffer, char* filename,  UINT32* p_byte_per_sec = NULL );
extern	CHAR*	ds_get_str_error( UINT32 err );
extern	void	ds_err_show( UINT32 err_code, CHAR* mess );
extern	LPGUID	ds_get_guid( INT32 in );

#endif	//WIN32

extern	AAA_ERR	bass_load_from_file( HSTREAM* stream, char* filename, BOOL b_loop );
extern	CHAR*	bass_get_str_error( UINT32 err );

#endif	//  __SOUND_H__
 