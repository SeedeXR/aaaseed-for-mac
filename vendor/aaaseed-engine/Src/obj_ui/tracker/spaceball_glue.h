
#ifdef AAA_SPACEBALL_GLUE_H
#error "SPACEBALL_GLUE_H included more than once."
#endif
#define AAA_SPACEBALL_GLUE_H 1


#ifndef AAA_TRACKERS_H
#	include "obj_ui/tracker/trackers.h"
#endif

#if AAA_TRACKER_SPACEBALL()
#	ifndef	AAA_AAA_OS_H
#		include "aaa_os.h"
#	endif

extern bool sb_init();
extern bool sb_open();
extern void sb_deinit();

//extern int sb_process_message();

extern LRESULT WINAPI sb_win_proc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam);

#endif	//#if AAA_TRACKER_SPACEBALL()
