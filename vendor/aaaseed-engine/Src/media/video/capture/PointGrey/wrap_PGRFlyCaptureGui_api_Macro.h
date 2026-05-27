
WRAP_LOADER(	pgrcamguiCreateContext,				CameraGUIError,	( CameraGUIContext* pcontext )																							)
WRAP_LOADER(	pgrcamguiShowCameraSelectionModal,	CameraGUIError,	( CameraGUIContext context, GenericCameraContext camcontext, unsigned long* pulSerialNumber, INT_PTR* pipDialogStatus )	)
WRAP_LOADER(	pgrcamguiToggleSettingsWindowState,	CameraGUIError,	( CameraGUIContext context, HWND hwndParent )																			)
WRAP_LOADER(	pgrcamguiInitializeSettingsDialog,	CameraGUIError,	( CameraGUIContext context, GenericCameraContext camcontext )															)
WRAP_LOADER(	pgrcamguiDestroyContext,			CameraGUIError,	( CameraGUIContext context )																							)
