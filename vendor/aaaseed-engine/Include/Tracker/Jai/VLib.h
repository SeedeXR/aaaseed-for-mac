/////////////////////////////////////////////////////////////////////////////////////////
// 
// Simple Vision Library
// 2006
// JAI Corporation
//
/////////////////////////////////////////////////////////////////////////////////////////

/// \file		$Source: $
/// \brief		Vision Library
/// \version	$Revison: 0.1 $
/// \author		sek ( JAI )
/// \date		$Date: 2006/08/09 15:12:00 $

#pragma once

#ifndef	VISION_LIBRARY_H_
#define	VISION_LIBRARY_H_

#include <TLI/Client.h>

typedef	void *	VW_HANDLE;
typedef void *	SP_HANDLE;
typedef void *  DM_HANDLE;

#ifdef WIN32
  #ifdef _VLIBDLL
    #define VL_IMPORT_EXPORT __declspec(dllexport) 
  #else
    #define VL_IMPORT_EXPORT 
  #endif
  #define VL_CALLTYPE __stdcall
  
  #if ! defined EXTERN_C
    #define EXTERN_C extern "C"
  #endif
#else

  #define VL_IMPORT_EXPORT 
  #define VL_CALLTYPE 

  #define VL_IMPORT_EXPORT 
  
  #if ! defined EXTERN_C
    #define EXTERN_C extern "C"
  #endif

#endif
	
typedef int32_t VL_ERROR;

enum VLErrorID 
{
	VL_ERR_SUCCESS         = 0,
    VL_INVALID_BUFFER_SIZE = -1,  ///< Invalid buffer size
    VL_INVALID_HANDLE      = -2,  ///< Invalid handle
    VL_INVALID_ID          = -3,  ///< Invalid ID
    VL_ACCESS_DENIED       = -4,  ///< Access denied
    VL_NO_DATA             = -5,  ///< No data
	VL_ERR_ERROR           = -6,  ///< Generic errorcode
    VL_INVALID_PARAMETER   = -7,  ///< Invalid parameter
    VL_TIMEOUT             = -8,  ///< Timeout
    VL_INVALID_FILENAME    = -9,  ///< Invalid file name
    VL_INVALID_ADDRESS     = -10, ///< Invalid address
    VL_FILE_IO             = -11, ///< File IO error
    VL_GC_ERROR            = -12  ///< GenICam error
};

#define	SCR_COLOR_DEPTH	4
#define	XGA_WIDTH		1024
#define	XGA_HEIGHT		768
#define	SXGA_WIDTH		1280
#define	SXGA_HEIGHT		1024
#define	NUM_OF_BUFFER	10
#define	MAX_BPP			4
#define	BUFFER_SIZE		XGA_WIDTH*XGA_HEIGHT*MAX_BPP

typedef enum
{
	IVW_OVERLAPPED = 0,
	IVW_CHILD = 1,
	IVW_CHILD_STRETCH = 2,
	IVW_OVERLAPPED_STRETCH = 3,	// sek 2007/08/22 Added.
	IVW_OVERLAPPED_AUTO = 4,	// sek 2007/09/12 Added.
} IVW_WINDOW_TYPE;

// Image View Window Option selector to be used when setting and getting options for the View Wiondow
typedef enum
{
	IVW_MOUSE_ZOOM = 0,         // Enable/Disable the mouse zoom feature using vlSetViewWindowOption(). The pOption parameter is a pointer to a bool value
	IVW_MOUSE_CURSOR = 1,       // Enable/Disable the mouse cursor using vlSetViewWindowOption(). The pOption parameter is a pointer to a bool value
	IVW_CALLBACK_MASK = 2,      // Select which of the callback events to enable. The pOption parameter is a pointer to an integer value and the value is a bitmask created by OR'ing the IVW_CALLBACK_TYPE enumeration values
	IVW_DOUBLE_BUFFERING = 3,   // Enable/Disable double-buffering using vlSetViewWindowOption(). The pOption parameter is a pointer to a bool value
} IVW_OPTION;

// Image View Window Callback type. This type determines how to interprete the data passed in by the callback value parameter
typedef enum
{
	IVW_CB_WINDOW_MOVE        = 0x0001,  // The View Window has been moved. The pValue pointer in the callback function points to a POINT structure
	IVW_CB_WINDOW_SIZE        = 0x0002,  // The View Window has been resized. The pValue pointer in the callback function points to a SIZE structure
	IVW_CB_MOUSE_MOVE         = 0x0004,  // The mouse has been moved inside the View Window. The pValue pointer in the callback function points to a POINT structure. The location is in image pixel coordinates
	IVW_CB_MOUSE_LBUTTON_DOWN = 0x0008,  // The left mouse button has been clicked inside the View Window. The pValue pointer in the callback function points to a POINT structure. The location is in image pixel coordinates
	IVW_CB_MOUSE_LBUTTON_UP   = 0x0010,  // The left mouse button has been released inside the View Window. The pValue pointer in the callback function points to a POINT structure. The location is in image pixel coordinates
	IVW_CB_MOUSE_RBUTTON_DOWN = 0x0020,  // The right mouse button has been clicked inside the View Window. The pValue pointer in the callback function points to a POINT structure. The location is in image pixel coordinates
	IVW_CB_MOUSE_RBUTTON_UP   = 0x0040,  // The right mouse button has been released inside the View Window. The pValue pointer in the callback function points to a POINT structure. The location is in image pixel coordinates
    IVW_CB_ZOOM_POS           = 0x0080,  // The Zoom or view location has changed the View Window. The pValue pointer returns the new view position and zoom values in a IVW_ZOOM structure
    IVW_CB_AREA_SELECT        = 0x0100,  // The user has dragged a selection rectangle using the mouse. The pValue pointer returns the selected rectangle in a RECT structure
    IVW_CB_USER_PAINT_IMAGE   = 0x0200,  // The user can now paint something on top of the image (such as text, graphics, ...)
    IVW_CB_USER_PAINT_WINDOW  = 0x0400,  // The user can now paint something on top of the View Window (such as text, graphics, ...)
} IVW_CALLBACK_TYPE;

typedef struct tagViewWindowZoomPosition
{
    POINT  offset;
    double ZoomRatioX;
    double ZoomRatioY;
} IVW_ZOOM_POS, *PIVW_ZOOM_POS;

typedef struct tagViewWindowUserPaint
{
    HDC     hDC;
    int32_t ImageWidth;
    int32_t ImageHeight;
} IVW_USER_PAINT, *PIVW_USER_PAINT;

// Parallel Processing for Image Conversion
typedef enum
{
	IA_PARALLEL_PROCESSING_DEFAULT = -1,	
	IA_PARALLEL_PROCESSING_DISABLED = 0,	
	IA_PARALLEL_PROCESSING_ENABLED = 1,
} IFC_PARALLEL_PROCESSING;

// Bayer Interpolation Algorithm
typedef enum
{
	IA_BAYER_STANDARD = 0,
	IA_BAYER_STANDARD_MULTI = 1, // kic 2010.09.28 Changed order of Extend and StandardMulti
	IA_BAYER_EXTEND = 2,         // kic 2010.09.28 Changed order of Extend and StandardMulti
	IA_BAYER_EXTEND_MULTI = 3,
	IA_BAYER_SIMPLE = 4,		// sek 2010.09.08 Added.
	IA_BAYER_SIMPLE_MULTI = 5,	// kic 2010.09.28 Added.
	IA_BAYER_HALF = 6,			// sek 2010.09.08 Added.
	IA_BAYER_HALF_MULTI = 7,	// sek 2010.09.11 Added.
} IFC_BAYER_ALGORITHM;

// Output Format of ConvertImage()
typedef enum
{
	OF_NONE = 0,	        // No conversion. Used in MallocEx to allocate identical image buffer
	OF_24BIT_SWAP_RB = 1,	// Swaps the data of R and B.
	OF_8BIT_Y,			// Converts to Y.
	OF_24BIT_YUV,
	OF_24BIT_YCBCR,

	OF_48BIT_SWAP_RB,
	OF_16BIT_Y,

	OF_SWAP_RB,			// selects the bit-length automatically.
	OF_Y,				// selects the bit-length automatically.
    OF_3Y_H,            // selects the bit-length automatically.
    OF_3Y_V,            // selects the bit-length automatically.
} IFC_PIXEL_FORMAT;

typedef enum
{
	FF_TIFF = 1,		// TIFF
	FF_JPEG,			// JPEG
	FF_BMP,				// BMP
	FF_JAI,				// JAI proprietary RAW image format including header with details about image size, location and pixelformat
    FF_RAW,             // Same as vlStoreToFileRAW()
} SIF_FILE_FORMAT;

//===================================================
// PIXEL TYPES
//===================================================
// Indicate if pixel is monochrome or RGB
#define GVSP_PIX_MONO		0x01000000
#define GVSP_PIX_RGB		0x02000000
#define GVSP_PIX_COLOR		0x02000000		// added 2008/07/14 Ver1.1c
#define GVSP_PIX_CUSTOM		0x80000000
#define GVSP_PIX_COLOR_MASK	0x7F000000

// Indicate effective number of bits occupied by the pixel (including padding).
// This can be used to compute amount of memory required to store an image.
#define GVSP_PIX_OCCUPY8BIT					0x00080000
#define GVSP_PIX_OCCUPY10BIT				0x000A0000
#define GVSP_PIX_OCCUPY12BIT				0x000C0000
#define GVSP_PIX_OCCUPY16BIT				0x00100000
#define GVSP_PIX_OCCUPY24BIT				0x00180000
#define GVSP_PIX_OCCUPY32BIT				0x00200000
#define GVSP_PIX_OCCUPY36BIT				0x00240000
#define GVSP_PIX_OCCUPY48BIT				0x00300000
#define GVSP_PIX_EFFECTIVE_PIXEL_SIZE_MASK	0x00FF0000
#define GVSP_PIX_EFFECTIVE_PIXEL_SIZE_SHIFT 16

// Pixel ID: lower 16-bit of the pixel type
#define GVSP_PIX_ID_MASK 0x0000FFFF

// 26.1 Mono buffer format defines
#define GVSP_PIX_MONO8		 	(GVSP_PIX_MONO | GVSP_PIX_OCCUPY8BIT | 0x0001)
#define GVSP_PIX_MONO8_SIGNED	(GVSP_PIX_MONO | GVSP_PIX_OCCUPY8BIT | 0x0002)
#define GVSP_PIX_MONO10			(GVSP_PIX_MONO | GVSP_PIX_OCCUPY16BIT | 0x0003)
#define GVSP_PIX_MONO10_PACKED	(GVSP_PIX_MONO | GVSP_PIX_OCCUPY12BIT | 0x0004)
#define PFNC_PIX_MONO10_PACKED	(GVSP_PIX_MONO | GVSP_PIX_OCCUPY10BIT | 0x8004)			// New PFNC
#define GVSP_PIX_MONO12			(GVSP_PIX_MONO | GVSP_PIX_OCCUPY16BIT | 0x0005)
#define GVSP_PIX_MONO12_PACKED	(GVSP_PIX_MONO | GVSP_PIX_OCCUPY12BIT | 0x0006)
#define PFNC_PIX_MONO12_PACKED	(GVSP_PIX_MONO | GVSP_PIX_OCCUPY12BIT | 0x8006)			// New PFNC
#define	GVSP_PIX_MONO14			(GVSP_PIX_MONO | GVSP_PIX_OCCUPY16BIT | 0x0025)			// added 2008/07/14 Ver1.1c
#define GVSP_PIX_MONO16			(GVSP_PIX_MONO | GVSP_PIX_OCCUPY16BIT | 0x0007)

// 26.2 Bayer buffer format defines
#define GVSP_PIX_BAYGR8 (GVSP_PIX_MONO | GVSP_PIX_OCCUPY8BIT | 0x0008)
#define GVSP_PIX_BAYRG8 (GVSP_PIX_MONO | GVSP_PIX_OCCUPY8BIT | 0x0009)
#define GVSP_PIX_BAYGB8 (GVSP_PIX_MONO | GVSP_PIX_OCCUPY8BIT | 0x000A)
#define GVSP_PIX_BAYBG8 (GVSP_PIX_MONO | GVSP_PIX_OCCUPY8BIT | 0x000B)
#define GVSP_PIX_BAYGR10 (GVSP_PIX_MONO | GVSP_PIX_OCCUPY16BIT | 0x000C)
#define GVSP_PIX_BAYRG10 (GVSP_PIX_MONO | GVSP_PIX_OCCUPY16BIT | 0x000D)
#define GVSP_PIX_BAYGB10 (GVSP_PIX_MONO | GVSP_PIX_OCCUPY16BIT | 0x000E)
#define GVSP_PIX_BAYBG10 (GVSP_PIX_MONO | GVSP_PIX_OCCUPY16BIT | 0x000F)
#define GVSP_PIX_BAYGR12 (GVSP_PIX_MONO | GVSP_PIX_OCCUPY16BIT | 0x0010)
#define GVSP_PIX_BAYRG12 (GVSP_PIX_MONO | GVSP_PIX_OCCUPY16BIT | 0x0011)
#define GVSP_PIX_BAYGB12 (GVSP_PIX_MONO | GVSP_PIX_OCCUPY16BIT | 0x0012)
#define GVSP_PIX_BAYBG12 (GVSP_PIX_MONO | GVSP_PIX_OCCUPY16BIT | 0x0013)

#define GVSP_PIX_BAYGR10_PACKED  (GVSP_PIX_MONO | GVSP_PIX_OCCUPY12BIT  | 0x0026) 
#define GVSP_PIX_BAYRG10_PACKED  (GVSP_PIX_MONO | GVSP_PIX_OCCUPY12BIT  | 0x0027) 
#define GVSP_PIX_BAYGB10_PACKED  (GVSP_PIX_MONO | GVSP_PIX_OCCUPY12BIT  | 0x0028) 
#define GVSP_PIX_BAYBG10_PACKED  (GVSP_PIX_MONO | GVSP_PIX_OCCUPY12BIT  | 0x0029) 
#define GVSP_PIX_BAYGR12_PACKED  (GVSP_PIX_MONO | GVSP_PIX_OCCUPY12BIT  | 0x002A) 
#define GVSP_PIX_BAYRG12_PACKED  (GVSP_PIX_MONO | GVSP_PIX_OCCUPY12BIT  | 0x002B) 
#define GVSP_PIX_BAYGB12_PACKED  (GVSP_PIX_MONO | GVSP_PIX_OCCUPY12BIT  | 0x002C) 
#define GVSP_PIX_BAYBG12_PACKED  (GVSP_PIX_MONO | GVSP_PIX_OCCUPY12BIT  | 0x002D) 

// New PFNC types
#define PFNC_PIX_BAYGR10_PACKED	(GVSP_PIX_MONO | GVSP_PIX_OCCUPY10BIT  | 0x8026) 
#define PFNC_PIX_BAYRG10_PACKED	(GVSP_PIX_MONO | GVSP_PIX_OCCUPY10BIT  | 0x8027) 
#define PFNC_PIX_BAYGB10_PACKED	(GVSP_PIX_MONO | GVSP_PIX_OCCUPY10BIT  | 0x8028) 
#define PFNC_PIX_BAYBG10_PACKED	(GVSP_PIX_MONO | GVSP_PIX_OCCUPY10BIT  | 0x8029) 
#define PFNC_PIX_BAYGR12_PACKED	(GVSP_PIX_MONO | GVSP_PIX_OCCUPY12BIT  | 0x802A) 
#define PFNC_PIX_BAYRG12_PACKED	(GVSP_PIX_MONO | GVSP_PIX_OCCUPY12BIT  | 0x802B) 
#define PFNC_PIX_BAYGB12_PACKED	(GVSP_PIX_MONO | GVSP_PIX_OCCUPY12BIT  | 0x802C) 
#define PFNC_PIX_BAYBG12_PACKED	(GVSP_PIX_MONO | GVSP_PIX_OCCUPY12BIT  | 0x802D) 

#define GVSP_PIX_BAYGR16		 (GVSP_PIX_MONO | GVSP_PIX_OCCUPY16BIT  | 0x002E) 
#define GVSP_PIX_BAYRG16		 (GVSP_PIX_MONO | GVSP_PIX_OCCUPY16BIT  | 0x002F) 
#define GVSP_PIX_BAYGB16		 (GVSP_PIX_MONO | GVSP_PIX_OCCUPY16BIT  | 0x0030) 
#define GVSP_PIX_BAYBG16		 (GVSP_PIX_MONO | GVSP_PIX_OCCUPY16BIT  | 0x0031) 

// 26.3 RGB Packed buffer format defines
#define GVSP_PIX_RGB8_PACKED (GVSP_PIX_COLOR | GVSP_PIX_OCCUPY24BIT | 0x0014)
#define GVSP_PIX_BGR8_PACKED (GVSP_PIX_COLOR | GVSP_PIX_OCCUPY24BIT | 0x0015)
#define GVSP_PIX_RGBA8_PACKED (GVSP_PIX_COLOR | GVSP_PIX_OCCUPY32BIT | 0x0016)
#define GVSP_PIX_BGRA8_PACKED (GVSP_PIX_COLOR | GVSP_PIX_OCCUPY32BIT | 0x0017)
#define GVSP_PIX_RGB10_PACKED (GVSP_PIX_COLOR | GVSP_PIX_OCCUPY48BIT | 0x0018)
#define GVSP_PIX_BGR10_PACKED (GVSP_PIX_COLOR | GVSP_PIX_OCCUPY48BIT | 0x0019)
#define GVSP_PIX_RGB12_PACKED (GVSP_PIX_COLOR | GVSP_PIX_OCCUPY48BIT | 0x001A)
#define GVSP_PIX_BGR12_PACKED (GVSP_PIX_COLOR | GVSP_PIX_OCCUPY48BIT | 0x001B)
#define GVSP_PIX_RGB16_PACKED (GVSP_PIX_COLOR | GVSP_PIX_OCCUPY48BIT | 0x0033)			// added 2008/07/14 Ver1.1c
#define GVSP_PIX_RGB10V1_PACKED (GVSP_PIX_COLOR | GVSP_PIX_OCCUPY32BIT | 0x001C)
#define GVSP_PIX_RGB10V2_PACKED (GVSP_PIX_COLOR | GVSP_PIX_OCCUPY32BIT | 0x001D)
#define GVSP_PIX_RGB12V1_PACKED (GVSP_PIX_COLOR | GVSP_PIX_OCCUPY36BIT | 0x0034)		// added 2008/07/14 Ver1.1c

// 26.4 YUV Packed buffer format defines
#define GVSP_PIX_YUV411_PACKED (GVSP_PIX_COLOR | GVSP_PIX_OCCUPY12BIT | 0x001E)
#define GVSP_PIX_YUV422_PACKED (GVSP_PIX_COLOR | GVSP_PIX_OCCUPY16BIT | 0x001F)
#define GVSP_PIX_YUV422_YUYV_PACKED (GVSP_PIX_COLOR | GVSP_PIX_OCCUPY16BIT | 0x0032)	// added 2008/07/14 Ver1.1c
#define GVSP_PIX_YUV444_PACKED (GVSP_PIX_COLOR | GVSP_PIX_OCCUPY24BIT | 0x0020)
#define GVSP_PIX_YUV420P_NV12_PACKED (GVSP_PIX_COLOR | GVSP_PIX_OCCUPY12BIT | 0x0040)   // Special YUV 420 semi-planar from TS-5000

// 26.5 RGB Planar buffer format defines
#define GVSP_PIX_RGB8_PLANAR (GVSP_PIX_COLOR | GVSP_PIX_OCCUPY24BIT | 0x0021)
#define GVSP_PIX_RGB10_PLANAR (GVSP_PIX_COLOR | GVSP_PIX_OCCUPY48BIT | 0x0022)
#define GVSP_PIX_RGB12_PLANAR (GVSP_PIX_COLOR | GVSP_PIX_OCCUPY48BIT | 0x0023)
#define GVSP_PIX_RGB16_PLANAR (GVSP_PIX_COLOR | GVSP_PIX_OCCUPY48BIT | 0x0024)

// RGB 8 bit with alpha channel
#define GVSP_PIX_RGB8A		(GVSP_PIX_COLOR | GVSP_PIX_OCCUPY32BIT | 0x0025)


// Internal use only
#define	GVSP_PIX_BGR16_PACKED_INTERNAL (/*GVSP_PIX_RGB*/ GVSP_PIX_COLOR | GVSP_PIX_OCCUPY48BIT | 0x0000)

// New unified pixel formats for the VL
// These Pixel Formats are used internally in the Vision Library and are unique to the JAI SDK (no longer just GigE Vision)
#define PF_MONO8				(GVSP_PIX_CUSTOM | GVSP_PIX_MONO8)
#define PF_MONO8_SIGNED			(GVSP_PIX_CUSTOM | GVSP_PIX_MONO8_SIGNED)
#define PF_MONO10				(GVSP_PIX_CUSTOM | GVSP_PIX_MONO10)
#define PF_MONO10_PACKED		(GVSP_PIX_CUSTOM | GVSP_PIX_MONO10_PACKED)
#define PF_MONO10_P				(GVSP_PIX_CUSTOM | PFNC_PIX_MONO10_PACKED)
#define PF_MONO12				(GVSP_PIX_CUSTOM | GVSP_PIX_MONO12)
#define PF_MONO12_PACKED		(GVSP_PIX_CUSTOM | GVSP_PIX_MONO12_PACKED)
#define PF_MONO12_P				(GVSP_PIX_CUSTOM | PFNC_PIX_MONO12_PACKED)
#define	PF_MONO14				(GVSP_PIX_CUSTOM | GVSP_PIX_MONO14)
#define PF_MONO16				(GVSP_PIX_CUSTOM | GVSP_PIX_MONO16)

// 26.2 Bayer buffer format defines
#define PF_BAYGR8				(GVSP_PIX_CUSTOM | GVSP_PIX_BAYGR8)
#define PF_BAYRG8				(GVSP_PIX_CUSTOM | GVSP_PIX_BAYRG8)
#define PF_BAYGB8				(GVSP_PIX_CUSTOM | GVSP_PIX_BAYGB8)
#define PF_BAYBG8				(GVSP_PIX_CUSTOM | GVSP_PIX_BAYBG8)
#define PF_BAYGR10				(GVSP_PIX_CUSTOM | GVSP_PIX_BAYGR10)
#define PF_BAYRG10				(GVSP_PIX_CUSTOM | GVSP_PIX_BAYRG10)
#define PF_BAYGB10				(GVSP_PIX_CUSTOM | GVSP_PIX_BAYGB10)
#define PF_BAYBG10				(GVSP_PIX_CUSTOM | GVSP_PIX_BAYBG10)
#define PF_BAYGR12				(GVSP_PIX_CUSTOM | GVSP_PIX_BAYGR12)
#define PF_BAYRG12				(GVSP_PIX_CUSTOM | GVSP_PIX_BAYRG12)
#define PF_BAYGB12				(GVSP_PIX_CUSTOM | GVSP_PIX_BAYGB12)
#define PF_BAYBG12				(GVSP_PIX_CUSTOM | GVSP_PIX_BAYBG12)

#define PF_BAYGR10_PACKED		(GVSP_PIX_CUSTOM | GVSP_PIX_BAYGR10_PACKED)
#define PF_BAYRG10_PACKED		(GVSP_PIX_CUSTOM | GVSP_PIX_BAYRG10_PACKED)
#define PF_BAYGB10_PACKED		(GVSP_PIX_CUSTOM | GVSP_PIX_BAYGB10_PACKED)
#define PF_BAYBG10_PACKED		(GVSP_PIX_CUSTOM | GVSP_PIX_BAYBG10_PACKED)
#define PF_BAYGR12_PACKED		(GVSP_PIX_CUSTOM | GVSP_PIX_BAYGR12_PACKED)
#define PF_BAYRG12_PACKED		(GVSP_PIX_CUSTOM | GVSP_PIX_BAYRG12_PACKED)
#define PF_BAYGB12_PACKED		(GVSP_PIX_CUSTOM | GVSP_PIX_BAYGB12_PACKED)
#define PF_BAYBG12_PACKED		(GVSP_PIX_CUSTOM | GVSP_PIX_BAYBG12_PACKED)

#define PF_BAYGR10_P			(GVSP_PIX_CUSTOM | PFNC_PIX_BAYGR10_PACKED)
#define PF_BAYRG10_P			(GVSP_PIX_CUSTOM | PFNC_PIX_BAYRG10_PACKED)
#define PF_BAYGB10_P			(GVSP_PIX_CUSTOM | PFNC_PIX_BAYGB10_PACKED)
#define PF_BAYBG10_P			(GVSP_PIX_CUSTOM | PFNC_PIX_BAYBG10_PACKED)
#define PF_BAYGR12_P			(GVSP_PIX_CUSTOM | PFNC_PIX_BAYGR12_PACKED)
#define PF_BAYRG12_P			(GVSP_PIX_CUSTOM | PFNC_PIX_BAYRG12_PACKED)
#define PF_BAYGB12_P			(GVSP_PIX_CUSTOM | PFNC_PIX_BAYGB12_PACKED)
#define PF_BAYBG12_P			(GVSP_PIX_CUSTOM | PFNC_PIX_BAYBG12_PACKED)
#define PF_BAYGR16				(GVSP_PIX_CUSTOM | GVSP_PIX_BAYGR16)
#define PF_BAYRG16				(GVSP_PIX_CUSTOM | GVSP_PIX_BAYRG16)
#define PF_BAYGB16				(GVSP_PIX_CUSTOM | GVSP_PIX_BAYGB16)
#define PF_BAYBG16				(GVSP_PIX_CUSTOM | GVSP_PIX_BAYBG16)

// 26.3 RGB Packed buffer format defines
#define PF_RGB8_PACKED			(GVSP_PIX_CUSTOM | GVSP_PIX_RGB8_PACKED)
#define PF_BGR8_PACKED			(GVSP_PIX_CUSTOM | GVSP_PIX_BGR8_PACKED)
#define PF_RGBA8_PACKED			(GVSP_PIX_CUSTOM | GVSP_PIX_RGBA8_PACKED)
#define PF_BGRA8_PACKED			(GVSP_PIX_CUSTOM | GVSP_PIX_BGRA8_PACKED)
#define PF_RGB10_PACKED			(GVSP_PIX_CUSTOM | GVSP_PIX_RGB10_PACKED)
#define PF_BGR10_PACKED			(GVSP_PIX_CUSTOM | GVSP_PIX_BGR10_PACKED)
#define PF_RGB12_PACKED			(GVSP_PIX_CUSTOM | GVSP_PIX_RGB12_PACKED)
#define PF_BGR12_PACKED			(GVSP_PIX_CUSTOM | GVSP_PIX_BGR12_PACKED)
#define PF_RGB16_PACKED			(GVSP_PIX_CUSTOM | GVSP_PIX_RGB16_PACKED)
#define PF_RGB10V1_PACKED		(GVSP_PIX_CUSTOM | GVSP_PIX_RGB10V1_PACKED)
#define PF_RGB10V2_PACKED		(GVSP_PIX_CUSTOM | GVSP_PIX_RGB10V2_PACKED)
#define PF_RGB12V1_PACKED		(GVSP_PIX_CUSTOM | GVSP_PIX_RGB12V1_PACKED)

// 26.4 YUV Packed buffer format defines
#define PF_YUV411_PACKED		(GVSP_PIX_CUSTOM | GVSP_PIX_YUV411_PACKED)
#define PF_YUV422_PACKED		(GVSP_PIX_CUSTOM | GVSP_PIX_YUV422_PACKED)
#define PF_YUV422_YUYV_PACKED	(GVSP_PIX_CUSTOM | GVSP_PIX_YUV422_YUYV_PACKED)
#define PF_YUV444_PACKED		(GVSP_PIX_CUSTOM | GVSP_PIX_YUV444_PACKED)
#define PF_YUV420P_NV12_PACKED	(GVSP_PIX_CUSTOM | GVSP_PIX_YUV420P_NV12_PACKED)	// Special YUV 420 semi-planar from TS-5000

// 26.5 RGB Planar buffer format defines
#define PF_RGB8_PLANAR			(GVSP_PIX_CUSTOM | GVSP_PIX_RGB8_PLANAR)
#define PF_RGB10_PLANAR			(GVSP_PIX_CUSTOM | GVSP_PIX_RGB10_PLANAR)
#define PF_RGB12_PLANAR			(GVSP_PIX_CUSTOM | GVSP_PIX_RGB12_PLANAR)
#define PF_RGB16_PLANAR			(GVSP_PIX_CUSTOM | GVSP_PIX_RGB16_PLANAR)

#define PF_RGBA8				(GVSP_PIX_CUSTOM | GVSP_PIX_RGB8A)

// Internal use only
#define	PF_BGR16_PACKED_INTERNAL	(GVSP_PIX_CUSTOM | GVSP_PIX_BGR16_PACKED_INTERNAL)

//---------------------------------------------------------------------------------------------------------------------------------------------------
/// \brief	Image information structure
typedef struct _tIMAGE_INFO {
	uint64_t	iPixelType;		///< Pixel Format Type
	uint32_t	iSizeX;			///< Image width
	uint32_t	iSizeY;			///< Image height
	uint32_t	iImageSize;		///< Number of bytes for image
	uint8_t		*pImageBuffer;	///< Buffer pointer
	uint64_t	iTimeStamp;		///< Timestamp , sek 2007/04/26 Added.
	uint32_t	iMissingPackets;	///< Number of missing packets.   // sek 2007/06/01 Added.
	uint32_t	iAnnouncedBuffers;	///< Number of announced buffers. // sek 2008/06/30 Added.
	uint32_t	iQueuedBuffers;		///< Number of queued buffers.    // sek 2008/06/30 Added.
	uint32_t	iOffsetX;		///< Image offset x
	uint32_t	iOffsetY;		///< Image offset y
	uint32_t	iAwaitDelivery;	///< Number of frames awaiting delivery. // sek 2009/0729 Added.
	uint64_t	iBlockID;		///< Block ID                            // sek 2009/07/29 Added.
} tIMAGE_INFO;

// R/G/B structure
typedef struct _tDIB24 {
	BYTE	B;
	BYTE	G;
	BYTE	R;
} tDIB24;

#define	tBGR24	tDIB24

// Alpha/R/G/B structure
typedef struct _tDIB32 {
	tDIB24	BGR;
	BYTE	Alpha;
} tDIB32;

// Structure of word three data
typedef struct _tBGR48 {
	uint16_t	B16;
	uint16_t	G16;
	uint16_t	R16;
} tBGR48;

typedef struct _tLUT_TYPE 
{
   uint32_t iLUTEntries;        ///< Number of LUT entries allocated. This will depend on the pixel depth!
   uint32_t iPixelDepth;        ///< Pixel depth! This can be 8-bit, 10-bit, 12-bit, 14-bit or 16-bit.
   uint32_t iColors;            ///< Number of colors. This is either 1 (monochrome) or 3 for Bayer and RGB
   void  *pLutR;                ///< Pointer to the actual LUT data for color 1 (Monochrome or Red)
   void  *pLutG;                ///< Pointer to the actual LUT data for color 2 (Green). This will be NULL for Monochrome LUT
   void  *pLutB;                ///< Pointer to the actual LUT data for color 3 (Blue). This will be NULL for Monochrome LUT
} tLUT;

typedef struct _tHIST_TYPE 
{
   uint32_t iHistEntries;       ///< Number of Histogram entries allocated. This will depend on the pixel depth!
   uint32_t iPixelDepth;        ///< Pixel depth! This can be 8-bit, 10-bit, 12-bit, 14-bit or 16-bit.
   uint32_t iColors;            ///< Number of colors. This is either 1 (monochrome) or 3 for Bayer and RGB
   void  *pHistR;               ///< Pointer to the actual Histogram data for color 1 (Monochrome or Red)
   void  *pHistG;               ///< Pointer to the actual Histogram data for color 2 (Green). This will be NULL for Monochrome LUT
   void  *pHistB;               ///< Pointer to the actual Histogram data for color 3 (Blue). This will be NULL for Monochrome LUT
} tHIST;

/// \brief Flip type used in \c Flip()
typedef enum _IMAGE_FLIP_TYPE
{
   FLIP_HORIZONTAL = 0,         //!< Flip Horizontally (Mirror)
   FLIP_VERTICAL = 1,           //!< Flip Vertically
} IMAGE_FLIP_TYPE;

/// \brief Rotate type used in \c Rotate()
typedef enum _IMAGE_ROTATE_TYPE
{
   ROTATE_90_DEG_CW = 0,        //!< Rotate 90 degree clockwise
   ROTATE_90_DEG_CCW = 1,       //!< Rotate 90 degree counter-clockwise
   ROTATE_180_DEG = 2,          //!< Rotate 180 degree (upside-down)
} IMAGE_ROTATE_TYPE;

/// \brief Image processing type used in \c vlImageProcessing()
typedef enum _IMAGE_PROCESS_TYPE
{
	IP_RED_COMPENSATION = 0,			//!< Red compensation
	IP_RED_COMPENSATION_MULTI,			//!< Red compensation multi-processor
	IP_GREEN_COMPENSATION,				//!< Green compensation
	IP_GREEN_COMPENSATION_MULTI,		//!< Green compensation multi-processor
	IP_COLOR_NOISE_REDUCTION,			//!< Color noise reduction
	IP_COLOR_NOISE_REDUCTION_MULTI,		//!< Color noise reduction multi-processor
	IP_CONVOLUTION,						//!< Convolution
	IP_CONVOLUTION_MULTI,				//!< Convolution multi-processor
	IP_PSEUDO_COLOR,					//!< Pseudo color
	IP_PSEUDO_COLOR_MULTI,				//!< Pseudo color multi-processor
	IP_LENS_DISTORTION,					//!< Lens distortion
	IP_LENS_DISTORTION_MULTI,			//!< Lens distortion multi-processor
	IP_LENS_DISTORTION_MAP_INIT,		//!< Prepare internal Lens distortion map (speed optimization)
	IP_LENS_DISTORTION_MAP_FREE,		//!< Free internal Lens distortion map (speed optimization)
} IFC_IMAGE_PROCESSING;

typedef struct _tCOLOR_COMPEN_ROI
{
	RECT		RectOfROI;
	uint32_t	BThreshold;
	uint32_t	GThreshold;
	uint32_t	RThreshold;
	uint32_t	BGain;	// 4096:1.00
	uint32_t	GGain;	// 4096:1.00
	uint32_t	RGain;	// 4096:1.00
} tCOLOR_COMPEN_ROI;

typedef struct tCOMPEN_PARAM
{
	uint32_t	NumOfROI;
	tCOLOR_COMPEN_ROI pCompenParams[10];
} tCOLOR_COMPEN;

/// \brief Rotate type used in \c tUNDISTORT_PARAMS
typedef enum _INTERPOLATION_TYPE
{
   T_INTERPOLATION_NONE = 0,      //!< No interpolation. Closest pixel is used instead
   T_INTERPOLATION_BILINEAR = 1,  //!< Bi-linear interpolation is used
} INTERPOLATION_TYPE;

typedef struct tUNDISTORT_MAP
{
    int    iImageWidth;         ///< Width of the input image
    int    iImageHeight;        ///< Height of the input image
    uint32_t iSecretKey;        ///< Integer used for checking if the map data has been initialized yet!
    double*  pMapDataX;         ///< Pointer to the actual map data for X
    double*  pMapDataY;         ///< Pointer to the actual map data for Y
} tUNDISTORT_MAP,*ptUNDISTORT_MAP; 

/// \brief Struct containing parameters for lens distortion correction.
typedef struct tUNDISTORT_PARAMS
{
    double dOpticalCenterX;     ///< Optical center X used to model possible displacement from optical axis
    double dOpticalCenterY;     ///< Optical center Y used to model possible displacement from optical axis
    double dFocalLengthX;       ///< Focal length X used for normalizing the lens distortion calculation. This is typically a value close to the Width of the image in pixels
    double dFocalLengthY;       ///< Focal length Y used for normalizing the lens distortion calculation. If the pixels on the imager are square then this is the same as dFocalLengthX
    double dCoeffK1;            ///< Radial Distortion coefficient K1 for r^2 in Taylor series expansion around r=0: Xcorr = Xin(1+k1*r^2+k2*r^4+k3*r^6)
    double dCoeffK2;            ///< Radial Distortion coefficient K2
    double dCoeffK3;            ///< Radial Distortion coefficient K3 (normally set to 0 unless it is a fish-eye lens)
    double dCoeffP1;            ///< Tangential Distortion coefficient P1. Xcorr=X+[2*p1*Y+p2*(r^2+2*X^2)], Ycorr=Y+[p1*(r^2+2*y^2)+2*p2*X]
    double dCoeffP2;            ///< Tangential Distortion coefficient P2
    uint32_t iInterpolationType;  ///< Type of interpolation used for the lens distortion. Specified as one of the \c INTERPOLATION_TYPE enumeration values
    ptUNDISTORT_MAP pMap;       ///< Pointer to Map stucture used for speed optimization
} tUNDISTORT_PARAMS; 

#define bitsperpixel(x) (((uint32_t)x & GVSP_PIX_EFFECTIVE_PIXEL_SIZE_MASK) >>(uint32_t)GVSP_PIX_EFFECTIVE_PIXEL_SIZE_SHIFT)

class CDummyClass
{

};

typedef CDummyClass * GVSP_CALLBACK_OBJECT;

// New image callback declaration
typedef void (CDummyClass::*GVSP_CALLBACK_FUNCTION)(tIMAGE_INFO * pAqImageInfo);
// kic 2007/04/20 added
typedef void (__stdcall *STATIC_GVSP_CALLBACK_FUNCTION)(tIMAGE_INFO * pAqImageInfo);

// Image View Window notification callback declaration
typedef CDummyClass * IVW_CALLBACK_OBJECT;
typedef void (CDummyClass::*IVW_CALLBACK_FUNCTION)(VW_HANDLE hWin, IVW_CALLBACK_TYPE Type, void *pValue);
typedef void (__stdcall *STATIC_IVW_CALLBACK_FUNCTION)(VW_HANDLE hWin, IVW_CALLBACK_TYPE Type, void *pValue);

#ifdef __cplusplus
extern "C" {
#endif

using namespace GenICam::Client;


namespace GenICam
{
	namespace VL
	{

//---------------------------------------------------------------------------------------------------------------------------------------------------
		/// @}
		/// \addtogroup VWMAN	View Window Management
		/// @{
//---------------------------------------------------------------------------------------------------------------------------------------------------

        #ifdef UNICODE
        #define vlOpenViewWindow  vlOpenViewWindowW
        #else
        #define vlOpenViewWindow  vlOpenViewWindowA
        #endif // !UNICODE


		/// \brief	Open View Window (Create)
		/// \brief	The window of size pFrameRect of window name pWindowName is made, and size pMaxSize view window is made for the client area.
		/// \brief	At this time, the size of the window displayed in the first stage is specified for pFrameRect, and the assumed maximum image size is specified for pViewRect. 

		/// \brief	Therefore, it usually becomes
		/// \brief	pFrameRect<=*pMaxSize. 
		///
		///	\param	pWindowName		[in]	Window name
		///	\param	*pFrameRect		[in]	Externals size of frame window(absolute coordinate).
		///	\param	*pMaxSize		[in]	Maximum size of image data.
		/// \param  *pParent        [in]    Pointer to parent window.
		///	\param	*pWin			[out]	The handle of the created window (window object) is returned.
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlOpenViewWindowA(IVW_WINDOW_TYPE iWindowType, LPCSTR pWindowName, RECT *pFrameRect, SIZE *pMaxSize, HWND hParent, VW_HANDLE *pWin);

		/// \brief	Open View Window (Create)
		/// \brief	The window of size pFrameRect of window name pWindowName is made, and size pMaxSize view window is made for the client area.
		/// \brief	At this time, the size of the window displayed in the first stage is specified for pFrameRect, and the assumed maximum image size is specified for pViewRect. 

		/// \brief	Therefore, it usually becomes
		/// \brief	pFrameRect<=*pMaxSize. 
		///
		///	\param	pWindowName		[in]	Window name
		///	\param	*pFrameRect		[in]	Externals size of frame window(absolute coordinate).
		///	\param	*pMaxSize		[in]	Maximum size of image data.
		/// \param  *pParent        [in]    Pointer to parent window.
		///	\param	*pWin			[out]	The handle of the created window (window object) is returned.
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlOpenViewWindowW(IVW_WINDOW_TYPE iWindowType, LPCWSTR pWindowName, RECT *pFrameRect, SIZE *pMaxSize, HWND hParent, VW_HANDLE *pWin);

        #ifdef UNICODE
        #define vlOpenViewWindowAuto  vlOpenViewWindowAutoW
        #else
        #define vlOpenViewWindowAuto  vlOpenViewWindowAutoA
        #endif // !UNICODE

		/// \brief	Open View Window (Create)
		/// \brief	The window of size pFrameRect of window name pWindowName is made.
		/// \brief	At this time, the size of the window displayed in the first stage is specified for pFrameRect. 
		///
		///	\param	pWindowName		[in]	Window name
		///	\param	*pPoint			[in]	Upper left position of frame window(absolute coordinate).
		///	\param	*pMaxSize		[in]	Maximum size of image data.
		///	\param	*pWin			[out]	The handle of the created window (window object) is returned.
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlOpenViewWindowAutoA(LPCSTR pWindowName, POINT *pPoint, SIZE *pMaxSize, VW_HANDLE *pWin);

		/// \brief	Open View Window (Create)
		/// \brief	The window of size pFrameRect of window name pWindowName is made.
		/// \brief	At this time, the size of the window displayed in the first stage is specified for pFrameRect. 
		///
		///	\param	pWindowName		[in]	Window name
		///	\param	*pPoint			[in]	Upper left position of frame window(absolute coordinate).
		///	\param	*pMaxSize		[in]	Maximum size of image data.
		///	\param	*pWin			[out]	The handle of the created window (window object) is returned.
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlOpenViewWindowAutoW(LPCTSTR pWindowName, POINT *pPoint, SIZE *pMaxSize, VW_HANDLE *pWin);


		/// \brief	Close view window.(Destroy it)
		/// \brief	The window of handle hWin that vlOpenViewWindow returned is destroyed.
		///
		///	\param	hWin			[in]		handle that vlOpenViewWindow returned.
		///	\return 
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlCloseViewWindow(VW_HANDLE hWin);


		/// \brief	Move view window.
		/// \brief	The window of handle hWin made with vlOpenViewWindow is moved to the position of pPoint.
		/// \brief	In pPoint, the absolute coordinate in the upper left corner of the window is specified.
		///
		///	\param	hWin			[in]		Handle that vlOpenViewWindow returned.
		///	\param	*pPoint			[in]		Upper left position of view window.
		///	\return 
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlMoveViewWindow(VW_HANDLE hWin, POINT *pPoint);


		/// \brief	Get rectangular coordinates of the Client area of View Window. 
		/// \brief	Get rectangular coordinates of the window of handle hWin made with vlOpenViewWindow. 
		/// \brief	Coordinates of a present Client window in four corners are returned to pRect.
		///
		///	\param	hWin			[in]		Handle that vlOpenViewWindow returned.
		///	\param	*pRect			[out]		Present window coordinates.
		///	\return 
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlGetViewWindowRect(VW_HANDLE hWin, RECT *pRect);


		/// \brief	Get externals of View Window rectangular coordinates. 
		/// \brief	Get external rectangular coordinates of the window of handle hWin made with vlOpenViewWindow. 
		/// \brief	Coordinates of a present Client window in four corners are returned to pRect.
		///
		///	\param	hWin			[in]		Handle that vlOpenViewWindow returned.
		///	\param	*pRect			[out]		Present window coordinates.
		///	\return 
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlGetFrameWindowRect(VW_HANDLE hWin, RECT *pRect);


        #ifdef UNICODE
        #define vlSetViewWindowTitle  vlSetViewWindowTitleW
        #else
        #define vlSetViewWindowTitle  vlSetViewWindowTitleA
        #endif // !UNICODE

		/// \brief	Set the title of View Window.
		/// \brief	Display pWindowTitle in the title bar of the window of handle hWin made with vlOpenViewWindow. 
		///
		///	\param	hWin			[in]		Handle that vlOpenViewWindow returned.
		///	\param	*pWindowTitle	[in]	    Character string that wants to be displayed in title bar.
		///	\return 
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlSetViewWindowTitleA(VW_HANDLE hWin, LPCSTR pWindowTitle);

		/// \brief	Set the title of View Window.
		/// \brief	Display pWindowTitle in the title bar of the window of handle hWin made with vlOpenViewWindow. 
		///
		///	\param	hWin			[in]		Handle that vlOpenViewWindow returned.
		///	\param	*pWindowTitle	[in]	    Character string that wants to be displayed in title bar.
		///	\return 
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlSetViewWindowTitleW(VW_HANDLE hWin, LPCWSTR pWindowTitle);

		/// \brief	Display the image. 
		/// \brief	Convert the RAW image specified with pAqImageInfo into DIB, and display it in the view window in the window of handle hWin made with vlOpenViewWindow. 
		///
		///	\param	hWin			[in]		Handle that vlOpenViewWindow returned.
		///	\param	*pAqImageInfo	[in]        Pointer of tIMAGE_INFO structure with information on RAW image.
		///	\return 
		// sek 2007/02/22 Adds parameters for bayer gain control.
		// sek 2007/02/27 Adds iGGain32 parameter.
//		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlShowImage(VW_HANDLE hWin, tIMAGE_INFO * pAqImageInfo);
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlShowImage(VW_HANDLE hWin, tIMAGE_INFO * pAqImageInfo, uint32_t iRGain=0x01000, uint32_t iGGain=0x01000, uint32_t iBGain=0x01000);

		// sek 2008/05/19 Added New Function.
		/// \brief	Display the image with Extended Bayer Interpolation. 
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlShowImageEx(VW_HANDLE hWin, tIMAGE_INFO * pAqImageInfo, IFC_BAYER_ALGORITHM iBayerAlgorithm = IA_BAYER_STANDARD, uint32_t iRGain=0x01000, uint32_t iGGain=0x01000, uint32_t iBGain=0x01000);


		/// \brief	Set image offset and zoom ratio.
		/// \brief	Set the offset and the zoom ratio of the view image. 
		///
		///	\param	hWin			[in]		handle that vlOpenViewWindow returned.
		///	\param	*pOffset		[in]	    POINT structure pointer with the horizontal, vertical offset (number of pixels).
		/// \param	ZoomRatio		[in]	    Zoom ratio.
		/// 								    100:The original image is reduced to the window size.
		///									    1  :1/100 of the original images is expanded to the window size. 
		///	\return 
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlSetImageOffset(VW_HANDLE hWin, POINT *pOffset, int32_t ZoomRatio);

		/// \brief	Set image offset and zoom ratio.
		/// \brief	Set the offset and the zoom ratio of the view image. 
		///
		///	\param	hWin			[in]		handle that vlOpenViewWindow returned.
		///	\param	*pOffset		[in]	    POINT structure pointer with the horizontal, vertical offset (number of pixels).
		/// \param	ZoomRatioX		[in]	    Zoom ratio in X-direction.
		/// \param	ZoomRatioY		[in]	    Zoom ratio in X-direction.
		/// 								    100.0:The original image is reduced to the window size.
		///									    1.0  :1/100 of the original images is expanded to the window size. 
		///	\return 
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlSetImageOffsetEx(VW_HANDLE hWin, POINT *pOffset, double ZoomRatioX, double ZoomRatioY);

		/// \brief	Set image offset and zoom ratio.
		/// \brief	Set the offset and the zoom ratio of the view image. 
		///
		///	\param	hWin			[in]		handle that vlOpenViewWindow returned.
		///	\param	*pOffset		[in]	    POINT structure pointer with the horizontal, vertical offset (number of pixels).
		/// \param	pdZoomRatioX	[out]	    Pointer to variable to get Zoom ratio in X-direction.
		/// \param	pdZoomRatioY	[out]	    Pointer to variable to get Zoom ratio in Y-direction.
		/// 								    100.0:The original image is reduced to the window size.
		///									    1.0  :1/100 of the original images is expanded to the window size. 
		///	\return 
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlGetImageOffsetEx(VW_HANDLE hWin, POINT *pOffset, double *pdZoomRatioX, double *pdZoomRatioY);

		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlSetGain(HANDLE hThread, uint32_t iRGain, uint32_t iGGain, uint32_t iBGain);

		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlGetGain(HANDLE hThread, uint32_t * piRGain, uint32_t * piGGain, uint32_t * piBGain);

		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlExecuteWhiteBalance(SP_HANDLE hThread);


		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlResizeChildWindow(VW_HANDLE hWin, RECT *pRect);

		/// \brief	Set View Window options.
		/// \brief	Set an option for the window of handle hWin made with vlOpenViewWindow. 
		///
		///	\param	hWin            [in]		Handle that vlOpenViewWindow returned.
		///	\param	Option	        [in]	    Option to set. Has to be one of the IVW_OPTION enum types.
        ///	\param	pOptionValue    [in]        Pointer to the option value. The type is depending on which option to set
		///	\return 
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlSetViewWindowOption(VW_HANDLE hWin, IVW_OPTION Option, void* pOptionValue);

		/// \brief	Get View Window options.
		/// \brief	Get the value of an option for the window of handle hWin made with vlOpenViewWindow. 
		///
		///	\param	hWin            [in]		Handle that vlOpenViewWindow returned.
		///	\param	Option	        [in]	    Option to get. Has to be one of the IVW_OPTION enum types.
        ///	\param	pOptionValue    [in]        Pointer to the option value. The type is depending on which option to set
		///	\return 
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlGetViewWindowOption(VW_HANDLE hWin, IVW_OPTION Option, void* pOptionValue);

        EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlSetViewWindowCallback(VW_HANDLE hWin, IVW_CALLBACK_OBJECT CBObject, IVW_CALLBACK_FUNCTION CBFunction);

//---------------------------------------------------------------------------------------------------------------------------------------------------
		/// @}
		/// \addtogroup IMCNV	Image Conversion
		/// @{
//---------------------------------------------------------------------------------------------------------------------------------------------------

		/// \brief	Convert from RAW to DIB.
		/// \brief	Convert the RAW image into DIB.
		/// \brief	Convert the RAW image specified with pAqImageInfo into 32bit aRGB, and store it in the area specified with pDIB. 
		///
		///	\param	*pAqImageInfo	[in]	Pointer of tIMAGE_INFO structure with information on RAW data.
		///	\param	*pBufferInfo	[in]	Pointer of tIMAGE_INFO structure in which information on converted image is written.
		/// \param	iRGain			[in]	R Gain for Bayer (0x01000(4096) = 1.00)
		/// \param	iGGain			[in]	G Gain for Bayer (0x01000(4096) = 1.00)
		/// \param	iBGain			[in]	B Gain for Bayer (0x01000(4096) = 1.00)
		///	\return 
		// sek 2007/02/22 Adds parameters for bayer gain control.
		// sek 2007/02/27 Adds iGGain parameter.
//		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlFromRAWtoDIB(tIMAGE_INFO * pAqImageInfo, tIMAGE_INFO * pBufferInfo);
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlFromRAWtoDIB(tIMAGE_INFO * pAqImageInfo, tIMAGE_INFO * pBufferInfo, uint32_t iRGain=0x01000, uint32_t iGGain=0x01000, uint32_t iBGain=0x01000);

		// sek 2008/05/19 Added New Function.
		/// \brief	Convert from RAW to DIB with Extended Bayer Interpolation.
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlFromRAWtoDIBEx(tIMAGE_INFO * pAqImageInfo, tIMAGE_INFO * pBufferInfo, IFC_BAYER_ALGORITHM iBayerAlgorithm = IA_BAYER_STANDARD, uint32_t iRGain=0x01000, uint32_t iGGain=0x01000, uint32_t iBGain=0x01000);

		/// \brief	Allocate the buffer memory for the image.
		/// \brief	Allocate the buffer to store the converted image.
		/// \brief	Information on the allocated buffer is returned to pImageInfo.
		///
		///	\param	*pAqImageInfo	[in]	Pointer of tIMAGE_INFO structure with information on RAW data.
		///	\param	*pBufferInfo	[out]	Pointer of tIMAGE_INFO structure that information on allocated buffer is returned.
		///	\return 
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlMalloc(tIMAGE_INFO * pAqImageInfo, tIMAGE_INFO * pBufferInfo);


		/// \brief	Allocate the buffer memory for the DIB.
		/// \brief	Allocate the buffer to store the converted image.
		/// \brief	Information on the allocated buffer is returned to pImageInfo.
		///
		///	\param	*pAqImageInfo	[in]	Pointer of tIMAGE_INFO structure with information on RAW data.
		///	\param	*pBufferInfo	[out]	Pointer of tIMAGE_INFO structure that information on allocated buffer is returned.
		///	\return 
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlMallocDIB(tIMAGE_INFO * pAqImageInfo, tIMAGE_INFO * pBufferInfo);


		/// \brief	Destruct the buffer memory.
		/// \brief	Destruct the buffer allocated with vlMalloc. 
		///
		///	\param	*pBufferInfo	[in]	Pointer of tIMAGE_INFO structure with information on buffer that should be destructed.
		///	\return 
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlFree(tIMAGE_INFO * pBufferInfo);


		/// \brief	Convert from RAW to an effective all bit image.
		/// \brief	Convert the RAW image specified with pAqImageInfo by the following rule, and store it in the area specified with pBufferInfo->pImageBuffer.
		/// \brief	Information on the converted image is returned to pBufferInfo.
		/// \brief	 8bpp Grayscale ->  8bpp Grayscale
		/// \brief	10bpp Grayscale -> 16bpp Grayscale
		/// \brief	24bpp RGB       -> 24bpp RGB
		/// \brief	30bpp RGB       -> 48bpp RGB
		/// \brief	 8bpp Bayer     -> 24bpp RGB
		/// \brief	10bpp Bayer     -> 48bpp RGB
		///
		///	\param	*pAqImageInfo	[in]	Pointer of tIMAGE_INFO structure with information on RAW data.
		///	\param	*pBufferInfo	[in,out]	Pointer of tIMAGE_INFO structure in which information on converted image is written.
		/// \param	iRGain			[in]	R Gain for Bayer (0x01000(4096) = 1.00)
		/// \param	iGGain			[in]	G Gain for Bayer (0x01000(4096) = 1.00)
		/// \param	iBGain			[in]	B Gain for Bayer (0x01000(4096) = 1.00)
		///	\return 
		// sek 2007/02/22 Adds parameters for bayer gain control.
		// sek 2007/02/27 Adds iGGain parameter.
		//EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlFromRAWtoImage(tIMAGE_INFO * pAqImageInfo, tIMAGE_INFO * pBufferInfo);
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlFromRAWtoImage(tIMAGE_INFO * pAqImageInfo, tIMAGE_INFO * pBufferInfo, uint32_t iRGain=0x01000, uint32_t iGGain=0x01000, uint32_t iBGain=0x01000);

		// sek 2008/05/19 Added New Function.
		/// \brief	Convert from RAW to an effective all bit image with Extended Bayer Interpolation.
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlFromRAWtoImageEx(tIMAGE_INFO * pAqImageInfo, tIMAGE_INFO * pBufferInfo, IFC_BAYER_ALGORITHM iBayerAlgorithm = IA_BAYER_STANDARD, uint32_t iRGain=0x01000, uint32_t iGGain=0x01000, uint32_t iBGain=0x01000);

		// sek 2008/05/19 Added New Function.
		/// \brief	Convert from full-bit image to an specified image.
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlConvertImage(tIMAGE_INFO *pInputBufferInfo, tIMAGE_INFO *pOutputBufferInfo, IFC_PIXEL_FORMAT iOutputFormat);

		// sek 2008/05/19 Added New Function.
		/// \brief			/// \brief	Allocate the buffer memory for specified image.
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlMallocEx(tIMAGE_INFO *pInputBufferInfo, tIMAGE_INFO *pOutputBufferInfo, IFC_PIXEL_FORMAT iOutputFormat);

		/// \brief   This function allows the user to explicitly enable or disable parallel processing for the FromRAWtoImageEx and FromRAWtoDIBEx functions.
		/// If the function is not called, the behavior is the same as previous versions: passing in one of the IFC_BAYER_ALGORITHM with "_MULTI" appended
		/// to the name will use multi-core processing if available. Note that this caused confusion, because the parallel processing mode for MONO images 
		/// was also set by the passed in IFC_BAYER_ALGORITHM value.  Now the user can explicitly select the parallel processing mode for all image types
		/// (for the FromRAWtoImageEx and FromRAWtoDIBEx functions only) by calling vlEnableParallelProcessing.
		/// 		
		/// \param [in] processingMode One of the following values:
		/// IA_PARALLEL_PROCESSING_DEFAULT = -1,	
		/// IA_PARALLEL_PROCESSING_DISABLED = 0,	
		/// IA_PARALLEL_PROCESSING_ENABLED = 1,
		/// \retval none.
		EXTERN_C VL_IMPORT_EXPORT void VL_CALLTYPE  vlEnableParallelProcessing(IFC_PARALLEL_PROCESSING processingMode);

//---------------------------------------------------------------------------------------------------------------------------------------------------
		/// @}
		/// \addtogroup IMFILE	Image File
		/// @{
//---------------------------------------------------------------------------------------------------------------------------------------------------

        #ifdef UNICODE
        #define vlStoreToFile  vlStoreToFileW
        #else
        #define vlStoreToFile  vlStoreToFileA
        #endif // !UNICODE


		/// \brief	Preserve an effective all bit image in the TIFF file.
		/// \brief	Preserve it in the TIFF file for which the image specified with pImageInfo is specified with pPath.
		///
		///	\param	*pBufferInfo	[in]	Pointer of tIMAGE_INFO structure with information on converted image.
		///	\param	*pPath			[in]	Full Path of preserved file.
		///	\return 
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlStoreToFileA(tIMAGE_INFO * pBufferInfo, LPCSTR pPath);

		/// \brief	Preserve an effective all bit image in the TIFF file.
		/// \brief	Preserve it in the TIFF file for which the image specified with pImageInfo is specified with pPath.
		///
		///	\param	*pBufferInfo	[in]	Pointer of tIMAGE_INFO structure with information on converted image.
		///	\param	*pPath			[in]	Full Path of preserved file.
		///	\return 
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlStoreToFileW(tIMAGE_INFO * pBufferInfo, LPCWSTR pPath);

        #ifdef UNICODE
        #define vlStoreToFileEx  vlStoreToFileExW
        #else
        #define vlStoreToFileEx  vlStoreToFileExA
        #endif // !UNICODE

		// sek 2008/05/28 Added New Function.
		/// \brief	Preserve an image in the specified file.
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlStoreToFileExA(tIMAGE_INFO * pBufferInfo, LPCSTR pPath, SIF_FILE_FORMAT iFileFormat = FF_TIFF, uint8_t iJpegQuality = 75);

		/// \brief	Preserve an image in the specified file.
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlStoreToFileExW(tIMAGE_INFO * pBufferInfo, LPCWSTR pPath, SIF_FILE_FORMAT iFileFormat = FF_TIFF, uint8_t iJpegQuality = 75);

//---------------------------------------------------------------------------------------------------------------------------------------------------
		/// @}
		/// \addtogroup IMMAN	Image Manipulation
		/// @{
//---------------------------------------------------------------------------------------------------------------------------------------------------

		/// \brief	Get the pixel at a specified position.
		/// \brief	Get the pixel of position pPoint in the image data and return it to pPixel.
		///
		///	\param	*pBufferInfo	[in]	Pointer of tIMAGE_INFO structure with information on converted image.
		///	\param	pPoint			[in]	Position in which pixel is read.
		///	\param	pPixel			[out]	Pointer of variable or structure that receives pixel data.
		///	\param							It changes from 8bit(uint8_t) to 48bit(tBGR48) by PIXEL FORMAT TYPE.
		///	\return 
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlGetPixel(tIMAGE_INFO * pBufferInfo, POINT *pPoint, void * pPixel);

		/// \brief	Get the pixel at a specified position.
		/// \brief	Get the pixel of position pPoint in the image data and return it to pPixel.
		///
		///	\param	*pBufferInfo	[in]	Pointer of tIMAGE_INFO structure with information on converted image.
		///	\param	pPoint			[in]	Position in which pixel is read.
		///	\param	pPixel			[out]	Pointer of variable or structure that receives pixel data.
		///	\param							It changes from 8bit(uint8_t) to 48bit(tBGR48) by PIXEL FORMAT TYPE.
		///	\return 
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlGetPixelEx(tIMAGE_INFO * pBufferInfo, double dXPos, double dYPos, void * pPixel);


		/// \brief	Set the pixel at a specified position.
		/// \brief	Set the pixel of position pPoint in the image data.
		///
		///	\param	pBufferInfo		[in]	Pointer of tIMAGE_INFO structure with information on converted image.
		///	\param	pPoint			[in]	Position in which pixel is write.
		///	\param	pPixel			[in]	Pointer of variable or structure that passes pixel written data.
		///	\param							It changes from 8bit(uint8_t) to 48bit(tBGR48) by PIXEL FORMAT TYPE.
		///	\return 
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlSetPixel(tIMAGE_INFO * pBufferInfo, POINT *pPoint, void * pPixel);

		/// \brief	Measures pixel mean value.
		///
		///	\param	pBufferInfo		[in]	Pointer of tIMAGE_INFO structure with information on converted image.
		///	\param	pMeasureRect	[in]	Pointer of measured rectangular coordinates.
		///	\param	pRGBAverage		[out]	Pointer of easured data.
		///	\return 
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE   vlGetAverage(tIMAGE_INFO * pBufferInfo, RECT *pMeasureRect, tBGR48 *pRGBAverage);

//---------------------------------------------------------------------------------------------------------------------------------------------------
		/// @}
		/// \addtogroup	SPThread	Stream Processing Thread
		/// @{
//---------------------------------------------------------------------------------------------------------------------------------------------------
		/// \brief	Begin the Stream processing.ÅB
		/// \brief	Begin Stream processing Thread. And, call pCallBack if it received Frame.
		///
		///	\param	hDev			[in]	Handle of device that opens Stream.
		///	\param	iChannel		[in]	Stream Channel
		/// \param	CBObject		[in]	Class object that belongs CallBack function.
		/// \param	CBFunction		[in]	CallBack function.
		///	\param	phThread		[in/out]	Pointer of HANDLE that receives thread handle.
		///	\return 
		// sek 2007/02/21 Mod for multicast
//		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlOpenStream(DEV_HANDLE hDev, uint32_t iChannel, GVSP_CALLBACK_OBJECT CBObject, GVSP_CALLBACK_FUNCTION CBFunction, SP_HANDLE * phThread, uint32_t iBufferSize=(XGA_WIDTH*XGA_HEIGHT*MAX_BPP));
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlOpenStream(DEV_HANDLE hDev, uint32_t iChannel, GVSP_CALLBACK_OBJECT CBObject, GVSP_CALLBACK_FUNCTION CBFunction, SP_HANDLE * phThread, uint32_t iBufferSize=(XGA_WIDTH*XGA_HEIGHT*MAX_BPP), DWORD iMcIP=0);

		/// \brief	Terminate the stream processing.
		/// \brief	Terminate the stream processing thread, and annul thread.
		///
		///	\param	hThread			[in]	Handle of the thread.
		///	\return 
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlCloseStream(SP_HANDLE hThread);

//---------------------------------------------------------------------------------------------------------------------------------------------------
		/// @}
		/// \addtogroup	DeviceMan	Device Manager
		/// @{
//---------------------------------------------------------------------------------------------------------------------------------------------------
		// sek 2007/02/21 Added
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlOpenDeviceMan(DM_HANDLE *phDM, uint32_t * piNumDevices);

		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlCloseDeviceMan(DM_HANDLE hDM);

		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlOpenDevice(DM_HANDLE hDM, uint32_t iDevIndex, DEV_HANDLE *p_hDev, PORT_HANDLE *p_hPort, DEVICE_ACCESS_FLAGS iOpenFlags=GenICam::Client::DEVICE_ACCESS_CONTROL, DWORD iMcIP=0);

		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlCloseDevice(DM_HANDLE hDM, DEV_HANDLE hDev, PORT_HANDLE hPort);

		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlGetDeviceName(DM_HANDLE hDM, uint32_t iDevIndex, char * pDevName);

		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlGetInterfaceName(DM_HANDLE hDM, uint32_t iDevIndex, char * pIFName);

		// sek 2007/08/27 Added.
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlGetStreamHandle(SP_HANDLE hThread, DS_HANDLE * p_hDS);

        #ifdef UNICODE
        #define vlStoreToFileRAW  vlStoreToFileRAWW
        #else
        #define vlStoreToFileRAW  vlStoreToFileRAWA
        #endif // !UNICODE


		// sek 2007/08/29 Added.
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlStoreToFileRAWA(tIMAGE_INFO * pBufferInfo, LPCSTR pPath);

		// sek 2007/08/29 Added.
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlStoreToFileRAWW(tIMAGE_INFO * pBufferInfo, LPCWSTR pPath);

		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlLoadFromFileRAWA(tIMAGE_INFO * pBufferInfo, LPCSTR pPath);
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlLoadFromFileRAWW(tIMAGE_INFO * pBufferInfo, LPCWSTR pPath);

        // mat 2007/09/07 Added.
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	 vlGetStreamInfo(SP_HANDLE hThread, STREAM_INFO_CMD iCmd, void *pBuffer, size_t *iSize);

//---------------------------------------------------------------------------------------------------------------------------------------------------
		/// @}
		/// \addtogroup	HDR
		/// @{
//---------------------------------------------------------------------------------------------------------------------------------------------------
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	vlMallocHDR(tIMAGE_INFO * pImageInfo1, tIMAGE_INFO * pImageInfo2, tIMAGE_INFO * pBufferInfo);

		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	vlFreeHDR(tIMAGE_INFO * pBufferInfo);

		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	vlHDRAnalyzeImages(tIMAGE_INFO * pImageInfo1, tIMAGE_INFO * pImageInfo2, uint8_t* pImage1IsBrighter, int32_t *pBlackLevelImage1, int32_t *pBlackLevelImage2, float* pDarkGain);

		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	vlCombineImagesHDR(tIMAGE_INFO* pBrighterImageInfo, tIMAGE_INFO* pDarkerImageInfo, tIMAGE_INFO* pOutputBufferInfo, int32_t brighterBlackLevel, int32_t darkerBlackLevel, float fDarkGain, float fDualSlopeGain, uint8_t logOutput);

//---------------------------------------------------------------------------------------------------------------------------------------------------
		/// @}
		/// \addtogroup	LUT
		/// @{
//---------------------------------------------------------------------------------------------------------------------------------------------------

        //******************************************************************************************************************
        /// \brief   Allocate the buffer memory for the LUT based on the pixel format read from the image buffer.
        /// 
        /// \par
        /// The LUT will be initialized and the LUT memory will be allocated based on the pixel format read from the image.
        /// The allocated LUT will need to be freed up using \c vlFreeLUT()
        /// \note The input images needs to be Mono8, Mono10, Mono12, Mono14 or Mono16 pixel format.
        ///
        /// \param [in] pImageInfo     Pointer to tIMAGE_INFO structure with information on the image data.
        /// \param [in] pLutInfo       Pointer to tLUT structure with information on LUT.
        /// \retval                    Status code defined in the VL_ERROR. If the function succeeds, returns VL_ERR_SUCCESS.
        EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	vlMallocLUT(tIMAGE_INFO* pImageInfo, tLUT* pLutInfo);

        //******************************************************************************************************************
        /// \brief   Free the LUT memory previously allocated using \c vlMallocLUT().
        /// 
        /// \param [in] pLutInfo       Pointer to tLUT structure with information on LUT.
        /// \retval                    Status code defined in the VL_ERROR. If the function succeeds, returns VL_ERR_SUCCESS.
        EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	vlFreeLUT(tLUT* pLutInfo);

        //******************************************************************************************************************
        /// \brief   Convert an existing LUT into a different size and copy the data.
        /// 
        /// \param [in] pSourceLutInfo      Pointer to Source tLUT structure with information on LUT.
        /// \param [in] pDestinationLutInfo Pointer to Source tLUT structure with information on LUT.
        /// \retval                         Status code defined in the VL_ERROR. If the function succeeds, returns VL_ERR_SUCCESS.
        EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	vlConvertLUT(tLUT* pSourceLutInfo, tLUT* pDestinationLutInfo);

        //******************************************************************************************************************
        /// \brief   Process the image by sending all pixels through the LUT.
        /// 
        /// \par
        /// All the pixels values in the image will be replaced with the corresponding value from the LUT.
        /// \note The input images needs to be Mono8, Mono10, Mono12, Mono14 or Mono16 pixel format and the LUT has to be created
        /// using \c vlMallocLUT() from an image with the identical pixel format so the LUT has the correct dimension.
        ///
        /// \param [in] pImageInfo     Pointer to tIMAGE_INFO structure with the image data.
        /// \param [in] pLutInfo       Pointer to tLUT structure with the LUT.
        /// \retval                    Status code defined in the VL_ERROR. If the function succeeds, returns VL_ERR_SUCCESS.
        EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	vlLUT(tIMAGE_INFO* pImageInfo, tLUT* pLutInfo);

        //******************************************************************************************************************
        /// \brief   This function creates a user-defined Gamma LUT and fills in the values to a LUT that has previously been 
        /// allocated using \c vlMallocLUT().
        /// 
        /// \note The LUT has to be created using \c vlMallocLUT() before this function can be called.
        ///
        /// \param [in] pLutInfo       Pointer to tLUT structure with information on LUT.
        /// \param [in] ColorIndex     Select the LUT color index to create. 0=Monochrome or Red, 1=Green, 2=Blue
        /// \param [in] gamma          Gamma value to be used for the creation of the LUT entries.
        /// \retval                    Status code defined in the VL_ERROR. If the function succeeds, returns VL_ERR_SUCCESS.
        EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	vlCreateGammaLUT(tLUT* pLutInfo, uint32_t ColorIndex, double gamma);

        //******************************************************************************************************************
        /// \brief   This function applies a user-defined Gamma LUT to a LUT that has already been initialized with values.
        /// 
        /// \note The LUT has to be created using \c vlMallocLUT() before this function can be called.
        ///
        /// \param [in] pLutInfo       Pointer to tLUT structure with information on existing LUT.
        /// \param [in] ColorIndex     Select the LUT color index to create. 0=Monochrome or Red, 1=Green, 2=Blue
        /// \param [in] gamma          Gamma value to be used for the creation of the LUT entries.
        /// \retval                    Status code defined in the VL_ERROR. If the function succeeds, returns VL_ERR_SUCCESS.
        EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	vlApplyGammaToLUT(tLUT* pLutInfo, uint32_t ColorIndex, double gamma);

        //******************************************************************************************************************
        /// \brief   This function initializes a LUT with linear values between an user defined input and output range. Any
        /// existing LUT values will be overwritten.
        /// \par
        /// The output values for input values between X1 and X2 will be linear between Y1 and Y2.
        /// \par
        /// The output values for input values between 0 and iMix will be min(Y1,Y2) and the output values for inputs between
        /// X2 and the maximum input value will be max(Y1,Y2). 
        ///
        /// \note The LUT has to be created using \c vlMallocLUT() before this function can be called.
        ///
        /// \param [in] pLutInfo       Pointer to tLUT structure with information on existing LUT.
        /// \param [in] ColorIndex     Select the LUT color index to create. 0=Monochrome or Red, 1=Green, 2=Blue
        /// \param [in] X1             Minimum input value to be used for the creation of the LUT entries.
        /// \param [in] X2             Maximum input value to be used for the creation of the LUT entries.
        /// \param [in] Y1             Minimum output value to be used for the creation of the LUT entries.
        /// \param [in] Y2             Maximum output value to be used for the creation of the LUT entries.
        /// \retval                    Status code defined in the VL_ERROR. If the function succeeds, returns VL_ERR_SUCCESS.
        /// \code
        /// // This pseudo code illustrates how the LUT values are created:
        ///
        /// slope = (Y2 - Y1)/(X2 - X1);
        /// offset = Y1 - (slope * X1);
        ///
        /// for (val=0; val<numLUTEntries; val++)
        /// {
        ///     if ((val>=0) && (val<X1))
        ///     {
        ///        LUT[val] = min(Y1,Y2);
        ///     }
        ///     else if ((val>=X1) && (val<X2))
        ///     {
        ///        LUT[val] = offset + slope*val;
        ///     }
        ///     else if (val>=X2))
        ///     {
        ///        LUT[val] = max(Y1,Y2);
        ///     }
        /// }
        /// 
        /// // If Y1 < Y2 then it leads to a slope like this:
        ///
        ///    Output
        ///      ^
        ///  Y2  |       ___
        ///      |      /
        ///      |     /
        ///      |    /
        ///      |   /
        ///  Y1  |__/
        ///      |
        ///      -----------> Input
        ///         ^    ^
        ///         X1   X2
        ///
        /// // If Y1 > Y2 then it leads to a slope like this:
        ///
        ///    Output
        ///      ^
        ///   Y1 |__      
        ///      |  \   
        ///      |   \ 
        ///      |    \
        ///      |     \
        ///   Y2 |      \___
        ///      |
        ///      -----------> Input
        ///         ^    ^
        ///         X1   X2
        /// \endcode
        EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	vlCreateLinearLUT(tLUT* pLutInfo, uint32_t ColorIndex, uint32_t X1, uint32_t X2, uint32_t Y1, uint32_t Y2);

        //******************************************************************************************************************
        /// \brief   This function initializes a LUT with linear values between two user defined knee-points. The first linear
        /// segment will be between (0,0) and Knee1. Second linear segment will bee between Knee1 and Knee2 and the last linear
        /// segment will be between Knee2 and (Input Max,Output Max). Any existing LUT values will be overwritten.
        ///
        /// \note The LUT has to be created using \c vlMallocLUT() before this function can be called.
        ///
        /// \param [in] pLutInfo       Pointer to tLUT structure with information on existing LUT.
        /// \param [in] ColorIndex     Select the LUT color index to create. 0=Monochrome or Red, 1=Green, 2=Blue
        /// \param [in] knee1          First Knee-point to be used for the creation of the LUT entries.
        /// \param [in] knee2          Second Knee-point to be used for the creation of the LUT entries.
        /// \retval                    Status code defined in the VL_ERROR. If the function succeeds, returns VL_ERR_SUCCESS.
        /// \code
        /// // This pseudo code illustrates how the LUT values are created:
        ///
        /// slope1 = Y2/X2;
        ///
        /// for (val=0; val<Knee1.X; val++)
        /// {
        ///    LUT[val] = offset1 + slope1*val;
        /// }
        /// 
        /// slope2 = (Knee2.Y - Knee1.Y)/(Knee2.X - Knee1.X);
        /// offset2 = Knee1.Y - (slope2 * Knee1.X);
        ///
        /// for (val=Knee1.X; val<Knee2.X; val++)
        /// {
        ///    LUT[val] = offset2 + slope2*val;
        /// }
        /// 
        /// slope3 = (Output Max - Knee2.Y)/(Input Max - Knee2.X);
        /// offset3 = Knee2.Y - (slope3 * Knee2.X);
        ///
        /// for (val=Knee2.X; val<Input Max; val++)
        /// {
        ///    LUT[val] = offset3 + slope3*val;
        /// }
        /// \endcode
        EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	vlCreateKneeLUT(tLUT* pLutInfo, uint32_t ColorIndex, POINT knee1, POINT knee2);

        //******************************************************************************************************************
        /// \brief   This function directly reads a value from the LUT.
        /// 
        /// \note The LUT has to be created using \c vlMallocLUT() and initialized before this function can be called.
        ///
        /// \param [in] pLutInfo       Pointer to tLUT structure with information on existing LUT.
        /// \param [in] ColorIndex     Select the LUT color index to use. 0=Monochrome or Red, 1=Green, 2=Blue
        /// \param [in] LUTIndex       Zero-based index into the LUT.
        /// \param [in] pLUTValue      Pointer to uint32_t variable where the LUT value is to be returned.
        /// \retval                    Status code defined in the VL_ERROR. If the function succeeds, returns VL_ERR_SUCCESS.
        EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	vlGetLUTValue(tLUT* pLutInfo, uint32_t ColorIndex, uint32_t LUTIndex, uint32_t* pLUTValue);

        //******************************************************************************************************************
        /// \brief   This function directly writes a value to the LUT.
        /// 
        /// \note The LUT has to be created using \c vlMallocLUT() and initialized before this function can be called.
        ///
        /// \param [in] pLutInfo       Pointer to tLUT structure with information on existing LUT.
        /// \param [in] ColorIndex     Select the LUT color index to use. 0=Monochrome or Red, 1=Green, 2=Blue
        /// \param [in] LUTIndex       Zero-based index into the LUT.
        /// \param [in] LUTValue       New uint32_t value to be entered into the LUT.
        /// \retval                    Status code defined in the VL_ERROR. If the function succeeds, returns VL_ERR_SUCCESS.
        EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	vlSetLUTValue(tLUT* pLutInfo, uint32_t ColorIndex, uint32_t LUTIndex, uint32_t LUTValue);

        #ifdef UNICODE
        #define vlSaveLUT  vlSaveLUTW
        #else
        #define vlSaveLUT  vlSaveLUTA
        #endif // !UNICODE

        //******************************************************************************************************************
        /// \brief   This function saves a LUT to disk as a binary file
        /// 
        /// \note The LUT has to be created using \c vlMallocLUT() and initialized before this function can be called.
        ///
        /// \param [in] pLutInfo       Pointer to tLUT structure with information on existing LUT.
        /// \param [in] pPath          Filename and path to where the LUT will be written. The filename is a 8-bit (ANSI) character
        ///                            string.
        /// \retval                    Status code defined in the VL_ERROR. If the function succeeds, returns VL_ERR_SUCCESS.
        EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	vlSaveLUTA(tLUT* pLutInfo, LPCSTR pPath);

        //******************************************************************************************************************
        /// \brief   This function saves a LUT to disk as a binary file
        /// 
        /// \note The LUT has to be created using \c vlMallocLUT() and initialized before this function can be called.
        ///
        /// \param [in] pLutInfo       Pointer to tLUT structure with information on existing LUT.
        /// \param [in] pPath          Filename and path to where the LUT will be written. The filename is an Unicode character string.
        /// \retval                    Status code defined in the VL_ERROR. If the function succeeds, returns VL_ERR_SUCCESS.
        EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	vlSaveLUTW(tLUT* pLutInfo, LPCWSTR pPath);

        #ifdef UNICODE
        #define vlLoadLUT  vlLoadLUTW
        #else
        #define vlLoadLUT  vlLoadLUTA
        #endif // !UNICODE

        //******************************************************************************************************************
        /// \brief   This function loads a LUT from disk. If the LUT already contains data then it will be overwritten.
        /// 
        /// \note This function will automatically free any memory allocated for the existing LUT and allocate new memory
        /// that will be large enough to hold the LUT that is read from disk.
        ///
        /// \param [in] pLutInfo       Pointer to tLUT structure with information on LUT.
        /// \param [in] pPath          Filename and path to the LUT file to be read. The filename is a 8-bit (ANSI) character
        ///                            string.
        /// \retval                    Status code defined in the VL_ERROR. If the function succeeds, returns VL_ERR_SUCCESS.
        EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	vlLoadLUTA(tLUT* pLutInfo, LPCSTR pPath);

        //******************************************************************************************************************
        /// \brief   This function loads a LUT from disk. If the LUT already contains data then it will be overwritten.
        /// 
        /// \note This function will automatically free any memory allocated for the existing LUT and allocate new memory
        /// that will be large enough to hold the LUT that is read from disk.
        ///
        /// \param [in] pLutInfo       Pointer to tLUT structure with information on LUT.
        /// \param [in] pPath          Filename and path to the LUT file to be read. The filename is an Unicode character string.
        /// \retval                    Status code defined in the VL_ERROR. If the function succeeds, returns VL_ERR_SUCCESS.
        EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	vlLoadLUTW(tLUT* pLutInfo, LPCWSTR pPath);

//---------------------------------------------------------------------------------------------------------------------------------------------------
		/// @}
		/// \addtogroup	Transform
		/// @{
//---------------------------------------------------------------------------------------------------------------------------------------------------

        //******************************************************************************************************************
        /// \brief   Flip the image contents in the image buffer.
        /// 
        /// \note The input images needs to be 8, 10, 12, 14 or 16-bit Monochrome or Bayer-Color unpacked pixel format.
        ///
        /// \param [in] pImageInfo     Pointer to tIMAGE_INFO structure with information on the image data.
        /// \param [in] flipType       Type of flip operation. \c FLIP_HORIZONTAL will Mirror the image and \c FLIP_VERTICAL will
        ///                            flip the image upside-down.
        /// \retval                    Status code defined in the VL_ERROR. If the function succeeds, returns VL_ERR_SUCCESS.
        EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	vlFlip(tIMAGE_INFO* pImageInfo, IMAGE_FLIP_TYPE flipType);

        //******************************************************************************************************************
        /// \brief   Rotate the image contents in the image buffer.
        /// 
        /// \note The input images needs to be 8, 10, 12, 14 or 16-bit Monochrome or Bayer-Color unpacked pixel format.
        ///
        /// \param [in] pImageInfo     Pointer to tIMAGE_INFO structure with information on the image data.
        /// \param [in] rotateType     Type of rotate operation. \c ROTATE_90_DEG_CW will rotate the image 90 degrees clockwise,
        ///                            \c ROTATE_90_DEG_CCW will rotate the image 90 degrees counter-clockwise and 
        ///                            \c ROTATE_180_DEG will rotate the image 180 degrees similar to if the camera was mounted 
        ///                            upside-down.
        /// \retval                    Status code defined in the VL_ERROR. If the function succeeds, returns VL_ERR_SUCCESS.
        EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	vlRotate(tIMAGE_INFO* pImageInfo, IMAGE_ROTATE_TYPE rotateType);

                       //---------------------------------------------------------------------------------------------------------------------------------------------------
		/// @}
		/// \addtogroup	Histogram
		/// @{
        //---------------------------------------------------------------------------------------------------------------------------------------------------

        //******************************************************************************************************************
        /// \brief   Allocate the buffer memory for the Histogram based on the pixel format read from the image buffer.
        /// 
        /// \par
        /// The Histogram will be initialized and the memory will be allocated based on the pixel format read from the image.
        /// The allocated Histogram will need to be freed up using \c vlFreeHistogram()
        ///
        /// \param [in] pImageInfo       Pointer to tIMAGE_INFO structure with information on the image data.
        /// \param [in] pHistogramInfo   Pointer to tHIST structure with information on Histogram.
        /// \retval                      Status code defined in the J_STATUS_CODES. If the function succeeds, returns J_ST_SUCCESS.
        EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE vlMallocHistogram(tIMAGE_INFO* pImageInfo, tHIST* pHistogramInfo);

        //******************************************************************************************************************
        /// \brief   Free the Histogram memory previously allocated using \c vlMallocHistogram().
        /// 
        /// \param [in] pHistogramInfo Pointer to tHIST structure with information on Histogram.
        /// \retval                    Status code defined in the J_STATUS_CODES. If the function succeeds, returns J_ST_SUCCESS.
        EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE vlFreeHistogram(tHIST* pHistogramInfo);

        //******************************************************************************************************************
        /// \brief   Clear the contents of the Histogram memory previously allocated using \c vlMallocHistogram().
        /// 
        /// \param [in] pHistogramInfo Pointer to tHIST structure with information on Histogram.
        /// \retval                    Status code defined in the J_STATUS_CODES. If the function succeeds, returns J_ST_SUCCESS.
        EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE vlClearHistogram(tHIST* pHistogramInfo);

        //******************************************************************************************************************
        /// \brief   Creates the Histogram from the image contents.
        /// 
        /// \par
        /// The Histogram will be has to be allocated in advance using \c vlMallocHistogram()
        ///
        /// \param [in] pImageInfo       Pointer to tIMAGE_INFO structure with information on the image data.
        /// \param [in] pHistogramInfo   Pointer to tHIST structure with information on Histogram.
        /// \retval                      Status code defined in the J_STATUS_CODES. If the function succeeds, returns J_ST_SUCCESS.
        EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE vlCreateHistogram(tIMAGE_INFO* pImageInfo, RECT* pMeasureRect, tHIST* pHistogramInfo);

        //******************************************************************************************************************
        /// \brief   This function directly reads a value from the Histogram.
        /// 
        /// \note The Histogram has to be created using \c vlMallocHistogram() and initialized using \c vlCreateHistogram() before 
        /// this function can be called.
        ///
        /// \param [in] pHistogramInfo Pointer to tHIST structure with information on existing Histogram.
        /// \param [in] ColorIndex     Select the Histogram color index to use. 0=Monochrome or Red, 1=Green, 2=Blue
        /// \param [in] HistogramIndex Zero-based index into the LUT.
        /// \param [in] pLUTValue      Pointer to uint32_t variable where the Histogram value is to be returned.
        /// \retval                    Status code defined in the VL_ERROR. If the function succeeds, returns VL_ERR_SUCCESS.
        EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE	vlGetHistogramValue(tHIST* pHistogramInfo, uint32_t ColorIndex, uint32_t HistogramIndex, uint32_t* pHistogramValue);

//---------------------------------------------------------------------------------------------------------------------------------------------------
		/// @}
		/// \addtogroup	ImageProcessing
		/// @{
//---------------------------------------------------------------------------------------------------------------------------------------------------
		EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE  vlImageProcessing(tIMAGE_INFO* pImageInfo, IFC_IMAGE_PROCESSING iProcessFunction, void * pParameters);

        EXTERN_C VL_IMPORT_EXPORT VL_ERROR VL_CALLTYPE  vlImageProcessingEx(tIMAGE_INFO* pInputImageInfo, tIMAGE_INFO* pOutputImageInfo, IFC_IMAGE_PROCESSING iProcessFunction, void * pParameters);
		/// @}
	}	// namespace VL
}	// namespace GenICam

#ifdef __cplusplus
}
#endif

#endif
