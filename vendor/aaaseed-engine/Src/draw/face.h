
#ifdef AAA_FACE_H
#error "FACE_H included more than once."
#endif
#define AAA_FACE_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

//QUAD
extern	void	draw_quad_fn(			REAL sx, REAL sy, REAL z );
//extern	void	draw_quad_with_axe_fn(	CONST REAL *pos, REAL u, REAL v, INT32 axe, BOOL flip );
//extern	void	draw_quad_with_axe_fn(REAL u, REAL v, REAL z, INT32 axe, BOOL flip);


