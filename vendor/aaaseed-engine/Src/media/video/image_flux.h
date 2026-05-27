
#ifdef AAA_IMAGE_FLUX_H
#error "IMAGE_FLUX_H included more than once."
#endif
#define AAA_IMAGE_FLUX_H 1


#ifndef AAA_IMG_H
#	include "image/img.h"
#endif
#ifndef	AAA_PIXEL_FORMAT_H
#	include "image/pixel_format.h"
#endif
#ifndef AAA_AAA_TIME_H
#	include "time/aaa_time.h"
#endif
#ifndef _DEQUE_
#	include <deque>
#endif

//class c_frame_info
//{
//public:
//	UINT32			sx;
//	UINT32			sy;
//	UINT32			stride;
//	PIXEL_FORMAT	format;
//	UINT8*			src;
//	c_frame_info()
//		:sx			(	0		)
//		,sy			(	0		)
//		,stride		(	0		)
//		,format		(	PIXEL_FORMAT::UNKNOWN	)
//		,src		(	nullptr	)
//	{}
//};

class	c_image_flux_buffer final : public c_img_base
{
private:
	typedef	std::deque<c_img_2d*>	IMG_CONT;
	typedef	IMG_CONT::iterator	IMG_ITER;	//todo use auto where it is used
	static void free_container_image_lock( IMG_CONT& container );

	IMG_CONT			_images_free;
	IMG_CONT			_images_used;
	INT32				_image_nb_to_keep	{};
	mutable aaa::MUTEX*	_images_lock		{};
	bool				_b_callback			{};

	INT32				_image_index_in		{-1};
	void*				_owner				{};

			void				sync_low( INT32 size_x, INT32 size_y, aaa::PIXEL_FORMAT format );

public:
			c_image_flux_buffer();
	virtual	~c_image_flux_buffer();

			void				alloc( INT32 size_x, INT32 size_y, aaa::PIXEL_FORMAT format, INT32 nb_in );
			void				dealloc();

	FINLINE	void				sync( INT32 size_x, INT32 size_y, aaa::PIXEL_FORMAT format )
								{
									if(	!is_size_format( size_x,size_y, format ) )
										sync_low( size_x,size_y, format );
								}

	FINLINE	void				set_callback( bool CONST b_in )	
								{
									if( _b_callback != b_in )
									{
										if( _b_callback )
										{	//todo atomics
											std::lock_guard<aaa::MUTEX> guard(*_images_lock);
											_b_callback = b_in;
										}
										else
											_b_callback = b_in;
									}
								}

			c_img_2d*			get_free_image_locked();
			c_img_2d*			lock_and_get_image_index( INT32 image_index, bool b_get_closest, bool& b_img_locked ); 

			void				clear_image_index();
	FINLINE	INT32				get_image_index() CONST 		{	return	_image_index_in;	}
			void				inc_image_index();

	FINLINE void				set_owner( void* owner )		{	_owner = owner;	}
	FINLINE bool				is_owner( void* owner )	CONST	{	return _owner == owner;	}
};

class c_tex_video;
class c_speed;


class	c_image_flux : public c_obj
{
private:
	c_tex_video*		_p_tex_video		{nullptr};
	bool				_b_flip_vertical	{};
	bool				_b_valid;
	bool				_b_frame_callback	{};
	bool				_b_use_last_frame;

	o_str				_flux_name;

	aaa::PIXEL_FORMAT	_src_pixel_format	{aaa::PIXEL_FORMAT::UNKNOWN};		/// Source pixel format

public:
	FINLINE	c_tex_video*	get_tex_video()			CONST					{	return _p_tex_video;		}
			void			set_tex_video(			c_tex_video* CONST p );

	FINLINE	bool			is_flip_vertical()		CONST 					{	return _b_flip_vertical;	}
	FINLINE	void			set_flip_vertical(		bool CONST b )			{	_b_flip_vertical = b;		}

	FINLINE	bool			is_frame_callback()		CONST 					{	return _b_frame_callback;	}
	FINLINE	void			set_frame_callback(		bool CONST b )			{	_b_frame_callback = b;		}

	FINLINE	bool			is_valid()				CONST 					{	return _b_valid;			}
	FINLINE	void			set_valid(				bool CONST b )			{	_b_valid = b;				}

	FINLINE	void			set_use_last_frame(		bool CONST b )			{	_b_use_last_frame = b;		}

protected:
	aaa::PIXEL_FORMAT_SRC_FORCE		_s_force_src_pixel_format;
	aaa::PIXEL_FORMAT_FORCE			_s_force_out_pixel_format;
	aaa::PIXEL_FORMAT				_pixel_format_default;
	aaa::PIXEL_FORMAT				_pixel_format_used;	

private:
	UINT32					_size_x;		//	size of image taking crop in account
	UINT32					_size_y;
	UINT32					_flux_size_x;	//	size of flux
	UINT32					_flux_size_y;
protected:
	bool					_b_crop;
	bool					_b_crop_h;
	bool					_b_crop_v;
	UINT32					_crop_left;
	UINT32					_crop_right;
	UINT32					_crop_top;
	UINT32					_crop_bottom;

	bool					_b_src_y_inverted;		// Some library will produced inverted images, need to flip them
	bool					_b_src_bgr;
	bool					_b_swap_red_blue;

	bool					_b_field_flip_order;
	bool					_b_do_field_separation;

//unused	INT32				_b_async;
	c_image_flux_buffer*	_image_flux_buffer;

	st_threshold			_threshold;
	bool					_b_threshold;
	REAL					_luma_min;
	REAL					_luma_max;

	REAL					_disp_min;
	REAL					_disp_max;


	INT32					_image_flux_obj_index;

	REAL					_min;
	REAL					_max;
	REAL					_gain_factor;
	REAL					_bias_factor;
//	bool					_b_lut_inverse;

//	REAL					_time_got_frame;
//	REAL					_time_got_frame_last;

//	bool					_b_alpha_done;

	//	STATE
	id_unique::c_u32		_state_unique;

	aaa::time::ST_TIME		_time_move_begin;
	aaa::time::ST_TIME		_time_move_end;

	c_speed*				_speed_fps;

	//just for info now
	CONST void*	RESTRICT	_p_buffer_first;
	INT32					_buffer_count;

private:
	void				set_flux_size_format_direct(	INT32 CONST size_x_in, INT32 CONST size_y_in, aaa::PIXEL_FORMAT CONST pixel_format );
	bool				copy_frame_to_img( c_img_2d* CONST img_dst, CONST UINT8 * src8, UINT32 src8_pitch, st_img_conv & options, bool b_field_one );
public:
	static	INT32			image_flux_obj_count;
//private:
//	INT32					_audio_index_in;
public:
	c_image_flux( c_image_flux_buffer* buf, bool b_callback );
	virtual ~c_image_flux();

	FINLINE	aaa::PIXEL_FORMAT	get_pixel_format_used()		CONST	{	return _pixel_format_used;		}
	FINLINE	aaa::PIXEL_FORMAT	get_src_pixel_format()		CONST	{	return _src_pixel_format;		}
	FINLINE	void set_src_pixel_format( aaa::PIXEL_FORMAT f_in )		{	_src_pixel_format = f_in;		}
	//FINLINE	aaa::PIXEL_FORMAT	get_pixel_format_default()	CONST	{	return _pixel_format_default;	}

//	ACCESSORS	SET
public:
			void				set_flux_size_format(			INT32 CONST size_x_in, INT32 CONST size_y_in, aaa::PIXEL_FORMAT CONST pixel_format );

	FINLINE	o_str*				get_flux_name_pt()					{	return &_flux_name;			}
	FINLINE	C_PCHAR_C			get_flux_name()	CONST				{	return _flux_name.get();	}
	FINLINE	void				set_flux_name( C_PCHAR_C	str )	{	_flux_name.set(str);		}
	FINLINE	void				set_flux_name( C_PWCHAR_C	str )	{	_flux_name.set(str);		}
	FINLINE	void				set_flux_name( o_str CONST & o )	{	_flux_name.set(o);			}

	FINLINE	UINT32				get_state_unique()	CONST					{	 return _state_unique.get(); }
	FINLINE	bool				is_state_unique( UINT32 CONST id ) CONST	{	 return get_state_unique() == id; }

	FINLINE	void				set_changed()								{	_state_unique.change();		}

			void				clear_image_index();
			INT32				get_image_index()	CONST;

//			void				clear_audio_index();
//			void				inc_audio_index();
//			void				clear_stream_index();

			void				got_frame( CONST UINT8* CONST RESTRICT p, C_PCHAR_C signature, UINT32 CONST src_pitch = 0, bool CONST b_force_alpha = false, REAL CONST alpha_value = 1. );
	FINLINE	UINT32				get_size_x()		CONST					{	return _size_x;			}
	FINLINE	UINT32				get_size_y()		CONST					{	return _size_y;			}
	FINLINE	UINT32				get_flux_size_x()	CONST					{	return _flux_size_x;	}
	FINLINE	UINT32				get_flux_size_y()	CONST					{	return _flux_size_y;	}
			REAL				get_fps()			CONST;

	FINLINE	void				set_swap_red_blue( bool CONST b_in )		{	_b_swap_red_blue = b_in;	}
	FINLINE bool				is_swap_red_blue()		CONST;

	//unused
	//		void	start_loop();

	FINLINE	aaa::PIXEL_FORMAT_SRC_FORCE	get_force_src_pixel_format()	CONST									{	return _s_force_src_pixel_format;	}
	FINLINE	void						set_force_src_pixel_format( CONST aaa::PIXEL_FORMAT_SRC_FORCE s_in )	{	_s_force_src_pixel_format = s_in;	}

	FINLINE	aaa::PIXEL_FORMAT_FORCE		get_force_out_pixel_format()	CONST									{	return _s_force_out_pixel_format;	}
	FINLINE	void						set_force_out_pixel_format( CONST aaa::PIXEL_FORMAT_FORCE s_in )		{	_s_force_out_pixel_format = s_in;	}

	FINLINE	void				set_do_field_split( bool CONST b_in )		{	_b_do_field_separation = b_in;	}
	FINLINE	bool				is_field() CONST							{	return _b_do_field_separation;	}
	FINLINE	void				set_field_flip( bool CONST b_in )			{	_b_field_flip_order = b_in;		}

	FINLINE	void				set_threshold( bool CONST b, REAL CONST th, REAL CONST offset, REAL CONST factor )
								{
									_b_threshold			= b;
									_threshold.threshold	= th;
									_threshold.y_offset		= offset;
									_threshold.y_factor		= factor;
								}
	//FINLINE	void	set_y_transform( REAL offset_in, REAL factor_in )	{ threshold._y_offset = offset_in; threshold._y_factor = factor_in; }
	FINLINE	void				set_image_nb_to_keep( INT32 CONST nb_in )					{ _image_flux_buffer->alloc( get_size_x(), get_size_y(), get_pixel_format_used(), nb_in ); }
	FINLINE	void				set_luma_min_max( REAL CONST min_in, REAL CONST max_in )	{ _luma_min = min_in; _luma_max = max_in; }
	FINLINE	void				set_disp_min_max( REAL CONST min_in, REAL CONST max_in )	{ _disp_min = min_in; _disp_max = max_in; }
	FINLINE	void				set_src_y_inverted( bool CONST b_in )						{ _b_src_y_inverted = b_in; }
	FINLINE	void				set_crop( bool CONST b_crop, UINT32 CONST crop_left, UINT32 CONST crop_right, UINT32 CONST crop_top, UINT32 CONST crop_bottom )
								{
									_b_crop			= b_crop;
									_crop_left		= crop_left;
									_crop_right		= crop_right;
									_crop_top		= crop_top;
									_crop_bottom	= crop_bottom;
								}

public:
			c_img_2d*			lock_and_get_image_index( INT32 CONST index, bool& b_img_locked ) CONST;
};

