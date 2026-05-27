
#ifdef AAA_AAA_SQL_H
#error "AAA_SQL_H included more than once."
#endif
#define AAA_AAA_SQL_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

extern	void sql_connect();
extern	void sql_query( CHAR* query_string);

#endif	//__AAA_SQL_H__
