
#ifdef AAA_FLEX_SDK_H
#error "FLEX/FLEX_SDK_H included more than once."
#endif
#define AAA_FLEX_SDK_H 1


#define AAA_USE_FLEX() 1

#if AAA_USE_FLEX()
#	ifndef NV_FLEX_H
#		include "NVidia/NvFlex.h"
#	endif
#else
	struct NvFlexLibrary;
	struct NvFlexSolver;
#endif
