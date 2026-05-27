
#ifdef AAA_PRIMITIVE_H
#	error "PRIMITIVE_H included more than once."
#endif
#define AAA_PRIMITIVE_H 1


#ifndef	AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef	AAA_GOL_DRAW_H
#	include "gol/gol_draw.h"
#endif
#ifndef	AAA_GOL_COLOR_H
#	include "gol/gol_color.h"
#endif


namespace GOL {

template < INT32 DIM >
class c_attrib final : public c_obj
{
protected:
	FP32*	_data;
	INT32	_data_vec_allocated_nb;

	FP32*	alloc_low( INT32 CONST nb )
	{
		_data = (FP32*) REALLOC_ALIGNED_SIGNATURE( _data, nb * DIM * sizeof(FP32), __FUNCTION__ );
		if( _data )
			_data_vec_allocated_nb = nb;
		else
			dealloc();
		return _data;
	}

public:
//	static c_attrib<DIM>	base;

	void dealloc()
	{
		FREE_ALIGNED_AND_NULL( _data );
		_data_vec_allocated_nb = 0;
	}
	FINLINE FP32*	alloc( INT32 CONST nb )
	{
		if( nb <= _data_vec_allocated_nb )
			return _data;
		return alloc_low( nb );
	}
	FINLINE INT32	get_allocated_nb() CONST		{	return _data_vec_allocated_nb;	}
	FINLINE FP32*	get_fp32()  CONST				{	return _data;	}

	c_attrib()
		:_data					(nullptr)
		,_data_vec_allocated_nb	(0)
	{
	}

	c_attrib( INT32 CONST nb )
 		:_data					(nullptr)
	{
		alloc_low( nb );
	}

	~c_attrib()
	{
		dealloc();
	}


//	FINLINE	void		bind()		{	GOL::bind_vao(_vao);	}
/*
	static FINLINE void		set_vertex(		FP32 CONST * CONST vertex )
	{
		GOL::unbind_vao_secu();
		if( DIM==2 )
			GOL::set_pointer_vertex2( vertex );
		else
			GOL::set_pointer_vertex3( vertex );
		//GOL::enable_client_state_vertex();
	}
	static FINLINE void		set_vertex(		FP32 CONST * CONST vertex, INT32 CONST stride )
	{
		GOL::unbind_vao_secu();
		if( DIM==2 )
			GOL::set_pointer_vertex2( vertex, stride );
		else
			GOL::set_pointer_vertex3( vertex, stride );
		//GOL::enable_client_state_vertex();
	}
*/
};
typedef	c_attrib< 2 >	c_attrib2;
typedef	c_attrib< 3 >	c_attrib3;
typedef	c_attrib< 4 >	c_attrib4;

}	//namespace GOL

template < INT32 DIM >
class c_prim_base : public c_obj
{
protected:
	FP32*	_vertex;
	INT32	_vertex_allocated_nb;

	//FP32*	_uv;
	//INT32	_uv_allocated_nb;

//todo deal with normal also ?
	FP32*	_color;
	INT32	_color_allocated_fp32_nb;
//	GLuint	_vao;

	FP32*	alloc_vertex_low( INT32 CONST nb )
	{
		_vertex = (FP32*) REALLOC_ALIGNED_SIGNATURE( _vertex, nb * DIM * sizeof(FP32), __FUNCTION__ );
		if( _vertex )
		{
			//if( !_vao )
			//	GOL::gen_vao( _vao );
			_vertex_allocated_nb = nb;
			//GLint vao = GOL::get_vao();
			//GOL::bind_vao_low( _vao );
			//	set_point();
				//GOL::disable_client_state_normal();
				//GOL::disable_client_state_color();
				//GOL::disable_client_state_normal();
				//GOL::texcoord_disable_pointer();
			//GOL::bind_vao_low( vao );
		}
		else
			dealloc_vertex();
		return _vertex;
	}
	//FP32*	alloc_uv_low( INT32 CONST nb )
	//{
	//	_uv = (FP32*) REALLOC_ALIGNED_SIGNATURE( _uv, nb * 2 * sizeof(FP32), __FUNCTION__ );
	//	if( _uv )
	//		_uv_allocated_nb = nb;
	//	else
	//		dealloc_uv();
	//	return _uv;
	//}
	FP32*	alloc_color_low( INT32 CONST nb )
	{
		_color = (FP32*) REALLOC_ALIGNED_SIGNATURE( _color, nb * sizeof(FP32), __FUNCTION__ );
		if( _color )
			_color_allocated_fp32_nb = nb;
		else
			dealloc_color();
		return _color;
	}

public:
	void dealloc_vertex()
	{
		FREE_ALIGNED_AND_NULL( _vertex );
		_vertex_allocated_nb = 0;
	}
	//void dealloc_uv()
	//{
	//	FREE_ALIGNED_AND_NULL( _uv );
	//	_uv_allocated_nb = 0;
	//}
	void dealloc_color()
	{
		FREE_ALIGNED_AND_NULL( _color );
		_color_allocated_fp32_nb = 0;
	}
	void dealloc()
	{
		dealloc_vertex();
//		dealloc_uv();
		dealloc_color();
	}

	FINLINE FP32*	alloc_vertex( INT32 CONST nb )
	{
		if( nb <= _vertex_allocated_nb )
			return _vertex;
		return alloc_vertex_low( nb );
	}
	FINLINE FP32*	alloc_line( INT32 CONST nb )
	{
		return alloc_vertex( nb * 2 );	//todo we should extend the line concept
	}
	FINLINE INT32	get_vertex_allocated_nb() CONST	{	return _vertex_allocated_nb;	}
	FINLINE FP32*	get_vertex() CONST				{	return _vertex;		} 

	//FINLINE FP32*	alloc_uv( INT32 CONST nb )
	//{
	//	if( nb <= _uv_allocated_nb )
	//		return _uv;
	//	return alloc_uv_low( nb );
	//}
	//FINLINE INT32	get_uv_allocated_nb() CONST		{	return _uv_allocated_nb;	}
	//FINLINE FP32*	get_uv()  CONST					{	return _uv;	}

	FINLINE FP32*	get_color()  CONST				{	return _color;	}

public:
	FINLINE FP32*	alloc_color3( INT32 nb )
	{
		nb *= 3;
		if( _color_allocated_fp32_nb >= nb )
			return _color;
		return alloc_color_low( nb );
	}
	FINLINE FP32*	alloc_color3()
	{
		return alloc_color3( _vertex_allocated_nb );
	}
	FINLINE FP32*	alloc_color4( INT32 nb )
	{
		nb *= 4;
		if( _color_allocated_fp32_nb >= nb )
			return _color;
		return alloc_color_low( nb );
	}
	FINLINE FP32*	alloc_color4()
	{
		return alloc_color4( _vertex_allocated_nb );
	}


	c_prim_base()
		:_vertex(nullptr)
		,_vertex_allocated_nb(0)
		//,_uv(nullptr)
		//,_uv_allocated_nb(0)
		,_color(nullptr)
		,_color_allocated_fp32_nb(0)
	{
	}
	~c_prim_base()
	{
		dealloc();
	}


//	FINLINE	void		bind()		{	GOL::bind_vao(_vao);	}

//	virtual INT32	get_real_nb(void)	CONST {	return DIM;	}
//	c_prim_dim( INT32 nb = 64 );
public:	//todo why required by c_stroke::draw
	static FINLINE void		set_vertex(		FP32 CONST * CONST vertex )
	{
		GOL::unbind_vao_secu();
		if( DIM==2 )
			GOL::set_pointer_vertex2( vertex );
		else
			GOL::set_pointer_vertex3( vertex );
		//GOL::enable_client_state_vertex();
	}
	static FINLINE void		set_vertex(		FP32 CONST * CONST vertex, INT32 CONST stride )
	{
		GOL::unbind_vao_secu();
		if( DIM==2 )
			GOL::set_pointer_vertex2( vertex, stride );
		else
			GOL::set_pointer_vertex3( vertex, stride );
		//GOL::enable_client_state_vertex();
	}
	static	FINLINE	void draw_vertex_one(	FP32 CONST * CONST vertex, INT32 CONST i )
	{
		if( DIM==2 )
			GOL::vertex2v( vertex + i*2 );
		else
			GOL::vertex3v( vertex + i*3 );
	}
};



template < INT32 DIM >
class c_prim final : public c_prim_base<DIM>
{
	typedef c_prim_base<DIM> SUPER;
public:
	c_prim()
	{
	}
	virtual ~c_prim()
	{
	}


	static c_prim<DIM>	base;
//	virtual INT32	get_real_nb(void)	CONST {	return DIM;	}
//	c_prim_dim( INT32 nb = 64 );
protected:
	static	FINLINE void	draw_arrays(		FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb )
	{
		SUPER::set_vertex( vertex );
		GOL::draw_arrays( mode, nb );
		//GOL::disable_client_state_vertex();
	}
	static	FINLINE void	draw_arrays(		FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb, INT32 CONST first )
	{
		SUPER::set_vertex( vertex );
		GOL::draw_arrays( mode, nb, first );
		//GOL::disable_client_state_vertex();
	}
	static	FINLINE void	draw_arrays_uv(		FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb,					FP32 CONST * CONST uv )
	{
		GOL::unbind_vao_secu();
		GOL::enable_client_state_texcoor();
			GOL::set_pointer_texcoor( 2, GL_FLOAT, uv );
			draw_arrays( vertex, mode, nb );
		GOL::disable_client_state_texcoor();
	}
	static	FINLINE void	draw_arrays_uv(		FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb, INT32 CONST first, FP32 CONST * CONST uv )
	{
		GOL::unbind_vao_secu();
		GOL::enable_client_state_texcoor();
			GOL::set_pointer_texcoor( 2, GL_FLOAT, uv );
			draw_arrays( vertex, mode, nb, first );
		GOL::disable_client_state_texcoor();
	}
	static	FINLINE void	draw_arrays_normal(	FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb,					FP32 CONST * CONST normal )
	{
		GOL::unbind_vao_secu();
		GOL::enable_client_state_normal();
			GOL::set_pointer_normal( normal );
			draw_arrays( vertex, mode, nb );
		GOL::disable_client_state_normal();
	}
	static	FINLINE void	draw_arrays_normal(	FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb, INT32 CONST first, FP32 CONST * CONST normal )
	{
		GOL::unbind_vao_secu();
		GOL::enable_client_state_normal();
			GOL::set_pointer_normal( normal );
			draw_arrays( vertex, mode, nb, first );
		GOL::disable_client_state_normal();
	}
	static	FINLINE void	draw_arrays_uv_normal(	FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb,					FP32 CONST * CONST uv, FP32 CONST * CONST normal )
	{
		GOL::unbind_vao_secu();
		GOL::enable_client_state_texcoor();
		GOL::enable_client_state_normal();
			GOL::set_pointer_texcoor( 2, GL_FLOAT, uv );
			GOL::set_pointer_normal( normal );
			draw_arrays( vertex, mode, nb );
		GOL::disable_client_state_normal();
		GOL::disable_client_state_texcoor();
	}
	static	FINLINE void	draw_arrays_uv_normal(	FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb, INT32 CONST first, FP32 CONST * CONST uv, FP32 CONST * CONST normal )
	{
		GOL::unbind_vao_secu();
		GOL::enable_client_state_texcoor();
		GOL::enable_client_state_normal();
			GOL::set_pointer_texcoor( 2, GL_FLOAT, uv );
			GOL::set_pointer_normal( normal );
			draw_arrays( vertex, mode, nb, first );
		GOL::disable_client_state_normal();
		GOL::disable_client_state_texcoor();
	}
	static	FINLINE void	draw_arrays_color3(	FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb,					FP32 CONST * CONST color )
	{
		GOL::unbind_vao_secu();
		GOL::enable_client_state_color();
			GOL::set_pointer_color3( color );
			draw_arrays( vertex, mode, nb );
		GOL::disable_client_state_color();
	}
	static	FINLINE void	draw_arrays_color3(	FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb, INT32 CONST first, FP32 CONST * CONST color )
	{
		GOL::unbind_vao_secu();
		GOL::enable_client_state_color();
			GOL::set_pointer_color3( color );
			draw_arrays( vertex, mode, nb, first );
		GOL::disable_client_state_color();
	}
	static	FINLINE void	draw_arrays_color4(	FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb,					FP32 CONST * CONST color )
	{
		GOL::unbind_vao_secu();
		GOL::enable_client_state_color();
			GOL::set_pointer_color4( color );
			draw_arrays( vertex, mode, nb );
		GOL::disable_client_state_color();
	}
	static	FINLINE void	draw_arrays_color4(	FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb, INT32 CONST first, FP32 CONST * CONST color )
	{
		GOL::unbind_vao_secu();
		GOL::enable_client_state_color();
			GOL::set_pointer_color4( color );
			draw_arrays( vertex, mode, nb, first );
		GOL::disable_client_state_color();
	}
protected:
	static	FINLINE void	draw_oldschool(	FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb, INT32 CONST first )
	{
		GOL::begin( mode );
			INT32 last = first + nb;
			for( INT32 i = first; i < last; ++i )
				SUPER::draw_vertex_one( vertex, i );
		GOL::end();
	}
public:
	static	FINLINE void	draw(					FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb )
	{
		if( GOL::b_draw_avoid_vertex_use )
			draw_arrays(	vertex, mode, nb );
		else
			draw_oldschool( vertex, mode, nb, 0	);
	}
	static	FINLINE void	draw(					FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb, INT32 CONST first )
	{
		if( GOL::b_draw_avoid_vertex_use )
			draw_arrays(	vertex, mode, nb, first );
		else
			draw_oldschool( vertex, mode, nb, first	);
	}
protected:
	static			void	draw_uv_oldschool(		FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb, INT32 CONST first, FP32 CONST * CONST uv )
	{
		GOL::begin( mode );
			INT32 last = first + nb;
			for( INT32 i = first; i < last; ++i )
			{
				GOL::texcoor2v( uv + i*2 );
				SUPER::draw_vertex_one( vertex, i );
			}
		GOL::end();
	}
public:
	static	FINLINE void	draw_uv(			FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb, FP32 CONST * CONST uv )
	{
		if( GOL::b_draw_avoid_vertex_use )
			draw_arrays_uv(		vertex, mode, nb,		uv );
		else
			draw_uv_oldschool(	vertex, mode, nb, 0,	uv );
	}
	static	FINLINE void	draw_uv(			FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb, INT32 CONST first, FP32 CONST * CONST uv )
	{
		if( GOL::b_draw_avoid_vertex_use )
			draw_arrays_uv(		vertex, mode, nb, first,	uv );
		else
			draw_uv_oldschool(	vertex, mode, nb, first,	uv );
	}
protected:
	static			void	draw_normal_oldschool(	FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb, INT32 CONST first, FP32 CONST * CONST normal )
	{
		GOL::begin( mode );
			INT32 last = first + nb;
			for( INT32 i = first; i < last; ++i )
			{
				GOL::normal3v( normal + i*3 );
				SUPER::draw_vertex_one( vertex, i );
			}
		GOL::end();
	}
public:
	static	FINLINE void	draw_normal(			FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb, FP32 CONST * CONST normal )
	{
		if( GOL::b_draw_avoid_vertex_use )
			draw_arrays_normal(		vertex, mode, nb,			normal );
		else
			draw_normal_oldschool(	vertex, mode, nb, 0,		normal );
	}
	static	FINLINE void	draw_normal(			FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb, INT32 CONST first, FP32 CONST * CONST normal )
	{
		if( GOL::b_draw_avoid_vertex_use )
			draw_arrays_normal(		vertex, mode, nb, first,	normal );
		else
			draw_normal_oldschool(	vertex, mode, nb, first,	normal );
	}
protected:
	static			void	draw_uv_normal_oldschool(	FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb, INT32 CONST first, FP32 CONST * CONST uv, FP32 CONST * CONST normal )
	{
		GOL::begin( mode );
			INT32 last = first + nb;
			for( INT32 i = first; i < last; ++i )
			{
				GOL::texcoor2v( uv + i*2 );
				GOL::normal3v( normal + i*3 );
				SUPER::draw_vertex_one( vertex, i );
			}
		GOL::end();
	}
public:
	static	FINLINE void	draw_uv_normal(			FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb, FP32 CONST * CONST uv, FP32 CONST * CONST normal )
	{
		if( GOL::b_draw_avoid_vertex_use )
			draw_arrays_uv_normal(		vertex, mode, nb,			uv, normal );
		else
			draw_uv_normal_oldschool(	vertex, mode, nb, 0,		uv, normal );
	}
	static	FINLINE void	draw_uv_normal(			FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb, INT32 CONST first, FP32 CONST * CONST uv, FP32 CONST * CONST normal )
	{
		if( GOL::b_draw_avoid_vertex_use )
			draw_arrays_uv_normal(		vertex, mode, nb, first,	uv, normal );
		else
			draw_uv_normal_oldschool(	vertex, mode, nb, first,	uv, normal );
	}
protected:
	static			void	draw_color3_oldschool(	FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb, INT32 CONST first, FP32 CONST * CONST color )
	{
		GOL::begin( mode );
			INT32 last = first + nb;
			for( INT32 i = first; i < last; ++i )
			{
				GOL::color3v( color + i*3 );
				SUPER::draw_vertex_one( vertex, i );
			}
		GOL::end();
	}
public:
	static	FINLINE void	draw_color3(		FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb, FP32 CONST * CONST color )
	{
		if( GOL::b_draw_avoid_vertex_use )
			draw_arrays_color3(		vertex, mode, nb,		color );
		else
			draw_color3_oldschool(  vertex, mode, nb, 0,	color );
	}
	static	FINLINE void	draw_color3(		FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb, INT32 CONST first, FP32 CONST * CONST color )
	{
		if( GOL::b_draw_avoid_vertex_use )
			draw_arrays_color3(		vertex, mode, nb, first, color );
		else
			draw_color3_oldschool(  vertex, mode, nb, first, color );
	}
protected:
	static			void	draw_color4_oldschool(	FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb, INT32 CONST first, FP32 CONST * CONST color )
	{
		GOL::begin( mode );
			INT32 last = first + nb;
			for( INT32 i = first; i < last; ++i )
			{
				GOL::color4v( color + i*4 );
				SUPER::draw_vertex_one( vertex, i );
			}
		GOL::end();
	}
public:
	static	FINLINE void	draw_color4(		FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb, FP32 CONST * CONST color )
	{
		if( GOL::b_draw_avoid_vertex_use )
			draw_arrays_color4(		vertex, mode, nb,		color );
		else
			draw_color4_oldschool(  vertex, mode, nb, 0,	color );
	}
	static	FINLINE void	draw_color4(		FP32 CONST * CONST vertex, GLenum CONST mode, INT32 CONST nb, INT32 CONST first, FP32 CONST * CONST color )
	{
		if( GOL::b_draw_avoid_vertex_use )
			draw_arrays_color4(		vertex, mode, nb, first, color );
		else
			draw_color4_oldschool(  vertex, mode, nb, first, color );
	}

	FINLINE void draw(		  GLenum CONST mode, INT32 CONST nb )												CONST {	draw(		 SUPER::_vertex, mode, nb			);	}
	FINLINE void draw(		  GLenum CONST mode, INT32 CONST nb, INT32 CONST first )							CONST {	draw(		 SUPER::_vertex, mode, nb, first	);	}
				 			 																							  				 
//	FINLINE void draw_uv(	  GLenum CONST mode, INT32 CONST nb	)												CONST {	draw_uv(	 SUPER::_vertex,  mode, nb,			SUPER::_uv	);	}
//	FINLINE void draw_uv(	  GLenum CONST mode, INT32 CONST nb, INT32 CONST first	)							CONST {	draw_uv(	 SUPER::_vertex,  mode, nb, first,	SUPER::_uv	);	}
	FINLINE void draw_uv(	  GLenum CONST mode, INT32 CONST nb,					FP32 CONST * CONST uv )		CONST {	draw_uv(	 SUPER::_vertex,  mode, nb,			uv			);	}
	FINLINE void draw_uv(	  GLenum CONST mode, INT32 CONST nb, INT32 CONST first,	FP32 CONST * CONST uv )		CONST {	draw_uv(	 SUPER::_vertex,  mode, nb, first,	uv			);	}
				 			 																							  				 
	FINLINE void draw_normal( GLenum CONST mode, INT32 CONST nb,					FP32 CONST * CONST normal )	CONST {	draw_normal( SUPER::_vertex,  mode, nb,			normal	);	}
	FINLINE void draw_normal( GLenum CONST mode, INT32 CONST nb, INT32 CONST first,	FP32 CONST * CONST normal )	CONST {	draw_normal( SUPER::_vertex,  mode, nb, first,	normal	);	}
				 			 																							  				 
	FINLINE void draw_color3( GLenum CONST mode, INT32 CONST nb						)							CONST {	draw_color3( SUPER::_vertex,  mode, nb,			SUPER::_color	);	}
	FINLINE void draw_color3( GLenum CONST mode, INT32 CONST nb, INT32 CONST first	)							CONST {	draw_color3( SUPER::_vertex,  mode, nb, first,	SUPER::_color	);	}
	FINLINE void draw_color3( GLenum CONST mode, INT32 CONST nb,					FP32 CONST * CONST color )	CONST {	draw_color3( SUPER::_vertex,  mode, nb,			color			);	}
	FINLINE void draw_color3( GLenum CONST mode, INT32 CONST nb, INT32 CONST first,	FP32 CONST * CONST color )	CONST {	draw_color3( SUPER::_vertex,  mode, nb, first,	color			);	}
				 			 																							  				 
	FINLINE void draw_color4( GLenum CONST mode, INT32 CONST nb						)							CONST {	draw_color4( SUPER::_vertex,  mode, nb,			SUPER::_color	);	}
	FINLINE void draw_color4( GLenum CONST mode, INT32 CONST nb, INT32 CONST first	)							CONST {	draw_color4( SUPER::_vertex,  mode, nb, first,	SUPER::_color	);	}
	FINLINE void draw_color4( GLenum CONST mode, INT32 CONST nb,					FP32 CONST * CONST color )	CONST {	draw_color4( SUPER::_vertex,  mode, nb,			color			);	}
	FINLINE void draw_color4( GLenum CONST mode, INT32 CONST nb, INT32 CONST first,	FP32 CONST * CONST color )	CONST {	draw_color4( SUPER::_vertex,  mode, nb, first,	color			);	}
};

typedef	c_prim< 2 >			c_prim2;
typedef	c_prim< 3 >			c_prim3;
