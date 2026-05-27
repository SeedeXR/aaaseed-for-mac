
#ifdef AAA_LINE_H
#error "LINE_H included more than once."
#endif
#define AAA_LINE_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

extern	void	draw_line(				REAL CONST x0, REAL CONST y0,					REAL CONST x1, REAL CONST y1				);
extern	void	draw_line(				REAL CONST x0, REAL CONST y0, REAL CONST z0,	REAL CONST x1, REAL CONST y1, REAL CONST z1	);
extern	void	draw_line_2r(			REAL	CONST * CONST a, REAL	CONST * CONST b );
extern	void	draw_line(				REAL	CONST * CONST a, REAL	CONST * CONST b );
extern	void	draw_line(				DOUBLE  CONST * CONST a, DOUBLE CONST * CONST b );
extern	void	draw_line(				REAL CONST * pt, INT32 CONST nb, bool CONST b_open );

extern	void	draw_line_z(			REAL CONST * CONST r4 );
extern	void	draw_line(				REAL CONST u1, REAL CONST v1, REAL CONST u2, REAL CONST v2, INT32 CONST axe	);
extern	void	draw_line_u(			REAL CONST u, REAL CONST v, REAL CONST du, INT32 CONST axe );
extern	void	draw_line_v(			REAL CONST u, REAL CONST v, REAL CONST dv, INT32 CONST axe );
extern	void	draw_line_uv(			REAL CONST u, REAL CONST v, REAL CONST du, REAL CONST dv, INT32 CONST axe );

extern	void	draw_line_x(			REAL CONST x0, REAL CONST x1 );
FINLINE	void	draw_line_x(			REAL CONST * CONST val )			{	draw_line_x( val[0], val[1] );	}
extern	void	draw_line_y(			REAL CONST y0, REAL CONST y1 );
FINLINE	void	draw_line_y(			REAL CONST * CONST val )			{	draw_line_y( val[0], val[1] );	}


extern	void	draw_lines_vert_nb(		UINT32 nb, REAL x, REAL CONST step, REAL CONST bottom,	REAL CONST top		);
extern	void	draw_lines_hori_nb(		UINT32 nb, REAL y, REAL CONST step, REAL CONST left,	REAL CONST right	);
