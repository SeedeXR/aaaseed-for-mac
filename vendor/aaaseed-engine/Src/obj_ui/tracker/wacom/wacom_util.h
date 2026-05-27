
#ifdef AAA_WACOM_UTIL_H
#error "WACOM_UTIL_H included more than once."
#endif
#define AAA_WACOM_UTIL_H 1


#ifndef _WINDOWS_
#	include	<windows.h>
#endif
#ifndef _INC_WINTAB
#	include	"tracker/wacom/wintab.h"		// NOTE: get from wactab header package
#endif


// typedef for Wintab functions exported from wintab32.dll. 
typedef UINT ( API * WTINFOA )				( UINT, UINT, LPVOID );
typedef HCTX ( API * WTOPENA )				( HWND, LPLOGCONTEXTA, BOOL );
typedef BOOL ( API * WTGETA )				( HCTX, LPLOGCONTEXT );
typedef BOOL ( API * WTSETA )				( HCTX, LPLOGCONTEXT );
typedef BOOL ( API * WTCLOSE )				( HCTX );
typedef BOOL ( API * WTENABLE )				( HCTX, BOOL );
typedef BOOL ( API * WTPACKET )				( HCTX, UINT, LPVOID );
typedef BOOL ( API * WTOVERLAP )			( HCTX, BOOL );
typedef BOOL ( API * WTSAVE )				( HCTX, LPVOID );
typedef BOOL ( API * WTCONFIG )				( HCTX, HWND );
typedef HCTX ( API * WTRESTORE )			( HWND, LPVOID, BOOL );
typedef BOOL ( API * WTEXTSET )				( HCTX, UINT, LPVOID );
typedef BOOL ( API * WTEXTGET )				( HCTX, UINT, LPVOID );
typedef BOOL ( API * WTQUEUESIZESET )		( HCTX, int );
typedef int  ( API * WTDATAPEEK )			( HCTX, UINT, UINT, int, LPVOID, LPINT );
typedef int  ( API * WTPACKETSGET )			( HCTX, int, LPVOID );
typedef HMGR ( API * WTMGROPEN )			( HWND, UINT );
typedef BOOL ( API * WTMGRCLOSE )			( HMGR );
typedef HCTX ( API * WTMGRDEFCONTEXT )		( HMGR, BOOL );
typedef HCTX ( API * WTMGRDEFCONTEXTEX )	( HMGR, UINT, BOOL );

// Function pointers to Wintab functions exported from wintab32.dll. 
extern WTINFOA				gpWTInfoA;
extern WTOPENA				gpWTOpenA;
extern WTGETA				gpWTGetA;
extern WTSETA				gpWTSetA;
extern WTCLOSE				gpWTClose;
extern WTPACKET				gpWTPacket;
extern WTENABLE				gpWTEnable;
extern WTOVERLAP			gpWTOverlap;
extern WTSAVE				gpWTSave;
extern WTCONFIG				gpWTConfig;
extern WTRESTORE			gpWTRestore;
extern WTEXTSET				gpWTExtSet;
extern WTEXTGET				gpWTExtGet;
extern WTQUEUESIZESET		gpWTQueueSizeSet;
extern WTDATAPEEK			gpWTDataPeek;
extern WTPACKETSGET			gpWTPacketsGet;
extern WTMGROPEN			gpWTMgrOpen;
extern WTMGRCLOSE			gpWTMgrClose;
extern WTMGRDEFCONTEXT		gpWTMgrDefContext;
extern WTMGRDEFCONTEXTEX	gpWTMgrDefContextEx;

// add more typedef and function pointers as needed

extern bool load_wintab( void );
extern void unload_wintab( void );
