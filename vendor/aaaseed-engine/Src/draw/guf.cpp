#include "guf.h"
#include "aaa_mem.h"

bool c_guf::b_normal_set = false;
bool c_guf::b_uv_set	 = false;


void	c_guf::init()
{
	_point_nb	= 0;
	_normal		= _point		= _uv			= nullptr;
	_b_owner	= true;
}

void	c_guf::dealloc_point_min()
{
	_point_nb = 0;
	if( _point )
	{
		FREE_ALIGNED_AND_NULL( _point );
		IF_FREE_ALIGNED_AND_NULL( _normal );
		IF_FREE_ALIGNED_AND_NULL( _uv );
	}
}

void	c_guf::dealloc_point()
{
	if( _b_owner )
		dealloc_point_min();
	else
	{
		_point_nb	= 0;
		_normal		= _point		= _uv			= nullptr;
	}
}

void	c_guf::set_data( FP32 * CONST point, FP32 * CONST normal, FP32 * CONST uv )
{
	if( _b_owner )
	{
		dealloc_point_min();
		_b_owner	= false;
	}
	else
	{
		_point		= point;
		_normal		= normal;
		_uv			= uv;
	}
}
bool	c_guf::alloc_point_low( INT32 CONST nb_point, C_PCHAR_C signature )
{
	_point		= (FP32*) REALLOC_ALIGNED_SIGNATURE( _point,	nb_point * 3 * sizeof(FP32), signature );
	_normal		= (FP32*) REALLOC_ALIGNED_SIGNATURE( _normal,	nb_point * 3 * sizeof(FP32), signature );
	_uv			= (FP32*) REALLOC_ALIGNED_SIGNATURE( _uv,		nb_point * 2 * sizeof(FP32), signature );
	if( !_point || !_normal || !_uv )
	{
		dealloc_point_min();
		return false;
	}
	_point_nb	= nb_point;
	return true;
}

c_guf::c_guf()
{
	init();
}

c_guf::c_guf( INT32 CONST nb_point )
{
	init();
	alloc_point( nb_point, __FUNCTION__ );
}

c_guf::~c_guf()
{
	dealloc_point();
	//todo dealloc index
}


//
//	GUF_INDEX
//
void	c_guf_index::build_index_strip_along_u( INT32 CONST nb_u, INT32 CONST nb_v, UINT32* dst )
{
	--dst;
	UINT32 nb = nb_u * (nb_v - 1);
	for( UINT32 ind = 0; ind < nb; ++ind )
	{
		*++dst = ind + nb_u;
		*++dst = ind;
	}
}

void	c_guf_index::build_index_strip_along_v( INT32 CONST nb_u, INT32 nb_v, UINT32* dst )
{
	--dst;
	for( INT32 iu = 0; iu < nb_u-1; ++iu )
	{
		UINT32 ind = iu;
		for( INT32 iv = nb_v; iv > 0; --iv )
		{
			*++dst = ind;
			*++dst = ind + 1;
			ind += nb_u;
		}
	}
}

void	c_guf_index::build_index_for_hexa( INT32 CONST nb_u, INT32 CONST nb_v, UINT32* dst )
{
	--dst;
	INT32 ind = 0;
	for( INT32 iv = 1; iv < nb_v; ++iv )
	{ 
		bool b = (iv & 1) == 1;
		for( INT32 iu = 1; iu < nb_u; ++iu )
		{
			
			*++dst = ind + nb_u;
			*++dst = ind;
			if( b )
			{
				*++dst = ind + 1		;
				*++dst = ind + nb_u		;
				*++dst = ind + 1		;
			}
			else
			{
				*++dst = ind + nb_u + 1	;
				*++dst = ind			;
				*++dst = ind + 1		;
			}
			*++dst = ind + nb_u + 1;
			b = !b;
			++ind;
		}
		++ind;
	}
}

bool	c_guf_index::alloc_for_strip( INT32 CONST nb_u, INT32 CONST nb_v, C_PCHAR_C signature )
{
	if( _nb_u == nb_u && _nb_v == nb_v && _b_triangle == false )
		return true;
	if( c_guf::alloc_point( nb_u * nb_v, signature ) )
	{
		_b_triangle = false;
		//INT32 nb_index = MAX(		( nb_v-1 ) * 2,			//	ONE QUAD_STRIP				all quad strip is nb_u * ( nb_v-1 ) * 2,
		//							( nb_u-1 ) * 2			//	ONE QUAD_STRIP OTHER WAY	all quad strip is nb_v * ( nb_u-1 ) * 2
		//					);
		INT32 nb_index = nb_u * 2;			//	one strip
		if		( nb_index <= 256 )
			_gl_type = GL_UNSIGNED_BYTE;
		else if ( nb_index <= (256 * 256) )
			_gl_type = GL_UNSIGNED_SHORT;
		else
			_gl_type = GL_UNSIGNED_INT;

		UINT32 byte_nb = GOL::get_byte_nb_from_type_enum( _gl_type );
		_index = (void*) REALLOC_ALIGNED_SIGNATURE( _index, nb_index * byte_nb, signature );
		if( _index )
		{
			_nb_u = nb_u;
			_nb_v = nb_v;
			switch(_gl_type)
			{
			case GL_UNSIGNED_BYTE:	build_index_strip_one< UINT8  > ( nb_u, _index );	break;
			case GL_UNSIGNED_SHORT:	build_index_strip_one< UINT16 > ( nb_u, _index );	break;
			case GL_UNSIGNED_INT:	build_index_strip_one< UINT32 > ( nb_u, _index );	break;
			default:
				break;
			}
			_index_nb = nb_index;
			return true;
		}
	}
	dealloc();
	return false;
}

bool	c_guf_index::alloc_for_triangles( INT32 CONST nb_u, INT32 CONST nb_v, C_PCHAR_C signature )
{
	if( _nb_u == nb_u && _nb_v == nb_v && _b_triangle == true )
		return true;
	if( c_guf::alloc_point( nb_u * nb_v, signature ) )
	{
		_b_triangle = true;

		INT32 nb_index = ( nb_u-1 ) * ( nb_v-1 ) * 6;
		INT32 index_max = nb_u * nb_v - 1;
		if		( index_max < 256 )
			_gl_type = GL_UNSIGNED_BYTE;
		else if ( index_max < (256 * 256) )
			_gl_type = GL_UNSIGNED_SHORT;
		else
			_gl_type = GL_UNSIGNED_INT;

		UINT32 byte_nb = GOL::get_byte_nb_from_type_enum( _gl_type );
		_index = (void*) REALLOC_ALIGNED_SIGNATURE( _index, nb_index * byte_nb, signature );
		if( _index )
		{
			_nb_u = nb_u;
			_nb_v = nb_v;
			switch(_gl_type)
			{
			case GL_UNSIGNED_BYTE:	build_index_triangles< UINT8  > ( nb_u, nb_v, _index );	break;
			case GL_UNSIGNED_SHORT:	build_index_triangles< UINT16 > ( nb_u, nb_v, _index );	break;
			case GL_UNSIGNED_INT:	build_index_triangles< UINT32 > ( nb_u, nb_v, _index );	break;
			default:
				break;
			}
			_index_nb = nb_index;
			return true;
		}
	}
	dealloc();
	return false;
}
/*
bool	c_guf_index::alloc( INT32 nb_u, INT32 nb_v, CHAR* signature )
{
	if( _nb_u == nb_u && _nb_v == nb_v )
		return true;
	if( c_guf::alloc( nb_u * nb_v, signature ) )
	{
		INT32 nb_index = aaa::MAX(	nb_u * ( nb_v-1 ) * 2,				//	QUAD_STRIP
									nb_v * ( nb_u-1 ) * 2,				//	QUAD_STRIP OTHER WAY
									( nb_u-1 ) * ( nb_v-1 ) * 2 * 3		//	TRIANGLES
							);
		_index = (UINT32*) REALLOC_ALIGNED_SIGNATURE( _index, nb_index * sizeof(INT32), signature );
		if( _index )
		{
			_nb_u = nb_u;
			_nb_v = nb_v;			
			build_index_strip( nb_u, nb_v, _index );
			_nb_index = nb_index;
			return true;
		}
	}
	dealloc();
	return false;
}
*/

void	c_guf_index::dealloc()
{
	FREE_ALIGNED_AND_NULL( _index );
	_index_nb = 0;
	dealloc_point();
}

void	c_guf_index::init()
{
	_nb_u	= 0;
	_nb_v	= 0;
	_index	= nullptr;
}

c_guf_index::c_guf_index()
{
	init();
}

c_guf_index::c_guf_index( INT32 CONST nb_u, INT32 CONST nb_v )
{
	init();
	alloc_for_strip( nb_u, nb_v, __FUNCTION__ );
}

c_guf_index::~c_guf_index()
{
	dealloc();
}




