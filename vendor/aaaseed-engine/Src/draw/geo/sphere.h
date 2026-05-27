#pragma once

#ifdef AAA_SPHERE_H
#error "SPHERE_H included more than once."
#endif
#define AAA_SPHERE_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
//#ifndef AAA_V_H
//#	include "math/v.h"
//#endif

extern	void	draw_sphere(		REAL size,												INT32 CONST yaw_nb = 40,	INT32 CONST pitch_nb = 20 );

#if AAA_REAL_IS_DOUBLE()
extern	void	draw_sphere_at(		REAL CONST * CONST size,	REAL CONST * CONST pos,		INT32 CONST yaw_nb = 40,	INT32 CONST pitch_nb = 20 );
#endif
extern	void	draw_sphere_at(		FP32 CONST * CONST size,	FP32 CONST * CONST pos,	INT32 CONST yaw_nb = 40,	INT32 CONST pitch_nb = 20 );
FINLINE	void	draw_sphere_at(		REAL CONST size,			REAL CONST * CONST pos,		INT32 CONST yaw_nb = 40,	INT32 CONST pitch_nb = 20 )
{
	FP32	s[3] = { size, size, size };
//	set_v3( s, size );
	draw_sphere_at(	s, pos,  yaw_nb, pitch_nb );
}
extern	void	draw_sphere_ui_at(	REAL CONST * CONST size,	REAL CONST * CONST pos,		INT32 CONST yaw_nb,			INT32 CONST pitch_nb );
FINLINE	void	draw_sphere_ui_at(	REAL CONST size,			REAL CONST * CONST pos,		INT32 CONST yaw_nb = 40,	INT32 CONST pitch_nb = 20 )
{
	REAL	s[3] = { size, size, size };
//	set_v3( s, size );
	draw_sphere_ui_at( s, pos,  yaw_nb, pitch_nb );
}
