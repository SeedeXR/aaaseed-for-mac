
#ifdef AAA_CPU_INTRINSICS_H
#error "CPU_INTRINSICS_H included more than once."
#endif
#define AAA_CPU_INTRINSICS_H 1


// Cross-compiler wrappers for the x86 CPUID and XGETBV intrinsics.
// MSVC exposes them via <intrin.h> under their MSVC names ; GCC and Clang
// expose equivalents through <cpuid.h> (CPUID) and <immintrin.h> (XGETBV).
// Wrap them once here so the consumer code (CPU.cpp) stays compiler-agnostic
// and the only place that knows about the platform split is this file.


#ifndef AAA_AAA_TYPE_H
#	include "aaa_type.h"
#endif


#ifdef _MSC_VER

#	include <intrin.h>
#	include <immintrin.h>	// for _XCR_XFEATURE_ENABLED_MASK and _xgetbv

	FINLINE void	aaa_cpuid	( int info[4], int leaf )				{	__cpuid	( info, leaf      ); }
	FINLINE void	aaa_cpuidex	( int info[4], int leaf, int sub )		{	__cpuidex( info, leaf, sub ); }
	FINLINE UINT64	aaa_xgetbv	( unsigned xcr )						{	return _xgetbv( xcr ); }

#elif defined(__GNUC__) || defined(__clang__)

#	include <cpuid.h>		// __cpuid_count
#	include <immintrin.h>	// _xgetbv (also _XCR_XFEATURE_ENABLED_MASK)

	FINLINE void	aaa_cpuid	( int info[4], int leaf )
	{
		__cpuid_count( leaf, 0,
					   reinterpret_cast<unsigned int&>(info[0]),
					   reinterpret_cast<unsigned int&>(info[1]),
					   reinterpret_cast<unsigned int&>(info[2]),
					   reinterpret_cast<unsigned int&>(info[3]) );
	}
	FINLINE void	aaa_cpuidex	( int info[4], int leaf, int sub )
	{
		__cpuid_count( leaf, sub,
					   reinterpret_cast<unsigned int&>(info[0]),
					   reinterpret_cast<unsigned int&>(info[1]),
					   reinterpret_cast<unsigned int&>(info[2]),
					   reinterpret_cast<unsigned int&>(info[3]) );
	}
	FINLINE UINT64	aaa_xgetbv	( unsigned xcr )						{	return _xgetbv( xcr ); }

#else
#	error "cpu_intrinsics.h: unsupported compiler, add a branch for your toolchain."
#endif
