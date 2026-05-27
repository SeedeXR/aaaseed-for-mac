
#ifdef AAA_SOUND_INPUT_H
#error "SOUND_INPUT_H included more than once."
#endif
#define AAA_SOUND_INPUT_H 1


#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef	AAA_BUFFER_BLK_H
#	include "buffer/buffer_blk.h"
#endif

class	c_sound_input : public c_obj
{
private:
	bool			_b_enable;
//	INT32			_capture_count;

	INT32			_check_valid;			//	check fot crashed and validity

	INT32			_bit_per_sample_mono;
	INT32			_byte_per_sample_mono;
	INT32			_sample_per_sec_mono;
	INT32			_byte_per_sec_mono;
	INT32			_sample_per_capture;
	UINT32			_byte_per_capture;

	INT32			_id;

	INT32			_device;			// Device we use

//	INT32			_frequency;
	INT32			_channel_first;
	c_buffer_stream	_capture_stream;	// the sound

	INT32			_buffer_size;
	o_str			_device_name;

protected:
//	INT32			_channel_nb_max;	//todosnd finish only in bass asio now
	INT32			_nb_input	{0};
	INT32			_nb_output	{0};

	virtual	bool	enable_low()	= 0;
	virtual	bool	disable_low()	= 0;

	virtual	void	compute_sample_size();
			bool	alloc_sound_buffer( INT32 size );
			void	dealloc_sound_buffer();

			o_str			_o_type;	//todo	should be by input

public:

	FINLINE	bool				is_valid()
								{	//todo quick hack to avoid crash
									return _check_valid == 42;
									//return _id > 0 && _bit_per_sample_mono > 0 && _sample_per_capture > 0;
								}

	c_sound_input();
	virtual	~c_sound_input();

	virtual	void				init();
	virtual	void				deinit();

	virtual	void				update() = 0;

	virtual	bool				enable( INT32 id, INT32 device, INT32 channel_first, INT32 channel_nb, INT32 sample_rate, INT32 sample_bits, INT32 sample_per_capture );
	virtual	bool				disable();

	FINLINE c_buffer_stream&	get_capture_stream()			{ return _capture_stream;		}
	FINLINE	INT32				get_buffer_size()				{ return _buffer_size;			}
	FINLINE	void				set_buffer_size( INT32 size )	{ _buffer_size = size;			}

//	virtual	INT32				get_capture_count()				{ return _capture_count; }
//			INT32*				get_capture_count_pt()			{ return &_capture_count; }

	FINLINE INT32				get_device()					{ return _device;				}
	FINLINE o_str*				get_device_name()				{ return &_device_name;			}
	FINLINE o_str*				get_type_name()					{ return &_o_type;				}
//	FINLINE INT32				get_channel_nb()				{ return _channel_nb;			}

	FINLINE	void				set_input_nb( INT32 nb )		{ _nb_input = nb;				}							
	FINLINE	INT32				get_input_nb() const			{ return _nb_input;				}							

	FINLINE	void				set_output_nb( INT32 nb )		{ _nb_output = nb;				}							
	FINLINE	INT32				get_output_nb() const			{ return _nb_output;			}							

	FINLINE INT32				get_id()						{ return _id;					}
	FINLINE bool				is_enable()						{ return _b_enable;				}

	FINLINE	UINT32				get_bit_per_sample_mono()		{ return _bit_per_sample_mono;	}
	FINLINE	UINT32				get_byte_per_capture()			{ return _byte_per_capture;		}
	FINLINE	INT32				get_sample_per_sec_mono()		{ return _sample_per_sec_mono;	}
	FINLINE	INT32				get_byte_per_sec()				{ return _byte_per_sec_mono		* _nb_input;	}
	FINLINE	INT32				get_byte_per_sample()			{ return _byte_per_sample_mono	* _nb_input;	}
	FINLINE	INT32				get_sample_per_capture()		{ return _sample_per_capture;	}
	
};



