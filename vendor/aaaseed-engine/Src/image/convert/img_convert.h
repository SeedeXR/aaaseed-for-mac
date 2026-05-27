
#ifdef AAA_IMG_CONVERT_H
#error "IMG_CONVERT_H included more than once."
#endif
#define AAA_IMG_CONVERT_H 1



#ifndef AAA_IMG_UTILS_H
#	include "image/img_utils.h"
#endif


namespace imgcon
{
//	Naming convention :
//		- rgb / rgba / rg / r / bgr / bgra refer to channel layout in the buffer.
//		- Function name often picks one of (rgb / bgr) but the swap, aware ones template on B_SWAP_RB
//		  (driven from options.b_swap_red_blue), so the same function covers both layouts.
//		- bool return value means "alpha has been written to dst" (b_alpha_done).
	extern void memcpy_image(		void  CONST * src,			INT32 CONST src_pitch, void* dst,			 INT32 CONST dst_pitch, INT32 CONST sy );
	extern void uint8_to_uint16(	UINT8 CONST * RESTRICT src,	INT32 CONST src_pitch, UINT16* RESTRICT dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options );
	extern void uint16_to_uint8(	UINT8 CONST * RESTRICT src,	INT32 CONST src_pitch, UINT8*  RESTRICT dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options );
	extern void fp16_to_uint16(		UINT8 CONST * RESTRICT src,	INT32 CONST src_pitch, UINT16* RESTRICT dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options );
	extern void fp32_to_uint16(		UINT8 CONST * RESTRICT src,	INT32 CONST src_pitch, UINT16* RESTRICT dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options );

	// R-source replicated to RGB(3) or RGBA(4) dst : reads the single R component and writes it on the 3 RGB lanes via
	// c_compo::convert<T_SRC, T_DST>. CH_NB == 4 adds an alpha lane (opaque if !B_FORCE_ALPHA, options.alpha_fp32 routed
	// through c_compo::convert<FP32, T_DST> otherwise). Fast path for CH_NB == 4 && T_DST == UINT8 packs r|r|r|alpha into
	// one UINT32 store via PACK_RGBA. Body and explicit instantiations live in img_convert_generic.cpp.
	template< typename T_SRC, typename T_DST, int CH_NB, bool B_FORCE_ALPHA >
	void	r_to_rgbx(				UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options );

	// Thin inline wrappers around r_to_rgbx, named after CH_NB. r_to_rgba returns true (b_alpha_done) and runtime, dispatches
	// on options.b_force_alpha to pick one of the 2 specialisations.
	template< typename T_SRC, typename T_DST >
	FINLINE void	r_to_rgb(		UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
	{	r_to_rgbx<T_SRC, T_DST, 3, false>( src, src_pitch, dst, dst_pitch, options );	}
	template< typename T_SRC, typename T_DST >
	FINLINE bool	r_to_rgba(		UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
	{
		if( options.b_force_alpha )	r_to_rgbx<T_SRC, T_DST, 4, true >( src, src_pitch, dst, dst_pitch, options );
		else						r_to_rgbx<T_SRC, T_DST, 4, false>( src, src_pitch, dst, dst_pitch, options );
		return true;
	}

	// R-source replicated to RG(2) dst : reads the single R component and writes it on both R and G lanes via
	// c_compo::convert<T_SRC, T_DST>. Matches the R-replicate semantics of r_to_rgb / r_to_rgba (consistent with OpenGL's
	// luminance-like sampling). Stripe parallel.
	template< typename T_SRC, typename T_DST >
	void	r_to_rg(				UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options );

	// RG-source pass-through to RGB(3) or RGBA(4) dst : reads the 2 R and G components, applies c_compo::convert<T_SRC, T_DST>
	// on each, writes (R, G, 0) on RGB lanes. CH_NB == 4 adds an alpha lane (opaque or options.alpha_fp32 routed per
	// B_FORCE_ALPHA). Fast path for CH_NB == 4 && T_DST == UINT8 packs r|g|0|alpha into one UINT32 via PACK_RGBA.
	// Body and explicit instantiations live in img_convert_generic.cpp.
	template< typename T_SRC, typename T_DST, int CH_NB, bool B_FORCE_ALPHA >
	void	rg_to_rgbx(				UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options );

	// Thin inline wrappers around rg_to_rgbx. rg_to_rgba returns true and runtime, dispatches on options.b_force_alpha.
	template< typename T_SRC, typename T_DST >
	FINLINE void	rg_to_rgb(		UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
	{	rg_to_rgbx<T_SRC, T_DST, 3, false>( src, src_pitch, dst, dst_pitch, options );	}
	template< typename T_SRC, typename T_DST >
	FINLINE bool	rg_to_rgba(		UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
	{
		if( options.b_force_alpha )	rg_to_rgbx<T_SRC, T_DST, 4, true >( src, src_pitch, dst, dst_pitch, options );
		else						rg_to_rgbx<T_SRC, T_DST, 4, false>( src, src_pitch, dst, dst_pitch, options );
		return true;
	}

	// RGB / BGR / RGBA / BGRA source narrowed to RG dst : per pixel, read SRC_CH_NB components, swap R/B when B_SWAP_RB is
	// true, apply c_compo::convert<T_SRC, T_DST> on R and G, write (R, G) on the 2 RG lanes. B and alpha (if present) are
	// dropped. Stripe parallel. Body and explicit instantiations live in img_convert_generic.cpp. Symmetric to rgbx_to_r.
	template< typename T_SRC, typename T_DST, int SRC_CH_NB, bool B_SWAP_RB >
	void	rgbx_to_rg(				UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options );

	// Thin inline wrappers around rgbx_to_rg with SRC_CH_NB = 3 or 4, for readability at the dispatcher call sites.
	template< typename T_SRC, typename T_DST, bool B_SWAP_RB >
	FINLINE void	rgb_to_rg(		UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
	{	rgbx_to_rg<T_SRC, T_DST, 3, B_SWAP_RB>( src, src_pitch, dst, dst_pitch, options );	}
	template< typename T_SRC, typename T_DST, bool B_SWAP_RB >
	FINLINE void	rgba_to_rg(		UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
	{	rgbx_to_rg<T_SRC, T_DST, 4, B_SWAP_RB>( src, src_pitch, dst, dst_pitch, options );	}

	// RG source narrowed to R dst (drop G). Per pixel : read R component, apply c_compo::convert<T_SRC, T_DST>, write to R lane.
	// G is dropped (no swap variant needed : RG has no B/A and the R / G ordering is fixed by the format). Stripe parallel.
	template< typename T_SRC, typename T_DST >
	void	rg_to_r(				UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options );

	// RGB(x) source reduced to a single R component on T_DST. Per pixel : read 3 or 4 T_SRC components (RGB or RGBA, swapped to
	// RGB when B_SWAP_RB is true), compute grey via aaa::color::rgb_to_grey (Rec.601 weighting), then c_compo::convert to T_DST.
	// Alpha (when present) is dropped. T_SRC == UINT8 uses the integer fixed-point rgb_to_grey + UINT8 -> T_DST LUT path; other
	// T_SRC types convert each component to FP32 and use the FP rgb_to_grey for precision. Stripe parallel.
	template< typename T_SRC, int SRC_CH_NB, typename T_DST, bool B_SWAP_RB >
	bool	rgbx_to_r(				UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options );

	// Thin inline wrappers around rgbx_to_r with SRC_CH_NB = 3 or 4, for readability at the dispatcher call sites.
	template< typename T_SRC, typename T_DST, bool B_SWAP_RB >
	FINLINE bool	rgb_to_r(		UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
	{	return rgbx_to_r<T_SRC, 3, T_DST, B_SWAP_RB>( src, src_pitch, dst, dst_pitch, options );	}
	template< typename T_SRC, typename T_DST, bool B_SWAP_RB >
	FINLINE bool	rgba_to_r(		UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
	{	return rgbx_to_r<T_SRC, 4, T_DST, B_SWAP_RB>( src, src_pitch, dst, dst_pitch, options );	}

	// RGB-family <-> RGB-family conversion (precision widen or narrow, optional R/B swap, optional alpha override or widen).
	// Per pixel : read SRC_CH_NB components (3 = RGB / BGR or 4 = RGBA / BGRA), swap R and B when B_SWAP_RB is true (so BGR /
	// BGRA become RGB / RGBA in dst), apply c_compo::convert<T_SRC, T_DST> on each, write DST_CH_NB components on dst.
	// Alpha rules by (SRC_CH_NB, DST_CH_NB, B_FORCE_ALPHA) :
	//   - DST = 3                    : no alpha lane written (alpha dropped on 4 -> 3 narrow).
	//   - SRC = 3, DST = 4, FORCE=0  : alpha = opaque (c_compo::convert<FP32, T_DST>(1.f)).
	//   - SRC = 3, DST = 4, FORCE=1  : alpha = options.alpha_fp32 routed through c_compo::convert<FP32, T_DST>.
	//   - SRC = 4, DST = 4, FORCE=0  : alpha pass-through from p_src[3].
	//   - SRC = 4, DST = 4, FORCE=1  : alpha = options.alpha (UINT8 dst) or options.alpha_fp32 routed (other dsts).
	// Static_assert in body : B_FORCE_ALPHA implies DST_CH_NB == 4 (force-alpha on a no-alpha dst is meaningless).
	// Fast paths : DST_CH_NB == 4 && T_DST == UINT8 packs the 4 bytes into one UINT32 store via PACK_RGBA. SRC_CH_NB == 4 &&
	// T_SRC == UINT8 reads one UINT32 via UNPACK_RGBA / UNPACK_BGRA. Stripe parallel. Body and explicit instantiations live
	// in img_convert_generic.cpp. Callers should prefer the rgb_to_rgb / rgba_to_rgba / rgba_to_rgb / rgb_to_rgba thin
	// wrappers below for readability.
	template< typename T_SRC, typename T_DST, int SRC_CH_NB, int DST_CH_NB, bool B_SWAP_RB, bool B_FORCE_ALPHA >
	void	rgbx_to_rgbx(			UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options );

	// Thin inline wrappers around rgbx_to_rgbx<...>, named after the (SRC_CH_NB, DST_CH_NB) pair for call-site readability.
	template< typename T_SRC, typename T_DST, bool B_SWAP_RB >
	FINLINE void	rgb_to_rgb(		UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
	{	rgbx_to_rgbx<T_SRC, T_DST, 3, 3, B_SWAP_RB, false>( src, src_pitch, dst, dst_pitch, options );	}
	template< typename T_SRC, typename T_DST, bool B_SWAP_RB, bool B_FORCE_ALPHA = false >
	FINLINE void	rgba_to_rgba(	UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
	{	rgbx_to_rgbx<T_SRC, T_DST, 4, 4, B_SWAP_RB, B_FORCE_ALPHA>( src, src_pitch, dst, dst_pitch, options );	}
	template< typename T_SRC, typename T_DST, bool B_SWAP_RB >
	FINLINE void	rgba_to_rgb(	UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
	{	rgbx_to_rgbx<T_SRC, T_DST, 4, 3, B_SWAP_RB, false>( src, src_pitch, dst, dst_pitch, options );	}
	// rgb_to_rgba widens 3 -> 4 channels. Returns true (b_alpha_done). Alpha picked at runtime from options.b_force_alpha :
	// the wrapper dispatches between the 2 specialisations of rgbx_to_rgbx<...,3,4,...,B_FORCE_ALPHA> once per call.
	template< typename T_SRC, typename T_DST, bool B_SWAP_RB >
	FINLINE bool	rgb_to_rgba(	UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
	{
		if( options.b_force_alpha )	rgbx_to_rgbx<T_SRC, T_DST, 3, 4, B_SWAP_RB, true >( src, src_pitch, dst, dst_pitch, options );
		else						rgbx_to_rgbx<T_SRC, T_DST, 3, 4, B_SWAP_RB, false>( src, src_pitch, dst, dst_pitch, options );
		return true;
	}

	// Runtime dispatch helper : drives the 4 (B_SWAP_RB, B_FORCE_ALPHA) specialisations of rgba_to_rgba once based on
	// options.b_swap_red_blue / options.b_force_alpha. Used by the scalar fallbacks of FP -> u8 / u8 -> FP / FP -> FP RGBA
	// converters whose SIMD path already hoists both branches via templates.
	template< typename T_SRC, typename T_DST >
	FINLINE void	dispatch_rgba_to_rgba(	UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, T_DST* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
	{
		if( options.b_swap_red_blue )
		{
			if( options.b_force_alpha )	rgba_to_rgba<T_SRC, T_DST, true,  true >( src, src_pitch, dst, dst_pitch, options );
			else						rgba_to_rgba<T_SRC, T_DST, true,  false>( src, src_pitch, dst, dst_pitch, options );
		}
		else
		{
			if( options.b_force_alpha )	rgba_to_rgba<T_SRC, T_DST, false, true >( src, src_pitch, dst, dst_pitch, options );
			else						rgba_to_rgba<T_SRC, T_DST, false, false>( src, src_pitch, dst, dst_pitch, options );
		}
	}

// Binary
	extern	void	binary_to_r8(				UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options );

// MONO to RGB
//   The r8/r16/rfp16/rfp32 -> rgb8 scalar paths are served by r_to_rgbx<*, UINT8, 3>
//   (replicate template below); only the SSE3 variant for UINT8 remains as a dedicated kernel.
	extern	void	r8_to_rgb8_sse3(			UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options );

// MONO to RGBA
	extern	void	r8_to_rgba8_sse3(			UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options );
	extern	bool	r8_to_rgba8(				UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options );
	extern	bool	r16_to_rgba8(				UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options );
	extern	bool	disp16_to_rgba8(			UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options );

// RGB to RGB
	extern	void	bgr8_to_rgb8_sse3(			UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options );
	extern	void	bgr8_to_rgb8_fast(			UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options );
	extern	void	bgr8_to_rgb8(				UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options );

// RGB to RGBA
	extern	void	rgb8_to_rgba8_sse3(			UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options );
	extern	void	rgb8_to_rgba8_ssse3_fast(	UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options );
	extern	bool	rgb8_to_rgba8(				UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options );

// BGRA to RGB
	extern	void	rgba8_to_rgb8_sse3(			UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options );
// rgba8_to_rgb8 removed : served by imgcon::rgba_to_rgb<UINT8, UINT8, B_SWAP_RB>. The SSE3 variant above stays.

// RGBA to RGBA
	extern	void	bgra8_to_rgba8_sse2(		UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options );
	extern	void	rgba8_to_rgba8_sse3(		UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options );
	extern	void	rgba8_to_rgba8_sse3_fast(	UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options );
//	extern	bool	rgba8_to_rgba8_sse3_alpha(	UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options );
//	extern	void	bgra_to_rgba_avx2(			UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options );
	// Dispatcher : handles both RGBA_8 -> RGBA_8 (no swap) and BGRA_8 -> RGBA_8 (swap) cases via options.b_swap_red_blue.
	// Picks rgba8_to_rgba8_sse3 / _sse3_fast / bgra8_to_rgba8_sse2 / bitcon::*_fast / memcpy_image based on CPU + options.
	extern	bool	rgba8_to_rgba8(				UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options );

// RGBA to MONO
	extern	void	rgba8_to_r8_fast(			UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options );
	extern	void	rgba8_to_r8_sse2(			UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options );
	extern	void	rgba8_to_r8_sse3(			UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options );
	extern	void	rgba8_to_r8_sse3_fast(		UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options );
//	extern	void	rgba_r8_avx2_fast(			UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options );
	extern	void	rgba8_to_r8(				UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options );


	extern	bool	yuyv_to_rgba8(				UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv & options, bool b_uyvy );
	extern	bool	i420_to_rgba8(				UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv & options );
	extern	bool	nv12_to_rgba8(				UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv & options );

	extern	bool	v210_to_bgra8(				UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options );

//FLOAT part
// n channel
	extern	void	uint8_to_fp16(				UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP16*  RESTRICT dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options );
	extern	void	uint8_to_fp32(				UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP32*  RESTRICT dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options );
	extern	void	uint16_to_fp16(				UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP16*  RESTRICT dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options );
	extern	void	uint16_to_fp32(				UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP32*  RESTRICT dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options );
																									 		 															   
	extern	void	fp16_to_uint8(				UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options );
	extern	void	fp16_to_fp32(				UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP32*  RESTRICT dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options );
																									 		 															   
	extern	void	fp32_to_uint8(				UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options );
	extern	void	fp32_to_fp16(				UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP16*  RESTRICT dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options );
	
	// RGB8
// rgba16_to_rgb8 / rgba16fp_to_rgb8 / rgba32fp_to_rgb8 removed : served by imgcon::rgba_to_rgb<*, UINT8, B_SWAP_RB>.
																									 
// rgb16_to_rgb8 / rgb16fp_to_rgb8 / rgb32fp_to_rgb8 removed : served by imgcon::rgb_to_rgb<*, UINT8, B_SWAP_RB>.

// rg*_to_rgb8 scalar paths are served by rg_to_rgb<*, UINT8> (RG-pass-through template above).

	// RGBA8
	extern	bool	rgba32fp_to_rgba8(			UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options );
	extern	bool	rgba16fp_to_rgba8(			UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options );

	// RGB																							 		 
// bgr8_to_rgb16fp removed : served by imgcon::rgb_to_rgb<UINT8, FP16, true>.
	extern	void	bgr32fp_to_rgb16fp(			UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP16*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options );
																								 		 								   
	extern	void	bgr8_to_rgb32fp(			UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP32*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options );
	extern	void	bgr16fp_to_rgb32fp(			UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP32*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options );
																								 		 								   																									 																								 
	// RGBA																						 		 								   
	extern	bool	rgba8_to_rgba16fp(			UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP16*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options );
	extern	bool	rgba32fp_to_rgba16fp(		UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP16*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options );
													 
													 
	extern	bool	rgba16fp_to_rgba32fp(		UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP32*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options );
	extern	bool	rgba8_to_rgba32fp(			UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP32*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options );

	// same format, optional R/B swap (BGRA -> RGBA via options.b_swap_red_blue) and optional alpha override
	extern	bool	rgba16fp_to_rgba16fp(		UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP16*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options );
	extern	bool	rgba32fp_to_rgba32fp(		UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, FP32*  RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options );
																							 		 					   
//	template< typename T_SRC, typename T_DST >
//			bool	rgba_to_rgb(				T_SRC   CONST * RESTRICT src, INT32 CONST src_pitch, T_DST* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options );

};

