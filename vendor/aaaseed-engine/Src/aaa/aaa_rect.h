
#ifdef AAA_AAA_RECT_H
#error "AAA_RECT_H included more than once."
#endif
#define AAA_AAA_RECT_H 1

#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

namespace aaa
{
namespace rect
{
	typedef struct lbrt
	{
		INT32	left;
		INT32	bottom;
		INT32	right;
		INT32	top;
	} lbrt;
	typedef struct lbrt_real
	{
		REAL	left;
		REAL	bottom;
		REAL	right;
		REAL	top;
	} lbrt_real;
	
	typedef struct xy_sxy
	{
		INT32	x;
		INT32	y;
		INT32	sx;
		INT32	sy;
	} xy_sxy;
	typedef struct xy_sxy_real
	{
		REAL	x;
		REAL	y;
		REAL	sx;
		REAL	sy;
	} xy_sxy_real;

	typedef struct lbrt_sxy
	{
		INT32	left;
		INT32	bottom;
		INT32	right;
		INT32	top;
		INT32	sx;
		INT32	sy;
	} lbrt_sxy;
	typedef struct lbrt_sxy_real
	{
		REAL	left;
		REAL	bottom;
		REAL	right;
		REAL	top;
		REAL	sx;
		REAL	sy;
	} lbrt_sxy_real;

}	//namespace rect
}	//namespace aaa

