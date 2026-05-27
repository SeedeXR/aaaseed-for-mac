#include "shader.h"

c_shader::c_shader( SHADER_TYPE shader_type )
	:_b_loaded	{false}
	,_b_valid	{false}
{
	_s_shader_type = shader_type;
}

void	c_shader::set_shader_fpath(C_PCHAR_C fpath)
{
	_fpath.set( fpath );
}
void	c_shader::reload()
{
	dealloc();
	load();
}