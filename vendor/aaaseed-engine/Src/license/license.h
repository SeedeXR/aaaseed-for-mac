
#ifdef AAA_LICENSE_H
#error "LICENSE_H included more than once."
#endif
#define AAA_LICENSE_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif


extern	INT32	license_get_year();
extern	CHAR*	license_get_year_str();
extern	INT32	license_get_month();
extern	CHAR*	license_get_month_str();
extern	INT32	license_get_day();
extern	CHAR*	license_get_day_str();
extern	CHAR*	license_get_user();

extern	CHAR*	license_get_version_short();

extern	void	license_read();
extern	void	license_check();
extern	void	license_update();

extern	void	license_generate_what( CHAR* what, CHAR* user, INT32 year, INT32 month, INT32 day, CHAR* version_short, C_PCHAR_C from_who );
extern	void	license_generate_key( CHAR* key, C_PCHAR_C what, C_PCHAR_C langroup, C_PCHAR_C machine );

extern	void	license_check_key( CHAR* key, CHAR* what, CHAR* langroup, CHAR* machine );
extern	void	license_check();
extern	void	license_get_date();
