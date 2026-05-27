
#ifdef AAA_SYSTEM_ERROR_H
#error "SYSTEM_ERROR_H included more than once."
#endif
#define AAA_SYSTEM_ERROR_H 1


#ifndef	AAA_AAA_TYPE_H
	#include "aaa_type.h"
#endif
#ifndef _STRING_
	#include <string>
#endif

namespace aaa
{
	namespace system
	{
		void		reset_err_message();
		DWORD		get_err_last(		void ); 
		std::string get_err_message(	DWORD error );
		std::string get_err_message(	void );
	}
}

