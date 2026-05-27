
#ifdef AAA_NATIVE_TYPES_H
#error "NATIVE_TYPES_H included more than once."
#endif
#define AAA_NATIVE_TYPES_H 1


#ifndef AAA_NATIVE_SYSTEM_H
	#include "Native_System.h"
#endif
#if defined(__APPLE__)
	#include <stddef.h>     // needed for "size_t" type on mac os
#endif


/*===================================================================================================*/
/*	Windows Type Name	!	Bytes	! Other Type Names(Unix)!           Range of Values				!*/
/*--------------------------------------------------------------------------------------------------!*/
/*	int					!	4		!	signed				!	–2,147,483,648 to 2,147,483,647		!*/
/*--------------------------------------------------------------------------------------------------!*/
/*	unsigned int		!	4		!	unsigned			!	0 to 4,294,967,295					!*/
/*--------------------------------------------------------------------------------------------------!*/
/*	__int8				!	1		!	char				!	–128 to 127							!*/
/*--------------------------------------------------------------------------------------------------!*/
/*	unsigned __int8		!	1		!	unsigned char		!	0 to 255							!*/
/*--------------------------------------------------------------------------------------------------!*/
/*	__int16				!	2		!	short,				!	–32,768 to 32,767					!*/
/*						!			!	short int,			!										!*/
/*						!			!	signed short int	!										!*/
/*--------------------------------------------------------------------------------------------------!*/
/*	unsigned __int16	!	2		!	unsigned short,		!	0 to 65,535							!*/
/*						!			!	unsigned short int	!										!*/
/*--------------------------------------------------------------------------------------------------!*/
/*	__int32				!	4		!	signed,				!	 –2,147,483,648 to 2,147,483,647	!*/
/*						!			!	signed int,			!										!*/
/*						!			!	int					!										!*/
/*--------------------------------------------------------------------------------------------------!*/
/*	unsigned __int32	!	4		!	unsigned,			!	0 to 4,294,967,295					!*/
/*						!			!	unsigned int		!										!*/
/*--------------------------------------------------------------------------------------------------!*/
/*	__int64				!	8		!	long long,			!	–9,223,372,036,854,775,808			!*/
/*						!			!	signed long long	!	to 9,223,372,036,854,775,807		!*/
/*--------------------------------------------------------------------------------------------------!*/
/*	unsigned __int64	!	8		!	unsigned long long	!	0 to 18,446,744,073,709,551,615		!*/
/*--------------------------------------------------------------------------------------------------!*/
/*	bool				!	1		!	none				!	false or true						!*/
/*--------------------------------------------------------------------------------------------------!*/
/*	char				!	1		!	none				!	–128 to 127 by default				!*/
/*						!			!						!	0 to 255 when compiled with /J		!*/
/*--------------------------------------------------------------------------------------------------!*/
/*	signed char			!	1		!	none				!	–128 to 127							!*/
/*--------------------------------------------------------------------------------------------------!*/
/*	unsigned char		!	1		!	none				!	0 to 255							!*/
/*--------------------------------------------------------------------------------------------------!*/
/*	short				!	2		!	short int,			!	–32,768 to 32,767					!*/
/*						!			!	signed short int	!										!*/
/*--------------------------------------------------------------------------------------------------!*/
/*	unsigned short		!	2		!	unsigned short int	!	0 to 65,535							!*/
/*--------------------------------------------------------------------------------------------------!*/
/*	long				!	4		!	long int,			!	–2,147,483,648 to 2,147,483,647		!*/
/*						!			!	signed long int		!										!*/
/*--------------------------------------------------------------------------------------------------!*/
/*	unsigned long		!	4		!	unsigned long int	!	0 to 4,294,967,295					!*/
/*--------------------------------------------------------------------------------------------------!*/
/*	long long			!	8		!	none (equivalent	!	–9,223,372,036,854,775,808			!*/
/*						!			!	 to __int64)		!	to 9,223,372,036,854,775,807		!*/
/*--------------------------------------------------------------------------------------------------!*/
/*	unsigned long long	!	8		!	none (equivalent	!	0									!*/
/*						!			!	to unsigned __int64)!	to 18,446,744,073,709,551,615		!*/
/*--------------------------------------------------------------------------------------------------!*/
/*	float				!	4		!	none				!	3.4E +/- 38 (7 digits)				!*/
/*--------------------------------------------------------------------------------------------------!*/
/*	double				!	8		!	none				!	1.7E +/- 308 (15 digits)			!*/
/*--------------------------------------------------------------------------------------------------!*/
/*	long double			!	same	!	none				!	same as double						!*/
/*						!	as		!						!										!*/
/*						!	double	!						!										!*/
/*--------------------------------------------------------------------------------------------------!*/
/*	wchar_t				!	2		!	__wchar_t			!	0 to 65,535							!*/
/*===================================================================================================*/ 


#include "inttypes.h"

#ifdef _MSC_VER

#include <cstddef>

#else /* _MSC_VER */

#ifndef _MSC_STDINT_H_
#include <stdint.h>
#endif

#endif /* _MSC_VER */

/* Sys type*/
# if HAVE_SYS_TYPES_H
#  include <sys/types.h>
# endif

/********************/
/* Basic data types */
/********************/
/* grab a few fixed-width types to insure compliance with C99*/

//! 8 bit unsigned variable.
/** This is a typedef for unsigned char, it ensures portability of the engine. */
#ifdef _MSC_VER
typedef unsigned __int8			NATIVE_Ubyte8_t;
#else
typedef unsigned char			NATIVE_Ubyte8_t;
#endif

//! 8 bit unsigned variable.
/** This is a typedef for unsigned char, it ensures portability of the engine. */
#ifdef _MSC_VER
typedef unsigned __int8			NATIVE_Uchar8_t;
#else
typedef unsigned char			NATIVE_Uchar8_t;
#endif

//! 8 bit signed variable.
/** This is a typedef for signed char, it ensures portability of the engine. */
#ifdef _MSC_VER
typedef __int8					NATIVE_Schar8_t;
#else
typedef signed char				NATIVE_Schar8_t;
#endif

//! 8 bit character variable.
/** This is a typedef for char, it ensures portability of the engine. */
#ifdef _MSC_VER
typedef __int8					NATIVE_char8_t;
#else
typedef char					NATIVE_char8_t;
#endif

//! const 8 bit character variable.
/** This is a typedef for char, it ensures portability of the engine. */
#ifdef _MSC_VER
typedef const __int8			NATIVE_Cchar8_t;
#else
typedef const char				NATIVE_Cchar8_t;
#endif

//! 8 bit unsigned int.
/** This is a typedef for unsigned char, it ensures portability of the engine. */
#ifdef _MSC_VER
typedef unsigned __int8			NATIVE_Uint8_t;
#else
typedef unsigned char			NATIVE_Uint8_t;
#endif

//! 8 bit signed int.
/** This is a typedef for signed char, it ensures portability of the engine. */
#ifdef _MSC_VER
typedef __int8					NATIVE_Sint8_t;
#else
typedef signed char				NATIVE_Sint8_t;
#endif

//! 16 bit unsigned variable.
/** This is a typedef for unsigned short, it ensures portability of the engine. */
#ifdef _MSC_VER
typedef unsigned __int16		NATIVE_Ushort16_t;
#else
typedef unsigned short int		NATIVE_Ushort16_t;
#endif

//! 16 bit signed variable.
/** This is a typedef for signed short, it ensures portability of the engine. */
#ifdef _MSC_VER
typedef __int16					NATIVE_Sshort16_t;
#else
typedef signed short int		NATIVE_Sshort16_t;
#endif

//! 32 bit unsigned variable.
/** This is a typedef for unsigned int, it ensures portability of the engine. */
#ifdef _MSC_VER
typedef unsigned __int32		NATIVE_Uint32_t;
#else
typedef unsigned int			NATIVE_Uint32_t;
#endif

//! 32 bit signed variable.
/** This is a typedef for signed int, it ensures portability of the engine. */
#ifdef _MSC_VER
typedef __int32					NATIVE_Sint32_t;
#else
typedef signed int				NATIVE_Sint32_t;
#endif

// !64 bit signed variable.
#ifdef _MSC_VER
typedef __int64					NATIVE_Sint64_t;
#else
typedef signed long long int	NATIVE_Sint64_t;
#endif

// !64 bit unsigned variable.
#ifdef _MSC_VER
typedef unsigned __int64		NATIVE_Uint64_t;
#else
typedef unsigned long long int	NATIVE_Uint64_t;
#endif

//! 32 bit floating point variable.
/** This is a typedef for float, it ensures portability of the engine. */
typedef float					NATIVE_float32_t;

//! 64 bit floating point variable.
/** This is a typedef for double, it ensures portability of the engine. */
typedef double					NATIVE_float64_t;

//! 128 bit floating point variable.
/** This is a typedef for double, it ensures portability of the engine. */
typedef long double				NATIVE_float128_t;

/** 
 *
 *		CHAR CLASS
 *		This class contains information for individual chars
 *		x1,x2,y1,y2 are the texture coordinates, width, A, and C 
 *		the remaining char properties
 *
 **************************************************************************************************/ 

class NATIVE_Char32_t
{
public:
	// TexCoords
	NATIVE_float32_t x1, x2, y1, y2;
	/// Char width
	NATIVE_Uint32_t width;
	NATIVE_Uint32_t A, C;
};	


/*************/
/* Constants */
/*************/

/* Booleans */
#define NATIVE_TRUE  (1==1)
#define NATIVE_FALSE (1==0)

/* File reading modes */
#define NATIVE_TEXT_MODE   1
#define NATIVE_BINARY_MODE 2

/* Max line length when doing text i/o */
#define NATIVE_MAX_STR_LEN 1024

/* Useful math constants */
#define NATIVE_PI    3.14159265358979323846
#define NATIVE_2PI   6.28318530717958647692

#define NATIVE_PI_SQ     9.8696044010893579923049401
#define NATIVE_INV_PI_SQ 0.1013211836423377754101693
/* Note: the two above constants have been computed with doubles
   and copied from a printf( "%.25f", x ) callback_display. Their accuracy
   could probably be enhanced.*/

#ifndef PI
	#define PI       3.14159265358979323846
#endif

#ifndef TWO_PI
	#define TWO_PI   6.28318530717958647693
#endif

#ifndef M_TWO_PI
	#define M_TWO_PI   6.28318530717958647693
#endif

#ifndef FOUR_PI
	#define FOUR_PI 12.56637061435917295385
#endif

#ifndef HALF_PI
	#define HALF_PI  1.57079632679489661923
#endif


template <typename T>
inline const T &tMin(const T &a, const T &b) { return (a < b) ? a : b; }
template <typename T>
inline const T &tMax(const T &a, const T &b) { return (a < b) ? b : a; }
template <typename T>
inline const T &tBound(const T &min, const T &val, const T &max)
{ return tMax(min, tMin(max, val)); }

//==============================================================================
/** This namespace contains a few template classes for helping work out class type variations.
*/
namespace TypeHelpers
{
/** The ParameterType struct is used to find the best type to use when passing some kind
		of object as a parameter.

		Of course, this is only likely to be useful in certain esoteric template situations.

		Because "typename TypeHelpers::ParameterType<SomeClass>::type" is a bit of a mouthful, there's
		a PARAMETER_TYPE(SomeClass) macro that you can use to get the same effect.

		E.g. "myFunction (PARAMETER_TYPE (int), PARAMETER_TYPE (MyObject))"
		would evaluate to "myfunction (int, const MyObject&)", keeping any primitive types as
		pass-by-value, but passing objects as a const reference, to avoid copying.
	*/
	template <typename Type> struct ParameterType                   { typedef const Type& type; };

	//template <typename Type> struct ParameterType <Type&>           { typedef Type& type; };
	//template <typename Type> struct ParameterType <Type*>           { typedef Type* type; };
	//template <>              struct ParameterType <char>            { typedef char type; };
	//template <>              struct ParameterType <unsigned char>   { typedef unsigned char type; };
	//template <>              struct ParameterType <short>           { typedef short type; };
	//template <>              struct ParameterType <unsigned short>  { typedef unsigned short type; };
	//template <>              struct ParameterType <int>             { typedef int type; };
	//template <>              struct ParameterType <unsigned int>    { typedef unsigned int type; };
	//template <>              struct ParameterType <long>            { typedef long type; };
	//template <>              struct ParameterType <unsigned long>   { typedef unsigned long type; };
	//template <>              struct ParameterType <NATIVE_Sint64_t>    { typedef NATIVE_Sint64_t type; };
	//template <>              struct ParameterType <NATIVE_Uint64_t>    { typedef NATIVE_Uint64_t type; };
	//template <>              struct ParameterType <bool>            { typedef bool type; };
	//template <>              struct ParameterType <float>           { typedef float type; };
	//template <>              struct ParameterType <double>          { typedef double type; };

	/** A helpful macro to simplify the use of the ParameterType template.
		@see ParameterType
	*/
	#define PARAMETER_TYPE(a)    typename TypeHelpers::ParameterType<a>::type
}

class CustomFlag
{
	NATIVE_Sint32_t i;
public:
	inline CustomFlag(NATIVE_Sint32_t i);
	inline operator NATIVE_Sint32_t() const { return i; }
};

inline CustomFlag::CustomFlag(NATIVE_Sint32_t ai) : i(ai) {}

class  IncompatibleCustomFlag
{
	NATIVE_Sint32_t i;
public:
	inline explicit IncompatibleCustomFlag(NATIVE_Sint32_t i);
	inline operator NATIVE_Sint32_t() const { return i; }
};

inline IncompatibleCustomFlag::IncompatibleCustomFlag(NATIVE_Sint32_t ai) : i(ai) {}

template<typename Enum>
class CustomFlags
{
	typedef void **Zero;
	int32_t i;
public:
	typedef Enum enum_type;
	inline CustomFlags(const CustomFlags &f) : i(f.i) {}
	inline CustomFlags(Enum f) : i(f) {}
	inline CustomFlags(Zero = 0) : i(0) {}
	inline CustomFlags(CustomFlag f) : i(f) {}

	inline CustomFlags &operator=(const CustomFlags &f) { i = f.i; return *this; }
	inline CustomFlags &operator&=(int32_t mask) { i &= mask; return *this; }
	inline CustomFlags &operator&=(uint32_t mask) { i &= mask; return *this; }
	inline CustomFlags &operator|=(CustomFlags f) { i |= f.i; return *this; }
	inline CustomFlags &operator|=(Enum f) { i |= f; return *this; }
	inline CustomFlags &operator^=(CustomFlags f) { i ^= f.i; return *this; }
	inline CustomFlags &operator^=(Enum f) { i ^= f; return *this; }

	inline operator int32_t() const { return i; }

	inline CustomFlags operator|(CustomFlags f) const { return CustomFlags(Enum(i | f.i)); }
	inline CustomFlags operator|(Enum f) const { return CustomFlags(Enum(i | f)); }
	inline CustomFlags operator^(CustomFlags f) const { return CustomFlags(Enum(i ^ f.i)); }
	inline CustomFlags operator^(Enum f) const { return CustomFlags(Enum(i ^ f)); }
	inline CustomFlags operator&(int32_t mask) const { return CustomFlags(Enum(i & mask)); }
	inline CustomFlags operator&(uint32_t mask) const { return CustomFlags(Enum(i & mask)); }
	inline CustomFlags operator&(Enum f) const { return CustomFlags(Enum(i & f)); }
	inline CustomFlags operator~() const { return CustomFlags(Enum(~i)); }

	inline bool operator!() const { return !i; }

	inline bool testFlag(Enum f) const { return (i & f) == f && (f != 0 || i == int(f) ); }
};

#define NATIVE_DECLARE_FLAGS(Flags, Enum)\
typedef CustomFlags<Enum> Flags;

#define NATIVE_DECLARE_INCOMPATIBLE_FLAGS(Flags) \
inline IncompatibleCustomFlag operator|(Flags::enum_type f1, NATIVE_Sint32_t f2) \
{ return IncompatibleCustomFlag(NATIVE_Sint32_t(f1) | f2); }

#define NATIVE_DECLARE_OPERATORS_FOR_FLAGS(Flags) \
inline CustomFlags<Flags::enum_type> operator|(Flags::enum_type f1, Flags::enum_type f2) \
{ return CustomFlags<Flags::enum_type>(f1) | f2; } \
inline CustomFlags<Flags::enum_type> operator|(Flags::enum_type f1, CustomFlags<Flags::enum_type> f2) \
{ return f2 | f1; } NATIVE_DECLARE_INCOMPATIBLE_FLAGS(Flags)

/*****/
/* Bounds of integer types */

const unsigned short int NATIVE_MAX_BITS_SHORT_INT = 8*sizeof(short int) - 1; /* 15 */
const short int NATIVE_MIN_SHORT_INT = (short int)   ( 1 << NATIVE_MAX_BITS_SHORT_INT ); /*-32768*/
const short int NATIVE_MAX_SHORT_INT = (short int) ( ( 1 << NATIVE_MAX_BITS_SHORT_INT ) - 1); /*32767*/

const unsigned short int NATIVE_MAX_BITS_UNSIGNED_SHORT_INT = 8*sizeof(unsigned short int); /* 16 */
const unsigned short int NATIVE_MIN_UNSIGNED_SHORT_INT = 0;
const unsigned short int NATIVE_MAX_UNSIGNED_SHORT_INT = (unsigned short int) (-1); /* 65535 */

const unsigned short int NATIVE_MAX_BITS_LONG_INT = 8*sizeof(long int) - 1; /* 31 */
const long int NATIVE_MIN_LONG_INT = (unsigned long int)   ( (unsigned long int)(1) << NATIVE_MAX_BITS_LONG_INT ); /*-2147483648*/
const long int NATIVE_MAX_LONG_INT = (unsigned long int) ( ( (unsigned long int)(1) << NATIVE_MAX_BITS_LONG_INT ) - (unsigned long int)(1) ); /* 2147483647 */

const unsigned short int NATIVE_MAX_BITS_UNSIGNED_LONG_INT = 8*sizeof(unsigned long int); /* 32 */
const unsigned long int NATIVE_MIN_UNSIGNED_LONG_INT = 0;
const unsigned long int NATIVE_MAX_UNSIGNED_LONG_INT = (unsigned long int) (-1); /* 4294967295 */

const unsigned short int NATIVE_MAX_BITS_SIZE_T = 8*sizeof(size_t);
const size_t NATIVE_MIN_SIZE_T = 0;
const size_t NATIVE_MAX_SIZE_T = (size_t) (-1);

#if defined(NATIVE_WS_MAC)
	typedef void * NATIVE_HANDLE;
#elif defined(NATIVE_WS_WIN)
	typedef void *NATIVE_HANDLE;
#elif defined(NATIVE_WS_X11)
	typedef unsigned long NATIVE_HANDLE;
#elif defined(NATIVE_WS_QWS) || defined(NATIVE_WS_QPA)
	typedef void * NATIVE_HANDLE;
#endif

