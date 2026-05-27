
#ifdef AAA_TEXT_DLG_H
#error "TEXT_DLG_H included more than once."
#endif
#define AAA_TEXT_DLG_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

extern	AAA_ERR	execute_str( CHAR* buf, INT32 size );
extern	INT32	generate_str( C_PCHAR_C command, CHAR* buf, INT32 size );
