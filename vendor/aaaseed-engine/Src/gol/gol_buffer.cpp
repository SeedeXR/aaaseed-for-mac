#include "gol/gol_buffer.h"
#include "gol/gol_draw.h"
#include "OpenCL/opencl_types.h"

gol_buffer::gol_buffer( C_PCHAR_C name_dbg )
	:_name_dbg			(name_dbg)
	,_vbo				{0}
	,_size_allocated	{0}
	,_size_vbo_binded	{0}
	,_size_vbo_asked	{0}
	,_data				{nullptr}		
	,_b_gl_alloc_mem	{true}
{
}

gol_buffer::~gol_buffer()
{
	GOL::delete_vbo( _vbo );
	FREE_ALIGNED_AND_NULL( _data );
}

AAA_ERR	gol_buffer::alloc_bytes( INT32 bytes_nb )
{
	INT32	size_asked = bytes_nb;

	if( size_asked > _size_allocated )
	{
		unbind_vbo_data();
		if( !_b_gl_alloc_mem )
		{
			_data =	(void*)	REALLOC_ALIGNED( _data,	size_asked );	//todo eventually deal with the double size of line being always allocated
			if( !_data )
			{
				_size_vbo_asked = 0;
				ERR_PRINT_STRING( "can't alloc %d bytes for buffer %s", size_asked, _name_dbg.get() );
				return ERR_MEM_BASE;
			}
		}
		_size_allocated = size_asked;
		_size_vbo_asked = size_asked;
	}
	else
	{
		if( _size_vbo_binded != size_asked )
		{
			unbind_vbo_data();
			_size_vbo_asked = size_asked;
		}
	}
	return AAA_OK;
}

AAA_ERR	gol_buffer::alloc_float4( INT32 vec_nb )
{
	if( vec_nb <= 0 )
	{
		vec_nb = 32 * 2;	//force a minimum buffer
	}
//chanel	vec_nb = adapt_to_work_group_size( vec_nb, _block_size_x );	//chanel _opencl->get_kernel_workgroup_size() );

	return alloc_bytes( vec_nb * sizeof(float4) );
}

void	gol_buffer::bind_vbo( )
{
	if( _vbo==0 )
		GOL::gen_vbo( _vbo );
	GOL::bind_buffer( GL_ARRAY_BUFFER, _vbo );
}
void	gol_buffer::bind_vbo_data()
{
	if( _size_vbo_binded == _size_vbo_asked )
		return;
	bind_vbo();
	GOL::set_buffer_data( GL_ARRAY_BUFFER, _size_vbo_asked, _b_gl_alloc_mem ? nullptr : (GLvoid *)_data, GL_DYNAMIC_DRAW );	// or GL_DYNAMIC_COPY
	_size_vbo_binded = _size_vbo_asked;
	GOL::unbind_buffer( GL_ARRAY_BUFFER );
}

void	gol_buffer::unbind_vbo_data()
{
	if( _size_vbo_binded == 0 )
		return;
	//not necessary. is it even valid to do this
	/*
	bind_vbo();
	GOL::set_buffer_data( GL_ARRAY_BUFFER, 0, nullptr,	GL_DYNAMIC_DRAW );	// or GL_DYNAMIC_COPY
	GOL::unbind_buffer( GL_ARRAY_BUFFER );
	*/
	_size_vbo_binded = 0;
}

void	gol_buffer::set_gl_alloc_mem( bool b )
{
	_b_gl_alloc_mem = b;
}

AAA_ERR	gol_buffer::be_ready()
{
	bind_vbo_data();
	return AAA_OK;
}

#if AAA_OPENCL_USE()
opencl_buffer::opencl_buffer( C_PCHAR_C name_dbg ) 
	:gol_buffer(name_dbg)
	,_cl_mem(nullptr)
	,_kernel_arg_set(-1)
	,_kernel_arg_wanted(-1)
	,_opencl(nullptr)
{
}

opencl_buffer::~opencl_buffer()
{
	destroy_cl_buffer();
}

void	opencl_buffer::unbind_vbo_data()
{
	destroy_cl_buffer();
	gol_buffer::unbind_vbo_data();
}

//	used to reset
void opencl_buffer::set_cl_context( AAA_OPENCL::c_opencl* opencl )
{
	destroy_cl_buffer();
	_opencl = opencl;
}

AAA_ERR	opencl_buffer::create_cl_buffer()
{
	if( _cl_mem )
		return AAA_OK;
	_cl_mem = _opencl->buffer_gl_create( CL_MEM_WRITE_ONLY, get_vbo() );
	if( _cl_mem )
		return AAA_OK;

	ERR_PRINT_STRING( "can't create a %s openCL buffer from the openGL ref %d", get_name_dbg(), get_vbo() );
	return ERR_ANY;
}

AAA_ERR	opencl_buffer::destroy_cl_buffer()
{
	if( _cl_mem )
	{
		//not necessary. is it even valid to do this
/*
		if( _kernel_arg_wanted>0 && _kernel_arg_set == _kernel_arg_wanted )
		{
			if( !_opencl->kernel_set_arg( _kernel_arg_wanted, nullptr, get_name_str() ) )
			{
				debug_break( "%s() can't set to NULL", __FUNCTION__ );
			}
			unvalidate_kernel_arg();
		}
*/
		clear_kernel_arg();
		SAFE_DELETE( _cl_mem );
	}
	return AAA_OK;
}

AAA_ERR	opencl_buffer::set_kernel_arg()
{
	if( _kernel_arg_wanted>0 && _kernel_arg_set != _kernel_arg_wanted )
	{
		if( !_opencl->kernel_set_arg( _kernel_arg_wanted, get_cl_mem(), get_name_dbg() ) )
			return ERR_ANY;
		_kernel_arg_set = _kernel_arg_wanted;
	}
	return AAA_OK;
}

void		opencl_buffer::clear_kernel_arg()
{
	if( _kernel_arg_set >= 0 && _kernel_arg_wanted >= 0 )
	{
		//not necessary. is it even valid to do this
		/*
		if( !_opencl->kernel_set_arg( _kernel_arg_wanted, nullptr, get_name_str() ) )
		{
			debug_break( "%s() can't set to NULL", __FUNCTION__ );
		}
		*/
	}
	_kernel_arg_set = -1;
}

AAA_ERR	opencl_buffer::be_ready()
{
	gol_buffer::be_ready();
	if( ERR( create_cl_buffer() ) )
		return ERR_ANY;
	if( ERR( set_kernel_arg() ) )
		return ERR_ANY;
	return AAA_OK;
}

#endif //AAA_OPENCL_USE
