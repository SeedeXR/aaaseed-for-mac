#include "gol/gol_draw.h"

namespace
{
	INT32 scissor_x  = -1;
	INT32 scissor_y  = -1;
	INT32 scissor_sx = -1;
	INT32 scissor_sy = -1;
	INT32 viewport_x  = -1;
	INT32 viewport_y  = -1;
	INT32 viewport_sx = -1;
	INT32 viewport_sy = -1;
}

//todo add a lua access to get and set scissor
void GOL::scissor( INT32 CONST x, INT32 CONST y, INT32 CONST sx, INT32 CONST sy )
{
	if( is_state_cache_no()
		|| sx!=scissor_sx || sy!=scissor_sy
		|| x !=scissor_x  || y !=scissor_y
		)
	{
		glScissor( x,y, sx,sy );
		scissor_sx = sx;
		scissor_sy = sy;
		scissor_x  = x;
		scissor_y  = y;
	}
}

void GOL::get_scissor( INT32& x, INT32& y, INT32& sx, INT32& sy )
{
	sx = scissor_sx;
	sy = scissor_sy;
	x  = scissor_x;
	y  = scissor_y;
}

//todo add a lua access to get and set viewport
void GOL::viewport( INT32 CONST x, INT32 CONST y, INT32 CONST sx, INT32 CONST sy )
{
	if( is_state_cache_no()
		|| sx!=viewport_sx || sy!=viewport_sy
		|| x !=viewport_x  || y !=viewport_y
		)
	{
		glViewport( x,y, sx,sy );
		viewport_sx = sx;
		viewport_sy = sy;
		viewport_x  = x;
		viewport_y  = y;
	}
}

void GOL::get_viewport( INT32& x, INT32& y, INT32& sx, INT32& sy )
{
	sx = viewport_sx;
	sy = viewport_sy;
	x  = viewport_x;
	y  = viewport_y;
}



