
#ifdef AAA_AAA_MEM_H
#error "AAA_MEM_H included more than once."
#endif
#define AAA_AAA_MEM_H 1


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif

class mem final
{
friend	class c_pref;
friend	class c_gol_ui;
private:
			static	bool	b_cpy_use_movsb;
			static	bool	b_cpy_use_avx2;
			static	bool	b_cpy_use_sse41;
			static	size_t	cpy_use_cache_L2_size;
			static	size_t	cpy_use_cache_L3_size;
public:
			static	bool	b_check_memory;
			static	bool	b_check_heap;
			static	bool	b_verbose;
			static	UINT32	verbose_size_min;
			static	bool	b_cpy_use_movsb_asked;
			static	bool	b_cpy_use_avx2_asked;
			static	bool	b_cpy_use_sse41_asked;
			static	bool	b_cpy_use_cache_L2_size_ui;
			static	bool	b_cpy_use_cache_L3_size_ui;
			static	bool	b_cpy_parallel;
			static	UINT32	cpy_parallel_thread_nb;
			static	UINT32	cpy_parallel_threshold_kb;		// min Kbytes before using parallel copy
			static	bool	b_memset_use_avx_asked;
			static	bool	b_memset_use_avx;
private:
			static	INT32	memblock_nb;
			static	INT32	counter_malloc;
			static	INT32	counter_malloc_aligned;
			static	INT32	counter_free;
			static	INT32	counter_free_aligned;
			static	INT32	counter_realloc;
			static	INT32	counter_realloc_aligned;
			static	INT32	counter_alloca;
			static	INT32	counter_calloc;
			static	INT32	counter_check_memory;
			static	INT32	counter_check_heap;

			static	INT32	working_set_size;
			static	INT32	working_set_size_peak;

			static	bool	is_memory_ok(		C_PCHAR_C str );
			static	bool	is_heap_ok(			C_PCHAR_C str );
			static	void	err_print(			C_PCHAR_C fmt,	... );

			static	void	memcpy_monothread(	void* CONST dst,	 void CONST * CONST src,	size_t nb	)												NOEXCEPT;
			static	void	memcpy_parallel(	void* CONST dst,	 void CONST * CONST src,	size_t nb,	INT32 CONST thread_nb,	C_PCHAR_C signature )	NOEXCEPT;

public:
	FINLINE	static	bool	is_verbose()						{	return b_verbose;			 }
			static	void	set_verbose(		bool CONST in );
			static	void	flip_verbose();
	FINLINE	static	UINT32	get_verbose_size_min()				{	return verbose_size_min;	}

	FINLINE	static	bool	is_check_heap()						{	return b_check_heap;		}
			static	void	set_check_heap(		bool CONST in )	{	b_check_heap = in;			}

	FINLINE	static	bool	is_check_memory()					{	return b_check_memory;		}
			static	void	set_check_memory(	bool CONST in )	{	b_check_memory = in;		}

	FINLINE	static	void	check( C_PCHAR_C str )
							{
								if( is_check_memory() )
									is_memory_ok( str );
								if( is_check_heap() )
									is_heap_ok( str );
							}
	FINLINE	static	bool	is_all_ok( C_PCHAR_C str )			{	return is_memory_ok(str) && is_heap_ok(str);	}
		
			static	void*	calloc(				size_t CONST nb,		size_t CONST size,							C_PCHAR_C signature	);
		
			static	void*	malloc(										size_t CONST size,							C_PCHAR_C signature	);
			static	void*	malloc_aligned(								size_t CONST size,	UINT32 alignment,		C_PCHAR_C signature	);
			static	void	free(				void* CONST memblock,												C_PCHAR_C signature	);
			static	void	free_aligned(		void* CONST memblock,												C_PCHAR_C signature	);
			static	void*	realloc(			void* memblock,			size_t CONST size,							C_PCHAR_C signature	);
			static	void*	realloc_aligned(	void* memblock,			size_t CONST size,	UINT32 alignment,		C_PCHAR_C signature	);
		
			static	void	memcpy(				void* CONST dst,	 void CONST * CONST src,	size_t byte_nb,		C_PCHAR_C signature	)	NOEXCEPT;
			static	void	memmove(			void* CONST dst,	 void CONST * CONST src,	size_t byte_nb		)						NOEXCEPT;
			static	void	memset(				void* CONST dst,	 INT32 CONST val,			size_t byte_nb		)						NOEXCEPT;
			static	void	memclear(			void* CONST dst,								size_t byte_nb		)						NOEXCEPT;

			static	void	counter_reset();
			static	void	update();
			static	void	update_info();

	template< typename T >	CONSTEXPR static uintptr_t	GET_ALIGNED_16(		T CONST * CONST p )	NOEXCEPT	{	return reinterpret_cast<uintptr_t>(p) & 0xf;	}
	template< typename T >	CONSTEXPR static bool		IS_ALIGNED_16(		T CONST * CONST p )	NOEXCEPT	{	return GET_ALIGNED_16(p) == 0;					}
	template< typename T >	CONSTEXPR static bool		IS_ALIGNED_16_NOT(	T CONST * CONST p )	NOEXCEPT	{	return GET_ALIGNED_16(p) != 0;					}

	template< typename T >	CONSTEXPR static uintptr_t	GET_ALIGNED_32(		T CONST * CONST p )	NOEXCEPT	{	return reinterpret_cast<uintptr_t>(p) & 0x1f;	}
	template< typename T >	CONSTEXPR static bool		IS_ALIGNED_32(		T CONST * CONST p )	NOEXCEPT	{	return GET_ALIGNED_32(p) == 0;					}
	template< typename T >	CONSTEXPR static bool		IS_ALIGNED_32_NOT(	T CONST * CONST p )	NOEXCEPT	{	return GET_ALIGNED_32(p) != 0;					}

	template< typename T >	CONSTEXPR static uintptr_t	GET_ALIGNED_64(		T CONST * CONST p )	NOEXCEPT	{	return reinterpret_cast<uintptr_t>(p) & 0x3f;	}
	template< typename T >	CONSTEXPR static bool		IS_ALIGNED_64(		T CONST * CONST p )	NOEXCEPT	{	return GET_ALIGNED_64(p) == 0;					}
	template< typename T >	CONSTEXPR static bool		IS_ALIGNED_64_NOT(	T CONST * CONST p )	NOEXCEPT	{	return GET_ALIGNED_64(p) != 0;					}


	
	static bool is_pointer_fucked( CP_CVOID pt );
	static FINLINE	bool	is_pointer_valid_or_null( CP_CVOID pt )
	{
		if( !pt )
			return true;
		return !is_pointer_fucked( pt );
	}

	static FINLINE	bool	is_pointer_valid( CP_CVOID pt )
	{
		if( !pt )
			return false;
	#if AAA_DEBUG()
		return !is_pointer_fucked( pt );
	#else
		return true;
	#endif
	}

	//	VLD
	static	void	enable_vld();
	static	void	disable_vld();
	static	void	report_leaks();
	static	void	mark_leaks_reported();
};

#define	HEAP_IS_CORRUPT()	( !mem::is_all_ok( __FUNCTION__ ) )
#if	AAA_DEBUG()
	#define	DBG_HEAP_IS_CORRUPT()	HEAP_IS_CORRUPT()
#else
	#define	DBG_HEAP_IS_CORRUPT()	(false)
#endif

#define HEAP_CHECK()		mem::check( __FUNCTION__ )
#if	AAA_DEBUG()
	#define	DBG_HEAP_CHECK()		HEAP_CHECK()
#else
	#define	DBG_HEAP_CHECK()		{}
#endif


FINLINE	void	MEMCPY(		UINT8* CONST dst, UINT8 CONST *	CONST src,	size_t CONST byte_nb, C_PCHAR_C signature )	NOEXCEPT	{	mem::memcpy( (void*)dst, (void CONST * CONST)src,	byte_nb,	signature );	}
FINLINE	void	MEMCPY(		void*  CONST dst, void  CONST *	CONST src,	size_t CONST byte_nb, C_PCHAR_C signature )	NOEXCEPT	{	mem::memcpy(   dst,		 src,						byte_nb,	signature );	}
FINLINE	void	MEMMOVE(	void*  CONST dst, void  CONST *	CONST src,	size_t CONST byte_nb )	NOEXCEPT						{	mem::memmove(  dst,		 src,						byte_nb );	}
FINLINE	void	MEMSET(		void*  CONST dst, INT32 CONST		  val,	size_t CONST byte_nb )	NOEXCEPT						{	mem::memset(   dst,		 val,						byte_nb );	}
FINLINE	void	MEMCLEAR(	void*  CONST dst,							size_t CONST byte_nb )	NOEXCEPT						{	mem::memclear( dst,									byte_nb );	}

FINLINE	void*	CALLOC(							size_t CONST nb,	size_t CONST size )												{	return mem::calloc(				nb,			size,			__FUNCTION__ );	}

FINLINE	void*	MALLOC_SIGNATURE(									size_t CONST size,						C_PCHAR_C signature	)	{	return mem::malloc(							size,			signature );	}
FINLINE	void*	MALLOC(												size_t CONST size											)	{	return mem::malloc(							size,			__FUNCTION__ );	}
FINLINE	void*	MALLOC_ALIGNED_SIGNATURE(							size_t CONST size,	UINT32 CONST align,	C_PCHAR_C signature	)	{	return mem::malloc_aligned(					size,	align,	signature );	}
FINLINE	void*	MALLOC_ALIGNED(										size_t CONST size,	UINT32 CONST align						)	{	return mem::malloc_aligned(					size,	align,	__FUNCTION__ );	}
															
FINLINE	void*	REALLOC_SIGNATURE(			void* CONST memblock,	size_t CONST size,						C_PCHAR_C signature	)	{	return mem::realloc(			memblock,	size,			signature );	}
FINLINE	void*	REALLOC(					void* CONST memblock,	size_t CONST size											)	{	return mem::realloc(			memblock,	size,			__FUNCTION__ );	}
FINLINE	void*	REALLOC_ALIGNED_SIGNATURE(	void* CONST memblock,	size_t CONST size,	UINT32 CONST align,	C_PCHAR_C signature	)	{	return mem::realloc_aligned(	memblock,	size,	align,	signature );	}
FINLINE	void*	REALLOC_ALIGNED_SIGNATURE(	void* CONST memblock,	size_t CONST size,						C_PCHAR_C signature	)	{	return mem::realloc_aligned(	memblock,	size,	0,		signature );	}
FINLINE	void*	REALLOC_ALIGNED(			void* CONST memblock,	size_t CONST size,	UINT32 CONST align						)	{	return mem::realloc_aligned(	memblock,	size,	align,	__FUNCTION__ );	}
FINLINE	void*	REALLOC_ALIGNED(			void* CONST memblock,	size_t CONST size											)	{	return mem::realloc_aligned(	memblock,	size,	0,		__FUNCTION__ );	}

#define	FREE_SIGNATURE(				memblock,	signature	)	mem::free(			(memblock),	(signature)		)
#define	FREE(						memblock				)	mem::free(			(memblock),	__FUNCTION__	)
#define	FREE_ALIGNED_SIGNATURE(		memblock,	signature	)	mem::free_aligned(	(memblock),	(signature)		)
#define	FREE_ALIGNED(				memblock				)	mem::free_aligned(	(memblock),	__FUNCTION__	)

#define	FREE_AND_NULL(				memblock )					{	FREE(			(memblock) ); (memblock) = nullptr;	}
#define	FREE_ALIGNED_AND_NULL(		memblock )					{	FREE_ALIGNED(	(memblock) ); (memblock) = nullptr;	}

#define	IF_FREE_AND_NULL(			memblock )					{	if( memblock )	{	FREE_AND_NULL(			(memblock) )	}	}
#define	IF_FREE_ALIGNED_AND_NULL(	memblock )					{	if( memblock )	{	FREE_ALIGNED_AND_NULL(	(memblock) )	}	}


//template on several lines for debug
template<class T>		// Standard says that delete (T*)0; is valid
FINLINE	void	SAFE_DELETE( T*& p )
{
	if(p)
	{
		delete p;
		p = nullptr;
	}
}

template<class T>
FINLINE	void	SAFE_NEW_ARRAY( T*& p, INT32 CONST size )
{
	if( !p )
	{
		p = new(std::nothrow) T[size];
	}
}
template<class T>
FINLINE	void	SAFE_DELETE_ARRAY( T*& p )
{
	if(p)
	{
		delete[] p;
		p = nullptr;
	}
}

template< typename T >
static	T* ALLOC_ARRAY_AND_INIT( INT32 nb, T CONST val_init )
{
	T* pt = new(std::nothrow) T[nb];
	if( pt )
	{
		T* p = pt-1;
		for( ; nb>0; --nb )
		{
			*++p = val_init;
		}
	}
	return pt;
}

