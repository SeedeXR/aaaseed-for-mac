
#include "gol/gol_tex.h"
#include "gol/gol_draw.h"
#include "gol/gol_pbo.h"
#include "gol/gol_os.h"
#include "aaa_util.h"
#include "err.h"
#include "gol/gol.h"
#include "spy.h"
#include "draw/aaa_glut.h"

//todo use glBindImageTexture

namespace GOL	{

bool	b_tex_allow_ui				= false;
bool	b_tex_verbose_ui			= false;
bool	b_tex_unit_verbose_ui		= false;
bool	b_tex_unit_dim_use_ui		= false;

bool	b_tex_swizzle_can			= false;

bool	b_multisampling_allow_ui	= false;
bool	b_multisampling_can			= false;
bool	b_multisampling_do			= false;
bool	b_multisampling_state		= false;

bool	b_mono_is_alpha_ui			= true;
GLint	swiz[5][4]	=	{
	{	GL_ONE,	GL_ONE,		GL_ONE,		GL_RED		},	// try at deprecated GL_ALPHA
	{	GL_RED,	GL_RED,		GL_RED,		GL_ONE		},	// try at deprecated GL_LUMINANCE
	{	GL_RED,	GL_GREEN,	GL_ZERO,	GL_ONE		},
	{	GL_RED,	GL_GREEN,	GL_BLUE,	GL_ONE		},
	{	GL_RED,	GL_GREEN,	GL_BLUE,	GL_ALPHA	}
};

void set_swizzle_for_gl_alpha_mode( GLenum CONST target )
{
	GLint	ivec[4]	=  {	GL_ONE,	GL_ONE, GL_ONE,	GL_ALPHA };
	set_swizzle_direct( target, ivec );
}
void set_swizzle_for_gl_red_mode( GLenum CONST target )
{
	GLint	ivec[4]	=  {	GL_ONE,	GL_ONE,	GL_ONE,	GL_RED };
	set_swizzle_direct( target, ivec );
}

bool	b_copy_image_sub_can		=	false;	
bool	b_copy_image_sub_allow_ui	=	true;		//todo param ui
bool	b_copy_image_sub_do			=	false;

INT32	tex_size_max = 0;

void	flip_tex_allow()
{
	b_tex_allow_ui = !b_tex_allow_ui;
	SWITCH_PRINT_STATE( "Texture_Allow", b_tex_allow_ui );
}
void	reset_texture()
{
	//Texture
	for( INT32 i = sampler_nb-1; i >= 0; --i )
	{
		set_tex_unit(i);

		tex_unit_cur->set_dim( 0 );

		if( i < tex_unit_nb )
		{	
			//this is done only for old school texture
			disable_texture_and_force_update_later();
			//Texture_gen
			disable_texture_gen_s_direct();
			disable_texture_gen_t_direct();
			disable_texture_gen_r_direct();
			disable_texture_gen_q_direct();
			//Texture_env_mode
			glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE );	//	GL_MODULATE is the default value of GL_TEXTURE_ENV_MODE
			tex_unit_cur->_env_mode = GL_MODULATE;
		}
	}
}

c_tex_unit::c_tex_unit()
	:_bind_2d(-128)
	,_bind_3d(-128)
	,_dim(0)
	,_b_gen_q(false)
	,_b_gen_r(false)
	,_b_gen_s(false)
	,_b_gen_t(false)
	,_env_mode(0)
{
}

//
//	TEXTURE
//

/*
//found on http://stackoverflow.com/questions/22210684/16-bit-floats-and-gl-half-float
short FloatToFloat16( float value )
{
	short	fltInt16;
	int		fltInt32;
	MEMCPY( &fltInt32, &value, sizeof( float ) );
	fltInt16	=  ((fltInt32 & 0x7fffffff) >> 13) - (0x38000000 >> 13);
	fltInt16	|= ((fltInt32 & 0x80000000) >> 16);

	return fltInt16;
}
float Float16ToFloat( short fltInt16 )
{
	int fltInt32	=  ((fltInt16 & 0x8000) << 16);
	fltInt32		|= ((fltInt16 & 0x7fff) << 13) + 0x38000000;

	float fRet;
	MEMCPY( &fRet, &fltInt32, sizeof( float ) );
	return fRet;
}
*/

//CONST	GLenum			gl_type[				(INT32)INTERNAL_TYPE::MAX_NB]		= {	GL_UNSIGNED_BYTE,	GL_UNSIGNED_SHORT,	GL_HALF_FLOAT,	GL_FLOAT,	GL_NONE,	GL_NONE,	GL_NONE		};
//INTERNAL_TYPE			internal_type_def_cur		= INTERNAL_TYPE::UINT_8;
INTERNAL_TYPE_FORCE		internal_type_def_asked		= INTERNAL_TYPE_FORCE::DEFAULT;

namespace {
	//	use GL_ALPHA ?
	CONSTEXPR GLenum gl_format							[]	= { GL_RED,		GL_RG,		GL_RGB,		GL_RGBA		};
	CONSTEXPR GLenum gl_format_flipped					[]	= { GL_RED,		GL_RG,		GL_BGR,		GL_BGRA		};

	// GL_ALPHA is depreciated in GL4 we should use extension GL_ARB_texture_swizzle
	CONSTEXPR GLenum internal_format_by_channel_uint8	[]	= { GL_R8,		GL_RG8,		GL_RGB8,	GL_RGBA8	};
	CONSTEXPR GLenum internal_format_by_channel_uint16	[]	= { GL_R16,		GL_RG16,	GL_RGB16,	GL_RGBA16	};
	CONSTEXPR GLenum internal_format_by_channel_fp16	[]	= { GL_R16F,	GL_RG16F,	GL_RGB16F,	GL_RGBA16F	};
	CONSTEXPR GLenum internal_format_by_channel_fp32	[]	= { GL_R32F,	GL_RG32F,	GL_RGB32F,	GL_RGBA32F	};
}

GLenum	get_gl_format_from_channel_nb( INT32 CONST channel_nb, bool CONST b_bgr )
{
	if( OUTSIDE_MIN_MAX( channel_nb, 1, 4 ) )
	{
		//todo check with caller
		debug_break( "%s() channel_nb %d not supported", __FUNCTION__, channel_nb );
		return GL_INVALID_VALUE;
	}
	return ( b_bgr ? gl_format_flipped : gl_format ) [channel_nb-1];
}

GLenum	make_internal_format_gl( INT32 CONST channel_nb, INTERNAL_TYPE CONST internal_type )
{
	if( OUTSIDE_MIN_MAX( channel_nb, 1, 4 ) )
	{
		//todo check with caller
		debug_break( "%s() channel_nb %d not supported", __FUNCTION__, channel_nb );
		return GL_INVALID_VALUE;
	}
	switch( internal_type )
	{
	default:
		//todo really deal with this
		//todo message os wrong
		//	and we need a default mode
		debug_break( "%s() type %d not supported: IT_UINT_8 is used", __FUNCTION__, internal_type );
	case INTERNAL_TYPE::UINT_8:				return internal_format_by_channel_uint8	[channel_nb-1];
	case INTERNAL_TYPE::UINT_16:			return internal_format_by_channel_uint16[channel_nb-1];
	case INTERNAL_TYPE::FLOAT_16:			return internal_format_by_channel_fp16	[channel_nb-1];
	case INTERNAL_TYPE::FLOAT_32:			return internal_format_by_channel_fp32	[channel_nb-1];
	case INTERNAL_TYPE::DEPTH_16:			return GL_DEPTH_COMPONENT16;
	case INTERNAL_TYPE::DEPTH_24:			return GL_DEPTH_COMPONENT24;
	case INTERNAL_TYPE::DEPTH_32:			return GL_DEPTH_COMPONENT32F;
	case INTERNAL_TYPE::DEPTH_24_STENCIL_8:	return GL_DEPTH24_STENCIL8;
	case INTERNAL_TYPE::DEPTH_32_STENCIL_8:	return GL_DEPTH32F_STENCIL8;
	}
}

bool	is_same_internal_format_gl(	INT32 CONST channel_nb_new,	INTERNAL_TYPE CONST internal_type_new,
									INT32 CONST channel_nb,		INTERNAL_TYPE CONST internal_type		)
{
	switch( internal_type )
	{
		case INTERNAL_TYPE::DEPTH_16:			
		case INTERNAL_TYPE::DEPTH_24:			
		case INTERNAL_TYPE::DEPTH_32:			
		case INTERNAL_TYPE::DEPTH_24_STENCIL_8:	
		case INTERNAL_TYPE::DEPTH_32_STENCIL_8:
			return internal_type == internal_type_new;
	}
	return channel_nb_new == channel_nb && internal_type_new == internal_type;
}

C_PCHAR get_internal_format_str_from_glenum( GLenum CONST format )
{
	C_PCHAR str = nullptr;
	switch( format )
	{
	case GL_ALPHA :			str = "ALPHA 8 deprecated";				break;

	case GL_R8 :			str = "R8 8";							break;
	case GL_R8I :			str = "R8I i8";							break;
	case GL_R8UI :			str = "R8UI ui8";						break;
	case GL_R8_SNORM :		str = "R8_SNORM s8";					break;
	case GL_R16 :			str = "R16 16";							break;
	case GL_R16I :			str = "R16I i16";						break;
	case GL_R16UI :			str = "R16UI ui16";						break;
	case GL_R16_SNORM :		str = "R16_SNORM s16";					break;
	case GL_R16F :			str = "R16F f16";						break;
	case GL_R32I :			str = "R32I i32";						break;
	case GL_R32UI :			str = "R32UI ui32";						break;
	case GL_R32F :			str = "R32F f32";						break;

	case GL_RG8 :			str = "RG8 8 8";						break;
	case GL_RG8I :			str = "RG8I i8 i8";						break;
	case GL_RG8UI :			str = "RG8UI ui8 ui8";					break;
	case GL_RG8_SNORM :		str = "RG8_SNORM s8 s8";				break;
	case GL_RG16 :			str = "RG16 16 16";						break;
	case GL_RG16I :			str = "RG16I i16 i16";					break;
	case GL_RG16UI :		str = "RG16UI ui16 ui16";				break;
	case GL_RG16_SNORM :	str = "RG8_SNORM s16 s16";				break;
	case GL_RG16F :			str = "RG16F f16 f16";					break;
	case GL_RG32I :			str = "RG32I i32 i32";					break;
	case GL_RG32UI :		str = "RG32UI ui32 ui32";				break;
	case GL_RG32F :			str = "RG32F f32 f32";					break;

	case GL_R3_G3_B2 :		str = "R3_G3_B2 3 3 2";					break;
	case GL_RGB4 :			str = "RGB4 4 4 4";						break;
	case GL_RGB5 :			str = "RGB5 5 5 5";						break;
	case GL_RGB565 :		str = "RGB565 5 6 5";					break;
	case GL_RGB8 :			str = "RGB8 8 8 8";						break;
	case GL_SRGB8 :			str = "SRGB8 8 8 8";					break;
	case GL_RGB8I :			str = "RGB8I i8 i8 i8";					break;
	case GL_RGB8UI :		str = "RGB8UI ui8 ui8 ui8";				break;
	case GL_RGB8_SNORM :	str = "RGB8_SNORM s8 s8 s8";			break;
	case GL_RGB9_E5 :		str = "RGB9_E5 9 9 9 5";				break;
	case GL_RGB10 :			str = "RGB10 10 10 10";					break;
	case GL_R11F_G11F_B10F:	str = "R11F_G11F_B10F f11 f11 f10";		break;
	case GL_RGB12 :			str = "RGB12 12 12 12";					break;
	case GL_RGB16 :			str = "RGB16 16 16 16";					break;
	case GL_RGB16I :		str = "RGB16I i16 i16 i16";				break;
	case GL_RGB16UI :		str = "RGB16UI ui16 ui16 ui16";			break;
	case GL_RGB16_SNORM :	str = "RGB16_SNORM s16 s16 s16";		break;
	case GL_RGB16F :		str = "RGB16F f16 f16 f16";				break;
	case GL_RGB32I :		str = "RGB32I i32 i32 i32";				break;
	case GL_RGB32UI :		str = "RGB32UI ui32 ui32 ui32";			break;
	case GL_RGB32F :		str = "RGB32F f32 f32 f32";				break;

	case GL_RGBA2 :			str = "RGBA2 2 2 2 2";					break;
	case GL_RGBA4 :			str = "RGBA4 4 4 4 4";					break;
	case GL_RGB5_A1 :		str = "RGB5_A1 5 5 5 1";				break;
	case GL_RGBA8 :			str = "RGBA8 8 8 8 8";					break;
	case GL_RGBA8I :		str = "RGBA8I i8 i8 i8 i8";				break;
	case GL_RGBA8UI :		str = "RGBA8UI ui8 ui8 ui8 ui8";		break;
	case GL_SRGB8_ALPHA8 :	str = "SRGB8_ALPHA8 8 8 8 8";			break;
	case GL_RGBA8_SNORM :	str = "RGBA8_SNORM s8 s8 s8 s8";		break;
	case GL_RGB10_A2 :		str = "RGB10_A2 10 10 10 2";			break;
	case GL_RGB10_A2UI :	str = "RGB10_A2UI ui10 ui10 ui10 ui2";	break;
	case GL_RGBA12 :		str = "RGBA12 12 12 12 12";				break;
	case GL_RGBA16 :		str = "RGBA16 16 16 16 16";				break;
	case GL_RGBA16I :		str = "RGBA16I i16 i16 i16 i16";		break;
	case GL_RGBA16UI :		str = "RGBA16UI ui16 ui16 ui16 ui16";	break;
	case GL_RGBA16_SNORM :	str = "RGBA16_SNORM s16 s16 s16 s16";	break;
	case GL_RGBA16F :		str = "RGBA16F f16 f16 f16 f16";		break;
	case GL_RGBA32F :		str = "RGBA32F f32 f32 f32 f32";		break;

	case GL_DEPTH_COMPONENT16	:	str = "Depth 16";				break;
	case GL_DEPTH_COMPONENT24	:	str = "Depth 24";				break;
	case GL_DEPTH_COMPONENT32	:	str = "Depth 32";				break;
	case GL_DEPTH24_STENCIL8	:	str = "Depth 24 Stencil 8";		break;
	case GL_DEPTH_COMPONENT32F	:	str = "Depth f32";				break;
	case GL_DEPTH32F_STENCIL8	:	str = "Depth f32 Stencil 8";	break;
	}
	return str;
}

C_PCHAR get_format_str_from_glenum( GLenum CONST format )
{
	C_PCHAR str = get_internal_format_str_from_glenum( format );
	if( !str )
	{
		switch( format )
		{
		//these should probably be somewhere else : not internal format
		case GL_RGB :				str = "GL_RGB";								break;
		case GL_RGBA :				str = "GL_RGBA";							break;
		case GL_LUMINANCE :			str = "GL_LUMINANCE";						break;
		case GL_LUMINANCE_ALPHA:	str = "GL_LUMINANCE_ALPHA";					break;

		case 1:						str = "No texture defined yet";				break;

		default:					str = "AAASeed don't know this GL format";	break;
		}
	}
	return str;
}

//	removed from glew 2.0 to 2.1
// ------------------- GL_OES_compressed_paletted_texture

#ifndef GL_OES_compressed_paletted_texture
#	define GL_OES_compressed_paletted_texture 1

#	define GL_PALETTE4_RGB8_OES		0x8B90
#	define GL_PALETTE4_RGBA8_OES	0x8B91
#	define GL_PALETTE4_R5_G6_B5_OES	0x8B92
#	define GL_PALETTE4_RGBA4_OES	0x8B93
#	define GL_PALETTE4_RGB5_A1_OES	0x8B94
#	define GL_PALETTE8_RGB8_OES		0x8B95
#	define GL_PALETTE8_RGBA8_OES	0x8B96
#	define GL_PALETTE8_R5_G6_B5_OES	0x8B97
#	define GL_PALETTE8_RGBA4_OES	0x8B98
#	define GL_PALETTE8_RGB5_A1_OES	0x8B99

//#	define GLEW_OES_compressed_paletted_texture GLEW_GET_VAR(__GLEW_OES_compressed_paletted_texture)

#endif // GL_OES_compressed_paletted_texture

C_PCHAR get_compressed_format_str_from_glenum( GLenum CONST format )
{
	C_PCHAR str = nullptr;
	switch( format )
	{
	case GL_COMPRESSED_RGB_S3TC_DXT1_EXT :					str = "RGB_S3TC_DXT1_EXT";					break;
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT :					str = "RGBA_S3TC_DXT1_EXT";					break;
	case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT :					str = "RGBA_S3TC_DXT3_EXT";					break;
	case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT :					str = "RGBA_S3TC_DXT5_EXT";					break;
			
	case GL_PALETTE4_RGB8_OES :								str = "PALETTE4_RGB8_OES";					break;
	case GL_PALETTE4_RGBA8_OES :							str = "PALETTE4_RGBA8_OES";					break;
	case GL_PALETTE4_R5_G6_B5_OES :							str = "PALETTE4_R5_G6_B5_OES";				break;
	case GL_PALETTE4_RGBA4_OES :							str = "PALETTE4_RGBA4_OES";					break;
	case GL_PALETTE4_RGB5_A1_OES :							str = "PALETTE4_RGB5_A1_OES";				break;
	case GL_PALETTE8_RGB8_OES :								str = "PALETTE8_RGB8_OES";					break;
	case GL_PALETTE8_RGBA8_OES :							str = "PALETTE8_RGBA8_OES";					break;
	case GL_PALETTE8_R5_G6_B5_OES :							str = "PALETTE8_R5_G6_B5_OES";				break;
	case GL_PALETTE8_RGBA4_OES :							str = "PALETTE8_RGBA4_OES";					break;
	case GL_PALETTE8_RGB5_A1_OES :							str = "PALETTE8_RGB5_A1_OES";				break;

	case GL_COMPRESSED_R11_EAC :							str = "R11_EAC";							break;
	case GL_COMPRESSED_SIGNED_R11_EAC :						str = "SIGNED_R11_EAC";						break;
	case GL_COMPRESSED_RG11_EAC :							str = "RG11_EAC";							break;
	case GL_COMPRESSED_SIGNED_RG11_EAC :					str = "SIGNED_RG11_EAC";					break;
	case GL_COMPRESSED_RGB8_ETC2 :							str = "RGB8_ETC2";							break;
	case GL_COMPRESSED_SRGB8_ETC2 :							str = "SRGB8_ETC2";							break;
	case GL_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2 :		str = "RGB8_PUNCHTHROUGH_ALPHA1_ETC2";		break;
	case GL_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2 :		str = "SRGB8_PUNCHTHROUGH_ALPHA1_ETC2";		break;
	case GL_COMPRESSED_RGBA8_ETC2_EAC :						str = "RGBA8_ETC2_EAC";						break;
	case GL_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC :				str = "SRGB8_ALPHA8_ETC2_EAC";				break;

	case GL_COMPRESSED_LUMINANCE_LATC1_EXT :				str = "LUMINANCE_LATC1_EXT";				break;
	case GL_COMPRESSED_SIGNED_LUMINANCE_LATC1_EXT :			str = "SIGNED_LUMINANCE_LATC1_EXT";			break;
	case GL_COMPRESSED_LUMINANCE_ALPHA_LATC2_EXT :			str = "LUMINANCE_ALPHA_LATC2_EXT";			break;
	case GL_COMPRESSED_SIGNED_LUMINANCE_ALPHA_LATC2_EXT :	str = "SIGNED_LUMINANCE_ALPHA_LATC2_EXT";	break;	

	case GL_COMPRESSED_RED_RGTC1_EXT :						str = "RED_RGTC1_EXT";						break;
	case GL_COMPRESSED_SIGNED_RED_RGTC1 :					str = "SIGNED_RED_RGTC1";					break;
	case GL_COMPRESSED_RG_RGTC2 :							str = "RG_RGTC2";							break;
	case GL_COMPRESSED_SIGNED_RG_RGTC2 :					str = "SIGNED_RG_RGTC2";					break;

	case GL_COMPRESSED_SRGB_EXT :							str = "SRGB_EXT";							break;
	case GL_COMPRESSED_SRGB_ALPHA_EXT :						str = "SRGB_ALPHA_EXT";						break;
	case GL_COMPRESSED_SLUMINANCE_EXT :						str = "SLUMINANCE_EXT";						break;
	case GL_COMPRESSED_SLUMINANCE_ALPHA_EXT :				str = "SLUMINANCE_ALPHA_EXT";				break;
	case GL_COMPRESSED_SRGB_S3TC_DXT1_EXT :					str = "SRGB_S3TC_DXT1_EXT";					break;
	case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT :			str = "SRGB_ALPHA_S3TC_DXT1_EXT";			break;
	case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT :			str = "SRGB_ALPHA_S3TC_DXT3_EXT";			break;
	case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT :			str = "SRGB_ALPHA_S3TC_DXT5_EXT";			break;

	case GL_COMPRESSED_RGBA_BPTC_UNORM :					str = "RGBA_BPTC_UNORM";					break;
	case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM :				str = "SRGB_ALPHA_BPTC_UNORM";				break;
	case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT :				str = "RGB_BPTC_SIGNED_FLOAT";				break;
	case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT :			str = "RGB_BPTC_UNSIGNED_FLOAT";			break;

	case GL_COMPRESSED_RGB_FXT1_3DFX :						str = "RGB_FXT1_3DFX";						break;
	case GL_COMPRESSED_RGBA_FXT1_3DFX :						str = "RGBA_FXT1_3DFX";						break;

	case GL_COMPRESSED_LUMINANCE_ALPHA_3DC_ATI :			str = "LUMINANCE_ALPHA_3DC_ATI";			break;

	case GL_COMPRESSED_RGBA_ASTC_4x4_KHR :					str = "RGBA_ASTC_4x4_KHR";					break;
	case GL_COMPRESSED_RGBA_ASTC_5x4_KHR :					str = "RGBA_ASTC_5x4_KHR";					break;
	case GL_COMPRESSED_RGBA_ASTC_5x5_KHR :					str = "RGBA_ASTC_5x5_KHR";					break;
	case GL_COMPRESSED_RGBA_ASTC_6x5_KHR :					str = "RGBA_ASTC_6x5_KHR";					break;
	case GL_COMPRESSED_RGBA_ASTC_6x6_KHR :					str = "RGBA_ASTC_6x6_KHR";					break;
	case GL_COMPRESSED_RGBA_ASTC_8x5_KHR :					str = "RGBA_ASTC_8x5_KHR";					break;
	case GL_COMPRESSED_RGBA_ASTC_8x6_KHR :					str = "RGBA_ASTC_8x6_KHR";					break;
	case GL_COMPRESSED_RGBA_ASTC_8x8_KHR :					str = "RGBA_ASTC_8x8_KHR";					break;
	case GL_COMPRESSED_RGBA_ASTC_10x5_KHR :					str = "RGBA_ASTC_10x5_KHR";					break;
	case GL_COMPRESSED_RGBA_ASTC_10x6_KHR :					str = "RGBA_ASTC_10x6_KHR";					break;
	case GL_COMPRESSED_RGBA_ASTC_10x8_KHR :					str = "RGBA_ASTC_10x8_KHR";					break;
	case GL_COMPRESSED_RGBA_ASTC_10x10_KHR :				str = "RGBA_ASTC_10x10_KHR";				break;
	case GL_COMPRESSED_RGBA_ASTC_12x10_KHR :				str = "RGBA_ASTC_12x10_KHR";				break;
	case GL_COMPRESSED_RGBA_ASTC_12x12_KHR :				str = "RGBA_ASTC_12x12_KHR";				break;
	case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR :			str = "SRGB8_ALPHA8_ASTC_4x4_KHR";			break;
	case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR :			str = "SRGB8_ALPHA8_ASTC_5x4_KHR";			break;
	case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR :			str = "SRGB8_ALPHA8_ASTC_5x5_KHR";			break;
	case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR :			str = "SRGB8_ALPHA8_ASTC_6x5_KHR";			break;
	case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR :			str = "SRGB8_ALPHA8_ASTC_6x6_KHR";			break;
	case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR :			str = "SRGB8_ALPHA8_ASTC_8x5_KHR";			break;
	case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR :			str = "SRGB8_ALPHA8_ASTC_8x6_KHR";			break;
	case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR :			str = "SRGB8_ALPHA8_ASTC_8x8_KHR";			break;
	case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR :			str = "SRGB8_ALPHA8_ASTC_10x5_KHR";			break;
	case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR :			str = "SRGB8_ALPHA8_ASTC_10x6_KHR";			break;
	case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR :			str = "SRGB8_ALPHA8_ASTC_10x8_KHR";			break;
	case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR :		str = "SRGB8_ALPHA8_ASTC_10x10_KHR";		break;
	case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR :		str = "SRGB8_ALPHA8_ASTC_12x10_KHR";		break;
	case GL_COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR :		str = "SRGB8_ALPHA8_ASTC_12x12_KHR";		break;

	default:
		str = "unknown format by AAASeed";
		ERR_PRINT_STRING( "AAASeed don't know this GL compressed format : 0x%x", format );
		break;
	}
	return str;
}

INT32 get_channel_nb_from_glenum( GLenum CONST format )
{
	INT32 ch_nb;
	switch( format )
	{
	case GL_DEPTH_COMPONENT16	:
	case GL_DEPTH_COMPONENT24	:
	case GL_DEPTH_COMPONENT32	:
	case GL_DEPTH_COMPONENT32F	:
	case GL_DEPTH_COMPONENT		:
	case GL_ALPHA:	case GL_LUMINANCE:
	case GL_COMPRESSED_RED_RGTC1:
	case GL_R8:		case GL_R16:		case GL_R16F:		case GL_R32F:		ch_nb = 1;	break;
	case GL_DEPTH24_STENCIL8	:
	case GL_DEPTH32F_STENCIL8	:
	case GL_COMPRESSED_RG_RGTC2	:
	case GL_RG8:	case GL_RG16:		case GL_RG16F:		case GL_RG32F:		ch_nb = 2;	break;
	case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
	case GL_RGB8:	case GL_RGB16:		case GL_RGB16F:		case GL_RGB32F:		ch_nb = 3;	break;
	default:
		debug_break( "%s() internal format not supported 0x%08x", __FUNCTION__, format );
	case GL_COMPRESSED_RGBA_ASTC_4x4_KHR :
	case GL_COMPRESSED_RGBA_ASTC_5x4_KHR :
	case GL_COMPRESSED_RGBA_ASTC_5x5_KHR :
	case GL_COMPRESSED_RGBA_ASTC_6x5_KHR :
	case GL_COMPRESSED_RGBA_ASTC_6x6_KHR :
	case GL_COMPRESSED_RGBA_ASTC_8x5_KHR :
	case GL_COMPRESSED_RGBA_ASTC_8x6_KHR :
	case GL_COMPRESSED_RGBA_ASTC_8x8_KHR :
	case GL_COMPRESSED_RGBA_ASTC_10x5_KHR :
	case GL_COMPRESSED_RGBA_ASTC_10x6_KHR :
	case GL_COMPRESSED_RGBA_ASTC_10x8_KHR :
	case GL_COMPRESSED_RGBA_ASTC_10x10_KHR :
	case GL_COMPRESSED_RGBA_ASTC_12x10_KHR :
	case GL_COMPRESSED_RGBA_ASTC_12x12_KHR :
	case GL_COMPRESSED_RGBA_BPTC_UNORM:
	case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
	case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
	case GL_COMPRESSED_RGBA8_ETC2_EAC:
	case GL_RGBA8:	case GL_RGBA16:		case GL_RGBA16F:	case GL_RGBA32F:	ch_nb = 4;	break;
	}
	return ch_nb;
}

void	print_texture_implementation()
{
//	GOOD_PRINT_STRING( "Texture Mapping : 1D %s, 2D %s", get_boolean_str( GL_TEXTURE_1D ), get_boolean_str( GL_TEXTURE_2D ) );
	GOOD_PRINT_STRING( "Texture Max Size : %d",					tex_size_max );
	GOOD_PRINT_STRING( "preferred pixel read format : %s",		get_format_str_from_glenum( get_integer( GL_IMPLEMENTATION_COLOR_READ_FORMAT ) )	);
	GOOD_PRINT_STRING( "preferred pixel read type : %s",		get_str_from_glenum( get_integer( GL_IMPLEMENTATION_COLOR_READ_TYPE ) )				);
	GOOD_PRINT_STRING( "Texture Coords : %d",					get_integer( GL_MAX_TEXTURE_COORDS )			);
	GOOD_PRINT_STRING( "Texture 3D Max Size : %d",				get_integer( GL_MAX_3D_TEXTURE_SIZE )			);
	GOOD_PRINT_STRING( "Texture Cube Map Max Size : %d",		get_integer( GL_MAX_CUBE_MAP_TEXTURE_SIZE )		);
	if( test_version_or_extensions( 3,0, "GL_EXT_texture_array", "GL_NV_texture_array" ) )
		GOOD_PRINT_STRING( "Array Texture Layers Max : %d",		get_integer( GL_MAX_ARRAY_TEXTURE_LAYERS ) );
	if( test_version_or_extensions( 3,1, "GL_ARB_texture_buffer_object", "GL_EXT_texture_buffer_object" ) )
		GOOD_PRINT_STRING( "Texture Buffer Max Texels : %d",	get_integer( GL_MAX_TEXTURE_BUFFER_SIZE ) );
	
	if( test_version_or_extensions( 0,0, "GL_KHR_texture_compression_astc_ldr", "GL_KHR_texture_compression_astc_hdr" ) )
		GOOD_PRINT_STRING( "GL_KHR_texture_compression_astc_ldr or GL_KHR_texture_compression_astc_hdr Found" );

	if( have_extension( "GL_ARB_ES3_compatibility" ) )
	{
		// means ETC2 compressed texture should be supported!!
		GOOD_PRINT_STRING( "GL_ARB_ES3_compatibility Found" );
		//  Compressed Internal Format                 Base Internal Format Type
		//	------------------------------------------ -------------------- ----------
		//	COMPRESSED_RGB8_ETC2                       RGB                  Specific
		//	COMPRESSED_SRGB8_ETC2                      RGBA                 Specific
		//	COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2   RGB                  Specific
		//	COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2  RGB                  Specific
		//	COMPRESSED_RGBA8_ETC2_EAC                  RGBA                 Specific
		//	COMPRESSED_SRGB8_ALPHA8_ETC2_EAC           RGBA                 Specific
		//	COMPRESSED_R11_EAC                         RED                  Specific
		//	COMPRESSED_SIGNED_R11_EAC                  RED                  Specific
		//	COMPRESSED_RG11_EAC                        RG                   Specific
		//	COMPRESSED_SIGNED_RG11_EAC                 RG                   Specific
	}
	if( test_version_or_extensions( 1,3, "GL_ARB_texture_compression" ) )
	{
		INT32	num_compressed_format = get_integer( GL_NUM_COMPRESSED_TEXTURE_FORMATS );
		GOOD_PRINT_STRING( "Texture Compression Nb : %d", num_compressed_format );
		// give us 0 an ATI card!!!
		if( num_compressed_format > 0 )
		{
			GLint*	compressed_format = (GLint*)MALLOC( num_compressed_format * sizeof(GLint) );
			get_integer( GL_COMPRESSED_TEXTURE_FORMATS, compressed_format );
			for( INT32 i = 0; i < num_compressed_format; ++i )
			{
				C_PCHAR str = get_compressed_format_str_from_glenum( compressed_format[ i ] );
				GOOD_PRINT_STRING( "\tTexture Compression %d : %s", i, str );
			}
			FREE( compressed_format );
		}
	}
}
//BIND
FINLINE	void bind_texture(	GLenum CONST target,	UINT32 CONST gl_name )
{
	glBindTexture( target, gl_name );
}
void bind_texture_1d( UINT32 CONST gl_name )
{
	bind_texture( GL_TEXTURE_1D, gl_name );
}
void bind_texture_2d( UINT32 CONST gl_name )
{
	get_tex_unit_cur()->set_bind_2d( gl_name );
	bind_texture( GL_TEXTURE_2D, gl_name );
}
UINT32	get_texture_2d()
{
	return get_tex_unit_cur()->get_bind_2d();
}
void bind_texture_3d( UINT32 CONST gl_name )
{
	get_tex_unit_cur()->set_bind_3d( gl_name );
	bind_texture( GL_TEXTURE_3D, gl_name );
}
UINT32	get_texture_3d()
{
	return get_tex_unit_cur()->get_bind_3d();
}

void bind_texture_2d_and_init( UINT32 CONST gl_name )
{
	bind_texture_2d( gl_name );

	set_tex_2d_wrap_s( GL_CLAMP );
	set_tex_2d_wrap_t( GL_CLAMP );
	set_tex_2d_magnification( GL_LINEAR );
	set_tex_2d_minification( GL_LINEAR);
//		set_tex_2d_minification( GL_LINEAR_MIPMAP_LINEAR );
}
//MIPMAP
bool	b_mipmap_generate_allow_ui			= false;
bool	b_mipmap_generate_do				= false;

//ANISOTROPIC
bool	b_anisotropic_filtering_can			= false;
bool	b_anisotropic_filtering_allow_ui	= false;
bool	b_anisotropic_filtering_do			= false;
REAL	anisotropy_max = 1.;	
REAL	anisotropy_value = 1.;	// 1. is isotropic filtering

void	init_anisotropic_filtering()
{
	b_anisotropic_filtering_can = test_version_or_extensions( 4,6, "GL_ARB_texture_filter_anisotropic", "GL_EXT_texture_filter_anisotropic" );
	if( b_anisotropic_filtering_can )
	{
		//get the value
		GOL::get_real( GL_MAX_TEXTURE_MAX_ANISOTROPY, &anisotropy_max );
	}
}

// Texture NPOT ( Non Power Of Two )
bool	b_tex_npot_can = false;
bool	b_tex_npot_allow_ui = false;
void	init_tex_npot()
{
	b_tex_npot_can = test_version_or_extensions( 2,0, "GL_ARB_texture_non_power_of_two" ) || have_extension( "OES_texture_npot" );
	if( !b_tex_npot_can )
		ERR_PRINT_STRING( "Texture size should be power of 2" );
}

// Texture Rectangle
bool	b_tex_rectangle_can = false;
void	init_tex_rectangle()
{
	b_tex_rectangle_can = test_version_or_extensions( 3,1, "GL_ARB_texture_rectangle", "GL_EXT_texture_rectangle", "GL_NV_texture_rectangle" );
	if( !b_tex_rectangle_can )
		ERR_PRINT_STRING( "Texture should be squared" );
}

// Texture Float
bool	b_tex_float_can = false;
void	init_tex_float()
{
	b_tex_float_can = test_version_or_extensions( 3,0, "GL_ARB_texture_float", "GL_ATI_texture_float", "GL_EXT_texture_storage" );	// also GL_APPLE_float_pixels
}

// Texture Compression
bool	b_tex_compressed_allow_ui	= false;
bool	b_tex_compressed_can		= false;
void	init_tex_compressed()
{
	b_tex_compressed_can = test_version_or_extensions( 1,3, "GL_ARB_texture_compression" );
}

//	Texture Init
INT32	tex_unit_nb_context = 1;
INT32	tex_unit_nb = 1;
INT32	sampler_nb_context = 1;
INT32	sampler_nb = 1;

c_tex_unit	tex_units[TEX_SAMPLER_NB_MAX];
c_tex_unit*	tex_unit_cur = &tex_units[0];

//AAADepreciated : we don't use it anymore
//	bool	b_imaging_can = false;
//	void	init_imaging()
//	{
////		b_imaging_can =
//		have_extension( "GL_ARB_imaging" );
//	}

//AGP
//AGPMEM
//
//	bool	b_agpmem_can = false;
//void ( APIENTRY *glPixelDataRangeNV )( GLenum target, GLsizei length, void *pointer );
//void ( APIENTRY *glFlushPixelDataRangeNV )( GLenum target );
//PFNWGLALLOCATEMEMORYNVPROC	wglAllocateMemoryNV;
//PFNWGLFREEMEMORYNVPROC		wglFreeMemoryNV;

//bool	init_agpmem_fns()
//{
//	WGL_GET_PROC( wglAllocateMemoryNV		);
//	WGL_GET_PROC( wglFreeMemoryNV			);
//	WGL_GET_PROC( glPixelDataRangeNV		);
//	WGL_GET_PROC( glFlushPixelDataRangeNV	);
//	return ( wglAllocateMemoryNV		)
//		&& ( wglFreeMemoryNV			)
//		&& ( glPixelDataRangeNV			)
//		&& ( glFlushPixelDataRangeNV	);
//
////	return ( !wglAllocateMemoryNV );
////	return true;
//}
//void	init_agpmem()
//{
//	if( have_extension( "GL_NV_pixel_data_range" ) )
//	{
//		b_agpmem_can = init_agpmem_fns();
//	}
//	else
//	{
//		b_agpmem_can = false;
//	}
//}

void	init_texture()
{
//old school
//infact this the number we use in opengl pre 3.3 and the nb where we store data in AAASeed and pass it to shaders
//todo we should remove this
	INT32 nb = test_version_or_extensions( 1,3, "GL_ARB_multitexture" ) ? get_integer( GL_MAX_TEXTURE_UNITS ) : 1;
	GOL::tex_unit_nb_context = nb;
	GOOD_PRINT_STRING_1T( "%d Texture Units (old fixed function)",	nb );
	GOOD_PRINT_STRING_1T( "AAASeed support %d texture units in this version.", TEX_UNIT_NB_MAX );
	if( nb != TEX_UNIT_NB_MAX )
	{
		if( nb > TEX_UNIT_NB_MAX )
		{
			GOOD_PRINT_STRING_2T( "\t\tso even if %d are available here only %d will be used.", nb, TEX_UNIT_NB_MAX );
			nb = TEX_UNIT_NB_MAX;
		}
		else
		{
			ERR_PRINT_STRING( "\t\tonly %d are available here, the %d missing should be ignored by AAASeed.", nb, TEX_UNIT_NB_MAX - nb );
			ERR_PRINT_STRING( "\t\tNot enough testing was done on this.");
		}
	}
	GOL::tex_unit_nb = nb;
//		GOL::tex_unit_nb = 2;	//glcup2021 only 2 seems to be ok 1 is wrong

//modern version
	INT32 sampler_nb		 = TEX_SAMPLER_NB_MAX;
	INT32 sampler_nb_context = aaa::BIGGEST<INT32>;
		
	INT32 tmp;

	if( test_version_or_extensions( 2,0, "GL_ARB_vertex_shader" ) )
	{
		tmp = get_integer( GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS );
		if( tmp > 0 )
		{
			sampler_nb_context = MIN( sampler_nb_context, tmp );
			sampler_nb         = MIN( sampler_nb, tmp );
		}
		GOOD_PRINT_STRING_1T( "%d Texture Image Units (vertex shader)",						tmp	);
	}
	if( test_version_or_extensions( 2,0, "GL_ARB_tessellation_shader" ) )
	{
		tmp = get_integer( GL_MAX_TESS_CONTROL_TEXTURE_IMAGE_UNITS );
		if( tmp > 0 )
		{
			sampler_nb_context = MIN( sampler_nb_context, tmp );
			sampler_nb         = MIN( sampler_nb, tmp );
		}
		GOOD_PRINT_STRING_1T( "%d Texture Image Units (tessellation control shader)",		tmp	);
		tmp = get_integer( GL_MAX_TESS_EVALUATION_TEXTURE_IMAGE_UNITS );
		if( tmp > 0 )
		{
			sampler_nb_context = MIN( sampler_nb_context, tmp );
			sampler_nb         = MIN( sampler_nb, tmp );
		}
		GOOD_PRINT_STRING_1T( "%d Texture Image Units (tessellation evaluation shader)",	tmp	);
	}
	if( test_version_or_extensions( 3,2, "GL_ARB_geometry_shader4", "GL_EXT_geometry_shader4", "GL_EXT_geometry_shader" ) )	//also GL_EXT_geometry_point_size
	{
		tmp = get_integer( GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS );
		if( tmp > 0 )
		{
			sampler_nb_context = MIN( sampler_nb_context, tmp );
			sampler_nb         = MIN( sampler_nb, tmp );
		};
		GOOD_PRINT_STRING_1T( "%d Texture Image Units (geometry shader)",					tmp	);
	}
	if( test_version_or_extensions( 4,3, "GL_ARB_compute_shader" ) )
	{
		tmp = get_integer( GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS );
		if( tmp > 0 )
		{
			sampler_nb_context = MIN( sampler_nb_context, tmp );
			sampler_nb         = MIN( sampler_nb, tmp );
		}
		GOOD_PRINT_STRING_1T( "%d Texture Image Units (compute shader)",					tmp	);
	}
	if( test_version_or_extensions( 2,0, "GL_ARB_fragment_program" ) )
	{
		tmp = get_integer( GL_MAX_TEXTURE_IMAGE_UNITS );
		if( tmp > 0 )
		{
			sampler_nb_context = MIN( sampler_nb_context, tmp );
			sampler_nb         = MIN( sampler_nb, tmp );
		}
		GOOD_PRINT_STRING_1T( "%d Texture Image Units (frag shader)",						tmp	);
	}

	if( test_version_or_extensions( 2,0, "GL_ARB_vertex_shader" ) )
		GOOD_PRINT_STRING_1T( "%d Texture Image Units Combined (all shaders)",	get_integer( GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS ) );


	GOOD_PRINT_STRING_1T( "AAASeed support %d samplers in this version.", TEX_SAMPLER_NB_MAX );
	if( sampler_nb != TEX_SAMPLER_NB_MAX )
	{
		if( sampler_nb > TEX_SAMPLER_NB_MAX )
		{
			GOOD_PRINT_STRING_2T( "\t\tso even if %d are available here only %d will be used.", sampler_nb, TEX_SAMPLER_NB_MAX );
			sampler_nb = TEX_SAMPLER_NB_MAX;
		}
		else
		{
			ERR_PRINT_STRING( "\t\tonly %d are available here, the %d missing should be ignored by AAASeed.", sampler_nb, TEX_SAMPLER_NB_MAX-sampler_nb );
			ERR_PRINT_STRING( "\t\tNot enough testing was done on this.");
		}
	}
	GOL::sampler_nb_context = sampler_nb_context;
	GOL::sampler_nb         = sampler_nb;

	//Texture
	for( INT32 i = 0; i < TEX_SAMPLER_NB_MAX; ++i )
		tex_units[i].set_index( i );

	tex_size_max = get_integer( GL_MAX_TEXTURE_SIZE );
	// texture-environment-combining supported?
	test_version_or_extensions( 1,3, "GL_ARB_texture_env_combine", "GL_EXT_texture_env_combine" );

	b_blend_equation_advanced_can			= test_version_or_extensions( 4,5, "GL_KHR_blend_equation_advanced", "GL_NV_blend_equation_advanced" );

	// related to GL_KHR_blend_equation_advanced but waranties on the new mode coherency without explicit barrier if I understand right
	b_blend_equation_advanced_coherent_can	= test_version_or_extensions( 0,0, "GL_KHR_blend_equation_advanced_coherent", "GL_NV_blend_equation_advanced_coherent" );

	b_tex_swizzle_can = test_version_or_extensions( 0,0, "GL_ARB_texture_swizzle", "GL_EXT_texture_swizzle" );

#if AAA_GLUT_USE()
	b_multisampling_can = maaglut_multisample_is_on();
#endif //AAA_GLUT_USE()

	init_anisotropic_filtering();
	//	init_agpmem();
	pbo_init();

//		init_imaging();
	init_tex_float();
	init_tex_npot();
	init_tex_rectangle();
	init_tex_compressed();
	//init_internal_type_def();
}

void	deinit_texture()
{
	pbo_deinit();
}

void c_tex_unit::print()
{
	CHAR CONST * str;
	switch( _dim )
	{
	case 0:		str = "0d";	break;
	case 1:		str = "1d";	break; 
	case 2:		str = "2d";	break;
	case 3:		str = "3d";	break;
	case -1:	str = "-1";	break;
	case -2:	str = "-2";	break;
	default:	str = "??";	break;
	}
	//todo refine : 2d/3d ....
	DBG_PRINT_STRING( "Tu %2d : %5d %s", _index, _bind_2d, str );
}

void dump_tex_unit()
{
	DBG_PRINT_STRING( "tex unit cur is %d", tex_unit_cur->get_index()  );
	tex_units[0].print();
	tex_units[1].print();
}

void set_tex_unit_private( INT32 CONST index )
{
	if( index < sampler_nb && sampler_nb > 1 )	//	if only when texture calling glActiveTexture could even be wrong
	{
		glActiveTexture( GL_TEXTURE0 + index );
		tex_unit_cur = tex_units + index;
		if( GOL::b_tex_unit_verbose_ui )
			::VERBOSE_PRINT_STRING( "Tex Unit cur is now ---------------- %d", index  );
	}
	else
		ERR_PRINT_STRING( "texture unit %d don't exist only %d texture units", index, sampler_nb );
}

void	set_texture_dim_private( INT32 CONST dim )
{
	if( b_tex_unit_dim_use_ui )
	{
		if( is_state_cache_no()	)
		{
			switch( dim )
			{
			case 0:
				GOL::disable( GL_TEXTURE_1D );
				GOL::disable( GL_TEXTURE_2D );
				GOL::disable( GL_TEXTURE_3D );
				break;
			case 1:
				GOL::enable( GL_TEXTURE_1D );
				GOL::disable( GL_TEXTURE_2D );
				GOL::disable( GL_TEXTURE_3D );
				break;					
			case 2:
				GOL::disable( GL_TEXTURE_1D );
				GOL::enable( GL_TEXTURE_2D );
				GOL::disable( GL_TEXTURE_3D );
				break;
			case 3:
				GOL::disable( GL_TEXTURE_1D );
				GOL::disable( GL_TEXTURE_2D );
				GOL::enable( GL_TEXTURE_3D );
				break;
			}
		}
		else
		{
			switch( tex_unit_cur->get_dim() )
			{
			case 0:	break;
			case 1:	GOL::disable( GL_TEXTURE_1D );	break;
			case 2:	GOL::disable( GL_TEXTURE_2D );	break;
			case 3:	GOL::disable( GL_TEXTURE_3D );	break;
			}
			switch( dim )
			{
			case 0:	break;
			case 1: GOL::enable( GL_TEXTURE_1D );	break;
			case 2:	GOL::enable( GL_TEXTURE_2D );	break;
			case 3:	GOL::enable( GL_TEXTURE_3D );	break;
			}
		}
	}
	tex_unit_cur->set_dim( dim );
}

//	glTexEnvf

//texcoor
//should do a template
void	texncoord2( REAL CONST u, REAL CONST v )	{}

void	tex0coord2( REAL CONST u, REAL CONST v )	{	texcoor2( u, v );	}
void	tex1coord2( REAL CONST u, REAL CONST v )	{	multitexcoor2( GL_TEXTURE1, u, v );	}
void	tex2coord2( REAL CONST u, REAL CONST v )	{	multitexcoor2( GL_TEXTURE2, u, v );	}
void	tex3coord2( REAL CONST u, REAL CONST v )	{	multitexcoor2( GL_TEXTURE3, u, v );	}

void	tex01coord2( REAL CONST u, REAL CONST v )	{	texcoor2( u, v );	multitexcoor2( GL_TEXTURE1, u, v );	}
void	tex02coord2( REAL CONST u, REAL CONST v )	{	texcoor2( u, v );	multitexcoor2( GL_TEXTURE2, u, v );	}
void	tex03coord2( REAL CONST u, REAL CONST v )	{	texcoor2( u, v );	multitexcoor2( GL_TEXTURE3, u, v );	}
void	tex12coord2( REAL CONST u, REAL CONST v )	{	multitexcoor2( GL_TEXTURE1, u, v );	multitexcoor2( GL_TEXTURE2, u, v );	}
void	tex13coord2( REAL CONST u, REAL CONST v )	{	multitexcoor2( GL_TEXTURE1, u, v );	multitexcoor2( GL_TEXTURE3, u, v );	}
void	tex23coord2( REAL CONST u, REAL CONST v )	{	multitexcoor2( GL_TEXTURE2, u, v );	multitexcoor2( GL_TEXTURE3, u, v );	}

void	tex012coord2( REAL CONST u, REAL CONST v )	{	texcoor2( u, v );	multitexcoor2( GL_TEXTURE1, u, v );	multitexcoor2( GL_TEXTURE2, u, v );	}
void	tex013coord2( REAL CONST u, REAL CONST v )	{	texcoor2( u, v );	multitexcoor2( GL_TEXTURE1, u, v );	multitexcoor2( GL_TEXTURE3, u, v );	}
void	tex023coord2( REAL CONST u, REAL CONST v )	{	texcoor2( u, v );	multitexcoor2( GL_TEXTURE2, u, v );	multitexcoor2( GL_TEXTURE3, u, v );	}
void	tex123coord2( REAL CONST u, REAL CONST v )	{	multitexcoor2( GL_TEXTURE1, u, v );	multitexcoor2( GL_TEXTURE2, u, v );	multitexcoor2( GL_TEXTURE3, u, v );	}

void	tex0123coord2( REAL CONST u, REAL CONST v )	{	texcoor2( u, v );	multitexcoor2( GL_TEXTURE1, u, v );	multitexcoor2( GL_TEXTURE2, u, v );	multitexcoor2( GL_TEXTURE3, u, v );	}


void	texncoord2v( REAL CONST* pt )		{}

void	tex0coord2v( REAL CONST* pt )		{	texcoor2v( pt );	}
void	tex1coord2v( REAL CONST* pt )		{	multitexcoor2v( GL_TEXTURE1, pt );	}
void	tex2coord2v( REAL CONST* pt )		{	multitexcoor2v( GL_TEXTURE2, pt );	}
void	tex3coord2v( REAL CONST* pt )		{	multitexcoor2v( GL_TEXTURE3, pt );	}

void	tex01coord2v( REAL CONST* pt )		{	texcoor2v( pt );	multitexcoor2v( GL_TEXTURE1, pt );	}
void	tex02coord2v( REAL CONST* pt )		{	texcoor2v( pt );	multitexcoor2v( GL_TEXTURE2, pt );	}
void	tex03coord2v( REAL CONST* pt )		{	texcoor2v( pt );	multitexcoor2v( GL_TEXTURE3, pt );	}
void	tex12coord2v( REAL CONST* pt )		{	multitexcoor2v( GL_TEXTURE1, pt );		multitexcoor2v( GL_TEXTURE2, pt );	}
void	tex13coord2v( REAL CONST* pt )		{	multitexcoor2v( GL_TEXTURE1, pt );		multitexcoor2v( GL_TEXTURE3, pt );	}
void	tex23coord2v( REAL CONST* pt )		{	multitexcoor2v( GL_TEXTURE2, pt );		multitexcoor2v( GL_TEXTURE3, pt );	}

void	tex012coord2v( REAL CONST* pt )		{	texcoor2v( pt );	multitexcoor2v( GL_TEXTURE1, pt );		multitexcoor2v( GL_TEXTURE2, pt );	}
void	tex013coord2v( REAL CONST* pt )		{	texcoor2v( pt );	multitexcoor2v( GL_TEXTURE1, pt );		multitexcoor2v( GL_TEXTURE3, pt );	}
void	tex023coord2v( REAL CONST* pt )		{	texcoor2v( pt );	multitexcoor2v( GL_TEXTURE2, pt );		multitexcoor2v( GL_TEXTURE3, pt );	}
void	tex123coord2v( REAL CONST* pt )		{	multitexcoor2v( GL_TEXTURE1, pt );		multitexcoor2v( GL_TEXTURE2, pt );		multitexcoor2v( GL_TEXTURE3, pt );	}

void	tex0123coord2v( REAL CONST* pt )	{	texcoor2v( pt );	multitexcoor2v( GL_TEXTURE1, pt );		multitexcoor2v( GL_TEXTURE2, pt );		multitexcoor2v( GL_TEXTURE3, pt );	}

TEXCOORD2*	texcoord2_cur = tex01coord2;
TEXCOORD2V*	texcoord2v_cur = tex01coord2v;

namespace{
FINLINE	bool	is_not_need_uv( INT32 CONST index )
{
	c_tex_unit* unit = &tex_units[index];
	return unit->is_dim(0) || unit->_b_gen_s;
}
}

void	texcoord_set_0()
{
	texcoord2_cur = tex0coord2;
	texcoord2v_cur = tex0coord2v;
}

bool	texcoord_update()
{
	if( is_not_need_uv( 0 ) )
	{
		if( is_not_need_uv( 1 ) )
		{
			if( is_not_need_uv( 2 ) )
			{
				if( is_not_need_uv( 3 ) )
				{
					texcoord2_cur = texncoord2;
					texcoord2v_cur = texncoord2v;
					return false;
				}
				else
				{
					texcoord2_cur = tex3coord2;
					texcoord2v_cur = tex3coord2v;
				}
			}
			else
			{
				if( is_not_need_uv( 3 ) )
				{
					texcoord2_cur = tex2coord2;
					texcoord2v_cur = tex2coord2v;
				}
				else
				{
					texcoord2_cur = tex23coord2;
					texcoord2v_cur = tex23coord2v;
				}
			}
		}
		else
		{
			if( is_not_need_uv( 2 ) )
			{
				if( is_not_need_uv( 3 ) )
				{
					texcoord2_cur = tex1coord2;
					texcoord2v_cur = tex1coord2v;
				}
				else
				{
					texcoord2_cur = tex13coord2;
					texcoord2v_cur = tex13coord2v;
				}
			}
			else
			{
				if( is_not_need_uv( 3 ) )
				{
					texcoord2_cur = tex12coord2;
					texcoord2v_cur = tex12coord2v;
				}
				else
				{
					texcoord2_cur = tex123coord2;
					texcoord2v_cur = tex123coord2v;
				}
			}
		}
	}
	else
	{
		if( is_not_need_uv( 1 ) )
		{
			if( is_not_need_uv( 2 ) )
			{
				if( is_not_need_uv( 3 ) )
				{
					texcoord2_cur = tex0coord2;
					texcoord2v_cur = tex0coord2v;
				}
				else
				{
					texcoord2_cur = tex03coord2;
					texcoord2v_cur = tex03coord2v;
				}
			}
			else
			{
				if( is_not_need_uv( 3 ) )
				{
					texcoord2_cur = tex02coord2;
					texcoord2v_cur = tex02coord2v;
				}
				else
				{
					texcoord2_cur = tex023coord2;
					texcoord2v_cur = tex023coord2v;
				}
			}
		}
		else
		{
			if( is_not_need_uv( 2 ) )
			{
				if( is_not_need_uv( 3 ) )
				{
					texcoord2_cur = tex01coord2;
					texcoord2v_cur = tex01coord2v;
				}
				else
				{
					texcoord2_cur = tex013coord2;
					texcoord2v_cur = tex013coord2v;
				}
			}
			else
			{
				if( is_not_need_uv( 3 ) )
				{
					texcoord2_cur = tex012coord2;
					texcoord2v_cur = tex012coord2v;
				}
				else
				{
					texcoord2_cur = tex0123coord2;
					texcoord2v_cur = tex0123coord2v;
				}
			}
		}
	}
	return true;
}

// stored in fbo (when fbo so memorization to avoid call should deal with it) 
//GLenum read_buffer_cur = 0;
void	set_read_buffer(			GLenum CONST which )
{
	glReadBuffer( which );
}

void read_pixels( GLint CONST x, GLint CONST y, GLsizei CONST width, GLsizei CONST height, GLenum CONST format, GLenum CONST type, void* CONST pixels )
{
	SPY_PUSH_RANGE( "glReadPixels", spy::GOL_LOW );
		glReadPixels( x,y, width,height, format, type, pixels );
	SPY_POP_RANGE();
}
void copy_image_sub_data(	GLuint CONST src_name, GLenum CONST src_target, GLint CONST src_level, GLint CONST src_x, GLint CONST src_y, GLint CONST src_z,
 							GLuint CONST dst_name, GLenum CONST dst_target, GLint CONST dst_level, GLint CONST dst_x, GLint CONST dst_y, GLint CONST dst_z,
 							GLsizei CONST src_width, GLsizei CONST src_height, GLsizei CONST src_depth )
{
	SPY_PUSH_RANGE( "glCopyImageSubData", spy::GOL_LOW );
		glCopyImageSubData(		src_name, src_target, src_level, src_x, src_y, src_z,
 								dst_name, dst_target, dst_level, dst_x, dst_y, dst_z,
 								src_width, src_height, src_depth );
	SPY_POP_RANGE();
}

//	1d
void set_tex_1d_min_mag(			GLenum CONST minification, GLenum CONST magnification )
{
	set_tex_1d_minification(  minification );
	set_tex_1d_magnification( magnification );
}
void tex_image_1d( GLint CONST level, GLint CONST internalformat, GLsizei CONST width, GLenum CONST format, GLenum CONST type, void CONST * CONST pixels )
{
	SPY_PUSH_RANGE( "glTexImage1D", spy::GOL_LOW );
		glTexImage1D( GL_TEXTURE_1D, level, internalformat, width, 0, format, type, pixels );
	SPY_POP_RANGE();
}
void tex_sub_image_1d( GLint CONST level, GLint CONST xoffset, GLsizei CONST width, GLenum CONST format, GLenum CONST type, void CONST * CONST pixels )
{
	SPY_PUSH_RANGE( "glTexSubImage1D", spy::GOL_LOW );
		glTexSubImage1D( GL_TEXTURE_1D, level, xoffset, width, format, type, pixels );
	SPY_POP_RANGE();
}
//	2d
void	set_tex_2d_min_mag(			GLenum CONST minification, GLenum CONST magnification )
{
	set_tex_2d_minification(  minification );
	set_tex_2d_magnification( magnification );
}
void tex_image_2d( GLint CONST level, GLint CONST internalformat, GLsizei CONST width, GLsizei CONST height, GLenum CONST format, GLenum CONST type, void CONST * CONST pixels )
{
	SPY_PUSH_RANGE( "glTexImage2D", spy::GOL_LOW );
		glTexImage2D( GL_TEXTURE_2D, level, internalformat, width,height, 0, format, type, pixels );
	SPY_POP_RANGE();
}
void tex_sub_image_2d( GLint CONST level, GLint CONST xoffset, GLint CONST yoffset, GLsizei CONST width, GLsizei CONST height, GLenum CONST format, GLenum CONST type, void CONST * CONST pixels )
{
	SPY_PUSH_RANGE( "glTexSubImage2D", spy::GOL_LOW );
		glTexSubImage2D( GL_TEXTURE_2D, level, xoffset,yoffset, width, height, format, type, pixels );
	SPY_POP_RANGE();
}
void tex_image_compressed_2d( GLint CONST level, GLenum CONST internalformat, GLsizei CONST width, GLsizei CONST height, GLsizei CONST data_size, void CONST * CONST data )
{
	SPY_PUSH_RANGE( "glCompressedTexImage2D", spy::GOL_LOW );
		glCompressedTexImage2D( GL_TEXTURE_2D, level, internalformat, width, height, 0, data_size, data );
	SPY_POP_RANGE();
}

void get_tex_image_2d( GLint CONST level, GLenum CONST format, GLenum CONST type, void* CONST pixels )
{
	SPY_PUSH_RANGE( "glTexImage2D", spy::GOL_LOW );
		glGetTexImage( GL_TEXTURE_2D, level, format, type, (GLvoid *)pixels);
	SPY_POP_RANGE();
}
void get_tex_image_compressed_2d( GLint CONST level, void * CONST pixels )
{
	SPY_PUSH_RANGE( "glGetCompressedTexImage2D", spy::GOL_LOW );
		glGetCompressedTexImage( GL_TEXTURE_2D, 0, pixels );
	SPY_POP_RANGE();
}

void tex_copy_image_2d( GLint CONST level, GLint CONST internalFormat, GLint CONST x, GLint CONST y, GLsizei CONST width, GLsizei CONST height, GLsizei CONST border )
{
	SPY_PUSH_RANGE( "glCopyTexSubImage2D", spy::GOL_LOW );
		glCopyTexImage2D( GL_TEXTURE_2D, level, internalFormat, x,y, width,height, border );
	SPY_POP_RANGE();
}
void tex_copy_sub_image_2d( GLint CONST level, GLint CONST xoffset, GLint CONST yoffset, GLint CONST x, GLsizei CONST y, GLsizei CONST width, GLsizei CONST height )
{
	SPY_PUSH_RANGE( "glCopyTexSubImage2D", spy::GOL_LOW );
		glCopyTexSubImage2D( GL_TEXTURE_2D, level, xoffset,yoffset, x,y, width,height );
	SPY_POP_RANGE();
}
void copy_image_sub_data_2d(	GLuint CONST src_name, GLint CONST src_level, GLint CONST src_x, GLint CONST src_y,
 								GLuint CONST dst_name, GLint CONST dst_level, GLint CONST dst_x, GLint CONST dst_y,
 								GLsizei CONST src_width, GLsizei CONST src_height )
{
	if( b_copy_image_sub_do )
	{
		SPY_PUSH_RANGE( "glCopyImageSubData", spy::GOL_LOW );
			glCopyImageSubData(		src_name, GL_TEXTURE_2D, src_level, src_x, src_y, 0,
 									dst_name, GL_TEXTURE_2D, dst_level, dst_x, dst_y, 0,
 									src_width, src_height, 1 );
		SPY_POP_RANGE();
	}
	else
		debug_break( "No glCopyImageSubData() prtesent or allowed, skipping texture copy" );
} 
void	copy_image_sub_data_2d(	GLuint CONST src_name, GLint CONST src_level,
 								GLuint CONST dst_name, GLint CONST dst_level,
 								GLsizei CONST src_width, GLsizei CONST src_height )
{
	copy_image_sub_data_2d(		src_name, src_level, 0, 0,
 								dst_name, dst_level, 0, 0,
 								src_width, src_height );
}

//	3d
void	set_tex_3d_min_mag(			GLenum CONST minification, GLenum CONST magnification )
{
	set_tex_3d_minification(  minification );
	set_tex_3d_magnification( magnification );
}
void tex_image_3d( GLint CONST level, GLint CONST internalFormat, GLsizei CONST width, GLsizei CONST height, GLsizei CONST depth, GLenum CONST format, GLenum CONST type, void CONST * CONST pixels )
{
	SPY_PUSH_RANGE( "glTexImage3D", spy::GOL_LOW );
		glTexImage3D( GL_TEXTURE_3D, level, internalFormat, width, height, depth, 0, format, type, pixels );
	SPY_POP_RANGE();
}
void tex_sub_image_3d( GLint CONST level, GLint CONST xoffset, GLint CONST yoffset, GLint CONST zoffset, GLsizei CONST width, GLsizei CONST height, GLsizei CONST depth, GLenum CONST format, GLenum CONST type, void CONST * CONST pixels )
{
	SPY_PUSH_RANGE( "glTexSubImage3D", spy::GOL_LOW );
		glTexSubImage3D( GL_TEXTURE_3D, level, xoffset, yoffset, zoffset, width, height, depth, format, type, pixels );
	SPY_POP_RANGE();
}

void get_tex_image_3d( GLint CONST level, GLenum CONST format, GLenum CONST type, void* CONST pixels )
{
	SPY_PUSH_RANGE( "glTexImage3D", spy::GOL_LOW );
		glGetTexImage( GL_TEXTURE_3D, level, format, type, (GLvoid *)pixels);
	SPY_POP_RANGE();
}
void get_tex_image_compressed_3d( GLint CONST level, void * CONST pixels )
{
	SPY_PUSH_RANGE( "glGetCompressedTexImage3D", spy::GOL_LOW );
		glGetCompressedTexImage( GL_TEXTURE_3D, 0, pixels );
	SPY_POP_RANGE();
}

//	having an opengl texture complete was a requirement for some driver
//this an experimental helper fns remember state are eventually cached and controlled by a tex_xd layer
void make_tex_complete_2d( INT32 CONST name_gl )
{
	if( name_gl != 0 )
		GOL::bind_texture_2d( name_gl );
//	glGenerateMipmap( GL_TEXTURE_2D);
	GOL::set_tex_2d_wrap_s( GL_CLAMP_TO_EDGE );
	GOL::set_tex_2d_wrap_s( GL_CLAMP_TO_EDGE );
	GOL::set_tex_2d_min_mag( GL_LINEAR, GL_LINEAR );
//	GOL::set_tex_2d_max_level(0);

//	GOL::set_tex_2d_magnification( GL_LINEAR );
//	GOL::set_tex_2d_minification( GL_NEAREST_MIPMAP_LINEAR ); 
//	GOL::set_tex_2d_minification( GL_NEAREST );
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
//	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
//	GOL::finish();
}

}	//namespace GOL
