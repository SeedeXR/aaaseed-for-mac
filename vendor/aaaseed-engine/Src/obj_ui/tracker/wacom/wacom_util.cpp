#ifndef AAA_WACOM_UTIL_H
#	include "wacom_util.h"
#endif
#include "err.h"
#include "system/win32/SystemError.h"


namespace {
	HINSTANCE h_lib_wintab = NULL;
}

WTINFOA				gpWTInfoA			;
WTOPENA				gpWTOpenA			;
WTGETA				gpWTGetA			;
WTSETA				gpWTSetA			;
WTCLOSE				gpWTClose			;
WTPACKET			gpWTPacket			;
WTENABLE			gpWTEnable			;
WTOVERLAP			gpWTOverlap			;
WTSAVE				gpWTSave			;
WTCONFIG			gpWTConfig			;
WTRESTORE			gpWTRestore			;
WTEXTSET			gpWTExtSet			;
WTEXTGET			gpWTExtGet			;
WTQUEUESIZESET		gpWTQueueSizeSet	;
WTDATAPEEK			gpWTDataPeek		;
WTPACKETSGET		gpWTPacketsGet		;
WTMGROPEN			gpWTMgrOpen			;
WTMGRCLOSE			gpWTMgrClose		;
WTMGRDEFCONTEXT		gpWTMgrDefContext	;
WTMGRDEFCONTEXTEX	gpWTMgrDefContextEx	;
// TODO - add more wintab32 function pointers as needed


void clear_wintab( void )
{
	gpWTOpenA			= nullptr;
	gpWTClose			= nullptr;
	gpWTInfoA			= nullptr;
	gpWTPacket			= nullptr;
	gpWTEnable			= nullptr;
	gpWTOverlap			= nullptr;
	gpWTSave			= nullptr;
	gpWTConfig			= nullptr;
	gpWTGetA			= nullptr;
	gpWTSetA			= nullptr;
	gpWTRestore			= nullptr;
	gpWTExtSet			= nullptr;
	gpWTExtGet			= nullptr;
	gpWTQueueSizeSet	= nullptr;
	gpWTDataPeek		= nullptr;
	gpWTPacketsGet		= nullptr;
	gpWTMgrOpen			= nullptr;
	gpWTMgrClose		= nullptr;
	gpWTMgrDefContext	= nullptr;
	gpWTMgrDefContextEx = nullptr;
}

void unload_wintab( void )
{
	//WACOM_TRACE( "UnloadWintab()" );
	if( h_lib_wintab )
	{
		FreeLibrary( h_lib_wintab );
		h_lib_wintab = nullptr;
	}

	clear_wintab();
}

#define GETPROCADDRESS( val, type, func )					\
	gp##func = (type)GetProcAddress( h_lib_wintab, #func ); \
	if( gp##func )	{	val += 1;	}						\
	else			{	ERR_PRINT_STRING( "Can't GetProcAddress for %s()", #func );	}

//////////////////////////////////////////////////////////////////////////////
// Purpose
//		Find wintab32.dll and load it.  
//		Find the exported functions we need from it.
//
//	Returns
//		TRUE on success.
//		FALSE on failure.
//
bool load_wintab( void )
{
	if( h_lib_wintab )
	{
		debug_break( "Wintab32.dll already loaded, this should not happen" );
		return true;
	}

	clear_wintab();
//	ghWintab = LoadLibraryA(  "C:\\dev\\mainline\\Wacom\\Win\\Win32\\Debug\\Wacom_Tablet.dll" );
//	ghWintab = LoadLibraryA(  "C:\\dev\\mainline\\Wacom\\Win\\Win32\\Debug\\Wintab32.dll" );	
	h_lib_wintab = LoadLibraryA( "Wintab32.dll" );	
	if( !h_lib_wintab )
	{
		ERR_PRINT_STRING( "Wintab32.dll LoadLibrary error : %s", aaa::system::get_err_message().c_str() );
		return false;
	}

	// Explicitly find the exported Wintab functions in which we are interested.
	// We are using the ASCII, not unicode versions (where applicable).
	INT32 nb = 0;
	GETPROCADDRESS( nb,		WTOPENA,			WTOpenA				);
	GETPROCADDRESS( nb,		WTINFOA,			WTInfoA				);
	GETPROCADDRESS( nb,		WTGETA,				WTGetA				);
	GETPROCADDRESS( nb,		WTSETA,				WTSetA				);
	GETPROCADDRESS( nb,		WTPACKET,			WTPacket			);
	GETPROCADDRESS( nb,		WTCLOSE,			WTClose				);
	GETPROCADDRESS( nb,		WTENABLE,			WTEnable			);
	GETPROCADDRESS( nb,		WTOVERLAP,			WTOverlap			);
	GETPROCADDRESS( nb,		WTSAVE,				WTSave				);
	GETPROCADDRESS( nb,		WTCONFIG,			WTConfig			);
	GETPROCADDRESS( nb,		WTRESTORE,			WTRestore			);
	GETPROCADDRESS( nb,		WTEXTSET,			WTExtSet			);
	GETPROCADDRESS( nb,		WTEXTGET,			WTExtGet			);
	GETPROCADDRESS( nb,		WTQUEUESIZESET,		WTQueueSizeSet		);
	GETPROCADDRESS( nb,		WTDATAPEEK,			WTDataPeek			);
	GETPROCADDRESS( nb,		WTPACKETSGET,		WTPacketsGet		);
	GETPROCADDRESS( nb,		WTMGROPEN,			WTMgrOpen			);
	GETPROCADDRESS( nb,		WTMGRCLOSE,			WTMgrClose			);
	GETPROCADDRESS( nb,		WTMGRDEFCONTEXT,	WTMgrDefContext		);
	GETPROCADDRESS( nb,		WTMGRDEFCONTEXTEX,	WTMgrDefContextEx	);

	if( nb!=20 )
		GOOD_PRINT_STRING( "Wintab32.dll LoadLibrary found only %d functions on %d", nb, 20 );

	return true;
}



