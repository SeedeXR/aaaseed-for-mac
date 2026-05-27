
#ifdef AAA_GOL_DRAW_H
#error "GOL_DRAW_H included more than once."
#endif
#define AAA_GOL_DRAW_H 1

#ifndef AAA_GOL_BASE_H
#	include "gol_base.h"
#endif


namespace GOL	{

#define	AAA_SCREEN_DEFORMATION() 0
//	pour memoire
#if	AAA_SCREEN_DEFORMATION()
	enum VERTEX_DEF_TYPE : INT32
	{
		VERTEX_DEF_NONE = 0,
		VERTEX_DEF_SHADER,
		VERTEX_DEF_T_D,
		VERTEX_DEF_T_D_T,
		VERTEX_DEF_TYPE_MAX_NB,
	};

	extern	INT32	s_screen_def_type;
	extern	REAL	screen_def_cos_freq;
	extern	REAL	screen_def_cos_limit;
	extern	REAL	screen_def_z_factor;
	extern	REAL	screen_def_y_origin;
	extern	REAL	screen_def_y_factor;
#endif // #ifdef	AAA_SCREEN_DEFORMATION

void scissor(		INT32 CONST x, INT32 CONST y, INT32 CONST sx, INT32 CONST sy );
void get_scissor(	INT32&      x, INT32&      y, INT32&      sx, INT32&      sy );
									   				  
void viewport(		INT32 CONST x, INT32 CONST y, INT32 CONST sx, INT32 CONST sy );
void get_viewport(	INT32&      x, INT32&      y, INT32&      sx, INT32&      sy );
	
//VBO
//
extern	bool	b_vbo_allow;
FINLINE	bool	is_vbo() { return b_vbo_allow; }

FINLINE void	gen_vbo(		GLuint& vbo )								{	gen_buffer(		vbo );		}
FINLINE	void	delete_vbo(		GLuint& vbo )								{	delete_buffer(	vbo );		}
FINLINE void	gen_vbos(		INT32 CONST nb,	GLuint* CONST vbos )		{	gen_buffers(	nb, vbos );	}
FINLINE	void	delete_vbos(	INT32 CONST nb, GLuint* CONST vbos )		{	delete_buffers(	nb, vbos );	}

//VAO
//
enum class VAO_TYPE : INT32 
{
	FP32,
	VEC2,
	VEC3,
	VEC4,
	UINT4,
	UNDEFINED,
	TYPE_NB
};
CONSTEXPR UINT32 vao_compo_nb[ static_cast<INT32>(VAO_TYPE::TYPE_NB) ]
{
	1, // fp32
	2, // vec2
	3, // vec3
	4, // vec4
	4, // uvec4/int4
	0, // UNDEFINED
};
CONSTEXPR UINT32 vao_byte_nb[ static_cast<INT32>(VAO_TYPE::TYPE_NB) ]
{
	sizeof(FP32),	// fp32
	sizeof(FP32)*2,	// vec2
	sizeof(FP32)*3,	// vec3
	sizeof(FP32)*4,	// vec4
	sizeof(INT32)*4,	// uvec4/int4
	0,					// UNDEFINED
};
CONSTEXPR UINT32 vao_gl_type[ static_cast<INT32>(VAO_TYPE::TYPE_NB) ]
{
	GL_FLOAT,			// fp32
	GL_FLOAT,			// vec2
	GL_FLOAT,			// vec3
	GL_FLOAT,			// vec4
	GL_UNSIGNED_INT,	// uvec4/int4
	GL_NONE,			// UNDEFINED
};

enum class IBO_TYPE : INT32
{
	IBO_UINT8,
	IBO_UINT16,
	IBO_UINT32,
	UNDEFINED,
	TYPE_NB
};
CONSTEXPR UINT32 ibo_byte_nb[ static_cast<INT32>(IBO_TYPE::TYPE_NB) ]
{
	sizeof( UINT8 ),	// UINT8
	sizeof( UINT16 ),	// UINT16
	sizeof( UINT32 ),	// UINT32
	0,					// UNDEFINED
};
CONSTEXPR UINT32 ibo_gl_type[ static_cast<INT32>(IBO_TYPE::TYPE_NB) ]
{
	GL_UNSIGNED_BYTE,	// UINT8
	GL_UNSIGNED_SHORT,	// UINT16
	GL_UNSIGNED_INT,	// UINT32
	GL_NONE,			// UNDEFINED
};


extern	bool	b_vao_can;
extern	bool	b_vao_allow;
extern	bool	b_vao_do;
extern	bool	b_vao_element_bind;
extern	GLuint	vao_cur;
extern	void	init_vao();
extern	void	update_vao();
FINLINE	GLuint	get_vao()	{ return vao_cur; }
FINLINE	bool	is_vao()	{ return b_vao_do; }
FINLINE	void	bind_vao_low( GLuint CONST c_vao )
{
	if( is_state_cache_no() || vao_cur != c_vao )
	{
		glBindVertexArray( c_vao );
		vao_cur = c_vao;
	}
}
FINLINE	void	bind_vao( GLuint CONST c_vao )
{
	if( b_vao_do )
		bind_vao_low( c_vao );
	//	in case we switch to no c_vao and we already activated it
	else if( b_vao_can )
		bind_vao_low( 0 );
}
FINLINE	void	unbind_vao()		{	bind_vao_low(0);	}
FINLINE void	UNBIND_VAO()		{}
FINLINE void	unbind_vao_secu()	{}

FINLINE void	gen_vao( GLuint& c_vao )
{
	if( b_vao_can )
		glGenVertexArrays( 1, &c_vao );
}
FINLINE	void	delete_vao( GLuint& c_vao )
{
	if( b_vao_can )
	{
		if( vao_cur == c_vao )
			unbind_vao();
		glDeleteVertexArrays( 1, &c_vao );
		c_vao = 0;
	}
}

//DRAW
CONSTEXPR C_PCHAR_C	draw_primitive_str[10*2] =	// *2 because of SYNOMYM
{
	"POINTS",
	"LINES",
	"LINE_LOOP",
	"LINE_STRIP",
	"TRIANGLES",
	"TRIANGLE_STRIP",
	"TRIANGLE_FAN",
	"QUADS",
	"QUAD_STRIP",
	"POLYGON",

	"GL_POINTS",
	"GL_LINES",
	"GL_LINE_LOOP",
	"GL_LINE_STRIP",
	"GL_TRIANGLES",
	"GL_TRIANGLE_STRIP",
	"GL_TRIANGLE_FAN",
	"GL_QUADS",
	"GL_QUAD_STRIP",
	"GL_POLYGON",
};

extern	bool	b_draw_avoid_vertex_use;
extern	bool	b_draw_test_using_array_element;

extern	bool	b_poly_use;
FINLINE	bool	is_poly_use()				{	return b_poly_use;	}

//DRAW OLD SCHOOL
//
FINLINE	void	begin( INT32 CONST what )	{	glBegin( what );		}
FINLINE	void	begin_lines()				{	glBegin( GL_LINES );	}
FINLINE	void	end()						{	glEnd();				}

FINLINE	void	vertex2fv(		FP32 CONST * CONST pt )										{	glVertex2fv( pt );		}
FINLINE	void	vertex2f(		FP32 CONST x,	FP32 CONST y )								{	glVertex2f( x,y );		}
FINLINE	void	vertex3fv(		FP32 CONST * CONST pt )										{	glVertex3fv( pt );		}
FINLINE	void	vertex3f(		FP32 CONST x,	FP32 CONST y,	FP32 CONST z )				{	glVertex3f(	x,y,z );	}

FINLINE	void	vertex2dv(		DOUBLE CONST * CONST pt )									{	glVertex2dv( pt );		}
FINLINE	void	vertex2d(		DOUBLE CONST x,		DOUBLE CONST y )						{	glVertex2d( x,y );		}
FINLINE	void	vertex3dv(		DOUBLE CONST * CONST pt )									{	glVertex3dv( pt );		}
FINLINE	void	vertex3d(		DOUBLE CONST x,		DOUBLE CONST y,		DOUBLE CONST z )	{	glVertex3d(	x,y,z );	}

#if AAA_REAL_IS_DOUBLE()
FINLINE	void	vertex2v(		REAL CONST * CONST pt )										{	vertex2dv( pt );		}
FINLINE	void	vertex3v(		REAL CONST * CONST pt )										{	vertex3dv( pt );		}
FINLINE	void	vertex2(		REAL CONST x,		REAL CONST y )							{	vertex2d( x,y );		}
FINLINE	void	vertex3(		REAL CONST x,		REAL CONST y,		REAL CONST z )		{	vertex3d( x,y,z );		}
FINLINE	void	normal3v(		REAL CONST * CONST pt )										{	glNormal3dv( pt );		}
FINLINE	void	normal3(		REAL CONST x,		REAL CONST y,		REAL CONST z )		{	glNormal3d( x,y,z );	}
#else
FINLINE	void	vertex2v(		REAL CONST * CONST pt )										{	vertex2fv( pt );		}
FINLINE	void	vertex3v(		REAL CONST * CONST pt )										{	vertex3fv( pt );		}
FINLINE	void	vertex2(		REAL CONST x,		REAL CONST y )							{	vertex2f( x,y );		}
FINLINE	void	vertex3(		REAL CONST x,		REAL CONST y,		REAL CONST z )		{	vertex3f( x,y,z );		}
FINLINE	void	normal3v(		REAL CONST * CONST pt )										{	glNormal3fv( pt );		}
FINLINE	void	normal3(		REAL CONST x,		REAL CONST y,		REAL CONST z )		{	glNormal3f( x,y,z );	}
#endif
FINLINE	void	normal3v_neg(	REAL CONST * CONST pt )										{	normal3( -*pt, -*( pt+1 ),-*( pt+2 ) ) ;	}


//VERTEX
//
FINLINE void	draw_arrays(				GLenum CONST mode, INT32 CONST count, INT32 CONST first )										{	glDrawArrays( mode, first,	count );	}
FINLINE void	draw_arrays(				GLenum CONST mode, INT32 CONST count )															{	glDrawArrays( mode, 0,		count );	}

extern	void	draw_arrays_instanced(		GLenum CONST mode, INT32 CONST count, INT32 CONST first,	INT32 CONST instance_count	);
FINLINE void	draw_arrays_instanced(		GLenum CONST mode, INT32 CONST count,						INT32 CONST instance_count	)		{	draw_arrays_instanced( mode, count, 0, instance_count );	}

FINLINE void	draw_arrays_indirect(		GLenum CONST mode,	void CONST * CONST indirect	)												{	glDrawArraysIndirect( mode, indirect );		}

FINLINE	void	draw_elements(				GLenum CONST mode, INT32 CONST count, GLenum CONST type, void CONST * CONST indices )			{	glDrawElements( mode, count, type, indices );				}
FINLINE	void	draw_elements(				GLenum CONST mode, INT32 CONST count, UINT32 CONST * indices )									{	draw_elements( mode, count, GL_UNSIGNED_INT,	indices	);	}
FINLINE	void	draw_elements(				GLenum CONST mode, INT32 CONST count, UINT16 CONST * indices )									{	draw_elements( mode, count, GL_UNSIGNED_SHORT,	indices	);	}
FINLINE	void	draw_elements(				GLenum CONST mode, INT32 CONST count, UINT8 CONST *  indices )									{	draw_elements( mode, count, GL_UNSIGNED_BYTE,	indices	);	}

FINLINE	void	draw_elements_instanced(	GLenum CONST mode, INT32 CONST count, GLenum CONST type, void CONST * CONST indices,
																									INT32 CONST instance_count	)			{	glDrawElementsInstanced( mode, count, type, indices, instance_count );	}

FINLINE	void	draw_elements_base_vertex(	GLenum CONST mode, INT32 CONST count, GLint CONST basevertex, GLenum CONST type, void CONST * CONST indices )
																																			{	glDrawElementsBaseVertex( mode, count, type, (void*)indices, basevertex );	}
FINLINE	void	draw_elements_base_vertex(	GLenum CONST mode, INT32 CONST count, GLint CONST basevertex, UINT32 CONST * CONST indices )	{	draw_elements_base_vertex( mode, count, basevertex, GL_UNSIGNED_INT,	indices	);	}
FINLINE	void	draw_elements_base_vertex(	GLenum CONST mode, INT32 CONST count, GLint CONST basevertex, UINT16 CONST * CONST indices )	{	draw_elements_base_vertex( mode, count, basevertex, GL_UNSIGNED_SHORT,	indices );	}
FINLINE	void	draw_elements_base_vertex(	GLenum CONST mode, INT32 CONST count, GLint CONST basevertex, UINT8 CONST *  CONST indices )	{	draw_elements_base_vertex( mode, count, basevertex, GL_UNSIGNED_BYTE,	indices	);	}

FINLINE	void	draw_array_element(			GLint CONST index )																				{	glArrayElement( index );	}

//CLIENT STATE
//
extern	bool b_client_state_vertex;
FINLINE	void	enable_client_state_vertex()
{
	if( is_state_cache_no() || vao_cur==0 )
	{
		if( is_state_cache_no() || !b_client_state_vertex )
		{
			glEnableClientState( GL_VERTEX_ARRAY );
			b_client_state_vertex = true;
		}
	}
	else
		glEnableClientState( GL_VERTEX_ARRAY );
}
FINLINE	void	disable_client_state_vertex()
{
	if( is_state_cache_no() || vao_cur==0 )
	{
		if( is_state_cache_no() || b_client_state_vertex )
		{
			glDisableClientState( GL_VERTEX_ARRAY );
			b_client_state_vertex = false;
		}
	}
	else
		glDisableClientState( GL_VERTEX_ARRAY );
}

FINLINE	void set_pointer_vertex2f(	CP_CVOID vec,				INT32 CONST stride=0 )	{	glVertexPointer( 2, GL_FLOAT,	stride, vec );	}
FINLINE	void set_pointer_vertex3f(	CP_CVOID vec,				INT32 CONST stride=0 )	{	glVertexPointer( 3, GL_FLOAT,	stride, vec );	}
FINLINE	void set_pointer_vertex4f(	CP_CVOID vec,				INT32 CONST stride=0 )	{	glVertexPointer( 4, GL_FLOAT,	stride, vec );	}

FINLINE	void set_pointer_vertex2(	FP32 CONST * CONST vec,		INT32 CONST stride=0 )	{	glVertexPointer( 2, GL_FLOAT,	stride, vec );	}
FINLINE	void set_pointer_vertex3(	FP32 CONST * CONST vec,		INT32 CONST stride=0 )	{	glVertexPointer( 3, GL_FLOAT,	stride, vec );	}
FINLINE	void set_pointer_vertex4(	FP32 CONST * CONST vec,		INT32 CONST stride=0 )	{	glVertexPointer( 4, GL_FLOAT,	stride, vec );	}

FINLINE	void set_pointer_vertex2d(	CP_CVOID vec,				INT32 CONST stride=0 )	{	glVertexPointer( 2, GL_DOUBLE,	stride, vec );	}
FINLINE	void set_pointer_vertex3d(	CP_CVOID vec,				INT32 CONST stride=0 )	{	glVertexPointer( 3, GL_DOUBLE,	stride, vec );	}
FINLINE	void set_pointer_vertex4d(	CP_CVOID vec,				INT32 CONST stride=0 )	{	glVertexPointer( 4, GL_DOUBLE,	stride, vec );	}

FINLINE	void set_pointer_vertex2(	DOUBLE CONST * CONST vec,	INT32 CONST stride=0 )	{	glVertexPointer( 2, GL_DOUBLE,	stride, vec );	}
FINLINE	void set_pointer_vertex3(	DOUBLE CONST * CONST vec,	INT32 CONST stride=0 )	{	glVertexPointer( 3, GL_DOUBLE,	stride, vec );	}
FINLINE	void set_pointer_vertex4(	DOUBLE CONST * CONST vec,	INT32 CONST stride=0 )	{	glVertexPointer( 4, GL_DOUBLE,	stride, vec );	}


extern	bool b_client_state_normal;
FINLINE	void	enable_client_state_normal()
{
	if( is_state_cache_no() || vao_cur==0 )
	{
		if( is_state_cache_no() || !b_client_state_normal )
		{
			glEnableClientState( GL_NORMAL_ARRAY );
			b_client_state_normal = true;
		}
	}
	else
		glEnableClientState( GL_NORMAL_ARRAY );
}
FINLINE	void	disable_client_state_normal()
{
	if( is_state_cache_no() || vao_cur==0 )
	{
		if( is_state_cache_no() || b_client_state_normal )
		{
			glDisableClientState( GL_NORMAL_ARRAY );
			b_client_state_normal = false;
		}
	}
	else
		glDisableClientState( GL_NORMAL_ARRAY );
}

FINLINE	void set_pointer_normal3f(	CP_CVOID vec,				INT32 CONST stride=0 )	{	glNormalPointer( GL_FLOAT,	stride, vec );	}
FINLINE	void set_pointer_normal3d(	CP_CVOID vec,				INT32 CONST stride=0 )	{	glNormalPointer( GL_DOUBLE,	stride, vec );	}
FINLINE	void set_pointer_normal(	FP32 CONST * CONST vec,		INT32 CONST stride=0 )	{	glNormalPointer( GL_FLOAT,	stride, vec );	}
FINLINE	void set_pointer_normal(	DOUBLE CONST * CONST vec,	INT32 CONST stride=0 )	{	glNormalPointer( GL_DOUBLE,	stride, vec );	}

extern	bool b_client_state_color;
FINLINE	void enable_client_state_color()
{
	if( is_state_cache_no() || vao_cur==0 )
	{
		if( is_state_cache_no() || !b_client_state_color )
		{
			glEnableClientState( GL_COLOR_ARRAY );
			b_client_state_color = true;
		}
	}
	else
		glEnableClientState( GL_COLOR_ARRAY );
}
FINLINE	void	disable_client_state_color()
{
	if( is_state_cache_no() || vao_cur==0 )
	{
		if( is_state_cache_no() || b_client_state_color )
		{
			glDisableClientState( GL_COLOR_ARRAY );
			b_client_state_color = false;
		}
	}
	else
		glDisableClientState( GL_COLOR_ARRAY );
}

FINLINE	void set_pointer_color1f(	CP_CVOID vec,				INT32 CONST stride=0 )	{	glColorPointer(	1, GL_FLOAT,	stride, vec );	}
FINLINE	void set_pointer_color2f(	CP_CVOID vec,				INT32 CONST stride=0 )	{	glColorPointer( 2, GL_FLOAT,	stride, vec );	}
FINLINE	void set_pointer_color3f(	CP_CVOID vec,				INT32 CONST stride=0 )	{	glColorPointer( 3, GL_FLOAT,	stride, vec );	}
FINLINE	void set_pointer_color4f(	CP_CVOID vec,				INT32 CONST stride=0 )	{	glColorPointer( 4, GL_FLOAT,	stride, vec );	}

FINLINE	void set_pointer_color1(	FP32 CONST * CONST vec,	INT32 CONST stride=0 )	{	glColorPointer( 1, GL_FLOAT,	stride, vec );	}
FINLINE	void set_pointer_color2(	FP32 CONST * CONST vec,	INT32 CONST stride=0 )	{	glColorPointer( 2, GL_FLOAT,	stride, vec );	}
FINLINE	void set_pointer_color3(	FP32 CONST * CONST vec,	INT32 CONST stride=0 )	{	glColorPointer( 3, GL_FLOAT,	stride, vec );	}
FINLINE	void set_pointer_color4(	FP32 CONST * CONST vec,	INT32 CONST stride=0 )	{	glColorPointer( 4, GL_FLOAT,	stride, vec );	}

FINLINE	void set_pointer_color1d(	CP_CVOID vec,				INT32 CONST stride=0 )	{	glColorPointer( 1, GL_DOUBLE,	stride, vec );	}
FINLINE	void set_pointer_color2d(	CP_CVOID vec,				INT32 CONST stride=0 )	{	glColorPointer( 2, GL_DOUBLE,	stride, vec );	}
FINLINE	void set_pointer_color3d(	CP_CVOID vec,				INT32 CONST stride=0 )	{	glColorPointer( 3, GL_DOUBLE,	stride, vec );	}
FINLINE	void set_pointer_color4d(	CP_CVOID vec,				INT32 CONST stride=0 )	{	glColorPointer( 4, GL_DOUBLE,	stride, vec );	}

FINLINE	void set_pointer_color1(	DOUBLE CONST * CONST vec,	INT32 CONST stride=0 )	{	glColorPointer( 1, GL_DOUBLE,	stride, vec );	}
FINLINE	void set_pointer_color2(	DOUBLE CONST * CONST vec,	INT32 CONST stride=0 )	{	glColorPointer( 2, GL_DOUBLE,	stride, vec );	}
FINLINE	void set_pointer_color3(	DOUBLE CONST * CONST vec,	INT32 CONST stride=0 )	{	glColorPointer( 3, GL_DOUBLE,	stride, vec );	}
FINLINE	void set_pointer_color4(	DOUBLE CONST * CONST vec,	INT32 CONST stride=0 )	{	glColorPointer( 4, GL_DOUBLE,	stride, vec );	}

FINLINE	void	enable_vertex_attrib_array(		GLuint CONST location )
{
	glEnableVertexAttribArray( location );
}
FINLINE	void	disable_vertex_attrib_array(	GLuint CONST location )
{
	glDisableVertexAttribArray( location );
}
FINLINE	void	set_vertex_attrib_pointer(		GLuint CONST index, GLint CONST size, GLenum CONST type,
												GLvoid CONST * CONST pointer=nullptr, INT32 CONST stride=0 )
{
	glVertexAttribPointer( index, size, type, GL_FALSE, stride, pointer );
}
FINLINE	void	set_vertex_attrib_pointer_int(	GLuint CONST index, GLint CONST size, GLenum CONST type,
												GLvoid CONST * CONST pointer=nullptr, INT32 CONST stride=0 )
{
	glVertexAttribIPointer( index, size, type, stride, pointer );
}

extern	bool b_client_state_texcoor;
FINLINE	void	enable_client_state_texcoor()
{
	if( is_state_cache_no() || vao_cur==0 )
	{
		if( is_state_cache_no() || !b_client_state_texcoor )
		{
			glEnableClientState( GL_TEXTURE_COORD_ARRAY );
			b_client_state_texcoor = true;
		}
	}
	else
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
}


FINLINE	void	disable_client_state_texcoor()
{
	if( is_state_cache_no() || vao_cur==0 )
	{
		if( is_state_cache_no() || b_client_state_texcoor )
		{
			glDisableClientState( GL_TEXTURE_COORD_ARRAY );
			b_client_state_texcoor = false;
		}
	}
	else
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );
}

//todoqqq deal with glTexCoordPointer and glClientActiveTexture( GLenum texUnit );
//	 done the poor way wasfor now
FINLINE	void	set_pointer_texcoor( INT32 CONST size, GLenum CONST type, INT32 CONST stride, void CONST * CONST pointer )
{
//todo clean this once for all
#if	1	//done for aquariums
	//maa we don't have several texture uv set by AAASeed anyhow and shader only use texture 0 coor
	//glClientActiveTexture( GL_TEXTURE0 );
	glTexCoordPointer( size, type, stride, pointer );
#else
	for( INT32 i = 0; i < tex_unit_nb; ++i )
	{
		if( !tex_units[i]._b_gen_s )
		{
			glClientActiveTexture( GL_TEXTURE0 + i );
			glTexCoordPointer( size, type, stride, pointer );
		}
	}
#endif
}
FINLINE	void	set_pointer_texcoor( INT32 CONST size, GLenum CONST type, void CONST * CONST pointer )
{
	set_pointer_texcoor( size, type, 0, pointer );
}

#if AAA_REAL_IS_DOUBLE()
FINLINE	void	texcoor2(								REAL CONST u, REAL CONST v )				{	glTexCoord2d(	u, v );		}
FINLINE	void	texcoor2v(								REAL CONST * CONST pt )						{	glTexCoord2dv(	pt );		}
FINLINE	void	texcoor3(								REAL CONST u, REAL CONST v, REAL CONST w )	{	glTexCoord3d(	u, v, w );	}
FINLINE	void	texcoor3v(								REAL CONST * CONST pt )						{	glTexCoord3dv(	pt );		}
FINLINE	void	multitexcoor2(	GLenum CONST target,	REAL CONST u, REAL CONST v )				{	glMultiTexCoord2d(	target, u, v );		}
FINLINE	void	multitexcoor2v(	GLenum CONST target,	REAL CONST * CONST pt )						{	glMultiTexCoord2dv(	target, pt );		}
FINLINE	void	multitexcoor3(	GLenum CONST target,	REAL CONST u, REAL CONST v, REAL CONST w )	{	glMultiTexCoord3d(	target, u, v, w );	}
FINLINE	void	multitexcoor3v(	GLenum CONST target,	REAL CONST * CONST pt )						{	glMultiTexCoord3dv( target, pt );		}
#else
FINLINE	void	texcoor2(								REAL CONST u, REAL CONST v )				{	glTexCoord2f(	u, v );		}
FINLINE	void	texcoor2v(								REAL CONST * CONST pt )						{	glTexCoord2fv(	pt );		}
FINLINE	void	texcoor3(								REAL CONST u, REAL CONST v, REAL CONST w )	{	glTexCoord3f(	u, v, w );	}
FINLINE	void	texcoor3v(								REAL CONST * CONST pt )						{	glTexCoord3fv(	pt );		}
FINLINE	void	multitexcoor2(	GLenum CONST target,	REAL CONST u, REAL CONST v )				{	glMultiTexCoord2f(	target, u, v );		}
FINLINE	void	multitexcoor2v(	GLenum CONST target,	REAL CONST * CONST pt )						{	glMultiTexCoord2fv( target, pt );		}
FINLINE	void	multitexcoor3(	GLenum CONST target,	REAL CONST u, REAL CONST v, REAL CONST w )	{	glMultiTexCoord3f(	target, u, v, w );	}
FINLINE	void	multitexcoor3v(	GLenum CONST target,	REAL CONST * CONST pt )						{	glMultiTexCoord3fv( target, pt );		}
#endif

typedef	void	TEXCOORD2( REAL CONST u, REAL CONST v );
typedef	void	TEXCOORD2V( REAL CONST * CONST pt );

extern	TEXCOORD2*	texcoord2_cur;
extern	TEXCOORD2V*	texcoord2v_cur;

FINLINE	void	texcoord2(	REAL CONST u, REAL CONST v )		{	( *texcoord2_cur )( u, v );	}
FINLINE	void	texcoord2v( REAL CONST * CONST pt )				{	( *texcoord2v_cur )( pt );	}

}	//namespace GOL
