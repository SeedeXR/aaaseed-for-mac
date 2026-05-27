
#include "img_convert.h"
#include "img_convert_macros.h"
#include "color_space.h"
#include "image/img.h"
#include "media/video/texture_flux_master.h"
#include <immintrin.h>


using namespace aaa;


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Generic per-component conversions (ch_nb-driven). These walk every component of every pixel and write each one
// after a fixed transformation. They are pattern-agnostic: the caller picks ch_nb = 1 / 2 / 3 / 4 to cover R / RG /
// RGB / RGBA without channel reorder. BGR / BGRA srcs need a swap-aware variant elsewhere.
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////


// INSTANCE_FOREACH_T_DST : cascades a per-instantiation macro FN over the 4 T_DST precisions {UINT8, UINT16, FP16, FP32}.
// FN must take its T_DST arg LAST so the cascade can append it. Used to collapse every per-family INSTANCE_X_ALL_DST(...)
// 4-line cascade to a 1-liner that owns the precision list in this one place (so a new precision is a 1-line edit).
// EXPAND wrapper works around MSVC's traditional preprocessor flattening __VA_ARGS__ when re-expanded through a macro.
#define INSTANCE_EXPAND( x )	x
#define INSTANCE_FOREACH_T_DST( FN, ... ) \
	INSTANCE_EXPAND( FN(__VA_ARGS__, UINT8 ) );	\
	INSTANCE_EXPAND( FN(__VA_ARGS__, UINT16) );	\
	INSTANCE_EXPAND( FN(__VA_ARGS__, FP16  ) );	\
	INSTANCE_EXPAND( FN(__VA_ARGS__, FP32  ) )



namespace
{
	// Stripe parallel per-component conversion via c_compo::convert<T_SRC, T_DST>. Used by the scalar fallbacks
	// of u8 -> u16, u16 -> fp16, u16 -> fp32 widenings. Stripe (multiple lines per task, nb_stripes = thread count
	// capped to sy) keeps per-task overhead low on fast inner loops.
	template< typename T_SRC, typename T_DST >
	FINLINE void	convert_per_compo_stripe( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST* RESTRICT dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options ) NOEXCEPT
	{
		UINT32 CONST	blocks	= (options.sx * ch_nb) / 4;
		UINT32 CONST	rem		= (options.sx * ch_nb) - blocks * 4;

		imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
		{
			IMGCON_PIXEL_LOOP( T_SRC, T_DST, blocks )
			{
				p_dst[0] = aaa::img::c_compo::convert<T_SRC, T_DST>( p_src[0] );
				p_dst[1] = aaa::img::c_compo::convert<T_SRC, T_DST>( p_src[1] );
				p_dst[2] = aaa::img::c_compo::convert<T_SRC, T_DST>( p_src[2] );
				p_dst[3] = aaa::img::c_compo::convert<T_SRC, T_DST>( p_src[3] );
				p_dst += 4;
				p_src += 4;
			}
			for( auto i = rem; i > 0; --i )
				*p_dst++ = aaa::img::c_compo::convert<T_SRC, T_DST>( *p_src++ );
		});
	}
}


// R-source replicated to RGB(3) or RGBA(4) dst : read 1 T_SRC component per pixel, c_compo::convert<T_SRC, T_DST>,
// broadcast on the 3 RGB lanes. CH_NB == 4 adds an alpha lane :
//   - B_FORCE_ALPHA = 0 : alpha = opaque (c_compo::convert<FP32, T_DST>(1.f))
//   - B_FORCE_ALPHA = 1 : alpha = options.alpha_fp32 routed through c_compo::convert<FP32, T_DST>
// Fast path for CH_NB == 4 && T_DST == UINT8 packs r|r|r|alpha into one UINT32 via PACK_RGBA. Stripe parallel.
// r_to_rgb / r_to_rgba are thin inline wrappers in img_convert.h.
template< typename T_SRC, typename T_DST, int CH_NB, bool B_FORCE_ALPHA >
void	imgcon::r_to_rgbx( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	static_assert( CH_NB == 3 || CH_NB == 4, "r_to_rgbx CH_NB must be 3 (RGB) or 4 (RGBA)" );
	static_assert( !B_FORCE_ALPHA || CH_NB == 4, "B_FORCE_ALPHA only makes sense when CH_NB == 4" );

	T_DST alpha_dst{};
	if constexpr (CH_NB == 4)
	{
		FP32 CONST a_fp = B_FORCE_ALPHA ? options.alpha_fp32 : FP32(1);
		alpha_dst = aaa::img::c_compo::convert<FP32, T_DST>( a_fp );
	}

	imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
	{
		IMGCON_PIXEL_LOOP( T_SRC, T_DST, options.sx )
		{
			if constexpr (CH_NB == 4 && std::is_same_v<T_DST, UINT8>)
			{
				// Fast path : 1 UINT32 store via PACK_RGBA (saves 3 byte stores per pixel).
				UINT8 CONST v = aaa::img::c_compo::convert<T_SRC, UINT8>( *p_src++ );
				*reinterpret_cast<UINT32*>(p_dst) = PACK_RGBA( v, v, v, alpha_dst );
			}
			else
			{
				T_DST CONST v = aaa::img::c_compo::convert<T_SRC, T_DST>( *p_src++ );
				p_dst[0] = v;
				p_dst[1] = v;
				p_dst[2] = v;
				if constexpr (CH_NB == 4)
					p_dst[3] = alpha_dst;
			}
			p_dst += CH_NB;
		}
	});
}

// Explicit instantiations for r_to_rgbx. Valid (CH_NB, B_FORCE_ALPHA) combos : (3, false), (4, false), (4, true) = 3.
// With 4 T_SRC * 4 T_DST that gives 48 specialisations.
#define INSTANCE_R_TO_RGBX(T_SRC, CH_NB, B_FORCE_ALPHA, T_DST) \
	template void imgcon::r_to_rgbx<T_SRC, T_DST, CH_NB, B_FORCE_ALPHA>( UINT8 CONST * RESTRICT, INT32 CONST, T_DST* RESTRICT, INT32 CONST, st_img_conv CONST & )

#define INSTANCE_R_TO_RGBX_ALL_DST(T_SRC, CH_NB, B_FORCE_ALPHA)	INSTANCE_FOREACH_T_DST( INSTANCE_R_TO_RGBX, T_SRC, CH_NB, B_FORCE_ALPHA )

#define INSTANCE_R_TO_RGBX_ALL(T_SRC) \
	INSTANCE_R_TO_RGBX_ALL_DST(T_SRC, 3, false); \
	INSTANCE_R_TO_RGBX_ALL_DST(T_SRC, 4, false); \
	INSTANCE_R_TO_RGBX_ALL_DST(T_SRC, 4, true )

INSTANCE_R_TO_RGBX_ALL(UINT8 );
INSTANCE_R_TO_RGBX_ALL(UINT16);
INSTANCE_R_TO_RGBX_ALL(FP16  );
INSTANCE_R_TO_RGBX_ALL(FP32  );

#undef INSTANCE_R_TO_RGBX_ALL
#undef INSTANCE_R_TO_RGBX_ALL_DST
#undef INSTANCE_R_TO_RGBX


// R-source replicated to RG(2) dst : per pixel, read R, apply c_compo::convert<T_SRC, T_DST>, write it on both R and G lanes.
// Matches the R-replicate semantics of r_to_rgb / r_to_rgba (consistent with OpenGL luminance sampling). Stripe parallel.
template< typename T_SRC, typename T_DST >
void	imgcon::r_to_rg( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
	{
		IMGCON_PIXEL_LOOP( T_SRC, T_DST, options.sx )
		{
			T_DST CONST v = aaa::img::c_compo::convert<T_SRC, T_DST>( *p_src++ );
			p_dst[0] = v;
			p_dst[1] = v;
			p_dst += 2;
		}
	});
}

// Explicit instantiations for r_to_rg : 4 T_SRC * 4 T_DST = 16 combinations.
#define INSTANCE_R_TO_RG(T_SRC, T_DST) \
	template void imgcon::r_to_rg<T_SRC, T_DST>( UINT8 CONST * RESTRICT, INT32 CONST, T_DST* RESTRICT, INT32 CONST, st_img_conv CONST & )

#define INSTANCE_R_TO_RG_ALL_DST(T_SRC)		INSTANCE_FOREACH_T_DST( INSTANCE_R_TO_RG, T_SRC )

INSTANCE_R_TO_RG_ALL_DST(UINT8 );
INSTANCE_R_TO_RG_ALL_DST(UINT16);
INSTANCE_R_TO_RG_ALL_DST(FP16  );
INSTANCE_R_TO_RG_ALL_DST(FP32  );

#undef INSTANCE_R_TO_RG_ALL_DST
#undef INSTANCE_R_TO_RG


// RG-source pass-through to RGB(3) or RGBA(4) dst : per pixel, read R and G, apply c_compo::convert<T_SRC, T_DST> on each,
// write (R, G, 0) on the 3 RGB lanes. CH_NB == 4 adds an alpha lane :
//   - B_FORCE_ALPHA = 0 : alpha = opaque
//   - B_FORCE_ALPHA = 1 : alpha = options.alpha_fp32 routed
// Fast path for CH_NB == 4 && T_DST == UINT8 packs r|g|0|alpha into one UINT32 store via PACK_RGBA. Stripe parallel.
// rg_to_rgb / rg_to_rgba are thin inline wrappers in img_convert.h.
template< typename T_SRC, typename T_DST, int CH_NB, bool B_FORCE_ALPHA >
void	imgcon::rg_to_rgbx( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	static_assert( CH_NB == 3 || CH_NB == 4, "rg_to_rgbx CH_NB must be 3 (RGB) or 4 (RGBA)" );
	static_assert( !B_FORCE_ALPHA || CH_NB == 4, "B_FORCE_ALPHA only makes sense when CH_NB == 4" );

	T_DST CONST	zero	= T_DST{};
	T_DST		alpha_dst{};
	if constexpr (CH_NB == 4)
	{
		FP32 CONST a_fp = B_FORCE_ALPHA ? options.alpha_fp32 : FP32(1);
		alpha_dst = aaa::img::c_compo::convert<FP32, T_DST>( a_fp );
	}

	imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
	{
		IMGCON_PIXEL_LOOP( T_SRC, T_DST, options.sx )
		{
			if constexpr (CH_NB == 4 && std::is_same_v<T_DST, UINT8>)
			{
				// Fast path : 1 UINT32 store via PACK_RGBA (saves 3 byte stores per pixel).
				UINT8 CONST r = aaa::img::c_compo::convert<T_SRC, UINT8>( p_src[0] );
				UINT8 CONST g = aaa::img::c_compo::convert<T_SRC, UINT8>( p_src[1] );
				*reinterpret_cast<UINT32*>(p_dst) = PACK_RGBA( r, g, UINT8(0), alpha_dst );
			}
			else
			{
				p_dst[0] = aaa::img::c_compo::convert<T_SRC, T_DST>( p_src[0] );
				p_dst[1] = aaa::img::c_compo::convert<T_SRC, T_DST>( p_src[1] );
				p_dst[2] = zero;
				if constexpr (CH_NB == 4)
					p_dst[3] = alpha_dst;
			}
			p_src += 2;
			p_dst += CH_NB;
		}
	});
}

// Explicit instantiations for rg_to_rgbx. Valid (CH_NB, B_FORCE_ALPHA) combos : (3, false), (4, false), (4, true) = 3.
// With 4 T_SRC * 4 T_DST that gives 48 specialisations.
#define INSTANCE_RG_TO_RGBX(T_SRC, CH_NB, B_FORCE_ALPHA, T_DST) \
	template void imgcon::rg_to_rgbx<T_SRC, T_DST, CH_NB, B_FORCE_ALPHA>( UINT8 CONST * RESTRICT, INT32 CONST, T_DST* RESTRICT, INT32 CONST, st_img_conv CONST & )

#define INSTANCE_RG_TO_RGBX_ALL_DST(T_SRC, CH_NB, B_FORCE_ALPHA)	INSTANCE_FOREACH_T_DST( INSTANCE_RG_TO_RGBX, T_SRC, CH_NB, B_FORCE_ALPHA )

#define INSTANCE_RG_TO_RGBX_ALL(T_SRC) \
	INSTANCE_RG_TO_RGBX_ALL_DST(T_SRC, 3, false); \
	INSTANCE_RG_TO_RGBX_ALL_DST(T_SRC, 4, false); \
	INSTANCE_RG_TO_RGBX_ALL_DST(T_SRC, 4, true )

INSTANCE_RG_TO_RGBX_ALL(UINT8 );
INSTANCE_RG_TO_RGBX_ALL(UINT16);
INSTANCE_RG_TO_RGBX_ALL(FP16  );
INSTANCE_RG_TO_RGBX_ALL(FP32  );

#undef INSTANCE_RG_TO_RGBX_ALL
#undef INSTANCE_RG_TO_RGBX_ALL_DST
#undef INSTANCE_RG_TO_RGBX


// RGB / BGR / RGBA / BGRA source narrowed to RG dst : per pixel, read SRC_CH_NB components, swap R/B when B_SWAP_RB is true
// (so BGR / BGRA become RGB / RGBA before extracting R+G), apply c_compo::convert<T_SRC, T_DST> on R and G, write (R, G) on
// the 2 RG lanes. B and alpha (if present) are dropped. Stripe parallel. rgb_to_rg / rgba_to_rg are thin inline wrappers in
// img_convert.h, symmetric to rgbx_to_r.
template< typename T_SRC, typename T_DST, int SRC_CH_NB, bool B_SWAP_RB >
void	imgcon::rgbx_to_rg( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	static_assert( SRC_CH_NB == 3 || SRC_CH_NB == 4, "rgbx_to_rg SRC_CH_NB must be 3 (RGB / BGR) or 4 (RGBA / BGRA)" );

	imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
	{
		IMGCON_PIXEL_LOOP( T_SRC, T_DST, options.sx )
		{
			if constexpr (B_SWAP_RB)
				p_dst[0] = aaa::img::c_compo::convert<T_SRC, T_DST>( p_src[2] );	// R is at index 2 for BGR / BGRA
			else
				p_dst[0] = aaa::img::c_compo::convert<T_SRC, T_DST>( p_src[0] );
			p_dst[1] = aaa::img::c_compo::convert<T_SRC, T_DST>( p_src[1] );	// G is always at index 1
			p_src += SRC_CH_NB;
			p_dst += 2;
		}
	});
}

// Explicit instantiations for rgbx_to_rg. 4 T_SRC * 2 SRC_CH_NB * 2 B_SWAP_RB * 4 T_DST = 64 combinations covering
// RGB / BGR / RGBA / BGRA sources to RG dsts.
#define INSTANCE_RGBX_TO_RG(T_SRC, SRC_CH_NB, B_SWAP_RB, T_DST) \
	template void imgcon::rgbx_to_rg<T_SRC, T_DST, SRC_CH_NB, B_SWAP_RB>( UINT8 CONST * RESTRICT, INT32 CONST, T_DST* RESTRICT, INT32 CONST, st_img_conv CONST & )

#define INSTANCE_RGBX_TO_RG_ALL_DST(T_SRC, SRC_CH_NB, B_SWAP_RB)	INSTANCE_FOREACH_T_DST( INSTANCE_RGBX_TO_RG, T_SRC, SRC_CH_NB, B_SWAP_RB )

#define INSTANCE_RGBX_TO_RG_ALL(T_SRC) \
	INSTANCE_RGBX_TO_RG_ALL_DST(T_SRC, 3, false); \
	INSTANCE_RGBX_TO_RG_ALL_DST(T_SRC, 3, true ); \
	INSTANCE_RGBX_TO_RG_ALL_DST(T_SRC, 4, false); \
	INSTANCE_RGBX_TO_RG_ALL_DST(T_SRC, 4, true )

INSTANCE_RGBX_TO_RG_ALL(UINT8 );
INSTANCE_RGBX_TO_RG_ALL(UINT16);
INSTANCE_RGBX_TO_RG_ALL(FP16  );
INSTANCE_RGBX_TO_RG_ALL(FP32  );

#undef INSTANCE_RGBX_TO_RG_ALL
#undef INSTANCE_RGBX_TO_RG_ALL_DST
#undef INSTANCE_RGBX_TO_RG


// RG source narrowed to R dst : per pixel, read R only (G is dropped), apply c_compo::convert<T_SRC, T_DST>, write to R lane.
// No swap variant : RG has no B/A and the R / G ordering is fixed by the format. Stripe parallel.
template< typename T_SRC, typename T_DST >
void	imgcon::rg_to_r( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
	{
		IMGCON_PIXEL_LOOP( T_SRC, T_DST, options.sx )
		{
			*p_dst++ = aaa::img::c_compo::convert<T_SRC, T_DST>( p_src[0] );	// R only ; G is dropped
			p_src += 2;
		}
	});
}

// Explicit instantiations for rg_to_r : 4 T_SRC * 4 T_DST = 16 combinations.
#define INSTANCE_RG_TO_R(T_SRC, T_DST) \
	template void imgcon::rg_to_r<T_SRC, T_DST>( UINT8 CONST * RESTRICT, INT32 CONST, T_DST* RESTRICT, INT32 CONST, st_img_conv CONST & )

#define INSTANCE_RG_TO_R_ALL_DST(T_SRC)		INSTANCE_FOREACH_T_DST( INSTANCE_RG_TO_R, T_SRC )

INSTANCE_RG_TO_R_ALL_DST(UINT8 );
INSTANCE_RG_TO_R_ALL_DST(UINT16);
INSTANCE_RG_TO_R_ALL_DST(FP16  );
INSTANCE_RG_TO_R_ALL_DST(FP32  );

#undef INSTANCE_RG_TO_R_ALL_DST
#undef INSTANCE_RG_TO_R


// RGB(x) source reduced to a single R component on T_DST. Per pixel : read 3 (RGB) or 4 (RGBA) T_SRC components (swapped to RGB
// when B_SWAP_RB is true so we always read r, g, b in that order), compute grey via aaa::color::rgb_to_grey, then
// c_compo::convert to T_DST. Alpha is dropped. T_SRC == UINT8 takes the integer fixed-point path (rgb_to_grey returns 0..255,
// then UINT8 -> T_DST LUT); other T_SRC use the FP path (each component to FP32, Rec.601 weighted sum, FP32 -> T_DST). Stripe
// parallel.
template< typename T_SRC, int SRC_CH_NB, typename T_DST, bool B_SWAP_RB >
bool	imgcon::rgbx_to_r( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	static_assert( SRC_CH_NB == 3 || SRC_CH_NB == 4, "rgbx_to_r expects SRC_CH_NB = 3 (RGB / BGR) or 4 (RGBA / BGRA)" );

	imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
	{
		IMGCON_PIXEL_LOOP( T_SRC, T_DST, options.sx )
		{
			if constexpr (std::is_same_v<T_SRC, UINT8>)
			{
				// Integer fixed-point path : rgb_to_grey returns clamped 0..255, route through UINT8 -> T_DST LUT.
				UINT32 grey;
				if constexpr (B_SWAP_RB)
					grey = aaa::color::rgb_to_grey( p_src[2], p_src[1], p_src[0] );
				else
					grey = aaa::color::rgb_to_grey( p_src[0], p_src[1], p_src[2] );
				*p_dst++ = aaa::img::c_compo::convert<UINT8, T_DST>( static_cast<UINT8>(grey) );
			}
			else
			{
				// FP path : convert each component to FP32, Rec.601 weighted sum in fp, then FP32 -> T_DST.
				FP32 fr, fb;
				if constexpr (B_SWAP_RB)
					{ fr = aaa::img::c_compo::convert<T_SRC, FP32>( p_src[2] );	fb = aaa::img::c_compo::convert<T_SRC, FP32>( p_src[0] ); }
				else
					{ fr = aaa::img::c_compo::convert<T_SRC, FP32>( p_src[0] );	fb = aaa::img::c_compo::convert<T_SRC, FP32>( p_src[2] ); }
				FP32 CONST fg    = aaa::img::c_compo::convert<T_SRC, FP32>( p_src[1] );
				FP32 CONST fgrey = aaa::color::rgb_to_grey( fr, fg, fb );
				*p_dst++ = aaa::img::c_compo::convert<FP32, T_DST>( fgrey );
			}
			p_src += SRC_CH_NB;
		}
	});
	return true;
}

// Explicit instantiations for rgbx_to_r. Cascade : for each (T_SRC, B_SWAP_RB) combo, emit 2 SRC_CH_NB * 4 T_DST = 8 instantiations.
// Total : 4 T_SRC * 2 B_SWAP_RB * 2 CH_NB * 4 T_DST = 64. The r8 dst column is dead (rgba8_to_r8 family covers it via dedicated
// SIMD kernels) but included for symmetry.
#define INSTANCE_RGBX_TO_R(T_SRC, SRC_CH_NB, B_SWAP_RB, T_DST) \
	template bool imgcon::rgbx_to_r<T_SRC, SRC_CH_NB, T_DST, B_SWAP_RB>( UINT8 CONST * RESTRICT, INT32 CONST, T_DST* RESTRICT, INT32 CONST, st_img_conv CONST & )

#define INSTANCE_RGBX_TO_R_ALL_DST(T_SRC, SRC_CH_NB, B_SWAP_RB)	INSTANCE_FOREACH_T_DST( INSTANCE_RGBX_TO_R, T_SRC, SRC_CH_NB, B_SWAP_RB )

#define INSTANCE_RGBX_TO_R_ALL_DST_CHN(T_SRC, B_SWAP_RB) \
	INSTANCE_RGBX_TO_R_ALL_DST(T_SRC, 3, B_SWAP_RB); \
	INSTANCE_RGBX_TO_R_ALL_DST(T_SRC, 4, B_SWAP_RB)

INSTANCE_RGBX_TO_R_ALL_DST_CHN(UINT8 , false);
INSTANCE_RGBX_TO_R_ALL_DST_CHN(UINT8 , true );
INSTANCE_RGBX_TO_R_ALL_DST_CHN(UINT16, false);
INSTANCE_RGBX_TO_R_ALL_DST_CHN(UINT16, true );
INSTANCE_RGBX_TO_R_ALL_DST_CHN(FP16  , false);
INSTANCE_RGBX_TO_R_ALL_DST_CHN(FP16  , true );
INSTANCE_RGBX_TO_R_ALL_DST_CHN(FP32  , false);
INSTANCE_RGBX_TO_R_ALL_DST_CHN(FP32  , true );

#undef INSTANCE_RGBX_TO_R_ALL_DST_CHN
#undef INSTANCE_RGBX_TO_R_ALL_DST
#undef INSTANCE_RGBX_TO_R


// RGB / BGR / RGBA / BGRA conversion across the whole RGB, family : 3<->3, 4<->4, 3->4 widen, 4->3 narrow. Per pixel reads
// SRC_CH_NB components, optionally swaps R/B, converts each via c_compo::convert<T_SRC, T_DST>, writes DST_CH_NB components.
// Alpha behaviour by (SRC_CH_NB, DST_CH_NB, B_FORCE_ALPHA) :
//   - DST == 3                    : no alpha lane written.
//   - SRC == 3, DST == 4, FORCE=0 : alpha = opaque (c_compo::convert<FP32, T_DST>(1.f)).
//   - SRC == 3, DST == 4, FORCE=1 : alpha = options.alpha_fp32 routed through c_compo::convert<FP32, T_DST>.
//   - SRC == 4, DST == 4, FORCE=0 : alpha pass, through from p_src[3] via c_compo::convert<T_SRC, T_DST>.
//   - SRC == 4, DST == 4, FORCE=1 : alpha = options.alpha (UINT8 dst) or options.alpha_fp32 routed (other dsts).
// Fast paths : DST_CH_NB == 4 && T_DST == UINT8 packs 4 bytes via PACK_RGBA (single 32, bit store) ; SRC_CH_NB == 4 &&
// T_SRC == UINT8 reads via UNPACK_RGBA / UNPACK_BGRA to coalesce the 4, byte load. Stripe parallel.
template< typename T_SRC, typename T_DST, int SRC_CH_NB, int DST_CH_NB, bool B_SWAP_RB, bool B_FORCE_ALPHA >
void	imgcon::rgbx_to_rgbx( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	static_assert( SRC_CH_NB == 3 || SRC_CH_NB == 4, "rgbx_to_rgbx SRC_CH_NB must be 3 (RGB / BGR) or 4 (RGBA / BGRA)" );
	static_assert( DST_CH_NB == 3 || DST_CH_NB == 4, "rgbx_to_rgbx DST_CH_NB must be 3 (RGB / BGR) or 4 (RGBA / BGRA)" );
	static_assert( !B_FORCE_ALPHA || DST_CH_NB == 4, "B_FORCE_ALPHA only makes sense when DST_CH_NB == 4" );

	// Precompute the dst, typed alpha once when relevant. Three regimes are folded into a single value : SRC=3 widen
	// (alpha = options.alpha_fp32 if FORCE else opaque 1.f), SRC=4 FORCE (override with options.alpha[_fp32]). The
	// remaining SRC=4 !FORCE case uses pass, through from p_src[3] and ignores alpha_dst.
	T_DST alpha_dst{};
	if constexpr (DST_CH_NB == 4)
	{
		if constexpr (SRC_CH_NB == 3)
		{
			FP32 CONST a_fp = B_FORCE_ALPHA ? options.alpha_fp32 : FP32(1);
			alpha_dst = aaa::img::c_compo::convert<FP32, T_DST>( a_fp );
		}
		else if constexpr (B_FORCE_ALPHA)
		{
			if constexpr (std::is_same_v<T_DST, UINT8>)
				alpha_dst = options.alpha;
			else
				alpha_dst = aaa::img::c_compo::convert<FP32, T_DST>( options.alpha_fp32 );
		}
	}

	imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
	{
		IMGCON_PIXEL_LOOP( T_SRC, T_DST, options.sx )
		{
			if constexpr (SRC_CH_NB == 4 && DST_CH_NB == 4 && std::is_same_v<T_SRC, UINT8> && std::is_same_v<T_DST, UINT8>)
			{
				// Fastest path : u8 src and u8 dst (4 -> 4), no per-component convert needed. Read 1 UINT32, write 1 UINT32
				// (UNPACK + PACK on swap or force, plain UINT32 copy otherwise).
				if constexpr (B_SWAP_RB)
				{
					UINT8 r,g,b, a;
					UNPACK_BGRA( *reinterpret_cast<UINT32 CONST*>(p_src), b,g,r, a );
					if constexpr (B_FORCE_ALPHA)
						*reinterpret_cast<UINT32*>(p_dst) = PACK_RGBA( r,g,b, alpha_dst );
					else
						*reinterpret_cast<UINT32*>(p_dst) = PACK_RGBA( r,g,b, a );
				}
				else if constexpr (B_FORCE_ALPHA)
				{
					UINT8 r,g,b, a;
					UNPACK_RGBA( *reinterpret_cast<UINT32 CONST*>(p_src), r,g,b, a );
					*reinterpret_cast<UINT32*>(p_dst) = PACK_RGBA( r,g,b, alpha_dst );
				}
				else
					*reinterpret_cast<UINT32*>(p_dst) = *reinterpret_cast<UINT32 CONST*>(p_src);
			}
			else if constexpr (SRC_CH_NB == 4 && DST_CH_NB == 3 && std::is_same_v<T_SRC, UINT8>)
			{
				// 4 -> 3 narrow with u8 source : 1 UINT32 load via UNPACK_RGBA / UNPACK_BGRA, alpha byte discarded.
				UINT8 r,g,b, a;
				if constexpr (B_SWAP_RB)
					UNPACK_BGRA( *reinterpret_cast<UINT32 CONST*>(p_src), b,g,r, a );
				else
					UNPACK_RGBA( *reinterpret_cast<UINT32 CONST*>(p_src), r,g,b, a );
				(void)a;	// discarded
				p_dst[0] = aaa::img::c_compo::convert<UINT8, T_DST>( r );
				p_dst[1] = aaa::img::c_compo::convert<UINT8, T_DST>( g );
				p_dst[2] = aaa::img::c_compo::convert<UINT8, T_DST>( b );
			}
			else if constexpr (DST_CH_NB == 4 && std::is_same_v<T_DST, UINT8>)
			{
				// DST == RGBA8 fast path : pack the 4 bytes into one UINT32 store via PACK_RGBA.
				UINT8 r,g,b, a;
				if constexpr (B_SWAP_RB)
					{ r = aaa::img::c_compo::convert<T_SRC, UINT8>( p_src[2] );	b = aaa::img::c_compo::convert<T_SRC, UINT8>( p_src[0] ); }
				else
					{ r = aaa::img::c_compo::convert<T_SRC, UINT8>( p_src[0] );	b = aaa::img::c_compo::convert<T_SRC, UINT8>( p_src[2] ); }
				g = aaa::img::c_compo::convert<T_SRC, UINT8>( p_src[1] );
				if constexpr (SRC_CH_NB == 4 && !B_FORCE_ALPHA)
					a = aaa::img::c_compo::convert<T_SRC, UINT8>( p_src[3] );
				else
					a = alpha_dst;
				*reinterpret_cast<UINT32*>(p_dst) = PACK_RGBA( r,g,b, a );
			}
			else
			{
				// Generic scalar path : per, component convert with optional swap and optional alpha lane.
				if constexpr (B_SWAP_RB)
					{ p_dst[0] = aaa::img::c_compo::convert<T_SRC, T_DST>( p_src[2] ); p_dst[2] = aaa::img::c_compo::convert<T_SRC, T_DST>( p_src[0] ); }
				else
					{ p_dst[0] = aaa::img::c_compo::convert<T_SRC, T_DST>( p_src[0] ); p_dst[2] = aaa::img::c_compo::convert<T_SRC, T_DST>( p_src[2] ); }
				p_dst[1] = aaa::img::c_compo::convert<T_SRC, T_DST>( p_src[1] );
				if constexpr (DST_CH_NB == 4)
				{
					if constexpr (SRC_CH_NB == 4 && !B_FORCE_ALPHA)
						p_dst[3] = aaa::img::c_compo::convert<T_SRC, T_DST>( p_src[3] );
					else
						p_dst[3] = alpha_dst;
				}
			}
			p_src += SRC_CH_NB;
			p_dst += DST_CH_NB;
		}
	});
}

// Explicit instantiations for rgbx_to_rgbx. Valid (SRC_CH, DST_CH, B_FORCE_ALPHA) combos : (3,3,false), (4,4,false),
// (4,4,true), (3,4,false), (3,4,true), (4,3,false) = 6. With 4 T_SRC * 4 T_DST * 2 B_SWAP_RB that gives 192 specialisations.
#define INSTANCE_RGBX_TO_RGBX(T_SRC, SRC_CH_NB, DST_CH_NB, B_SWAP_RB, B_FORCE_ALPHA, T_DST) \
	template void imgcon::rgbx_to_rgbx<T_SRC, T_DST, SRC_CH_NB, DST_CH_NB, B_SWAP_RB, B_FORCE_ALPHA>( UINT8 CONST * RESTRICT, INT32 CONST, T_DST* RESTRICT, INT32 CONST, st_img_conv CONST & )

#define INSTANCE_RGBX_TO_RGBX_ALL_DST(T_SRC, SRC_CH_NB, DST_CH_NB, B_SWAP_RB, B_FORCE_ALPHA) \
	INSTANCE_FOREACH_T_DST( INSTANCE_RGBX_TO_RGBX, T_SRC, SRC_CH_NB, DST_CH_NB, B_SWAP_RB, B_FORCE_ALPHA )

#define INSTANCE_RGBX_TO_RGBX_ALL(T_SRC, B_SWAP_RB) \
	INSTANCE_RGBX_TO_RGBX_ALL_DST(T_SRC, 3, 3, B_SWAP_RB, false); \
	INSTANCE_RGBX_TO_RGBX_ALL_DST(T_SRC, 4, 4, B_SWAP_RB, false); \
	INSTANCE_RGBX_TO_RGBX_ALL_DST(T_SRC, 4, 4, B_SWAP_RB, true ); \
	INSTANCE_RGBX_TO_RGBX_ALL_DST(T_SRC, 3, 4, B_SWAP_RB, false); \
	INSTANCE_RGBX_TO_RGBX_ALL_DST(T_SRC, 3, 4, B_SWAP_RB, true ); \
	INSTANCE_RGBX_TO_RGBX_ALL_DST(T_SRC, 4, 3, B_SWAP_RB, false)

INSTANCE_RGBX_TO_RGBX_ALL(UINT8 , false);
INSTANCE_RGBX_TO_RGBX_ALL(UINT8 , true );
INSTANCE_RGBX_TO_RGBX_ALL(UINT16, false);
INSTANCE_RGBX_TO_RGBX_ALL(UINT16, true );
INSTANCE_RGBX_TO_RGBX_ALL(FP16  , false);
INSTANCE_RGBX_TO_RGBX_ALL(FP16  , true );
INSTANCE_RGBX_TO_RGBX_ALL(FP32  , false);
INSTANCE_RGBX_TO_RGBX_ALL(FP32  , true );

#undef INSTANCE_RGBX_TO_RGBX_ALL
#undef INSTANCE_RGBX_TO_RGBX_ALL_DST
#undef INSTANCE_RGBX_TO_RGBX



//todo generic fn using a byte_to_copy_per_line argument ?
void	imgcon::memcpy_image( void CONST * src, INT32 CONST src_pitch, void* dst, INT32 CONST dst_pitch, INT32 CONST sy )
{
	//todo refine, min of src_pitch and dst_pitch or more parameter or check
	if( dst_pitch > 0 && src_pitch == dst_pitch )
		MEMCPY( dst, src, size_t(dst_pitch) * sy, __FUNCTION__ );
	else
	{
		INT32 byte_to_copy_per_line = MIN( src_pitch, ABS(dst_pitch) );
		imgcon::call_by_line_striped( sy, [&](INT32 j) NOEXCEPT
		{ MEMCPY((UINT8*)dst + (j * dst_pitch), (UINT8 CONST*)src + (j * src_pitch), byte_to_copy_per_line, __FUNCTION__); } );
	}
}


//
// u8 -> u16 unorm widening (b -> b*257 = (b<<8)|b). Covers R_8 -> R_16,
// RG_8 -> RG_16, RGB_8 -> RGB_16, RGBA_8 -> RGBA_16 via ch_nb.
// No channel reorder: BGR/BGRA srcs are NOT supported here, they need a
// swap-aware variant.
//
void	imgcon::uint8_to_uint16( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT16* RESTRICT dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options )
{
	INT32 CONST		byte_nb	= options.sx * ch_nb;

	if( texture_flux_master->is_convert_to_rgb_sse2() )
	{
		SPY_PUSH_RANGE_FUNCTION( spy::IMG_LOW );
			// No parallel::call here: the SSE2 unpack pattern saturates memory bandwidth on a single core.
			INT32 CONST		blocks	= byte_nb / 16;
			INT32 CONST		rem		= byte_nb - blocks * 16;
			for( INT32 j = 0; j < options.sy; ++j )
			{
				IMGCON_PIXEL_LOOP( UINT8, UINT16, blocks )
				{
					__m128i v  = _mm_loadu_si128( (__m128i CONST *)p_src );
					// Interleave each byte with itself: read as u16, each lane = b*256 + b = b * 257 (standard u8 -> u16 unorm).
					__m128i lo = _mm_unpacklo_epi8( v, v );
					__m128i hi = _mm_unpackhi_epi8( v, v );
					_mm_storeu_si128( (__m128i*)(p_dst + 0), lo );
					_mm_storeu_si128( (__m128i*)(p_dst + 8), hi );
					p_src += 16;
					p_dst += 16;
				}
				for( INT32 i = rem; i > 0; --i )
					*p_dst++ = UINT16( UINT16(*p_src++) * 257u );
			}
		SPY_POP_RANGE();
	}
	else
	{
		// Scalar fallback: delegate to the per-component widen template.
		// c_compo::convert<UINT8,UINT16> is the same b -> b*257 = (b<<8)|b unorm widening.
		convert_per_compo_stripe<UINT8, UINT16>( src, src_pitch, dst, dst_pitch, ch_nb, options );
	}
}

//
// u16 -> u8 unorm narrowing (LUT-based via c_compo::convert<UINT16,UINT8>, sta_uint16_to_uint8 = (i * 255) / 65535). Covers
// R_16 -> R_8, RG_16 -> RG_8, RGB_16 -> RGB_8 (no swap), RGBA_16 -> RGBA_8 via ch_nb. Scalar only: narrowing is rare; SIMD would
// need _mm_mulhi_epu16( v, 0xff01 ) or similar to match the LUT bit-exact, deferred.
//
void	imgcon::uint16_to_uint8( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options )
{
	convert_per_compo_stripe<UINT16, UINT8>( src, src_pitch, dst, dst_pitch, ch_nb, options );
}

//
// fp16 -> u16 unorm narrowing (CLAMP*65535 via convert<FP16,UINT16>(val) = convert<FP32,UINT16>(to_fp32(val))). Covers R_16FP ->
// R_16, RG_16FP -> RG_16, RGB_16FP -> RGB_16, RGBA_16FP -> RGBA_16 via ch_nb.
//
void	imgcon::fp16_to_uint16( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT16* RESTRICT dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options )
{
	convert_per_compo_stripe<FP16, UINT16>( src, src_pitch, dst, dst_pitch, ch_nb, options );
}

//
// fp32 -> u16 unorm narrowing (CLAMP*65535 via convert<FP32,UINT16>). Covers R_32FP -> R_16, RG_32FP -> RG_16, etc.
//
void	imgcon::fp32_to_uint16( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT16* RESTRICT dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options )
{
	convert_per_compo_stripe<FP32, UINT16>( src, src_pitch, dst, dst_pitch, ch_nb, options );
}


//
// u16 -> fp16 unorm widening. Covers R_16 -> R_16FP, RG_16 -> RG_16FP, RGB_16 -> RGB_16FP, RGBA_16 -> RGBA_16FP via ch_nb.
// SIMD path: 8 u16 / iter, unpack to i32 (SSE2), cvt to fp32, multiply by 1/65535, then cvtps_ph (F16C) packs back to 8 fp16.
// Scalar fallback uses the 256*256 entry LUT via convert_per_compo_stripe.
//
void	imgcon::uint16_to_fp16( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP16* RESTRICT dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options )
{
	if( texture_flux_master->is_convert_to_rgb_sse2() )
	{
		SPY_PUSH_RANGE_FUNCTION( spy::IMG_LOW );
			INT32 CONST		count		= options.sx * ch_nb;
			INT32 CONST		blocks_8	= count / 8;
			INT32 CONST		rem			= count - blocks_8 * 8;
			__m128  CONST	inv			= _mm_set1_ps( 1.f / 65535.f );
			__m128i CONST	zero		= _mm_setzero_si128();

			imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
			{
				IMGCON_PIXEL_LOOP( UINT16, FP16, blocks_8 )
				{
					__m128i v		= _mm_loadu_si128( (__m128i CONST *)p_src );
					__m128i lo_i32	= _mm_unpacklo_epi16( v, zero );
					__m128i hi_i32	= _mm_unpackhi_epi16( v, zero );
					__m128  lo_f	= _mm_mul_ps( _mm_cvtepi32_ps( lo_i32 ), inv );
					__m128  hi_f	= _mm_mul_ps( _mm_cvtepi32_ps( hi_i32 ), inv );
					__m128i lo_h	= _mm_cvtps_ph( lo_f, 0 );	// 4 fp16 in low 64 bits
					__m128i hi_h	= _mm_cvtps_ph( hi_f, 0 );
					_mm_storeu_si128( (__m128i*)p_dst, _mm_unpacklo_epi64( lo_h, hi_h ) );
					p_src += 8;
					p_dst += 8;
				}
				for( INT32 i = rem; i > 0; --i )
					*p_dst++ = aaa::img::c_compo::convert<UINT16, FP16>( *p_src++ );
			});
		SPY_POP_RANGE();
	}
	else
		convert_per_compo_stripe<UINT16, FP16>( src, src_pitch, dst, dst_pitch, ch_nb, options );
}


//
// u16 -> fp32 unorm widening. Covers R_16 -> R_32FP, RG_16 -> RG_32FP, RGB_16 -> RGB_32FP, RGBA_16 -> RGBA_32FP via ch_nb.
// SIMD path: 8 u16 / iter, unpack to i32 (SSE2), cvt to fp32, multiply by 1/65535. Scalar fallback via convert_per_compo_stripe (256*256 LUT).
//
void	imgcon::uint16_to_fp32( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP32* RESTRICT dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options )
{
	if( texture_flux_master->is_convert_to_rgb_sse2() )
	{
		SPY_PUSH_RANGE_FUNCTION( spy::IMG_LOW );
			INT32 CONST		count		= options.sx * ch_nb;
			INT32 CONST		blocks_8	= count / 8;
			INT32 CONST		rem			= count - blocks_8 * 8;
			__m128  CONST	inv			= _mm_set1_ps( 1.f / 65535.f );
			__m128i CONST	zero		= _mm_setzero_si128();

			imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
			{
				IMGCON_PIXEL_LOOP( UINT16, FP32, blocks_8 )
				{
					__m128i v		= _mm_loadu_si128( (__m128i CONST *)p_src );
					__m128i lo_i32	= _mm_unpacklo_epi16( v, zero );
					__m128i hi_i32	= _mm_unpackhi_epi16( v, zero );
					__m128  lo_f	= _mm_mul_ps( _mm_cvtepi32_ps( lo_i32 ), inv );
					__m128  hi_f	= _mm_mul_ps( _mm_cvtepi32_ps( hi_i32 ), inv );
					_mm_storeu_ps( p_dst + 0, lo_f );
					_mm_storeu_ps( p_dst + 4, hi_f );
					p_src += 8;
					p_dst += 8;
				}
				for( INT32 i = rem; i > 0; --i )
					*p_dst++ = aaa::img::c_compo::convert<UINT16, FP32>( *p_src++ );
			});
		SPY_POP_RANGE();
	}
	else
		convert_per_compo_stripe<UINT16, FP32>( src, src_pitch, dst, dst_pitch, ch_nb, options );
}


//
// u8 -> fp16
//
void	imgcon::uint8_to_fp16( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP16* RESTRICT dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options )
{
	convert_per_compo_stripe<UINT8, FP16>( src, src_pitch, dst, dst_pitch, ch_nb, options );
}

//
// u8 -> fp32
//
void	imgcon::uint8_to_fp32( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP32* RESTRICT dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options )
{
#if AAA_WIN64()
	if( texture_flux_master->is_convert_to_rgb_sse2() )
	{
		SPY_PUSH_RANGE_FUNCTION( spy::IMG_LOW );
			__m128 CONST	inv_255	= _mm_set_ps1( 1.f / 255.f );
			INT32 CONST		blocks	= (options.sx * ch_nb) / 16;
			INT32 CONST		rem		= (options.sx * ch_nb) - blocks * 16;
			imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
			{
				IMGCON_PIXEL_LOOP( UINT8, FP32, blocks )
				{
					auto	out1	= _mm_cvtepi32_ps( _mm_cvtepu8_epi32( _mm_cvtsi32_si128( *reinterpret_cast<INT32 CONST*>(p_src + 0 ) ) ) );
					auto	out2	= _mm_cvtepi32_ps( _mm_cvtepu8_epi32( _mm_cvtsi32_si128( *reinterpret_cast<INT32 CONST*>(p_src + 4 ) ) ) );
					auto	out3	= _mm_cvtepi32_ps( _mm_cvtepu8_epi32( _mm_cvtsi32_si128( *reinterpret_cast<INT32 CONST*>(p_src + 8 ) ) ) );
					auto	out4	= _mm_cvtepi32_ps( _mm_cvtepu8_epi32( _mm_cvtsi32_si128( *reinterpret_cast<INT32 CONST*>(p_src + 12) ) ) );
					out1 = _mm_mul_ps( out1, inv_255 );
					out2 = _mm_mul_ps( out2, inv_255 );
					out3 = _mm_mul_ps( out3, inv_255 );
					out4 = _mm_mul_ps( out4, inv_255 );

					_mm_storeu_ps( (p_dst + 0 ), out1 );
					_mm_storeu_ps( (p_dst + 4 ), out2 );
					_mm_storeu_ps( (p_dst + 8 ), out3 );
					_mm_storeu_ps( (p_dst + 12), out4 );
					p_src += 16;
					p_dst += 16;
				}
				for( auto i = rem; i > 0; --i )
				{
					p_dst[0] = aaa::img::c_compo::to_fp32( p_src[0] );
					++p_dst;
					++p_src;
				}
			});
		SPY_POP_RANGE();
	}
	else
#endif	//#if AAA_WIN64()
	{
		convert_per_compo_stripe<UINT8, FP32>( src, src_pitch, dst, dst_pitch, ch_nb, options );
	}
}


//
// fp16 -> u8
//
// Pure per, component bulk conversion driven by ch_nb : iterates over byte_nb = sx * ch_nb components without any
// pixel, layout awareness. R/B swap is NOT handled here ; channel, aware swap conversions go through the dedicated
// rgb_to_rgb / rgba_to_rgba templates (img_convert_generic.cpp). Real callers pass ch_nb = 1 (R) or 2 (RG) where
// R/B swap is meaningless.
void	imgcon::fp16_to_uint8( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options )
{
	INT32  CONST	byte_nb = options.sx * ch_nb;
	UINT32 CONST	blocks	= byte_nb / (4);
	UINT32 CONST	rem		= byte_nb - blocks * (4);

	imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
	{
		IMGCON_PIXEL_LOOP( FP16, UINT8, blocks )
		{
			p_dst[0] = aaa::img::c_compo::to_uint8( p_src[0] );
			p_dst[1] = aaa::img::c_compo::to_uint8( p_src[1] );
			p_dst[2] = aaa::img::c_compo::to_uint8( p_src[2] );
			p_dst[3] = aaa::img::c_compo::to_uint8( p_src[3] );
			p_dst += 4;
			p_src += 4;
		}
		for( auto i = rem; i > 0; --i )
			*p_dst++ = aaa::img::c_compo::to_uint8( *p_src++ );
	});
}


//
// fp16 -> fp32
//
void	imgcon::fp16_to_fp32( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP32* RESTRICT dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options )
{
#if AAA_WIN64()
	if( texture_flux_master->is_convert_to_rgb_sse2() )
	{
		SPY_PUSH_RANGE_FUNCTION( spy::IMG_LOW );
			INT32 CONST		blocks	= (options.sx * ch_nb) / 16;
			INT32 CONST		rem		= (options.sx * ch_nb) - blocks * 16;
			imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
			{
				IMGCON_PIXEL_LOOP( FP16, FP32, blocks )
				{
					__m128i in1 = _mm_loadu_si128( (__m128i*)(p_src + 0) );
					__m128i in2 = _mm_loadu_si128( (__m128i*)(p_src + 8) );

					auto	outf1 = _mm_cvtph_ps( in1 );
					auto	outf2 = _mm_cvtph_ps( _mm_unpackhi_epi64( in1, in1 ) );
					auto	outf3 = _mm_cvtph_ps( in2 );
					auto	outf4 = _mm_cvtph_ps( _mm_unpackhi_epi64( in2, in2 ) );

					_mm_storeu_ps( (p_dst + 0), outf1 );
					_mm_storeu_ps( (p_dst + 4), outf2 );
					_mm_storeu_ps( (p_dst + 8), outf3 );
					_mm_storeu_ps( (p_dst + 12), outf4 );

					p_dst += 16;
					p_src += 16;
				}
				for( auto i = rem; i > 0; --i )
					*p_dst++ = aaa::img::c_compo::to_fp32( *p_src++ );
			});
		SPY_POP_RANGE();
	}
	else
#endif	//#if AAA_WIN64()
	{
		convert_per_compo_stripe<FP16, FP32>( src, src_pitch, dst, dst_pitch, ch_nb, options );
	}
}


#if AAA_USE_AVX2()
static FINLINE void	convert_float_uint( CONST float * CONST RESTRICT src, std::uint8_t * RESTRICT dst, const std::size_t bytesNb )
{
	__m256  x255 = _mm256_set1_ps(255.f);       // Load the floats
	auto * RESTRICT p_src = src;
	auto * RESTRICT p_dst = dst;
	for( auto k = bytesNb >> 5; k > 0; --k )	// bytesNb / 32
	{
		__m256i a		= _mm256_cvtps_epi32( _mm256_mul_ps(_mm256_loadu_ps(p_src),			x255) );
		__m256i b		= _mm256_cvtps_epi32( _mm256_mul_ps(_mm256_loadu_ps(p_src + 8),		x255) );
		__m256i c		= _mm256_cvtps_epi32( _mm256_mul_ps(_mm256_loadu_ps(p_src + 16),	x255) );
		__m256i d		= _mm256_cvtps_epi32( _mm256_mul_ps(_mm256_loadu_ps(p_src + 24),	x255) );
		__m256i ab		= _mm256_packus_epi32( a, b );
		__m256i cd		= _mm256_packus_epi32( c, d );
		__m256i abcd	= _mm256_packus_epi16( ab, cd );
		__m256i lanefix = _mm256_permutevar8x32_epi32( abcd, _mm256_setr_epi32(0, 4, 1, 5, 2, 6, 3, 7) );
		_mm256_storeu_si256((__m256i*)p_dst, lanefix);
		p_src += 32;
		p_dst += 32;
	}
	for( INT32 k = bytesNb & 0x1f; k > 0 ; --k )	// bytesNb % 32
		*p_dst++ = aaa::img::c_compo::to_uint8( *p_src++ );
}
#endif	//AAA_USE_AVX2()


//
// fp32 -> u8
//
void	imgcon::fp32_to_uint8( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options )
{
	INT32 byte_nb = options.sx * ch_nb;
#if AAA_USE_AVX2()
	if( c_cpu::one->is_use_AVX2() )
	{
		imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
		{
			IMGCON_LINE_PTRS( FP32, UINT8 );
			convert_float_uint( p_src, p_dst, byte_nb );
		});
	}
	else
#endif	//AAA_USE_AVX2()
	if( texture_flux_master->is_convert_to_rgb_sse2() )
	{
		// SSE2 / SSE4.1 fallback for CPUs without AVX2 : 32 components / iter. Same as the AVX2 path semantics
		// (no swap, no force_alpha ; per-component mul-255 + cvt-pack). Stripe parallel.
		SPY_PUSH_RANGE_FUNCTION( spy::IMG_LOW );
			__m128 CONST	mul_255	= _mm_set_ps1( 255.f );
			INT32 CONST		blocks	= byte_nb / 32;
			INT32 CONST		rem		= byte_nb - blocks * 32;
			imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
			{
				IMGCON_PIXEL_LOOP( FP32, UINT8, blocks )
				{
					__m128 in1 = _mm_mul_ps( _mm_loadu_ps( p_src +  0 ), mul_255 );
					__m128 in2 = _mm_mul_ps( _mm_loadu_ps( p_src +  4 ), mul_255 );
					__m128 in3 = _mm_mul_ps( _mm_loadu_ps( p_src +  8 ), mul_255 );
					__m128 in4 = _mm_mul_ps( _mm_loadu_ps( p_src + 12 ), mul_255 );
					__m128 in5 = _mm_mul_ps( _mm_loadu_ps( p_src + 16 ), mul_255 );
					__m128 in6 = _mm_mul_ps( _mm_loadu_ps( p_src + 20 ), mul_255 );
					__m128 in7 = _mm_mul_ps( _mm_loadu_ps( p_src + 24 ), mul_255 );
					__m128 in8 = _mm_mul_ps( _mm_loadu_ps( p_src + 28 ), mul_255 );
					__m128i out1 = _mm_packus_epi32( _mm_cvtps_epi32( in1 ), _mm_cvtps_epi32( in2 ) );
					__m128i y2   = _mm_packus_epi32( _mm_cvtps_epi32( in3 ), _mm_cvtps_epi32( in4 ) );
					out1         = _mm_packus_epi16( out1, y2 );
					__m128i out2 = _mm_packus_epi32( _mm_cvtps_epi32( in5 ), _mm_cvtps_epi32( in6 ) );
					__m128i y3   = _mm_packus_epi32( _mm_cvtps_epi32( in7 ), _mm_cvtps_epi32( in8 ) );
					out2         = _mm_packus_epi16( out2, y3 );
					_mm_storeu_si128( (__m128i*)(p_dst +  0), out1 );
					_mm_storeu_si128( (__m128i*)(p_dst + 16), out2 );
					p_src += 32;
					p_dst += 32;
				}
				for( auto i = rem; i > 0; --i )
					*p_dst++ = aaa::img::c_compo::to_uint8( *p_src++ );
			});
		SPY_POP_RANGE();
	}
	else
	{
		// Pure scalar fallback : per, component bulk conversion (no swap, see fp16_to_uint8 header comment).
		UINT32 CONST	blocks	= byte_nb / 4;
		UINT32 CONST	rem		= byte_nb - blocks * (4);
		imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
		{
			IMGCON_PIXEL_LOOP( FP32, UINT8, blocks )
			{
				p_dst[0] = aaa::img::c_compo::to_uint8( p_src[0] );
				p_dst[1] = aaa::img::c_compo::to_uint8( p_src[1] );
				p_dst[2] = aaa::img::c_compo::to_uint8( p_src[2] );
				p_dst[3] = aaa::img::c_compo::to_uint8( p_src[3] );
				p_dst += 4;
				p_src += 4;
			}
			for( auto i = rem; i > 0; --i )
				*p_dst++ = aaa::img::c_compo::to_uint8( *p_src++ );
		});
	}
}


//
// fp32 -> fp16
//
void	imgcon::fp32_to_fp16( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP16* RESTRICT dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options )
{
#if AAA_WIN64()
	if( texture_flux_master->is_convert_to_rgb_sse2() )
	{
		SPY_PUSH_RANGE_FUNCTION( spy::IMG_LOW );
			INT32 CONST blocks = (options.sx * ch_nb) / 16;
			INT32 CONST rem    = (options.sx * ch_nb) - blocks * 16;

			imgcon::call_by_line_striped( options, [&]( INT32 j ) NOEXCEPT
			{
				IMGCON_PIXEL_LOOP( FP32, FP16, blocks )
				{
					__m128 in1 = _mm_loadu_ps( (p_src + 0) );
					__m128 in2 = _mm_loadu_ps( (p_src + 4) );
					__m128 in3 = _mm_loadu_ps( (p_src + 8) );
					__m128 in4 = _mm_loadu_ps( (p_src + 12) );

					auto	out = _mm_unpacklo_epi64( _mm_cvtps_ph( in1, 0 ), _mm_cvtps_ph( in2, 0 ) );
					auto	out3 = _mm_unpacklo_epi64( _mm_cvtps_ph( in3, 0 ), _mm_cvtps_ph( in4, 0 ) );

					_mm_storeu_si128( (__m128i*)(p_dst + 0), out );
					_mm_storeu_si128( (__m128i*)(p_dst + 8), out3 );
					p_dst += 16;
					p_src += 16;
				}
				for( auto i = rem; i > 0; --i )
					*p_dst++ = aaa::img::c_compo::to_fp16( *p_src++ );
			});
		SPY_POP_RANGE();
	}
	else
#endif	//#if AAA_WIN64()
	{
		convert_per_compo_stripe<FP32, FP16>( src, src_pitch, dst, dst_pitch, ch_nb, options );
	}
}


//
// Binary (1 bit per pixel) -> R_8 (0 or 255).
// Kept at the end of the file: it is the odd one out, not part of the per-component widening family.
//
void	imgcon::binary_to_r8( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	auto CONST rem	= options.sx % 8;		// there is 8 pixel by byte, take the remainder
	auto CONST	nb	= options.sx / 8;

	imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
	{
		IMGCON_PIXEL_LOOP( UINT8, UINT8, nb )
		{
			for( auto k = 7; k >= 0; --k )
			{
				p_dst[ 7 - k ] = ((*p_src >> k) & 1) * 255;
			}
			p_dst += 8;
			++p_src;
		}
		// process the remainder
		for( auto k = 0; k < rem; ++k )
			p_dst[ k ] = ((*p_src >> k) & 1) * 255;
	});
}


#undef INSTANCE_FOREACH_T_DST
#undef INSTANCE_EXPAND
