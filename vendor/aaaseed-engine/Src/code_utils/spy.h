
#ifdef AAA_SPY_H
#error "SPY_H included more than once."
#endif
#define AAA_SPY_H 1


#ifndef AAA_NSIGHTEVENTS_H
#	include "draw/NSightEvents.h"
#endif

#if 0
#	ifndef AAA_TIME_BUF_H
#		include "time_buf.h"
#	endif
#else
#	ifdef TBUF_ADD_OBJ
#		undef TBUF_ADD_OBJ
#		undef TBUF_ADD
#		undef TBUF_INC_OBJ
#		undef TBUF_INC
#		undef TBUF_DEC_OBJ
#		undef TBUF_DEC
#	endif
#	define TBUF_ADD_OBJ(	channel_id, val, text,	obj )	{}
#	define TBUF_ADD(		channel_id, val, text	)		{}
#	define TBUF_INC_OBJ(	channel_id, val, text,	obj )	{}
#	define TBUF_INC(		channel_id, val, text	)		{}
#	define TBUF_DEC_OBJ(	channel_id, val, text,	obj )	{}
#	define TBUF_DEC(		channel_id, val, text	)		{}
#endif

namespace spy
{
	extern bool		b_print;
	extern bool		b_print_frame_trig_ui;
	extern INT32	stack_count;


	extern	void	init();
	extern	void	deinit();
	extern	void	update();

	extern	void	yield(																					C_PCHAR_C text = nullptr );
	extern	void	sleep(									 INT32 CONST second_milli,							C_PCHAR_C text = nullptr );
	extern	void	sleep_micro(							 INT32 CONST second_micro,							C_PCHAR_C text = nullptr );
	extern	void	sleep_ex(								 INT32 CONST second_milli, bool CONST b_alertable,	C_PCHAR_C text = nullptr );
#if AAA_OS_WINDOWS()
	//	Win32-only : exposes WaitForSingleObject which takes HANDLE + DWORD.
	//	On Mac / Linux callers use std::condition_variable / pthread instead.
	extern	DWORD	wait_for_single_object( HANDLE CONST hd, INT32 CONST second_milli,							C_PCHAR_C text );
#endif

	enum SPY_COLOR : UINT32
	{
		MEM_LOW		=	0xff00c0c0,
		MEM			=	0xff008080,
		MEM_HIGH	=	0xff004040,
		
		FILE_LOW	=	0xffc0c000,
		FILE		=	0xff808000,
		FILE_HIGH	=	0xff404000,
		
		INFRA_LOW	=	0xff00ffff,
		INFRA		=	0xff00a0a0,
		SLEEP		=	0xff00c0c0,
		OS			=	0xff00ffff,

		NET			=	0xff00ffff,
		SYNC		=	0xff00c0c0,

		COL_1		=	0xffff0000,
		COL_2		=	0xffffff00,
		COL_3		=	0xff00ff00,
		COL_4		=	0xff00ffff,
		COL_5		=	0xff0000ff,
		COL_6		=	0xffff00ff,
		
		GOL_LOW		=	0xffff8000,
		GOL			=	0xffc06000,
		GOL_HIGH	=	0xff804000,
		OPENCL		=	0xffc06080,

		TEX_LOW		=	0xffff0000,
		IMG_LOW		=	0xffff0000,
		IMG			=	0xffc00000,
		
		LAYER		=	0xffff8000,
		LAYERS		=	0xffa05000,
		RENDER		=	0xff402000,

		UPDATE		=	0xffc000c0,
		UPDATE_2	=	0xff900090,
		UPDATE_3	=	0xff600060,
		DRAW		=	0xffff0090,
		DRAW_2		=	0xffc00060,
		DRAW_3		=	0xff800030,
		LUA			=	0xff800080,
		
		CALLBACK_FN	=	0xffc0c0c0,
		
		UI			=	0xff008080,
	};
}


#if AAA_NSIGHT_USE()

	extern FINLINE void SPY_PUSH_RANGE_DIRECT(			C_PCHAR_C str, UINT32 u32_color );
	extern FINLINE void SPY_PUSH_EVENT_RANGE_DIRECT(	C_PCHAR_C str, UINT32 u32_color );	//todo seems to be identical to SPY_PUSH_RANGE_DIRECT and should be removed 
	extern FINLINE void SPY_POP_RANGE_DIRECT();
	extern FINLINE void SPY_POP_RANGE2_DIRECT();
	extern FINLINE void SPY_MARK_DIRECT(				C_PCHAR_C str );

	FINLINE void SPY_PUSH_RANGE_DIRECT( o_str CONST & o, UINT32 u32_color )	{ SPY_PUSH_RANGE_DIRECT( o.get(), u32_color ); }
	FINLINE void SPY_MARK_DIRECT(		o_str CONST & o )					{ SPY_MARK_DIRECT( o.get() ); }

#	define SPY_IS_ACTIVE()			(nsight::b_active)
#	define SPY_EVENT_IS_ACTIVE()	(nsight::b_active_event)

#	define SPY_PUSH_RANGE(			str,	color )			{ if( SPY_IS_ACTIVE() )			{ SPY_PUSH_RANGE_DIRECT( str,  color );														} }
#	define SPY_PUSH_RANGE2(			str1,	color, str2 )	{ if( SPY_IS_ACTIVE() )			{ SPY_PUSH_RANGE_DIRECT( str1, color );	SPY_PUSH_RANGE_DIRECT( str2, color );				} }
#	define SPY_PUSH_RANGE_FUNCTION(			color )			{ if( SPY_IS_ACTIVE() )			{ SPY_PUSH_RANGE_DIRECT( __FUNCTION__, color );												} }
#	define SPY_PUSH_RANGE_OBJ(		str1,	color )			{ if( SPY_IS_ACTIVE() )			{ SPY_PUSH_RANGE_DIRECT( str1, color );	SPY_PUSH_RANGE_DIRECT( get_name_search(), color );	} }
#	define SPY_POP_RANGE()									{ if( SPY_IS_ACTIVE() )			{ SPY_POP_RANGE_DIRECT();																	} }
#	define SPY_POP_RANGE2()									{ if( SPY_IS_ACTIVE() )			{ SPY_POP_RANGE2_DIRECT();																	} }
															  								  
#	define SPY_EVENT_PUSH_RANGE(	str,	color )			{ if( SPY_EVENT_IS_ACTIVE() )	{ SPY_PUSH_EVENT_RANGE_DIRECT( str, color );	} }	
#	define SPY_EVENT_POP_RANGE()							{ if( SPY_EVENT_IS_ACTIVE() )	{ SPY_POP_RANGE_DIRECT();						} }
															  								  
#	define SPY_MARK(				str )					{ if( SPY_IS_ACTIVE() )			{ SPY_MARK_DIRECT( str ); } }

#else	//#if AAA_NSIGHT_USE()

#	define SPY_IS_ACTIVE()			false

#	define SPY_PUSH_RANGE_DIRECT(	str,	color )			{}
#	define SPY_POP_RANGE_DIRECT()							{}

#	define SPY_PUSH_RANGE(			str,	color )			{}
#	define SPY_PUSH_RANGE2(			str1,	color, str2 )	{}
#	define SPY_PUSH_RANGE_FUNCTION(			color )			{}
#	define SPY_PUSH_RANGE_OBJ(		str1,	color	)		{}
#	define SPY_POP_RANGE()									{}
#	define SPY_POP_RANGE2()		

#	define SPY_EVENT_PUSH_RANGE(	str,	color )			{}
#	define SPY_EVENT_POP_RANGE()							{}

#	define SPY_MARK_DIRECT(			str	)					{}
#	define SPY_MARK(				str )					{}

#endif	//#if AAA_NSIGHT_USE()

