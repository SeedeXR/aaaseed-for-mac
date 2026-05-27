
#ifdef AAA_AXE_H
#error "AXE_H included more than once."
#endif
#define AAA_AXE_H 1


#ifndef	AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif


namespace	n_axe
{
	//todo
	CONSTEXPR INT32	DRAW_NONE	= 0;
	CONSTEXPR INT32	DRAW_X		= 1;
	CONSTEXPR INT32 DRAW_Y		= 2;
	CONSTEXPR INT32	DRAW_Z		= 4;
	CONSTEXPR INT32 DRAW_ALL	= DRAW_X | DRAW_Y | DRAW_Z;

	extern	bool	b_line_smooth;
	extern	REAL	line_width_plane;
	extern	REAL	line_width;

	extern	void	init();
	extern	void	update();

	//todo generalize in a draw_debug 
	extern	void	push_draw();
	extern	void	pop_draw();

	extern 	void	draw_null_3d( REAL CONST x, REAL CONST y, REAL CONST z, REAL CONST size=1. );
	FINLINE	void	draw_null_3d( REAL CONST * CONST pos, REAL CONST size=1. )						{	draw_null_3d(	pos[0],	pos[1],	pos[2],	size	);	}
	extern	void	draw_null_3d( REAL CONST size=1. );

	extern	void	draw_null_3d_list();

	extern	void	draw_null_2d( REAL CONST x, REAL CONST y, REAL CONST z, REAL CONST size=1., INT32 CONST axe=2 );
	FINLINE	void	draw_null_2d( REAL CONST * CONST pos, REAL CONST size=1., INT32 CONST axe=2 )	{	draw_null_2d(	pos[0],	pos[1],	pos[2],	size, axe	);	}
	extern	void	draw_null_2d( REAL CONST size=1., INT32 CONST axe=2 );


	extern	void	draw_null_2d_list( INT32 CONST axe );

	extern	void	draw_units();

	extern	void	draw_origin_geo();
	extern  void	draw_orientation(	FP32 CONST size );
	extern	void	draw_origin_geo(	FP32 CONST size );
	extern	void	draw_origin();
	extern	void	draw_origin(		FP32 CONST size );

	extern	void	draw_axe_and_plane( INT32 CONST axe_cur );
	extern	void	draw_axe();
}

