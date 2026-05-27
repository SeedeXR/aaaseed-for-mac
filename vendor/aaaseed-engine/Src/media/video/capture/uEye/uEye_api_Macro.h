//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\///
//###########################################################################//
//#                                                                         #//
//#     IDS - Imaging Development Systems GmbH, All rights reserved         #//
//#                                                                         #//
//###########################################################################//
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\///


// ---------------------------------------------------------------------------------
// set the following define in the project environment to a version
// number in order to include API functions of a specific driver version
//
// _UEYE_DYNAMIC_VERSION_       description
// -------------------------------------------------------------------------------
//          114                 includes only API functions of version 1.14.x
//          200                 includes additional API functions of version 2.00.x
//          300                 includes additional API functions of version 3.00.x
//          310                 includes additional API functions of version 3.10.x
//          320                 includes additional API functions of version 3.20.x
//          330                 includes additional API functions of version 3.30.x
//          332                 includes additional API functions of version 3.32.x
//          340                 includes additional API functions of version 3.40.x
//          360                 includes additional API functions of version 3.60.x
//          361                 includes additional API functions of version 3.61.x
//          370                 includes additional API functions of version 3.70.x
//          380                 includes additional API functions of version 3.80.x
//          382                 includes additional API functions of version 3.82.x
//          390                 includes additional API functions of version 3.90.x
//          400                 includes additional API functions of version 4.00.x
//          401                 includes additional API functions of version 4.01.x
//          402                 includes additional API functions of version 4.02.x
//          420                 includes additional API functions of version 4.20.x
//          430                 includes additional API functions of version 4.30.x
// e.g.: #define _UEYE_DYNAMIC_VERSION_  400
//
// ---------------------------------------------------------------------------------

#ifndef _UEYE_DYNAMIC_VERSION_
#pragma message("WARNING: define _UEYE_DYNAMIC_VERSION_ not explicit set! (see ueye_api_macro.h)")
#pragma message("         setting _UEYE_DYNAMIC_VERSION_ to default value 460")
#define _UEYE_DYNAMIC_VERSION_ 460
#endif

#if _UEYE_DYNAMIC_VERSION_ > 0 // Use API functions of version 1.14.x
// Add functions here, they will be wrapped automatically
// in the CuEyeDll class.

// ----------------------------------------------
// functions only effective on Falcon boards
// ----------------------------------------------
//  IDSEXP   is_SetVideoInput          (HIDS hf, INT Source);
UEYE_MACRO_DO(SetVideoInput)
DECLARE((HIDS hf, INT Source))

//  IDSEXP   is_SetHue                 (HIDS hf, INT Hue);
UEYE_MACRO_DO(SetHue)
DECLARE((HIDS hf, INT Hue))

//  IDSEXP   is_SetVideoMode           (HIDS hf, INT Mode);
UEYE_MACRO_DO(SetVideoMode)
DECLARE((HIDS hf, INT Mode))

//  IDSEXP   is_SetAGC                 (HIDS hf, INT Mode);
UEYE_MACRO_DO(SetAGC)
DECLARE((HIDS hf, INT Mode))

//  IDSEXP   is_SetSyncLevel           (HIDS hf, INT Level);
UEYE_MACRO_DO(SetSyncLevel)
DECLARE((HIDS hf, INT Level))

//  IDSEXP   is_ShowColorBars          (HIDS hf, INT Mode);
UEYE_MACRO_DO(ShowColorBars)
DECLARE((HIDS hf, INT Mode))

//  IDSEXP   is_SetScaler              (HIDS hf, float Scalex, float Scaley);
UEYE_MACRO_DO(SetScaler)
DECLARE((HIDS hf, float Scalex, float Scaley))

//  IDSEXP   is_SetCaptureMode         (HIDS hf, INT Mode);
UEYE_MACRO_DO(SetCaptureMode)
DECLARE((HIDS hf, INT Mode))

//  IDSEXP   is_SetHorFilter           (HIDS hf, INT Mode);
UEYE_MACRO_DO(SetHorFilter)
DECLARE((HIDS hf, INT Mode))

//  IDSEXP   is_SetVertFilter          (HIDS hf, INT Mode);
UEYE_MACRO_DO(SetVertFilter)
DECLARE((HIDS hf, INT Mode))

//  IDSEXP   is_EnableDDOverlay        (HIDS hf);
UEYE_MACRO_DO(EnableDDOverlay)
DECLARE((HIDS hf))

//  IDSEXP   is_DisableDDOverlay       (HIDS hf);
UEYE_MACRO_DO(DisableDDOverlay)
DECLARE((HIDS hf))

//  IDSEXP   is_ShowDDOverlay          (HIDS hf);
UEYE_MACRO_DO(ShowDDOverlay)
DECLARE((HIDS hf))

//  IDSEXP   is_HideDDOverlay          (HIDS hf);
UEYE_MACRO_DO(HideDDOverlay)
DECLARE((HIDS hf))

//  IDSEXP   is_ScaleDDOverlay         (HIDS hf, BOOL boScale);
UEYE_MACRO_DO(ScaleDDOverlay)
DECLARE((HIDS hf, BOOL boScale))

//  IDSEXP   is_SetDDUpdateTime        (HIDS hf, INT ms);
UEYE_MACRO_DO(SetDDUpdateTime)
DECLARE((HIDS hf, INT ms))

//  IDSEXP   is_GetCurrentField        (HIDS hf, int* pField);
UEYE_MACRO_DO(GetCurrentField)
DECLARE((HIDS hf, int* pField))

//  IDSEXP   is_SetVideoSize           (HIDS hf, INT xpos, INT ypos, INT xsize, INT ysize);
UEYE_MACRO_DO(SetVideoSize)
DECLARE((HIDS hf, INT xpos, INT ypos, INT xsize, INT ysize))

//  IDSEXP   is_SetKeyOffset           (HIDS hf, INT nOffsetX, INT nOffsetY);
UEYE_MACRO_DO(SetKeyOffset)
DECLARE((HIDS hf, INT offX, INT offY))

//  IDSEXP   is_PrepareStealVideo      (HIDS hf, int Mode, ULONG StealColorMode);
UEYE_MACRO_DO(PrepareStealVideo)
DECLARE((HIDS hf, int Mode, ULONG StealColorMode))

//  IDSEXP   is_SetParentHwnd          (HIDS hf, HWND hwnd);
UEYE_MACRO_DO(SetParentHwnd)
DECLARE((HIDS hf, HWND hwnd))

//  IDSEXP   is_SetUpdateMode          (HIDS hf, INT mode);
UEYE_MACRO_DO(SetUpdateMode)
DECLARE((HIDS hf, INT mode))

//  IDSEXP   is_OvlSurfaceOffWhileMove (HIDS hf, BOOL boMode);
UEYE_MACRO_DO(OvlSurfaceOffWhileMove)
DECLARE((HIDS hf, BOOL boMode))

//  IDSEXP   is_GetPciSlot             (HIDS hf, INT* pnSlot);
UEYE_MACRO_DO(GetPciSlot)
DECLARE((HIDS hf, INT* pnSlot))

//  IDSEXP   is_GetIRQ                 (HIDS hf, INT* pnIRQ);
UEYE_MACRO_DO(GetIRQ)
DECLARE((HIDS hf, INT* pnIRQ))

//  IDSEXP   is_SetToggleMode          (HIDS hf, int nInput1, int nInput2, int nInput3, int nInput4);
UEYE_MACRO_DO(SetToggleMode)
DECLARE((HIDS hf, int nInput1, int nInput2, int nInput3, int nInput4))

//  IDSEXP   is_SetDecimationMode      (HIDS hf, INT nMode, INT nDecimate);

//  IDSEXP   is_SetSync                (HIDS hf, INT nSync);
UEYE_MACRO_DO(SetSync)
DECLARE((HIDS hf, INT nSync))

// only FALCON duo/quattro
//  IDSEXP   is_SetVideoCrossbar       (HIDS hf, INT In, INT Out);
UEYE_MACRO_DO(SetVideoCrossbar)
DECLARE((HIDS hf, INT In, INT Out))

// watchdog functions
//  IDSEXP   is_WatchdogTime           (HIDS hf, long lTime);
//  IDSEXP   is_Watchdog               (HIDS hf, long lMode);

// video out functions
//  IDSEXP   is_SetPassthrough         (HIDS hf, INT Source);

// ----------------------------------------------
// alias functions for compatibility
// ----------------------------------------------
//  IDSEXP   is_InitBoard              (HIDS* phf, HWND hWnd);
UEYE_MACRO_DO(InitBoard)
DECLARE((HIDS* phf, HWND hWnd))

//  IDSEXP   is_ExitBoard              (HIDS hf);
UEYE_MACRO_DO(ExitBoard)
DECLARE((HIDS hf))

//  IDSEXP   is_InitFalcon             (HIDS* phf, HWND hWnd);
UEYE_MACRO_DO(InitFalcon)
DECLARE((HIDS* phf, HWND hWnd))

//  IDSEXP   is_ExitFalcon             (HIDS hf);
UEYE_MACRO_DO(ExitFalcon)
DECLARE((HIDS hf))

//  IDSEXP   is_GetBoardType           (HIDS hf);
UEYE_MACRO_DO(GetBoardType)
DECLARE((HIDS hf))

//  IDSEXP   is_GetBoardInfo           (HIDS hf, PBOARDINFO pInfo);
UEYE_MACRO_DO(GetBoardInfo)
DECLARE((HIDS hf, PBOARDINFO pInfo))

//  IDSEXPUL is_BoardStatus            (HIDS hf, INT nInfo, ULONG ulValue);
UEYE_MACRO_DO(BoardStatus)
DECLARE((HIDS hf, INT nInfo, ULONG ulValue))

//  IDSEXP   is_GetNumberOfDevices     ();
UEYE_MACRO_DO(GetNumberOfDevices)
DECLARE((void))

//  IDSEXP   is_GetNumberOfBoards      (INT* pnNumBoards);
UEYE_MACRO_DO(GetNumberOfBoards)
DECLARE((INT* pnNumBoards))

// ----------------------------------------------
// common function
// ----------------------------------------------
//  IDSEXP   is_StopLiveVideo          (HIDS hf, INT Wait);
UEYE_MACRO_DO(StopLiveVideo)
DECLARE((HIDS hf, INT Wait))

//  IDSEXP   is_FreezeVideo            (HIDS hf, INT Wait);
UEYE_MACRO_DO(FreezeVideo)
DECLARE((HIDS hf, INT Wait))

//  IDSEXP   is_CaptureVideo           (HIDS hf, INT Wait);
UEYE_MACRO_DO(CaptureVideo)
DECLARE((HIDS hf, INT Wait))

//  IDSEXP   is_IsVideoFinish          (HIDS hf, BOOL* pbo);
UEYE_MACRO_DO(IsVideoFinish)
DECLARE((HIDS hf, BOOL* pbo))

//  IDSEXP   is_HasVideoStarted        (HIDS hf, BOOL* pbo);
UEYE_MACRO_DO(HasVideoStarted)
DECLARE((HIDS hf,  BOOL* pbo))

//  IDSEXP   is_SetSaturation          (HIDS hf, INT ChromU, INT ChromV);
UEYE_MACRO_DO(SetSaturation)
DECLARE((HIDS hf, INT ChromU, INT ChromV))

//  IDSEXP   is_SetBrightness          (HIDS hf, INT Bright);
UEYE_MACRO_DO(SetBrightness)
DECLARE((HIDS hf, INT Bright))

//  IDSEXP   is_SetContrast            (HIDS hf, INT Cont);
UEYE_MACRO_DO(SetContrast)
DECLARE((HIDS hf, INT Cont))

//  IDSEXP   is_SetGamma               (HIDS hf, INT nGamma);
UEYE_MACRO_DO(SetGamma)
DECLARE((HIDS hf, INT Mode))

//  IDSEXP   is_AllocImageMem          (HIDS hf, INT width, INT height, INT bitspixel, char** ppcImgMem, int* pid);
UEYE_MACRO_DO(AllocImageMem)
DECLARE((HIDS hf, INT width, INT height, INT bitspixel, char** ppcImgMem, int* pid))

//  IDSEXP   is_SetImageMem            (HIDS hf, char* pcMem, int id);
UEYE_MACRO_DO(SetImageMem)
DECLARE((HIDS hf, char* pcMem, int id))

//  IDSEXP   is_FreeImageMem           (HIDS hf, char* pcMem, int id);
UEYE_MACRO_DO(FreeImageMem)
DECLARE((HIDS hf, char* pcMem, int id))

//  IDSEXP   is_GetImageMem            (HIDS hf, VOID** pMem);
UEYE_MACRO_DO(GetImageMem)
DECLARE((HIDS hf, VOID** pMem))

//  IDSEXP   is_GetActiveImageMem      (HIDS hf, char** ppcMem, int* pnID);
UEYE_MACRO_DO(GetActiveImageMem)
DECLARE((HIDS hf, char** pccMem, int* pnID ))

//  IDSEXP   is_InquireImageMem        (HIDS hf, char* pcMem, int nID, int* pnX, int* pnY, int* pnBits, int* pnPitch);
UEYE_MACRO_DO(InquireImageMem)
DECLARE((HIDS hf, char* pcMem, int nID, int* pnX, int* pnY, int* pnBits, int* pnPitch))

//  IDSEXP   is_GetImageMemPitch       (HIDS hf, INT* pPitch);
UEYE_MACRO_DO(GetImageMemPitch)
DECLARE((HIDS hf, INT* pPitch))

//  IDSEXP   is_SetAllocatedImageMem   (HIDS hf, INT width, INT height, INT bitspixel, char* pcImgMem, int* pid);
UEYE_MACRO_DO(SetAllocatedImageMem)
DECLARE((HIDS hf, INT width, INT height, INT bitspixel, char* pcImgMem, int* pid))

//  IDSEXP   is_SaveImageMem           (HIDS hf, const IS_CHAR* File, char* pcMem, int nID);
UEYE_MACRO_DO(SaveImageMem)
DECLARE((HIDS hf, const IS_CHAR* File, char* pcMem, int nID))

//  IDSEXP   is_CopyImageMem           (HIDS hf, char* pcSource, int nID, char* pcDest);
UEYE_MACRO_DO(CopyImageMem)
DECLARE((HIDS hf, char* pcSource, int nID, char* pcDest))

//  IDSEXP   is_CopyImageMemLines      (HIDS hf, char* pcSource, int nID, int nLines, char* pcDest);
UEYE_MACRO_DO(CopyImageMemLines)
DECLARE((HIDS hf, char* pcSource, int nID, int nLines, char* pcDest))

//  IDSEXP   is_AddToSequence          (HIDS hf, char* pcMem, INT nID);
UEYE_MACRO_DO(AddToSequence)
DECLARE((HIDS hf, char* pcImgMem, int nId))

//  IDSEXP   is_ClearSequence          (HIDS hf);
UEYE_MACRO_DO(ClearSequence)
DECLARE((HIDS hf))

//  IDSEXP   is_GetActSeqBuf           (HIDS hf, INT* pnNum, char** ppcMem, char** ppcMemLast);
UEYE_MACRO_DO(GetActSeqBuf)
DECLARE((HIDS hf, INT* pnNum, char** ppcMem, char** ppcMemLast))

//  IDSEXP   is_LockSeqBuf             (HIDS hf, INT nNum, char* pcMem);
UEYE_MACRO_DO(LockSeqBuf)
DECLARE((HIDS hf, INT nNum, char* pcMem))

//  IDSEXP   is_UnlockSeqBuf           (HIDS hf, INT nNum, char* pcMem);
UEYE_MACRO_DO(UnlockSeqBuf)
DECLARE((HIDS hf, INT nNum, char* pcMem))

//  IDSEXP   is_SetImageSize           (HIDS hf, INT x, INT y);
UEYE_MACRO_DO(SetImageSize)
DECLARE((HIDS hf, INT x, INT y))

//  IDSEXP   is_SetImagePos            (HIDS hf, INT x, INT y);
UEYE_MACRO_DO(SetImagePos)
DECLARE((HIDS hf, INT x, INT y))

//  IDSEXP   is_GetError               (HIDS hf, INT* pErr, IS_CHAR** ppcErr);
UEYE_MACRO_DO(GetError)
DECLARE((HIDS hf, INT* pErr, IS_CHAR** ppcErr))

//  IDSEXP   is_SetErrorReport         (HIDS hf, INT Mode);
UEYE_MACRO_DO(SetErrorReport)
DECLARE((HIDS hf, INT Mode))

//  IDSEXP   is_ReadEEPROM             (HIDS hf, INT Adr, char* pcString, INT Count);
UEYE_MACRO_DO(ReadEEPROM)
DECLARE((HIDS hf, INT Adr, char* pcString, INT Count))

//  IDSEXP   is_WriteEEPROM            (HIDS hf, INT Adr, char* pcString, INT Count);
UEYE_MACRO_DO(WriteEEPROM)
DECLARE((HIDS hf, INT Adr, char* pcString, INT Count))

//  IDSEXP   is_SaveImage              (HIDS hf, const IS_CHAR* File);
UEYE_MACRO_DO(SaveImage)
DECLARE((HIDS hf, const IS_CHAR* File))

//  IDSEXP   is_SetColorMode           (HIDS hf, INT Mode);
UEYE_MACRO_DO(SetColorMode)
DECLARE((HIDS hf, INT Mode))

//  IDSEXP   is_GetColorDepth          (HIDS hf, INT* pnCol, INT* pnColMode);
UEYE_MACRO_DO(GetColorDepth)
DECLARE((HIDS hf, INT* pnCol, INT* pnColMode))

// bitmap display function
//  IDSEXP   is_RenderBitmap       (HIDS hf, INT nMemID, HWND hwnd, INT nMode);
UEYE_MACRO_DO(RenderBitmap)
DECLARE((HIDS hf, INT nMemID, HWND hwnd, INT nMode))

//  IDSEXP   is_SetDisplayMode         (HIDS hf, INT Mode);
UEYE_MACRO_DO(SetDisplayMode)
DECLARE((HIDS hf, INT Mode))

//  IDSEXP   is_GetDC                  (HIDS hf, HDC* phDC);
UEYE_MACRO_DO(GetDC)
DECLARE((HIDS hf, HDC* phDC))

//  IDSEXP   is_ReleaseDC              (HIDS hf, HDC hDC);
UEYE_MACRO_DO(ReleaseDC)
DECLARE((HIDS hf, HDC hDC))

//  IDSEXP   is_UpdateDisplay          (HIDS hf);
UEYE_MACRO_DO(UpdateDisplay)
DECLARE((HIDS hf))

//  IDSEXP   is_SetDisplaySize         (HIDS hf, INT x, INT y);
UEYE_MACRO_DO(SetDisplaySize)
DECLARE((HIDS hf, INT x, INT y))

//  IDSEXP   is_SetDisplayPos          (HIDS hf, INT x, INT y);
UEYE_MACRO_DO(SetDisplayPos)
DECLARE((HIDS hf, INT x, INT y))

//  IDSEXP   is_LockDDOverlayMem       (HIDS hf, VOID** ppMem, INT* pPitch);
UEYE_MACRO_DO(LockDDOverlayMem)
DECLARE((HIDS hf, VOID** ppMem, INT* pPitch))

//  IDSEXP   is_UnlockDDOverlayMem     (HIDS hf);
UEYE_MACRO_DO(UnlockDDOverlayMem)
DECLARE((HIDS hf))

//  IDSEXP   is_LockDDMem              (HIDS hf, VOID** ppMem, INT* pPitch);
UEYE_MACRO_DO(LockDDMem)
DECLARE((HIDS hf, VOID** ppMem, INT* pPitch))

//  IDSEXP   is_UnlockDDMem            (HIDS hf);
UEYE_MACRO_DO(UnlockDDMem)
DECLARE((HIDS hf))

//  IDSEXP   is_GetDDOvlSurface        (HIDS hf, void** ppDDSurf);
UEYE_MACRO_DO(GetDDOvlSurface)
DECLARE((HIDS hf, void** ppDDSurf))

//  IDSEXP   is_SetKeyColor            (HIDS hf, INT r, INT g, INT b);
UEYE_MACRO_DO(SetKeyColor)
DECLARE((HIDS hf, INT r, INT g, INT b ))

//  IDSEXP   is_StealVideo             (HIDS hf, int Wait);
UEYE_MACRO_DO(StealVideo)
DECLARE((HIDS hf, int Wait))

//  IDSEXP   is_SetHwnd                (HIDS hf, HWND hwnd);
UEYE_MACRO_DO(SetHwnd)
DECLARE((HIDS hf, HWND hwnd))

//  IDSEXP   is_GetVsyncCount          (HIDS hf, long* pIntr, long* pActIntr);
UEYE_MACRO_DO(GetVsyncCount)
DECLARE((HIDS hf, long* pIntr, long* pActIntr))

//  IDSEXP   is_GetOsVersion           ();
UEYE_MACRO_DO(GetOsVersion)
DECLARE((void))
// version information
// IDSEXP is_GetDllVersion()
UEYE_MACRO_DO(GetDLLVersion)
DECLARE((void))

//  IDSEXP   is_InitEvent              (HIDS hf, HANDLE hEv, INT which);
UEYE_MACRO_DO(InitEvent)
DECLARE((HIDS hf, HANDLE hEv, INT which))

//  IDSEXP   is_ExitEvent              (HIDS hf, INT which);
UEYE_MACRO_DO(ExitEvent)
DECLARE((HIDS hf, INT which))

//  IDSEXP   is_EnableEvent            (HIDS hf, INT which);
UEYE_MACRO_DO(EnableEvent)
DECLARE((HIDS hf, INT which))

//  IDSEXP   is_DisableEvent           (HIDS hf, INT which);
UEYE_MACRO_DO(DisableEvent)
DECLARE((HIDS hf, INT which))

//  IDSEXP   is_SetIO                  (HIDS hf, INT nIO);
UEYE_MACRO_DO(SetIO)
DECLARE((HIDS hf, INT nIO))

//  IDSEXP   is_SetFlashStrobe         (HIDS hf, INT nField, INT nLine);
UEYE_MACRO_DO(SetFlashStrobe)
DECLARE((HIDS hf, INT nMode, INT nLine))

//  IDSEXP   is_SetExternalTrigger     (HIDS hf, INT nTriggerMode);
UEYE_MACRO_DO(SetExternalTrigger)
DECLARE((HIDS hf, INT nTriggerMode))

//  IDSEXP   is_SetTriggerCounter      (HIDS hf, INT nValue);

//  IDSEXP   is_SetRopEffect           (HIDS hf, INT effect, INT param, INT reserved);
UEYE_MACRO_DO(SetRopEffect)
DECLARE((HIDS hf, INT effect, INT param, INT reserved))

// IDSEXP is_InitCamera		(HIDS* phf, HWND hWnd)
UEYE_MACRO_DO(InitCamera)
DECLARE((HIDS* phf, HWND hWnd))

// IDSEXP is_ExitCamera		(HIDS* phf, HWND hWnd)
UEYE_MACRO_DO(ExitCamera)
DECLARE((HIDS hf))

// IDSEXP is_GetCameraInfo		(HIDS hf, PCAMINFO pInfo)
UEYE_MACRO_DO(GetCameraInfo)
DECLARE((HIDS hf, PBOARDINFO pInfo))

// IDSEXPUL is_CameraStatus		(HIDS hf, INT nInfo, ULONG ulValue)
UEYE_MACRO_DO(CameraStatus)
DECLARE((HIDS hf, INT nInfo, ULONG ulValue))

// IDSEXP is_GetCameraType		(HIDS hf)
UEYE_MACRO_DO(GetCameraType)
DECLARE((HIDS hf))

// IDSEXP is_GetNumberOfCameras		(INT* pnNumCams)
UEYE_MACRO_DO(GetNumberOfCameras)
DECLARE((INT* pnNumCams))

//Pixelclock
// is_GetPixelClockRange( HIDS hf, double *min, double *max, double *intervall );
UEYE_MACRO_DO(GetPixelClockRange)
DECLARE(( HIDS hf, INT *pnMIn, INT *pnMax ))

//  IDSEXP   is_SetPixelClock       (HIDS hf, INT nClockt);
UEYE_MACRO_DO(SetPixelClock)
DECLARE((HIDS hf, INT nClock))

// IDSEXP is_GetUsedBandwidth( HIDS hf );
UEYE_MACRO_DO(GetUsedBandwidth)
DECLARE(( HIDS hf ))

// Set/Get Framerate
// is_GetFrameTimeRange( HIDS hf, double *min, double *max, double *intervall );
UEYE_MACRO_DO(GetFrameTimeRange)
DECLARE(( HIDS hf, double *min, double *max, double *intervall ))

// IDSEXP is_SetFrameRate           ( HIDS hf, double FPS, double* newFPS )
UEYE_MACRO_DO(SetFrameRate)
DECLARE(( HIDS hf, double FPS, double* newFPS ))

// Set/Get Exposure
// is_GetExposureRange( HIDS hf, double *min, double *max, double *intervall );
UEYE_MACRO_DO(GetExposureRange)
DECLARE(( HIDS hf, double *min, double *max, double *intervall ))

// IDSEXP is_SetExposureTime           ( HIDS hf, double EXP, double* newEXP )
UEYE_MACRO_DO(SetExposureTime)
DECLARE(( HIDS hf, double EXP, double* newEXP ))

// get frames per second
// IDSEXP is_GetFramesPerSecond           ( HIDS hf, double *dblFPS )
UEYE_MACRO_DO(GetFramesPerSecond)
DECLARE(( HIDS hf, double *dblFPS ))

// is_SetIOMask ( only uEye USB )
//  IDSEXP   is_SetIOMask       (HIDS hf, INT nMask);
UEYE_MACRO_DO(SetIOMask)
DECLARE((HIDS hf, INT nMask))

// Get Sensorinfo
// IDSEXP is_GetSensorInfo           (HIDS hf, PSENSORINFO pInfo )
UEYE_MACRO_DO(GetSensorInfo)
DECLARE((HIDS hf, PSENSORINFO pInfo ))

// Get RevisionInfo
// IDSEXP is_GetRevisionInfo( HIDS hf, PREVISIONINFO prevInfo  )
UEYE_MACRO_DO(GetRevisionInfo)
DECLARE(( HIDS hf, PREVISIONINFO prevInfo ))

// enable/disable AutoExit after device remove
// IDSEXP is_EnableAutoExit           ( HIDS hf, INT nMode )
UEYE_MACRO_DO(EnableAutoExit)
DECLARE(( HIDS hf, INT nMode ))

// Message
// IDSEXP is_EnableMessage           (HIDS hf, INT which, HWND hWnd)
UEYE_MACRO_DO(EnableMessage)
DECLARE((HIDS hf, INT which, HWND hWnd))

// hardware gain settings
// IDSEXP is_SetHardwareGain           (HIDS hf, INT nMaster, INT nRed, INT nGreen, INT nBlue )
UEYE_MACRO_DO(SetHardwareGain)
DECLARE((HIDS hf, INT nMaster, INT nRed, INT nGreen, INT nBlue ))

// set render mode
// IDSEXP is_SetRenderMode           ( HIDS hf, INT Mode )
UEYE_MACRO_DO(SetRenderMode)
DECLARE(( HIDS hf, INT Mode ))

// enable/disable WhiteBalance
// IDSEXP is_SetWhiteBalance           ( HIDS hf, INT nMode )
UEYE_MACRO_DO(SetWhiteBalance)
DECLARE(( HIDS hf, INT nMode ))

// IDSEXP is_SetWhiteBalanceMultipliers           ( HIDS hf, double dblRed, double dblGreen, double dblBlue )
UEYE_MACRO_DO(SetWhiteBalanceMultipliers)
DECLARE(( HIDS hf, double dblRed, double dblGreen, double dblBlue ))

// IDSEXP is_GetWhiteBalanceMultipliers           ( HIDS hf, double *pdblRed, double *pdblGreen, double *pdblBlue )
UEYE_MACRO_DO(GetWhiteBalanceMultipliers)
DECLARE(( HIDS hf, double *pdblRed, double *pdblGreen, double *pdblBlue ))

// Edge enhancement
// IDSEXP is_SetEdgeEnhancement		( HIDS hf, INT nEnable )
UEYE_MACRO_DO(SetEdgeEnhancement)
DECLARE(( HIDS hf, INT nEnable ))

// Sensor features
// IDSEXP is_SetColorCorrection		( HIDS hf, INT nEnable, double *factors )
UEYE_MACRO_DO(SetColorCorrection)
DECLARE(( HIDS hf, INT nEnable, double *factors ))

// IDSEXP is_SetBlCompensation		( HIDS hf, INT nEnable, INT offset, INT reserved )
UEYE_MACRO_DO(SetBlCompensation)
DECLARE(( HIDS hf, INT nEnable, INT offset, INT reserved ))

// Hotpixel
// IDSEXP is_SetBadPixelCorrection( HIDS hf, INT nEnable, INT threshold )
UEYE_MACRO_DO(SetBadPixelCorrection)
DECLARE(( HIDS hf, INT nEnable, INT threshold ))

// IDSEXP is_LoadBadPixelCorrectionTable( HIDS hf, const IS_CHAR *File )
UEYE_MACRO_DO(LoadBadPixelCorrectionTable)
DECLARE(( HIDS hf, const IS_CHAR *File ))

// IDSEXP is_SaveBadPixelCorrectionTable( HIDS hf, const IS_CHAR *File )
UEYE_MACRO_DO(SaveBadPixelCorrectionTable)
DECLARE(( HIDS hf, const IS_CHAR *File ))

// IDSEXP is_SetBadPixelCorrectionTable( HIDS hf, INT nMode, WORD *pList )
UEYE_MACRO_DO(SetBadPixelCorrectionTable)
DECLARE(( HIDS hf, INT nMode, WORD *pList ))

// Memoryboard
// is_SetMemoryMode( HIDS hf, INT nCount, INT nDelay );
UEYE_MACRO_DO(SetMemoryMode)
DECLARE((HIDS hf, INT nCount, INT nDelay ))

// is_TransferImage(  HIDS hf, INT nMemID, INT seqID, INT imageNr, INT reserved );
UEYE_MACRO_DO(TransferImage)
DECLARE((  HIDS hf, INT nMemID, INT seqID, INT imageNr, INT reserved ))

// is_TransferMemorySequence( HIDS hf, INT seqID, INT StartNr, INT nCount, INT nSeqPos );
UEYE_MACRO_DO(TransferMemorySequence)
DECLARE(( HIDS hf, INT seqID, INT StartNr, INT nCount, INT nSeqPos ))

// is_MemoryFreezeVideo( (HIDS hf, INT nMemID, INT Wait );
UEYE_MACRO_DO(MemoryFreezeVideo)
DECLARE(( HIDS hf, INT nMemID, INT Wait ))

// is_GetLastMemorySequence( HIDS hf, INT *pID );
UEYE_MACRO_DO(GetLastMemorySequence)
DECLARE(( HIDS hf, INT *pID ))

// is_GetNumberOfMemoryImages( HIDS hf, INT pID, INT *pnCount );
UEYE_MACRO_DO(GetNumberOfMemoryImages)
DECLARE(( HIDS hf, INT pID, INT *pnCount ))

// is_GetMemorySequenceWindow( HIDS hf, INT nID, INT *left, INT *top, INT *right, INT *bottom );
UEYE_MACRO_DO(GetMemorySequenceWindow)
DECLARE(( HIDS hf, INT nID, INT *left, INT *top, INT *right, INT *bottom ))

// is_IsMemoryBoardConnected( HIDS hf, BOOL* pConnected  );
UEYE_MACRO_DO(IsMemoryBoardConnected)
DECLARE(( HIDS hf, BOOL* pConnected  ))

// is_ResetMemory( HCAM hf );
UEYE_MACRO_DO(ResetMemory)
DECLARE(( HIDS hf, INT nReserved ))

// is_SetSubSampling( HIDS hf, INT mode );
UEYE_MACRO_DO(SetSubSampling)
DECLARE(( HIDS hf, INT mode ))

// is_ForceTrigger( HIDS hf );
UEYE_MACRO_DO(ForceTrigger)
DECLARE(( HIDS hf ))

// new with driver version 1.12.0006
// is_GetBusSpeed( HIDS hf )
UEYE_MACRO_DO(GetBusSpeed)
DECLARE(( HIDS hf  ))

// new with driver version 1.12.0015
// is_SetBinning( HIDS hf, INT mode );
UEYE_MACRO_DO(SetBinning)
DECLARE(( HIDS hf, INT mode ))

// new with driver version 1.12.0017
// is_ResetToDefault( HIDS hf );
UEYE_MACRO_DO(ResetToDefault)
DECLARE(( HIDS hf ))

// is_LoadParameters( HIDS hf, const IS_CHAR* pFilename );
UEYE_MACRO_DO(LoadParameters)
DECLARE(( HIDS hf, const IS_CHAR* pFilename ))

// is_SaveParameters( HIDS hf, const IS_CHAR* pFilename );
UEYE_MACRO_DO(SaveParameters)
DECLARE(( HIDS hf, const IS_CHAR* pFilename ))

// new with driver version 1.14.0001
// is_GetGlobalFlashDelays( HIDS hf, ULONG *pulDelay, ULONG *pulDuration );
UEYE_MACRO_DO(GetGlobalFlashDelays)
DECLARE(( HIDS hf, ULONG *pulDelay, ULONG *pulDuration ))

// is_SetFlashDelay( HIDS hf, ULONG ulDelay, ULONG ulDuration );
UEYE_MACRO_DO(SetFlashDelay)
DECLARE(( HIDS hf, ULONG ulDelay, ULONG ulDuration  ))

// new with driver version 1.14.0002
//  IDSEXP   is_LoadImage              (HIDS hf, const IS_CHAR* File);
UEYE_MACRO_DO(LoadImage)
DECLARE((HIDS hf, const IS_CHAR* File))

// new with driver version 1.14.0008
// is_SetImageAOI( HIDS hf, INT xPos, INT yPos, INT width, INT height );
UEYE_MACRO_DO(SetImageAOI)
DECLARE(( HIDS hf, INT xPos, INT yPos, INT width, INT height  ))

// is_SetCameraID( HCAM hf, INT nID );
UEYE_MACRO_DO(SetCameraID)
DECLARE(( HCAM hf, INT nID  ))

// is_SetBayerConversion( HCAM hf, INT nMode );
UEYE_MACRO_DO(SetBayerConversion)
DECLARE(( HCAM hf, INT nMode  ))

// is_SetTestImage( HCAM hf, INT nMode );
UEYE_MACRO_DO(SetTestImage)
DECLARE(( HCAM hf, INT nMode  ))

// new with driver version 1.14.0009
// is_SetHardwareGamma( HCAM hf, INT nMode );
UEYE_MACRO_DO(SetHardwareGamma)
DECLARE(( HCAM hf, INT nMode  ))

#endif

#if _UEYE_DYNAMIC_VERSION_ > 114 // Use API functions of version 2.00.x

// new with driver version 2.00.0001
// is_GetCameraList( PUEYE_CAMERA_LIST pucl );
UEYE_MACRO_DO(GetCameraList)
DECLARE(( PUEYE_CAMERA_LIST pucl  ))

// new with driver version 2.00.0011
// is_SetAOI (HIDS hf, INT type, INT *pXPos, INT *pYPos, INT *pWidth, INT *pHeight);
UEYE_MACRO_DO(SetAOI)
DECLARE(( HIDS hf, INT type, INT *pXPos, INT *pYPos, INT *pWidth, INT *pHeight ))

// is_SetAutoParameter          (HIDS hf, INT param, double *pval1, double *pval2);
UEYE_MACRO_DO(SetAutoParameter)
DECLARE(( HIDS hf, INT param, double *pval1, double *pval2 ))

// IDSEXP is_GetAutoInfo (HIDS hf, UEYE_AUTO_INFO *pInfo);
UEYE_MACRO_DO(GetAutoInfo)
DECLARE(( HIDS hf, UEYE_AUTO_INFO *pInfo ))

// IDSEXP is_ConvertImage               (HIDS hf, char* pcSource, INT nIDSource, char** pcDest, INT *nIDDest, INT * reserve);
UEYE_MACRO_DO(ConvertImage)
DECLARE((HIDS hf, char* pcSource, INT nIDSource, char** pcDest, INT *nIDDest, INT * reserve))

// IDSEXP is_SetConvertParam            (HIDS hf, BOOL ColorCorrection, INT BayerConversionMode, INT ColorMode, BOOL Gamma, BOOL WhiteBalance)
UEYE_MACRO_DO(SetConvertParam)
DECLARE((HIDS hf, BOOL ColorCorrection, INT BayerConversionMode, INT ColorMode, INT Gamma, double *WhiteBalanceMultipliers))

// IDSEXP is_SaveImageEx                (HIDS hf, const IS_CHAR* File, INT fileFormat, INT Param);
UEYE_MACRO_DO(SaveImageEx)
DECLARE((HIDS hf, const IS_CHAR* File, INT fileFormat, INT Param))

// IDSEXP is_SaveImageMemEx             (HIDS hf, const IS_CHAR* File, char* pcMem, INT nID, INT FileFormat, INT Param);
UEYE_MACRO_DO(SaveImageMemEx)
DECLARE((HIDS hf, const IS_CHAR* File, char* pcMem, INT nID, INT FileFormat, INT Param))

// IDSEXP is_LoadImageMem               (HIDS hf, const IS_CHAR* File, char** ppcImgMem, INT* pid);
UEYE_MACRO_DO(LoadImageMem)
DECLARE((HIDS hf, const IS_CHAR* File, char** ppcImgMem, INT* pid))

// IDSEXP SetTriggerDelay               (HIDS hf, INT nTriggerDelay );
UEYE_MACRO_DO(SetTriggerDelay)
DECLARE((HIDS hf, INT nTriggerDelay ))

// IDSEXP is_SetGainBoost				(HIDS hf, INT mode);
UEYE_MACRO_DO(SetGainBoost)
DECLARE((HIDS hf, INT mode ))

// IDSEXP is_SetLED                     ( HIDS hf, INT nValue );
UEYE_MACRO_DO(SetLED)
DECLARE(( HIDS hf, INT nValue  ))

// IDSEXP is_SetGlobalShutter           ( HIDS hf, INT mode );
UEYE_MACRO_DO(SetGlobalShutter)
DECLARE(( HIDS hf, INT mode  ))

// IDSEXP is_SetExtendedRegister        ( HIDS hf, INT index,WORD value);
UEYE_MACRO_DO(SetExtendedRegister)
DECLARE(( HIDS hf, INT index,WORD value))

// IDSEXP is_SetHWGainFactor            (HIDS hf, INT nMode, INT nFactor);
UEYE_MACRO_DO(SetHWGainFactor)
DECLARE((HIDS hf, INT nMode, INT nFactor))

// camera renumeration
// IDSEXP is_Renumerate                 ( HIDS hf, INT nMode )
UEYE_MACRO_DO(Renumerate)
DECLARE(( HIDS hf, INT nMode ))

//IDSEXP is_GetExtendedRegister(HIDS hf, INT index, WORD *pwValue)
UEYE_MACRO_DO(GetExtendedRegister)
DECLARE((HIDS hf, INT index, WORD *pwValue))

// Read / Write I2C
// IDSEXP is_WriteI2C               ( HIDS hf, INT nDeviceAddr, INT nRegisterAddr, BYTE* pbData, INT nLen )
UEYE_MACRO_DO(WriteI2C)
DECLARE((HIDS hf, INT nDeviceAddr, INT nRegisterAddr, BYTE* pbData, INT nLen ))

// IDSEXP is_ReadI2C                ( HIDS hf, INT nDeviceAddr, INT nRegisterAddr, BYTE* pbData, INT nLen )
UEYE_MACRO_DO(ReadI2C)
DECLARE((HIDS hf, INT nDeviceAddr, INT nRegisterAddr, BYTE* pbData, INT nLen ))

// IDSEXP is_GetImageHistogram		(HIDS hf, int nID, INT ColorMode, DWORD* pHistoMem)
UEYE_MACRO_DO(GetImageHistogram)
DECLARE((HIDS hf, int nID, INT ColorMode, DWORD* pHistoMem))

//  IDSEXP is_WaitForNextImage      ( HIDS hf, UINT timeout, char **ppcMem, INT *imageID )
UEYE_MACRO_DO(WaitForNextImage)
DECLARE(( HIDS hf, UINT timeout, char **ppcMem, INT *imageID ))

//  IDSEXP is_InitImageQueue        ( HIDS hf, INT nMode )
UEYE_MACRO_DO(InitImageQueue)
DECLARE(( HIDS hf, INT nMode ))

//  IDSEXP is_ExitImageQueue        ( HIDS hf )
UEYE_MACRO_DO(ExitImageQueue)
DECLARE(( HIDS hf ))

#endif

#if _UEYE_DYNAMIC_VERSION_ > 200 // Use API functions of version 3.00.x
/* New with driver version 3.00
 * is_GetEthDeviceInfo()
 * is_SetAutoCfgIpSetup()
 * is_SetPacketFilter()
 * is_SetPersistentIpCfg()
 * is_SetStarterFirmware()
 */
//	IDSEXP is_GetEthDeviceInfo	( HIDS hf, UEYE_ETH_DEVICE_INFO* pDeviceInfo, UINT uStructSize);
UEYE_MACRO_DO(GetEthDeviceInfo)
DECLARE((HIDS hf, UEYE_ETH_DEVICE_INFO* pDeviceInfo, UINT uStructSize))

//	IDSEXP is_SetPersistentIpCfg	( HIDS hf, UEYE_ETH_IP_CONFIGURATION* pIpCfg, UINT uStructSize);
UEYE_MACRO_DO(SetPersistentIpCfg)
DECLARE((HIDS hf, UEYE_ETH_IP_CONFIGURATION* pIpCfg, UINT uStructSize))

//	IDSEXP is_SetStarterFirmware	( HIDS hf, const CHAR* pcFilepath, UINT uFilepathLen);
UEYE_MACRO_DO(SetStarterFirmware)
DECLARE((HIDS hf, const CHAR* pcFilepath, UINT uFilepathLen))

//	IDSEXP is_SetAutoCfgIpSetup	( INT iAdapterID, const UEYE_ETH_AUTOCFG_IP_SETUP* pSetup, UINT uStructSize);
UEYE_MACRO_DO(SetAutoCfgIpSetup)
DECLARE((INT iAdapterID, const UEYE_ETH_AUTOCFG_IP_SETUP* pSetup, UINT uStructSize))

//	IDSEXP is_SetPacketFilter		( INT iAdapterID, UINT uFilterSetting);
UEYE_MACRO_DO(SetPacketFilter)
DECLARE((INT iAdapterID, UINT uFilterSetting))

#endif 

#if _UEYE_DYNAMIC_VERSION_ > 300 // Use API functions of version 3.10.x
/* New with driver version 3.10
 * is_EnableHdr()
 * is_GetHdrKneepointInfo()
 * is_GetHdrKneepoints()
 * is_GetHdrMode()
 * is_SetHdrKneepoints()
 */
 
 // IDSEXP is_GetHdrMode                 ( HIDS hf, INT *Mode )
UEYE_MACRO_DO(GetHdrMode)
DECLARE(( HIDS hf, INT *Mode ))

// IDSEXP is_EnableHdr( HIDS hf, INT Enable )
UEYE_MACRO_DO(EnableHdr)
DECLARE(( HIDS hf, INT Enable ))

// IDSEXP is_SetHdrKneepoints (HIDS hf, KNEEPOINTARRAY *KneepointArray, INT KneepointArraySize)
UEYE_MACRO_DO(SetHdrKneepoints)
DECLARE(( HIDS hf, KNEEPOINTARRAY *KneepointArray, INT KneepointArraySize ))

// IDSEXP is_GetHdrKneepoints (HIDS hf, KNEEPOINTARRAY *KneepointArray, INT KneepointArraySize)
UEYE_MACRO_DO(GetHdrKneepoints)
DECLARE(( HIDS hf, KNEEPOINTARRAY *KneepointArray, INT KneepointArraySize ))

// IDSEXP is_GetHdrKneepointInfo (HIDS hf, KNEEPOINTINFO *KneepointInfo, INT KneepointInfoSize)
UEYE_MACRO_DO(GetHdrKneepointInfo)
DECLARE(( HIDS hf, KNEEPOINTINFO *KneepointInfo, INT KneepointInfoSize ))
#endif

#if _UEYE_DYNAMIC_VERSION_ > 310 // Use API functions of version 3.20.x
/* New with driver version 3.20
 * is_SetOptimalCameraTiming()
 */
 // IDSEXP is_SetOptimalCameraTiming (HIDS hf, INT Mode, INT Timeout, INT *pMaxPxlClk, double *pMaxFrameRate)
UEYE_MACRO_DO(SetOptimalCameraTiming)
DECLARE(( HIDS hf, INT Mode, INT Timeout, INT *pMaxPxlClk, double *pMaxFrameRate))

#endif

#if _UEYE_DYNAMIC_VERSION_ > 320 // Use API functions of version 3.30.x
/* New with driver version 3.30.x
 * is_SetSensorTestImage()
 * is_GetSupportedTestImages() 
 * is_GetTestImageValueRange() 
 * is_SetCameraLUT()
 * is_GetCameraLUT()
 * is_SetColorConverter()
 * is_GetColorConverter()
 * is_GetComportNumber()
 * is_GetCaptureErrorInfo()
 * is_ResetCaptureErrorInfo()
 */

//	IDSEXP is_GetComportNumber	(HIDS hf, UINT *ComportNumber);
UEYE_MACRO_DO(GetComportNumber)
DECLARE((HIDS hf, UINT *ComportNumber))

// IDSEXP is_SetColorConverter      (HIDS hf, INT ColorMode, INT ConvertMode)
UEYE_MACRO_DO(SetColorConverter)
DECLARE((HIDS hf, INT ColorMode, INT ConvertMode))

// IDSEXP is_GetColorConverter      (HIDS hf, INT ColorMode, INT *pCurrentConvertMode, INT *pDefaultConvertMode, INT *pSupportedConvertModes)
UEYE_MACRO_DO(GetColorConverter)
DECLARE((HIDS hf, INT ColorMode, INT *pCurrentConvertMode, INT *pDefaultConvertMode, INT *pSupportedConvertModes))

// IDSEXP is_SetCameraLUT           (HIDS hf, UINT Mode, UINT NumberOfEntries, double *pRed_Grey, double *pGreen, double *pBlue)
UEYE_MACRO_DO(SetCameraLUT)
DECLARE((HIDS hf, UINT Mode, UINT NumberOfEntries, double *pRed_Grey, double *pGreen, double *pBlue))

// IDSEXP is_GetCameraLUT           (HIDS hf, UINT Mode, UINT NumberOfEntries, double *pRed_Grey, double *pGreen, double *pBlue)
UEYE_MACRO_DO(GetCameraLUT)
DECLARE((HIDS hf, UINT Mode, UINT NumberOfEntries, double *pRed_Grey, double *pGreen, double *pBlue))

//	IDSEXP is_GetSupportedTestImages (HIDS hf, INT *SupportedTestImages);
UEYE_MACRO_DO(GetSupportedTestImages)
DECLARE((HIDS hf, INT *SupportedTestImages))

//	IDSEXP is_GetTestImageValueRange (HIDS hf, INT TestImage, INT *TestImageValueMin, INT *TestImageValueMax);
UEYE_MACRO_DO(GetTestImageValueRange)
DECLARE((HIDS hf, INT TestImage, INT *TestImageValueMin, INT *TestImageValueMax))

//	IDSEXP is_SetSensorTestImage (HIDS hf, INT Param1, INT Param2);
UEYE_MACRO_DO(SetSensorTestImage)
DECLARE((HIDS hf, INT Param1, INT Param2))

#endif

#if _UEYE_DYNAMIC_VERSION_ > 330 // Use API functions of version 3.32.x
/* New with driver version 3.32.x
 * is_SetTimeout()
 * is_GetTimeout()
 */
 
// IDSEXP is_SetTimeout             (HIDS hf, UINT nMode, UINT Timeout)
UEYE_MACRO_DO(SetTimeout)
DECLARE(( HIDS hf, UINT nMode, UINT Timeout ))

// IDSEXP is_GetTimeout             (HIDS hf, UINT nMode, UINT *pTimeout)
UEYE_MACRO_DO(GetTimeout)
DECLARE(( HIDS hf, UINT nMode, UINT *pTimeout ))

#endif

#if _UEYE_DYNAMIC_VERSION_ > 332 // Use API functions of version 3.40.x
/* New with driver version 3.40.x
 * is_GetImageInfo()
 * is_GetDuration()
 * is_GetSensorScalerInfo()
 * is_SetSensorScaler()
 * is_DirectRenderer()
 */
 
 // IDSEXP is_GetImageInfo           (HIDS hf, INT nID, UEYEIMAGEINFO *pImageInfo, INT ImageInfoSize)
UEYE_MACRO_DO(GetImageInfo)
DECLARE(( HIDS hf, INT nID, UEYEIMAGEINFO *pImageInfo, INT ImageInfoSize ))

// IDSEXP is_GetDuration            (HIDS hf, UINT nMode, INT* pnTime)
UEYE_MACRO_DO(GetDuration)
DECLARE(( HIDS hf, UINT nMode, INT *pnTime ))

// IDSEXP is_GetSensorScalerInfo   (HIDS hf, SENSORSCALERINFO *pSensorScalerInfo, INT nSensorScalerInfoSize)
UEYE_MACRO_DO(GetSensorScalerInfo)
DECLARE(( HIDS hf, SENSORSCALERINFO *pSensorScalerInfo, INT nSensorScalerInfoSize ))

// IDSEXP is_SetSensorScaler        (HIDS hf, UINT nMode, double dblFactor) 
UEYE_MACRO_DO(SetSensorScaler)
DECLARE(( HIDS hf, UINT nMode, double dblFactor ))

// IDSEXP is_DirectRenderer         (HIDS hf, UINT nMode, void *pParam, UINT SizeOfParam)
UEYE_MACRO_DO(DirectRenderer)
DECLARE(( HIDS hf, UINT nMode, void *pParam, UINT SizeOfParam ))

#endif

#if _UEYE_DYNAMIC_VERSION_ > 340 // Use API functions of version 3.60.x
/* New with driver version 3.52.x
 * is_ImageFormat()
 * is_FaceDetection()
 * is_Focus()
 * is_ImageStabilization()
 */
 
 // IDSEXP is_ImageFormat            (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(ImageFormat)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

// IDSEXP is_FaceDetection          (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(FaceDetection)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

// IDSEXP is_Focus                  (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(Focus)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

// IDSEXP is_ImageStabilization     (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(ImageStabilization)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

#endif

#if _UEYE_DYNAMIC_VERSION_ > 360 // Use API functions of version 3.61.x
/* New with driver version 3.61.x
 * is_ScenePreset()
 * is_Zoom()
 * is_Sharpness()
 * is_Saturation()
 */
 
 // IDSEXP is_ScenePreset            (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(ScenePreset)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

// IDSEXP is_Zoom                   (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(Zoom)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

// IDSEXP is_Sharpness              (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(Sharpness)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

// IDSEXP is_Saturation             (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(Saturation)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))
#endif
 
#if _UEYE_DYNAMIC_VERSION_ > 361 // Use API functions of version 3.70.x
/* New with driver version 3.70.x
 * is_TriggerDebounce()
 * is_ColorTemperature()
 */

// IDSEXP is_TriggerDebounce        (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(TriggerDebounce)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

// IDSEXP is_ColorTemperature       (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(ColorTemperature)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

#endif

#if _UEYE_DYNAMIC_VERSION_ > 370 // Use API functions of version 3.80.x
/* New with driver version 3.80.x
 * is_Hotpixel()
 * is_AOI()
 * is_Transfer()
 * is_IpConfig()
 * is_DeviceFeature()
 */
 
// IDSEXP is_HotPixel               (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(HotPixel)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

// IDSEXP is_AOI                    (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(AOI)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

// IDSEXP is_Transfer               (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(Transfer)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

// IDSEXP is_IpConfig               (INT iID, UEYE_ETH_ADDR_MAC mac, UINT nCommand, void* pParam, UINT cbSizeOfParam)
UEYE_MACRO_DO(IpConfig)
DECLARE(( INT iID, UEYE_ETH_ADDR_MAC mac, UINT nCommand, void* pParam, UINT cbSizeOfParam ))

// IDSEXP is_DeviceFeature          (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(DeviceFeature)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

#endif

#if _UEYE_DYNAMIC_VERSION_ > 380 // Use API functions of version 3.82.x
/* New with driver version 3.82.x
 * is_Exposure()
 * is_Configuration()
 * is_Trigger()
 */
 
// IDSEXP is_Exposure               (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(Exposure)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

// IDSEXP is_Configuration          (UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(Configuration)
DECLARE(( UINT nCommand, void *pParam, UINT nSizeOfParam ))

// IDSEXP is_Trigger                (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(Trigger)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

#endif

#if _UEYE_DYNAMIC_VERSION_ > 382 // Use API functions of version 3.90.x
/* New with driver version 3.90.x
 * is_IO()
 * is_CaptureStatus()
 * is_BootBoost()
 */
 
// IDSEXP is_IO                     (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(IO)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

// IDSEXP is_CaptureStatus          (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(CaptureStatus)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

// IDSEXP is_BootBoost              (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(BootBoost)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

#endif

#if _UEYE_DYNAMIC_VERSION_ > 390 // Use API functions of version 4.00.x
/* New with driver version 4.00.x
 * is_PixelClock()
 * is_DeviceInfo()
 * is_ImageFile()
 * is_ParameterSet()
 * is_EdgeEnhancement()
 * is_AutoParameter()
 * is_Convert()
 */

// IDSEXP is_PixelClock             (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(PixelClock)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

// IDSEXP is_DeviceInfo             (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(DeviceInfo)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

// IDSEXP is_ImageFile              (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(ImageFile)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

// IDSEXP is_ParameterSet           (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(ParameterSet)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

// IDSEXP is_EdgeEnhancement        (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(EdgeEnhancement)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

// IDSEXP is_AutoParameter          (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(AutoParameter)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

// IDSEXP is_Convert                (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(Convert)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

#endif

#if _UEYE_DYNAMIC_VERSION_ > 400 // Use API functions of version 4.01.x
/* New with driver version 4.01.x
 * is_Blacklevel()
 */
 
// IDSEXP is_Blacklevel             (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(Blacklevel)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

#endif

#if _UEYE_DYNAMIC_VERSION_ > 401 // Use API functions of version 4.02.x
/* New with driver version 4.02.x
 * 
 */

#endif

#if _UEYE_DYNAMIC_VERSION_ > 402 // Use API functions of version 4.20.x
/* New with driver version 4.20.x
 * 
 */

// IDSEXP is_Measure				(HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(Measure)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

// IDSEXP is_ImageBuffer			(HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(ImageBuffer)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

#endif

#if _UEYE_DYNAMIC_VERSION_ > 422 // Use API functions of version 4.30.x
/* New with driver version 4.30.x
 * 
 */

// IDSEXP is_OptimalCameraTiming    (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(OptimalCameraTiming)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

#endif


#if _UEYE_DYNAMIC_VERSION_ > 431 // Use API functions of version 4.40.x
/* New with driver version 4.40.x
 * 
 */

// IDSEXP is_LUT    (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(LUT)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))


// IDSEXP is_Gamma  (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(Gamma)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))


// IDSEXP is_Memory  (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(Memory)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

#endif


#if _UEYE_DYNAMIC_VERSION_ > 441 // Use API functions of version 4.50.x
/* New with driver version 4.50.x
 * 
 */

// IDSEXP is_Multicast (HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam)
UEYE_MACRO_DO(Multicast)
DECLARE(( HIDS hf, UINT nCommand, void *pParam, UINT nSizeOfParam ))

#endif
