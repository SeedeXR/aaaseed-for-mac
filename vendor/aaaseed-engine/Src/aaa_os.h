
#ifdef AAA_AAA_OS_H
#error "AAA_OS_H included more than once."
#endif
#define AAA_AAA_OS_H 1


// This is an OS specific header file
#ifdef WIN32
#	ifndef WIN32_LEAN_AND_MEAN
#		ifndef _WINDOWS_
#			define WIN32_LEAN_AND_MEAN	// Exclude rarely-used stuff from Windows headers
#			include <windows.h>
#		endif
#	endif

//------------------------------------------------------------------------
//	Windows API floor and how to raise it (as of 2026-04-22)
//------------------------------------------------------------------------
//	The vcxproj sets  _WIN32_WINNT=0x0601 ; WINVER=0x0601  (Windows 7).
//	That defines the API set the compiler exposes and therefore the oldest
//	Windows this binary supports at runtime. The SDK version AAASeed is
//	COMPILED against (reported in pref panel as sdk_version, e.g.
//	"10.0.28000.0") is completely separate: the SDK just provides the
//	headers; _WIN32_WINNT gates which of those headers' APIs we're allowed
//	to call. So the binary currently runs on everything from Win7 up,
//	regardless of how new the installed SDK is.
//
//	If/when you want to call a newer-than-Win7 API (e.g. IsWow64Process2,
//	IsWowGuestMachineSupported, any Win10/Win11 specific API), pick one:
//
//	  1. RAISE _WIN32_WINNT in the vcxproj (e.g. 0x0A00 for Win10+).
//	     Pros : cleanest, compiler-checked, all new APIs available.
//	     Cons : drops support for every Windows older than the new floor.
//	            Users on that older Windows get "entry point not found"
//	            at process load time.
//
//	  2. DYNAMICALLY LOAD the newer API via GetProcAddress + null-check.
//	     Pros : _WIN32_WINNT stays at Win7, binary keeps running on old
//	            Windows (the feature just silently degrades there).
//	     Cons : more ceremony per API, bypasses compile-time type checks.
//	     Pattern already used in Src/code_utils/os_version.cpp for
//	     RtlGetVersion -- that's the template to follow:
//	         HMODULE const h = GetModuleHandleW( L"ntdll.dll" );
//	         if( h ) {
//	             auto fn = (SignaturePtr) GetProcAddress( h, "RtlGetVersion" );
//	             if( fn ) { ... }
//	         }
//
//	DO NOT bump _WIN32_WINNT casually; it's a user-facing compatibility
//	decision, not just a compiler setting.
//------------------------------------------------------------------------

#endif