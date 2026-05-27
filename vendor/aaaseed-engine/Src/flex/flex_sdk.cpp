#include "flex_sdk.h"


#if AAA_USE_FLEX()

//there is a bug in flex copy than we solve with a work around using a 2 steps copy using cuda
#	include "Buffer_Gl.h"
#	include "lib_use.h"
//#	if	AAA_DEBUG()
//		AAA_LIB_USE( "flex/NvFlexDebugCUDA_x64" )
//#	else
		AAA_LIB_USE( "NVidia/NvFlexReleaseCuda_x64" )
//#	endif


#endif //#if AAA_USE_FLEX()