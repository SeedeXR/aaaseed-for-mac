
#ifdef AAA_IMG_BASE_H
#error "IMG_BASE_H included more than once."
#endif
#define AAA_IMG_BASE_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef	AAA_PIXEL_FORMAT_H
#	include "pixel_format.h"
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif
#ifndef	AAA_ID_UNIQUE_H
#	include "id_unique.h"
#endif
#ifndef AAA_AAA_MUTEX_H
#	include "aaa/aaa_mutex.h"
#endif


class	c_img_base : public c_obj
{
public:
	static			bool		gb_verbose_image_missing_ui;
	static			bool		gb_force_reload_ui;
	static			bool		gb_free_when_on_board_ui;

	static	FINLINE	bool		is_force_reload()				{ return gb_force_reload_ui; }

	c_img_base();
	~c_img_base();

	static			INT32		compute_byte_pitch(	INT32 CONST sx, INT32 CONST bits_per_pixel, INT32 CONST alignment = 4 );
	static			INT32		compute_byte_pitch(	INT32 CONST sx,									aaa::PIXEL_FORMAT CONST pixel_format, INT32 CONST alignment = 4 );
	static			UINT64		compute_data_size(	INT32 CONST sx, INT32 CONST sy,					aaa::PIXEL_FORMAT CONST pixel_format );
	static			UINT64		compute_data_size(	INT32 CONST sx, INT32 CONST sy, INT32 CONST sz, aaa::PIXEL_FORMAT CONST pixel_format );

private:
	INT32					_sx;
	INT32					_sy;	
	FP32					_over_sx;
	FP32					_over_sy;

// DATA
	bool					_b_gpu_move;
	bool					_b_free_when_on_board;
	bool					_b_data_keep;
	bool					_b_data_owner;

	void*					_data;
	UINT64					_data_size_allocated;
	UINT64					_data_size_used;

// FILE
	o_str					_filename;
	time_t					_time_modification;

// STATE
	bool					_b_changed;
	id_unique::c_u32		_state_unique;

// LOCK
	mutable aaa::MUTEX		_lock;
	bool					_b_lock;

	bool					_b_reading;
	bool					_b_empty;

//todo extend ? unused right now
//private:
//	bool					_channel_split			{ false };
//public:
//	FINLINE	bool			is_channel_split()		{	return _channel_split;		}

protected:
	UINT32					_byte_pitch;		// pitch is the size in byte to go from a pixel at position x in one line to the pixel at the same position in the next line
	INT32					_byte_per_pixel;
	INT32					_channel_nb;
	aaa::PIXEL_FORMAT		_pixel_format;
	aaa::PIXEL_TYPE			_pixel_type;

	//	COMPRESSION
	UINT8*					_compressed_data;
	INT32					_compressed_mipmap_nb;
	INT32					_compressed_format;
	UINT64					_compressed_size;
	bool					_b_compressed_data;
	bool					_b_compress_do;
	//o_str					_filename_compressed;

			void				set_data_size_allocated(	UINT64 CONST size );
			void				set_data_size_used(			UINT64 CONST size );
			void				init_base();

			void				set_compress_do(			bool CONST b_in )		{	_b_compress_do = b_in;		}

			void				print_err_unimplemented(			C_PCHAR_C fn_name, C_PCHAR_C str_format_asked ) CONST;
			void				print_err_unsupported_channel_nb(	C_PCHAR_C fn_name ) CONST;
			void				print_err_unsupported_format(		C_PCHAR_C fn_name ) CONST;
			void				print_err_unsupported_type(			C_PCHAR_C fn_name ) CONST;

			AAA_ERR				alloc_data(					UINT64 CONST size,	aaa::PIXEL_FORMAT CONST format, C_PCHAR_C signature );

			void				alloc_data_compressed(		UINT64 CONST size,	INT32 CONST format );
			UINT8*				get_data_compressed()						CONST	{	return _compressed_data;		}
			void				dealloc_data_compressed();

			void				set_free_when_on_board(		bool CONST b_in )		{	_b_free_when_on_board = b_in;	}
	FINLINE	bool				is_free_when_on_board()								{	return ( _b_free_when_on_board || gb_free_when_on_board_ui ) && !is_cpu_keep();	}

public:
	FINLINE	aaa::PIXEL_TYPE		get_data_type()								CONST	{	return _pixel_type;				}
	FINLINE	UINT64				get_data_size_allocated()					CONST	{	return _data_size_allocated;	}
	FINLINE	UINT64				get_data_size_used()						CONST	{	return _data_size_used;			}

			void				dealloc_data();

	FINLINE	aaa::PIXEL_FORMAT	get_pixel_format()							CONST	{	return _pixel_format;			}
	FINLINE	INT32				get_channel_nb()							CONST	{	return _channel_nb;				}
	FINLINE bool				has_alpha()									CONST	{	return _channel_nb == 4;		}	//could be refined if needed

	FINLINE	GLenum				get_gl_type()								CONST	{	return aaa::c_pixel_format::get_gl_type( _pixel_format );		}
	FINLINE	GLenum				get_gl_format( bool CONST b_bgr = false )	CONST	{	return aaa::c_pixel_format::get_gl_format( _pixel_format, b_bgr );	}

	FINLINE	bool				make_compo_index( aaa::COMPO compo, INT32& a )
								{
									if( compo <= aaa::COMPO::ALPHA )
									{
										a = aaa::c_pixel_format::get_compo_index( get_pixel_format(), compo );
										return a>=0;
									}
									return false;
								}
	FINLINE	bool				make_compo_index( aaa::COMPO compo, INT32& a, INT32& b )
								{
									return aaa::c_pixel_format::make_compo_index( get_pixel_format(), compo, a, b );
								}

	FINLINE	void				set_cpu_keep( bool CONST b_in )						{	_b_data_keep = b_in;			}
	FINLINE	bool				is_cpu_keep()								CONST	{	return _b_data_keep;			}

	FINLINE	void				set_gpu_move( bool CONST b_in )						{	_b_gpu_move = b_in;				}
	FINLINE	bool				is_gpu_move()								CONST	{	return _b_gpu_move;				}


	FINLINE	INT32				get_size_x()								CONST	{	return _sx;						}
//	FINLINE	INT32				get_size_x_aligned() CONST							{	return _sx_aligned;				}
	FINLINE	INT32				get_size_y()								CONST	{	return _sy;						}
	FINLINE	void				get_size_xy( INT32& sx, INT32& sy )			CONST	{	sx = _sx;	sy = _sy;			}
	FINLINE	bool				is_valid_xy( INT32 CONST x, INT32 CONST y )	CONST	{	return 0 <= x && x < _sx && 0 <= y && y < _sy; }

	FINLINE	FP32				get_size_x_over()							CONST	{	return _over_sx;				}
	FINLINE	FP32				get_size_y_over()							CONST	{	return _over_sy;				}

	FINLINE	INT32				get_pixel_nb_to_process()					CONST	{	return _sy * _sx;				}

	FINLINE	FP32				get_ratio_x()								CONST	{	return _sx * _over_sy;			}
	FINLINE	FP32				get_ratio_y()								CONST	{	return _sy * _over_sx;			}
private:
	FINLINE	void				set_pixel_format( aaa::PIXEL_FORMAT CONST format );
public:
			void				set_size_format( INT32 CONST sx, INT32 CONST sy, aaa::PIXEL_FORMAT CONST format );

	FINLINE	INT32				get_byte_per_pixel()						CONST	{	 return _byte_per_pixel;		}
	FINLINE	INT32				get_byte_pitch()							CONST	{	 return _byte_pitch;			}

	FINLINE	UINT64				compute_data_size() CONST
								{
								//	return get_pixel_nb_to_process_aligned_x() * get_byte_per_pixel();
									return _sy * _byte_pitch;
								}
	FINLINE	UINT64				get_byte_offset( INT32 CONST x, INT32 CONST y )	CONST
								{
									return y * get_byte_pitch() + x * get_byte_per_pixel();
								}

	FINLINE	bool				is_bgr()									CONST	{	 return aaa::c_pixel_format::is_bgr( _pixel_format );							}
	FINLINE	void				flip_bgr()											{	 _pixel_format = aaa::c_pixel_format::get_format_red_blue_swapped( _pixel_format );	}

	FINLINE	bool				is_empty()									CONST	{	 return	_b_empty;				}
	FINLINE	void				set_empty()											{	 _b_empty = true;				}
	FINLINE	void				clear_empty()										{	 _b_empty = false;				}
														//hack	test on 0x80 with cast just NULL was crashing
	FINLINE	bool				is_ok()										CONST
								{
#if AAA_BIT64()
									return ((UINT64)this)>0x80 && !is_empty();
#else
									return ((UINT32)this)>0x80 && !is_empty();
#endif
								}

	FINLINE	bool				is_format(				aaa::PIXEL_FORMAT CONST pixel_format ) CONST				{	return get_pixel_format() == pixel_format; }
	FINLINE	bool				is_format_ignore_bgr(	aaa::PIXEL_FORMAT CONST pixel_format ) CONST
								{
									auto format = get_pixel_format();
									return	format == pixel_format || aaa::c_pixel_format::get_format_red_blue_swapped( format ) == pixel_format;
								}
	FINLINE	bool				is_size(			INT32 CONST sx, INT32 CONST sy ) CONST							{	return get_size_x() == sx && get_size_y() == sy; }

//	FINLINE	bool				is_size_channel(	INT32 CONST sx, INT32 CONST sy, INT32 CONST ch_nb ) CONST		{	return is_size( sx,sy ) && get_channel_nb() == ch_nb;	}
	FINLINE	bool				is_size_format(		INT32 CONST sx, INT32 CONST sy, aaa::PIXEL_FORMAT CONST pixel_format ) CONST
								{
									return is_size( sx,sy ) && is_format( pixel_format );
								}
	FINLINE	bool				is_size_format_ignore_bgr(		INT32 CONST sx, INT32 CONST sy, aaa::PIXEL_FORMAT CONST pixel_format ) CONST
								{
									return is_size( sx,sy ) && is_format_ignore_bgr( pixel_format );
								}


			bool				check_full(			C_PCHAR_C signature ) CONST; 

	FINLINE bool				is_data_valid(		C_PCHAR_C signature ) CONST
								{
//	always until we solve it
//#if	AAA_DEBUG()
									if( !check_full( signature ) )
										return false;
//#endif
									return _data;
								}
	FINLINE	bool				check_data_valid(	C_PCHAR_C signature ) CONST
								{
									if( !is_data_valid( signature ) )
									{
										ERR_PRINT_STRING( "%s() data invalid", signature );
										return false;
									}
									return true;
								}
	FINLINE bool				is_valid()	CONST	{	return this ? is_data_valid(__FUNCTION__) : false;	}

	FINLINE	bool				check_valid(		C_PCHAR_C signature ) CONST
								{
									IF_THIS_NULL()
									{
										ERR_PRINT_STRING( "%s() img NULL pointer passed,", signature );
										return false;
									}
									return check_data_valid( signature );
								}
	FINLINE	UINT8*				get_data_uint8()								CONST	{	return get_data_type() == aaa::PIXEL_TYPE::UINT_8	? (UINT8*)	_data	: nullptr;	}
	FINLINE	UINT16*				get_data_uint16()								CONST	{	return get_data_type() == aaa::PIXEL_TYPE::UINT_16	? (UINT16*)	_data	: nullptr;	}
	FINLINE	FP16*				get_data_fp16()									CONST	{	return get_data_type() == aaa::PIXEL_TYPE::FLOAT_16	? (FP16*)	_data	: nullptr;	}
	FINLINE	FP32*				get_data_fp32()									CONST	{	return get_data_type() == aaa::PIXEL_TYPE::FLOAT_32	? (FP32*)	_data	: nullptr;	}
	FINLINE	void*				get_data()										CONST	{	return is_valid()									? _data				: nullptr;	}
	FINLINE	void*				get_data_as_type(	aaa::PIXEL_TYPE	  typ )		CONST	{	return get_data_type() == typ						? _data				: nullptr;	}
	FINLINE	void*				get_data_as_format(	aaa::PIXEL_FORMAT format )	CONST	{	return get_pixel_format() == format					? _data				: nullptr;	}

	FINLINE	UINT8*				get_data_valid_rgb_uint8( C_PCHAR_C signature )
								{
									if( !check_valid( signature ) )
										return nullptr;
									if( !aaa::c_pixel_format::is_format_rgb_i8( _pixel_format ) )
									{
										print_err_unsupported_format( signature );
										return nullptr;
									}
									UINT8* data = get_data_uint8();
									if( !data )
										ERR_PRINT_STRING( "%s() no data uint8.", signature );
									return data;
								}
	//for performance sensitive path
	FINLINE	UINT8*				get_data_rgb_uint8()		CONST
								{
									if( !aaa::c_pixel_format::is_format_rgb_i8( _pixel_format ) )
										return nullptr;
									return get_data_uint8();
								}	

			void				change_data(		UINT8* CONST data_new, UINT64 CONST size_new, bool CONST b_owner_new );

	FINLINE	void				erase_filename()									{ _filename.erase(); }
	FINLINE	void				set_filename(		o_str CONST &	filename )		{ _filename.set( filename ); }
	FINLINE	void				set_filename_rel(	o_str CONST &	filename )		{ _filename.set_fname_relative( filename ); }
	FINLINE	C_PCHAR_C			get_filename()				CONST					{
																						//if ( _b_compressed_data )
																						//	return _filename_compressed.get();
																						//else
																						return _filename.get();
																					}
	FINLINE	bool				is_filename()				CONST					{ return !_filename.is_empty(); }
	FINLINE	o_str CONST &		get_filename_o_str()		CONST					{ return _filename;	}

	FINLINE	time_t				get_time_modification()		CONST					{ return _time_modification; }
	FINLINE	void				set_time_modification(		CONST time_t t_in )		{ _time_modification = t_in ; }
	FINLINE	void				reset_time_modification()							{ set_time_modification( 0 ); }
	// can be used to know if there is file associated
	FINLINE	bool				is_time_modification()		CONST					{ return get_time_modification ()!= 0; }

	virtual void				print_info()				CONST;

	FINLINE	void				lock()
								{
									_lock.lock();
									_b_lock = true;
								}
	FINLINE	bool				try_lock()
								{
									_b_lock = _lock.try_lock();
									return _b_lock;
								}
	FINLINE	void				unlock()
								{
									_b_lock = false;
									_lock.unlock();
								}

	FINLINE	UINT32				get_state_unique()					CONST	{ return _state_unique.get();		}
	FINLINE	bool				is_state_unique( UINT32 CONST id )	CONST	{ return get_state_unique()==id;	}

			void				set_reading( bool b )				{ _b_reading = b;		}
			bool				is_reading()				CONST	{ return _b_reading;	}


	//this was a mechanism to check if we' need to transfer to gpu
	//		now changed do this (check)
	//		and this is used to know if we need to redo a full transfer a just a sub image
	//		we should move to a check against the info stored in tex
//gl_bind
//	FINLINE	bool				is_gl_bind( INT32 bind )			{ return gl_bind_ == bind; } 
//	FINLINE	void				set_gl_bind( INT32 bind_in )		{ gl_bind_ = bind_in; } 
//	FINLINE	void				clear_gl_bind()						{ gl_bind_ = -1; } 

	FINLINE	bool				is_changed()				CONST	{ return _b_changed; }
	FINLINE	void				set_changed()
								{ 
									_b_changed = true; 
									_state_unique.change();
									clear_empty();
								}
	FINLINE	void				set_gpu_in_sync( INT32 CONST index = -1 )	// param unused anyhow
								{
									_b_changed = false;
//									gl_bind_ = index; 
								}

	INT32		get_checksum();

	//todo	better memory scheme
	template< class IMG >	static	IMG*	read(	IMG* image, o_str CONST & filename,	
																bool CONST b_load_data = true,
																bool CONST b_free = false,
																bool CONST b_force_keep = false,
																bool CONST b_premultiply = false );
	template< class IMG >	static	IMG*	reread(	IMG* image	);
};



