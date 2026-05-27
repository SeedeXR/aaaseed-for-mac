
#ifdef AAA_TEX_H
#error "TEX_H included more than once."
#endif
#define AAA_TEX_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_TEX_INFO_H
#	include "tex_info.h"
#endif

namespace tex
{
//public:
	CONSTEXPR GLint gl_minmag_mode[6] =
	{
		GL_NEAREST,
		GL_LINEAR,
		GL_NEAREST_MIPMAP_NEAREST,
		GL_LINEAR_MIPMAP_NEAREST,
		GL_NEAREST_MIPMAP_LINEAR,
		GL_LINEAR_MIPMAP_LINEAR,
	};
	CONSTEXPR C_PCHAR_C minmag_mode_str[6] =
	{
		"NEAREST",
		"LINEAR",
		"NEAREST_MIPMAP_NEAREST",
		"LINEAR_MIPMAP_NEAREST",
		"NEAREST_MIPMAP_LINEAR",
		"LINEAR_MIPMAP_LINEAR"
	};


	extern	bool			b_compression_verbose_ui;

	extern	DOUBLE			tex_mem_total_mb;
	extern	INT64			i64_size_total;

/*	static	FINLINE void	bind_2d_delete( INT32 tex_id )
	{
		if( tex_id >= 0 && tex_id < nb_2d )
		{
			GOL::delete_textures( &name_2d[tex_id] );
		}
	}
*/

//public:
	extern	bool	b_sub_mipmap_do;
	FINLINE	bool	is_sub_mipmap_do()				{	return b_sub_mipmap_do; }

	extern	void	alloc(				UINT32 bind_1d_max_nb, UINT32 bind_2d_max_nb, UINT32 bind_3d_max_nb );
	extern	void	dealloc();
	extern	void	begin();
	extern	void	update();

	FINLINE	void	add_tex_size( INT64 CONST byte_nb )
			{
				i64_size_total += byte_nb;
				tex_mem_total_mb = DOUBLE(i64_size_total) / DOUBLE(1024*1024); //	10 for K, 10 for M
			}
};

typedef void(*type_fn_min_mag) ( GLenum CONST, GLenum CONST );
template< class tex_info, type_fn_min_mag FN_MIN_MAG >
class c_texnd_base : public c_obj
{
private:
	tex_info*	_infos								{nullptr};
	INT32		_index								{-3};
	INT32		_nb									{0};
	INT32		_s_force_filtering					{-1};
	bool		_b_force_nearest					{false};
	bool		_b_force_linear						{false};
	bool		_b_mipmap_generate_do				{false};

public:
	bool		_b_mipmap_generate_allow_ui			{false};
	bool		_b_mipmap_generate_compressed_ui	{false};
	INT32		_s_minification_ui					{1};
	INT32		_s_magnification_ui					{1};

//			c_texnd_base();
//	virtual ~c_texnd_base();

	FINLINE void		set_min_mag( INT32 CONST index_min, INT32 CONST index_mag )				
			{
				FN_MIN_MAG( tex::gl_minmag_mode[index_min], tex::gl_minmag_mode[index_mag] );
			}
			void		begin();

			void		alloc( UINT32 CONST bind_max_nb );
			void		dealloc()
			{
				SAFE_DELETE_ARRAY( _infos );
				_nb = 0;
			}
			
	FINLINE	bool		is_mipmap_generate() CONST					{	return _b_mipmap_generate_do;	}
				
	FINLINE	bool		is_force_nearest() CONST					{	return _b_force_nearest;		}
	FINLINE	bool*		get_force_nearest_pt()						{	return &_b_force_nearest;		}
	FINLINE	void		set_force_nearest( bool CONST b )			{	_b_force_nearest = b;			}

	FINLINE	bool		is_force_linear() CONST						{	return _b_force_linear;			}
	FINLINE	bool*		get_force_linear_pt()						{	return &_b_force_linear;		}
	FINLINE	void		set_force_linear( bool CONST b )			{	_b_force_linear = b;			}

	FINLINE	INT32		make_index(	INT32 CONST tex_id ) CONST		{	return ( 0 <= tex_id && tex_id < _nb ) ? tex_id : _nb ;	}	//	we could use 0 but Maa tried something else
	FINLINE	void		set_index(	INT32 CONST in )				{	_index = in;					}
	FINLINE	void		bind_reset()								{	_index = -3;					}

	FINLINE	INT32		get_index()	CONST							{	return	_index;					}

protected:
	FINLINE	tex_info* CONST	get_info(	INT32 CONST tex_id ) CONST	{	return	&_infos[make_index(tex_id)];	}
	FINLINE	tex_info*		get_info(	INT32 CONST tex_id )		{	return	&_infos[make_index(tex_id)];	}
	FINLINE	tex_info* CONST	get_info()						 CONST	{	return	get_info(get_index());			}
	FINLINE	tex_info*		get_info()								{	return	get_info(get_index());			}

public:
	FINLINE	UINT32		get_name_gl(	INT32 CONST tex_id ) CONST	{	return	get_info(tex_id)->get_name_gl();	}
protected:

	FINLINE	UINT32		bind_base(		INT32 CONST tex_id )
			{
				INT32 i = make_index( tex_id );
				set_index( i );
				return  _infos[i].get_name_gl();
			}
	FINLINE void		adjust_base( bool CONST b_mipmap_use );
public:
			void swap(	INT32 ia, INT32 ib )
			{
				get_info(ia)->swap_with( get_info(ib) );	//todo this have to be checked
			}
			void update()
			{
				_b_mipmap_generate_do = _b_mipmap_generate_allow_ui && GOL::is_mipmap_generate();
				if( is_force_linear() )	// we want linear have priority on nearest
					_s_force_filtering = 1;
				else if( is_force_nearest() )
					_s_force_filtering = 0;
				else
					_s_force_filtering = -1;
			}



	FINLINE	GOL::INTERNAL_TYPE	get_internal_type() CONST						{	return get_info()->get_internal_type();				}
	FINLINE	GOL::INTERNAL_TYPE	get_internal_type( INT32 CONST tex_id ) CONST	{	return get_info( tex_id )->get_internal_type();		}
	FINLINE	GLenum				get_internal_format() CONST						{	return get_info()->get_internal_format();			}
	FINLINE	GLenum				get_internal_format( INT32 CONST tex_id ) CONST	{	return get_info( tex_id )->get_internal_format();	}

			INT32 acquire()
			{
				for( INT32 i = _nb-1; i>=0; i-- )
				{
					tex_info* CONST info = &_infos[i];
					if( info->is_free() )
					{
						info->set_free( false );
						return i;
					}
				}
				return -1;
			}
			bool release( INT32 CONST tex_id )
			{
				if( 0 <= tex_id && tex_id < _nb )
				{
					tex_info* CONST info = &_infos[tex_id];
					if( !info->is_free() )
					{
						info->set_free( true );
						return true;
					}
				}
				return false;
			}
};

//	1D
class c_tex1d final : public c_texnd_base< c_tex_info_1d, GOL::set_tex_1d_min_mag >
{
public:
	FINLINE	void	bind(		INT32 CONST tex_id )		{	GOL::bind_texture_1d( bind_base( tex_id ) ); }
			bool	generate_mipmap(	bool b_mipmap_use );
			void	adjust_filtering(	bool b_mipmap_use );

	FINLINE void	set_wrap(	GLenum CONST wrap_s )
			{
				auto p = get_info();
				//if( p->get_wrap_s() != wrap_s )
				{
					GOL::set_tex_1d_wrap_s( wrap_s );
				//	p->set_wrap_s( wrap_s );
				}
			}
};
extern class c_tex1d tex1d;

//	2D
class c_tex2d final : public c_texnd_base< c_tex_info_2d, GOL::set_tex_2d_min_mag >
{
#if 0 // pre gl3.0
private:
	void	build_mipmap(			INT32 sx, INT32 sy,		INT32 channel_nb,
									GLenum format, GLenum type, CONST void* data
	);
#endif
//	INT32			tex_store			{0};
public:
			void	bind(				INT32 CONST tex_id );
			//void	swap(				INT32 ia, INT32 ib );
			bool	generate_mipmap(	bool b_mipmap_use );
			void	adjust_filtering(	bool b_mipmap_use );
	FINLINE void	set_wrap(	GLenum CONST wrap_s, GLenum CONST wrap_t )
			{
				auto p = get_info();
				//if( p->get_wrap_s() != wrap_s )
				{
					GOL::set_tex_2d_wrap_s( wrap_s );
				//	p->set_wrap_s( wrap_s );
				}
				//if( p->get_wrap_t() != wrap_t )
				{
					GOL::set_tex_2d_wrap_t( wrap_t );
				//	p->set_wrap_t( wrap_t );
				}
			}

	FINLINE	INT32	get_channel_nb(INT32 CONST tex_id) CONST	{	return get_info(tex_id)->get_channel_nb();	}
	FINLINE	bool	is_size_internal_format(	INT32 CONST sx, INT32 CONST sy, GLenum CONST internal_format )
			{
				return get_info()->is_size_xy_internal_format( sx, sy, internal_format );
			}
			GLenum	get_internal_format_compressed();
			void	get_compressed( UINT8* buf );

	FINLINE	void	add_user(									c_obj_ui* CONST obj )		{ get_info()->add_user( obj );			}
	FINLINE	void	remove_user(								c_obj_ui* CONST obj )		{ get_info()->remove_user( obj );		}

	FINLINE	void	add_user(				INT32 CONST tex_id,	c_obj_ui* CONST obj )		{ get_info(tex_id)->add_user( obj );	}
	FINLINE	void	remove_user(			INT32 CONST tex_id,	c_obj_ui* CONST obj )		{ get_info(tex_id)->remove_user( obj );	}
	FINLINE	void	delete_user(			INT32 CONST tex_id,	c_obj_ui* CONST obj )		{ get_info(tex_id)->delete_user( obj );	}

	FINLINE	void	ask_release_from_users(	INT32 CONST tex_id,	c_obj_ui* obj )				{ get_info(tex_id)->ask_release_from_users( obj, tex_id );	}

	FINLINE	void	get_size_cur(								INT32& sx, INT32& sy )		{ get_info()->get_size_xy( sx,sy );			}
	FINLINE	void	get_size(				INT32 CONST tex_id,	INT32& sx, INT32& sy )		{ get_info(tex_id)->get_size_xy( sx,sy );	}
//	FINLINE	void	get_size(				INT32 CONST tex_id,	FP32& sx,  FP32& sy )		{ get_info(tex_id)->get_size_xy( sx,sy );	}

	FINLINE	INT64	get_size_byte()															{ return get_info()->get_size_byte_nb();	}
	FINLINE	bool	is_size(							INT32 CONST sx, INT32 CONST sy )	{ return get_info()->is_size_xy( sx,sy );	}

	//todo	keepit ? more stack level ?
//	FINLINE	void	push()	{ tex_store = get_index(); }
//	FINLINE	void	pop()	{ bind(tex_store); }

	FINLINE	bool	check(		INT32 CONST sx, INT32 CONST sy,	GLenum CONST internal_format );
	//todo move size stuff in info function, deal when 0 is passed
	FINLINE void	store_at(	INT32 CONST tex_id,
								INT32 CONST sx, INT32 CONST sy, GLenum CONST internal_format, CONST GOL::INTERNAL_TYPE internal_type, CONST INT64 size_byte_nb )
			{
				c_tex_info_2d*	info = get_info( tex_id );
				CONST INT64 size = info->get_size_byte_nb();
				info->store( sx, sy, internal_format, internal_type, size_byte_nb );
				tex::add_tex_size( size_byte_nb - size );
			}
	FINLINE void	store(	INT32 CONST sx, INT32 CONST sy, GLenum CONST internal_format, CONST GOL::INTERNAL_TYPE internal_type, CONST INT64 size_byte_nb )
			{
				c_tex_info_2d*	info = get_info();
				CONST INT64 size = info->get_size_byte_nb();
				info->store( sx,sy, internal_format, internal_type, size_byte_nb );
				tex::add_tex_size( size_byte_nb - size );
			}
			void	image_level_gl_internal_format(	INT32 CONST level, INT32 CONST sx, INT32 CONST sy,
													GLenum internal_format, INT32 CONST channel_nb, GOL::INTERNAL_TYPE CONST internal_type,
													void CONST * data, GLenum CONST format, GLenum CONST type, 
													bool b_mipmap_generate, bool b_compressed
													);
			void	image_level(					INT32 CONST level, INT32 CONST sx, INT32 CONST sy,
													                        INT32 CONST channel_nb, GOL::INTERNAL_TYPE CONST internal_type,
													void CONST * CONST data = nullptr, GLenum CONST format = GL_RED, GLenum CONST type = GL_UNSIGNED_BYTE, // format type of data are unused when no data is passed
													bool b_mipmap_generate = false, bool CONST b_compressed = false
													);
			void	sub_image(						INT32 CONST level, INT32 CONST offset_x, INT32 CONST offset_y, INT32 CONST sx, INT32 CONST sy,		INT32 CONST channel_nb,
													GLenum CONST format, GLenum CONST type, void CONST * CONST data,
													bool CONST b_mipmap_generate
													);
			void	image(							INT32 CONST sx, INT32 CONST sy,
													INT32 CONST channel_nb, GLenum CONST format, void CONST * CONST data,
													bool CONST b_compressed, GLenum CONST type = GL_UNSIGNED_BYTE
													);
private:
			void	image_level_compressed(	INT32 CONST level,
											INT32 CONST sx, INT32 CONST sy,		INT32 CONST channel_nb,
											GLenum CONST format, GLenum CONST type, INT32 CONST size, void CONST * CONST data
									//,		bool b_mipmap_generate, bool b_do_adjust
									);
public:
			void	image_compressed(		INT32 sx, INT32 sy,		INT32 CONST channel_nb,
											GLenum CONST format, UINT64 CONST size, INT32 CONST mipmap_nb, void CONST * CONST data
									);
			void	read_pixels(			void* buf, GLenum CONST gl_format, GLenum CONST type,
											INT32 CONST x, INT32 CONST y, INT32 CONST sx, INT32 CONST sy,
											INT32 CONST which_buffer
									);
};
extern class c_tex2d tex2d;

//	3D
class c_tex3d final : public c_texnd_base< c_tex_info_3d, GOL::set_tex_3d_min_mag >
{
public:
			void	bind(				INT32 CONST tex_id );
			//void	swap(				INT32 ia, INT32 ib );
			bool	generate_mipmap(	bool b_mipmap_use );
			void	adjust_filtering(	bool b_mipmap_use );

	FINLINE void	set_wrap(	GLenum CONST wrap_s, GLenum CONST wrap_t, GLenum CONST wrap_r )
			{
				auto p = get_info();
				//if( p->get_wrap_s() != wrap_s )
				{
					GOL::set_tex_3d_wrap_s( wrap_s );
				//	p->set_wrap_s( wrap_s );
				}
				//if( p->get_wrap_t() != wrap_t )
				{
					GOL::set_tex_3d_wrap_t( wrap_t );
				//	p->set_wrap_t( wrap_t );
				}
				//if( p->get_wrap_r() != wrap_r )
				{
					GOL::set_tex_3d_wrap_r( wrap_r );
				//	p->set_wrap_r( wrap_r );
				}
			}
	FINLINE	bool	is_size_internal_format(	INT32 CONST sx, INT32 CONST sy, INT32 CONST sz, GLenum CONST internal_format )
			{
				return get_info()->is_size_xyz_internal_format( sx, sy, sz, internal_format );
			}
	FINLINE	bool	check(	INT32 CONST sx, INT32 CONST sy, INT32 CONST sz,	GLenum CONST internal_format );
			void	store(	INT32 CONST sx, INT32 CONST sy, INT32 CONST sz,	GLenum CONST internal_format, CONST GOL::INTERNAL_TYPE internal_type, CONST INT64 size_byte_nb )
			{
				c_tex_info_3d*	info = get_info();
				CONST INT64 size = info->get_size_byte_nb();
				info->store( sx, sy, sz, internal_format, internal_type, size_byte_nb );
				tex::add_tex_size( size_byte_nb - size );
			}
			void	image_level(		INT32 CONST level,
										INT32 CONST sx, INT32 CONST sy, INT32 CONST sz,
										INT32 CONST channel_nb, GOL::INTERNAL_TYPE CONST internal_type,
										GLenum CONST format, GLenum CONST type, void CONST * CONST data = nullptr,
										bool b_mipmap_generate = false, bool CONST b_compressed = false
								);
			void	sub_image(			INT32 CONST level,
										INT32 CONST offset_x, INT32 CONST offset_y, INT32 CONST offset_z,
										INT32 CONST sx, INT32 CONST sy, INT32 CONST sz,
										INT32 CONST channel_nb,
										GLenum CONST format, GLenum CONST type, void CONST * CONST data,
										bool CONST b_mipmap_generate
								);
			void	image(				INT32 CONST sx, INT32 CONST sy, INT32 CONST sz,
										INT32 CONST channel_nb,
										GLenum CONST format, void CONST * CONST data,
										bool b_compressed, GLenum CONST type = GL_UNSIGNED_BYTE
								);
};
extern class c_tex3d tex3d;

