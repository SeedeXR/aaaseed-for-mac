/*******************************************************************************
**
**  Module: bdd_maa.c
**
**  DESCRIPTION: 
**
**  CREATION: Emmanuel BERRIET					date : 12/04/92
**
\******************************************************************************/

#include "bdd_algo_maa.h"
#include "obj_ui/deformer/def_node.h"
#include "math/rand.h"
#include "draw/colorrnd.h"
#include "draw/tex_anim.h"
#include "draw/render.h"
#include "gol/gol_draw.h"

FACTORY_CREATE_PROP_V1( c_bdd_algo_maa, bdd_algo_maa, Algo Maa, algo_maa, sub_menu="Procedural"; sel0="Maa Algo"; );

CONSTEXPR	INT32	X	= 0;
CONSTEXPR	INT32	Y	= 1;
CONSTEXPR	INT32	Z	= 2;

CONSTEXPR	INT32	BOX_TYPE_NB = 3;
C_PCHAR_C str_box_type[BOX_TYPE_NB] =
{
	"No",
	"Wrap",
	"Bounce",
};
CONSTEXPR	REAL OVER_256 = 1. / 256.;
namespace n_bdd_algo_maa
{
	CONSTEXPR INT32 BASE_PARAM_NB	= 15 + c_bdd::GEO_PARAM_NB;
	CONSTEXPR INT32 BOX_PARAM_NB	= 8;
	CONSTEXPR INT32 GROUP_PARAM_NB	= 1;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
									+	BOX_PARAM_NB
									+	GROUP_PARAM_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
		{
		BDD_BASE_PARAMS

		PARAM_DEF_INT32_POS_ONE(	point_allocated )
		PARAM_DEF_REAL_POS_ONE(		point_nb	)
		PARAM_DEF_INT32_POS(		seed,		0, 42	)
		PARAM_DEF_SCALE_XYZF(		step		)
		PARAM_DEF_BOOL_OFF(			step_random	)
		PARAM_DEF_POINT_XYZ(		start		)
		PARAM_DEF_XYZ_LOCKED(		stop		)
		PARAM_DEF_BOOL_OFF(			jump_random_tmp	)

		PARAM_DEF_GROUP_CLOSED( Box, BOX_PARAM_NB )
			PARAM_DEF_SYMBO_PSTR_ZERO(	box_type,	str_box_type )
			PARAM_DEF_POINT_XYZ(		box_origin	)
			PARAM_DEF_SCALE_XYZF(		box_size	)
		};
}

void	c_bdd_algo_maa::param_init_pt()
{
INT32	h;
	h = param_init_pt_geo();

	param_set_pt(		h, _point_allocated_ui	);
	param_set_pt(		h, _point_nb_as_real	);
	param_set_pt(		h, _seed				);

	param_set_pt_4(	h, _step_ui				);
	param_set_pt(		h, _b_step_random		);

	param_set_pt_3(	h, _start				);
	param_set_pt_3(	h, _stop_out			);

	param_set_pt(		h, _b_test				);

//	box
	++h;
		param_set_pt(		h, _s_box_type		);
		param_set_pt_3(	h, _box_origin		);
		param_set_pt_3(	h, _box_size_ui		);
		param_set_pt(		h, _box_size_factor	);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_bdd_algo_maa)
{
	_point = nullptr;
	_point_allocated = 0;
	_b_draw = nullptr;

	param_init_with( n_bdd_algo_maa::param, n_bdd_algo_maa::PARAM_NB_MAX );
}

c_bdd_algo_maa::~c_bdd_algo_maa()
{
	dealloc_point();
}

void c_bdd_algo_maa::alloc_point()
{
	if( _point_allocated != _point_allocated_ui )
	{
		_point	= (REAL *) REALLOC_ALIGNED(	_point,		_point_allocated_ui * sizeof(REAL) * 3 * 2	);
		_b_draw	= (bool *) REALLOC_ALIGNED(	_b_draw,	_point_allocated_ui * sizeof(bool)			);
		if( _point && _b_draw )
		{
			_point_allocated = _point_allocated_ui;
			_point_deformed = _point + _point_allocated_ui * 3;
		}
		else
		{
			err_print( "Can't alloc point" );
			dealloc_point();
		}
	}
}

void c_bdd_algo_maa::dealloc_point()
{ 
	if ( _point )
	{
		FREE_ALIGNED_AND_NULL( _point );
		FREE_ALIGNED_AND_NULL( _b_draw );
		_point_allocated = 0;
	}
}

void	c_bdd_algo_maa::update()
{
	scale_v3( _step, _step_ui, _step_ui[3] );
	//	prepare the box
	if( _s_box_type )
	{
		scale_v3( _box_size, _box_size_ui, _box_size_factor );
		add_scale_v3( _box_max, _box_origin, _box_size, .5 );
		sub_v3( _box_min, _box_max, _box_size );
	}

	_point_nb = MAX1( INT32(_point_nb_as_real) );	//hack
	_point_allocated_ui = MAX( _point_allocated_ui, _point_nb );
	alloc_point();
	compute_point();

	auto def = c_def_node::get_cur();
	if( def->is_deforming() )
	{
		def->apply( _point_deformed, _point, _point_nb );
		_point_to_draw = _point_deformed;
	}
	else
		_point_to_draw = _point;

	cpy_v3( _stop_out, _point_to_draw + (_point_nb-1)*3 );	//hack
}


FINLINE	void	change_xyz( REAL& x, REAL& y, REAL& z )
{
	REAL	tmp;

	tmp = FMOD( x, REAL(4) );
	if( tmp>3 )
		x -= 2.5;
	else if( tmp>2 )
		x += 2.5;

	tmp = FMOD( y, REAL(4) );
	if( tmp>3 )
		y -= 2.5;
	else if( tmp>2 )
		y += 2.5;

	tmp = FMOD( z, REAL(4) );
	if( tmp>3  )
		z -= 2.5;
	else if( tmp>2 )
		z += 2.5;
}


void	c_bdd_algo_maa::compute_point()
{
	REAL*	pt = _point;
	bool*	pb = _b_draw;

	cpy_v3( pt, _start );
	pt += 3 - 1;	//	-1 for preinc
	*pb++ = true;	//	idem

	REAL	x = _start[0];
	REAL	y = _start[1];
	REAL	z = _start[2];

	REAL	step_x = _step[0];
	REAL	step_y = _step[1];
	REAL	step_z = _step[2];

	INT32	i = _point_nb-1;

	SRAND( _seed );
	switch( _s_box_type )
	{
	case 0:
		do
		{
			INT32 r = RAND_I32();
	//		rb = (r & 0xfff00);
			if( _b_step_random )
			{
				step_x = _step[0] * (RAND_I32()&0xff) * OVER_256 ;
				step_y = step_x;
				step_z = step_x;
			}

			if( (r & 0x2a) != 0x2a )
			{
				INT32	rb = r & 3;
				if ( rb == 0 )
					x += step_x;
				else if ( rb == 1 )
					x -= step_x;

				rb = r & 0xc;
				if ( rb == 0 )
					y += step_y;
				else if ( rb == 4 )
					y -= step_y;
					
				rb = r & 0x30;
				if ( rb == 0 )
					z += step_z;
				else if ( rb == 0x10 )
					z -= step_z;

				if( _b_test )
					change_xyz( x, y, z );

				*++pt = x;
				*++pt = y;
				*++pt = z;
				--i;
			}
			}
		while( i>0 );
		break;
	case 1:
		do
		{
			INT32 r = RAND_I32();
	//		rb = (r & 0xfff00);		
			if( _b_step_random )
			{
				step_x = _step[0] * (RAND_I32()&0xff) * OVER_256 ;
				step_y = step_x;
				step_z = step_x;
			}

			if( (r & 0x2a) != 0x2a )
			{
				*pb = true;
				INT32	rb = r & 3;
				if ( rb == 0 )
				{
					x += step_x;
					if ( x > _box_max[X] )
					{
						x -= _box_size[X];
						*pb = false;
					}
				}
				else if ( rb == 1)
				{
					x -= step_x;
					if ( x < _box_min[X] )
					{
						x += _box_size[X];
						*pb = false;
					}
				}
						
				rb = r & 0xc;
				if ( rb == 0 )
				{
					y += step_y;
					if ( y > _box_max[Y] )
					{
						y -= _box_size[Y];
						*pb = false;
					}
				}
				else if ( rb == 4 )
				{
					y -= step_y;
					if ( y < _box_min[Y] )
					{
						y += _box_size[Y];
						*pb = false;
					}
				}
					
				rb = r & 0x30;
				if ( rb == 0 )
				{
					z += step_z;
					if ( z > _box_max[Z] )
					{
						z -= _box_size[Z];
						*pb = false;
					}
				}
				else if ( rb == 0x10 )
				{
					z -= step_z;
					if ( z < _box_min[Z] )
					{
						z += _box_size[Z];
						*pb = false;
					}
				}

				if( _b_test )
					change_xyz( x, y, z );

				*++pt = x;
				*++pt = y;
				*++pt = z;
				++pb;
				--i;
			}
		}
		while( i>0 );
		break;
	case 2:	//case bounce
		do
		{
			INT32 r = RAND_I32();
	//		rb = (r & 0xfff00);		
			if( _b_step_random )
			{
				step_x = _step[0] * (RAND_I32()&0xff) * OVER_256 ;
				step_y = step_x;
				step_z = step_x;
			}

			if( (r & 0x2a) != 0x2a )
			{
				INT32	rb = r & 3;
				if ( rb == 0 )
				{
					x += step_x;
					if ( x > _box_max[X] )
						x -= step_x*2;
				}
				else if ( rb == 1)
				{
					x -= step_x;
					if ( x < _box_min[X] )
						x += step_x*2;
				}
						
				rb = r & 0xc;
				if ( rb == 0 )
				{
					y += step_y;
					if ( y > _box_max[Y] )
						y -= step_y*2;
				}
				else if ( rb == 4 )
				{
					y -= step_y;
					if ( y < _box_min[Y] )
						y += step_y*2;
				}
					
				rb = r & 0x30;
				if ( rb == 0 )
				{
					z += step_z;
					if ( z > _box_max[Z] )
						z -= step_z*2;
				}
				else if ( rb == 0x10 )
				{
					z -= step_z;
					if ( z < _box_min[Z] )
						z += step_z*2;
				}

				if( _b_test )
					change_xyz( x, y, z );

				*++pt = x;
				*++pt = y;
				*++pt = z;
				--i;
			}
		}
		while( i>0 );
		break;
	}

}

void	c_bdd_algo_maa::draw_single()
{
	REAL const *	pt = _point_to_draw;
	bool const *	pb = _b_draw;
	INT32	i = _point_nb;
	if( i>1 )
	{
		GOL::begin( GL_LINE_STRIP );
		switch( _s_box_type )
		{
		case 1:
			while( i-->0 )
			{
				if( *pb++ )
					GOL::vertex3v( pt);
				else
				{
					GOL::end();
					GOL::begin( GL_LINE_STRIP );
					GOL::vertex3v( pt);
				}
				pt += 3;
			}
			break;
		default:
			while( i-->0 )
			{
				GOL::vertex3v( pt);
				pt += 3;
			}
			break;
		}
		GOL::end();
	}
}

void	c_bdd_algo_maa::save_obj_file( FILE* file )
{
	save_obj_file_points( file, _point_to_draw, _point_nb );
}	


/*
//old
void	bdd_maa_6_draw(INT32	reso)
{
INT32	i;
REAL	x;
REAL	y;
REAL	z;
REAL	step;
INT32	r;

	i = reso * reso;
	
	x = 0;
	y = 0;
	z = 0;
	step = .004;

	SRAND(42);
	GOL::begin( GL_LINE_STRIP );
	while( i-- )
		{
		while ( (r = (RAND_I32() & 0x7) ) > 5);
		switch (r)
			{
			case 0: x += step;	break;
			case 1: x -= step;	break;
			case 2: y += step;	break;
			case 3: y -= step;	break;
			case 4: z += step;	break;
			case 5: z -= step;	break;
			}
		GOL::vertex3( x, y, z);
		}
	GOL::end();

}

void	bdd_maa_26_draw(INT32	reso)
{
INT32	i;
REAL	x;
REAL	y;
REAL	z;
REAL	step;
INT32	r;
INT32	rb;

	i = reso * reso;
	
	x = 0;
	y = 0;
	z = 0;
	step = .04;

	SRAND(42);
	GOL::begin( GL_LINE_STRIP );
	while( i-- )
		{
		r = RAND_I32();
		
		rb = r & 3;
		if ( rb == 0 )
			x += step;
		else if ( rb == 1)
			x -= step;

		rb = r & 0xc;
		if ( rb == 0 )
			y += step;
		else if ( rb == 4 )
			y -= step;
		
		rb = r & 0x30;
		if ( rb == 0 )
			z += step;
		else if ( rb == 0x10 )
			z -= step;

		GOL::vertex3( x, y, z);
		}
	GOL::end();

}
*/

INT32	c_bdd_algo_maa::get_point_nb()							{	return _point_nb;		}
REAL*	c_bdd_algo_maa::get_points()							{	return _point_to_draw;	}
REAL*	c_bdd_algo_maa::get_point_pt( INT32 CONST index )
{
	if( IS_INDEX_VALID( index, _point_nb ) )
		return _point_to_draw + index*3;
	return nullptr;
}
