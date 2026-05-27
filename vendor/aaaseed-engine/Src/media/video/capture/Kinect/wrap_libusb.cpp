#include "wrap_libusb.h"

c_dll_libusb	dll_libusb;

//#include "libusb\include\usb.h"
//
//#include "aaa_def.h"
//#include "err.h"
//
//#include "wrap_libusb.h"
//#define		WRAP_SECTION_NAME	"LIBUSB"
//#include "platform/win32/wrap_dll.h"
//
//#if AAA_WIN64()
//#define		USE_LINKED_LIBUSB		// protector
//#endif
//
//#ifdef		USE_LINKED_LIBUSB		// do wrapped DLL calls
//
//#if AAA_WIN64()
//#	include <lib_use.h>
//	AAA_LIB_USE32( "libusb/msvc_x64/libusb" )
//#else
//	AAA_LIB_USE32( "libusb/msvc/libusb" )
//#endif
//#define		WRAPPER_NAME	libusb
//
////-----------------------------------------------------------------------------
//// Ritual dance - fake the wrap_XXX_Init/Term implementation for linked form
////-----------------------------------------------------------------------------
//UINT32	wrap_libusb_Init ( void )
//{
//	return( WRAP_DLL_LINKED( WRAPPER_NAME ) );		// just give a data structure to work
//}//wrap_triclops_Init
////-----------------------------------------------------------------------------
//UINT32	wrap_libusb_Term ( void )
//{
//	return( UNWRAP_DLL_LINKED( WRAPPER_NAME ) );	// just give a data structure to work
//}//wrap_triclops_Term
////-----------------------------------------------------------------------------
//
//#else	//  USE_LINKED_LIBUSB
////=============================================================================
//// Wrapper NAKED (ritual dance part 1)
//// NOTE  For the wrapper implementation part we  use "__declspec(naked)"
//// NOTE: The __cdecl has to be there, since the library interface is defined so.
//
//#undef	USBLIB_API
//#define	USBLIB_API	__declspec(naked)
//
////=============================================================================
//// Wrapper static data (ritual dance part 2)
////
//DECL_WRAPPER_DLL( LIBUSB, "libusb0.dll" )
////
//// ============================================================================
//
//// ============================================================================
////
//// Wrappers, see the "usb.h" for semantic declarations
////
////-----------------------------------------------------------------------------
//
//	USBLIB_API	usb_dev_handle*	MAAAPIENTRY	usb_open(struct usb_device *dev)
//	WRAP_CALL2JUMP( usb_open )
//
//	USBLIB_API	int	MAAAPIENTRY	usb_close(usb_dev_handle *dev)
//	WRAP_CALL2JUMP( usb_close )
//
//	USBLIB_API	int	MAAAPIENTRY	usb_control_msg(usb_dev_handle *dev, int requesttype, int request, int value, int index, char *bytes, int size, int timeout)
//	WRAP_CALL2JUMP( usb_control_msg )
//
//	USBLIB_API	int	MAAAPIENTRY	usb_set_configuration(usb_dev_handle *dev, int configuration)
//	WRAP_CALL2JUMP( usb_set_configuration )
//
//	USBLIB_API	int	MAAAPIENTRY	usb_claim_interface(usb_dev_handle *dev, int interface)
//	WRAP_CALL2JUMP( usb_claim_interface )
//
//	USBLIB_API	int	MAAAPIENTRY	usb_release_interface(usb_dev_handle *dev, int interface)
//	WRAP_CALL2JUMP( usb_release_interface )
//
//	USBLIB_API	int	MAAAPIENTRY	usb_clear_halt(usb_dev_handle *dev, unsigned int ep)
//	WRAP_CALL2JUMP( usb_clear_halt )
//
//	USBLIB_API	int	MAAAPIENTRY	usb_reset(usb_dev_handle *dev)
//	WRAP_CALL2JUMP( usb_reset )
//
//	USBLIB_API	char*	MAAAPIENTRY	usb_strerror(void)
//	WRAP_CALL2JUMP( usb_strerror )
//
//	USBLIB_API	void	MAAAPIENTRY	usb_init(void)
//	WRAP_CALL2JUMP( usb_init )
//
//	USBLIB_API	void	MAAAPIENTRY	usb_set_debug(int level)
//	WRAP_CALL2JUMP( usb_set_debug )
//
//	USBLIB_API	int	MAAAPIENTRY	usb_find_busses(void)
//	WRAP_CALL2JUMP( usb_find_busses )
//
//	USBLIB_API	int	MAAAPIENTRY	usb_find_devices(void)
//	WRAP_CALL2JUMP( usb_find_devices )
//
//	USBLIB_API	struct usb_device*	MAAAPIENTRY	usb_device(usb_dev_handle *dev)
//	WRAP_CALL2JUMP( usb_device )
//
//	USBLIB_API	struct usb_bus*	MAAAPIENTRY	usb_get_busses(void)
//	WRAP_CALL2JUMP( usb_get_busses )
//
//	USBLIB_API	int	MAAAPIENTRY	usb_isochronous_setup_async(usb_dev_handle *dev, void **context, unsigned char ep, int pktsize)
//	WRAP_CALL2JUMP( usb_isochronous_setup_async )
//
//	USBLIB_API	int	MAAAPIENTRY	usb_bulk_setup_async(usb_dev_handle *dev, void **context, unsigned char ep)
//	WRAP_CALL2JUMP( usb_bulk_setup_async )
//
//	USBLIB_API	int	MAAAPIENTRY	usb_interrupt_setup_async(usb_dev_handle *dev, void **context, unsigned char ep)
//	WRAP_CALL2JUMP( usb_interrupt_setup_async )
//
//	USBLIB_API	int	MAAAPIENTRY	usb_submit_async(void *context, char *bytes, int size)
//	WRAP_CALL2JUMP( usb_submit_async )
//
//	USBLIB_API	int	MAAAPIENTRY	usb_reap_async(void *context, int timeout)
//	WRAP_CALL2JUMP( usb_reap_async )
//
//	USBLIB_API	int	MAAAPIENTRY	usb_reap_async_nocancel(void *context, int timeout)
//	WRAP_CALL2JUMP( usb_reap_async_nocancel )
//
//	USBLIB_API	int	MAAAPIENTRY	usb_cancel_async(void *context)
//	WRAP_CALL2JUMP( usb_cancel_async )
//
//	USBLIB_API	int	MAAAPIENTRY	usb_free_async(void **context)
//	WRAP_CALL2JUMP( usb_free_async )
//
////=============================================================================
//// ritual dance (part3)
////=============================================================================
////
//DECL_WRAPPER_END( LIBUSB )
////
//UINT32	wrap_libusb_Init( void )
//{
//	return( WRAP_DLL( LIBUSB ) );		// just give a data structure to work
//}//wrap_libusb_Init
////-----------------------------------------------------------------------------
//UINT32	wrap_libusb_Term( void )
//{
//	return( UNWRAP_DLL( LIBUSB ) );	// just give a data structure to work
//}//wrap_libusb_Term
////-----------------------------------------------------------------------------
//int //libusbError
//_unresolved_call( void )
//{
//	WRAP_UNRESOLVED( LIBUSB );		// just to get a 'fancy' error printout
//	return( USB_ERROR_BEGIN );
//}//_unresolved_call
//
//#endif//USE_LINKED_LIBUSB
