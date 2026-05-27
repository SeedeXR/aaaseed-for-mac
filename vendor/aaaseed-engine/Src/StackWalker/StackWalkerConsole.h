
/* Define one file to protect from multiple include*/
#ifndef StackWalkerConsol_h_
#define StackWalkerConsol_h_

#include "seed.h"
#include "StackWalker.h"

#include <tchar.h>
#include <stdio.h>
#include <iostream> // To kill and use AAASeed native messaging system

class StackWalkerToConsole : public StackWalker
{
protected:
  virtual void OnOutput(LPCSTR szText)
  {
		std::cout << szText << std::endl;//native_info_msg( "StackWalkerToConsole::OnOutput", "%s \n", szText  );
  }
};

class StackWalkerToFile : public StackWalker
{
protected:
  virtual void OnOutput(LPCSTR szText)
  {
		std::cout << szText << std::endl;//native_error_msg( "StackWalkerToFile", "%s", szText);
  }
};

#ifdef __cplusplus
extern "C" {
#endif 

// For more info about "PreventSetUnhandledExceptionFilter" see:
// "SetUnhandledExceptionFilter" and VC8
// http://blog.kalmbachnet.de/?postid=75
// and
// Unhandled exceptions in VC8 and above… for x86 and x64
// http://blog.kalmbach-software.de/2008/04/02/unhandled-exceptions-in-vc8-and-above-for-x86-and-x64/

#if defined _M_X64 || defined _M_IX86
LPTOP_LEVEL_EXCEPTION_FILTER WINAPI MyDummySetUnhandledExceptionFilter(LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter);

BOOL PreventSetUnhandledExceptionFilter();

#else //defined _M_X64 || defined _M_IX86
#pragma message("This code works only for x86 and x64!")
#endif ////defined _M_X64 || defined _M_IX86

LONG __stdcall CrashHandlerExceptionFilter(EXCEPTION_POINTERS* pExPtrs);

void InitUnhandledExceptionFilter( void );

// EXCEPTION_FILTER_TEST USAGE:
/*------------------------------------------
  __try
  {
    ExpTest1();
  }
  __except (ExpFilter(GetExceptionInformation(), GetExceptionCode()))
  {
    print("\n\nException-Handler called\n\n\n");
  }
}
-----------------------------------------*/

LONG WINAPI ExpFilter(EXCEPTION_POINTERS* pExp, DWORD dwExpCode);

#ifdef __cplusplus
}
#endif 

#endif //StackWalkerConsol_h_