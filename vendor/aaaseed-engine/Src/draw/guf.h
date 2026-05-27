
#ifdef AAA_GUF_H
#error "GUF_H included more than once."
#endif
#define AAA_GUF_H 1


#ifndef	AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif
#ifndef	AAA_GOL_DRAW_H
#	include "gol/gol_draw.h"
#endif

//	wrap glDrawArrays
class c_guf : public c_obj
{
private:
//	Quick optimization would be faster without the copy but this require serious work on allocation
//todo do the same for more bdds and draw fns
	FP32*	_point;
	FP32*	_normal;
	FP32*	_uv;
	UINT32	_point_nb;

	bool	_b_owner;

			void	init();
			void	dealloc_point_min();	//private because maa optimized
			bool	alloc_point_low(	INT32  CONST nb_point,	C_PCHAR_C signature );
	static bool b_normal_set;
	static bool b_uv_set;
protected:
			void	dealloc_point();
	static FINLINE	void	draw_before_normal_uv_static( FP32 CONST * CONST normal, FP32 CONST * CONST uv );

public:
	c_guf();
	c_guf( INT32 CONST nb_point );
	virtual ~c_guf();

	FINLINE	bool	alloc_point(	UINT32 CONST nb_point,	C_PCHAR_C signature );
			void	set_data(	FP32* CONST point,	FP32* CONST normal, FP32* CONST uv );

	FINLINE	UINT32	get_nb()		CONST	{	return _point_nb;	}
	FINLINE	FP32*	get_point()		CONST	{	return _point;		}
	FINLINE	FP32*	get_normal()	CONST	{	return _normal;		}
	FINLINE	FP32*	get_uv()		CONST	{	return _uv;			}

	static	FINLINE	void	draw_before_static(	FP32 CONST * CONST point,	FP32 CONST * CONST normal,	FP32 CONST * CONST uv );
			FINLINE	void	draw_before(		FP32 CONST * CONST point,	FP32 CONST * CONST normal,	FP32  CONST * CONST uv );
			FINLINE	void	draw_before(		bool CONST b_normal,	bool CONST b_uv );

	static	FINLINE	void	draw_after_static();
			FINLINE	void	draw_after();
	
			FINLINE	void	draw_low(	INT32 CONST mode, INT32 CONST nb )  CONST
			{
				GOL::draw_arrays( mode, nb );
			}
			FINLINE	void	draw(				INT32 CONST mode, INT32 CONST nb,				bool CONST b_normal, bool CONST b_uv );
			FINLINE	void	draw_index_UINT8(	INT32 CONST mode, INT32 CONST nb, void* index );
};

FINLINE	bool	c_guf::alloc_point(	UINT32 CONST nb_point, C_PCHAR_C signature ) 
{
	if( _b_owner )
	{
		if( nb_point <= _point_nb )
			return true;
		return alloc_point_low( nb_point, signature );
	}
	else
		_point_nb = nb_point;
	return true;
}

FINLINE	void	c_guf::draw_before_normal_uv_static( FP32 CONST * CONST normal, FP32 CONST * CONST uv )
{
	b_normal_set = (normal != nullptr);
	if( normal )
	{
		GOL::enable_client_state_normal();
		GOL::set_pointer_normal( normal );
	}
	else
		GOL::disable_client_state_normal();

	b_uv_set = (uv != nullptr);
	if( uv )
	{
		GOL::enable_client_state_texcoor();
		GOL::set_pointer_texcoor( 2, GOL_REAL, uv );
	}
	else
		GOL::disable_client_state_texcoor();
}

FINLINE	void	c_guf::draw_before_static( FP32 CONST * CONST point, FP32 CONST * CONST normal, FP32 CONST * CONST uv )
{
	//GOL::enable_client_state_vertex();
	GOL::set_pointer_vertex3( point );
	draw_before_normal_uv_static( normal, uv );
}

FINLINE	void	c_guf::draw_before( FP32 CONST * CONST point, FP32 CONST * CONST normal, FP32 CONST * CONST uv )
{
	draw_before_static( point, normal, uv );
}

FINLINE	void	c_guf::draw_before( bool CONST b_normal, bool CONST b_uv )
{
	draw_before_static( _point, b_normal ? _normal : nullptr, b_uv ? _uv : nullptr );
}

FINLINE	void	c_guf::draw_after_static()
{
	if( b_normal_set )
		GOL::disable_client_state_normal();
	if( b_uv_set )
		GOL::disable_client_state_texcoor();
}
FINLINE	void	c_guf::draw_after()
{
	draw_after_static();
}


FINLINE	void	c_guf::draw( INT32 CONST mode, INT32 CONST nb, bool CONST b_normal, bool CONST b_uv )
{
	draw_before( b_normal, b_uv );
	draw_low( mode, nb );
	draw_after();
}

FINLINE	void	c_guf::draw_index_UINT8(	INT32 CONST mode, INT32 CONST nb, void* index )
{
	GOL::disable_client_state_normal();
	GOL::disable_client_state_texcoor();
	GOL::set_pointer_vertex3( _point );
	GOL::draw_elements( mode, nb, GL_UNSIGNED_BYTE, index );
}

class c_guf_index final : public c_guf
{
private:
	INT32	_nb_u;
	INT32	_nb_v;
	UINT32	_index_nb;
	void*	_index;
	GLenum	_gl_type;
	bool	_b_triangle;

	void init();
public:
	c_guf_index();
	c_guf_index( INT32 CONST nb_u, INT32 CONST nb_v );
	virtual ~c_guf_index();

	bool	alloc_for_strip(		INT32 CONST nb_u, INT32 CONST nb_v, C_PCHAR_C signature );
	bool	alloc_for_triangles(	INT32 CONST nb_u, INT32 CONST nb_v, C_PCHAR_C signature );
	void	dealloc();

	FINLINE	INT32	get_nb_u()	CONST	{	return _nb_u;	}
	FINLINE	INT32	get_nb_v()	CONST	{	return _nb_v;	}

//	FINLINE	void	draw_using_index(	INT32 CONST mode );
	FINLINE	void	draw_as_strip(		INT32 CONST mode ) CONST;
	FINLINE	void	draw_as_triangles() CONST;
//	FINLINE	void	draw(				INT32 CONST mode, bool b_normal, bool b_uv );

	template< typename T >
	static	void	build_index_strip_one(			INT32 CONST nb_u,					void* dst );
	template< typename T >
	static	void	build_index_triangles(			INT32 CONST nb_u, INT32 CONST nb_v,	void* dst );
//	static	void	build_index_strip_one_along_v(	INT32 CONST nb_u, INT32 CONST nb_v,	void* dst );

	// suppose that point are stored along u first
	static	void	build_index_strip_along_u(		INT32 CONST nb_u, INT32       nb_v,	UINT32* dst );
	static	void	build_index_strip_along_v(		INT32 CONST nb_u, INT32 CONST nb_v,	UINT32* dst );
	static	void	build_index_for_hexa(			INT32 CONST nb_u, INT32 CONST nb_v,	UINT32* dst );
};

/*
FINLINE	void	c_guf_index::draw_using_index( INT32 mode )
{
	INT32	nbu		= _nb_u*2;
	UINT32*	index	= _index;
	for( INT32 iv=_nb_v-1; iv>0; --iv )
	{
		GOL::draw_elements( mode, nbu, index );
		index += nbu;
	}
}
*/

template< typename T >
void	c_guf_index::build_index_strip_one( INT32 CONST nb_u, void* dst )
{
	T* p = ((T*) dst) - 1;
	for( INT32 iu=0; iu<nb_u; ++iu )
	{
		*++p = iu + nb_u;
		*++p = iu;
	}
}

template< typename T >
void	c_guf_index::build_index_triangles( INT32 CONST nb_u, INT32 CONST nb_v, void* dst )
{
	T* p = ((T*) dst) - 1;
	INT32 ind = 0;
	for( INT32 iv = 1; iv < nb_v; ++iv )
	{
		for( INT32 iu = 1; iu < nb_u; ++iu )
		{
			*++p = ind;
			*++p = ind + 1;
			*++p = ind + nb_u;

			*++p = ind + 1 ;
			*++p = ind + nb_u + 1;
			*++p = ind + nb_u;

			++ind;
		}
		++ind;
	}
}

FINLINE	void	c_guf_index::draw_as_strip( INT32 CONST mode ) CONST
{
	INT32	nbu		= _nb_u*2;
	INT32	off		= 0;
	for( INT32 iv=_nb_v-1; iv>0; --iv )
	{
		GOL::draw_elements_base_vertex( mode, nbu, off, _gl_type, _index  );
		off += _nb_u;
	}
}

FINLINE	void	c_guf_index::draw_as_triangles() CONST
{
	GOL::draw_elements( GL_TRIANGLES, (_nb_u-1)*(_nb_v-1)*6, _gl_type, _index  );
}

/*
FINLINE	void	c_guf_index::draw( INT32 mode, bool b_normal, bool b_uv )
{
	draw_before( b_normal, b_uv );
	draw_using_index( mode );
	draw_after();
}
*/

