
#include "capture_jai.h"
		 
#include "infrastructure/param/param_declare.h"
#ifndef WIN32
#include <inttypes.h>
#endif


#if	AAA_JAI_SDK_DYNAMIC_LOAD()
#	include "Jai_Factory_Dynamic.h"
#else
#	include <lib_use.h>
#	if AAA_DEBUG()
		AAA_LIB_USE32( "tracker/Jai/Jai_Factory" )
#	else
		AAA_LIB_USE32( "tracker/Jai/Jai_Factory" )
#	endif // DEBUG
#endif


#if	AAA_USE_JAI()
#include "spy.h"

namespace {

	C_PCHAR_C	get_str_error( UINT32 err )
	{
		C_PCHAR	s;
		switch( err )
		{
		case J_ST_SUCCESS            	:	s = "SUCCESS";				break;
		case J_ST_ERROR              	:	s = "ERROR";				break;
		case J_ST_ERR_NOT_INITIALIZED	:	s = "ERR_NOT_INITIALIZED";	break;
		case J_ST_ERR_NOT_IMPLEMENTED	:	s = "ERR_NOT_IMPLEMENTED";	break;
		case J_ST_ERR_RESOURCE_IN_USE	:	s = "ERR_RESOURCE_IN_USE";	break;
		case J_ST_ACCESS_DENIED      	:	s = "ACCESS_DENIED";		break;
		case J_ST_INVALID_HANDLE     	:	s = "INVALID_HANDLE";		break;
		case J_ST_INVALID_ID         	:	s = "INVALID_ID";			break;
		case J_ST_NO_DATA            	:	s = "NO_DATA";				break;
		case J_ST_INVALID_PARAMETER  	:	s = "INVALID_PARAMETER";	break;
		case J_ST_FILE_IO            	:	s = "FILE_IO";				break;
		case J_ST_TIMEOUT            	:	s = "TIMEOUT";				break;
		case J_ST_ERR_ABORT          	:	s = "ERR_ABORT";			break;
		case J_ST_INVALID_BUFFER_SIZE	:	s = "INVALID_BUFFER_SIZE";	break;
		case J_ST_ERR_NOT_AVAILABLE  	:	s = "ERR_NOT_AVAILABLE";	break;
		case J_ST_INVALID_ADDRESS    	:	s = "INVALID_ADDRESS";		break;

		case J_ST_ERR_CUSTOM_ID      	:	s = "ERR_CUSTOM_ID";		break;
		case J_ST_INVALID_FILENAME   	:	s = "INVALID_FILENAME";		break;
		case J_ST_GC_ERROR           	:	// use J_Factory_GetGenICamErrorInfo() for more detail
											s = "GC_ERROR";				break;
		case J_ST_VALIDATION_ERROR   	:	s = "VALIDATION_ERROR";		break;
		case J_ST_VALIDATION_WARNING 	:	s = "VALIDATION_WARNING";	break;

		default							:	s = "unknown error code";	break;
		}
		return s;
	}

	C_PCHAR_C	get_str_type( J_NODE_TYPE node_type )
	{
		C_PCHAR	s;
		switch( node_type )
		{
		case J_UnknowNodeType			:	s = "Unknow";				break;
		case J_INode					:	s = "Node";					break;
		case J_ICategory				:	s = "Category";				break;
		case J_IInteger					:	s = "Integer";				break;
		case J_IEnumeration				:	s = "Enumeration";			break;
		case J_IEnumEntry      			:	s = "EnumEntry";			break;
		case J_IMaskedIntReg     		:	s = "MaskedIntReg";			break;
		case J_IRegister         		:	s = "Register";				break;
		case J_IIntReg            		:	s = "IntReg";				break;
		case J_IFloat  					:	s = "Float";				break;
		case J_IFloatReg            	:	s = "FloatReg";				break;
		case J_ISwissKnife            	:	s = "SwissKnife";			break;
		case J_IIntSwissKnife          	:	s = "IntSwissKnife";		break;
		case J_IIntKey					:	s = "IntKey";				break;
		case J_ITextDesc  				:	s = "TextDesc";				break;
		case J_IPort					:	s = "Port";					break;
		case J_IConfRom      			:	s = "ConfRom";				break;
		case J_IAdvFeatureLock   		:	s = "AdvFeatureLock";		break;
		case J_ISmartFeature           	:	s = "SmartFeature";			break;
		case J_IStringReg   			:	s = "StringReg";			break;
		case J_IBoolean 				:	s = "Boolean";				break;
		case J_ICommand 				:	s = "Command";				break;
		case J_IConverter 				:	s = "Converter";			break;
		case J_IIntConverter 			:	s = "IntConverter";			break;
		case J_IChunkPort 				:	s = "ChunkPort";			break;
		case J_INodeMap 				:	s = "NodeMap";				break;
		case J_INodeMapDyn 				:	s = "NodeMapDyn";			break;
		case J_IDeviceInfo 				:	s = "DeviceInfo";			break;
		case J_ISelector 				:	s = "ISelector";			break;
		case J_IPortConstruct 			:	s = "PortConstruct";		break;

		default							:	s = "Undefined";			break;
		}
		return s;
	}

	J_BAYER_ALGORITHM bayer_mode[] =
	{
		BAYER_FAST,               //!< Very fast Bayer algorithm that sacrifies the image quality as well as halfing both the horizontal and vertical resolution. This can be used in application that only display the images and where the speed is most important.
		BAYER_SIMPLE,             //!< Simple Bayer algorithm that improves processing speed over the Standard 3x3 Bayer algorithm but sacrifies the image quality. Instead of averaging neighbour pixels this algorithm will simply copy values instead.
		BAYER_STANDARD,           //!< Standard 3x3 Bayer algorithm where the nearest neighbours are averaged to create color information
		BAYER_EXTEND,             //!< Extented 3x3 Bayer algorithm where the nearest neighbour pixels used for averaging are selected based on the gradient. This will help remove zipper-effects on horizontal and vertical edges

		BAYER_FAST_MULTI ,         //!< Very fast Bayer algorithm with multi core processing
		BAYER_SIMPLE_MULTI,       //!< Simple Bayer algorithm with multi core processing
		BAYER_STANDARD_MULTI,     //!< Standard 3x3 Bayer algorithm with multi core processing
		BAYER_EXTEND_MULTI,       //!< Extented 3x3 Bayer algorithm with multi core processing
	};

	#define NODE_NAME_WIDTH         (int8_t*)"Width"
	#define NODE_NAME_HEIGHT        (int8_t*)"Height"
	#define NODE_NAME_PIXELFORMAT   (int8_t*)"PixelFormat"
	#define NODE_NAME_GAIN          (int8_t*)"GainRaw"
	#define NODE_NAME_ACQSTART      (int8_t*)"AcquisitionStart"
	#define NODE_NAME_ACQSTOP       (int8_t*)"AcquisitionStop"

	#define NODE_NAME_FRAMERATE     (int8_t*)"AcquisitionFrameRate"

	#define NODE_NAME_EXPOSURE      (int8_t*)"ExposureTimeRaw"
	#define NODE_NAME_EXPOSURE_MODE (int8_t*)"ExposureMode"

	#define NODE_NAME_PAYLOADSIZE   (int8_t*)"PayloadSize"
}


	CONSTEXPR C_PCHAR_C	JAI_HEADER = "### JAI -> ";
	void	JAI_PRINT( C_PCHAR fmt, ... )
	{
		va_list args;
		va_start( args, fmt );
		HEADER_PRINT_STRING_VA( JAI_HEADER, fmt, args );
		va_end( args );
	}
	void	JAI_PRINT_ERR_DIRECT( C_PCHAR fmt, ... )
	{
		va_list args;
		va_start( args, fmt );
		ERR_HEADER_PRINT_STRING_VA( JAI_HEADER, fmt, args );
		va_end( args );
	}
	void	JAI_PRINT_ERR( J_STATUS_TYPE retval, C_PCHAR fmt, ... )
	{
		va_list args;
		va_start( args, fmt );
		ERR_HEADER_PRINT_STRING_VA( JAI_HEADER, fmt, args );
		ERR_HEADER_PRINT_STRING( JAI_HEADER, "\terror code is %d : %s", retval, get_str_error(retval) );
		va_end( args );
	}

#endif

o_str		c_capture_jai::version;
o_str		c_capture_jai::manufacturer;
bool		c_capture_jai::b_dll_loaded		= false;	//todo wrap it	//	because static link	
//bool		c_capture_jai::b_dll_gui_loaded	= false;
UINT32		c_capture_jai::device_count	= 0;


#if	AAA_USE_JAI()
static	FACTORY_HANDLE  hd_factory = 0;     // Factory Handle

//Utility function to set the frame grabber's width/height (if one is present in the system).
void SetFramegrabberValue(CAM_HANDLE hCam, int8_t* szName, int64_t int64Val, int8_t* sCameraId)
{
	//Set frame grabber value, if applicable
	DEV_HANDLE hDev = nullptr; //If a frame grabber exists, it is at the GenTL "local device layer".
	J_STATUS_TYPE retval = J_Camera_GetLocalDeviceHandle(hCam, &hDev);
	if(J_ST_SUCCESS != retval)
		return;

	if( !hDev )
		return;

	NODE_HANDLE hNode;
	retval = J_Camera_GetNodeByName(hDev, szName, &hNode);
	if(J_ST_SUCCESS != retval)
		return;

	retval = J_Node_SetValueInt64(hNode, false, int64Val);
	if(J_ST_SUCCESS != retval)
		return;

	//Special handling for Active Silicon CXP boards, which also has nodes prefixed
	//with "Incoming":
	std::string strTransportName((char*)sCameraId);
	if(std::string::npos != strTransportName.find("TLActiveSilicon"))
	{
		std::string strName((char*)szName);
		if(std::string::npos != strName.find("Width") 
			|| std::string::npos != strName.find("Height"))
		{
			std::string strIncoming = "Incoming" + strName;
			NODE_HANDLE hNodeIncoming;
			J_STATUS_TYPE retval = J_Camera_GetNodeByName(hDev, (int8_t*)strIncoming.c_str(), &hNodeIncoming);
			if (retval == J_ST_SUCCESS)
			{
				retval = J_Node_SetValueInt64(hNodeIncoming, false, int64Val);
			}
		}

	}//if(std::string::npos != strTransportName.find("TLActiveSilicon"))
}

//Utility function to set the frame grabber's pixel format (if one is present in the system).
void SetFramegrabberPixelFormat( CAM_HANDLE hCam, int8_t* szName, int64_t jaiPixelFormat, int8_t* sCameraId )
{
	DEV_HANDLE hDev = nullptr; //If a frame grabber exists, it is at the GenTL "local device layer".
	J_STATUS_TYPE retval = J_Camera_GetLocalDeviceHandle( hCam, &hDev );
	if( J_ST_SUCCESS != retval )
		return;
	if( !hDev )
		return;

	int8_t szJaiPixelFormatName[512];
	uint32_t iSize = 512;
	retval = J_Image_Get_PixelFormatName( hCam, jaiPixelFormat, szJaiPixelFormatName, iSize );
	if( J_ST_SUCCESS != retval )
		return;

	NODE_HANDLE hLocalDeviceNode = 0;
	retval = J_Camera_GetNodeByName( hDev, (int8_t *)"PixelFormat", &hLocalDeviceNode );
	if( J_ST_SUCCESS != retval )
		return;
	if( 0 == hLocalDeviceNode )
		return;

	//NOTE: this may fail if the camera and/or frame grabber does not use the SFNC naming convention for pixel formats!
	//Check the camera and frame grabber for details.
	retval = J_Node_SetValueString( hLocalDeviceNode, false, szJaiPixelFormatName );
	if( J_ST_SUCCESS != retval )
		return;

	//Special handling for Active Silicon CXP boards, which also has nodes prefixed
	//with "Incoming":
	std::string strTransportName((char*)sCameraId);
	if( std::string::npos != strTransportName.find("TLActiveSilicon") )
	{
		std::string strIncoming = std::string("Incoming") + std::string((char*)szName);
		NODE_HANDLE hNodeIncoming;
		J_STATUS_TYPE retval = J_Camera_GetNodeByName( hDev, (int8_t*)strIncoming.c_str(), &hNodeIncoming );
		if( retval == J_ST_SUCCESS )
		{
			//NOTE: this may fail if the camera and/or frame grabber does not use the SFNC naming convention for pixel formats!
			//Check the camera and frame grabber for details.
			retval = J_Node_SetValueString( hNodeIncoming, false, szJaiPixelFormatName );
		}
	}
}

AAA_ERR	c_capture_jai::get_cam_value( CAM_HANDLE hd_cam, int8_t* name, int64_t& val )
{
	J_STATUS_TYPE retval = J_Camera_GetValueInt64( hd_cam, name, &val );
	if( retval != J_ST_SUCCESS )
	{
		JAI_PRINT_ERR( retval, "Could not get Int64 value %.64s !", name );
		return ERR_ANY;
	}
#ifdef WIN32
	JAI_PRINT( "%.64s is %I64d as hex 0x%I64x", name, val, val );
#else
	JAI_PRINT( "%.64s is %" PRId64 " as hex 0x%" PRIx64, name, val, val );
#endif
	return AAA_OK;
}

AAA_ERR	c_capture_jai::get_cam_value( CAM_HANDLE hd_cam, int8_t* name, double& val )
{
	J_STATUS_TYPE retval = J_Camera_GetValueDouble( hd_cam, name, &val );
	if( retval != J_ST_SUCCESS )
	{
		JAI_PRINT_ERR( retval, "Could not get double value %.64s !", name );
		return ERR_ANY;
	}
	JAI_PRINT( "%.64s is %f", name, val );
	return AAA_OK;
}

AAA_ERR	c_capture_jai::set_cam_value( CAM_HANDLE hd_cam, C_PCHAR_C name, int64_t val )
{
	J_STATUS_TYPE retval = J_Camera_SetValueInt64( hd_cam, (int8_t*)name, val );
	if( retval != J_ST_SUCCESS )
	{
#ifdef WIN32
	JAI_PRINT_ERR( retval, "Could not set Int64 value %I64d for %.64s", val, name );
#else
	JAI_PRINT_ERR( retval, "Could not set Int64 value %" PRId64 " for %.64s", val, name );
#endif		
		return ERR_ANY;
	}
	return AAA_OK;
}

AAA_ERR	c_capture_jai::set_cam_value( CAM_HANDLE hd_cam, C_PCHAR_C name, double val )
{
	J_STATUS_TYPE retval = J_Camera_SetValueDouble( hd_cam, (int8_t*)name, val );
	if( retval != J_ST_SUCCESS )
	{
		JAI_PRINT_ERR( retval, "Could not set double value %f for %.64s !", val, name );
		return ERR_ANY;
	}
	return AAA_OK;
}

AAA_ERR	c_capture_jai::get_cam_pixel_format( CAM_HANDLE hd_cam, int64_t& pixel_format, uint64_t& jai_pixel_format, o_str* o )
{
	pixel_format = 0;
	if( ERR( get_cam_value( hd_cam, NODE_NAME_PIXELFORMAT, pixel_format ) ) )
		return ERR_ANY;

	J_STATUS_TYPE retval = J_Image_Get_PixelFormat( hd_cam, pixel_format, &jai_pixel_format );
	if( J_ST_SUCCESS != retval )
		return ERR_ANY;

	int8_t name[512];
	uint32_t size = 512;
	retval = J_Image_Get_PixelFormatName( hd_cam, jai_pixel_format, name, size );
	if( J_ST_SUCCESS != retval )
		JAI_PRINT_ERR( retval, "Can find pixel format name." );
	else
	{
		// Calculate number of bits (not bytes) per pixel using macro
		int bpp = J_BitsPerPixel( jai_pixel_format );
		JAI_PRINT( "pixel format is %.64s with %d bits per pixel", name, bpp );
		if( o )
		{
			o->set( (CHAR*)name );
			o->add_space();
			o->add( bpp );
		}
	}

	return AAA_OK;
}

void	c_capture_jai::print_cam_nodes( CAM_HANDLE hd_cam )
{
	uint32_t        nodes_nb;
	J_STATUS_TYPE   retval;
	NODE_HANDLE     hd_node;
	int8_t          name[256];
	uint32_t        size;
  
	// Get the number of nodes
	retval = J_Camera_GetNumOfNodes( hd_cam, &nodes_nb );
	if( retval == J_ST_SUCCESS )
	{
		JAI_PRINT( "%u nodes were found",  nodes_nb );
		// Run through the list of nodes and print out the names
		for( uint32_t index = 0; index < nodes_nb; ++index )
		{
			// Get node handle
			retval = J_Camera_GetNodeByIndex( hd_cam, index, &hd_node );
			if( retval == J_ST_SUCCESS )
			{
				// Get node name
				size = sizeof(name);
				retval = J_Node_GetName( hd_node, name, &size, 0 );
				if( retval == J_ST_SUCCESS )
				{
					J_NODE_TYPE node_type;
					retval = J_Node_GetType( hd_node, &node_type );
					if( retval == J_ST_SUCCESS )
						JAI_PRINT( "\tType\t%d", node_type );
					// Print out the name
					JAI_PRINT( "%u NodeName = %.64s\t\t%.64s", index, name, get_str_type(node_type) );

					size = sizeof(name);
					retval = J_Node_GetDisplayName( hd_node, name, &size );
					if( retval == J_ST_SUCCESS )
						JAI_PRINT( "\tDisplayName\t%.64s", name );

					size = sizeof(name);
					retval = J_Node_GetDescription( hd_node, name, &size );
					if( retval == J_ST_SUCCESS )
						JAI_PRINT( "\tDescription\t%.64s", name );
				}
			}
		}
	}
}

void c_capture_jai::print_factory_info_one( int8_t* info, J_FACTORY_INFO info_selector, C_PCHAR_C str )
{
	uint32_t info_size = J_FACTORY_INFO_SIZE;
	J_STATUS_TYPE retval = J_Factory_GetInfo( info_selector, info, &info_size );
	JAI_PRINT( "\t.64%s\t: %.64s", str, info );
}

/*
void CHdrSampleDlg::InitializeControls()
{
	J_STATUS_TYPE   retval;
	NODE_HANDLE hNode;
	int64_t int64Val;

	//- Exposure Time 1 ------------------------------------------------
	// Get SliderCtrl for Exposure Time 1
	pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_EXPOSURE_TIME_SLIDER1);

	// We have two possible ways of setting up Exposure time: JAI or GenICam SFNC
	// The JAI Exposure time setup uses a node called "ShutterMode" and the SFNC 
	// does not need to set up anything in order to be able to control the exposure time.
	// Therefor we have to determine which way to use here.
	// First we see if a node called "ShutterMode" exists.
	hNode = nullptr;
	retval = J_Camera_GetNodeByName( m_hCamera[0], (int8_t*)"ShutterMode", &hNode );

	// Does the "ShutterMode" node exist?
	if( (retval == J_ST_SUCCESS) && !hNode )
	{
		// Here we assume that this is JAI way so we do the following:
		// ShutterMode=ProgrammableExposure
		// Make sure that the ExposureMode selector is set to ProgrammableExposure
		retval = J_Camera_SetValueString( m_hCamera[0], (int8_t*)"ShutterMode", (int8_t*)"ProgrammableExposure" );
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not set ShutterMode=ProgrammableExposure!"), retval);
		}
	}
	else
	{
		ShowErrorMsg(CString("Could not get ShutterMode node!"), retval);
	}

	// Get Exposure Time Raw Node
	retval = J_Camera_GetNodeByName(m_hCamera[0], (int8_t*)"ExposureTimeRaw", &hNode);
	if (retval == J_ST_SUCCESS)
	{
		// Get/Set Min
		retval = J_Node_GetMinInt64(hNode, &int64Val);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not get minimum value for ExposureTimeRaw!"), retval);
		}
		pSCtrl->SetRangeMin((int)int64Val, TRUE);
		SetDlgItemInt(IDC_MIN_STATIC1, (int)int64Val);

		// Get/Set Max
		retval = J_Node_GetMaxInt64(hNode, &int64Val);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not get maximum value for ExposureTimeRaw!"), retval);
		}
		pSCtrl->SetRangeMax((int)int64Val, TRUE);
		SetDlgItemInt(IDC_MAX_STATIC1, (int)int64Val);

		// Get/Set Value
		retval = J_Node_GetValueInt64(hNode, FALSE, &int64Val);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not get value for ExposureTimeRaw!"), retval);
		}
		pSCtrl->SetPos((int)int64Val);
	}
	else
	{
		ShowErrorMsg(CString("Could not get ExposureTimeRaw node!"), retval);
	}

	pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_GAIN_SLIDER1);

	// Get  Gain Raw Node
	retval = J_Camera_GetNodeByName(m_hCamera[0], (int8_t*)"GainRaw", &hNode);
	if (retval == J_ST_SUCCESS)
	{
		// Get/Set Min
		retval = J_Node_GetMinInt64(hNode, &int64Val);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not get minimum value for GainRaw!"), retval);
		}
		pSCtrl->SetRangeMin((int)int64Val, TRUE);
		SetDlgItemInt(IDC_MIN_GAIN_STATIC1, (int)int64Val);

		// Get/Set Max
		retval = J_Node_GetMaxInt64(hNode, &int64Val);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not get maximum value for GainRaw!"), retval);
		}
		pSCtrl->SetRangeMax((int)int64Val, TRUE);
		SetDlgItemInt(IDC_MAX_GAIN_STATIC1, (int)int64Val);

		// Get/Set Value
		retval = J_Node_GetValueInt64(hNode, FALSE, &int64Val);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not get value for GainRaw!"), retval);
		}
		pSCtrl->SetPos((int)int64Val);

	}
	else
	{
		ShowErrorMsg(CString("Could not get ExposureTimeRaw node!"), retval);
	}

	//- Exposure Time 2 ------------------------------------------------

	// Get SliderCtrl for Exposure Time 2
	pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_EXPOSURE_TIME_SLIDER2);

	// We have two possible ways of setting up Exposure time: JAI or GenICam SFNC
	// The JAI Exposure time setup uses a node called "ShutterMode" and the SFNC 
	// does not need to set up anything in order to be able to control the exposure time.
	// Therefor we have to determine which way to use here.
	// First we see if a node called "ShutterMode" exists.
	hNode = nullptr;
	retval = J_Camera_GetNodeByName(m_hCamera[1], (int8_t*)"ShutterMode", &hNode);

	// Does the "ShutterMode" node exist?
	if( (retval == J_ST_SUCCESS) && !hNode )
	{
		// Here we assume that this is JAI way so we do the following:
		// ShutterMode=ProgrammableExposure
		// Make sure that the ExposureMode selector is set to ProgrammableExposure
		retval = J_Camera_SetValueString(m_hCamera[1], (int8_t*)"ShutterMode", (int8_t*)"ProgrammableExposure");
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not set ShutterMode=ProgrammableExposure!"), retval);
		}
	}
	else
	{
		ShowErrorMsg(CString("Could not get ShutterMode node!"), retval);
	}

	// Get  Exposure Time Raw Node
	retval = J_Camera_GetNodeByName(m_hCamera[1], (int8_t*)"ExposureTimeRaw", &hNode);
	if (retval == J_ST_SUCCESS)
	{
		// Get/Set Min
		retval = J_Node_GetMinInt64(hNode, &int64Val);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not get minimum value for ExposureTimeRaw!"), retval);
		}
		pSCtrl->SetRangeMin((int)int64Val, TRUE);
		SetDlgItemInt(IDC_MIN_STATIC2, (int)int64Val);

		// Get/Set Max
		retval = J_Node_GetMaxInt64(hNode, &int64Val);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not get maximum value for ExposureTimeRaw!"), retval);
		}
		pSCtrl->SetRangeMax((int)int64Val, TRUE);
		SetDlgItemInt(IDC_MAX_STATIC2, (int)int64Val);

		// Get/Set Value
		retval = J_Node_GetValueInt64(hNode, FALSE, &int64Val);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not get value for ExposureTimeRaw!"), retval);
		}
		pSCtrl->SetPos((int)int64Val);
	}
	else
	{
		ShowErrorMsg(CString("Could not get ExposureTimeRaw node!"), retval);
	}

	pSCtrl = (CSliderCtrl*)GetDlgItem(IDC_GAIN_SLIDER2);

		// Get  Gain Raw Node
	retval = J_Camera_GetNodeByName(m_hCamera[1], (int8_t*)"GainRaw", &hNode);
	if (retval == J_ST_SUCCESS)
	{
		// Get/Set Min
		retval = J_Node_GetMinInt64(hNode, &int64Val);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not get minimum value for GainRaw!"), retval);
		}
		pSCtrl->SetRangeMin((int)int64Val, TRUE);
		SetDlgItemInt(IDC_MIN_GAIN_STATIC2, (int)int64Val);

		// Get/Set Max
		retval = J_Node_GetMaxInt64(hNode, &int64Val);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not get maximum value for GainRaw!"), retval);
		}
		pSCtrl->SetRangeMax((int)int64Val, TRUE);
		SetDlgItemInt(IDC_MAX_GAIN_STATIC2, (int)int64Val);

		// Get/Set Value
		retval = J_Node_GetValueInt64(hNode, FALSE, &int64Val);
		if (retval != J_ST_SUCCESS)
		{
			ShowErrorMsg(CString("Could not get value for GainRaw!"), retval);
		}
		pSCtrl->SetPos((int)int64Val);
	}
	else
	{
		ShowErrorMsg(CString("Could not get ExposureTimeRaw node!"), retval);
	}
}
*/

void c_capture_jai::print_cam_info_one( t_cam_info& cam_id, int8_t* info, J_CAMERA_INFO info_selector, C_PCHAR_C str )
{
	uint32_t info_size = J_CAMERA_INFO_SIZE;
	J_STATUS_TYPE retval = J_Factory_GetCameraInfo( hd_factory, cam_id, info_selector,	info, &info_size );
	JAI_PRINT( "\t%.64s\t: %.256s", str, info );
}

void c_capture_jai::get_sensor_size( CAM_HANDLE hd_cam, INT32& sx, INT32& sy )
{
	int64_t s;
	get_cam_value( hd_cam, (int8_t*)"SensorWidth", s );
	sx = s;
	get_cam_value( hd_cam, (int8_t*)"SensorHeight", s );
	sy = s;
}	

AAA_ERR	c_capture_jai::open_camera_by_id( t_cam_info& cam_id, CAM_HANDLE& hd_cam, bool& b_can_stream )
{
	JAI_PRINT( "Camera ID: %.128s", cam_id );

	int8_t	info[J_CAMERA_INFO_SIZE];

	print_cam_info_one( cam_id, info, CAM_INFO_MANUFACTURER,	"Manufacturer"		);
	//version.set( (CHAR*)info );
	print_cam_info_one( cam_id, info, CAM_INFO_MODELNAME,		"Model"				);
	print_cam_info_one( cam_id, info, CAM_INFO_USERNAME,		"User"				);
	print_cam_info_one( cam_id, info, CAM_INFO_SERIALNUMBER,	"Serial"			);
	print_cam_info_one( cam_id, info, CAM_INFO_CMD_CUSTOM,		"Command Custom"	);
	print_cam_info_one( cam_id, info, CAM_INFO_IP,				"IP"				);
	print_cam_info_one( cam_id, info, CAM_INFO_MAC,				"MAC"				);
	print_cam_info_one( cam_id, info, CAM_INFO_INTERFACE_ID,	"Interface"			);

	J_STATUS_TYPE retval = J_Camera_Open( hd_factory, cam_id, &hd_cam );
	if( retval != J_ST_SUCCESS )
	{
		JAI_PRINT_ERR( retval, "Could not open the camera !" );
		return ERR_ANY;
	}
	
	//Make sure streaming is supported!
	uint32_t streams_nb = 0;
	retval = J_Camera_GetNumOfDataStreams( hd_cam, &streams_nb );
	if( retval != J_ST_SUCCESS)
		JAI_PRINT_ERR( retval, "Error with J_Camera_GetNumOfDataStreams." );
	b_can_stream = !( streams_nb == 0 );

	INT32 dum;
	get_sensor_size( hd_cam, dum, dum );


	return AAA_OK;
}

AAA_ERR	c_capture_jai::close_camera( CAM_HANDLE& hd_cam )
{
	J_STATUS_TYPE retval = J_Camera_Close(hd_cam);
	if( retval != J_ST_SUCCESS )
	{
		JAI_PRINT_ERR( retval, "Could not close the camera !" );
		return ERR_ANY;
	}
	hd_cam = 0;
	JAI_PRINT( "Closed camera" );

	return AAA_OK;
}


static	std::list<c_capture*>	list_cap_jai;


void	capture_jai_add( c_capture* pt )
{
	if( pt )
		list_cap_jai.push_back( pt );
}

void	capture_jai_remove( c_capture* pt )
{
	if( pt )
		list_cap_jai.remove( pt );
}
#endif

//static	bool		b_enum_first = true;

INT32	c_capture_jai::do_enum( bool b_verbose )
{
	device_count = 0;
#if	AAA_USE_JAI()
	if( !b_dll_loaded )
	{
		JAI_PRINT_ERR_DIRECT( "Jai SDK not initialized," );
		JAI_PRINT_ERR_DIRECT( "\tprobably because start_with_jai OFF in pref." );
		return 0;
	}

//	if( b_enum_first )
//	{
//		b_enum_first = false;
//	}

	J_STATUS_TYPE   retval;
	//Update camera list
	bool8_t	b_has_changed;
	retval = J_Factory_UpdateCameraList( hd_factory, &b_has_changed );
	if( retval != J_ST_SUCCESS )
	{
		JAI_PRINT_ERR( retval, "Could not update camera list !");
		goto exit;
	}
	JAI_PRINT( "Updating camera list succeeded" );

	// Get the number of Cameras
	uint32_t	device_nb;
	retval = J_Factory_GetNumOfCameras( hd_factory, &device_nb );
	if( retval != J_ST_SUCCESS )
	{
		JAI_PRINT_ERR( retval,  "Could not get the number of cameras !" );
		goto exit;
	}
	device_count = device_nb;
	if( device_count == 0 )
	{
		JAI_PRINT( "No Jai camera were found" );
		goto exit;
	}
	JAI_PRINT( "%d cameras were found", device_count );


	CAM_HANDLE	hd_cam;
	bool b_can_stream;
	// Connect to all detected cameras
	for( UINT32 i = 0; i != device_count; ++i )
	{
		// Get camera ID
		t_cam_info		camera_id;

		uint32_t size = (uint32_t)sizeof(camera_id);
		retval = J_Factory_GetCameraIDByIndex( hd_factory, i, camera_id, &size );
		if( retval != J_ST_SUCCESS )
		{
			JAI_PRINT_ERR( retval, "Could not get the camera ID !" );
		}
		else
		{
			b_can_stream = false;
			if( NOERR( open_camera_by_id( camera_id, hd_cam, b_can_stream ) ) )
				close_camera( hd_cam );
		}
	}

exit:
#endif
	return device_count;
}


void	c_capture_jai::c_init()
{
#if	AAA_USE_JAI()
	list_cap_jai.clear();

	if( !b_dll_loaded )
	{
		// Open factory
		J_STATUS_TYPE retval = J_Factory_Open( (int8_t*)"", &hd_factory );
		if (retval != J_ST_SUCCESS)
		{
			JAI_PRINT_ERR( retval, "Could not open Jai factory !" );
			return;
		}
		JAI_PRINT( "Opening Jai factory succeeded" );

		int8_t	info[J_FACTORY_INFO_SIZE];

		print_factory_info_one( info, FAC_INFO_VERSION,			"Version"		);
			version.set( (CHAR*)info );
		print_factory_info_one( info, FAC_INFO_MANUFACTURER,	"Manufacturer"	);
			manufacturer.set( (CHAR*)info );
		print_factory_info_one( info, FAC_INFO_BUILDDATE,		"Date"			);
		print_factory_info_one( info, FAC_INFO_BUILDTIME,		"Time"			);

		b_dll_loaded = true;
	}

	do_enum( true );
#endif
}

void	c_capture_jai::c_deinit()
{
#if	AAA_USE_JAI()
	while( !list_cap_jai.empty() )
	{
		c_capture*	pt;
		pt = *list_cap_jai.begin();
		list_cap_jai.pop_front();		//hack is it thread/callback safe ?
		delete pt;

	}

	if( hd_factory )
	{
		// Close factory
		J_STATUS_TYPE  retval = J_Factory_Close( hd_factory );
		if (retval != J_ST_SUCCESS)
		{
			JAI_PRINT_ERR( retval, "Could not close the factory !" );
		}
		hd_factory = nullptr;
		JAI_PRINT( "Closed Jai factory" );
	}
#endif
}


c_capture_jai::c_capture_jai()
//:_cam_base(nullptr)
{
#if	AAA_USE_JAI()
//	_o_crossbar_name.set( "None" );
	InitializeCriticalSection( &_thread_lock );
#endif
}

c_capture_jai::~c_capture_jai()
{
#if	AAA_USE_JAI()
	close();
	DeleteCriticalSection( &_thread_lock );
#endif
}

/*
//void	c_capture::dlg()
//{
//	dlg_source();
////	dlg_format();
////	dlg_display();
//}

void	c_capture_jai::dlg_source()
{
	//// Show the camera selection dialog.
	//CameraGUIError	guierror;
	//INT32			iDialogStatus  = 0;

	//if( _m_guicontext )
	//{
	//	guierror = pgrcamguiShowCameraSelectionModal( _m_guicontext, _flycapture_context, &_flycapture_serial_number, nullptr ); //&iDialogStatus );
	//	if( guierror != PGRCAMGUI_OK )
	//	{
	//		ERR_PRINT_STRING("Error showing camera selection dialog." );
	//	}
	//}
}

void	c_capture_jai::dlg_display()
{
	//if( _m_guicontext )
	//{
	//	pgrcamguiToggleSettingsWindowState( _m_guicontext, get_window_main_handle() );
	//}
}
*/

//todo
/*
void	__cdecl	flycap2_callback( FlyCapture2::Image* pImage, CONST void* pCallbackData )
{
	c_capture_flycap2*	p_callback;
	p_callback = (c_capture_flycap2*)pCallbackData;
//	printf( "Grabbed image\n" );
	if ( p_callback )
	{
		//CRITICAL_SECTION	thread_lock;
		
		EnterCriticalSection( &p_callback->_thread_lock );
		//tbuf_inc( c_tbuf_master::CH_VIDEO_MOVE_FRAME, 1., "flycap2_move_frame" );
		//FlyCapture2::Image	img_copy;
		// todo make sure we don't need to convert image?
		//was good
		//p_callback->_image.DeepCopy( pImage );
		//p_callback->process_frame_low( &p_callback->_image );
		//	p_callback->got_frame( p_callback->_image.GetData() );
		p_callback->process_frame_low( pImage );
//		p_callback->got_frame( pImage->GetData() );
		//tbuf_dec( c_tbuf_master::CH_VIDEO_MOVE_FRAME, 1., "flycap2_move_frame" );
		LeaveCriticalSection( &p_callback->_thread_lock );
	}
	return;
}
*/

#if	AAA_USE_JAI()
void c_capture_jai::callback_stream( J_tIMAGE_INFO* pAqImageInfo )
{
	if( _view )
		// Shows image
		J_Image_ShowImage( _view, pAqImageInfo );

	if( !_buffer_info.pImageBuffer )
		//J_Image_Malloc( pAqImageInfo, &_buffer_info );
		J_Image_MallocDIB( pAqImageInfo, &_buffer_info );

	// Allocates buffer memory for RGB image.
	if( _buffer_info.pImageBuffer )
	{
		INT32 index = _jai_ui->get_bayer_algo();
		if( _jai_ui->is_bayer_multicore() )
			index += 4;
		J_BAYER_ALGORITHM algo = bayer_mode[ index ];
		// Converts from RAW to full bit image using extended (zipper-effect free) color interpolation.
		//if( J_ST_SUCCESS == J_Image_FromRawToImageEx(pAqImageInfo, &_buffer_info, algo, _gain[0], _gain[1], _gain[2] ) )
		if( J_ST_SUCCESS == J_Image_FromRawToDIBEx( pAqImageInfo, &_buffer_info, algo, _gain[0], _gain[1], _gain[2] ) )
		{
			got_frame( (UINT8*)_buffer_info.pImageBuffer, "Capture Jai", 0, true );
		}
	}
}
#endif


bool	c_capture_jai::run( bool b_stream_in )
{
#if	AAA_USE_JAI()
	if( _b_opened && !_b_running )
	{
		int64_t int64Val;
		SIZE	ViewSize;
		
		if( !_b_can_stream )
		{
			JAI_PRINT_ERR_DIRECT( "Streaming not enabled on this device." );
			return false;
		}

		set_cam_value( _hd_cam, (CHAR*)NODE_NAME_FRAMERATE,		_jai_ui->get_framerate_asked() );

		if( _jai_ui->is_custom_rect() )
		{
			// Set Width value
			set_cam_value( _hd_cam, (CHAR*)NODE_NAME_WIDTH,		(int64_t) _jai_ui->get_size_x() );
			set_cam_value( _hd_cam, (CHAR*)NODE_NAME_HEIGHT,	(int64_t) _jai_ui->get_size_y() );
		}

		// Get Width from the camera
		if( ERR( get_cam_value( _hd_cam, NODE_NAME_WIDTH, int64Val ) ) )
			return false;
		ViewSize.cx = (LONG)int64Val;     // Set window size cx
		//Set frame grabber dimension, if applicable
		SetFramegrabberValue( _hd_cam, NODE_NAME_WIDTH, int64Val, &_camera_id[0] );
	
		// Get Height from the camera
		if( ERR( get_cam_value( _hd_cam, NODE_NAME_HEIGHT, int64Val ) ) )
			return false;
		ViewSize.cy = (LONG)int64Val;     // Set window size cy
		//Set frame grabber dimension, if applicable
		SetFramegrabberValue( _hd_cam, NODE_NAME_HEIGHT, int64Val, &_camera_id[0] );

		// Get the pixel format from the camera
		int64_t pixel_format = 0;
		uint64_t jai_pixel_format = 0;
		if( ERR( get_cam_pixel_format( _hd_cam, pixel_format, jai_pixel_format, _jai_ui->get_stream_format() ) ) )
			return false;
		//Set frame grabber pixel format, if applicable
		SetFramegrabberPixelFormat( _hd_cam, NODE_NAME_PIXELFORMAT, pixel_format, &_camera_id[0] );

		// Calculate number of bits (not bytes) per pixel using macro
		int bpp = J_BitsPerPixel( jai_pixel_format );

		set_flux_size_format( ViewSize.cx, ViewSize.cy, aaa::PIXEL_FORMAT::BGRA_8 );

		J_STATUS_TYPE   retval;

		if( _b_use_window )
		{
			// Set window position
			POINT	TopLeft;
			TopLeft.x = 100;
			TopLeft.y = 50;

			// Open view window
			retval = J_Image_OpenViewWindowA( "Image View Window", &TopLeft, &ViewSize, &_view );
			if( retval != J_ST_SUCCESS )
			{
				JAI_PRINT_ERR( retval, "Could not open view window !" );
				return false;
			}
			JAI_PRINT( "Opening view window succeeded" );
		}

		// Open stream
		retval = J_Image_OpenStream( _hd_cam, 0,	reinterpret_cast<J_IMG_CALLBACK_OBJECT>(this),
													reinterpret_cast<J_IMG_CALLBACK_FUNCTION>(&c_capture_jai::callback_stream),
													&_thread, (ViewSize.cx*ViewSize.cy*bpp)/8
									);
		if( retval != J_ST_SUCCESS )
		{
			JAI_PRINT_ERR( retval, "Could not open stream !" );
			return false;
		}
		JAI_PRINT( "Opening stream succeeded" );

		// Start Acquisition
		retval = J_Camera_ExecuteCommand( _hd_cam, NODE_NAME_ACQSTART );
		if( retval != J_ST_SUCCESS )
		{
			JAI_PRINT_ERR( retval, "Could not Start Acquisition !" );
			return false;
		}

		 _buffer_info.pImageBuffer = nullptr;
		_b_running = true;

/*		if ( _cam_base->IsConnected() )
		{
			CAPTURE_PRINT_STRING( "FlyCap2 : PGR camera is connected" );
		}
		else
		{
			ERR_PRINT_STRING( "FlyCap2 : PGR camera is not connected" );
			goto exit_on_error;
		}

		FlyCapture2::Error		error;
		bool	b_running;
		if( _b_giga_ethernet )
			b_running = set_gige_video_mode();
		else
			b_running = set_video_mode();

		if( !b_running )
		{
			goto exit_on_error;
		}

		if( !_b_giga_ethernet )
		{
			// don't work with gige cameras!!!!	block capture!!!!
			FlyCapture2::FC2Config	config;
			config.grabMode		= FlyCapture2::DROP_FRAMES;
			config.grabTimeout	= 1;
			config.numBuffers	= 10;
			_cam_base->SetConfiguration( &config );
		}
			// Start capturing images
		if ( b_stream_in )
		{
			// SetCallback does not work with 2.6.2.3; need to startcapture with callback
			//error = _cam_base->SetCallback( flycap2_callback, (CONST void*)this );
			//if( error != FlyCapture2::PGRERROR_OK )
			//{
			//	ERR_PRINT_STRING( "FlyCap2, could not set callback (%s)", error.GetDescription() );
			//	goto exit_on_error;
			//}
			_b_streaming = true;
			set_frame_callback( true );
			error = _cam_base->StartCapture( flycap2_callback, (CONST void*)this );
		}
		else
		{
			//error = _cam_base->SetCallback( nullptr, nullptr );
			//if( error != FlyCapture2::PGRERROR_OK )
			//{
			//	ERR_PRINT_STRING( "FlyCap2, could not clear callback (%s)", error.GetDescription() );
			//	goto exit_on_error;
			//}
			set_frame_callback( false );
			_b_streaming = false;
			error = _cam_base->StartCapture();
		}
		//error = _cam_base->StartCapture();
		if( error != FlyCapture2::PGRERROR_OK )
		{
			ERR_PRINT_STRING( "FlyCap2 : could not start capture (%s)", error.GetDescription() );
			goto exit_on_error;
		}

		// Retrieve frame rate property
		FlyCapture2::Property	frmRate;
		frmRate.type = FlyCapture2::FRAME_RATE;
		error = _cam_base->GetProperty( &frmRate );
		if (error != FlyCapture2::PGRERROR_OK )
		{
			ERR_PRINT_STRING( "FlyCap2; error getting framerate (%s)", error.GetDescription() );
			goto exit_on_error;
		}
		_framerate = frmRate.absValue;
		_flycap2->set_framerate( _framerate );
		_b_running = true;
		_time_last = 0;

		set_properties();

		if( !_b_streaming )
			ask_frame();
*/
		return _b_running;
	}
	

//exit_on_error:
		// error during run
	_b_running = false;
#endif
	return _b_running;
}


void	c_capture_jai::stop()
{
#if	AAA_USE_JAI()
	if( _b_running && IS_NOT_NULL( _hd_cam ) )
	{
		J_STATUS_TYPE retval;
		// Stop Acquisition

		retval = J_Camera_ExecuteCommand( _hd_cam, NODE_NAME_ACQSTOP );
		if( retval != J_ST_SUCCESS )
		{
			JAI_PRINT_ERR( retval, "Could not Stop Acquisition !" );
		}

		if( _thread )
		{
			// Close stream
			retval = J_Image_CloseStream( _thread );
			if (retval != J_ST_SUCCESS)
			{
				JAI_PRINT_ERR( retval, "Could not close Stream !" );
			}
			_thread = nullptr;
			JAI_PRINT( "Closed stream" );
		}

		if( _view )
		{
			// Close view window
			retval = J_Image_CloseViewWindow(_view);
			if (retval != J_ST_SUCCESS)
			{
				JAI_PRINT_ERR( retval, "Could not close View Window !" );
			}
			_view = nullptr;
			JAI_PRINT( "Closed view window" );
		}

		J_Image_Free( &_buffer_info );

/*		//// Stop camera grabbing
		FlyCapture2::Error	error;
		error = _cam_base->StopCapture();
		if( error != FlyCapture2::PGRERROR_OK )
		{
			ERR_PRINT_STRING( "FlyCap2, error stopping capture (%s)", error.GetDescription() );
		}
		//error = _cam_base->SetCallback( nullptr, nullptr );
		//if( error != FlyCapture2::PGRERROR_OK )
		//{
		//	ERR_PRINT_STRING( "FlyCap2, could not clear callback (%s)", error.GetDescription() );
		//}
	*/
		_b_running = false;
		_b_streaming = false;
	}
#endif
}


void	c_capture_jai::close_specific()
{
#if	AAA_USE_JAI()
	if( _b_opened )
	{
		stop();

		close_camera( _hd_cam );
	
		_b_opened = false;

//		capture_flycap2_remove( this );
	}
#endif
}

#if	AAA_USE_JAI()
void	c_capture_jai::update_cam_param()
{
	REAL*	p4 = _jai_ui->get_gain();
	DOUBLE	f = p4[3];
	for( INT32 i=0; i<=2; ++i )
		_gain[i] = I_FLOOR( DOUBLE(p4[i]) * f * 0x1000 + .5 );

	DOUBLE v64;
	v64 = _jai_ui->get_black_level() * 511.;
	if( _black_level_last != v64 )
	{
		set_cam_value( _hd_cam, "BlackLevel",	v64		);
		_black_level_last = v64;
	}
	v64 = _jai_ui->get_exposure_time_ms() * 1000.;
	if( _exposure_last != v64 )
	{
		set_cam_value( _hd_cam, "ExposureTime",	v64		);
		_exposure_last = v64;
	}
}
#endif

void	c_capture_jai::update()
{
#if	AAA_USE_JAI()
	if( _b_opened )
	{
		if( _jai_ui->is_print_nodes() )
		{
			_jai_ui->set_print_nodes( false );
			print_cam_nodes( _hd_cam );
		}
	//	if( _b_running && IS_NOT_NULL( _cam_base ) )
		if( _b_running )
		{
			update_cam_param();
/*
		if ( !_cam_base->IsConnected() )
		{
			// Ooops, camera have been disconnected while running
			close();
			return;
		}
		set_properties();
*/
			if ( !_b_streaming )
				ask_frame();
		}
	}
#endif
}



AAA_ERR	c_capture_jai::open_specific( INT32 CONST index, REAL CONST framerate, o_str CONST * CONST device_name )
{
#if	AAA_USE_JAI()
	if( _b_opened )
		return AAA_OK;

	uint32_t size = (uint32_t)sizeof(_camera_id);
	J_STATUS_TYPE retval = J_Factory_GetCameraIDByIndex( hd_factory, index, _camera_id, &size );
	if( retval != J_ST_SUCCESS )
	{
		JAI_PRINT_ERR( retval, "Could not get the camera ID !" );
		return ERR_ANY;
	}

	if( ERR( open_camera_by_id( _camera_id, _hd_cam, _b_can_stream ) ) )
		return ERR_ANY;

	_jai_ui->set_camera_model( (CHAR*)_camera_id );

	INT32 sx, sy;
	get_sensor_size( _hd_cam, sx, sy );
	_jai_ui->set_sensor_size( sx, sy );

	update_cam_param();

	_b_opened = true;
	return AAA_OK;
#else
	return ERR_NOT_COMPILED;
#endif
}

/*
void	c_capture_flycap2::process_frame_low( FlyCapture2::Image* img_fly )
{
	// Get the raw image dimensions
	FlyCapture2::PixelFormat	pixFormat;
	unsigned int				sx, sy, stride;
	img_fly->GetDimensions( &sy, &sx, &stride, &pixFormat );

	CONST UINT8*		src = img_fly->GetData();
	aaa::PIXEL_FORMAT	format = aaa::PIXEL_FORMAT::UNKNOWN;
	switch ( pixFormat )
	{
	case FlyCapture2::PIXEL_FORMAT_MONO8:	format = aaa::PIXEL_FORMAT::R_8;		break;
	case FlyCapture2::PIXEL_FORMAT_RGB8:	format = aaa::PIXEL_FORMAT::RGB;		break;
	case FlyCapture2::PIXEL_FORMAT_BGR:		format = aaa::PIXEL_FORMAT::BGR;		break;
	case FlyCapture2::PIXEL_FORMAT_BGRU:	format = aaa::PIXEL_FORMAT::RGBA;		break;
	case FlyCapture2::PIXEL_FORMAT_RGBU:	format = aaa::PIXEL_FORMAT::BGRA;		break;
	default:
		// Create a converted image
		img_fly->SetColorProcessing( ( FlyCapture2::ColorProcessingAlgorithm ) _flycap2->get_color_processing() );
		// Convert the raw image
		FlyCapture2::Error	error = img_fly->Convert( FlyCapture2::PIXEL_FORMAT_RGBU, &_image_converted );		// was PIXEL_FORMAT_BGRU ???
		if ( error != FlyCapture2::PGRERROR_OK )
		{
			return;
		}
		format = aaa::PIXEL_FORMAT::RGBA;
		src = _image_converted.GetData();
	}
	//st_frame_info frame_info;
	//_frame_info.size_x			= sx;
	//_frame_info.size_y			= sy;
	//_frame_info.src				= src;
	//_frame_info.pixel_format	= _src_pixel_format;
	//_frame_info.pitch			= stride;
	//set_src_pitch( stride );
	set_flux_size_format( sx, sy, format );
	got_frame( src, "FlyCap2 image", stride );

}
*/

void	c_capture_jai::ask_frame()
{
#if	AAA_USE_JAI()
	//if ( _b_streaming )
	//{
	//	// don't ask frame in callback mode
	//	return;
	//}

/*
//
//	printf( "flycapture::ask_frame" );
	FlyCapture2::Error	error;
	FlyCapture2::Image	raw_image;

	// Grab an image
	TBUF_INC( tbuf::CH_CAPTURE_CALLBACK, 1., "flycap2" );
	TBUF_INC( tbuf::CH_VIDEO_GET_FRAME, 1., "flycap2_get_frame" );

	if( _cam_base )
	{
		// Retrieve an image
		error = _cam_base->RetrieveBuffer( &raw_image );
		TBUF_DEC( tbuf::CH_VIDEO_GET_FRAME, 1., "flycap2_get_frame" );
	
		if( error != FlyCapture2::PGRERROR_OK )
		{
			//ERR_PRINT_STRING( "Error retrieving buffer (%s)", error.GetDescription() );
			if ( error != FlyCapture2::PGRERROR_TIMEOUT )
			{
				ERR_PRINT_STRING( "FlyCap2, error retrieving buffer (%s)", error.GetDescription() );
			//	_framerate = 0.0f;
			//	ERR_PRINT_STRING( "FlyCap2 : could not get image" );
			//	ERR_PRINT_STRING( "FlyCap2 : %s", error.GetDescription() );
			}
		}
		else
		{
			process_frame_low( &raw_image );
		}
		_flycap2->set_framerate( _framerate );
	}
*/
	TBUF_DEC( tbuf::CH_CAPTURE_CALLBACK, 0., "Jai" );
#endif
}


FACTORY_CREATE_V1( c_jai_ui, jai_ui, Jai Camera Sdk, cap_jai );

namespace	n_jai
{

	INT32 CONST BAYER_ALGO_NB = 4;
	C_PCHAR_C	bayer_algo_str[ BAYER_ALGO_NB ] =
	{
		"FAST",
		"SIMPLE",
		"STANDARD",
		"EXTENDED"
	};

	CONSTEXPR INT32	BASE_PARAM_NB		= 5;
	CONSTEXPR INT32	CUSTOM_PARAM_NB		= 4;
	CONSTEXPR INT32	REALTIME_PARAM_NB	= 8;
	CONSTEXPR INT32	INFO_PARAM_NB		= 3;
	CONSTEXPR INT32	GROUP_NB			= 3;

	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	CUSTOM_PARAM_NB
									+	REALTIME_PARAM_NB
									+	INFO_PARAM_NB
									+	GROUP_NB;


	CONST c_param_def param[PARAM_NB_MAX] =
	{
	//	PARAM_DEF_BOOL_OFF(			open_from_serial	)
		PARAM_DEF_STR_LOCKED(		camera_id			)
		PARAM_DEF_INT32_LOCKED_XY(	sensor_size			)
		PARAM_DEF_BOOL_OFF(			print_nodes_trig	)
		PARAM_DEF_STR_LOCKED(		stream_format		)
	
//		PARAM_DEF_SYMBO(		resolution,			4, 0.,		c_flycap2_ui::FLYCAP2_CAM_RES_MAX-1, c_flycap2_ui::flycap2_camera_resolution_str )
//		PARAM_DEF_SYMBO(		framerate,			4, 0.,		c_flycap2_ui::FLYCAP2_FRAME_RATE_MAX-1, c_flycap2_ui::flycap2_framerate_str )
//		PARAM_DEF_SYMBO(		color_processing,	0, 1,		COLOR_PROCESSING_NB-1, flycap2_color_processing_str )
//		PARAM_DEF_REAL_ZERO_ONE( bus_speed							)
//		PARAM_DEF_REAL_LOCKED( framerate_real )

		PARAM_DEF_GROUP_CLOSED( Custom, CUSTOM_PARAM_NB )
			PARAM_DEF_REAL(			framerate_asked,	25., 30.,	1.,	1000000.		)	//yes we are optimist and long term
			PARAM_DEF_BOOL_OFF(		define_offset_size						)
//			PARAM_DEF_INT32(		x_offset,		0, 0,		0, 1024*1024 )	//yes we are optimist and long term
			PARAM_DEF_INT32(		x_size,			2, 1280,	2, 1024*1024 )
//			PARAM_DEF_INT32(		y_offset,		0, 0,		0, 1024*1024 )
			PARAM_DEF_INT32(		y_size,			2, 720,		2, 1024*1024 )
//			PARAM_DEF_SYMBO(		pixel_format,	4, 0,		c_flycap2_ui::FLYCAP2_PIXEL_FORMAT_MAX-1, c_flycap2_ui::flycap2_pixel_format_str )

		PARAM_DEF_GROUP_CLOSED( Real Time, REALTIME_PARAM_NB )
			PARAM_DEF_REAL_ONE_ZERO(	black_level )
			PARAM_DEF_REAL(				exposure_time_ms,		1, 10,		0, 1000. )
			PARAM_DEF_SYMBO(			bayer,			1, 3,		BAYER_ALGO_NB-1, bayer_algo_str )
			PARAM_DEF_BOOL_OFF(			bayer_multicore	)
			PARAM_DEF_COLOR_RGBF(		gain			)

		PARAM_DEF_GROUP_CLOSED( Info, INFO_PARAM_NB )
			PARAM_DEF_STR_LOCKED(		manufacturer	)
			PARAM_DEF_STR_LOCKED(		Dll_version		)
			PARAM_DEF_INT32_LOCKED(		camera_nb		)
/*			PARAM_DEF_INT32_POS_ZERO(	SerialNumber	)
			PARAM_DEF_STR_LOCKED(		CameraModel		)
			PARAM_DEF_STR_LOCKED(		CameraVendor	)
			PARAM_DEF_STR_LOCKED(		Sensor			)
			PARAM_DEF_SYMBO_LOCKED(		Interface,	0, 1, INTERFACE_TYPE_NB-1, flycap2_interface_type_str )
*/	};
}

void	c_jai_ui::param_init_pt()
{
	INT32	h = 0;

//	param_set_pt(	h, _b_open_from_serial_ui	);
	param_set_pt(	h, _camera_model		);
	param_set_pt_2(	h, _sensor_size			);
	param_set_pt(	h, _b_print_nodes_trig	);
	param_set_pt(	h, _stream_format		);
	
//	param_set_pt( h, _resolution_ui			);
//	param_set_pt( h, _framerate_ui			);
//	param_set_pt( h, _color_processing_ui	);
//	param_set_pt( h, _bus_speed_ui			);
	

	++h;
		param_set_pt( h, _framerate_asked_ui	);
		param_set_pt( h, _b_custom_rect_ui		);
//		param_set_pt( h, _offset_x_ui			);
		param_set_pt( h, _size_x_ui				);
//		param_set_pt( h, _offset_y_ui			);
		param_set_pt( h, _size_y_ui				);
//		param_set_pt( h, _pixel_format_ui		);


	++h;
		param_set_pt(	h, _black_level_ui			);
		param_set_pt(	h, _exposure_time_ms_ui		);
		param_set_pt(	h, _s_bayer_algo_ui			);
		param_set_pt(	h, _b_bayer_multicore_ui	);
		param_set_pt_4(	h, _gain_ui					);

	++h;
		param_set_pt( h, c_capture_jai::manufacturer	);
		param_set_pt( h, c_capture_jai::version			);
		param_set_pt( h, c_capture_jai::device_count	);	

/*		param_set_pt( h, _serial_number		);
		param_set_pt( h, _camera_model		);
		param_set_pt( h, _camera_vendor		);
		param_set_pt( h, _camera_sensor		);
		param_set_pt( h, _interface_type	);
*/
	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE( c_jai_ui )
//,_serial_number(0)
//,_framerate_ui(.0)
{
	param_init_with( n_jai::param, n_jai::PARAM_NB_MAX );
}

EMPTY_DESTRUCTOR( c_jai_ui )


/*
void	c_flycap2_ui::set_info( CONST FlyCapture2::CameraInfo* pInfo)
{
	_serial_number = pInfo->serialNumber;
	_camera_model.set( pInfo->modelName );
	_camera_vendor.set( pInfo->vendorName );
	_camera_sensor.set( pInfo->sensorInfo );
}
*/


