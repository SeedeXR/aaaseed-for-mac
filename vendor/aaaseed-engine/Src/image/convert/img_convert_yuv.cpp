
#include "img_convert.h"
#include "img_convert_macros.h"
#include "bitmap_convert.h"
#include "media/video/texture_flux_master.h"


using namespace aaa;


//
// YUYV / UYVY to RGBA8
//
bool	imgcon::yuyv_to_rgba8( UINT8 CONST * src8, INT32 CONST src_pitch, UINT8* dst8, INT32 CONST dst8_pitch, st_img_conv & options, bool b_uyvy )
{
	options.src_a = src8;

	UINT32*	dst32 = (UINT32*)dst8;
	INT32 CONST dst32_pitch = dst8_pitch >> 2;

	INT32 thread_nb = INT32( aaa::parallel::get_thread_nb_def() );
	INT32 line_nb = (options.sy + thread_nb - 1) / thread_nb ;

	//todo test wrong len for avx2
	if( texture_flux_master->is_convert_to_rgb_avx2() )
	{
		line_nb = ( line_nb + 1 ) & 0xfffffffe; // round up to be sure to use an even number because the fn handle 2 lines at a time
		if( b_uyvy )
			parallel::call( thread_nb, [&](INT32 j) NOEXCEPT {
				bitcon::uyvy_to_rgba8_avx2( j*line_nb, line_nb, dst32, dst32_pitch, options ); });
		else
			parallel::call( thread_nb, [&](INT32 j) NOEXCEPT {
				bitcon::yuyv_to_rgba8_avx2( j*line_nb, line_nb, dst32, dst32_pitch, options ); });
	}
	else
		if( b_uyvy )
			parallel::call( thread_nb, [&](INT32 j) NOEXCEPT {
				bitcon::uyvy_to_rgba8_line_block( j*line_nb, line_nb, dst32, dst32_pitch, options ); });
		else
			parallel::call( thread_nb, [&](INT32 j) NOEXCEPT {
				bitcon::yuyv_to_rgba8_line_block( j*line_nb, line_nb, dst32, dst32_pitch, options ); });
	return true;
}

//
// I420 to RGBA and BGRA
//
bool	imgcon::i420_to_rgba8( UINT8 CONST * src8, INT32 CONST src_pitch, UINT8* dst8, INT32 CONST dst8_pitch, st_img_conv & options )
{
	INT32 CONST sx = options.sx;
	auto CONST pixel_nb = sx * options.sy;
	UINT8 CONST * src_u8 = src8 + pixel_nb;
	UINT8 CONST * src_v8 = src_u8 + pixel_nb / 4;
	options.src_a = src8;
	options.src_b = src_u8;
	options.src_c = src_v8;

	UINT32*	dst32 = (UINT32*)dst8;
	INT32 CONST dst32_pitch = dst8_pitch >> 2;

	INT32 thread_nb = INT32( aaa::parallel::get_thread_nb_def() );
	INT32 line_nb = (options.sy + thread_nb - 1) / thread_nb;
	line_nb = ( line_nb + 1 ) & 0xfffffffe; // round up to be sure to use an even number because the fn handle 2 lines at a time

	if( texture_flux_master->is_convert_to_rgb_avx2() )
		parallel::call( thread_nb, [&](INT32 j) NOEXCEPT {
			bitcon::i420_to_rgba8_avx2( j*line_nb, line_nb, dst32, dst32_pitch, options ); });
	else if( texture_flux_master->is_convert_to_rgb_fast() )
		parallel::call( thread_nb, [&](INT32 j) NOEXCEPT {
			bitcon::i420_to_rgba8_line_block( j*line_nb, line_nb, dst32, dst32_pitch, options ); });
	else	// the slow version below is faster on a 4k with or without luma
	{
		INT32 CONST sxh = sx / 2;
		INT32 CONST sxd = sx * 2;
		imgcon::call_by_line_striped( options.sy/2, [&](INT32 j) NOEXCEPT {
			bitcon::i420_to_rgba8_line_two( src8+j*sxd, src_u8+j*sxh, src_v8+j*sxh, dst32+(j*dst32_pitch*2), dst32_pitch, options ); });
	}
	return true;
}

//
// NV12 to RGBA and BGRA
//
bool	imgcon::nv12_to_rgba8( UINT8 CONST * src8, INT32 CONST src_pitch, UINT8* dst8, INT32 CONST dst8_pitch, st_img_conv & options )
{
	INT32 CONST sx = options.sx;
	UINT8 CONST * src_uv8 = src8 + (sx * options.sy);
	options.src_a = src8;
	options.src_b = src_uv8;

	UINT32*	dst32 = (UINT32*)dst8;
	INT32 CONST dst32_pitch = dst8_pitch >> 2;

	INT32 thread_nb = INT32( aaa::parallel::get_thread_nb_def() );
	INT32 line_nb = (options.sy + thread_nb - 1) / thread_nb;
	line_nb = ( line_nb + 1 ) & 0xfffffffe; // round up to be sure to use an even number because the fn handle 2 lines at a time

	if( texture_flux_master->is_convert_to_rgb_avx2() )
		parallel::call( thread_nb, [&](INT32 j) NOEXCEPT {
			bitcon::nv12_to_rgba8_avx2( j*line_nb, line_nb, dst32, dst32_pitch, options ); });
	else if( texture_flux_master->is_convert_to_rgb_fast() )
		parallel::call( thread_nb, [&](INT32 j) NOEXCEPT {
			bitcon::nv12_to_rgba8_line_block( j*line_nb, line_nb, dst32, dst32_pitch, options ); });
	else	// the slow version below is faster on a 4k or HD with or without luma
		imgcon::call_by_line_striped( options.sy/2, [&](INT32 j) NOEXCEPT {
			bitcon::nv12_to_rgba8_line_two( src8+j*2*sx, src_uv8+j*sx, dst32+(j*2*dst32_pitch), dst32_pitch, options ); });
	return true;
}


//
// V210 (Blackmagic 10-bit packed) to BGRA8
//
bool	imgcon::v210_to_bgra8( UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
//if( texture_flux_master->is_convert_to_rgb_sse() )
//{
//	UINT32	flags = build_pixfc_create_flag();
//	if( !_pixfc || _pixfc->source_fmt != PixFcV210 || _pixfc_flag_last != flags )
//	{
//		init_pixfc( PIXEL_FORMAT::V210, size_x, 1, compute_src_byte_per_line( size_x, src_bit_per_pixel ) );
//	}
//	if( _pixfc )
//	{
//		for( INT32 i = size_y; i > 0; --i )
//		{
//			_pixfc->convert( _pixfc, src, dst );
//			src += src_step;
//			dst += dst_step;
//		}
//	}
//}
//else
	imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT {
		bitcon::v210_to_bgra8_slow( src + (j * src_pitch), dst + (j * dst_pitch), options.sx, options.alpha ); }); //todo mot sure of option.sx
	return true;
}


////	bit_align_p2 is the power of 2 to use (the shift)
//static	INT32	compute_src_byte_per_line( INT32 options.sx, INT32 bit_per_pixel )
//{
//	INT32	bit_align_p2 = tex_video_master->is_src_aligned_4() ? 5 : 3;
//	UINT32	mask = (1 << bit_align_p2) - 1;
//	options.sx *= bit_per_pixel;
//	if( options.sx & mask )
//	{
//		options.sx &= ~mask;
//		options.sx += 1 << bit_align_p2;
//	}
//	return options.sx >> 3;
//}
