#include "aaa_glut.h"

#if	AAA_GLUT_USE()
#	include <lib_use.h>
#	if AAA_DEBUG()
#		define TMP_CONF() "Debug"
#	else
#		define TMP_CONF() "Metal"
#	endif
	AAA_LIB_USE(  "GlutMaa/GlutMaa_" TMP_CONF() "_" AAA_LIB_VSTOOL_STR() "_" AAA_LIB_USE_PLATFORM_SHORT() )
#	undef	TMP_CONF
#endif
