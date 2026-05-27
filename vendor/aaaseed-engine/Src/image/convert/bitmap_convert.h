
#ifdef AAA_BITMAP_CONVERT_H
#error "BITMAP_CONVERT_H included more than once."
#endif
#define AAA_BITMAP_CONVERT_H 1


#ifndef	AAA_PIXEL_FORMAT_H
#	include "image/pixel_format.h"
#endif
#ifndef AAA_IMG_H
#	include "image/img.h"
#endif

class bitcon
{
public:
	static FINLINE	INT32 shift_len_check_align( C_PCHAR_C fn_name, INT32 CONST len, INT32 CONST shift )
	{
		if( len & ((1<<shift)-1) )
		{
			ERR_PRINT_STRING( "in %s() the len %d should be a multiple of %d,", fn_name, len, 1<<shift ); 
			ERR_PRINT_STRING( "\tskipping last pixels." );
		}
		return len >> shift;
	}
	static FINLINE	INT32 shift_height_check_align( C_PCHAR_C fn_name, INT32 CONST height, INT32 CONST shift )
	{
		if( height & ((1<<shift)-1) )
		{
			ERR_PRINT_STRING( "in %s() the height %d should be a multiple of %d,", fn_name, height, 1<<shift ); 
			ERR_PRINT_STRING( "\tskipping last lines." );
		}
		return height >> shift;
	}
	// FP32 -> unorm conversion: bucketing scheme (intentional).
	// Maps [k/256, (k+1)/256) -> k, with c=1.0 clamped to 255. Equivalent to
	// floor(256*c) clamped, NOT round(255*c). Slight DC bias on tiny values
	// (e.g. c=0.002 -> 0 here, would be 1 with OpenGL-spec round-to-nearest)
	// but matches the convention used by many engines for speed and is what
	// the rest of the AAA conversion stack assumes. Same pattern in all the
	// color_to_u32r* / color_to_u16r16 / color_to_u64r* helpers below.
	static FINLINE UINT8	color_to_u8r8( FP32 CONST * CONST color )
	{
		return		UINT8( CLAMP( 256. * color[0], 0., 255. ) );
	}
	//todo not endian safe
	static FINLINE UINT32	color_to_u32r8( FP32 CONST * CONST color )
	{
		return		UINT32( CLAMP( 256. * color[0], 0., 255. ) )
				|	0xff<<24;
	}
	//todo not endian safe
	static FINLINE UINT32	color_to_u32rg8( FP32 CONST * CONST color )
	{
		return		UINT32( CLAMP( 256. * color[0], 0., 255. ) )
				|	UINT32( CLAMP( 256. * color[1], 0., 255. ) ) << 8
				|	0xff<<24;
	}
	//todo not endian safe
	static FINLINE UINT32	color_to_u32rgb8( FP32 CONST * CONST color )
	{
		return		UINT32( CLAMP( 256. * color[0], 0., 255. ) )
				|	UINT32( CLAMP( 256. * color[1], 0., 255. ) ) << 8
				|	UINT32( CLAMP( 256. * color[2], 0., 255. ) ) << 16
				|	0xff<<24;
	}
	//todo not endian safe
	static FINLINE UINT32	color_to_u32rgba8( FP32 CONST * CONST color ) 
	{
		return		UINT32( CLAMP( 256. * color[0], 0., 255. ) )
				|	UINT32( CLAMP( 256. * color[1], 0., 255. ) ) << 8
				|	UINT32( CLAMP( 256. * color[2], 0., 255. ) ) << 16
				|	UINT32( CLAMP( 256. * color[3], 0., 255. ) ) << 24;
	}
	//todo not endian safe
	static FINLINE UINT16	color_to_u16r16( FP32 CONST * CONST color ) 
	{
		return		UINT16( CLAMP( 65536 * color[0], 0., 65535. ) );
	}
	//todo not endian safe
	static FINLINE UINT32	color_to_u32rg16( FP32 CONST * CONST color ) 
	{
		return		(UINT32( CLAMP( 65536 * color[0], 0., 65535. ) ) & 0xffff )
				|	(UINT32( CLAMP( 65536 * color[1], 0., 65535. ) ) & 0xffff ) << 16;
	}
	//todo not endian safe
	static FINLINE UINT64	color_to_u64rg16( FP32 CONST * CONST color ) 
	{
		return		(UINT64( CLAMP( 65536 * color[0], 0., 65535. ) ) & 0xffff )
				|	(UINT64( CLAMP( 65536 * color[1], 0., 65535. ) ) & 0xffff ) << 16;
	}
	//todo not endian safe
	static FINLINE UINT64	color_to_u64rgb16( FP32 CONST * CONST color ) 
	{
		return		(UINT64( CLAMP( 65536 * color[0], 0., 65535. ) ) & 0xffff )
				|	(UINT64( CLAMP( 65536 * color[1], 0., 65535. ) ) & 0xffff ) << 16
				|	(UINT64( CLAMP( 65536 * color[2], 0., 65535. ) ) & 0xffff ) << 32;
	}
	//todo not endian safe
	static FINLINE UINT64	color_to_u64rgba16( FP32 CONST * CONST color ) 
	{
		return		(UINT64( CLAMP( 65536 * color[0], 0., 65535. ) ) & 0xffff )
				|	(UINT64( CLAMP( 65536 * color[1], 0., 65535. ) ) & 0xffff ) << 16
				|	(UINT64( CLAMP( 65536 * color[2], 0., 65535. ) ) & 0xffff ) << 32
				|	(UINT64( CLAMP( 65536 * color[3], 0., 65535. ) ) & 0xffff ) << 48;
	}

	template< aaa::PIXEL_FORMAT FORMAT >
	FINLINE static void write_pixel( void*& pt, FP32* color )
	{
	using aaa::PIXEL_FORMAT;
		switch( FORMAT )
		{
		case aaa::PIXEL_FORMAT::R_8:		*(UINT8*)pt = bitcon::color_to_u8r8( color );
											pt = (INT8*)pt + 1;
											break;
		case aaa::PIXEL_FORMAT::R_16:		*(UINT16*)pt = bitcon::color_to_u16r16( color );
											pt = (INT16*)pt + 1;
											break;
		case aaa::PIXEL_FORMAT::DEPTH_16:
		case aaa::PIXEL_FORMAT::R_16FP:		*(FP16*)pt = (FP16)*color;
											pt = (FP16*)pt + 1;
											break;
		case aaa::PIXEL_FORMAT::DEPTH_32:
		case aaa::PIXEL_FORMAT::R_32FP:		*(FP32*)pt = *color;
											pt = (FP32*)pt + 1;
											break;

		case aaa::PIXEL_FORMAT::RG_8:		{
												UINT32 src = bitcon::color_to_u32rg8( color );
												UINT8* dst = (UINT8*)pt;
												*dst++ = src  & 0xff;
												*dst++ = (src >> 8) & 0xff;
												pt = dst;
											}
											break;
		case aaa::PIXEL_FORMAT::RG_16:		{
												UINT32 src = bitcon::color_to_u32rg16( color );
												UINT16* dst = (UINT16*)pt;
												*dst++ = src & 0xffff;
												*dst++ = (src >> 16) & 0xffff;
												pt = dst;
											}
											break;
		case aaa::PIXEL_FORMAT::RG_16FP:	cpy_v2( (FP16*)pt, color );
											pt = (FP16*)pt + 2;
											break;
		case aaa::PIXEL_FORMAT::RG_32FP:	cpy_v2( (FP32*)pt, color );
											pt = (FP32*)pt + 2;
											break;

		case aaa::PIXEL_FORMAT::RGB_8:		{
												UINT32 src = bitcon::color_to_u32rgb8( color );
												UINT8* dst = (UINT8*)pt;
												*dst++ = src  & 0xff;
												*dst++ = (src >> 8) & 0xff;
												*dst++ = (src >> 16) & 0xff;
												pt = dst;
											}
											break;
		case aaa::PIXEL_FORMAT::RGB_16:		{
												UINT64 src = bitcon::color_to_u64rgb16( color );
												UINT16* dst = (UINT16*)pt;
												*dst++ = src & 0xffff;
												*dst++ = (src >> 16) & 0xffff;
												*dst++ = (src >> 32) & 0xffff;
												pt = dst;
											}
											break;
		case aaa::PIXEL_FORMAT::RGB_16FP:	cpy_v3( (FP16*)pt, color );
											pt = (FP16*)pt + 3;
											break;
		case aaa::PIXEL_FORMAT::RGB_32FP:	cpy_v3( (FP32*)pt, color );
											pt = (FP32*)pt + 3;
											break;
		case aaa::PIXEL_FORMAT::RGBA_8:		*((UINT32*)pt) = bitcon::color_to_u32rgba8( color );
											pt = (UINT8*)pt + 4;
											break;
		case aaa::PIXEL_FORMAT::RGBA_16:	*((UINT64*)pt) = bitcon::color_to_u64rgba16( color );
											pt = (UINT16*)pt + 4;
											break;
		case aaa::PIXEL_FORMAT::RGBA_16FP:	cpy_v4( (FP16*)pt, color );
											pt = (FP16*)pt + 4;
											break;
		case aaa::PIXEL_FORMAT::RGBA_32FP:	cpy_v4( (FP32*)pt, color );
											pt = (FP32*)pt + 4;
											break;
		}
	}

	static	void	init();

	static	void	yuyv_to_r8_fast (				UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb );
	static	void	rgb8_to_r8_fast (				UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb );
	static	void	rgba8_to_r8_fast(				UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb );
	static	void	yuyv_to_r8_lut  (				UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, UINT8 CONST * CONST lut );
	static	void	rgb8_to_r8_lut  (				UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, UINT8 CONST * CONST lut );
	static	void	rgba8_to_r8_lut (				UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, UINT8 CONST * CONST lut );
														  
	static	void	rgb8_green_to_r8 (				UINT8 CONST * RESTRICT src8, UINT8* RESTRICT st8, INT32 nb );
	static	void	rgba8_green_to_r8(				UINT8 CONST * RESTRICT src8, UINT8* RESTRICT st8, INT32 nb );

	static	void	bgr8_to_rgba8_fast(				UINT8 CONST * RESTRICT src8, UINT8* RESTRICT st8, INT32 nb, UINT8 CONST alpha );
	static	void	rgb8_to_rgba8_fast(				UINT8 CONST * RESTRICT src8, UINT8* RESTRICT st8, INT32 nb, UINT8 CONST alpha );
	static	void	bgra8_to_rgba8_fast(			UINT8 CONST * RESTRICT src8, UINT8* RESTRICT st8, INT32 nb );
	static	void	bgra8_to_rgba8_slow(			UINT8 CONST * RESTRICT src8, UINT8* RESTRICT st8, INT32 nb );
	static	void	rgb8_to_rgba8_slow(				UINT8 CONST * RESTRICT src8, UINT8* RESTRICT st8, INT32 nb, UINT8 CONST alpha );
				 
	static	void	rgba8_to_rgba8_incrust_green(	UINT8 CONST * RESTRICT src8, UINT8* RESTRICT st8, INT32 nb, UINT8 CONST * CONST lut );
	static	void	bgra8_to_rgba8_incrust_green(	UINT8 CONST * RESTRICT src8, UINT8* RESTRICT st8, INT32 nb, UINT8 CONST * CONST lut );
				 
	static	void	bgr8_to_rgba8_incrust(			UINT8 CONST * RESTRICT src8, UINT8* RESTRICT st8, INT32 nb, UINT8 CONST * CONST lut );
	static	void	rgb8_to_rgba8_incrust(			UINT8 CONST * RESTRICT src8, UINT8* RESTRICT st8, INT32 nb, UINT8 CONST * CONST lut );
	static	void	rgb8_to_bgra8_incrust_green(	UINT8 CONST * RESTRICT src8, UINT8* RESTRICT st8, INT32 nb, UINT8 CONST * CONST lut );

// I420
//
	// dst32_pitch may be negative
	static	void	i420_to_rgba8_ref(				UINT8 CONST * RESTRICT src_y8,  UINT8 CONST * RESTRICT src_u8, UINT8 CONST * RESTRICT src_v8, // Y plane (first row)m  U plane (width/2 contiguous), V plane (width/2 contiguous)  
													UINT32*	RESTRICT dst32, INT32  CONST dst32_pitch, st_img_conv CONST & options );
	static	void	i420_to_bgr8_slow(				UINT8 CONST * RESTRICT src, UINT8 CONST * RESTRICT src_a, UINT8 CONST * RESTRICT src_b, UINT8* RESTRICT dst, INT32 nb );

	static	void	update_convert_yuv_to_rgb();

	static	void	yuyv_to_rgba8_line_one(			UINT32 CONST * RESTRICT src32, UINT32* RESTRICT dst32, INT32 pixel_nb, st_img_conv CONST & options );
	static	void	uyvy_to_rgba8_line_one(			UINT32 CONST * RESTRICT src32, UINT32* RESTRICT dst32, INT32 pixel_nb, st_img_conv CONST & options );
	static	void	i420_to_rgba8_line_two(			UINT8 CONST * RESTRICT src8, UINT8 CONST * RESTRICT src_a8, UINT8 CONST * RESTRICT src_b8,	UINT32* RESTRICT dst32, INT32 dst32_pitch, st_img_conv CONST & options );
	static	void	nv12_to_rgba8_line_two(			UINT8 CONST * RESTRICT src8, UINT8 CONST * RESTRICT src_uv,									UINT32* RESTRICT dst32, INT32 dst32_pitch, st_img_conv CONST & options );


	static	void	yuyv_to_rgba8_line_block(		INT32 line_begin, INT32 line_nb, UINT32* RESTRICT dst32, INT32 dst32_pitch, st_img_conv CONST & options );
	static	void	uyvy_to_rgba8_line_block(		INT32 line_begin, INT32 line_nb, UINT32* RESTRICT dst32, INT32 dst32_pitch, st_img_conv CONST & options );
	static	void	i420_to_rgba8_line_block(		INT32 line_begin, INT32 line_nb, UINT32* RESTRICT dst32, INT32 dst32_pitch, st_img_conv CONST & options );
	static	void	nv12_to_rgba8_line_block(		INT32 line_begin, INT32 line_nb, UINT32* RESTRICT dst32, INT32 dst32_pitch, st_img_conv CONST & options );


	static	void	yuyv_to_rgba8_avx2(				INT32 line_begin, INT32 line_nb, UINT32* RESTRICT dst32, INT32 dst32_pitch, st_img_conv CONST & options );
	static	void	uyvy_to_rgba8_avx2(				INT32 line_begin, INT32 line_nb, UINT32* RESTRICT dst32, INT32 dst32_pitch, st_img_conv CONST & options );
	static	void	i420_to_rgba8_avx2(				INT32 line_begin, INT32 line_nb, UINT32* RESTRICT dst32, INT32 dst32_pitch, st_img_conv CONST & options );
	static	void	nv12_to_rgba8_avx2(				INT32 line_begin, INT32 line_nb, UINT32* RESTRICT dst32, INT32 dst32_pitch, st_img_conv CONST & options );


//DISPARITY
	static	bool	disparity16_to_rgba(			UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, REAL CONST _disp_min, REAL CONST _disp_max, UINT8 CONST alpha );
//GREY													  
	static	void	r8_to_rgba8_fast(				UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, UINT8 CONST alpha );
	static	void	r8_to_rgba8_slow(				UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, UINT8 CONST alpha );
	static	void	r8_to_rgba8_lut(				UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, UINT8 CONST * CONST RESTRICT lut );
	static	void	r8_to_r8_lut(					UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, UINT8 CONST * CONST RESTRICT lut );
														  
	static	void	r16_to_rgba8_fast(				UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, REAL CONST min, REAL CONST max, UINT8 CONST alpha );
	static	void	r16_to_rgba8_slow(				UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, REAL CONST min, REAL CONST max, UINT8 CONST alpha );
//YUYV
//	static	void	yuyv_to_rgba8_fast_threshold(	UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, st_img_conv CONST & options );
//	static	void	yuyv_to_rgba8_slow(				UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, st_img_conv CONST & options );
//	static	void	yuyv_to_bgra8_slow(				UINT8 CONST * RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, st_img_conv CONST & options );
//	static	void	yuyv_to_bgr8_slow(				UINT8 CONST * RESTRICT src, UINT8* RESTRICT dst, INT32 nb );

//UYUV
//	static	void	uyvy_to_rgba8_fast_threshold(	UINT8 CONST * RESTRICT src8, UINT8* dst8, INT32 nb, st_img_conv CONST & options );
//	static	void	uyvy_to_rgba8_fast(				UINT8 CONST * RESTRICT src8, UINT8* dst8, INT32 nb, st_img_conv CONST & options );
//	static	void	uyvy_to_rgba8_slow(				UINT8 CONST * RESTRICT src8, UINT8* dst8, INT32 nb, st_img_conv CONST & options );
//	static	void	uyvy_to_rgb8_other(				UINT8 CONST * RESTRICT src8, UINT8* dst8, INT32 nb, st_img_conv CONST & options );
//	static	void	uyvy_to_rgba8_incrust(			UINT8 CONST * RESTRICT src8, UINT8* dst8, INT32 nb, st_img_conv CONST & options );


	static void		v210_to_bgra8_slow(				UINT8 CONST  *RESTRICT src8, UINT8* RESTRICT dst8, INT32 nb, UINT8 CONST alpha );

	static	void	rgba8_incrust(					UINT8 * RESTRICT p8, INT32 nb, UINT8 CONST * CONST RESTRICT lut );
	static void		bgra8_incrust(					UINT8 * RESTRICT p8, INT32 nb, UINT8 CONST * CONST RESTRICT lut );
};

