#ifndef AAA_SOUND_INPUT_H
#	include "sound_input.h"
#endif
#ifndef AAA_SOUND_NEW_H
#	include "sound_new.h"
#endif
#include "snd_master.h"

c_sound_input::c_sound_input()
		:_device(-1)
		,_buffer_size(0)
{
}

c_sound_input::~c_sound_input()
{
	deinit();
}

void	c_sound_input::init()
{
	_b_enable = false;
	dealloc_sound_buffer();

	_id = 0;
//	_device = 0;			// Device we want to use for input

	_nb_input = 0;			// stereo input
	_nb_output = 0;

	_bit_per_sample_mono	= snd::BIT_PER_SAMPLE_MONO_DEF;
	_sample_per_sec_mono	= snd::SAMPLE_PER_SEC_DEF;

	_sample_per_capture		= snd::SAMPLE_PER_CAPTURE_DEF;
	// calculate all the size for sample per sec, byte, etc...
	compute_sample_size();

//	_capture_count = 0;
	_o_type.erase();
	_device_name.erase();
	_check_valid = 42;	//lv we protect
}

void	c_sound_input::deinit()
{
	_check_valid = 0;	//lv we protect
}

// Create a new input using parameters, return true or false
bool	c_sound_input::enable( INT32 id, INT32 device, INT32 channel, INT32 nb_input, INT32 sample_rate, INT32 sample_bits, INT32 sample_per_capture )
{
	_id						= id;
	_device					= device;
	_channel_first			= channel;
	_nb_input				= nb_input;

	_bit_per_sample_mono	= sample_bits;
	_sample_per_sec_mono	= sample_rate;
	_sample_per_capture		= sample_per_capture;
	// calculate all the size for sample per sec, byte, etc...
	compute_sample_size();

	// init the sound buffer
	if( alloc_sound_buffer( _byte_per_capture ) )
	{
		if( !_b_enable )
		{
			snd::PRINT( "will enable sound input" );
			if( enable_low() )
			{
				set_buffer_size( _byte_per_capture );
				_b_enable = true;
			}
			else
			{
				snd::ERR_PRINT( "Failed : did not succeed to enable sound input" );
				//todo commented for bass asio : is this normal
				//disable_low();
			}
		}
	}
	// could not create or enable the sound input
	return _b_enable;
}

bool	c_sound_input::disable()
{
	if( !is_valid() )
		return false;

	if( _b_enable )
	{
		bool b_ret = disable_low();
		if( b_ret )
			_b_enable = false;
		else
			snd::ERR_PRINT( "Failed : did not succeed to disable sound input" );
	}
	return true;
}

void	c_sound_input::compute_sample_size()
{
	_byte_per_sample_mono	= (_bit_per_sample_mono + 7) >> 3;
	_byte_per_sec_mono		= _sample_per_sec_mono * _byte_per_sample_mono;
	_byte_per_capture		= _sample_per_capture * _byte_per_sample_mono;
}

bool	c_sound_input::alloc_sound_buffer( INT32 size )
{
	_capture_stream.set_blk_size( size );
	return true;
}

void	c_sound_input::dealloc_sound_buffer()
{
/*
	if( _sound_buffer )
		delete [] _sound_buffer;
	 _sound_buffer = nullptr;
	 _buffer_size = 0;
*/
}

/*
#define WAVE_FORMAT_4M08       0x00000100	//	44.1   kHz, Mono,   8-bit
#define WAVE_FORMAT_4S08       0x00000200	//	44.1   kHz, Stereo, 8-bit
#define WAVE_FORMAT_4M16       0x00000400	//	44.1   kHz, Mono,   16-bit
#define WAVE_FORMAT_4S16       0x00000800	//	44.1   kHz, Stereo, 16-bit
*/
