
WRAP_LOADER(	TC_GetNumDevices,		TC_STATUS,	( unsigned int* numDevs )							)
WRAP_LOADER(	TC_OpenByIndex,			TC_STATUS,	( TC_HANDLE* handle, int index )					)
WRAP_LOADER(	TC_InitForceFrame,		TC_STATUS,	( TC_HANDLE handle, TCForceFrame* forceFrame )		)
WRAP_LOADER(	TC_InitContactFrame,	TC_STATUS,	( TC_HANDLE handle, TCContactFrame* contactFrame )	)
WRAP_LOADER(	TC_GetSensorInfo,		TC_STATUS,	( TC_HANDLE handle, TCSensorInfo* sensorInfo )		)
WRAP_LOADER(	TC_SetOption,			TC_STATUS,	( TC_HANDLE handle, long option, int value )		)
WRAP_LOADER(	TC_Close,				TC_STATUS,	( TC_HANDLE handle )								)
WRAP_LOADER(	TC_GetForceFrame,		TC_STATUS,	( TC_HANDLE handle, TCForceFrame* forceframe )		)
