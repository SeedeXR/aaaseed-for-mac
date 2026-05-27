//=============================================================================
// Copyright © 2004 Point Grey Research, Inc. All Rights Reserved.
// 
// This software is the confidential and proprietary information of Point
// Grey Research, Inc. ("Confidential Information").  You shall not
// disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with Point Grey Research Inc.
// 
// PGR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//
// Digiclops® is a registered trademark of Point Grey Research Inc.
//=============================================================================
//=============================================================================
// $Id: digiclops.h,v 1.48 2004/02/13 21:36:32 mwhite Exp $
//=============================================================================
#ifndef __DIGICLOPS_H__
#define __DIGICLOPS_H__

//=============================================================================
// 
// Digiclops SDK v2.4 - Digiclops Stereo Vision Camera System Access Library
//
// Please see the release notes for the changes since the last version.
//
//=============================================================================

#ifdef WIN32

#ifndef	AAA_FN_TYPES_H
#	include "fn_types.h"
#endif

#ifdef DIGICLOPSLIB_EXPORTS                 // dll generation       //sr@20100408
#define DIGICLOPSLIB_API __declspec( dllexport )
#else

#ifdef  USE_LINKED_DIGICLOPSLIB             // do wrapped DLL calls //sr@20100408
	#define DIGICLOPSLIB_API    __declspec( dllimport )
#else// USE_LINKED_DIGICLOPSLIB
	#define DIGICLOPSLIB_API                // do wrapped DLL calls //sr@20100408
#endif//USE_LINKED_DIGICLOPSLIB

#endif

#else// DIGICLOPSLIB_EXPORTS
#define DIGICLOPSLIB_API
#endif


//=============================================================================
// PGR Includes
//=============================================================================
#include <triclops.h>

#ifdef __cplusplus
extern "C"
{
#endif

//=============================================================================
// Macro Definitions
//=============================================================================
// Group = Macro Definitions

//
// The version of the library.
//
#define DIGICLOPS_VERSION 20401027


// 32bpp BGRU packed image from the top camera.
#define TOP_IMAGE    0x1
// 32bpp BGRU packed image from the left camera.
#define LEFT_IMAGE   0x2
// 32bpp BGRU packed image from the right camera.
#define RIGHT_IMAGE  0x4
// 24bpp unpacked image from all 3 cameras.
#define STEREO_IMAGE 0x8   
// All images.
#define ALL_IMAGES   ( TOP_IMAGE | LEFT_IMAGE | RIGHT_IMAGE | STEREO_IMAGE )

#define DEFAULT_IMAGE_BUFFER NULL

//=============================================================================
// Type Definitions and Enumerations
//=============================================================================
// Group = Type Definitions

//
// Description: 
//  The type of the user level handle used to access individual Digiclops 
//  camera systems on the 1394 bus.
//
typedef void* DigiclopsContext;


//
// Description: 
//  The type of the various DigiclopsImageTypes.  These include TOP_IMAGE, 
//  LEFT_IMAGE, RIGHT_IMAGE, and STEREO_IMAGE. 
//
typedef unsigned long DigiclopsImageType;


//
// Description: 
//  The error codes returned by the functions in this library.
//
typedef enum DigiclopsError
{
   // Function completed successfully. Deprecated, use DIGICLOPS_OK.
   DIGICLOPS_ok   = 0,                    
   // Function completed successfully.
   DIGICLOPS_OK   = 0,                    
   // Device already initialized.
   DIGICLOPS_ALREADY_INITIALIZED,         
   // Grabbing has already been started.
   DIGICLOPS_ALREADY_STARTED,             
   // Callback is not registered
   DIGICLOPS_CALLBACK_NOT_REGISTERED,     
   // Callback is already registered
   DIGICLOPS_CALLBACK_ALREADY_REGISTERED, 
   // Problem controlling camera.
   DIGICLOPS_CAMERACONTROL_PROBLEM,       
   // Failed to open file.
   DIGICLOPS_COULD_NOT_OPEN_FILE,         
   // General failure.
   DIGICLOPS_FAILED,                      
   // Invalid argument passed.
   DIGICLOPS_INVALID_ARGUMENT,            
   // Invalid context passed.
   DIGICLOPS_INVALID_CONTEXT,             
   // Invalid image type passed.
   DIGICLOPS_INVALID_IMAGE_TYPE,          
   // Isoch grab error occured
   DIGICLOPS_ISOCH_GRAB_ERROR,            
   // Isoch grab not started
   DIGICLOPS_ISOCH_GRAB_NOT_STARTED,      
   // Error shutting down isoch grab
   DIGICLOPS_ISOCH_GRAB_SHUTDOWN_ERROR,   
   // Memory allocation error
   DIGICLOPS_MEMORY_ALLOC_ERROR,          
   // DigiclopsGrabImage() not called.
   DIGICLOPS_NO_IMAGE,                    
   // No Triclops context found on camera.
   DIGICLOPS_NO_TRICLOPS_CONTEXT_FOUND,   
   // Function not implemented.
   DIGICLOPS_NOT_IMPLEMENTED,                
   // Device not initialized.
   DIGICLOPS_NOT_INITIALIZED,             
   // DigiclopsStart() not called.
   DIGICLOPS_NOT_STARTED,                 
   // Request would exceed maximum bandwidth.
   DIGICLOPS_MAX_BANDWIDTH_EXCEEDED,      
   // Attempt to use driver on non-PGR camera
   DIGICLOPS_NON_PGR_CAMERA,		  
   // Invalid mode or framerate set or retrieved.
   DIGICLOPS_INVALID_MODE,		  
   // Unknown Error.
   DIGICLOPS_ERROR_UNKNOWN,		  

} DigiclopsError;


//
// Depricated message value for old bus callback mechanism.  Do not use.
// This will be removed in future versions of this SDK.
//
#define	 DIGICLOPS_BUS_INVALID  0
      
//
// Depricated message value for old bus callback mechanism.  Do not use.
// This will be removed in future versions of this SDK.
//
#define	 DIGICLOPS_BUS_VALID	 1
  
//
// Depricated function prototupe for old bus callback mechanism.  Do not use.
// This will be removed in future versions of this SDK.
//
typedef void DigiclopsBusNotificationCallback( int iBusNotificationMsg );


//
// A message returned from the bus callback mechanism indicating a bus reset.
//
#define DIGICLOPS_MESSAGE_BUS_RESET      0x02

//
// A message returned from the bus callback mechanism indicating a device has
// arrived on the bus.  ulParam contains the serial number of the device.
//
#define DIGICLOPS_MESSAGE_DEVICE_ARRIVAL 0x03

//
// A message returned from the bus callback mechanism indicating a device has
// been removed from the bus.  ulParam contains the serial number of the 
// device.
//
#define DIGICLOPS_MESSAGE_DEVICE_REMOVAL 0x04

//
// Function prototype for the bus callback mechanism.  pparam contains the 
// parameter passed in when registering the callback.  iMessage is one of the
// above DIGICLOPS_MESSAGE_* #defines and ulParam is a message-defined 
// parameter.
//
typedef void 
#ifndef LINUX
__cdecl 
#endif
DigiclopsCallback( void* pparam, int iMessage, unsigned long ulParam );


//
// Description: 
//  The type used to store the serial number uniquely identifying a Digiclops 
//  camera.
//
typedef unsigned long DigiclopsSerialNumber;


//
// Description: 
//  An enumeration used to identify the different image resolutions that are 
//  supported by the library.
//
typedef enum DigiclopsImageResolution
{
   DIGICLOPS_160x120,	   // 160 x 120 resolution.
   DIGICLOPS_320x240,      // 320 x 240 resolution.
   DIGICLOPS_640x480,      // 640 x 480 resolution.
   DIGICLOPS_1024x768,     // 1024 x 768 resolution.

} DigiclopsImageResolution;


//
// Description: 
//  An enumeration used to identify the settings for the size of the output 
//  image relative to the camera sensor resolution.
//
typedef enum DigiclopsOutputImageResolution
{
   DIGICLOPS_FULL,   // full resolution images
   DIGICLOPS_HALF,   // half resolution images

} DigiclopsOutputImageResolution;


//
// Description:
//  An enumeration used to describe the different camera color configurations.
//
typedef enum DigiclopsCameraType
{
   DIGICLOPS_BLACK_AND_WHITE, // black and white system
   DIGICLOPS_COLOR            // color system

} DigiclopsCameraType;


//
// Description:
//  An enumeration used to describe the camera device currently being 
//  controlled. 
//
typedef enum DigiclopsCameraDevice
{
   // Digiclops 3-camera system.
   DIGICLOPS_DEVICE_DIGICLOPS,
   // Bumblebee 2-camera system.
   DIGICLOPS_DEVICE_BUMBLEBEE,
   // This is an unknown device, or a device not supported by the Digiclops
   // library.
   DIGICLOPS_DEVICE_UNKNOWN,

} DigiclopsCameraDevice;


//
// Description:
//   An enumeration used to describe the different color processing 
//   method.
//
typedef enum DigiclopsColorMethod
{
   DIGICLOPS_DISABLE_COLOUR_PROCESSING,	     // do no color processing
   DIGICLOPS_NEAREST_NEIGHBOR,               // nearest neighbor de-mosaicing
   DIGICLOPS_EDGE_SENSING                    // edge sensing de-mosaicing

} DigiclopsColorMethod;


//
// Description: 
//  A record used in querying the Digiclops properties.
//
typedef struct DigiclopsInfo
{
   // Camera serial number
   DigiclopsSerialNumber      SerialNumber;
   // CCD resolution
   DigiclopsImageResolution   ImageSize;
   // Type of CCD (color or b&w)
   DigiclopsCameraType        CameraType;
   // Type of device.
   DigiclopsCameraDevice      CameraDevice;

} DigiclopsInfo;


//
// Description: 
//  The low-level 1394 bus timestamp.  Note that this is not an absolute
//  epoch-based timestamp, it is a relative timestamp that wraps around every
//  128 seconds.
//
// Note:
//  Use this timestamp for synchronizing cameras across the
//  Digiclops Sync Unit.  The addition of this structure is the only 
//  difference between a DigiclopsImage and a TriclopsInput.
//
typedef struct Digiclops1394Timestamp
{
   // The current seconds value of the 1394 bus time.  (0-127)
   unsigned long  ulCycleSeconds;
   // The current cycle count value of the 1394 bus time. (0-7999)
   unsigned long  ulCycleCount;

} Digiclops1394Timestamp;


//
// Description:
//  A wrapper for a TriclopsInput that includes a Digiclops1394Timestamp.
//
typedef struct DigiclopsImage
{
   // The low-level timestamp value for this DigiclopsImage.
   Digiclops1394Timestamp  digiclopsTimestamp;
   // The TriclopsInput structure contained in this DigiclopsImage.
   TriclopsInput	   triclopsInput;

} DigiclopsImage;


//
// Description:
//  Returned by digiclopsExtractRawImage().  Provides access to the raw
//  buffer returned by the camera driver.  For advanced users only.
//
typedef struct DigiclopsRawImage
{
   // Columns in the raw image.
   unsigned int   uiCols;
   // Rows in the image raw image.
   unsigned int   uiRows;
   // Number of bytes in each row.
   unsigned int   uiRowInc;

   // Timestamp of the image.
   TriclopsTimestamp       timeStamp;
   // The low-level timestamp value for this DigiclopsImage.
   Digiclops1394Timestamp  digiclopsTimestamp;

   // The raw image buffer.
   unsigned char* pBuffer;

} DigiclopsRawImage;


//
// Description:
//  Returned by the advanced image locking and buffering functions.  For 
//  advanced users only.
//
typedef struct DigiclopsRawImagePlus
{
   //
   // The DigiclopsRawImage that this DigiclopsRawImagePlus sturcture is 
   // wrapping.  Please see documentation for advanced locking functions.
   //
   DigiclopsRawImage image;

   //
   // The sequence number of the image.  This number is generated in the 
   // driver and sequential images should have a difference of one.  If
   // the difference is greater than one, it indicates the number of missed
   // images since the last lock image call.
   //
   unsigned int      uiSeqNum;

   //
   // The internal buffer index that the image buffer contained in the
   // DigiclopsRawImage corresponds to.  For functions that lock the image,
   // this number must be passed back to the "unlock" function.  If 
   // digiclopsInitializePlus() was called, this number corresponds to the 
   // position of the buffer in the buffer array passed in.
   //
   unsigned int      uiBufferIndex;

   //
   // Reserved for future use.
   //
   unsigned long     ulReserved[ 8 ];

} DigiclopsRawImagePlus;


//
// Description: 
//  An enumeration of the different camera properties that can be set via the 
//  API.
// 
// See Also:
//   digiclopsGetCameraPropertyRange(), 
//   digiclopsGetCameraProperty(), digiclopsSetCameraProperty()
//
typedef enum DigiclopsCameraProperty
{
   // A combination of gain and shutter.
   DIGICLOPS_AUTO_EXPOSURE,   
   // Camera shutter setting.
   DIGICLOPS_SHUTTER,             
   // Camera gain setting.
   DIGICLOPS_GAIN,                
   // Gain difference between top and right camera.
   DIGICLOPS_DELTA_GAIN_TOP,      
   // Gain difference between left and right camera.
   DIGICLOPS_DELTA_GAIN_LEFT,     
   // Lower bound on auto-gain.
   DIGICLOPS_AUTO_GAIN_LOW,       
   // Upper bound on auto-gain.
   DIGICLOPS_AUTO_GAIN_HIGH,      
   // Lower bound on auto-shutter.
   DIGICLOPS_AUTO_SHUTTER_LOW,    
   // Upper bound on auto-shutter.
   DIGICLOPS_AUTO_SHUTTER_HIGH,   
   //
   // Software White balance setting. Use this for Digiclops cameras, as they
   // do not have hardware whitebalance.  When using this property, the "A"
   // value corresponds to Red, and the "B" value corresponds to Blue.
   //
   DIGICLOPS_WHITEBALANCE,

   //
   // Hardware White balance setting. Use this for Bumblebee cameras, and 
   // make sure software white balance (DIGICLOPS_WHITEBALANCE, above,) is 
   // off.  When using this property, the "A" value corresponds to Red, and 
   // the "B" value corresponds to Blue.
   //
   DIGICLOPS_HARDWARE_WHITEBALANCE,

   // The number of settable digiclops camera properties.
   DIGICLOPS_PROPERTY_TOTAL,	  

} DigiclopsCameraProperty;


//
// Description: 
//  An enumeration of the normalized frame rate at which images are transmitted 
//  by the Digiclops camera.
//  
typedef enum DigiclopsFrameRate
{
   DIGICLOPS_FRAMERATE_100,   // 100 percent
   DIGICLOPS_FRAMERATE_050,   //  50 percent
   DIGICLOPS_FRAMERATE_025,   //  25 percent
   DIGICLOPS_FRAMERATE_012    //  12 percent

} DigiclopsFrameRate;

//=============================================================================
// Digiclops Bus Functions
//=============================================================================
// Group = Digiclops Bus

//-----------------------------------------------------------------------------
//
// Name: digiclopsBusCameraCount() 
//
// Description:
//   This function returns the number of cameras on the 1394 bus.
//
// Output Arguments:
//   puiCount - Returns the number of cameras on the bus.
// 
// Returns: 
//   A DigiclopsError indicating the success or failure of the function.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsBusCameraCount( 
			unsigned int*  puiCount );


//-----------------------------------------------------------------------------
//
// Name: digiclopsBusEnumerateCameras() 
//
// Description:
//   This function enumerates all of the Digiclops cameras found on the 1394 
//   bus.  It fills an array of DigiclopsInfo structures with all of the 
//   pertinent information from the attached cameras. The index of a given 
//   DigiclopsInfo structure in the array pDigiclopsInfoArray is the Digiclops 
//   device number.
//
// Output Arguments:
//   parInfo - An array of DigiclopsInfo structures, at least as
//             large as the number of Digiclops cameras on the bus.
//   puiSize - A pointer to an integer, set to the number of serial 
//             numbers (Digiclops cameras) found on the bus and 
//             written to pSerialNumberArray.
//
// Returns: 
//   A DigiclopsError indicating the success or failure of the function.
//
// See Also:
//   digiclopsBusCameraCount()
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsBusEnumerateCameras(
			     DigiclopsInfo*  parInfo,
			     unsigned int*   puiSize );


//-----------------------------------------------------------------------------
//
// Name: digiclopsBusRegisterNotificationCallback()
//
// Description:
//   This function is depricated.  Use digiclopsModifyCallback().
// 
// Returns: 
//   DIGICLOPS_NOT_IMPLEMENTED
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsBusRegisterNotificationCallback(
	 DigiclopsBusNotificationCallback* pCallbackFunction,
	 bool                              bRegister );


//-----------------------------------------------------------------------------
//
// Name: digiclopsModifyCallback()
//
// Description:
//   This function registers or deregisters a bus callback function.
//   When the state of the bus changes, the DigiclopsCallback
//   function will be called with an integer message parameter.  Please see
//   the DigiclopsCap example for more information on how to use callback
//   functionality.
//
// Arguments:
//   context - The context to access.
//   pfnCallback - A pointer to an externally defined callback function.
//   pparam - A user-specified parameter to be passed back to the callback 
//            function.  Can be NULL.
//   bAdd - true if the callback is to be added to the list of callbacks,
//          false if the callback is to be removed.
// 
// Returns: 
//   A DigiclopsError indicating the success or failure of the function.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsModifyCallback(
                        DigiclopsContext   context,
                        DigiclopsCallback* pfnCallback,
                        void*              pparam,
                        bool               bAdd );

//=============================================================================
// Construction/Destruction
//=============================================================================
// Group = Construction/Destruction

//-----------------------------------------------------------------------------
//
// Name: digiclopsCreateContext()
//
// Description:
//   This function creates a DigiclopsContext and allocates all of the memory 
//   that it requires.  The purpose of the DigiclopsContext is to act as a 
//   handle to one of the cameras attached to the system. This call must be 
//   made before any other calls involving the context will work.
//
// Output Arguments:
//   pContext - A pointer to the DigiclopsContext to be created.
//
// Returns:
//   DIGICLOPS_OK - Upon the successful creation of the DigiclopsContext.
//   DIGICLOPS_MEMORY_ALLOC_ERROR - If there was not enough memory to create the
//                                  context.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsCreateContext(
		       DigiclopsContext* pContext );


//-----------------------------------------------------------------------------
//
// Name: digiclopsDestroyContext() 
//
// Description:
//   Destroys the given DigiclopsContext.  In order to prevent memory leaks 
//   from occurring, this function must be called when the user is finished 
//   with the DigiclopsContext. 
//
// Input Arguments:
//   context - The DigiclopsContext to be destroyed.
//
// Returns:		  
//   A DigiclopsError indicating the success or failure of the function.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsDestroyContext(
			DigiclopsContext context );


//-----------------------------------------------------------------------------
//
// Name: digiclopsInitialize() 
//
// Description:
//   This function initializes one of the cameras on the bus and associates it
//   with the provided DigiclopsContext. This call must be made after a 
//   digiclopsCreateContext() command and prior to a digiclopsStart() command 
//   in order for images to be grabbed.  Users can also use the 
//   digiclopsInitializeFromSerialNumber() command to initialize a Digiclops 
//   with a specific serial number.
//
// Input Arguments:
//   context - The DigiclopsContext to be associated with the camera being 
//             initialized.
//   ulDevice - The 1394 bus index of the Digiclops camera to be initialized.
//
// Remarks:
//  If there is only one Digiclops on the bus, its index is generally 0.
// 
// Returns: 
//   A DigiclopsError indicating the success or failure of the function.
//
// See Also:
//   digiclopsInitializeFromSerialNumber(), digiclopsCreateContext(), 
//   digiclopsStart()
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsInitialize(
		    DigiclopsContext context,
		    unsigned long    ulDevice );


//-----------------------------------------------------------------------------
//
// Name: digiclopsInitializeFromSerialNumber()
//
// Description:
//   Similar to the digiclopsInitialize() command, this function initializes 
//   one of the cameras on the bus and associates it with the given 
//   DigiclopsContext.  This function differs from its counter part in that it
//   takes a serial number rather than a bus index.
//
// Input Arguments:
//   context - The DigiclopsContext to be associated with the camera being 
//             initialized.
//   serialNumber - The serial number of the Digiclops camera system to be 
//                  initialized.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
// See Also:
//   digiclopsInitialize(), digiclopsCreateContext(), digiclopsStart()
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsInitializeFromSerialNumber(
				    DigiclopsContext      context,
				    DigiclopsSerialNumber serialNumber );


//-----------------------------------------------------------------------------
//
// Name: digiclopsGetCameraInformation()
//
// Description:
//   Fills a DigiclopsInfo structure with the relevant information from the
//   camera system.
//
// Input Arguments:
//   context - The DigiclopsContext to access the Digiclops camera.
//
// Output Arguments:
//   pDigiclopsInfo - A pointer to a DigiclopsInfo structure.
// 
// Returns: 
//   A DigiclopsError indicating the success or failure of the function.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsGetCameraInformation(
			      DigiclopsContext context,
                              DigiclopsInfo*   pDigiclopsInfo );


//-----------------------------------------------------------------------------
//
// Name: digiclopsGetTriclopsContextFromCamera()
//
// Description:
//   This function allows the user to extract the information contained in
//   a standard Triclops configuration file directly from the camera and into
//   a TriclopsContext.
//
// Input Arguments:
//   context - The initialized DigiclopsContext to get the TriclopsContext from.
//
// Output Arguments:
//   pTriclopsContext - A pointer to a TriclopsContext to be filled.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
// Remarks:
//   Due to the fact that not all devices will have been upgraded to have a 
//   configuration file on them, this function will check to see if there is a 
//   configuration file on the camera, then it will look for a configuration 
//   file on the disk named 'digiclopsXXXXXXX.cal' where XXXXXXX is the 
//   device's serial number.  If this is not successful, it will look for a 
//   file name 'config'.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsGetTriclopsContextFromCamera(
				      DigiclopsContext context,
				      TriclopsContext* pTriclopsContext );


//=============================================================================
// General Functions
//=============================================================================
// Group = General

//-----------------------------------------------------------------------------
//
// Name: digiclopsGetLibraryVersion()
//
// Description:
//  This function returns the version of the library in the format 
//  100*(major version)+(minor version).
//
// Returns:
//  An integer indicating the current version of the library.
//
DIGICLOPSLIB_API int MAAAPIENTRY
digiclopsGetLibraryVersion();


//-----------------------------------------------------------------------------
//
// Name: digiclopsGetLastError() 
//
// Description:
//   Deprecated.  This function will be removed in a future release.

// Input Arguments:
//   context - The DigiclopsContext to examine.
//
// Returns: 
//   DIGICLOPS_NOT_IMPLEMENTED.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsGetLastError( 
		      DigiclopsContext context );


//-----------------------------------------------------------------------------
//
// Name: digiclopsErrorToString() 
//
// Description:
//    This function returns a description of the provided DigiclopsError.
//
// Input Arguments:
//   error - The Digiclops error to be parsed.
//
// Returns: 
//   A null-terminated character string that describes the Digiclops error.
//
DIGICLOPSLIB_API const char* MAAAPIENTRY
digiclopsErrorToString( 
		       DigiclopsError error );


//=============================================================================
// Control Functions
//=============================================================================
// Group = Control Functions

//-----------------------------------------------------------------------------
//
// Name:  digiclopsStart()
//
// Description:
//   This function starts the image grabbing process.  It should be called 
//   after digiclopsCreateContext() and digiclopsInitialize().
//
// Input Arguments:
//   context - The DigiclopsContext to start grabbing.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
// See Also:
//   digiclopsCreateContext(), digiclopsInitialize(), 
//   digiclopsInitializeFromSerialNumber(), digiclopsStop()
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsStart(
	       DigiclopsContext context );


//-----------------------------------------------------------------------------
//
// Name:
//  digiclopsStop() 
//
// Description:
//   This function halts all image grabbing for the specified DigiclopsContext.
//
// Input Arguments:
//   context - The DigiclopsContext to stop.
//
// Returns:       
//   A DigiclopsError indicating the success or failure of the function.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsStop(
	      DigiclopsContext context );


//=============================================================================
// Image Related Functions
//=============================================================================
// Group = Image Related Functions

//-----------------------------------------------------------------------------
//
// Name:  digiclopsGetImageTypes() 
//
// Description:
//   Allows the user to determine the specific images 
//   being grabbed when digiclopsGrabImage() is called.
//
// Input Arguments:
//   context - The DigiclopsContext to access.
//
// Output Arguments:
//   pImageTypes - All of the image types to be grabbed, OR'd together.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
// Remarks: 
//   By default, the allocated image types are ( RIGHT_IMAGE | STEREO_IMAGE ).
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsGetImageTypes(
		       DigiclopsContext	     context,
		       DigiclopsImageType*   pImageTypes );


//-----------------------------------------------------------------------------
//
// Name: 
//   digiclopsSetImageTypes() 
//
// Description:
//   Allows the user to set the specific images to be grabbed when 
//   digiclopsGrabImage() is called.
//
// Input Arguments:
//   context	  The Digiclops context to access.
//   imageTypes	  The different camera images to be grabbed.  The supported
//                combinations are:
//                * ALL_IMAGES which grabs all of the images
//                * STEREO_IMAGE which grabs only the stereo image
//                * RIGHT_IMAGE | STEREO_IMAGE which grabs both the right 
//                   and stereo images
//                ALL OTHER COMBINTATIONS WILL RESULT IN ALL IMAGES BEING 
//                GENERATED.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
// Remarks:
//  The fewer the number of images being generated, the lower the processing
//  requirements.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsSetImageTypes(
		       DigiclopsContext   context,
		       DigiclopsImageType imageTypes );


//-----------------------------------------------------------------------------
//
// Name: digiclopsGetColorProcessing()
//
// Description:
//   This function allows users to check the current color processing method.
//
// Input Arguments:
//   context - The Digiclops context to access.
//
// Output Arguments:
//   pMethod - A pointer to a DigiclopsColorMethod that will store the current
//             color processing method.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//  
// See Also:
//  digiclopsSetColorProcessing()
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsGetColorProcessing(
			    DigiclopsContext	   context,
			    DigiclopsColorMethod*  pMethod );


//-----------------------------------------------------------------------------
//
// Name: digiclopsSetColorProcessing()
//
// Description:
//   This function allows users to select the method used for color processing.
//
// Input Arguments:
//   context - The Digiclops context to access.
//   method  - A variable of type DigiclopsColorMethod indicating the color
//             processing method to be used.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
// Remarks:
//   Selecting DIGICLOPS_DISABLE_COLOUR_PROCESSING turns off all color
//   processing.  DIGICLOPS_NEAREST_NEIGHBOR uses a fast nearest neighbor
//   approach to de-mosaic the images and DIGICLOPS_EDGE_SENSING uses a more
//   expensive method to produce higher quality color images.
//  
// See Also:
//   digiclopsGetColorProcessing()
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsSetColorProcessing(
			    DigiclopsContext     context,
			    DigiclopsColorMethod method );


//-----------------------------------------------------------------------------
//
// Name: digiclopsDoColorProcessing()
//
// Description:
//   Deprecated.  Please use digiclopsProcessRaw().
//
// Input Arguments:
//    pStippledData	- The stippled image data as read from a STEREO_IMAGE 
//                       .ppm (RGB format.)
//    iRows		- Image rows.
//    iCols		- Image columns.
//    colorMethod	- Color processing method to use
//    iWhiteBalanceRed	- Red value for whitebalance calculations.  Use -1 for
//                        an internal default value.
//    iWhiteBalanceBlue	- Blue value for whitebalance calculations.  Use -1 for
//                        an internal default value.
//
// Output Arguments:
//    pinputTop - A TriclopsInput pointer that will contain the Top image.
//    pinputLeft - A TriclopsInput pointer that will contain the Left image.
//    pinputRight - A TriclopsInput pointer that will contain the Right image.
//    pinputStereo - A TriclopsInput pointer that will contain the Stereo image.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//  
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsDoColorProcessing(
			   const unsigned char*	pStippledData,
			   int			iRows,
			   int			iCols,
			   TriclopsInput*	pinputTop,
			   TriclopsInput*	pinputLeft,
			   TriclopsInput*	pinputRight,
			   TriclopsInput*	pinputStereo,
			   DigiclopsColorMethod colorMethod,
			   int			iWhiteBalanceRed,
			   int			iWhiteBalanceBlue  );


//-----------------------------------------------------------------------------
//
// Name: digiclopsProcessRaw()
//
// Description:
//   This function allows users to process raw images.
//
// Input Arguments:
//    context        - The context to access.
//    pRawImage      - The DigiclopsRawImage to parse.
//
// Output Arguments:
//    pinputTop      - The destination Top image.
//    pinputLeft     - The destination Left image.
//    pinputRight    - The destination Right image.
//    pinputStereo   - The destination Stereo image.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
// Remarks:
//   This function replaced digiclopsDoColorProcessing
//   and supports Bumblebee cameras.  Colour processing methods, whitebalance
//   values, and bayer orientation will be read from the context, so it is
//   a good idea to initialize the camera attached to the context.  This is not 
//   required.  The 4 triclops inputs will be generated based on the images
//   set by digiclopsSetImageTypes().  The TriclopsInputs must be populated
//   and have valid data members pointing to valid memory.
//  
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsProcessRaw(
                    DigiclopsContext         context,
                    const DigiclopsRawImage* pRawImage,
                    TriclopsInput*	     pinputTop,
                    TriclopsInput*	     pinputLeft,
                    TriclopsInput*	     pinputRight,
                    TriclopsInput*	     pinputStereo );


//-----------------------------------------------------------------------------
//
// Name: digiclopsGetMaxFrameRate()
//
// Description:
//    This function gets the maximum frame rate that has been allocated for 
//    the camera system.  This frame rate will dictate the maximum frame rate  
//    that the camera can be set to.  Additionally, this will affect the number
//    of cameras that can be used on the same bus simultaneously.  The following
//    table illustrates what percentage of the bus is required for each frame
//    rate and camera resolution.
//
//
//	    |                        | 640 x 480 | 1024 x 768 |
//	    -------------------------+-----------+------------+
//	    DIGICLOPS_FRAME_RATE_100 |    60.0%  |     65%    |
//	    DIGICLOPS_FRAME_RATE_050 |    30.0%  |     32%    |
//	    DIGICLOPS_FRAME_RATE_025 |    15.0%  |     16%    |
//	    DIGICLOPS_FRAME_RATE_012 |     7.5%  |      8%    |
//
//
// Input Arguments:
//   context - The DigiclopsContext associated with the camera to access.
//
// Output Arguments:
//   pFrameRate - A pointer to a DigiclopsFrame, set to the current maximum
//                frame rate.
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsGetMaxFrameRate(
			 DigiclopsContext    context,
			 DigiclopsFrameRate* pFrameRate );


//-----------------------------------------------------------------------------
//
// Name: digiclopsSetMaxFrameRate()
//
// Description:
//   This function sets the maximum frame rate that this camera will use.
//
// Input Arguments:
//   context - The DigiclopsContext associated with the camera to access.
//   frameRate - The new maximum frame rate for this camera.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
// Remarks: This function cannot be called after digiclopsStart().
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsSetMaxFrameRate(
			 DigiclopsContext   context,
			 DigiclopsFrameRate frameRate );


//-----------------------------------------------------------------------------
//
// Description:
//
// Name:  digiclopsGetFrameRate() 
//
// Description:
//   Gets the current rate at which frames are being transmitted from the
//   Digiclops camera. 
//
// Input Arguments:
//   context   - The DigiclopsContext associated with the camera to access.
//
// Output Arguments:
//   pFrameRate - A pointer to a DigiclopsFrameRate, set to the current frame 
//                rate.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsGetFrameRate(	
		      DigiclopsContext    context,
		      DigiclopsFrameRate* pFrameRate );


//-----------------------------------------------------------------------------
//
// Name: 
//   digiclopsSetFrameRate() 
//
// Description:
//   Sets the rate at which data is transmitted from the Digiclops camera.
//
// Input Arguments:
//   context   - The DigiclopsContext associated with the camera to access.
//   frameRate - One of DIGICLOPS_FRAMERATE_100, DIGICLOPS_FRAMERATE_050, 
//               DIGICLOPS_FRAMERATE_025, or DIGICLOPS_FRAMERATE_012, 
//               indicating the rate at which data should be transmitted 
//               from the camera.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
// Remarks:
//   Currently, users must manage their own bandwidth.  That is, there is no 
//   checking to ensure that the bus is not overallocated.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsSetFrameRate(
		      DigiclopsContext   context,
		      DigiclopsFrameRate frameRate );


//-----------------------------------------------------------------------------
//
// Name:  digiclopsGetImageResolution() 
//
// Description:
//   Allows the user to query the current resolution of the images being 
//   grabbed.
//
// Input Arguments:
//   context - The Digiclops context to access.
//
// Output Arguments:
//   pOutputImageResolution  - A pointer to a DigiclopsImageResolution variable 
//			       that will contain the result of the operation.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
// Remarks: The lower the resolution, the lower the processing requirements.			     
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsGetImageResolution(
			    DigiclopsContext		    context,
			    DigiclopsOutputImageResolution* pOutputImageResolution );


//-----------------------------------------------------------------------------
//
// Name:  digiclopsSetImageResolution() 
//
// Description:
//   Allows the user to set the resolution
//   of the images being grabbed.
//
// Input Arguments:
//   context		      The Digiclops context to access.
//   outputImageResolution    A variable of type DigiclopsImageResolution that
//			      contains the resolution of the images to be 
//			      grabbed.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsSetImageResolution(
			    DigiclopsContext               context,
			    DigiclopsOutputImageResolution outputImageResolution );


//-----------------------------------------------------------------------------
//
// Name: digiclopsSetImageBuffer()
//
// Description:
//   This function allows the user to set the individual buffers that will 
//   images will be stored in.
//
// Input Arguments:
//  context   - The DigiclopsContext to set the buffer for.
//  imageType - A DigiclopsImageType indicating which buffer to set. This is 
//              one of TOP_IMAGE, LEFT_IMAGE, RIGHT_IMAGE or STEREO_IMAGE.  
//  pBuffer   - A pointer to a buffer large enough to hold the indicated image.  
//              This parameter should be set to either 'NULL' or 
//              DEFAULT_IMAGE_BUFFER.if the user does not want to be 
//              responsible for the buffer.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsSetImageBuffer(
			DigiclopsContext   context,
			DigiclopsImageType imageType,
			unsigned char*     pBuffer );


//-----------------------------------------------------------------------------
//
// Name:  digiclopsGrabImage() 
//
// Description:
//   This function grabs the newest image from the Digiclops camera system, 
//   effectively locking it for the user.  This function
//   must be called before a digiclopsExtractTriclopsInput() command or a
//   digiclopsWritePPM() command.
//
// Input Arguments:
//   context - The Digiclops context to lock the image in.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
// Remarks: If the user attempts to extract or save an image that hasn't been
//   allocated using digiclopsSetImageTypes(), the user will receive a grey ramp.
//
// See Also:
//  digiclopsStart()
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsGrabImage(
		   DigiclopsContext context );


//-----------------------------------------------------------------------------
//
// Name: digiclopsExtractTriclopsInput() 
//
// Description:
//   Allows the user to extract the latest grabbed image into a TriclopsInput 
//   structure.
//
// Input Arguments:
//   context        - The Digiclops context to extract from.
//   imageType      - The type of image to extract (Remarks: only one image 
//		      can be extracted at a time.
//
// Output Arguments:
//   ptriclopsInput - A pointer to the TriclopsInput structure.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsExtractTriclopsInput(
			      DigiclopsContext	 context,
			      DigiclopsImageType imageType,
			      TriclopsInput*	 ptriclopsInput );


//-----------------------------------------------------------------------------
//
// Name: digiclopsExtractDigiclopsImage() 
//
// Description:
//   Allows the user to extract the latest grabbed image into a DigiclopsImage 
//   structure.
//
// Input Arguments:
//   context	     - The Digiclops context to extract from.
//   imageType	     - The type of image to extract (Remarks: only one image 
//		       can be extracted at a time.
// 
// Output Arguments:
//   pdigiclopsImage - A pointer to the DigiclopsImage structure to be filled 
//                     in.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
// Remarks:
//   This function is identical to digiclopsExtractTriclopsInput() except for
//   the return type and extra information contained therein.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsExtractDigiclopsImage(
			      DigiclopsContext	 context,
			      DigiclopsImageType imageType,
			      DigiclopsImage*	 pdigiclopsImage );


//-----------------------------------------------------------------------------
//
// Name: digiclopsExtractRawImage() 
//
// Description:
//   Provides access to the raw camera buffers.  Advanced use only.
//   
// Input Arguments:
//   context	     - The Digiclops context to extract from.
// 
// Output Arguments:
//   pdigiclopsImage - A pointer to the DigiclopsImage structure to be filled 
//                     in.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
// Remarks:
//   This function is for advanced users only.  The format of the returned
//   image buffer is dependant on the camera being accessed by the context.
//   For Digiclopses it is the 3 images byte-interleaved, and for Bumblebees
//   is is the two images byte-interleaved.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsExtractRawImage(
                         DigiclopsContext    context,
                         DigiclopsRawImage*  prawimage );


//-----------------------------------------------------------------------------
//
// Name:  digiclopsWritePPM() 
//
// Description:
//   Writes the last grabbed image to disk.
//
// Input Arguments:
//   context		- The Digiclops context to write the image from.
//   imageType		- The type of image to be written
//   pszFileName        - The name of the file to write to.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsWritePPM(
		  DigiclopsContext     context,
		  DigiclopsImageType   imageType,
		  const char*	       pszFileName );


//-----------------------------------------------------------------------------
//
// Name:  digiclopsWritePPMFromDigiclopsImage() 
//
// Description:
//   Writes a DigiclopsImage to disk.
//
// Input Arguments:
//   digiclopsImage - The DigiclopsImage to use.
//   pszFileName    - The name of the file to write to.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
// 
// Remarks:
//   This function duplicates the functionality of the pnmutils library
//   but adds read/write capabilities for Digiclops1394Timestamp.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsWritePPMFromDigiclopsImage(
				    DigiclopsImage digiclopsImage,
				    const char*	   pszFileName );


//-----------------------------------------------------------------------------
//
// Name:  digiclopsReadPPMToDigiclopsImage() 
//
// Description:
//   Reads a .PPM file into a DigiclopsImage.
//
// Input Arguments:
//   pszFileName     - The name of the file to read from.
// 
// Output Arguments:
//   pdigiclopsImage - A pointer to the DigiclopsImage to fill in.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
// 
// Remarks:
//   This function duplicates the functionality of the pnmutils library
//   but adds read/write capabilities for Digiclops1394Timestamp.  This
//   function allocates memory in the TriclopsInput structure within the 
//   DigiclopsImage.  Use freeInput() in the pnmutils library to free this
//   memory.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsReadPPMToDigiclopsImage(
				 DigiclopsImage*   pdigiclopsImage,
				 const char*	   pszFileName );


//-----------------------------------------------------------------------------
//
// Name:  digiclopsWrite3PGM() 
//
// Description: 
//   Writes the last grabbed image to 3 pgm files on disk.
//
// Input Arguments:
//   context - The Digiclops context to write the image from.
//   imageType		- The type of image (RIGHT_IMAGE, etc)
//   pszRedFileName     - The name of the red file to write to.
//   pszGreFileName     - The name of the green file to write to.
//   pszBluFileName     - The name of the blue file to write to.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsWrite3PGM( 
		   DigiclopsContext    context, 
		   DigiclopsImageType  imageType,
		   const char*	       pszRedFileName,
		   const char*	       pszGreFileName,
		   const char*	       pszBluFileName );


//=============================================================================
// Camera Property Functions
//=============================================================================
// Group = Camera Property Functions


//-----------------------------------------------------------------------------
//
// Name:  digiclopsGetCameraPropertyRange() 
//
// Description:
//   Allows the user to examine the default, minimum, maximum, and auto 
//   characteristics for the given property.
//
// Input Arguments:
//   context        - The Digiclops context to extract the properties from.
//   cameraProperty - A DigiclopsCameraProperty indicating the property to 
//                    examine.
//
// Output Arguments:
//   pbPresent      - The Presence property value.
//   plMin          - The Minimum property value.
//   plMax          - The Maximum property value.
//   plDefault      - The Default property value.
//   pbAuto	    - The Auto property value.
//   pbManual	    - The Manual property value.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
// Remarks:
//   Any of the output argument pointers can be replaced with NULL to
//   ignore that return value.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsGetCameraPropertyRange(
				DigiclopsContext	 context,
				DigiclopsCameraProperty	 cameraProperty,
				bool*			 pbPresent,
				long*			 plMin,
				long*			 plMax,
				long*			 plDefault,
				bool*			 pbAuto,
				bool*			 pbManual );


//-----------------------------------------------------------------------------
//
// Name:  digiclopsGetCameraProperty() 
//
// Description:
//   Allows the user to query the current value of the given property.
//
// Input Arguments:
//   context        - The Digiclops context to extract the properties from.
//   cameraProperty - A DigiclopsCameraProperty indicating the property to 
//                    query.
//
// Output Arguments:
//   plValueA       - The current value of the property.
//   plValueB       - The current secondary value of the property.  This is 
//                    currently only used for DIGICLOPS_WHITEBALANCE.
//   pbAuto         - The current value of the property.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
// Remarks:
//   Any of the output argument pointers can be replaced with NULL to
//   ignore that return value.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsGetCameraProperty(
			   DigiclopsContext        context,
			   DigiclopsCameraProperty cameraProperty,
			   long*		   plValueA,
			   long*                   plValueB,
			   bool*                   pbAuto );


//-----------------------------------------------------------------------------
//
// Name:  digiclopsSetCameraProperty() 
//
// Description:
//   Allows the user to set the given property.
//
// Input Arguments:
//   context        - The DigiclopsContext to set the properties in.
//   cameraProperty - A DigiclopsCameraProperty indicating the property to set.
//   lValueA        - A long containing the new value of the property.
//   lValueB        - A long containing the secondary new value of the property.
//                    This argument is currently only used for the 
//                    DIGICLOPS_WHITEBALANCE property.
//   bAuto          - A boolean containing the new 'auto' state of the property.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
// Remarks: Calling this function with DIGICLOPS_WHITEBALANCE as the 
//   cameraProperty parameter and 'true' for the bAuto parameter will invoke 
//   a single shot auto white balance method.  The assumption is that 
//   digiclopsGrabImage() has been called previously with a white object 
//   centered in the field of view.  
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsSetCameraProperty(
			   DigiclopsContext        context,
			   DigiclopsCameraProperty cameraProperty,
			   long			   lValueA,
			   long                    lValueB,
			   bool                    bAuto );


//-----------------------------------------------------------------------------
// Name:  digiclopsGetCameraRegister()
//
// Description:
//   This function allows the user to get any of camera's registers.
//
// Arguments:
//   context    - The context associated with the camera to be queried.
//   ulRegister - The 12bit register location to query.
//   pulValue   - The 32bit value currently stored in the register.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsGetCameraRegister(
			   DigiclopsContext  context,
			   unsigned long     ulRegister,
			   unsigned long*    pulValue );


//-----------------------------------------------------------------------------
// Name:  digiclopsSetCameraRegister()
//
// Description:
//   This function allows the user to get any of the camera's registers.
//
// Arguments:
//   context    - The context associated with the camera to be queried.
//   ulRegister - The 12bit register location to query.
//   ulValue    - The 32bit value to store in the register.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsSetCameraRegister(
			   DigiclopsContext  context,
			   unsigned long     ulRegister,
			   unsigned long     ulValue );


//-----------------------------------------------------------------------------
//
// Name:  digiclopsGetCameraPropertyRangeEx() 
//
// Description:
//   Replaces digiclopsGetCameraPropertyRange() and provides better access to 
//   camera features.
//
// Arguments:
//   context        - The Digiclops context to extract the properties from.
//   cameraProperty - A DigiclopsProperty indicating the property to examine.
//   pbPresent      - Indicates the presence of this property on the camera.
//   pbOnePush      - Indicates the availablilty of the one push feature.
//   pbReadOut      - Indicates the ability to read out the value of this property.
//   pbOnOff        - Indicates the ability to turn this property on and off.
//   pbAuto         - Indicates the availability of auto mode for this property.
//   pbManual       - Indicates the ability to manually control this property.
//   piMin          - The minimum value of the property is returned in this 
//                    argument.
//   piMax          - The maximum value of the property is returned in this 
//                    argument.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
// Remarks:
//   Pass NULL for any pointer argument to ignore that argument.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsGetCameraPropertyRangeEx(
                                  DigiclopsContext          context,
                                  DigiclopsCameraProperty   cameraProperty,
                                  bool*                     pbPresent,
                                  bool*                     pbOnePush,
                                  bool*                     pbReadOut,
                                  bool*                     pbOnOff,
                                  bool*                     pbAuto,
                                  bool*		                pbManual,
                                  int*                      piMin,
                                  int*                      piMax );


//-----------------------------------------------------------------------------
//
// Name:  digiclopsGetCameraPropertyEx() 
//
// Description:
//   Replaces digiclopsGetCameraProperty() and provides better access to 
//   camera features.
//
// Arguments:
//   context        - The Digiclops context to extract the properties from.
//   cameraProperty - A DigiclopsProperty indicating the property to query.
//   pbOnePush      - The value of the one push bit.
//   pbOnOff        - The value of the On/Off bit.
//   pbAuto         - The value of the Auto bit.
//   piValueA       - The current value of this property.
//   piValueB       - The current secondary value of this property. (only
//                    used for the two whitebalance values.)
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
// Remarks:
//   Pass NULL for any pointer argument to ignore that argument.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsGetCameraPropertyEx(
                             DigiclopsContext         context,
                             DigiclopsCameraProperty  cameraProperty,
                             bool*                    pbOnePush,
                             bool*                    pbOnOff,
                             bool*                    pbAuto,
                             int*                     piValueA,
                             int*                     piValueB );


//-----------------------------------------------------------------------------
//
// Name:  digiclopsSetCameraPropertyEx() 
//
// Description:
//   Replaces digiclopsSetCameraPropertyEx() and provides better access to 
//   camera features.
//
// Arguments:
//   context        - The DigiclopsContext to set the properties in.
//   cameraProperty - A DigiclopsProperty indicating the property to set.
//   bOnePush       - Set the one push bit.
//   bOnOff         - Set the on/off bit.
//   bAuto          - Set the auto bit.
//   iValueA        - The value to set.
//   iValueB        - The seconary value to set.  (only used for the two
//                    whitebalance values.)
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsSetCameraPropertyEx(
                             DigiclopsContext         context,
                             DigiclopsCameraProperty  cameraProperty,
                             bool                     bOnePush,
                             bool                     bOnOff,
                             bool                     bAuto,
                             int                      iValueA,
                             int                      iValueB );


//=============================================================================
// Advanced Buffer Functions
//=============================================================================
// Group = Advanced Buffer Functions

//-----------------------------------------------------------------------------
// Name:  digiclopsInitializePlus()
//
// Description:
//   Identical behaviour to digiclopsInitialize(), except that the user has
//   the option of specifying the number of buffers to use, and optionally
//   allocate those buffers outside the library.
//
// Arguments:
//   context    - The DigiclopsContext associated with the camera to be queried.
//   uiBusIndex - The zero-based device index of the camera to be initialized.
//   uiNumBuffers - The number of buffers to expect or allocate.
//   arpBuffers   - An array of pointers to buffers.  If this argument is NULL
//                  the library will allocate and free the buffers internally,
//                  otherwise the caller is responsible for allocation and 
//                  deallocation.  No boundary checking is done on these 
//                  images, if you are supplying your own buffers, they must
//                  be large enough to hold the largest image you are 
//                  expecting.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
// Remarks:
//   If you wish to use the camera serial number to initialize, or you don't 
//   care about the number of buffers being allocated, use either of the
//   other initialize methods.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsInitializePlus(
                        DigiclopsContext  context,
                        unsigned int      uiBusIndex,
                        unsigned int      uiNumBuffers,
                        unsigned char**   arpBuffers );


//-----------------------------------------------------------------------------
// Name:  digiclopsStartLockNext()
//
// Description:
//   Starts the camera streaming and initializes the library for "grab next"
//   functionality.  This function needs to used instead of digiclopsStart()
//   for the following "lock next" functions.
//
// Arguments:
//   context   - The context associated with the camera to be started.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
// Remarks:
//   For "lock latest" functionality, use digiclopsStart() and the 
//   digiclopsLockLatest().
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsStartLockNext(
                       DigiclopsContext context );


//-----------------------------------------------------------------------------
// Name:  digiclopsSyncForLockNext()
//
// Description:
//  Takes in an array of contexts attached to multiple cameras that 
//  are synchronized in hardware and assures that the next time lockNext()
//  is called for all contexts, the images locked will correspond to one 
//  another.  Note that this function only needs to be called once after the
//  contexts have been started.  The contexts should be started in the same
//  order that they are listed in arContexts before this function is called.
//
// Arguments:
//   arContexts      - An array of contexts attached to the cameras to 
//                     synchronize.
//   uiNumContexts   - The number of contexts in arContext.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
// Remarks:
//   This function operates by skipping the appropriate number of images
//   in contexts that were started "after" the reference context (position 0
//   in the array).  If this function fails it does not necessarily mean the
//   cameras are out of sync.  This is still experimental.  Note also that
//   this function will turn on image timestamping.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsSyncForLockNext(
                         DigiclopsContext*   arContexts,
                         unsigned int        uiNumContexts );


//-----------------------------------------------------------------------------
// Name:  digiclopsLockNext()
//
// Description:
//   Lock the "next" image that has not been seen.  Provided that the previous 
//   image processing time is not greater than the time taken for the camera
//   to transmit images to the available unlocked buffers, this function can
//   be called repeatedly to guarantee that each image will be seen.  If the 
//   camera has not finished transmitting the next image, this function will 
//   block.  Users can verify image sequentiality by comparing sequence 
//   numbers of sequential images. 
//
// Arguments:
//   context   - The DigiclopsContext associated with the camera to be queried.
//   pimage    - The returned DigiclopsImagePlus.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
// Remarks:
//   digiclopsUnlock() must be called using the buffer index returned in 
//   pimage when processing on this image has been completed.  The camera must
//   have been started using digiclopsStartLockNext() for this function to
//   succeed.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsLockNext(
                  DigiclopsContext        context,
                  DigiclopsRawImagePlus*  pimage );


//-----------------------------------------------------------------------------
// Name:  digiclopsLockLatest()
//
// Description:
//   Lock the "latest" image that has not been seen.  If there is an unseen 
//   image waiting, this function will return immediately with that image, 
//   otherwise it will block until the next image has been received.  The 
//   difference in the sequence numbers of images returned by consecutive calls
//   to this function indicates the number of missed images between calls.
//
// Arguments:
//   context - The DigiclopsContext associated with the camera to be queried.
//   pimage  - The returned DigiclopsImagePlus.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
// Remarks:
//   digiclopsUnlock() must be called using the buffer index returned in 
//   pimage when processing on this image has been completed.  This function
//   behaves identically to digiclopsGrabImage(), except it doesn't implictly
//   unlock the previously seen image first.  The camera must have been 
//   started using digiclopsStart() in order for this function to succeed.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsLockLatest(
                    DigiclopsContext         context,
                    DigiclopsRawImagePlus*   pimage );


//-----------------------------------------------------------------------------
// Name:  digiclopsUnlock()
//
// Description:
//   Returns a buffer into the pool to be filled by the camera driver.  This 
//   must be called for each image locked using the above lock functions after
//   processing on that image has been completed.
//
// Arguments:
//   context - The DigiclopsContext associated with the camera to be queried.
//   uiBufferIndex - The buffer to unlock.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsUnlock(
                DigiclopsContext context,
                unsigned int     uiBufferIndex );


//-----------------------------------------------------------------------------
// Name:  digiclopsUnlockAll()
//
// Description:
//   Unlocks all locked images.  This is equivalant to maintaining a list of 
//   locked buffers and calling digiclopsUnlock() for each.
//
// Arguments:
//   context - The DigiclopsContext associated with the camera to be queried.
//
// Returns:
//   A DigiclopsError indicating the success or failure of the function.
//
DIGICLOPSLIB_API DigiclopsError MAAAPIENTRY
digiclopsUnlockAll(
                   DigiclopsContext  context );





#ifdef __cplusplus
};
#endif



#endif // #ifndef __DIGICLOPS_H__
