
#include "aaa_shader.h"
#include "glsl_shader.h"


c_shaders*	g_shaders_vertex	=	nullptr;
c_shaders*	g_shaders_geometry	=	nullptr;
c_shaders*	g_shaders_fragment	=	nullptr;
c_shaders*	g_shaders_compute	=	nullptr;


void	c_shaders::c_init()
{
	g_shaders_vertex			= new c_shaders( c_shader::SHADER_VERTEX	);
	g_shaders_vertex->set(		SHADERS_MAX_NB, "Vertex Shader Bind",		"shader_vertex_bind"	);

	g_shaders_geometry			= new c_shaders( c_shader::SHADER_GEOMETRY	);
	g_shaders_geometry->set(	SHADERS_MAX_NB, "Geometry Shader Bind",		"shader_geometry_bind"	);

	g_shaders_fragment			= new c_shaders( c_shader::SHADER_FRAGMENT	);
	g_shaders_fragment->set(	SHADERS_MAX_NB, "Fragment Shader Bind",		"shader_fragment_bind"	);

	g_shaders_compute			= new c_shaders( c_shader::SHADER_COMPUTE	);
	g_shaders_compute->set(		SHADERS_MAX_NB, "Compute Shader Bind",		"shader_compute_bind"	);
}

void	c_shaders::c_deinit()
{
	SAFE_DELETE( g_shaders_compute	);
	SAFE_DELETE( g_shaders_fragment	);
	SAFE_DELETE( g_shaders_geometry	);
	SAFE_DELETE( g_shaders_vertex	);
}

void	c_shaders::c_load_from_file( o_str CONST & filename )
{
	g_shaders_vertex->load_bind(	filename );
	g_shaders_geometry->load_bind(	filename );
	g_shaders_fragment->load_bind(	filename );
	g_shaders_compute->load_bind(	filename );
}

void	c_shaders::c_save_to_file( o_str CONST & filename )
{
	g_shaders_vertex->save_bind(	filename );
	g_shaders_geometry->save_bind(	filename );
	g_shaders_fragment->save_bind(	filename );
	g_shaders_compute->save_bind(	filename );
}

c_shaders::c_shaders( c_shader::SHADER_TYPE type )
	:_type( type )
	,_bind(0)
{
}

c_shaders::~c_shaders()
{
	dealloc();
}
AAA_ERR	c_shaders::alloc( INT32 nb, C_PCHAR_C bind_name, C_PCHAR_C bind_ext )
{
	if( !_bind )
		_bind = c_bind::get_new( bind_ext );
	if( _bind )
	{
		_bind->set( nb, bind_name, bind_ext, aaa::file::TYPE_IO_NONE, 4, true, true );
		return AAA_OK;
	}
	return  ERR_ANY;
}

void	c_shaders::dealloc()
{
	if( _bind )
	{
		INT32	nb = _bind->get_elt_nb();
		for( INT32 i = 0; i < nb; ++i )
			SAFE_DELETE( _vect[i] );
		SAFE_DELETE( _bind );
	}
}

AAA_ERR	c_shaders::set( INT32 nb, C_PCHAR_C bind_name, C_PCHAR_C bind_ext )
{
	AAA_ERR	retcode = alloc( nb, bind_name, bind_ext );
	if( NOERR(retcode) )
		_vect.assign( nb, nullptr ); 
	else
		BOX_ERR("Can't allocate the shader list");
	return retcode;
}

FINLINE	C_PCHAR_C	c_shaders::bind_get_name( INT32 index )
{
	return _bind->get_str(index);
}

c_shader*	c_shaders::get_shader( INT32 index )
{
	c_shader*	p_shader = _vect[ index ];
	if( !p_shader )
	{
		C_PCHAR_C fpath = bind_get_name( index );
		p_shader = (c_shader*) new c_shader_glsl( _type );
		p_shader->set_shader_fpath( fpath );
		_vect[ index ] = p_shader;
	}
	return p_shader;
}

/*
void	shaders::update()
{
}
*/

void	c_shaders::menu_do( INT32 in )								{	_bind->set_index_cur( in);	}
INT32	c_shaders::menu_build( PT_MENU_FN menu_fn )					{	return _bind->menu_build( 0, menu_fn );	}
/*
void	c_shaders::load(char* CONST filename_in)
{
	bind_list_->set_item( bind_list_->get_index_cur(), (char *)filename_in );
}
*/
//todo we can't change add or remove on the fly
void	c_shaders::load_bind(	o_str CONST & filename )			{	_bind->load_from_file_add_ext( filename );	}
void	c_shaders::save_bind(	o_str CONST & filename )			{	_bind->save_to_file_add_ext(   filename );	}

void	MAACALLBACK	shader_vertex_menu_fn(		INT32 in )			{	g_shaders_vertex->menu_do(		in );	}
void	MAACALLBACK	shader_fragment_menu_fn(	INT32 in )			{	g_shaders_fragment->menu_do(	in );	}
void	MAACALLBACK	shader_geometry_menu_fn(	INT32 in )			{	g_shaders_geometry->menu_do(	in );	}
void	MAACALLBACK	shader_compute_menu_fn(		INT32 in )			{	g_shaders_compute->menu_do(		in );	}