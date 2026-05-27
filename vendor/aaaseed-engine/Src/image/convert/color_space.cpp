#include "color_space.h"
#include "err.h"
#include "image/convert/bitmap_convert.h"

namespace aaa
{
	// --- table for optimisation of rgb to grey8 conversion 
	// --- using Y = (54 * R + 183 * G + 19 * B)/256;
	UINT16	color::u16_red2grey_shift8		[LUT_SIZE];
	UINT16	color::u16_green2grey_shift8	[LUT_SIZE];
	UINT16	color::u16_blue2grey_shift8		[LUT_SIZE];
//	UINT32	color::u32_red2grey_shift8		[LUT_SIZE];
//	UINT32	color::u32_green2grey_shift8	[LUT_SIZE];
//	UINT32	color::u32_blue2grey_shift8		[LUT_SIZE];

	//bool	b_tab_grey8_init = false;
	void	color::init_tab_grey8()
	{
		//if( b_tab_grey8_init )
		//	return;
		for( UINT32 i = 0; i < LUT_SIZE; ++i )
		{
			u16_red2grey_shift8[  i] = i * RED_TO_GREY_MUL_255;
			u16_green2grey_shift8[i] = i * GREEN_TO_GREY_MUL_255;
			u16_blue2grey_shift8[ i] = i * BLUE_TO_GREY_MUL_255;
			//u16_red2grey_shift8[  i]	= u32_red2grey_shift8[  i ]	= i * RED_TO_GREY_MUL_255;
			//u16_green2grey_shift8[i]	= u32_green2grey_shift8[i ]	= i * GREEN_TO_GREY_MUL_255;
			//u16_blue2grey_shift8[ i]	= u32_blue2grey_shift8[ i ]	= i * BLUE_TO_GREY_MUL_255;
		}
		//b_tab_grey8_init = true;
	}

// YUV ? RGB conversion parameters
	static CONSTEXPR color::yuv_to_rgb_params yuv_rgb_bt601_studio =
	{	// BT.601 – Studio (16–235 / 16–240)
		1.1643835616, 16.0,	// y_mul, y_sub
		1.5960267857,		// r_cr
		-0.3917622901,		// g_cb
		-0.8129676472,		// g_cr
		2.0172321429		// b_cb
	};
	static CONSTEXPR color::yuv_to_rgb_params yuv_rgb_bt601_full =
	{	// BT.601 – Full (0–255)
		1., 0.,			// y_mul, y_sub
		1.402,			// r_cr
		-0.3441362862,	// g_cb
		-0.7141362862,	// g_cr
		1.772			// b_cb
	};
	static CONSTEXPR color::yuv_to_rgb_params yuv_rgb_bt709_studio =
	{	// BT.709 – Studio (HD, limited range)
		1.1643835616, 16.0,	// y_mul, y_sub
		1.7927410714,		// r_cr
		-0.2132486143,		// g_cb
		-0.5329093286,		// g_cr
		2.1124017857		// b_cb
	};
	static CONSTEXPR color::yuv_to_rgb_params yuv_rgb_bt709_full =
	{	// BT.709 – Full
		1., 0.,			// y_mul, y_sub
		1.5748,			// r_cr
		-0.1873242730,	// g_cb
		-0.4681242730,	// g_cr
		1.8556			// b_cb
	};		
	static CONSTEXPR color::yuv_to_rgb_params yuv_rgb_bt2020_studio =
	{	// BT.2020 – Studio (limited range, 16–235/16–240)
		1.1643835616, 16.0,	// y_mul, y_sub (same scaling factor as BT.601/709)
		1.678674,			// r_cr
		-0.187326,			// g_cb
		-0.650424,			// g_cr
		1.734659			// b_cb
	};	
	static CONSTEXPR color::yuv_to_rgb_params yuv_rgb_bt2020_full =
	{
		1.0, 0.0,	// y_mul, y_sub
		1.4746,		// r_cr
		-0.164553,	// g_cb
		-0.571353,	// g_cr
		1.8814		// b_cb
	};
	static CONSTEXPR color::yuv_to_rgb_params yuv_rgb_models[color::YUV_COLOR_MODEL_NB_MAX] = 
	{	// BT.2020 – Full (0–255)
		yuv_rgb_bt601_studio,
		yuv_rgb_bt601_full,
		yuv_rgb_bt709_studio,
		yuv_rgb_bt709_full,
		yuv_rgb_bt2020_studio,
		yuv_rgb_bt2020_full
	};
	C_PCHAR_C	color::yuv_color_model_str[color::YUV_COLOR_MODEL_NB_MAX] =
	{
		"BT601_STUDIO",		"BT601_FULL",
		"BT709_STUDIO",		"BT709_FULL",
		"BT2020_STUDIO",	"BT2020_FULL"
	};

	color::e_yuv_color_model color::yuv_color_model_used = color::YUV_COLOR_MODEL_NB_MAX;	// so set_yuv_color_model_used() will do the job at first call
	color::yuv_to_rgb_params color::yuv_rgb_param_used = yuv_rgb_models[yuv_color_model_used];
	void color::set_yuv_color_model_used( e_yuv_color_model color_model )
	{
		if( yuv_color_model_used != color_model )
		{
			yuv_color_model_used = color_model;
			yuv_rgb_param_used = yuv_rgb_models[color_model];
			precompute_yuv_to_rgb();
			bitcon::update_convert_yuv_to_rgb();
		}
	}

	INT32	color::tab_Y[LUT_SIZE];
	INT32	color::tab_R_CR[LUT_SIZE];
	INT32	color::tab_G_CB[LUT_SIZE];
	INT32	color::tab_G_CR[LUT_SIZE];
	INT32	color::tab_B_CB[LUT_SIZE];
	


	//FINLINE INT32 fp_to_int32_x8(double v) { return (INT32)(v * 256. * 256. + (v >= 0.0 ? 0.5 : -0.5)); }
	FINLINE INT32 fp_to_lut(double v)
	{
		return (INT32)(v * (1 << color::YUV_LUT_SHIFT) );   // truncate, no rounding
	}

	void	color::precompute_yuv_to_rgb()
	{
		yuv_to_rgb_params const * const params = &color::yuv_rgb_param_used;
		// Assign variables from array
		INT32 const y_mul = fp_to_lut(params->y_mul);
		INT32 const y_sub = INT32(params->y_sub);
		INT32 const r_cr  = fp_to_lut(params->r_cr);
		INT32 const g_cb  = fp_to_lut(params->g_cb);
		INT32 const g_cr  = fp_to_lut(params->g_cr);
		INT32 const b_cb  = fp_to_lut(params->b_cb);

		for( INT32 i = 0; i < LUT_SIZE; ++i )
		{
			// Luma table
			tab_Y[i] = y_mul * (i - y_sub);
			// Chroma contribution tables (centered at 128)
			INT32 uv = i - 128; // U/V centered
			tab_R_CR[i] = uv * r_cr;
			tab_G_CB[i] = uv * g_cb;
			tab_G_CR[i] = uv * g_cr;
			tab_B_CB[i] = uv * b_cb;
		}
	}

	void YUVToRGB(double y, double cb, double cr, int* r_out, int* g_out, int* b_out, color::e_yuv_color_model s_color_model )
	{
		if (s_color_model < 0 || s_color_model > 3)
			s_color_model = color::YUV_BT601_FULL;
    
		color::yuv_to_rgb_params const& params = yuv_rgb_models[s_color_model];
    
		// Assign variables from array
		double y_mul = params.y_mul;
		double y_sub = params.y_sub;
		double r_cr  = params.r_cr; 
		double g_cb  = params.g_cb; 
		double g_cr  = params.g_cr; 
		double b_cb  = params.b_cb;
    
		// Intermediate values
		double Y = y_mul * (y - y_sub);
		double Cb = cb - 128.0;
		double Cr = cr - 128.0;
    
		// Compute deltas only
		double R_add = r_cr * Cr;
		double G_add = g_cb * Cb + g_cr * Cr;
		double B_add = b_cb * Cb;
    
		// Final RGB
		double R = Y + R_add;
		double G = Y + G_add;
		double B = Y + B_add;
    
		// Clamp to 8-bit
		*r_out = (int)(R < 0.0 ? 0 : (R > 255.0 ? 255 : R + 0.5));
		*g_out = (int)(G < 0.0 ? 0 : (G > 255.0 ? 255 : G + 0.5));
		*b_out = (int)(B < 0.0 ? 0 : (B > 255.0 ? 255 : B + 0.5));
	}

	void	color::c_init()
	{
		DBG_PRINT_STRING( "%s() Begin", __FUNCTION__ );
		set_yuv_color_model_used( YUV_BT601_FULL );
		DBG_PRINT_STRING( "%s() before init_tab_grey8", __FUNCTION__ );
		init_tab_grey8();
		DBG_PRINT_STRING( "%s() Done", __FUNCTION__ );
	}	
	void	color::c_deinit()
	{
	}

}	//namespace aaa
