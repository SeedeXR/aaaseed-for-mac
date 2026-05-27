
#ifdef AAA_CPU_H
#error "CPU_H included more than once."
#endif
#define AAA_CPU_H 1


#ifndef	AAA_OBJ_UI_H
#	include "infrastructure/obj/obj_ui.h"
#endif
#ifndef _BITSET_
#	include <bitset>
#endif

class	c_cpu final : public c_obj_ui
{
	static CONSTEXPR INT32 CACHE_LINE_SIZE_DEFAULT = 64;
	FACTORY_DECLARE( c_cpu, c_obj_ui );
public:
	
	static	c_cpu*	one;	//todo deal better with singleton
	FINLINE static INT32 get_align_def()
	{
		if( one ) //hack had to do this for static init before c_cpu exist which require also c_obj_ui
			return one->_internal._cache_line_size;
		else
			return CACHE_LINE_SIZE_DEFAULT;	
	}

	static	void	c_init();
	static	void	c_deinit();

			void	init();
	virtual void	update();
	virtual void	param_init_pt_static();

			void	disable_most();

private:
	class c_internal
	{
	public:
		o_str	_vendor;
		o_str	_name;
		bool	_b_64;
		INT32	_cache_L1_Data_size;
		INT32	_cache_L1_Inst_size;
		INT32	_cache_L2_line;
		INT32	_cache_L2_size;
		INT32	_cache_L3_line;
		INT32	_cache_L3_size;
		INT32	_nSteppingID;
		INT32	_nModel;
		INT32	_nFamily;
		INT32	_nProcessorType;
		INT32	_nExtendedmodel;
		INT32	_nExtendedfamily;
		INT32	_nBrandIndex;
//		INT32	_cpu_cache_size;
		INT32	_cache_line_size;
		INT32	_core_physical_nb;
		INT32	_core_logical_nb;
		INT32	_socket_nb;

		int _nIds;
		int _nExIds;
		bool _isIntel;
		bool _isAMD;
		std::bitset<32> _f_1_ECX;
		std::bitset<32> _f_1_EDX;
		std::bitset<32> _f_7_EBX;
		std::bitset<32> _f_7_ECX;
		std::bitset<32> _f_81_ECX;
		std::bitset<32> _f_81_EDX;
		std::vector<std::array<int, 4>> _data;
		std::vector<std::array<int, 4>> _extdata;

		c_internal();
		void init();
		void enum_cpu();

		FINLINE bool x64( void ) NOEXCEPT						{ return _f_81_EDX[29]; }
	};

	//bool	_b_MMX;	//todo set but not used ?
	//bool	b_MMX_EXT;
	//bool	b_3DNow;
	//bool	b_3DNow_ext;

	bool	_b_SSE_use;
	bool	_b_SSE2_use;
	bool	_b_SSE3_use;
	bool	_b_SSSE3_use;
	bool	_b_SSE4_A_use;
	bool	_b_SSE4_1_use;
	bool	_b_SSE4_2_use;
	bool	_b_AVX_use;
	bool	_b_AVX2_use;
	bool	_b_AVX512_use;

	bool	_b_SSE_allow;
	bool	_b_SSE2_allow;
	bool	_b_SSE3_allow;
	bool	_b_SSSE3_allow;
	bool	_b_SSE4_A_allow;
	bool	_b_SSE4_1_allow;
	bool	_b_SSE4_2_allow;
	bool	_b_AVX_allow;
	bool	_b_AVX2_allow;
	bool	_b_AVX512_allow;
	
	bool	_b_SSE_has;
	bool	_b_SSE2_has;
	bool	_b_SSE3_has;
	bool	_b_SSSE3_has;
	bool	_b_SSE4_A_has;
	bool	_b_SSE4_1_has;
	bool	_b_SSE4_2_has;
	bool	_b_AVX_has;
	bool	_b_AVX2_has;
	bool	_b_AVX512_has;

	c_internal _internal;

public:
	FINLINE	bool is_use_SSE() CONST		{ return _b_SSE_use;	}
	FINLINE	bool is_use_SSE2() CONST	{ return _b_SSE2_use;	}
	FINLINE	bool is_use_SSE3() CONST	{ return _b_SSE3_use;	}
	FINLINE	bool is_use_SSE4_1() CONST	{ return _b_SSE4_1_use; }
	FINLINE	bool is_use_AVX() CONST		{ return _b_AVX_use;	}
	FINLINE	bool is_use_AVX2() CONST	{ return _b_AVX2_use;	}


private:
	// getters
	bool SSE3( void )			CONST	{ return _internal._f_1_ECX[0]; }
	bool PCLMULQDQ( void )		CONST	{ return _internal._f_1_ECX[1]; }
	bool MONITOR( void )		CONST	{ return _internal._f_1_ECX[3]; }
	bool VMExtensions( void )	CONST	{ return _internal._f_1_ECX[4]; }
	bool CPL( void )			CONST	{ return _internal._f_1_ECX[5]; }
	bool EnhSpeedStep( void )	CONST	{ return _internal._f_1_ECX[7]; }
	bool ThermalMonitor2( void )CONST	{ return _internal._f_1_ECX[8]; }
	bool SSSE3( void )			CONST	{ return _internal._f_1_ECX[9]; }
	bool FMA3( void )			CONST	{ return _internal._f_1_ECX[12]; }
	bool CMPXCHG16B( void )		CONST	{ return _internal._f_1_ECX[13]; }
	bool xTPR( void )			CONST	{ return _internal._f_1_ECX[14]; }
	bool PerfDebugMSR( void )	CONST	{ return _internal._f_1_ECX[15]; }
	bool SSE41( void )			CONST	{ return _internal._f_1_ECX[19]; }
	bool SSE42( void )			CONST	{ return _internal._f_1_ECX[20]; }
	bool MOVBE( void )			CONST	{ return _internal._f_1_ECX[22]; }
	bool POPCNT( void )			CONST	{ return _internal._f_1_ECX[23]; }
	bool AES( void )			CONST	{ return _internal._f_1_ECX[25]; }
	bool XSAVE( void )			CONST	{ return _internal._f_1_ECX[26]; }
	bool OSXSAVE( void )		CONST	{ return _internal._f_1_ECX[27]; }
	bool AVX( void )			CONST	{ return _internal._f_1_ECX[28]; }
	bool F16C( void )			CONST	{ return _internal._f_1_ECX[29]; }
	bool RDRAND( void )			CONST	{ return _internal._f_1_ECX[30]; }
	
	bool MSR( void )			CONST	{ return _internal._f_1_EDX[5]; }
	bool CX8( void )			CONST	{ return _internal._f_1_EDX[8]; }
	bool SEP( void )			CONST	{ return _internal._f_1_EDX[11]; }
	bool CMOV( void )			CONST	{ return _internal._f_1_EDX[15]; }
	bool CLFSH( void )			CONST	{ return _internal._f_1_EDX[19]; }
	bool MMX( void )			CONST	{ return _internal._f_1_EDX[23]; }
	bool FXSR( void )			CONST	{ return _internal._f_1_EDX[24]; }
	bool SSE( void )			CONST	{ return _internal._f_1_EDX[25]; }
	bool SSE2( void )			CONST	{ return _internal._f_1_EDX[26]; }
	
	bool FSGSBASE( void )		CONST	{ return _internal._f_7_EBX[0]; }
	bool BMI1( void )			CONST	{ return _internal._f_7_EBX[3]; }
	bool HLE( void )			CONST	{ return _internal._isIntel && _internal._f_7_EBX[4]; }
	bool AVX2( void )			CONST	{ return _internal._f_7_EBX[5]; }
	bool BMI2( void )			CONST	{ return _internal._f_7_EBX[8]; }
	bool ERMS( void )			CONST	{ return _internal._f_7_EBX[9]; }
	bool INVPCID( void )		CONST	{ return _internal._f_7_EBX[10]; }
	bool RTM( void )			CONST	{ return _internal._isIntel && _internal._f_7_EBX[11]; }
	bool MPX( void )			CONST	{ return _internal._f_7_EBX[14]; }
	bool AVX512F( void )		CONST	{ return _internal._f_7_EBX[16]; }
	bool AVX512_DQ( void )		CONST	{ return _internal._f_7_EBX[17]; }
	bool RDSEED( void )			CONST	{ return _internal._f_7_EBX[18]; }
	bool ADX( void )			CONST	{ return _internal._f_7_EBX[19]; }
	bool AVX512IFMA( void )		CONST	{ return _internal._f_7_EBX[21]; }
	bool AVX512PF( void )		CONST	{ return _internal._f_7_EBX[26]; }
	bool AVX512ER( void )		CONST	{ return _internal._f_7_EBX[27]; }
	bool AVX512CD( void )		CONST	{ return _internal._f_7_EBX[28]; }
	bool SHA( void )			CONST	{ return _internal._f_7_EBX[29]; }
	bool AVX512BW( void )		CONST	{ return _internal._f_7_EBX[30]; }
	bool AVX512VL( void )		CONST	{ return _internal._f_7_EBX[31]; }

	bool AVX512VBMI( void )		CONST	{ return _internal._f_7_ECX[1]; }
	bool PREFETCHWT1( void )	CONST	{ return _internal._f_7_ECX[0]; }
	
	bool LAHF( void )			CONST	{ return _internal._f_81_ECX[0]; }
	bool CmpLegacy( void )		CONST	{ return _internal._f_81_ECX[1]; }
	bool SVM( void )			CONST	{ return _internal._f_81_ECX[2]; }
	bool ExtApicSpace( void )	CONST	{ return _internal._f_81_ECX[3]; }
	bool AltMovCr8( void )		CONST	{ return _internal._f_81_ECX[4]; }
	bool LZCNT( void )			CONST	{ return _internal._isIntel && _internal._f_81_ECX[5]; }
	bool ABM( void )			CONST	{ return _internal._isAMD && _internal._f_81_ECX[5]; }
	bool SSE4a( void )			CONST	{ return _internal._isAMD && _internal._f_81_ECX[6]; }
	bool MisalignedSSE( void )	CONST	{ return _internal._f_81_ECX[7]; }
	bool PREFETCH( void )		CONST	{ return _internal._f_81_ECX[8]; }
	bool XOP( void )			CONST	{ return _internal._isAMD && _internal._f_81_ECX[11]; }
	bool SKINITandDEV( void )	CONST	{ return _internal._f_81_ECX[12]; }
	bool FMA4( void )			CONST	{ return _internal._f_81_ECX[16]; }
	bool TBM( void )			CONST	{ return _internal._isAMD && _internal._f_81_ECX[21]; }
	
	bool SYSCALL( void )		CONST		{ return _internal._isIntel && _internal._f_81_EDX[11]; }
	bool ExecuteDisableBitAvailable( void )	CONST	{ return _internal._f_81_EDX[16]; }
	bool MMXExtensions( void )	CONST	{ return _internal._f_81_EDX[18]; }
	bool FFXSR( void )			CONST	{ return _internal._f_81_EDX[21]; }
	bool GBSupport( void )		CONST	{ return _internal._isIntel && _internal._f_81_EDX[22]; }
	bool MMXEXT( void )			CONST	{ return _internal._isAMD && _internal._f_81_EDX[22]; }
	bool RDTSCP( void )			CONST	{ return _internal._isIntel && _internal._f_81_EDX[27]; }
//	bool x64( void )			CONST	{ return _internal._f_81_EDX[29]; }
	bool _3DNOWEXT( void )		CONST	{ return _internal._isAMD && _internal._f_81_EDX[30]; }
	bool _3DNOW( void )			CONST	{ return _internal._isAMD && _internal._f_81_EDX[31]; }

public:
	//static INT32 get_stepping( void )		{ return CPU_Rep._nSteppingID; }
	//static INT32 get_model( void )		{ return CPU_Rep._nModel; }
	//static INT32 get_family( void )		{ return CPU_Rep._nFamily; }
	//static INT32 get_cpu_type( void )		{ return CPU_Rep._nProcessorType; }
	INT32 get_cpu_core_physical_nb( void )	CONST	{ return _internal._core_physical_nb; }
	INT32 get_cpu_core_logical_nb( void )	CONST	{ return _internal._core_logical_nb; }
	INT32 get_cpu_socket_nb( void )			CONST	{ return _internal._socket_nb; }

	INT32 get_cache_L2_size( void )			CONST	{ return _internal._cache_L2_size; }
	INT32 get_cache_L3_size( void )			CONST	{ return _internal._cache_L3_size; }
	//static INT32 get_model_ext( void )	{ return CPU_Rep._nExtendedmodel; }
	//static INT32 get_family_ext( void )	{ return CPU_Rep._nExtendedfamily; }
	//static INT32 get_brand_index( void )	{ return CPU_Rep._nBrandIndex; }
//	static INT32 get_cache_size( void )		{ return CPU_Rep._cpu_cache_size; }
//	INT32 get_cache_line_size( void )		{ return _internal._nCacheLineSize; }
};

