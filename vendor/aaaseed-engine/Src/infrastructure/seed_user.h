#pragma once

#ifdef AAA_SEED_USER_H
#	error "SEED_USER_H included more than once."
#endif
#define AAA_SEED_USER_H 1


#ifndef AAA_ERR_H
#	include "err.h"
#endif

namespace aaa
{
namespace file
{
	extern	C_PCHAR_C	get_user_pref_filename();
	extern	AAA_ERR		read_user_pref();
}
}
