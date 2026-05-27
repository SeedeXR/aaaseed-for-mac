#ifdef AAA_FTGL_CONFIG_H
#error "FTGL_CONFIG_H included more than once."
#endif
#define AAA_FTGL_CONFIG_H 1

/*
 * FTGL - OpenGL font library
 *
 * Copyright (c) 2008 Jeff Myers <JeffM2501@users.sourceforge.net>
 * Copyright (c) 2008 Daniel Remenak <dtremenak@users.sourceforge.net>
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

//maa cleaned and optimized in 2025

#if !AAASEED()
// Default font file
#	define FONT_FILE "C:\\Windows\\Fonts\\Arial.ttf"

// GLUT
#	define HAVE_GL_GLUT_H

// M_PI and friends on VC
#	define _USE_MATH_DEFINES	//defined is the command line (under AAASeed)
#endif

// quell spurious "'this': used in base member initializer list" warnings
//#ifdef _MSC_VER
//#	pragma warning(disable: 4355)
//#endif

// quell spurious portable-function deprecation warnings
//#define _CRT_SECURE_NO_DEPRECATE 1
//#define _POSIX_ 1


#if AAASEED()
#	ifndef AAA_AAA_TYPE_H
#		include "aaa_type.h"
#	endif
#endif

namespace FTGL
{
#if AAASEED()
	CONSTEXPR INT32		size_factor_shift		= 6;	//because of format FT_F26Dot6
	CONSTEXPR FP32		size_factor				= FP32( 1 << size_factor_shift );
	CONSTEXPR FP32		size_outside			= size_factor;	
	CONSTEXPR FP32		size_factor_over		= 1. / size_factor;
	CONSTEXPR DOUBLE	size_factor_over_double = 1. / size_factor;
	//#	ifndef	PTRDIFF_MAX
	//#	include <intsafe.h>
	//#		define INT64_MAX		9223372036854775807i64
	//#		define INTPTR_MAX		INT64_MAX
	//#		define INTMAX_MAX		INT64_MAX
	//#		define PTRDIFF_MAX      INTPTR_MAX
	//#	endif
	typedef UINT32 VECTOR_SIZE_TYPE;
#else
	typedef size_t VECTOR_SIZE_TYPE;
#endif
}

// use __FUNCTION__
#define __FUNC__ __FUNCTION__

#define PACKAGE_VERSION "2.4.0 AAASeed"

#define FTGL_LIBRARY_STATIC 1



