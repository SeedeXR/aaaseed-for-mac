
#include "pixel_format.h"
#include "aaa_util.h"

using namespace aaa;

namespace
{
	CONSTEXPR c_pixel_format::RGBA_INDEX_TABLE	index_r		= {  0,	-1, -1,	-1 };
	CONSTEXPR c_pixel_format::RGBA_INDEX_TABLE	index_rg	= {  0,	 1,	-1,	-1 };
	CONSTEXPR c_pixel_format::RGBA_INDEX_TABLE	index_rgb	= {  0,	 1,  2,	-1 };
	CONSTEXPR c_pixel_format::RGBA_INDEX_TABLE	index_rgba	= {  0,	 1,  2,	 3 };
	CONSTEXPR c_pixel_format::RGBA_INDEX_TABLE	index_bgr	= {  2,	 1,  0,	-1 };
	CONSTEXPR c_pixel_format::RGBA_INDEX_TABLE	index_bgra	= {  2,	 1,  0,	 3 };
	CONSTEXPR c_pixel_format::RGBA_INDEX_TABLE	index_none	= { -1,	-1, -1,	-1 };
}

//todo gl_type are unused for now
//todo  PIXEL_TYPE::UINT_8 used as default even when linked format for now
#define PF PIXEL_FORMAT
#define PT PIXEL_TYPE
#define NO false
#define Y true
CONSTEXPR	struct c_pixel_format::st_info	c_pixel_format::info[ (INT32)PF::MAX_NB ] =
{
	//	name					bpp		r,g,b,a			y_inv	default_format					format_red_blue_swapped			gl_type : GL_INVALID_VALUE protect c_img moving to gpu
	//							    ch_nb					luma				gl_format_internal				type			
	{	"Red Int8",				8,	1,	&index_r,		NO,	NO,	PF::R_8,		GL_R8,			PF::UNKNOWN,	PT::UINT_8,		GL_UNSIGNED_BYTE	},
	{	"Red Fp16",				16,	1,	&index_r,		NO,	NO,	PF::R_16FP,		GL_R16F,		PF::UNKNOWN,	PT::FLOAT_16,	GL_HALF_FLOAT		},
	{	"Red Fp32",				32,	1,	&index_r,		NO,	NO,	PF::R_32FP,		GL_R32F,		PF::UNKNOWN,	PT::FLOAT_32,	GL_FLOAT			},
	{	"Red 12bit",			12,	1,	&index_r,		NO,	NO,	PF::UNKNOWN,	GL_NONE,		PF::UNKNOWN,	PT::NO_SUPPORT,	GL_INVALID_VALUE	},
//todo deal with conversion in mono ?,
	{	"Red Int16",			16,	1,	&index_r,		NO,	NO,	PF::R_16,		GL_R16,			PF::UNKNOWN,	PT::UINT_16,	GL_UNSIGNED_SHORT	},
	{	"Red Int16Signed",		16,	1,	&index_r,		NO,	NO,	PF::UNKNOWN,	GL_RG16_SNORM,	PF::UNKNOWN,	PT::UINT_16,	GL_SHORT			},

//	{	"Raw 8 bit",			8,	1,	&index_r,		NO,	NO,	PF::R_8,		GL_R8,			PF::UNKNOWN,	PT::UINT_8,		GL_UNSIGNED_BYTE	},	//	BAYER grid flycap ?
//	{	"Raw 8 bit 4 Channel",	8,	4,	&index_r,		NO,	NO,	PF::UNKNOWN,	GL_NONE,		PF::UNKNOWN,	PT::UINT_8,		GL_UNSIGNED_BYTE	},
//	{	"Raw 12 bit",			12,	1,	&index_r,		NO,	NO,	PF::UNKNOWN,	GL_NONE,		PF::UNKNOWN,	PT::UINT_8,		GL_UNSIGNED_BYTE	},
//todo deal with conversion in mono ?
//	{	"Raw 16 bit",			16,	1,	&index_r,		NO,	NO,	PF::R_16,		GL_R16,			PF::UNKNOWN,	PT::UINT_16,	GL_UNSIGNED_SHORT	},

	{	"RG Int8",				16,	2,	&index_rg,		NO,	NO,	PF::RG_8,		GL_RG8,			PF::UNKNOWN,	PT::UINT_8,		GL_UNSIGNED_BYTE	},
	{	"RG Int16",				32,	2,	&index_rg,		NO,	NO,	PF::RG_16,		GL_RG16,		PF::UNKNOWN,	PT::UINT_16,	GL_UNSIGNED_SHORT	},
	{	"RG 16fp",				32,	2,	&index_rg,		NO,	NO,	PF::RG_16FP,	GL_RG16F,		PF::UNKNOWN,	PT::FLOAT_16,	GL_HALF_FLOAT		},
	{	"RG 32fp",				64,	2,	&index_rg,		NO,	NO,	PF::RG_32FP,	GL_RG32F,		PF::UNKNOWN,	PT::FLOAT_32,	GL_FLOAT			},
																//really ?
	{	"RGB Int8",				24,	3,	&index_rgb,		NO,	NO,	PF::RGBA_8,		GL_RGB8,		PF::BGR_8,		PT::UINT_8,		GL_UNSIGNED_BYTE	},
	{	"RGB Int16",			48,	3,	&index_rgb,		NO,	NO,	PF::RGB_16,		GL_RGB16,		PF::BGR_16,		PT::UINT_16,	GL_UNSIGNED_SHORT	},
	{	"RGB Int16Signed",		48,	3,	&index_none,	NO,	NO,	PF::UNKNOWN,	GL_NONE,		PF::UNKNOWN,	PT::UINT_16,	GL_SHORT			},
	{	"RGB Fp16",				48,	3,	&index_rgb,		NO,	NO,	PF::RGB_16FP,	GL_RGB16F,		PF::BGR_16FP,	PT::FLOAT_16,	GL_HALF_FLOAT		},
	{	"RGB Fp32",				96,	3,	&index_rgb,		NO,	NO,	PF::RGB_32FP,	GL_RGB32F,		PF::BGR_32FP,	PT::FLOAT_32,	GL_FLOAT			},
																//really ?
	{	"BGR Int8",				24,	3,	&index_bgr,		NO,	NO,	PF::RGBA_8,		GL_NONE,		PF::RGB_8,		PT::UINT_8,		GL_UNSIGNED_BYTE	},
	{	"BGR Int16",			48,	3,	&index_bgr,		NO,	NO,	PF::BGR_16,		GL_NONE,		PF::RGB_16,		PT::UINT_16,	GL_UNSIGNED_SHORT	},
	{	"BGR Fp16",				48,	3,	&index_bgr,		NO,	NO,	PF::BGR_16FP,	GL_NONE,		PF::RGB_16FP,	PT::FLOAT_16,	GL_HALF_FLOAT		},
	{	"BGR Fp32",				96,	3,	&index_bgr,		NO,	NO,	PF::BGR_32FP,	GL_NONE,		PF::RGB_32FP,	PT::FLOAT_32,	GL_FLOAT			},

	{	"RGBA Int8",			32,	4,	&index_rgba,	NO,	NO,	PF::RGBA_8,		GL_RGBA8,		PF::BGRA_8,		PT::UINT_8,		GL_UNSIGNED_BYTE	},
	{	"RGBA Int16",			64,	4,	&index_rgba,	NO,	NO,	PF::RGBA_16,	GL_RGBA16,		PF::BGRA_16,	PT::UINT_16,	GL_UNSIGNED_SHORT	},
	{	"RGBA Fp16",			64,	4,	&index_rgba,	NO,	NO,	PF::RGBA_16FP,	GL_RGBA16F,		PF::BGRA_16FP,	PT::FLOAT_16,	GL_HALF_FLOAT		},
	{	"RGBA Fp32",			128,4,	&index_rgba,	NO,	NO,	PF::RGBA_32FP,	GL_RGBA32F,		PF::BGRA_32FP,	PT::FLOAT_32,	GL_FLOAT			},

	{	"BGRA Int8",			32,	4,	&index_bgra,	NO,	NO,	PF::RGBA_8,		GL_NONE,		PF::RGBA_8,		PT::UINT_8,		GL_UNSIGNED_BYTE	},
	{	"BGRA Int16",			64,	4,	&index_bgra,	NO,	NO,	PF::BGRA_16,	GL_NONE,		PF::RGBA_16,	PT::UINT_16,	GL_UNSIGNED_SHORT	},
	{	"BGRA Fp16",			64,	4,	&index_bgra,	NO,	NO,	PF::BGRA_16FP,	GL_NONE,		PF::RGBA_16FP,	PT::FLOAT_16,	GL_HALF_FLOAT		},
	{	"BGRA Fp32",			128,4,	&index_bgra,	NO,	NO,	PF::BGRA_32FP,	GL_NONE,		PF::RGBA_32FP,	PT::FLOAT_32,	GL_FLOAT			},

	{	"RGB 565",				16,	3,	&index_none,	NO,	NO,	PF::UNKNOWN,	GL_NONE,		PF::UNKNOWN,	PT::NO_SUPPORT,	GL_INVALID_VALUE	},
	{	"RGB 555",				16,	3,	&index_none,	NO,	NO,	PF::UNKNOWN,	GL_RGB5,		PF::UNKNOWN,	PT::NO_SUPPORT,	GL_INVALID_VALUE	},

	{	"YVYU",					16,	3,	&index_none,	Y,	NO,	PF::RGBA_8,		GL_NONE,		PF::UNKNOWN,	PT::UINT_8,		GL_INVALID_VALUE	},
	{	"UYVY",					16,	3,	&index_none,	Y,	Y,	PF::RGBA_8,		GL_NONE,		PF::UNKNOWN,	PT::UINT_8,		GL_INVALID_VALUE	},
	{	"YUV 4:1:1",			12,	3,	&index_none,	NO,	NO,	PF::UNKNOWN,	GL_NONE,		PF::UNKNOWN,	PT::UINT_8,		GL_INVALID_VALUE	},
	{	"YUV 4:2:2",			16,	3,	&index_none,	Y,	NO,	PF::RGBA_8,		GL_NONE,		PF::UNKNOWN,	PT::UINT_8,		GL_INVALID_VALUE	},
	{	"YUV 4:2:2 JPEG",		32,	3,	&index_none,	NO,	NO,	PF::UNKNOWN,	GL_NONE,		PF::UNKNOWN,	PT::UINT_8,		GL_INVALID_VALUE	},	// not sure of bit_per_pixel
	{	"YUV 4:4:4",			24,	3,	&index_none,	NO,	NO,	PF::UNKNOWN,	GL_NONE,		PF::UNKNOWN,	PT::UINT_8,		GL_INVALID_VALUE	},
	{	"I420",					12,	3,	&index_none,	Y,	NO,	PF::RGBA_8,		GL_NONE,		PF::UNKNOWN,	PT::UINT_8,		GL_INVALID_VALUE	},
	{	"NV12",					12,	3,	&index_none,	Y,	NO,	PF::RGBA_8,		GL_NONE,		PF::UNKNOWN,	PT::UINT_8,		GL_INVALID_VALUE	},
	{	"HDYC YUV 4:2:2 8 bit",	16,	3,	&index_none,	Y,	Y,	PF::RGBA_8,		GL_NONE,		PF::UNKNOWN,	PT::UINT_8,		GL_INVALID_VALUE	},
	{	"IYUV",					16,	3,	&index_none,	NO,	NO,	PF::UNKNOWN,	GL_NONE,		PF::UNKNOWN,	PT::UINT_8,		GL_INVALID_VALUE	},
	{	"YVU9",					9,	3,	&index_none,	NO,	NO,	PF::UNKNOWN,	GL_NONE,		PF::UNKNOWN,	PT::UINT_8,		GL_INVALID_VALUE	},
	{	"Y41P",					12,	0,	&index_none,	NO,	NO,	PF::UNKNOWN,	GL_NONE,		PF::UNKNOWN,	PT::UINT_8,		GL_INVALID_VALUE	},
	{	"Y211",					8,	0,	&index_none,	Y,	Y,	PF::UNKNOWN,	GL_NONE,		PF::UNKNOWN,	PT::UINT_8,		GL_INVALID_VALUE	},
	{	"v210",					20,	3,	&index_none,	NO,	NO,	PF::RGBA_8,		GL_NONE,		PF::UNKNOWN,	PT::UINT_8,		GL_INVALID_VALUE	},	//	flag to be checked, this is blackmagic stuff

	{	"Depth 16",				16,	1,	&index_r,		NO,	NO,	PF::DEPTH_16,	GL_DEPTH_COMPONENT16,
																								PF::UNKNOWN,	PT::FLOAT_32,	GL_HALF_FLOAT		},	//todo ckeck type here
	{	"Depth 24",				24,	1,	&index_r,		NO,	NO,	PF::DEPTH_24,	GL_DEPTH_COMPONENT24,
																								PF::UNKNOWN,	PT::FLOAT_32,	GL_FLOAT			},
	//opengl have 32 and 32f 32 is optional so we use 32f
	{	"Depth 32",				32,	1,	&index_r,		NO,	NO,	PF::DEPTH_32,	GL_DEPTH_COMPONENT32F,
																								PF::UNKNOWN,	PT::FLOAT_32,	GL_FLOAT			},
	{	"Depth 24 Stencil 8",	32,	1,	&index_none,	NO,	NO,	PF::DEPTH_24_STENCIL_8,
																				GL_DEPTH24_STENCIL8,
																								PF::UNKNOWN,	PT::NO_SUPPORT,	GL_UNSIGNED_INT_24_8 	},
	{	"Depth 32 Stencil 8",	64,	1,	&index_none,	NO,	NO,	PF::DEPTH_32_STENCIL_8,
																				GL_DEPTH32F_STENCIL8,
																								PF::UNKNOWN,	PT::NO_SUPPORT,	GL_FLOAT_32_UNSIGNED_INT_24_8_REV 	},

//todo deal with conversion in mono ?
	{	"Disparity 16 bits",	16,	1,	&index_r,		NO,	NO,	PF::R_16,		GL_R16,			PF::UNKNOWN,	PT::UINT_16,	GL_UNSIGNED_SHORT	},
	{	"Binary",				1,	1,	&index_r,		NO,	NO,	PF::R_8,		GL_NONE,		PF::UNKNOWN,	PT::UINT_8,		GL_INVALID_VALUE	},
	{	"Unknown",				0,	0,	&index_none,	NO,	NO,	PF::UNKNOWN,	GL_NONE,		PF::UNKNOWN,	PT::NO_SUPPORT,	GL_INVALID_VALUE	},
};

// Values are derived from the actual case PIXEL_FORMAT::<src> lines present in
// each copy_src_to_<dst>() switch in img_copy.cpp. BGRA / BGR destinations are
// not modeled separately: they go through the matching rgba* / rgb* path with
// swap, so b_to_bgra<W> / b_to_bgr<W> would always equal b_to_rgba<W> /
// b_to_rgb<W>. Callers wanting BGRA / BGR query the RGB / RGBA counterpart.
CONSTEXPR	struct c_pixel_format::st_info_convert	c_pixel_format::info_convert[ (INT32)PF::MAX_NB ] =
{
	//	-------- R --------		-------- RG -------		-------- RGB ------		-------- RGBA -----
	//	8	16	16fp	32fp	8	16	16fp	32fp	8	16	16fp	32fp	8	16	16fp	32fp
	{	Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y	},	// Red Int8
	{	Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y	},	// Red Fp16
	{	Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y	},	// Red Fp32
	{	NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO	},	// Red 12bit
	{	Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y	},	// Red Int16
	{	NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO	},	// Red Int16Signed

	{	Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y	},	// RG Int8
	{	Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y	},	// RG Int16
	{	Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y	},	// RG 16fp
	{	Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y	},	// RG 32fp

	{	Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y	},	// RGB Int8
	{	Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y	},	// RGB Int16
	{	NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO	},	// RGB Int16Signed
	{	Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y	},	// RGB Fp16
	{	Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y	},	// RGB Fp32

	{	Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y	},	// BGR Int8
	{	Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y	},	// BGR Int16
	{	Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y	},	// BGR Fp16
	{	Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y	},	// BGR Fp32

	{	Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y	},	// RGBA Int8
	{	Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y	},	// RGBA Int16
	{	Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y	},	// RGBA Fp16
	{	Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y	},	// RGBA Fp32

	{	Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y	},	// BGRA Int8
	{	Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y	},	// BGRA Int16
	{	Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y	},	// BGRA Fp16
	{	Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y,		Y,	Y,	Y,		Y	},	// BGRA Fp32

	{	NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO	},	// RGB 565
	{	NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO	},	// RGB 555

	{	Y,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		Y,	NO,	NO,		NO	},	// YVYU
	{	NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		Y,	NO,	NO,		NO	},	// UYVY
	{	NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO	},	// YUV 4:1:1
	{	Y,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		Y,	NO,	NO,		NO	},	// YUV 4:2:2
	{	NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO	},	// YUV 4:2:2 JPEG
	{	NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO	},	// YUV 4:4:4
	{	Y,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		Y,	NO,	NO,		NO	},	// I420
	{	Y,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		Y,	NO,	NO,		NO	},	// NV12
	{	Y,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		Y,	NO,	NO,		NO	},	// HDYC YUV 4:2:2 8 bit
	{	NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO	},	// IYUV
	{	NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO	},	// YVU9
	{	NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO	},	// Y41P
	{	NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO	},	// Y211
	{	NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		Y,	NO,	NO,		NO	},	// v210

	{	Y,	NO,	Y,		Y,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO	},	// Depth 16
	{	NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO	},	// Depth 24
	{	Y,	NO,	Y,		Y,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO	},	// Depth 32
	{	NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO	},	// Depth 24 Stencil 8
	{	NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO	},	// Depth 32 Stencil 8

	{	NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		Y,	NO,	NO,		NO	},	// Disparity 16 bits
	{	Y,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO	},	// Binary
	{	NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO,		NO,	NO,	NO,		NO	},	// Unknown
};
#undef NO
#undef Y

namespace {
	CONSTEXPR PF format_by_channel_uint8		[]	=	{	PF::R_8,		PF::RG_8,		PF::RGB_8,		PF::RGBA_8		};
	CONSTEXPR PF format_by_channel_uint16		[]	=	{	PF::R_16,		PF::RG_16,		PF::RGB_16,		PF::RGBA_16		};
	CONSTEXPR PF format_by_channel_fp16			[]	=	{	PF::R_16FP,		PF::RG_16FP,	PF::RGB_16FP,	PF::RGBA_16FP	};
	CONSTEXPR PF format_by_channel_fp32			[]	=	{	PF::R_32FP,		PF::RG_32FP,	PF::RGB_32FP,	PF::RGBA_32FP	};

	CONSTEXPR PF format_by_channel_uint8_bgr	[]	=	{	PF::R_8,		PF::RG_8,		PF::BGR_8,		PF::BGRA_8		};
	CONSTEXPR PF format_by_channel_uint16_bgr	[]	=	{	PF::R_16,		PF::RG_16,		PF::BGR_16,		PF::BGRA_16		};
	CONSTEXPR PF format_by_channel_fp16_bgr		[]	=	{	PF::R_16FP,		PF::RG_16FP,	PF::BGR_16FP,	PF::BGRA_16FP	};
	CONSTEXPR PF format_by_channel_fp32_bgr		[]	=	{	PF::R_32FP,		PF::RG_32FP,	PF::BGR_32FP,	PF::BGRA_32FP	};
}	//end namespace with no name

#undef PT
#undef PF

PIXEL_FORMAT	c_pixel_format::make_format_from_channel_type( INT32 CONST channel_nb, PIXEL_TYPE CONST type, bool CONST b_bgr )
{
	if( OUTSIDE_MIN_MAX( channel_nb, 1, 4 ) )
	{	//todo check with caller
		debug_break( "%s() channel_nb %d not supported", __FUNCTION__, channel_nb );
		return PIXEL_FORMAT::UNKNOWN;
	}
	PIXEL_FORMAT CONST * format;
	switch( type )
	{
	default:
		//todo really deal with this
		//todo message os wrong
		//	and we need a default mode
		debug_break( "%s() type %d not supported: PIXEL_TYPE::UINT_8 is used", __FUNCTION__, type );
	case PIXEL_TYPE::UINT_8:	format = b_bgr ? format_by_channel_uint8_bgr	:	format_by_channel_uint8		;		break;
	case PIXEL_TYPE::UINT_16:	format = b_bgr ? format_by_channel_uint16_bgr	:	format_by_channel_uint16	;		break;
	case PIXEL_TYPE::FLOAT_16:	format = b_bgr ? format_by_channel_fp16_bgr		:	format_by_channel_fp16		;		break;
	case PIXEL_TYPE::FLOAT_32:	format = b_bgr ? format_by_channel_fp32_bgr		:	format_by_channel_fp32		;		break;
	case PIXEL_TYPE::DEPTH_16:	return PIXEL_FORMAT::DEPTH_16;
	case PIXEL_TYPE::DEPTH_24:	return PIXEL_FORMAT::DEPTH_24;
	case PIXEL_TYPE::DEPTH_32:	return PIXEL_FORMAT::DEPTH_32;
	}
	return format[ channel_nb-1 ];
}

PIXEL_FORMAT	c_pixel_format::make_format_from_channel_gltype( INT32 CONST channel_nb, GOL::INTERNAL_TYPE CONST gl_type, bool CONST b_bgr )
{
	return make_format_from_channel_type( channel_nb, get_type_from_gl_type(gl_type), b_bgr );
}

//this is only used in messages
CONSTEXPR C_PCHAR_C	pixel_type_str[ (INT32)PIXEL_TYPE::MAX_NB] =
{
	"Int8",
	"Int16",
	"Fp16",
	"Fp32",
	"Depth16",
	"Depth24",
	"Depth32",
	"Not supported",
	"Unknowned"
};
C_PCHAR_C	c_pixel_format::get_pixel_type_name(	PIXEL_TYPE CONST type )
{
	return pixel_type_str[static_cast<INT32>(type)];
}

// Persisted in .deproj files as string label, not numeric enum value.
// Existing labels are immutable (renaming = breaks old projects). New labels
// can use any consistent style. Pre-existing labels kept verbatim:
// "Default", "Mono Int8", "Mono Int16", "RGB 24", "RGBA", "BGRA",
// "RGBA Fp32", "RGBA Fp16".
CONSTEXPR C_PCHAR_C	c_pixel_format::force_pixel_format_str[ (INT32)PIXEL_FORMAT_FORCE::MAX_NB ] =
{
	"Default",

	"Mono Int8",
	"RG Int8",
	"RGB 24",
	"RGBA",

	"BGR 24",
	"BGRA",

	"Mono Int16",
	"RG Int16",
	"RGB Int16",
	"RGBA Int16",

	"Mono Fp16",
	"RG Fp16",
	"RGB Fp16",
	"RGBA Fp16",

	"Mono Fp32",
	"RG Fp32",
	"RGB Fp32",
	"RGBA Fp32",
};
static_assert( sizeof(c_pixel_format::force_pixel_format_str) / sizeof(C_PCHAR_C) == (INT32)PIXEL_FORMAT_FORCE::MAX_NB,
	"force_pixel_format_str[] is out of sync with PIXEL_FORMAT_FORCE" );

CONSTEXPR C_PCHAR_C	c_pixel_format::force_format_str[ static_cast<INT32>( PIXEL_FORMAT_SRC_FORCE::MAX_NB ) ] =
{
	"Default",
	"RGB",
	"RGBA",
	"YUY2",
	"I420",
	"Mono 8",
	"UVVY",
};

// Runtime-dispatched "can pf_src be directly converted to pf_dst?".
// Routes pf_dst to the matching is_move_<dst>(pf_src) field of info_convert.
// BGR_8 and BGRA_8 destinations reuse the rgb8 / rgba8 paths (red-blue swap applied at copy time).
bool	c_pixel_format::is_move( PIXEL_FORMAT CONST pf_dst, PIXEL_FORMAT CONST pf_src )
{
	switch( pf_dst )
	{
	// ---- 8 bit ----
	case PIXEL_FORMAT::R_8:			return is_move_r8(			pf_src );
	case PIXEL_FORMAT::RG_8:		return is_move_rg8(			pf_src );
	case PIXEL_FORMAT::RGB_8:		return is_move_rgb8(		pf_src );
	case PIXEL_FORMAT::BGR_8:		return is_move_rgb8(		pf_src );	// rgb  path with swap
	case PIXEL_FORMAT::RGBA_8:		return is_move_rgba8(		pf_src );
	case PIXEL_FORMAT::BGRA_8:		return is_move_rgba8(		pf_src );	// rgba path with swap

	// ---- 16 bit ----
	case PIXEL_FORMAT::R_16:		return is_move_r16(			pf_src );
	case PIXEL_FORMAT::RG_16:		return is_move_rg16(		pf_src );
	case PIXEL_FORMAT::RGB_16:		return is_move_rgb16(		pf_src );
	case PIXEL_FORMAT::RGBA_16:		return is_move_rgba16(		pf_src );

	// ---- 16 fp ----
	case PIXEL_FORMAT::R_16FP:		return is_move_r16fp(		pf_src );
	case PIXEL_FORMAT::RG_16FP:		return is_move_rg16fp(		pf_src );
	case PIXEL_FORMAT::RGB_16FP:	return is_move_rgb16fp(		pf_src );
	case PIXEL_FORMAT::RGBA_16FP:	return is_move_rgba16fp(	pf_src );

	// ---- 32 fp ----
	case PIXEL_FORMAT::R_32FP:		return is_move_r32fp(		pf_src );
	case PIXEL_FORMAT::RG_32FP:		return is_move_rg32fp(		pf_src );
	case PIXEL_FORMAT::RGB_32FP:	return is_move_rgb32fp(		pf_src );
	case PIXEL_FORMAT::RGBA_32FP:	return is_move_rgba32fp(	pf_src );

	default:
		return false;
	}
}


GLenum			c_pixel_format::get_gl_format(PIXEL_FORMAT CONST format, bool CONST b_bgr)
{
	//todo move this in pixel Format
	switch (format)
	{
	case PIXEL_FORMAT::DEPTH_16:
	case PIXEL_FORMAT::DEPTH_24:
	case PIXEL_FORMAT::DEPTH_32:
		return GL_DEPTH_COMPONENT;
	case PIXEL_FORMAT::DEPTH_24_STENCIL_8:	//deal with GL_UNSIGNED_INT_24_8 type
	case PIXEL_FORMAT::DEPTH_32_STENCIL_8:	//deal with GL_FLOAT_32_UNSIGNED_INT_24_8_REV type
		return GL_DEPTH_STENCIL;
	default:
		return GOL::get_gl_format_from_channel_nb(get_channel_nb(format), b_bgr != is_bgr(format));
	}
}
bool	c_pixel_format::make_compo_index(PIXEL_FORMAT CONST pf, COMPO CONST compo, INT32& a, INT32& b)
{
	if (compo <= COMPO::ALPHA)
	{
		a = get_compo_index(pf, compo);
		return a >= 0;
	}
	else
	{
		RGBA_INDEX_TABLE* tab_compo = get_index_table(pf);
		switch (compo)
		{
		case COMPO::RED_MINUS_GREEN:	a = 0;	b = 1;	break;
		case COMPO::GREY:
		case COMPO::RED_MINUS_BLUE:		a = 0;	b = 2;	break;
		case COMPO::GREEN_MINUS_RED:	a = 1;	b = 0;	break;
		case COMPO::GREEN_MINUS_BLUE:	a = 1;	b = 2;	break;
		case COMPO::BLUE_MINUS_RED:		a = 2;	b = 0;	break;
		case COMPO::BLUE_MINUS_GREEN:	a = 2;	b = 1;	break;
		}
		a = (*tab_compo)[a];
		if (a < 0)
			return false;
		b = (*tab_compo)[b];
		return b >= 0;
	}
}


PIXEL_FORMAT	c_pixel_format::add_alpha(PIXEL_FORMAT CONST format)
{
	PIXEL_FORMAT	format_new;
	switch (format)
	{
	case PIXEL_FORMAT::RGB_8:		format_new = PIXEL_FORMAT::RGBA_8;		break;
	case PIXEL_FORMAT::BGR_8:		format_new = PIXEL_FORMAT::BGRA_8;		break;
	case PIXEL_FORMAT::RGB_16FP:	format_new = PIXEL_FORMAT::RGBA_16FP;	break;
	case PIXEL_FORMAT::BGR_16FP:	format_new = PIXEL_FORMAT::BGRA_16FP;	break;
	case PIXEL_FORMAT::RGB_32FP:	format_new = PIXEL_FORMAT::RGBA_32FP;	break;
	case PIXEL_FORMAT::BGR_32FP:	format_new = PIXEL_FORMAT::BGRA_32FP;	break;
	case PIXEL_FORMAT::RGB_16:		format_new = PIXEL_FORMAT::RGBA_16;		break;
	case PIXEL_FORMAT::BGR_16:		format_new = PIXEL_FORMAT::BGRA_16;		break;
	case PIXEL_FORMAT::RGBA_8:
	case PIXEL_FORMAT::BGRA_8:
	case PIXEL_FORMAT::RGBA_16:
	case PIXEL_FORMAT::BGRA_16:
	case PIXEL_FORMAT::RGBA_16FP:
	case PIXEL_FORMAT::BGRA_16FP:
	case PIXEL_FORMAT::RGBA_32FP:
	case PIXEL_FORMAT::BGRA_32FP:
	default:						format_new = format;					break;
	}
	return format_new;
}

//INT32	c_pixel_format::get_channel_nb_from_force( CONST PIXEL_FORMAT_FORCE fin )
//{
//	INT32	channel_nb;
//	switch ( fin )
//	{
//	case PIXEL_FORMAT_FORCE::R_8:
//	case PIXEL_FORMAT_FORCE::R_16:		channel_nb = 1;	break;
//	case PIXEL_FORMAT_FORCE::RGB:		channel_nb = 3;	break;
//	default:							channel_nb = 4;	break;
//	}
//	return channel_nb;
//}

PIXEL_FORMAT	c_pixel_format::get_pixel_format_from_force(PIXEL_FORMAT_FORCE CONST fin)
{
	//PIXELFORMAT	type;
	//switch ( fin )
	//{
	//case PIXEL_FORMAT_FORCE::R_8:		type = PIXEL_FORMAT::R_8;	break;
	//case PIXEL_FORMAT_FORCE::RGB:		type = PIXEL_FORMAT::RGB;	break;
	//case PIXEL_FORMAT_FORCE::RGBA:	type = PIXEL_FORMAT::RGBA;	break;
	//case PIXEL_FORMAT_FORCE::BGRA:	type = PIXEL_FORMAT::BGRA;	break;
	//default:							type = PIXEL_FORMAT::RGBA;	break;
	//}
	//return type;
	static PIXEL_FORMAT CONST Table[] = // Implicitly sized
	{
		PIXEL_FORMAT::RGBA_8,	// DEFAULT (resolved to RGBA_8 by historical convention)

		PIXEL_FORMAT::R_8,
		PIXEL_FORMAT::RG_8,
		PIXEL_FORMAT::RGB_8,
		PIXEL_FORMAT::RGBA_8,

		PIXEL_FORMAT::BGR_8,
		PIXEL_FORMAT::BGRA_8,

		PIXEL_FORMAT::R_16,
		PIXEL_FORMAT::RG_16,
		PIXEL_FORMAT::RGB_16,
		PIXEL_FORMAT::RGBA_16,

		PIXEL_FORMAT::R_16FP,
		PIXEL_FORMAT::RG_16FP,
		PIXEL_FORMAT::RGB_16FP,
		PIXEL_FORMAT::RGBA_16FP,

		PIXEL_FORMAT::R_32FP,
		PIXEL_FORMAT::RG_32FP,
		PIXEL_FORMAT::RGB_32FP,
		PIXEL_FORMAT::RGBA_32FP
	};
	// The compiler will generate an error each time we add a new value to the zero-based enum
	static_assert(sizeof(Table) / sizeof(PIXEL_FORMAT_FORCE) == (INT32)PIXEL_FORMAT_FORCE::MAX_NB, "ERROR: The translation table for 'pixeformat force' needs to be updated.");

	return Table[static_cast<INT32>(fin)];
}

PIXEL_FORMAT	c_pixel_format::get_src_pixel_format_from_force(PIXEL_FORMAT_SRC_FORCE CONST fin)
{
	//PIXEL_FORMAT	pixel_format;
	//switch ( fin )
	//{
	//case PIXEL_FORMAT_SRC_FORCE::DEFAULT:	pixel_format = PIXEL_FORMAT::RGBA;		break;
	//case PIXEL_FORMAT_SRC_FORCE::RGB:		pixel_format = PIXEL_FORMAT::RGB;		break;
	//case PIXEL_FORMAT_SRC_FORCE::RGBA:	pixel_format = PIXEL_FORMAT::RGBA;		break;
	//case PIXEL_FORMAT_SRC_FORCE::YUY2:	pixel_format = PIXEL_FORMAT::YUY2;		break;
	//case PIXEL_FORMAT_SRC_FORCE::I420:	pixel_format = PIXEL_FORMAT::I420;		break;
	//case PIXEL_FORMAT_SRC_FORCE::R_8:		pixel_format = PIXEL_FORMAT::R_8;		break;
	//case PIXEL_FORMAT_SRC_FORCE::UYVY:	pixel_format = PIXEL_FORMAT::UYVY;		break;
	//default:								pixel_format = PIXEL_FORMAT::RGBA;		break;
	//}
	// One weakness in the design : Don't change the order of the enum values
	static PIXEL_FORMAT CONST Table[] =
	{
		PIXEL_FORMAT::RGBA_8,
		PIXEL_FORMAT::RGB_8,
		PIXEL_FORMAT::RGBA_8,
		PIXEL_FORMAT::YUY2,
		PIXEL_FORMAT::I420,
		PIXEL_FORMAT::R_8,
		PIXEL_FORMAT::UYVY,
	};
	// The compiler will generate an error each time we add a new value to the zero-based enum
	static_assert(sizeof(Table) / sizeof(PIXEL_FORMAT_SRC_FORCE) == (INT32)PIXEL_FORMAT_SRC_FORCE::MAX_NB, "ERROR: The translation table for 'force src pixeformat' needs to be updated.");
	return Table[static_cast<INT32>(fin)];
}

//work only for rgb for now (maa 2020)
PIXEL_TYPE	c_pixel_format::get_type_from_gl_type(GOL::INTERNAL_TYPE CONST type)
{
	PIXEL_TYPE pixel_type;
	switch (type)
	{
	default:
		ERR_PRINT_STRING("Unsupported internal type %s in %s() use UINT_8", GOL::internal_type_str[(INT32)type], __FUNCTION__);
	case GOL::INTERNAL_TYPE::UINT_8:	pixel_type = PIXEL_TYPE::UINT_8;	break;
	case GOL::INTERNAL_TYPE::UINT_16:	pixel_type = PIXEL_TYPE::UINT_16;	break;
	case GOL::INTERNAL_TYPE::FLOAT_16:	pixel_type = PIXEL_TYPE::FLOAT_16;	break;
	case GOL::INTERNAL_TYPE::FLOAT_32:	pixel_type = PIXEL_TYPE::FLOAT_32;	break;
	case GOL::INTERNAL_TYPE::DEPTH_16:	pixel_type = PIXEL_TYPE::DEPTH_16;	break;
	case GOL::INTERNAL_TYPE::DEPTH_24:	pixel_type = PIXEL_TYPE::DEPTH_24;	break;
	case GOL::INTERNAL_TYPE::DEPTH_32:	pixel_type = PIXEL_TYPE::DEPTH_32;	break;
	}
	return pixel_type;
}