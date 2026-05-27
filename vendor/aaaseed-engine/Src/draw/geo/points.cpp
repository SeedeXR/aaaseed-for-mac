#include "points.h"
#include "obj_ui/deformer/def_node.h"
#include "obj_ui/bdd/util/bdd.h"
#include "file/file_csv.h"


c_points_1d::c_points_1d()
:_nb(0)
,_points(nullptr)
{
}

c_points_1d::~c_points_1d()
{
	dealloc();
}

bool	c_points_1d::alloc( UINT32 nb )
{
	if( nb == _nb )
		return true;

	_points = (REAL*) REALLOC_ALIGNED( (void*)_points, nb * sizeof(REAL) );
	if( _points || nb==0 )
	{
		_nb = nb;
		return true;
	}
	else
	{
		_nb = 0;
		err_print( "can't allocate %d points 1d", nb );
		return false;
	}
}

void c_points_1d::dealloc()
{
	IF_FREE_ALIGNED_AND_NULL( _points );
}

REAL	c_points_1d::remap( REAL u )
{
	if( _nb )
	{
		UINT32 iu;
		REAL a = _points[0];
		REAL b;
		for( iu=1; iu<_nb; ++iu )
		{
			b = _points[iu];
			if( a<=u && u<b )
			{
				u = (u-a) / (b-a) + REAL(iu-1);
				break;
			}
			a = b;
		}
	}
	return u; 
}

void	c_points_1d::write_float_in_file( C_PCHAR_C name )
{
	file_csv::write_real_grid( name, _points, _nb, 1 );
}

void	c_points_1d::read_float_from_file( C_PCHAR_C name )
{
	UINT32	x;
	UINT32	y;
	file_csv::count_float( name, x,y );
	if( y==1 )
	{
		alloc( x );
		file_csv::read_float_grid( name, _points, x, 1 );
	}
	else
	{
		alloc( 0 );
	}
}


c_points_3d::c_points_3d()
	:_nb			(0)
	,_nb_alloc		(0)
	,_points		(nullptr)
	,_points_def	(nullptr)
	,_points_draw	(nullptr)
	,_normals		(nullptr)
{
}

c_points_3d::~c_points_3d()
{
	dealloc();
}

bool	c_points_3d::alloc( UINT32 nb )
{
	UINT32	tmp = nb * 3;
	if( nb > _nb_alloc )
	{
		_points = (REAL*) REALLOC_ALIGNED( (void*)_points, tmp * 3 * sizeof(REAL) );
	}
	if( _points )
	{
		_nb = nb;
		_nb_alloc = MAX( _nb_alloc, nb );
		_points_def = _points + tmp;
		_normals = _points_def + tmp;
		_points_draw = _points;
		return true;
	}
	else
	{
		_nb = 0;
		_points_def = nullptr;
		_normals = nullptr;
		_points_draw = nullptr;
		err_print( "can't allocate points" );
		return false;
	}
}

void	c_points_3d::dealloc()
{
	IF_FREE_ALIGNED_AND_NULL( _points );
	_points_def = nullptr;
	_points_draw = nullptr;
}

void	c_points_3d::deform()
{
	//hack don't work if using object don't call it in his update
	auto def = c_def_node::get_cur();
	if( def->is_deforming() )
	{
		def->apply( _points_def, _points, _nb );
		_points_draw = _points_def;
	}
	else
		_points_draw = _points;
}

void	c_points_3d::save_obj_file_points( FILE* CONST file )
{
	c_bdd::save_obj_file_points( file, _points_draw, _nb );
}

c_points_uv::c_points_uv()
:_uv(nullptr)
{
}

c_points_uv::~c_points_uv()
{
	dealloc();
}

bool	c_points_uv::alloc( UINT32 nb )
{
	c_points_3d::alloc( nb );
	if( nb==get_nb() )
	{
		_uv = (REAL*) REALLOC_ALIGNED( (void*)_uv, nb*2*sizeof(REAL) );
		if( _uv )	// ok
			return true;
	}
	dealloc();
	return false;
}

void	c_points_uv::dealloc()
{
	c_points_3d::dealloc();
	IF_FREE_ALIGNED_AND_NULL( _uv );
}



c_grid_point3d::c_grid_point3d()
:_nb_u(0)
,_nb_v(0)
,_points(nullptr)
,_nb_alloc(0)
{
}

c_grid_point3d::~c_grid_point3d()
{
	dealloc();
}

REAL*	c_grid_point3d::get_point( UINT32 CONST iu, UINT32 CONST iv )	CONST
{
	if( iu >= (UINT32)_nb_u || iv >= (UINT32)_nb_v )
	{
		err_print( "%s() index %d,%d too big, returning nullptr.", __FUNCTION__, iu,iv );
		return nullptr;
	}
	return get_point_direct( iu, iv );
}
bool	c_grid_point3d::alloc( UINT32 nb_u, UINT32 nb_v, bool b_clear )
{
	UINT32	nb_real = nb_u * nb_v * 3;
	if( nb_real > _nb_alloc )
	{
		//ERR_PRINT_STRING( "%s() before alloc", __FUNCTION__ );
		_points = (REAL*) REALLOC_ALIGNED( (void*)_points, nb_real * sizeof(REAL), 32 );
		//ERR_PRINT_STRING( "%s() after alloc", __FUNCTION__ );
	}
	if( _points )
	{
		_nb_u = nb_u;
		_nb_v = nb_v; 
		_nb_alloc = MAX( _nb_alloc, nb_real );
		if( b_clear )
			clear_vr( _points, nb_real );
		return true;
	}
	_nb_u = 0;
	_nb_v = 0;
	_nb_alloc = 0;
	err_print( "%s() can't allocate points", __FUNCTION__ );
	return false;
}

void	c_grid_point3d::dealloc()
{
	IF_FREE_ALIGNED_AND_NULL( _points );
	_nb_u = 0;
	_nb_v = 0;
}

void	c_grid_point3d::copy_from( c_grid_point3d CONST * CONST src )
{
	set_nb( src->get_nb_u(),  src->get_nb_v(), false );
	MEMCPY( _points, src->_points, _nb_alloc * sizeof(REAL), __FUNCTION__ );
}

void	c_grid_point3d::write_float_in_file( C_PCHAR_C name )
{
	file_csv::write_real_grid( name, _points, _nb_u * 3, _nb_v );
}

void	c_grid_point3d::read_float_from_file( C_PCHAR_C name )
{
	file_csv::read_float_grid( name, _points, _nb_u * 3, _nb_v );
}

void	c_grid_point3d::curve_read_float_from_file( C_PCHAR_C name )
{
	//INT32	nb_read = 0;
	//nb_read = file_csv::read_float( name, _points, _nb_u * 3, _nb_v );
	//alloc( nb_read / _nb_v / 3, _nb_v, false );
	UINT32	x;
	UINT32	y;
	file_csv::count_float( name, x,y );
	if( y == 1 )
	{
		alloc( x / 3 , 1 );
		file_csv::read_float_grid( name, _points, x, 1 );
	}
	//else
	//{
	//	alloc( 0, 0 );
	//}
}

/*
void	c_grid_point3d::save_obj_file_points( FILE* file )
{
	c_bdd::save_obj_file_points( file, _points_draw, _nb );
}
*/
FINLINE	void	line_get_equation_coef( REAL& a, REAL& b, REAL& c, REAL* pta, REAL* ptb )
{
	REAL xa = pta[0];
	REAL ya = pta[1];
	REAL xb = ptb[0];
	REAL yb = ptb[1];

	a = ya-yb;
	b = xa-xb;
	c = xa*yb - xb*ya ;
}

FINLINE	REAL	line_side( REAL* pta, REAL* ptb, REAL CONST * CONST vec )
{
	REAL a, b, c;
	line_get_equation_coef( a, b, c, pta, ptb );
	return a*vec[0]+ b*vec[1] + c ;
}

bool	c_grid_point3d::get_uv_from_pos( REAL& u, REAL& v, REAL CONST * CONST vec )
{
	//	find a quad where the point fit
	REAL*	c;	REAL*	d;	//	cd
	REAL*	a;	REAL*	b;	//	ab
	for( INT32 j=1; j<_nb_v-2; ++j )
	{
		c = get_point_direct( 1, j+1 );
		a = get_point_direct( 1, j );
		for( INT32 i=1; i<_nb_u-2; ++i )
		{
			d = get_point_direct( i+1, j+1 );
			b = get_point_direct( i+1, j );
			//	on the left side of all side mean inside
			if(		line_side( a, b, vec ) >= 0.
				&&	line_side( b, d, vec ) >= 0.
				&&	line_side( d, c, vec ) >= 0.
				&&	line_side( c, a, vec ) >= 0.
				)
			{
				REAL	x = vec[0];
				REAL	y = vec[1];
				REAL	a1, b1, c1;	//	coef of line 1
				REAL	a2, b2, c2;	//	coef of line 2

				line_get_equation_coef( a1, b1, c1, a, c );
				line_get_equation_coef( a2, b2, c2, b, d );
				u = a1*x + b1*y + c1;
				u /= (a1-a2)*x + (b1-b2)*y + (c1-c2);
				u = REAL((i-1.+u)/(_nb_u-3.));

				line_get_equation_coef( a1, b1, c1, a, b );
				line_get_equation_coef( a2, b2, c2, c, d );
				v = a1*x + b1*y + c1;
				v /= (a1-a2)*x + (b1-b2)*y + (c1-c2);
				v = REAL((j-1.+v)/(_nb_v-3.));

				return true;
			}
			c = d;
			a = b;
		}
	}
	u = 0.;
	v = 0.;
	return false;
}


c_point3d_marker::c_point3d_marker()
:_nb_u(0)
,_nb_v(0)
,_points(nullptr)
,_nb_alloc(0)
,_text(nullptr)
{
}

c_point3d_marker::~c_point3d_marker()
{
	dealloc();
}

bool	c_point3d_marker::alloc( UINT32 nb_u, UINT32 nb_v, bool b_clear )
{
	UINT32	nb_real = nb_u * nb_v * 3;
	if( nb_real > _nb_alloc )
	{
		_points = (REAL*) REALLOC_ALIGNED( (void*)_points, nb_real * sizeof(REAL) );
		_text = (o_str*) REALLOC( (void*)_text, nb_real * sizeof( o_str ) );
	}
	if( _points )
	{
		if( b_clear )
			clear_vr( _points, nb_real );
		_nb_alloc = MAX( _nb_alloc, nb_real );
		_nb_u = nb_u;
		_nb_v = nb_v; 
		return true;
	}
	else
	{
		_nb_u = 0;
		_nb_v = 0;
		err_print( "%s() can't allocate points", __FUNCTION__ );
		return false;
	}
}

void	c_point3d_marker::dealloc()
{
	IF_FREE_ALIGNED_AND_NULL( _points );
	IF_FREE_AND_NULL( _text );
	_nb_u = 0;
	_nb_v = 0;
}

void	c_point3d_marker::write_marker_in_file( C_PCHAR_C name )
{
	file_csv::write_real_grid( name, _points, _nb_u * 3, _nb_v );
}

void	c_point3d_marker::read_marker_from_file( C_PCHAR_C name )
{
	file_csv::read_float_grid( name, _points, _nb_u * 3, _nb_v );
}

/*
void	c_point3d_marker::set_text( UINT32 iu, UINT32 iv, C_PCHAR_C	text )
{
	// get pointer
	// update o_str
}
*/

//void	c_point3d_marker::curve_read_float_from_file( CHAR* name )
//{
//	//INT32	nb_read = 0;
//	//nb_read = file_csv::read_float( name, _points, _nb_u * 3, _nb_v );
//	//alloc( nb_read / _nb_v / 3, _nb_v, false );
//	UINT32	x;
//	UINT32	y;
//	file_csv::count_float( name, x,y );
//	if( y == 1 )
//	{
//		alloc( x / 3 , 1 );
//		file_csv::read_float( name, _points, x, 1 );
//	}
//	//else
//	//{
//	//	alloc( 0, 0 );
//	//}
//}
