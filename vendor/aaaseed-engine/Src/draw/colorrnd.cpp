#include "draw/colorrnd.h"
#include "math/rand.h"
#include "err.h"
		 
//#include "math/v_base.h"
#include "gol/gol_draw.h"
#include "gol/gol_color.h"
#include "draw/render.h"
#include "draw/mat.h"
#include "gol/gol_light.h"

c_color_random::RANDOM_COLOR_FN	c_color_random::random_color_fn = nullptr;
static	INT32	color_index = 0;

static	CONST	INT32	FACE_COLOR_MAX = 2048;

C_PCHAR_C	c_color_random::str_type_name[c_color_random::TYPE_NB+1] =
{
	"No",
	"RANDOM_MAA",
	"RANDOM_MAA_ALPHA",
	"RANDOM_MIGUEL",
	"RANDOM_MIGUEL_ALPHA",
	"RANDOM_DOMBIS",
	"COLOR_BAR_VIDEO",
};

static	FP32	color_miguel[FACE_COLOR_MAX][4];
static	FP32	color_maa[FACE_COLOR_MAX][4];
static	FP32	color_dombis[4][3] = 
{ 
	{ 1., 1., 1. },
	{ 1., REAL(.2), REAL(.2) },
	{ 1., REAL(.2), REAL(.2) },
	{ .5, .0, .5 },
};
static	FP32	color_bar[8][3] = 
{ 
	{ 1, 1, 1 },
	{ 1, 1, 0 },
	{ 0, 1, 1 },
	{ 0, 1, 0 },
	{ 1, 0, 1 },
	{ 1, 0, 0 },
	{ 0, 0, 1 },
	{ 0, 0, 0 },
};

//todo use in bdd_cur_edit
static	FP32	color_tracking[] =
{
	1, 0, 0, 1,
	0, 1, 0, 1,
	0, 0, 1, 1,
	1, 1, 0, 1,
	0, 1, 1, 1,
	1, 0, 1, 1,
};

AAA_ERR	c_color_random::init()
{
	c_rand_lin	color_rand;

	for( INT32 i = 0; i < FACE_COLOR_MAX; ++i )
	{
/*		face_color[i].x = ((i & 0x18) == 0) * .75 * (i & 0x7) / 7. + .25;
		face_color[i].y = ((i & 0x18) & 0x08) * .75 * (i & 0x7) / 7. + .25;
		face_color[i].z = ((i & 0x18) & 0x10) * .75 * (i & 0x7) / 7. + .25;
*/
		if( color_rand.get_fp32_01() < .8 )
		{
			color_miguel[i][0] = 1;
			color_miguel[i][1] = 1;
			color_miguel[i][2] = 1;
			if( color_rand.get_fp32_01() < .3 )
				color_miguel[i][3] = 0;
			else
				color_miguel[i][3] = 1;
		}
		else
		{
			color_miguel[i][0] = 0;
			color_miguel[i][1] = 0;
			color_miguel[i][2] = 0;
			color_miguel[i][ I_FLOOR( color_rand.get_fp32_max(2.9999) ) ] = 1;
			color_miguel[i][3] = 1;
		}

		REAL a,b,c;
		a = color_rand.get_fp32_01();
		b = color_rand.get_fp32_01();
		c = color_rand.get_fp32_01();

//		REAL	sum;
//		sum = SQRT( a*a+b*b+c*c) / SQRT(3.);
//		color_maa[i][0] = a/sum;
//		color_maa[i][1] = b/sum;
//		color_maa[i][2] = c/sum;

		color_maa[i][0] = a;
		color_maa[i][1] = b;
		color_maa[i][2] = c;

		color_maa[i][3] = color_rand.get_fp32_01();
	}
	return AAA_OK;
}


void set_maa( INT32 index)
{
	GOL::color3v( color_maa[ index&(FACE_COLOR_MAX-1)] );
}

void set_maa_alpha( INT32 index)
{
	GOL::color4v( color_maa[ index&(FACE_COLOR_MAX-1)] );
}

void set_miguel( INT32 index)
{
	GOL::color3v( color_miguel[ index&(FACE_COLOR_MAX-1)] );
}

void set_miguel_alpha( INT32 index)
{
	GOL::color4v( color_miguel[ index&(FACE_COLOR_MAX-1)] );
}

void set_dombis( INT32 index)
{
	GOL::color3v( color_dombis[ index & (4-1)] );
}

void set_color_bar( INT32 index)
{
	GOL::color3v( color_bar[ index & (8-1)] );
}

void set_tracking( INT32 index)
{
	GOL::color3v( &color_tracking[ IMOD( index, 6) ] );
}

c_color_random::RANDOM_COLOR_FN  get_random_color_fn( INT32 in )
{
	switch( in )
	{
	case 1:	return set_maa;
	case 2:	return set_maa_alpha;
	case 3:	return set_miguel;
	case 4:	return set_miguel_alpha;
	case 5:	return set_dombis;
	case 6:	return set_color_bar;
	case 7:	return set_tracking;
	default:
		ERR_PRINT_STRING( "%s() : This random color type (%d) is unimplemented.", __FUNCTION__, in );
		return set_maa;
		break;
	}
}

FINLINE void c_color_random::set_and_inc( INT32 inc)
{
	set( color_index );
	color_index += inc;
}


void c_color_random::set()
{
	set_and_inc( 1 );
}

void	c_color_random::begin()
{
	GOL::color_material( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
	GOL::enable_color_material();
	color_index = 0;
	random_color_fn = get_random_color_fn( c_render::get_cur()->get_random_on_color() );
}

void	c_color_random::end()
{
	GOL::disable_color_material();
	c_materials::get_cur()->reset();
}


c_color_drawer::c_color_drawer( INT32 nb )
{
	_nb = 6;
	_color_loop = color_tracking;
}
c_color_drawer::~c_color_drawer()
{
}

void	c_color_drawer::set_color_fix( FP32 CONST * color )
{
	if( color )
	{
		cpy_v4( _color_fix, color );
		_b_fix = true;
	}
	else
		_b_fix = false;
}

void	c_color_drawer::draw_color_cur()
{
	GOL::color4v( get_color_cur() );
}
void	c_color_drawer::draw_color( INT32 ind )
{
	GOL::color4v( get_color(ind) );
}
void	c_color_drawer::draw_color( INT32 ind, FP32 alpha )
{
	FP32*	c = get_color(ind);
	GOL::color4( *c, *(c+1), *(c+2), alpha );
}
