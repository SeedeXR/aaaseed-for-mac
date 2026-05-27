
#ifdef AAA_NATIVE_SYSTEM_H
#error "NATIVE_SYSTEM_H included more than once."
#endif
#define AAA_NATIVE_SYSTEM_H 1

/*
 Selecting operating system, from (NATIVE_OS_type)
 
 DARWIN   - Darwin OS (same as NATIVE_OS_MAC)
 WIN32    - Win32 (Windows 2000/XP/Vista/7 and Windows Server 2003/2008)
 LINUX    - Linux
 FREEBSD  - FreeBSD
 NETBSD   - NetBSD
 OPENBSD  - OpenBSD
 BSDI     - BSD/OS
 BSD4     - Various BSD 4.4 system
 UNIX     - Various UNIX BSD/SYSV system
 */

#if defined(__APPLE__) && (defined(__GNUC__) || defined(__xlC__) || defined(__xlc__))
#  define NATIVE_OS_DARWIN
#  define NATIVE_OS_BSD4
#  define NATIVE_NO_STL_SWAP
#ifndef __MACOSX_CORE__
	#define __MACOSX_CORE__
#endif
#  ifdef __LP64__
#    define NATIVE_OS_DARWIN64
#    define NATIVE_OS_64 1
#  else
#    define NATIVE_OS_DARWIN32
#  endif
#elif !defined(SAG_COM) && (defined(WIN64) || defined(_WIN64) || defined(__WIN64__))
#  define NATIVE_OS_WIN32
#  define NATIVE_OS_WIN64
#  define NATIVE_OS_64 1
#elif !defined(SAG_COM) && (defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__))
#  if defined(WINCE) || defined(_WIN32_WCE)
#    define NATIVE_OS_WINCE
#  else
#    define NATIVE_OS_WIN32
#    define NATIVE_COMPILER_MANGLES_RETURN_TYPE
#  endif
#elif defined(__linux__) || defined(__linux)
#  define NATIVE_OS_LINUX
#if defined (__LP64__) || defined (_LP64)
#define NATIVE_OS_64 1
#endif
#elif defined(__FreeBSD__) || defined(__DragonFly__)
#if defined (__LP64__) || defined (_LP64)
#define NATIVE_OS_64 1
#endif
#  define NATIVE_OS_FREEBSD
#  define NATIVE_OS_BSD4
#elif defined(__NetBSD__)
#if defined (__LP64__) || defined (_LP64)
#define NATIVE_OS_64 1
#endif
#  define NATIVE_OS_NETBSD
#  define NATIVE_OS_BSD4
#elif defined(__OpenBSD__)
#if defined (__LP64__) || defined (_LP64)
#define NATIVE_OS_64 1
#endif
#  define NATIVE_OS_OPENBSD
#  define NATIVE_OS_BSD4
#elif defined(__bsdi__)
#if defined (__LP64__) || defined (_LP64)
#define NATIVE_OS_64 1
#endif
#  define NATIVE_OS_BSDI
#  define NATIVE_OS_BSD4
#else
#  error "Not supported build plateform"
#endif

#if defined(NATIVE_OS_WIN32) || defined(NATIVE_OS_WIN64) || defined(NATIVE_OS_WINCE)
	#define NATIVE_OS_WIN
#endif


/*
   Selecting window system type (NATIVE_WS_type):

	 MACX     - Mac OS X
	 WIN32    - Windows
	 X11      - X Window System
*/

#if defined(NATIVE_OS_MSDOS)
#  define NATIVE_WS_WIN16
#  error "CPGP requires Win32 and does not work with Windows 3.x"
#elif defined(_WIN32_X11_)
#  define NATIVE_WS_X11
#elif defined(NATIVE_OS_WIN32)
#  define NATIVE_WS_WIN32
#  if defined(NATIVE_OS_WIN64)
#    define NATIVE_WS_WIN64
#  endif
#elif defined(NATIVE_OS_WINCE)
#  define NATIVE_WS_WIN32
#  define NATIVE_WS_WINCE
#  if defined(NATIVE_OS_WINCE_WM)
#    define NATIVE_WS_WINCE_WM
#  endif
#elif defined(NATIVE_OS_UNIX)
#  if defined(NATIVE_OS_MAC) && !defined(__USE_WS_X11__) && !defined(NATIVE_WS_QWS) && !defined(NATIVE_WS_QPA)
#    define NATIVE_WS_MAC
#    define NATIVE_WS_MACX
#    if defined(NATIVE_OS_MAC64)
#      define NATIVE_WS_MAC64
#    elif defined(NATIVE_OS_MAC32)
#      define NATIVE_WS_MAC32
#    endif
#  elif !defined(NATIVE_WS_QWS) && !defined(NATIVE_WS_QPA)
#    define NATIVE_WS_X11
#  endif
#endif

#if defined(NATIVE_WS_WIN16) || defined(NATIVE_WS_WIN32) || defined(NATIVE_WS_WIN64) || defined(NATIVE_WS_WINCE) 
#  define NATIVE_WS_WIN
#endif

// Intrisics
#ifdef  NATIVE_ATOMIC_USE_WIN32_INTERLOCKED
	#define NATIVE_USE_INTRINSICS 1
	#include <intrin.h>
#endif



/*
 Selecting compiler type: (NATIVE_CC_type)
 
 MSVC     - Microsoft Visual C/C++, Intel C++ for Windows
 GNU      - GNU C++
 MINGW    - Minimal GNU for Windows
 */
//! Defines for s{w,n}printf because these methods do not match the ISO C
//! standard on Windows platforms, but it does on all others.
//! These should be int snprintf(char *_inputString, size_t size, const char *format, ...);
//! and int swprintf(wchar_t *wcs, size_t maxlen, const wchar_t *format, ...);

#if defined(_MSC_VER) && _MSC_VER > 1310 && !defined (_WIN32_WCE)
	#ifndef crtsecure
		#define crtsecure _CRT_SECURE_NO_DEPRECATE
	#endif //crtsecure
	#include <stdio.h>
#ifndef snprintf
	//#define snprintf _snprintf_s better but not
	#define snprintf sprintf_s
#endif  //snprintf
#ifndef swprintf
	#define swprintf swprintf_s
#endif //swprintf
#if _MSC_VER >= 1600
	#define NATIVE_COMPILER_SUPPORTS_NULLPTR 1
	#define NATIVE_COMPILER_SUPPORTS_MOVE_SEMANTICS 1
	#define _ALLOW_KEYWORD_MACROS 1 // (to stop VC2012 complaining)
#endif  //_MSC_VER >= 1600
#else // defined(_MSC_VER) && _MSC_VER > 1310 && !defined (_WIN32_WCE)
#ifndef snprintf
#ifdef NATIVE_OS_WIN32 // //
	#define snprintf _snprintf
#endif //NATIVE_OS_WIN32 // //
#endif  //snprintf
#endif



//----- Common extensions of the STL
#ifdef __GNUC__
#  if defined LB_GCC_4_3_OR_LATER && !defined __INTEL_COMPILER
#    define NATIVE_STDEXT_TR1
#  else
#    define NATIVE_STDEXT_EXT
#    ifndef NATIVE_HAVE_LONG_HASH
#      define NATIVE_HAVE_LONG_HASH
#    endif
#  endif
#else
#  ifdef _MSC_VER
#    define NATIVE_STDEXT_MSVC
#  else
#    define NATIVE_STDEXT_STD
#  endif
#endif



#if defined(_MSC_VER) && (_MSC_VER >= 1600)
	
#if 0	//2025 November maa disagree
// The following are real warnings but are generated by almost all MS headers, including  
	// standard library headers, so it's impractical to leave them on.  
	#pragma  warning( disable: 4996 )   // MSVC 9: a C std library function has been "deprecated" (says MS)
	#pragma  warning( disable: 4619 )   // there is no warning number 'XXXX'  
	#pragma  warning( disable: 4668 )   // XXX is not defined as a preprocessor macro  


	// The following are pure sillywarnings:  
	#pragma warning( disable: 4061 )    // enum value is not *explicitly* handled in switch  
	#pragma warning( disable: 4099 )    // first seen using 'struct' now seen using 'class'  
	#pragma warning( disable: 4127 )    // conditional expression is constant  
	#pragma warning( disable: 4217 )    // member template isn't copy constructor  
	#pragma warning( disable: 4250 )    // inherits (implements) some member via dominance  
	#pragma warning( disable: 4251 )    // needs to have dll-interface to be used by clients  
	#pragma warning( disable: 4275 )    // exported class derived from non-exported class  
	#pragma warning( disable: 4347 )    // "behavior change", function called instead of template  
	#pragma warning( disable: 4355 )    // "'this': used in member initializer list  
	#pragma warning( disable: 4428 )    // MSVC 9: universal-character-name encountered in source  
	#pragma warning( disable: 4505 )    // unreferenced function has been removed  
	#pragma warning( disable: 4510 )    // default constructor could not be generated  
	#pragma warning( disable: 4511 )    // copy constructor could not be generated  
	#pragma warning( disable: 4512 )    // assignment operator could not be generated  
	#pragma warning( disable: 4513 )    // destructor could not be generated  
	#pragma warning( disable: 4610 )    // can never be instantiated user defined constructor required  
	#pragma warning( disable: 4623 )    // default constructor could not be generated  
	#pragma warning( disable: 4624 )    // destructor could not be generated  
	#pragma warning( disable: 4625 )    // copy constructor could not be generated  
	#pragma warning( disable: 4626 )    // assignment operator could not be generated  
	#pragma warning( disable: 4640 )    // a local static object is not thread-safe  
	#pragma warning( disable: 4661 )    // a member of the template class is not defined.  
	#pragma warning( disable: 4670 )    // a base class of an exception class is inaccessible for catch  
	#pragma warning( disable: 4672 )    // a base class of an exception class is ambiguous for catch  
	#pragma warning( disable: 4673 )    // a base class of an exception class is inaccessible for catch  
	#pragma warning( disable: 4675 )    // resolved overload was found by argument-dependent lookup  
	#pragma warning( disable: 4702 )    // unreachable code, e.g. in <list> header.  
	#pragma warning( disable: 4710 )    // call was not inlined  
	#pragma warning( disable: 4711 )    // call was inlined  
	#pragma warning( disable: 4820 )    // some padding was added  
	#pragma warning( disable: 4917 )    // a GUID can only be associated with a class, interface or namespace  
	#pragma warning( disable: 4290 )    // C++ exception specification ignored except to indicate a function is not __declspec(nothrow), A function is declared using exception specification, which Visual C++ accepts but does not implement.
#endif

	/* MSVC does not support SSE/MMX on x64 */
	#  if defined(_M_X64)
	#    undef NATIVE_HAVE_SSE
	#    undef NATIVE_HAVE_MMX
	#    undef NATIVE_HAVE_3DNOW
	#  endif //(_M_X64)

	#      define NATIVE_COMPILER_RVALUE_REFS
	#      define NATIVE_COMPILER_AUTO_TYPE
	#      define NATIVE_COMPILER_LAMBDA
	#      define NATIVE_COMPILER_DECLTYPE

#endif // defined(_MSC_VER) && (_MSC_VER >= 1600)




#ifdef __GNUC__
#  define NATIVE_CC_GNU
#  define NATIVE_C_CALLBACKS
#  if defined(__MINGW32__)
#    define NATIVE_CC_MINGW
#endif //(__MINGW32__)
#  if (( __GNUC__ > 4 ) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 0)) )
#    define NATIVE_GCC_4_0_OR_LATER
#  endif
#  if (( __GNUC__ > 4 ) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 1)) )
#    define NATIVE_GCC_4_1_OR_LATER
#  endif
#  if (( __GNUC__ > 4 ) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 2)) )
#    define NATIVE_GCC_4_2_OR_LATER
#  endif
#  if (( __GNUC__ > 4 ) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 3)) )
#    define NATIVE_GCC_4_3_OR_LATER
#  endif
#  if (( __GNUC__ > 4 ) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 4)) )
#    define NATIVE_GCC_4_4_OR_LATER
#  endif
#  if (( __GNUC__ > 4 ) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5)) )
#    define NATIVE_GCC_4_5_OR_LATER
#  endif
#  if (( __GNUC__ > 4 ) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 6)) )
#    define NATIVE_GCC_4_6_OR_LATER
#  endif
#  if (( __GNUC__ > 4 ) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 7)) )
#    define NATIVE_GCC_4_7_OR_LATER
#  endif
#  if (( __GNUC__ > 4 ) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 8)) )
#    define NATIVE_GCC_4_8_OR_LATER
#  endif
#  if (( __GNUC__ > 4 ) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 9)) )
#    define NATIVE_GCC_4_9_OR_LATER
#  endif
#endif // GCC



#if defined (NATIVE_OS_WIN32)
	/** This macro will be set to 1 if the compiler's native wchar_t is an 8-bit type. */
	#define NATIVE_WCHAR_IS_UTF8      0
	/** This macro will be set to 1 if the compiler's native wchar_t is a 16-bit type. */
	#define NATIVE_WCHAR_IS_UTF16     1
	/** This macro will be set to 1 if the compiler's native wchar_t is a 32-bit type. */
	#define NATIVE_WCHAR_IS_UTF32     0
#else //defined (NATIVE_OS_WIN32)
	/** This macro will be set to 1 if the compiler's native wchar_t is an 8-bit type. */
	#define NATIVE_WCHAR_IS_UTF8      0
	/** This macro will be set to 1 if the compiler's native wchar_t is a 16-bit type. */
	#define NATIVE_WCHAR_IS_UTF16     0
	/** This macro will be set to 1 if the compiler's native wchar_t is a 32-bit type. */
	#define NATIVE_WCHAR_IS_UTF32     1
#endif // defined (NATIVE_OS_WIN32)

