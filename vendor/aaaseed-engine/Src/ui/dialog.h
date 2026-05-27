
#ifdef AAA_DIALOG_H
#error "DIALOG_H included more than once."
#endif
#define AAA_DIALOG_H 1


//class	c_param;
#ifndef AAA_PARAM_H
#	include "infrastructure/param/param.h"
#endif

class	c_obj_ui;	
extern	void	th_dialog( c_obj_ui* CONST obj, p_param CONST param,	bool CONST b_simple = false,
																		C_PCHAR_C title = nullptr,
																		bool CONST b_lua_callback = false,
																		INT32 CONST lua_id = 0,
																		bool CONST b_integer = false );
