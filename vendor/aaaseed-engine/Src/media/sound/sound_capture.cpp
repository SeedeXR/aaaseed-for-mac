#ifndef AAA_SOUND_CAPTURE_H
#	include "sound_capture.h"
#endif


c_sound_capture::c_sound_capture()
{
}

c_sound_capture::~c_sound_capture()
{
	deinit();
}

void	c_sound_capture::init()
{
}

void	c_sound_capture::deinit()
{
}

void	c_sound_capture::update()
{
}

void	c_sound_capture::sample_size()
{
	//	const	INT32 SOUND_BIT_PER_SAMPLE = 16;
	//	const	INT32 SOUND_BYTE_PER_SAMPLE = ( SOUND_BIT_PER_SAMPLE >> 3 );

	//	const	INT32 SOUND_SAMPLE_PER_SEC = 44100;
	//	const	INT32 SOUND_BYTE_PER_SECOND_MONO = ( SOUND_SAMPLE_PER_SEC * SOUND_BYTE_PER_SAMPLE );

	//	const	INT32 SOUND_CAPTURE_SIZE_IN_BIT	= 10;
	//	const	INT32 SOUND_SAMPLE_PER_CAPTURE = ( 1 << SOUND_CAPTURE_SIZE_IN_BIT );
	//	const	INT32 SOUND_BYTE_PER_CAPTURE = ( SOUND_SAMPLE_PER_CAPTURE * SOUND_BYTE_PER_SAMPLE );

	byte_per_sample_ = bit_per_sample_ >> 3;
	byte_per_sec_mono_ = sample_rate_ * byte_per_sample_;
	sample_per_capture_ = ( 1 << capture_size_bit_ );
	byte_per_capture_ = sample_per_capture_ * byte_per_sample_;

}

INT32	c_sound_capture::enable( INT32 id, INT32 device, INT32 channel, BOOL b_stereo, INT32 sample_rate, INT32 sampe_bits, INT32 capture_size_bits_ui_ )
{
	return 0;
}

BOOL	c_sound_capture::disable()
{
	return TRUE;
}

UINT8*	c_sound_capture::get_sound_buffer()
{
	return NULL;
}
