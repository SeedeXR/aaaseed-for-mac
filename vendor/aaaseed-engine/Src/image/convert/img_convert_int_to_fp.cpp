
#include "img_convert.h"
#include "img_convert_macros.h"
#include "image/img.h"
#include "media/video/texture_flux_master.h"
#include <immintrin.h>


using namespace aaa;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pattern-specific u8 -> fp conversions. The ch_nb-driven generic widenings (uint8_to_fp16 / uint8_to_fp32) live
// in img_convert_generic.cpp; here we deal with BGR / RGBA layouts that have explicit channel handling.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// bgr8_to_rgb16fp removed : replaced by imgcon::rgb_to_rgb<UINT8, FP16, true> (rgbx_to_rgbx<UINT8, FP16, 3, 3, true, false>)
// which has identical inner loop and gains stripe parallel over the previous per-line task dispatch.


void	imgcon::bgr8_to_rgb32fp( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP32* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
#if AAA_WIN64()
	if( texture_flux_master->is_convert_to_rgb_sse2() )
	{
		SPY_PUSH_RANGE_FUNCTION( spy::IMG_LOW );
			__m128 CONST	inv_255	= _mm_set_ps1( 1.f /255.f );
			// Batch up loads/stores into 12 byte chunks to use SSE efficiently :
			INT32 CONST		blocks	= (options.sx * 3) / 12;
			INT32 CONST		rem		= (options.sx * 3) - blocks * 12;
			imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
			{
				IMGCON_LINE_PTRS( UINT8, FP32 );
				for( auto i = blocks; i > 0; --i )
				{
					auto	out1 = _mm_cvtepi32_ps( _mm_cvtepu8_epi32( _mm_cvtsi32_si128( *reinterpret_cast<INT32 CONST*>(p_src + 0) ) ) );
					auto	out2 = _mm_cvtepi32_ps( _mm_cvtepu8_epi32( _mm_cvtsi32_si128( *reinterpret_cast<INT32 CONST*>(p_src + 4) ) ) );
					auto	out3 = _mm_cvtepi32_ps( _mm_cvtepu8_epi32( _mm_cvtsi32_si128( *reinterpret_cast<INT32 CONST*>(p_src + 8) ) ) );
					out1 = _mm_mul_ps( out1, inv_255 );
					out2 = _mm_mul_ps( out2, inv_255 );
					out3 = _mm_mul_ps( out3, inv_255 );
					out1 = _mm_shuffle_ps( out1, out1, _MM_SHUFFLE( 3, 0, 2, 1 ) );
					out2 = _mm_shuffle_ps( out2, out2, _MM_SHUFFLE( 3, 0, 2, 1 ) );
					out3 = _mm_shuffle_ps( out3, out3, _MM_SHUFFLE( 3, 0, 2, 1 ) );

					_mm_storeu_ps( (p_dst + 0), out1 );
					_mm_storeu_ps( (p_dst + 4), out2 );
					_mm_storeu_ps( (p_dst + 8), out3 );
					p_src += 12;
					p_dst += 12;
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
		imgcon::rgb_to_rgb<UINT8, FP32, true>( src, src_pitch, dst, dst_pitch, options );
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool	imgcon::rgba8_to_rgba16fp( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP16* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	if( options.lut )
	{
		imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
		{
			IMGCON_LINE_PTRS( UINT32, FP16 );
			for( auto i = options.sx; i > 0; --i )
			{
				UINT32 u32 = *p_src++;
				UINT8 g = GET_BYTE_1(u32);
				if( options.b_swap_red_blue )
				{
					p_dst[0] = aaa::img::c_compo::to_fp16( GET_BYTE_2(u32) );
					p_dst[2] = aaa::img::c_compo::to_fp16( GET_BYTE_0(u32) );
				}
				else
				{
					p_dst[0] = aaa::img::c_compo::to_fp16( GET_BYTE_0(u32) );
					p_dst[2] = aaa::img::c_compo::to_fp16( GET_BYTE_2(u32) );
				}
				p_dst[1] = aaa::img::c_compo::to_fp16( g );
				p_dst[3] = aaa::img::c_compo::to_fp16( options.lut[g] );
				p_dst += 4;
			}
		});
	}
	else
	{
		// Non, LUT : factorized through dispatch_rgba_to_rgba<UINT8, FP16>, 4 specialisations of rgbx_to_rgbx hoisted out.
		imgcon::dispatch_rgba_to_rgba<UINT8, FP16>( src, src_pitch, dst, dst_pitch, options );
	}

	return true;
}


#if AAA_WIN64()
// SSE4.1 inner body for rgba8_to_rgba32fp, templated on swap + force-alpha to hoist both branches out
// of the per-pixel work. Block size = 4 RGBA pixels = 16 bytes in / 16 floats out. Per stripe, processes
// one line at a time via call_by_line_striped (contiguous lines per worker).
template< bool B_SWAP_RB, bool B_FORCE_ALPHA >
FINLINE void	rgba8_to_rgba32fp_sse41_body( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP32* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options ) NOEXCEPT
{
	__m128 CONST	inv_255			= _mm_set_ps1( 1.f / 255.f );
	__m128 CONST	mask_alpha		= _mm_set_ps1( options.alpha_fp32 );
	INT32 CONST		blocks			= (options.sx * 4) / 16;
	INT32 CONST		rem				= (options.sx * 4) - blocks * 16;

	imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
	{
		IMGCON_LINE_PTRS( UINT8, FP32 );
		for( auto i = blocks; i > 0; --i )
		{
			auto	xmm0 = _mm_cvtepi32_ps( _mm_cvtepu8_epi32( _mm_cvtsi32_si128( *reinterpret_cast<INT32 CONST*>(p_src + 0 ) ) ) );
			auto	xmm1 = _mm_cvtepi32_ps( _mm_cvtepu8_epi32( _mm_cvtsi32_si128( *reinterpret_cast<INT32 CONST*>(p_src + 4 ) ) ) );
			auto	xmm2 = _mm_cvtepi32_ps( _mm_cvtepu8_epi32( _mm_cvtsi32_si128( *reinterpret_cast<INT32 CONST*>(p_src + 8 ) ) ) );
			auto	xmm3 = _mm_cvtepi32_ps( _mm_cvtepu8_epi32( _mm_cvtsi32_si128( *reinterpret_cast<INT32 CONST*>(p_src + 12) ) ) );
			xmm0 = _mm_mul_ps( xmm0, inv_255 );
			xmm1 = _mm_mul_ps( xmm1, inv_255 );
			xmm2 = _mm_mul_ps( xmm2, inv_255 );
			xmm3 = _mm_mul_ps( xmm3, inv_255 );
			if constexpr (B_SWAP_RB)
			{
				xmm0 = _mm_shuffle_ps( xmm0, xmm0, _MM_SHUFFLE( 3, 0, 1, 2 ) );
				xmm1 = _mm_shuffle_ps( xmm1, xmm1, _MM_SHUFFLE( 3, 0, 1, 2 ) );
				xmm2 = _mm_shuffle_ps( xmm2, xmm2, _MM_SHUFFLE( 3, 0, 1, 2 ) );
				xmm3 = _mm_shuffle_ps( xmm3, xmm3, _MM_SHUFFLE( 3, 0, 1, 2 ) );
			}
			if constexpr (B_FORCE_ALPHA)
			{
				xmm0 = _mm_blend_ps( xmm0, mask_alpha, 8 );
				xmm1 = _mm_blend_ps( xmm1, mask_alpha, 8 );
				xmm2 = _mm_blend_ps( xmm2, mask_alpha, 8 );
				xmm3 = _mm_blend_ps( xmm3, mask_alpha, 8 );
			}
			_mm_storeu_ps( (p_dst + 0 ), xmm0 );
			_mm_storeu_ps( (p_dst + 4 ), xmm1 );
			_mm_storeu_ps( (p_dst + 8 ), xmm2 );
			_mm_storeu_ps( (p_dst + 12), xmm3 );
			p_src += 16;
			p_dst += 16;
		}
		for( auto i = rem / 4; i > 0; --i )
		{
			if constexpr (B_SWAP_RB)
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
			if constexpr (B_FORCE_ALPHA)
				p_dst[3] = options.alpha_fp32;
			else
				p_dst[3] = aaa::img::c_compo::to_fp32( p_src[3] );
			p_dst += 4;
			p_src += 4;
		}
	});
}
#endif //#if AAA_WIN64()


bool	imgcon::rgba8_to_rgba32fp( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP32* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
#if AAA_WIN64()
	if( texture_flux_master->is_convert_to_rgb_sse2() )
	{
		SPY_PUSH_RANGE_FUNCTION( spy::IMG_LOW );
			// Dispatch on (swap, force_alpha) once : 4 specialisations, each branch hoisted out of the inner loop.
			if( options.b_force_alpha )
			{
				if( options.b_swap_red_blue )	rgba8_to_rgba32fp_sse41_body<true,  true >( src, src_pitch, dst, dst_pitch, options );
				else							rgba8_to_rgba32fp_sse41_body<false, true >( src, src_pitch, dst, dst_pitch, options );
			}
			else
			{
				if( options.b_swap_red_blue )	rgba8_to_rgba32fp_sse41_body<true,  false>( src, src_pitch, dst, dst_pitch, options );
				else							rgba8_to_rgba32fp_sse41_body<false, false>( src, src_pitch, dst, dst_pitch, options );
			}
		SPY_POP_RANGE();
	}
	else
#endif //#if AAA_WIN64()
	{
		if( options.lut )
		{
			imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
			{
				IMGCON_LINE_PTRS( UINT32, FP32 );
				for( auto i = options.sx; i > 0; --i )
				{
					UINT32 u32 = *p_src++;
					UINT8 g = GET_BYTE_1(u32);
					if( options.b_swap_red_blue )
					{
						p_dst[0] = aaa::img::c_compo::to_fp32( GET_BYTE_2(u32) );
						p_dst[2] = aaa::img::c_compo::to_fp32( GET_BYTE_0(u32) );
					}
					else
					{
						p_dst[0] = aaa::img::c_compo::to_fp32( GET_BYTE_0(u32) );
						p_dst[2] = aaa::img::c_compo::to_fp32( GET_BYTE_2(u32) );
					}
					p_dst[1] = aaa::img::c_compo::to_fp32( g );
					p_dst[3] = aaa::img::c_compo::to_fp32( options.lut[g] );
					p_dst += 4;
				}
			});
		}
		else
		{
			// Non, LUT scalar fallback : factorized through dispatch_rgba_to_rgba<UINT8, FP32>.
			imgcon::dispatch_rgba_to_rgba<UINT8, FP32>( src, src_pitch, dst, dst_pitch, options );
		}
	}
	return true;
}

