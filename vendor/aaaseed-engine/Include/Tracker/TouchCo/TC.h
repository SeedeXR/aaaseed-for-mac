/*++

Copyright (c) 2009 Touchco Inc.

Module Name:
    TC.h

Description:
    API for interfacing with Touchco devices

Revision History:
    04/09/09  nawad   Created.
    27/10/09  nawad   Added TCForceFrame,TCContactFrame

--*/

#ifndef __TC_H__
#define __TC_H__

#ifdef WIN32
#include <windows.h>
#else
#define WINAPI
#endif

#ifdef TOUCHCO_EXPORTS
  #define TOUCHCO_API __declspec(dllexport)
#else
  #define TOUCHCO_API __declspec(dllimport)
#endif

#ifndef WIN32
  #ifdef TOUCHCO_API
    #undef TOUCHCO_API
    #define TOUCHCO_API
  #endif
#endif

typedef void*           TC_HANDLE;
typedef unsigned int    TC_STATUS;

#define TC_MAX_DEVICES  16

//
// Device status
//

enum  TCErrors
{
  TC_OK,
  TC_DEVICE_NOT_FOUND,
  TC_DEVICE_NOT_OPENED,
  TC_DEVICE_ALREADY_OPENED,
  TC_DEVICE_TIMEOUT,
  TC_OPEN_PORT_ERROR,
  TC_IO_ERROR,
  TC_ERROR,  
};

#define TC_SUCCESS(status) ((status) == TC_OK)

//
// TC_SetOptions Flags
//

#define TC_ENABLED          1
#define TC_DISABLED         0

#define TC_CONTACTS         0x01 // Default : TC_ENABLED
#define TC_EVENT_QUEUE      0x02 // Default : TC_ENABLED
#define TC_PEAK_SEPARATION  0x04 // Default : TC_ENABLED


#ifdef __cplusplus
extern "C" {
#endif

//
// Structure to hold contact information
//

typedef struct tc_contact_s
{
  enum TCEventType
  {
    NONE,
    START,
    UPDATE,
    END
  };

  enum TCContactType          // Reserved for future use
  {
    UNKNOWN,
    FINGER,
    PEN
  };

  TCEventType    eventType;   // Type of event
  TCContactType  contactType; // Reserved for future use

  // Basic touch information
  float          x;           // X position in millimeters
  float          y;           // Y position in millimeters
  float          force;       // Contact force
  float          radius;      // Contact radius in millimeters
  float          area;        // Contact area in millimeters

  // Tracking information
  float          dx;          // Difference in X since last frame in millimeters
  float          dy;          // Difference in Y ince last frame in millimeters
  float          dforce;      // Difference in force since last frame
  double         dt;          // Difference in time ince last frame in seconds

  // Ellipse information
  float          majorAxis;   // Major axis
  float          minorAxis;   // Minor axis
  float          orientation; // Orientation

  // Touch identification
  int            id;          // Contact identifier
  unsigned long  uid;         // Contact unique identifier
  double         startTime;   // First time the contact was detected in seconds
  double         time;        // Time when the contact was updated in seconds
  unsigned long  frameNumber; // The frame number where the contact was detected
} TCContact;


//
// Structure to hold one frame of force values
//

typedef struct tc_forceframe_s
{
  float*        forces;       // Array of force values
  unsigned long frameNumber;  // Frame number the values correspond to
  double        timeStamp;    // Sensor time of the frame
} TCForceFrame;


//
// Structure to hold one frame of contacts
//

typedef struct tc_contactframe_s
{
  TCContact*     contacts;    // Array of contacts
  unsigned int   numContacts; // Mumber of contacts in the array
  unsigned long  frameNumber; // Frame number these contacts correspond to
  double         timeStamp;   // Sensor time when the contacts were detected
} TCContactFrame;


//
// Structure to hold Touchco sensor description
// 

typedef struct tc_sensorinfo_s
{
  int     rows;                // Number of rows
  int     cols;                // Number of columns
  int     maxContacts;         // Maximum number of contacts the sensor can report   
  float   width;               // Width in millimeters
  float   height;              // Height in millimeters
  char    model[16];           // Device model name
  char    firmwareVersion[16]; // Firmware version of the device
  char    serialNumber[16];    // Serial number of the device
} TCSensorInfo;

typedef void (*PTC_CONTACT_CALLBACK)(TC_HANDLE const handle, void* data, TCSensorInfo sensorInfo);


// Scan for available devices
TOUCHCO_API
TC_STATUS WINAPI TC_GetNumDevices(unsigned int* numDevs);

// Open device by identifier
TOUCHCO_API
TC_STATUS WINAPI TC_OpenByIndex(TC_HANDLE* handle, int index);

// Open device by serial number
TOUCHCO_API
TC_STATUS WINAPI TC_OpenBySerialNumber(TC_HANDLE* handle, char* serialNumber);

// Close device
TOUCHCO_API
TC_STATUS WINAPI TC_Close(TC_HANDLE handle);

// Retrieve information on the sensor
TOUCHCO_API
TC_STATUS WINAPI TC_GetSensorInfo(TC_HANDLE handle,
                                  TCSensorInfo* sensorInfo);

// Initialize a force image for this sensor
TOUCHCO_API
TC_STATUS WINAPI TC_InitForceFrame(TC_HANDLE handle, TCForceFrame* forceFrame);

// Retrieve latest available force image
TOUCHCO_API
TC_STATUS WINAPI TC_GetForceFrame(TC_HANDLE handle, TCForceFrame* forceframe);

// Initialize an array of max contacts for this sensor
TOUCHCO_API
TC_STATUS WINAPI TC_InitContactFrame(TC_HANDLE handle,
                                     TCContactFrame* contactFrame);

// Retrieve the number of frames available in the contact queue
TOUCHCO_API
TC_STATUS WINAPI TC_GetNumFramesInQueue(TC_HANDLE handle,
                                        unsigned int* numFramesFound);

// Retrieve one frame of contacts from the queue
TOUCHCO_API
TC_STATUS WINAPI TC_GetQueuedContactFrame(TC_HANDLE handle,
                                          TCContactFrame* contactFrame);

// Get the latest detected contacts
TOUCHCO_API
TC_STATUS WINAPI TC_GetLatestContactFrame(TC_HANDLE handle,
                                          TCContactFrame* contactFrame);

// Register a contact callback
TOUCHCO_API
TC_STATUS WINAPI TC_RegisterContactCallback(TC_HANDLE handle,
                                            PTC_CONTACT_CALLBACK callback,
                                            void* data);

// Enable option
TOUCHCO_API
TC_STATUS WINAPI TC_SetOption(TC_HANDLE handle, long option, int value);

// Disable option
TOUCHCO_API
TC_STATUS WINAPI TC_GetOption(TC_HANDLE handle, long option, int* value);

#ifdef __cplusplus
}
#endif

#endif // __TC_H__
