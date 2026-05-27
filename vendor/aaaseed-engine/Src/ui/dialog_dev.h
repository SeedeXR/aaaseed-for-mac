
#ifdef AAA_DIALOG_DEV_H
#error "DIALOG_DEV_H included more than once."
#endif
#define AAA_DIALOG_DEV_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

namespace n_dialog {
	enum return_code : UINT32
	{
		RETURN_CANCEL	= 0,
		RETURN_OK		= 1,
		RETURN_OTHER	= 2
	};
	extern INT32 do_dev(	C_PCHAR_C title, C_PCHAR_C text, C_PCHAR_C button_other = nullptr );
	extern INT32 do_lua(	C_PCHAR_C title, C_PCHAR_C text, C_PCHAR_C button_other = nullptr );
	extern INT32 do_finger(	C_PCHAR_C title, C_PCHAR_C text, C_PCHAR_C button_other = nullptr );
	extern INT32 do_show(	C_PCHAR_C title, C_PCHAR_C text, C_PCHAR_C button_other = nullptr );
}
