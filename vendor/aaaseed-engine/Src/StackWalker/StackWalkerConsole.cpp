
#include "StackWalkerConsole.h"

#ifdef _WIN32
#include <windows.h>
#include <winbase.h>
#include <io.h>
#endif

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
LPTOP_LEVEL_EXCEPTION_FILTER WINAPI 
  MyDummySetUnhandledExceptionFilter(
  LPTOP_LEVEL_EXCEPTION_FILTER lpTopLevelExceptionFilter)
{
  return NULL;
}

BOOL PreventSetUnhandledExceptionFilter()
{
  HMODULE hKernel32 = LoadLibraryA(/*_T*/("kernel32.dll"));
  if (hKernel32 == NULL) return FALSE;
  void *pOrgEntry = GetProcAddress(hKernel32, 
    "SetUnhandledExceptionFilter");
  if(pOrgEntry == NULL) return FALSE;
 
  DWORD dwOldProtect = 0;
  SIZE_T jmpSize = 5;
#ifdef _M_X64
  jmpSize = 13;
#endif //_M_X64
  BOOL bProt = VirtualProtect(pOrgEntry, jmpSize, 
    PAGE_EXECUTE_READWRITE, &dwOldProtect);
  BYTE newJump[20];
  void *pNewFunc = &MyDummySetUnhandledExceptionFilter;
#ifdef _M_IX86
  DWORD dwOrgEntryAddr = (DWORD) pOrgEntry;
  dwOrgEntryAddr += jmpSize; // add 5 for 5 op-codes for jmp rel32
  DWORD dwNewEntryAddr = (DWORD) pNewFunc;
  DWORD dwRelativeAddr = dwNewEntryAddr - dwOrgEntryAddr;
  // JMP rel32: Jump near, relative, displacement relative to next instruction.
  newJump[0] = 0xE9;  // JMP rel32
  memcpy(&newJump[1], &dwRelativeAddr, sizeof(pNewFunc));
#elif _M_X64
  newJump[0] = 0x49;  // MOV R15, ...
  newJump[1] = 0xBF;  // ...
  memcpy(&newJump[2], &pNewFunc, sizeof (pNewFunc));
  //pCur += sizeof (ULONG_PTR);
  newJump[10] = 0x41;  // JMP R15, ...
  newJump[11] = 0xFF;  // ...
  newJump[12] = 0xE7;  // ...
#endif //_M_IX86
  SIZE_T bytesWritten;
  BOOL bRet = WriteProcessMemory(GetCurrentProcess(),
    pOrgEntry, newJump, jmpSize, &bytesWritten);
 
  if (bProt != FALSE)
  {
    DWORD dwBuf;
    VirtualProtect(pOrgEntry, jmpSize, dwOldProtect, &dwBuf);
  }
  return bRet;
}

#else //defined _M_X64 || defined _M_IX86
#pragma message("This code works only for x86 and x64!")
#endif ////defined _M_X64 || defined _M_IX86

LONG __stdcall CrashHandlerExceptionFilter(EXCEPTION_POINTERS* pExPtrs)
{
#ifdef _M_IX86
  if (pExPtrs->ExceptionRecord->ExceptionCode == EXCEPTION_STACK_OVERFLOW)
  {
    static char MyStack[1024*128];  // be sure that we have enought space...
    // it assumes that DS and SS are the same!!! (this is the case for Win32)
    // change the stack only if the selectors are the same (this is the case for Win32)
    //__asm push offset MyStack[1024*128];
    //__asm pop esp;
  __asm mov eax,offset MyStack[1024*128];
  __asm mov esp,eax;
  }
#endif //_M_IX86

  StackWalkerToFile sw;  // output to console
  sw.ShowCallstack(GetCurrentThread(), pExPtrs->ContextRecord);
  TCHAR lString[500];
  _stprintf_s(lString,
     _T("*** Unhandled Exception! See logFile for more infos!\n")
     _T("   ExpCode: 0x%8.8X\n")
     _T("   ExpFlags: %d\n")
     _T("   ExpAddress: 0x%8.8X\n")
     _T("   Please report!"),
     pExPtrs->ExceptionRecord->ExceptionCode,
     pExPtrs->ExceptionRecord->ExceptionFlags,
     pExPtrs->ExceptionRecord->ExceptionAddress);
  std::cout << "StackWalkerToFile: " << lString << std::endl;
  //NativeMsgServer::release_msg_server();
  //FatalAppExit(-1, lString);
  return EXCEPTION_CONTINUE_SEARCH;
}

static TCHAR s_szExceptionLogFileName[_MAX_PATH] = _T("\\exceptions.log");  // default
static BOOL s_bUnhandledExeptionFilterSet = FALSE;

void InitUnhandledExceptionFilter( void )
{
  /*TCHAR szModName[_MAX_PATH];
  if (GetModuleFileNameA(NULL, szModName, sizeof(szModName)/sizeof(TCHAR)) != 0)
  {
    _tcscpy_s(s_szExceptionLogFileName, szModName);
    _tcscat_s(s_szExceptionLogFileName, _T(".exp.log"));
  }*/
	wchar_t szModName[_MAX_PATH];
  if (GetModuleFileNameW(NULL, szModName, sizeof(szModName)/sizeof(wchar_t)) != 0)
  {
    //_tcscpy_s(s_szExceptionLogFileName, szModName);
    //_tcscat_s(s_szExceptionLogFileName, _T(".exp.log"));
  }
  if (s_bUnhandledExeptionFilterSet == FALSE)
  {
    // set global exception handler (for handling all unhandled exceptions)
    SetUnhandledExceptionFilter(CrashHandlerExceptionFilter);
#if defined _M_X64 || defined _M_IX86
    PreventSetUnhandledExceptionFilter();
#endif //defined _M_X64 || defined _M_IX86
    s_bUnhandledExeptionFilterSet = TRUE;
  }
}

LONG WINAPI ExpFilter(EXCEPTION_POINTERS* pExp, DWORD dwExpCode)
{
  //StackWalker sw;  // output to default (Debug-Window)
  StackWalkerToConsole sw;  // output to the console
  sw.ShowCallstack(GetCurrentThread(), pExp->ContextRecord);
  return EXCEPTION_EXECUTE_HANDLER;
}

#ifdef __cplusplus
}
#endif 