
#include "img_convert.h"
#include "img_convert_macros.h"
#include "image/img.h"
#include "media/video/texture_flux_master.h"
#include <immintrin.h>


using namespace aaa;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FP <-> FP pattern conversions: BGR FP <-> RGB FP, RGBA FP precision changes, same-format RGBA FP with optional
// R/B swap. The ch_nb-driven generic fp16<->fp32 widenings live in img_convert_generic.cpp.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// 7-mask shuffle table shared by bgr32fp_to_rgb16fp and bgr16fp_to_rgb32fp. Both walk a 24-fp16 / 24-fp32 stripe
// per block and need to permute 16-bit lanes between BGR and RGB layouts ; the masks are the same in both.
namespace
{
	struct bgr_rgb_24_shuffle_masks
	{
		__m128i mask_bgr_1, mask_bgr_2, mask_bgr_3, mask_bgr_4, mask_bgr_5, mask_bgr_6, mask_bgr_7;
	};
	FINLINE bgr_rgb_24_shuffle_masks make_bgr_rgb_24_shuffle_masks() NOEXCEPT
	{
		return {
			_mm_set_epi8( 15, 14, -127, -127, 7, 6, 9, 8, 11, 10, 1, 0, 3, 2, 5, 4 ),
			_mm_set_epi8( -127, -127, 1, 0, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127 ),
			_mm_set_epi8( -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, 13, 12 ),
			_mm_set_epi8( -127, -127, 9, 8, 11, 10, 13, 12, 3, 2, 5, 4, 7, 6, -127, -127 ),
			_mm_set_epi8( 3, 2, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127 ),
			_mm_set_epi8( -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, 15, 14, -127, -127 ),
			_mm_set_epi8( 11, 10, 13, 12, 15, 14, 5, 4, 7, 6, 9, 8, -127, -127, 1, 0 )
		};
	}
}


void	imgcon::bgr32fp_to_rgb16fp( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP16* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
#if AAA_WIN64()
	if( texture_flux_master->is_convert_to_rgb_sse2() )
	{
		SPY_PUSH_RANGE_FUNCTION( spy::IMG_LOW );
			auto CONST [mask_bgr_1, mask_bgr_2, mask_bgr_3, mask_bgr_4, mask_bgr_5, mask_bgr_6, mask_bgr_7] = make_bgr_rgb_24_shuffle_masks();
			INT32 CONST blocks = (options.sx * 3) / 24;
			INT32 CONST rem    = (options.sx * 3) - blocks * 24;

			imgcon::call_by_line_striped( options, [&]( INT32 j ) NOEXCEPT
			{
				/*		RGBR GBRG BRGB RGBR GBRG BRGB
						BGRB GRBG RBGR BGRB GRBG RBGR
				*/
				IMGCON_PIXEL_LOOP( FP32, FP16, blocks )
				{
					__m128 in1 = _mm_loadu_ps( (p_src + 0) );
					__m128 in2 = _mm_loadu_ps( (p_src + 4) );
					__m128 in3 = _mm_loadu_ps( (p_src + 8) );
					__m128 in4 = _mm_loadu_ps( (p_src + 12) );
					__m128 in5 = _mm_loadu_ps( (p_src + 16) );
					__m128 in6 = _mm_loadu_ps( (p_src + 20) );

					auto	out = _mm_unpacklo_epi64( _mm_cvtps_ph( in1, 0 ), _mm_cvtps_ph( in2, 0 ) );
					auto	out3 = _mm_unpacklo_epi64( _mm_cvtps_ph( in3, 0 ), _mm_cvtps_ph( in4, 0 ) );
					auto	out5 = _mm_unpacklo_epi64( _mm_cvtps_ph( in5, 0 ), _mm_cvtps_ph( in6, 0 ) );

					auto	tmp = _mm_shuffle_epi8( out, mask_bgr_3 );
					out = _mm_or_si128( _mm_shuffle_epi8( out, mask_bgr_1 ), _mm_shuffle_epi8( out3, mask_bgr_2 ));
					auto	tmp2 =_mm_shuffle_epi8( out3, mask_bgr_6 );
					auto	ou2 = _mm_or_si128( tmp, _mm_shuffle_epi8( out3, mask_bgr_4 ) );
					out3 = _mm_or_si128( ou2, _mm_shuffle_epi8( out3, mask_bgr_5 ) );
					out5 = _mm_or_si128( tmp2, _mm_shuffle_epi8( out5, mask_bgr_7 ) );

					_mm_storeu_si128( (__m128i*)(p_dst + 0), out );
					_mm_storeu_si128( (__m128i*)(p_dst + 8), out3 );
					_mm_storeu_si128( (__m128i*)(p_dst + 16), out5 );
					p_dst += 24;
					p_src += 24;
				}
				for( auto i = rem; i > 0; --i )
				{
					p_dst[0] = aaa::img::c_compo::to_fp16( p_src[2] );
					p_dst[1] = aaa::img::c_compo::to_fp16( p_src[1] );
					p_dst[2] = aaa::img::c_compo::to_fp16( p_src[0] );
					p_dst += 3;
					p_src += 3;
				}
			});
		SPY_POP_RANGE();
	}
	else
#endif //#if AAA_WIN64()
		imgcon::rgb_to_rgb<FP32, FP16, true>( src, src_pitch, dst, dst_pitch, options );
}


void	imgcon::bgr16fp_to_rgb32fp( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP32* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
#if AAA_WIN64()
	if( texture_flux_master->is_convert_to_rgb_sse2() )
	{
		SPY_PUSH_RANGE_FUNCTION( spy::IMG_LOW );
			auto CONST [mask_bgr_1, mask_bgr_2, mask_bgr_3, mask_bgr_4, mask_bgr_5, mask_bgr_6, mask_bgr_7] = make_bgr_rgb_24_shuffle_masks();
			INT32 CONST blocks = (options.sx * 3) / 24;
			INT32 CONST rem    = (options.sx * 3) - blocks * 24;

			imgcon::call_by_line_striped( options, [&]( INT32 j ) NOEXCEPT
			{
				/*		RGBR GBRG BRGB RGBR GBRG BRGB
						BGRB GRBG RBGR BGRB GRBG RBGR
				*/
				IMGCON_PIXEL_LOOP( FP16, FP32, blocks )
				{
					__m128i in1 = _mm_loadu_si128( (__m128i*)(p_src + 0 ) );
					__m128i in2 = _mm_loadu_si128( (__m128i*)(p_src + 8 ) );
					__m128i in3 = _mm_loadu_si128( (__m128i*)(p_src + 16) );
					auto	tmp =_mm_shuffle_epi8( in1, mask_bgr_3 );
					auto	tmp2 = _mm_shuffle_epi8( in2, mask_bgr_6 );
					in1 = _mm_or_si128( _mm_shuffle_epi8( in1, mask_bgr_1 ), _mm_shuffle_epi8( in2, mask_bgr_2 ) );
					auto	ou2 = _mm_or_si128( tmp, _mm_shuffle_epi8( in2, mask_bgr_4 ) );
					in2 = _mm_or_si128( ou2, _mm_shuffle_epi8( in3, mask_bgr_5 ) );
					in3 = _mm_or_si128( tmp2, _mm_shuffle_epi8( in3, mask_bgr_7 ) );

					auto	outf1 = _mm_cvtph_ps( in1 );
					auto	outf2 = _mm_cvtph_ps( _mm_unpackhi_epi64( in1, in1 ) );
					auto	outf3 = _mm_cvtph_ps( in2 );
					auto	outf4 = _mm_cvtph_ps( _mm_unpackhi_epi64( in2, in2 ) );
					auto	outf5 = _mm_cvtph_ps( in3 );
					auto	outf6 = _mm_cvtph_ps( _mm_unpackhi_epi64( in3, in3 ) );

					_mm_storeu_ps( (p_dst + 0 ), outf1 );
					_mm_storeu_ps( (p_dst + 4 ), outf2 );
					_mm_storeu_ps( (p_dst + 8 ), outf3 );
					_mm_storeu_ps( (p_dst + 12), outf4 );
					_mm_storeu_ps( (p_dst + 16), outf5 );
					_mm_storeu_ps( (p_dst + 20), outf6 );
					p_dst += 24;
					p_src += 24;
				}
				for( auto i = rem; i > 0; --i )
				{
					p_dst[0] = aaa::img::c_compo::to_fp32( p_src[2] );
					p_dst[1] = aaa::img::c_compo::to_fp32( p_src[1] );
					p_dst[2] = aaa::img::c_compo::to_fp32( p_src[0] );
					p_dst += 3;
					p_src += 3;
				}
			});
		SPY_POP_RANGE();
	}
	else
#endif //#if AAA_WIN64()
		imgcon::rgb_to_rgb<FP16, FP32, true>( src, src_pitch, dst, dst_pitch, options );
}



#if AAA_WIN64()
// SSE2 / SSE4.1 / F16C inner body for rgba32fp_to_rgba16fp, templated on swap + force-alpha so both
// branches are hoisted out of the per-pixel work. Block size = 4 RGBA pixels = 16 floats in / 16 fp16 out.
// Per stripe, processes one line at a time via call_by_line_striped (contiguous lines per worker).
template< bool B_SWAP_RB, bool B_FORCE_ALPHA >
FINLINE void	rgba32fp_to_rgba16fp_sse41_body( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP16* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options, FP16 CONST alpha ) NOEXCEPT
{
	__m128i mask_bgr     = _mm_setzero_si128();
	__m128i mask_alpha_2 = _mm_setzero_si128();
	if constexpr( B_SWAP_RB )	mask_bgr = _mm_set_epi8( 15, 14, 9, 8, 11, 10, 13, 12, 7, 6, 2, 1, 3, 2, 5, 4 );
	if constexpr( B_FORCE_ALPHA )
	{
		INT16 CONST alpha_bits = reinterpret_cast<INT16 CONST &>( alpha );
		mask_alpha_2 = _mm_set_epi16( alpha_bits, -127, -127, -127, alpha_bits, -127, -127, -127 );
	}
	INT32 CONST blocks = (options.sx * 4) / 16;
	INT32 CONST rem    = (options.sx * 4) - blocks * 16;

	imgcon::call_by_line_striped( options, [&]( INT32 j ) NOEXCEPT
	{
		/*		RGBA RGBA RGBA RGBA
				BGRA BGRA BGRA BGRA
		*/
		IMGCON_PIXEL_LOOP( FP32, FP16, blocks )
		{
			__m128 in1 = _mm_loadu_ps( (p_src + 0) );
			__m128 in2 = _mm_loadu_ps( (p_src + 4) );
			__m128 in3 = _mm_loadu_ps( (p_src + 8) );
			__m128 in4 = _mm_loadu_ps( (p_src + 12) );

			auto	out = _mm_unpacklo_epi64( _mm_cvtps_ph( in1, 0 ), _mm_cvtps_ph( in2, 0 ) );
			auto	out3 = _mm_unpacklo_epi64( _mm_cvtps_ph( in3, 0 ), _mm_cvtps_ph( in4, 0 ) );

			if constexpr( B_SWAP_RB )
			{
				out  = _mm_shuffle_epi8( out, mask_bgr );
				out3 = _mm_shuffle_epi8( out3, mask_bgr );
			}
			if constexpr( B_FORCE_ALPHA )
			{
				out  = _mm_blend_epi16( out, mask_alpha_2, 136 );
				out3 = _mm_blend_epi16( out3, mask_alpha_2, 138 );
			}

			_mm_storeu_si128( (__m128i*)(p_dst + 0), out );
			_mm_storeu_si128( (__m128i*)(p_dst + 8), out3 );
			p_dst += 16;
			p_src += 16;
		}
		for( auto i = rem; i > 0; --i )
		{
			if constexpr( B_SWAP_RB )
			{
				p_dst[0] = aaa::img::c_compo::to_fp16( p_src[2] );
				p_dst[1] = aaa::img::c_compo::to_fp16( p_src[1] );
				p_dst[2] = aaa::img::c_compo::to_fp16( p_src[0] );
			}
			else
			{
				p_dst[0] = aaa::img::c_compo::to_fp16( p_src[0] );
				p_dst[1] = aaa::img::c_compo::to_fp16( p_src[1] );
				p_dst[2] = aaa::img::c_compo::to_fp16( p_src[2] );
			}
			if constexpr( B_FORCE_ALPHA )	p_dst[3] = alpha;
			else							p_dst[3] = aaa::img::c_compo::to_fp16( p_src[3] );
			p_dst += 4;
			p_src += 4;
		}
	});
}
#endif //#if AAA_WIN64()


bool	imgcon::rgba32fp_to_rgba16fp( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP16* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	FP16 CONST alpha = aaa::img::c_compo::to_fp16( options.alpha_fp32 );
#if AAA_WIN64()
	if( texture_flux_master->is_convert_to_rgb_sse2() )
	{
		SPY_PUSH_RANGE_FUNCTION( spy::IMG_LOW );
			// Dispatch on (swap, force_alpha) once : 4 specialisations, each branch hoisted out of the inner loop.
			if( options.b_force_alpha )
			{
				if( options.b_swap_red_blue )	rgba32fp_to_rgba16fp_sse41_body<true,  true >( src, src_pitch, dst, dst_pitch, options, alpha );
				else							rgba32fp_to_rgba16fp_sse41_body<false, true >( src, src_pitch, dst, dst_pitch, options, alpha );
			}
			else
			{
				if( options.b_swap_red_blue )	rgba32fp_to_rgba16fp_sse41_body<true,  false>( src, src_pitch, dst, dst_pitch, options, alpha );
				else							rgba32fp_to_rgba16fp_sse41_body<false, false>( src, src_pitch, dst, dst_pitch, options, alpha );
			}
		SPY_POP_RANGE();
	}
	else
#endif //#if AAA_WIN64()
	{
		// Scalar fallback : factorized through dispatch_rgba_to_rgba<FP32, FP16>, 4 specialisations hoisted out.
		imgcon::dispatch_rgba_to_rgba<FP32, FP16>( src, src_pitch, dst, dst_pitch, options );
	}
	return true;
}

// -------------------------------------------------------------------------------------------------
// SIMD helpers for rgba16fp_to_rgba32fp (cross-format widen). Uses F16C: _mm_cvtph_ps (4 FP16 -> 4 FP32)
// and _mm256_cvtph_ps (8 FP16 -> 8 FP32). Swap and force-alpha are applied in FP32 land after the convert,
// reusing the same RGBA_SHUFFLE_MASK / blend mask conventions as the same-format kernels below.
// Dispatch ladder: AVX2 (2 px/iter) -> SSE2+F16C (1 px/iter) -> scalar. Note that the SSE tier still
// requires F16C ; this matches the convention used by bgr16fp_to_rgb32fp and rgba32fp_to_rgba16fp.
// -------------------------------------------------------------------------------------------------
namespace
{
	INT32 CONSTEXPR	RGBA16TO32_SHUFFLE_MASK = 0xC6;	// _MM_SHUFFLE(3,0,1,2)

	template <bool B_SWAP, bool B_FORCE_ALPHA>
	FINLINE void rgba16fp_to_rgba32fp_row_avx2( FP16 CONST * RESTRICT p_src, FP32* RESTRICT p_dst, INT32 sx, FP32 alpha ) NOEXCEPT
	{
		__m256 alpha_vec_256 = _mm256_setzero_ps();
		if constexpr( B_FORCE_ALPHA )	alpha_vec_256 = _mm256_set1_ps( alpha );
		INT32 i = sx;
		for( ; i >= 2; i -= 2, p_src += 8, p_dst += 8 )
		{
			__m128i in = _mm_loadu_si128( (__m128i CONST *)p_src );	// 8 FP16 = 2 RGBA pixels
			__m256  v  = _mm256_cvtph_ps( in );						// 8 FP32
			if constexpr( B_SWAP )
				v = _mm256_permute_ps( v, RGBA16TO32_SHUFFLE_MASK );
			if constexpr( B_FORCE_ALPHA )
				v = _mm256_blend_ps( v, alpha_vec_256, 0x88 );		// alpha lane of both 128-bit halves
			_mm256_storeu_ps( p_dst, v );
		}
		if( i > 0 )	// 0 or 1 leftover pixel
		{
			__m128i in = _mm_loadl_epi64( (__m128i CONST *)p_src );	// 4 FP16 (lower 64 bits)
			__m128  v  = _mm_cvtph_ps( in );						// 4 FP32
			if constexpr( B_SWAP )
				v = _mm_shuffle_ps( v, v, RGBA16TO32_SHUFFLE_MASK );
			if constexpr( B_FORCE_ALPHA )
				v = _mm_blend_ps( v, _mm_set1_ps( alpha ), 0x8 );	// SSE4.1, available under AVX2
			_mm_storeu_ps( p_dst, v );
		}
	}

	template <bool B_SWAP, bool B_FORCE_ALPHA>
	FINLINE void rgba16fp_to_rgba32fp_row_sse( FP16 CONST * RESTRICT p_src, FP32* RESTRICT p_dst, INT32 sx, FP32 alpha ) NOEXCEPT
	{
		// Pure SSE2 (+ F16C for the half->float convert): blend via and/andn/or, no _mm_blend_ps.
		__m128 alpha_vec  = _mm_setzero_ps();
		__m128 alpha_mask = _mm_setzero_ps();
		if constexpr( B_FORCE_ALPHA )
		{
			alpha_vec  = _mm_set_ps( alpha, 0.f, 0.f, 0.f );
			alpha_mask = _mm_castsi128_ps( _mm_set_epi32( -1, 0, 0, 0 ) );
		}
		for( INT32 i = sx; i > 0; --i, p_src += 4, p_dst += 4 )
		{
			__m128i in = _mm_loadl_epi64( (__m128i CONST *)p_src );	// 4 FP16 = 1 RGBA pixel
			__m128  v  = _mm_cvtph_ps( in );						// 4 FP32 (F16C)
			if constexpr( B_SWAP )
				v = _mm_shuffle_ps( v, v, RGBA16TO32_SHUFFLE_MASK );
			if constexpr( B_FORCE_ALPHA )
				v = _mm_or_ps( _mm_andnot_ps( alpha_mask, v ), alpha_vec );
			_mm_storeu_ps( p_dst, v );
		}
	}

	template <bool B_SWAP, bool B_FORCE_ALPHA>
	FINLINE void rgba16fp_to_rgba32fp_row_scalar( FP16 CONST * RESTRICT p_src, FP32* RESTRICT p_dst, INT32 sx, FP32 alpha ) NOEXCEPT
	{
		for( INT32 i = sx; i > 0; --i, p_src += 4, p_dst += 4 )
		{
			if constexpr( B_SWAP )
			{
				p_dst[0] = aaa::img::c_compo::to_fp32( p_src[2] );
				p_dst[1] = aaa::img::c_compo::to_fp32( p_src[1] );
				p_dst[2] = aaa::img::c_compo::to_fp32( p_src[0] );
			}
			else
			{
				p_dst[0] = aaa::img::c_compo::to_fp32( p_src[0] );
				p_dst[1] = aaa::img::c_compo::to_fp32( p_src[1] );
				p_dst[2] = aaa::img::c_compo::to_fp32( p_src[2] );
			}
			if constexpr( B_FORCE_ALPHA )	p_dst[3] = alpha;
			else							p_dst[3] = aaa::img::c_compo::to_fp32( p_src[3] );
		}
	}

	template <bool B_SWAP, bool B_FORCE_ALPHA>
	void rgba16fp_to_rgba32fp_run( FP16 CONST * RESTRICT src, INT32 src_pitch, FP32* RESTRICT dst, INT32 dst_pitch, INT32 sx, INT32 sy, FP32 alpha )
	{
		if( texture_flux_master->is_convert_to_rgb_avx2() )
		{
			imgcon::call_by_line_striped( sy, [&](INT32 j) NOEXCEPT
			{
				IMGCON_LINE_PTRS( FP16, FP32 );
				rgba16fp_to_rgba32fp_row_avx2<B_SWAP, B_FORCE_ALPHA>( p_src, p_dst, sx, alpha );
			});
		}
		else if( texture_flux_master->is_convert_to_rgb_sse2() )
		{
			imgcon::call_by_line_striped( sy, [&](INT32 j) NOEXCEPT
			{
				IMGCON_LINE_PTRS( FP16, FP32 );
				rgba16fp_to_rgba32fp_row_sse<B_SWAP, B_FORCE_ALPHA>( p_src, p_dst, sx, alpha );
			});
		}
		else
		{
			imgcon::call_by_line_striped( sy, [&](INT32 j) NOEXCEPT
			{
				IMGCON_LINE_PTRS( FP16, FP32 );
				rgba16fp_to_rgba32fp_row_scalar<B_SWAP, B_FORCE_ALPHA>( p_src, p_dst, sx, alpha );
			});
		}
	}
}

bool	imgcon::rgba16fp_to_rgba32fp( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP32* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	SPY_PUSH_RANGE_FUNCTION( spy::IMG_LOW );
	// Dispatch on (swap, force_alpha) once : 4 specialisations, each branch hoisted out of the inner loop.
	if( options.b_swap_red_blue )
	{
		if( options.b_force_alpha )	rgba16fp_to_rgba32fp_run<true , true >( reinterpret_cast<FP16 CONST *>(src), src_pitch, dst, dst_pitch, options.sx, options.sy, options.alpha_fp32 );
		else						rgba16fp_to_rgba32fp_run<true , false>( reinterpret_cast<FP16 CONST *>(src), src_pitch, dst, dst_pitch, options.sx, options.sy, options.alpha_fp32 );
	}
	else
	{
		if( options.b_force_alpha )	rgba16fp_to_rgba32fp_run<false, true >( reinterpret_cast<FP16 CONST *>(src), src_pitch, dst, dst_pitch, options.sx, options.sy, options.alpha_fp32 );
		else						rgba16fp_to_rgba32fp_run<false, false>( reinterpret_cast<FP16 CONST *>(src), src_pitch, dst, dst_pitch, options.sx, options.sy, options.alpha_fp32 );
	}
	SPY_POP_RANGE();
	return true;
}

// -------------------------------------------------------------------------------------------------
// SIMD helpers for rgba16fp_to_rgba16fp / rgba32fp_to_rgba32fp.
//
// Per-pixel work is at most: optional R/B swap (lane 0 <-> lane 2), optional alpha override.
// Both flags are hoisted out of the inner loop by templating on bool B_SWAP / bool B_FORCE_ALPHA,
// so the compiler emits a dedicated specialization per useful combination.
//
// Shuffle mask reused across fp32 and fp16: _MM_SHUFFLE(3,0,1,2) = 0xC6
//   selects lanes (3, 0, 1, 2) -> (A, B, G, R) on a BGRA register, producing RGBA.
//
// Dispatch ladders (selected once before parallel::call):
//   fp32: AVX1 (256-bit, 2 pixels/iter)  -> SSE2 (128-bit, 1 pixel/iter)  -> scalar
//   fp16: AVX2 (256-bit, 4 pixels/iter)  -> SSE2 (128-bit, 2 pixels/iter) -> scalar
// AVX2 fp32 uses the AVX1 code (same instructions); AVX1 cannot do 256-bit epi16 shuffles
// so fp16 has no AVX1 tier.
// -------------------------------------------------------------------------------------------------
namespace
{
	INT32 CONSTEXPR	RGBA_SHUFFLE_MASK = 0xC6;	// _MM_SHUFFLE(3,0,1,2)

	// Scalar copy of one rgba pixel: optional R/B swap on lanes 0..2, optional alpha override on lane 3.
	// Used for the scalar fallback path and for the 0/1-pixel tail of the SIMD paths.
	// NOTE: lanes are stored individually (not via cpy_v3) because the FP32 overload of cpy_v3 in
	// math/v_base.h uses memcpy(dst, src, 12) which is NOT inlined in Debug builds. Per the codebase
	// rule "no memcpy in per-pixel paths", we expand the stores here.
	template <bool B_SWAP, bool B_FORCE_ALPHA, typename T>
	FINLINE void cpy_rgba_pixel( T* RESTRICT p_dst, T CONST * RESTRICT p_src, T alpha ) NOEXCEPT
	{
		if constexpr( B_SWAP )
		{
			p_dst[0] = p_src[2];
			p_dst[1] = p_src[1];
			p_dst[2] = p_src[0];
		}
		else
		{
			p_dst[0] = p_src[0];
			p_dst[1] = p_src[1];
			p_dst[2] = p_src[2];
		}
		if constexpr( B_FORCE_ALPHA )	p_dst[3] = alpha;
		else							p_dst[3] = p_src[3];
	}

	// ---- fp32 row kernels ----
	template <bool B_SWAP, bool B_FORCE_ALPHA>
	FINLINE void rgba32fp_row_avx( FP32 CONST * RESTRICT p_src, FP32* RESTRICT p_dst, INT32 sx, FP32 alpha ) NOEXCEPT
	{
		__m256 alpha_vec_256 = _mm256_setzero_ps();
		if constexpr( B_FORCE_ALPHA )	alpha_vec_256 = _mm256_set1_ps( alpha );
		INT32 i = sx;
		for( ;i >= 2; i -= 2, p_src += 8, p_dst += 8 )
		{
			__m256 v = _mm256_loadu_ps( p_src );
			if constexpr( B_SWAP )
				v = _mm256_permute_ps( v, RGBA_SHUFFLE_MASK );
			if constexpr( B_FORCE_ALPHA )
				v = _mm256_blend_ps( v, alpha_vec_256, 0x88 );		// alpha lane of both 128-bit halves
			_mm256_storeu_ps( p_dst, v );
		}
		if( i > 0 )	// 0 or 1 leftover pixel
		{
			__m128 v = _mm_loadu_ps( p_src );
			if constexpr( B_SWAP )
				v = _mm_shuffle_ps( v, v, RGBA_SHUFFLE_MASK );
			if constexpr( B_FORCE_ALPHA )
				v = _mm_blend_ps( v, _mm_set1_ps( alpha ), 0x8 );	// SSE4.1, available under AVX
			_mm_storeu_ps( p_dst, v );
		}
	}

	template <bool B_SWAP, bool B_FORCE_ALPHA>
	FINLINE void rgba32fp_row_sse( FP32 CONST * RESTRICT p_src, FP32* RESTRICT p_dst, INT32 sx, FP32 alpha ) NOEXCEPT
	{
		// Pure SSE2: blend implemented with and/andn/or (no SSE4.1).
		__m128 alpha_vec  = _mm_setzero_ps();
		__m128 alpha_mask = _mm_setzero_ps();
		if constexpr( B_FORCE_ALPHA )
		{
			alpha_vec  = _mm_set_ps( alpha, 0.f, 0.f, 0.f );
			alpha_mask = _mm_castsi128_ps( _mm_set_epi32( -1, 0, 0, 0 ) );
		}
		for( INT32 i = sx; i > 0; --i, p_src += 4, p_dst += 4 )
		{
			__m128 v = _mm_loadu_ps( p_src );
			if constexpr( B_SWAP )
				v = _mm_shuffle_ps( v, v, RGBA_SHUFFLE_MASK );
			if constexpr( B_FORCE_ALPHA )
				v = _mm_or_ps( _mm_andnot_ps( alpha_mask, v ), alpha_vec );
			_mm_storeu_ps( p_dst, v );
		}
	}

	template <bool B_SWAP, bool B_FORCE_ALPHA>
	FINLINE void rgba32fp_row_scalar( FP32 CONST * RESTRICT p_src, FP32* RESTRICT p_dst, INT32 sx, FP32 alpha ) NOEXCEPT
	{
		for( INT32 i = sx; i > 0; --i, p_src += 4, p_dst += 4 )
		{
			cpy_rgba_pixel<B_SWAP, B_FORCE_ALPHA>( p_dst, p_src, alpha );
		}
	}

	template <bool B_SWAP, bool B_FORCE_ALPHA>
	void rgba32fp_run( FP32 CONST * RESTRICT src, INT32 src_pitch, FP32* RESTRICT dst, INT32 dst_pitch, INT32 sx, INT32 sy, FP32 alpha )
	{
		if( texture_flux_master->is_convert_to_rgb_avx2() )
		{
			imgcon::call_by_line_striped( sy, [&](INT32 j) NOEXCEPT
			{
				IMGCON_LINE_PTRS( FP32, FP32 );
				rgba32fp_row_avx<B_SWAP, B_FORCE_ALPHA>( p_src, p_dst, sx, alpha );
			});
		}
		else if( texture_flux_master->is_convert_to_rgb_sse2() )
		{
			imgcon::call_by_line_striped( sy, [&](INT32 j) NOEXCEPT
			{
				IMGCON_LINE_PTRS( FP32, FP32 );
				rgba32fp_row_sse<B_SWAP, B_FORCE_ALPHA>( p_src, p_dst, sx, alpha );
			});
		}
		else
		{
			imgcon::call_by_line_striped( sy, [&](INT32 j) NOEXCEPT
			{
				IMGCON_LINE_PTRS( FP32, FP32 );
				rgba32fp_row_scalar<B_SWAP, B_FORCE_ALPHA>( p_src, p_dst, sx, alpha );
			});
		}
	}

	// ---- fp16 row kernels ----
	template <bool B_SWAP, bool B_FORCE_ALPHA>
	FINLINE void rgba16fp_row_avx2( FP16 CONST * RESTRICT p_src, FP16* RESTRICT p_dst, INT32 sx, FP16 alpha ) NOEXCEPT
	{
		// 256-bit AVX2: 4 pixels / iter (16 * FP16 = 256 bits).
		// FP16 ( = half_float::half ) is a thin wrapper around a single uint16_t, so reinterpret_cast gives the raw bits.
		__m256i alpha_vec_256 = _mm256_setzero_si256();
		if constexpr( B_FORCE_ALPHA )	alpha_vec_256 = _mm256_set1_epi16( reinterpret_cast<INT16 CONST &>( alpha ) );
		INT32 i = sx;
		for( ;i >= 4; i -= 4, p_src += 16, p_dst += 16 )
		{
			__m256i v = _mm256_loadu_si256( (__m256i CONST *)p_src );
			if constexpr( B_SWAP )
			{
				v = _mm256_shufflelo_epi16( v, RGBA_SHUFFLE_MASK );
				v = _mm256_shufflehi_epi16( v, RGBA_SHUFFLE_MASK );
			}
			if constexpr( B_FORCE_ALPHA )
				v = _mm256_blend_epi16( v, alpha_vec_256, 0x88 );		// alpha lane (3) of each pixel in each 128-bit half
			_mm256_storeu_si256( (__m256i *)p_dst, v );
		}
		if( i >= 2 )	// 2-pixel SSE tail (128 bits = 8 FP16)
		{
			__m128i v = _mm_loadu_si128( (__m128i CONST *)p_src );
			if constexpr( B_SWAP )
			{
				v = _mm_shufflelo_epi16( v, RGBA_SHUFFLE_MASK );
				v = _mm_shufflehi_epi16( v, RGBA_SHUFFLE_MASK );
			}
			if constexpr( B_FORCE_ALPHA )
			{
				__m128i CONST alpha_vec = _mm_set1_epi16( reinterpret_cast<INT16 CONST &>( alpha ) );
				v = _mm_blend_epi16( v, alpha_vec, 0x88 );				// SSE4.1, available under AVX2
			}
			_mm_storeu_si128( (__m128i *)p_dst, v );
			i -= 2; p_src += 8; p_dst += 8;
		}
		if( i > 0 )	// 0 or 1 leftover pixel
		{
			cpy_rgba_pixel<B_SWAP, B_FORCE_ALPHA>( p_dst, p_src, alpha );
		}
	}

	template <bool B_SWAP, bool B_FORCE_ALPHA>
	FINLINE void rgba16fp_row_sse( FP16 CONST * RESTRICT p_src, FP16* RESTRICT p_dst, INT32 sx, FP16 alpha ) NOEXCEPT
	{
		// 128-bit SSE2: 2 pixels / iter (8 * FP16 = 128 bits). Pure SSE2: no _mm_blend_epi16.
		__m128i alpha_vec  = _mm_setzero_si128();
		__m128i alpha_mask = _mm_setzero_si128();
		if constexpr( B_FORCE_ALPHA )
		{
			// FP16 ( = half_float::half ) is a thin wrapper around a single uint16_t, so reinterpret_cast gives the raw bits.
			INT16 CONST alpha_bits = reinterpret_cast<INT16 CONST &>( alpha );
			alpha_vec  = _mm_set_epi16( alpha_bits, 0, 0, 0, alpha_bits, 0, 0, 0 );
			alpha_mask = _mm_set_epi16( -1, 0, 0, 0, -1, 0, 0, 0 );
		}
		INT32 i = sx;
		for( ;i >= 2; i -= 2, p_src += 8, p_dst += 8 )
		{
			__m128i v = _mm_loadu_si128( (__m128i CONST *)p_src );
			if constexpr( B_SWAP )
			{
				v = _mm_shufflelo_epi16( v, RGBA_SHUFFLE_MASK );
				v = _mm_shufflehi_epi16( v, RGBA_SHUFFLE_MASK );
			}
			if constexpr( B_FORCE_ALPHA )
				v = _mm_or_si128( _mm_andnot_si128( alpha_mask, v ), alpha_vec );
			_mm_storeu_si128( (__m128i *)p_dst, v );
		}
		if( i > 0 )	// 0 or 1 leftover pixel
		{
			cpy_rgba_pixel<B_SWAP, B_FORCE_ALPHA>( p_dst, p_src, alpha );
		}
	}

	template <bool B_SWAP, bool B_FORCE_ALPHA>
	FINLINE void rgba16fp_row_scalar( FP16 CONST * RESTRICT p_src, FP16* RESTRICT p_dst, INT32 sx, FP16 alpha ) NOEXCEPT
	{
		for( INT32 i = sx; i > 0; --i, p_src += 4, p_dst += 4 )
		{
			cpy_rgba_pixel<B_SWAP, B_FORCE_ALPHA>( p_dst, p_src, alpha );
		}
	}

	template <bool B_SWAP, bool B_FORCE_ALPHA>
	void rgba16fp_run( FP16 CONST * RESTRICT src, INT32 src_pitch, FP16* RESTRICT dst, INT32 dst_pitch, INT32 sx, INT32 sy, FP16 alpha )
	{
		if( texture_flux_master->is_convert_to_rgb_avx2() )
		{
			imgcon::call_by_line_striped( sy, [&](INT32 j) NOEXCEPT
			{
				IMGCON_LINE_PTRS( FP16, FP16 );
				rgba16fp_row_avx2<B_SWAP, B_FORCE_ALPHA>( p_src, p_dst, sx, alpha );
			});
		}
		else if( texture_flux_master->is_convert_to_rgb_sse2() )
		{
			imgcon::call_by_line_striped( sy, [&](INT32 j) NOEXCEPT
			{
				IMGCON_LINE_PTRS( FP16, FP16 );
				rgba16fp_row_sse<B_SWAP, B_FORCE_ALPHA>( p_src, p_dst, sx, alpha );
			});
		}
		else
		{
			imgcon::call_by_line_striped( sy, [&](INT32 j) NOEXCEPT
			{
				IMGCON_LINE_PTRS( FP16, FP16 );
				rgba16fp_row_scalar<B_SWAP, B_FORCE_ALPHA>( p_src, p_dst, sx, alpha );
			});
		}
	}
}

bool	imgcon::rgba16fp_to_rgba16fp( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP16* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	if( !options.b_swap_red_blue && !options.b_force_alpha )
	{
		imgcon::memcpy_image( src, src_pitch, dst, dst_pitch, options.sy );
		return false;
	}
	SPY_PUSH_RANGE_FUNCTION( spy::IMG_LOW );
	FP16 CONST alpha = aaa::img::c_compo::to_fp16( options.alpha_fp32 );
	if( options.b_swap_red_blue )
	{
		if( options.b_force_alpha )	rgba16fp_run<true , true >( reinterpret_cast<FP16 CONST *>(src), src_pitch, dst, dst_pitch, options.sx, options.sy, alpha );
		else						rgba16fp_run<true , false>( reinterpret_cast<FP16 CONST *>(src), src_pitch, dst, dst_pitch, options.sx, options.sy, alpha );
	}
	else
	{
		// b_force_alpha is necessarily true here: the (!swap && !force) case was handled by memcpy_image above.
		rgba16fp_run<false, true >( reinterpret_cast<FP16 CONST *>(src), src_pitch, dst, dst_pitch, options.sx, options.sy, alpha );
	}
	SPY_POP_RANGE();
	return true;
}

bool	imgcon::rgba32fp_to_rgba32fp( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP32* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	if( !options.b_swap_red_blue && !options.b_force_alpha )
	{
		imgcon::memcpy_image( src, src_pitch, dst, dst_pitch, options.sy );
		return false;
	}
	SPY_PUSH_RANGE_FUNCTION( spy::IMG_LOW );
	if( options.b_swap_red_blue )
	{
		if( options.b_force_alpha )	rgba32fp_run<true , true >( reinterpret_cast<FP32 CONST *>(src), src_pitch, dst, dst_pitch, options.sx, options.sy, options.alpha_fp32 );
		else						rgba32fp_run<true , false>( reinterpret_cast<FP32 CONST *>(src), src_pitch, dst, dst_pitch, options.sx, options.sy, options.alpha_fp32 );
	}
	else
	{
		// b_force_alpha is necessarily true here: the (!swap && !force) case was handled by memcpy_image above.
		rgba32fp_run<false, true >( reinterpret_cast<FP32 CONST *>(src), src_pitch, dst, dst_pitch, options.sx, options.sy, options.alpha_fp32 );
	}
	SPY_POP_RANGE();
	return true;
}

