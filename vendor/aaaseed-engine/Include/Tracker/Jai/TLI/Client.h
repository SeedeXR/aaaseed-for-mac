/****************************************************************************
 (c) 2004 by STEMMER IMAGING

 License: This file is published under the license of the EMVA GenICam  Standard Group. 
 A text file describing the legal terms is included in  your installation as 'license.txt'. 
 If for some reason you are missing  this file please contact the EMVA or visit the website
 (http://www.genicam.org) for a full copy.

 THIS SOFTWARE IS PROVIDED BY THE EMVA GENICAM STANDARD GROUP "AS IS"
 AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,  
 THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR  
 PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE EMVA GENICAM STANDARD  GROUP 
 OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,  SPECIAL, 
 EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT  LIMITED TO, 
 PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,  DATA, OR PROFITS; 
 OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY  THEORY OF LIABILITY, 
 WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT  (INCLUDING NEGLIGENCE OR OTHERWISE) 
 ARISING IN ANY WAY OUT OF THE USE  OF THIS SOFTWARE, EVEN IF ADVISED OF THE 
 POSSIBILITY OF SUCH DAMAGE.

****************************************************************************/

/// \file     $Source: /cvs/genicam/genicam/library/CPP/include/TLI/Client.h,v $
/// \brief    GenICam Transport Layer Client Interface
/// \version  $Revision: 1.4 $
/// \author   rst ( SI )
/// \date     $Date: 2005/10/28 13:17:43 $


#ifndef GC_JAI_TLI_CLIENT_H_
#define GC_JAI_TLI_CLIENT_H_

#include <TLI/Platform.h>

#define GCTLIDLL 1

#include <TLI\Gentl.h>
#include <TLI/System.h>
#include <Base/GCError.h>
#include <InheritEnum.h>

#if defined(__cplusplus)
    //! 8 bit bool
    typedef bool              bool8_t;
#else
    typedef uint8_t           bool8_t;
#endif

#ifdef __cplusplus
extern "C" {
#endif

  namespace GenICam  {
      namespace Client   {

          #define STRING_BUF_SIZE 256

          ///  The TLINFO struct is used to obtain information about a Transport Layer library
          typedef struct S_TLINFO_T{

              int   iVersion;                                 ///< Version of the Transport Layer DLL
              int   iSize;                                    ///< Size of the structure in Bytes 
              char  sVendor[STRING_BUF_SIZE];                 ///< name of the manufacturer
              char  sTLName[STRING_BUF_SIZE];                 ///< name of the Transport Layer DLL
              char  sTransportInterfaceType[STRING_BUF_SIZE]; ///< Name of the Transport interface "IEEE1394", "GigEVision", "Generic"
          }  TLINFO;

          /// The DEVIINFO struct contains information on the identity of a device
          /// The sVendor::sModel::sID should represent a unique device ID
          typedef struct S_DEVINFO_T{
              char sStandard[STRING_BUF_SIZE];                ///< GenICam supported standard IEEE_1394, GigEVision
              char sVendor[STRING_BUF_SIZE];                  ///< Device Vendor
              char sModel[STRING_BUF_SIZE];                   ///< Model Vendor
              char sID[STRING_BUF_SIZE];                      ///< Model ID ( SN, MAC or similiar )
          } DEVINFO;

#pragma pack ( push, 1 )
          typedef struct S_ACTION_ACK_T
          {
              uint16_t              Status;           //< \c GEV::STATUS of the ACK.
              uint16_t              Type;             //< \c GEV::GVCP of the packet.
              uint16_t              Length;           //< Number of bytes after the header.
              uint16_t              ID;               //< ACK ID to match incomming CMD.
              uint32_t				IPAddress;		  //< IP-Address of camera that has returned this ACK.
          } ACTION_ACK;
#pragma pack ( pop )

          enum INTERFACE_INFO_CMD_LIST_EX
          {
              INTERFACE_INFO_CUSTOM       = INTERFACE_INFO_CUSTOM_ID,   /* Starting value for GenTL Producer custom IDs. */

              GC_INTERFACE_MAC,              ///< MAC address of the interface as array of 6 bytes
              GC_INTERFACE_MAC_STRING,       ///< MAC address of the interface as string
              GC_INTERFACE_INFO_MAX_ID
          };
		  typedef InheritEnum< INTERFACE_INFO_CMD_LIST, INTERFACE_INFO_CMD_LIST_EX > J_INTERFACE_INFO_CMD_LIST_EX;
		  typedef uint32_t J_INTERFACE_INFO_CMD;

          enum DEVICE_INFO_CMD_LIST_EX
          {
			  //Added by d.r. to support serial numbers.  Will be a part of GenTL 1.4 anyway...
              DEVICE_INFO_USERNAME = 6,                                ///< <char[]>    String containing the Username of the device
              DEVICE_INFO_SERIAL_NUMBER = 7,                           ///< <char[]>    String containing SN of the Device 
		      
			  DEVICE_INFO_CMD_CUSTOM = DEVICE_INFO_CUSTOM_ID,
              DEVICE_INFO_IP,                                          ///< uint32_t    IP of the device as uint32_t
              DEVICE_INFO_MAC,                                         ///< <char[]>    MAC of the Device
              DEVICE_INFO_IP_STRING,                                   ///< <char[]>    String containing IP of the Device 
              DEVICE_INFO_MAC_STRING,                                  ///< <char[]>    String containing MAC of the Device 
              DEVICE_INFO_MAX_ID
          };
		  typedef InheritEnum< DEVICE_INFO_CMD_LIST, DEVICE_INFO_CMD_LIST_EX > J_DEVICE_INFO_CMD_LIST_EX;
		  typedef uint32_t J_DEVICE_INFO_CMD;

		  /// \brief The J_BUFFER_INFO_CMD is used to get static buffer information \c J_DataStream_GetBufferInfo()
          enum  BUFFER_INFO_CMD_LIST_EX
          {
			  BUFFER_INFO_PIXELTYPE = BUFFER_INFO_PIXELFORMAT, /* ADDED BY D.R. UINT64     GenTL v1.2 */
              BUFFER_INFO_NUM_PACKETS_MISSING             = 1001, /* ADDED BY D.R. U */
              BUFFER_INFO_BLOCKID			              = BUFFER_INFO_FRAMEID,  /* Modified by d.r. 11/26/14 */
              BUFFER_INFO_NUMBER                          = 1003   /*Not used*/
          };
		  typedef InheritEnum< BUFFER_INFO_CMD_LIST, BUFFER_INFO_CMD_LIST_EX > J_BUFFER_INFO_CMD_LIST_EX;
		  typedef uint32_t J_BUFFER_INFO_CMD;

          //#pragma pack ( push, 1 )
          //      /// Struct containing the information provided by a GigE Event to pass it on to the GenAPI
          //      struct EVENT_DATA_GEV_EVENT_CMD
          //      {
          //         uint16_t              m_EventID;          	  ///< \c EVENT identifying reason.
          //         uint16_t              m_StreamChannelIndex;  ///< Index of stream channel (0xFFFF for no channel).
          //         uint16_t              m_BlockID;          	  ///< Data block ID (0 for no block).
          //         uint64_t              m_Timestamp;        	  ///< Event timestamp (0 if not supported).
          //      };
          //#pragma pack ( pop )

#pragma pack ( push, 1 )
          /// Struct containing the information provided by a GigE Event to pass it on to the application layer
          struct EVENT_DATA_GEV_EVENTDATA_CMD
          {
              uint16_t              m_EventID;          	  ///< \c EVENT identifying reason.
              uint16_t              m_StreamChannelIndex;  ///< Index of stream channel (0xFFFF for no channel).
              uint16_t              m_BlockID;          	  ///< Data block ID (0 for no block).
              uint64_t              m_Timestamp;        	  ///< Event timestamp (0 if not supported).
              uint16_t              m_EventDataLength;     ///< Number of bytes data filled into m_EventData
              uint8_t               m_EventData[540];      ///< Data associated with the event
          };
#pragma pack ( pop )

          /// Struct containing the information provided by a GigE Event to pass it on to the GenAPI
          struct EVENT_DATA_NEW_IMAGE
          {
              BUFFER_HANDLE         m_iBufferID;          	  ///< Buffer ID of the delivered buffer
          };

          ///  \addtogroup PortInterface   Port Interface Functions
          ///  @{

          /* C API Interface Functions */
          GC_API      GC_CALLTYPE GCReadData              ( PORT_HANDLE hPort, uint64_t iAddress, void *pBuffer, size_t *iSize);
          GC_API      GC_CALLTYPE GCWriteData             ( PORT_HANDLE hPort, uint64_t iAddress, const void *pBuffer, size_t *iSize);
          GC_API      GC_CALLTYPE GCGetURL                ( PORT_HANDLE hPort, char *pURL, size_t *iSize );

          ///  @}
          ///  \addtogroup EventSourceInterface Event Source Interface functions
          ///  @{
          GC_API      GC_CALLTYPE GCRegisterEventEx      ( EVENTSRC_HANDLE hEventSrc, EVENT_TYPE iEvent, HANDLE hEvent, EVENT_HANDLE *pE);

          ///  @}
          ///  \addtogroup EventInterface Event Interface functions
          ///  @{

          ///  @}
          ///  \addtogroup TLAccess  TL access functions
          ///  @{
          GC_API      GC_CALLTYPE TLInfo                  ( const char *psPath, TLINFO *pInfo,/* uint32_t*/size_t *piSize);
          GC_API      GC_CALLTYPE GCGetErrorText          ( TL_HANDLE hTL, char *pErrText, size_t *iSize);
          GC_API      GC_CALLTYPE TLGetInterfaceName      ( TL_HANDLE hTL, uint32_t iIndex,  char * psName, size_t *iSize );
          //GC_API      GC_CALLTYPE TLUpdateInterfaceList   ( TL_HANDLE hTL, bool8_t *bHasChanged );

          ///  @}
          ///  \addtogroup InterfaceAccess  Interface access functions
          ///  @{
          // obsolete
          //GC_API      GC_CALLTYPE TLGetPort               ( TL_HANDLE hTL, const char *sPortName, PORT_HANDLE *pPort);


          GC_API      GC_CALLTYPE TLCloseInterface        ( IF_HANDLE hInterface);
		  GC_API      GC_CALLTYPE TLGetNumDevices         ( IF_HANDLE hInterface, uint32_t *iNumDevices );
		  GC_API      GC_CALLTYPE TLGetDeviceName         ( IF_HANDLE hInterface, uint32_t iIndex, char * psName, size_t *iSize  );
		  GC_API      GC_CALLTYPE TLUpdateDeviceList      ( IF_HANDLE hInterface, uint32_t iTimeout );
		  GC_API      GC_CALLTYPE TLGetDeviceInfo         ( IF_HANDLE hInterface, const char *sDevName, DEVICE_INFO_CMD iID, void* pInfo, size_t *iSize);
          GC_API      GC_CALLTYPE TLOpenDevice            ( IF_HANDLE hInterface, const char *sDevName, DEVICE_ACCESS_FLAGS iOpenFlags, DEV_HANDLE *pDev );
          // sek 2007/01/26 added function for multicast
          GC_API      GC_CALLTYPE TLOpenDeviceMc          ( IF_HANDLE hInterface, const char *sDevName, DEVICE_ACCESS_FLAGS iOpenFlags, DEV_HANDLE *pDev , uint32_t iMcIP=0);
          // sek 2007/07/06 added functions for ForceIP
          GC_API      GC_CALLTYPE TLGetForceIp            ( IF_HANDLE hInterface, uint32_t *piEable, uint32_t *piInitialIp);
          GC_API      GC_CALLTYPE TLSetForceIp            ( IF_HANDLE hInterface, uint32_t iEable, uint32_t iInitialIp=0);

          // sek 2008/07/08 added functions for Execute ForceIP
          GC_API      GC_CALLTYPE TLForceIp               ( IF_HANDLE hInterface, uint8_t *pMacAddress, uint32_t iIPAddress, uint32_t iSubnetMask, uint32_t iDefaultGateway);

          // sek 2009/07/17 Added.
          // sek 2009/10/15 Changed.
          GC_API      GC_CALLTYPE TLSendActionCommand     ( TL_HANDLE hTL, uint32_t iDeviceKey, uint32_t iGroupKey, uint32_t iGroupMask, ACTION_ACK * pAckBuffer, uint32_t * piNumOfAck);

          ///  @}
          ///  \addtogroup DeviceAccess Device access functions
          ///  @{

          //GC_API      GC_CALLTYPE DevGetTLPort            ( DEV_HANDLE hDev, const char *sPortName, PORT_HANDLE *);
          GC_API      GC_CALLTYPE DevGetPort              ( DEV_HANDLE hDev, PORT_HANDLE *);
          GC_API      GC_CALLTYPE DevCreateDataStream     ( DEV_HANDLE hDev, uint32_t iChannel, DS_HANDLE *pDS);
          // sek 2007/01/26 add function for multicast
          GC_API      GC_CALLTYPE DevCreateDataStreamMc     ( DEV_HANDLE hDev, uint32_t iChannel, DS_HANDLE *pDS, uint32_t iMcIP=0);
          //GC_API      GC_CALLTYPE DevGetInfo              ( DEV_HANDLE hDev, DEVINFO *pInfo, uint32_t *iSize);

          //GC_API      GC_CALLTYPE DevSetCallback          ( DEV_HANDLE hDev, DEVICE_EVENT_TYPE iEvent, Dev_Callback &pCB, void *pPrivate);
          //GC_API      GC_CALLTYPE DevRemoveCallback       ( DEV_HANDLE hDev, DEVICE_EVENT_TYPE iEvent);

          /// @}
          /// \addtogroup DSFunctions Data Stream Functions
          /// @{

          //GC_API      GC_CALLTYPE DSWaitForBuffer         ( DS_HANDLE hDS, uint32_t iTimeoutMs, uint64_t &iBufferId, ACQ_WAIT_FLAGS iWaitFlags = ACQ_WAIT_FLAGS_NONE, void **ppPrivate = NULL);
          GC_API      GC_CALLTYPE DSGetStreamInfo         ( DS_HANDLE hDS, STREAM_INFO_CMD iCmd, void *pBuffer, size_t *iSize);

          typedef enum
          {
              STREAM_PARAM_CMD_CAN_RESEND = 0x1,                    ///< <uint32_t> flag indicating if the source can handle resends
              STREAM_PARAM_CMD_TOTALNUMBEROFBYTESPERFRAME ,         ///< xx
              STREAM_PARAM_CMD_PASS_CORRUPT_FRAMES,                 ///< <uint32_t> flag indicating if corrupt frames are passed to the layer above
              STREAM_PARAM_CMD_MAX_FRAMES_IN_NOT_COMPLETE_LIST,     ///< <uint32_t> max number of frames in not complete list 
              STREAM_PARAM_CMD_RECIEVE_TIMEOUTS_BEFORE_LISTFLUSH,   ///< <uint32_t> number of recieve timeouts before incomplete list is flushed / delivered
              STREAM_PARAM_CMD_OOO_PACKETS_BEFORE_RESEND,           ///< <uint32_t> Out of order Packets before resend is issued
              STREAM_PARAM_CMD_RECIEVE_TIMEOUT,                     ///< <uint32_t> Timeout in ms for recieve
              STREAM_PARAM_CMD_MAX_ID
          }  STREAM_PARAM_CMD;

          // sek 2007/07/03 Added.
          GC_API      GC_CALLTYPE DSSetStreamParam        ( DS_HANDLE hDS, STREAM_PARAM_CMD iCmd, void *pBuffer, size_t *iSize);
          GC_API      GC_CALLTYPE DSGetStreamParam        ( DS_HANDLE hDS, STREAM_PARAM_CMD iCmd, void *pBuffer, size_t *iSize);

          /// @}

          /// \addtogroup SOCustomInterfaceFunctions Custom Interface Function
          /// @{

          GC_API      GC_CALLTYPE DevQueryCustomFeature   ( DEV_HANDLE hDev, void *pInBuf, uint32_t iInSize, void *pOutBuf, uint32_t *iOutSize);

        /// @}
     
      } // end of namespace Client

  } // end of namespace GenICam


  #ifdef __cplusplus
  }
  #endif
#endif //GC_JAI_TLI_CLIENT_H_


