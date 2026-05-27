#include "polyline.h"
#include "gol/gol.h"
#include "draw/map.h"
#include "draw/primitive.h"

//should be a bdd_uv or at least avoid double work
//todo we should draw point too
void	c_polyline_2d::draw_line() CONST
{
	INT32 CONST nb = get_point_nb();
	FP32* vec = c_prim2::base.alloc_vertex( nb );
	--vec;

	for( INT32 i=0; i<nb; ++i )
	{
		st_point_2d CONST & pt = _points[i];
		*++vec = pt.x;
		*++vec = pt.y;
	}
	c_prim2::base.draw( is_closed() ? GL_LINE_LOOP : GL_LINE_STRIP, nb );
}

void	c_polyline_2d::scale_translate( REAL CONST * CONST scale, REAL CONST * CONST translate )
{
	INT32 CONST nb = get_point_nb();	
	for( INT32 i=0; i<nb; ++i )
	{
		st_point_2d & pt = _points[i];
		pt.x = translate[0] + pt.x*scale[0];
		pt.y = translate[1] + pt.y*scale[1];
	}
}

bool 	c_polyline_2d::alloc_strip()
{
	if( alloc_uv() )
	{
		_triangle_strip = (REAL*) REALLOC( _triangle_strip, (sizeof(REAL)*2) * 2 * (get_point_nb()+1) );	// do like _uv will be more simple to draw in less call 
		return _triangle_strip != nullptr;
	}
	return false;
}


void	compute_point( REAL* dst, st_point_2d* prev, st_point_2d* cur, st_point_2d* next, REAL side_size, bool b_constant_width )
{
	REAL	nor[2];
	nor[0] = - next->y + prev->y;
	nor[1] =   next->x - prev->x;

	if( b_constant_width )
	{	// to deal with 45 degree roundoff only
		if( nor[0] < 0 )
		{
			nor[0] = -nor[0];
			nor[1] = -nor[1];
			side_size = -side_size;
		}
		REAL tmp = nor[0]*REAL(.5);
		if( nor[1] <= tmp )
		{
			if( nor[1] >= -tmp )
			{
				nor[0] = side_size;
				nor[1] = 0.;
			}
			else if( nor[1] > -2.*nor[0] )
			{
				nor[0] = side_size;
				nor[1] = -side_size;
			}
			else
			{
				nor[0] = 0.;
				nor[1] = -side_size;
			}
		}
		else if( nor[1] > 2.*nor[0] )
		{
			nor[0] = 0.;
			nor[1] = side_size;
		}
		else
		{
			nor[0] = side_size;
			nor[1] = side_size;
		}
	}
	else
	{
		normalize_scale_v2r( nor, side_size );
	}
	*dst	= cur->x - nor[0];
	*++dst	= cur->y - nor[1];

	*++dst	= cur->x + nor[0];
	*++dst	= cur->y + nor[1];	
}

void	c_polyline_2d::draw_quad( bool b_constant_width )
{
	build_strip( b_constant_width );

	INT32 nb = get_point_nb();
	if( is_closed() )
		++nb;
	if( nb < 2 )
		return;

	REAL* pt = _triangle_strip;
	GOL::begin( GL_TRIANGLE_STRIP );
		if( c_layer::get_cur()->is_need_uv() )
		{
			REAL*	puv = _uv;
			for( INT32 i=0; i<nb; ++i )
			{
				GOL::texcoord2v( puv );
				GOL::vertex2v( pt );
				pt += 2;
				puv += 2;

				GOL::texcoord2v( puv );
				GOL::vertex2v( pt );
				pt += 2;
				puv += 2;
			}
		}
		else		
		{
			for( INT32 i=0; i<nb; ++i )
			{
				GOL::vertex2v( pt );
				pt += 2;
				GOL::vertex2v( pt );
				pt += 2;
			}
		}
	GOL::end();
}
