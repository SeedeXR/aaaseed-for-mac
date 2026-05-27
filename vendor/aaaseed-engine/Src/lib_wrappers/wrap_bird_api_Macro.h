
WRAP_LOADER(	birdFrameReady,			BOOL,	( int nGroupID )																								)
WRAP_LOADER(	birdStopFrameStream,	BOOL,	( int nGroupID )																								)
WRAP_LOADER(	birdShutDown,			int,	( int nGroupID )																								)
WRAP_LOADER(	birdISAWakeUp,			BOOL,	( int nGroupID, BOOL bStandAlone, int nNumDevices, WORD *pwAddress, DWORD dwReadTimeout, DWORD dwWriteTimeout )	)
WRAP_LOADER(	birdGetSystemConfig,	BOOL,	( int nGroupID, BIRDSYSTEMCONFIG *psyscfg )																		)
WRAP_LOADER(	birdGetDeviceConfig,	BOOL,	( int nGroupID, int nDeviceNum, BIRDDEVICECONFIG *pdevcfg )														)
WRAP_LOADER(	birdStartFrameStream,	BOOL,	( int nGroupID )																								)
WRAP_LOADER(	birdGetFrame,			BOOL,	( int nGroupID, BIRDFRAME *pframe )																				)
