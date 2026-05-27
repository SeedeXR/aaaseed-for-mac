/**************************************************************************
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
 *  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
 *  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
 *  PURPOSE.
 *
 *  Copyright (C) 1992 - 1996 Microsoft Corporation.  All Rights Reserved.
 *
 **************************************************************************/
/*--------------------------------------------------------------------
|
| MovPlay1.c - Sample Win app to play AVI movies using mciSendCommand
|
| Movie Functions supported:
|	Play/Pause
|	Home/End
|	Step/ReverseStep
|
+--------------------------------------------------------------------*/


#define		STRICT
#include <windows.h>
#include <mmsystem.h>
#include <digitalv.h>
#include "Thread/aaa_thread.h"
#include "err.h"

/* Menu items */
/* File menu */
#define	IDM_OPEN	1
#define	IDM_CLOSE	2
#define	IDM_EXIT	3
#define	IDM_ANOTHER	4

/* Movie menu */
#define	IDM_PLAY	10
#define	IDM_HOME	11
#define	IDM_END		12
#define	IDM_STEP	13
#define	IDM_RSTEP	14
#define	IDM_RPLAY	15

/* Help menu */
#define	IDM_ABOUT	20

/* About Dialog */
#define	IDD_ABOUT	101

//stringtable ID's
#define	IDS_APPNAME			1
#define	IDS_APPERR			2
#define	IDS_REGCLSFAILED	3
#define	IDS_CRTWNDFAILED	4
#define	IDS_NOOPEN			5
#define	IDS_FILTERSTRING	6

//----------------------------------------------------------------------------

CONST	UINT32	BUFSIZE = 260;
#define	MOVPLAY_CLASS	"movplay"
#define	AVI_VIDEO		L"avivideo"

//----------------------------------------------------------------------------
//function declarations

LRESULT	CALLBACK	WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL	CALLBACK	AboutDlgProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);


static	BOOL	initAVI();
static	void	termAVI();
static	HWND	initApp(HINSTANCE, HINSTANCE, int);
static	void	menubarUpdate(HWND);
static	void	titlebarUpdate(HWND hWnd, LPSTR lpstrMovie);
static	void	positionMovie(HWND);
static	void	fileCloseMovie(HWND);
static	void	fileOpenMovie(HWND);
extern	void	playMovie(HWND, int);
static	void	seekMovie(HWND, int);
static	void	stepMovie(HWND, int);
static	BOOL	FormatFilterString(UINT, LPSTR);
static	void	MovPlay_OnInitMenuPopup(HWND, HMENU, int, BOOL);
static	void	MovPlay_OnCommand(HWND, int, HWND, UINT);
static	void	MovPlay_OnSize(HWND, UINT, int, int);
static	void	MovPlay_OnDestroy(HWND);

//----------------------------------------------------------------------------
//AVI stuff to keep around

HINSTANCE	ghInst;
MCIDEVICEID	gwMCIDeviceID	= 0;		/* MCI Device ID for the AVI file */
HWND		ghwndMovie;					/* window handle of the movie */
BOOL		gfPlaying		= FALSE;	/* Play flag: TRUE == playing, FALSE == paused */
BOOL		gfMovieOpen		= FALSE;	/* Open flag: TRUE == movie open, FALSE = none */
HANDLE		ghAccel			= nullptr;		/* accelerator table */
HMENU		ghMenuBar		= nullptr;		/* menu bar handle */
char		gszBuffer[BUFSIZE];

//----------------------------------------------------------------------------
//maa
BOOL	b_waiting_async = FALSE;
/*--------------------------------------------------------------+
| initAVI - initialize avi libraries							|
|																|
+--------------------------------------------------------------*/
static BOOL initAVI( void )
{
	MCI_DGV_OPEN_PARMS	mciOpen;
		
	/* set up the open parameters */
	mciOpen.dwCallback 			= 0L;
	mciOpen.wDeviceID 			= 0;
	mciOpen.lpstrDeviceType 	= AVI_VIDEO;
	mciOpen.lpstrElementName 	= nullptr;
	mciOpen.lpstrAlias 			= nullptr;
	mciOpen.dwStyle 			= 0;
	mciOpen.hWndParent 			= nullptr;
		
	/* try to open the driver */
	return ( mciSendCommand(0, MCI_OPEN, (DWORD)(MCI_OPEN_TYPE), (DWORD)(LPMCI_DGV_OPEN_PARMS)&mciOpen) == 0 );
}

/*--------------------------------------------------------------+
| termAVI - Closes the opened AVI file and the opened device    |
|           type.                                               |
|                                                               |
+--------------------------------------------------------------*/
void termAVI()
{
	MCIDEVICEID			mciID;
	MCI_GENERIC_PARMS	mciClose;
	//
	// Get the device ID for the opened device type and then close
	// the device type.
	mciID = mciGetDeviceID( AVI_VIDEO );
	mciSendCommand( mciID, MCI_CLOSE, 0L, (DWORD)(LPMCI_GENERIC_PARMS)&mciClose );
}


/*--------------------------------------------------------------+
| MovPlay_OnCommand - Message handler for WM_COMMAND			|
|																|
+--------------------------------------------------------------*/
static void MovPlay_OnCommand( HWND hWnd, int id, HWND hwndCtl, UINT codeNotify )
{
	/* handle the menu commands */
	switch ( id )
		{
		case IDM_ABOUT:
			DialogBox(ghInst, MAKEINTRESOURCE(IDD_ABOUT), hWnd, (DLGPROC)AboutDlgProc );
			break;
		/* File Menu */
		case IDM_OPEN:
			fileOpenMovie( hWnd );
			break;
		case IDM_CLOSE:
			fileCloseMovie( hWnd );
			break;
		case IDM_EXIT:
			PostMessage( hWnd, WM_CLOSE, 0, 0L );
			break;
		/* Movie Menu - note some of these are by */
		/* keyboard only, especially the REVERSE  */
		/* commands.				*/
		case IDM_PLAY:
		case IDM_RPLAY:
			playMovie( hWnd, id );
			break;
		case IDM_HOME:
		case IDM_END:
			seekMovie( hWnd, id );
			break;
		case IDM_STEP:
		case IDM_RSTEP:
			stepMovie( hWnd,id );
			break;
	}
	return;
}
/*--------------------------------------------------------------+
| MovPlay_OnSize - Message handler for WM_SIZE					|
|																|
+--------------------------------------------------------------*/
static void MovPlay_OnSize( HWND hWnd, UINT state, int cx, int cy )
{
	positionMovie( hWnd );	/* re-center the movie */
	return;
}
/*--------------------------------------------------------------+
| MovPlay_OnDestroy - Message handler for WM_DESTROY			|
|																|
+--------------------------------------------------------------*/
static void MovPlay_OnDestroy( HWND hWnd )
{
	if ( gfMovieOpen )
		fileCloseMovie(hWnd );
	termAVI();
	PostQuitMessage(0);
	return;
}

/*
//--------------------------------------------------------------+
| WndProc - window proc for the app								|
|																|
+--------------------------------------------------------------//
LRESULT CALLBACK WndProc(
HWND hWnd,
UINT message,
WPARAM wParam,
LPARAM lParam)
{
	switch (message){
	case WM_INITMENUPOPUP:
		return HANDLE_WM_INITMENUPOPUP(hWnd, wParam, lParam, MovPlay_OnInitMenuPopup);
		break;

	case WM_COMMAND:
		return HANDLE_WM_COMMAND(hWnd, wParam, lParam, MovPlay_OnCommand );
		break;

	case WM_SIZE:
		return HANDLE_WM_SIZE(hWnd, wParam, lParam, MovPlay_OnSize);
		break;

	case WM_DESTROY:
		return HANDLE_WM_DESTROY(hWnd, wParam, lParam, MovPlay_OnDestroy);
		break;

		// We need to realize the AVI's palette
	case WM_QUERYNEWPALETTE:
	case WM_PALETTECHANGED:
	case WM_ACTIVATE:
		{
		MCI_GENERIC_PARMS	mciGeneric;

		// buggy drivers crash if we pass a null parms address
			mciSendCommand(gwMCIDeviceID, MCI_REALIZE,
				MCI_ANIM_REALIZE_NORM, (DWORD)(LPVOID)&mciGeneric);
		break;
		}
		
	case MM_MCINOTIFY:
		// This is where we check the status of an AVI
		// movie that might have been playing.  We do
		// the play with MCI_NOTIFY on so we should get
		// a MCI_NOTIFY_SUCCESSFUL if the play
		// completes on it's own.
		switch(wParam){
			case MCI_NOTIFY_SUCCESSFUL:
				// the play finished, let's rewind
				// and clear our flag.
				gfPlaying = FALSE;
				mciSendCommand(gwMCIDeviceID, MCI_SEEK,
						MCI_SEEK_TO_START,
						(DWORD)(LPVOID)NULL);
				return 0;
		}
	} // switch
	return DefWindowProc(hWnd, message, wParam, lParam);
}
*/

/*--------------------------------------------------------------+
| f - sets the movie rectangle <rcMovie> to be					|
|		centered within the app's window.						|
|																|
+--------------------------------------------------------------*/
static void positionMovie( HWND hWnd )
{
	RECT rcMovie;		/* the rect where the movie is positioned      */
				/* for QT/W this is the movie rect, for AVI    */
				/* this is the location of the playback window */
	RECT	rcClient, rcMovieBounds;
	MCI_DGV_RECT_PARMS	mciRect;

	/* if there is no movie yet then just get out of here */
	if (!gfMovieOpen)
		return;

	GetClientRect(hWnd, &rcClient);	/* get the parent windows rect */
	
	/* get the original size of the movie */
	mciSendCommand(gwMCIDeviceID, MCI_WHERE, (DWORD)(MCI_DGV_WHERE_SOURCE), (DWORD)(LPMCI_DGV_RECT_PARMS)&mciRect);
	CopyRect( &rcMovieBounds, &mciRect.rc );	/* get it in the movie bounds rect */

	rcMovie.left = (rcClient.right/2) - (rcMovieBounds.right / 2);
	rcMovie.top = (rcClient.bottom/2) - (rcMovieBounds.bottom / 2);
	rcMovie.right = rcMovie.left + rcMovieBounds.right;
	rcMovie.bottom = rcMovie.top + rcMovieBounds.bottom;

	/* reposition the playback (child ) window */
	MoveWindow( ghwndMovie, rcMovie.left, rcMovie.top, rcMovieBounds.right, rcMovieBounds.bottom, TRUE);
//hackbpi was			rcMovieBounds.right, rcMovieBounds.bottom, TRUE);
}

static void movie_get_status()
{
	if (gfMovieOpen)
	{
/*
typedef struct tagMCI_STATUS_PARMS {
	DWORD	dwCallback;
	DWORD	dwReturn;
	DWORD	dwItem;
	DWORD	dwTrack;
}
*/
		tagMCI_STATUS_PARMS Status;
		Status.dwItem = MCI_STATUS_MODE;
		if( mciSendCommand( gwMCIDeviceID, MCI_STATUS , MCI_WAIT|MCI_STATUS_ITEM, (DWORD) &Status) )
			ERR_PRINT_STRING( "movie status error" );
		else
			DBG_PRINT_STRING( "movie status returned ->0x%8x", Status.dwReturn);
	}
}


BOOL movie_is_playing()
{
	if (gfMovieOpen)
	{
		tagMCI_STATUS_PARMS Status;
		Status.dwItem = MCI_STATUS_MODE;
		if( mciSendCommand( gwMCIDeviceID, MCI_STATUS , MCI_WAIT|MCI_STATUS_ITEM, (DWORD) &Status) )
			return FALSE;
		else
			return (Status.dwReturn == MCI_MODE_PLAY);
	}
	return FALSE;
}

/*--------------------------------------------------------------+
| f - sets the movie rectangle <rcMovie> to be					|
|		centered within the app's window.						|
|																|
+--------------------------------------------------------------*/
void	movie_position( INT32 x, INT32 y )
{
	RECT rcMovie;	//	the rect where the movie is positioned
					//	for QT/W this is the movie rect, for AVI
					//	this is the location of the playback window
	RECT	rcMovieBounds;
	MCI_DGV_RECT_PARMS	mciRect;

	//	if there is no movie yet then just get out of here
	if ( !gfMovieOpen )
		return;
	if( b_waiting_async )
		return;
//	GetClientRect(hWnd, &rcClient);	//	get the parent windows rect
	
	//	get the original size of the movie
	mciSendCommand( gwMCIDeviceID, MCI_WHERE, (DWORD)(MCI_DGV_WHERE_SOURCE), (DWORD)(LPMCI_DGV_RECT_PARMS)&mciRect );
	CopyRect( &rcMovieBounds, &mciRect.rc );	//	get it in the movie bounds rect

	rcMovie.left = x;
	rcMovie.top = y;
	rcMovie.right = rcMovie.left + rcMovieBounds.right;
	rcMovie.bottom = rcMovie.top + rcMovieBounds.bottom;

	/* reposition the playback (child ) window */
	MoveWindow( ghwndMovie, rcMovie.left, rcMovie.top, rcMovieBounds.right, rcMovieBounds.bottom, TRUE );
}

void	movie_position( INT32 x, INT32 y, INT32 w, INT32 h )
{
	//	if there is no movie yet then just get out of here
	if (!gfMovieOpen)
		return;
	if( b_waiting_async )
		return;

	/* reposition the playback (child ) window */
//	MoveWindow( ghwndMovie, x, y, w, h, FALSE);
}

/*--------------------------------------------------------------+
| fileCloseMovie - close the movie and anything associated		|
|		   with it.												|
|																|
| This function clears the <gfPlaying> and <gfMovieOpen> flags	|
|																|
+--------------------------------------------------------------*/
static	void	fileCloseMovie(HWND hWnd )
{
	MCI_GENERIC_PARMS  mciGeneric;

	mciSendCommand( gwMCIDeviceID, MCI_CLOSE, 0L, (DWORD)(LPMCI_GENERIC_PARMS)&mciGeneric);

	gfPlaying = FALSE;	// can't be playing any longer
	gfMovieOpen = FALSE;	// no more movies open
	
//	titlebarUpdate(hWnd, NULL);	// title bar back to plain
//	menubarUpdate(hWnd );		// update menu bar
	
	/* cause a total repaint to occur */
	InvalidateRect(hWnd, nullptr, TRUE);
	UpdateWindow(hWnd );
}


/*--------------------------------------------------------------+
| fileOpenMovie - open an AVI movie. Use CommDlg open box to	|
|	        open and then handle the initialization to			|
|		show the movie and position it properly.  Keep			|
|		the movie paused when opened.							|
|																|
|		Sets <gfMovieOpened> on success.						|
+--------------------------------------------------------------*/
extern	void
/*
#ifdef	WIN32
 __cdecl
#endif
*/
 fileOpenMovie( HWND hWnd )
{
	OPENFILENAMEA ofn;

	static char szFile [BUFSIZE];
	static char szFileTitle [BUFSIZE];
	static char szFilter [BUFSIZE];

	FormatFilterString( IDS_FILTERSTRING, szFilter );

	/* use the OpenFile dialog to get the filename */
	MEMSET(&ofn, 0, sizeof(ofn));
	ofn.lStructSize		= sizeof(ofn);
	ofn.hwndOwner		= hWnd;
	ofn.lpstrFilter		= szFilter;
	ofn.lpstrFile		= szFile;
	ofn.nMaxFile		= sizeof(szFile);
	ofn.lpstrFileTitle	= szFileTitle;
	ofn.nMaxFileTitle	= sizeof(szFileTitle);
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	/* use CommDlg to get our filename */
	if (GetOpenFileNameA(&ofn))
	{
		MCI_DGV_OPEN_PARMSA		mciOpen;
		MCI_DGV_WINDOW_PARMSA	mciWindow;
		MCI_DGV_STATUS_PARMSA	mciStatus;

		/* we got a filename, now close any old movie and open */
		/* the new one.					*/
		if (gfMovieOpen)
			fileCloseMovie(hWnd );
	
		/* we have a .AVI movie to open, use MCI */
		/* set up the open parameters */
		mciOpen.dwCallback = 0L;
		mciOpen.wDeviceID = 0;
		mciOpen.lpstrDeviceType = nullptr;
		mciOpen.lpstrElementName = ofn.lpstrFile;
		mciOpen.lpstrAlias = nullptr;
		mciOpen.dwStyle = WS_CHILD;
		mciOpen.hWndParent = hWnd;

		/* try to open the file */
		if (mciSendCommand(0, MCI_OPEN,(DWORD)(MCI_OPEN_ELEMENT|MCI_DGV_OPEN_PARENT|MCI_DGV_OPEN_WS), (DWORD)(LPMCI_DGV_OPEN_PARMS)&mciOpen) == 0)
		{

			/* we opened the file o.k., now set up to */
			/* play it.					*/
			gwMCIDeviceID = mciOpen.wDeviceID;	/* save ID */
			gfMovieOpen = TRUE;	/* a movie was opened */

			/* show the playback window */
			mciWindow.dwCallback = 0L;
			mciWindow.hWnd = nullptr;
			mciWindow.nCmdShow = SW_SHOW;
			mciWindow.lpstrText = nullptr;
			mciSendCommand(gwMCIDeviceID, MCI_WINDOW, MCI_DGV_WINDOW_STATE, (DWORD)(LPMCI_DGV_WINDOW_PARMS)&mciWindow);

			/* get the window handle */
			mciStatus.dwItem = MCI_DGV_STATUS_HWND;
			mciSendCommand(gwMCIDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPMCI_STATUS_PARMS)&mciStatus);
			ghwndMovie = (HWND)mciStatus.dwReturn;

			/* now get the movie centered */
			positionMovie(hWnd );
		}
		else
		{
			/* generic error for open */
			LoadStringA( ghInst, IDS_NOOPEN, gszBuffer, BUFSIZE );
			MessageBoxA( hWnd, gszBuffer, nullptr, MB_ICONEXCLAMATION | MB_OK | MB_TOPMOST );
			gfMovieOpen = FALSE;
		}
	}
}

/*--------------------------------------------------------------+
| fileOpenMovie - open an AVI movie. Use CommDlg open box to	|
|	        open and then handle the initialization to			|
|		show the movie and position it properly.  Keep			|
|		the movie paused when opened.							|
|																|
|		Sets <gfMovieOpened> on success.						|
+--------------------------------------------------------------*/
extern	void
#ifdef	WIN32
 __cdecl
#endif
fileOpenMovie( HWND hWnd, char* filename )
{

	MCI_DGV_OPEN_PARMSA		mciOpen;
	MCI_DGV_WINDOW_PARMSA	mciWindow;
	MCI_DGV_STATUS_PARMSA	mciStatus;

	// we got a filename, now close any old movie and open
	//	the new one.
	if (gfMovieOpen)
		fileCloseMovie(hWnd );
	
	//	we have a .AVI movie to open, use MCI
	//	set up the open parameters */
	mciOpen.dwCallback = 0L;
	mciOpen.wDeviceID = 0;
	mciOpen.lpstrDeviceType = nullptr;
	mciOpen.lpstrElementName = filename;
	mciOpen.lpstrAlias = nullptr;
	mciOpen.dwStyle = WS_CHILD;
	mciOpen.hWndParent = hWnd;

	//	try to open the file
	if ( mciSendCommandA(0, MCI_OPEN, (DWORD)(MCI_OPEN_ELEMENT|MCI_DGV_OPEN_PARENT|MCI_DGV_OPEN_WS), (DWORD)(LPMCI_DGV_OPEN_PARMS)&mciOpen) == 0)
	{
		//	we opened the file o.k., now set up to play it.
		gwMCIDeviceID = mciOpen.wDeviceID;	//	save ID
		gfMovieOpen = TRUE;					//	a movie was opened

		//	show the playback window
		mciWindow.dwCallback = 0L;
		mciWindow.hWnd = nullptr;
//hackbpi		mciWindow.nCmdShow = SW_SHOW;
		mciWindow.nCmdShow = SW_HIDE;

		mciWindow.lpstrText = nullptr;
		mciSendCommandA( gwMCIDeviceID, MCI_WINDOW, MCI_DGV_WINDOW_STATE, (DWORD)(LPMCI_DGV_WINDOW_PARMS)&mciWindow );

		//	get the window handle
		mciStatus.dwItem = MCI_DGV_STATUS_HWND;
		mciSendCommandA(gwMCIDeviceID, MCI_STATUS, MCI_STATUS_ITEM, (DWORD)(LPMCI_STATUS_PARMS)&mciStatus);
		ghwndMovie = (HWND)mciStatus.dwReturn;

		//	now get the movie centered
		//hackbpi	positionMovie(hWnd );
	}
	else
	{
		//	generic error for open
		LoadStringA( ghInst, IDS_NOOPEN, gszBuffer, BUFSIZE );
		MessageBoxA( hWnd, gszBuffer, nullptr, MB_ICONEXCLAMATION|MB_OK|MB_TOPMOST );
		gfMovieOpen = FALSE;
	}
}

/*--------------------------------------------------------------+
| playMovie - play/pause the movie depending on the state		|
|		of the <gfPlaying> flag.								|
|																|
| This function sets the <gfPlaying> flag appropriately when done|
|																|
+--------------------------------------------------------------*/
void	playMovie( HWND hWnd, int nDirection )
{
	gfPlaying = !gfPlaying;	/* swap the play flag */
	if( !nDirection )
		gfPlaying = FALSE;	/* wDirection == NULL means PAUSE */

	/* play/pause the AVI movie */
	if ( gfPlaying )
	{
		DWORD				dwFlags;
		MCI_DGV_PLAY_PARMS	mciPlay;
		
		/* init to play all */
		mciPlay.dwCallback = MAKELONG(hWnd,0);
		mciPlay.dwFrom = mciPlay.dwTo = 0;
		dwFlags = MCI_NOTIFY;
		if ( nDirection == IDM_RPLAY )
			dwFlags |= MCI_DGV_PLAY_REVERSE;
		
		mciSendCommand( gwMCIDeviceID, MCI_PLAY, dwFlags, (DWORD)(LPMCI_DGV_PLAY_PARMS)&mciPlay );
	}
	else
	{
		MCI_DGV_PAUSE_PARMS	mciPause;
		/* tell it to pause */
		mciSendCommand( gwMCIDeviceID,MCI_PAUSE,0L, (DWORD)(LPMCI_DGV_PAUSE_PARMS)&mciPause );
	}
}

/*--------------------------------------------------------------+
| seekMovie - seek in the movie depending on the wAction.		|
|		Possible actions are IDM_HOME (start of movie) or		|
|		IDM_END (end of movie)									|
|																|
|		Always stop the play before seeking.					|
|																|
+--------------------------------------------------------------*/
static	void	seekMovie( HWND hWnd, int nAction )
{
	/* first stop the movie from playing if it is playing */
	if ( gfPlaying )
	{
		playMovie(hWnd, 0);
	}
	if (nAction == IDM_HOME)
	{
		/* home the movie */
		mciSendCommand(gwMCIDeviceID, MCI_SEEK, MCI_SEEK_TO_START,(DWORD)(LPVOID)NULL);
			
	}
	else if (nAction == IDM_END)
	{
		/* go to the end of the movie */
		mciSendCommand(gwMCIDeviceID, MCI_SEEK, MCI_SEEK_TO_END,(DWORD)(LPVOID)NULL);
	}
}

/*--------------------------------------------------------------+
| stepMovie - step forward or reverse in the movie.  wDirection	|
|		can be IDM_STEP (forward ) or IDM_RSTEP (reverse)		|
|																|
|		Again, stop the play if one is in progress.				|
|																|
+--------------------------------------------------------------*/
static	void	stepMovie( HWND hWnd, int nDirection )
{
	MCI_DGV_STEP_PARMS	mciStep;

	if ( gfPlaying )
		playMovie(hWnd, 0);  /* turn off the movie */

	mciStep.dwFrames = 1L;
	if ( nDirection == IDM_STEP )
		mciSendCommand(gwMCIDeviceID, MCI_STEP, MCI_DGV_STEP_FRAMES, (DWORD)(LPMCI_DGV_STEP_PARMS)&mciStep);
	else
		mciSendCommand( gwMCIDeviceID, MCI_STEP, MCI_DGV_STEP_FRAMES|MCI_DGV_STEP_REVERSE, (DWORD)(LPMCI_DGV_STEP_PARMS)&mciStep );
}
/*--------------------------------------------------------------+
| FormatFilterString 											|
|																|
+--------------------------------------------------------------*/
static	BOOL	FormatFilterString(
UINT uID,
LPSTR lpszString )
{
	int		nLength, nCtr = 0;
	char	chWildCard;
	
	*lpszString = 0;
	
	nLength = LoadStringA( ghInst, uID, lpszString, BUFSIZE );

	chWildCard = lpszString[nLength-1];

	while( lpszString[nCtr] )
	{
		if( lpszString[nCtr] == chWildCard )
			lpszString[nCtr] = 0;
		nCtr++;
	}

	return TRUE;
}				

#include "movie_ms.h"

HWND	win_handle = nullptr;

void	movie_choose()
{
	fileOpenMovie( get_window_main_handle() );
}

MCI_GENERIC_PARMS	param;
void	movie_choose( CHAR* filename )
{
	fileOpenMovie( get_window_main_handle(), filename );
//	mciSendCommand( gwMCIDeviceID, MCI_CONFIGURE, 0, (DWORD) (LPMCI_GENERIC_PARMS) &param);
}

char	str_movie[_MAX_PATH];
void
#ifdef	WIN32
__cdecl 
#endif
th_movie_choose_and_play( void* dummy )
{
	fileOpenMovie( win_handle, str_movie );
	movie_play();
	b_waiting_async = FALSE;
	thread_end();
}

void	movie_choose_and_play_async( CHAR* filename )
{
	b_waiting_async = TRUE;
	strcpy( str_movie, filename);
	win_handle = get_window_main_handle();
	thread_begin( th_movie_choose_and_play );
}

void	movie_play()
{
	if( gfMovieOpen )
		playMovie( get_window_main_handle(), 1 );
}

void	movie_stop()
{
	if( gfMovieOpen )
		playMovie( get_window_main_handle(), 0 );
}

void	movie_rewind()
{
	if( gfMovieOpen )
		seekMovie( get_window_main_handle(), IDM_HOME );
}

void	movie_close()
{
	if( gfMovieOpen )
		fileCloseMovie( get_window_main_handle() );
}
