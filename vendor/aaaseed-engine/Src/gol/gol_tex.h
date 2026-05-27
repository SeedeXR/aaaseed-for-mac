
#ifdef AAA_GOL_TEX_H
#error "GOL_TEX_H included more than once."
#endif
#define AAA_GOL_TEX_H 1


#ifndef AAA_GOL_BASE_H
#	include "gol_base.h"
#endif
#ifndef AAA_GOL_ATTRIB_H
#	include "gol_attrib.h"
#endif
#ifndef AAA_AAA_MATH_H
#	include "math/aaa_math.h"
#endif

namespace GOL	{

//TEXTURE
//
extern	void	init_texture();
extern	void	deinit_texture();

extern	void	reset_texture();

extern	bool	b_tex_allow_ui;
extern	bool	b_tex_verbose_ui;
extern	bool	b_tex_unit_verbose_ui;
extern	bool	b_tex_unit_dim_use_ui;

extern	void	flip_tex_allow();

extern	INT32	tex_size_max;

//NPOT ( Non Power Of Two )
//
extern	bool	b_tex_npot_allow_ui;
extern	bool	b_tex_npot_can;
extern	void	init_tex_npot();
FINLINE	bool	is_tex_npot()	{	return b_tex_npot_allow_ui && b_tex_npot_can;	}

FINLINE	void	adjust_tex_size_to_valid( INT32& sx_out, INT32& sy_out, INT32 CONST sx_in, INT32 CONST sy_in )
{
	if( is_tex_npot() )
	{
		sx_out = sx_in ;
		sy_out = sy_in ;
	}
	else
	{
		sx_out = POW2_EQUAL_OR_SUP( sx_in );
		sy_out = POW2_EQUAL_OR_SUP( sy_in );
	}
}
FINLINE	void	adjust_tex_size_to_valid( INT32& sx, INT32& sy )
{
	if( !is_tex_npot() )
	{
		sx = POW2_EQUAL_OR_SUP( sx );
		sy = POW2_EQUAL_OR_SUP( sy );
	}
}
FINLINE	void	adjust_tex_size_to_valid( INT32& sx, INT32& sy, INT32& sz )
{
	if( !is_tex_npot() )
	{
		sx = POW2_EQUAL_OR_SUP( sx );
		sy = POW2_EQUAL_OR_SUP( sy );
		sz = POW2_EQUAL_OR_SUP( sz );
	}
}

//Rectangle
//
extern	bool	b_tex_rectangle_can;
extern	void	init_tex_rectangle();

//Float
//
extern	bool	b_tex_float_can;
extern	void	init_tex_float();

//Compression
//
extern	bool	b_tex_compressed_allow_ui;
extern	bool	b_tex_compressed_can;
extern	void	init_tex_compressed();
FINLINE	bool	is_tex_compressed()
{
	return b_tex_compressed_allow_ui && b_tex_compressed_can;
}


//Internal format type
//
//todotex add invalid, unknown, default
enum class INTERNAL_TYPE_FORCE : INT32
{
	DEFAULT = 0,	//	GL_NONE		use type corresponding to data DEFAULT
	UINT_8,			//	GL_UNSIGNED_BYTE
	UINT_16,		//	GL_UNSIGNED_SHORT
	FLOAT_16,		//	GL_HALF_FLOAT
	FLOAT_32,		//	GL_FLOAT
	MAX_NB
};

//	IT_DEFAULT = 0,
enum class INTERNAL_TYPE : INT32
{
	UINT_8 = 0,			//	GL_UNSIGNED_BYTE
	UINT_16,			//	GL_UNSIGNED_SHORT
	FLOAT_16,			//	GL_HALF_FLOAT
	FLOAT_32,			//	GL_FLOAT
	DEPTH_16,			//	HACKED for nOw
	DEPTH_24,			//	HACKED for nOw
	DEPTH_32,			//	HACKED for nOw
	//todo add depth 32f look at https://gamedev.stackexchange.com/questions/168241/is-gl-depth-component32-deprecated-in-opengl-4-5
	//in fact it is depth 32s depth 32 is only optionnal
	DEPTH_24_STENCIL_8,	//	HACKED for nOw
	DEPTH_32_STENCIL_8,	//	HACKED for nOw

	NONE,				//	GL_NONE
	INVALID,			//	Invalid
	UNKNOWN,			//	Unknown
	MAX_NB
};

extern	INTERNAL_TYPE_FORCE		internal_type_def_asked;
//extern	INTERNAL_TYPE			internal_type_def_cur;
//extern	GLenum CONST			gl_type[				(INT32)INTERNAL_TYPE::MAX_NB];

CONSTEXPR C_PCHAR_C		int8_str	= "int8";
CONSTEXPR C_PCHAR_C		int16_str	= "int16";
CONSTEXPR C_PCHAR_C		fp16_str	= "fp16";
CONSTEXPR C_PCHAR_C		fp32_str	= "fp32";

CONSTEXPR C_PCHAR_C		internal_type_asked_str[(INT32)INTERNAL_TYPE_FORCE::MAX_NB]	=
{
	"Default",	int8_str,	int16_str,	fp16_str,	fp32_str
};
CONSTEXPR C_PCHAR_C		internal_type_str[		(INT32)INTERNAL_TYPE::MAX_NB]		=
{				int8_str,	int16_str,	fp16_str,	fp32_str,		
				"Depth16",	"Depth24",	"Depth32",	"Depth24_Stencil8",	"Depth32_Stencil8",
				"None",		"Invalid",	"Unknown"
};

FINLINE	INTERNAL_TYPE_FORCE		get_internal_type_def_asked()							{ return internal_type_def_asked;			}
FINLINE	INTERNAL_TYPE_FORCE*	get_internal_type_def_asked_pt()						{ return &internal_type_def_asked;			}

//FINLINE	INTERNAL_TYPE		get_internal_type_def()									{ return internal_type_def_cur;						}
//FINLINE	INTERNAL_TYPE*		get_internal_type_def_pt()								{ return &internal_type_def_cur;					}
//FINLINE	GLenum				get_internal_type_def_gl()								{ return gl_type[(INT32)get_internal_type_def()];	}


FINLINE	INTERNAL_TYPE	get_internal_type( GLenum CONST type )
{
	GOL::INTERNAL_TYPE	internal_type = GOL::INTERNAL_TYPE::UINT_8;
	switch( GOL::get_internal_type_def_asked() )
	{
	case GOL::INTERNAL_TYPE_FORCE::DEFAULT:
		switch ( type )
		{
		case GL_HALF_FLOAT:		internal_type = GOL::INTERNAL_TYPE::FLOAT_16;	break;
		case GL_FLOAT:			internal_type = GOL::INTERNAL_TYPE::FLOAT_32;	break;
		case GL_UNSIGNED_SHORT:	internal_type = GOL::INTERNAL_TYPE::UINT_16;	break;
		case GL_UNSIGNED_BYTE:	internal_type = GOL::INTERNAL_TYPE::UINT_8;		break;
		default:	//todo do better
			internal_type = GOL::INTERNAL_TYPE::INVALID;		break;
		}
		break;
	case GOL::INTERNAL_TYPE_FORCE::UINT_8:		internal_type = GOL::INTERNAL_TYPE::UINT_8;		break;
	case GOL::INTERNAL_TYPE_FORCE::UINT_16:		internal_type = GOL::INTERNAL_TYPE::UINT_16;	break;
	case GOL::INTERNAL_TYPE_FORCE::FLOAT_16:	internal_type = GOL::INTERNAL_TYPE::FLOAT_16;	break;
	case GOL::INTERNAL_TYPE_FORCE::FLOAT_32:	internal_type = GOL::INTERNAL_TYPE::FLOAT_32;	break;
	}
	return internal_type;
}

FINLINE	INT32	get_byte_nb_from_internal_type( INTERNAL_TYPE CONST internal_type )
{																
	INT32 byte_nb;
	switch( internal_type )
	{
	case INTERNAL_TYPE::FLOAT_32:			byte_nb = 4;	break;
	case INTERNAL_TYPE::FLOAT_16:
	case INTERNAL_TYPE::UINT_16:			byte_nb = 2;	break;
	case INTERNAL_TYPE::UINT_8:				byte_nb = 1;	break;
											//check all these values
	case INTERNAL_TYPE::DEPTH_16:			byte_nb = 2;	break;
	case INTERNAL_TYPE::DEPTH_24:
	case INTERNAL_TYPE::DEPTH_32:
	case INTERNAL_TYPE::DEPTH_24_STENCIL_8:	byte_nb = 4;	break;
	case INTERNAL_TYPE::DEPTH_32_STENCIL_8:	byte_nb = 8;	break;
	default:
		byte_nb = 1;
		ERR_PRINT_STRING( "Unsupported internal type %s in %s()", internal_type_str[(INT32)internal_type], __FUNCTION__ );
		break;
	}
	return byte_nb;
}

//FINLINE	void		set_internal_type_def(	CONST INTERNAL_TYPE internal_type )	{ internal_type_def_cur = get_internal_type_valid( internal_type ); }
//FINLINE	void		set_internal_type_def_byte()								{ set_internal_type_def( INTERNAL_TYPE::UINT_8 );	}
//FINLINE	void		set_internal_type_def_half_float()							{ set_internal_type_def( INTERNAL_TYPE::FLOAT_16 );	}

//FINLINE	void		init_internal_type_def()									{ set_internal_type_def( internal_type_def_cur );	}

//	extern	GLenum	make_internal_format_gl(			INT32 CONST channel_nb,		GLenum CONST type						);
extern	GLenum	make_internal_format_gl(			INT32 CONST channel_nb,		CONST INTERNAL_TYPE internal_type	);
extern	bool	is_same_internal_format_gl(			INT32 CONST channel_nb_new,	CONST INTERNAL_TYPE internal_type_new,
													INT32 CONST channel_nb,		CONST INTERNAL_TYPE internal_type		);
//FINLINE	GLenum		make_internal_format_gl(	INT32 CONST channel_nb	)
//						{
//							return make_internal_format_gl( channel_nb, get_internal_type_def() );
//						}

FINLINE	GLenum	get_internal_format_low( GLenum CONST what )
{
	GLint	internalformat = 0;
	glGetTexLevelParameteriv( what, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalformat );
	return internalformat;
}
		
extern	C_PCHAR	get_internal_format_str_from_glenum(	GLenum CONST format );
extern	C_PCHAR	get_format_str_from_glenum(				GLenum CONST format );

extern	C_PCHAR	get_compressed_format_str_from_glenum(	GLenum CONST format );
extern	INT32	get_channel_nb_from_glenum(				GLenum CONST format );

extern	void	print_texture_implementation();

FINLINE	GLenum	get_1d_internal_format()			{	return get_internal_format_low( GL_TEXTURE_1D );					}
FINLINE	C_PCHAR	get_1d_internal_format_as_str()		{	return get_format_str_from_glenum( get_1d_internal_format() );		}

FINLINE	GLenum	get_2d_internal_format()			{	return get_internal_format_low( GL_TEXTURE_2D );					}
FINLINE	C_PCHAR	get_2d_internal_format_as_str()		{	return get_format_str_from_glenum( get_2d_internal_format() );		}

FINLINE	GLenum	get_3d_internal_format()			{	return get_internal_format_low( GL_TEXTURE_3D );					}
FINLINE	C_PCHAR	get_3d_internal_format_as_str()		{	return get_format_str_from_glenum( get_3d_internal_format() );		}


FINLINE	INT32	get_size_compressed_2d()
{
	GLint	compressed;
	GLint	compressed_size = 0;
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &compressed );
	// if the compression has been successful
	if( compressed == GL_TRUE )
	{
		glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compressed_size);
	}
	return compressed_size;
}

FINLINE	bool	is_internal_format_compressed_2d()
{
	GLint	compressed = GL_FALSE;
	glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED, &compressed );
	// if the compression has been successful
	return compressed == GL_TRUE;
}
//Format
//
extern	GLenum	get_gl_format_from_channel_nb(	INT32 CONST channel_nb,	bool CONST b_bgr = false	);

//Swizzle
//
extern	bool	b_tex_swizzle_can;
extern	bool	b_mono_is_alpha_ui;
extern	GLint	swiz[5][4];

FINLINE	GLint CONST * get_swizzle_private( INT32 CONST channel_nb )
{
	if( channel_nb == 1 && b_mono_is_alpha_ui )
		return swiz[0];
	else
		return swiz[channel_nb];
}


FINLINE	void	set_swizzle_direct( GLenum CONST target, GLint CONST * params )
{
	glTexParameteriv( target, GL_TEXTURE_SWIZZLE_RGBA, params );
}

// GL_ALPHA is depreciated in GL4 we should use extension GL_ARB_texture_swizzle
//todo use store info to avoid setting it every time
FINLINE	void	set_swizzle( GLenum CONST target, INT32 CONST channel_nb )
{
	if( b_tex_swizzle_can )
		set_swizzle_direct( target, get_swizzle_private(channel_nb) );
}

FINLINE	void	set_swizzle_def( GLenum CONST target )
{
	if( b_tex_swizzle_can )
		set_swizzle_direct( target, swiz[4] );
}

//Bind
//
//extern	void	bind_texture(	GLenum CONST target,	UINT32 CONST gl_name );
extern	void	bind_texture_1d(						UINT32 CONST gl_name );
extern	void	bind_texture_2d(						UINT32 CONST gl_name );
extern	UINT32	get_texture_2d();
extern	void	bind_texture_3d(						UINT32 CONST gl_name );
extern	UINT32	get_texture_3d();
		void	bind_texture_2d_and_init(				UINT32 CONST gl_name );

FINLINE	void	gen_textures(		INT32 CONST nb,		GLuint* CONST gl_name )	{ glGenTextures(	nb, gl_name );	}
FINLINE	void	gen_texture(							GLuint* CONST gl_name )	{ gen_textures(		1,  gl_name );	}
FINLINE	void	delete_textures(	INT32 CONST nb,		GLuint* CONST gl_name )	{ glDeleteTextures(	nb, gl_name );	}
FINLINE void	delete_texture(							GLuint* CONST gl_name )	{ delete_textures(	1,  gl_name );	*gl_name = 0;	}



//MIPMAP
//
//todo	refine name to include ext or hardware notion
extern	bool	b_mipmap_generate_allow_ui;
extern	bool	b_mipmap_generate_do;

FINLINE	bool	is_mipmap_generate()		{	return b_mipmap_generate_do;	}

//	_direct have to be called only when is_mipmap_generate() have been tested
FINLINE	void	generate_mipmap_1d_direct()	{	glGenerateMipmap( GL_TEXTURE_1D );	}
FINLINE	void	generate_mipmap_1d()		{
												if( is_mipmap_generate() )
													generate_mipmap_1d_direct();
											}

FINLINE	void	generate_mipmap_2d_direct()	{	glGenerateMipmap( GL_TEXTURE_2D );	}
FINLINE	void	generate_mipmap_2d()		{
												if( is_mipmap_generate() )
													generate_mipmap_2d_direct();
											}

FINLINE	void	generate_mipmap_3d_direct()	{	glGenerateMipmap( GL_TEXTURE_3D );	}
FINLINE	void	generate_mipmap_3d()		{
												if( is_mipmap_generate() )
													generate_mipmap_3d_direct();
											}

// require Direct State Access only from Gl 4.5 
FINLINE	void	generate_mipmap_texture_direct( GLuint CONST tex_name )
											{	glGenerateTextureMipmap( tex_name );	}
FINLINE	void	generate_mipmap_texture( GLuint CONST tex_name )
											{
												if( is_mipmap_generate() )
													generate_mipmap_texture_direct( tex_name );
											}
/*
//extern	bool	b_mipmap_generate; should be done by texture
DEPRECEATED IN 3.0 REMOVED IN 3.1
FINLINE	void	enable_mipmap_generate()
{
	if( is_mipmap_generate() )
	{
//		if( b_mipmap_generate == false )
		{
			glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_TRUE );
//			b_mipmap_generate = true;
		}
	}
}

FINLINE	void	disable_mipmap_generate()
{
	if( is_mipmap_generate() )
	{
//		if( b_mipmap_generate == true )
		{
			glTexParameteri( GL_TEXTURE_2D, GL_GENERATE_MIPMAP, GL_FALSE );
//			b_mipmap_generate = false;
		}
	}
}

FINLINE	void	set_mipmap_generate( bool in )
{
	if( in )
		enable_mipmap_generate();
	else
		disable_mipmap_generate();
}
*/
//MULTISAMPLE
extern	bool	b_multisampling_can;		//todo we should get ghis info (Maa think it come from the pixel format used to open CTX 
extern	bool	b_multisampling_allow_ui;
extern	bool	b_multisampling_do;
extern	bool	b_multisampling_state;

FINLINE	void	disable_multisampling()
{
	if( is_state_cache_no() || b_multisampling_state )
	{
		GOL::disable( GL_MULTISAMPLE_ARB );
		b_multisampling_state = false;
	}
}

FINLINE	void	enable_multisampling()
{
	if( (is_state_cache_no() || !b_multisampling_state) && b_multisampling_do )
	{
		GOL::enable( GL_MULTISAMPLE_ARB );
		b_multisampling_state = true;
	}
}

FINLINE	void	set_multisampling( bool CONST b )
{
	if( b )
		enable_multisampling();
	else
		disable_multisampling();
}

//Level
FINLINE void	set_tex_2d_max_level(	GLint CONST level=1000 )				{ glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, level ); }
																					  
//Wrap by texture unit																  
FINLINE void	set_tex_1d_wrap_s(		GLenum CONST s )						{ glTexParameteri( GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, s );	}
																					  
FINLINE void	set_tex_2d_wrap_s(		GLenum CONST s )						{ glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, s );	}
FINLINE void	set_tex_2d_wrap_t(		GLenum CONST t )						{ glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, t );	}
																					  
FINLINE void	set_tex_3d_wrap_s(		GLenum CONST s )						{ glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, s );	}
FINLINE void	set_tex_3d_wrap_t(		GLenum CONST t )						{ glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, t );	}
FINLINE void	set_tex_3d_wrap_r(		GLenum CONST r )						{ glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, r );	}
																					  
//Wrap by texture only from Gl 4.5 unused in AAA for now							  
FINLINE void	set_texture_wrap_s(		GLuint CONST tex_name, GLenum CONST s )	{ glTextureParameteri(	tex_name, GL_TEXTURE_WRAP_S, s );	}
FINLINE void	set_texture_wrap_t(		GLuint CONST tex_name, GLenum CONST t )	{ glTextureParameteri(	tex_name, GL_TEXTURE_WRAP_T, t );	}
FINLINE void	set_texture_wrap_r(		GLuint CONST tex_name, GLenum CONST r )	{ glTextureParameteri(	tex_name, GL_TEXTURE_WRAP_R, r );	}

//border
FINLINE void	set_tex_2d_border_color( FP32 CONST * CONST pt )				{ glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, pt );	}
//Minification	Magnification
//

FINLINE void	set_tex_minification(		GLenum CONST target, GLenum CONST minification )
{
	glTexParameteri( target, GL_TEXTURE_MIN_FILTER, minification );
}
FINLINE void	set_tex_magnification(		GLenum CONST target, GLenum CONST magnification )
{
	glTexParameteri( target, GL_TEXTURE_MAG_FILTER, magnification );
}
FINLINE void	set_texture_minification(	GLuint CONST tex_name, GLenum CONST minification )
{
	glTextureParameteri( tex_name, GL_TEXTURE_MIN_FILTER, minification );
}
FINLINE void	set_texture_magnification(	GLuint CONST tex_name, GLenum CONST magnification )
{
	glTextureParameteri( tex_name, GL_TEXTURE_MAG_FILTER, magnification );
}

FINLINE void	set_tex_1d_minification(	GLenum CONST minification )		{	set_tex_minification(	GL_TEXTURE_1D, minification );	}
FINLINE void	set_tex_1d_magnification(	GLenum CONST magnification )	{	set_tex_magnification(	GL_TEXTURE_1D, magnification );	}
		void	set_tex_1d_min_mag(			GLenum CONST minification, GLenum CONST magnification );

FINLINE void	set_tex_2d_minification(	GLenum CONST minification )		{	set_tex_minification(	GL_TEXTURE_2D, minification );	}
FINLINE void	set_tex_2d_magnification(	GLenum CONST magnification )	{	set_tex_magnification(	GL_TEXTURE_2D, magnification );	}
		void	set_tex_2d_min_mag(			GLenum CONST minification, GLenum CONST magnification );

FINLINE void	set_tex_3d_minification(	GLenum CONST minification )		{	set_tex_minification(	GL_TEXTURE_3D, minification );	}
FINLINE void	set_tex_3d_magnification(	GLenum CONST magnification )	{	set_tex_magnification(	GL_TEXTURE_3D, magnification );	}
		void	set_tex_3d_min_mag(			GLenum CONST minification, GLenum CONST magnification );

//Anisotropic
//
extern	bool	b_anisotropic_filtering_can;
extern	bool	b_anisotropic_filtering_allow_ui;
extern	bool	b_anisotropic_filtering_do;
extern	GLfloat	anisotropy_max; 
extern	GLfloat	anisotropy_value; 

//todonow clean up at list have a state variable to avoid fn calls
FINLINE	bool	is_anisotropic_filtering()
{
	return b_anisotropic_filtering_do;
}

FINLINE	void	enable_anisotropic_filtering( GLenum CONST what )
{
	if( is_anisotropic_filtering() )
		glTexParameterf( what, GL_TEXTURE_MAX_ANISOTROPY, anisotropy_value );
}
FINLINE	void	disable_anisotropic_filtering( GLenum CONST what )
{
	if( is_anisotropic_filtering() )
		glTexParameterf( what, GL_TEXTURE_MAX_ANISOTROPY, 1. );
}

FINLINE	void	set_anisotropic_filtering( GLenum CONST what, bool CONST in )
{
	if( in )
		enable_anisotropic_filtering( what );
	else
		disable_anisotropic_filtering( what );
}

FINLINE	void	enable_anisotropic_filtering_1d()				{	enable_anisotropic_filtering(	GL_TEXTURE_1D );		}
FINLINE	void	disable_anisotropic_filtering_1d()				{	disable_anisotropic_filtering(	GL_TEXTURE_1D );		}
FINLINE	void	set_anisotropic_filtering_1d( bool CONST in )	{	set_anisotropic_filtering(		GL_TEXTURE_1D,	in );	}

FINLINE	void	enable_anisotropic_filtering_2d()				{	enable_anisotropic_filtering(	GL_TEXTURE_2D );		}
FINLINE	void	disable_anisotropic_filtering_2d()				{	disable_anisotropic_filtering(	GL_TEXTURE_2D );		}
FINLINE	void	set_anisotropic_filtering_2d( bool CONST in )	{	set_anisotropic_filtering(		GL_TEXTURE_2D,	in );	}

FINLINE	void	enable_anisotropic_filtering_3d()				{	enable_anisotropic_filtering(	GL_TEXTURE_3D );		}
FINLINE	void	disable_anisotropic_filtering_3d()				{	disable_anisotropic_filtering(	GL_TEXTURE_3D );		}
FINLINE	void	set_anisotropic_filtering_3d( bool CONST in )	{	set_anisotropic_filtering(		GL_TEXTURE_3D,	in );	}

//FINLINE	void	enable_anisotropic_filtering( GLuint CONST tex_name )
//{
//	if( is_anisotropic_filtering() )
//		glTextureParameterf( tex_name, GL_TEXTURE_MAX_ANISOTROPY, anisotropy_value );
//}
//FINLINE	void	disable_anisotropic_filtering( GLuint CONST tex_name )
//{
//	if( is_anisotropic_filtering() )
//		glTextureParameterf( tex_name, GL_TEXTURE_MAX_ANISOTROPY, 1. );
//}

//Texture unit
//
class	c_tex_unit
{
private:
	INT32	_bind_2d;
	INT32	_bind_3d;
	INT32	_dim;		//todo reduce memory footprint ?
						//todo regroup with _env_mode and _b_gen_
	INT32	_index;
public:
	bool	_b_gen_s;
	bool	_b_gen_t;
	bool	_b_gen_q;
	bool	_b_gen_r;
	INT32	_env_mode;

	c_tex_unit();
	FINLINE	INT32	get_bind_2d()						CONST	{	return _bind_2d;			}
	FINLINE	bool	is_bind_2d(		INT32 CONST bind )	CONST	{	return _bind_2d == bind;	}
	FINLINE	void	set_bind_2d(	INT32 CONST bind )			{	_bind_2d = bind;			}

	FINLINE	INT32	get_bind_3d()						CONST	{	return _bind_3d;			}
	FINLINE	bool	is_bind_3d(		INT32 CONST bind )	CONST	{	return _bind_3d == bind;	}
	FINLINE	void	set_bind_3d(	INT32 CONST bind )			{	_bind_3d = bind;			}

	FINLINE	INT32	get_index()							CONST	{	return _index;				}
	FINLINE	bool	is_index(		INT32 CONST index )	CONST	{	return _index == index;		}
	FINLINE	void	set_index(		INT32 CONST index )			{	_index = index;				}

	FINLINE	INT32	get_dim()							CONST	{	return _dim;				}
	FINLINE	bool	is_dim(			INT32 CONST dim )	CONST	{	return _dim == dim;			}
	FINLINE	void	set_dim(		INT32 CONST dim )			{	_dim = dim;					}

	FINLINE void	print();
};

//this the modern version
//AMD
CONSTEXPR	INT32		TEX_SAMPLER_NB_MAX	= 16;
extern		INT32		sampler_nb_context;
extern		INT32		sampler_nb;
//this the old version implemented in layer tex_unit
CONSTEXPR	INT32		TEX_UNIT_NB_MAX		= 4;
extern		INT32		tex_unit_nb_context;
extern		INT32		tex_unit_nb;

extern	c_tex_unit	tex_units[TEX_SAMPLER_NB_MAX];
extern	c_tex_unit*	tex_unit_cur;

extern	void dump_tex_unit();

//todo pick right name
extern void set_texture_dim_private( INT32 CONST dim );
FINLINE	void	set_texture_dim( INT32 CONST dim )
{
	if( (is_state_cache_no() || !tex_unit_cur->is_dim( dim )) && b_tex_allow_ui )
		set_texture_dim_private( dim );
}

FINLINE	void	disable_texture_and_force_update_later()
{
	if( b_tex_unit_dim_use_ui )
	{
		GOL::disable( GL_TEXTURE_1D );
		GOL::disable( GL_TEXTURE_2D );
		GOL::disable( GL_TEXTURE_3D );
	}
	tex_unit_cur->set_dim( -1 );	//hack	for lua push_attrib / pop_attrib or other : will force change
}
//FINLINE	void	set_texture_0D_private()
//{
//	if( b_tex_unit_dim_use_ui )
//	{
//		GOL::disable( GL_TEXTURE_1D );
//		GOL::disable( GL_TEXTURE_2D );
//		GOL::disable( GL_TEXTURE_3D );
//	}
//	tex_unit_cur->set_dim( 0 );
//}
FINLINE	void	set_texture_0D()
{
	if( b_tex_unit_dim_use_ui )
	{
		//if( tex_unit_cur->is_dim( 0 ) )
		//	return;
		//set_texture_0D_private();
		auto dim = tex_unit_cur->get_dim();
		if( is_state_cache_no() )
		{
			tex_unit_cur->set_dim( 0 );
			GOL::disable( GL_TEXTURE_1D );
			GOL::disable( GL_TEXTURE_2D );
			GOL::disable( GL_TEXTURE_3D );
		}
		else if( dim != 0 )
		{
			tex_unit_cur->set_dim( 0 );
			switch( dim )
			{
			case 1:	GOL::disable( GL_TEXTURE_1D );	break;
			case 2:	GOL::disable( GL_TEXTURE_2D );	break;
			case 3:	GOL::disable( GL_TEXTURE_3D );	break;
			}
		}
	}
	else
		tex_unit_cur->set_dim( 0 );
}
FINLINE	void	disable_texture()
{
	set_texture_0D();
}

FINLINE	void	set_texture_1D_private()
{
	if( b_tex_unit_dim_use_ui )
	{
		GOL::enable( GL_TEXTURE_1D );
		GOL::disable( GL_TEXTURE_2D );
		GOL::disable( GL_TEXTURE_3D );
	}
	tex_unit_cur->set_dim( 1 );
}
FINLINE	void	set_texture_1D()
{
	if( (is_state_cache_no() || !tex_unit_cur->is_dim( 1 )) && b_tex_allow_ui )
		set_texture_1D_private();
}

FINLINE	void	set_texture_2D_private()
{
	if( b_tex_unit_dim_use_ui )
	{
		//todo on Nvidia/windows highest dimension win, on Mac ?
		GOL::disable( GL_TEXTURE_1D );	//2021 was commented but seens ok
		GOL::enable( GL_TEXTURE_2D );
		GOL::disable( GL_TEXTURE_3D );
	}
	tex_unit_cur->set_dim( 2 );
}
FINLINE	void	set_texture_2D()
{
	if( (is_state_cache_no() || !tex_unit_cur->is_dim( 2 )) && b_tex_allow_ui )
		set_texture_2D_private();
}

FINLINE	void	set_texture_3D_private()
{
	if( b_tex_unit_dim_use_ui )
	{
		//todo on Nvidia/windows highest dimension win, on Mac ?
		GOL::disable( GL_TEXTURE_1D );	//2021 was commented but seens ok
		GOL::disable( GL_TEXTURE_2D );	//2021 was commented but seens ok
		GOL::enable( GL_TEXTURE_3D );
	}
	tex_unit_cur->set_dim( 3 );
}
FINLINE	void	set_texture_3D()
{
	if( (is_state_cache_no() || !tex_unit_cur->is_dim( 3 )) && b_tex_allow_ui )
		set_texture_3D_private();
}

FINLINE	INT32	get_texture_dim()
{
	return tex_unit_cur->get_dim();
}
FINLINE	void	push_texture_dim_low( INT32 CONST dim )
{
	if( b_tex_allow_ui )
	{
		c_poper* poper = get_att_poper();
		switch( dim )
		{
		case 0:	poper->add_fn( set_texture_0D );	break;
		case 1:	poper->add_fn( set_texture_1D );	break;
		case 2:	poper->add_fn( set_texture_2D );	break;
		case 3:	poper->add_fn( set_texture_3D );	break;
		}
	}
}
FINLINE	void	push_texture_dim( INT32 CONST dim )
{
	INT32 dim_cur = get_texture_dim();
	if( is_state_cache_no() || dim_cur != dim )	//potential state sync problem
	{
		push_texture_dim_low( dim_cur );
		set_texture_dim( dim );
	}
}

// GEN_S
FINLINE	void	disable_texture_gen_s_direct()
{
	GOL::disable( GL_TEXTURE_GEN_S );
	tex_unit_cur->_b_gen_s = false;
}
FINLINE	void	disable_texture_gen_s()
{
	if( is_state_cache_no() || tex_unit_cur->_b_gen_s )
		disable_texture_gen_s_direct();
}
FINLINE	void	enable_texture_gen_s()
{
	if( !is_state_cache_no() && tex_unit_cur->_b_gen_s )
		return;
	GOL::enable( GL_TEXTURE_GEN_S );
	tex_unit_cur->_b_gen_s = true;
}
// GEN_T
FINLINE	void	disable_texture_gen_t_direct()
{
	GOL::disable( GL_TEXTURE_GEN_T );
	tex_unit_cur->_b_gen_t = false;
}
FINLINE	void	disable_texture_gen_t()
{
	if( is_state_cache_no() || tex_unit_cur->_b_gen_t )
		disable_texture_gen_t_direct();
}
FINLINE	void	enable_texture_gen_t()
{
	if( !is_state_cache_no() && tex_unit_cur->_b_gen_t )
		return;
	GOL::enable( GL_TEXTURE_GEN_T );
	tex_unit_cur->_b_gen_t = true;
}
// GEN_Q
FINLINE	void	disable_texture_gen_q_direct()
{
	GOL::disable( GL_TEXTURE_GEN_Q );
	tex_unit_cur->_b_gen_q = false;
}
FINLINE	void	disable_texture_gen_q()
{
	if( is_state_cache_no() || tex_unit_cur->_b_gen_q )
		disable_texture_gen_q_direct();
}
FINLINE	void	enable_texture_gen_q()
{
	if( !is_state_cache_no() && tex_unit_cur->_b_gen_q )
		return;
	GOL::enable( GL_TEXTURE_GEN_Q );
	tex_unit_cur->_b_gen_q = true;
}
// GEN_R
FINLINE	void	disable_texture_gen_r_direct()
{
	GOL::disable( GL_TEXTURE_GEN_R );
	tex_unit_cur->_b_gen_r = false;
}
FINLINE	void	disable_texture_gen_r()
{
	if( is_state_cache_no() || tex_unit_cur->_b_gen_r )
		disable_texture_gen_r_direct();
}
FINLINE	void	enable_texture_gen_r()
{
	if( !is_state_cache_no() && tex_unit_cur->_b_gen_r )
		return;
	GOL::enable( GL_TEXTURE_GEN_R );
	tex_unit_cur->_b_gen_r = true;
}

FINLINE	void	set_tex_env_mode( GLint CONST mode )
{
	if( is_state_cache_no() || tex_unit_cur->_env_mode != mode )
	{
		glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, mode );
		tex_unit_cur->_env_mode = mode;
	}
}
FINLINE	GLint	get_tex_env_mode()	{	return tex_unit_cur->_env_mode;	}

extern void set_tex_unit_private( INT32 CONST index );
FINLINE	void	set_tex_unit( INT32 CONST index )
{
	if( is_state_cache_no() || !tex_unit_cur->is_index(index) )
		set_tex_unit_private( index );
}

//	having an opengl texture complete was a requirement for some driver
extern	void	make_tex_complete_2d( INT32 CONST name_gl = 0 );


FINLINE	c_tex_unit*			get_tex_unit_cur()						{	return tex_unit_cur;				}
//	FINLINE	INT32				get_tex_unit_index()					{	return get_tex_unit_cur()->_index;	}
FINLINE	bool				is_tex_unit_index( INT32 CONST index )	{	return get_tex_unit_cur()->is_index( index );	}
FINLINE	c_tex_unit CONST &	get_tex_unit( INT32 CONST index )		{	return tex_units[index];			}

//FINLINE	void	set_tex_unit_2d( INT32 CONST tu_index )
//{
//	if( tu_index < tex_unit_nb )
//	{
//		c_tex_unit* tu = tex_units + tu_index;
//		if( tu->is_dim( 2 ) )
//			return;
//		set_tex_unit( tu_index );
//		set_texture_2D_private();
//	}
//}
FINLINE	void	set_tex_unit_dim_2( INT32 CONST tu_index )
{
	if( tu_index < tex_unit_nb )
	{
		c_tex_unit* unit = tex_units + tu_index;
		if( b_tex_unit_dim_use_ui )
		{
			if( is_state_cache_no() )
			{
				set_tex_unit( tu_index );
				GOL::disable( GL_TEXTURE_1D );
				GOL::disable( GL_TEXTURE_3D );
				GOL::enable( GL_TEXTURE_2D );
			}
			else
			{
				auto dim = unit->get_dim();
				if( dim != 2 )
				{
					set_tex_unit( tu_index );
					switch( dim )
					{
					case 0:	break;
					case 1:	GOL::disable( GL_TEXTURE_1D );	break;
					case 3:	GOL::disable( GL_TEXTURE_3D );	break;
					}
					GOL::enable( GL_TEXTURE_2D );
				}
			}
		}
		unit->set_dim( 2 );
		//was	set_texture_2D_private();
	}
}
FINLINE	void	disable_tex_unit( INT32 CONST tu_index )
{
	if( tu_index < tex_unit_nb )
	{
		c_tex_unit* unit = tex_units + tu_index;
		if( b_tex_unit_dim_use_ui )
		{
			if( is_state_cache_no() )
			{
				set_tex_unit( tu_index );
				GOL::disable( GL_TEXTURE_1D );
				GOL::disable( GL_TEXTURE_2D );
				GOL::disable( GL_TEXTURE_3D );
			}
			else
			{
				auto dim = unit->get_dim();
				if( dim != 0 )
				{
					set_tex_unit( tu_index );
					switch( dim )
					{
					case 1:	GOL::disable( GL_TEXTURE_1D );	break;
					case 2:	GOL::disable( GL_TEXTURE_2D );	break;
					case 3:	GOL::disable( GL_TEXTURE_3D );	break;
					}
				}
			}
		}
		unit->set_dim( 0 );
		//was	set_texture_0D_private();
	}
}
FINLINE	void	disable_tex_unit_from( INT32 CONST start = 0 )
{
	for( INT32 i=tex_unit_nb-1; i>=start; --i )
		disable_tex_unit( i );
	set_tex_unit( 0 );
}
FINLINE	void	disable_tex_unit_from_and_force_update_later( INT32 CONST start = 0 )
{
	for( INT32 i=tex_unit_nb-1; i>=start; --i )
	{
		set_tex_unit( i );
		disable_texture_and_force_update_later();
	}
	if( start != 0 )
		set_tex_unit( 0 );
}
//FINLINE void	invalidate_tex_unit()
//{
//	auto ptu = get_tex_unit_cur();
//	ptu->set_bind_2d(-2);
//	ptu->set_bind_3d(-2);
//}
//TEXCOOR
extern	void	texcoord_set_0();
extern	bool	texcoord_update();	// return true if any UV is needed (implicit)

FINLINE	void	texgen_i(	GLenum CONST coord, GLenum CONST pname, GLint CONST param )
{
	glTexGeni( coord, pname, param );
}

FINLINE	void	texgen_fv(	GLenum CONST coord, GLenum CONST pname, CONST GLfloat* params )
{
	glTexGenfv( coord, pname, params );
}

//TEX TRANSFER
FINLINE void	raster_pos_2f( GLfloat x, GLfloat y )	{ glRasterPos2f( x,y ); }
FINLINE void	raster_pos_2i( GLint x,   GLint y   )	{ glRasterPos2i( x,y ); }

extern	bool	b_copy_image_sub_can;	
extern	bool	b_copy_image_sub_allow_ui;
extern	bool	b_copy_image_sub_do;

extern	void	set_read_buffer(			GLenum CONST which );
//don't exist anymore caching read buffer have to use fbo too 
//	FINLINE	GLenum	get_read_buffer()																{	return read_buffer_cur;	}

extern	void	read_pixels(				GLint CONST x,		GLint CONST y,					GLsizei CONST width,
																								GLsizei CONST height,	GLenum CONST format, GLenum CONST type, void* CONST pixels );
extern	void	copy_image_sub_data(	GLuint CONST src_name, GLenum CONST src_target, GLint CONST src_level, GLint CONST src_x, GLint CONST src_y, GLint CONST src_z,
 										GLuint CONST dst_name, GLenum CONST dst_target, GLint CONST dst_level, GLint CONST dst_x, GLint CONST dst_y, GLint CONST dst_z,
 										GLsizei CONST src_width, GLsizei CONST src_height, GLsizei CONST src_depth );
FINLINE void	bitmap( GLsizei CONST width, GLsizei CONST height, GLfloat CONST xorig, GLfloat CONST yorig, 
						GLfloat CONST xmove, GLfloat CONST ymove, const GLubyte * CONST bitmap )
{
	glBitmap( width,height, xorig,yorig, xmove,ymove, bitmap );
}
FINLINE void	draw_pixels( GLsizei CONST width, GLsizei CONST height, GLenum CONST format, GLenum CONST type, const void * CONST pixels )
{
	glDrawPixels( width,height, format, type, pixels );
}
//	1d
extern	void	tex_image_1d(				GLint CONST level,	GLint CONST internalformat,		GLsizei CONST width,	GLenum CONST format, GLenum CONST type, void CONST * CONST pixels = nullptr );	
extern	void	tex_sub_image_1d(			GLint CONST level,	GLint CONST xoffset,			GLsizei CONST width,	GLenum CONST format, GLenum CONST type, void CONST * CONST pixels );
//	2d
extern	void	tex_image_2d(				GLint CONST level,	GLint CONST internalformat,		GLsizei CONST width,
																								GLsizei CONST height,	GLenum CONST format, GLenum CONST type, void CONST * CONST pixels = nullptr );
extern	void	tex_sub_image_2d(			GLint CONST level,	GLint CONST xoffset,
																GLint CONST yoffset,			GLsizei CONST width,
																								GLsizei CONST height,	GLenum CONST format, GLenum CONST type, void CONST * CONST pixels );
extern	void	tex_image_compressed_2d(	GLint CONST level,	GLenum CONST internalformat,	GLsizei CONST width,
																								GLsizei CONST height,	GLsizei CONST imageSize,				void CONST * CONST data );

extern	void	get_tex_image_2d(			GLint CONST level,															GLenum CONST format, GLenum CONST type, void* CONST pixels );
extern	void	get_tex_image_compressed_2d( GLint CONST level, void * CONST pixels );

extern	void	tex_copy_image_2d(			GLint CONST level, GLint CONST internalFormat,
																GLint CONST x,
																GLint CONST y,
																								GLsizei CONST width,
																								GLsizei CONST height,
																								GLsizei CONST border	);
extern	void	tex_copy_sub_image_2d(		GLint CONST level,	GLint CONST xoffset,
																GLint CONST yoffset,
																GLint CONST x,
																GLsizei CONST y,
																								GLsizei CONST width,
																								GLsizei CONST height		);
FINLINE	bool	is_copy_image_sub_data_2d()		{	return b_copy_image_sub_do;	}
extern	void	copy_image_sub_data_2d(	GLuint CONST src_name, GLint CONST src_level, GLint CONST src_x, GLint CONST src_y,
 										GLuint CONST dst_name, GLint CONST dst_level, GLint CONST dst_x, GLint CONST dst_y,
 										GLsizei CONST src_width, GLsizei CONST src_height );
extern	void	copy_image_sub_data_2d(	GLuint CONST src_name, GLint CONST src_level,
 										GLuint CONST dst_name, GLint CONST dst_level,
 										GLsizei CONST src_width, GLsizei CONST src_height );
//	3d		
extern	void	tex_image_3d(				GLint CONST level,	GLint CONST internalFormat,		GLsizei CONST width,
																								GLsizei CONST height,
																								GLsizei CONST depth,
																														GLenum CONST format, GLenum CONST type, void CONST * CONST pixels );
extern	void	tex_sub_image_3d(			GLint CONST level,	GLint CONST xoffset,
																GLint CONST yoffset,
																GLint CONST zoffset,			GLsizei CONST width,
																								GLsizei CONST height,
																								GLsizei CONST depth,	GLenum CONST format, GLenum CONST type, void CONST * CONST pixels );

extern	void	get_tex_image_3d(			GLint CONST level,															GLenum CONST format, GLenum CONST type, void* CONST pixels );
extern	void	get_tex_image_compressed_3d( GLint CONST level, void * CONST pixels );

}	//namespace GOL
