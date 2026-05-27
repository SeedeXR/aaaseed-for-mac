
#ifdef AAA_UI_MASTER_H
#error "UI_MASTER_H included more than once."
#endif
#define AAA_UI_MASTER_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

namespace	master
{
	enum NO_ALLOW_FORCE : INT32
	{
		NO = 0,
		ALLOW,
		FORCE,
		MAX_NB
	};
	extern	C_PCHAR_C	str_no_allow_force[NO_ALLOW_FORCE::MAX_NB];

	FINLINE CONSTEXPR bool process_master_switch( NO_ALLOW_FORCE CONST ms, bool CONST b )
	{
		return ms==ALLOW ? b : ms==FORCE ;
	}

};
