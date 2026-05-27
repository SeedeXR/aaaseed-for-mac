
#ifdef AAA_COMMAND_H
#error "COMMAND_H included more than once."
#endif
#define AAA_COMMAND_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

class	c_obj_ui;

void	command_do(						C_PCHAR_C str );
void	command_send(					C_PCHAR_C str );
void	command_send(	c_obj_ui* pobj,	C_PCHAR_C str_in);


