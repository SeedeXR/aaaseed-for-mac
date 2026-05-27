
#include "spaceball_glue.h"
#if AAA_TRACKER_SPACEBALL()
//#include "err.h"
#include "aaaseed.h"
#include "spaceball.h"
#include "infrastructure/aaa_window.h"

#include "Tracker/3DxWare/spwmacro.h"	//	Common macros used by SpaceWare functions.
#include "Tracker/3DxWare/si.h"			//	Required for any SpaceWare support within an app.
#include "Tracker/3DxWare/siapp.h"		//	Required for siapp.lib symbols

#include "Tracker/3DxWare/virtualkeys.hpp"


#include <lib_use.h>
AAA_LIB_USE( "tracker/3DxWare/siapp" )


SiHdl        devHdl {nullptr};      /* Handle to 3D Mouse Device */

void sb_deinit()
{
	SiTerminate();	//	called to shut down the SpaceWare input library
}


/*----------------------------------------------------------------------
* Function: SbMotionEvent()
*
* Description:
*    This function receives motion information and prints out the info 
*    on screen.
*    
*Fset_target

* Args:
*    SiSpwEvent *pEvent   Containts Data from a 3D mouse Event
*
* Return Value:
*    NONE
*
*----------------------------------------------------------------------*/
void sb_motion_event( SiSpwEvent *pEvent )
{	
	SiSpwData* st = &pEvent->u.spwData;
//	DBG_PRINT_STRING( " %d %d %d", st->mData[SI_TX], st->mData[SI_TY], st->mData[SI_TZ] );
	spaceball->set_tra_in( REAL( st->mData[SI_TX] ) / REAL(5000), REAL( st->mData[SI_TY] ) / REAL(5000), REAL( st->mData[SI_TZ] ) / REAL(5000) );
	spaceball->set_rot_in( REAL( st->mData[SI_RX] ) / REAL(5000), REAL( st->mData[SI_RY] ) / REAL(5000), REAL( st->mData[SI_RZ] ) / REAL(5000) );

/* put the actual ball data into the buffers
  len6= _stprintf_s( buff6, 30, _T(" P: %d         "), pEvent->u.spwData.period);
*/
}


/*----------------------------------------------------------------------
* Function: SbZeroEvent()
*
* Description:
*    This function clears the previous data, no motion data was received
*    
*    
*
* Args:
*    NONE
*
* Return Value:
*    NONE
*
*----------------------------------------------------------------------*/
void sb_zero_event()
{
	spaceball->set_tra_in( zero_v4fp32 );
	spaceball->set_rot_in( zero_v4fp32 );
}

static bool b_init = false;
bool sb_init()
{
	//	init the SpaceWare input library
	if( SiInitialize() == SPW_DLL_LOAD_ERROR )  
		ERR_PRINT_STRING( "Could not load SiAppDll dll files" );
	else
		b_init = true;
	return b_init;
}

bool sb_open()
{
	if( !b_init )
		return false;

	SiOpenData oData;	//	OS Independent data to open ball
	SiOpenWinInit( &oData, get_window_main_handle() );   // init Win. platform specific data

	//	open data, which will check for device type and return the device handle
	//		to be used by this function
	if( !(devHdl = SiOpen( aaa::AAASEED_WINDOW_NAME, SI_ANY_DEVICE, SI_NO_MASK, SI_EVENT, &oData )) ) 
	{
		sb_deinit();
		return false; //	could not open device
	}
	else
	{
		SiDeviceName devName;
		SiGetDeviceName( devHdl, &devName );
		GOOD_PRINT_STRING( "Opened : %256s", devName.name );
		SiGrabDevice( devHdl, SPW_TRUE );
		return true;	//	opened device successfully
	}
}


C_PCHAR_C V3DKeyToName( V3DKey v3dk )
{
	C_PCHAR str = nullptr;

	switch (v3dk) 
	{
	case s3dm::V3DK_MENU		: str = "V3DK_MENU"; break;
	case s3dm::V3DK_FIT			: str = "V3DK_FIT"; break;
	case s3dm::V3DK_TOP			: str = "V3DK_TOP"; break;
	case s3dm::V3DK_LEFT		: str = "V3DK_LEFT"; break;
	case s3dm::V3DK_RIGHT		: str = "V3DK_RIGHT"; break;
	case s3dm::V3DK_FRONT		: str = "V3DK_FRONT"; break;
	case s3dm::V3DK_BOTTOM		: str = "V3DK_BOTTOM"; break;
	case s3dm::V3DK_BACK		: str = "V3DK_BACK"; break;
	case s3dm::V3DK_ROLL_CW		: str = "V3DK_ROLL_CW"; break;
	case s3dm::V3DK_ROLL_CCW	: str = "V3DK_ROLL_CCW"; break;
	case s3dm::V3DK_ISO1		: str = "V3DK_ISO1"; break;
	case s3dm::V3DK_ISO2		: str = "V3DK_ISO2"; break;
	case s3dm::V3DK_1			: str = "V3DK_1"; break;
	case s3dm::V3DK_2			: str = "V3DK_2"; break;
	case s3dm::V3DK_3			: str = "V3DK_3"; break;
	case s3dm::V3DK_4			: str = "V3DK_4"; break;
	case s3dm::V3DK_5			: str = "V3DK_5"; break;
	case s3dm::V3DK_6			: str = "V3DK_6"; break;
	case s3dm::V3DK_7			: str = "V3DK_7"; break;
	case s3dm::V3DK_8			: str = "V3DK_8"; break;
	case s3dm::V3DK_9			: str = "V3DK_9"; break;
	case s3dm::V3DK_10			: str = "V3DK_10"; break;
	case s3dm::V3DK_ESC			: str = "V3DK_ESC"; break;
	case s3dm::V3DK_ALT			: str = "V3DK_ALT"; break;
	case s3dm::V3DK_SHIFT		: str = "V3DK_SHIFT"; break;
	case s3dm::V3DK_CTRL		: str = "V3DK_CTRL"; break;
	case s3dm::V3DK_ROTATE		: str = "V3DK_ROTATE"; break;
	case s3dm::V3DK_PANZOOM		: str = "V3DK_PANZOOM"; break;
	case s3dm::V3DK_DOMINANT	: str = "V3DK_DOMINANT"; break;
	case s3dm::V3DK_PLUS		: str = "V3DK_PLUS"; break;
	case s3dm::V3DK_MINUS		: str = "V3DK_MINUS"; break;
	case s3dm::V3DK_SPIN_CW		: str = "V3DK_SPIN_CW"; break;
	case s3dm::V3DK_SPIN_CCW	: str = "V3DK_SPIN_CCW"; break;
	case s3dm::V3DK_TILT_CW		: str = "V3DK_TILT_CW"; break;
	case s3dm::V3DK_TILT_CCW	: str = "V3DK_TILT_CCW"; break;
	case V3DK_ENTER				: str = "V3DK_ENTER"; break;
	case V3DK_DELETE			: str = "V3DK_DELETE"; break;
	case V3DK_RESERVED0			: str = "V3DK_RESERVED0"; break;
	case V3DK_RESERVED1			: str = "V3DK_RESERVED1"; break;
	case V3DK_RESERVED2			: str = "V3DK_RESERVED2"; break;
	case V3DK_F1				: str = "V3DK_F1"; break;
	case V3DK_F2				: str = "V3DK_F2"; break;
	case V3DK_F3				: str = "V3DK_F3"; break;
	case V3DK_F4				: str = "V3DK_F4"; break;
	case V3DK_F5				: str = "V3DK_F5"; break;
	case V3DK_F6				: str = "V3DK_F6"; break;
	case V3DK_F7				: str = "V3DK_F7"; break;
	case V3DK_F8				: str = "V3DK_F8"; break;
	case V3DK_F9				: str = "V3DK_F9"; break;
	case V3DK_F10				: str = "V3DK_F10"; break;
	case V3DK_F11				: str = "V3DK_F11"; break;
	case V3DK_F12				: str = "V3DK_F12"; break;
	case V3DK_F13				: str = "V3DK_F13"; break;
	case V3DK_F14				: str = "V3DK_F14"; break;
	case V3DK_F15				: str = "V3DK_F15"; break;
	case V3DK_F16				: str = "V3DK_F16"; break;
	case V3DK_F17				: str = "V3DK_F17"; break;
	case V3DK_F18				: str = "V3DK_F18"; break;
	case V3DK_F19				: str = "V3DK_F19"; break;
	case V3DK_F20				: str = "V3DK_F20"; break;
	case V3DK_F21				: str = "V3DK_F21"; break;
	case V3DK_F22				: str = "V3DK_F22"; break;
	case V3DK_F23				: str = "V3DK_F23"; break;
	case V3DK_F24				: str = "V3DK_F24"; break;
	case V3DK_F25				: str = "V3DK_F25"; break;
	case V3DK_F26				: str = "V3DK_F26"; break;
	case V3DK_F27				: str = "V3DK_F27"; break;
	case V3DK_F28				: str = "V3DK_F28"; break;
	case V3DK_F29				: str = "V3DK_F29"; break;
	case V3DK_F30				: str = "V3DK_F30"; break;
	case V3DK_F31				: str = "V3DK_F31"; break;
	case V3DK_F32				: str = "V3DK_F32"; break;
	case V3DK_F33				: str = "V3DK_F33"; break;
	case V3DK_F34				: str = "V3DK_F34"; break;
	case V3DK_F35				: str = "V3DK_F35"; break;
	case V3DK_F36				: str = "V3DK_F36"; break;
	case V3DK_11				: str = "V3DK_11"; break;
	case V3DK_12				: str = "V3DK_12"; break;
	case V3DK_13				: str = "V3DK_13"; break;
	case V3DK_14				: str = "V3DK_14"; break;
	case V3DK_15				: str = "V3DK_15"; break;
	case V3DK_16				: str = "V3DK_16"; break;
	case V3DK_17				: str = "V3DK_17"; break;
	case V3DK_18				: str = "V3DK_18"; break;
	case V3DK_19				: str = "V3DK_19"; break;
	case V3DK_20				: str = "V3DK_20"; break;
	case V3DK_21				: str = "V3DK_21"; break;
	case V3DK_22				: str = "V3DK_22"; break;
	case V3DK_23				: str = "V3DK_23"; break;
	case V3DK_24				: str = "V3DK_24"; break;
	case V3DK_25				: str = "V3DK_25"; break;
	case V3DK_26				: str = "V3DK_26"; break;
	case V3DK_27				: str = "V3DK_27"; break;
	case V3DK_28				: str = "V3DK_28"; break;
	case V3DK_29				: str = "V3DK_29"; break;
	case V3DK_30				: str = "V3DK_30"; break;
	case V3DK_31				: str = "V3DK_31"; break;
	case V3DK_32				: str = "V3DK_32"; break;
	case V3DK_33				: str = "V3DK_33"; break;
	case V3DK_34				: str = "V3DK_34"; break;
	case V3DK_35				: str = "V3DK_35"; break;
	case V3DK_36				: str = "V3DK_36"; break;
	case V3DK_VIEW_1			: str = "V3DK_VIEW_1"; break;
	case V3DK_VIEW_2			: str = "V3DK_VIEW_2"; break;
	case V3DK_VIEW_3			: str = "V3DK_VIEW_3"; break;
	case V3DK_VIEW_4			: str = "V3DK_VIEW_4"; break;
	case V3DK_VIEW_5			: str = "V3DK_VIEW_5"; break;
	case V3DK_VIEW_6			: str = "V3DK_VIEW_6"; break;
	case V3DK_VIEW_7			: str = "V3DK_VIEW_7"; break;
	case V3DK_VIEW_8			: str = "V3DK_VIEW_8"; break;
	case V3DK_VIEW_9			: str = "V3DK_VIEW_9"; break;
	case V3DK_VIEW_10			: str = "V3DK_VIEW_10"; break;
	case V3DK_VIEW_11			: str = "V3DK_VIEW_11"; break;
	case V3DK_VIEW_12			: str = "V3DK_VIEW_12"; break;
	case V3DK_VIEW_13			: str = "V3DK_VIEW_13"; break;
	case V3DK_VIEW_14			: str = "V3DK_VIEW_14"; break;
	case V3DK_VIEW_15			: str = "V3DK_VIEW_15"; break;
	case V3DK_VIEW_16			: str = "V3DK_VIEW_16"; break;
	case V3DK_VIEW_17			: str = "V3DK_VIEW_17"; break;
	case V3DK_VIEW_18			: str = "V3DK_VIEW_18"; break;
	case V3DK_VIEW_19			: str = "V3DK_VIEW_19"; break;
	case V3DK_VIEW_20			: str = "V3DK_VIEW_20"; break;
	case V3DK_VIEW_21			: str = "V3DK_VIEW_21"; break;
	case V3DK_VIEW_22			: str = "V3DK_VIEW_22"; break;
	case V3DK_VIEW_23			: str = "V3DK_VIEW_23"; break;
	case V3DK_VIEW_24			: str = "V3DK_VIEW_24"; break;
	case V3DK_VIEW_25			: str = "V3DK_VIEW_25"; break;
	case V3DK_VIEW_26			: str = "V3DK_VIEW_26"; break;
	case V3DK_VIEW_27			: str = "V3DK_VIEW_27"; break;
	case V3DK_VIEW_28			: str = "V3DK_VIEW_28"; break;
	case V3DK_VIEW_29			: str = "V3DK_VIEW_29"; break;
	case V3DK_VIEW_30			: str = "V3DK_VIEW_30"; break;
	case V3DK_VIEW_31			: str = "V3DK_VIEW_31"; break;
	case V3DK_VIEW_32			: str = "V3DK_VIEW_32"; break;
	case V3DK_VIEW_33			: str = "V3DK_VIEW_33"; break;
	case V3DK_VIEW_34			: str = "V3DK_VIEW_34"; break;
	case V3DK_VIEW_35			: str = "V3DK_VIEW_35"; break;
	case V3DK_VIEW_36			: str = "V3DK_VIEW_36"; break;
	case V3DK_SAVE_VIEW_1		: str = "V3DK_SAVE_VIEW_1"; break;
	case V3DK_SAVE_VIEW_2		: str = "V3DK_SAVE_VIEW_2"; break;
	case V3DK_SAVE_VIEW_3		: str = "V3DK_SAVE_VIEW_3"; break;
	case V3DK_SAVE_VIEW_4		: str = "V3DK_SAVE_VIEW_4"; break;
	case V3DK_SAVE_VIEW_5		: str = "V3DK_SAVE_VIEW_5"; break;
	case V3DK_SAVE_VIEW_6		: str = "V3DK_SAVE_VIEW_6"; break;
	case V3DK_SAVE_VIEW_7		: str = "V3DK_SAVE_VIEW_7"; break;
	case V3DK_SAVE_VIEW_8		: str = "V3DK_SAVE_VIEW_8"; break;
	case V3DK_SAVE_VIEW_9		: str = "V3DK_SAVE_VIEW_9"; break;
	case V3DK_SAVE_VIEW_10		: str = "V3DK_SAVE_VIEW_10"; break;
	case V3DK_SAVE_VIEW_11		: str = "V3DK_SAVE_VIEW_11"; break;
	case V3DK_SAVE_VIEW_12		: str = "V3DK_SAVE_VIEW_12"; break;
	case V3DK_SAVE_VIEW_13		: str = "V3DK_SAVE_VIEW_13"; break;
	case V3DK_SAVE_VIEW_14		: str = "V3DK_SAVE_VIEW_14"; break;
	case V3DK_SAVE_VIEW_15		: str = "V3DK_SAVE_VIEW_15"; break;
	case V3DK_SAVE_VIEW_16		: str = "V3DK_SAVE_VIEW_16"; break;
	case V3DK_SAVE_VIEW_17		: str = "V3DK_SAVE_VIEW_17"; break;
	case V3DK_SAVE_VIEW_18		: str = "V3DK_SAVE_VIEW_18"; break;
	case V3DK_SAVE_VIEW_19		: str = "V3DK_SAVE_VIEW_19"; break;
	case V3DK_SAVE_VIEW_20		: str = "V3DK_SAVE_VIEW_20"; break;
	case V3DK_SAVE_VIEW_21		: str = "V3DK_SAVE_VIEW_21"; break;
	case V3DK_SAVE_VIEW_22		: str = "V3DK_SAVE_VIEW_22"; break;
	case V3DK_SAVE_VIEW_23		: str = "V3DK_SAVE_VIEW_23"; break;
	case V3DK_SAVE_VIEW_24		: str = "V3DK_SAVE_VIEW_24"; break;
	case V3DK_SAVE_VIEW_25		: str = "V3DK_SAVE_VIEW_25"; break;
	case V3DK_SAVE_VIEW_26		: str = "V3DK_SAVE_VIEW_26"; break;
	case V3DK_SAVE_VIEW_27		: str = "V3DK_SAVE_VIEW_27"; break;
	case V3DK_SAVE_VIEW_28		: str = "V3DK_SAVE_VIEW_28"; break;
	case V3DK_SAVE_VIEW_29		: str = "V3DK_SAVE_VIEW_29"; break;
	case V3DK_SAVE_VIEW_30		: str = "V3DK_SAVE_VIEW_30"; break;
	case V3DK_SAVE_VIEW_31		: str = "V3DK_SAVE_VIEW_31"; break;
	case V3DK_SAVE_VIEW_32		: str = "V3DK_SAVE_VIEW_32"; break;
	case V3DK_SAVE_VIEW_33		: str = "V3DK_SAVE_VIEW_33"; break;
	case V3DK_SAVE_VIEW_34		: str = "V3DK_SAVE_VIEW_34"; break;
	case V3DK_SAVE_VIEW_35		: str = "V3DK_SAVE_VIEW_35"; break;
	case V3DK_SAVE_VIEW_36		: str = "V3DK_SAVE_VIEW_36"; break;
	case V3DK_TAB				: str = "V3DK_TAB"; break;
	case V3DK_SPACE				: str = "V3DK_SPACE"; break;
	case V3DK_MENU_1			: str = "V3DK_MENU_1"; break;
	case V3DK_MENU_2			: str = "V3DK_MENU_2"; break;
	case V3DK_MENU_3			: str = "V3DK_MENU_3"; break;
	case V3DK_MENU_4			: str = "V3DK_MENU_4"; break;
	case V3DK_MENU_5			: str = "V3DK_MENU_5"; break;
	case V3DK_MENU_6			: str = "V3DK_MENU_6"; break;
	case V3DK_MENU_7			: str = "V3DK_MENU_7"; break;
	case V3DK_MENU_8			: str = "V3DK_MENU_8"; break;
	case V3DK_MENU_9			: str = "V3DK_MENU_9"; break;
	case V3DK_MENU_10			: str = "V3DK_MENU_10"; break;
	case V3DK_MENU_11			: str = "V3DK_MENU_11"; break;
	case V3DK_MENU_12			: str = "V3DK_MENU_12"; break;
	case V3DK_MENU_13			: str = "V3DK_MENU_13"; break;
	case V3DK_MENU_14			: str = "V3DK_MENU_14"; break;
	case V3DK_MENU_15			: str = "V3DK_MENU_15"; break;
	case V3DK_MENU_16			: str = "V3DK_MENU_16"; break;
	case V3DK_USER				: str = "V3DK_USER"; break;

	default:
		str = "Unrecognized";
		break;
	}

  return str;
}

/*----------------------------------------------------------------------
* Function: SbButtonPressEvent()
*
* Description:
*    This function receives 3D mouse button information and prints out the 
*    info on screen.
*    
*
* Args:
*    int     buttonnumber   //Contains number of button pressed 
*
* Return Value:
*    NONE
*
*----------------------------------------------------------------------*/
void SbButtonPressEvent( int buttonnumber, bool b_press )
{
	DBG_PRINT_STRING( "%s %s (%d)", b_press ? "Pressed": "Release", V3DKeyToName(V3DKey(buttonnumber)), buttonnumber );

	//REAL v = b_press ? 1 : 0;
	// (provide a switch statement for easy cut & paste)
	switch( buttonnumber ) 
	{
	case s3dm::V3DK_LEFT	: spaceball->set_button( 0, b_press ); break;
	case s3dm::V3DK_RIGHT	: spaceball->set_button( 1, b_press ); break;
	default:	break;
	}
}	


void HandleDeviceChangeEvent( SiSpwEvent* pEvent )
{
//  hdc = GetDC(hWndMain);

	switch( pEvent->u.deviceChangeEventData.type )
	{
	case SI_DEVICE_CHANGE_CONNECT:
//("Device ID %d connected"),pEvent->u.deviceChangeEventData.devID);
		break;
	case SI_DEVICE_CHANGE_DISCONNECT:
//("Device ID %d disconnected"),pEvent->u.deviceChangeEventData.devID);
		break;
	default:
//("Unknown deviceChangeEvent type: %d"),pEvent->u.deviceChangeEventData.type);
		break;
	}
//  ReleaseDC(hWndMain,hdc);
}

void HandleV3DCMDEvent(SiSpwEvent *pEvent)
{
//	hdc = GetDC(hWndMain);
	switch(pEvent->u.cmdEventData.functionNumber)
	{
	case V3DCMD_MENU_OPTIONS:	//"V3DCMD_MENU_OPTIONS(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_VIEW_FIT:		//"V3DCMD_VIEW_FIT(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_KEY_F1:			//"V3DCMD_KEY_F1(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_KEY_F2:			//"V3DCMD_KEY_F2(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_KEY_F3:			//"V3DCMD_KEY_F3(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_KEY_F4:			//"V3DCMD_KEY_F4(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_KEY_F5:			//"V3DCMD_KEY_F5(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_KEY_F6:			//"V3DCMD_KEY_F6(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_KEY_F7:			//"V3DCMD_KEY_F7(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_KEY_F8:			//"V3DCMD_KEY_F8(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_KEY_F9:			//"V3DCMD_KEY_F9(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_KEY_F10:		//"V3DCMD_KEY_F10(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_KEY_F11:		//"V3DCMD_KEY_F11(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_KEY_F12:		//"V3DCMD_KEY_F12(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_VIEW_TOP:		//"V3DCMD_VIEW_TOP(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_VIEW_LEFT:		//"V3DCMD_VIEW_LEFT(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_VIEW_RIGHT:		//"V3DCMD_VIEW_RIGHT(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_VIEW_FRONT:		//"V3DCMD_VIEW_FRONT(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_VIEW_BOTTOM:	//"V3DCMD_VIEW_BOTTOM(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_VIEW_BACK:		//"V3DCMD_VIEW_BACK(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_VIEW_ROLLCW:	//"V3DCMD_VIEW_ROLLCW(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_VIEW_ROLLCCW:	//"V3DCMD_VIEW_ROLLCCW(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_VIEW_ISO1:		//"V3DCMD_VIEW_ISO1(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_VIEW_ISO2:		//"V3DCMD_VIEW_ISO2(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_VIEW_1:			//"V3DCMD_VIEW_1(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_VIEW_2:			//"V3DCMD_VIEW_2(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_VIEW_3:			//"V3DCMD_VIEW_3(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_SAVE_VIEW_1:	//"V3DCMD_SAVE_VIEW_1(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_SAVE_VIEW_2:	//"V3DCMD_SAVE_VIEW_2(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_SAVE_VIEW_3:	//"V3DCMD_SAVE_VIEW_3(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_VIEW_QZ_IN:		//"V3DCMD_VIEW_QZ_IN(%s)"), pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_VIEW_QZ_OUT:	//"V3DCMD_VIEW_QZ_OUT(%s)"),pEvent->u.cmdEventData.pressed ? L"Pressed" : L"Released");
		break;
	case V3DCMD_MOTIONMACRO_ZOOM:	//("V3DCMD_MOTIONMACRO_ZOOM(ZoomFrom=%d, ZoomTo=%d, Scale=%f)"),pEvent->u.cmdEventData.iArgs[0],pEvent->u.cmdEventData.iArgs[1],pEvent->u.cmdEventData.fArgs[0]);
		break;
	case V3DCMD_MOTIONMACRO_ZOOMIN_CENTERTOCENTER:	//("V3DCMD_MOTIONMACRO_ZOOMIN_CENTERTOCENTER(Scale=%f)"),pEvent->u.cmdEventData.fArgs[0]);
		break;
	case V3DCMD_MOTIONMACRO_ZOOMIN_CURSORTOCENTER:	//("V3DCMD_MOTIONMACRO_ZOOMIN_CURSORTOCENTER(Scale=%f)"),pEvent->u.cmdEventData.fArgs[0]);
		break;
	case V3DCMD_MOTIONMACRO_ZOOMIN_CURSORTOCURSOR:	//("V3DCMD_MOTIONMACRO_ZOOMIN_CURSORTOCURSOR(Scale=%f)"),pEvent->u.cmdEventData.fArgs[0]);
		break;
	case V3DCMD_MOTIONMACRO_ZOOMOUT_CENTERTOCENTER:	//("V3DCMD_MOTIONMACRO_ZOOMOUT_CENTERTOCENTER(Scale=%f)"),pEvent->u.cmdEventData.fArgs[0]);
		break;
	case V3DCMD_MOTIONMACRO_ZOOMOUT_CURSORTOCENTER:	//("V3DCMD_MOTIONMACRO_ZOOMOUT_CURSORTOCENTER(Scale=%f)"),pEvent->u.cmdEventData.fArgs[0]);
		break;
	case V3DCMD_MOTIONMACRO_ZOOMOUT_CURSORTOCURSOR:	//("V3DCMD_MOTIONMACRO_ZOOMOUT_CURSORTOCURSOR(Scale=%f)"),pEvent->u.cmdEventData.fArgs[0]);
		break;
	default:					//("Unhandled V3DCMD: number = %d"),pEvent->u.cmdEventData.functionNumber);
		break;
	}
//	ReleaseDC(hWndMain,hdc);
}

void HandleAppEvent(SiSpwEvent *pEvent)
{
// TCHAR buf[100];
//  hdc = GetDC(hWndMain);
 //("AppCmd: %S %s"), pEvent->u.appCommandData.id.appCmdID,pEvent->u.appCommandData.pressed ? _T(" pressed") : _T(" released"));
  
}

LRESULT
WINAPI
sb_win_proc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam)
{
	if( !devHdl )
		return 0;

//	MSG msg;	//	incoming message to be evaluated
	BOOL           handled;  //	is message handled yet
	SiSpwEvent     Event;    //	SpaceWare Event
	SiGetEventData EData;    //	SpaceWare Event Data

	handled = SPW_FALSE;     //	init handled

	// init Window platform specific data for a call to SiGetEvent
	SiGetEventWinInit( &EData, Msg, wParam, lParam );

	// check whether msg was a 3D mouse event and process it
	if( SiGetEvent(devHdl, SI_AVERAGE_EVENTS, &EData, &Event ) == SI_IS_EVENT )
	{
		if( Msg != 0xc302 && Msg != 0xc4cf )
		{
			DBG_PRINT_STRING( "Msg %d is not 0xc302 or 0xc4cf", Msg );
		}
		switch( Event.type  )
		{
		case SI_MOTION_EVENT:			 //	3D mouse motion event
			sb_motion_event( &Event );      
			break;
		case SI_ZERO_EVENT:
			sb_zero_event();        	// 3D mouse zero event
			break;
		case SI_BUTTON_PRESS_EVENT:		//	Single button events (replace SI_BUTTON_EVENT)
		case SI_BUTTON_RELEASE_EVENT:
			SbButtonPressEvent( Event.u.hwButtonEvent.buttonNumber, Event.type==SI_BUTTON_PRESS_EVENT );
			break;

		// M�a never seen these 3 types of event
		case SI_DEVICE_CHANGE_EVENT:	//	Connect or disconnect device events
			HandleDeviceChangeEvent( &Event );
			break;
		case SI_CMD_EVENT:				//	V3DCMD_ events
			HandleV3DCMDEvent( &Event );
			break;
		case SI_APP_EVENT:				//	Application functions
			HandleAppEvent( &Event );
			break;
		case SI_COMBO_EVENT:			//	Not implemented
		case SI_EXCEPTION_EVENT:		//	Driver use only
		case SI_OUT_OF_BAND:			//	Driver use only
		case SI_ORIENTATION_EVENT:		//	Driver use only
		case SI_KEYBOARD_EVENT:			//	Driver use only
		case SI_LPFK_EVENT:				//	Driver use only

		case SI_SYNC_EVENT:				//	GUI synchronization events

		case SI_MOUSE_EVENT:			//	Driver use only
		case SI_JOYSTICK_EVENT:			//	Driver use only

		case SI_MOTION_HID_EVENT:		//	Motion event in HID nomenclature order
		case SI_SETTING_CHANGED_EVENT:	//	One or more smart ui settings have changed
		case SI_RECONNECT_EVENT:		//	Occurs if the driver reconnects to the application
		default:
			break;
		}
		return 1;
	}

//	return( (int) msg.wParam );
	return 0;
}

#endif	//#if AAA_TRACKER_SPACEBALL()
