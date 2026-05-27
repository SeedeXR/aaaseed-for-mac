
#include "tex.h"
#include "spy.h"
#include "err.h"
#include "aaa_mem.h"

//--------------------------------------------------------------------------------------------------------
bool	tex::b_sub_mipmap_do			;
INT64	tex::i64_size_total				= 0;
bool	tex::b_compression_verbose_ui	= false;
DOUBLE	tex::tex_mem_total_mb			= .0;

class c_tex1d tex1d;
class c_tex2d tex2d;
class c_tex3d tex3d;

template< class tex_info, type_fn_min_mag FN_MIN_MAG >
void	c_texnd_base< tex_info, FN_MIN_MAG >::begin()
{
	set_min_mag( _s_minification_ui,  _s_magnification_ui );
}


template< class tex_info, type_fn_min_mag FN_MIN_MAG >
void	c_texnd_base< tex_info, FN_MIN_MAG >::alloc( UINT32 CONST bind_max_nb )
{
	bind_reset();
	INT32 nb = bind_max_nb + 1;
	_infos = new tex_info[ nb ];
	// we ask the driver all the tex ids in one call
	GLuint* names = (GLuint*)MALLOC( sizeof(GLuint) * nb );
	GOL::gen_textures( nb, names );	
	for( INT32 i=0; i<nb; ++i )
		_infos[i].set_name_gl( names[i] );
	FREE( names );
	_nb = bind_max_nb;
}


template< class tex_info, type_fn_min_mag FN_MIN_MAG >
FINLINE void c_texnd_base< tex_info, FN_MIN_MAG >::adjust_base( bool CONST b_mipmap_use )
{
	if( _s_force_filtering >= 0 )
		set_min_mag( _s_force_filtering, _s_force_filtering );
	else if( b_mipmap_use )
		set_min_mag( _s_minification_ui, _s_magnification_ui );
	else
		set_min_mag( 1, 1 );	// GL_LINEAR, GL_LINEAR
}


void	tex::alloc( UINT32 bind_1d_max_nb, UINT32 bind_2d_max_nb, UINT32 bind_3d_max_nb )
{
	tex1d.alloc( bind_1d_max_nb );
	tex2d.alloc( bind_2d_max_nb );
	tex3d.alloc( bind_3d_max_nb );
}
void	tex::dealloc()
{
	tex1d.dealloc();
	tex2d.dealloc();
	tex3d.dealloc();
}

void	tex::begin()
{
	tex1d.begin();
	tex2d.begin();
	tex3d.begin();
}
void	tex::update()
{
	tex1d.update();
	tex2d.update();
	tex3d.update();
}

bool	c_tex1d::generate_mipmap( bool b_mipmap_use )
{
	b_mipmap_use &= is_mipmap_generate();
	if( b_mipmap_use )
	{
		GOL::generate_mipmap_1d_direct();
		if( GOL::check_error() != 0 )
		{
			ERR_PRINT_STRING( "after GOL::generate_mipmap_1d_direct()" );
			ERR_PRINT_STRING( "Try to allow texture_hardware_generate_mipmap and restart AAASeed." );
		}
	}
	return b_mipmap_use;
}
void	c_tex1d::adjust_filtering( bool b_mipmap_use )
{
	if( GOL::is_anisotropic_filtering() )
		GOL::enable_anisotropic_filtering_1d();	//todo refine one day we do thw same test inside
	adjust_base( b_mipmap_use );

}
bool	c_tex2d::generate_mipmap( bool b_mipmap_use )
{
	b_mipmap_use &= is_mipmap_generate();
	if( b_mipmap_use )
	{
		GOL::generate_mipmap_2d_direct();
		if( GOL::check_error() != 0 )
		{
			ERR_PRINT_STRING( "after GOL::generate_mipmap_2d_direct()" );
			ERR_PRINT_STRING( "Try to allow texture_hardware_generate_mipmap and restart AAASeed." );
		}
	}	
	return b_mipmap_use;
}
//todo infact we adjust filtering too other and also disregarding what is asked in map and texturing we should store info in tex
//todo time to go to sampler...
void	c_tex2d::adjust_filtering( bool b_mipmap_use )
{
	if( GOL::is_anisotropic_filtering() )
		GOL::enable_anisotropic_filtering_2d();	//todo refine one day we do thw same test inside
	adjust_base( b_mipmap_use );
}

bool	c_tex3d::generate_mipmap( bool b_mipmap_use )
{
	b_mipmap_use &= is_mipmap_generate();
	if( b_mipmap_use )
	{
		GOL::generate_mipmap_3d_direct();
		if( GOL::check_error() != 0 )
		{
			ERR_PRINT_STRING( "after GOL::generate_mipmap_3d_direct()" );
			ERR_PRINT_STRING( "Try to allow texture_hardware_generate_mipmap and restart AAASeed." );
		}
	}
	return b_mipmap_use;
}
void	c_tex3d::adjust_filtering( bool b_mipmap_use )
{
	if( GOL::is_anisotropic_filtering() )
		GOL::enable_anisotropic_filtering_3d();	//todo refine one day we do thw same test inside
	adjust_base( b_mipmap_use );
}

FINLINE	bool c_tex2d::check( INT32 CONST sx, INT32 CONST sy, GLenum CONST internal_format )
{
	bool b_ret = true; 
	CONST c_tex_info_2d*	info = get_info();
	if( !info->is_size_x(sx) )	
	{
		debug_break( "%s() different width",			__FUNCTION__ );
		b_ret = false; 
	}
	if( !info->is_size_y(sy) )
	{
		debug_break( "%s() different height",			__FUNCTION__ );
		b_ret = false;
	}
	if( !info->is_internal_format(internal_format) )
	{
		debug_break( "%s() different internal format",	__FUNCTION__ );
		b_ret = false;
	}
	return b_ret;
}
FINLINE	bool c_tex3d::check( INT32 CONST sx, INT32 CONST sy, INT32 CONST sz, GLenum CONST internal_format )
{
	bool b_ret = true; 
	CONST c_tex_info_3d*	info = get_info();
	if( !info->is_size_x(sx) )	
	{
		debug_break( "%s() different width",			__FUNCTION__ );
		b_ret = false; 
	}
	if( !info->is_size_y(sy) )
	{
		debug_break( "%s() different height",			__FUNCTION__ );
		b_ret = false;
	}
	if( !info->is_size_z(sz) )
	{
		debug_break( "%s() different depth",			__FUNCTION__ );
		b_ret = false;
	}
	if( !info->is_internal_format(internal_format) )
	{
		debug_break( "%s() different internal format",	__FUNCTION__ );
		b_ret = false;
	}
	return b_ret;
}

void c_tex2d::bind(		INT32 CONST tex_id )
{
	INT32 i = make_index( tex_id );
	UINT32 name_gl = get_name_gl( i );
	if( !GOL::get_tex_unit_cur()->is_bind_2d(name_gl) )
	{
		set_index( i );
		GOL::bind_texture_2d( name_gl );
		if( GOL::b_tex_unit_verbose_ui )
			VERBOSE_PRINT_STRING( "Tex Unit (%d) 2d AAA bind %d\t gl_bind %d)", GOL::get_tex_unit_cur()->get_index(), tex_id, name_gl );
	}
}

void c_tex3d::bind(		INT32 CONST tex_id )
{	
	INT32 i = make_index( tex_id );
	UINT32 name_gl = get_name_gl( i );
	if( !GOL::get_tex_unit_cur()->is_bind_3d(name_gl) )
	{
		set_index( i );
		GOL::bind_texture_3d( name_gl );
		if( GOL::b_tex_unit_verbose_ui )
			VERBOSE_PRINT_STRING( "Tex Unit (%d) 3d AAA bind %d\t gl_bind %d)", GOL::get_tex_unit_cur()->get_index(), tex_id, name_gl );
	}
}

//should move to GOL ?
void c_tex2d::sub_image(	INT32 CONST level,
							INT32 CONST offset_x, INT32 CONST offset_y,		INT32 CONST sx, INT32 CONST sy,
							INT32 CONST channel_nb, GLenum CONST format, GLenum CONST type,	void CONST * CONST data,
							bool CONST b_mipmap_generate
						)
{
	if( sx<=0 || sy<=0 )
	{
#if	AAA_DEBUG()
		debug_break( "%s negative or null texture size %d x %d", __FUNCTION__, sx,sy );
#endif
		return;
	}

//	if( !check( sx, sy, get_internal_format( channel_nb, type ) ) {}
	TBUF_ADD( tbuf::CH_GL_TEX, 1., "tex::tex2d_sub_image" );
	SPY_PUSH_RANGE_FUNCTION( spy::TEX_LOW );
	
		//get_info()->ask_release_from_users( nullptr, get_index_2d() );	//todo perhaps we should refine this (the NULL)
		//GOL::set_swizzle_def( GL_TEXTURE_2D );
		GOL::check_error_debug( "before glTexSubImage2D" );
			GOL::tex_sub_image_2d( level, offset_x,offset_y, sx,sy, format, type, data );
		GOL::check_error_debug( "after glTexSubImage2D in tex2d_sub_image." );

		adjust_filtering( generate_mipmap( b_mipmap_generate ) );
		//we don't change channel_nb here so no reason to change swizzle except of realtime option are changed
		//	GOL::set_swizzle( GL_TEXTURE_2D, get_info()->get_channel_nb() );

	SPY_POP_RANGE();
	TBUF_ADD( tbuf::CH_GL_TEX, 0., "" );
}


//should move to GOL ?
void c_tex3d::sub_image(	INT32 CONST level,
							INT32 CONST offset_x, INT32 CONST offset_y,	INT32 CONST offset_z, INT32 CONST sx, INT32 CONST sy, INT32 CONST sz,
							INT32 CONST channel_nb, GLenum CONST format, GLenum type, void CONST * CONST  data,
							bool CONST b_mipmap_generate
						)
{
	if( sx<=0 || sy<=0 || sz<=0 )
	{
#if	AAA_DEBUG()
		debug_break( "%s negative or null texture size %d x %d x %d", __FUNCTION__, sx,sy,sz );
#endif
		return;
	}

//	if( !check_2d(sx, sy, get_internal_format( channel_nb, type ) )	{}
	TBUF_ADD( tbuf::CH_GL_TEX, 1., "tex::tex3d_sub_image" );
	SPY_PUSH_RANGE_FUNCTION( spy::TEX_LOW );

		//get_3d_info()->ask_release_from_users( nullptr, get_index_3d() );	//todo perhaps we should refine this (the NULL)
		GOL::check_error_debug( "before glTexSubImage3D" );
			GOL::tex_sub_image_3d( level, offset_x, offset_y, offset_z, sx,sy,sz, format, type, data );
		GOL::check_error_debug( "after glTexSubImage3D in tex3d_sub_image." );

		adjust_filtering( generate_mipmap( b_mipmap_generate ) );
		//we don't change channel_nb here so no reason to change swizzle except of realtime option are changed
		//GOL::set_swizzle( GL_TEXTURE_3D, get_info()->get_channel_nb() );

	SPY_POP_RANGE();
	TBUF_ADD( tbuf::CH_GL_TEX, 0., "" );
}

void c_tex2d::image_level_gl_internal_format(	INT32 CONST level,
												INT32 CONST sx, INT32 CONST sy,		GLenum internal_format, INT32 CONST channel_nb, GOL::INTERNAL_TYPE CONST internal_type,
												void CONST * data, GLenum CONST format, GLenum CONST type, 
												bool b_mipmap_generate, bool b_compressed )
{
	//	DBG_PRINT_STRING( "before image level", sx, sy, channel_nb, format, data, b_compressed, type  );

	if( sx<=0 || sy<=0 )
	{
#if	AAA_DEBUG()
		debug_break( "%s() negative or null texture size %d x %d", __FUNCTION__, sx, sy );
#endif
		return;
	}

	SPY_PUSH_RANGE_FUNCTION( spy::TEX_LOW );

		get_info()->ask_release_from_users( nullptr, get_index() );	//todo perhaps we should refine this (the NULL)

		b_compressed &= GOL::is_tex_compressed();
		if( b_compressed )
		{
			switch( channel_nb )
			{
			case 1 :	internal_format = GL_COMPRESSED_ALPHA;		break;
			case 2 :	internal_format = GL_COMPRESSED_RG;			break;
			case 3 :	internal_format = GL_COMPRESSED_RGB;		break;
			case 4 :	internal_format = GL_COMPRESSED_RGBA;		break;
			}
		}

		GOL::check_error_debug( "before glTexImage2D() in tex2d_image_level" );
		if( GOL::b_tex_verbose_ui )
		{
			GLenum internal_format_cur = GOL::get_2d_internal_format();	//we want the gol data not what we cache so : GOL::
			if( internal_format_cur != 0 )
			{
				DBG_PRINT_STRING( "glTexImage2D Before : bind %d\t\tinternal format\t%.64s",
										get_index(), GOL::get_format_str_from_glenum( internal_format_cur ) );
			}
		}

		//GOL::set_swizzle_def( GL_TEXTURE_2D );
			//	the NVidia drivers correct bad internal format but on AMD you have to be right
			//DBG_PRINT_STRING( "before GOL::tex_image_2d l%d: %d x %d x %d 0x%x/0x%x at 0x%x %d 0x%x", level, sx, sy, channel_nb, format, type, data, b_compressed, internal_format  );
			GOL::tex_image_2d( level, internal_format, sx,sy, format, type, data );
			//DBG_PRINT_STRING( "after GOL::tex_image_2d" );
	
		if( GOL::b_tex_verbose_ui )
		{
			DBG_PRINT_STRING( "glTexImage2D After : bind %d\t%d x %d\twith internal format\t%.64s", get_index(), sx, sy,
										GOL::get_2d_internal_format_as_str() );
		}
		GOL::check_error_debug( "after glTexImage2D() in tex2d_image_level" );

		b_mipmap_generate = generate_mipmap( b_mipmap_generate && data );	// Have to be false with a NULL pointer ( in fact crash on ati if true), now dealt at low level anyhow
		adjust_filtering( b_mipmap_generate );
		GOL::set_swizzle( GL_TEXTURE_2D, channel_nb );	//todo only on level 0 ?

		INT64	size = I_CEIL(sx * sy * channel_nb * ( b_mipmap_generate ? 1.333333 : 1.0 ));  //todo does GL_RGB takes 24 or 32 on video card ?
		size *= GOL::get_byte_nb_from_internal_type( internal_type );
		if( b_compressed )
		{
			//todo final status and 3d 1d too
			GLint	compressed;
			glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_ARB, &compressed );
			// if the compression has been successful
			if( compressed == GL_TRUE  )
			{
				GLint	compressed_size;
				GLint	compressed_internal_format;
				glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &compressed_internal_format );
				glGetTexLevelParameteriv( GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compressed_size );
				size = compressed_size;
				internal_format = compressed_internal_format;
				if( tex::b_compression_verbose_ui )
				{
					REAL	compression_ratio = (REAL)(sx * sy * channel_nb) / (REAL)compressed_size;
					GOOD_PRINT_STRING( "Texture Compression %s : size %d, ratio %.2f",
										GOL::get_compressed_format_str_from_glenum( compressed_internal_format ), 
										compressed_size, compression_ratio );
				}
			}
			else
			{
				DBG_PRINT_STRING( "Texture Compression : could not compress texture" );
			}
		}

		store( sx,sy, internal_format, internal_type, size );

		GOL::check_error_debug( "after glTexImage2D()" );

	SPY_POP_RANGE();
}

void c_tex2d::image_level(		INT32 CONST level,
								INT32 CONST sx, INT32 CONST sy,		INT32 CONST channel_nb, GOL::INTERNAL_TYPE CONST internal_type,
								void CONST * data, GLenum CONST format, GLenum CONST type, 
								bool b_mipmap_generate, bool b_compressed )
{
	//	DBG_PRINT_STRING( "before image level", sx, sy, channel_nb, format, data, b_compressed, type  );
	GLenum internal_format = GOL::make_internal_format_gl( channel_nb, internal_type );
	if( internal_format == GL_INVALID_VALUE )
	{
		debug_break( "Invalid internal format in %s()", __FUNCTION__ );
		return;
	}

	image_level_gl_internal_format( level,	sx,sy,		internal_format, channel_nb, internal_type, data, format, type, b_mipmap_generate, b_compressed );
}

void c_tex3d::image_level(	INT32 CONST level,
								INT32 CONST sx, INT32 CONST sy, INT32 CONST sz,
								INT32 CONST channel_nb, GOL::INTERNAL_TYPE CONST internal_type,
								GLenum CONST format, GLenum CONST type, void CONST * CONST data,
								bool b_mipmap_generate, bool b_compressed )
{
	if( sx<=0 || sy<=0 || sz<=0 )
	{
#if	AAA_DEBUG()
		debug_break( "%s negative or null texture size %d x %d x %d", __FUNCTION__, sx, sy, sz );
#endif
		return;
	}
	//opencl was a bug before ? INT32	internal_format = channel_nb;
	GLenum internal_format = GOL::make_internal_format_gl( channel_nb, internal_type );
	if( internal_format == GL_INVALID_VALUE )
	{
		debug_break( "Invalid internal format in %s()", __FUNCTION__ );
		return;
	}

	SPY_PUSH_RANGE_FUNCTION( spy::TEX_LOW );
		
		get_info()->ask_release_from_users( nullptr, get_index() );	//todo perhaps we should refine this (the NULL)

	/*
		b_compressed &= GOL::is_tex_compressed();
		if( b_compressed )
		{
			switch( channel_nb )
			{
			case 1 :	internal_format = GL_COMPRESSED_ALPHA;		break;
			case 2 :	internal_format = GL_COMPRESSED_RG;			break;
			case 3 :	internal_format = GL_COMPRESSED_RGB;		break;
			case 4 :	internal_format = GL_COMPRESSED_RGBA;		break;
			}
		}
	*/
		GOL::check_error_debug( "before glTexImage3D() in tex3d_image_level" );
			if( GOL::b_tex_verbose_ui )
			{
				GLenum internal_format = GOL::get_3d_internal_format();	//we want the gol data not what we cache so : GOL::
				if( internal_format != 0 )
				{
					DBG_PRINT_STRING( "glTexImage3D Before : bind %d\t\tinternal format\t%.64s",
										get_index(), GOL::get_format_str_from_glenum( internal_format ) );
				}
			}

			//	Nvidia driver correct bad internal format but on AMD you have to be right
			GOL::tex_image_3d( level, internal_format, sx, sy, sz, format, type, data );

			if( GOL::b_tex_verbose_ui )
			{
				DBG_PRINT_STRING( "glTexImage3D After : bind %d\t%d x %d x %d\twith internal format\t%.64s",
									get_index(), sx, sy, sz, GOL::get_3d_internal_format_as_str() );
			}
		GOL::check_error_debug( "after glTexImage3D() in tex3d_image_level" );

	//todo check/redo size or drop it generate mipmap should be refine for this
		b_mipmap_generate = generate_mipmap( b_mipmap_generate && data );
		adjust_filtering( b_mipmap_generate );
		GOL::set_swizzle( GL_TEXTURE_3D, channel_nb );

		INT64	size = I_CEIL(sx * sy * sz * channel_nb * ( b_mipmap_generate ? 1.333333 : 1.0 ));  //todo does GL_RGB takes 24 or 32 on video card ?
		size *= GOL::get_byte_nb_from_internal_type( internal_type );

		store( sx, sy, sz, internal_format, internal_type, size );

		GOL::check_error_debug( "after glTexImage3D()" );

	SPY_POP_RANGE();
}

#if 0 // pre gl3.0
void c_tex2d::build_mipmap(	INT32 sx, INT32 sy, INT32 channel_nb, GLenum format, GLenum type, CONST void* data )
{
	//hackhack ?
	//todoqqq for fp32 texture, mipmap is not supported on most hardware
	adjust( true );
	// obsolete starting in opengl 3.0
	if( !data )
		debug_break( "in %s() try to call gluBuild2DMipmaps with Null data, skiping", __FUNCTION__  );
	else
		gluBuild2DMipmaps( GL_TEXTURE_2D, channel_nb, sx, sy, format, type, data );

	INT32	size = sx * sy * channel_nb * 1.333333 ;  //todo does GL_RGB takes 24 or 32 on video card ?

	//todotex internal format and type are not the right one
	// even now get_2d_internal_format() return 0x4 ???
	store( sx, sy, GOL::get_2d_internal_format(), GOL::INTERNAL_TYPE::UINT_8, size );

	GOL::check_error_debug( "after gluBuild2DMipmaps()" );
}
#endif

void c_tex2d::image( INT32 CONST sx, INT32 CONST sy, INT32 CONST channel_nb,
						GLenum CONST format, void CONST * CONST data, bool CONST b_compressed, GLenum CONST type )
{
	if( sx<=0 || sy<=0 )
	{
#if	AAA_DEBUG()
		debug_break( "%s() negative or null texture size %d x %d", __FUNCTION__, sx, sy );
#endif
		return;
	}

	//	set_alpha_scale_bias();
	//DBG_PRINT_STRING( "c_tex2d::image %d x %d x %d 0x%x 0x%x %d 0x%x", sx, sy, channel_nb, format, data, b_compressed, type  );

#if 1
	bool CONST b_mipmap_generate = data && !is_force_nearest() && !is_force_linear();
	GOL::INTERNAL_TYPE	internal_type =  GOL::get_internal_type( type );

	//DBG_PRINT_STRING( "before image level 0x%x", internal_type  );
	image_level( 0, sx,sy, channel_nb, internal_type, data, format, type, b_mipmap_generate, b_compressed );
#else	// pre gl3.0
	bool b_mipmap_generate;
	if( is_force_nearest() || is_force_linear() || !data || !_b_mipmap_generate_allow_ui )
		b_mipmap_generate = false;
	else if( (GOL::is_tex_npot() || ( INT_IS_POW2(sx) && INT_IS_POW2(sy)) ) && GOL::is_mipmap_generate() )
		b_mipmap_generate = true;
	else
	{
		build_mipmap( sx, sy, channel_nb, format, type, data );
		return;
	}
	GOL::INTERNAL_TYPE	internal_type =  GOL::get_internal_type( type );

	//todotex	GLenum	internal_format = GOL::make_internal_format_gl( channel_nb, GOL::get_internal_type( type ) );
	//			GLenum internal_format = GOL::make_internal_format_gl( channel_nb, GOL::get_internal_type_def_asked(), type );
	//DBG_PRINT_STRING( "before image level 0x%x", internal_type  );
	image_level( 0, sx,sy, channel_nb, internal_type, data, format, type, b_mipmap_generate, b_compressed );
	//DBG_PRINT_STRING( "after image level" );

	//todo	change it then restore it
	//		GOL::reset_scale_bias();
#endif
}

void c_tex2d::image_level_compressed( INT32 CONST level, INT32 CONST sx, INT32 CONST sy, INT32 CONST channel_nb,
										GLenum CONST format, GLenum CONST type,
										INT32 CONST size, void CONST * CONST data )
{
	if( sx<=0 || sy<=0 )
	{ 
#if	AAA_DEBUG()
		debug_break( "%s negative or null texture size %d x %d", __FUNCTION__, sx, sy );
#endif
		return;
	}

	GOL::check_error_debug( "before glCompressedTexImage2D()" );
		GOL::tex_image_compressed_2d( level, format, sx, sy, size, data ); 
	GOL::check_error_debug( "after glCompressedTexImage2D()" );

//	b_mipmap_generate = adjust( b_mipmap_generate && data );

//	if( b_mipmap_generate )
//		size *= 1.333333;

	//todo internal format is not the right one
//	store( sx, sy, format, GOL::INTERNAL_TYPE::UINT_8, size );
//	GOL::set_swizzle( GL_TEXTURE_2D, channel_nb );
}

void c_tex2d::image_compressed( INT32 sx, INT32 sy, INT32 CONST channel_nb,
									GLenum CONST format, UINT64 CONST size, INT32 CONST mipmap_nb, void CONST * CONST data )
{
	//	set_alpha_scale_bias();

	bool b_mipmap_generate;
	if( mipmap_nb == 1 )
	{
#if 1
		b_mipmap_generate = data && !is_force_nearest() && !is_force_linear();
#else	// pre gl3.0
		if( is_force_nearest() || is_force_linear() || !data || !_b_mipmap_generate_compressed_ui )
			b_mipmap_generate = false;
		else if( ( GOL::is_tex_npot() || ( INT_IS_POW2( sx ) && INT_IS_POW2( sy ) ) ) && GOL::is_mipmap_generate() )
			b_mipmap_generate = true;
	//	else
	//	{
	//		build_mipmap( sx, sy, channel_nb, format, GL_UNSIGNED_BYTE, data );
	//		return;
	//	}
#endif
	}
	else
		b_mipmap_generate = false;
	INT32 blk_size = ( format == GL_COMPRESSED_RED_RGTC1 || format == GL_COMPRESSED_RGB_S3TC_DXT1_EXT  || format == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT ) ? 8 : 16;
	INT64 offset = 0;
	
	// load the mipmaps
	//refine generate mipmap only on the last here
	for( INT32 level = 0; level < mipmap_nb && ( sx || sy ); ++level )
	{
		if( sx == 0 ) sx = 1;
		if( sy == 0 ) sy = 1;
		INT64 size_mipmap = ( (sx+3)/4 ) * ( (sy+3)/4 ) * blk_size;
		image_level_compressed( level, sx, sy, channel_nb, format, GL_UNSIGNED_BYTE, static_cast<INT32>(size_mipmap), (unsigned char*)data + offset );	//, b_mipmap_generate );
		offset += size_mipmap;
		sx >>= 1;
		sy >>= 1;
	}

	//todo	change it then restore it
	//		GOL::reset_scale_bias();

	b_mipmap_generate = generate_mipmap( b_mipmap_generate );
	adjust_filtering( b_mipmap_generate );
	GOL::set_swizzle( GL_TEXTURE_2D, channel_nb );

	//todo internal format is not the right one
	store( sx, sy, format, GOL::INTERNAL_TYPE::UINT_8, b_mipmap_generate ? I_CEIL(size * 1.333333) : size );

}

void c_tex3d::image( INT32 CONST sx, INT32 CONST sy, INT32 CONST sz, INT32 CONST channel_nb,
							GLenum CONST format, void CONST * CONST data, bool CONST b_compressed, GLenum CONST type )
{
	//	set_alpha_scale_bias();
	//todotex
#if 1
	bool CONST b_mipmap_generate = data && !is_force_nearest() && !is_force_linear();
#else	// pre gl3.0
	bool b_mipmap_generate;
	if( is_force_nearest() || is_force_linear() || !data || !_b_mipmap_generate_allow_ui )
		b_mipmap_generate = false;
	else if( (GOL::is_tex_npot() || ( INT_IS_POW2(sx) && INT_IS_POW2(sy) && INT_IS_POW2(sz) ) ) && GOL::is_b_mipmap_generate() )
		b_mipmap_generate = true;
	else
	{
		debug_break( "%s() this case unimplemented", __FUNCTION__ );
		return;
	}
#endif

	GOL::INTERNAL_TYPE	internal_type =  GOL::get_internal_type( type );	//GOL::INTERNAL_TYPE::UINT_8;
	image_level( 0, sx, sy, sz, channel_nb, internal_type, format, type, data, b_mipmap_generate, b_compressed );

	//todo	change it then restore it
	//		GOL::reset_scale_bias();
}

//todotex go out of just the rgb/rgba logic
void	c_tex2d::read_pixels( void* CONST buf, GLenum CONST gl_format, GLenum CONST type, INT32 CONST x, INT32 CONST y, INT32 CONST sx, INT32 CONST sy, INT32 CONST which_buffer )
{
	if( !buf )
		return;
//#if	AAA_DEBUG()
	if( sx<=0 || sy<=0 )
	{
		debug_break( "%s negative or null texture size %d x %d", __FUNCTION__, sx, sy );
		return;
	}
//#endif

	//todo		color_base->pixel_transfert_update();
	TBUF_INC( tbuf::CH_GPU_READ, 1., "read_from_gpu" );

		if( which_buffer && which_buffer!=GL_DEPTH_ATTACHMENT && which_buffer!=GL_STENCIL_ATTACHMENT )
		{
			GOL::set_read_buffer( which_buffer );
			GOL::check_error_debug( "tex:read_pixels doing first glReadBuffer" );
		}

		GOL::read_pixels(	x,y,	sx,sy,		gl_format, type, buf );
		//  INT32   format = GOL::make_format_gl( channel_nb, texture_flux_master->is_move_from_gpu_bgr() );
		//GOL::get_tex_image_2d( 2, format, type, buf );

//		GOL::check_error_debug( "read_pixel" );

	TBUF_DEC( tbuf::CH_GPU_READ, 1., "read_from_gpu" );
	//todo		color_base->pixel_transfert_reset();
	//hack	remove it because it cause trouble when fbo use it
	//if( which_buffer )
	//{
	//	GOL::read_buffer( g_stereo->get_gl_buffer_used() );
	//	GOL::print_errors( "tex:read_pixels doing second glReadBuffer" );
	//}
}

//unused ?
/*
void	tex::get_tex_image( UINT8* buf, INT32 channel_nb )
{
	if( buf )
	{
		//todo		color_base->pixel_transfert_update();
		//	GOL::read_buffer( which_buffer );
		//	bind_2d( which_buffer );
		//GOL::bind_texture_2d( texID );
		tbuf_inc( c_tbuf_master::CH_GPU_READ, 1., "read_from_gpu" );
		glGetTexImage( GL_TEXTURE_2D, 0, ( channel_nb == 3 )?GL_RGB:GL_RGBA, get_internal_type_def(), buf );
		GOL::check_error_debug( "tex_image" );
		tbuf_dec( c_tbuf_master::CH_GPU_READ, 1., "read_from_gpu" );
		//todo		color_base->pixel_transfert_reset();
	}
}
*/

GLenum	c_tex2d::get_internal_format_compressed()
{
	// if the compression has been successful
	if( GOL::is_internal_format_compressed_2d() )
		return get_internal_format();
	return 0;
}

void	c_tex2d::get_compressed( UINT8* buf )
{
	if( buf )
	{
		// if the compression has been successful
		if( GOL::is_internal_format_compressed_2d() )
		{
			TBUF_INC( tbuf::CH_GPU_READ, 1., "read_from_gpu" );
				GOL::get_tex_image_compressed_2d( 0, buf );
			TBUF_DEC( tbuf::CH_GPU_READ, 1., "read_from_gpu" );
		}
	}
}

//kind of reference code
//AAA_ERR	c_img_2d::write_compressed()
//{
//	//glTexImage2D(GL_TEXTURE_2D, 0, GL_COMPRESSED_RGB_ARB, width, height,
//	//	0, GL_BGR_EXT, GL_UNSIGNED_BYTE, pixels);
//	//glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_ARB, &compressed);
//	///* if the compression has been successful */
//	//if(compressed == GL_TRUE)
//	//{
//	//	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_INTERNAL_FORMAT, &internalformat);
//	//	glGetTexLevelParameteriv(GL_TEXTURE_2D, 0, GL_TEXTURE_COMPRESSED_IMAGE_SIZE, &compressed_size);
//	//	img = (unsigned char *)MALLOC(compressed_size * sizeof(unsigned char));
//	//	glGetCompressedTexImageARB(GL_TEXTURE_2D, 0, img);
//	//	SaveTexture(width, height, compressed_size, img, internalFormat, 0);
//	//}
//	return AAA_OK;
//}

