
WRAP_LOADER( GetLibVersion,			const char*,	(void) )
WRAP_LOADER( OpenDUO,				bool,			(DUOInstance *duo) )
WRAP_LOADER( CloseDUO,				bool,			(DUOInstance duo) )
WRAP_LOADER( EnumerateResolutions,	int,			(DUOResolutionInfo *resList, int32_t resListSize, int32_t width, int32_t height, int32_t binning, float fps) )
WRAP_LOADER( StartDUO,				bool,			(DUOInstance duo, DUOFrameCallback frameCallback, void *pUserData, bool masterMode) )
WRAP_LOADER( StopDUO,				bool,			(DUOInstance duo) )
WRAP_LOADER( __DUOParamSet__,		bool,			(DUOInstance duo, DUOParameter param, ...) )
WRAP_LOADER( __DUOParamGet__,		bool,			(DUOInstance duo, DUOParameter param, ...) )


