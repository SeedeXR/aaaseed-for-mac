#include "math/v_base.h"


INT32	c_vector_lib::s_vector_lib = c_vector_lib::VECTOR_LIB_NO;

C_PCHAR_C	c_vector_lib::vector_lib_str[ VECTOR_LIB_MAX_NB ] = 
{
	"None",
	"XNA Math",
};


/*
#if	!AAA_REAL_IS_DOUBLE()
FINLINE	void	cpy_v3r( REAL* dst, DOUBLE CONST * src )
{
	*dst		=	(REAL)	*src;
#if AAA_V_DOIT_WITH_INC()
	*++dst		=	(REAL)	*++src;
	*++dst		=	(REAL)	*++src;
#else
	*(dst+1)	=	(REAL)	*(src+1);
	*(dst+2)	=	(REAL)	*(src+2);
#endif
}
#endif


FINLINE	void	cpy_v2r( REAL* dst, REAL CONST * src )
{
	*dst = *src;
	*++dst = *++src;
	*dst = *src;
	*(dst+1) = *(src+1);
}
*/

CONSTEXPR	void	cpy_from_with_stride_v3r( REAL* dst, REAL CONST * src, INT32 stride, INT32 nb )
{
	stride -= 2;
	for( ; nb > 0 ; --nb )
	{
		*dst	= *src;
		*++dst	= *++src;
		*++dst	= *++src;
		++dst;
		src += stride;
	}
}

CONSTEXPR	void	cpy_to_with_stride_v3r( REAL* dst, INT32 stride, REAL CONST * src,  INT32 nb )
{
	stride -= 2;
	for( ; nb  >0 ; --nb )
	{
		*dst	= *src;
		*++dst	= *++src;
		*++dst	= *++src;
		dst	+= stride;
		++src;
	}
}

FINLINE		REAL* cpy_alternate_v3( REAL* dst, REAL CONST * a, REAL CONST * b, INT32 nb )
{
	for( ; nb>0; --nb )	
	{  
		cpy_v3( dst, a );
		dst += 3;
		a += 3;
		cpy_v3( dst, b );
		dst += 3;
		b += 3;
	}
	return dst;
}

FINLINE	REAL* cpy_alternate_v2( REAL* dst, REAL CONST * a, REAL CONST * b, INT32 nb )
{
	for( ; nb>0; --nb )	
	{  
		cpy_v2( dst, a );
		dst += 2;
		a += 2;
		cpy_v2( dst, b );
		dst += 2;
		b += 2;
	}
	return dst;
}


