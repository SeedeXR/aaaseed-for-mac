
#ifdef AAA_STREAM_UTIL_H
#error "STREAM_UTIL_H included more than once."
#endif
#define AAA_STREAM_UTIL_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

//INT32	stream_read_str_255				( FILE* CONST stream, CHAR* dst );
INT32	stream_read_until_str_nocase	( FILE* CONST stream, C_PCHAR str );
INT32	stream_read_until_str			( FILE* CONST stream, C_PCHAR str );

bool	stream_read_until_lf( FILE* CONST stream );

void	stream_fscanf_string( FILE* CONST stream, CHAR* str, INT32 size_max );

