
#ifdef AAA_EXECUTE_H
#error "EXECUTE_H included more than once."
#endif
#define AAA_EXECUTE_H 1


#ifndef	AAA_ERR_H
#	include "err.h"
#endif

namespace aaa
{
	AAA_ERR		execute_shell(		C_PCHAR_C command, C_PCHAR_C arg );
	AAA_ERR		execute_process(	C_PCHAR_C command, C_PCHAR_C arg );
}


