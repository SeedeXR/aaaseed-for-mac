#include "aaa_mem_win.h"
//#ifndef _INC_CRTDBG
//#include <crtdbg.h>
//#endif

namespace {
	_CRT_REPORT_HOOK hook_prev;
	bool b_report_absorb = false;
	c_memory_leak_detector_wrapper MLD; //this lives as long as this translation unit
}


static int aaa_hook( int reportType, char* userMessage, int* retVal )
{
	//_CrtDumpMemoryLeaks() outputs "Detected memory leaks!\n" and calls
	//_CrtDumpAllObjectsSince(NULL) which outputs all leaked objects,
	//ending this (possibly long) list with "Object dump complete.\n"
	//In between those two headings I want to swallow the report.

	if( !b_report_absorb )
	{
		if( strcmp(userMessage, "Detected memory leaks!\n" ) == 0)
			b_report_absorb = true;
	}
	else
	{
		if( strcmp(userMessage, "Object dump complete.\n" ) == 0)
		{
			b_report_absorb = false;
//			MLD.restore_hook();
		}
		else
			return 1;	//TRUE; //absorb it
	}
	return 0;	//FALSE; //give it back to _CrtDbgReport()
}

c_memory_leak_detector_wrapper::c_memory_leak_detector_wrapper()
{
	//don't swallow assert and trace reports
	b_report_absorb = false;
	//change the report function
	hook_prev = _CrtSetReportHook( aaa_hook );
}

void c_memory_leak_detector_wrapper::restore_hook()
{
	//reset the report function to the old one
	_CrtSetReportHook( hook_prev );
}

//this destructor is called after mfc has died
c_memory_leak_detector_wrapper::~c_memory_leak_detector_wrapper()
{
	//make sure that there is memory leak detection at the end of the program
	//_CrtSetDbgFlag( _CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_ALLOC_MEM_DF | _CRTDBG_CHECK_ALWAYS_DF);

	//restore_hook()
}
