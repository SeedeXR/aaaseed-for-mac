//=============================================================================
// Copyright © 2000-2004 Point Grey Research, Inc. All Rights Reserved.
// 
// This software is the confidential and proprietary information of Point
// Grey Research, Inc. ("Confidential Information").  You shall not
// disclose such Confidential Information and shall use it only in
// accordance with the terms of the license agreement you entered into
// with PGR.
// 
// PGR MAKES NO REPRESENTATIONS OR WARRANTIES ABOUT THE SUITABILITY OF THE
// SOFTWARE, EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
// IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
// PURPOSE, OR NON-INFRINGEMENT. PGR SHALL NOT BE LIABLE FOR ANY DAMAGES
// SUFFERED BY LICENSEE AS A RESULT OF USING, MODIFYING OR DISTRIBUTING
// THIS SOFTWARE OR ITS DERIVATIVES.
//=============================================================================

#ifndef __CENSYS_H__
#define __CENSYS_H__


//=============================================================================
// Includes
//=============================================================================
#if defined( CENSYS_INTERNAL ) || !defined( _WIN32 )
#	include "censysinternaldefs.h"
#endif

#ifdef CENSYS_EMBEDDED
#	include "triclops.h"
#else
#	include "digiclops.h"
#endif

//=============================================================================
// Macros
//=============================================================================
// Group=Macros

#ifdef _WIN32
#	ifdef CENSYS_EXPORTS
#		define CENSYS_API __declspec( dllexport )
#	else  // #ifdef CENSYS_EXPORTS

		#ifdef  USE_LINKED_CENSYS_LIB           // do wrapped DLL calls //sr@20100409
#			define CENSYS_API      __declspec( dllimport )
#		else// USE_LINKED_CENSYS_LIB
#			define CENSYS_API                  // do wrapped DLL calls //sr@20100409
#		endif//USE_LINKED_CENSYS_LIB

#	endif // #ifdef CENSYS_EXPORTS
#else  // #ifdef _WIN32
#	define CENSYS_API
#endif // #ifdef _WIN32


#ifndef ALIGNED
#	define ALIGNED
#endif

//=============================================================================
// Definitions
//=============================================================================

// Group=Definitions

//
// Name: CENSYS_VERSION
//
// Description:
//  The version of the library.
//
#define CENSYS_VERSION 20000002

//
// Group=Definitions
//

//
// Name: INVALID_CENSYS_INSTANCE
//
// Description:
//  The definition of an invalid instance of a *Censys3D* system.
//
// See Also:
//  CENSYS_INSTANCE
//
#define INVALID_CENSYS_INSTANCE (CENSYS_INSTANCE)0

//
// Name: CENSYS_AUTO
//
// Description:
//  Used to indicate a numeric setting whose value is to be set automatically 
//  by the system.
//
#define CENSYS_AUTO -999999


//=============================================================================
// Enumerations
//=============================================================================

// Group=Enumerations

//
// Name: CENSYS_ERROR
//
// Description:
//  This enumeration lists the different error codes that can be returned from 
//  the *Censys3D* SDK.  The following error codes are common to virtually all 
//  API functions, and have been omitted from all function descriptions to
//  reduce clutter:
//     - censyserrorSUCCESS
//     - censyserrorFAILED
//     - censyserrorINVALID_ARGUMENT
//     - censyserrorINVALID_INSTANCE
//
//  All other abnormal return codes along with possible causes of failure are 
//  listed with the description of the respective function.  For a description
//  of any error code, call censysErrorToString().  For details of the cause of 
//  an error, call censysGetLastError().
//
typedef enum CENSYS_ERROR
{
   censyserrorSUCCESS,                 // The operation completed successfully.
   censyserrorFAILED,                  // There was a general failure.
   censyserrorFILE_IO_ERROR,           // There was a problem with file input/output.
   censyserrorINVALID_INSTANCE,        // The CENSYS_INSTANCE argument was invalid.
   censyserrorINVALID_IMAGE,           // The CENSYS_IMAGE_CONTENT argument was invalid.
   censyserrorINVALID_RESOLUTION,      // An invalid resolution has been submitted.
   censyserrorINVALID_FILENAME,        // An invalid filename was provided
   censyserrorINVALID_FRAME_COUNT,     // An invalid frame count was received.
   censyserrorINVALID_ARGUMENT,        // One of the given arguments was invalid.
   censyserrorNOT_IMPLEMENTED,	       // The given function was not implemented.
   censyserrorOUT_OF_MEMORY,           // There wasn't enough memory to complete 
		     		       //  an operation.
   censyserrorSYSTEM_NOT_CONFIGURED,   // The system is not configured for the given layer.
   censyserrorSYSTEM_NOT_CREATED,      // The system has yet to be created or has not
                                       //  been configured for the given processing layer.
   censyserrorSYSTEM_INIT_ERROR        // The system was not successfully initialized.

} CENSYS_ERROR;

//
// Name: CENSYS_IMAGE_CONTENT
//
// Description:
//  This enumeration lists the different images produced by
//  the *Censys3D* system.
//
typedef enum CENSYS_IMAGE_CONTENT
{
   censysImageContentSCENE,
   censysImageContentFOREGROUND,
   censysImageContentBACKGROUND,

} CENSYS_IMAGE_CONTENT;

//
// Name: CENSYS_DIAGNOSTIC_IMAGE_CONTENT
//
// Description:
//  This enumeration lists the different types of diagnostic images available from
//  the *Censys3D* system.
//
typedef enum CENSYS_DIAG_IMAGE_CONTENT
{
   censysDiagImageContentNONE,
   censysDiagImageContentGROUNDPLANE,           // Quality of ground plane calibration.
   censysDiagImageContentBACKGROUND_STABILITY,  // Stability of background depth values.
   censysDiagImageContentHEIGHTMAP,             // 3-D heightmap.

} CENSYS_DIAG_IMAGE_CONTENT;

//
// Name: CENSYS_IMAGE_TYPE
//
//  This enumeration defines the different types of images being returned by
//  the SEGMENTOR_DATA structure.
//
typedef enum CENSYS_IMAGE_TYPE
{
   censysImageTypeCOLOUR,
   censysImageTypeGREYSCALE,
   censysImageTypeDISPARITY,

} CENSYS_IMAGE_TYPE;

//
// Name: CENSYS_COMPRESSION_TYPE
//
//  This enumeration defines the different types of compression used in
//  the CENSYS_IMAGE structure.
//
typedef enum CENSYS_COMPRESSION_TYPE
{
   censysCompressionNONE,
   censysCompressionJPEG,

} CENSYS_COMPRESSION_TYPE;

//
// Name: CENSYS_DATA_TYPE
//
//  This enumeration defines the different types of data produced by
//  the Segmentor processing layer.
//
typedef enum CENSYS_DATA_TYPE
{
   censysDataSCENE_INTENSITY_IMAGE         =    0x10,
   censysDataSCENE_DISPARITY_IMAGE         =    0x20,
   censysDataFOREGROUND_INTENSITY_IMAGE    =   0x100,
   censysDataFOREGROUND_DISPARITY_IMAGE    =   0x200,
   censysDataFOREGROUND_POINTS             =   0x400,
   censysDataBACKGROUND_INTENSITY_IMAGE    =  0x1000,
   censysDataBACKGROUND_DISPARITY_IMAGE    =  0x2000,
   censysDataBACKGROUND_POINTS             =  0x4000,

} CENSYS_DATA_TYPE;

//
// Name: CENSYS_SYSTEM_CONFIG
//
// Description:
//  This enumeration lists the different *Censys3D* system level configuration 
//  codes.
//  
typedef int CENSYS_SYSTEM_CONFIG;
enum
{
   censysGRABBER     = 0,
   censysSEGMENTOR   = 1,
   censysEXTRACTOR   = 2,
   censysTRACKER     = 3
};

//=============================================================================
// Structure Definitions
//=============================================================================

// Group=Structure Definitions

//
// Name: CENSYS_INSTANCE
//
// Description:
//  This type represents an instance of a *Censys3D* system.  It is the handle
//  that the user should pass around when making calls to the SDK.
//
typedef void* CENSYS_INSTANCE;

//
// Name: CENSYS_TIMESTAMP
//
// Description:
//  This type represents a timestamp, where the time is expressed in
//  seconds since epoch and milliseconds. 
//
typedef struct CENSYS_TIMESTAMP
{
   unsigned int   time;     // Seconds since epoch
   unsigned short millitm;  // Milliseconds
#ifdef TI_DSP
   char padding[2];
#endif
} ALIGNED
CENSYS_TIMESTAMP;

//
// Name: POINT2D
//
// Description:
//  Represents a 2D point with coordinates (x,y)
//
typedef struct POINT2D
{
   double x;
   double y;

} ALIGNED
POINT2D, *PPOINT2D;

//
// Name: POINTRC
//
// Description:
//  Represents a point in image coordinates (row,column), where nCamera
//  is the index of the camera from which the image was obtained.
//
typedef struct POINTRC
{
   double row;
   double col;
   int camera;

} ALIGNED
POINTRC, *PPOINTRC;

//
// Name: COLOUR_POINT3D
//
// Description:
//  This structure represents the format of a 3D color point.
//
typedef struct COLOUR_POINT3D
{
   double   row;     // The image row coordinate of the point. 
   double   col;     // The image column coordinate of the point. 
   double   red;     // The red component of the point's color (0-255). 
   double   green;   // The green component of the point's color (0-255).
   double   blue;    // The blue component of the point's color (0-255).  
   double   x;       // The x component of the point's position. 
   double   y;       // The y component of the point's position. 
   double   z;       // The z component of the point's position. 

} ALIGNED
COLOUR_POINT3D, *PCOLOUR_POINT3D, **PPCOLOUR_POINT3D;

//
// Name: COLOUR_POINT3D_ARRAY
//
// Description:
//  This structure represents the format in which an array of COLOUR_POINT3D's
//  are stored.
//
typedef struct COLOUR_POINT3D_ARRAY
{
   int               size; // Point pointer array size. 
   PPCOLOUR_POINT3D  data; // Pointer to array of point pointers. 

} ALIGNED
COLOUR_POINT3D_ARRAY, *PCOLOUR_POINT3D_ARRAY;

//
// Name: REGION
//
// Description:
//  Represents a rectangular image region, used to set/reset regions of
//  interest for stereo processing.
//
typedef struct REGION
{
   int start_row; // Starting row
   int start_col; // Starting col
   int rows;      // Number of rows
   int columns;   // Number of columns

} ALIGNED
REGION, *PREGION;

//
// Name: CENSYS_IMAGE_DATA
//
// Description:
//  This structure represents the image data.
//
typedef struct CENSYS_IMAGE_DATA
{
   int               length;  // The size of the image data (bytes).
   unsigned char*    pBuffer; // A pointer to the start of the image data. 
} CENSYS_IMAGE_DATA, *PCENSYS_IMAGE_DATA;

//
// Name: CENSYS_IMAGE
//
// Description:
//  This structure represents an image returned by the segmentor layer.
//
typedef struct CENSYS_IMAGE
{
   CENSYS_IMAGE_TYPE type;    // The type of the image.
   int               width;   // The image width (cols).
   int               height;  // The image height (rows).
   int               Bpp;     // The image bytes per pixel.
   CENSYS_COMPRESSION_TYPE compression; // The type of compression used.
   CENSYS_IMAGE_DATA data;   // A structure containing the pointer to the start of the image data. 
                              
} ALIGNED
CENSYS_IMAGE, *PCENSYS_IMAGE;

//
// Name: CENSYS_STEREO_IMAGE
//
// Description:
//  This structure represents a stereo image produced by the grabber layer.
//  arData[] is an array of images that comprise the 2- or 3-channel stereo 
//  image, where:
//   arpData[0] - right camera image
//   arpData[1] - left camera image
//   arpData[2] - top camera image (if applicable)
//
typedef struct CENSYS_STEREO_IMAGE
{
   CENSYS_TIMESTAMP  timestamp;    // The timestamp associated with the image.
   int               width;        // The image width (cols).
   int               height;       // The image height (rows).
   CENSYS_COMPRESSION_TYPE compression; // The type of compression used.
   CENSYS_IMAGE_DATA arData[3];    // The array of images

} ALIGNED
CENSYS_STEREO_IMAGE, *PCENSYS_STEREO_IMAGE;

//
// Name: PATHPOINT
//
// Description:
//  This structure represents the format of a single point along a tracked 
//  person's path.
//
typedef struct PATHPOINT
{
   CENSYS_TIMESTAMP  timestamp;  // The timestamp of the image data from which
                                 // this data was derived.
   COLOUR_POINT3D    point3D;    // The 3D location of the point.

} ALIGNED
PATHPOINT, *PPATHPOINT;

//
// Name: PATH
//
// Description:
//  This structure represents the data format of the path of a tracked person.
//
typedef struct PATH
{
   int        size;            // The number of elements in pathpoint_array. 
   PPATHPOINT pathpoint_array; // The array of points.

} ALIGNED
PATH, *PPATH;

//
// Name: PERSON_ID
//
// Description:
//  This type represents a unique identification number associated with a 
//  tracked person.
//
typedef int PERSON_ID, *PPERSON_ID;

//
// Name: PERSON
//
// Description:
//  This structure defines the representation of a single tracked person.
//
typedef struct PERSON
{
   PERSON_ID nID;           // The person's unique identification number
   PATH      tracked_path;  // The path associated with the person.
   double    dHeight;       // The person's height (metres)
   POINTRC   heading;       // The person's heading in image coordinates
   POINT2D   velocity;      // The person's average velocity in x-y coordinates (m/sec)
} ALIGNED
PERSON, *PPERSON;

//
// Name: PEOPLE
//
// Description:
//  This structure represents a collection of tracked people
//
typedef struct PEOPLE
{
   int     size;             // The number of people represented by the array. 
   PPERSON person_array;     // Pointer to array of people.

} ALIGNED
PEOPLE, *PPEOPLE;

#ifdef CENSYS_FIXEDPOINT
#	include "censysfixeddefs.h"
#endif

//
// Name: GRABBER_DATA
//
// Description:
//  This structure defines the format in which data is returned from the 
//  Grabber layer.  Access to this functionality is not currently available.
//
typedef struct GRABBER_DATA
{
   CENSYS_TIMESTAMP  timestamp;              // The timestamp from the stereo image 
                                             // of camera 0.
   int               num_cameras;            // The number of elements in stereo_image_array.
   PCENSYS_STEREO_IMAGE stereo_image_array;  // Pointer to array of stereo images.

} ALIGNED
GRABBER_DATA, *PGRABBER_DATA;

//
// Name: SEGMENTOR_CAMERA_DATA
//
// Description:
//  This structure represents the format in which data associated with a single
//  camera is returned from the Segmentor layer.
//
typedef struct SEGMENTOR_CAMERA_DATA
{
   CENSYS_IMAGE   image_scene_disparity;        // The scene disparity image. 
   CENSYS_IMAGE   image_foreground_disparity;   // The foreground disparity image. 
   CENSYS_IMAGE   image_background_disparity;   // The background disparity image. 
   
   CENSYS_IMAGE   image_scene;                  // The scene image. 
   CENSYS_IMAGE   image_foreground;             // The foreground image. 
   CENSYS_IMAGE   image_background;             // The background image. 

   COLOUR_POINT3D_ARRAY foreground_point_array; // The foreground point array. 
   COLOUR_POINT3D_ARRAY background_point_array; // The background point array. 

} ALIGNED
SEGMENTOR_CAMERA_DATA, *PSEGMENTOR_CAMERA_DATA;

//
// Name: SEGMENTOR_DATA
//
// Description:
//  This structure defines the format of data is returned from the Segmentor layer.
//
typedef struct SEGMENTOR_DATA
{
   CENSYS_TIMESTAMP        timestamp;            // The timestamp of the image 
                                                 // data (camera 0) from which 
                                                 // this data was derived.
   int                     num_cameras;          // The number of elements in camera_data_array.
   PSEGMENTOR_CAMERA_DATA  camera_data_array;    // The array of scene data for all cameras.

   COLOUR_POINT3D_ARRAY foreground_point_array; // The foreground point array for all cameras. 
   COLOUR_POINT3D_ARRAY background_point_array; // The background point array for all cameras. 

} ALIGNED
SEGMENTOR_DATA, *PSEGMENTOR_DATA;

#ifndef EXTRACTOR_DATA_DEFINED

//
// Name: EXTRACTOR_DATA
//
// Description:
//  This structure defines the format of data is returned from the Extractor layer.
//
typedef struct EXTRACTOR_DATA
{
   CENSYS_TIMESTAMP    timestamp;   // The timestamp of the image data from which 
                                    // this data was derived.
   int                 size;        // The number of new people detected.
   PCOLOUR_POINT3D     point_array; // The array of locations where new people
                                    // were detected.
} ALIGNED
EXTRACTOR_DATA, *PEXTRACTOR_DATA;

#endif

#ifndef TRACKER_DATA_DEFINED

//
// Name: TRACKER_DATA
//
// Description:
//  This structure defines the format of data is returned from the Tracker layer.
//
typedef struct TRACKER_DATA
{
   CENSYS_TIMESTAMP timestamp;        // The timestamp of the image data from
                                      // which this data was derived.
   PEOPLE           tracked_people;   // All people currently being tracked.
   PEOPLE           tracked_people_removed; // People who are no longer being 
                                            // tracked as of the current frame.
} ALIGNED
TRACKER_DATA, *PTRACKER_DATA;

#endif

//
// Name: CENSYS_DIAGNOSTIC_DATA
//
// Description:
//  This structure defines the format of diagnostic data returned by the system.
//
typedef struct CENSYS_DIAGNOSTIC_DATA
{
   CENSYS_TIMESTAMP  timestamp;        // The timestamp of the image data from
                                       // which this data was derived.
   CENSYS_IMAGE      image;            // Diagnostic image.
   int               camera_index;     // Index of the camera from which the
                                       // diagnostic image was derived.
   CENSYS_DIAG_IMAGE_CONTENT image_content; // Content of the diagnostic image.

} ALIGNED
CENSYS_DIAGNOSTIC_DATA, *PCENSYS_DIAGNOSTIC_DATA;

#ifndef	AAA_FN_TYPES_H
#	include "fn_types.h"
#endif

#ifdef __cplusplus
extern "C"
{
#endif

// ============================================================================
// Callback Function Prototypes
// ============================================================================

// Group=Callback Function Prototypes

//
// Name: CENSYS_GRABBER_CALLBACK
//
// Description:
//  This is the function prototype for the Grabber layer callback.  When a 
//  callback of this type is registered, the Grabber layer will call the 
//  registered function once for every image that it receives from the camera
//  system.
//
// See Also:
//  censysSystemSetGrabberCallback() 
//
typedef void MAAAPIENTRY CENSYS_GRABBER_CALLBACK( PGRABBER_DATA pData, void* pArg );

//
// Name: CENSYS_SEGMENTOR_CALLBACK
//
// Description:
//  This is the function prototype for the Segmentor layer callback. When a
//  callback of this type is registered, the Segmentor layer will call the
//  registered function once for every image that it receives from the Grabber
//  layer.
//
// See Also:
//  censysSystemSetSegmentorCallback()
//
typedef void MAAAPIENTRY CENSYS_SEGMENTOR_CALLBACK( PSEGMENTOR_DATA pData, void* pArg );

//
// Name: CENSYS_EXTRACTOR_CALLBACK
//
// Description:
//  This is the function prototype for the Extractor layer callback.  When a 
//  callback of this type is registered, the Tracker layer will call the 
//  registered function once for every frame of data that it processes from
//  the Segmentor layer.
//
// See Also:
//  censysSystemSetExtractorCallback() 
//
typedef void MAAAPIENTRY CENSYS_EXTRACTOR_CALLBACK( PEXTRACTOR_DATA pData, void* pArg );

//
// Name: CENSYS_TRACKER_CALLBACK
//
// Description:
//  This is the function prototype for the Tracker layer callback.  When a 
//  callback of this type is registered, the Tracker layer will call the 
//  registered function once for every frame of data that it processes from the
//  Extractor layer.
//
// See Also:
//  censysSystemSetTrackerCallback() 
//  
typedef void MAAAPIENTRY CENSYS_TRACKER_CALLBACK( PTRACKER_DATA pData, void* pArg );

//
// Name: CENSYS_DIAGNOSTICS_CALLBACK
//
// Description:
//  This is the function prototype for the diagnostics callback.  When a 
//  callback of this type is registered, the registered function is called 
//  whenever system diagnostic information is updated.
//
// See Also:
//  censysSystemSetDiagnosticImageProps()
//  censysSystemGetDiagnosticImageProps()
//
typedef void MAAAPIENTRY CENSYS_DIAGNOSTICS_CALLBACK( PCENSYS_DIAGNOSTIC_DATA pData, void* pArg );

//
// Name: CENSYS_CALLBACK
//
// Description:
//  This is the function prototype for a callback function that is passed to
//  an asynchronous function; i.e., a function that returns right away, but
//  does not necessarily complete its task before it returns.
//
// See Also:
//  censysSystemOptimizeExposure()
//  censysSystemInitializeBackground()
//  
typedef void MAAAPIENTRY CENSYS_CALLBACK( CENSYS_ERROR ce, void* pArg );

//=============================================================================
// General Functions
//=============================================================================
//
// Group=General Functions
//

//
// Name: censysErrorToString()
//
// Description:
//  This function returns a description associated with the specified
//  error code.
//
// Arguments:
//  ce - [in] Censys3D error code
//
// Returns:
//  Null-terminated string that describes the specified error code.
//
CENSYS_API const char*  MAAAPIENTRY
censysErrorToString( CENSYS_ERROR ce );

//
// Name: censysGetLastError()
//
// Description:
//  This function returns a description for the last error that occurred.
//
// Returns:
//  Null-terminated string that describes the last error encountered.
//
CENSYS_API const char*  MAAAPIENTRY
censysGetLastError();

//
// Name: censysEnableLogFile()
//
// Description: 
//  This function enables or disables the output of errors and informational
//  messages to a log file, censys.log.
//
// Arguments:
//  bEnable - [in] whether or not to enable logging
//
CENSYS_API void MAAAPIENTRY                             // sr@20100409 - restored lost CENSYS_API
censysEnableLogFile( const bool bEnable );

//
// Name: censysSystemDisplayVersionInformation() 
//
// Description: 
//  This function pops up a modal dialog that displays all of the version 
//  information for the current library.
//
CENSYS_API CENSYS_ERROR
censysSystemDisplayVersionInformation();

//=============================================================================
// System Construction/Destruction
//=============================================================================

// Group=System Construction/Destruction

//
// Name: censysSystemCreateInstance() 
//
// Description:
//   This function creates an instance of a *Censys3D* system 
//   using the specified file to initialize system settings.
//
// Arguments: 
//  pInstance  - [out] pointer to the CENSYS_INSTANCE to be created
//  szFileName - [in]  name of the initialization file, or NULL to 
//                     initialize the sytem with default values for all settings
//
// Returns:
//  censyserrorOUT_OF_MEMORY - insufficient memory resources to create the
//   system
//
// See Also:
//  censysSystemDeleteInstance()
//  censysSystemSaveInstance()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSystemCreateInstance( CENSYS_INSTANCE *pInstance,
                            char            *szFileName );

//
// Name: censysSystemDeleteInstance() 
// 
// Description:
//  This function deletes an instance of a *Censys3D* system.  This function
//  should be the last call to the *Censys3D* SDK.  Calling this function
//  will help avoid memory leaks.
//
// Arguments:
//  ciInstance - [in] CENSYS_INSTANCE to be deleted
//
// See Also:
//  censysSystemCreateInstance()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSystemDeleteInstance( CENSYS_INSTANCE   ciInstance ); 

//
// Name: censysSystemSaveInstance() 
//
// Description:
//  This function saves all of the parameters associated with the given
//  *Censys3D* instance to the specified file, allowing the user to
//  restore the system's state at a later time.
//
// Arguments:
//  ciInstance - [in] CENSYS_INSTANCE to save
//  szFileName - [in] destination filename
//
// See Also:
//  censysSystemCreateInstance()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSystemSaveInstance( CENSYS_INSTANCE  ciInstance,
                          char            *szFileName );

//=============================================================================
// System Control Functions
//=============================================================================

// Group=System Control Functions

//
// Name: censysSystemSetSystemConfig() 
//
// Description:
//  This function sets the system configuration, specifying the level of
//  data processing to be performed.  This function should not be called when
//  the system has already been created.
//
// Arguments:
//  ciInstance - [in] CENSYS_INSTANCE of interest
//  config     - [in] system configuration
//
// See Also:
//  censysSystemGetSystemConfig()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSystemSetSystemConfig( CENSYS_INSTANCE      ciInstance,
                             CENSYS_SYSTEM_CONFIG config );

//
// Name: censysSystemGetSystemConfig() 
//
// Description:
//  This function retrieves the current system configuration, which specifies
//  the level of data processing to be performed.
//
// Arguments:
//  ciInstance - [in]  CENSYS_INSTANCE of interest
//  config     - [out] system configuration
//
// See Also:
//  censysSystemSetSystemConfig()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSystemGetSystemConfig( CENSYS_INSTANCE       ciInstance,
                             CENSYS_SYSTEM_CONFIG* config );

//
// Name: censysSystemProcessData() 
//
// Description:
//  This function processes a single image frame through the *Censys3D* system
//
// Arguments:
//  ciInstance - [in] CENSYS_INSTANCE of interest
//
// See Also:
//  censysSystemCreateInstance()
//  censysSystemReset()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSystemProcessData( CENSYS_INSTANCE   ciInstance );

//
// Name: censysSystemInitialize()
//
// Description:
//  This function initializes the *Censys3D* system using the current 
//  settings.
//
// Arguments:
//  ciInstance - [in] CENSYS_INSTANCE of interest
//
// See Also:
//  censysSystemCreateInstance()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSystemInitialize( CENSYS_INSTANCE   ciInstance );

//
// Name: censySystemSetResolution() 
//
// Description: 
//  This function sets the resolution of stereo processing performed by the
//  *Censys3D* system.  The lower the image resolution, the faster the system
//  will be capable of processing individual images.  The tradeoff however is
//  in the system's capability to track individuals.  Lower image resolution 
//  results in fewer data points and poorer tracking capabilities.
//
// Arguments:
//  ciInstance - [in] CENSYS_INSTANCE of interest
//  nRows      - [in] number of rows
//  nCols      - [in] number of columns
//
// Returns:
//  censyserrorINVALID_RESOLUTION - if an unhandled image resolution is
//				    submitted.
//
// See Also:
//   censysSystemGetResolution()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSystemSetResolution( CENSYS_INSTANCE   ciInstance,
                           int nRows,
                           int nCols );

//
// Name: censysSystemGetResolution() 
//
// Description:
//  This function gets the current resolution at which stereo processing is
//  performed by the *Censys3D* system.
//
// Arguments:
//  ciInstance - [in] CENSYS_INSTANCE of interest
//  nRows      - [in] number of rows
//  nCols      - [in] number of columns
//
// See Also:
//   censysSystemSetResolution()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSystemGetResolution( CENSYS_INSTANCE    ciInstance,
                           int *nRows,
                           int *nCols );

//
// Name: censysSystemSetMinMaxZDistance() 
//
// Description:
//  This function sets the range of distance values considered by the
//  *Censys3D* system, where the distance is measured along the z-axis
//  of the camera frame (not the world frame).  Increasing the minimum 
//  distance increases the speed of the stereo processing while also
//  reducing the spatial resolution of the resulting depth data.  There
//  is presently no performance penalty for setting maximum distance to
//  a large value.  
//
//  For those users familiar with the Triclops Stereo Vision SDK, this
//  function has a direct mapping to the triclopsSetDisparity functions.
// 
//  Example:
//   If the camera is pointed straight down, dMaxDistance
//   should be set to the distance between the camera and the floor,
//   and dMinDistance should be set to dMaxDistance - dMaxPersonHeight,
//   where dMaxPersonHeight is the maximum height of a person to be
//   detected.
//
// Arguments:
//  ciInstance   - [in] CENSYS_INSTANCE of interest
//  nCameraIndex - [in] index of the camera of interest
//  dMinDistance - [in] new minimum z distance (meters), or
//                      CENSYS_AUTO to have the value set automatically
//  dMaxDistance - [in] new maximum z distance (meters), or
//                      CENSYS_AUTO to have the value set automatically
//
// See Also:
//  censysSystemGetMinMaxZDistance()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSystemSetMinMaxZDistance( CENSYS_INSTANCE   ciInstance,
                                int               nCameraIndex,
                                double            dMinDistance,
                                double            dMaxDistance );

//
// Name: censysSystemGetMinMaxZDistance() 
//
// Description:
//  This function gets the range of distance values considered by the
//  *Censys3D* system, where the distance is measured along the z-axis
//  of the camera frame (not the world frame).
//
// Arguments:
//  ciInstance   - [in]  CENSYS_INSTANCE of interest
//  nCameraIndex - [in]  index of the camera of interest
//  dMinDistance - [out] current minimum z distance (meters)
//  dMaxDistance - [out] current maximum z distance (meters)
//
// See Also:
//  censysSystemSetMinMaxZDistance()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSystemGetMinMaxZDistance( CENSYS_INSTANCE   ciInstance,
                                int    nCameraIndex,
                                double *dMinDistance,
                                double *dMaxDistance );

//
// Name: censysSystemSetProcessingRegion()
//
// Description:
//  This function is used to turn on or turn off stereo processing
//  in a specified region.
//
// Arguments:
//  ciInstance   - [in] CENSYS_INSTANCE of interest
//  nCameraIndex - [in] index of the camera of interest
//  pRegion      - [in] region of the image in which stereo processing
//                      is to be turned on or off
//  bPerformProcessing - [in] a boolean flag; 1 to turn processing on,
//                            0 to turn processing off 
//
// Returns:
//  censyserrorSYSTEM_NOT_CREATED - system has not been created
//
// See Also:
//  censysSystemGetProcessingRegion()
//  censysSystemSetProcessingRegionsString()
//
// Note:
//  The *Censys3D* system must be created before calling this function.
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSystemSetProcessingRegion( CENSYS_INSTANCE ciInstance,
                                 int             nCameraIndex, 
                                 PREGION         pRegion,
                                 int             bPerformProcessing );

//
// Name: censysSystemGetProcessingRegion()
//
// Description:
//  This function is used to retrieve a processing region
//  defined for stereo processing.  
//
// Arguments:
//  ciInstance   - [in]  CENSYS_INSTANCE of interest
//  nCameraIndex - [in]  index of the camera of interest
//  nRegionIndex - [in]  index to the region to retrieve
//  pRegion      - [out] pointer to the region to retrieve
//
// Returns:
//  censyserrorSYSTEM_NOT_CREATED - system has not been created
//
// See Also:
//  censysSystemGetNumProcessingRegions()
//  censysSystemSetProcessingRegion()
//  censysSystemGetProcessingRegionsString()
//
// Note: 
//  The *Censys3D* system must be created before calling this function.
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSystemGetProcessingRegion( CENSYS_INSTANCE ciInstance,
                                 int             nCameraIndex,
                                 int             nRegionIndex,
                                 PREGION         pRegion );

//
// Name: censysSystemGetNumProcessingRegions()
//
// Description:
//  This function is used to retrieve the number of processing regions
//  defined for stereo processing.  
//
// Arguments:
//  ciInstance   - [in]  CENSYS_INSTANCE of interest
//  nCameraIndex - [in]  index of the camera of interest
//  nNumRegions  - [out] pointer to the number of regions
//
// Returns:
//  censyserrorSYSTEM_NOT_CREATED - system has not been created
//
// See Also:
//  censysSystemGetProcessingRegion()
//
// Note: 
//  The *Censys3D* system must be created before calling this function.
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSystemGetNumProcessingRegions( CENSYS_INSTANCE ciInstance,
                                     int             nCameraIndex,
                                     int            *nNumRegions );

//
// Name: censysSystemSetProcessingRegionsString()
//
// Description:
//  This function is used to set the processing regions from a specially
//  formatted string.  These regions define where stereo processing is to be 
//  performed.  Format of szRegions:
//  "<camera index>{bounding region}{region1}{region2}..."
//  where each region is defined by 
//  "{start_row (top=0), start_col (left=0), height, width}",
//   or "auto".
//
// Arguments:
//  ciInstance - [in] CENSYS_INSTANCE of interest
//  szRegions  - [in] string describing the regions of the image in which
//                    stereo processing is to be performed, or 
//                    "auto" for the value to be set automatically
//
// See Also:
//  censysSystemGetProcessingRegionsString()
//  censysSystemSetProcessingRegion()
//  censysSystemGetProcessingRegion()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSystemSetProcessingRegionsString( CENSYS_INSTANCE   ciInstance,
                                        char             *szRegions );

//
// Name: censysSystemGetProcessingRegionsString()
//
// Description:
//  This function is used to retrieve the processing regions in string
//  format.  These regions define where stereo processing is to be performed.
//
// Arguments:
//  ciInstance   - [in]  CENSYS_INSTANCE of interest
//  szRegions    - [out] string describing the regions of the image in which
//                       stereo processing is to be performed.
//  nMaxLen      - [in]  maximum size of buffer pointed to by szRegions
//
// See Also:
//  censysSystemSetProcessingRegionsString()
//  censysSystemGetProcessingRegion()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSystemGetProcessingRegionsString( CENSYS_INSTANCE   ciInstance,
                                        char             *szRegions,
                                        int               nMaxLen );

//
// Name: censysSystemInitializeBackground()
//
// Description:
//  This function attempts to initialize the depth values of the background
//  by successively varying the camera exposure settings over a series of
//  image frames.  As such, this function is asynchronous; i.e., it returns
//  immediately and subsequently calls a callback function (if specified)
//  upon completion.  This function will fail if the system is configured to
//  grab from files.
//
//  The *Censys3D* system must be created before calling this function.
//
// Arguments:
//  ciInstance   - [in] CENSYS_INSTANCE of interest
//  pCallbackFn  - [in] callback function to call upon completion, or NULL
//  pCallbackArg - [in] argument passed through to the callback function, or NULL
//
// Returns:
//  censyserrorSYSTEM_NOT_CREATED - system has not been created
//
// See Also:
//  censysSystemOptimizeExposure()
//
// Note: 
//  The *Censys3D* system must be created before calling this function.
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSystemInitializeBackground( CENSYS_INSTANCE   ciInstance,
                                  CENSYS_CALLBACK* pCallbackFn,
                                  void* pCallbackArg );

//
// Name: censysSystemOptimizeExposure()
//
// Description:
//  This function attempts to optimize the quality of the depth data for the
//  pixels enclosed by the ground plane polygon by successively varying the
//  camera exposure settings over a series of image frames.  As such, this
//  function is asynchronous; i.e., it returns immediately and subsequently
//  calls a callback function (if specified) upon completion.  This function
//  will fail if the system is configured to grab from files.
//
// Arguments:
//  ciInstance   - [in] CENSYS_INSTANCE of interest
//  pCallbackFn  - [in] callback function to call upon completion, or NULL.
//  pCallbackArg - [in] argument passed through to the callback function, or NULL.
//
// Returns:
//  censyserrorSYSTEM_NOT_CREATED - system has not been created
//
// See Also:
//  censysSystemInitializeBackground()
//  censysSegmentorSetTransformationFromScene()
//
// Note: 
//  The *Censys3D* system must be created before calling this function.
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSystemOptimizeExposure( CENSYS_INSTANCE   ciInstance,
                              CENSYS_CALLBACK* pCallbackFn,
                              void* pCallbackArg );

//
// Name: censysSystemXYZToRowCol() 
//
// Description:
//  This function projects a point in 3-D space into image space.  The 
//  resulting image coordinates (row,col) can then be used, for example, 
//  to index an individual pixel in images exported in the SEGMENTOR_DATA 
//  structure.  This allows the user to identify the image data which
//  corresponds to an (x,y,z) point.
//
// Arguments:
//  ciInstance   - [in]  CENSYS_INSTANCE of interest
//  nCameraIndex - [in]  index of the camera of interest
//  dx           - [in]  x-coordinate of the point (meters)
//  dy           - [in]  y-coordinate of the point (meters)
//  dz           - [in]  z-coordinate of the point (meters)
//  pRow         - [out] pointer to the row coordinate
//  pCol         - [out] pointer to column coordinate
//
// See Also:
//  censysSystemSceneRowColToXYZ()
//  censysSystemBackgroundRowColToXYZ()
//  censysSystemRowColToGroundPlaneXY()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSystemXYZToRowCol( CENSYS_INSTANCE  ciInstance,
                         int              nCameraIndex,
                         double           dx,
                         double           dy,
                         double           dz,
                         double          *pRow,
                         double          *pCol );

//
// Name: censysSystemBackgroundRowColToXYZ() 
//
// Description:
//  This function uses the depth values from the background image to
//  project a point from image space to 3-D space.
//
// Arguments:
//  ciInstance   - [in] CENSYS_INSTANCE of interest
//  nCameraIndex - [in] index of the camera of interest
//  dRow         - [in] row coordinate of the point in image space
//  dCol         - [in] column coordinate of the point in image space
//  dx           - [out] pointer to the x-coordinate of the point (meters)
//  dy           - [out] pointer to the y-coordinate of the point (meters)
//  dz           - [out] pointer to the z-coordinate of the point (meters)
//
// See Also:
//  censysSystemXYZToRowCol()
//  censysSystemSceneRowColToXYZ()
//  censysSystemRowColToGroundPlaneXY()
//
CENSYS_API CENSYS_ERROR  MAAAPIENTRY
censysSystemBackgroundRowColToXYZ( CENSYS_INSTANCE  ciInstance,
                                   int nCameraIndex,
                                   double dRow,
                                   double dCol,
                                   double *dx,
                                   double *dy,
                                   double *dz );

//
// Name: censysSystemSceneRowColToXYZ() 
//
// Description:
//  This function uses the depth values from the scene image to
//  project a point from image space to 3-D space.
//
// Arguments:
//  ciInstance   - [in]  CENSYS_INSTANCE of interest
//  nCameraIndex - [in]  index of the camera of interest
//  dRow         - [in]  row coordinate of the point in image space
//  dCol         - [in]  column coordinate of the point in image space
//  dx           - [out] pointer to the x-coordinate of the point (meters)
//  dy           - [out] pointer to the y-coordinate of the point (meters)
//  dz           - [out] pointer to the z-coordinate of the point (meters)
//
// See Also:
//  censysSystemXYZToRowCol()
//  censysSystemBackgroundRowColToXYZ()
//  censysSystemRowColToGroundPlaneXY()
//
CENSYS_API CENSYS_ERROR  MAAAPIENTRY
censysSystemSceneRowColToXYZ( CENSYS_INSTANCE  ciInstance,
                              int nCameraIndex,
                              double dRow,
                              double dCol,
                              double *dx,
                              double *dy,
                              double *dz );

//
// Name: censysSystemRowColToGroundPlaneXY() 
//
// Description:
//  This function uses the position and orientation of the ground plane
//  to project a point from image space to 3-D space, where the resulting 
//  point (x,y) lies on the ground plane (z=0).
//
// Arguments:
//  ciInstance   - [in]  CENSYS_INSTANCE of interest
//  nCameraIndex - [in]  index of the camera of interest
//  dRow         - [in]  row coordinate of the point in image space
//  dCol         - [in]  column coordinate of the point in image space
//  dx           - [out] pointer to the x-coordinate of the point (meters)
//  dy           - [out] pointer to the y-coordinate of the point (meters)
//
// See Also:
//  censysSystemXYZToRowCol()
//
CENSYS_API CENSYS_ERROR  MAAAPIENTRY
censysSystemRowColToGroundPlaneXY( CENSYS_INSTANCE  ciInstance,
                                   int nCameraIndex,
                                   double dRow,
                                   double dCol,
                                   double *dx,
                                   double *dy );

//
// Name: censysSystemSetDiagnosticImageProps()
//
// Description:
//  This function selects a diagnostic image to be exported.  This image can
//  be accessed by setting a callback function (see
//  censysSystemSetDiagnosticsCallback()).  If imageContent is set to 
//  censysDiagImageContentNONE or censysDiagImageContentHEIGHTMAP, the 
//  nCameraIndex parameter is ignored.
//
// Arguments:
//  ciInstance   - [in] CENSYS_INSTANCE of interest
//  nCameraIndex - [in] index of the camera (if applicable)
//  imageContent - [in] content of the image to be selected
//
// See Also:
//  censysSystemGetDiagnosticImageProps()
//  censysSystemSetDiagnosticsCallback()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSystemSetDiagnosticImageProps( CENSYS_INSTANCE ciInstance,
                                     int             nCameraIndex,
                                     CENSYS_DIAG_IMAGE_CONTENT imageContent );
  
//
// Name: censysSystemGetDiagnosticImageProps()
//
// Description:
//  This function retrieves the attributes of the diagnostic image currently
//  selected for export.
//
// Arguments:
//  ciInstance   - [in]  CENSYS_INSTANCE of interest
//  nCameraIndex - [out] pointer to the index of the camera 
//  imageContent - [out] pointer to the content of the image
//
// See Also:
//  censysSystemSetDiagnosticImageProps()
//  censysSystemSetDiagnosticsCallback()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSystemGetDiagnosticImageProps( CENSYS_INSTANCE ciInstance,
                                     int*            nCameraIndex,
                                     CENSYS_DIAG_IMAGE_CONTENT* imageContent );

//=============================================================================
// Callback Setting Functions
//=============================================================================

// Group=Callback Setting Functions

//
// Name: censysSystemSetGrabberCallback() 
//
// Description:
//  This function allows the user to set the callback function for the Grabber 
//  layer.  The supplied function will be called every time the Grabber layer
//  finishes processing an image frame from the device.
//
// Arguments:
//  ciInstance          - [in] CENSYS_INSTANCE of interest
//  cgcCallbackFunction - [in] function to be called when data is available.
//  pArg                - [in] optional argument to be passed to the callback
//                             function (set to NULL if not used)
//
// Returns:
//  censyserrorSYSTEM_NOT_CONFIGURED - the system is not configured
//			               to support this layer.  
//           
// Note: 
//  This function is not yet fully functional for this layer.
//
// See Also:
//  CENSYS_GRABBER_CALLBACK
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSystemSetGrabberCallback( CENSYS_INSTANCE         ciInstance,
                                CENSYS_GRABBER_CALLBACK cgcCallbackFunction,
                                void*                   pArg );

//
// Name: censysSystemSetSegmentorCallback() 
//
// Description:
//  This function allows the user to set the callback function for the 
//  Segmentor layer.  The supplied function will be called every time the 
//  Segmentor layer finishes processing a frame of data from the Grabber 
//  layer.
//
// Arguments:
//  ciInstance          - [in] CENSYS_INSTANCE of interest
//  cscCallbackFunction - [in] function to be called when data is available.
//  pArg                - [in] optional argument to be passed to the callback
//                             function (set to NULL if not used)
//
// Returns:
//  censyserrorSYSTEM_NOT_CONFIGURED - the system is not configured
//                                     to support this level.  
//
// See Also:
//  CENSYS_SEGMENTOR_CALLBACK
//           
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSystemSetSegmentorCallback( CENSYS_INSTANCE           ciInstance,
                                  CENSYS_SEGMENTOR_CALLBACK cscCallbackFunction,
                                  void*                     pArg );

//
// Name: censysSystemSetExtractorCallback() 
//
// Description: 
//  This function allows the user to set the callback function for the 
//  Extractor layer.  The supplied function will be called every time the
//  Extractor layer finishes processing a frame of data from the Segmentor 
//  layer.
//
// Arguments:
//  ciInstance          - [in] CENSYS_INSTANCE of interest
//  cecCallbackFunction - [in] function to be called when data is available.
//  pArg                - [in] optional argument to be passed to the callback
//                             function (set to NULL if not used)
//
// Returns:
//  censyserrorSYSTEM_NOT_CONFIGURED - the system is not configured
//                                     to support this level.  
//
// See Also:
//  CENSYS_EXTRACTOR_CALLBACK
//           
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSystemSetExtractorCallback( CENSYS_INSTANCE           ciInstance,
                                  CENSYS_EXTRACTOR_CALLBACK cecCallbackFunction,
                                  void*                     pArg );

//
// Name: censysSystemSetTrackerCallback() 
//
// Description: 
//  This function allows the user to set the callback function for the
//  Tracker layer.  The supplied function will be called every time the 
//  Tracker layer finishes processing a frame of data from the Segmentor layer.
//
//
// Arguments:
//  ciInstance          - [in] CENSYS_INSTANCE of interest
//  ctcCallbackFunction - [in] function to be called when data is available.
//  pArg                - [in] optional argument to be passed to the callback
//                             function (set to NULL if not used)
//
// Returns:
//  censyserrorSYSTEM_NOT_CONFIGURED - the system is not configured
//                                     to support this layer.  
//
// See Also:
//  CENSYS_TRACKER_CALLBACK
//           
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSystemSetTrackerCallback( CENSYS_INSTANCE         ciInstance,
                                CENSYS_TRACKER_CALLBACK ctcCallbackFunction,
                                void*                   pArg );

//
// Name: censysSystemSetDiagnosticsCallback() 
//
// Description: 
//  This function allows the user to set the callback function for diagnostics.
//  The supplied function will be called every time the system diagnostic
//  information is updated.
//
// Arguments:
//  ciInstance          - [in] CENSYS_INSTANCE of interest
//  cdcCallbackFunction - [in] function to be called when data is available.
//  pArg                - [in] optional argument to be passed to the callback
//                             function (set to NULL if not used)
//
// Returns:
//  censyserrorSYSTEM_NOT_CONFIGURED - the system is not configured
//                                     to support this layer.  
//
// See Also:
//  CENSYS_DIAGNOSTICS_CALLBACK
//           
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSystemSetDiagnosticsCallback( CENSYS_INSTANCE             ciInstance,
                                    CENSYS_DIAGNOSTICS_CALLBACK cdcCallbackFunction,
                                    void*                       pArg );

//=============================================================================
// Grabber Layer Functions
//=============================================================================
//
// Group=Grabber Layer Functions
//

//
// Name: censysGrabberGetNumberOfDevices()
// 
// Description:
//  This function retrieves the number of grabbing devices that have been
//  initialized.
//
// Arguments:
//  ciInstance  - [in]  CENSYS_INSTANCE of interest
//  pNumDevices - [out] pointer to the number of grabbing devices
//
// Returns: 
//  censyserrorSYSTEM_NOT_CREATED - the system has not been created.
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysGrabberGetNumberOfDevices( CENSYS_INSTANCE    ciInstance,
                                 int                *pNumDevices );

//
// Name: censysGrabberGetDeviceSerialNumber()
// 
// Description:
//  This function retrieves the serial number of a single grabbing device
//  that has been initialized.
//
// Arguments:
//  ciInstance   - [in]  CENSYS_INSTANCE of interest
//  nDeviceIndex - [in]  index of the device in question
//  pSerialNum   - [out] pointer to the serial number of the device
//
// Returns: 
//  censyserrorSYSTEM_NOT_CREATED - the system has not been created
//
// See Also:
//  censysGrabberGetNumberOfDevices()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysGrabberGetDeviceSerialNumber( CENSYS_INSTANCE    ciInstance,
                                    int                nDeviceIndex,
                                    unsigned long      *pSerialNum );

//
// Name: censysGrabberSetDeviceSerialNumbersString()
// 
// Description:
//  This function sets the serial numbers of the device(s) to be
//  initialized for grabbing.  This function can be called prior to system
//  initialization to identify specific cameras to be initialized for grabbing
//  images.  If this function is not called, or is called with "auto", all
//  cameras detected on the bus will be used.  Format of szSerialNums:
//  "<camera_index>serial_no..." or "auto".
//
// Arguments:
//  ciInstance   - [in] CENSYS_INSTANCE of interest
//  szSerialNums - [in] string containing the device serial number(s), or
//                      "auto" for the value to be set automatically
//
// See Also:
//  censysGrabberGetDeviceSerialNumber()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysGrabberSetDeviceSerialNumbersString( CENSYS_INSTANCE ciInstance,
                                           char *szSerialNums );

//
// Name: censysGrabberGetDigiclopsContext()
// 
// Description:
//  This function retrieves the Digiclops context of the specified
//  grabbing device that has already been initialized.
//
// Arguments:
//  ciInstance   - [in]  CENSYS_INSTANCE of interest
//  nDeviceIndex - [in]  index of the device in question
//  pContext     - [out] pointer to the Digiclops context of the device
//
// Returns: 
//  censyserrorSYSTEM_NOT_CREATED - the system has not been created
//
// See Also:
//  censysGrabberGetNumberOfDevices()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysGrabberGetDigiclopsContext( CENSYS_INSTANCE    ciInstance,
                                  int                nDeviceIndex,
                                  DigiclopsContext   *pContext );

//
// Name: censysGrabberSetGrabFromFilesParams()
//
// Description:
//  This function sets the parameters to be used by the Grabber layer
//  to grab images from Portable Pixelmap (PPM) files.
//  In order for this function to have any effect, it must be called before
//  calling censysSystemCreateInstance() or censysSystemProcess().
//  The filename of each image file consists of a basename followed by the
//  image index, represented as a fixed-length numeric field, then followed by
//  ".ppm".  The length of the numeric field is the minimum number of digits
//  needed to represent the given image sequence; zeros are padded as needed.
//
// Arguments:
//  ciInstance    - [in] CENSYS_INSTANCE of interest
//  nDeviceIndex  - [in] index to the device in question 
//                       (presently only 0 is supported)
//  pTriclopsContextFileName  - [in] name of the triclops context file
//  pBaseFileName - [in] base filename for the images
//  nStartImage   - [in] image to start with
//  nFinishImage  - [in] image to finish on
//  nIncrement    - [in] value by which the image counter is to be incremented
//  bWrapAround   - [in] whether or not to repeat grabbing the image sequence
//                       once it has completed
//
// Example:
//   For files c:\image0000.ppm to c:\image0010.ppm:
//
//            pBaseFileName: "c:\\image00"
//            nStartImage:    0
//            nFinishImage:  10
//            nIncrement:     1
//
// Note: 
//   Portable Pixelmap files are a standard image file format that can be
//   viewed using Jasc's Paint Shop Pro.  An evaluation version can
//   be downloaded from their web site.
//
// See Also:
//  censysGrabberGetGrabFromFilesParams()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysGrabberSetGrabFromFilesParams( CENSYS_INSTANCE  ciInstance,
                                     int              nDeviceIndex,
                                     char            *pTriclopsContextFileName,
                                     char            *pBaseFileName, 
                                     int              nStartImage, 
                                     int              nFinishImage,
                                     int              nIncrement,
                                     bool             bWrapAround );

//
// Name: censysGrabberGetGrabFromFilesParams()
//
// Description:
//  This function retrieves the parameters to be used by the Grabber layer
//  to grab images from Portable Pixelmap (PPM) files.
//
// Arguments:
//  ciInstance      - [in]  CENSYS_INSTANCE of interest
//  nDeviceIndex    - [in]  index to the device in question 
//                          (presently only 0 is supported)
//  pTriclopsContextFileName  - [out] name of the triclops context file, or NULL 
//                                    if not needed
//  pBaseFileName   - [out] base filename for the images, or NULL if not needed
//  nMaxFileNameLen - [in]  maximum size of the string buffers pointed to by
//                          pTriclopsContextFileName and pBaseFileName
//  nStartImage     - [out] pointer to the image to start with, or NULL if not needed
//  nFinishImage    - [out] pointer to the image to finish on, or NULL if not needed
//  nIncrement      - [out] pointer to the value by which the image counter is
//                          to be incremented, or NULL if not needed
//  bWrapAround     - [out] pointer to whether or not grabbing should repeat 
//                          grabbing the image sequence once it completes
//
// See Also:
//  censysGrabberSetGrabFromFilesParams()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysGrabberGetGrabFromFilesParams( CENSYS_INSTANCE  ciInstance,
                                     int              nDeviceIndex,
                                     char            *pTriclopsContextFileName,
                                     char            *pBaseFileName, 
                                     int              nMaxFileNameLen,
                                     int             *nStartImage, 
                                     int             *nFinishImage,
                                     int             *nIncrement,
                                     bool            *bWrapAround );

//
// Name: censysGrabberGetGrabFromFiles()
//
// Description:
//  This function retrieves whether or not the Grabber layer is to grab
//  images from Portable Pixelmap (PPM) files.
//
// Arguments:
//  ciInstance     - [in] CENSYS_INSTANCE of interest
//  bGrabFromFiles - [out] pointer to whether or not images are to be 
//                         grabbed from files
//
// See Also:
//  censysGrabberSetGrabFromFiles()
//  censysGrabberGetGrabFromFilesParams()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysGrabberGetGrabFromFiles( CENSYS_INSTANCE  ciInstance,
                               bool            *bGrabFromFiles );


//
// Name: censysGrabberSetGrabFromFiles()
//
// Description:
//  This function sets whether or not the Grabber layer is to grab
//  images from Portable Pixelmap (PPM) files.
//
// Arguments:
//  ciInstance     - [in] CENSYS_INSTANCE of interest
//  bGrabFromFiles - [in] whether or not to grab from files
//
// See Also:
//  censysGrabberGetGrabFromFiles()
//  censysGrabberSetGrabFromFilesParams()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysGrabberSetGrabFromFiles( CENSYS_INSTANCE  ciInstance,
                               bool             bGrabFromFiles );

//
// Name: censysGrabberGetImageIndex()
//
// Description:
//  This function retrieves the index of the last image that was grabbed from
//  file.
//
// Arguments:
//  ciInstance  - [in]  CENSYS_INSTANCE of interest
//  nImageIndex - [out] pointer to the index of the last image
//
// Returns: 
//  censyserrorSYSTEM_NOT_CREATED - the system has not been created
//
// See Also:
//  censysGrabberGetGrabFromFilesParams()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysGrabberGetImageIndex( CENSYS_INSTANCE  ciInstance,
                            int             *nImageIndex );

//=============================================================================
// Scene Segmentor Layer Functions
//=============================================================================
//
// Group=Scene Segmentor Layer Functions
//

//
// Name: censysSegmentorResetBackground() 
//
// Description: 
//  This function resets the Segmentor background.
//
// Arguments:
//  ciInstance - [in] CENSYS_INSTANCE of interest
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSegmentorResetBackground( CENSYS_INSTANCE   ciInstance );

//
// Name: censysSegmentorStoreCurrentImage()
//
// Description:
//  This function allows the user to store the specified current image to disk
//  as a Portable Pixelmap (.ppm).  This can be useful when attempting to replay
//  trajectory data at a later time.
//
// Arguments:
//  ciInstance   - [in] CENSYS_INSTANCE of interest
//  nCameraIndex - [in] index of the camera of interest
//  imageContent - [in] content of the image to be stored
//  imageType    - [in] type of the image to be stored
//  szFileName   - [in] name of the image file to be created
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSegmentorStoreCurrentImage( CENSYS_INSTANCE      ciInstance,
                                  int                  nCameraIndex,
                                  CENSYS_IMAGE_CONTENT imageContent,
                                  CENSYS_IMAGE_TYPE    imageType,
                                  char                 *szFileName );

//
// Name: censysSegmentorGetDepthThreshold() 
//
// Description: 
//  This function retrieves the difference threshold between 
//  the foreground and background in the Segmentor.  That is, how much 
//  taller a foreground element has to be before it is no longer 
//  considered part of the background.
//
// Arguments:
//  ciInstance - [in]  CENSYS_INSTANCE of interest
//  pDistance  - [out] pointer to the current depth threshold (meters)
//
// See Also:
//  censysSegmentorSetDepthThreshold() 
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSegmentorGetDepthThreshold( CENSYS_INSTANCE  ciInstance,
                                  double          *pDistance );

//
// Name: censysSegmentorSetDepthThreshold() 
//
// Description: 
//  This function sets the difference threshold between 
//  the foreground and background in the segmentor.  That is, how much 
//  taller a foreground pixel has to be before it is no longer 
//  considered part of the background.
//
// Arguments:
//  ciInstance - [in] CENSYS_INSTANCE of interest
//  dDistance  - [in] new depth threshold (meters)
//
// See Also:
//  censysSegmentorGetDepthThreshold() 
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSegmentorSetDepthThreshold( CENSYS_INSTANCE ciInstance,
                                  double          dDistance );

//
// Name:  censysSegmentorGetCreepTime() 
//
// Description:
//  This function gets the time that it takes for foreground pixels to be 
//  integrated into the background.
//
// Arguments:
//  ciInstance - [in]  CENSYS_INSTANCE of interest
//  pCreepTime - [out] pointer to the current creep time
//
// See Also:
//  censysSegmentorSetCreepTime()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSegmentorGetCreepTime( CENSYS_INSTANCE  ciInstance,
                             double          *pCreepTime );


//
// Name: censysSegmentorSetCreepTime() 
//
// Description: 
//  This function sets the time that it takes for foreground pixels to be 
//  integrated into the background.  Set creep time to zero for "never".
// 
// Arguments:
//  ciInstance - [in] CENSYS_INSTANCE of interest
//  dCreepTime - [in] creep time
//
// See Also:
//  censysSegmentorGetCreepTime()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSegmentorSetCreepTime( CENSYS_INSTANCE ciInstance,
                             double          dCreepTime );

//
// Name: censysSegmentorGetTransformation() 
//
// Description:
//  This function gets the transformation between the 
//  Digiclops reference frame and the desired reference frame.
//
// Arguments:
//  ciInstance - [in]  CENSYS_INSTANCE of interest
//  dPositionX - [out] pointer to the X translation (meters)
//  dPositionY - [out] pointer to the Y translation (meters)
//  dPositionZ - [out] pointer to the Z translation (meters)
//  dRotationX - [out] pointer to the X rotation (radians)
//  dRotationY - [out] pointer to the Y rotation (radians)
//  dRotationZ - [out] pointer to the Z rotation (radians)
//
// See Also:
//  censysSegmentorSetTransformation()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSegmentorGetTransformation( CENSYS_INSTANCE   ciInstance,
                                  double *dPositionX,
                                  double *dPositionY,
                                  double *dPositionZ,
                                  double *dRotationX,
                                  double *dRotationY,
                                  double *dRotationZ );

//
// Name: censysSegmentorSetTransformation() 
//
// Description:
//  This function sets the transformation used to transform a 3-D point from 
//  the camera coordinate frame to the world coordinate frame.  
//  (dPositionX, dPositionY, dPositionZ) specify the 3-D location of the camera
//  with respect to the origin of the world frame, and
//  (dRotationX, dRotationY, dRotationZ) specify its orientation.
//  PositionZ is the height of the camera above the ground plane.  PositionX
//  and PositionY are usually set such that the camera is centred above the
//  volume of interest (VOI).
//
// Arguments:
//  ciInstance - [in] CENSYS_INSTANCE of interest
//  dPositionX - [in] X translation (meters), or
//                    CENSYS_AUTO to have the value set automatically
//  dPositionY - [in] Y translation (meters), or
//                    CENSYS_AUTO to have the value set automatically
//  dPositionZ - [in] Z translation (meters)
//  dRotationX - [in] X rotation (radians)
//  dRotationY - [in] Y rotation (radians)
//  dRotationZ - [in] Z rotation (radians), or
//                    CENSYS_AUTO to have the value set automatically
//
//  Example:
//   If the camera is oriented facing straight down, 3 meters above
//   the ground, and a simple cube-shaped VOI is used:
//    VOIPolygon=(0,0)(10,0)(10,10)(0,10)
//    VOIZ=2
//    VOITide=0.5
//   the transformation would be:
//    dPositionX=5.0
//    dPositionY=5.0
//    dPositionZ=3.0       (camera's origin is 3 m up from the ground)
//    dRotationX=3.1415926 (rotate z-axis to point up)
//    dRotationY=0.0
//    dRotationZ=0.0
//
//  Rotation angles can be interpreted according to the Rxyz fixed
//  angle set convention or the equivalent Rz'y'x Euler angle set convention,
//  as described by Craig (John J. Craig, Introduction to Robotics, 2nd ed.,
//  Addison-Wesley, 1989, pp. 442-443).  Both of these interpretations yield
//  the same transformation.  If interpreted according to the Rxyz fixed 
//  angle set convention, with the camera's coordinate frame initially coincident
//  with the world frame (z pointing up from the ground), rotations to the camera
//  are performed in the following order:
//   - rotation by dRotationX about x-axis
//   - rotation by dRotationY about fixed (world) y-axis
//   - rotation by dRotationZ about fixed (world) z-axis
//  Using the alternative interpretation following the Rz'y'x' Euler angle set 
//  convention, rotations to the camera are performed in the following order:
//   - rotation by dRotationZ about z-axis
//   - rotation by dRotationY about the rotated y-axis
//   - rotation by dRotationX about the rotated x-axis
//
// See Also:
//  censysSegmentorSetTransformationFromScene()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSegmentorSetTransformation( CENSYS_INSTANCE   ciInstance,
                                  double dPositionX,
                                  double dPositionY,
                                  double dPositionZ,
                                  double dRotationX,
                                  double dRotationY,
                                  double dRotationZ );

//
// Name: censysSegmentorSetTransformationFromScene()
//
// Description:
//  This function sets the transformation used to transform a 3-D point from 
//  the reference camera coordinate frame to the world coordinate frame.  
//  It does this by fitting a ground plane to the 3-D points derived from
//  the pixels that lie within the ground plane polygon.
//
//  Setting bAutoGrowPolygon to true causes this function to iteratively 
//  stretch the ground plane polygon of each camera in an attempt to encompass
//  more ground plane points to achieve a more accurate plane fit.  The growing
//  process stops once no further improvement in the standard deviation of the
//  plane fit can be achieved.
//
// Arguments:
//  ciInstance       - [in]  CENSYS_INSTANCE of interest
//  bAutoGrowPolygon - [in]  whether or not to automatically adjust the size of
//                           the ground plane polygon of each camera
//  dStdDev          - [out] pointer to the standard deviation of the distance
//                           from points to the ground plane
//  nNumPts          - [out] pointer to the total number of points used to fit plane
//  nOutliers        - [out] pointer to the number of points rejected as outliers
//
// Returns:
//  censyserrorSYSTEM_NOT_CREATED - if the system has not been created.
//
// See Also:
//  censysSegmentorSetTransformation()
//  censysSegmentorSetGroundPlanePolygon()
//  censysSystemOptimizeExposure()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSegmentorSetTransformationFromScene( CENSYS_INSTANCE   ciInstance,
                                           bool bAutoGrowPolygon,
                                           double* dStdDev,
                                           int* nNumPts,
                                           int* nOutliers );

//
// Name: censysSegmentorSetGroundPlanePolygon()
//
// Description:
//  This function sets the ground plane polygon that is used to calibrate 
//  the height and orientation of the camera with respect to the ground.
//
// Arguments:
//  ciInstance   - [in] CENSYS_INSTANCE of interest
//  nCameraIndex - [in] index of the camera of interest
//  pVertices    - [in] vertices of the polygon that encloses ground plane pixels;
//                      vertices are expressed in image coordinates
//  nNumVertices - [in] number of vertices
//
// Returns:
//  censyserrorSYSTEM_NOT_CREATED - if the system has not been created.
//
// See Also:
//  censysSegmentorSetTransformationFromScene()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSegmentorSetGroundPlanePolygon( CENSYS_INSTANCE   ciInstance,
                                      const int nCameraIndex,
                                      const PPOINT2D pVertices,
                                      const int nNumVertices );

//
// Name: censysSegmentorGetGroundPlanePolygonNumVertices()
//
// Description:
//  This function retrieves the number of vertices in the ground plane
//  polygon that is used to calibrate the height and orientation of the
//  camera with respect to the ground.
//
// Arguments:
//  ciInstance   - [in]  CENSYS_INSTANCE of interest
//  nCameraIndex - [in]  index of the camera in question
//  nNumVertices - [out] pointer to the number of vertices
//
// Returns:
//  censyserrorSYSTEM_NOT_CREATED - if the system has not been created.
//
// See Also:
//  censysSegmentorGetGroundPlanePolygon()
//  censysSegmentorSetGroundPlanePolygon()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSegmentorGetGroundPlanePolygonNumVertices( 
                                                CENSYS_INSTANCE ciInstance,
                                                const int nCameraIndex,
                                                int *nNumVertices );

//
// Name: censysSegmentorGetGroundPlanePolygon()
//
// Description:
//  This function retrieves the vertices of the ground plane polygon.
//
// Arguments:
//  ciInstance   - [in]  CENSYS_INSTANCE of interest
//  nCameraIndex - [in]  index of the camera of interest
//  pVertices    - [out] pointer to the array of vertex points
//  nMaxVertices - [in]  size of the point array
//
// Returns:
//  censyserrorSYSTEM_NOT_CREATED - if the system has not been created.
//
// See Also:
//  censysSegmentorGetGroundPlanePolygonNumVertices()
//  censysSegmentorSetGroundPlanePolygon()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSegmentorGetGroundPlanePolygon( CENSYS_INSTANCE ciInstance, 
                                      const int nCameraIndex,
                                      PPOINT2D pVertices, 
                                      const int nMaxVertices );

//
// Name: censysSegmentorSelectOutputData()
//
// Description:
//  This function is used to select the types of data that
//  are generated by the scene segmentor.  By default, the scene
//  segmentor generates the following:
//     - scene intensity image
//     - foreground intensity image
//     - background intensity image
//     - foreground disparity image
//     - foreground point cloud for each camera
//     - foreground point cloud for all cameras
//
// Arguments:
//  ciInstance - [in] CENSYS_INSTANCE of interest
//  unFlags    - [in] bitfield that indicates which types of data are to be 
//                    produced; construct bitfield by OR-ing any combination of
//                    CENSYS_DATA_TYPE enum constants
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSegmentorSelectOutputData( CENSYS_INSTANCE ciInstance,
			         unsigned int unFlags );

//
// Name: censysSegmentorGetTriclopsContext()
// 
// Description:
//  This function retrieves the Triclops context of the specified camera.  The
//  Triclops context is used for image rectification and stereo processing.
//
// Arguments:
//  ciInstance   - [in]  CENSYS_INSTANCE of interest
//  nCameraIndex - [in]  index of the camera in question
//  pContext     - [out] pointer to the Triclops context
//
// Returns: 
//  censyserrorSYSTEM_NOT_CREATED - if the system has not been created.
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysSegmentorGetTriclopsContext( CENSYS_INSTANCE    ciInstance,
                                   int                nCameraIndex,
                                   TriclopsContext    *pContext );

//=============================================================================
// Extractor Layer Functions
//=============================================================================
//
// Group=Extractor Layer Functions
//

//
// Name: censysExtractorGetVOIBounds()
//
// Description:
//  This function retrieves the bounding box around the volume of interest for 
//  people extraction.
//
// Arguments:
//  ciInstance - [in]  CENSYS_INSTANCE of interest
//  dMinX      - [out] pointer to the minimum bounds on x (meters)
//  dMaxX      - [out] pointer to the maximum bounds on x (meters)
//  dMinY      - [out] pointer to the minimum bounds on y (meters)
//  dMaxY      - [out] pointer to the maximum bounds on y (meters)
//  dTide      - [out] pointer to the minimum height of a person (meters)
//  dMaxHeight - [out] pointer to the maximum height of a person (meters)
//
// Returns:
//  censyserrorSYSTEM_NOT_CREATED - if the system has not yet been created
//
// See Also:
//  censysExtractorSetSimpleVOI()
//  censysExtractorGetVOIPolygon()
//  censysExtractorGetVOIHeight()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysExtractorGetVOIBounds( CENSYS_INSTANCE ciInstance,
                             double   *dMinX,
                             double   *dMaxX,
                             double   *dMinY,
                             double   *dMaxY,
                             double   *dTide,
                             double   *dMaxHeight );

//
// Name: censysExtractorSetSimpleVOI()
//
// Description:
//  This function sets a cube-shaped volume of interest for people extraction.
//
// Arguments:
//  ciInstance - [in] CENSYS_INSTANCE of interest
//  dMinX      - [in] minimum bounds on x (meters)
//  dMaxX      - [in] maximum bounds on x (meters)
//  dMinY      - [in] minimum bounds on y (meters)
//  dMaxY      - [in] maximum bounds on y (meters)
//  dTide      - [in] minimum height of a person (meters)
//  dMaxHeight - [in] maximum height of a person (meters)
//
// Returns:
//  censyserrorSYSTEM_NOT_CREATED - if the system has not yet been created
//
// See Also:
//  censysExtractorSetVOIPolygon()
//  censysExtractorSetVOIHeight()
//  censysExtractorGetVOIBounds()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysExtractorSetSimpleVOI( CENSYS_INSTANCE ciInstance,
                             double          dMinX,
                             double          dMaxX,
                             double          dMinY,
                             double          dMaxY,
                             double          dTide,
                             double          dMaxHeight );

//
// Name: censysExtractorSetVOIPolygon()
//
// Description:
//  This function sets the x-y dimension of the volume of interest (VOI) 
//  for people detection.  The VOI can be thought of as a vertically oriented
//  tube-like volume whose horizontal cross-section is a polygon in the x-y plane.
//  Vertices of the VOI polygon should be listed in either clockwise or 
//  counter-clockwise order.  The polygon need not be convex.
//
// Arguments:
//  ciInstance   - [in] CENSYS_INSTANCE of interest
//  pVertices    - [in] array of vertices of the VOI polygon (meters)
//  nNumVertices - [in] number of vertices
//
// Returns:
//  censyserrorSYSTEM_NOT_CREATED - if the system has not yet been created
//
// See Also:
//  censysExtractorGetVOIPolygon()
//  censysExtractorGetVOIBounds()
//  censysExtractorSetVOIHeight()
//  censysExtractorSetVOIPolygonInImageCoords()
//  censysExtractorSetSimpleVOI()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysExtractorSetVOIPolygon( CENSYS_INSTANCE ciInstance,
                              PPOINT2D pVertices,
                              int      nNumVertices );

//
// Name: censysExtractorGetNumVOIPolygonVertices()
//
// Description:
//  This function retrieves the number of vertices belonging to the VOI polygon.
//
// Arguments:
//  ciInstance   - [in]  the CENSYS_INSTANCE.
//  nNumVertices - [out] pointer to the number of vertices
//
// Returns:
//  censyserrorSYSTEM_NOT_CREATED - if the system has not yet been created
//
// See Also:
//  censysExtractorGetVOIPolygon()
//  censysExtractorSetVOIPolygon()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysExtractorGetNumVOIPolygonVertices( CENSYS_INSTANCE ciInstance,
                                         int *nNumVertices );

//
// Name: censysExtractorGetVOIPolygon()
//
// Description:
//  This function retrieves the volume of interest (VOI) polygon for people detection.  
//
// Arguments:
//  ciInstance   - [in]  CENSYS_INSTANCE of interest
//  pVertices    - [out] array of vertices of the VOI polygon (meters)
//  nMaxVertices - [int] size of the pVertices array
//
// Returns:
//  censyserrorSYSTEM_NOT_CREATED - if the system has not yet been created
//
// See Also:
//  censysExtractorGetNumVOIPolygonVertices()
//  censysExtractorSetVOIPolygon()
//  censysExtractorGetVOIBounds()
//  censysExtractorGetVOIHeight()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysExtractorGetVOIPolygon( CENSYS_INSTANCE ciInstance,
                              PPOINT2D   pVertices,
                              int        nMaxVertices );

//
// Name: censysExtractorSetVOIPolygonInImageCoords()
//
// Description:
//  This function is similar to censysExtractorSetVOIPolygon() except that 
//  vertices of the VOI polygon are specified in image coordinates 
//  (x=column, y=row).
//
// Arguments:
//  ciInstance   - [in] CENSYS_INSTANCE of interest
//  pVertices    - [in] array of vertices of the VOI polygon, in image coordinates
//  nNumVertices - [in] number of vertices
//
// Returns:
//  censyserrorSYSTEM_NOT_CREATED - if the system has not yet been created
//
// See Also:
//  censysExtractorSetVOIPolygon()
//  censysExtractorGetVOIPolygonInImageCoords()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysExtractorSetVOIPolygonInImageCoords( CENSYS_INSTANCE ciInstance,
                                           PPOINT2D pVertices,
                                           int      nNumVertices );

//
// Name: censysExtractorGetVOIPolygonInImageCoords()
//
// Description:
//  This function retrieves the volume of interest (VOI) polygon for people detection,
//  with the coordinates of the VOI polygon vertices specified in image 
//  coordinates (x=column, y=row).  
//
// Arguments:
//  ciInstance   - [in]  CENSYS_INSTANCE.
//  pVertices    - [out] array of vertices of the VOI polygon, in image coordinates
//  nMaxVertices - [in]  size of the pVertices array
//
// Returns:
//  censyserrorSYSTEM_NOT_CREATED - if the system has not yet been created
//
// See Also:
//  censysExtractorGetNumVOIPolygonVertices()
//  censysExtractorSetVOIInImageCoords()
//  censysExtractorGetVOIPolygon()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysExtractorGetVOIPolygonInImageCoords( CENSYS_INSTANCE ciInstance,
                                           PPOINT2D   pVertices,
                                           int        nMaxVertices );

//
// Name: censysExtractorSetVOIHeight()
//
// Description:
//  This function sets the height range for people who are to be detected.
//
// Arguments:
//  ciInstance - [in] CENSYS_INSTANCE of interest
//  dMinHeight - [in] minimum height of people to be detected (meters)
//  dMaxHeight - [in] maximum height of people to be detected (meters)
//
// Returns:
//  censyserrorSYSTEM_NOT_CREATED - if the system has not yet been created
//
// See Also:
//  censysExtractorGetVOIHeight()
//  censysExtractorSetVOIPolygon()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysExtractorSetVOIHeight( CENSYS_INSTANCE ciInstance,
                             double dMinHeight,
                             double dMaxHeight );

//
// Name: censysExtractorGetVOIHeight()
//
// Description:
//  This function gets the height range for people who are to be detected.
//
// Arguments:
//  ciInstance - [in]  CENSYS_INSTANCE of interest
//  dMinHeight - [out] pointer to minimum height of people to be detected (meters)
//  dMaxHeight - [out] pointer to maximum height of people to be detected (meters)
//
// Returns:
//  censyserrorSYSTEM_NOT_CREATED - if the system has not yet been created
//
// See Also:
//  censysExtractorSetVOIHeight()
//  censysExtractorGetVOIPolygon()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysExtractorGetVOIHeight( CENSYS_INSTANCE ciInstance,
                             double *dMinHeight,
                             double *dMaxHeight );

//
// Name: censysExtractorSetVOIPolygonString()
//
// Description:
//  This function is sets the volume of interest (VOI) polygon from
//  a string whose format is as specified in the INI file.  szString can take
//  on one of two formats:
//  "[numRows,numCols](row,col)..."
//  or
//  "(x,y)..."
//  or
//  "auto".
//
// In the first format, vertices are expressed in image coordinates.
// In the second format, vertices are expressed in x-y coordinates.
// Either format may be used.  Vertices may be listed in either clockwise
// or counter-clockwise order.  The polygon need not be convex.
//
// Note:
//  Multiple camera support has not yet been implemented for this
//  setting; thus, in order to set a VOI Polygon that spans multiple cameras,
//  use the second format in which vertices are expressed in x-y coordinates.
//
// Arguments:
//  ciInstance - [in] CENSYS_INSTANCE of interest
//  szString   - [in] string description of the VOI polygon, or
//                    "auto" for the value to be set automatically
//
// Returns:
//  censyserrorSYSTEM_NOT_CREATED - if the system has not yet been created
//
// See Also:
//  censysExtractorSetVOIPolygon()
//  censysExtractorGetVOIPolygonString()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysExtractorSetVOIPolygonString( CENSYS_INSTANCE ciInstance,
                                    char* szString );

//
// Name: censysExtractorGetVOIPolygonString()
//
// Description:
//  This function is sets the volume of interest (VOI) polygon from
//  a string whose format is as specified in the INI file.
//
// Arguments:
//  ciInstance - [in]  CENSYS_INSTANCE of interest
//  szString   - [out] pointer to the string description of the VOI polygon
//  nMaxLen    - [in]  maximum size of the string buffer
//
// Returns:
//  censyserrorSYSTEM_NOT_CREATED - if the system has not yet been created
//
// See Also:
//  censysExtractorGetVOIPolygon()
//  censysExtractorSetVOIPolygonString()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysExtractorGetVOIPolygonString( CENSYS_INSTANCE ciInstance,
                                    char *szString,
                                    int   nMaxLen );

//
// Name: censysExtractorGetGranularity() 
//
// Description: 
//  This function gets the size of the x-y grid used to low-pass filter 
//  point data and detect people. 
//
// Arguments:
//  ciInstance  - [in]  CENSYS_INSTANCE of interest
//  dResolution - [out] pointer to the cell size of the x-y grid (meters)
//
// See Also:
//  censysExtractorSetGranularity()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysExtractorGetGranularity( CENSYS_INSTANCE ciInstance,
                               double          *dResolution );

//
// Name: censysExtractorSetGranularity() 
//
// Description: 
//  This function sets the size of the x-y grid used to low-pass filter 
//  point data and detect people. 
//
// Arguments:
//  ciInstance  - [in] CENSYS_INSTANCE of interest
//  dResolution - [in] cell size of the x-y grid (meters)
//
// See Also:
//  censysExtractorGetGranularity()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysExtractorSetGranularity( CENSYS_INSTANCE ciInstance,
                               double          dResolution );

//
// Name: censysExtractorGetPointThreshold() 
// 
// Description: 
//  Gets the minimum number of points that must be present in an x-y grid
//  cell for it to be considered for people detection.
// 
// Arguments:
//  ciInstance - [in]  CENSYS_INSTANCE of interest
//  nThreshold - [out] pointer to the minimum number of points
//
// See Also:
//  censysExtractorSetPointThreshold()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysExtractorGetPointThreshold( CENSYS_INSTANCE ciInstance,
                                  int             *nThreshold );

//
// Name: censysExtractorSetPointThreshold() 
//
// Description:
//  Sets the minimum number of points that must be present in an x-y grid
//  cell for it to be considered for people detection.
// 
// Arguments:
//  ciInstance - [in] CENSYS_INSTANCE of interest
//  nThreshold - [in] minimum number of points, or
//                    CENSYS_AUTO to have the value set automatically
//
// See Also:
//  censysExtractorGetPointThreshold()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysExtractorSetPointThreshold( CENSYS_INSTANCE ciInstance,
                                  int             nThreshold );

//
// Name: censysExtractorGetProximityFilterRadius() 
//
// Description:
//  This function gets the maximum radius that a person
//  normally occupies in the x-y plane.
//
// Arguments:
//  ciInstance - [in]  CENSYS_INSTANCE of interest
//  dRadius    - [out] pointer to the proximity filter radius (meters)
//
// See Also:
//  censysExtractorSetProximityFilterRadius()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysExtractorGetProximityFilterRadius( CENSYS_INSTANCE ciInstance,
                                         double          *dRadius );

//
// Name: censysExtractorSetProximityFilterRadius() 
//
// Description:
//  This function sets the maximum radius that a person
//  normally occupies in the x-y plane.
//
// Arguments
//  ciInstance - [in] CENSYS_INSTANCE of interest
//  dRadius    - [in] proximity filter radius (meters)
//
// See Also:
//  censysExtractorGetProximityFilterRadius()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysExtractorSetProximityFilterRadius( CENSYS_INSTANCE ciInstance,
                                         double          dRadius );

//
// Name: censysExtractorGetMinHeightDropBetweenPeople()
//
// Description:
//  This function gets the minimum drop in height that is expected
//  to occur between people.  This aids in the detection of people
//  who are in close proximity to one another.
//
// Arguments:
//  ciInstance  - [in]  CENSYS_INSTANCE of interest
//  dHeightDrop - [out] pointer to the minimum height drop (meters)
//
// See Also:
//  censysExtractorSetMinHeightDropBetweenPeople()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysExtractorGetMinHeightDropBetweenPeople( CENSYS_INSTANCE ciInstance,
                                              double   *dHeightDrop );

//
// Name: censysExtractorSetMinHeightDropBetweenPeople()
//
// Description:
//  This function sets the minimum drop in height that is expected
//  to occur between people.  This aids in the detection of people
//  who are in close proximity to one another.
//
// Arguments
//  ciInstance  - [in] CENSYS_INSTANCE of interest
//  dHeightDrop - [in] minimum height drop (meters)
//
// Returns:
//  censyserrorSUCCESS - if the operation was successful.
//  censyserrorINVALID_INSTANCE - if the CENSYS_INSTANCE is invalid.
//
// See Also:
//  censysExtractorGetMinHeightDropBetweenPeople()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysExtractorSetMinHeightDropBetweenPeople( CENSYS_INSTANCE ciInstance,
                                              double   dHeightDrop );

//=============================================================================
// Tracker Layer Functions
//=============================================================================
//
// Group=Tracker Layer Functions
//

//
// Name: censysTrackerReset
//
// Description:
//  This function clears all history of tracked people.
//
// Arguments:
//  ciInstance - [in] CENSYS_INSTANCE of interest
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysTrackerReset( CENSYS_INSTANCE  ciInstance );

//
// Name: censysTrackerGetTotalPeopleTracked() 
//
// Description:
//  This function gets the total number of people tracked.
//
// Arguments:
//  ciInstance          - [in]  CENSYS_INSTANCE of interest
//  nTotalPeopleTracked - [out] pointer to the total people tracked
// 
// See Also:
//  censysTrackerSetTotalPeopleTracked()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysTrackerGetTotalPeopleTracked( CENSYS_INSTANCE  ciInstance,
                                    int             *nTotalPeopleTracked );

//
// Name: censysTrackerSetTotalPeopleTracked() 
//
// Description:
//  This function sets the total number of people tracked.  
//  Note that this counter is used to generate tracked people's IDs.
//
// Arguments:
//  ciInstance          - [in] CENSYS_INSTANCE of interest
//  nTotalPeopleTracked - [in] total people tracked
// 
// See Also:
//  censysTrackerGetTotalPeopleTracked()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysTrackerSetTotalPeopleTracked( CENSYS_INSTANCE  ciInstance,
                                    int             nTotalPeopleTracked );

//
// Name: censysTrackerGetMaxValidPersonVelocity() 
//
// Description:
//  This function gets the maximum velocity at which a tracked person is
//  expected to move.
//
// Arguments: 
//  ciInstance        - [in]  CENSYS_INSTANCE of interest
//  pMaxValidVelocity - [out] pointer to the max. person velocity (m/s)
//
// See Also:
//  censysTrackerSetMaxValidPersonVelocity
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysTrackerGetMaxValidPersonVelocity( CENSYS_INSTANCE  ciInstance,
                                        double          *pMaxValidVelocity );

//
// Name: censysTrackerSetMaxValidPersonVelocity() 
//
// Description:
//  This function sets the maximum velocity at which a tracked person is
//  expected to move.
//
// Arguments:
//  ciInstance        - [in] CENSYS_INSTANCE of interest
//  dMaxValidVelocity - [in] maximum allowable velocity of a person (m/s)
//
// See Also:
//  censysTrackerGetMaxValidPersonVelocity()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysTrackerSetMaxValidPersonVelocity( CENSYS_INSTANCE ciInstance,
                                        double          dMaxValidVelocity );

//
// Name: censysTrackerSetMaxVelocityAngle() 
//
// Description: 
//  This function sets the maximum velocity-angle product that a person
//  is capable of producing in a valid trajectory.
//
// Arguments:
//  ciInstance        - [in] CENSYS_INSTANCE of interest
//  dMaxVelocityAngle - [in] maximum velocity angle product (in rad.m/s)
//
// See Also:
//  censysTrackerGetMaxVelocityAngle()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysTrackerSetMaxVelocityAngle( CENSYS_INSTANCE ciInstance,
                                  double dMaxVelocityAngle );

//
// Name: censysTrackerGetMaxVelocityAngle()
//
// Description: 
//  This function gets the maximum velocity-angle product that a person
//  is capable of producing in a valid trajectory.
//
// Arguments:
//  ciInstance        - [in]  CENSYS_INSTANCE of interest
//  dMaxVelocityAngle - [out] pointer to the maximum velocity angle product (in rad.m/s)
//
// See Also:
//  censysTrackerSetMaxVelocityAngle()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysTrackerGetMaxVelocityAngle( CENSYS_INSTANCE ciInstance,
                                  double *dMaxVelocityAngle );

//
// Name: censysTrackerEnableFileOutput() 
//
// Description:
//  This function enables or disables the output of people's trajectories to
//  a file.  A person's trajectory is written to the file once they are no longer
//  tracked.
//
// Arguments: 
//  ciInstance - [in] CENSYS_INSTANCE of interest
//  bEnable    - [in] whether or not to enable the output of trajectories to file
//
// See Also:
//  censysTrackerSetOutputFileName()
//  censysTrackerGetOutputFileName()
//
CENSYS_API CENSYS_ERROR  MAAAPIENTRY
censysTrackerEnableFileOutput( CENSYS_INSTANCE  ciInstance,
                               bool bEnable );

//
// Name: censysTrackerSetOutputFileName
//
// Description: 
//  This function sets the name of the file to which tracked people
//  data is to be written.
//
// Arguments: 
//  ciInstance - [in] CENSYS_INSTANCE of interest
//  szFileName - [in] name of the output file
//
// See Also:
//  censysTrackerGetOutputFileName()
//  censysTrackerEnableFileOutput()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysTrackerSetOutputFileName( CENSYS_INSTANCE   ciInstance,
                                const char *szFileName );

//
// Name: censysTrackerGetOutputFileName
//
// Description: 
//  This function retrieves the name of the file to which tracked people
//  data is to be written, if file output is enabled.
//
// Arguments: 
//  ciInstance - [in]  CENSYS_INSTANCE of interest
//  szFileName - [out] pointer to the file name
//  nMaxLength - [in]  maximum length of the buffer pointed to by szFileName
//
// See Also:
//  censysTrackerSetOutputFileName()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysTrackerGetOutputFileName( CENSYS_INSTANCE   ciInstance,
                                char *szFileName,
                                int nMaxLength );

//
// Name: censysTrackerSetPlaybackFromFile() 
//
// Description:
//  This function enables or disables playback of tracker data from file.
//
// Arguments: 
//  ciInstance - [in] CENSYS_INSTANCE of interest
//  bEnable    - [in] whether or not to enable playback from file
//
// See Also:
//  censysTrackerGetPlaybackFromFile()
//  censysTrackerSetPlaybackFileName()
//  censysTrackerGetPlaybackFileName()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysTrackerSetPlaybackFromFile( CENSYS_INSTANCE   ciInstance,
                                  bool bEnable );

//
// Name: censysTrackerGetPlaybackFromFile() 
//
// Description:
//  This function indicates whether or not playback of tracker data from file
//  is enabled.
//
// Arguments: 
//  ciInstance - [in]  CENSYS_INSTANCE of interest
//  pEnabled   - [out] pointer to whether or not playback from file is enabled
//
// See Also:
//  censysTrackerSetPlaybackFromFile()
//  censysTrackerSetPlaybackFileName()
//  censysTrackerGetPlaybackFileName()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysTrackerGetPlaybackFromFile( CENSYS_INSTANCE   ciInstance,
                                  bool *pEnabled );

//
// Name: censysTrackerSetPlaybackFileName
//
// Description: 
//  This function sets the name of the file from which tracked people
//  data is to be played back.
//
// Arguments: 
//  ciInstance - [in] CENSYS_INSTANCE of interest
//  szFileName - [in] name of the input file
//
// See Also:
//  censysTrackerGetPlaybackFileName()
//  censysTrackerSetPlaybackFromFile()
//  censysTrackerGetPlaybackFromFile()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysTrackerSetPlaybackFileName( CENSYS_INSTANCE   ciInstance,
                                  const char *szFileName );

//
// Name: censysTrackerGetPlaybackFileName
//
// Description: 
//  This function retrieves the name of the file from which tracked people
//  data is to be played back, if playback from file is enabled.
//
// Arguments: 
//  ciInstance - [in]  CENSYS_INSTANCE of interest
//  szFileName - [out] pointer to the file name
//  nMaxLength - [int] maximum length of the szFileName buffer
//
// See Also:
//  censysTrackerSetPlaybackFileName()
//  censysTrackerSetPlaybackFromFile()
//  censysTrackerGetPlaybackFromFile()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysTrackerGetPlaybackFileName( CENSYS_INSTANCE   ciInstance,
                                  char *szFileName,
                                  int nMaxLength );

//
// Name: censysTrackerGetFramesBeforeDeletingPerson() 
//
// Description:
//  This function gets the number of frames before a tracked person
//  with no new path points is removed from the people tracker.  
//  e.g., if set to 2, a person is removed if they have no new points 
//  for 2 frames.
//
// Arguments:
//  ciInstance - [in]  CENSYS_INSTANCE of interest
//  pFrames    - [out] pointer to the number of frames before a person is removed
//
// See Also:
//  censysTrackerGetFramesBeforeDeletingPerson()
//  censysTrackerSetFramesBeforeDeletingPersonInBorder()
//  censysTrackerSetFramesBeforeDeletingCandidatePerson()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysTrackerGetFramesBeforeDeletingPerson( CENSYS_INSTANCE  ciInstance,
                                            int             *pFrames );

//
// Name: censysTrackerSetFramesBeforeDeletingPerson() 
//
// Description: 
//  This function sets the number of frames before a tracked person
//  with no new path points is removed from the people tracker.  
//  e.g., if set to 2, a person is removed if they have no new points 
//  for 2 frames.
//
// Arguments:
//  ciInstance - [in] CENSYS_INSTANCE of interest
//  nFrames    - [in] number of frames before a person is removed
//
// See Also:
//  censysTrackerGetFramesBeforeDeletingPerson()
//  censysTrackerGetFramesBeforeDeletingPersonInBorder()
//  censysTrackerGetFramesBeforeDeletingCandidatePerson()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysTrackerSetFramesBeforeDeletingPerson( CENSYS_INSTANCE ciInstance,
                                            int             nFrames );

//
// Name: censysTrackerGetFramesBeforeDeletingCandidatePerson() 
//
// Description:
//  Same as censysTrackerGetFramesBeforeDeletingPerson() except it applies to
//  candidate people (i.e., people who do not yet have the minimum number of
//  path points to qualify as a tracked person).
//
// Arguments:
//  ciInstance - [in]  CENSYS_INSTANCE of interest
//  nFrames    - [out] pointer to the number of frames before a person is removed
//
// See Also:
//  censysTrackerSetFramesBeforeDeletingCandidatePerson()
//  censysTrackerSetFramesBeforeDeletingPerson()
//  censysTrackerGetFramesBeforeDeletingPerson()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysTrackerGetFramesBeforeDeletingCandidatePerson( CENSYS_INSTANCE ciInstance,
                                                     int *nFrames );

//
// Name: censysTrackerSetFramesBeforeDeletingCandidatePerson() 
//
// Description: 
//  Same as censysTrackerSetFramesBeforeDeletingPerson() except it applies to
//  candidate people (i.e., people who do not yet have the minimum number of
//  path points to qualify as a tracked person).
//
// Arguments:
//  ciInstance - [in] CENSYS_INSTANCE of interest
//  nFrames    - [in] number of frames before a person is removed
//
// See Also:
//  censysTrackerGetFramesBeforeDeletingCandidatePerson()
//  censysTrackerGetFramesBeforeDeletingPerson()
//  censysTrackerSetFramesBeforeDeletingPerson()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysTrackerSetFramesBeforeDeletingCandidatePerson( CENSYS_INSTANCE ciInstance,
                                                     int nFrames );

//
// Name: censysTrackerSetFramesBeforeDeletingPersonInBorder() 
//
// Description:
//  Same as censysTrackerSetFramesBeforeDeletingPerson() except it applies to
//  tracked people and candidate people whose last path point lies within
//  an image border region.
//
// Arguments:
//  ciInstance - [in] CENSYS_INSTANCE of interest
//  nFrames    - [in] number of frames before a person is removed
//
// See Also:
//  censysTrackerGetFramesBeforeDeletingPersonInBorder()
//  censysTrackerSetFramesBeforeDeletingPerson()
//  censysTrackerGetFramesBeforeDeletingPerson()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysTrackerSetFramesBeforeDeletingPersonInBorder( CENSYS_INSTANCE ciInstance,
                                                    int nFrames );

//
// Name: censysTrackerGetFramesBeforeDeletingPersonInBorder() 
//
// Description: 
//  Same as censysTrackerSetFramesBeforeDeletingPerson() except it applies to
//  tracked people and candidate people whose last path point lies within
//  an image border region.
//
// Arguments:
//  ciInstance - [in]  CENSYS_INSTANCE of interest
//  nFrames    - [out] pointer to the number of frames before a person is removed
//
// See Also:
//  censysTrackerGetFramesBeforeDeletingPersonInBorder()
//  censysTrackerGetFramesBeforeDeletingPerson()
//  censysTrackerSetFramesBeforeDeletingPerson()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysTrackerGetFramesBeforeDeletingPersonInBorder( CENSYS_INSTANCE ciInstance,
                                                    int *nFrames );

//
// Name: censysTrackerSetMinPathPoints() 
//
// Description: 
//  This function sets the number of path points that a person must
//  have in order to be tracked.
//
// Arguments:
//  ciInstance - [in] CENSYS_INSTANCE of interest
//  nNumPoints - [in] minimum number of path points (greater than zero)
//
// See Also:
//  censysTrackerGetMinPathPoints()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysTrackerSetMinPathPoints( CENSYS_INSTANCE ciInstance,
                               int nNumPoints );

//
// Name: censysTrackerGetMinPathPoints() 
//
// Description: 
//  This function gets the number of path points that a person must
//  have in order to be tracked.
//
// Arguments:
//  ciInstance - [in]  CENSYS_INSTANCE of interest
//  nNumPoints - [out] pointer to the minimum number of path points
//
// See Also:
//  censysTrackerSetMinPathPoints()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysTrackerGetMinPathPoints( CENSYS_INSTANCE ciInstance,
                               int *nNumPoints );

//
// Name: censysTrackerSetMaxPathPoints() 
//
// Description: 
//  This function sets the maximum number of path points that a person 
//  can have.  Once a person's path has attained this number of points,
//  only the newer points are retained.
//
// Arguments:
//  ciInstance - [in] CENSYS_INSTANCE of interest
//  nNumPoints - [in] maximum number of path points (greater than zero)
//
// See Also:
//  censysTrackerGetMaxPathPoints()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysTrackerSetMaxPathPoints( CENSYS_INSTANCE ciInstance,
                               int nNumPoints );

//
// Name: censysTrackerGetMaxPathPoints() 
//
// Description: 
//  This function gets the maximum number of path points that a person
//  can have.
//
// Arguments:
//  ciInstance - [in]  CENSYS_INSTANCE of interest
//  nNumPoints - [out] pointer to the maximum number of path points
//
// See Also:
//  censysTrackerSetMaxPathPoints()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysTrackerGetMaxPathPoints( CENSYS_INSTANCE ciInstance,
                               int *nNumPoints );

//
// Name: censysTrackerSetMinPathPointsPenalty() 
//
// Description: 
//  This function sets the additional number of path points that a person must
//  have in order to be tracked if their path does not originate outside the 
//  trusted regions.
//
// Arguments:
//  ciInstance     - [in] CENSYS_INSTANCE of interest
//  nPointsPenalty - [in] path-point penalty
//
// See Also:
//  censysTrackerGetMinPathPointsPenalty()
//  censysTrackerSetMinPathPoints()
//  censysTrackerGetMinPathPoints()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysTrackerSetMinPathPointsPenalty( CENSYS_INSTANCE ciInstance,
                                      int nPointsPenalty );

//
// Name: censysTrackerGetMinPathPointsPenalty() 
//
// Description: 
//  This function gets the additional number of path points that a person must
//  have in order to be tracked if their path does not originate outside the 
//  trusted regions.
//
// Arguments:
//  ciInstance     - [in]  CENSYS_INSTANCE of interest
//  nPointsPenalty - [out] pointer to the path-point penalty
//
// See Also:
//  censysTrackerSetMinPathPointsPenalty()
//  censysTrackerSetMinPathPoints()
//  censysTrackerGetMinPathPoints()
//
CENSYS_API CENSYS_ERROR MAAAPIENTRY
censysTrackerGetMinPathPointsPenalty( CENSYS_INSTANCE ciInstance,
                                      int *nPointsPenalty );

#ifdef CENSYS_FIXEDPOINT
#include "censysfixed.h"
#endif

#ifdef CENSYS_INTERNAL
#include "censysinternal.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* !__CENSYS_H__ */
