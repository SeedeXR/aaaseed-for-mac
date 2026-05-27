#include "tex_info.h"
#include "err.h"
#include "infrastructure/obj/obj_ui.h"


c_tex_info::c_tex_info()
	:_name_gl			{0}
	,_channel_nb		{4}			
	,_internal_format	{GL_NONE}					//todo we should init in a smarter way : undefined/uninitialized format 
	,_internal_type		{GOL::INTERNAL_TYPE::NONE}	//todo we should init in a smarter way : undefined/uninitialized type 	(this a try)
	,_size_byte_nb		{0}
	,_b_free			{true}
{
}

c_tex_info::~c_tex_info()
{
	delete_name();
}

void	c_tex_info::delete_name()
{
	if( _name_gl!=0 )
	{
		GOL::delete_texture( &_name_gl );
		_name_gl = 0;
	}
}
void	c_tex_info::set_name_gl( GLuint CONST name_gl )
{
	if( _name_gl == 0 )
		_name_gl = name_gl;
	else
		debug_break( "%s() but name already defined", __FUNCTION__ );
}

void	c_tex_info::add_user( c_obj_ui* CONST user )
{
	user_map::iterator	it = _users.find(user);
	if( it == _users.end() )
		_users[user] = 1;
	else
		++(it->second);	
}

void	c_tex_info::remove_user( c_obj_ui* CONST user )
{
	user_map::iterator	it = _users.find(user);
	if( it == _users.end() )
		ERR_PRINT_STRING( "%s() can't find object, this should not happen", __FUNCTION__ );
	else
	{
		INT32 i = it->second;
		if( i > 0 )
			it->second = i - 1;
		else
			ERR_PRINT_STRING( "%s() counter asked to be negative, this should not happen", __FUNCTION__ );
	}
}

void	c_tex_info::delete_user( c_obj_ui* CONST user )
{
	user_map::iterator	it = _users.find(user);
	if( it != _users.end() )
	{
		_users.erase( it );
	}
}

void	c_tex_info::ask_release_from_users( c_obj_ui* CONST obj, INT32 CONST bind )
{
	for( auto const & elt : _users )
	{
		if( elt.second > 0 )
		{
			elt.first->release_texture( obj, bind );
		}
	}
}

void	c_tex_info::set_internal_format( GLenum internal_format )
{
	_internal_format = internal_format;
	_channel_nb = GOL::get_channel_nb_from_glenum( internal_format );
}


void c_tex_info_1d::store( INT32 sx, GLenum internal_format, GOL::INTERNAL_TYPE internal_type, INT64 size_byte_nb )
{
	set_size_x( sx );
	set_internal_format( internal_format );
	set_internal_type( internal_type );
	set_size( size_byte_nb );
}

void c_tex_info_2d::store( INT32 sx, INT32 sy, GLenum internal_format, GOL::INTERNAL_TYPE internal_type, INT64 size_byte_nb )
{
	set_size_y( sy );
	c_tex_info_1d::store( sx, internal_format, internal_type, size_byte_nb );
}

void c_tex_info_3d::store( INT32 sx, INT32 sy, INT32 sz, GLenum internal_format, GOL::INTERNAL_TYPE internal_type, INT64 size_byte_nb )
{
	set_size_z( sz );
	c_tex_info_2d::store( sx, sy, internal_format, internal_type, size_byte_nb );
}


