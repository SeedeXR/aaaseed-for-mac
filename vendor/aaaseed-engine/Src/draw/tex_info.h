
#ifdef AAA_TEX_INFO_H
#error "TEX_INFO_H included more than once."
#endif
#define AAA_TEX_INFO_H 1


#ifndef AAA_AAA_UTIL_H
#	include		"aaa_util.h"
#endif
#ifndef _UNORDERED_MAP_
#	include		<unordered_map>
#endif
#ifndef AAA_GOL_TEX_H
#	include		"gol/gol_tex.h"
#endif
#ifndef AAA_OBJ_H
#	include		"infrastructure/obj/obj.h"
#endif

class c_obj_ui;

class c_tex_info	: public c_obj // can't use c_obj because of C_NO_CPY_MOVE
{
private:
	GLuint				_name_gl			;	//	Gl name used for bind
	INT32				_channel_nb			;	//to deal quickly with swizzle
	GLenum				_internal_format	;
	GOL::INTERNAL_TYPE	_internal_type		;
	INT64				_size_byte_nb		;
	bool				_b_free				;

	typedef				std::unordered_map< c_obj_ui*, INT32 >	user_map;
	user_map			_users;

	void				delete_name();
protected:

public:


	FINLINE	UINT32	get_name_gl()
	{
		if( _name_gl==0 )
			GOL::gen_texture( &_name_gl );
		return _name_gl;
	}
// we have to be sure we transfer ownership because we delete name in destructor
	void	set_name_gl( GLuint CONST gl_name );
	
	c_tex_info();
	virtual ~c_tex_info();

	FINLINE	INT32	get_channel_nb()	CONST									{ return _channel_nb; }


	FINLINE	bool	is_internal_format(		GLenum internal_format )	CONST	{ return internal_format == _internal_format; }
	FINLINE	GLenum	get_internal_format()								CONST	{ return _internal_format; }
			void	set_internal_format(	GLenum internal_format );

	FINLINE	void				set_internal_type( GOL::INTERNAL_TYPE type )	{ _internal_type = type; }
	FINLINE	GOL::INTERNAL_TYPE	get_internal_type()						CONST	{ return _internal_type; }

	FINLINE	void	set_size( INT64 size )										{ _size_byte_nb = size; }
	FINLINE	void	set_size_byte_nb( INT64 size_byte_nb )						{ _size_byte_nb = size_byte_nb; }
	FINLINE	INT64	get_size_byte_nb()									CONST	{ return _size_byte_nb; }

	FINLINE	bool	is_free()	CONST											{ return _b_free;		}
			void	set_free(	bool CONST b_free )								{ _b_free = b_free;		}

			void	add_user(				c_obj_ui* CONST user );
			void	remove_user(			c_obj_ui* CONST user );
			void	delete_user(			c_obj_ui* CONST user );
			void	ask_release_from_users(	c_obj_ui* CONST obj_ui, INT32 CONST bind );

	void swap_with( c_tex_info* pt )
	{
		SWAP( _name_gl,			pt->_name_gl );
		SWAP( _channel_nb,		pt->_channel_nb );
		SWAP( _internal_format, pt->_internal_format );
		SWAP( _internal_type,	pt->_internal_type );
		SWAP( _size_byte_nb,	pt->_size_byte_nb );

		//todola
		//SWAP( _users,			pt->_users );
	}
};

class c_tex_info_1d : public c_tex_info
{
private:
	GLenum				_wrap_s			{GL_NONE};
	INT32				_size_x			{0};
public:
//	c_tex_info_1d()		{}

	FINLINE	GLenum	get_wrap_s()										CONST						{ return _wrap_s; }
	FINLINE	void	set_wrap_s(					GLenum CONST wrap_s )								{ _wrap_s = wrap_s; }
	
	FINLINE	bool	is_size_x(					INT32 CONST sx )		CONST						{ return _size_x == sx; }
	FINLINE	INT32	get_size_x()										CONST						{ return _size_x; }
	FINLINE	void	set_size_x(					INT32 CONST sx )									{ _size_x = sx; }


	FINLINE	bool	is_size_x_internal_format(	INT32 CONST sx, GLenum CONST internal_format )	CONST
	{
		return is_size_x( sx ) && is_internal_format( internal_format );
	}

			void	store(						INT32 CONST sx, GLenum CONST internal_format, GOL::INTERNAL_TYPE internal_type,		 INT64 size_byte_nb );
	void swap_with( c_tex_info_1d* pt )
	{
		SWAP( _wrap_s, pt->_wrap_s );
		SWAP( _size_x, pt->_size_x );
		c_tex_info::swap_with( pt );
	}
};

class c_tex_info_2d : public c_tex_info_1d
{
private:
	GLenum				_wrap_t			{GL_NONE};
	INT32				_size_y			{0};
public:
//	c_tex_info_2d()		{}

	FINLINE	GLenum	get_wrap_t()											CONST					{ return _wrap_t; }
	FINLINE	void	set_wrap_t(						GLenum CONST wrap_t )							{ _wrap_t = wrap_t; }
	
	FINLINE	bool	is_size_y(						INT32 CONST sy )		CONST					{ return _size_y == sy; }
	FINLINE	INT32	get_size_y()											CONST					{ return _size_y; }
	FINLINE	void	set_size_y(						INT32 CONST sy )								{ _size_y = sy; }

	FINLINE	bool	is_size_xy(						INT32 CONST sx, INT32 CONST sy )	CONST		{ return is_size_x(sx) && is_size_y(sy); }
	FINLINE	void	get_size_xy(					INT32& sx, INT32& sy )				CONST		{ sx = get_size_x(); sy = get_size_y(); }
	FINLINE	void	set_size_xy(					INT32 CONST sx, INT32 CONST sy )				{ set_size_x(sx); set_size_y(sy); }

	FINLINE	bool	is_size_xy_internal_format(		INT32 CONST sx, INT32 CONST sy, GLenum CONST internal_format )	CONST
	{
		return is_size_xy( sx,sy ) && is_internal_format( internal_format );
	}

			void	store(							INT32 CONST sx, INT32 CONST sy, GLenum CONST internal_format,
													GOL::INTERNAL_TYPE CONST internal_type,
													INT64 CONST size_byte_nb );
	void swap_with( c_tex_info_2d* pt )
	{
		SWAP( _wrap_t, pt->_wrap_t );
		SWAP( _size_y, pt->_size_y );
		c_tex_info_1d::swap_with( pt );
	}
};

class c_tex_info_3d final : public c_tex_info_2d
{
private:
	GLenum				_wrap_r			{GL_NONE};
	INT32				_size_z			{0};
public:
//	c_tex_info_3d()		{}

	FINLINE	GLenum	get_wrap_r()											CONST					{ return _wrap_r; }
	FINLINE	void	set_wrap_r(						GLenum CONST wrap_r )							{ _wrap_r = wrap_r; }
	
	FINLINE	bool	is_size_z(						INT32 CONST sz )		CONST					{ return _size_z == sz; }
	FINLINE	INT32	get_size_z()											CONST					{ return _size_z; }
	FINLINE	void	set_size_z(						INT32 CONST sz )								{ _size_z = sz; }

	FINLINE	bool	is_size_xyz(					INT32 CONST sx, INT32 CONST sy, INT32 CONST sz )	CONST	{ return is_size_x(sx) && is_size_y(sy) && is_size_z(sz); }
	FINLINE	void	get_size_xyz(					INT32& sx, INT32& sy, INT32& sz )					CONST	{ sx = get_size_x(); sy = get_size_y(); sz = get_size_z(); }
	FINLINE	void	set_size_xyz(					INT32 CONST sx, INT32 CONST sy, INT32 CONST sz )			{ set_size_x(sx); set_size_y(sy); set_size_z(sz); }

	FINLINE	bool	is_size_xyz_internal_format(	INT32 CONST sx, INT32 CONST sy, INT32 CONST sz, GLenum CONST internal_format )	CONST
	{
		return is_size_xyz( sx,sy,sz ) && is_internal_format( internal_format );
	}

			void	store(							INT32 CONST sx, INT32 CONST sy, INT32 CONST sz, GLenum CONST internal_format,
													GOL::INTERNAL_TYPE CONST internal_type,
													INT64 CONST size_byte_nb );
	void swap_with( c_tex_info_3d* pt )
	{
		SWAP( _wrap_r, pt->_wrap_r );
		SWAP( _size_z, pt->_size_z );
		c_tex_info_2d::swap_with( pt );
	}
};

