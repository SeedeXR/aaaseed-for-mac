
WRAP_LOADER(	sixenseInit,							int,	( void )																	)
WRAP_LOADER(	sixenseExit,							int,	( void )																	)

WRAP_LOADER(	sixenseGetMaxBases,						int,	( void )																	)
WRAP_LOADER(	sixenseSetActiveBase,					int,	( int i )																	)
WRAP_LOADER(	sixenseIsBaseConnected,					int,	( int i )																	)

WRAP_LOADER(	sixenseGetMaxControllers,				int,	( void )																	)
WRAP_LOADER(	sixenseIsControllerEnabled,				int,	( int which )																)
WRAP_LOADER(	sixenseGetNumActiveControllers,			int,	( void )																	)

WRAP_LOADER(	sixenseGetHistorySize,					int,	( void )																	)

WRAP_LOADER(	sixenseGetData,							int,	( int which, int index_back, sixenseControllerData * )						)
WRAP_LOADER(	sixenseGetAllData,						int,	( int index_back, sixenseAllControllerData * )								)
WRAP_LOADER(	sixenseGetNewestData,					int,	( int which, sixenseControllerData * )										)
WRAP_LOADER(	sixenseGetAllNewestData,				int,	( sixenseAllControllerData * )												)

WRAP_LOADER(	sixenseSetHemisphereTrackingMode,		int,	( int which_controller, int state )											)
WRAP_LOADER(	sixenseGetHemisphereTrackingMode,		int,	( int which_controller, int *state )										)

WRAP_LOADER(	sixenseAutoEnableHemisphereTracking,	int,	( int which_controller )													)

WRAP_LOADER(	sixenseSetHighPriorityBindingEnabled,	int,	( int on_or_off )															)
WRAP_LOADER(	sixenseGetHighPriorityBindingEnabled,	int,	( int *on_or_off )															)

WRAP_LOADER(	sixenseTriggerVibration,				int,	( int controller_id, int duration_100ms, int pattern_id )					)

WRAP_LOADER(	sixenseSetFilterEnabled,				int,	( int on_or_off )															)
WRAP_LOADER(	sixenseGetFilterEnabled,				int,	( int *on_or_off )															)

WRAP_LOADER(	sixenseSetFilterParams,					int,	( float near_range, float near_val, float far_range, float far_val )		)
WRAP_LOADER(	sixenseGetFilterParams,					int,	( float *near_range, float *near_val, float *far_range, float *far_val )	)

WRAP_LOADER(	sixenseSetBaseColor,					int,	( unsigned char red, unsigned char green, unsigned char blue )				)
WRAP_LOADER(	sixenseGetBaseColor,					int,	( unsigned char *red, unsigned char *green, unsigned char *blue )			)

WRAP_LOADER(	sixenseSetCoilOffset,					int,	( float yaw, float pitch, float roll )										)
