// * * * * * * * * * * * * * * * * * * * *
// *  Sound Input Abstract Class         *
// *                                     *
// * Date Created :  17/02/2006          *
// *                                     *
// * Date Modified : 22/02/2006          *
// *                                     *
// * Author :        Franz Hildgen       *
// *                                     *
// * * * * * * * * * * * * * * * * * * * *

#ifdef AAA_SOUND_CAPTURE_H
#error "SOUND_CAPTURE_H included more than once."
#endif
#define AAA_SOUND_CAPTURE_H 1

//#ifndef __OBJ_UI_H__
//#	include "obj_ui.h"
//#endif
#ifndef AAA_ERR_H
#	include "err.h"
#endif
//#ifndef AAA_SOUND_NEW_H
//#	include "sound_new.h"
//#endif


class c_sound_capture
{
protected :
	static	const	INT32 SOUND_BIT_PER_SAMPLE = 16;
	static	const	INT32 SOUND_SAMPLE_PER_SEC = 44100;
	static	const	INT32 SOUND_CAPTURE_SIZE_IN_BIT	= 10;

//	const	INT32 SOUND_BYTE_PER_SAMPLE = ( SOUND_BIT_PER_SAMPLE >> 3 );

//	const	INT32 SOUND_BYTE_PER_SECOND_MONO = ( SOUND_SAMPLE_PER_SEC * SOUND_BYTE_PER_SAMPLE );

//	const	INT32 SOUND_SAMPLE_PER_CAPTURE = ( 1 << SOUND_CAPTURE_SIZE_IN_BIT );
//	const	INT32 SOUND_BYTE_PER_CAPTURE = ( SOUND_SAMPLE_PER_CAPTURE * SOUND_BYTE_PER_SAMPLE );

	INT32		bit_per_sample_;
	INT32		byte_per_sample_;
	INT32		sample_rate_;
	INT32		byte_per_sec_mono_;
	INT32		capture_size_bit_;
	INT32		sample_per_capture_;
	UINT32		byte_per_capture_;

	BOOL		b_enable_;

	INT32		nb_input_;

	INT32		device_;			// Device we want to use for input

	INT32		frequency_;
	INT32		channel_first_;
	INT32		channel_nb_;

	INT32		capture_count_;
	UINT8*		sound_buffer_;	// the sound
	INT32		buffer_size_;

	virtual	void	sample_size();

public:
//	virtual	const char*	const	get_class_name()	{ return "sound_capture"; };
//	virtual	const char*	const	get_fname_ext()		{ return "sound_capture"; };	//now

	c_sound_capture();
	virtual	~c_sound_capture();

	virtual void	init();
	virtual	void	deinit();

	virtual	void	update();

	virtual	INT32	enable( INT32 id, INT32 device, INT32 channel_first, INT32 channel_nb, INT32 sample_rate, INT32 sampe_bits, INT32 capture_size_bits_ui_ );
	virtual	BOOL	disable();// INT32 input_id );
	virtual UINT8*	get_sound_buffer();// INT32 input_id );
	virtual	INT32	get_buffer_size()			{ return buffer_size_; };
	virtual	INT32	get_sample_per_capture()	{ return sample_per_capture_; };
	virtual	INT32	get_capture_count()			{ return capture_count_; };

			BOOL	IsEnable()					{ return b_enable_; };

};



