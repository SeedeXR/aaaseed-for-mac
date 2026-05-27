#include "aaa_mem.h"
#include "spy.h"
#include "err.h"
#include "cpu.h"
#include "thread/scheduler.h"

#if AAA_OS_MAC()
//	POSIX / Mach replacements for Win32 aligned-allocation, _msize and
//	GetProcessMemoryInfo. The Win32 code paths below are kept verbatim and
//	gated with AAA_OS_WINDOWS() ; every Mac branch lives under AAA_OS_MAC().
#	include <cstdlib>		//	posix_memalign, free
#	include <cstring>		//	memcpy
#	include <malloc/malloc.h>	//	malloc_size
#	include <mach/mach.h>
#	include <mach/task.h>
#	include <mach/task_info.h>
#endif //AAA_OS_MAC()

//	comment to get absolutely no mem check (max speed)
#define AAA_LOCAL_MENCHECK_ALLOW()	AAA_DEBUG()
#define _CRTDBG_MAP_ALLOC 0
#define	AAA_LEAK_DETECTOR() 0	//2023 Jan did not link in debug (so lib and dll have beeing removed from env)
	
bool	mem::b_verbose					= false;
UINT32	mem::verbose_size_min			= 0;
bool	mem::b_check_memory				= false;
bool	mem::b_check_heap				= false;
INT32	mem::counter_check_memory		= 0;
INT32	mem::counter_check_heap			= 0;

INT32	mem::memblock_nb				= 0;
INT32	mem::counter_malloc				= 0;
INT32	mem::counter_malloc_aligned		= 0;
INT32	mem::counter_free				= 0;
INT32	mem::counter_free_aligned		= 0;
INT32	mem::counter_realloc			= 0;
INT32	mem::counter_realloc_aligned	= 0;
INT32	mem::counter_alloca				= 0;
INT32	mem::counter_calloc				= 0;


//todo64 move to size_t : adapt param also
INT32	mem::working_set_size			= 0;
INT32	mem::working_set_size_peak		= 0;


#if AAA_LEAK_DETECTOR()
#	include <vld.h>
#	include <lib_use.h>
//todo this don't link ????
	AAA_LIB_USE( "vld" )
#endif	//AAA_LEAK_DETECTOR()

void	mem::report_leaks()
{
#if	AAA_LEAK_DETECTOR()
	VLDReportLeaks();
#endif
}
void	mem::enable_vld()
{
#if	AAA_LEAK_DETECTOR()
	VLDEnable();
#endif
}
void	mem::disable_vld()
{
#if	AAA_LEAK_DETECTOR()
	VLDDisable();
#endif
}
void	mem::mark_leaks_reported()
{
#if	AAA_LEAK_DETECTOR()
	VLDMarkAllLeaksAsReported();
#endif
}

void	mem::err_print( C_PCHAR_C fmt,	... )
{
	va_list args;
	va_start( args, fmt );

		BELL();
		BELL();

		BOX_TITLE_ERR_VA( "Memory Error",	fmt, args );
		debug_break(						fmt, args );

	va_end( args );
}

bool	mem::is_memory_ok( C_PCHAR_C str )
{
#if AAA_LOCAL_MENCHECK_ALLOW()
#ifdef	WIN32
#if	AAA_DEBUG()
	SPY_PUSH_RANGE( "_CrtCheckMemory", spy::MEM_LOW );
		auto CONST result = _CrtCheckMemory();
	SPY_POP_RANGE();
	if( !result )
	{
		debug_break( "_CrtCheckMemory() detected a problem in %s", str );
		return	false;
	}
	++counter_check_memory;
#endif
#endif
#endif	//AAA_LOCAL_MENCHECK_ALLOW
	return true;
}

bool	mem::is_heap_ok( C_PCHAR_C str )
{
#if AAA_LOCAL_MENCHECK_ALLOW()
#	ifdef	WIN32
		SPY_PUSH_RANGE( "_heapchk", spy::MEM_LOW );
			INT32 CONST	heapstatus = _heapchk();
		SPY_POP_RANGE();
		++counter_check_heap;
		if( heapstatus == _HEAPOK )
			return true;
		switch( heapstatus )
		{
		case _HEAPEMPTY:
			GOOD_PRINT_STRING( "%s() OK - heap is empty", str );
			return true;
		case _HEAPBADBEGIN:	ERR_PRINT_STRING( "%s() bad start of heap", str );					break;
		case _HEAPBADNODE:	ERR_PRINT_STRING( "%s() bad node in heap", str );					break;
		default:			ERR_PRINT_STRING( "%s() heap_check() detected a problem", str );	break;
		}
		return false;
#	else
		return true;
#	endif
#else
	return true;
#endif	//AAA_LOCAL_MENCHECK_ALLOW
}

static FINLINE void check_size( size_t CONST size, C_PCHAR_C signature )
{
	//if( 1116 <= size && size <= 1144 )
	//	debug_break( "mem::Debug Hook" );

	if( size==0 )
	{
		debug_break( "%s() with a ZERO size.", signature );
	}
	if( size >= 0xfffffff )
	{
		if( size >= 0x40000000 )
			ERR_PRINT_STRING( "in %s() Memory size could be too big here : %luMB", signature, size>>20 );
		else
			DBG_PRINT_STRING( "in %s() Memory size could be too big here : %luMB", signature, size>>20 );
	}
}

void*	mem::calloc( size_t CONST nb, size_t CONST size, C_PCHAR_C signature )
{
#if	AAA_DEBUG()
	if( size & 3 )
	{
		if( b_verbose )
		{
			if( size > verbose_size_min )
				VERBOSE_PRINT_STRING( "MEM CALLOC( %8d ) %s : will be done for %zu bytes", counter_calloc, signature, size * nb );
		}
	}
#endif
	check_size( size * nb, signature );
	DBG_HEAP_CHECK();
	++counter_calloc;
	SPY_PUSH_RANGE( "calloc", spy::MEM_LOW );
		void* CONST pt = ::calloc( nb, size );
	SPY_POP_RANGE();
	
	if( !pt )
		err_print( "can't calloc %zu bytes", size * nb );

	if( b_verbose )
	{
		if( size > verbose_size_min )
			VERBOSE_PRINT_STRING( "MEM CALLOC( %8d ) %zu bytes at %p", counter_calloc, size * nb, pt );
	}
	return pt;
}

void*	mem::malloc( size_t CONST size, C_PCHAR_C signature )
{
#if	AAA_DEBUG()
	if( size & 3 )
	{
		if( b_verbose )
		{
			if( size > verbose_size_min )
				VERBOSE_PRINT_STRING( "MEM MALLOC( %8d ) %s : will be done for %zu bytes", counter_malloc, signature, size );
			// VERBOSE_PRINTF( "MEM could be bad size");
			// BELL();
		}
	}
#endif
	check_size( size, signature );
	DBG_HEAP_CHECK();
	++counter_malloc;
	SPY_PUSH_RANGE2( signature, spy::MEM_LOW, "malloc" );
		void* CONST pt = ::malloc( size );
	SPY_POP_RANGE2();

	if( pt )
		++memblock_nb;
	else
		err_print( "MEM MALLOC( %8d ) %s : can't malloc %zu bytes", counter_malloc, signature, size );

	if( b_verbose )
	{
		if( size > verbose_size_min )
			VERBOSE_PRINT_STRING( "MEM MALLOC( %8d ) %s : %zu bytes at %p", counter_malloc, signature, size, pt );
	}
	return pt;
}


// but we need it for memory allocation

void*	mem::malloc_aligned( size_t CONST size, UINT32 alignment, C_PCHAR_C signature )
{
#if	AAA_DEBUG()
	if( size & 3 )
	{
		if( b_verbose )
		{
			if( size > verbose_size_min )
				VERBOSE_PRINT_STRING( "MEM MALLOC ALIGNED( %8d ) %s : will be done for %zu bytes", counter_malloc_aligned, signature, size );
			// VERBOSE_PRINTF( "MEM could be bad size");
			// BELL();
		}
	}
#endif
	if( alignment==0 )
		alignment = c_cpu::get_align_def();

	check_size( size, signature );
	DBG_HEAP_CHECK();
	++counter_malloc_aligned;
#if AAA_OS_WINDOWS()
	SPY_PUSH_RANGE2( signature, spy::MEM_LOW, "_aligned_malloc" );
		void* pt = _aligned_malloc( size, alignment );
	SPY_POP_RANGE2();
#elif AAA_OS_MAC()
	//	posix_memalign requires alignment to be a power of two AND a multiple
	//	of sizeof(void*). Round up to sizeof(void*) if needed ; the caller
	//	usually passes a SIMD alignment (16/32/64) which already qualifies.
	SPY_PUSH_RANGE2( signature, spy::MEM_LOW, "posix_memalign" );
		void* pt = nullptr;
		if( alignment < sizeof(void*) )
			alignment = (UINT32) sizeof(void*);
		if( posix_memalign( &pt, alignment, size ) != 0 )
			pt = nullptr;
	SPY_POP_RANGE2();
#endif //AAA_OS_WINDOWS / AAA_OS_MAC
	
	if( pt )
		++memblock_nb;
	else
		err_print( "MEM MALLOC ALIGNED( %8d ) %s : can't malloc %zu bytes with alignment of %u", counter_malloc_aligned, signature, size, alignment );

	if( b_verbose )
	{
		if( size > verbose_size_min )
			VERBOSE_PRINT_STRING( "MEM MALLOC ALIGNED( %8d ) %s : %zu bytes at %p", counter_malloc_aligned, signature, size, pt );
	}

	return pt;
}

void*	mem::realloc( void* memblock, size_t CONST size, C_PCHAR_C signature )
{
	if( size==0 )
	{
		FREE( memblock );
		return nullptr;
	}

	size_t	size_before;
	if( b_verbose )
#if AAA_OS_WINDOWS()
		size_before = memblock ? _msize(memblock) : 0;
#elif AAA_OS_MAC()
		size_before = memblock ? malloc_size(memblock) : 0;
#else
		size_before = 0;
#endif

	check_size( size, signature );
	DBG_HEAP_CHECK();
	++counter_realloc;
	void* pt;
	if( memblock )
	{
		SPY_PUSH_RANGE2( signature, spy::MEM_LOW, "realloc" );
		pt = ::realloc( memblock, size );
	}
	else
	{
		SPY_PUSH_RANGE2( signature, spy::MEM_LOW, "malloc" );
		pt = ::malloc( size );
	}
	SPY_POP_RANGE2();
	if( pt )
	{
		if( !memblock )
			++memblock_nb;
	}
	else
	{
		err_print( "MEM REALLOC( %8d ) %s : can't realloc to %zu bytes", counter_realloc, signature, size );
		// Non-standard: free the original block on failure instead of preserving it (std realloc keeps it).
		// Every caller in Src uses `x = REALLOC(x, size)` so they lose the pointer either way -- preserving would leak.
		if( memblock )
			mem::free( memblock, signature );
	}

	if( b_verbose )
	{
		if( size > verbose_size_min )
		{
			if( size_before )
				VERBOSE_PRINT_STRING( "MEM REALLOC( %8d ) %s : from %zu bytes to %zu at %p", counter_realloc, signature, size_before, size, pt );
			else
				VERBOSE_PRINT_STRING( "MEM REALLOC( %8d ) %s : first time %zu bytes at %p", counter_realloc, signature, size, pt );
		}
	}
	return pt;
}

void*	mem::realloc_aligned( void* memblock, size_t CONST size, UINT32 alignment, C_PCHAR_C signature )
{
	if( size==0 )
	{
		FREE_ALIGNED( memblock );
		return nullptr;
	}

	if( alignment==0 )
		alignment = c_cpu::get_align_def();

	//if( ( (UINT8)memblock ) & 0xf )
//	UINT32	memblock_align_bits = UINT32(memblock);
//	memblock_align_bits = memblock_align_bits & (alignment-1);
//	if( memblock_align_bits == 0 )
	check_size( size, signature );
	size_t	size_before;
	if( b_verbose )
#if AAA_OS_WINDOWS()
		size_before = memblock ? _aligned_msize( memblock, alignment, 0 ) : 0;
#elif AAA_OS_MAC()
		size_before = memblock ? malloc_size( memblock ) : 0;
#else
		size_before = 0;
#endif

	DBG_HEAP_CHECK();
	++counter_realloc_aligned;

	void* pt;
#if AAA_OS_WINDOWS()
	if( memblock )
	{
		SPY_PUSH_RANGE2( signature, spy::MEM_LOW, "_aligned_realloc" );
		pt = _aligned_realloc( memblock, size, alignment );
	}
	else
	{
		SPY_PUSH_RANGE2( signature, spy::MEM_LOW, "_aligned_malloc" );
		pt = _aligned_malloc( size, alignment );
	}
	SPY_POP_RANGE2();
#elif AAA_OS_MAC()
	//	No POSIX equivalent of _aligned_realloc : posix_memalign-allocated
	//	blocks must NOT be passed to realloc() because realloc() can return a
	//	pointer that is not aligned to the original alignment. We emulate
	//	with posix_memalign + memcpy + free of the old block.
	if( alignment < sizeof(void*) )
		alignment = (UINT32) sizeof(void*);
	if( memblock )
	{
		SPY_PUSH_RANGE2( signature, spy::MEM_LOW, "posix_memalign_realloc" );
			void* new_pt = nullptr;
			if( posix_memalign( &new_pt, alignment, size ) != 0 )
				new_pt = nullptr;
			if( new_pt )
			{
				size_t CONST old_size = malloc_size( memblock );
				size_t CONST cpy_size = (old_size < size) ? old_size : size;
				::memcpy( new_pt, memblock, cpy_size );
				::free( memblock );
			}
			pt = new_pt;
		SPY_POP_RANGE2();
	}
	else
	{
		SPY_PUSH_RANGE2( signature, spy::MEM_LOW, "posix_memalign" );
			void* new_pt = nullptr;
			if( posix_memalign( &new_pt, alignment, size ) != 0 )
				new_pt = nullptr;
			pt = new_pt;
		SPY_POP_RANGE2();
	}
#endif //AAA_OS_WINDOWS / AAA_OS_MAC
	if( pt )
	{
		if( !memblock )
			++memblock_nb;
	}
	else
	{
		err_print( "MEM REALLOC ALIGNED( %8d ) %s : can't realloc %zu bytes with alignment of %u", counter_realloc_aligned, signature, size, alignment );
		// Non-standard: free the original on failure (same rationale as mem::realloc above).
		if( memblock )
			mem::free_aligned( memblock, signature );
	}

	if( b_verbose )
	{
		if( size > verbose_size_min )
		{
			if( size_before )
				VERBOSE_PRINT_STRING( "MEM REALLOC ALIGNED( %8d ) %s : from %zu bytes to %zu at %p", counter_realloc_aligned, signature, size_before, size, pt );
			else
				VERBOSE_PRINT_STRING( "MEM REALLOC ALIGNED( %8d ) %s : first time %zu bytes at %p", counter_realloc_aligned, signature, size, pt );
		}
	}
	return pt;
}

void	mem::free( void* CONST memblock, C_PCHAR_C signature )
{
	if( b_verbose )
	{
		if( memblock )
		{
#if AAA_OS_WINDOWS()
			auto size = _msize( memblock );
#elif AAA_OS_MAC()
			auto size = malloc_size( memblock );
#else
			size_t size = 0;
#endif
			if( size > verbose_size_min )
				VERBOSE_PRINT_STRING( "MEM FREE( %8d ) %s : %zu bytes at %p", counter_free, signature, size, memblock );
		}
		else
			VERBOSE_PRINT_STRING( "MEM FREE( %8d ) %s : FREE of NULL memblock", counter_free, signature );
	}
	if( memblock )
	{
		DBG_HEAP_CHECK();
		++counter_free;
		SPY_PUSH_RANGE2( signature, spy::MEM_LOW, "free" );
			::free( memblock );
		SPY_POP_RANGE2();
		--memblock_nb;
	}
}

void	mem::free_aligned( void* CONST memblock, C_PCHAR_C signature )
{
	if( b_verbose )
	{
		if( memblock )
			VERBOSE_PRINT_STRING( "MEM FREE ALIGNED ( %8d ) %s : at %p", counter_free_aligned, signature, memblock );
		else
			VERBOSE_PRINT_STRING( "MEM FREE ALIGNED ( %8d ) %s : FREE of NULL memblock", counter_free_aligned, signature );
	}
	if( memblock )
	{
		DBG_HEAP_CHECK();
		++counter_free_aligned;
		SPY_PUSH_RANGE2( signature, spy::MEM_LOW, "free_aligned" );
#if AAA_OS_WINDOWS()
			_aligned_free( memblock );
#elif AAA_OS_MAC()
			//	POSIX free handles posix_memalign-allocated blocks.
			::free( memblock );
#endif //AAA_OS_WINDOWS / AAA_OS_MAC
		SPY_POP_RANGE2();
		--memblock_nb;
	}
}

#ifdef	WIN32
#	include <psapi.h>
namespace{
	PROCESS_MEMORY_COUNTERS pmc;
}
#endif

void	mem::update_info()
{
	SPY_PUSH_RANGE( "update_info", spy::MEM );
#ifdef	WIN32
//	HANDLE hProcess;
	// Print the process identifier.
	// printf( "\nProcess ID: %u\n", processID );

	// Print information about the memory usage of the process.

	// hProcess = OpenProcess(  PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID );
	//if( !hProcess )
	//	return;


#ifdef	WIN32
	if( GetProcessMemoryInfo( get_process_cur(), &pmc, sizeof(pmc)) )
	{
		//todox64 deal with the detail : overflow ?
		working_set_size		= (INT32) (pmc.WorkingSetSize >> 20);
		working_set_size_peak	= (INT32) (pmc.PeakWorkingSetSize >> 20);
//todo use this info
/*
		printf( "\tPageFaultCount: 0x%08X\n", pmc.PageFaultCount );
		printf( "\tPeakWorkingSetSize: 0x%08X\n", pmc.PeakWorkingSetSize );
		printf( "\tWorkingSetSize: 0x%08X\n", pmc.WorkingSetSize );
		printf( "\tQuotaPeakPagedPoolUsage: 0x%08X\n", pmc.QuotaPeakPagedPoolUsage );
		printf( "\tQuotaPagedPoolUsage: 0x%08X\n",pmc.QuotaPagedPoolUsage );
		printf( "\tQuotaPeakNonPagedPoolUsage: 0x%08X\n", pmc.QuotaPeakNonPagedPoolUsage );
		printf( "\tQuotaNonPagedPoolUsage: 0x%08X\n", pmc.QuotaNonPagedPoolUsage );
		printf( "\tPagefileUsage: 0x%08X\n", pmc.PagefileUsage ); 
		printf( "\tPeakPagefileUsage: 0x%08X\n", pmc.PeakPagefileUsage );
*/
	}
#endif

//	CloseHandle( hProcess );
#endif

#if AAA_OS_MAC()
	//	Mach equivalent of Win32 GetProcessMemoryInfo : task_basic_info exposes
	//	resident_size (the process's current resident set, akin to WorkingSetSize).
	//	macOS does not expose a "peak working set" through task_basic_info ; we
	//	track an in-process high-water mark instead.
	mach_task_basic_info_data_t info;
	mach_msg_type_number_t count = MACH_TASK_BASIC_INFO_COUNT;
	if( task_info( mach_task_self(), MACH_TASK_BASIC_INFO,
				   reinterpret_cast<task_info_t>(&info), &count ) == KERN_SUCCESS )
	{
		INT32 CONST rss_mb = (INT32) (info.resident_size >> 20);
		working_set_size = rss_mb;
		if( rss_mb > working_set_size_peak )
			working_set_size_peak = rss_mb;
	}
#endif //AAA_OS_MAC()

	SPY_POP_RANGE();
}

void	mem::update()
{
	b_cpy_use_movsb			= b_cpy_use_movsb_asked;
	b_cpy_use_sse41			= b_cpy_use_sse41_asked && c_cpu::one->is_use_SSE4_1();
#if AAA_USE_AVX2()
	b_cpy_use_avx2			= b_cpy_use_avx2_asked && c_cpu::one->is_use_AVX2();
#else
	b_cpy_use_avx2			= false;
#endif //AAA_USE_AVX2()
	b_memset_use_avx		= b_memset_use_avx_asked && c_cpu::one->is_use_AVX();
	cpy_use_cache_L2_size	= b_cpy_use_cache_L2_size_ui ? c_cpu::one->get_cache_L2_size() : 0;
	cpy_use_cache_L3_size	= b_cpy_use_cache_L3_size_ui ? c_cpu::one->get_cache_L3_size() : 1024 * 1024 * 1024;
}

void	mem::set_verbose( bool CONST in )
{
	b_verbose = in;
	SWITCH_PRINT_STATE( "Memory Verbose", b_verbose );
}

void	mem::flip_verbose()
{
	set_verbose( !b_verbose );
}

void	mem::counter_reset()
{
	counter_check_memory	= 0;
	counter_check_heap		= 0;

	memblock_nb				= 0;
	counter_realloc			= 0;
	counter_realloc_aligned	= 0;
	counter_free			= 0;
	counter_free_aligned	= 0;
	counter_malloc			= 0;
	counter_malloc_aligned	= 0;
	counter_alloca			= 0;
	counter_calloc			= 0;

/*
	if( _b_verbose )
	{
		VERBOSE_PRINTF( "MEM counter reset");
	}
*/
}

bool mem::is_pointer_fucked( CP_CVOID pt )
{
#if AAA_OS_WINDOWS()
	// this is linked to Microsoft Visual Studio
#	if AAA_BIT64()
//	UINT64 ui64 = reinterpret_cast<UINT64>(pt);
	UINT32 ui32		= reinterpret_cast<UINT64>(pt) & 0xffffffff;
	//	if( i64 == 0xcdcdcdcdcdcdcdcd )
	UINT32 ui32b	= (reinterpret_cast<UINT64>(pt)>>32) & 0xffffffff;
	if( ui32 == 0xcdcdcdcd || ui32b == 0xcdcdcdcd )
#	else
	if( (UINT32)pt == 0xcdcdcdcd )
#	endif //#if AAA_BIT64()
	{
		debug_break( "uninitialized pointer" );
		return true;
	}
#	if AAA_BIT64()
	//	if( (UINT64)pt == 0xabababababababab || (UINT64)pt == 0xfeeefeeefeeefeee )
	if( ui32 == 0xabababab || ui32b == 0xabababab || ui32 == 0xfeeefeee || ui32b == 0xfeeefeee )
#	else
	if( (UINT32)pt == 0xabababab || (UINT32)pt == 0xfeeefeee )
#	endif //#if AAA_BIT64()
	{
		debug_break( "pointer is fucked up" );
		return true;
	}
#endif //#if AAA_OS_WINDOWS()

	if( reinterpret_cast<uintptr_t>(pt) < 0x1000 )
	{
		debug_break( "no way pointer can be so low" );
		return true;
	}
	return	false;
}