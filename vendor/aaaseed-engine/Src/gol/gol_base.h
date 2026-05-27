
#ifdef AAA_GOL_BASE_H
#error "GOL_BASE_H included more than once."
#endif
#define AAA_GOL_BASE_H 1


#ifndef	AAA_AAA_GL_H
#	include "draw/aaa_gl.h"
#endif
#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

namespace GOL	{

//extern void			do_matrox_hack();

extern	bool	__b_state_cache_no;
// when true GOL keep track of opengl states and use it to avoid calls
// when false GOL still keep track but don't use it to avoid calls
//	this is useful to debug underlying graphics implementation
//	please use ancessor below is_state_cache_no() and not the variable directly 
FINLINE	CONSTEXPR bool	is_state_cache_no()	{	return __b_state_cache_no;	}

// look for extension function and check failure.
#define WGL_GET_PROC( xx )														\
{																			\
	void **x = ( void** )&xx;												\
	*x = ( void * ) wglGetProcAddress( #xx );								\
	if( *x == nullptr )														\
		ERR_PRINT_STRING( "wglGetProcAddress() : Couldn't find %s.", #xx );	\
	else																	\
		GOOD_PRINT_STRING( "%s() ready to use.", #xx );						\
}

#if	AAA_REAL_IS_DOUBLE()
#define	GOL_REAL		GL_DOUBLE
#else
#define	GOL_REAL		GL_FLOAT
#endif

extern	void	DBG_PRINT_STRING(		C_PCHAR_C fmt, ... );
extern	void	GOOD_PRINT_STRING(		C_PCHAR_C fmt, ... );
extern	void	GOOD_PRINT_STRING_1T(	C_PCHAR_C fmt, ... );
extern	void	GOOD_PRINT_STRING_2T(	C_PCHAR_C fmt, ... );
extern	void	GOOD_PRINT_STRING_3T(	C_PCHAR_C fmt, ... );
extern	void	WARNING_PRINT_STRING(	C_PCHAR_C fmt, ... );
extern	void	ERR_PRINT_STRING(		C_PCHAR_C fmt, ... );
extern	void	VERBOSE_PRINT_STRING(	C_PCHAR_C fmt, ... );

extern	bool	b_check_error;
extern	bool	b_check_error_by_frame;
extern	bool	b_check_error_by_frame_ui;
extern	bool	b_check_error_by_layer;
extern	bool	b_check_error_by_layer_ui;
extern	bool	b_check_error_by_call;
extern	bool	b_check_error_by_call_ui;

extern	C_PCHAR_C	get_error_str( GLenum CONST err );
extern	void		print_errors_low( INT32 err_first, C_PCHAR_C str );

FINLINE	UINT32		get_error( C_PCHAR_C str )
{
	UINT32	err_first = glGetError();
	if( err_first != GL_NO_ERROR )
		print_errors_low( err_first, str );
	return err_first;
}
FINLINE	INT32		check_error( C_PCHAR_C str = nullptr )
{
	if( b_check_error )
		return get_error( str );
	return 0;
}
#if	AAA_DEBUG()
FINLINE	INT32		check_error_debug( C_PCHAR_C str )
{
	return check_error( str );
}
#else
FINLINE	void		check_error_debug( C_PCHAR_C str )
{
}
#endif


extern	C_PCHAR_C	get_str_from_glenum( GLenum CONST gl_enum );

extern	void		print_extension();
extern	bool		have_extension(		C_PCHAR ext );
extern	bool		test_version_or_extensions( INT32 CONST major_min, INT32 CONST minor_min, C_PCHAR_C ext_a = nullptr, C_PCHAR_C ext_b = nullptr, C_PCHAR_C ext_c = nullptr );


extern	C_PCHAR_C	get_boolean_str( GLenum pname );

FINLINE	INT32		get_integer( GLenum CONST pname )
{
	GLint	param;
	glGetIntegerv( pname, &param );
	check_error( __FUNCTION__ );
	return (INT32) param;
}
FINLINE	void		get_integer( GLenum CONST pname, GLint* CONST params )
{
	glGetIntegerv( pname, params );
}
FINLINE	void		get_integer_by_index( GLenum CONST pname, GLint* CONST params, UINT32 CONST nb )
{
	for( GLuint i=0; i<nb; ++i )
		glGetIntegeri_v( pname, i, &params[i] );
}
	
FINLINE	FP32		get_fp32( GLenum CONST pname )
{
	GLfloat	param;
	glGetFloatv( pname, &param );
	check_error( __FUNCTION__ );
	return (FP32) param;
}
FINLINE	void		get_fp32( GLenum CONST pname, FP32* CONST params )
{
	glGetFloatv( pname, params );
}

FINLINE	void		get_double( GLenum CONST pname, DOUBLE* CONST params )
{
	glGetDoublev( pname, params );
}

FINLINE	void		get_real( GLenum CONST pname, REAL* CONST params )
{
#if AAA_REAL_IS_DOUBLE()
	get_double( pname, params );
#else
	get_fp32( pname, params );
#endif
}

enum HINT : INT32
{
	DONT_CARE	= 0,
	FASTEST	= 1,
	NICEST		= 2
};
extern	INT16 CONST	gl_hint[3];
FINLINE	void		hint( GLenum CONST what, GOL::HINT CONST s_hint )	{	glHint( what, gl_hint[s_hint] );	}

//BUFFER and Direct State Access
//
extern bool	b_named_buffer_can;
extern bool	b_named_buffer_asked_ui;
extern bool b_named_buffer_use;

extern bool b_direct_state_access_can;
extern bool b_direct_state_access_asked_ui;
extern bool b_direct_state_access_use;

extern	INT32 buffer_nb_current;
extern	INT32 buffer_nb_generated;


extern	void		gen_buffers(	GLsizei CONST n,		GLuint* CONST buffers );
extern	void		gen_buffer(		GLuint& buffer	);

extern	void		delete_buffers( GLsizei n, GLuint* buffers );
extern	void		delete_buffer_private( GLuint& buffer );
FINLINE	void		delete_buffer( GLuint& buffer )
{
	if( buffer )
		delete_buffer_private( buffer );
}

FINLINE	void		bind_buffer(			GLenum CONST target,	GLuint CONST buffer )		{	glBindBuffer( target, buffer ); }
FINLINE	void		unbind_buffer(			GLenum CONST target )								{	glBindBuffer( target, 0 );		}

FINLINE	void		bind_buffer_base(		GLenum CONST target,	GLuint CONST binding_index, GLuint CONST buffer )
{
	glBindBufferBase( target, binding_index, buffer );
}
FINLINE	void		unbind_buffer_base(		GLenum CONST target,	GLuint CONST binding_index )
{
	glBindBufferBase( target, binding_index, 0 );
}

FINLINE void		set_buffer_data(		GLenum CONST target,	GLsizeiptr CONST size_byte,		GLvoid CONST * CONST data,	GLenum CONST usage )
{
	glBufferData( target, size_byte, data, usage );
}
FINLINE	void 		set_named_buffer_data( GLuint CONST id,			GLsizeiptr CONST size_byte,		GLvoid CONST * CONST data,	GLenum CONST usage )
{
	if( b_named_buffer_use )
		return glNamedBufferData( id, size_byte, data, usage );
	ERR_PRINT_STRING( "GOL try to use Named buffer when impossible or not allowed" );
}
FINLINE void		set_buffer_data_sub(	GLenum CONST target,	GLintptr CONST offset,	GLsizeiptr CONST size_byte,		GLvoid CONST * CONST data )
{
	glBufferSubData( target, offset, size_byte, data );
}

FINLINE void		set_buffer_data(		GLenum CONST target,							GLsizeiptr CONST size_byte,									GLenum CONST usage )
{
	set_buffer_data( target, size_byte, 0, usage );
}

FINLINE	void		bind_set_buffer(		GLenum CONST target,	GLuint CONST buffer,	GLsizeiptr CONST size_byte,		GLvoid CONST * CONST data,	GLenum CONST usage )
{
	bind_buffer( target, buffer );
	set_buffer_data( target, size_byte, data, usage );
}
FINLINE	void		bind_set_unbind_buffer(	GLenum CONST target,	GLuint CONST buffer,	GLsizeiptr CONST size_byte,		GLvoid CONST * CONST data,	GLenum CONST usage )
{
	bind_set_buffer( target, buffer, size_byte, data, usage );
	unbind_buffer( target );
}

FINLINE	void*		map_buffer(				GLenum CONST target,	GLenum CONST access )
{
	return glMapBuffer( target, access ); 
}
FINLINE	void*		map_buffer_range(		GLenum CONST target,	GLintptr CONST offset,	GLsizeiptr CONST length,	GLbitfield CONST access )
{
	return glMapBufferRange( target, offset, length, access );
}
FINLINE	bool		unmap_buffer(			GLenum CONST target		)
{
	return (bool)glUnmapBuffer( target );
}

FINLINE	void *		map_named_buffer(		GLuint CONST id,		GLenum CONST access )
{
	if( b_named_buffer_use )
		return glMapNamedBuffer( id, access );
	ERR_PRINT_STRING( "GOL try to use Named buffer when impossible or not allowed" );
	return nullptr;
}
FINLINE	void *		map_named_buffer_range( GLuint CONST id,		GLintptr CONST offset,	GLsizeiptr CONST length,	GLbitfield CONST access )
{
	if( b_named_buffer_use )
		return glMapNamedBufferRange( id, offset, length, access );
	ERR_PRINT_STRING( "GOL try to use Named buffer when impossible or not allowed" );
	return nullptr;
}
FINLINE	bool		unmap_named_buffer(		GLuint CONST id		)
{
	if( b_named_buffer_use )
		return bool( glUnmapNamedBuffer(id) );
	ERR_PRINT_STRING( "GOL try to use Named buffer when impossible or not allowed" );
	return false;
}
//ENABLE DISABLE
//
FINLINE	void		enable( GLenum CONST cap )
{
	glEnable( cap );
}
FINLINE	void		disable( GLenum CONST cap )
{
	glDisable( cap );
}

extern INT32		get_byte_nb_from_type_enum( GLenum CONST type );

// BARRIER
FINLINE void		barrier_memory( GLbitfield CONST barriers )
{
	glMemoryBarrier( barriers );
}
FINLINE void		barrier_vertex_buffer()			{	GOL::barrier_memory( GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT );	}
FINLINE void		barrier_index_buffer()			{	GOL::barrier_memory( GL_ELEMENT_ARRAY_BARRIER_BIT );		}
FINLINE void		barrier_uniform_buffer()		{	GOL::barrier_memory( GL_UNIFORM_BARRIER_BIT );				}
FINLINE void		barrier_pixel_buffer()			{	GOL::barrier_memory( GL_PIXEL_BUFFER_BARRIER_BIT );			}
FINLINE void		barrier_frame_buffer()			{	GOL::barrier_memory( GL_FRAMEBUFFER_BARRIER_BIT );			}
FINLINE void		barrier_buffer_update()			{	GOL::barrier_memory( GL_BUFFER_UPDATE_BARRIER_BIT );		}
FINLINE void		barrier_texture_fetch()			{	GOL::barrier_memory( GL_TEXTURE_FETCH_BARRIER_BIT );		}
FINLINE void		barrier_texture_update()		{	GOL::barrier_memory( GL_TEXTURE_UPDATE_BARRIER_BIT );		}
FINLINE void		barrier_image_access()			{	GOL::barrier_memory( GL_SHADER_IMAGE_ACCESS_BARRIER_BIT );	}
FINLINE void		barrier_command()				{	GOL::barrier_memory( GL_COMMAND_BARRIER_BIT );				}
FINLINE void		barrier_transform_feedback()	{	GOL::barrier_memory( GL_TRANSFORM_FEEDBACK_BARRIER_BIT );	}
FINLINE void		barrier_atomic_counter()		{	GOL::barrier_memory( GL_ATOMIC_COUNTER_BARRIER_BIT );		}
FINLINE void		barrier_all()					{	GOL::barrier_memory( GL_ALL_BARRIER_BITS );					}

}	//namespace GOL

