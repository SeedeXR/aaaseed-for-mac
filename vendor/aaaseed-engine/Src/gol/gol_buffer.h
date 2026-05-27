
#ifdef AAA_GOL_BUFFER_H
#error "GOL_BUFFER_H included more than once."
#endif
#define AAA_GOL_BUFFER_H 1


#ifndef	AAA_GOL_H
#	include "gol.h"
#endif
#ifndef	AAA_AAA_OPENCL_H
#	include "OpenCL/aaa_opencl.h"
#endif
#ifndef AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

class gol_buffer
{
private:
	GLuint		_vbo				;
	o_str		_name_dbg			;
	INT32		_size_allocated		;
	INT32		_size_vbo_binded	;
	INT32		_size_vbo_asked		;
	void*		_data				;			
	bool		_b_gl_alloc_mem		;
public:
	gol_buffer( C_PCHAR_C st );
	virtual ~gol_buffer();

	AAA_ERR		alloc_bytes( INT32 bytes_nb );
	AAA_ERR		alloc_float4( INT32 vec_nb );
	void		bind_vbo();

	void		set_gl_alloc_mem( bool b );

	virtual		void		bind_vbo_data();
	virtual		void		unbind_vbo_data();
	virtual		AAA_ERR		be_ready();

	FINLINE GLuint			get_vbo()							{	return _vbo;				}
	FINLINE C_PCHAR_C		get_name_dbg() CONST				{	return _name_dbg.get();		}
	FINLINE void*			get_data()							{	return _data;				}
	FINLINE	INT32			get_data_size()						{	return _size_vbo_binded;	}
};

#if AAA_OPENCL_USE()
class opencl_buffer final : public gol_buffer
{
private:
	cl::Memory*				_cl_mem;
	INT32					_kernel_arg_set;
	INT32					_kernel_arg_wanted;
	AAA_OPENCL::c_opencl*	_opencl;
public:
	opencl_buffer( C_PCHAR_C st );
	virtual ~opencl_buffer();

	virtual		void		unbind_vbo_data();
	virtual		AAA_ERR		be_ready();

	void		set_cl_context( AAA_OPENCL::c_opencl* opencl );
	AAA_ERR		create_cl_buffer();
	AAA_ERR		destroy_cl_buffer();

	void		set_kernel_arg_index( INT32 dst )	{	clear_kernel_arg(); _kernel_arg_wanted = dst;		}
	AAA_ERR		set_kernel_arg();
	void		clear_kernel_arg();

	FINLINE cl::Memory*	get_cl_mem()								{	return _cl_mem;		}
	FINLINE void		push_on( VECTOR_CLASS<cl::Memory>* vec )	{	if(_cl_mem) vec->push_back( *_cl_mem ); }
};
#endif //AAA_OPENCL_USE

