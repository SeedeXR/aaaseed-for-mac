#include "image/convert/bitmap_convert.h"
#include "color_space.h"

using namespace aaa;


namespace {
#if 0 // more correctbut slower
	FINLINE void make_rgba8(UINT32* CONST RESTRICT dst, INT32 y, INT32 r, INT32 g, INT32 b, UINT8 CONST alpha)
	{
		// add Y and downscale
		r = (y + r + (1 << (LUT_SHIFT-1))) >> color::YUV_LUT_SHIFT;
		g = (y + g + (1 << (LUT_SHIFT-1))) >> color::YUV_LUT_SHIFT;
		b = (y + b + (1 << (LUT_SHIFT-1))) >> color::YUV_LUT_SHIFT;

		// branchless 0..255 clamp using min/max trick
		r = (r & -(r >= 0)) | (-(r > 255) & 255);
		g = (g & -(g >= 0)) | (-(g > 255) & 255);
		b = (b & -(b >= 0)) | (-(b > 255) & 255);

		// pack RGBA
		*dst = PACK_RGBA( r,g,b, alpha );
	}
	FINLINE void make_bgra8(UINT32* CONST RESTRICT dst, INT32 y, INT32 r, INT32 g, INT32 b, UINT8 CONST alpha)
	{
		// add Y and downscale
		r = (y + r + (1 << (LUT_SHIFT-1))) >> color::YUV_LUT_SHIFT;
		g = (y + g + (1 << (LUT_SHIFT-1))) >> color::YUV_LUT_SHIFT;
		b = (y + b + (1 << (LUT_SHIFT-1))) >> color::YUV_LUT_SHIFT;

		// branchless clamp 0..255
		r = (r & -(r >= 0)) | (-(r > 255) & 255);
		g = (g & -(g >= 0)) | (-(g > 255) & 255);
		b = (b & -(b >= 0)) | (-(b > 255) & 255);

		// pack BGRA
		*dst = PACK_BGRA( r,g,b, alpha );
	}
#else
	FINLINE void	make_rgba8( UINT32 * CONST RESTRICT dst, INT32 CONST y, INT32 r, INT32 g, INT32 b, UINT8 CONST alpha )
	{
		r = (y + r) >> color::YUV_LUT_SHIFT;
		g = (y + g) >> color::YUV_LUT_SHIFT;
		b = (y + b) >> color::YUV_LUT_SHIFT;
		*dst = PACK_RGBA( CLAMP(r, 0, 255), CLAMP(g, 0, 255), CLAMP(b, 0, 255), alpha );
	}
	FINLINE void	make_bgra8( UINT32 * CONST RESTRICT dst, INT32 CONST y, INT32 r, INT32 g, INT32 b, UINT8 CONST alpha )
	{
		r = (y + r) >> color::YUV_LUT_SHIFT;
		g = (y + g) >> color::YUV_LUT_SHIFT;
		b = (y + b) >> color::YUV_LUT_SHIFT;
		*dst = PACK_BGRA( CLAMP(r, 0, 255), CLAMP(g, 0, 255), CLAMP(b, 0, 255), alpha );
	}
#endif
}


//used
void	bitcon::yuyv_to_r8_lut( UINT8 CONST * RESTRICT src8, UINT8* dst8, INT32 nb, UINT8 CONST * CONST lut )
{
	--dst8;
	for( ; nb > 0; --nb )
	{
		*++dst8 = lut[*src8];
		src8+=2;
	}
}



//
// reference implementation with float math to compare against
//
FINLINE UINT8 clamp_round_u8(float v)
{
    if (v <= 0.0f)   return 0;
    if (v >= 255.0f) return 255;
    return (UINT8)(v + 0.5f); // round-to-nearest
}
#if 0
FINLINE void yuv_to_rgba_ref(
    UINT8  y,
    UINT8  u,
    UINT8  v,
    UINT32* RESTRICT dst,
    UINT8  alpha
)
{
	color::yuv_to_rgb_params const& p = color::get_yuv_color_model_param_used();

    // Exact float math
    float Yf = (float(y) - float(p.y_sub)) * float(p.y_mul);
    float Uf = float(u) - 128.0f;
    float Vf = float(v) - 128.0f;

    float Rf = Yf + float(p.r_cr) * Vf;
    float Gf = Yf + float(p.g_cb) * Uf + float(p.g_cr) * Vf;
    float Bf = Yf + float(p.b_cb) * Uf;

    UINT8 r = clamp_round_u8(Rf);
    UINT8 g = clamp_round_u8(Gf);
    UINT8 b = clamp_round_u8(Bf);

    *dst = PACK_RGBA( r,g,b, alpha );
}
// Y plane (first row)m  U plane (width/2 contiguous), V plane (width/2 contiguous)  
void	bitcon::i420_to_rgba8_ref(	UINT8 CONST * RESTRICT src_y8,  UINT8 CONST * RESTRICT src_u8, UINT8 CONST * RESTRICT src_v8, 
							UINT32*	 RESTRICT dst32, INT32  CONST dst32_pitch,    // pixels pitch (may be negative)
							st_img_conv CONST & options )
{
	const INT32 width = ABS(dst32_pitch);
    if( width < 1 )
		return; // need at least one 16-pixel block
    UINT32* RESTRICT dst32_next = dst32 + dst32_pitch;
	INT32	loop = bitcon::shift_len_check_align( __FUNCTION__, width, 1 );

	for (; loop > 0; --loop)
	{
		UINT8 u = *src_u8++;
		UINT8 v = *src_v8++;

		yuv_to_rgba_ref(*src_y8,			u, v, dst32++,		options.alpha );
		yuv_to_rgba_ref(*(src_y8+width),	u, v, dst32_next++, options.alpha );
		src_y8++;
		yuv_to_rgba_ref(*src_y8,			u, v, dst32++,		options.alpha );
		yuv_to_rgba_ref(*(src_y8+width),	u, v, dst32_next++,	options.alpha );
		src_y8++;
	}
}
#else
static	FINLINE	void	uv_2_rgb_to_add( UINT8 CONST u_in, UINT8 CONST v_in, REAL &r, REAL &g, REAL &b )
{
	color::yuv_to_rgb_params const& p = color::get_yuv_color_model_param_used();
	REAL CONST u = REAL(u_in -128);
	REAL CONST v = REAL(v_in -128);

	r =                  REAL(p.r_cr)*v;
	g = REAL(p.g_cb)*u + REAL(p.g_cr)*v;
	b = REAL(p.b_cb)*u;
}
FINLINE void yrgb_to_rgba_ref(
    UINT8  y,
    REAL fr, REAL fg, REAL fb,
    UINT32* dst,
    UINT8  alpha
)
{
	color::yuv_to_rgb_params const& p = color::get_yuv_color_model_param_used();

    // Exact float math
    float fy = (float(y) - float(p.y_sub)) * float(p.y_mul);

    UINT8 r = clamp_round_u8( fy + fr );
    UINT8 g = clamp_round_u8( fy + fg );
    UINT8 b = clamp_round_u8( fy + fb );

    *dst = PACK_RGBA( r,g,b, alpha );
}
 // Y plane (first row)m  U plane (width/2 contiguous), V plane (width/2 contiguous) 
void	bitcon::i420_to_rgba8_ref(	UINT8 CONST * RESTRICT src_y8,  UINT8 CONST * RESTRICT src_u8, UINT8 CONST * RESTRICT src_v8, 
							UINT32*	 dst32, INT32  CONST dst32_pitch,    // pixels pitch (may be negative)
							st_img_conv CONST & options )
{
	const INT32 width = ABS(dst32_pitch);
    if( width < 1 )
		return; // need at least one 16-pixel block
    UINT32* RESTRICT dst32_next = dst32 + dst32_pitch;
	INT32	loop = bitcon::shift_len_check_align( __FUNCTION__, width, 1 );

	for (; loop > 0; --loop)
	{
		UINT8 u = *src_u8++;
		UINT8 v = *src_v8++;
		REAL r,g,b;
		uv_2_rgb_to_add( u,v, r,g,b );

		yrgb_to_rgba_ref(*src_y8,			r,g,b, dst32++,		 options.alpha );
		yrgb_to_rgba_ref(*(src_y8+width),	r,g,b, dst32_next++, options.alpha );
		src_y8++;
		yrgb_to_rgba_ref(*src_y8,			r,g,b, dst32++,		 options.alpha );
		yrgb_to_rgba_ref(*(src_y8+width),	r,g,b, dst32_next++, options.alpha );
		src_y8++;
	}
}
#endif

//	---------------------------- AVX2 stuff ----------------------------
//
// here we do i420/nv12/yuyv/uyvy_to_rgba8_avx2()
template <bool B, typename T>
constexpr FINLINE const T& select_value(const T& value_if_true, const T& value_if_false)
{
    if constexpr (B)
        return value_if_true;
    else
        return value_if_false;
}
enum E_UV_TYPE { UV_SEPARATE, UV_FUSED, UV_YUYV, UV_UYVY };

// Shuffle masks for RGBA/BGRA
//const __m128i shuffle_rgba = _mm_setr_epi8( 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15);
static const __m128i shuffle_bgra		= _mm_setr_epi8( 2,1,0,3,6,5,4,7,10,9,8,11,14,13,12,15);
static const __m256i shuffle_bgra_256	= _mm256_set_m128i(shuffle_bgra, shuffle_bgra);
// Extract Y bytes
static const __m128i shuffle_yuyv_y = _mm_setr_epi8(0,2,4,6,8,10,12,14, -1,-1,-1,-1,-1,-1,-1,-1);
// Extract U and duplicate horizontally
static const __m128i shuffle_yuyv_u = _mm_setr_epi8(1,1,5,5,9,9,13,13, -1,-1,-1,-1,-1,-1,-1,-1);
// Extract V and duplicate horizontally
static const __m128i shuffle_yuyv_v = _mm_setr_epi8(3,3,7,7,11,11,15,15, -1,-1,-1,-1,-1,-1,-1,-1);
// Extract Y bytes
static const __m128i shuffle_uyvy_y = _mm_setr_epi8(1,3,5,7,9,11,13,15, -1,-1,-1,-1,-1,-1,-1,-1);
// Extract U and duplicate horizontally
static const __m128i shuffle_uyvy_u = _mm_setr_epi8(0,0,4,4,8,8,12,12, -1,-1,-1,-1,-1,-1,-1,-1);
// Extract V and duplicate horizontally
static const __m128i shuffle_uyvy_v = _mm_setr_epi8(2,2,6,6,10,10,14,14, -1,-1,-1,-1,-1,-1,-1,-1);



// YUV->RGB coefficients scaled by 256
		__m256i avx2_y_mul;	// scale factor for Y
		__m256i avx2_y_sub;	// Y offset (16 for limited range)
		__m256i avx2_r_cr;	// V → R contribution
		__m256i avx2_g_cb;	// U → G contribution
		__m256i avx2_g_cr;	// V → G contribution
		__m256i avx2_b_cb;	// U → B contribution
const	__m256i avx2_128   = _mm256_set1_epi32(128);  // center chroma

FINLINE int fp_to_int32_x8(double v) { return (int)(v * 256. + (v >= 0.0 ? 0.5 : -0.5)); }

void bitcon::update_convert_yuv_to_rgb()
{
	color::yuv_to_rgb_params CONST & p = color::get_yuv_color_model_param_used();
    avx2_y_mul = _mm256_set1_epi32(fp_to_int32_x8(p.y_mul));	// scale factor for Y
    avx2_y_sub = _mm256_set1_epi32((int)p.y_sub);				// Y offset (16 for limited range)
    avx2_r_cr  = _mm256_set1_epi32(fp_to_int32_x8(p.r_cr));		// V → R contribution
    avx2_g_cb  = _mm256_set1_epi32(fp_to_int32_x8(p.g_cb));		// U → G contribution
    avx2_g_cr  = _mm256_set1_epi32(fp_to_int32_x8(p.g_cr));		// V → G contribution
    avx2_b_cb  = _mm256_set1_epi32(fp_to_int32_x8(p.b_cb));		// U → B contribution
}

// ---------------------------- Helper functions ----------------------------

// Helper: load and upsample 8 NV12 UV samples to two __m256i for U and V
FINLINE void load_nv12_uv_8x_to_16x_avx2(
    UINT8 CONST* CONST src_uv,  // Pointer to 16 bytes: U0 V0 U1 V1 ...
    __m256i& u_lo,
    __m256i& u_hi,
    __m256i& v_lo,
    __m256i& v_hi)
{
    // Load 16 bytes: U0 V0 U1 V1 ...
    __m128i uv = _mm_loadu_si128((const __m128i*)src_uv);
    // Extract U bytes (even indices)
    __m128i u8 = _mm_and_si128(uv, _mm_set1_epi16(0x00FF));
    // Extract V bytes (odd indices)
    __m128i v8 = _mm_srli_epi16(uv, 8);

    // Pack down to bytes
    u8 = _mm_packus_epi16(u8, u8); // u0..u7
    v8 = _mm_packus_epi16(v8, v8); // v0..v7

    // Duplicate each chroma sample: u0 u0 u1 u1 ...
    __m128i u16 = _mm_unpacklo_epi8(u8, u8);
    __m128i v16 = _mm_unpacklo_epi8(v8, v8);

    // Expand to 32-bit and split
    u_lo = _mm256_sub_epi32(_mm256_cvtepu8_epi32(u16), avx2_128);
    u_hi = _mm256_sub_epi32(_mm256_cvtepu8_epi32(_mm_srli_si128(u16, 8)), avx2_128);

    v_lo = _mm256_sub_epi32(_mm256_cvtepu8_epi32(v16), avx2_128);
    v_hi = _mm256_sub_epi32(_mm256_cvtepu8_epi32(_mm_srli_si128(v16, 8)), avx2_128);
}

// Helper: load and upsample 8 U or V samples to two __m256i (lower 8 pixels, upper 8 pixels)
FINLINE void load_u_or_v_8x_to_16x_avx2(
    UINT8 CONST * CONST RESTRICT src,     // Pointer to 8 bytes of U or V
    __m256i& lo,	// Output: lower 8 pixels (expanded)
    __m256i& hi		// Output: upper 8 pixels (expanded)
)
{
    __m128i uv8 = _mm_loadl_epi64((const __m128i*)src);
    __m128i uv16 = _mm_unpacklo_epi8(uv8, uv8);
    lo = _mm256_cvtepu8_epi32(uv16);                     
    hi = _mm256_cvtepu8_epi32(_mm_srli_si128(uv16, 8)); 
    lo = _mm256_sub_epi32(lo, avx2_128);
    hi = _mm256_sub_epi32(hi, avx2_128);
}

FINLINE void compute_rgb_add_from_uv_avx2( __m256i const u32, __m256i const v32, __m256i& r_add, __m256i& g_add, __m256i& b_add )
{
    r_add = _mm256_mullo_epi32(v32, avx2_r_cr);
    g_add = _mm256_add_epi32(_mm256_mullo_epi32(u32, avx2_g_cb), _mm256_mullo_epi32(v32, avx2_g_cr));
    b_add = _mm256_mullo_epi32(u32, avx2_b_cb);
}

//
// AVX2 very fast version using template
// 
// REQUIREMENT:
constexpr uint64_t pack_alpha8( const UINT8* src_y, const UINT8* RESTRICT alpha_lut)
{
    return	(uint64_t)alpha_lut[src_y[0]]
			| ((uint64_t)alpha_lut[src_y[1]] << 8)
			| ((uint64_t)alpha_lut[src_y[2]] << 16)
			| ((uint64_t)alpha_lut[src_y[3]] << 24)
			| ((uint64_t)alpha_lut[src_y[4]] << 32)
			| ((uint64_t)alpha_lut[src_y[5]] << 40)
			| ((uint64_t)alpha_lut[src_y[6]] << 48)
			| ((uint64_t)alpha_lut[src_y[7]] << 56);
}
//if constexpr (B_LUT)
//{
//	alignas(16) UINT8 alpha[8];
//	for (int i = 0; i < 8; ++i)
//		alpha[i] = alpha_lut[src_y[i]];
//	a8 = _mm_loadl_epi64((const __m128i*)alpha);
//}

// Helper: convert 8 Y pixels + UV contributions into RGBA8
template<bool B_FLIP_RB, bool B_DST_ALIGN >
FINLINE void compute_y32_add_rgb_pack_store_8x( UINT32* CONST RESTRICT dst, __m256i y32, __m256i CONST r_add, __m256i CONST g_add, __m256i CONST b_add, __m128i CONST a8	) 
{
	y32 = _mm256_mullo_epi32( _mm256_sub_epi32(y32, avx2_y_sub), avx2_y_mul);

    // Add UV contributions and shift
    __m256i r = _mm256_srai_epi32(_mm256_add_epi32(y32, r_add), 8);
    __m256i g = _mm256_srai_epi32(_mm256_add_epi32(y32, g_add), 8);
    __m256i b = _mm256_srai_epi32(_mm256_add_epi32(y32, b_add), 8);

    // Pack 32-bit -> 16-bit -> 8-bit
    __m128i r16 = _mm_packus_epi32(_mm256_castsi256_si128(r), _mm256_extracti128_si256(r, 1));
    __m128i g16 = _mm_packus_epi32(_mm256_castsi256_si128(g), _mm256_extracti128_si256(g, 1));
    __m128i b16 = _mm_packus_epi32(_mm256_castsi256_si128(b), _mm256_extracti128_si256(b, 1));

    r16 = _mm_packus_epi16(r16, r16);
    g16 = _mm_packus_epi16(g16, g16);
    b16 = _mm_packus_epi16(b16, b16);

    // Conditional unpacking depending on B_FLIP_RB to avoid shuffle
    __m128i rg, ba;
    if constexpr (B_FLIP_RB)
	{ // BGRA: swap R and B channels during unpack
        rg = _mm_unpacklo_epi8(b16, g16);  // B & G
        ba = _mm_unpacklo_epi8(r16, a8);   // R & A
    }
	else
	{ // RGBA: normal order
        rg = _mm_unpacklo_epi8(r16, g16);  // R & G
        ba = _mm_unpacklo_epi8(b16, a8);   // B & A
    }

    // Build final 128-bit RGBA halves
    __m128i rgba_lo = _mm_unpacklo_epi16(rg, ba);
    __m128i rgba_hi = _mm_unpackhi_epi16(rg, ba);

    // Merge into a single 256-bit register
#if 0
    __m256i rgba = _mm256_set_m128i(rgba_hi, rgba_lo);
#else
	__m256i rgba = _mm256_castsi128_si256(rgba_lo);
	rgba = _mm256_inserti128_si256(rgba, rgba_hi, 1);

#endif
    // Store aligned or unaligned
    if constexpr (B_DST_ALIGN)
        _mm256_store_si256((__m256i*)dst, rgba);
    else
        _mm256_storeu_si256((__m256i*)dst, rgba);
}


/*	ok but slower
	if constexpr (B_FLIP_RB)	// Only compute shuffle if flipping is needed
		_mm_storeu_si128((__m128i*)dst, _mm_shuffle_epi8(rgba_lo, shuffle_bgra) );
	else
		_mm_storeu_si128((__m128i*)dst, rgba_lo);
	dst += 4;
	if constexpr (B_FLIP_RB)	// Only compute shuffle if flipping is needed
		_mm_storeu_si128((__m128i*)dst, _mm_shuffle_epi8(rgba_hi, shuffle_bgra) );
	else
		_mm_storeu_si128((__m128i*)dst, rgba_hi);
	dst += 4;
*/
// Helper: convert 8 Y pixels + UV contributions into RGBA8
template<bool B_LUT, bool B_FLIP_RB, bool B_DST_ALIGN >
FINLINE void process_y_block_8x_with_int32(
	UINT8 CONST * CONST RESTRICT src_y, UINT32*& RESTRICT dst,
	__m256i CONST & r_add, __m256i CONST & g_add, __m256i CONST & b_add, __m128i & a8, UINT8 CONST* CONST alpha_lut
) 
{
    // Load 8 Y pixels
    __m128i y8 = _mm_loadl_epi64((const __m128i*)src_y);

	// Load or compute alpha if using LUT
	if constexpr (B_LUT)
		a8 = _mm_cvtsi64_si128(pack_alpha8(src_y, alpha_lut));
	//	a8 = _mm_loadl_epi64((const __m128i*)&pack_alpha8(src_y, alpha_lut));uint64_t y64 = (uint64_t)_mm_cvtsi128_si64(y8);

    // Convert Y to 32-bit and apply scaling
    __m256i y32 = _mm256_cvtepu8_epi32(y8);

	compute_y32_add_rgb_pack_store_8x< B_FLIP_RB, B_DST_ALIGN >( dst, y32, r_add,g_add,b_add, a8 );
    dst += 8;
}

template< E_UV_TYPE UV_TYPE, bool B_LUT, bool B_FLIP_RB, bool B_DST_ALIGN>
FINLINE void to_rgba8_by_row_yuyv_avx2_8x( INT32 loop_hori, UINT32* RESTRICT dst, const UINT8* RESTRICT src_yuyv, const st_img_conv& options )
{
    __m128i a8;
    if constexpr (!B_LUT)
        a8 = _mm_set1_epi8((char)options.alpha);

	for (INT32 i = loop_hori; i > 0; --i)
    {
        // Load 8 YUYV pixels = 16 bytes
        // Layout: Y0 U0 Y1 V0 Y2 U1 Y3 V1 ...
        __m128i yuyv = _mm_loadu_si128((const __m128i*)src_yuyv);

		__m128i y8,u8,v8;
		// Extract Y, U, V
		if constexpr (UV_TYPE == UV_YUYV)
		{
			y8 = _mm_shuffle_epi8(yuyv, shuffle_yuyv_y);
			u8 = _mm_shuffle_epi8(yuyv, shuffle_yuyv_u);
			v8 = _mm_shuffle_epi8(yuyv, shuffle_yuyv_v);
		}
		else
		{
			y8 = _mm_shuffle_epi8(yuyv, shuffle_uyvy_y);
			u8 = _mm_shuffle_epi8(yuyv, shuffle_uyvy_u);
			v8 = _mm_shuffle_epi8(yuyv, shuffle_uyvy_v);
		}
        // Alpha from LUT or constant
        if constexpr (B_LUT)
        {
#if 0
            alignas(8) UINT8 y_tmp[8];
            _mm_storel_epi64((__m128i*)y_tmp, y8);
            a8 = _mm_cvtsi64_si128(pack_alpha8(y_tmp, options.lut));
#else
			uint64_t y64 = (uint64_t)_mm_cvtsi128_si64(y8);
			UINT8 const* yptr = reinterpret_cast<UINT8 const*>(&y64); // char aliasing OK
			a8 = _mm_cvtsi64_si128(pack_alpha8(yptr, options.lut));
#endif
        }

        // Expand to 32-bit and apply offsets
       
        __m256i u32 = _mm256_sub_epi32(_mm256_cvtepu8_epi32(u8), avx2_128);
        __m256i v32 = _mm256_sub_epi32(_mm256_cvtepu8_epi32(v8), avx2_128);

		__m256i r_add, g_add, b_add;
		compute_rgb_add_from_uv_avx2( u32,v32, r_add,g_add,b_add );
		__m256i y32 = _mm256_cvtepu8_epi32(y8);
		compute_y32_add_rgb_pack_store_8x< B_FLIP_RB, B_DST_ALIGN >( dst, y32, r_add,g_add,b_add, a8 );

        src_yuyv += 16; // 8 pixels
        dst      += 8;
    }
}

template< E_UV_TYPE UV_TYPE, bool B_LUT, bool B_FLIP_RB, bool B_DST_ALIGN >
void to_rgba8_by_row_yuv_avx2_8x( const INT32 loop_hori, INT32 CONST sx, UINT32 * dst0, UINT32 * dst1,
								const uint8_t* RESTRICT src_y8, const uint8_t* RESTRICT src_u8, const uint8_t* RESTRICT src_v8, st_img_conv CONST & options )
{
    // Alpha initialization
    __m128i a8;
    if constexpr (!B_LUT)
        a8 = _mm_set1_epi8((char)options.alpha);

	__m256i r_add, g_add, b_add;
	auto process_y_block_8x = [&]( UINT8 CONST * CONST src_y8, UINT32*& dst) constexpr
		{  process_y_block_8x_with_int32< B_LUT, B_FLIP_RB, B_DST_ALIGN >( src_y8,dst, r_add,g_add,b_add, a8, options.lut ); };

    for (INT32 i = loop_hori; i > 0; --i)
	{
        __m256i u32_lo, u32_hi;
        __m256i v32_lo, v32_hi;
        // Load UV depending on format
        if constexpr (UV_TYPE == UV_FUSED)
		{
            load_nv12_uv_8x_to_16x_avx2(src_u8, u32_lo, u32_hi, v32_lo, v32_hi);
            src_u8 += 16;
        }
		else
		{
            load_u_or_v_8x_to_16x_avx2(src_u8, u32_lo, u32_hi);
            load_u_or_v_8x_to_16x_avx2(src_v8, v32_lo, v32_hi);
            src_u8 += 8;
            src_v8 += 8;
        }

        compute_rgb_add_from_uv_avx2(	u32_lo, v32_lo,		r_add, g_add, b_add	);
        process_y_block_8x( src_y8,      dst0 );
        process_y_block_8x( src_y8 + sx, dst1 );
        src_y8 += 8;

        compute_rgb_add_from_uv_avx2(	u32_hi, v32_hi,		r_add, g_add, b_add	);
        process_y_block_8x( src_y8,      dst0 );
        process_y_block_8x( src_y8 + sx, dst1 );
        src_y8 += 8;
    }
}

template< E_UV_TYPE UV_TYPE, bool B_LUT, bool B_FLIP_RB, bool B_DST_ALIGN >
void to_rgba8_avx2_low( INT32 line_begin, INT32 line_nb, UINT32* RESTRICT dst32, INT32 CONST dst32_pitch, st_img_conv CONST & options )
{
    const INT32 sx = options.sx;
    INT32 loop_hori;
	const uint8_t* RESTRICT src_y8;
	const uint8_t* RESTRICT src_u8;
	const uint8_t* RESTRICT src_v8;
	INT32 sx_u; 
	if constexpr (UV_TYPE >= UV_YUYV)
	{
		loop_hori = bitcon::shift_len_check_align(__FUNCTION__, sx, 3);
		src_y8 = options.src_a + line_begin * (sx<<1);
	}
	else
	{
		loop_hori = bitcon::shift_len_check_align(__FUNCTION__, sx, 4);
		src_y8 = options.src_a + line_begin * sx;
		if constexpr ( UV_TYPE == UV_FUSED )
		{
			sx_u = sx;	//this is for 2 lines of y
			src_v8 = nullptr; // initialized to remove warning
		}
		else
		{
			sx_u = sx >> 1; //this is for 2 lines of y
			src_v8 = options.src_c + line_begin * (sx_u >> 1);
		}
		src_u8 = options.src_b + line_begin * (sx_u >> 1);
	}

    dst32 += line_begin * dst32_pitch;
    //// Alpha initialization
    //__m128i a8;
    //if constexpr (!B_LUT)
    //    a8 = _mm_set1_epi8((char)options.alpha);

    INT32 line_next = std::min(line_begin + line_nb, options.sy);
#if AAA_DEBUG()
    if( line_begin >= line_next )
        DBG_PRINT_STRING("%s() problem : line_begin %d --- line_end %d", __FUNCTION__, line_begin, line_next);
#endif
    for( INT32 j = line_begin; j < line_next; j += 2 )
	{
		if constexpr (UV_TYPE >= UV_YUYV)
		{
			to_rgba8_by_row_yuyv_avx2_8x<UV_TYPE, B_LUT, B_FLIP_RB, B_DST_ALIGN>( loop_hori, dst32, src_y8, options);
			dst32 += dst32_pitch ;
			src_y8 += sx << 1;
			to_rgba8_by_row_yuyv_avx2_8x<UV_TYPE, B_LUT, B_FLIP_RB, B_DST_ALIGN>( loop_hori, dst32, src_y8, options);
			dst32 += dst32_pitch ;
			src_y8 += sx << 1;
		}
		else
		{
			to_rgba8_by_row_yuv_avx2_8x< UV_TYPE, B_LUT, B_FLIP_RB, B_DST_ALIGN > ( loop_hori, sx, dst32, dst32 + dst32_pitch, src_y8, src_u8, src_v8, options );
		
			src_y8 += sx*2;
			src_u8 += sx_u;
			if constexpr ( UV_TYPE == UV_SEPARATE )
				src_v8 += sx_u;
			dst32 += dst32_pitch * 2;
		}
    }
}
template<E_UV_TYPE UV_TYPE>
FINLINE void dispatch_yuv_to_rgba8_avx2( INT32 line_begin, INT32 line_nb, UINT32* RESTRICT dst32, INT32 CONST dst32_pitch, st_img_conv CONST& options)
{
	const bool b_dst_align =	((reinterpret_cast<std::uintptr_t>(dst32) & 0x1F) == 0)
							&&	(((dst32_pitch * sizeof(UINT32)) & 0x1F) == 0); // 32-byte aligned

	if( options.b_lut_active )
		if( b_dst_align )
			if( options.b_swap_red_blue )
				to_rgba8_avx2_low<UV_TYPE, true, true,  true >(line_begin, line_nb, dst32, dst32_pitch, options);
			else
				to_rgba8_avx2_low<UV_TYPE, true, false, true >(line_begin, line_nb, dst32, dst32_pitch, options);
		else
			if( options.b_swap_red_blue )
				to_rgba8_avx2_low<UV_TYPE, true, true,  false>(line_begin, line_nb, dst32, dst32_pitch, options);
			else
				to_rgba8_avx2_low<UV_TYPE, true, false, false>(line_begin, line_nb, dst32, dst32_pitch, options);
	else
		if (b_dst_align)
			if( options.b_swap_red_blue )
				to_rgba8_avx2_low<UV_TYPE, false, true,  true >(line_begin, line_nb, dst32, dst32_pitch, options);
			else
				to_rgba8_avx2_low<UV_TYPE, false, false, true >(line_begin, line_nb, dst32, dst32_pitch, options);
		else
			if( options.b_swap_red_blue )
				to_rgba8_avx2_low<UV_TYPE, false, true,  false>(line_begin, line_nb, dst32, dst32_pitch, options);
			else
				to_rgba8_avx2_low<UV_TYPE, false, false, false>(line_begin, line_nb, dst32, dst32_pitch, options);
}
void bitcon::i420_to_rgba8_avx2(INT32 line_begin, INT32 line_nb, UINT32* dst32, INT32 CONST dst32_pitch, st_img_conv CONST& options)
{ dispatch_yuv_to_rgba8_avx2< UV_SEPARATE >(line_begin, line_nb, dst32, dst32_pitch, options); }
void bitcon::nv12_to_rgba8_avx2(INT32 line_begin, INT32 line_nb, UINT32* dst32, INT32 CONST dst32_pitch, st_img_conv CONST& options)
{ dispatch_yuv_to_rgba8_avx2< UV_FUSED    >(line_begin, line_nb, dst32, dst32_pitch, options); }
void bitcon::yuyv_to_rgba8_avx2(INT32 line_begin, INT32 line_nb, UINT32* dst32, INT32 CONST dst32_pitch, st_img_conv CONST& options)
{ dispatch_yuv_to_rgba8_avx2< UV_YUYV     >(line_begin, line_nb, dst32, dst32_pitch, options); }
void bitcon::uyvy_to_rgba8_avx2(INT32 line_begin, INT32 line_nb, UINT32* dst32, INT32 CONST dst32_pitch, st_img_conv CONST& options)
{ dispatch_yuv_to_rgba8_avx2< UV_UYVY     >(line_begin, line_nb, dst32, dst32_pitch, options); }



//	---------------------------- Scalar stuff ----------------------------
//
// here we do i420/nv12_to_rgba8_line_two() which process 2 lines at a time
template< E_UV_TYPE UV_TYPE, bool B_LUT, auto fn_out >
FINLINE void	yuv_to_rgba8_line_two( UINT8 const * RESTRICT src8, UINT8 const * RESTRICT src_a8, UINT8 const * RESTRICT src_b8, UINT32* RESTRICT dst32, INT32 const dst32_pitch, st_img_conv const & options )
{
	INT32 r,g,b;
	INT32 y8_pitch = ABS(dst32_pitch);
	INT32 loop = bitcon::shift_len_check_align( __FUNCTION__, y8_pitch, 1 );

	UINT8 const * RESTRICT lut;
	UINT8 alpha;
	if constexpr (B_LUT)
		lut = options.lut;
	else
		alpha = options.alpha;

	auto write_pixel = [&]( UINT8 y, UINT32* dst ) constexpr { fn_out(dst, color::compute_y(y), r,g,b, select_value<B_LUT>(lut[y], alpha) ); };

	for( ; loop > 0; --loop )
	{
		UINT8 const u = *src_a8++;
		UINT8 v;
		if constexpr (UV_TYPE==UV_FUSED)
			v = *src_a8++;
		else
			v = *src_b8++;
		color::uv_2_rgb_to_add( u,v, r,g,b );

		//	PIXEL 1 on both lines
		write_pixel( *src8				,dst32					);
		write_pixel( *(src8+y8_pitch)	,dst32 + dst32_pitch	);
		++dst32;
		++src8;
		//	PIXEL 2 on both lines
		write_pixel( *src8				,dst32					);
		write_pixel( *(src8+y8_pitch)	,dst32 + dst32_pitch	);
		++dst32;
		++src8;
	}
}
template<E_UV_TYPE UV_TYPE>
FINLINE void dispatch_yuv_to_rgba8_line_two( UINT8 CONST * CONST RESTRICT src8, UINT8 CONST * CONST RESTRICT src_a8, UINT8 CONST * CONST RESTRICT src_b8, UINT32* CONST dst32, INT32 CONST dst32_pitch, st_img_conv CONST & options )
{
	const bool b_dst_align =	((reinterpret_cast<std::uintptr_t>(dst32) & 0x1F) == 0)
							&&	(((dst32_pitch * sizeof(UINT32)) & 0x1F) == 0); // 32-byte aligned

    if( options.b_lut_active )
		if (options.b_swap_red_blue)
			yuv_to_rgba8_line_two<UV_TYPE, true, make_bgra8 >(src8, src_a8, src_b8, dst32, dst32_pitch, options );
		else
			yuv_to_rgba8_line_two<UV_TYPE, true, make_rgba8 >(src8, src_a8, src_b8, dst32, dst32_pitch, options );
	else
		if (options.b_swap_red_blue)
			yuv_to_rgba8_line_two<UV_TYPE, false, make_bgra8 >(src8, src_a8, src_b8, dst32, dst32_pitch, options );
		else
			yuv_to_rgba8_line_two<UV_TYPE, false, make_rgba8 >(src8, src_a8, src_b8, dst32, dst32_pitch, options);
}
void	bitcon::i420_to_rgba8_line_two( UINT8 CONST * CONST src8, UINT8 CONST * CONST src_a8, UINT8 CONST * CONST src_b8,	UINT32* CONST dst32, INT32 CONST dst32_pitch, st_img_conv CONST & options )
{	dispatch_yuv_to_rgba8_line_two< UV_SEPARATE>( src8, src_a8, src_b8, dst32, dst32_pitch, options );	}	
void	bitcon::nv12_to_rgba8_line_two( UINT8 CONST * CONST src8, UINT8 CONST * CONST src_a8,								UINT32* CONST dst32, INT32 CONST dst32_pitch, st_img_conv CONST & options )
{	dispatch_yuv_to_rgba8_line_two< UV_FUSED>( src8, src_a8, nullptr, dst32, dst32_pitch, options );	}



// here we do yuyv/uyuy_to_rgba8_line_one() which process 2 lines at a time
template<E_UV_TYPE UV_TYPE, bool B_LUT, auto fn_out >
void yuyv_to_rgba8_line_one_low( UINT32 CONST * RESTRICT src32, UINT32* RESTRICT dst32, INT32 nb_loop, st_img_conv CONST & options )
{
	INT32	u,v;
	INT32	r,g,b;

	UINT8 const * RESTRICT lut;
	UINT8 alpha;
	if constexpr (B_LUT)
		lut = options.lut;
	else
		alpha = options.alpha;

	auto write_pixel = [&](UINT8 y, UINT32* dst) constexpr { fn_out(dst, color::compute_y(y), r,g,b, select_value<B_LUT>(lut[y], alpha) ); };
	for( ; nb_loop > 0; --nb_loop )
	{
		INT32 y,y2;
		//	Color
		if constexpr (UV_TYPE == UV_YUYV)		UNPACK_UINT32( *src32, y,u,y2,v );
		else if constexpr (UV_TYPE == UV_UYVY)	UNPACK_UINT32( *src32, u,y,v,y2 );
		++src32;

		color::uv_2_rgb_to_add( u,v, r,g,b );
		//	PIXEL 1
		write_pixel( y	,dst32 );
		++dst32;
		//	PIXEL 2
		write_pixel( y2	,dst32 );
		++dst32;
	}
}
template< E_UV_TYPE UV_TYPE, bool B_LUT >
FINLINE void dispatch_yuyv_to_rgba8_line_one( UINT32 CONST * RESTRICT src32, UINT32* RESTRICT dst32, INT32 nb, st_img_conv CONST & options )
{
	nb = bitcon::shift_len_check_align( __FUNCTION__, nb, 1 );
	if( options.b_lut_active )
		if( options.b_swap_red_blue )
			yuyv_to_rgba8_line_one_low<UV_TYPE, true, make_bgra8 >(src32, dst32, nb, options );
		else
			yuyv_to_rgba8_line_one_low<UV_TYPE, true, make_rgba8 >(src32, dst32, nb, options );
	else
		if (options.b_swap_red_blue )
			yuyv_to_rgba8_line_one_low<UV_TYPE, false, make_bgra8 >(src32, dst32, nb, options );
		else
			yuyv_to_rgba8_line_one_low<UV_TYPE, false, make_rgba8 >(src32, dst32, nb, options );

}
void	bitcon::yuyv_to_rgba8_line_one( UINT32 CONST * RESTRICT src32, UINT32* RESTRICT dst32, INT32 nb, st_img_conv CONST & options )
{	dispatch_yuyv_to_rgba8_line_one<UV_YUYV,false>( src32, dst32, nb, options );	}
void	bitcon::uyvy_to_rgba8_line_one( UINT32 CONST * RESTRICT src32, UINT32* RESTRICT dst32, INT32 nb, st_img_conv CONST & options )
{	dispatch_yuyv_to_rgba8_line_one<UV_UYVY,true>( src32, dst32, nb, options );	}


// here we do yuyv/uyuy_to_rgba8_line_block() which process line_nb lines at a time
template< E_UV_TYPE UV_TYPE, bool B_LUT,  auto fn_out >
void yuyv_to_rgba8_line_block_low( INT32 line_begin, INT32 line_nb, UINT32* RESTRICT dst32, INT32 CONST dst32_pitch, st_img_conv CONST & options )
{
	if constexpr (UV_TYPE < UV_YUYV)
	{
		ERR_PRINT_STRING( "%s() problem : UV_TYPE can never be %d here", __FUNCTION__, UV_TYPE );
		//error message here
		return;
	}
	//DBG_PRINT_STRING( "%s() problem %d : line_begin %d --- line_end %d", __FUNCTION__, UV_TYPE, line_begin, line_begin + line_nb );
	INT32 loop_hori = bitcon::shift_len_check_align(__FUNCTION__, options.sx, 1);
	const UINT32* src32 = ((UINT32*)options.src_a) + line_begin * loop_hori;
    dst32 += line_begin * dst32_pitch;

    INT32 line_end = std::min(line_begin + line_nb, options.sy);
#if AAA_DEBUG()
    if( line_begin >= line_end )
        DBG_PRINT_STRING( "%s() problem : line_begin %d --- line_end %d", __FUNCTION__, line_begin, line_end );
#endif
    for( INT32 line_index = line_begin; line_index < line_end; ++line_index )
	{
		yuyv_to_rgba8_line_one_low< UV_TYPE, B_LUT, fn_out >(src32, dst32, loop_hori, options );
		src32 += loop_hori;
		dst32 += dst32_pitch;
	}
}
template<E_UV_TYPE UV_TYPE>
FINLINE void dispatch_yuyv_to_rgba8_line_block( INT32 line_begin, INT32 line_nb, UINT32* RESTRICT dst32, INT32 CONST dst32_pitch, st_img_conv CONST & options )
{
	if( options.b_lut_active )
		if (options.b_swap_red_blue)
			yuyv_to_rgba8_line_block_low<UV_TYPE, true, make_bgra8>(line_begin, line_nb, dst32, dst32_pitch, options);
		else
			yuyv_to_rgba8_line_block_low<UV_TYPE, true, make_rgba8>(line_begin, line_nb, dst32, dst32_pitch, options);
	else
		if (options.b_swap_red_blue)
			yuyv_to_rgba8_line_block_low<UV_TYPE, false, make_bgra8>(line_begin, line_nb, dst32, dst32_pitch, options);
		else
			yuyv_to_rgba8_line_block_low<UV_TYPE, false, make_rgba8>(line_begin, line_nb, dst32, dst32_pitch, options);
}
void	bitcon::yuyv_to_rgba8_line_block(		INT32 line_begin, INT32 line_nb, UINT32* RESTRICT dst32, INT32 CONST dst32_pitch, st_img_conv CONST & options )
{	dispatch_yuyv_to_rgba8_line_block< UV_YUYV >( line_begin, line_nb, dst32, dst32_pitch, options ); }	
void	bitcon::uyvy_to_rgba8_line_block(		INT32 line_begin, INT32 line_nb, UINT32* RESTRICT dst32, INT32 CONST dst32_pitch, st_img_conv CONST & options )
{	dispatch_yuyv_to_rgba8_line_block< UV_UYVY >( line_begin, line_nb, dst32, dst32_pitch, options ); }

template< E_UV_TYPE UV_TYPE, bool B_LUT, auto fn_out >
void yuv_to_rgba8_line_block_low( INT32 line_begin, INT32 line_nb, UINT32* RESTRICT dst32, INT32 CONST dst32_pitch, st_img_conv CONST & options )
{
    const INT32 sx = options.sx;
    INT32 loop_hori;
	const uint8_t* RESTRICT src_y8;
	const uint8_t* RESTRICT src_u8;
	const uint8_t* RESTRICT src_v8;
	INT32 sx_u; 

	loop_hori = bitcon::shift_len_check_align(__FUNCTION__, sx, 4);
	src_y8 = options.src_a + line_begin * sx;
	if constexpr ( UV_TYPE == UV_FUSED )
	{
		sx_u = sx;	//this is for 2 lines of y
		src_v8 = nullptr; // initialized to remove warning
	}
	else
	{
		sx_u = sx >> 1; //this is for 2 lines of y
		src_v8 = options.src_c + line_begin * (sx_u >> 1);
	}
	src_u8 = options.src_b + line_begin * (sx_u >> 1);

    dst32 += line_begin * dst32_pitch;

    INT32 line_end = std::min(line_begin + line_nb, options.sy);
#if AAA_DEBUG()
    if( line_begin >= line_end )
        DBG_PRINT_STRING("%s() problem : line_begin %d --- line_end %d", __FUNCTION__, line_begin, line_end );
#endif

    for( INT32 j = line_begin; j < line_end; j += 2 )
	{
		yuv_to_rgba8_line_two<UV_TYPE, B_LUT, fn_out>( src_y8,src_u8,src_v8,	dst32, dst32_pitch, options );
		
		src_y8 += sx*2;
		src_u8 += sx_u;
		if constexpr ( UV_TYPE == UV_SEPARATE )
			src_v8 += sx_u;
		dst32 += dst32_pitch * 2;
	}
}
template<E_UV_TYPE UV_TYPE>
FINLINE void dispatch_yuv_to_rgba8_line_block( INT32 line_begin, INT32 line_nb, UINT32* RESTRICT dst32, INT32 CONST dst32_pitch, st_img_conv CONST & options )
{
	if( options.b_lut_active )
		if (options.b_swap_red_blue)
			yuv_to_rgba8_line_block_low<UV_TYPE, true, make_bgra8>(line_begin, line_nb, dst32, dst32_pitch, options);
		else
			yuv_to_rgba8_line_block_low<UV_TYPE, true, make_rgba8>(line_begin, line_nb, dst32, dst32_pitch, options);
	else
		if (options.b_swap_red_blue)
			yuv_to_rgba8_line_block_low<UV_TYPE, false, make_bgra8>(line_begin, line_nb, dst32, dst32_pitch, options);
		else
			yuv_to_rgba8_line_block_low<UV_TYPE, false, make_rgba8>(line_begin, line_nb, dst32, dst32_pitch, options);
}
void bitcon::i420_to_rgba8_line_block(INT32 line_begin, INT32 line_nb, UINT32* dst32, INT32 CONST dst32_pitch, st_img_conv CONST& options)
{ dispatch_yuv_to_rgba8_line_block< UV_SEPARATE >(line_begin, line_nb, dst32, dst32_pitch, options); }
void bitcon::nv12_to_rgba8_line_block(INT32 line_begin, INT32 line_nb, UINT32* dst32, INT32 CONST dst32_pitch, st_img_conv CONST& options)
{ dispatch_yuv_to_rgba8_line_block< UV_FUSED    >(line_begin, line_nb, dst32, dst32_pitch, options); }

//used
//hack to check black magic
	void	bitcon::v210_to_bgra8_slow( UINT8 CONST * src8, UINT8* dst8, INT32 nb, UINT8 CONST alpha )
	{
	INT32			y;
	INT32			cb;
	INT32			cr;
	UINT32 CONST *	src = (UINT32 CONST *) src8;


	if( (nb % 6) != 0 )
	{
		ERR_PRINT_STRING( "in %s() the len %d should be a multiple of 6,", __FUNCTION__, nb  ); 
		ERR_PRINT_STRING( "\tskipping last pixels." );
	}
	nb /= 6;
	

	--src;
	for( ; nb > 0; --nb )
	{
		UINT32 tmp;
		//	Color
		tmp = *++src;
		cb = tmp & 0x3f;
		y = (tmp>>10) & 0x3f;
		cr =(tmp>>20) & 0x3f;

		//	PIXEL 1
		*dst8++ = (UINT8) CLAMP( y>>2, 0, 255 );
		*dst8++ = (UINT8) CLAMP( cb>>2, 0, 255 );
		*dst8++ = 0xff;
		*dst8++ = alpha;

		tmp = *++src;
		y = tmp & 0x3f;
		//	PIXEL 2
		*dst8++ = (UINT8) CLAMP( y>>2, 0, 255 );
		*dst8++ = 0xff;
		*dst8++ = (UINT8) CLAMP( cr>>2, 0, 255 );
		*dst8++ = alpha;


		cb = (tmp>>10) & 0x3f;
		y = (tmp>>20) & 0x3f;

		//	PIXEL 3
		*dst8++ = (UINT8) CLAMP( y>>2, 0, 255 );
		*dst8++ = (UINT8) CLAMP( cb>>2, 0, 255 );
		*dst8++ = 0xff;
		*dst8++ = alpha;

		tmp = *++src;
		cr = tmp & 0x3f;
		y = (tmp>>10) & 0x3f;
		cb =(tmp>>20) & 0x3f;

		//	PIXEL 4
		*dst8++ = (UINT8) CLAMP( y>>2, 0, 255 );
		*dst8++ = 0xff;
		*dst8++ = (UINT8) CLAMP( cr>>2, 0, 255 );
		*dst8++ = alpha;

		tmp = *++src;
		y = tmp & 0x3f;
		//	PIXEL 5
		*dst8++ = (UINT8) CLAMP( y>>2, 0, 255 );
		*dst8++ = (UINT8) CLAMP( cb>>2, 0, 255 );
		*dst8++ = 0xff;
		*dst8++ = alpha;


		cr = (tmp>>10) & 0x3f;
		y = (tmp>>20) & 0x3f;

		//	PIXEL 6
		*dst8++ = (UINT8) CLAMP( y>>2, 0, 255 );
		*dst8++ = 0xff;
		*dst8++ = (UINT8) CLAMP( cr>>2, 0, 255 );
		*dst8++ = alpha;
	}
}




//void	bitcon::i420_to_rgb8_luma( UINT8* src8, UINT8* src_a8, UINT8* src_b8, UINT8* dst8, INT32 nb )
//{
//	INT32	y;
//	UINT32	data_y;
//	UINT32	data_a;
//	UINT32	data_b;
//	UINT32	out;
//	UINT32*	src = (UINT32*)src8;
//	UINT32*	dst = (UINT32*)dst8;
//	UINT32*	src_a = (UINT32*)src_a8;
//	UINT32*	src_b = (UINT32*)src_b8;
//
//	nb = bitcon::shift_len_check_align( __FUNCTION__, nb, 2 );
//
//	--src_a;
//	--src_b;
//	--src;
//	for( ; nb > 0; --nb )
//	{
//		data_y = *++src;
//		data_a = *++src_a;
//		data_b = *++src_b;
//
//		//	Pixel 1
//		y = ((data_y>>  0) & 0xff);
//		out =  y <<  0;
//		out |= y <<  8;
//		out |= y << 16;
//		//	Pixel 2
//		y = ((data_y>>  8) & 0xff);
//		out |= y << 24;
//		*dst++ = out;
//		out =  y <<  0;
//		out |= y <<  8;
//		//	Pixel 3
//		y = ((data_y>> 16) & 0xff);
//		out |= y << 16;
//		out |= y << 24;
//		*dst++ = out;
//		out =  y <<  0;
//		//	Pixel 4
//		y = ((data_y>> 24) & 0xff);
//		out |= y <<  8;
//		out |= y << 16;
//		out |= y << 24;
//		*dst++ = out;
//
////		*dst++ = (a&0x0f)<<4;
////		*dst++ = (a&0xf0);
//	}
//}


//used
//void	bitcon::yuyv_to_bgr8_fast( UINT8* src8, UINT8* dst8, INT32 nb )
//{
//	UINT32*	src = (UINT32*)src8;
//	UINT32*	dst = (UINT32*)dst8;
//	UINT32	data;
//	UINT32	out;
//	INT32	y;
//	INT32	r;
//	INT32	g;
//	INT32	b;
//
//	nb = bitcon::shift_len_check_align( __FUNCTION__, nb, 2 );
//
//	--src;
//	for( ; nb > 0; --nb )
//		{
//		data = *++src;
//		//	Color
//		uv_2_rgb_to_add( (data>>24) & 0xff, (data>> 8) & 0xff, r, g, b );
//		//	PIXEL 1
//		y = compute_y( (data>> 0) & 0xff );
//		out =  CLAMP( (y+b)>>8, 0, 255 ) <<  0;
//		out |= CLAMP( (y+g)>>8, 0, 255 ) <<  8;
//		out |= CLAMP( (y+r)>>8, 0, 255 ) << 16;
//		//	PIXEL 2
//		y = compute_y( (data>>16) & 0xff );
//		out |= CLAMP( (y+b)>>8, 0, 255 ) << 24;
//		*dst++ = out;
//		out =  CLAMP( (y+g)>>8, 0, 255 ) <<  0;
//		out |= CLAMP( (y+r)>>8, 0, 255 ) <<  8;
//
//		data = *++src;
//		//	Color
//		uv_2_rgb_to_add( (data>>24) & 0xff, (data>> 8) & 0xff, r, g, b );
//		//	PIXEL 3
//		y = compute_y( (data>> 0) & 0xff );
//		out |= CLAMP( (y+b)>>8, 0, 255 ) << 16;
//		out |= CLAMP( (y+g)>>8, 0, 255 ) << 24;
//		*dst++ = out;
//		out =  CLAMP( (y+r)>>8, 0, 255 ) <<  0;
//		//	PIXEL 4
//		y = compute_y( (data>>16) & 0xff );
//		out |= CLAMP( (y+b)>>8, 0, 255 ) <<  8;
//		out |= CLAMP( (y+g)>>8, 0, 255 ) << 16;
//		out |= CLAMP( (y+r)>>8, 0, 255 ) << 24;
//		*dst++ = out;
//		}
//}
//void	bitcon::i420_to_bgr8_slow(UINT8* src, UINT8* src_a, UINT8* src_b, UINT8* dst, INT32 nb )
//{
//	UINT8	y;
//	UINT8	a;
//	UINT8	b;
//
//	nb = shift_len_check_align( __FUNCTION__, nb, 1 );
//
//	--src_a;
//	--src_b;
//	--src;
//	for( ; nb > 0; --nb )
//	{
//		a = *++src_a;
//		b = *++src_b;
//		y = *++src;
//		*dst++ = CLAMP( (y+b-128)>>0, 0, 255 );
//		*dst++ = y;
//		*dst++ = CLAMP( (y+a-128)>>0, 0, 255 );
//		y = *++src;
//		*dst++ = CLAMP( (y+b-128)>>0, 0, 255 );
//		*dst++ = y;
//		*dst++ = CLAMP( (y+a-128)>>0, 0, 255 );
////		*dst++ = (a&0x0f)<<4;
////		*dst++ = (a&0xf0);
//	}
//}
