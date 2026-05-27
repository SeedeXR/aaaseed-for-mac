#include "img_convert.h"
#include "img_convert_macros.h"
#include "bitmap_convert.h"
#include "media/video/texture_flux_master.h"


//todo integrate https://github.com/ermig1979/Simd ?

using namespace aaa;


// Templated inner body for r8_to_rgb8_sse3 : B_LUT toggles the per, source LUT lookup. Hoisted out of the per, block
// loop so the cold path doesn't pay a branch every 16 pixels.
template< bool B_LUT >
FINLINE void	r8_to_rgb8_sse3_body( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options ) NOEXCEPT
{
	UINT8 CONST * RESTRICT lut			= options.lut;
	INT16 CONSTEXPR block_byte_nb		=  16;
	INT32 CONST		block_nb			= options.sx / block_byte_nb;
	INT16 CONST		remainder_pixel_nb	= options.sx & (block_byte_nb -1);
	INT32 CONST		x_end				= options.sx * 3;
	INT32 CONST		x_begin				= x_end - remainder_pixel_nb * 3;

	imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
	{
		IMGCON_LINE_PTRS( UINT8, UINT8 );
		// Do conversions in batches of 3 * 16 bytes : 16 pixel at once
		if( block_nb > 0 )
		{
			auto * RESTRICT dst128 = ( __m128i * )p_dst;
			--dst128;
			for( auto loop_nb = block_nb; loop_nb > 0; --loop_nb, p_src += 16 )
			{
				UINT8 g[ 16 ];	// grey, either pass, through from p_src or LUT, mapped
				if constexpr( B_LUT )
				{
					g[0]=lut[p_src[0]];   g[1]=lut[p_src[1]];   g[2]=lut[p_src[2]];   g[3]=lut[p_src[3]];
					g[4]=lut[p_src[4]];   g[5]=lut[p_src[5]];   g[6]=lut[p_src[6]];   g[7]=lut[p_src[7]];
					g[8]=lut[p_src[8]];   g[9]=lut[p_src[9]];   g[10]=lut[p_src[10]]; g[11]=lut[p_src[11]];
					g[12]=lut[p_src[12]]; g[13]=lut[p_src[13]]; g[14]=lut[p_src[14]]; g[15]=lut[p_src[15]];
				}
				else
				{
					g[0]=p_src[0];   g[1]=p_src[1];   g[2]=p_src[2];   g[3]=p_src[3];
					g[4]=p_src[4];   g[5]=p_src[5];   g[6]=p_src[6];   g[7]=p_src[7];
					g[8]=p_src[8];   g[9]=p_src[9];   g[10]=p_src[10]; g[11]=p_src[11];
					g[12]=p_src[12]; g[13]=p_src[13]; g[14]=p_src[14]; g[15]=p_src[15];
				}
				auto	out = _mm_set_epi8( g[5], g[4],g[4],g[4], g[3],g[3],g[3], g[2],g[2],g[2], g[1],g[1],g[1], g[0],g[0],g[0] );
				_mm_storeu_si128( ++dst128, out );
				out = _mm_set_epi8( g[10],g[10], g[9],g[9],g[9], g[8],g[8],g[8], g[7],g[7],g[7], g[6],g[6],g[6], g[5],g[5] );
				_mm_storeu_si128( ++dst128, out );
				out = _mm_set_epi8( g[15],g[15],g[15], g[14],g[14],g[14], g[13],g[13],g[13], g[12],g[12],g[12], g[11],g[11],g[11], g[10] );
				_mm_storeu_si128( ++dst128, out );
			}
		}
		// Take the remainder colors at the end of the row.
		if( remainder_pixel_nb > 0 )
		{
			for( auto x = x_begin; x < x_end; x += 3, ++p_src )
			{
				UINT8 CONST grey = B_LUT ? lut[*p_src] : *p_src;
				set_v3( p_dst + x, grey );
			}
		}
	});
}

void imgcon::r8_to_rgb8_sse3( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	if( options.lut )	r8_to_rgb8_sse3_body<true >( src, src_pitch, dst, dst_pitch, options );
	else				r8_to_rgb8_sse3_body<false>( src, src_pitch, dst, dst_pitch, options );
}

//
// MONO to RGBA
//
// Templated inner body for r8_to_rgba8_sse3 : B_LUT toggles the per, source LUT lookup. Hoisted out of the per, block
// loop so the cold path doesn't pay a branch every 4 pixels.
template< bool B_LUT >
FINLINE void	r8_to_rgba8_sse3_body( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options ) NOEXCEPT
{
	UINT8 CONST		alpha				= options.alpha;
	UINT8 CONST * RESTRICT lut			= options.lut;
	INT32 CONSTEXPR	block_byte_nb		=  4;
	INT32 CONST		block_nb			= options.sx / block_byte_nb;
	INT32 CONST		remainder_pixel_nb	= options.sx & (block_byte_nb -1);
	INT32 CONST		x_end				= options.sx * 4;
	INT32 CONST		x_begin				= x_end - remainder_pixel_nb * 4;

	imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
	{
		IMGCON_LINE_PTRS( UINT8, UINT8 );
		// Do conversions in batches of 16 bytes:
		if( block_nb > 0 )
		{
			auto * RESTRICT dst128 = (__m128i *)p_dst;
			for( auto loop_nb = block_nb; loop_nb > 0; --loop_nb, ++dst128, p_src += 4 )
			{
				UINT8 grey[4];
				if constexpr( B_LUT )	{ grey[0]=lut[p_src[0]]; grey[1]=lut[p_src[1]]; grey[2]=lut[p_src[2]]; grey[3]=lut[p_src[3]]; }
				else					cpy_v4( grey, p_src );
				auto out = _mm_set_epi8(	alpha, grey[3], grey[3], grey[3],
											alpha, grey[2], grey[2], grey[2],
											alpha, grey[1], grey[1], grey[1],
											alpha, grey[0], grey[0], grey[0] );
				_mm_storeu_si128( dst128, out );
			}
		}
		if( remainder_pixel_nb > 0 )
		{
			for( auto x = x_begin; x < x_end; x += 4, ++p_src )
			{
				UINT8 CONST grey = B_LUT ? lut[*p_src] : *p_src;
				set_v3( p_dst + x, grey );
				p_dst[x + 3] = alpha;
			}
		}
	});
}

void imgcon::r8_to_rgba8_sse3( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	if( options.lut )	r8_to_rgba8_sse3_body<true >( src, src_pitch, dst, dst_pitch, options );
	else				r8_to_rgba8_sse3_body<false>( src, src_pitch, dst, dst_pitch, options );
}

bool	imgcon::r8_to_rgba8( UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	if( options.lut )
		imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT {
			bitcon::r8_to_rgba8_lut( src + (j * src_pitch), dst + (j * dst_pitch), options.sx, options.lut ); }); 
	else
		IMGCON_WRAP_CALL_SPEED(		bitcon::r8_to_rgba8,	( src + (j * src_pitch), dst + (j * dst_pitch), options.sx, options.alpha ) )
	return true;
}

bool	imgcon::r16_to_rgba8( UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	IMGCON_WRAP_CALL_SPEED( bitcon::r16_to_rgba8, ( src + (j * src_pitch), dst + (j * dst_pitch), options.sx, options.luma_min, options.luma_max, options.alpha ) )
	return true;
}


bool	imgcon::disp16_to_rgba8( UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT {
		bitcon::disparity16_to_rgba( (UINT8*)src + (j * src_pitch), (UINT8*)dst + (j * dst_pitch), options.sx, options.disp_min, options.disp_max, options.alpha ); });
	return true;
}

//
// BGR to RGB
//
void imgcon::bgr8_to_rgb8_sse3( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	auto CONST	mask1 = _mm_set_epi8( -127, 12, 13, 14,			9, 10, 11, 6,				7, 8, 3, 4,					5, 0, 1, 2 );
	auto CONST	mask2 = _mm_set_epi8( 1, -127, -127, -127,		-127, -127, -127, -127,		-127, -127, -127, -127,		-127, -127, -127, -127 );
		 
	auto CONST	mask3 = _mm_set_epi8( -127, -127, -127, -127,	-127, -127, -127, -127,		-127, -127, -127, -127,		-127, -127, 15, -127 );
	auto CONST	mask4 = _mm_set_epi8( 15, -127, 11, 12,			13, 8, 9, 10,				5, 6, 7, 2,					3, 4, -127, 0 );
	auto CONST	mask5 = _mm_set_epi8( -127, 0, -127, -127,		-127, -127, -127, -127,		-127, -127, -127, -127,		-127, -127, -127, -127 );
		 
	auto CONST	mask6 = _mm_set_epi8( -127, -127, -127, -127,	-127, -127, -127, -127,		-127, -127, -127, -127,		-127, -127, -127, 14 );
	auto CONST	mask7 = _mm_set_epi8( 13, 14, 15, 10,			11, 12, 7, 8,				9, 4, 5, 6,					1, 2, 3, -127 );

	// Batch up loads/stores into 3 * 16 byte chunks it : 16 rgb pixels
	INT16 CONSTEXPR block_byte_nb		=  16 * 3; 
	INT32 CONST		block_nb			= ( options.sx * 3 ) / block_byte_nb;
	INT16 CONST		remainder_pixel_nb	= (( options.sx * 3 ) - block_nb * block_byte_nb) / 3;

	imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
	{
		IMGCON_PIXEL_LOOP( UINT8, UINT8, block_nb )
		{
			//      0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15
			// in1  Ra Ga Ba Rb Gb Bb Rc Gc Bc Rd Gd Bd Re Ge Be Rf
			// in2  Gf Bf Rg Gg Bg Rh Gh Bh Ri Gi Bi Rj Gj Bj Rk Gk
			// in3  Bk Rl Gl Bl Rm Gm Bm Rn Gn Bn Ro Go Bo Rp Gp Bp
			__m128i	CONST in1 = _mm_loadu_si128( (__m128i *)(p_src +  0) );
			__m128i CONST in2 = _mm_loadu_si128( (__m128i *)(p_src + 16) );
			__m128i CONST in3 = _mm_loadu_si128( (__m128i *)(p_src + 32) );
			_mm_storeu_si128( (__m128i *)(p_dst +  0), _mm_or_si128( _mm_shuffle_epi8( in1, mask1 ), _mm_shuffle_epi8( in2, mask2 ) ) );
			auto	out		= _mm_or_si128( _mm_shuffle_epi8( in1, mask3 ), _mm_shuffle_epi8( in2, mask4 ) );
			_mm_storeu_si128( (__m128i *)(p_dst + 16), _mm_or_si128( out, _mm_shuffle_epi8( in3, mask5 ) ) );
			_mm_storeu_si128( (__m128i *)(p_dst + 32), _mm_or_si128( _mm_shuffle_epi8( in2, mask6 ), _mm_shuffle_epi8( in3, mask7 ) ) );
			p_src += block_byte_nb;
			p_dst += block_byte_nb;
		}
		for( auto nb = remainder_pixel_nb; nb > 0; --nb )
		{
			cpy_swap_v3( p_dst, p_src );
			p_dst += 3;
			p_src += 3;
		}
	});
}

void imgcon::bgr8_to_rgb8_fast( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
	{
		IMGCON_LINE_PTRS( UINT8, UINT8 );
		auto x = 0;
		for( x = src_pitch; x > 3; x -= 3 )
		{
			auto * RESTRICT dst32 = (UINT32*)p_dst;
			INT32 t = *((UINT32*)p_src);
			*dst32 = (t & 0xff00ff00) | ((t >> 16) & 0xff) | ((t & 0xff) << 16);
			p_src	+= 3;
			p_dst	+= 3;
		}
		if( x > 0 )
			cpy_swap_v3( p_dst, p_src );
	});
}

void imgcon::bgr8_to_rgb8( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
	{
		IMGCON_LINE_PTRS( UINT8, UINT8 );
		for( auto x = 0; x < src_pitch; x += 3 )
			cpy_swap_v3( p_dst + x , p_src + x );
	});
}

//
// RGB / BGR to RGBA (B_SWAP_RB picked from options.b_swap_red_blue, hoisted out of the SIMD loop via mask choice).
//
void imgcon::rgb8_to_rgba8_sse3( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	auto CONST	mask_alpha	= _mm_set_epi8( options.alpha, 0, 0, 0,	options.alpha, 0, 0, 0, options.alpha, 0, 0, 0, options.alpha, 0, 0, 0 );
	auto CONST	right		= _mm_set_epi32( 0xffffffff, 0xffffffff, 0, 0 );
	auto CONST	left		= _mm_set_epi32( 0, 0, 0xffffffff, 0xffffffff );
	__m128i	mask1, mask2, mask3, mask4;
	if( options.b_swap_red_blue )
	{
		mask1 = _mm_set_epi8( -127, 9, 10, 11,	-127, 6, 7, 8,		-127, 3, 4, 5,		-127, 0, 1, 2		);
		mask2 = _mm_set_epi8( -127, 5, 6, 7,	-127, 2, 3, 4,		-127, 15, 0, 1,		-127, 12, 13, 14	);
		mask3 = _mm_set_epi8( -127, 1, 2, 3,	-127, 14, 15, 0,	-127, 11, 12, 13,	-127, 8, 9, 10		);
		mask4 = _mm_set_epi8( -127, 13, 14, 15, -127, 10, 11, 12,	-127, 7, 8, 9,		-127, 4, 5, 6		);
	}
	else
	{
		mask1 = _mm_set_epi8( -127, 11, 10, 9,	-127, 8, 7, 6,		-127, 5, 4, 3,		-127, 2, 1, 0		);
		mask2 = _mm_set_epi8( -127, 7, 6, 5,	-127, 4, 3, 2,		-127, 1, 0, 15,		-127, 14, 13, 12	);
		mask3 = _mm_set_epi8( -127, 3, 2, 1,	-127, 0, 15, 14,	-127, 13, 12, 11,	-127, 10, 9, 8		);
		mask4 = _mm_set_epi8( -127, 15, 14, 13, -127, 12, 11, 10,	-127, 9, 8, 7,		-127, 6, 5, 4		);
	}

	// Batch up loads/stores into 16 byte chunks to use SSE3 efficiently:
	INT16 CONSTEXPR block_byte_nb		=  16 * 3; 
	INT32 CONST		block_nb			= ( options.sx * 3 ) / block_byte_nb;
	INT16 CONST		remainder_pixel_nb	= (( options.sx * 3 ) - block_nb * block_byte_nb) / 3;

	imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
	{
		IMGCON_PIXEL_LOOP( UINT8, UINT8, block_nb )
		{
			//             0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15
			// in_vec[0]   Ra Ga Ba Rb Gb Bb Rc Gc Bc Rd Gd Bd Re Ge Be Rf
			// in_vec[1]   Gf Bf Rg Gg Bg Rh Gh Bh Ri Gi Bi Rj Gj Bj Rk Gk
			// in_vec[2]   Bk Rl Gl Bl Rm Gm Bm Rn Gn Bn Ro Go Bo Rp Gp Bp

			__m128i	CONST in1 = _mm_loadu_si128( (__m128i *)(p_src +  0) );
			__m128i CONST in2 = _mm_loadu_si128( (__m128i *)(p_src + 16) );
			__m128i CONST in3 = _mm_loadu_si128( (__m128i *)(p_src + 32) );
			__m128i out;
			out = _mm_or_si128( _mm_shuffle_epi8( in1, mask1 ), mask_alpha );
			_mm_storeu_si128( (__m128i *)(p_dst +  0), out );

			out = _mm_or_si128( _mm_and_si128( in2, left ), _mm_and_si128( in1, right ) );
			out = _mm_or_si128( _mm_shuffle_epi8( out, mask2 ), mask_alpha );
			_mm_storeu_si128( (__m128i *)(p_dst + 16), out );

			out = _mm_or_si128( _mm_and_si128( in3, left ), _mm_and_si128( in2, right ) );
			out = _mm_or_si128( _mm_shuffle_epi8( out, mask3 ), mask_alpha );
			_mm_storeu_si128( (__m128i *)(p_dst + 32), out );

			out = _mm_or_si128( _mm_shuffle_epi8(in3, mask4 ), mask_alpha );
			_mm_storeu_si128( (__m128i *)(p_dst + 48), out );

			p_src	+= block_byte_nb;
			p_dst	+= 64;
		}
		if( remainder_pixel_nb > 0 )
		{
			for( auto nb = remainder_pixel_nb; nb > 0; --nb )
			{
				if( options.b_swap_red_blue )
					cpy_swap_v3( p_dst, p_src );
				else
					cpy_v3( p_dst, p_src );
				p_dst[3] = options.alpha;
				p_dst	+= 4;
				p_src	+= 3;
			}
		}
	});
}

void imgcon::rgb8_to_rgba8_ssse3_fast( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	auto CONST	mask_alpha	= _mm_set_epi8( options.alpha, 0, 0, 0, options.alpha, 0, 0, 0, options.alpha, 0, 0, 0, options.alpha, 0, 0, 0 );
	auto CONST	right		= _mm_set_epi32( 0xffffffff, 0xffffffff, 0, 0);
	auto CONST	left		= _mm_set_epi32( 0, 0, 0xffffffff, 0xffffffff );
	__m128i	mask1, mask2, mask3, mask4;
	if( options.b_swap_red_blue )
	{
		mask1 = _mm_set_epi8( -127, 9, 10, 11,	-127, 6, 7, 8,		-127, 3, 4, 5,		-127, 0, 1, 2		);
		mask2 = _mm_set_epi8( -127, 5, 6, 7,	-127, 2, 3, 4,		-127, 15, 0, 1,		-127, 12, 13, 14	);
		mask3 = _mm_set_epi8( -127, 1, 2, 3,	-127, 14, 15, 0,	-127, 11, 12, 13,	-127, 8, 9, 10		);
		mask4 = _mm_set_epi8( -127, 13, 14, 15, -127, 10, 11, 12,	-127, 7, 8, 9,		-127, 4, 5, 6		);
	}
	else
	{
		mask1 = _mm_set_epi8( -127, 11, 10, 9,	-127, 8, 7, 6,		-127, 5, 4, 3,		-127, 2, 1, 0		);
		mask2 = _mm_set_epi8( -127, 7, 6, 5,	-127, 4, 3, 2,		-127, 1, 0, 15,		-127, 14, 13, 12	);
		mask3 = _mm_set_epi8( -127, 3, 2, 1,	-127, 0, 15, 14,	-127, 13, 12, 11,	-127, 10, 9, 8		);
		mask4 = _mm_set_epi8( -127, 15, 14, 13, -127, 12, 11, 10,	-127, 9, 8, 7,		-127, 6, 5, 4		);
	}

	// Batch up loads into 6 * 16 byte chunks and stores into 8 * 16 byte chunks : 32 pixel at once
	INT16 CONSTEXPR block_byte_nb	=  16 * 6; 
	INT32 CONST block_nb			= ( options.sx * 3 ) / block_byte_nb;
	INT16 CONST remainder_pixel_nb	= (( options.sx * 3 ) - block_nb * block_byte_nb) / 3;

	imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
	{
		IMGCON_PIXEL_LOOP( UINT8, UINT8, block_nb )
		{
			//             0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15
			// in_vec[0]   Ra Ga Ba Rb Gb Bb Rc Gc Bc Rd Gd Bd Re Ge Be Rf
			// in_vec[1]   Gf Bf Rg Gg Bg Rh Gh Bh Ri Gi Bi Rj Gj Bj Rk Gk
			// in_vec[2]   Bk Rl Gl Bl Rm Gm Bm Rn Gn Bn Ro Go Bo Rp Gp Bp
			__m128i out;
			__m128i	CONST in1 = _mm_loadu_si128( (__m128i *)(p_src +  0) );
			__m128i CONST in2 = _mm_loadu_si128( (__m128i *)(p_src + 16) );
			__m128i CONST in3 = _mm_loadu_si128( (__m128i *)(p_src + 32) );
			__m128i CONST in4 = _mm_loadu_si128( (__m128i *)(p_src + 48) );
			__m128i CONST in5 = _mm_loadu_si128( (__m128i *)(p_src + 64) );
			__m128i CONST in6 = _mm_loadu_si128( (__m128i *)(p_src + 80) );

			out = _mm_or_si128( _mm_shuffle_epi8( in1, mask1 ), mask_alpha );
			_mm_storeu_si128( (__m128i *)(p_dst + 0), out );
			out = _mm_or_si128( _mm_and_si128( in2, left ), _mm_and_si128( in1, right ) );
			out = _mm_or_si128( _mm_shuffle_epi8( out, mask2 ), mask_alpha );
			_mm_storeu_si128( (__m128i *)(p_dst + 16), out );
			out = _mm_or_si128( _mm_and_si128( in3, left ), _mm_and_si128( in2, right ) );
			out = _mm_or_si128( _mm_shuffle_epi8( out, mask3 ), mask_alpha );
			_mm_storeu_si128( (__m128i *)(p_dst + 32), out );
			out = _mm_or_si128( _mm_shuffle_epi8( in3, mask4 ), mask_alpha );
			_mm_storeu_si128( (__m128i *)(p_dst + 48), out );

			out = _mm_or_si128( _mm_shuffle_epi8( in4, mask1 ), mask_alpha );
			_mm_storeu_si128( (__m128i *)(p_dst + 64), out );
			out = _mm_or_si128( _mm_and_si128( in5, left ), _mm_and_si128( in4, right ) );
			out = _mm_or_si128( _mm_shuffle_epi8( out, mask2 ), mask_alpha );
			_mm_storeu_si128( (__m128i *)(p_dst + 80), out );
			out = _mm_or_si128( _mm_and_si128( in6, left ), _mm_and_si128( in5, right ) );
			out = _mm_or_si128( _mm_shuffle_epi8( out, mask3 ), mask_alpha );
			_mm_storeu_si128( (__m128i *)(p_dst + 96), out );
			out = _mm_or_si128( _mm_shuffle_epi8( in6, mask4 ), mask_alpha );
			_mm_storeu_si128( (__m128i *)(p_dst + 112), out );

			p_dst += 128;
			p_src += 96;
		}
		for( auto nb = remainder_pixel_nb; nb > 0; --nb )
		{
			if( options.b_swap_red_blue )
				cpy_swap_v3( p_dst, p_src );
			else
				cpy_v3( p_dst, p_src );
			p_dst[3] = options.alpha;
			p_dst += 4;
			p_src += 3;
		}
	});
}

//void imgcon::rgb_to_rgba_avx2( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, INT32 CONST options.sx, INT32 CONST options.sy, UINT8 alpha_value, bool b_bgr )
//{
//	CONST	__m256i	mask_alpha = _mm256_set_epi8( alpha_value, 0, 0, 0, alpha_value, 0, 0, 0, alpha_value, 0, 0, 0, alpha_value, 0, 0, 0, alpha_value, 0, 0, 0, alpha_value, 0, 0, 0, alpha_value, 0, 0, 0, alpha_value, 0, 0, 0 );
//	CONST	__m256i mask2 = _mm256_set_epi8( -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 );
//	CONST	__m256i mask3 = _mm256_set_epi8( 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127, -127 );
//	__m256i	mask1, mask4, mask5, mask6;
//	if( b_bgr )
//	{
//		mask1 = _mm256_set_epi8( -127, 21, 22, 23, -127, 18, 19, 20, -127, 15, 16, 17, -127, 12, 13, 14, -127, 9, 10, 11, -127, 6, 7, 8, -127, 3, 4, 5, -127, 0, 1, 2 );
//		mask4 = _mm256_set_epi8( -127, 13, 14, 15, -127, 10, 11, 12, -127, 7, 8, 9, -127, 4, 5, 6, -127, 1, 2, 3, -127, 30, 31, 0, -127, 27, 28, 29, -127, 24, 25, 26 );
//		mask5 = _mm256_set_epi8( -127, 5, 6, 7, -127, 2, 3, 4, -127, 31, 0, 1, -127, 28, 29, 30, -127, 25, 26, 27, -127, 22, 23, 24, -127, 19, 20, 21, -127, 16, 17, 18 );
//		mask6 = _mm256_set_epi8( -127, 29, 30, 31, -127, 26, 27, 28, -127, 23, 24, 25, -127, 20, 21, 22, -127, 17, 18, 19, -127, 14, 15, 16, -127, 11, 12, 13, -127, 8, 9, 10 );
//	}
//	else
//	{
//		mask1 = _mm256_set_epi8( -127, 23, 22, 21, -127, 20, 19, 18, -127, 17, 16, 15, -127, 14, 13, 12, -127, 11, 10, 9, -127, 8, 7, 6, -127, 5, 4, 3, -127, 2, 1, 0 );
//		mask4 = _mm256_set_epi8( -127, 15, 14, 13, -127, 12, 11, 10, -127, 9, 8, 7, -127, 6, 5, 4, -127, 3, 2, 1, -127, 0, 31, 30, -127, 29, 28, 27, -127, 26, 25, 24 );
//		mask5 = _mm256_set_epi8( -127, 7, 6, 5, -127, 4, 3, 2, -127, 1, 0, 31, -127, 30, 29, 28, -127, 27, 26, 25, -127, 24, 23, 22, -127, 21, 20, 19, -127, 18, 17, 16 );
//		mask6 = _mm256_set_epi8( -127, 31, 30, 29, -127, 28, 27, 26, -127, 25, 24, 23, -127, 22, 21, 20, -127, 19, 18, 17, -127, 16, 15, 14, -127, 13, 12, 11, -127, 10, 9, 8 );
//	}
//
//
//	for( INT32 i = 0; i < options.sy; i++, src += src_pitch )
//	{
//		UINT8 * RESTRICT p_dst = (UINT8*)dst + (i * dst_pitch);
//
//		// Batch up loads/stores into 16 byte chunks to use SSE2 efficiently:
//		INT32	ssse3blocks = (options.sx) / 32;
//		INT32	ssse3remainder = (options.sx) & 31;
//
//		CONST	__m256i * RESTRICT in_vec		= (__m256i*)(src);
//				__m256i * RESTRICT out_vec	= (__m256i*)(p_dst);
//
//		bool b_aligned = mem::IS_ALIGNED_16( p_dst );
//		for( INT32 j = ssse3blocks; j > 0; --j )
//		{
//			/*                0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15 16 17 18 19 20 21 22 23 24 25 26 27 28 29 30 31
//				* in_vec[0]   Ra Ga Ba Rb Gb Bb Rc Gc Bc Rd Gd Bd Re Ge Be Rf Gf Bf Rg Gg Bg Rh Gh Bh Ri Gi Bi Rj Gj Bj Rk Gk
//				* in_vec[1]   Bk Rl Gl Bl Rm Gm Bm Rn Gn Bn Ro Go Bo Rp Gp Bp Rq Gq Bq Rr Gr Br Rs Gs Bs Rt Gt Bt Ru Gu Bu Rv
//				* in_vec[2]   Gv Bv Rw Gw Bw Rx Gx Bx Ry Gy By Rz Gz Bz R1 G1 B1 R2 G2 B2 R3 G3 B3 R4 G4 B4 R5 G5 B5 R6 G6 B6
//
//				* out_vec[0]  Ra Ga Ba 0  Rb Gb Bb 0  Rc Gc Bc 0  Rd Gd Bd 0  Re Ge Be 0  Rf Gf Bf 0  Rg Gg Bg 0  Rh Gh Bh 0
//							  Ri Gi Bi 0  Rj Gj Bj 0  Rk Gk Bk 0  Rl Gl Bl 0  Rm Gm Bm 0  Rn Gn Bn 0  Ro Go Bo 0  Rp Gp Bp 0
//							  Rq Gq Bq 0  Rr Gr Br 0  Rs Gs Bs 0  Rt Gt Bt 0  Ru Gu Bu 0  Rv Gv Bv 0  Rw Gw Bw 0  Rx Gx Bx 0
//							  Ry Gy By 0  Rz Gz Bz 0  R1 G1 B1 0  R2 G2 B2 0  R3 G3 B3 0  R4 G4 B4 0  R5 G5 B5 0  R6 G6 B6 0
//			*/
//			__m256i out;
//
//			__m256i	in1 = _mm256_loadu_si256( in_vec++ );
//			__m256i	in2 = _mm256_loadu_si256( in_vec++ );
//			__m256i	in3 = _mm256_loadu_si256( in_vec++ );
//
//			out = _mm256_or_si256( _mm256_shuffle_epi8( in1, mask1 ), mask_alpha );
//			if( b_aligned )			{	_mm256_store_si256( out_vec++, out );		}
//			else					{	_mm256_storeu_si256( out_vec++, out );		}
//
//			out = _mm256_or_si256( _mm256_and_si256( in2, mask3 ), _mm256_and_si256( in1, mask2 ) );
//			out = _mm256_or_si256( _mm256_shuffle_epi8( out, mask4 ), mask_alpha );
//			if( b_aligned )			{	_mm256_store_si256( out_vec++, out );		}
//			else					{	_mm256_storeu_si256( out_vec++, out );		}
//
//			out = _mm256_or_si256( _mm256_and_si256( in3, mask3 ), _mm256_and_si256( in2, mask2 ) );
//			out = _mm256_or_si256( _mm256_shuffle_epi8( out, mask5 ), mask_alpha );
//			if( b_aligned )			{	_mm256_store_si256( out_vec++, out );		}
//			else					{	_mm256_storeu_si256( out_vec++, out );		}
//
//			out = _mm256_or_si256( _mm256_shuffle_epi8(in3, mask6 ), mask_alpha );
//			if( b_aligned )			{	_mm256_store_si256( out_vec++, out );		}
//			else					{	_mm256_storeu_si256( out_vec++, out );		}
//
//		}
//		if( ssse3remainder > 0 )
//		{
//			for( INT32 x = (ssse3blocks * 32); x < (options.sx * 4); ++x )
//			{
//				if( b_bgr )
//				{
//					p_dst[x * 3 + 0] = src[x * 4 + 2];
//					p_dst[x * 3 + 1] = src[x * 4 + 1];
//					p_dst[x * 3 + 2] = src[x * 4 + 0];
//					p_dst[x * 3 + 3] = alpha_value;
//				}
//				else
//				{
//					p_dst[x * 3 + 0] = src[x * 4 + 0];
//					p_dst[x * 3 + 1] = src[x * 4 + 1];
//					p_dst[x * 3 + 2] = src[x * 4 + 2];
//					p_dst[x * 3 + 3] = alpha_value;
//				}
//			}
//		}
//	}
//	_mm256_zeroupper();
//	_mm_mfence();
//}

//
// RGB to RGBA
//
bool	imgcon::rgb8_to_rgba8( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	if( options.lut )
	{
		if( options.b_swap_red_blue )
			imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT {
				bitcon::bgr8_to_rgba8_incrust( src + (j * src_pitch), dst + (j * dst_pitch), options.sx, options.lut ); });
		else
			imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT {
				bitcon::rgb8_to_rgba8_incrust( src + (j * src_pitch), dst + (j * dst_pitch), options.sx, options.lut ); });
	}
	else // no lut
	{
		bool CONST b_fast = texture_flux_master->is_convert_to_rgb_fast();
		// Fast method is faster than sse3
		//bool b_16 = (src_pitch % 16) == 0;
		//if( b_16 && texture_flux_master->is_convert_to_rgb_sse3() )
		//{
		//	imgcon::bgr_to_rgba_sse3( src, src_pitch, dst, dst_pitch, options.sx, options.sy, alpha_value, false );
		//}
		//else
		//if( b_16 && texture_flux_master->is_convert_to_rgb_avx2() && c_cpu::b_AVX2 )
		//{
		//	//imgcon::rgb_to_rgba_sse( src, src_pitch, dst, dst_pitch, options.sx, options.sy );
		//	imgcon::bgr_to_rgba_avx2( src, src_pitch, dst, dst_pitch, options.sx, options.sy, alpha_value, false );
		//	return true;
		//}
		//else
		if( texture_flux_master->is_convert_to_rgb_sse3() )
		{
			SPY_PUSH_RANGE_FUNCTION( spy::MEM_LOW );
			if( b_fast )
				imgcon::rgb8_to_rgba8_ssse3_fast(	src, src_pitch, dst, dst_pitch, options );
			else
				imgcon::rgb8_to_rgba8_sse3(			src, src_pitch, dst, dst_pitch, options );
			SPY_POP_RANGE();
		}
		else
		{
			if( options.b_swap_red_blue )
				imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT {
					bitcon::bgr8_to_rgba8_fast	( src + (j * src_pitch), dst + (j * dst_pitch), options.sx, options.alpha ); });
			else
				IMGCON_WRAP_CALL_SPEED(		bitcon::rgb8_to_rgba8,		( src + (j * src_pitch), dst + (j * dst_pitch), options.sx, options.alpha ) )
		}
	}
	return true;
}




//
// RGBA / BGRA to RGB (B_SWAP_RB picked from options.b_swap_red_blue, hoisted out of the SIMD loop via mask choice).
//
void imgcon::rgba8_to_rgb8_sse3( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	__m128i	mask1, mask2, mask3, mask4, mask5, mask6;

	if( options.b_swap_red_blue )
	{
		mask1	= _mm_set_epi8( -127,				-127, -127, -127,		12, 13, 14,			8, 9, 10,			4, 5, 6,			0, 1, 2				);
		mask2	= _mm_set_epi8( 6,					0, 1, 2,				-127, -127, -127,	-127, -127, -127,	-127, -127, -127,	-127, -127, -127	);
		mask3	= _mm_set_epi8( -127, -127,			-127, -127,	-127,		-127, -127,-127,	12, 13, 14,			8, 9, 10,			4, 5				);
		mask4	= _mm_set_epi8( 9, 10,				4, 5, 6,				0, 1, 2,			-127, -127, -127,	-127, -127, -127,	-127, -127			);
		mask5	= _mm_set_epi8( -127, -127, -127,	-127, -127, -127,		-127, -127,	-127,	-127, -127, -127,	12, 13, 14,			8					);
		mask6	= _mm_set_epi8( 12, 13, 14,			8, 9, 10,				4, 5, 6,			0, 1, 2,			-127, -127, -127,	-127				);
	}
	else
	{
		mask1	= _mm_set_epi8( -127,				-127, -127, -127,		14, 13, 12,			10,	9, 8,			6, 5, 4,			2, 1, 0				);
		mask2	= _mm_set_epi8( 4,					2, 1, 0,				-127, -127, -127,	-127, -127, -127,	-127, -127,	-127,	-127, -127, -127	);
		mask3	= _mm_set_epi8( -127, -127,			-127, -127,	-127,		-127, -127, -127,	14, 13, 12,			10, 9, 8,			6, 5				);
		mask4	= _mm_set_epi8( 9, 8,				6, 5, 4,				2, 1, 0,			-127, -127, -127,	-127, -127, -127,	-127, -127			);
		mask5	= _mm_set_epi8( -127, -127, -127,	-127, -127, -127,		-127, -127,	-127,	-127, -127, -127,	14, 13, 12,			10					);
		mask6	= _mm_set_epi8( 14, 13, 12,			10,	9, 8,				6, 5, 4,			2, 1, 0,			-127, -127, -127,	-127				);
	}

	/*				0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15
		in_vec[0]	Ba Ga Ra 0  Bb Gb Rb 0  Bc Gc Rc 0  Bd Gd Rd  0
		in_vec[1]	Be Ge Re 0  Bf Gf Rf 0  Bg Gg Rg 0  Bh Gh Rh  0
		in_vec[2]	Bi Gi Ri 0  Bj Gj Rj 0  Bk Gk Rk 0  Bl Gl Rl  0
		in_vec[3]	Bm Gm Rm 0  Bn Gn Rn 0  Bo Go Ro 0  Bp Gp Rp  0

					0  1  2  3  4  5  6  7  8  9  10 11 12 13 14 15
		out_vec[0]	Ra Ga Ba Rb Gb Bb Rc Gc Bc Rd Gd Bd Re Ge Be Rf
		out_vec[1]	Gf Bf Rg Gg Bg Rh Gh Bh Ri Gi Bi Rj Gj Bj Rk Gk
		out_vec[2]	Bk Rl Gl Bl Rm Gm Bm Rn Gn Bn Ro Go Bo Rp Gp Bp
	*/

	// Batch up loads into 4 * 16 byte chunks and stores into 3 * 16 byte chunks : 16 pixels at once
	INT32 CONSTEXPR block_byte_nb		=  16 * 4; 
	INT32 CONST		block_nb			= ( options.sx * 4 ) / block_byte_nb;
	INT32 CONST		remainder_pixel_nb	= (( options.sx * 4 ) & ( block_byte_nb - 1)) / 4;

	imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
	{
		IMGCON_PIXEL_LOOP( UINT8, UINT8, block_nb )
		{
			__m128i in1 = _mm_loadu_si128( (__m128i *)(p_src +  0) );
			__m128i in2 = _mm_loadu_si128( (__m128i *)(p_src + 16) );
			__m128i in3 = _mm_loadu_si128( (__m128i *)(p_src + 32) );
			__m128i in4 = _mm_loadu_si128( (__m128i *)(p_src + 48) );

			__m128i out1; __m128i out2; __m128i out3;
			out1 = _mm_or_si128( _mm_shuffle_epi8( in1, mask1 ), _mm_shuffle_epi8( in2, mask2 ) );
			out2 = _mm_or_si128( _mm_shuffle_epi8( in2, mask3 ), _mm_shuffle_epi8( in3, mask4 ) );
			out3 = _mm_or_si128( _mm_shuffle_epi8( in3, mask5 ), _mm_shuffle_epi8( in4, mask6 ) );

			_mm_storeu_si128( (__m128i *)(p_dst +  0), out1 );
			_mm_storeu_si128( (__m128i *)(p_dst + 16), out2 );
			_mm_storeu_si128( (__m128i *)(p_dst + 32), out3 );
			p_src += 64;
			p_dst += 48;
		}
		for( auto nb = remainder_pixel_nb; nb > 0; --nb )
		{
			if( options.b_swap_red_blue )
				cpy_swap_v3( p_dst, p_src );
			else
				cpy_v3( p_dst, p_src );
			p_dst	+= 3;
			p_src	+= 4;
		}
	});
}


//
// BGRA to RGBA
//
void imgcon::bgra8_to_rgba8_sse2( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	// Batch up loads/stores into 16 byte chunks to use SSE2 efficiently:
	INT32 CONSTEXPR block_byte_nb		=  16; 
	INT32 CONST		block_nb			= ( options.sx * 4 ) / block_byte_nb;
	INT32 CONST		remainder_byte_nb	= ( options.sx * 4 ) & ( block_byte_nb - 1);
	// x is computed for the dst
	INT32 CONST x_end = options.sx * 4;
	INT32 CONST x_begin = x_end - remainder_byte_nb;

	// Generate a constant of all FF bytes:
	auto CONST	allFFs128 = _mm_cmpeq_epi32( _mm_setzero_si128(), _mm_setzero_si128() );
	auto CONST	bMask	= _mm_srli_epi32( allFFs128, 24 );
	auto CONST	rMask	= _mm_slli_epi32( bMask, 16 );

	imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
	{
		IMGCON_LINE_PTRS( UINT8, UINT8 );

		// Do conversions in batches of 16 bytes:
		if( block_nb > 0 )
		{
			auto * RESTRICT src128 = (__m128i CONST *)p_src;
			auto * RESTRICT dst128 = (__m128i *)p_dst;

			// Increment by 16 bytes at a time:
			for( auto loop_nb = block_nb; loop_nb > 0; --loop_nb, ++dst128, ++src128 )
			{
				// Load up 4 colors simultaneously:
				__m128i CONST rgba = _mm_loadu_si128( src128 );
				// Swap the R and B components:
				// Isolate the B component and shift it left 16 bits:
				// ABGR
				auto CONST bNew		= _mm_slli_epi32( _mm_and_si128( rgba, bMask ), 16 );
				// Isolate the R component and shift it right 16 bits:
				auto CONST rNew		= _mm_srli_epi32( _mm_and_si128( rgba, rMask ), 16 );
				// Now mask off the old R and B components from the rgba data to get 0g0a:
				auto CONST _g_a		= _mm_or_si128( _mm_and_si128( rgba, _mm_or_si128( _mm_slli_epi32( bMask, 8 ), _mm_slli_epi32( rMask, 8 ) ) ), _mm_or_si128( rNew, bNew ) );
				// Finally, OR up all the individual components to get BGRA:
				auto CONST bgra		= _mm_or_si128( _g_a, _mm_or_si128( rNew, bNew ) );
				_mm_storeu_si128( dst128, bgra );
			}
		}

		// Take the remainder colors at the end of the row that weren't able to
		// be included into the last 16 byte chunk:
		if( remainder_byte_nb > 0 )
		{
			//todo better than the previous ones ?
			// x is computed for the src and dst
			for( auto x = x_begin; x < x_end; x += 4 )
			{
				cpy_swap_v3( p_dst + x, p_src + x );
				p_dst[x + 3] = p_src[x + 3];
			}
		}
	});
}

void imgcon::rgba8_to_rgba8_sse3( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	auto CONST	mask =  options.b_force_alpha
							?	(	options.b_swap_red_blue	? _mm_set_epi8(	-127,	12, 13, 14,		-127,	8, 9, 10,	-127,	4, 5, 6,	-127,	0, 1, 2 )
														: _mm_set_epi8(	-127,	14, 13, 12,		-127,	10, 9, 8,	-127,	6, 5, 4,	-127,	2, 1, 0 )
								)
							:	(	options.b_swap_red_blue	? _mm_set_epi8(	15,		12, 13, 14,		15,		8, 9, 10,	15,		4, 5, 6,	15,		0, 1, 2 )
														: _mm_set_epi8(	15,		14, 13, 12,		15,		10, 9, 8,	15,		6, 5, 4,	15,		2, 1, 0 )
								)
							;
	auto CONST maskalpha	= _mm_set_epi8( options.alpha, 0, 0, 0, options.alpha, 0, 0, 0, options.alpha, 0, 0, 0, options.alpha, 0, 0, 0 );
	//UINT8	val = alpha_value;
	//for( INT32 y = 0; y < options.sy; y++, src += src_pitch )
	//{

	// Batch up loads/stores into 16 byte chunks to use SSE2 efficiently:
	INT32 CONSTEXPR block_byte_nb		= 16; 
	INT32 CONST		block_nb			= ( options.sx * 4 ) / block_byte_nb;
	INT32 CONST		remainder_byte_nb	= ( options.sx * 4 ) & ( block_byte_nb - 1);
	// x is computed for the dst
	INT32 CONST x_end = options.sx * 4;
	INT32 CONST x_begin = x_end - remainder_byte_nb;

	imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
	{
		IMGCON_LINE_PTRS( UINT8, UINT8 );
		// Do conversions in batches of 16 bytes:
		if( block_nb > 0 )
		{
			auto * src128 = (__m128i CONST *)p_src;
			auto * dst128 = (__m128i *)p_dst;

			// Increment by 16 bytes at a time:
			if( options.b_force_alpha )
			{
				for( auto loop_nb = block_nb; loop_nb > 0; --loop_nb, ++dst128, ++src128 )
					_mm_storeu_si128( dst128, _mm_or_si128( _mm_shuffle_epi8( _mm_loadu_si128( src128 ), mask ), maskalpha ) );
			}
			else
			{
				for( auto loop_nb = block_nb; loop_nb > 0; --loop_nb, ++dst128, ++src128 )
					_mm_storeu_si128( dst128, _mm_shuffle_epi8( _mm_loadu_si128( src128 ), mask ) );
			}
		}

		// Take the remainder colors at the end of the row that weren't able to
		// be included into the last 16 byte chunk:
		if( remainder_byte_nb > 0 )
		{
			for( auto x = x_begin; x < x_end; x += 4 )
			{
				cpy_swap_v3( p_dst + x, p_src + x );
				p_dst[x + 3] = options.b_force_alpha ? options.alpha : src[x + 3];
			}
		}
	});
}

void imgcon::rgba8_to_rgba8_sse3_fast( UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	auto CONST sx4 = options.sx * 4;
	if( options.b_force_alpha )
	{
		auto CONST	mask = options.b_swap_red_blue	? _mm_set_epi8(	-127,	12,13,14,	-127,	8,9,10,	-127,	4,5,6,	-127,	0,1,2 )
												: _mm_set_epi8(	-127,	14,13,12,	-127,	10,9,8,	-127,	6,5,4,	-127,	2,1,0 );
		auto CONST	maskalpha	= _mm_set_epi8( options.alpha, 0,0,0, options.alpha, 0,0,0, options.alpha, 0,0,0, options.alpha, 0,0,0 );

		// Increment by 6 * 16 bytes at a time :  24 pixel at once
		INT32 CONST BYTE_PER_LOOP = 96;
		INT32 CONST BLK_NB = sx4 / BYTE_PER_LOOP;
		INT32 CONST BLK_16 = (sx4 % BYTE_PER_LOOP) >> 4;
		INT32 CONST BLK_4 = ((sx4 % BYTE_PER_LOOP) & 0xf) >> 2;

		imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
		{
			IMGCON_LINE_PTRS( UINT8, UINT8 );
			__m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5;
			for( auto blk_nb = BLK_NB; blk_nb > 0; --blk_nb )
			{
				xmm0 = _mm_loadu_si128( (__m128i *)(p_src +  0) );
				xmm1 = _mm_loadu_si128( (__m128i *)(p_src + 16) );
				xmm2 = _mm_loadu_si128( (__m128i *)(p_src + 32) );
				xmm3 = _mm_loadu_si128( (__m128i *)(p_src + 48) );
				xmm4 = _mm_loadu_si128( (__m128i *)(p_src + 64) );
				xmm5 = _mm_loadu_si128( (__m128i *)(p_src + 80) );
				xmm0 = _mm_or_si128( _mm_shuffle_epi8( xmm0, mask ), maskalpha );
				xmm1 = _mm_or_si128( _mm_shuffle_epi8( xmm1, mask ), maskalpha );
				xmm2 = _mm_or_si128( _mm_shuffle_epi8( xmm2, mask ), maskalpha );
				xmm3 = _mm_or_si128( _mm_shuffle_epi8( xmm3, mask ), maskalpha );
				xmm4 = _mm_or_si128( _mm_shuffle_epi8( xmm4, mask ), maskalpha );
				xmm5 = _mm_or_si128( _mm_shuffle_epi8( xmm5, mask ), maskalpha );
				_mm_storeu_si128( (__m128i *)(p_dst + 0 ), xmm0 );
				_mm_storeu_si128( (__m128i *)(p_dst + 16), xmm1 );
				_mm_storeu_si128( (__m128i *)(p_dst + 32), xmm2 );
				_mm_storeu_si128( (__m128i *)(p_dst + 48), xmm3 );
				_mm_storeu_si128( (__m128i *)(p_dst + 64), xmm4 );
				_mm_storeu_si128( (__m128i *)(p_dst + 80), xmm5 );
				p_src += BYTE_PER_LOOP;
				p_dst += BYTE_PER_LOOP;
			}
			if( BLK_16 )
			{
				switch( BLK_16 )
				{
				case 5: xmm4 = _mm_loadu_si128( (__m128i *)(p_src + 64) );
				case 4: xmm3 = _mm_loadu_si128( (__m128i *)(p_src + 48) );
				case 3: xmm2 = _mm_loadu_si128( (__m128i *)(p_src + 32) );
				case 2: xmm1 = _mm_loadu_si128( (__m128i *)(p_src + 16) );
				case 1: xmm0 = _mm_loadu_si128( (__m128i *)(p_src + 0 ) );
				}
				switch( BLK_16 )
				{
				case 5: xmm4 = _mm_or_si128( _mm_shuffle_epi8( xmm4, mask ), maskalpha );
				case 4: xmm3 = _mm_or_si128( _mm_shuffle_epi8( xmm3, mask ), maskalpha );
				case 3: xmm2 = _mm_or_si128( _mm_shuffle_epi8( xmm2, mask ), maskalpha );
				case 2: xmm1 = _mm_or_si128( _mm_shuffle_epi8( xmm1, mask ), maskalpha );
				case 1: xmm0 = _mm_or_si128( _mm_shuffle_epi8( xmm0, mask ), maskalpha );
				}
				switch( BLK_16 )
				{
				case 5: _mm_storeu_si128( (__m128i *)(p_dst + 64), xmm4 );
				case 4: _mm_storeu_si128( (__m128i *)(p_dst + 48), xmm3 );
				case 3: _mm_storeu_si128( (__m128i *)(p_dst + 32), xmm2 );
				case 2: _mm_storeu_si128( (__m128i *)(p_dst + 16), xmm1 );
				case 1: _mm_storeu_si128( (__m128i *)(p_dst + 0 ), xmm0 );
				}
				p_src += 16 * BLK_16;
				p_dst += 16 * BLK_16;
			}
			for( auto nb = BLK_4; nb > 0; --nb )
			{
				cpy_swap_v3( p_dst, p_src );
				p_dst[3] = options.alpha;
				p_dst += 4;
				p_src += 4;
			}
		});
	}
	else
	{
		auto CONST 	mask = options.b_swap_red_blue	? _mm_set_epi8(	15,	12,13,14,	11,	8,9,10,	7,	4,5,6,	3,	0,1,2 )
												: _mm_set_epi8(	15,	14,13,12,	11,	10,9,8,	7,	6,5,4,	3,	2,1,0 );
		// Increment by 7 * 16 bytes at a time :  28 pixel at once
		INT32 CONSTEXPR	BYTE_PER_LOOP	= 112;
		INT32 CONST		BLK_NB			= sx4 / BYTE_PER_LOOP;
		INT32 CONST		BLK_16			= (sx4 % BYTE_PER_LOOP) >> 4;
		INT32 CONST		BLK_4 			= ((sx4 % BYTE_PER_LOOP) & 0xf) >> 2;

		imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
		{
			IMGCON_LINE_PTRS( UINT8, UINT8 );
			__m128i xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6;
			for(  auto blk_nb = BLK_NB; blk_nb > 0; --blk_nb )
			{
				xmm0 = _mm_loadu_si128( (__m128i *)(p_src + 0 ) );
				xmm1 = _mm_loadu_si128( (__m128i *)(p_src + 16) );
				xmm2 = _mm_loadu_si128( (__m128i *)(p_src + 32) );
				xmm3 = _mm_loadu_si128( (__m128i *)(p_src + 48) );
				xmm4 = _mm_loadu_si128( (__m128i *)(p_src + 64) );
				xmm5 = _mm_loadu_si128( (__m128i *)(p_src + 80) );
				xmm6 = _mm_loadu_si128( (__m128i *)(p_src + 96) );
				xmm0 = _mm_shuffle_epi8( xmm0, mask );
				xmm1 = _mm_shuffle_epi8( xmm1, mask );
				xmm2 = _mm_shuffle_epi8( xmm2, mask );
				xmm3 = _mm_shuffle_epi8( xmm3, mask );
				xmm4 = _mm_shuffle_epi8( xmm4, mask );
				xmm5 = _mm_shuffle_epi8( xmm5, mask );
				xmm6 = _mm_shuffle_epi8( xmm6, mask );
				_mm_storeu_si128( (__m128i *)(p_dst + 0 ), xmm0 );
				_mm_storeu_si128( (__m128i *)(p_dst + 16), xmm1 );
				_mm_storeu_si128( (__m128i *)(p_dst + 32), xmm2 );
				_mm_storeu_si128( (__m128i *)(p_dst + 48), xmm3 );
				_mm_storeu_si128( (__m128i *)(p_dst + 64), xmm4 );
				_mm_storeu_si128( (__m128i *)(p_dst + 80), xmm5 );
				_mm_storeu_si128( (__m128i *)(p_dst + 96), xmm6 );
				p_src	+= BYTE_PER_LOOP;
				p_dst	+= BYTE_PER_LOOP;
			}
			if( BLK_16 )
			{
				switch( BLK_16 )
				{
				case 6: xmm5 = _mm_loadu_si128( (__m128i *)(p_src + 80) );
				case 5: xmm4 = _mm_loadu_si128( (__m128i *)(p_src + 64) );
				case 4: xmm3 = _mm_loadu_si128( (__m128i *)(p_src + 48) );
				case 3: xmm2 = _mm_loadu_si128( (__m128i *)(p_src + 32) );
				case 2: xmm1 = _mm_loadu_si128( (__m128i *)(p_src + 16) );
				case 1: xmm0 = _mm_loadu_si128( (__m128i *)(p_src + 0 ) );
				}
				switch( BLK_16 )
				{
				case 6: xmm5 = _mm_shuffle_epi8( xmm5, mask );
				case 5: xmm4 = _mm_shuffle_epi8( xmm4, mask );
				case 4: xmm3 = _mm_shuffle_epi8( xmm3, mask );
				case 3: xmm2 = _mm_shuffle_epi8( xmm2, mask );
				case 2: xmm1 = _mm_shuffle_epi8( xmm1, mask );
				case 1: xmm0 = _mm_shuffle_epi8( xmm0, mask );
				}
				switch( BLK_16 )
				{
				case 6: _mm_storeu_si128( (__m128i *)(p_dst + 80), xmm5 );
				case 5: _mm_storeu_si128( (__m128i *)(p_dst + 64), xmm4 );
				case 4: _mm_storeu_si128( (__m128i *)(p_dst + 48), xmm3 );
				case 3: _mm_storeu_si128( (__m128i *)(p_dst + 32), xmm2 );
				case 2: _mm_storeu_si128( (__m128i *)(p_dst + 16), xmm1 );
				case 1: _mm_storeu_si128( (__m128i *)(p_dst + 0 ), xmm0 );
				}
				p_src += 16 * BLK_16;
				p_dst += 16 * BLK_16;
			}
			for( auto nb = BLK_4; nb > 0; --nb )
			{
				cpy_swap_v3( p_dst, p_src );
				p_dst[3] = p_src[3];
				p_dst += 4;
				p_src += 4;
			}
		});
	}
}


bool imgcon::rgba8_to_rgba8( UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	SPY_PUSH_RANGE_FUNCTION( spy::MEM_LOW );
	bool b_alpha_done = true;
	if( options.lut )
	{
		if( options.b_swap_red_blue )
			imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT {
				bitcon::bgra8_to_rgba8_incrust_green( src + (j * src_pitch), dst + (j * dst_pitch), options.sx, options.lut ); });
		else	
			imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT {
				bitcon::rgba8_to_rgba8_incrust_green( src + (j * src_pitch), dst + (j * dst_pitch), options.sx, options.lut ); });
	}
	else if( !options.b_force_alpha && !options.b_swap_red_blue )
	{
		imgcon::memcpy_image( src, src_pitch, dst, dst_pitch, options.sy );
		b_alpha_done = false;
	}
	//if( texture_flux_master->is_convert_to_rgb_avx2() )
	//{
	//	bgra_to_rgba_avx2( src, src_pitch, dst, dst_pitch, options.sx, options.sy, b_force_alpha, alpha_value );
	//}
	//else
	// SSE3 is 2.5-3x faster than our method
	else if( texture_flux_master->is_convert_to_rgb_sse3() )
	{
		//options.b_swap_red_blue = true;	// done by the function
		// SSE3 fast is 5-6x faster than our method
		if( texture_flux_master->is_convert_to_rgb_fast() )
			rgba8_to_rgba8_sse3_fast(	src, src_pitch, dst, dst_pitch, options );
		else
			rgba8_to_rgba8_sse3(		src, src_pitch, dst, dst_pitch, options );
	}
	// SSE2 takes 50% more time than our method
	else if( options.b_swap_red_blue && texture_flux_master->is_convert_to_rgb_sse2() )
		bgra8_to_rgba8_sse2( src, src_pitch, dst, dst_pitch, options );
	else if( options.b_swap_red_blue )
	{
		imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT {
			bitcon::bgra8_to_rgba8_fast( src + (j * src_pitch), dst + (j * dst_pitch), options.sx ); });
		b_alpha_done = false;
	}
	else
	{
		debug_break( "Unsupported code path in %s()", __FUNCTION__ );
		b_alpha_done = false;
	}
	SPY_POP_RANGE();
	return b_alpha_done;
}


//void imgcon::bgra8_to_rgba8_avx2( UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, INT32 CONST options.sx, INT32 CONST options.sy, bool b_force_alpha, UINT8 alpha_value )
//{
//	CONST __m256i mask1		= _mm256_set_epi8( 31, 28, 29, 30, 27, 24, 25, 26, 23, 20, 21, 22, 19, 16, 17, 18, 15, 12, 13, 14, 11, 8, 9, 10, 7, 4, 5, 6, 3, 0, 1, 2 );
//	CONST __m256i mask2		= _mm256_set_epi8( -127, 28, 29, 30, -127, 24, 25, 26, -127, 20, 21, 22, -127, 16, 17, 18, -127, 12, 13, 14, -127, 8, 9, 10, -127, 4, 5, 6, -127, 0, 1, 2 );
//	CONST __m256i maskalpha = _mm256_set_epi8( alpha_value, 0, 0, 0, alpha_value, 0, 0, 0, alpha_value, 0, 0, 0, alpha_value, 0, 0, 0, alpha_value, 0, 0, 0, alpha_value, 0, 0, 0, alpha_value, 0, 0, 0, alpha_value, 0, 0, 0 );
//	for( INT32 y = 0; y < options.sy; y++, src += src_pitch )
//	{
//		UINT8*	p_dst = (UINT8*)dst + (y * dst_pitch);
//
//		// Batch up loads/stores into 32 byte chunks to use AVX efficiently:
//		INT32	avx_blocks		= (options.sx * 4) / 32;
//		INT32	avx_remainder	= (options.sx * 4) & 31;
//
//		// Do conversions in batches of 32 bytes:
//		if( avx_blocks > 0 )
//		{
//			__m256i *src128 = (__m256i *)src;
//			__m256i *dst128 = (__m256i *)p_dst;
//
//			bool	b_aligned = mem::IS_ALIGNED_16( src ) && mem::IS_ALIGNED_16( p_dst );		// maybe IS_ALIGNED_32
//			// Increment by 16 bytes at a time:
//			for( INT32 i = 0; i < avx_blocks; ++i, ++dst128, ++src128 )
//			{
//				if( b_force_alpha )
//				{
//					if( b_aligned )
//					{
////						_mm_store_si128( dst128, _mm_or_si128( _mm_shuffle_epi8( _mm_load_si128( src128 ), mask2 ), maskalpha ) );
//						_mm256_store_si256( dst128, _mm256_or_si256( _mm256_shuffle_epi8( _mm256_load_si256( src128 ), mask2 ), maskalpha ) );
//					}
//					else
//					{
//						_mm256_storeu_si256( dst128, _mm256_or_si256( _mm256_shuffle_epi8( _mm256_loadu_si256( src128 ), mask2 ), maskalpha ) );
//					}
//				}
//				else
//				{
//					if( b_aligned )
//					{
//						_mm256_store_si256( dst128, _mm256_shuffle_epi8( _mm256_load_si256( src128 ), mask1 ) );
//					}
//					else
//					{
//						_mm256_storeu_si256( dst128, _mm256_shuffle_epi8( _mm256_loadu_si256( src128 ), mask1 ) );
//					}
//				}
//			}
//		}
//		// Take the remainder colors at the end of the row that weren't able to
//		// be included into the last 16 byte chunk:
//		if( avx_remainder > 0 )
//		{
//			for( INT32 x = (avx_blocks * 32); x < (options.sx * 4); x += 4 )
//			{
//				cpy_swap_v3( p_dst + x, src + x );
//				p_dst[x + 3] = b_force_alpha ? alpha_value : src[x + 3];
//			}
//		}
//	}
//
//	_mm256_zeroupper();
//	// Memory fence to make sure the stores are good:
//	_mm_mfence();
//}


//
// RGBA to MONO
//

//void	imgcon::flip_vertical( UINT8* src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, INT32 CONST options.sx, INT32 CONST options.sy )
//{
//	UINT8*	src_inv = src + src_pitch * options.sy;
//	for( INT32 j = options.sy; j > 0; --j )	//todo opt put this in the loop
//	{
//		src_inv -= src_pitch;
//		MEMCPY( dst, src_inv, src_pitch );
//		dst		+= dst_pitch;
//	}
//}


namespace {
// constants for RGB to Y conversion
// Ey = 0.299*Er + 0.587*Eg + 0.114*Eb
CONSTEXPR INT32 SCALING_LOG			= 15;
CONSTEXPR INT32 SCALING_FACTOR		= (1 << SCALING_LOG);
CONSTEXPR INT32 Y_R_SCALE_RGB		= (INT32)(0.299 * SCALING_FACTOR);
CONSTEXPR INT32 Y_G_SCALE_RGB		= (INT32)(0.587 * SCALING_FACTOR);
CONSTEXPR INT32 Y_B_SCALE_RGB		= (INT32)(0.114 * SCALING_FACTOR);

CONSTEXPR INT32 Y_RGB_SCALE_RGB[]	= { Y_R_SCALE_RGB, Y_G_SCALE_RGB, Y_B_SCALE_RGB };
CONSTEXPR INT32 Y_RGB_SCALE_BGR[]	= { Y_B_SCALE_RGB, Y_G_SCALE_RGB, Y_R_SCALE_RGB };
};

// rgba_r8_fast
// reference serial code that converts a given RGB image to a luma-only image
// both input and output image are unidimensional arrays of ImageWidth * ImageHeight pixels
// PixelOffset is the distance in bytes between two pixels in the input RGB image (RGB24 -> PixelOffset = 3, RGBA32 -> PixelOffset = 4)
void imgcon::rgba8_to_r8_fast( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, UINT32 CONST ch_nb, st_img_conv CONST & options )
{
	INT32 CONST * y_scale = Y_RGB_SCALE_RGB;
	if( options.b_swap_red_blue )
	{
		y_scale = Y_RGB_SCALE_BGR;
	}
	imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
	{
		IMGCON_LINE_PTRS( UINT8, UINT8 );
		for( auto i = 0; i < options.sx; ++i )
		{
			auto	val = (p_src[0] * y_scale[0]) + (p_src[1] * y_scale[1]) + (p_src[2] * y_scale[2]);
			p_src += ch_nb;
			val += 1 << (SCALING_LOG - 1);
			val >>= SCALING_LOG;
			if( val > 255 )
				val = 255;
			*p_dst = (UINT8)val;
			++p_dst;
		}
	});
}


// rgba_R8_sse2
// SSE2 version of Serial code
// both input and output image are unidimensional arrays of ImageWidth * ImageHeight pixels
// assumes that PixelOffset is 4 (RGBA image) and that ImageWidth * ImageHeight * PixelOffset is a multiple of 16
// does not assume that the input image is aligned on 16 bytes
void imgcon::rgba8_to_r8_sse2( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	INT32 CONST * y_scale = Y_RGB_SCALE_RGB;
	if( options.b_swap_red_blue )
		y_scale = Y_RGB_SCALE_BGR;

	auto CONST	RGBScale			= _mm_set_epi16( 0, y_scale[2], y_scale[1], y_scale[0], 0, y_scale[2], y_scale[1], y_scale[0] );
	auto CONST	ShiftScalingAdjust	= _mm_set1_epi32( 1 << (SCALING_LOG - 1) );

	// Batch up loads/stores into 16 byte chunks to use SSE2 efficiently:
	INT32 CONSTEXPR	block_byte_nb		= 16; 
	INT32 CONST		block_nb			= ( options.sx * 4 ) / block_byte_nb;
	INT32 CONST		remainder_byte_nb	= ( options.sx * 4 ) & ( block_byte_nb - 1);
	// x is computed for the dst
	INT32 CONST x_end = options.sx * 4;
	INT32 CONST x_begin = x_end - remainder_byte_nb;

	imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
	{ 
		IMGCON_LINE_PTRS( UINT8, UINT8 );
		auto	YImagePtr	= (INT32 *)p_dst;
		// Do conversions in batches of 16 bytes:
		if( block_nb > 0 )
		{
			__m128i CONST *	src128 = (__m128i *)p_src;
			// Increment by 16 bytes at a time:
			for( auto loop_nb = block_nb; loop_nb > 0; --loop_nb, ++src128 )
			{
				__m128i CONST RGBValue = _mm_loadu_si128( src128 );
				auto CONST LowRGBValue	= _mm_unpacklo_epi8( RGBValue, _mm_setzero_si128() );
				auto CONST HighRGBValue	= _mm_unpackhi_epi8( RGBValue, _mm_setzero_si128() );
				// int YValue = (SourceImagePtr[0] * Y_RED_SCALE  ) +
				//			 (SourceImagePtr[1] * Y_GREEN_SCALE) +
				//			 (SourceImagePtr[2] * Y_BLUE_SCALE );
				auto LowYValue		= _mm_madd_epi16( LowRGBValue, RGBScale );
				auto HighYValue		= _mm_madd_epi16( HighRGBValue, RGBScale );
				LowYValue			= _mm_add_epi32( LowYValue, _mm_slli_epi64( LowYValue, 32 ) );
				HighYValue			= _mm_add_epi32( HighYValue, _mm_slli_epi64( HighYValue, 32 ) );
				// YValue += 1 << (SCALING_LOG - 1);
				LowYValue			= _mm_add_epi32( LowYValue, ShiftScalingAdjust );
				HighYValue			= _mm_add_epi32( HighYValue, ShiftScalingAdjust );
				// YValue >>= SCALING_LOG;
				LowYValue			= _mm_srli_epi64( LowYValue, 32 + SCALING_LOG );
				HighYValue			= _mm_srli_epi64( HighYValue, 32 + SCALING_LOG );
				__m128i YValue		= _mm_packs_epi32( LowYValue, HighYValue );
				YValue				= _mm_packs_epi32( YValue, _mm_setzero_si128() );
				// if(YValue > 255)
				//	YValue = 255;
				YValue				= _mm_packus_epi16( YValue, YValue );
				// *YImagePtr		= (unsigned char)YValue;
				*YImagePtr			= _mm_cvtsi128_si32( YValue );
				YImagePtr++;
			}
		}
		// Take the remainder colors at the end of the row that weren't able to
		// be included into the last 16 byte chunk:
		if( remainder_byte_nb > 0 )
		{
			// x is computed for the src
			for( auto x = x_begin; x < x_end; x += 4 )
			{
				auto	val = (p_src[x + 0] * y_scale[0]) + (p_src[x + 1] * y_scale[1]) + (p_src[x + 2] * y_scale[2]);
				val += 1 << (SCALING_LOG - 1);
				val >>= SCALING_LOG;
				if( val > 255 )
					val = 255;
				p_dst[ x / 4 ] = val;
			}
		}
	});
}

// rgba_r8_sse3
// SSSE3 version of Serial code
// both input and output image are unidimensional arrays of ImageWidth * ImageHeight pixels
// assumes that PixelOffset is 4 (RGBA image) and that ImageWidth * ImageHeight * PixelOffset is a multiple of 16
// does not assume that the input image is aligned on 16 bytes
void imgcon::rgba8_to_r8_sse3( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	INT32 CONST * y_scale = Y_RGB_SCALE_RGB;
	if( options.b_swap_red_blue )
		y_scale = Y_RGB_SCALE_BGR;

	auto CONST	RGBScale			= _mm_set_epi16( 0, y_scale[2], y_scale[1], y_scale[0], 0, y_scale[2], y_scale[1], y_scale[0] );
	auto CONST	ShiftScalingAdjust	= _mm_set1_epi32( 1 << (SCALING_LOG - 1) );
	auto CONST	ZeroConst			= _mm_setzero_si128();

	// Batch up loads/stores into 16 byte chunks to use SSE2 efficiently: loads 4 RGBA pixels
	INT32 CONSTEXPR block_byte_nb		= 16; 
	INT32 CONST		block_nb			= ( options.sx * 4 ) / block_byte_nb;
	INT32 CONST		remainder_byte_nb	= ( options.sx * 4 ) & ( block_byte_nb - 1);
	// x is computed for the src
	INT32 CONST x_end = options.sx * 4;
	INT32 CONST x_beg = x_end - remainder_byte_nb;

	imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
	{
		IMGCON_LINE_PTRS( UINT8, UINT8 );
		// 4 pixels output, use a INT32 pointer
		auto	YImagePtr = (INT32 *)p_dst;

		// Do conversions in batches of 16 bytes:
		if( block_nb > 0 )
		{
			__m128i CONST *	src128 = (__m128i *)p_src;

			// Increment by 16 bytes at a time:
			for( auto loop_nb = block_nb; loop_nb > 0; --loop_nb, ++src128 )
			{
				__m128i CONST RGBValue = _mm_loadu_si128( src128 );
				//SourceImagePtr++;
				auto CONST LowRGBValue	= _mm_unpacklo_epi8( RGBValue, ZeroConst );
				auto CONST HighRGBValue	= _mm_unpackhi_epi8( RGBValue, ZeroConst );
				// int YValue = (SourceImagePtr[0] * Y_RED_SCALE  ) +
				//			 (SourceImagePtr[1] * Y_GREEN_SCALE) +
				//			 (SourceImagePtr[2] * Y_BLUE_SCALE );
				auto CONST LowYValue		= _mm_madd_epi16( LowRGBValue, RGBScale );
				auto CONST HighYValue		= _mm_madd_epi16( HighRGBValue, RGBScale );
				auto YValue			= _mm_hadd_epi32( LowYValue, HighYValue );
				// YValue += 1 << (SCALING_LOG - 1);
				YValue				= _mm_add_epi32( YValue, ShiftScalingAdjust );
				// YValue >>= SCALING_LOG;
				YValue				= _mm_srli_epi32( YValue, SCALING_LOG );
				YValue				= _mm_packs_epi32( YValue, YValue );
				// if(YValue > 255)
				//	YValue = 255;
				YValue				= _mm_packus_epi16( YValue, YValue );
				// *YImagePtr = (unsigned char)YValue;
				*YImagePtr			= _mm_cvtsi128_si32( YValue );
				YImagePtr++;
			}
		}
		// Take the remainder colors at the end of the row that weren't able to
		// be included into the last 16 byte chunk:
		if( remainder_byte_nb > 0 )
		{
			// x is computed for the src
			for( auto x = (options.sx * 4) - remainder_byte_nb; x < (options.sx * 4); x += 4 )
			{
				auto	val = (p_src[x + 0] * y_scale[0]) + (p_src[x + 1] * y_scale[1]) + (p_src[x + 2] * y_scale[2]);
				val += 1 << (SCALING_LOG - 1);
				val >>= SCALING_LOG;
				if( val > 255 )
					val = 255;
				p_dst[x / 4] = val;
			}
		}
	});
}

// rgba_r8_sse3_fast
// reduced precision SSSE3 version of Serial code that does NOT produce results that match serial code's ones.
// both input and output image are unidimensional arrays of ImageWidth * ImageHeight pixels
// assumes that PixelOffset is 4 (RGBA image) and that ImageWidth * ImageHeight * PixelOffset is a multiple of 16
// does not assume that the input image is aligned on 16 bytes
void imgcon::rgba8_to_r8_sse3_fast( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	auto CONST	SCALING_LOG		= 7;
	auto CONST	SCALING_FACTOR	= (1 << SCALING_LOG);
	auto CONST 	Y_RED_SCALE		= (INT32)(( options.b_swap_red_blue ? 0.114 : 0.299 ) * SCALING_FACTOR);
	auto CONST 	Y_GREEN_SCALE	= (INT32)(                              0.587   * SCALING_FACTOR);
	auto CONST 	Y_BLUE_SCALE	= (INT32)(( options.b_swap_red_blue ? 0.299 : 0.114 ) * SCALING_FACTOR);

	auto CONST	RGBScale			= _mm_set_epi8( 0, Y_BLUE_SCALE, Y_GREEN_SCALE, Y_RED_SCALE, 0, Y_BLUE_SCALE, Y_GREEN_SCALE, Y_RED_SCALE, 0, Y_BLUE_SCALE, Y_GREEN_SCALE, Y_RED_SCALE, 0, Y_BLUE_SCALE, Y_GREEN_SCALE, Y_RED_SCALE );
	auto CONST	ShiftScalingAdjust	= _mm_set1_epi16( 1 << (SCALING_LOG - 1) );

	// Batch up loads/stores into 16 byte chunks to use SSE2 efficiently: loads 4 RGBA pixels
	INT32 CONSTEXPR block_byte_nb		=  16; 
	INT32 CONST		block_nb			= ( options.sx * 4 ) / block_byte_nb;
	INT32 CONST		remainder_byte_nb	= ( options.sx * 4 ) & ( block_byte_nb - 1);
	INT32 CONST x_end = options.sx * 4;
	INT32 CONST x_begin = x_end - remainder_byte_nb;

	imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT
	{ 
		IMGCON_LINE_PTRS( UINT8, UINT8 );
		// 4 pixels output, use a INT32 pointer
		auto	YImagePtr	= (INT32 *)p_dst;
		// Do conversions in batches of 16 bytes:
		if( block_nb > 0 )
		{
			__m128i CONST *	RESTRICT src128 = (__m128i *)p_src;
			auto *			RESTRICT dst128 = (__m128i *)p_dst;

			// Increment by 16 bytes at a time:
			for( auto loop_nb = block_nb; loop_nb > 0; --loop_nb, ++dst128, ++src128 )
			{
				__m128i CONST RGBValue = _mm_loadu_si128( src128 );
				//SourceImagePtr++;
				auto CONST MultYValue = _mm_maddubs_epi16( RGBValue, RGBScale );
				// int YValue = (SourceImagePtr[0] * Y_RED_SCALE  ) +
				//			 (SourceImagePtr[1] * Y_GREEN_SCALE) +
				//			 (SourceImagePtr[2] * Y_BLUE_SCALE );
				auto YValue = _mm_hadd_epi16( MultYValue, MultYValue );
				YValue = _mm_add_epi16( YValue, ShiftScalingAdjust );
				YValue = _mm_srli_epi16( YValue, SCALING_LOG );
				// if( YValue > 255 )
				//	YValue = 255;
				YValue = _mm_packus_epi16( YValue, YValue );
				*YImagePtr = _mm_cvtsi128_si32( YValue );
				YImagePtr++;
			}
		}
		// Take the remainder colors at the end of the row that weren't able to
		// be included into the last 16 byte chunk:
		if( remainder_byte_nb > 0 )
		{
			//todo wrong too ?
			// x is computed for the src
			for( auto x = x_begin; x < x_end; x += 4 )
			{
				auto	val = (p_src[x + 0] * Y_RED_SCALE) + (p_src[x + 1] * Y_GREEN_SCALE) + (p_src[x + 2] * Y_BLUE_SCALE);
				val += 1 << (SCALING_LOG - 1);
				val >>= SCALING_LOG;
				if( val > 255 )
					val = 255;
				p_dst[x / 4] = val;
			}
		}
	});
}
//#include "immintrin.h"
//void imgcon::rgba_r8_avx2_fast( UINT8 CONST * RESTRICT src, INT32 CONST src_pitch, UINT8* RESTRICT dst, INT32 CONST dst_pitch, INT32 CONST options.sx, INT32 CONST options.sy, bool b_bgr )
//{
//	INT32 CONST SCALING_LOG = 7;
//	INT32 CONST SCALING_FACTOR = (1 << SCALING_LOG);
//	INT32	Y_RED_SCALE = (INT32)(0.299 * SCALING_FACTOR);
//	INT32	Y_GREEN_SCALE = (INT32)(0.587 * SCALING_FACTOR);
//	INT32	Y_BLUE_SCALE = (INT32)(0.114 * SCALING_FACTOR);
//	if( b_bgr )
//	{
//		Y_BLUE_SCALE = (INT32)(0.299 * SCALING_FACTOR);
//		Y_GREEN_SCALE = (INT32)(0.587 * SCALING_FACTOR);
//		Y_RED_SCALE = (INT32)(0.114 * SCALING_FACTOR);
//	}
//
//	__m256i RGBScale = _mm256_set_epi8( 0, Y_BLUE_SCALE, Y_GREEN_SCALE, Y_RED_SCALE, 0, Y_BLUE_SCALE, Y_GREEN_SCALE, Y_RED_SCALE, 0, Y_BLUE_SCALE, Y_GREEN_SCALE, Y_RED_SCALE, 0, Y_BLUE_SCALE, Y_GREEN_SCALE, Y_RED_SCALE, 0, Y_BLUE_SCALE, Y_GREEN_SCALE, Y_RED_SCALE, 0, Y_BLUE_SCALE, Y_GREEN_SCALE, Y_RED_SCALE, 0, Y_BLUE_SCALE, Y_GREEN_SCALE, Y_RED_SCALE, 0, Y_BLUE_SCALE, Y_GREEN_SCALE, Y_RED_SCALE );
//	__m256i ShiftScalingAdjust = _mm256_set1_epi16( 1 << (SCALING_LOG - 1) );
//	__m256i ZeroConst = _mm256_setzero_si256();
//
//	for( INT32 y = 0; y < options.sy; y++, src += src_pitch )
//	{
//		UINT8 *pBits = (UINT8*)dst + (y * dst_pitch);
//		INT32 *YImagePtr = (INT32 *)pBits;
//		// Batch up loads/stores into 16 byte chunks to use SSE2 efficiently:
//		INT32	ssse3blocks = (options.sx * 4) / 32;
//		INT32	ssse3remainder = (options.sx * 4) & 31;
//		// Do conversions in batches of 16 bytes:
//		if( ssse3blocks > 0 )
//		{
//			__m256i * RESTRICT src128 = (__m256i *)src;
//			__m256i * RESTRICT dst128 = (__m256i *)pBits;
//			bool	b_aligned = mem::IS_ALIGNED_16( src );
//
//			// Increment by 16 bytes at a time:
//			for( INT32 i = ssse3blocks; i > 0; --i, ++dst128, ++src128 )
//			{
//				__m256i RGBValue;
//				if( b_aligned )		{ RGBValue = _mm256_load_si256( src128 ); }
//				else					{ RGBValue = _mm256_loadu_si256( src128 ); }
//				__m256i MultYValue = _mm256_maddubs_epi16( RGBValue, RGBScale );
//				// int YValue = (SourceImagePtr[0] * Y_RED_SCALE  ) +
//				//			 (SourceImagePtr[1] * Y_GREEN_SCALE) +
//				//			 (SourceImagePtr[2] * Y_BLUE_SCALE );
//				__m256i YValue = _mm256_hadd_epi16( MultYValue, MultYValue );
//				YValue = _mm256_add_epi16( YValue, ShiftScalingAdjust );
//				YValue = _mm256_srli_epi16( YValue, SCALING_LOG );
//				// if( YValue > 255 )
//				//	YValue = 255;
//				YValue = _mm256_packus_epi16( YValue, YValue );
//				*YImagePtr =_mm_cvtsi128_si32(_mm256_castsi256_si128(YValue));// _mm256_extract_epi64( YValue );
//				YImagePtr++;
//				*YImagePtr = _mm_cvtsi128_si32( _mm256_extracti128_si256( YValue, 1 ) );// _mm256_extract_epi64( YValue );
//				YImagePtr++;
//
//			}
//		}
//		// Take the remainder colors at the end of the row that weren't able to
//		// be included into the last 16 byte chunk:
//		if( ssse3remainder > 0 )
//		{
//			for( INT32 x = (ssse3remainder * 32); x < (options.sx * 4); x += 4 )
//			{
//				INT32 val = (src[x + 0] * Y_RED_SCALE) + (src[x + 1] * Y_GREEN_SCALE) + (src[x + 2] * Y_BLUE_SCALE);
//				val += 1 << (SCALING_LOG - 1);
//				val >>= SCALING_LOG;
//				if( val > 255 )
//					val = 255;
//				pBits[x / 4] = val;
//			}
//		}
//
//	}
//	_mm256_zeroupper();
//	// Memory fence to make sure the stores are good:
//	_mm_mfence();
//}

void	imgcon::rgba8_to_r8( UINT8 CONST * src, INT32 CONST src_pitch, UINT8* dst, INT32 CONST dst_pitch, st_img_conv CONST & options )
{
	if( texture_flux_master->is_convert_to_rgb_sse3() )
	{
		if( texture_flux_master->is_convert_to_rgb_fast() )
			rgba8_to_r8_sse3_fast( src, src_pitch, dst, dst_pitch, options );
		else
			rgba8_to_r8_sse3( src, src_pitch, dst, dst_pitch, options );
	}
	else if( texture_flux_master->is_convert_to_rgb_sse2() )
		rgba8_to_r8_sse2( src, src_pitch, dst, dst_pitch, options );
	else if( texture_flux_master->is_convert_to_rgb_fast() )
		rgba8_to_r8_fast( src, src_pitch, dst, dst_pitch, 4, options );
	else
		imgcon::call_by_line_striped( options, [&](INT32 j) NOEXCEPT {
			bitcon::rgba8_to_r8_fast(src + (j * src_pitch), dst + (j * dst_pitch), options.sx ); });
}





