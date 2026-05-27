#include "draw/axe.h"
#include "primitive.h"
#include "gol/gol.h"
#include "gol/gol_light.h"
#include "gol/gol_tex.h"
#include "gol/gol_matrix.h"
#include "draw/seeddraw.h"

#define AAA_LOCAL_AXE_USE_LIST()	0

namespace {

CONSTEXPR INT32	PLANE_YZ		= 1;
CONSTEXPR INT32	PLANE_XZ		= 2;
CONSTEXPR INT32	PLANE_XY		= 4;
CONSTEXPR INT32	PLANE_NONE		= 0;
CONSTEXPR INT32	PLANE_ALL		= PLANE_XY | PLANE_YZ | PLANE_XZ;

typedef	struct	ST_AXE
{
	INT32	_axe_to_draw;
//	INT32	_plane_to_draw;
	FP32	_axe_color[4];
	FP32	_plane_color[3][4];

	FP32	_red_color[4];
	FP32	_green_color[4];
	FP32	_blue_color[4];
	REAL	_plane_size;
	REAL	_plane_res;
}	ST_AXE;

CONSTEXPR REAL	AXE_LENGTH		=	5.;
//	must have a floating point
CONSTEXPR REAL	AXE_RES			=	1.;
CONSTEXPR REAL	AXE_RES_OVER	=	1. / AXE_RES;

CONSTEXPR REAL	AXE_PLANE_DOM	=	REAL(.4);
CONSTEXPR REAL	AXE_PLANE_GREY	=	REAL(.1);

CONSTEXPR ST_AXE	st_axe = 
{
	n_axe::DRAW_ALL, // PLANE_XZ, 
	{ REAL(0.2), 0.5, REAL(0.2), 1.0 },
	{
		{ AXE_PLANE_DOM, AXE_PLANE_GREY, AXE_PLANE_GREY, 1.0 },
		{ AXE_PLANE_GREY, AXE_PLANE_DOM, AXE_PLANE_GREY, 1.0 },
		{ AXE_PLANE_GREY, AXE_PLANE_GREY, AXE_PLANE_DOM, 1.0 }
	},
	{ REAL(0.6), REAL(0.2), REAL(0.2), 1.0 }, 
	{ REAL(0.2), REAL(0.6), REAL(0.2), 1.0 }, 
	{ REAL(0.2), REAL(0.2), REAL(0.6), 1.0 }, 
	AXE_LENGTH,  AXE_RES
};

ST_AXE CONST * CONST p_axe = &st_axe;

#if	0
#	define	SET_COLOR(pt)	GOL::materialv( GL_FRONT, GL_AMBIENT_AND_DIFFUSE, pt );
#else
#	define	SET_COLOR(pt)	GOL::color4v( pt );
#endif

FINLINE	void	set_red()		{	SET_COLOR( p_axe->_red_color );		}
FINLINE	void	set_green()		{	SET_COLOR( p_axe->_green_color );	}
FINLINE	void	set_blue()		{	SET_COLOR( p_axe->_blue_color );	}

#if AAA_LOCAL_AXE_USE_LIST()
	INT32	RENDER_LIST_AXE			=	0;
	INT32	RENDER_LIST_AXE_X		=	0;
	INT32	RENDER_LIST_AXE_MARKER	=	0;
	INT32	RENDER_LIST_ORIGIN_X	=	0;
	//	INT32	RENDER_LIST_ORIGIN		=	0;

	INT32	RENDER_LIST_NULL_3D		=	0;
	INT32	RENDER_LIST_NULL_2D_X	=	0;
	INT32	RENDER_LIST_NULL_2D_Y	=	0;
	INT32	RENDER_LIST_NULL_2D_Z	=	0;

	//	INT32	RENDER_LIST_UNITS;

	INT32	CONSTEXPR	RENDER_LIST_NB = 8;

	FINLINE	void	draw_list_on_3_axe( INT32 list )
	{
		if( list==0 )
			return;

		//todo, 2015 maa removed push/pop attrib
		//	GOL::push_attrib( GL_POLYGON_BIT );
		//		GOL::enable( GL_CULL_FACE );	//GL_POLYGON_BIT
		//		GOL::set_cull( GL_BACK );

		set_red();												GOL::call_list( list );
		set_green();	GOL::rotate_deg( 120., 1., 1., 1. );	GOL::call_list( list );
		set_blue();		GOL::rotate_deg( 120., 1., 1., 1. );	GOL::call_list( list );
		GOL::rotate_deg( -240., 1., 1., 1. );

		//	GOL::pop_attrib();
	}
#endif	//AAA_LOCAL_AXE_USE_LIST()

typedef void	FN_PT	();
FINLINE	void	draw_fn_on_3_axe( FN_PT fn )
{
	set_red();													fn();
	set_green();	GOL::matrix::rotate_deg( 120., 1.,1.,1. );	fn();
	set_blue();		GOL::matrix::rotate_deg( 120., 1.,1.,1. );	fn();
	GOL::matrix::rotate_deg( -240., 1.,1.,1. );
}

CONSTEXPR REAL	MARKER_SIZE	=	REAL(.05);
CONSTEXPR REAL	MX			=	MARKER_SIZE * 4;
CONSTEXPR REAL	MY			=	MARKER_SIZE;
CONSTEXPR REAL	MZ			=	MY;

void	draw_axe_marker_low()	//pyramid
{
#if AAA_LOCAL_AXE_USE_LIST()
	static	CONSTEXPR REAL	p0[3]	=	{	MX,		.0,		.0	};
	static	CONSTEXPR REAL	p1[3]	=	{	.0,		-MY,	+MZ	};
	static	CONSTEXPR REAL	p2[3]	=	{	.0,		+MY,	+MZ	};
	static	CONSTEXPR REAL	p3[3]	=	{	.0,		+MY,	-MZ	};
	static	CONSTEXPR REAL	p4[3]	=	{	.0,		-MY,	-MZ	};

	static	CONSTEXPR REAL	na[3]	=	{	+MZ,	.0,		+MX	};
	static	CONSTEXPR REAL	nb[3]	=	{	+MY,	+MX,	.0	};
	static	CONSTEXPR REAL	nc[3]	=	{	+MZ,	.0,		-MX	};
	static	CONSTEXPR REAL	nd[3]	=	{	+MY,	-MX,	.0	};
	static	CONSTEXPR REAL	ne[3]	=	{	-1,		.0,		.0	};

	//	four triangles of the pyramid
	GOL::begin( GL_TRIANGLES );
		GOL::normal3v( na );	GOL::vertex3v( p0 );	GOL::vertex3v( p2 );	GOL::vertex3v( p1 );
		GOL::normal3v( nb );	GOL::vertex3v( p0 );	GOL::vertex3v( p3 );	GOL::vertex3v( p2 );
		GOL::normal3v( nc );	GOL::vertex3v( p0 );	GOL::vertex3v( p4 );	GOL::vertex3v( p3 );
		GOL::normal3v( nd );	GOL::vertex3v( p0 );	GOL::vertex3v( p1 );	GOL::vertex3v( p4 );
	GOL::end();

	//	base of the pyramid
	GOL::begin( GL_TRIANGLE_FAN );
		GOL::normal3v( ne );
		GOL::vertex3v( p1 );	GOL::vertex3v( p2 );	GOL::vertex3v( p3 );
		GOL::vertex3v( p4 );
	GOL::end();
#else	//AAA_LOCAL_AXE_USE_LIST()
	static CONSTEXPR FP32	pa[18][3]	=
	{		
		{	MX,		.0,		.0	},	{	.0,		+MY,	+MZ	},	{	.0,		-MY,	+MZ	},
		{	MX,		.0,		.0	},	{	.0,		+MY,	-MZ	},	{	.0,		+MY,	+MZ	},
		{	MX,		.0,		.0	},	{	.0,		-MY,	-MZ	},	{	.0,		+MY,	-MZ	},
		{	MX,		.0,		.0	},	{	.0,		-MY,	+MZ	},	{	.0,		-MY,	-MZ	},
		{	.0,		-MY,	+MZ	},	{	.0,		+MY,	+MZ	},	{	.0,		+MY,	-MZ	},
		{	.0,		-MY,	+MZ	},	{	.0,		+MY,	-MZ	},	{	.0,		-MY,	-MZ	},
	};
	static CONSTEXPR FP32	na[18][3]	=
	{				
		{	+MZ,	.0,		+MX	},	{	+MZ,	.0,		+MX	},	{	+MZ,	.0,		+MX	},
		{	+MY,	+MX,	.0	},	{	+MY,	+MX,	.0	},	{	+MY,	+MX,	.0	},
		{	+MZ,	.0,		-MX	},	{	+MZ,	.0,		-MX	},	{	+MZ,	.0,		-MX	},
		{	+MY,	-MX,	.0	},	{	+MY,	-MX,	.0	},	{	+MY,	-MX,	.0	},
		{	-1,		.0,		.0	},	{	-1,		.0,		.0	},	{	-1,		.0,		.0	},
		{	-1,		.0,		.0	},	{	-1,		.0,		.0	},	{	-1,		.0,		.0	},
	};

	c_prim3::draw_normal( (FP32 CONST * CONST)pa, GL_TRIANGLES, 18, (FP32 CONST * CONST)na );
#endif //AAA_LOCAL_AXE_USE_LIST()
}

FINLINE	void	draw_axe_marker()
{
#if AAA_LOCAL_AXE_USE_LIST()
	GOL::call_list( RENDER_LIST_AXE_MARKER );
#else
	draw_axe_marker_low();
#endif
}

FINLINE	void draw_line( FP32 CONST * CONST vec )
{
#if AAA_LOCAL_AXE_USE_LIST()
	GOL::begin( GL_LINES );
		GOL::vertex3fv( vec );
		GOL::vertex3fv( vec+3 );
	GOL::end();
#else
	c_prim3::draw( vec, GL_LINES, 2 );
#endif
}

CONSTEXPR FP32	vec_x[6] = {  0, 0, 0,	1,0,0	};
CONSTEXPR FP32	vec_y[6] = {  0, 0, 0,	0,1,0	};
CONSTEXPR FP32	vec_z[6] = {  0, 0, 0,	0,0,1	};

CONSTEXPR FP32	ori_x[6] = { -1, 0, 0,	1,0,0	};
CONSTEXPR FP32	ori_y[6] = {  0,-1, 0,	0,1,0	};
CONSTEXPR FP32	ori_z[6] = {  0, 0,-1,	0,0,1	};

void	draw_origin_x_low()
{
	draw_line( vec_x );
	GOL::matrix::translate_x( 1. );
		draw_axe_marker();
	GOL::matrix::translate_x( -1. );	//	back in place
}

void	draw_axe_x()
{
	static CONSTEXPR FP32	s		[3] = { 2.f, .2f, .2f	}; 
	static CONSTEXPR FP32	s_inv	[3] = { .5f, 5.f, 5.f	}; 
	INT32 number = INT32( AXE_LENGTH * AXE_RES_OVER );

	GOL::matrix::translate_x( -AXE_LENGTH );
	for( INT32 i = number * 2 - 1; i > 0; --i )
	{
		if( i != number )
		{
			GOL::matrix::translate_x( AXE_RES );
			draw_axe_marker();
		}
		else
		{
			GOL::matrix::translate_x( AXE_RES-MX );
			GOL::matrix::scale3v( s );
				draw_axe_marker();
			GOL::matrix::scale3v( s_inv );
			GOL::matrix::translate_x( MX );
		}
	}
	GOL::matrix::translate_x( -AXE_LENGTH + AXE_RES );	//	back in place

	//glbug	seems to consume the glMaterialfv attribution
	//	so I move it after the polys
	static CONSTEXPR FP32	vec[2][3]	=	{	{	-AXE_LENGTH - AXE_RES , .0, .0	},	{	AXE_LENGTH + AXE_RES , .0, .0	}	};
	draw_line( (FP32 CONST * CONST) vec );
}

}	//end anonymous namespace



namespace n_axe
{

//todo move this out of axe thius more generic
FINLINE	void	push_draw()
{
	//	GOL::push_attrib( GL_DEPTH_BUFFER_BIT | GL_LIGHTING_BIT | GL_COLOR_BUFFER_BIT | GL_TEXTURE_BIT | GL_CURRENT_BIT );

	GOL::push_att();
		//todoatt still some att to deal with
		//	GOL:hint( GL_LINE_SMOOTH_HINT, GOL::DONT_CARE );
		GOL::push_line_smooth( b_line_smooth );
		GOL::push_line_width( line_width );

		GOL::push_depth_test( true );	//GL_DEPTH_BUFFER_BIT
		GOL::push_depth_write( true );
		GOL::push_depth_func( GL_LEQUAL );	//wa GL_LESS );

		GOL::push_lighting( false );	//GL_LIGHTING_BIT

		GOL::push_blend( false );											//GL_COLOR_BUFFER_BIT
		GOL::push_blend_func( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );		//GL_COLOR_BUFFER_BIT

		GOL::push_texture_dim( 0 );											//GL_TEXTURE_BIT
		GOL::push_color();
}
FINLINE	void	pop_draw()
{
	GOL::pop_att();
}

namespace {
	//used for null_3d and null_2d
	FP32	r[18];	
}
void	draw_null_3d( REAL CONST x, REAL CONST y, REAL CONST z, REAL CONST size )
{
	REAL CONST s = size * REAL(.5);
	r[0]  = x-s;	r[3]  = x+s;	r[15] = r[12] = r[9] = r[6] = x;
	r[7]  = y-s;	r[10] = y+s;	r[16] = r[13] = r[4] = r[1] = y;
	r[14] = z-s;	r[17] = z+s;	r[11] = r[8]  = r[5] = r[2] = z;
	c_prim3::draw( r, GL_LINES, 6 );
}

void	draw_null_3d( REAL CONST size )
{
	static FP32	rc[18] = { 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0, 0,0,0 };
	REAL CONST s = size * REAL(.5);
	rc[0] = rc[7]  = rc[14] = -s;
	rc[3] = rc[10] = rc[17] = s ;
	c_prim3::draw( rc, GL_LINES, 6 );
}

void	draw_null_2d( REAL CONST x, REAL CONST y, REAL CONST z, REAL CONST size, INT32 CONST axe )
{
	REAL CONST s = size * REAL(.5);
	switch( axe )
	{
	case 0: r[9] = r[6] = r[3] = r[0] = x; 
			r[1] = y-s;
			r[4] = y+s;
			r[10] = r[7] = y;
			r[5] = r[2] = z;
			r[8] = z-s;
			r[11] = z+s;
			break;
	case 1: r[0] = x-s;
			r[3] = x+s;
			r[9] = r[6] = x;
			r[10] = r[7] = r[4] = r[1] = y; 
			r[5] = r[2] = z;
			r[8] = z-s;
			r[11] = z+s;
			break;
	case 2:	r[0] = x-s;
			r[3] = x+s;
			r[9] = r[6] = x; 
			r[4] = r[1] = y;
			r[7] = y-s;
			r[10] = y+s;
			r[11] = r[8] = r[5] = r[2] = z;
			break;
	}
	c_prim3::draw( r, GL_LINES, 4 );
}

void	draw_null_2d( REAL CONST size, INT32 CONST axe )
{
	static	FP32	r0[12] = { 0,0,0, 0,0,0, 0,0,0, 0,0,0 };
	static	FP32	r1[12] = { 0,0,0, 0,0,0, 0,0,0, 0,0,0 };
	static	FP32	r2[12] = { 0,0,0, 0,0,0, 0,0,0, 0,0,0 };
	REAL CONST s = size * REAL(.5);
	switch( axe )
	{
	case 0: r0[8]	= r0[1]	= -s;
			r0[11]	= r0[4]	= s;
			c_prim3::draw( r0, GL_LINES, 4 );
			break;
	case 1: r1[8]	= r1[0] = -s;
			r1[11]	= r1[3] = s;
			c_prim3::draw( r1, GL_LINES, 4 );
			break;
	case 2:	r2[7]	= r2[0] = -s;
			r2[10]	= r2[3] = s;
			c_prim3::draw( r2, GL_LINES, 4 );
			break;
	}
}

#if AAA_LOCAL_AXE_USE_LIST()
	FINLINE	void	draw_null_3d_list()						{	GOL::call_list( RENDER_LIST_NULL_3D );			}
	FINLINE	void	draw_null_2d_list( INT32 CONST axe )	{	GOL::call_list( RENDER_LIST_NULL_2D_X + axe );	}
	FINLINE	void	draw_axe()								{	GOL::call_list( RENDER_LIST_AXE );				}
	FINLINE	void	draw_origin_geo()						{	draw_list_on_3_axe( RENDER_LIST_ORIGIN_X );		}
#else
	FINLINE	void	draw_null_3d_list()						{	draw_null_3d();									}
	FINLINE	void	draw_null_2d_list( INT32 CONST axe )	{	draw_null_2d( 1, axe );							}
	FINLINE	void	draw_axe()								{	draw_fn_on_3_axe( draw_axe_x );					}
	FINLINE	void	draw_origin_geo()						{	draw_fn_on_3_axe( draw_origin_x_low );			}
#endif

//
//	following fns are accessible outside this code unit
//		or used by this fns
//

bool	b_line_smooth		= false;
REAL	line_width			= 1. ;
REAL	line_width_plane	= line_width * REAL(.5);

void	draw_units()
{
//	GOL::push_attrib( GL_POLYGON_BIT );	
//		GOL::enable( GL_CULL_FACE );	//GL_POLYGON_BIT
//		GOL::set_cull( GL_BACK );

	SET_COLOR( p_axe->_red_color );
	draw_line( vec_x );
	
	SET_COLOR( p_axe->_green_color );
	draw_line( vec_y );

	SET_COLOR( p_axe->_blue_color );
	draw_line( vec_z );

//	GOL::pop_attrib();
}

void	draw_origin()
{
	push_draw();
		draw_origin_geo();
	pop_draw();
}

void	draw_origin_geo( FP32 CONST size )
{
	if( size != 1. )
	{
		GOL::matrix::push();
			GOL::matrix::scale( size );
			draw_origin_geo();
		GOL::matrix::pop();
	}
	else
		draw_origin_geo();
}
void	draw_origin( FP32 CONST size )
{
	push_draw();
		draw_origin_geo( size );
	pop_draw();
}



}	//end namespace n_axe

namespace {
	static FP32 vec_line[4][3]; 

	FINLINE	void	draw_lines( INT32 CONST index, REAL CONST beg, REAL CONST end, REAL CONST step )
	{
		// skip central line
		for( REAL f = end; f > beg; f -= step )
		{
			vec_line[0][index] = vec_line[1][index] = f;
			vec_line[2][index] = vec_line[3][index] = -f;
			c_prim3::draw( (FP32 CONST * CONST)vec_line, GL_LINES, 4 );
		}
	}

	FINLINE	void	draw_plane_one( INT32 CONST index )
	{
		REAL	size = p_axe->_plane_size;
		REAL	res = p_axe->_plane_res;
		REAL	CONST stop = res * REAL(.5);

		vec_line[0][index] = vec_line[1][index] = vec_line[2][index] = vec_line[3][index] = 0;

		INT32 tmp_index = ( index + 1 ) % 3;
		//	one way
		vec_line[0][tmp_index] = vec_line[2][tmp_index] = -size;
		vec_line[1][tmp_index] = vec_line[3][tmp_index] = size;
		GOL::color3v( p_axe->_plane_color[tmp_index] );
		tmp_index = ( index + 2 ) % 3;
		draw_lines( tmp_index, stop, size, res );

		//	other way
		vec_line[0][tmp_index] = vec_line[2][tmp_index] = -size;
		vec_line[1][tmp_index] = vec_line[3][tmp_index] = size;
		GOL::color3v( p_axe->_plane_color[tmp_index] );
		draw_lines( ( index + 1 ) % 3, stop, size, res );
	}

	FINLINE	void	draw_planes( INT32 axe_cur )
	{
		//if( p_axe->_b_enable )	return;

		INT32	plane_to_draw;

		if( axe_cur )
			plane_to_draw = axe_cur;
		else
			plane_to_draw = PLANE_XZ;
	
		if( plane_to_draw & PLANE_XY )
			draw_plane_one( 2 );
		if( plane_to_draw & PLANE_YZ )
			draw_plane_one( 0 );
		if( plane_to_draw & PLANE_XZ )
			draw_plane_one( 1 );
	}
};

namespace n_axe
{

void	draw_orientation( FP32 CONST size )
{
	set_red();	
	GOL::matrix::push();
		GOL::matrix::scale( size );
		draw_line( ori_x );
		GOL::matrix::translate_x( 1. );
		draw_axe_marker();
	GOL::matrix::pop();

	set_green();
	GOL::matrix::push();
		GOL::matrix::rotate_deg( 120., 1.,1.,1. );
		GOL::matrix::scale( size );
		draw_line( ori_x );
		GOL::matrix::translate_x( 1. );
		draw_axe_marker();
	GOL::matrix::pop();

	set_blue();
	GOL::matrix::push();
		GOL::matrix::rotate_deg( 240., 1.,1.,1. );
		GOL::matrix::scale( size );
		draw_line( ori_x );
		GOL::matrix::translate_x( 1. );
		draw_axe_marker();
	GOL::matrix::pop();
}


void	draw_axe_and_plane( INT32 CONST axe_cur )
{
	c_draw_ui_guard guard;
	//if( !p_axe->_b_enable )	return;
	//push_draw();	//done already in guard

		GOL::push_color();
		GOL::set_line_width( n_axe::line_width_plane );	//GL_LINE_BIT
		draw_planes( axe_cur );

		GOL::color3v( p_axe->_axe_color );
		GOL::set_depth_func( GL_LEQUAL );	//GL_DEPTH_BUFFER_BIT

		GOL::set_line_width( n_axe::line_width );
		draw_axe();

	//pop_draw();	//done already in guard
}


void	init()
{
#if AAA_LOCAL_AXE_USE_LIST()
	RENDER_LIST_AXE = GOL::gen_lists( RENDER_LIST_NB );

	if( RENDER_LIST_AXE == 0 )
		return;

	INT32 index = RENDER_LIST_AXE;
	RENDER_LIST_AXE_X		=	++index;
	RENDER_LIST_AXE_MARKER	=	++index;
	RENDER_LIST_ORIGIN_X	=	++index;
	RENDER_LIST_NULL_3D		=	++index;
	RENDER_LIST_NULL_2D_X	=	++index;
	RENDER_LIST_NULL_2D_Y	=	++index;
	RENDER_LIST_NULL_2D_Z	=	++index;
	//	RENDER_LIST_UNITS		=	RENDER_LIST_AXE		+	9;

	GOL::compile_list( RENDER_LIST_AXE_MARKER	);	draw_axe_marker_low();			GOL::end_list();
	GOL::compile_list( RENDER_LIST_AXE_X		);	draw_axe_x();					GOL::end_list();
	GOL::compile_list( RENDER_LIST_AXE			);	draw_fn_on_3_axe( draw_axe_x );	GOL::end_list();

	GOL::compile_list( RENDER_LIST_ORIGIN_X		);	draw_origin_x_low();			GOL::end_list();
	GOL::compile_list( RENDER_LIST_NULL_3D		);	draw_null_3d();					GOL::end_list();

	for( INT32 i=0; i<3; ++i )
	{
		GOL::compile_list( RENDER_LIST_NULL_2D_X+i );
		draw_null_2d( 1, i );
		GOL::end_list();
	}
#endif	//AAA_LOCAL_AXE_USE_LIST()
}

}	//end namespace n_axe
