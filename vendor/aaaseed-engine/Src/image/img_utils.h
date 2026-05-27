
#ifdef AAA_IMG_UTILS_H
#error "IMG_UTILS_H included more than once."
#endif
#define AAA_IMG_UTILS_H 1


#ifndef AAA_AAA_UTIL_H
#	include "aaa_util.h"
#endif
#ifndef	AAA_PIXEL_FORMAT_H
#	include "pixel_format.h"
#endif

#ifdef	WIN32
#	ifndef	AAA_AAA_DEF_H
#		include "aaa_def.h"
#	endif
#	if	AAA_DEMO()
#	else
#		define	AAA_LIB_USE_IMG_GFLSDK()	1
#		define	AAA_LIB_USE_IMG_FREEIMAGE()	1
#	endif
#	ifndef	AAA_IMG_OPENEXR_H
#		include "img_openexr.h"
#	endif
#endif


struct	st_threshold
{
	REAL	threshold	{0};
	REAL	y_factor	{1};
	REAL	y_offset	{0};	
};

struct	st_img_conv
{
	INT32				sx;
	INT32				sy;
	INT32				ox;	//offset
	INT32				oy;

//	bool				b_crop				{	false	};
	bool				b_flip_vert			{	false	};
	bool				b_do_field			{	false	};
	bool				b_swap_red_blue		{	false	};
	bool				b_field_one			{	false	};
	bool				b_force_alpha		{	false	};
	bool				b_threshold			{	false	};
	bool				b_lut_active		{	false	};

	UINT8				alpha				{	0xff	};
	REAL				alpha_fp32			{	1.0f	};

	st_threshold		threshold;

	REAL				luma_min			{	0.		};
	REAL				luma_max			{	1.		};
	REAL				disp_min			{	0.		};
	REAL				disp_max			{	1.		};

	
//	INT32				src_pixel_size_x	{	0		};
//	INT32				src_pixel_size_y	{	0		};
	aaa::PIXEL_FORMAT	src_pixel_format	{	aaa::PIXEL_FORMAT::UNKNOWN	};
	UINT32				src_a_pitch			{	0		};
	UINT8 CONST *		RESTRICT src_a;
	UINT8 CONST *		RESTRICT src_b;
	UINT8 CONST *		RESTRICT src_c;
	UINT8*				RESTRICT lut		{	nullptr	};

	C_PCHAR				signature			{	nullptr };

	st_img_conv( INT32 CONST sx_in, INT32 CONST sy_in )
	{
		sx = sx_in;
		sy = sy_in;
		ox = 0;
		oy = 0;
	}
	st_img_conv( INT32 CONST sx_in, INT32 CONST sy_in, INT32 CONST ox_in, INT32 CONST oy_in )
	{
		sx = sx_in;
		sy = sy_in;
		ox = ox_in;
		oy = oy_in;
	}
	void set_offset(INT32 CONST ox_in, INT32 CONST oy_in )
	{
		ox = ox_in;
		oy = oy_in;
	}
};

class o_str;

class c_img_utils
{
public:
	static CONSTEXPR C_PCHAR_C ext_tga		= "tga";
	static CONSTEXPR C_PCHAR_C ext_png		= "png";
	static CONSTEXPR C_PCHAR_C ext_jpg		= "jpg";
	static CONSTEXPR C_PCHAR_C ext_exr		= "exr";
	static CONSTEXPR C_PCHAR_C ext_aaatc	= "aaatc";
	static CONSTEXPR C_PCHAR_C ext_dds		= "dds";
	static CONSTEXPR C_PCHAR_C ext_yuv		= "yuv";
	static CONSTEXPR C_PCHAR_C ext_sgi		= "sgi";
	static CONSTEXPR C_PCHAR_C ext_tif		= "tif";
	static CONSTEXPR C_PCHAR_C ext_tiff		= "tiff";
	static CONSTEXPR C_PCHAR_C ext_rgb		= "rgb";
	static CONSTEXPR C_PCHAR_C ext_cpp		= "cpp";

	enum	ERR : INT32
	{
		ERR_NULL = ERR_IMG_BASE,
		ERR_DATA_NO,
		ERR_DATA_ALLOC,
		ERR_CANT_READ,
		ERR_CANT_READ_THIS_FORMAT,
		ERR_CANT_WRITE,
		ERR_CANT_CONVERT,
		ERR_TYPE,
		ERR_FILE_TYPE
	};

	enum class FILE_TYPE : INT32
	{
		TGA = 0,	//	RGB ou RGBA
		PNG,
		JPG,
		TIF,
		EXR,
		YUV,
		CPP,
		DDS,
		AAATC,
		SGI,
		DEFAULT,
		MAX_NB
	};

	enum class FILE_TYPE_3D : INT32
	{
		VTK = 0,
		DEFAULT,
		MAX_NB
	};

	enum class LIBRARY : INT32
	{
		GFLSDK = 0,
		FREEIMAGE,
		MAX_NB,
	};

	static	C_PCHAR_C	str_lib_name		[INT32(LIBRARY		::MAX_NB)];
	static	o_str		gflsdk_version;
	static	o_str		gflsdk_libformat;
	static	o_str		freeimage_version;
	static	o_str		openexr_version;
	//static	C_PCHAR_C	str_file_type[(INT32)FILE_TYPE::MAX_NB];
	static	C_PCHAR_C	str_file_type		[INT32(FILE_TYPE	::MAX_NB)];
	static	C_PCHAR_C	str_file_type_3d	[INT32(FILE_TYPE_3D	::MAX_NB)];

	static	LIBRARY		s_lib;
	static	FILE_TYPE	save_format_ui;
	static	REAL		save_compression_ui;
	static	bool		b_save_compo_as_white_with_alpha_ui;
	static	bool		b_save_as_tga_grey_ui;
	static	bool		b_lib_print_format_trig_ui;

	static	FILE_TYPE	get_save_type_from_ext( C_PCHAR_C ext );
	static	void		update();
};

//extern	c_img_utils	img_utils;

class c_img_2d;

extern	AAA_ERR		img_print(			c_img_2d* img );
extern	AAA_ERR		img_read_with_lib(	c_img_2d* img, o_str CONST & filename, bool b_load_data );

extern	void		img_libs_init();
extern	void		img_libs_deinit();
extern	void		img_lib_print_formats();
