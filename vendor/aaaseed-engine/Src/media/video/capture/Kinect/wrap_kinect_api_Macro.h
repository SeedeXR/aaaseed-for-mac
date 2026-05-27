
WRAP_LOADER(	NuiGetSensorCount,							HRESULT,	( int* pCount )																	)
WRAP_LOADER(	NuiCreateSensorByIndex,						HRESULT,	( int index, INuiSensor ** ppNuiSensor )										)
WRAP_LOADER(	NuiCreateSensorById,						HRESULT,	( const OLECHAR *strInstanceId, INuiSensor ** ppNuiSensor )						)
WRAP_LOADER(	NuiGetAudioSource,							HRESULT,	( INuiAudioBeam ** ppDmo )														)
WRAP_LOADER(	NuiSetDeviceStatusCallback,					void,		( NuiStatusProc callback, void* pUserData )										)

