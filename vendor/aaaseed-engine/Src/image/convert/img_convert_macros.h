
#ifdef AAA_IMG_CONVERT_MACROS_H
#error "IMG_CONVERT_MACROS_H included more than once."
#endif
#define AAA_IMG_CONVERT_MACROS_H 1


#ifndef AAA_COMPUTE_PARALLEL_H
#	include "infrastructure/compute_parallel.h"
#endif
#ifndef AAA_IMG_COMPO_H
#	include "image/img_compo.h"
#endif


namespace imgcon
{
	template < typename T >
	FINLINE void	adjust_src_dst( INT32& line_nb, UINT8 CONST * &src, INT32 &src_step, T* &dst, INT32& dst_step, st_img_conv CONST & options )
	{
		if( options.b_do_field )
		{
			line_nb /= 2;
			if( options.b_field_one )
				src += src_step;
			src_step *= 2;
		}
		if( options.b_flip_vert )
		{
			dst = reinterpret_cast<T*> ( (reinterpret_cast<UINT8*>(dst)) + dst_step * (size_t)(line_nb-1) );	//we need size_t cast or 32 bits overflow will fuck it up
			dst_step = -dst_step;
		}
	}

	// Stripe parallel walk over line_nb destination lines. Splits the lines into nb_stripes = thread count (capped
	// to line_nb) stripes, dispatches each stripe to one task, and inside each task calls body(j) for every line
	// index j in the stripe. The per-line body is a callable taking a single INT32 (the destination line index) ;
	// it captures src / dst / pitches / per-template state from the enclosing scope. Used everywhere a per-line
	// dispatch through aaa::parallel::call would otherwise spawn N tasks for short SIMD inner loops.
	//
	// Signedness convention : param and body index are INT32 (matches options.sy / size_y / get_size_y across the
	// codebase, so callers and body lambdas see no implicit conversion). Internal arithmetic is UINT32 : avoids the
	// cast around aaa::parallel::get_thread_nb_def() which returns UINT32, and counts can't be negative anyway.
	// The 3 explicit boundary casts below are intentional and visible.
	template< typename T_BODY >
	FINLINE void	call_by_line_striped( INT32 CONST line_nb, T_BODY CONST & body ) NOEXCEPT
	{
		UINT32 CONST		count		= UINT32( line_nb );
		UINT32 CONST		nb_stripes	= MIN( aaa::parallel::get_thread_nb_def(), count );
		UINT32 CONST		stripe		= (count + nb_stripes - 1) / nb_stripes;
		aaa::parallel::call( nb_stripes, [&](INT32 s) NOEXCEPT
		{
			UINT32 CONST	y_begin	= UINT32( s ) * stripe;
			UINT32 CONST	y_end	= MIN( y_begin + stripe, count );
			for( INT32 j = INT32( y_begin ); j < INT32( y_end ); ++j )
				body( j );
		});
	}

	// Convenience overload that pulls the line count from options.sy (the most common case).
	template< typename T_BODY >
	FINLINE void	call_by_line_striped( st_img_conv CONST & options, T_BODY CONST & body ) NOEXCEPT
	{
		call_by_line_striped( options.sy, body );
	}

	// Per-line inner loops for *_to_rgb8 / *_to_rgba8 downscalers have all been retired in favour of templates declared in
	// img_convert.h : imgcon::r_to_rgb / rg_to_rgb / rgb_to_rgb / rgba_to_rgb (the last 2 via rgbx_to_rgbx / a dedicated template),
	// all in img_convert_generic.cpp with stripe parallel and compile-time swap.

// Per-line preamble used inside a call_by_line_striped( options, [&](INT32 j){ ... } ) body :
// declares p_src / p_dst as typed line pointers at offset j*pitch from the raw src / dst.
// Assumes the enclosing function has 'src' (UINT8 CONST*), 'dst' (any T* castable through UINT8*),
// 'src_pitch' (INT32), 'dst_pitch' (INT32) in scope, and the lambda captures them.
#define IMGCON_LINE_PTRS( T_SRC, T_DST )	\
	auto CONST * RESTRICT p_src = (T_SRC CONST *)  (src + (j * src_pitch));	\
	auto       * RESTRICT p_dst = (T_DST *)((UINT8*)dst + (j * dst_pitch))

// IMGCON_LINE_PTRS followed by a for loop iterating NB times. The body follows in braces. NB is typically
// options.sx (full per pixel walk) but the macro accepts any count : remainder pixels after a SIMD block
// loop, blocks, etc. Used by every uniform per-pixel template in img_convert_generic.cpp and the SIMD
// tails of the other img_convert_*.cpp files.
#define IMGCON_PIXEL_LOOP( T_SRC, T_DST, NB )	\
	IMGCON_LINE_PTRS( T_SRC, T_DST );	\
	for( INT32 i = (NB); i > 0; --i )

// Stripe parallel (one task per thread, contiguous lines per task) instead of one task per line :
// less task overhead and better cache locality on the per-line reads for the simple SSE2/SSE3 inner
// loops bitcon::*_fast / *_slow run for each j.
#define IMGCON_WRAP_CALL_SPEED( the_call, the_args )				\
	{																\
		if( texture_flux_master->is_convert_to_rgb_fast() )			\
			imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT { the_call##_fast##the_args; } );	\
		else														\
			imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT { the_call##_slow##the_args; } );	\
	}

// Caller contract:
//   options.sx / options.sy describe the destination size.
//   When options.b_do_field is true, options.sy is the field height (half of the
//   interlaced source height) and the source buffer is expected to contain
//   2 * options.sy lines packed (field 0 and field 1 interlaced).
// Field handling flow:
//   1. local size_y starts at options.sy (= field / dst height) so the dst image
//      is sized accordingly via is_size_format / init_with_size.
//   2. size_y is then doubled, temporarily representing the full source height,
//      and is passed to imgcon::adjust_src_dst() which halves it back to
//      options.sy while doubling src_step to skip every other line. Net effect:
//      conversion loop iterates options.sy times reading from one field only.
#define IMGCON_BEGIN_COPY( format )															\
	auto CONST	size_x	= options.sx;														\
	auto		size_y	= options.sy;														\
	if( !is_size_format( size_x,size_y, format ) )											\
	{																						\
		if( init_with_size( size_x,size_y, format, options.signature ) != AAA_OK )			\
		{																					\
			ERR_PRINT_STRING( "Error redefining image to %d x %d (%s) in method %s()",		\
						size_x,size_y, aaa::c_pixel_format::get_name(format), options.signature, __FUNCTION__ );	\
			return false;																	\
		}																					\
	}																						\
	SPY_PUSH_RANGE_FUNCTION( spy::IMG_LOW );												\
	TBUF_ADD( tbuf::CH_CAPTURE_CONVERT, 1., __FUNCTION__ );									\
		if( options.b_do_field )															\
			size_y *= 2;																	\
		auto	dst_pitch		= get_byte_pitch();											\
		bool	b_alpha_done	= false;													\
		bool	b_done			= true;

//
//
#define IMGCON_END_COPY_ALPHA()								\
		if( !b_done )										\
			print_err_unsupported_format( __FUNCTION__ );	\
		else												\
		{													\
			set_changed();									\
			if( options.b_force_alpha && !b_alpha_done )	\
				fill_alpha( options.alpha_fp32 );			\
		}													\
	TBUF_ADD( tbuf::CH_CAPTURE_CONVERT, 0., nullptr );		\
	SPY_POP_RANGE();										\
	return b_done;

//
//
#define IMGCON_END_COPY()									\
		if( !b_done )										\
			print_err_unsupported_format( __FUNCTION__ );	\
		else												\
			set_changed();									\
	TBUF_ADD( tbuf::CH_CAPTURE_CONVERT, 0., nullptr );		\
	SPY_POP_RANGE();										\
	return b_done;

};

