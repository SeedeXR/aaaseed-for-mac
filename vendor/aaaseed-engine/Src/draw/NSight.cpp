#include "NSightEvents.h"
#include "err.h"


namespace nsight {

	bool	b_started_out		;
	bool	b_active_ui			;
	bool	b_active_event_ui	;

#if AAA_NSIGHT_USE()

	//nvtxEventAttributes_t eventAttr = {
	//	NVTX_VERSION,
	//	sizeof(nvtxEventAttributes_t),
	//	1,
	//	NVTX_COLOR_ARGB,
	//	0xFF00FF11,
	//	NVTX_PAYLOAD_TYPE_INT64,
	//	0,
	//	NVTX_MESSAGE_TYPE_ASCII,
	//	0
	//};

	// For dynamic load (and to avoid linking with any NSight library

	nvtxRangePushEx_Pfn		nvtxRangePushEx_dyn		= nullptr;
	nvtxRangePush_Pfn		nvtxRangePush_dyn		= nullptr;
	nvtxRangePop_Pfn		nvtxRangePop_dyn		= nullptr;
	nvtxMarkA_Pfn			nvtxMarkA_dyn			= nullptr;

	nvtxEventAttributes_t	g_nv_event_range			= {0};	
	nvtxEventAttributes_t	g_nv_event_aaa_evt_range	= {0};

	bool	b_active			;
	bool	b_active_event		;
#endif //#if AAA_NSIGHT_USE()

};

#if AAA_NSIGHT_USE()
void nsight::init()
{
	GOOD_PRINT_STRING( "nsight::init()" );

	b_started_out	= false;
	b_active		= false;
	b_active_event	= false;
//	if( !b_start_ui )
//		return;

#ifdef TESTIFNULL
#	undef TESTIFNULL
#endif
#define TESTIFNULL(x) if(nullptr == (x)) { { ERR_PRINT_STRING( "Error>> NSight dll doesn't have" #x); } return; }

	// Let's dynamically load this library so we can avoid it if not here.
	WCHAR const * const lib_name =
#if AAA_WIN64()
							L"nvToolsExt64_1.dll";
#else
							L"nvToolsExt32_1.dll";
#endif	//#if AAA_WIN64()

	GOOD_PRINT_STRING( "NSight : Will try to load lib %ls", lib_name );

	HMODULE hLibNSight = LoadLibraryW( lib_name );
	if( !hLibNSight )
	{
		ERR_PRINT_STRING( "NSight : No dll %ls found.", lib_name );
		return;
	}
	
	TESTIFNULL(	nvtxRangePushEx_dyn	= (nvtxRangePushEx_Pfn)	GetProcAddress(	hLibNSight,	"nvtxRangePushEx"	)	);
	TESTIFNULL(	nvtxRangePush_dyn	= (nvtxRangePush_Pfn)	GetProcAddress(	hLibNSight,	"nvtxRangePushA"	)	);
	TESTIFNULL(	nvtxRangePop_dyn	= (nvtxRangePop_Pfn)	GetProcAddress(	hLibNSight,	"nvtxRangePop"		)	);
	TESTIFNULL(	nvtxMarkA_dyn		= (nvtxMarkA_Pfn)		GetProcAddress(	hLibNSight,	"nvtxMarkA"			)	);

	g_nv_event_range.version				= NVTX_VERSION;
	g_nv_event_range.size					= NVTX_EVENT_ATTRIB_STRUCT_SIZE;
	g_nv_event_range.colorType				= NVTX_COLOR_ARGB;
	g_nv_event_range.color					= 0xff000000;
	g_nv_event_range.messageType			= NVTX_MESSAGE_TYPE_ASCII;

	g_nv_event_aaa_evt_range.version		= NVTX_VERSION;
	g_nv_event_aaa_evt_range.size			= NVTX_EVENT_ATTRIB_STRUCT_SIZE;
	g_nv_event_aaa_evt_range.colorType		= NVTX_COLOR_ARGB;
	g_nv_event_aaa_evt_range.color			= 0xff000000;
	g_nv_event_aaa_evt_range.messageType	= NVTX_MESSAGE_TYPE_ASCII;

	b_started_out = true;
	GOOD_PRINT_STRING( "NSight : Yes, Custom markers available !" );
}

/*
// To automatically tag a function... (see NSightEvents.h helper)
void OGLMeshWrapper::draw(COGLSceneHelper *pOGLSceneHelper, int primGroupStart, int primGroupEnd, bool bFakeImmediateMode)
{
	NXPROFILEFUNC(__FUNCTION__);
	...
}
*/
/*
void anotherExampleOfFunction()
{
	NXPROFILEFUNCCOL( __FUNCTION__, 0xFF0000 );
	for( INT32 i=0; i<1000; i++ )
	{
		NXPROFILEFUNC("Loop");
	}
}
*/

void nsight::deinit()
{
}

void nsight::update()
{
	b_active		= b_active_ui       && b_started_out;
	b_active_event	= b_active_event_ui && b_started_out;
}
#endif	//#if AAA_NSIGHT_USE()
