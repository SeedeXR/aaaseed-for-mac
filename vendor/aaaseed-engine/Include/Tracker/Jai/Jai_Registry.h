#ifndef _GCREGISTRY
#define _GCREGISTRY

//#define _WINDOWS_ //to remove windows version of DOM parser

#include "RegistryPlatform.h"
#include <tli/System.h>
#include <Base/GCTypes.h>
#include <Base/GCError.h>
#include <GenApi/IPort.h>

//*******************************
///\file GCRegistry.h
///\brief This is the GCRegistry include file.
///\author mkr
///\version 0.0.0.1
///



/// \mainpage
///\b use \b case
/// 
///\b installer
///
///Install a transport layer and a xml file for it. The user don't want to use default tl xml file.
///
/// \code
/// RG_HANDLE hReg;
/// ELEMENT_HANDLE* hTl;
/// J_Registry_Open(&hReg);
/// J_Registry_InsertNewElement( hReg , TransportLayer , "MyUniqTL" , &hTl);
/// J_Element_InsertAttibute( hTl , TLPath , "c:\mytl.dll");
/// J_Element_InsertAttibute( hTl , XMLPath , "c:\mytl.xml");
/// GCRegistryCloseElementHandle(Tl);
/// J_Registry_Close(hReghReg);
/// \endcode
///
///Remove a transport layer from the GCRegistry.
///
/// \code
/// RG_HANDLE hReg;
/// J_Registry_Open(&hReg);
/// J_Registry_RemoveElement(hReg , TransportLayer , "MyUniqTL" );
/// J_Registry_Close(hReghReg);
/// \endcode
///
///
/// \b Configure
///
/// To create a entry for a new camera or a transport layer is done in the same way as with the installer. See the installer for ex.
/// 
/// To list all transport layer
///
/// \code
/// RG_HANDLE hReg;
/// LIST_HANDLE hList;
/// ELEMENT_HANDLE* hTl;
/// int iNumberOfTL = 0;
/// J_Registry_Open(&hReg);
/// J_Registry_OpenElementList( hReg,TransportLayer , &hList);
/// J_ElementList_GetNumOfElements( hList, &iNumberOfTL);
/// for(int i=0; i<iNumberOfTL ; i++)
/// {
///   GCRegistryGetElementByNumer(&hList , i ,&hTl);
///   //Do something with it
///   GCRegistryCloseElementHandle(hTl);
/// }
/// J_Registry_CloseElementList( hList );
/// J_Registry_Close( hReghReg );
/// \endcode
///
///
/// Change one of the parameter of MyCamera
///
/// \code
/// //open the GCRegistry
/// ELEMENT_HANDLE hEle;
/// J_Registry_OpenElementByName( hReg , TransportLayer , "MyCamera" , &hEle);
/// J_Element_InsertAttibute( hEle , XMLPath ,"c:\MyDir\MyCamera.xml");
/// // Clean up
/// \endcode
///
///
///
/// \b Factory
///
///
/// The user want to connect to MyCamera, 
/// \code
/// ELEMENT_HANDLE hEle;
/// int iError;
/// int8_t pXMLBuffer[512];
/// int32_t pXMLBufferSize = 512;
/// iError = J_Registry_OpenElementByName( hReg , TransportLayer , "MyCamera" , &hEle); // get the handler to the element
/// if( iError == ERROR )
///   //error handling
/// J_Element_OpenAttribute( hEle, TLPath , pXMLBuffer , &pXMLBufferSize );
/// // load the transport layer and find the device
/// J_Registry_GetXML( hReg, device , NULL , "JAI" , "CV-A10GE" , "1.3" , pPort, "local::MyXML.xml:3000:4999" , 
///                 pXMLBuffer, pXMLBufferSize);
/// \endcode


namespace GenICam  
{
	namespace GCRegistry
	{
		using namespace GenApi;

		typedef uint64_t RG_HANDLE; /*!< A handler for GCRegistry*/
		typedef uint64_t LIST_HANDLE; /*!< A handler to a list of element*/
		typedef uint64_t ELEMENT_HANDLE; /*!< A handler to a element*/
		typedef uint64_t ATLIST_HANDLE; /*!< A handler to a list of attributes*/

		enum RGType
		{
			TransportLayer = 1,
			Device
		};

		enum RGAttribute
		{
			TLPath = 1,
			GluePath,
			TLXMLPath,
			TLPrefex,
			DeviceXML,
			TLEnabled,
			CacheLocalXml
		};

		class IPort;

		//******************************************************************
		///  \par J_Registry_Open
		///                                Open the GCRegistry an return a handler to the GCRegistry
		///  \param [in] pDataBaseName     The name of the database to open
		///  \param [out] hReg             A empthy RG_HANDLER.
		///  \retval                       error code
		///
		///  \note

		EXTERN_C GCRC_IMPORT_EXPORT GC_ERROR GCFC_CALLTYPE J_Registry_Open(const int8_t* pDataBaseName,RG_HANDLE* hReg);

		//******************************************************************
		///  \par J_Registry_Close
		///                                Close the GCRegistry handler and free all allcated memory that the handler have used
		///  \param [in] hReg              A handler to a GCRegistry
		///  \retval                       error code
		///
		///  \note

		EXTERN_C GCRC_IMPORT_EXPORT GC_ERROR GCFC_CALLTYPE J_Registry_Close(const RG_HANDLE hReg);

		//******************************************************************
		///  \par J_Registry_InsertNewElement
		///                                Close the GCRegistry handler and free all allcated memory that the handler have used
		///  \param [in] hReg              A handler to a GCRegistry
		///  \param [in] Type              A string with contains the type of the elements that is inserted
		///  \param [in] strName           The name of the element that is inserted
		///  \param [out] hEle             A handle the new element
		///  \retval                       error code
		///
		///  \note

		EXTERN_C GCRC_IMPORT_EXPORT GC_ERROR GCFC_CALLTYPE J_Registry_InsertNewElement(const RG_HANDLE hReg,const RGType Type,const int8_t* strName, ELEMENT_HANDLE* hEle);


		//******************************************************************
		///  \par J_Registry_OpenElementList
		///                                Create a list of elements of the type ::strType
		///  \param [in] hReg              A handler to a GCRegistry
		///  \param [in] Type              The types of the wanted elements
		///  \param [out] hList            The name of the element that is inserted
		///  \retval                       error code
		///
		///  \note
		EXTERN_C GCRC_IMPORT_EXPORT GC_ERROR GCFC_CALLTYPE J_Registry_OpenElementList(const RG_HANDLE hReg,const RGType Type, LIST_HANDLE* hList);

		//******************************************************************
		///  \par J_Registry_OpenElementList
		///                                Close a list
		///  \param [in] hList             A handler to a list
		///  \retval                       error code
		///
		///  \note
		EXTERN_C GCRC_IMPORT_EXPORT GC_ERROR GCFC_CALLTYPE J_Registry_CloseElementList(const LIST_HANDLE hList );
		//******************************************************************
		///  \par J_Registry_OpenElementList
		///                                Get the number of elements in the list
		///  \param [in]  hList            A handler the a element list
		///  \param [out] iNumber          the number of elements in the list
		///  \retval                       error code
		///
		///  \note

		EXTERN_C GCRC_IMPORT_EXPORT GC_ERROR GCFC_CALLTYPE J_ElementList_GetNumOfElements(const LIST_HANDLE hList, int32_t* iNumber);

		//******************************************************************
		///  \par GCRegistryGetElementByNumber
		///                                Get the element with index ::iNumber
		///  \param [in]  hList            A handler the a element list
		///  \param [in]  iNumber          the index of the wanted element
		///  \param [out] strName          the Name og the element
		///  \param [out] iNameSize        The size of ::strName
		///  \retval                       error code
		///
		///  \note

		EXTERN_C GCRC_IMPORT_EXPORT GC_ERROR GCFC_CALLTYPE J_ElementList_GetElementNameByIndex(const LIST_HANDLE hList ,const int32_t iNumber ,int8_t* strName, size_t* iNameSize);

		//******************************************************************
		///  \par GCRegistryGetElementByNumber
		///                                Get the element with index ::iNumber
		///  \param [in]  hList            A handler the a element list
		///  \param [in]  iNumber          the index of the wanted element
		///  \param [out] hEle             A handle to the wanted element
		///  \retval                       error code
		///
		///  \note

		//    EXTERN_C GCRC_IMPORT_EXPORT GC_ERROR GCFC_CALLTYPE GCRegistryGetElementByNumber(const LIST_HANDLE hList,const int32_t iNumber,ELEMENT_HANDLE* hEle);

		//******************************************************************
		///  \par J_Registry_OpenElementByName
		///                                Get the element with the name in ::pName
		///  \param [in]  hReg             A handler to the GCRegistry
		///  \param [in]  Type             what type of element
		///  \param [out] pName            The name of the element
		///  \param [out] hEle             A handler to deliver the element in. If error then the handler is 0
		///  \retval                       error code
		///
		///  \note

		EXTERN_C GCRC_IMPORT_EXPORT GC_ERROR GCFC_CALLTYPE J_Registry_OpenElementByName(const RG_HANDLE hReg ,const RGType Type,const int8_t* pName , ELEMENT_HANDLE* hEle);
		//******************************************************************
		///  \par J_Element_InsertAttibute
		///                                Insert a attribute to a element
		///  \param [in]  hEle             A handler the a element list
		///  \param [in]  AttType          The type of attribute 
		///  \param [in]  strValue         a string that contains the value of the attribute
		///  \retval                       error code
		///
		///  \note

		EXTERN_C GCRC_IMPORT_EXPORT GC_ERROR GCFC_CALLTYPE J_Element_InsertAttibute(const ELEMENT_HANDLE hEle,const RGAttribute AttType,const int8_t* strValue);

		//******************************************************************
		///  \par J_Registry_RemoveElement
		///                                Remove a element from the GCRegistry
		///  \param [in]  hReg             A handler the a element list
		///  \param [in]  Type             The type of the element
		///  \param [in]  strName          The name of the element
		///  \retval                       error code
		///
		///  \note
		EXTERN_C GCRC_IMPORT_EXPORT GC_ERROR GCFC_CALLTYPE J_Registry_RemoveElement(const RG_HANDLE hReg,const RGType Type ,const int8_t* strName );


		//******************************************************************
		///  \par J_Element_GetAttributeList
		///                                Insert an attribute to an element
		///  \param [in]  hEle             A handler the a element list
		///  \param [in]  hAttList a string that contains the attribute type
		///  \retval                       error code
		///
		///  \note

		EXTERN_C GCRC_IMPORT_EXPORT GC_ERROR GCFC_CALLTYPE J_Element_GetAttributeList(const ELEMENT_HANDLE hEle, ATLIST_HANDLE* hAttList);

		//******************************************************************
		///  \par J_AttributeList_OpenAttributeByIndex
		///                                    Gets an attribute of an element
		///  \param [in]      hAttList         A handler the a attribute list
		///  \param [in]      iNumber          the number of the attribute
		///  \param [in]      AttType          The requested information type
		///  \param [in,out]  strValue         A buffer to place the information
		///  \param [in,out]  strValueSize     The size of the buffer ::strValue
		///  \retval                           error code
		///
		///  \note

		EXTERN_C GCRC_IMPORT_EXPORT GC_ERROR GCFC_CALLTYPE J_AttributeList_OpenAttributeByIndex(const ATLIST_HANDLE* hAttList,const int32_t iNumber,const RGAttribute* AttType , int8_t* strValue , int32_t* strValueSize );

		//******************************************************************
		///  \par J_Element_OpenAttribute
		///                                    Gets an attribute of an element
		///  \param [in]      hEle             A handler the a element list
		///  \param [in]      AttType          The requested information type
		///  \param [in,out]  strValue         A buffer to place the information
		///  \param [in,out]  strValueSize     The size of the buffer ::strValue
		///  \retval                           error code
		///
		///  \note

		EXTERN_C GCRC_IMPORT_EXPORT GC_ERROR GCFC_CALLTYPE J_Element_OpenAttribute(const ELEMENT_HANDLE hEle,const RGAttribute AttType ,const int8_t* strValue , int32_t* strValueSize );

		//******************************************************************
		///  \par GCRegistryCloseElementHandle
		///                                Close a element handle or a attribute handle
		///  \param [in]  hEle             A handler the a element list
		///  \retval                       error code
		///
		///  \note

		EXTERN_C GCRC_IMPORT_EXPORT GC_ERROR GCFC_CALLTYPE J_Element_Close(const ELEMENT_HANDLE hEle);


		/*!
		\par J_Registry_GetXML
		To get a xml file for a device or a transport layer. 

		\param [in] hReg           a handle to a GCRegistry
		\param [in] type           is it a transport layer or a device
		\param [in] pUserName      a user define name in the GCRegistry, for a transport layer this is NULL
		\param [in] pManufacturer  the name of the manufacturer for the transport layer or device
		\param [in] pModel         the model name for a device or transport layer type ex. "GigE Vision"
		\param [in] pVersion       the version of the transport layer or device
		\param [in] pPort          a port from where to load the xml file if the xml file is store in the device or the transport layer
		\param [in] pURL           a url with the information on where the xml file is  
		\param [in,out] pXMLBuffer     a buffer to put the xml file into
		\param [in,out] pXMLBufferSize the size of the buffer, and the size that are used by the function after the call.
		\retval          error code

		\code
		// Get a xml file for a predefind transport layer
		J_Registry_GetXML( hReg,TransportLayer,"MyTL",NULL,NULL,NULL,NULL,NULL,
		pXMLBuffer,&pXMLBufferSize);
		// Get a xml file for a transport layer from the name and version
		J_Registry_GetXML( hReg,TransportLayer, NULL ,"JAI","GigE Vision","1.4", NULL ,NULL,
		pXMLBuffer,&pXMLBufferSize);
		// Get a xml file for a device with a user define name
		J_Registry_GetXML( hReg,Device,"MyCamera",NULL,NULL,NULL, pPort,"local::MyXML.xml:3000:4999",
		pXMLBuffer,&pXMLBufferSize);
		// Get a xml file from a no GCRegistry device
		J_Registry_GetXML( hReg, Device ,NULL ,"JAI" ,"CV-A10GE","2.3", pPort,"local::MyXML.xml:3000:4999",
		pXMLBuffer,&pXMLBufferSize);
		// Get a xml file from a uknow device
		J_Registry_GetXML( hReg, Device,NULL ,NULL ,NULL ,NULL ,pPort,"local::MyXML.xml:3000:4999",
		pXMLBuffer,pXMLBufferSize);
		\endcode
		*/

		EXTERN_C GCRC_IMPORT_EXPORT GC_ERROR GCFC_CALLTYPE J_Registry_GetXML( const RG_HANDLE hReg, 
			/*const RGType    type, */
			const int8_t*   pUserDefName, 
			/*const int8_t*   pManufacturer, */
			const int8_t*   pModel,
			/*const int8_t*   pVersion,*/
			GenApi::IPort*    pPort,
			const int8_t*   pURL,
			int8_t*   pXMLBuffer,
			size_t*   pXMLBufferSize);


		/*!
		\par J_Registry_GetData
		To get a xml file for a device or a transport layer. 

		\param [in] hReg           a handle to a GCRegistry
		\param [in] pName          a name in the registry that have information about transport layer or interface or device
		\param [in] AttType        the type of info that the user want
		\param [in] strValue       a buffer to put the information
		\param [in] strValueSize   The size of the buffer ::strValue
		\retval                    GC_ERR_SUCCESS if success

		\code
		// get the path for the transport layer

		int8_t TLBuffer[256];
		size_t TLBufferSize = 256;
		if(J_Registry_GetData( hReg, "MyDevice" , TransportLayer , TLBuffer , &TLBufferSize ) != GC_ERR_SUCCESS)
		{
		return error;
		}


		\endcode
		*/
		EXTERN_C GCRC_IMPORT_EXPORT GC_ERROR GCFC_CALLTYPE J_Registry_GetData( const RG_HANDLE hReg, const int8_t* pName, RGAttribute AttType, int8_t* strValue, size_t* strValueSize );

		// Shig
		EXTERN_C GCRC_IMPORT_EXPORT GC_ERROR GCFC_CALLTYPE J_Registry_GetXMLFromCamera(GenApi::IPort* pPort, const int8_t* pURL, int8_t* pXMLBuffer, size_t* pXMLBufferSize);

		bool uncompress_zipfile(std::string src_filename, std::string &dst_filename);

	}

}

#endif //_GCREGISTRY
