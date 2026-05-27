
#ifdef AAA_BOX_H
#error "BOX_H included more than once."
#endif
#define AAA_BOX_H 1


#ifndef AAA_OBJ_H
#	include "infrastructure/obj/obj.h"
#endif

namespace box
{
	extern void init();
	extern void deinit();
};


// BOX DRAWING
//
extern	void	draw_box_min_max( FP32 CONST min_x, FP32 CONST min_y, FP32 CONST min_z, FP32 CONST max_x, FP32 CONST max_y, FP32 CONST max_z );
extern	void	draw_box_at_tgn( FP32 CONST * CONST size, FP32 CONST * CONST pos, FP32 CONST * CONST u, FP32 CONST * CONST v, FP32 CONST * CONST nor );
FINLINE	void	draw_box_sxyz( FP32 size_x, FP32 size_y, FP32 size_z )
{
	size_x *= .5;
	size_y *= .5;
	size_z *= .5;
	draw_box_min_max( -size_x, -size_y, -size_z, size_x, size_y, size_z );
}
FINLINE	void	draw_box_sxyz_at( FP32 size_x, FP32 size_y, FP32 size_z, FP32 CONST * CONST pos )
{
	size_x *= .5;
	size_y *= .5;
	size_z *= .5;
	draw_box_min_max(	pos[0]-size_x, pos[1]-size_y, pos[2]-size_z,	pos[0]+size_x, pos[1]+size_y, pos[2]+size_z	);
}
FINLINE	void	draw_box(		FP32 CONST * CONST size )								{	draw_box_sxyz( size[0], size[1], size[2] );	}
FINLINE	void	draw_box_at(	FP32 CONST * CONST size, FP32 CONST * CONST pos )			{	draw_box_sxyz_at( size[0], size[1], size[2], pos );	}

extern void	draw_box_min_max_line( FP32 CONST min_x, FP32 CONST min_y, FP32 CONST min_z, FP32 CONST max_x, FP32 CONST max_y, FP32 CONST max_z );
//extern	void	draw_box_at_tgn_line( FP32 CONST* CONST size, FP32 CONST* CONST pos, FP32 CONST* CONST u, FP32 CONST* CONST v, FP32 CONST* CONST nor );
FINLINE	void	draw_box_sxyz_line( FP32 size_x, FP32 size_y, FP32 size_z )
{
	size_x *= .5;
	size_y *= .5;
	size_z *= .5;
	draw_box_min_max_line( -size_x, -size_y, -size_z, size_x, size_y, size_z );
}
FINLINE	void	draw_box_sxyz_at_line( FP32 size_x, FP32 size_y, FP32 size_z, FP32 CONST* CONST pos )
{
	size_x *= .5;
	size_y *= .5;
	size_z *= .5;
	draw_box_min_max_line(	pos[0]-size_x, pos[1]-size_y, pos[2]-size_z,	pos[0]+size_x, pos[1]+size_y, pos[2]+size_z	);
}
FINLINE	void	draw_box_line(		FP32 CONST * CONST size )								{	draw_box_sxyz_line( size[0], size[1], size[2] );	}
FINLINE	void	draw_box_at_line(	FP32 CONST * CONST size, FP32 CONST* CONST pos )		{	draw_box_sxyz_at_line( size[0], size[1], size[2], pos );	}

extern void		draw_box_min_max_top_line( FP32 CONST min_x, FP32 CONST min_y, FP32 CONST min_z, FP32 CONST max_x, FP32 CONST max_y, FP32 CONST max_z );
//extern	void	draw_box_top_at_tgn_line( FP32 CONST* CONST size, FP32 CONST* CONST pos, FP32 CONST* CONST u, FP32 CONST* CONST v, FP32 CONST* CONST nor );
FINLINE	void	draw_box_sxyz_top_line( FP32 size_x, FP32 size_y, FP32 size_z )
{
	size_x *= .5;
	size_y *= .5;
	size_z *= .5;
	draw_box_min_max_top_line( -size_x, -size_y, -size_z, size_x, size_y, size_z );
}
FINLINE	void	draw_box_sxyz_top_line_at( FP32 size_x, FP32 size_y, FP32 size_z, FP32 CONST * CONST pos )
{
	size_x *= .5;
	size_y *= .5;
	size_z *= .5;
	draw_box_min_max_top_line(	pos[0]-size_x, pos[1]-size_y, pos[2]-size_z,	pos[0]+size_x, pos[1]+size_y, pos[2]+size_z	);
}
FINLINE	void	draw_box_top_line(		FP32 CONST * CONST size )							{	draw_box_sxyz_top_line( size[0], size[1], size[2] );	}
FINLINE	void	draw_box_top_line_at(	FP32 CONST * CONST size, FP32 CONST * CONST pos )	{	draw_box_sxyz_top_line_at( size[0], size[1], size[2], pos );	}

//
//	CUBE
//
extern	void	draw_cube( FP32 CONST size );
FINLINE	void	draw_cube_at( FP32 size, FP32 CONST * CONST pos )
{
	size *= .5;
	draw_box_min_max(	pos[0]-size, pos[1]-size, pos[2]-size,	pos[0]+size, pos[1]+size, pos[2]+size	);
}
FINLINE	void	draw_cube_at_tgn( FP32 CONST size, FP32 CONST * CONST pos, FP32 CONST * CONST u, FP32 CONST * CONST v, FP32 CONST * CONST nor )
{
	FP32 siz[3];
	siz[0] = siz[1] = siz[2] = size;
	draw_box_at_tgn( siz, pos, u, v, nor );
}

extern	void	draw_cube_cano();
