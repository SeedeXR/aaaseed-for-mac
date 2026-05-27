
#ifdef AAA_IMG_3D_H
#error "IMG_3D_H included more than once."
#endif
#define AAA_IMG_3D_H 1


#ifndef	AAA_IMG_BASE_H
#	include "img_base.h"
#endif
#ifndef AAA_IMG_UTILS_H
#	include "img_utils.h"
#endif

class c_file_io;

class	c_img_3d final : public c_img_base
{
private:
	INT32		_sz;
	FP32		_over_sz;
	c_file_io*	_file_io;

protected:
	c_img_3d();

public:
	static c_img_3d*		_lua_cur;

	virtual ~c_img_3d();

	FINLINE static	INT32	get_dim()	{	return 3;	}

	static c_img_3d*		create( C_PCHAR_C signature );
	static c_img_3d*		create( C_PCHAR_C signature, INT32 CONST index );

	FINLINE	INT32			get_size_z() CONST			{	return _sz;							}
	FINLINE	void			get_size_xyz( INT32& sx, INT32& sy, INT32& sz ) CONST
														{	get_size_xy( sx, sy );	sz = _sz;	}
	FINLINE	FP32			get_over_size_z() CONST		{	return _over_sz;					}

	FINLINE	bool			is_size_format(						INT32 CONST sx, INT32 CONST sy, INT32 CONST sz, aaa::PIXEL_FORMAT pixel_format ) CONST
							{
								return get_size_x() == sx && get_size_y() == sy && get_size_z() == sz && get_pixel_format() == pixel_format;
							}

			void			set_size_format(					INT32 CONST sx, INT32 CONST sy, INT32 CONST sz, aaa::PIXEL_FORMAT CONST format );

	FINLINE	bool			is_valid_xyz(						INT32 CONST x, INT32 CONST y, INT32 CONST z ) CONST
							{
								return 0 <= z && z < _sz && is_valid_xy( x, y );
							}

			void			fill_rgba(							FP32 CONST * CONST color );

	virtual void			print_info() CONST;

			void			init_from_mem(						INT32 CONST sx, INT32 CONST sy, INT32 CONST sz,	aaa::PIXEL_FORMAT CONST format, UINT8* CONST data, INT32 data_buf_size = 0 );
			AAA_ERR			init_with_size(						INT32 CONST sx, INT32 CONST sy, INT32 CONST sz, aaa::PIXEL_FORMAT CONST format,	C_PCHAR_C signature );
			AAA_ERR			init_with_size_no_cpu_mem(			INT32 CONST sx, INT32 CONST sy, INT32 CONST sz, aaa::PIXEL_FORMAT CONST format,	C_PCHAR_C signature );
	
			AAA_ERR			alloc_data(							INT32 CONST sx, INT32 CONST sy, INT32 CONST sz, aaa::PIXEL_FORMAT CONST format,	C_PCHAR_C signature );
			
//			void			read_pixels(						INT32 CONST x,	INT32 CONST y,	INT32 CONST z,
//																INT32 CONST sx, INT32 CONST sy,	INT32 CONST sz, INT32 CONST which_buffer );
			void			move_to_gpu(						C_PCHAR_C signature,  INT32 index_to_debug=-43 );

	static	c_img_3d*		img_init_with_size(	c_img_3d* pt,	INT32 CONST sx, INT32 CONST sy, INT32 CONST sz, aaa::PIXEL_FORMAT CONST format,	C_PCHAR_C signature );

			AAA_ERR			read_from_existing_file(			o_str CONST & filename, bool CONST b_load_data );

			AAA_ERR			read_vtk(							o_str CONST & filename, bool b_load_data );
			AAA_ERR			read_pvm(							o_str CONST & filename, bool b_load_data );
			AAA_ERR			read_dds(							o_str CONST & filename, bool b_load_data );
			AAA_ERR			read_raw(							o_str CONST & filename, bool b_load_data );

			AAA_ERR			write_vtk(							o_str CONST & filename );
			AAA_ERR			write(								o_str CONST & filename, c_img_utils::FILE_TYPE_3D image_file_type = c_img_utils::FILE_TYPE_3D::DEFAULT );

			void			premultiply_alpha() {}

protected:
private:
	template< typename T > void fill_channel_1_low( T* dst, INT32 CONST sx, INT32 CONST sy, INT32 CONST sz, INT32 CONST pitch, T CONST v1 )
	{
		--dst;
		for( UINT32 s=sy*sz; s > 0; --s )
		{
			T*	p = dst;
			for( INT32 ix = sx; ix > 0; --ix )	{	*++p = v1;	}
			dst += pitch;
		}
	}
	template< typename T > void fill_channel_1( T* CONST dst, INT32 CONST pitch, T CONST v1 )
	{
		fill_channel_1_low( dst, get_size_x(), get_size_y(), get_size_z(), pitch, v1 );
		set_changed();
	}
	template< typename T > void fill_channel_2_low( T* dst, INT32 CONST sx, INT32 CONST sy, INT32 CONST sz, INT32 CONST pitch, T CONST v1, T CONST v2 )
	{
		--dst;
		for( UINT32 s=sy*sz; s > 0; --s )
		{
			T*	p = dst;
			for( INT32 ix = sx; ix > 0; --ix )	{	*++p = v1;	*++p = v2;	}
			dst += pitch;
		}
	}
	template< typename T > void fill_channel_2( T* CONST dst, INT32 CONST pitch, T CONST v1, T CONST v2 )
	{
		fill_channel_2_low( dst, get_size_x(), get_size_y(), get_size_z(), pitch, v1, v2 );
		set_changed();
	}
	template< typename T > void fill_channel_3_low( T* dst, INT32 CONST sx, INT32 CONST sy, INT32 CONST sz, INT32 CONST pitch, T CONST v1, T CONST v2, T CONST v3 )
	{
		--dst;
		for( UINT32 s=sy*sz; s > 0; --s )
		{
			T*	p = dst;
			for( INT32 ix = sx; ix > 0; --ix )	{	*++p = v1;	*++p = v2;	*++p = v3;	}
			dst += pitch;
		}
	}
	template< typename T > void fill_channel_3( T* CONST dst, INT32 CONST pitch, T CONST v1, T CONST v2, T CONST v3 )
	{
		fill_channel_3_low( dst,get_size_x(), get_size_y(), get_size_z(), pitch, v1, v2, v3 );
		set_changed();
	}
	template< typename T > void fill_channel_4_low( T* dst, INT32 CONST sx, INT32 CONST sy, INT32 CONST sz, INT32 CONST pitch, T CONST v1, T CONST v2, T CONST v3, T CONST v4 )
	{
		--dst;
		for( UINT32 s=sy*sz; s > 0; --s )
		{
			T*	p = dst;
			for( INT32 ix = sx; ix > 0; --ix )	{	*++p = v1;	*++p = v2;	*++p = v3;	*++p = v4;	}
			dst += pitch;
		}
	}
	template< typename T > void fill_channel_4( T* CONST dst, INT32 CONST pitch, T CONST v1, T CONST v2, T CONST v3, T CONST v4 )
	{
		fill_channel_4_low( dst, get_size_x(), get_size_y(), get_size_z(), pitch, v1, v2, v3, v4 );
		set_changed();
	}

};
