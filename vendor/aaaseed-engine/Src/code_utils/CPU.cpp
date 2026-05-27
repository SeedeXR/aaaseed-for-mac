#include "cpu.h"
#include "cpu_intrinsics.h"
#include "infrastructure/param/param_declare.h"
#include "system/shared/SystemUtils.h"
#include "aaaseed.h"
#include "infrastructure/obj/root.h"
#include <array>


FACTORY_CREATE_V1( c_cpu, cpu, CPU Info, cpu );

namespace	n_cpu
{
	CONSTEXPR INT32	PARAM_NB_MAX = 42;

	CONST c_param_def param[PARAM_NB_MAX] =
	{
		PARAM_DEF_STR_LOCKED(	vendor				)
		PARAM_DEF_STR_LOCKED(	name				)
		PARAM_DEF_BOOL_LOCKED(	64_bits				)
		PARAM_DEF_INT32_LOCKED(	core_physical_nb	)
		PARAM_DEF_INT32_LOCKED(	core_logical_nb		)
		PARAM_DEF_INT32_LOCKED(	cache_L1_data_size	)
		PARAM_DEF_INT32_LOCKED(	cache_L1_inst_size	)
		PARAM_DEF_INT32_LOCKED(	cache_L2_line		)
		PARAM_DEF_INT32_LOCKED(	cache_L2_size		)
		PARAM_DEF_INT32_LOCKED(	cache_L3_line		)
		PARAM_DEF_INT32_LOCKED(	cache_L3_size		)
		PARAM_DEF_INT32_LOCKED(	cacheline_size		)
		//PARAM_DEF_BOOL_LOCKED(	MMX				)
		//PARAM_DEF_BOOL_LOCKED(	MMX_Ext			)
		//PARAM_DEF_BOOL_LOCKED(	3DNow!			)
		//PARAM_DEF_BOOL_LOCKED(	3DNow_Ext		)
		PARAM_DEF_BOOL_LOCKED(	SSE_has		)
		PARAM_DEF_BOOL_ON(		SSE_allow	)
		PARAM_DEF_BOOL_LOCKED(	SSE_use		)

		PARAM_DEF_BOOL_LOCKED(	SSE2_has		)
		PARAM_DEF_BOOL_ON(		SSE2_allow		)
		PARAM_DEF_BOOL_LOCKED(	SSE2_use		)

		PARAM_DEF_BOOL_LOCKED(	SSE3_has		)
		PARAM_DEF_BOOL_ON(		SSE3_allow		)
		PARAM_DEF_BOOL_LOCKED(	SSE3_use		)

		PARAM_DEF_BOOL_LOCKED(	SSSE3_has		)
		PARAM_DEF_BOOL_ON(		SSSE3_allow		)
		PARAM_DEF_BOOL_LOCKED(	SSSE3_use		)

		PARAM_DEF_BOOL_LOCKED(	SSE4_a_has		)
		PARAM_DEF_BOOL_ON(		SSE4_a_allow	)
		PARAM_DEF_BOOL_LOCKED(	SSE4_a_use		)

		PARAM_DEF_BOOL_LOCKED(	SSE4_1_has		)
		PARAM_DEF_BOOL_ON(		SSE4_1_allow	)
		PARAM_DEF_BOOL_LOCKED(	SSE4_1_use		)

		PARAM_DEF_BOOL_LOCKED(	SSE4_2_has		)
		PARAM_DEF_BOOL_ON(		SSE4_2_allow	)
		PARAM_DEF_BOOL_LOCKED(	SSE4_2_use		)

		PARAM_DEF_BOOL_LOCKED(	AVX_has			)
		PARAM_DEF_BOOL_ON(		AVX_allow		)
		PARAM_DEF_BOOL_LOCKED(	AVX_use			)

		PARAM_DEF_BOOL_LOCKED(	AVX2_has		)
		PARAM_DEF_BOOL_ON(		AVX2_allow		)
		PARAM_DEF_BOOL_LOCKED(	AVX2_use		)

		PARAM_DEF_BOOL_LOCKED(	AVX512_has		)
		PARAM_DEF_BOOL_ON(		AVX512_allow	)
		PARAM_DEF_BOOL_LOCKED(	AVX512_use		)
	};
}

c_cpu*	c_cpu::one = {nullptr};

void	c_cpu::c_init()
{
	node_pref->obj_get(one);
}

void	c_cpu::c_deinit()
{
	SAFE_DELETE(one);
}

void	c_cpu::param_init_pt_static()
{
	INT32	h = 0;

	param_set_pt( h, _internal._vendor				);
	param_set_pt( h, _internal._name				);
	param_set_pt( h, _internal._b_64				);
	param_set_pt( h, _internal._core_physical_nb	);
	param_set_pt( h, _internal._core_logical_nb		);
	param_set_pt( h, _internal._cache_L1_Data_size	);
	param_set_pt( h, _internal._cache_L1_Inst_size	);
	param_set_pt( h, _internal._cache_L2_line		);
	param_set_pt( h, _internal._cache_L2_size		);
	param_set_pt( h, _internal._cache_L3_line		);
	param_set_pt( h, _internal._cache_L3_size		);
	param_set_pt( h, _internal._cache_line_size		);

	//		param_set_pt(		h, c_cpu::b_MMX			);
	//		param_set_pt(		h, c_cpu::b_MMX_EXT		);
	//		param_set_pt(		h, c_cpu::b_3DNow		);
	//		param_set_pt(		h, c_cpu::b_3DNow_ext	);

	param_set_pt( h, _b_SSE_has			);
	param_set_pt( h, _b_SSE_allow		);
	param_set_pt( h, _b_SSE_use			);

	param_set_pt( h, _b_SSE2_has		);
	param_set_pt( h, _b_SSE2_allow		);
	param_set_pt( h, _b_SSE2_use		);

	param_set_pt( h, _b_SSE3_has		);
	param_set_pt( h, _b_SSE3_allow		);
	param_set_pt( h, _b_SSE3_use		);

	param_set_pt( h, _b_SSSE3_has		);
	param_set_pt( h, _b_SSSE3_allow		);
	param_set_pt( h, _b_SSSE3_use		);

	param_set_pt( h, _b_SSE4_A_has		);
	param_set_pt( h, _b_SSE4_A_allow	);
	param_set_pt( h, _b_SSE4_A_use		);

	param_set_pt( h, _b_SSE4_1_has		);
	param_set_pt( h, _b_SSE4_1_allow	);
	param_set_pt( h, _b_SSE4_1_use		);

	param_set_pt( h, _b_SSE4_2_has		);
	param_set_pt( h, _b_SSE4_2_allow	);
	param_set_pt( h, _b_SSE4_2_use		);

	param_set_pt( h, _b_AVX_has			);
	param_set_pt( h, _b_AVX_allow		);
	param_set_pt( h, _b_AVX_use			);

	param_set_pt( h, _b_AVX2_has		);
	param_set_pt( h, _b_AVX2_allow		);
	param_set_pt( h, _b_AVX2_use		);

	param_set_pt( h, _b_AVX512_has		);
	param_set_pt( h, _b_AVX512_allow	);
	param_set_pt( h, _b_AVX512_use		);

	err_param_init_pt(h);
}

CONSTRUCTOR_CREATE( c_cpu )
	,_b_SSE_use		{false}
	,_b_SSE2_use	{false}
	,_b_SSE3_use	{false}
	,_b_SSSE3_use	{false}
	,_b_SSE4_A_use	{false}
	,_b_SSE4_1_use	{false}
	,_b_SSE4_2_use	{false}
	,_b_AVX_use		{false}
	,_b_AVX2_use	{false}
	,_b_AVX512_use	{false}
{
	init();
	param_init_with( n_cpu::param, n_cpu::PARAM_NB_MAX );
	update();
}
EMPTY_DESTRUCTOR( c_cpu )




//	bool	c_cpu::b_MMX_EXT;
//	bool	c_cpu::b_3DNow;
//	bool	c_cpu::b_3DNow_ext;


c_cpu::c_internal::c_internal()
	:_b_64					{false}
	,_cache_L1_Data_size	{0}
	,_cache_L1_Inst_size	{0}
	,_cache_L2_line			{0}
	,_cache_L2_size			{0}
	,_cache_L3_line			{0}
	,_cache_L3_size			{0}
	,_nSteppingID			{0}
	,_nModel				{0}
	,_nFamily				{0}
	,_nProcessorType		{0}
	,_nExtendedmodel		{0}
	,_nExtendedfamily		{0}
	,_nBrandIndex			{0}
	,_cache_line_size		{CACHE_LINE_SIZE_DEFAULT}
	,_core_physical_nb		{0}
	,_core_logical_nb		{0}
	,_socket_nb				{0}
	,_nIds					{0}
	,_nExIds				{0}
	,_isIntel				{false}
	,_isAMD					{false}
	,_f_1_ECX				{0}
	,_f_1_EDX				{0}
	,_f_7_EBX				{0}
	,_f_7_ECX				{0}
	,_f_81_ECX				{0}
	,_f_81_EDX				{0}
	,_data					{}
	,_extdata				{}
{
}

namespace {
	// Helper function to count set bits in the processor mask.
	//	the math one is not inited yet
	CONSTEXPR INT32 count_bit_on( ULONG_PTR bitMask )
	{
		DWORD LSHIFT		= sizeof( ULONG_PTR ) * 8 - 1;
		DWORD bitSetCount	= 0;
		ULONG_PTR bitTest	= (ULONG_PTR)1 << LSHIFT;

		for ( DWORD i = 0; i <= LSHIFT; ++i )
		{
			bitSetCount += ((bitMask & bitTest) ? 1 : 0);
			bitTest /= 2;
		}

		return bitSetCount;
	}

	void str_size_kb( o_str& str, std::size_t CONST value )
	{
		static const char* SIZES[] = { "B", "KiB", "MiB", "GiB" };
		int div = 0;
		size_t rem = 0;
		size_t size = value;

		while ( size >= 1024 && div < ( sizeof SIZES / sizeof * SIZES ) )
		{
			rem = ( size % 1024 );
			++div;
			size /= 1024;
		}

		double size_d = (float)size + (float)rem / 1024.0;
		str.add( (REAL)F_FLOOR( size_d ) );
		str.add( " " );
		str.add( SIZES[ div ] );
	}
}

void	c_cpu::c_internal::enum_cpu()
{
//	DBG_PRINT_STRING( "c_cpu::c_internal::enum_cpu() Begin" );

	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION buffer	= nullptr;
	DWORD returnLength			= 0;

	//loop from m$ example: 1st call sizes the buffer, 2nd call fills it. Cap retries to avoid hanging on a misbehaving runtime.
	INT32 CONST	RETRIES_MAX	= 5;
	INT32		retries		= 0;
	while( true )
	{
		if( ++retries > RETRIES_MAX )
		{
			ERR_PRINT_STRING( "GetLogicalProcessorInformation: too many retries (%d)", RETRIES_MAX );
			FREE_AND_NULL( buffer );
			return;
		}
		DWORD rc = GetLogicalProcessorInformation( buffer, &returnLength );
		if( FALSE == rc )
		{
			if( auto err = aaa::system::get_err_last(); err == ERROR_INSUFFICIENT_BUFFER )
			{
				buffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)REALLOC( (void*) buffer, returnLength );
				if( !buffer )
				{
					ERR_PRINT_STRING( "Error: Allocation failure" );
					return;
				}
			}
			else
			{
				ERR_PRINT_STRING( "Error %s", aaa::system::get_err_message(err).c_str() );
				return;
			}
		}
		else
			break;
	}

	INT32 logicalProcessorCount = 0;
	INT32 processorCoreCount	= 0;
	INT32 processorPackageCount = 0;
	INT32 numaNodeCount			= 0;

	INT64 processorL1CacheData_size = 0;
	INT64 processorL1CacheInst_size = 0;
	INT32 processorL1CacheData_count = 0;
	INT32 processorL1CacheInst_count = 0;

	INT32 processorL2Cache_line = 0;
	INT64 processorL2Cache_size = 0;
	INT32 processorL2Cache_count = 0;

	INT32 processorL3Cache_line = 0;
	INT64 processorL3Cache_size = 0;
	INT32 processorL3Cache_count = 0;

	UINT32 byteOffset			= 0;
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION p_cache = buffer;
	while ( byteOffset + sizeof( SYSTEM_LOGICAL_PROCESSOR_INFORMATION ) <= returnLength )
	{
		switch( p_cache->Relationship )
		{
		case RelationNumaNode:
			// Non-NUMA systems report a single record of this type.
			++numaNodeCount;
			break;
		case RelationProcessorCore:
			++processorCoreCount;
			//DBG_PRINT_STRING( "processorCoreCount %d -> bits count %d", processorCoreCount, count_bit_on(p_cache->ProcessorMask) );
			// A hyperthreaded core supplies more than one logical processor.
			logicalProcessorCount += count_bit_on( p_cache->ProcessorMask );
			break;
		case RelationProcessorPackage:
			// Logical processors share a physical package.
			++processorPackageCount;
			break;
		case RelationCache:
			{
				// Cache data is in ptr->Cache, one CACHE_DESCRIPTOR structure for each cache. 
				PCACHE_DESCRIPTOR CONST & cache = &p_cache->Cache;
				switch( cache->Level )
				{
				case 1:
					if( cache->Type == CacheData )
					{
						processorL1CacheData_size += cache->Size;
						++processorL1CacheData_count;
					}
					else if( cache->Type == CacheInstruction )
					{
						processorL1CacheInst_size += cache->Size;
						++processorL1CacheInst_count;
					}
					break;
				case 2:
					if( cache->Type == CacheData || cache->Type == CacheUnified )
					{
						processorL2Cache_line = cache->LineSize;
						processorL2Cache_size += cache->Size;
						++processorL2Cache_count;
					}
					break;
				case 3:
					if( cache->Type == CacheData || cache->Type == CacheUnified )
					{
						processorL3Cache_line = cache->LineSize;
						processorL3Cache_size += cache->Size;
						++processorL3Cache_count;
					}
					break;
				}
			}
			break;
		case RelationGroup:
			break;
		default:
			// Win10+ adds new relations (RelationProcessorModule, RelationProcessorDie, ...). Ignore silently.
			break;
		}
		byteOffset += sizeof( SYSTEM_LOGICAL_PROCESSOR_INFORMATION );
		++p_cache;
	}
	FREE_AND_NULL( buffer );

	if( processorL1CacheData_count )
		_cache_L1_Data_size = I_FLOOR(processorL1CacheData_size / processorL1CacheData_count);
	if( processorL1CacheInst_count )
		_cache_L1_Inst_size = I_FLOOR(processorL1CacheInst_size / processorL1CacheInst_count);

	_cache_L2_line = processorL2Cache_line;
	if( processorL2Cache_count )
		_cache_L2_size = I_FLOOR(processorL2Cache_size / processorL2Cache_count);

	_cache_L3_line = processorL3Cache_line;
	if( processorL3Cache_count )
		_cache_L3_size = I_FLOOR(processorL3Cache_size / processorL3Cache_count);

	_core_physical_nb	= processorCoreCount;
	_core_logical_nb	= logicalProcessorCount;
	_socket_nb			= processorPackageCount;
	GOOD_PRINT_STRING( "GetLogicalProcessorInformation results:" );
	GOOD_PRINT_STRING( "\tNUMA nodes: %d", numaNodeCount );
	GOOD_PRINT_STRING( "\tphysical processor packages: %d", processorPackageCount );
	GOOD_PRINT_STRING( "\tprocessor cores: %d", processorCoreCount );
	GOOD_PRINT_STRING( "\tlogical processors: %d", logicalProcessorCount );

	o_str l1_data_size;
	o_str l1_inst_size;
	o_str l2_data_size;
	o_str l3_data_size;
	str_size_kb( l1_data_size, _cache_L1_Data_size );
	str_size_kb( l1_inst_size, _cache_L1_Inst_size );
	str_size_kb( l2_data_size, _cache_L2_size );
	str_size_kb( l3_data_size, _cache_L3_size );
	GOOD_PRINT_STRING( "\tprocessor L1 cache: Data %d * %s / Inst. %d * %s", processorL1CacheData_count, l1_data_size.get(), processorL1CacheInst_count, l1_inst_size.get() );
	GOOD_PRINT_STRING( "\tprocessor L2 cache: %d * %s", processorL2Cache_count, l2_data_size.get() );
	GOOD_PRINT_STRING( "\tprocessor L3 cache: %d * %s", processorL3Cache_count, l3_data_size.get() );

//	DBG_PRINT_STRING( "c_cpu::c_internal::enum_cpu() End" );
}

void c_cpu::c_internal::init()
{
	DBG_PRINT_STRING( "c_cpu::c_internal::init() Begin" );

	//int cpuInfo[4] = {-1};
	std::array<int, 4> cpui;

	// Calling __cpuid with 0x0 as the function_id argument
	// gets the number of the highest valid function ID.
	aaa_cpuid( cpui.data(), 0 );
	_nIds = cpui[0];

	for( int i = 0; i <= _nIds; ++i )
	{
		aaa_cpuidex( cpui.data(), i, 0 );
		_data.push_back( cpui );
	}

	// Capture vendor string
	CHAR vendor[0x20];
	MEMCLEAR( vendor, sizeof( vendor ) );
	std::memcpy( vendor + 0, &_data[0][1], sizeof(int) );	// EBX
	std::memcpy( vendor + 4, &_data[0][3], sizeof(int) );	// EDX
	std::memcpy( vendor + 8, &_data[0][2], sizeof(int) );	// ECX
	_vendor.set(vendor);
	if(			_vendor.is_str_equal( "GenuineIntel" ) )
		_isIntel = true;
	else if(	_vendor.is_str_equal( "AuthenticAMD" ) )
		_isAMD = true;

	// load bitset with flags for function 0x00000001
	if( _nIds >= 1 )
	{
		_f_1_ECX = _data[1][2];
		_f_1_EDX = _data[1][3];

		// CPUID outputs are unsigned 32-bit; cast before shifting so we don't right-shift a signed int (UB pre-C++20)
		UINT32 CONST eax	= UINT32( _data[1][0] );
		UINT32 CONST ebx	= UINT32( _data[1][1] );
		_nSteppingID		=  eax        & 0xf;
		_nModel				= (eax >>  4) & 0xf;
		_nFamily			= (eax >>  8) & 0xf;
		_nProcessorType		= (eax >> 12) & 0x3;
		_nExtendedmodel		= (eax >> 16) & 0xf;
		_nExtendedfamily	= (eax >> 20) & 0xff;
		_nBrandIndex		=  ebx        & 0xff;
	}
	// load bitset with flags for function 0x00000007
	if( _nIds >= 7 )
	{
		_f_7_EBX = _data[7][1];
		_f_7_ECX = _data[7][2];
	}

	// Calling __cpuid with 0x80000000 as the function_id argument
	// gets the number of the highest valid extended ID.
	aaa_cpuid( cpui.data(), 0x80000000 );
	_nExIds = cpui[0];

	char brand[0x40];
	std::memset( brand, 0, sizeof( brand ) );

	for( int i = 0x80000000; i <= _nExIds; ++i )
	{
		aaa_cpuidex( cpui.data(), i, 0 );
		_extdata.push_back( cpui );
	}

	// load bitset with flags for function 0x80000001
	if( _nExIds >= 0x80000001 )
	{
		_f_81_ECX = _extdata[1][2];
		_f_81_EDX = _extdata[1][3];
	}

	// Interpret CPU brand string if reported
	if( _nExIds >= 0x80000004 )
	{
		MEMCPY( brand,		_extdata[2].data(), sizeof( cpui ), __FUNCTION__ );
		MEMCPY( brand + 16, _extdata[3].data(), sizeof( cpui ), __FUNCTION__ );
		MEMCPY( brand + 32, _extdata[4].data(), sizeof( cpui ), __FUNCTION__ );
		_name.set( brand );
	}
	if( _nExIds >= 0x80000006 )
	{
		_cache_line_size = _extdata[6][2] & 0xff;
//		_cpu_cache_size = (_extdata[6][2] >> 16) & 0xffff;
		if( _cache_line_size == 0 )
			_cache_line_size = 64;	// CPUID returned 0 (some virtualized CPUs); fall back so get_align_def() never returns 0
	}
	else
	{	//err message here ?
		_cache_line_size = 64;
//		_cpu_cache_size = 0;s
	}
	enum_cpu();
//	cpu_core_nb		= get_cpu_logical_nb();
	_b_64			= x64();

	DBG_PRINT_STRING( "c_cpu::c_internal::init() End" );

}

void c_cpu::update()
{
	_b_SSE_use		= _b_SSE_has	&&	_b_SSE_allow;
	_b_SSE2_use		= _b_SSE2_has	&&	_b_SSE2_allow;
	_b_SSE3_use		= _b_SSE3_has	&&	_b_SSE3_allow;
	_b_SSSE3_use	= _b_SSSE3_has	&&	_b_SSSE3_allow;
	_b_SSE4_A_use	= _b_SSE4_A_has	&&	_b_SSE4_A_allow;
	_b_SSE4_1_use	= _b_SSE4_1_has	&&	_b_SSE4_1_allow;
	_b_SSE4_2_use	= _b_SSE4_2_has	&&	_b_SSE4_2_allow;
	_b_AVX_use		= _b_AVX_has	&&	_b_AVX_allow;
	_b_AVX2_use		= _b_AVX2_has	&&	_b_AVX2_allow;
	_b_AVX512_use	= _b_AVX512_has	&&	_b_AVX512_allow;
}

// Initialize static member data
//c_cpu::cpu_internal c_cpu::CPU_Rep;

void c_cpu::init()
{
	_internal.init();

//	_b_MMX			= MMX();

	_b_SSE_has		= SSE();
	_b_SSE2_has		= SSE2();
	_b_SSE3_has		= SSE3();
	_b_SSSE3_has	= SSSE3();
	_b_SSE4_A_has	= SSE4a();
	_b_SSE4_1_has	= SSE41();
	_b_SSE4_2_has	= SSE42();
	// Read XCR0 once to know which YMM/ZMM lanes the OS preserves on context switch.
	bool	b_os_avx	= false;
	bool	b_os_avx512	= false;
	if( OSXSAVE() && AVX() )
	{
		UINT64 CONST xcr = aaa_xgetbv( _XCR_XFEATURE_ENABLED_MASK );
		b_os_avx	= (xcr & 0x6 ) == 0x6;
		b_os_avx512	= (xcr & 0xe6) == 0xe6;
	}
	_b_AVX_has		= b_os_avx;
	_b_AVX2_has		= b_os_avx && c_cpu::AVX2();
	_b_AVX512_has	= b_os_avx512;
	
//	cpu_cache_size	= get_cache_size();
//	cpu_vendor		= Vendor().c_str();
//	cpu_name		= Brand().c_str();
//	if( cpu_vendor == "GenuineIntel" )
//	{
//		GOOD_PRINT_STRING( "Here" );
//	}

	// Display all the information in user-friendly format.

	GOOD_PRINT_STRING( "CPU String: %s",			_internal._vendor.get()	);
	GOOD_PRINT_STRING( "CPU Brand String: %s",		_internal._name.get()	);
	//GOOD_PRINT_STRING( "\tStepping ID = %d",		get_stepping()		);
	//GOOD_PRINT_STRING( "\tModel = %d",			get_model()			);
	//GOOD_PRINT_STRING( "\tFamily = %d",			get_family()		);
	//GOOD_PRINT_STRING( "\tProcessor Type = %d",	get_cpu_type()		);
	//GOOD_PRINT_STRING( "\tExtended model = %d",	get_model_ext()		);
	//GOOD_PRINT_STRING( "\tExtended family = %d",	get_family_ext()	);
	//GOOD_PRINT_STRING( "\tBrand Index = %d",		get_brand_index()	);

	GOOD_PRINT_STRING( "Logical CPU count = %d (%d socket, %d core)", get_cpu_core_logical_nb(), get_cpu_socket_nb(), get_cpu_core_physical_nb() );
	o_str	cpu_ext;
	if( _b_SSE_has		)
		cpu_ext.add( "SSE "		);
	if( _b_SSE2_has		)
		cpu_ext.add( "SSE2 "	);
	if( _b_SSE3_has		)
		cpu_ext.add( "SSE3 "	);
	if( _b_SSSE3_has	)
		cpu_ext.add( "SSSE3 "	);
	if( _b_SSE4_A_has	)
		cpu_ext.add( "SSE4.a "	);
	if( _b_SSE4_1_has	)
		cpu_ext.add( "SSE4.1 "	);
	if( _b_SSE4_2_has	)
		cpu_ext.add( "SSE4.2 "	);
	if( _b_AVX_has		)
		cpu_ext.add( "AVX "		);
	if( _b_AVX2_has		)
		cpu_ext.add( "AVX2 "	);
	if( _b_AVX512_has	)
		cpu_ext.add( "AVX512 "	);
	GOOD_PRINT_STRING( "CPU Supports : %s", cpu_ext.get() );
	//if( b_64 )
	//	GOOD_PRINT_STRING( "\t64 bit" );
}

void c_cpu::disable_most()
{
	_b_SSE_allow	= false;
	_b_SSE2_allow	= false;
	_b_SSE3_allow	= false;
	_b_SSSE3_allow	= false;
	_b_SSE4_A_allow	= false;
	_b_SSE4_1_allow	= false;
	_b_SSE4_2_allow	= false;
	_b_AVX_allow	= false;
	_b_AVX2_allow	= false;
	_b_AVX512_allow	= false;
	update();
}
