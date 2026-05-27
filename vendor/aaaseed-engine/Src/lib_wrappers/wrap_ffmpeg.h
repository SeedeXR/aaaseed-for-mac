

#ifdef AAA_WRAP_FFMPEG_H
#error "WRAP_FFMPEG_H included more than once."
#endif
#define AAA_WRAP_FFMPEG_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

#if AAA_WIN64()
#		define	AAA_USE_FFMPEG()	0	// 2025 Sep with old FFmpeg: compile but don't link, with recent FFmpeg don't compile
#else
#		define	AAA_USE_FFMPEG()	1
#endif


#if AAA_USE_FFMPEG()

#	ifndef AAA_AAA_TYPE_H
#		include "aaa_type.h"
#	endif

	// ---------------------------------------------------------------------------
	// "FFMPEG" DLL Wrapper Init/Term - loads the ffmpeg.dll, wraps the calls.
	// Functions are to be implemented in both forms (linked and wrapped)
	// In form of linked DLL - they do nothing, "print" and return NO_ERROR
	//
	UINT32	wrap_ffmpeg_Init ( void );	// returns: winerror code
	UINT32	wrap_ffmpeg_Term ( void );	// returns: winerror code

#endif