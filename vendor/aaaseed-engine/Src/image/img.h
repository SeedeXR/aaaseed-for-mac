
#ifdef AAA_IMG_H
#error "IMG_H included more than once."
#endif
#define AAA_IMG_H 1


#ifndef AAA_IMG_BASE_H
#	include "img_base.h"
#endif
#ifndef AAA_IMG_UTILS_H
#	include "img_utils.h"
#endif
#ifndef AAA_V_BASE_H
#	include "math/v_base.h"
#endif
#ifndef AAA_AAA_RECT_H
#	include "aaa/aaa_rect.h"
#endif


//using namespace aaa;
class c_file_io;
struct st_img_send;

class	c_img_2d final : public c_img_base
{
//archi perhaps these friend are the best way to do it 
//friend	void	difrea_init_from_image( c_img_2d *img );
//friend	void	difrea_start( c_img_2d *img,  bool in );
//friend	class	c_bdd_feedback;

public:
protected:	//hack for tex3d
	c_img_2d();


public:
	static c_img_2d*	_lua_cur;

	virtual ~c_img_2d();

	FINLINE static	INT32	get_dim()	{	return 2;	}

private:
	static	void			c_init_send();
	static	void			c_deinit_send();
public:
	static	void			c_init();
	static	void			c_deinit();

	static c_img_2d*		create( C_PCHAR_C signature );
	static c_img_2d*		create( C_PCHAR_C signature, INT32 CONST index );

			void			init_from_mem(				INT32 CONST sx, INT32 CONST sy, aaa::PIXEL_FORMAT CONST format, UINT8* CONST data, UINT64 data_buf_size = 0 );
			AAA_ERR			init_with_size(				INT32 CONST sx, INT32 CONST sy, aaa::PIXEL_FORMAT CONST format,	C_PCHAR_C signature );
			AAA_ERR			init_with_size_no_cpu_mem(	INT32 CONST sx, INT32 CONST sy, aaa::PIXEL_FORMAT CONST format,	C_PCHAR_C signature );
	
			AAA_ERR			alloc_data(					INT32 CONST sx, INT32 CONST sy,	aaa::PIXEL_FORMAT CONST format, C_PCHAR_C signature );
			void			set_colum(					UINT8* buf, INT32 CONST x, INT32 CONST h );

			AAA_ERR			resize(						FP32 CONST factor );

	FINLINE	void			uv_to_pixel(				INT32& iu, INT32& iv,	REAL CONST u,	REAL CONST v,	bool CONST b_clamped ) CONST;
	FINLINE	void			uv_to_pixels(				INT32& cu, INT32& cv,
														INT32& nu, INT32& nv,	REAL CONST u,	REAL CONST v,	bool CONST b_clamped ) CONST;
	FINLINE	void			uv_to_pixels_with_uv(		INT32& cu, INT32& cv,
														INT32& nu, INT32& nv,	REAL& u,		REAL& v,		bool CONST b_clamped ) CONST;
//	FINLINE	bool			uv_to_pixels(				INT32& pu, INT32& pv,
//														INT32& cu, INT32& cv,
//														INT32& nu, INT32& nv,	REAL& u,		REAL& v,		bool CONST b_clamped );

//todo there is some mixture between _low (x y are valid) and _valid (img is valid)
	template < aaa::PIXEL_TYPE type >
	FINLINE FP32			get_valid_value_from_xy_low(						INT32 CONST x,	INT32 CONST y,	aaa::COMPO CONST what	) CONST;
	FINLINE	FP32			get_valid_value_from_xy(							INT32 CONST x,	INT32 CONST y,	aaa::COMPO CONST what	) CONST;
			FP32			get_value_from_xy(									INT32 CONST x,	INT32 CONST y,	aaa::COMPO CONST what	);

			void			get_color3r_from_xy_low(			FP32* color,	INT32 CONST x,	INT32 CONST y	) CONST;
			void			get_color3r_from_xy(				FP32* color,	INT32 CONST x,	INT32 CONST y	);
			void			get_color4r_from_xy_low(			FP32* color,	INT32 CONST x,	INT32 CONST y	) CONST;
			void			get_color4r_from_xy(				FP32* color,	INT32 CONST x,	INT32 CONST y	);

			void			set_xy_color3r_low(									INT32 CONST x,	INT32 CONST y,	FP32 CONST * CONST color	 );
			void			set_xy_color3r(										INT32 CONST x,	INT32 CONST y,	FP32 CONST * CONST color	 );
			void			set_xy_color4r_low(									INT32 CONST x,	INT32 CONST y,	FP32 CONST * CONST color	 );
			void			set_xy_color4r(										INT32 CONST x,	INT32 CONST y,	FP32 CONST * CONST color	 );

// VALUE
	FINLINE	FP32			get_valid_value_from_uv_linear(						REAL CONST u,	REAL CONST v,	bool CONST b_clamped,	aaa::COMPO CONST what	) CONST;
	FINLINE	FP32			get_valid_value_from_uv_nearest(					REAL CONST u,	REAL CONST v,	bool CONST b_clamped,	aaa::COMPO CONST what	) CONST;
			FP32			get_valid_value_from_uv(							REAL CONST u,	REAL CONST v,	bool CONST b_clamped,	aaa::COMPO CONST what,	bool CONST b_linear=false ) CONST;
	FINLINE	FP32			get_value_from_uv(									REAL CONST u,	REAL CONST v,	bool CONST b_clamped,	aaa::COMPO CONST what,	bool CONST b_linear=false )
							{
								if( is_valid() )
								{
									set_cpu_keep( true );
									return get_valid_value_from_uv( u,v, b_clamped, what, b_linear );
								}
								else
									return 0.;
							}

			FP32			get_valid_value_from_uv_ellipse(					REAL CONST u,	REAL CONST v,	 REAL CONST ru, REAL CONST rv, INT32 nb,		bool CONST b_clamped, aaa::COMPO CONST what, bool CONST b_linear ) CONST;
	FINLINE FP32			get_value_from_uv_ellipse(							REAL CONST u,	REAL CONST v,	 REAL CONST ru, REAL CONST rv, INT32 CONST nb,	bool CONST b_clamped, aaa::COMPO CONST what, bool CONST b_linear )
							{
								if( is_valid() )
								{
									set_cpu_keep( true );
									return get_valid_value_from_uv_ellipse( u,v, ru,rv, nb, b_clamped, what, b_linear );
								}
								else
									return 0.;
							}


// COLOR 3
			void			get_valid_color3r_from_uv_nearest(	FP32* color,	REAL CONST u,	REAL CONST v,	bool CONST b_clamped ) CONST;
			void			get_valid_color3r_from_uv_linear(	FP32* color,	REAL CONST u,	REAL CONST v,	bool CONST b_clamped ) CONST;
	FINLINE	void			get_valid_color3r_from_uv(			FP32* color,	REAL CONST u,	REAL CONST v,	bool CONST b_clamped,	bool CONST b_linear=false ) CONST
							{
								if( b_linear )
									get_valid_color3r_from_uv_linear(	color, u,v, b_clamped );
								else
									get_valid_color3r_from_uv_nearest(	color, u,v, b_clamped );
							}
			void			get_color3r_from_uv_nearest(		FP32* color,	REAL CONST u,	REAL CONST v,	bool CONST b_clamped	);
			void			get_color3r_from_uv_linear(			FP32* color,	REAL CONST u,	REAL CONST v,	bool CONST b_clamped	);
	FINLINE	void			get_color3r_from_uv(				FP32* color,	REAL CONST u,	REAL CONST v,	bool CONST b_clamped,	bool CONST b_linear=false )
							{
								if( b_linear )
									get_color3r_from_uv_linear(			color, u,v, b_clamped );
								else
									get_color3r_from_uv_nearest(		color, u,v, b_clamped );
							}
			void			get_valid_color3r_from_uv_ellipse(	FP32* color,	REAL CONST u,	REAL CONST v,	 REAL CONST ru, REAL CONST rv, INT32 nb,		bool CONST b_clamped, bool CONST b_linear ) CONST;
	FINLINE void			get_color3r_from_uv_ellipse(		FP32* color,	REAL CONST u,	REAL CONST v,	 REAL CONST ru, REAL CONST rv, INT32 CONST nb,	bool CONST b_clamped, bool CONST b_linear )
							{
								if( is_valid() )
								{
									set_cpu_keep( true );
									return get_valid_color3r_from_uv_ellipse( color, u,v, ru,rv, nb, b_clamped, b_linear );
								}
								else
									clear_v3( color );
							}
	// COLOR 4
			void			get_valid_color4r_from_uv_nearest(	FP32* color,	REAL CONST u,	REAL CONST v,	bool CONST b_clamped	) CONST;
			void			get_valid_color4r_from_uv_linear(	FP32* color,	REAL CONST u,	REAL CONST v,	bool CONST b_clamped	) CONST;
	FINLINE	void			get_valid_color4r_from_uv(			FP32* color,	REAL CONST u,	REAL CONST v,	bool CONST b_clamped,	bool CONST b_linear=false ) CONST
							{
								if( b_linear )
									get_valid_color4r_from_uv_linear(	color, u,v, b_clamped );
								else
									get_valid_color4r_from_uv_nearest(	color, u,v, b_clamped );
							}
			void			get_color4r_from_uv_nearest(		FP32* color,	REAL CONST u,	REAL CONST v,	bool CONST b_clamped	);
			void			get_color4r_from_uv_linear(			FP32* color,	REAL CONST u,	REAL CONST v,	bool CONST b_clamped	);
	FINLINE	void			get_color4r_from_uv(				FP32* color,	REAL CONST u,	REAL CONST v,	bool CONST b_clamped,	bool CONST b_linear=false )
							{
								if( b_linear )
									get_color4r_from_uv_linear(			color, u,v, b_clamped );
								else
									get_color4r_from_uv_nearest(		color, u,v, b_clamped );
							}

			void			get_valid_color4r_from_uv_ellipse(	FP32* color,	REAL CONST u,	REAL CONST v,	 REAL CONST ru, REAL CONST rv, INT32 nb,		bool CONST b_clamped, bool CONST b_linear ) CONST;
	FINLINE void			get_color4r_from_uv_ellipse(		FP32* color,	REAL CONST u,	REAL CONST v,	 REAL CONST ru, REAL CONST rv, INT32 CONST nb,	bool CONST b_clamped, bool CONST b_linear )
							{
								if( is_valid() )
								{
									set_cpu_keep( true );
									return get_valid_color4r_from_uv_ellipse( color, u,v, ru,rv, nb, b_clamped, b_linear );
								}
								else
									clear_v4( color );
							}

			UINT8*			get_color4ubv_from_uv(								REAL CONST u,	REAL CONST v	);
			void			mul_valid_and_set_color4v_from_uv(	FP32 CONST * CONST color,
																				REAL CONST u,	REAL CONST v,	bool CONST b_clamped );
// GRADIENT
			void			get_valid_gradient_from_uv_nearest(	REAL* dst,		REAL CONST u,	REAL CONST v,	 bool CONST b_clamped,	 aaa::COMPO CONST what ) CONST;
			void			get_valid_gradient_from_uv_linear(	REAL* dst,		REAL CONST u,	REAL CONST v,	 bool CONST b_clamped,	 aaa::COMPO CONST what ) CONST;
	FINLINE	void			get_valid_gradient_from_uv(			REAL* dst,		REAL CONST u,	REAL CONST v,	 bool CONST b_clamped,	 aaa::COMPO CONST what, bool CONST b_linear=false ) CONST
							{
								if( b_linear )
									get_valid_gradient_from_uv_linear(	dst, u,v, b_clamped, what );
								else
									get_valid_gradient_from_uv_nearest(	dst, u,v, b_clamped, what );
							}
				
	FINLINE	void			get_gradient_from_uv(				REAL* dst,		REAL CONST u,	REAL CONST v,	 bool CONST b_clamped,	 aaa::COMPO CONST what, bool CONST b_linear=false )
							{
								if( is_valid() )
								{
									
									set_cpu_keep( true );
									get_valid_gradient_from_uv( dst, u,v, b_clamped, what, b_linear );
								}
								else
								{
									*dst		= 0;
									*( dst+1 )	= 0;
								}
							}

			void			get_valid_gradient_from_uv_ellipse(	REAL* dst,		REAL CONST u,	REAL CONST v,	REAL CONST ru, REAL CONST rv,		INT32 nb, bool CONST b_clamped, aaa::COMPO CONST what, bool CONST b_linear=false ) CONST;
	FINLINE	void			get_gradient_from_uv_ellipse(		REAL* dst,		REAL CONST u,	REAL CONST v,	REAL CONST ru, REAL CONST rv, INT32 CONST nb, bool CONST b_clamped, aaa::COMPO CONST what, bool CONST b_linear=false )
							{
								if( is_valid() )
								{
									set_cpu_keep( true );
									get_valid_gradient_from_uv_ellipse( dst, u,v, ru,rv, nb, b_clamped, what, b_linear );
								}
								else
								{
									*dst		= 0;
									*( dst+1 )	= 0;
								}
							}
// VALUE MAX
	FINLINE	FP32			get_value_max_from_line_low(	INT32 CONST x_begin, INT32 CONST x_end, INT32 CONST y, aaa::COMPO CONST what );
			FP32			get_value_max_from_rect(		INT32 CONST x_begin, INT32 CONST x_end, INT32 CONST y_begin, INT32 CONST y_end, aaa::COMPO CONST what );

			void			merge_channel_to_rgba();
			void			compo_inverse(				UINT32 CONST index );
			void			alpha_inverse();
			void			inverse();
			void			transform_compo_to_white_with_alpha(	UINT32 CONST index );
			void			premultiply_alpha();

			void			fill_compo(					INT32 CONST compo_index, FP32 CONST val );
			void			fill_alpha(					FP32 CONST alpha );
			void			fill_alpha_from_rgb();
			void			fill_rgba(					FP32 CONST * CONST color );
			void			fill_rgba8_sse3(			FP32 CONST * CONST color );
			void			fill_with_colorbar_grey(	INT32 nb = 0 );	//	nb = 0 means as much as the resolution allow
			void			fill_with_colorbar();
			void			fill_with_2x2(				INT32 pat, FP32* col_0, FP32* col_1 );	//	low 4 bits of pat define the pattern (bit 0 is top left)

			void			compute_incrust();
			
			bool			find_rect_with_value(		aaa::rect::lbrt_real& rect, FP32 min, FP32 max, bool b_outside, aaa::COMPO what );

	//		AAA_ERR			write(	C_PCHAR_C filename, CONST FILE_TYPE image_file_type = FILE_TYPE::UNKNOWN );


			void			draw();
			void			read_pixels(				INT32 CONST x,	INT32 CONST y,
														INT32 CONST sx,	INT32 CONST sy, INT32 CONST which_buffer );

			void			move_to_gpu(				C_PCHAR_C signature, INT32 index_to_debug=-42 );
			void			move_from_gpu(				C_PCHAR_C signature, INT32 index_to_debug=-42 );
//			void			copy_to_gpu(				C_PCHAR_C signature, INT32 index_dst );
		
private:
	// all the copy return true when they are done
			bool			copy_src_to_r8(				UINT8 CONST * RESTRICT src8, INT32 src_pitch, aaa::PIXEL_FORMAT CONST src_format, st_img_conv& options );
			bool			copy_src_to_rg8(			UINT8 CONST * RESTRICT src8, INT32 src_pitch, aaa::PIXEL_FORMAT CONST src_format, st_img_conv& options );
			bool			copy_src_to_rgb8(			UINT8 CONST * RESTRICT src8, INT32 src_pitch, aaa::PIXEL_FORMAT CONST src_format, st_img_conv& options );
			bool			copy_src_to_rgba8(			UINT8 CONST * RESTRICT src8, INT32 src_pitch, aaa::PIXEL_FORMAT CONST src_format, st_img_conv& options );
										  			 											 
			bool			copy_src_to_r16(			UINT8 CONST * RESTRICT src8, INT32 src_pitch, aaa::PIXEL_FORMAT CONST src_format, st_img_conv& options );
			bool			copy_src_to_rg16(			UINT8 CONST * RESTRICT src8, INT32 src_pitch, aaa::PIXEL_FORMAT CONST src_format, st_img_conv& options );
			bool			copy_src_to_rgb16(			UINT8 CONST * RESTRICT src8, INT32 src_pitch, aaa::PIXEL_FORMAT CONST src_format, st_img_conv& options );
			bool			copy_src_to_bgra16(			UINT8 CONST * RESTRICT src8, INT32 src_pitch, aaa::PIXEL_FORMAT CONST src_format, st_img_conv& options );
			bool			copy_src_to_rgba16(			UINT8 CONST * RESTRICT src8, INT32 src_pitch, aaa::PIXEL_FORMAT CONST src_format, st_img_conv& options );
										 			 											 
			bool			copy_src_to_r16fp(			UINT8 CONST * RESTRICT src8, INT32 src_pitch, aaa::PIXEL_FORMAT CONST src_format, st_img_conv& options );
			bool			copy_src_to_rg16fp(			UINT8 CONST * RESTRICT src8, INT32 src_pitch, aaa::PIXEL_FORMAT CONST src_format, st_img_conv& options );
			bool			copy_src_to_rgb16fp(		UINT8 CONST * RESTRICT src8, INT32 src_pitch, aaa::PIXEL_FORMAT CONST src_format, st_img_conv& options );
			bool			copy_src_to_rgba16fp(		UINT8 CONST * RESTRICT src8, INT32 src_pitch, aaa::PIXEL_FORMAT CONST src_format, st_img_conv& options );
											  			 											 
			bool			copy_src_to_r32fp(			UINT8 CONST * RESTRICT src8, INT32 src_pitch, aaa::PIXEL_FORMAT CONST src_format, st_img_conv& options );
			bool			copy_src_to_rg32fp(			UINT8 CONST * RESTRICT src8, INT32 src_pitch, aaa::PIXEL_FORMAT CONST src_format, st_img_conv& options );
			bool			copy_src_to_rgba32fp(		UINT8 CONST * RESTRICT src8, INT32 src_pitch, aaa::PIXEL_FORMAT CONST src_format, st_img_conv& options );
			bool			copy_src_to_rgb32fp(		UINT8 CONST * RESTRICT src8, INT32 src_pitch, aaa::PIXEL_FORMAT CONST src_format, st_img_conv& options );

public:

			bool			copy_from_src(				CONST void* RESTRICT src, INT32 CONST src_pitch, st_img_conv& options );
			bool			copy_from_src(				CONST void* RESTRICT src, INT32 CONST src_pitch, aaa::PIXEL_FORMAT CONST pixel_format_src,	aaa::PIXEL_FORMAT CONST pixel_format_dst, UINT32 CONST sx, UINT32 CONST sy, bool CONST b_flip_vert );
			bool			copy_from_img(				c_img_2d* CONST img_src, bool CONST b_flip_vert, bool b_swap_red_blue = false );

			AAA_ERR			read_from_existing_file(	o_str CONST & filename,	bool CONST b_load_data );

			AAA_ERR			read_sgi(					FILE* file, o_str CONST & filename );
			AAA_ERR			write_tga(					o_str CONST & filename,	bool b_compressed, bool b_grey );
			AAA_ERR			read_tga(					c_file_io& file_io,		bool CONST b_load_data );
			AAA_ERR			read_tga(					o_str CONST & filename,	bool CONST b_load_data );
			AAA_ERR			write_yuv(					o_str CONST & filename	);
			AAA_ERR			read_yuv(					FILE* file			);

			AAA_ERR			write_cpp(					o_str CONST & filename );

			AAA_ERR			write_aaatc(				o_str CONST & filename, UINT8* CONST buffer, INT32 CONST format, UINT64 CONST size );
			AAA_ERR			read_aaatc(					FILE* file, o_str CONST & filename );
			AAA_ERR			read_astc(					FILE* file, o_str CONST & filename );
			AAA_ERR			read_dds(					FILE* file, o_str CONST & filename );
		
			AAA_ERR			write(						o_str CONST & filename, c_img_utils::FILE_TYPE image_file_type = c_img_utils::FILE_TYPE::DEFAULT );

			AAA_ERR			send(						st_img_send* st_send );
			AAA_ERR			receive_mem(				UINT8*& data );

	static	AAA_ERR			process_blk(				UINT8 CONST * CONST data, INT32 CONST len, INT32 CONST net_link_index );
			
	static	c_img_2d*		img_init_with_size(			c_img_2d* p_img, INT32 CONST sx, INT32 CONST sy, aaa::PIXEL_FORMAT CONST format, C_PCHAR_C signature );
	static	c_img_2d*		img_init_from_img(			c_img_2d* p_img, C_PCHAR_C signature );
	static	c_img_2d*		img_init_from_img_cropped(	c_img_2d* p_img, INT32 x, INT32 y, INT32 sx, INT32 sy );
	static	c_img_2d*		img_init_from_mem(			c_img_2d* p_img, INT32 CONST sx, INT32 CONST sy, aaa::PIXEL_FORMAT CONST format, UINT8* CONST new_data, UINT64 CONST	new_data_buf_size );

	virtual void			print_info() CONST;

	template< typename T > void fill_channel_1_low( T* dst, INT32 CONST sx, INT32 sy, INT32 CONST pitch, T CONST v1 )
	{
		--dst;
		for( ; sy > 0; --sy )
		{
			T*	p = dst;
			for( INT32 ix = sx; ix > 0; --ix )
				*++p = v1;
			dst += pitch;
		}
	}
	template< typename T > void fill_channel_1( T* CONST dst, INT32 CONST pitch, T CONST v1 )
	{
		fill_channel_1_low( dst, get_size_x(), get_size_y(), pitch, v1 );
		set_changed();
	}
	template< typename T > void fill_channel_2_low( T* dst, INT32 CONST sx, INT32 sy, INT32 CONST pitch, T CONST v1, T CONST v2 )
	{
		--dst;
		for( ; sy > 0; --sy )
		{
			T*	p = dst;
			for( INT32 ix = sx; ix > 0; --ix )
			{
				*++p = v1;
				*++p = v2;
			}
			dst += pitch;
		}
	}
	template< typename T > void fill_channel_2( T* CONST dst, INT32 CONST pitch, T CONST v1, T CONST v2 )
	{
		fill_channel_2_low( dst, get_size_x(), get_size_y(), pitch, v1, v2 );
		set_changed();
	}
	template< typename T > void fill_channel_3_low( T* dst, INT32 CONST sx, INT32 sy, INT32 pitch, T CONST v1, T CONST v2, T CONST v3 )
	{
		--dst;
		for( ; sy > 0; --sy )
		{
			T*	p = dst;
			for( INT32 ix = sx; ix > 0; --ix )
			{
				*++p = v1;
				*++p = v2;
				*++p = v3;
			}
			dst += pitch;
		}
	}
	template< typename T > void fill_channel_3( T* CONST dst, INT32 CONST pitch, T CONST v1, T CONST v2, T CONST v3 )
	{
		fill_channel_3_low( dst, get_size_x(), get_size_y(), pitch, v1, v2, v3 );
		set_changed();
	}
	template< typename T > void fill_channel_4_low( T* dst, INT32 CONST sx, INT32 sy, INT32 pitch, T CONST v1, T CONST v2, T CONST v3, T CONST v4 )
	{
		--dst;
		for( ; sy > 0; --sy )
		{
			T*	p = dst;
			for( INT32 ix = sx; ix > 0; --ix )
			{
				*++p = v1;
				*++p = v2;
				*++p = v3;
				*++p = v4;
			}
			dst += pitch;
		}
	}
	template< typename T > void fill_channel_4( T* CONST dst, INT32 CONST pitch, T CONST v1, T CONST v2, T CONST v3, T CONST v4 )
	{
		fill_channel_4_low( dst, get_size_x(), get_size_y(), pitch, v1, v2, v3, v4 );
		set_changed();
	}
protected:
	template< typename T, INT32 ch_nb >		void fill_compo_loop( UINT8* CONST dst, INT32 CONST sx, INT32 CONST sy, T CONST v );
};

