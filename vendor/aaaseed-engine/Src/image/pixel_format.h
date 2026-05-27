

#ifdef AAA_PIXEL_FORMAT_H
#error "PIXEL_FORMAT_H included more than once."
#endif
#define AAA_PIXEL_FORMAT_H 1


#ifndef AAA_ERR_H
#	include "err.h"
#endif
#ifndef AAA_GOL_TEX_H
#	include "gol/gol_tex.h"
#endif



namespace aaa
{
	enum class COMPO : INT32
	{
		RED = 0,	GREEN,		BLUE,		ALPHA,
		GREY,		MAX_COMPO,	MIN_COMPO,
		RED_MINUS_GREEN,	RED_MINUS_BLUE,
		GREEN_MINUS_RED,	GREEN_MINUS_BLUE,
		BLUE_MINUS_RED,		BLUE_MINUS_GREEN,
		MAX_NB
	};
	CONSTEXPR C_PCHAR_C	str_compo[ INT32(COMPO::MAX_NB) ] =
	{
		"RED",		"GREEN",	"BLUE",		"ALPHA",
		"GREY",		"MAX",		"MIN",
		"RED_MINUS_GREEN",	"RED_MINUS_BLUE",
		"GREEN_MINUS_RED",	"GREEN_MINUS_BLUE",
		"BLUE_MINUS_RED",	"BLUE_MINUS_GREEN",
	};	

	enum class PIXEL_TYPE : INT32
	{
		UINT_8 = 0,
		UINT_16,
		FLOAT_16,
		FLOAT_32,
		DEPTH_16,
		DEPTH_24,
		DEPTH_32,
		NO_SUPPORT,
		UNKNOWN,
		MAX_NB
	};

	//enum class PIXEL_TYPE : INT32
	//{
	//	GREY	= 0,
	//	GREY_FP32,
	//	YUV,
	//	RGB,
	//	RGB_FP32,
	//	RGBA,
	//	RGBA_FP32,
	//	UNDEFINED,
	//	MAX_NB
	//};

	//todo use UINT32 and start at 1
	//todo have enforce here or thru another structure a stable number or name over time
	enum class PIXEL_FORMAT : INT32
	{
		R_8 = 0,
		R_16FP,
		R_32FP,
		R_12,
		R_16,
		R_16S,

	//	RAW_8,
	//	RAW_8_CH_4,
	//	RAW_12,
	//	RAW_16,

		RG_8,
		RG_16,
		RG_16FP,
		RG_32FP,

		RGB_8,
		RGB_16,
		RGB_16S,
		RGB_16FP,
		RGB_32FP,

		BGR_8,
		BGR_16,
		BGR_16FP,
		BGR_32FP,

		RGBA_8,
		RGBA_16,
		RGBA_16FP,
		RGBA_32FP,

		BGRA_8,
		BGRA_16,
		BGRA_16FP,
		BGRA_32FP,

		RGB_565,
		RGB_555,

		YVYU,
		UYVY,
		YUV_411,
		YUY2,
		YUV_422_JPEG,
		YUV_444,
		I420,
		NV12,
		HDYC,
		IYUV,
		YVU9,
		Y41P,
		Y211,
		V210,
		
		DEPTH_16,
		DEPTH_24,
		DEPTH_32,
		DEPTH_24_STENCIL_8,
		DEPTH_32_STENCIL_8,

		DISP_16,
		BINARY,
	//NOT_SUPPORTED,
		UNKNOWN,
		MAX_NB
	};


	enum class PIXEL_FORMAT_FORCE : INT32
	{
		DEFAULT = 0,
		R_8,RG_8,RGB_8,RGBA_8,

		BGR_8,BGRA_8,

		R_16,RG_16,RGB_16,RGBA_16,
		R_16FP,RG_16FP,RGB_16FP,RGBA_16FP,
		R_32FP,RG_32FP,RGB_32FP,RGBA_32FP,

		MAX_NB
	};

	enum class PIXEL_FORMAT_SRC_FORCE : INT32
	{
		DEFAULT = 0,
		RGB,
		RGBA,
		YUY2,
		I420,
		R_8,
		UYVY,
		MAX_NB,
	};

	class	c_pixel_format
	{
	public:
		typedef CONST INT8 RGBA_INDEX_TABLE[4];

		struct st_info
		{
			C_PCHAR_C			name;						// Format name
			INT32				bits_per_pixel;				// Nb of bits per pixel
			INT32				channel_nb;					// Format channel nb

			RGBA_INDEX_TABLE*	rgba_index;					//	-1 when none

			bool				b_y_invert;					// Format is Vertically inverted
			bool				b_luma_invert;				// Luma must be inverted

			PIXEL_FORMAT		default_format;				// Default format for conversion
			GLenum				gl_format_internal;
			PIXEL_FORMAT		format_red_blue_swapped;	// corresponding format with red and blue channel swapped or UNKNOWN if none

			PIXEL_TYPE			type;
			GLenum				gl_type;
		};

		// Conversion capability flags, indexed by PIXEL_FORMAT just like st_info.
		// Kept in a separate struct so updates to the conversion catalog don't
		// touch the physical/GL metadata in st_info.
		struct st_info_convert
		{
			// Conversion catalog: 4 destination patterns (r / rg / rgb / rgba)
			// x 4 bit-widths (8 / 16 / 16fp / 32fp).
			// Flag = true means a direct src -> dst conversion exists in the
			// img_copy.cpp / img_convert*.cpp switch network. Indirect paths
			// via a pivot format (typically RGBA_8) are not modeled here.
			//
			// BGRA_* / BGR_* destinations are intentionally NOT in the grid:
			// they are produced by the matching rgba* / rgb* path with
			// options.b_swap_red_blue toggled, so b_to_bgra<W> / b_to_bgr<W>
			// would be identical to b_to_rgba<W> / b_to_rgb<W> for every src.
			// Callers that want a BGRA / BGR dst should query the RGB / RGBA
			// counterpart.
			bool	b_to_r8;
			bool	b_to_r16;
			bool	b_to_r16fp;
			bool	b_to_r32fp;

			bool	b_to_rg8;
			bool	b_to_rg16;
			bool	b_to_rg16fp;
			bool	b_to_rg32fp;

			bool	b_to_rgb8;
			bool	b_to_rgb16;
			bool	b_to_rgb16fp;
			bool	b_to_rgb32fp;

			bool	b_to_rgba8;
			bool	b_to_rgba16;
			bool	b_to_rgba16fp;
			bool	b_to_rgba32fp;
		};

	private:
		static CONST  struct st_info			info[ (INT32)PIXEL_FORMAT::MAX_NB ];
		static CONST  struct st_info_convert	info_convert[ (INT32)PIXEL_FORMAT::MAX_NB ];
		static CONST FINLINE st_info&			get_info( PIXEL_FORMAT CONST format )
												{
													return info[static_cast<INT32>(format)];
												}
		static CONST FINLINE st_info_convert&	get_info_convert( PIXEL_FORMAT CONST format )
												{
													return info_convert[static_cast<INT32>(format)];
												}

	public:
	//hack we should refine the default type
				static	PIXEL_FORMAT	make_format_from_channel_type(		INT32 CONST channel_nb,	PIXEL_TYPE         CONST type = PIXEL_TYPE::UINT_8,	bool CONST b_bgr = false );		
				static	PIXEL_FORMAT	make_format_from_channel_gltype(	INT32 CONST channel_nb, GOL::INTERNAL_TYPE CONST gl_type,			        bool CONST b_bgr = false );
		//		static	INT32			get_channel_nb_from_force(			PIXEL_FORMAT_FORCE     CONST fin );
				static	PIXEL_FORMAT	get_pixel_format_from_force(		PIXEL_FORMAT_FORCE     CONST fin );
				static	PIXEL_FORMAT	get_src_pixel_format_from_force(	PIXEL_FORMAT_SRC_FORCE CONST fin );

				static	PIXEL_TYPE		get_type_from_gl_type(				GOL::INTERNAL_TYPE CONST type );

				static	C_PCHAR_C		force_pixel_format_str[	(INT32)PIXEL_FORMAT_FORCE::MAX_NB		];
				static	C_PCHAR_C		force_format_str[		(INT32)PIXEL_FORMAT_SRC_FORCE::MAX_NB	];


				static	C_PCHAR_C		get_pixel_type_name(			PIXEL_TYPE CONST type		);

		FINLINE static	bool			is_supported(					PIXEL_FORMAT CONST format )		{ return get_info(format).default_format != PIXEL_FORMAT::UNKNOWN;	}
		FINLINE static	C_PCHAR_C		get_name(						PIXEL_FORMAT CONST format )		{ return get_info(format).name;						}
		FINLINE static	INT32			get_bits_per_pixel(				PIXEL_FORMAT CONST format )		{ return get_info(format).bits_per_pixel;			}
		FINLINE static	INT32			get_channel_nb(					PIXEL_FORMAT CONST format )		{ return get_info(format).channel_nb;				}
		FINLINE static	bool			is_y_inverted(					PIXEL_FORMAT CONST format )		{ return get_info(format).b_y_invert;				}
		FINLINE static	bool			is_luma_inverted(				PIXEL_FORMAT CONST format )		{ return get_info(format).b_luma_invert;			}
		FINLINE static	PIXEL_FORMAT	get_default_format(				PIXEL_FORMAT CONST format )		{ return get_info(format).default_format;			}
		FINLINE static	GLenum			get_gl_format_internal(			PIXEL_FORMAT CONST format )		{ return get_info(format).gl_format_internal;		}		
		FINLINE static	PIXEL_FORMAT	get_format_red_blue_swapped(	PIXEL_FORMAT CONST format )		{ return get_info(format).format_red_blue_swapped;	}
		FINLINE static	bool			is_move_r8(						PIXEL_FORMAT CONST format )		{ return get_info_convert(format).b_to_r8;			}
		FINLINE static	bool			is_move_r16(					PIXEL_FORMAT CONST format )		{ return get_info_convert(format).b_to_r16;			}
		FINLINE static	bool			is_move_r16fp(					PIXEL_FORMAT CONST format )		{ return get_info_convert(format).b_to_r16fp;		}
		FINLINE static	bool			is_move_r32fp(					PIXEL_FORMAT CONST format )		{ return get_info_convert(format).b_to_r32fp;		}
		FINLINE static	bool			is_move_rg8(					PIXEL_FORMAT CONST format )		{ return get_info_convert(format).b_to_rg8;			}
		FINLINE static	bool			is_move_rg16(					PIXEL_FORMAT CONST format )		{ return get_info_convert(format).b_to_rg16;		}
		FINLINE static	bool			is_move_rg16fp(					PIXEL_FORMAT CONST format )		{ return get_info_convert(format).b_to_rg16fp;		}
		FINLINE static	bool			is_move_rg32fp(					PIXEL_FORMAT CONST format )		{ return get_info_convert(format).b_to_rg32fp;		}
		FINLINE static	bool			is_move_rgb8(					PIXEL_FORMAT CONST format )		{ return get_info_convert(format).b_to_rgb8;		}
		FINLINE static	bool			is_move_rgb16(					PIXEL_FORMAT CONST format )		{ return get_info_convert(format).b_to_rgb16;		}
		FINLINE static	bool			is_move_rgb16fp(				PIXEL_FORMAT CONST format )		{ return get_info_convert(format).b_to_rgb16fp;		}
		FINLINE static	bool			is_move_rgb32fp(				PIXEL_FORMAT CONST format )		{ return get_info_convert(format).b_to_rgb32fp;		}
		FINLINE static	bool			is_move_rgba8(					PIXEL_FORMAT CONST format )		{ return get_info_convert(format).b_to_rgba8;		}
		FINLINE static	bool			is_move_rgba16(					PIXEL_FORMAT CONST format )		{ return get_info_convert(format).b_to_rgba16;		}
		FINLINE static	bool			is_move_rgba16fp(				PIXEL_FORMAT CONST format )		{ return get_info_convert(format).b_to_rgba16fp;	}
		FINLINE static	bool			is_move_rgba32fp(				PIXEL_FORMAT CONST format )		{ return get_info_convert(format).b_to_rgba32fp;	}
		// Runtime-dispatched flavour: "can pf_src be directly converted to pf_dst?"
		// Equivalent to the is_move_<dst>(pf_src) call selected by pf_dst at runtime.
		// BGR_8 and BGRA_8 destinations reuse the rgb8 / rgba8 paths (red-blue swap applied later).
		// Returns false for any pf_dst not in the supported set (UNKNOWN included).
				static	bool			is_move(						PIXEL_FORMAT CONST pf_dst,	PIXEL_FORMAT CONST pf_src );
		FINLINE static	PIXEL_TYPE		get_pixel_type(					PIXEL_FORMAT CONST format )		{ return get_info(format).type;						}
		FINLINE static	GLenum			get_gl_type(					PIXEL_FORMAT CONST format )		{ return get_info(format).gl_type;					}
		//could be refined later																  
		FINLINE static	bool			has_alpha(						PIXEL_FORMAT CONST format )		{ return get_info(format).channel_nb == 4;			}


//Maa not sure it is a good idea to have it here		}
				static	GLenum			get_gl_format(					PIXEL_FORMAT CONST format, bool CONST b_bgr);

		FINLINE static	INT32			get_byte_per_pixel(				PIXEL_FORMAT CONST format )
										{
											return (get_bits_per_pixel( format ) + 7) >> 3;
										}


		FINLINE static	bool			is_format_i8(					PIXEL_FORMAT CONST format )		{ return get_info(format).type == PIXEL_TYPE::UINT_8;		}
		FINLINE static	bool			is_format_i16(					PIXEL_FORMAT CONST format )		{ return get_info(format).type == PIXEL_TYPE::UINT_16;	}
		FINLINE static	bool			is_format_fp16(					PIXEL_FORMAT CONST format )		{ return get_info(format).type == PIXEL_TYPE::FLOAT_16;	}
		FINLINE static	bool			is_format_fp32(					PIXEL_FORMAT CONST format )		{ return get_info(format).type == PIXEL_TYPE::FLOAT_32;	}
		//FINLINE static	bool		is_format_float(				PIXEL_FORMAT CONST format )		{	auto type = get_info(format).type;					
		//																									return type==PIXEL_TYPE::FLOAT_32 || type==PIXEL_TYPE::FLOAT_16;	}

		FINLINE static	RGBA_INDEX_TABLE* 	get_index_table(			PIXEL_FORMAT CONST format )						{	return get_info(format).rgba_index;						}
		FINLINE static	INT32 				get_compo_index(			PIXEL_FORMAT CONST format, COMPO CONST index )	{	return (*get_index_table(format))[INT32(index)];		}

	//todo process grey also it can exist
		FINLINE static	bool				make_compo_index(			PIXEL_FORMAT CONST pf, COMPO CONST compo, INT32& a	)
											{
												if (compo <= COMPO::ALPHA)
												{
													a = get_compo_index(pf, compo);
													return a >= 0;
												}
												return false;
											}
				static	bool				make_compo_index(			PIXEL_FORMAT CONST pf, COMPO CONST compo, INT32& a, INT32& b );

				static	bool				is_bgr(						PIXEL_FORMAT CONST format )		{	return get_compo_index( format, COMPO::RED ) == 2;	}	

		FINLINE static	bool				is_format_rgb(				PIXEL_FORMAT CONST format )		{	return get_compo_index( format, COMPO::RED )>=0;			}
		FINLINE static	bool				is_format_rgb_i8(			PIXEL_FORMAT CONST format )		{	return is_format_rgb(format) && is_format_i8(	format);	}
		FINLINE static	bool				is_format_rgb_i16(			PIXEL_FORMAT CONST format )		{	return is_format_rgb(format) && is_format_i16(	format);	}
		FINLINE static	bool				is_format_rgb_fp16(			PIXEL_FORMAT CONST format )		{	return is_format_rgb(format) && is_format_fp16(	format);	}
		FINLINE static	bool				is_format_rgb_fp32(			PIXEL_FORMAT CONST format )		{	return is_format_rgb(format) && is_format_fp32(	format);	}
		
				static	PIXEL_FORMAT		add_alpha(					PIXEL_FORMAT CONST format );	
	};

/*
	UINT32			c_pixel_format::get_gl_type( CONST PIXEL_FORMAT format )
	{
		if (		is_float_32(	format ) )	return GL_FLOAT;
		else if (	is_float_16(	format ) )	return GL_HALF_FLOAT;
		else if (	is_short(		format ) )	return GL_UNSIGNED_SHORT;
		return GL_UNSIGNED_BYTE;
	}

	PIXEL_TYPE		c_pixel_format::get_pixel_type( CONST PIXEL_FORMAT format )
	{
		if (		is_float_32(	format ) )	return PIXEL_TYPE::FLOAT_32;
		else if (	is_float_16(	format ) )	return PIXEL_TYPE::FLOAT_16;
		else if (	is_short(		format ) )	return PIXEL_TYPE::UINT_16;
		return PIXEL_TYPE::UINT_8;
	}
*/

}	//namespace aaa

