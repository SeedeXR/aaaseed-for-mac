/*|------------------------------------------------------------

	MAGELLAN Windows NT Driver     Link      Version 1.0 06.10.95

	Logitech Inc.
	6505 Kaiser Drive
	Fremont, CA 94555-3615

	Volker Senft:

	Main:   (510) 795-8500
	Direct: (510) 713-4055
	Sales:  (800) 231-7717
	Fax:    (510) 505-0979

	06-Oct-95 G-01-08  VS   $$1 Link functions to the Magellan driver
  --------------------------------------------------------------|*/
#include <string.h>
#include <math.h>

#include <aaa_os.h>
#include <winuser.h>

#include "mgldll.h"

/*|-------------------- Private Definitions --------------------|*/

static MagellanControl MagellanControlInfoDLL;

/*|-------------------------- Functions ------------------------|*/

/*|------- MagellanSetWindow ------------------------------------------------|*/
int WINAPI MagellanSetWindow( HMAGELLAN MagellanHandle, HWND ApplicationWindow )
{
	if( !MagellanHandle )
		return FALSE;
	else
	{
		MagellanHandle->MagellanWindow =	ApplicationWindow;
		return MagellanInfoWindow( MagellanHandle, (WPARAM) MagellanSetWindowCommand, (LPARAM) ApplicationWindow );
	};
}

/*|------- MagellanInfoWindow ----------------------------------------------------------------------|*/
int WINAPI MagellanInfoWindow( HMAGELLAN MagellanHandle, WPARAM MagellanInfoType, LPARAM MagellanInfo )
{
	HWND MagellanDriverWindow;

	if( !MagellanHandle )
		return FALSE;
	else
	{
		MagellanDriverWindow = FindWindowW( MagellanClassName, 0 );
		if( !MagellanDriverWindow )
			return FALSE;
		PostMessage( MagellanDriverWindow, MagellanHandle->MagellanCommandEvent, 
				(WPARAM) MagellanInfoType, (LPARAM) MagellanInfo );    
		return TRUE;
	};
	return FALSE;
}

/*|------------- MagellanInit ----------------------|*/
HMAGELLAN WINAPI MagellanInit( HWND ApplicationWindow )
{
	memset( &MagellanControlInfoDLL, 0x00, sizeof( MagellanControlInfoDLL ) );

	MagellanControlInfoDLL.MagellanDriverWindow = FindWindowW( MagellanClassName, 0 );
	if( !MagellanControlInfoDLL.MagellanDriverWindow )
		return 0;	/*| Can't find Magellan driver |*/

	MagellanControlInfoDLL.MagellanWindow = ApplicationWindow;

	MagellanControlInfoDLL.MagellanMotionEvent			= RegisterWindowMessageW( L"MotionEvent" );
	MagellanControlInfoDLL.MagellanButtonPressEvent		= RegisterWindowMessageW( L"ButtonPressEvent" );
	MagellanControlInfoDLL.MagellanButtonReleaseEvent	= RegisterWindowMessageW( L"ButtonReleaseEvent" );
	MagellanControlInfoDLL.MagellanCommandEvent			= RegisterWindowMessageW( L"CommandEvent" );
	if(	(MagellanControlInfoDLL.MagellanMotionEvent==0) || 
			(MagellanControlInfoDLL.MagellanButtonPressEvent==0) ||
			(MagellanControlInfoDLL.MagellanButtonReleaseEvent==0) ||
			(MagellanControlInfoDLL.MagellanCommandEvent==0) )
		return 0;		/*| Can't create the Magellan event types |*/

	if( MagellanSetWindow( &MagellanControlInfoDLL, ApplicationWindow ) )
		return &MagellanControlInfoDLL;
	else
		return 0;
}

/*|------- MagellanClose ------------------------|*/
int WINAPI MagellanClose( HMAGELLAN MagellanHandle )
{
	if( !MagellanHandle )
		return FALSE;

	MagellanSetWindow( MagellanHandle, MagellanHandle->MagellanDriverWindow );
	memset( MagellanHandle, 0x00, sizeof( *MagellanHandle ) );
	return TRUE;
}

/*|------- MagellanTranslateEvent -------------------------------------|*/
int WINAPI MagellanTranslateEvent( HMAGELLAN MagellanHandle, LPMSG Message,
		 								MagellanIntegerEvent *MagellanEvent )
{
MagellanCompress MagellanData;

	if( !MagellanHandle )
		return FALSE;

	if( Message->message == MagellanHandle->MagellanMotionEvent )
	{
		MagellanEvent->MagellanWindow = Message->hwnd;

		//todox64 is this cast a good solution
		MagellanData.MagellanDWord = (DWORD) Message->wParam;
		MagellanEvent->MagellanData[ MagellanX ] = MagellanData.MagellanValues.ValueAX;
		MagellanEvent->MagellanData[ MagellanY ] = MagellanData.MagellanValues.ValueBY;
		MagellanEvent->MagellanData[ MagellanZ ] = MagellanData.MagellanValues.ValueCZ;

		//todox64 is this cast a good solution
		MagellanData.MagellanDWord = (DWORD) Message->lParam;
		MagellanEvent->MagellanData[ MagellanA ] = MagellanData.MagellanValues.ValueAX;
		MagellanEvent->MagellanData[ MagellanB ] = MagellanData.MagellanValues.ValueBY;
		MagellanEvent->MagellanData[ MagellanC ] = MagellanData.MagellanValues.ValueCZ;

		MagellanEvent->MagellanPeriod = 60;
		MagellanEvent->MagellanType = MotionEvent;
		return MotionEvent;
	};

	if( Message->message == MagellanHandle->MagellanButtonPressEvent )
	{
		MagellanEvent->MagellanWindow = Message->hwnd;
		MagellanEvent->MagellanButton = (int) Message->wParam;
		MagellanEvent->MagellanType = ButtonPressEvent;
		return ButtonPressEvent;
	};

	if( Message->message == MagellanHandle->MagellanButtonReleaseEvent )
	{
		MagellanEvent->MagellanWindow = Message->hwnd;
		MagellanEvent->MagellanButton = (int) Message->wParam;
		MagellanEvent->MagellanType = ButtonReleaseEvent;
		return ButtonReleaseEvent;
	};

	return FALSE;
}

/*------------------------------ EOF ------------------------------------|*/
