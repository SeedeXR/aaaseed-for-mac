

#ifdef AAA_AAA_BITMAP_H
#error "AAA_BITMAP_H included more than once."
#endif
#define AAA_AAA_BITMAP_H 1

#ifndef AAA_AAA_TYPE_H
#include "aaa_type.h"
#endif

enum OP_TYPE : INT32
{
	OP_COPY = 0,
//	OP_ALPHA,
//	OP_LUMINANCE,
/*	OP_RED,
	OP_GREEN,
	OP_BLUE,
	OP_INV,
*/	OP_CANAL,
//	OP_NOISE,
	OP_NOISE_COLOR,
//	OP_SPECIAL,
	OP_BLUE_SCREEN_RAW,
	OP_BLUE_SCREEN,
	OP_BLUE_SCREEN_ALPHA,
	OP_MAX_NB,
};


extern	void	process( INT32 bit_in, INT32 bit_out,
						INT32 s_op, UINT8* src, INT32 w, INT32 h, UINT8* dst,
						REAL* col, REAL* col_dia,
						INT32 CONST sel_r, INT32 CONST sel_g, INT32 CONST sel_b, INT32 CONST sel_a);
