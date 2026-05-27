
#include "aaa_mem.h"
#include <memory.h>
#include "err.h"
#include "cpu.h"
#include "spy.h"
#include "infrastructure/compute_parallel.h"

//we were using at some point stuff from
// https://www.agner.org/optimize/asmlib-instructions.pdf
//todo any good in this link ?


void	mem::memmove( void* CONST dst, void CONST * CONST src, size_t CONST nb ) NOEXCEPT
{
	if( dst && src )
		::memmove( dst, src, nb );
	else
		debug_break( "MEMMOVE() can't deal with a Null src or dst" );
}

#if !AAA_WIN64()
/////////////////////////////////////////////////////////////////////////////
// memcpyMMX
// Uses MMX instructions to move memory around
// does as much as we can in 64 byte chunks (128-byte on SSE machines)
// using MMX instructions
// then copies any extra bytes
// assumes there will be at least 64 bytes to copy
// This code was originally from Borg's bTV plugin SDK 
/////////////////////////////////////////////////////////////////////////////
FINLINE	void*	memcpyMMX( void *Dest, const void *Src, size_t nBytes ) NOEXCEPT
{
	__asm
	{
		mov		esi, dword ptr[Src]
		mov		edi, dword ptr[Dest]
		mov		ecx, nBytes
		shr		ecx, 6			// nBytes / 64
		align 8
CopyLoop:
		movq	mm0, qword ptr[esi]
		movq	mm1, qword ptr[esi+8*1]
		movq	mm2, qword ptr[esi+8*2]
		movq	mm3, qword ptr[esi+8*3]
		movq	mm4, qword ptr[esi+8*4]
		movq	mm5, qword ptr[esi+8*5]
		movq	mm6, qword ptr[esi+8*6]
		movq	mm7, qword ptr[esi+8*7]
		movq	qword ptr[edi], mm0
			movq	qword ptr[edi+8*1], mm1
			movq	qword ptr[edi+8*2], mm2
			movq	qword ptr[edi+8*3], mm3
			movq	qword ptr[edi+8*4], mm4
			movq	qword ptr[edi+8*5], mm5
			movq	qword ptr[edi+8*6], mm6
			movq	qword ptr[edi+8*7], mm7
			add		esi, 64
			add		edi, 64
			loop CopyLoop
			mov		ecx, nBytes
			and		ecx, 63
			cmp		ecx, 0
			je EndCopyLoop
			align 8
CopyLoop2:
		mov dl, byte ptr[esi] 
		mov byte ptr[edi], dl
			inc esi
			inc edi
			dec ecx
			jne CopyLoop2
EndCopyLoop:
		emms
	}
}

/////////////////////////////////////////////////////////////////////////////
// memcpySSE
// On SSE machines, we can use the 128-bit floating-point registers and
// bypass write caching to copy a bit faster. The destination has to be
// 16-byte aligned.
/////////////////////////////////////////////////////////////////////////////
FINLINE	void*	memcpySSE( void* Dest, CONST void* Src, size_t nBytes ) NOEXCEPT
{
	__asm
	{
		mov		esi, dword ptr[Src]
		mov		edi, dword ptr[Dest]
		mov		ecx, nBytes
			shr		ecx, 7			// nBytes / 128
			align 8
CopyLoopSSE:
		// movaps should be slightly more efficient
		// as the data is 16 bit aligned
		movaps	xmm0, xmmword ptr[esi]
		movaps	xmm1, xmmword ptr[esi+16*1]
		movaps	xmm2, xmmword ptr[esi+16*2]
		movaps	xmm3, xmmword ptr[esi+16*3]
		movaps	xmm4, xmmword ptr[esi+16*4]
		movaps	xmm5, xmmword ptr[esi+16*5]
		movaps	xmm6, xmmword ptr[esi+16*6]
		movaps	xmm7, xmmword ptr[esi+16*7]
		movntps	xmmword ptr[edi], xmm0
			movntps	xmmword ptr[edi+16*1], xmm1
			movntps	xmmword ptr[edi+16*2], xmm2
			movntps	xmmword ptr[edi+16*3], xmm3
			movntps	xmmword ptr[edi+16*4], xmm4
			movntps	xmmword ptr[edi+16*5], xmm5
			movntps	xmmword ptr[edi+16*6], xmm6
			movntps	xmmword ptr[edi+16*7], xmm7
			add		esi, 128
			add		edi, 128
			loop CopyLoopSSE
			mov		ecx, nBytes
			and		ecx, 127
			cmp		ecx, 0
			je EndCopyLoopSSE
			align 8
CopyLoop2SSE:
		mov dl, byte ptr[esi] 
		mov byte ptr[edi], dl
			inc esi
			inc edi
			dec ecx
			jne CopyLoop2SSE
EndCopyLoopSSE:
		emms
	}
}


///* for small memory blocks (<256 bytes) this version is faster */
//#define small_memcpy(to,from,n)\
//{\
//register unsigned long int dummy;\
//__asm volatile (\
//	"rep; movsb"\
//	:"=&D"(to), "=&S"(from), "=&c"(dummy)\
//	:"0" (to), "1" (from),"2" (n)\
//	: "memory");\
//}
//#define SSE_MMREG_SIZE 16
//#define MMX_MMREG_SIZE 8
//
//#define MMX1_MIN_LEN 0x800	/* 2K blocks */
//#define MIN_LEN 0x40			/* 64-byte blocks */
//
///* SSE note: i tried to move 128 bytes a time instead of 64 but it
//didn't make any measureable difference. i'm using 64 for the sake of
//simplicity. [MF] */
//static void * sse_memcpy(void * to, CONST void * from, size_t len)
//{
//	void *retval;
//	size_t i;
//	retval = to;
//
//	/* PREFETCH has effect even for MOVSB instruction ;) */
//	_asm volatile (
//		"   prefetchnta (%0)\n"
//		"   prefetchnta 64(%0)\n"
//		"   prefetchnta 128(%0)\n"
//		"   prefetchnta 192(%0)\n"
//		"   prefetchnta 256(%0)\n"
//		: : "r" (from) );
//    
//  if(len >= MIN_LEN)
//  {
//    register unsigned long int delta;
//    /* Align destination to MMREG_SIZE -boundary */
//    delta = ((unsigned long int)to)&(SSE_MMREG_SIZE-1);
//    if(delta)
//    {
//      delta=SSE_MMREG_SIZE-delta;
//      len -= delta;
//      small_memcpy(to, from, delta);
//    }
//    i = len >> 6; /* len/64 */
//    len&=63;
//    if(((unsigned long)from) & 15)
//      /* if SRC is misaligned */
//      for(; i>0; --i )
//      {
//        __asm volatile (
//        "prefetchnta 320(%0)\n"
//        "movups (%0), %%xmm0\n"
//        "movups 16(%0), %%xmm1\n"
//        "movups 32(%0), %%xmm2\n"
//        "movups 48(%0), %%xmm3\n"
//        "movntps %%xmm0, (%1)\n"
//        "movntps %%xmm1, 16(%1)\n"
//        "movntps %%xmm2, 32(%1)\n"
//        "movntps %%xmm3, 48(%1)\n"
//        :: "r" (from), "r" (to) : "memory");
//        ((CONST unsigned char *)from)+=64;
//        ((unsigned char *)to)+=64;
//      }
//    else 
//      /*
//         Only if SRC is aligned on 16-byte boundary.
//         It allows to use movaps instead of movups, which required data
//         to be aligned or a general-protection exception (#GP) is generated.
//      */
//      for(; i>0; --i )
//      {
//        _asm volatile (
//        "prefetchnta 320(%0)\n"
//        "movaps (%0), %%xmm0\n"
//        "movaps 16(%0), %%xmm1\n"
//        "movaps 32(%0), %%xmm2\n"
//        "movaps 48(%0), %%xmm3\n"
//        "movntps %%xmm0, (%1)\n"
//        "movntps %%xmm1, 16(%1)\n"
//        "movntps %%xmm2, 32(%1)\n"
//        "movntps %%xmm3, 48(%1)\n"
//        :: "r" (from), "r" (to) : "memory");
//        ((CONST unsigned char *)from)+=64;
//        ((unsigned char *)to)+=64;
//      }
//    /* since movntq is weakly-ordered, a "sfence"
//     * is needed to become ordered again. */
//    __asm volatile ("sfence":::"memory");
//    /* enables to use FPU */
//    __asm volatile ("emms":::"memory");
//  }
//  /*
//   *	Now do the tail of the block
//   */
//  if(len) __memcpy(to, from, len);
//  return retval;
//}

///* for small memory blocks (<256 bytes) this version is faster */
//#define small_memcpy(to,from,n)\
//{\
//register unsigned long int dummy;\
//_asm volatile(\
//	"rep; movsb"\
//	:"=&D"(to), "=&S"(from), "=&c"(dummy)\
///* It's most portable way to notify compiler */\
///* that edi, esi and ecx are clobbered in asm block. */\
///* Thanks to A'rpi for hint!!! */\
//        :"0" (to), "1" (from),"2" (n)\
//	: "memory");\
//}

void*	small_memcpy( void * p_dst, CONST void * p_src, size_t len ) NOEXCEPT
{
	_asm
	{
		cld
			mov esi, p_src
			mov edi, p_dst
			mov ecx, len

			shr ecx, 2
			rep movsd

			mov ecx, len
			and ecx, 3
			rep movsb
	}
}
#endif //#if !AAA_WIN64()

///*
// * SIMD Optimized memcpy's are graciously borrowed from DirectFB.
// */
//
//#  define SSE_MMREG_SIZE 16
//#  define MIN_LEN 0x40  /* 64-byte blocks */
//
//void * memcpy_sse2( void *p_dst, CONST void *p_src, size_t len )
//{
//	void *retval = p_dst;
//	CONST char	*p_src = (CONST char *) p_src;
//	char		*p_dst = (char * ) p_dst;
//	size_t i;
//
//	_mm_prefetch( (char* ) p_src, _MM_HINT_NTA);
//	_mm_prefetch( (char* ) p_src + 64, _MM_HINT_NTA);
//	_mm_prefetch( (char* ) p_src + 128, _MM_HINT_NTA);
//	_mm_prefetch( (char* ) p_src + 192, _MM_HINT_NTA);
//	_mm_prefetch( (char* ) p_src + 256, _MM_HINT_NTA);
//
//	if( len >= MIN_LEN )
//		{
//		register unsigned long int delta;
//		delta = ( ( unsigned long int ) p_dst ) & ( SSE_MMREG_SIZE - 1 );
//		if( delta )
//			{
//			delta = SSE_MMREG_SIZE - delta;
//			len -= delta;
//			small_memcpy( p_dst, p_src, delta );
//			}
//		i = len >> 6;
//		len &= 63;
//
//		if( ( ( unsigned long ) p_src ) & 15 )
//			{
//			for( ; i > 0; --i )
//				{
//				__m128 xmm0, xmm1, xmm2, xmm3;
//				_mm_prefetch( ( char* ) p_src + 320, _MM_HINT_NTA );
//				xmm0 = _mm_loadu_ps( ( float* ) p_src );
//				xmm1 = _mm_loadu_ps( ( float* ) p_src + 4 );
//				xmm2 = _mm_loadu_ps( ( float* ) p_src + 8 );
//				xmm3 = _mm_loadu_ps( ( float* ) p_src + 12 );
//				_mm_stream_ps( ( float* ) p_dst, xmm0 );
//				_mm_stream_ps( ( float* ) p_dst + 4, xmm1 );
//				_mm_stream_ps( ( float* ) p_dst + 8, xmm2 );
//				_mm_stream_ps( ( float* ) p_dst + 12, xmm3 );
//				p_src += 64;
//				p_dst += 64;
//				}
//			}
//		else
//			{
//			for( ; i > 0; --i )
//				{
//				__m128 xmm0, xmm1, xmm2, xmm3;
//				_mm_prefetch( ( char* ) p_src + 320, _MM_HINT_NTA );
//				xmm0 = _mm_load_ps( ( float* ) p_src );
//				xmm1 = _mm_load_ps( ( float* ) p_src + 4 );
//				xmm2 = _mm_load_ps( ( float* ) p_src + 8 );
//				xmm3 = _mm_load_ps( ( float* ) p_src + 12 );
//				_mm_stream_ps( ( float* ) p_dst, xmm0 );
//				_mm_stream_ps( ( float* ) p_dst + 4, xmm1 );
//				_mm_stream_ps( ( float* ) p_dst + 8, xmm2 );
//				_mm_stream_ps( ( float* ) p_dst + 12, xmm3 );
//				p_src += 64;
//				p_dst += 64;
//				}
//			}	
//		}
//	_mm_empty();
//	if( len )
//		memcpy( p_dst, p_src, len );
//
//	return retval;
//}

///*
// * SIMD Optimized memcpy's are graciously borrowed from DirectFB.
// */
//
//#  define SSE_MMREG_SIZE_2 16
//#  define MIN_LEN_2 0x80  /* 64-byte blocks */
//
//void * memcpy_sse3( void *p_dst, CONST void *p_src, size_t len )
//{
//	void *retval = p_dst;
//	CONST char	*p_src = (CONST char *) p_src;
//	char		*p_dst = (char * ) p_dst;
//	size_t i;
//
//	_mm_prefetch( (char* ) p_src, _MM_HINT_NTA);
//	_mm_prefetch( (char* ) p_src + 64, _MM_HINT_NTA);
//	_mm_prefetch( (char* ) p_src + 128, _MM_HINT_NTA);
//	_mm_prefetch( (char* ) p_src + 192, _MM_HINT_NTA);
//	_mm_prefetch( (char* ) p_src + 256, _MM_HINT_NTA);
//
//	if( len >= MIN_LEN_2 )
//		{
//		register unsigned long int delta;
//		delta = ( ( unsigned long int ) p_dst ) & ( SSE_MMREG_SIZE - 1 );
//		if( delta )
//			{
//			delta = SSE_MMREG_SIZE - delta;
//			len -= delta;
//			small_memcpy( p_dst, p_src, delta );
//			}
//		i = len >> 7;
//		len &= 127;
//
//		if( ( ( unsigned long ) p_src ) & 15 )
//			{
//			for( ; i > 0; --i )
//				{
//				__m128 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
//				_mm_prefetch( ( char* ) p_src + 320, _MM_HINT_NTA );
//				xmm0 = _mm_loadu_ps( ( float* ) p_src );
//				xmm1 = _mm_loadu_ps( ( float* ) p_src + 4 );
//				xmm2 = _mm_loadu_ps( ( float* ) p_src + 8 );
//				xmm3 = _mm_loadu_ps( ( float* ) p_src + 12 );
//				xmm4 = _mm_loadu_ps( ( float* ) p_src + 16);
//				xmm5 = _mm_loadu_ps( ( float* ) p_src + 20 );
//				xmm6 = _mm_loadu_ps( ( float* ) p_src + 24 );
//				xmm7 = _mm_loadu_ps( ( float* ) p_src + 28 );
//				_mm_stream_ps( ( float* ) p_dst, xmm0 );
//				_mm_stream_ps( ( float* ) p_dst + 4, xmm1 );
//				_mm_stream_ps( ( float* ) p_dst + 8, xmm2 );
//				_mm_stream_ps( ( float* ) p_dst + 12, xmm3 );
//				_mm_stream_ps( ( float* ) p_dst + 16, xmm4 );
//				_mm_stream_ps( ( float* ) p_dst + 20, xmm5 );
//				_mm_stream_ps( ( float* ) p_dst + 24, xmm6 );
//				_mm_stream_ps( ( float* ) p_dst + 28, xmm7 );
//				p_src += 128;
//				p_dst += 128;
//				}
//			}
//		else
//			{
//			for( ; i > 0; --i )
//				{
//				__m128 xmm0, xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
//				_mm_prefetch( ( char* ) p_src + 320, _MM_HINT_NTA );
//				xmm0 = _mm_load_ps( ( float* ) p_src );
//				xmm1 = _mm_load_ps( ( float* ) p_src + 4 );
//				xmm2 = _mm_load_ps( ( float* ) p_src + 8 );
//				xmm3 = _mm_load_ps( ( float* ) p_src + 12 );
//				xmm4 = _mm_load_ps( ( float* ) p_src + 16 );
//				xmm5 = _mm_load_ps( ( float* ) p_src + 20 );
//				xmm6 = _mm_load_ps( ( float* ) p_src + 24 );
//				xmm7 = _mm_load_ps( ( float* ) p_src + 28 );
//				_mm_stream_ps( ( float* ) p_dst, xmm0 );
//				_mm_stream_ps( ( float* ) p_dst + 4, xmm1 );
//				_mm_stream_ps( ( float* ) p_dst + 8, xmm2 );
//				_mm_stream_ps( ( float* ) p_dst + 12, xmm3 );
//				_mm_stream_ps( ( float* ) p_dst + 16, xmm4 );
//				_mm_stream_ps( ( float* ) p_dst + 20, xmm5 );
//				_mm_stream_ps( ( float* ) p_dst + 24, xmm6 );
//				_mm_stream_ps( ( float* ) p_dst + 28, xmm7 );
//				p_src += 128;
//				p_dst += 128;
//				}
//			}	
//		}
//	_mm_empty();
//	if( len )
//		small_memcpy( p_dst, p_src, len );
//
//	return retval;
//}

namespace {
	CONSTEXPR INT32 SSE_MMREG_BYTE_NB	= 16;
	CONSTEXPR INT32 SSE_LOOP_BYTE_NB	= SSE_MMREG_BYTE_NB * 8;		// 128-byte blocks
#if AAA_USE_AVX()
	CONSTEXPR INT32 AVX_MMREG_BYTE_NB	= 32;
	CONSTEXPR INT32	AVX_LOOP_BYTE_NB	= AVX_MMREG_BYTE_NB * 8;	// 256-byte blocks
#endif
}

FINLINE	void	memset_sse( void* CONST dst, int CONST val, size_t size ) NOEXCEPT
{
	CHAR*	p_dst	= (CHAR*)dst;

	if( size >= SSE_MMREG_BYTE_NB )
	{
		size_t	delta;
		if( ( delta = ((size_t)p_dst) & ( SSE_MMREG_BYTE_NB - 1 ) ) ) /* Align destination */
		{
			delta = SSE_MMREG_BYTE_NB - delta;
			size -= delta;
			memset( p_dst, val, delta );
			p_dst += delta;
		}

		//Get number of even blocks and save reminder
		size_t	i = size >> 7; // Divide by 128
		size &= SSE_LOOP_BYTE_NB-1; // Remainder of the size

		float	w;
		std::memset( &w, val, 4 );

		__m128 CONST xmm = _mm_set_ps1( w );
		for( ; i; --i, p_dst += 128 )
		{
			_mm_stream_ps( (float*)p_dst     , xmm );
			_mm_stream_ps( (float*)p_dst + 4 , xmm );
			_mm_stream_ps( (float*)p_dst + 8 , xmm );
			_mm_stream_ps( (float*)p_dst + 12, xmm );
			_mm_stream_ps( (float*)p_dst + 16, xmm );
			_mm_stream_ps( (float*)p_dst + 20, xmm );
			_mm_stream_ps( (float*)p_dst + 24, xmm );
			_mm_stream_ps( (float*)p_dst + 28, xmm );
		}

		while( size >= SSE_MMREG_BYTE_NB )
		{
			_mm_stream_ps( (float*)(p_dst)	, xmm );
			p_dst += SSE_MMREG_BYTE_NB;
			size -= SSE_MMREG_BYTE_NB;
		}
		_mm_sfence();
	}

	if( size )
		std::memset( p_dst, val, size );
}

FINLINE	void	memclear_sse( void* CONST dst, size_t size ) NOEXCEPT
{
	CHAR*	p_dst	= (CHAR*)dst;

	if( size >= SSE_MMREG_BYTE_NB )
	{
		size_t	delta;
		if( ( delta = ((size_t)p_dst) & ( SSE_MMREG_BYTE_NB - 1 ) ) ) // Align destination
		{
			delta = SSE_MMREG_BYTE_NB - delta;
			size -= delta;
			memset( p_dst, 0, delta );
			p_dst += delta;
		}

		//Get number of even blocks and save reminder
		size_t i = size >> 7;	// divide by 128
		size &= SSE_LOOP_BYTE_NB-1;	

		__m128 CONST xmm = _mm_setzero_ps();
		for( ; i; --i, p_dst += 128 )
		{
			_mm_stream_ps( (float*)p_dst     , xmm );
			_mm_stream_ps( (float*)p_dst + 4 , xmm );
			_mm_stream_ps( (float*)p_dst + 8 , xmm );
			_mm_stream_ps( (float*)p_dst + 12, xmm );
			_mm_stream_ps( (float*)p_dst + 16, xmm );
			_mm_stream_ps( (float*)p_dst + 20, xmm );
			_mm_stream_ps( (float*)p_dst + 24, xmm );
			_mm_stream_ps( (float*)p_dst + 28, xmm );
		}

		while( size >= SSE_MMREG_BYTE_NB )
		{
			_mm_stream_ps( (float*)(p_dst)	, xmm );
			p_dst += SSE_MMREG_BYTE_NB;
			size -= SSE_MMREG_BYTE_NB;
		}
		_mm_sfence();
	}

	if( size )
		std::memset( p_dst, 0, size );
}

#if AAA_USE_AVX()
FINLINE void memclear_avx( void* CONST dst, size_t size ) NOEXCEPT
{
	CHAR* p_dst = (CHAR*)dst;

	if( size >= AVX_MMREG_BYTE_NB )
	{
		if( INT32 delta = ((size_t)p_dst) & (AVX_MMREG_BYTE_NB - 1) ) // Align destination 
		{
			delta = AVX_MMREG_BYTE_NB - delta;
			size -= delta;
			std::memset( p_dst, 0, delta );
			p_dst += delta;
		}

		// Get number of even blocks and save remainder
		size_t i = size >> 8; // Divide by 256
		size &= AVX_LOOP_BYTE_NB-1; // Remainder of the size

		// Use a zeroed 256-bit register
		__m256i ymm = _mm256_setzero_si256();
		for( ; i; --i, p_dst += 256 )
		{
			_mm256_stream_si256( (__m256i*)(p_dst)      , ymm);
			_mm256_stream_si256( (__m256i*)(p_dst + 32) , ymm);
			_mm256_stream_si256( (__m256i*)(p_dst + 64) , ymm);
			_mm256_stream_si256( (__m256i*)(p_dst + 96) , ymm);
			_mm256_stream_si256( (__m256i*)(p_dst + 128), ymm);
			_mm256_stream_si256( (__m256i*)(p_dst + 160), ymm);
			_mm256_stream_si256( (__m256i*)(p_dst + 192), ymm);
			_mm256_stream_si256( (__m256i*)(p_dst + 224), ymm);
		}

		while( size >= AVX_MMREG_BYTE_NB )
		{
			_mm256_stream_si256( (__m256i*)(p_dst)		, ymm);
			p_dst += AVX_MMREG_BYTE_NB;
			size -= AVX_MMREG_BYTE_NB;
		}
		_mm_sfence();
	}

	if( size )
		std::memset( p_dst, 0, size );
}
FINLINE void memset_avx( void* CONST dst, int CONST val, size_t size ) NOEXCEPT
{
	CHAR* p_dst = (CHAR*)dst;

	if( size >= AVX_MMREG_BYTE_NB )
	{
		if( INT32 delta = ((size_t)p_dst) & (AVX_MMREG_BYTE_NB - 1) ) // Align destination 
		{
			delta = AVX_MMREG_BYTE_NB - delta;
			size -= delta;
			std::memset( p_dst, val, delta );
			p_dst += delta;
		}

		// Get number of even blocks and save remainder
		size_t i = size >> 8; // Divide by 256
		size &= AVX_LOOP_BYTE_NB-1; // Remainder of the size

		// Prepare 256-bit register with the value
		__m256i CONST ymm = _mm256_set1_epi8((char)val);
		for( ; i; --i, p_dst += 256 )
		{
			_mm256_stream_si256( (__m256i*)(p_dst)		, ymm);
			_mm256_stream_si256( (__m256i*)(p_dst + 32)	, ymm);
			_mm256_stream_si256( (__m256i*)(p_dst + 64)	, ymm);
			_mm256_stream_si256( (__m256i*)(p_dst + 96)	, ymm);
			_mm256_stream_si256( (__m256i*)(p_dst + 128), ymm);
			_mm256_stream_si256( (__m256i*)(p_dst + 160), ymm);
			_mm256_stream_si256( (__m256i*)(p_dst + 192), ymm);
			_mm256_stream_si256( (__m256i*)(p_dst + 224), ymm);
		}

		while( size >= AVX_MMREG_BYTE_NB )
		{
			_mm256_stream_si256( (__m256i*)(p_dst)		, ymm);
			p_dst += AVX_MMREG_BYTE_NB;
			size -= AVX_MMREG_BYTE_NB;
		}
		_mm_sfence();
	}

	if( size )
		std::memset( p_dst, val, size );
}
#endif	//#if AAA_USE_AVX()

bool		mem::b_memset_use_avx_asked	= true;
bool		mem::b_memset_use_avx		= false;


void	mem::memclear( void* CONST dst, size_t CONST byte_nb ) NOEXCEPT
{
#if AAA_USE_AVX()
	if( mem::b_memset_use_avx )
		memclear_avx( dst, byte_nb );
	else
#endif
		memclear_sse( dst, byte_nb );

}

void	mem::memset( void* CONST dst, int CONST val, size_t CONST byte_nb ) NOEXCEPT
{
#if AAA_USE_AVX()
	if( mem::b_memset_use_avx )
		memset_avx( dst, val, byte_nb );
	else
#endif
		memset_sse( dst, val, byte_nb );
}

//#  define SSE_MMREG_SIZE 16
//#  define MMX_MMREG_SIZE 8
//#  define SSE_MMREG_SIZE_MASK 15
//
//#  define MMX1_MIN_LEN 0x800	//	2K blocks
//#  define SSE_MIN_LEN 0x40		//	64-byte blocks
//
//FINLINE	void* memcpy_sse( void *dest, CONST void *p_src, size_t nbytes )
//{
//	void *ret = dest;
//
//	_asm 
//	{
//		mov esi, p_src 
//		mov edi, dest
//		mov edx, nbytes
//
//		// Also affects movsd/movsb
//		prefetchnta 0[esi]
//		prefetchnta 32[esi]
//		prefetchnta 64[esi]
//		prefetchnta 96[esi]
//
//		//Only do SSE if enough memory to copy
//		cmp edx, SSE_MMREG_SIZE
//		jle copyTail
//
//		//Align destination to MMREG_SIZE boundary
//		mov ecx, edi
//		and ecx, SSE_MMREG_SIZE_MASK
//		jz  alignedDest
//
//		mov ebx, SSE_MMREG_SIZE
//		sub ebx, ecx
//		sub edx, ebx
//		mov ecx, ebx
//		shr ecx, 2
//		rep movsd
//		mov ecx, ebx
//		and ecx, 3
//		rep movsb
//
//		alignedDest:
//
//		//Save reminer bytes in edx and number of 64-byte blocks in ebx
//		mov ebx, edx
//		and edx, 63
//		shr ebx, 6
//
//		mov ecx, esi
//		and ecx, SSE_MMREG_SIZE_MASK
//		jz  alignedSource
//
//		//Source not aligned, use movups
//		misalignedSource:
//
//		prefetchnta 128[esi]
//		prefetchnta 160[esi]
//		movups xmm0, XMMWORD PTR [esi]
//		movups xmm1, XMMWORD PTR [esi+16]
//		movups xmm2, XMMWORD PTR [esi+32]
//		movups xmm3, XMMWORD PTR [esi+48]
//		movntps XMMWORD PTR [edi], xmm0
//		movntps XMMWORD PTR [edi+16], xmm1
//		movntps XMMWORD PTR [edi+32], xmm2
//		movntps XMMWORD PTR [edi+48], xmm3
//		add esi, 64
//		add edi, 64
//		dec ebx
//		jne misalignedSource
//
//		//Source aligned, use movaps
//		alignedSource:
//
//		prefetchnta 128[esi]
//		prefetchnta 160[esi]
//		movaps xmm0, XMMWORD PTR [esi]
//		movaps xmm1, XMMWORD PTR [esi+16]
//		movaps xmm2, XMMWORD PTR [esi+32]
//		movaps xmm3, XMMWORD PTR [esi+48]
//		movntps XMMWORD PTR [edi], xmm0
//		movntps XMMWORD PTR [edi+16], xmm1
//		movntps XMMWORD PTR [edi+32], xmm2
//		movntps XMMWORD PTR [edi+48], xmm3
//		add esi, 64
//		add edi, 64
//		dec ebx
//		jne alignedSource
//
//		sfence
//		emms
//
//
//		// Copy remainder
//		copyTail:
//		
//		mov esi, edx
//		mov edx, ecx
//		shr ecx, 2
//		rep movsd
//		mov ecx, edx
//		and ecx, 3
//		rep movsb
//
//	}
//
//	return ret;
//}

#if !AAA_WIN64()
FINLINE	void*	memcpy_amd( void *dest, CONST void *p_src, size_t n )
{
#define	TINY_BLOCK_COPY	64 // upper limit for movsd type copy
	// The smallest copy uses the X86 "movsd" instruction, in an optimized
	// form which is an "unrolled loop".
#define	IN_CACHE_COPY	64 * 1024 // upper limit for movq/movq copy w/SW prefetch
	// Next is a copy that uses the MMX registers to copy 8 bytes at a time,
	// also using the "unrolled loop" optimization. This code uses
	// the software prefetch instruction to get the data into the cache.
#define	UNCACHED_COPY	197 * 1024 // upper limit for movq/movntq w/SW prefetch
	// For larger blocks, which will spill beyond the cache, it's faster to
	// use the Streaming Store instruction MOVNTQ. This write instruction
	// bypasses the cache and writes straight to main memory. This code also
	// uses the software prefetch instruction to pre-read the data.
	// USE 64 * 1024 FOR THIS VALUE IF YOU'RE ALWAYS FILLING A "CLEAN CACHE"
#define	BLOCK_PREFETCH_COPY	infinity // no limit for movq/movntq w/block prefetch
#define	CACHEBLOCK	80h // # of 64-byte blocks (cache lines) for block prefetch
	// For the largest size blocks, a special technique called Block Prefetch
	// can be used to accelerate the read operations. Block Prefetch reads
	// one address per cache line, for a series of cache lines, in a short loop.
	// This is faster than using software prefetch. The technique is great for
	// getting maximum read bandwidth, especially in DDR memory systems.

	__asm
	{
		mov ecx, [n]	// number of bytes to copy
		mov edi, [dest] // destination
		mov esi, [p_src]	// source
		mov ebx, ecx	// keep a copy of count

			cld
			cmp ecx, TINY_BLOCK_COPY
			jb $memcpy_ic_3		// tiny? skip mmx copy
			cmp ecx, 32*1024	// don't align between 32k-64k because
			jbe $memcpy_do_align	// it appears to be slower
			cmp ecx, 64*1024
			jbe $memcpy_align_done

$memcpy_do_align:
		mov ecx, 8		// a trick that's faster than rep movsb...
			sub ecx, edi	// align destination to qword
			and ecx, 111b	// get the low bits
			sub ebx, ecx	// update copy count
			neg ecx			// set up to jump into the array
			add ecx, offset $memcpy_align_done
			jmp ecx			// jump to array of movsb's

			align 4
			movsb
			movsb
			movsb
			movsb
			movsb
			movsb
			movsb
			movsb

$memcpy_align_done:		// destination is dword aligned
		mov ecx, ebx		// number of bytes left to copy
			shr ecx, 6			// get 64-byte block count
			jz $memcpy_ic_2		// finish the last few bytes
			cmp ecx, IN_CACHE_COPY/64	// too big 4 cache? use uncached copy
			jae $memcpy_uc_test

			// This is small block copy that uses the MMX registers to copy 8 bytes
			// at a time. It uses the "unrolled loop" optimization, and also uses
			// the software prefetch instruction to get the data into the cache.
			align 16

$memcpy_ic_1:			// 64-byte block copies, in-cache copy
		prefetchnta [esi + (200*64/34+192)]		// start reading ahead
		movq mm0, [esi+0]			// read 64 bits
		movq mm1, [esi+8]
		movq [edi+0], mm0			// write 64 bits
			movq [edi+8], mm1			// note: the normal movq writes the
			movq mm2, [esi+16]			// data to cache; a cache line will be
		movq mm3, [esi+24]			// allocated as needed, to store the data
		movq [edi+16], mm2
			movq [edi+24], mm3
			movq mm0, [esi+32]
		movq mm1, [esi+40]
		movq [edi+32], mm0
			movq [edi+40], mm1
			movq mm2, [esi+48]
		movq mm3, [esi+56]
		movq [edi+48], mm2
			movq [edi+56], mm3
			add esi, 64					// update source pointer
			add edi, 64					// update destination pointer
			dec ecx						// count down
			jnz $memcpy_ic_1			// last 64-byte block?

$memcpy_ic_2:
		mov ecx, ebx				// has valid low 6 bits of the byte count

$memcpy_ic_3:
		shr ecx, 2					// dword count
			and ecx, 1111b				// only look at the "remainder" bits
			neg ecx						// set up to jump into the array
			add ecx, offset $memcpy_last_few
			jmp ecx						// jump to array of movsd's

$memcpy_uc_test:
		cmp ecx, UNCACHED_COPY/64	// big enough? use block prefetch copy
			jae $memcpy_bp_1

$memcpy_64_test:
		or ecx, ecx					// tail end of block prefetch will jump here
			jz $memcpy_ic_2				// no more 64-byte blocks left

			// For larger blocks, which will spill beyond the cache, it's faster to
			// use the Streaming Store instruction MOVNTQ. This write instruction
			// bypasses the cache and writes straight to main memory. This code also
			// uses the software prefetch instruction to pre-read the data.
			align 16

$memcpy_uc_1: ; 64-byte blocks, uncached copy
			  prefetchnta [esi + (200*64/34+192)]		// .. start reading ahead
		movq mm0,[esi+0]			// read 64 bits
		add edi,64					// update destination pointer
			movq mm1,[esi+8]
		add esi,64					// update source pointer
			movq mm2,[esi-48]
		movntq [edi-64], mm0		// write 64 bits, bypassing the cache
			movq mm0,[esi-40]			// note: movntq also prevents the CPU
		movntq [edi-56], mm1		// from READING the destination address
			movq mm1,[esi-32]			// into the cache, only to be over-written
		movntq [edi-48], mm2		// so that also helps performance
			movq mm2,[esi-24]
		movntq [edi-40], mm0
			movq mm0,[esi-16]
		movntq [edi-32], mm1
			movq mm1,[esi-8]
		movntq [edi-24], mm2
			movntq [edi-16], mm0
			dec ecx
			movntq [edi-8], mm1
			jnz $memcpy_uc_1			// last 64-byte block?
			jmp $memcpy_ic_2			// almost dont

			// For the largest size blocks, a special technique called Block Prefetch
			// can be used to accelerate the read operations. Block Prefetch reads
			// one address per cache line, for a series of cache lines, in a short loop.
			// This is faster than using software prefetch. The technique is great for
			// getting maximum read bandwidth, especially in DDR memory systems.
$memcpy_bp_1:		// large blocks, block prefetch copy
		cmp ecx, CACHEBLOCK			// big enough to run another prefetch loop?
			jl $memcpy_64_test			// no, back to regular uncached copy
			mov eax, CACHEBLOCK / 2		// block prefetch loop, unrolled 2X
			add esi, CACHEBLOCK * 64	// move to the top of the block

			align 16

$memcpy_bp_2:
		mov edx, [esi-64]			// grab one address per cache line
		mov edx, [esi-128]			// grab one address per cache line
		sub esi, 128				// go reverse order
			dec eax						// count down the cache lines
			jnz $memcpy_bp_2			// keep grabbing more lines into cache
			mov eax, CACHEBLOCK			// now that it's in cache, do the copy

			align 16

$memcpy_bp_3:
		movq mm0, [esi ]			// read 64 bits
		movq mm1, [esi+ 8]
		movq mm2, [esi+16]
		movq mm3, [esi+24]
		movq mm4, [esi+32]
		movq mm5, [esi+40]
		movq mm6, [esi+48]
		movq mm7, [esi+56]
		add esi, 64					// update source pointer
			movntq [edi ], mm0			// write 64 bits, bypassing cache
			movntq [edi+ 8], mm1		// note: movntq also prevents the CPU
			movntq [edi+16], mm2		// from READING the destination address
			movntq [edi+24], mm3		// into the cache, only to be over-written,
			movntq [edi+32], mm4		// so that also helps performance
			movntq [edi+40], mm5
			movntq [edi+48], mm6
			movntq [edi+56], mm7
			add edi, 64					// update dest pointer
			dec eax						// count down
			jnz $memcpy_bp_3			// keep copying
			sub ecx, CACHEBLOCK			// update the 64-byte block count
			jmp $memcpy_bp_1			// keep processing blocks

			// The smallest copy uses the X86 "movsd" instruction, in an optimized
			// form which is an "unrolled loop". Then it handles the last few bytes.
			align 4
			movsd
			movsd		// perform last 1-15 dword copies
			movsd
			movsd
			movsd
			movsd
			movsd
			movsd
			movsd
			movsd		// perform last 1-7 dword copies
			movsd
			movsd
			movsd
			movsd
			movsd
			movsd
$memcpy_last_few:			// dword aligned from before movsd's
		mov ecx, ebx			// has valid low 2 bits of the byte count
			and ecx, 11b			// the last few cows must come home
			jz $memcpy_final		// no more, let's leave
			rep movsb				// the last 1, 2, or 3 bytes
$memcpy_final:
		emms					// clean up the MMX state
			sfence					// flush the write buffer
			mov eax, [dest]			// ret value = destination pointer
	}

}
#endif	//#if !AAA_WIN64()

FINLINE void memcpy_sse2( void* dst_in, void CONST * src_in, UINT32 size ) NOEXCEPT
{
	SPY_PUSH_RANGE( "memcpy_sse2", spy::MEM_LOW );
	//if( !__sse2_available || size < 16 || ((UINT32)p_src & 15) != ((UINT32)p_dst & 15) )
	//{
	//	memcpy( p_dst, Src, size );
	//}
	//else
	{
		CHAR CONST *	p_src	= (CHAR CONST *)src_in;
		CHAR*			p_dst	= (CHAR *)      dst_in;
		// Contract : p_src and p_dst must be 16, byte aligned. Uses _mm_load_si128 (aligned load) and _mm_stream_si128
		// (aligned, non, temporal store), both fault on misaligned addresses. Verified in debug only.
		#if AAA_DEBUG()
			if( !mem::IS_ALIGNED_16( p_src ) )	DBG_PRINT_STRING( "%s() src not 16, byte aligned : %p", __FUNCTION__, p_src );
			if( !mem::IS_ALIGNED_16( p_dst ) )	DBG_PRINT_STRING( "%s() dst not 16, byte aligned : %p", __FUNCTION__, p_dst );
		#endif

		//UINT32 Count = (16 - ((UINT32)p_src & 15)) & 15;
		//for ( UINT32 Index = Count; Index > 0; --Index )
		//{
		//	*p_dst++ = *p_src++;
		//}
		//size -= Count;

		
		for( UINT32 nb = size >> 7; nb > 0; --nb )
		{
			// SSE2 prefetch
			_mm_prefetch( p_src + 256,      _MM_HINT_NTA );
			_mm_prefetch( p_src + 256 + 64, _MM_HINT_NTA );

			// move data from p_src to registers
			// 8 x 128 bit (16 bytes each)
			// Increment source pointer by 16 bytes each
			// for a total of 128 bytes per cycle
			__m128i CONST Reg0 = _mm_load_si128( (__m128i *)(p_src)       );
			__m128i CONST Reg1 = _mm_load_si128( (__m128i *)(p_src + 16)  );
			__m128i CONST Reg2 = _mm_load_si128( (__m128i *)(p_src + 32)  );
			__m128i CONST Reg3 = _mm_load_si128( (__m128i *)(p_src + 48)  );
			__m128i CONST Reg4 = _mm_load_si128( (__m128i *)(p_src + 64)  );
			__m128i CONST Reg5 = _mm_load_si128( (__m128i *)(p_src + 80)  );
			__m128i CONST Reg6 = _mm_load_si128( (__m128i *)(p_src + 96)  );
			__m128i CONST Reg7 = _mm_load_si128( (__m128i *)(p_src + 112) );

			// move data from registers to p_dst
			_mm_stream_si128( (__m128i *)(p_dst),       Reg0 );
			_mm_stream_si128( (__m128i *)(p_dst + 16),  Reg1 );
			_mm_stream_si128( (__m128i *)(p_dst + 32),  Reg2 );
			_mm_stream_si128( (__m128i *)(p_dst + 48),  Reg3 );
			_mm_stream_si128( (__m128i *)(p_dst + 64),  Reg4 );
			_mm_stream_si128( (__m128i *)(p_dst + 80),  Reg5 );
			_mm_stream_si128( (__m128i *)(p_dst + 96),  Reg6 );
			_mm_stream_si128( (__m128i *)(p_dst + 112), Reg7 );

			// other version ?
			//Reg0 = _mm_stream_load_si128( (__m128i *)(p_src) );
			//Reg1 = _mm_stream_load_si128( (__m128i *)(p_src + 16) );
			//Reg2 = _mm_stream_load_si128( (__m128i *)(p_src + 32) );
			//Reg3 = _mm_stream_load_si128( (__m128i *)(p_src + 48) );
			//Reg4 = _mm_stream_load_si128( (__m128i *)(p_src + 64) );
			//Reg5 = _mm_stream_load_si128( (__m128i *)(p_src + 80) );
			//Reg6 = _mm_stream_load_si128( (__m128i *)(p_src + 96) );
			//Reg7 = _mm_stream_load_si128( (__m128i *)(p_src + 112) );

			p_src += 128;
			p_dst += 128;
		}

		{
			__m128i Reg1, Reg2, Reg3, Reg4, Reg5, Reg6, Reg7;
			UINT32 CONST size_16 = size & 0x70;
			UINT32 CONST nb = size_16 >> 4;
			switch( nb )
			{
			case 7: Reg7 = _mm_load_si128( (__m128i *)(p_src + 96) );
			case 6:	Reg6 = _mm_load_si128( (__m128i *)(p_src + 80) );
			case 5:	Reg5 = _mm_load_si128( (__m128i *)(p_src + 64) );
			case 4:	Reg4 = _mm_load_si128( (__m128i *)(p_src + 48) );
			case 3:	Reg3 = _mm_load_si128( (__m128i *)(p_src + 32) );
			case 2:	Reg2 = _mm_load_si128( (__m128i *)(p_src + 16) );
			case 1:	Reg1 = _mm_load_si128( (__m128i *)(p_src) );
			}
			switch( nb )
			{
			case 7: _mm_stream_si128( (__m128i *)(p_dst + 96), Reg7 );
			case 6: _mm_stream_si128( (__m128i *)(p_dst + 80), Reg6 );
			case 5: _mm_stream_si128( (__m128i *)(p_dst + 64), Reg5 );
			case 4: _mm_stream_si128( (__m128i *)(p_dst + 48), Reg4 );
			case 3: _mm_stream_si128( (__m128i *)(p_dst + 32), Reg3 );
			case 2: _mm_stream_si128( (__m128i *)(p_dst + 16), Reg2 );
			case 1: _mm_stream_si128( (__m128i *)(p_dst),      Reg1 );
			}

			p_src += size_16;
			p_dst += size_16;
		}

		for( UINT32 nb = size & 0xf; nb > 0; --nb )
		{
			*p_dst++ = *p_src++;
		}
	}
	_mm_sfence();
	SPY_POP_RANGE();
}

FINLINE void memcpy_sse41_stream( void* dst, void CONST * src, size_t size ) NOEXCEPT
{
	SPY_PUSH_RANGE( "memcpy_sse41_stream", spy::MEM_LOW );
	//if( !__sse2_available || size < 16 || ((UINT32)p_src & 15) != ((UINT32)p_dst & 15) )
	//{
	//	memcpy( p_dst, Src, size );
	//}
	//else
	{
		UINT8 CONST *	p_src = (UINT8 CONST *)src;
		UINT8*			p_dst = (UINT8 *)dst;
		// Contract : p_src and p_dst must be 16, byte aligned (_mm_stream_load_si128 + _mm_stream_si128 both fault on
		// misaligned addresses). Verified in debug only.
		#if AAA_DEBUG()
			if( !mem::IS_ALIGNED_16( p_src ) )	DBG_PRINT_STRING( "%s() src not 16, byte aligned : %p", __FUNCTION__, p_src );
			if( !mem::IS_ALIGNED_16( p_dst ) )	DBG_PRINT_STRING( "%s() dst not 16, byte aligned : %p", __FUNCTION__, p_dst );
		#endif

		//UINT32 Count = (16 - ((UINT32)p_src & 15)) & 15;
		//for ( UINT32 Index = Count; Index > 0; --Index )
		//{
		//	*p_dst++ = *p_src++;
		//}
		//size -= Count;

		for( size_t Index = size >> 7; Index > 0; --Index )
		{
			//_mm_prefetch( p_src + 256, _MM_HINT_NTA );
			//_mm_prefetch( p_src + 256 + 64, _MM_HINT_NTA );
			//Reg0 = _mm_load_si128( (__m128i *)(p_src) );
			//Reg1 = _mm_load_si128( (__m128i *)(p_src + 16) );
			//Reg2 = _mm_load_si128( (__m128i *)(p_src + 32) );
			//Reg3 = _mm_load_si128( (__m128i *)(p_src + 48) );
			//Reg4 = _mm_load_si128( (__m128i *)(p_src + 64) );
			//Reg5 = _mm_load_si128( (__m128i *)(p_src + 80) );
			//Reg6 = _mm_load_si128( (__m128i *)(p_src + 96) );
			//Reg7 = _mm_load_si128( (__m128i *)(p_src + 112) );
			// SSE41 instructions
			__m128i CONST xmm0 = _mm_stream_load_si128( (__m128i CONST *)(p_src) );
			__m128i CONST xmm1 = _mm_stream_load_si128( (__m128i CONST *)(p_src + 16) );
			__m128i CONST xmm2 = _mm_stream_load_si128( (__m128i CONST *)(p_src + 32) );
			__m128i CONST xmm3 = _mm_stream_load_si128( (__m128i CONST *)(p_src + 48) );
			__m128i CONST xmm4 = _mm_stream_load_si128( (__m128i CONST *)(p_src + 64) );
			__m128i CONST xmm5 = _mm_stream_load_si128( (__m128i CONST *)(p_src + 80) );
			__m128i CONST xmm6 = _mm_stream_load_si128( (__m128i CONST *)(p_src + 96) );
			__m128i CONST xmm7 = _mm_stream_load_si128( (__m128i CONST *)(p_src + 112) );
			_mm_stream_si128( (__m128i *)(p_dst),		xmm0 );
			_mm_stream_si128( (__m128i *)(p_dst + 16),	xmm1 );
			_mm_stream_si128( (__m128i *)(p_dst + 32),	xmm2 );
			_mm_stream_si128( (__m128i *)(p_dst + 48),	xmm3 );
			_mm_stream_si128( (__m128i *)(p_dst + 64),	xmm4 );
			_mm_stream_si128( (__m128i *)(p_dst + 80),	xmm5 );
			_mm_stream_si128( (__m128i *)(p_dst + 96),	xmm6 );
			_mm_stream_si128( (__m128i *)(p_dst + 112), xmm7 );
			p_src += 128;
			p_dst += 128;
		}
		if( size & 0x7f )
		{
			__m128i xmm1, xmm2, xmm3, xmm4, xmm5, xmm6, xmm7;
			UINT32 CONST SizeOn16 = size & 0x70;
			switch( SizeOn16 >> 4 )
			{
			case 7: xmm7 = _mm_stream_load_si128( (__m128i CONST *)(p_src + 96) );
			case 6:	xmm6 = _mm_stream_load_si128( (__m128i CONST *)(p_src + 80) );
			case 5:	xmm5 = _mm_stream_load_si128( (__m128i CONST *)(p_src + 64) );
			case 4:	xmm4 = _mm_stream_load_si128( (__m128i CONST *)(p_src + 48) );
			case 3:	xmm3 = _mm_stream_load_si128( (__m128i CONST *)(p_src + 32) );
			case 2:	xmm2 = _mm_stream_load_si128( (__m128i CONST *)(p_src + 16) );
			case 1:	xmm1 = _mm_stream_load_si128( (__m128i CONST *)(p_src) );
			}
			switch( SizeOn16 >> 4 )
			{
			case 7: _mm_stream_si128( (__m128i *)(p_dst + 96), xmm7 );
			case 6: _mm_stream_si128( (__m128i *)(p_dst + 80), xmm6 );
			case 5: _mm_stream_si128( (__m128i *)(p_dst + 64), xmm5 );
			case 4: _mm_stream_si128( (__m128i *)(p_dst + 48), xmm4 );
			case 3: _mm_stream_si128( (__m128i *)(p_dst + 32), xmm3 );
			case 2: _mm_stream_si128( (__m128i *)(p_dst + 16), xmm2 );
			case 1: _mm_stream_si128( (__m128i *)(p_dst), xmm1 );
			}
			p_src += SizeOn16;
			p_dst += SizeOn16;
			for( UINT32 Index = size & 0x0f; Index > 0; --Index )
			{
				*p_dst++ = *p_src++;
			}
		}
	}
	_mm_sfence();
	SPY_POP_RANGE();
}

#if AAA_USE_AVX2()
FINLINE void memcpy_avx2_stream_16( UINT8 * CONST dst, UINT8 CONST * CONST src, size_t size ) NOEXCEPT
{
	SPY_PUSH_RANGE( "memcpy_avx2_stream_16", spy::MEM_LOW );

	auto p_src = ( __m256i CONST *)src;
	auto p_dst = ( __m256i *)dst;
 
	// One __m256i contains 32 byte, load 16 * 32 bytes per loop = 512 bytes
	__m256i ymm0, ymm1, ymm2,  ymm3,  ymm4,  ymm5,  ymm6,  ymm7;
	__m256i ymm8, ymm9, ymm10, ymm11, ymm12, ymm13, ymm14, ymm15;
	for( size_t nb = size >> 9; nb > 0; --nb )
	{
		// AVX2 instructions
		ymm0  = _mm256_loadu_si256( ( p_src +  0 ) );
		ymm1  = _mm256_loadu_si256( ( p_src +  1 ) );
		ymm2  = _mm256_loadu_si256( ( p_src +  2 ) );
		ymm3  = _mm256_loadu_si256( ( p_src +  3 ) );
		ymm4  = _mm256_loadu_si256( ( p_src +  4 ) );
		ymm5  = _mm256_loadu_si256( ( p_src +  5 ) );
		ymm6  = _mm256_loadu_si256( ( p_src +  6 ) );
		ymm7  = _mm256_loadu_si256( ( p_src +  7 ) );
		ymm8  = _mm256_loadu_si256( ( p_src +  8 ) );
		ymm9  = _mm256_loadu_si256( ( p_src +  9 ) );
		ymm10 = _mm256_loadu_si256( ( p_src + 10 ) );
		ymm11 = _mm256_loadu_si256( ( p_src + 11 ) );
		ymm12 = _mm256_loadu_si256( ( p_src + 12 ) );
		ymm13 = _mm256_loadu_si256( ( p_src + 13 ) );
		ymm14 = _mm256_loadu_si256( ( p_src + 14 ) );
		ymm15 = _mm256_loadu_si256( ( p_src + 15 ) );

		_mm256_storeu_si256( ( p_dst +  0 ), ymm0 );
		_mm256_storeu_si256( ( p_dst +  1 ), ymm1 );
		_mm256_storeu_si256( ( p_dst +  2 ), ymm2 );
		_mm256_storeu_si256( ( p_dst +  3 ), ymm3 );
		_mm256_storeu_si256( ( p_dst +  4 ), ymm4 );
		_mm256_storeu_si256( ( p_dst +  5 ), ymm5 );
		_mm256_storeu_si256( ( p_dst +  6 ), ymm6 );
		_mm256_storeu_si256( ( p_dst +  7 ), ymm7 );
		_mm256_storeu_si256( ( p_dst +  8 ), ymm8 );
		_mm256_storeu_si256( ( p_dst +  9 ), ymm9 );
		_mm256_storeu_si256( ( p_dst + 10 ), ymm10 );
		_mm256_storeu_si256( ( p_dst + 11 ), ymm11 );
		_mm256_storeu_si256( ( p_dst + 12 ), ymm12 );
		_mm256_storeu_si256( ( p_dst + 13 ), ymm13 );
		_mm256_storeu_si256( ( p_dst + 14 ), ymm14 );
		_mm256_storeu_si256( ( p_dst + 15 ), ymm15 );

		p_src += 16;
		p_dst += 16;
	}

	INT32 CONST blk_32_bytes = (size & 0x1E0) >> 5;	// 480 -> 15 * 32
	switch( blk_32_bytes )	// divide by 32
	{
		case 15:	ymm15 = _mm256_loadu_si256( ( p_src + 14 ) );
		case 14:	ymm14 = _mm256_loadu_si256( ( p_src + 13 ) );
		case 13:	ymm13 = _mm256_loadu_si256( ( p_src + 12 ) );
		case 12:	ymm12 = _mm256_loadu_si256( ( p_src + 11 ) );
		case 11:	ymm11 = _mm256_loadu_si256( ( p_src + 10 ) );
		case 10:	ymm10 = _mm256_loadu_si256( ( p_src +  9 ) );
		case 9:		ymm9  = _mm256_loadu_si256( ( p_src +  8 ) );
		case 8:		ymm8  = _mm256_loadu_si256( ( p_src +  7 ) );
		case 7:		ymm7  = _mm256_loadu_si256( ( p_src +  6 ) );
		case 6:		ymm6  = _mm256_loadu_si256( ( p_src +  5 ) );
		case 5:		ymm5  = _mm256_loadu_si256( ( p_src +  4 ) );
		case 4:		ymm4  = _mm256_loadu_si256( ( p_src +  3 ) );
		case 3:		ymm3  = _mm256_loadu_si256( ( p_src +  2 ) );
		case 2:		ymm2  = _mm256_loadu_si256( ( p_src +  1 ) );
		case 1:		ymm1  = _mm256_loadu_si256( ( p_src +  0 ) );
	}
	switch( blk_32_bytes )	// divide by 32
	{
		case 15:	_mm256_storeu_si256( ( p_dst + 14 ), ymm15 );
		case 14:	_mm256_storeu_si256( ( p_dst + 13 ), ymm14 );
		case 13:	_mm256_storeu_si256( ( p_dst + 12 ), ymm13 );
		case 12:	_mm256_storeu_si256( ( p_dst + 11 ), ymm12 );
		case 11:	_mm256_storeu_si256( ( p_dst + 10 ), ymm11 );
		case 10:	_mm256_storeu_si256( ( p_dst +  9 ), ymm10 );
		case 9:		_mm256_storeu_si256( ( p_dst +  8 ), ymm9  );
		case 8:		_mm256_storeu_si256( ( p_dst +  7 ), ymm8  );
		case 7:		_mm256_storeu_si256( ( p_dst +  6 ), ymm7  );
		case 6:		_mm256_storeu_si256( ( p_dst +  5 ), ymm6  );
		case 5:		_mm256_storeu_si256( ( p_dst +  4 ), ymm5  );
		case 4:		_mm256_storeu_si256( ( p_dst +  3 ), ymm4  );
		case 3:		_mm256_storeu_si256( ( p_dst +  2 ), ymm3  );
		case 2:		_mm256_storeu_si256( ( p_dst +  1 ), ymm2  );
		case 1:		_mm256_storeu_si256( ( p_dst +  0 ), ymm1  );
	}
	p_src += blk_32_bytes;
	p_dst += blk_32_bytes;
	// size & 31 -> used as bitfield, for bits 0..4
	size &= 31;
	if( size )
		std::memcpy( p_dst, p_src, size );

	SPY_POP_RANGE();
}

FINLINE void memcpy_avx2_stream_16_aligned( UINT8 * dst, UINT8 CONST * src, size_t size ) NOEXCEPT
{
	SPY_PUSH_RANGE( "memcpy_avx2_stream_16_aligned", spy::MEM_LOW );

	{
		INT32 nb = ((size_t)dst) & 31;
		if( nb )
		{
			nb = 32 - nb;
			__movsb( dst, src, nb );
			//std::memcpy( p_dst, p_src, nb );
			src += nb;
			dst += nb;
			size -= nb;
		}
	}

	auto p_src = ( __m256i CONST *)src;
	auto p_dst = ( __m256i *)dst;
 
	// One __m256i contains 32 byte, load 16 * 32 bytes per loop = 512 bytes
	__m256i ymm0, ymm1, ymm2,  ymm3,  ymm4,  ymm5,  ymm6,  ymm7;
	__m256i ymm8, ymm9, ymm10, ymm11, ymm12, ymm13, ymm14, ymm15;
	for( size_t nb = size >> 9; nb > 0; --nb )
	{
		// AVX2 instructions
		ymm0  = _mm256_stream_load_si256( ( p_src +  0 ) );
		ymm1  = _mm256_stream_load_si256( ( p_src +  1 ) );
		ymm2  = _mm256_stream_load_si256( ( p_src +  2 ) );
		ymm3  = _mm256_stream_load_si256( ( p_src +  3 ) );
		ymm4  = _mm256_stream_load_si256( ( p_src +  4 ) );
		ymm5  = _mm256_stream_load_si256( ( p_src +  5 ) );
		ymm6  = _mm256_stream_load_si256( ( p_src +  6 ) );
		ymm7  = _mm256_stream_load_si256( ( p_src +  7 ) );
		ymm8  = _mm256_stream_load_si256( ( p_src +  8 ) );
		ymm9  = _mm256_stream_load_si256( ( p_src +  9 ) );
		ymm10 = _mm256_stream_load_si256( ( p_src + 10 ) );
		ymm11 = _mm256_stream_load_si256( ( p_src + 11 ) );
		ymm12 = _mm256_stream_load_si256( ( p_src + 12 ) );
		ymm13 = _mm256_stream_load_si256( ( p_src + 13 ) );
		ymm14 = _mm256_stream_load_si256( ( p_src + 14 ) );
		ymm15 = _mm256_stream_load_si256( ( p_src + 15 ) );

		_mm256_stream_si256( ( p_dst +  0 ), ymm0 );
		_mm256_stream_si256( ( p_dst +  1 ), ymm1 );
		_mm256_stream_si256( ( p_dst +  2 ), ymm2 );
		_mm256_stream_si256( ( p_dst +  3 ), ymm3 );
		_mm256_stream_si256( ( p_dst +  4 ), ymm4 );
		_mm256_stream_si256( ( p_dst +  5 ), ymm5 );
		_mm256_stream_si256( ( p_dst +  6 ), ymm6 );
		_mm256_stream_si256( ( p_dst +  7 ), ymm7 );
		_mm256_stream_si256( ( p_dst +  8 ), ymm8 );
		_mm256_stream_si256( ( p_dst +  9 ), ymm9 );
		_mm256_stream_si256( ( p_dst + 10 ), ymm10 );
		_mm256_stream_si256( ( p_dst + 11 ), ymm11 );
		_mm256_stream_si256( ( p_dst + 12 ), ymm12 );
		_mm256_stream_si256( ( p_dst + 13 ), ymm13 );
		_mm256_stream_si256( ( p_dst + 14 ), ymm14 );
		_mm256_stream_si256( ( p_dst + 15 ), ymm15 );

		p_src += 16;
		p_dst += 16;
	}

	INT32 CONST blk_32_bytes = (size & 0x1E0) >> 5;	// 480 -> 15 * 32
	switch( blk_32_bytes )	// divide by 32
	{
		case 15:	ymm15 = _mm256_stream_load_si256( ( p_src + 14 ) );
		case 14:	ymm14 = _mm256_stream_load_si256( ( p_src + 13 ) );
		case 13:	ymm13 = _mm256_stream_load_si256( ( p_src + 12 ) );
		case 12:	ymm12 = _mm256_stream_load_si256( ( p_src + 11 ) );
		case 11:	ymm11 = _mm256_stream_load_si256( ( p_src + 10 ) );
		case 10:	ymm10 = _mm256_stream_load_si256( ( p_src +  9 ) );
		case 9:		ymm9  = _mm256_stream_load_si256( ( p_src +  8 ) );
		case 8:		ymm8  = _mm256_stream_load_si256( ( p_src +  7 ) );
		case 7:		ymm7  = _mm256_stream_load_si256( ( p_src +  6 ) );
		case 6:		ymm6  = _mm256_stream_load_si256( ( p_src +  5 ) );
		case 5:		ymm5  = _mm256_stream_load_si256( ( p_src +  4 ) );
		case 4:		ymm4  = _mm256_stream_load_si256( ( p_src +  3 ) );
		case 3:		ymm3  = _mm256_stream_load_si256( ( p_src +  2 ) );
		case 2:		ymm2  = _mm256_stream_load_si256( ( p_src +  1 ) );
		case 1:		ymm1  = _mm256_stream_load_si256( ( p_src +  0 ) );
	}
	switch( blk_32_bytes )	// divide by 32
	{
		case 15:	_mm256_stream_si256( ( p_dst + 14 ), ymm15 );
		case 14:	_mm256_stream_si256( ( p_dst + 13 ), ymm14 );
		case 13:	_mm256_stream_si256( ( p_dst + 12 ), ymm13 );
		case 12:	_mm256_stream_si256( ( p_dst + 11 ), ymm12 );
		case 11:	_mm256_stream_si256( ( p_dst + 10 ), ymm11 );
		case 10:	_mm256_stream_si256( ( p_dst +  9 ), ymm10 );
		case 9:		_mm256_stream_si256( ( p_dst +  8 ), ymm9  );
		case 8:		_mm256_stream_si256( ( p_dst +  7 ), ymm8  );
		case 7:		_mm256_stream_si256( ( p_dst +  6 ), ymm7  );
		case 6:		_mm256_stream_si256( ( p_dst +  5 ), ymm6  );
		case 5:		_mm256_stream_si256( ( p_dst +  4 ), ymm5  );
		case 4:		_mm256_stream_si256( ( p_dst +  3 ), ymm4  );
		case 3:		_mm256_stream_si256( ( p_dst +  2 ), ymm3  );
		case 2:		_mm256_stream_si256( ( p_dst +  1 ), ymm2  );
		case 1:		_mm256_stream_si256( ( p_dst +  0 ), ymm1  );
	}
	p_src += blk_32_bytes;
	p_dst += blk_32_bytes;
	// size & 31 -> used as bitfield, for bits 0..4
	size &= 31;
	_mm_sfence();
	if( size )
		std::memcpy( p_dst, p_src, size );

	SPY_POP_RANGE();
}
#endif	//AAA_USE_AVX2()

//bool	gb_memcpy_use_amd	= true;

bool	mem::b_cpy_use_movsb_asked		= true;
bool	mem::b_cpy_use_sse41_asked		= true;
bool	mem::b_cpy_use_avx2_asked		= true;
bool	mem::b_cpy_use_movsb			= false;
bool	mem::b_cpy_use_sse41			= false;
bool	mem::b_cpy_use_avx2				= false;
bool	mem::b_cpy_use_cache_L2_size_ui	= false;
bool	mem::b_cpy_use_cache_L3_size_ui	= false;
size_t	mem::cpy_use_cache_L2_size		= 0;
size_t	mem::cpy_use_cache_L3_size		= 0;
bool	mem::b_cpy_parallel				= false;
UINT32	mem::cpy_parallel_thread_nb		= 0;
UINT32	mem::cpy_parallel_threshold_kb	= 1024;



void	mem::memcpy_monothread( void* CONST dst, void CONST * CONST src, size_t CONST nb ) NOEXCEPT
{
	if( nb < cpy_use_cache_L2_size )
	{
	//	SPY_PUSH_RANGE( "std::memcpy@L2", spy::MEM_LOW );
			std::memcpy( dst, src, nb );
	//	SPY_POP_RANGE();
		return;
	}
	if( nb < cpy_use_cache_L3_size )
	{
#if AAA_USE_AVX2()
		if( b_cpy_use_avx2 )
		{
			if( GET_ALIGNED_32(dst) == GET_ALIGNED_32(src) )
				memcpy_avx2_stream_16_aligned(	(UINT8* CONST)dst,	(UINT8 CONST * CONST)src,	nb );
			else
				memcpy_avx2_stream_16(			(UINT8* CONST)dst,	(UINT8 CONST * CONST)src,	nb );			
			return;
		}
#endif //AAA_USE_AVX2()
		//		if( mem::use_sse2 )		memcpy_sse2( p_dst, p_src, nb );
		if( mem::b_cpy_use_sse41 )
		{
			if( IS_ALIGNED_16( dst ) && IS_ALIGNED_16( src ) )
			{
				memcpy_sse41_stream( dst, src, nb );
				return;
			}
		}
	}
	//SPY_PUSH_RANGE( "std::memcpy@def", spy::MEM_LOW );
	std::memcpy( dst, src, nb );
	//SPY_POP_RANGE();
/*
	if( !IS_ALIGNED_16( p_dst ) || !IS_ALIGNED_16( p_src ) )
	{
		SPY_PUSH_RANGE( "__movsb", spy::MEM_LOW );
		//	DBG_PRINT_STRING( "MEMCPY p_dst not align on 16 Bytes");
			__movsb( (UINT8*)p_dst, (UINT8 CONST *)p_src, nb );
		//memcpy( p_dst, p_src, nb );
		SPY_POP_RANGE();
	}
	else
	{
//		if( mem::use_sse2 )		memcpy_sse2( p_dst, p_src, nb );
		if( mem::use_sse2 && c_cpu::cur->_b_SSE4_1 )
			memcpy_sse41_stream( p_dst, p_src, nb );
		else
		{
			SPY_PUSH_RANGE( "__movsb", spy::MEM_LOW );
				__movsb( (UINT8*)p_dst, (UINT8*)p_src, nb );
			SPY_POP_RANGE();
		}
		//else							memcpy( p_dst, p_src, nb );

//#if AAA_WIN64()
//		memcpy( p_dst, p_src, nb );
//#else
//		//memcpy_sse2( p_dst, p_src, nb );
//		//// __movsb( (UINT8*)p_dst, (UINT8*)p_src, nb );
//		if( gb_memcpy_use_sse2 )		memcpy_sse2( p_dst, p_src, nb );
//	//	else							memcpy( p_dst, p_src, nb );
//	//	else						memcpySSE( p_dst, p_src, nb );
//#endif
	}
*/
}


void mem::memcpy_parallel( void* CONST dst, void CONST * CONST src, size_t CONST nb, INT32 CONST thread_nb, C_PCHAR_C signature ) NOEXCEPT
{
	INT32 core_nb = INT32( aaa::parallel::get_thread_nb_def() );
	INT32 threads_nb = thread_nb == 0 ? core_nb : thread_nb;

	std::size_t CONST size_by_thread = nb / threads_nb;
	PARALLEL_LIB::parallel_for( 0, threads_nb, [&]( INT32 CONST i ) NOEXCEPT
	{
		auto offset = i * size_by_thread;
		auto p_src = ( std::uint8_t CONST * )	( src ) + offset;
		auto p_dst = ( std::uint8_t* )			( dst ) + offset;
		std::size_t size;
		if( i == ( threads_nb - 1 ) )
			size = nb - offset;
		else
			size = size_by_thread;
		memcpy_monothread( p_dst, p_src, size );
		//std::memcpy( p_dst, p_src, sz );
	} );
}

void	mem::memcpy( void* CONST dst, void CONST * CONST src, size_t CONST nb, C_PCHAR_C signature ) NOEXCEPT
{
	if( !dst || !src )
	{
		debug_break( "%s() can't deal with a Null src or dst", __FUNCTION__ );
		return;
	}

	// performance seems to be better with __movsb than with memcopy
	//__movsb( (UINT8*)p_dst, (CONST UINT8*)p_src, nb );
	if( nb <= 2048 )
	{
		if( b_cpy_use_movsb )
		{
			SPY_PUSH_RANGE( "__movsb", spy::MEM_LOW );
				__movsb( (UINT8*)dst, (UINT8 CONST * CONST)src, nb );
			SPY_POP_RANGE();
		}
		else
		{
			SPY_PUSH_RANGE( "memcpy small", spy::MEM_LOW );
				std::memcpy( dst, src, nb );
			SPY_POP_RANGE();
		}
	}
	else
	{
		if( b_cpy_parallel && (nb >> 10) > cpy_parallel_threshold_kb )
		{
			SPY_PUSH_RANGE( "memcpy_parallel", spy::MEM_LOW );
				memcpy_parallel( dst, src, nb, cpy_parallel_thread_nb, signature );
			SPY_POP_RANGE();
		}
		else
		{
			SPY_PUSH_RANGE( "memcpy_monothread", spy::MEM_LOW );
				memcpy_monothread( dst, src, nb );
			SPY_POP_RANGE();
		}
	}
}



