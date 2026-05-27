
#ifdef AAA_LIB_USE_H
#error "LIB_USE_H included more than once."
#endif
#define AAA_LIB_USE_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif
#ifndef AAA_AAA_DEF_H
#	include "aaa_def.h"
#endif

// centralize lib use
#if AAA_WIN64()
#	define AAA_LIB_USE_PLATFORM()		"x64"
#	define AAA_LIB_USE_PLATFORM_SHORT()	"x64"
#else
#	define AAA_LIB_USE_PLATFORM()		"Win32"
#	define AAA_LIB_USE_PLATFORM_SHORT()	"x86"
#endif

#if	AAA_DEBUG()
#	define AAA_LIB_USE_CONFIGURATION()	"Debug"
#else
#	define AAA_LIB_USE_CONFIGURATION()	"Release"	
#endif

//make sure the lib is included for the link and display a message
#define AAA_LIB_USE_MESSAGE(s)	\
	__pragma( comment( lib, s##".lib" )	)\
	__pragma( message( "\tincluded Lib : "s##".lib" ) )

#define AAA_LIB_USE32(s)		AAA_LIB_USE_MESSAGE( "./lib/"##s )
#define AAA_LIB_USE64(s)		AAA_LIB_USE_MESSAGE( "./lib_x64/"##s )

#if AAA_WIN64()
#	define AAA_LIB_USE(s)		AAA_LIB_USE64(s)
#else
#	define AAA_LIB_USE(s)		AAA_LIB_USE32(s)
#endif

#if	AAA_DEBUG()
#	define AAA_LIB_USE_D(s)		AAA_LIB_USE(s##"_d")
#else
#	define AAA_LIB_USE_D(s)		AAA_LIB_USE(s)
#endif

