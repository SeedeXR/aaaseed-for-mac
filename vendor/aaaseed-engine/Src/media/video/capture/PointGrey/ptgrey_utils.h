

#ifdef AAA_PTGREY_UTILS_H
#error "PTGREY_UTILS_H included more than once."
#endif
#define AAA_PTGREY_UTILS_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

#define AAA_USE_POINT_GREY() 0
#define AAA_USE_POINT_GREY_PGRFLYCAPTURE() 0

#if AAA_USE_POINT_GREY()
	C_PCHAR	get_camera_gui_error_str( INT32 guierror );
	C_PCHAR	get_digiclops_error_str( INT32 digiclops_error );
	C_PCHAR	triclops_get_error_str( INT32 err_num );
#endif