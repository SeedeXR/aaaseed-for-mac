#include "img.h"
#include "image/convert/img_convert.h"
#include "image/convert/bitmap_convert.h"
#include "media/video/texture_flux_master.h"
#include "image/convert/img_convert_macros.h"

using namespace aaa;


//
// Dispatcher case-list helpers for the c_img_2d::copy_src_to_<dst>() switches below.
// Each expands to the full set of case labels for one src-family across the 4 precisions
// (* 2 swap variants for RGB / RGBA which have a B_SWAP_RB template parameter).
// Local names assumed by the expansion : src, src_pitch, dst, dst_pitch, options.
// FN is the imgcon template name (no imgcon:: prefix). T_DST is the destination element type.
//
// _ALPHA variants are for callees returning bool (b_alpha_done) ; the result is assigned to the
// b_alpha_done local declared by IMGCON_BEGIN_COPY. Plain variants are for callees returning void.
// Macros are #undef'd at the bottom of this TU (they are intentionally not exported).

// Common argument bundles forwarded to every imgcon converter callee in this TU.
// DISPATCH_ARGS_4 is the 4-tuple (src, src_pitch, dst, dst_pitch) ; useful when the callee takes
// extra middle or trailing args (ch_nb, size_y, flip flag, ...). DISPATCH_ARGS is the 5-tuple
// (DISPATCH_ARGS_4, options) used by the standard converter signature.
#define DISPATCH_ARGS_4		src, src_pitch, dst, dst_pitch
#define DISPATCH_ARGS		DISPATCH_ARGS_4, options

// DISPATCH_ONE : one case label that calls imgcon::FN<...template args...>( DISPATCH_ARGS ).
// PREFIX is empty (void callee) or "b_alpha_done =" (bool callee that drives the alpha-done local).
#define DISPATCH_ONE( FMT, PREFIX, FN, ... ) \
	case PIXEL_FORMAT::FMT:	PREFIX imgcon::FN<__VA_ARGS__>( DISPATCH_ARGS );	break

// DISPATCH_FOREACH_PRECISION_2 / _3 : walk the 4 precisions {8, 16, 16FP, 32FP} for one format prefix.
// _2 emits 2 template args (T_SRC, T_DST) ; _3 emits 3 template args (T_SRC, T_DST, SWAP).
// All per-format _CASES macros below are 4-line lists built on top of one of these.
#define DISPATCH_FOREACH_PRECISION_2( FN, T_DST, PREFIX, FMT_PFX ) \
	DISPATCH_ONE( FMT_PFX##_8,    PREFIX, FN, UINT8,  T_DST );	\
	DISPATCH_ONE( FMT_PFX##_16,   PREFIX, FN, UINT16, T_DST );	\
	DISPATCH_ONE( FMT_PFX##_16FP, PREFIX, FN, FP16,   T_DST );	\
	DISPATCH_ONE( FMT_PFX##_32FP, PREFIX, FN, FP32,   T_DST )

#define DISPATCH_FOREACH_PRECISION_3(FN, T_DST, PREFIX, FMT_PFX, SWAP ) \
	DISPATCH_ONE( FMT_PFX##_8,    PREFIX, FN, UINT8,  T_DST, SWAP );	\
	DISPATCH_ONE( FMT_PFX##_16,   PREFIX, FN, UINT16, T_DST, SWAP );	\
	DISPATCH_ONE( FMT_PFX##_16FP, PREFIX, FN, FP16,   T_DST, SWAP );	\
	DISPATCH_ONE( FMT_PFX##_32FP, PREFIX, FN, FP32,   T_DST, SWAP )

// Per-src-family case-list macros, called directly at the dispatcher call sites.
// PREFIX is empty when the callee returns void ; pass "b_alpha_done =" when the callee returns bool
// so the alpha-done local declared by IMGCON_BEGIN_COPY stays visible at the call site.
#define DISPATCH_R_CASES(    FN, T_DST, PREFIX )	DISPATCH_FOREACH_PRECISION_2( FN, T_DST, PREFIX, R	   )
#define DISPATCH_RG_CASES(   FN, T_DST, PREFIX )	DISPATCH_FOREACH_PRECISION_2( FN, T_DST, PREFIX, RG	   )
#define DISPATCH_RGB_CASES(  FN, T_DST, PREFIX )	DISPATCH_FOREACH_PRECISION_3( FN, T_DST, PREFIX, RGB,  false	)
#define DISPATCH_BGR_CASES(  FN, T_DST, PREFIX )	DISPATCH_FOREACH_PRECISION_3( FN, T_DST, PREFIX, BGR,  true		)
#define DISPATCH_RGBA_CASES( FN, T_DST, PREFIX )	DISPATCH_FOREACH_PRECISION_3( FN, T_DST, PREFIX, RGBA, false	)
#define DISPATCH_BGRA_CASES( FN, T_DST, PREFIX )	DISPATCH_FOREACH_PRECISION_3( FN, T_DST, PREFIX, BGRA, true		)

bool	c_img_2d::copy_src_to_rgba16fp( UINT8 CONST * RESTRICT src, INT32 src_pitch, PIXEL_FORMAT CONST src_format, st_img_conv& options )
{
	IMGCON_BEGIN_COPY( PIXEL_FORMAT::RGBA_16FP )
		auto dst = get_data_fp16();
		imgcon::adjust_src_dst( size_y, DISPATCH_ARGS );
		switch( src_format )
		{
		DISPATCH_R_CASES   ( r_to_rgba,   FP16, b_alpha_done = );
		DISPATCH_RG_CASES  ( rg_to_rgba,  FP16, b_alpha_done = );
		DISPATCH_RGB_CASES ( rgb_to_rgba, FP16, b_alpha_done = );
		DISPATCH_BGR_CASES ( rgb_to_rgba, FP16, b_alpha_done = );
		// RGBA / BGRA srcs use precision-specific fast paths (SIMD + same-format alpha override).
		case PIXEL_FORMAT::BGRA_8:		options.b_swap_red_blue = !options.b_swap_red_blue;
		case PIXEL_FORMAT::RGBA_8:		b_alpha_done = imgcon::rgba8_to_rgba16fp(		DISPATCH_ARGS );		break;
		case PIXEL_FORMAT::RGBA_16:		imgcon::uint16_to_fp16(							DISPATCH_ARGS_4, 4, options );	break;
		case PIXEL_FORMAT::BGRA_16:		imgcon::rgba_to_rgba<UINT16, FP16, true>(		DISPATCH_ARGS );		break;
		case PIXEL_FORMAT::BGRA_16FP:	options.b_swap_red_blue = !options.b_swap_red_blue;
		case PIXEL_FORMAT::RGBA_16FP:	b_alpha_done = imgcon::rgba16fp_to_rgba16fp(	DISPATCH_ARGS );		break;
		case PIXEL_FORMAT::BGRA_32FP:	options.b_swap_red_blue = !options.b_swap_red_blue;
		case PIXEL_FORMAT::RGBA_32FP:	b_alpha_done = imgcon::rgba32fp_to_rgba16fp(	DISPATCH_ARGS );		break;
		default:
			b_done = false;
			break;
		}
	IMGCON_END_COPY_ALPHA()
}

bool	c_img_2d::copy_src_to_rgb16fp( UINT8 CONST * RESTRICT src, INT32 src_pitch, PIXEL_FORMAT CONST src_format, st_img_conv& options )
{
	IMGCON_BEGIN_COPY( PIXEL_FORMAT::RGB_16FP )
		auto dst = get_data_fp16();
		imgcon::adjust_src_dst( size_y, DISPATCH_ARGS );
		switch( src_format )
		{
		DISPATCH_R_CASES   ( r_to_rgb,   FP16, );
		DISPATCH_RG_CASES  ( rg_to_rgb,  FP16, );
		DISPATCH_RGBA_CASES( rgba_to_rgb, FP16, );
		DISPATCH_BGRA_CASES( rgba_to_rgb, FP16, );
		// RGB / BGR srcs use precision-specific fast paths (uint8_to_fp16, memcpy, fp32_to_fp16, bgr32fp_to_rgb16fp).
		case PIXEL_FORMAT::RGB_8:		imgcon::uint8_to_fp16(					DISPATCH_ARGS_4, 3, options );	break;
		case PIXEL_FORMAT::BGR_8:		imgcon::rgb_to_rgb<UINT8,  FP16, true>(	DISPATCH_ARGS );				break;
		case PIXEL_FORMAT::RGB_16:		imgcon::uint16_to_fp16(					DISPATCH_ARGS_4, 3, options );	break;
		case PIXEL_FORMAT::BGR_16:		imgcon::rgb_to_rgb<UINT16, FP16, true>(	DISPATCH_ARGS );				break;
		case PIXEL_FORMAT::RGB_16FP:	imgcon::memcpy_image(					DISPATCH_ARGS_4, size_y );		break;
		case PIXEL_FORMAT::BGR_16FP:	imgcon::rgb_to_rgb<FP16,   FP16, true>(	DISPATCH_ARGS );				break;
		case PIXEL_FORMAT::RGB_32FP:	imgcon::fp32_to_fp16(					DISPATCH_ARGS_4, 3, options );	break;
		case PIXEL_FORMAT::BGR_32FP:	imgcon::bgr32fp_to_rgb16fp(				DISPATCH_ARGS );				break;
		default:
			b_done = false;
			break;
		}
	IMGCON_END_COPY()
}

bool	c_img_2d::copy_src_to_rg16fp( UINT8 CONST * RESTRICT src, INT32 src_pitch, PIXEL_FORMAT CONST src_format, st_img_conv& options )
{
	IMGCON_BEGIN_COPY( PIXEL_FORMAT::RG_16FP )
		auto dst = get_data_fp16();
		imgcon::adjust_src_dst( size_y, DISPATCH_ARGS );
		switch( src_format )
		{
		case PIXEL_FORMAT::RG_8:		imgcon::uint8_to_fp16(	DISPATCH_ARGS_4, 2, options );	break;
		case PIXEL_FORMAT::RG_16:		imgcon::uint16_to_fp16(	DISPATCH_ARGS_4, 2, options );	break;
		case PIXEL_FORMAT::RG_16FP:		imgcon::memcpy_image(	DISPATCH_ARGS_4, size_y );		break;
		case PIXEL_FORMAT::RG_32FP:		imgcon::fp32_to_fp16(	DISPATCH_ARGS_4, 2, options );	break;
		DISPATCH_R_CASES   ( r_to_rg,    FP16, );
		DISPATCH_RGB_CASES ( rgb_to_rg,  FP16, );
		DISPATCH_BGR_CASES ( rgb_to_rg,  FP16, );
		DISPATCH_RGBA_CASES( rgba_to_rg, FP16, );
		DISPATCH_BGRA_CASES( rgba_to_rg, FP16, );
		default:
			b_done = false;
			break;
		}
	IMGCON_END_COPY()
}

bool	c_img_2d::copy_src_to_r16fp( UINT8 CONST * RESTRICT src, INT32 src_pitch, PIXEL_FORMAT CONST src_format, st_img_conv& options )
{
	IMGCON_BEGIN_COPY( PIXEL_FORMAT::R_16FP )
		auto dst = get_data_fp16();
		imgcon::adjust_src_dst( size_y, DISPATCH_ARGS );
		switch( src_format )
		{
		case PIXEL_FORMAT::R_8:			imgcon::uint8_to_fp16(	DISPATCH_ARGS_4, 1, options );	break;
		case PIXEL_FORMAT::R_16:		imgcon::uint16_to_fp16(	DISPATCH_ARGS_4, 1, options );	break;
		case PIXEL_FORMAT::DEPTH_16:
		case PIXEL_FORMAT::R_16FP:		imgcon::memcpy_image(	DISPATCH_ARGS_4, size_y );		break;
		case PIXEL_FORMAT::DEPTH_32:
		case PIXEL_FORMAT::R_32FP:		imgcon::fp32_to_fp16(	DISPATCH_ARGS_4, 1, options );	break;
		DISPATCH_RG_CASES  ( rg_to_r,   FP16, );
		DISPATCH_RGB_CASES ( rgb_to_r,  FP16, );
		DISPATCH_BGR_CASES ( rgb_to_r,  FP16, );
		DISPATCH_RGBA_CASES( rgba_to_r, FP16, );
		DISPATCH_BGRA_CASES( rgba_to_r, FP16, );
		default:
			b_done = false;
			break;
		}
	IMGCON_END_COPY()
}


bool	c_img_2d::copy_src_to_rgba32fp( UINT8 CONST * RESTRICT src, INT32 src_pitch, PIXEL_FORMAT CONST src_format, st_img_conv& options )
{
	IMGCON_BEGIN_COPY( PIXEL_FORMAT::RGBA_32FP )
		auto dst = get_data_fp32();
		imgcon::adjust_src_dst( size_y, DISPATCH_ARGS );
		switch( src_format )
		{
		DISPATCH_R_CASES   ( r_to_rgba,   FP32, b_alpha_done = );
		DISPATCH_RG_CASES  ( rg_to_rgba,  FP32, b_alpha_done = );
		DISPATCH_RGB_CASES ( rgb_to_rgba, FP32, b_alpha_done = );
		DISPATCH_BGR_CASES ( rgb_to_rgba, FP32, b_alpha_done = );
		// RGBA / BGRA srcs use precision-specific fast paths (SIMD + same-format alpha override).
		case PIXEL_FORMAT::BGRA_8:		options.b_swap_red_blue = !options.b_swap_red_blue;
		case PIXEL_FORMAT::RGBA_8:		b_alpha_done = imgcon::rgba8_to_rgba32fp(		DISPATCH_ARGS );		break;
		case PIXEL_FORMAT::RGBA_16:		imgcon::uint16_to_fp32(							DISPATCH_ARGS_4, 4, options );	break;
		case PIXEL_FORMAT::BGRA_16:		imgcon::rgba_to_rgba<UINT16, FP32, true>(		DISPATCH_ARGS );		break;
		case PIXEL_FORMAT::BGRA_32FP:	options.b_swap_red_blue = !options.b_swap_red_blue;
		case PIXEL_FORMAT::RGBA_32FP:	b_alpha_done = imgcon::rgba32fp_to_rgba32fp(	DISPATCH_ARGS );		break;
		case PIXEL_FORMAT::BGRA_16FP:	options.b_swap_red_blue = !options.b_swap_red_blue;
		case PIXEL_FORMAT::RGBA_16FP:	b_alpha_done = imgcon::rgba16fp_to_rgba32fp(	DISPATCH_ARGS );		break;
		default:
			b_done = false;
			break;
		}
	IMGCON_END_COPY_ALPHA()
}

bool	c_img_2d::copy_src_to_rgb32fp( UINT8 CONST * RESTRICT src, INT32 src_pitch, PIXEL_FORMAT CONST src_format, st_img_conv& options )
{
	IMGCON_BEGIN_COPY( PIXEL_FORMAT::RGB_32FP )
		auto dst = get_data_fp32();
		imgcon::adjust_src_dst( size_y, DISPATCH_ARGS );
		switch( src_format )
		{
		DISPATCH_R_CASES   ( r_to_rgb,    FP32, );
		DISPATCH_RG_CASES  ( rg_to_rgb,   FP32, );
		DISPATCH_RGBA_CASES( rgba_to_rgb, FP32, );
		DISPATCH_BGRA_CASES( rgba_to_rgb, FP32, );
		// RGB / BGR srcs use precision-specific fast paths (uint8/16_to_fp32, memcpy, dedicated bgr_to_rgb FP32 helpers).
		case PIXEL_FORMAT::RGB_8:		imgcon::uint8_to_fp32(					DISPATCH_ARGS_4, 3, options );	break;
		case PIXEL_FORMAT::BGR_8:		imgcon::bgr8_to_rgb32fp(				DISPATCH_ARGS );				break;
		case PIXEL_FORMAT::RGB_16:		imgcon::uint16_to_fp32(					DISPATCH_ARGS_4, 3, options );	break;
		case PIXEL_FORMAT::BGR_16:		imgcon::rgb_to_rgb<UINT16, FP32, true>(	DISPATCH_ARGS );				break;
		case PIXEL_FORMAT::RGB_16FP:	imgcon::fp16_to_fp32(					DISPATCH_ARGS_4, 3, options );	break;
		case PIXEL_FORMAT::BGR_16FP:	imgcon::bgr16fp_to_rgb32fp(				DISPATCH_ARGS );				break;
		case PIXEL_FORMAT::RGB_32FP:	imgcon::memcpy_image(					DISPATCH_ARGS_4, size_y );		break;
		case PIXEL_FORMAT::BGR_32FP:	imgcon::rgb_to_rgb<FP32,   FP32, true>(	DISPATCH_ARGS );				break;
		default:
			b_done = false;
			break;
		}
	IMGCON_END_COPY()
}

bool	c_img_2d::copy_src_to_r32fp( UINT8 CONST * RESTRICT src, INT32 src_pitch, PIXEL_FORMAT CONST src_format, st_img_conv& options )
{
	IMGCON_BEGIN_COPY( PIXEL_FORMAT::R_32FP )
		auto * dst = get_data_fp32();
		imgcon::adjust_src_dst( size_y, DISPATCH_ARGS );
		switch( src_format )
		{
		case PIXEL_FORMAT::R_8:		imgcon::uint8_to_fp32(	DISPATCH_ARGS_4, 1, options );	break;
		case PIXEL_FORMAT::R_16:	imgcon::uint16_to_fp32(	DISPATCH_ARGS_4, 1, options );	break;
		case PIXEL_FORMAT::DEPTH_16:
		case PIXEL_FORMAT::R_16FP:	imgcon::fp16_to_fp32(	DISPATCH_ARGS_4, 1, options );	break;
		case PIXEL_FORMAT::DEPTH_32:
		case PIXEL_FORMAT::R_32FP:	imgcon::memcpy_image(	DISPATCH_ARGS_4, size_y );		break;
		DISPATCH_RG_CASES  ( rg_to_r,   FP32, );
		DISPATCH_RGB_CASES ( rgb_to_r,  FP32, );
		DISPATCH_BGR_CASES ( rgb_to_r,  FP32, );
		DISPATCH_RGBA_CASES( rgba_to_r, FP32, );
		DISPATCH_BGRA_CASES( rgba_to_r, FP32, );
		default:
			b_done = false;
			break;
		}
	IMGCON_END_COPY()
}

bool	c_img_2d::copy_src_to_rg32fp( UINT8 CONST * RESTRICT src, INT32 src_pitch, PIXEL_FORMAT CONST src_format, st_img_conv& options )
{
	IMGCON_BEGIN_COPY( PIXEL_FORMAT::RG_32FP )
		auto dst = get_data_fp32();
		imgcon::adjust_src_dst( size_y, DISPATCH_ARGS );
		switch( src_format )
		{
		case PIXEL_FORMAT::RG_8:		imgcon::uint8_to_fp32(	DISPATCH_ARGS_4, 2, options );	break;
		case PIXEL_FORMAT::RG_16:		imgcon::uint16_to_fp32(	DISPATCH_ARGS_4, 2, options );	break;
		case PIXEL_FORMAT::RG_16FP:		imgcon::fp16_to_fp32(	DISPATCH_ARGS_4, 2, options );	break;
		case PIXEL_FORMAT::RG_32FP:		imgcon::memcpy_image(	DISPATCH_ARGS_4, size_y );		break;
		DISPATCH_R_CASES   ( r_to_rg,    FP32, );
		DISPATCH_RGB_CASES ( rgb_to_rg,  FP32, );
		DISPATCH_BGR_CASES ( rgb_to_rg,  FP32, );
		DISPATCH_RGBA_CASES( rgba_to_rg, FP32, );
		DISPATCH_BGRA_CASES( rgba_to_rg, FP32, );
		default:
			b_done = false;
			break;
		}
	IMGCON_END_COPY()
}

bool	c_img_2d::copy_src_to_rgba8( UINT8 CONST * src, INT32 src_pitch, PIXEL_FORMAT CONST src_format, st_img_conv& options )
{
	IMGCON_BEGIN_COPY( PIXEL_FORMAT::RGBA_8 )
		auto dst = get_data_uint8();
		imgcon::adjust_src_dst( size_y, DISPATCH_ARGS );
		switch( src_format )
		{
		//we should deal with this format and rgb vs bgr
		case PIXEL_FORMAT::UYVY:
		case PIXEL_FORMAT::HDYC:
		case PIXEL_FORMAT::YVYU:		b_alpha_done = imgcon::yuyv_to_rgba8(	DISPATCH_ARGS, true );		break;
		case PIXEL_FORMAT::YUY2:		b_alpha_done = imgcon::yuyv_to_rgba8(	DISPATCH_ARGS, false );		break;
		case PIXEL_FORMAT::I420:		b_alpha_done = imgcon::i420_to_rgba8(	DISPATCH_ARGS );			break;
		case PIXEL_FORMAT::NV12:		b_alpha_done = imgcon::nv12_to_rgba8(	DISPATCH_ARGS );			break;
		case PIXEL_FORMAT::V210:		b_alpha_done = imgcon::v210_to_bgra8(	DISPATCH_ARGS );			break;

		// R_8 / R_16 / DISP_16 srcs use SIMD fast paths ; R_16FP / R_32FP go through the r_to_rgba template.
		// Full DISPATCH_R_CASES would re-introduce duplicate R_8 / R_16 labels, so R stays enumerated by hand.
		case PIXEL_FORMAT::R_8:			b_alpha_done = imgcon::r8_to_rgba8(				DISPATCH_ARGS );		break;
		case PIXEL_FORMAT::DISP_16:		b_alpha_done = imgcon::disp16_to_rgba8(			DISPATCH_ARGS );		break;
		case PIXEL_FORMAT::R_16:		b_alpha_done = imgcon::r16_to_rgba8(			DISPATCH_ARGS );		break;
		case PIXEL_FORMAT::R_16FP:		b_alpha_done = imgcon::r_to_rgba<FP16, UINT8>(	DISPATCH_ARGS );		break;
		case PIXEL_FORMAT::R_32FP:		b_alpha_done = imgcon::r_to_rgba<FP32, UINT8>(	DISPATCH_ARGS );		break;
		DISPATCH_RG_CASES( rg_to_rgba, UINT8, b_alpha_done = );
		// RGBA / BGRA srcs use precision-specific fast paths (SIMD + same-format alpha override).
		case PIXEL_FORMAT::RGBA_16:		imgcon::uint16_to_uint8(					DISPATCH_ARGS_4, 4, options );	break;
		case PIXEL_FORMAT::BGRA_16:		imgcon::rgba_to_rgba<UINT16, UINT8, true>(	DISPATCH_ARGS );				break;
		case PIXEL_FORMAT::BGRA_8:		options.b_swap_red_blue = !options.b_swap_red_blue;
		case PIXEL_FORMAT::RGBA_8:		b_alpha_done = imgcon::rgba8_to_rgba8(		DISPATCH_ARGS );		break;
		case PIXEL_FORMAT::BGRA_16FP:	options.b_swap_red_blue = !options.b_swap_red_blue;
		case PIXEL_FORMAT::RGBA_16FP:	b_alpha_done = imgcon::rgba16fp_to_rgba8(	DISPATCH_ARGS );		break;
		case PIXEL_FORMAT::BGRA_32FP:	options.b_swap_red_blue = !options.b_swap_red_blue;
		case PIXEL_FORMAT::RGBA_32FP:	b_alpha_done = imgcon::rgba32fp_to_rgba8(	DISPATCH_ARGS );		break;
		// RGB_8 / BGR_8 use SIMD fast path (rgb8_to_rgba8) ; 16+ precisions go through the rgb_to_rgba template.
		case PIXEL_FORMAT::BGR_8:		options.b_swap_red_blue = !options.b_swap_red_blue;
		case PIXEL_FORMAT::RGB_8:		b_alpha_done = imgcon::rgb8_to_rgba8(						DISPATCH_ARGS );	break;
		case PIXEL_FORMAT::RGB_16:		b_alpha_done = imgcon::rgb_to_rgba<UINT16, UINT8, false>(	DISPATCH_ARGS );	break;
		case PIXEL_FORMAT::BGR_16:		b_alpha_done = imgcon::rgb_to_rgba<UINT16, UINT8, true >(	DISPATCH_ARGS );	break;
		case PIXEL_FORMAT::RGB_16FP:	b_alpha_done = imgcon::rgb_to_rgba<FP16,   UINT8, false>(	DISPATCH_ARGS );	break;
		case PIXEL_FORMAT::BGR_16FP:	b_alpha_done = imgcon::rgb_to_rgba<FP16,   UINT8, true >(	DISPATCH_ARGS );	break;
		case PIXEL_FORMAT::RGB_32FP:	b_alpha_done = imgcon::rgb_to_rgba<FP32,   UINT8, false>(	DISPATCH_ARGS );	break;
		case PIXEL_FORMAT::BGR_32FP:	b_alpha_done = imgcon::rgb_to_rgba<FP32,   UINT8, true >(	DISPATCH_ARGS );	break;
		default:
			b_done = false;
			break;
		}
	IMGCON_END_COPY_ALPHA()
}

//todoopt move // stuff in convert fp fns
bool	c_img_2d::copy_src_to_r8( UINT8 CONST * src, INT32 src_pitch, PIXEL_FORMAT CONST src_format, st_img_conv& options )
{
	IMGCON_BEGIN_COPY( PIXEL_FORMAT::R_8  )

	auto dst	= get_data_uint8();
	imgcon::adjust_src_dst( size_y, DISPATCH_ARGS );

	switch( src_format )
	{
	case PIXEL_FORMAT::I420:
	case PIXEL_FORMAT::NV12:
		src_pitch = size_x;
	case PIXEL_FORMAT::R_8:
		if( options.lut )
			imgcon::call_by_line_striped( size_y, [&](INT32 j) NOEXCEPT {
				bitcon::r8_to_r8_lut( (UINT8*)src + (j * src_pitch), (UINT8*)dst + (j * dst_pitch), size_x, options.lut ); });
		else
			imgcon::memcpy_image( DISPATCH_ARGS_4, size_y );
		break;
	case PIXEL_FORMAT::BGRA_8:	options.b_swap_red_blue = !options.b_swap_red_blue;
	case PIXEL_FORMAT::RGBA_8:
		imgcon::rgba8_to_r8( DISPATCH_ARGS );
		//The original RGB to Grayscale formula is given as Y = 0.299 R + 0.587 G + 0.114 B
		// normal conversion equation is:
		//	Y = 0.212671 * R + 0.715160 * G + 0.072169 * B;
		//	Y = (54 * R + 183 * G + 19 * B)/256;
		// SC08
		//for( INT32 i = size_y; i > 0; --i )	//todo opt put this in the loop
		//{
		//	bitcon::rgba8_to_r8_fast(src, dst, size_x );
		//	src += src_step;
		//	dst += dst_step;
		//}
		break;
	case PIXEL_FORMAT::BGR_8:	options.b_swap_red_blue = !options.b_swap_red_blue;
	case PIXEL_FORMAT::RGB_8:
		if( texture_flux_master->is_convert_to_rgb_fast() )
			imgcon::rgba8_to_r8_fast( DISPATCH_ARGS_4, 3, options );
		else
			imgcon::call_by_line_striped( size_y, [&](INT32 j) NOEXCEPT {
				bitcon::rgb8_to_r8_fast( (UINT8 CONST *)src + (j * src_pitch), (UINT8*)dst + (j * dst_pitch), size_x ); });
		break;
	case PIXEL_FORMAT::YUY2:
	case PIXEL_FORMAT::YVYU:
	case PIXEL_FORMAT::HDYC:
		if( options.lut )
			imgcon::call_by_line_striped( size_y, [&](INT32 j) NOEXCEPT {
				bitcon::yuyv_to_r8_lut( (UINT8 CONST *)src + (j * src_pitch), (UINT8*)dst + (j * dst_pitch), size_x,  options.lut ); });
		else
			imgcon::call_by_line_striped( size_y, [&](INT32 j) NOEXCEPT {
				bitcon::yuyv_to_r8_fast( (UINT8 CONST *)src + (j * src_pitch), (UINT8*)dst + (j * dst_pitch), size_x ); });
		break;
	case PIXEL_FORMAT::BINARY:
		imgcon::binary_to_r8(		DISPATCH_ARGS );
		break;
	case PIXEL_FORMAT::R_16:
		imgcon::uint16_to_uint8(	DISPATCH_ARGS_4, 1, options );
		break;
	case PIXEL_FORMAT::DEPTH_16:
	case PIXEL_FORMAT::R_16FP:
		imgcon::fp16_to_uint8(		DISPATCH_ARGS_4, 1, options );
		break;
	case PIXEL_FORMAT::DEPTH_32:
	case PIXEL_FORMAT::R_32FP:
		imgcon::fp32_to_uint8(		DISPATCH_ARGS_4, 1, options );
		break;
	// RGB_8 / BGR_8 / RGBA_8 / BGRA_8 srcs handled higher up via SIMD fast paths (rgba8_to_r8, rgb8_to_r8_fast, ...).
	// The 16-bit + FP precisions below go through the rgb_to_r / rgba_to_r templates ; full DISPATCH_*_CASES would
	// re-introduce duplicate RGB_8 / BGR_8 / RGBA_8 / BGRA_8 labels, so the 16+ precisions stay enumerated by hand.
	case PIXEL_FORMAT::RGB_16:		imgcon::rgb_to_r<UINT16, UINT8, false>(	DISPATCH_ARGS );		break;
	case PIXEL_FORMAT::BGR_16:		imgcon::rgb_to_r<UINT16, UINT8, true >(	DISPATCH_ARGS );		break;
	case PIXEL_FORMAT::RGB_16FP:	imgcon::rgb_to_r<FP16,   UINT8, false>(	DISPATCH_ARGS );		break;
	case PIXEL_FORMAT::BGR_16FP:	imgcon::rgb_to_r<FP16,   UINT8, true >(	DISPATCH_ARGS );		break;
	case PIXEL_FORMAT::RGB_32FP:	imgcon::rgb_to_r<FP32,   UINT8, false>(	DISPATCH_ARGS );		break;
	case PIXEL_FORMAT::BGR_32FP:	imgcon::rgb_to_r<FP32,   UINT8, true >(	DISPATCH_ARGS );		break;

	case PIXEL_FORMAT::RGBA_16:		imgcon::rgba_to_r<UINT16, UINT8, false>(DISPATCH_ARGS );		break;
	case PIXEL_FORMAT::BGRA_16:		imgcon::rgba_to_r<UINT16, UINT8, true >(DISPATCH_ARGS );		break;
	case PIXEL_FORMAT::RGBA_16FP:	imgcon::rgba_to_r<FP16,   UINT8, false>(DISPATCH_ARGS );		break;
	case PIXEL_FORMAT::BGRA_16FP:	imgcon::rgba_to_r<FP16,   UINT8, true >(DISPATCH_ARGS );		break;
	case PIXEL_FORMAT::RGBA_32FP:	imgcon::rgba_to_r<FP32,   UINT8, false>(DISPATCH_ARGS );		break;
	case PIXEL_FORMAT::BGRA_32FP:	imgcon::rgba_to_r<FP32,   UINT8, true >(DISPATCH_ARGS );		break;
	DISPATCH_RG_CASES( rg_to_r, UINT8, );
	default:
		b_done = false;
		break;
	}

	IMGCON_END_COPY()
}

bool	c_img_2d::copy_src_to_rg8( UINT8 CONST * src, INT32 src_pitch, PIXEL_FORMAT CONST src_format, st_img_conv& options )
{
	IMGCON_BEGIN_COPY( PIXEL_FORMAT::RG_8 )
		auto dst	= get_data_uint8();
		imgcon::adjust_src_dst( size_y, DISPATCH_ARGS );
		if( options.lut )
			ERR_PRINT_STRING( "%s(): AAASeed don't know yet how to move frame from src to Red Green 8 with lut, doing with no Lut", __FUNCTION__ );
		switch( src_format )
		{
		case PIXEL_FORMAT::RG_8:		imgcon::memcpy_image(			DISPATCH_ARGS_4, size_y );		break;
		case PIXEL_FORMAT::RG_16:		imgcon::uint16_to_uint8(		DISPATCH_ARGS_4, 2, options );	break;
		case PIXEL_FORMAT::RG_16FP:		imgcon::fp16_to_uint8(			DISPATCH_ARGS_4, 2, options );	break;
		case PIXEL_FORMAT::RG_32FP:		imgcon::fp32_to_uint8(			DISPATCH_ARGS_4, 2, options );	break;
		DISPATCH_R_CASES   ( r_to_rg,    UINT8, );
		DISPATCH_RGB_CASES ( rgb_to_rg,  UINT8, );
		DISPATCH_BGR_CASES ( rgb_to_rg,  UINT8, );
		DISPATCH_RGBA_CASES( rgba_to_rg, UINT8, );
		DISPATCH_BGRA_CASES( rgba_to_rg, UINT8, );
		default:
			b_done = false;
			break;
		}
	IMGCON_END_COPY()
}

bool	c_img_2d::copy_src_to_r16( UINT8 CONST * src, INT32 src_pitch, PIXEL_FORMAT CONST src_format, st_img_conv& options )
{
	IMGCON_BEGIN_COPY( PIXEL_FORMAT::R_16 )
		auto dst	= get_data_uint16();
		imgcon::adjust_src_dst( size_y, DISPATCH_ARGS );
		if( options.lut )
			ERR_PRINT_STRING( "%s(): AAASeed don't know yet how to move frame from src to Red 16 with lut, doing with no Lut", __FUNCTION__ );
		switch( src_format )
		{
		case PIXEL_FORMAT::R_8:		imgcon::uint8_to_uint16(	DISPATCH_ARGS_4, 1, options );	break;
		case PIXEL_FORMAT::R_16:	imgcon::memcpy_image(		DISPATCH_ARGS_4, size_y );		break;
		case PIXEL_FORMAT::R_16FP:	imgcon::fp16_to_uint16(		DISPATCH_ARGS_4, 1, options );	break;
		case PIXEL_FORMAT::R_32FP:	imgcon::fp32_to_uint16(		DISPATCH_ARGS_4, 1, options );	break;
		DISPATCH_RG_CASES  ( rg_to_r,   UINT16, );
		DISPATCH_RGB_CASES ( rgb_to_r,  UINT16, );
		DISPATCH_BGR_CASES ( rgb_to_r,  UINT16, );
		DISPATCH_RGBA_CASES( rgba_to_r, UINT16, );
		DISPATCH_BGRA_CASES( rgba_to_r, UINT16, );
		default:
			debug_break( "Unsupported source format in %s", __FUNCTION__ );
			b_done = false;
			break;
		}
	IMGCON_END_COPY()
}

bool	c_img_2d::copy_src_to_rg16( UINT8 CONST * src, INT32 src_pitch, PIXEL_FORMAT CONST src_format, st_img_conv& options )
{
	IMGCON_BEGIN_COPY( PIXEL_FORMAT::RG_16 )
		auto dst	= get_data_uint16();
		imgcon::adjust_src_dst( size_y, DISPATCH_ARGS );
		if( options.lut )
			ERR_PRINT_STRING( "%s(): AAASeed don't know yet how to move frame from src to red Green 16 with lut, doing with no Lut", __FUNCTION__ );
		switch( src_format )
		{
		case PIXEL_FORMAT::RG_8:		imgcon::uint8_to_uint16(	DISPATCH_ARGS_4, 2, options );	break;
		case PIXEL_FORMAT::RG_16:		imgcon::memcpy_image(		DISPATCH_ARGS_4, size_y );		break;
		case PIXEL_FORMAT::RG_16FP:		imgcon::fp16_to_uint16(		DISPATCH_ARGS_4, 2, options );	break;
		case PIXEL_FORMAT::RG_32FP:		imgcon::fp32_to_uint16(		DISPATCH_ARGS_4, 2, options );	break;
		DISPATCH_R_CASES   ( r_to_rg,    UINT16, );
		DISPATCH_RGB_CASES ( rgb_to_rg,  UINT16, );
		DISPATCH_BGR_CASES ( rgb_to_rg,  UINT16, );
		DISPATCH_RGBA_CASES( rgba_to_rg, UINT16, );
		DISPATCH_BGRA_CASES( rgba_to_rg, UINT16, );
		default:
			debug_break( "Unsupported source format in %s", __FUNCTION__ );
			b_done = false;
			break;
		}
	IMGCON_END_COPY()
}

bool	c_img_2d::copy_src_to_rgb16( UINT8 CONST * src, INT32 src_pitch, PIXEL_FORMAT CONST src_format, st_img_conv& options )
{
	IMGCON_BEGIN_COPY( PIXEL_FORMAT::RGB_16 )
		auto dst	= get_data_uint16();
		imgcon::adjust_src_dst( size_y, DISPATCH_ARGS );
		if( options.lut )
			ERR_PRINT_STRING( "%s(): AAASeed don't know yet how to move frame from src to rgba 16 with lut, doing with no Lut", __FUNCTION__ );
		switch( src_format )
		{
		DISPATCH_R_CASES   ( r_to_rgb,    UINT16, );
		DISPATCH_RG_CASES  ( rg_to_rgb,   UINT16, );
		DISPATCH_BGR_CASES ( rgb_to_rgb,  UINT16, );
		DISPATCH_RGBA_CASES( rgba_to_rgb, UINT16, );
		DISPATCH_BGRA_CASES( rgba_to_rgb, UINT16, );
		// RGB srcs : RGB_8 / RGB_16 use fast paths (uint8_to_uint16, memcpy on same format) ; 16FP / 32FP go via rgb_to_rgb template.
		case PIXEL_FORMAT::RGB_8:	imgcon::uint8_to_uint16(					DISPATCH_ARGS_4, 3, options );	break;
		case PIXEL_FORMAT::RGB_16:	imgcon::memcpy_image(						DISPATCH_ARGS_4, size_y );		break;
		case PIXEL_FORMAT::RGB_16FP:imgcon::rgb_to_rgb<FP16,   UINT16, false>(	DISPATCH_ARGS );				break;
		case PIXEL_FORMAT::RGB_32FP:imgcon::rgb_to_rgb<FP32,   UINT16, false>(	DISPATCH_ARGS );				break;
		default:
			debug_break( "Unsupported source format in %s", __FUNCTION__ );
			b_done = false;
			break;
		}
	IMGCON_END_COPY()
}

bool	c_img_2d::copy_src_to_rgba16( UINT8 CONST * src, INT32 src_pitch, PIXEL_FORMAT CONST src_format, st_img_conv& options )
{
	IMGCON_BEGIN_COPY( PIXEL_FORMAT::RGBA_16 )
		auto dst	= get_data_uint16();
		imgcon::adjust_src_dst( size_y, DISPATCH_ARGS );
		if( options.lut )
			ERR_PRINT_STRING( "%s(): AAASeed don't know yet how to move frame from src to rgba 16 with lut, doing with no Lut", __FUNCTION__ );
		switch( src_format )
		{
		DISPATCH_R_CASES   ( r_to_rgba,   UINT16, b_alpha_done = );
		DISPATCH_RG_CASES  ( rg_to_rgba,  UINT16, b_alpha_done = );
		DISPATCH_RGB_CASES ( rgb_to_rgba, UINT16, b_alpha_done = );
		DISPATCH_BGR_CASES ( rgb_to_rgba, UINT16, b_alpha_done = );
		DISPATCH_BGRA_CASES( rgba_to_rgba, UINT16, );
		// RGBA srcs : RGBA_8 / RGBA_16 use fast paths (uint8_to_uint16, memcpy on same format) ; 16FP / 32FP go via rgba_to_rgba template.
		case PIXEL_FORMAT::RGBA_8:		imgcon::uint8_to_uint16(					DISPATCH_ARGS_4, 4, options );	break;
		case PIXEL_FORMAT::RGBA_16:		imgcon::memcpy_image(						DISPATCH_ARGS_4, size_y );		break;
		case PIXEL_FORMAT::RGBA_16FP:	imgcon::rgba_to_rgba<FP16,   UINT16, false>(DISPATCH_ARGS );				break;
		case PIXEL_FORMAT::RGBA_32FP:	imgcon::rgba_to_rgba<FP32,   UINT16, false>(DISPATCH_ARGS );				break;
		default:
			debug_break( "Unsupported source format in %s", __FUNCTION__ );
			b_done = false;
			break;
		}
	IMGCON_END_COPY()
}

bool	c_img_2d::copy_src_to_rgb8( UINT8 CONST * src, INT32 src_pitch, PIXEL_FORMAT CONST src_format, st_img_conv& options )
{
	IMGCON_BEGIN_COPY( PIXEL_FORMAT::RGB_8 )

		auto dst	= get_data_uint8();
		imgcon::adjust_src_dst( size_y, DISPATCH_ARGS );

		switch( src_format )
		{
		// RGBA / BGRA srcs : 16+ precisions via rgba_to_rgb template ; *_8 use SSE3 fast path when available
		// (would clash with DISPATCH_RGBA_CASES which also emits RGBA_8 / BGRA_8).
		case PIXEL_FORMAT::BGRA_32FP:	imgcon::rgba_to_rgb<FP32,   UINT8, true >(	DISPATCH_ARGS );	break;
		case PIXEL_FORMAT::RGBA_32FP:	imgcon::rgba_to_rgb<FP32,   UINT8, false>(	DISPATCH_ARGS );	break;
		case PIXEL_FORMAT::BGRA_16FP:	imgcon::rgba_to_rgb<FP16,   UINT8, true >(	DISPATCH_ARGS );	break;
		case PIXEL_FORMAT::RGBA_16FP:	imgcon::rgba_to_rgb<FP16,   UINT8, false>(	DISPATCH_ARGS );	break;
		case PIXEL_FORMAT::BGRA_16:		imgcon::rgba_to_rgb<UINT16, UINT8, true >(	DISPATCH_ARGS );	break;
		case PIXEL_FORMAT::RGBA_16:		imgcon::rgba_to_rgb<UINT16, UINT8, false>(	DISPATCH_ARGS );	break;
		case PIXEL_FORMAT::BGRA_8:
			options.b_swap_red_blue = !options.b_swap_red_blue;
			if( texture_flux_master->is_convert_to_rgb_sse3() )
				imgcon::rgba8_to_rgb8_sse3(					DISPATCH_ARGS );
			else
				imgcon::rgba_to_rgb<UINT8, UINT8, true >(	DISPATCH_ARGS );
			break;
		case PIXEL_FORMAT::RGBA_8:
			if( texture_flux_master->is_convert_to_rgb_sse3() )
				imgcon::rgba8_to_rgb8_sse3(					DISPATCH_ARGS );
			else
				imgcon::rgba_to_rgb<UINT8, UINT8, false>(	DISPATCH_ARGS );
			break;
		// RGB / BGR srcs : 16+ precisions via rgb_to_rgb template ; *_8 share a runtime-swap fast path block (memcpy / SSE3 / fast / scalar).
		case PIXEL_FORMAT::BGR_32FP:	imgcon::rgb_to_rgb<FP32,   UINT8, true >(	DISPATCH_ARGS );	break;
		case PIXEL_FORMAT::RGB_32FP:	imgcon::rgb_to_rgb<FP32,   UINT8, false>(	DISPATCH_ARGS );	break;
		case PIXEL_FORMAT::BGR_16FP:	imgcon::rgb_to_rgb<FP16,   UINT8, true >(	DISPATCH_ARGS );	break;
		case PIXEL_FORMAT::RGB_16FP:	imgcon::rgb_to_rgb<FP16,   UINT8, false>(	DISPATCH_ARGS );	break;
		case PIXEL_FORMAT::BGR_16:		imgcon::rgb_to_rgb<UINT16, UINT8, true >(	DISPATCH_ARGS );	break;
		case PIXEL_FORMAT::RGB_16:		imgcon::rgb_to_rgb<UINT16, UINT8, false>(	DISPATCH_ARGS );	break;
		case PIXEL_FORMAT::BGR_8:		options.b_swap_red_blue = !options.b_swap_red_blue;
		case PIXEL_FORMAT::RGB_8:
			if( options.b_swap_red_blue )
			{
				if( texture_flux_master->is_convert_to_rgb_sse3() )
					imgcon::bgr8_to_rgb8_sse3(	DISPATCH_ARGS );
				else if( texture_flux_master->is_convert_to_rgb_fast() )
					imgcon::bgr8_to_rgb8_fast(	DISPATCH_ARGS );
				else
					imgcon::bgr8_to_rgb8(		DISPATCH_ARGS );
			}
			else
				imgcon::memcpy_image( DISPATCH_ARGS_4, size_y );
			break;
		DISPATCH_RG_CASES( rg_to_rgb, UINT8, );
		// R src : R_8 uses SSE3 fast path ; others go through r_to_rgb template (would clash with DISPATCH_R_CASES).
		case PIXEL_FORMAT::R_32FP:		imgcon::r_to_rgb<FP32,    UINT8>(	DISPATCH_ARGS );	break;
		case PIXEL_FORMAT::R_16FP:		imgcon::r_to_rgb<FP16,    UINT8>(	DISPATCH_ARGS );	break;
		case PIXEL_FORMAT::R_16:		imgcon::r_to_rgb<UINT16,  UINT8>(	DISPATCH_ARGS );	break;
		case PIXEL_FORMAT::R_8:
			if( texture_flux_master->is_convert_to_rgb_sse3() )
				imgcon::r8_to_rgb8_sse3(			DISPATCH_ARGS );
			else
				imgcon::r_to_rgb<UINT8, UINT8>(		DISPATCH_ARGS );
			break;
		default:
			b_done = false;
			break;
		}

	IMGCON_END_COPY()
}

bool	c_img_2d::copy_from_src( void CONST * RESTRICT src, INT32 CONST src_pitch, st_img_conv & options )
{
	auto src_format = options.src_pixel_format;
	if( src_format == PIXEL_FORMAT::UNKNOWN )
	{
		debug_break( "Unknown source pixel format in %s with signature %s", __FUNCTION__, options.signature ? options.signature : "None" );
		return false;
	}
	if( IS_NULL( src ) )
	{
		debug_break( "%s() Null src buffer with signature %s", __FUNCTION__, options.signature ? options.signature : "None" );
		return false;
	}

	auto CONST pixel_format_dst = get_pixel_format();
	auto src8 = reinterpret_cast<UINT8 CONST *>(src);

	if( options.oy != 0 )
		src8 += src_pitch * options.oy;
	if( options.ox != 0 )
		src8 +=  aaa::c_pixel_format::get_byte_per_pixel( src_format ) * options.ox;

	SPY_PUSH_RANGE2( "c_img_2d::copy_from_src", spy::IMG_LOW, c_pixel_format::get_name(pixel_format_dst) );

		bool b_copied = false;
		switch( pixel_format_dst )
		{
		case PIXEL_FORMAT::R_8		:	b_copied = copy_src_to_r8(			src8, src_pitch, src_format, options );	break;
		case PIXEL_FORMAT::RG_8		:	b_copied = copy_src_to_rg8(			src8, src_pitch, src_format, options );	break;
		case PIXEL_FORMAT::BGR_8	:	options.b_swap_red_blue = !options.b_swap_red_blue;
		case PIXEL_FORMAT::RGB_8	:	b_copied = copy_src_to_rgb8(		src8, src_pitch, src_format, options );	break;
		case PIXEL_FORMAT::BGRA_8	:	options.b_swap_red_blue = !options.b_swap_red_blue;
		case PIXEL_FORMAT::RGBA_8	:	b_copied = copy_src_to_rgba8(		src8, src_pitch, src_format, options );	break;

		case PIXEL_FORMAT::R_16		:	b_copied = copy_src_to_r16(			src8, src_pitch, src_format, options );	break;
		case PIXEL_FORMAT::RG_16	:	b_copied = copy_src_to_rg16(		src8, src_pitch, src_format, options );	break;
		case PIXEL_FORMAT::BGR_16	:	options.b_swap_red_blue = !options.b_swap_red_blue;
		case PIXEL_FORMAT::RGB_16	:	b_copied = copy_src_to_rgb16(		src8, src_pitch, src_format, options );	break;
		case PIXEL_FORMAT::BGRA_16	:	options.b_swap_red_blue = !options.b_swap_red_blue;
		case PIXEL_FORMAT::RGBA_16	:	b_copied = copy_src_to_rgba16(		src8, src_pitch, src_format, options );	break;

		case PIXEL_FORMAT::DEPTH_16:
		case PIXEL_FORMAT::R_16FP	:	b_copied = copy_src_to_r16fp(		src8, src_pitch, src_format, options );	break;
		case PIXEL_FORMAT::RG_16FP	:	b_copied = copy_src_to_rg16fp(		src8, src_pitch, src_format, options );	break;
		case PIXEL_FORMAT::BGR_16FP	:	options.b_swap_red_blue = !options.b_swap_red_blue;
		case PIXEL_FORMAT::RGB_16FP	:	b_copied = copy_src_to_rgb16fp(		src8, src_pitch, src_format, options );	break;
		case PIXEL_FORMAT::BGRA_16FP:	options.b_swap_red_blue = !options.b_swap_red_blue;
		case PIXEL_FORMAT::RGBA_16FP:	b_copied = copy_src_to_rgba16fp(	src8, src_pitch, src_format, options );	break;

		case PIXEL_FORMAT::DEPTH_32	:
		case PIXEL_FORMAT::R_32FP	:	b_copied = copy_src_to_r32fp(		src8, src_pitch, src_format, options );	break;
		case PIXEL_FORMAT::RG_32FP	:	b_copied = copy_src_to_rg32fp(		src8, src_pitch, src_format, options );	break;
		case PIXEL_FORMAT::BGR_32FP	:	options.b_swap_red_blue = !options.b_swap_red_blue;
		case PIXEL_FORMAT::RGB_32FP	:	b_copied = copy_src_to_rgb32fp(		src8, src_pitch, src_format, options );	break;
		case PIXEL_FORMAT::BGRA_32FP:	options.b_swap_red_blue = !options.b_swap_red_blue;
		case PIXEL_FORMAT::RGBA_32FP:	b_copied = copy_src_to_rgba32fp(	src8, src_pitch, src_format, options );	break;
		default:
			debug_break( "%s(): copy to %s not supported", __FUNCTION__, c_pixel_format::get_name( get_pixel_format() ) );
			break;
		}
		if( b_copied )
			set_changed();

	SPY_POP_RANGE2();
	return b_copied;
}


#undef DISPATCH_R_CASES
#undef DISPATCH_RG_CASES
#undef DISPATCH_RGB_CASES
#undef DISPATCH_BGR_CASES
#undef DISPATCH_RGBA_CASES
#undef DISPATCH_BGRA_CASES
#undef DISPATCH_FOREACH_PRECISION_2
#undef DISPATCH_FOREACH_PRECISION_3
#undef DISPATCH_ONE
#undef DISPATCH_ARGS
#undef DISPATCH_ARGS_4