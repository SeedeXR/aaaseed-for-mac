#ifdef AAA_FLEX_UTILS_H
#error "FLEX/UTILS_H included more than once."
#endif
#define AAA_FLEX_UTILS_H 1


#if !defined(AAA_FLEX_SDK_H)
#	include "flex_sdk.h"
#endif


#if AAA_USE_FLEX()
void flex_set_default_simulation_params( NvFlexParams& params );
#endif	//#if AAA_USE_FLEX()
