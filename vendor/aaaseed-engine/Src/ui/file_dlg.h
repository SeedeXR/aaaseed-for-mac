
#ifdef AAA_FILE_DLG_H
#error "FILE_DLG_H included more than once."
#endif
#define AAA_FILE_DLG_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef	AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

class system_window;

namespace aaa::file
{
	extern	o_str		str_file_dialog;

	extern	AAA_ERR	get_filename_open	( o_str & filename, C_PCHAR_C str_message, system_window* parent, C_PCHAR_C str_filter, bool CONST b_multiple );
	extern	AAA_ERR	get_filename_save	( o_str & filename, C_PCHAR_C str_message, system_window* parent, C_PCHAR_C str_filter );
	extern	AAA_ERR	get_folder			( o_str & filename, C_PCHAR_C str_message, system_window* parent );
};


