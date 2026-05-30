
#ifdef AAA_NSIGHTEVENTS_H
#error "NSIGHTEVENTS_H included more than once."
#endif
#define AAA_NSIGHTEVENTS_H 1


#ifndef AAA_NSIGHT_USE
#	define	AAA_NSIGHT_USE()	1
#endif

//-----------------------------------------------------------------------------
// NSIGHT
//-----------------------------------------------------------------------------

#ifndef	AAA_AAA_STR_H
#	include "aaa_str.h"
#endif

#if AAA_NSIGHT_USE()

#ifndef NVTOOLSEXT_H_
// NSight perf markers - Visual studio project have a link to something like C:\Program Files\NVIDIA GPU Computing Toolkit\CUDA\v13.1\include
#	include "nvtx3/nvToolsExt.h"
#endif

namespace nsight
{
	extern	bool	b_active;
	extern	bool	b_active_event;

	extern	void	init();
	extern	void	deinit();
	extern	void	update();
	
	// If you want to load dynamically the entry points...
	typedef int (__stdcall *nvtxRangePushEx_Pfn)	(const nvtxEventAttributes_t* eventAttrib);
	typedef int (__stdcall *nvtxRangePush_Pfn)		(const char* message);
	typedef int (__stdcall *nvtxRangePop_Pfn)		();
	typedef int (__stdcall *nvtxMarkA_Pfn)			(const char* message);
	
	extern nvtxRangePushEx_Pfn		nvtxRangePushEx_dyn;
	extern nvtxRangePush_Pfn		nvtxRangePush_dyn;
	extern nvtxRangePop_Pfn			nvtxRangePop_dyn;
	extern nvtxMarkA_Pfn			nvtxMarkA_dyn;

	extern nvtxEventAttributes_t	g_nv_event_range;	
	extern nvtxEventAttributes_t	g_nv_event_aaa_evt_range;

	struct NXProfileFunc
	{
		NXProfileFunc(const char *name, uint32_t c, /*int64_t*/uint32_t p = 0 )
		{
			if( nvtxRangePushEx_dyn )
			{
				nvtxEventAttributes_t eventAttrib = {0};
				// set the version and the size information
				eventAttrib.version			= NVTX_VERSION;
				eventAttrib.size			= NVTX_EVENT_ATTRIB_STRUCT_SIZE;
				// configure the attributes.  0 is the default for all attributes.
				eventAttrib.colorType		= NVTX_COLOR_ARGB;
				eventAttrib.color			= c;
				eventAttrib.messageType		= NVTX_MESSAGE_TYPE_ASCII;
				eventAttrib.message.ascii	= name;
				eventAttrib.payloadType		= NVTX_PAYLOAD_TYPE_INT64;
				eventAttrib.payload.llValue	= (int64_t)p;
				eventAttrib.category		= (uint32_t)p;
				nvtxRangePushEx_dyn( &eventAttrib );
			}
		}
		~NXProfileFunc()
		{
			if( nvtxRangePop_dyn )
			{
				nvtxRangePop_dyn();
			}
		}
	};

#	ifdef NXPROFILEFUNC
#		undef NXPROFILEFUNC
#		undef NXPROFILEFUNCCOL
#		undef NXPROFILEFUNCCOL2
#	endif

#	define NXPROFILEFUNC(		name )			NXProfileFunc nsight::nxProfileMe( name, 0xFF0000FF )
#	define NXPROFILEFUNCCOL(	name, c )		NXProfileFunc nsight::nxProfileMe( name, c )
#	define NXPROFILEFUNCCOL2(	name, c, p )	NXProfileFunc nsight::nxProfileMe( name, c, p )
}
#else //AAA_NSIGHT_USE()
//	-----------------------------------------------------------------------
//	Apple Silicon backend : NVIDIA NSight (NVTX) is x86 + NVIDIA SDK only.
//	The macOS / iOS equivalent for CPU-side scope markers is os_signpost
//	from <os/signpost.h>. Signposts surface in Instruments and Xcode's Time
//	Profiler with the same "interval begin / end" semantics NVTX provides,
//	so callers that wrap hot scopes with NXPROFILEFUNC get real profiling
//	visibility on Mac too.
//
//	GPU-side markers ([encoder pushDebugGroup:]) live in MTLCommandEncoder
//	and will land once the Metal backend is in place. They are orthogonal
//	to these CPU signposts.
//	-----------------------------------------------------------------------
#include <os/signpost.h>
#include <cstdint>

namespace nsight
{
	//	Lazy, single shared os_log instance. Function-local static in an
	//	inline function is required (C++17) to behave as one object across
	//	all TUs that include this header.
	inline os_log_t & get_log()
	{
		static os_log_t log = os_log_create( "com.seedexr.aaaseed", "performance" );
		return log;
	}

	//	Public API matching the Windows side so callers don't need to gate.
	inline void init()		{ (void) get_log(); }
	inline void deinit()	{}
	inline void update()	{}

	//	RAII scope marker. Construction issues an interval-begin signpost ;
	//	destruction issues the matching end. Visible in Instruments under
	//	"os_signpost" with subsystem com.seedexr.aaaseed / category performance.
	struct NXProfileFunc
	{
		os_signpost_id_t	_spid;
		char const*			_name;

		NXProfileFunc( char const* name, std::uint32_t /*color*/ = 0, std::uint32_t /*payload*/ = 0 )
			: _spid( os_signpost_id_generate( get_log() ) )
			, _name( name )
		{
			os_signpost_interval_begin( get_log(), _spid, "scope", "%s", name );
		}
		~NXProfileFunc()
		{
			os_signpost_interval_end( get_log(), _spid, "scope", "%s", _name );
		}

		NXProfileFunc( NXProfileFunc const& ) = delete;
		NXProfileFunc& operator=( NXProfileFunc const& ) = delete;
	};
}

//	Token-paste helpers to give each NXPROFILEFUNC instance a unique
//	variable name (one signpost per source line ; multiple per line would
//	require __COUNTER__ but the engine never stacks two on one line).
#define	NXPROFILEFUNC_PASTE2(a, b)		a##b
#define	NXPROFILEFUNC_PASTE(a, b)		NXPROFILEFUNC_PASTE2(a, b)
#define	NXPROFILEFUNC(		name )			::nsight::NXProfileFunc NXPROFILEFUNC_PASTE( _nx_, __LINE__ )( name )
#define	NXPROFILEFUNCCOL(	name, c )		::nsight::NXProfileFunc NXPROFILEFUNC_PASTE( _nx_, __LINE__ )( name, c )
#define	NXPROFILEFUNCCOL2(	name, c, p )	::nsight::NXProfileFunc NXPROFILEFUNC_PASTE( _nx_, __LINE__ )( name, c, p )
#endif //AAA_NSIGHT_USE()
