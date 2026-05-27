
#ifdef AAA_STR_SYMBO_H
#error "STR_SYMBO_H included more than once."
#endif
#define AAA_STR_SYMBO_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

class	gstr
{
public:
	static	C_PCHAR_C	xyz[3];
	static	C_PCHAR_C	rot_order[6];
	static	C_PCHAR_C	uva[3];
	static	C_PCHAR_C	dim[4];
	static	C_PCHAR_C	dim_prim[3];
	static	C_PCHAR_C	current[2];
	static	C_PCHAR_C	borrow[3];
	static	C_PCHAR_C	borrow_once[4];

//	static	C_PCHAR_C	borrow_main[3];
	static	C_PCHAR_C	p2[17];
	static	C_PCHAR_C	hint[3];
	static	C_PCHAR_C	test_func[8];
	static	C_PCHAR_C	draw_number[8];
	static	C_PCHAR_C	no[1];
	static	C_PCHAR_C	front_and_back[3];
//	static	C_PCHAR_C	default[1];
};

#define	PARAM_DEF_AXE_X(	axe_name )	PARAM_DEF_SYMBO( axe_name, 1, 0,	2, gstr::xyz )
#define	PARAM_DEF_AXE_Y(	axe_name )	PARAM_DEF_SYMBO( axe_name, 0, 1,	2, gstr::xyz )
#define	PARAM_DEF_AXE_Z(	axe_name )	PARAM_DEF_SYMBO( axe_name, 0, 2,	2, gstr::xyz )


