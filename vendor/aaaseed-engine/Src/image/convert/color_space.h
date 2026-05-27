
#ifdef AAA_COLOR_SPACE_H
#error "COLOR_SPACE_H included more than once."
#endif
#define AAA_COLOR_SPACE_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_AAA_UTIL_H
#	include "aaa_util.h"
#endif
#ifndef AAA_V_BASE_H
#	include "math/v_base.h"
#endif

namespace aaa
{
	//yodo deal with singleton
	class color	
	{
	private:
		static	INT32 CONSTEXPR LUT_SIZE = 256;
		static	UINT16	u16_red2grey_shift8		[LUT_SIZE];
		static	UINT16	u16_green2grey_shift8	[LUT_SIZE];
		static	UINT16	u16_blue2grey_shift8	[LUT_SIZE];

		//static	UINT32	u32_red2grey_shift8		[LUT_SIZE];
		//static	UINT32	u32_green2grey_shift8	[LUT_SIZE];
		//static	UINT32	u32_blue2grey_shift8	[LUT_SIZE];
	public:
		static	FINLINE	UINT32	U16_RED2GREY_SHIFT8(	UINT8 CONST r )	{	return u16_red2grey_shift8	[r];	}
		static	FINLINE	UINT32	U16_GREEN2GREY_SHIFT8(	UINT8 CONST g )	{	return u16_green2grey_shift8[g];	}
		static	FINLINE	UINT32	U16_BLUE2GREY_SHIFT8(	UINT8 CONST b )	{	return u16_blue2grey_shift8	[b];	}

		//static	FINLINE	UINT32	U32_RED2GREY_SHIFT8(	UINT8 r )	{	return u32_red2grey_shift8	[r];	}
		//static	FINLINE	UINT32	U32_GREEN2GREY_SHIFT8(	UINT8 g )	{	return u32_green2grey_shift8[g];	}
		//static	FINLINE	UINT32	U32_BLUE2GREY_SHIFT8(	UINT8 b )	{	return u32_blue2grey_shift8	[b];	}
	private:
		static	INT32 CONSTEXPR 	RED_TO_GREY_MUL_255		= 54;
		static	INT32 CONSTEXPR 	GREEN_TO_GREY_MUL_255	= 183;
		static	INT32 CONSTEXPR 	BLUE_TO_GREY_MUL_255	= 19;

		static	FP32 CONSTEXPR RED_TO_GREY_MUL		= FP32( RED_TO_GREY_MUL_255	  / 255.);
		static	FP32 CONSTEXPR GREEN_TO_GREY_MUL	= FP32( GREEN_TO_GREY_MUL_255 / 255.);
		static	FP32 CONSTEXPR BLUE_TO_GREY_MUL		= FP32( BLUE_TO_GREY_MUL_255  / 255.);

		static	INT32	tab_Y[LUT_SIZE];
		static	INT32	tab_R_CR[LUT_SIZE];
		static	INT32	tab_G_CB[LUT_SIZE];
		static	INT32	tab_G_CR[LUT_SIZE];
		static	INT32	tab_B_CB[LUT_SIZE];

	public:
		static	INT32	CONSTEXPR YUV_LUT_SHIFT = 16;
		struct yuv_to_rgb_params
		{
			double y_mul;  // Y multiplier
			double y_sub;  // Y offset (16 for studio, 0 for full)
			double r_cr;   // R contribution from Cr	r = y_mul*(y-y_sub) + r_cr *(v-128);
			double g_cb;   // G contribution from Cb	g = y_mul*(y-y_sub) + g_cb *(u-128) + g_cr *(v-128);
			double g_cr;   // G contribution from Cr
			double b_cb;   // B contribution from Cb	b = y_mul*(y-y_sub) + b_cb *(u-128);
		};
		enum e_yuv_color_model
		{
			YUV_BT601_STUDIO = 0,
			YUV_BT601_FULL,
			YUV_BT709_STUDIO,
			YUV_BT709_FULL,
			YUV_BT2020_STUDIO,
			YUV_BT2020_FULL,
			YUV_COLOR_MODEL_NB_MAX
		};
	private:
		static e_yuv_color_model yuv_color_model_used;
		static yuv_to_rgb_params yuv_rgb_param_used;

	public:
		static	C_PCHAR_C	yuv_color_model_str[YUV_COLOR_MODEL_NB_MAX];

		static	void						set_yuv_color_model_used( e_yuv_color_model color_model );
		static	e_yuv_color_model			get_yuv_color_model_used()			{ return yuv_color_model_used; }
		static	yuv_to_rgb_params CONST &	get_yuv_color_model_param_used()	{ return yuv_rgb_param_used; }

		static	FINLINE	void	uv_2_rgb_to_add( UINT8 CONST u_in, UINT8 CONST v_in, REAL &r, REAL &g, REAL &b )
		{
			REAL CONST u = REAL(u_in) - REAL(128.);
			REAL CONST v = REAL(v_in) - REAL(128.);

			r = REAL(yuv_rgb_param_used.r_cr)*v;
			g = REAL(yuv_rgb_param_used.g_cb)*u + REAL(yuv_rgb_param_used.g_cr)*v;
			b = REAL(yuv_rgb_param_used.b_cb)*u;
		}
		static	FINLINE	void	uv_2_rgb_to_add( INT32 CONST u, INT32 CONST v, INT32 &r, INT32 &g, INT32 &b )
		{
			r =               tab_R_CR[v];
			g = tab_G_CB[u] + tab_G_CR[v];
			b = tab_B_CB[u];
		}
		static	FINLINE	INT32	compute_y( UINT8 CONST in )
		{
			return tab_Y[in];
		}

		static	FINLINE	INT32	rgb_to_grey( INT32 CONST r, INT32 CONST g, INT32 CONST b )
		{
			INT32 CONST tmp = r*RED_TO_GREY_MUL_255 + g*GREEN_TO_GREY_MUL_255 + b*BLUE_TO_GREY_MUL_255;
			return (tmp > 0xff00) ? 255 : (tmp >> 8);
		}
		static	FINLINE	FP32	rgb_to_grey( FP32 CONST r, FP32 CONST g, FP32 CONST b )
		{
			return r*RED_TO_GREY_MUL + g*GREEN_TO_GREY_MUL + b*BLUE_TO_GREY_MUL;
		}
		static	FINLINE	FP32	rgb_to_grey( FP32 CONST * src )
		{
			return rgb_to_grey( (*src), *(src+1), *(src+2) );
		}

		static	FINLINE	void	rgb_from_hsv( FP32& r, FP32& g, FP32& b, FP32 h, FP32 CONST s, FP32 CONST v )
		{
			if( s == 0 )	//HSV values = 0 � 1
			{
				r = v;
				g = v;
				b = v;
			}
			else
			{
				if( h < 0. || 1. <= h )
					h = FMOD( h );
				h *= 6.;

				INT32 CONST 	hi	=	INT32( h ) ;		//Or ... var_i = floor( var_h )
				FP32 CONST 	v1	=	v * ( 1 - s );
				FP32 CONST 	v2	=	v * ( 1 - s * ( h - hi ) );
				FP32 CONST 	v3	=	v * ( 1 - s * ( 1 - ( h - hi ) ) );

				switch( hi )
				{
				case 0:		r = v;		g = v3;		b = v1;		break;
				case 1:		r = v2;		g = v;		b = v1;		break;
				case 2:		r = v1;		g = v;		b = v3;		break;
				case 3:		r = v1;		g = v2;		b = v;		break;
				case 4:		r = v3;		g = v1;		b = v;		break;
				default:
				case 5:		r = v;		g = v1;		b = v2;		break;
				};
			}
		}

		static	FINLINE	void	rgb_from_hsv( FP32* dst, FP32 h, FP32 CONST s, FP32 CONST v )
		{
			if( s == 0 )			//HSV values = 0 � 1
				set_v3( dst, v );
			else
			{
				if( h < 0. || 1. <= h )
					h = FMOD( h );
				h *= 6.;

				INT32	CONST hi	=	INT32( h ) ;		//Or ... var_i = floor( var_h )
				FP32 CONST v1	=	v * ( 1 - s );
				FP32 CONST v2	=	v * ( 1 - s * ( h - hi ) );
				FP32 CONST v3	=	v * ( 1 - s * ( 1 - ( h - hi ) ) );

				FP32	r, g, b;
				switch( hi )
				{
				case 0:		r = v;		g = v3;		b = v1;		break;
				case 1:		r = v2;		g = v;		b = v1;		break;
				case 2:		r = v1;		g = v;		b = v3;		break;
				case 3:		r = v1;		g = v2;		b = v;		break;
				case 4:		r = v3;		g = v1;		b = v;		break;
				default:
				case 5:		r = v;		g = v1;		b = v2;		break;
				};

				*dst	=	r;
				*++dst	=	g;
				*++dst	=	b;
			}
		}

		static	FINLINE	void	rgb_from_h( FP32* dst, FP32 h )
		{
			if( h < 0. || 1. <= h )
				h = FMOD( h );
			h *= 6.;

			INT32 CONST hi	=	INT32( h ) ;		//Or ... var_i = floor( var_h )
			switch( hi )
			{
			case 0:		*dst = 1;			*++dst = h-hi;		*++dst = 0;			break;
			case 1:		*dst = 1 + hi-h;	*++dst = 1;			*++dst = 0;			break;
			case 2:		*dst = 0;			*++dst = 1;			*++dst = h-hi;		break;
			case 3:		*dst = 0;			*++dst = 1 + hi-h;	*++dst = 1;			break;
			case 4:		*dst = h-hi;		*++dst = 0;			*++dst = 1;			break;
			default:
			case 5:		*dst = 1;			*++dst = 0;			*++dst = 1 + hi-h;	break;
			};
		}

		static	FINLINE	void	rgb_from_h( FP32& r, FP32& g, FP32& b, FP32 h )
		{
			if( h < 0. || 1. <= h )
				h = FMOD( h );
			h *= 6.;

			INT32 CONST hi	=	INT32( h ) ;		//Or ... var_i = floor( var_h )
			switch( hi )
			{
			case 0:		r = 1;			g = h-hi;		b = 0;			break;
			case 1:		r = 1 + hi-h;	g = 1;			b = 0;			break;
			case 2:		r = 0;			g = 1;			b = h-hi;		break;
			case 3:		r = 0;			g = 1 + hi-h;	b = 1;			break;
			case 4:		r = h-hi;		g = 0;			b = 1;			break;
			default:
			case 5:		r = 1;			g = 0;			b = 1 + hi-h;	break;
			};
		}


		static	FINLINE	void	rgb_from_hsv( FP32* vec )						{	rgb_from_hsv( vec, vec[0], vec[1], vec[2] );	}
		//	src and dst can be the same
		static	FINLINE	void	rgb_from_hsv( FP32* dst, FP32 CONST * src )	{	rgb_from_hsv( dst, src[0], src[1], src[2] );	}

		static	FINLINE	void	hsv_from_rgb( FP32* dst, FP32 CONST r, FP32 CONST g, FP32 CONST b )
		{
			FP32 CONST	min =	aaa::MIN<FP32>( r, g, b );	//Min. value of RGB
			FP32 CONST	max =	aaa::MAX<FP32>( r, g, b );	//Max. value of RGB
			FP32			del =	max - min;						//Delta RGB value

			dst[2] = max;

			if( del == 0 )		//This is a gray, no chroma...
				clear_v2( dst );
			else			//Chromatic data...
			{
				FP32	h;
				dst[1] = del / max;
				del = FP32( 1. / (6. * del) );
				if( r == max )
				{
					h = (g - b) * del;
					if( h < 0. )
						h += 1;
				}
				else if( g == max ) 
					h = (b - r) * del + FP32(1./3.);
				else
					h = (r - g) * del + FP32(2./3.);
				dst[0] = h;
			}
		}
		static	FINLINE	void	hsv_from_rgb( FP32* vec )					{	hsv_from_rgb( vec, vec[0], vec[1], vec[2] );	}
		//	src and dst can be the same
		static	FINLINE	void	hsv_from_rgb( FP32* dst, FP32 CONST * src )	{	hsv_from_rgb( dst, src[0], src[1], src[2] );	}

	private:
		static	void	init_tab_grey8();
		static	void	precompute_yuv_to_rgb();
	public:
		static	void	c_init();
		static	void	c_deinit();
	};
};


