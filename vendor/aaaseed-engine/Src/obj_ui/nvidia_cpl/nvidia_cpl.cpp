#include "nvidia_cpl.h"
#include "infrastructure/param/param_declare.h"
#include "NvCpl.h"

#include "obj_ui/tracker/trackers.h"

#define	NV_CPL_HEADER  "# NVIDIA CPL "
void	NV_CPL_PRINT_STRING( C_PCHAR_C fmt, ... )
{
	va_list args;
	va_start( args, fmt );
	trackers::PRINT_STRING_VA( NV_CPL_HEADER, fmt, args );
	va_end( args );
}

fNvGetDisplayInfo					pfNvGetDisplayInfo						= nullptr;
NvCplGetDataIntType					pfNvCplGetDataInt						= nullptr;
NvCplSetDataIntType					pfNvCplSetDataInt						= nullptr;
fdtcfgex							pfNVdtcfgex								= nullptr;
fNvColorGetGammaRampEx				pfNvColorGetGammaRampEx					= nullptr;
fGetdtcfgLastErrorEx				pfGetdtcfgLastErrorEx					= nullptr;
fNvCplGetRealConnectedDevicesString	pfNvCplGetRealConnectedDevicesString	= nullptr;
fNvGetPhysicalConnectorInfo			pfNvGetPhysicalConnectorInfo			= nullptr;
fNvEnumPhysicalConnectorDetails		pfNvEnumPhysicalConnectorDetails		= nullptr;

// Display Mode
// Single or Multi-Monitor Display Mode
enum NV_DISPLAY_MODE : INT32
{
	
	NV_DISPLAY_MODE_NONE = 0,
	NV_DISPLAY_MODE_STANDARD,
	NV_DISPLAY_MODE_CLONE,
	NV_DISPLAY_MODE_HSPAN,
	NV_DISPLAY_MODE_VSPAN,
	NV_DISPLAY_MODE_DUALVIEW,
	NV_DISPLAY_MODE_MAX_NB,
};

C_PCHAR_C	str_nv_display_mode[NV_DISPLAY_MODE_MAX_NB] =
{
	"None/Unknown",
	"Single",
	"Clone",
	"Horizontal Span",
	"Vertical Span",
	"DualView",
};

// Display Type
enum NV_DISPLAY_TYPE : INT32
{
	NV_DISPLAY_TYPE_NONE = 0,
	NV_DISPLAY_TYPE_CRT,
	NV_DISPLAY_TYPE_DFP,
	NV_DISPLAY_TYPE_DFP_LAPTOP,
	NV_DISPLAY_TYPE_TV,
	NV_DISPLAY_TYPE_HDTV,
	NV_DISPLAY_TYPE_MAX_NB,
};

C_PCHAR_C	str_nv_display_type[NV_DISPLAY_TYPE_MAX_NB] =
{
	"None/Unknown",
	"CRT",
	"DFP",
	"Laptop DFP",
	"TV",
	"HDTV",
};

// Antialiasing
enum NV_AA_METHOD : INT32
{
	NV_AA_METHOD_OFF = 0,
	NV_AA_METHOD_2X,
	NV_AA_METHOD_2XQ,
	NV_AA_METHOD_4X,
	NV_AA_METHOD_4X_GAUSSIAN,
	NV_AA_METHOD_4XS,
	NV_AA_METHOD_6XS,
	NV_AA_METHOD_8XS,
	NV_AA_METHOD_16XS,
	NV_AA_METHOD_UNKNOWN,
	NV_AA_METHOD_MAX_NB,
};

C_PCHAR_C	str_nv_antialiasing[NV_AA_METHOD_MAX_NB] =
{
	"Off",
	"2x",
	"2x Quincunx",
	"4x",
	"4x 9 Tap",
	"4xS",
	"6xS",
	"8xS",
	"16xS",
	"Unknown",
};

enum NV_SLI_MODE : INT32
{
	NV_SLI_MODE_SINGLE_GPU = 0,
	NV_SLI_MODE_AUTO_SELECT,
	NV_SLI_MODE_AFR,
	NV_SLI_MODE_SFR,
	NV_SLI_UNKNOWN,
	NV_SLI_MAX_NB,
};

C_PCHAR_C	str_nv_sli_mode[NV_SLI_MAX_NB] = 
{
	"Single GPU",
	"Auto-select",
	"Alternate Frame Rendering",
	"Split Frame Rendering",
	"Unknown Mode",
};


enum NV_CONNECTOR_LAYOUT : INT32
{
	NV_CONNECTORLAYOUT_UNKNOWN =  0,
	NV_CONNECTORLAYOUT_CARD_SINGLESLOT,
	NV_CONNECTORLAYOUT_CARD_DOUBLESLOT,
	NV_CONNECTORLAYOUT_CARD_MOBILE_MXM,
	NV_CONNECTORLAYOUT_CARD_MOBILE_OEM,
	NV_CONNECTORLAYOUT_MOBILE_BACK,
	NV_CONNECTORLAYOUT_MOBILE_BACK_LEFT,
	NV_CONNECTORLAYOUT_MOBILE_BACK_DOCK,
	NV_CONNECTORLAYOUT_NFORCE_STANDARD,
	NV_CONNECTORLAYOUT_MAX_NB,
};

C_PCHAR_C	str_nv_connector_layout[NV_CONNECTORLAYOUT_MAX_NB] =
{
	"Unknown",
	"Single slot add-in card",
	"Double slot add-in card",
	"Mobile MXM module",
	"Mobile add-in card for OEMs",
	"Mobile displays back",
	"Mobile displays back & left",
	"Mobile displays back / dock",
	"nForce",
};

// Physical Connector Type
enum NV_CONNECTOR_TYPE : INT32
{
	NV_CONNECTOR_TYPE_UNKNOWN = 0,
	NV_CONNECTOR_TYPE_UNCLASSIFIED_ANALOG,
	NV_CONNECTOR_TYPE_UNCLASSIFIED_DIGITAL,
	NV_CONNECTOR_TYPE_UNCLASSIFIED_TV,
	NV_CONNECTOR_TYPE_UNCLASSIFIED_LVDS,
	NV_CONNECTOR_TYPE_VGA,
	NV_CONNECTOR_TYPE_DVI_A,
	NV_CONNECTOR_TYPE_DVI_D,
	NV_CONNECTOR_TYPE_DVI_I,
	NV_CONNECTOR_TYPE_DVI_I_TV_SVIDEO,
	NV_CONNECTOR_TYPE_DVI_I_TV_COMPOSITE,
	NV_CONNECTOR_TYPE_DVI_I_TV_SVIDEO_BREAKOUT_COMPOSITE,
	NV_CONNECTOR_TYPE_LFH_DVI_I_1,
	NV_CONNECTOR_TYPE_LFH_DVI_I_2,
	NV_CONNECTOR_TYPE_LVDS_SPWG,
	NV_CONNECTOR_TYPE_LVDS_OEM,
	NV_CONNECTOR_TYPE_TMDS_OEM,
	NV_CONNECTOR_TYPE_ADC,
	NV_CONNECTOR_TYPE_TV_COMPOSITE,
	NV_CONNECTOR_TYPE_TV_SVIDEO,
	NV_CONNECTOR_TYPE_TV_SVIDEO_BREAKOUT_COMPOSITE,
	NV_CONNECTOR_TYPE_TV_SCART,
	NV_CONNECTOR_TYPE_PC_YPRPB,
	NV_CONNECTOR_TYPE_PC_SVIDEO,
	NV_CONNECTOR_TYPE_PC_COMPOSITE,
	NV_CONNECTOR_TYPE_STEREO,
	NV_CONNECTOR_TYPE_MAX_NB,
} ;

// Physical Connector Type
C_PCHAR_C	str_nv_connector_type[NV_CONNECTOR_TYPE_MAX_NB] = 
{
	"Unknown",
	"Unclassifiable Analog",
	"Unclassifiable Digital",
	"Unclassifiable TV",
	"Unclassifiable LVDS",
	"VGA 15-pin",
	"DVI-A - DVI Analog",
	"DVI-D - DVI Digital",
	"DVI-I - DVI Integrated",
	"DVI-I-TV S-Video",
	"DVI-I-TV Composite",
	"DVI-I-TV S-Video Breakout (Composite)",
	"LFH-DVI-I-1 - 60-pin LFH (as in Quadro NVS series)",
	"LFH-DVI-I-2 - 60-pin LFH",
	"LVDS-SPWG - Low Voltage Differential Signaling (as in laptop panels)",
	"LVDS-OEM  - Low Voltage Differential Signaling",
	"TMDS-OEM  - Transition Minimized Differential Signaling",
	"Apple Display Connector (ADC)",
	"TV - Composite Out",
	"TV - S-Video Out",
	"TV - S-Video Breakout (Composite)",
	"TV - SCART D-Connector",
	"Personal Cinema - YPrPb",
	"Personal Cinema - S-Video",
	"Personal Cinema - Composite",
	"3-Pin DIN Stereo",
} ;

/*
c_factory<c_nvidia_cpl_screen>&	c_nvidia_cpl_screen::the_factory()
	{
	static	c_factory<c_nvidia_cpl_screen> f( "nvidia_cpl_screen", nullptr, "Nvidia Display", "nvidia_cpl_screen", nullptr );
		;
	return f;
	}
//&(SUPER::the_factory())
*/
FACTORY_CREATE_V1( c_nvidia_cpl_screen, nvidia_cpl_screen, Nvidia Display, nvidia_cpl_screen );

namespace n_nvidia_cpl_screen
{
	CONSTEXPR INT32	BASE_PARAM_NB				= 3;
	CONSTEXPR INT32	GROUP_NB					= 4;
	CONSTEXPR INT32	BRIGHTNESS_PARAM_NB			= 3;
	CONSTEXPR INT32	GAMMA_PARAM_NB				= 3;
	CONSTEXPR INT32	CONTRAPARAM_DEF_NB			= 3;
	CONSTEXPR INT32	INFO_PARAM_NB				= 5;
//	CONSTEXPR INT32	COLOR_CORRECTION_PARAM_NB	= 2 + BRIGHTNESS_PARAM_NB + GAMMA_PARAM_NB + CONTRAPARAM_DEF_NB + 3;
	CONSTEXPR INT32	PARAM_NB_MAX	=	BASE_PARAM_NB
									+	INFO_PARAM_NB
									+	BRIGHTNESS_PARAM_NB
									+	CONTRAPARAM_DEF_NB
									+	GAMMA_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_STR_LOCKED(	Display_Name )
		PARAM_DEF_INT32_LOCKED(	Display_Nb )
		PARAM_DEF_GROUP(			Info,	INFO_PARAM_NB )
			PARAM_DEF_STR_LOCKED(			Display_Vendor )
			PARAM_DEF_STR_LOCKED(			Display_Model )
			PARAM_DEF_SYMBO_LOCKED_PSTR(	Display_Type,	str_nv_display_type	)
			PARAM_DEF_STR_LOCKED(			Resolution )
			PARAM_DEF_REAL_LOCKED(			Frequency )
		//PARAM_DEF_GROUP( ColorCorrection, COLOR_CORRECTION_PARAM_NB )
		//	{	nullptr,	PARAM_INT32,		"Digital_Vibrance",		1,	0,		0.,	NV_DISPLAY_DIGITAL_VIBRANCE_MAX,	nullptr, nullptr },
		//todo these three group have the same names for parameter
		PARAM_DEF_GROUP( Brightness, BRIGHTNESS_PARAM_NB )
			PARAM_DEF_REAL( brightness_red,				1,	.5,		0,	1 )
			PARAM_DEF_REAL( brightness_green,			1,	.5,		0,	1 )
			PARAM_DEF_REAL( brightness_blue,				1,	.5,		0,	1 )
		PARAM_DEF_GROUP( Contrast, CONTRAPARAM_DEF_NB )
			PARAM_DEF_REAL( contrast_red,				1,	.5,		0,	1 )
			PARAM_DEF_REAL( contrast_green,				1,	.5,		0,	1 )
			PARAM_DEF_REAL( contrast_blue,				1,	.5,		0,	1 )
		PARAM_DEF_GROUP( Gamma, GAMMA_PARAM_NB )
			PARAM_DEF_REAL( gamama_red,					.8,	1,		.5, 6. )
			PARAM_DEF_REAL( gamma_green,					.8,	1,		.5, 6. )
			PARAM_DEF_REAL( gamma_blue,					.8,	1,		.5, 6. )
		PARAM_DEF_REAL_ONE_ZERO( Image_Sharpening )
	};
}

void	c_nvidia_cpl_screen::param_init_pt()
{
	INT32	h=0;

	param_set_pt( h, str_nv_display_name_ );
	param_set_pt( h, screen_number_ );
	++h;
		param_set_pt( h, str_nv_display_vendor_ );
		param_set_pt( h, str_nv_display_model_ );
		param_set_pt( h, display_type_ );
		param_set_pt( h, resolution_ );
		param_set_pt( h, frequency_ );
	++h;
		param_set_pt_3( h, brightness_ui_ );
	++h;	
		param_set_pt_3( h, contrast_ui_ );
	++h;	
		param_set_pt_3( h, gamma_ui_ );

	param_set_pt( h, image_sharpening_ui_ );

	err_param_init_pt( h );
}

CONSTRUCTOR_CREATE(c_nvidia_cpl_screen)
,screen_index_(0)
,display_type_(NV_DISPLAY_TYPE_NONE)
{
	display_type_ = NV_DISPLAY_TYPE_NONE;
	frequency_ = 0.;
	_b_valid = false;
	screen_number_ = 0;

	param_init_with( n_nvidia_cpl_screen::param, n_nvidia_cpl_screen::PARAM_NB_MAX );
}

void	c_nvidia_cpl_screen::set_index( INT32 index )
{
	screen_index_ = index;
	get_display_info();
	get_color_correction();
}

void	c_nvidia_cpl_screen::close()
{
	// Restore Color Correction
	if( screen_index_ )
		restore_color_correction();
}

c_nvidia_cpl_screen::~c_nvidia_cpl_screen()
{
	close();
}

void	c_nvidia_cpl_screen::set_nv_dtcfgex( C_PCHAR_C command, C_PCHAR_C color, REAL value )
{
	CHAR set[1024];

	snprintf( set, sizeof(set)-1, "%s %s %s %f", command, moniker_, color, value );
//	sprintf( set, "%s %d %s %f", command, screen_index, color, value );
	pfNVdtcfgex( set );
}

REAL	c_nvidia_cpl_screen::get_nv_dtcfgex( C_PCHAR_C command, C_PCHAR_C color )
{
	CHAR get[1024];

	snprintf( get, sizeof(get)-1, "%s %s %s", command, moniker_, color );
//	sprintf( get, "%s %d %s", command, screen_index, color );
	pfNVdtcfgex( get );
	return REAL( atof(get) );
}

void	c_nvidia_cpl_screen::get_display_info()
{
	CHAR str_connected_devices[1024];

	if( pfNvCplGetRealConnectedDevicesString( str_connected_devices, sizeof( str_connected_devices ), FALSE /*bOnlyActive*/ ) )
	{
//		NV_CPL_PRINT_STRING( "Screen %d, Connected Devices : %s", screen_index_, str_connected_devices );
	}
	//if (NvCplGetRealConnectedDevicesString(szConnectedDevices,
	//	sizeof(szConnectedDevices), TRUE/*bOnlyActive*/) == TRUE)
	//{
	//	printf( * Connected Devices (Active) : %s\n,
	//		szConnectedDevices);
	//}
	//printf(\n);
	//printf( Relative to active devices,\n
	//	 where AA is first active device\n
	//	 and -AA means no connection.\n);
	//if (NvCplGetActiveDevicesString(szConnectedDevices,
	//	sizeof(szConnectedDevices)) == TRUE)
	//{
	//	NVI D I A C o r p o r a t i o n 129
	//		N V C P L . D L L A P I M a n u a l - V e r s i o n 1 3 . 0
	//		U t i l i t i e s a n d A P I s C o d e S a m p l e s
	//		printf( * Active Devices : %s\n,
	//		szConnectedDevices);
	//}
	//printf(\n);
	//printf( Output from NvCplGetMSOrdinalDeviceString,\n
	//	 where the first listed device is primary\n
	//	 and the later is secondary if shown.\n) ;

	NVDISPLAYINFO displayInfo = {0};
	// displayInfo.cbSize must be set to size of structure
	// displayInfo.dwInputFields1 must be set before call to indicate which fields to retrieve
	// displayInfo.dwOutputFields1 will be set on return to indicate which fields were successfully retrieved
	// see NVDISPLAYINFO1_* bit definitions for field information, use 0xffffffff to retrieve all fields
	memset( &displayInfo, 0, sizeof( displayInfo ) );
	displayInfo.cbSize = sizeof( displayInfo );
	displayInfo.dwInputFields1 = 0xffffffff; // 0xffffffff means all fields should be retrieved
	displayInfo.dwInputFields2 = 0xffffffff; // 0xffffffff means all fields should be retrieved

	CHAR set[10];

//todo we may have to use device number in case of SLI

	if( strlen( str_connected_devices ) == 0 )
	{
		ERR_PRINT_STRING( "NvCplGetRealConnectedDevicesString returned an empty string" );
		return;
	}
	CHAR*	token;
	token = strtok( str_connected_devices, "," );
	for( INT32 i = 1; i < screen_index_; ++i )
	{
		// Get next token: 
		token = strtok( nullptr, "," );
	}
	if( !token )		//hack
	{
		ERR_PRINT_STRING( "c_nvidia_cpl_screen::get_display_info() no token");
		return;
	}
	sprintf( set, "%9s", token );
	sprintf( moniker_, "%5s", token );
//	sprintf( set, "%d", screen_index );
	if( !pfNvGetDisplayInfo( set, &displayInfo ) )
		NV_CPL_PRINT_STRING( "Unable to retrieve display info." );
	else
	{
		_b_valid = true;
		str_nv_display_name_.set( displayInfo.szDisplayName );
		str_nv_display_vendor_.set( displayInfo.szVendorName );
		str_nv_display_model_.set( displayInfo.szModelName );

		screen_number_ = displayInfo.dwWindowsMonitorNumber;

		NV_CPL_PRINT_STRING( "Screen %d moniker : %s", screen_number_, moniker_ );

		CHAR	str[256];
		sprintf( str, "%dx%d", displayInfo.dwPelsWidth, displayInfo.dwPelsHeight );
		resolution_.set( str );
		frequency_ = REAL(displayInfo.dwDisplayFrequency);

		switch ( displayInfo.nDisplayType )
		{
			case NVDISPLAYTYPE_NONE :		display_type_ = NV_DISPLAY_TYPE_NONE;			break;
			case NVDISPLAYTYPE_CRT	:		display_type_ = NV_DISPLAY_TYPE_CRT;			break;
			case NVDISPLAYTYPE_DFP	:		display_type_ = NV_DISPLAY_TYPE_DFP;			break;
			case NVDISPLAYTYPE_DFP_LAPTOP :	display_type_ = NV_DISPLAY_TYPE_DFP_LAPTOP;		break;
			case NVDISPLAYTYPE_TV	:		display_type_ = NV_DISPLAY_TYPE_TV;				break;
			case NVDISPLAYTYPE_TV_HDTV :	display_type_ = NV_DISPLAY_TYPE_HDTV;			break;
			default	:						display_type_ = NV_DISPLAY_TYPE_NONE;			break;
		}
		
		C_PCHAR str_tmp;
		switch( displayInfo.nTvFormat )
		{
			case NVTVFORMAT_NTSC_M :	str_tmp = "NTSC/M" ;			break;
			case NVTVFORMAT_NTSC_J :	str_tmp = "NTSC/J" ;			break;
			case NVTVFORMAT_PAL_M :		str_tmp = "PAL/M" ;				break;
			case NVTVFORMAT_PAL_A :		str_tmp = "PAL/B, D, G, H, I" ;	break;
			case NVTVFORMAT_PAL_N :		str_tmp = "PAL/N" ;				break;
			case NVTVFORMAT_PAL_NC :	str_tmp = "PAL/NC" ;			break;
			case NVTVFORMAT_HD576i :	str_tmp = "HDTV 576i" ;			break;
			case NVTVFORMAT_HD480i :	str_tmp = "HDTV 480i" ;			break;
			case NVTVFORMAT_HD480p :	str_tmp = "HDTV 480p" ;			break;
			case NVTVFORMAT_HD576p :	str_tmp = "HDTV 576p" ;			break;
			case NVTVFORMAT_HD720p :	str_tmp = "HDTV 720p" ;			break;
			case NVTVFORMAT_HD1080i :	str_tmp = "HDTV 1080i" ;		break;
			case NVTVFORMAT_HD1080p :	str_tmp = "HDTV 1080p" ;		break;
			case NVTVFORMAT_HD720i :	str_tmp = "HDTV 720i" ;			break;
			default :					str_tmp = "No Format" ;			break;	//todo unkwonded format
		}
		str_nb_tv_format_.set( str_tmp );
	}
}

void	c_nvidia_cpl_screen::restore_color_correction()
{
	//// Digital vibrance
	//set_nv_dtcfgex( "setdvc", "", digital_vibrance_orig );
	// Brightness
	set_nv_dtcfgex( "set_normalize_brightness", "red", brightness_orig_[0] );
	set_nv_dtcfgex( "set_normalize_brightness", "green", brightness_orig_[1] );
	set_nv_dtcfgex( "set_normalize_brightness", "blue", brightness_orig_[2] );
	// Contrast
	set_nv_dtcfgex( "set_normalize_contrast", "red", contrast_orig_[0] );
	set_nv_dtcfgex( "set_normalize_contrast", "green", contrast_orig_[1] );
	set_nv_dtcfgex( "set_normalize_contrast", "blue", contrast_orig_[2] );
	// Gamma
	set_nv_dtcfgex( "setgamma", "red", gamma_orig_[0] );
	set_nv_dtcfgex( "setgamma", "green", gamma_orig_[1] );
	set_nv_dtcfgex( "setgamma", "blue", gamma_orig_[2] );
	// Sharpness
	set_nv_dtcfgex( "set_normalize_sharpness", "", image_sharpening_orig_ );
}

void	c_nvidia_cpl_screen::get_color_correction()
{
	//// Digital vibrance
	//digital_vibrance_orig = digital_vibrance = get_nv_dtcfgex( "getdvcvalue", "" );
	// Brightness
	brightness_orig_[0] = brightness_[0] = get_nv_dtcfgex( "get_normalize_brightnessvalue", "red" );
	brightness_orig_[1] = brightness_[1] = get_nv_dtcfgex( "get_normalize_brightnessvalue", "green" );
	brightness_orig_[2] = brightness_[2] = get_nv_dtcfgex( "get_normalize_brightnessvalue", "blue" );
	// Contrast
	contrast_orig_[0] = contrast_[0] = get_nv_dtcfgex( "get_normalize_contrastvalue", "red" );
	contrast_orig_[1] = contrast_[1] = get_nv_dtcfgex( "get_normalize_contrastvalue", "green" );
	contrast_orig_[2] = contrast_[2] = get_nv_dtcfgex( "get_normalize_contrastvalue", "blue" );
	// Gamma
	gamma_orig_[0] = gamma_[0] = get_nv_dtcfgex( "getgammavalue", "red" );
	gamma_orig_[1] = gamma_[1] = get_nv_dtcfgex( "getgammavalue", "green" );
	gamma_orig_[2] = gamma_[2] = get_nv_dtcfgex( "getgammavalue", "blue" );
	// Sharpness
	image_sharpening_orig_ = image_sharpening_ = get_nv_dtcfgex( "get_normalize_sharpnessvalue", "" );
}

void	c_nvidia_cpl_screen::update()
{
	if( !_b_valid )
		return;
	//// Digital vibrance
	//if( digital_vibrance_ui != digital_vibrance )
	//	{
	//	set_nv_dtcfgex( "setdvc", "", digital_vibrance_ui );
	//	digital_vibrance = digital_vibrance_ui;
	//	}
	// Brightness
	if( brightness_ui_[0] != brightness_[0] )
	{
		set_nv_dtcfgex( "set_normalize_brightness", "red", brightness_ui_[0] );
		brightness_[0] = brightness_ui_[0];
	}
	if( brightness_ui_[1] != brightness_[1] )
	{
		set_nv_dtcfgex( "set_normalize_brightness", "green", brightness_ui_[1] );
		brightness_[1] = brightness_ui_[1];
	}
	if( brightness_ui_[2] != brightness_[2] )
	{
	 	set_nv_dtcfgex( "set_normalize_brightness", "blue", brightness_ui_[2] );
		brightness_[2] = brightness_ui_[2];
	}
	// Contrast
	if( contrast_ui_[0] != contrast_[0] )
	{
		set_nv_dtcfgex( "set_normalize_contrast", "red", contrast_ui_[0] );
		contrast_[0] = contrast_ui_[0];
	}
	if( contrast_ui_[1] != contrast_[1] )
	{
		set_nv_dtcfgex( "set_normalize_contrast", "green", contrast_ui_[1] );
		contrast_[1] = contrast_ui_[1];
	}
	if( contrast_ui_[2] != contrast_[2] )
	{
		set_nv_dtcfgex( "set_normalize_contrast", "blue", contrast_ui_[2] );
		contrast_[2] = contrast_ui_[2];
	}
	// Gamma
	if( gamma_ui_[0] != gamma_[0] )
	{
		set_nv_dtcfgex( "setgamma", "red", gamma_ui_[0] );
		gamma_[0] = gamma_ui_[0];
	}
	if( gamma_ui_[1] != gamma_[1] )
	{
		set_nv_dtcfgex( "setgamma", "green", gamma_ui_[1] );
		gamma_[1] = gamma_ui_[1];
	}
	if( gamma_ui_[2] != gamma_[2] )
	{
		set_nv_dtcfgex( "setgamma", "blue", gamma_ui_[2] );
		gamma_[2] = gamma_ui_[2];
	}
	// Sharpness
	if( image_sharpening_ui_ != image_sharpening_ )
	{
		set_nv_dtcfgex( "set_normalize_sharpness", "", image_sharpening_ui_ );
		image_sharpening_ = image_sharpening_ui_;
	}
}

FACTORY_CREATE_V1( c_nvidia_cpl, nvidia_cpl, Nvidia Stuff, nvidia_cpl );

namespace n_nvidia_cpl
{
	CONSTEXPR INT32 BASE_PARAM_NB			= 10;
//	CONSTEXPR INT32 NVIEW_PARAM_NB			= 1;
	CONSTEXPR INT32 PERFORMANCE_PARAM_NB	= 2;
	CONSTEXPR INT32 SLI_PARAM_NB			= 4;
	CONSTEXPR INT32 GROUP_NB				= 2;
	CONSTEXPR INT32 PARAM_NB_MAX	=	BASE_PARAM_NB
//									+	NVIEW_PARAM_NB
									+	PERFORMANCE_PARAM_NB
									+	SLI_PARAM_NB
									+	GROUP_NB;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_BOOL_OFF(				active )
		PARAM_DEF_STR_LOCKED(			Processor )
		PARAM_DEF_STR_LOCKED(			Bus )
		PARAM_DEF_STR_LOCKED(			Memory )
		PARAM_DEF_SYMBO_LOCKED_PSTR(		Connector,	str_nv_connector_layout	)
		PARAM_DEF_STR_LOCKED(			ForceWare )
		PARAM_DEF_BOOL_OFF_SAVE_NOT(	VSync )
		PARAM_DEF_SYMBO_LOCKED_PSTR(		nView_DisplayMode,	str_nv_display_mode	)
		PARAM_DEF_GROUP_CLOSED( SLI, SLI_PARAM_NB )
			PARAM_DEF_INT32_LOCKED(		Nb_GPU	)
			PARAM_DEF_INT32_LOCKED(		Nb_GPU_in_SLI )
			PARAM_DEF_BOOL_OFF_SAVE_NOT(	Enabled )
			PARAM_DEF_SYMBO_PSTR_ONE(	SLI_Mode,	str_nv_sli_mode )
		PARAM_DEF_GROUP_CLOSED( Performance&Quality, PERFORMANCE_PARAM_NB )
			PARAM_DEF_SYMBO_LOCKED_PSTR(		Antialiasing,	str_nv_antialiasing	)
			PARAM_DEF_INT32_LOCKED(			frames_buffered_nb	)
		//	PARAM_DEF_INT32_LOCKED(			frames_buffered_nb,			1,	1,		0.,	6	)
		//			{	nullptr,	PARAM_INT32,	"AnisotropicFiltering",		1,	1,		0.,	1,		nullptr, nullptr },
		PARAM_DEF_NONE( Screen_0 )
		PARAM_DEF_NONE( Screen_1 )
	};
}

void	c_nvidia_cpl::param_init_pt()
{
	INT32	h=0;

	param_set_pt( h, get_pt_active() );
	param_set_pt( h, str_nv_processor_ );
	param_set_pt( h, str_nv_bus_ );
	param_set_pt( h, str_nv_videoram_ );
	param_set_pt( h, _s_connector_layout );
	param_set_pt( h, str_nv_forceware_ );
	param_set_pt( h, _b_v_sync_ui );
	param_set_pt( h, _display_mode );
	++h;
		param_set_pt( h, nb_gpu_ );
		param_set_pt( h, nb_gpu_sli_ );
		param_set_pt( h, _b_sli_on );
		param_set_pt( h, _s_sli_mode );
	++h;
		param_set_pt( h, _s_antialiasing );
		param_set_pt( h, _frames_buffered_nb );
//		param_set_pt( h, anisotropic_filtering_ui );

	param_attach_obj( h, screen_00_ );
	param_attach_obj( h, screen_01_ );

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE(c_nvidia_cpl)
	,_s_connector_layout	(NV_CONNECTORLAYOUT_UNKNOWN)
	,_s_sli_mode			(NV_SLI_UNKNOWN)
	,_s_antialiasing		(NV_AA_METHOD_UNKNOWN)
	,_display_mode			(NV_DISPLAY_MODE_NONE)
{
	screen_00_ = nullptr;
	screen_01_ = nullptr;
	_b_valid = false;

	param_init_with( n_nvidia_cpl::param, n_nvidia_cpl::PARAM_NB_MAX );

	init();
}

void	c_nvidia_cpl::set_nv_dtcfgex( C_PCHAR_C command )
{
	CHAR set[1024];

	sprintf( set, "%s", command );
	//	sprintf( set, "%s %d %s %f", command, screen_index, color, value );
	pfNVdtcfgex( set );
}

REAL	c_nvidia_cpl::get_nv_dtcfgex( C_PCHAR_C command, C_PCHAR_C color )
{
	CHAR get[1024];

	sprintf( get, "%s %s", command, color );
	//	sprintf( get, "%s %d %s", command, screen_index, color );
	pfNVdtcfgex( get );
	return REAL( atof(get) );
}

void	c_nvidia_cpl::update()
{
	if( !_b_valid || !is_active() )
		return;

	if( screen_00_ )
		screen_00_->update();
	if( screen_01_ )
		screen_01_->update();
	//if( s_antialiasing_ui != s_antialiasing )
	//{
	//	if( pfNvCplSetDataInt( NVCPL_API_CURRENT_AA_VALUE, s_antialiasing_ui ) )
	//		s_antialiasing = s_antialiasing_ui;
	//	else
	//		ERR_PRINT_STRING( "Nvidia_cpl : unable to set antialiasing setting" );
	//}
	//if( nb_frames_buffered_ui != nb_frames_buffered )
	//{
	//	if( pfNvCplSetDataInt( NVCPL_API_FRAME_QUEUE_LIMIT, nb_frames_buffered_ui ) )
	//		nb_frames_buffered = nb_frames_buffered_ui;
	//	else
	//		ERR_PRINT_STRING( "Nvidia_cpl : unable to set nb frame prerendered" );
	
	//}
	//if( anisotropic_filtering_ui != anisotropic_filtering )
	//{
	//	if( pfNvCplSetDataInt( NVCPL_API_FRAME_QUEUE_LIMIT, anisotropic_filtering_ui ) )
	//		anisotropic_filtering = anisotropic_filtering_ui;
	//	else
	//		ERR_PRINT_STRING( "Nvidia_cpl : unable to set anisotropic filtering" );
	//}
	if(	_b_v_sync_ui != _b_v_sync )
	{
		if( _b_v_sync_ui )
			set_nv_dtcfgex( "setvsync on" );
		else
			set_nv_dtcfgex( "setvsync off" );
		_b_v_sync = _b_v_sync_ui;
	}
}

bool	c_nvidia_cpl::init_info()
{
	NVDISPLAYINFO displayInfo;
	// displayInfo.cbSize must be set to size of structure
	// displayInfo.dwInputFields1 must be set before call to indicate which fields to retrieve
	// displayInfo.dwOutputFields1 will be set on return to indicate which fields were successfully retrived
	// see NVDISPLAYINFO1_* bit definitions for field information, use 0xffffffff to retrieve all fields
	memset( &displayInfo, 0, sizeof( displayInfo ) );
	displayInfo.cbSize = sizeof( displayInfo );
	displayInfo.dwInputFields1 = NVDISPLAYINFO1_ALL;
	displayInfo.dwInputFields2 = NVDISPLAYINFO2_ALL;
	if( !pfNvGetDisplayInfo( "0", &displayInfo ) )
		NV_CPL_PRINT_STRING( "Unable to retrieve display info." );
	else
	{
		str_nv_processor_.set( displayInfo.szAdapterName );
		NV_CPL_PRINT_STRING( "Display Adapter : %s", displayInfo.szAdapterName );
		switch( displayInfo.nDisplayMode )
		{
			case NVDISPLAYMODE_NONE	:		_display_mode = NV_DISPLAY_MODE_NONE;		break;
			case NVDISPLAYMODE_STANDARD	:	_display_mode = NV_DISPLAY_MODE_STANDARD;	break;
			case NVDISPLAYMODE_CLONE	:	_display_mode = NV_DISPLAY_MODE_CLONE;		break;
			case NVDISPLAYMODE_HSPAN	:	_display_mode = NV_DISPLAY_MODE_HSPAN;		break;
			case NVDISPLAYMODE_VSPAN	:	_display_mode = NV_DISPLAY_MODE_VSPAN;		break;
			case NVDISPLAYMODE_DUALVIEW	:	_display_mode = NV_DISPLAY_MODE_DUALVIEW;	break;
			default :						_display_mode = NV_DISPLAY_MODE_DUALVIEW;	break;
		}
		if( ( displayInfo.dwOutputFields1 & NVDISPLAYINFO1_BOARDTYPE ) != 0 ) // not supported by all drivers
		{
			switch( displayInfo.dwBoardType )
			{
				case NVBOARDTYPE_GEFORCE:	NV_CPL_PRINT_STRING( "Display Board : GeForce" );			break;
				case NVBOARDTYPE_QUADRO:	NV_CPL_PRINT_STRING( "Display Board : Quadro" );			break;
				case NVBOARDTYPE_NVS:		NV_CPL_PRINT_STRING( "Display Board : NVS" );				break;
				default:	NV_CPL_PRINT_STRING( "Display Board : 0x%08lX", displayInfo.dwBoardType );	break;
			}
		}
		str_nv_forceware_.set( displayInfo.szDriverVersion );
		if( !str_nv_forceware_.is_empty() )
			NV_CPL_PRINT_STRING( "Display Driver : %s", displayInfo.szDriverVersion );
		else
		{
			ERR_PRINT_STRING( "%s() can't Display driver info desactivating Nvidia cpl to avois stop on vista", __FUNCTION__ );
			return false;	//if we go on we crash on vista nvidia 169.25
		}
	}
	return true;
}

void c_nvidia_cpl::init()
{
	// Load NVCPL library
	_hd_lib = ::LoadLibraryW( L"NVCPL.dll" );
	if( _hd_lib == 0 )
	{
		NV_CPL_PRINT_STRING( "Unable to load NVCPL.dll.");
		goto exit;
	}

	// MMW: It's possible to load nvcpl even when current the graphics 
	// card is not Nvidia: therefore also need to check for Nvidia 
	// card to make sure the results of these queries reflect reality.
	pfNvGetDisplayInfo = (fNvGetDisplayInfo)::GetProcAddress( _hd_lib, "NvGetDisplayInfo");
	if( !pfNvGetDisplayInfo )
	{
		NV_CPL_PRINT_STRING( "Unable to get a pointer to NvGetDisplayInfo" );
		goto exit;
	}
	// Get the NvCplGetDataInt function pointer from the library
	pfNvCplGetDataInt = (NvCplGetDataIntType)::GetProcAddress( _hd_lib, "NvCplGetDataInt" );
	if( pfNvCplGetDataInt == 0 )
	{
		NV_CPL_PRINT_STRING( "Unable to get a pointer to NvCplGetDataInt" );
		goto exit;
	}
	pfNvCplSetDataInt = (NvCplSetDataIntType)::GetProcAddress( _hd_lib, "NvCplSetDataInt" );
	if( pfNvCplSetDataInt == 0 )
	{
		NV_CPL_PRINT_STRING( "Unable to get a pointer to NvCplSetDataInt" );
		goto exit;
	}
	pfNVdtcfgex = (fdtcfgex)::GetProcAddress(_hd_lib, "dtcfgex");
	if( pfNVdtcfgex == 0 )
	{
		NV_CPL_PRINT_STRING( "Unable to get a pointer to NVdtcfgex" );
		goto exit;
	}
	pfNvColorGetGammaRampEx = (fNvColorGetGammaRampEx)::GetProcAddress(_hd_lib, "NvColorGetGammaRampEx");
	if( !pfNvColorGetGammaRampEx )
	{
		NV_CPL_PRINT_STRING( "Unable to get a pointer to NvColorGetGammaRampEx.");
		goto exit;
	}
	pfGetdtcfgLastErrorEx = (fGetdtcfgLastErrorEx)::GetProcAddress( _hd_lib, "GetdtcfgLastErrorEx" );
	if( !pfGetdtcfgLastErrorEx )
	{
		NV_CPL_PRINT_STRING( "Unable to get a pointer to GetdtcfgLastErrorEx.");
		goto exit;
	}
	pfNvCplGetRealConnectedDevicesString = (fNvCplGetRealConnectedDevicesString)::GetProcAddress( _hd_lib, "NvCplGetRealConnectedDevicesString" );
	if( !pfNvCplGetRealConnectedDevicesString )
	{
		NV_CPL_PRINT_STRING( "Unable to get a pointer to NvCplGetRealConnectedDevicesString.");
		goto exit;
	}
	pfNvGetPhysicalConnectorInfo = (fNvGetPhysicalConnectorInfo)::GetProcAddress( _hd_lib, "NvGetPhysicalConnectorInfo" );
	if( !pfNvGetPhysicalConnectorInfo )
	{
		NV_CPL_PRINT_STRING( "Unable to get a pointer to NvGetPhysicalConnectorInfo.");
		goto exit;
	}
	pfNvEnumPhysicalConnectorDetails = (fNvEnumPhysicalConnectorDetails)::GetProcAddress( _hd_lib, "NvEnumPhysicalConnectorDetails" );
	if( !pfNvEnumPhysicalConnectorDetails )
	{
		NV_CPL_PRINT_STRING( "Unable to get a pointer to NvEnumPhysicalConnectorDetails.");
		goto exit;
	}

	_b_valid = true;	//why here and next
	if( !init_info() )
		goto exit;
	gpu_config();

	// Reset any delay
	//pfNVdtcfgex( (PCHAR)"setdelay pre 0" );
	//pfNVdtcfgex( (PCHAR)"setdelay post 0" );

	obj_get( screen_00_ );
	screen_00_->set_index(1);

	obj_get( screen_01_ );
	screen_01_->set_index(2);
	return;
exit:
	_b_valid = false;
}

void	c_nvidia_cpl::close()
{
	// Free NVCPL library
	if( _hd_lib )
	{
		obj_delete(  screen_00_ );
		obj_delete(  screen_01_ );
		::FreeLibrary( _hd_lib );
	}
}

c_nvidia_cpl::~c_nvidia_cpl()
{
	close();
}

void	c_nvidia_cpl::gpu_config()
{
	CHAR	str[128];
//	NV_CPL_PRINT_STRING( "GPU CONFIGURATION" );
	// Get bus mode
	long busMode;
	if( !pfNvCplGetDataInt( NVCPL_API_AGP_BUS_MODE, &busMode ) )
		NV_CPL_PRINT_STRING( "Unable to retrieve Bus mode" );
	else
	{
		// Get bus transfer rate
		long busTransferRate;
		if( !pfNvCplGetDataInt( NVCPL_API_TX_RATE, &busTransferRate ) )
			NV_CPL_PRINT_STRING( "Unable to retrieve Bus transfer rate" );
		else
		{
			// Graphics card connection to system
			//  Values are:
			//   1 = PCI
			//   4 = AGP
			//   8 = PCI Express
			switch( busMode )
			{
			case 1:		sprintf( str, "PCI %dX", busTransferRate );			break;
			case 4:		sprintf( str, "AGP %dX", busTransferRate );			break;
			case 8:		sprintf( str, "PCI Express %dX", busTransferRate );	break;
			default:	sprintf( str, "Unknown %dX", busTransferRate );		break;
			}
			str_nv_bus_.set( str );
			NV_CPL_PRINT_STRING( "Bus Mode : %s", str_nv_bus_.get() );
		}
		// Get bus transfer rate
		// Get AGP GART size
		if( busMode == 4 )
		{
			long AGPGARTSize;
			if( !pfNvCplGetDataInt( NVCPL_API_AGP_LIMIT, &AGPGARTSize ) )
				NV_CPL_PRINT_STRING( "Unable to retrieve AGP GART/Memory size" );
			else
			{
				// AGP GART Size is reported in Bytes: convert to MB before printing.
				NV_CPL_PRINT_STRING( "AGP GART/Memory Size : %d MBytes", AGPGARTSize/(1024*1024));
			}
		}
	}
	// Get video memory size
	long videoRAMSize;
	if( !pfNvCplGetDataInt( NVCPL_API_VIDEO_RAM_SIZE, &videoRAMSize ) )
	{
		str_nv_videoram_.set( "Unknown" );
		NV_CPL_PRINT_STRING( "Unable to retrieve Video memory size" );
	}
	else
	{
		sprintf( str, "%d MB", videoRAMSize );
		str_nv_videoram_.set( str );
		NV_CPL_PRINT_STRING( "Video Memory size : %d MBytes", videoRAMSize);
	}

	// Get antialiasing mode
	long AAValue;
	if( !pfNvCplGetDataInt( NVCPL_API_CURRENT_AA_VALUE, &AAValue ) )
		NV_CPL_PRINT_STRING( "Unable to retrieve Antialiasing setting" );
	else
	{
		C_PCHAR	str_2;
		switch( AAValue )
		{
		case NVCPL_API_AA_METHOD_OFF:			_s_antialiasing_orig = NV_AA_METHOD_OFF;			str_2 = "Off";			break;
		case NVCPL_API_AA_METHOD_2X:			_s_antialiasing_orig = NV_AA_METHOD_2X;				str_2 = "2X";			break;
		case NVCPL_API_AA_METHOD_2XQ:			_s_antialiasing_orig = NV_AA_METHOD_2XQ;			str_2 = "2XQ";			break;
		case NVCPL_API_AA_METHOD_4X:			_s_antialiasing_orig = NV_AA_METHOD_4X;				str_2 = "4X";			break;
		case NVCPL_API_AA_METHOD_4X_GAUSSIAN:	_s_antialiasing_orig = NV_AA_METHOD_4X_GAUSSIAN;	str_2 = "4X 9Tap";		break;
		case NVCPL_API_AA_METHOD_4XS:			_s_antialiasing_orig = NV_AA_METHOD_4XS;			str_2 = "4X Skewed";	break;
		case NVCPL_API_AA_METHOD_6XS:			_s_antialiasing_orig = NV_AA_METHOD_6XS;			str_2 = "6XS";			break;
		case NVCPL_API_AA_METHOD_8XS:			_s_antialiasing_orig = NV_AA_METHOD_8XS;			str_2 = "8XS";			break;
		case NVCPL_API_AA_METHOD_16XS:			_s_antialiasing_orig = NV_AA_METHOD_16XS;			str_2 = "16XS";			break;
		default:								_s_antialiasing_orig = NV_AA_METHOD_UNKNOWN;		str_2 = "Unknown";		break;
		}
		_s_antialiasing = _s_antialiasing_orig ;
		NV_CPL_PRINT_STRING( "Antialiasing setting : %s", str_2 );
	}

	// Get and Modify Number of Frames Buffered
		
	long numFramesBuffered;
	if( !pfNvCplGetDataInt( NVCPL_API_FRAME_QUEUE_LIMIT, &numFramesBuffered ) )
	{
		_frames_buffered_nb = -1;
		NV_CPL_PRINT_STRING( "Unable to retrieve Frame Queue");
	}
	else
	{
		_frames_buffered_nb = numFramesBuffered;
		NV_CPL_PRINT_STRING( "Number of frames buffered : %d", _frames_buffered_nb );
	}

		//	long CONST kForceBufferedFrames = 1;
		//	printf("- Setting Number of Frames Buffered to %d: ", kForceBufferedFrames);
		//	if( !pfNvCplSetDataInt(NVCPL_API_FRAME_QUEUE_LIMIT, kForceBufferedFrames) )
		//		printf("Unable to set\n");
		//	else
		//	{
		//		long    readbackValue;
		//		if( pfNvCplGetDataInt(NVCPL_API_FRAME_QUEUE_LIMIT, &readbackValue) )
		//			printf("Unable to retrieve\n");
		//		else
		//			printf("%d Frame(s)\n", readbackValue);
		//	}
		//	printf("- Resetting Number of Frames Buffered to %d: ", numFramesBuffered);
		//	if( !pfNvCplSetDataInt(NVCPL_API_FRAME_QUEUE_LIMIT, numFramesBuffered) )
		//		printf("Unable to set\n");
		//	else
		//	{
		//		long    readbackValue;
		//		if( !pfNvCplGetDataInt(NVCPL_API_FRAME_QUEUE_LIMIT, &readbackValue) )
		//			printf("Unable to retrieve\n");
		//		else
		//			printf("%d Frame(s)\n", readbackValue);
		//	}
		//}
	// Get number of GPUs and number of SLI GPUs
	long    numGPUs     = 0L;
	long    numSLIGPUs  = 0L;
	if( !pfNvCplGetDataInt( NVCPL_API_NUMBER_OF_GPUS, &numGPUs ) )
		NV_CPL_PRINT_STRING( "Unable to retrieve number of GPUs");
	else
	{
		nb_gpu_ = numGPUs;
		NV_CPL_PRINT_STRING(  "Number of GPUs : %ld.", numGPUs);
	}
	if( !pfNvCplGetDataInt( NVCPL_API_NUMBER_OF_SLI_GPUS, &numSLIGPUs ) )
		NV_CPL_PRINT_STRING( "Unable to retrieve number of GPU in SLI mode" );
	else
	{
		NV_CPL_PRINT_STRING( "Number of GPUs in SLI mode : %ld.", numSLIGPUs);
		nb_gpu_sli_ = numSLIGPUs;
	}
	if( numSLIGPUs > 0L )
	{
		long    SLIMode = 0L;
		if( !pfNvCplGetDataInt( NVCPL_API_SLI_MULTI_GPU_RENDERING_MODE, &SLIMode ) )
			NV_CPL_PRINT_STRING( "Unable to retrieve SLI rendering mode" );
		else
		{
			_b_sli_on = false;
			if ((SLIMode & NVCPL_API_SLI_ENABLED) == 0L)
				NV_CPL_PRINT_STRING( "SLI rendering mode : SLI is not enabled." );
			else 
			{
				_b_sli_on = true;
				if( ( SLIMode & NVCPL_API_SLI_RENDERING_MODE_AFR ) != 0L )
				{
					NV_CPL_PRINT_STRING( "SLI rendering mode : SLI is in AFR mode." );
					_s_sli_mode = NV_SLI_MODE_AFR;
				}
				else if( ( SLIMode & NVCPL_API_SLI_RENDERING_MODE_SFR ) != 0L )
				{
					NV_CPL_PRINT_STRING( "SLI rendering mode : SLI is in SFR mode." );
					_s_sli_mode = NV_SLI_MODE_SFR;
				}
				else if( ( SLIMode & NVCPL_API_SLI_RENDERING_MODE_SINGLE_GPU ) != 0L )
				{
					NV_CPL_PRINT_STRING( "SLI rendering mode : SLI is in single GPU mode." );
					_s_sli_mode = NV_SLI_MODE_SINGLE_GPU;
				}
				else if( ( SLIMode & NVCPL_API_SLI_RENDERING_MODE_AUTOSELECT ) == 0L )
				{
					NV_CPL_PRINT_STRING( "SLI rendering mode : SLI is in auto-select mode." );
					_s_sli_mode = NV_SLI_MODE_AUTO_SELECT;
				}
				else
				{
					NV_CPL_PRINT_STRING( "SLI rendering mode : unknown SLI mode." );
					_s_sli_mode = NV_SLI_UNKNOWN;
				}
			}
		}
	}

	// Get Connector Details
	NVCONNECTORINFO	connector_info = {0};
	memset( &connector_info, 0, sizeof( connector_info ) );
	connector_info.cbSize = sizeof( connector_info );
	NVRESULT res = pfNvGetPhysicalConnectorInfo( 1, &connector_info );
	if( res == NV_NOTSUPPORTED )
		NV_CPL_PRINT_STRING( "Unable to retrieve physical connector info : Unsupported feature (requires compatible video BIOS)" );
	else if( res != NV_OK )
		NV_CPL_PRINT_STRING( "Unable to retrieve physical connector info" );
	else
	{
		switch( connector_info.nConnectorLayout )
		{
			case NVCONNECTORLAYOUT_CARD_SINGLESLOT:		_s_connector_layout = NV_CONNECTORLAYOUT_CARD_SINGLESLOT;	break;
			case NVCONNECTORLAYOUT_CARD_DOUBLESLOT:		_s_connector_layout = NV_CONNECTORLAYOUT_CARD_DOUBLESLOT;	break;
			case NVCONNECTORLAYOUT_CARD_MOBILE_MXM:		_s_connector_layout = NV_CONNECTORLAYOUT_CARD_MOBILE_MXM;	break;
			case NVCONNECTORLAYOUT_CARD_MOBILE_OEM:		_s_connector_layout = NV_CONNECTORLAYOUT_CARD_MOBILE_OEM;	break;
			case NVCONNECTORLAYOUT_MOBILE_BACK:			_s_connector_layout = NV_CONNECTORLAYOUT_MOBILE_BACK;		break;
			case NVCONNECTORLAYOUT_MOBILE_BACK_LEFT:	_s_connector_layout = NV_CONNECTORLAYOUT_MOBILE_BACK_LEFT;	break;
			case NVCONNECTORLAYOUT_MOBILE_BACK_DOCK:	_s_connector_layout = NV_CONNECTORLAYOUT_MOBILE_BACK_DOCK;	break;
			case NVCONNECTORLAYOUT_NFORCE_STANDARD:		_s_connector_layout = NV_CONNECTORLAYOUT_NFORCE_STANDARD;	break;
			default :									_s_connector_layout = NV_CONNECTORLAYOUT_UNKNOWN;			break;
		}
		NVCONNECTORDETAIL	connector_detail = {0};
		memset( &connector_detail, 0, sizeof( connector_detail ) );
		connector_detail.cbSize = sizeof( connector_detail );
		for( UINT32 i = 0; i < connector_info.dwConnectorCount; ++i )
		{
			if( pfNvEnumPhysicalConnectorDetails( 1, i, &connector_detail ) == NV_OK )
			{
				NV_CPL_PRINT_STRING( "Physical Connector %d : %s", connector_detail.dwConnectorLocation, str_nv_connector_type[connector_detail.nConnectorType] );
			}
		}
	}

}

c_nvidia_cpl*	g_nvidia_cpl = nullptr;
