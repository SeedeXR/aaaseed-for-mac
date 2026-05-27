
#include "img_convert.h"
#include "img_convert_macros.h"
#include "image/img.h"
#include "media/video/texture_flux_master.h"
#include <immintrin.h>


using namespace aaa;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FP -> integer (u8) pattern conversions. All rgb/rgba/rg/r downscalers (rgb*fp_to_rgb8, rgba*fp_to_rgb8,
// rg*fp_to_rgb8, rfp*_to_rgb8) are served by the imgcon::* template families in img_convert_generic.cpp
// (rgb_to_rgb / rgba_to_rgb / rg_to_rgb / r_to_rgb thin wrappers around rgbx_to_rgbx / rg_to_rgbx / r_to_rgbx).
// The RGBA8 scalar fallback in this file flows through imgcon::dispatch_rgba_to_rgba<T_SRC, UINT8> (declared in
// img_convert.h, body in img_convert_generic.cpp via rgbx_to_rgbx<T_SRC, UINT8, 4, 4, swap, force>).
// Only the local loop_rgba_to_rgba8 helper is kept, used exclusively by the SIMD tail of rgba32fp_to_rgba8_sse41_body.
// The ch_nb-driven generic fp16->u8 / fp32->u8 narrowings live in img_convert_generic.cpp.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// RGBA fp -> RGBA8 scalar inner loop. T_SRC = FP16 or FP32. Used by the < 8 pixel tail of the SSE2 body
// rgba32fp_to_rgba8_sse41_body. Standalone fallbacks (rgba16fp_to_rgba8 and the non-SSE2 path of rgba32fp_to_rgba8)
// go through imgcon::dispatch_rgba_to_rgba<T_SRC, UINT8> which reuses the same per-pixel logic via rgbx_to_rgbx.
//
// Both branches (b_swap_red_blue, b_force_alpha) are hoisted at compile time via template parameters, so the
// per-pixel body has zero conditional. The 4 bytes (r, g, b, a) are packed into a single UINT32 via the
// endian, aware PACK_RGBA and emitted with one 32, bit store instead of 4 byte, stores. On x86 the compiler
// can't merge 4 byte, stores through a UINT8* because of aliasing rules ; doing the pack ourselves halves
// the store buffer pressure (1 entry per pixel instead of 4) and removes 3 of the 4 address, computation chains.
template< typename T_SRC, bool B_SWAP_RB, bool B_FORCE_ALPHA >
FINLINE void	loop_rgba_to_rgba8( T_SRC CONST * RESTRICT p_src, UINT8 * RESTRICT p_dst, INT32 sx, UINT8 alpha ) NOEXCEPT
{
	for( INT32 i = sx; i > 0; --i, p_src += 4, p_dst += 4 )
	{
		UINT8 r,g,b, a;
		if constexpr( B_SWAP_RB )
		{
			r = aaa::img::c_compo::to_uint8( p_src[2] );
			g = aaa::img::c_compo::to_uint8( p_src[1] );
			b = aaa::img::c_compo::to_uint8( p_src[0] );
		}
		else
		{
			r = aaa::img::c_compo::to_uint8( p_src[0] );
			g = aaa::img::c_compo::to_uint8( p_src[1] );
			b = aaa::img::c_compo::to_uint8( p_src[2] );
		}
		if constexpr( B_FORCE_ALPHA )
			a = alpha;
		else
			a = aaa::img::c_compo::to_uint8( p_src[3] );
		// Single 32, bit store via the endian, aware PACK_RGBA helper.
		*reinterpret_cast<UINT32 *>( p_dst ) = PACK_RGBA( r,g,b, a );
	}
}


#if AAA_WIN64()
// SSE2 / SSSE3 / SSE4.1 inner body for rgba32fp_to_rgba8, templated on swap + force-alpha to hoist both
// branches out of the per-pixel work. Block size = 8 RGBA pixels = 32 floats in / 32 bytes out. Per stripe,
// processes one line at a time via call_by_line_striped (contiguous lines per worker).
template< bool B_SWAP_RB, bool B_FORCE_ALPHA >
FINLINE void	rgba32fp_to_rgba8_sse41_body( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options ) NOEXCEPT
{
	// Shuffle mask : byte 0..15 in the output = which input byte (or 0x80 = -127 = zero out).
	// For the no-swap / no-force-alpha case the shuffle is identity, we skip it entirely.
	__m128i mask;
	if constexpr (B_SWAP_RB && B_FORCE_ALPHA)	mask = _mm_set_epi8( -127, 12, 13, 14, -127, 8, 9, 10, -127, 4, 5, 6, -127, 0, 1, 2 );
	else if constexpr (B_SWAP_RB)				mask = _mm_set_epi8( 15, 12, 13, 14, 11, 8, 9, 10, 7, 4, 5, 6, 3, 0, 1, 2 );
	else if constexpr (B_FORCE_ALPHA)			mask = _mm_set_epi8( -127, 14, 13, 12, -127, 10, 9, 8, -127, 6, 5, 4, -127, 2, 1, 0 );
	__m128i CONST	maskalpha	= _mm_set_epi8( options.alpha, 0, 0, 0, options.alpha, 0, 0, 0, options.alpha, 0, 0, 0, options.alpha, 0, 0, 0 );
	__m128  CONST	mul_255		= _mm_set_ps1( 255.f );
	INT32 CONST		blocks		= options.sx / 8;
	INT32 CONST		rem_pix		= options.sx - blocks * 8;

	imgcon::call_by_line_striped( options, [&]( INT32 j ) NOEXCEPT
	{
		IMGCON_PIXEL_LOOP( FP32, UINT8, blocks )
		{
			// 8 x __m128 loads (32 floats = 8 RGBA pixels). Unaligned loads ; modern x86 has no penalty on aligned addresses.
			__m128 in1 = _mm_mul_ps( _mm_loadu_ps( p_src +  0 ), mul_255 );
			__m128 in2 = _mm_mul_ps( _mm_loadu_ps( p_src +  4 ), mul_255 );
			__m128 in3 = _mm_mul_ps( _mm_loadu_ps( p_src +  8 ), mul_255 );
			__m128 in4 = _mm_mul_ps( _mm_loadu_ps( p_src + 12 ), mul_255 );
			__m128 in5 = _mm_mul_ps( _mm_loadu_ps( p_src + 16 ), mul_255 );
			__m128 in6 = _mm_mul_ps( _mm_loadu_ps( p_src + 20 ), mul_255 );
			__m128 in7 = _mm_mul_ps( _mm_loadu_ps( p_src + 24 ), mul_255 );
			__m128 in8 = _mm_mul_ps( _mm_loadu_ps( p_src + 28 ), mul_255 );

			// float -> i32 (round to nearest), pack to u16 saturated, pack to u8 saturated.
			__m128i out1 = _mm_packus_epi32( _mm_cvtps_epi32( in1 ), _mm_cvtps_epi32( in2 ) );
			__m128i y1   = _mm_packus_epi32( _mm_cvtps_epi32( in3 ), _mm_cvtps_epi32( in4 ) );
			out1         = _mm_packus_epi16( out1, y1 );
			__m128i out2 = _mm_packus_epi32( _mm_cvtps_epi32( in5 ), _mm_cvtps_epi32( in6 ) );
			__m128i y2   = _mm_packus_epi32( _mm_cvtps_epi32( in7 ), _mm_cvtps_epi32( in8 ) );
			out2         = _mm_packus_epi16( out2, y2 );

			if constexpr (B_FORCE_ALPHA)
			{
				out1 = _mm_or_si128( _mm_shuffle_epi8( out1, mask ), maskalpha );
				out2 = _mm_or_si128( _mm_shuffle_epi8( out2, mask ), maskalpha );
			}
			else if constexpr (B_SWAP_RB)
			{
				out1 = _mm_shuffle_epi8( out1, mask );
				out2 = _mm_shuffle_epi8( out2, mask );
			}
			// else : identity, no shuffle needed.

			_mm_storeu_si128( (__m128i*)(p_dst +  0), out1 );
			_mm_storeu_si128( (__m128i*)(p_dst + 16), out2 );
			p_src += 32;
			p_dst += 32;
		}
		// Scalar remainder for the tail < 8 pixels. Reuses the templated loop_rgba_to_rgba8 above so the same
		// PACK_RGBA single, store pack applies, no duplicated per, pixel logic.
		loop_rgba_to_rgba8<FP32, B_SWAP_RB, B_FORCE_ALPHA>( p_src, p_dst, rem_pix, options.alpha );
	});
}
#endif //#if AAA_WIN64()


bool	imgcon::rgba32fp_to_rgba8( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
#if AAA_WIN64()
	if( texture_flux_master->is_convert_to_rgb_sse2() )
	{
		SPY_PUSH_RANGE_FUNCTION( spy::IMG_LOW );
			// Dispatch on (swap, force_alpha) once : 4 specialisations, each branch hoisted out of the inner loop.
			if( options.b_force_alpha )
			{
				if( options.b_swap_red_blue )	rgba32fp_to_rgba8_sse41_body<true,  true >( src, src_pitch, dst, dst_pitch, options );
				else							rgba32fp_to_rgba8_sse41_body<false, true >( src, src_pitch, dst, dst_pitch, options );
			}
			else
			{
				if( options.b_swap_red_blue )	rgba32fp_to_rgba8_sse41_body<true,  false>( src, src_pitch, dst, dst_pitch, options );
				else							rgba32fp_to_rgba8_sse41_body<false, false>( src, src_pitch, dst, dst_pitch, options );
			}
		SPY_POP_RANGE();
	}
	else
#endif //#if AAA_WIN64()
	{
		// Scalar fallback : dispatch on (swap, force_alpha) once : 4 specialisations of rgbx_to_rgbx<FP32, UINT8, 4, 4, ...>.
		imgcon::dispatch_rgba_to_rgba<FP32, UINT8>( src, src_pitch, dst, dst_pitch, options );
	}

	return true;
}

bool	imgcon::rgba16fp_to_rgba8( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	imgcon::dispatch_rgba_to_rgba<FP16, UINT8>( src, src_pitch, dst, dst_pitch, options );
	return true;
}

