
WRAP_LOADER(	FT_SetBitMode,		FT_STATUS,	( FT_HANDLE ftHandle, UCHAR ucMask, UCHAR ucEnable )									)
WRAP_LOADER(	FT_GetBitMode,		FT_STATUS,	( FT_HANDLE ftHandle, PUCHAR pucMode )													)
WRAP_LOADER(	FT_Write,			FT_STATUS,	( FT_HANDLE ftHandle, LPVOID lpBuffer, DWORD nBufferSize, LPDWORD lpBytesWritten )		)
WRAP_LOADER(	FT_ListDevices,		FT_STATUS,	( P_VOID pArg1, P_VOID pArg2, DWORD Flags )												)
WRAP_LOADER(	FT_OpenEx,			FT_STATUS,	( P_VOID pArg1, DWORD Flags, FT_HANDLE *pHandle )										)
WRAP_LOADER(	FT_ResetDevice,		FT_STATUS,	( FT_HANDLE ftHandle )																	)
WRAP_LOADER(	FT_SetBaudRate,		FT_STATUS,	( FT_HANDLE ftHandle, ULONG BaudRate )													)
WRAP_LOADER(	FT_Close,			FT_STATUS,	( FT_HANDLE ftHandle )																	)
