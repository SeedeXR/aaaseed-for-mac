#include "Afxwin.h"
#ifndef AAA_GYPSY_H
#	include "gypsy/gypsy.h"
#endif
#include "resource.h"

class CInputOpt : public CDialog
{
public:
	// Construction
	CInputOpt(CWnd* pParent = NULL);   // standard constructor

	//Sets up list of suits
	void InitSuitList();

// Dialog Data
	//{{AFX_DATA(CInputOpt)
	enum { IDD = IDD_INPUTOPTIONS };
	float	m_FPS;
	int		m_limit;
	BOOL	m_stop;
	int		m_suit_number;
	BOOL	m_delayEnable;
	int		m_delay;
	BOOL	m_beep;
	BOOL	m_usePosition;
	BOOL	m_Autoload;
	//}}AFX_DATA
	BOOL	m_FramesOrSeconds;	//False if m_limit is in seconds
	int		m_frames;			//Total frame limit
	SuitInfo m_info;			//Info for current suit


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputOpt)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInputOpt)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnFrames();
	afx_msg void OnSeconds();
	afx_msg void OnFind();
	afx_msg void OnSelchangeSuitnumber();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

HGYPSY	hGypsy = NULL;
BOOL RecordFramesOrSeconds = FALSE;
int RecordLimitFS = 5;
BOOL RecordStop = FALSE;
BOOL RecordBeep = TRUE;
BOOL RecordDelayEnable = FALSE;
int RecordDelay = 3;
BOOL UsePositionSensor = FALSE;
BOOL Autoload = TRUE;
BOOL IgnoreLowerBody = FALSE;
BOOL LookForSuitOnStartup = TRUE;

void Callback(Frame *frame, SuitInfo *info)
{
/*
	static BOOL StopRecording = FALSE;
	int FrameLimit;
	int i;

	if(!Record) StopRecording = FALSE;

	if(!Record && !Live) return;
	if(Play) return;

	//If we have multiple suits open and we vector into the
	//same callback for all of them, we can tell which
	//suit is sending this data by checking info.

	if(IgnoreLowerBody) {
		//Set all the channels for the lower body to 0
		//These are the channels below CHANNEL_CHEST_X
		for(i = 0; i < CHANNEL_CHEST_X; ++i)
		 frame->channel[i] = 0.f;
	}

	//Store the new frame if we're recording and not paused
	if(Record && !Pause) {
		//Calculate record limit in frames
		if(RecordFramesOrSeconds)
		 FrameLimit = RecordLimitFS;
		else
		 FrameLimit = (int)(RecordLimitFS * info->FrameRate + .5f);

		//See if we need to stop because we hit record limit
		if(skeleton.NumFrames >= FrameLimit && RecordStop) {
			//Stop recording
			//We do it this way instead of pMainFrame->OnActionsStop();
			//because we are running in a time critical thread here
			//Stop recording until the message is processed
			StopRecording = TRUE;
			pMainFrame->PostMessage(WM_COMMAND, ID_ACTIONS_STOP);
			//We could instead use SendMessage(), but then we would
			//drop frames while we wait for our message to be processed
		} else {
			//Add one frame to frames in skeleton
			if(!GypsyAddFrame(&skeleton)) {
				//Stop recording
				//We do it this way instead of pMainFrame->OnActionsStop();
				//because we are running in a time critical thread here
				//Stop recording until the message is processed
				StopRecording = TRUE;
				pMainFrame->PostMessage(WM_COMMAND, ID_ACTIONS_STOP);
				//We could instead use SendMessage(), but then we
				//drop frames while we wait for our message to be processed

				//Tell user we ran out of memory
				AfxMessageBox("Out of memory during record");
			} else {
				//Add this frame to the list
				memcpy(&skeleton.frames[skeleton.NumFrames - 1],
					   frame, sizeof(Frame));
			}
		}
	}

	//We're currently redrawing a frame, so don't wipe out LiveFrame
	if(InRedraw) return;
	InRedraw = TRUE;

	//We are Live or Recording, so show the current frame
	//Update the most recent samples here into LiveFrame
	memcpy(&LiveFrame, frame, sizeof(Frame));
	pMainFrame->Invalidate();
*/
}

//This function is called by Gypsy when a suit times out
//This function is reentrant
void Timeout(SuitInfo *info)
{
	static BOOL InTimeout = FALSE;
	char msg[100];

	//For this program, we'll handle reentrancy by ignoring
	//subsequent timeouts while we are still telling the
	//user about this one
	if(InTimeout) return;
	InTimeout = TRUE;

	if(*info->name) {
		sprintf(msg, "Timeout on suit at %d.%d.%d.%d:%d \"%s\"",
				info->address.b[0], info->address.b[1],
				info->address.b[2], info->address.b[3],
				info->port,
				info->name);
	} else {
		sprintf(msg, "Timeout on suit at %d.%d.%d.%d:%d",
				info->address.b[0], info->address.b[1],
				info->address.b[2], info->address.b[3],
				info->port);
	}

	if(AfxMessageBox(msg, MB_RETRYCANCEL | MB_ICONEXCLAMATION) == IDCANCEL) {
		//The user wants to stop listening to this suit
//hack		pMainFrame->OnActionsStop();
		GypsyClose(hGypsy);
		hGypsy = NULL;
	}

	InTimeout = FALSE;
}

void Inputoptions() 
{
	CInputOpt dlg;

	//Get the info for the current suit (if any)
	GypsyGetSuitInfo( hGypsy, &dlg.m_info);

	//Now close the suit
	//Since clicking on the Find Suits button of the dialog
	//calls GypsyFindSuits(), we must have no open handles when
	//we call this dialog
	GypsyClose(hGypsy);

	//Calculate record limit in frames
	if(RecordFramesOrSeconds)
	 dlg.m_frames = RecordLimitFS;
	else
	 dlg.m_frames = (int)(RecordLimitFS * dlg.m_info.FrameRate + .5f);

	dlg.m_FramesOrSeconds = RecordFramesOrSeconds;
	dlg.m_stop = RecordStop;
	dlg.m_delayEnable = RecordDelayEnable;
	dlg.m_delay = RecordDelay;
	dlg.m_beep = RecordBeep;
	dlg.m_usePosition = UsePositionSensor;
	dlg.m_Autoload = Autoload;

	if(dlg.DoModal() == IDOK) {
		RecordFramesOrSeconds = dlg.m_FramesOrSeconds;
		RecordStop = dlg.m_stop;
		RecordDelayEnable = dlg.m_delayEnable;
		RecordDelay = dlg.m_delay;
		RecordBeep = dlg.m_beep;
		UsePositionSensor = dlg.m_usePosition;
		Autoload = dlg.m_Autoload;

		//Calculate record limit in frames or seconds
		if(RecordFramesOrSeconds)
		 RecordLimitFS = dlg.m_frames;
		else
		 RecordLimitFS = (int)(dlg.m_frames / dlg.m_info.FrameRate + .5f);

	}

	//The dialog can change Handle even if Cancel is selected
	//if the user clicks on Find Suits, so update here
	//Open the suit with the desired info
	hGypsy = GypsyOpen(&dlg.m_info, Callback, Timeout);

	//Make sure Gypsy flags are updated
	GypsyEnablePositionSensor(hGypsy, UsePositionSensor);

	//Set the floor geometry
//hack	GypsySetFloor(CurrentFloor);
}
