
#ifdef AAA_DATA_SRC_H
#error "DATA_SRC_H included more than once."
#endif
#define AAA_DATA_SRC_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

enum DATA_SRC_TYPE : INT32
{
	SOUND_SPECTRUM_RAW,
	SOUND_SPECTRUM_LINEAR,
	SOUND_SPECTRUM,
	SOUND_WAVE,
	MIDI_CONTROL,
	MIDI_VELOCITY,
	DATACUBE,
	DATA_SRC_TYPE_NB
};

extern	C_PCHAR_C	gstr_data_src[DATA_SRC_TYPE_NB];

