/*******************************************************************************
**
**  Module: bdd_maa.c
**
**  DESCRIPTION: 
**
**  CREATION: Emmanuel BERRIET					date : 12/04/92
**
\******************************************************************************/

/*
**                                                                INCLUDE FILES
\******************************************************************************/

#include "bdd_maa.h"
#include "obj_ui/deformer/def_node.h"
#include "gol/gol_draw.h"
#include "math/rand.h"

void	bdd_maa_6_draw(INT32	reso)
{
	INT32	i		= reso * reso;
	REAL	x		= 0;
	REAL	y		= 0;
	REAL	z		= 0;
	REAL	step	= REAL(.004);

	SRAND(42);
	GOL::begin( GL_LINE_STRIP );
	while( i-- )
	{
		INT32	r;
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
	INT32	i		= reso * reso;
	REAL	x		= 0;
	REAL	y		= 0;
	REAL	z		= 0;
	REAL	step	= REAL(.04);

	SRAND(42);
	GOL::begin( GL_LINE_STRIP );
	while( i-- )
	{
		INT32 r = RAND_I32();
		
		INT32 rb = r & 3;
		if		( rb == 0 )		x += step;
		else if ( rb == 1)		x -= step;

		rb = r & 0xc;
		if		( rb == 0 )		y += step;
		else if ( rb == 4 )		y -= step;
		
		rb = r & 0x30;
		if ( rb == 0 )			z += step;
		else if ( rb == 0x10 )	z -= step;

		GOL::vertex3( x, y, z);
	}
	GOL::end();

}

void	bdd_maa_algo_draw( REAL size, INT32 reso )
{
	INT32	i		= reso * reso;
	REAL	x		= 0;
	REAL	y		= 0;
	REAL	z		= 0;
	REAL	step	= size*REAL(.04);

	SRAND(42);
	auto def = c_def_node::get_cur();
	if( !def->is_deforming() )
		def = nullptr;

	GOL::begin( GL_LINE_STRIP );
	while( i-- )
	{
		INT32 r = RAND_I32();

		INT32 rb = (r & 0xfff00);
/*
REAL	xb;
REAL	yb;
REAL	zb;
		if (  rb == 0 )
			{
			xb = x;
			yb = y;
			zb = z;
			}
		else if ( (rb & 0xff00) == 0 )
			{
			x = xb;
			y = yb;
			z = zb;
			}
*/		
		rb = r & 3;
		if		( rb == 0 )		x += step;
		else if ( rb == 1)		x -= step;

		rb = r & 0xc;
		if		( rb == 0 )		y += step;
		else if ( rb == 4 )		y -= step;
		
		rb = r & 0x30;
		if		( rb == 0 )		z += step;
		else if ( rb == 0x10 )	z -= step;

		if( def )
		{
			REAL	f[3];
			f[0] = x;
			f[1] = y;
			f[2] = z;
			def->apply( f, 1 );
			GOL::vertex3v( f );
		}
		else
			GOL::vertex3( x, y, z );

	}
	GOL::end();

}
