#ifdef AAA_WRAP_LIBUSB_H
#error "WRAP_LIBUSB_H included more than once."
#endif
#define AAA_WRAP_LIBUSB_H 1


#ifndef AAA_USB_H
#	include "libusb\include\usb.h"
#endif
#ifndef AAA_WRAP_LOADER_H
#	include "lib_wrappers/wrap_loader.h"
#endif

//#define		USE_LINKED_LIB		// protector
#ifdef		USE_LINKED_LIB		// do wrapped DLL calls
#	include <lib_use.h>
	AAA_LIB_USE32( "libusb0" )
#endif

#define WRAP_DLL_NAME		"libusb0"
#define WRAP_API_MACRO_H	"wrap_libusb_api_Macro.h"
#define WRAP_CLASS_NAME		c_dll_libusb
#define WRAP_CONV_CALL		__cdecl

#include "lib_wrappers/wrap_class.h"

//Maa 2023 Jan special case because it crash as static
extern	WRAP_CLASS_NAME		dll_libusb;

#undef WRAP_CLASS_NAME
#undef WRAP_CONV_CALL


//// ---------------------------------------------------------------------------
//// libusb used for kinect first version DLL Wrapper Init/Term - loads the libusb.dll, wraps the calls.
//// Functions are to be implemented in both forms (linked and wrapped)
//// In form of linked DLL - they do nothing, "print" and return NO_ERROR
////
//UINT32 wrap_libusb_Init ( void );   // returns: winerror code
//UINT32 wrap_libusb_Term ( void );   // returns: winerror code
