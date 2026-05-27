#include "spy.h"
#include "err.h"
#include "time_buf_master.h"
#include "time_buf.h"
#ifndef _THREAD_
#	include <thread>
#endif

namespace spy
{
	bool				b_print = false;
	bool				b_print_frame_trig_ui = false;
	INT32				stack_count = 0;

#if AAA_NSIGHT_USE()

#	define AAA_SPY_FAST()	1

	FINLINE	void inc_stack_count()		{ ++stack_count; }
	FINLINE	void dec_stack_count()		{ --stack_count; }
	FINLINE	void dec_stack_count_2()	{ stack_count -= 2; }
#endif //#if AAA_NSIGHT_USE()
}

#if AAA_NSIGHT_USE()
FINLINE void SPY_PUSH_RANGE_DIRECT(	C_PCHAR_C str, UINT32 u32_color )
{	
	// don't need SPY_IS_ACTIVE() should have being called before calling this 
	//if( nsight::nvtxRangePushEx_dyn )
	//{
#if AAA_SPY_FAST()
		nsight::g_nv_event_range.color			= u32_color;						
		nsight::g_nv_event_range.message.ascii	= str;					
		nsight::nvtxRangePushEx_dyn( &nsight::g_nv_event_range );
#else
		nvtxEventAttributes_t nv_event_range	= {0};	
		nv_event_range.color			= u32_color;						
		nv_event_range.message.ascii	= str;
		nv_event_range.version			= NVTX_VERSION;					
		nv_event_range.size				= NVTX_EVENT_ATTRIB_STRUCT_SIZE;	
		nv_event_range.colorType		= NVTX_COLOR_ARGB;								
		nv_event_range.messageType		= NVTX_MESSAGE_TYPE_ASCII;	
		nsight::nvtxRangePushEx_dyn( &nv_event_range );
#endif	//AAA_SPY_FAST()
		spy::inc_stack_count();
		if( spy::b_print )
			DBG_PRINT_STRING( "%d %s", spy::stack_count, str );
	//}
	//else
	//	debug_break( "SPY_PUSH_RANGE_DIRECT() nvtxRangePushEx_dyn is NULL !!!, message is \"%s\"", str );
}
//FINLINE void SPY_PUSH_RANGE_DIRECT( o_str CONST & o, UINT32 u32_color )
//{
//	SPY_PUSH_RANGE_DIRECT( o.get(), u32_color );
//}
FINLINE void SPY_PUSH_EVENT_RANGE_DIRECT(	C_PCHAR_C str, UINT32 u32_color )
{		
	// don't need SPY_IS_ACTIVE() should have being called before calling this 
	// if( nsight::nvtxRangePushEx_dyn )
	//{
#if AAA_SPY_FAST()
		nsight::g_nv_event_aaa_evt_range.color			= u32_color;						
		nsight::g_nv_event_aaa_evt_range.message.ascii	= str;					
		nsight::nvtxRangePushEx_dyn( &nsight::g_nv_event_aaa_evt_range );
#else
		nvtxEventAttributes_t nv_event_range	= {0};	
		nv_event_range.color			= u32_color;						
		nv_event_range.message.ascii	= str;
		nv_event_range.version			= NVTX_VERSION;					
		nv_event_range.size				= NVTX_EVENT_ATTRIB_STRUCT_SIZE;	
		nv_event_range.colorType		= NVTX_COLOR_ARGB;								
		nv_event_range.messageType		= NVTX_MESSAGE_TYPE_ASCII;	
		nsight::nvtxRangePushEx_dyn( &nv_event_range );
#endif	//#if AAA_SPY_FAST()
		spy::inc_stack_count();
		if( spy::b_print )
			DBG_PRINT_STRING( "%d %s", spy::stack_count, str );
	//}
	//else
	//	debug_break( "SPY_PUSH_RANGE_DIRECT() nvtxRangePushEx_dyn is NULL !!!, message is \"%s\"", str );
}
FINLINE void SPY_POP_RANGE_DIRECT()
{	
	// don't need SPY_IS_ACTIVE() should have being called before calling this 
	// if( nsight::nvtxRangePop_dyn )
	//{
		nsight::nvtxRangePop_dyn();
		spy::dec_stack_count();
		if( spy::b_print )
			DBG_PRINT_STRING( "%d", spy::stack_count );
	//}
	//else
	//	debug_break( "SPY_POP_RANGE_DIRECT() nvtxRangePop_dyn is NULL !!!" );
}
FINLINE void SPY_POP_RANGE2_DIRECT()
{
	// don't need SPY_IS_ACTIVE() should have being called before calling this
	// if( nsight::nvtxRangePop_dyn )
	//{
		nsight::nvtxRangePop_dyn();
		nsight::nvtxRangePop_dyn();
		spy::dec_stack_count_2();
		if( spy::b_print )
			DBG_PRINT_STRING( "%d", spy::stack_count );
	//}
	//else
	//	debug_break( "SPY_POP_RANGE2_DIRECT() nvtxRangePop_dyn is NULL !!!" );
}
FINLINE void SPY_MARK_DIRECT( C_PCHAR_C str )
{	
	// don't need SPY_IS_ACTIVE() should have being called before calling this 
	// if( nsight::nvtxMarkA_dyn )
		nsight::nvtxMarkA_dyn( str );
	//else
	//	debug_break( "SPY_POP_RANGE_DIRECT() nvtxMarkA_dyn is NULL !!!, mark is \"%s\"", str );
}
//FINLINE void SPY_MARK_DIRECT( o_str CONST & o )
//{
//	SPY_MARK_DIRECT( o.get() );
//}
#endif //#if AAA_NSIGHT_USE()

namespace spy
{
	void	init()
	{
		tbuf::init();
#if AAA_NSIGHT_USE()
		nsight::init();
#endif
	}
	void	deinit()
	{
#if AAA_NSIGHT_USE()
		nsight::deinit();
#endif
		tbuf::deinit();
	}
	void	update()
	{
		if( b_print_frame_trig_ui )
		{
			if( b_print )
			{
				b_print = false;
				b_print_frame_trig_ui = false;
				DBG_PRINT_STRING( "SPY NEW frame : End print." );
			}
			else
			{
				b_print = true;
				DBG_PRINT_STRING( "SPY NEW frame : Begin print." );
			}
		}
		else
		{
			if( b_print )
			{
				DBG_PRINT_STRING( "SPY NEW frame." );
			}
		}
#if AAA_NSIGHT_USE()
		nsight::update();
#endif
		tbuf::update();
	}

	void	yield( C_PCHAR_C text )
	{
		TBUF_ADD( tbuf::CH_SLEEP, 1, text );
		SPY_EVENT_PUSH_RANGE( text, spy::SLEEP );
			std::this_thread::yield();
		SPY_EVENT_POP_RANGE();
		TBUF_ADD( tbuf::CH_SLEEP, 0., nullptr );
	}

	void	sleep( INT32 CONST second_milli, C_PCHAR_C text )
	{
		TBUF_ADD( tbuf::CH_SLEEP, 1, text );
		SPY_EVENT_PUSH_RANGE( text, spy::SLEEP );
			//Sleep( second_milli );
			// Romain Cheminade say : deal better with the os, handle different case, and so avoid wait for event on window
			std::this_thread::sleep_for( std::chrono::milliseconds(second_milli) );
		SPY_EVENT_POP_RANGE();
		TBUF_ADD( tbuf::CH_SLEEP, 0., nullptr );
	}

	void	sleep_micro( INT32 CONST second_micro, C_PCHAR_C text )
	{
		TBUF_ADD( tbuf::CH_SLEEP, 1, text );
		SPY_EVENT_PUSH_RANGE( text, spy::SLEEP );
			// Romain Cheminade say : deal better with the os, handle different case, and so avoid wait for event on window
			std::this_thread::sleep_for( std::chrono::microseconds(second_micro) );
		SPY_EVENT_POP_RANGE();
		TBUF_ADD( tbuf::CH_SLEEP, 0., nullptr );
	}

	void	sleep_ex( INT32 CONST second_milli, bool CONST b_alertable, C_PCHAR_C text )
	{
		TBUF_ADD( tbuf::CH_SLEEP, 1, text );
		SPY_EVENT_PUSH_RANGE( text, spy::SLEEP );
#if AAA_OS_WINDOWS()
			SleepEx( second_milli, b_alertable ? TRUE : FALSE );
#else
			//	Mac / Linux : no SleepEx equivalent. b_alertable's Windows
			//	semantics (early-return on queued APC) have no portable
			//	analogue. Fall back to a plain sleep -- callers that need
			//	signal-interruptible waits use std::condition_variable
			//	or pthread primitives on this platform.
			std::this_thread::sleep_for( std::chrono::milliseconds( second_milli ) );
			(void)b_alertable;
#endif
		SPY_EVENT_POP_RANGE();
		TBUF_ADD( tbuf::CH_SLEEP, 0., nullptr );
	}

#if AAA_OS_WINDOWS()
	DWORD	wait_for_single_object( HANDLE CONST hd, INT32 CONST second_milli, C_PCHAR_C text )
	{
		TBUF_ADD( tbuf::CH_SLEEP, 1, text );
		SPY_EVENT_PUSH_RANGE( text, spy::OS );
			DWORD ret = WaitForSingleObject( hd, second_milli );
		SPY_EVENT_POP_RANGE();
		TBUF_ADD( tbuf::CH_SLEEP, 0., nullptr );
		return ret;
	}
#endif
};

