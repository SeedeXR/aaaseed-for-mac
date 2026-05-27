
#ifdef AAA_AAA_TYPE_H
#error "AAA_TYPE_H included more than once."
#endif
#define AAA_AAA_TYPE_H 1


// Define WIN32 if this is MS Windows
#ifndef WIN32
#	if defined( _MSC_VER) || defined(__BORLANDC__) || defined(_WIN32) || defined(__WIN32__) || defined(__TOS_WIN__)
#		define WIN32
#	endif 
#endif

//#define AAA_OS(X) AAA_OS_##X()
#ifdef WIN32
//should replace WIN32 in AAA source code better that WIN32 which confuse 32/64 bits
#	define AAA_OS_WINDOWS() 1
#	define AAA_OS_MAC() 0
#	define AAA_OS_LINUX() 0
#else
#	define AAA_OS_WINDOWS() 0
#	define AAA_OS_LINUX() 0
#	ifdef __APPLE__
#		include <TargetConditionals.h>
#		if TARGET_OS_MAC // macOS (desktop)
#			define AAA_OS_MAC() 1
#		else
#			define AAA_OS_MAC() 0
#		endif
#		if TARGET_OS_IPHONE	// iOS
#			error Unsupported iOS
#		endif
#		if TARGET_OS_SIMULATOR	// iOS Simulator
#			error Unsupported iOS Simulator
#		endif
#	else
#		error Unknown target OS
#	endif
#endif

// make sure WIN64 define are OK
#if defined(_WIN64) || defined(WIN64)
#	define AAA_WIN64() 1
#	define AAA_BIT64() 1
#	ifndef WIN64
#		define WIN64 1
#	endif
#	ifndef _WIN64
#		define _WIN64 1
#	endif
#	ifndef __WIN64__
#		define __WIN64__ 1
#	endif
#else
#	define AAA_WIN64() 0
#	define AAA_BIT64() 0
#endif

#ifdef	DEBUG
#		define AAA_DEBUG() 1
#else
#	ifdef	NDEBUG
#		define AAA_DEBUG() 0
#	else
#		define AAA_DEBUG() 1
#	endif
#endif



#define AAA_STRING2(x) #x
#define AAA_STRING(x) AAA_STRING2(x)

#if 0
#	pragma message( "__cplusplus is " AAA_STRING(__cplusplus) ) 
#endif


#ifdef _MSC_VER
//#		pragma message( "_MSC_VER is " AAA_STRING(_MSC_VER) )
#	if _MSC_VER >= 1950	//uncharted territory
#		define AAA_COMPILER_STR() "Insiders Visual Studio 2026 or older"
#		define AAA_VSTOOL() 145
#		define NOEXCEPT noexcept
#		define CONSTEXPR constexpr
#	elif _MSC_VER > 1944	//uncharted territory
#		define AAA_COMPILER_STR() "Probably insiders Visual Studio 2026"
#		define AAA_VSTOOL() 143
#		define NOEXCEPT noexcept
#		define CONSTEXPR constexpr
#	elif _MSC_VER >= 1930	//VS2022 MSVC++ 14.3
#		if _MSC_VER > 1940
#			define AAA_COMPILER_STR() "Visual Studio 2022 Preview"
#		elif _MSC_VER == 1930
#			define AAA_COMPILER_STR() "Visual Studio 2022 RTW (17.0)"
#		elif _MSC_VER == 1931
#			define AAA_COMPILER_STR() "Visual Studio 2022 version 17.1"
#		elif _MSC_VER == 1932
#			define AAA_COMPILER_STR() "Visual Studio 2022 version 17.2"
#		elif _MSC_VER == 1933
#			define AAA_COMPILER_STR() "Visual Studio 2022 version 17.3"
#		elif _MSC_VER == 1934
#			define AAA_COMPILER_STR() "Visual Studio 2022 version 17.4"
#		elif _MSC_VER == 1935
#			define AAA_COMPILER_STR() "Visual Studio 2022 version 17.5"
#		elif _MSC_VER == 1936
#			define AAA_COMPILER_STR() "Visual Studio 2022 version 17.6"
#		elif _MSC_VER == 1937
#			define AAA_COMPILER_STR() "Visual Studio 2022 version 17.7"
#		elif _MSC_VER == 1938
#			define AAA_COMPILER_STR() "Visual Studio 2022 version 17.8"
#		elif _MSC_VER == 1939
#			define AAA_COMPILER_STR() "Visual Studio 2022 version 17.9"
#		elif _MSC_VER == 1940
#			define AAA_COMPILER_STR() "Visual Studio 2022 version 17.10"
#		elif _MSC_VER == 1941
#			define AAA_COMPILER_STR() "Visual Studio 2022 version 17.11"
#		elif _MSC_VER == 1942
#			define AAA_COMPILER_STR() "Visual Studio 2022 version 17.12"
#		elif _MSC_VER == 1943
#			define AAA_COMPILER_STR() "Visual Studio 2022 version 17.13"
#		elif _MSC_VER == 1944
#			define AAA_COMPILER_STR() "Visual Studio 2022 version 17.14"
#		endif
#		define AAA_VSTOOL() 143
#		define NOEXCEPT noexcept
#		define CONSTEXPR constexpr
#	elif _MSC_VER >= 1920	//VS2019 MSVC++ 14.2
#		if _MSC_VER == 1929
#			define AAA_COMPILER_STR() "Visual Studio 2019 version 16.10, 16.11"
#		elif _MSC_VER == 1928
#			define AAA_COMPILER_STR() "Visual Studio 2019 version 16.8, 16.9"
#		elif _MSC_VER == 1927
#			define AAA_COMPILER_STR() "Visual Studio 2019 version 16.7"
#		elif _MSC_VER == 1926
#			define AAA_COMPILER_STR() "Visual Studio 2019 version 16.6"
#		elif _MSC_VER == 1925
#			define AAA_COMPILER_STR() "Visual Studio 2019 version 16.5"
#		elif _MSC_VER == 1924
#			define AAA_COMPILER_STR() "Visual Studio 2019 version 16.4"
#		elif _MSC_VER == 1923
#			define AAA_COMPILER_STR() "Visual Studio 2019 version 16.3"
#		elif _MSC_VER == 1922
#			define AAA_COMPILER_STR() "Visual Studio 2019 version 16.2"
#		elif _MSC_VER == 1921
#			define AAA_COMPILER_STR() "Visual Studio 2019 version 16.1"
#		elif _MSC_VER == 1920
#			define AAA_COMPILER_STR() "Visual Studio 2019 RTW (16.0)"
#		endif
#		define AAA_VSTOOL() 142
#		define NOEXCEPT noexcept
#		define CONSTEXPR constexpr
#	elif _MSC_VER >= 1910	//VS2017 MSVC++ 14.1
#		if _MSC_VER == 1910
#			define AAA_COMPILER_STR() "Visual Studio 2017 RTW (15.0)"
#		elif _MSC_VER == 1911
#			define AAA_COMPILER_STR() "Visual Studio 2017 version 15.3"
#		elif _MSC_VER == 1912
#			define AAA_COMPILER_STR() "Visual Studio 2017 version 15.5"
#		elif _MSC_VER == 1913
#			define AAA_COMPILER_STR() "Visual Studio 2017 version 15.6"
#		elif _MSC_VER == 1914
#			define AAA_COMPILER_STR() "Visual Studio 2017 version 15.7"
#		elif _MSC_VER == 1915
#			define AAA_COMPILER_STR() "Visual Studio 2017 version 15.8"
#		elif _MSC_VER == 1916
#			define AAA_COMPILER_STR() "Visual Studio 2017 version 15.9"
#		else
#			error _MSC_VER unknown
#		endif
#		define AAA_VSTOOL() 141
#		define NOEXCEPT noexcept
#		define CONSTEXPR constexpr
#	elif _MSC_VER >= 1900	//VS2015 MSVC++ 14.0
#		define AAA_COMPILER_STR() "Visual Studio 2015 (14.0)"
#		define AAA_VSTOOL() 140
#		define NOEXCEPT noexcept
#		define CONSTEXPR
#	elif _MSC_VER >= 1800	//VS2013 MSVC++ 12.0
#		define AAA_COMPILER_STR() "Visual Studio 2013 (12.0)"
#		define AAA_VSTOOL() 120
#		define NOEXCEPT
#		define CONSTEXPR
#	elif _MSC_VER >= 1700	//VS2012 MSVC++ 11.0
#		define AAA_COMPILER_STR() "Visual Studio 2012 (11.0)"
#		define	AAA_VSTOOL() 110
#	elif _MSC_VER >= 1600	//VS2010 MSVC++ 10.0
#		define AAA_COMPILER_STR() "Visual Studio 2010 (10.0)"
#		define	AAA_VSTOOL() 100
#	else
#		error _MSC_VER unknow
#	endif
//#	pragma message( "AAA_VSTOOL is " STRING(AAA_VSTOOL()) ) 
#else
//	Non-MSVC compiler (AppleClang on macOS, Clang or GCC on Linux, etc).
//	Define the same macros the MSVC branch defines so the rest of the engine
//	does not need to gate on _MSC_VER. AAA_VSTOOL() = 145 declares "this is
//	a modern toolchain with everything the engine cares about (C++11+, the
//	full STL, atomic, mutex, ...)" — used by aaa_mutex.h to gate c_spinlock.
#	define NOEXCEPT noexcept
#	define CONSTEXPR constexpr
#	define AAA_VSTOOL() 145
#	if defined(__clang__)
#		define AAA_COMPILER_STR() "Clang"
#	elif defined(__GNUC__)
#		define AAA_COMPILER_STR() "GCC"
#	else
#		define AAA_COMPILER_STR() "non-MSVC"
#	endif
#endif
#define AAA_LIB_VSTOOL_STR() "v" AAA_STRING(AAA_VSTOOL())
//#pragma message( "AAA_LIB_VSTOOL_STR() is " AAA_LIB_VSTOOL_STR() )

// Endianity
#if defined(__BYTE_ORDER__) && (__BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__) || defined(_WIN32) || defined(__i386__) || defined(__x86_64__) || defined(__aarch64__) || defined(__arm64__)
// It's a little-endian target architecture
#	define AAA_ENDIAN_LITTLE() 1
#else
// It's a big-endian target architecture
#	error "I don't deal with this architecture yet !"
#	define AAA_ENDIAN_LITTLE() 0
#endif

#if 0
#	pragma message( "__AVX__ is " AAA_STRING(__AVX__) )
#endif
#ifdef __AVX__
#	define AAA_USE_AVX() 1
#else
#	define AAA_USE_AVX() 0
#endif

#if 0
#	pragma message( "__AVX2__ is " AAA_STRING(__AVX2__) )
#endif
#ifdef __AVX2__
#	define AAA_USE_AVX2() 1
#else
#	define AAA_USE_AVX2() 0
#endif

#ifdef AAA_STATE_COMPILE_PRIVATE
#	define  AAA_STATE_COMPILE()	1
#else
#	define  AAA_STATE_COMPILE()	0
#endif

#ifdef _MSC_VER
#	if AAA_VSTOOL() >= 100	//VS2010
#		define AAA_TMP_USE_INCLUDE() 1
#	else
#		define AAA_TMP_USE_INCLUDE() 0
#	endif
#elif __GNUC__ >= 3
#	define AAA_TMP_USE_INCLUDE() 1
#endif

#ifndef _MSC_STDINT_H_
#	ifndef __STDC_LIMIT_MACROS
#		define __STDC_LIMIT_MACROS 1
#	endif
#	include	<stdint.h>
#endif

#if AAA_TMP_USE_INCLUDE() == 1
#	ifndef _CSTDINT_
#		include <cstdint>
#	endif
#else
	typedef __int8				int8_t;
	typedef __int16				int16_t;
	typedef __int32				int32_t;
	typedef __int64				int64_t;
	typedef unsigned __int8		uint8_t;
	typedef unsigned __int16	uint16_t;
	typedef unsigned __int32	uint32_t;
	typedef unsigned __int64	uint64_t;
#endif
#undef AAA_TMP_USE_INCLUDE


//windows cause troubles
#ifndef NOMINMAX
#	define NOMINMAX // prevent windows from messing with std::min and std::max
#endif

#ifndef	CONST
#	define	CONST	const
#endif
#define	RESTRICT __restrict


#ifdef max
#	pragma push_macro("max")
#	undef max
#	define _restore_max_
#endif

#ifndef _ALGORITHM_
#	include <algorithm>
#endif

#ifdef	_MSC_VER
#	define	FINLINE		__forceinline
#else
	//	define because of X include
#	define	FINLINE		inline
#endif	//#ifdef	_MSC_VER

//	FINLINE_PB : "FINLINE Problem" marker. Empty macro, no inlining. Used to tag a function we
//	would WANT as FINLINE but currently can't be cleanly inlined. The typical case is a `static`
//	member or a free function whose body lives in a .cpp ; promoting it to FINLINE then emits
//	MSVC warning C4506 "no definition for inline function" in every TU that includes the header.
//	Non-static member functions do NOT trigger C4506 even with body in cpp, so they are usually
//	promoted directly to FINLINE rather than tagged FINLINE_PB. When the body of a tagged function
//	can be moved to the header (or its dependencies promoted to class statics), change the marker
//	to FINLINE.
#	define	FINLINE_PB

// INTEGER stuff
typedef	uint8_t		UINT8;
typedef	int8_t		INT8;		  								   
								  								   
typedef	uint16_t	UINT16;		  								   
typedef	int16_t		INT16;		  								   
								  								   
typedef	uint32_t	UINT32;	  								   
typedef	int32_t		INT32;		  								   
								  								   
typedef	uint64_t	UINT64;		  								   
typedef	int64_t		INT64;		  								   

//todo remove and do PARAM_TYPE too
//these type should be use to tag already value in 32 bits which should move to 64 bits at some point
//	param should first be extended to 64 bits type
typedef	UINT32					UINT64_SOON;

typedef	unsigned long			DWORD;

#ifdef _MSC_VER
#	if _MSC_VER >= 1700	//(vs2013/v120 tools)
//next include is a pb with toolset v140 (VS 2015)
// maximum signed 64 bit value
#		ifndef INTMAX_MAX
#			define INTMAX_MAX 9223372036854775807i64
#		endif
// minimum signed 64 bit value
#		ifndef INTMAX_MIN
#			define INTMAX_MIN ((int64_t)(-INTMAX_MAX - 1))
#		endif
#	endif
#endif

// FLOAT stuff
typedef	float		FP32;	// used for GOL color for example
typedef	double		DOUBLE;

#ifndef	AAA_REAL_IS_DOUBLE
#	define	AAA_REAL_IS_DOUBLE()	0
#endif	

#if	AAA_REAL_IS_DOUBLE()
	typedef	DOUBLE	REAL;
#else
	typedef	FP32	REAL;
#endif

// Floating point 16 bits used by graphic card
#define HALF_FLOAT_USE() 0
#if HALF_FLOAT_USE()==0
#	ifndef HALF_HALF_HPP
#		include "HalfFloat/half.hpp"
#	endif
	typedef half_float::half FP16;
//#	typedef UINT16 FP16;
#elif HALF_FLOAT_USE()==1
#	ifndef _HALF_H_
#		include "OpenEXR/half.h"
#	endif
	typedef half FP16;
#endif

namespace aaa
{
template< typename T >	CONSTEXPR T LOWEST		= std::numeric_limits<T>::lowest();
template< typename T >	CONSTEXPR T BIGGEST		= std::numeric_limits<T>::max();
template< typename T >	CONSTEXPR T	QUIET_NAN	= std::numeric_limits<T>::quiet_NaN();
}

// CHARACTER types
typedef	char					CHAR;
typedef	CHAR	*				PCHAR;
typedef	CHAR	CONST *			C_PCHAR;	//todo rename this P_CCHAR ?
typedef CHAR	CONST * CONST	C_PCHAR_C;	//todo rename this CP_CCHAR ?

typedef	wchar_t					WCHAR;
typedef	WCHAR	*				PWCHAR;
typedef	WCHAR	CONST *			C_PWCHAR;	//todo rename this P_CWCHAR ?
typedef	WCHAR	CONST * CONST	C_PWCHAR_C;	//todo rename this P_CWCHAR ?

typedef	void	*				P_VOID;
typedef	void	CONST *			P_CVOID;
typedef	void		  * CONST	CP_VOID;	
typedef	void	CONST * CONST	CP_CVOID;


// ERROR //todo refined
//
typedef	INT32	AAA_ERR;
CONSTEXPR AAA_ERR	AAA_OK					=	0	;
CONSTEXPR AAA_ERR	ERR_ANY					=	-1	;
CONSTEXPR AAA_ERR	ERR_UNIMPLEMENTED_YET	=	-2	;
CONSTEXPR AAA_ERR	ERR_NOT_COMPILED		=	-3	;
CONSTEXPR AAA_ERR	ERR_OBJ_NULL			=	-4	;
CONSTEXPR AAA_ERR	ERR_OUT_OF_BOUND		=	-5	;
CONSTEXPR AAA_ERR	ERR_HACK				=	-6	;	//something quickly done not there to stay
CONSTEXPR AAA_ERR	ERR_LOCKED				=	-7	;
CONSTEXPR AAA_ERR	ERR_DATA_INCOHERENT		=	-8	;
CONSTEXPR AAA_ERR	ERR_DATA_SKIPPED		=	-9	;	//bdd_line_3d ok but can't be processed
CONSTEXPR AAA_ERR	ERR_STR_EMPTY			=	-10	;
CONSTEXPR AAA_ERR	ERR_TYPE_UNUSED			=	-11	;
CONSTEXPR AAA_ERR	ERR_CANCEL				=	-12	;

#define	NOERR(c)	( (c) == AAA_OK )
#define	ERR(c)		( !NOERR(c) )

extern C_PCHAR_C ERR_GET_STR( AAA_ERR err_code );

//when we want the next == test against the value to fail
CONSTEXPR void init_bool_to_invalid( bool& b )
{
	CHAR* pt = (CHAR*)&b;
	*pt = 42;
}

//we do not use anymore
//typedef	FP32	RGBA[5];
//typedef	FP32	RGBAF[5];			
//typedef	FP32	FLOAT2x4[4][2];
//typedef	FP32	FLOAT3x3[3][3];
//typedef	FP32	FLOAT3x4[4][3];
//typedef	FP32	FLOAT3x8[8][3];
//typedef	FP32	FLOAT4x4[4][4];

CONSTEXPR	REAL	REAL_BIG_VALUE	= std::numeric_limits<REAL>::max();
CONSTEXPR	FP32	FP32_BIG_VALUE	= std::numeric_limits<FP32>::max();

#ifdef _restore_max_
#	pragma pop_macro("max")
#	undef _restore_max_
#endif


// useful macros
#ifndef	NULL
#	ifdef	__cplusplus
#		define	NULL	nullptr
#	else
#		define	NULL	((void *)0)
#	endif
#endif

//todo IF_THIS_NULL is not koscher it is UB. function with MSVC but no standard c++ (can call a method on a null object, should check at caller level)
extern	void	PRINT_THIS_NULL( C_PCHAR_C signature	);	
#define IF_THIS_NULL()				if( !this )
#define IF_THIS_NULL_RETURN()		{	IF_THIS_NULL() { PRINT_THIS_NULL( __FUNCTION__); return;			}	}
#define IF_THIS_NULL_RETURN_FALSE()	{	IF_THIS_NULL() { PRINT_THIS_NULL( __FUNCTION__); return false;		}	}
#define IF_THIS_NULL_RETURN_TRUE()	{	IF_THIS_NULL() { PRINT_THIS_NULL( __FUNCTION__); return true;		}	}
#define IF_THIS_NULL_RETURN_NULL()	{	IF_THIS_NULL() { PRINT_THIS_NULL( __FUNCTION__); return nullptr;	}	}

// from https://coderwall.com/p/nb9ngq/better-getting-array-size-in-c
template< INT32 SIZE, class T > CONSTEXPR INT32 ARRAY_SIZE( T (&arr)[SIZE] ) { return SIZE; }


// FUNCTION types
#ifndef MAAAPIENTRY
#	define	MAAAPIENTRY	__cdecl
#endif
#ifndef MAACALLBACK
#	define	MAACALLBACK	__cdecl
#endif
typedef	void (MAACALLBACK MENU_FN)(int);
typedef	MENU_FN*			PT_MENU_FN;


// C++ cooking
#define C_NO_CPY_MOVE( cla )				\
	cla(cla && p_rhs) = delete;				\
	cla(cla const &) = delete;				\
	cla & operator=(cla &&) = delete;		\
	cla & operator=(cla const &) = delete;

//#pragma message( "__cplusplus is " AAA_STRING(__cplusplus) ) 
//#if defined(__cplusplus) && __cplusplus >= 201703L
#define C_DEF_CTOR_DTOR( cla )		\
	cla(void) = default;			\
	~cla(void) NOEXCEPT = default;
//#else
//#define C_DEF_CTOR_DTOR( cla )	\
//	cla(void) = default;			\
//	~cla(void) NOEXCEPT = default;
//#endif


//
// Single-byte access (portable, constexpr)
//
#if AAA_ENDIAN_LITTLE()
// Little-endian mapping
constexpr UINT8 GET_BYTE_INDEX(UINT32 x, INT32 index ) { return static_cast<UINT8>(x >> (index<<3)); }
constexpr UINT8 GET_BYTE_0(UINT32 x) { return static_cast<UINT8>(x >> 0); }
constexpr UINT8 GET_BYTE_1(UINT32 x) { return static_cast<UINT8>(x >> 8); }
constexpr UINT8 GET_BYTE_2(UINT32 x) { return static_cast<UINT8>(x >> 16); }
constexpr UINT8 GET_BYTE_3(UINT32 x) { return static_cast<UINT8>(x >> 24); }

constexpr void SET_BYTE_0(UINT32 &x, UINT8 v) { x = (x & 0xFFFFFF00u) | (static_cast<UINT32>(v) << 0); }
constexpr void SET_BYTE_1(UINT32 &x, UINT8 v) { x = (x & 0xFFFF00FFu) | (static_cast<UINT32>(v) << 8); }
constexpr void SET_BYTE_2(UINT32 &x, UINT8 v) { x = (x & 0xFF00FFFFu) | (static_cast<UINT32>(v) << 16); }
constexpr void SET_BYTE_3(UINT32 &x, UINT8 v) { x = (x & 0x00FFFFFFu) | (static_cast<UINT32>(v) << 24); }

// Pack/unpack RGBA or BGRA from/to UINT32
constexpr UINT32 PACK_RGB(	UINT8 r, UINT8 g, UINT8 b )
{ return (static_cast<UINT32>(r) << 0) | (static_cast<UINT32>(g) << 8) | (static_cast<UINT32>(b) << 16); }
constexpr UINT32 PACK_RGBA(	UINT8 r, UINT8 g, UINT8 b, UINT8 a)
{ return (static_cast<UINT32>(r) << 0) | (static_cast<UINT32>(g) << 8) | (static_cast<UINT32>(b) << 16) | (static_cast<UINT32>(a) << 24); }
constexpr UINT32 PACK_BGR(	UINT8 r, UINT8 g, UINT8 b)
{ return (static_cast<UINT32>(b) << 0) | (static_cast<UINT32>(g) << 8) | (static_cast<UINT32>(r) << 16); }
constexpr UINT32 PACK_BGRA(	UINT8 r, UINT8 g, UINT8 b, UINT8 a)
{ return (static_cast<UINT32>(b) << 0) | (static_cast<UINT32>(g) << 8) | (static_cast<UINT32>(r) << 16) | (static_cast<UINT32>(a) << 24); }

#else
// Big-endian mapping
constexpr UINT8 GET_BYTE_INDEX(UINT32 x, INT32 index ) { return static_cast<UINT8>(x >> ((3-index)<<3)); }
constexpr UINT8 GET_BYTE_0(UINT32 x) { return static_cast<UINT8>(x >> 24); }
constexpr UINT8 GET_BYTE_1(UINT32 x) { return static_cast<UINT8>(x >> 16); }
constexpr UINT8 GET_BYTE_2(UINT32 x) { return static_cast<UINT8>(x >> 8); }
constexpr UINT8 GET_BYTE_3(UINT32 x) { return static_cast<UINT8>(x >> 0); }

constexpr void SET_BYTE_0(UINT32 &x, UINT8 v) { x = (x & 0x00FFFFFFu) | (static_cast<UINT32>(v) << 24); }
constexpr void SET_BYTE_1(UINT32 &x, UINT8 v) { x = (x & 0xFF00FFFFu) | (static_cast<UINT32>(v) << 16); }
constexpr void SET_BYTE_2(UINT32 &x, UINT8 v) { x = (x & 0xFFFF00FFu) | (static_cast<UINT32>(v) << 8); }
constexpr void SET_BYTE_3(UINT32 &x, UINT8 v) { x = (x & 0xFFFFFF00u) | (static_cast<UINT32>(v) << 0); }

// Pack/unpack RGBA or BGRA from/to UINT32
constexpr UINT32 PACK_RGB(	UINT8 r, UINT8 g, UINT8 b)
{ return (static_cast<UINT32>(r) << 24) | (static_cast<UINT32>(g) << 16) | (static_cast<UINT32>(b) << 8); }
constexpr UINT32 PACK_RGBA(	UINT8 r, UINT8 g, UINT8 b, UINT8 a)
{ return (static_cast<UINT32>(r) << 24) | (static_cast<UINT32>(g) << 16) | (static_cast<UINT32>(b) << 8) | (static_cast<UINT32>(a) << 0); }
constexpr UINT32 PACK_BGR(	UINT8 r, UINT8 g, UINT8 b)
{ return (static_cast<UINT32>(b) << 24) | (static_cast<UINT32>(g) << 16) | (static_cast<UINT32>(r) << 8); }
constexpr UINT32 PACK_BGRA(	UINT8 r, UINT8 g, UINT8 b, UINT8 a)
{ return (static_cast<UINT32>(b) << 24) | (static_cast<UINT32>(g) << 16) | (static_cast<UINT32>(r) << 8) | (static_cast<UINT32>(a) << 0); }

#endif

template< typename T >
constexpr void UNPACK_UINT32( const UINT32 x, T &r, T &g, T &b, T &a )
{
    r = GET_BYTE_0(x);
    g = GET_BYTE_1(x);
    b = GET_BYTE_2(x);
    a = GET_BYTE_3(x);
}
constexpr void UNPACK_RGBA( const UINT32 x, UINT8 &r, UINT8 &g, UINT8 &b, UINT8 &a )	{ UNPACK_UINT32( x, r,g,b, a ); }
constexpr void UNPACK_BGRA( const UINT32 x, UINT8 &b, UINT8 &g, UINT8 &r, UINT8 &a )	{ UNPACK_UINT32( x, b,g,r, a ); }


// we need it here to call it in .h without including err.h
//	this fn display a debug message in the terminal window and provoke a break if debugger is on
extern void debug_break( C_PCHAR_C fmt = nullptr, ... );
